/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_INFO_H__
#define __UI_INFO_H__

/* coordinate */
#define INFO_CONT_X        ((COMM_BG_W - INFO_CONT_W) / 2)
#define INFO_CONT_Y        COMM_BG_Y
#define INFO_CONT_W        COMM_BG_W
#define INFO_CONT_H        420

#define INFO_LIST_X        COMM_ITEM_OX_IN_ROOT
#define INFO_LIST_Y        COMM_ITEM_OY_IN_ROOT
#define INFO_LIST_W       COMM_ITEM_MAX_WIDTH 
#define INFO_LIST_H        360

#define INFO_LIST_MID_L     4
#define INFO_LIST_MID_T     4
#define INFO_LIST_MID_W   (INFO_LIST_W - 2 * INFO_LIST_MID_L)
#define INFO_LIST_MID_H    (360 - INFO_LIST_ITEM_VGAP)

#define INFO_LIST_ITEM_H        COMM_ITEM_H
#define INFO_LIST_ITEM_VGAP    4

/* rect style */
#define RSI_INFO_FRM       RSI_WINDOW_1
#define RSI_INFO_LIST      RSI_PBACK
/* font style */

/* others */
#ifdef CAS_CONFIG_ABV
#define INFO_LIST_CNT      5
#else
#define INFO_LIST_CNT      6
#endif
#define INFO_LIST_FIELD   2

RET_CODE open_info(u32 para1, u32 para2);
extern u8* ads_get_ad_version(void);
#endif


