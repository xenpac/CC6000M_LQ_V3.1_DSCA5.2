/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_KEYBOARD_MENU_H_
#define _UI_KEYBOARD_MENU_H_
/*!
 \file ui_keyboard_menu.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in keyboard menu.
   And defines interface for open a keyboard menu.
   
   Development policy:
   If you want to change the menu style, you will just need to modify the macro 
   definitions only.
 */
#include "ui_common.h"

/*!
coordinate 
*/
/*!
root container coordinate
*/
#define KEYBOARD_MENU_CONT_X			((SCREEN_WIDTH-KEYBOARD_MENU_CONT_W)/2)
#define KEYBOARD_MENU_CONT_Y			((SCREEN_HEIGHT-KEYBOARD_MENU_CONT_H)/2)
#define KEYBOARD_MENU_CONT_W			(KEYBOARD_MBOX_W + 2 * KEYBOARD_MBOX_X)
#define KEYBOARD_MENU_CONT_H			(KEYBOARD_EDIT_H + 2 * KEYBOARD_EDIT_MBOX_I + 2 * KEYBOARD_EDIT_Y + KEYBOARD_MBOX_H + KEYBOARD_PAGE_H)

/*!
coordinate for editbox of keyboard menu
*/
#define KEYBOARD_EDIT_X					10
#define KEYBOARD_EDIT_Y					5
#define KEYBOARD_EDIT_W					(KEYBOARD_MENU_CONT_W - 2 * KEYBOARD_EDIT_X)
#define KEYBOARD_EDIT_H					30

#define KEYBOARD_EDIT_MBOX_I				5
/*!
coordinate for matrixbox of keyboard menu
*/
#define KEYBOARD_MBOX_X					5
#define KEYBOARD_MBOX_Y					(KEYBOARD_EDIT_Y + KEYBOARD_EDIT_H + KEYBOARD_EDIT_MBOX_I)
#define KEYBOARD_MBOX_W					((KEYBOARD_ITEM_W + KEYBOARD_ITEM_INTERVAL_H) * KEYBOARD_MBOX_COL - KEYBOARD_ITEM_INTERVAL_H)
#define KEYBOARD_MBOX_H					((KEYBOARD_ITEM_H + KEYBOARD_ITEM_INTERVAL_V) * KEYBOARD_MBOX_ROW - KEYBOARD_ITEM_INTERVAL_V)

/*!
item coordinate
*/
#define KEYBOARD_ITEM_W					24
#define KEYBOARD_ITEM_H					24

/*!
Back button coordinate.
*/
#define KEYBOARD_BACK_X				KEYBOARD_MBOX_X
#define KEYBOARD_BACK_Y				(KEYBOARD_MBOX_Y + KEYBOARD_MBOX_ROW * (KEYBOARD_ITEM_H + KEYBOARD_ITEM_INTERVAL_V))
#define KEYBOARD_BACK_W				70
#define KEYBOARD_BACK_H				30


/*!
Enter button coordinate.
*/
#define KEYBOARD_OK_X					(KEYBOARD_BACK_X + KEYBOARD_BACK_W+KEYBOARD_ITEM_INTERVAL_H)	
#define KEYBOARD_OK_Y					KEYBOARD_BACK_Y
#define KEYBOARD_OK_W					KEYBOARD_BACK_W
#define KEYBOARD_OK_H					KEYBOARD_BACK_H


/*!
Page number control
*/
#define KEYBOARD_PAGE_X				(KEYBOARD_MENU_CONT_W-KEYBOARD_MBOX_X-KEYBOARD_PAGE_W)
#define KEYBOARD_PAGE_Y				KEYBOARD_BACK_Y
#define KEYBOARD_PAGE_W				80
#define KEYBOARD_PAGE_H				KEYBOARD_BACK_H

/*!
coordinate for keyboard item
*/
#define KEYBOARD_ITEM_INTERVAL_H		2
#define KEYBOARD_ITEM_INTERVAL_V		2

/* rect style */
#define RSI_KEYBOARD_MENU_FRM			RSI_PBACK
#define RSI_MBOX_KEYBOARD_SH			RSI_KEYBOARD_ITEM_SH
#define RSI_MBOX_KEYBOARD_HL			RSI_KEYBOARD_ITEM_HL
#define RSI_MBOX_KEYBOARD				RSI_IGNORE
#define RSI_EDIT_KEYBOARD				RSI_WINDOW_3
#define RSI_TEXT_KEYBOARD_SH			RSI_ITEM_1_SH
#define RSI_TEXT_KEYBOARD_HL			RSI_ITEM_1_HL
#define RSI_KEYBOARD_PAGE				RSI_ITEM_1_SH

/* font style */
#define FSI_TXT							FSI_BLACK
#define FSI_MBOX_KEYBOARD_TEXT_HL	FSI_BLACK
#define FSI_MBOX_KEYBOARD_TEXT_SH	FSI_BLACK

/* others */
#define KEYBOARD_MBOX_TOL				(KEYBOARD_MBOX_COL*KEYBOARD_MBOX_ROW)
#define KEYBOARD_MBOX_COL				13
#define KEYBOARD_MBOX_ROW			4
#define KEYBOARD_TOTAL_PAGE			2
#define KEYBOARD_START_PAGE			1
#define KEYBOARD_PAGE_STEP			1
#define KEYBOARD_PAGE_BLEN			1
/*!
Interface for open a calendar menu
*/
s32 open_keyboard_menu(u32 content, u32 ctrl);

#endif


