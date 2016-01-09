/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_SRC_FILTER_INTRA_H_
#define __FILE_SRC_FILTER_INTRA_H_

/*!
  private data
  */
typedef struct tag_fsrc_filter_private
{
  /*!
    this point !!
    */
  void *p_this;

  /*!
    it's output pin
    */
  fsrc_pin_t m_pin;
}fsrc_filter_private_t;


/*!
  the file source filter define
  */
typedef struct tag_fsrc_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER src_filter_t m_filter;
  /*!
    private data
    */
  fsrc_filter_private_t private_data;
}fsrc_filter_t;

#endif // End for __FILE_SRC_FILTER_INTRA_H_

