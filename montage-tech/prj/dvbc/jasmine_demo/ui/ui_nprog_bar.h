/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NPROG_BAR_H__
#define __UI_NPROG_BAR_H__

/* coordinate */
#define NPROG_BAR_CONT_X            ((SCREEN_WIDTH - NPROG_BAR_CONT_W) / 2)
#define NPROG_BAR_CONT_Y            (SCREEN_HEIGHT - NPROG_BAR_CONT_H - (10+SCREEN_POS_Y))
#define NPROG_BAR_CONT_W            (COMM_BG_W)
#define NPROG_BAR_CONT_H            190

//frame
#define NPROG_BAR_FRM_X              ((NPROG_BAR_CONT_W - NPROG_BAR_FRM_W)/2)
#define NPROG_BAR_FRM_Y              0
#define NPROG_BAR_FRM_W              COMM_BG_W
#define NPROG_BAR_FRM_H              NPROG_BAR_CONT_H

//prog num
#define NPROG_BAR_NUMER_X    40
#define NPROG_BAR_NUMER_Y     20
#define NPROG_BAR_NUMER_W    76
#define NPROG_BAR_NUMER_H     40

//prog name
#define NPROG_BAR_NAME_TXT_X         (NPROG_BAR_NUMER_X + NPROG_BAR_NUMER_W + 20)//210//130
#define NPROG_BAR_NAME_TXT_Y         NPROG_BAR_NUMER_Y
#define NPROG_BAR_NAME_TXT_W         360
#define NPROG_BAR_NAME_TXT_H         40

//current tp info
#define NPROG_BAR_TP_INFO_X      NPROG_BAR_NUMER_X
#define NPROG_BAR_TP_INFO_Y      (NPROG_BAR_PF_INFO_Y + NPROG_BAR_PF_INFO_CNT * NPROG_BAR_PF_INFO_H + NPROG_BAR_PF_INFO_V_GAP + 2)//(NPROG_BAR_NUMER_Y + NPROG_BAR_NUMER_H + 10)
#define NPROG_BAR_TP_INFO_W     280
#define NPROG_BAR_TP_INFO_H      NPROG_BAR_NUMER_H

//pf info
#define NPROG_BAR_PF_INFO_CNT        2
#define NPROG_BAR_PF_INFO_X          NPROG_BAR_NUMER_X
#define NPROG_BAR_PF_INFO_Y          (NPROG_BAR_NUMER_Y + NPROG_BAR_NUMER_H + NPROG_BAR_PF_INFO_V_GAP)//(NPROG_BAR_TP_INFO_Y + NPROG_BAR_TP_INFO_H + 6)
#define NPROG_BAR_PF_INFO_W         500
#define NPROG_BAR_PF_INFO_H          36
#define NPROG_BAR_PF_INFO_V_GAP      6

//date icon
#define NPROG_BAR_DATE_ICON_X        620//660
#define NPROG_BAR_DATE_ICON_Y        10
#define NPROG_BAR_DATE_ICON_W        30
#define NPROG_BAR_DATE_ICON_H        30

//date
#define NPROG_BAR_DATE_TXT_X         (NPROG_BAR_DATE_ICON_X + NPROG_BAR_DATE_ICON_W + 20)
#define NPROG_BAR_DATE_TXT_Y         NPROG_BAR_DATE_ICON_Y
#define NPROG_BAR_DATE_TXT_W         140
#define NPROG_BAR_DATE_TXT_H         30

//time
#define NPROG_BAR_TIME_TXT_X         (NPROG_BAR_DATE_TXT_X + NPROG_BAR_DATE_TXT_W + 10)
#define NPROG_BAR_TIME_TXT_Y         NPROG_BAR_DATE_TXT_Y
#define NPROG_BAR_TIME_TXT_W         70
#define NPROG_BAR_TIME_TXT_H         30

//ad
#define NPROG_BAR_AD_X        NPROG_BAR_DATE_ICON_X
#define NPROG_BAR_AD_Y        (NPROG_BAR_DATE_ICON_Y + NPROG_BAR_DATE_ICON_H + NPROG_BAR_PF_INFO_V_GAP)
#define NPROG_BAR_AD_W        300
#define NPROG_BAR_AD_H        130

//prog bar info icon /* SUBTT TELTEXT EPG */
#define NPROG_BAR_INFO_ICON_CNT      3 /* SUBTT TELTEXT EPG */
#define NPROG_BAR_INFO_ICON_X        (NPROG_BAR_TP_INFO_X + NPROG_BAR_TP_INFO_W )//NPROG_BAR_AD_X
#define NPROG_BAR_INFO_ICON_Y        NPROG_BAR_TP_INFO_Y//(NPROG_BAR_AD_Y + NPROG_BAR_AD_H + 10)
#define NPROG_BAR_INFO_ICON_W        35
#define NPROG_BAR_INFO_ICON_H        30
#define NPROG_BAR_INFO_ICON_H_GAP    2

//prog bar MARK icon /* MONEY FAV LOCK SKIP*/
#define NPROG_BAR_MARK_ICON_CNT      4/*  MONEY FAV LOCK SKIP*/
#define NPROG_BAR_MARK_ICON_X       (NPROG_BAR_INFO_ICON_X + (NPROG_BAR_INFO_ICON_CNT + 1) *  \
                                                              (NPROG_BAR_INFO_ICON_W + NPROG_BAR_INFO_ICON_H_GAP))//788
#define NPROG_BAR_MARK_ICON_Y        NPROG_BAR_INFO_ICON_Y
#define NPROG_BAR_MARK_ICON_W        25
#define NPROG_BAR_MARK_ICON_H        30
#define NPROG_BAR_MARK_ICON_H_GAP    2

//unused
#define NPROG_BAR_AV_ICON_CNT      3/*  10801 H.264 MPEGI*/
#define NPROG_BAR_AV_ICON_X       570
#define NPROG_BAR_AV_ICON_Y       (NPROG_BAR_CA_Y + NPROG_BAR_CA_H)//201
#define NPROG_BAR_AV_ICON_H        36
#define NPROG_BAR_AV_ICON_W       48//80 //

#define NPROG_BAR_GROUP_INFO_X       151
#define NPROG_BAR_GROUP_INFO_Y       199
#define NPROG_BAR_GROUP_INFO_W       300
#define NPROG_BAR_GROUP_INFO_H       36

#define NPROG_BAR_GROUP_X  550
#define NPROG_BAR_GROUP_Y  78
#define NPROG_BAR_GROUP_W  250
#define NPROG_BAR_GROUP_H  36

#define NPROG_BAR_CA_X     NPROG_BAR_GROUP_X
#define NPROG_BAR_CA_Y     78//(NPROG_BAR_INFO_ICON_Y + NPROG_BAR_INFO_ICON_H + 3)//114
#define NPROG_BAR_CA_W      280
#define NPROG_BAR_CA_H       NPROG_BAR_GROUP_H

#define NPROG_BAR_TP_X      NPROG_BAR_GROUP_X
#define NPROG_BAR_TP_Y       (NPROG_BAR_CA_Y + NPROG_BAR_CA_H +3)
#define NPROG_BAR_TP_W      NPROG_BAR_GROUP_W
#define NPROG_BAR_TP_H       NPROG_BAR_GROUP_H

#define NPROG_BAR_QUALITY_X       506
#define NPROG_BAR_QUALITY_Y       (NPROG_BAR_NAME_TXT_Y + 30)
#define NPROG_BAR_QUALITY_W       12
#define NPROG_BAR_QUALITY_H       70

#define NPROG_BAR_INTEN_X       (NPROG_BAR_QUALITY_X + NPROG_BAR_QUALITY_W + 14)
#define NPROG_BAR_INTEN_Y       (NPROG_BAR_NAME_TXT_Y + 30)
#define NPROG_BAR_INTEN_W       12
#define NPROG_BAR_INTEN_H       70

#define NPROG_BAR_SIGNAL_X      840
#define NPROG_BAR_SIGNAL_Y       110
#define NPROG_BAR_SIGNAL_W       12
#define NPROG_BAR_SIGNAL_H       70

/* rect style */
#define RSI_NPROG_BAR_FRM            RSI_INFOR_WIN
#define RSI_TP_LIST_SBAR_BG     RSI_SCROLL_BAR_BG
#define RSI_TP_LIST_FRM         RSI_WINDOW_1
#define RSI_TP_LIST             RSI_PBACK
#define RSI_TP_LIST_SBAR_MID    RSI_SCROLL_BAR_MID
#define RSI_TP_LIST_PBAR_BG     RSI_PROGRESS_BAR_BG
#define RSI_TP_LIST_PBAR_MID    RSI_PROGRESS_BAR_MID_ORANGE
#define RSI_TP_LIST_SAT         RSI_ITEM_1_HL

/* font style */
#define FSI_NPROG_BAR_NAME           FSI_WHITE_32
#define FSI_NPROG_BAR_PF             FSI_BLUE
#define FSI_NPROG_BAR_GROUP          FSI_PROGBAR_BOTTOM
#define FSI_NPLIST_BRIEF        FSI_WHITE_INFO

#define NPLIST_CONT_MBOXH        44

RET_CODE open_nprog_bar(u32 para1, u32 para2);
void fill_nprog_info(control_t *cont, u16 prog_id);
#endif

