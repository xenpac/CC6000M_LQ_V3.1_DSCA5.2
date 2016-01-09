/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_NEWS_INFO_H__
#define __UI_NEWS_INFO_H__

#define ENLAGE_W NEWS_INFO_PREV_W

#define NEWS_INFO_MENU_X  0
#define NEWS_INFO_MENU_Y  0
#define NEWS_INFO_MENU_W SCREEN_WIDTH 
#define NEWS_INFO_MENU_H  (SCREEN_HEIGHT - 30)

#define NEWS_INFO_PAGE_CONT_X  20
#define NEWS_INFO_PAGE_CONT_Y  140
#define NEWS_INFO_PAGE_CONT_W  300
#define NEWS_INFO_PAGE_CONT_H  30
//#define NEWS_INFO_PAGE_CONT_H  130


#define NEWS_INFO_TITLE_X  20
#define NEWS_INFO_TITLE_Y  40
#define NEWS_INFO_TITLE_W  (NEWS_INFO_MENU_W - 40)
#define NEWS_INFO_TITLE_H  30

#define NEWS_INFO_MSG_X  10
#define NEWS_INFO_MSG_Y  140
#define NEWS_INFO_MSG_W  (NEWS_INFO_MENU_W - 20)
#define NEWS_INFO_MSG_H  30

#define NEWS_INFO_NAME_X 70
#define NEWS_INFO_NAME_Y 0
#define NEWS_INFO_NAME_W 100
#define NEWS_INFO_NAME_H 30

#define  NEWS_INFO_PAGE_X 170
#define  NEWS_INFO_PAGE_Y 0
#define  NEWS_INFO_PAGE_W 100
#define  NEWS_INFO_PAGE_H 30

#define NEWS_INFO_PREV_X  (NEWS_INFO_PAGE_CONT_X + 35)
#define NEWS_INFO_PREV_Y  (NEWS_INFO_PAGE_CONT_Y + 55)
#define NEWS_INFO_PREV_W   290
#define NEWS_INFO_PREV_H   220

#define NEWS_INFO_TEXT_X  (NEWS_INFO_PAGE_CONT_X * 2 + NEWS_INFO_PAGE_CONT_W+30)
#define NEWS_INFO_TEXT_Y  (NEWS_INFO_PREV_Y - 95)
//#define NEWS_INFO_TEXT_W  (NEWS_INFO_MENU_W - NEWS_INFO_TEXT_X - 34)
#define NEWS_INFO_TEXT_W  (NEWS_INFO_MENU_W - NEWS_INFO_TEXT_X - 150 + 30)
//#define NEWS_INFO_TEXT_H  405
#define NEWS_INFO_TEXT_H  450



#define NEWS_INFO_TEXT2_X  (NEWS_INFO_PAGE_CONT_X * 4 +NEWS_OFFSET)
#define NEWS_INFO_TEXT2_W  (NEWS_INFO_MENU_W - NEWS_INFO_TEXT_X - 34 + NEWS_INFO_PAGE_CONT_W - 155)//150)

#define NEWS_INFO_SBAR_X  (NEWS_INFO_TEXT_X+ NEWS_INFO_TEXT_W +4)
#define NEWS_INFO_SBAR_Y  NEWS_INFO_TEXT_Y
#define NEWS_INFO_SBAR_W  6
#define NEWS_INFO_SBAR_H  NEWS_INFO_TEXT_H



#define NEWS_INFO_GAP  10

RET_CODE open_news_info(u32 para1, u32 para2);

#endif
