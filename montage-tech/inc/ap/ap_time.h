/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_TIME_H_
#define __AP_TIME_H_

/*!
  \file ap_time.h
  Delecalre time application.
  And this is a base class for all time applications.
  */


/*!
  Application time definion
  */
typedef enum
{
  /*!
    Time update.
    */
  TIME_EVT_TIME_UPDATE = ((APP_TIME << 16)|ASYNC_EVT),
  /*!
    LNB power short.
    */
  TIME_EVT_LNB_SHORT,
  /*!
    LNB power no short.
    */
  TIME_EVT_LNB_NO_SHORT,
  /*!
    Time heart beat
    */
  TIME_EVT_HEART_BEAT,
  /*!
    Notify UI that current application EPG module is ready to stop
    */
  TIME_EVT_STOP =((APP_TIME << 16)|SYNC_EVT)
}time_evt_t;

/*!
  Max message number supported in APP. TIME
  */
#define AP_TIME_MSG_MAX_NUM  (8)

/*!
  Command definition in application Time
  */
typedef enum
{
  /*!
    Start ap time.
    */
  TIME_CMD_START = ASYNC_CMD,
  /*!
    open heart beat
    */
  TIME_CMD_OPEN_HEART,
  /*!
    close heart beat
    */
  TIME_CMD_CLOSE_HEART,  
  /*!
    open net time process
    */
  TIME_CMD_OPEN_NTP,
  /*!
    close net time process
    */
  TIME_CMD_CLOSE_NTP,  
  /*!
    Request tdt.
    */
  TIME_CMD_REQ_TDT,
  /*!
    Release from LNB short circuit protection
    */
  TIME_CMD_LNB_CHECK_ENABLE,
  /*!
    Time source update
    */
  TIME_CMD_UPDATE_SOURCE,
  /*!
    Stop ap time.
    */
  TIME_CMD_STOP = SYNC_CMD
}time_cmd_t;



/*!
  Implementation policy definition 
  */
typedef struct
{
  /*!
    Implementation policy for ap Time tdt found.
    */
  void (*on_time_tdt_found)(u32 para1, u32 para2);
  /*!
    Implementation policy for ap Time tdt found.
    */
  BOOL (*on_time_update_check)(u32 para1, u32 para2);
  /*!
    Implementation policy for ap Time lnb protect check.
    */
  void (*on_time_lnb_check)(u32 para1, u32 para2);  
  /*!
    Implementation policy for ap Time enable lnb protect check.
    */
  void (*on_time_lnb_check_enable)(u32 para1, u32 para2);
  /*!
    Implementation policy for ap Time enable lnb protect check.
    */
  u32 (*ntp_time_get)(void);
  /*!
    Implementation policy for ap Time enable lnb protect check.
    */
  u32 (*ntp_time_update)(u32 para1, u32 para2);
  /*!
    tuner id
    */
  u8 tuner_id; 
  /*!
    Private data for time policy
    */
  void *p_data;  
}time_policy_t;

/*!
  Get the instance of application Time

  \param[in] p_policy Reserved, not being used
  
  \return application Time instance
  */
app_t *construct_ap_time(time_policy_t *p_policy);

/*!
  if use ntp time get, should attach this function at module construct
  
  \return time value
  */
u32 ntp_time_get(void);

#endif // End for _AP_TIME_H_
