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

#include "hal_misc.h"

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

hdc_t gdi_get_roll_dc(rect_t *p_src, void *p_rsurf)
{
  int i = 0;
  dc_t *p_cdc = NULL;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  for(i = 0; i < p_gdi_info->dc_slot_cnt; i++)
  {
    p_cdc = &p_gdi_info->p_dc_slot[i];

    if(!p_cdc->is_used)
    {
      p_cdc->is_used = TRUE;

      p_cdc->type = TYPE_GENERAL | TYPE_MEMORY;

      p_cdc->p_curn_flinger = p_rsurf;
      p_cdc->p_back_flinger = NULL;

      break;
    }
  }

  if(i >= p_gdi_info->dc_slot_cnt)
  {
    return HDC_INVALID;
  }

  //dc_init(&p_gdi_info->p_dc_slot[i], p_src, NULL);
  gdi_set_cliprgn(&p_gdi_info->p_dc_slot[i].ecrgn, p_src);

  p_gdi_info->p_dc_slot[i].dev_rc = *p_src;

  return (hdc_t)(&p_gdi_info->p_dc_slot[i]);
}


void gdi_release_roll_dc(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);

  gdi_empty_cliprgn(&p_dc->lcrgn);
  gdi_empty_cliprgn(&p_dc->ecrgn);

  p_dc->p_gcrgn_info = NULL;

  p_dc->is_used = FALSE;
}

handle_t gdi_create_rsurf(BOOL is_top_screen, u16 width, u16 height, u32 *p_addr)
{
  void *p_vflinger = NULL, *p_ref_flinger = NULL;
  u8 bpp = 0;
  u32 pitch = 0, size = 0;
  gdi_main_t *p_gdi_info = NULL;
  void *p_buf = NULL;
  flinger_virtual_param_t param = {0};
  flinger_cfg_t scr_cfg = {0};
  rect_t scr_rect = {0}, rgn_rect = {0};
  #ifndef WIN32
  chip_ic_t chip_ic = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic = IC_MAGIC;
  #endif

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  if((width == 0) || (height == 0))
  {
    return 0;
  }

  if(is_top_screen)
  {
    if(p_gdi_info->top.p_flinger != NULL)
    {
      p_ref_flinger = p_gdi_info->top.p_flinger;
    }
    else
    {
      p_ref_flinger = p_gdi_info->bot.p_flinger;
    }
  }
  else
  {
    p_ref_flinger = p_gdi_info->bot.p_flinger;  
  }
  MT_ASSERT(p_ref_flinger != NULL);
  
  
  bpp = flinger_get_bpp(p_ref_flinger);

  pitch = (((bpp >> 3) * width + 7) >> 3) << 3; 
  size = pitch * height;

  switch(chip_ic)
  {
    case IC_SONATA:
    case IC_CONCERTO:
      p_buf = mtos_dma_malloc_alias(size + 4);
      break;

    default:
      p_buf = mtos_align_malloc(size + 4, 64);
      break;
  }

  memset(p_buf, 0, size + 4);

  if(p_buf == NULL)
  {
    return 0;
  }

  scr_rect.right = width;
  scr_rect.bottom = height;
  
  scr_cfg.rect_cnt = 1;
  
  rgn_rect.left = 0;
  rgn_rect.top = 0;
  rgn_rect.right = RECTW(scr_rect);
  rgn_rect.bottom = RECTH(scr_rect);

  scr_cfg.p_rgn_rect = &rgn_rect;

  param.p_virtual_buf = p_buf;
  param.p_pal = flinger_get_palette_addr(p_ref_flinger);
  param.format = flinger_get_format(p_ref_flinger);
  param.pitch = pitch;
  param.p_cfg = &scr_cfg;
  param.p_flinger_rect = &scr_rect;

  p_vflinger = flinger_create_virtual(&param);
  MT_ASSERT(p_vflinger != NULL);

 
  //disable the color key
  flinger_set_colorkey(p_vflinger, SURFACE_INVALID_COLORKEY);

  *p_addr = (u32)p_buf;

  return p_vflinger;
}

void gdi_delete_rsurf(handle_t handle, u32 addr)
{
  #ifndef WIN32
  chip_ic_t chip_ic = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic = IC_MAGIC;
  #endif

  flinger_delete((void *)handle);

  switch(chip_ic)
  {
    case IC_SONATA:
    case IC_CONCERTO:
      mtos_dma_free_alias((void *)addr);
      break;

    default:
      mtos_align_free((void *)addr);
      break;
  }  
}

hdc_t gdi_get_mem_dc(rect_t *p_src, void *p_surf)
{
  int i = 0;
  dc_t *p_cdc = NULL;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  for(i = 0; i < p_gdi_info->dc_slot_cnt; i++)
  {
    p_cdc = &p_gdi_info->p_dc_slot[i];

    if(!p_cdc->is_used)
    {
      p_cdc->is_used = TRUE;

      p_cdc->type = TYPE_GENERAL | TYPE_MEMORY;

      p_cdc->p_curn_flinger = p_surf;
      p_cdc->p_back_flinger = NULL;

      break;
    }
  }

  if(i >= p_gdi_info->dc_slot_cnt)
  {
    return HDC_INVALID;
  }

  //dc_init(&p_gdi_info->p_dc_slot[i], p_src, NULL);
  gdi_set_cliprgn(&p_gdi_info->p_dc_slot[i].ecrgn, p_src);

  p_gdi_info->p_dc_slot[i].dev_rc = *p_src;

  return (hdc_t)(&p_gdi_info->p_dc_slot[i]);
}


void gdi_release_mem_dc(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);

  gdi_empty_cliprgn(&p_dc->lcrgn);
  gdi_empty_cliprgn(&p_dc->ecrgn);

  p_dc->p_gcrgn_info = NULL;

  p_dc->is_used = FALSE;
}

handle_t gdi_create_mem_surf(BOOL is_top_screen, u16 width, u16 height, u32 *p_addr)
{
  void *p_vflinger = NULL, *p_ref_flinger = NULL;
  u8 bpp = 0;
  u32 pitch = 0, size = 0;
  gdi_main_t *p_gdi_info = NULL;
  void *p_buf = NULL;
  flinger_virtual_param_t param = {0};
  flinger_cfg_t scr_cfg = {0};
  rect_t scr_rect = {0}, rgn_rect = {0};
  #ifndef WIN32
  chip_ic_t chip_ic = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic = IC_MAGIC;
  #endif

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  if((width == 0) || (height == 0))
  {
    return 0;
  }

  if(is_top_screen)
  {
    p_ref_flinger = p_gdi_info->top.p_flinger;

  }
  else
  {
    p_ref_flinger = p_gdi_info->bot.p_flinger;  
  }
  MT_ASSERT(p_ref_flinger != NULL);
  
  
  bpp = flinger_get_bpp(p_ref_flinger);

  pitch = (((bpp >> 3) * width + 7) >> 3) << 3; 
  size = pitch * height;

  switch(chip_ic)
  {
    case IC_SONATA:
    case IC_CONCERTO:
      p_buf = mtos_dma_malloc_alias(size + 4);
      break;

    default:
      p_buf = mtos_align_malloc(size + 4, 64);
      break;
  }

  memset(p_buf, 0, size + 4);

  if(p_buf == NULL)
  {
    return 0;
  }

  scr_rect.right = width;
  scr_rect.bottom = height;
  
  scr_cfg.rect_cnt = 1;
  
  rgn_rect.left = 0;
  rgn_rect.top = 0;
  rgn_rect.right = RECTW(scr_rect);
  rgn_rect.bottom = RECTH(scr_rect);

  scr_cfg.p_rgn_rect = &rgn_rect;

  param.p_virtual_buf = p_buf;
  param.p_pal = flinger_get_palette_addr(p_ref_flinger);
  param.format = flinger_get_format(p_ref_flinger);
  param.pitch = pitch;
  param.p_cfg = &scr_cfg;
  param.p_flinger_rect = &scr_rect;

  p_vflinger = flinger_create_virtual(&param);
  MT_ASSERT(p_vflinger != NULL);

 
  //disable the color key
  flinger_set_colorkey(p_vflinger, SURFACE_INVALID_COLORKEY);

  *p_addr = (u32)p_buf;

  return p_vflinger;
}

void gdi_delete_mem_surf(handle_t handle, u32 addr)
{
  #ifndef WIN32
  chip_ic_t chip_ic = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic = IC_MAGIC;
  #endif

  flinger_delete((void *)handle);

  switch(chip_ic)
  {
    case IC_SONATA:
    case IC_CONCERTO:
      mtos_dma_free_alias((void *)addr);
      break;

    default:
      mtos_align_free((void *)addr);
      break;
  }
}

