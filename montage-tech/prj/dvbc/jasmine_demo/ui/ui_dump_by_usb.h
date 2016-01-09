/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_DUMP_BY_USB_H_
#define _UI_DUMP_BY_USB_H_

#include "ui_common.h"

/*! coordinate */
#define DUMP_BY_USB_CONT_X        ((COMM_BG_W - DUMP_BY_USB_CONT_W) / 2)
#define DUMP_BY_USB_CONT_H        360

#define DUMP_BY_USB_ITEM_CNT      4
#define DUMP_BY_USB_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define DUMP_BY_USB_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define DUMP_BY_USB_ITEM_LW       COMM_ITEM_LW
#define DUMP_BY_USB_ITEM_RW       (COMM_ITEM_MAX_WIDTH - DUMP_BY_USB_ITEM_LW)
#define DUMP_BY_USB_ITEM_H        COMM_ITEM_H
#define DUMP_BY_USB_ITEM_V_GAP    5

#define DUMP_BY_USB_TXT_X	DUMP_BY_USB_ITEM_X
#define DUMP_BY_USB_TXT_W	0//60
#define DUMP_BY_USB_TXT_H	30

#define DUMP_BY_USB_PBAR_X  (DUMP_BY_USB_TXT_X + DUMP_BY_USB_TXT_W)
#define DUMP_BY_USB_PBAR_W (COMM_ITEM_MAX_WIDTH - DUMP_BY_USB_PER_W)
#define DUMP_BY_USB_PBAR_H  16

#define DUMP_BY_USB_PER_X (DUMP_BY_USB_PBAR_X + DUMP_BY_USB_PBAR_W)
#define DUMP_BY_USB_PER_W 60
#define DUMP_BY_USB_PER_H 30

//misc file list
#define DUMP_BY_USB_MISC_LIST_X    140
#define DUMP_BY_USB_MISC_LIST_Y    180
#define DUMP_BY_USB_MISC_LIST_W    300
#define DUMP_BY_USB_MISC_LIST_H    260

#define DUMP_BY_USB_MISC_LIST_MIDL  0
#define DUMP_BY_USB_MISC_LIST_MIDT  0
#define DUMP_BY_USB_MISC_LIST_MIDW  (DUMP_BY_USB_MISC_LIST_W - 2 * DUMP_BY_USB_MISC_LIST_MIDL)
#define DUMP_BY_USB_MISC_LIST_MIDH  (DUMP_BY_USB_MISC_LIST_H - 2 * DUMP_BY_USB_MISC_LIST_MIDT)

#define DUMP_BY_USB_MISC_LIST_VGAP  0

#define DUMP_BY_USB_MISC_LIST_PAGE     7
#define DUMP_BY_USB_MISC_LIST_FIELD    1

//file files list
#define DUMP_BY_USB_IMG_LIST_X     DUMP_BY_USB_MISC_LIST_X
#define DUMP_BY_USB_IMG_LIST_Y     DUMP_BY_USB_MISC_LIST_Y
#define DUMP_BY_USB_IMG_LIST_W     DUMP_BY_USB_MISC_LIST_W
#define DUMP_BY_USB_IMG_LIST_H     DUMP_BY_USB_MISC_LIST_H

#define DUMP_BY_USB_IMG_LIST_MIDL  0
#define DUMP_BY_USB_IMG_LIST_MIDT  0
#define DUMP_BY_USB_IMG_LIST_MIDW  (DUMP_BY_USB_IMG_LIST_W - 2 * DUMP_BY_USB_IMG_LIST_MIDL)
#define DUMP_BY_USB_IMG_LIST_MIDH  (DUMP_BY_USB_IMG_LIST_H - 2 * DUMP_BY_USB_IMG_LIST_MIDT)

#define DUMP_BY_USB_IMG_LIST_VGAP  0

#define DUMP_BY_USB_IMG_LIST_PAGE     7
#define DUMP_BY_USB_IMG_LIST_FIELD    1


#define DUMP_BY_USB_MODE_CNT 1
#define DUMP_BY_USB_FILE_CNT 2

#define DUMP_BY_USB_MAX_MISC_FILES_CNT 7
#define DUMP_BY_USB_MAX_IMG_FILES_CNT 7


#define DUMP_BY_USB_STS_ITEM_CNT 4

/*! rect style */
#define RSI_DUMP_BY_USB_FRM       RSI_WINDOW_1
#define RSI_DUMP_BY_USB_PBAR_BG     RSI_PROGRESS_BAR_BG
#define RSI_DUMP_BY_USB_PBAR_MID    RSI_PROGRESS_BAR_MID_ORANGE


#define STS_STR_MAX_LEN_DUMP 32

/*! font style */

/*! others*/
typedef enum
{
    UI_USB_DUMP_IDEL = 0,
    UI_USB_DUMP_PRE_READ,
    UI_USB_DUMP_READING,
    UI_USB_DUMP_READ_END,
    UI_USB_DUMP_WRITING    
}ui_usb_dump_status_t;

RET_CODE open_dump_by_usb(u32 para1, u32 para2);

u32 ui_usb_dumping_sts(void);

void ui_release_dump(void);

#endif
