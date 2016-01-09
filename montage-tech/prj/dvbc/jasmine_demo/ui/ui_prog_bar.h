/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PROG_BAR_H__
#define __UI_PROG_BAR_H__

/* coordinate */
#define PROG_BAR_CONT_X             ((SCREEN_WIDTH - PROG_BAR_CONT_W) / 2)
#define PROG_BAR_CONT_Y             (SCREEN_HEIGHT - PROG_BAR_CONT_H)//419
#define PROG_BAR_CONT_W            UI_COMMON_WIDTH
#define PROG_BAR_CONT_H             260

#define PROG_BAR_ICON_X             39
#define PROG_BAR_ICON_Y             0
#define PROG_BAR_ICON_W             76
#define PROG_BAR_ICON_H             70

#define PROG_BAR_ICON_TXT_X    36
#define PROG_BAR_ICON_TXT_Y    105
#define PROG_BAR_ICON_TXT_W    96
#define PROG_BAR_ICON_TXT_H    40

#define PROG_BAR_FRM_X              0
#define PROG_BAR_FRM_Y              0
#define PROG_BAR_FRM_W              PROG_BAR_CONT_W
#define PROG_BAR_FRM_H              PROG_BAR_CONT_H

#define PROG_BAR_NAME_TXT_X         129
#define PROG_BAR_NAME_TXT_Y         25
#define PROG_BAR_NAME_TXT_W         340
#define PROG_BAR_NAME_TXT_H         36

#define PROG_BAR_DATE_ICON_X        601
#define PROG_BAR_DATE_ICON_Y        26
#define PROG_BAR_DATE_ICON_W        32
#define PROG_BAR_DATE_ICON_H        32

#define PROG_BAR_DATE_TXT_X         (PROG_BAR_DATE_ICON_X + \
                                     PROG_BAR_DATE_ICON_W)
#define PROG_BAR_DATE_TXT_Y         PROG_BAR_NAME_TXT_Y
#define PROG_BAR_DATE_TXT_W         83
#define PROG_BAR_DATE_TXT_H         36

#define PROG_BAR_TIME_ICON_X        716
#define PROG_BAR_TIME_ICON_Y        26
#define PROG_BAR_TIME_ICON_W        32
#define PROG_BAR_TIME_ICON_H        32

#define PROG_BAR_TIME_TXT_X         (PROG_BAR_TIME_ICON_X + \
                                     PROG_BAR_TIME_ICON_W)
#define PROG_BAR_TIME_TXT_Y         PROG_BAR_NAME_TXT_Y
#define PROG_BAR_TIME_TXT_W         83
#define PROG_BAR_TIME_TXT_H         36

/*#define PROG_BAR_NOTIFY_ICON_X      (PROG_BAR_TIME_TXT_X + PROG_BAR_TIME_TXT_W)
#define PROG_BAR_NOTIFY_ICON_Y      PROG_BAR_ARROW_ICON_Y
#define PROG_BAR_NOTIFY_ICON_W      24
#define PROG_BAR_NOTIFY_ICON_H      24*/

#define PROG_BAR_MARK_ICON_CNT      5/* TRACK MONEY FAV LOCK SKIP*/
#define PROG_BAR_MARK_ICON_X        621
#define PROG_BAR_MARK_ICON_Y        84
#define PROG_BAR_MARK_ICON_W        42
#define PROG_BAR_MARK_ICON_H        30
#define PROG_BAR_MARK_ICON_H_GAP    2

#define PROG_BAR_INFO_ICON_CNT      3 /* SUBTT TELTEXT EPG */
#define PROG_BAR_INFO_ICON_X        720
#define PROG_BAR_INFO_ICON_Y        136//120
#define PROG_BAR_INFO_ICON_W        36
#define PROG_BAR_INFO_ICON_H        36
#define PROG_BAR_INFO_ICON_H_GAP    2

#define PROG_BAR_PF_INFO_CNT        2
#define PROG_BAR_PF_INFO_X          151
#define PROG_BAR_PF_INFO_Y          88
#define PROG_BAR_PF_INFO_W          460
#define PROG_BAR_PF_INFO_H          36
#define PROG_BAR_PF_INFO_V_GAP      4

#define PROG_BAR_GROUP_INFO_X       151
#define PROG_BAR_GROUP_INFO_Y       199
#define PROG_BAR_GROUP_INFO_W       300
#define PROG_BAR_GROUP_INFO_H       36

#define PROG_BAR_TP_X       (PROG_BAR_GROUP_INFO_X + PROG_BAR_GROUP_INFO_W+ 5)
#define PROG_BAR_TP_Y       PROG_BAR_GROUP_INFO_Y
#define PROG_BAR_TP_W       200
#define PROG_BAR_TP_H       36

#define PROG_BAR_CA_INFO_X       (PROG_BAR_TP_X + PROG_BAR_TP_W+ 5)
#define PROG_BAR_CA_INFO_Y       110
#define PROG_BAR_CA_INFO_W       180
#define PROG_BAR_CA_INFO_H       26



#define PROG_BAR_QUALITY_X       506//(PROG_BAR_NOTIFY_ICON_X + PROG_BAR_NOTIFY_ICON_W + 14)
#define PROG_BAR_QUALITY_Y       (PROG_BAR_NAME_TXT_Y + 30)
#define PROG_BAR_QUALITY_W       8
#define PROG_BAR_QUALITY_H       70

#define PROG_BAR_INTEN_X       (PROG_BAR_QUALITY_X + PROG_BAR_QUALITY_W + 14)
#define PROG_BAR_INTEN_Y       (PROG_BAR_NAME_TXT_Y + 30)
#define PROG_BAR_INTEN_W       8
#define PROG_BAR_INTEN_H       70

/* rect style */
#define RSI_PROG_BAR_FRM            RSI_INFOR_WIN

/* font style */
#define FSI_PROG_BAR_NAME           FSI_WHITE
#define FSI_PROG_BAR_PF             FSI_BLUE
#define FSI_PROG_BAR_GROUP          FSI_PROGBAR_BOTTOM

RET_CODE open_prog_bar(u32 para1, u32 para2);
void fill_prog_info(control_t *cont, u16 prog_id);
void fill_prog_name_info(control_t *cont, u16 prog_id);
#endif

