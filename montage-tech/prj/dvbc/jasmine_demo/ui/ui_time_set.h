/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_TIME_SET_H__
#define __UI_TIME_SET_H__

/* coordinate */
#define TIME_SET_CONT_X        ((COMM_BG_W - TIME_SET_CONT_W) / 2)
#define TIME_SET_CONT_H        420

#define TIME_SET_ITEM_CNT      5
#define TIME_SET_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define TIME_SET_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define TIME_SET_ITEM_LW       COMM_ITEM_LW
#define TIME_SET_ITEM_RW       (COMM_ITEM_MAX_WIDTH - TIME_SET_ITEM_LW)
#define TIME_SET_ITEM_H        COMM_ITEM_H
#define TIME_SET_ITEM_V_GAP    10//20

/* rect style */
#define RSI_TIME_SET_FRM       RSI_WINDOW_1
/* font style */

/* others */
#define TIME_SET_GMT_OFFSET_NUM 48
#define TIME_SET_GMT_OFFSET_CITY_LEN 200

RET_CODE open_time_set(u32 para1, u32 para2);

#endif

