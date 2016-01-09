/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AV_RENDER_PIN_INTRA_H_
#define __AV_RENDER_PIN_INTRA_H_

/*!
  video render pin private data
  */
typedef struct tag_av_render_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    dmx device
    */
  dmx_device_t *p_dmx_dev;
  /*!
    video dev
    */
  void *p_video_dev;
  /*!
    audio dev
    */
  void *p_audio_dev;
  /*!
    display device
    */
  void *p_disp_dev;
  /*!
    video is show
    */
  u32 b_video_show : 1;
  /*!
    video is freeze
    */
  u32 b_video_freeze : 1;
  /*!
    av abnormal
    */
  u32 av_abnormal : 8;
  /*!
    video is show
    */
  u32 reserved : 22;
  /*!
    resv data
    */
  u8 *p_resv_data;
  /*!
  reserved data
  */
  u8 resv_data[4];
  /*!
  TS is clear stream
  */
  u8 is_clear_stream;
}av_render_pin_private_t;

/*!
  the input pin
  */
typedef struct tag_av_render_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_pin_t base_pin;
  /*!
    private data
    */
  av_render_pin_private_t private_data;
}av_render_pin_t;

/*!
  create a pin

  \return return the instance of video_render_pin_t
  */
av_render_pin_t * av_render_pin_create(av_render_pin_t *p_pin, interface_t *p_owner,void *p_para);

#endif // End for __AV_RENDER_PIN_INTRA_H_

