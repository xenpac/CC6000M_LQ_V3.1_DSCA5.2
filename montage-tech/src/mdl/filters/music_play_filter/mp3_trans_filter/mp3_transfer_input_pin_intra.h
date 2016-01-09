/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MP3_TRANS_IN_PIN_INTRA_H_
#define __MP3_TRANS_IN_PIN_INTRA_H_

/*!
  private data
  */
typedef struct tag_mp3_trans_in_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    mem alloc interface
    */
  iasync_reader_t *p_reader;
}mp3_trans_in_pin_private_t;

/*!
  the mp3 transfer input pin
  */
typedef struct tag_mp3_trans_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data
    */
  mp3_trans_in_pin_private_t private_data;

  /*!
    get the next sample
    */
  void (*request_next_sample)(handle_t _this, media_sample_t *p_sample);

}mp3_trans_in_pin_t;

/*!
  create a pin

  \return return the instance of mp3_trans_in_pin_t
  */
mp3_trans_in_pin_t * mp3_trans_in_pin_create(
                    mp3_trans_in_pin_t *p_pin, interface_t *p_owner);



#endif // End for __MP3_TRANS_IN_PIN_INTRA_H_
