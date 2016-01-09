/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NEWS_H__
#define __UI_NEWS_H__

//icon text
#define UI_NEWS_CTRL_TEXT_H_GAP 10
#define UI_NEWS_CTRL_TEXT_H  20
//icon logo
#define UI_NEWS_LOGO_CTRL_X 600
#define UI_NEWS_LOGO_CTRL_Y 100
#define UI_NEWS_LOGO_CTRL_W 60
#define UI_NEWS_LOGO_CTRL_H 40

//icon box 
#define UI_NEWS_CTRL_X 20//0
#define UI_NEWS_CTRL_Y 210//140
#define UI_NEWS_CTRL_W (SCREEN_WIDTH - 2 * UI_NEWS_CTRL_X - 20)
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


