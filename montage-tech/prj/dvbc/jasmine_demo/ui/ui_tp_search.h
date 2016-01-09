/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_TP_SEARCH_H__
#define __UI_TP_SEARCH_H__

/* coordinate */
#define TP_SEARCH_CONT_X       ((COMM_BG_W - TP_SEARCH_CONT_W) / 2)
#define TP_SEARCH_CONT_Y       ((COMM_BG_H - TP_SEARCH_CONT_H) / 2 - 10)
#define TP_SEARCH_CONT_W       460//400
#define TP_SEARCH_CONT_H       280

#define TP_SEARCH_ITEM_NUM     3
#define TP_SEARCH_ITEM_X       10
#define TP_SEARCH_ITEM_LW     250// 200
#define TP_SEARCH_ITEM_RW      (TP_SEARCH_CONT_W - 2 * TP_SEARCH_ITEM_X - \
                                TP_SEARCH_ITEM_LW)
#define TP_SEARCH_ITEM_H       36
#define TP_SEARCH_ITEM_VGAP    4

#define TP_SEARCH_BTN_CNT      2
#define TP_SEARCH_BTN_W        100
#define TP_SEARCH_BTN_X        ((TP_SEARCH_CONT_W - 2 * TP_SEARCH_BTN_W - \
                                 TP_SEARCH_BTN_HGAP) / 2)
#define TP_SEARCH_BTN_Y        210
#define TP_SEARCH_BTN_H        36
#define TP_SEARCH_BTN_HGAP     60

/* rect style */

/* font style */

/*rstyle*/
#define RSI_TP_SEARCH_FRM       RSI_COMMAN_BG
#define RSI_TP_SEARCH_BTN_SH    RSI_ITEM_1_SH
#define RSI_TP_SEARCH_BTN_HL    RSI_ITEM_1_HL


/*others*/

#define TP_SEARCH_MAX      8191
#define TP_SEARCH_MIN      0
#define TP_SEARCH_PLACE    0
#define TP_SEARCH_BLEN     4

RET_CODE open_tp_search(u32 para1, u32 para2);

#endif
