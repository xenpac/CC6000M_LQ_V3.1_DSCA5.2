/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PROG_DETAIL_H__
#define __UI_PROG_DETAIL_H__

/* coordinate */
#define PDETAIL_CONT_X       ((SCREEN_WIDTH - PDETAIL_CONT_W)/2)
#define PDETAIL_CONT_Y       100
#define PDETAIL_CONT_W        900
#define PDETAIL_CONT_H        300

#define PDETAIL_V_GAP  4
#define PDETAIL_H_GAP  6

#define PDETAIL_TTL_X  0
#define PDETAIL_TTL_Y  0
#define PDETAIL_TTL_W  (PDETAIL_ITEM_W + 2 * PDETAIL_H_GAP)
#define PDETAIL_TTL_H (4 * PDETAIL_ITEM_H + 5 * PDETAIL_V_GAP)

#define PDETAIL_ITEM_X  PDETAIL_H_GAP
#define PDETAIL_ITEM_Y  PDETAIL_V_GAP
#define PDETAIL_ITEM_LW  168
#define PDETAIL_ITEM_RW 168
#define PDETAIL_ITEM_W (PDETAIL_ITEM_LW + PDETAIL_ITEM_RW+80)
#define PDETAIL_ITEM_H  30

#define PDETAIL_DETAIL_X 0
#define PDETAIL_DETAIL_Y 140
#define PDETAIL_DETAIL_W 900
#define PDETAIL_DETAIL_H 160

#define PDETAIL_DETAIL_MIDX PDETAIL_H_GAP
#define PDETAIL_DETAIL_MIDY PDETAIL_V_GAP
#define PDETAIL_DETAIL_MIDW (PDETAIL_DETAIL_W - 2 * PDETAIL_DETAIL_MIDX)
#define PDETAIL_DETAIL_MIDH (PDETAIL_DETAIL_H - 2 * PDETAIL_DETAIL_MIDY)

#define PDETAIL_BAR_X (PDETAIL_DETAIL_X + PDETAIL_DETAIL_W)
#define PDETAIL_BAR_Y PDETAIL_DETAIL_Y
#define PDETAIL_BAR_W 6
#define PDETAIL_BAR_H PDETAIL_DETAIL_H

#define PDETAIL_HELP_X PDETAIL_DETAIL_X
#define PDETAIL_HELP_Y (PDETAIL_DETAIL_Y + PDETAIL_DETAIL_H + PDETAIL_V_GAP)
#define PDETAIL_HELP_W PDETAIL_DETAIL_W
#define PDETAIL_HELP_H 40

/* rect style */
#define RSI_PDETAIL_FRM       RSI_COMMAN_BG
#define RSI_PDETAIL_TTL       RSI_PBACK
#define RSI_PDETAIL_DTL       RSI_PBACK 
#define RSI_PDETAIL_HELP      RSI_PBACK
#define RSI_PDETAIL_SBAR          RSI_SCROLL_BAR_BG
#define RSI_PDETAIL_SBAR_MID      RSI_SCROLL_BAR_MID

/* font style */
#define FSI_PROG_DETAIL_TXT  FSI_WHITE


/* others */
#define PDETAIL_ITEM_NUM 5

#define PDETAIL_MAX_LENTH 1024

RET_CODE open_prog_detail(u32 para1, u32 para2);

#endif

