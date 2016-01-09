/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NETWORK_PLAYBAR_H__
#define __UI_NETWORK_PLAYBAR_H__

/* coordinate */
#define NETPLAY_BAR_DLNA_CONT_X             (0)
#define NETPLAY_BAR_DLNA_CONT_Y             (0)
#define NETPLAY_BAR_DLNA_CONT_W             (SCREEN_WIDTH)
#define NETPLAY_BAR_DLNA_CONT_H             (SCREEN_HEIGHT)//260

//bar frame
#define NETPLAY_BAR_DLNA_FRM_X              NETPLAY_BAR_DLNA_CONT_X//(200)
#define NETPLAY_BAR_DLNA_FRM_Y              (NETPLAY_BAR_DLNA_CONT_H - NETPLAY_BAR_DLNA_FRM_H)
#define NETPLAY_BAR_DLNA_FRM_W             ( NETPLAY_BAR_DLNA_CONT_W -2 *NETPLAY_BAR_DLNA_FRM_X)
#define NETPLAY_BAR_DLNA_FRM_H              100

#if 0
//bar title container
#define NETPLAY_BAR_TITLE_X             0
#define NETPLAY_BAR_TITLE_Y             0
#define NETPLAY_BAR_TITLE_W             NETPLAY_BAR_FRM_W
#define NETPLAY_BAR_TITLE_H             60
#endif

#define NETPLAY_BAR_DLNA_ICON_X             10//39
#define NETPLAY_BAR_DLNA_ICON_Y             0
#define NETPLAY_BAR_DLNA_ICON_W             110
#define NETPLAY_BAR_DLNA_ICON_H             50//70

#define NETPLAY_BAR_DLNA_PLAY_STAT_ICON_X    60//132
#define NETPLAY_BAR_DLNA_PLAY_STAT_ICON_Y    28//58//110//140
#define NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W    36
#define NETPLAY_BAR_DLNA_PLAY_STAT_ICON_H    36

#define NETPLAY_BAR_DLNA_NAME_TXT_X         130
#define NETPLAY_BAR_DLNA_NAME_TXT_Y         28
#define NETPLAY_BAR_DLNA_NAME_TXT_W         800//360
#define NETPLAY_BAR_DLNA_NAME_TXT_H         36

#define NETPLAY_BAR_DLNA_PROGRESS_X      142
#define NETPLAY_BAR_DLNA_PROGRESS_Y      38//108
#define NETPLAY_BAR_DLNA_PROGRESS_W      860
#define NETPLAY_BAR_DLNA_PROGRESS_H      20
#define NETPLAY_BAR_DLNA_PROGRESS_MIDX 0
#define NETPLAY_BAR_DLNA_PROGRESS_MIDY 2
#define NETPLAY_BAR_DLNA_PROGRESS_MIDW NETPLAY_BAR_DLNA_PROGRESS_W
#define NETPLAY_BAR_DLNA_PROGRESS_MIDH (NETPLAY_BAR_DLNA_PROGRESS_H-2*NETPLAY_BAR_DLNA_PROGRESS_MIDY)
#define NETPLAY_BAR_DLNA_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_BLUE
#define NETPLAY_BAR_DLNA_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define NETPLAY_BAR_DLNA_PROGRESS_MID  INVALID_RSTYLE_IDX

#define NETPLAY_BAR_DLNA_PLAY_CUR_TIME_X      (NETPLAY_BAR_DLNA_PROGRESS_X +NETPLAY_BAR_DLNA_PROGRESS_W +20)
#define NETPLAY_BAR_DLNA_PLAY_CUR_TIME_Y      30//100
#define NETPLAY_BAR_DLNA_PLAY_CUR_TIME_W      80
#define NETPLAY_BAR_DLNA_PLAY_CUR_TIME_H      36

#define NETPLAY_BAR_DLNA_TOTAL_TIME_X       (NETPLAY_BAR_DLNA_PLAY_CUR_TIME_X + NETPLAY_BAR_DLNA_PLAY_CUR_TIME_W + 10)
#define NETPLAY_BAR_DLNA_TOTAL_TIME_Y      30// 100
#define NETPLAY_BAR_DLNA_TOTAL_TIME_W       100
#define NETPLAY_BAR_DLNA_TOTAL_TIME_H       36

#define NETPLAY_DLNA_TRICK_X      242
#define NETPLAY_DLNA_TRICK_Y      25//125 
#define NETPLAY_DLNA_TRICK_W      16
#define NETPLAY_DLNA_TRICK_H      15

#define NETPLAY_BAR_DLNA_PROGRESS_STEP NETPLAY_BAR_DLNA_PROGRESS_W

enum netplay_bar_dlna_local_msg
{
    MSG_NP_DLNA_INFO = MSG_LOCAL_BEGIN + 750,
    MSG_NP_DLNA_OK,
    MSG_NP_DLNA_HIDE_BAR,
    MSG_NP_DLNA_EXIT,
    //MSG_ONE_SECOND_ARRIVE,
    MSG_NP_DLNA_PLAY_PAUSE,
    MSG_NP_DLNA_PLAY_STOP,
    MSG_NP_DLNA_AUDIO_SET,
    MSG_NP_DLNA_PLAY_SEEK,
#if ENABLE_TRICK_PLAY_FOR_NET	
    MSG_NP_DLNA_PLAY_FB,         //fast backward
    MSG_NP_DLNA_PLAY_FF,         //fast forward
    MSG_NP_DLNA_PLAY_SF,         //slow forward
    MSG_NP_DLNA_PLAY_SB,         //slow backwrd
#endif	
    MSG_NP_DLNA_PLAY_SUBT,
    //MSG_FP_PLAY_FPF,        //time seek forward
    //MSG_FP_PLAY_FPB,        //time seek backward
    MSG_NP_DLNA_BEGIN,
    MSG_NP_DLNA_END,
    MSG_NP_DLNA_SHOW_GOTO,
    MSG_UP_DLNA_NETBAR
};

typedef struct
{ 
  u8 *name;
  u8 *url;
} netplay_dlna_url;

RET_CODE open_network_playbar(u32 para1, u32 para2);

u32 dlna_video_goto_get_net_play_time(void);
u32 dlna_video_get_total_time(void);
RET_CODE dlna_on_exit_netplay_bar(control_t *p_cont, u16 msg, u32 para1, u32 para2);

#endif

