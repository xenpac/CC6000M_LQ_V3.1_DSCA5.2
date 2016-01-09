/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef _UI_FAV_LIST_H_
#define _UI_FAV_LIST_H_

#include "ui_common.h"


//preview
#define FAV_LIST_PREV_X            PREV_COMMON_X
#define FAV_LIST_PREV_Y            PREV_COMMON_Y
#define FAV_LIST_PREV_W            PREV_COMMON_W
#define FAV_LIST_PREV_H            PREV_COMMON_H

#define FAV_LIST_GROUP_X       COMM_ITEM_OX_IN_ROOT//(FAV_LIST_PREV_X + FAV_LIST_PREV_W + 20)
#define FAV_LIST_GROUP_Y       FAV_LIST_PREV_Y
#define FAV_LIST_GROUP_W       (COMM_BG_W - FAV_LIST_GROUP_X - WIN_LR_SPACE)
#define FAV_LIST_GROUP_H       COMM_ITEM_H//36

#define FAV_LIST_LIST_X        FAV_LIST_GROUP_X
#define FAV_LIST_LIST_Y        (FAV_LIST_GROUP_Y + FAV_LIST_GROUP_H + 10)
#define FAV_LIST_LIST_W        (FAV_LIST_GROUP_W - 10)
#define FAV_LIST_LIST_H        360//460
#define FAV_LIST_LIST_MIDL     10
#define FAV_LIST_LIST_MIDT     10
#define FAV_LIST_LIST_MIDW     (FAV_LIST_LIST_W - 2 * FAV_LIST_LIST_MIDL)
#define FAV_LIST_LIST_MIDH     (FAV_LIST_LIST_H - 2 * FAV_LIST_LIST_MIDT - 20)

#define FAV_LIST_BAR_X         (FAV_LIST_LIST_X + FAV_LIST_LIST_W + 4)
#define FAV_LIST_BAR_Y         FAV_LIST_LIST_Y
#define FAV_LIST_BAR_W         6
#define FAV_LIST_BAR_H         FAV_LIST_LIST_H


#define FAV_LIST_LIST_VGAP         0

#define RSI_FAV_LIST_MENU          RSI_PBACK
#define RSI_FAV_LIST_SBAR          RSI_SCROLL_BAR_BG
#define RSI_FAV_LIST_SBAR_MID      RSI_SCROLL_BAR_MID
#define RSI_FAV_LIST_PREV          RSI_TV

#define FAV_LIST_LIST_PAGE         8//12
#define FAV_LIST_LIST_FIELD        2

/*rstyle*/
#define RSI_FLIST_CONT              RSI_TRANSPARENT//rect style of program list window
#define RSI_FLIST_LIST              RSI_TRANSLUCENT
#define RSI_FLIST_SBAR              RSI_SCROLL_BAR_BG //rect style of scroll bar
#define RSI_FLIST_SBAR_MID          RSI_SCROLL_BAR_MID//rect style of scroll bar middle rectangle
#define RSI_FLIST_TITLE             RSI_SMALL_LIST_TITLE

/*fstyle*/
#define FSI_FLIST_TEXT                      FSI_WHITE     

enum flist_control_id
{
  IDC_FLIST_LIST = 1,
  IDC_FLIST_SBAR,
  IDC_FLIST_TTL,
};

RET_CODE open_fav_list(u32 para1, u32 para2);

#endif

