/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_OTA_H_
#define __AP_OTA_H_

/*!
  \file ap_dvbs_ota.h
  Delecalre dvbs ota application.
  And this is a base class for all dvbs ota applications.
 */

/*!
  Max ota upgrade file size
  */
#define MAX_OTA_FILE_SIZE  0x200000  

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
    OTA idle state.
    */
  OTA_SM_IDLE = 0,
  /*!
    OTA try to locking.
    */
  OTA_SM_LOCKING,
  /*!
    OTA wait lock.
    */
  OTA_SM_WAIT_LOCK,
  /*!
    OTA user input.
    */
  OTA_SM_USER_INPUT,
  /*!
    OTA searching.
    */
  OTA_SM_SEARCHING,
  /*!
    OTA wait DSI.
    */
  OTA_SM_WAIT_DSI,  
  /*!
    OTA wait DII.
    */
  OTA_SM_WAIT_DII,
  /*!
    OTA wait DDM.
    */
  OTA_SM_WAIT_DDM,  
  /*!
    OTA app is downloading data
    */
  OTA_SM_DOWNLOADING,
  /*!
    OTA app is updating
    */
  OTA_SM_UPDATE,
  /*!
    OTA burn check.
    */
  OTA_SM_BURN_CHECK,
  /*!
    OTA burnning.
    */
  OTA_SM_BURNNING,
  /*!
    OTA burn finished.
    */
  OTA_SM_BURN_FINISHED,    

}ota_sm_t;

/*!
  OTA AP event definition
  */
typedef enum 
{
  /*!
    DVBS OTA app event for finding upgrade info
    */
  OTA_EVT_UPG_INFO_FOUND = ((APP_OTA << 16)|ASYNC_EVT),
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
    OTA start locking tp:
    for UI to start OTA with locking tp, used when user input done.
    */
  OTA_CMD_START_LOCKING,
  /*!
    OTA app start.
    */
  OTA_CMD_START,
  /*!
    Exit OTA:
    for UI to exit OTA, and reboot to maincode.
    */  
  OTA_CMD_STOP,
  /*!
    OTA relock.
    */
  OTA_CMD_RELOCK,  
   /*!
    OTA check main
    */
  OTA_CMD_CHECK_MAIN,    
  /*!
    OTA start burn,
    */
  OTA_CMD_BURN_START,  
  /*!
    OTA app stop:for UI to stop OTA upg info search
    */
  OTA_CMD_STOP_SEARCHING = SYNC_CMD,

}ota_cmd_t;

/*!
  OTA TDI info definition
*/
typedef struct
{
  /*!
    oui
    */
  u32 oui;
  /*!
    manufacture id
    */
  u16 manufacture_id;
  /*!
    hardware module id
    */
  u16 hw_mod_id;
  /*!
    software module id
    */
  u16 sw_mod_id;
  /*!
    hardware version
    */
  u16 hw_version;
  /*!
    reserved
    */
  u32 reserved;
}ota_tdi_t;

/*!
  ota config
  */
typedef struct
{
  /*!
    buffer for save ota data.
    */
  u8 *p_save_buf;
  /*!
    save buffer size.
    */
  u32 save_size;
  /*!
    buffer for unzipped data.
    */
  u8 *p_burn_buf;
  /*!
    unzipped buffer size.
    */
  u32 burn_size;
  /*!
    buffer for LZMA unzip
    */
  u8 *p_unzip_buf;
  /*!
    unzip buffer size.
    */
  u32 unzip_size;
  /*!
    buffer for LZMA unzip
    */
  u8 *p_fake_buf;
  /*!
    unzip buffer size.
    */
  u32 fake_size;
  /*!
    flash size
    */
  u32 flash_size;
  /*!
    section buffer.
    */
  dvb_section_t *p_sec_buf;
  /*!
    extern buffer for pti/dmx.
    */
  filter_ext_buf_t *p_dsmcc_extern_buf;  
  /*!
    extern buffer for pti/dmx dtm.
    */
  filter_ext_buf_t *p_dtm_extern_buf;
}ota_cfg_t;

/*!
  DVBS OTA instance policy interface declaration
  */
typedef struct 
{
  /*!
    ota crc init interface
    */
  void (*ota_crc_init)(void);
  /*!
    ota crc 32 generate interface
    */
  u32 (*ota_crc32_generate)(u8 *p_buf, u32 len);
  /*!
    write ota upgrade info to flash
    */
  void (*ota_write_otai)(ota_info_t *p_otai);
  /*!
    write ota upgrade info to flash
    */
  void (*ota_read_otai)(ota_info_t *p_otai);
  /*!
    get the terminal device info from flash
    */
  void (*ota_init_tdi)(void *p_data);
  /*!
    ota config project.
    */
  void (*ota_cfg)(ota_cfg_t *p_ota_cfg, BOOL is_upgrade_all, BOOL is_zipped);
  /*!
    burn file.
    */
  BOOL (*ota_burn_file)(u8 *p_buf, u32 size, void *p_notify_progress); 
  /*!
    burn file.
    */
  BOOL (*ota_burn_file_group)(u8 *p_buf, u32 start, u32 size, void *p_notify_progress); 
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
app_t *construct_ap_ota(ota_policy_t *p_policy);

/*!
  Call this method, load ota instance by the policy.

  \param[in] p_policy The policy of application ota
  \return Return app_t instance address
  */
app_t *construct_ap_group_ota(ota_policy_t *p_policy);
#endif // End for __AP_DVBS_OTA_H_
