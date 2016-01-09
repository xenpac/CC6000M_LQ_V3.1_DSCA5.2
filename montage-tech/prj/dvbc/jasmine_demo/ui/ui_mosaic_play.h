/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_MOSAIC_PLAY_H__
#define __UI_MOSAIC_PLAY_H__

//info frame
#define MOSAIC_PLAY_INFO_FRM_X            0 
#define MOSAIC_PLAY_INFO_FRM_Y            0
#define MOSAIC_PLAY_INFO_FRM_W            SCREEN_WIDTH 
#define MOSAIC_PLAY_INFO_FRM_H            40 

//name
#define MOSAIC_PLAY_NAME_X   30
#define MOSAIC_PLAY_NAME_Y   0
#define MOSAIC_PLAY_NAME_W   250
#define MOSAIC_PLAY_NAME_H   40

//help
#define MOSAIC_PLAY_HELP_X         400
#define MOSAIC_PLAY_HELP_Y         0
#define MOSAIC_PLAY_HELP_W         200
#define MOSAIC_PLAY_HELP_H         40

//video frame
#define MOSAIC_PLAY_VIDEO_FRM_X            0 
#define MOSAIC_PLAY_VIDEO_FRM_Y            50
#define MOSAIC_PLAY_VIDEO_FRM_W            SCREEN_WIDTH 
#define MOSAIC_PLAY_VIDEO_FRM_H            460

//video mbox
#define MOSAIC_PLAY_VIDEO_MBOX_X            0 
#define MOSAIC_PLAY_VIDEO_MBOX_Y            0
#define MOSAIC_PLAY_VIDEO_MBOX_W            MOSAIC_PLAY_VIDEO_FRM_W 
#define MOSAIC_PLAY_VIDEO_MBOX_H            MOSAIC_PLAY_VIDEO_FRM_H


RET_CODE open_mosaic_play(u32 para1, u32 para2);

#endif

