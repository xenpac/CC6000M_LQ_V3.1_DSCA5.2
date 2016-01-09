/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_SINK_FILTER_INTRA_H_
#define __FILE_SINK_FILTER_INTRA_H_

/*!
  private data
  */
typedef struct tag_flash_sink_filter_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    input pin
    */
  flash_sink_pin_t m_pin;
}flash_sink_filter_private_t;

/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_flash_sink_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_filter_t _sink_filter;
  
  /*!
    private data
    */
  flash_sink_filter_private_t private_data;
}flash_sink_filter_t;

#endif // End for __FILE_SINK_FILTER_INTRA_H_
