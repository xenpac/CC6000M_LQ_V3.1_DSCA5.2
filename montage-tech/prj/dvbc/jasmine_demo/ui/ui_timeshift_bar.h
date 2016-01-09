/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_TIMESHIFT_BAR_H__
#define __UI_TIMESHIFT_BAR_H__

/* coordinate */
#define TIMESHIFT_BAR_CONT_X             ((SCREEN_WIDTH - TIMESHIFT_BAR_CONT_W) / 2)
#define TIMESHIFT_BAR_CONT_Y             ((SCREEN_HEIGHT - TIMESHIFT_BAR_CONT_H) / 2)
#define TIMESHIFT_BAR_CONT_W             (COMM_BG_W)
#define TIMESHIFT_BAR_CONT_H             (COMM_BG_H)

/* TS logo*/
#define TIMESHIFT_BAR_TS_LOGO_X             60
#define TIMESHIFT_BAR_TS_LOGO_Y             30
#define TIMESHIFT_BAR_TS_LOGO_W             100
#define TIMESHIFT_BAR_TS_LOGO_H             50

/* special play display*/
#define TIMESHIFT_BAR_SPECIAL_PLAY_IMG_X     (TIMESHIFT_BAR_TS_LOGO_X + TIMESHIFT_BAR_TS_LOGO_W + 20)
#define TIMESHIFT_BAR_SPECIAL_PLAY_IMG_Y     TIMESHIFT_BAR_TS_LOGO_Y
#define TIMESHIFT_BAR_SPECIAL_PLAY_IMG_W     140
#define TIMESHIFT_BAR_SPECIAL_PLAY_IMG_H     TIMESHIFT_BAR_TS_LOGO_H
/* special play text*/
#define TIMESHIFT_BAR_SPECIAL_PLAY_TXT_X     (TIMESHIFT_BAR_SPECIAL_PLAY_IMG_X + 54)
#define TIMESHIFT_BAR_SPECIAL_PLAY_TXT_Y     TIMESHIFT_BAR_SPECIAL_PLAY_IMG_Y
#define TIMESHIFT_BAR_SPECIAL_PLAY_TXT_W     90
#define TIMESHIFT_BAR_SPECIAL_PLAY_TXT_H     TIMESHIFT_BAR_SPECIAL_PLAY_IMG_H

/* No signal logo*/
#define TIMESHIFT_BAR_TS_NO_SIGNAL_X            ( TIMESHIFT_BAR_TS_LOGO_H + 10)
#define TIMESHIFT_BAR_TS_NO_SIGNAL_Y             0
#define TIMESHIFT_BAR_TS_NO_SIGNAL_W             150
#define TIMESHIFT_BAR_TS_NO_SIGNAL_H             50

//bar frame
#define TIMESHIFT_BAR_FRM_X             ((TIMESHIFT_BAR_CONT_W - TIMESHIFT_BAR_FRM_W)/2)
#define TIMESHIFT_BAR_FRM_Y             (TIMESHIFT_BAR_CONT_H - TIMESHIFT_BAR_FRM_H - 48)
#define TIMESHIFT_BAR_FRM_W              960
#define TIMESHIFT_BAR_FRM_H              231

#define TIMESHIFT_BAR_ICON_X             39
#define TIMESHIFT_BAR_ICON_Y             0
#define TIMESHIFT_BAR_ICON_W             76
#define TIMESHIFT_BAR_ICON_H             70

#define TIMESHIFT_BAR_PLAY_STAT_ICON_X    260
#define TIMESHIFT_BAR_PLAY_STAT_ICON_Y    150
#define TIMESHIFT_BAR_PLAY_STAT_ICON_W    36
#define TIMESHIFT_BAR_PLAY_STAT_ICON_H    36

#define TIMESHIFT_BAR_TXT_TS_X    850
#define TIMESHIFT_BAR_TXT_TS_Y    TIMESHIFT_BAR_PLAY_STAT_ICON_Y
#define TIMESHIFT_BAR_TXT_TS_W    56
#define TIMESHIFT_BAR_TXT_TS_H    36

#define TIMESHIFT_BAR_PLAY_SPEED_X    110
#define TIMESHIFT_BAR_PLAY_SPEED_Y    110
#define TIMESHIFT_BAR_PLAY_SPEED_W    80
#define TIMESHIFT_BAR_PLAY_SPEED_H    36

#define TIMESHIFT_BAR_NAME_TXT_X        TIMESHIFT_BAR_ICON_X// 130
#define TIMESHIFT_BAR_NAME_TXT_Y         28
#define TIMESHIFT_BAR_NAME_TXT_W         360
#define TIMESHIFT_BAR_NAME_TXT_H         50//36

#define TIMESHIFT_BAR_DATE_ICON_X        630
#define TIMESHIFT_BAR_DATE_ICON_Y        28
#define TIMESHIFT_BAR_DATE_ICON_W        32
#define TIMESHIFT_BAR_DATE_ICON_H        32

#define TIMESHIFT_BAR_DATE_TXT_X         (TIMESHIFT_BAR_DATE_ICON_X + \
                                     TIMESHIFT_BAR_DATE_ICON_W)
#define TIMESHIFT_BAR_DATE_TXT_Y         TIMESHIFT_BAR_NAME_TXT_Y
#define TIMESHIFT_BAR_DATE_TXT_W         150
#define TIMESHIFT_BAR_DATE_TXT_H         32

#define TIMESHIFT_BAR_TIME_ICON_X        780
#define TIMESHIFT_BAR_TIME_ICON_Y        28
#define TIMESHIFT_BAR_TIME_ICON_W        32
#define TIMESHIFT_BAR_TIME_ICON_H        32

#define TIMESHIFT_BAR_TIME_TXT_X         (TIMESHIFT_BAR_TIME_ICON_X + \
                                     TIMESHIFT_BAR_TIME_ICON_W)
#define TIMESHIFT_BAR_TIME_TXT_Y         TIMESHIFT_BAR_NAME_TXT_Y
#define TIMESHIFT_BAR_TIME_TXT_W         70
#define TIMESHIFT_BAR_TIME_TXT_H         32

#define TIMESHIFT_BAR_CAPACITY_ICON_X        780
#define TIMESHIFT_BAR_CAPACITY_ICON_Y        100
#define TIMESHIFT_BAR_CAPACITY_ICON_W        28
#define TIMESHIFT_BAR_CAPACITY_ICON_H        50

#define TIMESHIFT_BAR_CAPACITY_TEXT_X        820
#define TIMESHIFT_BAR_CAPACITY_TEXT_Y        120
#define TIMESHIFT_BAR_CAPACITY_TEXT_W        70
#define TIMESHIFT_BAR_CAPACITY_TEXT_H        36

#define TIMESHIFT_BAR_RECORDING_ICON_X        780
#define TIMESHIFT_BAR_RECORDING_ICON_Y        140
#define TIMESHIFT_BAR_RECORDING_ICON_W        64
#define TIMESHIFT_BAR_RECORDING_ICON_H        32

#define TIMESHIFT_BAR_REC_CUR_TIME_X       170
#define TIMESHIFT_BAR_REC_CUR_TIME_Y       140
#define TIMESHIFT_BAR_REC_CUR_TIME_W       100
#define TIMESHIFT_BAR_REC_CUR_TIME_H       36

#define TIMESHIFT_BAR_PLAY_CUR_TIME_X      110
#define TIMESHIFT_BAR_PLAY_CUR_TIME_Y      120
#define TIMESHIFT_BAR_PLAY_CUR_TIME_W      120
#define TIMESHIFT_BAR_PLAY_CUR_TIME_H      36
/*
#define TIMESHIFT_BAR_PLAY_TOTAL_TIME_X      650
#define TIMESHIFT_BAR_PLAY_TOTAL_TIME_Y      120
#define TIMESHIFT_BAR_PLAY_TOTAL_TIME_W      120
#define TIMESHIFT_BAR_PLAY_TOTAL_TIME_H      36
*/
#define TIMESHIFT_BAR_REC_TOTAL_TIME_X       650
#define TIMESHIFT_BAR_REC_TOTAL_TIME_Y       120
#define TIMESHIFT_BAR_REC_TOTAL_TIME_W       120
#define TIMESHIFT_BAR_REC_TOTAL_TIME_H       36

#define TIMESHIFT_BAR_PLAY_TIME_X      200
#define TIMESHIFT_BAR_PLAY_TIME_Y      80
#define TIMESHIFT_BAR_PLAY_TIME_W      100
#define TIMESHIFT_BAR_PLAY_TIME_H      30

#define TIMESHIFT_BAR_PLAY_PROGRESS_X      242
#define TIMESHIFT_BAR_PLAY_PROGRESS_Y      115
#define TIMESHIFT_BAR_PLAY_PROGRESS_W      16
#define TIMESHIFT_BAR_PLAY_PROGRESS_H      15

#define TSFT_TRICK_X      242
#define TSFT_TRICK_Y      145
#define TSFT_TRICK_W      16
#define TSFT_TRICK_H      15

#define TIMESHIFT_BAR_PLAY_PROGRESS_MIDX 0
#define TIMESHIFT_BAR_PLAY_PROGRESS_MIDY 2
#define TIMESHIFT_BAR_PLAY_PROGRESS_MIDW TIMESHIFT_BAR_PLAY_PROGRESS_W
#define TIMESHIFT_BAR_PLAY_PROGRESS_MIDH (TIMESHIFT_BAR_PLAY_PROGRESS_H-2*TIMESHIFT_BAR_PLAY_PROGRESS_MIDY)
#define TIMESHIFT_BAR_PLAY_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_RED
#define TIMESHIFT_BAR_PLAY_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define TIMESHIFT_BAR_PLAY_PROGRESS_MID  RSI_PROGRESS_BAR_MID_RED

#define TIMESHIFT_BAR_SEEK_POINT_X      242
#define TIMESHIFT_BAR_SEEK_POINT_Y      145
#define TIMESHIFT_BAR_SEEK_POINT_W      16
#define TIMESHIFT_BAR_SEEK_POINT_H      15
//#define TIMESHIFT_BAR_SEEK_POINT_MIDX 0
//#define TIMESHIFT_BAR_SEEK_POINT_MIDY 2
//#define TIMESHIFT_BAR_SEEK_POINT_MIDW TIMESHIFT_BAR_SEEK_POINT_W
//#define TIMESHIFT_BAR_SEEK_POINT_MIDH (TIMESHIFT_BAR_SEEK_POINT_H-2*TIMESHIFT_BAR_SEEK_POINT_MIDY)
//#define TIMESHIFT_BAR_SEEK_POINT_MIN  RSI_PROGRESS_BAR_MID_RED
//#define TIMESHIFT_BAR_SEEK_POINT_MAX  RSI_PROGRESS_BAR_BG
//#define TIMESHIFT_BAR_SEEK_POINT_MID  RSI_PROGRESS_BAR_MID_RED

#define TIMESHIFT_BAR_REC_PROGRESS_X      (242 + (TIMESHIFT_BAR_SEEK_POINT_W / 2))
#define TIMESHIFT_BAR_REC_PROGRESS_Y      128
#define TIMESHIFT_BAR_REC_PROGRESS_W      370
#define TIMESHIFT_BAR_REC_PROGRESS_H      20
#define TIMESHIFT_BAR_REC_PROGRESS_MIDX 0
#define TIMESHIFT_BAR_REC_PROGRESS_MIDY 2
#define TIMESHIFT_BAR_REC_PROGRESS_MIDW TIMESHIFT_BAR_REC_PROGRESS_W//(TIMESHIFT_BAR_REC_PROGRESS_W - TIMESHIFT_BAR_SEEK_POINT_W)
#define TIMESHIFT_BAR_REC_PROGRESS_MIDH (TIMESHIFT_BAR_REC_PROGRESS_H-2*TIMESHIFT_BAR_REC_PROGRESS_MIDY)
#define TIMESHIFT_BAR_REC_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_ORANGE
#define TIMESHIFT_BAR_REC_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define TIMESHIFT_BAR_REC_PROGRESS_MID  RSI_PROGRESS_BAR_MID_ORANGE
/*S bar*/
//#define RSI_PLIST_SBAR          RSI_SCROLL_BAR_BG
//#define RSI_PLIST_SBAR_MID      RSI_SCROLL_BAR_MID
/* rect style */
#define RSI_TIMESHIFT_BAR_FRM            RSI_INFOR_WIN

/* font style */
#define FSI_TIMESHIFT_BAR_DATE           FSI_WHITE
#define FSI_TIMESHIFT_BAR_INFO           FSI_WHITE
#define FSI_TIMESHIFT_PLAY_TIME        FSI_PROGLIST_BRIEF

#define FSI_TIMESHIFT_SPECIAL_PLAY_TEXT           FSI_WHITE
/* other */
#define TIMESHIFT_BAR_LIST_PAGE         8
#define TIMESHIFT_BAR_LIST_FIELD        4
#define TIMESHIFT_BAR_REC_PROGRESS_STEP TIMESHIFT_BAR_REC_PROGRESS_W

enum timeshift_bar_local_msg
{
	MSG_INFO = MSG_LOCAL_BEGIN + 750,
	MSG_OK,
	MSG_HIDE_BAR,
	MSG_EXIT_TO_MENU,
	MSG_ONE_SECOND_ARRIVE,
	MSG_TS_PLAY_PAUSE,
	MSG_TS_PLAY_STOP,
	MSG_TS_PLAY_FB,         //fast backward
	MSG_TS_PLAY_FF,         //fast forward
	MSG_TS_PLAY_SF,         //slow forward
	MSG_TS_PLAY_SB,         //slow backwrd
	//MSG_TS_PLAY_TSF,        //time seek forward
	//MSG_TS_PLAY_TSB,        //time seek backward
	MSG_TS_BEGIN,
	MSG_TS_END,
	MSG_TS_VOLDOWN,
	MSG_TS_VOLUP,
};

typedef struct
{
  s32 h_play;
  s32 h_rec;
  u32 data_size;
  u16 file_name[256]; 
  u32 file_name_len;
  BOOL b_scramble;
  BOOL b_showbar;
  u32 audio_pid;
  u8 audio_type;
  u32 video_pid;
  u8 video_type;
  u32 pcr_pid;
  u16 jump_time;
} ui_pvr_private_info_t;

RET_CODE open_timeshift_bar(u32 para1, u32 para2);
ui_pvr_private_info_t * ui_timeshift_para_get(void);
BOOL ui_timeshift_switch_get(void);
u32 ui_timeshift_get_rec_time(void);
void reset_pvr_para(void);
#endif

