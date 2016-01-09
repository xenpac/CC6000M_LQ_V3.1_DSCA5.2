/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_SLEEP_TIMER_H__
#define __UI_SLEEP_TIMER_H__

/* coordinate */
#define SLEEP_TIMER_CONT_X        ((COMM_BG_W - SLEEP_TIMER_CONT_W) / 2)
#define SLEEP_TIMER_CONT_H        420

#define SLEEP_TIMER_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define SLEEP_TIMER_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define SLEEP_TIMER_ITEM_LW       COMM_ITEM_LW
#define SLEEP_TIMER_ITEM_RW       (COMM_ITEM_MAX_WIDTH - SLEEP_TIMER_ITEM_LW)
#define SLEEP_TIMER_ITEM_H        COMM_ITEM_H
#define SLEEP_TIMER_ITEM_V_GAP    2

/* rect style */
#define RSI_SLEEP_TIMER_FRM         RSI_WINDOW_1

/* font style */

/* others */

RET_CODE open_sleep_timer(u32 para1, u32 para2);

#endif

