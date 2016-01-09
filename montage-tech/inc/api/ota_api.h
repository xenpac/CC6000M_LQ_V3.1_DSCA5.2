/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_API_H_
#define __OTA_API_H_

/*!
  defines ota filter evt call back. 
  */
typedef void (*ota_evt_call_back)(u32 content, u32 para1, u32 para2,u32 context);

/*!
  defines ota filter event. 
  */
typedef enum tag_ota_api_evt
{
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_UNKOWN = 0,
/*!
  filter event:stop result
  */
  OTA_API_EVENT_STOP_RESULT,
/*!
  defines ota filter event.:UNLOCK
  */
  OTA_API_EVENT_LOCK,
/*!
  defines ota filter event.:UNLOCK
  */
  OTA_API_EVENT_UNLOCK,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_CHECK_FAIL,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_CHECK_SUCCESS,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_DOWNLOAD_FAIL,
 /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_DOWNLOAD_START,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_DOWNLOAD_ING,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_DOWNLOAD_TIMEOUT,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_DOWNLOAD_SUCCESS,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_NO_BLOCK_UPG,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_START,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_BLOCK,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_ING,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SAVE_OTA_STATUS,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SAVE_OTA_DSTROY_FLAGE,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SAVE_CURR_OTA_ID,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SAVE_BLOCK_VERSION_INFO,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_RESET_SYS_DATA_BLOCK,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_RESET_DB_DATA_BLOCK,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_RESET_CA_DATA_BLOCK,
  /*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_RESET_ADS_DATA_BLOCK,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SAVE_UPG_VERSION,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_FACTORY,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_FAIL,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_BURNFLASH_SUCCESS,
/*!
  defines ota filter event. 
  */
  OTA_API_EVENT_FINISH_REBOOT, 
/*!
  defines ota filter event. 
  */
  OTA_API_REQUEST_TIMEOUT,  
}ota_api_evt_t;

/*!
  OTA information in dvb
  */
typedef struct
{
 /*!
  task stack size.
  */ 
  u32 task_stk_size;
  /*!
   ota evt call back
   */
  ota_evt_call_back evt_process;  
  /*!
    ota medium select
    */
  ota_medium_t medium;
  /*!
    stream protocol type
    */
  ota_stream_protocol_t protocol_type;
}ota_para_t;

/*!
  OTA run policy case
  */
typedef struct
{
  /*!
    ota work select
    */
  ota_work_t  run_select;
  /*!
    ota block,select bock download data or burn flash param;
    */ 
  ota_block_info_t ota_block[OTA_MAX_BLOCK_NUMBER];
 /*!
    ota check info
    */ 
  upg_check_info_t upg_check_info;
  /*!
    ota tp info
    */
  ota_tp_info_t tp_info;
 /*!
   ota monitor param
   */
   ota_monitor_para_t monitor_para;
}ota_api_policy_t;


/*!
  ota api init
  */
RET_CODE mul_ota_api_init(ota_para_t *para);
/*!
  ota api config
  */
RET_CODE mul_ota_api_config(ota_config_t *p_cfg);
/*!
  ota api start
  */
void mul_ota_api_start(void);
/*!
  ota api stop
  */
void mul_ota_api_stop(void);
/*!
  ota run policy and case
  */
void mul_ota_api_select_policy(ota_api_policy_t *policy);
/*!
  api process nim lock status result
  */
void mul_ota_api_process_nim_lock_status(BOOL is_locked);
#endif //end for __OTA_API_H_
