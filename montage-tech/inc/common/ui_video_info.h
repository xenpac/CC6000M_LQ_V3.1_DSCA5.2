/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_VIDEO_INFO_H__
#define __UI_VIDEO_INFO_H__

#define VINFO_MENU_X  210
#define VINFO_MENU_Y  60
#define VINFO_MENU_W  (SCREEN_WIDTH - VINFO_MENU_X)
#define VINFO_MENU_H  482

#define VINFO_PREV_X  ((VINFO_MENU_W - VINFO_PREV_W) / 2)
#define VINFO_PREV_Y  VINFO_GAP//20
#define VINFO_PREV_W  480
#define VINFO_PREV_H  360

#define VINFO_TEXT_X  VINFO_GAP
#define VINFO_TEXT_Y  (VINFO_PREV_Y + VINFO_PREV_H + VINFO_GAP)//400
#define VINFO_TEXT_W  (VINFO_MENU_W - 2 * VINFO_TEXT_X)
#define VINFO_TEXT_H  (VINFO_MENU_H - VINFO_TEXT_Y ) //80

#define VINFO_SBAR_X  (VINFO_TEXT_X+ VINFO_TEXT_W)
#define VINFO_SBAR_Y  VINFO_TEXT_Y
#define VINFO_SBAR_W  6
#define VINFO_SBAR_H  VINFO_TEXT_H

#define VINFO_GAP  10

RET_CODE open_video_info(u32 para1, u32 para2);

#endif
