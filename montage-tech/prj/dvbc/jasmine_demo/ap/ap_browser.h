/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_BROWSER_H_
#define __AP_BROWSER_H_

//#include "browser_adapter.h"

#define AP_BROWSER_ID       BROWSER_ID_IPANEL28


/*!
  Browser policy
  */
typedef struct
{
  /*!
    on initialized
    */
  void (*on_init)(void);
  
  /*!
    for test ca performance
    */
  void (*test_browser_performance)(u32 ticks);
 } browser_policy_t;

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
    Send irkey msg
    */
  CMD_SEND_IRKEY_MSG,
  /*!
    stop
    */
  CMD_STOP_BROWSER = SYNC_CMD,
}browser_cmd_t;

typedef enum
{
  /*!
    EVT_TEST
    */
  BROWSER_EVT_START = ((APP_SI_MON << 16) | ASYNC_EVT),
   BROWSER_EVT_EXIT,

  /*!
    Notify UI that current application browse module is ready to stop 
    */
  BROWSER_EVT_STOP = ((APP_SI_MON << 16) | SYNC_EVT),
}browser_evt_t;

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
  Browser input parameter
  */
typedef struct
{
 u16 net_id;//network id
 u16 ts_id;
 u16 s_id;//service id
}browser_pg_para;

/*!
  The structure used for io control command
  */
typedef struct
{
  /*!
      The specific browser module id, see browser_module_id_t
    */
  browser_module_id_t browser_id;
  /*!
      Browser key event
    */
  u32 key_event;
  /*!
      Browser key event
    */
  u32 mem_size;
  /*!
      The parameter corresponding to the io control command
    */
  void *p_param;
}browser_io_param_t;
/*!
  The browser module configuation parameters
  */
typedef struct
{
 /*!
    The callback of get freq function
    */
    u8 task_prio;
    u8 task_end_pro;
 ;
}browser_module_cfg_t;


/*!
  Singleton patern interface which can get a scan application's
  instance
  
  \return instance address
  */
app_t *construct_ap_browser(browser_policy_t *p_policy);
#endif

