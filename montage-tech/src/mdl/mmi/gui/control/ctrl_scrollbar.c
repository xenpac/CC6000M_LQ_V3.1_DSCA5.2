/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_scrollbar.c
   this file  implement the functions defined in  ctrl_scrollbar.h, also it
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

#include "ctrl_scrollbar.h"

/*!
  scroll-bar control.
  */
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
     Highlight style(when pressed) of middle block.
    */
  u32 h_mid_rstyle;
  /*!
     Gray style of middle block, can't scroll.
    */
  u32 g_mid_rstyle;
  /*!
     Pressed status.
    */
  u8 pressed;
  /*!
     Item numbers per page.
    */
  u16 page;
  /*!
     Total item numbers.
    */
  u16 total;
  /*!
     Current position.
    */
  u16 cur_pos;  
}ctrl_sbar_t;
static u32 _sbar_get_rstyle_idx(control_t *p_ctrl)
{
  ctrl_sbar_t *p_sbar = NULL;
  u32 rstyle = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;

  if(p_sbar->pressed != 0)
  {
    rstyle = p_sbar->h_mid_rstyle;
  }
  else if(p_ctrl->attr == OBJ_ATTR_INACTIVE)
  {
    rstyle = p_sbar->g_mid_rstyle;
  }
  else
  {
    rstyle = p_sbar->n_mid_rstyle;
  }
  return rstyle;
}


static u16 _sbar_calc_grid(control_t *p_ctrl)
{
  rsc_rstyle_t *p_rstyle = NULL;
  u32 rstyle_id = 0;
  u16 bmp_w = 0, bmp_h = 0;
  u16 min = 0, mid = 0, grid = 0;
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;

  rstyle_id = _sbar_get_rstyle_idx(p_ctrl);
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), rstyle_id);

  min = 0;
  if(p_ctrl->priv_attr & SBAR_DIRECTION_MASK)
  {
    // In this case, rect style should be (left + bg + right)
    if(RSTYLE_IS_R_ICON(p_rstyle->left))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->left), &bmp_w, &bmp_h);
      min += bmp_w;
    }

    if(RSTYLE_IS_R_ICON(p_rstyle->right))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->right), &bmp_w, &bmp_h);
      min += bmp_w;
    }
  }
  else
  {
    if(RSTYLE_IS_R_ICON(p_rstyle->top))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->top), &bmp_w, &bmp_h);
      min += bmp_h;
    }

    if(RSTYLE_IS_R_ICON(p_rstyle->bottom))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->bottom), &bmp_w, &bmp_h);
      min += bmp_h;
    }
  }

  if(p_ctrl->priv_attr & SBAR_AUTOSIZE_MASK)
  {
    mid = p_ctrl->priv_attr & SBAR_DIRECTION_MASK ?
          RECTW(p_ctrl->mrect) : RECTH(p_ctrl->mrect);
    grid = p_sbar->page * mid / p_sbar->total;

    if(min > grid)
    {
      grid = min;
    }
  }
  else
  {
    grid = min;
  }

  return grid;
}


static BOOL _sbar_get_thumb_rect(control_t *p_ctrl, rect_t *p_thumb_rect)
{
  BOOL last_page = FALSE;
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;

  if((p_sbar->page >= p_sbar->total) || (p_sbar->page == 0))
  {
    memcpy(p_thumb_rect, &p_ctrl->mrect, sizeof(rect_t));
    return TRUE;
  }
  else   // normal case
  {
    u16 gird = _sbar_calc_grid(p_ctrl);

    last_page = (BOOL)(!(p_sbar->cur_pos + p_sbar->page < p_sbar->total));

    if(p_ctrl->priv_attr & SBAR_DIRECTION_MASK) //get thumb rect
    {
      // width
      if(last_page)
      {
        p_thumb_rect->left = p_ctrl->mrect.right - gird;
      }
      else
      {
        p_thumb_rect->left = p_ctrl->mrect.left + p_sbar->cur_pos *
                             (p_ctrl->mrect.right -
                              p_ctrl->mrect.left -
                              gird) / (p_sbar->total - p_sbar->page);
      }
      p_thumb_rect->right = p_thumb_rect->left + gird;
      p_thumb_rect->top = p_ctrl->mrect.top;
      p_thumb_rect->bottom = p_ctrl->mrect.bottom;
    }
    else
    {
      if(last_page)
      {
        p_thumb_rect->top = p_ctrl->mrect.bottom - gird;
      }
      else
      {
        p_thumb_rect->top = p_ctrl->mrect.top + p_sbar->cur_pos *
                            (p_ctrl->mrect.bottom - p_ctrl->mrect.top -
                             gird) / (p_sbar->total - p_sbar->page);
      }
      p_thumb_rect->bottom = p_thumb_rect->top + gird;
      p_thumb_rect->left = p_ctrl->mrect.left;
      p_thumb_rect->right = p_ctrl->mrect.right;
    }

    return TRUE;
  }
}


static void _sbar_draw(control_t *p_ctrl, hdc_t hdc)
{
  u32 rstyle_id = 0;
  rsc_rstyle_t *p_rstyle = NULL;
  rect_t rect_mid;

  gui_paint_frame(hdc, p_ctrl);

  if(_sbar_get_thumb_rect(p_ctrl, &rect_mid))
  {
    rstyle_id = _sbar_get_rstyle_idx(p_ctrl);
    p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), rstyle_id);
    gui_draw_style_rect(hdc, &rect_mid, p_rstyle);
  }
}


RET_CODE sbar_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_sbar_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of scrollbar class
  p_default_ctrl->priv_attr = SBAR_AUTOSIZE_MASK;
  p_default_ctrl->p_proc = sbar_class_proc;
  p_default_ctrl->p_paint = _sbar_draw;

  // initalize the default data of scrollbar class
  
  if(ctrl_register_ctrl_class(CTRL_SBAR, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void sbar_set_mid_rstyle(control_t *p_ctrl,
                        u32 n_mid_rstyle,
                        u32 h_mid_rstyle,
                        u32 g_mid_rstyle)
{
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;
  p_sbar->n_mid_rstyle = n_mid_rstyle;
  p_sbar->h_mid_rstyle = h_mid_rstyle;
  p_sbar->g_mid_rstyle = g_mid_rstyle;
}

void sbar_get_mid_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_rstyle)
{
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rstyle != NULL);
  
  p_sbar = (ctrl_sbar_t *)p_ctrl;

  p_rstyle->gray_idx = p_sbar->g_mid_rstyle;
  p_rstyle->show_idx = p_sbar->n_mid_rstyle;
  p_rstyle->hl_idx = p_sbar->h_mid_rstyle;
}

void sbar_set_count(control_t *p_ctrl, u16 page, u16 total)
{
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;
  p_sbar->page = page;
  p_sbar->total = total;
}


void sbar_set_current_pos(control_t *p_ctrl, u16 cur_pos)
{
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;
  p_sbar->cur_pos = cur_pos;
}


u16 sbar_get_current_pos(control_t *p_ctrl)
{
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;
  return p_sbar->cur_pos;
}


void sbar_set_direction(control_t *p_ctrl, BOOL is_hori)
{
  MT_ASSERT(p_ctrl != NULL);

  if(is_hori)
  {
    p_ctrl->priv_attr |= SBAR_DIRECTION_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~SBAR_DIRECTION_MASK);
  }
}

BOOL sbar_is_hori(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (BOOL)(p_ctrl->priv_attr & SBAR_DIRECTION_MASK);
}


void sbar_set_autosize_mode(control_t *p_ctrl, BOOL is_autosize)
{
  MT_ASSERT(p_ctrl != NULL);

  if(is_autosize)
  {
    p_ctrl->priv_attr |= SBAR_AUTOSIZE_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~SBAR_AUTOSIZE_MASK);
  }
}

BOOL sbar_is_autosize(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (BOOL)(p_ctrl->priv_attr & SBAR_AUTOSIZE_MASK);
}

BOOL sbar_increase(control_t *p_ctrl)
{
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;
  if(p_sbar->cur_pos < p_sbar->total - 1)
  {
    p_sbar->cur_pos++;
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    return TRUE;
  }
  return FALSE;
}


BOOL sbar_decrease(control_t *p_ctrl)
{
  ctrl_sbar_t *p_sbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_sbar = (ctrl_sbar_t *)p_ctrl;
  if(p_sbar->cur_pos > 0)
  {
    p_sbar->cur_pos--;
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    return TRUE;
  }
  return FALSE;
}


static RET_CODE on_sbar_increase(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  sbar_increase(p_ctrl);
  return SUCCESS;
}


static RET_CODE on_sbar_decrease(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  sbar_decrease(p_ctrl);
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(sbar_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_INCREASE, on_sbar_increase)
ON_COMMAND(MSG_DECREASE, on_sbar_decrease)
END_CTRLPROC(sbar_class_proc, ctrl_default_proc)
