/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_event.h"
#include "mtos_sem.h"

#include "ads_ware.h"
#include "ads_adapter.h"



ads_adapter_priv_t g_ads_priv = {0};

RET_CODE ads_event_notify_register(ads_event_notify_t *p_notify)
{
  memcpy(&g_ads_priv.notify, p_notify, sizeof(ads_event_notify_t));

  return SUCCESS;
}

void ads_clr_events(u32 adm_id)
{
  memset(g_ads_priv.events[adm_id], 0x00, AD_EVT_U32_LEN * 4);
  g_ads_priv.event_param[adm_id] = 0;
}

//发送事件，并把存入全局中
void ads_send_event(u32 adm_id, u32 event, u32 param)
{
  u32 events[AD_EVT_U32_LEN];
  
  if(event == 0)
  {
    return;
  }

  if(g_ads_priv.notify.evt_cb != NULL)
  {
    memset(events, 0x00, AD_EVT_U32_LEN * 4);
    set_bit(event, events);
    g_ads_priv.notify.evt_cb(adm_id, events, param, g_ads_priv.notify.context);
  }
    
  mtos_sem_take(&g_ads_priv.evt_mutex, 0);
  set_bit(event - 1, g_ads_priv.events[adm_id]);
  g_ads_priv.event_param[adm_id] = param;
  mtos_sem_give(&g_ads_priv.evt_mutex);
}

RET_CODE ads_event_polling(ads_event_info_t *p_event_info)
{
  u32 i = 0;

  memset(p_event_info, 0x00, sizeof(ads_event_info_t));
  mtos_sem_take(&g_ads_priv.evt_mutex, 0);
  for(i = 0; i < ADS_ID_ADT_MAX_NUM; i++)
  {
      if(g_ads_priv.event_param[i] != 0)
      {
        memcpy(p_event_info->events[i],g_ads_priv.events[i], AD_EVT_U32_LEN * 4);
      //p_event_info->happened[i] = 1;
        memset(g_ads_priv.events[i], 0x00, AD_EVT_U32_LEN * 4);
        g_ads_priv.event_param[i] = 0;
      }
  }
  mtos_sem_give(&g_ads_priv.evt_mutex);
  return SUCCESS;
}

ads_event_id_t ads_event_parse(u32 events[AD_EVT_U32_LEN])
{
  u32 i = 0;

  for(i = 0; i < (AD_EVT_U32_LEN << 4); i++)
  {
    if(test_and_clear_bit(i, events) == 1)
    {
      return (ads_event_id_t)(i + 1);
    }
  }

  return ADS_EVT_NONE;
}

RET_CODE ads_detach(u32 adm_id)
{
  ads_op_t *p_op = (ads_op_t *)&g_ads_priv.adm_op[adm_id];

  if(p_op->inited == 1)
  {
    p_op->func.deinit();
  }   
  p_op->inited = 0;
  
  
  if(p_op->p_priv != NULL)
  {
    mtos_free(p_op->p_priv);
    p_op->p_priv = NULL;
  }
  p_op->attached = 0;
  
  return SUCCESS;
}

ads_adapter_priv_t *ads_get_adapter_priv(void)
{
  return &g_ads_priv;
}

RET_CODE ads_init(u32 adm_id, ads_adapter_cfg_t *p_param)
{
  RET_CODE ret = ERR_FAILURE;

  if(g_ads_priv.inited == 1)
  {
    ret = SUCCESS;
    return ret;
  }
  g_ads_priv.inited = 1;
  MT_ASSERT(TRUE == mtos_sem_create(&g_ads_priv.evt_mutex, TRUE));
  ads_clr_events(adm_id);
  return SUCCESS;
}

RET_CODE ads_deinit(u32 adm_id)
{
  if(g_ads_priv.inited == 1)
  {
    MT_ASSERT(TRUE == mtos_sem_destroy(&g_ads_priv.evt_mutex, 0));
  }
  g_ads_priv.inited = 0;
  ads_clr_events(adm_id);
  return g_ads_priv.adm_op[adm_id].func.deinit();
}

RET_CODE ads_module_init(ads_module_id_t cam_id)
{
  ads_op_t *p_op = NULL;

  p_op = &g_ads_priv.adm_op[cam_id];
  if(p_op->inited == 1)
  {
    return SUCCESS;
  }

  p_op->inited = 1;
  if(p_op->func.init == NULL)
  {
    return SUCCESS;
  }
  return p_op->func.init(NULL);
}

RET_CODE ads_module_deinit(ads_module_id_t cam_id)
{
  ads_op_t *p_op = NULL;

  p_op = &g_ads_priv.adm_op[cam_id];
  if(p_op->inited == 0)
  {
    return SUCCESS;
  }

  p_op->inited = 0;
  if(p_op->func.deinit == NULL)
  {
    return SUCCESS;
  }
  return p_op->func.deinit();
}

RET_CODE ads_open(u32 adm_id)
{
  return g_ads_priv.adm_op[adm_id].func.open();
}

RET_CODE ads_close(u32 adm_id)
{
  return g_ads_priv.adm_op[adm_id].func.close();
}

RET_CODE ads_display(u32 adm_id, ads_info_t *p_param)
{
  if( g_ads_priv.adm_op[adm_id].func.display)
  {
  	return g_ads_priv.adm_op[adm_id].func.display(p_param);
  }
  return ERR_FAILURE;
}

RET_CODE ads_hide(u32 adm_id, ads_info_t *p_param)
{
  return g_ads_priv.adm_op[adm_id].func.hide(p_param);
}

RET_CODE ads_io_ctrl(u32 adm_id, u32 cmd, void *p_param)
{
  if(g_ads_priv.adm_op[adm_id].attached != 1)
  {
    return ERR_STATUS;
  }
  return g_ads_priv.adm_op[adm_id].func.io_ctrl(cmd, p_param);
}
