/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
/*!
 \file ui_keyboard_menu.c
   this file  implement the interfaces defined in  ui_keyboard_menu.h, and some internal used functions,
   such as control message process functions, keymap functions.
   All these functions are about create a keyboard menu.
 */

#include "ui_keyboard_menu.h"

/*!
   control id for create keyboard menu.
 */
enum keyboard_control_id
{
  IDC_INVALID = 0,
  IDC_KEYBOARD_EDIT,
  IDC_KEYBOARD_MBOX,
  IDC_KEYBOARD_BACK,
  IDC_KEYBOARD_OK,
  IDC_KEYBOARD_PAGE,
};

#if 0
static s32 keyboard_invalid_dlg_proc(struct control *cont, u16 msg, u32 para1,
                                     u32 para2);
#endif
static s32 close_keyboard_menu(u32 para1, u32 para2);

comm_dlg_data_t keyboard_dlg_invalid = //only show
{
  ROOT_ID_INVALID,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  COMM_DLG_X,
  COMM_DLG_Y,
  COMM_DLG_W,
  COMM_DLG_H,
  IDS_TV_SYSTEM,
  0//keyboard_invalid_dlg_proc,
};

/*!
   max character number in current page.
 */
static u8 max_num[KEYBOARD_TOTAL_PAGE] = { 52, 43 };
/*!
   current page number.
 */
static u8 page = 0;
/*!
   content for edit.
 */
static u16 **g_pp_content = NULL;
/*!
   the control that should be update after editing.
 */
static struct control *g_control = NULL;

static void save_data(void)
{
  u16 *unistr = NULL;
  u32 len;
  struct control *ctrl;
  
  ctrl = ui_comm_root_get_ctrl (ROOT_ID_KEYBOARD, IDC_KEYBOARD_EDIT);
  ebox_class_proc(ctrl, MSG_UNSELECT, 0, 0);
  unistr = ebox_get_content(ctrl);
  len = uni_strlen(unistr);
  if (len == 0)
  {
    ui_comm_dlg_open(&keyboard_dlg_invalid);
    return;
  }

  *g_pp_content = ctrl_unistr_realloc(*g_pp_content, len);
  MT_ASSERT(*g_pp_content != NULL);
  uni_strcpy(*g_pp_content, unistr);
  /* close keyboard */
  close_keyboard_menu(0, 0);
  /* update view */
  if (g_control != NULL)
  {
    ctrl_paint_ctrl(g_control, TRUE);
  }
}

static void keyboard_load_data(struct control *p_ctrl)
{
  u16 index, i;
  u8 ascstr[10];
  u8 temp[2] = "A", numbers;

  for (index = 0; index < KEYBOARD_MBOX_TOL; index++)
  {
    numbers = 0;
    for (i = 0; i < page; i++)
    {
      numbers += max_num[page - 1];
    }
    temp[0] = (u8)(' ' + (u8)index + numbers);
    mbox_set_content_by_ascstr(p_ctrl, index, temp);
  }

  ctrl_paint_ctrl(p_ctrl, TRUE);

  p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_KEYBOARD_PAGE);
  sprintf((char *)ascstr, "Page	%d/%d", (page + 1), KEYBOARD_TOTAL_PAGE);
  text_set_content_by_ascstr(p_ctrl, ascstr);
  ctrl_paint_ctrl(p_ctrl, TRUE);
}

#if 0
static s32 keyboard_invalid_dlg_proc(struct control *cont, u16 msg, u32 para1,
                                     u32 para2)
{
  s32 ret = SUCCESS;
  struct control *ctrl;

  switch (msg)
  {
    case MSG_YES:
      ui_comm_dlg_close();
      cont = desktop_find_root_by_id(ROOT_ID_KEYBOARD);
      ctrl = ctrl_get_child_by_id(cont, IDC_KEYBOARD_EDIT);
      ebox_class_proc(ctrl, MSG_SELECT, para1, para2);
      break;
    default:
      ret = cont_class_proc(cont, msg, para1, para2);
  }

  return ret;
}
#endif

/*static u16 keyboard_menu_text_keymap(struct control *ctrl, u16 key, u32 para1,
                                     u32 para2)*/
static u16 keyboard_menu_text_keymap(u16 key)
{
  u16 msg = MSG_INVALID;

  switch (key)
  {
    case V_KEY_OK:
      msg = MSG_EXIT;
      break;
    default:
      break;
  }
  return msg;
}


static s32 keyboard_menu_text_proc(struct control *ctrl, u16 msg, u32 para1,
                                   u32 para2)
{
  s32 ret = SUCCESS;

  switch (msg)
  {
    case MSG_EXIT:
      if (ctrl->id == IDC_KEYBOARD_OK) //save the new program name
      {
        save_data();
      }
      else //backspace
      {
        ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_KEYBOARD_EDIT);
        ebox_class_proc(ctrl, MSG_BACKSPACE, para1, para2);
      }
      break;
    default:
      ret = text_class_proc(ctrl, msg, para1, para2);
      break;
  }
  return ret;
}


/*static u16 keyboard_menu_mbox_keymap(struct control *ctrl, u16 key, u32 para1,
                                     u32 para2)*/
static u16 keyboard_menu_mbox_keymap(u16 key)
{
  u16 msg = MSG_INVALID;
  static int i = 0;
  i++;

  switch (i%7)
  {
    case 0:
      msg = MSG_FOCUS_UP;
      break;
    case 1:
      msg = MSG_FOCUS_DOWN;
      break;
    case 2:
      msg = MSG_FOCUS_LEFT;
      break;
    case 3:
      msg = MSG_FOCUS_RIGHT;
      break;
    case 4:
      msg = MSG_CHAR;
      break;
     default:
      msg = MSG_CHAR;
      break;
  }
  return msg;
}


static s32 keyboard_menu_mbox_proc(struct control *ctrl, u16 msg, u32 para1,
                                   u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;
  u8 numbers, i;

  focus = mbox_get_focus(ctrl);

  switch (msg)
  {
    case MSG_FOCUS_UP:
      if (focus < KEYBOARD_MBOX_COL) /* focus on top line */
      {
        if (page == 0)               //firset page
        {
          /* pass to root container */
          ret = ERR_NOFEATURE;
        }
        else
        {
          page--;
          focus += KEYBOARD_MBOX_COL * (KEYBOARD_MBOX_ROW - 1);
          while (focus > (max_num[page] - 1))
          {
            focus -= KEYBOARD_MBOX_COL;
          }

          mbox_set_focus(ctrl, focus);
          keyboard_load_data(ctrl);
        }
      }
      else
      {
        ret = mbox_class_proc(ctrl, msg, para1, para2);
      }
      break;
    case MSG_FOCUS_DOWN:
      if ((focus + KEYBOARD_MBOX_COL) > (max_num[page] - 1)) //page down
      {
        if (page == (KEYBOARD_TOTAL_PAGE - 1))               /* focus on bottom line */
        {
          /* pass to root container */
          ret = ERR_NOFEATURE;
        }
        else
        {
          page++;
          focus = focus % KEYBOARD_MBOX_COL;
          mbox_set_focus(ctrl, focus);
          keyboard_load_data(ctrl);
        }
      }
      else
      {
        mbox_class_proc(ctrl, msg, para1, para2);
      }
      break;

    case MSG_CHAR:
      ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_KEYBOARD_EDIT);
      numbers
        = 0;
      for (i = 0; i < page; i++)
      {
        numbers += max_num[i];
      }
      ret = ebox_class_proc(ctrl, (u16)(MSG_CHAR | (focus + ' ' + numbers)), 0, 0);
      break;
    case MSG_FOCUS_RIGHT:
      if((page == (KEYBOARD_TOTAL_PAGE - 1))&&((focus == max_num[KEYBOARD_TOTAL_PAGE - 1] - 1)))
      {
        //mbox_set_focus_draw(ctrl, max_num[KEYBOARD_TOTAL_PAGE - 1]/KEYBOARD_MBOX_COL*KEYBOARD_MBOX_COL);
        break;
      }
      else
      {
        ret = mbox_class_proc(ctrl, msg, para1, para2);
      }
      break;
    case MSG_FOCUS_LEFT:
      if((page == (KEYBOARD_TOTAL_PAGE - 1))&&((focus == max_num[KEYBOARD_TOTAL_PAGE - 1]/KEYBOARD_MBOX_COL*KEYBOARD_MBOX_COL)))
      {
        //mbox_set_focus_draw(ctrl, max_num[KEYBOARD_TOTAL_PAGE - 1] - 1);
        break;
      }
      else
      {
        ret = mbox_class_proc(ctrl, msg, para1, para2);
      }
      break;

    default:
      ret = mbox_class_proc(ctrl, msg, para1, para2);
      break;
  }
  return ret;
}


/*!
   keymap of keyboard menu container.
 \para[in]cont			: control pointer of root container.
 \para[in]key			: key value.
 \para[in]para1			: reserved.
 \para[in]para2			: reserved.
 \Return				: a message.
 */
/*static u16 keyboard_menu_cont_keymap(struct control *cont, u16 key, u32 para1,
                                     u32 para2)*/
static u16 keyboard_menu_cont_keymap(u16 key)
{
  u16 msg = MSG_INVALID;

  switch (key)
  {
    case V_KEY_UP:
      msg = MSG_FOCUS_UP;
      break;
    case V_KEY_DOWN:
      msg = MSG_FOCUS_DOWN;
      break;
    case V_KEY_LEFT:
      msg = MSG_FOCUS_LEFT;
      break;
    case V_KEY_RIGHT:
      msg = MSG_FOCUS_RIGHT;
      break;

    case V_KEY_CANCEL:
      msg = MSG_EXIT;
      break;

    default:
      ;
//    case V_KEY_MENU:
//      msg = MSG_EXIT_ALL;
//      break;
  }
  
  return msg;
}


/*!
   process function of keyboard menu container

 \para[in]cont			: control pointer of root container.
 \para[in]msg			: message value.
 \para[in]para1			: reserved.
 \para[in]para2			: reserved.
 \Return				: SUCCESS or ERR_NOFEATURE.
 */
static s32 keyboard_menu_cont_proc(struct control *cont, u16 msg, u32 para1,
                                   u32 para2)
{
  s32 ret = SUCCESS;

  switch (msg)
  {
//    case MSG_EXIT_ALL:
//      ui_close_all_mennus();
//      break;
    case MSG_EXIT:
      save_data();
//      cont = ctrl_get_child_by_id(cont, IDC_KEYBOARD_EDIT);
//      ebox_class_proc(cont, MSG_BACKSPACE, para1, para2);
      break;
    default:
      ret = cont_class_proc(cont, msg, para1, para2);
  }
  return ret;
}


/*!
   open a keyboard menu.

 \para[in]para1			: reserved.
 \para[in]para2			: reserved.
 \Return					: SUCCESS or ERR_FAILURE
 */
s32 open_keyboard_menu(u32 content, u32 ctrl)
//s32 open_keyboard_menu(u16 **content, struct control *ctrl)
{
  struct control *p_cont, *p_edit, *p_mbox, *p_btn[2], *p_page;
  u8 ascstr[16];
  u16 index;
  u8 i, temp[2] = "A";
  
#if 1//#ifndef SPT_SUPPORT
  u16 x[2] = { KEYBOARD_BACK_X, KEYBOARD_OK_X };
  u16 strid[2] = { IDS_DELETE, IDS_OK };
#endif

  g_pp_content = (u16 **)content;
  g_control = (struct control *)ctrl;

  page = 0;
#if 1//#ifndef SPT_SUPPORT
  /*create keyboard menu container*/
  p_cont = fw_create_mainwin(ROOT_ID_KEYBOARD,
                                  KEYBOARD_MENU_CONT_X, KEYBOARD_MENU_CONT_Y,
                                  KEYBOARD_MENU_CONT_W, KEYBOARD_MENU_CONT_H,
                                  0, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont,
                  RSI_KEYBOARD_MENU_FRM,
                  RSI_KEYBOARD_MENU_FRM,
                  RSI_KEYBOARD_MENU_FRM);
  ctrl_set_keymap(p_cont, keyboard_menu_cont_keymap);
  ctrl_set_proc(p_cont, keyboard_menu_cont_proc);

  /*create edit box for show the input character*/
  p_edit = ctrl_create_ctrl(CTRL_EBOX, IDC_KEYBOARD_EDIT,
                            KEYBOARD_EDIT_X, KEYBOARD_EDIT_Y,
                            KEYBOARD_EDIT_W, KEYBOARD_EDIT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_edit,
                  RSI_EDIT_KEYBOARD,
                  RSI_EDIT_KEYBOARD,
                  RSI_EDIT_KEYBOARD);
  ebox_set_maxtext(p_edit, DB_DVBS_MAX_NAME_LENGTH);
  //ebox_set_content_by_unistr(p_edit, *g_pp_content);

  /*create the keyboard*/
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_KEYBOARD_MBOX, KEYBOARD_MBOX_X,
                            KEYBOARD_MBOX_Y, KEYBOARD_MBOX_W, KEYBOARD_MBOX_H,
                            p_cont,
                            0);
  ctrl_set_keymap(p_mbox, keyboard_menu_mbox_keymap);
  ctrl_set_proc(p_mbox, keyboard_menu_mbox_proc);
  ctrl_set_rstyle(p_mbox, RSI_MBOX_KEYBOARD, RSI_MBOX_KEYBOARD,
                  RSI_MBOX_KEYBOARD);
  ctrl_set_mrect(p_mbox, 0, 0, KEYBOARD_MBOX_W, KEYBOARD_MBOX_H);                  
  mbox_enable_icon_mode(p_mbox, FALSE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, KEYBOARD_MBOX_TOL, KEYBOARD_MBOX_COL,
                 KEYBOARD_MBOX_ROW);
  mbox_set_item_interval(p_mbox, KEYBOARD_ITEM_INTERVAL_H, KEYBOARD_ITEM_INTERVAL_V);
  mbox_set_item_rstyle(p_mbox, RSI_MBOX_KEYBOARD_HL,
                      RSI_MBOX_KEYBOARD_SH, RSI_MBOX_KEYBOARD_SH);
  mbox_set_string_fstyle(p_mbox, FSI_MBOX_KEYBOARD_TEXT_HL,
                        FSI_MBOX_KEYBOARD_TEXT_SH, FSI_MBOX_KEYBOARD_TEXT_SH);
  mbox_set_string_offset(p_mbox, 0, 0);
  mbox_set_string_align_type(p_mbox, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_UNICODE);
  for (index = 0; index < KEYBOARD_MBOX_TOL; index++)
  {
    temp[0] = (u8)(' ' + (u8)index);
    mbox_set_content_by_ascstr(p_mbox, index, temp);
  }

  ctrl_set_related_id(p_mbox, IDC_KEYBOARD_MBOX, IDC_KEYBOARD_BACK,
                      IDC_KEYBOARD_MBOX,
                      IDC_KEYBOARD_BACK);


  /*create backspace button*/
  for (i = 0; i < 2; i++)
  {
    p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_KEYBOARD_BACK+i,
                                x[i], KEYBOARD_BACK_Y,
                                KEYBOARD_BACK_W, KEYBOARD_BACK_H,
                                p_cont, 0);
    ctrl_set_keymap(p_btn[i], keyboard_menu_text_keymap);
    ctrl_set_proc(p_btn[i], keyboard_menu_text_proc);
    ctrl_set_rstyle(p_btn[i],
                    RSI_TEXT_KEYBOARD_SH,
                    RSI_TEXT_KEYBOARD_HL,
                    RSI_TEXT_KEYBOARD_SH);
    text_set_font_style(p_btn[i],
                        FSI_MBOX_KEYBOARD_TEXT_SH,
                        FSI_MBOX_KEYBOARD_TEXT_HL,
                        FSI_MBOX_KEYBOARD_TEXT_SH);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], strid[i]);
  }

  ctrl_set_related_id(p_btn[0],
                      IDC_KEYBOARD_OK, IDC_KEYBOARD_MBOX,
                      IDC_KEYBOARD_OK, IDC_KEYBOARD_MBOX);
  ctrl_set_related_id(p_btn[1],
                      IDC_KEYBOARD_BACK, IDC_KEYBOARD_MBOX,
                      IDC_KEYBOARD_BACK, IDC_KEYBOARD_MBOX);

  p_page = ctrl_create_ctrl(CTRL_TEXT, IDC_KEYBOARD_PAGE,
                            KEYBOARD_PAGE_X, KEYBOARD_PAGE_Y,
                            KEYBOARD_PAGE_W, KEYBOARD_PAGE_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_page,
                  RSI_KEYBOARD_PAGE,
                  RSI_KEYBOARD_PAGE,
                  RSI_KEYBOARD_PAGE);
  text_set_font_style(p_page, FSI_TXT, FSI_TXT, FSI_TXT);
  text_set_content_type(p_page, TEXT_STRTYPE_UNICODE);
  sprintf((char *)ascstr, "Page	%d/%d", (page + 1), KEYBOARD_TOTAL_PAGE);
  text_set_content_by_ascstr(p_page, ascstr);
#else
  p_cont = spt_load_menu(ROOT_ID_KEYBOARD);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, keyboard_menu_cont_keymap);
  ctrl_set_proc(p_cont, keyboard_menu_cont_proc);

  p_edit = ctrl_get_child_by_id(p_cont, IDC_KEYBOARD_EDIT);
  ebox_set_maxtext(p_edit, DB_DVBS_MAX_NAME_LENGTH);
  ebox_set_content_by_unistr(p_edit, *g_pp_content);

  p_mbox = ctrl_get_child_by_id(p_cont, IDC_KEYBOARD_MBOX);
  ctrl_set_keymap(p_mbox, keyboard_menu_mbox_keymap);
  ctrl_set_proc(p_mbox, keyboard_menu_mbox_proc);
  mbox_set_count(p_mbox, KEYBOARD_MBOX_TOL, KEYBOARD_MBOX_COL,
                 KEYBOARD_MBOX_ROW);
  for (index = 0; index < KEYBOARD_MBOX_TOL; index++)
  {
    temp[0] = (u8)(' ' + (u8)index);
    mbox_set_content_by_ascstr(p_mbox, index, temp);
  }

  for (i = 0; i < 2; i++)
  {
    p_btn[i] = ctrl_get_child_by_id(p_cont, IDC_KEYBOARD_BACK+i);
    ctrl_set_keymap(p_btn[i], keyboard_menu_text_keymap);
    ctrl_set_proc(p_btn[i], keyboard_menu_text_proc);
  }

  p_page = ctrl_get_child_by_id(p_cont, IDC_KEYBOARD_PAGE);
  sprintf(ascstr, "Page	%d/%d", (page + 1), KEYBOARD_TOTAL_PAGE);
  text_set_content_by_ascstr(p_page, ascstr);
#endif

  /* enter edit status */
  ebox_enter_edit(p_edit);

  /*draw the whole menu*/
  ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


/*!
   close a keyboard menu.

 \para[in]para1			: reserved.
 \para[in]para2			: reserved.
 \Return					: SUCCESS or ERR_FAILURE
 */
static s32 close_keyboard_menu(u32 para1, u32 para2)
{
  if (!fw_destroy_mainwin_by_id(ROOT_ID_KEYBOARD))
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}


