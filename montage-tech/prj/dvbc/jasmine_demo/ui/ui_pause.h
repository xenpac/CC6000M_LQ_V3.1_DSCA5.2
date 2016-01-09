/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_PAUSE_H__
#define __UI_PAUSE_H__

#define PAUSE_CONT_FULL_X			(SCREEN_WIDTH - PAUSE_ICON_FULL_W - 250)
#define PAUSE_CONT_FULL_Y			30
#define PAUSE_CONT_FULL_W			PAUSE_ICON_FULL_W
#define PAUSE_CONT_FULL_H			PAUSE_ICON_FULL_H

#define PAUSE_ICON_FULL_X			0
#define PAUSE_ICON_FULL_Y			0
#define PAUSE_ICON_FULL_W			56
#define PAUSE_ICON_FULL_H			56

#define PAUSE_CONT_PREV_W			PAUSE_ICON_PREV_W
#define PAUSE_CONT_PREV_H			PAUSE_ICON_PREV_H

#define PAUSE_ICON_PREV_X			0
#define PAUSE_ICON_PREV_Y			0
#define PAUSE_ICON_PREV_W			36
#define PAUSE_ICON_PREV_H			36

RET_CODE open_pause (u32 para1, u32 para2);

BOOL ui_is_pause (void);

void ui_set_pause (BOOL is_pause);

void close_pause(void);

#endif

