/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_LANGUAGE_H__
#define __UI_LANGUAGE_H__

/* coordinate */
#define LANGUAGE_CONT_X			((COMM_BG_W-LANGUAGE_CONT_W)/2)
#define LANGUAGE_CONT_H			420

#if ENABLE_TTX_SUBTITLE
#define LANGUAGE_ITEM_CNT		5
#else
#define LANGUAGE_ITEM_CNT		3
#endif
#define LANGUAGE_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define LANGUAGE_ITEM_Y			COMM_ITEM_OY_IN_ROOT
#define LANGUAGE_ITEM_LW		COMM_ITEM_LW
#define LANGUAGE_ITEM_RW		(COMM_ITEM_MAX_WIDTH - LANGUAGE_ITEM_LW)
#define LANGUAGE_ITEM_H			COMM_ITEM_H
#define LANGUAGE_ITEM_V_GAP		15 //20

#define LANGUAGE_MAX_CNT    2//11

/* rect style */
#define RSI_LANGUAGE_FRM		RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_language (u32 para1, u32 para2);

#endif

