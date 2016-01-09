/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef _UI_SMALL_LIST_PVR_H_
#define _UI_SMALL_LIST_PVR_H_
/*!
 \file ui_small_list_pvr.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in SMALLor list menu.
   And interfaces for open the SMALLor list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */

/*coordinate*/



#define SMALL_LIST_MENU_WIDTH_PVR         600
#define SMALL_LIST_MENU_HEIGHT_PVR        460
#define SMALL_LIST_MENU_CONT_X_PVR        0
#define SMALL_LIST_MENU_CONT_Y_PVR        0

#define SMALL_LIST_H_GAP_PVR    6
#define SMALL_LIST_V_GAP_PVR    4

//group
#define SMALL_LIST_SAT_X_PVR              100
#define SMALL_LIST_SAT_Y_PVR              80
#define SMALL_LIST_SAT_W_PVR              430 
#define SMALL_LIST_SAT_H_PVR              46


//list
#define SMALL_LIST_LIST_X_PVR             100
#define SMALL_LIST_LIST_Y_PVR             (SMALL_LIST_SAT_Y_PVR + SMALL_LIST_SAT_H_PVR + 10)
#define SMALL_LIST_LIST_W_PVR             420
#define SMALL_LIST_LIST_H_PVR             320

#define SMALL_LIST_MID_L_PVR              4
#define SMALL_LIST_MID_T_PVR              6
#define SMALL_LIST_MID_W_PVR              (SMALL_LIST_LIST_W_PVR - 2 * SMALL_LIST_MID_L_PVR)
#define SMALL_LIST_MID_H_PVR              (SMALL_LIST_LIST_H_PVR - 2 * SMALL_LIST_MID_T_PVR)

#define SMALL_LIST_SBAR_X_PVR             (SMALL_LIST_LIST_X_PVR + SMALL_LIST_LIST_W_PVR + 4) 
#define SMALL_LIST_SBAR_Y_PVR             (SMALL_LIST_LIST_Y_PVR)
#define SMALL_LIST_SBAR_W_PVR             6
#define SMALL_LIST_SBAR_H_PVR             SMALL_LIST_LIST_H_PVR
#define SMALL_LIST_SBAR_VERTEX_GAP_PVR    12


/*rstyle*/
#define RSI_SLIST_LIST_PVR                RSI_TRANSLUCENT          //rect style of the list
#define RSI_SLIST_SBAR_PVR                RSI_SCROLL_BAR_BG  //rect style of scroll bar
#define RSI_SLIST_SBAR_MID_PVR            RSI_SCROLL_BAR_MID //rect style of scroll bar middle rectangle
#define RSI_SLIST_SAT_CBOX_PVR            RSI_SMALL_LIST_TITLE_TRANS


/*fstyle*/
#define FSI_SLIST_TEXT_PVR                FSI_WHITE
#define FSI_SLIST_BOTTOM_PVR              FSI_WHITE//FSI_PROGBAR_BOTTOM


/*others*/
#define SMALL_LIST_PAGE_PVR               8
#define SMALL_LIST_FIELD_NUM_PVR          7

RET_CODE open_small_list_pvr(u32 para1, u32 para2);

#endif

