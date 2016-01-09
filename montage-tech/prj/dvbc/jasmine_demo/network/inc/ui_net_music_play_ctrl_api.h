/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_NET_MUSIC_PLAY_CTRL_API_H__
#define __UI_NET_MUSIC_PLAY_CTRL_API_H__


typedef enum 
{
    MSG_NET_MUSIC_PLAY_END = MSG_EXTERN_BEGIN + 1870,
    MSG_NET_MUSIC_CANNOT_PLAY,
    MSG_NET_MUSIC_GET_PLAY_TIME,
    MSG_NET_MUSIC_STOPPED,
    MSG_NET_MUSIC_DATA_ERROR, 
    MSG_NET_MUSIC_TIMEOUT,
}net_music_play_ctrl_msg_t;


//play contrl
RET_CODE ui_net_music_play_ctrl_init(void);

RET_CODE ui_net_music_play_ctrl_release(void);

RET_CODE ui_net_music_play_ctrl_set_url(u8* p_url);


RET_CODE ui_net_music_play_ctrl_start(void);


RET_CODE ui_net_music_play_ctrl_stop(void);

RET_CODE ui_net_music_play_ctrl_pause(void);

RET_CODE ui_net_music_play_ctrl_resume(void);





#endif