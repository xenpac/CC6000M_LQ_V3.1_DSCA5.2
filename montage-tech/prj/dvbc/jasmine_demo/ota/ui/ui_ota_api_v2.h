/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _UI_OTA_API_V2_H
#define _UI_OTA_API_V2_H

typedef enum ota_msg_em
{
  MSG_OTA_FOUND = MSG_EXTERN_BEGIN + 200, //0x60c8
  MSG_OTA_TRIGGER_RESET,
  MSG_OTA_TMOUT,  
  MSG_TS_OTA_STATUS,
  MSG_OTA_PROGRESS ,
  MSG_OTA_LOCK,
  MSG_OTA_UNLOCK,
  MSG_OTA_START_DL,
  MSG_OTA_CHECKING,
  MSG_OTA_BURNING,
  MSG_OTA_FINISH,
  MSG_OTA_UPGRADE,
  MSG_OTA_UNZIP_OK,
  MSG_OTA_TABLE_TIMEOUT,
  MSG_OTA_LOADER_MON_END, //OTA loader monitor check end
} ota_msg;

/*!
  OTA AP event definition
  */
typedef enum 
{
  /*!
    DVBS OTA app event UNKOWN
    */
  OTA_EVT_UNKOWN = ((APP_OTA << 16)|ASYNC_EVT),
  /*!
    DVBS OTA app event for finding upgrade info
    */
  OTA_EVT_UPG_INFO_FOUND ,
  /*!
    DVBS OTA app event for reset
    */
  OTA_EVT_TRIGGER_RESET,
  /*!
    DVBS OTA app event for starting downloading
    */
  OTA_EVT_START_DL,
  /*!
    DVBS OTA app event for upgrade completes
    */
  OTA_EVT_UPG_COMPLETE,
  /*!
    DVBS OTA app event for upgrade progress
    */
  OTA_EVT_PROGRESS,
  /*!
    locked.
    */
  OTA_EVT_LOCKED,
  /*!
    unlocked.
    */
  OTA_EVT_UNLOCKED,
  /*!
    wrong size
    */
  OTA_ERROR_WRONG_SIZE,    
  /*!
    CRC failed.
    */
  OTA_EVT_CRC_OK,
  /*!
    CRC failed.
    */
  OTA_EVT_CRC_FAILED,
  /*!
    Burn failed.
    */
  OTA_EVT_BURN_FAILED,
  /*!
    Unzip OK.
    */
  OTA_EVT_UNZIP_OK,
  /*!
    Unzip failed.
    */
  OTA_EVT_UNZIP_FAILED,
  /*!
    burn start.
    */
  OTA_EVT_BURN_START,   
  /*!
    burnning.
    */
  OTA_EVT_BURNING,    
  /*!
    Request table time out
    */
  OTA_EVT_TABLE_TIMEOUT,  
  /*!
    OTA loader monitor check end
    */
  OTA_EVT_LOADER_MON_END,  
  /*!
    DVBS OTA app event for reset
    */
  OTA_EVT_SEARCH_STOPPED ,
  /*!
    DVBS OTA app event for reset
    */
  OTA_EVT_CHECK_SUCESS ,
  /*!
    DVBS OTA app event for reset
    */
  OTA_EVT_CHECK_FAIL,

}ota_evt_t;


void ui_ota_dm_api_init(void);
u32 ui_ota_api_get_upg_check_version(void);
/****main code block id = 0xff****/
u16 ui_ota_api_get_block_version(u8 block_id);
void ui_ota_dm_api_test(void);
void ui_ota_api_bootload_info_init(void);
void ui_ota_api_ota_param_init(void);
void ui_ota_api_set_ota_tp(void);

void ui_ota_api_manual_save_ota_info(void);
void ui_ota_api_init(void);
void ui_ota_api_stop(void);
void ui_ota_api_auto_check_cancel(void);


#endif
