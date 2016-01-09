/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NETWORK_CONFIG_LAN_H__
#define __UI_NETWORK_CONFIG_LAN_H__

//weather  area cont
#define NETWORK_LAN_ITEM_X         120
#define NETWORK_LAN_ITEM_Y         90//100
#define NETWORK_LAN_ITEM_W         1040
#define NETWORK_LAN_ITEM_LW         430
#define NETWORK_LAN_ITEM_RW         NETWORK_LAN_ITEM_W-NETWORK_LAN_ITEM_LW
#define NETWORK_LAN_ITEM_H        50
#define NETWORK_LAN_ITEM_VGAP 8

//connect status text
#define NETWORK_LAN_CONNECT_STATUS_TEXTX NETWORK_LAN_ITEM_X+NETWORK_LAN_ITEM_LW+280
#define NETWORK_LAN_CONNECT_STATUS_TEXTY 662
#define NETWORK_LAN_CONNECT_STATUS_TEXTW 200
#define NETWORK_LAN_CONNECT_STATUS_TEXTH NETWORK_LAN_ITEM_H

//connect status
#define NETWORK_LAN_CONNECT_STATUSX NETWORK_LAN_CONNECT_STATUS_TEXTX+NETWORK_LAN_CONNECT_STATUS_TEXTW
#define NETWORK_LAN_CONNECT_STATUSY NETWORK_LAN_CONNECT_STATUS_TEXTY
#define NETWORK_LAN_CONNECT_STATUSW NETWORK_LAN_CONNECT_STATUS_TEXTW
#define NETWORK_LAN_CONNECT_STATUSH NETWORK_LAN_CONNECT_STATUS_TEXTH


#define IP_CTRL_LAN_CNT 9//10//9
#define CONFIG_MODE_TYPE_CNT 3

RET_CODE open_network_config_lan(u32 para1, u32 para2);
RET_CODE open_pppoe_connect(u32 para1, u32 para2);


#endif
