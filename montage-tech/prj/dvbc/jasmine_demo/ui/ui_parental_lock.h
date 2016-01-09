/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PARENTAL_LOCK_H__
#define __UI_PARENTAL_LOCK_H__

/* coordinate */
#define PARENTAL_LOCK_CONT_X        ((COMM_BG_W - PARENTAL_LOCK_CONT_W) / 2)
#define PARENTAL_LOCK_CONT_H        420

#define PARENTAL_LOCK_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define PARENTAL_LOCK_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define PARENTAL_LOCK_ITEM_LW       COMM_ITEM_LW
#define PARENTAL_LOCK_ITEM_RW       (COMM_ITEM_MAX_WIDTH - PARENTAL_LOCK_ITEM_LW)
#define PARENTAL_LOCK_ITEM_H        COMM_ITEM_H
#define PARENTAL_LOCK_ITEM_V_GAP    10 //20

/* rect style */
#define RSI_PARENTAL_LOCK_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE preopen_parental_lock(u32 para1, u32 para2);

RET_CODE open_parental_lock(u32 para1, u32 para2);

#endif


