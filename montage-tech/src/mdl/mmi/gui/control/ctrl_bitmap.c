/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_bitmap.c
   this file  implement the funcs defined in  ctrl_bitmap.h,and some internal
   used functions.
   These functions are about register,set feature and draw a bitmap control.
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

#include "ctrl_bitmap.h"

/*!
  bitmap control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;
  /*!
     Bitmap id or bitmap data pointer, decided by the data mode of bitmap
     control
    */
  u32 data;
  /*!
     Left interval, distance to left side
    */
  u16 left;
  /*!
     Top interval, distance to top side
    */
  u16 top;  
}ctrl_bmap_t;

static void _bmap_draw(control_t *p_ctrl, hdc_t hdc)
{
  rect_t bmap_rect;
  ctrl_bmap_t *p_bmap = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(ctrl_is_rolling(p_ctrl))
  {
    return;
  }

  gui_paint_frame(hdc, p_ctrl);
  //draw bitmap
  p_bmap = (ctrl_bmap_t *)p_ctrl;
  ctrl_get_draw_rect(p_ctrl, &bmap_rect);

  if(p_ctrl->priv_attr & BMAP_DATAMODE_MASK)
  {
    u16 left = 0, top = 0;
    bitmap_t *p_bmp = (bitmap_t *)p_bmap->data;
    rect_t temp;
    gui_get_posi(&bmap_rect, p_ctrl->priv_attr, p_bmap->left,
                 p_bmap->top,
                 (u16)(p_bmp->width), (u16)(p_bmp->height), &left,
                 &top);
    set_rect(&temp, left, top, (s16)(left + p_bmp->width),
             (s16)(top + p_bmp->height));

    gdi_fill_bitmap(hdc, &temp, p_bmap->data, 0);
  }
  else
  {
    gui_draw_picture(hdc, &bmap_rect, p_ctrl->priv_attr, p_bmap->left,
                     p_bmap->top,
                     (u16)p_bmap->data);
  }
}


RET_CODE bmap_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_bmap_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of bitmap class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER;
  p_default_ctrl->p_proc = bmap_class_proc;
  p_default_ctrl->p_paint = _bmap_draw;

  // initalize the default data of bitmap class

  if(ctrl_register_ctrl_class(CTRL_BMAP, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void bmap_set_bmap_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_bmap_t *p_bmap = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_bmap = (ctrl_bmap_t *)p_ctrl;
  p_bmap->left = left;
  p_bmap->top = top;
}

void bmap_get_bmap_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top)
{
  ctrl_bmap_t *p_bmap = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_left != NULL);
  MT_ASSERT(p_top != NULL);

  p_bmap = (ctrl_bmap_t *)p_ctrl;

  *p_left = p_bmap->left;
  *p_top = p_bmap->top;
}


void bmap_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~BMAP_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & BMAP_ALIGN_MASK); //set new style
}

u32 bmap_get_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & BMAP_ALIGN_MASK);
}

void bmap_set_content_by_id(control_t *p_ctrl, u16 bmap_id)
{
  ctrl_bmap_t *p_bmap = NULL;

  MT_ASSERT(p_ctrl != NULL);

//  if(p_ctrl->priv_attr & BMAP_DATAMODE_MASK)
//  {
//    return;
//  }
  MT_ASSERT(!(p_ctrl->priv_attr & BMAP_DATAMODE_MASK));

  p_bmap = (ctrl_bmap_t *)p_ctrl;
  p_bmap->data = bmap_id;
}


void bmap_set_content_by_data(control_t *p_ctrl, bitmap_t *p_bmp)
{
  ctrl_bmap_t *p_bmap = NULL;

  MT_ASSERT(p_ctrl != NULL);

//  if(!(p_ctrl->priv_attr & BMAP_DATAMODE_MASK))
//  {
//    return;
//  }
  MT_ASSERT((p_ctrl->priv_attr & BMAP_DATAMODE_MASK));

  p_bmap = (ctrl_bmap_t *)p_ctrl;
  p_bmap->data = (u32)p_bmp;
}


u32 bmap_get_content(control_t *p_ctrl)
{
  ctrl_bmap_t *p_bmap = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_bmap = (ctrl_bmap_t *)p_ctrl;
  return (u32)(p_bmap->data);
}


void bmap_set_content_type(control_t *p_ctrl, BOOL enable)
{
  MT_ASSERT(p_ctrl != NULL);

  if(enable)
  {
    p_ctrl->priv_attr |= BMAP_DATAMODE_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~BMAP_DATAMODE_MASK);
  }
}

BOOL bmap_get_content_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (BOOL)(p_ctrl->priv_attr & BMAP_DATAMODE_MASK);
}

static RET_CODE on_bmap_roll_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_bmap_t *p_bmap = NULL;
  rsurf_ceate_t rsurf = {0};
    
  p_bmap = (ctrl_bmap_t *)p_ctrl;

  ctrl_get_draw_rect(p_ctrl, &rsurf.rect);

  rsurf.pic_data = bmap_get_content(p_ctrl);

  rsurf.is_pic = TRUE;
  
  rsurf.is_pic_rawdata = bmap_get_content_type(p_ctrl);

  rsurf.p_ctrl = p_ctrl;

  rsurf.context = 0;

  rsurf.is_topmost = ctrl_is_topmost(p_ctrl);
  
  gui_create_rsurf(&rsurf, (roll_param_t *)para1);
  
  return SUCCESS;;
}

static RET_CODE on_bmap_rolling(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  hdc_t hdc = 0;  

  MT_ASSERT(p_ctrl != NULL);

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  hdc = gui_begin_paint(p_ctrl, 0);
  gui_paint_frame(hdc, p_ctrl);  

  gui_rolling_node((void *)para1, hdc);

  gui_end_paint(p_ctrl, hdc);  
  
  return SUCCESS;;
}

static RET_CODE on_bmap_roll_next(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{ 
  return SUCCESS;;
}

// define the default msgmap of class
BEGIN_CTRLPROC(bmap_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_START_ROLL, on_bmap_roll_start)
  ON_COMMAND(MSG_ROLLING, on_bmap_rolling)
  ON_COMMAND(MSG_ROLL_NEXT, on_bmap_roll_next) 
END_CTRLPROC(bmap_class_proc, ctrl_default_proc)

