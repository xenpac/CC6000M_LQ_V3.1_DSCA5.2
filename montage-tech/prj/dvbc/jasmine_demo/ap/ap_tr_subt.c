/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std headers
#include  "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_fifo.h"
#include "mtos_task.h"
#ifdef TR_HANGAD
#include "mtos_sem.h"
#endif
// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "lib_util.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "subt_pic_ware.h"
// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "monitor_service.h"

// ap headers
#include "ap_framework.h"
#include "ap_tr_subt.h"
#ifdef TR_HANGAD
#include "subt_pic_adapter.h"
//#include "ads_funtion_public.h"
#endif

#ifdef TENGRUI_ROLLTITLE
#include "subt_pic_ware.h"
#include "ui_roll_title.h"
#endif

typedef enum
{
  SP_SM_IDLE,
  SP_SM_START,
  SP_SM_STOP,
  SP_SM_CLOSE,
}subt_state_t;

typedef struct
{
  subt_policy_t *p_policy;
  app_t instance;
  subt_state_t state;
}subt_priv_t;

static void subt_pic_send_evt_to_ui(u32 evt_id, u32 para1, u32 para2)
{
  event_t evt = {0, 0, 0};

  evt.id = evt_id;
  evt.data1 = para1;
  evt.data2 = para2;

  ap_frm_send_evt_to_ui(APP_RESERVED2, &evt);
}


static void subt_process_msg(handle_t handle, os_msg_t *p_msg)
{
  subt_priv_t  *p_priv = (subt_priv_t *)handle;
  switch(p_msg->content)
  {
    case SUBT_PIC_CMD_START:
      p_priv->state = SP_SM_START;
      OS_PRINTF("\n SUBT_PIC_CMD_START######\n");
      subt_pic_open(p_msg->para1);
      break;
    case SUBT_PIC_CMD_START_SYNC:
      p_priv->state = SP_SM_START;
      subt_pic_open(p_msg->para1);
      subt_pic_send_evt_to_ui(SUBT_PIC_EVT_START, 0, 0);
      break;
    case SUBT_PIC_CMD_STOP_SYNC:
      p_priv->state = SP_SM_STOP;
      OS_PRINTF("\n SUBT_PIC_CMD_STOP_SYNC######\n");
      subt_pic_stop(p_msg->para1);
      subt_pic_send_evt_to_ui(SUBT_PIC_EVT_STOP, 0, 0);
      break;
    case SUBT_PIC_CMD_CLOSE:
      p_priv->state = SP_SM_CLOSE;
      subt_pic_close(p_msg->para1);
      break;
   
    default:
      break;    
  }
}
#ifdef TR_HANGAD
extern void ui_trhangad_process(void);
BOOL g_tr_hangad = FALSE; //重新收挂角广告数据
extern void ui_set_trhangad_show(BOOL flag);
extern BOOL hangad_finish_flag;
#endif


static void subt_state_machine(handle_t handle)
{
  subt_priv_t  *p_priv = (subt_priv_t *)handle;
  #ifdef TR_HANGAD
  subt_pic_event_info_t event_info;
  subt_pic_event_id_t event_id;
  subt_pic_module_id_t module_id = SUBT_PIC_ID_ADT_TR;
  //解析腾锐挂角广告消息
  if(g_tr_hangad != TRUE)
  {
    //OS_PRINTF("\n subt_pic_event_parse######\n");
    subt_pic_event_polling(&event_info);
    event_id = subt_pic_event_parse(event_info.events[module_id]);
    if(event_id == SUBT_PIC_S_HANGADSERACH_FINISHED)
    {
      OS_PRINTF("\n $$$$$$$$$$$got the hangad data######\n");
      g_tr_hangad = TRUE;
      hangad_finish_flag = TRUE;
    }
    else
    {
      OS_PRINTF("subt sm polling\n");
    }
  }
   else
   {
     //OS_PRINTF("\n ui_hangad_process######\n");
     ui_trhangad_process();
   }
  #endif
  switch(p_priv->state)
  {
    default:
      break;
  }
  mtos_task_sleep(3000);
}

static void init(handle_t handle)
{


}

app_t *construct_ap_tr_subt(subt_policy_t *p_policy)
{
  subt_priv_t *p_priv = mtos_malloc(sizeof(subt_priv_t));
  MT_ASSERT(NULL != p_priv);
  memset(p_priv, 0, sizeof(subt_priv_t));
  
  MT_ASSERT(p_policy != NULL);

  p_priv->state = SP_SM_IDLE;

  p_priv->p_policy = p_policy;

  p_priv->instance.init = init;
  p_priv->instance.process_msg = subt_process_msg;
  p_priv->instance.state_machine = subt_state_machine;
  p_priv->instance.get_msgq_timeout = NULL;
  p_priv->instance.p_data = (void *)p_priv;
  return &p_priv->instance;
}
