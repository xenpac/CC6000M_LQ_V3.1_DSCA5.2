/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_container.c
   this file  implement the functions defined in  ctrl_container.h .
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

#include "ctrl_container.h"

/*!
  container control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;    
  /*!
    clip region information about container control.
    */
  crgn_info_t *p_clip_info;
}ctrl_cont_t;

static void _cont_draw(control_t *p_ctrl, hdc_t hdc)
{
  gui_paint_frame(hdc, p_ctrl);
}


RET_CODE cont_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_cont_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of container class
  p_default_ctrl->p_proc = cont_class_proc;
  p_default_ctrl->p_paint = _cont_draw;

  // initalize the default data of container class

  if(ctrl_register_ctrl_class(CTRL_CONT, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void cont_move_focus(control_t *p_ctrl, u16 msg)
{
  control_t *p_active = NULL, *p_next_ctrl = p_ctrl;
  u8 next_id = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_active = p_ctrl->p_active_child;
  if(p_active != NULL)
  {
    next_id = ctrl_get_related_id(p_active, msg);
    while((next_id) && (next_id != p_active->id))
    {
      p_next_ctrl = ctrl_get_child_by_id(p_ctrl, next_id);
      if((OBJ_ATTR_ACTIVE == p_next_ctrl->attr) && (p_next_ctrl->sts == OBJ_STS_SHOW))
      {
        break;
      }
      else
      {
        next_id = ctrl_get_related_id(p_next_ctrl, msg);
      }
    }
    if(next_id != p_active->id && next_id != 0)
    {
      ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
      ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);

      // it don't need to redraw, if whole hl is actived
      if(!(ctrl_get_style(p_ctrl) & STL_EX_WHOLE_HL))
      {
        ctrl_process_msg(p_active, MSG_PAINT, 0, 0);
        ctrl_process_msg(p_next_ctrl, MSG_PAINT, 0, 0);
      }
    }
  }
}


crgn_info_t *cont_get_gcrgn_info(control_t *p_ctrl)
{
  ctrl_cont_t *p_cont = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_cont = (ctrl_cont_t *)p_ctrl;

  return p_cont->p_clip_info;
}


void cont_set_gcrgn_info(control_t *p_ctrl, crgn_info_t *p_info)
{
  ctrl_cont_t *p_cont = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_cont = (ctrl_cont_t *)p_ctrl;

  if(ctrl_is_root(p_ctrl)) /* root container */
  {
    p_cont->p_clip_info = p_info;
  }
}


static RET_CODE on_cont_change_focus(control_t *p_ctrl,
                                     u16 msg,
                                     u32 para1,
                                     u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  cont_move_focus(p_ctrl, msg);
  return SUCCESS;
}


// define the default msgmap of class
BEGIN_CTRLPROC(cont_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_FOCUS_UP, on_cont_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_cont_change_focus)
ON_COMMAND(MSG_FOCUS_LEFT, on_cont_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_cont_change_focus)
END_CTRLPROC(cont_class_proc, ctrl_default_proc)
