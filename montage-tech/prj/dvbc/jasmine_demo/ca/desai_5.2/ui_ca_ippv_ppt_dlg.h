/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_IPPV_PPT_DLG_H__
#define __UI_CA_IPPV_PPT_DLG_H__


#define CA_IPPV_COMM_DLG_X         ((SCREEN_WIDTH - CA_IPPV_COMM_DLG_W) / 2)
#define CA_IPPV_COMM_DLG_Y         ((SCREEN_HEIGHT - CA_IPPV_COMM_DLG_H) / 2 )
#define CA_IPPV_COMM_DLG_W         600
#define CA_IPPV_COMM_DLG_H         400

#define CA_IPPV_TEXT_TITLE_X       0
#define CA_IPPV_TEXT_TITLE_Y       10
#define CA_IPPV_TEXT_TITLE_W       CA_IPPV_COMM_DLG_W
#define CA_IPPV_TEXT_TITLE_H       40
#define CA_IPPV_TEXT_ITEM_HGAP   10

#define CA_IPPV_INFO_ITEM_X        5
#define CA_IPPV_INFO_ITEM_Y        5
#define CA_IPPV_INFO_ITEM_LW0      220
#define CA_IPPV_INFO_ITEM_RW0      220


#define CA_IPPV_INFO_CONT_X        80
#define CA_IPPV_INFO_CONT_W        200
#define CA_IPPV_INFO_CONT_H        35

#define CA_IPPV_INFO_ITEM_CNT       9

#define CA_IPPV_BURSE_ITEM_GAP   10
#define CA_IPPV_BURSE_ITEM_LW0   130
#define CA_IPPV_BURSE_ITEM_RW0   100
#define CA_IPPV_BURSE_ITEM_X     (CA_IPPV_INFO_ITEM_X+CA_IPPV_BURSE_ITEM_LW0+CA_IPPV_BURSE_ITEM_RW0+CA_IPPV_BURSE_ITEM_GAP)
#define CA_IPPV_PWD_X         (CA_IPPV_COMM_DLG_X + 100)
#define CA_IPPV_PWD_Y         (CA_IPPV_COMM_DLG_Y + 50)
RET_CODE open_ca_ippv_ppt_dlg(u32 para1, u32 para2);
#endif


