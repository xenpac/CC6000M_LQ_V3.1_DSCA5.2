/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_WEATHER_H__
#define __UI_WEATHER_H__

#define MAX_WEATHER_PIC             49
//weather  area cont
#define WEATHER_AREA_LIST_X         40
#define WEATHER_AREA_LIST_Y         65
#define WEATHER_AREA_LIST_W         220
#define WEATHER_AREA_LIST_H         450

#define WEATHER_AREA_LIST_ITEM_H        COMM_ITEM_H
#define WEATHER_AREA_LIST_ITEM_VGAP   8

#define WEATHER_AREA_LIST_MID_L     10
#define WEATHER_AREA_LIST_MID_T     10
#define WEATHER_AREA_LIST_MID_W     (WEATHER_AREA_LIST_W - WEATHER_AREA_LIST_MID_L)
#define WEATHER_AREA_LIST_MID_H     448

//weather detail container
#define WEATHER_DETAIL_GAP  10
#define WEATHER_DETAIL_RIGHT_GAP  30

#define WEATHER_DETAIL_CONT_X      (WEATHER_AREA_LIST_X + WEATHER_AREA_LIST_W + WEATHER_DETAIL_GAP) //270
#define WEATHER_DETAIL_CONT_Y       WEATHER_AREA_LIST_Y 
#define WEATHER_DETAIL_CONT_W      (COMM_BG_W - WEATHER_DETAIL_CONT_X - WEATHER_DETAIL_RIGHT_GAP)//660
#define WEATHER_DETAIL_CONT_H        WEATHER_AREA_LIST_H

//weather city detail info
#define WEATHER_CITY_DETAIL_X      5
#define WEATHER_CITY_DETAIL_Y      10
#define WEATHER_CITY_DETAIL_W      (WEATHER_DETAIL_CONT_W - WEATHER_CITY_DETAIL_X)//655
#define WEATHER_CITY_DETAIL_H      32

//weather city detail info
#define WEATHER_TEMP_DETAIL_X       5
#define WEATHER_TEMP_DETAIL_Y       50
#define WEATHER_TEMP_DETAIL_W       180//(150 - WEATHER_TEMP_DETAIL_X)
#define WEATHER_TEMP_DETAIL_H       160

#define WEATHER_DETAIL_PIC_X       (WEATHER_TEMP_DETAIL_X + WEATHER_TEMP_DETAIL_W)//185
#define WEATHER_DETAIL_PIC_Y       WEATHER_TEMP_DETAIL_Y //50
#define WEATHER_DETAIL_PIC_W       WEATHER_TEMP_DETAIL_W//180
#define WEATHER_DETAIL_PIC_H       WEATHER_TEMP_DETAIL_H  //160

//weather detail info
#define WEATHER_DETAIL_INFO_ITEM_X      (WEATHER_DETAIL_PIC_W + WEATHER_DETAIL_PIC_X )//365
#define WEATHER_DETAIL_INFO_ITEM_Y       WEATHER_TEMP_DETAIL_Y 
#define WEATHER_DETAIL_INFO_ITEM_W       140
#define WEATHER_DETAIL_INFO_ITEM_H        30
#define WEATHER_DETAIL_INFO_ITEM_VGAP        0
#define WEATHER_DETAIL_INFO_ITEM_HGAP        0

#define WEATHER_DETAIL_WEEK_GAP   10
#define WEATHER_DETAIL_WEEK_X      WEATHER_TEMP_DETAIL_X
#define WEATHER_DETAIL_WEEK_Y      (WEATHER_DETAIL_PIC_Y + WEATHER_DETAIL_PIC_H + 4*WEATHER_DETAIL_WEEK_GAP) 
#define WEATHER_DETAIL_WEEK_W       120
#define WEATHER_DETAIL_WEEK_H       32

#define WEATHER_DETAIL_WEEK_PIC_Y      (WEATHER_DETAIL_WEEK_Y + WEATHER_DETAIL_WEEK_H * 2 + WEATHER_DETAIL_GAP) 
#define WEATHER_DETAIL_WEEK_PIC_H       74

#define WEATHER_DETAIL_WEEK_TEM_Y      (WEATHER_DETAIL_WEEK_PIC_Y + WEATHER_DETAIL_WEEK_PIC_H + WEATHER_DETAIL_GAP) 
#define WEATHER_DETAIL_WEEK_TEM_H       32

#define WEATHER_DETAIL_ITEM_VGAP 3

//bottom help container
#define WEATHER_BOTTOM_HELP_X  100
#define WEATHER_BOTTOM_HELP_W  800
//province
#define WEATHER_PROVINCE_X    0
#define WEATHER_PROVINCE_Y    0
#define WEATHER_PROVINCE_W    100
#define WEATHER_PROVINCE_H    40

//city
#define WEATHER_CITY_X        (WEATHER_PROVINCE_X + WEATHER_PROVINCE_W + 10)
#define WEATHER_CITY_Y        WEATHER_PROVINCE_Y
#define WEATHER_CITY_W        100
#define WEATHER_CITY_H        40

#define PROVINCE_CBOX_PAGE 6

#define WEATHER_MBOX_CNT  10
#define WEATHER_MBOX_COL  5
#define WEATHER_MBOX_ROW  2

/* rect style */

//Other
#define WEATHER_AREA_CNT 4
#define WEATHER_INFO_CNT 6

/* others */
#define AREA_LIST_PAGE    10
#define AREA_LIST_FIELD   2
#define RSI_WEATHER_AREA_LIST RSI_PBACK

//TODO:
#define RSI_ITEM_12_SH              RSI_ITEM_1_SH


#define RSI_BLACK_FRM RSI_WINDOW_1
  
#define RSI_POPUP_BG  RSI_COMMAN_BG

RET_CODE open_weather(u32 para1, u32 para2);

#endif
