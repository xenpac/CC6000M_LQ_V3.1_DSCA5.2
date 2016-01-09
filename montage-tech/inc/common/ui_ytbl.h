/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_YOUTUBE_LIST_H__
#define __UI_YOUTUBE_LIST_H__

//prview size
#define YTBL_PRVIEW_W 320//480 //120 //320
#define YTBL_PRVIEW_H 180// 360 //90  //180
//cont
#define YTBL_CONT_X  0
#define YTBL_CONT_Y  0
#define YTBL_CONT_W  SCREEN_WIDTH //960
#define YTBL_CONT_H  SCREEN_HEIGHT //600

//page
#define YTBL_TTL_X 300
#define YTBL_TTL_Y 30
#define YTBL_TTL_W 500
#define YTBL_TTL_H 30

#define YTBL_TTL_NAME_X 100
#define YTBL_TTL_NAME_Y 0
#define YTBL_TTL_NAME_W 100
#define YTBL_TTL_NAME_H 30

#define  YTBL_TTL_PAGE_X 240
#define  YTBL_TTL_PAGE_Y 0
#define  YTBL_TTL_PAGE_W 100
#define  YTBL_TTL_PAGE_H 30
//BMP
#define YTBL_TTL_BMP_X  50
#define YTBL_TTL_BMP_Y  30 
#define YTBL_TTL_BMP_W 110
#define YTBL_TTL_BMP_H 50
//list
#define YTBL_LIST_X YTBL_HGAP + 5//15
#define YTBL_LIST_Y (YTBL_TTL_Y + YTBL_TTL_H + 40)//110
#define YTBL_LIST_W 200
#define YTBL_LIST_H  440

#define YTBL_LIST_ML 0
#define YTBL_LIST_MT 0
#define YTBL_LIST_MR (YTBL_LIST_W - 2 * YTBL_LIST_ML) //200
#define YTBL_LIST_MB YTBL_LIST_H 

#define YTBL_LIST_CNT   9
#define YTBL_LIST_PAGE  9
#define YTBL_LIST_FIELD_CNT 1

//item container
#define YTBL_ICONT_X  (YTBL_LIST_X + YTBL_LIST_W + YTBL_HGAP) //210
#define YTBL_ICONT_Y  75
#define YTBL_ICONT_W  (YTBL_CONT_W - YTBL_LIST_W - 2 * YTBL_LIST_X - 2*YTBL_HGAP)//745
#define YTBL_ICONT_H  (YTBL_ITEM_H * YTBL_ITEM_PAGE + (YTBL_ITEM_PAGE - 1) * YTBL_VGAP)//445

//item
#define YTBL_ITEM_X 0
#define YTBL_ITEM_Y 0
#define YTBL_ITEM_W YTBL_ICONT_W //745
#define YTBL_ITEM_H 109
//item view
#define YTBL_ITEM_PREWX 0
#define YTBL_ITEM_PREWY 0
#define YTBL_ITEM_PREWW 150
#define YTBL_ITEM_PREWH YTBL_ITEM_H //145
//item cont
#define YTBL_ITEM_CONTX (YTBL_ITEM_PREWX + YTBL_ITEM_PREWW + YTBL_HGAP) //155
#define YTBL_ITEM_CONTY 0
#define YTBL_ITEM_CONTW (YTBL_ITEM_W - YTBL_ITEM_CONTX) //590
#define YTBL_ITEM_CONTH YTBL_ITEM_H

#define YTBL_ITEM_LX  5
#define YTBL_ITEM_LY  5
#define YTBL_ITEM_LW  240
#define YTBL_ITEM_LH  49

#define YTBL_ITEM_RX  (YTBL_ITEM_LX + YTBL_ITEM_LW)//245
#define YTBL_ITEM_RY  0
#define YTBL_ITEM_RW  (YTBL_ITEM_CONTW - YTBL_ITEM_RX - 10)
#define YTBL_ITEM_RH  36

//other list coordinates.
#define YTBL_OLIST_X  (YTBL_LIST_X + YTBL_LIST_W + YTBL_HGAP)
#define YTBL_OLIST_Y   (YTBL_TTL_Y + YTBL_TTL_H + 15)
#define YTBL_OLIST_W  (YTBL_CONT_W - YTBL_LIST_W - 4 * YTBL_LIST_X )
#define YTBL_OLIST_H  (YTBL_ITEM_H * YTBL_ITEM_PAGE + (YTBL_ITEM_PAGE - 1) * YTBL_VGAP)

#define YTBL_OLIST_CNT   5
#define YTBL_OLIST_PAGE  4
#define YTBL_OLIST_FIELD_CNT 2

#define YTBL_VGAP  10
#define YTBL_HGAP  10


#define YTBL_ITEM_PAGE 4

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

RET_CODE open_ytbl(u32 para1, u32 para2);
#endif

