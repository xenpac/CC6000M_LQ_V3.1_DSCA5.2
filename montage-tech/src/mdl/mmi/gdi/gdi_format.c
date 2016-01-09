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

#include "gui_resource.h"

/*
 * match an RGBA value to a particular palette index
 */
u8 gdi_find_color(palette_t *p_pal, u8 r, u8 g, u8 b, u8 a)
{
  /* do colorspace distance matching */
  u32 smallest = 0;
  u32 distance = 0;
  s32 rd = 0, gd = 0, bd = 0, ad = 0;
  u16 i = 0, index = 0;

  smallest = ~0;
  for(i = 0; i < p_pal->cnt; i++)
  {
    rd = p_pal->p_entry[i].r - r;
    gd = p_pal->p_entry[i].g - g;
    bd = p_pal->p_entry[i].b - b;
    ad = p_pal->p_entry[i].a - a;
    // reduce the weight of alpha
    distance = ((rd * rd) + (gd * gd) + (bd * bd) + (ad * ad)) >> 4; 
    if(distance < smallest)
    {
      index = i;
      if(distance == 0)   /* perfect match! */
      {
        break;
      }
      smallest = distance;
    }
  }
  return (u8)(index);
}


/* find the opaque pixel value corresponding to an RGBA triple */
u32 gdi_map_RGBA(hdc_t hdc, u8 r, u8 g, u8 b, u8 a)
{
  u32 pix_val = 0;
  void *p_flinger = dc_hdc2pdc(hdc)->p_curn_flinger;

  switch(flinger_get_format(p_flinger))
  {
    case COLORFORMAT_RGB4BIT:
    case COLORFORMAT_RGB8BIT:
      pix_val = gdi_find_color(flinger_get_palette_addr(p_flinger), r, g, b, a);
      break;

    default:
      pix_val = 0;
      break;
  }

  return pix_val;
}


void gdi_get_RGBA(hdc_t hdc, u32 pix_val, u8 *p_r, u8 *p_g, u8 *p_b, u8 *p_a)
{
  void *p_flinger = dc_hdc2pdc(hdc)->p_curn_flinger;
  palette_t *p_pal = flinger_get_palette_addr(p_flinger);
  
  switch(flinger_get_format(p_flinger))
  {
    case COLORFORMAT_RGB4BIT:
    case COLORFORMAT_RGB8BIT:
      if(pix_val < p_pal->cnt)
      {
        *p_a = p_pal->p_entry[pix_val].a;
        *p_r = p_pal->p_entry[pix_val].r;
        *p_g = p_pal->p_entry[pix_val].g;
        *p_b = p_pal->p_entry[pix_val].b;
      }
      break;

    default:
      *p_a = *p_r = *p_g = *p_b = 0xFF;
      break;
  }
}


u32 gdi_get_invert_color(hdc_t hdc, u32 color)
{
  u8 r = 0, g = 0, b = 0, a = 0;
  dc_t *p_dc = (dc_t *)hdc;
  palette_t *p_pal = flinger_get_palette_addr(p_dc->p_curn_flinger);
  u32 found = 0;
  
  switch(flinger_get_format(p_dc->p_curn_flinger))
  {
    case COLORFORMAT_RGB4BIT:
    case COLORFORMAT_RGB8BIT:
      gdi_get_RGBA(hdc, color, &r, &g, &b, &a);

      r = 0xFF - r;
      g = 0xFF - g;
      b = 0xFF - b;

      found = (u32)gdi_find_color(p_pal, r, g, b, a);
    
      break;
    
    case COLORFORMAT_ARGB8888:
      a = color & 0xFF000000;
      r = (0x00FF0000 - (color & 0x00FF0000));
      g = (0x0000FF00 - (color & 0x0000FF00));
      b = (0x0000FF00 - (color & 0x000000FF));

      found = a | r | g | b;
      break;

    case COLORFORMAT_ARGB1555:
      color = color & 0xFFFF;
      
      a = color & 0x8000;
      r = (0x7C00 - (color & 0x7C00));
      g = (0x03E0 - (color & 0x03E0));
      b = (0x001F - (color & 0x001F));

      found = a | r | g | b;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return found;
}

