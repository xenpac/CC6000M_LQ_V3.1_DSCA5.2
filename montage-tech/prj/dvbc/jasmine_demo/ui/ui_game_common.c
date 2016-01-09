/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_game_common.h"

/*!
   Function:
   Get the pointer of a specified child control
   Parameter:
    u8 root_id		[in]:	root container id
    u8 child_id		[in]:	the specified child control id
   Return:
   the pointer of a specified child control
 */
control_t *ui_game_get_ctrl(u8 root_id, u8 child_id)
{
  control_t *p_child;

  p_child = fw_find_root_by_id(root_id);
  p_child = ctrl_get_child_by_id(p_child, GAME_CONT_ID);
  p_child = ctrl_get_child_by_id(p_child, child_id);

  return p_child;
}

static void empty_board(control_t *p_board)
{
  u16 i,  total;
  u16 col, row;

  mbox_get_count(p_board, &total, &col, &row);

  for (i = 0; i < total; i++)
  {
    mbox_set_content_by_icon(p_board, i, 0, 0);
  }
}

void game_over_common(u8 root_id)
{
  control_t *p_mbox, *p_info;

  p_mbox = ui_game_get_ctrl(root_id, GAME_BOARD_ID);
  p_info = ui_game_get_ctrl(root_id, GAME_INFO_ID);

  empty_board(p_mbox);
  ctrl_default_proc(p_mbox, MSG_LOSTFOCUS, 0, 0);
  ctrl_paint_ctrl(p_mbox, FALSE);

  p_info = ctrl_get_child_by_id(p_info, 2);
  ctrl_default_proc(p_info, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_info, FALSE);
}

BEGIN_KEYMAP(game_level_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(game_level_keymap, NULL)

BEGIN_KEYMAP(game_se_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(game_se_keymap, NULL)

BEGIN_KEYMAP(game_board_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(game_board_keymap, NULL)

BEGIN_KEYMAP(game_info_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
END_KEYMAP(game_info_keymap, NULL)

