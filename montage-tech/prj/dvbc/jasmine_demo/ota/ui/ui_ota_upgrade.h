/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 ****************************************************************************/
#ifndef __UI_OTA_UPGRADE_H__
#define __UI_OTA_UPGRADE_H__

/*! coordinates*/
#define OTA_UPGRADE_CONTX 0
#define OTA_UPGRADE_CONTY 0
#define OTA_UPGRADE_CONTW SCREEN_WIDTH
#define OTA_UPGRADE_CONTH SCREEN_HEIGHT

#define OTA_UPGRADE_TTLX 14
#define OTA_UPGRADE_TTLY 20
#define OTA_UPGRADE_TTLW (OTA_UPGRADE_CONTW - 2 * OTA_UPGRADE_TTLX)
#define OTA_UPGRADE_TTLH 40

#define OTA_UPGRADE_STATEX OTA_UPGRADE_TTLX
#define OTA_UPGRADE_STATEY (OTA_UPGRADE_TTLY + OTA_UPGRADE_TTLH + OTA_UPGRADE_ITEM_VGAP)
#define OTA_UPGRADE_STATEW (OTA_UPGRADE_CONTW - 2 * OTA_UPGRADE_STATEX)
#define OTA_UPGRADE_STATEH 40

#define OTA_UPGRADE_NOTESX OTA_UPGRADE_STATEX
#define OTA_UPGRADE_NOTESY (OTA_UPGRADE_STATEY + OTA_UPGRADE_STATEH + OTA_UPGRADE_ITEM_VGAP)
#define OTA_UPGRADE_NOTESW  (OTA_UPGRADE_CONTW - 2 * OTA_UPGRADE_NOTESX)
#define OTA_UPGRADE_NOTESH  60

#define OTA_UPGRADE_PBARX OTA_UPGRADE_STATEX
#define OTA_UPGRADE_PBARY (OTA_UPGRADE_NOTESY + OTA_UPGRADE_NOTESH + OTA_UPGRADE_ITEM_VGAP)
#define OTA_UPGRADE_PBARW (OTA_UPGRADE_CONTW - 2 * OTA_UPGRADE_PBARX - OTA_UPGRADE_PERW)
#define OTA_UPGRADE_PBARH 20

#define OTA_UPGRADE_PERX (OTA_UPGRADE_PBARX + OTA_UPGRADE_PBARW)
#define OTA_UPGRADE_PERY  OTA_UPGRADE_PBARY
#define OTA_UPGRADE_PERW  60
#define OTA_UPGRADE_PERH  40

#define OTA_UPGRADE_ITEM_VGAP 10

/*! r-style*/
#define RSI_OTA_UPGRADE_PBAR_MIN RSI_YELLOW
#define RSI_OTA_UPGRADE_PBAR_MAX RSI_WHITE

/*! f-style*/

/*! others*/
#define OTA_UPGRADE_PBAR_MIN 0
#define OTA_UPGRADE_PBAR_MAX 100

s32 open_ota_upgrade(u32 para1, u32 para2);

#endif
