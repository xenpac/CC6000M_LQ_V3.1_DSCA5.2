/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _UI_UPGRADE_BY_USB_H_
#define _UI_UPGRADE_BY_USB_H_

#include "ui_common.h"

/*! coordinate */
#define UPGRADE_BY_USB_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define UPGRADE_BY_USB_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define UPGRADE_BY_USB_ITEM_LW       COMM_ITEM_LW
#define UPGRADE_BY_USB_ITEM_RW       (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_USB_ITEM_LW)
#define UPGRADE_BY_USB_ITEM_H        36//COMM_ITEM_H
#define UPGRADE_BY_USB_ITEM_V_GAP    10 //20

#define UPGRADE_BY_USB_TXT_X	UPGRADE_BY_USB_ITEM_X
#define UPGRADE_BY_USB_TXT_W	0//60
#define UPGRADE_BY_USB_TXT_H	30

#define UPGRADE_BY_USB_PBAR_X  (UPGRADE_BY_USB_TXT_X + UPGRADE_BY_USB_TXT_W)
#define UPGRADE_BY_USB_PBAR_W (COMM_ITEM_MAX_WIDTH - UPGRADE_BY_USB_PER_W)
#define UPGRADE_BY_USB_PBAR_H  16

#define UPGRADE_BY_USB_PER_X (UPGRADE_BY_USB_PBAR_X + UPGRADE_BY_USB_PBAR_W)
#define UPGRADE_BY_USB_PER_W 60
#define UPGRADE_BY_USB_PER_H 30

#define UPGRADE_BY_USB_LIST_X	240
#define UPGRADE_BY_USB_LIST_Y 300
#define UPGRADE_BY_USB_LIST_W	300
#define UPGRADE_BY_USB_LIST_H	260

#define UPGRADE_BY_USB_LIST_MIDL  0
#define UPGRADE_BY_USB_LIST_MIDT  0
#define UPGRADE_BY_USB_LIST_MIDW  (UPGRADE_BY_USB_LIST_W - 2 * UPGRADE_BY_USB_LIST_MIDL)
#define UPGRADE_BY_USB_LIST_MIDH  (UPGRADE_BY_USB_LIST_H - 2 * UPGRADE_BY_USB_LIST_MIDT)
#define UPGRADE_BY_USB_SELECT_MIDX 25
#define UPGRADE_BY_USB_SELECT_MIDY 290

#define UPGRADE_DLG_X   ((SCREEN_WIDTH -UPGRADE_DLG_W)/2)
#define UPGRADE_DLG_Y   ((SCREEN_HEIGHT-UPGRADE_DLG_H)/2)
#define UPGRADE_DLG_W   300
#define UPGRADE_DLG_H    200

#define UPGRADE_BY_USB_LIST_VGAP  0

#define UPGRADE_BY_USB_LIST_PAGE     7
#define UPGRADE_BY_USB_LIST_FIELD    1

#define UPGRADE_BY_USB_MODE_CNT 3

#define UPGRADE_BY_USB_MAX_MISC_FILES_CNT 7

#define UPGRADE_BY_USB_STS_ITEM_CNT 1  //  4

/*! rect style */
#define RSI_UPGRADE_BY_USB_FRM       RSI_WINDOW_1
#define RSI_UPGRADE_BY_USB_PBAR_BG     RSI_PROGRESS_BAR_BG
#define RSI_UPGRADE_BY_USB_PBAR_MID    RSI_PROGRESS_BAR_MID_ORANGE

/*! others*/
#define STS_STR_MAX_LEN       32
#define VERSION_LIST_CNT 2//4
#define VERSION_LIST_FIELD 3

#define USB_UPG_MAX_GRP_CNT 4

typedef enum
{
    UI_USB_UPG_IDEL,
    UI_USB_UPG_LOADING,
    UI_USB_UPG_LOAD_END,
    UI_USB_UPG_BURNING
}ui_usb_upg_status_t;

typedef struct
{
  u8 curn_burn_grp;
  u8 grp_cnt;

  struct
  {
    u32 grp_addr;
    u32 grp_size;
  }grps[USB_UPG_MAX_GRP_CNT];
}ui_usb_upg_groups_t;

typedef struct
{
  u32 sdram_size;
  u32 flash_size;
  u16 block_num;
  u16 bh_size;
}ui_usb_dm_base_info_t;

RET_CODE open_upgrade_by_usb(u32 para1, u32 para2);
BOOL usb_upg_is_finish(void);
u32 ui_usb_upgade_sts(void);
#endif
