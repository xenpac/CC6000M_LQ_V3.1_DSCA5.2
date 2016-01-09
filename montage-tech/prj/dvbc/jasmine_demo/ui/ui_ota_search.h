/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_OTA_SEARCH_H__
#define __UI_OTA_SEARCH_H__

/* coordinate */
#define OTA_SEARCH_CONT_X        ((COMM_BG_W - OTA_SEARCH_CONT_W) / 2)
#define OTA_SEARCH_CONT_Y        COMM_BG_Y
#define OTA_SEARCH_CONT_W        COMM_BG_W
#define OTA_SEARCH_CONT_H        COMM_BG_H

#define OTA_SEARCH_ITEM_X COMM_ITEM_OX_IN_ROOT
#define OTA_SEARCH_ITEM_Y COMM_ITEM_OY_IN_ROOT
#define OTA_SEARCH_ITEM_W COMM_ITEM_MAX_WIDTH
#define OTA_SEARCH_ITEM_LW COMM_ITEM_LW
#define OTA_SEARCH_ITEM_RW (COMM_ITEM_MAX_WIDTH - COMM_ITEM_LW)
#define OTA_SEARCH_ITEM_H COMM_ITEM_H

#define OTA_SEARCH_ITEM_VGAP 5

#define OTA_COLLECT_EXIT_X ((OTA_SEARCH_CONT_W - OTA_COLLECT_EXIT_W)/2)
#define OTA_COLLECT_EXIT_W 160
#define OTA_COLLECT_EXIT_H COMM_ITEM_H

#define OTA_CHECK_YES_X ((OTA_SEARCH_CONT_W - 2 * OTA_CHECK_YES_W)/3)
#define OTA_CHECK_YES_W 120
#define OTA_CHECK_YES_H COMM_ITEM_H

#define OTA_CHECK_NO_X  (OTA_SEARCH_CONT_W - OTA_CHECK_YES_X - OTA_CHECK_NO_W)
#define OTA_CHECK_NO_W OTA_CHECK_YES_W
#define OTA_CHECK_NO_H COMM_ITEM_H

/* rect style */
#define RSI_OTA_SEARCH_FRM       RSI_WINDOW_1
#define RSI_OTA_SEARCH_BTN_HL     RSI_ITEM_1_HL
#define RSI_OTA_SEARCH_BTN_SH RSI_ITEM_1_SH

/* font style */
#define FSI_OTA_SEARCH_INFO  FSI_WHITE
#define FSI_OTA_SEARCH_BTN FSI_BLACK

/* others */
#define OTA_CHECK_ITEM_CNT 3
#define OTA_COLLECT_ITEM_CNT 2

RET_CODE open_ota_search(u32 para1, u32 para2);

#endif