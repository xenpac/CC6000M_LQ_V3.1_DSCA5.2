/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MP3_DEC_PIN_INTRA_H_
#define __MP3_DEC_PIN_INTRA_H_

/*!
  private data
  */
typedef struct tag_mp3_dec_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    audio dev
    */
  void *p_audio_dev;
  /*!
    last sample
    */
  BOOL found_last_sample;
  /*!
    error data timeout
    */
  u32 error_data_timeout;
}mp3_dec_pin_private_t;


/*!
  the input pin
  */
typedef struct tag_mp3_dec_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_pin_t base_pin;
  /*!
    private data buffer
    */
  mp3_dec_pin_private_t private_data;
}mp3_dec_pin_t;

/*!
  create a pin

  \return return the instance of mp3_dec_pin_t
  */
mp3_dec_pin_t * mp3_dec_pin_create(mp3_dec_pin_t *p_pin, interface_t *p_owner);


#endif // End for __MP3_DEC_PIN_INTRA_H_
