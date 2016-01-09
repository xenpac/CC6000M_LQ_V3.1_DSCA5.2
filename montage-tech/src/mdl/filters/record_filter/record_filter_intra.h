/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RECORD_FILTER_INTRA_H_
#define __RECORD_FILTER_INTRA_H_


/*!
  record state
  */
typedef enum tag_record_state
{
  /*!
    stopped
    */
  RECORD_FILTER_STOPPED = 0,
  /*!
    running
    */
  RECORD_FILTER_RUNNING,
  /*!
    resume
    */
  RECORD_FILTER_RESUME = RECORD_FILTER_RUNNING,
  /*!
    pre pause
    */
  RECORD_FILTER_PRE_PAUSE,
  /*!
    pause
    */
  RECORD_FILTER_PAUSE,
  /*!
    pre resume
    */
  RECORD_FILTER_PRERESUME
}record_state_t;

/*!
  record filter private data
  */
typedef struct tag_rec_filter_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    input pin
    */
  rec_in_pin_t m_in_pin;
  /*!
    output pin
    */
  rec_out_pin_t m_out_pin;
  /*!
    static rec info
    */
  static_rec_info_t st_rec_info;
  /*!
    dynamic rec info
    */
  dynamic_rec_info_t dy_rec_info;
  /*!
    duration time(seconds) timeshift is 0
    */
  u32 timeshift_mode : 1;
  /*!
    run
    */
  u32 running : 1;
  /*!
    reserved
    */
  u32 reserved : 30;
  /*!
    media_array
    */
  media_idx_t *p_media_array;
  /*!
    start  ticks
    */
  u32 start_tick;
  /*!
    dring pause
    */
  u32 pause_tick;
  /*!
    skip time(ms)
    */
  u32 skip_time;
  /*!
    last rec time (ms)
    */
  u32 last_rec_time;
  /*!
    last vkey_frame_offset
    */
  u32 vkey_frame_offset;
  /*!
    last pts
    */
  u32 last_pts;
  /*!
    unit pts offset
    */
  u32 unit_pts_offset;
  /*!
    record mode
    */
  u32 record_mode;
}rec_filter_private_t;

/*!
  the record filter define
  */
typedef struct tag_record_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data
    */
  rec_filter_private_t private_data;
}record_filter_t;

#endif // End for __RECORD_FILTER_INTRA_H_

