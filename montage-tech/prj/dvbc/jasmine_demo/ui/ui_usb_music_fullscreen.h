/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_MUSIC_FULL_SCREEN_H__
#define __UI_MUSIC_FULL_SCREEN_H__

/* coordinate */
#define MUSIC_FULL_SCREEN_CONT_X        COMM_BG_X
#define MUSIC_FULL_SCREEN_CONT_Y        COMM_BG_Y
#define MUSIC_FULL_SCREEN_CONT_W        COMM_BG_W
#define MUSIC_FULL_SCREEN_CONT_H        COMM_BG_H

#define MUSIC_FULL_PICTURE_X        120
#define MUSIC_FULL_PICTURE_Y        120//(MUSIC_FULL_SCREEN_CONT_H - MUSIC_FULL_PICTURE_H)/2
#define MUSIC_FULL_PICTURE_W       360//400
#define MUSIC_FULL_PICTURE_H        400//450

#define MUSIC_FULL_LRC_X        (MUSIC_FULL_PICTURE_X + MUSIC_FULL_PICTURE_W + 10)
#define MUSIC_FULL_LRC_Y        MUSIC_FULL_PICTURE_Y
#define MUSIC_FULL_LRC_W        360//400
#define MUSIC_FULL_LRC_H        400//450

#define MUSIC_FULL_BACKGROUP_X        50
#define MUSIC_FULL_BACKGROUP_Y        70
#define MUSIC_FULL_BACKGROUP_W        855
#define MUSIC_FULL_BACKGROUP_H        450

/*rstyle*/
#define RSI_MUSIC_PIC_RECT  RSI_TV
#define RSI_MUSIC_LYRICS_RECT  RSI_TV

/* font style */

/* others */
#define MUSIC_FULL_SCREEN_LIST_CNT      4
#define MUSIC_FULL_SCREEN_LIST_FIELD   3

/**/
//preview logo
#define MUSIC_FULL_LOGO_X  100
#define MUSIC_FULL_LOGO_Y  120
#define MUSIC_FULL_LOGO_W  250
#define MUSIC_FULL_LOGO_H  150

RET_CODE open_usb_music_fullscreen(u32 para1, u32 para2);

BOOL ui_music_is_fullscreen_play(void);
void ui_music_set_fullscreen_play(BOOL fullscreen_play);

#endif


