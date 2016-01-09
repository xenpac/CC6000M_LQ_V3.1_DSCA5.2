/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_progressbar.c
   this file  implement the functions defined in  ctrl_progressbar.h, also it
   implement some internal used
   function. All these functions are about how to decribe, set and draw a pbar
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

#include "ctrl_progressbar.h"

/*!
  progress-bar control.
  */
typedef struct
{
  /*!
    base contro.
    */
  control_t base;
  /*!
     Minimum value of pbar.
    */
  u16 min;
  /*!
     Maximum value of pbar.
    */
  u16 max;
  /*!
     Current value of pbar.
    */
  u16 curn;
  /*!
     How many steps between max and min value,
     every step have at lease 1 pixel and 1 value
    */
  u16 step;
  /*!
     Interval between blocks.
    */
  u8 interval;
  /*!
     Rectangle style at minimum value side, 0xff means invalid.
    */
  u32 min_rstyle;
  /*!
     Rectangle style at maximum value side, 0xff means invalid.
    */
  u32 max_rstyle;
  /*!
     Rectangle style at current point, 0xff means invalid.
    */
  u32 cur_rstyle;  
  /*!
    pbar segment
    */
  pbar_seg_t *p_seg;      
}ctrl_pbar_t;

static u16 _pbar_get_mid_width(control_t *p_ctrl, ctrl_pbar_t *p_pbar)
{
  u16 cur_size = 0, bmp_w = 0, bmp_h = 0;
  rsc_rstyle_t *p_rstyle = NULL;

  if(p_pbar->cur_rstyle == INVALID_RSTYLE_IDX)
  {
    return 0;
  }
  else
  {
    cur_size = 0;
    p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_pbar->cur_rstyle);
    if(RSTYLE_IS_R_ICON(p_rstyle->left))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->left), &bmp_w, &bmp_h);
      cur_size += bmp_w;
    }
    if(RSTYLE_IS_R_ICON(p_rstyle->right))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->right), &bmp_w, &bmp_h);
      cur_size += bmp_w;
    }
    if(cur_size == 0)
    {
      cur_size =
        (p_ctrl->mrect.right - p_ctrl->mrect.left) / p_pbar->step;
    }
    return cur_size;
  }
}


static u16 _pbar_get_mid_height(control_t *p_ctrl, ctrl_pbar_t *p_pbar)
{
  u16 cur_size = 0, bmp_w = 0, bmp_h = 0;
  rsc_rstyle_t *p_rstyle = NULL;

  if(p_pbar->cur_rstyle == INVALID_RSTYLE_IDX)
  {
    return 0;
  }
  else
  {
    cur_size = 0;
    p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_pbar->cur_rstyle);
    if(RSTYLE_IS_R_ICON(p_rstyle->top))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->top), &bmp_w, &bmp_h);
      cur_size += bmp_h;
    }
    if(RSTYLE_IS_R_ICON(p_rstyle->bottom))
    {
      gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->bottom), &bmp_w, &bmp_h);
      cur_size += bmp_h;
    }
    if(cur_size == 0)
    {
      cur_size =
        (p_ctrl->mrect.bottom - p_ctrl->mrect.top) / p_pbar->step;
    }
    return cur_size;
  }
}


static BOOL _pbar_get_continuous_min_rect(control_t *p_ctrl, rect_t *p_min_rect)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 temp = 0, cur_size = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  if(p_pbar->min_rstyle == 0xFF)
  {
    return FALSE;
  }

  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK)
  {
    temp = p_ctrl->mrect.right - p_ctrl->mrect.left;
    temp = temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);

    if(temp == 0)
    {
      return FALSE;
    }

    cur_size = _pbar_get_mid_width(p_ctrl, p_pbar);
    if(cur_size > temp)
    {
      return FALSE;
    }

    p_min_rect->top = p_ctrl->mrect.top;
    p_min_rect->bottom = p_ctrl->mrect.bottom;
    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK)
    {
      p_min_rect->right = p_ctrl->mrect.right;
      p_min_rect->left = p_min_rect->right - temp;
      if(p_pbar->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->left += cur_size;
      }
    }
    else                        //0 : left is min value
    {
      p_min_rect->left = p_ctrl->mrect.left;
      p_min_rect->right = p_min_rect->left + temp;
      if(p_pbar->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->right -= cur_size;
      }
    }
    return TRUE;
  }
  else                          //0 is vertical
  {
    temp = p_ctrl->mrect.bottom - p_ctrl->mrect.top;
    temp = temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);

    if(temp == 0)
    {
      return FALSE;
    }

    cur_size = _pbar_get_mid_height(p_ctrl, p_pbar);
    if(cur_size > temp)
    {
      return FALSE;
    }

    p_min_rect->left = p_ctrl->mrect.left;
    p_min_rect->right = p_ctrl->mrect.right;

    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK)
    {
      p_min_rect->top = p_ctrl->mrect.top;
      p_min_rect->bottom = p_min_rect->top + temp;
      if(p_pbar->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->bottom -= cur_size;
      }
    }
    else                        //0 : bottom is min value
    {
      p_min_rect->bottom = p_ctrl->mrect.bottom;
      p_min_rect->top = p_min_rect->bottom - temp;
      if(p_pbar->cur_rstyle != INVALID_RSTYLE_IDX)
      {
        p_min_rect->top += cur_size;
      }
    }
    return TRUE;
  }
}


static BOOL _pbar_get_continuous_max_rect(control_t *p_ctrl, rect_t *p_max_rect)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  if(p_pbar->max_rstyle == 0xFF)
  {
    return FALSE;
  }

  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK) // 1 is horizontal
  {
    temp = p_ctrl->mrect.right - p_ctrl->mrect.left;
    temp = temp * (p_pbar->max - p_pbar->curn) / (p_pbar->max - p_pbar->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_max_rect->top = p_ctrl->mrect.top;
    p_max_rect->bottom = p_ctrl->mrect.bottom;

    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1, right is min value
    {
      p_max_rect->left = p_ctrl->mrect.left;
      p_max_rect->right = p_max_rect->left + temp;
    }
    else                        //0 : left is min value
    {
      p_max_rect->right = p_ctrl->mrect.right;
      p_max_rect->left = p_max_rect->right - temp;
    }
    return TRUE;
  }
  else                          //0 is vertical
  {
    temp = p_ctrl->mrect.bottom - p_ctrl->mrect.top;
    temp = temp * (p_pbar->max - p_pbar->curn) / (p_pbar->max - p_pbar->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_max_rect->left = p_ctrl->mrect.left;
    p_max_rect->right = p_ctrl->mrect.right;

    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1: top is min value
    {
      p_max_rect->bottom = p_ctrl->mrect.bottom;
      p_max_rect->top = p_max_rect->bottom - temp;
    }
    else                        //0 : bottom is min value
    {
      p_max_rect->top = p_ctrl->mrect.top;
      p_max_rect->bottom = p_max_rect->top + temp;
    }
    return TRUE;
  }
}


static BOOL _pbar_get_continuous_mid_rect(control_t *p_ctrl, rect_t *p_mid_rect)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 temp = 0, cur_size = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  if(p_pbar->cur_rstyle == INVALID_RSTYLE_IDX)
  {
    return FALSE;
  }

  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK) // 1 is horizontal
  {
    temp = p_ctrl->mrect.right - p_ctrl->mrect.left;
    temp = temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_mid_rect->top = p_ctrl->mrect.top;
    p_mid_rect->bottom = p_ctrl->mrect.bottom;

    cur_size = _pbar_get_mid_width(p_ctrl, p_pbar);
    if(cur_size > temp)
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:right is min value
      {
        p_mid_rect->right = p_ctrl->mrect.right;
        p_mid_rect->left = p_mid_rect->right - temp;
      }
      else                        //0 : left is min value
      {
        p_mid_rect->left = p_ctrl->mrect.left;
        p_mid_rect->right = p_mid_rect->left + temp;
      }
    }
    else
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:right is min value
      {
        p_mid_rect->left = p_ctrl->mrect.right - temp;
        p_mid_rect->right = p_mid_rect->left + cur_size;
      }
      else                        //0 : left is min value
      {
        p_mid_rect->right = p_ctrl->mrect.left + temp;
        p_mid_rect->left = p_mid_rect->right - cur_size;
      }
    }

    return TRUE;
  }
  else                            //0 is vertical
  {
    temp = p_ctrl->mrect.bottom - p_ctrl->mrect.top;
    temp = temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);

    if(temp == 0)
    {
      return FALSE;
    }

    p_mid_rect->left = p_ctrl->mrect.left;
    p_mid_rect->right = p_ctrl->mrect.right;

    cur_size = _pbar_get_mid_height(p_ctrl, p_pbar);
    if(cur_size > temp)
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:top is min value
      {
        p_mid_rect->top = p_ctrl->mrect.top;
        p_mid_rect->bottom = p_mid_rect->top + temp;
      }
      else                        //0 : bottom is min value
      {
        p_mid_rect->bottom = p_ctrl->mrect.bottom;
        p_mid_rect->top = p_mid_rect->bottom - temp;
      }
    }
    else
    {
      if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:top is min value
      {
        p_mid_rect->bottom = p_ctrl->mrect.top + temp;
        p_mid_rect->top = p_mid_rect->bottom - cur_size;
      }
      else                        //0 : bottom is min value
      {
        p_mid_rect->top = p_ctrl->mrect.bottom - temp;
        p_mid_rect->bottom = p_mid_rect->top + cur_size;
      }
    }

    return TRUE;
  }
}

static u16 _pbar_get_seg_width(control_t *p_ctrl, u32 rstyle_idx)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 seg_w = 0, bmp_w = 0, bmp_h = 0;
  rsc_rstyle_t *p_rstyle = NULL;

  MT_ASSERT(p_ctrl != NULL);
  
  p_pbar = (ctrl_pbar_t *)p_ctrl;  

  seg_w = 0;
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), rstyle_idx);
  MT_ASSERT(p_rstyle != NULL);
  
  if(RSTYLE_IS_R_ICON(p_rstyle->left))
  {
    gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->left), &bmp_w, &bmp_h);
    seg_w += bmp_w;
  }
  if(RSTYLE_IS_R_ICON(p_rstyle->right))
  {
    gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->right), &bmp_w, &bmp_h);
    seg_w += bmp_w;
  }
  
  if(seg_w == 0)
  {
    seg_w =
      (p_ctrl->mrect.right - p_ctrl->mrect.left) / p_pbar->step;
  }
  
  return seg_w;
}


static u16 _pbar_get_seg_height(control_t *p_ctrl, u32 rstyle_id)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 seg_h = 0, bmp_w = 0, bmp_h = 0;
  rsc_rstyle_t *p_rstyle = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_pbar = (ctrl_pbar_t *)p_ctrl;  

  seg_h = 0;
  p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), rstyle_id);
  MT_ASSERT(p_rstyle != NULL);
  
  if(RSTYLE_IS_R_ICON(p_rstyle->top))
  {
    gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->top), &bmp_w, &bmp_h);
    seg_h += bmp_h;
  }
  if(RSTYLE_IS_R_ICON(p_rstyle->bottom))
  {
    gui_get_bmp_info(RSTYLE_GET_ICON(p_rstyle->bottom), &bmp_w, &bmp_h);
    seg_h += bmp_h;
  }
  
  if(seg_h == 0)
  {
    seg_h =
      (p_ctrl->mrect.bottom - p_ctrl->mrect.top) / p_pbar->step;
  }
  
  return seg_h;
}

static BOOL _pbar_get_continuous_seg_rect(control_t *p_ctrl,
  pbar_seg_des_t *p_seg_des, rect_t *p_seg_rect)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 temp = 0;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_seg_des != NULL);
  
  p_pbar = (ctrl_pbar_t *)p_ctrl;
  
  if(p_pbar->curn < p_seg_des->per_min)
  {
    return FALSE;
  }

  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK)
  {
    p_seg_rect->top = p_ctrl->mrect.top;
    p_seg_rect->bottom = p_ctrl->mrect.bottom;

    temp = p_ctrl->mrect.right - p_ctrl->mrect.left;
    
    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK)  //right is min-end
    {
      p_seg_rect->right = p_ctrl->mrect.right -
        temp * (p_seg_des->per_min - p_pbar->min) / (p_pbar->max - p_pbar->min);

      if(p_seg_des->per_max >= p_pbar->curn)
      {
        p_seg_rect->left = p_ctrl->mrect.right -
          temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
      else
      {
        p_seg_rect->left = p_ctrl->mrect.right -
          temp * (p_seg_des->per_max - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
    }
    else                                        //left is min value
    {
      p_seg_rect->left = p_ctrl->mrect.left +
        temp * (p_seg_des->per_min - p_pbar->min) / (p_pbar->max - p_pbar->min);

      if(p_seg_des->per_max >= p_pbar->curn)
      {
        p_seg_rect->right = p_ctrl->mrect.left +
          temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
      else
      {
        p_seg_rect->right = p_ctrl->mrect.left +
          temp * (p_seg_des->per_max - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
    }

    if(RECTWP(p_seg_rect) < _pbar_get_seg_width(p_ctrl, p_seg_des->rstyle_id))
    {
      return FALSE;
    }
  }
  else                          //0 is vertical
  {
    p_seg_rect->left = p_ctrl->mrect.left;
    p_seg_rect->right = p_ctrl->mrect.right;

    temp = p_ctrl->mrect.right - p_ctrl->mrect.left;

    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK)//top is min-end
    {
      p_seg_rect->top = p_ctrl->mrect.top +
        temp * (p_seg_des->per_min - p_pbar->min) / (p_pbar->max - p_pbar->min);

      if(p_seg_des->per_max >= p_pbar->curn)
      {
        p_seg_rect->bottom = p_ctrl->mrect.top +
          temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
      else
      {
        p_seg_rect->bottom = p_ctrl->mrect.top +
          temp * (p_seg_des->per_max - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
    }
    else                                    //bottom is min-end
    {
      p_seg_rect->bottom = p_ctrl->mrect.bottom -
        temp * (p_seg_des->per_min - p_pbar->min) / (p_pbar->max - p_pbar->min);

      if(p_seg_des->per_max >= p_pbar->curn)
      {
        p_seg_rect->top = p_ctrl->mrect.bottom -
          temp * (p_pbar->curn - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
      else
      {
        p_seg_rect->top = p_ctrl->mrect.bottom -
          temp * (p_seg_des->per_max - p_pbar->min) / (p_pbar->max - p_pbar->min);
      }
    }

    if(RECTHP(p_seg_rect) < _pbar_get_seg_height(p_ctrl, p_seg_des->rstyle_id))
    {
      return FALSE;
    }    
  }

  return TRUE;  
}

static u16 _pbar_get_cur_block_index(control_t *p_ctrl)
{
  u16 size = 0, temp1 = 0, temp2 = 0;
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;

  if(p_pbar->curn <= p_pbar->min)
  {
    return 0;
  }

  if(p_pbar->curn >= p_pbar->max)
  {
    return p_pbar->step;
  }

  size = p_pbar->max - p_pbar->min;
  temp1 = size / p_pbar->step;
  temp2 = size % p_pbar->step;
  //the block 0 ~ (temp2-1) have temp1+1
  //the block temp2 ~ (step - temp2 +1) have temp1
  if(temp2 * (temp1 + 1) > p_pbar->curn)
  {
    return p_pbar->curn / (temp1 + 1);
  }
  else
  {
    return (p_pbar->curn - temp2) / temp1;
  }
}


static u16 _pbar_calc_curn(control_t *p_ctrl, u16 index)
{
  u16 size = 0, temp1 = 0, temp2 = 0;
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;

  if(index >= (p_pbar->step))
  {
    return p_pbar->max;
  }

  size = p_pbar->max - p_pbar->min;
  temp1 = size / p_pbar->step;
  temp2 = size % p_pbar->step;
  //the block 0 ~ (temp2-1) have temp1+1
  //the block temp2 ~ (step - temp2 +1) have temp1
  if(index < temp2)
  {
    return p_pbar->min + index * (temp1 + 1);
  }
  else
  {
    return p_pbar->min + temp2 + index * temp1;
  }
}


//block index: from 0 ~ (p_pbar->step-1) , 0 is min size
static u32 _pbar_get_block_rstyle(control_t *p_ctrl, u16 index)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 cur_index = 0, i = 0;
  pbar_seg_des_t *p_seg_desc = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  cur_index = _pbar_get_cur_block_index(p_ctrl);

  if(p_pbar->p_seg != NULL)
  {
    for(i = 0; i < p_pbar->p_seg->seg_count; i++)
    {
      p_seg_desc = (pbar_seg_des_t *)(p_pbar->p_seg->p_seg_des + i);
      MT_ASSERT(p_seg_desc != NULL);

      if(index < p_seg_desc->per_max && index >= p_seg_desc->per_min && index < cur_index)
      {
        return p_seg_desc->rstyle_id;
      }
    }
  }
  
  if(index < cur_index)
  {
    return p_pbar->min_rstyle;
  }
  else if(index > cur_index)
  {
    return p_pbar->max_rstyle;
  }
  else if(cur_index == 0)
  {
    return p_pbar->min_rstyle;
  }
  else
  {
    return p_pbar->cur_rstyle;
  }
}


static BOOL _pbar_get_block_rect(control_t *p_ctrl,
                                 u16 index,
                                 rect_t *p_block_rect)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 temp = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(index == 0)
  {
    return FALSE;
  }

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  if(p_ctrl->priv_attr & PBAR_DIRECTION_MASK) // 1 is horizontal
  {
    temp = p_ctrl->mrect.right - p_ctrl->mrect.left -
           (p_pbar->step - 1) * (p_pbar->interval);

    p_block_rect->top = p_ctrl->mrect.top;
    p_block_rect->bottom = p_ctrl->mrect.bottom;
    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:right is min value
    {
      p_block_rect->right = p_ctrl->mrect.right -
        temp * (index - 1) / p_pbar->step - (index - 1) * (p_pbar->interval);
      p_block_rect->left = p_ctrl->mrect.right - 
        temp * index / p_pbar->step - (index - 1) * (p_pbar->interval);
    }
    else                        //0 : left is min value
    {
      p_block_rect->left = p_ctrl->mrect.left + 
        temp * (index - 1) / p_pbar->step + (index - 1) * (p_pbar->interval);
      p_block_rect->right = p_ctrl->mrect.left + 
        temp * index / p_pbar->step + (index - 1) * (p_pbar->interval);
    }
  }
  else                            //0 is vertical
  {
    temp = p_ctrl->mrect.bottom - p_ctrl->mrect.top -
           (p_pbar->step - 1) * (p_pbar->interval);

    p_block_rect->left = p_ctrl->mrect.left;
    p_block_rect->right = p_ctrl->mrect.right;
    if(p_ctrl->priv_attr & PBAR_WORKMODE_MASK) // 1:top is min value
    {
      p_block_rect->top = p_ctrl->mrect.top +
        temp * (index - 1) / p_pbar->step + (index - 1) * (p_pbar->interval);
      p_block_rect->bottom = p_ctrl->mrect.top + 
        temp * index / p_pbar->step + (index - 1) * (p_pbar->interval);
    }
    else                        //0 : bottom is min value
    {
      p_block_rect->bottom = p_ctrl->mrect.bottom - 
        temp * (index - 1) / p_pbar->step - (index - 1) * (p_pbar->interval);
      p_block_rect->top = p_ctrl->mrect.bottom - 
        temp * index / p_pbar->step - index * (p_pbar->interval);
    }
  }
  return TRUE;
}


static void _pbar_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_pbar_t *p_pbar = NULL;
  rect_t temp_rect;
  rsc_rstyle_t *p_rstyle = NULL;
  u16 i = 0;
  u32 color = 0;

  gui_paint_frame(hdc, p_ctrl);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  if(p_pbar->step == 0)
  {
    return;
  }

  if(p_ctrl->priv_attr & PBAR_CONTINUOUS_MASK) // 1 is continuous bar
  {
    if(_pbar_get_continuous_min_rect(p_ctrl, &temp_rect))
    {
      p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_pbar->min_rstyle);
      gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
    }
    if(_pbar_get_continuous_max_rect(p_ctrl, &temp_rect))
    {
      p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_pbar->max_rstyle);
      gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
    }
    if(TRUE == _pbar_get_continuous_mid_rect(p_ctrl, &temp_rect))
    {
      p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_pbar->cur_rstyle);
      gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
    }

    if(p_pbar->p_seg != NULL)
    {
      for(i = 0; i < p_pbar->p_seg->seg_count; i++)
      {
        MT_ASSERT(p_pbar->p_seg->p_seg_des + i != NULL);
        if(_pbar_get_continuous_seg_rect(p_ctrl, p_pbar->p_seg->p_seg_des + i, &temp_rect))
        {
          p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), p_pbar->p_seg->p_seg_des[i].rstyle_id);
          gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
        }
      }
    }
  }
  else                          // 0 is block bar
  {
    for(i = p_pbar->min; i <= p_pbar->max; i++)
    {
      if(_pbar_get_block_rect(p_ctrl, i, &temp_rect))
      {
        color = _pbar_get_block_rstyle(p_ctrl, i);
        p_rstyle = rsc_get_rstyle(gui_get_rsc_handle(), color);
        gui_draw_style_rect(hdc, &temp_rect, p_rstyle);
      }
    }
  }
}


RET_CODE pbar_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_pbar_t *p_default_pbar = NULL;

  register_info.data_size = sizeof(ctrl_pbar_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_pbar = (ctrl_pbar_t *)register_info.p_default_ctrl;
  
  // initialize the default control of progressbar class
  p_default_ctrl->priv_attr = PBAR_DIRECTION_MASK | PBAR_CONTINUOUS_MASK;
  p_default_ctrl->p_proc = pbar_class_proc;
  p_default_ctrl->p_paint = _pbar_draw;

  // initalize the default data of progressbar class
  p_default_pbar->p_seg = NULL;
  
  if(ctrl_register_ctrl_class(CTRL_PBAR, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}

void pbar_set_rstyle(control_t *p_ctrl,
                     u32 min_rstyle,
                     u32 max_rstyle,
                     u32 cur_rstyle)
{
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  p_pbar->min_rstyle = min_rstyle;
  p_pbar->max_rstyle = max_rstyle;
  p_pbar->cur_rstyle = cur_rstyle;
}

void pbar_get_rstyle(control_t *p_ctrl, u32 *p_min_rstyle, u32 *p_max_rstyle, u32 *p_curn_rstyle)
{
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_min_rstyle != NULL);
  MT_ASSERT(p_max_rstyle != NULL);
  MT_ASSERT(p_curn_rstyle != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;

  *p_min_rstyle = p_pbar->min_rstyle;
  *p_max_rstyle = p_pbar->max_rstyle;
  *p_curn_rstyle = p_pbar->cur_rstyle;
}


void pbar_set_count(control_t *p_ctrl, u16 min, u16 max, u16 step)
{
  ctrl_pbar_t *p_pbar = NULL;

  if(p_ctrl == NULL)
  {
    return;
  }
  p_pbar = (ctrl_pbar_t *)p_ctrl;
  p_pbar->min = min;
  p_pbar->max = max;
  p_pbar->step = step;
}


void pbar_set_current(control_t *p_ctrl, u16 curn)
{
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  p_pbar->curn = curn;
}


u16 pbar_get_current(control_t *p_ctrl)
{
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  return p_pbar->curn;
}


void pbar_set_direction(control_t *p_ctrl, u8 dir)
{
  MT_ASSERT(p_ctrl != NULL);

  if(dir != 0)
  {
    p_ctrl->priv_attr |= PBAR_DIRECTION_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~PBAR_DIRECTION_MASK);
  }
}

void pbar_set_seg(control_t *p_ctrl, pbar_seg_t *p_seg)
{
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_seg != NULL);
  MT_ASSERT(p_seg->seg_count != 0);
  MT_ASSERT(p_seg->p_seg_des != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;

  if(p_pbar->p_seg != NULL)
  {
    if(p_pbar->p_seg->p_seg_des != NULL)
    {
      mmi_free_buf((void *)p_pbar->p_seg->p_seg_des);
    }
    mmi_free_buf((void *)p_pbar->p_seg);
  }

  p_pbar->p_seg = mmi_alloc_buf(sizeof(pbar_seg_t));
  MT_ASSERT(p_pbar->p_seg != NULL);

  p_pbar->p_seg->seg_count = p_seg->seg_count;

  p_pbar->p_seg->p_seg_des = mmi_alloc_buf(sizeof(pbar_seg_des_t) * p_pbar->p_seg->seg_count);
  MT_ASSERT(p_pbar->p_seg->p_seg_des != NULL);

  memcpy(p_pbar->p_seg->p_seg_des,
    p_seg->p_seg_des, sizeof(pbar_seg_des_t) * p_pbar->p_seg->seg_count);
}

BOOL pbar_is_hori(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);


  return (BOOL)(p_ctrl->priv_attr & PBAR_DIRECTION_MASK);
}

BOOL pbar_is_continue(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);


  return (BOOL)(p_ctrl->priv_attr & PBAR_CONTINUOUS_MASK);
}

u8 pbar_get_interval(control_t *p_ctrl)
{
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  
  return p_pbar->interval;
}

void pbar_set_workmode(control_t *p_ctrl, u8 continuous, u8 interval)
{
  ctrl_pbar_t *p_pbar = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(continuous != 0)
  {
    p_ctrl->priv_attr |= PBAR_CONTINUOUS_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~PBAR_CONTINUOUS_MASK);
    p_pbar = (ctrl_pbar_t *)p_ctrl;
    p_pbar->interval = interval;
  }
}


BOOL pbar_increase(control_t *p_ctrl)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 index = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  if(p_pbar->curn < p_pbar->max)
  {
    index = _pbar_get_cur_block_index(p_ctrl);
    if(index == (p_pbar->step - 1))
    {
      p_pbar->curn = p_pbar->max;
    }
    else
    {
      p_pbar->curn = _pbar_calc_curn(p_ctrl, (u16)(index + 1));
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    return TRUE;
  }
  return FALSE;
}


BOOL pbar_decrease(control_t *p_ctrl)
{
  ctrl_pbar_t *p_pbar = NULL;
  u16 index = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_pbar = (ctrl_pbar_t *)p_ctrl;
  if(p_pbar->curn > p_pbar->min)
  {
    index = _pbar_get_cur_block_index(p_ctrl);
    if(index == 0)
    {
      p_pbar->curn = p_pbar->min;
    }
    else
    {
      p_pbar->curn = _pbar_calc_curn(p_ctrl, (u16)(index - 1));
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    return TRUE;
  }
  return FALSE;
}


static RET_CODE on_pbar_increase(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  pbar_increase(p_ctrl);
  return SUCCESS;
}


static RET_CODE on_pbar_decrease(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  pbar_decrease(p_ctrl);
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(pbar_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_INCREASE, on_pbar_increase)
ON_COMMAND(MSG_DECREASE, on_pbar_decrease)
END_CTRLPROC(pbar_class_proc, ctrl_default_proc)
