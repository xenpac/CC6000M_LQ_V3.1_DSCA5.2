/****************************************************************************

****************************************************************************/
#ifndef __UI_SUMENU_NETWORK_H__
#define __UI_SUMENU_NETWORK_H__

//icon text
#define UI_SUBMENU_NET_CTRL_TEXT_H_GAP 10
#define UI_SUBMENU_NET_CTRL_TEXT_H  40
//icon box 
#define UI_SUBMENU_NET_CTRL_X 90//50
#define UI_SUBMENU_NET_CTRL_Y 120//50//240//140
#define UI_SUBMENU_NET_CTRL_W 1070//(COMM_BG_W - 2 * UI_SUBMENU_NET_CTRL_X)
#define UI_SUBMENU_NET_CTRL_H 480//550//600 //650//850//( UI_SUBMENU_NET_CTRL_TEXT_H + UI_SUBMENU_NET_CTRL_TEXT_H_GAP)

#define SUBMENU_BOTTOM_HELP_X  130
#define SUBMENU_BOTTOM_HELP_Y  610
#define SUBMENU_BOTTOM_HELP_W  1000
#define SUBMENU_BOTTOM_HELP_H  50

#define NET_MBOX_L 20
#define NET_MBOX_T 0
#define NET_MBOX_R (UI_SUBMENU_NET_CTRL_W - NET_MBOX_L)
#define NET_MBOX_B UI_SUBMENU_NET_CTRL_H

//bottom help container
#define UI_NETWORK_BOTTOM_HELP_X  350//200
#define UI_NETWORK_BOTTOM_HELP_W  680

#define NETWORK_SUBMENU_ITEM 11

//#define NETWORK_SUBMENU_COL 2
#define NETWORK_SUBMENU_COL 4


#define NETWORK_SUBMENU_ROW  3

//#define NETWORK_SUBMENU_WINCOL 2
#define NETWORK_SUBMENU_WINCOL 4


#define NETWORK_SUBMENU_WINROW  3


#define YOUPORN_PWD_DLG_FOR_CHK_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define YOUPORN_PWD_DLG_FOR_CHK_Y      ((SCREEN_HEIGHT-PWDLG_H)/2)

RET_CODE open_submenu_network(u32 para1, u32 para2);
#endif


