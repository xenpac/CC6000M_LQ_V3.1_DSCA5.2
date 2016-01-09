/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

//CA info items
#define ACCEPT_INFO_CARD_INFO_ITEM_CNT      11

#define ACCEPT_INFO_CA_INFO_ITEM_X        ((UI_COMMON_WIDTH - ACCEPT_INFO_CA_INFO_ITEM_LW - ACCEPT_INFO_CA_INFO_ITEM_RW) / 2)
#define ACCEPT_INFO_CA_INFO_ITEM_Y        110
#define ACCEPT_INFO_CA_INFO_ITEM_LW       400
#define ACCEPT_INFO_CA_INFO_ITEM_RW       400
#define ACCEPT_INFO_CA_INFO_ITEM_H        COMM_CTRL_H

#define ACCEPT_CA_INFO_ITEM_V_GAP    0


RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif


