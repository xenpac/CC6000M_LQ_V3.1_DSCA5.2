/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NETWORK_CONFIG_H__
#define __UI_NETWORK_CONFIG_H__

//weather  area cont
#define NETWORK_ITEM_X         120
#define NETWORK_ITEM_Y         60//100
#define NETWORK_ITEM_W         800
#define NETWORK_ITEM_LW         360
#define NETWORK_ITEM_RW         NETWORK_ITEM_W-NETWORK_ITEM_LW
#define NETWORK_ITEM_H        35
#define NETWORK_ITEM_VGAP 8

//connect status text
#define NETWORK_CONNECT_STATUS_TEXTX NETWORK_ITEM_X+NETWORK_ITEM_LW+100
#define NETWORK_CONNECT_STATUS_TEXTY 500
#define NETWORK_CONNECT_STATUS_TEXTW 100
#define NETWORK_CONNECT_STATUS_TEXTH NETWORK_ITEM_H

//connect status
#define NETWORK_CONNECT_STATUSX NETWORK_CONNECT_STATUS_TEXTX+NETWORK_CONNECT_STATUS_TEXTW
#define NETWORK_CONNECT_STATUSY NETWORK_CONNECT_STATUS_TEXTY
#define NETWORK_CONNECT_STATUSW NETWORK_CONNECT_STATUS_TEXTW + 50
#define NETWORK_CONNECT_STATUSH NETWORK_CONNECT_STATUS_TEXTH


#define IP_CTRL_CNT 10//9
#define CONFIG_MODE_TYPE_CNT 3

RET_CODE open_network_config(u32 para1, u32 para2);
RET_CODE open_pppoe_connect(u32 para1, u32 para2);


#endif
