/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

#include "class_factory.h"
#include "mem_manager.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "common.h"
//#include "osd.h"
//#include "gpe.h"
#include "gpe_vsb.h"

#include "mdl.h"

#include "surface.h"
#include "flinger.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_container.h"

#include "ap_framework.h"
#include "framework.h"
#include "fw_common.h"

/* first(and its next) is affected, node is affector */
static void _op_clip_list_by_node(znode_t *p_first, znode_t *p_node)
{
  znode_t *p_affected = NULL;
  rect_t rc;

  if(p_first == NULL || p_node == NULL)
  {
    return;
  }

  /* don't need convert coordinate, the root container is base on the screen */
  ctrl_get_frame(p_node->p_host->p_root, &rc);
  if(is_empty_rect(&rc))
  {
    return;
  }

  p_affected = p_first;
  while(p_affected != NULL)
  {
    if(ctrl_get_sts(p_affected->p_host->p_root) != OBJ_STS_HIDE)
    {
      znode_subtract_gcrgn(p_affected, &rc);
    }

    p_affected = p_affected->p_next;
  }
}


/* node is affected, first (and its nexts) is affector */
static void _op_clip_node_by_list(znode_t *p_node, znode_t *p_first)
{
  znode_t *p_affector = NULL;
  control_t *p_root = NULL;
  rect_t rc;

  if(p_first == NULL || p_node == NULL)
  {
    return;
  }

  /* don't need convert coordinate, the root container is base on the screen */
  ctrl_get_frame(p_node->p_host->p_root, &rc);
  if(is_empty_rect(&rc))
  {
    return;
  }

  p_affector = p_first;
  while(p_affector != NULL)
  {
    if(p_affector == p_node)  /* no one is above it*/
    {
      break;
    }
    p_root = p_affector->p_host->p_root;
    if(ctrl_get_sts(p_root) != OBJ_STS_HIDE)
    {
      /* don't need convert coordinate, the root container is base on the screen
        */
      ctrl_get_frame(p_root, &rc);
      znode_subtract_gcrgn(p_node, &rc);
    }

    p_affector = p_affector->p_next;
  }
}


void znode_init_gcrgn(znode_t *p_node)
{
  rect_t rc;
  mainwin_t *p_win = p_node->p_host;

  /* don't need convert coordinate, the root container is base on the screen */
  ctrl_get_frame(p_win->p_root, &rc);

#ifdef MUTI_THREAD_SUPPORT
  os_mutex_lock(p_win->gcrgn_info.lock);
#endif
  gdi_empty_cliprgn(&p_win->gcrgn_info.crgn);
  gdi_set_cliprgn(&p_win->gcrgn_info.crgn, &rc);
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_unlock(p_win->gcrgn_info.lock);
#endif
}


void znode_subtract_gcrgn(znode_t *p_node, rect_t *p_rc)
{
  mainwin_t *p_win = p_node->p_host;

#ifdef MUTI_THREAD_SUPPORT
  os_mutex_lock(p_win->gcrgn_info.lock);
#endif
  gdi_subtract_cliprect(&p_win->gcrgn_info.crgn, p_rc);
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_unlock(p_win->gcrgn_info.lock);
#endif
}


BOOL znode_is_topmost(znode_t *p_node)
{
  return (BOOL)(p_node->p_host->style & STL_EX_TOPMOST);
}


void znode_set_style(znode_t *p_node, obj_attr_t new_style)
{
  mainwin_t *p_win = p_node->p_host;
  obj_attr_t old_style = ctrl_get_attr(p_win->p_root);

  if(old_style == new_style)
  {
    return;
  }

  ctrl_set_attr(p_win->p_root, new_style);

  if(((old_style == OBJ_ATTR_ACTIVE) && (new_style == OBJ_ATTR_HL))
    || ((old_style == OBJ_ATTR_HL) && (new_style == OBJ_ATTR_ACTIVE)))
  {
    return;
  }

  ctrl_add_rect_to_invrgn(p_win->p_root, NULL);
  p_node->is_need_update = TRUE;
}


void zlist_add_node(zlist_t *p_list, znode_t *p_node)
{
  p_node->p_prev = NULL;
  p_node->p_next = p_list->p_top;
  p_list->p_top = p_node;

  if(p_node->p_next != NULL)
  {
    p_node->p_next->p_prev = p_node;
  }

  p_list->cnt++;
}


void zlist_remove_node(zlist_t *p_list, znode_t *p_node)
{
  znode_t *p_tmp = NULL;

  if(p_list->cnt == 0)
  {
    return;
  }

  if(p_node == p_list->p_top)
  {
    p_list->p_top = p_list->p_top->p_next;
    if(p_list->p_top != NULL)
    {
      p_list->p_top->p_prev = NULL;
    }
    p_list->cnt--;
    return;
  }
  else
  {
    p_tmp = p_list->p_top->p_next;
  }

  while(p_tmp != NULL && p_tmp != p_node)
  {
    p_tmp = p_tmp->p_next;
  }

  if(p_tmp != NULL)
  {
    p_tmp->p_prev->p_next = p_tmp->p_next;

    if(p_tmp->p_next != NULL)
    {
      p_tmp->p_next->p_prev = p_tmp->p_prev;
    }
    
    p_tmp->p_prev = p_tmp->p_next = NULL;
    p_list->cnt--;
  }
}


znode_t *zlist_get_the_last(zlist_t *p_list)
{
  znode_t *p_node = NULL;

  if(p_list->cnt == 0
    || p_list->p_top == NULL)
  {
    return NULL;
  }

  p_node = p_list->p_top;
  while(p_node->p_next != NULL)
  {
    p_node = p_node->p_next;
  }

  return p_node;
}


control_t *zlist_get_the_root(znode_t *p_node)
{
  if(p_node->p_host != NULL)
  {
    return p_node->p_host->p_root;
  }

  return NULL;
}


znode_t *zlist_get_the_first_activable(zlist_t *p_list)
{
  znode_t *p_node = NULL;
  control_t *p_root = NULL;

  if(p_list->cnt == 0
    || p_list->p_top == NULL)
  {
    return NULL;
  }

  p_node = p_list->p_top;
  while(p_node != NULL)
  {
    if(p_node->p_host != NULL)
    {
      p_root = p_node->p_host->p_root;

      if((ctrl_get_attr(p_root) != OBJ_ATTR_INACTIVE)
        && (ctrl_get_sts(p_root) != OBJ_STS_HIDE))
      {
        return p_node;
      }
    }

    p_node = p_node->p_next;
  }

  /* it's impossible because the desktop container is still work */
  return NULL;
}


znode_t *zlist_find_node_by_node(zlist_t *p_list, znode_t *p_node)
{
  znode_t *p_tmp = NULL;

  p_tmp = p_list->p_top;
  while(NULL != p_tmp)
  {
    if(p_tmp == p_node)
    {
      return p_tmp;
    }

    p_tmp = p_tmp->p_next;
  }

  return NULL;
}


znode_t *zlist_find_node_by_mainwin(zlist_t *p_list, mainwin_t *p_win)
{
  znode_t *p_node = NULL;

  p_node = p_list->p_top;
  while(NULL != p_node)
  {
    if(p_node->p_host == p_win)
    {
      return p_node;
    }

    p_node = p_node->p_next;
  }

  return NULL;
}


znode_t *zlist_find_node_by_root(zlist_t *p_list, control_t *p_root)
{
  znode_t *p_node = NULL;
  mainwin_t *p_win = NULL;

  p_node = p_list->p_top;
  while(NULL != p_node)
  {
    if((p_win = p_node->p_host) != NULL)
    {
      if(p_win->p_root == p_root)
      {
        return p_node;
      }
    }

    p_node = p_node->p_next;
  }

  return NULL;
}


znode_t *zlist_find_node_by_id(zlist_t *p_list, u16 root_id)
{
  znode_t *p_node = NULL;
  mainwin_t *p_win = NULL;
  control_t *p_root = NULL;

  p_node = p_list->p_top;
  while(NULL != p_node)
  {
    if((p_win = p_node->p_host) != NULL)
    {
      if((p_root = p_win->p_root) != NULL)
      {
        if(p_root->id == root_id)
        {
          return p_node;
        }
      }
    }

    p_node = p_node->p_next;
  }

  return NULL;
}


void zlist_clip_list_by_node(zlist_t *p_list, znode_t *p_node)
{
  znode_t *p_first = NULL;

  if(p_node->p_host == NULL
    || p_list->p_top == NULL
    || p_list->cnt == 0)
  {
    return;
  }

  /* if the node is in the list then clip those nodes which under the node */
  if(zlist_find_node_by_node(p_list, p_node) != NULL)
  {
    p_first = p_node->p_next;
  }
  else
  {
    p_first = p_list->p_top;
  }

  _op_clip_list_by_node(p_first, p_node);
}


void zlist_clip_under_this_node(znode_t *p_node)
{
  if(p_node->p_host == NULL)
  {
    return;
  }

  _op_clip_list_by_node(p_node->p_next, p_node);
}


void zlist_clip_node_by_list(znode_t *p_node, zlist_t *p_list)
{
  if(p_node->p_host == NULL
    || p_list->p_top == NULL
    || p_list->cnt == 0)
  {
    return;
  }

  _op_clip_node_by_list(p_node, p_list->p_top);
}


/*
  * it is must that the specified node is in the specified list.
  */
void zlist_clip_node_by_all_above_this(znode_t *p_node, zlist_t *p_list)
{
  if(p_node->p_host == NULL
    || p_list->p_top == NULL
    || p_list->cnt == 0)
  {
    return;
  }

  _op_clip_node_by_list(p_node, p_list->p_top);
}
