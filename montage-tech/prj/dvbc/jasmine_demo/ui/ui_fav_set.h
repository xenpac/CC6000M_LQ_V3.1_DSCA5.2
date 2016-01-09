/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_FAV_SET_H_
#define _UI_FAV_SET_H_
/*!
 \file ui_fav_set.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in fav list menu.
   And interfaces for open the fav list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */
#include "ui_common.h"

//coordinates
#define FAV_SET_X 206
#define FAV_SET_Y 238
#define FAV_SET_W  280//160
#define FAV_SET_H  (FAV_SET_ITEM_PAGE * FAV_SET_ITEM_HEIGHT - FAV_SET_LIST_Y + 15) 

#define FAV_SET_LIST_X 0
#define FAV_SET_LIST_Y 5//0
#define FAV_SET_LIST_W (FAV_SET_W - 2 * FAV_SET_LIST_X - FAV_SET_SBARW)
#define FAV_SET_LIST_H (FAV_SET_H - 2 * FAV_SET_LIST_Y)

#define FAV_SET_LIST_MIDL  10
#define FAV_SET_LIST_MIDT  0
#define FAV_SET_LIST_MIDW  (FAV_SET_LIST_W - 2*FAV_SET_LIST_MIDL)
#define FAV_SET_LIST_MIDH  (FAV_SET_LIST_H - 2*FAV_SET_LIST_MIDT)


#define FAV_SET_SBARX (FAV_SET_LIST_X + FAV_SET_LIST_W)
#define FAV_SET_SBARY FAV_SET_LIST_Y
#define FAV_SET_SBARW 6
#define FAV_SET_SBARH FAV_SET_LIST_H

#define FAV_SET_LIST_VGAP  0

//rstyles
#define RSI_FAV_SET_MENU RSI_COMMAN_BG
#define RSI_FAV_SET_SBAR  RSI_SCROLL_BAR_BG
#define RSI_FAV_SET_SBAR_MID  RSI_SCROLL_BAR_MID

//fstyles


//others
#define FAV_SET_ITEM_TOL  MAX_PRESET_FAV_CNT
#define FAV_SET_ITEM_PAGE 8
#define FAV_SET_ITEM_HEIGHT 36
#define FAV_SET_FIELD_NUM 2

RET_CODE open_fav_set(u32 para1, u32 para2);

#endif

