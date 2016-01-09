/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_GAME_TETRIS_H_
#define _UI_GAME_TETRIS_H_

/* coordinate */
#define TETRIS_CONT_X       ((SCREEN_WIDTH-TETRIS_CONT_W)/2)
#define TETRIS_CONT_Y       ((SCREEN_HEIGHT - TETRIS_CONT_H) / 2)
#define TETRIS_CONT_W       500
#define TETRIS_CONT_H       590//608

#define TETRIS_BOARD_X       0
#define TETRIS_BOARD_Y       0
#define TETRIS_BOARD_W       308
#define TETRIS_BOARD_H       (TETRIS_CONT_H - 2 * TETRIS_BOARD_Y)

#define TETRIS_BOARD_INT_X       4
#define TETRIS_BOARD_INT_Y       4
#define TETRIS_BOARD_INT_R       304
#define TETRIS_BOARD_INT_B       (TETRIS_BOARD_H - TETRIS_BOARD_INT_Y)

#define TETRIS_ITEM_GAP       5

#define TETRIS_INFO_X       (TETRIS_BOARD_W + TETRIS_ITEM_GAP)
#define TETRIS_INFO_Y       TETRIS_BOARD_Y
#define TETRIS_INFO_W       (TETRIS_CONT_W - TETRIS_BOARD_W - 2 * TETRIS_BOARD_X - TETRIS_ITEM_GAP)
#define TETRIS_INFO_H       TETRIS_BOARD_H

#define TETRIS_INFO_ITEM_X       5
#define TETRIS_INFO_ITEM_Y       20
#define TETRIS_INFO_ITEM_W       (TETRIS_INFO_W - 2 * TETRIS_INFO_ITEM_X)
#define TETRIS_INFO_ITEM_H       36
#define TETRIS_INFO_ITEM_I       15

#define TETRIS_DLG_X       ((SCREEN_WIDTH -TETRIS_DLG_W)/2)
#define TETRIS_DLG_W       300
#define TETRIS_DLG_H       250

#define 	TETRIS_NEXT_SHAPE_X       ((TETRIS_INFO_W - TETRIS_NEXT_SHAPE_W)/2)
#define 	TETRIS_NEXT_SHAPE_W       120
#define 	TETRIS_NEXT_SHAPE_H       TETRIS_NEXT_SHAPE_W

#define TETRIS_PAUSE_X       ((TETRIS_BOARD_W - TETRIS_PAUSE_W)/2)
#define TETRIS_PAUSE_Y       ((TETRIS_BOARD_H - TETRIS_PAUSE_H)/2)
#define TETRIS_PAUSE_W       100
#define TETRIS_PAUSE_H       36

/* rect style */
#define RSI_TETRIS_CONT        RSI_IGNORE
#define RSI_TETRIS_DESKTOP     RSI_TRANSPARENT
#define RSI_TETRIS_BOARD       RSI_WINDOW_GAME
#define RSI_TETRIS_ITEM        RSI_PBACK
#define RSI_TETRIS_INFO        RSI_WINDOW_GAME
#define RSI_TETRIS_PREVIEW     RSI_PBACK
#define RSI_TETRIS_PAUSE       RSI_ITEM_2_SH
/* font style */
#define FSI_TETRIS_PAUSE       FSI_WHITE
/* others */
#define TETRIS_INFO_ITEM_COUNT 11
#define TETRIS_CLASS_NUM       10

#define TETRIS_IDLE      0
#define TETRIS_START     1
#define TETRIS_PAUSE     2
#define TETRIS_HAPPY     3
#define TETRIS_OVER      4

#define TETRIS_COL_NUM   10
#define TETRIS_ROW_NUM   20
#define TETRIS_TOT_NUM   (TETRIS_COL_NUM * TETRIS_ROW_NUM)
#define TETRIS_MAX       TETRIS_TOT_NUM

#define TETRIS_NEXT_SHAPE_COL       4
#define TETRIS_NEXT_SHAPE_ROW       4
#define TETRIS_NEXT_SHAPE_TOT       (TETRIS_NEXT_SHAPE_COL * TETRIS_NEXT_SHAPE_ROW)

#define TETRIS_LEFT     3
#define TETRIS_RIGHT    4
#define TETRIS_UP       1
#define TETRIS_DOWN     2

#define TETRIS_DEFAULT_X      5
#define TETRIS_DEFAULT_Y      0

#define TETRIS_TOT_SHAPE      7

#define TETRIS_ACTIVE_ICON    IM_GAME_TETRIS_BLOCK_01
#define TETRIS_INACTIVE_ICON  IM_GAME_TETRIS_BLOCK_08
#define TETRIS_WALL_ICON      IM_GAME_TETRIS_BLOCK_02
#define TETRIS_BG_ICON        0

#define TETRIS_PAUSE_ID       1

RET_CODE open_game_tetris (u32 para1, u32 para2);
#endif

