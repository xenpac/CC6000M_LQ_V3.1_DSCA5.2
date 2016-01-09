/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_MAC_SETTING_H__
#define __UI_MAC_SETTING_H__

/* coordinate */
#define MAC_SETTING_ITEM_CNT		1
#define MAC_SETTING_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define MAC_SETTING_ITEM_Y			COMM_ITEM_OY_IN_ROOT
#define MAC_SETTING_ITEM_LW		200
#define MAC_SETTING_ITEM_RW		(COMM_ITEM_MAX_WIDTH - MAC_SETTING_ITEM_LW)
#define MAC_SETTING_ITEM_H			COMM_ITEM_H
#define MAC_SETTING_ITEM_V_GAP		16

/* rect style */
#define MAC_SETTING_OTHERS_FRM		RSI_WINDOW_1
/* font style */

#define MAX_DEVICE_NAME_LENGTH 64

RET_CODE open_mac_setting (u32 para1, u32 para2);

#endif

