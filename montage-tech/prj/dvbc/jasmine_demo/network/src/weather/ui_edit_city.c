/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_edit_city.h"
#include "ui_keyboard_v2.h"


enum control_id
{
  IDC_EDIT_CITY_TITLE = 1,
  IDC_EDIT_CITY_EBOX,
  IDC_EDIT_CITY_CAPS_BMP,
  IDC_EDIT_CITY_CAPS_TXT,
};

enum edit_city_local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 700,
  MSG_CONFIRM,
};

static comm_help_data_t help_data = //help bar data
{
  3,                                    //select  + scroll page + exit
  3,
  {  IDS_INPUT,//,IDS_OK,
     IDS_SAVE,
     IDS_CANCEL },
  {
     IM_HELP_OK,
     IM_HELP_YELLOW,
     IM_HELP_BLUE },
};

static u8 g_is_upper = FALSE;
edit_city_param_t edit_city_param;
static u16 new_name[32 + 1];

u16 edit_city_cont_keymap(u16 key);
RET_CODE edit_city_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 edit_city_name_keymap(u16 key);
RET_CODE edit_city_name_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static RET_CODE on_edit_city_confirm(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;
  u16 *unistr;

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_EDIT_CITY_EBOX);
  unistr = ui_comm_t9edit_get_content(p_ctrl);
  if(uni_strlen(unistr) != 0)
  {
    uni_strcpy(new_name, unistr);

    if(SUCCESS == fw_notify_parent(
      ROOT_ID_EDIT_CITY, NOTIFY_T_MSG, TRUE,
      MSG_EDIT_CITY_CHECK, (u32)new_name, 0))//check to invalidation of the new string.
    {
      if (uni_strcmp(new_name, edit_city_param.uni_str))
      {
        //rename_param.cb(new_name);
        fw_notify_parent(ROOT_ID_EDIT_CITY, NOTIFY_T_MSG, FALSE,
          MSG_EDIT_CITY_UPDATE, (u32)new_name, 0);
      }
      ret = manage_close_menu(ROOT_ID_EDIT_CITY, 0, 0);
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

static RET_CODE on_edit_city_cancle(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
 fw_notify_parent(
      ROOT_ID_EDIT_CITY, NOTIFY_T_MSG, TRUE,
      MSG_EDIT_CITY_CHECK, (u32)new_name, 0);
  return manage_close_menu(ROOT_ID_EDIT_CITY, 0, 0);
}

RET_CODE open_edit_city(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ebox, *p_title; //*p_caps_bmp;
  g_is_upper = FALSE;
  memcpy((void *)&edit_city_param, (void *)para1, sizeof(edit_city_param_t));

//#ifndef SPT_SUPPORT
  p_cont = fw_create_mainwin(ROOT_ID_EDIT_CITY, EDIT_CITY_CONT_X,
                                  EDIT_CITY_CONT_Y, EDIT_CITY_CONT_W, EDIT_CITY_CONT_H,
                                  (u8)para2,
                                  0, OBJ_ATTR_ACTIVE,
                                  0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);//RSI_RENAME_FRM
  ctrl_set_keymap(p_cont, edit_city_cont_keymap);
  ctrl_set_proc(p_cont, edit_city_cont_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_EDIT_CITY_TITLE,
                             EDIT_CITY_TITLE_X, EDIT_CITY_TITLE_Y, EDIT_CITY_TITLE_W,
                             EDIT_CITY_TITLE_H, p_cont, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  if(edit_city_param.add_type)
  {
    text_set_content_by_strid(p_title, IDS_ADD);
  }
  else
  {
    text_set_content_by_strid(p_title, IDS_EDIT);
  }
  p_ebox = ui_comm_t9edit_create(p_cont, IDC_EDIT_CITY_EBOX,
                                   EDIT_CITY_EBOX_X, EDIT_CITY_EBOX_Y,
                                   0, EDIT_CITY_EBOX_W,
                                   ROOT_ID_EDIT_CITY);

  ctrl_set_rstyle(p_ebox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ui_comm_ctrl_set_keymap(p_ebox, edit_city_name_keymap);
  ui_comm_ctrl_set_proc(p_ebox, edit_city_name_proc);

  ui_comm_t9edit_set_static_txt(p_ebox, IDS_NAME);
  ui_comm_t9edit_set_param(p_ebox, edit_city_param.max_len);
  ui_comm_t9edit_set_content_by_unistr(p_ebox, edit_city_param.uni_str);
  // add help bar
  ui_comm_help_create_for_pop_dlg(&help_data, p_cont);

  ctrl_default_proc(p_ebox, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}


//void city_name_update(void)
RET_CODE city_name_update(u16 *p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_EDIT_CITY, IDC_EDIT_CITY_EBOX);

  ctrl_paint_ctrl(p_edit_cont, TRUE);  
  return SUCCESS;
}

static RET_CODE on_edit_name(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;
  control_t *p_ebox;

  p_ebox = ctrl_get_parent(p_ctrl);
  param.uni_str = ui_comm_t9edit_get_content(p_ebox);
  param.type = edit_city_param.type;
  param.max_len = 32;
  param.cb = city_name_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, ROOT_ID_EDIT_CITY, (u32) & param);
  return SUCCESS;
}


BEGIN_KEYMAP(edit_city_cont_keymap, NULL)
  ON_EVENT(V_KEY_YELLOW, MSG_CONFIRM)
  ON_EVENT(V_KEY_BLUE, MSG_CANCEL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(edit_city_cont_keymap, NULL)

BEGIN_MSGPROC(edit_city_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CANCEL, on_edit_city_cancle)
  ON_COMMAND(MSG_CONFIRM, on_edit_city_confirm)
END_MSGPROC(edit_city_cont_proc, cont_class_proc)

BEGIN_KEYMAP(edit_city_name_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
END_KEYMAP(edit_city_name_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(edit_city_name_proc, ui_comm_t9_proc)
ON_COMMAND(MSG_SELECT, on_edit_name)
ON_COMMAND(MSG_NUMBER, on_edit_name)
END_MSGPROC(edit_city_name_proc, ui_comm_t9_proc)

