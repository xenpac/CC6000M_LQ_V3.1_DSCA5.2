/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_AP_EPG_H__
#define __LOG_AP_EPG_H__

/*!
  performance epg check point
  */
typedef enum
{
/*!
  check set header start
*/
  PERF_DM_SETHEADER_BEGIN = 0,
/*!
  check set header end
*/
  PERF_DM_SETHEADER_END,
/*!
  check write node to flash start
*/
  PERF_DM_WRITE_TO_FLASH_BEGIN,
/*!
  check write node to flash end
*/
  PERF_DM_WRITE_TO_FLASH_END,
/*!
  check arrange flash start
*/
  PERF_DM_ARRANGE_FLASH_BEGIN,
/*!
  check arrange flash end
*/
  PERF_DM_ARRANGE_FLASH_END,
/*!
  show sec arrange time
*/
  PERF_DM_ARRANGE_FLASH,
/*!
  show sec set header time
*/
  PERF_DM_SET_HEADER
}perf_epg_check_point_t;
/*!
  init
*/
handle_t log_dm_init(void);

#endif //End for __LOG_AP_EPG_H__
