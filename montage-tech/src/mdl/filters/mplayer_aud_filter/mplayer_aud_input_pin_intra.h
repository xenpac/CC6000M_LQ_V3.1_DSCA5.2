/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MPLAYER_AUD_IN_PIN_INTRA_H_
#define __MPLAYER_AUD_IN_PIN_INTRA_H_

/*!
  private data
  */
typedef struct tag_mplayer_aud_in_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
}mplayer_aud_in_pin_private_t;

/*!
  the mp3 transfer input pin
  */
typedef struct tag_mplayer_aud_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data
    */
  mplayer_aud_in_pin_private_t private_data;
}mplayer_aud_in_pin_t;

/*!
  create a pin

  \return return the instance of mplayer_aud_in_pin_t
  */
mplayer_aud_in_pin_t * mplayer_aud_in_pin_create(
                    mplayer_aud_in_pin_t *p_pin, interface_t *p_owner);

#endif // End for __MPLAYER_AUD_IN_PIN_INTRA_H_