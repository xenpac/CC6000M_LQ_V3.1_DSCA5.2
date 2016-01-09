/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 ****************************************************************************/
#ifndef __UI_OTA_USER_INPUT_H__
#define __UI_OTA_USER_INPUT_H__


/*! coordinates*/
#define OTA_USER_INPUT_CONTX 0
#define OTA_USER_INPUT_CONTY 0
#define OTA_USER_INPUT_CONTW  SCREEN_WIDTH
#define OTA_USER_INPUT_CONTH SCREEN_HEIGHT

#define OTA_USER_INPUT_TTLX 14
#define OTA_USER_INPUT_TTLY 20
#define OTA_USER_INPUT_TTLW (OTA_USER_INPUT_CONTW - 2 * OTA_USER_INPUT_TTLX)
#define OTA_USER_INPUT_TTLH 40

#define OTA_USER_INPUT_ITEMLX 20
#define OTA_USER_INPUT_ITEMRX (OTA_USER_INPUT_ITEMLX + OTA_USER_INPUT_ITEMLW)
#define OTA_USER_INPUT_ITEMY  (OTA_USER_INPUT_TTLY + OTA_USER_INPUT_TTLH + OTA_USER_INPUT_ITEM_VGAP)
#define OTA_USER_INPUT_ITEMW (OTA_USER_INPUT_CONTW - 2 * OTA_USER_INPUT_ITEMLX)
#define OTA_USER_INPUT_ITEMLW 240
#define OTA_USER_INPUT_ITEMRW (OTA_USER_INPUT_ITEMW - OTA_USER_INPUT_ITEMLW)
#define OTA_USER_INPUT_ITEMH  40

#define OTA_USER_INPUT_ITEM_VGAP 3
/*! r-style*/

/*! others*/
#define OTA_USER_INPUT_ITEM_CNT 10

#define OTA_TPFREQ_MIN 0
#define OTA_TPFREQ_MAX 999999
#define OTA_TPFREQ_BIT 6

#define OTA_LNBFREQ_MIN 0
#define OTA_LNBFREQ_MAX 9999
#define OTA_LNBFREQ_BIT  4

#define OTA_SYM_MIN 0
#define OTA_SYM_MAX 9999
#define OTA_SYM_BIT 4

#define OTA_DPID_MIN 0
//#define OTA_DPID_MAX 17777//0x1FFF
#define OTA_DPID_MAX 9999//0x1FFF
#define OTA_DPID_BIT 4

#define LNB_FREQ_TYPE_CNT 15

s32 open_ota_user_input(u32 para1, u32 para2);
#endif
