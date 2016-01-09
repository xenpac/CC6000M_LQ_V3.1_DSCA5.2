/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef _UI_CATEGORY_H_
#define _UI_CATEGORY_H_

#include "ui_common.h"

//preview
#define CATEGORY_PREV_X            PREV_COMMON_X
#define CATEGORY_PREV_Y            PREV_COMMON_Y
#define CATEGORY_PREV_W            PREV_COMMON_W
#define CATEGORY_PREV_H            PREV_COMMON_H

#define CATEGORY_GROUP_X       COMM_ITEM_OX_IN_ROOT//(CATEGORY_PREV_X + 20)
#define CATEGORY_GROUP_Y       COMM_ITEM_OY_IN_ROOT//CATEGORY_PREV_Y
#define CATEGORY_GROUP_W       (COMM_BG_W - 2 * CATEGORY_GROUP_X)
#define CATEGORY_GROUP_H       COMM_ITEM_H //36

#define CATEGORY_LIST_X        CATEGORY_GROUP_X  //420
#define CATEGORY_LIST_Y        (CATEGORY_GROUP_Y + CATEGORY_GROUP_H + 10) //175
#define CATEGORY_LIST_W        (CATEGORY_GROUP_W - 10) //440
#define CATEGORY_LIST_H         (COMM_BG_H - CATEGORY_LIST_Y - 50)//460
#define CATEGORY_LIST_MIDL     10
#define CATEGORY_LIST_MIDT     10
#define CATEGORY_LIST_MIDW     (CATEGORY_LIST_W - 2 * CATEGORY_LIST_MIDL)
#define CATEGORY_LIST_MIDH     (CATEGORY_LIST_H - 2 * CATEGORY_LIST_MIDT)

#define CATEGORY_BAR_X         (CATEGORY_LIST_X + CATEGORY_LIST_W + 4)
#define CATEGORY_BAR_Y         CATEGORY_LIST_Y
#define CATEGORY_BAR_W         6
#define CATEGORY_BAR_H         CATEGORY_LIST_H


#define CATEGORY_LIST_VGAP         0

#define RSI_CATEGORY_SBAR          RSI_SCROLL_BAR_BG
#define RSI_CATEGORY_SBAR_MID      RSI_SCROLL_BAR_MID
#define RSI_CATEGORY_PREV          RSI_TV

#define CATEGORY_LIST_PAGE         8//12
#define CATEGORY_LIST_FIELD        2

RET_CODE open_category(u32 para1, u32 para2);

#endif
