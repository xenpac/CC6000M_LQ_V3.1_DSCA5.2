/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SOURCE_PIN_H_
#define __SOURCE_PIN_H_

/*!
  private data length
  */
#define SOURCE_PIN_PRIVAT_DATA (64)

/*!
  source pin define
  */
typedef struct tag_source_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER base_output_pin_t m_pin;
  
  /*!
    private data buffer
    */
  u8 private_data[SOURCE_PIN_PRIVAT_DATA];

  /*!
    process a sample. can't overload it.
    */
  PRIVATE RET_CODE (*source_creator)(handle_t _this);
  
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************
}source_pin_t;

/*!
  create source pin parameter define
  */
typedef struct tag_source_pin_para
{
  /*!
    Pointer to the filter that created the pin. 
    */
  interface_t *p_filter;
  /*!
    working mode
    */
  stream_mode_t stream_mode;
  /*!
    name
    */
  char *p_name;
}source_pin_para_t;

/*!
  create a source_pin instance

  \param[in] p_pin instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of source_pin_t
  */
source_pin_t * source_pin_create(
      source_pin_t *p_pin, source_pin_para_t *p_para);

#endif // End for __SOURCE_PIN_H_


