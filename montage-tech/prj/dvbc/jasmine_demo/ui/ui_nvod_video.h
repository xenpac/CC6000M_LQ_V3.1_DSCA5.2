/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NVOD_VIDEO_H__
#define __UI_NVOD_VIDEO_H__

#define NVOD_VIDEO_CONT_X            0 
#define NVOD_VIDEO_CONT_Y            0
#define NVOD_VIDEO_CONT_W            COMM_BG_W 
#define NVOD_VIDEO_CONT_H            COMM_BG_H 
//time
#define NVOD_VIDEO_TIME_X   60
#define NVOD_VIDEO_TIME_Y   30
#define NVOD_VIDEO_TIME_W   100
#define NVOD_VIDEO_TIME_H   40

#define NVOD_VIDEO_INFO_CONT_X         ((NVOD_VIDEO_CONT_W - NVOD_VIDEO_INFO_CONT_W)/2)
#define NVOD_VIDEO_INFO_CONT_Y         ((NVOD_VIDEO_CONT_H - NVOD_VIDEO_INFO_CONT_H)/2)
#define NVOD_VIDEO_INFO_CONT_W         500
#define NVOD_VIDEO_INFO_CONT_H         200

#define NVOD_TITLE_STR_X         ((NVOD_VIDEO_INFO_CONT_W - NVOD_TITLE_STR_W)/2)
#define NVOD_TITLE_STR_Y         0
#define NVOD_TITLE_STR_W         200
#define NVOD_TITLE_STR_H         40

#define NVOD_TITLE_NAME_X         20
#define NVOD_TITLE_NAME_Y         (NVOD_TITLE_STR_Y + NVOD_TITLE_STR_H+5)
#define NVOD_TITLE_NAME_W         160
#define NVOD_TITLE_NAME_H         30

#define NVOD_TITLE_SCREEN_X         (NVOD_TITLE_NAME_X+NVOD_TITLE_NAME_W+60)
#define NVOD_TITLE_SCREEN_Y         NVOD_TITLE_NAME_Y
#define NVOD_TITLE_SCREEN_W         250
#define NVOD_TITLE_SCREEN_H         30


RET_CODE open_nvod_video(u32 para1, u32 para2);

#endif

