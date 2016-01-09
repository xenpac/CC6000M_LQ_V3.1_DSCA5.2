/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_OUT_PIN_INTRA_H_
#define __PIC_OUT_PIN_INTRA_H_

/*!
  private data
  */
typedef struct tag_pic_out_pin_private
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
}pic_out_pin_private_t;

/*!
  the record output pin define
  */
typedef struct tag_pic_out_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data
    */
  pic_out_pin_private_t private_data;
}pic_out_pin_t;

/*!
  set properties.

  \param[in] p_output_pin   output pin.
  \param[in] buffer_size    buffer size.
  \param[in] p_buffer       buffer.
  */
void pic_out_pin_cfg(pic_out_pin_t *p_output_pin, u32 buffer_size, void *p_buffer);

/*!
  create a pin

  \return return the instance of pic_out_pin_t
  */
pic_out_pin_t * pic_out_pin_create(pic_out_pin_t *p_pin, interface_t *p_owner);

#endif // End for __PIC_OUT_PIN_INTRA_H_
