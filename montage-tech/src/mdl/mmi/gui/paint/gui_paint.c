/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file gui_paint.c
   This file  implement the funcs defined in  gui_paint.h,and some internal used
   functions.
   These functions are mainly used to draw a bitmap/string/......
  */
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
#include "lib_util.h"
#include "lib_char.h"
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
#include "gdi_anim.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"
#include "gui_pango.h"

#include "ctrl_container.h"

#include "class_factory.h"


//#define PAINT_DEBUG

#ifdef PAINT_DEBUG
#define PAINT_PRINTF    OS_PRINTF
#else
#define PAINT_PRINTF DUMMY_PRINTF
#endif


/*!
   gui map info.
  */
typedef struct
{
  /*!
     unicode
    */
  //u16 *p_line_str_code;
  /*!
    string convert(for arabic string)
    */
  u16 *p_str_convert;
  /*!
    string base(for arabic string)
    */
  u16 *p_str_base;    
  /*!
     char pos.
    */
  u16 *p_char_pos;
  /*!
     max count per line.
    */
  u32 line_str_max_cnt;
  /*!
     max lines.
    */
  u32 line_str_max_lines;
  /*!
    line width. 
    */
  u16 *p_linew;
  /*!
    line height. 
    */
  u16 *p_lineh; 
  /*!
    line position. 
    */
  u16 **pp_linep;  
  /*!
    line character numbers. 
    */
  u16 *p_lines;     
  /*!
    resource handle
    */
  handle_t rsc_handle;
  /*!
    chip ic.
    */
  chip_ic_t chip_ic;  
  /*!
    pango handle
    */
  handle_t h_pango;
  /*!
    anim mode.
    */
  anim_mode_t anim_mode;    
  /*!
    animation in
    */
  anim_in_t anim_in;
  /*!
    animation out
    */
  anim_out_t anim_out;    
}gui_main_t;

/*!
   Get bitmap information and data from resource(the structure bmp_info and data
   pointer).

   \param[in] p_bmp_info : bitmap information from resource.
   \param[in] p_data : bitmap data buffer, for store the bitmap data.
   \param[out] p_bmp	: structure for saving bitmap information.
   \return : NULL
  */
static void _get_bmp_from_rsc(rsc_bitmap_t *p_bmp_info,
                              u8 *p_data,
                              bitmap_t *p_bmp);

/*!
   Draw icon.

   \param[in] hdc : dc
   \param[in] p_rc : rect to fill
   \param[in] p_data : bitmap data buffer
   \param[in] p_head : bitmap resource head
   \param[in] flag : fill flag : tile, stretch, .......
   \return : NULL
  */
static void _draw_icon(hdc_t hdc,
                       rect_t *p_rc,
                       u8 *p_data,
                       rsc_bitmap_t *p_head,
                       u32 flag);

/*!
   Fill background.

   \param[in] hdc : dc
   \param[in] p_rc : rect to be filled.
   \param[in] p_rstyle : rect style to fill the rect.
   \return : NULL
  */
static void _fill_background(hdc_t hdc, rect_t *p_rc, rsc_rstyle_t *p_rstyle);

/*!
   Get bitmap size(width and height).

   \param[in] p_rc : rect to be filled by bitmap.
   \param[in] p_head : bitmap resource head.
   \param[out] p_w : output width of bitmap.
   \param[out] p_h : output height of bitmap.
   \return : NULL
  */
static void _get_bmp_size(rect_t *p_rc,
                          rsc_bitmap_t *p_head,
                          s16 *p_w,
                          s16 *p_h);

/*!
   Get bound width.

   \param[in] p_rc				: rect to be filled.
   \param[in] color			: bound color.
   \param[in] p_head			: bitmap resource head.
   \return					: bound width.
  */
static u16 _get_bound_w(rect_t *p_rc, rsc_part_style_t *p_part, rsc_bitmap_t *p_head);


/*!
   Get bound height.

   \param[in] p_rc : rect to be filled.
   \param[in] color : bound color.
   \param[in] p_head : bitmap resource head.
   \return : bound height.
  */
static u16 _get_bound_h(rect_t *p_rc, rsc_part_style_t *p_part, rsc_bitmap_t *p_head);

/*!
   Get real bound width.

   \param[in/out] cur_width : current width.
   \param[in/out] bound_width : bound width.
   \return : TRUE means need draw background, otherwise return FALSE.
  */
static BOOL _get_real_bound(u16 *p_cur_width, u16 *p_bound_width);

/*!
   Get line information.

   \param[in] f_style : font style.
   \param[in] rc : rect
   \param[in] style : align style
   \param[in] x_offset : offset in horizontal direction
   \param[in] y_offset : offset in vertical direction
   \param[in] line_space : interval between lines.
   \param[in] str : string buffer.
   \param[out] top : top position
   \param[out] status : string is end | how many chars have been
   processed(include format sign)
   
   \return	: total lines needed
  */
static u8 gui_get_line_data(rsc_fstyle_t *p_fstyle,
                              rect_t *p_rc,
                              u32 style,
                              u16 x_offset,
                              u16 y_offset,
                              u8 line_space,
                              u16 *p_str,
                              u16 *p_top,
                              u32 *p_status,
                              u32 draw_style);
                              

#ifdef PAINT_DEBUG
void gui_dump_bmp_head(rsc_bitmap_t *p_head)
{
  PAINT_PRINTF("dump bitmap head...\n");
  PAINT_PRINTF("\t pixcel_type       = %d\n", head->pixel_type);
  PAINT_PRINTF("\t en ckey           = %d\n", head->enable_ckey);
  PAINT_PRINTF("\t palette id        = %d\n", head->palette_id);
  PAINT_PRINTF("\t width             = %d\n", head->width);
  PAINT_PRINTF("\t height            = %d\n", head->height);
  PAINT_PRINTF("\t colorkey          = 0x%x\n", head->colorkey);
  PAINT_PRINTF("\t pitch             = %d\n", head->pitch);
  PAINT_PRINTF("\t\t type            = %d\n", head->head.type);
  PAINT_PRINTF("\t\t id              = %d\n", head->head.id);
  PAINT_PRINTF("\t\t c_mode          = %d\n", head->head.c_mode);
  PAINT_PRINTF("\t\t cmp_size        = %d\n", head->head.cmp_size);
  PAINT_PRINTF("\t\t org_size        = %d\n", head->head.org_size);
}


void gui_dump_bmp(bitmap_t *p_bmp)
{
  PAINT_PRINTF("dump bitmap...\n");
  PAINT_PRINTF("\t pixcel_type       = %d\n", p_bmp->format);
  PAINT_PRINTF("\t en ckey           = %d\n", p_bmp->enable_ckey);
  PAINT_PRINTF("\t bpp               = %d\n", p_bmp->bpp);
  PAINT_PRINTF("\t width             = %d\n", p_bmp->width);
  PAINT_PRINTF("\t height            = %d\n", p_bmp->height);
  PAINT_PRINTF("\t pitch             = %d\n", p_bmp->pitch);
  PAINT_PRINTF("\t colorkey          = 0x%x\n", p_bmp->colorkey);
}


#endif


static void _get_bmp_from_rsc(rsc_bitmap_t *p_hdr_bmp,
                              u8 *p_bits,
                              bitmap_t *p_bmp)
{
  rsc_palette_t hdr_pal;
  u8 *p_entrys = NULL;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_hdr_bmp != NULL && p_bmp != NULL);

  p_bmp->format = (u8)p_hdr_bmp->pixel_type;
  p_bmp->enable_ckey = (u8)p_hdr_bmp->enable_ckey;
  p_bmp->bpp = gdi_get_bpp((pix_type_t)p_hdr_bmp->pixel_type);
  p_bmp->p_bits = p_bits;
  p_bmp->colorkey = p_hdr_bmp->colorkey;
  p_bmp->width = p_hdr_bmp->width;
  p_bmp->height = p_hdr_bmp->height;

  if(p_bmp->bpp <= 8)
  {
    p_bmp->pal.cnt = 1 << p_bmp->bpp;
    rsc_get_palette(p_info->rsc_handle,
      rsc_get_curn_palette(p_info->rsc_handle), &hdr_pal, &p_entrys);
    p_bmp->pal.p_entry = (color_t *)p_entrys;
    MT_ASSERT(p_bmp->pal.cnt == hdr_pal.color_num);
  }

  p_bmp->pitch = p_hdr_bmp->pitch;
#ifdef PAINT_DEBUG
  gui_dump_bmp(p_bmp);
#endif
}


static void _draw_icon(hdc_t hdc,
                       rect_t *p_rc,
                       u8 *p_bits,
                       rsc_bitmap_t *p_hdr_bmp,
                       u32 flag)
{
  bitmap_t bmp = {0};
  hbitmap_t c_bmp = 0;

  normalize_rect(p_rc);

  _get_bmp_from_rsc(p_hdr_bmp, p_bits, &bmp);

  c_bmp = gdi_create_compatible_bitmap(hdc, &bmp);

  gdi_fill_bitmap(hdc, p_rc, (hbitmap_t)c_bmp, flag);
}


static void _fill_background(hdc_t hdc, rect_t *p_rc, rsc_rstyle_t *p_rstyle)
{
  u8 *p_bits = NULL;
  rsc_bitmap_t hdr_bmp;
  u32 flag = 0;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(RSTYLE_IS_R_ICON(p_rstyle->bg))
  {
    if(rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->bg), &hdr_bmp, &p_bits))
    {
      switch(RSTYLE_GET_ICON_STYLE(p_rstyle->bg))
      {
        case R_TILE:
          flag = FILL_FLAG_TILE | FILL_DIRECTION_HORI | FILL_DIRECTION_VERT;
          break;

        case R_CENTER:
          flag = FILL_FLAG_CENTER;
          break;

        case R_STRETCH:
          flag = FILL_FLAG_STRETCH;
          break;

        default:
          MT_ASSERT(0);
          break;
      }
      
      _draw_icon(hdc, p_rc, p_bits, &hdr_bmp, flag);
    }
  }
  else if (RSTYLE_IS_R_COLOR(p_rstyle->bg))
  {
    gui_fill_rect(hdc, p_rc, RSTYLE_GET_COLOR(p_rstyle->bg));
  }
}


static void _get_bmp_size(rect_t *p_rc,
                          rsc_bitmap_t *p_hdr_bmp,
                          s16 *p_w,
                          s16 *p_h)
{
  u16 bmp_w = 0, bmp_h = 0;

  bmp_w = p_hdr_bmp->width;
  bmp_h = p_hdr_bmp->height;
  *p_w = (bmp_w < (p_rc->right - p_rc->left)) ? bmp_w :
         (p_rc->right - p_rc->left);
  *p_h = (bmp_h < (p_rc->bottom - p_rc->top)) ? bmp_h :
         (p_rc->bottom - p_rc->top);
}


static u16 _get_bound_w(rect_t *p_rc, rsc_part_style_t *p_part, rsc_bitmap_t *p_hdr_bmp)
{
  u16 w = 0, h = 0;

  if(RSTYLE_IS_R_ICON(*p_part))
  {
    if(NULL != p_hdr_bmp)
    {
      _get_bmp_size(p_rc, p_hdr_bmp, (s16 *)&w, (s16 *)&h);
    }
    else
    {
      w = 0;
    }
  }
  else
  {
    w = (u16)RSTYLE_GET_LINE_WIDTH(*p_part);
    if(w > (p_rc->right - p_rc->left))
    {
      w = p_rc->right - p_rc->left;
    }
  }
  return w;
}


static u16 _get_bound_h(rect_t *p_rc, rsc_part_style_t *p_part, rsc_bitmap_t *p_head)
{
  u16 w = 0, h = 0;

  if(RSTYLE_IS_R_ICON(*p_part))
  {
    if(NULL != p_head)
    {
      _get_bmp_size(p_rc, p_head, (s16 *)&w, (s16 *)&h);
    }
    else
    {
      h = 0;
    }
  }
  else
  {
    h = (u16)RSTYLE_GET_LINE_WIDTH(*p_part);
    if(h > (p_rc->bottom - p_rc->top))
    {
      h = p_rc->bottom - p_rc->top;
    }
  }
  return h;
}


//return TRUE means need draw background
static BOOL _get_real_bound(u16 *p_cur_width, u16 *p_bound_width)
{
  if(0 != *p_bound_width)
  {
    if(0 != *p_cur_width)
    {
      if(*p_bound_width >= *p_cur_width)
      {
        *p_bound_width = *p_cur_width;
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
    else
    {
      *p_cur_width = *p_bound_width;
      return FALSE;
    }
  }
  else
  {
    return TRUE;
  }
}

handle_t gui_get_rsc_handle(void)
{
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  return p_info->rsc_handle;
}

u32 gui_get_string_addr(u16 string_id)
{
  u8 *p_addr = NULL;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(!rsc_get_string(p_info->rsc_handle,
    rsc_get_curn_language(p_info->rsc_handle), string_id, &p_addr))
  {
    p_addr = NULL;
  }

  return (u32)(p_addr);
}


BOOL gui_get_string(u16 string_id, u16 *p_buffer, u16 length)
{
#if 0
  u8 *str_tab;
  u16 code, i = 0;
  u16 code;

  MT_ASSERT(string_id != RSC_INVALID_ID
           && length != 0
           && buffer != NULL);

  *buffer = '\0';
  rsc_lock_data(RSC_TYPE_STRING);
  str_tab = rsc_get_string(rsc_get_curn_language(), string_id);
  //here we use uni_strcat because I want use buffer_size for safety check
//  uni_strcat(buffer, (u16*)str_tab, length);

  rsc_read_data((u32)(str_tab), &code, sizeof(u16));
  while(code != '\0' && i < length)
  {
    *buffer++ = code;
    str_tab += 2;
    i++;
    rsc_read_data((u32)(str_tab), &code, sizeof(u16));
  }
  *buffer = '\0';
  rsc_unlock_data(RSC_TYPE_STRING);

  return TRUE;

#else
  u8 *p_str_addr = NULL;
  u16 i = 0;
  u16 j = 0;
  u16 *p_buf_addr = NULL;
  u32 unread_size = 0, read_size = 0, buf_size = 0;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);


  MT_ASSERT(string_id != RSC_INVALID_ID
           && length != 0
           && p_buffer != NULL);

  p_buf_addr = (u16 *)rsc_get_comm_buf_addr(p_info->rsc_handle);
  buf_size = rsc_get_comm_buf_size(p_info->rsc_handle) / sizeof(u16);
  unread_size = length;

  *p_buffer = '\0';
//  rsc_lock_data(RSC_TYPE_STRING);
  if(!rsc_get_string(p_info->rsc_handle,
    rsc_get_curn_language(p_info->rsc_handle), string_id, &p_str_addr))
  {
    PAINT_PRINTF("gui_get_string: can NOT get string(id = %d), ERROR!\n",
                 string_id);
    return FALSE;
  }

  read_size = unread_size > buf_size ? buf_size : unread_size;
  unread_size -= read_size;
  rsc_read_data(p_info->rsc_handle, (u32)(p_str_addr), p_buf_addr, read_size * sizeof(u16));
  while(p_buf_addr[j] != '\0' && i < length)
  {
    *p_buffer++ = p_buf_addr[j];
    p_str_addr += 2;
    i++;
    j++;
    if(j == buf_size)
    {
      j = 0;
      read_size = unread_size > buf_size ? buf_size : unread_size;
      unread_size -= read_size;
      rsc_read_data(p_info->rsc_handle, (u32)(p_str_addr), p_buf_addr, read_size * sizeof(u16));
    }
  }
  *p_buffer = '\0';
//  rsc_unlock_data(RSC_TYPE_STRING);

  return TRUE;
#endif
}


void gui_set_palette(BOOL is_top_screen, u16 pal_id, u16 trans)
{
  u16 color_num = 0;
  rsc_palette_t hdr_pal;
  u8 *p_bits = NULL;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(pal_id != RSC_INVALID_ID);

//  rsc_lock_data(RSC_TYPE_PALETTE);
  if(!rsc_get_palette(p_info->rsc_handle, pal_id, &hdr_pal, (u8 **)&p_bits))
  {
    PAINT_PRINTF("gui_set_palette: can NOT get palette(id = %d), ERROR!\n",
                 pal_id);
    return;
  }
  rsc_set_curn_palette(p_info->rsc_handle, pal_id);

  color_num = hdr_pal.color_num;
  if(trans < color_num)
  {
    color_t *p_entry = (color_t *)p_bits;
    p_entry[trans].a = 0x0;
  }
  gdi_set_palette(is_top_screen, 0, color_num, (color_t *)p_bits);
//  rsc_unlock_data(RSC_TYPE_PALETTE);
}


BOOL gui_get_bmp_info(u16 bmp_id, u16 *p_width, u16 *p_height)
{
  u32 addr = 0;
  rsc_bitmap_t hdr_bmp;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  addr = rsc_get_hdr_addr(p_info->rsc_handle, RSC_TYPE_BITMAP, bmp_id);
  if(addr != 0)
  {
    rsc_read_data(p_info->rsc_handle, addr, &hdr_bmp, sizeof(rsc_bitmap_t));
    *p_width = hdr_bmp.width;
    *p_height = hdr_bmp.height;
    return TRUE;
  }
  else
  {
    *p_width = *p_height = 0;
    return FALSE;
  }
}


// the corner must be icons.
void _draw_style_rect_corner(hdc_t hdc,
                             rect_t *p_rc,
                             rsc_rstyle_t *p_rstyle,
                             u16 *p_lw,
                             u16 *p_rw,
                             u16 *p_th,
                             u16 *p_bh)
{
  rsc_bitmap_t hdr_bmp;
  u8 *p_bits = NULL;
  s16 w = 0, h = 0;
  rect_t rc_out;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  //left top
  if(RSTYLE_IS_R_ICON(p_rstyle->left_top))
  {
    if(rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->left_top), &hdr_bmp, &p_bits))
    {
      _get_bmp_size(p_rc, &hdr_bmp, &w, &h);

      set_rect(&rc_out, p_rc->left, p_rc->top,
               (s16)(p_rc->left + w), (s16)(p_rc->top + h));

      _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, 0);
      *p_lw = w;
      *p_th = h;
    }
  }

  //right bottom
  if(RSTYLE_IS_R_ICON(p_rstyle->right_bottom))
  {
    if(rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->right_bottom), &hdr_bmp, &p_bits))
    {
      _get_bmp_size(p_rc, &hdr_bmp, &w, &h);
      set_rect(&rc_out, (s16)(p_rc->right - w), (s16)(p_rc->bottom - h),
               p_rc->right, p_rc->bottom);
      _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, 0);
      *p_rw = w;
      *p_bh = h;
    }
  }

  //right top
  if(RSTYLE_IS_R_ICON(p_rstyle->right_top))
  {
    if(rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->right_top), &hdr_bmp, &p_bits))
    {
      _get_bmp_size(p_rc, &hdr_bmp, &w, &h);
      set_rect(&rc_out, (s16)(p_rc->right - w), p_rc->top,
               p_rc->right, (s16)(p_rc->top + h));
      _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, 0);
      *p_rw = w;
      *p_th = h;
    }
  }


  //left bottom
  if(RSTYLE_IS_R_ICON(p_rstyle->left_bottom))
  {
    if(rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->left_bottom), &hdr_bmp, &p_bits))
    {
      _get_bmp_size(p_rc, &hdr_bmp, &w, &h);
      set_rect(&rc_out, p_rc->left, (s16)(p_rc->bottom - h),
               (s16)(p_rc->left + w), p_rc->bottom);
      _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, 0);
      *p_lw = w;
      *p_bh = h;
    }
  }
}

// the border can be icon or color
void _draw_style_rect_border(hdc_t hdc,
                             rect_t *p_rc,
                             rsc_rstyle_t *p_rstyle,
                             u16 lw,
                             u16 rw,
                             u16 th,
                             u16 bh)
{
  rsc_bitmap_t hdr_bmp;
  u8 *p_bits = NULL;
  u16 w = 0, h = 0;
  rect_t rc_out = {0};
  u32 flag = 0;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  //left
  if((!RSTYLE_IS_R_IGNORE(p_rstyle->left)) 
    && (!RSTYLE_IS_R_COPY_BG(p_rstyle->left))
    && ((p_rc->bottom - p_rc->top) > (th + bh)))
  {
    if(RSTYLE_IS_R_ICON(p_rstyle->left))
    {
      rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->left), &hdr_bmp, &p_bits);
    }
    w = _get_bound_w(p_rc, &p_rstyle->left, &hdr_bmp);
    if(_get_real_bound(&lw, &w))
    {
      set_rect(&rc_out, (s16)(p_rc->left + w), (s16)(p_rc->top + th),
               (s16)(p_rc->left + lw), (s16)(p_rc->bottom - bh));
      _fill_background(hdc, &rc_out, p_rstyle);
    }
    if(0 != w)
    {
      set_rect(&rc_out, p_rc->left, (s16)(p_rc->top + th),
               (s16)(p_rc->left + w), (s16)(p_rc->bottom - bh));
      if(RSTYLE_IS_R_ICON(p_rstyle->left))
      {
        switch(RSTYLE_GET_ICON_STYLE(p_rstyle->left))
        {
          case R_TILE:
            flag = FILL_FLAG_TILE | FILL_DIRECTION_VERT;
            break;

          case R_CENTER:
            flag = FILL_FLAG_CENTER;
            break;

          case R_STRETCH:
            flag = FILL_FLAG_STRETCH;
            break;

          default:
            MT_ASSERT(0);
            break;
        }      
        _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, flag);
      }
      else
      {
        gui_fill_rect(hdc, &rc_out, RSTYLE_GET_COLOR(p_rstyle->left));
      }
    }
  }

  //top
  if((!RSTYLE_IS_R_IGNORE(p_rstyle->top)) 
    && (!RSTYLE_IS_R_COPY_BG(p_rstyle->top))
    && ((p_rc->right - p_rc->left) > (lw + rw)))
  {
    if(RSTYLE_IS_R_ICON(p_rstyle->top))
    {
      rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->top), &hdr_bmp, &p_bits);
    }
    h = _get_bound_h(p_rc, &(p_rstyle->top), &hdr_bmp);
    if(_get_real_bound(&th, &h))
    {
      set_rect(&rc_out, (s16)(p_rc->left + lw), (s16)(p_rc->top + h),
               (s16)(p_rc->right - rw), (s16)(p_rc->top + th));
      _fill_background(hdc, &rc_out, p_rstyle);
    }
    if(0 != h)
    {
      set_rect(&rc_out, (s16)(p_rc->left + lw), p_rc->top,
               (s16)(p_rc->right - rw), (s16)(p_rc->top + h));
      if(RSTYLE_IS_R_ICON(p_rstyle->top))
      {
        switch(RSTYLE_GET_ICON_STYLE(p_rstyle->top))
        {
          case R_TILE:
            flag = FILL_FLAG_TILE | FILL_DIRECTION_HORI;
            break;

          case R_CENTER:
            flag = FILL_FLAG_CENTER;
            break;

          case R_STRETCH:
            flag = FILL_FLAG_STRETCH;
            break;

          default:
            MT_ASSERT(0);
            break;
        }      
        _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, flag);
      }
      else
      {
        gui_fill_rect(hdc, &rc_out, RSTYLE_GET_COLOR(p_rstyle->top));
      }
    }
  }

  //right
  if((!RSTYLE_IS_R_IGNORE(p_rstyle->right))
    && (!RSTYLE_IS_R_COPY_BG(p_rstyle->right))
    && ((p_rc->bottom - p_rc->top) > (th + bh)))
  {
    if(RSTYLE_IS_R_ICON(p_rstyle->right))
    {
      rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->right), &hdr_bmp, &p_bits);
    }
    w = _get_bound_w(p_rc, &(p_rstyle->right), &hdr_bmp);
    if(_get_real_bound(&rw, &w))
    {
      set_rect(&rc_out, (s16)(p_rc->right - rw), (s16)(p_rc->top + th),
               (s16)(p_rc->right - w), (s16)(p_rc->bottom - bh));
      _fill_background(hdc, &rc_out, p_rstyle);
    }
    if(0 != w)
    {
      set_rect(&rc_out, (s16)(p_rc->right - w), (s16)(p_rc->top + th),
               p_rc->right, (s16)(p_rc->bottom - bh));
      if(RSTYLE_IS_R_ICON(p_rstyle->right))
      {
        switch(RSTYLE_GET_ICON_STYLE(p_rstyle->right))
        {
          case R_TILE:
            flag = FILL_FLAG_TILE | FILL_DIRECTION_VERT;
            break;

          case R_CENTER:
            flag = FILL_FLAG_CENTER;
            break;

          case R_STRETCH:
            flag = FILL_FLAG_STRETCH;
            break;

          default:
            MT_ASSERT(0);
            break;
        }      
        _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, flag);      
      }
      else
      {
        gui_fill_rect(hdc, &rc_out, RSTYLE_GET_COLOR(p_rstyle->right));
      }
    }
  }

  //bottom
  if((!RSTYLE_IS_R_IGNORE(p_rstyle->bottom))
    && (!RSTYLE_IS_R_COPY_BG(p_rstyle->bottom))
    && ((p_rc->right - p_rc->left) > (lw + rw)))
  {
    if(RSTYLE_IS_R_ICON(p_rstyle->bottom))
    {
      rsc_get_bmp(p_info->rsc_handle, RSTYLE_GET_ICON(p_rstyle->bottom), &hdr_bmp, &p_bits);
    }
    h = _get_bound_h(p_rc, &(p_rstyle->bottom), &hdr_bmp);
    if(_get_real_bound(&bh, &h))
    {
      set_rect(&rc_out, (s16)(p_rc->left + lw), (s16)(p_rc->bottom - bh),
               (s16)(p_rc->right - rw), (s16)(p_rc->bottom - h));
      _fill_background(hdc, &rc_out, p_rstyle);
    }
    if(0 != h)
    {
      set_rect(&rc_out, (s16)(p_rc->left + lw), (s16)(p_rc->bottom - h),
               (s16)(p_rc->right - rw), p_rc->bottom);
      if(RSTYLE_IS_R_ICON(p_rstyle->bottom))
      {
        switch(RSTYLE_GET_ICON_STYLE(p_rstyle->bottom))
        {
          case R_TILE:
            flag = FILL_FLAG_TILE | FILL_DIRECTION_HORI;
            break;

          case R_CENTER:
            flag = FILL_FLAG_CENTER;
            break;

          case R_STRETCH:
            flag = FILL_FLAG_STRETCH;
            break;

          default:
            MT_ASSERT(0);
            break;
        }      
        _draw_icon(hdc, &rc_out, p_bits, &hdr_bmp, flag);
      }
      else
      {
        gui_fill_rect(hdc, &rc_out, RSTYLE_GET_COLOR(p_rstyle->bottom));
      }
    }
  }

  //background
  if((!RSTYLE_IS_R_IGNORE(p_rstyle->bg))
    && (!RSTYLE_IS_R_COPY_BG(p_rstyle->bg))
    && ((p_rc->right - p_rc->left) > (lw + rw))
    && ((p_rc->bottom - p_rc->top) > (th + bh)))
  {
    set_rect(&rc_out, (s16)(p_rc->left + lw), (s16)(p_rc->top + th),
             (s16)(p_rc->right - rw), (s16)(p_rc->bottom - bh));
    _fill_background(hdc, &rc_out, p_rstyle);
  }
}


void gui_draw_style_rect(hdc_t hdc, rect_t *p_rc, rsc_rstyle_t *p_rstyle)
{
  u16 lw = 0, rw = 0, th = 0, bh = 0;

  normalize_rect(p_rc);

  //  rsc_lock_data(RSC_TYPE_BITMAP);
  switch (RSTYLE_GET_SNAP(p_rstyle->bg))
  {
    case R_SNAP_BORDER:
      _draw_style_rect_border(hdc, p_rc, p_rstyle, lw, rw, th, bh);
      _draw_style_rect_corner(hdc, p_rc, p_rstyle, &lw, &rw, &th, &bh);
      break;

    default: // R_SNAP_VERTEX
      _draw_style_rect_corner(hdc, p_rc, p_rstyle, &lw, &rw, &th, &bh);
      _draw_style_rect_border(hdc, p_rc, p_rstyle, lw, rw, th, bh);
  }
//  rsc_unlock_data(RSC_TYPE_BITMAP);
}

void gui_draw_image_rect(hdc_t hdc, rect_t *p_rc, u16 win_bg)
{
  gui_main_t *p_info = NULL;
  rsc_bitmap_t hdr_bmp = {{0}};
  u8 *p_bits = NULL;
  
  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  normalize_rect(p_rc);

  if(rsc_get_bmp(p_info->rsc_handle, win_bg, &hdr_bmp, &p_bits))
  {
    _draw_icon(hdc, p_rc, p_bits, &hdr_bmp, FILL_FLAG_CENTER);
  }
}

void gui_get_posi(rect_t *p_rc,
                  u32 style,
                  u16 x_offset,
                  u16 y_offset,
                  u16 width,
                  u16 height,
                  u16 *p_left,
                  u16 *p_top)
{
  if(style & STL_LEFT)
  {
    *p_left = p_rc->left + x_offset;
  }
  else if(style & STL_RIGHT)
  {
    *p_left = p_rc->right - width;
  }
  else
  {
    *p_left = p_rc->left + x_offset + (RECTWP(p_rc) - x_offset - width) / 2;
  }
  /**/
  if(*p_left < p_rc->left)
  {
    *p_left = p_rc->left;
  }

  if(style & STL_TOP)
  {
    *p_top = p_rc->top + y_offset;
  }
  else if(style & STL_BOTTOM)
  {
    *p_top = p_rc->bottom - height;
  }
  else
  {
    *p_top = p_rc->top + y_offset + (RECTHP(p_rc) - y_offset - height) / 2;
  }
  /**/
  if(*p_top < p_rc->top)
  {
    *p_top = p_rc->top;
  }
}


void gui_draw_hor_line(hdc_t hdc, u16 x, u16 y, u16 width, u32 color)
{
  rect_t line;

  set_rect(&line, x, y, (s16)(x + width), (s16)(y + 2));
  gdi_fill_rect(hdc, &line, color);
}


void gui_draw_picture(hdc_t hdc,
                      rect_t *p_rc,
                      u32 style,
                      u16 x_offset,
                      u16 y_offset,
                      u16 bmp_id)
{
  rsc_bitmap_t hdr_bmp;
  bitmap_t bmp_info = {0};
  u8 *p_bits = NULL;
  u16 left = 0, top = 0;
  rect_t rc_out;
  hbitmap_t c_bmp = 0;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(bmp_id == RSC_INVALID_ID)
  {
    return;
  }

//  rsc_lock_data(RSC_TYPE_BITMAP);
  if(rsc_get_bmp(p_info->rsc_handle, bmp_id, &hdr_bmp, &p_bits))
  {
    _get_bmp_from_rsc(&hdr_bmp, p_bits, &bmp_info);
    gui_get_posi(p_rc, style, x_offset, y_offset, (u16)(bmp_info.width),
                 (u16)(bmp_info.height), &left, &top);
    set_rect(&rc_out, left, top, (s16)(left + bmp_info.width),
             (s16)(top + bmp_info.height));
    c_bmp = gdi_create_compatible_bitmap(hdc, &bmp_info);
    gdi_fill_bitmap(hdc, &rc_out, (hbitmap_t)c_bmp, 0);
  }
//  rsc_unlock_data(RSC_TYPE_BITMAP);
}

u16 gui_get_bmp_attr(u32 bmp_data, BOOL is_rawdata, u16 *bmp_width, u16 *bmp_height)

{
  rsc_bitmap_t hdr_bmp;
  bitmap_t bmp_info = {0};
  u8 *p_bits = NULL;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if((!is_rawdata && (bmp_data == RSC_INVALID_ID))
    || (is_rawdata && ((void*)bmp_data == NULL)))
  {
    return 0;
  }

  if(!is_rawdata && rsc_get_bmp(p_info->rsc_handle, bmp_data, &hdr_bmp, &p_bits))
  {
    _get_bmp_from_rsc(&hdr_bmp, p_bits, &bmp_info);
    *bmp_width = bmp_info.width;
    *bmp_height = bmp_info.height;
  }
  else if(is_rawdata)
  {
    memcpy(&bmp_info, (void*)bmp_data, sizeof(bitmap_t));
    
    *bmp_width = bmp_info.width;
    *bmp_height = bmp_info.height;
  }

  return 0; 
//  rsc_unlock_data(RSC_TYPE_BITMAP);
}

u16 gui_draw_picture_offset(hdc_t hdc, rect_t *p_rc,
    u32 bmp_data, BOOL is_rawdata, u16 bmp_drawed, u16 bmp_width, u16 bmp_height, s16 *p_bmp_offset)

{
  rsc_bitmap_t hdr_bmp;
  bitmap_t bmp_info = {0};
  u8 *p_bits = NULL;
  u16 left = 0, top = 0;
  rect_t rc_out;
  hbitmap_t c_bmp = 0;
  gui_main_t *p_info = NULL;
  BOOL data_ok = FALSE;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if((!is_rawdata && (bmp_data == RSC_INVALID_ID))
    || (is_rawdata && ((void*)bmp_data == NULL)))
  {
    return 0;
  }

//  rsc_lock_data(RSC_TYPE_BITMAP);
  if(!is_rawdata && rsc_get_bmp(p_info->rsc_handle, bmp_data, &hdr_bmp, &p_bits))
  {
    _get_bmp_from_rsc(&hdr_bmp, p_bits, &bmp_info);
    data_ok = TRUE;
  }
  else if(is_rawdata)
  {
    memcpy(&bmp_info, (void*)bmp_data, sizeof(bitmap_t));
    data_ok = TRUE;
  }

  if(data_ok)
  {
    left = 0;
    top = 0;

    left = *p_bmp_offset;

    if(RECTHP(p_rc) - bmp_height >= 0)
    {
      top = (RECTHP(p_rc) - bmp_height) / 2;
    }
  
    set_rect(&rc_out, left, top, (s16)(left + bmp_info.width),
             (s16)(top + bmp_info.height));
    c_bmp = gdi_create_compatible_bitmap(hdc, &bmp_info);
    gdi_fill_bitmap(hdc, &rc_out, (hbitmap_t)c_bmp, 0);

    //*p_bmp_offset = -10;
  }

  return 0; 
//  rsc_unlock_data(RSC_TYPE_BITMAP);
}



void gui_draw_picture_ext(hdc_t hdc, s16 x, s16 y, u16 bmp_id)
{
  rsc_bitmap_t hdr_bmp;
  bitmap_t bmp_info = {0};
  u8 *p_bits = NULL;
  rect_t rc;
  hbitmap_t c_bmp = 0;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

//  rsc_lock_data(RSC_TYPE_BITMAP);
  if(rsc_get_bmp(p_info->rsc_handle, bmp_id, &hdr_bmp, &p_bits))
  {
    _get_bmp_from_rsc(&hdr_bmp, p_bits, &bmp_info);
    c_bmp = gdi_create_compatible_bitmap(hdc, &bmp_info);

    set_rect(&rc, x, y, (s16)(x + hdr_bmp.width),
             (s16)(y + hdr_bmp.height));
    gdi_fill_bitmap(hdc, &rc, (hbitmap_t)c_bmp, 0);
  }
//  rsc_unlock_data(RSC_TYPE_BITMAP);
}


void gui_fill_rect(hdc_t hdc, rect_t *p_rc, u32 color)
{
  //...to be added
  gdi_fill_rect(hdc, p_rc, color);
}

u16 gui_draw_char(hdc_t hdc,
                  rsc_fstyle_t *p_fstyle,
                  u16 char_code,
                  u16 left,
                  u16 top)
{
  u8 *p_pal_data = NULL;
  rsc_palette_t hdr_pal = {{0}};
  rect_t r_char = {0};
  bitmap_t char_info = {0};
  pix_type_t format = COLORFORMAT_RGB8BIT;
  rsc_char_info_t info = {0};
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(!rsc_get_char(p_info->rsc_handle, p_fstyle, char_code, &info))
  {
    return left;
  }

  format = gdi_get_screen_pixel_type();
  
  set_rect(&r_char, (s16)left, (s16)top, (s16)(left + info.width),
           (s16)(top + info.height));

  offset_rect(&r_char, info.xoffset, info.yoffset);
    left += info.x_step;

  char_info.format = format;
  char_info.bpp = info.bpp;
  
  if(info.bpp <= 8)
  {
    char_info.pal.cnt = 1 << info.bpp;
    rsc_get_palette(p_info->rsc_handle,
      rsc_get_curn_palette(p_info->rsc_handle), &hdr_pal, &p_pal_data);
    MT_ASSERT(char_info.pal.cnt == hdr_pal.color_num);
    char_info.pal.p_entry = (color_t *)p_pal_data;
  }

  char_info.enable_ckey = TRUE;
  char_info.colorkey = info.ckey;
  char_info.width = info.width;
  char_info.height = info.height;
  char_info.is_font = TRUE;  
  char_info.pitch = info.pitch;
  char_info.p_bits = info.p_char;

  if(info.is_alpha_spt)
  {
    char_info.p_alpha = info.p_alpha;
    char_info.color = p_fstyle->color;
    char_info.alpha_pitch = info.alpha_pitch;
    char_info.p_strok_alpha = info.p_strok_alpha;
    char_info.p_strok_char = info.p_strok_char;
  }
  else
  {
    char_info.p_alpha = NULL;
    char_info.alpha_pitch = 0;
    char_info.p_strok_alpha = NULL;
    char_info.p_strok_char = NULL;
  }  

#ifdef PAINT_DEBUG
  gui_dump_bmp(&char_info);
#endif
  gdi_fill_bitmap(hdc, &r_char, (hbitmap_t)&char_info, 0);
  return left;
}

u16 gui_draw_unistr_offset(hdc_t hdc, rect_t *p_rc,
  u16 *p_str, u32 f_style_idx, u32 draw_style,
  u16 str_drawed, u16 str_width, u16 str_height, s16 *p_str_offset)
{
  s16 left = 0, top = 0, left_tmp = 0;
  u16 width = 0, height = 0;
  rsc_fstyle_t *p_fstyle = NULL;
  u16 *p_tmp_str = NULL;
  u16 line_width = 0, line_height = 0;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(NULL == p_str)
  {
    return 0;
  }


  p_fstyle = rsc_get_fstyle(p_info->rsc_handle, f_style_idx);
  normalize_rect(p_rc);

  //rsc_get_string_attr(p_info->rsc_handle, p_str, p_fstyle, &line_width, &line_height);
  line_width = str_width;
  line_height = str_height;

  if(RECTHP(p_rc) < line_height)
  {
    return 0;
  }

  left = *p_str_offset;
  
  top = (RECTHP(p_rc) - line_height) / 2;

  p_tmp_str = p_str + str_drawed;
  
  while(*p_tmp_str != 0)
  {
    width = 0;
    height = 0;
    
    rsc_get_char_attr(p_info->rsc_handle, *p_tmp_str, p_fstyle, &width, &height);

    left_tmp = gui_draw_char(hdc, p_fstyle, *p_tmp_str, left, top);
    
    if(left + width > RECTWP(p_rc))
    {
      *p_str_offset = left - RECTWP(p_rc); 
      break;
    }

    left = left_tmp;
    p_tmp_str++;    
  }

  return (p_tmp_str - p_str);  
}


static BOOL gui_get_string_dir(u16 uni_code)
{
  BOOL dwDir = BIDI_LEFT;

  if (uni_code == 0x200C || uni_code == 0x200D)
  {
    dwDir = BIDI_RIGHT;
  }
  else if ((0x0600 <= uni_code && uni_code < 0x0660)   /* Arabic */
           || (0x0669 < uni_code && uni_code <= 0x06FE)
           || (0x0750 <= uni_code && uni_code <= 0x076D)
           || (0xFB50 <= uni_code && uni_code <= 0xFDFC)
           || (0xFE70 <= uni_code && uni_code <= 0xFEFC))
  {
    dwDir = BIDI_RIGHT;
  }
  else if((0x05d0 <= uni_code) && (uni_code <= 0x05ea))/* Hebrew */
  {
    dwDir = BIDI_RIGHT;
  }
  else
  {
    dwDir = BIDI_LEFT;
  }

  return dwDir;
}


static  u16 get_left_str_attr(u16 *p_uni_code_str, u16 *p_end, handle_t *p_handle, 
  rsc_fstyle_t *p_fstyle, u16 *p_width, BOOL is_arabic_blank)
{
  u16 len = 0;
  u16 i = 0;
  u16 temp_width = 0, temp_height = 0;

  *p_width = 0;

  while((0 != p_uni_code_str[i]) && (p_uni_code_str + i < p_end))
  {
    if(0x000a == p_uni_code_str[i])
    {
      break;
    }

    if((0x000d == p_uni_code_str[i]) && (0x000a == p_uni_code_str[i]))
    {
      break;
    }
      
    if(gui_get_string_dir(p_uni_code_str[i]) != BIDI_RIGHT)
    {

      len++;
      rsc_get_char_attr(p_handle, p_uni_code_str[i], p_fstyle, &temp_width, &temp_height);
      
      *p_width = *p_width + temp_width;
    }
    else
    {
      break;
    }
    
    i++;
  }

  return len;
}


static  u16 get_right_str_attr(u16 *p_uni_code_str, u16 *p_end, handle_t *p_handle,
  rsc_fstyle_t *p_fstyle, u16 *p_width, BOOL is_arabic_blank)
{
  u16 len = 0;
  u16 i = 0;
  u16 temp_width = 0,temp_height = 0;

  *p_width = 0;

  while((0 != p_uni_code_str[i]) && (p_uni_code_str + i < p_end))
  {
    if(0x000a == p_uni_code_str[i])
    {
      break;
    }

    if((0x000d == p_uni_code_str[i]) && (0x000a == p_uni_code_str[i]))
    {
      break;
    }
    
    
    if((gui_get_string_dir(p_uni_code_str[i]) == BIDI_RIGHT) || (p_uni_code_str[i]== 0x0020))
    {
      len ++;
      rsc_get_char_attr(p_handle, p_uni_code_str[i], p_fstyle, &temp_width, &temp_height);
      *p_width = *p_width + temp_width;
    }
    else
    {
      break;
    }
    i++;
  }

  return len;
}

//return how many characters of the word.
static u16 gui_get_word(rsc_fstyle_t *p_fstyle, u16 *p_input, u16 *p_width, u16 *p_height)
{
  gui_main_t *p_info = NULL;
  u16 i = 0, char_width = 0, char_height = 0, char_code = 0;
  
  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  *p_width = 0;
  *p_height = 0;
  
  while(1)
  {
    char_code = *(p_input + i);

    if(char_code == 0x0020)//is blank, means the end of a word.
    {
      break;
    }
    else if(char_code == 0)//end of the string.
    {
      break;
    }
    else if(char_code == 0x000a)//end of the ling.
    {
      break;
    }
    else if((char_code == 0x000d) && (*(p_input + i + 1) == 0x000a))
    {
      break;
    }
    
    rsc_get_char_attr(p_info->rsc_handle, char_code, p_fstyle, &char_width, &char_height);

    *p_width += char_width;
    *p_height = MAX(*p_height, char_height);

    i++;
  }

  return i;
}

static u8 gui_get_line_data(rsc_fstyle_t *p_fstyle,
                              rect_t *p_rc,
                              u32 style,
                              u16 x_offset,
                              u16 y_offset,
                              u8 line_space,
                              u16 *p_str,
                              u16 *p_top,
                              u32 *p_status,
                              u32 draw_style)
{
  gui_main_t *p_info = NULL;
  u16 area_width = 0, area_height = 0;
  u16 remn_width = 0, remn_height = 0;
  u16 char_width = 0, char_height = 0;
  u16 word_width = 0, word_height = 0;
  u16 strdraw_style = 0, strdraw_data = 0;
  u16 lines = 0, char_code = 0, i = 0, chars = 0;
  u16 str_len = 0, tot_height = 0;
  BOOL is_out = FALSE, is_start_with_word = TRUE;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  strdraw_style = GET_DRAW_STYLE(draw_style);
  strdraw_data = GET_DRAW_DATA(draw_style);  
  
  //caculate area width & area height.
  if((style & STL_LEFT) || (style & STL_RIGHT))
  {
    if(RECTWP(p_rc) <= x_offset)
    {
      //too narrow for draw.
      return 0;
    }
    area_width = RECTWP(p_rc)- x_offset;
  }
  else
  {
    if(RECTWP(p_rc) <= (2 * x_offset))
    {
      //too narrow for draw.
      return 0;
    }  
    area_width = RECTWP(p_rc)- x_offset * 2;
  }

  if((style & STL_TOP) || (style & STL_BOTTOM))
  {
    if(RECTHP(p_rc) <= y_offset)
    {
      //too narrow for draw.
      return 0;
    }    
    area_height = RECTHP(p_rc)- y_offset;
  }
  else
  {
    if(RECTHP(p_rc) <= (2 * y_offset))
    {
      //too narrow for draw.
      return 0;
    }    
    area_height = RECTHP(p_rc) - y_offset * 2;
  }    

  str_len = uni_strlen(p_str);
  lines = 0;
  remn_height = area_height + line_space;
  remn_width = area_width;
  p_info->p_lineh[lines] = 0;
  p_info->p_linew[lines] = 0;
  p_info->pp_linep[lines] = p_str;
  p_info->p_lines[lines] = 0;  
  
  //caculate lines.
  i = 0;
  while(i < str_len)
  {
    //caculate the line.
    char_code = *(p_str + i);

    if(char_code == 0x000a)
    {
      //skip it.
      i++;
      
      //new line.
      remn_width = area_width;
      remn_height -= (p_info->p_lineh[lines] + line_space);
      lines++;
      if(lines >= p_info->line_str_max_lines)
      {
        //max lines, stop caculate.
        break;
      }
      
      p_info->pp_linep[lines] = p_info->pp_linep[lines - 1] + p_info->p_lines[lines - 1] + 1;
      p_info->p_lineh[lines] = 0;
      p_info->p_linew[lines] = 0;
      p_info->p_lines[lines] = 0;
    }
    else if((char_code == 0x000d) && (*(p_str + i + 1) == 0x000a))
    {
      //skip it.
      i += 2;

      //new line.
      remn_width = area_width;
      remn_height -= (p_info->p_lineh[lines] + line_space);
      lines++;
      if(lines >= p_info->line_str_max_lines)
      {
        //max lines, stop caculate.
        break;
      }
      
      p_info->pp_linep[lines] = p_info->pp_linep[lines - 1] + p_info->p_lines[lines - 1] + 2;
      p_info->p_lineh[lines] = 0;
      p_info->p_linew[lines] = 0;
      p_info->p_lines[lines] = 0;         
    }
    else
    {      
      rsc_get_char_attr(p_info->rsc_handle, char_code, p_fstyle, &char_width, &char_height);

      if((remn_height - line_space) < MAX(p_info->p_lineh[lines], char_height))
      {
        //draw end, not enough area in vertical direction. 
        is_out = TRUE;
        break;
      }
      
      if(char_code == 0x0020)//is blank
      {
        is_start_with_word = FALSE;
        
        if(remn_width >= char_width)
        {
          p_info->p_lines[lines]++;
          i++;
          remn_width -= char_width;
          p_info->p_linew[lines] += char_width;
          p_info->p_lineh[lines] = MAX(p_info->p_lineh[lines], char_height);
        }    
        else
        {
          //new line, reset width & height.
          remn_width = area_width;
          remn_height -= (p_info->p_lineh[lines] + line_space);
          lines++;
          if(lines >= p_info->line_str_max_lines)
          {
            //max lines, stop caculate.
            break;
          }

          p_info->pp_linep[lines] = p_info->pp_linep[lines - 1] + p_info->p_lines[lines - 1];
          p_info->p_lines[lines] = 0;              
          p_info->p_lineh[lines] = 0;
          p_info->p_linew[lines] = 0;
        }
      }
      else
      {
        if(is_start_with_word || (strdraw_style & STRDRAW_BREAK_WORD))
        {
          if(remn_width >= char_width)
          {
            p_info->p_lines[lines]++;
            i++;
            remn_width -= char_width;
            p_info->p_linew[lines] += char_width;
            p_info->p_lineh[lines] = MAX(p_info->p_lineh[lines], char_height);
          }    
          else
          {
            //new line, reset width & height.
            remn_width = area_width;
            remn_height -= (p_info->p_lineh[lines] + line_space);
            lines++;
            if(lines >= p_info->line_str_max_lines)
            {
              //max lines, stop caculate.
              break;
            }                

            p_info->pp_linep[lines] = p_info->pp_linep[lines - 1] + p_info->p_lines[lines - 1];
            p_info->p_lineh[lines] = 0;
            p_info->p_linew[lines] = 0;
            p_info->p_lines[lines] = 0;                
          }              
        }
        else
        {
          chars = gui_get_word(p_fstyle,
            p_info->pp_linep[lines] + p_info->p_lines[lines], &word_width, &word_height);

          if((remn_height - line_space) < MAX(p_info->p_lineh[lines], word_height))
          {
            //draw end, not enough area in vertical direction.   
            is_out = TRUE;
            break;
          }

          if(remn_width >= word_width)
          {
            remn_width -= word_width;
            p_info->p_linew[lines] += word_width;
            p_info->p_lines[lines] += chars;
            i += chars;
            p_info->p_lineh[lines] = MAX(p_info->p_lineh[lines], word_height);
          }
          else
          {
            //new line, reset remain width & remain height.
            remn_width = area_width;
            remn_height -= (p_info->p_lineh[lines] + line_space);
            is_start_with_word = TRUE;
            lines++;
            if(lines >= p_info->line_str_max_lines)
            {
              //max lines, stop caculate.
              break;
            }                
            p_info->pp_linep[lines] = p_info->pp_linep[lines - 1] + p_info->p_lines[lines - 1];
            p_info->p_lines[lines] = 0;                
            p_info->p_lineh[lines] = 0;
            p_info->p_linew[lines] = 0;

          }
        }
      }

      if(lines >= p_info->line_str_max_lines)
      {
        //max lines.
        break;
      }
    }
  }

  if(lines < p_info->line_str_max_lines && !is_out)
  {
    lines++;
  }  

  for(i = 0; i < lines; i++)
  {
    tot_height += p_info->p_lineh[i];
  }
  
  if(style & STL_TOP)
  {
    *p_top = p_rc->top + y_offset;
  }
  else if(style & STL_BOTTOM)
  {
    *p_top = p_rc->bottom - y_offset - tot_height - line_space * (lines - 1);
  }
  else
  {
    *p_top = p_rc->top + y_offset + ((area_height - tot_height - line_space * (lines - 1)) / 2);
  }
  
  // how many chars have been processed (include format sign)?
  *p_status = ((((u32)p_info->pp_linep[lines] - (u32)p_str) >> 1) + p_info->p_lines[lines]);

  return lines;
}


u32 gui_draw_unistr(hdc_t hdc,
                    rect_t *p_rc,
                    u32 style,
                    u16 x_offset,
                    u16 y_offset,
                    u8 line_space,
                    u16 *p_str,
                    u32 f_style_idx,
                    u32 draw_style)
{
  rsc_fstyle_t *p_fstyle = NULL;
  gui_main_t *p_info = NULL;
  rsc_fstyle_t temp_fstyle = {0};
  rect_t temp_rc = {0};
  u32 status = 0;
  u16 *p_tmp = NULL;
  u16 strdraw_style = 0, strdraw_data = 0, str_len = 0;
  u16 i = 0, j = 0, k = 0, l = 0;
  u16 left = 0, top = 0, convert_len = 0, old_len = 0;
  u16 temp_height = 0, temp_width = 0, temp_left = 0;
  u16 arabic_str_num = 0, arabic_str_width = 0;
  u16 str_num = 0, str_width = 0, char_code = 0;
  u8 language_style = 0, line = 0;
  BOOL bidi = BIDI_LEFT;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(NULL == p_str)
  {
    return 0;
  }

  p_fstyle = rsc_get_fstyle(p_info->rsc_handle, f_style_idx);
  normalize_rect(p_rc);

  if(p_info->h_pango != NULL)
  {
    return pango_draw_unistr(p_info->h_pango,
      hdc, p_rc, style, x_offset, y_offset, line_space, p_str, p_fstyle, draw_style);
  }
  
  strdraw_style = GET_DRAW_STYLE(draw_style);
  strdraw_data = GET_DRAW_DATA(draw_style);

  //allocate memory for string convert.
  str_len =  uni_strlen(p_str);

  if(str_len > p_info->line_str_max_cnt)
  {
    str_len = p_info->line_str_max_cnt;
  }
  
  memset(p_info->p_str_convert, 0, sizeof(u16) * (str_len + 1));  
  memset(p_info->p_str_base, 0, sizeof(u16) * (str_len + 1));
  memset(p_info->p_char_pos, 0, sizeof(u16) * (str_len + 1)); 

  if((is_arabic_uni_code(p_str[0]) || is_hebrew_uni_code(p_str[0])) && (style & STL_LEFT))
  {
    style = style | STL_RIGHT;
  }
  
  //convert ara string & recaculate strdraw data.
  for(i = 0; i < str_len; i++)
  {
    convert_len = uni_strlen(p_info->p_str_convert);
    
    if(i == strdraw_data)
    {
      strdraw_data = convert_len;
    }

    //remeber the start postion of every word(after convert) in original string.
    if(convert_len != old_len)
    {
      old_len = convert_len;

      *(p_info->p_char_pos + old_len) = i;
    }
    
    arabic_input_char_convert(p_info->p_str_base, p_info->p_str_convert, p_str[i]);
  }

  if(strdraw_data > uni_strlen(p_info->p_str_convert))
  {
    strdraw_data = uni_strlen(p_info->p_str_convert);
  }
  
  //paint
  left = top = 0;
  
  line = gui_get_line_data(p_fstyle,
    p_rc, style, x_offset, y_offset, line_space, p_info->p_str_convert,
    &top, &status, draw_style);
  
  for(i = 0; i < line; i++)
  {
    char_code = *(p_info->pp_linep[i]);
    bidi = gui_get_string_dir(char_code);
     
    if(style & STL_RIGHT)
    {
      if(bidi == BIDI_RIGHT)
      {
        left = p_rc->right - x_offset;
        language_style = 1;
      }
      else
      {
        left = p_rc->right - x_offset - p_info->p_linew[i];
        language_style = 0;
      }
    }
    else if(style & STL_LEFT)
    {
      if(bidi == BIDI_RIGHT)
      {
        left = p_rc->left + x_offset + p_info->p_linew[i];
        language_style = 1;
      }
      else
      {
        left = p_rc->left + x_offset;
        language_style = 0;
      }
    }
    else
    {
      if(bidi == BIDI_RIGHT)
      {
        left = p_rc->left + ((p_rc->right - p_rc->left + p_info->p_linew[i]) >> 1);
        language_style = 1;
      }
      else
      {
        left = p_rc->left + ((p_rc->right - p_rc->left - p_info->p_linew[i]) >> 1);
        language_style = 0;
      }
    }

    j = 0;

    for(j = 0; j < p_info->p_lines[i];)
    {
      char_code = *(p_info->pp_linep[i] + j);
      
      bidi = gui_get_string_dir(char_code);

      p_tmp = p_info->pp_linep[i] + j;

      if((bidi == BIDI_RIGHT) || ((char_code == 0x20) && (language_style == 1)))
      {

        arabic_str_num = get_right_str_attr(p_tmp, p_info->pp_linep[i] + p_info->p_lines[i],
          p_info->rsc_handle, p_fstyle, &arabic_str_width, language_style);
          
        if(language_style == 0)
        {
          left = left + arabic_str_width;
        }

        
        for(l = 0; l < arabic_str_num; l ++)
        {
          char_code = *(p_tmp + l);
          rsc_get_char_attr(p_info->rsc_handle, char_code, p_fstyle, &temp_width, &temp_height);
            
          left = left - temp_width;

          if((strdraw_style & STRDRAW_WITH_INVERT) && (strdraw_data == k))
          {
            temp_rc.left = left;
            temp_rc.top = top;
            temp_rc.right = left + temp_width;
            temp_rc.bottom = top + p_info->p_lineh[i];

            memcpy(&temp_fstyle, p_fstyle, sizeof(rsc_fstyle_t));
            temp_fstyle.color = gdi_get_invert_color(hdc, p_fstyle->color);
            //temp_fstyle.font_id = p_fstyle->font_id;
            //temp_fstyle.style = p_fstyle->style;
              
            gui_fill_rect(hdc, &temp_rc, p_fstyle->color);
            
            left = gui_draw_char(hdc, &temp_fstyle, char_code, left, top);
          }
          else
          {
            left = gui_draw_char(hdc, p_fstyle, char_code, left, top);
          }
          
          left = left - temp_width;

          if((strdraw_style & STRDRAW_WITH_UNDERLINE) && (strdraw_data == k))
          {
            gui_draw_hor_line(hdc,
              left, top + p_info->p_lineh[i] - 0x2, temp_width, p_fstyle->color);
          }   

//          if((strdraw_style & STRDRAW_WITH_UNDERLINE) &&
//            (k == (str_len - 1)) &&
//            (strdraw_data == str_len))
//          {
//            gui_draw_hor_line(hdc, left - 8, top + p_info->p_lineh[i] - 0x2, 8, p_fstyle->color);
//          }          

          j++;
          k++;
        }
        
        if(language_style == 0)
        {
          left = left + arabic_str_width;
        }
      }
      else
      {
        str_num = get_left_str_attr(p_tmp, p_info->pp_linep[i] + p_info->p_lines[i],
          p_info->rsc_handle, p_fstyle ,&str_width, language_style);
          
        if(language_style == 1)
        {
          left = left - str_width;
        }

        for(l = 0; l < str_num; l++)
        {
          char_code = *(p_tmp + l);
          rsc_get_char_attr(p_info->rsc_handle, char_code, p_fstyle, &temp_width, &temp_height);
        
          if((strdraw_style & STRDRAW_WITH_INVERT) && (strdraw_data == k))
          {
            temp_rc.left = left;
            temp_rc.top = top;
            temp_rc.right = left + temp_width;
            temp_rc.bottom = top + p_info->p_lineh[i];

            memcpy(&temp_fstyle, p_fstyle, sizeof(rsc_fstyle_t));
            temp_fstyle.color = gdi_get_invert_color(hdc, p_fstyle->color);
            //temp_fstyle.font_id = p_fstyle->font_id;
            //temp_fstyle.style = p_fstyle->style;

            gui_fill_rect(hdc, &temp_rc, p_fstyle->color);
            
            temp_left = gui_draw_char(hdc, &temp_fstyle, char_code, left, top);
          }
          else
          {
            temp_left = 
              gui_draw_char(hdc, p_fstyle, char_code, left, top);
          }        

          if((strdraw_style & STRDRAW_WITH_UNDERLINE) && (strdraw_data == k))
          {
            gui_draw_hor_line(hdc,
              left, top + p_info->p_lineh[i] - 0x2, temp_width, p_fstyle->color);
          } 

          left = temp_left;

//          if((strdraw_style & STRDRAW_WITH_UNDERLINE) &&
//            (k == (str_len - 1)) &&
//            (strdraw_data == str_len))
//          {
//            gui_draw_hor_line(hdc, left, top + p_info->p_lineh[i] - 0x2, 8, p_fstyle->color);
//          }
          
          
          j++;
          k++;
        }        

        if(language_style == 1)
        {
          left = left - str_width;
        }
      }

    }

    top += p_info->p_lineh[i] + line_space; 
  }
 
  // how many chars has been processed? (include format sign)
  
  return status;
}


u32 gui_draw_strid(hdc_t hdc,
                   rect_t *p_rc,
                   u32 style,
                   u16 x_offset,
                   u16 y_offset,
                   u8 line_space,
                   u16 str_id,
                   u32 f_style_idx,
                   u32 draw_style)
{
  u8 *p_str = NULL;
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(str_id == RSC_INVALID_ID)
  {
    return 0;
  }

  if(!rsc_get_string(p_info->rsc_handle, rsc_get_curn_language(p_info->rsc_handle), str_id, &p_str))
  {
    return 0;
  }

  return gui_draw_unistr(hdc, p_rc, style, x_offset, y_offset, line_space,
                         (u16 *)p_str, f_style_idx, draw_style);
}

u8 gui_get_lines_per_page(u32 f_style_idx,
                           u32 style,
                           rect_t *p_rc,
                           u16 *p_str,
                           u16 x_offset,
                           u16 y_offset,
                           u8 line_space,
                           u32 draw_style)
{
  rsc_fstyle_t *p_fstyle = NULL;
  gui_main_t *p_info = NULL;
  u32 status = 0;
  u16 top = 0;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(NULL == p_str)
  {
    return 0;
  }

  p_fstyle = rsc_get_fstyle(p_info->rsc_handle, f_style_idx);
  normalize_rect(p_rc);
  
  return gui_get_line_data(p_fstyle,
    p_rc, style, x_offset, y_offset, line_space, p_str, &top, &status, draw_style);
}

u16 *gui_get_next_line(u32 f_style_idx,
                           u32 style,
                           rect_t *p_rc,
                           u16 *p_str,
                           u16 x_offset,
                           u32 draw_style)
{
  gui_main_t *p_info = NULL;
  u16 area_width = 0;
  u16 remn_width = 0;
  u16 char_width = 0, char_height = 0;
  u16 word_width = 0, word_height = 0;
  u16 strdraw_style = 0, strdraw_data = 0;
  u16 char_code = 0, i = 0, chars = 0;
  u16 str_len = 0, convert_len = 0, old_len = 0;
  BOOL is_start_with_word = TRUE;
  rsc_fstyle_t *p_fstyle = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  strdraw_style = GET_DRAW_STYLE(draw_style);
  strdraw_data = GET_DRAW_DATA(draw_style);  
  
  //caculate area width & area height.
  if((style & STL_LEFT) || (style & STL_RIGHT))
  {
    if(RECTWP(p_rc) <= x_offset)
    {
      //too narrow for draw.
      return 0;
    }
    area_width = RECTWP(p_rc)- x_offset;
  }
  else
  {
    if(RECTWP(p_rc) <= (2 * x_offset))
    {
      //too narrow for draw.
      return 0;
    }  
    area_width = RECTWP(p_rc)- x_offset * 2;
  }

  p_fstyle = rsc_get_fstyle(p_info->rsc_handle, f_style_idx);  

  str_len = uni_strlen(p_str);

  if(str_len > p_info->line_str_max_cnt)
  {
    str_len = p_info->line_str_max_cnt;
  }  

  memset(p_info->p_str_convert, 0, sizeof(u16) * (str_len + 1));  
  memset(p_info->p_str_base, 0, sizeof(u16) * (str_len + 1));
  memset(p_info->p_char_pos, 0, sizeof(u16) * (str_len + 1)); 

  //convert ara string & recaculate strdraw data.
  for(i = 0; i < str_len; i++)
  {
    convert_len = uni_strlen(p_info->p_str_convert);
    
    //remeber the start postion of every word(after convert) in original string.
    if(convert_len != old_len)
    {
      old_len = convert_len;

      *(p_info->p_char_pos + old_len) = i;
    }
    
    arabic_input_char_convert(p_info->p_str_base, p_info->p_str_convert, p_str[i]);
  }

  remn_width = area_width;

  str_len = uni_strlen(p_info->p_str_convert);
  
  //caculate lines.
  i = 0;
  while(i < str_len)
  {
    //caculate the line.
    char_code = *(p_info->p_str_convert + i);

    if(char_code == 0x000a)
    {
      //skip it.
      i++;
      
      //found new line.
      break;
    }
    else if((char_code == 0x000d) && (*(p_info->p_str_convert + i + 1) == 0x000a))
    {
      //skip it.
      i += 2;

      //found new line.
      break;
    }
    
    rsc_get_char_attr(p_info->rsc_handle, char_code, p_fstyle, &char_width, &char_height);

    if(char_code == 0x0020)//is blank
    {
      is_start_with_word = FALSE;
      
      if(remn_width >= char_width)
      {
        i++;
        remn_width -= char_width;
      }    
      else
      {
        //found new line.
        break;
      }
    }
    else
    {
      if(is_start_with_word || (strdraw_style & STRDRAW_BREAK_WORD))
      {
        if(remn_width >= char_width)
        {
          i++;
          remn_width -= char_width;
        }    
        else
        {
          //found new line.
          break;
        }              
      }
      else
      {
        chars = gui_get_word(p_fstyle, p_info->p_str_convert + i, &word_width, &word_height);

        if(remn_width >= word_width)
        {
          remn_width -= word_width;
          i += chars;
        }
        else
        {
          //found new line.
          break;
        }
      }
    }
  }

  if(i >= str_len)
  {
    return NULL;
  }  
  
  return (p_str + *(p_info->p_char_pos + i));
}  

u16 gui_get_unistr_total_lines(u32 f_style_idx, u32 style,
  rect_t *p_rc, u16 *p_str, u16 x_offset, u32 draw_style)
{
  u16 *p_str_buf = p_str;
  u8 lines = 0;

  if(p_str_buf == NULL)
  {
    return 0;
  }

  while(1)
  {
    p_str_buf = gui_get_next_line(f_style_idx, style, p_rc, p_str_buf, x_offset, draw_style);
    if(p_str_buf == NULL)
    {
      break;
    }

    lines++;
  }

  if(lines != 0)
  {
    lines++;
  }

  return lines;
}


static u32 _get_ctrl_rstyle_id(control_t *p_ctrl)
{
  u32 rid = 0;

  switch(p_ctrl->attr)
  {
    case OBJ_ATTR_ACTIVE:
      if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
      {
        rid = p_ctrl->rstyle.hl_idx;
      }
      else
      {
        rid = p_ctrl->rstyle.show_idx;
      }
      break;
      
    case OBJ_ATTR_HL:
      rid = p_ctrl->rstyle.hl_idx;
      break;
      
    default:   /*OBJ_ATTR_INACTIVE*/
      rid = p_ctrl->rstyle.gray_idx;
      break;
  }
  return rid;
}

static BOOL _paint_its_parent(control_t *p_ctrl, hdc_t own_hdc)
{
  hdc_t hdc = 0;
  u32 rstyle_id = 0;
  control_t *p_parent = p_ctrl;
  rsc_rstyle_t *p_rstyle = NULL;
  rect_t rc, rc_parent;
  BOOL is_found = FALSE;
  gui_main_t *p_info = NULL;
  cliprc_t *p_cliprc = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_ctrl != NULL);

  while(NULL != p_parent->p_parent)
  {
    p_parent = p_parent->p_parent;
    rstyle_id = _get_ctrl_rstyle_id(p_parent);
    p_rstyle = rsc_get_rstyle(p_info->rsc_handle, rstyle_id);

    if (rsc_is_rstyle_has_bg(p_rstyle))
    {
      is_found = TRUE;
      break;
    }
  }

  // found and paint parent's frame
  if (is_found)
  {
    // add the ctrl's boundrect of invrgn to parent's invrgn
    ctrl_get_frame(p_parent, &rc_parent);
    ctrl_client2screen(p_parent, &rc_parent);

#if 0    
    gdi_get_cliprgn_boundrect(&p_ctrl->invrgn_info.crgn, &rc);
    ctrl_client2screen(p_ctrl, &rc);

    if(!intersect_rect(&rc, &rc, &rc_parent))
    {
      return FALSE;
    }

    ctrl_screen2client(p_parent, &rc);
    ctrl_update_invrgn_by_rect(p_parent, TRUE, &rc);
#else
    p_cliprc = p_ctrl->invrgn_info.crgn.p_head;
    while(p_cliprc != NULL)
    {
      copy_rect(&rc, &p_cliprc->rc);
      ctrl_client2screen(p_ctrl, &rc);

      if(intersect_rect(&rc, &rc, &rc_parent))
      {
        ctrl_screen2client(p_parent, &rc);
        ctrl_update_invrgn_by_rect(p_parent, TRUE, &rc);
      }

      p_cliprc = p_cliprc->p_next;
    }
#endif

    // paint parent
    if(!ctrl_is_onshow(p_parent))
    {
      return FALSE;
    }

    hdc = gui_begin_paint(p_parent, own_hdc);
    if(HDC_INVALID == hdc)
    {
      return ERR_FAILURE;
    }

    if(!gdi_is_empty_cliprgn(&p_parent->invrgn_info.crgn))
    {
      if(p_parent->p_paint != NULL)
      {
        p_parent->p_paint(p_parent, hdc);
      }
    }

    p_cliprc = p_ctrl->invrgn_info.crgn.p_head;
    while(p_cliprc != NULL)
    {
      copy_rect(&rc, &p_cliprc->rc);
      ctrl_client2screen(p_ctrl, &rc);

      if(intersect_rect(&rc, &rc, &rc_parent))
      {
        ctrl_screen2client(p_parent, &rc);
        ctrl_update_invrgn_by_rect(p_parent, FALSE, &rc);
      }

      p_cliprc = p_cliprc->p_next;
    }    

    gui_end_paint_ex(p_parent, hdc);

    return TRUE;
  }

  return FALSE;
}


void gui_paint_frame(hdc_t hdc, control_t *p_ctrl)
{
  u32 rstyle_id = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  rect_t rc_out;
  gui_main_t *p_info = NULL;
  u16 ctrl_bg = 0;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_ctrl != NULL);

  rstyle_id = _get_ctrl_rstyle_id(p_ctrl);
  p_rstyle = rsc_get_rstyle(p_info->rsc_handle, rstyle_id);

  // if it need copy its bg and this painting is from the control itself
  // then to find a parent control with the non-R_IGNORE rstyle and paint it as background at first.
  if (rsc_is_rstyle_cpy_bg(p_rstyle)
    && !dc_is_inherit_hdc(hdc))
  {
    _paint_its_parent(p_ctrl, hdc);
  }

  ctrl_get_client_rect(p_ctrl, &rc_out);

  ctrl_bg = ctrl_get_bg(p_ctrl);

  gui_draw_image_rect(hdc, &rc_out, ctrl_bg);  
  
  gui_draw_style_rect(hdc, &rc_out, p_rstyle);
}

static void gui_get_children_cliprgn_bound(control_t *p_ctrl, rect_t *p_vrc)
{
  control_t *p_child = NULL;
  rect_t child_trc = {0}, child_drc = {0};

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_vrc != NULL);
  
  p_child = p_ctrl->p_child;
  while(p_child != NULL)
  {
    gdi_get_cliprgn_boundrect(&p_child->invrgn_info.crgn, &child_drc);
    ctrl_client2screen(p_child, &child_drc);
    
    gui_get_children_cliprgn_bound(p_child, &child_trc);
    if(!is_invalid_rect(&child_trc))
    {
      generate_boundrect(&child_drc, &child_drc, &child_trc);
    }
    p_child = p_child->p_next;
  }

  copy_rect(p_vrc, &child_drc);
}

static void gui_get_cliprgn_bound(control_t *p_ctrl, rect_t *p_vrc)
{
  rect_t trc = {0}, drc = {0};

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_vrc != NULL);

  gdi_get_cliprgn_boundrect(&p_ctrl->invrgn_info.crgn, &drc);
  ctrl_client2screen(p_ctrl, &drc);

  gui_get_children_cliprgn_bound(p_ctrl, &trc);
  if(!is_invalid_rect(&trc))
  {
    generate_boundrect(&drc, &drc, &trc);
  }
  copy_rect(p_vrc, &drc);
}

static void anim_end_cb(u32 context)
{
  hdc_t hdc = (hdc_t)context;

  //OS_PRINTF("anim end cb.\n");
  
  if(hdc == 0)
  {
    return;
  }

  if(!dc_is_anim_hdc(hdc))
  {
    return;
  }

  //OS_PRINTF("release anim dc.\n");
  
  gdi_release_anim_dc(hdc);
}

hdc_t gui_begin_paint(control_t *p_ctrl, hdc_t parent_dc)
{
  control_t *p_parent = NULL;
  rsc_rstyle_t *p_rstyle = NULL;
  hdc_t hdc = 0;
  u32 rstyle_id = 0;
  rect_t src = {0}, vrc = {0};
  gui_main_t *p_info = NULL;
  handle_t anim_handle = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_ctrl != NULL);

  p_parent = ctrl_get_root(p_ctrl);

  ctrl_get_frame(p_ctrl, &src);
  ctrl_client2screen(p_ctrl, &src);

  gui_get_cliprgn_bound(p_ctrl, &vrc);

  hdc = gdi_get_dc(ctrl_is_topmost(p_ctrl), &src, &vrc, cont_get_gcrgn_info(p_parent), parent_dc);
  MT_ASSERT(hdc != HDC_INVALID);

  if(parent_dc == HDC_INVALID)
  {
    rstyle_id = _get_ctrl_rstyle_id(p_ctrl);
    p_rstyle = rsc_get_rstyle(p_info->rsc_handle, rstyle_id);
  }
  
  gdi_select_cliprgn(hdc, &p_ctrl->invrgn_info.crgn);


  if(!dc_is_inherit_hdc(hdc) && (p_info->chip_ic == IC_CONCERTO))
  {
    if((p_info->anim_mode == ANIM_MODE_IN) && (p_info->anim_in != ANIM_IN_NONE))
    {
      if(SUCCESS == gdi_create_anim_surf(hdc))
      {
        //OS_PRINTF("anim in %d\n", p_info->anim_in);
        switch(p_info->anim_in)
        {
          case ANIM_FADE_IN:
            anim_handle = alpha_anim_new(0, 255, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);
            break;

          case ANIM_ZROTATE_IN:
            anim_handle = step_anim_new(STEP_ZROTATE, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);
            break;

          case ANIM_LEFT_IN:
            anim_handle = step_anim_new(STEP_IN_LEFT, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;

          case ANIM_TOP_IN:
            anim_handle = step_anim_new(STEP_IN_TOP, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;

          case ANIM_RIGHT_IN:
            anim_handle = step_anim_new(STEP_IN_RIGHT, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;

          case ANIM_BOTTOM_IN:
            anim_handle = step_anim_new(STEP_IN_BOTTOM, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;          

          default:
            MT_ASSERT(0);
            break;
        }

        anim_set_end_cb(anim_handle, anim_end_cb, (u32)hdc);
        gdi_set_anim(hdc, anim_handle);
      }
    }
    else if((p_info->anim_mode == ANIM_MODE_OUT) && (p_info->anim_out != ANIM_OUT_NONE))
    {
      if(SUCCESS == gdi_create_anim_surf(hdc))
      {
        //OS_PRINTF("anim out %d\n", p_info->anim_out);
        switch(p_info->anim_out)
        {
          case ANIM_FADE_OUT:
            anim_handle = alpha_anim_new(255, 0, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);
            break;

          case ANIM_ZROTATE_OUT:
            anim_handle = step_anim_new(STEP_ZROTATE, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);
            break;

          case ANIM_LEFT_OUT:
            anim_handle = step_anim_new(STEP_OUT_LEFT, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;

          case ANIM_TOP_OUT:
            anim_handle = step_anim_new(STEP_OUT_TOP, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;

          case ANIM_RIGHT_OUT:
            anim_handle = step_anim_new(STEP_OUT_RIGHT, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;

          case ANIM_BOTTOM_OUT:
            anim_handle = step_anim_new(STEP_OUT_BOTTOM, INTERPOLATOR_LINEAR);
            anim_set_start_time(anim_handle, mtos_ticks_get());
            anim_set_duration(anim_handle, 100);
            anim_set_repeat(anim_handle, 1, FALSE);          
            break;          

          default:
            MT_ASSERT(0);
            break;
        }

        anim_set_end_cb(anim_handle, anim_end_cb, (u32)hdc);
        gdi_set_anim(hdc, anim_handle);
      }
    }
  }

  //OS_PRINTF("begin return.\n");
  return hdc;
}


void gui_end_paint(control_t *p_ctrl, hdc_t hdc)
{
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_ctrl != NULL);


  //if the dc is general dc & animation dc, try to do some animations.
  if(dc_is_anim_hdc(hdc) && !dc_is_inherit_hdc(hdc) && (p_info->chip_ic == IC_CONCERTO))
  {
    //dc is animatio dc, don't release it now.
    gdi_start_anim(hdc);
  }
  else
  {
    gdi_release_dc(hdc);
  }
  
  ctrl_empty_invrgn(p_ctrl);
  
}

void gui_end_paint_ex(control_t *p_ctrl, hdc_t hdc)
{
  MT_ASSERT(p_ctrl != NULL);

  gdi_release_dc(hdc);
  //ctrl_empty_invrgn(p_ctrl);
}

void gui_set_anim_mode(anim_mode_t anim_mode)
{
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(anim_mode < ANIM_MODE_MAX);
  
  p_info->anim_mode = anim_mode;
}

void gui_set_anim(anim_in_t anim_in, anim_out_t anim_out)
{
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(anim_in < ANIM_IN_MAX);
  MT_ASSERT(anim_out < ANIM_OUT_MAX);
  
  p_info->anim_in = anim_in;
  p_info->anim_out = anim_out;
}


void gui_paint_init(paint_param_t *p_param)
{
  class_handle_t p_handle = NULL;
  gui_main_t *p_info = NULL;

  MT_ASSERT(p_param != NULL);
  MT_ASSERT(p_param->max_str_len * p_param->max_str_lines != 0);
  MT_ASSERT(p_param->rsc_handle != 0);
  
  p_handle = (void *)mmi_alloc_buf(sizeof(gui_main_t));
  MT_ASSERT(p_handle != NULL);

  memset((void *)p_handle, 0, sizeof(gui_main_t));
  class_register(PAINT_CLASS_ID, p_handle);

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_info->p_str_convert = (u16 *)mmi_alloc_buf((p_param->max_str_len + 1) * sizeof(u16));
  MT_ASSERT(p_info->p_str_convert != NULL);

  p_info->p_str_base = (u16 *)mmi_alloc_buf((p_param->max_str_len + 1) * sizeof(u16));
  MT_ASSERT(p_info->p_str_base != NULL);

  p_info->p_char_pos = (u16 *)mmi_alloc_buf((p_param->max_str_len + 1) * sizeof(u16));
  MT_ASSERT(p_info->p_char_pos != NULL);
  
  p_info->p_linew = (u16 *)mmi_alloc_buf(p_param->max_str_lines * sizeof(u16));
  MT_ASSERT(p_info->p_linew != NULL);
  p_info->p_lineh = (u16 *)mmi_alloc_buf(p_param->max_str_lines * sizeof(u16));
  MT_ASSERT(p_info->p_lineh != NULL);
  p_info->p_lines = (u16 *)mmi_alloc_buf(p_param->max_str_lines * sizeof(u16));
  MT_ASSERT(p_info->p_lines != NULL);
  p_info->pp_linep = (u16 **)mmi_alloc_buf(p_param->max_str_lines * sizeof(u16 *));
  MT_ASSERT(p_info->pp_linep != NULL);
  
  p_info->line_str_max_cnt = p_param->max_str_len;
  p_info->line_str_max_lines = p_param->max_str_lines;
  p_info->rsc_handle = p_param->rsc_handle;
  p_info->h_pango = p_param->h_pango;

#ifndef WIN32
  p_info->chip_ic = hal_get_chip_ic_id();
#else
  p_info->chip_ic = IC_MAGIC;
#endif  

  p_info->anim_mode = ANIM_MODE_NONE;
}


void gui_paint_release(void)
{
  gui_main_t *p_info = NULL;

  p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_info->p_str_base != NULL)
  {
    mmi_free_buf(p_info->p_str_base);
    p_info->p_str_base = NULL;
  }

  if(p_info->p_str_convert != NULL)
  {
    mmi_free_buf(p_info->p_str_convert);
    p_info->p_str_convert = NULL;
  }  

  if(p_info->p_linew != NULL)
  {
    mmi_free_buf(p_info->p_linew);
    p_info->p_linew = NULL;
  }

  if(p_info->pp_linep != NULL)
  {
    mmi_free_buf(p_info->pp_linep);
    p_info->pp_linep = NULL;
  }

  if(p_info->p_lines != NULL)
  {
    mmi_free_buf(p_info->p_lines);
    p_info->p_lines = NULL;
  }

  if(p_info->p_lineh != NULL)
  {
    mmi_free_buf(p_info->p_lineh);
    p_info->p_lineh = NULL;
  }  

  //release global variable.
  mmi_free_buf((void *)p_info);
}
