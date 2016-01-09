/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_IPTV_PLAYER_H__
#define __UI_IPTV_PLAYER_H__

/* coordinate */
#define IPTV_PLAYER_CONT_X             (0)
#define IPTV_PLAYER_CONT_Y             (0)
#define IPTV_PLAYER_CONT_W             (SCREEN_WIDTH)
#define IPTV_PLAYER_CONT_H             (SCREEN_HEIGHT)//260

//panel
#define IPTV_PLAYER_PANEL_X              (90)//(60)
#define IPTV_PLAYER_PANEL_Y              (540)//(560)
#define IPTV_PLAYER_PANEL_W              (1100)//(1160)
#define IPTV_PLAYER_PANEL_H              (120)//(100)

//name
#define IPTV_PLAYER_TITLE_X         (50)//(20)
#define IPTV_PLAYER_TITLE_Y         (14)//(10)//(0)
#define IPTV_PLAYER_TITLE_W         (300)
#define IPTV_PLAYER_TITLE_H         (46)//(50)

//system time
#define IPTV_PLAYER_SYSTEM_TIME_X      ((IPTV_PLAYER_PANEL_W - IPTV_PLAYER_SYSTEM_TIME_W) >> 1)
#define IPTV_PLAYER_SYSTEM_TIME_Y      (IPTV_PLAYER_TITLE_Y)//(0)
#define IPTV_PLAYER_SYSTEM_TIME_W      (300)//(400)
#define IPTV_PLAYER_SYSTEM_TIME_H      (IPTV_PLAYER_TITLE_H)//(50)

//small play icon
#define IPTV_PLAYER_SMALL_PLAY_ICON_X    (IPTV_PLAYER_TITLE_X)//(20)
#define IPTV_PLAYER_SMALL_PLAY_ICON_Y    (IPTV_PLAYER_TITLE_Y + IPTV_PLAYER_TITLE_H)//(50)
#define IPTV_PLAYER_SMALL_PLAY_ICON_W    (26)
#define IPTV_PLAYER_SMALL_PLAY_ICON_H    (IPTV_PLAYER_TITLE_H)//(50)

//progress
#define IPTV_PLAYER_PROGRESS_X      (IPTV_PLAYER_SMALL_PLAY_ICON_X + IPTV_PLAYER_SMALL_PLAY_ICON_W + 20)
#define IPTV_PLAYER_PROGRESS_Y      (IPTV_PLAYER_SMALL_PLAY_ICON_Y + ((IPTV_PLAYER_SMALL_PLAY_ICON_H - IPTV_PLAYER_PROGRESS_H) >> 1))
#define IPTV_PLAYER_PROGRESS_W      (624)//(750)
#define IPTV_PLAYER_PROGRESS_H      (12)//(23)
#define IPTV_PLAYER_PROGRESS_MIDL   (0)
#define IPTV_PLAYER_PROGRESS_MIDT   (0)
#define IPTV_PLAYER_PROGRESS_MIDR   (IPTV_PLAYER_PROGRESS_W)
#define IPTV_PLAYER_PROGRESS_MIDB   (IPTV_PLAYER_PROGRESS_H)

#define IPTV_PLAYER_PROGRESS_STEP (IPTV_PLAYER_PROGRESS_W)

//seek time
#define IPTV_PLAYER_SEEK_TIME_X      (IPTV_PLAYER_PROGRESS_X + IPTV_PLAYER_PROGRESS_W + 20)
#define IPTV_PLAYER_SEEK_TIME_Y      (IPTV_PLAYER_TITLE_Y + IPTV_PLAYER_TITLE_H)
#define IPTV_PLAYER_SEEK_TIME_W      (80+10)//(110)
#define IPTV_PLAYER_SEEK_TIME_H      (IPTV_PLAYER_TITLE_H)

//play time
#define IPTV_PLAYER_PLAY_TIME_X      (IPTV_PLAYER_SEEK_TIME_X + IPTV_PLAYER_SEEK_TIME_W + 20)
#define IPTV_PLAYER_PLAY_TIME_Y      (IPTV_PLAYER_SEEK_TIME_Y)
#define IPTV_PLAYER_PLAY_TIME_W      (80+10)//(110)
#define IPTV_PLAYER_PLAY_TIME_H      (IPTV_PLAYER_SEEK_TIME_H)

//total time
#define IPTV_PLAYER_TOTAL_TIME_X       (IPTV_PLAYER_PLAY_TIME_X + IPTV_PLAYER_PLAY_TIME_W)
#define IPTV_PLAYER_TOTAL_TIME_Y       (IPTV_PLAYER_SEEK_TIME_Y)
#define IPTV_PLAYER_TOTAL_TIME_W       (90+20)//(120)
#define IPTV_PLAYER_TOTAL_TIME_H       (IPTV_PLAYER_SEEK_TIME_H)

//small play icon
#define IPTV_PLAYER_LARGE_PLAY_ICON_X    ((SCREEN_WIDTH - IPTV_PLAYER_LARGE_PLAY_ICON_W) >> 1)
#define IPTV_PLAYER_LARGE_PLAY_ICON_Y    (260)
#define IPTV_PLAYER_LARGE_PLAY_ICON_W    (90)
#define IPTV_PLAYER_LARGE_PLAY_ICON_H    (90)

//bps
#define IPTV_PLAYER_BPS_CONT_X      ((SCREEN_WIDTH - IPTV_PLAYER_BPS_CONT_W) >> 1) 
#define IPTV_PLAYER_BPS_CONT_Y      (200)
#define IPTV_PLAYER_BPS_CONT_W      (400)
#define IPTV_PLAYER_BPS_CONT_H      (60)

#define IPTV_PLAYER_BPS_X      (0) 
#define IPTV_PLAYER_BPS_Y      (0)
#define IPTV_PLAYER_BPS_W      (IPTV_PLAYER_BPS_CONT_W >> 1)
#define IPTV_PLAYER_BPS_H      (IPTV_PLAYER_BPS_CONT_H)

#define IPTV_PLAYER_BUF_PERCENT_X      (IPTV_PLAYER_BPS_CONT_W >> 1) 
#define IPTV_PLAYER_BUF_PERCENT_Y      (0)
#define IPTV_PLAYER_BUF_PERCENT_W      (IPTV_PLAYER_BPS_CONT_W >> 1)
#define IPTV_PLAYER_BUF_PERCENT_H      (IPTV_PLAYER_BPS_CONT_H)

//format
#define IPTV_PLAYER_FORMAT_LIST_CONT_X   ((SCREEN_WIDTH - IPTV_PLAYER_FORMAT_LIST_CONT_W) >> 1)
#define IPTV_PLAYER_FORMAT_LIST_CONT_Y   (400)//((SCREEN_HEIGHT - IPTV_PLAYER_FORMAT_LIST_CONT_H) >> 1)
#define IPTV_PLAYER_FORMAT_LIST_CONT_W   (350)
#define IPTV_PLAYER_FORMAT_LIST_CONT_H   (60)//(40)

#define IPTV_PLAYER_FORMAT_LIST_X              (20)//(10)
#define IPTV_PLAYER_FORMAT_LIST_Y              (10)//(5)
#define IPTV_PLAYER_FORMAT_LIST_W              (IPTV_PLAYER_FORMAT_LIST_CONT_W - IPTV_PLAYER_FORMAT_LIST_X - IPTV_PLAYER_FORMAT_LIST_X)
#define IPTV_PLAYER_FORMAT_LIST_H              (IPTV_PLAYER_FORMAT_LIST_CONT_H - IPTV_PLAYER_FORMAT_LIST_Y - IPTV_PLAYER_FORMAT_LIST_Y)

#define IPTV_PLAYER_FORMAT_LIST_MIDL           (0)
#define IPTV_PLAYER_FORMAT_LIST_MIDT           (0)
#define IPTV_PLAYER_FORMAT_LIST_MIDR           (IPTV_PLAYER_FORMAT_LIST_MIDL + IPTV_PLAYER_FORMAT_LIST_W)
#define IPTV_PLAYER_FORMAT_LIST_MIDB           (IPTV_PLAYER_FORMAT_LIST_MIDT + IPTV_PLAYER_FORMAT_LIST_H)

#define IPTV_PLAYER_FORMAT_LIST_ICON_X                (0)
#define IPTV_PLAYER_FORMAT_LIST_ICON_Y                ((IPTV_PLAYER_FORMAT_LIST_H - IPTV_PLAYER_FORMAT_LIST_ICON_H) >> 1)
#define IPTV_PLAYER_FORMAT_LIST_ICON_W                (24)
#define IPTV_PLAYER_FORMAT_LIST_ICON_H                (24)

#define IPTV_PLAYER_FORMAT_LIST_NAME_X                (IPTV_PLAYER_FORMAT_LIST_ICON_X + IPTV_PLAYER_FORMAT_LIST_ICON_W)
#define IPTV_PLAYER_FORMAT_LIST_NAME_Y                (0)
#define IPTV_PLAYER_FORMAT_LIST_NAME_W                (46)
#define IPTV_PLAYER_FORMAT_LIST_NAME_H                (IPTV_PLAYER_FORMAT_LIST_H)

typedef enum
{
    MSG_HIDE_PANEL = MSG_LOCAL_BEGIN + 750,
    MSG_HIDE_PLAY_ICON,
    MSG_STOP_REPEAT_KEY,
    MSG_STOP,
    MSG_CHANGE_DEFINITION,
    MSG_CHANGE_ASPECT,
} ui_iptv_player_local_msg_t;

typedef struct
{ 
  u16 *pg_name;   //program name
  //u16 *sub_name;  //subject name
  //u8  *info_url;
  u32 vdo_id;
  u32 res_id;
  u32 play_time;
  u16 total_episode;
  u16 episode_num;
  u8  b_single_page;

  u8  category;
} ui_iptv_player_param;

#endif

