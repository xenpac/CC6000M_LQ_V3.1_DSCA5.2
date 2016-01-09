/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __STR_IN_PIN_INTRA_H_
#define __STR_IN_PIN_INTRA_H_

/*!
  private data length
  */
typedef struct
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    jpeg info
    */
//  char_info_t jpeg_info;    
  /*!
    mem alloc interface
    */
  iasync_reader_t *p_reader;
}str_in_pin_private_t;

/*!
  the char input pin
  */
typedef struct
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data buffer
    */
  str_in_pin_private_t private_data;
}str_in_pin_t;

/*!
  create a pin

  \return return the instance of char_in_pin_t
  */
str_in_pin_t * str_in_pin_create(str_in_pin_t *p_pin, interface_t *p_owner);

#endif // End for __CHAR_IN_PIN_INTRA_H_
