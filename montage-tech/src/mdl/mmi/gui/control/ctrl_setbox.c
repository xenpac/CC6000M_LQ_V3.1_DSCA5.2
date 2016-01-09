/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_setbox.c
   this file  implement the functions defined in  ctrl_setbox.h, also it
   implement some internal used function. All these functions are about how to
   decribe, set and draw a pbar control.
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

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_setbox.h"

typedef struct
{
  /*!
    base control.
    */
  control_t base;
  /*!
     Normal r-style of middle block
    */
  u32 n_mid_rstyle;
  /*!
     Highlight style of middle block.
    */
  u32 h_mid_rstyle;
  /*!
     Gray style of middle block.
    */
  u32 g_mid_rstyle;
  /*!
     Font style of item in gary state.
    */
  u32 g_fstyle;
  /*!
     Font style of item in normal state.
    */
  u32 n_fstyle;
  /*!
     Font style of item in focus state.
    */
  u32 f_fstyle;  
  /*!
     Total item numbers.
    */
  u8 total;
  /*!
     Focus position.
    */
  u8 focus;
  /*!
     Left interval of string, the reference is left of item rectangle.
    */
  u8 str_left;
  /*!
     Top interval of string, the reference is top of item rectangle.
    */
  u8 str_top;  
  /*!
     String content buffer.
    */
  u32 *p_content;
  /*!
     Control x-offset buffer.
    */
  u16 *p_left;  
  /*!
     Control width buffer.
    */
  u16 *p_width;  
  /*!
    Setbox item context
    */
  u32 *p_item_context;  
}ctrl_sbox_t;


static u32 *_sbox_get_str_buf(control_t *p_ctrl, u16 item_idx, u32 type)
{
  ctrl_sbox_t *p_sbox = (ctrl_sbox_t *)p_ctrl;

  if((p_ctrl->priv_attr & SBOX_STRTYPE_MASK) != type)
  {
    return NULL;
  }

  if(item_idx >= p_sbox->total)
  {
    return NULL;
  }

  return p_sbox->p_content + item_idx;
}

static u32 _sbox_set_str_buf(control_t *p_ctrl, u16 item_idx)
{
  ctrl_sbox_t *p_sbox = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;
  p_temp = p_sbox->p_content + item_idx;

  return *p_temp;
}

static void _sbox_set_static_content(control_t *p_ctrl,
  u16 item_idx, u32 type, u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _sbox_get_str_buf(p_ctrl, item_idx, type);
  if(p_temp == NULL)
  {
    return;
  }
  *p_temp = val;
}

static BOOL _sbox_get_item_rect(control_t *p_ctrl, u16 index, rect_t *p_rect)
{
  ctrl_sbox_t *p_sbox = NULL;
  u16 left = 0, top = 0, height = 0, width = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  if(index >= p_sbox->total)
  {
    return FALSE;
  }

  left = *(p_sbox->p_left + index);
  top = p_sbox->base.mrect.top;
  height = (p_sbox->base.mrect.bottom - p_sbox->base.mrect.top);
  width = *(p_sbox->p_width + index);

  set_rect(p_rect, left, top, (s16)(left + width), (s16)(top + height));
  return TRUE;
}

static void _sbox_get_style_by_attr(u8 attr, control_t *p_ctrl,
  u16 index, u32 *p_rstyle_idx, u32 *p_fstyle_idx)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  switch(attr)
  {
    case OBJ_ATTR_HL:
      if(p_sbox->focus == index) //focus
      {
        *p_rstyle_idx = p_sbox->h_mid_rstyle;
        *p_fstyle_idx = p_sbox->f_fstyle;
      }
      else
      {
        *p_rstyle_idx = p_sbox->n_mid_rstyle;
        *p_fstyle_idx = p_sbox->n_fstyle;
      }
      break;
    case OBJ_ATTR_INACTIVE:
      // gray
      *p_rstyle_idx = p_sbox->g_mid_rstyle;
      *p_fstyle_idx = p_sbox->g_fstyle;
      break;
    default:
      *p_rstyle_idx = p_sbox->n_mid_rstyle;
      *p_fstyle_idx = p_sbox->n_fstyle;
      break;
  }
}

static void _sbox_draw_item(control_t *p_ctrl, hdc_t hdc, u16 index)
{
  u32 color_idx = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  u32 content = 0;
  u16 str_number[NUM_STRING_LEN + 1] = {0};
  u16 left = 0, top = 0;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  rect_t temp_rect = {0};


  if(!_sbox_get_item_rect(p_ctrl, index, &temp_rect))
  {
    return;
  }
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }
  _sbox_get_style_by_attr(attr, p_ctrl, index, &color_idx, &font);

  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color_idx);
  gui_draw_style_rect(hdc, &temp_rect, p_rstyle);

  content = _sbox_set_str_buf(p_ctrl, index);
  sbox_get_string_offset(p_ctrl, &left, &top);
  switch(p_ctrl->priv_attr & SBOX_STRTYPE_MASK)
  {
    case SBOX_STRTYPE_UNICODE:
    case SBOX_STRTYPE_EXTSTR:
      gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                      left, top, 0, (u16 *)content,
                      font, STRDRAW_NORMAL);
      break;
    case SBOX_STRTYPE_STRID:
      gui_draw_strid(hdc, &temp_rect, p_ctrl->priv_attr,
                     left, top, 0, (u16)content,
                     font, STRDRAW_NORMAL);
      break;
    case SBOX_STRTYPE_DEC:
      convert_i_to_dec_str(str_number, content);
      gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                      left, top, 0, str_number,
                      font, STRDRAW_NORMAL);
      break;
    case SBOX_STRTYPE_HEX:
      convert_i_to_hex_str(str_number, content);
      gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                      left, top, 0, str_number,
                      font, STRDRAW_NORMAL);
      break;
    default:
      MT_ASSERT(0);
  }
}


static void _sbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_sbox_t *p_sbox = NULL;
  u16 i = 0;

  gui_paint_frame(hdc, p_ctrl);
  p_sbox = (ctrl_sbox_t *)p_ctrl;

  for(i = 0; i < p_sbox->total; i++)
  {
    _sbox_draw_item(p_ctrl, hdc, i);
  }
}

static void _sbox_free_str_buf(control_t *p_ctrl)
{
  ctrl_sbox_t *p_sbox = NULL;
  u16 i = 0;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  for(i = 0; i < p_sbox->total; i++)
  {
    if((p_ctrl->priv_attr & SBOX_STRTYPE_MASK) == SBOX_STRTYPE_UNICODE)
    {
      p_temp = p_sbox->p_content + i;
      ctrl_unistr_free((void *)*p_temp);
      *p_temp = 0;
    }
  }
}

static void _sbox_free_ctrl(control_t *p_ctrl)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  _sbox_free_str_buf(p_ctrl);
  p_sbox = (ctrl_sbox_t *)p_ctrl;
  if(p_sbox->p_left != NULL)
  {
    mmi_free_buf(p_sbox->p_left);
    p_sbox->p_left = 0;
  }
  if(p_sbox->p_content != NULL)
  {
    mmi_free_buf(p_sbox->p_content);
    p_sbox->p_content = 0;
  }
  if(p_sbox->p_width != NULL)
  {
    mmi_free_buf(p_sbox->p_width);
    p_sbox->p_width = 0;
  }
  if(p_sbox->p_item_context != NULL)
  {
    mmi_free_buf(p_sbox->p_item_context);
    p_sbox->p_item_context = 0;
  }
}

RET_CODE sbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_sbox_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of setbox class
  p_default_ctrl->p_proc = sbox_class_proc;
  p_default_ctrl->p_paint = _sbox_draw;

  // initalize the default data of setbox class
  
  if(ctrl_register_ctrl_class(CTRL_SBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}

void sbox_set_mid_rstyle(control_t *p_ctrl,
  u32 n_mid_rstyle, u32 h_mid_rstyle, u32 g_mid_rstyle)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  p_sbox->n_mid_rstyle = n_mid_rstyle;
  p_sbox->h_mid_rstyle = h_mid_rstyle;
  p_sbox->g_mid_rstyle = g_mid_rstyle;
}

void sbox_get_mid_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_rstyle)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rstyle != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  p_rstyle->gray_idx = p_sbox->g_mid_rstyle;
  p_rstyle->hl_idx = p_sbox->h_mid_rstyle;
  p_rstyle->show_idx = p_sbox->n_mid_rstyle;
}

BOOL sbox_set_count(control_t *p_ctrl, u8 total)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // free content at first
  _sbox_free_str_buf(p_ctrl);
  _sbox_free_ctrl(p_ctrl);

  p_sbox = (ctrl_sbox_t *)p_ctrl;
  p_sbox->total = total;

  if(total == 0)
  {
    return FALSE;
  }

  //left
  p_sbox->p_left = mmi_realloc_buf(p_sbox->p_left, sizeof(u16) * total);
  MT_ASSERT(p_sbox->p_left != NULL);

  if(p_sbox->p_left == NULL)
  {
    return FALSE;
  }
  memset(p_sbox->p_left, 0, sizeof(u16) * total);

  //width
  p_sbox->p_width = mmi_realloc_buf(p_sbox->p_width, sizeof(u16) * total);
  MT_ASSERT(p_sbox->p_width != NULL);

  if(p_sbox->p_width == NULL)
  {
    return FALSE;
  }
  memset(p_sbox->p_width , 0, sizeof(u16) * total);

  //context
  p_sbox->p_item_context = mmi_realloc_buf(p_sbox->p_item_context, sizeof(u32) * total);
  MT_ASSERT(p_sbox->p_item_context != NULL);

  if(p_sbox->p_item_context == NULL)
  {
    return FALSE;
  }
  memset(p_sbox->p_item_context , 0, sizeof(u32) * total);

  //alloc content
  p_sbox->p_content = mmi_realloc_buf(p_sbox->p_content, sizeof(u32) * total);
  MT_ASSERT(p_sbox->p_content != NULL);

  if(p_sbox->p_content == NULL)
  {
    //release left buf
    if(p_sbox->p_left != NULL)
    {
      mmi_free_buf(p_sbox->p_left);
      p_sbox->p_left = 0;
    }

    //release content buf
    if(p_sbox->p_content != NULL)
    {
      mmi_free_buf(p_sbox->p_content);
      p_sbox->p_content = 0;
    }

    //release width buf
    if(p_sbox->p_width != NULL)
    {
      mmi_free_buf(p_sbox->p_width);
      p_sbox->p_width = 0;
    }

    //release context buf
    if(p_sbox->p_item_context != NULL)
    {
      mmi_free_buf(p_sbox->p_item_context);
      p_sbox->p_item_context = 0;
    }    
    return FALSE;
  }
  
  memset(p_sbox->p_content, 0, sizeof(u32) * total);

  return TRUE;
}

u8 sbox_get_count(control_t *p_ctrl)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  return p_sbox->total;
}

void sbox_set_focus_pos(control_t *p_ctrl, u16 focus)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  p_sbox->focus = focus;
}

u8 sbox_get_focus_pos(control_t *p_ctrl)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  return p_sbox->focus;
}

void sbox_draw_item_ext(control_t *p_ctrl, u16 index, BOOL is_force)
{
  hdc_t hdc = 0;
  ctrl_sbox_t *p_sbox = NULL;
  rect_t temp_rect;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;
  if(index >= p_sbox->total)
  {
    return;
  }

  if(is_force)
  {
    _sbox_get_item_rect(p_ctrl, index, &temp_rect);
    ctrl_add_rect_to_invrgn(p_ctrl, &temp_rect);
  }
  hdc = gui_begin_paint(p_ctrl, 0);
  if(HDC_INVALID == hdc)
  {
    return;
  }
  // draw frame
  gui_paint_frame(hdc, p_ctrl);
  // draw item
  _sbox_draw_item(p_ctrl, hdc, index);
  gui_end_paint(p_ctrl, hdc);
}

BOOL sbox_set_content_by_unistr(control_t *p_ctrl, u16 item_idx, u16 *p_unistr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _sbox_get_str_buf(p_ctrl, item_idx, SBOX_STRTYPE_UNICODE);
  if(p_temp == NULL)
  {
    return FALSE;
  }

  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, uni_strlen(p_unistr));
  if((*p_temp) == 0)
  {
    return FALSE;
  }
  uni_strcpy((u16 *)(*p_temp), p_unistr);

  return TRUE;
}


BOOL sbox_set_content_by_ascstr(control_t *p_ctrl, u16 item_idx, u8 *p_ascstr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _sbox_get_str_buf(p_ctrl, item_idx, SBOX_STRTYPE_UNICODE);
  CTRL_PRINTF("\n%d  %d\n", item_idx, p_temp);
  if(p_temp == NULL)
  {
    return FALSE;
  }

  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, strlen((char *)p_ascstr));
  CTRL_PRINTF("\n%d  %d\n", item_idx, *p_temp);
  if((*p_temp) == 0)
  {
    return FALSE;
  }
  str_asc2uni(p_ascstr, (u16 *)(*p_temp));

  CTRL_PRINTF("\n%d  %d\n", item_idx, *p_temp);
  return TRUE;
}


void sbox_set_content_by_strid(control_t *p_ctrl, u16 item_idx, u16 strid)
{
  _sbox_set_static_content(p_ctrl, item_idx, SBOX_STRTYPE_STRID, (u32)strid);
}


void sbox_set_content_by_extstr(control_t *p_ctrl, u16 item_idx, u32 p_extstr)
{
  _sbox_set_static_content(p_ctrl,
                           item_idx, SBOX_STRTYPE_EXTSTR, (u32)p_extstr);
}


void sbox_set_content_by_hex(control_t *p_ctrl, u16 item_idx, s32 hex)
{
  _sbox_set_static_content(p_ctrl, item_idx, SBOX_STRTYPE_HEX, (u32)hex);
}


void sbox_set_content_by_dec(control_t *p_ctrl, u16 item_idx, s32 dec)
{
  _sbox_set_static_content(p_ctrl, item_idx, SBOX_STRTYPE_DEC, (u32)dec);
}

void sbox_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~SBOX_ALIGN_MASK);          //clean old style
  p_ctrl->priv_attr |= (style & SBOX_ALIGN_MASK);   //set new style
}

u32 sbox_get_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & SBOX_ALIGN_MASK);
}

void sbox_set_content_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~SBOX_STRTYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & SBOX_STRTYPE_MASK);    //set new type
}

u32 sbox_get_item_context(control_t *p_ctrl, u8 item_idx)
{
  ctrl_sbox_t *p_sbox = NULL;
  u32 content = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  if(p_sbox->p_item_context != NULL)
  {
    content = *(p_sbox->p_item_context + item_idx);
  }
  
  return content;
}

void sbox_set_item_context(control_t *p_ctrl, u8 item_idx, u32 context)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;
  *(p_sbox->p_item_context + item_idx) = context;
}

void sbox_get_string_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  *p_left = (u16)p_sbox->str_left;
  *p_top = (u16)p_sbox->str_top;
}

void sbox_set_string_offset(control_t *p_ctrl, u8 str_left, u8 str_top)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;
  p_sbox->str_left = str_left;
  p_sbox->str_top = str_top;
}

void sbox_get_item_status(control_t *p_ctrl,
  u8 item_idx, u16 *p_left, u16 *p_width)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  *p_left = *(p_sbox->p_left + item_idx);
  *p_width = *(p_sbox->p_width + item_idx);
}

void sbox_set_item_status(control_t *p_ctrl,
  u8 item_idx, u16 left, u16 width)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;
  *(p_sbox->p_left + item_idx) = left;
  *(p_sbox->p_width + item_idx) = width;
}

void sbox_set_fstyle(control_t *p_ctrl,
  u32 f_fstyle, u32 n_fstyle, u32 g_fstyle)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  p_sbox->f_fstyle = f_fstyle;
  p_sbox->n_fstyle = n_fstyle;
  p_sbox->g_fstyle = g_fstyle;
}

void sbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive)
{
  ctrl_sbox_t *p_sbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_factive != NULL);
  MT_ASSERT(p_fhlight != NULL);
  MT_ASSERT(p_finactive != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;
  *p_factive = p_sbox->n_fstyle;
  *p_fhlight = p_sbox->f_fstyle;
  *p_finactive = p_sbox->g_fstyle;
  
}

static RET_CODE on_sbox_increase(control_t *p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  ctrl_sbox_t *p_sbox = NULL;
  u8 old_focus = 0, new_focus = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  MT_ASSERT(p_sbox->focus <= p_sbox->total);

  if(p_sbox->total <= 1)
  {
    return ERR_FAILURE;
  }

  old_focus = p_sbox->focus;
  p_sbox->focus++;

  if(p_sbox->focus >= p_sbox->total)
  {
    p_sbox->focus = 0;
  }

  new_focus = p_sbox->focus;
  sbox_draw_item_ext(p_ctrl, old_focus, TRUE);
  sbox_draw_item_ext(p_ctrl, new_focus, TRUE);  

  return SUCCESS;
}

static RET_CODE on_sbox_decrease(control_t *p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  ctrl_sbox_t *p_sbox = NULL;
  u8 old_focus = 0, new_focus = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_sbox = (ctrl_sbox_t *)p_ctrl;

  MT_ASSERT(p_sbox->focus <= p_sbox->total);

  if(p_sbox->total <= 1)
  {
    return ERR_FAILURE;
  }

  old_focus = p_sbox->focus;

  if(p_sbox->focus == 0)
  {
    p_sbox->focus = p_sbox->total - 1;
  }
  else
  {
    p_sbox->focus--;
  }

  new_focus = p_sbox->focus;
  
  sbox_draw_item_ext(p_ctrl, old_focus, TRUE);
  sbox_draw_item_ext(p_ctrl, new_focus, TRUE);  

  return SUCCESS;
}

static RET_CODE on_sbox_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _sbox_free_ctrl(p_ctrl);

  return ERR_NOFEATURE;
}

static RET_CODE on_sbox_created(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  return ERR_NOFEATURE;
}

BEGIN_CTRLPROC(sbox_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_INCREASE, on_sbox_increase)
  ON_COMMAND(MSG_DECREASE, on_sbox_decrease)
  ON_COMMAND(MSG_DESTROY, on_sbox_destroy)
  ON_COMMAND(MSG_CREATED, on_sbox_created)
END_CTRLPROC(sbox_class_proc, ctrl_default_proc)


