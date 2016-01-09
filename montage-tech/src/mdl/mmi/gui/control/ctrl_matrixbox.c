/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_list.c
   this file  implement the functions defined in  ctrl_matrixbox.h, also it
   implement some internal used
   function. All these functions are about how to decribe, set and draw a
   matrixbox control.
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

#include "ctrl_matrixbox.h"

/*!
  matrix-box control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;
  /*!
     Total item count
    */
  u16 total;
  /*!
     Focus item index
    */
  u16 focus;
  /*!
     Row number, how many items in one column.
    */
  u16 row;
  /*!
     Column number, how many items in one row.
    */
  u16 col;
  /*!
     Interval between items in horizontal direction.
    */
  u8 h_interval;
  /*!
     Interval between items in vertical direction.
    */
  u8 v_interval;
  /*!
     Left interval of string, the reference is left of item rectangle.
    */
  u8 str_left;
  /*!
     Top interval of string, the reference is top of item rectangle.
    */
  u8 str_top;
  /*!
     Left interval of icon, the reference is left of item rectangle.
    */
  u8 icon_left;
  /*!
     Top interval of icon, the reference is top of item rectangle.
    */
  u8 icon_top;
  /*!
     Item rstyle when item is in gray state.
    */
  u32 item_g_idx;
  /*!
     Item rstyle when item is in normal state.
    */
  u32 item_n_idx;
  /*!
     Item rstyle when item is in focus state.
    */
  u32 item_f_idx;
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
     Item status buffer , 1 bits for every item. 0 is normal, 1 is disable.
    */
  u32 *p_status;
  /*!
     String content buffer.
    */
  u32 *p_content;
  /*!
     Icon buffer.
    */
  u32 *p_icon;
  /*!
    win col
    */
  u16 win_col;
  /*!
    win row
    */
  u16 win_row;
  /*!
    org col
    */
  u16 org_col;
  /*!
    org row
    */
  u16 org_row;      
}ctrl_mbox_t;

static BOOL _mbox_recl_original(control_t *p_ctrl)
{
  ctrl_mbox_t *p_mbox = NULL;
  u16 focus_col = 0, focus_row = 0;
  BOOL ret_boo = FALSE;
  
  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;  

  focus_col = p_mbox->focus % p_mbox->col;
  focus_row = p_mbox->focus / p_mbox->col;

  if(focus_col >= (p_mbox->org_col + p_mbox->win_col))
  {
    p_mbox->org_col += (focus_col - p_mbox->org_col - p_mbox->win_col + 1);
  
    ret_boo = TRUE;
  }

  if(focus_row >= (p_mbox->org_row + p_mbox->win_row))
  {
    p_mbox->org_row += (focus_row - p_mbox->org_row - p_mbox->win_row + 1);
    
    ret_boo = TRUE;
  }

  if(focus_col < p_mbox->org_col)
  {
    p_mbox->org_col = focus_col;
    
    ret_boo = TRUE;
  }
  
  if(focus_row < p_mbox->org_row)
  {
    p_mbox->org_row = focus_row;
    
    ret_boo = TRUE;
  }

  return ret_boo;
  
}

static BOOL _mbox_get_item_rect(control_t *p_ctrl, u16 index, rect_t *p_rect)
{
  ctrl_mbox_t *p_mbox = NULL;
  u16 left = 0, top = 0, height = 0, width = 0;
  s16 x = 0, y = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  if(index >= p_mbox->total)
  {
    return FALSE;
  }
  
  x = index % (p_mbox->col) - p_mbox->org_col;
  y = index / (p_mbox->col) - p_mbox->org_row;

  if((x < 0) || (x >= p_mbox->win_col))
  {
    //out of window
    return FALSE;
  }

  if((y < 0) || (y >= p_mbox->win_row))
  {
    //out of window
    return FALSE;
  }

  height =
    (p_ctrl->mrect.bottom - p_ctrl->mrect.top -
     (p_mbox->win_row - 1) * p_mbox->v_interval) / p_mbox->win_row;
  width =
    (p_ctrl->mrect.right - p_ctrl->mrect.left -
     (p_mbox->win_col - 1) * p_mbox->h_interval) / p_mbox->win_col;

  left = p_ctrl->mrect.left + x * (width + p_mbox->h_interval);
  top = p_ctrl->mrect.top + y * (height + p_mbox->v_interval);

  set_rect(p_rect, left, top, (s16)(left + width), (s16)(top + height));
  return TRUE;
}


static u32 *_mbox_get_content_strbuf(control_t *p_ctrl, u16 item_idx, u32 type)
{
  ctrl_mbox_t *p_mbox = (ctrl_mbox_t *)p_ctrl;

  if((p_ctrl->priv_attr & MBOX_STRTYPE_MASK) != type)
  {
    return NULL;
  }

  if(item_idx >= p_mbox->total)
  {
    return NULL;
  }

  return p_mbox->p_content + item_idx;
}


static void _mbox_set_static_content(control_t *p_ctrl,
                                     u16 item_idx,
                                     u32 type,
                                     u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _mbox_get_content_strbuf(p_ctrl, item_idx, type);
  if(p_temp == NULL)
  {
    return;
  }
  *p_temp = val;
}


static void _mbox_get_style_by_attr(u8 attr,
                                    control_t *p_ctrl,
                                    u16 index,
                                    u32 *p_rstyle_idx,
                                    u32 *p_fstyle_idx,
                                    u16 *p_icon_idx)
{
  u8 item_status = 0;
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  item_status = mbox_get_item_status(p_ctrl, index);

  switch(attr)
  {
    case OBJ_ATTR_HL:
      if(p_mbox->focus == index) //focus
      {
        *p_rstyle_idx = p_mbox->item_f_idx;
        *p_fstyle_idx = p_mbox->f_fstyle;
        *p_icon_idx = mbox_get_focus_icon(p_ctrl, index);
      }
      else
      {
        if(item_status == MBOX_ITEM_NORMAL) //normal
        {
          *p_rstyle_idx = p_mbox->item_n_idx;
          *p_fstyle_idx = p_mbox->n_fstyle;
        }
        else //gray
        {
          *p_rstyle_idx = p_mbox->item_g_idx;
          *p_fstyle_idx = p_mbox->g_fstyle;
        }
        *p_icon_idx = mbox_get_normal_icon(p_ctrl, index);
      }
      break;
    case OBJ_ATTR_INACTIVE:
      // gray
      *p_rstyle_idx = p_mbox->item_g_idx;
      *p_fstyle_idx = p_mbox->g_fstyle;
      *p_icon_idx = mbox_get_normal_icon(p_ctrl, index);
      break;
    default:
      if(item_status == MBOX_ITEM_NORMAL) //normal
      {
        *p_rstyle_idx = p_mbox->item_n_idx;
        *p_fstyle_idx = p_mbox->n_fstyle;
      }
      else //gray
      {
        *p_rstyle_idx = p_mbox->item_g_idx;
        *p_fstyle_idx = p_mbox->g_fstyle;
      }
      *p_icon_idx = mbox_get_normal_icon(p_ctrl, index);
  }
}


static u32 _mbox_set_content_strbuf(control_t *p_ctrl, u16 item_idx)
{
  ctrl_mbox_t *p_mbox = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & MBOX_STRING_MODE))
  {
    return RSC_INVALID_ID;
  }

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  p_temp = p_mbox->p_content + item_idx;

  return *p_temp;
}


static void _mbox_free_str_buf(control_t *p_ctrl)
{
  ctrl_mbox_t *p_mbox = NULL;
  u16 i = 0;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    p_mbox = (ctrl_mbox_t *)p_ctrl;
    for(i = 0; i < p_mbox->total; i++)
    {
      if((p_ctrl->priv_attr & MBOX_STRTYPE_MASK) == MBOX_STRTYPE_UNICODE)
      {
        p_temp = p_mbox->p_content + i;
        ctrl_unistr_free((void *)*p_temp);
        *p_temp = 0;
      }
    }
  }
}


static void _mbox_draw_item(control_t *p_ctrl, hdc_t hdc, u16 index)
{
  u32 color_idx = 0;
  u16 icon_idx = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  u32 content = 0;
  u16 str_number[NUM_STRING_LEN + 1] = {0};
  u8 left = 0, top = 0;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  rect_t temp_rect = {0};

  if(!_mbox_get_item_rect(p_ctrl, index, &temp_rect))
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
  _mbox_get_style_by_attr(attr, p_ctrl, index, &color_idx, &font, &icon_idx);

  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color_idx);
  gui_draw_style_rect(hdc, &temp_rect, p_rstyle);

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    mbox_get_icon_offset(p_ctrl, &left, &top);
    gui_draw_picture(hdc, &temp_rect, (p_ctrl->priv_attr) << 4, left, top,
                     icon_idx);
  }

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    content = _mbox_set_content_strbuf(p_ctrl, index);
    mbox_get_string_offset(p_ctrl, &left, &top);
    switch(p_ctrl->priv_attr & MBOX_STRTYPE_MASK)
    {
      case MBOX_STRTYPE_UNICODE:
      case MBOX_STRTYPE_EXTSTR:
        gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                        left, top, 0, (u16 *)content,
                        font, STRDRAW_NORMAL);
        break;
      case MBOX_STRTYPE_STRID:
        gui_draw_strid(hdc, &temp_rect, p_ctrl->priv_attr,
                       left, top, 0, (u16)content,
                       font, STRDRAW_NORMAL);
        break;
      case MBOX_STRTYPE_DEC:
        convert_i_to_dec_str(str_number, content);
        gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                        left, top, 0, str_number,
                        font, STRDRAW_NORMAL);
        break;
      case MBOX_STRTYPE_HEX:
        convert_i_to_hex_str(str_number, content);
        gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                        left, top, 0, str_number,
                        font, STRDRAW_NORMAL);
        break;
      default:
        MT_ASSERT(0);
    }
  }
}


static void _mbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_mbox_t *p_mbox = NULL;
  u16 i = 0, j = 0;

  gui_paint_frame(hdc, p_ctrl);
  p_mbox = (ctrl_mbox_t *)p_ctrl;

  for(i = 0; i < p_mbox->win_row; i++)
  {
    for(j = 0; j < p_mbox->win_col; j++)
    {
      _mbox_draw_item(p_ctrl, hdc, (i + p_mbox->org_row) * p_mbox->col + j + p_mbox->org_col);
    }
  }
}


static u16 _mbox_get_next_item(control_t *p_ctrl,
                               u16 curn,
                               s16 offset,
                               BOOL is_hori)
{
  u16 z = 0, o = 0;
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  z = curn / p_mbox->col, o = curn % p_mbox->col;

  if(is_hori)
  {
    o = (o + offset + p_mbox->col) % p_mbox->col;
  }
  else
  {
    z = (z + offset + p_mbox->row) % p_mbox->row;
  }

  return z * p_mbox->col + o;
}


static void _mbox_free_ctrl(control_t *p_ctrl)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  _mbox_free_str_buf(p_ctrl);
  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(p_mbox->p_status != NULL)
  {
    mmi_free_buf(p_mbox->p_status);
    p_mbox->p_status = 0;
  }
  if(p_mbox->p_content != NULL)
  {
    mmi_free_buf(p_mbox->p_content);
    p_mbox->p_content = 0;
  }
  if(p_mbox->p_icon != NULL)
  {
    mmi_free_buf(p_mbox->p_icon);
    p_mbox->p_icon = 0;
  }
}


static void _mbox_switch_focus(control_t *p_ctrl,
                               ctrl_mbox_t *p_mbox,
                               u16 new_focus)
{
  u16 old_focus = 0;

  old_focus = p_mbox->focus;
  p_mbox->focus = new_focus;

  if(!_mbox_recl_original(p_ctrl))
  {
    mbox_draw_item_ext(p_ctrl, old_focus, TRUE);
    mbox_draw_item_ext(p_ctrl, new_focus, TRUE);
  }
  else
  {
    //org point changed, repaint all.
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}


static BOOL _mbox_change_focus(control_t *p_ctrl, s16 offset, BOOL is_hori)
{
  ctrl_mbox_t *p_mbox = NULL;
  u16 z = 0;
  BOOL find_ret = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  if((is_hori ? p_mbox->col : p_mbox->row) <= 1)
  {
    return FALSE;
  }

  z = p_mbox->focus;
  find_ret = FALSE;
  do
  {
    z = _mbox_get_next_item(p_ctrl, z, offset, is_hori);
    if(mbox_get_item_status(p_ctrl, z) == MBOX_ITEM_NORMAL)
    {
      if(z != p_mbox->focus)
      {
        find_ret = TRUE;
      }
      break;
    }
  }
  while(z != p_mbox->focus);

  if(find_ret)
  {
    _mbox_switch_focus(p_ctrl, p_mbox, z);
    return TRUE;
  }
  return FALSE;
}


RET_CODE mbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_mbox_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of matrixbox class
  p_default_ctrl->priv_attr = MBOX_STRTYPE_UNICODE |
                              MBOX_STRING_MODE | MBOX_ICON_MODE;;
  p_default_ctrl->p_proc = mbox_class_proc;
  p_default_ctrl->p_paint = _mbox_draw;

  // initalize the default data of matrixbox class
  
  if(ctrl_register_ctrl_class(CTRL_MBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void mbox_set_string_fstyle(control_t *p_ctrl,
                            u32 f_fstyle,
                            u32 n_fstyle,
                            u32 g_fstyle)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  p_mbox->f_fstyle = f_fstyle;
  p_mbox->n_fstyle = n_fstyle;
  p_mbox->g_fstyle = g_fstyle;
}

void mbox_get_string_fstyle(control_t *p_ctrl,
  u32 *p_factive, u32 *p_fhlight, u32 *p_finactive)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_factive != NULL);
  MT_ASSERT(p_fhlight != NULL);
  MT_ASSERT(p_finactive != NULL);
  
  p_mbox = (ctrl_mbox_t *)p_ctrl;

  *p_factive = p_mbox->n_fstyle;
  *p_fhlight = p_mbox->f_fstyle;
  *p_finactive = p_mbox->g_fstyle;
}


u32 *mbox_get_content_str(control_t *p_ctrl, u16 item_idx)
{
  ctrl_mbox_t *p_mbox = (ctrl_mbox_t *)p_ctrl;

   if(item_idx >= p_mbox->total)
  {
    return NULL;
  }

  return p_mbox->p_content + item_idx;
}

void mbox_set_item_rstyle(control_t *p_ctrl,
                          u32 f_rstyle,
                          u32 n_rstyle,
                          u32 g_rstyle)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  p_mbox->item_f_idx = f_rstyle;
  p_mbox->item_n_idx = n_rstyle;
  p_mbox->item_g_idx = g_rstyle;
}

void mbox_get_item_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_rstyle)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rstyle != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  p_rstyle->hl_idx = p_mbox->item_f_idx;
  p_rstyle->show_idx = p_mbox->item_n_idx;
  p_rstyle->gray_idx = p_mbox->item_g_idx;
}


void mbox_set_item_interval(control_t *p_ctrl, u8 h_interval, u8 v_interval)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  p_mbox->h_interval = h_interval;
  p_mbox->v_interval = v_interval;
}

void mbox_get_item_interval(control_t *p_ctrl, u8 *p_h_interval, u8 *p_v_interval)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_h_interval != NULL);
  MT_ASSERT(p_v_interval != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  *p_h_interval = p_mbox->h_interval;
  *p_v_interval = p_mbox->v_interval;
}


void mbox_set_string_offset(control_t *p_ctrl, u8 str_left, u8 str_top)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    p_mbox = (ctrl_mbox_t *)p_ctrl;
    p_mbox->str_left = str_left;
    p_mbox->str_top = str_top;
  }
}


void mbox_set_icon_offset(control_t *p_ctrl, u8 icon_left, u8 icon_top)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    p_mbox = (ctrl_mbox_t *)p_ctrl;
    p_mbox->icon_left = icon_left;
    p_mbox->icon_top = icon_top;
  }
}

void mbox_set_win(control_t *p_ctrl, u16 win_col, u16 win_row)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  MT_ASSERT(win_col <= p_mbox->col);
  MT_ASSERT(win_row <= p_mbox->row);

  p_mbox->win_col = win_col;
  p_mbox->win_row = win_row;

  _mbox_recl_original(p_ctrl);
}

void mbox_set_size(control_t *p_ctrl, u16 col, u16 row, u16 win_col, u16 win_row)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  MT_ASSERT(win_col <= col);
  MT_ASSERT(win_row <= row);

  p_mbox->win_col = win_col;
  p_mbox->win_row = win_row;

  p_mbox->col = col;
  p_mbox->row = row;

  _mbox_recl_original(p_ctrl);
}

void mbox_get_size(control_t *p_ctrl, u16 *p_col, u16 *p_row, u16 *p_win_col, u16 *p_win_row)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_col != NULL);
  MT_ASSERT(p_row != NULL);
  MT_ASSERT(p_win_col != NULL);
  MT_ASSERT(p_win_row != NULL);
  
  p_mbox = (ctrl_mbox_t *)p_ctrl;

  *p_col = p_mbox->col;
  *p_row = p_mbox->row;
  *p_win_col = p_mbox->win_col;
  *p_win_row = p_mbox->win_row;
}

void mbox_get_win(control_t *p_ctrl, u16 *p_win_col, u16 *p_win_row)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  *p_win_col = p_mbox->win_col;
  *p_win_row = p_mbox->win_row;

  return;
}


BOOL mbox_set_count(control_t *p_ctrl, u16 total, u16 col, u16 row)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // free content at first
  _mbox_free_str_buf(p_ctrl);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  p_mbox->total = total;
  p_mbox->col = col;
  p_mbox->row = row;
  p_mbox->win_col = col;
  p_mbox->win_row = row;
  p_mbox->org_col = 0;
  p_mbox->org_row = 0;

  p_mbox->p_status =
    mmi_realloc_buf(p_mbox->p_status, sizeof(u32) * (total / 32 + 1));
  MT_ASSERT(p_mbox->p_status != NULL);

  if(p_mbox->p_status == NULL)
  {
    return FALSE;
  }
  memset(p_mbox->p_status, 0, sizeof(u32) * (total / 32 + 1));

  if(p_ctrl->priv_attr & MBOX_STRING_MODE)
  {
    p_mbox->p_content = mmi_realloc_buf(p_mbox->p_content, sizeof(u32) * total);
    MT_ASSERT(p_mbox->p_content != NULL);

    if(p_mbox->p_content == NULL)
    {
      if(p_mbox->p_status != NULL)
      {
        mmi_free_buf(p_mbox->p_status);
        p_mbox->p_status = 0;
      }
      if(p_mbox->p_content != NULL)
      {
        mmi_free_buf(p_mbox->p_content);
        p_mbox->p_content = 0;
      }
      if(p_mbox->p_icon != NULL)
      {
        mmi_free_buf(p_mbox->p_icon);
        p_mbox->p_icon = 0;
      }
      return FALSE;
    }
    memset(p_mbox->p_content, 0, sizeof(u32) * total);
  }

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    p_mbox->p_icon = mmi_realloc_buf(p_mbox->p_icon, sizeof(u32) * total);
    MT_ASSERT(p_mbox->p_icon != NULL);

    if(p_mbox->p_icon == NULL)
    {
      if(p_mbox->p_status != NULL)
      {
        mmi_free_buf(p_mbox->p_status);
        p_mbox->p_status = 0;
      }
      if(p_mbox->p_content != NULL)
      {
        mmi_free_buf(p_mbox->p_content);
        p_mbox->p_content = 0;
      }
      if(p_mbox->p_icon != NULL)
      {
        mmi_free_buf(p_mbox->p_icon);
        p_mbox->p_icon = 0;
      }
      return FALSE;
    }
    memset(p_mbox->p_icon, 0, sizeof(u32) * total);
  }

  return TRUE;
}


BOOL mbox_get_count(control_t *p_ctrl, u16 *p_total, u16 *p_col, u16 *p_row)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  *p_total = p_mbox->total;
  *p_col = p_mbox->col;
  *p_row = p_mbox->row;

  return TRUE;
}

void mbox_set_focus(control_t *p_ctrl, u16 focus)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(focus >= p_mbox->total)
  {
    return;
  }
  p_mbox->focus = focus;

  _mbox_recl_original(p_ctrl);
}


u16 mbox_get_focus(control_t *p_ctrl)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  return p_mbox->focus;
}


void mbox_get_focus_ex(control_t *p_ctrl, u16 *p_focus, u16 *p_org_col, u16 *p_org_row)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  *p_focus = p_mbox->focus;
  *p_org_col = p_mbox->org_col;
  *p_org_row = p_mbox->org_row;
}

BOOL mbox_set_focus_ex(control_t *p_ctrl, u16 focus, u16 org_col, u16 org_row)
{
  u16 focus_col = 0, focus_row = 0;
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;

  if(focus >= p_mbox->total)
  {
    //invalid parameter.
    return FALSE;
  }

  focus_col = focus % p_mbox->col;
  focus_row = focus / p_mbox->col;


  if((focus_row < org_row) || (focus_row > org_row + p_mbox->win_row))
  {
    //invalid parameter.
    return FALSE;
  }

  if((focus_col < org_col) || (focus_col > org_col + p_mbox->win_col))
  {
    //invalid parameter.
    return FALSE;
  }
  
  p_mbox->focus = focus;

  p_mbox->org_col = org_col;

  p_mbox->org_row = org_row;

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}


BOOL mbox_set_item_status(control_t *p_ctrl, u16 index, u8 status)
{
  ctrl_mbox_t *p_mbox = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(index >= p_mbox->total)
  {
    return FALSE;
  }
  temp = *(p_mbox->p_status + index / 32);
  status &= 0x01;                     //clean useless information
  temp &= ~(0x1 << (index % 32));     //clean old status
  temp |= status << (index % 32);     //set new status
  *(p_mbox->p_status + index / 32) = temp;
  return TRUE;
}


u8 mbox_get_item_status(control_t *p_ctrl, u16 index)
{
  ctrl_mbox_t *p_mbox = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  temp = *(p_mbox->p_status + index / 32);
  temp = (temp >> (index % 32)) & 0x01;
  return (u8)temp;
}


BOOL mbox_set_content_by_unistr(control_t *p_ctrl, u16 item_idx, u16 *p_unistr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _mbox_get_content_strbuf(p_ctrl, item_idx, MBOX_STRTYPE_UNICODE);
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


BOOL mbox_set_content_by_ascstr(control_t *p_ctrl, u16 item_idx, u8 *p_ascstr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _mbox_get_content_strbuf(p_ctrl, item_idx, MBOX_STRTYPE_UNICODE);
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


void mbox_set_content_by_strid(control_t *p_ctrl, u16 item_idx, u16 strid)
{
  _mbox_set_static_content(p_ctrl, item_idx, MBOX_STRTYPE_STRID, (u32)strid);
}


void mbox_set_content_by_extstr(control_t *p_ctrl, u16 item_idx, u32 p_extstr)
{
  _mbox_set_static_content(p_ctrl,
                           item_idx, MBOX_STRTYPE_EXTSTR, (u32)p_extstr);
}


void mbox_set_content_by_hex(control_t *p_ctrl, u16 item_idx, s32 hex)
{
  _mbox_set_static_content(p_ctrl, item_idx, MBOX_STRTYPE_HEX, (u32)hex);
}


void mbox_set_content_by_dec(control_t *p_ctrl, u16 item_idx, s32 dec)
{
  _mbox_set_static_content(p_ctrl, item_idx, MBOX_STRTYPE_DEC, (u32)dec);
}


void mbox_set_content_by_icon(control_t *p_ctrl,
                              u16 item_idx,
                              u16 f_icon_id,
                              u16 n_icon_id)
{
  ctrl_mbox_t *p_mbox = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    p_mbox = (ctrl_mbox_t *)p_ctrl;

    p_temp = p_mbox->p_icon + item_idx;
    *p_temp = (u32)(n_icon_id | (u32)(f_icon_id << 16));
  }
}

void mbox_get_icon(control_t *p_ctrl, u16 item_idx, u16 *p_icon_hlight, u16 *p_icon_normal)
{
  ctrl_mbox_t *p_mbox = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_icon_hlight != NULL);
  MT_ASSERT(p_icon_normal != NULL);

  if(p_ctrl->priv_attr & MBOX_ICON_MODE)
  {
    p_mbox = (ctrl_mbox_t *)p_ctrl;

    p_temp = p_mbox->p_icon + item_idx;

    *p_icon_hlight = (u16)((*p_temp) >> 16);
    *p_icon_normal = (u16)((*p_temp) & 0xFFFF);
  }  
  else
  {
    *p_icon_hlight = 0;
    *p_icon_normal = 0;
  }
}

void mbox_enable_string_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(is_enable)
  {
    if(p_mbox->total > 0)
    {
      CTRL_PRINTF("can NOT enable string mode after _set_count.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr |= MBOX_STRING_MODE;
  }
  else
  {
    if(p_mbox->p_content != 0)
    {
      CTRL_PRINTF("can NOT disable string mode when it has been set enable.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr &= (~MBOX_STRING_MODE);
  }
}


void mbox_enable_icon_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(is_enable)
  {
    if(p_mbox->total > 0)
    {
      CTRL_PRINTF("can NOT enable icon mode after _set_count.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr |= MBOX_ICON_MODE;
  }
  else
  {
    if(p_mbox->p_icon != 0)
    {
      CTRL_PRINTF("can NOT disable icon mode when it has been set enable.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr &= (~MBOX_ICON_MODE);
  }
}


void mbox_set_string_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~MBOX_STR_ALIGN_MASK);          //clean old style
  p_ctrl->priv_attr |= (style & MBOX_STR_ALIGN_MASK);   //set new style
}

u32 mbox_get_string_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & MBOX_STR_ALIGN_MASK);
}

void mbox_set_icon_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~MBOX_ICON_ALIGN_MASK);               //clean old style
  p_ctrl->priv_attr |= ((style >> 4) & MBOX_ICON_ALIGN_MASK); //set new style
}

u32 mbox_get_icon_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & MBOX_ICON_ALIGN_MASK);
}


void mbox_set_content_strtype(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~MBOX_STRTYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & MBOX_STRTYPE_MASK);    //set new type
}


void mbox_get_string_offset(control_t *p_ctrl,
                            u8 *p_left,
                            u8 *p_top)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(!(p_ctrl->priv_attr & MBOX_STRING_MODE))
  {
    *p_left = 0;
    *p_top = 0;
    return;
  }
  else
  {
    *p_left = p_mbox->str_left;
    *p_top = p_mbox->str_top;
  }
}


void mbox_get_icon_offset(control_t *p_ctrl,
                          u8 *p_left,
                          u8 *p_top)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(!(p_ctrl->priv_attr & MBOX_ICON_MODE))
  {
    *p_left = 0;
    *p_top = 0;
    return;
  }
  else
  {
    *p_left = p_mbox->icon_left;
    *p_top = p_mbox->icon_top;
  }
}


u16 mbox_get_focus_icon(control_t *p_ctrl, u16 item_idx)
{
  ctrl_mbox_t *p_mbox = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & MBOX_ICON_MODE))
  {
    return RSC_INVALID_ID;
  }

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  p_temp = p_mbox->p_icon + item_idx;

  return (u16)((*p_temp) >> 16) & 0xFFFF;
}


u16 mbox_get_normal_icon(control_t *p_ctrl, u16 item_idx)
{
  ctrl_mbox_t *p_mbox = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & MBOX_ICON_MODE))
  {
    return RSC_INVALID_ID;
  }

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  p_temp = p_mbox->p_icon + item_idx;

  return (u16)((*p_temp) & 0xFFFF);
}


void mbox_draw_item_ext(control_t *p_ctrl, u16 index, BOOL is_force)
{
  hdc_t hdc = 0;
  ctrl_mbox_t *p_mbox = NULL;
  rect_t temp_rect;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(index >= p_mbox->total)
  {
    return;
  }

  if(is_force)
  {
    _mbox_get_item_rect(p_ctrl, index, &temp_rect);
    ctrl_add_rect_to_invrgn(p_ctrl, &temp_rect);
  }
  hdc = gui_begin_paint(p_ctrl, 0);
  if(HDC_INVALID == hdc)
  {
    return;
  }
  gui_paint_frame(hdc, p_ctrl);
  _mbox_draw_item(p_ctrl, hdc, index);
  gui_end_paint(p_ctrl, hdc);
}


BOOL mbox_set_focus_ext(control_t *p_ctrl, u16 focus)
{
  ctrl_mbox_t *p_mbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_mbox = (ctrl_mbox_t *)p_ctrl;
  if(focus >= p_mbox->total)
  {
    return FALSE;
  }

  _mbox_switch_focus(p_ctrl, p_mbox, focus);
  return TRUE;
}

u32 *mbox_content_strbuf_get(control_t *p_ctrl, u16 item_idx, u32 type)
{
  return _mbox_get_content_strbuf(p_ctrl, item_idx, type);
}


static RET_CODE on_mbox_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _mbox_free_ctrl(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}


static RET_CODE on_mbox_change_focus(control_t *p_ctrl,
                                     u16 msg,
                                     u32 para1,
                                     u32 para2)
{
  s16 offset = 1;
  BOOL is_hori = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  if(msg == MSG_FOCUS_LEFT
    || msg == MSG_FOCUS_RIGHT)
  {
    is_hori = TRUE;
  }

  if(msg == MSG_FOCUS_LEFT
    || msg == MSG_FOCUS_UP)
  {
    offset = -1;
  }

  _mbox_change_focus(p_ctrl, offset, is_hori);
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(mbox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_FOCUS_UP, on_mbox_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_mbox_change_focus)
ON_COMMAND(MSG_FOCUS_LEFT, on_mbox_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_mbox_change_focus)
ON_COMMAND(MSG_DESTROY, on_mbox_destroy)
END_CTRLPROC(mbox_class_proc, ctrl_default_proc)
