/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_HDD_INFO_H__
#define __UI_HDD_INFO_H__

/* coordinate */
#define HDD_INFO_CONT_X        ((COMM_BG_W - HDD_INFO_CONT_W) / 2)
#define HDD_INFO_CONT_H        420

#define HDD_INFO_ITEM_CNT      6
#define HDD_INFO_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define HDD_INFO_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define HDD_INFO_ITEM_LW       COMM_ITEM_LW
#define HDD_INFO_ITEM_RW       (COMM_ITEM_MAX_WIDTH - HDD_INFO_ITEM_LW)
#define HDD_INFO_ITEM_H        COMM_ITEM_H
#define HDD_INFO_ITEM_V_GAP    16

#define HDD_INFO_HELP_X        COMM_ITEM_OX_IN_ROOT
#define HDD_INFO_HELP_W        (HDD_INFO_ITEM_LW + HDD_INFO_ITEM_RW)
#define HDD_INFO_HELP_H        COMM_ITEM_H

/* rect style */
#define RSI_HDD_INFO_FRM       RSI_WINDOW_1

/* others */

RET_CODE open_hdd_info(u32 para1, u32 para2);

#endif
