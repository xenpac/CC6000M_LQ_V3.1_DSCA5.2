/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PVR_PLAY_BAR_H__
#define __UI_PVR_PLAY_BAR_H__

/* coordinate */
#define PVR_PLAY_BAR_CONT_X             ((SCREEN_WIDTH - PVR_PLAY_BAR_CONT_W) / 2)
#define PVR_PLAY_BAR_CONT_Y             ((SCREEN_HEIGHT - PVR_PLAY_BAR_CONT_H) / 2)
#define PVR_PLAY_BAR_CONT_W             (COMM_BG_W)
#define PVR_PLAY_BAR_CONT_H             (COMM_BG_H)

//list container
#define PVR_PLAY_BAR_LIST_CONT_X        ((PVR_PLAY_BAR_CONT_W - PVR_PLAY_BAR_LIST_W)/2)
#define PVR_PLAY_BAR_LIST_CONT_Y        40
#define PVR_PLAY_BAR_LIST_CONT_W        720
#define PVR_PLAY_BAR_LIST_CONT_H        340

//list title
#define PVR_PLAY_BAR_TITLE_X             ((PVR_PLAY_BAR_LIST_CONT_W - PVR_PLAY_BAR_TITLE_W) / 2)
#define PVR_PLAY_BAR_TITLE_Y             10
#define PVR_PLAY_BAR_TITLE_W             350
#define PVR_PLAY_BAR_TITLE_H             40

//list
#define PVR_PLAY_BAR_LIST_X             20
#define PVR_PLAY_BAR_LIST_Y             60
#define PVR_PLAY_BAR_LIST_W            650 
#define PVR_PLAY_BAR_LIST_H             270

#define PVR_PLAY_BAR_LIST_MIDL          0
#define PVR_PLAY_BAR_LIST_MIDT          10
#define PVR_PLAY_BAR_LIST_MIDW          (PVR_PLAY_BAR_LIST_W - 2 * PVR_PLAY_BAR_LIST_MIDL)
#define PVR_PLAY_BAR_LIST_MIDH          (PVR_PLAY_BAR_LIST_H - 2 * PVR_PLAY_BAR_LIST_MIDT)

#define PVR_PLAY_BAR_LIST_VGAP          0

//list scroll
#define PVR_PLAY_BAR_LIST_SBAR_X         (PVR_PLAY_BAR_LIST_X + PVR_PLAY_BAR_LIST_W + 10)
#define PVR_PLAY_BAR_LIST_SBAR_Y         PVR_PLAY_BAR_LIST_Y
#define PVR_PLAY_BAR_LIST_SBAR_W         6
#define PVR_PLAY_BAR_LIST_SBAR_H         PVR_PLAY_BAR_LIST_H

//bar frame
#define PVR_PLAY_BAR_FRM_X             ((PVR_PLAY_BAR_CONT_W - PVR_PLAY_BAR_FRM_W)/2)
#define PVR_PLAY_BAR_FRM_Y             (PVR_PLAY_BAR_CONT_H - PVR_PLAY_BAR_FRM_H - 10)
#define PVR_PLAY_BAR_FRM_W              960
#define PVR_PLAY_BAR_FRM_H              200

//seek duration time edit
#define PVR_PLAY_DUR_FRM_W              350
#define PVR_PLAY_DUR_TIME_X             ((PVR_PLAY_BAR_CONT_W - PVR_PLAY_DUR_FRM_W) / 2 - 40)
#define PVR_PLAY_DUR_TIME_Y             220
#define PVR_PLAY_DUR_TIME_LW            120
#define PVR_PLAY_DUR_TIME_RW            (PVR_PLAY_DUR_FRM_W - PVR_PLAY_DUR_TIME_LW)

#define PVR_PLAY_BAR_ICON_X             39
#define PVR_PLAY_BAR_ICON_Y             0
#define PVR_PLAY_BAR_ICON_W             76
#define PVR_PLAY_BAR_ICON_H             70

#define PVR_PLAY_BAR_PLAY_STAT_ICON_X    270//260
#define PVR_PLAY_BAR_PLAY_STAT_ICON_Y    130
#define PVR_PLAY_BAR_PLAY_STAT_ICON_W    36
#define PVR_PLAY_BAR_PLAY_STAT_ICON_H    36

#define PVR_PLAY_BAR_PLAY_SPEED_X    ((PVR_PLAY_BAR_FRM_W - PVR_PLAY_BAR_PLAY_SPEED_W)/2)
#define PVR_PLAY_BAR_PLAY_SPEED_Y    PVR_PLAY_BAR_PLAY_STAT_ICON_Y
#define PVR_PLAY_BAR_PLAY_SPEED_W    40
#define PVR_PLAY_BAR_PLAY_SPEED_H    36

#define PVR_PLAY_BAR_NAME_TXT_X         PVR_PLAY_BAR_ICON_X//129
#define PVR_PLAY_BAR_NAME_TXT_Y         25
#define PVR_PLAY_BAR_NAME_TXT_W         340
#define PVR_PLAY_BAR_NAME_TXT_H         50//36

#define PVR_PLAY_BAR_DATE_ICON_X        640
#define PVR_PLAY_BAR_DATE_ICON_Y        30
#define PVR_PLAY_BAR_DATE_ICON_W        32
#define PVR_PLAY_BAR_DATE_ICON_H        32

#define PVR_PLAY_BAR_DATE_TXT_X         (PVR_PLAY_BAR_DATE_ICON_X + \
                                     PVR_PLAY_BAR_DATE_ICON_W +2)
#define PVR_PLAY_BAR_DATE_TXT_Y         (PVR_PLAY_BAR_NAME_TXT_Y +2)
#define PVR_PLAY_BAR_DATE_TXT_W         123
#define PVR_PLAY_BAR_DATE_TXT_H         36

#define PVR_PLAY_BAR_TIME_ICON_X        776//716
#define PVR_PLAY_BAR_TIME_ICON_Y        25
#define PVR_PLAY_BAR_TIME_ICON_W        32
#define PVR_PLAY_BAR_TIME_ICON_H        32

#define PVR_PLAY_BAR_TIME_TXT_X         (PVR_PLAY_BAR_TIME_ICON_X + \
                                     PVR_PLAY_BAR_TIME_ICON_W +2)
#define PVR_PLAY_BAR_TIME_TXT_Y         (PVR_PLAY_BAR_NAME_TXT_Y +2)
#define PVR_PLAY_BAR_TIME_TXT_W         100
#define PVR_PLAY_BAR_TIME_TXT_H         36

#define PVR_PLAY_BAR_CAPACITY_ICON_X        800
#define PVR_PLAY_BAR_CAPACITY_ICON_Y        98
#define PVR_PLAY_BAR_CAPACITY_ICON_W        75
#define PVR_PLAY_BAR_CAPACITY_ICON_H        16

#define PVR_PLAY_BAR_CAPACITY_TEXT_X        800
#define PVR_PLAY_BAR_CAPACITY_TEXT_Y        120
#define PVR_PLAY_BAR_CAPACITY_TEXT_W        75
#define PVR_PLAY_BAR_CAPACITY_TEXT_H        36

#define PVR_PLAY_BAR_RECORDING_ICON_X        780
#define PVR_PLAY_BAR_RECORDING_ICON_Y        140
#define PVR_PLAY_BAR_RECORDING_ICON_W        64
#define PVR_PLAY_BAR_RECORDING_ICON_H        32

#define PVR_PLAY_BAR_REC_CUR_TIME_X       170
#define PVR_PLAY_BAR_REC_CUR_TIME_Y       140
#define PVR_PLAY_BAR_REC_CUR_TIME_W       100
#define PVR_PLAY_BAR_REC_CUR_TIME_H       36

#define PVR_PLAY_BAR_PLAY_CUR_TIME_X      150
#define PVR_PLAY_BAR_PLAY_CUR_TIME_Y      90
#define PVR_PLAY_BAR_PLAY_CUR_TIME_W      110
#define PVR_PLAY_BAR_PLAY_CUR_TIME_H      36

#define PVR_PLAY_BAR_PLAY_TOTAL_TIME_X      660
#define PVR_PLAY_BAR_PLAY_TOTAL_TIME_Y      90
#define PVR_PLAY_BAR_PLAY_TOTAL_TIME_W      110
#define PVR_PLAY_BAR_PLAY_TOTAL_TIME_H      36

#define PVR_PLAY_BAR_REC_TOTAL_TIME_X       670
#define PVR_PLAY_BAR_REC_TOTAL_TIME_Y       140
#define PVR_PLAY_BAR_REC_TOTAL_TIME_W       100
#define PVR_PLAY_BAR_REC_TOTAL_TIME_H       36

#define PVR_PLAY_BAR_PLAY_PROGRESS_X      270
#define PVR_PLAY_BAR_PLAY_PROGRESS_Y      98
#define PVR_PLAY_BAR_PLAY_PROGRESS_W      370
#define PVR_PLAY_BAR_PLAY_PROGRESS_H      20
#define PVR_PLAY_BAR_PLAY_PROGRESS_MIDX 0
#define PVR_PLAY_BAR_PLAY_PROGRESS_MIDY 2
#define PVR_PLAY_BAR_PLAY_PROGRESS_MIDW PVR_PLAY_BAR_PLAY_PROGRESS_W
#define PVR_PLAY_BAR_PLAY_PROGRESS_MIDH (PVR_PLAY_BAR_PLAY_PROGRESS_H-2*PVR_PLAY_BAR_PLAY_PROGRESS_MIDY)
#define PVR_PLAY_BAR_PLAY_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_BLUE
#define PVR_PLAY_BAR_PLAY_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define PVR_PLAY_BAR_PLAY_PROGRESS_MID  RSI_PROGRESS_BAR_MID_BLUE

#define PVR_PLAY_BAR_TRICK_X      PVR_PLAY_BAR_PLAY_PROGRESS_X
#define PVR_PLAY_BAR_TRICK_Y      (PVR_PLAY_BAR_PLAY_PROGRESS_Y + PVR_PLAY_BAR_PLAY_PROGRESS_H + 2)
#define PVR_PLAY_BAR_TRICK_W      16
#define PVR_PLAY_BAR_TRICK_H      15

#define PVR_PLAY_BAR_REC_PROGRESS_X      280
#define PVR_PLAY_BAR_REC_PROGRESS_Y      151
#define PVR_PLAY_BAR_REC_PROGRESS_W      370
#define PVR_PLAY_BAR_REC_PROGRESS_H      20
#define PVR_PLAY_BAR_REC_PROGRESS_MIDX 0
#define PVR_PLAY_BAR_REC_PROGRESS_MIDY 2
#define PVR_PLAY_BAR_REC_PROGRESS_MIDW PVR_PLAY_BAR_REC_PROGRESS_W
#define PVR_PLAY_BAR_REC_PROGRESS_MIDH (PVR_PLAY_BAR_REC_PROGRESS_H-2*PVR_PLAY_BAR_REC_PROGRESS_MIDY)
#define PVR_PLAY_BAR_REC_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_RED
#define PVR_PLAY_BAR_REC_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define PVR_PLAY_BAR_REC_PROGRESS_MID  RSI_PROGRESS_BAR_MID_RED

/* rect style */
#define RSI_PVR_PLAY_BAR_FRM            RSI_INFOR_WIN

/* font style */
#define FSI_PVR_PLAY_BAR_NAME          FSI_WHITE_32
#define FSI_PVR_PLAY_BAR_DATE           FSI_WHITE
#define FSI_PVR_PLAY_BAR_INFO           FSI_WHITE

/* other */
#define PVR_PLAY_BAR_LIST_PAGE        6// 8
#define PVR_PLAY_BAR_LIST_FIELD       5// 4

/*password dlg*/
#define PVR_PLAY_BAR_PWD_DLG_FOR_CHK_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define PVR_PLAY_BAR_PWD_DLG_FOR_CHK_Y      ((SCREEN_HEIGHT-PWDLG_H)/2)

typedef struct
{
  mul_pvr_play_speed_t  play_speed;
  u16                   value;
}pvr_trick_mode_t;

typedef struct
{
  u32 record_play_id;                       /* record program play id */
  u32 audio_channel;                        /* default audio channel */
  u32 audio_track;                          /* 0: stereo, 1: left, 2: right, 3: mono */
  u32 audio_channel_total;                  /* multiple audio total */
  audio_t audio[DB_DVBS_MAX_AUDIO_CHANNEL]; /* multiple audio */
  u32 total_time;                           /* total time (second) */
  u32 total_size;                           /* total size */
  u16 program_name[PROGRAM_NAME_MAX];       /* program name */
  
}pvr_audio_info_t;


RET_CODE open_pvr_play_bar(u32 para1, u32 para2);

#endif

