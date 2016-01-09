/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RECORD_INPUT_PIN_INTRA_H_
#define __RECORD_INPUT_PIN_INTRA_H_

/*!
  record input pin private data
  */
typedef struct tag_rec_input_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
}rec_in_pin_private_t;

/*!
  the record input pin
  */
typedef struct tag_rec_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data
    */
  rec_in_pin_private_t private_data;
}rec_in_pin_t;

/*!
  create a record input pin
  \param[in] p_pin record input pin
  \param[in] p_owner record input pin's owner: record filter
  \return return the instance of rec_in_pin_t
  */
rec_in_pin_t * record_in_pin_create(rec_in_pin_t *p_pin, interface_t *p_owner);

#endif // End for __RECORD_INPUT_PIN_INTRA_H_

