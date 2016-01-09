/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_NVOD_H_
#define __AP_NVOD_H_

/*!
  Application nvod definion
  */
typedef enum
{
  /*!
    All nvod schedule information in one table is ready
    */
  NVOD_REF_SVC_FOUND = ((APP_NVOD << 16)|ASYNC_EVT),
  /*!
    NVOD reference evt found
    */
 NVOD_REF_EVT_FOUND,
  /*!
    NVOD shift evt found
    */
  NVOD_SHIFT_EVT_FOUND,
  /*!
    NVOD nc locked
    */
  NVOD_NC_LOCKED,
  /*!
    NVOD nc unlocked
    */
  NVOD_NC_UNLOCKED,
  /*!
    nvod tp scan end
    */
  NVOD_SVC_SCAN_END,
  /*!
    Notify UI that current application NVOD module is ready to stop
    */
  NVOD_STOP = ((APP_NVOD << 16)|SYNC_EVT)
}nvod_evt_t;

/*!
  Delcare all command for scan
  */
typedef enum
{
  /*!
    Start nvod
    */
  CMD_START_NVOD,
  /*!
    nvod start reference service
    */
  CMD_NVOD_START_REF_SVC,
  /*!
    stop reference service
    */
  CMD_NVOD_STOP_REF_SVC,
  /*!
    stop scan
    */
  CMD_STOP_NVOD = SYNC_CMD
}nvod_cmd_t;

/*!
  nvod start paramter
  */
typedef struct tag_nvod_tp_info
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
  /*!
    is nvod main freq
    */
  BOOL main_freq;
}nvod_tp_info_t;

/*!
  Singleton patern interface which can get a scan application's
  instance
  
  \return instance address
  */
app_t *construct_ap_nvod(void);

#endif // End for __AP_NVOD_H_
