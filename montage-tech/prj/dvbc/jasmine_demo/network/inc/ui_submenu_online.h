/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_SUMENU_ONLINE_MOVIE_H__
#define __UI_SUMENU_ONLINE_MOVIE_H__

//icon text
#define UI_SUBMENU_NET_CTRL_TEXT_H_GAP 10
#define UI_SUBMENU_NET_CTRL_TEXT_H  40
//icon box 
#define UI_SUBMENU_WEB_CTRL_X 90//50
#define UI_SUBMENU_WEB_CTRL_Y 120//50//240//140
#define UI_SUBMENU_WEB_CTRL_W 1070//(COMM_BG_W - 2 * UI_SUBMENU_NET_CTRL_X)
#define UI_SUBMENU_WEB_CTRL_H 500//550//600 //650//850//( UI_SUBMENU_NET_CTRL_TEXT_H + UI_SUBMENU_NET_CTRL_TEXT_H_GAP)

//
#define DYN_WEBSITE_WEB_ITEM_X 20
#define DYN_WEBSITE_WEB_ITEM_Y 0
#define DYN_WEBSITE_WEB_ITEM_W 190//180
#define DYN_WEBSITE_WEB_ITEM_H 140//130

//
#define DYN_WEBSITE_WEB_PIC_X     5
#define DYN_WEBSITE_WEB_PIC_Y     0
#define DYN_WEBSITE_WEB_PIC_W    180
#define DYN_WEBSITE_WEB_PIC_H  90//84

//
#define DYN_WEBSITE_WEB_PIC_MIDL 0
#define DYN_WEBSITE_WEB_PIC_MIDT 0
#define DYN_WEBSITE_WEB_PIC_MIDR (DYN_WEBSITE_WEB_PIC_W - DYN_WEBSITE_WEB_PIC_MIDL)
#define DYN_WEBSITE_WEB_PIC_MIDB (DYN_WEBSITE_WEB_PIC_H - DYN_WEBSITE_WEB_PIC_MIDT)

//
#define DYN_WEBSITE_WEB_NAME_X  0
#define DYN_WEBSITE_WEB_NAME_Y  100
#define DYN_WEBSITE_WEB_NAME_W  190
#define DYN_WEBSITE_WEB_NAME_H  40

//
#define DYN_WEBSITE_WEB_NAME_MIDL  0
#define DYN_WEBSITE_WEB_NAME_MIDT  0
#define DYN_WEBSITE_WEB_NAME_MIDR  (DYN_WEBSITE_WEB_NAME_W - DYN_WEBSITE_WEB_NAME_MIDL)
#define DYN_WEBSITE_WEB_NAME_MIDB  (DYN_WEBSITE_WEB_NAME_H - DYN_WEBSITE_WEB_NAME_MIDT)

//
#define DYN_WEBSITE_WEB_ITEM_H_GAP 95
#define DYN_WEBSITE_WEB_ITEM_V_GAP 40

#define NET_WEB_MBOX_L 20
#define NET_WEB_MBOX_T 0
#define NET_WEB_MBOX_R (UI_SUBMENU_WEB_CTRL_W - NET_WEB_MBOX_L)
#define NET_WEB_MBOX_B UI_SUBMENU_WEB_CTRL_H

//bottom help container
#define UI_NETWORK_BOTTOM_HELP_X  350//200
#define UI_NETWORK_BOTTOM_HELP_W  680

#define DYN_WEBSITE_WEB_COL 1

#define DYN_WEBSITE_WEB_ROW  1

#define DYN_WEBSITE_WEB_PAGE_SIZE  (DYN_WEBSITE_WEB_COL * DYN_WEBSITE_WEB_ROW)

// nm website message
typedef enum
{
    MSG_FOCUS_KEY = MSG_LOCAL_BEGIN + 1150,
    MSG_PAGE_KEY,
    MSG_CLOSE_CFMDLG_NTF,
    MSG_OPEN_WEBSITE_REQ,
    MSG_OPEN_CFMDLG_REQ,
    MSG_OPEN_DLG_REQ,    
} ui_nm_website_msg_t;
RET_CODE open_submenu_online_movie(u32 para1, u32 para2);
#endif


