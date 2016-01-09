/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_CHANNEL_EDIT_H__
#define __UI_CHANNEL_EDIT_H__

#include "ui_common.h"

#define CHANNEL_EDIT_MENU_X            (COMM_ITEM_OX_IN_ROOT - 30)//60
#define CHANNEL_EDIT_MENU_Y            COMM_ITEM_OY_IN_ROOT 
#define CHANNEL_EDIT_MENU_W            (COMM_BG_W - CHANNEL_EDIT_MENU_X)
#define CHANNEL_EDIT_MENU_H            (COMM_BG_H - CHANNEL_EDIT_MENU_Y - 70)

#define CHANNEL_EDIT_LIST_CONTX        0
#define CHANNEL_EDIT_LIST_CONTY        0
#define CHANNEL_EDIT_LIST_CONTW        (COMM_BG_W - 2 * CHANNEL_EDIT_MENU_X)//400//502//552
#define CHANNEL_EDIT_LIST_CONTH        (CHANNEL_EDIT_MENU_H - CHANNEL_EDIT_LIST_CONTY)

//prog list
#define CHANNEL_EDIT_LIST_X            10
#define CHANNEL_EDIT_LIST_Y            50//120
#define CHANNEL_EDIT_LIST_W            (CHANNEL_EDIT_LIST_CONTW - 2 * CHANNEL_EDIT_LIST_X)
#define CHANNEL_EDIT_LIST_H           (CHANNEL_EDIT_LIST_CONTH - CHANNEL_EDIT_LIST_Y)

#define CHANNEL_EDIT_LIST_MIDL  0
#define CHANNEL_EDIT_LIST_MIDT  0
#define CHANNEL_EDIT_LIST_MIDW  (CHANNEL_EDIT_LIST_W - 10)
#define CHANNEL_EDIT_LIST_MIDH (CHANNEL_EDIT_LIST_H - 2 * CHANNEL_EDIT_LIST_MIDT)

#define CHANNEL_EDIT_LIST_VGAP         0

#define CHANNEL_EDIT_BAR_X             (CHANNEL_EDIT_LIST_CONTW - CHANNEL_EDIT_BAR_W - 10)//490//540
#define CHANNEL_EDIT_BAR_Y             (CHANNEL_EDIT_LIST_Y + CHANNEL_EDIT_LIST_CONTY + CHANNEL_EDIT_LIST_MIDT)
#define CHANNEL_EDIT_BAR_W             6
#define CHANNEL_EDIT_BAR_H             (CHANNEL_EDIT_LIST_MIDH -5)

//head
#define CHANNEL_EDIT_LIST_HEADX        CHANNEL_EDIT_LIST_X
#define CHANNEL_EDIT_LIST_HEADY        8
#define CHANNEL_EDIT_LIST_HEADW       (CHANNEL_EDIT_LIST_CONTW - 2 * CHANNEL_EDIT_LIST_X)
#define CHANNEL_EDIT_LIST_HEADH        COMM_ITEM_H//36

//number
#define CHANNEL_EDIT_LIST_NUMBX        (CHANNEL_EDIT_LIST_HEADX+10)
#define CHANNEL_EDIT_LIST_NUMBY        CHANNEL_EDIT_LIST_HEADY
#define CHANNEL_EDIT_LIST_NUMBW       100
#define CHANNEL_EDIT_LIST_NUMBH        CHANNEL_EDIT_LIST_HEADH

//name
#define CHANNEL_EDIT_LIST_NAMEX        (CHANNEL_EDIT_LIST_NUMBX+CHANNEL_EDIT_LIST_NUMBW)
#define CHANNEL_EDIT_LIST_NAMEY        CHANNEL_EDIT_LIST_HEADY
#define CHANNEL_EDIT_LIST_NAMEW       100
#define CHANNEL_EDIT_LIST_NAMEH       CHANNEL_EDIT_LIST_HEADH

//sort container
#define CHANNEL_EDIT_SORT_CONT_X  0//100
#define CHANNEL_EDIT_SORT_CONT_Y  0//CHANNEL_EDIT_LIST_HEADX+CHANNEL_EDIT_LIST_HEADH+8
#define CHANNEL_EDIT_SORT_CONT_W  270
#define CHANNEL_EDIT_SORT_CONT_H  300

//sort list bg
#define CHANNEL_EDIT_SORT_LIST_BG_X     (CHANNEL_EDIT_SORT_LIST_TITLE_X-5)
#define CHANNEL_EDIT_SORT_LIST_BG_Y     (CHANNEL_EDIT_SORT_LIST_TITLE_Y-5)
#define CHANNEL_EDIT_SORT_LIST_BG_W     (CHANNEL_EDIT_SORT_LIST_TITLE_W+2*5)
#define CHANNEL_EDIT_SORT_LIST_BG_H     (CHANNEL_EDIT_SORT_LIST_Y+CHANNEL_EDIT_SORT_LIST_H+10-CHANNEL_EDIT_SORT_LIST_BG_Y)

//sort list title
#define CHANNEL_EDIT_SORT_LIST_TITLE_X  ((CHANNEL_EDIT_SORT_CONT_W-CHANNEL_EDIT_SORT_LIST_TITLE_W)/2)
#define CHANNEL_EDIT_SORT_LIST_TITLE_Y  6
#define CHANNEL_EDIT_SORT_LIST_TITLE_W  250
#define CHANNEL_EDIT_SORT_LIST_TITLE_H  COMM_ITEM_H//36

//sort list
#define CHANNEL_EDIT_SORT_LIST_X        CHANNEL_EDIT_SORT_LIST_TITLE_X
#define CHANNEL_EDIT_SORT_LIST_Y        (CHANNEL_EDIT_SORT_LIST_TITLE_Y+CHANNEL_EDIT_SORT_LIST_TITLE_H+4)
#define CHANNEL_EDIT_SORT_LIST_W        CHANNEL_EDIT_SORT_LIST_TITLE_W
#define CHANNEL_EDIT_SORT_LIST_H        (CHANNEL_EDIT_SORT_LIST_PAGE * CHANNEL_EDIT_SORT_LIST_TITLE_H)

#define CHANNEL_EDIT_SORT_LIST_MIDL         2
#define CHANNEL_EDIT_SORT_LIST_MIDT         3
#define CHANNEL_EDIT_SORT_LIST_MIDW         (CHANNEL_EDIT_SORT_LIST_W-2*CHANNEL_EDIT_SORT_LIST_MIDL)
#define CHANNEL_EDIT_SORT_LIST_MIDH         (CHANNEL_EDIT_SORT_LIST_H-CHANNEL_EDIT_SORT_LIST_MIDT)
#define CHANNEL_EDIT_SORT_LIST_VGAP         4

#define CHANNEL_EDIT_SORT_LIST_FIELD        1
#define CHANNEL_EDIT_SORT_LIST_PAGE         5

/*rstyle*/
#define RSI_CHANNEL_EDIT_MENU          RSI_PBACK
#define RSI_CHANNEL_EDIT_LIST          RSI_WINDOW_2
#define RSI_CHANNEL_EDIT_SBAR          RSI_SCROLL_BAR_BG
#define RSI_CHANNEL_EDIT_SBAR_MID      RSI_SCROLL_BAR_MID

/*others*/
#define CHANNEL_EDIT_LIST_PAGE         9//12
#define CHANNEL_EDIT_LIST_FIELD        6

RET_CODE open_channel_edit(u32 para1, u32 para2);

RET_CODE preopen_channel_edit(u32 para1, u32 para2);

void channel_edit_set_modify_state(BOOL state);

#endif

