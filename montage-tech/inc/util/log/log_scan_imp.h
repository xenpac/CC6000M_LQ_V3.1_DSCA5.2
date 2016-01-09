/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_AP_SCAN_H__
#define __LOG_AP_SCAN_H__
/*!
  store lock tp in database
  */
#define LOCK_TP_DB   (1)
/*!
  store unlock tp in database
  */
#define UNLOCK_TP_DB (2)
/*!
  store programme in database
  */
#define PROG_DB      (3)

/*!
  performace check point
  */
typedef enum
{
  /*!
    check point pat request
    */
  PERF_PAT_REQUEST = 0,
  /*!
    check point pmt request
    */
  PERF_PMT_REQUEST,
  /*!
    check point sdt request
    */
  PERF_SDT_REQUEST,
  /*!
    check point nit request
    */
  PERF_NIT_REQUEST,
  /*!
    check point bat request
    */
  PERF_BAT_REQUEST,
  /*!
    check point cat request
    */
  PERF_CAT_REQUEST,
  /*!
    check point pat found
    */
  PERF_PAT_FOUND,
  /*!
    check point pmt found
    */
  PERF_PMT_FOUND,
  /*!
    check point sdt found
    */
  PERF_SDT_FOUND,
  /*!
    check point nit found
    */
  PERF_NIT_FOUND,
  /*!
    bat found
    */
  PERF_BAT_FOUND,
  /*!
    cat found
    */
  PERF_CAT_FOUND,
  /*!
    do lock
    */
  PERF_DO_LOCK,
  /*!
    lock evt
    */
  PERF_LOCK_EVT,
  /*!
    unlock evt
    */
  PERF_UNLOCK_EVT,
  /*!
    do pre scan 
  */
  PERF_PRE_SCAN,
  /*!
    pre scan evt
  */
  PERF_SCAN_EVT,
  /*!
    do get tp begin 
  */
  PERF_GET_TP_BEGIN,
  /*!
    do get tp end 
  */
  PERF_GET_TP_END,
  /*!
    do get one tp begin 
  */
  PERF_GET_ONETP_BEGIN,
  /*!
    do get one tp end 
  */
  PERF_GET_ONETP_END,
  /*!
    start save db
    */
  PERF_SAVE_DB_START,
  /*!
    end save db
    */
  PERF_SAVE_DB_DONE,
    /*!
    start performance
    */
  PERF_BEGIN,
  /*!
    end performance
    */
  PERF_END,
  /*!
    scan finish
  */
  PERF_FINISH
  
}perf_check_point_t;

/*!
  tp's breif message
*/
typedef struct tps_t
{
  /*!
  tp's frequency
  */
  u32 freq;
  /*!
  tp's polar
  */
  u8 polar; //0:nothing 1:horiential 2:vertical 
  /*!
  tp's symbrate
  */
  u32 symbrate;
  /*!
  whether the tp is leaked?
  */
  BOOL leak_flag;
}tps_t;

 /*!
   init
 */
handle_t log_ap_scan_init(void);

#endif // End for __LOG_AP_SCAN_H__

