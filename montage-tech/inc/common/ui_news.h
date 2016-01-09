/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NEWS_H__
#define __UI_NEWS_H__

//icon text
#define UI_NEWS_CTRL_TEXT_H_GAP 10
#define UI_NEWS_CTRL_TEXT_H  20
//icon logo
#define UI_NEWS_LOGO_CTRL_X 400
#define UI_NEWS_LOGO_CTRL_Y 20
#define UI_NEWS_LOGO_CTRL_W 80
#define UI_NEWS_LOGO_CTRL_H 100

//icon box 
#define UI_NEWS_CTRL_X 0
#define UI_NEWS_CTRL_Y 140
#define UI_NEWS_CTRL_W (COMM_BG_W - 2 * UI_NEWS_CTRL_X)
#define UI_NEWS_CTRL_H (250 + UI_NEWS_CTRL_TEXT_H + UI_NEWS_CTRL_TEXT_H_GAP)

#define NEWS_MBOX_L 20
#define NEWS_MBOX_T 0
#define NEWS_MBOX_R (UI_NEWS_CTRL_W - NEWS_MBOX_L)
#define NEWS_MBOX_B UI_NEWS_CTRL_H

//bottom help container
#define UI_NEWS_BOTTOM_HELP_X  200
#define UI_NEWS_BOTTOM_HELP_W  675

#define NEWS_ITEM 8

#define NEWS_COL 4

#define NEWS_ROW  2

#define NEWS_WINCOL 4

#define NEWS_WINROW  2

RET_CODE open_news(u32 para1, u32 para2);

#endif


