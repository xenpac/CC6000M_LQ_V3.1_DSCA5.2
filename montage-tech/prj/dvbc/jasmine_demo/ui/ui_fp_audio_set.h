/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 
 ****************************************************************************/
#ifndef __UI_FP_AUDIO_SET_H__
#define __UI_FP_AUDIO_SET_H__

/* coordinate */
#define FP_AUDIO_SET_CONT_X			((SCREEN_WIDTH - FP_AUDIO_SET_CONT_W) / 2)
#define FP_AUDIO_SET_CONT_Y			((SCREEN_HEIGHT - FP_AUDIO_SET_CONT_H) / 2)
#define FP_AUDIO_SET_CONT_W			500
#define FP_AUDIO_SET_CONT_H			200

#define FP_AUDIO_SET_FRM_X				0
#define FP_AUDIO_SET_FRM_Y				0
#define FP_AUDIO_SET_FRM_W				FP_AUDIO_SET_CONT_W
#define FP_AUDIO_SET_FRM_H				FP_AUDIO_SET_CONT_H

#define FP_AUDIO_SET_TITLE_TXT_X       ((FP_AUDIO_SET_FRM_W - FP_AUDIO_SET_TITLE_TXT_W) / 2)
#define FP_AUDIO_SET_TITLE_TXT_Y       10
#define FP_AUDIO_SET_TITLE_TXT_W       200
#define FP_AUDIO_SET_TITLE_TXT_H       36


#define FP_AUDIO_SET_ITEM_CNT			2 
#define FP_AUDIO_SET_ITEM_X			((FP_AUDIO_SET_FRM_W - FP_AUDIO_SET_ITEM_LW - FP_AUDIO_SET_ITEM_RW) / 2)
#define FP_AUDIO_SET_ITEM_Y			(FP_AUDIO_SET_TITLE_TXT_Y + FP_AUDIO_SET_TITLE_TXT_H + 30)
#define FP_AUDIO_SET_ITEM_LW			180//150
#define FP_AUDIO_SET_ITEM_RW			300//230
#define FP_AUDIO_SET_ITEM_H			36
#define FP_AUDIO_SET_ITEM_V_GAP		6

/* rect style */
#define RSI_FP_AUDIO_SET_FRM			RSI_WINDOW_1

/* font style */

RET_CODE open_fp_audio_set(u32 para1, u32 para2);

#endif

