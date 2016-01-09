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
#include "fdt.h"
#include "service.h"


void parse_abs_fdt(handle_t handle, dvb_section_t *p_sec)
{

}
void request_abs_fdt_multi_section(dvb_section_t *p_sec, u32 table_id, u32 para2)
{
  abs_fdt_req_context_t *p_req_context = (abs_fdt_req_context_t *)para2;  
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  
  //Set PID information
  p_sec->pid      = p_req_context->fdt_pid;
  p_sec->table_id = table_id;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = table_id;
  p_sec->filter_mask[0] = 0xFF;
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_mask_size = 1;
  p_sec->timeout = (u32)FDT_TIMEOUT;

  //Attach buffer list
  p_sec->p_ext_buf_list = p_req_context->p_fdt_extern_buf;
  p_sec->crc_enable = 1;
  p_sec->direct_data = TRUE;
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}


