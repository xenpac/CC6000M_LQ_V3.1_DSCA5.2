/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_define.h"
#include "sys_types.h"
#include "lib_util.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "dvb_protocol.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "tdt.h"
#include "err_check_def.h"

#include "service.h"

void parse_tdt(handle_t handle, dvb_section_t *p_sec) 
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_svc = (service_t *)handle;

  u8 *p_data = NULL;
  utc_time_t utc_time = {0};
  //utc_time.value = 0;
  
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  p_data = p_sec->p_buffer;
  //Fix me!!!
  if(p_data[0]!= DVB_TABLE_ID_TDT)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;    
  } 

  if(date_trans(p_data + 3, &utc_time) == ERR_FAILURE)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;
  }
  
  {
    os_msg_t msg = {0};
    msg.content = DVB_TDT_FOUND;
    msg.para1 = (((u32)utc_time.year) << 16) | (((u32)utc_time.month) << 8) | utc_time.day;
    msg.para2 = (((u32)utc_time.hour) << 16) | (((u32)utc_time.minute) << 8) | utc_time.second;
    //memcpy(&(msg.para1), &utc_time, sizeof(utc_time_t));
    //OS_PRINTF("parse_tdt\n");
    //mdl_broadcast_msg(&msg);
    p_svc->notify(p_svc, &msg);
  } 
}

void request_tdt_section(dvb_section_t *p_sec, u32 table_id, u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);
  CHECK_FAIL_RET_VOID(table_id == DVB_TABLE_ID_TDT);
  
  p_sec->pid = DVB_TDT_PID;  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,sizeof(u8) * MAX_FILTER_MASK_BYTES);    
  
  p_sec->table_id = DVB_TABLE_ID_TDT;
  p_sec->filter_code[0] = DVB_TABLE_ID_TDT;
  p_sec->filter_mask[0] = 0xFF;
  
  p_sec->filter_mask_size = 1;
  p_sec->timeout = (u32)TDT_TIMEOUT; 
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  //p_sec->module_id = SYS_MODULE_EPG;
  p_sec->crc_enable = 0;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

