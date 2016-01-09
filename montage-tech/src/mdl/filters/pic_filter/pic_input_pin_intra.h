/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_IN_PIN_INTRA_H_
#define __PIC_IN_PIN_INTRA_H_

/*!
  private data
  */
typedef struct tag_pic_in_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    jpeg info
    */
  pic_info_t jpeg_info;    
  /*!
    mem alloc interface
    */
  iasync_reader_t *p_reader;
  /*!
    buffer
    */
  u8 *p_pic_buf;
  /*!
    buffer size
    */
  u32 pic_buff_size;
  /*!
	  current piece
  	*/
  u32 current_piece;
  /*!
    is pull mode.
    */
  BOOL is_push;    
}pic_in_pin_private_t;


/*!
  the pic input pin
  */
typedef struct tag_pic_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data buffer
    */
  pic_in_pin_private_t private_data;
}pic_in_pin_t;

/*!
  config input pin
  */
void pic_in_pin_cfg(pic_in_pin_t *p_this, u32 buffer_size, u8 *p_buffer, BOOL is_push);

/*!
  create a pin

  \return return the instance of pic_in_pin_t
  */
pic_in_pin_t * pic_in_pin_create(pic_in_pin_t *p_pin, interface_t *p_owner);

#endif // End for __PIC_IN_PIN_INTRA_H_
