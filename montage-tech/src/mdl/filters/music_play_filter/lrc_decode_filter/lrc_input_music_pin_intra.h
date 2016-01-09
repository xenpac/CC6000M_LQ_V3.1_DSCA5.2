/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LRC_MUSIC_INPUT_PIN_INTRA_H_
#define __LRC_MUSIC_INPUT_PIN_INTRA_H_


/*!
  lrc input pin private data
  */
typedef struct tag_lrc_music_input_pin_private
{
  /*!
    this point !!
    */
    void *p_this;
    /*!
    mem alloc interface
    */
    iasync_reader_t *p_reader;

}lrc_music_in_pin_private_t;

/*!
  the lrc input pin
  */
typedef struct tag_lrc_music_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data buffer
    */
  lrc_music_in_pin_private_t private_data;
}lrc_music_in_pin_t;


/*!
  create a lrc input pin
  \param[in] p_pin lrc input pin
  \param[in] p_owner lrc input pin's owner: lrc filter
  \return return the instance of rec_in_pin_t
  */
lrc_music_in_pin_t * lrc_music_in_pin_create(lrc_music_in_pin_t *p_pin, interface_t *p_owner);

#endif // End for _LRC_MUSIC_INPUT_PIN_INTRA_H_

