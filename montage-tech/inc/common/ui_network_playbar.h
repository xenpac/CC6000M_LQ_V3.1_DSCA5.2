/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NETWORK_PLAYBAR_H__
#define __UI_NETWORK_PLAYBAR_H__

/* coordinate */
#define NETPLAY_BAR_CONT_X             (0)
#define NETPLAY_BAR_CONT_Y             (0)
#define NETPLAY_BAR_CONT_W             (SCREEN_WIDTH)
#define NETPLAY_BAR_CONT_H             (SCREEN_HEIGHT)//260

//bar frame
#define NETPLAY_BAR_FRM_X              30
#define NETPLAY_BAR_FRM_Y              330
#define NETPLAY_BAR_FRM_W              900
#define NETPLAY_BAR_FRM_H              230

//bar title container
#define NETPLAY_BAR_TITLE_X             0
#define NETPLAY_BAR_TITLE_Y             0
#define NETPLAY_BAR_TITLE_W             NETPLAY_BAR_FRM_W
#define NETPLAY_BAR_TITLE_H             60

#define NETPLAY_BAR_ICON_X             10
#define NETPLAY_BAR_ICON_Y             0
#define NETPLAY_BAR_ICON_W             76
#define NETPLAY_BAR_ICON_H             60

#define NETPLAY_BAR_PLAY_STAT_ICON_X    240
#define NETPLAY_BAR_PLAY_STAT_ICON_Y    130
#define NETPLAY_BAR_PLAY_STAT_ICON_W    36
#define NETPLAY_BAR_PLAY_STAT_ICON_H    36

#define NETPLAY_BAR_NAME_TXT_X         99
#define NETPLAY_BAR_NAME_TXT_Y         15
#define NETPLAY_BAR_NAME_TXT_W         800//360
#define NETPLAY_BAR_NAME_TXT_H         36

#define NETPLAY_BAR_PLAY_CUR_TIME_X      120
#define NETPLAY_BAR_PLAY_CUR_TIME_Y      90
#define NETPLAY_BAR_PLAY_CUR_TIME_W      110
#define NETPLAY_BAR_PLAY_CUR_TIME_H      36

#define NETPLAY_BAR_TOTAL_TIME_X       630
#define NETPLAY_BAR_TOTAL_TIME_Y       90
#define NETPLAY_BAR_TOTAL_TIME_W       110
#define NETPLAY_BAR_TOTAL_TIME_H       36

#define NETPLAY_BAR_PROGRESS_X      240
#define NETPLAY_BAR_PROGRESS_Y      98
#define NETPLAY_BAR_PROGRESS_W      370
#define NETPLAY_BAR_PROGRESS_H      20
#define NETPLAY_BAR_PROGRESS_MIDX 0
#define NETPLAY_BAR_PROGRESS_MIDY 2
#define NETPLAY_BAR_PROGRESS_MIDW NETPLAY_BAR_PROGRESS_W
#define NETPLAY_BAR_PROGRESS_MIDH (NETPLAY_BAR_PROGRESS_H-2*NETPLAY_BAR_PROGRESS_MIDY)
#define NETPLAY_BAR_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_BLUE
#define NETPLAY_BAR_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define NETPLAY_BAR_PROGRESS_MID  INVALID_RSTYLE_IDX

#define NETPLAY_TRICK_X      240
#define NETPLAY_TRICK_Y      115 
#define NETPLAY_TRICK_W      16
#define NETPLAY_TRICK_H      15

#define NETPLAY_BAR_PROGRESS_STEP NETPLAY_BAR_PROGRESS_W

enum netplay_bar_local_msg
{
    MSG_NP_INFO = MSG_LOCAL_BEGIN + 750,
    MSG_NP_OK,
    MSG_NP_HIDE_BAR,
    MSG_NP_EXIT,
    //MSG_ONE_SECOND_ARRIVE,
    MSG_NP_PLAY_PAUSE,
    MSG_NP_PLAY_STOP,
    MSG_NP_AUDIO_SET,
#ifdef ENABLE_TRICK_PLAY	
    MSG_NP_PLAY_FB,         //fast backward
    MSG_NP_PLAY_FF,         //fast forward
    MSG_NP_PLAY_SF,         //slow forward
    MSG_NP_PLAY_SB,         //slow backwrd
#endif	
    MSG_NP_PLAY_SUBT,
    //MSG_FP_PLAY_FPF,        //time seek forward
    //MSG_FP_PLAY_FPB,        //time seek backward
    MSG_NP_BEGIN,
    MSG_NP_END,
    MSG_NP_SHOW_GOTO,
    MSG_UP_NETBAR
};

typedef struct
{ 
  u8 *name;
  u8 *url;
} netplay_url;

RET_CODE open_network_playbar(u32 para1, u32 para2);

u32 video_goto_get_net_play_time(void);

#endif

