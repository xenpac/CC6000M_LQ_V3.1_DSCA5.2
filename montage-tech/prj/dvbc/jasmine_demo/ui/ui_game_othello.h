/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_GAME_OTHELLO_H_
#define _UI_GAME_OTHELLO_H_

/* coordinate */
#define OTHELLO_CONT_X         ((SCREEN_WIDTH - OTHELLO_CONT_W) / 2)
#define OTHELLO_CONT_Y         ((SCREEN_HEIGHT - OTHELLO_CONT_H) / 2)
#define OTHELLO_CONT_W         720
#define OTHELLO_CONT_H         520

#define OTHELLO_BOARD_X        0
#define OTHELLO_BOARD_Y        0
#define OTHELLO_BOARD_W        568
#define OTHELLO_BOARD_H        (OTHELLO_CONT_H - 2 * OTHELLO_BOARD_Y)

#define OTHELLO_BOARD_IL      4
#define OTHELLO_BOARD_IT      4
#define OTHELLO_BOARD_IR      (OTHELLO_BOARD_W-OTHELLO_BOARD_IL)
#define OTHELLO_BOARD_IB      (OTHELLO_BOARD_H-OTHELLO_BOARD_IT)

#define OTHELLO_ITEM_GAP       5

#define OTHELLO_INFO_X         (OTHELLO_BOARD_W + OTHELLO_ITEM_GAP)
#define OTHELLO_INFO_Y         OTHELLO_BOARD_Y
#define OTHELLO_INFO_W         (OTHELLO_CONT_W - OTHELLO_BOARD_W - 2 * \
                                OTHELLO_BOARD_X - OTHELLO_ITEM_GAP)
#define OTHELLO_INFO_H         OTHELLO_BOARD_H

#define OTHELLO_INFO_ITEM_X    5
#define OTHELLO_INFO_ITEM_Y    5
#define OTHELLO_INFO_ITEM_W    (OTHELLO_INFO_W - 2 * OTHELLO_INFO_ITEM_X)
#define OTHELLO_INFO_ITEM_H    36
#define OTHELLO_INFO_ITEM_I    10

#define OTHELLO_DLG_X          ((SCREEN_WIDTH - OTHELLO_DLG_W) / 2)
#define OTHELLO_DLG_W          300
#define OTHELLO_DLG_H          250

/* rect style */
#define RSI_OTHELLO_DESKTOP    RSI_TRANSPARENT
#define RSI_OTHELLO_CONT       RSI_IGNORE
#define RSI_OTHELLO_BOARD      RSI_WINDOW_GAME
#define RSI_OTHELLO_ITEM_HL    RSI_CHESS_HL
#define RSI_OTHELLO_ITEM_SH    RSI_WHITE_FRM
#define RSI_OTHELLO_INFO        RSI_WINDOW_GAME
#define RSI_OTHELLO_INFO_ITEM_SH   RSI_ITEM_1_SH
#define RSI_OTHELLO_INFO_ITEM_HL  RSI_ITEM_1_HL

/* font style */

/* others */
#define OTHELLO_COL_NUM            8
#define OTHELLO_ROW_NUM            8
#define OTHELLO_TOT_NUM            (OTHELLO_COL_NUM * OTHELLO_ROW_NUM)

#define OTHELLO_INFO_ITEM_COUNT    10

#define OTHELLO_BLACK              1
#define OTHELLO_WHITE              0

#define OTHELLO_BLACK_BMP          IM_BLACK
#define OTHELLO_WHITE_BMP          IM_WHITE

#define OTHELLO_GAME_LEVEL_PRM     0
#define OTHELLO_GAME_LEVEL_JUR     1
#define OTHELLO_GAME_LEVEL_SER     2

#define OTHELLO_START              1
#define OTHELLO_HAPPY              2
#define OTHELLO_OVER               3

RET_CODE open_game_othello(u32 para1, u32 para2);
#endif
