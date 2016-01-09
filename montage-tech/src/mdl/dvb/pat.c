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
#include "service.h"
#include "dvb_protocol.h"
#include "mdl.h"
#include "dvb_svc.h"
#include "pat.h"
#include "service.h"
#include "err_check_def.h"

void parse_pat(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = (service_t *)handle;
  u8 *p_buf = p_sec->p_buffer;  
  u8 *p_ptr = NULL;
  u16 section_len = 0;
  u16 prog_no = 0;
  u16 pmt_pid = 0;
  s16 program_len = 0;

  u32 prog_num = 0;
  u32 ts_id = 0;
  os_msg_t msg = {0};
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);

  CHECK_FAIL_RET_VOID(NULL != p_buf);
  CHECK_FAIL_RET_VOID(p_dvb != NULL);

  //Fix me
  if(DVB_TABLE_ID_PAT != p_buf[0])
  {
    p_dvb->filter_data_error(p_svc, p_sec);
    return;
  }

  /* get section length */
  section_len = MAKE_WORD(p_buf[2], (p_buf[1] & 0x0f));
  section_len += 3;

  if((p_buf[5] & 0x01) == 0)
  {
    p_dvb->filter_data_error(p_svc, p_sec);
    return; /* this PAT is not usable */
  }

  ts_id = MAKE_WORD(p_buf[4], p_buf[3]);
  
  //ver_number = (p_buf[5] & 0x3E) >> 1;

  //p_pat->version = ver_number;

  program_len = (section_len - 8 - 4);

  prog_num = 0;
  p_ptr = &p_buf[8];

  while(program_len > 0)
  {
    /* get program number */
    prog_no = MAKE_WORD(p_ptr[1], p_ptr[0]);

    /* get program map pid */
    pmt_pid = MAKE_WORD(p_ptr[3], (p_ptr[2] & 0x1f));

    if(0 == prog_no)
    {
      /* get network table pid */
      //p_pat->nit_pid = pmt_pid;
    }
    else if(0xfffe != prog_no)
    {
      msg.content = DVB_PAT_PMT_INFO;
      msg.is_ext = 0;
      msg.para1 = pmt_pid;
      msg.para2 = prog_no;
      msg.context = p_sec->r_context;
      p_svc->notify(p_svc, &msg);

      /* get program number and map table pid*/
      //p_pat->progs[p_pat->prog_num].lbn = prog_no;
      //g_p_pat->progs[g_p_pat->prog_num].pmt_pid = pmt_pid;
      //g_p_pat->prog_num++;
      prog_num++;
      //request_pmt_section(pmt_pid, prog_no);
    }
    else if(0xFFFE == prog_no) // ABS spec, ref GD/J 027-2009 4.3.3.2
    {
      msg.content = DVB_ABS_UPGRADE_PMT_INFO;
      msg.is_ext = 0;
      msg.para1 = pmt_pid;
      msg.para2 = prog_no;
      p_svc->notify(p_svc, &msg);      
    }

    if(prog_num >= DVB_MAX_PAT_PROG_NUM - 12) //less than max , abort msgq full
    {
      break;
    }

    program_len -= 4;
    p_ptr += 4;
  }
 
  msg.content = DVB_PAT_FOUND;
  msg.is_ext = 0;
  msg.para1 = ts_id;
  msg.context = p_sec->r_context;
//  OS_PRINTF("@@@@parse pat context : 0x%x\n", msg.context);  
  p_svc->notify(p_svc, &msg);

  return;
}

void request_pat(dvb_section_t *p_sec, u32 table_id, u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);
  
  p_sec->pid = DVB_PAT_PID;
  p_sec->table_id = DVB_TABLE_ID_PAT;

//  OS_PRINTF("@@@@request pat context : 0x%x\n", p_sec->r_context);
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  
  p_sec->filter_code[0] = DVB_TABLE_ID_PAT;
  p_sec->filter_mask[0] = 0xFF;
  p_sec->timeout = (u32)PAT_TIMEOUT;
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->filter_mask_size = 1;
  p_sec->crc_enable = 1;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
  
}

void request_pat_dvbt(dvb_section_t *p_sec, u32 table_id, u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);
  
  p_sec->pid = DVB_PAT_PID;
  p_sec->table_id = DVB_TABLE_ID_PAT;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  
  p_sec->filter_code[0] = DVB_TABLE_ID_PAT;
  p_sec->filter_mask[0] = 0xFF;
  p_sec->timeout = 5000;
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->filter_mask_size = 1;
  p_sec->crc_enable = 1;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
  
  OS_PRINTF("@@@@request pat context : 0x%x,timeout=%d\n", p_sec->r_context, p_sec->timeout);
}

