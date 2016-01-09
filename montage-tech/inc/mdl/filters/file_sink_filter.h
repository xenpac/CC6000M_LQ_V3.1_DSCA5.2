/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_SINK_FILTER_H_
#define __FILE_SINK_FILTER_H_

/*!
  fsink_filter_sink_mode
  */
typedef enum tag_fsink_filter_sink_mode
{
  /*!
    sink to file
    */
  FSINK_SINK_TO_FILE = 0,
  /*!
    sink to block
    */
  FSINK_SINK_TO_BLOCK
}fsink_filter_sink_mode_t;

/*!
  file sink filter command define
  */
typedef enum tag_fsink_filter_cmd
{
  /*!
    config read file' name
    */
  FSINK_CFG_FILE_NAME,
  /*!
    config sink mode
    */
  FSINK_CFG_SINK_MODE,
  /*!
    config sink size
    */
  FSINK_CFG_SINK_SIZE,
  /*!
    seek : paramter see
    */
  FSINK_SEEK,
}fsink_filter_cmd_t;

/*!
  file sink filter event define
  */
typedef enum tag_fsink_filter_evt
{
  /*!
    file sink get first data
    */
  FILE_SINK_MSG_READY = FILE_SINK_FILTER << 16,
  /*!
    no enough mem
    */
  FILE_SINK_MSG_NO_ENOUGH_MEM,
  /*!
    block write back
    */
  FILE_SINK_MSG_BLOCK_WRITE_BACK,
  /*!
    block write REACH read
    */
  FILE_SINK_MSG_BLOCK_WRITE_REACH_READ,
  /*!
    record save file, when rec file is 4G
    */
  FILE_SINK_MSG_SAVE_FILE,
  /*!
    record write error
    */
  FILE_SINK_MSG_WRITE_ERROR,
}fsink_filter_evt_t;

/*!
  create a file sink filter instance
  filetr ID:FILE_SINK_FILTER
  \return return the instance of fsrc_filter
  */
ifilter_t * fsink_filter_create(void *p_para);

#endif // End for __FILE_SINK_FILTER_H_
