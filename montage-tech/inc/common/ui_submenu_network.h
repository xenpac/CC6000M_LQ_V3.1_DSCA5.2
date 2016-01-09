/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_SUMENU_NETWORK_H__
#define __UI_SUMENU_NETWORK_H__

//icon text
#define UI_SUBMENU_NET_CTRL_TEXT_H_GAP 10
#define UI_SUBMENU_NET_CTRL_TEXT_H  40
//icon box 
#define UI_SUBMENU_NET_CTRL_X 80
#define UI_SUBMENU_NET_CTRL_Y 140
#define UI_SUBMENU_NET_CTRL_W (COMM_BG_W - 2 * UI_SUBMENU_NET_CTRL_X)
#define UI_SUBMENU_NET_CTRL_H (190 + UI_SUBMENU_NET_CTRL_TEXT_H + UI_SUBMENU_NET_CTRL_TEXT_H_GAP)

#define NET_MBOX_L 20
#define NET_MBOX_T 0
#define NET_MBOX_R (UI_SUBMENU_NET_CTRL_W - NET_MBOX_L)
#define NET_MBOX_B UI_SUBMENU_NET_CTRL_H

//bottom help container
#define UI_NETWORK_BOTTOM_HELP_X  200
#define UI_NETWORK_BOTTOM_HELP_W  680

#define NETWORK_SUBMENU_ITEM 6

#define NETWORK_SUBMENU_COL 6

#define NETWORK_SUBMENU_ROW  1

#define NETWORK_SUBMENU_WINCOL 4

#define NETWORK_SUBMENU_WINROW  1

RET_CODE open_submenu_network(u32 para1, u32 para2);

#endif


