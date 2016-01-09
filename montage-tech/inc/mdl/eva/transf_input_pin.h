/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TRANSF_INPUT_PIN_H_
#define __TRANSF_INPUT_PIN_H_

/*!
  private data length
  */
#define TRANSF_INPUT_PIN_PRIVAT_DATA (16)

/*!
  transfer input pin define
  */
typedef struct tag_transf_input_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER base_input_pin_t m_pin;
  
  /*!
    private data buffer
    */
  u8 private_data[TRANSF_INPUT_PIN_PRIVAT_DATA];
  
}transf_input_pin_t;

/*!
  create transfer input pin parameter define
  */
typedef struct tag_transf_input_pin_para
{
  /*!
    Pointer to the filter that created the pin. 
    */
  interface_t *p_filter;
  /*!
    name
    */
  char *p_name;
}transf_input_pin_para_t;

/*!
  create a transfer input pin instance

  \param[in] p_pin instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of transfer input pin
  */
transf_input_pin_t * transf_input_pin_create(
      transf_input_pin_t *p_pin, transf_input_pin_para_t *p_para);

#endif // End for __TRANSF_INPUT_PIN_H_


