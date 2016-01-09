/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_movie_search.h"
#include "ui_keyboard_v2.h"
#include "ui_onmov_api.h"

enum control_id
{
  IDC_MOVIE_SEARCH_TITLE = 1,
  IDC_MOVIE_SEARCH_EBOX,
  IDC_MOVIE_SEARCH_CAPS_BMP,
  IDC_MOVIE_SEARCH_CAPS_TXT,
};

enum movie_search_local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 700,
  MSG_CONFIRM,
};

static comm_help_data_t help_data = //help bar data
{
  3,                                    //select  + scroll page + exit
  3,
  {  IDS_INPUT,//,IDS_OK,
     IDS_SEARCH,
     IDS_CANCEL },
  {
     IM_HELP_OK,
     IM_HELP_YELLOW,
     IM_HELP_BLUE },
};


static u8 g_is_upper = FALSE;
static u32 sg_search_id = 0;
u16 movie_search_cont_keymap(u16 key);
RET_CODE movie_search_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 movie_search_name_keymap(u16 key);
RET_CODE movie_search_name_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

extern char* get_cur_web_name();

static RET_CODE on_movie_search_confirm(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;
  u16 *unistr;

  
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_MOVIE_SEARCH_EBOX);
  unistr = ui_comm_t9edit_get_content(p_ctrl);
  if(uni_strlen(unistr) != 0)
  {
    u8 asc_str[64];

    str_uni2asc(asc_str, unistr);
    OS_PRINTF("XUHD: movie search pattern, %s;web,%s\n", asc_str,get_cur_web_name());
    ui_onmov_media_search(get_cur_web_name(),"",asc_str, sg_search_id);

    fw_notify_parent(ROOT_ID_MOVIE_SEARCH, NOTIFY_T_MSG, FALSE,
          MSG_MOVIE_SEARCH_UPDATE, 0, 0);
    manage_close_menu(ROOT_ID_MOVIE_SEARCH, 0, 0);

/*
    if(SUCCESS == fw_notify_parent(
      ROOT_ID_MOVIE_SEARCH, NOTIFY_T_MSG, TRUE,
      MSG_MOVIE_SEARCH_CHECK, (u32)new_name, 0))//check to invalidation of the new string.
    {
      if (uni_strcmp(new_name, movie_search_param.uni_str))
      {
        //rename_param.cb(new_name);
        fw_notify_parent(ROOT_ID_MOVIE_SEARCH, NOTIFY_T_MSG, FALSE,
          MSG_MOVIE_SEARCH_UPDATE, (u32)new_name, 0);
      }
      ret = manage_close_menu(ROOT_ID_MOVIE_SEARCH, 0, 0);
    }
    else
    {
      ui_comm_cfmdlg_open(NULL, IDS_MSG_INVALID_NAME, NULL, 0);
    }
	*/
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_INVALID_NAME, NULL, 0);
  }
  return ret;
}

static RET_CODE on_movie_search_cancle(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
/*
 fw_notify_parent(
      ROOT_ID_MOVIE_SEARCH, NOTIFY_T_MSG, TRUE,
      MSG_MOVIE_SEARCH_CHECK, (u32)new_name, 0);
 */
  return manage_close_menu(ROOT_ID_MOVIE_SEARCH, 0, 0);
}

RET_CODE open_movie_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ebox, *p_title; //*p_caps_bmp;
  g_is_upper = FALSE;

  sg_search_id = para1;
//#ifndef SPT_SUPPORT
  p_cont = fw_create_mainwin(ROOT_ID_MOVIE_SEARCH, MOVIE_SEARCH_CONT_X,
                                  MOVIE_SEARCH_CONT_Y, MOVIE_SEARCH_CONT_W, MOVIE_SEARCH_CONT_H,
                                  (u8)para2,
                                  0, OBJ_ATTR_ACTIVE,
                                  0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);//RSI_RENAME_FRM
  ctrl_set_keymap(p_cont, movie_search_cont_keymap);
  ctrl_set_proc(p_cont, movie_search_cont_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_MOVIE_SEARCH_TITLE,
                             MOVIE_SEARCH_TITLE_X, MOVIE_SEARCH_TITLE_Y, MOVIE_SEARCH_TITLE_W,
                             MOVIE_SEARCH_TITLE_H, p_cont, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);

  text_set_content_by_strid(p_title, IDS_SEARCH);

  p_ebox = ui_comm_t9edit_create(p_cont, IDC_MOVIE_SEARCH_EBOX,
                                   MOVIE_SEARCH_EBOX_X, MOVIE_SEARCH_EBOX_Y,
                                   0, MOVIE_SEARCH_EBOX_W,
                                   ROOT_ID_MOVIE_SEARCH);

  ctrl_set_rstyle(p_ebox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ui_comm_ctrl_set_keymap(p_ebox, movie_search_name_keymap);
  ui_comm_ctrl_set_proc(p_ebox, movie_search_name_proc);

  ui_comm_t9edit_set_static_txt(p_ebox, IDS_NAME);
  ui_comm_t9edit_set_param(p_ebox, 32);
  ui_comm_t9edit_set_content_by_unistr(p_ebox, (u16*)"");
  // add help bar
  ui_comm_help_create_for_pop_dlg(&help_data, p_cont);

  ctrl_default_proc(p_ebox, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}


RET_CODE search_pattern_update(u16 *p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_MOVIE_SEARCH, IDC_MOVIE_SEARCH_EBOX);

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
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = search_pattern_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, ROOT_ID_MOVIE_SEARCH, (u32)&param);
  return SUCCESS;
}


BEGIN_KEYMAP(movie_search_cont_keymap, NULL)
  ON_EVENT(V_KEY_YELLOW, MSG_CONFIRM)
  ON_EVENT(V_KEY_BLUE, MSG_CANCEL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(movie_search_cont_keymap, NULL)

BEGIN_MSGPROC(movie_search_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CANCEL, on_movie_search_cancle)
  ON_COMMAND(MSG_CONFIRM, on_movie_search_confirm)
END_MSGPROC(movie_search_cont_proc, cont_class_proc)

BEGIN_KEYMAP(movie_search_name_keymap, ui_comm_t9_keymap)
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
END_KEYMAP(movie_search_name_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(movie_search_name_proc, ui_comm_t9_proc)
ON_COMMAND(MSG_SELECT, on_edit_name)
ON_COMMAND(MSG_NUMBER, on_edit_name)
END_MSGPROC(movie_search_name_proc, ui_comm_t9_proc)
