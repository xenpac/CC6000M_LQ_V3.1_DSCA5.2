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

#include "hal_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "mem_manager.h"

#include "lib_memp.h"
#include "lib_memf.h"
#include "lib_rect.h"
#include "class_factory.h"

#include "common.h"
#include "region.h"
#include "display.h"
#include "gpe_vsb.h"
#include "common.h"
#include "gui_resource.h"
#include "surface.h"

#include <math.h>

/*!
  pi
  */
#define PI 3.1415926


/*!
   The structure is defined to descript a surface.
  */
typedef struct
{
  /*!
     The pixel format
    */
  pix_fmt_t format;
  /*!
     Bits per pixel
    */
  u8 bpp;
  /*!
     The global alpha
    */
  u8 alpha;
  /*!
     The attributes of the surface
    */
  u8 attr;
  /*!
     The width
    */
  u16 width;
  /*!
     The height
    */
  u16 height;
  /*!
     The pitch
    */
  u32 pitch;
  /*!
     colorkey
    */
  u32 colorkey;
  /*!
     Points to a palette
    */
  palette_t *p_pal;
  /*!
     The clipping rectangle
    */
  rect_t rc_clip;
  /*!
     The display rectangle
    */
  rect_t rc_src;
  /*!
     The handle of the OSD region.
    */
  s32 handle;
  /*!
     The display buffer odd & even.
    */
  u8 layer_id;
  /*!
     Points to a GPE object.
    */
  void *p_gpe_dev;
  /*!
    display device
    */
  void *p_disp;  
  /*!
     Point to the OSD region.
    */
  void *p_osd_rgn;  
  /*!
    chip id.
    */
  chip_ic_t chip_ic;    
} surface_t;


static surface_t *surface_alloc(void)
{
  return (surface_t *)mtos_malloc(sizeof(surface_t));
}


static void surface_free(void *p_addr)
{
  mtos_free(p_addr);
}


static u32 calc_pitch(u16 bpp, u16 width)
{
  return (u32)((width * bpp + 7) >> 3);
}

static BOOL get_effect_rc(rect_t *p_erc, rect_t *p_crc)
{
  MT_ASSERT(p_erc != NULL);
  MT_ASSERT(p_crc != NULL);
  
  return intersect_rect(p_erc, p_erc, p_crc);
}

void surface_enable(void *p_surface, BOOL is_enable)
{
  surface_t *p_surf = (surface_t *)p_surface;
  void *p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  
  MT_ASSERT(NULL != p_disp);  
  MT_ASSERT(p_surf != NULL);
  
  disp_layer_show(p_disp, p_surf->layer_id, is_enable);
}

static pix_fmt_t pix_type_to_fmt(pix_type_t type)
{
  pix_fmt_t pix_fmt = PIX_FMT_RGBPALETTE1;
  
  switch(type)
  {
    case COLORFORMAT_RGB4BIT:
      pix_fmt = PIX_FMT_RGBPALETTE4;
      break;
    case COLORFORMAT_RGB8BIT:
      pix_fmt = PIX_FMT_RGBPALETTE8;
      break;
    case COLORFORMAT_RGB565:  
      pix_fmt = PIX_FMT_RGB565;
      break;
    case COLORFORMAT_RGBA5551:  
      pix_fmt = PIX_FMT_RGBA5551;
      break;
    case COLORFORMAT_ARGB1555:  
      pix_fmt = PIX_FMT_ARGB1555;
      break;
    case COLORFORMAT_RGBA8888:  
      pix_fmt = PIX_FMT_RGBA8888;
      break;
    case COLORFORMAT_ARGB8888:  
      pix_fmt = PIX_FMT_ARGB8888;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return pix_fmt;
}

RET_CODE surface_create(void **pp_surf, surface_desc_t *p_desc)
{
  surface_t *p_surf = NULL;
  rect_size_t rect_size = {0};
  point_t pos = {0};  
  RET_CODE ret = SUCCESS;
  disp_layer_id_t layer_id = DISP_LAYER_ID_MAX;
  
  if(p_desc == NULL)
  {
    return ERR_FAILURE;
  }    

  if((p_surf = surface_alloc()) == NULL)
  {
    return ERR_NO_MEM;
  }

  memset(p_surf, 0, sizeof(surface_t));

  if(p_desc->flag & SURFACE_DESC_FORMAT)
  {
    p_surf->format = pix_type_to_fmt(p_desc->format);
  }
  else
  {
    p_surf->format = PIX_FMT_RGB565;
  }

  switch(p_surf->format)
  {
    case PIX_FMT_RGBPALETTE4:
      p_surf->bpp = 4;
      p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
      p_surf->p_pal = p_desc->p_pal;      
      break;
    case PIX_FMT_RGBPALETTE8:
      p_surf->bpp = 8;
      p_surf->attr |= SURFACE_ATTR_EN_PALETTE;
      p_surf->p_pal = p_desc->p_pal;      
      break;      
    case PIX_FMT_RGB565:
      p_surf->bpp = 16;
      break;
    case PIX_FMT_RGBA5551:
      p_surf->bpp = 16;
      break;
    case PIX_FMT_ARGB1555:
      p_surf->bpp = 16;
      break;
    case PIX_FMT_RGBA8888:
      p_surf->bpp = 32;
      break;
    case PIX_FMT_ARGB8888:
      p_surf->bpp = 32;
      break;
    default:
      return ERR_NOFEATURE;
  }

  /* link related gpe device */
  p_surf->p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_surf->p_gpe_dev);

  p_surf->p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_surf->p_disp);  

  /* get current width&height */
  p_surf->width = p_desc->flag & SURFACE_DESC_WIDTH ? p_desc->width : 1280;
  p_surf->height = p_desc->flag & SURFACE_DESC_HEIGHT ? p_desc->height : 720;
  p_surf->pitch = p_desc->flag & SURFACE_DESC_PITCH ? \
                  p_desc->pitch : calc_pitch(p_surf->bpp, p_surf->width);
  pos.x = p_desc->flag & SURFACE_DESC_X ? p_desc->x : 0;
  pos.y = p_desc->flag & SURFACE_DESC_Y ? p_desc->y : 0;      


#ifndef WIN32
  p_surf->chip_ic = hal_get_chip_ic_id();
#else
  p_surf->chip_ic = IC_MAGIC;
#endif  

  if(p_desc->flag & SURFACE_DESC_CAPS)
  {
    switch(p_desc->caps)
    {
      case SURFACE_CAPS_MEM_ASSIGN:
        //create region.
        pos.x = 0;
        pos.y = 0;
        rect_size.w = p_surf->width;
        rect_size.h = p_surf->height;
        p_surf->p_osd_rgn = region_create(&rect_size, p_surf->format);
        MT_ASSERT(NULL != p_surf->p_osd_rgn);
        
        p_surf->handle = -1;

        switch(p_surf->chip_ic)
        {
          case IC_MAGIC:
          case IC_ENSEMBLE:
          case IC_WIZARDS:
            ((region_t *)(p_surf->p_osd_rgn))->p_buf_odd = p_desc->p_vmem[0];
            ((region_t *)(p_surf->p_osd_rgn))->p_buf_even = p_desc->p_vmem[1];
            break;
        
          default:
            ((region_t *)(p_surf->p_osd_rgn))->p_buf_odd = p_desc->p_vmem[0];
            ((region_t *)(p_surf->p_osd_rgn))->p_buf_even = NULL;
            break;
        }

        break;
      case SURFACE_CAPS_MEM_SYSTEM:
        //create region.
        rect_size.w = p_surf->width;
        rect_size.h = p_surf->height;
        p_surf->p_osd_rgn = region_create(&rect_size, p_surf->format);
        MT_ASSERT(NULL != p_surf->p_osd_rgn);

        switch(p_desc->layer)
        {
          case SURFACE_OSD0:
            layer_id = DISP_LAYER_ID_OSD0;
            break;

          case SURFACE_OSD1:
            layer_id = DISP_LAYER_ID_OSD1;
            break;

          case SURFACE_SUB:
            layer_id = DISP_LAYER_ID_SUBTITL;
            break;

          default:
            MT_ASSERT(0);
            break;
        }

        ret = disp_layer_add_region(
          p_surf->p_disp, layer_id, p_surf->p_osd_rgn, &pos, NULL);
        MT_ASSERT(SUCCESS == ret);

        if(p_surf->attr & SURFACE_ATTR_EN_PALETTE)
        {
          MT_ASSERT(p_surf->p_pal != NULL);
          region_set_palette(p_surf->p_osd_rgn, (u32 *)p_surf->p_pal->p_entry, p_surf->p_pal->cnt);
        }

        disp_layer_alpha_onoff(p_surf->p_disp, layer_id, TRUE);
        region_alpha_onoff(p_surf->p_osd_rgn, FALSE);

        p_surf->layer_id = layer_id;

        p_surf->handle = 0;

        break;
        
      default:
        return ERR_NOFEATURE;
    }
  }

  p_surf->alpha = 0xFF;

  set_rect(&p_surf->rc_src, 0, 0, p_surf->width, p_surf->height);
  /* set surface */
  *pp_surf = (void *)p_surf;

  return SUCCESS;
}


RET_CODE surface_delete(void *p_surface)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL)
  {
    return ERR_FAILURE;
  }  
  
  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
  {
    if(p_surf->p_osd_rgn != NULL)
    {
      region_show(p_surf->p_osd_rgn, FALSE);
      p_surf->attr &= (~SURFACE_ATTR_EN_DISPLAY);
    }
  }

  if(p_surf->p_osd_rgn != NULL && p_surf->handle != -1)
  {
    //remove region from display layer.
    disp_layer_remove_region(p_surf->p_disp, p_surf->layer_id, p_surf->p_osd_rgn);
  }

  if(p_surf->p_osd_rgn != NULL)
  {
    //delete region
    region_delete(p_surf->p_osd_rgn);
  }

  if(p_surf->p_gpe_dev != NULL)
  {
    dev_close(p_surf->p_gpe_dev);
  }

  /* release */
  surface_free(p_surf);
  return SUCCESS;
}


RET_CODE surface_set_display(void *p_surface, BOOL is_display)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL || p_surf->p_osd_rgn == NULL)
  {
    return ERR_FAILURE;
  }
  
  if((p_surf->attr & SURFACE_ATTR_EN_DISPLAY) == is_display)
  {
    return SUCCESS;
  }

  if(is_display)
  {
    p_surf->attr |= SURFACE_ATTR_EN_DISPLAY;
  }
  else
  {
    p_surf->attr &= (~SURFACE_ATTR_EN_DISPLAY);
  }

  //set osd region.
  region_show(p_surf->p_osd_rgn, is_display);

  return SUCCESS;
}


RET_CODE surface_set_alpha(void *p_surface, u8 alpha)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL || p_surf->p_osd_rgn == NULL)
  {
    return ERR_FAILURE;
  }
  
  p_surf->alpha = alpha;

  //set osd region.
  disp_layer_alpha_onoff(p_surf->p_disp, p_surf->layer_id, TRUE);
  region_alpha_onoff(p_surf->p_osd_rgn, FALSE);
  disp_layer_set_alpha(p_surf->p_disp, p_surf->layer_id, alpha);

  return SUCCESS;
}

RET_CODE surface_set_trans(void *p_surface, u32 index, u8 alpha)
{
  return SUCCESS;
}

RET_CODE surface_init_palette(void *p_surface, palette_t *p_pal)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL || p_surf->p_osd_rgn == NULL)
  {
    return ERR_FAILURE;
  }
  
  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE)
  {
    p_surf->p_pal = p_pal;
  }
  else
  {
    return ERR_FAILURE;
  }

  //set osd region.
  region_set_palette(p_surf->p_osd_rgn, (u32 *)p_pal->p_entry, p_pal->cnt);

  return SUCCESS;
}


RET_CODE surface_set_palette(void *p_surface, u16 start, u16 len, color_t *p_entry)
{
  surface_t *p_surf = (surface_t *)p_surface;
  palette_t *p_pal = NULL;

  if(p_surf == NULL || p_surf->p_osd_rgn == NULL)
  {
    return ERR_FAILURE;
  }

  p_pal = p_surf->p_pal;
  
  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE && p_pal != NULL)
  {
    memcpy(&p_pal->p_entry[start], p_entry, sizeof(color_t) * len);
  }
  else
  {
    return ERR_FAILURE;
  }

  //set osd region.
  region_set_palette(p_surf->p_osd_rgn, (u32 *)p_pal->p_entry, p_pal->cnt);

  return SUCCESS;
}


RET_CODE surface_get_palette(void *p_surface, u16 start,
  u16 len, color_t *p_entry)
{
  surface_t *p_surf = (surface_t *)p_surface;
  palette_t *p_pal = NULL;

  if(p_surf == NULL)
  {
    return ERR_FAILURE;
  }

  p_pal = p_surf->p_pal;

  if(p_surf->attr & SURFACE_ATTR_EN_PALETTE && p_pal != NULL)
  {
    memcpy(p_entry, &p_pal->p_entry[start], sizeof(color_t) * len);
  }
  else
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}


RET_CODE surface_set_colorkey(void *p_surface, u32 ckey)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL)
  {
    return ERR_FAILURE;
  }

  if(ckey != SURFACE_INVALID_COLORKEY)
  {
    p_surf->colorkey = ckey;
    p_surf->attr |= SURFACE_ATTR_EN_COLORKEY;
  }
  else
  {
    p_surf->attr &= (~SURFACE_ATTR_EN_COLORKEY);
  }
  
  return SUCCESS;
}


RET_CODE surface_set_trans_clr(void *p_surface, u32 trans_clr)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL)
  {
    return ERR_FAILURE;
  }

  if(trans_clr != SURFACE_INVALID_COLORKEY)
  {  
    if(p_surf->handle != -1)
    {
      //set osd region.
      disp_layer_color_key_onoff(p_surf->p_disp, p_surf->layer_id, TRUE);    
      disp_layer_set_color_key(p_surf->p_disp, p_surf->layer_id, trans_clr);
    }
  }
  else
  {
    if(p_surf->handle != -1)
    {
      //set osd region.
      disp_layer_color_key_onoff(p_surf->p_disp, p_surf->layer_id, FALSE);   
    }
  }
  return SUCCESS;
}

RET_CODE surface_get_colorkey(void *p_surface, u32 *p_ckey)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL || p_ckey == NULL)
  {
    return ERR_FAILURE;
  }
  
  if(p_surf->attr & SURFACE_ATTR_EN_COLORKEY)
  {
    *p_ckey = p_surf->colorkey;
  }
  else
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}

RET_CODE surface_set_cliprect(void *p_surface, rect_t *p_rc)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL)
  {
    return ERR_FAILURE;
  }

  if(p_rc != NULL)
  {
    p_surf->attr |= SURFACE_ATTR_EN_CLIP;
    copy_rect(&p_surf->rc_clip, p_rc);
  }
  else
  {
    p_surf->attr &= (~SURFACE_ATTR_EN_CLIP);
  }

  return SUCCESS;
}


RET_CODE surface_get_cliprect(void *p_surface, rect_t *p_rc)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL || p_rc == NULL)
  {
    return ERR_FAILURE;
  }
  
  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    copy_rect(p_rc, &p_surf->rc_clip);
  }
  else
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}


RET_CODE surface_set_srcrect(void *p_surface, rect_t *p_rc)
{
  surface_t *p_surf = (surface_t *)p_surface;
  point_t pos = {0};

  if(p_surf == NULL || p_surf->p_osd_rgn == NULL)
  {
    return ERR_FAILURE;
  }

  if(RECTWP(p_rc) != RECTW(p_surf->rc_src)
    || RECTHP(p_rc) != RECTH(p_surf->rc_src))
  {
    return ERR_FAILURE;
  }

  copy_rect(&p_surf->rc_src, p_rc);

  if(p_surf->handle == -1)
  {
    return SUCCESS;
  }

  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
  {
    //set osd region.
    region_show(p_surf->p_osd_rgn, FALSE);
  }
  
  pos.x = p_surf->rc_src.left;
  pos.y = p_surf->rc_src.top;
  disp_layer_move_region(p_surf->p_disp, p_surf->p_osd_rgn, &pos);
    
  if(p_surf->attr & SURFACE_ATTR_EN_DISPLAY)
  {
    //set osd region.
    region_show(p_surf->p_osd_rgn, TRUE);
  }
  
  return SUCCESS;
}


RET_CODE surface_get_srcrect(void *p_surface, rect_t *p_rc)
{
  surface_t *p_surf = (surface_t *)p_surface;

  if(p_surf == NULL || p_rc == NULL)
  {
    return ERR_FAILURE;
  }

  copy_rect(p_rc, &p_surf->rc_src);
  return SUCCESS;
}


RET_CODE surface_fill_rect(void *p_surface, rect_t *p_rc, u32 value)
{
  surface_t *p_surf = (surface_t *)p_surface;
  rect_t erc = {0};

  if(p_surf == NULL || p_rc == NULL)
  {
    return ERR_FAILURE;
  } 

  copy_rect(&erc, p_rc);

  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    if(!get_effect_rc(&erc, &p_surf->rc_clip))
    {
      return ERR_PARAM;
    }
  }

  gpe_draw_rectangle_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &erc, value);
  
  return SUCCESS;
}

RET_CODE surface_fill_bmp(void *p_surface,
  rect_t *p_rc, bitmap_t *p_bmp, u16 x_off, u16 y_off)
{
  surface_t *p_surf = (surface_t *)p_surface;
  gpe_param_vsb_t param = {0};
  rect_t orc = {0}, erc = {0};
  rect_t fill_rect = {0};
  pix_fmt_t pix_fmt = PIX_FMT_RGBPALETTE1;

  if(p_surf == NULL || p_rc == NULL || p_bmp == NULL)
  {
    return ERR_FAILURE;
  } 

  copy_rect(&erc, p_rc);

  set_rect(&orc, p_rc->left, p_rc->top, p_rc->left + p_bmp->width,
           p_rc->top + p_bmp->height);

  if(!get_effect_rc(&erc, &orc))
  {
    return ERR_PARAM;
  }

  if(p_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    if(!get_effect_rc(&erc, &p_surf->rc_clip))
    {
      return ERR_PARAM;
    }
  }

  fill_rect.left = erc.left - p_rc->left + x_off;
  fill_rect.top = erc.top - p_rc->top + y_off;
  fill_rect.right = fill_rect.left + RECTW(erc);
  fill_rect.bottom = fill_rect.top + RECTH(erc);

  param.colorkey = p_bmp->colorkey;
  param.enable_colorkey = (u32)p_bmp->enable_ckey;
  param.is_font = p_bmp->is_font;

  if(p_bmp->p_alpha != NULL)
  {
    param.cmd = GPE_CMD_ALPHA_MAP;
    param.p_alpha = p_bmp->p_alpha;
    param.alpha_pitch = p_bmp->alpha_pitch;
  }

  //if dummy region exist, paint on the dummy region first, else paint on the osd region.
  pix_fmt = pix_type_to_fmt(p_bmp->format);
  
  gpe_draw_image_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &erc, p_bmp->p_bits, 
    NULL, 0, p_bmp->pitch, 
    (p_bmp->pitch * p_bmp->height), pix_fmt, &param, &fill_rect);

  if(p_bmp->p_strok_alpha != NULL)
  {
    param.cmd = GPE_CMD_ALPHA_MAP;
    param.p_alpha = p_bmp->p_strok_alpha;
    param.alpha_pitch = p_bmp->alpha_pitch;

    gpe_draw_image_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &erc, p_bmp->p_strok_char, 
      NULL, 0, p_bmp->pitch, 
      (p_bmp->pitch * p_bmp->height), pix_fmt, &param, &fill_rect);
  }
  
  return SUCCESS;
}


RET_CODE surface_get_bits(void *p_surface,
  rect_t *p_rc, void *p_buf, u32 pitch)
{
  surface_t *p_surf = (surface_t *)p_surface;
  u32 entries_num = 0;

  if(p_surf == NULL)
  {
    return ERR_FAILURE;
  } 

  if(p_rc == NULL)
  {
    return ERR_FAILURE;
  }    

  if(p_buf == NULL)
  {
    return ERR_FAILURE;
  }  
  
  if(!is_rect_covered(p_rc, &p_surf->rc_src))
  {
    return ERR_PARAM;
  }

  gpe_dump_image_vsb(p_surf->p_gpe_dev, 
    p_surf->p_osd_rgn, p_rc, p_buf, p_surf->p_pal->p_entry, &entries_num, p_surf->format);

  return SUCCESS;
}


RET_CODE surface_bitblt(void *p_src_surface,
  u16 sx, u16 sy, u16 sw, u16 sh,
  void *p_dst_surface, u16 dx, u16 dy, surface_rop_type_t rop, u32 rop_pat)
{
  surface_t *p_src_surf = (surface_t *)p_src_surface;
  surface_t *p_dst_surf = (surface_t *)p_dst_surface;
  gpe_param_vsb_t param = {0};
  rect_t src_rc = {0}, dst_rc = {0};
  void *p_src_rgn = NULL, *p_dst_rgn = NULL;
  RET_CODE ret = SUCCESS;

  cct_gpe_blt2opt_t bopt2 = {0};
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};

  memset(&param, 0, sizeof(gpe_param_vsb_t));
  param.cmd = GPE_CMD_RASTER_OP;

  //OS_PRINTF("surface bitblt, rop %d, pat %d\n", rop, rop_pat);
  
  switch(rop)
  {
    case SURFACE_ROP_AND:
      param.rop3 = SRCAND;
      break;
      
    case SURFACE_ROP_OR:
      param.rop3 = SRCPAINT;
      break;      

    case SURFACE_ROP_XOR:
      param.rop3 = SRCINVERT;
      break;      
    case SURFACE_ROP_PATCOPY:
       if(p_src_surf->chip_ic != IC_CONCERTO)
      {
        break;
      }
      param.rop3 = PATCOPY;
      param.pattern = rop_pat;
      break;

    case SURFACE_ROP_BLEND_DST:
       if(p_src_surf->chip_ic != IC_CONCERTO)
      {
        break;
      }
      param.cmd |= GPE_CMD_ALPHA_BLEND;

      bopt2.enableBld = TRUE;
      bopt2.blendCfg.src_blend_fact = GL_ONE_MINUS_DST_ALPHA;
      bopt2.blendCfg.dst_blend_fact = GL_DST_ALPHA;
      break;

    case SURFACE_ROP_BLEND_SRC:
    if(p_src_surf->chip_ic != IC_CONCERTO)
      {
        break;
      }
      param.cmd |= GPE_CMD_ALPHA_BLEND;
      bopt2.enableBld = TRUE;
      bopt2.blendCfg.src_blend_fact = GL_SRC_ALPHA;
      bopt2.blendCfg.dst_blend_fact = GL_ONE_MINUS_SRC_ALPHA;
      break;
    default:
      param.rop3 = SRCCOPY;
      break;
  }
  
  if(p_dst_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    param.cmd |= GPE_CMD_CLIP_RECT;
  }

  if(p_src_surf->attr & SURFACE_ATTR_EN_COLORKEY)
  {
    param.cmd |= GPE_CMD_COLORKEY;
    param.enable_colorkey = TRUE;
    param.colorkey = p_src_surf->colorkey;
  }

  src_rc.left = sx;
  src_rc.top = sy;
  src_rc.right = (sw + sx);
  src_rc.bottom = (sh + sy);

  if(p_dst_surf->attr & SURFACE_ATTR_EN_CLIP)
  {
    dst_rc.left = dx;
    dst_rc.top = dy;  
    dst_rc.right = (sw + dx);
    dst_rc.bottom = (sh + dy);
    if(!get_effect_rc(&dst_rc, &p_dst_surf->rc_clip))
    {
      return ERR_PARAM;
    }

    src_rc.left += (dst_rc.left - dx);
    src_rc.top += (dst_rc.top - dy);
    src_rc.right = src_rc.left + RECTW(dst_rc);
    src_rc.bottom = src_rc.top + RECTH(dst_rc);
  }
  else
  {
    dst_rc.left = dx;
    dst_rc.top = dy;  
    dst_rc.right = (sw + dx);
    dst_rc.bottom = (sh + dy);
  }

  p_dst_rgn = p_dst_surf->p_osd_rgn;
  p_src_rgn = p_src_surf->p_osd_rgn;

  //OS_PRINTF("src %d, %d, %d, %d\n", src_rc.left, src_rc.top, RECTW(src_rc), RECTH(src_rc));
  //OS_PRINTF("dst %d, %d, %d, %d\n", dst_rc.left, dst_rc.top, RECTW(dst_rc), RECTH(dst_rc));
  if((param.rop3 == PATCOPY) && (p_src_surf->chip_ic == IC_CONCERTO))
  {
    bopt2.enableRop = TRUE;
    bopt2.ropCfg.rop_a_id = ROP_PATCOPY;
    bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
    bopt2.ropCfg.rop_pattern = rop_pat << 24;

    src_vsb.x = src_rc.left;
    src_vsb.y = src_rc.top;
    src_vsb.w = RECTW(src_rc);
    src_vsb.h = RECTH(src_rc);

    drc_vsb.x = dst_rc.left;
    drc_vsb.y = dst_rc.top;
    drc_vsb.w = RECTW(dst_rc);
    drc_vsb.h = RECTH(dst_rc);

    ret = gpe_bitblt2src(p_dst_surf->p_gpe_dev, p_src_rgn, p_dst_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
  }
  else if((param.cmd & GPE_CMD_ALPHA_BLEND) && (p_src_surf->chip_ic == IC_CONCERTO))
  {
    src_vsb.x = src_rc.left;
    src_vsb.y = src_rc.top;
    src_vsb.w = RECTW(src_rc);
    src_vsb.h = RECTH(src_rc);

    drc_vsb.x = dst_rc.left;
    drc_vsb.y = dst_rc.top;
    drc_vsb.w = RECTW(dst_rc);
    drc_vsb.h = RECTH(dst_rc);    
    
    ret = gpe_bitblt2src(p_dst_surf->p_gpe_dev, p_src_rgn, p_dst_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
  }
  else
  {
    ret = gpe_bitblt_vsb(p_dst_surf->p_gpe_dev,
        p_dst_rgn, &dst_rc, NULL, NULL, p_src_rgn, &src_rc, &param);  
  }

  //OS_PRINTF("done\n");
  
  return ret;
}

RET_CODE surface_dert_scale(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst)
{
  RET_CODE ret = SUCCESS;
  surface_t *p_src_surf = (surface_t *)p_src_surface;
  surface_t *p_dst_surf = (surface_t *)p_dst_surface;
  void *p_src_rgn = NULL, *p_dst_rgn = NULL;
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
 // cct_gpe_dertopt_t dopt = {0};

  MT_ASSERT(p_src_surface != NULL);
  MT_ASSERT(p_dst_surface != NULL);
  MT_ASSERT(p_src != NULL);
  MT_ASSERT(p_dst != NULL);
  
  //OS_PRINTF("@# surface dert scale\n");

  //if dummy region exist, blt btwenn dummy region.
  p_dst_rgn = p_dst_surf->p_osd_rgn;
  p_src_rgn = p_src_surf->p_osd_rgn;

  src_vsb.x = p_src->left;
  src_vsb.y = p_src->top;
  src_vsb.w = RECTWP(p_src);
  src_vsb.h = RECTHP(p_dst);

  drc_vsb.x = p_dst->left;
  drc_vsb.y = p_dst->top;
  drc_vsb.w = RECTWP(p_dst);
  drc_vsb.h = RECTHP(p_dst);

 // dopt.direction = 0;
 // dopt.dert_num = 1;
 // dopt.dert_deno = 100;

  //OS_PRINTF("@# dert src : %d %d %d %d\n", src_vsb.x, src_vsb.y, src_vsb.w, src_vsb.h);
  //OS_PRINTF("@# dert dst : %d %d %d %d\n", drc_vsb.x, drc_vsb.y, drc_vsb.w, drc_vsb.h);
#ifndef WIN32  
  if(p_src_surf->chip_ic == IC_CONCERTO)
  {
    ret = gpe_dert_scale(p_dst_surf->p_gpe_dev, p_src_rgn, p_dst_rgn, 
      &src_vsb, &drc_vsb,NULL);
  }
  else
#endif  
  {
    ret = SUCCESS;
  }
  //OS_PRINTF("dert scale done!\n");
  
  return ret;
 
}

RET_CODE surface_stretch_blt(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst)
{
  RET_CODE ret = SUCCESS;
  surface_t *p_src_surf = (surface_t *)p_src_surface;
  surface_t *p_dst_surf = (surface_t *)p_dst_surface;
  void *p_src_rgn = NULL, *p_dst_rgn = NULL;

  MT_ASSERT(p_src_surface != NULL);
  MT_ASSERT(p_dst_surface != NULL);
  MT_ASSERT(p_src != NULL);
  MT_ASSERT(p_dst != NULL);
  
  //if dummy region exist, blt btwenn dummy region.
  p_dst_rgn = p_dst_surf->p_osd_rgn;
  p_src_rgn = p_src_surf->p_osd_rgn;

  //OS_PRINTF("stretch blt %d %d %d %d, %d %d %d %d\n",
  //  p_src->left, p_src->top, RECTWP(p_src), RECTHP(p_src),
  //  p_dst->left, p_dst->top, RECTWP(p_dst), RECTHP(p_dst));

  if(p_src_surf->chip_ic != IC_CONCERTO)
  {
    ret = SUCCESS;
  }
  else
  {
    ret = gpe_share_scale(p_dst_surf->p_gpe_dev, p_src_rgn, p_src, p_dst_rgn, p_dst, 0, FALSE);
  }
  
  return ret;
 
}

RET_CODE surface_trape_blt(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst, trape_t *p_trape)
{
  RET_CODE ret = SUCCESS;

  surface_t *p_src_surf = (surface_t *)p_src_surface;
  surface_t *p_dst_surf = (surface_t *)p_dst_surface;
  void *p_src_rgn = NULL, *p_dst_rgn = NULL;
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  trapez_t trapez = {0};
  cct_gpe_trapezopt_t topt = {0};

  MT_ASSERT(p_src_surface != NULL);
  MT_ASSERT(p_dst_surface != NULL);
  MT_ASSERT(p_src != NULL);
  MT_ASSERT(p_dst != NULL);
  MT_ASSERT(p_trape != NULL);
  
  p_dst_rgn = p_dst_surf->p_osd_rgn;
  p_src_rgn = p_src_surf->p_osd_rgn;

  src_vsb.x = p_src->left;
  src_vsb.y = p_src->top;
  src_vsb.w = RECTWP(p_src);
  src_vsb.h = RECTHP(p_dst);

  drc_vsb.x = p_dst->left;
  drc_vsb.y = p_dst->top;
  drc_vsb.w = RECTWP(p_dst);
  drc_vsb.h = RECTHP(p_dst);

  trapez.bottom_len = p_trape->bottom_len;
  trapez.bottom_start_x = p_trape->bottom_start_x;
  trapez.top_len = p_trape->top_len;
  trapez.top_start_x = p_trape->top_start_x;

  topt.direction = p_trape->direction;
  topt.dert_deno = p_trape->dert_deno;
  topt.dert_num = p_trape->dert_num;
  topt.enableBld = TRUE;
  topt.blendCfg.src_blend_fact = GL_SRC_ALPHA;
  topt.blendCfg.dst_blend_fact = GL_ONE_MINUS_SRC_ALPHA;

  //OS_PRINTF("trapez blt %d %d %d %d, %d %d %d %d, %d %d %d %d\n",
  //  src_vsb.x, src_vsb.y, src_vsb.w, src_vsb.h,
  //  drc_vsb.x, drc_vsb.y, drc_vsb.w, drc_vsb.h,
  //  trapez.bottom_len, trapez.bottom_start_x, trapez.top_len, trapez.top_start_x);
#ifndef WIN32  

  if(p_src_surf->chip_ic == IC_CONCERTO)
  {
    ret = gpe_draw_3d_trapez(p_dst_surf->p_gpe_dev,
      p_src_rgn, p_dst_rgn, &src_vsb, &drc_vsb, &trapez, &topt);  
  }
  else
#endif  
  {
    ret = SUCCESS;
  }
  
  return ret;
 
}

RET_CODE surface_paint_blt(void *p_dst_surface, rect_t *p_dst,
  cct_gpe_paint2opt_t *p_popt)
{
  RET_CODE ret = SUCCESS;
  surface_t *p_dst_surf = (surface_t *)p_dst_surface;
  void *p_dst_rgn = NULL;
  rect_vsb_t drc_vsb = {0};
  surface_info_t surf_info = {0};

  MT_ASSERT(p_dst_surface != NULL);
  MT_ASSERT(p_dst != NULL);
  MT_ASSERT(p_popt != NULL);
  
  p_dst_rgn = p_dst_surf->p_osd_rgn;

  drc_vsb.x = p_dst->left;
  drc_vsb.y = p_dst->top;
  drc_vsb.w = RECTWP(p_dst);
  drc_vsb.h = RECTHP(p_dst);

  //OS_PRINTF("paint blt %d %d %d %d, %d\n",
  //  drc_vsb.x, drc_vsb.y, drc_vsb.w, drc_vsb.h,
  //  p_popt->paint_cfg.paint_type);
#ifndef WIN32  
  
  if(p_dst_surf->chip_ic == IC_CONCERTO)
  {
    ret = gpe_paint_blit(p_dst_surf->p_gpe_dev,
      p_dst_rgn, &drc_vsb, &surf_info, p_popt);  
  }
  else
#endif
  {
    ret = SUCCESS;
  }

  //OS_PRINTF("paint blt done.\n");

  return ret; 
}

RET_CODE surface_rect_stroke(void *p_surface, rect_t *p_rect, u16 depth, u32 color)
{
  RET_CODE ret = SUCCESS;
  surface_t *p_surf = (surface_t *)p_surface;
  u16 h = 0, w = 0;
  cct_gpe_paint2opt_t popt = {0};
  surface_info_t surf_info = {0};
  rect_size_t rect_size = {0};
  void *p_grad_rgn = NULL, *p_grad_buffer = NULL;
  u32 buf_size = 0;
  rect_vsb_t drc = {0}, src = {0};
  cct_gpe_blt2opt_t bopt2 = {0};

  MT_ASSERT(p_surf != NULL);
  MT_ASSERT(p_rect != NULL);
  MT_ASSERT(depth > 0);

  w = RECTWP(p_rect);
  h = RECTHP(p_rect);
  
  MT_ASSERT(w >= 2 * depth);
  MT_ASSERT(h >= 2 * depth);
  MT_ASSERT(p_rect->left >= depth);
  MT_ASSERT(p_rect->top >= depth);

  region_get_rectsize(p_surf->p_osd_rgn, &rect_size);

  MT_ASSERT(p_rect->bottom + depth <= rect_size.h);
  MT_ASSERT(p_rect->right + depth <= rect_size.w);

  //create temp surface for radial gradient.
  rect_size.w = 2 * depth;
  rect_size.h = 2 * depth;

  p_grad_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

  buf_size = rect_size.w * rect_size.h * 4 + 10240;
  
  p_grad_buffer = mtos_dma_malloc(buf_size);
  MT_ASSERT(p_grad_buffer != NULL);

  ((region_t *)p_grad_rgn)->p_buf_odd = p_grad_buffer;

  //paint with radial gradient.
  popt.paint_cfg.paint_type = VG_PAINT_TYPE_RADIAL_GRADIENT;
  popt.paint_cfg.paint_radial_gradt.center.x = depth;
  popt.paint_cfg.paint_radial_gradt.center.y = depth;
  popt.paint_cfg.paint_radial_gradt.focus.x = depth;
  popt.paint_cfg.paint_radial_gradt.focus.y = depth;
  popt.paint_cfg.paint_radial_gradt.radius = depth;    
  popt.paint_cfg.paint_radial_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_PAD;
  popt.paint_cfg.paint_radial_gradt.stop0.argb = 0xFF000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_radial_gradt.stop1.argb = 0x00000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_radial_gradt.stop2.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_radial_gradt.stop3.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_radial_gradt.stop0.offset = 0;
  popt.paint_cfg.paint_radial_gradt.stop1.offset = 4095;
  popt.paint_cfg.paint_radial_gradt.stop2.offset = 4095;
  popt.paint_cfg.paint_radial_gradt.stop3.offset = 4095;  

  drc.x = 0;
  drc.y = 0;
  drc.w = 2 * depth;
  drc.h = 2 * depth;

  gpe_paint_blit(p_surf->p_gpe_dev, p_grad_rgn, &drc, &surf_info, &popt);

  bopt2.enableBld = TRUE;
  bopt2.blendCfg.src_blend_fact = GL_SRC_ALPHA;
  bopt2.blendCfg.dst_blend_fact = GL_ONE_MINUS_SRC_ALPHA;

  //blt to dst surface, first angles.

  //left-top
  src.x = 0;
  src.y = 0;
  src.w = depth;
  src.h = depth;

  drc.x = p_rect->left - depth;
  drc.y = p_rect->top - depth;
  drc.w = depth;
  drc.h = depth;
  
  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);

  //right-top
  src.x = depth;
  src.y = 0;
  src.w = depth;
  src.h = depth;

  drc.x = p_rect->right;
  drc.y = p_rect->top - depth;
  drc.w = depth;
  drc.h = depth;
  
  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);

  //left-bottom
  src.x = 0;
  src.y = depth;
  src.w = depth;
  src.h = depth;

  drc.x = p_rect->left - depth;
  drc.y = p_rect->bottom;
  drc.w = depth;
  drc.h = depth;
  
  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);    

  //right-bottom
  src.x = depth;
  src.y = depth;
  src.w = depth;
  src.h = depth;

  drc.x = p_rect->right;
  drc.y = p_rect->bottom;
  drc.w = depth;
  drc.h = depth;
  
  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);

  if(p_grad_rgn != NULL)
  {
    region_delete(p_grad_rgn);
  }  

  if(p_grad_buffer != NULL)
  {
    mtos_dma_free(p_grad_buffer);
  }  

  //left & right.
  rect_size.w = depth;
  rect_size.h = h;

  p_grad_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

  buf_size = rect_size.w * rect_size.h * 4 + 10240;
  
  p_grad_buffer = mtos_dma_malloc(buf_size);
  MT_ASSERT(p_grad_buffer != NULL);

  ((region_t *)p_grad_rgn)->p_buf_odd = p_grad_buffer;
  
  //paint linear gradient.---->left
  popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
  popt.paint_cfg.paint_liner_gradt.begin.x = 0;
  popt.paint_cfg.paint_liner_gradt.begin.y = 0;
  popt.paint_cfg.paint_liner_gradt.end.x = depth;
  popt.paint_cfg.paint_liner_gradt.end.y = 0;
  popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
  popt.paint_cfg.paint_liner_gradt.stop0.argb = 0x00000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xFF000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop0.offset = 0;
  popt.paint_cfg.paint_liner_gradt.stop1.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop2.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop3.offset = 4095;  

  drc.x = 0;
  drc.y = 0;
  drc.w = depth;
  drc.h = h;
  
  ret = gpe_paint_blit(p_surf->p_gpe_dev, p_grad_rgn, &drc, &surf_info, &popt);
  MT_ASSERT(ret == SUCCESS);

  src.x = 0;
  src.y = 0;
  src.w = depth;
  src.h = h;

  drc.x = p_rect->left - depth;
  drc.y = p_rect->top;
  drc.w = depth;
  drc.h = h;
  
  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);
  
  //paint linear gradient.---->right
  popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
  popt.paint_cfg.paint_liner_gradt.begin.x = 0;
  popt.paint_cfg.paint_liner_gradt.begin.y = 0;
  popt.paint_cfg.paint_liner_gradt.end.x = depth;
  popt.paint_cfg.paint_liner_gradt.end.y = 0;
  popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
  popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xFF000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop1.argb = 0x00000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop0.offset = 0;
  popt.paint_cfg.paint_liner_gradt.stop1.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop2.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop3.offset = 4095;  

  drc.x = 0;
  drc.y = 0;
  drc.w = depth;
  drc.h = h;  
  
  ret = gpe_paint_blit(p_surf->p_gpe_dev, p_grad_rgn, &drc, &surf_info, &popt);
  MT_ASSERT(ret == SUCCESS);  

  src.x = 0;
  src.y = 0;
  src.w = depth;
  src.h = h;

  drc.x = p_rect->right;
  drc.y = p_rect->top;
  drc.w = depth;
  drc.h = h;

  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);

  if(p_grad_rgn != NULL)
  {
    region_delete(p_grad_rgn);
  }  

  if(p_grad_buffer != NULL)
  {
    mtos_dma_free(p_grad_buffer);
  }  

  //top & bottom
  rect_size.w = w;
  rect_size.h = depth;

  p_grad_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

  buf_size = rect_size.w * rect_size.h * 4 + 10240;
  
  p_grad_buffer = mtos_dma_malloc(buf_size);
  MT_ASSERT(p_grad_buffer != NULL);

  ((region_t *)p_grad_rgn)->p_buf_odd = p_grad_buffer;

  //paint left linear gradient.---->top
  popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
  popt.paint_cfg.paint_liner_gradt.begin.x = 0;
  popt.paint_cfg.paint_liner_gradt.begin.y = 0;
  popt.paint_cfg.paint_liner_gradt.end.x = 0;
  popt.paint_cfg.paint_liner_gradt.end.y = depth;
  popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
  popt.paint_cfg.paint_liner_gradt.stop0.argb = 0x00000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xFF000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop0.offset = 0;
  popt.paint_cfg.paint_liner_gradt.stop1.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop2.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop3.offset = 4095;  

  drc.x = 0;
  drc.y = 0;
  drc.w = w;
  drc.h = depth;
  
  ret = gpe_paint_blit(p_surf->p_gpe_dev, p_grad_rgn, &drc, &surf_info, &popt);
  MT_ASSERT(ret == SUCCESS);  

  src.x = 0;
  src.y = 0;
  src.w = w;
  src.h = depth;

  drc.x = p_rect->left;
  drc.y = p_rect->top - depth;
  drc.w = w;
  drc.h = depth;

  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);
  
  //paint left linear gradient.---->bottom
  popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
  popt.paint_cfg.paint_liner_gradt.begin.x = 0;
  popt.paint_cfg.paint_liner_gradt.begin.y = 0;
  popt.paint_cfg.paint_liner_gradt.end.x = 0;
  popt.paint_cfg.paint_liner_gradt.end.y = depth;
  popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
  popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xFF000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop1.argb = 0x00000000 | (color & 0xFFFFFF);
  popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xFFFFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop0.offset = 0;
  popt.paint_cfg.paint_liner_gradt.stop1.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop2.offset = 4095;
  popt.paint_cfg.paint_liner_gradt.stop3.offset = 4095;  

  drc.x = 0;
  drc.y = 0;
  drc.w = w;
  drc.h = depth;
  
  ret = gpe_paint_blit(p_surf->p_gpe_dev, p_grad_rgn, &drc, &surf_info, &popt);
  MT_ASSERT(ret == SUCCESS);  

  src.x = 0;
  src.w = 0;
  src.w = w;
  src.h = depth;
  
  drc.x = p_rect->left;
  drc.y = p_rect->bottom;
  drc.w = w;
  drc.h = depth;
  
  gpe_bitblt2src(p_surf->p_gpe_dev,
    p_grad_rgn, p_surf->p_osd_rgn, &src, &drc, &surf_info, &surf_info, &bopt2);

  if(p_grad_rgn != NULL)
  {
    region_delete(p_grad_rgn);
  }  

  if(p_grad_buffer != NULL)
  {
    mtos_dma_free(p_grad_buffer);
  }  
  
  return SUCCESS;
}

RET_CODE surface_rotate_mirror(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, u16 dx, u16 dy, lld_gfx_rotator_t rotate_mod, lld_gfx_mirror_t mirror_mod)
{
  RET_CODE ret = SUCCESS;
  surface_t *p_src_surf = (surface_t *)p_src_surface;
  surface_t *p_dst_surf = (surface_t *)p_dst_surface;
  void *p_src_rgn = NULL, *p_dst_rgn = NULL;
  rect_vsb_t src_vsb = {0};
  pos_t dpos = {0};
  
  MT_ASSERT(p_src_surface != NULL);
  MT_ASSERT(p_dst_surface != NULL);
  MT_ASSERT(p_src != NULL);
  
  p_dst_rgn = p_dst_surf->p_osd_rgn;
  p_src_rgn = p_src_surf->p_osd_rgn;

  src_vsb.x = p_src->left;
  src_vsb.y = p_src->top;
  src_vsb.w = RECTWP(p_src);
  src_vsb.h = RECTHP(p_src);

  dpos.x = dx;
  dpos.y = dy;

  //OS_PRINTF("rotate mirror %d %d %d %d, %d %d, %d\n",
  //  src_vsb.x, src_vsb.y, src_vsb.w, src_vsb.h, dpos.x, dpos.y, mirror_mod);
#ifndef WIN32  
  if(p_src_surf->chip_ic == IC_CONCERTO)
  {
    ret = gpe_rotate_mirror(p_dst_surf->p_gpe_dev,
      p_src_rgn, p_dst_rgn, &src_vsb, &dpos, rotate_mod, mirror_mod);  
  }
  else
#endif
  {
    ret = SUCCESS;
  }
  //OS_PRINTF("rotate mirror done.\n");

  return ret;
}

RET_CODE surface_blt_3src(
  void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst,
  void *p_ref_surface, rect_t *p_ref, cct_gpe_blt3opt_t *p_opt3)
{
  RET_CODE ret = SUCCESS;
  surface_t *p_src_surf = (surface_t *)p_src_surface;
  surface_t *p_dst_surf = (surface_t *)p_dst_surface;
  surface_t *p_ref_surf = (surface_t *)p_ref_surface;
  void *p_src_rgn = NULL, *p_dst_rgn = NULL, *p_ref_rgn = NULL;
  rect_vsb_t src_vsb = {0}, drc_vsb = {0}, rrc_vsb = {0};
  surface_info_t surf_info = {0};
  u16 blt_w = 0, blt_h = 0;
  
  MT_ASSERT(p_src_surface != NULL);
  MT_ASSERT(p_dst_surface != NULL);
  MT_ASSERT(p_ref_surface != NULL);
  MT_ASSERT(p_src != NULL);
  MT_ASSERT(p_dst != NULL);
  MT_ASSERT(p_ref != NULL);
  MT_ASSERT(p_opt3 != NULL);
  
  p_dst_rgn = p_dst_surf->p_osd_rgn;
  p_src_rgn = p_src_surf->p_osd_rgn;
  p_ref_rgn = p_ref_surf->p_osd_rgn;

  blt_w = MIN(RECTWP(p_src), RECTWP(p_dst));
  blt_w = MIN(blt_w, RECTWP(p_ref));

  blt_h = MIN(RECTHP(p_src), RECTHP(p_dst));
  blt_h = MIN(blt_h, RECTHP(p_ref));
  
  src_vsb.x = p_src->left;
  src_vsb.y = p_src->top;
  src_vsb.w = blt_w;
  src_vsb.h = blt_h;

  drc_vsb.x = p_dst->left;
  drc_vsb.y = p_dst->top;
  drc_vsb.w = blt_w;
  drc_vsb.h = blt_h;

  rrc_vsb.x = p_ref->left;
  rrc_vsb.y = p_ref->top;
  rrc_vsb.w = blt_w;
  rrc_vsb.h = blt_h;
  
  //OS_PRINTF("blt 3src %d %d %d %d, %d %d %d %d, %d %d %d %d, %d\n",
  //  src_vsb.x, src_vsb.y, src_vsb.w, src_vsb.h,
  //  drc_vsb.x, drc_vsb.y, drc_vsb.w, drc_vsb.h,
  //  rrc_vsb.x, rrc_vsb.y, rrc_vsb.w, rrc_vsb.h,
  //  p_opt3->enableDrawMult);

  //OS_PRINTF("rgn 0x%x, 0x%x, 0x%x\n", (u32)p_src_rgn, (u32)p_dst_rgn, (u32)p_ref_rgn);
  if(p_src_surf->chip_ic == IC_CONCERTO)
  {
    ret = gpe_bitblt3src(p_dst_surf->p_gpe_dev,
      p_src_rgn, p_dst_rgn, p_ref_rgn, &src_vsb, &drc_vsb, &rrc_vsb,
      &surf_info, &surf_info, &surf_info, p_opt3);
  }
  else
  {
    ret = SUCCESS;
  }
  return ret;
 
}

static BOOL is_point_in_pie(
  pos_t cent, u16 radius, u16 frome_degree, u16 to_degree, pos_t pos)
{
  u16 s_degree = 0, e_degree = 0;
  double degree = 0;
  double deta_x = 0, deta_y = 0;
  
  if(frome_degree > to_degree)
  {
    s_degree = to_degree;
    e_degree = frome_degree;
  }
  else
  {
    s_degree = frome_degree;
    e_degree = to_degree;
  }

  if(ABS((s32)pos.x - (s32)cent.x) * ABS((s32)pos.x - (s32)cent.x) +
    ABS((s32)pos.y - (s32)cent.y) * ABS((s32)pos.y - (s32)cent.y) >= radius * radius)
  {
    return FALSE;
  }

  if(pos.x > cent.x)
  {
    deta_x = (double)(pos.x - cent.x);
  }
  else
  {
    deta_x = (double)(cent.x - pos.x);
  }

  if(pos.y > cent.y)
  {
    deta_y = (double)(pos.y - cent.y);
  }
  else
  {
    deta_y = (double)(cent.y - pos.y);
  }

  if(deta_x == 0)
  {
    degree = 90;
  }
  else if(deta_y == 0)
  {
    degree = 0;
  }
  else
  {
    degree = 90 - atan(deta_x / deta_y) * 180 / PI;
  }

  if(pos.x > cent.x)
  {
    if(pos.y > cent.y)
    {
      degree = 360 - degree;
    }
  }
  else
  {
    if(pos.y > cent.y)
    {
      degree = 180 + degree;
    }
    else
    {
      degree = 180 - degree;
    }
  }

  if(degree < s_degree || degree > e_degree)
  {
    return FALSE;
  }
  
  return TRUE;
}

void surface_draw_pie(void *p_surface,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u32 color)
{
  u16 start_x = 0, start_y = 0;
  u16 end_x = 0, end_y = 0;
  u16 line_s = 0, line_e = 0;
  u16 i = 0, j = 0;
  surface_t *p_surf = (surface_t *)p_surface;
  rect_size_t rgn_size = {0};
  BOOL line_start_found = FALSE;
  point_t point = {0};
  pos_t pos = {0};
  
  void *p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpe_dev);
  
  MT_ASSERT(p_surf != NULL);
  MT_ASSERT(radius > 0);
  MT_ASSERT(from_degree < 360);
  MT_ASSERT(to_degree < 360);
  
  if(cent.x > radius)
  {
    start_x = cent.x - radius;
  }
  else
  {
    start_x = 0;
  }
  
  if(cent.y > radius)
  {
    start_y = cent.y - radius;
  }
  else
  {
    start_y = 0;
  }

  region_get_rectsize(p_surf->p_osd_rgn, &rgn_size);

  if(cent.x + radius > rgn_size.w)
  {
    end_x = rgn_size.w;
  }
  else
  {
    end_x = cent.x + radius;
  }

  if(cent.y + radius > rgn_size.h)
  {
    end_y = rgn_size.h;
  }
  else
  {
    end_y = cent.y + radius;
  }

  for(i = start_y; i <= end_y; i++)
  {
    for(j = start_x; j <= end_x; j++)
    {
      pos.y = i;
      pos.x = j;
      if(is_point_in_pie(cent, radius, from_degree, to_degree, pos))
      {
        if(!line_start_found)
        {
          line_start_found = TRUE;
          line_s = j;
        }
      }
      else
      {
        if(line_start_found)
        {
          line_e = j;

          //draw line frome line_s to line_e
          point.x = line_s;
          point.y = i;

          gpe_draw_h_line_vsb(
            p_surf->p_gpe_dev, p_surf->p_osd_rgn, &point, line_e - line_s, color);
            
          line_start_found = FALSE;
        }
      }
    }
  }
}

static BOOL is_point_in_arc(
  pos_t cent, u16 radius, u16 thick, u16 frome_degree, u16 to_degree, pos_t pos)
{
  u16 s_degree = 0, e_degree = 0;
  double degree = 0;
  double deta_x = 0, deta_y = 0;
  u32 temp = 0;
  
  if(frome_degree > to_degree)
  {
    s_degree = to_degree;
    e_degree = frome_degree;
  }
  else
  {
    s_degree = frome_degree;
    e_degree = to_degree;
  }

  temp = ABS((s32)pos.x - (s32)cent.x) * ABS((s32)pos.x - (s32)cent.x) +
    ABS((s32)pos.y - (s32)cent.y) * ABS((s32)pos.y - (s32)cent.y);

  if((temp > radius * radius) || (temp < (radius - thick) * (radius - thick)))
  {
    return FALSE;
  }

  if(pos.x > cent.x)
  {
    deta_x = (double)(pos.x - cent.x);
  }
  else
  {
    deta_x = (double)(cent.x - pos.x);
  }

  if(pos.y > cent.y)
  {
    deta_y = (double)(pos.y - cent.y);
  }
  else
  {
    deta_y = (double)(cent.y - pos.y);
  }

  if(deta_x == 0)
  {
    degree = 90;
  }
  else if(deta_y == 0)
  {
    degree = 0;
  }
  else
  {
    degree = 90 - atan(deta_x / deta_y) * 180 / PI;
  }

  if(pos.x > cent.x)
  {
    if(pos.y > cent.y)
    {
      degree = 360 - degree;
    }
  }
  else
  {
    if(pos.y > cent.y)
    {
      degree = 180 + degree;
    }
    else
    {
      degree = 180 - degree;
    }
  }

  if(degree < s_degree || degree > e_degree)
  {
    return FALSE;
  }
  
  return TRUE;
}

void surface_draw_arc(void *p_surface,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u16 thick, u32 color)
{
  u16 start_x = 0, start_y = 0;
  u16 end_x = 0, end_y = 0;
  u16 i = 0, j = 0;
  surface_t *p_surf = (surface_t *)p_surface;
  rect_size_t rgn_size = {0};
  BOOL is_line_start = FALSE;
  point_t point = {0};
  pos_t pos = {0};
  u32 draw_color = 0;
  
  void *p_gpe_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpe_dev);
  
  MT_ASSERT(p_surf != NULL);
  MT_ASSERT(radius > 0);
  MT_ASSERT(from_degree < 360);
  MT_ASSERT(to_degree < 360);
  
  if(cent.x > radius)
  {
    start_x = cent.x - radius;
  }
  else
  {
    start_x = 0;
  }
  
  if(cent.y > radius)
  {
    start_y = cent.y - radius;
  }
  else
  {
    start_y = 0;
  }

  region_get_rectsize(p_surf->p_osd_rgn, &rgn_size);

  if(cent.x + radius > rgn_size.w)
  {
    end_x = rgn_size.w;
  }
  else
  {
    end_x = cent.x + radius;
  }

  if(cent.y + radius > rgn_size.h)
  {
    end_y = rgn_size.h;
  }
  else
  {
    end_y = cent.y + radius;
  }

  for(i = start_y; i <= end_y; i++)
  {
    is_line_start = FALSE;
    for(j = start_x; j <= end_x; j++)
    {
      pos.y = i;
      pos.x = j;
      if(is_point_in_arc(cent, radius, thick, from_degree, to_degree, pos))
      {
        if(!is_line_start || (j == end_x))
        {
          is_line_start = TRUE;


          //alpha = alpha / 2 for first point to anti-aliasing;
          draw_color = ((((color >> 25) & 0xFF) << 24) | (color & 0xFFFFFF)); 
        }
        else
        {
          draw_color = color;
        }
        
        point.x = j;
        point.y = i;
        gpe_draw_pixel_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &point, draw_color);
      }
      else
      {
        if(is_line_start)
        {
          is_line_start = FALSE;

          //alpha = alpha / 2 for last point to anti-aliasing;
          draw_color = ((((color >> 25) & 0xFF) << 24) | (color & 0xFFFFFF)); 
          pos.y = i;
          pos.x = j - 1;

          gpe_draw_pixel_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn, &point, draw_color);
        }
      }
    }
  }
}

u8 surface_get_attr(void *p_surface)
{
  surface_t *p_surf = (surface_t *)p_surface;

  MT_ASSERT(p_surf != NULL);
  
  return p_surf->attr;
}

s32 surface_get_handle(void *p_surface)
{
  surface_t *p_surf = (surface_t *)p_surface;
  s32 handle = 0;
  
  MT_ASSERT(p_surf != NULL);

  handle = (s32)p_surf->p_osd_rgn;

  return handle;
}

u8 surface_get_bpp(void *p_surface)
{
  surface_t *p_surf = (surface_t *)p_surface;
  
  MT_ASSERT(p_surf != NULL);
  
  return p_surf->bpp;
}

u8 surface_get_format(void *p_surface)
{
  surface_t *p_surf = (surface_t *)p_surface;
  u8 format = 0;

  MT_ASSERT(p_surf != NULL);

  switch(p_surf->format)
  {
    case PIX_FMT_RGBPALETTE4:
      format = COLORFORMAT_RGB4BIT;
      break;
      
    case PIX_FMT_RGBPALETTE8:
      format = COLORFORMAT_RGB8BIT;
      break;
      
    case PIX_FMT_RGB565:
      format = COLORFORMAT_RGB565;
      break;
      
    case PIX_FMT_RGBA5551:
      format = COLORFORMAT_RGBA5551;
      break;
      
    case PIX_FMT_ARGB1555:
      format = COLORFORMAT_ARGB1555;
      break;
      
    case PIX_FMT_RGBA8888:
      format = COLORFORMAT_RGBA8888;
      break;
      
    case PIX_FMT_ARGB8888:
      format = COLORFORMAT_ARGB8888;
      break;      
      
    default:
      MT_ASSERT(0);
      break;
  }
  
  return format;
}

palette_t *surface_get_palette_addr(void *p_surface)
{
  surface_t *p_surf = (surface_t *)p_surface;

  MT_ASSERT(p_surf != NULL);
  
  return p_surf->p_pal;
}

void surface_start_batch(void *p_surface)
{
  surface_t *p_surf = (surface_t *)p_surface;
  
  MT_ASSERT(p_surf != NULL);

  gpe_start_batch_vsb(p_surf->p_gpe_dev, p_surf->p_osd_rgn);
}

void surface_end_batch(void *p_surface, BOOL is_sync, rect_t *p_rect)
{
  surface_t *p_surf = (surface_t *)p_surface;
  //gpe_param_vsb_t param = {0};
  void *p_rgn = NULL;
  rect_t rect = {0};

  MT_ASSERT(p_surf != NULL);

  p_rgn = p_surf->p_osd_rgn;

  MT_ASSERT(p_rgn != NULL);

  if(p_rect == NULL)
  {
    disp_layer_update_region(p_surf->p_disp, p_rgn, NULL);

    return;
  }

  rect.left = p_rect->left;
  rect.top = p_rect->top;
  rect.right = p_rect->right;
  rect.bottom = p_rect->bottom; 
  
  switch(p_surf->chip_ic)
  {
    case IC_SONATA:
    case IC_WARRIORS:
      if(rect.left < 4)
      {
        rect.left = 0;
      }
      else
      {
        rect.left = (rect.left + 3) / 4 * 4 - 4;
      }

      if(rect.right < 4)
      {
        rect.right = 0;
      }
      else
      {
        rect.right = (rect.right + 3) / 4 * 4;
      }

      rect.top = 0;

      #ifdef __LINUX__
      if(p_rgn < 0x80000000)
      {
        rect.bottom = ((region_t *)p_rgn)->rect.h;
      }
      #else
      rect.bottom = ((region_t *)p_rgn)->rect.h;
      #endif

      if(!is_invalid_rect(&rect))
      {
        disp_layer_update_region(p_surf->p_disp, p_rgn, &rect);
      }    
      break;

    //case IC_SONATA:
    case IC_CONCERTO:
      if(!is_invalid_rect(&rect))
      {
        disp_layer_update_region(p_surf->p_disp, p_rgn, &rect);
      }        
      break;
      
    default:
      gpe_end_batch_vsb(p_surf->p_gpe_dev, p_rgn, is_sync);
      break;
  }
}

