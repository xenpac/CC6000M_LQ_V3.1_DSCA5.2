/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NVOD_H__
#define __UI_NVOD_H__

/* coordinate */
#define NVOD_CONT_X           0
#define NVOD_CONT_Y           0
#define NVOD_CONT_W           COMM_BG_W
#define NVOD_CONT_H           COMM_BG_H

//preview
#define NVOD_PREV_X           PREV_COMMON_X
#define NVOD_PREV_Y           PREV_COMMON_Y
#define NVOD_PREV_W           PREV_COMMON_W
#define NVOD_PREV_H           PREV_COMMON_H

//info
#define NVOD_INFO_X           NVOD_PREV_X//(NVOD_PREV_X + NVOD_PREV_W + 10)
#define NVOD_INFO_Y           (REF_EVENT_LIST_CONT_Y + REF_EVENT_LIST_CONT_H + 10)//NVOD_PREV_Y
#define NVOD_INFO_W          REF_EVENT_LIST_CONT_W// (NVOD_CONT_W - NVOD_INFO_X - WIN_LR_SPACE)
#define NVOD_INFO_H           145   //500

//progress name
#define NVOD_PBAR_NAME_X       NVOD_INFO_X
#define NVOD_PBAR_NAME_Y       (NVOD_INFO_Y + NVOD_INFO_H + 10)
#define NVOD_PBAR_NAME_W       100
#define NVOD_PBAR_NAME_H       30

//progress bar
#define NVOD_PBAR_X            (NVOD_PBAR_NAME_X + NVOD_PBAR_NAME_W + 5)
#define NVOD_PBAR_Y            (NVOD_PBAR_NAME_Y + (NVOD_PBAR_NAME_H - NVOD_PBAR_H)/2)
#define NVOD_PBAR_W            (COMM_BG_W - 2 * NVOD_PBAR_X)
#define NVOD_PBAR_H            16

//progress percent
#define NVOD_PBAR_PERCENT_X    (NVOD_PBAR_X + NVOD_PBAR_W)
#define NVOD_PBAR_PERCENT_Y    NVOD_PBAR_NAME_Y
#define NVOD_PBAR_PERCENT_W    60
#define NVOD_PBAR_PERCENT_H    30

//ref_event_container
#define REF_EVENT_LIST_CONT_X       NVOD_PREV_X
#define REF_EVENT_LIST_CONT_Y       (COMM_ITEM_OY_IN_ROOT + 10) //340  // 130
#define REF_EVENT_LIST_CONT_W       NVOD_PREV_W
#define REF_EVENT_LIST_CONT_H       210 //190

#define REF_EVENT_LIST_X            0
#define REF_EVENT_LIST_Y            0
#define REF_EVENT_LIST_W            (REF_EVENT_LIST_CONT_W - 2 * REF_EVENT_LIST_X - 10)
#define REF_EVENT_LIST_H            (REF_EVENT_LIST_CONT_H - 2*REF_EVENT_LIST_Y)
#define REF_EVENT_LIST_MIDL         8
#define REF_EVENT_LIST_MIDT         5
#define REF_EVENT_LIST_MIDW         (REF_EVENT_LIST_W - 2 * REF_EVENT_LIST_MIDL)
#define REF_EVENT_LIST_MIDH         (REF_EVENT_LIST_H - 2 * REF_EVENT_LIST_MIDT)
#define REF_EVENT_LIST_FIELD        2

#define REF_EVENT_LIST_SBARX        (REF_EVENT_LIST_X + REF_EVENT_LIST_W + 4)
#define REF_EVENT_LIST_SBARY        REF_EVENT_LIST_Y
#define REF_EVENT_LIST_SBARW        6
#define REF_EVENT_LIST_SBARH        REF_EVENT_LIST_H

#define REF_SVC_LIST_VGAP           0

//shift_event_container
#define SHIFT_EVENT_LIST_CONT_X     (REF_EVENT_LIST_CONT_X + REF_EVENT_LIST_CONT_W + 10)
#define SHIFT_EVENT_LIST_CONT_Y    REF_EVENT_LIST_CONT_Y// 320
#define SHIFT_EVENT_LIST_CONT_W     (NVOD_CONT_W - SHIFT_EVENT_LIST_CONT_X - WIN_LR_SPACE)
#define SHIFT_EVENT_LIST_CONT_H     370//310

#define SHIFT_EVENT_LIST_X          0
#define SHIFT_EVENT_LIST_Y          5
#define SHIFT_EVENT_LIST_W          (SHIFT_EVENT_LIST_CONT_W - 2 * SHIFT_EVENT_LIST_X - 10)
#define SHIFT_EVENT_LIST_H          (SHIFT_EVENT_LIST_CONT_H - 2 * SHIFT_EVENT_LIST_Y)
#define SHIFT_EVENT_LIST_MIDL       8
#define SHIFT_EVENT_LIST_MIDT       8
#define SHIFT_EVENT_LIST_MIDW       (SHIFT_EVENT_LIST_W - 2 * SHIFT_EVENT_LIST_MIDL)
#define SHIFT_EVENT_LIST_MIDH       (SHIFT_EVENT_LIST_H - 2 * SHIFT_EVENT_LIST_MIDT)
#define SHIFT_EVENT_LIST_FIELD      3

#define SHIFT_EVENT_LIST_SBARX      (SHIFT_EVENT_LIST_X + SHIFT_EVENT_LIST_W + 4)
#define SHIFT_EVENT_LIST_SBARY      SHIFT_EVENT_LIST_Y
#define SHIFT_EVENT_LIST_SBARW      6
#define SHIFT_EVENT_LIST_SBARH      SHIFT_EVENT_LIST_H

/* rect style */
#define RSI_NVOD_SBAR           RSI_SCROLL_BAR_BG
#define RSI_PLIST_SBAR_MID      RSI_SCROLL_BAR_MID
#define RSI_NVOD_INFO_CONT      RSI_COMMON_RECT1

/* font style */

/* others */
#define NVOD_LIST_PAGE               5//5
#define NVOD_LIST_SHIFT_PAGE         9//8

void ui_set_shift_event_list_focus(u16 idx);

RET_CODE open_nvod(u32 para1, u32 para2);

#endif

