/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PHOTO_INFO_H__
#define __UI_PHOTO_INFO_H__

/* coordinate */
#define PHOTO_INFO_CONT_X       (SCREEN_WIDTH- PHOTO_INFO_CONT_W) / 2
#define PHOTO_INFO_CONT_Y       (SCREEN_HEIGHT - PHOTO_INFO_CONT_H) / 2
#define PHOTO_INFO_CONT_W       450
#define PHOTO_INFO_CONT_H       305// 260

#define PHOTO_INFO_NAME_X       (PHOTO_INFO_CONT_W- PHOTO_INFO_NAME_W) / 2
#define PHOTO_INFO_NAME_Y        5
#define PHOTO_INFO_NAME_W       400
#define PHOTO_INFO_NAME_H        30

#define PHOTO_INFO_DESCR_X       PHOTO_INFO_ITEM_X
#define PHOTO_INFO_DESCR_Y       260 //210
#define PHOTO_INFO_DESCR_W       300
#define PHOTO_INFO_DESCR_H        30

#define PHOTO_INFO_ITEM_X        5
#define PHOTO_INFO_ITEM_Y        40
#define PHOTO_INFO_ITEM_LW       190
#define PHOTO_INFO_ITEM_RW       250
#define PHOTO_INFO_ITEM_H        COMM_ITEM_H
#define PHOTO_INFO_ITEM_V_GAP    5

#define PHOTO_INFO_ITEM_CNT      4

#define PHOTO_INFO_SET_FRM       RSI_COMMAN_BG



RET_CODE open_photo_info(u32 para1, u32 para2);


#endif


