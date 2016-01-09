/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_FAVORITE_H__
#define __UI_FAVORITE_H__

/* coordinate */
#define FAVORITE_LIST_X        COMM_ITEM_OX_IN_ROOT
#define FAVORITE_LIST_Y        COMM_ITEM_OY_IN_ROOT
#define FAVORITE_LIST_W       (COMM_ITEM_MAX_WIDTH - FAVORITE_SBAR_WIDTH)
#define FAVORITE_LIST_H       (FAVORITE_LIST_MID_H + 2 * FAVORITE_LIST_MID_T)

#define FAVORITE_LIST_MID_L     4
#define FAVORITE_LIST_MID_T     4
#define FAVORITE_LIST_MID_W   (FAVORITE_LIST_W - 2 * FAVORITE_LIST_MID_L)
#define FAVORITE_LIST_MID_H    ((COMM_ITEM_H + FAVORITE_LIST_ITEM_VGAP) * FAVORITE_LIST_PAGE - FAVORITE_LIST_ITEM_VGAP)

#define FAVORITE_LIST_ITEM_H        COMM_ITEM_H
#define FAVORITE_LIST_ITEM_VGAP    4

#define FAVORITE_SBAR_X           (FAVORITE_LIST_X + FAVORITE_LIST_W)
#define FAVORITE_SBAR_Y           FAVORITE_LIST_Y
#define FAVORITE_SBAR_WIDTH      6
#define FAVORITE_SBAR_HEIGHT    FAVORITE_LIST_H

#define FAVORITE_SBAR_VERTEX_GAP         12


/* rect style */
#define RSI_FAVORITE_FRM       RSI_WINDOW_1
#define RSI_FAVORITE_LIST       RSI_PBACK
#define RSI_FAVORITE_SBAR_BG     RSI_SCROLL_BAR_BG
#define RSI_FAVORITE_SBAR_MID    RSI_SCROLL_BAR_MID
/* font style */

/* others */
#define FAVORITE_LIST_PAGE    8
#define FAVORITE_LIST_FIELD   2

RET_CODE open_favorite(u32 para1, u32 para2);

#endif