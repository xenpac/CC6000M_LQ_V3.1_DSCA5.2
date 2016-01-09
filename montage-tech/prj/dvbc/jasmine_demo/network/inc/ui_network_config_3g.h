/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NETWORK_3G_CONFIG_H__
#define __UI_NETWORK_3G_CONFIG_H__

//weather  area cont
#define NETWORK_3G_ITEM_X         120
#define NETWORK_3G_ITEM_Y         90//100
#define NETWORK_3G_ITEM_W         1040
#define NETWORK_3G_ITEM_LW         430
#define NETWORK_3G_ITEM_RW         NETWORK_3G_ITEM_W-NETWORK_3G_ITEM_LW
#define NETWORK_3G_ITEM_H        50
#define NETWORK_3G_ITEM_VGAP 8

//connect status text
#define NETWORK_3G_CONNECT_STATUS_TEXTX NETWORK_3G_ITEM_X+NETWORK_3G_ITEM_LW+280
#define NETWORK_3G_CONNECT_STATUS_TEXTY 662
#define NETWORK_3G_CONNECT_STATUS_TEXTW 200
#define NETWORK_3G_CONNECT_STATUS_TEXTH NETWORK_3G_ITEM_H

//connect status
#define NETWORK_3G_CONNECT_STATUSX NETWORK_3G_CONNECT_STATUS_TEXTX+NETWORK_3G_CONNECT_STATUS_TEXTW
#define NETWORK_3G_CONNECT_STATUSY NETWORK_3G_CONNECT_STATUS_TEXTY
#define NETWORK_3G_CONNECT_STATUSW NETWORK_3G_CONNECT_STATUS_TEXTW
#define NETWORK_3G_CONNECT_STATUSH NETWORK_3G_CONNECT_STATUS_TEXTH


#define G3_CTRL_CNT 10

RET_CODE open_network_config_3g(u32 para1, u32 para2);

void paint_3g_conn_status(BOOL is_conn);

#endif
