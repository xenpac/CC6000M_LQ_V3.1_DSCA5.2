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

#include "af.h"
/*!
  af cache.
  */
typedef struct
{
  /*!
     The character code of the data
    */
  u16 char_code;
  /*!
     The font color of the character
    */
  rsc_fstyle_t fstyle;
  /*!
     The width of the character stored in the cache
    */
  u16 width;
  /*!
     The height  of the character stored in the cache
    */
  u16 height;
  /*!
    x step
    */
  u16 x_step;    
  /*!
    pitch
    */
  u16 pitch;    
  /*!
     A pointer to the cache buffer, which storsc the character data.
    */
  void *p_char;
}af_cache_t;

/*!
   array font priv data for free type.
  */
typedef struct
{
  /*!
    rsc handle
    */
  handle_t rsc_handle;  
  /*!
    One cache buffer size
    */
  u32 cache_size;
  /*!
    Cache buffer length
    */
  u8 cache_cnt;
  /*!
    A pointer point to cache, to record the information of the data in cache
    */
  af_cache_t *p_cache;
  /*!
    Cache buffer addrscs
    */
  u8 *p_cache_addr;  
  /*!
    bpp
    */
  u8 bpp;    
}af_priv_t;

#define SHIFT1(x)    (7 - (x & 7))
static const u8 pix1mask [] = { 0x80, 0x40, 0x20, 0x10, 
                                0x08, 0x04, 0x02, 0x01 };

static inline u8 af_get_1bit_in_byte(u32 x, const u8 *p_byte)
{
  u8 tmp = *p_byte;
  tmp = tmp & pix1mask[x & 7];
  return(tmp >> SHIFT1(x));
}

#define SHIFT2(x)    (6 - (x & 6))
static const u8 pix2mask [] = { 0xC0, 0x00, 0x30, 0x00, 
                                0x0C, 0x00, 0x03, 0x00};
static inline u8 af_get_2bit_in_byte(u32 x, const u8 *p_byte)
{
  u8 tmp = *p_byte;
  tmp = tmp & pix2mask[x & 6];
  return (tmp >> SHIFT2(x));
}

static void af_init_pixel4_model(u32 fc, u32 bc, u8 *p_model)
{
  p_model[0] = (bc << 4) | bc; /*00*/
  p_model[1] = (bc << 4) | fc; /*01*/
  p_model[2] = (fc << 4) | bc; /*10*/
  p_model[3] = (fc << 4) | fc; /*11*/
}

static u32 af_mono_to_u4buf(u8 *p_data, u16 width, u16 height, 
  u8 *p_buf, u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u32 s = 0, i = 0;
  u8 *p_cursor = NULL, *p_line = NULL;
  u16 aligned_width = 0; /* byte align */
  u8 pix4model[4];

  af_init_pixel4_model(front_color, back_color, pix4model);

  p_cursor = p_line = (u8 *)p_buf;
  s = 0;

  if(width & 1)
  {
    aligned_width = 1;
    width -= aligned_width;
  }

  while(0 != height)
  {
    (height--);
    
    i = 0;
    while(i < width)
    {

      if(s & 1)
      {
        pixel = af_get_1bit_in_byte(s, p_data + (s >> 3));
        s++;
        pixel = (pixel << 1) | af_get_1bit_in_byte(s, p_data + (s >> 3));
        s++;
      }
      else
      {
      pixel = af_get_2bit_in_byte(s, p_data + (s >> 3));
      s += 2;
      }

      *p_cursor++ = pix4model[pixel];
      i += 2;
    }

    if(aligned_width > 0)
    {
      pixel = af_get_1bit_in_byte(s, p_data + (s >> 3)) << 1;
      *p_cursor = pix4model[pixel];
      s += aligned_width;
    }

    p_line += pitch;
    p_cursor = p_line;
  }

  return (u32)(p_cursor - p_buf);
}

static u32 af_mono_to_u8buf(u8 *p_data, u16 width, u16 height, 
  u8 *p_buf, u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u8 color[2];
  u32 i = 0;
  u32 j = 0;
  u8 *p_cursor = (u8 *)p_buf;
  
  color[0] = (u8)back_color;
  color[1] = (u8)front_color;

  for(i = 0; i < height; i++)
  {
    p_cursor = p_buf + i * pitch;
    
    for(j = 0; j < width; j++)
    {
        pixel = af_get_1bit_in_byte((i * width + j), p_data + ((i * width + j) >> 3));

        *p_cursor++ = color[pixel];
    }
    
  }

  if((p_cursor - p_buf) != (height - 1) * pitch + width)
  {
    MT_ASSERT(0);
  }

  return (u32)(height * pitch);
}

static u32 af_mono_to_u16buf(u8 *p_data, u16 width, u16 height, u8 *p_buf,
  u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u16 color[2];
  u32 i = 0, j = 0;
  u16 *p_cursor = (u16 *)p_buf;

  color[0] = (u16)back_color;
  color[1] = (u16)front_color;

  for(i = 0; i < height; i++)
  {
    p_cursor = (u16 *)(p_buf + i * pitch);

    for(j = 0; j < width; j++)
    {
      pixel = af_get_1bit_in_byte(i * width + j, p_data + ((i * width + j) >> 3));
     
      *(p_cursor++) = color[pixel];
    }    
  }
  
  MT_ASSERT((u32)p_buf % 8 == 0);
  MT_ASSERT(pitch % 8 == 0);
  
  return (u32)(pitch * height);
}

static u32 af_mono_to_u32buf(u8 *p_data, u16 width, u16 height, u8 *p_buf,
  u32 pitch, u32 front_color, u32 back_color)
{
  u8 pixel = 0;
  u32 color[2];
  u32 i = 0;
  u32 j = 0;
  u32 *p_cursor = (u32 *)p_buf;
  
  color[0] = (u32)back_color;
  color[1] = (u32)front_color;

  for(i = 0; i < height; i++)
  {
    p_cursor = (u32 *)(p_buf + i * pitch);

    for(j = 0; j < width; j++)
    {
      pixel = af_get_1bit_in_byte(i * width + j, p_data + ((i * width + j) >> 3));
     
      *(p_cursor++) = color[pixel];
    }    
  }
  
  MT_ASSERT((u32)p_buf % 8 == 0);
  MT_ASSERT(pitch % 8 == 0);
  
  return (u32)(pitch * height);
}

static u32 af_transform_char(void *p_priv, u8 *p_input, u16 width, 
  u16 height, u16 pitch, u8 *p_output, u32 font_color, u32 bpp)
{
  af_priv_t *p_af = (af_priv_t *)p_priv;
  u32 color_key = 0, size = 0;
  u8 *p_char_buf = NULL;

  color_key = (0 == font_color) ? 1 : 0;
  size = ((width * height)+ 7) >> 3;
  
  p_char_buf = (u8 *)rsc_get_comm_buf_addr(p_af->rsc_handle);
  MT_ASSERT(size <= rsc_get_comm_buf_size(p_af->rsc_handle));
  
  if(size > rsc_get_comm_buf_size(p_af->rsc_handle))
  {
    return 0;
  }
  
  rsc_read_data(p_af->rsc_handle, (u32)p_input, p_char_buf, size);

  switch(bpp)
  {
    case 4:
      af_mono_to_u4buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;

    case 8:
      af_mono_to_u8buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;

    case 16:
      af_mono_to_u16buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;

    case 32:
      af_mono_to_u32buf(p_char_buf, width, 
        height, p_output, pitch, font_color, color_key);
      break;

    default:
      MT_ASSERT(0);
  }

  return color_key;
}

u16 af_search(handle_t rsc_handle, u8 *p_tab, u16 count, u16 target)
{
  u16 value = 0, temp = 0;
  u16 start = 0, end = 0, mid = 0;
  BOOL is_in_buf = FALSE;
  u16 buf_start_at = 0;
  u16 *p_buf_addr = (u16 *)rsc_get_comm_buf_addr(rsc_handle);
  u32 buf_size = rsc_get_comm_buf_size(rsc_handle) / sizeof(u16);

  temp = target;

  start = 0; end = count;
  mid = (start + end) / 2;

  if(count < buf_size)
  {
    rsc_read_data(rsc_handle, (u32)p_tab, p_buf_addr, count * sizeof(u16));
    p_tab = (u8 *)p_buf_addr;
    buf_start_at = 0;
    is_in_buf = TRUE;
  }

  rsc_read_data(rsc_handle, (u32)(p_tab + (mid - buf_start_at) * 2), &value, sizeof(u16));

  while(temp != value)
  {
    if(temp < value)
    {
      end = mid - 1;
    }
    else  if(temp > value)
    {
      start = mid + 1;
    }
    if(start > end)
    {
      return count;
    }

    /* try to load into buf */
    if(!is_in_buf)
    {
      if((end - start + 1) < buf_size)
      {
        rsc_read_data(rsc_handle, (u32)(p_tab + start * 2), p_buf_addr,
                      (end - start + 1) * sizeof(u16));
        p_tab = (u8 *)p_buf_addr;

        buf_start_at = start;
        is_in_buf = TRUE;
      }
    }

    mid = (start + end) / 2;
    rsc_read_data(rsc_handle, (u32)(p_tab + (mid - buf_start_at) * 2), 
      &value, sizeof(u16));
  }
  return mid;
}

static BOOL af_check_char(void *p_priv, rsc_font_t *p_hdr_font, u16 char_code)
{
  if((char_code >= p_hdr_font->start_code) && (char_code <= p_hdr_font->end_code))
  {
    return TRUE;
  }

  return FALSE;
}

static u8 *af_hit_cache(void *p_priv, u16 code, rsc_fstyle_t *p_fstyle, 
  u16 *p_width, u16 *p_height, u16 *p_xstep, u16 *p_pitch)
{
  af_cache_t cache = {0};
  u8 i = 0, j = 0;
  af_priv_t *p_af = (af_priv_t *)p_priv;
  
  for(i = 0; i < p_af->cache_cnt; i++) //search in cache
  {
    if((p_af->p_cache[i].char_code == code) &&
      (memcmp(&p_af->p_cache[i].fstyle, p_fstyle, sizeof(rsc_fstyle_t))== 0))
    {
      cache = p_af->p_cache[i];

      for(j = i; j > 0; (j--))
      {
        p_af->p_cache[j] = p_af->p_cache[j - 1];
      }

      p_af->p_cache[0] = cache;

      *p_width = cache.width;
      *p_height = cache.height;
      *p_xstep = cache.x_step;
      *p_pitch = cache.pitch;
      
      return (u8 *)cache.p_char;
    }
  }
  
  return NULL;
}

static void *af_insert_cache(void *p_priv,
  u16 char_code, rsc_fstyle_t *p_fstyle, u16 width, u16 height, u16 x_step, u16 pitch)
{
  af_priv_t *p_af = (af_priv_t *)p_priv;
  u8 i = 0, *p_data_buf = NULL;

  p_data_buf =  p_af->p_cache[p_af->cache_cnt -1].p_char;
  
  for(i = (p_af->cache_cnt - 1); i > 0; (i--))
  {
    p_af->p_cache[i] = p_af->p_cache[i - 1];
  }

  p_af->p_cache[0].p_char = p_data_buf;
  p_af->p_cache[0].char_code = char_code;
  p_af->p_cache[0].fstyle= *p_fstyle;
  p_af->p_cache[0].width = width;
  p_af->p_cache[0].height = height;
  p_af->p_cache[0].x_step = x_step;
  p_af->p_cache[0].pitch = pitch;

  return p_data_buf;
}

static BOOL af_get_char(void *p_priv,
  u16 char_code, rsc_fstyle_t *p_fstyle, rsc_char_info_t *p_info)
{
  af_priv_t *p_af = (af_priv_t *)p_priv;
  u16 char_offset = 0, char_count = 0;
  rsc_font_t hdr_font = {{0}};
  u8 *p_font = NULL, *p_char = NULL;
  u8 *p_index_tab = NULL, *p_width_tab = NULL;
  u8 *p_offset_tab = NULL;
  u32 data_offset = 0, size = 0;
  u8 char_width = 0;

  memset(p_info, 0, sizeof(rsc_char_info_t));

  if((p_info->p_char = af_hit_cache(p_priv, char_code,
    p_fstyle, &p_info->width, &p_info->height, &p_info->x_step, &p_info->pitch)) != NULL)
  {
    p_info->ckey = p_fstyle->color ? 0 : 1;
    p_info->bpp = p_af->bpp;

    return TRUE;
  }

  if(!rsc_get_font(p_af->rsc_handle, p_fstyle->font_id, &hdr_font, &p_font))
  {
    return FALSE;
  }

  MT_ASSERT(hdr_font.type == FONT_ARRAY);

  if(!af_check_char(p_priv, &hdr_font, char_code))
  {
    return FALSE;
  }

  char_count = hdr_font.count;
  p_index_tab = p_font;
  p_width_tab = p_font + char_count * sizeof(u16);
  p_offset_tab = p_width_tab + char_count * sizeof(u8);
  p_char = p_offset_tab + char_count * sizeof(u32);

  /*find char*/
  char_offset = af_search(p_af->rsc_handle, p_index_tab, char_count, char_code);
  rsc_read_data(p_af->rsc_handle, (u32)(p_offset_tab + sizeof(u32) * char_offset),
                &data_offset, sizeof(u32));

  p_char += data_offset;
  p_info->height = hdr_font.height;

  rsc_read_data(p_af->rsc_handle, (u32)(p_width_tab + char_offset), &char_width, sizeof(u8));
  p_info->width = char_width;
  p_info->x_step = char_width;

  size = (u32)(p_info->height * ((p_info->width * p_af->bpp + 7) >> 3));
  if(size > p_af->cache_size)
  {
    return FALSE;
  }

  if(p_af->bpp > 8)
  {
    p_info->pitch = (p_info->width * (p_af->bpp / 8) + 7)  /  8 * 8;
  }
  else
  {
     p_info->pitch = (p_info->width * p_af->bpp + 7) >> 3;
  }

  p_info->p_char = af_insert_cache(p_priv, char_code,
    p_fstyle, p_info->width, p_info->height, p_info->x_step, p_info->pitch);

  p_info->ckey = af_transform_char(p_priv, p_char, p_info->width,
                  p_info->height, p_info->pitch, p_info->p_char, p_fstyle->color, p_af->bpp);
  
  p_info->xoffset = 0;
  p_info->yoffset = 0;
  p_info->bpp = p_af->bpp;

  return TRUE;
}

static BOOL af_get_attr(void *p_priv,
  u16 char_code, rsc_fstyle_t *p_fstyle, u16 *p_width, u16 *p_height)
{
  af_priv_t *p_af = (af_priv_t *)p_priv;
  rsc_font_t hdr_font = {{0}};
  u8 *p_font = NULL;
  u16 array_count = 0;
  u16 char_offset = 0;
  u8 *p_index_tab = NULL, *p_width_tab = NULL;
  u8 char_width = 0;

  MT_ASSERT(p_af != NULL);
  MT_ASSERT(p_fstyle != NULL);
  MT_ASSERT(p_width != NULL);
  MT_ASSERT(p_height != NULL);

  if(!rsc_get_font(p_af->rsc_handle, p_fstyle->font_id, &hdr_font, &p_font))
  {
    *p_width = *p_height = 0;
    
    return FALSE;
  }

  MT_ASSERT(hdr_font.type == FONT_ARRAY);

  if(!af_check_char(p_priv, &hdr_font, char_code))
  {
    return FALSE;
  }  

  if(hdr_font.width > 0)
  {
    *p_width = hdr_font.width;
  }
  else
  {
    array_count = hdr_font.count;
    p_index_tab = p_font;
    p_width_tab = p_font + array_count * sizeof(u16);

    char_offset = af_search(p_af->rsc_handle, p_index_tab, array_count, char_code);

    rsc_read_data(p_af->rsc_handle, (u32)(p_width_tab + char_offset), &char_width, sizeof(u8));
    *p_width = char_width;
  }

  *p_height = hdr_font.height;      
  
  return TRUE;
}

void af_attach(handle_t rsc_handle, af_cfg_t *p_cfg)
{
  RET_CODE ret = SUCCESS;
  rsc_main_t *p_rsc_info = NULL;
  af_priv_t *p_priv = NULL;
  u32 i = 0;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  p_priv = (af_priv_t *)mmi_alloc_buf(sizeof(af_priv_t));
  MT_ASSERT(p_priv != NULL);

  memset(p_priv, 0, sizeof(af_priv_t));
  
  p_rsc_info->p_priv = p_priv;

  p_priv->bpp = p_cfg->bpp;


  if(p_cfg->cache_cnt != 0 && p_cfg->cache_size != 0)
  {
    p_priv->p_cache = mtos_malloc(p_cfg->cache_cnt * sizeof(af_cache_t));
    MT_ASSERT(p_priv->p_cache != 0);
    memset(p_priv->p_cache, 0, p_cfg->cache_cnt * sizeof(af_cache_t));
    
    for(i = 0; i < p_cfg->cache_cnt; i++)
    {
      p_priv->p_cache[i].p_char = mtos_align_malloc(p_cfg->cache_size, 8);
      MT_ASSERT(p_priv->p_cache[i].p_char != NULL);
    }
  }

  p_priv->cache_size = p_cfg->cache_size;
  p_priv->cache_cnt = p_cfg->cache_cnt;

  //attach function.
  p_rsc_info->get_attr = af_get_attr;
  p_rsc_info->get_char = af_get_char;

  p_priv->rsc_handle = rsc_handle;

  MT_ASSERT(ret == SUCCESS);
}

void af_detach(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  af_priv_t *p_priv = NULL;
  u32 i = 0;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  p_rsc_info->get_attr = NULL;
  p_rsc_info->get_char = NULL;

  p_priv = (af_priv_t *)p_rsc_info->p_priv;

  if(p_priv != NULL)
  {
    if(p_priv->p_cache != NULL)
    {
      for(i = 0; i < p_priv->cache_cnt; i++)
      {
        if(p_priv->p_cache[i].p_char != NULL)
        {
          mtos_align_free(p_priv->p_cache[i].p_char);

          p_priv->p_cache[i].p_char = NULL;
        }
      }

      mtos_free(p_priv->p_cache);

      p_priv->p_cache = NULL;
    } 
  }

  mmi_free_buf((void *)p_priv);  

  p_priv = NULL;
}
    

