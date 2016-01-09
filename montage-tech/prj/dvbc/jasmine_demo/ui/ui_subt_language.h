/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_SUBT_LANGUAGE_H__
#define __UI_SUBT_LANGUAGE_H__

/* coordinate */
#define SUBT_LANG_LIST_ITEM_V_GAP  4         
#define SUBT_LANG_LIST_ITEM_H_GAP  4


#define SUBT_LANG_LIST_MENU_CONT_X 50
#define SUBT_LANG_LIST_MENU_CONT_Y 50
#define SUBT_LANG_LIST_MENU_CONT_W 326
#define SUBT_LANG_LIST_MENU_CONT_H 330

#define SUBT_LANG_LIST_TITLE_X 20
#define SUBT_LANG_LIST_TITLE_Y 10
#define SUBT_LANG_LIST_TITLE_W 280
#define SUBT_LANG_LIST_TITLE_H 40 

#define SUBT_LANG_LIST_LIST_X  20
#define SUBT_LANG_LIST_LIST_Y  (SUBT_LANG_LIST_TITLE_Y + SUBT_LANG_LIST_TITLE_H)
#define SUBT_LANG_LIST_LIST_W  280
#define SUBT_LANG_LIST_LIST_H  (SUBT_LANG_LIST_MENU_CONT_H-SUBT_LANG_LIST_TITLE_Y-SUBT_LANG_LIST_TITLE_H-40)

#define SUBT_LANG_LIST_MID_L 4
#define SUBT_LANG_LIST_MID_T 4
#define SUBT_LANG_LIST_MID_W (SUBT_LANG_LIST_LIST_W - 2 * SUBT_LANG_LIST_MID_L)
#define SUBT_LANG_LIST_MID_H (SUBT_LANG_LIST_LIST_H - 2 * SUBT_LANG_LIST_MID_T)

#define SUBT_LANG_LIST_SBAR_X  (SUBT_LANG_LIST_LIST_X + SUBT_LANG_LIST_LIST_W)
#define SUBT_LANG_LIST_SBAR_Y  (SUBT_LANG_LIST_LIST_Y + SUBT_LANG_LIST_MID_T)
#define SUBT_LANG_LIST_SBAR_W  6
#define SUBT_LANG_LIST_SBAR_H SUBT_LANG_LIST_MID_H

#define SUBT_LANG_LIST_SBAR_VERTEX_GAP         12

/* rect style */

/* font style */

/* others */
#define SUBT_LANG_FIELD 3
#define SUBT_LANG_LIST_PAGE 6



RET_CODE open_subt_language (u32 para1, u32 para2);
#endif

