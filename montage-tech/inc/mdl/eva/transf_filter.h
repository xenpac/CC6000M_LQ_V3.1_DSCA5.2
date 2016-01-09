/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TRANSF_FILTER_H_
#define __TRANSF_FILTER_H_

/*
The CTransformFilter class is a base class for implementing transform filters.
This class is designed for implementing a transform filter with one input 
pin and one output pin. 
It uses separate allocators for the input pin and the output pin. 
To create a filter that processes data in place, use the 
CTransInPlaceFilter class.

This filter uses the CTransformInputPin class for its input pin, 
and the CTransformOutputPin class for its output pin. 
Typically, you do not need to override these pin classes. 
Most of the pin methods call corresponding methods on the 
CTransformFilter class, so you can override the filter methods if necessary.
The filter creates both pins in the CTransformFilter::GetPin method. 
If you do override the pin classes, you must override GetPin 
to create your custom pins.

To use this class, derive a new class from 
CTransformFilter and implement the following methods: 

You might need to override other methods as well,
depending on the requirements of your filter.

*/

/*!
  private data length
  */
#define TRANSF_FILTER_PRIVAT_DATA (64)


/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_transf_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER ifilter_t _filter;

  /*!
    private data buffer
    */
  u8 private_data[TRANSF_FILTER_PRIVAT_DATA];

  /*!
    Receives a media sample, processes it, 
    and delivers an output sample to the downstream filter.
    The filter's input pin calls this method when it receives a sample.
    Then it calls the transform method, which the derived class must implement.
    The transform method processes the input data and produces output data.
    If the transform method returns S_FALSE, the Receive method drops this sample.
    and the filter sends an event out.
    Otherwise, if the transform method returns S_OK, the filter delivers
    the output sample.
    */
  PRIVATE RET_CODE (*receive)(handle_t _this, interface_t *p_sample);

  /*!
    at the start of a flush operation,
    the input pin's begin_flush method calls this method.
    This method passes the begin_flush call downstream
    */
  PRIVATE RET_CODE (*begin_flush)(handle_t _this);

  /*!
    at the end of a flush operation,
    the input pin's end_flush method calls this method.
    This method passes the end_flush call downstream.
    */
  PRIVATE RET_CODE (*end_flush)(handle_t _this);
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************

  /*!
    The Request method queues an asynchronous request for data.
    \param[in] _this this point.
    \param[in] p_format media format provided by the caller. 
    \param[in] timeout waiting timed out. 
    \param[in] position offset to stream header. 
    \param[in] context Specifies an arbitrary value that is returned 
      when the request completes
      
    \return return the request done
  */
  VIRTUAL RET_CODE (*on_request)(handle_t _this, media_format_t *p_format, u32 read_len,
                          u32 timeout, s64 position, u8 *p_user_buf, u32 context);
  
  /*!
    Transforms an input sample to produce an output sample. Virtual. 
    If the filter should not deliver this sample, the method should return ERR_FAILURE
    in this interface, you can get new buffer sample to transform or in place transform,
    and push it to down filter by output pin interface "push_down"
    */
  PURE VIRTUAL RET_CODE (*transform)(handle_t _this, media_sample_t *p_in);
}transf_filter_t;


/*!
  create transfer filter paramter.
  */
typedef struct tag_transf_filter_para
{
  /*!
    input para.
    */
  u32 dummy;
}transf_filter_para_t;

/*!
  create a transf_filter instance

  \param[in] p_filter instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter,

  \return return the instance of transf_filter
  */
transf_filter_t * transf_filter_create(transf_filter_t *p_filter,
                                            transf_filter_para_t *p_para);

#endif // End for __TRANSF_FILTER_H_

