/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_TS_OTA_SEARCH_H__
#define __UI_TS_OTA_SEARCH_H__

/* coordinate */
#define TS_OTA_SEARCH_CONT_X        ((COMM_BG_W - TS_OTA_SEARCH_CONT_W) / 2)
#define TS_OTA_SEARCH_CONT_Y        COMM_BG_Y
#define TS_OTA_SEARCH_CONT_W        COMM_BG_W
#define TS_OTA_SEARCH_CONT_H        360

#define TS_OTA_SEARCH_ITEM_X COMM_ITEM_OX_IN_ROOT
#define TS_OTA_SEARCH_ITEM_Y COMM_ITEM_OY_IN_ROOT
#define TS_OTA_SEARCH_ITEM_W (TS_OTA_SEARCH_CONT_W - 2 * TS_OTA_SEARCH_ITEM_X)
#define TS_OTA_SEARCH_ITEM_H COMM_ITEM_H

#define TS_OTA_SEARCH_ITEM_VGAP 5

/* rect style */

/* font style */

/* others */
#define TS_OTA_ITEM_CNT 5

RET_CODE open_ts_ota_search(u32 para1, u32 para2);

#endif