/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SINK_PIN_H_
#define __SINK_PIN_H_

/*!
  private data length
  */
#define SINK_PIN_PRIVAT_DATA (16)

/*!
  sink pin define
  */
typedef struct tag_sink_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER base_input_pin_t m_pin;
  
  /*!
    private data buffer
    */
  u8 private_data[SINK_PIN_PRIVAT_DATA];

  /*!
    use async render to push the sample.
    */
  RET_CODE (*async_render)(handle_t _this, media_sample_t *p_sample, u32 context);
  
  /*!
    use the method to flush the unprocess sample on render robot
    */
  RET_CODE (*flush_async_render)(handle_t _this);
  
  /*!
    use the method to hold  the unprocess sample on render robot
    */
  RET_CODE (*hold_async_render)(handle_t _this);
  
  /*!
    use the method to unhold the unprocess sample on render robot
    */
  RET_CODE (*unhold_async_render)(handle_t _this);
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************
  
  /*!
    when using async render feature, you must implement this interface
    to check the hardware is idle or not.
    */
  VIRTUAL BOOL (*hw_ready)(handle_t _this, media_sample_t *p_sample, u32 context);
  
  /*!
    when using async render feature, you must implement this interface
    to render the sample.
    */
  VIRTUAL BOOL (*render_sample)(handle_t _this, media_sample_t *p_sample, u32 context);
}sink_pin_t;

/*!
  create sink pin parameter define
  */
typedef struct tag_sink_pin_para
{
  /*!
    Pointer to the filter that created the pin. 
    */
  interface_t *p_filter;
  /*!
    name
    */
  char *p_name;
}sink_pin_para_t;

/*!
  create a sink_pin instance

  \param[in] p_pin instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of sink_pin
  */
sink_pin_t * sink_pin_create(
      sink_pin_t *p_pin, sink_pin_para_t *p_para);

#endif // End for __SINK_PIN_H_


