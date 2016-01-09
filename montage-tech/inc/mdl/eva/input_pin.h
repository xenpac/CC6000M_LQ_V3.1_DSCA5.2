/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __INPUT_PIN_H_
#define __INPUT_PIN_H_

/*!
  private data length
  */
#define BASE_INPUT_PIN_PRIVAT_DATA (1600)

/*!
  base input pin define
  */
typedef struct tag_base_input_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER ipin_t _ipin;
  
  /*!
    private data buffer
    */
  u8 private_data[BASE_INPUT_PIN_PRIVAT_DATA];
  
  /*!
    receive a sample. can't overload .
    */
  PRIVATE void (*receive)(handle_t _this, interface_t *p_buffer_sample);
  
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************

  /*!
    receive a sample. Virtual.
    */
  VIRTUAL void (*on_receive)(handle_t _this, media_sample_t *p_sample);
  
  /*!
    notify the allocator. Pure virtual.
    */
  VIRTUAL BOOL (*notify_allocator)(handle_t _this,
                            imem_allocator_t *p_alloc, BOOL read_only);

}base_input_pin_t;

/*!
  create input pin parameter define
  */
typedef struct tag_input_pin_para
{
  /*!
    Pointer to the filter that created the pin. 
    */
  interface_t *p_filter;
}input_pin_para_t;

/*!
  create a base_input_pin instance

  \param[in] p_pin instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of base_input_pin
  */
base_input_pin_t * base_input_pin_create(
      base_input_pin_t *p_pin, input_pin_para_t *p_para);

#endif // End for __INPUT_PIN_H_

