/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef _UI_NETWORK_UPGRADE_H_
#define _UI_NETWORK_UPGRADE_H_

#include "ui_common.h"

/*! coordinate */
#define UPGRADE_BY_NET_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define UPGRADE_BY_NET_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define UPGRADE_BY_NET_ITEM_LW       COMM_ITEM_LW
#define UPGRADE_BY_NET_ITEM_RW       (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_NET_ITEM_LW)
#define UPGRADE_BY_NET_ITEM_H        36//COMM_ITEM_H
#define UPGRADE_BY_NET_ITEM_V_GAP    5

#define UPGRADE_BY_NET_TXT_X	UPGRADE_BY_NET_ITEM_X
#define UPGRADE_BY_NET_TXT_W	0//60
#define UPGRADE_BY_NET_TXT_H	30

#define UPGRADE_BY_NET_PBAR_X  (UPGRADE_BY_NET_TXT_X + UPGRADE_BY_NET_TXT_W)
#define UPGRADE_BY_NET_PBAR_W (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_NET_PER_W)
#define UPGRADE_BY_NET_PBAR_H  16

#define UPGRADE_BY_NET_PER_X (UPGRADE_BY_NET_PBAR_X + UPGRADE_BY_NET_PBAR_W)
#define UPGRADE_BY_NET_PER_W 60
#define UPGRADE_BY_NET_PER_H 30

#define UPGRADE_BY_NET_LIST_X	240
#define UPGRADE_BY_NET_LIST_Y 300
#define UPGRADE_BY_NET_LIST_W	300
#define UPGRADE_BY_NET_LIST_H	260

#define UPGRADE_BY_NET_LIST_MIDL  0
#define UPGRADE_BY_NET_LIST_MIDT  0
#define UPGRADE_BY_NET_LIST_MIDW  (UPGRADE_BY_NET_LIST_W - 2 * UPGRADE_BY_NET_LIST_MIDL)
#define UPGRADE_BY_NET_LIST_MIDH  (UPGRADE_BY_NET_LIST_H - 2 * UPGRADE_BY_NET_LIST_MIDT)
#define UPGRADE_BY_NET_SELECT_MIDX 25
#define UPGRADE_BY_NET_SELECT_MIDY 335

#define UPGRADE_DLG_X   ((SCREEN_WIDTH -UPGRADE_DLG_W)/2)
#define UPGRADE_DLG_Y   ((SCREEN_HEIGHT-UPGRADE_DLG_H)/2)
#define UPGRADE_DLG_W   300
#define UPGRADE_DLG_H    200

#define UPGRADE_BY_NET_LIST_VGAP  0

#define UPGRADE_BY_NET_LIST_PAGE     7
#define UPGRADE_BY_NET_LIST_FIELD    1

#define UPGRADE_BY_NET_MODE_CNT 2

#define UPGRADE_BY_NET_MAX_MISC_FILES_CNT 7

#define UPGRADE_BY_NET_STS_ITEM_CNT 1  //  4

/*! rect style */
#define RSI_UPGRADE_BY_NET_FRM       RSI_WINDOW_1
#define RSI_UPGRADE_BY_NET_PBAR_BG     RSI_PROGRESS_BAR_BG
#define RSI_UPGRADE_BY_NET_PBAR_MID    RSI_PROGRESS_BAR_MID_ORANGE

/*! others*/
#define STS_STR_MAX_LEN       32
#define VERSION_LIST_CNT 2//4
#define VERSION_LIST_FIELD 3

typedef enum
{
  UI_NET_UPG_IDEL,
  UI_NET_UPG_LOADING,
  UI_NET_UPG_LOAD_END,
  UI_NET_UPG_BURNING
}ui_net_upg_status_t;

RET_CODE open_network_upgrade(u32 para1, u32 para2);

u32 ui_net_upgade_sts(void);
#endif
