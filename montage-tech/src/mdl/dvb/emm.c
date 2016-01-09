/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "string.h"
#include "mtos_printk.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"

#include "emm.h"

void parse_emm(handle_t handle, dvb_section_t *p_sec)
{
  os_msg_t msg = {0};
  service_t *p_svc = (service_t *)handle;
 // OS_PRINTF("parse_emm\n");
  //memcpy(emm.buf, p_sec->p_buffer, MAX_SECTION_LENGTH);

  //Get section length
  //emm.len = SYS_GET_LOW_HALF_BYTE(p_sec->p_buffer[0]) << 8;

  msg.content = DVB_EMM_FOUND;
  msg.para1   = (u32)p_sec->p_buffer;
  msg.para2   = SYS_GET_LOW_HALF_BYTE(p_sec->p_buffer[1]) << 8 | p_sec->p_buffer[2];
  msg.context = p_sec->r_context;
  p_svc->notify(p_svc, &msg);
}

//Request mode is periodic 
void request_emm(dvb_section_t *p_sec, u32 para1, u32 para2)
{  
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  /*!
    para1 and para2 should be used and one of them should be table id
    */

  p_sec->pid = para1;
  p_sec->timeout  = para2;
  
  /*memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);  

  p_sec->filter_mode    = PTI_TYPE_SECTION; 
  p_sec->filter_code[0] = p_sec->table_id;
  p_sec->filter_mask[0] = 0xFF;
  p_sec->filter_mask_size = 7;*/
  p_sec->filter_mode    = FILTER_TYPE_SECTION;
  p_sec->filter_mask_size = MAX_FILTER_MASK_BYTES;

  p_sec->interval_time = 0;
  p_sec->crc_enable = 0;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

//Request mode is periodic 
void request_cas_emm(dvb_section_t *p_sec, u32 para1, u32 para2)
{  
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  /*!
    para1 and para2 should be used and one of them should be table id
    */

  p_sec->pid = para1;
  p_sec->timeout  = para2;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);  

//  p_sec->filter_mode    = PTI_TYPE_SECTION; 
  p_sec->filter_code[0] = (p_sec->table_id & 0xF0);
  p_sec->filter_mask[0] = 0xF0;
  p_sec->filter_mask_size = 1;
  p_sec->filter_mode    = FILTER_TYPE_SECTION;
  p_sec->filter_mask_size = MAX_FILTER_MASK_BYTES;

  p_sec->interval_time = 0;
  p_sec->crc_enable = 0;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}
