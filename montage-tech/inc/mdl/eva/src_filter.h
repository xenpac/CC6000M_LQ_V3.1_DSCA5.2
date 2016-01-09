/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SRC_FILTER_H_
#define __SRC_FILTER_H_

/*
The CSource class is a base class for implementing source filters.
A filter derived from CSource contains one or more output pins derived from
the CSourceStream class. Each output pin creates a worker thread that pushes 
media samples downstream. 

To implement an output pin, do the following: 

Derive a class from CSourceStream. 
Override the CSourceStream::GetMediaType method and possibly 
the CSourceStream::CheckMediaType method, which validate media types for the pin. 
Implement the CBaseOutputPin::DecideBufferSize method, 
which returns the pin's buffer requirements. 
Implement the CSourceStream::FillBuffer method, 
which fills a media sample buffer with data. 
To implement the filter, do the following: 

Derive a class from CSource. 
In the constructor, create one or more output pins derived from CSourceStream.
The pins automatically add themselves to the filter in their constructor 
methods, and remove themselves in their destructor methods. 
*/

/*!
  private data length
  */
#define SRC_FILTER_PRIVAT_DATA (64)


/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_src_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER ifilter_t _filter;
  
  /*!
    private data buffer
    */
  u8 private_data[SRC_FILTER_PRIVAT_DATA];
  
  /*!
    The get_request method waits for the next thread request
    */
  PRIVATE RET_CODE (*get_request)(handle_t _this, u32 cmd);
}src_filter_t;


/*!
  create source filter paramter.
  */
typedef struct tag_src_filter_para
{
  /*!
    dummy
    */
  u32 dummy;
}src_filter_para_t;

/*!
  create a src_filter instance

  \param[in] p_filter instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter, if it's NULL means the filter is deactived.

  \return return the instance of src_filter
  */
src_filter_t * src_filter_create(src_filter_t *p_filter,
                                      src_filter_para_t *p_para);

#endif // End for __SRC_FILTER_H_

