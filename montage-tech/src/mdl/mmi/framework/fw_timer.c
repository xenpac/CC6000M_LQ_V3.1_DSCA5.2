/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

#include "class_factory.h"
#include "mem_manager.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"

//#include "osd.h"
//#include "gpe.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"

#include "mdl.h"

#include "surface.h"
#include "flinger.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_container.h"

#include "ap_framework.h"
#include "framework.h"
#include "fw_common.h"

#define GET_TMR_OWNER(context)      ((u16)(context >> 16))
#define GET_TMR_NOTIFY(context)     ((u16)(context & 0xFFFF))

#define MAKE_TMR_CONTEXT(owner, notify)  \
  ((u32)((owner << 16) | (notify & 0xFFFF)))

typedef RET_CODE (*tmr_found_cb_t)(ui_tmr_entry_t *p_entry, u32 context);


static RET_CODE _find_tmr(u16 owner,
                          u16 notify,
                          tmr_found_cb_t func,
                          u32 context)
{
  u32 i = 0;
  ui_tmr_entry_t *p_entry = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_entry = p_info->infor.p_ui_tmr_entrys;

  for(i = 0; i < p_info->config.max_tmr_cnt; i++)
  {
    if(p_entry->owner == owner
      && p_entry->notify == notify)
    {
      return func(p_entry, context);
    }
    p_entry++;
  }
  return ERR_FAILURE;
}


static RET_CODE _clear_expired_tmr(ui_tmr_entry_t *p_entry, u32 context)
{
  if(p_entry->is_used
    && !p_entry->is_circle)
  {
    p_entry->tmr = INVALID;
    p_entry->owner = 0;
    p_entry->notify = 0;
    p_entry->is_used = FALSE;
  }
  return SUCCESS;
}


static RET_CODE _duplicate_tmr(ui_tmr_entry_t *p_entry, u32 context)
{
  return SUCCESS;
}


static RET_CODE _destroy_tmr(ui_tmr_entry_t *p_entry, u32 context)
{
  if(p_entry->is_used)
  {
    if(p_entry->tmr != INVALID)
    {
      mtos_timer_delete(p_entry->tmr);
      p_entry->tmr = INVALID;
    }
    p_entry->owner = 0;
    p_entry->notify = 0;
    p_entry->is_used = FALSE;

    return SUCCESS;
  }

  return ERR_FAILURE;
}


static RET_CODE _reset_tmr(ui_tmr_entry_t *p_entry, u32 context)
{
  if(p_entry->is_used)
  {
    if(p_entry->tmr != INVALID)
    {
      mtos_timer_reset(p_entry->tmr, context);
      return SUCCESS;
    }
  }

  return ERR_FAILURE;
}


static RET_CODE _start_tmr(ui_tmr_entry_t *p_entry, u32 context)
{
  if(p_entry->is_used)
  {
    if(p_entry->tmr != INVALID)
    {
      mtos_timer_start(p_entry->tmr);
      return SUCCESS;
    }
  }

  return ERR_FAILURE;
}


static RET_CODE _stop_tmr(ui_tmr_entry_t *p_entry, u32 context)
{
  if(p_entry->is_used)
  {
    if(p_entry->tmr != INVALID)
    {
      mtos_timer_stop(p_entry->tmr);
      return SUCCESS;
    }
  }

  return ERR_FAILURE;
}


static RET_CODE _get_sparetime(ui_tmr_entry_t *p_entry, u32 context)
{
  u32 *p_tm = (u32 *)context;

  if(p_entry->is_used)
  {
    if(p_entry->tmr != INVALID)
    {
      mtos_timer_get_sparetime(p_entry->tmr, p_tm);
      return SUCCESS;
    }
  }

  return ERR_FAILURE;
}


static void _tmr_routine(u32 context)
{
  u16 owner = 0, notify = 0;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  owner = GET_TMR_OWNER(context);
  notify = GET_TMR_NOTIFY(context);

  FW_PRINTF("=TMR EXPIRED=\n");

  fw_post_tmout(owner, notify, 0, 0);

  // clear if it is NOT circle
  _find_tmr(owner, notify, _clear_expired_tmr, 0);
}


void fw_tmr_init(void)
{
  u16 i = 0;
  ui_tmr_entry_t *p_entry = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_entry = p_info->infor.p_ui_tmr_entrys;

  memset(p_info->infor.p_ui_tmr_entrys, 0,
         sizeof(ui_tmr_entry_t) * p_info->config.max_tmr_cnt);

  for(i = 0; i < p_info->config.max_tmr_cnt; i++)
  {
    p_entry->tmr = INVALID;
    p_entry++;
  }  
}

void fw_tmr_release(void)
{
  u16 i = 0;
  ui_tmr_entry_t *p_entry = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_entry = p_info->infor.p_ui_tmr_entrys;

  for(i = 0; i < p_info->config.max_tmr_cnt; i++)
  {
    if(p_entry->tmr != INVALID)
    {
      mtos_timer_delete(p_entry->tmr);
      p_entry->tmr = INVALID;
    }
    p_entry++;
  }
}


RET_CODE fw_tmr_create(u16 owner, u16 notify, u32 tm_out, BOOL is_circle)
{
  u32 i = 0, context = 0;
  ui_tmr_entry_t *p_entry = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  // check for duplicate
  if(_find_tmr(owner, notify, _duplicate_tmr, 0) == SUCCESS)
  {
    return ERR_FAILURE;
  }

  FW_PRINTF("=TMR CREATE=\n");
  p_entry = p_info->infor.p_ui_tmr_entrys;
  for(i = 0; i < p_info->config.max_tmr_cnt; i++)
  {
    if(!p_entry->is_used)
    {
      context = MAKE_TMR_CONTEXT(owner, notify);
      if((p_entry->tmr =
            mtos_timer_create(tm_out, _tmr_routine, 
                              context, is_circle)) == INVALID)
      {
        return ERR_FAILURE;
      }
      FW_PRINTF("=CREATE OK=\n");
      p_entry->owner = owner;
      p_entry->notify = notify;
      p_entry->is_circle = is_circle;
      p_entry->is_used = TRUE;

      return SUCCESS;
    }
    p_entry++;
  }
  return ERR_NO_RSRC;
}


RET_CODE fw_tmr_destroy(u16 owner, u16 notify)
{
  return _find_tmr(owner, notify, _destroy_tmr, 0);
}


RET_CODE fw_tmr_reset(u16 owner, u16 notify, u32 tm_out)
{
  return _find_tmr(owner, notify, _reset_tmr, tm_out);
}


RET_CODE fw_tmr_get_sparetime(u16 owner, u16 notify, u32 *p_sparetime)
{
  return _find_tmr(owner, notify, _get_sparetime, (u32)p_sparetime);
}


RET_CODE fw_tmr_start(u16 owner, u16 notify)
{
  return _find_tmr(owner, notify, _start_tmr, 0);
}


RET_CODE fw_tmr_stop(u16 owner, u16 notify)
{
  return _find_tmr(owner, notify, _stop_tmr, 0);
}
