/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mt_time.h"

#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "pti.h"

#include "mdl.h"
#include "service.h"
#include "class_factory.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "epg_util.h"
#include "eit.h"
//#include "sdt.h"
#include "epg_data4.h"
#include "ap_framework.h"
#include "ap_epg4.h"
#include "ap_epg_data_process4.h"

/*!
  Reuest EIT section in multi_section mode
  \param[in] table id to be requested
  */
void request_eit_section4(service_t *p_svc, u32 table_id,
  pti_ext_buf_t *p_ext_buf_list, u8 ts_in)
{
  dvb_request_t eit_req = {0};
  u32 ext_buf_addr = (u32)p_ext_buf_list;
  
  //Table id is just one flag
  eit_req.table_id  = table_id;
  eit_req.req_mode  = DATA_MULTI;
  
  eit_req.para1 = table_id;
  eit_req.para2 = ext_buf_addr;
  eit_req.ts_in = ts_in;
  p_svc->do_cmd(p_svc, DVB_REQUEST, (u32)&eit_req, sizeof(dvb_request_t));

}

/*!
  Free  EIT section
  */
void free_eit_section4(service_t *p_svc, u32 table_id,
  pti_ext_buf_t *p_ext_buf_list)
{
  dvb_request_t eit_req = {0};
  u32 ext_buf_addr = (u32)p_ext_buf_list;
  
  //Table id is just one flag
  eit_req.table_id  = table_id;
  eit_req.req_mode  = DATA_MULTI;
  
  eit_req.para1 = table_id;
  eit_req.para2 = ext_buf_addr;
  p_svc->do_cmd(p_svc, DVB_FREE, (u32)&eit_req, sizeof(dvb_request_t));
}

void epg_process_tab_status_reset4(void *p_data)
{
  //u8 tab_cnt  = 0;
  proc_para_t *p_proc_data = (proc_para_t *)p_data;

  p_proc_data->default_tab_map[0].table_id  = DVB_TABLE_ID_EIT_ACTUAL;
  p_proc_data->default_tab_map[0].p_buf_addr = NULL;
  p_proc_data->default_tab_map[0].tick_requsted = 0;
  p_proc_data->default_tab_map[0].tick_freed = 0;
  p_proc_data->default_tab_map[0].table_is_requested = FALSE;
#if 1
  p_proc_data->default_tab_map[1].table_id  = DVB_TABLE_ID_EIT_SCH_ACTUAL ;
  p_proc_data->default_tab_map[1].p_buf_addr = NULL;
  p_proc_data->default_tab_map[1].use_st  = FALSE;
  p_proc_data->default_tab_map[1].table_is_requested = FALSE;

  p_proc_data->default_tab_map[2].table_id  = DVB_TABLE_ID_EIT_SCH_ACTUAL_51 ;
  p_proc_data->default_tab_map[2].p_buf_addr = NULL;
  p_proc_data->default_tab_map[2].use_st  = FALSE;
  p_proc_data->default_tab_map[2].table_is_requested = FALSE;
#endif
//   Set_EPG_Focus_debug(class_get_handle_by_id(EPG_CLASS_ID));
}

void epg_process_on_stop4(void *p_data)
{
  proc_para_t *p_proc_data = (proc_para_t *)p_data;
  dvb_request_t eit_req = {0};
  //void *p_epg_db_handle = class_get_handle_by_id(DVB_CLASS_ID);
  
  if(p_proc_data->is_eit_mon == TRUE)
  {
    //Table id is just one flag
    eit_req.table_id  = DVB_TABLE_ID_EIT_ACTUAL;
    eit_req.req_mode  = DATA_PERIODIC;
    
    eit_req.para1 = DVB_TABLE_ID_EIT_ACTUAL;
    eit_req.para2 = 0;
    p_proc_data->p_svc->do_cmd(p_proc_data->p_svc, DVB_FREE,
      (u32)&eit_req, sizeof(dvb_request_t));
    p_proc_data->is_eit_mon = FALSE;
  }  
  p_proc_data->is_active = FALSE;
}

/*!
 Process on the state of table requested
 */
u8 epg_process_on_tab_req4(void *p_data, epg_policy_t *p_epg_impl)
{
  u8 tab_cnt = 0;
  u8 req_tab_num = 0;
  epg_impl_data_t *p_impl_data = (epg_impl_data_t *)(p_epg_impl->p_data);
  pti_ext_buf_t *p_ext_buf  =  NULL;
  proc_para_t  *p_proc_data = (proc_para_t *)p_data;
  tab_status_t *p_cur_tab_st = NULL;

  //Check table id in default table map 
  for(tab_cnt = 0; tab_cnt < p_impl_data->ext_buf_num; tab_cnt ++)
  {
      p_cur_tab_st = &p_proc_data->default_tab_map[tab_cnt];
      p_ext_buf = (pti_ext_buf_t *)(p_impl_data->p_ext_buf_list);
      p_ext_buf += tab_cnt;
      p_proc_data->default_tab_map[tab_cnt].p_buf_addr = p_ext_buf;
      p_proc_data->default_tab_map[tab_cnt].table_is_requested = TRUE;
      request_eit_section4(p_proc_data->p_svc, p_cur_tab_st->table_id,
        p_ext_buf, p_impl_data->ts_in);      
    }
   return req_tab_num;
}

pti_ext_buf_t *epg_process_on_tab_free4(void *p_data, epg_policy_t *p_epg_impl)
{
  u8 tab_cnt = 0;
  //pti_ext_buf_t *p_ext_buf = NULL;
  epg_impl_data_t *p_impl_data = (epg_impl_data_t *)(p_epg_impl->p_data);
  proc_para_t  *p_proc_data = (proc_para_t *)p_data;
  tab_status_t *p_tab_info = NULL;

  //Check in default table map
  for(tab_cnt = 0; tab_cnt < p_impl_data->ext_buf_num; tab_cnt++)
  {
    p_tab_info = &p_proc_data->default_tab_map[tab_cnt];
    free_eit_section4(p_proc_data->p_svc, p_tab_info->table_id,
       p_tab_info->p_buf_addr);
    p_tab_info->table_is_requested = FALSE;
  }

  return NULL;
}

void *epg_process_construct4(service_t *p_svc)
{
  proc_para_t *p_data = mtos_malloc(sizeof(proc_para_t));
  if(p_data == NULL)
  {
    return NULL;
  }
  memset(p_data, 0, sizeof(proc_para_t));  
  p_data->p_svc = p_svc;
  return p_data;
}


void epg_process_destruct4(void *p_handle)
{
  proc_para_t *p_data = (proc_para_t *)p_handle;
  mtos_free(p_data);
}
