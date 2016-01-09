/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DMX_FILTER_INTRA_H_
#define __DMX_FILTER_INTRA_H_


/*!
  declare
  */
struct tag_dmx_filter;


/*!
  define dmx filter private type.
  */
typedef struct tag_dmx_filter_private
{
 /*!
  this point !!
  */
  struct tag_dmx_filter *p_this;

 /*!
  it's output pin
  */
  demux_pin_t m_pin;
}demux_filter_private_t;


/*!
  the demux filter define
  */
typedef struct tag_dmx_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER src_filter_t m_filter;
  
  /*!
    private data
    */
  demux_filter_private_t private_data;
}demux_filter_t;

#endif // End for __DMX_FILTER_INTRA_H_

