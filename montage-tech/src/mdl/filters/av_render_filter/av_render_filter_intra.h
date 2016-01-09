/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AV_RENDER_FILTER_INTRA_H_
#define __AV_RENDER_FILTER_INTRA_H_

/*!
  av_render_filter_private
  */
typedef struct tag_av_render_filter_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    input pin
    */
  av_render_pin_t m_pin;
  /*!
    dmx video chain
    */
  dmx_chanid_t video_chain;
  /*!
    dmx audio chain
    */
  dmx_chanid_t audio_chain;
  /*!
    dmx pcr chain
    */
  dmx_chanid_t pcr_chain;
  /*!
    av_render_config_t
    */
  av_render_config_t cfg;
  /*!
    is paused
    */
  BOOL b_pause;
  /*!
    play_mode
    */
  u32 play_mode : 8;
  /*!
    reserved
    */
  u32 reserved : 24;
}av_render_filter_private_t;

/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_av_render_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_filter_t _sink_filter;
  /*!
    private data
    */
  av_render_filter_private_t private_data;
  /*!
    av_render_change_pid
    */
  void (*av_render_change_pid)(handle_t _this, dynamic_rec_info_t *p_rec_info);
  /*!
    av_render_check_audio
    */
  u32 (*av_render_check_audio)(handle_t _this, u8 *p_data, u32 payload);
  /*!
    av_render_reset
    */
  void (*av_render_reset)(handle_t _this);
}av_render_filter_t;

#endif // End for __AV_RENDER_FILTER_INTRA_H_
