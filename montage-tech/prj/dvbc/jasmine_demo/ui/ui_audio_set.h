/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_AUDIO_SET_H__
#define __UI_AUDIO_SET_H__

/* coordinate */
#define AUDIO_SET_CONT_X			((SCREEN_WIDTH - AUDIO_SET_CONT_W) / 2)
#define AUDIO_SET_CONT_Y			((SCREEN_HEIGHT - AUDIO_SET_CONT_H) / 2)
#define AUDIO_SET_CONT_W			500
#define AUDIO_SET_CONT_H			200

#define AUDIO_SET_FRM_X				0
#define AUDIO_SET_FRM_Y				0
#define AUDIO_SET_FRM_W				AUDIO_SET_CONT_W
#define AUDIO_SET_FRM_H				AUDIO_SET_CONT_H

#define AUDIO_SET_TITLE_TXT_X       ((AUDIO_SET_FRM_W - AUDIO_SET_TITLE_TXT_W) / 2)
#define AUDIO_SET_TITLE_TXT_Y       10
#define AUDIO_SET_TITLE_TXT_W       200
#define AUDIO_SET_TITLE_TXT_H       36


#define AUDIO_SET_ITEM_CNT			2 
#define AUDIO_SET_ITEM_X			((AUDIO_SET_FRM_W - AUDIO_SET_ITEM_LW - AUDIO_SET_ITEM_RW) / 2)
#define AUDIO_SET_ITEM_Y			(AUDIO_SET_TITLE_TXT_Y + AUDIO_SET_TITLE_TXT_H + 30)
#define AUDIO_SET_ITEM_LW			180//150
#define AUDIO_SET_ITEM_RW			300//230
#define AUDIO_SET_ITEM_H			36
#define AUDIO_SET_ITEM_V_GAP		6

/* rect style */
#define RSI_AUDIO_SET_FRM			RSI_WINDOW_1

/* font style */

typedef enum 
{
  AUDIO_SET_PLAY_NULL = 0,
  AUDIO_SET_PLAY_RECORD,
  AUDIO_SET_PLAY_PROGRAM
}ui_audio_set_type_e;

RET_CODE open_audio_set (u32 para1, u32 para2);

#endif

