/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_epg_api.h"
#include "ui_config.h"


//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "ipin.h"
#include "ifilter.h"
#include "eva_filter_factory.h"
#include "demux_interface.h"
#include "demux_filter.h"
#include "epg_type.h"

u16 ui_epg_evtmap(u32 event);
static void get_current_epg_program_info(epg_prog_info_t *prog_info);


static BOOL g_epg_start = FALSE;
static BOOL g_epg_init = FALSE;
static epg_table_selecte_policy run_policy = EPG_TABLE_SELECTE_UNKOWN;

static u8 *p_epg_new_buf =  NULL;
static BOOL g_epg_buf_new = FALSE;

void epg_api_evt_process(u32 content, u32 para1, u32 para2)
{
    event_t evt = {0};
    BOOL  send_flag = TRUE;
    
    evt.id = content;
    evt.data1 = para1;
    evt.data2 = para2;
   
    switch(content)
    {
      case EPG_API_SCHE_EVENT:
      if(para1 == EVENT_ATTR_OTHERS_PROG)
      {
        return;
      }
      OS_PRINTF("epg_api_evt_process status:EPG_API_SCHE_EVENT\n"); 
      break;
      case EPG_API_DEL_EVENT:
      OS_PRINTF("epg_api_evt_process status:EPG_API_DEL_EVENT\n"); 
      break;
      case EPG_API_PF_EVENT:
       if(para1 == EVENT_ATTR_OTHERS_PROG)
      {
        return;
      }
      OS_PRINTF("epg_api_evt_process status:EPG_API_PF_EVENT\n");  
      break;
      case EPG_API_PF_SECTION_FINISH:
      OS_PRINTF("epg_api_evt_process status:EPG_API_PF_SECTION_FINISH\n");  
      break;
      case EPG_API_PF_SECTION_TIMEOUT:
       OS_PRINTF("epg_api_evt_process status:EPG_API_PF_SECTION_TIMEOUT\n");  
      break;
      case EPG_API_EIT_TIMEOUT:
       OS_PRINTF("epg_api_evt_process status:EPG_API_EIT_TIMEOUT\n");  
       break;
      default :
       send_flag = FALSE;    
      break;
    }
    if(TRUE == send_flag)
    {
      //OS_PRINTF("epg_api_evt_process content[%d]\n", content);  
      ap_frm_send_evt_to_ui(APP_EPG, &evt);
    }
}

void epg_api_init(void)
{
  epg_init_para_t para;
  epg_config_t  epg_config;
  
  memset(&para,0,sizeof(epg_init_para_t));
  para.task_priority = AP_EPG_TASK_PRIORITY;
  para.task_stk_size = AP_EPG_TASK_STKSIZE;
  para.evt_process = epg_api_evt_process;
  mul_epg_init(&para);

  memset(&epg_config,0,sizeof(epg_config_t));
  #ifndef WIN32 
  #if 1
    #if 1
    epg_config.p_attach_buf = (u8 *)mem_mgr_require_block(BLOCK_EPG_BUFFER, SYS_MODULE_EPG);
    MT_ASSERT(epg_config.p_attach_buf != NULL);
    epg_config.buffer_size = mem_mgr_get_block_size(BLOCK_EPG_BUFFER);
    mem_mgr_release_block(BLOCK_EPG_BUFFER);
    OS_PRINTF("%s epg addr 0x%x\n",__FUNCTION__,epg_config.p_attach_buf);
    p_epg_new_buf =  NULL;
    g_epg_buf_new = FALSE;
    #else
    epg_config.buffer_size = 1 * KBYTES * KBYTES;                                                                             
    epg_config.p_attach_buf = (u8 *)mtos_malloc(epg_config.buffer_size);
    MT_ASSERT(epg_config.p_attach_buf != NULL);
    p_epg_new_buf =  epg_config.p_attach_buf;
    g_epg_buf_new = TRUE;
   #endif 
  epg_config.config_open = FALSE;
  #else
  epg_config.buffer_size = 64 * KBYTES;////1 * KBYTES * KBYTES;                                                                             
  epg_config.p_attach_buf = (u8 *)mtos_malloc(epg_config.buffer_size);
  MT_ASSERT(epg_config.p_attach_buf != NULL);
  {
      /****the policy will auto adjust by memory,it can don't config*****/
      epg_config.sch_policy = SCH_EVENT_OF_NETWORK_ID;
      epg_config.pf_policy  = PF_EVENT_OF_NETWORK_ID;
      epg_config.fixed_sch_text_policy = SCH_FIXED_TEXT_NO_ID;
      epg_config.fixed_pf_text_policy =  PF_FIXED_TEXT_NO_ID;
      
        /****node size: net[12]B,ts[12]B,svc[20]B,evt[104]B********/
      epg_config.net_node_num = 2;
      epg_config.ts_node_num  = 128;
      epg_config.svc_node_num = 250;//512;
      epg_config.evt_node_num =  500;//5000;
      epg_config.max_actual_event_days = 7;
      epg_config.max_other_event_days = 1;
      epg_config.has_pf_evt = TRUE;
      epg_config.ext_able = FALSE;
      epg_config.text_able = TRUE;
      epg_config.max_ext_count = 1;
      epg_config.fixed_svc_node_num = 10;
      epg_config.del_pf_able = FALSE;
      epg_config.text_buffer_size = 10 * KBYTES;

      epg_config.config_open = TRUE;
    }
  #endif
  #else
  epg_config.buffer_size = 1 * KBYTES * KBYTES;                                                                             
  epg_config.p_attach_buf = (u8 *)mtos_malloc(epg_config.buffer_size);
  MT_ASSERT(epg_config.p_attach_buf != NULL);
    p_epg_new_buf =  epg_config.p_attach_buf;
    g_epg_buf_new = TRUE;
  #endif
  if(g_customer.customer_id != CUSTOMER_CHANGHONG)
 {
   epg_db_set_lang_code("eng","zho","zho");
 }
  mul_epg_config(&epg_config);                                                                          
}
                 
void ui_epg_init(void)
{
  if (g_epg_init)
  {
    OS_PRINTF("ui epg already init!\n");
    return;
  }
  epg_api_init();
  fw_register_ap_evtmap(APP_EPG, ui_epg_evtmap);
  fw_register_ap_msghost(APP_EPG, ROOT_ID_EPG); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_PROG_LIST); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_PROG_BAR); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_FAV_LIST); 

  g_epg_init = TRUE;
}
void ui_epg_api_release(void)
{
   ui_epg_stop();
   mul_epg_deinit();
  /******free  epg api config buffer******/
  if ((p_epg_new_buf != NULL) && (g_epg_buf_new == TRUE))
  {
     mtos_free(p_epg_new_buf);
     p_epg_new_buf = NULL;
  } 
}

void ui_epg_release(void)
{
  if (! g_epg_init)
  {
    OS_PRINTF("ui epg already released!\n");
    return;
  }
  
  ui_epg_api_release();
  
  fw_unregister_ap_evtmap(APP_EPG);
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_EPG); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_PROG_LIST); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_PROG_BAR); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_FAV_LIST);

  g_epg_init = FALSE;
}

void ui_epg_start(epg_table_selecte_policy selecte)
{
  epg_filter_ply_t config;
#if 0
  void *epg_buffer = NULL;
  u32 epg_buffer_size = 0;
#endif

  if((TRUE == g_epg_start ) && (run_policy == selecte))
  {
    return;
  }
  if(CUSTOMER_ID == CUSTOMER_AISAT || CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
  	selecte = EPG_TABLE_SELECTE_SCH_ALL;
  ui_epg_stop(); 
  #if 0
  epg_buffer = (void *)mem_mgr_require_block(BLOCK_REC_BUFFER, SYS_MODULE_EPG);
  MT_ASSERT(epg_buffer!= NULL);
  epg_buffer_size = mem_mgr_get_block_size(BLOCK_REC_BUFFER);
  mem_mgr_release_block(BLOCK_REC_BUFFER);
  //memset((void *)(epg_buffer),0,epg_buffer_size);
  #endif
  if(FALSE == g_epg_start)
  {   
      run_policy = selecte;
      memset(&config,0,sizeof(epg_filter_ply_t));
      config.epg_selecte = selecte;
      config.pf_timeout = 10000;/**ms**/  
      config.eit_timeout = 10000; /**ms***/ 
      config.pf_policy  =  PF_EVENT_OF_NETWORK_ID;//PF_EVENT_OF_NETWORK_ID;
      config.sch_policy = SCH_EVENT_OF_TS_ID;
      config.net_policy = NETWORK_ID_NORMAL;
      switch(selecte)
      {
          case EPG_TABLE_SELECTE_PF_ALL:
          config.evt_fixed_able = TRUE;
          break;
          case EPG_TABLE_SELECTE_PF_ACTUAL:
          config.evt_fixed_able = TRUE;  
          break;
          case EPG_TABLE_SELECTE_PF_OTHER:
          config.evt_fixed_able = TRUE;
          break;
          case EPG_TABLE_SELECTE_SCH_ALL:
          config.evt_fixed_able = FALSE;  
          break;
          case EPG_TABLE_SELECTE_SCH_ACTUAL:
          config.evt_fixed_able = FALSE;  
          break;
          case EPG_TABLE_SELECTE_SCH_OTHER:
          config.evt_fixed_able = FALSE;  
          break;
          case EPG_TABLE_SELECTE_PF_ALL_SCH_ACTUAL:
           config.evt_fixed_able = TRUE; 
          break;
          case EPG_TABLE_SELECTE_FROM_CONFIG:
           config.evt_fixed_able = TRUE; 
          break;
          default:
          break;
      }
      get_current_epg_program_info(&config.prog_info);   
      mul_epg_filter_policy(&config);  
      #ifndef WIN32
      mul_epg_start();              
      #endif
      g_epg_start = TRUE;
  }
}

void ui_epg_stop(void)
{   
    if(TRUE == g_epg_start )
    {
        g_epg_start = FALSE;
        #ifndef WIN32
        mul_epg_stop();
        #endif
    } 
 }
   
static void get_current_epg_program_info(epg_prog_info_t *prog_info)
{
    dvbs_prog_node_t pg = {0};
    utc_time_t g_time_st = {0};
    utc_time_t g_time_end = {0};
    db_dvbs_ret_t ret = DB_DVBS_OK;
    u16 pg_id = 0;
   
    time_get(&g_time_st, FALSE);
    memcpy(&g_time_end, &g_time_st, sizeof(utc_time_t));
    time_up(&g_time_end, 24 * 60 * 60);
    pg_id = sys_status_get_curn_group_curn_prog_id();
    if(INVALIDID == pg_id)
    {
      prog_info->network_id = EPG_IGNORE_ID;
      prog_info->ts_id       = EPG_IGNORE_ID;
      prog_info->svc_id     = EPG_IGNORE_ID;
    }
    ret = db_dvbs_get_pg_by_id(pg_id, &pg);
    if(DB_DVBS_OK == ret)
    {
      prog_info->network_id = (u16)pg.orig_net_id;
      prog_info->ts_id      = (u16)pg.ts_id;
      prog_info->svc_id     = (u16)pg.s_id;
    }
    else
    {
      prog_info->network_id = EPG_IGNORE_ID;
      prog_info->ts_id       = EPG_IGNORE_ID;
      prog_info->svc_id     = EPG_IGNORE_ID;
    }  
    memcpy(&prog_info->start_time, &g_time_st, sizeof(utc_time_t));
    memcpy(&prog_info->end_time, &g_time_end, sizeof(utc_time_t));
 }

void ui_epg_set_program_info(u16 pg_id)
{
    db_dvbs_ret_t ret = DB_DVBS_OK;
    dvbs_prog_node_t pg = {0};
    epg_prog_info_t prog_info = {0};
    utc_time_t g_time_st = {0};
    utc_time_t g_time_end = {0};
    
     time_get(&g_time_st, FALSE);
     memcpy(&g_time_end, &g_time_st, sizeof(utc_time_t));
     time_up(&g_time_end, 24 * 60 * 60);
     if(INVALIDID != pg_id)
    {
      ret = db_dvbs_get_pg_by_id(pg_id, &pg);
      MT_ASSERT(DB_DVBS_OK == ret);
      prog_info.network_id = (u16)pg.orig_net_id;
      prog_info.ts_id      = (u16)pg.ts_id;
      prog_info.svc_id     = (u16)pg.s_id;
    }
    else
    {
      prog_info.network_id = EPG_IGNORE_ID;
      prog_info.ts_id       = EPG_IGNORE_ID;
      prog_info.svc_id     = EPG_IGNORE_ID;
    }
    memcpy(&prog_info.start_time, &g_time_st, sizeof(utc_time_t));
    memcpy(&prog_info.end_time, &g_time_end, sizeof(utc_time_t));
    OS_PRINTF("epg api set filter info: net_id:0x%x,ts_id:0x%x,s_id:0x%x\n",
      prog_info.network_id,prog_info.ts_id,prog_info.svc_id );
    #ifndef WIN32
    mul_epg_set_filter_info(&prog_info);                            
    #endif
}


BEGIN_AP_EVTMAP(ui_epg_evtmap)
  CONVERT_EVENT(EPG_API_SCHE_EVENT, MSG_SCHE_READY)    
  CONVERT_EVENT(EPG_API_PF_EVENT,   MSG_PF_READY)
  CONVERT_EVENT(EPG_API_DEL_EVENT,  MSG_EPG_DEL)
  CONVERT_EVENT(EPG_API_PF_SECTION_FINISH,  MSG_EPG_CHANG_TABLE_POLICY)
  CONVERT_EVENT(EPG_API_PF_SECTION_TIMEOUT,  MSG_EPG_CHANG_TABLE_POLICY)
END_AP_EVTMAP(ui_epg_evtmap)


