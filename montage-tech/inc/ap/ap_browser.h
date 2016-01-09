/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_BROWSER_H_
#define __AP_BROWSER_H_

/*!
  the ap browser task temp define
  */
#define APP_BROWSER (APP_RESERVED1)

/*!
  Application browser definion
  */
typedef enum
{
  /*!
    EVT_TEST
    */
  BROWSER_EVT_START = ((APP_BROWSER << 16) | ASYNC_EVT),
  /*!
    Notify UI that current application browse module is ready to stop 
    */
  BROWSER_EVT_STOP = ((APP_BROWSER << 16) | SYNC_EVT),
}browser_evt_t;

/*!
  browser state definition
  */
typedef enum
{
  /*!
    TEST state
    */
  BROWSER_STATE_TEST = 0,
  
  BROWSER_STATE_UNKNOWN,  
} browser_state_t;

/*!
  Delcare command 
  */
typedef enum
{
  /*!
    Start 
    */
  CMD_START_BROWSER = ASYNC_CMD,
  /*!
    stop
    */
  CMD_STOP_BROWSER = SYNC_CMD,
}browser_cmd_t;

/*!
  browser_tp_info
  */
typedef struct browser_tp_info
{
  /*!
    Channel frequency in KHz
    */    
  u32 frequency;
  /*!
    symbol rate in Symbols per second, in KSs
    */
  u32 symbol_rate;
  /*!
    modulation type, see enum nim_modulation
    */    
  u32  modulation;
}browser_tp_info_t;

/*!
  Singleton patern interface which can get a scan application's
  instance
  
  \return instance address
  */
app_t *construct_ap_browser(void);

#endif

