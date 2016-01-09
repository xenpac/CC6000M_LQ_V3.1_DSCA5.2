/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_UPGRADE_BY_NETWORK_H_
#define _UI_UPGRADE_BY_NETWORK_H_


//net upgrade items
#define UPGRADE_BY_NET_ITEM1_X        120
#define UPGRADE_BY_NET_ITEM1_Y        100
#define UPGRADE_BY_NET_ITEM1_LW       COMM_ITEM_LW
#define UPGRADE_BY_NET_ITEM1_RW       (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_NET_ITEM1_LW)
#define UPGRADE_BY_NET_ITEM1_H        36
#define UPGRADE_BY_NET_ITEM1_V_GAP    20

//process bar
#define UPGRADE_BY_NET_TXT1_X 	UPGRADE_BY_NET_ITEM1_X
#define UPGRADE_BY_NET_TXT1_W	0
#define UPGRADE_BY_NET_TXT1_H	  30

#define UPGRADE_BY_NET_PBAR1_X  (UPGRADE_BY_NET_TXT1_X + UPGRADE_BY_NET_TXT1_W)
#define UPGRADE_BY_NET_PBAR1_Y UPGRADE_BY_NET_ITEM1_Y  + UPGRADE_BY_NET_ITEM_CNT*(UPGRADE_BY_NET_ITEM1_H + \
                                                          UPGRADE_BY_NET_ITEM1_V_GAP) + UPGRADE_BY_NET_ITEM1_V_GAP
#define UPGRADE_BY_NET_PBAR1_W (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_NET_PER1_W)
#define UPGRADE_BY_NET_PBAR1_H  16

#define UPGRADE_BY_NET_PER1_X (UPGRADE_BY_NET_PBAR1_X + UPGRADE_BY_NET_PBAR1_W)
#define UPGRADE_BY_NET_PER1_W 60
#define UPGRADE_BY_NET_PER1_H 30

//upgrade text status
#define UPGRADE_BY_NET_STS_X  UPGRADE_BY_NET_ITEM1_X
#define UPGRADE_BY_NET_STS_Y  UPGRADE_BY_NET_PBAR1_Y + 25
#define UPGRADE_BY_NET_STS_W  UPGRADE_BY_NET_ITEM1_LW + UPGRADE_BY_NET_ITEM1_RW
#define UPGRADE_BY_NET_STS_H  UPGRADE_BY_NET_ITEM1_H


//dlg
#define UPGRADE_BY_NET_DLG_X   ((SCREEN_WIDTH -UPGRADE_BY_NET_DLG_W)/2)
#define UPGRADE_BY_NET_DLG_Y   ((SCREEN_HEIGHT-UPGRADE_BY_NET_DLG_H)/2)
#define UPGRADE_BY_NET_DLG_W   300
#define UPGRADE_BY_NET_DLG_H    200

//count
#define UPGRADE_BY_NET_PROTOCOL_CNT 1
#define UPGRADE_BY_NET_URL_TYPE_CNT 2
#define UPGRADE_BY_NET_ITEM_CNT  6
#define UPGRADE_BY_NET_LEN 128
/*! rect style */
#define RSI_UPGRADE_BY_NET_PBAR_BG     RSI_PROGRESS_BAR_BG
#define RSI_UPGRADE_BY_NET_PBAR_MID    RSI_PROGRESS_BAR_MID_ORANGE

RET_CODE open_upgrade_by_network(u32 para1, u32 para2);
RET_CODE open_net_upg_burn(u32 para1, u32 para2);


#endif
