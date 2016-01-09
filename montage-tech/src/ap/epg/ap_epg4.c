/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_task.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"

#include "drv_dev.h"
#include "pti.h"
#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "eit.h"
//#include "sdt.h"
#include "tdt.h"
#include "mt_time.h"

#include "epg_data4.h"
#include "ap_framework.h"
#include "ap_epg4.h"
#include "ap_epg_data_process4.h"
#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_epg_imp.h"
//#include "ap_epg_main.h"
//#include "ap_epg_default.h"

#include "dmx.h"

#define ONE_MINUTE  (60 * 100)

#define EPG_DEBUG
#ifdef EPG_DEBUG
#define EPG_PRINT  OS_PRINTF
#else
#define EPG_PRINT(...)
#endif

/*!
   Private data to ap epg
  */
typedef struct
{
  void *p_main_handle; 
} epg_data_t;


/*!
   EPG private information
  */
typedef struct
{
  //EPG private data
  //epg_data_t *p_epg_data;
  //EPG instance
  app_t epg_instance;
  //EPG policy
  epg_policy_t *p_epg_impl;
  //EPG DB Handle
  void *p_epg_db_handle;
  //misc Handle
  void *p_proc_data;
  /*!
    dvb sevice handle
    */
  service_t *p_svc;

}epg_priv_t;

static void send_msg_to_ui(u32 id)
{
  event_t evt = {0};
  evt.id = id;
  ap_frm_send_evt_to_ui(APP_EPG, &evt);
}

static void req_single_eit(service_t *p_dvb_svc, u32 para1, u32 para2, u8 ts_in)
{
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_EIT_ACTUAL;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = (DVB_TABLE_ID_EIT_ACTUAL << 24) | (para1 & 0xffff); // table_id & service_id
  dvb_req.para2 = para2; //P/F
  dvb_req.ts_in = ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc,
    DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));
}


static void epg_single_step(void *p_handle, os_msg_t *p_msg)
{
  epg_priv_t *p_priv_data = (epg_priv_t *)p_handle;
  epg_policy_t *p_epg_impl = p_priv_data->p_epg_impl; 
  epg_impl_data_t *p_impl_data = (epg_impl_data_t *)(p_epg_impl->p_data);
  proc_para_t  *p_proc_data = (proc_para_t  *)p_priv_data->p_proc_data;
  void *p_epg_db_handle = p_priv_data->p_epg_db_handle;
  epg_event_para_t evt_para = {0};
  u32 cur_tick = 0;
  event_t evt = {0};
  eit_t *p_eit = NULL;
  eit_t *p_eit_impl = NULL;
  eit_t eit_temp ={0};
  dvb_section_t *p_sec = NULL;
  u32 section_buf_len = 0;
  dvb_request_t *p_eit_req = NULL;
  epg_db_status_t ret = EPG_DB_NORM;
  BOOL pf_new_ver = FALSE;
  BOOL lang_switch = FALSE;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  
  if(p_msg != NULL)
  {  
    switch(p_msg->content)
    {
      case DVB_ONE_SEC_FOUND:
        p_eit = (eit_t *)p_msg->para1;
        p_sec = (dvb_section_t *)p_msg->para1;
        section_buf_len = p_msg->para2;
        eit_section_processing(p_sec->p_buffer, &eit_temp);
        
        p_eit_impl = (eit_t *)&eit_temp;
        if (p_epg_impl->epg_impl_send_rc_epg != NULL)
        {
            if (p_eit_impl->table_id != 0x4e && p_eit_impl->table_id != 0x4f)
            {
               //send p_sec buffer
                p_epg_impl->epg_impl_send_rc_epg(p_sec->p_buffer,section_buf_len);
            }
        }
        log_perf(LOG_AP_EPG, PERF_20_EVTS_BEGIN, p_eit_impl->svc_id,
                    (u32)(sht_evt_desc_t *)(p_eit_impl->sht_evt_info));

         /*****filter epg lang code*********/
        lang_switch = epg_data_try_lang_code_priority(
          p_epg_db_handle,p_eit_impl);
        if(lang_switch == TRUE)
        {
            epg_data_delete(p_epg_db_handle, EPG_DEL_ALL_NODE, NULL);
            evt.id = EPG_EVT_EVT_DELETED;
            evt.data1 = 0;
            evt.data2 = 0;
            ap_frm_send_evt_to_ui(APP_EPG, &evt);
            epg_process_on_tab_free4(p_proc_data, p_epg_impl);
            epg_process_on_tab_req4(p_proc_data, p_epg_impl);
            break;
        }
        epg_data_filter_evt_lang_code(p_epg_db_handle,p_eit_impl);
       /**********filter epg lang code end**/
        
        ret = epg_data_add(p_epg_db_handle, p_eit_impl, &pf_new_ver);
        log_perf(LOG_AP_EPG, PERF_20_EVTS_END, p_eit_impl->svc_id, 
                    (u32)(sht_evt_desc_t *)(p_eit_impl->sht_evt_info));
        //epg_data_get_db_report(p_epg_db_handle);
        break;
        
      case EPG_CMD_START_SCAN: 
        p_impl_data->ts_in = p_msg->para1;
        p_epg_impl->epg_impl_start(p_epg_impl->p_data, p_msg->para2);
        epg_process_tab_status_reset4(p_proc_data);
        epg_process_on_tab_req4(p_proc_data, p_epg_impl);
        break;

      case EPG_CMD_START_SCAN_PF:
        req_single_eit(p_proc_data->p_svc, p_msg->para1, p_msg->para2, p_impl_data->ts_in);

        break;
      case EPG_CMD_EIT_TS_UPDATE:  
        nc_get_ts_by_tuner(nc_handle, p_msg->para1&0xff, &p_impl_data->ts_in);
        p_proc_data->p_svc->do_cmd(p_proc_data->p_svc,  DVB_TABLE_TS_UPDATE,
            DVB_TABLE_ID_EIT_ACTUAL, p_impl_data->ts_in);
        break;
 
      case EPG_CMD_STOP_SCAN:
          epg_process_on_tab_free4(p_proc_data, p_epg_impl);          
        break;
      case DVB_FREED:
        p_eit_req = (dvb_request_t *)p_msg->para1;        
        if(p_eit_req->table_id == DVB_TABLE_ID_EIT_SCH_ACTUAL_51)
          {
            epg_data_mem_release(p_epg_db_handle);
            send_msg_to_ui(EPG_EVT_STOP);
          }
        break;   
      case EPG_CMD_SAMPLE:
          EPG_PRINT("!!!!!!!!!!!!!rcv msg from UI");
        break;           
      default:
        break;
    }
  }

  if(pf_new_ver == TRUE)
      {
        EPG_PRINT("!!!!EPG_EVT_NEW_PF_VER_FOUND:%d %d\n", 
          p_eit->svc_id, p_eit->sht_evt_info[0].parental_rating.rating);
        evt_para.orig_network_id = p_eit->org_nw_id;
        evt_para.ts_id = p_eit->stream_id;
        evt_para.svc_id = p_eit->svc_id;
        evt_para.parental_rating = p_eit->sht_evt_info[0].parental_rating.rating;
        evt.id = EPG_EVT_NEW_PF_VER_FOUND;
        evt.data1 = (u32)(&evt_para);
        ap_frm_send_evt_to_ui(APP_EPG, &evt);
      }
  
  if(p_impl_data->pf_switch_enable == TRUE)
  {
   //check pf section status before state machine
    if(p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].table_is_requested == TRUE)
    {
        cur_tick = mtos_ticks_get();
        if((cur_tick - p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].tick_requsted)
                                    > p_impl_data->pf_requested_interval)
        {
            free_eit_section4(p_proc_data->p_svc, 
                             p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].table_id,
                             p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].p_buf_addr);
            p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].table_is_requested = FALSE;
            p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].tick_freed = mtos_ticks_get();
        }
    }
    else
    {        
        cur_tick = mtos_ticks_get();
        if((cur_tick - p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].tick_freed)
                                                > p_impl_data->pf_freed_interval)
        {
            //EPG_PRINT("!!!!!Before delete overdue!!\n");            
            epg_data_delete(p_epg_db_handle, EPG_DEL_OVERDEU_EVT, NULL);   
            ///EPG_PRINT("!!!!!After delete overdue!!\n");  
            evt.id = EPG_EVT_EVT_DELETED;
            evt.data1 = 0;
            evt.data2 = 0;
            ap_frm_send_evt_to_ui(APP_EPG, &evt);
            
            //EPG_PRINT("!!!!!!!!!PF section OPEN!!!!!!!\n");
            request_eit_section4(p_proc_data->p_svc,
                                p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].table_id,
                                p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].p_buf_addr, 
                                p_impl_data->ts_in);
            p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].table_is_requested = TRUE;
            p_proc_data->default_tab_map[EIT_ACTUAL_INDEX].tick_requsted = mtos_ticks_get();
         }
    }
  }
//state machine
    switch(ret)
    {
      case EPG_DB_OVERFLOW:      
     //   EPG_PRINT("!!!!!MEM_OVERFLOW!Before delete by dynamic policy!\n");
   //     epg_data_get_db_report(p_epg_db_handle);
            epg_data_delete(p_epg_db_handle, EPG_DEL_EXP_DY_POLICY, NULL);
            evt.id = EPG_EVT_EVT_DELETED;
            evt.data1 = 0;
            evt.data2 = 0;
            ap_frm_send_evt_to_ui(APP_EPG, &evt);
   //     EPG_PRINT("!!!!!After delete by dynamic policy!\n");
  //      epg_data_get_db_report(p_epg_db_handle);
        break;   
      case EPG_DB_NEW_SEC_FOUND:
        if(++p_proc_data->captured_eit_num >= p_impl_data->new_section_num)
          {
           p_proc_data->captured_eit_num = 0;
           evt.id = EPG_EVT_NEW_SEC_FOUND;
           evt.data1 = p_eit->svc_id;
           evt.data2 = p_eit->stream_id;
           ap_frm_send_evt_to_ui(APP_EPG, &evt);
          }
        break;

      default:
      break;
    }  
}

/*!
   The init operation will be invoked once when system power up
  */
static void init(void *p_handle)
{
  epg_priv_t *p_epg_priv = (epg_priv_t *)p_handle;
  dvb_t *p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);
  p_epg_priv->p_svc = p_dvb_handle->get_svc_instance(p_dvb_handle, APP_EPG);
  p_epg_priv->p_epg_impl->epg_impl_init(p_epg_priv->p_epg_impl->p_data, 
      PTI_SLOT_NUM_AVAIL);
  p_epg_priv->p_proc_data = epg_process_construct4(p_epg_priv->p_svc);
  p_epg_priv->p_epg_db_handle = class_get_handle_by_id(EPG_CLASS_ID);
}

app_t *construct_ap_epg4(epg_policy_t *p_epg_policy)
{
  epg_priv_t *p_epg_priv = mtos_malloc(sizeof(epg_priv_t));

  if(p_epg_priv == NULL)
  {
    return NULL;
  }
  p_epg_priv->p_epg_impl = p_epg_policy;  
  memset(&(p_epg_priv->epg_instance), 0, sizeof(app_t));
  //Attach EPG instance
  p_epg_priv->epg_instance.get_msgq_timeout = NULL;
  p_epg_priv->epg_instance.init = init;
  p_epg_priv->epg_instance.task_single_step = epg_single_step;
  p_epg_priv->epg_instance.p_data = (void *)p_epg_priv;
  return &(p_epg_priv->epg_instance);
}
