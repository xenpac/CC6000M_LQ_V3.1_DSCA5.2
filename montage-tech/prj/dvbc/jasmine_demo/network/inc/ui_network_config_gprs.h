/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NETWORK_CONFIG_GPRS_H__
#define __UI_NETWORK_CONFIG_GPRS_H__

//weather  area cont
#define NETWORK_GPRS_ITEM_X         120
#define NETWORK_GPRS_ITEM_Y         90//100
#define NETWORK_GPRS_ITEM_W         1040
#define NETWORK_GPRS_ITEM_LW         430
#define NETWORK_GPRS_ITEM_RW         NETWORK_GPRS_ITEM_W-NETWORK_GPRS_ITEM_LW
#define NETWORK_GPRS_ITEM_H        50
#define NETWORK_GPRS_ITEM_VGAP 8

//connect status text
#define NETWORK_GPRS_CONNECT_STATUS_TEXTX NETWORK_GPRS_ITEM_X+NETWORK_GPRS_ITEM_LW+280
#define NETWORK_GPRS_CONNECT_STATUS_TEXTY 662
#define NETWORK_GPRS_CONNECT_STATUS_TEXTW 200
#define NETWORK_GPRS_CONNECT_STATUS_TEXTH NETWORK_ITEM_H

//connect status
#define NETWORK_GPRS_CONNECT_STATUSX NETWORK_GPRS_CONNECT_STATUS_TEXTX+NETWORK_GPRS_CONNECT_STATUS_TEXTW
#define NETWORK_GPRS_CONNECT_STATUSY NETWORK_GPRS_CONNECT_STATUS_TEXTY
#define NETWORK_GPRS_CONNECT_STATUSW NETWORK_GPRS_CONNECT_STATUS_TEXTW
#define NETWORK_GPRS_CONNECT_STATUSH NETWORK_GPRS_CONNECT_STATUS_TEXTH


#define IP_CTRL_CNT 10//9
#define GPRS_CTRL_CNT 9
#define CONFIG_MODE_TYPE_CNT 3

RET_CODE open_network_config_gprs(u32 para1, u32 para2);
RET_CODE open_pppoe_connect(u32 para1, u32 para2);

void paint_gprs_conn_status(BOOL is_conn);

#endif
