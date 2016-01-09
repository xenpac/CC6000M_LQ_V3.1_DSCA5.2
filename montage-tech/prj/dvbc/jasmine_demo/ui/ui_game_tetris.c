/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_game_tetris.h"
#include "ui_game_common.h"

u16 tetris[TETRIS_INFO_ITEM_COUNT] =
{
  IDS_GAME_TETRIS, IDS_START, IDS_GAME_LEVEL, IDS_EXIT, 0,
  IDS_GAME_CLASS, 0, IDS_GAME_LINES, 0, IDS_GAME_SCORE,
};

enum local_msg
{
  MSG_TETRIS_TMR = MSG_LOCAL_BEGIN + 975,
};

static u32 t_mark = 0, t_erasered = 0;
static u8 t_speed = 1; // 1- 10
static u16 t_timedelay[10] = { 800, 700, 600, 500, 400, 300, 250, 200, 150, 100 };
static u8 bmp_id[7] = {IM_GAME_TETRIS_BLOCK_01, IM_GAME_TETRIS_BLOCK_02,
    IM_GAME_TETRIS_BLOCK_03, IM_GAME_TETRIS_BLOCK_04, IM_GAME_TETRIS_BLOCK_05,
    IM_GAME_TETRIS_BLOCK_06, IM_GAME_TETRIS_BLOCK_07};
static u8 t_newcolor;
static u8 t_curncolor;
static u8 t_newshape[2];
static u8 t_game = TETRIS_OVER;
static u8 t_playing = FALSE;
static u8 t_currentshape = 0, t_status = 0;
static u16 t_x = TETRIS_DEFAULT_X, t_y = TETRIS_DEFAULT_X;

static void tetris_initial(void);
void tetris_pause_create(void);
void tetris_pause_destroy(void);
//static void tetris_redraw(void);
void tetris_go_left(void);
void tetris_go_right(void);
BOOL tetris_go_down(void);
void tetris_change_shape(u16 x, u16 y, u8 currentshape, u8 status);
static void tetris_update_status(void);
static s8 tetris_createnewshape(void);
static void tetris_hide(u16 x, u16 y, u8 currentshape, u8 status);
static void tetris_show(u16 x, u16 y, u8 currentshape2, u8 status, u8 color);

static comm_dlg_data_t tetris_quit_dlg = //popup dialog data
{
  ROOT_ID_TETRIS,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  COMM_DLG_X,                        COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_GAME_MSG_FOR_EXIT,
  0,
};

static comm_dlg_data_t tetris_lose_dlg = //popup dialog data
{
  ROOT_ID_TETRIS,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  COMM_DLG_X,                            COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
  IDS_GAME_MSG_YOU_LOSE,
  0,
};

static u8 t_shape[TETRIS_TOT_SHAPE][4][4][4] =
{
  {
    {
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
  },
  {
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 1 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 1 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 }
    },
  },
  {
    {
      { 0, 1, 0, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
  },
  {
    {
      { 1, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 1, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
  },
  {
    {
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
  },
  {
    {
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 0, 1, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
  },
  {
    {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, 0, 0 }
    },
  },
};

static u8 t_backgroud[TETRIS_ROW_NUM + 2][TETRIS_COL_NUM + 4];

RET_CODE tetris_root_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

RET_CODE tetris_board_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE tetris_se_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE tetris_level_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);


static RET_CODE on_tetris_save(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  return fw_tmr_destroy(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
}

static RET_CODE on_tetris_board_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (t_playing == TRUE)  //pause
  {
    if (TETRIS_PAUSE == t_game)
    {
      t_game = TETRIS_HAPPY;
      tetris_pause_destroy();
//      tetris_redraw();
//					fw_tmr_create(ROOT_ID_TETRIS, MSG_TETRIS_TMR,
//										t_timedelay[t_speed - 1], TRUE);
      fw_tmr_start(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
    }
    else
    {
      tetris_pause_create();
//					fw_tmr_destroy(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
      fw_tmr_stop(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
      t_game = TETRIS_PAUSE;
    }
  }

  return SUCCESS;
}

static RET_CODE on_tetris_board_focus_up(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (TETRIS_HAPPY == t_game)
  {
    tetris_change_shape(t_x, t_y, t_currentshape, t_status);
  }

  return SUCCESS;
}

static RET_CODE on_tetris_board_focus_down(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (TETRIS_HAPPY == t_game)
  {
    tetris_go_down();
  }

  return SUCCESS;
}

static RET_CODE on_tetris_board_focus_left(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (TETRIS_HAPPY == t_game)
  {
    tetris_go_left();
  }

  return SUCCESS;
}

static RET_CODE on_tetris_board_focus_right(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if (TETRIS_HAPPY == t_game)
  {
    tetris_go_right();
  }

  return SUCCESS;
}

static RET_CODE on_tetris_board_timer_expired(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if ((TETRIS_HAPPY == t_game))
  {
    tetris_go_down();
    //        fw_tmr_reset(ROOT_ID_TETRIS, MSG_TETRIS_TMR,
    //                     t_timedelay[t_speed - 1]);
  }

  return SUCCESS;
}

static RET_CODE on_tetris_board_exit(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  fw_tmr_stop(ROOT_ID_TETRIS, MSG_TETRIS_TMR);

  if(ui_comm_dlg_open(&tetris_quit_dlg) == DLG_RET_YES)
  {
    tetris_pause_destroy();    //Mark add for bug 639
    fw_tmr_destroy(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
    t_speed = 1;
    t_erasered = 0;
    t_mark = 0;
    tetris_update_status();
    game_over_common(ROOT_ID_TETRIS);
  }
  else
  {
    if (t_game != TETRIS_PAUSE)
    {
//			  fw_tmr_create(ROOT_ID_TETRIS, MSG_TETRIS_TMR, t_timedelay[t_speed - 1], TRUE);
      fw_tmr_start(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
    }
  }

  return SUCCESS;
}

static RET_CODE on_tetris_se_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  u8 ctrl_id;
  RET_CODE ret = SUCCESS;

  ctrl_id = (u8)ctrl_get_ctrl_id(p_ctrl);

  if (2 == ctrl_id)
  {
    tetris_initial();
  }
  else if (4 == ctrl_id)
  {
    ret = manage_close_menu(ROOT_ID_TETRIS, 0, 0);
  }

  return ret;
}

static RET_CODE on_tetris_se_exit(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  return manage_close_menu(ROOT_ID_TETRIS, 0, 0);
}

static RET_CODE on_tetris_level_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_item;

  p_item = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), 7);

  return ctrl_process_msg(p_item, msg, 0, 0);

}

static RET_CODE on_tetris_level_exit(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  return manage_close_menu(ROOT_ID_TETRIS, 0, 0);
}

RET_CODE open_game_tetris(u32 para1, u32 para2)
{
  control_t *p_mask, *p_cont, *p_chessboard, *p_info;
  control_t *p_ctrl[TETRIS_INFO_ITEM_COUNT];
  u16 i, j;

  u16 item_y;
  memset(p_ctrl, 0, sizeof(control_t *) * TETRIS_INFO_ITEM_COUNT);
  p_mask = fw_create_mainwin(ROOT_ID_TETRIS, 0, 0,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0,
                                  OBJ_ATTR_ACTIVE,
                                  0);
  MT_ASSERT(p_mask != NULL);

  ctrl_set_rstyle(p_mask, RSI_TETRIS_DESKTOP, RSI_TETRIS_DESKTOP,
                  RSI_TETRIS_DESKTOP);
  ctrl_set_proc(p_mask, tetris_root_proc);

  // create container
  p_cont =
    ctrl_create_ctrl(CTRL_CONT, GAME_CONT_ID, TETRIS_CONT_X, TETRIS_CONT_Y,
                     TETRIS_CONT_W, TETRIS_CONT_H, p_mask,
                     0);
  ctrl_set_rstyle(p_cont, RSI_TETRIS_CONT, RSI_TETRIS_CONT, RSI_TETRIS_CONT);

  MT_ASSERT(p_cont != NULL);

  //Snake
  p_chessboard = ctrl_create_ctrl(CTRL_MBOX, GAME_BOARD_ID, 0, 0,
                                  TETRIS_BOARD_W, TETRIS_BOARD_H, p_cont, 0);
  ctrl_set_keymap(p_chessboard, game_board_keymap);
  ctrl_set_proc(p_chessboard, tetris_board_proc);
  ctrl_set_rstyle(p_chessboard, RSI_TETRIS_BOARD, RSI_TETRIS_BOARD,
                  RSI_TETRIS_BOARD);
  ctrl_set_mrect(p_chessboard, TETRIS_BOARD_INT_X,
    TETRIS_BOARD_INT_Y, TETRIS_BOARD_INT_R, TETRIS_BOARD_INT_B);                  
  mbox_enable_string_mode(p_chessboard, FALSE);
  mbox_set_count(p_chessboard, TETRIS_TOT_NUM, TETRIS_COL_NUM, TETRIS_ROW_NUM);
  mbox_set_item_interval(p_chessboard, 0, 0);
  mbox_set_item_rstyle(p_chessboard, RSI_TETRIS_ITEM, RSI_TETRIS_ITEM,
                      RSI_TETRIS_ITEM);
  mbox_set_icon_offset(p_chessboard, 0, 0);
  mbox_set_icon_align_type(p_chessboard, STL_CENTER | STL_VCENTER);
  for (i = 0; i < TETRIS_COL_NUM * TETRIS_ROW_NUM; i++)
  {
    mbox_set_content_by_icon(p_chessboard, i, 0, 0);
  }
  
  //tetris info container
  p_info =
    ctrl_create_ctrl(CTRL_CONT, GAME_INFO_ID, TETRIS_INFO_X, TETRIS_INFO_Y,
                     TETRIS_INFO_W, TETRIS_INFO_H, p_cont,
                     0);
  ctrl_set_keymap(p_info, game_info_keymap);
  ctrl_set_rstyle(p_info, RSI_TETRIS_INFO, RSI_TETRIS_INFO, RSI_TETRIS_INFO);

  item_y = TETRIS_INFO_ITEM_Y;

  for (i = 0; i < TETRIS_INFO_ITEM_COUNT; i++)
  {
    switch (i)
    {
      case 0:
      case 5:
      case 7:
      case 9:
        p_ctrl[i] =
          ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1), TETRIS_INFO_ITEM_X, item_y,
                           TETRIS_INFO_ITEM_W,
                           TETRIS_INFO_ITEM_H, p_info,
                           0);
        text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], tetris[i]);
        break;
      case 1:
      case 3:
        p_ctrl[i] =
          ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1), TETRIS_INFO_ITEM_X, item_y,
                           TETRIS_INFO_ITEM_W,
                           TETRIS_INFO_ITEM_H, p_info,
                           0);
        ctrl_set_keymap(p_ctrl[i], game_se_keymap);
        ctrl_set_proc(p_ctrl[i], tetris_se_proc);
        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_GAME_ITEM_HL,
                        RSI_COMM_STATIC_GRAY);
        text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL,
                            FSI_COMM_CTRL_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], tetris[i]);
        ctrl_set_related_id(p_ctrl[i], 0, (u8)((i - 2 + 3) % 3 + 2), 0,
                            (u8)((i) % 3 + 2));
        break;
      case 2:
        p_ctrl[i] =
          ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1), TETRIS_INFO_ITEM_X, item_y,
                           TETRIS_INFO_ITEM_W,
                           TETRIS_INFO_ITEM_H, p_info,
                           0);
        ctrl_set_keymap(p_ctrl[i], game_level_keymap);
        ctrl_set_proc(p_ctrl[i], tetris_level_proc);
        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_GAME_SELECT_HL,
                        RSI_COMM_STATIC_GRAY);
        text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL,
                            FSI_COMM_CTRL_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], tetris[i]);
        ctrl_set_related_id(p_ctrl[i], 0, (u8)((i - 2 + 3) % 3 + 2), 0,
                            (u8)((i) % 3 + 2));
        break;
      case 4: //next shap view
        p_ctrl[i] =
          ctrl_create_ctrl(CTRL_MBOX, (u8)(i + 1), TETRIS_NEXT_SHAPE_X, item_y,
                           TETRIS_NEXT_SHAPE_W,
                           TETRIS_NEXT_SHAPE_H, p_info,
                           0);
        ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);
        ctrl_set_mrect(p_ctrl[i], 0, 0, TETRIS_NEXT_SHAPE_W,
                           TETRIS_NEXT_SHAPE_H);                        
        mbox_enable_string_mode(p_ctrl[i], FALSE);
        mbox_set_count(p_ctrl[i], TETRIS_NEXT_SHAPE_TOT, TETRIS_NEXT_SHAPE_COL,
                       TETRIS_NEXT_SHAPE_ROW);
        mbox_set_item_interval(p_ctrl[i], 0, 0);
        mbox_set_item_rstyle(p_ctrl[i], RSI_TETRIS_PREVIEW, RSI_TETRIS_PREVIEW,
                            RSI_TETRIS_PREVIEW);
        mbox_set_icon_offset(p_ctrl[i], 0, 0);
        mbox_set_icon_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
        break;
      case 6:
        p_ctrl[i] =
          ctrl_create_ctrl(CTRL_CBOX, (u8)(i + 1), TETRIS_INFO_ITEM_X, item_y,
                           TETRIS_INFO_ITEM_W,
                           TETRIS_INFO_ITEM_H, p_info,
                           0);
        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_HL,
                        RSI_COMM_STATIC_GRAY);
        cbox_enable_cycle_mode(p_ctrl[i], TRUE);
        cbox_set_work_mode(p_ctrl[i], CBOX_WORKMODE_STATIC);
        cbox_set_align_style(p_ctrl[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL,
                            FSI_COMM_CTRL_GRAY);
        cbox_static_set_count(p_ctrl[i], TETRIS_CLASS_NUM);
        cbox_static_set_content_type(p_ctrl[i], CBOX_ITEM_STRTYPE_DEC);

        for (j = 0; j < TETRIS_CLASS_NUM; j++)
        {
          cbox_static_set_content_by_dec(p_ctrl[i], j, (s32)(j + 1));
        }
        cbox_static_set_focus(p_ctrl[i], 0);
        break;
      case 8:
      case 10:
        p_ctrl[i] =
          ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1), TETRIS_INFO_ITEM_X, item_y,
                           TETRIS_INFO_ITEM_W,
                           TETRIS_INFO_ITEM_H, p_info,
                           0);
        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_HL,
                        RSI_COMM_STATIC_GRAY);
        text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL,
                            FSI_COMM_CTRL_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_DEC);
        text_set_content_by_dec(p_ctrl[i], 0);
        break;
      default:
        break;
    }

    if ((i == 0) || (i == 1) || (i == 2) || (i == 3))
    {
      item_y += (TETRIS_INFO_ITEM_H + TETRIS_INFO_ITEM_I);
    }
    else if (i == 4)
    {
      item_y += TETRIS_NEXT_SHAPE_H;
    }
    else
    {
      item_y += (TETRIS_INFO_ITEM_H);
    }
  }

  ctrl_default_proc(p_ctrl[1], MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  return SUCCESS;
}


static void tetris_initial(void)
{
  control_t *p_mbox, *p_info;
  u16 tetris_idx;
  u8 i, j;

  p_info = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_INFO_ID);
  p_mbox = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_BOARD_ID);
  t_mark = 0;
  t_erasered = 0;
  t_newshape[0] = (u8)RAND(TETRIS_TOT_SHAPE);
  t_newcolor = bmp_id[(u8)RAND(7)];
//	t_newshape[0] = 1 ;
  t_currentshape = (u8)tetris_createnewshape();
  t_x = TETRIS_DEFAULT_X;
  t_y = TETRIS_DEFAULT_Y;
  t_status = 0;
  /*try to destory pause dialog before start a new game.#1097*/
  if (t_game == TETRIS_PAUSE)
  {
    tetris_pause_destroy();
  }
  t_game = TETRIS_HAPPY;
  t_playing = TRUE;

  for (i = 0; i < (TETRIS_ROW_NUM + 2); i++)
  {
    for (j = 0; j < (TETRIS_COL_NUM + 4); j++)
    {
      if ((j < 2) || (j >= (TETRIS_COL_NUM + 2)) || (i >= (TETRIS_ROW_NUM)))
      {
        t_backgroud[i][j] = 1;
      }
      else
      {
        tetris_idx = i * TETRIS_COL_NUM + j - 2;
        mbox_set_content_by_icon(p_mbox, tetris_idx, TETRIS_BG_ICON,
                                 TETRIS_BG_ICON);
        t_backgroud[i][j] = 0;
      }
    }
  }

  p_info = ctrl_get_child_by_id(p_info, 7); //speed container
  t_speed = (u8)(cbox_static_get_focus(p_info) + 1);
  p_info = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_INFO_ID);
  p_info = ctrl_get_child_by_id(p_info, 2);
  ctrl_default_proc(p_info, MSG_LOSTFOCUS, 0, 0);
  ctrl_paint_ctrl(p_info, TRUE);
  ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);
//	ctrl_paint_ctrl(p_mbox, TRUE);

  tetris_show(t_x, t_y, t_currentshape, t_status, TRUE);
  tetris_update_status(); //Mark wu for bug #114
  fw_tmr_create(ROOT_ID_TETRIS, MSG_TETRIS_TMR, t_timedelay[t_speed - 1],
                TRUE);
}


static s8 tetris_createnewshape(void)
{
  control_t *p_info;
  u16 tetris_idx;
  u8 l, p;

  t_newshape[1] = t_newshape[0];
  t_newshape[0] = (u8)RAND(7);
  t_curncolor = t_newcolor;
  t_newcolor = bmp_id[(u8)RAND(7)];
//	t_newshape[0] = 1;

  p_info = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_INFO_ID);
  p_info = ctrl_get_child_by_id(p_info, 5);
  for (l = 0; l < 4; l++)
  {
    for (p = 0; p < 4; p++)
    {
      tetris_idx = p * TETRIS_NEXT_SHAPE_COL + l;
      if (t_shape[t_newshape[0]][0][p][l] == 1)
      {
        mbox_set_content_by_icon(p_info, tetris_idx, t_newcolor, t_newcolor);
      }
      else
      {
        mbox_set_content_by_icon(p_info, tetris_idx, 0, 0);
      }
      mbox_draw_item_ext(p_info, tetris_idx, TRUE);
    }
  }
  return (s8)t_newshape[1];
}


static void tetris_show(u16 x, u16 y, u8 currentshape, u8 status, u8 color)
{
  control_t *p_mbox;
  u16 tetris_idx;
  u8 l, p;

  p_mbox = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_BOARD_ID);
  for (l = 0; l < 4; l++)
  {
    for (p = 0; p < 4; p++)
    {
      if (((l + x) < 2) || ((x + l) >= (TETRIS_COL_NUM + 2)) ||
          ((p + y) >= (TETRIS_ROW_NUM)))
      {
        continue;
      }

      if (t_shape[currentshape][status][p][l] == 1)
      {
        tetris_idx = (u16)((p + y) * TETRIS_COL_NUM + (l + x - 2));
        if (color)
        {
          mbox_set_content_by_icon(p_mbox, tetris_idx, t_curncolor,
                                   t_curncolor);
        }
        else
        {
          mbox_set_content_by_icon(p_mbox, tetris_idx, TETRIS_INACTIVE_ICON,
                                   TETRIS_INACTIVE_ICON);
        }
        mbox_draw_item_ext(p_mbox, tetris_idx, TRUE);
      }
    }
  }
}


static void tetris_hide(u16 x, u16 y, u8 currentshape, u8 status)
{
  control_t *p_mbox;
  u16 tetris_idx;
  u8 l, p;

  p_mbox = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_BOARD_ID);
  for (l = 0; l < 4; l++)
  {
    for (p = 0; p < 4; p++)
    {
      if (((x + l) >= (TETRIS_COL_NUM + 2)) ||
          ((p + y) >= (TETRIS_ROW_NUM)))
      {
        continue;
      }

      if (t_shape[currentshape][status][p][l] == 1)
      {
        tetris_idx = (u16)((p + y) * TETRIS_COL_NUM + (l + x - 2));
        mbox_set_content_by_icon(p_mbox, tetris_idx, TETRIS_BG_ICON,
                                 TETRIS_BG_ICON);
        mbox_draw_item_ext(p_mbox, tetris_idx, TRUE);
      }
    }
  }
}


u32 tetris_possible(u16 x, u16 y, u8 currentshape, u8 status)
{
  u8 i, j;

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      if (t_shape[currentshape][status][i][j] + t_backgroud[y + i][x + j] == 2)
      {
        return 0;
      }
    }
  }
  return 1;
}


void tetris_go_left(void)
{
  if (tetris_possible((u16)(t_x - 1), t_y, t_currentshape, t_status))
  {
    tetris_hide(t_x, t_y, t_currentshape, t_status);
    t_x--;
  }

  tetris_show(t_x, t_y, t_currentshape, t_status, TRUE);
}


void tetris_go_right(void)
{
  if (tetris_possible((u16)(t_x + 1), t_y, t_currentshape, t_status))
  {
    tetris_hide(t_x, t_y, t_currentshape, t_status);
    t_x++;
  }

  tetris_show(t_x, t_y, t_currentshape, t_status, TRUE);
}


BOOL tetris_go_down(void)
{
  control_t *p_mbox;
  u16 tetris_idx;
  u8 all_are_one, temp = 0;
  s8 i, j, r, l, rr, ll;

  p_mbox = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_BOARD_ID);

  if (tetris_possible(t_x, (u16)(t_y + 1), t_currentshape, t_status))
  {
    tetris_hide(t_x, t_y, t_currentshape, t_status);
    t_y++;
    tetris_show(t_x, t_y, t_currentshape, t_status, TRUE);
    return TRUE;
  }

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      t_backgroud[i + t_y][j + t_x] += t_shape[t_currentshape][t_status][i][j];
    }
  }

  for (r = 0; r < TETRIS_ROW_NUM; r++)
  {
    all_are_one = 1;
    for (l = 0; l < TETRIS_COL_NUM + 4; l++)
    {
      if (t_backgroud[r][l] == 0)
      {
        all_are_one = 0;
        break;
      }
    }

    if (all_are_one == 1)
    {
      for (rr = r; rr > 0; rr--)
      {
        for (ll = 0; ll < (TETRIS_COL_NUM + 4); ll++)
        {
          t_backgroud[rr][ll] = t_backgroud[rr - 1][ll];
        }
      }

      t_erasered++;
      if ((!(t_erasered % 30)) && (t_speed < 10))
      {
        t_speed++;
        fw_tmr_destroy(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
        fw_tmr_create(ROOT_ID_TETRIS, MSG_TETRIS_TMR, t_timedelay[t_speed - 1], TRUE);
      }
      temp++;
    }
  }

  tetris_show(t_x, t_y, t_currentshape, t_status, FALSE); //sunk the button
  t_currentshape = (u8)tetris_createnewshape();
  t_x = TETRIS_DEFAULT_X;
  t_y = TETRIS_DEFAULT_Y;
  t_status = 0;
  t_mark += (t_speed) * (10 + temp - 1) * temp; //to caculate total mark

  if (!tetris_possible(t_x, t_y, t_currentshape, t_status))
  {
    if (TETRIS_OVER != t_game)
    {
      t_game = TETRIS_OVER;
      t_playing = FALSE;
      fw_tmr_destroy(ROOT_ID_TETRIS, MSG_TETRIS_TMR);
      
      
      tetris_show(t_x, t_y, t_currentshape, t_status, TRUE);

      if(ui_comm_dlg_open(&tetris_lose_dlg) == DLG_RET_YES)
      {
        game_over_common(ROOT_ID_TETRIS);
        t_mark = 0, t_erasered = 0;      /*fix bug founder_mingxin 19910*/
      }
      tetris_update_status();
      return FALSE;
    }
  }

  tetris_update_status();

  if (temp > 0)
  {
    for (i = 0; i < TETRIS_ROW_NUM; i++)
    {
      for (j = 0; j < TETRIS_COL_NUM; j++)
      {
        tetris_idx = (u8)i * TETRIS_COL_NUM + (u8)j;
        if (t_backgroud[i][j + 2] == 1)
        {
          mbox_set_content_by_icon(p_mbox, tetris_idx, TETRIS_INACTIVE_ICON,
                                   TETRIS_INACTIVE_ICON);
        }
        else
        {
          mbox_set_content_by_icon(p_mbox, tetris_idx, 0, 0);
        }
        mbox_draw_item_ext(p_mbox, tetris_idx, TRUE);
      }
    }
  }

  tetris_show(t_x, t_y, t_currentshape, t_status, TRUE);

  return FALSE;
}


static void tetris_update_status(void)
{                                                    //speed, Totalmark, Erasered
  control_t *p_info;
  p_info = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_INFO_ID);
  p_info = ctrl_get_child_by_id(p_info, 7);          //speed container
  cbox_static_set_focus(p_info, (u8)(t_speed - 1));
  ctrl_paint_ctrl(p_info, TRUE);
  p_info = ctrl_get_child_by_id(p_info->p_parent, 9);  //total mark container
  text_set_content_by_dec(p_info, (s32)t_erasered);
  ctrl_paint_ctrl(p_info, TRUE);
  p_info = ctrl_get_child_by_id(p_info->p_parent, 11); //erasered textfield
  text_set_content_by_dec(p_info, (s32)t_mark);
  ctrl_paint_ctrl(p_info, TRUE);
}


void tetris_change_shape(u16 x, u16 y, u8 currentshape, u8 status)
{
  if (tetris_possible(x, y, currentshape, (u8)((status + 1) % 4)))
  {
    tetris_hide(x, y, currentshape, status);
    status = ((status) + 1) % 4;
    t_status = status;
  }
  tetris_show(x, y, currentshape, status, t_curncolor);
}

#if 0
static void tetris_redraw(void)
{
  control_t *p_mbox;
  u16 tetris_idx;
  u8 i, j;

  p_mbox = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_BOARD_ID);

  for (i = 0; i < TETRIS_ROW_NUM; i++)
  {
    for (j = 0; j < TETRIS_COL_NUM; j++)
    {
      tetris_idx = i * TETRIS_COL_NUM + j;
      if (t_backgroud[i][j + 2] == 1)
      {
        mbox_set_content_by_icon(p_mbox, tetris_idx, TETRIS_INACTIVE_ICON,
                                 TETRIS_INACTIVE_ICON);
      }
      else
      {
        mbox_set_content_by_icon(p_mbox, tetris_idx, TETRIS_BG_ICON,
                                 TETRIS_BG_ICON);
      }
      mbox_draw_item_ext(p_mbox, tetris_idx, TRUE);
    }
  }

  tetris_show(t_x, t_y, t_currentshape, t_status, TRUE);
}
#endif

void tetris_pause_create(void)
{
  control_t *p_mbox, *p_pause;

  p_mbox = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_BOARD_ID);

  p_pause =
    ctrl_create_ctrl(CTRL_TEXT, TETRIS_PAUSE_ID, TETRIS_PAUSE_X, TETRIS_PAUSE_Y,
                     TETRIS_PAUSE_W, TETRIS_PAUSE_H, p_mbox,
                     0);
  ctrl_set_rstyle(p_pause, RSI_TETRIS_PAUSE, RSI_TETRIS_PAUSE, RSI_TETRIS_PAUSE);
  text_set_align_type(p_pause, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_pause, TEXT_STRTYPE_STRID);
  text_set_font_style(p_pause, FSI_TETRIS_PAUSE, FSI_TETRIS_PAUSE,
                      FSI_TETRIS_PAUSE);
  text_set_content_by_strid(p_pause, IDS_PAUSE);

  ctrl_paint_ctrl(p_pause, TRUE);
}


void tetris_pause_destroy(void)
{
  control_t *p_mbox, *p_pause;

  p_mbox = ui_game_get_ctrl(ROOT_ID_TETRIS, GAME_BOARD_ID);
  p_pause = ctrl_get_child_by_id(p_mbox, TETRIS_PAUSE_ID);

  if (p_pause != NULL)
  {
    ctrl_destroy_ctrl(p_pause);
    ctrl_paint_ctrl(p_mbox, TRUE);
  }
}

BEGIN_MSGPROC(tetris_root_proc, cont_class_proc)
  ON_COMMAND(MSG_SAVE, on_tetris_save)
END_MSGPROC(tetris_root_proc, cont_class_proc)

BEGIN_MSGPROC(tetris_board_proc, mbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_tetris_board_select)
  ON_COMMAND(MSG_FOCUS_UP, on_tetris_board_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_tetris_board_focus_down)
  ON_COMMAND(MSG_FOCUS_LEFT, on_tetris_board_focus_left)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_tetris_board_focus_right)
  ON_COMMAND(MSG_TETRIS_TMR, on_tetris_board_timer_expired)
  ON_COMMAND(MSG_EXIT, on_tetris_board_exit)
END_MSGPROC(tetris_board_proc, mbox_class_proc)

BEGIN_MSGPROC(tetris_se_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_tetris_se_select)
  ON_COMMAND(MSG_EXIT_ALL, on_tetris_se_exit)
  ON_COMMAND(MSG_EXIT, on_tetris_se_exit)
END_MSGPROC(tetris_se_proc, text_class_proc)

BEGIN_MSGPROC(tetris_level_proc, text_class_proc)
  ON_COMMAND(MSG_INCREASE, on_tetris_level_change)
  ON_COMMAND(MSG_DECREASE, on_tetris_level_change)
  ON_COMMAND(MSG_EXIT_ALL, on_tetris_level_exit)
  ON_COMMAND(MSG_EXIT, on_tetris_se_exit)
END_MSGPROC(tetris_level_proc, text_class_proc)

