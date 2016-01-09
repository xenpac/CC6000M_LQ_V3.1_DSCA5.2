/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_STORAGE_FORMAT_H__
#define __UI_STORAGE_FORMAT_H__

/* coordinate */
#define STORAGE_FORMAT_ITEM_CNT      6
#define STORAGE_FORMAT_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define STORAGE_FORMAT_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define STORAGE_FORMAT_ITEM_LW       COMM_ITEM_LW
#define STORAGE_FORMAT_ITEM_RW       (COMM_ITEM_MAX_WIDTH - STORAGE_FORMAT_ITEM_LW)
#define STORAGE_FORMAT_ITEM_H        COMM_ITEM_H
#define STORAGE_FORMAT_ITEM_V_GAP    20

#define STORAGE_FORMAT_TEXT_X        STORAGE_FORMAT_ITEM_X//400
#define STORAGE_FORMAT_TEXT_Y        (STORAGE_FORMAT_ITEM_Y + STORAGE_FORMAT_ITEM_CNT*(STORAGE_FORMAT_ITEM_V_GAP+STORAGE_FORMAT_ITEM_H))
#define STORAGE_FORMAT_TEXT_W       200
#define STORAGE_FORMAT_TEXT_H       COMM_ITEM_H//36

/*confirm format dlg container*/
#define STORAGE_FORMAT_CONFIRM_CONT_X       ((COMM_BG_W - STORAGE_FORMAT_CONFIRM_CONT_W) / 2)
#define STORAGE_FORMAT_CONFIRM_CONT_Y       ((COMM_BG_H - STORAGE_FORMAT_CONFIRM_CONT_H) / 2)
#define STORAGE_FORMAT_CONFIRM_CONT_W       400
#define STORAGE_FORMAT_CONFIRM_CONT_H       200
/*dlg content*/
#define STORAGE_FORMAT_CONFIRM_CONTENT_X    ((STORAGE_FORMAT_CONFIRM_CONT_W - STORAGE_FORMAT_CONFIRM_CONTENT_W) / 2)
#define STORAGE_FORMAT_CONFIRM_CONTENT_Y    30
#define STORAGE_FORMAT_CONFIRM_CONTENT_W    (STORAGE_FORMAT_CONFIRM_CONT_W - 60)
#define STORAGE_FORMAT_CONFIRM_CONTENT_H    80
/*dlg fat button*/
#define STORAGE_FORMAT_CONFIRM_FAT_X       60
#define STORAGE_FORMAT_CONFIRM_FAT_Y       (STORAGE_FORMAT_CONFIRM_CONT_H - STORAGE_FORMAT_CONFIRM_FAT_H - 40)
#define STORAGE_FORMAT_CONFIRM_FAT_W       80
#define STORAGE_FORMAT_CONFIRM_FAT_H       36
/*dlg ntfs button*/
#define STORAGE_FORMAT_CONFIRM_NTFS_X       (STORAGE_FORMAT_CONFIRM_FAT_X + STORAGE_FORMAT_CONFIRM_FAT_W + STORAGE_FORMAT_CONFIRM_H_GAP)
#define STORAGE_FORMAT_CONFIRM_NTFS_Y       STORAGE_FORMAT_CONFIRM_FAT_Y
#define STORAGE_FORMAT_CONFIRM_NTFS_W       STORAGE_FORMAT_CONFIRM_FAT_W
#define STORAGE_FORMAT_CONFIRM_NTFS_H       STORAGE_FORMAT_CONFIRM_FAT_H
/*dlg cancel button*/
#define STORAGE_FORMAT_CONFIRM_CANCEL_X     (STORAGE_FORMAT_CONFIRM_NTFS_X + STORAGE_FORMAT_CONFIRM_NTFS_W + STORAGE_FORMAT_CONFIRM_H_GAP)
#define STORAGE_FORMAT_CONFIRM_CANCEL_Y     STORAGE_FORMAT_CONFIRM_FAT_Y
#define STORAGE_FORMAT_CONFIRM_CANCEL_W     STORAGE_FORMAT_CONFIRM_FAT_W
#define STORAGE_FORMAT_CONFIRM_CANCEL_H     STORAGE_FORMAT_CONFIRM_FAT_H

#define STORAGE_FORMAT_CONFIRM_H_GAP 20
/* rect style */
#define RSI_STORAGE_FORMAT_FRM       RSI_WINDOW_1

/* others */

RET_CODE open_storage_format(u32 para1, u32 para2);

#endif