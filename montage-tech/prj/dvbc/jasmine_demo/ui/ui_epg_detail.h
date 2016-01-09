/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_EPG_DETAIL_H__
#define __UI_EPG_DETAIL_H__

/* coordinate */
#define DETAIL_CONT_X        ((COMM_BG_W - DETAIL_CONT_W) / 2)
#define DETAIL_CONT_Y        ((COMM_BG_H- DETAIL_CONT_H) / 2)
#define DETAIL_CONT_W        (DETAIL_TTL_W + DETAIL_BAR_W)
#define DETAIL_CONT_H        (DETAIL_TTL_H + DETAIL_DETAIL_H + DETAIL_HELP_H + 2 * DETAIL_V_GAP)

#define DETAIL_V_GAP  4
#define DETAIL_H_GAP  4

#define DETAIL_TTL_X  COMM_ITEM_OX_IN_ROOT//0
#define DETAIL_TTL_Y  COMM_ITEM_OY_IN_ROOT//0
#define DETAIL_TTL_W  (DETAIL_ITEM_W + 2 * DETAIL_H_GAP)
#define DETAIL_TTL_H (4 * DETAIL_ITEM_H + 5 * DETAIL_V_GAP)

#define DETAIL_ITEM_X  DETAIL_H_GAP
#define DETAIL_ITEM_Y  DETAIL_V_GAP
#define DETAIL_ITEM_LW  368
#define DETAIL_ITEM_RW 368
#define DETAIL_ITEM_W (DETAIL_ITEM_LW + DETAIL_ITEM_RW)
#define DETAIL_ITEM_H  30//24

#define DETAIL_DETAIL_X DETAIL_TTL_X
#define DETAIL_DETAIL_Y (DETAIL_TTL_Y + DETAIL_TTL_H + DETAIL_V_GAP * 3)  //272
#define DETAIL_DETAIL_W DETAIL_TTL_W
#define DETAIL_DETAIL_H 240

#define DETAIL_DETAIL_MIDX DETAIL_H_GAP
#define DETAIL_DETAIL_MIDY DETAIL_V_GAP
#define DETAIL_DETAIL_MIDW (DETAIL_DETAIL_W - 2 * DETAIL_DETAIL_MIDX)
#define DETAIL_DETAIL_MIDH (DETAIL_DETAIL_H - 2 * DETAIL_DETAIL_MIDY)

#define DETAIL_BAR_X (DETAIL_DETAIL_X + DETAIL_DETAIL_W)
#define DETAIL_BAR_Y DETAIL_DETAIL_Y
#define DETAIL_BAR_W 6
#define DETAIL_BAR_H DETAIL_DETAIL_H

#define DETAIL_HELP_X DETAIL_DETAIL_X
#define DETAIL_HELP_Y (DETAIL_DETAIL_Y + DETAIL_DETAIL_H + DETAIL_V_GAP)
#define DETAIL_HELP_W DETAIL_DETAIL_W
#define DETAIL_HELP_H 40

/* rect style */
#define RSI_DETAIL_BG         RSI_PBACK
#define RSI_DETAIL_FRM       RSI_COMMAN_BG
#define RSI_DETAIL_TTL       RSI_PBACK
#define RSI_DETAIL_DTL       RSI_COMMAN_BG 
#define RSI_DETAIL_HELP      RSI_PBACK
#define RSI_DETAIL_SBAR          RSI_SCROLL_BAR_BG
#define RSI_DETAIL_SBAR_MID      RSI_SCROLL_BAR_MID

/* font style */
#define FSI_EPG_DETAIL_TXT  FSI_WHITE


/* others */
#define DETAIL_ITEM_NUM 5

#define DETAIL_MAX_LENTH 1024

RET_CODE open_epg_detail(u32 para1, u32 para2);

#endif

