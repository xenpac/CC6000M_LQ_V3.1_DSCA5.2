/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_WEATHER_H__
#define __UI_WEATHER_H__

#define MAX_WEATHER_PIC             49
//weather  area cont
#define WEATHER_AREA_LIST_X         120
#define WEATHER_AREA_LIST_Y         100
#define WEATHER_AREA_LIST_W         200//200
#define WEATHER_AREA_LIST_H         510

#define WEATHER_AREA_LIST_ITEM_H        COMM_ITEM_H
#define WEATHER_AREA_LIST_ITEM_VGAP     7

#define WEATHER_AREA_LIST_MID_L     9
#define WEATHER_AREA_LIST_MID_T     10
#define WEATHER_AREA_LIST_MID_W    220//(WEATHER_AREA_LIST_W - 2 * WEATHER_AREA_LIST_MID_L)
#define WEATHER_AREA_LIST_MID_H    ((COMM_ITEM_H + WEATHER_AREA_LIST_ITEM_VGAP) * AREA_LIST_PAGE - WEATHER_AREA_LIST_ITEM_VGAP)

//weather detail container
#define WEATHER_DETAIL_CONT_X      WEATHER_AREA_LIST_X + WEATHER_AREA_LIST_W + 40  
#define WEATHER_DETAIL_CONT_Y       WEATHER_AREA_LIST_Y 
#define WEATHER_DETAIL_CONT_W        800//(1160-WEATHER_DETAIL_CONT_X)
#define WEATHER_DETAIL_CONT_H        WEATHER_AREA_LIST_H

//weather city detail info
#define WEATHER_CITY_DETAIL_X      20
#define WEATHER_CITY_DETAIL_Y      20
#define WEATHER_CITY_DETAIL_W        790
#define WEATHER_CITY_DETAIL_H        50

//weather city detail info
#define WEATHER_TEMP_DETAIL_X       5
#define WEATHER_TEMP_DETAIL_Y       70
#define WEATHER_TEMP_DETAIL_W       200
#define WEATHER_TEMP_DETAIL_H       100

//weather detail info
#define WEATHER_DETAIL_INFO_ITEM_X      450//760
#define WEATHER_DETAIL_INFO_ITEM_Y       70 
#define WEATHER_DETAIL_INFO_ITEM_W       150
#define WEATHER_DETAIL_INFO_ITEM_H        30//COMM_CTRL_H
#define WEATHER_DETAIL_INFO_ITEM_VGAP        0
#define WEATHER_DETAIL_INFO_ITEM_HGAP        0


//bottom help container
#define WEATHER_BOTTOM_HELP_X  120
#define WEATHER_BOTTOM_HELP_Y  620
#define WEATHER_BOTTOM_HELP_W  1040
#define WEATHER_BOTTOM_HELP_H  50
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
#define AREA_LIST_PAGE    9
#define AREA_LIST_FIELD   2
#define RSI_WEATHER_AREA_LIST RSI_PBACK

RET_CODE open_weather(u32 para1, u32 para2);

RET_CODE close_weather(void);

#endif
