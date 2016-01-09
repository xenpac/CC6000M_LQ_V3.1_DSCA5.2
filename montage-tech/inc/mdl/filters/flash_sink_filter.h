/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FLASH_SINK_FILTER_H_
#define __FLASH_SINK_FILTER_H_



/*!
  file sink filter command define
  */
typedef enum tag_flash_sink_filter_cmd
{
  /*!
    config read file' name
    */
  FLASH_SINK_CFG,

}flash_sink_filter_cmd_t;

/*!
  file sink filter event define
  */
typedef enum tag_flash_sink_filter_evt
{
  /*!
    file sink get first data
    */
  FLASH_SINK_MSG_READY,

}flash_sink_filter_evt_t;

/*!
  create a file sink filter instance
  filetr ID:FILE_SINK_FILTER
  \return return the instance of fsrc_filter
  */
ifilter_t * flash_sink_filter_create(void *p_para);

#endif // End for __FILE_SINK_FILTER_H_
