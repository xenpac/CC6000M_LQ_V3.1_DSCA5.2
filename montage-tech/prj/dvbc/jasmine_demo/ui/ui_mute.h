/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_MUTE_H__
#define __UI_MUTE_H__

/* coordinate */
#define MUTE_CONT_FULL_X			(SCREEN_WIDTH - MUTE_ICON_FULL_W - 50 - SCREEN_POS_X)
#define MUTE_CONT_FULL_Y			(60 + SCREEN_POS_Y)//30//78
#define MUTE_CONT_FULL_W			MUTE_ICON_FULL_W
#define MUTE_CONT_FULL_H			MUTE_ICON_FULL_H

#define MUTE_CONT_IN_MUSIC_X          (SCREEN_POS_X + PREV_COMMON_X + PREV_COMMON_W - MUTE_CONT_PREV_W - 10)
#define MUTE_CONT_IN_MUSIC_Y          (PREV_COMMON_Y + 10 + SCREEN_POS_Y)

#define MUTE_ICON_FULL_X			0
#define MUTE_ICON_FULL_Y			0
#define MUTE_ICON_FULL_W			56
#define MUTE_ICON_FULL_H			56

#define MUTE_CONT_PREV_W			MUTE_ICON_PREV_W
#define MUTE_CONT_PREV_H			MUTE_ICON_PREV_H


#define MUTE_ICON_PREV_X			0
#define MUTE_ICON_PREV_Y			0
#define MUTE_ICON_PREV_W			36
#define MUTE_ICON_PREV_H			36

#define MUTE_CONT_IN_MENU_X           (SCREEN_WIDTH - MUTE_CONT_PREV_W - 10 - SCREEN_POS_X)
#define MUTE_CONT_IN_MENU_Y           (10 + SCREEN_POS_Y)

RET_CODE open_mute (u32 para1, u32 para2);

BOOL ui_is_mute (void);

void ui_set_mute (BOOL is_mute);

void close_mute(void);

#endif


