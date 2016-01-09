/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_OUTPUT_PIN_INTRA_H_
#define __OTA_OUTPUT_PIN_INTRA_H_

/*!
 ota output pin private data.
 */
typedef struct tag_ota_output_pin_priv
{
/*!
 ota input pin handle.  
 */  
 void *p_this;
  /*!
    out sample 
    */
  media_sample_t *p_out_sample;
  /*!
    out buf 
    */
  u8 *p_out_buf;

  /*!
    define format
    */
  allocator_properties_t properties;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
}ota_output_pin_priv_t;

/*!
 ota pin 
 */
typedef struct tag_ota_output_pin
{
 /*!
  base class.
  */  
  FATHER transf_output_pin_t m_sink_pin;
 /*!
  private data.
  */
  ota_output_pin_priv_t    m_priv_data;
}ota_output_pin_t;


/*!
 create ota input pin

 \param[in] p_pin : ota input pin handle.
 \param[in] p_owner : ota pin's owner.
 \return ota_pin_t handle
 */
ota_output_pin_t *ota_output_pin_create(ota_output_pin_t *p_pin, interface_t *p_owner);

#endif //End for __OTA_OUTPUT_PIN_INTRA_H_
