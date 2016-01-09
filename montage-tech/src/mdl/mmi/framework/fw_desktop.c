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

//#include "osd.h"
//#include "gpe.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"

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

#define FW_MSG_DEBUG

#ifdef  FW_MSG_DEBUG
#define FW_MSG_PRINT   OS_PRINTF
#else
#define FW_MSG_PRINT   DUMMY_PRINTF
#endif

#define FW_DESKTOP_PID    0xFF // specail for create mainwindow

void _add_child(mainwin_t *p_parent, mainwin_t *p_child)
{
  mainwin_t *p_first_child = NULL, *p_last_child = NULL;

  MT_ASSERT(p_child != NULL && p_parent != NULL);

  p_first_child = p_parent->p_child;

  p_child->p_parent = p_parent;
  p_child->p_next = NULL;

  if(p_first_child == NULL)
  {
    p_parent->p_child = p_child;
    p_child->p_prev = NULL;
  }
  else
  {
    p_last_child = p_first_child;

    while(p_last_child->p_next != NULL)
    {
      p_last_child = p_last_child->p_next;
    }

    p_last_child->p_next = p_child;
    p_child->p_prev = p_last_child;
  }
}


BOOL _remove_child(mainwin_t *p_parent, mainwin_t *p_child)
{
  mainwin_t *p_first_child = NULL;
  BOOL is_found = FALSE;

  MT_ASSERT(p_child != NULL && p_parent != NULL);

  p_first_child = p_parent->p_child;

  if(NULL == p_first_child)
  {
    return FALSE;
  }
  else
  {
    if(p_first_child == p_child)
    {
      p_parent->p_child = p_child->p_next;

      if(p_child->p_next != NULL)
      {
        p_child->p_next->p_prev = NULL;
      }

      is_found = TRUE;
    }
    else
    {
      while(p_first_child->p_next != NULL)
      {
        if(p_first_child->p_next == p_child)
        {
          p_first_child->p_next = p_child->p_next;

          if(p_first_child->p_next != NULL)
          {
            p_first_child->p_next->p_prev = p_child->p_prev;
          }
          is_found = TRUE;
          break;
        }

        p_first_child = p_first_child->p_next;
      }
    }
  }

  if(is_found)
  {
    p_child->p_parent = NULL;
  }

  return is_found;
}


void _link_desktop(mainwin_t *p_win)
{
  fw_main_t *p_info = NULL;

  MT_ASSERT(p_win != NULL);

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_info->infor.p_first_hosted == NULL);
  p_info->infor.p_first_hosted = p_win;
}


void _unlink_desktop(mainwin_t *p_win)
{
  fw_main_t *p_info = NULL;

  MT_ASSERT(p_win != NULL);
  MT_ASSERT(p_win->p_prev == NULL
           && p_win->p_next == NULL);

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_info->infor.p_first_hosted = NULL;
}


static void _set_focus(znode_t *p_focus)
{
  u16 old_id = 0, new_id = 0;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_info->infor.p_focus != NULL)
  {
    old_id = p_info->infor.p_focus->p_host->p_root->id;
  }
  else
  {
    old_id = ROOT_ID_INVALID;
  }

  if(p_focus)
  {
    new_id = p_focus->p_host->p_root->id;
  }
  else
  {
    new_id = ROOT_ID_INVALID;
  }

  p_info->infor.p_focus = p_focus;

  FW_MANAGEMENT(WINDOW_MANAGE_FOCUS_CHANGED, old_id, new_id);
}

#if 0
static void _recalc_invrgns_by_rect(znode_t *p_affected, rect_t *p_rc_node)
{
  cliprgn_t nrgn;
  rect_t crc;
  control_t *p_root = NULL;

  p_root = p_affected->p_host->p_root;
  crc = *p_rc_node;

  gdi_init_cliprgn(&nrgn, gdi_get_cliprc_heap());
  gdi_copy_cliprgn(&nrgn, &p_affected->p_host->gcrgn_info.crgn);
  if(gdi_intersect_cliprect(&nrgn, &crc))
  {
    /* convert to client coordinate */
    ctrl_screen2client(p_root, &crc);
    ctrl_add_rect_to_invrgn(p_root, &crc);

    p_affected->is_need_update = TRUE;
  }
  gdi_empty_cliprgn(&nrgn);
}
#else
static void _recalc_invrgns_by_rect(znode_t *p_affected, rect_t *p_rc_node)
{
  rect_t crc, orc;
  control_t *p_root = NULL;

  p_root = p_affected->p_host->p_root;
  crc = *p_rc_node;

  ctrl_get_frame(p_root, &orc);
  ctrl_client2screen(p_root, &orc);

  if(intersect_rect(&orc, &orc, &crc))
  {
    ctrl_screen2client(p_root, &orc);
    ctrl_add_rect_to_invrgn(p_root, &orc);

    p_affected->is_need_update = TRUE;
  }
}
#endif

/* first (and its nexts) is affecteds, node is affector */
static void _op_clip_and_recalc_invrgns(znode_t *p_first, znode_t *p_node)
{
  znode_t *p_affected = NULL;
  control_t *p_root = NULL;

  BOOL is_topmost = FALSE;
  rect_t rc_node;

  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_first == NULL
    || p_node == NULL)
  {
    return;
  }

  /* don't need convert coordinate, the root container is base on the screen */
  ctrl_get_frame(p_node->p_host->p_root, &rc_node);
  if(is_empty_rect(&rc_node))
  {
    return;
  }

  is_topmost = znode_is_topmost(p_first);

  p_affected = p_first;
  while(p_affected != NULL)
  {
    p_root = p_affected->p_host->p_root;

    if(ctrl_get_sts(p_root) != OBJ_STS_HIDE)
    {
      /* initalize, again */
      znode_init_gcrgn(p_affected);

      if(is_topmost)
      {
        /* clip node by all above it */
        zlist_clip_node_by_all_above_this(p_affected,
                                          &p_info->infor.zl_topmost);
      }
      else
      {
        /* clip node by topmost list */
        zlist_clip_node_by_list(p_affected,
                                &p_info->infor.zl_topmost);
        /* clip node by all above it */
        zlist_clip_node_by_all_above_this(p_affected,
                                          &p_info->infor.zl_normal);
      }

      /* recalc by this node */
      _recalc_invrgns_by_rect(p_affected, &rc_node);
    }

    p_affected = p_affected->p_next;
  }
}


static void _recalc_gcrgns_by_node(zlist_t *p_list, znode_t *p_node)
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

  _op_clip_and_recalc_invrgns(p_first, p_node);
}


static void _recalc_gcrgns_under_this_node(znode_t *p_node)
{
  if(p_node->p_host == NULL)
  {
    return;
  }

  _op_clip_and_recalc_invrgns(p_node->p_next, p_node);
}


/* it is not necessary to update the invalid rgn for every affected node . */
static void _recalc_gcrgns_on_add(znode_t *p_node)
{
  mainwin_t *p_win = p_node->p_host;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_win->style & STL_EX_TOPMOST)
  {
    /* use the node to clip others which is under this node */
    zlist_clip_under_this_node(p_node);

    /* clip the normal list by node */
    zlist_clip_list_by_node(&p_info->infor.zl_normal, p_node);
  }
  else
  {
    /* clip node by topmost list */
    zlist_clip_node_by_list(p_node, &p_info->infor.zl_topmost);

    /* use the node to clip others which is under this node */
    zlist_clip_under_this_node(p_node);
  }
}

/* it is not necessary to update the invalid rgn for every affected node . */
static void _recalc_gcrgns_on_move(znode_t *p_node)
{
  mainwin_t *p_win = p_node->p_host;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_win->style & STL_EX_TOPMOST)
  {
    /* use the node to clip others which is under this node */
    zlist_clip_under_this_node(p_node);

    /* clip the normal list by node */
    zlist_clip_list_by_node(&p_info->infor.zl_normal, p_node);
    
    /* clip the node by all above it.(top list)*/
    zlist_clip_node_by_all_above_this(p_node, &p_info->infor.zl_topmost);
  }
  else
  {
    /* clip node by topmost list */
    zlist_clip_node_by_list(p_node, &p_info->infor.zl_topmost);
    
    /* use the node to clip others which is under this node */
    zlist_clip_under_this_node(p_node);

    /* clip the node by all above it(normal list)*/
    zlist_clip_node_by_all_above_this(p_node, &p_info->infor.zl_normal);
  }
}


/*!
   It is necessary to update the invalid rgn for every affected node .
   because it will repaint all affected node
  */
static void _recalc_gcrgns_on_remove(znode_t *p_node)
{
  mainwin_t *p_win = p_node->p_host;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_win->style & STL_EX_TOPMOST)
  {
    _recalc_gcrgns_under_this_node(p_node);

    _recalc_gcrgns_by_node(&p_info->infor.zl_normal, p_node);
  }
  else
  {
    _recalc_gcrgns_under_this_node(p_node);
  }
}

static void _paint_mainwins(void)
{
  znode_t *p_node = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_node = zlist_get_the_last(&p_info->infor.zl_normal);
  while(p_node != NULL)
  {
    if(p_node->is_need_update)
    {
      if(p_node->p_host != NULL)
      {
#ifdef FW_DEBUG
        rect_t rc;
        ctrl_get_frame(p_node->p_host->p_root, &rc);
        FW_PRINTF("FW PAINT normal: id = %d (%d, %d, %d, %d)\n",
                  p_node->p_host->p_root->id, rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
        gdi_get_cliprgn_boundrect(&p_node->p_host->p_root->invrgn_info.crgn,
                                  &rc);
        FW_PRINTF("FW PAINT invalid rgn (%d, %d, %d, %d)\n", rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
#endif
        ctrl_paint_ctrl(p_node->p_host->p_root, FALSE);
      }

      p_node->is_need_update = FALSE;
    }

    p_node = p_node->p_prev;
  }

  p_node = zlist_get_the_last(&p_info->infor.zl_topmost);
  while(p_node != NULL)
  {
    if(p_node->is_need_update)
    {
      if(p_node->p_host != NULL)
      {
#ifdef FW_DEBUG
        rect_t rc;
        ctrl_get_frame(p_node->p_host->p_root, &rc);
        FW_PRINTF("FW PAINT topmost: id = %d (%d, %d, %d, %d)\n",
                  p_node->p_host->p_root->id, rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
        gdi_get_cliprgn_boundrect(&p_node->p_host->p_root->invrgn_info.crgn,
                                  &rc);
        FW_PRINTF("FW PAINT invalid rgn (%d, %d, %d, %d)\n", rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
#endif
        ctrl_paint_ctrl(p_node->p_host->p_root, FALSE);
      }
      p_node->is_need_update = FALSE;
    }

    p_node = p_node->p_prev;
  }
}

static void _paint_mainwins_force(void)
{
  znode_t *p_node = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_node = zlist_get_the_last(&p_info->infor.zl_normal);
  while(p_node != NULL)
  {
    //if(p_node->is_need_update)
    {
      if(p_node->p_host != NULL)
      {
#ifdef FW_DEBUG
        rect_t rc;
        ctrl_get_frame(p_node->p_host->p_root, &rc);
        FW_PRINTF("FW PAINT normal: id = %d (%d, %d, %d, %d)\n",
                  p_node->p_host->p_root->id, rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
        gdi_get_cliprgn_boundrect(&p_node->p_host->p_root->invrgn_info.crgn,
                                  &rc);
        FW_PRINTF("FW PAINT invalid rgn (%d, %d, %d, %d)\n", rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
#endif
        ctrl_paint_ctrl(p_node->p_host->p_root, TRUE);
      }

      //p_node->is_need_update = FALSE;
    }

    p_node = p_node->p_prev;
  }

  p_node = zlist_get_the_last(&p_info->infor.zl_topmost);
  while(p_node != NULL)
  {
    //if(p_node->is_need_update)
    {
      if(p_node->p_host != NULL)
      {
#ifdef FW_DEBUG
        rect_t rc;
        ctrl_get_frame(p_node->p_host->p_root, &rc);
        FW_PRINTF("FW PAINT topmost: id = %d (%d, %d, %d, %d)\n",
                  p_node->p_host->p_root->id, rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
        gdi_get_cliprgn_boundrect(&p_node->p_host->p_root->invrgn_info.crgn,
                                  &rc);
        FW_PRINTF("FW PAINT invalid rgn (%d, %d, %d, %d)\n", rc.left, rc.top,
                  RECTW(rc), RECTH(rc));
#endif
        ctrl_paint_ctrl(p_node->p_host->p_root, TRUE);
      }
      //p_node->is_need_update = FALSE;
    }

    p_node = p_node->p_prev;
  }
}

static BOOL _add_mainwin_to_zorder(mainwin_t *p_win)
{
  znode_t *p_node = NULL, *p_new_z = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if((p_node =
        (znode_t *)lib_memf_alloc(&p_info->infor.znode_heap)) == NULL)
  {
    return FALSE;
  }
  p_node->p_next = p_node->p_prev = NULL;
  p_node->p_host = p_win;
  p_node->is_need_update = FALSE;


  // init global clip region
  znode_init_gcrgn(p_node);

  // add to list
  zlist_add_node(p_win->style & STL_EX_TOPMOST ?
                 &p_info->infor.zl_topmost : &p_info->infor.zl_normal, p_node);

  if(ctrl_get_sts(p_win->p_root) == OBJ_STS_HIDE)
  {
    return TRUE;
  }

  // update global clip region
  switch(ctrl_get_attr(p_win->p_root))
  {
    case OBJ_ATTR_INACTIVE:
      _recalc_gcrgns_on_add(p_node);
      break;

    default:
      _recalc_gcrgns_on_add(p_node);

      p_new_z = zlist_get_the_first_activable(
        p_win->style & STL_EX_TOPMOST ?
        &p_info->infor.zl_topmost :
        &p_info->infor.zl_normal);

      if(p_new_z == p_node)
      {
        if(p_info->infor.p_focus != NULL)
        {
          znode_set_style(p_info->infor.p_focus, OBJ_ATTR_ACTIVE);
        }
        ctrl_set_attr(p_win->p_root, OBJ_ATTR_HL);

        // set focus
        _set_focus(p_node);
      }
      break;
  }

  return TRUE;
}


static BOOL _remove_mainwin_from_zorder(mainwin_t *p_win)
{
  znode_t *p_node = NULL, *p_new_z = NULL, *p_old_z = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_node = zlist_find_node_by_mainwin(
    p_win->style & STL_EX_TOPMOST ? \
    &p_info->infor.zl_topmost : &p_info->infor.zl_normal,
    p_win);
  if(p_node == NULL)
  {
    return FALSE;
  }
  //update all affected containers' GCR info
  if(ctrl_get_sts(p_win->p_root) == OBJ_STS_HIDE)
  {
    //do nothing
  }
  else
  {
    ctrl_set_sts(p_win->p_root, OBJ_STS_HIDE);

    _recalc_gcrgns_on_remove(p_node);

    p_old_z = p_info->infor.p_focus;

    if(p_old_z == p_node)
    {
      p_new_z = zlist_get_the_first_activable(&p_info->infor.zl_topmost);
      if(NULL == p_new_z)
      {
        p_new_z = zlist_get_the_first_activable(&p_info->infor.zl_normal);
      }
      znode_set_style(p_new_z, OBJ_ATTR_HL);

      // set focus
      _set_focus(p_new_z);
    }
  }

  // remove the node from list
  zlist_remove_node(p_win->style & STL_EX_TOPMOST ?
                    &p_info->infor.zl_topmost : &p_info->infor.zl_normal,
                    p_node);

  lib_memf_free(&p_info->infor.znode_heap, p_node);

  return TRUE;
}


static BOOL _get_parent_mainwin(u16 root_id,
                                u16 parent_root_id,
                                mainwin_t **p_parent)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  // check duplication
  if(root_id == ROOT_ID_INVALID
    || fw_find_root_by_id(root_id) != NULL)
  {
    FW_PRINTF("create mainwin is failed: already exist %d\n", root_id);
    return FALSE;
  }

  // check parent
  if(parent_root_id != ROOT_ID_INVALID)
  {
    if(parent_root_id == FW_DESKTOP_PID)  // means create desktop
    {
      *p_parent = NULL;
    }
    else // means create sub mainwin
    {
      *p_parent = fw_find_mainwin_by_id(parent_root_id);
      if(*p_parent == NULL)
      {
        FW_PRINTF("create mainwin is failed: parent is NOT exist [%d]\n",
                  parent_root_id);
        return FALSE;
      }
    }
  }
  else // means create root mainwin
  {
    // its parent is desktop
    *p_parent = p_info->infor.p_first_hosted;
    MT_ASSERT(p_parent != NULL);
  }

  return TRUE;
}


static BOOL _create_mainwin(control_t *p_root, mainwin_t *p_parent)
{
  mainwin_t *p_win = NULL;
  fw_main_t *p_info = NULL;
  u16 root_id = 0;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_win = (mainwin_t *)lib_memf_alloc(&p_info->infor.mainwin_heap);
  if(p_win == NULL)
  {
    FW_PRINTF("create mainwin is failed: alloc mainwin == NULL \n");
    return FALSE;
  }
  FW_PRINTF("FW ALLOK OK\n");
  memset(p_win, 0, sizeof(mainwin_t));

  FW_MANAGEMENT(WINDOW_MANAGE_PRE_OPEN, ctrl_get_ctrl_id(p_root), 0);

  // set global cliprgn info
  cont_set_gcrgn_info(p_root, &p_win->gcrgn_info);

  // init
  p_win->p_root = p_root;
  p_win->style = ctrl_get_style(p_root);

  // add to mainwin tree
  if(p_parent != NULL)
  {
    _add_child(p_parent, p_win);
  }
  else
  {
    _link_desktop(p_win);
  }

  gdi_init_cliprgn(&p_win->gcrgn_info.crgn, gdi_get_cliprc_heap());

#ifdef MUTI_THREAD_SUPPORT
  if(SUCCESS != os_mutex_create(&p_win->gcrgn_info.lock))
  {
    MT_ASSERT(0);
    return FALSE;
  }
#endif

  // add to zorder
  _add_mainwin_to_zorder(p_win);

  FW_MANAGEMENT(WINDOW_MANAGE_POST_OPEN, ctrl_get_ctrl_id(p_root), 0);

  // set state
  root_id = p_win->p_root->id;
  FW_SET_STATE(root_id, QS_OPEN);

  gui_set_anim_mode(ANIM_MODE_IN);

  return TRUE;
}


// TODO: nested recursion function
static BOOL _destroy_mainwin(mainwin_t *p_win, BOOL is_update)
{
  mainwin_t *p_parent = NULL, *p_child = NULL;
  znode_t *p_node = NULL;
  u16 root_id = 0;
  fw_main_t *p_info = NULL;
  rect_t rect = {0};
  BOOL is_top = FALSE;

  ctrl_get_frame(p_win->p_root, &rect);
  ctrl_client2screen(p_win->p_root, &rect);
  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_win == NULL
    || p_win->p_root == fw_get_root()) // can NOT destory desktop
  {
    return FALSE;
  }

  // destory children at first
  p_child = p_win->p_child;
  while(p_child != NULL)
  {
    _destroy_mainwin(p_child, is_update);
    p_child = p_child->p_next;
  }

  root_id = p_win->p_root->id;

  FW_MANAGEMENT(WINDOW_MANAGE_PRE_CLOSE, root_id, 0);

  // pick off from zorder
  is_top = p_win->style & STL_EX_TOPMOST;
  p_node = zlist_find_node_by_mainwin(
    is_top ? &p_info->infor.zl_topmost : &p_info->infor.zl_normal, p_win);
  if(p_node == NULL)
  {
    return FALSE;
  }

  if(!_remove_mainwin_from_zorder(p_win))
  {
    return FALSE;
  }

  // pick off from mainwin tree
  p_parent = p_win->p_parent;
  if(NULL != p_parent)
  {
    _remove_child(p_parent, p_win);
  }
  else
  {
    _unlink_desktop(p_win);
  }

  // release
  ctrl_destroy_ctrl(p_win->p_root);
  gdi_empty_cliprgn(&p_win->gcrgn_info.crgn);
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_destroy(p_win->gcrgn_info.lock);
#endif

  lib_memf_free(&p_info->infor.mainwin_heap, p_win);

  FW_MANAGEMENT(WINDOW_MANAGE_POST_CLOSE, root_id, 0);

  // update view
  if(is_update)
  {
    gdi_start_batch(FALSE);
    
    _paint_mainwins();
    
    gdi_end_batch(FALSE, TRUE, &rect);
  }

  // set state
  FW_SET_STATE(root_id, QS_QUIT);

  FW_MANAGEMENT(WINDOW_MANAGE_CLOSED, root_id, 0);
  return TRUE;
}

static void _ui_msg_parser(os_msg_t *p_msg)
{
  fw_msg_ext_t *p_ext = NULL; 

  if(p_msg->content == FW_INTERNAL_MSG)
  {
    MT_ASSERT(p_msg->para2 == sizeof(fw_msg_ext_t));
    
    p_ext = (fw_msg_ext_t *)p_msg->para1;

    /*!
     type    : fw_msg_type_t, see fw_common.h 130lines.
     dest    : ctrl id.
     content : 
     */
    FW_MSG_PRINT("UI_INTRA_MSG, type %d, dest %d, msg content %d, para1 %d, para2 %d\n",
        p_ext->type, p_ext->dest, p_ext->content, p_ext->para1, p_ext->para2);
  } 
}

BOOL fw_init(fw_config_t *p_config,
             keymap_t keymap,
             msgproc_t proc,
             window_manage_t manage)
{
  u32 size = 0;
  control_t *p_root = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)mmi_alloc_buf(sizeof(fw_main_t));
  MT_ASSERT(p_info != NULL);

  class_register(UIFRM_CLASS_ID, (class_handle_t)(p_info));

  if(is_empty_rect(&p_config->root_rc))
  {
    /* if root is null, desktop can not link to other containers */
    MT_ASSERT(0);
  }

  p_info->config = *p_config;

  // initialize
  memset(&p_info->infor, 0, sizeof(fw_infor_t));

  p_info->infor.p_znode_addr =
    mmi_create_memf(&p_info->infor.znode_heap,
                    p_info->config.max_win_cnt,
                    sizeof(znode_t));
  MT_ASSERT(p_info->infor.p_znode_addr != NULL);

  p_info->infor.p_mainwin_addr =
    mmi_create_memf(&p_info->infor.mainwin_heap,
                    p_info->config.max_win_cnt,
                    sizeof(mainwin_t));
  MT_ASSERT(p_info->infor.p_mainwin_addr != NULL);

  p_info->infor.p_ap_msghost_addr =
    mmi_create_memf(&p_info->infor.ap_msghost_heap,
                    p_info->config.max_host_cnt,
                    sizeof(ap_msghost_t));
  MT_ASSERT(p_info->infor.p_ap_msghost_addr != NULL);

  p_info->infor.msg_q_id = mtos_messageq_create(p_config->max_msg_cnt,
                                                (u8 *)"ui_frmwk");
  MT_ASSERT(p_info->infor.msg_q_id != INVALID);

  mtos_message_set_parser(p_info->infor.msg_q_id, _ui_msg_parser);

  size = sizeof(fw_msg_t) * p_config->max_msg_cnt;
  p_info->infor.msg_q_buf = (u32)mmi_alloc_buf(size);
  MT_ASSERT(p_info->infor.msg_q_buf != 0);

  mtos_messageq_attach(p_info->infor.msg_q_id,
                       (void *)(p_info->infor.msg_q_buf),
                       sizeof(fw_msg_ext_t),
                       p_config->max_msg_cnt);
  MT_ASSERT(p_info->infor.msg_q_id != INVALID);
  p_info->infor.msg_q_state = 0;

  size = sizeof(ui_tmr_entry_t) * p_config->max_tmr_cnt;
  p_info->infor.p_ui_tmr_entrys = (ui_tmr_entry_t *)mmi_alloc_buf(size);
  MT_ASSERT(p_info->infor.p_ui_tmr_entrys != NULL);

  // init tmr entrys
  fw_tmr_init();

  p_info->infor.p_first_hosted = NULL;
  p_info->infor.p_focus = NULL;

  p_info->infor.manage = manage;

  p_info->infor.zl_normal.cnt = p_info->infor.zl_topmost.cnt = 0;
  p_info->infor.zl_normal.p_top = p_info->infor.zl_topmost.p_top = NULL;

  // add the root mainwin
  p_root = fw_create_mainwin(p_config->root_id,
                             (u16)p_config->root_rc.left,
                             (u16)p_config->root_rc.top,
                             (u16)RECTW(p_config->root_rc),
                             (u16)RECTH(p_config->root_rc),
                             FW_DESKTOP_PID, 0, 0, 0);

  MT_ASSERT(p_root != NULL);

  ctrl_set_rstyle(p_root,
                  p_config->root_rc_style,
                  p_config->root_rc_style,
                  p_config->root_rc_style);

  ctrl_set_keymap(p_root, keymap);
  ctrl_set_proc(p_root, proc);

  // notify
  FW_MANAGEMENT(WINDOW_MANAGE_INIT, (u32)p_info->infor.msg_q_id, 0);

  return TRUE;
}


void fw_release(void)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_info->infor.msg_q_buf != 0)
  {
    mmi_free_buf((void *)p_info->infor.msg_q_buf);
    mtos_messageq_clr(p_info->infor.msg_q_id);
    mtos_messageq_detach(p_info->infor.msg_q_id, sizeof(fw_msg_ext_t));
    mtos_messageq_release(p_info->infor.msg_q_id);
    p_info->infor.msg_q_buf = 0;
  }

  if(p_info->infor.p_ui_tmr_entrys != NULL)
  {
    fw_tmr_release();
    mmi_free_buf((void *)p_info->infor.p_ui_tmr_entrys);
    p_info->infor.p_ui_tmr_entrys = NULL;
  }

  if(p_info->infor.p_ap_msghost_addr != NULL)
  {
    mmi_destroy_memf(&p_info->infor.ap_msghost_heap,
                     p_info->infor.p_ap_msghost_addr);
    p_info->infor.p_ap_msghost_addr = NULL;
  }

  if(p_info->infor.p_znode_addr != NULL)
  {
    mmi_destroy_memf(&p_info->infor.znode_heap,
                     p_info->infor.p_znode_addr);
    p_info->infor.p_znode_addr = NULL;
  }

  if(p_info->infor.p_mainwin_addr != NULL)
  {
    mmi_destroy_memf(&p_info->infor.mainwin_heap,
                     p_info->infor.p_mainwin_addr);
    p_info->infor.p_mainwin_addr = NULL;
  }

  mmi_free_buf(p_info);
}


BOOL fw_is_empty(void)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  return (BOOL)((p_info->infor.zl_topmost.cnt == 0x0)
               && (p_info->infor.zl_normal.cnt == 0x1)); /* except the dsk */
}


void fw_set_keymap(keymap_t keymap)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_info->infor.p_first_hosted != NULL);
  ctrl_set_keymap(p_info->infor.p_first_hosted->p_root, keymap);
}


void fw_set_proc(msgproc_t proc)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_info->infor.p_first_hosted != NULL);
  ctrl_set_proc(p_info->infor.p_first_hosted->p_root, proc);
}


void fw_set_manage(window_manage_t manage)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_info->infor.manage = manage;
}

control_t *fw_get_root(void)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_info->infor.p_first_hosted != NULL)
  {
    return (control_t *)p_info->infor.p_first_hosted->p_root;
  }

  return NULL;
}


control_t *fw_get_focus(void)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_info->infor.p_focus != NULL)
  {
    return (control_t *)p_info->infor.p_focus->p_host->p_root;
  }

  return NULL;
}


RET_CODE fw_set_focus(control_t *p_focus)
{
  fw_main_t *p_info = NULL;
  znode_t *p_znode = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_znode = zlist_find_node_by_root(&p_info->infor.zl_topmost, p_focus);
  if (p_znode != NULL)
  {
    _set_focus(p_znode);
    return SUCCESS;
  }

  p_znode = zlist_find_node_by_root(&p_info->infor.zl_normal, p_focus);
  if (p_znode != NULL)
  {
    _set_focus(p_znode);
    return SUCCESS;
  }

  return ERR_FAILURE;
}


u8 fw_get_focus_id(void)
{
  control_t *p_root = NULL;

  if((p_root = fw_get_focus()) != NULL)
  {
    return (u8)p_root->id;
  }

  return ROOT_ID_INVALID;
}

control_t *fw_get_first_active(void)
{
  fw_main_t *p_info = NULL;
  control_t *p_root = NULL;
  znode_t *p_new_z = NULL;
  u8  need_show = FALSE;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if (p_info->infor.p_focus != NULL)
  {
    p_root = p_info->infor.p_focus->p_host->p_root;
  }

  if (p_root != NULL)
  {
    if (ctrl_get_sts(p_root) == OBJ_STS_SHOW)
    {
      p_root->sts = OBJ_STS_HIDE;
      need_show = TRUE;
    }

    p_new_z = zlist_get_the_first_activable(&p_info->infor.zl_topmost);
    if (NULL == p_new_z)
    {
      p_new_z = zlist_get_the_first_activable(&p_info->infor.zl_normal);
    }

    if (need_show == TRUE)
    {
      p_root->sts = OBJ_STS_SHOW;
    }

    if (p_new_z != NULL)
    {
      return p_new_z->p_host->p_root;
    }
  }

  return NULL;
}

u8 fw_get_first_active_id(void)
{
  control_t *p_root = NULL;

  p_root = fw_get_first_active();
  if(p_root != NULL)
  {
    return (u8)p_root->id;
  }

  return ROOT_ID_INVALID;
}

control_t *fw_find_root_by_id(u16 root_id)
{
  znode_t *p_node = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if((p_node =
        zlist_find_node_by_id(&p_info->infor.zl_normal, root_id)) != NULL)
  {
    return zlist_get_the_root(p_node);
  }

  if((p_node =
        zlist_find_node_by_id(&p_info->infor.zl_topmost, root_id)) != NULL)
  {
    return zlist_get_the_root(p_node);
  }
  return NULL;
}

RET_CODE fw_reset_mainwin(u16 root_id)
{
  fw_main_t *p_info = NULL;
  mainwin_t *p_win = NULL;
  znode_t *p_node = NULL;
  obj_sts_t old_sts = OBJ_STS_SHOW;
  control_t *p_child = NULL;
  BOOL is_top = FALSE;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  p_win = fw_find_mainwin_by_id(root_id);

  if(p_win == NULL)
  {
    return ERR_FAILURE;
  }

  is_top = (BOOL)p_win->style & STL_EX_TOPMOST;
  p_node = zlist_find_node_by_mainwin(is_top ?
                 &p_info->infor.zl_topmost : &p_info->infor.zl_normal, p_win);

  if(p_node == NULL)
  {
    MT_ASSERT(0);
  }

  old_sts = ctrl_get_sts(p_win->p_root);

  ctrl_set_sts(p_win->p_root, OBJ_STS_HIDE);

  _recalc_gcrgns_on_remove(p_node);

  ctrl_set_sts_ex(p_win->p_root, old_sts);

  znode_init_gcrgn(p_node);

  _recalc_gcrgns_on_move(p_node);  

  ctrl_empty_invrgn(p_win->p_root);

  ctrl_init_invrgn(p_win->p_root);

  p_child = p_win->p_root->p_child;
  while(p_child != NULL)
  {
    ctrl_add_rect_to_invrgn(p_child, NULL);
    
    p_child = p_child->p_next;
  }  

  p_node->is_need_update = TRUE;

  return SUCCESS;
}



control_t *fw_create_mainwin(u16 root_id,
                             u16 x,
                             u16 y,
                             u16 w,
                             u16 h,
                             u16 parent_root_id,
                             u32 para,
                             obj_attr_t attr,
                             u8 style)
{
  control_t *p_root = NULL;
  mainwin_t *p_parent = NULL;

  // get parent
  if(!_get_parent_mainwin(root_id, parent_root_id, &p_parent))
  {
    MT_ASSERT(0);
    return NULL;
  }

  // create root
  if((p_root =
        ctrl_create_ctrl(CTRL_CONT, root_id, x, y, w, h, NULL, 0)) == NULL)
  {
    FW_PRINTF("create mainwin is failed: alloc root == NULL \n");
    MT_ASSERT(0);
    return NULL;
  }

  ctrl_set_attr(p_root, attr);
  ctrl_set_style(p_root, style);

  // create mainwin
  if(!_create_mainwin(p_root, p_parent))
  {
    MT_ASSERT(0);
    ctrl_destroy_ctrl(p_root);
    return NULL;
  }

  gui_set_anim_mode(ANIM_MODE_IN);
  gui_set_anim(ANIM_IN_NONE, ANIM_OUT_NONE);

  return p_root;
}

RET_CODE fw_move_mainwin(u16 root_id, u16 x, u16 y)
{
  fw_main_t *p_info = NULL;
  mainwin_t *p_win = NULL;
  znode_t *p_node = NULL;
  rect_t frame = {0};
  obj_sts_t old_sts = OBJ_STS_SHOW;
  control_t *p_child = NULL;
  BOOL is_top = FALSE;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  p_win = fw_find_mainwin_by_id(root_id);

  if(p_win == NULL)
  {
    return ERR_FAILURE;
  }

  is_top = (BOOL)p_win->style & STL_EX_TOPMOST;
  p_node = zlist_find_node_by_mainwin(is_top ?
                 &p_info->infor.zl_topmost : &p_info->infor.zl_normal, p_win);

  if(p_node == NULL)
  {
    MT_ASSERT(0);
  }

  old_sts = ctrl_get_sts(p_win->p_root);

  ctrl_set_sts(p_win->p_root, OBJ_STS_HIDE);

  _recalc_gcrgns_on_remove(p_node);

  ctrl_set_sts(p_win->p_root, old_sts);

  ctrl_get_frame(p_win->p_root, &frame);

  set_rect(&frame, x, y, x + RECTW(frame), y + RECTH(frame));

  ctrl_set_frame(p_win->p_root, &frame);

  znode_init_gcrgn(p_node);

  _recalc_gcrgns_on_move(p_node);  

  ctrl_empty_invrgn(p_win->p_root);

  ctrl_init_invrgn(p_win->p_root);

  p_child = p_win->p_root->p_child;
  while(p_child != NULL)
  {
    ctrl_add_rect_to_invrgn(p_child, NULL);
    
    p_child = p_child->p_next;
  }  

  p_node->is_need_update = TRUE;

  //disable animations before move.
  gui_set_anim_mode(ANIM_MODE_NONE);

  _paint_mainwins();
  
  return SUCCESS;
}

control_t *fw_attach_root(control_t *p_root, u16 parent_root_id)
{
  u16 root_id = 0;
  mainwin_t *p_parent = NULL;

  MT_ASSERT(p_root != NULL);
  root_id = p_root->id;

  // get parent
  if(!_get_parent_mainwin(root_id, parent_root_id, &p_parent))
  {
    MT_ASSERT(0);
    return NULL;
  }

  // create mainwin
  if(!_create_mainwin(p_root, p_parent))
  {
    MT_ASSERT(0);
    ctrl_destroy_ctrl(p_root);
    return NULL;
  }

  return p_root;
}


void fw_paint_all_mainwin(void)
{
  //disable animations.
  gui_set_anim_mode(ANIM_MODE_NONE);
  _paint_mainwins();
}

void fw_paint_all_mainwin_force(void)
{
  //disable animations.
  gui_set_anim_mode(ANIM_MODE_NONE);
  _paint_mainwins_force();
}

static RET_CODE _save_mainwin(mainwin_t *p_win)
{
  mainwin_t *p_child = NULL;
  RET_CODE ret = SUCCESS;
  
  // destory children at first
  p_child = p_win->p_child;
  while(p_child != NULL)
  {
    ret = _save_mainwin(p_child);
    if(ret != SUCCESS)
    {
      //failure or skip.
      return ret;
    }
    p_child = p_child->p_next;
  }

  return ctrl_process_msg(p_win->p_root, MSG_SAVE, 0, 0);

}


u8 fw_destroy_all_mainwin(BOOL is_redraw)
{
  znode_t *p_node = NULL;
  mainwin_t *p_win = NULL;
  control_t *p_root = NULL;
  fw_main_t *p_info = NULL;
  RET_CODE ret = SUCCESS;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  //disable animations.
  gui_set_anim_mode(ANIM_MODE_NONE);

  /* close mainwin from top to bottom */
  p_node = p_info->infor.zl_topmost.p_top;
  while(p_node != NULL)
  {
    if((p_win = p_node->p_host) != NULL)
    {
      p_root = p_win->p_root;
      /* because it will change the chain of zorder,
         so save something at first */
      p_node = p_node->p_next;

      if(p_root != fw_get_root())
      {
        ret = _save_mainwin(p_win);
        if(ERR_FAILURE == ret)
        {
          /* something is failed */
          FW_PRINTF("ROOT[%d] break fw_destroy_all_mainwin()......\n",
                    ctrl_get_ctrl_id(p_root));
          if (is_redraw)
          {
            _paint_mainwins();
          }
          return ctrl_get_ctrl_id(p_root);                    
        }
        else if(ERR_STATUS == ret)
        {
          //skip it, only top most menu support skip feature.
        }
        else
        {
          _destroy_mainwin(p_win, FALSE);
        }
      }
    }
    else
    {
      p_node = p_node->p_next;
    }
  }

  p_node = p_info->infor.zl_normal.p_top;
  while(p_node != NULL)
  {
    if((p_win = p_node->p_host) != NULL)
    {
      p_root = p_win->p_root;
      /* because it will change the chain of zorder,
         so save something at first */
      p_node = p_node->p_next;

      if(p_root != fw_get_root())
      {
        ret = _save_mainwin(p_win);
        if(ERR_FAILURE == ret)
        {
          /* something is failed */
          FW_PRINTF("ROOT[%d] break fw_destroy_all_mainwin()......\n",
                    ctrl_get_ctrl_id(p_root));

          if(is_redraw)
          {
            _paint_mainwins();
          }
          return ctrl_get_ctrl_id(p_root);
        }
        else
        {
          _destroy_mainwin(p_win, FALSE);
        }
      }
    }
    else
    {
      p_node = p_node->p_next;
    }
  }

  if (is_redraw)
  {
    _paint_mainwins();
  }

  // the desktop
  return ctrl_get_ctrl_id(fw_get_root());
}


mainwin_t *fw_find_mainwin_by_root(control_t *p_root)
{
  znode_t *p_node = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if((p_node =
        zlist_find_node_by_root(&p_info->infor.zl_normal, p_root)) != NULL)
  {
    return p_node->p_host;
  }

  if((p_node =
        zlist_find_node_by_root(&p_info->infor.zl_topmost, p_root)) != NULL)
  {
    return p_node->p_host;
  }

  return NULL;
}


mainwin_t *fw_find_mainwin_by_id(u16 root_id)
{
  znode_t *p_node = NULL;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if((p_node =
        zlist_find_node_by_id(&p_info->infor.zl_normal, root_id)) != NULL)
  {
    return p_node->p_host;
  }

  if((p_node =
        zlist_find_node_by_id(&p_info->infor.zl_topmost, root_id)) != NULL)
  {
    return p_node->p_host;
  }

  return NULL;
}


BOOL fw_destroy_mainwin_by_root(control_t *p_root)
{
  mainwin_t *p_win = fw_find_mainwin_by_root(p_root);

  if(p_win == NULL)
  {
    return FALSE;
  }
  else
  {
    gui_set_anim_mode(ANIM_MODE_OUT);
    gui_set_anim(p_win->anim_in, p_win->anim_out);

    return (BOOL)_destroy_mainwin(p_win, TRUE);
  }
}


BOOL fw_destroy_mainwin_by_id(u16 root_id)
{
  mainwin_t *p_win = fw_find_mainwin_by_id(root_id);

  if(p_win == NULL)
  {
    return FALSE;
  }
  else
  {
    gui_set_anim_mode(ANIM_MODE_OUT);
    gui_set_anim(p_win->anim_in, p_win->anim_out);

    return (BOOL)_destroy_mainwin(p_win, TRUE);
  }
}

void fw_get_anim(u16 root_id, anim_in_t *p_anim_in, anim_out_t *p_anim_out)
{
  mainwin_t *p_win = fw_find_mainwin_by_id(root_id);
  
  MT_ASSERT(p_win != NULL);
  MT_ASSERT(p_anim_in != NULL);
  MT_ASSERT(p_anim_out != NULL);

  *p_anim_in = p_win->anim_in;
  *p_anim_out = p_win->anim_out;
  
  return;  
}

void fw_set_anim(u16 root_id, anim_in_t anim_in, anim_out_t anim_out)
{
  mainwin_t *p_win = fw_find_mainwin_by_id(root_id);
  
  MT_ASSERT(p_win != NULL);

  p_win->anim_in = anim_in;
  p_win->anim_out = anim_out;

  gui_set_anim(anim_in, anim_out);
}

