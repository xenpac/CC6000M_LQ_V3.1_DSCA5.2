/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_TVSYS_SET_H__
#define __UI_TVSYS_SET_H__

/* coordinate */
#define TVSYS_SET_ITEM_CNT     9// 8
#define TVSYS_SET_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define TVSYS_SET_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define TVSYS_SET_ITEM_LW       COMM_ITEM_LW
#define TVSYS_SET_ITEM_RW       (COMM_ITEM_MAX_WIDTH - TVSYS_SET_ITEM_LW)
#define TVSYS_SET_ITEM_H        COMM_ITEM_H
#define TVSYS_SET_ITEM_V_GAP    8//20

/* rect style */
#define RSI_TVSYS_SET_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_tvsys_set(u32 para1, u32 para2);

#endif


