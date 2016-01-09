/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_WIFI_H__
#define __UI_WIFI_H__
#include "ethernet.h"

/* coordinate */
//container
#define WIFI_CONT_X       0// ((SCREEN_WIDTH - WIFI_CONT_W) / 2)
#define WIFI_CONT_Y       0// ((SCREEN_HEIGHT- WIFI_CONT_H) / 2) + 35
#define WIFI_CONT_W       1280 //1040
#define WIFI_CONT_H       720// 605//580//(2 * TIMER_TITLE_Y + TIMER_TITLE_H + TIMER_ITEM_CNT * (TIMER_ITEM_H + TIMER_ITEM_V_GAP) + TIMER_BTN_H + 2*TIMER_ITEM_V_GAP)

//title
#define WIFI_TITLE_X 4
#define WIFI_TITLE_Y 20
#define WIFI_TITLE_W (WIFI_CONT_W - 2 * WIFI_TITLE_X)
#define WIFI_TITLE_H 45

//connect status container
#define WIFI_STATUS_CONT_X 30
#define WIFI_STATUS_CONT_Y 2*WIFI_TITLE_Y + WIFI_TITLE_H
#define WIFI_STATUS_CONT_W WIFI_CONT_W - 2*WIFI_STATUS_CONT_X
#define WIFI_STATUS_CONT_H 50

//connect status left bmp
#define WIFI_STATUSL_X       44
#define WIFI_STATUSL_Y       7
#define WIFI_STATUSL_W      36
#define WIFI_STATUSL_H       36

//connect status right bmp
#define WIFI_STATUSR_X       WIFI_STATUS_CONT_W - WIFI_STATUSL_X - WIFI_STATUSL_W
#define WIFI_STATUSR_Y       WIFI_STATUSL_Y
#define WIFI_STATUSR_W      WIFI_STATUSL_W
#define WIFI_STATUSR_H       WIFI_STATUSL_H

//connect status 
#define WIFI_STATUS_X       WIFI_STATUSL_X + WIFI_STATUSL_W
#define WIFI_STATUS_Y       0
#define WIFI_STATUS_W      WIFI_STATUS_CONT_W - 2*WIFI_STATUS_X -100
#define WIFI_STATUS_H       WIFI_STATUS_CONT_H

//wifi list
#define WIFI_LIST_X  26
#define WIFI_LIST_Y  WIFI_STATUS_CONT_Y + WIFI_STATUS_CONT_H + 15
#define WIFI_LIST_W  WIFI_STATUS_CONT_W - WIFI_LIST_BAR_W
#define WIFI_LIST_H  380

//wifi listbar
#define WIFI_LIST_BAR_X  WIFI_LIST_X + WIFI_LIST_W + WIFI_LIST_MIDL
#define WIFI_LIST_BAR_Y  WIFI_LIST_Y + WIFI_LIST_MIDT
#define WIFI_LIST_BAR_W  6
#define WIFI_LIST_BAR_H  WIFI_LIST_H - 2*WIFI_LIST_MIDT

#define WIFI_LIST_MIDL  4
#define WIFI_LIST_MIDT  4
#define WIFI_LIST_MIDW  (WIFI_LIST_W - 2 * WIFI_LIST_MIDL)
#define WIFI_LIST_MIDH  (WIFI_LIST_H - 2 * WIFI_LIST_MIDT)

#define WIFI_LCONT_LIST_VGAP 4
/* rect style */

/* font style */
/* others */
#define WIFI_LIST_ITEM_NUM_ONE_PAGE  8
#define WIFI_LIST_FIELD 5

typedef enum
{
  WIFI_DEVICE_INVALID,
  WIFI_DEVICE_OFF,
  WIFI_DEVICE_ON_NOT_CONNECTED,
  WIFI_DEVICE_ON_CONNECTED,
  WIFI_DEVICE_ON_CONNECTING,
  
}wifi_status_t;

wifi_status_t get_wifi_connect_status(void);
ethernet_device_t* get_wifi_dev_handle(void);
void set_wifi_connect_status(wifi_status_t wifi_status);

RET_CODE ui_open_wifi(u32 para1, u32 para2);

#endif


