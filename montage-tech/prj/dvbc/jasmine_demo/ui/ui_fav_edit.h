/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_FAV_EDIT_H__
#define __UI_FAV_EDIT_H__

/* coordinate */
#define FAV_EDIT_MENU_X           ((COMM_BG_W - FAV_EDIT_MENU_W)/2)
#define FAV_EDIT_MENU_Y           ((COMM_BG_H - FAV_EDIT_MENU_H)/2)
#define FAV_EDIT_MENU_W           COMM_BG_W
#define FAV_EDIT_MENU_H           COMM_BG_H

//preview 
#define FAV_EDIT_PREV_X           PREV_COMMON_X
#define FAV_EDIT_PREV_Y           PREV_COMMON_Y
#define FAV_EDIT_PREV_W           PREV_COMMON_W
#define FAV_EDIT_PREV_H           PREV_COMMON_H

////////////////////////////////////////////////////////////////

//proglist title
#define FAV_EDIT_PROG_LIST_TITLE_X (FAV_EDIT_PREV_X + FAV_EDIT_PREV_W + 10)
#define FAV_EDIT_PROG_LIST_TITLE_Y FAV_EDIT_PREV_Y
#define FAV_EDIT_PROG_LIST_TITLE_W (FAV_EDIT_MENU_W - FAV_EDIT_PROG_LIST_TITLE_X - WIN_LR_SPACE)
#define FAV_EDIT_PROG_LIST_TITLE_H COMM_ITEM_H//36

//prog list
#define FAV_EDIT_PROG_LIST_X       FAV_EDIT_PROG_LIST_TITLE_X
#define FAV_EDIT_PROG_LIST_Y       (FAV_EDIT_PROG_LIST_TITLE_Y + FAV_EDIT_PROG_LIST_TITLE_H + 6)
#define FAV_EDIT_PROG_LIST_W       (FAV_EDIT_PROG_LIST_TITLE_W - 10)
#define FAV_EDIT_PROG_LIST_H       370//480

#define FAV_EDIT_PROG_LIST_MIDL         5//10
#define FAV_EDIT_PROG_LIST_MIDT         5//10
#define FAV_EDIT_PROG_LIST_MIDW         (FAV_EDIT_PROG_LIST_W-2*FAV_EDIT_PROG_LIST_MIDL)
#define FAV_EDIT_PROG_LIST_MIDH         (FAV_EDIT_PROG_LIST_H-2*FAV_EDIT_PROG_LIST_MIDT)
#define FAV_EDIT_PROG_LIST_VGAP         2

//prog bar
#define FAV_EDIT_PROG_BAR_X        (FAV_EDIT_PROG_LIST_X + FAV_EDIT_PROG_LIST_W + 4)
#define FAV_EDIT_PROG_BAR_Y        FAV_EDIT_PROG_LIST_Y
#define FAV_EDIT_PROG_BAR_W        6
#define FAV_EDIT_PROG_BAR_H        FAV_EDIT_PROG_LIST_H

#define FAV_EDIT_PROG_LIST_PAGE         9//13
#define FAV_EDIT_PROG_LIST_FIELD        5

////////////////////////////////////////////////////////////////////////////////////////////

//fav pg list title
#define FAV_EDIT_FAV_LIST_TITLE_X  FAV_EDIT_PREV_X
#define FAV_EDIT_FAV_LIST_TITLE_Y  (FAV_EDIT_PREV_Y)
#define FAV_EDIT_FAV_LIST_TITLE_W  FAV_EDIT_PREV_W
#define FAV_EDIT_FAV_LIST_TITLE_H  FAV_EDIT_PROG_LIST_TITLE_H

//fav pg list
#define FAV_EDIT_FAV_LIST_X        FAV_EDIT_FAV_LIST_TITLE_X
#define FAV_EDIT_FAV_LIST_Y        (FAV_EDIT_FAV_LIST_TITLE_Y + FAV_EDIT_FAV_LIST_TITLE_H + 6)
#define FAV_EDIT_FAV_LIST_W        (FAV_EDIT_FAV_LIST_TITLE_W - 10)
#define FAV_EDIT_FAV_LIST_H        370//150

//fav pg bar
#define FAV_EDIT_FAV_BAR_X        (FAV_EDIT_FAV_LIST_X + FAV_EDIT_FAV_LIST_W + 4)
#define FAV_EDIT_FAV_BAR_Y        FAV_EDIT_FAV_LIST_Y
#define FAV_EDIT_FAV_BAR_W        6
#define FAV_EDIT_FAV_BAR_H        FAV_EDIT_FAV_LIST_H

#define FAV_EDIT_LIST_MIDL         5
#define FAV_EDIT_LIST_MIDT         5
#define FAV_EDIT_LIST_MIDW         (FAV_EDIT_FAV_LIST_W-2*FAV_EDIT_LIST_MIDL)
#define FAV_EDIT_LIST_MIDH         (FAV_EDIT_FAV_LIST_H-2*FAV_EDIT_LIST_MIDT)
#define FAV_EDIT_LIST_VGAP         0

#define FAV_EDIT_LIST_PAGE         9//4
#define FAV_EDIT_LIST_FIELD        5

//////////////////////////////////////////////////////////////////////////////////////

//favgroup container
#define FAV_EDIT_FAVGROUP_CONT_X  FAV_EDIT_FAV_LIST_TITLE_X//(FAV_EDIT_MENU_W-FAV_EDIT_FAVGROUP_CONT_W)/2
#define FAV_EDIT_FAVGROUP_CONT_Y  FAV_EDIT_FAV_LIST_TITLE_Y//(FAV_EDIT_MENU_H - 240)/2
#define FAV_EDIT_FAVGROUP_CONT_W  270
#define FAV_EDIT_FAVGROUP_CONT_H  260//300

//favgroup list bg
#define FAV_EDIT_FAVGROUP_LIST_BG_X     (FAV_EDIT_FAVGROUP_LIST_TITLE_X-5)
#define FAV_EDIT_FAVGROUP_LIST_BG_Y     (FAV_EDIT_FAVGROUP_LIST_TITLE_Y-5)
#define FAV_EDIT_FAVGROUP_LIST_BG_W     (FAV_EDIT_FAVGROUP_LIST_TITLE_W+2*5)
#define FAV_EDIT_FAVGROUP_LIST_BG_H     (FAV_EDIT_FAVGROUP_LIST_Y+FAV_EDIT_FAVGROUP_LIST_H+10-FAV_EDIT_FAVGROUP_LIST_BG_Y)

//favgroup list title
#define FAV_EDIT_FAVGROUP_LIST_TITLE_X  ((FAV_EDIT_FAVGROUP_CONT_W-FAV_EDIT_FAVGROUP_LIST_TITLE_W)/2)
#define FAV_EDIT_FAVGROUP_LIST_TITLE_Y  6
#define FAV_EDIT_FAVGROUP_LIST_TITLE_W  250
#define FAV_EDIT_FAVGROUP_LIST_TITLE_H  COMM_ITEM_H//36

//favgroup list
#define FAV_EDIT_FAVGROUP_LIST_X        FAV_EDIT_FAVGROUP_LIST_TITLE_X
#define FAV_EDIT_FAVGROUP_LIST_Y        (FAV_EDIT_FAVGROUP_LIST_TITLE_Y+FAV_EDIT_FAVGROUP_LIST_TITLE_H+4)
#define FAV_EDIT_FAVGROUP_LIST_W        FAV_EDIT_FAVGROUP_LIST_TITLE_W
#define FAV_EDIT_FAVGROUP_LIST_H        206//190

#define FAV_EDIT_FAVGROUP_LIST_MIDL         3
#define FAV_EDIT_FAVGROUP_LIST_MIDT         3
#define FAV_EDIT_FAVGROUP_LIST_MIDW         (FAV_EDIT_FAVGROUP_LIST_W-2*FAV_EDIT_FAVGROUP_LIST_MIDL)
#define FAV_EDIT_FAVGROUP_LIST_MIDH         (FAV_EDIT_FAVGROUP_LIST_H-FAV_EDIT_FAVGROUP_LIST_MIDT)
#define FAV_EDIT_FAVGROUP_LIST_VGAP         4

#define FAV_EDIT_FAVGROUP_LIST_FIELD        1
#define FAV_EDIT_FAVGROUP_LIST_PAGE         5

#define RSI_FAV_EDIT_SBAR          RSI_SCROLL_BAR_BG
#define RSI_FAV_EDIT_SBAR_MID      RSI_SCROLL_BAR_MID

//rstyle
#define RSI_FAV_EDIT_CONT         RSI_IGNORE
//fstyle
#define FSI_FAV_EDIT_NUMB          FSI_GREEN

RET_CODE open_fav_edit(u32 para1, u32 para2);

RET_CODE preopen_fav_edit(u32 para1, u32 para2);

#endif


