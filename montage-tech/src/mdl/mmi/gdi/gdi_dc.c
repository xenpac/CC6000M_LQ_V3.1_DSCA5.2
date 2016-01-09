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

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "common.h"
#include "osd.h"
#include "gpe.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"
#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "gdi_private.h"
#include "class_factory.h"

dc_t *dc_hdc2pdc(hdc_t hdc)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);
  
  if(hdc == HDC_INVALID)
  {
    return (dc_t *)(NULL);
  }

  if(hdc == HDC_SCREEN)
  {
    if((((dc_t *)hdc)->type & TYPE_TOP))
    {
      return &p_gdi_info->top.screen_dc;
    }
    else
    {
      return &p_gdi_info->bot.screen_dc;
    }
  }

  return (dc_t *)hdc;
}

BOOL dc_is_top_hdc(hdc_t hdc)
{
  if(hdc == HDC_INVALID)
  {
    return FALSE;
  }

  return(((dc_t *)hdc)->type & TYPE_TOP);
}

BOOL dc_is_mem_hdc(hdc_t hdc)
{
  if(hdc == HDC_INVALID)
  {
    return FALSE;
  }

  if(hdc == HDC_SCREEN)
  {
    return FALSE;
  }
  return(((dc_t *)hdc)->type & TYPE_MEMORY);
}

BOOL dc_is_screen_hdc(hdc_t hdc)
{
  return(hdc == HDC_SCREEN);
}

BOOL dc_is_general_hdc(hdc_t hdc)
{
  if(hdc == HDC_INVALID)
  {
    return FALSE;
  }

  if(hdc == HDC_SCREEN)
  {
    return FALSE;
  }

  return(((dc_t *)hdc)->type & TYPE_GENERAL);
}

BOOL dc_is_inherit_hdc(hdc_t hdc)
{
  if(hdc == HDC_INVALID)
  {
    return FALSE;
  }

  if(hdc == HDC_SCREEN)
  {
    return FALSE;
  }

  return(((dc_t *)hdc)->type & TYPE_INHERIT);
}

BOOL dc_is_virtual_hdc(hdc_t hdc)
{
  if(hdc == HDC_INVALID)
  {
    return FALSE;
  }

  if(hdc == HDC_SCREEN)
  {
    return FALSE;
  }

  return(((dc_t *)hdc)->type & TYPE_VIRTUAL);
}

BOOL dc_is_anim_hdc(hdc_t hdc)
{
  if(hdc == HDC_INVALID)
  {
    return FALSE;
  }

  if(hdc == HDC_SCREEN)
  {
    return FALSE;
  }

  return(((dc_t *)hdc)->type & TYPE_ANIM);
}

BOOL dc_is_mem_dc(dc_t *p_pdc)
{
  return(p_pdc->type & TYPE_MEMORY);
}

BOOL dc_is_screen_dc(dc_t *p_pdc)
{
  return(p_pdc->type & TYPE_SCREEN);
}

BOOL dc_is_general_dc(dc_t *p_pdc)
{
  return(p_pdc->type & TYPE_GENERAL);
}

BOOL dc_is_virtual_dc(dc_t *p_pdc)
{
  return(p_pdc->type & TYPE_VIRTUAL);
}

BOOL dc_is_inherit_dc(dc_t *p_pdc)
{
  return(p_pdc->type & TYPE_INHERIT);
}


BOOL dc_is_visible(dc_t *p_pdc)
{
  return TRUE;
}

void coor_sp2sp(dc_t *p_pdc, s16 *p_x, s16 *p_y)
{
  if(p_pdc->type & TYPE_VIRTUAL)
  {
    *p_x -= p_pdc->vtrl_rc.left;
    *p_y -= p_pdc->vtrl_rc.top;
  }
}

void coor_dp2sp(dc_t *p_pdc, s16 *p_x, s16 *p_y)
{
//	if (pdc->type & TYPE_SCREEN)
//		return;

  *p_x += p_pdc->dev_rc.left;
  *p_y += p_pdc->dev_rc.top;
}

void coor_sp2dp(dc_t *p_pdc, s16 *p_x, s16 *p_y)
{
//	if (pdc->type & TYPE_SCREEN)
//		return;

  *p_x -= p_pdc->dev_rc.left;
  *p_y -= p_pdc->dev_rc.top;
}
