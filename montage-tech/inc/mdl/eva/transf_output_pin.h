/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TRANSF_OUTPUT_PIN_H_
#define __TRANSF_OUTPUT_PIN_H_

/*!
  private data length
  */
#define TRANSF_OUTPUT_PIN_PRIVAT_DATA (64)

/*!
  transfer output pin define
  */
typedef struct tag_transf_output_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER base_output_pin_t m_pin;
  
  /*!
    private data buffer
    */
  u8 private_data[TRANSF_OUTPUT_PIN_PRIVAT_DATA];

  /*!
    use this method to qurey new sample
    */
  RET_CODE (*query_sample)(handle_t _this, media_sample_t **pp_sample);

  /*!
    if you query an new sample, but don't push down. can use this method to release it.
    */
  RET_CODE (*release_sample)(handle_t _this, media_sample_t *p_sample);
  
  /*!
    use this method to push ready sample down
    */
  RET_CODE (*push_down)(handle_t _this, media_sample_t *p_sample);
  
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************
}transf_output_pin_t;

/*!
  create transfer output pin parameter define
  */
typedef struct tag_transf_output_pin_para
{
  /*!
    Pointer to the filter that created the pin. 
    */
  interface_t *p_filter;
  /*!
    name
    */
  char *p_name;
}transf_output_pin_para_t;

/*!
  create a transfer output pin instance

  \param[in] p_pin instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of transfer output pin
  */
transf_output_pin_t * transf_output_pin_create(
      transf_output_pin_t *p_pin, transf_output_pin_para_t *p_para);

#endif // End for __TRANSF_OUTPUT_PIN_H_


