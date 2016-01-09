/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __STR_OUT_PIN_INTRA_H_
#define __STR_OUT_PIN_INTRA_H_

/*!
  private data
  */
typedef struct
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    define format
    */
  allocator_properties_t properties;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
}str_out_pin_private_t;

/*!
  the record output pin define
  */
typedef struct
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data buffer
    */
  str_out_pin_private_t private_data;

//  u8 * (*alloc_buff)(u16 h,u16  w, dep);
}str_out_pin_t;

/*!
  set properties.

  \param[in] p_output_pin   output pin.
  \param[in] buffer_size    buffer size.
  \param[in] p_buffer       buffer.
  */
void str_out_pin_cfg(str_out_pin_t *p_output_pin, u32 buffer_size, void *p_buffer);

/*!
  create a pin

  \return return the instance of char_out_pin_t
  */
str_out_pin_t * str_out_pin_create(str_out_pin_t *p_pin, interface_t *p_owner);

#endif // End for __CHAR_OUT_PIN_INTRA_H_
