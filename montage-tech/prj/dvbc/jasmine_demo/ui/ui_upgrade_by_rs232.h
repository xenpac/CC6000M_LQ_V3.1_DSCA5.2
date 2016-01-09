/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_UPGRADE_BY_RS232_H_
#define _UI_UPGRADE_BY_RS232_H_

#include "ui_common.h"

/*! coordinate */
#define UPGRADE_BY_RS232_CONT_X        ((COMM_BG_W - UPGRADE_BY_RS232_CONT_W) / 2)
#define UPGRADE_BY_RS232_CONT_H        360

#define UPGRADE_BY_RS232_ITEM_CNT      5
#define UPGRADE_BY_RS232_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define UPGRADE_BY_RS232_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define UPGRADE_BY_RS232_ITEM_LW       COMM_ITEM_LW
#define UPGRADE_BY_RS232_ITEM_RW       (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_RS232_ITEM_LW)
#define UPGRADE_BY_RS232_ITEM_H        COMM_ITEM_H
#define UPGRADE_BY_RS232_ITEM_V_GAP    10 //20

#define UPGRADE_BY_RS232_TXT_X	UPGRADE_BY_RS232_ITEM_X
#define UPGRADE_BY_RS232_TXT_W	0//60
#define UPGRADE_BY_RS232_TXT_H	30

#define UPGRADE_BY_RS232_PBAR_X  (UPGRADE_BY_RS232_TXT_X + UPGRADE_BY_RS232_TXT_W)
#define UPGRADE_BY_RS232_PBAR_W (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_RS232_PER_W)
#define UPGRADE_BY_RS232_PBAR_H  16

#define UPGRADE_BY_RS232_PER_X (UPGRADE_BY_RS232_PBAR_X + UPGRADE_BY_RS232_PBAR_W)
#define UPGRADE_BY_RS232_PER_W 60
#define UPGRADE_BY_RS232_PER_H 30

#define UPGRADE_BY_RS232_MODE_CNT 1
#define UPGRADE_BY_RS232_TYPE_CNT 2

#define UPGRADE_BY_RS232_STS_ITEM_CNT 4

/*! rect style */
#define RSI_UPGRADE_BY_RS232_FRM       RSI_WINDOW_1
#define RSI_UPGRADE_BY_RS232_PBAR_BG     RSI_PROGRESS_BAR_BG
#define RSI_UPGRADE_BY_RS232_PBAR_MID    RSI_PROGRESS_BAR_MID_ORANGE

/*! font style */

/*! others*/

RET_CODE open_upgrade_by_rs232(u32 para1, u32 para2);

BOOL ui_is_rs232_upgrading(void);

#endif
