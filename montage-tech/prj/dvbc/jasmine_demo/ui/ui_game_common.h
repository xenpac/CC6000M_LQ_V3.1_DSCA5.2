/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_GAME_COMMON_H_
#define _UI_GAME_COMMON_H_
/*!
 \file ui_game_common.h

   This file defined some common interfaces used in game modules.

   Development policy:
   If there is some other common interface used in game modules, you can add it here for saving code size.
 */

#define RAND(x)    ((u16)mtos_ticks_get()%(x)) 

/*!
   Common control id for game menu
 */
#define GAME_CONT_ID     1
#define GAME_BOARD_ID    1
#define GAME_INFO_ID     2


/*!
   Function:
   Get the pointer of a specified child control
   Parameter:
    u8 root_id		[in]:	root container id
    u8 child_id		[in]:	the specified child control id
   Return:
   the pointer of a specified child control
 */
control_t *ui_game_get_ctrl(u8 root_id, u8 child_id);

/*!
   keymap for game level control
 */
u16 game_level_keymap(u16 key);


/*!
   keymap for start/exit game control
 */
u16 game_se_keymap(u16 key);


/*!
   keymap for game board control
 */
u16 game_board_keymap(u16 key);


/*!
   keymap for game information container
 */
u16 game_info_keymap(u16 key);


/*!
   Function:
   a common function when game over, it changes the focus
   from game board control to game information container when game over.
   Parameter:
    u8 root_id		[in]:	specified which game is over
   Return:
   NULL
 */
void game_over_common(u8 root_id);

#endif

