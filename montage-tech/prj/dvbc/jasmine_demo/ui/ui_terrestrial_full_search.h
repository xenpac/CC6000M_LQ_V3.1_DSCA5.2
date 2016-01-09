/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_TERRESTRIAL_FULL_SEARCH_H__
#define __UI_TERRESTRIAL_FULL_SEARCH_H__

/* coordinate */

#define FULL_SEARCH_ITEM_CNT		3
#define FULL_SEARCH_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define FULL_SEARCH_ITEM_Y			130
#define FULL_SEARCH_ITEM_LW		COMM_ITEM_LW
#define FULL_SEARCH_ITEM_RW		(COMM_ITEM_MAX_WIDTH - FULL_SEARCH_ITEM_LW)
#define FULL_SEARCH_ITEM_H			COMM_ITEM_H
#define FULL_SEARCH_ITEM_V_GAP		16
#define FULL_SEARCH_FREQ_BIT        6

#define RSI_FULL_SEARCH_BTN_SH    RSI_PBACK
#define RSI_FULL_SEARCH_BTN_HL    RSI_ITEM_1_HL
/* rect style */
#define RSI_LANGUAGE_FRM		RSI_WINDOW_1
/* font style */

/* others */
RET_CODE open_terrestrial_full_search(u32 para1, u32 para2);

#endif


#if 0
#define __UI_FULL_SEARCH_H__

/* coordinate */
#define FULL_SEARCH_CONT_W        480
#define FULL_SEARCH_CONT_H        350
#define FULL_SEARCH_CONT_X        ((SCREEN_WIDTH - FULL_SEARCH_CONT_W) / 2)
#define FULL_SEARCH_CONT_Y        ((SCREEN_HEIGHT- FULL_SEARCH_CONT_H) / 2)

#define FULL_SCAN_ITEM_CNT      2
#define FULL_SCAN_ITEM_X        20
#define FULL_SCAN_ITEM_Y        120
#define FULL_SCAN_ITEM_LW       220
#define FULL_SCAN_ITEM_RW       220
#define FULL_SCAN_ITEM_H        COMM_ITEM_H
#define FULL_SCAN_ITEM_V_GAP    20

#define FULL_SCAN_FREQ_MIN    474000
#define FULL_SCAN_FREQ_MAX    864000
#define FULL_SCAN_FREQ_BIT  6



RET_CODE open_full_search(u32 para1, u32 para2);

#endif


