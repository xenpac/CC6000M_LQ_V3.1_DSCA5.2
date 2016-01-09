/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"

// util
#include "class_factory.h"

// os
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"

// driver
#include "drv_dev.h"

// middleware
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "dds.h"

/*!
  Default version number
  */
#define DEFAULT_VER_NUM (0X1F) 

static u32 dump_sw_des(dl_sw_des_t *p_sw_des, u8 *p_data)
{
  u32 tot_sw_len = 0;
  u8 *p_buf = p_data;
  u32 sw_desc_len = 0;
  
  if(p_buf[0] != 0xE2)
  {
    return 0xFFFF;
  }
  
  //Over table id
  p_buf ++;
  tot_sw_len ++;

  //Software section length
  sw_desc_len = p_buf[0];
  p_buf ++;
  tot_sw_len ++;

  //OS_PRINTF("DDS: dump_software_section len[%d]\n", sw_desc_len);
  //Software version
  p_sw_des->sw_version = MAKE_WORD(p_buf[1], p_buf[0]);
  //memcpy(&p_sw_des->sw_version, p_buf, sizeof(u16));
  p_buf += sizeof(u16);
  tot_sw_len += sizeof(u16);

  //Software size 
  //memcpy(&p_sw_des->sw_size, p_buf, sizeof(u32));
  p_sw_des->sw_size = MT_MAKE_DWORD(
    MAKE_WORD(p_buf[3], p_buf[2]),
    MAKE_WORD(p_buf[1], p_buf[0]));
  p_buf += sizeof(u32);
  tot_sw_len += sizeof(u32);

  //software CRC
  //memcpy(&p_sw_des->sw_crc, p_buf, sizeof(u32));
  p_sw_des->sw_crc = MT_MAKE_DWORD(
    MAKE_WORD(p_buf[3], p_buf[2]),
    MAKE_WORD(p_buf[1], p_buf[0]));
  p_buf += sizeof(u32);
  tot_sw_len += sizeof(u32);


  return tot_sw_len;
}

void parse_dds(handle_t handle, dvb_section_t *p_sec)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_svc = handle;
  u8 *p_buf   = p_sec->p_buffer;
  s32 sec_len = 0;
  dds_t dds = {0};
  u8 sign_flag = 0;
  s32 desc_len = 0;
  u32 sub_desc_len = 0;
  u32 sign_len = 0;
  os_msg_t msg = {0};


  //Table id
  if(p_buf[0] != DVB_TABLE_ID_DDS)
  {
    // Bad table id notify time out
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }
  
  dds.table_id = DVB_TABLE_ID_DDS;
  
  //Move to Next byte
  p_buf ++;

  //Get section length
  sec_len = MAKE_WORD(p_buf[1],SYS_GET_LOW_HALF_BYTE(p_buf[0])) - CRC_LEN;

  if(SYS_GET_BIT(p_buf[0], 7) == 0) //Section syntax indicator should be 1
  { 
    //Data error
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }


  if(sec_len > MAX_DDS_SEC_LEN)
  {
    // Bad section length
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }

  //Jump over section length
  p_buf += 2;
  
  //Jump over the first reserved bytes
  p_buf ++;
  sec_len --;
  
  //Get sub table id
  dds.sub_tab_id = SYS_GET_LOW_HALF_BYTE(p_buf[0]);
  //Jump over sub-table id
  p_buf ++;
  sec_len --;
  //Check current next indicator
  if((p_buf[0] & 0x1) == 0)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }
  //Check version number
  if((p_buf[0] & 0x3E) >> 1 != DEFAULT_VER_NUM)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }
  p_buf ++;
  sec_len --;
  
  //Section number
  dds.sec_num = p_buf[0];
  p_buf ++;
  sec_len --;
  
  //Last section number
  dds.last_sec_num = p_buf[0];
  p_buf ++;
  sec_len --;
  
  //Signature existing or not
  sign_flag = SYS_GET_HIGH_HALF_BYTE(p_buf[0]) >> 4;
  desc_len = MAKE_WORD(p_buf[1],SYS_GET_LOW_HALF_BYTE(p_buf[0]));

  p_buf += 2;
  sec_len -= 2;
  
  if(desc_len > sec_len)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }

  //Dump down load descriptor
  sub_desc_len = dump_sw_des(&dds.sw_des, p_buf);
  if(sub_desc_len >= sec_len)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }

  sec_len -= sub_desc_len;
  p_buf += sub_desc_len;

  if(sign_flag == 0)
  {
    //No data signature
    if(sec_len > MAX_DDS_SEC_LEN)
    {
      p_dvb_handle->filter_data_error(p_svc, p_sec);
      return;
    }
    else
    {
      dds.payload_len = sec_len;
      memcpy(dds.payload, p_buf, sec_len);
    }
  }
  else if(sign_flag == 0x3)
  {
    //Sign_flag is 3 when signature existing
    sign_len = MAKE_WORD(p_buf[1], SYS_GET_LOW_HALF_BYTE(p_buf[0]));
    p_buf += 2;
    sec_len -= 2;
    
    if(sign_len < sec_len)
    {
      dds.payload_len = sec_len - sign_len;
    }
    else
    {
      p_dvb_handle->filter_data_error(p_svc, p_sec);
      return;
    }
    
    if(sec_len < 0 || sec_len > MAX_SIGN_LEN)
    {
      p_dvb_handle->filter_data_error(p_svc, p_sec);
      return;
    }
    else 
    {
      //Get payload information
      memcpy(dds.payload, p_buf, dds.payload_len);
      p_buf += dds.payload_len;
      sec_len -= dds.payload_len;
      
      //Get signature information
      memcpy(dds.sign, p_buf, sign_len);
      dds.sign_len = sign_len;
      sec_len -= sign_len;

    }
  }
  else
  {
    //Invalid signature flag
  }
  //Send out message
  msg.content = DVB_DDS_TAB_FOUND;
  msg.para1   = (u32)&dds;
  msg.para2   = sizeof(dds_t);
  p_svc->notify(p_svc, &msg);

}

void request_dds(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  p_sec->pid       = (para2 >> 16) & (0xFFFF);
  p_sec->timeout  = (u32)DDS_TIMEOUT * (para2 & (0xFFFF));
  p_sec->table_id  = DVB_TABLE_ID_DDS;
  p_sec->p_ext_buf_list = (filter_ext_buf_t *)para1;
  p_sec->filter_mask_size = 1;
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  p_sec->filter_code[0] = DVB_TABLE_ID_DDS;
  p_sec->filter_mask[0] = 0xFF;
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->crc_enable = 1;

  //Allocate dvb section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

