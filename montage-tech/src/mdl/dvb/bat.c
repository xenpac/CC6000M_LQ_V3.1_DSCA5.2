/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"
#include "string.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "bat.h"
#include "service.h"
#include "err_check_def.h"

/*!
  head len
  */
#define SDT_DESC_HEAD_LEN   (0x5)
/*!
  head len
  */
#define BAT_SEC_HEAD_LEN    (12)

void parse_bat(handle_t handle, dvb_section_t *p_sec)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_svc = (service_t *)handle;
  os_msg_t msg = {0};
  u8 *p_buf = p_sec->p_buffer;
  u16 bouquet_id = 0;
  u16 offset = 0;
  s16 bouquet_len = 0;
  s16 section_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  u8 dl_loop_len = 0;
  u8 i = 0;
  bat_t bat = {0};
  u8 tmp_len = 0;

  //u8 seg_num,total_seg_num;
  //u16 table_ext_id;
  //u8  last_section_num;
  linkage_update_desc_t update_info = {0};
  
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  if(p_buf[0] != p_sec->table_id)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;
  }

  if ((p_buf[5] & 0x01) == 0)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;
  }

  bouquet_id = MAKE_WORD2(p_buf[3], p_buf[4]);
  
  /* Section length */
  section_len = MAKE_WORD2((p_buf[1] & 0x0f), p_buf[2]);
  section_len += 3;

  memset(&bat, 0, sizeof(bat_t));
  
  bat.bouquet_id = bouquet_id;
  bat.version_num = (p_buf[5] & 0x3E) >> 1;
  bat.sec_number = p_buf[6];
  bat.last_sec_number = p_buf[7];

  bouquet_len = MAKE_WORD2(p_buf[8] & 0x0F, p_buf[9]);
  offset = 10;
  while(bouquet_len > 0)
  {
    tmp_len = p_buf[offset + 1];  
    if(p_buf[offset] == DVB_DESC_LINKAGE)
    {
  
  
      bat.ts_id = update_info.ts_id = MAKE_WORD2(p_buf[offset + 2], 
                                                            p_buf[offset + 3]);
      bat.network_id = update_info.network_id = MAKE_WORD2(p_buf[offset + 4], 
                                                                            p_buf[offset + 5]);
      bat.service_id = update_info.service_id = MAKE_WORD2(p_buf[offset + 6], 
                                                                            p_buf[offset + 7]);
      if(p_buf[offset + 8] == 0xA0)  // linkage type 0xA0
      {
         update_info.operator_num = p_buf[offset + 9];
         dl_loop_len = p_buf[offset + 10];
         for(i = 0; i < dl_loop_len;)
         {
            update_info.manufacture_id = p_buf[offset + 11 + i];
            update_info.hardware_id = MAKE_WORD2(p_buf[offset + 12 + i], p_buf[offset + 13 + i]);
            update_info.model_id = p_buf[offset + 14 + i];
            update_info.software_ver = MAKE_WORD2(p_buf[offset + 15], p_buf[offset + 16 + i]);

            update_info.force_flag = p_buf[offset + 18 + i] & 0x03;
            update_info.start_STB_id = make32(&p_buf[offset + 19 + i]);
            update_info.end_STB_id = make32(&p_buf[offset + 23 + i]);
            i += 16;  
        
                     }
      }
      else if(p_buf[offset + 8] == 0x80)
      {
        // TODO: add linkage type of 0x80 parse codes here
          //get service info fdt pid and table_id
      //seg_num = p_buf[offset + 9]>>5;
      //total_seg_num = p_buf[offset + 12]>>5;
      bat.fdt_table_id = p_buf[offset + 11];
      if(DVB_TABLE_ID_FDT == bat.fdt_table_id)
      {
       bat.fdt_pid = MAKE_WORD2(p_buf[offset + 9]&0x1F, p_buf[offset + 10 ]);
       bat.fdt_ver_num = p_buf[offset + 12]&0x1F;
       //get table_ext_id and section num info!
       //for(i=0;i<=total_seg_num;i++)
       //{
       // table_ext_id = MAKE_WORD2(p_buf[offset+13+(3*i)],p_buf[offset+14+(3*i)]);
             // last_section_num = p_buf[offset+15+(3*i)];
       //}
      }
      
      }
    }
    // descriptor name
    else if(p_buf[offset] == 0x47)
    {
      
      //OS_PRINTF("bat descriptor name: %s \n", &p_buf[offset + 2]);
    }
    else if(DVB_DESC_LOGICAL_CHANNEL == p_buf[offset])
    {
      logical_channel_t *p_log_ch = bat.log_ch_info + bat.log_ch_num;
      u8 *p_log_des = p_buf + offset + 2;  //skip tag and len
      u8 des_len = tmp_len;
      
      while(des_len)
      {
        if(bat.log_ch_num >= DVB_MAX_SDT_SVC_NUM) //check input
        {
          break;
        }
        p_log_ch->service_id = MAKE_WORD2(p_log_des[0] & 0x0F, p_log_des[1]);
        p_log_ch->logical_channel_id = 
          (MAKE_WORD2(p_log_des[2] & 0x0F, p_log_des[3]) & 0x3FFF);
        
        p_log_ch++;
        bat.log_ch_num++;
        p_log_des += 4;
        if(des_len < 4) //for check error
        {
          des_len = 0;
        }
        else
        {
          des_len -= 4;
        }
      }
    }
    offset += (tmp_len + 2);  
    bouquet_len -= (tmp_len + 2);  
  }

  // TODO ts loop len while
  ts_loop_len = MAKE_WORD2(p_buf[offset] & 0xF, p_buf[offset + 1]);
  while(ts_loop_len > 0)
  {
    offset += 4;
    desc_len = MAKE_WORD2(p_buf[offset] & 0xF, p_buf[offset + 1]);
    offset += 2;

    ts_loop_len -= (desc_len + 6);  
    while(desc_len > 0)
    {
      tmp_len = p_buf[offset + 1];
      // add any trans desc parse here
      offset += (tmp_len + 2);
      desc_len -= (tmp_len + 2);
    }
  }
  
  msg.content = DVB_BAT_FOUND;
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.para1 = (u32)(&bat);
  msg.para2 = sizeof(bat_t);
#ifdef __LINUX__
  msg.is_ext = 1;
#endif
  p_svc->notify(p_svc, &msg);  
    
  return;
}

void request_bat_section(dvb_section_t *p_sec, u32 table_id, u32 para)
{
  u16 bouquet_id = para & 0xFFFF;
  u8 sec_number = (para & 0xFF0000) >> 16;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  
  p_sec->pid      = DVB_BAT_PID;
  p_sec->table_id = table_id;
  p_sec->timeout = (u32)4000;
  p_sec->direct_data = FALSE;
  p_sec->crc_enable = 1;
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,sizeof(u8) * MAX_FILTER_MASK_BYTES);
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_code[0] = p_sec->table_id;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = bouquet_id >> 8;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = bouquet_id & 0xFF;
  p_sec->filter_mask[2] = 0xFF;
  p_sec->filter_code[4] = (u8)sec_number;
  p_sec->filter_mask[4] = 0xFF;  
  p_sec->filter_mask_size = 5;
#else
  p_sec->filter_code[3] = bouquet_id >> 8;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = bouquet_id & 0xFF;
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_code[6] = (u8)sec_number;
  p_sec->filter_mask[6] = 0xFF;  
  p_sec->filter_mask_size = 7;
#endif
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

void request_bat_multi_mode(dvb_section_t *p_sec, u32 table_id, u32 para2)
{
  filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t *)para2;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  
  //Set PID information
  p_sec->pid      = DVB_BAT_PID;
  p_sec->table_id = table_id;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = table_id;
  p_sec->filter_mask[0] = 0xFF;
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_mask_size = 1;
  p_sec->timeout = (u32)BAT_TIMEOUT;

  //Attach buffer list
  p_sec->p_ext_buf_list = p_ext_buf_list;
  p_sec->crc_enable = 1;
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

