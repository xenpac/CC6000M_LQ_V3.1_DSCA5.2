/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef _UI_SMALL_LIST_H_
#define _UI_SMALL_LIST_H_
/*!
 \file ui_small_list.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in SMALLor list menu.
   And interfaces for open the SMALLor list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */

/*coordinate*/

#define SMALL_LIST_ITEM_V_GAP         6
#define SMALL_LIST_ITEM_H_GAP         40


#define SMALL_LIST_MENU_WIDTH         UI_COMMON_WIDTH
#define SMALL_LIST_MENU_HEIGHT        UI_COMMON_HEIGHT
#define SMALL_LIST_MENU_CONT_X        (0 + SCREEN_POS_X)
#define SMALL_LIST_MENU_CONT_Y        (0 + SCREEN_POS_Y)

//group
#define SMALL_LIST_SAT_X              80
#define SMALL_LIST_SAT_Y              0 //80
#define SMALL_LIST_SAT_W             315//400//430//to show AZ find
#define SMALL_LIST_SAT_H              46


//list
#define SMALL_LIST_LIST_X             80
#define SMALL_LIST_LIST_Y             (SMALL_LIST_SAT_Y + SMALL_LIST_SAT_H + 10)
#define SMALL_LIST_LIST_W            315//390//420
#define SMALL_LIST_LIST_H             400

#define SMALL_LIST_MID_L              4//8
#define SMALL_LIST_MID_T              8
#define SMALL_LIST_MID_W              (SMALL_LIST_LIST_W - 2 * SMALL_LIST_MID_L)
#define SMALL_LIST_MID_H              (SMALL_LIST_LIST_H - 2 * SMALL_LIST_MID_T)

#define SMALL_LIST_SBAR_X             (SMALL_LIST_LIST_X + SMALL_LIST_LIST_W + 6) //(SMALL_LIST_MID_W + 2 * SMALL_LIST_ITEM_H_GAP)
#define SMALL_LIST_SBAR_Y             SMALL_LIST_LIST_Y
#define SMALL_LIST_SBAR_W             6
#define SMALL_LIST_SBAR_H             SMALL_LIST_LIST_H

#define SMALL_LIST_AZ_X             (SMALL_LIST_SBAR_X + SMALL_LIST_SBAR_W + 6) //(SMALL_LIST_MID_W + 2 * SMALL_LIST_ITEM_H_GAP)
#define SMALL_LIST_AZ_Y             SMALL_LIST_LIST_Y
#define SMALL_LIST_AZ_W             30
#define SMALL_LIST_AZ_H             SMALL_LIST_SBAR_H

#define SMALL_LIST_AZ_ITEM_L          0
#define SMALL_LIST_AZ_ITEM_T          0
#define SMALL_LIST_AZ_ITEM_W          (SMALL_LIST_AZ_W - 2 * SMALL_LIST_AZ_ITEM_L)
#define SMALL_LIST_AZ_ITEM_H          (SMALL_LIST_AZ_H - 2 * SMALL_LIST_AZ_ITEM_T)

#define SMALL_LIST_AZ_ITEM_V_GAP      0
#define SLIST_AZ_PAGE        12
#define SLIST_AZ_TOTAL                  37
//ads
#define SMALL_LIST_HELP_X             SMALL_LIST_LIST_X
#define SMALL_LIST_HELP_Y             (SMALL_LIST_LIST_Y + SMALL_LIST_LIST_H + 10)
#define SMALL_LIST_HELP_W             SMALL_LIST_LIST_W
#define SMALL_LIST_HELP_H             (130)

#define SMALL_LIST_HELP_TXT_X             (SMALL_LIST_HELP_X + (SMALL_LIST_HELP_W -SMALL_LIST_HELP_TXT_W)/2)
#define SMALL_LIST_HELP_TXT_Y             (SMALL_LIST_HELP_Y + (SMALL_LIST_HELP_H -SMALL_LIST_HELP_TXT_H)/2)
#define SMALL_LIST_HELP_TXT_W             (SMALL_LIST_HELP_W - 10)
#define SMALL_LIST_HELP_TXT_H             (SMALL_LIST_HELP_H - 10)

#define SMALL_LIST_SBAR_VERTEX_GAP    0

/*rstyle*/
#define RSI_SLIST_CONT                RSI_TRANSPARENT       //rect style of program list window
#define RSI_SLIST_LIST                RSI_TRANSLUCENT          //rect style of the list
#define RSI_SLIST_SBAR                RSI_SCROLL_BAR_BG  //rect style of scroll bar
#define RSI_SLIST_SBAR_MID            RSI_SCROLL_BAR_MID //rect style of scroll bar middle rectangle
#define RSI_SLIST_SAT_CBOX            RSI_SMALL_LIST_TITLE_TRANS

/*fstyle*/
#define FSI_SLIST_INFO_EDIT           FSI_WHITE      //font styel of single satellite information edit
#define FSI_SLIST_TEXT                FSI_WHITE

/*others*/
#define SMALL_LIST_PAGE              8// 9
#define SMALL_LIST_FIELD_NUM          6//7

RET_CODE open_small_list(u32 para1, u32 para2);

#endif

