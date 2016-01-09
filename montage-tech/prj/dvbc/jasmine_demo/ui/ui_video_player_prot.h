/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_VIDEO_PLAYER_PROT_H__
#define __UI_VIDEO_PLAYER_PROT_H__

typedef enum
{
    MSG_INFO = MSG_LOCAL_BEGIN + 750,
    MSG_CHANGE_ASPECT,
    MSG_AUDIO_SET,
    MSG_SUBTITLE,
    MSG_VP_SHOW_GOTO,
    MSG_GOTO_DONE,
    MSG_STOP,

    MSG_HIDE_PANEL,
    MSG_HIDE_PLAY_ICON,
    MSG_STOP_REPEAT_KEY

} ui_video_player_msg_t;

#endif
