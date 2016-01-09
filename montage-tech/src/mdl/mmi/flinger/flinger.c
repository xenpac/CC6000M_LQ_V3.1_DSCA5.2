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

#include "class_factory.h"

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

#include "mmi.h"
#include "mdl.h"

typedef struct
{
  /*!
    flinger rect.
    */
  rect_t flinger_rect;    
  /*!
    surface count.
    */
  u32 surf_cnt;
  /*!
    surface list.
    */
  u32 *p_surf;
}flinger_t;

void *flinger_create(flinger_param_t *p_param)
{
  surface_desc_t surf_desc = {0};
  RET_CODE ret = 0;
  flinger_t *p_flinger = NULL;
  rect_t orc = {0};
  rect_t *p_rect = NULL;
  u32 i = 0;
  u8 attr = 0;
  void *p_surf = NULL;

  MT_ASSERT(p_param != NULL);
  MT_ASSERT(p_param->p_cfg != NULL);
  MT_ASSERT(p_param->p_flinger_rect != NULL);

  p_flinger = mmi_alloc_buf(sizeof(flinger_t));
  MT_ASSERT(p_flinger != NULL);

  p_flinger->surf_cnt = p_param->p_cfg->rect_cnt;

  p_flinger->p_surf = mmi_alloc_buf(p_flinger->surf_cnt * sizeof(void *));
  MT_ASSERT(p_flinger->p_surf != NULL);

  copy_rect(&p_flinger->flinger_rect, p_param->p_flinger_rect);
  normalize_rect(&p_flinger->flinger_rect);
  
  for(i = 0; i < p_param->p_cfg->rect_cnt; i++)
  {
    p_rect = p_param->p_cfg->p_rgn_rect + i;

    normalize_rect(p_rect);

    MT_ASSERT(p_rect->left >= 0);
    MT_ASSERT(p_rect->top >= 0);
    MT_ASSERT(p_rect->right <= RECTW(p_flinger->flinger_rect));
    MT_ASSERT(p_rect->bottom <= RECTH(p_flinger->flinger_rect));

    offset_rect(p_rect, p_flinger->flinger_rect.left, p_flinger->flinger_rect.top);

    surf_desc.flag =
      SURFACE_DESC_CAPS | SURFACE_DESC_FORMAT |
      SURFACE_DESC_WIDTH | SURFACE_DESC_HEIGHT |
      SURFACE_DESC_X | SURFACE_DESC_Y;
    surf_desc.width = RECTWP(p_rect);
    surf_desc.height = RECTHP(p_rect);
    surf_desc.caps = SURFACE_CAPS_MEM_SYSTEM;
    surf_desc.p_pal = p_param->p_pal;
    surf_desc.format = p_param->format;
    surf_desc.x = p_rect->left;
    surf_desc.y = p_rect->top;
    surf_desc.layer = p_param->p_cfg->layer;
    

    ret = surface_create(&p_surf, &surf_desc);
    MT_ASSERT(ret == SUCCESS);   

    *(p_flinger->p_surf + i) = (u32)p_surf;

    attr = surface_get_attr(p_surf);
    if(attr & SURFACE_ATTR_EN_PALETTE)
    {
      ret = surface_set_palette(p_surf, 0, p_param->p_pal->cnt, p_param->p_pal->p_entry);
      MT_ASSERT(ret == SUCCESS);
    }
    
    surface_set_srcrect(p_surf, p_rect);

    ret = surface_set_colorkey(p_surf, p_param->ckey);
    MT_ASSERT(ret == SUCCESS);

    ret = surface_set_trans_clr(p_surf, p_param->ckey);
    MT_ASSERT(ret == SUCCESS);
    
    ret = surface_set_trans(p_surf, p_param->cdef, 0);
    MT_ASSERT(ret == SUCCESS);
    
    set_rect(&orc, 0, 0, RECTWP(p_rect), RECTHP(p_rect));

    surface_start_batch(p_surf);
    
    ret = surface_fill_rect(p_surf, &orc, p_param->cdef);
    MT_ASSERT(ret == SUCCESS);

    surface_end_batch(p_surf, TRUE, &orc);

    ret = surface_set_display(p_surf, TRUE);
    MT_ASSERT(ret == SUCCESS);

    surface_enable(p_surf, TRUE);
  }
  
  return p_flinger;
}

void *flinger_create_virtual(flinger_virtual_param_t *p_param)
{
  surface_desc_t surf_desc = {0};
  RET_CODE ret = 0;
  flinger_t *p_flinger = NULL;
  u32 i = 0;
  void *p_surf = NULL;
  rect_t *p_rect = NULL;
  
  MT_ASSERT(p_param != NULL);
  MT_ASSERT(p_param->p_cfg != NULL);
  MT_ASSERT(p_param->p_cfg->p_rgn_rect != NULL);
  MT_ASSERT(p_param->p_flinger_rect != NULL);
  
  p_flinger = mmi_alloc_buf(sizeof(flinger_t));
  MT_ASSERT(p_flinger != NULL);

  p_flinger->surf_cnt = p_param->p_cfg->rect_cnt;

  p_flinger->p_surf = mmi_alloc_buf(p_flinger->surf_cnt * sizeof(u32 *));
  MT_ASSERT(p_flinger->p_surf != NULL);

  copy_rect(&p_flinger->flinger_rect, p_param->p_flinger_rect);
  normalize_rect(&p_flinger->flinger_rect);
  
  for(i = 0; i < p_param->p_cfg->rect_cnt; i++)
  {
    p_rect = p_param->p_cfg->p_rgn_rect + i;

    normalize_rect(p_rect);

    MT_ASSERT(p_rect->left >= 0);
    MT_ASSERT(p_rect->top >= 0);
    MT_ASSERT(p_rect->right <= RECTW(p_flinger->flinger_rect));
    MT_ASSERT(p_rect->bottom <= RECTH(p_flinger->flinger_rect));

    offset_rect(p_rect, p_flinger->flinger_rect.left, p_flinger->flinger_rect.top);
    
    surf_desc.flag =
      SURFACE_DESC_CAPS | SURFACE_DESC_FORMAT | SURFACE_DESC_WIDTH |
      SURFACE_DESC_HEIGHT | SURFACE_DESC_PITCH |
      SURFACE_DESC_X | SURFACE_DESC_Y;;
    surf_desc.width = RECTWP(p_rect);
    surf_desc.height = RECTHP(p_rect);
    surf_desc.caps = SURFACE_CAPS_MEM_ASSIGN;
    surf_desc.p_pal = p_param->p_pal;
    surf_desc.pitch = p_param->pitch;
    surf_desc.format = p_param->format;
    surf_desc.x = p_rect->left;
    surf_desc.y = p_rect->top;
    surf_desc.p_vmem[0] = p_param->p_virtual_buf;
    surf_desc.layer = p_param->p_cfg->layer;
    
    ret = surface_create(&p_surf, &surf_desc);
    MT_ASSERT(ret == SUCCESS);   

    *(p_flinger->p_surf + i) = (u32)p_surf;
    
  }
  
  return p_flinger;
}

RET_CODE flinger_delete(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  u32 i = 0;
  
  //MT_ASSERT(p_flinger != NULL);
  if(p_flinger == NULL)
  {
    return ERR_FAILURE;
  }
  
  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
  
    surface_delete(p_surf);
  }

  mmi_free_buf(p_mflinger->p_surf);
  
  mmi_free_buf(p_flinger);
  
  return SUCCESS;
}

void flinger_enable(void *p_flinger, BOOL is_enable)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    surface_enable(p_surf, is_enable);
  }
  
  return;
}

void flinger_set_display(void *p_flinger, BOOL is_display)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_set_display(p_surf, is_display);

    MT_ASSERT(ret == SUCCESS);
  }

  return;
}

void flinger_set_trans(void *p_flinger, u32 index, u8 alpha)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_set_trans(p_surf, index, alpha);

    MT_ASSERT(ret == SUCCESS);
  }

  return;
}

void flinger_set_alpha(void *p_flinger, u8 alpha)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_set_alpha(p_surf, alpha);

    MT_ASSERT(ret == SUCCESS);
  }

  return;
}

void flinger_offset(void *p_flinger, s16 x_off, s16 y_off)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  rect_t surf_rect = {0};
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_get_srcrect(p_surf, &surf_rect);
    MT_ASSERT(ret == SUCCESS);
    
    offset_rect(&surf_rect, x_off, y_off);

    ret = surface_set_srcrect(p_surf, &surf_rect);
    MT_ASSERT(ret == SUCCESS);
  }

  return;
}

void flinger_set_cliprect(void *p_flinger, rect_t *p_rc)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  rect_t surf_rect = {0}, erc = {0};
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    if(p_rc == NULL)
    {
      surface_set_cliprect(p_surf, NULL);
    }
    else
    {
      ret = surface_get_srcrect(p_surf, &surf_rect);
      MT_ASSERT(ret == SUCCESS);

      offset_rect(&surf_rect, 0 - p_mflinger->flinger_rect.left, 0 - p_mflinger->flinger_rect.top);
      
      if(intersect_rect(&erc, &surf_rect, p_rc))
      {
        offset_rect(&erc, 0 - surf_rect.left, 0 - surf_rect.top);
        ret = surface_set_cliprect(p_surf, &erc);
        MT_ASSERT(ret == SUCCESS);
      }
      else
      {
        empty_rect(&erc);
        ret = surface_set_cliprect(p_surf, &erc);
        MT_ASSERT(ret == SUCCESS);
      }
    }
  }

  return;
}

void flinger_set_colorkey(void *p_flinger, u32 ckey)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_set_colorkey(p_surf, ckey);
    MT_ASSERT(ret == SUCCESS);
  }

  return;
}

void flinger_set_palette(void *p_flinger, u16 start, u16 len, color_t *p_entry)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_set_palette(p_surf, start, len, p_entry);
    MT_ASSERT(ret == SUCCESS);
  }

  return;
}

u8 flinger_get_attr(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  p_surf = (void *)(*p_mflinger->p_surf);

  return surface_get_attr(p_surf);
}

u32 flinger_get_colorkey(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  void *p_surf = NULL;
  u32 ckey = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  p_surf = (void *)(*p_mflinger->p_surf);

  ret = surface_get_colorkey(p_surf, &ckey);
  MT_ASSERT(ret == SUCCESS);
  
  return ckey;
}

u8 flinger_get_bpp(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  p_surf = (void *)(*p_mflinger->p_surf);

  return surface_get_bpp(p_surf);
}

pix_type_t flinger_get_format(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  p_surf = (void *)(*p_mflinger->p_surf);

  return surface_get_format(p_surf);
}


palette_t *flinger_get_palette_addr(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  p_surf = (void *)(*p_mflinger->p_surf);

  return surface_get_palette_addr(p_surf);
}

void flinger_get_palette(void *p_flinger, u16 start, u16 len, color_t *p_entry)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  p_surf = (void *)(*p_mflinger->p_surf);

  ret = surface_get_palette(p_surf, start, len, p_entry);
  MT_ASSERT(ret == SUCCESS);

  return;
  
}

rect_t *flinger_get_rect(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  return &p_mflinger->flinger_rect;
}

void flinger_bitblt(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy, flinger_rop_t flinger_rop, u32 rop_pat)
{
  flinger_t *p_mflinger_src = NULL, *p_mflinger_dst = NULL;
  RET_CODE ret = SUCCESS;
  rect_t src_surf_rect = {0}, dst_surf_rect = {0};
  rect_t src_blt_rect = {0}, dst_blt_rect = {0};
  rect_t erc = {0}, orc = {0}, src = {0};
  u32 i = 0, j = 0;
  u32 src_xoff = 0, src_yoff = 0, dst_xoff = 0, dst_yoff = 0;  
  surface_rop_type_t surf_rop = SURFACE_ROP_SET;
  void *p_src_surf = NULL, *p_dst_surf = NULL;
  
  MT_ASSERT(p_src_flinger != NULL);
  MT_ASSERT(p_dst_flinger != NULL);
  
  p_mflinger_src = (flinger_t *)p_src_flinger;
  p_mflinger_dst = (flinger_t *)p_dst_flinger;

  MT_ASSERT(p_mflinger_src->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->p_surf != NULL);

  src_blt_rect.left = sx;
  src_blt_rect.top = sy;
  src_blt_rect.right = sx + sw;
  src_blt_rect.bottom = sy + sh;

  offset_rect(&src_blt_rect, p_mflinger_src->flinger_rect.left, p_mflinger_src->flinger_rect.top);

  switch(flinger_rop)
  {
    case FLINGER_ROP_OR:
      surf_rop = SURFACE_ROP_OR;
      break;

    case FLINGER_ROP_XOR:
      surf_rop = SURFACE_ROP_XOR;
      break;

    case FLINGER_ROP_AND:
      surf_rop = SURFACE_ROP_AND;
      break;

    case FLINGER_ROP_PATCOPY:
      surf_rop = SURFACE_ROP_PATCOPY;
      break;

    case FLINGER_ROP_BLEND_DST:
      surf_rop = SURFACE_ROP_BLEND_DST;
      break;

    case FLINGER_ROP_BLEND_SRC:
      surf_rop = SURFACE_ROP_BLEND_SRC;
      break;

    default:
      surf_rop = SURFACE_ROP_SET;
      break;
  }

  for(i = 0; i < p_mflinger_src->surf_cnt; i++)
  {
    p_src_surf = (void *)(*(p_mflinger_src->p_surf + i));
    ret = surface_get_srcrect(p_src_surf, &src_surf_rect);
    MT_ASSERT(ret == SUCCESS);

    if(intersect_rect(&erc, &src_blt_rect, &src_surf_rect))
    {
      dst_blt_rect.left = dx;
      dst_blt_rect.top = dy;
      dst_blt_rect.right = dst_blt_rect.left + RECTW(erc);
      dst_blt_rect.bottom = dst_blt_rect.top + RECTH(erc);

       offset_rect(&dst_blt_rect,
        p_mflinger_dst->flinger_rect.left, p_mflinger_dst->flinger_rect.top);
     
      for(j = 0; j < p_mflinger_dst->surf_cnt; j++)
      {
        p_dst_surf = (void *)(*(p_mflinger_dst->p_surf + j));

        ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
        MT_ASSERT(ret == SUCCESS);

        if(intersect_rect(&orc, &dst_blt_rect, &dst_surf_rect))
        {
          copy_rect(&src, &erc);

          if((dy + p_mflinger_dst->flinger_rect.top) >= dst_surf_rect.top)
          {
            src_yoff = 0;
          }
          else
          {
            src_yoff = (dst_surf_rect.top - (dy + p_mflinger_dst->flinger_rect.top));
          }

          if((dx + p_mflinger_dst->flinger_rect.left) >= dst_surf_rect.left)
          {
            src_xoff = 0;
          }
          else
          {
            src_xoff = (dst_surf_rect.left - (dx + p_mflinger_dst->flinger_rect.left));
          }

          src.left += src_xoff;
          src.top += src_yoff;
          
          offset_rect(&src, 0 - src_surf_rect.left, 0 - src_surf_rect.top);


          if((sy + p_mflinger_src->flinger_rect.top) >= src_surf_rect.top)
          {
            dst_yoff = 0;
          }
          else
          {
            dst_yoff = (src_surf_rect.top - (sy + p_mflinger_src->flinger_rect.top));
          }

          if((sx + p_mflinger_src->flinger_rect.left) >= src_surf_rect.left)
          {
            dst_xoff = 0;
          }
          else
          {
            dst_xoff = (src_surf_rect.left - (sx + p_mflinger_src->flinger_rect.left));
          }
          
          offset_rect(&orc, dst_xoff, dst_yoff);
          offset_rect(&orc, 0 - dst_surf_rect.left, 0 - dst_surf_rect.top);
          
          ret = surface_bitblt(p_src_surf,
            src.left, src.top, RECTW(src), RECTH(src),
            p_dst_surf,
            orc.left, orc.top, surf_rop, rop_pat);

          MT_ASSERT(ret == SUCCESS);
        }
      }
    }
  } 
  
  return;
}

void flinger_fill_rect(void *p_flinger, rect_t *p_rc, u32 value)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  rect_t surf_rect = {0}, eff_rect = {0};
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);
  MT_ASSERT(p_rc != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_get_srcrect(p_surf, &surf_rect);
    MT_ASSERT(ret == SUCCESS);

    offset_rect(&surf_rect, 0 - p_mflinger->flinger_rect.left, 0 - p_mflinger->flinger_rect.top);
    
    if(intersect_rect(&eff_rect, &surf_rect, p_rc))
    {
      offset_rect(&eff_rect, 0 - surf_rect.left, 0 - surf_rect.top);
      surface_fill_rect(p_surf, &eff_rect, value);
    }
  }  
  
  return;
}

void flinger_fill_bmp(void *p_flinger, rect_t *p_rc, bitmap_t *p_bmp)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  rect_t surf_rect = {0}, eff_rect = {0};
  void *p_surf = NULL;
  s16 x_off = 0, y_off = 0;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);
  MT_ASSERT(p_rc != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_get_srcrect(p_surf, &surf_rect);
    MT_ASSERT(ret == SUCCESS);

    offset_rect(&surf_rect, 0 - p_mflinger->flinger_rect.left, 0 - p_mflinger->flinger_rect.top);

    if(intersect_rect(&eff_rect, &surf_rect, p_rc))
    {
      offset_rect(&eff_rect, 0 - surf_rect.left, 0 - surf_rect.top);

      //caculate bmap fill rect offset.
      x_off = (surf_rect.left > p_rc->left) ? (surf_rect.left - p_rc->left) : 0;
      y_off = (surf_rect.top > p_rc->top) ? ((surf_rect.top - p_rc->top)) : 0;
      
      surface_fill_bmp(p_surf, &eff_rect, p_bmp, x_off, y_off);
    }
  }   

  return;
}

void flinger_get_bits(void *p_flinger, rect_t *p_rc, void *p_buf, u32 pitch)
{
  flinger_t *p_mflinger = NULL;
  RET_CODE ret = SUCCESS;
  rect_t surf_rect = {0}, eff_rect = {0};
  void *p_surf = NULL;
  u32 i = 0;
  u8 bpp = 0;
  u8 *p_buffer = NULL;
  
  MT_ASSERT(p_flinger != NULL);
  MT_ASSERT(p_rc != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  bpp = flinger_get_bpp(p_flinger);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    ret = surface_get_srcrect(p_surf, &surf_rect);
    MT_ASSERT(ret == SUCCESS);

    if(intersect_rect(&eff_rect, &surf_rect, p_rc))
    {
      p_buffer = (u8 *)p_buf +
        (eff_rect.top - p_rc->top) * pitch + (eff_rect.left - p_rc->left) * bpp;
        
      surface_get_bits(p_surf, &eff_rect, (void *)p_buffer, pitch);
    }
  }   

  return;
}

s32 flinger_get_surface(void *p_flinger, u32 idx)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  MT_ASSERT(idx < p_mflinger->surf_cnt);

  p_surf = (void *)(*(p_mflinger->p_surf + idx));

  return surface_get_handle(p_surf);
}


void flinger_start_batch(void *p_flinger)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  u32 i = 0;
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    surface_start_batch(p_surf);
  }
  
  return;
}

void flinger_end_batch(void *p_flinger, BOOL is_sync, rect_t *p_rect)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  u32 i = 0;
  RET_CODE ret = SUCCESS;
  rect_t surf_rect = {0}, eff_rect = {0};
  
  MT_ASSERT(p_flinger != NULL);

  p_mflinger = (flinger_t *)p_flinger;

  MT_ASSERT(p_mflinger->p_surf != NULL);

  for(i = 0; i < p_mflinger->surf_cnt; i++)
  {
    //p_surf = (void *)(*(p_mflinger->p_surf + i));
    
    //surface_end_batch(p_surf, is_sync, p_rect);

    p_surf = (void *)(*(p_mflinger->p_surf + i));

    if(p_rect != NULL)
    {
      ret = surface_get_srcrect(p_surf, &surf_rect);
      MT_ASSERT(ret == SUCCESS);

      offset_rect(&surf_rect, 0 - p_mflinger->flinger_rect.left, 0 - p_mflinger->flinger_rect.top);
      
      if(intersect_rect(&eff_rect, &surf_rect, p_rect))
      {
        offset_rect(&eff_rect, 0 - surf_rect.left, 0 - surf_rect.top);
        surface_end_batch(p_surf, is_sync, &eff_rect);
      }
    }
    else
    {
      surface_end_batch(p_surf, is_sync, NULL);
    }
  }
  
  return;
}

void flinger_dert_scale(void *p_src_flinger, rect_t *p_src, void *p_dst_flinger, rect_t *p_dst)
{
  flinger_t *p_mflinger_src = NULL, *p_mflinger_dst = NULL;
  void *p_src_surf = NULL, *p_dst_surf = NULL;
  rect_t src_surf_rect = {0}, dst_surf_rect = {0};
  rect_t erc = {0}, orc = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_src_flinger != NULL);
  MT_ASSERT(p_dst_flinger != NULL);
  MT_ASSERT(p_src != NULL);
  MT_ASSERT(p_dst != NULL);
  
  p_mflinger_src = (flinger_t *)p_src_flinger;
  p_mflinger_dst = (flinger_t *)p_dst_flinger;

  MT_ASSERT(p_mflinger_src->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->p_surf != NULL);
  MT_ASSERT(p_mflinger_src->surf_cnt == 1);
  MT_ASSERT(p_mflinger_dst->surf_cnt == 1);

  p_src_surf = (void *)(*(p_mflinger_src->p_surf));
  ret = surface_get_srcrect(p_src_surf, &src_surf_rect);
  MT_ASSERT(ret == SUCCESS);

 offset_rect(&src_surf_rect,
  0 - p_mflinger_src->flinger_rect.left, 0 - p_mflinger_src->flinger_rect.top);
  

  if(intersect_rect(&erc, p_src, &src_surf_rect))
  {
    p_dst_surf = (void *)(*(p_mflinger_dst->p_surf));

    ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
    MT_ASSERT(ret == SUCCESS);

    offset_rect(&dst_surf_rect,
      0 - p_mflinger_dst->flinger_rect.left, 0 - p_mflinger_dst->flinger_rect.top);

    if(intersect_rect(&orc, p_dst, &dst_surf_rect))
    {
      ret = surface_dert_scale(p_src_surf, &erc, p_dst_surf, &orc);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  
  return;
}

void flinger_stretch_blt(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy, u16 dw, u16 dh)
{
  flinger_t *p_mflinger_src = NULL, *p_mflinger_dst = NULL;
  void *p_src_surf = NULL, *p_dst_surf = NULL;
  rect_t src_surf_rect = {0}, dst_surf_rect = {0};
  rect_t erc = {0}, orc = {0};
  rect_t src = {0}, drc = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_src_flinger != NULL);
  MT_ASSERT(p_dst_flinger != NULL);
  
  p_mflinger_src = (flinger_t *)p_src_flinger;
  p_mflinger_dst = (flinger_t *)p_dst_flinger;

  MT_ASSERT(p_mflinger_src->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->p_surf != NULL);
  MT_ASSERT(p_mflinger_src->surf_cnt == 1);
  MT_ASSERT(p_mflinger_dst->surf_cnt == 1);

  p_src_surf = (void *)(*(p_mflinger_src->p_surf));
  ret = surface_get_srcrect(p_src_surf, &src_surf_rect);
  MT_ASSERT(ret == SUCCESS);

 offset_rect(&src_surf_rect,
  0 - p_mflinger_src->flinger_rect.left, 0 - p_mflinger_src->flinger_rect.top);

  src.left = sx;
  src.top = sy;
  src.right = sx + sw;
  src.bottom = sy + sh;

  if(intersect_rect(&erc, &src, &src_surf_rect))
  {
    p_dst_surf = (void *)(*(p_mflinger_dst->p_surf));

    ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
    MT_ASSERT(ret == SUCCESS);

    offset_rect(&dst_surf_rect,
      0 - p_mflinger_dst->flinger_rect.left, 0 - p_mflinger_dst->flinger_rect.top);

    drc.left = dx;
    drc.top = dy;
    drc.right = dx + dw;
    drc.bottom = dy + dh;
    
    if(intersect_rect(&orc, &drc, &dst_surf_rect))
    {
      ret = surface_stretch_blt(p_src_surf, &erc, p_dst_surf, &orc);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  
  return;
}

void flinger_trape_blt(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy, u16 dw, u16 dh, trape_t *p_trape)
{
  flinger_t *p_mflinger_src = NULL, *p_mflinger_dst = NULL;
  void *p_src_surf = NULL, *p_dst_surf = NULL;
  rect_t src_surf_rect = {0}, dst_surf_rect = {0};
  rect_t erc = {0}, orc = {0};
  rect_t src = {0}, drc = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_src_flinger != NULL);
  MT_ASSERT(p_dst_flinger != NULL);
  
  p_mflinger_src = (flinger_t *)p_src_flinger;
  p_mflinger_dst = (flinger_t *)p_dst_flinger;

  MT_ASSERT(p_mflinger_src->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->p_surf != NULL);
  MT_ASSERT(p_mflinger_src->surf_cnt == 1);
  MT_ASSERT(p_mflinger_dst->surf_cnt == 1);

  p_src_surf = (void *)(*(p_mflinger_src->p_surf));
  ret = surface_get_srcrect(p_src_surf, &src_surf_rect);
  MT_ASSERT(ret == SUCCESS);

  offset_rect(&src_surf_rect,
    0 - p_mflinger_src->flinger_rect.left, 0 - p_mflinger_src->flinger_rect.top);

  src.left = sx;
  src.top = sy;
  src.right = sx + sw;
  src.bottom = sy + sh;

  if(intersect_rect(&erc, &src, &src_surf_rect))
  {
    p_dst_surf = (void *)(*(p_mflinger_dst->p_surf));

    ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
    MT_ASSERT(ret == SUCCESS);

    offset_rect(&dst_surf_rect,
      0 - p_mflinger_dst->flinger_rect.left, 0 - p_mflinger_dst->flinger_rect.top);

    drc.left = dx;
    drc.top = dy;
    drc.right = dx + dw;
    drc.bottom = dy + dh;
    
    if(intersect_rect(&orc, &drc, &dst_surf_rect))
    {
      ret = surface_trape_blt(p_src_surf, &erc, p_dst_surf, &orc, p_trape);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  
  return;
}

void flinger_paint_blt(void *p_dst_flinger,
  u16 dx, u16 dy, u16 dw, u16 dh, cct_gpe_paint2opt_t *p_popt)
{
  flinger_t *p_mflinger_dst = NULL;
  void *p_dst_surf = NULL;
  rect_t dst_surf_rect = {0};
  rect_t orc = {0}, drc = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_dst_flinger != NULL);
  
  p_mflinger_dst = (flinger_t *)p_dst_flinger;

  MT_ASSERT(p_mflinger_dst->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->surf_cnt == 1);

  p_dst_surf = (void *)(*(p_mflinger_dst->p_surf));

  ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
  MT_ASSERT(ret == SUCCESS);

  offset_rect(&dst_surf_rect,
    0 - p_mflinger_dst->flinger_rect.left, 0 - p_mflinger_dst->flinger_rect.top);

  drc.left = dx;
  drc.top = dy;
  drc.right = dx + dw;
  drc.bottom = dy + dh;
  
  if(intersect_rect(&orc, &drc, &dst_surf_rect))
  {
    ret = surface_paint_blt(p_dst_surf, &orc, p_popt);
    MT_ASSERT(ret == SUCCESS);
  }
  
  return;
}

void flinger_rect_stroke(void *p_dst_flinger,
  u16 dx, u16 dy, u16 dw, u16 dh, u16 depth, u32 color)
{
  flinger_t *p_mflinger_dst = NULL;
  void *p_dst_surf = NULL;
  rect_t dst_surf_rect = {0};
  rect_t orc = {0}, drc = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_dst_flinger != NULL);
  
  p_mflinger_dst = (flinger_t *)p_dst_flinger;

  MT_ASSERT(p_mflinger_dst->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->surf_cnt == 1);

  p_dst_surf = (void *)(*(p_mflinger_dst->p_surf));

  ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
  MT_ASSERT(ret == SUCCESS);

  offset_rect(&dst_surf_rect,
    0 - p_mflinger_dst->flinger_rect.left, 0 - p_mflinger_dst->flinger_rect.top);

  drc.left = dx;
  drc.top = dy;
  drc.right = dx + dw;
  drc.bottom = dy + dh;
  
  if(intersect_rect(&orc, &drc, &dst_surf_rect))
  {
    ret = surface_rect_stroke(p_dst_surf, &orc, depth, color);
    MT_ASSERT(ret == SUCCESS);
  }
  
  return;
}

void flinger_rotate_mirror(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy,
  lld_gfx_rotator_t rotate_mod, lld_gfx_mirror_t mirror_mod)
{
  flinger_t *p_mflinger_src = NULL, *p_mflinger_dst = NULL;
  void *p_src_surf = NULL, *p_dst_surf = NULL;
  rect_t src_surf_rect = {0}, dst_surf_rect = {0};
  rect_t erc = {0}, orc = {0};
  rect_t src = {0}, drc = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_src_flinger != NULL);
  MT_ASSERT(p_dst_flinger != NULL);
  
  p_mflinger_src = (flinger_t *)p_src_flinger;
  p_mflinger_dst = (flinger_t *)p_dst_flinger;

  MT_ASSERT(p_mflinger_src->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->p_surf != NULL);
  MT_ASSERT(p_mflinger_src->surf_cnt == 1);
  MT_ASSERT(p_mflinger_dst->surf_cnt == 1);

  p_src_surf = (void *)(*(p_mflinger_src->p_surf));
  ret = surface_get_srcrect(p_src_surf, &src_surf_rect);
  MT_ASSERT(ret == SUCCESS);

  offset_rect(&src_surf_rect,
    0 - p_mflinger_src->flinger_rect.left, 0 - p_mflinger_src->flinger_rect.top);

  src.left = sx;
  src.top = sy;
  src.right = sx + sw;
  src.bottom = sy + sh;

  if(intersect_rect(&erc, &src, &src_surf_rect))
  {
    p_dst_surf = (void *)(*(p_mflinger_dst->p_surf));

    ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
    MT_ASSERT(ret == SUCCESS);

    offset_rect(&dst_surf_rect,
      0 - p_mflinger_dst->flinger_rect.left, 0 - p_mflinger_dst->flinger_rect.top);

    drc.left = dx;
    drc.top = dy;
    drc.right = dx + sw;
    drc.bottom = dy + sh;
    
    if(intersect_rect(&orc, &drc, &dst_surf_rect))
    {
      ret = surface_rotate_mirror(p_src_surf, &erc,
        p_dst_surf, orc.left, orc.top, rotate_mod, mirror_mod);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  
  return;
}

void flinger_blt_3src(void *p_src_flinger, rect_t *p_src,
  void *p_dst_flinger, rect_t *p_dst,
  void *p_ref_flinger, rect_t *p_ref, cct_gpe_blt3opt_t *p_opt3)
{
  flinger_t *p_mflinger_src = NULL, *p_mflinger_dst = NULL, *p_mflinger_ref = NULL;
  void *p_src_surf = NULL, *p_dst_surf = NULL, *p_ref_surf = NULL;
  rect_t src_surf_rect = {0}, dst_surf_rect = {0}, ref_surf_rect = {0};
  rect_t erc = {0}, orc = {0}, rrc = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_src_flinger != NULL);
  MT_ASSERT(p_dst_flinger != NULL);
  
  p_mflinger_src = (flinger_t *)p_src_flinger;
  p_mflinger_dst = (flinger_t *)p_dst_flinger;
  p_mflinger_ref = (flinger_t *)p_ref_flinger;
  
  MT_ASSERT(p_mflinger_src->p_surf != NULL);
  MT_ASSERT(p_mflinger_dst->p_surf != NULL);
  MT_ASSERT(p_mflinger_ref->p_surf != NULL);
  MT_ASSERT(p_mflinger_src->surf_cnt == 1);
  MT_ASSERT(p_mflinger_dst->surf_cnt == 1);
  MT_ASSERT(p_mflinger_ref->surf_cnt == 1);

  p_src_surf = (void *)(*(p_mflinger_src->p_surf));
  ret = surface_get_srcrect(p_src_surf, &src_surf_rect);
  MT_ASSERT(ret == SUCCESS);

  offset_rect(&src_surf_rect,
    0 - p_mflinger_src->flinger_rect.left, 0 - p_mflinger_src->flinger_rect.top);

  if(intersect_rect(&erc, p_src, &src_surf_rect))
  {
    p_dst_surf = (void *)(*(p_mflinger_dst->p_surf));

    ret = surface_get_srcrect(p_dst_surf, &dst_surf_rect);
    MT_ASSERT(ret == SUCCESS);

    offset_rect(&dst_surf_rect,
      0 - p_mflinger_dst->flinger_rect.left, 0 - p_mflinger_dst->flinger_rect.top);

    if(intersect_rect(&orc, p_dst, &dst_surf_rect))
    {
      p_ref_surf = (void *)(*(p_mflinger_ref->p_surf));

      ret = surface_get_srcrect(p_ref_surf, &ref_surf_rect);
      MT_ASSERT(ret == SUCCESS);

      offset_rect(&ref_surf_rect,
        0 - p_mflinger_ref->flinger_rect.left, 0 - p_mflinger_ref->flinger_rect.top);
      
      if(intersect_rect(&rrc, p_ref, &ref_surf_rect))
      {
        ret = surface_blt_3src(p_src_surf, &erc,
          p_dst_surf, &orc, p_ref_surf, &rrc, p_opt3);
        MT_ASSERT(ret == SUCCESS);
      }
    }
  } 
  
  return;
}

void flinger_draw_pie(void *p_flinger,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u32 color)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  
  MT_ASSERT(p_flinger != NULL);
  
  p_mflinger = (flinger_t *)p_flinger;
  
  MT_ASSERT(p_mflinger->p_surf != NULL);
  MT_ASSERT(p_mflinger->surf_cnt == 1);

  p_surf = (void *)(*(p_mflinger->p_surf));

  surface_draw_pie(p_surf, cent, radius, from_degree, to_degree, color);
  
  return;
}

void flinger_draw_arc(void *p_flinger,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u16 thick, u32 color)
{
  flinger_t *p_mflinger = NULL;
  void *p_surf = NULL;
  
  MT_ASSERT(p_flinger != NULL);
  
  p_mflinger = (flinger_t *)p_flinger;
  
  MT_ASSERT(p_mflinger->p_surf != NULL);
  MT_ASSERT(p_mflinger->surf_cnt == 1);

  p_surf = (void *)(*(p_mflinger->p_surf));

  surface_draw_arc(p_surf, cent, radius, from_degree, to_degree, thick, color);
  
  return;
}


