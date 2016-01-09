/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

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
#include "drv_dev.h"
#include "mdl.h"

#include "nim.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"

//filter
#include "eva_filter_factory.h"
#include "demux_filter.h"


//ap
#include "ap_framework.h"
#include "ota_public_def.h"
#include "ota_api.h"
#include "ota_filter.h"
#include "err_check_def.h"

/*!
 ota private information
 */
typedef struct tag_ota_priv
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
  ota filter
  */
  ifilter_t   *p_ota_filter;
/*!
  ota filter
  */
  ifilter_t   *p_flash_filter;
/*!
  evt call back
  */
  ota_evt_call_back evt_process; 
  /*!
    OTA start mak bit,
    */
  BOOL ota_start_bit;
  /*!
    ota medium select
    */
  ota_medium_t medium;
  /*!
    ota work select
    */
  //ota_work_t  run_select;
  /*!
    stream protocol type
    */
  ota_stream_protocol_t protocol_type;
   /*!
    ota dm debug funciton
    */
    u8 debug_level;
   /*!
    ota dm debug funciton
    */
    ota_printf_t ota_api_printf;
}ota_priv_t;


/*!
 ota api get private data.
 */
static ota_priv_t *ota_api_get_priv(void)
{
  ota_priv_t *p_priv = class_get_handle_by_id(OTA_API_CLASS_ID);
  CHECK_FAIL_RET_NULL(p_priv != NULL);
  return p_priv;
}

/*!
 ota api get chain.
 */
static chain_t *ota_api_get_chain(void)
{
  ota_priv_t *p_priv = ota_api_get_priv();  
  CHECK_FAIL_RET_NULL(p_priv->p_chain != NULL);
  return p_priv->p_chain;
}


static void ota_api_debug_printf(ota_debug_level_t level,const char *p_fmt, ...)
{

  ota_priv_t *p_priv =  NULL;
  char buffer[MAX_PRINT_BUFFER_LEN] = {0};
  p_print_list p_args = NULL;
  p_priv = ota_api_get_priv();

  if(p_priv->debug_level < OTA_DEBUG_ALL)
  {
    return;
  }
  if(level >= p_priv->debug_level)
  {
     if(p_priv->ota_api_printf != NULL)
      {
        PRINTF_VA_START(p_args, p_fmt);
        ck_vsnprintf(buffer,sizeof(buffer), p_fmt, p_args);
        PRINTF_VA_END(p_args);
        p_priv->ota_api_printf("%s", buffer);
      }
  }
  return;
}



/*!
 ota api do cmd to filter.
 */
static void ota_api_do_cmd_to_filter(ifilter_t *p_filter,
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

static void ota_api_process_extern_evt(BOOL is_send,u32 msg_id, u32 para1, u32 para2,u32 context)
{
    ota_priv_t *p_priv =  NULL;
    p_priv = ota_api_get_priv();
   if((NULL != p_priv->evt_process) && (is_send == TRUE))
    {
      p_priv->evt_process(msg_id,para1,para2,context);
    }
}
/*!
 ota api process event.
 */
static BOOL ota_api_on_process_evt(handle_t _this, os_msg_t *p_msg)
{
  
  u32 msg_id = 0;
  BOOL extern_process_bit = FALSE; 
  switch(p_msg->content)
  {
    case OTA_FILTER_EVENT_LOCK :
       msg_id = OTA_API_EVENT_LOCK;
       extern_process_bit = TRUE;
    break;
    case OTA_FILTER_EVENT_UNLOCK :
      msg_id = OTA_API_EVENT_UNLOCK;
      extern_process_bit = TRUE;
     break;
    case OTA_FILTER_EVENT_CHECK_FAIL :
      msg_id = OTA_API_EVENT_CHECK_FAIL;
      extern_process_bit = TRUE;
      break;
     case OTA_FILTER_EVENT_CHECK_SUCCESS:
      msg_id = OTA_API_EVENT_CHECK_SUCCESS;
      extern_process_bit = TRUE;
      break;
      case OTA_FILTER_EVENT_DOWNLOAD_FAIL:
       msg_id = OTA_API_EVENT_DOWNLOAD_FAIL;
       extern_process_bit = TRUE;
      break;
      case OTA_FILTER_EVENT_DOWNLOAD_START:
      msg_id = OTA_API_EVENT_DOWNLOAD_START;
      extern_process_bit = TRUE;
      break;
      case OTA_FILTER_EVENT_DOWNLOAD_ING:
       msg_id = OTA_API_EVENT_DOWNLOAD_ING;
       extern_process_bit = TRUE;
      break;
      case OTA_FILTER_EVENT_DOWNLOAD_TIMEOUT:
       msg_id = OTA_API_EVENT_DOWNLOAD_TIMEOUT;
       extern_process_bit = TRUE;
      break;
     case OTA_FILTER_EVENT_DOWNLOAD_SUCCESS:
      msg_id = OTA_API_EVENT_DOWNLOAD_SUCCESS;
      extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_REQEUST_TIMEOUT: 
      msg_id = OTA_API_REQUEST_TIMEOUT;
      extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_START:
     msg_id = OTA_API_EVENT_BURNFLASH_START;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_BLOCK:
     msg_id = OTA_API_EVENT_BURNFLASH_BLOCK;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_ING:
     msg_id = OTA_API_EVENT_BURNFLASH_ING;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME:
     msg_id = OTA_API_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_STATUS:
     msg_id = OTA_API_EVENT_BURNFLASH_SAVE_OTA_STATUS;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_DSTROY_FLAGE:
     msg_id = OTA_API_EVENT_BURNFLASH_SAVE_OTA_DSTROY_FLAGE;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SAVE_CURR_OTA_ID:
     msg_id = OTA_API_EVENT_BURNFLASH_SAVE_CURR_OTA_ID;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SAVE_BLOCK_VERSION_INFO:
     msg_id = OTA_API_EVENT_BURNFLASH_SAVE_BLOCK_VERSION_INFO;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID:
     msg_id = OTA_API_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_RESET_SYS_DATA_BLOCK:
      msg_id = OTA_API_EVENT_BURNFLASH_RESET_SYS_DATA_BLOCK;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_RESET_DB_DATA_BLOCK:
      msg_id = OTA_API_EVENT_BURNFLASH_RESET_DB_DATA_BLOCK;
      extern_process_bit = TRUE;
      break;
      case OTA_FILTER_EVENT_BURNFLASH_RESET_CA_DATA_BLOCK:
      msg_id = OTA_API_EVENT_BURNFLASH_RESET_CA_DATA_BLOCK;
      extern_process_bit = TRUE;
      break;
      case OTA_FILTER_EVENT_BURNFLASH_RESET_ADS_DATA_BLOCK:
      msg_id = OTA_API_EVENT_BURNFLASH_RESET_ADS_DATA_BLOCK;
      extern_process_bit = TRUE;
      break;
     case OTA_FILTER_EVENT_BURNFLASH_FACTORY:
     msg_id = OTA_API_EVENT_BURNFLASH_FACTORY;
     extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SUCCESS:
      msg_id = OTA_API_EVENT_BURNFLASH_SUCCESS;
      extern_process_bit = TRUE;
      break;
     case OTA_FILTER_EVENT_BURNFLASH_FAIL: 
      msg_id = OTA_API_EVENT_BURNFLASH_FAIL;
      extern_process_bit = TRUE;
      break;
     case OTA_FILTER_EVENT_BURNFLASH_REBOOT: 
      msg_id = OTA_API_EVENT_FINISH_REBOOT;
      extern_process_bit = TRUE;
     break;
     case OTA_FILTER_EVENT_BURNFLASH_SAVE_UPG_VERSION:
      msg_id = OTA_API_EVENT_BURNFLASH_SAVE_UPG_VERSION;
      extern_process_bit = TRUE;
      break;
    case OTA_FILTER_EVENT_NO_BLOCK_UPG:
      msg_id = OTA_API_EVENT_NO_BLOCK_UPG;
      extern_process_bit = TRUE;
     break;
    case OTA_FILTER_EVENT_STOP_RESULT:
     msg_id = OTA_API_EVENT_STOP_RESULT;
     extern_process_bit = TRUE;
     break;
    default :
      ota_api_debug_printf(OTA_DEBUG_ASSERT,"[%s]:event is err\n",__FUNCTION__);  
      extern_process_bit = FALSE;
      break;
  }

  ota_api_process_extern_evt(extern_process_bit,msg_id,p_msg->para1,p_msg->para2,p_msg->context); 
  return TRUE;
}



/*!
 * Create controller
 */
static RET_CODE ota_api_create_controller(ota_priv_t *p_priv)
{

  ctrl_para_t   ctrl_para    = {0};
  controller_t *p_controller = NULL;

  ctrl_para.user_handle = p_priv;

  p_controller = controller_create(&p_priv->controller, &ctrl_para);

  if(NULL == p_controller)
  {
      //OS_PRINTF("[ota_api]:controller is NULL\n");
      return ERR_FAILURE;
  }
  p_priv->controller.on_process_evt = ota_api_on_process_evt;
  
  return SUCCESS;
}
/*!
 * ota connect pin
 */
static RET_CODE ota_api_connect_pin(ota_priv_t *p_priv)
{
  RET_CODE        ret       = ERR_FAILURE;
  ifilter_t      *p_filter  = NULL;
  #ifndef OTA_FILTER_USE_FLASH_FILTER
  ipin_t         *p_ota_pin = NULL;
  #endif
  media_format_t format = {MT_PSI};
  
  p_filter = p_priv->p_dmx_filter;
  ret = p_filter->get_unconnect_pin(p_filter, OUTPUT_PIN, &p_priv->p_dmx_pin);
  CHECK_FAIL_RET_CODE(p_priv->p_dmx_pin != NULL);

  /*!
   * Set dmx pin type, the pin type is psi pin
   */
  ota_api_do_cmd_to_filter(p_priv->p_dmx_filter,
                       DMX_SET_PIN_TYPE,
                       DMX_PIN_PSI_TYPE,
                       (u32 *)p_priv->p_dmx_pin);

  #ifdef OTA_FILTER_USE_FLASH_FILTER
    //1. >>-------------link dmx to ota filter --------->>>>
  format.stream_type = MT_PSI;
  ret = p_priv->p_chain->connect(p_priv->p_chain,
  p_priv->p_dmx_filter, p_priv->p_ota_filter, &format);
  if(ret != SUCCESS)
  {
    p_priv->p_chain->_interface._destroy(p_priv->p_chain);
    CHECK_FAIL_RET_CODE(ret == SUCCESS);
    return ret;
  }

   //2. >>-------------link dmx to ota filter --------->>>>
  format.stream_type = MT_FILE_DATA;
  ret = p_priv->p_chain->connect(p_priv->p_chain,
  p_priv->p_ota_filter, p_priv->p_flash_filter, &format);
  if(ret != SUCCESS)
  {
    p_priv->p_chain->_interface._destroy(p_priv->p_chain);
    CHECK_FAIL_RET_CODE(ret == SUCCESS);
    return ret;
  }
  #else
  /*!
   * Get unconnect input pin of epg
   */
  ret = p_filter->get_unconnect_pin(p_priv->p_ota_filter, INPUT_PIN, &p_ota_pin);
  CHECK_FAIL_RET_CODE(p_ota_pin != NULL);

    /*!
   * Connect output pin and input pin
   */
  format.stream_type = MT_PSI;
  ret = p_priv->p_chain->connect_pin(p_priv->p_chain,
                                     p_priv->p_dmx_pin,
                                     p_ota_pin,
                                     &format);
  CHECK_FAIL_RET_CODE(ret == SUCCESS);
  #endif
  return ret;
}

/*!
 * ota add filters to chain.
 */
static RET_CODE ota_api_add_filters_to_chain(ota_priv_t *p_priv)
{
  chain_t *p_chain = p_priv->p_chain;
  RET_CODE ret = SUCCESS;

  CHECK_FAIL_RET_CODE(p_chain != NULL);

  //add dmx filters to chain
  ret = p_chain->add_filter(p_chain, p_priv->p_dmx_filter, "dmx filter");
  CHECK_FAIL_RET_CODE(ret == SUCCESS);
  if(ret == ERR_FAILURE)
  {
     return ret;
  }

  //add ota filters to chain
  ret = p_chain->add_filter(p_chain, p_priv->p_ota_filter, "ota filter");
  CHECK_FAIL_RET_CODE(ret == SUCCESS);
  if(ret == ERR_FAILURE)
  {
     return ret;
  }

  #ifdef OTA_FILTER_USE_FLASH_FILTER
    //add ota filters to chain
  ret = p_chain->add_filter(p_chain, p_priv->p_flash_filter, "flash filter");
  CHECK_FAIL_RET_CODE(ret == SUCCESS);
  if(ret == ERR_FAILURE)
  {
     return ret;
  }
  #endif
  return ret;
}

/*!
 *epg create filters(dmx filter + ota filter + flash filter)
 */
static RET_CODE ota_api_create_filters(ota_priv_t *p_priv)
{
  RET_CODE ret = ERR_FAILURE;

  //add demux filter
  ret = eva_add_filter_by_id(DEMUX_FILTER, &p_priv->p_dmx_filter, NULL);
  CHECK_FAIL_RET_CODE(p_priv->p_dmx_filter != NULL);
  if(ret == ERR_FAILURE)
  {
     return ret;
  }
  
  //add ota filter.
  ret = eva_add_filter_by_id(OTA_FILTER, &p_priv->p_ota_filter, NULL);
  CHECK_FAIL_RET_CODE(p_priv->p_ota_filter != NULL);
  if(ret == ERR_FAILURE)
  {
     return ret;
  }
  
  #ifdef OTA_FILTER_USE_FLASH_FILTER
    //add flash filter.
  ret = eva_add_filter_by_id(FLASH_SINK_FILTER, &p_priv->p_flash_filter, NULL);
  CHECK_FAIL_RET_CODE(p_priv->p_flash_filter != NULL);
  if(ret == ERR_FAILURE)
  {
     return ret;
  }
  #endif
  return ret;
}

/*!
  ota create chain

  \param[in] p_priv : ota api private data.
  \param[in] task_priority : task priority for chain.
  \param[in] task_stk_size : task stack size.
  */
static RET_CODE ota_api_create_chain(ota_priv_t *p_priv,u32 task_stk_size)
{

  chain_para_t  chain_para = { 0 };

  RET_CODE ret = ERR_FAILURE;

  //create controller
  ret =  ota_api_create_controller(p_priv);
  if(ERR_FAILURE == ret)
  {
    return ERR_FAILURE;
  }

  //create chain
  chain_para.p_owner  = (interface_t *)&p_priv->controller;
  chain_para.p_name   = "ota_chain";
  chain_para.stk_size = task_stk_size;
  
  p_priv->p_chain = chain_create(&chain_para);
  if(NULL == p_priv->p_chain)
  {
      return ERR_FAILURE;
  }

  return SUCCESS;
}

/*!
 ota api destroy chain.
 */
RET_CODE ota_api_destroy_chain(void)
{
  chain_t *p_chain = ota_api_get_chain();

  p_chain->_interface._destroy(p_chain);

  return SUCCESS;
}
/*!
 ota api start .
 */
static void ota_api_start(void)
{
  chain_t *p_chain = ota_api_get_chain();
  ota_priv_t *p_priv = ota_api_get_priv();
  RET_CODE ret     = ERR_FAILURE;
  if(p_priv->ota_start_bit == FALSE)
  {
    ret = p_priv->p_chain->open(p_priv->p_chain);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);
    ret = p_chain->start(p_chain);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);
    p_priv->ota_start_bit = TRUE;
   }
}
/*!
 ota api stop .
 */
 static void ota_api_stop(void)
{
  chain_t *p_chain = ota_api_get_chain();
  ota_priv_t *p_priv = ota_api_get_priv();
  RET_CODE ret     = ERR_FAILURE;
  if(p_priv->ota_start_bit == TRUE)
  {
    ret = p_chain->stop(p_chain);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);  
    ret = p_priv->p_chain->close(p_priv->p_chain);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);
    p_priv->ota_start_bit = FALSE;
  }
}

/*!
  ota api init
  */
RET_CODE mul_ota_api_init(ota_para_t *p_para)
{
  ota_priv_t *p_priv = NULL;
  RET_CODE ret = ERR_FAILURE;

  if(class_get_handle_by_id(OTA_API_CLASS_ID) != NULL)
  {
    return SUCCESS;
  }

  CHECK_FAIL_RET_CODE(p_para != NULL);

  //alloc ota api private data.
  p_priv = (ota_priv_t *)mtos_malloc(sizeof(ota_priv_t));
  CHECK_FAIL_RET_CODE(p_priv != NULL);
  memset(p_priv,0,sizeof(ota_priv_t));
  class_register(OTA_API_CLASS_ID, (class_handle_t)p_priv);

  p_priv->evt_process = p_para->evt_process;
  p_priv->ota_start_bit = FALSE;

 /*!
  ota create chain
  */
  ret = ota_api_create_chain(p_priv, p_para->task_stk_size);
   if(ERR_FAILURE == ret)
  {
      return ERR_FAILURE;
  }

 /*!
  ota create filters
  */
  ret =  ota_api_create_filters(p_priv);
  if(ERR_FAILURE == ret)
  {
      return ERR_FAILURE;
  }

 /*!
  ota add filters to chain
  */
  ret = ota_api_add_filters_to_chain(p_priv);
  if(ERR_FAILURE == ret)
  {
      return ERR_FAILURE;
  }

 /*!
  ota connect pin.
  */
  ret = ota_api_connect_pin(p_priv);
  if(ERR_FAILURE == ret)
  {
      return ERR_FAILURE;
  }

   p_priv->medium = p_para->medium;
   p_priv->protocol_type = p_para->protocol_type;
   
  return SUCCESS;
}

/*!
  ota api config
  */
RET_CODE mul_ota_api_config(ota_config_t *p_cfg)
{
   ota_priv_t *p_priv = ota_api_get_priv();
   ota_filter_cfg_t filter_cgf = {0};
   psi_pin_cfg_t psi_cfg = {0};
   
   p_priv->debug_level = p_cfg->debug_level;
   p_priv->ota_api_printf = p_cfg->ota_debug_printf;
  
   if(p_priv->medium == OTA_MEDIUM_BY_TUNER)
     {
        if(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC)
       {
          psi_cfg.max_table_num = 4;
       }
       else
       {
           psi_cfg.max_table_num = 2;
       }
      //fix demux_index here
       psi_cfg.demux_index = p_cfg->demux_index;
       ota_api_do_cmd_to_filter(p_priv->p_dmx_filter,
                            DMX_CFG_PARA,
                            (u32)&psi_cfg,
                            (u32 *)p_priv->p_dmx_pin);
     }
   
   filter_cgf.medium = p_priv->medium;
   filter_cgf.protocol_type = p_priv->protocol_type ;
   
   memcpy(&filter_cgf.filter_cfg,p_cfg,sizeof(ota_config_t));

   ota_api_do_cmd_to_filter(p_priv->p_ota_filter,
             OTA_FILTER_CMD_CONFIG,
             0,
             (u32 *)&filter_cgf);
   
  return SUCCESS;
}

/*!
  ota api stop
  */
void mul_ota_api_stop(void)
{
   ota_api_stop();
}
/*!
  ota api destroy
  */
RET_CODE mul_ota_api_destroy(void)
{
  ota_priv_t *p_priv = ota_api_get_priv();
  ota_api_destroy_chain();
  mtos_free(p_priv);
  p_priv = NULL;
  class_register(EPG_API_CLASS_ID, NULL);
  return SUCCESS;
}

/*!
  ota api config and start
  */
void mul_ota_api_start(void)
{
   ota_api_start();
}

/*!
  ota select policy and case
  */
void mul_ota_api_select_policy(ota_api_policy_t *policy)
{
   ota_priv_t *p_priv = ota_api_get_priv();
   u32 *p_policy_param = NULL;
   if(p_priv->ota_start_bit != TRUE)
    {
      return;
    }
   if(p_priv->medium == OTA_MEDIUM_BY_TUNER)
    {
      switch(policy->run_select)
        {
           case OTA_WORK_SELECT_AUTO:
            p_policy_param = (u32 *)&policy->tp_info;;
           break;
           case OTA_WORK_SELECT_CHECK:
            p_policy_param = (u32 *)&policy->tp_info;
           break;
           case OTA_WORK_SELECT_DOWNLOAD:
            p_policy_param = (u32 *)&policy->upg_check_info;
           break;
           case OTA_WORK_SELECT_BURNFLASH:
            p_policy_param = (u32 *)policy->ota_block;
           break;
           case OTA_WORK_SELECT_UPGGRADE:
            p_policy_param = (u32 *)policy->ota_block;
           break;
           case OTA_WORK_SELECT_MONITOR:
            p_policy_param = (u32 *)&policy->monitor_para;
           break;
           case OTA_WORK_SELECT_INQUIRE_STOP: /****don't process when burn flash*****/
            p_policy_param = NULL;
           break;
           default:
           break;
        }
      ota_api_do_cmd_to_filter(p_priv->p_ota_filter,
                         OTA_FILTER_CMD_DO_POLICY,
                         (u32)policy->run_select,
                         (u32 *)p_policy_param); 
    }

}
/*!
  api process nim lock status result
  */
void mul_ota_api_process_nim_lock_status(BOOL is_locked)
{
   ota_priv_t *p_priv = ota_api_get_priv();
   u32 result = 0;
   
   if(is_locked == TRUE)
    {
     result = OTA_FILTER_LOCK_STATUS_LOCKED;
    }
   else
    {
      result = OTA_FILTER_LOCK_STATUS_UNLOCK;
    }

   ota_api_do_cmd_to_filter(p_priv->p_ota_filter,
                   OTA_FILTER_CMD_LOCK_PROCESS,
                   result,NULL); 
}



