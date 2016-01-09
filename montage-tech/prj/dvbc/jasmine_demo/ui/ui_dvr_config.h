/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_DVR_CONFIG_H__
#define __UI_DVR_CONFIG_H__

/* coordinate */
#define DVR_CONFIG_ITEM_CNT		2
#define DVR_CONFIG_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define DVR_CONFIG_ITEM_Y			COMM_ITEM_OY_IN_ROOT
#define DVR_CONFIG_ITEM_LW		COMM_ITEM_LW
#define DVR_CONFIG_ITEM_RW		(COMM_ITEM_MAX_WIDTH - DVR_CONFIG_ITEM_LW)
#define DVR_CONFIG_ITEM_H			COMM_ITEM_H


/*confirm format dlg container*/
#define DVR_FORMAT_CONFIRM_CONT_X       ((COMM_BG_W - DVR_FORMAT_CONFIRM_CONT_W) / 2)
#define DVR_FORMAT_CONFIRM_CONT_Y       ((COMM_BG_H - DVR_FORMAT_CONFIRM_CONT_H) / 2)
#define DVR_FORMAT_CONFIRM_CONT_W       400
#define DVR_FORMAT_CONFIRM_CONT_H       200
/*dlg content*/
#define DVR_FORMAT_CONFIRM_CONTENT_X    ((DVR_FORMAT_CONFIRM_CONT_W - DVR_FORMAT_CONFIRM_CONTENT_W) / 2)
#define DVR_FORMAT_CONFIRM_CONTENT_Y    30
#define DVR_FORMAT_CONFIRM_CONTENT_W    (DVR_FORMAT_CONFIRM_CONT_W - 60)
#define DVR_FORMAT_CONFIRM_CONTENT_H    80
/*dlg fat button*/
#define DVR_FORMAT_CONFIRM_FAT_X       60
#define DVR_FORMAT_CONFIRM_FAT_Y       (DVR_FORMAT_CONFIRM_CONT_H - DVR_FORMAT_CONFIRM_FAT_H - 40)
#define DVR_FORMAT_CONFIRM_FAT_W       80
#define DVR_FORMAT_CONFIRM_FAT_H       36
/*dlg ntfs button*/
#define DVR_FORMAT_CONFIRM_NTFS_X       (DVR_FORMAT_CONFIRM_FAT_X + DVR_FORMAT_CONFIRM_FAT_W + DVR_FORMAT_CONFIRM_H_GAP)
#define DVR_FORMAT_CONFIRM_NTFS_Y       DVR_FORMAT_CONFIRM_FAT_Y
#define DVR_FORMAT_CONFIRM_NTFS_W       DVR_FORMAT_CONFIRM_FAT_W
#define DVR_FORMAT_CONFIRM_NTFS_H       DVR_FORMAT_CONFIRM_FAT_H
/*dlg cancel button*/
#define DVR_FORMAT_CONFIRM_CANCEL_X     (DVR_FORMAT_CONFIRM_NTFS_X + DVR_FORMAT_CONFIRM_NTFS_W + DVR_FORMAT_CONFIRM_H_GAP)
#define DVR_FORMAT_CONFIRM_CANCEL_Y     DVR_FORMAT_CONFIRM_FAT_Y
#define DVR_FORMAT_CONFIRM_CANCEL_W     DVR_FORMAT_CONFIRM_FAT_W
#define DVR_FORMAT_CONFIRM_CANCEL_H     DVR_FORMAT_CONFIRM_FAT_H


#define DVR_FORMAT_CONFIRM_H_GAP 20
#define DVR_CONFIG_ITEM_V_GAP		20

/* rect style */
#define RSI_DVR_CONFIG_FRM		RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_dvr_config (u32 para1, u32 para2);

u16 *ui_usb_get_timeshift_file_name(void);

RET_CODE ui_timeshift_total_size(void);

#endif

