/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NVOD_VOLUME_H__
#define __UI_NVOD_VOLUME_H__

#define NVOD_VOLUME_L   ((SCREEN_WIDTH-NVOD_VOLUME_W)/2)
#define NVOD_VOLUME_T   580
#define NVOD_VOLUME_W   576
#define NVOD_VOLUME_H   40

#define NVOD_VOLUME_ICON_X 0
#define NVOD_VOLUME_ICON_Y 0
#define NVOD_VOLUME_ICON_W 60
#define NVOD_VOLUME_ICON_H 60

#define NVOD_VOLUME_BAR_CONT_X 0
#define NVOD_VOLUME_BAR_CONT_Y 0
#define NVOD_VOLUME_BAR_CONT_W NVOD_VOLUME_W
#define NVOD_VOLUME_BAR_CONT_H NVOD_VOLUME_H

//volume title
#define NVOD_VOLUME_TITLE_X  10
#define NVOD_VOLUME_TITLE_Y  10
#define NVOD_VOLUME_TITLE_W  80
#define NVOD_VOLUME_TITLE_H  30

//volume number
#define NVOD_VOLUME_NUM_X  30
#define NVOD_VOLUME_NUM_Y  50
#define NVOD_VOLUME_NUM_W  40
#define NVOD_VOLUME_NUM_H  30

//volume progress
#define NVOD_VOLUME_BAR_X  72
#define NVOD_VOLUME_BAR_Y  ((NVOD_VOLUME_BAR_CONT_H-NVOD_VOLUME_BAR_H)/2)
#define NVOD_VOLUME_BAR_W  (NVOD_VOLUME_BAR_CONT_W - 2*NVOD_VOLUME_BAR_X)
#define NVOD_VOLUME_BAR_H  16

/* rect style */
#define RSI_NVOD_VOLUME_CONT     RSI_IGNORE
#define RSI_NVOD_VOLUME_BAR_FRAME RSI_VOLUME_WIN

RET_CODE open_nvod_volume (u32 para1, u32 para2);

RET_CODE close_nvod_volume(void);

#endif


