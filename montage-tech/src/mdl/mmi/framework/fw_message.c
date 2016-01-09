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
#include "ctrl_wait.h"

#include "ap_framework.h"
#include "framework.h"
#include "fw_common.h"

#include "rc_xml.h"
#include "ap_rc.h"

#define GET_EVT_HOST(evt)    ((u16)(evt >> 16))

u16 fw_wait_keymap(u16 key);

RET_CODE fw_wait_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static ap_evt_entry_t *get_ap_entry(u16 ap_id)
{
  fw_main_t *p_info = NULL;

  MT_ASSERT(ap_id < APP_LAST
           && ap_id >= APP_FRAMEWORK);
  
  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  return (ap_evt_entry_t *)&p_info->infor.ap_evt_entrys[ap_id - APP_FRAMEWORK];
}

static void _fill_int_msg(fw_msg_t *p_msg,
                          u8 type,
                          u16 content,
                          u32 dest,
                          u32 para1,
                          u32 para2,
                          u32 reserve)
{
  memset(p_msg, 0, sizeof(fw_msg_t));
  // don't change follow settings
  p_msg->head.is_brdcst = FALSE;
  p_msg->head.is_ext = TRUE;
  p_msg->head.is_sync = FALSE;
  // end of settings
  p_msg->head.para1 = (u32)(&p_msg->ext);
  p_msg->head.para2 = sizeof(fw_msg_ext_t);
  p_msg->head.content = FW_INTERNAL_MSG;

  p_msg->ext.type = type;
  p_msg->ext.dest = dest;
  p_msg->ext.content = content;
  p_msg->ext.para1 = para1;
  p_msg->ext.para2 = para2;
  p_msg->ext.host = reserve;
}


static RET_CODE _post_int_msg(u32 msgq_id, fw_msg_t *p_msg)
{
  if(!mtos_messageq_send(msgq_id, (os_msg_t *)p_msg))
  {
    //MT_ASSERT(0);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void _sync_msg_ack(void)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  mtos_messageq_ack(p_info->infor.msg_q_id);
}


static RET_CODE _mainwin_default_msg_proc(mainwin_t *p_win,
                                          u16 msg,
                                          u32 para1,
                                          u32 para2)
{
  control_t *p_root = NULL;
  control_t *p_active = NULL;
  RET_CODE ret = SUCCESS;

  if(p_win == NULL)
  {
    return ERR_NOFEATURE;
  }

  p_root = p_win->p_root;
  if(p_root == NULL)
  {
    return ERR_FAILURE;
  }

  if(msg == MSG_EXIT)               /* only process this msg */
  {
    if(p_root == fw_get_root())  /* can not close the desktop */
    {
      return ERR_FAILURE;
    }

    ret = ctrl_process_msg(p_root, MSG_EXIT, para1, para2);

    if(ret != SUCCESS)  /* is_force to destroy */
    {
      if(!fw_destroy_mainwin_by_root(p_root))
      {
        return ERR_FAILURE;
      }
    }

    return ret;
  }
  else
  {
    p_active = p_root;

    // goto the child of the root
    while(p_active->p_active_child != NULL)
    {
      p_active = p_active->p_active_child;
    }

    // process the msg from child to parent
    while(p_root->p_parent != p_active)
    {
      ret = ctrl_process_msg(p_active, msg, para1, para2);
      if(ret != ERR_NOFEATURE)
      {
        break;   /* this msg has been processed */
      }
      p_active = p_active->p_parent;
    }
  }

  return ret;
}


static RET_CODE _mainwin_default_key_proc(mainwin_t *p_win,
                                          u16 key,
                                          u32 para1,
                                          u32 para2)
{
  control_t *p_root = NULL;
  control_t *p_active = NULL;
  u16 msg = MSG_INVALID;
  RET_CODE ret = SUCCESS;

  if(p_win == NULL)
  {
    return ERR_NOFEATURE;
  }

  p_root = p_win->p_root;
  if(p_root == NULL)
  {
    return ERR_FAILURE;
  }

  // goto the child of the root
  p_active = p_root;
  while(p_active->p_active_child != NULL)
  {
    p_active = p_active->p_active_child;
  }

  // process the msg from child to parent
  while(p_root->p_parent != p_active)
  {
    msg = ctrl_translate_key(p_active, key, para1, para2);
    if(msg != MSG_INVALID)
    {
      ret = ctrl_process_msg(p_active, msg, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }

    if(ret != ERR_NOFEATURE)
    {
      break; /* this key has been processed */
    }
    p_active = p_active->p_parent;
  }
  return ret;
}


/* all mainwins which is register on app */
static RET_CODE _dispatch_msg(u16 ap_id, u16 content, u32 para1, u32 para2)
{
  mainwin_t *p_win = NULL;
  ap_msghost_t *p_msghost = NULL;

  p_msghost = get_ap_entry(ap_id)->p_first_host;

  while(p_msghost != NULL)
  {
    p_win = fw_find_mainwin_by_id(p_msghost->root_id);
    if(p_win == NULL)
    {
      FW_PRINTF("UI_FW: NO mainwin[%d] to process msg[0x%x]\n",
                p_msghost->root_id, content);
    }
    else
    {
      _mainwin_default_msg_proc(p_win, content, para1, para2);
    }

    p_msghost = p_msghost->p_next;
  }

  return SUCCESS;
}


/* focus mainwin -> desktop */
static RET_CODE _dispatch_key(u16 content, u32 para1, u32 para2)
{
  mainwin_t *p_win = NULL;
  RET_CODE ret = SUCCESS;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_info->infor.p_focus == NULL)
  {
    return ERR_FAILURE;
  }
  // goto the focus mainwin
  p_win = p_info->infor.p_focus->p_host;
  ret = _mainwin_default_key_proc(p_win, content, para1, para2);

  if(ret == ERR_NOFEATURE)
  {
    if(p_win != p_info->infor.p_first_hosted)
    {
      p_win = p_info->infor.p_first_hosted;
      ret = _mainwin_default_key_proc(p_win, content, para1, para2);
    }
  }

  return ret;
}


/* just dispatch to dest */
static RET_CODE _dispatch_tmr(u32 dest, u16 content, u32 para1, u32 para2)
{
  fw_main_t *p_info = NULL;
  mainwin_t *p_win = NULL;


  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);  
  
  p_win = fw_find_mainwin_by_id((u16)dest);
  if(p_win == NULL)
  {
    FW_PRINTF("UI_FW: NO mainwin[%d] to process tmr[0x%x]\n",
              dest, content);
    return ERR_FAILURE;
  }

  return _mainwin_default_msg_proc(p_win, content, para1, para2);
}


/* just dispatch to dest */
static RET_CODE _dispatch_notify(u32 type,
                                 u32 dest,
                                 u16 content,
                                 u32 para1,
                                 u32 para2)
{
  mainwin_t *p_win = NULL;

  p_win = fw_find_mainwin_by_id((u16)dest);
  if(p_win == NULL)
  {
    FW_PRINTF("UI_FW: NO mainwin[%d] to process tmr[0x%x]\n",
              dest, content);
    return ERR_FAILURE;
  }

  switch(type)
  {
    case NOTIFY_T_KEY:
      return _mainwin_default_key_proc(p_win, content, para1, para2);
    case NOTIFY_T_MSG:
      return _mainwin_default_msg_proc(p_win, content, para1, para2);
    default:
      MT_ASSERT(0);
  }

  return ERR_NOFEATURE;
}


u16 _convert_ap_evt(u32 ap_id, u32 evt)
{
  ap_evtmap_t evtmap;
  evtmap = get_ap_entry(ap_id)->evtmap;
  if(evtmap == NULL)
  {
    return MSG_INVALID;
  }
  return evtmap(evt);
}

RET_CODE _process_ap_evt(u32 ap_id, u32 evt, u32 para1, u32 para2)
{
  ap_evtproc_t evtproc;
  evtproc = get_ap_entry(ap_id)->evtproc;
  if(evtproc == NULL)
  {
    return ERR_NOFEATURE;
  }
  return evtproc(evt, para1, para2);
}


static void _pre_dispatch_msg(fw_msg_t *p_msg)
{
  os_msg_t *p_head = &p_msg->head;
  RET_CODE ret = ERR_NOFEATURE;

  /* check, if is external msg */
  if(p_head->content != FW_INTERNAL_MSG)
  {
    /* associate system evt with app_framework */
    if(p_head->content & SYS_EVT_TO_UI_MSG_MASK)
    {
      p_msg->ext.host = APP_FRAMEWORK;
      p_head->content &= (~SYS_EVT_TO_UI_MSG_MASK);
    }
    else
    {
      p_msg->ext.host = GET_EVT_HOST(p_head->content);
    }

    /* convert */
    if(p_msg->ext.host == APP_UIO)
    {
      FW_PRINTF("__pre_dispatch_msg: is key = 0x%x\n", p_head->para1);
      p_msg->ext.type = MSG_T_UIO;
      p_msg->ext.content = (u16)p_head->para1;
      p_msg->ext.para1 = p_head->para1; /* take the vkey into proc */
      p_msg->ext.dest = 0;

      /* check, if need ack at first */
      if(p_head->is_sync)
      {
        _sync_msg_ack();
        p_head->is_sync = 0;
      }
    }
    else if(p_msg->ext.host == APP_RC)
    {
      FW_PRINTF("dispatch key from rc: 0x%x\n", p_head->para1);
      if(p_head->content == (RC_EVT_KEY))
      {
        p_msg->ext.type = MSG_T_UIO;
        p_msg->ext.content = (u16)p_head->para1;
        p_msg->ext.para1 = p_head->para1; /* take the vkey into proc */
        p_msg->ext.para2 = p_head->para2;
        p_msg->ext.dest = 0;
      }
      else
      {
        p_msg->ext.type = MSG_T_APP;
        p_msg->ext.content = _convert_ap_evt(APP_RC, p_head->content);
        p_msg->ext.para1 = p_head->para1;
        p_msg->ext.para2 = p_head->para2;
      }

      /* check, if need ack at first */
      if(p_head->is_sync)
      {
        _sync_msg_ack();
        p_head->is_sync = 0;
      }
    }
    else
    {
      FW_PRINTF("__pre_dispatch_msg: is msg = 0x%x\n", p_head->content);
      ret = _process_ap_evt(p_msg->ext.host, p_head->content, p_head->para1, p_head->para2);
      if (ret != SUCCESS)
      {
          p_msg->ext.content = _convert_ap_evt(p_msg->ext.host, p_head->content);
      }
      else
      {
          p_msg->ext.content = MSG_INVALID;
      }
      p_msg->ext.type = MSG_T_APP;
      p_msg->ext.para1 = p_head->para1;
      p_msg->ext.para2 = p_head->para2;
    }
  }
  else
  {
    /* it's a internal msg, do nothing */
    FW_PRINTF("__pre_dispatch_msg: is int msg = 0x%x\n", p_msg->ext.content);
  }
}


static void _post_dispatch_msg(fw_msg_t *p_msg)
{
  /* check, if need ack */
  if(p_msg->head.is_sync)
  {
    _sync_msg_ack();
  }
}


static BOOL _add_msghost(ap_evt_entry_t *p_entry, u16 host)
{
  ap_msghost_t *p_last = NULL, *p_new = NULL;
  fw_main_t *p_info = NULL;
  MT_ASSERT(p_entry != NULL);

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_new = (ap_msghost_t *)lib_memf_alloc(&p_info->infor.ap_msghost_heap);
  MT_ASSERT(p_new != NULL);

  p_new->root_id = host;
  p_new->p_next = NULL;

  p_last = p_entry->p_first_host;
  if(p_last != NULL)
  {
    // link to rear
    while(p_last->p_next != NULL)
    {
      if(p_last->root_id == host)
      {
        return FALSE;
      }
      p_last = p_last->p_next;
    }

    if(p_last->root_id == host)
    {
      return FALSE;
    }
    
    p_last->p_next = p_new;
  }
  else
  {
    // set the first host
    p_entry->p_first_host = p_new;
  }

  return TRUE;
}


static BOOL _del_msghost(ap_evt_entry_t *p_entry, u16 host)
{
  ap_msghost_t *p_curn = NULL, *p_prev = NULL;
  fw_main_t *p_info = NULL;
  MT_ASSERT(p_entry != NULL);
  
  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_curn = p_prev = p_entry->p_first_host;
  while(p_curn != NULL)
  {
    if(p_curn->root_id == host)
    {
      // found the host
      break;
    }
    p_prev = p_curn;
    p_curn = p_curn->p_next;
  }

  if(p_curn == NULL)
  {
    // not found
    return FALSE;
  }

  if(p_curn == p_entry->p_first_host)
  {
    p_entry->p_first_host = p_curn->p_next;
  }
  else
  {
    p_prev->p_next = p_curn->p_next;
  }

  lib_memf_free(&p_info->infor.ap_msghost_heap, p_curn);
  return TRUE;
}

static BOOL _find_msghost(ap_evt_entry_t *p_entry, u16 host)
{
  ap_msghost_t *p_curn = NULL, *p_prev = NULL;
  fw_main_t *p_info = NULL;
  MT_ASSERT(p_entry != NULL);
  
  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_curn = p_prev = p_entry->p_first_host;
  while(p_curn != NULL)
  {
    if(p_curn->root_id == host)
    {
      // found the host
      return TRUE;
    }
    p_prev = p_curn;
    p_curn = p_curn->p_next;
  }

  return FALSE;
}


static RET_CODE _send_notify_to_mainwin(mainwin_t *p_win,
                                        u8 type,
                                        u16 content,
                                        u32 para1,
                                        u32 para2)
{
  RET_CODE ret = ERR_NOFEATURE;

  switch(type)
  {
    case NOTIFY_T_KEY:
      ret = _mainwin_default_key_proc(p_win, content, para1, para2);
      break;
    case NOTIFY_T_MSG:
      ret = _mainwin_default_msg_proc(p_win, content, para1, para2);
      break;
    default:
      MT_ASSERT(0);
      ret = ERR_FAILURE;
  }

  return ret;
}


static RET_CODE _post_notify_to_mainwin(mainwin_t *p_win,
                                        u8 type,
                                        u16 content,
                                        u32 para1,
                                        u32 para2)
{
  RET_CODE ret = ERR_NOFEATURE;
  u32 dest = 0;

  if(p_win == NULL)
  {
    return ERR_NOFEATURE;
  }

  dest = ctrl_get_ctrl_id(p_win->p_root);
  ret = fw_post_notify(type, dest, content, para1, para2);

  return ret;
}


BOOL fw_get_msg(u16 root_id, fw_msg_t *p_msg)
{
  os_msg_t *p_head = &p_msg->head;
  fw_main_t *p_info = NULL;
  event_t evt = {0};

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(GET_QS_ROOT(p_info->infor.msg_q_state) == root_id
    && GET_QS_STATE(p_info->infor.msg_q_state) == QS_QUIT)
  {
    return FALSE;
  }

  if(!mtos_messageq_receive(p_info->infor.msg_q_id, p_head,
                          p_info->config.tmout_receive_msg))
  {
    MT_ASSERT(0);
    return FALSE;
  }

  if(p_head->content == FW_INTERNAL_MSG)
  {
    // only process internal message
    memcpy(&p_msg->ext, (void *)p_head->para1, p_head->para2);
  }
  else
  {
    // external message will be convert to internal message on _pre_dispatch_msg
    memset(&p_msg->ext, 0, sizeof(fw_msg_ext_t));

    evt.id = p_head->content;
    evt.data1 = p_head->para1;
    evt.data2 = p_head->para2;
    
    ap_frm_msg_spend(&evt);
  }

  return TRUE;
}


RET_CODE fw_empty_msg(void)
{
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  mtos_messageq_clr(p_info->infor.msg_q_id);
  return SUCCESS;
}


RET_CODE fw_post_tmout(u32 dest, u16 content, u32 para1, u32 para2)
{
  fw_msg_t msg;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  _fill_int_msg(&msg, MSG_T_TMR, content, dest, para1, para2, 0);

  return _post_int_msg(p_info->infor.msg_q_id, &msg);
}


RET_CODE fw_post_notify(u32 type, u32 dest, u16 content, u32 para1, u32 para2)
{
  fw_msg_t msg;
  fw_main_t *p_info = NULL;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  _fill_int_msg(&msg, MSG_T_INT, content, dest, para1, para2, type);

  return _post_int_msg(p_info->infor.msg_q_id, &msg);
}


RET_CODE fw_dispatch_msg(fw_msg_t *p_msg)
{
  RET_CODE ret = ERR_NOFEATURE;

  switch(p_msg->ext.type)
  {
    case MSG_T_UIO:
      FW_MANAGEMENT(WINDOW_MANAGE_ON_UIO_EVENT, \
                         p_msg->ext.content, 0);

      ret = _dispatch_key(p_msg->ext.content,
                          p_msg->ext.para1, p_msg->ext.para2);
      break;
    case MSG_T_APP:
      ret = _dispatch_msg(p_msg->ext.host, p_msg->ext.content,
                          p_msg->ext.para1, p_msg->ext.para2);
      break;
    case MSG_T_TMR:
      ret = _dispatch_tmr(p_msg->ext.dest, p_msg->ext.content,
                          p_msg->ext.para1, p_msg->ext.para2);
      break;

    case MSG_T_INT:
      ret = _dispatch_notify(p_msg->ext.host, p_msg->ext.dest,
                             p_msg->ext.content, p_msg->ext.para1,
                             p_msg->ext.para2);
      break;

    default:
      FW_PRINTF("UI_FW: unknown msg type, ERROR!\n");
      MT_ASSERT(0);
      ret = ERR_FAILURE;
  }

  return ret;
}


void fw_default_mainwin_loop(u16 root_id)
{
  fw_msg_t msg;

  FW_RESET_STATE(root_id);

  while(fw_get_msg(root_id, &msg))
  {
    FW_PRINTF("desktop get msg is OK! [%d]\n", mtos_ticks_get());
    /* check, if is external msg */
    _pre_dispatch_msg(&msg);

    fw_dispatch_msg(&msg);
    /* post dispatch msg */
    _post_dispatch_msg(&msg);
    FW_PRINTF("desktop process msg is OK[%d]\n", mtos_ticks_get());
  }
}


BOOL fw_register_ap_evtmap(u16 ap_id, ap_evtmap_t p_evtmap)
{
  get_ap_entry(ap_id)->evtmap = p_evtmap;
  return TRUE;
}


BOOL fw_unregister_ap_evtmap(u16 ap_id)
{
  get_ap_entry(ap_id)->evtmap = NULL;

  return TRUE;
}

BOOL fw_register_ap_evtproc(u16 ap_id, ap_evtproc_t p_evtproc)
{
  get_ap_entry(ap_id)->evtproc = p_evtproc;
  return TRUE;
}


BOOL fw_unregister_ap_evtproc(u16 ap_id)
{
  get_ap_entry(ap_id)->evtproc = NULL;

  return TRUE;
}


BOOL fw_register_ap_msghost(u16 ap_id, u16 root_id)
{
  ap_evt_entry_t *p_entry = NULL;

  p_entry = get_ap_entry(ap_id);

  if(_find_msghost(p_entry, root_id))
  {
    return TRUE;
  }

  return _add_msghost(p_entry, root_id);
}


BOOL fw_unregister_ap_msghost(u16 ap_id, u16 root_id)
{
  ap_evt_entry_t *p_entry = NULL;

  p_entry = get_ap_entry(ap_id);

  return _del_msghost(p_entry, root_id);
}


RET_CODE fw_notify_parent(u16 root_id,
                          u8 type,
                          BOOL is_sync,
                          u16 content,
                          u32 para1,
                          u32 para2)
{
  mainwin_t *p_win = NULL, *p_parent = NULL;

  p_win = fw_find_mainwin_by_id(root_id);
  if(p_win == NULL)
  {
    FW_PRINTF("UI_FW: the root (%d) is NOT existed, ERROR!\n", root_id);
    return ERR_FAILURE;
  }

  p_parent = p_win->p_parent;
  if(p_parent == NULL)
  {
    FW_PRINTF("UI_FW: the parent of root (%d) is NOT existed, ERROR!\n",
              root_id);
    return ERR_FAILURE;
  }

  if(is_sync)
  {
    return _send_notify_to_mainwin(p_parent, type, content, para1, para2);
  }
  else
  {
    return _post_notify_to_mainwin(p_parent, type, content, para1, para2);
  }
}


RET_CODE fw_notify_child(u16 root_id,
                         u16 child_id,
                         u8 type,
                         BOOL is_sync,
                         u16 content,
                         u32 para1,
                         u32 para2)
{
  mainwin_t *p_win = NULL, *p_child = NULL;

  p_win = fw_find_mainwin_by_id(root_id);
  if(p_win == NULL)
  {
    FW_PRINTF("UI_FW: the root (%d) is NOT existed, ERROR!\n", root_id);
    return ERR_FAILURE;
  }

  p_child = p_win->p_child;
  while(p_child != NULL)
  {
    if(ctrl_get_ctrl_id(p_child->p_root) == child_id)  // child found
    {
      break;
    }
    p_child = p_child->p_next;
  }

  if(p_child == NULL)
  {
    FW_PRINTF("UI_FW: the child(%d) of root (%d) is NOT existed, ERROR!\n",
              child_id, root_id);
    return ERR_FAILURE;
  }

  if(is_sync)
  {
    return _send_notify_to_mainwin(p_child, type, content, para1, para2);
  }
  else
  {
    return _post_notify_to_mainwin(p_child, type, content, para1, para2);
  }
}

RET_CODE fw_notify_root(control_t *p_ctrl,
                         u8 type,
                         BOOL is_sync,
                         u16 content,
                         u32 para1,
                         u32 para2)
{
  mainwin_t *p_win = NULL;
  control_t *p_tmp = NULL, *p_root = NULL;
  u16 root_id = ROOT_ID_INVALID;

  MT_ASSERT(p_ctrl != NULL);

  p_tmp = p_ctrl;

  while(p_tmp)
  {
    p_root = p_tmp;
    p_tmp = ctrl_get_parent(p_tmp);
  }

  
  root_id = ctrl_get_ctrl_id(p_root);
  p_win = fw_find_mainwin_by_id(root_id);
  
  if(p_win == NULL)
  {
    FW_PRINTF("UI_FW: the root (%d) is NOT existed, ERROR!\n", root_id);
    return ERR_FAILURE;
  }

  if(is_sync)
  {
    return _send_notify_to_mainwin(p_win, type, content, para1, para2);
  }
  else
  {
    return _post_notify_to_mainwin(p_win, type, content, para1, para2);
  }
}


RET_CODE fw_on_wm_event(const manage_evtmap_t *p_entry,
                          u32 event,
                          u32 para1,
                          u32 para2)
{
  RET_CODE ret = ERR_NOFEATURE;

  if(p_entry != NULL)
  {
    while(p_entry->event != WINDOW_MANAGE_EVT_INVALID)
    {
      if(p_entry->event == event)
      {
        ret = p_entry->proc(event, para1, para2);
        break;
      }
      p_entry++;
    }
  }
  return ret;
}


u16 fw_on_ap_event(const ap_evtmap_entry_t *p_evtmap, u32 event)
{
  u16 msg = MSG_INVALID;

  if(p_evtmap != NULL)
  {
    while(p_evtmap->ui_msg != MSG_INVALID)
    {
      if(p_evtmap->ap_evt == event)
      {
        msg = p_evtmap->ui_msg;
        break;
      }
      p_evtmap++;
    }
  }

  return msg;
}

RET_CODE fw_on_ap_event_proc(const ap_evtproc_entry_t *p_entry, u32 event, u32 para1, u32 para2)
{
  RET_CODE ret = ERR_NOFEATURE;

  if(p_entry != NULL)
  {
    while(p_entry->p_proc != NULL)
    {
      if(p_entry->ap_evt == event)
      {
        ret = p_entry->p_proc(event, para1, para2);
        break;
      }
      p_entry++;
    }
  }
  return ret;
}

void fw_watting(u32 rstyle_root, u32 rstyle_wait, u32 img_id)
{
  control_t *p_cont = NULL, *p_wait = NULL;
  fw_main_t *p_info = NULL;
  u16 x = 0, y = 0, w = 0, h = 0;
  RET_CODE ret = SUCCESS;

  p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  p_cont = fw_create_mainwin(ROOT_ID_WAITTING,
    p_info->config.root_rc.left, p_info->config.root_rc.top,
    RECTW(p_info->config.root_rc), RECTH(p_info->config.root_rc),
    0, 0, OBJ_ATTR_ACTIVE, STL_EX_TOPMOST);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, fw_wait_keymap);
  ctrl_set_proc(p_cont, fw_wait_proc);
  ctrl_set_rstyle(p_cont, rstyle_root, rstyle_root, rstyle_root);

  w = h = RECTW(p_info->config.root_rc) / 8;

  MT_ASSERT(RECTH(p_info->config.root_rc) > h);
  
  x = (RECTW(p_info->config.root_rc) - w) / 2;
  y = (RECTH(p_info->config.root_rc) - h) / 2;
  
  p_wait = ctrl_create_ctrl(CTRL_WAIT, 0,
    x, y, w, h, p_cont, 0);
  MT_ASSERT(p_wait != NULL);
  ctrl_set_rstyle(p_wait, rstyle_wait, rstyle_wait, rstyle_wait);
  wait_set_img(p_wait, WAIT_IMG_ID, img_id);

  ctrl_default_proc(p_wait, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  ret = fw_tmr_create(ROOT_ID_WAITTING, MSG_INCREASE, 50, TRUE);
  MT_ASSERT(ret == SUCCESS);
  
  fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
}

static RET_CODE on_wait_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  
  return fw_destroy_mainwin_by_id(ctrl_get_ctrl_id(p_ctrl));
}

static RET_CODE on_wait_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  fw_tmr_destroy(ctrl_get_ctrl_id(p_ctrl), MSG_INCREASE);
  
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(fw_wait_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
END_KEYMAP(fw_wait_keymap, NULL)

BEGIN_MSGPROC(fw_wait_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_wait_cancel)
  ON_COMMAND(MSG_DESTROY, on_wait_destroy)
END_MSGPROC(fw_wait_proc, cont_class_proc)
