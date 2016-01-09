/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_rename.h"

enum control_id
{
  IDC_RENAME_TITLE = 1,
  IDC_RENAME_EBOX,
  IDC_RENAME_CAPS_BMP,
  IDC_RENAME_CAPS_TXT,
};

enum rename_local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 700,
  MSG_CONFIRM,
};

static comm_help_data_t help_data = //help bar data
{
  4,                                    //select  + scroll page + exit
  4,
  { IDS_CAPS,                  IDS_DELETE,                           IDS_OK,
    IDS_CANCEL },
  { IM_EPG_COLORBUTTON_RED,
    IM_EPG_COLORBUTTON_GREEN,
    IM_EPG_COLORBUTTON_YELLOW,
    IM_EPG_COLORBUTTON_BLUE },
};

static u8 g_is_upper = FALSE;
rename_param_t rename_param;
static u16 new_name[MAX_FAV_NAME_LEN + 1];

u16 rename_cont_keymap(u16 key);
RET_CODE rename_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

static RET_CODE on_rename_switch_caps(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;
  
  g_is_upper = !g_is_upper;
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_EBOX);
  ui_comm_t9edit_set_upper(p_ctrl, g_is_upper);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_CAPS_TXT);
  text_set_content_by_ascstr(p_ctrl, g_is_upper ? (u8 *)"ABC" : (u8 *)"abc");
  ctrl_paint_ctrl(p_ctrl, TRUE);

  return ret;
}

static RET_CODE on_rename_confirm(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;
  u16 *unistr;  

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_EBOX);
  unistr = ui_comm_t9edit_get_content(p_ctrl);
  if(uni_strlen(unistr) != 0)
  {
    uni_strcpy(new_name, unistr);

    if(SUCCESS == fw_notify_parent(
      ROOT_ID_RENAME, NOTIFY_T_MSG, TRUE, 
      MSG_RENAME_CHECK, (u32)new_name, 0))//check to invalidation of the new string.
    {
      if (uni_strcmp(new_name, rename_param.uni_str))
      {
        //rename_param.cb(new_name);
        fw_notify_parent(ROOT_ID_RENAME, NOTIFY_T_MSG, FALSE,
          MSG_RENAME_UPDATE, (u32)new_name, 0);
      }
      ret = manage_close_menu(ROOT_ID_RENAME, 0, 0);
    }
    else
    {
      ui_comm_cfmdlg_open(NULL, IDS_MSG_INVALID_NAME, NULL, 0);
    }
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_INVALID_NAME, NULL, 0);
  }

  return ret;
}

static RET_CODE on_rename_backspace(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)  
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_EBOX);
  ui_comm_t9edit_backspace(p_ctrl);
  ui_comm_ctrl_paint_ctrl(p_ctrl, FALSE);  

  return ret;
}

static RET_CODE on_rename_cancle(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  return manage_close_menu(ROOT_ID_RENAME, 0, 0);  
}

RET_CODE open_rename(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ebox, *p_title, *p_caps_txt, *p_caps_bmp;

  g_is_upper = FALSE;
  memcpy((void *)&rename_param, (void *)para1, sizeof(rename_param_t));

#if 1//#ifndef SPT_SUPPORT
  p_cont = fw_create_mainwin(ROOT_ID_RENAME, RENAME_CONT_X,
                                  RENAME_CONT_Y, RENAME_CONT_W, RENAME_CONT_H,
                                  (u8)para2,
                                  0, OBJ_ATTR_ACTIVE,
                                  0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_RENAME_FRM, RSI_RENAME_FRM, RSI_RENAME_FRM);
  ctrl_set_keymap(p_cont, rename_cont_keymap);
  ctrl_set_proc(p_cont, rename_cont_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_RENAME_TITLE,
                             RENAME_TITLE_X, RENAME_TITLE_Y, RENAME_TITLE_W,
                             RENAME_TITLE_H, p_cont, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_RENAME);

  p_caps_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_RENAME_CAPS_BMP,
                                RENAME_CAPS_BMAPX, RENAME_CAPS_BMAPY,
                                RENAME_CAPS_BMAPW,
                                RENAME_CAPS_BMAPH, p_cont,
                                0);
  bmap_set_content_type(p_caps_bmp, FALSE);
  bmap_set_content_by_id(p_caps_bmp, IM_RENAME_PENCIL);

  p_caps_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_RENAME_CAPS_TXT,
                                RENAME_CAPS_TXTX, RENAME_CAPS_TXTY,
                                RENAME_CAPS_TXTW,
                                RENAME_CAPS_TXTH, p_cont,
                                0);;
  ctrl_set_rstyle(p_caps_txt, RSI_COMMAN_MID_BG, RSI_COMMAN_MID_BG, RSI_COMMAN_MID_BG);
  text_set_align_type(p_caps_txt, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_caps_txt, FSI_RENAME_SH, FSI_RENAME_SH, FSI_RENAME_SH);
  text_set_content_type(p_caps_txt, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_caps_txt, g_is_upper ? (u8 *)"ABC" : (u8 *)"abc");

  p_ebox = ui_comm_t9edit_create(p_cont, IDC_RENAME_EBOX,
                                   RENAME_EBOX_X, RENAME_EBOX_Y,
                                   0, RENAME_EBOX_W,
                                   ROOT_ID_RENAME);
  ui_comm_t9edit_set_param(p_ebox, rename_param.max_len);
  ui_comm_t9edit_set_content_by_unistr(p_ebox, rename_param.uni_str);

#else
#endif
  // add help bar
  ui_comm_help_create_for_pop_dlg(&help_data, p_cont);

  ctrl_default_proc(p_ebox, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(rename_cont_keymap, NULL)
  ON_EVENT(V_KEY_RED, MSG_SWITCH_CAPS)
  ON_EVENT(V_KEY_YELLOW, MSG_CONFIRM)
  ON_EVENT(V_KEY_GREEN, MSG_BACKSPACE)
  ON_EVENT(V_KEY_BLUE, MSG_CANCEL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(rename_cont_keymap, NULL)

BEGIN_MSGPROC(rename_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_SWITCH_CAPS, on_rename_switch_caps)
  ON_COMMAND(MSG_CONFIRM, on_rename_confirm)
  ON_COMMAND(MSG_BACKSPACE, on_rename_backspace)
  ON_COMMAND(MSG_CANCEL, on_rename_cancle)
END_MSGPROC(rename_cont_proc, cont_class_proc)

