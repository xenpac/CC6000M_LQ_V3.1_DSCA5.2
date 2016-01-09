/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef  __UI_TEXT_ENCODE_H__
#define  __UI_TEXT_ENCODE_H__

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define UI_TEXT_ENCODE_STR_LEN        30

#define UI_TEXT_ENCODE_LANG_LEN       3

#define UI_TEXT_ENCODE_LIST_ITEM_CNT  6
#define UI_TEXT_ENCODE_LIST_ITEM_H    36

#define UI_TEXT_ENCODE_TITLE_X 20
#define UI_TEXT_ENCODE_TITLE_Y 20
#define UI_TEXT_ENCODE_TITLE_W 294
#define UI_TEXT_ENCODE_TITLE_H 46

#define UI_TEXT_ENCODE_LIST_X UI_TEXT_ENCODE_TITLE_X
#define UI_TEXT_ENCODE_LIST_Y (UI_TEXT_ENCODE_TITLE_Y + UI_TEXT_ENCODE_TITLE_H + 10)
#define UI_TEXT_ENCODE_LIST_W (UI_TEXT_ENCODE_TITLE_W - 10)
#define UI_TEXT_ENCODE_LIST_H 248

#define UI_TEXT_ENCODE_LIST_MID_X 10
#define UI_TEXT_ENCODE_LIST_MID_Y 10
#define UI_TEXT_ENCODE_LIST_MID_W (UI_TEXT_ENCODE_LIST_W - 2 * UI_TEXT_ENCODE_LIST_MID_X)
#define UI_TEXT_ENCODE_LIST_MID_H (UI_TEXT_ENCODE_LIST_H - 2*UI_TEXT_ENCODE_LIST_MID_Y)
#define UI_TEXT_ENCODE_LIST_MID_G 0

#define UI_TEXT_ENCODE_SBAR_X (UI_TEXT_ENCODE_LIST_X + UI_TEXT_ENCODE_LIST_W + 4)
#define UI_TEXT_ENCODE_SBAR_Y UI_TEXT_ENCODE_LIST_Y
#define UI_TEXT_ENCODE_SBAR_W 6
#define UI_TEXT_ENCODE_SBAR_H UI_TEXT_ENCODE_LIST_H

#define UI_TEXT_ENCODE_SBAR_MID_X 0
#define UI_TEXT_ENCODE_SBAR_MID_Y 4
#define UI_TEXT_ENCODE_SBAR_MID_W (UI_TEXT_ENCODE_SBAR_W - 2 * UI_TEXT_ENCODE_SBAR_MID_X)
#define UI_TEXT_ENCODE_SBAR_MID_H (UI_TEXT_ENCODE_SBAR_H - 2 * UI_TEXT_ENCODE_SBAR_MID_Y)
#define UI_TEXT_ENCODE_SBAR_MID_G 12

#define UI_TEXT_ENCODE_LIST_ITEM_1_W 30
#define UI_TEXT_ENCODE_LIST_ITEM_1_X 10
#define UI_TEXT_ENCODE_LIST_ITEM_2_W 48
#define UI_TEXT_ENCODE_LIST_ITEM_2_X 40
#define UI_TEXT_ENCODE_LIST_ITEM_3_W 160
#define UI_TEXT_ENCODE_LIST_ITEM_3_X 88

#define UI_TEXT_ENCODE_MAIN_X (SCREEN_WIDTH / 8)
#define UI_TEXT_ENCODE_MAIN_Y 100
#define UI_TEXT_ENCODE_MAIN_W (UI_TEXT_ENCODE_SBAR_X + UI_TEXT_ENCODE_SBAR_W + 20)
#define UI_TEXT_ENCODE_MAIN_H (UI_TEXT_ENCODE_LIST_H + UI_TEXT_ENCODE_LIST_Y + 20)


RET_CODE ui_text_encode_open(u32 para1, u32 para2);

#endif
