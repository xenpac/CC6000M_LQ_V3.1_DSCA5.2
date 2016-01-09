/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef ___GAME_OTHELLO_H__
#define ___GAME_OTHELLO_H__

#include "ui_common.h"

#define BOARD_COLS 8
#define BOARD_ROWS 8
typedef struct
{
	u8 board[BOARD_ROWS+2][BOARD_COLS+2];
}board_type;

typedef u16 move_key_type; 

typedef struct
{
	board_type    board;
    u16        movepos;
	s16         value;
}tree_node_type;

typedef void (*othello_callback)(u8 type, s32 wparam, u32 lparam);

#define   TRANCHESS				0
#define   COMPUTER_NO_STEP		1
#define   USER_NO_STEP				2

#define   COMPUTER_WIN		0
#define   USER_WIN			1
#define   DOGFALL			2

#define   CHESS_NONE     0x00
#define   CHESS_BLACK    0x01
#define   CHESS_WHITE    0x02
#define   CHESS_BORDER   0xFF

#define   BD_PROTECTED   0x80
#define   FD_PROTECTED   0x40
#define   H_PROTECTED    0x20
#define   V_PROTECTED    0x10

#define   THITHER_COLOR(color)  ((~color)&0x03)


#define   INITIAL_VALUE  (32767)

#define   STEP_MONMENT1  10
#define   STEP_MONMENT2  48

//游戏难度等级
#define GMLEVEL_EASY        0
#define GMLEVEL_MID         1
#define GMLEVEL_ADV         2
extern  u8 g_iGameLevel; //游戏难度等级

//#define   USER_PASS     1
//#define   COMPUTER_PASS 2
#define   GAME_OVER     4

#define  WM_TRANCHESS  (WM_USER+10)
/*可一次吃掉的最多的子的个数*/
#define MAX_AFFECTED_PIECES 19

extern u8   computer_side;
extern u8   cur_step;
extern u16  step_array[64];

extern s16 calc_board_status(board_type *board_ptr, u8 obcolor);
extern void init_board(board_type *board_ptr, BOOL color);//true: player_color = black, false: player_color = white
extern BOOL computer_play(board_type *board_ptr, othello_callback callback);
extern  u8 do_move_chess(board_type *board_ptr, u16 movepos, u8 obcolor, othello_callback callback);
extern void get_chess_score(board_type *board_ptr, u16 *iWscore, u16 *iBscore);
extern void game_over(board_type *board_ptr, othello_callback callback);
#endif

