/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

// base class for logger, scan, flash, nim_svc ... loggers will derived from
// this class

/*!
  log for performance
  */
typedef struct
{
  /*!
    log interface
    */
  void (*log)(handle_t handle, const char *p_file_name, u32 line, u32 event, u32 p1, u32 p2);
/*!
  send one key
  */
  void (*send_one_key)(handle_t handle);
/*!
  log data
  */
  void *p_data;
} log_t;

#endif // End for __LOG_H__

