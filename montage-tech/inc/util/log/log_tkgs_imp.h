/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LOG_AP_TKGS_H__
#define __LOG_AP_TKGS_H__

/*!
 
 */
typedef enum 
{
 /*!
  tkgs perf point start
  */ 
  PREF_START_POINT = 0x00,
 /*!
  tkgs perf point end
  */ 
  PREF_END_POINT   = 0x01,
 /*!
  tkgs perf max point
  */ 
  PREF_MAX_POINT   = 0x02, 
}tkgs_perf_point_t;

/*!
  tkgs perf event 
  */
typedef enum
{
 /*!
  start update
  */ 
  PREF_EVT_START_UPDATE_BEGIN,
  /*!
  start update
  */ 
  PREF_EVT_START_UPDATE_END,
 /*!
  get tp begin
  */ 
  PREF_EVT_GET_TP_BEGIN,
  /*!
  get tp end
  */ 
  PREF_EVT_GET_TP_END,
 /*!
  set tp begin
  */ 
  PREF_EVT_SET_TP_BEGIN,
  /*!
  set tp end
  */ 
  PREF_EVT_SET_TP_END,
 /*!
  tp locked
  */  
  PREF_EVT_NC_LOCKED,
 /*!
  tp unlocked
  */ 
  PREF_EVT_NC_UNLOCKED,
 /*!
  request single tkgs begin
  */ 
  PREF_EVT_REQ_SINGLE_TKGS_BEGIN,
   /*!
  request single tkgs end
  */ 
  PREF_EVT_REQ_SINGLE_TKGS_END,
 /*!
  all tkgs receive begin
  */ 
  PREF_EVT_RECE_ALL_TKGS_BEGIN,
   /*!
  all tkgs receive end
  */ 
  PREF_EVT_RECE_ALL_TKGS_END,
 /*!
  parse tkgs info begin
  */
  PREF_EVT_PARSE_TKGS_INFO_BEGIN,
   /*!
  parse tkgs info emd
  */
  PREF_EVT_PARSE_TKGS_INFO_END,
 /*!
  parse start info desc begin
  */ 
  PREF_EVT_START_INFO_DESC_BEGIN,
   /*!
  parse start info desc end
  */ 
  PREF_EVT_START_INFO_DESC_END,
 /*!
  parse broadcast source loop begin
  */ 
  PREF_EVT_BRDCAST_SRC_LOOP_BEGIN,
 /*!
  parse broadcast source loop end
  */ 
  PREF_EVT_BRDCAST_SRC_LOOP_END,
 /*!
  parse service list begin.
  */ 
  PREF_EVT_SERVICE_LIST_BEGIN,
   /*!
  parse service list end.
  */ 
  PREF_EVT_SERVICE_LIST_END,
 /*!
  parse category loop begin
  */ 
  PREF_EVT_CATEGORY_LOOP_BEGIN,
   /*!
  parse category loop end
  */ 
  PREF_EVT_CATEGORY_LOOP_END,
 /*!
  parse channel block begin
  */ 
  PREF_EVT_CHANNEL_BLOCK_BEGIN,
 /*!
  parse channel block end
  */ 
  PREF_EVT_CHANNEL_BLOCK_END,
 /*!
  save data begin
  */
  PREF_EVT_SAVE_DATA_BEGIN,
   /*!
  save data end
  */
  PREF_EVT_SAVE_DATA_END,
 /*!
  finished begin.
  */
  PERF_EVT_FINISHED_BEGIN,
  /*!
  finished end.
  */
  PERF_EVT_FINISHED_END,
}tkgs_perf_evt_t;


 /*!
   init
 */
handle_t log_ap_tkgs_init(void);

#endif // End for __LOG_AP_SCAN_H__

