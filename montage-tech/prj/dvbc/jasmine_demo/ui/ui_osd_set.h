/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_OSD_SET_H__
#define __UI_OSD_SET_H__

/* coordinate */
#define OSD_SET_CONT_X        ((COMM_BG_W - OSD_SET_CONT_W) / 2)
#define OSD_SET_CONT_H        420

#if ENABLE_TTX_SUBTITLE
#define OSD_SET_ITEM_CNT     5
#else
#define OSD_SET_ITEM_CNT      4
#endif
#define OSD_SET_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define OSD_SET_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define OSD_SET_ITEM_LW       COMM_ITEM_LW
#define OSD_SET_ITEM_RW       (COMM_ITEM_MAX_WIDTH - OSD_SET_ITEM_LW)
#define OSD_SET_ITEM_H        COMM_ITEM_H
#define OSD_SET_ITEM_V_GAP    20

/* rect style */
#define RSI_OSD_SET_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_osd_set(u32 para1, u32 para2);

#endif



