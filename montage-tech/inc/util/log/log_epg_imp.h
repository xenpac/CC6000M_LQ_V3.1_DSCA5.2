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
  check one section start
*/
  PERF_20_EVTS_BEGIN = 0,
/*!
  check one section end
*/
  PERF_20_EVTS_END

}perf_epg_check_point_t;
/*!
  init
*/
handle_t log_ap_epg_init(void);

#endif //End for __LOG_AP_EPG_H__