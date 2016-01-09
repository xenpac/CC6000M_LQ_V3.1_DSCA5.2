/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_EMAIL_MESS__
#define __UI_EMAIL_MESS__

/* coordinate */
#define EMAIL_MENU_X            0 
#define EMAIL_MENU_Y            0
#define EMAIL_MENU_W            COMM_BG_W 
#define EMAIL_MENU_H            COMM_BG_H 

//TILTLE
#define EMAIL_TITLE_X       ((EMAIL_MENU_W - EMAIL_TITLE_W) / 2)
#define EMAIL_TITLE_Y       120
#define EMAIL_TITLE_W      900
#define EMAIL_TITLE_H       470

//items
#define EMAIL_MBOX_X        EMAIL_TITLE_X
#define EMAIL_MBOX_Y        EMAIL_TITLE_Y
#define EMAIL_MBOX_W       EMAIL_TITLE_W
#define EMAIL_MBOX_H        COMM_ITEM_H


//plist
#define EMAIL_LIST_X       EMAIL_TITLE_X
#define EMAIL_LIST_Y       (EMAIL_MBOX_Y+EMAIL_MBOX_H+5)
#define EMAIL_LIST_W      EMAIL_TITLE_W
#define EMAIL_LIST_H        (EMAIL_TITLE_H-EMAIL_MBOX_H-10-50)

//plist rect
#define EMAIL_LIST_MIDL         10
#define EMAIL_LIST_MIDT         10
#define EMAIL_LIST_MIDW        (EMAIL_LIST_W - EMAIL_LIST_MIDL * 3)
#define EMAIL_LIST_MIDH         (EMAIL_LIST_H - EMAIL_LIST_MIDT * 2)
#define EMAIL_LIST_VGAP         10

//list scroll bar
#define EMAIL_SBAR_X       (EMAIL_LIST_X + EMAIL_LIST_W - EMAIL_LIST_VGAP - EMAIL_SBAR_W)
#define EMAIL_SBAR_Y       (EMAIL_LIST_Y + EMAIL_LIST_MIDT)
#define EMAIL_SBAR_W       6
#define EMAIL_SBAR_H       EMAIL_LIST_MIDH

//received head
#define EMAIL_RECEIVED_HEAD_X    EMAIL_LIST_X
#define EMAIL_RECEIVED_HEAD_Y   (EMAIL_LIST_Y + EMAIL_LIST_H - EMAIL_RECEIVED_HEAD_H)
#define EMAIL_RECEIVED_HEAD_W   120 
#define EMAIL_RECEIVED_HEAD_H   25

//received
#define EMAIL_RECEIVED_X        (EMAIL_RECEIVED_HEAD_X + EMAIL_RECEIVED_HEAD_W)
#define EMAIL_RECEIVED_Y        EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RECEIVED_W        200
#define EMAIL_RECEIVED_H        EMAIL_RECEIVED_HEAD_H

//reset head
#define EMAIL_RESET_HEAD_X      (EMAIL_RECEIVED_HEAD_X + EMAIL_RECEIVED_W + 80)
#define EMAIL_RESET_HEAD_Y      EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_HEAD_W      150
#define EMAIL_RESET_HEAD_H      EMAIL_RECEIVED_HEAD_H

//reset
#define EMAIL_RESET_X          (EMAIL_RESET_HEAD_X+EMAIL_RESET_HEAD_W)
#define EMAIL_RESET_Y          EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_W          150
#define EMAIL_RESET_H          EMAIL_RECEIVED_HEAD_H
#define EMAIL_LIST_PAGE         11
#define EMAIL_ITEM_OFFSET     5

//others

#define EMAIL_LIST_FIELD        4

#define EMAIL_ITME_COUNT      4

#define EMAIL_MAX_NUMB         50

RET_CODE open_email_mess(u32 para1, u32 para2);

#endif




