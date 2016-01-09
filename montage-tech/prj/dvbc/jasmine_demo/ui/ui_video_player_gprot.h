/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_VIDEO_PLAYER_GPROT_H__
#define __UI_VIDEO_PLAYER_GPROT_H__

typedef struct
{ 
    u16 *name;
    u16 url_cnt;
    u8  **pp_urlList;
} ui_video_player_param;

RET_CODE open_video_player(u32 para1, u32 para2);

#endif
