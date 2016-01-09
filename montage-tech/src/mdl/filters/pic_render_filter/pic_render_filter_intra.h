/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_RENDER_FILTER_INTRA_H_
#define __PIC_RENDER_FILTER_INTRA_H_

/*!
  private data
  */
typedef struct tag_pic_render_filter_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    input pin
    */
  pic_render_pin_t m_pin;
}pic_render_filter_private_t;

/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_pic_render_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_filter_t _sink_filter;
  /*!
    private data
    */
  pic_render_filter_private_t private_data;
}pic_render_filter_t;

#endif // End for __PIC_RENDER_FILTER_INTRA_H_
