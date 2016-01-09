/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_list.c
   this file  implement the functions defined in  ctrl_list.h, also it implement
   some internal used
   function. All these functions are about how to decribe, set and draw a list
   control.
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
#include "gui_roll.h"

#include "ctrl_scrollbar.h"
#include "ctrl_list.h"

/*!
  list control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;
  /*!
     Each list contains a scroo bar control
    */
  control_t *p_scroll_bar;
  /*!
     Items per page
    */
  u16 page;
  /*!
     Totoal item numbers
    */
  u16 total;
  /*!
     Item buffer size, useless in full automatical mode
    */
  u16 size;
  /*!
     Current position
    */
  u16 cur_pos;
  /*!
     Valid positon, useless in full automatical mode
    */
  u16 valid_pos;
  /*!
     Focus positon
    */
  u16 focus;
  /*!
     Item status buffer , 2 bits for every item
     00 is normal , 01 is selected , 10 is disable
     only work when LIST_SELECT_MODE
    */
  u32 *p_status;
  /*!
     Field content buffer
    */
  u32 *p_content;
  /*!
     List field buffer
    */
  list_field_t *p_field;
  /*!
     Interval between items
    */
  u8 interval;
  /*!
     Field number per item
    */
  u8 field_num;
  /*!
    column
    */
  u8 columns;   
  /*!
    is horinzontal list
    */
  BOOL is_hori;    
  /*!
     Color style for field
    */
  list_xstyle_t rstyle;
  /*!
     Callback function for update the content of the list
    */
  list_update_t update_cb;
  /*!
    callback when draw item callback.
    */
  list_draw_item_cb_t draw_item_cb;    
  /*!
    callback when draw field callback.
    */
  list_draw_field_cb_t draw_field_cb;  
  /*!
     The context of update function
    */
  u32 context;  
}ctrl_list_t;

static void _list_get_item_size(control_t *p_ctrl, u16 *p_w, u16 *p_h)
{
  ctrl_list_t *p_list = NULL;
  u8 lines = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  MT_ASSERT(p_list->page >= p_list->columns);
  MT_ASSERT(p_list->page % p_list->columns == 0);

  lines = p_list->page / p_list->columns;

  *p_w = ((p_ctrl->mrect.right - p_ctrl->mrect.left - p_list->interval *
     (p_list->columns - 1)) / p_list->columns);

  *p_h = (p_ctrl->mrect.bottom - p_ctrl->mrect.top - p_list->interval * (lines - 1)) / lines;
}

static void _list_get_item_rect(control_t *p_ctrl,
                                u16 index,
                                rect_t *p_item_rect)
{
  ctrl_list_t *p_list = NULL;
  s16 top = 0, h = 0, left = 0, w = 0;
  u8 lines = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(index < p_list->cur_pos || index > (p_list->cur_pos + p_list->page - 1))
  {
    empty_rect(p_item_rect);
    return;
  }

  MT_ASSERT(p_list->page >= p_list->columns);
  MT_ASSERT(p_list->page % p_list->columns == 0);

  lines = p_list->page / p_list->columns;

  _list_get_item_size(p_ctrl, &w, &h);

  if(p_list->is_hori)
  {
    top = (p_ctrl->mrect.top + ((index - p_list->cur_pos) / p_list->columns) *
      (p_list->interval + h));

    left =  w * ((index - p_list->cur_pos) % p_list->columns) + p_ctrl->mrect.left;
  }
  else
  {
    top = (p_ctrl->mrect.top + ((index - p_list->cur_pos) % lines) *
      (p_list->interval + h));

    left =  w * ((index - p_list->cur_pos) / lines) + p_ctrl->mrect.left;
  }

  set_rect(p_item_rect, left, top, (left + w), (s16)(top + h));
}


void list_get_item_rect(control_t *p_ctrl,
                                u16 index,
                                rect_t *p_item_rect)
{
  ctrl_list_t *p_list = NULL;
  s16 top = 0, h = 0, left = 0, w = 0;
  u8 lines = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(index < p_list->cur_pos || index > (p_list->cur_pos + p_list->page - 1))
  {
    empty_rect(p_item_rect);
    return;
  }

  MT_ASSERT(p_list->page >= p_list->columns);
  MT_ASSERT(p_list->page % p_list->columns == 0);

  lines = p_list->page / p_list->columns;

  _list_get_item_size(p_ctrl, &w, &h);

  if(p_list->is_hori)
  {
    top = (p_ctrl->mrect.top + ((index - p_list->cur_pos) / p_list->columns) *
      (p_list->interval + h));

    left =  w * ((index - p_list->cur_pos) % p_list->columns) + p_ctrl->mrect.left;
  }
  else
  {
    top = (p_ctrl->mrect.top + ((index - p_list->cur_pos) % lines) *
      (p_list->interval + h));

    left =  w * ((index - p_list->cur_pos) / lines) + p_ctrl->mrect.left;
  }

  set_rect(p_item_rect, left, top, (left + w), (s16)(top + h));
}


static BOOL _list_check_valid(control_t *p_ctrl, u16 item_idx)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(item_idx >= p_list->total)
  {
    return FALSE;
  }
  if((item_idx < p_list->valid_pos)
    || (item_idx >= p_list->valid_pos + p_list->size))
  {
    return FALSE;
  }
  return TRUE;
}


static void _list_free_string(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;
  u16 i = 0, j = 0;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  for(i = 0; i < p_list->size; i++)
  {
    for(j = 0; j < p_list->field_num; j++)
    {
      p_field = p_list->p_field + j;
      if((p_field->attr & LISTFIELD_TYPE_MASK) != LISTFIELD_TYPE_UNISTR)
      {
        continue;
      }
      p_temp = p_list->p_content +
               i * p_list->field_num + j;
      ctrl_unistr_free((void *)*p_temp);
      *p_temp = 0;
    }
  }
}


static BOOL _list_calc_curn_pos(control_t *p_ctrl,
                                u16 new_pos,
                                list_focus_mode_t mode)
{
  ctrl_list_t *p_list = NULL;
  u16 shift = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  shift = p_list->focus - p_list->cur_pos;
  p_list->focus = new_pos;

  if(p_list->total <= p_list->page)
  {
    if(p_list->cur_pos == 0)
    {
      return FALSE;
    }
    else  //sobody changed total
    {
      p_list->cur_pos = 0;
      return TRUE;
    }
  }

  if((p_list->focus < p_list->cur_pos)
    || (p_list->focus >= p_list->cur_pos + p_list->page))
  {
    if(p_ctrl->priv_attr & LIST_PAGE_MODE)
    {
      p_list->cur_pos = p_list->focus - p_list->focus % p_list->page;
    }
    else
    {
      switch(mode)
      {
        case LIST_FOCUS_ON_TOP:
          if(p_list->focus + p_list->page - 1 < p_list->total)
          {
            p_list->cur_pos = p_list->focus;
          }
          else
          {
            p_list->cur_pos = p_list->total - p_list->page;
          }
          break;
        case LIST_FOCUS_ON_BOTTOM:
          if(p_list->focus >= p_list->page - 1)
          {
            p_list->cur_pos = p_list->focus + 1 - p_list->page;
          }
          else
          {
            p_list->cur_pos = 0;
          }
          break;
        case LIST_FOCUS_KEEP_POS:
          if(p_list->focus >= shift)
          {
            if(p_list->focus + p_list->page <= p_list->total + shift)
            {
              p_list->cur_pos = p_list->focus - shift;
            }
            else
            {
              p_list->cur_pos = p_list->total - p_list->page;
            }
          }
          else
          {
            p_list->cur_pos = 0;
          }
          break;
        case LIST_FOCUS_JUMP:
          if(p_list->focus + p_list->page - 1 < p_list->total)
          {
            p_list->cur_pos = p_list->focus;
          }
          else
          {
            p_list->cur_pos = p_list->total - p_list->page;
          }
          break;
        default:
          MT_ASSERT(0);
      }
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


static BOOL _list_calc_valid_pos(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(p_list->valid_pos > p_list->cur_pos)
  {
    p_list->valid_pos = p_list->cur_pos;
    return TRUE;
  }
  else if(p_list->valid_pos + p_list->size >= p_list->total)
  {
    return FALSE;
  }
  else if(p_list->valid_pos + p_list->size < p_list->cur_pos + p_list->page)
  {
    p_list->valid_pos = p_list->cur_pos + p_list->page - p_list->size;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


static u32 *_list_get_field_content_buf(control_t *p_ctrl,
                                        u16 item_idx,
                                        u8 field_idx,
                                        u32 type)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;

  p_list = (ctrl_list_t *)p_ctrl;
  if((item_idx < p_list->valid_pos)
    || (item_idx >= (p_list->valid_pos + p_list->size)))
  {
    return NULL;
  }

  if(field_idx >= p_list->field_num)
  {
    return NULL;
  }
  p_field = p_list->p_field + field_idx;

  if((p_field->attr & LISTFIELD_TYPE_MASK) != type)
  {
    return NULL;
  }

  return p_list->p_content +
         (item_idx - p_list->valid_pos) * p_list->field_num + field_idx;
}


static void _list_set_field_content(control_t *p_ctrl,
                                    u16 item_idx,
                                    u8 field_idx,
                                    u32 type,
                                    u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _list_get_field_content_buf(p_ctrl, item_idx, field_idx, type);
  if(p_temp == NULL)
  {
    return;
  }

  *p_temp = (u32)val;
}


static u32 _list_get_style_by_attr(u8 attr,
                                  control_t *p_ctrl,
                                  u8 status,
                                  BOOL is_onfocus,
                                  list_xstyle_t *p_xstyle)
{
  u32 style = 0;

  MT_ASSERT(p_ctrl != NULL);

  switch(attr)
  {
    case OBJ_ATTR_HL:
      if(is_onfocus)
      {
        if((p_ctrl->priv_attr & LIST_SELECT_MODE)
          && (status == LIST_ITEM_SELECTED)   //focus and select
          && (status != LIST_ITEM_DISABLED))  //focus and select and 'not disabled'
        {
          style = p_xstyle->sf_xstyle;
        }
        else if(status == LIST_ITEM_DISABLED) //disabled
        {
          style = p_xstyle->g_xstyle;
        }
        else //focus
        {
          style = p_xstyle->f_xstyle;
        }
      }
      else
      {
        if((p_ctrl->priv_attr & LIST_SELECT_MODE)
          && (status == LIST_ITEM_SELECTED))  //select
        {
          style = p_xstyle->s_xstyle;
        }
        else if(status == LIST_ITEM_DISABLED) //gray
        {
          style = p_xstyle->g_xstyle;
        }
        else //normal
        {
          style = p_xstyle->n_xstyle;
        }
      }
      break;

    case OBJ_ATTR_INACTIVE:
      style = p_xstyle->g_xstyle; //gray
      break;
      
    case OBJ_ATTR_ACTIVE:
      if((p_ctrl->priv_attr & LIST_SELECT_MODE)
        && (status == LIST_ITEM_SELECTED))
      {
        style = p_xstyle->s_xstyle;
      }
      else if(status == LIST_ITEM_DISABLED) //gray
      {
        style = p_xstyle->g_xstyle;
      }
      else //normal
      {
        style = p_xstyle->n_xstyle;
      }
      break;
      
    default:
      if(status == LIST_ITEM_DISABLED) //gray
      {
        style = p_xstyle->g_xstyle;
      }
      else //normal
      {
        style = p_xstyle->n_xstyle;
      }
      break;
  }

  return style;
}

static u32 _list_get_style_id(control_t *p_ctrl,
                             u8 status,
                             BOOL is_onfocus,
                             list_xstyle_t *p_xstyle)
{
  obj_attr_t attr = OBJ_ATTR_ACTIVE;

  MT_ASSERT(p_ctrl != NULL);

  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  return _list_get_style_by_attr(attr, p_ctrl, status, is_onfocus, p_xstyle);
}

static void _list_draw_field_frame(control_t *p_ctrl,
                             hdc_t hdc,
                             u16 item_idx,
                             u8 field_idx,
                             u8 status,
                             rect_t *p_item_rect)
{
  list_field_t *p_field = NULL;
  ctrl_list_t *p_list = NULL;
  u32 rstyle = 0, fstyle = 0;
  u32 content = 0;
  rect_t rc_field;
  rsc_rstyle_t *p_rstyle = NULL;
  BOOL ret_boo = FALSE;
  list_item_status_t temp_status = LIST_ITEM_NORMAL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(field_idx >= p_list->field_num)
  {
    return;
  }

  p_field = p_list->p_field + field_idx;

  if(p_list->draw_field_cb != NULL)
  {
    temp_status = status;

    if(p_list->focus == item_idx)
    {
      if(temp_status == LIST_ITEM_SELECTED)
      {
        temp_status = LIST_ITEM_SELECT_FOCUSED;
      }
      else
      {
        temp_status = LIST_ITEM_FOCUSED;
      }
    }
    
    ret_boo = p_list->draw_field_cb(p_ctrl, temp_status,
      item_idx, field_idx, &rstyle, &fstyle);
  }

  if(!ret_boo)
  {
    rstyle = _list_get_style_id(p_ctrl, status, p_list->focus == item_idx,
                                &p_field->rstyle);
    fstyle = _list_get_style_id(p_ctrl, status, p_list->focus == item_idx,
                                &p_field->fstyle);
  }
  
  content = list_get_field_content(p_ctrl, item_idx, field_idx);

  rc_field = *p_item_rect;
  rc_field.left += p_field->shift_left;
  rc_field.right = rc_field.left + p_field->width;


  // draw field rectangle
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(),rstyle);
  gui_draw_style_rect(hdc, &rc_field, p_rstyle);
}


static void _list_draw_item_frame(control_t *p_ctrl, hdc_t hdc, u16 index)
{
  ctrl_list_t *p_list = NULL;
  rect_t item_rect;
  u8 item_status = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  u32 color_idx = 0;
  BOOL ret_boo = FALSE;
  list_item_status_t temp_status = LIST_ITEM_NORMAL;

  p_list = (ctrl_list_t *)p_ctrl;

  //draw frame without field content.
  p_list = (ctrl_list_t *)p_ctrl;
  _list_get_item_rect(p_ctrl, index, &item_rect);
  item_status = list_get_item_status(p_ctrl, index);

  if(p_list->draw_item_cb != NULL)
  {
    temp_status = item_status;

    if(p_list->focus == index)
    {
      if(temp_status == LIST_ITEM_SELECTED)
      {
        temp_status = LIST_ITEM_SELECT_FOCUSED;
      }
      else
      {
        temp_status = LIST_ITEM_FOCUSED;
      }
    }
    
    ret_boo = p_list->draw_item_cb(p_ctrl, item_status, index, &color_idx);
  }

  if(!ret_boo)
  {
    color_idx = _list_get_style_id(p_ctrl,
      item_status, p_list->focus == index, &p_list->rstyle);
  }
  
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color_idx);
  gui_draw_style_rect(hdc, &item_rect, p_rstyle);
}


static void _list_draw_scroll_bar(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(p_list->p_scroll_bar != NULL)
  {
    ctrl_paint_ctrl(p_list->p_scroll_bar, TRUE);
  }
}


static void _list_draw_field(control_t *p_ctrl,
                             hdc_t hdc,
                             u16 item_idx,
                             u8 field_idx,
                             u8 status,
                             rect_t *p_item_rect)
{
  list_field_t *p_field = NULL;
  ctrl_list_t *p_list = NULL;
  u32 rstyle = 0, fstyle = 0;
  u32 content = 0;
  rect_t rc_field;
  rsc_rstyle_t *p_rstyle = NULL;
  u16 str_number[NUM_STRING_LEN + 1];
  u32 draw_style = 0;
  BOOL ret_boo = FALSE;
  list_item_status_t temp_status = LIST_ITEM_NORMAL;
  
  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(field_idx >= p_list->field_num)
  {
    return;
  }

  p_field = p_list->p_field + field_idx;

  if(p_list->draw_field_cb != NULL)
  {
    temp_status = status;

    if(p_list->focus == item_idx)
    {
      if(temp_status == LIST_ITEM_SELECTED)
      {
        temp_status = LIST_ITEM_SELECT_FOCUSED;
      }
      else
      {
        temp_status = LIST_ITEM_FOCUSED;
      }
    }
    
    ret_boo = p_list->draw_field_cb(p_ctrl, temp_status, 
      item_idx, field_idx, &rstyle, &fstyle);
  }

  if(!ret_boo)
  {
    rstyle = _list_get_style_id(p_ctrl, status, p_list->focus == item_idx,
                                &p_field->rstyle);

    fstyle = _list_get_style_id(p_ctrl, status, p_list->focus == item_idx,
                                &p_field->fstyle);
  }
  
  content = list_get_field_content(p_ctrl, item_idx, field_idx);

  rc_field = *p_item_rect;
  
  rc_field.left += p_field->shift_left;
  rc_field.right = rc_field.left + p_field->width;
  rc_field.top += p_field->shift_top;
  rc_field.bottom = rc_field.top + p_field->height;

  // draw field rectangle
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), rstyle);
  gui_draw_style_rect(hdc, &rc_field, p_rstyle);

  draw_style = MAKE_DRAW_STYLE(STRDRAW_NORMAL | STRDRAW_BREAK_WORD, 0);

  // draw field content
  switch(p_field->attr & LISTFIELD_TYPE_MASK)
  {
    case LISTFIELD_TYPE_UNISTR:
    case LISTFIELD_TYPE_EXTSTR:
      gui_draw_unistr(hdc, &rc_field, p_field->attr, 0, 0, 0,
                      (u16 *)content, fstyle, draw_style);
      break;
    case LISTFIELD_TYPE_STRID:
      gui_draw_strid(hdc, &rc_field, p_field->attr, 0, 0, 0,
                     (u16)content, fstyle, draw_style);
      break;
    case LISTFIELD_TYPE_ICON:
      gui_draw_picture(hdc, &rc_field, p_field->attr, 0, 0, (u16)content);
      break;
    case LISTFIELD_TYPE_DEC:
      convert_i_to_dec_str_ex(str_number, content, p_field->bit_length);
      gui_draw_unistr(hdc, &rc_field, p_field->attr, 0, 0, 0,
                      str_number, fstyle, draw_style);
      break;
    case LISTFIELD_TYPE_HEX:
      convert_i_to_hex_str_ex(str_number, content, p_field->bit_length);
      gui_draw_unistr(hdc, &rc_field, p_field->attr, 0, 0, 0,
                      str_number, fstyle, draw_style);
      break;
  }
}


static void _list_draw_item(control_t *p_ctrl, hdc_t hdc, u16 item_idx)
{
  u8 j = 0;
  rect_t item_rect;
  u8 item_status = 0;
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!_list_check_valid(p_ctrl, item_idx))
  {
    return;
  }
  p_list = (ctrl_list_t *)p_ctrl;
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);
  item_status = list_get_item_status(p_ctrl, item_idx);

  _list_draw_item_frame(p_ctrl, hdc, item_idx);

  for(j = 0; j < p_list->field_num; j++)
  {
    _list_draw_field(p_ctrl, hdc, item_idx, j, item_status, &item_rect);
  }
}


static void _list_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_list_t *p_list = NULL;
  u16 i = 0, j = 0;
  u8 item_status = 0;
  rect_t item_rect = {0};

  gui_paint_frame(hdc, p_ctrl);
  p_list = (ctrl_list_t *)p_ctrl;

  for(i = p_list->cur_pos; i < (p_list->cur_pos + p_list->page); i++)
  {
    if(i < p_list->total)
    {
      _list_draw_item(p_ctrl, hdc, i);
    }
    else
    {
      _list_draw_item_frame(p_ctrl, hdc, i);
      for(j = 0; j < p_list->field_num; j++)
      {
        _list_get_item_rect(p_ctrl, i, &item_rect);
        item_status = list_get_item_status(p_ctrl, i);        
        _list_draw_field_frame(p_ctrl, hdc, i, j, item_status, &item_rect);
      }
    }
  }
}


static BOOL _list_change_focus(control_t *p_ctrl,
                               u16 new_pos,
                               list_focus_mode_t mode,
                               BOOL paint)
{
  u16 old_focus = 0;
  ctrl_list_t *p_list = NULL;
  rect_t temp;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  old_focus = p_list->focus;
  if(_list_calc_curn_pos(p_ctrl, new_pos, mode))
  {
    if(NULL != p_list->p_scroll_bar)
    {
      sbar_set_current_pos(p_list->p_scroll_bar, p_list->cur_pos);
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    if(_list_calc_valid_pos(p_ctrl))
    {
      return TRUE;
    }
    if(paint)
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  else
  {
    if(_list_check_valid(p_ctrl, old_focus))
    {
      if(paint)
      {
        list_draw_item_ext(p_ctrl, old_focus, TRUE);
      }
      else
      {
        _list_get_item_rect(p_ctrl, old_focus, &temp);
        ctrl_add_rect_to_invrgn(p_ctrl, &temp);
      }
    }
    if(paint)
    {
      list_draw_item_ext(p_ctrl, p_list->focus, TRUE);
    }
    else
    {
      _list_get_item_rect(p_ctrl, old_focus, &temp);
      ctrl_add_rect_to_invrgn(p_ctrl, &temp);
    }
  }
  return FALSE;
}


static list_ret_t _list_change_scroll(control_t *p_ctrl, BOOL is_scroll_up)
{
  ctrl_list_t *p_list = NULL;
  s16 new_focus = 0;
  list_focus_mode_t mode = LIST_FOCUS_KEEP_POS;
  BOOL is_reverse = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(p_list->total == 0)
  {
    return LIST_FALSE;
  }

  mode = is_scroll_up ? LIST_FOCUS_ON_TOP : LIST_FOCUS_ON_BOTTOM;
  new_focus = list_get_new_focus(p_ctrl,
                                 is_scroll_up ? -1 : 1,
                                 &is_reverse);
  if(new_focus == -1)
  {
    return LIST_FALSE;
  }

  if(is_reverse)
  {
    mode = is_scroll_up ? LIST_FOCUS_ON_BOTTOM : LIST_FOCUS_ON_TOP;
  }

  if(_list_change_focus(p_ctrl, new_focus, mode, TRUE))
  {
    if(p_list->update_cb != NULL)
    {
      (*p_list->update_cb)(p_ctrl, p_list->valid_pos, p_list->size,
                           p_list->context);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    return LIST_RELOAD_ALL;
  }
  return LIST_SUCCESS;
}

static list_ret_t _list_change_line(control_t *p_ctrl, BOOL is_page_up)
{
  ctrl_list_t *p_list = NULL;
  s16 new_focus = 0;
  BOOL is_reverse = FALSE;
  list_focus_mode_t mode;
  u16 lines = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(p_list->total == 0)
  {
    return LIST_FALSE;
  }

  MT_ASSERT(p_list->page % p_list->columns == 0);

  lines = p_list->page / p_list->columns;    
  
  if(p_list->total <= lines)
  {
    return LIST_FALSE;
  }

  mode = LIST_FOCUS_KEEP_POS;
  new_focus = list_get_new_focus(p_ctrl,
                                 is_page_up ? 0 - lines : lines,
                                 &is_reverse);
  if(new_focus == -1)
  {
    return LIST_FALSE;
  }

  if(_list_change_focus(p_ctrl, new_focus, mode, TRUE))
  {
    if(p_list->update_cb != NULL)
    {
      (*p_list->update_cb)(p_ctrl, p_list->valid_pos, p_list->size,
                           p_list->context);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    return LIST_RELOAD_ALL;
  }
  return LIST_SUCCESS;
}

static list_ret_t _list_change_column(control_t *p_ctrl, BOOL is_page_up)
{
  ctrl_list_t *p_list = NULL;
  s16 new_focus = 0;
  BOOL is_reverse = FALSE;
  list_focus_mode_t mode;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(p_list->total == 0)
  {
    return LIST_FALSE;
  }
  
  if(p_list->total <= p_list->columns)
  {
    return LIST_FALSE;
  }

  mode = LIST_FOCUS_KEEP_POS;
  new_focus = list_get_new_focus(p_ctrl,
                                 is_page_up ? 0 - p_list->columns: p_list->columns,
                                 &is_reverse);
  if(new_focus == -1)
  {
    return LIST_FALSE;
  }

  if(_list_change_focus(p_ctrl, new_focus, mode, TRUE))
  {
    if(p_list->update_cb != NULL)
    {
      (*p_list->update_cb)(p_ctrl, p_list->valid_pos, p_list->size,
                           p_list->context);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    return LIST_RELOAD_ALL;
  }
  return LIST_SUCCESS;
}

static list_ret_t _list_change_page(control_t *p_ctrl, BOOL is_page_up)
{
  ctrl_list_t *p_list = NULL;
  s16 new_focus = 0;
  BOOL is_reverse = FALSE;
  list_focus_mode_t mode;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(p_list->total == 0)
  {
    return LIST_FALSE;
  }

  if(p_list->total <= p_list->page)
  {
    return LIST_FALSE;
  }

  mode = LIST_FOCUS_KEEP_POS;
  new_focus = list_get_new_focus(p_ctrl,
                                 is_page_up ? 0 - p_list->page : p_list->page ,
                                 &is_reverse);
  if(new_focus == -1)
  {
    return LIST_FALSE;
  }

  if(_list_change_focus(p_ctrl, new_focus, mode, TRUE))
  {
    if(p_list->update_cb != NULL)
    {
      (*p_list->update_cb)(p_ctrl, p_list->valid_pos, p_list->size,
                           p_list->context);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    return LIST_RELOAD_ALL;
  }
  return LIST_SUCCESS;
}


static void _list_free_ctrl(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  _list_free_string(p_ctrl);
  p_list = (ctrl_list_t *)p_ctrl;

  if((p_ctrl->priv_attr & LIST_SELECT_MODE) && (p_list->p_status))
  {
    mmi_free_buf(p_list->p_status);
    p_list->p_status = 0;
  }

  if(p_list->p_field != NULL)
  {
    mmi_free_buf(p_list->p_field);
    p_list->p_field = 0;
  }

  if(p_list->p_content != NULL)
  {
    mmi_free_buf(p_list->p_content);
    p_list->p_content = 0;
  }
}


RET_CODE list_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_list_t *p_default_data = NULL;

  register_info.data_size = sizeof(ctrl_list_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_data = (ctrl_list_t *)register_info.p_default_ctrl;
  
  // initialize the default control of list class
  p_default_ctrl->priv_attr = LIST_CYCLE_MODE;
  p_default_ctrl->p_proc = list_class_proc;
  p_default_ctrl->p_paint = _list_draw;

  // initalize the default data of list class
  p_default_data->focus = LIST_INVALID_FOCUS;
  p_default_data->columns = LIST_DEFAULT_COLUMNS;
  p_default_data->page = LIST_DEFAULT_COLUMNS;
  
  if(ctrl_register_ctrl_class(CTRL_LIST, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void list_set_update(control_t *p_ctrl, list_update_t p_cb, u32 context)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  p_list->update_cb = p_cb;
  p_list->context = context;
}

void list_set_draw_item_cb(control_t *p_ctrl, list_draw_item_cb_t p_draw_item_cb)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  
  p_list->draw_item_cb = p_draw_item_cb;
}

void list_set_draw_field_cb(control_t *p_ctrl, list_draw_field_cb_t p_draw_field_cb)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  
  p_list->draw_field_cb = p_draw_field_cb;
}

list_update_t list_get_update(control_t *p_ctrl,  u32 *p_context)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  *p_context = p_list->context;
  return p_list->update_cb;
}


BOOL list_set_count(control_t *p_ctrl, u16 total, u16 page)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  //cannot init 2 times
  //  if((ctrl->priv_attr & LIST_SELECT_MODE) && (p_list->status))
  //  {
  //    return FALSE;
  //  }

  // free content at first
  _list_free_string(p_ctrl);

  p_list->total = total;
  p_list->page = page;

  if(p_list->p_scroll_bar != NULL)
  {
    sbar_set_count(p_list->p_scroll_bar, p_list->page, p_list->total);
  }

  if(p_ctrl->priv_attr & LIST_SELECT_MODE)
  {
    p_list->p_status =
      mmi_realloc_buf(p_list->p_status, sizeof(u32) * (total / 16 + 1));
    MT_ASSERT(p_list->p_status != NULL);

    if(p_list->p_status != NULL)
    {
      memset(p_list->p_status, 0, sizeof(u32) * (total / 16 + 1));
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return TRUE;
  }
}


u16 list_get_count(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  return p_list->total;
}

void list_set_columns(control_t *p_ctrl, u8 columns, BOOL is_hori)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  MT_ASSERT(p_list != NULL);

  MT_ASSERT(columns != 0);

  p_list->columns = columns;

  p_list->is_hori = is_hori;
}


u8 list_get_columns(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  MT_ASSERT(p_list != NULL);
  
  return p_list->columns;
}

BOOL list_is_hori(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  return p_list->is_hori;
}

void list_set_page(control_t *p_ctrl, u16 page)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  p_list->page = page;

  if(p_list->update_cb != NULL)
  {
    (*p_list->update_cb)(p_ctrl, p_list->valid_pos, p_list->size,
                         p_list->context);
  }
}

u16 list_get_page(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  return p_list->page;
}


BOOL list_set_item_status(control_t *p_ctrl, u16 idx, u8 status)
{
  ctrl_list_t *p_list = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }

  p_list = (ctrl_list_t *)p_ctrl;
  if(idx >= p_list->total)
  {
    return FALSE;
  }
  temp = *(p_list->p_status + idx / 16);
  status &= 0x03;                      //clean useless information
  temp &= ~(0x3 << ((idx % 16) * 2));  //clean old status
  temp |= status << ((idx % 16) * 2);  //set new status
  *(p_list->p_status + idx / 16) = temp;
  return TRUE;
}


u8 list_get_item_status(control_t *p_ctrl, u16 idx)
{
  ctrl_list_t *p_list = NULL;
  u32 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return LIST_ITEM_NORMAL;
  }

  p_list = (ctrl_list_t *)p_ctrl;
  if(idx >= p_list->total)
  {
    return LIST_ITEM_NORMAL;
  }
  temp = *(p_list->p_status + idx / 16);
  temp = (temp >> ((idx % 16) * 2)) & 0x03;
  return (u8)temp;
}


void list_set_current_pos(control_t *p_ctrl, u16 cur_pos)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  p_list->cur_pos = cur_pos;

  if(p_list->p_scroll_bar != NULL)
  {
    sbar_set_current_pos(p_list->p_scroll_bar, p_list->cur_pos);
  }
}


u16 list_get_current_pos(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  return p_list->cur_pos;
}


void list_set_valid_pos(control_t *p_ctrl, u16 valid_pos)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  p_list->valid_pos = valid_pos;
}


u16 list_get_valid_pos(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  return p_list->valid_pos;
}


list_ret_t list_set_focus_pos(control_t *p_ctrl, u16 focus)
{
  ctrl_list_t *p_list = NULL;
  u8 status = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(focus >= p_list->total)
  {
    p_list->focus = LIST_INVALID_FOCUS;
    return LIST_FALSE;
  }

  if(p_ctrl->priv_attr & LIST_SELECT_MODE)
  {
    status = list_get_item_status(p_ctrl, focus);
    if(status == LIST_ITEM_DISABLED)
    {
      return LIST_FALSE;
    }
  }

  if(p_list->focus == focus)
  {
    return LIST_SUCCESS;
  }

  if(_list_change_focus(p_ctrl, focus, LIST_FOCUS_JUMP, FALSE))
  {
    if(p_list->update_cb != NULL)
    {
      (*p_list->update_cb)(p_ctrl, p_list->valid_pos, p_list->size,
                           p_list->context);
    }

    gui_roll_reset(p_ctrl);
    
    return LIST_RELOAD_ALL;
  }

  gui_roll_reset(p_ctrl);  

  return LIST_SUCCESS;
}


u16 list_get_focus_pos(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  return p_list->focus;
}

void list_set_field_bit_length(control_t *p_ctrl, u8 field_idx, u8 bit_length)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(field_idx >= p_list->field_num)
  {
    return;
  }
  p_field = p_list->p_field + field_idx;

  if(((p_field->attr & LISTFIELD_TYPE_MASK) != LISTFIELD_TYPE_DEC)
    && ((p_field->attr & LISTFIELD_TYPE_MASK) != LISTFIELD_TYPE_HEX))
  {
    return;
  }
  p_field->bit_length = bit_length;
}


BOOL list_set_field_count(control_t *p_ctrl, u8 field_cnt, u16 size)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
/*
   if(p_list->p_content)  //cannot init 2 times
   {
    return FALSE;
   }
   if(p_list->p_field)  //cannot init 2 times
   {
    return FALSE;
   }
  */
  p_list->field_num = field_cnt;
  p_list->p_field = mmi_realloc_buf(p_list->p_field,
                                    sizeof(list_field_t) * field_cnt);
  MT_ASSERT(p_list->p_field != NULL);

  if(p_list->p_field == NULL)
  {
    return FALSE;
  }
  memset(p_list->p_field, 0, sizeof(list_field_t) * field_cnt);

  // set size
  p_list->size = size;

  p_list->p_content = mmi_realloc_buf(p_list->p_content,
                                      sizeof(u32) * field_cnt * p_list->size);
  MT_ASSERT(p_list->p_content != NULL);

  if(p_list->p_content == NULL)
  {
    mmi_free_buf(p_list->p_field);
    p_list->p_field = 0;
    return FALSE;
  }

  memset(p_list->p_content, 0, sizeof(u32) * field_cnt * p_list->size);

  return TRUE;
}


void list_set_field_attr(control_t *p_ctrl,
                         u8 field_idx,
                         u32 attr,
                         u16 width,
                         u16 left,
                         u16 top)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;
  u16 item_w = 0, item_h = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(field_idx >= p_list->field_num)
  {
    return;
  }

  _list_get_item_size(p_ctrl, &item_w, &item_h);

  if(left + width > item_w)
  {
    MT_ASSERT(0);
  }

  if(top > item_h)
  {
    MT_ASSERT(0);
  }
  
  p_field = p_list->p_field + field_idx;
  p_field->attr = attr;

  p_field->width = width;
  p_field->height = item_h - top;
  
  p_field->shift_left = left;
  p_field->shift_top = top;

  p_field->bit_length = 0;    //auto size
}

void list_set_field_attr2(control_t *p_ctrl,
                         u8 field_idx,
                         u32 attr,
                         u16 width,
                         u16 height,
                         u16 left,
                         u16 top)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;
  u16 item_w = 0, item_h = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(field_idx >= p_list->field_num)
  {
    return;
  }

  _list_get_item_size(p_ctrl, &item_w, &item_h);

  if(left + width > item_w)
  {
    MT_ASSERT(0);
  }

  if(top + height > item_h)
  {
    MT_ASSERT(0);
  }
  
  p_field = p_list->p_field + field_idx;
  p_field->attr = attr;

  p_field->width = width;
  p_field->height = height;
  
  p_field->shift_left = left;
  p_field->shift_top = top;

  p_field->bit_length = 0;    //auto size
}

void list_set_field_font_style(control_t *p_ctrl,
                               u8 field_idx,
                               list_xstyle_t *p_fstyle)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(field_idx >= p_list->field_num)
  {
    return;
  }
  p_field = p_list->p_field + field_idx;

  if(p_fstyle != NULL)
  {
    memcpy(&p_field->fstyle, p_fstyle, sizeof(list_xstyle_t));
  }
}


void list_set_field_rect_style(control_t *p_ctrl,
                               u8 field_idx,
                               list_xstyle_t *p_rstyle)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(field_idx >= p_list->field_num)
  {
    return;
  }
  p_field = p_list->p_field + field_idx;

  if(p_rstyle != NULL)
  {
    memcpy(&p_field->rstyle, p_rstyle, sizeof(list_xstyle_t));
  }
}


BOOL list_set_field_content_by_unistr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u16 *p_unistr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _list_get_field_content_buf(p_ctrl, item_idx, field_idx,
                                       LISTFIELD_TYPE_UNISTR);
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


BOOL list_set_field_content_by_ascstr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u8 *p_ascstr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _list_get_field_content_buf(p_ctrl, item_idx, field_idx,
                                       LISTFIELD_TYPE_UNISTR);
  if(p_temp == NULL)
  {
    return FALSE;
  }

  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, strlen((char *)p_ascstr));
  if((*p_temp) == 0)
  {
    return FALSE;
  }

  str_asc2uni(p_ascstr, (u16 *)(*p_temp));
  return TRUE;
}


void list_set_field_content_by_strid(control_t *p_ctrl,
                                     u16 item_idx,
                                     u8 field_idx,
                                     u16 str_id)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_STRID, (u32)str_id);
}


void list_set_field_content_by_icon(control_t *p_ctrl,
                                    u16 item_idx,
                                    u8 field_idx,
                                    u16 bmp_id)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_ICON, (u32)bmp_id);
}


void list_set_field_content_by_extstr(control_t *p_ctrl,
                                      u16 item_idx,
                                      u8 field_idx,
                                      u16 *p_extstr)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_EXTSTR, (u32)p_extstr);
}


void list_set_field_content_by_dec(control_t *p_ctrl,
                                   u16 item_idx,
                                   u8 field_idx,
                                   s32 dec)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_DEC, (u32)dec);
}


void list_set_field_content_by_hex(control_t *p_ctrl,
                                   u16 item_idx,
                                   u8 field_idx,
                                   s32 hex)
{
  _list_set_field_content(p_ctrl, item_idx, field_idx,
                          LISTFIELD_TYPE_HEX, (u32)hex);
}


u32 list_get_field_content(control_t *p_ctrl, u16 item_idx, u8 field_idx)
{
  ctrl_list_t *p_list = NULL;
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(item_idx >= p_list->total)
  {
    return 0;
  }
  if(field_idx >= p_list->field_num)
  {
    return 0;
  }
  if(item_idx < p_list->valid_pos)
  {
    //		DEBUG_PRINTF("list_get_field_content: ERROR, out of valid range\n");
    return 0;
  }

  p_temp = p_list->p_content +
           (item_idx - p_list->valid_pos) * p_list->field_num + field_idx;
  return *p_temp;
}


void list_set_item_interval(control_t *p_ctrl, u8 interval)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);
  
  p_list = (ctrl_list_t *)p_ctrl;

  p_list->interval = interval;
}

u8 list_get_item_interval(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);
  
  p_list = (ctrl_list_t *)p_ctrl;

  return p_list->interval;
}


void list_set_item_rstyle(control_t *p_ctrl, list_xstyle_t *p_rstyle)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rstyle != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  memcpy(&p_list->rstyle, p_rstyle, sizeof(list_xstyle_t));
}

void list_get_item_rstyle(control_t *p_ctrl, list_xstyle_t *p_rstyle)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rstyle != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  
  memcpy(p_rstyle, &p_list->rstyle, sizeof(list_xstyle_t));
}

void list_set_scrollbar(control_t *p_ctrl, control_t *p_ctrl_bar)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  p_list->p_scroll_bar = p_ctrl_bar;

  if(p_ctrl_bar != NULL)
  {
    sbar_set_count(p_ctrl_bar, p_list->page, p_list->total);
    sbar_set_current_pos(p_ctrl_bar, p_list->cur_pos);
  }
}


control_t *list_get_scrollbar(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  return p_list->p_scroll_bar;
}


//This function should be called before list_set_count(),
//in order to alloc memory.
void list_enable_select_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(is_enable)
  {
    if(p_list->total > 0)
    {
      CTRL_PRINTF("can NOT enable select mode after u call list_set_count.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr |= LIST_SELECT_MODE;
  }
  else
  {
    if(p_list->p_status != 0)
    {
      CTRL_PRINTF("can NOT disable select mode when it has been set enable.\n");
      MT_ASSERT(0);
      return;
    }
    p_ctrl->priv_attr &= ~LIST_SELECT_MODE;
  }
}


void list_set_select_mode(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~LIST_SELECT_MASK);         //clean old type
  p_ctrl->priv_attr |= (type & LIST_SELECT_MASK);   //set new type
}


void list_enable_cycle_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(is_enable)
  {
    p_ctrl->priv_attr |= LIST_CYCLE_MODE;
  }
  else
  {
    p_ctrl->priv_attr &= ~LIST_CYCLE_MODE;
  }
}

BOOL list_is_cycle_mode(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (BOOL)(p_ctrl->priv_attr & LIST_CYCLE_MODE);
}


void list_enable_page_mode(control_t *p_ctrl, BOOL is_enable)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(is_enable)
  {
    p_ctrl->priv_attr |= LIST_PAGE_MODE;
  }
  else
  {
    p_ctrl->priv_attr &= ~LIST_PAGE_MODE;
  }
}

BOOL list_is_page_mode(control_t *p_ctrl)
{

  MT_ASSERT(p_ctrl != NULL);

  return (BOOL)(p_ctrl->priv_attr & LIST_PAGE_MODE);
}


BOOL list_select_item(control_t *p_ctrl, u16 item_idx)
{
  ctrl_list_t *p_list = NULL;
  u8 status = 0;
  u16 first = 0;
  rect_t item_rect;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(item_idx >= p_list->total)
  {
    return FALSE;
  }

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }
  status = list_get_item_status(p_ctrl, item_idx);
  if(status == LIST_ITEM_DISABLED)
  {
    return FALSE;
  }
  else if(status == LIST_ITEM_SELECTED)
  {
    return TRUE;
  }

  if((p_ctrl->priv_attr & LIST_SELECT_MASK) == LIST_SINGLE_SELECT)
  {
    first = list_get_the_first_selected(p_ctrl, 0);
    if(first != LIST_INVALID_FOCUS)
    {
      list_set_item_status(p_ctrl, first, LIST_ITEM_NORMAL);
      _list_get_item_rect(p_ctrl, first, &item_rect);
      ctrl_add_rect_to_invrgn(p_ctrl, &item_rect);
    }
  }

  list_set_item_status(p_ctrl, item_idx, LIST_ITEM_SELECTED);
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);
  ctrl_add_rect_to_invrgn(p_ctrl, &item_rect);

  return TRUE;
}


BOOL list_unselect_item(control_t *p_ctrl, u16 item_idx)
{
  ctrl_list_t *p_list = NULL;
  rect_t item_rect;
  u8 status = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(item_idx >= p_list->total)
  {
    return FALSE;
  }

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }

  status = list_get_item_status(p_ctrl, item_idx);
  if(status != LIST_ITEM_SELECTED)
  {
    return FALSE;
  }

  list_set_item_status(p_ctrl, item_idx, LIST_ITEM_NORMAL);
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);
  ctrl_add_rect_to_invrgn(p_ctrl, &item_rect);

  return TRUE;
}


void list_clear_status(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return;
  }
  p_list = (ctrl_list_t *)p_ctrl;

  memset(p_list->p_status, 0, sizeof(u32) * (p_list->total / 16 + 1));
}


u16 list_get_the_first_selected(control_t *p_ctrl, u16 begin)
{
  ctrl_list_t *p_list = NULL;
  u16 i = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  for(i = begin; i < p_list->total; i++)
  {
    if(list_get_item_status(p_ctrl, i) == LIST_ITEM_SELECTED)
    {
      return i;
    }
  }
  return LIST_INVALID_FOCUS;
}


/*!
   draw field

   \param[in] ctrl				: list control
   \param[in] item_idx		: item index
   \param[in] field_idx		: field index
   \param[in] is_force			: update invalid region before draw
   \return					: NULL
  */
void list_draw_field_ext(control_t *p_ctrl,
                         u16 item_idx,
                         u8 field_idx,
                         BOOL is_force)
{
  hdc_t hdc = 0;
  rect_t item_rect, field_rect;
  u8 item_status = 0;
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;
  u32 color_idx = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  BOOL ret_boo = FALSE;
  list_item_status_t temp_status = LIST_ITEM_NORMAL;
  
  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  //if(item_idx >= p_list->total)
  if(!_list_check_valid(p_ctrl, item_idx))
  {
    return;
  }
  if(field_idx >= p_list->field_num)
  {
    return;
  }
  p_field = p_list->p_field + field_idx;
  _list_get_item_rect(p_ctrl, item_idx, &item_rect);

  copy_rect(&field_rect, &item_rect);
  field_rect.left += p_field->shift_left;
  field_rect.right = field_rect.left + p_field->width;

  if(is_force)
  {
    ctrl_add_rect_to_invrgn(p_ctrl, &field_rect);
  }

  hdc = gui_begin_paint(p_ctrl, 0);
  if(HDC_INVALID == hdc)
  {
    return;
  }

  // draw frame
  gui_paint_frame(hdc, p_ctrl);

  if(p_ctrl->priv_attr & LIST_SELECT_MODE)
  {
    item_status = list_get_item_status(p_ctrl, item_idx);
  }
  else
  {
    item_status = LIST_ITEM_NORMAL;
  }
  // draw field
  if(p_list->draw_item_cb != NULL)
  {
    temp_status = item_status;

    if(p_list->focus == item_idx)
    {
      if(temp_status == LIST_ITEM_SELECTED)
      {
        temp_status = LIST_ITEM_SELECT_FOCUSED;
      }
      else
      {
        temp_status = LIST_ITEM_FOCUSED;
      }
    }
    
    ret_boo = p_list->draw_item_cb(p_ctrl, temp_status, item_idx, &color_idx);
  }

  if(!ret_boo)
  {
    color_idx = _list_get_style_id(
      p_ctrl, item_status, p_list->focus == item_idx, &p_list->rstyle);
  }
  
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color_idx);
  gui_draw_style_rect(hdc, &field_rect, p_rstyle);

  _list_draw_field(p_ctrl, hdc, item_idx, field_idx, item_status, &item_rect);
  gui_end_paint(p_ctrl, hdc);
}


void list_draw_item_ext(control_t *p_ctrl, u16 item_idx, BOOL is_force)
{
  hdc_t hdc = 0;
  rect_t temp;
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  if(item_idx >= p_list->total)
  {
    return;
  }
  if(is_force)
  {
    _list_get_item_rect(p_ctrl, item_idx, &temp);
    ctrl_add_rect_to_invrgn(p_ctrl, &temp);
  }
  hdc = gui_begin_paint(p_ctrl, 0);
  if(HDC_INVALID == hdc)
  {
    return;
  }
  // draw frame
  gui_paint_frame(hdc, p_ctrl);
  // draw item
  _list_draw_item(p_ctrl, hdc, item_idx);
  gui_end_paint(p_ctrl, hdc);
}


s16 list_get_new_focus(control_t *p_ctrl, s16 offset, BOOL *p_is_reverse)
{
  ctrl_list_t *p_list = NULL;
  s16 new_focus = 0;
  s16 i = 0;
  BOOL find_ret = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  *p_is_reverse = FALSE;
  if(offset == 0)
  {
    return p_list->focus;
  }

  i = p_list->focus + offset;
  find_ret = FALSE;
  while(offset < 0 ? i >= 0 : i < p_list->total)
  {
    if(list_get_item_status(p_ctrl, i) != LIST_ITEM_DISABLED)
    {
      new_focus = i;
      find_ret = TRUE;
      break;
    }
    (offset < 0) ? (i--) : (i++);
  }

  if(!find_ret)
  {
    if(p_ctrl->priv_attr & LIST_CYCLE_MODE)
    {
      if(p_ctrl->priv_attr & LIST_PAGE_MODE)
      {
        if((offset < 0 && p_list->focus + offset < 0)
          || (offset > 0 && p_list->focus >= (p_list->total / p_list->page) *
              p_list->page + p_list->total % p_list->page - 1))
        {
          offset = 0 - offset;
        }
      }
      else
      {
        if((offset < 0 && p_list->focus == 0)
          || (offset > 0 && p_list->focus == p_list->total - 1))
        {
          offset = 0 - offset;
        }
      }
    }

    i = offset < 0 ? 0 : (p_list->total - 1);
    while(offset < 0 ? i < p_list->focus : i > p_list->focus)
    {
      if(list_get_item_status(p_ctrl, i) != LIST_ITEM_DISABLED)
      {
        new_focus = i;
        find_ret = TRUE;
        *p_is_reverse = TRUE;
        break;
      }
      (offset < 0) ? (i++) : (i--);
    }
  }


  if(!find_ret)
  {
    return -1;
  }

  return new_focus;
}


list_ret_t list_scroll_up(control_t *p_ctrl)
{
  return _list_change_scroll(p_ctrl, TRUE);
}


list_ret_t list_scroll_down(control_t *p_ctrl)
{
  return _list_change_scroll(p_ctrl, FALSE);
}


list_ret_t list_page_up(control_t *p_ctrl)
{
  return _list_change_page(p_ctrl, TRUE);
}


list_ret_t list_page_down(control_t *p_ctrl)
{
  return _list_change_page(p_ctrl, FALSE);
}


BOOL list_select(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  if(!(p_ctrl->priv_attr & LIST_SELECT_MODE))
  {
    return FALSE;
  }

  switch(list_get_item_status(p_ctrl, p_list->focus))
  {
    case LIST_ITEM_NORMAL:                                 //normal
      list_select_item(p_ctrl, p_list->focus);
      break;
    case LIST_ITEM_SELECTED:                               //select
      list_unselect_item(p_ctrl, p_list->focus);
      break;
    default:
      return FALSE;
  }

  switch(p_ctrl->priv_attr & LIST_SELECT_MASK)
  {
    case LIST_SINGLE_SELECT:
      ctrl_paint_ctrl(p_ctrl, FALSE);
      break;
    case LIST_MULTI_SELECT:
      list_draw_item_ext(p_ctrl, p_list->focus, FALSE);
      break;
    default:
      MT_ASSERT(0);
      return FALSE;
  }

  return TRUE;
}

void list_item_rect_get(control_t *p_ctrl, u16 index, rect_t *p_item_rect)
{
  _list_get_item_rect(p_ctrl, index, p_item_rect);
}

u8 list_get_field_num(control_t *p_ctrl)
{
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  
  return p_list->field_num;

}

void list_get_field_attr(control_t * p_ctrl, u8 idx, u32 *p_attr,
  u16 * p_left, u16 * p_top, u16 * p_width, u16 * p_height,
  list_xstyle_t *p_fstyle, list_xstyle_t *p_rstyle)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;
  
  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  MT_ASSERT(p_left != NULL);
  MT_ASSERT(p_top != NULL);
  MT_ASSERT(p_width != NULL);
  MT_ASSERT(p_height != NULL);
  MT_ASSERT(p_fstyle != NULL);
  MT_ASSERT(p_rstyle != NULL);
  MT_ASSERT(p_attr != NULL);
  MT_ASSERT(idx < p_list->field_num);
  
  p_field = p_list->p_field + idx;

  *p_left = p_field->shift_left;
  *p_top = p_field->shift_top;
  *p_width = p_field->width;
  *p_height = p_field->height;
  *p_attr = p_field->attr;

  memcpy(p_fstyle, &p_field->fstyle, sizeof(list_xstyle_t));
  memcpy(p_rstyle, &p_field->rstyle, sizeof(list_xstyle_t));

  return;
}

static RET_CODE on_list_paint(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  hdc_t parent_dc = (hdc_t)(para2);

  ctrl_default_proc(p_ctrl, msg, para1, para2);

  //this bar isn't list's child, when parent_dc isn't 0,
  //parent control will draw bar
  if(parent_dc == HDC_INVALID)
  {
    _list_draw_scroll_bar(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_list_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(ctrl_is_rolling(p_ctrl))
  {
    gui_stop_roll(p_ctrl);
  }  

  _list_free_ctrl(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}


static RET_CODE on_list_focus_up(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  old_focus = list_get_focus_pos(p_ctrl);

  if(p_list->columns == LIST_DEFAULT_COLUMNS)
  {
    list_scroll_up(p_ctrl);
  }
  else
  {
    if(p_list->is_hori)
    {
      _list_change_column(p_ctrl, TRUE);
    }
    else
    {
      list_scroll_up(p_ctrl);
    }
  }
  
  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset(p_ctrl);
  }
  return SUCCESS;
}


static RET_CODE on_list_focus_down(control_t *p_ctrl,
                                   u16 msg,
                                   u32 para1,
                                   u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  old_focus = list_get_focus_pos(p_ctrl);

  if(p_list->columns == LIST_DEFAULT_COLUMNS)
  {
    list_scroll_down(p_ctrl);
  }
  else
  {
    if(p_list->is_hori)
    {
      _list_change_column(p_ctrl, FALSE);
    }
    else
    {
      list_scroll_down(p_ctrl);
    }
  }
  
  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset(p_ctrl);
  }  
  return SUCCESS;
}

static RET_CODE on_list_focus_left(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  u16 old_focus = 0, new_focus = 0, lines = 0;
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  
  MT_ASSERT(p_list != NULL);

  if(p_list->columns == LIST_DEFAULT_COLUMNS)
  {
    return SUCCESS;
  }

  old_focus = list_get_focus_pos(p_ctrl);

  new_focus = old_focus;

  MT_ASSERT(p_list->page % p_list->columns == 0);

  lines = p_list->page / p_list->columns;
      
  if(p_list->is_hori)
  {
    list_scroll_up(p_ctrl);
  }
  else
  {
    _list_change_line(p_ctrl, TRUE);
  }
  
  if(new_focus != old_focus)
  {
    gui_roll_reset(p_ctrl);
  }
  return SUCCESS;
}


static RET_CODE on_list_focus_right(control_t *p_ctrl,
                                   u16 msg,
                                   u32 para1,
                                   u32 para2)
{
  u16 old_focus = 0, new_focus = 0, lines = 0;
  ctrl_list_t *p_list = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;
  
  MT_ASSERT(p_list != NULL);

  if(p_list->columns == LIST_DEFAULT_COLUMNS)
  {
    return SUCCESS;
  }
  
  old_focus = list_get_focus_pos(p_ctrl);

  new_focus = old_focus;

  MT_ASSERT(p_list->page % p_list->columns == 0);

  lines = p_list->page / p_list->columns;  

  if(p_list->is_hori)
  {
    list_scroll_down(p_ctrl);
  }
  else
  {
    _list_change_line(p_ctrl, FALSE);
  }

  if(new_focus != old_focus)
  {
    list_draw_item_ext(p_ctrl, old_focus, TRUE);
    list_draw_item_ext(p_ctrl, new_focus, TRUE);
    
    gui_roll_reset(p_ctrl);
  }

  return SUCCESS;
}



static RET_CODE on_list_page_up(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  old_focus = list_get_focus_pos(p_ctrl);
  
  list_page_up(p_ctrl);

  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset(p_ctrl);
  }    
  return SUCCESS;
}


static RET_CODE on_list_page_down(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  u16 old_focus = 0, new_focus = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  old_focus = list_get_focus_pos(p_ctrl);
  
  list_page_down(p_ctrl);

  new_focus = list_get_focus_pos(p_ctrl);
  if(new_focus != old_focus)
  {
    gui_roll_reset(p_ctrl);
  }      
  return SUCCESS;
}


static RET_CODE on_list_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  list_select(p_ctrl);
  return SUCCESS;
}

static RET_CODE on_list_roll_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_list_t *p_list = NULL;
  u8 i = 0, status = 0;
  rect_t item_rect = {0};
  list_field_t *p_field = NULL;
  rsurf_ceate_t rsurf = {0};
  
  p_list = (ctrl_list_t *)p_ctrl;

  MT_ASSERT(p_list != NULL);
  
  _list_get_item_rect(p_ctrl, p_list->focus, &item_rect);

  for(i = 0; i < p_list->field_num; i++)
  {
    p_field = p_list->p_field + i;

    status = list_get_item_status(p_ctrl, i); 

    if(p_field->attr & LISTFIELD_SCROLL)
    {
      rsurf.rect.left = item_rect.left + p_field->shift_left;
      rsurf.rect.top = item_rect.top + p_field->shift_top;
      rsurf.rect.right = rsurf.rect.left + p_field->width;
      rsurf.rect.bottom = item_rect.bottom;

      rsurf.p_str = (u16 *)list_get_field_content(p_ctrl, p_list->focus, i);

      rsurf.fstyle_idx = _list_get_style_id(p_ctrl, status, TRUE, &p_field->fstyle);;

      rsurf.p_ctrl = p_ctrl;

      rsurf.context = i;

      rsurf.draw_style = 0;

      rsurf.is_topmost = ctrl_is_topmost(p_ctrl);

      gui_create_rsurf(&rsurf, (roll_param_t *)para1);
    }
  }

  return SUCCESS;
}

static RET_CODE on_list_rolling(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_list_t *p_list = NULL;
  list_field_t *p_field = NULL;
  u8 field_idx = 0, status = 0;
  rect_t item_rect = {0}, field_rect = {0};
  hdc_t hdc = 0;  

  MT_ASSERT(p_ctrl != NULL);

  p_list = (ctrl_list_t *)p_ctrl;

  MT_ASSERT(p_list != NULL);

  field_idx = gui_roll_get_context((void *)para1);
  
  MT_ASSERT(field_idx < p_list->field_num);

  _list_get_item_rect(p_ctrl, p_list->focus, &item_rect);

  p_field = p_list->p_field + field_idx;
  
  field_rect.left = item_rect.left + p_field->shift_left;
  field_rect.top = item_rect.top + p_field->shift_top;
  field_rect.right = field_rect.left + p_field->width;
  field_rect.bottom = item_rect.bottom;

  ctrl_add_rect_to_invrgn(p_ctrl, &field_rect);

  hdc = gui_begin_paint(p_ctrl, 0);
  gui_paint_frame(hdc, p_ctrl);  

  _list_draw_item_frame(p_ctrl, hdc, p_list->focus);

  status = list_get_item_status(p_ctrl, p_list->focus);        
  _list_draw_field_frame(p_ctrl, hdc, p_list->focus, field_idx, status, &item_rect);

  gui_rolling_node((void *)para1, hdc);

  gui_end_paint(p_ctrl, hdc);

  return SUCCESS;
}

static RET_CODE on_list_roll_next(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  //list don't support this function, so return error.
  
  return ERR_FAILURE;
}

static RET_CODE on_list_attr_changed(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 old_attr = OBJ_ATTR_ACTIVE, new_attr = OBJ_ATTR_ACTIVE;

  old_attr = (u8)para1;

  new_attr = (u8)para2;

  if(old_attr == OBJ_ATTR_HL && new_attr != OBJ_ATTR_HL)
  {
    gui_pause_roll(p_ctrl);
  }
  else if(old_attr != OBJ_ATTR_HL && new_attr == OBJ_ATTR_HL)
  {
    gui_resume_roll(p_ctrl);
  }
  
  return SUCCESS;
}

// define the default msgmap of class
BEGIN_CTRLPROC(list_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_PAINT, on_list_paint)
  ON_COMMAND(MSG_FOCUS_UP, on_list_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_list_focus_down)
  ON_COMMAND(MSG_FOCUS_LEFT, on_list_focus_left)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_list_focus_right)  
  ON_COMMAND(MSG_PAGE_UP, on_list_page_up)
  ON_COMMAND(MSG_PAGE_DOWN, on_list_page_down)
  ON_COMMAND(MSG_SELECT, on_list_select)
  ON_COMMAND(MSG_DESTROY, on_list_destroy)
  ON_COMMAND(MSG_START_ROLL, on_list_roll_start)
  ON_COMMAND(MSG_ROLLING, on_list_rolling)
  ON_COMMAND(MSG_ROLL_NEXT, on_list_roll_next)
  ON_COMMAND(MSG_ATTR_CHANGED, on_list_attr_changed)
END_CTRLPROC(list_class_proc, ctrl_default_proc)
