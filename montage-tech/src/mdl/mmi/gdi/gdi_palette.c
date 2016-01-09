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

//-extern void *p_screen_surface;
/*!
  gets palette entries.
  */
BOOL gdi_get_palette(BOOL is_top_screen, u16 start, u16 len, color_t *p_colors)
{
  void *p_flinger = NULL;
  gdi_main_t *p_gdi_info = NULL;
  u8 attr = 0;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(is_top_screen)
  {
    p_flinger = p_gdi_info->top.p_flinger;
  }
  else
  {
    p_flinger = p_gdi_info->bot.p_flinger;
  }

  attr = flinger_get_attr(p_flinger);
  
  if(attr & SURFACE_ATTR_EN_PALETTE)
  {
    flinger_get_palette(p_flinger, start, len, p_colors);
  }

  return TRUE;
}


/*!
  sets palette entries.
  */
BOOL gdi_set_palette(BOOL is_top_screen, u16 start, u16 len, color_t *p_colors)
{
  void *p_flinger = NULL;
  gdi_main_t *p_gdi_info = NULL;
  u8 attr = 0;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(is_top_screen)
  {
    p_flinger = p_gdi_info->top.p_flinger;
  }
  else
  {
    p_flinger = p_gdi_info->bot.p_flinger;
  }
  
  attr = flinger_get_attr(p_flinger);
  
  if(attr & SURFACE_ATTR_EN_PALETTE)
  {
    flinger_set_palette(p_flinger, start, len, p_colors);
  }

  return TRUE;
}


BOOL gdi_set_colorkey(BOOL is_top_screen, u32 ckey)
{
  void *p_flinger = NULL;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(is_top_screen)
  {
    p_flinger = p_gdi_info->top.p_flinger;
  }
  else
  {
    p_flinger = p_gdi_info->bot.p_flinger;
  }
  
  flinger_set_colorkey(p_flinger, ckey);

  return TRUE;
}


BOOL gdi_get_colorkey(BOOL is_top_screen, u32 *p_ckey)
{
  void *p_flinger = NULL;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  MT_ASSERT(p_ckey != NULL);
  
  if(is_top_screen)
  {
    p_flinger = p_gdi_info->top.p_flinger;
  }
  else
  {
    p_flinger = p_gdi_info->bot.p_flinger;
  }

  *p_ckey = flinger_get_colorkey(p_flinger);
  
  return TRUE;
}

palette_t *gdi_get_palette_addr(BOOL is_top_screen)
{
  void *p_flinger = NULL;
  gdi_main_t *p_gdi_info = NULL;
  u8 attr = 0;
  palette_t *p_pal = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(is_top_screen)
  {
    p_flinger = p_gdi_info->top.p_flinger;
  }
  else
  {
    p_flinger = p_gdi_info->bot.p_flinger;
  }

  attr = flinger_get_attr(p_flinger);
  
  if(attr & SURFACE_ATTR_EN_PALETTE)
  {
    p_pal = flinger_get_palette_addr(p_flinger);
  }

  return p_pal;
}