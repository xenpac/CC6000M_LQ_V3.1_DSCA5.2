/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 
 ****************************************************************************/
#include "ui_common.h"

#include "ui_notify.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

enum comm_dlg_idc
{
  IDC_CONTENT = 1,
};


static rect_t g_default_notify_rc = 
{
  NOTIFY_CONT_X, NOTIFY_CONT_Y,
  NOTIFY_CONT_X + NOTIFY_CONT_W,
  NOTIFY_CONT_Y + NOTIFY_CONT_H
};

static rect_t g_notify_rc = 
{
  NOTIFY_CONT_X, NOTIFY_CONT_Y,
  NOTIFY_CONT_X + NOTIFY_CONT_W,
  NOTIFY_CONT_Y + NOTIFY_CONT_H
};

static u32 g_notify_content = 0;
static u32 g_notify_type = NOTIFY_TYPE_STRID;
static BOOL g_notify_autoclose = TRUE;

RET_CODE notify_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void ui_notify_set_default(void)
{
  if(g_notify_type != NOTIFY_TYPE_STRID)
  {
    mtos_free((void *)g_notify_content);
  }

  g_notify_type = NOTIFY_TYPE_STRID;
  g_notify_content = 0;
  copy_rect(&g_notify_rc, &g_default_notify_rc);

  return;
}

void  ui_set_notify_autoclose(BOOL is_autoclose)
{
  g_notify_autoclose = is_autoclose;
}

void ui_set_notify (rect_t *p_rect, u32 type, u32 content, u32 duration)
{
  BOOL is_active;
  control_t *p_cont, *p_txt;
  u16 size = 0;
  
  u8 focus = fw_get_focus_id();

  is_active = (ui_is_fullscreen_menu(focus) || ui_is_popup_menu(focus));

  if (content == 0)
  {
    ui_notify_set_default();
    close_notify();
  }
  else
  {
    if(g_notify_type != NOTIFY_TYPE_STRID)
    {
      mtos_free((void *)g_notify_content);
    }

    g_notify_type = type;

    if(type == NOTIFY_TYPE_ASC)
    {
      size = (u16)strlen((char *)content) + 1;
      g_notify_content = (u32)mtos_malloc(size);
      memset((void *)g_notify_content, 0, size);
      memcpy((u8 *)g_notify_content, (u8 *)content, size);
    }
    else if(type == NOTIFY_TYPE_UNI)
    {
      size = (u16)uni_strlen((u16 *)content) + 1;
      g_notify_content = (u32)mtos_malloc(size * sizeof(u16));
      memset((void *)g_notify_content, 0, size * sizeof(u16));
      memcpy((u16 *)g_notify_content, (u16 *)content, size * sizeof(u16));
    }
    else
    {
      g_notify_content = content;      
    }

    if ((p_cont = fw_find_root_by_id(ROOT_ID_NOTIFY)) != NULL)//already open
    {
      if((p_rect == NULL) && (memcmp(&g_default_notify_rc, &g_notify_rc, sizeof(rect_t)) != 0))
      {
        copy_rect(&g_notify_rc, &g_default_notify_rc);
        close_notify();
        open_notify(duration, 0);
      }
      else if((p_rect != NULL) && (memcmp(p_rect, &g_notify_rc, sizeof(rect_t)) != 0))
      {
        copy_rect(&g_notify_rc, p_rect);
        close_notify();
        open_notify(duration, 0);
      }
      else
      {
        p_txt = ctrl_get_child_by_id(p_cont, IDC_CONTENT);

        switch(g_notify_type)
        {
          case NOTIFY_TYPE_STRID:
            text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
            text_set_content_by_strid(p_txt, (u16)g_notify_content);
            break;
          case NOTIFY_TYPE_ASC:
            text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);  
            text_set_content_by_ascstr(p_txt, (u8 *)g_notify_content);
            break;
          case NOTIFY_TYPE_UNI:
            text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE); 
            text_set_content_by_unistr(p_txt, (u16 *)g_notify_content);
            break;
          default:
            MT_ASSERT(0);
        }
        ctrl_paint_ctrl(p_txt, TRUE);
        fw_tmr_reset(ROOT_ID_NOTIFY, MSG_EXIT, duration);
      }
    }
    else
    {
      if(p_rect != NULL)
      {
        memcpy(&g_notify_rc, p_rect, sizeof(rect_t));
      }
      
      if (is_active)
      {
        open_notify(duration, 0);
      }
    }
  }
}

BOOL ui_is_notify(void)
{
  return (BOOL)(g_notify_content != 0);
}

RET_CODE open_notify(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_txt;
  u32 duration = para1;

  if (g_notify_content == 0)
    return ERR_FAILURE;

  if (fw_find_root_by_id(ROOT_ID_NOTIFY) != NULL)
    return ERR_FAILURE;

  p_cont = fw_create_mainwin(ROOT_ID_NOTIFY,
                                  (u16)g_notify_rc.left, (u16)g_notify_rc.top, 
                                  (u16)RECTW(g_notify_rc), (u16)RECTH(g_notify_rc),
                                  ROOT_ID_INVALID, 0, 
                                  OBJ_ATTR_INACTIVE, 0);
  ctrl_set_proc(p_cont, notify_proc);
  MT_ASSERT(NULL != p_cont);
  ctrl_set_rstyle(p_cont,
                  RSI_TRANSPARENT,
                  RSI_TRANSPARENT,
                  RSI_TRANSPARENT);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           0, 0,
                           (u16)RECTW(g_notify_rc),(u16)RECTH(g_notify_rc),
                           p_cont, 0);
  ctrl_set_rstyle(p_txt,
                  RSI_NOTIFY_TXT,
                  RSI_NOTIFY_TXT,
                  RSI_NOTIFY_TXT);
  text_set_font_style(p_txt, FSI_NOTIFY_TXT, FSI_NOTIFY_TXT, FSI_NOTIFY_TXT);
  
  switch(g_notify_type)
  {
    case NOTIFY_TYPE_STRID:
      text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_txt, (u16)g_notify_content);
      break;
    case NOTIFY_TYPE_ASC:
      text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);  
      text_set_content_by_ascstr(p_txt, (u8 *)g_notify_content);
      break;
    case NOTIFY_TYPE_UNI:
      text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE); 
      text_set_content_by_unistr(p_txt, (u16 *)g_notify_content);
      break;
    default:
      MT_ASSERT(0);
  }

  ctrl_paint_ctrl(p_cont, TRUE);

  fw_tmr_create(ROOT_ID_NOTIFY, MSG_EXIT, duration, FALSE);
  
  #ifdef ENABLE_CA
  #ifndef ONLY1_CA_VER
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif
  #endif

  return SUCCESS;
}

void close_notify(void)
{
  if(g_notify_autoclose)
  {
    fw_destroy_mainwin_by_id(ROOT_ID_NOTIFY);
  }
}

static RET_CODE on_notify_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_notify_set_default();
  close_notify();

  fw_tmr_destroy(ROOT_ID_NOTIFY, MSG_EXIT);
  return SUCCESS;
}


BEGIN_MSGPROC(notify_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_notify_exit)
END_MSGPROC(notify_proc, cont_class_proc)

