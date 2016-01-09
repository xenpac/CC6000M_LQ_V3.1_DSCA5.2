/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_VIDEO_PLAYER_H__
#define __UI_VIDEO_PLAYER_H__

/* coordinate */
#define VDO_PLAYER_CONT_X             (0)
#define VDO_PLAYER_CONT_Y             (0)
#define VDO_PLAYER_CONT_W             (SCREEN_WIDTH)
#define VDO_PLAYER_CONT_H             (SCREEN_HEIGHT)

//Loadmedia time
#define VDO_PLAYER_LOADMEDIA_TIME_CONT_X      ((SCREEN_WIDTH - VDO_PLAYER_LOADMEDIA_TIME_CONT_W) >> 1)
#define VDO_PLAYER_LOADMEDIA_TIME_CONT_Y      (200)
#define VDO_PLAYER_LOADMEDIA_TIME_CONT_W      (560)
#define VDO_PLAYER_LOADMEDIA_TIME_CONT_H      (60)

#define VDO_PLAYER_LOADMEDIA_TIME_X      (0)
#define VDO_PLAYER_LOADMEDIA_TIME_Y      (0)
#define VDO_PLAYER_LOADMEDIA_TIME_W      (VDO_PLAYER_LOADMEDIA_TIME_CONT_W)
#define VDO_PLAYER_LOADMEDIA_TIME_H      (VDO_PLAYER_LOADMEDIA_TIME_CONT_H)

//Bps
#define VDO_PLAYER_BPS_CONT_X      ((SCREEN_WIDTH - VDO_PLAYER_BPS_CONT_W) >> 1)
#define VDO_PLAYER_BPS_CONT_Y      (200)
#define VDO_PLAYER_BPS_CONT_W      (560)
#define VDO_PLAYER_BPS_CONT_H      (60)

#define VDO_PLAYER_BPS_X      (0)
#define VDO_PLAYER_BPS_Y      (0)
#define VDO_PLAYER_BPS_W      (VDO_PLAYER_BPS_CONT_W >> 1)
#define VDO_PLAYER_BPS_H      (VDO_PLAYER_BPS_CONT_H)

#define VDO_PLAYER_BUF_PERCENT_X      (VDO_PLAYER_BPS_CONT_W >> 1) 
#define VDO_PLAYER_BUF_PERCENT_Y      (0)
#define VDO_PLAYER_BUF_PERCENT_W      (VDO_PLAYER_BPS_CONT_W >> 1)
#define VDO_PLAYER_BUF_PERCENT_H      (VDO_PLAYER_BPS_CONT_H)

//Panel
#define VDO_PLAYER_PANEL_X              (90)
#define VDO_PLAYER_PANEL_Y              (560)
#define VDO_PLAYER_PANEL_W              (1100)
#define VDO_PLAYER_PANEL_H              (100)

//Name
#define VDO_PLAYER_NAME_X         (30)
#define VDO_PLAYER_NAME_Y         (10)
#define VDO_PLAYER_NAME_W         (400)
#define VDO_PLAYER_NAME_H         (40)
//System time
#define VDO_PLAYER_SYSTEM_TIME_X      ((VDO_PLAYER_PANEL_W - VDO_PLAYER_SYSTEM_TIME_W) >> 1)
#define VDO_PLAYER_SYSTEM_TIME_Y      (VDO_PLAYER_NAME_Y)
#define VDO_PLAYER_SYSTEM_TIME_W      (200)
#define VDO_PLAYER_SYSTEM_TIME_H      (VDO_PLAYER_NAME_H)
//Help bar
#define VDO_PLAYER_HELP_BAR_X         (VDO_PLAYER_PANEL_W - VDO_PLAYER_NAME_X - VDO_PLAYER_HELP_BAR_W)
#define VDO_PLAYER_HELP_BAR_Y         (VDO_PLAYER_NAME_Y)
#define VDO_PLAYER_HELP_BAR_W         (200)
#define VDO_PLAYER_HELP_BAR_H         (VDO_PLAYER_NAME_H)
//Small play icon
#define VDO_PLAYER_SMALL_PLAY_ICON_X    (VDO_PLAYER_NAME_X)
#define VDO_PLAYER_SMALL_PLAY_ICON_Y    (VDO_PLAYER_NAME_Y + VDO_PLAYER_NAME_H)
#define VDO_PLAYER_SMALL_PLAY_ICON_W    (26)
#define VDO_PLAYER_SMALL_PLAY_ICON_H    (VDO_PLAYER_NAME_H)
//Progress
#define VDO_PLAYER_PROGRESS_X      (VDO_PLAYER_SMALL_PLAY_ICON_X + VDO_PLAYER_SMALL_PLAY_ICON_W + 20)
#define VDO_PLAYER_PROGRESS_Y      (VDO_PLAYER_SMALL_PLAY_ICON_Y + ((VDO_PLAYER_SMALL_PLAY_ICON_H - VDO_PLAYER_PROGRESS_H) >> 1))
#define VDO_PLAYER_PROGRESS_W      (674)
#define VDO_PLAYER_PROGRESS_H      (12)
#define VDO_PLAYER_PROGRESS_MIDL   (0)
#define VDO_PLAYER_PROGRESS_MIDT   (0)
#define VDO_PLAYER_PROGRESS_MIDR   (VDO_PLAYER_PROGRESS_W)
#define VDO_PLAYER_PROGRESS_MIDB   (VDO_PLAYER_PROGRESS_H)
#define VDO_PLAYER_PROGRESS_STEP  (VDO_PLAYER_PROGRESS_W)
//Seek time
#define VDO_PLAYER_SEEK_TIME_X      (VDO_PLAYER_PROGRESS_X + VDO_PLAYER_PROGRESS_W + 15)
#define VDO_PLAYER_SEEK_TIME_Y      (VDO_PLAYER_NAME_Y + VDO_PLAYER_NAME_H)
#define VDO_PLAYER_SEEK_TIME_W      (90)
#define VDO_PLAYER_SEEK_TIME_H      (VDO_PLAYER_NAME_H)
//Play time
#define VDO_PLAYER_PLAY_TIME_X      (VDO_PLAYER_SEEK_TIME_X + VDO_PLAYER_SEEK_TIME_W + 15)
#define VDO_PLAYER_PLAY_TIME_Y      (VDO_PLAYER_SEEK_TIME_Y)
#define VDO_PLAYER_PLAY_TIME_W      (90)
#define VDO_PLAYER_PLAY_TIME_H      (VDO_PLAYER_SEEK_TIME_H)
//Total time
#define VDO_PLAYER_TOTAL_TIME_X       (VDO_PLAYER_PLAY_TIME_X + VDO_PLAYER_PLAY_TIME_W)
#define VDO_PLAYER_TOTAL_TIME_Y       (VDO_PLAYER_SEEK_TIME_Y)
#define VDO_PLAYER_TOTAL_TIME_W       (110)
#define VDO_PLAYER_TOTAL_TIME_H       (VDO_PLAYER_SEEK_TIME_H)

//Large play icon
#define VDO_PLAYER_LARGE_PLAY_ICON_X        ((SCREEN_WIDTH - VDO_PLAYER_LARGE_PLAY_ICON_W) >> 1)
#define VDO_PLAYER_LARGE_PLAY_ICON_Y        (260)
#define VDO_PLAYER_LARGE_PLAY_ICON_W        (90)
#define VDO_PLAYER_LARGE_PLAY_ICON_H        (90)


#endif
