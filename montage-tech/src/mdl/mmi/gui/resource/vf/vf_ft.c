/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
 \file gui_vfont.c
   this file  implement the functions defined in  gui_vfont.h, also it implement some internal used
   function. All these functions are about how to decribe, set and draw a pbar control.
 */
#include "sys_types.h"
#include "sys_define.h"
#include "string.h" 
#ifdef __LINUX__
#include "stdio.h"
#include "stdlib.h"
#endif
#include "assert.h"
#include "drv_dev.h"
#include "charsto.h"

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
#include "gpe_vsb.h"
#include "surface.h"

#include "flinger.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "gui_resource.h"

#include "class_factory.h"

#include "vf_ft.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_BITMAP_H

/*!
  cache paramter.
  */
typedef struct
{
  /*!
    alpha map
    */
  void *p_alpha;
  /*!
    char data
    */
  void *p_char;
  /*!
    char height
    */
  u16 char_height;
  /*!
    char width
    */
  u16 char_width;
  /*!
    x offset
    */
  s16 xoff;
  /*!
    y offset
    */
  s16 yoff;    
  /*!
    pitch
    */
  u16 alpha_pitch;
  /*!
    pitch
    */
  u16 pitch;    
  /*!
    char code
    */
  u16 char_code;
  /*!
    font id
    */
  u16 font_id;
  /*!
    face id
    */
  u8 face_id;
  /*!
    step width
    */
  u16 step_width;
  /*!
    step height
    */
  u16 step_height;  
  /*!
    x step
    */
  u16 x_step;  
  /*!
    char height
    */
  u16 height;
  /*!
    char width
    */
  u16 width;  
  /*!
    color
    */
  u32 color;
  /*!
    attr
    */
  u32 attr;    
  /*!
    stork alpha
    */
  void *p_strok_alpha;
  /*!
    stork char
    */
  void *p_strok_char;    
}ft_cache_t;

/*!
   vfont priv data for free type.
  */
typedef struct
{
  /*!
    free type library
    */
  FT_Library p_library;
  /*!
    current face
    */
  FT_Face p_face;
  /*!
    current font
    */
  u16 curn_font_id;
  /*!
    current face id.
    */
  u8 curn_face_id;
  /*!
    cache
    */
  ft_cache_t *p_cache;
  /*!
    max height
    */
  u16 max_height;
  /*!
    max width
    */
  u16 max_width;
  /*!
    Cache buffer length
    */
  u8 cache_cnt;
  /*!
    bit per pixel
    */
  u8 bpp;    
  /*!
    mono
    */
  BOOL is_mono;   
  /*!
    is gpe alpha blending enable
    */
  BOOL is_alpha_spt;  
  /*!
    res handle.
    */
  handle_t rsc_handle;    
}ft_priv_t;

typedef struct
{
  /*!
    alpha map.
    */
  void *p_alpha;
  /*!
    pitch of alpha map.
    */
  u32 alpha_picth;
  /*!
    y max
    */
  s16 y_max;
  /*!
    y min
    */
  s16 y_min;  
  /*!
    x offset
    */
  u16 x_off;
  /*!
    y offset
    */
  u16 y_off;    
}spans_t;


static void mono_to_u32buf(u8 *p_mono, u16 width, u16 height, u32 mono_pitch, u8 *p_out,
  u32 out_pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u32 color[2];
  u32 i = 0;
  u32 j = 0;
  u32 *p_cursor = (u32 *)p_out;
  
  color[0] = (u32)back_color;
  color[1] = (u32)front_color;

  for(i = 0; i < height; i++)
  {
    p_cursor = (u32 *)(p_out + i * out_pitch);
    
    for(j = 0; j < width; j++)
    {
      pixel = ((p_mono[i * mono_pitch + j / 8] << (j % 8)) & 0x80) >> 7;

      *(p_cursor++) = color[pixel];
    }    
  }
  
  MT_ASSERT((u32)p_out % 8 == 0);
  MT_ASSERT(out_pitch % 8 == 0);
  
  return;
}

static u32 mono_to_u16buf(u8 *p_mono, u16 width, u16 height, u32 mono_pitch, u8 *p_out,
  u32 out_pitch, u16 front_color, u16 back_color)
{
  u8 pixel = 0;
  u16 color[2];
  u32 i = 0, j = 0;
  u16 *p_cursor = (u16 *)p_out;

  color[0] = (u16)back_color;
  color[1] = (u16)front_color;

  for(i = 0; i < height; i++)
  {
    p_cursor = (u16 *)(p_out + i * out_pitch);

    for(j = 0; j < width; j++)
    {
      pixel = ((p_mono[i * mono_pitch + j / 8] << (j % 8)) & 0x80) >> 7;
     
      *(p_cursor++) = color[pixel];
    }    
  }
  
  MT_ASSERT((u32)p_out % 8 == 0);
  MT_ASSERT(out_pitch % 8 == 0);
  
  return (u32)(out_pitch * height);
}

static void raster_cb(const int y, const int count,
  const FT_Span * const p_spans, void * const p_user) 
{
  u32 i = 0, j = 0;
  spans_t *p_spft = (spans_t *)p_user;
  s32 offset = 0;
  
  for (i = 0; i < count; ++i) 
  {
    for(j = 0; j < p_spans[i].len; j++)
    {
      offset = 
        ((p_spft->y_max - p_spft->y_min - (y - p_spft->y_min) - 1 + p_spft->y_off)
        * p_spft->alpha_picth + p_spans[i].x + j + p_spft->x_off);
      
      if(offset >= 0)
      {
        *((u8 *)p_spft->p_alpha + offset) = p_spans[i].coverage;
      }
    }
  }
}


static void ft_get_face(void *p_priv, u16 font_id, u8 face_id, void *p_font, u32 size)
{
  RET_CODE ret = SUCCESS;
  ft_priv_t *p_vf = (ft_priv_t *)p_priv;

  MT_ASSERT(p_vf != NULL);

  if((font_id == p_vf->curn_font_id) && (face_id == p_vf->curn_face_id))
  {
    return;
  }

  if(p_vf->p_face != NULL)
  {
    //try to release previous face.
    FT_Done_Face(p_vf->p_face);
  }
  
  ret = FT_New_Memory_Face(p_vf->p_library, p_font, size, face_id, &p_vf->p_face);
  MT_ASSERT(ret == SUCCESS);

  p_vf->curn_face_id = face_id;
  p_vf->curn_font_id = font_id;
}

static BOOL ft_check_char(void *p_priv, 
  u16 char_code, void *p_font, u32 size, rsc_fstyle_t *p_fstyle)
{
  u16 glyph_index = 0;
  ft_priv_t *p_vf = (ft_priv_t *)p_priv;

  MT_ASSERT(p_vf != NULL);

  ft_get_face(p_priv, p_fstyle->font_id, p_fstyle->face_id, p_font, size);

  glyph_index = FT_Get_Char_Index(p_vf->p_face, char_code);

  if(glyph_index)
  {
    return TRUE;
  }

  return FALSE;
}

static BOOL ft_hit_cache(void *p_priv, u16 char_code,
  rsc_fstyle_t *p_style, rsc_char_info_t *p_info)
{
  u32 i = 0,j = 0;
  ft_cache_t *p_cache = NULL;
  ft_priv_t *p_vf = (ft_priv_t *)p_priv;
  
  MT_ASSERT(p_vf != NULL);
  MT_ASSERT(p_info != NULL);
  MT_ASSERT(p_style != NULL);

  for(i = 0; i < p_vf->cache_cnt; i++)
  {
    p_cache = p_vf->p_cache + i;

    if((p_cache->char_code == char_code) &&
      (p_cache->face_id == p_style->face_id) &&
      (p_cache->char_height == p_style->height) &&
      (p_cache->char_width == p_style->width) &&
      (p_cache->font_id == p_style->font_id) &&
      (p_cache->attr == p_style->attr))
    {
      p_info->width = p_cache->width;
      p_info->height = p_cache->height;
      p_info->pitch = p_cache->pitch;
      p_info->alpha_pitch = p_cache->alpha_pitch;
      p_info->xoffset = p_cache->xoff;
      p_info->yoffset = p_cache->yoff;

      p_info->p_char = p_cache->p_char;
      p_info->p_alpha = p_cache->p_alpha;

      if(p_style->attr & VFONT_STROK)
      {
        p_info->p_strok_alpha = p_cache->p_strok_alpha;
        p_info->p_strok_char = p_cache->p_strok_char;
      }
      else
      {
        p_info->p_strok_alpha = NULL;
        p_info->p_strok_char = NULL;

      }
      p_info->step_height = p_cache->step_height;
      p_info->step_width = p_cache->step_width;
      p_info->is_alpha_spt = p_vf->is_alpha_spt;
      p_info->x_step = p_cache->x_step;

      if(p_vf->is_alpha_spt)
      {
        if(p_cache->color != p_style->color)
        {
          for(j = 0; j < (p_vf->max_height * p_vf->max_width); j++)
          {
            if(p_vf->bpp == 32)
            {
              if((p_style->attr & VFONT_STROK) != 0)
              {
                *((u32 *)(p_cache->p_char) + j) = 
                  (0xFFFFFF - (p_style->color & 0xFFFFFF)) | 0xFF000000;
                *((u32 *)(p_cache->p_strok_char) + j) = p_style->color;
              }
              else
              {
                *((u32 *)(p_cache->p_char) + j) = p_style->color;
              }
            }
            else if(p_vf->bpp == 16)
            {
              if((p_style->attr & VFONT_STROK) != 0)
              {
                *((u16 *)(p_cache->p_char) + j) = 
                  (0x7FFF - (p_style->color & 0x7FFF)) | 0x8000;              
                *((u16 *)(p_cache->p_strok_char) + j) = p_style->color;
              }
              else
              {
                *((u16 *)(p_cache->p_char) + j) = p_style->color;
              }
            }
          } 

          p_cache->color = p_style->color;
        }

        return TRUE;
      }
      else
      {
        if(p_cache->color == p_style->color)
        {
          return TRUE;
        }
      }

    }
  }

  return FALSE;
}

static ft_cache_t *ft_insert_cache(void *p_priv, u16 char_code, rsc_fstyle_t *p_style)
{
  s32 i = 0;
  void *p_char = NULL, *p_alpha = NULL, *p_strok_alpha = NULL, *p_strok_char = NULL;
  ft_priv_t *p_vf = (ft_priv_t *)p_priv;

  MT_ASSERT(p_vf != NULL);
  
  p_char = p_vf->p_cache[p_vf->cache_cnt - 1].p_char;
  p_alpha = p_vf->p_cache[p_vf->cache_cnt - 1].p_alpha;
  p_strok_char = p_vf->p_cache[p_vf->cache_cnt - 1].p_strok_char;
  p_strok_alpha = p_vf->p_cache[p_vf->cache_cnt - 1].p_strok_alpha;
  
  for(i = (p_vf->cache_cnt - 1); i > 0; i--)
  {
    p_vf->p_cache[i] = p_vf->p_cache[i - 1];
  }

  p_vf->p_cache[0].p_char = p_char;
  p_vf->p_cache[0].p_alpha = p_alpha;
  p_vf->p_cache[0].char_code = char_code;
  p_vf->p_cache[0].face_id = p_style->face_id;
  p_vf->p_cache[0].font_id = p_style->font_id;
  p_vf->p_cache[0].char_width = p_style->width;
  p_vf->p_cache[0].char_height = p_style->height;
  p_vf->p_cache[0].p_strok_alpha = p_strok_alpha;
  p_vf->p_cache[0].p_strok_char = p_strok_char;
  p_vf->p_cache[0].attr = p_style->attr;

  return &p_vf->p_cache[0];
}

static BOOL ft_get_char(void *p_priv,
  u16 char_code, rsc_fstyle_t *p_fstyle, rsc_char_info_t *p_info)
{
  RET_CODE ret = SUCCESS;
  ft_cache_t *p_vf_cache = NULL;
  u32 i = 0, ckey = 0;
  ft_priv_t *p_vf = (ft_priv_t *)p_priv;
  FT_Stroker p_stroker = {0};
  FT_Raster_Params params = {0};  
  FT_Glyph p_glyph = {0};
  spans_t spans = {0};
  FT_BBox bbox_fg = {0}, bbox_bg = {0};
  u16 w_bg = 0, w_fg = 0, h_bg = 0, h_fg = 0;
  FT_Matrix matrix = {0};
  rsc_font_t hdr_font = {{0}};
  u8 *p_font = NULL;

  MT_ASSERT(p_vf != NULL);

  if(ft_hit_cache(p_priv, char_code, p_fstyle, p_info))
  {
    p_info->ckey = p_fstyle->color ? 0 : 1;
    p_info->bpp = p_vf->bpp;
    
    return TRUE;
  }

  MT_ASSERT(p_fstyle->width <= p_vf->max_width);
  MT_ASSERT(p_fstyle->height <= p_vf->max_height);
  
  if(!rsc_get_font(p_vf->rsc_handle, p_fstyle->font_id, &hdr_font, &p_font))
  {
    return FALSE;
  }

  MT_ASSERT(hdr_font.type == FONT_VECTOR);

  if(!ft_check_char(p_priv, char_code, p_font, hdr_font.head.org_size, p_fstyle))
  {
    return FALSE;
  }

  p_vf_cache = ft_insert_cache(p_priv, char_code, p_fstyle);

  ckey = p_fstyle->color ? 0 : 1;

  MT_ASSERT(p_vf_cache != NULL);

  
  if((p_fstyle->attr & VFONT_ITALIC) != 0)
  {
    matrix.xx = 0x10000L;
    matrix.xy = 0x1000L;
    matrix.yx = 0;
    matrix.yy = 0x10000L;
    FT_Set_Transform(p_vf->p_face, &matrix, 0);  
  }
  else
  {
    FT_Set_Transform(p_vf->p_face, NULL, 0);  
  }

  ret = FT_Set_Pixel_Sizes(p_vf->p_face, p_vf_cache->char_width, p_vf_cache->char_height);
  MT_ASSERT(ret == SUCCESS);

  if(((p_fstyle->attr & VFONT_STROK) == 0) || (p_vf->is_alpha_spt == FALSE))
  {
    if(p_vf->is_alpha_spt)
    {
      ret = FT_Load_Char(p_vf->p_face, char_code, FT_LOAD_RENDER);
      p_info->is_alpha_spt = TRUE;    
    }
    else
    {
      ret = FT_Load_Char(p_vf->p_face, char_code, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
      p_info->is_alpha_spt = FALSE;
    }

    if((p_fstyle->attr & VFONT_BOLD) != 0)
    {
      FT_Bitmap_Embolden(p_vf->p_library, &p_vf->p_face->glyph->bitmap, 64, 64);    
    }
    
    MT_ASSERT(ret == SUCCESS);

    p_info->height = p_vf->p_face->glyph->bitmap.rows;
    p_info->width = p_vf->p_face->glyph->bitmap.width;
    p_info->xoffset = p_vf->p_face->glyph->bitmap_left;
    p_info->yoffset = ((p_vf->p_face->size->metrics.ascender
     - p_vf->p_face->glyph->metrics.horiBearingY) >> 6);
    p_info->pitch = p_vf->max_width * p_vf->bpp >> 3;
    p_info->alpha_pitch = p_vf->max_width;
    p_info->x_step = ((p_vf->p_face->glyph->advance.x) >> 6);
    p_info->step_width = p_vf->p_face->glyph->metrics.horiAdvance >> 6;
    //p_info->step_height = p_vf->p_face->size->metrics.y_ppem;
    p_info->step_height = p_vf->p_face->size->metrics.height >> 6;

    p_vf_cache->pitch = p_info->pitch;
    p_vf_cache->alpha_pitch = p_info->alpha_pitch;
    p_vf_cache->xoff = p_info->xoffset;
    p_vf_cache->yoff = p_info->yoffset;
    p_vf_cache->step_width = p_info->step_width;
    p_vf_cache->step_height = p_info->step_height;
    p_vf_cache->x_step = p_info->x_step;
    p_vf_cache->width = p_info->width;
    p_vf_cache->height = p_info->height;  
    p_vf_cache->color = p_fstyle->color;
    
    if(p_vf->is_alpha_spt)
    {
      for(i = 0; i < p_vf->p_face->glyph->bitmap.rows; i++)
      {
        memcpy((u8 *)p_vf_cache->p_alpha + i * p_vf->max_width,
          p_vf->p_face->glyph->bitmap.buffer + i * p_vf->p_face->glyph->bitmap.pitch,
          p_vf->p_face->glyph->bitmap.pitch);
      }

      for(i = 0; i < (p_vf->max_height * p_vf->max_width); i++)
      {
        if(p_vf->bpp == 32)
        {
          *((u32 *)(p_vf_cache->p_char) + i) = p_fstyle->color;
        }
        else if(p_vf->bpp == 16)
        {
          *((u16 *)(p_vf_cache->p_char) + i) = p_fstyle->color;
        }
        
      }    
    }
    else
    {
      if(p_vf->bpp == 32)
      {
        mono_to_u32buf(p_vf->p_face->glyph->bitmap.buffer,
          p_vf->p_face->glyph->bitmap.width, p_vf->p_face->glyph->bitmap.rows,
          p_vf->p_face->glyph->bitmap.pitch,
          p_vf_cache->p_char, p_vf->max_width * 4, p_fstyle->color, ckey);
      }
      else if(p_vf->bpp == 16)
      {
        mono_to_u16buf(p_vf->p_face->glyph->bitmap.buffer,
          p_vf->p_face->glyph->bitmap.width, p_vf->p_face->glyph->bitmap.rows,
          p_vf->p_face->glyph->bitmap.pitch,
          p_vf_cache->p_char, p_vf->max_width * 2, p_fstyle->color, ckey);
      }
    }

    p_info->p_alpha = p_vf_cache->p_alpha;
    p_info->p_char = p_vf_cache->p_char;
    p_info->p_strok_alpha = NULL;
    p_info->p_strok_char = NULL;
  }
  else if((p_fstyle->attr & VFONT_STROK) != 0)  
  {
    memset(p_vf_cache->p_strok_alpha, 0, p_vf->max_height * p_vf->max_width);
    memset(p_vf_cache->p_strok_char, 0, ((p_vf->max_height * p_vf->max_width * p_vf->bpp) >> 3));

    //get basic glyph.
    if(0 == FT_Load_Glyph(p_vf->p_face,
      FT_Get_Char_Index(p_vf->p_face, char_code), FT_LOAD_NO_BITMAP))
    {
      if((p_fstyle->attr & VFONT_BOLD) != 0)
      {
        FT_Outline_Embolden(&p_vf->p_face->glyph->outline, 64);      
      }
      
      //get fg bbox.
      FT_Get_Glyph(p_vf->p_face->glyph, &p_glyph);

      FT_Glyph_Get_CBox(p_glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox_fg);

      //get bg bbox.
      FT_Stroker_New(p_vf->p_library, &p_stroker);

      FT_Stroker_Set(p_stroker, 64 * 2, FT_STROKER_LINECAP_ROUND,  FT_STROKER_LINEJOIN_ROUND, 0);

      FT_Glyph_StrokeBorder(&p_glyph, p_stroker, FALSE, TRUE);

      FT_Glyph_Get_CBox(p_glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox_bg);   

      h_bg = (bbox_bg.yMax - bbox_bg.yMin) >> 6;
      w_bg = (bbox_bg.xMax - bbox_bg.xMin) >> 6;

      h_fg = (bbox_fg.yMax - bbox_fg.yMin) >> 6;
      w_fg = (bbox_fg.xMax - bbox_fg.xMin) >> 6;

      MT_ASSERT(h_bg <= p_vf->max_height);
      MT_ASSERT(w_bg <= p_vf->max_width);
      
      if (p_vf->p_face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)      
      {
        spans.p_alpha = p_vf_cache->p_strok_alpha;
        spans.alpha_picth = p_vf->max_width;    
        spans.y_min = bbox_fg.yMin >> 6;
        spans.y_max = bbox_fg.yMax >> 6;
        spans.x_off = 0;
        spans.y_off = (h_bg - h_fg) >> 1;
        
        memset(&params, 0, sizeof(params));
        params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
        params.gray_spans = raster_cb;
        params.user = &spans;

        FT_Outline_Render(p_vf->p_library, &p_vf->p_face->glyph->outline, &params);
      }

      MT_ASSERT(p_vf->is_alpha_spt == TRUE);
      
      for(i = 0; i < (p_vf->max_height * p_vf->max_width); i++)
      {
        if(p_vf->bpp == 32)
        {
          *((u32 *)(p_vf_cache->p_strok_char) + i) = p_fstyle->color;
        }
        else if(p_vf->bpp == 16)
        {
          *((u16 *)(p_vf_cache->p_strok_char) + i) = p_fstyle->color;
        }      
      }    

      p_info->p_strok_alpha = p_vf_cache->p_strok_alpha;
      p_info->p_strok_char = p_vf_cache->p_strok_char;    

      memset(p_vf_cache->p_alpha, 0, p_vf->max_height * p_vf->max_width);
      memset(p_vf_cache->p_char, 0, ((p_vf->max_height * p_vf->max_width * p_vf->bpp) >> 3));
      
      //get stork glyph.
      if (p_glyph->format == FT_GLYPH_FORMAT_OUTLINE)
      {
        spans.p_alpha = p_vf_cache->p_alpha;
        spans.alpha_picth = p_vf->max_width;    
        spans.y_min = bbox_bg.yMin >> 6;
        spans.y_max = bbox_bg.yMax >> 6;
        spans.x_off = 0;
        spans.y_off = 0;
      
        memset(&params, 0, sizeof(params));
        params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
        params.gray_spans = raster_cb;
        params.user = &spans;

        FT_Outline_Render(p_vf->p_library, &((FT_OutlineGlyphRec *)p_glyph)->outline, &params);
      }

      FT_Stroker_Done(p_stroker);
      FT_Done_Glyph(p_glyph);    

      p_info->height = h_bg;
      p_info->width = w_bg;
      p_info->xoffset = p_vf->p_face->glyph->metrics.horiBearingX >> 6;
      p_info->yoffset = ((p_vf->p_face->size->metrics.ascender
       - p_vf->p_face->glyph->metrics.horiBearingY) >> 6);
      p_info->alpha_pitch = p_vf->max_width;
      p_info->x_step = ((p_vf->p_face->glyph->advance.x) >> 6) + ((w_bg - w_fg) >> 1);
      p_info->step_width = (p_vf->p_face->glyph->metrics.horiAdvance >> 6) + ((w_bg - w_fg) >> 1);
      //p_info->step_height = p_vf->p_face->size->metrics.y_ppem + ((h_bg - h_fg) >> 1);
      p_info->step_height = (p_vf->p_face->size->metrics.height >> 6) + ((h_bg - h_fg) >> 1);
      p_info->is_alpha_spt = p_vf->is_alpha_spt;
      
      p_vf_cache->alpha_pitch = p_info->alpha_pitch;
      p_vf_cache->xoff = p_info->xoffset;
      p_vf_cache->yoff = p_info->yoffset;
      p_vf_cache->step_width = p_info->step_width;
      p_vf_cache->step_height = p_info->step_height;
      p_vf_cache->x_step = p_info->x_step;
      p_vf_cache->width = p_info->width;
      p_vf_cache->height = p_info->height;  
      p_vf_cache->color = p_fstyle->color;

      for(i = 0; i < (p_vf->max_height * p_vf->max_width); i++)
      {
        if(p_vf->bpp == 32)
        {
          *((u32 *)(p_vf_cache->p_char) + i) = 
            (0xFFFFFF - (p_fstyle->color & 0xFFFFFF)) | 0xFF000000;
        }
        else if(p_vf->bpp == 16)
        {
          *((u16 *)(p_vf_cache->p_char) + i) = 
            (0x7FFF - (p_fstyle->color & 0x7FFF)) | 0x8000;
        }      
      }    

      p_info->p_alpha = p_vf_cache->p_alpha;
      p_info->p_char = p_vf_cache->p_char;        
    }
  }


  p_info->ckey = ckey;
  p_info->bpp = p_vf->bpp;
  
  return TRUE;
}

static BOOL ft_get_attr(void *p_priv,
  u16 char_code, rsc_fstyle_t *p_fstyle, u16 *p_width, u16 *p_height)
{
  ft_priv_t *p_vf = (ft_priv_t *)p_priv;
  rsc_char_info_t char_info = {0};
  
  MT_ASSERT(p_vf != NULL);  
  MT_ASSERT(p_fstyle != NULL);
  MT_ASSERT(p_width != NULL);
  MT_ASSERT(p_height != NULL);

  if(!ft_get_char(p_priv, char_code, p_fstyle, &char_info))
  {
    *p_width = 0;
    *p_height = 0;
    
    return FALSE;
  }

  *p_width = char_info.step_width;
  *p_height = char_info.step_height;
  
  return TRUE;
}

void vf_ft_attach(handle_t rsc_handle, vf_ft_cfg_t *p_cfg)
{
  RET_CODE ret = SUCCESS;
  rsc_main_t *p_rsc_info = NULL;
  ft_priv_t *p_priv = NULL;
  u32 i = 0;
  #ifndef WIN32
  chip_ic_t chip_ic = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic = IC_MAGIC;
  #endif

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  p_priv = (ft_priv_t *)mmi_alloc_buf(sizeof(ft_priv_t));
  MT_ASSERT(p_priv != NULL);

  memset(p_priv, 0, sizeof(ft_priv_t));
  
  p_rsc_info->p_priv = p_priv;

  MT_ASSERT(p_cfg != NULL);
  MT_ASSERT(p_cfg->max_cnt != 0);
  
  //align with 8 byte, temp solution.
  MT_ASSERT(p_cfg->max_width % 8 == 0);

  MT_ASSERT(p_cfg->bpp % 8 == 0);
  MT_ASSERT(p_cfg->bpp >= 16);

  p_priv->bpp = p_cfg->bpp;

  if(p_cfg->max_cnt != 0 && p_cfg->max_height * p_cfg->max_width != 0)
  {
    p_priv->p_cache = mtos_malloc(p_cfg->max_cnt * sizeof(ft_cache_t));
    MT_ASSERT(p_priv->p_cache != 0);
    memset(p_priv->p_cache, 0, p_cfg->max_cnt * sizeof(ft_cache_t));
    
    for(i = 0; i < p_cfg->max_cnt; i++)
    {
      switch(chip_ic)
      {
        case IC_SONATA:
        case IC_CONCERTO:
          p_priv->p_cache[i].p_alpha =
            mtos_dma_malloc_alias(p_cfg->max_height * p_cfg->max_width);
          p_priv->p_cache[i].p_char =
            mtos_dma_malloc_alias(p_cfg->max_height * p_cfg->max_width * (p_priv->bpp >> 3));
          p_priv->p_cache[i].p_strok_alpha =
            mtos_dma_malloc_alias(p_cfg->max_height * p_cfg->max_width);
          p_priv->p_cache[i].p_strok_char =
            mtos_dma_malloc_alias(p_cfg->max_height * p_cfg->max_width * (p_priv->bpp >> 3));        
          break;

        default:
          p_priv->p_cache[i].p_alpha =
            mtos_align_malloc(p_cfg->max_height * p_cfg->max_width, 8);
          p_priv->p_cache[i].p_char =
            mtos_align_malloc(p_cfg->max_height * p_cfg->max_width * (p_priv->bpp >> 3), 8);
          p_priv->p_cache[i].p_strok_alpha =
            mtos_align_malloc(p_cfg->max_height * p_cfg->max_width, 8);
          p_priv->p_cache[i].p_strok_char =
            mtos_align_malloc(p_cfg->max_height * p_cfg->max_width * (p_priv->bpp >> 3), 8);          
          break;
      }
      
      MT_ASSERT(p_priv->p_cache[i].p_alpha != NULL);
      MT_ASSERT(p_priv->p_cache[i].p_char != NULL);
      MT_ASSERT(p_priv->p_cache[i].p_strok_alpha != NULL);
      MT_ASSERT(p_priv->p_cache[i].p_strok_char != NULL);      
    }
  }

  p_priv->cache_cnt = p_cfg->max_cnt;
  p_priv->max_width = p_cfg->max_width;
  p_priv->max_height = p_cfg->max_height;
  p_priv->is_alpha_spt = p_cfg->is_alpha_spt;
  p_priv->rsc_handle = rsc_handle;

  ret = FT_Init_FreeType(&p_priv->p_library);

  //attach function.
  p_rsc_info->get_attr = ft_get_attr;
  p_rsc_info->get_char = ft_get_char;

  MT_ASSERT(ret == SUCCESS);
}

void vf_ft_detach(handle_t res_handle)
{
  rsc_main_t *p_res_info = NULL;
  ft_priv_t *p_priv = NULL;
  u32 i = 0;
  #ifndef WIN32
  chip_ic_t chip_ic = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic = IC_MAGIC;
  #endif

  p_res_info = (rsc_main_t *)res_handle;
  MT_ASSERT(p_res_info != NULL);

  p_res_info->get_attr = NULL;
  p_res_info->get_char = NULL;

  p_priv = (ft_priv_t *)p_res_info->p_priv;

  if(p_priv != NULL)
  {
    if(p_priv->p_cache != NULL)
    {
      for(i = 0; i < p_priv->cache_cnt; i++)
      {
        switch(chip_ic)
        {
          case IC_SONATA:
          case IC_CONCERTO:
            if(p_priv->p_cache[i].p_alpha != NULL)
            {
              mtos_dma_free_alias(p_priv->p_cache[i].p_alpha);

              p_priv->p_cache[i].p_alpha = NULL;            
            }

            if(p_priv->p_cache[i].p_char != NULL)
            {
              mtos_dma_free_alias(p_priv->p_cache[i].p_char);

              p_priv->p_cache[i].p_char = NULL;
            }

            if(p_priv->p_cache[i].p_strok_alpha != NULL)
            {
              mtos_dma_free_alias(p_priv->p_cache[i].p_strok_alpha);

              p_priv->p_cache[i].p_strok_alpha = NULL;
            }

            if(p_priv->p_cache[i].p_strok_char != NULL)
            {
              mtos_dma_free_alias(p_priv->p_cache[i].p_strok_char);

              p_priv->p_cache[i].p_strok_char = NULL;
            }     
            break;

          default:
            if(p_priv->p_cache[i].p_alpha != NULL)
            {
              mtos_align_free(p_priv->p_cache[i].p_alpha);

              p_priv->p_cache[i].p_alpha = NULL;            
            }

            if(p_priv->p_cache[i].p_char != NULL)
            {
              mtos_align_free(p_priv->p_cache[i].p_char);

              p_priv->p_cache[i].p_char = NULL;
            }

            if(p_priv->p_cache[i].p_strok_alpha != NULL)
            {
              mtos_align_free(p_priv->p_cache[i].p_strok_alpha);

              p_priv->p_cache[i].p_strok_alpha = NULL;
            }

            if(p_priv->p_cache[i].p_strok_char != NULL)
            {
              mtos_align_free(p_priv->p_cache[i].p_strok_char);

              p_priv->p_cache[i].p_strok_char = NULL;
            }        
            break;
        }      
      }

      mtos_free(p_priv->p_cache);

      p_priv->p_cache = NULL;
    }

    if(p_priv->p_face != NULL)
    {
      FT_Done_Face(p_priv->p_face);
      p_priv->p_face = NULL;
    }

    if(p_priv->p_library != NULL)
    {
      FT_Done_FreeType(p_priv->p_library);
      p_priv->p_library = NULL;
    }
    
    mmi_free_buf((void *)p_priv);

    p_priv = NULL;
  }
}
    

