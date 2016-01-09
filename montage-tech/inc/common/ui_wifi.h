/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_WIFI_H__
#define __UI_WIFI_H__
#include "ethernet.h"

/* coordinate */
//container
/*!
  fix me
  */
#define WIFI_CONT_X        ((SCREEN_WIDTH - WIFI_CONT_W) / 2)
/*!
  fix me
  */
#define WIFI_CONT_Y        ((SCREEN_HEIGHT- WIFI_CONT_H) / 2)
/*!
  fix me
  */
#define WIFI_CONT_W        750
/*!
  fix me
  */
#define WIFI_CONT_H        (480)//(2 * TIMER_TITLE_Y + TIMER_TITLE_H + TIMER_ITEM_CNT * (TIMER_ITEM_H + TIMER_ITEM_V_GAP) + TIMER_BTN_H + 2*TIMER_ITEM_V_GAP)

//title
/*!
  fix me
  */
#define WIFI_TITLE_X 4
/*!
  fix me
  */
#define WIFI_TITLE_Y 15
/*!
  fix me
  */
#define WIFI_TITLE_W (WIFI_CONT_W - 2 * WIFI_TITLE_X)
/*!
  fix me
  */
#define WIFI_TITLE_H 30

//connect status container
/*!
  fix me
  */
#define WIFI_STATUS_CONT_X 30
/*!
  fix me
  */
#define WIFI_STATUS_CONT_Y 2*WIFI_TITLE_Y + WIFI_TITLE_H //60
/*!
  fix me
  */
#define WIFI_STATUS_CONT_W WIFI_CONT_W - 2*WIFI_STATUS_CONT_X //690
/*!
  fix me
  */
#define WIFI_STATUS_CONT_H 36

//connect status left bmp
/*!
  fix me
  */
#define WIFI_STATUSL_X       44
/*!
  fix me
  */
#define WIFI_STATUSL_Y       0
/*!
  fix me
  */
#define WIFI_STATUSL_W      36
/*!
  fix me
  */
#define WIFI_STATUSL_H       36

//connect status right bmp
/*!
  fix me
  */
#define WIFI_STATUSR_X       WIFI_STATUS_CONT_W - WIFI_STATUSL_X - WIFI_STATUSL_W
/*!
  fix me
  */
#define WIFI_STATUSR_Y       WIFI_STATUSL_Y
/*!
  fix me
  */
#define WIFI_STATUSR_W      WIFI_STATUSL_W
/*!
  fix me
  */
#define WIFI_STATUSR_H       WIFI_STATUSL_H

//connect status 
/*!
  fix me
  */
#define WIFI_STATUS_X       WIFI_STATUSL_X + WIFI_STATUSL_W //110
/*!
  fix me
  */
#define WIFI_STATUS_Y       0
/*!
  fix me
  */
#define WIFI_STATUS_W      WIFI_STATUS_CONT_W - 2*WIFI_STATUS_X -200
/*!
  fix me
  */
#define WIFI_STATUS_H       WIFI_STATUS_CONT_H

//wifi list
/*!
  fix me
  */
#define WIFI_LIST_X  26
/*!
  fix me
  */
#define WIFI_LIST_Y  WIFI_STATUS_CONT_Y + WIFI_STATUS_CONT_H //111
/*!
  fix me
  */
#define WIFI_LIST_W  WIFI_STATUS_CONT_W - WIFI_LIST_BAR_W //684
/*!
  fix me
  */
#define WIFI_LIST_H  320

//wifi listbar
/*!
  fix me
  */
#define WIFI_LIST_BAR_X  (WIFI_LIST_X + WIFI_LIST_W + WIFI_LIST_MIDL) //714
/*!
  fix me
  */
#define WIFI_LIST_BAR_Y  (WIFI_LIST_Y + WIFI_LIST_MIDT) //115
/*!
  fix me
  */
#define WIFI_LIST_BAR_W  6
/*!
  fix me
  */
#define WIFI_LIST_BAR_H  (WIFI_LIST_H - 2*WIFI_LIST_MIDT - 40)//372

/*!
  fix me
  */
#define WIFI_LIST_MIDL  4
/*!
  fix me
  */
#define WIFI_LIST_MIDT  15
/*!
  fix me
  */
#define WIFI_LIST_MIDW  (WIFI_LIST_W - 2 * WIFI_LIST_MIDL)//676
/*!
  fix me
  */
#define WIFI_LIST_MIDH  (36*6 + 5 * WIFI_LCONT_LIST_VGAP)//372

/*!
  fix me
  */
#define WIFI_LCONT_LIST_VGAP 5
/* rect style */

/* font style */
/* others */
/*!
  fix me
  */
#define WIFI_LIST_ITEM_NUM_ONE_PAGE  6
/*!
  fix me
  */
#define WIFI_LIST_FIELD 5


/*!
  fix me
  */
typedef enum
{
/*!
  fix me
  */
  WIFI_DEVICE_INVALID,
/*!
  fix me
  */
  WIFI_DEVICE_OFF,
/*!
  fix me
  */
  WIFI_DEVICE_ON_NOT_CONNECTED,
/*!
  fix me
  */
  WIFI_DEVICE_ON_CONNECTED,
/*!
  fix me
  */
  WIFI_DEVICE_ON_CONNECTING,
  
}wifi_status_t;

wifi_status_t get_wifi_connect_status();
/*!
  fix me
  */
ethernet_device_t* get_wifi_dev_handle();
/*!
  fix me
  */
void set_wifi_connect_status(wifi_status_t wifi_status);

/*!
  fix me
  */
RET_CODE ui_open_wifi(u32 para1, u32 para2);

#endif


