/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __IASYNC_READER_H_
#define __IASYNC_READER_H_

/*
  The iasync_reader interface performs an asynchronous data request 
  on a filter.
  
  This interrace is exposed by output pins that perform asynchronous 
  read operations. The interface is used by the input pin on the 
  downstream filter. Applications do not use this interface. 

  Filter developers: Implement this interface if your output pin delivers 
  data in the form of a byte stream and supports the pull model.
*/

/*!
  ias_reader
  */
#define IASYNC_READER_INTERFACE "ias_reader"

/*!
  private data length
  */
#define IASYNC_READER_PRIVAT_DATA (64)

/*!
  async reader interface define
  */
typedef struct tag_iasync_reader
{
  /*!
    private data buffer
    */
  u8 private_data[IASYNC_READER_PRIVAT_DATA];

  /*!
    The get unprocess sample.
    \param[in] _this this point.
    \param[in] p_sample unprocess sample. 
      
    \return return the result
  */
  PRIVATE RET_CODE (*get_unprocess_sample)(handle_t _this,void *p_sample);

  /*!
    The get unprocess sample.
    \param[in] _this this point.
      
    \return return the master
  */
  interface_t * (*get_master)(handle_t _this);

  /*!
    The Request method queues an asynchronous request for data.
    \param[in] _this this point.
    \param[in] p_format media format provided by the caller. 
    \param[in] read_len read length. 
    \param[in] timeout waiting timed out. 
    \param[in] position offset to stream header. 
    \param[in] p_user_buf:
      if it is 0, system will use output_pin's sample buffer
      if it isn't 0, will use the user bufer and ignor output_pin's sample buffer
    \param[in] context Specifies an arbitrary value that is returned 
      when the request completes

    \return return the request done
  */
  RET_CODE (*request)(handle_t _this, media_format_t *p_format, u32 read_len,
                          u32 timeout, s64 position, u8 *p_user_buf, u32 context);

  /*!
  The SyncRead method performs a synchronous read.
  The method blocks until the request is completed.
  */
  RET_CODE (*sync_read)(handle_t _this, media_sample_t *p_sample, u32 read_len,
                          s64 position);
}iasync_reader_t;

#endif // End for __IASYNC_READER_H_

