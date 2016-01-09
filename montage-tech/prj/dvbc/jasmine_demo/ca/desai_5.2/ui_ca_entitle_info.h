/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_ENTITLE_INFO_H__
#define __UI_CA_ENTITLE_INFO_H__

/*cont*/
#define CA_CONT_X          0
#define CA_CONT_Y          ((SCREEN_HEIGHT - CA_CONT_H) / 2)
#define CA_CONT_W          UI_COMMON_WIDTH
#define CA_CONT_H          UI_COMMON_HEIGHT

//TILTLE
#define ENTITLE_TITLE_X       ((CA_CONT_W - ENTITLE_TITLE_W) / 2)
#define ENTITLE_TITLE_Y       120
#define ENTITLE_TITLE_W      900
#define ENTITLE_TITLE_H       470

//plist
#define CA_PLIST_BG_X  ENTITLE_TITLE_X
#define CA_PLIST_BG_Y  (CA_MBOX_Y + CA_MBOX_H + 5)
#define CA_PLIST_BG_W  ENTITLE_TITLE_W
#define CA_PLIST_BG_H  (ENTITLE_TITLE_H - CA_MBOX_H - 10)

//plist rect
#define CA_LIST_BG_MIDL         10
#define CA_LIST_BG_MIDT         10
#define CA_LIST_BG_MIDW        (CA_PLIST_BG_W-20)
#define CA_LIST_BG_MIDH        (CA_PLIST_BG_H-20)
#define CA_LIST_BG_VGAP         12

//mbox
#define CA_MBOX_X           ENTITLE_TITLE_X
#define CA_MBOX_Y           ENTITLE_TITLE_Y
#define CA_MBOX_W          ENTITLE_TITLE_W
#define CA_MBOX_H           COMM_ITEM_H

//other
#define CA_INFO_LIST_PAGE         12
#define CA_INFO_ENTITLE_FIELD        2

//product id & entitle time
#define CA_INFO_PRODUCT_ID_W            80
#define CA_INFO_PRODUCT_ID_VGAP         15
#define CA_INFO_PRODUCT_ID_ITEM_OFFSET 40
#define CA_INFO_ENTITLE_TIME_W         (CA_PLIST_BG_W - CA_INFO_PRODUCT_ID_W - CA_INFO_PRODUCT_ID_VGAP - CA_LIST_BG_MIDL * 2)

#define CA_ITME_COUNT           2



/* rect style */
RET_CODE open_ca_entitle_info(u32 para1, u32 para2);

#endif
