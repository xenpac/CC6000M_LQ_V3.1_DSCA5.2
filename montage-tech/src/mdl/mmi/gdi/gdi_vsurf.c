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

RET_CODE gdi_init_vsurf(u32 vsurf_addr, u32 vsurf_size)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);
  
  if(vsurf_size == 0 || vsurf_addr == 0)
  {
    gdi_release_vsurf();
    
    return ERR_FAILURE;
  }

  p_gdi_info->p_vsurf_buf = (void *)vsurf_addr;

  p_gdi_info->vsurf_buf_size = vsurf_size;

  return SUCCESS;
}


void gdi_release_vsurf(void)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  p_gdi_info->p_vsurf_buf = NULL;

  p_gdi_info->vsurf_buf_size = 0;
}


RET_CODE gdi_create_vsurf(hdc_t hdc, s16 width, s16 height)
{
  void *p_vflinger = NULL, *p_ref_flinger = NULL;
  flinger_virtual_param_t param = {0};
  flinger_cfg_t scr_cfg = {0};
  rect_t scr_rect = {0}, rgn_rect = {0};
  dc_t *p_dc = dc_hdc2pdc(hdc);
  u32 pitch = 0, size = 0;
  u8 bpp = 0;

  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  if((width == 0) || (height == 0))
  {
    return ERR_FAILURE;
  }
  
  if(p_gdi_info->p_vsurf_buf == NULL)
  {
    return ERR_FAILURE;
  }

  p_ref_flinger = p_dc->p_curn_flinger;

  bpp = flinger_get_bpp(p_ref_flinger);

  pitch = (bpp * width) >> 3;
  size = pitch * height;

  if (size > p_gdi_info->vsurf_buf_size)
  {    
    return ERR_FAILURE;
  }

  //create virtual flinger.
  scr_rect.right = width;
  scr_rect.bottom = height;
  
  scr_cfg.rect_cnt = 1;
  rgn_rect.left = 0;
  rgn_rect.top = 0;
  rgn_rect.right = RECTW(scr_rect);
  rgn_rect.bottom = RECTH(scr_rect);

  scr_cfg.p_rgn_rect = &rgn_rect;

  param.p_virtual_buf = p_gdi_info->p_vsurf_buf;
  param.p_pal = flinger_get_palette_addr(p_ref_flinger);
  param.format = flinger_get_format(p_ref_flinger);
  param.pitch = pitch;
  param.p_cfg = &scr_cfg;
  param.p_flinger_rect = &scr_rect;

  p_vflinger = flinger_create_virtual(&param);
  MT_ASSERT(p_vflinger != NULL);
  
  flinger_set_colorkey(p_vflinger, SURFACE_INVALID_COLORKEY);

  p_dc->type |= TYPE_VIRTUAL;

  p_dc->p_curn_flinger = p_vflinger;
  p_dc->p_back_flinger = p_ref_flinger;

  return SUCCESS;
}


RET_CODE gdi_delete_vsurf(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);
    
  if(p_dc->type & TYPE_VIRTUAL)
  {
    flinger_delete(p_dc->p_curn_flinger);

    p_dc->p_curn_flinger = p_dc->p_back_flinger;
    
    p_dc->type &= ~TYPE_VIRTUAL;

    return SUCCESS;
  }

  return ERR_FAILURE;
}

