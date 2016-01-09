/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_NET_MUSIC_METADATA_API_H__
#define __UI_NET_MUSIC_METADATA_API_H__
#include "net_music_filter.h"


typedef enum 
{
    MSG_NET_MUSIC_API_EVT_FAILED = MSG_EXTERN_BEGIN + 1850,
    MSG_NET_MUSIC_API_EVT_SUCCESS,
}net_music_metadata_msg_t;

//get meta data
RET_CODE ui_net_music_metadata_init(void);

RET_CODE ui_net_music_metadata_release(void);

RET_CODE ui_net_music_load_metadata(net_music_param_t *param);

net_music_tracks_content_t * ui_net_music_get_track_list(void);


net_music_albums_content_t * ui_net_music_get_album_list(void);


net_music_artists_content_t * ui_net_music_get_artist_list(void);








#endif