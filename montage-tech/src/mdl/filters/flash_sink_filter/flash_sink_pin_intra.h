/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FLASH_SINK_PIN_INTRA_H_
#define __FLASH_SINK_PIN_INTRA_H_


/*!
  file sink pin private data
  */
typedef struct tag_flash_sink_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
}flash_sink_pin_private_t;


/*!
  the input pin
  */
typedef struct tag_flash_sink_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_pin_t base_pin;
  /*!
    private data
    */
  flash_sink_pin_private_t private_data;
}flash_sink_pin_t;

/*!
  create a pin

  \return return the instance of usb_pin_t
  */
flash_sink_pin_t * flash_sink_pin_create(flash_sink_pin_t *p_pin, interface_t *p_owner);

#endif // End for __USB_PIN_INTRA_H_
