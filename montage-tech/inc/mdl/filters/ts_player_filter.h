/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TS_PLAYER_FILTER_H_
#define __TS_PLAYER_FILTER_H_

/*!
  ts player source
  */
typedef enum tag_ts_player_source
{
  /*!
    file
    */
  TS_PLAYER_SRC_UNKNOW_FILE = 0,
  /*!
    rec file
    */
  TS_PLAYER_SRC_REC_FILE,
  /*!
    rec block
    */
  TS_PLAYER_SRC_REC_BLOCK,
}ts_player_src_t;

/*!
  ts player play mode
  */
typedef enum tag_ts_player_play_mode
{
  /*!
    normal play
    */
  NORMAL_PLAY = 0,
  /*!
    fast forward
    */
  FAST_FORWARD,
  /*!
    fast backward
    */
  FAST_BACKWARD,
  /*!
    slow forward
    */
  SLOW_FORWARD,
  /*!
    slow backward
    */
  SLOW_BACKWARD,
}ts_play_mode_t;

/*!
  ts player config
  */
typedef struct tag_ts_player_config
{
  /*!
    video pid
    */
  u16 video_pid;
  /*!
    audio pid
    */
  u16 audio_pid;
  /*!
    pcr pid
    */
  u16 pcr_pid;
  /*!
    vdec_src_fmt_t
    */
  u8 video_fmt;
  /*!
    adec_src_fmt_vsb_t
    */
  u8 audio_fmt;
  /*!
    load_unit_size
    */
  u32 load_unit_size;
  /*!
    player source
    */
  ts_player_src_t player_src;
  /*!
    media_array
    */
  media_idx_t *p_media_idx;
  /*!
    total time
    */
  u32 total_time;
  /*!
    total size
    */
  u32 total_size;
  /*!
    buffer for ts seq
    */
  u8 *p_buffer;
  /*!
    buffer size, min (300 * 188)
    */
  u32 buffer_size;
}ts_player_config_t;

/*!
  ts player seek type
  */
typedef enum tag_ts_player_seek_type
{
  /*!
    no seek
    */
  TS_PLAYER_NO_SEEK = 0,
  /*!
    time seek
    */
  TS_PLAYER_TIME_SEEK,
  /*!
    position seek
    */
  TS_PLAYER_POSITION_SEEK,
  /*!
    frame seek
    */
  TS_PLAYER_FRAME_SEEK,
}ts_player_seek_type_t;

/*!
  ts player seek type
  */
typedef enum tag_ts_player_seek_option
{
  /*!
    seek set
    */
  TS_PLAYER_SEEK_SET,
  /*!
    seek current
    */
  TS_PLAYER_SEEK_CUR,
  /*!
    seek end
    */
  TS_PLAYER_SEEK_END,
}ts_player_seek_option_t;

/*!
  ts player seek paramter
  */
typedef struct tag_ts_player_seek_paramter
{
  /*!
    ts_player_seek_type_t
    */
  ts_player_seek_type_t seek_type;
  /*!
    ts_player_seek_option_t
    */
  ts_player_seek_option_t seek_op;
  /*!
    offset
    */
  s64 offset;
}ts_player_seek_param_t;

/*!
  ts player filter command define
  */
typedef enum tag_ts_player_filter_cmd
{
  /*!
    config video pid...
    */
  TS_PLAYER_CMD_CFG,
  /*!
    set play mode
    */
  TS_PLAYER_CMD_SET_PLAY_MODE,
  /*!
    ts player CMD seek
    */
  TS_PLAYER_CMD_SEEK,
  /*!
    ts player CMD change audio
    */
  TS_PLAYER_CMD_CHANGE_AUDIO,
  /*!
    end
    */
  TS_PLAYER_CFG_END
}ts_player_filter_cmd_t;

/*!
  ts player filter event define
  */
typedef enum tag_ts_player_filter_evt
{
  /*!
    play mode auto changed
    */
  PLAYER_MSG_PLAY_MODE_CHANGED = TS_PLAYER_FILTER << 16,
  /*!
    player play time & size update
    */
  PLAYER_MSG_PLAY_TIME_UPDATE,
  /*!
    player play error
    */
  PLAYER_MSG_PLAY_ERROR,
  /*!
    player play play end
    */
  AV_RENDER_MSG_FILE_PLAY_END,
}ts_player_filter_evt_t;

/*!
  create a ts player filter instance
  filter ID:TS_PLAYER_FILTER
  \return return the instance of record_filter_t
  */
ifilter_t * ts_player_filter_create(void *p_para);

#endif // End for __TS_PLAYER_FILTER_H_

