/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/

#include "ui_common.h"

#include "ui_game_othello.h"
#include "game_othello.h"
#include "ui_game_common.h"

static u16 BlackPoint, WhitePoint, cur_col, cur_row;
static board_type chess_board;
static u8 cpu_color;//othello_game = OTHELLO_OVER;

void callback(u8 type, s32 wparam, u32 lparam);
static void othello_update_status(void);
void othello_initial(BOOL BOW);
void othello_draw_background(BOOL is_redraw_bg);
void othello_draw_chess(control_t *p_mbox, u16 col_num, u16 row_num,
                        BOOL color);
void othello_draw_cursor(control_t *p_mbox);

u16 othello[OTHELLO_INFO_ITEM_COUNT] =
{
  IDS_GAME_OTHELLO, IDS_START,   IDS_GAME_LEVEL, IDS_EXIT,
  IDS_GAME_CLASS,             0, IDS_GAME_BLACK,        0, IDS_GAME_WHITE,
};

static comm_dlg_data_t color_choose_dlg =
{
  ROOT_ID_OTHELLO,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  COMM_DLG_X,                        COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_GAME_MSG_FOR_BLACK_FIRST,
  0,
};

static comm_dlg_data_t othello_quit_dlg =
{
  ROOT_ID_OTHELLO,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  COMM_DLG_X,                        COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_GAME_MSG_FOR_EXIT,
  0,
};

static comm_dlg_data_t othello_draw_dlg =
{
  ROOT_ID_OTHELLO,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  COMM_DLG_X,                            COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_GAME_MSG_YOU_WIN,
  0,
};

static comm_dlg_data_t othello_win_dlg =
{
  ROOT_ID_OTHELLO,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  COMM_DLG_X,                            COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_GAME_MSG_YOU_WIN,
  0,
};

static comm_dlg_data_t othello_lose_dlg =
{
  ROOT_ID_OTHELLO,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  COMM_DLG_X,                            COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_GAME_MSG_YOU_LOSE,
};

RET_CODE othello_board_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE othello_level_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE othello_se_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);


static RET_CODE on_othello_board_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  BOOL ret;
  
  if (do_move_chess(&chess_board,
                    (u16)((cur_row + 1) * 10 + (cur_col + 1)),
                    (u8)(~computer_side & 3), callback))
  {
    //othello_game = OTHELLO_HAPPY;
    get_chess_score(&chess_board, &WhitePoint, &BlackPoint);
    othello_update_status();
    othello_draw_background(FALSE);
    /* computer round */
    ret = computer_play(&chess_board, callback);
    get_chess_score(&chess_board, &WhitePoint, &BlackPoint);
    othello_update_status();
    othello_draw_background(FALSE);
    
    if(!ret)
    {
      game_over(&chess_board, callback);
    }
  }  

  return SUCCESS;
}

static RET_CODE on_othello_board_focus_up(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (cur_col == 0)
  {
    cur_col = OTHELLO_COL_NUM - 1;
  }
  else
  {
    cur_col--;
  }
  
  othello_draw_cursor(p_ctrl);

  return SUCCESS;
}

static RET_CODE on_othello_board_focus_down(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (cur_col == (OTHELLO_COL_NUM - 1))
  {
    cur_col = 0;
  }
  else
  {
    cur_col++;
  }
  othello_draw_cursor(p_ctrl);

  return SUCCESS;
}

static RET_CODE on_othello_board_focus_left(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (cur_row == 0)
  {
    cur_row = OTHELLO_ROW_NUM - 1;
  }
  else
  {
    cur_row--;
  }
  othello_draw_cursor(p_ctrl);

  return SUCCESS;
}

static RET_CODE on_othello_board_focus_right(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (cur_row == (OTHELLO_ROW_NUM - 1))
  {
    cur_row = 0;
  }
  else
  {
    cur_row++;
  }
  othello_draw_cursor(p_ctrl);

  return SUCCESS;
}

static RET_CODE on_othello_board_exit(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if(ui_comm_dlg_open(&othello_quit_dlg) == DLG_RET_YES)
  {
    game_over_common(ROOT_ID_OTHELLO);
		WhitePoint = 0;
		BlackPoint = 0;
		othello_update_status();
  }

  return SUCCESS;
}

static RET_CODE on_othello_se_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  u8 ctrl_id;
  
  ctrl_id = (u8)ctrl_get_ctrl_id(p_ctrl);

  if (2 == ctrl_id)
  {
    if(ui_comm_dlg_open(&color_choose_dlg) == DLG_RET_YES)//start
    {
      othello_initial(TRUE); //Ö´ºÚÏÈ×ß
    }
    else
    {
      othello_initial(FALSE); //Ö´°×
    }
  }
  else if (4 == ctrl_id)
  {
    manage_close_menu(ROOT_ID_OTHELLO, 0, 0);
  }

  return SUCCESS;
}

static RET_CODE on_othello_se_exit(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  return manage_close_menu(ROOT_ID_OTHELLO, 0, 0);
}

static RET_CODE on_othello_level_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_item;
  
  p_item = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), 6);

  return cbox_class_proc(p_item, msg, para1, para2);  

  //return ctrl_process_msg(p_item, msg, 0, 0);
}


static RET_CODE on_othello_level_exit(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  return manage_close_menu(ROOT_ID_OTHELLO, 0, 0);
}

RET_CODE open_game_othello(u32 para1, u32 para2)
{
  control_t *p_mask, *p_cont, *p_chessboard, *p_info;
  control_t *p_ctrl[OTHELLO_INFO_ITEM_COUNT];
  u16 i;

  u16 item_y;
  memset(p_ctrl, 0 ,sizeof( control_t *) * OTHELLO_INFO_ITEM_COUNT);
  p_mask = fw_create_mainwin(ROOT_ID_OTHELLO, 0, 0, SCREEN_WIDTH,
                                  SCREEN_HEIGHT, 0, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_mask != NULL);

  ctrl_set_rstyle(p_mask, RSI_OTHELLO_DESKTOP,
                  RSI_OTHELLO_DESKTOP, RSI_OTHELLO_DESKTOP);

  // create container
  p_cont = ctrl_create_ctrl(CTRL_CONT, GAME_CONT_ID,
                            OTHELLO_CONT_X, OTHELLO_CONT_Y, OTHELLO_CONT_W,
                            OTHELLO_CONT_H, p_mask, 0);
  ctrl_set_rstyle(p_cont, RSI_OTHELLO_CONT, RSI_OTHELLO_CONT,
                  RSI_OTHELLO_CONT);

  MT_ASSERT(p_cont != NULL);

  //Chess board
  p_chessboard = ctrl_create_ctrl(CTRL_MBOX, GAME_BOARD_ID,
                                  OTHELLO_BOARD_X, OTHELLO_BOARD_Y,
                                  OTHELLO_BOARD_W, OTHELLO_BOARD_H, p_cont, 0);
  ctrl_set_keymap(p_chessboard, game_board_keymap);
  ctrl_set_proc(p_chessboard, othello_board_proc);
  ctrl_set_rstyle(p_chessboard, RSI_OTHELLO_BOARD,
                  RSI_OTHELLO_BOARD, RSI_OTHELLO_BOARD);
  ctrl_set_mrect(p_chessboard, OTHELLO_BOARD_IL, OTHELLO_BOARD_IT,
    OTHELLO_BOARD_IR, OTHELLO_BOARD_IB);                  
  mbox_enable_string_mode(p_chessboard, FALSE);
  mbox_set_count(p_chessboard, OTHELLO_TOT_NUM,
                 OTHELLO_COL_NUM, OTHELLO_ROW_NUM);
  mbox_set_item_interval(p_chessboard, 0, 0);
  mbox_set_item_rstyle(p_chessboard, RSI_OTHELLO_ITEM_HL,
                      RSI_OTHELLO_ITEM_SH, RSI_OTHELLO_ITEM_SH);

  mbox_set_icon_offset(p_chessboard, 0, 0);
  mbox_set_icon_align_type(p_chessboard, STL_CENTER | STL_VCENTER);
  mbox_set_focus(p_chessboard, (3 * OTHELLO_ROW_NUM) + 2);
  for (i = 0; i < OTHELLO_COL_NUM * OTHELLO_ROW_NUM; i++)
  {
    mbox_set_content_by_icon(p_chessboard, i, 0, 0);
  }

  //othello info container
  p_info = ctrl_create_ctrl(CTRL_CONT, GAME_INFO_ID, OTHELLO_INFO_X,
                            OTHELLO_INFO_Y, OTHELLO_INFO_W, OTHELLO_INFO_H,
                            p_cont,
                            0);
  ctrl_set_keymap(p_info, game_info_keymap);
  ctrl_set_rstyle(p_info, RSI_OTHELLO_INFO, RSI_OTHELLO_INFO,
                  RSI_OTHELLO_INFO);

  item_y = OTHELLO_INFO_ITEM_Y;

  for (i = 0; i < OTHELLO_INFO_ITEM_COUNT; i++)
  {
    switch (i)
    {
      case 0:
      case 4:
      case 6:
      case 8:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1),
                                     OTHELLO_INFO_ITEM_X, item_y,
                                     OTHELLO_INFO_ITEM_W,
                                     OTHELLO_INFO_ITEM_H, p_info,
                                     0);
        text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], othello[i]);
        break;
      case 1:
      case 3:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1),
                                     OTHELLO_INFO_ITEM_X, item_y,
                                     OTHELLO_INFO_ITEM_W,
                                     OTHELLO_INFO_ITEM_H, p_info,
                                     0);
        ctrl_set_keymap(p_ctrl[i], game_se_keymap);
        ctrl_set_proc(p_ctrl[i], othello_se_proc);


        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH,
                        RSI_GAME_ITEM_HL, RSI_COMM_STATIC_GRAY);
        text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH,
                            FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], othello[i]);
        ctrl_set_related_id(p_ctrl[i], 0, (u8)((i - 2 + 3) % 3 + 2),
                            0, (u8)((i) % 3 + 2));
        break;
      case 2:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1),
                                     OTHELLO_INFO_ITEM_X, item_y,
                                     OTHELLO_INFO_ITEM_W,
                                     OTHELLO_INFO_ITEM_H, p_info,
                                     0);
        ctrl_set_keymap(p_ctrl[i], game_level_keymap);
        ctrl_set_proc(p_ctrl[i], othello_level_proc);


        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_GAME_SELECT_HL,
                        RSI_COMM_STATIC_GRAY);
        text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH,
                            FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], othello[i]);
        ctrl_set_related_id(p_ctrl[i], 0, (u8)((i - 2 + 3) % 3 + 2),
                            0, (u8)((i) % 3 + 2));
        break;
      case 5:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_CBOX, (u8)(i + 1),
                                     OTHELLO_INFO_ITEM_X, item_y,
                                     OTHELLO_INFO_ITEM_W,
                                     OTHELLO_INFO_ITEM_H, p_info,
                                     0);
        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH,
                        RSI_COMM_STATIC_HL, RSI_COMM_STATIC_GRAY);
        cbox_enable_cycle_mode(p_ctrl[i], TRUE);
        cbox_set_work_mode(p_ctrl[i], CBOX_WORKMODE_STATIC);
        cbox_set_align_style(p_ctrl[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH,
                            FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
        cbox_static_set_count(p_ctrl[i], 3);
        cbox_static_set_content_type(p_ctrl[i], CBOX_ITEM_STRTYPE_STRID);
        cbox_static_set_content_by_strid(p_ctrl[i], 0, IDS_GAME_EASY);
        cbox_static_set_content_by_strid(p_ctrl[i], 1, IDS_GAME_NORMAL);
        cbox_static_set_content_by_strid(p_ctrl[i], 2, IDS_GAME_HARD);
        cbox_static_set_focus(p_ctrl[i], 0);
        break;
      case 7:
      case 9:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1),
                                     OTHELLO_INFO_ITEM_X, item_y,
                                     OTHELLO_INFO_ITEM_W,
                                     OTHELLO_INFO_ITEM_H, p_info,
                                     0);
        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH,
                        RSI_COMM_STATIC_HL, RSI_COMM_STATIC_GRAY);
        text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH,
                            FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_DEC);
        text_set_content_by_dec(p_ctrl[i], 0);
        break;
      default:
        break;
    }

    if ((i == 0) || (i == 1) || (i == 2) || (i == 3))
    {
      item_y += (OTHELLO_INFO_ITEM_H + OTHELLO_INFO_ITEM_I);
    }
    else
    {
      item_y += (OTHELLO_INFO_ITEM_H);
    }

    if (i == 3)
    {
      item_y += (OTHELLO_INFO_ITEM_H + OTHELLO_INFO_ITEM_I);
    }
  }

  ctrl_default_proc(p_ctrl[1], MSG_GETFOCUS, 0, 0);


  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


void othello_draw_chess(control_t* p_mbox, u16 col_num, u16 row_num,
                        BOOL color)
{
  u16 focus;
  focus = (u16)((col_num * OTHELLO_ROW_NUM) + row_num);

  mbox_set_content_by_icon(p_mbox, focus,
                           (u16)(color ? OTHELLO_BLACK_BMP : OTHELLO_WHITE_BMP),
                           (u16)(color ? OTHELLO_BLACK_BMP : OTHELLO_WHITE_BMP));
  mbox_draw_item_ext(p_mbox, focus, TRUE);
}


void othello_initial(BOOL BOW)
{
  control_t *p_mbox, *p_info;

  p_info = ui_game_get_ctrl(ROOT_ID_OTHELLO, GAME_INFO_ID);
  p_info = ctrl_get_child_by_id(p_info, 6);

  g_iGameLevel =
    (text_get_content(p_info) == IDS_GAME_EASY) ? OTHELLO_GAME_LEVEL_PRM :
    (text_get_content(p_info) ==
     IDS_GAME_NORMAL ? OTHELLO_GAME_LEVEL_JUR : OTHELLO_GAME_LEVEL_SER);

  p_info = ctrl_get_child_by_id(p_info->p_parent, 2);

  ctrl_default_proc(p_info, MSG_LOSTFOCUS, 0, 0);
  ctrl_paint_ctrl(p_info, TRUE);
  p_mbox = ui_game_get_ctrl(ROOT_ID_OTHELLO, GAME_BOARD_ID);
  ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_mbox, TRUE);

  cur_col = 3;
  cur_row = 2;
  BlackPoint = 2;
  WhitePoint = 2;

  if (!BOW) //if user is white, cpu first
  {
    init_board(&chess_board, FALSE);
    cpu_color = CHESS_BLACK;
    computer_play(&chess_board, callback);
    get_chess_score(&chess_board, &WhitePoint, &BlackPoint);
    //othello_game = OTHELLO_HAPPY;
  }
  else
  {
    init_board(&chess_board, TRUE);
    cpu_color = CHESS_WHITE;
    //othello_game = OTHELLO_HAPPY;
  }

  othello_update_status();
  othello_draw_background(TRUE);
}


static void othello_update_status(void)
{
  control_t *p_info;
  p_info = ui_game_get_ctrl(ROOT_ID_OTHELLO, GAME_INFO_ID);
  p_info = ctrl_get_child_by_id(p_info, 8);
  text_set_content_by_dec(p_info, BlackPoint);
  ctrl_paint_ctrl(p_info, TRUE);
  p_info = ctrl_get_child_by_id(p_info->p_parent, 10);
  text_set_content_by_dec(p_info, WhitePoint);
  ctrl_paint_ctrl(p_info, TRUE);
}


void othello_draw_cursor(control_t *p_mbox)
{
  u16 new_focus, old_focus;

  new_focus = (u16)((cur_col * OTHELLO_ROW_NUM) + cur_row);
  old_focus = mbox_get_focus(p_mbox);
//	if(new_focus == old_focus)
//		return;

  mbox_set_focus(p_mbox, new_focus);
  mbox_draw_item_ext(p_mbox, new_focus, TRUE);
  mbox_draw_item_ext(p_mbox, old_focus, TRUE);
}


void othello_draw_background(BOOL is_redraw_bg)
{
  control_t *p_mbox;
  u8 i, j;
  u16 focus, orig_icon, new_icon;

  p_mbox = ui_game_get_ctrl(ROOT_ID_OTHELLO, GAME_BOARD_ID);

  WhitePoint = 0;
  BlackPoint = 0;

  for (i = 0; i < OTHELLO_COL_NUM; i++)
  {
    for (j = 0; j < OTHELLO_ROW_NUM; j++)
    {
      focus = (i * OTHELLO_ROW_NUM) + j;
      orig_icon = mbox_get_normal_icon(p_mbox, focus);
      switch (chess_board.board[j + 1][i + 1])
      {
        case CHESS_WHITE:
          new_icon = OTHELLO_WHITE_BMP;
          WhitePoint++;
          break;
        case CHESS_BLACK:
          new_icon = OTHELLO_BLACK_BMP;
          BlackPoint++;
          break;
        default:
          new_icon = 0;
      }

      if (orig_icon != new_icon)
      {
        mbox_set_content_by_icon(p_mbox, focus,
                                 new_icon, new_icon);
        mbox_draw_item_ext(p_mbox, focus, TRUE);
      }
    }
  }

  othello_draw_cursor(p_mbox);
}


void callback(u8 type, s32 wparam, u32 lparam)
{
  u8 winner;

  switch (type)
  {
    case TRANCHESS:
      if (lparam == cpu_color)
      {
        cur_row = (u16)(wparam / 10 - 1);
        cur_col = (u16)((u16)wparam % 10 - 1);
      }
      break;
    case GAME_OVER:
      //othello_game = OTHELLO_OVER;
      winner = (u8)wparam;
      othello_update_status();
      if (COMPUTER_WIN == winner)  //"Sorry! You lost!"
      {                            //"Sorry! You lost!"
        if(ui_comm_dlg_open(&othello_lose_dlg) == DLG_RET_YES)
        {
          game_over_common(ROOT_ID_OTHELLO);
        }
      }
      else if (USER_WIN == winner) //"You Win!"
      {
        //"You Win!"
        if(ui_comm_dlg_open(&othello_win_dlg) == DLG_RET_YES)
        {
          game_over_common(ROOT_ID_OTHELLO);
        }
      }
      else
      {
        //"Draw!"
        if(ui_comm_dlg_open(&othello_draw_dlg) == DLG_RET_YES)
        {
          game_over_common(ROOT_ID_OTHELLO);
        }
      }
      break;
    default:
      break;
  }
}


BEGIN_MSGPROC(othello_board_proc, mbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_othello_board_select)
  ON_COMMAND(MSG_FOCUS_UP, on_othello_board_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_othello_board_focus_down)
  ON_COMMAND(MSG_FOCUS_LEFT, on_othello_board_focus_left)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_othello_board_focus_right)
  ON_COMMAND(MSG_EXIT, on_othello_board_exit)
END_MSGPROC(othello_board_proc, mbox_class_proc)

BEGIN_MSGPROC(othello_level_proc, text_class_proc)
  ON_COMMAND(MSG_INCREASE, on_othello_level_change)
  ON_COMMAND(MSG_DECREASE, on_othello_level_change)
  ON_COMMAND(MSG_EXIT, on_othello_level_exit)
END_MSGPROC(othello_level_proc, text_class_proc)

BEGIN_MSGPROC(othello_se_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_othello_se_select)
  ON_COMMAND(MSG_EXIT, on_othello_se_exit)
END_MSGPROC(othello_se_proc, text_class_proc)


