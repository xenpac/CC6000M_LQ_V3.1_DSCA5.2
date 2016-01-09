/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NETWORKPLACES_H__
#define __UI_NETWORKPLACES_H__

/* coordinate */
#define NETWORK_LIST_X        COMM_ITEM_OX_IN_ROOT
#define NETWORK_LIST_Y        COMM_ITEM_OY_IN_ROOT
#define NETWORK_LIST_W       (COMM_ITEM_MAX_WIDTH - NETWORK_SBAR_WIDTH)
#define NETWORK_LIST_H       (NETWORK_LIST_MID_H + 2 * NETWORK_LIST_MID_T)

#define NETWORK_LIST_MID_L     4
#define NETWORK_LIST_MID_T     4
#define NETWORK_LIST_MID_W   (NETWORK_LIST_W - 2 * NETWORK_LIST_MID_L)
#define NETWORK_LIST_MID_H    ((32 + NETWORK_LIST_ITEM_VGAP) * NETWORK_LIST_PAGE - NETWORK_LIST_ITEM_VGAP)

#define NETWORK_LIST_ITEM_H        COMM_ITEM_H
#define NETWORK_LIST_ITEM_VGAP    6

#define NETWORK_SBAR_X           (NETWORK_LIST_X + NETWORK_LIST_W)
#define NETWORK_SBAR_Y           NETWORK_LIST_Y
#define NETWORK_SBAR_WIDTH      6
#define NETWORK_SBAR_HEIGHT    NETWORK_LIST_H

#define NETWORK_SBAR_VERTEX_GAP         12


/* rect style */
#define RSI_NETWORK_FRM       RSI_WINDOW_1
#define RSI_NETWORK_LIST       RSI_PBACK
#define RSI_NETWORK_SBAR_BG     RSI_SCROLL_BAR_BG
#define RSI_NETWORK_SBAR_MID    RSI_SCROLL_BAR_MID
/* font style */

/* others */
#define NETWORK_LIST_PAGE    9
#define NETWORK_LIST_FIELD   2

RET_CODE open_networkplaces(u32 para1, u32 para2);

#endif
