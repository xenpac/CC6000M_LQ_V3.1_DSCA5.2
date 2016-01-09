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
#include "mtos_task.h"
#include "mtos_printk.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"

#include "dmx.h"

#include "class_factory.h"
#include "mdl.h"


//eva
#include "media_format_define.h"
#include "interface.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"

//filter
#include "epg_type.h"
#include "epg_database.h"
#include "eva_filter_factory.h"
#include "demux_filter.h"
#include "epg_filter.h"
#include "err_check_def.h"

//ap
#include "ap_framework.h"

#include "epg_type.h"
#include "epg_database.h"
#include "epg_api.h"

/*!
 epg private information
 */
typedef struct tag_epg_priv
{
 /*!
  controller
  */
  controller_t controller;
 /*!
  chain
  */
  chain_t     *p_chain;
 /*!
  dmx filter
  */
  ifilter_t   *p_dmx_filter;
 /*!
  demux pin.
  */
  ipin_t      *p_dmx_pin;
 /*!
  epg filter
  */
  ifilter_t   *p_epg_filter;
 /*!
  has P/F event
  */
  BOOL         b_pf_event;
/*!
  epg_fl_cfg_buf,if malloc by myself,it meed to free myself;
  */
 u8 *p_epg_fl_cfg_buf;
/*!
  evt call back
  */
  epg_evt_call_back evt_process;  
}epg_priv_t;


static eit_table_id_t tbl_id_pf_actual_array[] = 
{
  EIT_TABLE_ID_PF_ACTUAL,
};
static eit_table_id_t tbl_id_pf_other_array[] = 
{
  EIT_TABLE_ID_PF_OTHER,
};
static eit_table_id_t tbl_id_pf_all_array[] = 
{
  EIT_TABLE_ID_PF_ACTUAL,
  EIT_TABLE_ID_PF_OTHER,
};
static eit_table_id_t tbl_id_pf_all_sch_actual_array[] = 
{
  EIT_TABLE_ID_PF_ACTUAL,
  EIT_TABLE_ID_PF_OTHER,
  EIT_TABLE_ID_SCH_ACTUAL_50,
  EIT_TABLE_ID_SCH_ACTUAL_51,
};

static eit_table_id_t tbl_id_sch_actual_array[] = 
{
  EIT_TABLE_ID_SCH_ACTUAL_50,
  EIT_TABLE_ID_SCH_ACTUAL_51,
};
static eit_table_id_t tbl_id_sch_other_array[] = 
{
  EIT_TABLE_ID_SCH_OTHER_60,
  EIT_TABLE_ID_SCH_OTHER_61, 
};
static eit_table_id_t tbl_id_sch_all_array[] = 
{
  EIT_TABLE_ID_SCH_ACTUAL_50,
  EIT_TABLE_ID_SCH_ACTUAL_51,
  EIT_TABLE_ID_SCH_OTHER_60,
  EIT_TABLE_ID_SCH_OTHER_61,
};


/*!
 epg api get private data.
 */
static epg_priv_t *epg_get_priv(void)
{
  epg_priv_t *p_priv = class_get_handle_by_id(EPG_API_CLASS_ID);
  CHECK_FAIL_RET_NULL(p_priv != NULL);
  return p_priv;
}

static chain_t *epg_get_chain(void)
{
  epg_priv_t *p_priv = epg_get_priv();
  CHECK_FAIL_RET_NULL(p_priv->p_chain != NULL);
  return p_priv->p_chain;
}

static BOOL epg_on_process_evt(handle_t _this, os_msg_t *p_msg)
{
  epg_priv_t *p_priv_t = ((controller_t *)_this)->user_handle;
  u32 msg_id = 0;
  switch(p_msg->content)
  {
    case EPG_FILTER_SCHE_EVENT :
      msg_id = EPG_API_SCHE_EVENT;
      break;
    case EPG_FILTER_SCHE_EVENT_OTHER:
      msg_id = EPG_API_SCHE_EVENT_OTHER;
      break;
    case EPG_FILTER_DEL_EVENT :
      msg_id = EPG_API_DEL_EVENT;
      break;
    case EPG_FILTER_PF_EVENT :
      msg_id = EPG_API_PF_EVENT;
      break;
    case EPG_FILTER_PF_EVENT_OTHER:
      msg_id = EPG_API_PF_EVENT_OTHER;
      break;
    case EPG_FILTER_PF_FINISH_EVENT :
      msg_id = EPG_API_PF_SECTION_FINISH;
      break;
    case EPG_FILTER_PF_TIMEOUT_EVENT:
     msg_id = EPG_API_PF_SECTION_TIMEOUT;
     break;
    case EPG_FILTE_EIT_TIMEOUT_EVENT:
      msg_id = EPG_API_EIT_TIMEOUT;
     break;
    default :
      OS_PRINTF("epg_on_process_evt content %d\n", p_msg->content);
      CHECK_FAIL_RET_FALSE(0);
      break;
  }

  if(NULL != p_priv_t->evt_process)
  {
    p_priv_t->evt_process(msg_id,p_msg->para1,p_msg->para2);
  }
   
  return TRUE;
}

RET_CODE epg_destroy_chain(void)
{
  chain_t *p_chain = epg_get_chain();

  p_chain->_interface._destroy(p_chain);

  return SUCCESS;
}

static void epg_do_cmd_to_filter(ifilter_t *p_filter,
                                 u32        cmd,
                                 u32        lpara,
                                 u32        *p_para)
{
  icmd_t icmd = {0};

  //do command
  icmd.cmd    = cmd;
  icmd.lpara  = lpara;
  icmd.p_para = p_para;
  p_filter->do_command(p_filter, &icmd);
}

/*!
 * epg connect pin
 */
static void epg_connect_pin(epg_priv_t *p_priv)
{
  RET_CODE        ret       = ERR_FAILURE;
  ifilter_t      *p_filter  = NULL;
  ipin_t         *p_epg_pin = NULL;
  media_format_t  format    = {0};

  /*!
   *  Get unconnect pin of dmx
   */
  p_filter = p_priv->p_dmx_filter;
  ret = p_filter->get_unconnect_pin(p_filter, OUTPUT_PIN, &p_priv->p_dmx_pin);
  CHECK_FAIL_RET_VOID(p_priv->p_dmx_pin != NULL);

  /*!
   * Set dmx pin type
   */
  epg_do_cmd_to_filter(p_priv->p_dmx_filter,
                       DMX_SET_PIN_TYPE,
                       DMX_PIN_PSI_TYPE,
                       (u32 *)p_priv->p_dmx_pin);

  /*!
   * Get unconnect input pin of epg
   */
  p_filter = p_priv->p_epg_filter;
  ret = p_filter->get_unconnect_pin(p_filter, INPUT_PIN, &p_epg_pin);
  CHECK_FAIL_RET_VOID(p_epg_pin != NULL);

  /*!
   * Connect output pin and input pin
   */
  format.stream_type = MT_PSI;
  ret = p_priv->p_chain->connect_pin(p_priv->p_chain,
                                     p_priv->p_dmx_pin,
                                     p_epg_pin,
                                     &format);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

/*!
 * epg add filters to chain.
 */
static void epg_add_filters_to_chain(epg_priv_t *p_priv)
{
  chain_t *p_chain = p_priv->p_chain;
  RET_CODE ret = SUCCESS;

  CHECK_FAIL_RET_VOID(p_chain != NULL);

  //add dmx filters to chain
  ret = p_chain->add_filter(p_chain, p_priv->p_dmx_filter, "dmx filter");
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  //add epg filters to chain
  ret = p_chain->add_filter(p_chain, p_priv->p_epg_filter, "epg filter");
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

/*!
 *epg create filters(dmx filter + epg filter)
 */
static void epg_create_filters(epg_priv_t *p_priv)
{
  RET_CODE ret = ERR_FAILURE;

  //add demux filter
  ret = eva_add_filter_by_id(DEMUX_FILTER, &p_priv->p_dmx_filter, NULL);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  CHECK_FAIL_RET_VOID(p_priv->p_dmx_filter != NULL);

  //add epg filter.
  ret = eva_add_filter_by_id(EPG_FILTER, &p_priv->p_epg_filter, NULL);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  CHECK_FAIL_RET_VOID(p_priv->p_epg_filter != NULL);
}

/*!
  epg create chain

  \param[in] p_priv : epg api private data.
  \param[in] task_priority : task priority for chain.
  \param[in] task_stk_size : task stack size.
  */
static void epg_create_chain(epg_priv_t *p_priv,
                             u32         task_priority,
                             u32         task_stk_size)
{
  controller_t *p_contrl   = NULL;
  chain_para_t  chain_para = { 0 };
  ctrl_para_t   ctrl_para  = { 0 };

  //create controller
  p_contrl = &(p_priv->controller);
  ctrl_para.user_id = APP_EPG;
  controller_create(p_contrl, &ctrl_para);
  CHECK_FAIL_RET_VOID(p_contrl != NULL);
  
  p_contrl->on_process_evt = epg_on_process_evt;
  p_contrl->user_handle = (handle_t)p_priv;

  //create chain
  chain_para.p_owner  = (interface_t *)&p_priv->controller;
  chain_para.p_name   = "epg_chain";
  //chain_para.priority = task_priority;
  chain_para.stk_size = task_stk_size;
  p_priv->p_chain = chain_create(&chain_para);
}


void mul_epg_set_db_info(epg_prog_info_t *p_prog_info)
{
    epg_db_set_info(p_prog_info);
}
event_node_t *mul_epg_get_sch_event(epg_prog_info_t *p_prog_info,
                                    u16             *p_event_num)
{
  return epg_db_get_sch_event(p_prog_info, p_event_num);
}

event_node_t *mul_epg_get_sch_event_by_pos(epg_prog_info_t *p_prog_info,
                                           event_node_t    *p_evt_head,
                                           u16              offset)
{
  return epg_db_get_sch_event_by_pos(p_prog_info, p_evt_head, offset);
}

event_node_t *mul_epg_get_event_by_nibble(epg_nibble_t *p_nibble,
                                          u16 *p_event_num)
{
  if(p_nibble->nibble_in_pf == TRUE)
  {
    return epg_db_get_pf_event_by_nibble(p_nibble, p_event_num);
  }
  else
  {
    return epg_db_get_event_by_nibble(p_nibble, p_event_num);
  }
}

event_node_t *mul_epg_get_next_event_by_nibble(epg_nibble_t *p_nibble,
                                               event_node_t *p_evt_head)
{
  return epg_db_get_next_event_by_nibble(p_nibble, p_evt_head);
}

void mul_epg_get_config_help_info(u16 net_node_num,
                                                               u16 ts_node_num,
                                                               u16 svc_node_num,
                                                               u16 evt_node_num,
                                                               u32 mem_size)
{
  epg_db_get_config_help_info(net_node_num,
                                                    ts_node_num,
                                                    svc_node_num,
                                                    evt_node_num,
                                                    mem_size);
}
RET_CODE mul_epg_get_pf_event(epg_prog_info_t *p_prog_info,
                              event_node_t    **pp_present_evt,
                              event_node_t    **pp_follow_evt)
{ 
  epg_priv_t   *p_priv = epg_get_priv();
  u16          evt_num = 0;
  RET_CODE     ret     = SUCCESS;
  
  if(p_priv->b_pf_event)
  {
    ret = epg_db_get_pf_event(p_prog_info, pp_present_evt, pp_follow_evt);    
  }
  else
  {
    *pp_present_evt = epg_db_get_sch_event(p_prog_info, &evt_num);
    if(*pp_present_evt == NULL)
      return ERR_FAILURE;
      
    if(evt_num > 1)//has follow event
    {
      *pp_follow_evt = epg_db_get_sch_event_by_pos(p_prog_info,
                                                  *pp_present_evt, 1);
    }
  }
  return ret;
}

RET_CODE mul_epg_filter_policy(epg_filter_ply_t *p_policy)
{
    epg_priv_t   *p_priv = epg_get_priv();
    u32           epg_ft_buf_size = 0;
    epg_filter_cfg_t epg_fl_cfg = {0};
    psi_pin_cfg_t psi_cfg = { 0 };


  if(FALSE == p_priv->b_pf_event)
  {
      if(p_policy->epg_selecte == EPG_TABLE_SELECTE_PF_ALL)
      {
          p_policy->epg_selecte = EPG_TABLE_SELECTE_SCH_ALL;
      }
      if(p_policy->epg_selecte == EPG_TABLE_SELECTE_PF_ACTUAL)
      {
          p_policy->epg_selecte = EPG_TABLE_SELECTE_SCH_ACTUAL;
      }
      if(p_policy->epg_selecte == EPG_TABLE_SELECTE_PF_OTHER)
      {
          p_policy->epg_selecte = EPG_TABLE_SELECTE_SCH_OTHER;
      }
     if(p_policy->epg_selecte == EPG_TABLE_SELECTE_PF_ALL_SCH_ACTUAL)
      {
          p_policy->epg_selecte = EPG_TABLE_SELECTE_SCH_ACTUAL;
      }
  }
  switch(p_policy->epg_selecte)
  {
      case EPG_TABLE_SELECTE_PF_ALL:
      p_policy->p_tbl_id_list = tbl_id_pf_all_array;
      p_policy->max_eit_table_num = sizeof(tbl_id_pf_all_array) / sizeof(eit_table_id_t);
      break;
      case EPG_TABLE_SELECTE_PF_ACTUAL:
      p_policy->p_tbl_id_list = tbl_id_pf_actual_array;
      p_policy->max_eit_table_num = sizeof(tbl_id_pf_actual_array) / sizeof(eit_table_id_t);
      break;
      case EPG_TABLE_SELECTE_PF_OTHER:
      p_policy->p_tbl_id_list = tbl_id_pf_other_array;
      p_policy->max_eit_table_num = sizeof(tbl_id_pf_other_array) / sizeof(eit_table_id_t);
      break;
      case EPG_TABLE_SELECTE_SCH_ALL:
      p_policy->p_tbl_id_list = tbl_id_sch_all_array;
      p_policy->max_eit_table_num = sizeof(tbl_id_sch_all_array) / sizeof(eit_table_id_t);
      break;
      case EPG_TABLE_SELECTE_SCH_ACTUAL:
      p_policy->p_tbl_id_list = tbl_id_sch_actual_array;
      p_policy->max_eit_table_num = sizeof(tbl_id_sch_actual_array) / sizeof(eit_table_id_t);
      break;
      case EPG_TABLE_SELECTE_SCH_OTHER:
      p_policy->p_tbl_id_list = tbl_id_sch_other_array;
      p_policy->max_eit_table_num = sizeof(tbl_id_sch_other_array)/ sizeof(eit_table_id_t);
      break;
      case EPG_TABLE_SELECTE_PF_ALL_SCH_ACTUAL:
      p_policy->p_tbl_id_list = tbl_id_pf_all_sch_actual_array;
      p_policy->max_eit_table_num = sizeof(tbl_id_pf_all_sch_actual_array) / sizeof(eit_table_id_t);
      break;
      case EPG_TABLE_SELECTE_FROM_CONFIG:
      break;
      default:
      break;
  }
  /*!
   *config dmx filter.
   */
  psi_cfg.max_table_num = p_policy->max_eit_table_num;
  //fix demux_index here
  psi_cfg.demux_index = p_policy->demux_index;
  epg_do_cmd_to_filter(p_priv->p_dmx_filter,
                       DMX_CFG_PARA,
                       (u32)&psi_cfg,
                       (u32 *)p_priv->p_dmx_pin);
  /*!
   *config epg filter
   */
   if(p_policy->spin_multi_buffer_size < EIT_BUFFER_SIZE)
    {
       p_policy->spin_multi_buffer_size = EIT_BUFFER_SIZE;
    }
   
  
  epg_ft_buf_size = p_policy->spin_multi_buffer_size * p_policy->max_eit_table_num;
  
  if((NULL == p_policy->p_attach_buf) || (0 == p_policy->buffer_size))
  {

      if(NULL != p_priv->p_epg_fl_cfg_buf)
      {
          mtos_free(p_priv->p_epg_fl_cfg_buf);
           p_priv->p_epg_fl_cfg_buf = NULL;
      } 
      p_priv->p_epg_fl_cfg_buf  = (u8 *)mtos_malloc(epg_ft_buf_size);
      CHECK_FAIL_RET_CODE(NULL != p_priv->p_epg_fl_cfg_buf);
      //attach buffer.
      epg_fl_cfg.p_attach_buf = p_priv->p_epg_fl_cfg_buf;
      epg_fl_cfg.buffer_size  = epg_ft_buf_size; 
  }
  else
  {
    //attach buffer.
    epg_fl_cfg.p_attach_buf = p_policy->p_attach_buf;
    epg_fl_cfg.buffer_size  = p_policy->buffer_size;
    CHECK_FAIL_RET_CODE(epg_ft_buf_size <= epg_fl_cfg.buffer_size);
    p_priv->p_epg_fl_cfg_buf = NULL;
  }

  epg_fl_cfg.spin_multi_buffer_size = p_policy->spin_multi_buffer_size;
  epg_fl_cfg.msg_send_cycle = p_policy->msg_send_cycle;
  if((epg_fl_cfg.msg_send_cycle > 0)
      && (epg_fl_cfg.msg_send_cycle < 100))
    {
      epg_fl_cfg.msg_send_cycle = 100;
    }
  
  //table id list.
  epg_fl_cfg.max_eit_table_num = p_policy->max_eit_table_num;
  epg_fl_cfg.p_tbl_id_list     = p_policy->p_tbl_id_list;
  
  //policy
  epg_fl_cfg.pf_policy  = p_policy->pf_policy;
  epg_fl_cfg.sch_policy = p_policy->sch_policy;
  epg_fl_cfg.net_policy = p_policy->net_policy;
  
  epg_fl_cfg.pf_timeout = p_policy->pf_timeout;
  epg_fl_cfg.eit_timeout = p_policy->eit_timeout;
  epg_fl_cfg.evt_fixed_able = p_policy->evt_fixed_able;
  epg_fl_cfg.task_release_time = p_policy->task_release_time;
  memcpy(&epg_fl_cfg.prog_info,&p_policy->prog_info,sizeof(epg_prog_info_t)) ;
  
  epg_do_cmd_to_filter(p_priv->p_epg_filter,
                       EPG_FILTER_CMD_CONFIG,
                       0,
                       (u32 *)&epg_fl_cfg);
  epg_db_set_info(&epg_fl_cfg.prog_info);
  return SUCCESS;
}
RET_CODE mul_epg_config(epg_config_t *p_config)
{
  epg_priv_t   *p_priv = epg_get_priv();
  epg_db_cfg_t  db_cfg   = {0};

  if(FALSE == p_config->config_open)
  {
      /****the policy will auto adjust by memory,it can don't config*****/
      p_config->sch_policy = SCH_EVENT_OF_NETWORK_ID;
      p_config->pf_policy  = PF_EVENT_OF_NETWORK_ID;
      p_config->fixed_sch_text_policy = SCH_FIXED_TEXT_NO_ID;
      p_config->fixed_pf_text_policy =  PF_FIXED_TEXT_NO_ID;
      
        /****node size: net[12]B,ts[12]B,svc[20]B,evt[104]B********/
      p_config->net_node_num = 2;
      p_config->ts_node_num  = 128;
      p_config->svc_node_num = 512;
      p_config->evt_node_num = 5000;
      p_config->max_actual_event_days = 7;
      p_config->max_other_event_days = 1;
      p_config->has_pf_evt = TRUE;
      p_config->ext_able = FALSE;
      p_config->text_able = TRUE;
      p_config->max_ext_count = 1;
      p_config->fixed_svc_node_num = 10;
      p_config->del_pf_able = FALSE;
      p_config->text_buffer_size = 256 * KBYTES;
      p_config->lang_code_check_times = 200;
      p_config->lang_code_forbit_swich = FALSE;
      p_config->lang_code_sync = LANG_CODE_SYNC_SVC;
  }

  /*!
   * config epg database.
   */
   if((NULL == p_config->p_attach_buf) || (0 == p_config->buffer_size))
    {
        db_cfg.p_attach_buf = NULL; /**it will malloc min node number***/
        db_cfg.buffer_size = 0;
        /****the policy will auto adjust by memory,it can don't config*****/
        p_config->sch_policy = SCH_EVENT_OF_NETWORK_ID;
        p_config->pf_policy  = PF_EVENT_OF_NETWORK_ID;
        p_config->fixed_sch_text_policy = SCH_FIXED_TEXT_NO_ID;
        p_config->fixed_pf_text_policy =  PF_FIXED_TEXT_NO_ID;
        
        /****node size: net[12]B,ts[12]B,svc[20]B,evt[104]B********/
        p_config->net_node_num = 2;
        p_config->ts_node_num  = 128;
        p_config->svc_node_num = 512;
        p_config->evt_node_num = 5000;
        p_config->max_actual_event_days = 7;
        p_config->max_other_event_days = 1;
         p_config->has_pf_evt = TRUE;
        p_config->ext_able = FALSE;
        p_config->text_able = FALSE;
        p_config->max_ext_count = 1;
        p_config->fixed_svc_node_num = 10;
        p_config->del_pf_able = TRUE;
        p_config->text_buffer_size = 1 * KBYTES;
        p_config->lang_code_check_times = 50;
        p_config->lang_code_forbit_swich = TRUE;
        p_config->lang_code_sync = LANG_CODE_SYNC_TS;
    }
   else
    {
      db_cfg.p_attach_buf =  p_config->p_attach_buf;
      db_cfg.buffer_size = p_config->buffer_size;
    }
  
  db_cfg.net_node_num   = p_config->net_node_num;
  db_cfg.ts_node_num    = p_config->ts_node_num;
  db_cfg.svc_node_num   = p_config->svc_node_num;
  db_cfg.evt_node_num   = p_config->evt_node_num;
  db_cfg.max_actual_event_days = p_config->max_actual_event_days;
  db_cfg.max_other_event_days = p_config->max_other_event_days;
  //db_cfg.p_tbl_id_list  = p_config->p_tbl_id_list;
  //db_cfg.table_num      = p_config->max_eit_table_num;
  //db_cfg.net_policy     = p_config->net_policy;
  db_cfg.sch_policy     = p_config->sch_policy;
  db_cfg.pf_policy      = p_config->pf_policy;
  db_cfg.fixed_sch_text_policy = p_config->fixed_sch_text_policy;
  db_cfg.fixed_pf_text_policy = p_config->fixed_pf_text_policy;
  db_cfg.ext_able = p_config->ext_able;
  db_cfg.text_able = p_config->text_able;
  db_cfg.lang_code_check_times = p_config->lang_code_check_times;
  db_cfg.lang_code_forbit_swich = p_config->lang_code_forbit_swich;
  db_cfg.lang_code_sync = p_config->lang_code_sync;
  db_cfg.eit_edit_id_call_back = p_config->eit_edit_id_call_back;
  db_cfg.eit_edit_info_call_back = p_config->eit_edit_info_call_back;
  db_cfg.max_ext_count = p_config->max_ext_count;
  db_cfg.fixed_svc_node_num = p_config->fixed_svc_node_num;
  db_cfg.del_pf_able = p_config->del_pf_able;
  db_cfg.fixed_sch_text_policy = p_config->fixed_sch_text_policy;
  db_cfg.text_buffer_size = p_config->text_buffer_size;
  p_priv->b_pf_event = p_config->has_pf_evt;
  
  epg_db_config(&db_cfg);
  return SUCCESS;
}


void mul_epg_open(void)
{
  epg_priv_t *p_priv = epg_get_priv();
  RET_CODE    ret    = ERR_FAILURE;

  ret = p_priv->p_chain->open(p_priv->p_chain);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

void mul_epg_close(void)
{
  epg_priv_t *p_priv = epg_get_priv();
  RET_CODE    ret    = ERR_FAILURE;

  ret = p_priv->p_chain->close(p_priv->p_chain);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

void mul_epg_set_filter_info(epg_prog_info_t *p_prog_info)
{
  epg_priv_t *p_priv = epg_get_priv();

  epg_do_cmd_to_filter(p_priv->p_epg_filter,
                       EPG_FILTER_CMD_SET_INFO,
                       0,
                       (u32 *)p_prog_info);
  epg_db_set_info(p_prog_info);   
}

void mul_epg_start(void)
{
  chain_t *p_chain = epg_get_chain();
  epg_priv_t *p_priv = epg_get_priv();
  RET_CODE ret     = ERR_FAILURE;
  ret = p_priv->p_chain->open(p_priv->p_chain);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  ret = p_chain->start(p_chain);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

void mul_epg_stop(void)
{
  chain_t *p_chain = epg_get_chain();
  epg_priv_t *p_priv = epg_get_priv();
  RET_CODE ret     = ERR_FAILURE;
  ret = p_chain->stop(p_chain);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);  
  ret = p_priv->p_chain->close(p_priv->p_chain);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
   if(NULL != p_priv->p_epg_fl_cfg_buf)
    {
        mtos_free(p_priv->p_epg_fl_cfg_buf);
        p_priv->p_epg_fl_cfg_buf = NULL;
    } 
  
}

RET_CODE mul_epg_deinit(void)
{
  epg_priv_t *p_priv = epg_get_priv();
  epg_db_deinit();
  epg_destroy_chain(); 
  mtos_free(p_priv);
  class_register(EPG_API_CLASS_ID, NULL);
  return SUCCESS;
}

RET_CODE mul_epg_init(epg_init_para_t *p_para)
{
  epg_priv_t *p_priv = NULL;
  if(class_get_handle_by_id(EPG_API_CLASS_ID) != NULL)
  {
    return SUCCESS;
  }

  CHECK_FAIL_RET_CODE(p_para != NULL);

 /*!
  epg database initialize.
  */
  epg_db_init();

  //alloc epg api private data.
  p_priv = (epg_priv_t *)mtos_malloc(sizeof(epg_priv_t));
  CHECK_FAIL_RET_CODE(p_priv != NULL);
  memset(p_priv,0,sizeof(epg_priv_t));
  class_register(EPG_API_CLASS_ID, (class_handle_t)p_priv);

  p_priv->evt_process = p_para->evt_process;
 /*!
  epg create chain
  */
  epg_create_chain(p_priv, 
                   p_para->task_priority,
                   p_para->task_stk_size);

 /*!
  epg create filters
  */
  epg_create_filters(p_priv);

 /*!
  epg add filters to chain
  */
  epg_add_filters_to_chain(p_priv);

 /*!
  epg connect pin.
  */
  epg_connect_pin(p_priv);
  return SUCCESS;
}
