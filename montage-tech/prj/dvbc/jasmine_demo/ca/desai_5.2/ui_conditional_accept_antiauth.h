/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_ANTIAUTH_H__
#define __UI_CONDITIONAL_ACCEPT_ANTIAUTH_H__

//piar info items
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_CNT      4

#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_X        ((COMM_BG_W - CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_LW - CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_RW) / 2)
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_Y        200
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_LW       400
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_RW       400
#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_H        COMM_CTRL_H

#define CONDITIONAL_ACCEPT_BURSE_INFO_ITEM_V_GAP    20


/* rect style */

/* font style */

/* others */

RET_CODE open_conditional_accept_antiauth(u32 para1, u32 para2);

#endif


