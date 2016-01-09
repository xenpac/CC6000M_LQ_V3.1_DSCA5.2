/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SIGNAL_MONITOR_H_
#define __AP_SIGNAL_MONITOR_H_

/*!
  \file ap_signal_monitor.h
  
  This file implements the fucntion of monitoring signal information in certain 
  frequency and notify above layer the signal information
  */

/*!
  Definition of command to signal monitor task
  */
typedef enum
{
  /*!
    Start signal monitor API
    */
  SIG_MON_CMD_START = 0,
  /*!
    Check signal performance command
    */
  SIG_MON_MONITOR_PERF,
  /*!
    Reset signal monitor command
    */
  SIG_MON_RESET,
  /*!
    motor drv step by step
    */
  SIG_MON_MOTOR_DRV_STEP = SYNC_CMD,
  /*!
    motor drv auto search.
    */
  SIG_MON_MOTOR_DRV_AUTO,
  /*!
    motor drv stop.
    */
  SIG_MON_MOTOR_DRV_STOP,
  /*!
    LOCK tp commands
    */
  SIG_MON_LOCK_TP,
} sig_mon_cmd_t;

/*!
  Event definition of signal monitor
  */
typedef enum
{
  /*!
    Signal monitor begin event 
    */
  SIG_MON_EVT_BEGIN = (APP_SIGNAL_MONITOR << 16),
  /*!
    Signal information notify event
    */
  SIG_MON_SIGNAL_INFO,
  /*!
    Signal position turning 
    */
  SIG_MON_POS_TURNING,
  /*!
    Signal out of range 
    */
  SIG_MON_POS_OUT_RANGE,
  /*!
   Signal locked
   */
  SIG_MON_LOCKED,
  /*!
   Signal locked
   */
  SIG_MON_UNLOCKED,
  /*!
    drv locked.
    */
  SIG_MON_DRV_LOCKED,    
  /*!
    ack the sync cmd SIG_MON_MOTOR_DRV_STEP
    */
  SIG_MON_DRV_STEP = ((APP_SIGNAL_MONITOR<< 16) | SYNC_EVT),
  /*!
    ack the sync cmd SIG_MON_MOTOR_DRV_STEP
    */
  SIG_MON_DRV_AUTO,
  /*!
    ack the sync cmd SIG_MON_MOTOR_DRV_STOP
    */  
  SIG_MON_DRV_STOP,
  /*!
    ack the sync cmd SIG_MON_MOTOR_DRV_STOP
    */  
  SIG_MON_SYNC_LOCK
} sig_mon_evt_t;

/*!
  Signal information status definition
  */
typedef enum
{
  /*!
    No signal status
    */
  SF_NO_SIGNAL,
  /*!
    No ts input status
    */
  SF_NO_TS_INPUT,
  /*!
    Signal stable status
    */
  SF_SIGNAL_STABLE
} signal_info_t;

/*!
  Signal information
  Notice: The size of this structure is must less then 4 bytes.
  */
typedef struct
{
  /*!
    Signal noise rate, 0 ~ 100 
    */ 
  u8 snr;
  /*!
    Bit error rate, 0 ~ 100 
    */
  u8 ber;
  /*!
    Signal streagth, 0 ~ 100 
    */
  u8 strength;
  /*!
    tuner id
    */ 
  u8 tuner_id;
  /*!
    Bit error rate, normally it is a very little number.for dvb-c
    */    
    double ber_c;  
  /*!
    tp freq
    */
  u32 tp_freq;
} sig_mon_info_t;

/*!
  Implementation policy for signal monitors
  */
typedef struct 
{
  /*!
    convert signal information
    \see convert_perf
    */
  void (*p_convert_perf)(nim_channel_perf_t *p_signal, sig_mon_info_t *p_info);
  /*!
     Lock LED status control
     \param[in] locked  locked will be true
    */
  void (*lock_led_set)(BOOL locked,u8 tuner_id);
}sig_mon_policy_t;

/*!
  Get the implementation of signal monitor
  return signal monitor application instance
  \param[in] p_impl_policy implementation policy for signal monitor
  */
app_t *construct_ap_signal_monintor(sig_mon_policy_t *p_impl_policy);

#endif // End for __AP__H_
