/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_base.c
   this file  implement the funcs defined in  ctrl_base.h,and some internal used
   functions.
   All these functions are common functions of all control class.
  */

#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"
#include "ctype.h"

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

#include "class_factory.h"

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

static control_class_t *_alloc_a_class(void)
{
  control_lib_t *p_info = NULL;

  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  return (control_class_t *)lib_memf_alloc(&p_info->class_heap);
}

static void _free_a_class(control_class_t *p_class)
{
  control_lib_t *p_info = NULL;

  if(p_class != NULL)
  {
    p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
    MT_ASSERT(p_info != NULL);

    lib_memf_free(&p_info->class_heap, p_class);
  }
}

static control_t *_alloc_a_ctrl(ctrl_type_t ctrl_type)
{
  control_lib_t *p_info = NULL;
  control_class_t *p_oci = NULL;
  
  MT_ASSERT(ctrl_type < CTRL_TCNT);
  
  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  p_oci = p_info->p_class_table[ctrl_type];
  if(p_oci == NULL)
  {
    return NULL;
  }
  
  return lib_memf_alloc(&p_oci->data_heap);
}

static void _free_a_ctrl(control_t *p_ctrl)
{
  control_lib_t *p_info = NULL;
  control_class_t *p_oci = NULL;
  
  if(p_ctrl != NULL)
  {
    p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
    MT_ASSERT(p_info != NULL);

    MT_ASSERT(p_ctrl->ctrl_type < CTRL_TCNT);

    p_oci = p_info->p_class_table[p_ctrl->ctrl_type];

    MT_ASSERT(p_oci != NULL);

    lib_memf_free(&p_oci->data_heap, p_ctrl);
  }
}


static RET_CODE _add_ctrl_class(control_class_t *p_new_class)
{
  control_lib_t *p_info = NULL;

  MT_ASSERT(p_new_class != NULL);

  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_new_class->ctrl_type >= CTRL_TCNT)
  {
    return ERR_CTRL_CTRL_TYPE;
  }

  /* chk for same name */
  if(p_info->p_class_table[p_new_class->ctrl_type] != NULL)
  {
    return ERR_CTRL_CTRL_TYPE;
  }
  
  p_info->p_class_table[p_new_class->ctrl_type] = p_new_class;
  
  return SUCCESS;
}


void ctrl_client2screen(control_t *p_ctrl, rect_t *p_rc)
{
  control_t *p_parent = p_ctrl;

  MT_ASSERT(p_ctrl != NULL && p_rc != NULL);

  // rect is in this control
  if(is_equal_rect(&p_ctrl->frame, p_rc))
  {
    p_parent = p_ctrl->p_parent;
  }

  while(p_parent != NULL)
  {
    offset_rect(p_rc, p_parent->frame.left, p_parent->frame.top);
    p_parent = p_parent->p_parent;
  }
}


BOOL ctrl_screen2client(control_t *p_ctrl, rect_t *p_rc)
{
  rect_t rc_frame;

  ctrl_get_frame(p_ctrl, &rc_frame);
  ctrl_client2screen(p_ctrl, &rc_frame);

  if(!intersect_rect(p_rc, p_rc, &rc_frame))
  {
    return FALSE;
  }

  offset_rect(p_rc,
              (s16)(0 - rc_frame.left),
              (s16)(0 - rc_frame.top));

  return TRUE;
}


void ctrl_get_client_rect(control_t *p_ctrl, rect_t *p_rc)
{
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rc != NULL);
  
  ctrl_get_frame(p_ctrl, p_rc);
  offset_rect(p_rc, (s16)(0 - p_rc->left), (s16)(0 - p_rc->top));
}

void ctrl_get_draw_rect(control_t *p_ctrl, rect_t *p_rc)
{
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rc != NULL);
  
  ctrl_get_client_rect(p_ctrl, p_rc);

  p_rc->left = p_rc->left + p_ctrl->mrect.left;
  p_rc->top = p_rc->top + p_ctrl->mrect.top;
  p_rc->right = p_rc->left + RECTW(p_ctrl->mrect);
  p_rc->bottom = p_rc->top + RECTH(p_ctrl->mrect);  
}

void ctrl_get_frame(control_t *p_ctrl, rect_t *p_rc)
{
  MT_ASSERT(p_ctrl != NULL && p_rc != NULL);
  copy_rect(p_rc, &p_ctrl->frame);
}

void ctrl_get_mrect(control_t *p_ctrl, rect_t *p_rc)
{
  MT_ASSERT(p_ctrl != NULL && p_rc != NULL);
  copy_rect(p_rc, &p_ctrl->mrect);
}

RET_CODE ctrl_init_ctrl(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return SUCCESS;
}


void ctrl_free_ctrl(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
}


control_t *ctrl_create_ctrl(ctrl_type_t ctrl_type,
                            u16 id,
                            u16 x,
                            u16 y,
                            u16 w,
                            u16 h,
                            control_t *p_parent,
                            u32 para)
{
  control_class_t *p_oci = NULL;
  control_t *p_new_ctrl = NULL;
  rect_t rc1, rc2;

/*
   rect_t rc1, rc2;
  */

  if(p_parent != NULL)
  {
    set_rect(&rc1, x, y, (s16)(x + w), (s16)(y + h));
    set_rect(&rc2, 0, 0,
             (s16)(p_parent->frame.right - p_parent->frame.left),
             (s16)(p_parent->frame.bottom - p_parent->frame.top));


    if(is_rect_covered(&rc1, &rc2) == FALSE)
    {
      return NULL;
    }

    if(id != 0)
    {
      if(ctrl_get_child_by_id(p_parent, id) != NULL)
      {
        return NULL;
      }
    }
  }

  p_oci = ctrl_get_ctrl_class(ctrl_type);
  if(NULL == p_oci)
  {
    return NULL;
  }

  p_new_ctrl = _alloc_a_ctrl(ctrl_type);
  if(NULL == p_new_ctrl)
  {
    OS_PRINTF("ERROR! No enough memory for creating a control, increase the control heap\n");
    return NULL;
  }

  memcpy(p_new_ctrl, p_oci->p_default_ctrl, p_oci->data_size);
  p_new_ctrl->ctrl_type = ctrl_type;
  p_new_ctrl->p_oci = p_oci;
  p_new_ctrl->attr = OBJ_ATTR_ACTIVE;
  p_new_ctrl->sts = OBJ_STS_SHOW;

  p_new_ctrl->frame.left = x;
  p_new_ctrl->frame.top = y;
  p_new_ctrl->frame.right = x + w;
  p_new_ctrl->frame.bottom = y + h;

  p_new_ctrl->mrect.left = 0;
  p_new_ctrl->mrect.top = 0;
  p_new_ctrl->mrect.right = w;
  p_new_ctrl->mrect.bottom = h;

  p_new_ctrl->id = id;
  ctrl_add_child(p_parent, p_new_ctrl);

  if(SUCCESS != ctrl_process_msg(p_new_ctrl, MSG_CREATE, (u32)p_parent, 0))
  {
    OS_PRINTF("ERROR! No enough memory for %d control, increase the control max cnt\n",
              ctrl_type);
    ctrl_remove_child(p_parent, p_new_ctrl);
    _free_a_ctrl(p_new_ctrl);
    return NULL;
  }
  else
  {
    ctrl_process_msg(p_new_ctrl, MSG_CREATED, (u32)p_parent, 0);
    ctrl_init_invrgn(p_new_ctrl);

    return p_new_ctrl;
  }
}


BOOL ctrl_destroy_ctrl(control_t *p_ctrl)
{
  control_t *p_parent = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(SUCCESS != ctrl_process_msg(p_ctrl, MSG_DESTROY, 0, 0))
  {
    return FALSE;
  }

  p_parent = p_ctrl->p_parent;
  if(NULL != p_parent)
  {
    if(p_parent->p_active_child == (control_t *)p_ctrl)
    {
      p_parent->p_active_child = NULL;
    }

    if(ctrl_remove_child(p_parent, p_ctrl) == FALSE)
    {
      return FALSE;
    }
  }

  ctrl_empty_invrgn(p_ctrl);
  _free_a_ctrl(p_ctrl);

  return TRUE;
}

obj_sts_t ctrl_get_sts(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->sts;
}


void ctrl_set_sts(control_t *p_ctrl, obj_sts_t new_sts)
{
  control_t *p_child = NULL;
  obj_sts_t old_sts = OBJ_STS_SHOW;

  MT_ASSERT(p_ctrl != NULL);

  old_sts = p_ctrl->sts;

  if(old_sts == new_sts)
  {
    return;
  }

  if((new_sts < OBJ_STS_SHOW) || (new_sts > OBJ_STS_HIDE))
  {
    return;
  }

  //set ctrl's attribute
  p_ctrl->sts = new_sts;

  p_child = p_ctrl->p_child;
  while(p_child != NULL)
  {
    ctrl_set_sts(p_child, new_sts);
    p_child = p_child->p_next;
  }
}

void ctrl_set_sts_ex(control_t *p_ctrl, obj_sts_t new_sts)
{
  obj_sts_t old_sts = OBJ_STS_SHOW;

  MT_ASSERT(p_ctrl != NULL);

  old_sts = p_ctrl->sts;

  if(old_sts == new_sts)
  {
    return;
  }

  if((new_sts < OBJ_STS_SHOW) || (new_sts > OBJ_STS_HIDE))
  {
    return;
  }

  //set ctrl's attribute
  p_ctrl->sts = new_sts;
}

obj_attr_t ctrl_get_attr(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->attr;
}


/*!
   Define attributes
  */
enum
{
  /*!
     Don't set any attributes
    */
  ATTR_NONE,
  /*!
     Set attributes for all childrens
    */
  ATTR_ALL,
  /*!
     Set attributes for the childrens which are actived
    */
  ATTR_ACTIVE_ONLY,
  /*!
     Set attributes to HL for the childrens which are actived
    */
  ATTR_ACTIVE_HL
};

void ctrl_set_attr(control_t *p_ctrl, obj_attr_t new_attr)
{
  control_t *p_child = NULL;
  obj_attr_t old_attr = 0;
  obj_attr_t child_attr = ATTR_NONE;

  MT_ASSERT(p_ctrl != NULL);

  old_attr = p_ctrl->attr;

  if(old_attr == new_attr)
  {
    return;
  }

  if((new_attr < OBJ_ATTR_ACTIVE) || (new_attr > OBJ_ATTR_INACTIVE))
  {
    return;
  }

  //set ctrl's attribute
  p_ctrl->attr = new_attr;

  ctrl_process_msg(p_ctrl, MSG_ATTR_CHANGED, old_attr, new_attr);

  //set children's attribute
  switch(old_attr)
  {
    case OBJ_ATTR_ACTIVE:
      switch(new_attr)
      {
        case OBJ_ATTR_INACTIVE:
          child_attr = ATTR_ALL;
          break;
          
        case OBJ_ATTR_HL:
          child_attr = ATTR_ACTIVE_ONLY;
          break;
          
        default:
          child_attr = ATTR_NONE;
          break;
      }
      break;

    case OBJ_ATTR_INACTIVE:
      switch(new_attr)
      {
        case OBJ_ATTR_ACTIVE:
          child_attr = ATTR_ALL;
          break;

        case OBJ_ATTR_HL:
          child_attr = ATTR_ACTIVE_HL;
          break;

        default:
          child_attr = ATTR_NONE;
          break;
      }
      break;
      
    case OBJ_ATTR_HL:
      switch(new_attr)
      {
        case OBJ_ATTR_INACTIVE:
          child_attr = ATTR_ALL;
          break;

        case OBJ_ATTR_ACTIVE:
          child_attr = ATTR_ACTIVE_ONLY;
          break;

        default:
          child_attr = ATTR_NONE;
          break;
      }
      break;
      
    default:
      break;
  }

  switch(child_attr)
  {
    case ATTR_ALL:
      p_child = p_ctrl->p_child;
      
      while(p_child != NULL)
      {
        ctrl_set_attr(p_child, new_attr);
        p_child = p_child->p_next;
      }
      break;
      
    case ATTR_ACTIVE_ONLY:
      if(p_ctrl->p_active_child != NULL)
      {
        ctrl_set_attr(p_ctrl->p_active_child, new_attr);
      }
      break;
      
    case ATTR_ACTIVE_HL:
      p_child = p_ctrl->p_child;
      
      while(p_child != NULL)
      {
        if(p_child == p_ctrl->p_active_child)
        {
          ctrl_set_attr(p_child, OBJ_ATTR_HL);
        }
        else
        {
          ctrl_set_attr(p_child, new_attr);
        }

        p_child = p_child->p_next;
      }
      break;

    default:
      break;
  }
}


u8 ctrl_get_style(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->style;
}


void ctrl_set_style(control_t *p_ctrl, u8 style)
{
  MT_ASSERT(p_ctrl != NULL);
  p_ctrl->style = style;
}


u32 ctrl_get_context(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->priv_context;
}


void ctrl_set_context(control_t *p_ctrl, u32 context)
{
  MT_ASSERT(p_ctrl != NULL);
  p_ctrl->priv_context = context;
}


msgproc_t ctrl_get_proc(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->p_proc;
}


void ctrl_set_proc(control_t *p_ctrl, msgproc_t p_proc)
{
  MT_ASSERT(p_ctrl != NULL);
  p_ctrl->p_proc = p_proc;
}


keymap_t ctrl_get_keymap(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->p_keymap;
}


void ctrl_set_keymap(control_t *p_ctrl, keymap_t p_keymap)
{
  MT_ASSERT(p_ctrl != NULL);
  p_ctrl->p_keymap = p_keymap;
}

BOOL ctrl_is_topmost(control_t *p_ctrl)
{
  control_t *p_root = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_root = ctrl_get_root(p_ctrl);

  return (BOOL)(p_root->style & STL_EX_TOPMOST);
}

u16 ctrl_translate_key(control_t *p_ctrl, u16 key, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->attr == OBJ_ATTR_INACTIVE)
  {
    return MSG_INVALID;
  }

  if(p_ctrl->p_keymap == NULL)
  {
    return MSG_INVALID;
  }

  return p_ctrl->p_keymap(key);
}


RET_CODE ctrl_process_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->p_proc == NULL)
  {
    return ERR_NOFEATURE;
  }

  return p_ctrl->p_proc(p_ctrl, msg, para1, para2);
}


u16 ctrl_get_ctrl_id(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->id;
}


u16 ctrl_get_related_id(control_t *p_ctrl, u16 msg)
{
  MT_ASSERT(p_ctrl != NULL);

  switch(msg)
  {
    case MSG_FOCUS_UP:
      return p_ctrl->up;

    case MSG_FOCUS_DOWN:
      return p_ctrl->down;

    case MSG_FOCUS_LEFT:
      return p_ctrl->left;

    case MSG_FOCUS_RIGHT:
      return p_ctrl->right;

    default:
      return p_ctrl->id;
  }
}


void ctrl_set_related_id(control_t *p_ctrl, u16 left, u16 up, u16 right, u16 down)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->left = left;
  p_ctrl->up = up;
  p_ctrl->right = right;
  p_ctrl->down = down;
}


control_t *ctrl_get_root(control_t *p_ctrl)
{
  control_t *p_control = p_ctrl;

  MT_ASSERT(p_ctrl != NULL);

  while(NULL != p_control->p_parent)
  {
    p_control = p_control->p_parent;
  }

  return p_control;
}


s16 ctrl_get_width(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->frame.right - p_ctrl->frame.left;
}


s16 ctrl_get_height(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->frame.bottom - p_ctrl->frame.top;
}


void ctrl_set_rstyle(control_t *p_ctrl, u32 sh_style, u32 hl_style, u32 gr_style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->rstyle.show_idx = sh_style;
  p_ctrl->rstyle.hl_idx = hl_style;
  p_ctrl->rstyle.gray_idx = gr_style;
}


BOOL ctrl_is_whole_hl(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->p_parent == NULL)
  {
    return FALSE;
  }

  if((p_ctrl->p_parent->style & STL_EX_WHOLE_HL)
    && (p_ctrl->p_parent->attr == OBJ_ATTR_HL))
  {
    return TRUE;
  }

  return FALSE;
}


BOOL ctrl_is_always_hl(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->style & STL_EX_ALWAYS_HL)
  {
    return TRUE;
  }

  return FALSE;
}


BOOL ctrl_is_onshow(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->sts == OBJ_STS_HIDE)
  {
    return FALSE;
  }

  return TRUE;
}


BOOL ctrl_is_onfocus(control_t *p_ctrl)
{
  control_t *p_focus = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_focus = ctrl_get_root(p_ctrl);
  while(p_focus != NULL)
  {
    if(p_focus == p_ctrl)
    {
      return TRUE;
    }

    p_focus = p_focus->p_active_child;
  }

  return FALSE;
}


BOOL ctrl_is_root(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  if(p_ctrl->p_parent != NULL)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}


control_t *ctrl_get_child_by_id(control_t *p_parent, u16 id)
{
  control_t *p_ctrl = NULL;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(id != 0);

  p_ctrl = p_parent->p_child;
  while((NULL != p_ctrl) && (p_ctrl->id != id))
  {
    p_ctrl = p_ctrl->p_next;
  }


  return p_ctrl;
}

BOOL ctrl_destroy_child_by_id(control_t *p_parent, u16 id)
{
  BOOL b_ret = FALSE;
  control_t *p_ctrl = NULL;

  p_ctrl = ctrl_get_child_by_id (p_parent, id);
  if(NULL == p_ctrl)
  {
      return FALSE;
  }
  b_ret = ctrl_destroy_ctrl(p_ctrl);

  return b_ret;
}

control_t *ctrl_get_parent(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->p_parent;
}


control_t *ctrl_get_first_child(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->p_child;
}


control_t *ctrl_get_active_ctrl(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->p_active_child;
}


void ctrl_set_active_ctrl(control_t *p_ctrl, control_t *p_active_child)
{
  MT_ASSERT(p_ctrl != NULL);

  if(p_active_child == NULL)
  {
    p_ctrl->p_active_child = NULL;
  }
  else if(p_active_child->p_parent == p_ctrl)
  {
    p_ctrl->p_active_child = p_active_child;
  }
}


control_t *ctrl_get_prev_ctrl(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->p_prev;
}


control_t *ctrl_get_next_ctrl(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return p_ctrl->p_next;
}


void ctrl_insert_ctrl(control_t *p_prev, control_t *p_ctrl)
{
  control_t *p_old_next = NULL;

  MT_ASSERT(p_ctrl != NULL && p_prev != NULL);

//  if(!ctrl_is_root(ctrl))
//  {
//    return;
//  }
//  if(!ctrl_is_root(prev))
//  {
//    return;
//  }
  if(p_ctrl->p_parent != NULL)
  {
    return;
  }

  p_old_next = p_prev->p_next;
  p_prev->p_next = p_ctrl;
  p_ctrl->p_prev = p_prev;
  p_ctrl->p_next = p_old_next;

  if(NULL != p_old_next)
  {
    p_old_next->p_prev = p_ctrl;
  }

  p_ctrl->p_parent = p_prev->p_parent;
}


void ctrl_seperate_ctrl(control_t *p_ctrl)
{
  control_t *p_pre_ctrl = NULL, *p_next_ctrl = NULL;

  MT_ASSERT(p_ctrl != NULL);

//  if(!ctrl_is_root(ctrl))
//  {
//    return;
//  }
  p_pre_ctrl = p_ctrl->p_prev;
  p_next_ctrl = p_ctrl->p_next;
  if(NULL != p_pre_ctrl)
  {
    p_pre_ctrl->p_next = p_next_ctrl;
  }
  if(NULL != p_next_ctrl)
  {
    p_next_ctrl->p_prev = p_pre_ctrl;
  }
  p_ctrl->p_prev = NULL;
  p_ctrl->p_next = NULL;

//set new child of parent control.
  if((p_ctrl->p_parent != NULL) && (p_ctrl->p_parent->p_child == p_ctrl))
  {
    if(NULL != p_next_ctrl)
    {
      p_ctrl->p_parent->p_child = p_next_ctrl;
    }
    else if(NULL != p_pre_ctrl)
    {
      p_ctrl->p_parent->p_child = p_pre_ctrl;
    }
    else
    {
      p_ctrl->p_parent->p_child = NULL;
    }
  }
//set new active control of parent control.
  if((p_ctrl->p_parent != NULL)
    && (p_ctrl->p_parent->p_active_child == p_ctrl))
  {
    if(NULL != p_next_ctrl)
    {
      p_ctrl->p_parent->p_active_child = p_next_ctrl;
    }
    else if(NULL != p_pre_ctrl)
    {
      p_ctrl->p_parent->p_active_child = p_pre_ctrl;
    }
    else
    {
      p_ctrl->p_parent->p_active_child = NULL;
    }
  }

  p_ctrl->p_parent = NULL;
}


void ctrl_set_frame(control_t *p_ctrl, rect_t *p_frame)
{
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_frame != NULL);
  
  copy_rect(&p_ctrl->frame, p_frame);
}

void ctrl_set_mrect(control_t *p_ctrl, u16 left, u16 top, u16 right, u16 bottom)
{
  rect_t mrect = {0};
  
  MT_ASSERT(p_ctrl != NULL);

  MT_ASSERT(left >= 0);
  MT_ASSERT(top >= 0);
  MT_ASSERT(right <= RECTW(p_ctrl->frame));
  MT_ASSERT(bottom <= RECTH(p_ctrl->frame));

  mrect.left = left;
  mrect.top = top;
  mrect.right = right;
  mrect.bottom = bottom;

  copy_rect(&p_ctrl->mrect, &mrect);
}

void ctrl_resize(control_t *p_ctrl, rect_t *p_frame)
{
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_frame != NULL);

  ctrl_set_frame(p_ctrl, p_frame);
  ctrl_set_mrect(p_ctrl, 0, 0, RECTWP(p_frame), RECTHP(p_frame));

  ctrl_empty_invrgn(p_ctrl);
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
}


void ctrl_paint_ctrl(control_t *p_ctrl, BOOL is_force)
{
  MT_ASSERT(p_ctrl != NULL);

  if(is_force == TRUE)
  {
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  }
  ctrl_process_msg(p_ctrl, MSG_PAINT, 0, 0);
}

//Note : the frame of ctrl must in the frame of parent
BOOL ctrl_hide_ctrl(control_t *p_ctrl)
{
  control_t *p_parent = NULL;
  rect_t rc, rc_parent;

  MT_ASSERT(p_ctrl != NULL);

  p_parent = ctrl_get_root(p_ctrl);
  MT_ASSERT(NULL != p_parent);
  ctrl_get_frame(p_parent, &rc_parent);

  ctrl_get_frame(p_ctrl, &rc);
  ctrl_client2screen(p_ctrl, &rc);

  if(!intersect_rect(&rc, &rc, &rc_parent))
  {
    return FALSE;
  }

  ctrl_screen2client(p_parent, &rc);
  ctrl_add_rect_to_invrgn(p_parent, &rc);

  //ctrl_process_msg(p_parent, MSG_PAINT, 0, 0);

  return TRUE;
}


//Note : the frame of ctrl must in the frame of parent
BOOL ctrl_erase_ctrl(control_t *p_ctrl)
{
  control_t *p_parent = NULL;
  rect_t rc, rc_parent;

  MT_ASSERT(p_ctrl != NULL);

  p_parent = ctrl_get_root(p_ctrl);
  MT_ASSERT(NULL != p_parent);
  ctrl_get_frame(p_parent, &rc_parent);

  ctrl_get_frame(p_ctrl, &rc);
  ctrl_client2screen(p_ctrl, &rc);

  if(!intersect_rect(&rc, &rc, &rc_parent))
  {
    return FALSE;
  }

  ctrl_screen2client(p_parent, &rc);
  ctrl_add_rect_to_invrgn(p_parent, &rc);

  ctrl_process_msg(p_parent, MSG_PAINT, 0, 0);
  return TRUE;
}


BOOL ctrl_move_ctrl(control_t *p_ctrl, s16 x, s16 y)
{
  rect_t rc = {0};
  rect_t rc1 = {0};
  control_t *p_parent = NULL;

  MT_ASSERT(p_ctrl != NULL);

  copy_rect(&rc, &p_ctrl->frame);
  offset_rect(&rc, x, y);

  p_parent = ctrl_get_parent(p_ctrl);
  if(p_parent != NULL)
  {
    rect_t rc_parent;
    rect_t rc_temp;
    ctrl_get_frame(p_parent, &rc_parent);

    rc_temp.left = 0;
    rc_temp.top = 0;
    rc_temp.right = rc_parent.right - rc_parent.left;
    rc_temp.bottom = rc_parent.bottom - rc_parent.top;

    if(is_rect_covered(&rc, &rc_temp) == FALSE)
    {
      return FALSE;
    }

    copy_rect(&rc1, &p_ctrl->frame);
    //offset_rect(&rc1, rc_parent.left, rc_parent.top);
    ctrl_add_rect_to_invrgn(p_parent, &rc1);  
    ctrl_add_rect_to_invrgn(p_parent, &rc);  
  }

  copy_rect(&p_ctrl->frame, &rc);

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  return TRUE;
}


void ctrl_paint_child(control_t *p_ctrl, control_t *p_sub_ctrl, BOOL is_force)
{
  control_t *p_parent = NULL;

  MT_ASSERT(p_ctrl != NULL && p_sub_ctrl != NULL);

  p_parent = p_sub_ctrl->p_parent;
  while(p_parent != NULL)
  {
    if(p_parent == p_ctrl)
    {
      ctrl_paint_ctrl(p_sub_ctrl, is_force);
      break;
    }

    p_parent = p_parent->p_parent;
  }
}


void ctrl_paint_children(control_t *p_ctrl, hdc_t hdc)
{
  control_t *p_child = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_child = p_ctrl->p_child;
  while(p_child != NULL)
  {
    ctrl_process_msg(p_child, MSG_PAINT, 0, hdc);
    p_child = p_child->p_next;
  }
}


control_class_t *ctrl_get_ctrl_class(ctrl_type_t ctrl_type)
{
  control_lib_t *p_info = NULL;

  MT_ASSERT(ctrl_type < CTRL_TCNT);
  
  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  return p_info->p_class_table[ctrl_type];
}


RET_CODE ctrl_link_ctrl_class_buf(control_class_register_info_t *p_info)
{
  MT_ASSERT(p_info != NULL);
  MT_ASSERT(p_info->data_size > 0);

  p_info->p_default_ctrl = (control_t *)mmi_alloc_buf(p_info->data_size);
  MT_ASSERT(p_info->p_default_ctrl != NULL);
  memset(p_info->p_default_ctrl, 0, p_info->data_size);

  return SUCCESS;
}


RET_CODE ctrl_unlink_ctrl_class_buf(control_class_register_info_t *p_info)
{
  MT_ASSERT(p_info != NULL);

  if(p_info->p_default_ctrl != NULL)
  {
    mmi_free_buf(p_info->p_default_ctrl);
    p_info->p_default_ctrl = NULL;
  }

  return SUCCESS;
}


RET_CODE ctrl_register_ctrl_class(ctrl_type_t ctrl_type, control_class_register_info_t *p_info)
{
  control_class_t *p_new_class = NULL;

  MT_ASSERT(p_info != NULL
           && p_info->max_cnt > 0);

  p_new_class = _alloc_a_class();
  MT_ASSERT(p_new_class != NULL);
  memset(p_new_class, 0, sizeof(control_class_t));

  p_new_class->ctrl_type = ctrl_type;

  if(_add_ctrl_class(p_new_class) != SUCCESS)
  {
    _free_a_class(p_new_class);
    return ERR_FAILURE;
  }

  // set class info
  p_new_class->data_size = p_info->data_size;
  p_new_class->p_data_heap_addr =
    mmi_create_memf(&p_new_class->data_heap, p_info->max_cnt, p_info->data_size);
  MT_ASSERT(p_new_class->p_data_heap_addr != NULL);

  p_new_class->p_default_ctrl = p_info->p_default_ctrl;

  return SUCCESS;
}


RET_CODE ctrl_unregister_ctrl_class(ctrl_type_t ctrl_type)
{
  control_lib_t *p_info = NULL;
  control_class_t *p_oci = NULL;

  MT_ASSERT(ctrl_type < CTRL_TCNT);
  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_oci = p_info->p_class_table[ctrl_type];

  mmi_destroy_memf(&p_oci->data_heap, p_oci->p_data_heap_addr);

  _free_a_class(p_oci);
  
  return SUCCESS;
}


BOOL ctrl_init_lib(u16 max_class_cnt, u16 max_ctrl_cnt)
{
  class_handle_t p_handle = NULL;
  control_lib_t *p_info = NULL;
  u32 slice = 0;

  MT_ASSERT(max_class_cnt * max_ctrl_cnt != 0);

  p_handle = (void *)mmi_alloc_buf(sizeof(control_lib_t));
  MT_ASSERT(p_handle != NULL);

  memset((void *)p_handle, 0, sizeof(control_lib_t));
  class_register(GUI_CLASS_ID, p_handle);

  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  slice = sizeof(control_class_t);
  p_info->p_class_heap_addr = mmi_create_memf(&p_info->class_heap, CTRL_TCNT, slice);
  MT_ASSERT(p_info->p_class_heap_addr != NULL);

  return TRUE;
}


void ctrl_release_lib(void)
{
  control_lib_t *p_info = NULL;

  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  mmi_destroy_memf(&p_info->class_heap, p_info->p_class_heap_addr);
  p_info->p_class_heap_addr = NULL;
  
  mmi_free_buf(p_info);
}

ctrl_type_t ctrl_get_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return p_ctrl->ctrl_type;
}

void ctrl_set_bg(control_t *p_ctrl, u16 ctrl_bg)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->ctrl_bg = ctrl_bg;
}

u16 ctrl_get_bg(control_t *p_ctrl)
{  
  MT_ASSERT(p_ctrl != NULL);

  return p_ctrl->ctrl_bg;
}

void ctrl_get_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_rstyle)
{
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_rstyle != NULL);
  
  p_rstyle->gray_idx = p_ctrl->rstyle.gray_idx;
  p_rstyle->hl_idx = p_ctrl->rstyle.hl_idx;
  p_rstyle->show_idx = p_ctrl->rstyle.show_idx;
  
  return; 
}

RET_CODE ctrl_on_msg(control_t *p_ctrl,
                     const msgmap_entry_t *p_entry,
                     u16 msg,
                     u32 para1,
                     u32 para2)
{
  u16 target = 0;
  RET_CODE ret = ERR_NOFEATURE;

  if(p_entry != NULL)
  {
    if(msg < MSG_SHIFT_DEFAULT_BEGIN
      || msg > MSG_SHIFT_EXTERN_END)
    {
      // normal msg
      target = msg;
      CTRL_PRINTF("ctrl_on_msg: get a normal msg [0x%x] <----------\n", target);
    }
    else
    {
      // shift msg
      target = msg & MSG_TYPE_MASK;
      CTRL_PRINTF("ctrl_on_msg: get a shfit msg [0x%x] <----------\n", target);
    }

    while(p_entry->msg != MSG_INVALID)
    {
      if(p_entry->msg == target)
      {
        ret = p_entry->p_proc(p_ctrl, msg, para1, para2);
        break;
      }
      p_entry++;
    }
  }
  return ret;
}


u16 ctrl_on_key(const keymap_entry_t *p_entry, u16 key)
{
  u16 msg = MSG_INVALID;

  if(p_entry != NULL)
  {
    while(p_entry->key != 0 /* V_KEY_INVALID */)
    {
      if(p_entry->key == key)
      {
        msg = p_entry->msg;
        break;
      }
      p_entry++;
    }
  }
  return msg;
}


static RET_CODE on_ctrl_create(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  return ctrl_init_ctrl(p_ctrl);
}


static RET_CODE on_ctrl_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  ctrl_destroy_children(p_ctrl);
  ctrl_free_ctrl(p_ctrl);

  return SUCCESS;
}


static RET_CODE on_ctrl_paint(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL is_force = (BOOL)(para1);
  hdc_t hdc = 0, parent_dc = (hdc_t)para2;
  BOOL is_painted = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  if(is_force)
  {
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  }

  if(!ctrl_is_onshow(p_ctrl))
  {
    return SUCCESS;
  }

  hdc = gui_begin_paint(p_ctrl, parent_dc);
  if(HDC_INVALID == hdc)
  {
    return ERR_FAILURE;
  }

  if(!gdi_is_empty_cliprgn(&p_ctrl->invrgn_info.crgn))
  {
    if(p_ctrl->p_paint != NULL)
    {
      p_ctrl->p_paint(p_ctrl, hdc);
      is_painted = TRUE;
    }
  }

  ctrl_paint_children(p_ctrl, hdc);
  gui_end_paint(p_ctrl, hdc);

  if(!is_painted)
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


static RET_CODE on_ctrl_get_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_parent = NULL, *p_child = NULL;

  if(p_ctrl->attr == OBJ_ATTR_ACTIVE)
  {
    p_ctrl->attr = OBJ_ATTR_HL;
    p_parent = p_ctrl->p_parent, p_child = p_ctrl;
    while(p_parent != NULL)
    {
      p_parent->p_active_child = p_child;
      ctrl_process_msg(p_parent, MSG_GETFOCUS, 0, 0);


      p_child = p_parent;
      p_parent = p_parent->p_parent;
    }
    p_child = p_ctrl->p_active_child;
    while(p_child != NULL)
    {
      ctrl_process_msg(p_child, MSG_GETFOCUS, 0, 0);
      p_child = p_child->p_active_child;
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  }

  return SUCCESS;
}


static RET_CODE on_ctrl_lost_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_child = NULL;

  if(p_ctrl->attr == OBJ_ATTR_HL)
  {
    p_ctrl->attr = OBJ_ATTR_ACTIVE;
    p_child = p_ctrl->p_active_child;

    while(NULL != p_child)
    {
      ctrl_process_msg(p_child, MSG_LOSTFOCUS, 0, 0);
      p_child = p_child->p_active_child;
    }
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  }

  return SUCCESS;
}



BEGIN_MSGPROC(ctrl_default_proc, NULL)
ON_COMMAND(MSG_PAINT, on_ctrl_paint)
ON_COMMAND(MSG_GETFOCUS, on_ctrl_get_focus)
ON_COMMAND(MSG_LOSTFOCUS, on_ctrl_lost_focus)
ON_COMMAND(MSG_CREATE, on_ctrl_create)
ON_COMMAND(MSG_DESTROY, on_ctrl_destroy)
END_MSGPROC(ctrl_default_proc, NULL)
