/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#ifndef __UI_VIDEO_V_FULL_SCREEN_H__
#define __UI_VIDEO_V_FULL_SCREEN_H__

/* coordinate */
#define FILEPLAY_BAR_CONT_X             ((SCREEN_WIDTH - FILEPLAY_BAR_CONT_W) / 2)
#define FILEPLAY_BAR_CONT_Y             (SCREEN_HEIGHT - FILEPLAY_BAR_CONT_H)//419
#define FILEPLAY_BAR_CONT_W             SCREEN_WIDTH
#define FILEPLAY_BAR_CONT_H             SCREEN_HEIGHT//260

/* special play display*/
#define FILEPLAY_BAR_SPECIAL_PLAY_IMG_X             0
#define FILEPLAY_BAR_SPECIAL_PLAY_IMG_Y             0
#define FILEPLAY_BAR_SPECIAL_PLAY_IMG_W             200
#define FILEPLAY_BAR_SPECIAL_PLAY_IMG_H             50
/* special play text*/
#define FILEPLAY_BAR_SPECIAL_PLAY_TXT_X             80
#define FILEPLAY_BAR_SPECIAL_PLAY_TXT_Y             0
#define FILEPLAY_BAR_SPECIAL_PLAY_TXT_W             120
#define FILEPLAY_BAR_SPECIAL_PLAY_TXT_H             50



//bar frame
#define FILEPLAY_BAR_FRM_X              ((FILEPLAY_BAR_CONT_W - FILEPLAY_BAR_FRM_W)/2)
#define FILEPLAY_BAR_FRM_Y              (FILEPLAY_BAR_CONT_H - FILEPLAY_BAR_FRM_H - 48)
#define FILEPLAY_BAR_FRM_W              960
#define FILEPLAY_BAR_FRM_H              231

//speed play frame
#define FILEPLAY_SPEED_PLAY_FRM_X              (COMM_BG_W - FILEPLAY_BAR_SPECIAL_PLAY_IMG_W - 105)
#define FILEPLAY_SPEED_PLAY_FRM_Y              30
#define FILEPLAY_SPEED_PLAY_FRM_W              FILEPLAY_BAR_SPECIAL_PLAY_IMG_W
#define FILEPLAY_SPEED_PLAY_FRM_H              FILEPLAY_BAR_SPECIAL_PLAY_IMG_H

#define FILEPLAY_BAR_ICON_X             39
#define FILEPLAY_BAR_ICON_Y             0
#define FILEPLAY_BAR_ICON_W             76
#define FILEPLAY_BAR_ICON_H             70

#define FILEPLAY_BAR_PLAY_STAT_ICON_X    232//260
#define FILEPLAY_BAR_PLAY_STAT_ICON_Y    140//200
#define FILEPLAY_BAR_PLAY_STAT_ICON_W    36
#define FILEPLAY_BAR_PLAY_STAT_ICON_H    36

#define FILEPLAY_BAR_TXT_FP_X    850
#define FILEPLAY_BAR_TXT_FP_Y    140//200
#define FILEPLAY_BAR_TXT_FP_W    46
#define FILEPLAY_BAR_TXT_FP_H    36

#define FILEPLAY_BAR_PLAY_SPEED_X    110
#define FILEPLAY_BAR_PLAY_SPEED_Y    90//110
#define FILEPLAY_BAR_PLAY_SPEED_W    80
#define FILEPLAY_BAR_PLAY_SPEED_H    36

#define FILEPLAY_BAR_NAME_TXT_X        FILEPLAY_BAR_ICON_X// 130
#define FILEPLAY_BAR_NAME_TXT_Y         28
#define FILEPLAY_BAR_NAME_TXT_W         800//360
#define FILEPLAY_BAR_NAME_TXT_H         50

#define FILEPLAY_BAR_DATE_ICON_X        600
#define FILEPLAY_BAR_DATE_ICON_Y        28
#define FILEPLAY_BAR_DATE_ICON_W        32
#define FILEPLAY_BAR_DATE_ICON_H        32

#define FILEPLAY_BAR_DATE_TXT_X         (FILEPLAY_BAR_DATE_ICON_X + \
                                     FILEPLAY_BAR_DATE_ICON_W)
#define FILEPLAY_BAR_DATE_TXT_Y         FILEPLAY_BAR_NAME_TXT_Y
#define FILEPLAY_BAR_DATE_TXT_W         150
#define FILEPLAY_BAR_DATE_TXT_H         32

#define FILEPLAY_BAR_TIME_ICON_X        780
#define FILEPLAY_BAR_TIME_ICON_Y        28
#define FILEPLAY_BAR_TIME_ICON_W        32
#define FILEPLAY_BAR_TIME_ICON_H        32

#define FILEPLAY_BAR_TIME_TXT_X         (FILEPLAY_BAR_TIME_ICON_X + \
                                     FILEPLAY_BAR_TIME_ICON_W)
#define FILEPLAY_BAR_TIME_TXT_Y         FILEPLAY_BAR_NAME_TXT_Y
#define FILEPLAY_BAR_TIME_TXT_W         70
#define FILEPLAY_BAR_TIME_TXT_H         32

#define FILEPLAY_CAPACITY_ICON_X        820
#define FILEPLAY_CAPACITY_ICON_Y        80//100
#define FILEPLAY_CAPACITY_ICON_W        100
#define FILEPLAY_CAPACITY_ICON_H        15

#define FILEPLAY_CAPACITY_TEXT_X        820
#define FILEPLAY_CAPACITY_TEXT_Y        100//120
#define FILEPLAY_CAPACITY_TEXT_W        70
#define FILEPLAY_CAPACITY_TEXT_H        36

#define FILEPLAY_BAR_PLAY_CUR_TIME_X      110
#define FILEPLAY_BAR_PLAY_CUR_TIME_Y      100//120
#define FILEPLAY_BAR_PLAY_CUR_TIME_W      120
#define FILEPLAY_BAR_PLAY_CUR_TIME_H      36

#define FILEPLAY_BAR_TOTAL_TIME_X       650
#define FILEPLAY_BAR_TOTAL_TIME_Y       100//120
#define FILEPLAY_BAR_TOTAL_TIME_W       120
#define FILEPLAY_BAR_TOTAL_TIME_H       36

#define FILEPLAY_BAR_PLAY_TIME_X      210
#define FILEPLAY_BAR_PLAY_TIME_Y      80
#define FILEPLAY_BAR_PLAY_TIME_W      60//80//120
#define FILEPLAY_BAR_PLAY_TIME_H      30

#define FILEPLAY_BAR_PLAY_PROGRESS_X      242
#define FILEPLAY_BAR_PLAY_PROGRESS_Y      95//115
#define FILEPLAY_BAR_PLAY_PROGRESS_W      16
#define FILEPLAY_BAR_PLAY_PROGRESS_H      15

#define FILEPLAY_BAR_PROGRESS_X      242
#define FILEPLAY_BAR_PROGRESS_Y      108//128
#define FILEPLAY_BAR_PROGRESS_W      370
#define FILEPLAY_BAR_PROGRESS_H      20
#define FILEPLAY_BAR_PROGRESS_MIDX 0
#define FILEPLAY_BAR_PROGRESS_MIDY 2
#define FILEPLAY_BAR_PROGRESS_MIDW FILEPLAY_BAR_PROGRESS_W
#define FILEPLAY_BAR_PROGRESS_MIDH (FILEPLAY_BAR_PROGRESS_H-2*FILEPLAY_BAR_PROGRESS_MIDY)
#define FILEPLAY_BAR_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_ORANGE
#define FILEPLAY_BAR_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define FILEPLAY_BAR_PROGRESS_MID  RSI_PROGRESS_BAR_MID_ORANGE

#define FILEPLAY_TRICK_X      242
#define FILEPLAY_TRICK_Y      125//145
#define FILEPLAY_TRICK_W      16
#define FILEPLAY_TRICK_H      15

//help
#define FILEPLAY_HELP_IMG_X      (FILEPLAY_HELP_TXT_X-FILEPLAY_HELP_IMG_W)
#define FILEPLAY_HELP_IMG_Y      (FILEPLAY_HELP_TXT_Y+10)
#define FILEPLAY_HELP_IMG_W      38
#define FILEPLAY_HELP_IMG_H      20

#define FILEPLAY_HELP_TXT_X      (FILEPLAY_BAR_FRM_W-FILEPLAY_HELP_TXT_W)
#define FILEPLAY_HELP_TXT_Y      (FILEPLAY_BAR_FRM_H-FILEPLAY_HELP_TXT_H-30)
#define FILEPLAY_HELP_TXT_W      200
#define FILEPLAY_HELP_TXT_H      36

/* rect style */
#define RSI_FILEPLAY_BAR_FRM            RSI_INFOR_WIN

/* font style */
#define FSI_FILEPLAY_BAR_NAME           FSI_WHITE_32
#define FSI_FILEPLAY_BAR_DATE           FSI_WHITE
#define FSI_FILEPLAY_BAR_INFO           FSI_WHITE//FSI_GRAY
#define FSI_FILEPLAY_PLAY_TIME        FSI_PROGLIST_BRIEF

#define FSI_FILEPLAY_SPECIAL_PLAY_TEXT           FSI_WHITE
/* other */
#define FILEPLAY_BAR_LIST_PAGE         8
#define FILEPLAY_BAR_LIST_FIELD        4
#define FILEPLAY_BAR_PROGRESS_STEP FILEPLAY_BAR_PROGRESS_W

enum fileplay_bar_local_msg
{
  MSG_FP_INFO = MSG_LOCAL_BEGIN + 750,
  MSG_FP_OK,
  MSG_FP_HIDE_BAR,
  MSG_FP_EXIT,
  //MSG_ONE_SECOND_ARRIVE,
  MSG_FP_PLAY_PAUSE,
  MSG_FP_PLAY_STOP,
  MSG_FP_AUDIO_SET,
  MSG_FP_PLAY_PREV,        //Prev
  MSG_FP_PLAY_NEXT,        //Next
  MSG_FP_PLAY_FB,         //fast backward
  MSG_FP_PLAY_FF,         //fast forward
  MSG_FP_PLAY_SF,         //slow forward
  MSG_FP_PLAY_SB,         //slow backwrd
  MSG_FP_PLAY_SUBT,
  //MSG_FP_PLAY_FPF,        //time seek forward
  //MSG_FP_PLAY_FPB,        //time seek backward
  MSG_FP_BEGIN,
  MSG_FP_END,
  MSG_SHOW_GOTO,
  MSG_UP_FILEBAR,
  MSG_RED,
  MSG_GREEN,
  MSG_FP_DISAPPEAR_UNSUPPORT
};

/*
typedef struct
{
  BOOL b_showbar;
} ui_fp_info_t;
*/
RET_CODE ui_video_v_fscreen_open(u32 para1, u32 para2);

//ui_pvr_private_info_t * ui_fileplay_para_get(void);
BOOL ui_fileplay_switch_get();

u32 video_goto_get_file_play_time(void);
#endif


