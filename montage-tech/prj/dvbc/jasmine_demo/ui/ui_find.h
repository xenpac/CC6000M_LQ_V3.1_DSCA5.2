/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_FIND_H__
#define __UI_FIND_H__
//coordinate

#define FIND_MENU_ITEM_V_GAP  6         

#define FIND_MENU_CONT_WIDTH  SCREEN_WIDTH
#define FIND_MENU_CONT_HEIGHT SCREEN_HEIGHT
#define FIND_MENU_CONT_X 0
#define FIND_MENU_CONT_Y 0

// list cont
#define FIND_LIST_CONT_X  0
#define FIND_LIST_CONT_Y  0
#define FIND_LIST_CONT_W  410
#define FIND_LIST_CONT_H  579

//keyboard container
#define FIND_EDIT_CONTX (FIND_MENU_CONT_WIDTH - FIND_EDIT_CONTW)
#define FIND_EDIT_CONTY (FIND_LIST_CONT_Y + 12)
#define FIND_EDIT_CONTW 424
#define FIND_EDIT_CONTH 554

//top line
#define FIND_TOP_LINE_X  40
#define FIND_TOP_LINE_Y  79
#define FIND_TOP_LINE_W  320
#define FIND_TOP_LINE_H  18

//bottom line
#define FIND_BOTTOM_LINE_X  FIND_TOP_LINE_X
#define FIND_BOTTOM_LINE_Y  480
#define FIND_BOTTOM_LINE_W  FIND_TOP_LINE_W
#define FIND_BOTTOM_LINE_H  18

//list
#define FIND_LIST_X  60
#define FIND_LIST_Y  93
#define FIND_LIST_W  290
#define FIND_LIST_H  380

#define FIND_LIST_MID_L 4
#define FIND_LIST_MID_T 4
#define FIND_LIST_MID_W (FIND_LIST_W - 2 * FIND_LIST_MID_L)
#define FIND_LIST_MID_H (FIND_LIST_H - 2 * FIND_LIST_MID_T)

#define FIND_SBAR_X  (FIND_LIST_X + FIND_LIST_W)
#define FIND_SBAR_Y  FIND_LIST_Y
#define FIND_SBAR_W  6
#define FIND_SBAR_H FIND_LIST_H

#define FIND_SBAR_VERTEX_GAP         12

/*rstyle*/
#define RSI_FIND_CONT              RSI_TRANSPARENT//rect style of program list window
#define RSI_FIND_LIST_CONT           RSI_WINDOW_1//rect style of the list
#define RSI_FIND_LIST           RSI_PBACK//rect style of the list
#define RSI_FIND_SBAR              RSI_SCROLL_BAR_BG //rect style of scroll bar
#define RSI_FIND_SBAR_MID       RSI_SCROLL_BAR_MID//rect style of scroll bar middle rectangle

/*others*/
#define FIND_LIST_PAGE                      9
#define FIND_LIST_FIELD_NUM            2

RET_CODE open_find(u32 para1, u32 para2);
#endif


