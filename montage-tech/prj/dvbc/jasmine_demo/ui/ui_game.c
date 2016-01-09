/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_game.h"

enum control_id
{
  IDC_GAME_OTHELLO = 1,
  IDC_GAME_TETRIS,
};

/*
static comm_help_data_t game_help_data = 
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
static u16 game_keymap(u16 key);


static RET_CODE game_proc(control_t *ctrl, u16 msg, u32 para1,
                                    u32 para2);

RET_CODE open_game(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_ctrl[GAME_ITEM_CNT];
  u32 i = 0, y = 0;
  u8 str_id[GAME_ITEM_CNT] = {IDS_GAME_OTHELLO, IDS_GAME_TETRIS};

  p_cont = ui_comm_root_create(ROOT_ID_GAME, 0, COMM_BG_X, COMM_BG_Y,
    COMM_BG_W, COMM_BG_H, 0, IDS_GAME);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, game_keymap);
  ctrl_set_proc(p_cont, game_proc);

  y = GAME_ITEM_Y;

  for (i = 0; i < GAME_ITEM_CNT; i++)
  {
    p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_GAME_OTHELLO + i),
      GAME_ITEM_X, (u16)y, GAME_ITEM_W, GAME_ITEM_H, p_cont, 0);
    ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_ITEM_1_HL, RSI_PBACK);
    text_set_font_style(p_ctrl[i], FSI_COMM_TXT_SH, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
    text_set_offset(p_ctrl[i], 20, 0);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl[i], str_id[i]);

    ctrl_set_related_id(p_ctrl[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              GAME_ITEM_CNT) %
                             GAME_ITEM_CNT + 1),            /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % GAME_ITEM_CNT + 1)); /* down */

    y += GAME_ITEM_H + GAME_ITEM_V_GAP;
  }
  //ui_comm_help_create(&game_help_data, p_cont);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_game_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 focus_id  = 0, root_id = 0;
  control_t *p_active = NULL;

  p_active = ctrl_get_active_ctrl(p_ctrl);
  if(p_active == NULL)
  {
    return ERR_FAILURE;
  }


  focus_id = (u8)ctrl_get_ctrl_id(p_active);

  switch (focus_id)
  {
    case IDC_GAME_OTHELLO:
      root_id = ROOT_ID_OTHELLO;
      break;
    case IDC_GAME_TETRIS:
      root_id = ROOT_ID_TETRIS;
      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
    }

  return manage_open_menu(root_id, 0, 0);
}


BEGIN_KEYMAP(game_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_GAME, MSG_EXIT_ALL)
END_KEYMAP(game_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(game_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_game_select)
END_MSGPROC(game_proc, ui_comm_root_proc)


