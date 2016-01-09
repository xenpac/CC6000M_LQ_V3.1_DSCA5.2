/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_NUM_PLAY_H__
#define __UI_NUM_PLAY_H__

/* coordinate */
#define NUM_PLAY_CONT_X       140
#define NUM_PLAY_CONT_Y       60
#define NUM_PLAY_CONT_W       120
#define NUM_PLAY_CONT_H       NUM_PLAY_ICON_H

#define NUM_PLAY_ICON_X       0
#define NUM_PLAY_ICON_Y       0
#define NUM_PLAY_ICON_W       30
#define NUM_PLAY_ICON_H       45

#define NUM_PLAY_FRM_X       (NUM_PLAY_CONT_W-NUM_PLAY_FRM_W)
#define NUM_PLAY_FRM_Y       0
#define NUM_PLAY_FRM_W       NUM_PLAY_CONT_W
#define NUM_PLAY_FRM_H       NUM_PLAY_CONT_H

//for ui_proglist
#define NUM_PLAY_CONT_PREVX       660//710
#define NUM_PLAY_CONT_PREVY       150
#define NUM_PLAY_CONT_PREVW       120
#define NUM_PLAY_CONT_PREVH       NUM_PLAY_ICON_H

#define NUM_PLAY_ICON_PREVX       0
#define NUM_PLAY_ICON_PREVY       0
#define NUM_PLAY_ICON_PREVW       60
#define NUM_PLAY_ICON_PREVH       71

#define NUM_PLAY_FRM_PREVX       0
#define NUM_PLAY_FRM_PREVY       0
#define NUM_PLAY_FRM_PREVW       NUM_PLAY_CONT_PREVW
#define NUM_PLAY_FRM_PREVH       NUM_PLAY_CONT_H

#define NUM_PLAY_CNT        4

/* rect style */
#define RSI_NUM_PLAY_CONT     RSI_IGNORE
#define RSI_NUM_PLAY_TXT     RSI_IGNORE

/* font style */
#define FSI_NUM_PLAY_TXT      FSI_NUMBER

/* others */

RET_CODE open_num_play(u32 para1, u32 para2);

#endif

