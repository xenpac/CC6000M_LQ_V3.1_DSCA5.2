/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_TERRESTRIAL_MANUAL_SEARCH_H__
#define __UI_TERRESTRIAL_MANUAL_SEARCH_H__

/* coordinate */

#define _MANUAL_SEARCH_ITEM_CNT		2
#define _MANUAL_SEARCH_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define _MANUAL_SEARCH_ITEM_Y			130
#define _MANUAL_SEARCH_ITEM_LW		  COMM_ITEM_LW
#define _MANUAL_SEARCH_ITEM_RW		  (COMM_ITEM_MAX_WIDTH - _MANUAL_SEARCH_ITEM_LW)
#define _MANUAL_SEARCH_ITEM_H			COMM_ITEM_H
#define _MANUAL_SEARCH_ITEM_V_GAP	16
#define _MANUAL_SEARCH_FREQ_BIT    6

#define _RSI_MANUAL_SEARCH_BTN_SH    RSI_PBACK
#define _RSI_MANUAL_SEARCH_BTN_HL    RSI_ITEM_1_HL
/* rect style */
#define _RSI_LANGUAGE_FRM		RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_terrestrial_manual_search(u32 para1, u32 para2);

#endif

