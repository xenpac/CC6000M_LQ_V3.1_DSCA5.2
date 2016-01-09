/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TS_PLAYER_FILTER_INTRA_H_
#define __TS_PLAYER_FILTER_INTRA_H_

/*!
  sample request type
  */
typedef enum tag_sample_request_type
{
  /*!
    sample request default
    */
  SAMPLE_REQUEST_DEFAULT = 0,
  /*!
    sample request seek
    */
  SAMPLE_REQUEST_SEEK,
  /*!
    sample request trick mode
    */
  SAMPLE_REQUEST_TRICK_MODE_AV_RESET
}sample_request_type_t;

/*!
  player state
  */
typedef enum tag_player_running_state
{
  /*!
    player running normal
    */
  PLAYER_RUNNING_NORMAL = 0,
  /*!
    player running check play end
    */
  PLAYER_RUNNING_CHECK_PLAY_END,
  /*!
    player running check play mode to normal
    */
  PLAYER_RUNNING_CHECK_PLAY_MODE_TO_NORMAL,
  /*!
    player running play end
    */
  PLAYER_RUNNING_PLAY_END,
}player_running_state_t;

/*!
  ts player filter private data
  */
typedef struct tag_ts_player_filter_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    input pin
    */
  ts_player_in_pin_t m_in_pin;
  /*!
    video output pin
    */
  ts_player_out_pin_t m_out_pin;
  /*!
    ts player cfg
    */
  ts_player_config_t cfg;
  /*!
    play mode
    */
  u8 play_mode;
  /*!
    ts sequence direction, 0:backward, 1:forward
    */
  u8 ts_seq_direction;
  /*!
    pid_changed
    */
  u8 pid_changed;
  /*!
    sample request
    */
  u8 sample_request;
  /*!
    ts sequence
    */
  void *p_ts_seq;
  /*!
    last play time info
    */
 u32 last_play_time;
  /*!
    last play pos
    */
 u32 last_play_pos;
  /*!
    last play pos
    */
 u32 last_pts;
  /*!
    last send index
    */
  u32 last_send_index;
  /*!
    frame len, used for fast forward mode
    */
  dynamic_rec_info_t rec_info;
  /*!
    seek end
    */
  u32 seek_end_check : 4;
  /*!
    play end
    */
  u32 play_end : 4;
  /*!
    play end check cnt
    */
  u32 play_end_check : 8;
  /*!
    reserved
    */
  u32 trick_mode_skip : 11;
  /*!
    player_running_state_t
    */
  u32 player_running_state : 5;
  /*!
    last sample
    */
  u32 last_sample;
  /*!
    bit_rate
    */
  u32 bit_rate;
  /*!
    seek sem
    */
  os_sem_t seek_sem;
  /*!
    seek request sem
    */
  os_sem_t seek_req_sem;
  /*!
    video dev
    */
  void *p_video_dev;
  /*!
    ts info
    */
  ts_detail_info_t ts_info;
}ts_player_filter_private_t;

/*!
  the ts player filter
  */
typedef struct tag_ts_player_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data
    */
  ts_player_filter_private_t private_data;
}ts_player_filter_t;

#endif // End for __TS_PLAYER_FILTER_INTRA_H_

