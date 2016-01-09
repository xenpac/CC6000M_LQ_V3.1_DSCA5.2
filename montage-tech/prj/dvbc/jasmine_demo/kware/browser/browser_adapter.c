/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_event.h"
#include "mtos_sem.h"

#include "mtos_msg.h"
#include "ap_framework.h"
#include "browser_adapter.h"
#include "ap_browser.h"
bowser_adapter_priv_t g_browser_priv = {0};

void browser_clr_events(u32 browser_id)
{
  memset(g_browser_priv.events[browser_id], 0x00, BROWSER_EVT_U32_LEN * 4);
}

void browser_send_event(u32 slot, browser_module_id_t browser_id, u32 event, u32 param)
{
   OS_PRINTF("\n dave test send event %x",event);
   if(event == 0)
  {
    return;
  }

  if(g_browser_priv.notify.evt_cb != NULL)
  {
    g_browser_priv.notify.evt_cb(slot, event, param, browser_id, 
    g_browser_priv.notify.context);
  }
}

RET_CODE browser_event_polling(browser_event_info_t *p_event_info)
{
  u32 i = 0;

  memset(p_event_info, 0x00, sizeof(browser_event_info_t));
  mtos_sem_take(&g_browser_priv.evt_mutex, 0);
  for(i = 0; i < BROWSER_MAX_SLOT_NUM; i++)
  {
      memcpy(p_event_info->events[i], 
      g_browser_priv.events[i], BROWSER_EVT_U32_LEN * 4);
      p_event_info->happened[i] = 1;
      memset(g_browser_priv.events[i], 0x00, BROWSER_EVT_U32_LEN * 4);
  }
  mtos_sem_give(&g_browser_priv.evt_mutex);
  return SUCCESS;
}

browser_event_id_t browser_event_parse(u32 events[BROWSER_EVT_U32_LEN])
{
  u32 i = 0;

  for(i = 0; i < (BROWSER_EVT_U32_LEN << 4); i++)
  {
    if(test_and_clear_bit(i, events) == 1)
    {
      return (i + 1);
    }
  }

  return BROWSER_EVT_NONE;
}


RET_CODE browser_detach(u32 browser_id)
{
  browser_op_t *p_op = (browser_op_t *)browser_id;

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

RET_CODE browser_module_init(browser_module_id_t browser_id)
{
  browser_op_t *p_op = NULL;

  p_op = &g_browser_priv.adm_op[browser_id];
  if(p_op->inited == 1)
  {
    return SUCCESS;
  }

  p_op->inited = 1;
  if(p_op->func.init == NULL)
  {
    return SUCCESS;
  }
   //p_op->func.init();
   return SUCCESS;
}
RET_CODE browser_init(u32 browser_id,  void *p_param)
{
  RET_CODE ret = ERR_FAILURE;

  if(g_browser_priv.inited == 1)
  {
    ret = SUCCESS;
    return ret;
  }
  g_browser_priv.inited = 1;
  MT_ASSERT(TRUE == mtos_sem_create(&g_browser_priv.evt_mutex, TRUE));
  browser_clr_events(browser_id);
  return g_browser_priv.adm_op[browser_id].func.init(p_param);
}

RET_CODE browser_deinit(u32 browser_id)
{
   MT_ASSERT(TRUE == mtos_sem_destroy(&g_browser_priv.evt_mutex, 0));
   g_browser_priv.inited = 0;
   browser_clr_events(browser_id);
   return g_browser_priv.adm_op[browser_id].func.deinit();
}

RET_CODE browser_open(u32 browser_id,  void *p_param)
{
  return g_browser_priv.adm_op[browser_id].func.open(p_param);
}

RET_CODE browser_close(u32 browser_id)
{
  return g_browser_priv.adm_op[browser_id].func.close();
}

RET_CODE browser_resume(u32 browser_id)
{
  return g_browser_priv.adm_op[browser_id].func.resume();
}

RET_CODE browser_pause(u32 browser_id)
{
  return g_browser_priv.adm_op[browser_id].func.pause();
}

RET_CODE browser_io_ctrl(u32 browser_id, u32 cmd, void *p_param)
{
  if(g_browser_priv.adm_op[browser_id].attached != 1)
  {
    return ERR_STATUS;
  }
  return g_browser_priv.adm_op[browser_id].func.io_ctrl(cmd, p_param);
}


