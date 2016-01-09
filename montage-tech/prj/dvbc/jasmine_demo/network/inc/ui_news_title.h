/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NEWS_TITLE_H__
#define __UI_NEWS_TITLE_H__

//prview size
#define NEWS_PRVIEW_W 320//480 //120 //320
#define NEWS_PRVIEW_H 180// 360 //90  //180

#define NEWS_OFFSET 60
//cont
#define NEWS_CONT_X  0//NEWS_OFFSET/2
#define NEWS_CONT_Y  0
#define NEWS_CONT_W  SCREEN_WIDTH //SCREEN_WIDTH - NEWS_OFFSET//960
#define NEWS_CONT_H  SCREEN_HEIGHT//600

//page
#define NEWS_TTL_X 300
#define NEWS_TTL_Y 35  //20
#define NEWS_TTL_W 500
#define NEWS_TTL_H 30

#define NEWS_TTL_NAME_X 100
#define NEWS_TTL_NAME_Y 0
#define NEWS_TTL_NAME_W 100
#define NEWS_TTL_NAME_H 30

#define  NEWS_TTL_PAGE_X 240
#define  NEWS_TTL_PAGE_Y 0
#define  NEWS_TTL_PAGE_W 100
#define  NEWS_TTL_PAGE_H 30
//BMP
#define NEWS_TTL_BMP_X  50
#define NEWS_TTL_BMP_Y  20 
#define NEWS_TTL_BMP_W 110
#define NEWS_TTL_BMP_H 50

//item container
#define NEWS_ICONT_X  (NEWS_HGAP) //210
#define NEWS_ICONT_Y  100//65
#define NEWS_ICONT_W  (NEWS_CONT_W - 4 * NEWS_ICONT_X)//745
#define NEWS_ICONT_H  (NEWS_ITEM_H * NEWS_ITEM_PAGE + (NEWS_ITEM_PAGE - 1) * NEWS_VGAP)//445

//item
#define NEWS_ITEM_X 0
#define NEWS_ITEM_Y 0
#define NEWS_ITEM_W NEWS_ICONT_W //745
#define NEWS_ITEM_H 109
//item view
#define NEWS_ITEM_PREWX 30
#define NEWS_ITEM_PREWY 0
#define NEWS_ITEM_PREWW 170
#define NEWS_ITEM_PREWH NEWS_ITEM_H //145
//item cont
#define NEWS_ITEM_TEXTX (NEWS_ITEM_PREWX + NEWS_ITEM_PREWW + NEWS_HGAP) //155
#define NEWS_ITEM_TEXTY 0
#define NEWS_ITEM_TEXTW (NEWS_ITEM_W - NEWS_ITEM_TEXTX - 60) //590
#define NEWS_ITEM_TEXTH NEWS_ITEM_H

#define NEWS_VGAP  10
#define NEWS_HGAP  10


#define NEWS_ITEM_PAGE 4

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


RET_CODE open_news_title(u32 para1, u32 para2);
void news_clear_bmp(control_t *p_menu);

#endif



