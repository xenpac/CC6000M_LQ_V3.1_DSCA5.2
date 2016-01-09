/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_fifo.h"


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
#include "mt_time.h"

#include "mtos_msg.h"
#include "mtos_sem.h"

#include "ap_framework.h"
#include "browser_adapter.h"
#include "ap_browser.h"

#define BROWSER_FIFO_SIZE  256
/*!
  Browser event struct
  */
typedef struct
{
/*!
  browser slot id
  */
  u8 slot_id;
/*!
  browser module id
  */
  u8 browser_id;
/*!
  browser event
  */
  u16 event;
/*!
  browser param
  */
  u32 param;
}ap_browser_evt_t;

/*!
   browser private information
  */
typedef struct
{
  /*
    browser app
  */ 
  app_t browser_app;
  /*!
  dvb sevice handle
  */
  service_t *p_dvb_svc;
  /*!
    NIM service handle
    */
  service_t *p_nim_svc;  
   /*!
    Browser policy
    */
  browser_policy_t *p_policy;
  os_fifo_t rcv_fifo;
  u16 rcv_buf[BROWSER_FIFO_SIZE];
  u16 evt_index;
  ap_browser_evt_t evt[BROWSER_FIFO_SIZE];
   
}browser_priv_t;
#define BROWSER_MAIN_FREQ  456000
#define BROWSER_SYMBOL_RATE  6875
static RET_CODE browser_events_get(handle_t handle, ap_browser_evt_t *p_event)
{
  browser_priv_t *p_this = (browser_priv_t *)handle;
  u16 index = 0;
  RET_CODE ret = ERR_FAILURE;
  MT_ASSERT(NULL != p_this);
  ret = mtos_fifo_get(&p_this->rcv_fifo, &index);
  if (ret == TRUE)
  {
    p_event->browser_id = p_this->evt[index].browser_id;
    p_event->slot_id = p_this->evt[index].slot_id;
    p_event->event = p_this->evt[index].event;
    p_event->param = p_this->evt[index].param;

    return SUCCESS;
  }

  return ERR_FAILURE;
}

static RET_CODE browser_notify_callback(u32 slot, u32 event, u32 param, u32 
                              browser_id, u32 context)
{
  browser_priv_t *p_this = (browser_priv_t *)context;
  ap_browser_evt_t *p_evt = NULL;

  MT_ASSERT(NULL != p_this);

  if(p_this->evt_index == BROWSER_FIFO_SIZE)
  {
    p_this->evt_index = 0;
  }
  p_evt = &p_this->evt[p_this->evt_index];

  p_evt->browser_id = browser_id;
  p_evt->event = event;
  p_evt->param = param;
  p_evt->slot_id = slot;

  mtos_fifo_put(&p_this->rcv_fifo, p_this->evt_index);
  p_this->evt_index ++;
  return SUCCESS;
}

static void send_msg_to_ui(u32 id)
{
  event_t evt = {0};
  evt.id = id;
  ap_frm_send_evt_to_ui(APP_SI_MON, &evt);
}
#if 1
static void browser_lock_tuner(browser_priv_t *p_priv, u32 freq, u32 sym, u32 mod)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  nc_channel_info_t nc_channel_info;
  u8 tuner_id = 0;
  u8 ts_id = 0;
  memset(&nc_channel_info, 0, sizeof(nc_channel_info_t));
  
  nc_channel_info.channel_info.frequency = freq;
  nc_channel_info.channel_info.param.dvbc.symbol_rate = sym;
  nc_channel_info.channel_info.param.dvbc.modulation = NIM_MODULA_QAM64;
  nc_channel_info.polarization = NIM_PORLAR_HORIZONTAL;  
  nc_channel_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
  nc_channel_info.channel_info.lock = NIM_CHANNEL_UNLOCK;

  OS_PRINTF("\n**** browser dvbc_lock_tuner, freq %d, sym %d, mod %d\n", freq, sym, mod);
  nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_id);
  nc_set_tp(nc_handle, tuner_id, p_priv->p_nim_svc,
                  &nc_channel_info);
}
#endif
extern bowser_adapter_priv_t g_browser_priv;
RET_CODE browser_event_notify_register(browser_event_notify_t *p_notify)
{
  memcpy(&g_browser_priv.notify, p_notify, sizeof(browser_event_notify_t));
  return SUCCESS;
}
extern RET_CODE browser_io_ctrl(u32 adm_id, u32 cmd, void *p_param);
static void browser_single_step(void *p_handle, os_msg_t *p_msg)
{
  browser_priv_t *p_this = (browser_priv_t *)p_handle;
  browser_state_t e_browser_state =   BROWSER_STATE_UNKNOWN;
  browser_io_param_t browser_param = {0};
  browser_module_id_t browser_id = BROWSER_UNKNOWN;
  browser_event_id_t browser_event = BROWSER_EVT_NONE;
  ap_browser_evt_t event = {0};
  event_t evt = {0, 0, 0};
  //msg process
  if(p_msg != NULL)
  {  
    switch(p_msg->content)
    {
      case CMD_START_BROWSER:
        p_this->p_policy->on_init();//attach
              #if 1
            //  p_browser_tp = (browser_tp_info_t *)p_msg->para2;
              browser_lock_tuner(p_handle, (u32)476000, 
                                        (u32)6875,64);
              #endif
              browser_param.mem_size = p_msg->para2;
         
      //  browser_param.p_param = (void*)p_msg->para1;
   //     browser_io_ctrl(0,CMD_START_BROWSER,&browser_param);
        break;
        case NC_EVT_LOCKED:
           browser_io_ctrl(0,CMD_START_BROWSER,&browser_param);
          break;
          case NC_EVT_UNLOCKED:
      case CMD_STOP_BROWSER: 
        send_msg_to_ui(BROWSER_EVT_STOP); //sync cmd must give a feedback msg
        OS_PRINTF("cmd stop browser!\n");
        break;
      case CMD_SEND_IRKEY_MSG:
        OS_PRINTF("\n ipanel_key msg is %d",p_msg->para1);
        browser_id = (p_msg->para2 & 0xFF);
        browser_param.browser_id = browser_id;
        browser_param.key_event= p_msg->para1;
        browser_io_ctrl(0,CMD_SEND_IRKEY_MSG,&browser_param);
        break;
      default:
        OS_PRINTF("cmd unknown!\n");
        break;
    }
  }
  #ifndef WIN32
  if(SUCCESS == browser_events_get(p_this, &event))
 {
    browser_event = event.event;
    //OS_PRINTF("CAS: parse event[%d]\n", cas_event);
    browser_param.browser_id= event.browser_id;
    
    switch(browser_event)
    {
      case BROWSER_O_PLAY_PROGRAM_FROM_SERVICE_ID:
 //      evt.id = BROWSER_EVT_PALY_MUSIC;
        evt.data1 = (event.slot_id << 16) | browser_param.browser_id;
        evt.data2 = event.param;
        break;
        
      case BROWSER_O_STOP_PROGRAM:
        //evt.id = BROWSER_EVT_STOP_MUSIC;
        evt.data1 = (event.slot_id << 16) | browser_param.browser_id;
        evt.data2 = event.param;
        break;
        
      case BROWSER_O_EXIT:
        evt.id = BROWSER_EVT_EXIT;
        evt.data1 = (event.slot_id << 16) | browser_param.browser_id;
        evt.data2 = event.param;
        break;
      default:
         break;
    }
    ap_frm_send_evt_to_ui(APP_SI_MON, &evt);
}
#endif

  //state machine  
  switch(e_browser_state)
  {
      case BROWSER_STATE_TEST:        
        break;
      case BROWSER_STATE_UNKNOWN:
        break;
      default:
        break;
  }
}


static void init(void *p_handle)
{
  browser_priv_t *p_browser_priv = (browser_priv_t *)p_handle;
  dvb_t *p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);
  nim_ctrl_t *p_nc = class_get_handle_by_id(NC_CLASS_ID);

  p_browser_priv->p_dvb_svc = p_dvb_handle->get_svc_instance(p_dvb_handle, APP_SI_MON);
  p_browser_priv->p_nim_svc = nc_get_svc_instance(p_nc, APP_SI_MON);
}

app_t *construct_ap_browser(browser_policy_t *p_policy)
{
  browser_priv_t *p_browser_priv = mtos_malloc(sizeof(browser_priv_t));
  browser_event_notify_t notify = {0};
  MT_ASSERT(p_browser_priv != NULL);
  memset(p_browser_priv, 0, sizeof(browser_priv_t));
  //memset(&(p_browser_priv->browser_app), 0, sizeof(app_t));
  p_browser_priv->p_policy = p_policy;
  //Attach browser instance
  p_browser_priv->browser_app.get_msgq_timeout = NULL;
  p_browser_priv->browser_app.init = init;
  p_browser_priv->browser_app.task_single_step = browser_single_step;
  p_browser_priv->browser_app.p_data = (void *)p_browser_priv;

  p_browser_priv->rcv_fifo.p_buffer = p_browser_priv->rcv_buf;
  p_browser_priv->rcv_fifo.m_size = BROWSER_FIFO_SIZE;
  p_browser_priv->rcv_fifo.m_overlay = TRUE;
  //p_browser_priv->rcv_fifo.m_cnt =0;
  
  notify.context = (u32)p_browser_priv;
  notify.evt_cb = browser_notify_callback;
  browser_event_notify_register(&notify);
  return &(p_browser_priv->browser_app);
}

