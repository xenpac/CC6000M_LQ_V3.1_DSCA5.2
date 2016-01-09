/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_DVBS_OTA_H_
#define __AP_DVBS_OTA_H_

/*!
  \file ap_dvbs_ota.h
  Delecalre dvbs ota application.
  And this is a base class for all dvbs ota applications.
 */

/*!
  Max ota upgrade file size
  */
#define MAX_UPGRADE_FILE_SIZE  0x200000  

/*!
  Used to check if OTAI has first writen into flash
  */
#define OTAI_CHECK_FLAG 0xAAAA 

/*!
  main code crc error
  */
#define S_OTA_MC_ERR 0x01 
/*! 
  downloader crc error
  */
#define S_OTA_DL_ERR 0x02 
/*!
  uset manual force
  */
#define S_OTA_USER_FORCE 0x04 
/*! 
  bat trigger mode
  */
#define S_OTA_BAT_TRIG 0x08
/*!
  flash header crc error
  */
#define S_OTA_FH_ERR 0x10 
/*!
  nvram crc error
  */
#define S_OTA_NVRAM_ERR 0x20 
/*! 
  tdi crc error
  */
#define S_OTA_TDI_ERR 0x40 

/*!
  Max message number supported in ota
  */
#define MAX_OTA_MSG_NUM (32)

/*!
  DVBS OTA State Machine definition
  */
typedef enum
{
  /*!
    OTA app idle state
    */
  OTA_SM_IDLE = 0,
  /*!
    OTA app user input state
    */
  OTA_SM_USER_INPUT,
  /*!
    OTA app is searching tables
    */
  OTA_SM_SEARCHING,  
  /*!
    OTA app is entering 
    */
  OTA_SM_ENTRY,  
  /*!
    OTA app is downloading data
    */
  OTA_SM_DATA_DL,
  /*!
    OTA app is updating and burning data
    */
  OTA_SM_UPDATE,
  /*!
    OTA app exit
    */
  OTA_SM_EXIT
}ota_sm_t;

/*!
  DVBS OTA AP event definition
  */
typedef enum 
{
  /*!
    DVBS OTA app error for no dsmcc tables
    */
  OTA_ERROR_NO_DSMCC = ((APP_OTA << 16)|ASYNC_EVT),
  /*!
    DVBS OTA app error for no matched upgrade info
    */
  OTA_ERROR_NO_MATCH_UPG_INFO,
  /*!
    DVBS OTA app error for wrong section data
    */
  OTA_ERROR_WRONG_SECTION,
  /*!
    DVBS OTA app error for wrong file size
    */
  OTA_ERROR_WRONG_SIZE,
  /*!
    DVBS OTA app error for unlock stream
    */
  OTA_ERROR_STREAM_UNLOCK,
  /*!
    DVBS OTA app error for wrong lzma file crc
    */
  OTA_ERROR_UPG_FILE_CRC,
  /*!
    DVBS OTA app error for wrong flash file crc
    */
  OTA_ERROR_FLASH_FILE_CRC,
  /*!
    DVBS OTA app error for burning failure
    */
  OTA_ERROR_BURNING,
  /*!
    DVBS OTA app event for finding upgrade info
    */
  OTA_EVT_UPG_INFO_FOUND,
  /*!
    DVBS OTA app event for reset
    */
  OTA_EVT_TRIGGER_RESET,
  /*!
    DVBS OTA app event for lock user input's tp
    */
  OTA_EVT_USER_INPUT_LOCKED,
  /*!
    DVBS OTA app event for unlock user input's tp
    */
  OTA_EVT_USER_INPUT_UNLOCKED,
  /*!
    DVBS OTA app event for starting downloading
    */
  OTA_EVT_START_DL,
  /*!
    DVBS OTA app event for starting to check
    */
  OTA_EVT_START_CHECKING,
  /*!
    DVBS OTA app event for starting burning
    */
  OTA_EVT_START_BURNING,
  /*!
    DVBS OTA app event for upgrade completes
    */
  OTA_EVT_UPG_COMPLETE,
  /*!
    DVBS OTA app event for start user input
    */
  OTA_EVT_START_USER_INPUT,
  /*!
    DVBS OTA app event for upgrade progress
    */
  OTA_EVT_PROGRESS,
  /*!
    DVBS OTA app event for reset
    */
  OTA_EVT_SEARCH_STOPPED = ((APP_OTA << 16)|SYNC_EVT)

}ota_evt_t;

/*!
  DVBS OTA AP command definition
  */
typedef enum
{
  /*!
    OTA app start to search upg info:
    for UI to start OTA upg info search
    */
  OTA_CMD_START_SEARCHING = ASYNC_CMD,
  /*!
    OTA app stop:for UI to stop OTA upg info search
    */
  OTA_CMD_STOP_SEARCHING = SYNC_CMD,
  /*!
    OTA start locking tp:
    for UI to start OTA with locking tp, used when user input done.
    */
  OTA_CMD_START_LOCKING = ASYNC_CMD + 1
}ota_cmd_t;

/*!
  DVBS OTA instance policy interface declaration
  */
typedef struct 
{
  /*!
    ota crc init interface
    */
  void (*ota_crc_init)();
  /*!
    ota crc 32 generate interface
    */
  u32 (*ota_crc32_generate)(u8 *p_buf, u32 len);
  /*!
    write ota upgrade info to flash
    */
  void (*write_otai)(ota_info_t *p_otai);
  /*!
    get the terminal device info from flash
    */
  void (*init_tdi)(void *p_data);
  /*!
    get the OUI that is allocated by the ORG
    */
  u32 (*get_oui)(void *p_data);
  /*!
    get the manufacturer id
    */
  u16 (*get_manufact_id)(void *p_data);
  /*!
    get the hardware model id
    */
  u16 (*get_hw_mod_id)(void *p_data);
  /*!
    get the hardware version
    */
  u16 (*get_hw_ver)(void *p_data);
  /*!
    get the software model id
    */
  u32 (*get_sw_mod_id)(void *p_data);
  /*!
    Private data for OTA policy
    */
  void *p_data;
}ota_policy_t;

/*!
  Singleton patern interface which can get an ota application's
  instance

  \return Return app_t instance address
  */
app_t *get_ota_instance(void);

/*!
  Call this method, load ota instance by the policy.

  \param[in] p_policy The policy of application ota
  \return Return app_t instance address
  */
app_t *construct_ap_dvbs_ota(ota_policy_t *p_policy);


#endif // End for __AP_DVBS_OTA_H_
