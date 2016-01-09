/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_SLEEP_HOTKEY_H__
#define __UI_SLEEP_HOTKEY_H__

/* coordinate */
#define SLEEP_HOTKEY_CONT_X         (SCREEN_WIDTH - SLEEP_HOTKEY_CONT_W - 80)
#define SLEEP_HOTKEY_CONT_Y         50
#define SLEEP_HOTKEY_CONT_W         150
#define SLEEP_HOTKEY_CONT_H         COMM_ITEM_H

#define SLEEP_HOTKEY_ITEM_X        0
#define SLEEP_HOTKEY_ITEM_Y        0

#define SLEEP_HOTKEY_ITEM_W        SLEEP_HOTKEY_CONT_W
#define SLEEP_HOTKEY_ITEM_H        SLEEP_HOTKEY_CONT_H

/* rect style */

/* font style */

/* others */

#define SLEEP_CNT 6

void sleep_hotkey_set_content(u8 focus);

RET_CODE open_sleep_hotkey(u32 para1, u32 para2);

#endif

