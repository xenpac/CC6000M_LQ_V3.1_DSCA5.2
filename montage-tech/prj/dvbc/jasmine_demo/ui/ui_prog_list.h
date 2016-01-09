/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PROG_LIST_H__
#define __UI_PROG_LIST_H__

#include "ui_common.h"

#define PLIST_MENU_X            COMM_ITEM_OX_IN_ROOT//60
#define PLIST_MENU_Y            (COMM_ITEM_OY_IN_ROOT - 20)
#define PLIST_MENU_W            (COMM_BG_W - PLIST_MENU_X)
#define PLIST_MENU_H            (COMM_BG_H - PLIST_MENU_Y)

#define PLIST_LIST_CONTX        0
#define PLIST_LIST_CONTY        0
#define PLIST_LIST_CONTW        450//400//502//552
#define PLIST_LIST_CONTH        490

//top line
#define PLIST_TOP_LINE_X  0
#define PLIST_TOP_LINE_Y  6
#define PLIST_TOP_LINE_W  PLIST_LIST_CONTW
#define PLIST_TOP_LINE_H  18

//bottom line
#define PLIST_BOTTOM_LINE_X  PLIST_TOP_LINE_X
#define PLIST_BOTTOM_LINE_Y  100
#define PLIST_BOTTOM_LINE_W  PLIST_TOP_LINE_W
#define PLIST_BOTTOM_LINE_H  18

//group container
//group
#define PLIST_CONT_GROUPX       0 //(PLIST_LIST_CONTW - PLIST_CONT_GROUPW) / 2
#define PLIST_CONT_GROUPY       10//20
#define PLIST_CONT_GROUPW       PLIST_LIST_CONTW
#define PLIST_CONT_GROUPH       36

#define PLIST_GROUP_ARROW_X       PLIST_LIST_CONTX //(PLIST_CONT_GROUPX - PLIST_GROUP_ARROWL_W - 10)
#define PLIST_GROUP_ARROW_Y       PLIST_CONT_GROUPY
#define PLIST_GROUP_ARROW_W       PLIST_LIST_CONTW
#define PLIST_GROUP_ARROW_H       24

//color mbox
#define PLIST_CONT_MBOXX        ((PLIST_LIST_CONTW - PLIST_CONT_MBOXW) / 2)
#define PLIST_CONT_MBOXY        45//60
#define PLIST_CONT_MBOXW        PLIST_LIST_CONTW//400//502//552
#define PLIST_CONT_MBOXH        44

//prog list
#define PLIST_LIST_X            0
#define PLIST_LIST_Y            56//120
#define PLIST_LIST_W            450//480//530
#define PLIST_LIST_H            390

#define PLIST_LIST_MIDL  10
#define PLIST_LIST_MIDT  10
#define PLIST_LIST_MIDW  (PLIST_LIST_W - 3 * PLIST_LIST_MIDL)
#define PLIST_LIST_MIDH (PLIST_LIST_H - 2 * PLIST_LIST_MIDT)

#define PLIST_LIST_VGAP         0

#define PLIST_MOVE_ICON_X          5// 510//560
#define PLIST_MOVE_ICON_Y          460
#define PLIST_MOVE_ICON_W           40
#define PLIST_MOVE_ICON_H           30

#define PLIST_MOVE_HELP_X       50
#define PLIST_MOVE_HELP_Y       PLIST_MOVE_ICON_Y
#define PLIST_MOVE_HELP_W       (PLIST_LIST_W - PLIST_MOVE_HELP_X -50)
#define PLIST_MOVE_HELP_H       30

#define PLIST_BAR_X             435//490//540
#define PLIST_BAR_Y             (PLIST_LIST_Y + PLIST_LIST_CONTY + PLIST_LIST_MIDT)
#define PLIST_BAR_W             6
#define PLIST_BAR_H             PLIST_LIST_MIDH

//preview
#define PLIST_PREV_X            500//415//520//570
#define PLIST_PREV_Y            10//30
#define PLIST_PREV_W            480//320 //270
#define PLIST_PREV_H            300//204

#define PLIST_BRIEF_ICON_X          510// 510//560
#define PLIST_BRIEF_ICON_Y          320// 315//260
#define PLIST_BRIEF_ICON_W           60
#define PLIST_BRIEF_ICON_H           60

#define PLIST_BRIEF_X           600
#define PLIST_BRIEF_Y           330
#define PLIST_BRIEF_W           350
#define PLIST_BRIEF_H           160

#define PLIST_BRIFE_ITEM_CNT     5

#define PLIST_BRIEF_ITEMAX       0
#define PLIST_BRIEF_ITEMAW       (PLIST_BRIEF_W - 2 * PLIST_BRIEF_ITEMAX)

#define PLIST_BRIEF_ITEMBX       0 //4
#define PLIST_BRIEF_ITEMBW       (PLIST_BRIEF_W - 2 * PLIST_BRIEF_ITEMBX)

#define PLIST_BRIEF_ITEMY       5//12
#define PLIST_BRIEF_ITEM_GAP    5//10
#define PLIST_BRIEF_ITEMH      32// 26

#define PLIST_ICON_X            0
#define PLIST_ICON_Y            0
#define PLIST_ICON_W            80
#define PLIST_ICON_H            80

#define PLIST_PWD_X             COMM_DLG_X
#define PLIST_PWD_Y             COMM_DLG_Y
#define PLIST_PWD_W             COMM_DLG_W
#define PLIST_PWD_H             COMM_DLG_H

/*rstyle*/
#define RSI_PLIST_MENU          RSI_PBACK
#define RSI_PLIST_LIST_CONT     RSI_WINDOW_2
#define RSI_PLIST_TITLE         RSI_ITEM_1_HL
#define RSI_PLIST_MBOX         RSI_PBACK
#define RSI_PLIST_LIST          RSI_WINDOW_2
#define RSI_PLIST_SBAR          RSI_SCROLL_BAR_BG
#define RSI_PLIST_SBAR_MID      RSI_SCROLL_BAR_MID
#define RSI_PLIST_PREV          RSI_TV
#define RSI_PLIST_BRIEF              RSI_PBACK
#define RSI_PLIST_BRIEF_ITEM    RSI_COMMAN_MID_BG//RSI_PBACK

/*fstyle*/
#define FSI_PLIST_MBOX          FSI_WHITE
#define FSI_PLIST_CBOX          FSI_PROGBAR_BOTTOM
#define FSI_PLIST_TITLE         FSI_WHITE
#define FSI_PLIST_BRIEF         FSI_WHITE_INFO//FSI_PROGLIST_BRIEF

/*others*/
#define PLIST_LIST_PAGE         10
#define PLIST_LIST_FIELD        6
#define PLIST_MBOX_TOL          5
#define PLIST_MBOX_COL          5
#define PLIST_MBOX_ROW          1
#define PLIST_MBOX_HGAP         6
#define PLIST_MBOX_VGAP         0
#define PLIST_CBOX_PAGE         4


RET_CODE open_prog_list(u32 para1, u32 para2);

void plist_set_modify_state(BOOL state);

#endif

