/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PVR_REC_BAR_H__
#define __UI_PVR_REC_BAR_H__

/* coordinate */
#define PVR_REC_BAR_CONT_X             ((SCREEN_WIDTH - PVR_REC_BAR_CONT_W) / 2)
#define PVR_REC_BAR_CONT_Y             ((SCREEN_HEIGHT - PVR_REC_BAR_CONT_H) / 2)
#define PVR_REC_BAR_CONT_W             (COMM_BG_W)
#define PVR_REC_BAR_CONT_H             (COMM_BG_H)

//record flag container
#define PVR_REC_FLAG_FRM_X             (PVR_REC_BAR_CONT_W - PVR_REC_FLAG_FRM_W - 180)
#define PVR_REC_FLAG_FRM_Y             30
#define PVR_REC_FLAG_FRM_W             76
#define PVR_REC_FLAG_FRM_H             56

//duration time container
#define PVR_REC_DUR_FRM_W              350
#define PVR_REC_DUR_TIME_X             ((PVR_REC_BAR_CONT_W - PVR_REC_DUR_FRM_W) / 2 - 40)
#define PVR_REC_DUR_TIME_Y             220
#define PVR_REC_DUR_TIME_LW            150
#define PVR_REC_DUR_TIME_RW            (PVR_REC_DUR_FRM_W - PVR_REC_DUR_TIME_LW)

//bar container
#define PVR_REC_BAR_FRM_X              ((COMM_BG_W - PVR_REC_BAR_FRM_W)/2)
#define PVR_REC_BAR_FRM_Y              (COMM_BG_H - PVR_REC_BAR_FRM_H)
#define PVR_REC_BAR_FRM_W              960
#define PVR_REC_BAR_FRM_H              231

#define PVR_REC_BAR_ICON_X             39
#define PVR_REC_BAR_ICON_Y             0
#define PVR_REC_BAR_ICON_W             76
#define PVR_REC_BAR_ICON_H             70

#define PVR_REC_BAR_PLAY_STAT_ICON_X    66
#define PVR_REC_BAR_PLAY_STAT_ICON_Y    110
#define PVR_REC_BAR_PLAY_STAT_ICON_W    36
#define PVR_REC_BAR_PLAY_STAT_ICON_H    36

#define PVR_REC_BAR_PLAY_SPEED_X    880
#define PVR_REC_BAR_PLAY_SPEED_Y    148
#define PVR_REC_BAR_PLAY_SPEED_W    64
#define PVR_REC_BAR_PLAY_SPEED_H    30

#define PVR_REC_BAR_NAME_TXT_X         229
#define PVR_REC_BAR_NAME_TXT_Y         25
#define PVR_REC_BAR_NAME_TXT_W         340
#define PVR_REC_BAR_NAME_TXT_H         50//36

#define PVR_REC_BAR_DATE_ICON_X        670
#define PVR_REC_BAR_DATE_ICON_Y        28
#define PVR_REC_BAR_DATE_ICON_W        32
#define PVR_REC_BAR_DATE_ICON_H        32

#define PVR_REC_BAR_DATE_TXT_X         (PVR_REC_BAR_DATE_ICON_X + \
                                     PVR_REC_BAR_DATE_ICON_W)
#define PVR_REC_BAR_DATE_TXT_Y         PVR_REC_BAR_NAME_TXT_Y
#define PVR_REC_BAR_DATE_TXT_W         150
#define PVR_REC_BAR_DATE_TXT_H         36

#define PVR_REC_BAR_TIME_ICON_X        815
#define PVR_REC_BAR_TIME_ICON_Y        25
#define PVR_REC_BAR_TIME_ICON_W        32
#define PVR_REC_BAR_TIME_ICON_H        32

#define PVR_REC_BAR_TIME_TXT_X         (PVR_REC_BAR_TIME_ICON_X + \
                                     PVR_REC_BAR_TIME_ICON_W)
#define PVR_REC_BAR_TIME_TXT_Y         PVR_REC_BAR_NAME_TXT_Y
#define PVR_REC_BAR_TIME_TXT_W         70
#define PVR_REC_BAR_TIME_TXT_H         36

#define PVR_REC_BAR_CAPACITY_ICON_X        750
#define PVR_REC_BAR_CAPACITY_ICON_Y        114
#define PVR_REC_BAR_CAPACITY_ICON_W        28
#define PVR_REC_BAR_CAPACITY_ICON_H        50

#define PVR_REC_BAR_CAPACITY_ICON_MIDX     2
#define PVR_REC_BAR_CAPACITY_ICON_MIDY     2
#define PVR_REC_BAR_CAPACITY_ICON_MIDW     (PVR_REC_BAR_CAPACITY_ICON_W - PVR_REC_BAR_CAPACITY_ICON_MIDX)
#define PVR_REC_BAR_CAPACITY_ICON_MIDH     (PVR_REC_BAR_CAPACITY_ICON_H - PVR_REC_BAR_CAPACITY_ICON_MIDY)

#define PVR_REC_BAR_CAPACITY_TEXT_X        800
#define PVR_REC_BAR_CAPACITY_TEXT_Y        138
#define PVR_REC_BAR_CAPACITY_TEXT_W        70
#define PVR_REC_BAR_CAPACITY_TEXT_H        36

#define PVR_REC_BAR_RECORDING_ICON_X        45
#define PVR_REC_BAR_RECORDING_ICON_Y        148
#define PVR_REC_BAR_RECORDING_ICON_W        64
#define PVR_REC_BAR_RECORDING_ICON_H        32

#define PVR_REC_BAR_STOP_ICON_X (PVR_REC_BAR_RECORDING_ICON_X + PVR_REC_BAR_RECORDING_ICON_W + 150)
#define PVR_REC_BAR_STOP_ICON_Y             198
#define PVR_REC_BAR_STOP_ICON_W             28
#define PVR_REC_BAR_STOP_ICON_H             28

#define PVR_REC_BAR_STOP_TXT_X (PVR_REC_BAR_STOP_ICON_X + PVR_REC_BAR_STOP_ICON_W)
#define PVR_REC_BAR_STOP_TXT_Y             198
#define PVR_REC_BAR_STOP_TXT_W             250
#define PVR_REC_BAR_STOP_TXT_H             36

#define PVR_REC_BAR_PAUSE_ICON_X (PVR_REC_BAR_STOP_TXT_X + PVR_REC_BAR_STOP_TXT_W)
#define PVR_REC_BAR_PAUSE_ICON_Y             198
#define PVR_REC_BAR_PAUSE_ICON_W             28
#define PVR_REC_BAR_PAUSE_ICON_H             28

#define PVR_REC_BAR_PAUSE_TXT_X (PVR_REC_BAR_PAUSE_ICON_X + PVR_REC_BAR_PAUSE_ICON_W)
#define PVR_REC_BAR_PAUSE_TXT_Y             198
#define PVR_REC_BAR_PAUSE_TXT_W             250
#define PVR_REC_BAR_PAUSE_TXT_H             36

#define PVR_REC_BAR_PLAY_CUR_TIME_X      170
#define PVR_REC_BAR_PLAY_CUR_TIME_Y      90
#define PVR_REC_BAR_PLAY_CUR_TIME_W      100
#define PVR_REC_BAR_PLAY_CUR_TIME_H      36

#define PVR_REC_BAR_PLAY_TOTAL_TIME_X      670
#define PVR_REC_BAR_PLAY_TOTAL_TIME_Y      90
#define PVR_REC_BAR_PLAY_TOTAL_TIME_W      100
#define PVR_REC_BAR_PLAY_TOTAL_TIME_H      36

#define PVR_REC_BAR_REC_CUR_TIME_X       150
#define PVR_REC_BAR_REC_CUR_TIME_Y       140
#define PVR_REC_BAR_REC_CUR_TIME_W       120
#define PVR_REC_BAR_REC_CUR_TIME_H       36

#define PVR_REC_BAR_REC_TOTAL_TIME_X       570
#define PVR_REC_BAR_REC_TOTAL_TIME_Y       140
#define PVR_REC_BAR_REC_TOTAL_TIME_W       120
#define PVR_REC_BAR_REC_TOTAL_TIME_H       36

#define PVR_REC_BAR_PLAY_PROGRESS_X      280
#define PVR_REC_BAR_PLAY_PROGRESS_Y      98
#define PVR_REC_BAR_PLAY_PROGRESS_W      370
#define PVR_REC_BAR_PLAY_PROGRESS_H      20
#define PVR_REC_BAR_PLAY_PROGRESS_MIDX 0
#define PVR_REC_BAR_PLAY_PROGRESS_MIDY 2
#define PVR_REC_BAR_PLAY_PROGRESS_MIDW PVR_REC_BAR_PLAY_PROGRESS_W
#define PVR_REC_BAR_PLAY_PROGRESS_MIDH (PVR_REC_BAR_PLAY_PROGRESS_H-2*PVR_REC_BAR_PLAY_PROGRESS_MIDY)
#define PVR_REC_BAR_PLAY_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_BLUE
#define PVR_REC_BAR_PLAY_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define PVR_REC_BAR_PLAY_PROGRESS_MID  RSI_PROGRESS_BAR_MID_BLUE


#define PVR_REC_BAR_REC_PROGRESS_X      160//230
#define PVR_REC_BAR_REC_PROGRESS_Y      110
#define PVR_REC_BAR_REC_PROGRESS_W      490//370
#define PVR_REC_BAR_REC_PROGRESS_H      20
#define PVR_REC_BAR_REC_PROGRESS_MIDX 0
#define PVR_REC_BAR_REC_PROGRESS_MIDY 2
#define PVR_REC_BAR_REC_PROGRESS_MIDW PVR_REC_BAR_REC_PROGRESS_W
#define PVR_REC_BAR_REC_PROGRESS_MIDH (PVR_REC_BAR_REC_PROGRESS_H-2*PVR_REC_BAR_REC_PROGRESS_MIDY)
#define PVR_REC_BAR_REC_PROGRESS_MIN  RSI_PROGRESS_BAR_MID_RED
#define PVR_REC_BAR_REC_PROGRESS_MAX  RSI_PROGRESS_BAR_BG
#define PVR_REC_BAR_REC_PROGRESS_MID  RSI_PROGRESS_BAR_MID_RED

/* rect style */
#define RSI_PVR_REC_BAR_FRM            RSI_INFOR_WIN

/* font style */
#define FSI_PVR_REC_BAR_NAME           FSI_WHITE_32
#define FSI_PVR_REC_BAR_DATE           FSI_WHITE
#define FSI_PVR_REC_BAR_INFO           FSI_WHITE

RET_CODE open_pvr_rec_bar(u32 para1, u32 para2);

u32 get_cur_rec_time(void);
#endif

