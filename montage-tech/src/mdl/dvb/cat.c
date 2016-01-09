/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

// driver headers

#include "class_factory.h"
// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "cat.h"

#include "service.h"

/*!
  Parse CAT table
  \param[in] handle DVB handle
  \param[in] p_sec DVB section information
  */
void parse_cat(handle_t handle, dvb_section_t *p_sec)
{
  os_msg_t msg = {0};
  cat_t ca = {{0}};
  service_t *p_svc = (service_t *)handle;
  
  memcpy(ca.buf, p_sec->p_buffer, MAX_SECTION_LENGTH);

  //Get section length
  ca.len = SYS_GET_LOW_HALF_BYTE(p_sec->p_buffer[0]) << 8;

  msg.content = DVB_CAT_FOUND;
  msg.para1   = (u32)&ca;
  msg.para2   = p_sec->sid;
  msg.context = p_sec->r_context;

  p_svc->notify(p_svc, &msg);
}

/*!
  Parse CAT table
  \param[in] handle DVB handle
  \param[in] p_sec DVB section information
  */
void parse_cas_cat(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = (service_t *)handle;
  u16 cnt = 0 ;
  s32 sec_length = 0;
  u8 *p_inbuf = NULL;
  os_msg_t msg = {0};
  cat_cas_t cat_cas = {0};
  s32 desc_lenth = 0;
  u16 ca_desc_cnt = 0;
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);

  //OS_PRINTF("parse cas cat\n");
  
  p_inbuf = p_sec->p_buffer ;
  if(p_inbuf[cnt]!= p_sec->table_id)
  {
    p_dvb->filter_data_error(p_svc, p_sec);
    return;    
  }  
  cat_cas.p_origion_data = p_sec->p_buffer;
  cat_cas.tab_id = p_inbuf[cnt];
  cnt ++;

  //Get section length
  sec_length = MAKE_WORD2(SYS_GET_LOW_HALF_BYTE(p_inbuf[cnt]), p_inbuf[cnt + 1]);
  cnt += 2;

  //Skip reserved
  cnt += 2;
  sec_length --;

  //Get version number
  cat_cas.ver_num = (p_inbuf[cnt] & 0x3e) >> 1; 
  cnt ++;
  sec_length --;

  //Get section number
  cat_cas.sec_num = p_inbuf[cnt];

  cnt ++;
  sec_length --;

  //Get last section number
  cat_cas.last_sec_num = p_inbuf[cnt];
  cnt ++;
  sec_length --;

  while(sec_length > 0)
  {
    desc_lenth = 0;
    switch(p_inbuf[cnt])
    {
      case CA_DESC_ID:
      {
        //skip tag
        cnt ++;
        sec_length --;  

        //Get section length
        desc_lenth = p_inbuf[cnt];
        cnt ++;
        sec_length --;  

        //Delete whole descriptor
        sec_length -= desc_lenth;
        if (ca_desc_cnt >= MAX_EMM_DESC_NUM)
        {
          cnt += 4;
          desc_lenth -= 4; 
        }
        else
        {
          cat_cas.ca_desc[ca_desc_cnt].ca_sys_id = MAKE_WORD2(p_inbuf[cnt], p_inbuf[cnt + 1]);
          cnt += 2;
          desc_lenth -= 2; 
          
          cat_cas.ca_desc[ca_desc_cnt].emm_pid  
                  = MAKE_WORD2(SYS_GET_LOW_FIVE_BITS(p_inbuf[cnt]), p_inbuf[cnt + 1]);
          cnt += 2;
          desc_lenth -= 2; 

          //memcpy(cat_cas.ca_desc[ca_desc_cnt].ca_prv_data, &p_inbuf[cnt], desc_lenth);

          ca_desc_cnt ++;
        }
      }
      break;
      default:
      {
        //skip tag
        cnt ++;
        sec_length --;  
        
        //Get section length
        desc_lenth = p_inbuf[cnt];
        cnt ++;
        sec_length --;  
        
        //Delete whole descriptor
        sec_length -= desc_lenth;
      }
      break;
   }
     
  }

  cat_cas.emm_cnt = ca_desc_cnt;
  
  msg.content = DVB_CAT_FOUND;
  msg.para1   = (u32)&cat_cas;
  msg.para2   = sizeof(cat_cas_t);
  msg.context = p_sec->r_context;
#ifdef __LINUX__
  msg.is_ext = 1;
#endif
  p_svc->notify(p_svc, &msg);
}

/*!
  Request CAT table
  \param[in] p_sec section table
  \param[in] table_id table id
  \param[in] para2 parameter
  */
void request_cat(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  //filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t*)para2;
  
 
  //Set PID information
  p_sec->sid = para1; // save para1 in sid
  p_sec->pid      = DVB_CAT_PID;
  p_sec->table_id = DVB_TABLE_ID_CAT;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = DVB_TABLE_ID_CAT;
  p_sec->filter_mask[0] = 0xFF;
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_mask_size = 1;
  p_sec->timeout = (u32)CAT_TIMEOUT;

  //Attach buffer list
  //p_sec->p_ext_buf_list = p_ext_buf_list;
  //p_sec->buf_mode = PTI_BUF_MODE_LINEAR_EXT;
  p_sec->crc_enable = 1;
  
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

