/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_TS_OTA_H_
#define __AP_TS_OTA_H_

/*!
  \file ap_ts_ota.h
  Delecalre ts ota application.
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
  Max message number supported in ts ota
  */
#define MAX_TS_OTA_MSG_NUM 32

/*!
  Max message number supported in ts ota
  */
#define TS_OTA_INVALID_SID 0xFFFF

/*!
  TS OTA State Machine definition
  */
typedef enum
{
  /*!
    TS OTA IDLE,
    */
  TS_OTA_SM_IDLE = 0,
  /*!
    TS_OTA_LOCKING
    */
  TS_OTA_SM_LOCKING,
  /*!
    TS OTA SEARCHING
    */
  TS_OTA_SM_SEARCHING,  
  /*!
    TS OTA DOWNLOAD
    */
  TS_OTA_SM_DOWLOAD,  

}ts_ota_sm_t;

/*!
  DVBS OTA AP event definition
  */
typedef enum 
{
  /*!
    TS OTA PAT TIME OUT
    */
  TS_OTA_PAT_TIMEOUT = ((APP_TS_OTA << 16)|ASYNC_EVT),
  /*!
    TS OTA SDT TIME OUT
    */
  TS_OTA_SDT_TIMEOUT,
  /*!
    TS OTA PMT TIME OUT
    */  
  TS_OTA_PMT_TIMEOUT,
  /*!
    TS OTA FOUND
    */
  TS_OTA_EVT_FOUND,
  /*!
    TS OTA RESET STB
    */
  TS_OTA_EVT_RESET,
  /*!
    TS OTA UNLOCKED
    */
  TS_OTA_EVT_UNLOCKED,  
  /*!
    TS OTA LOCKED
    */
  TS_OTA_EVT_LOCKED,  
  /*!
    TS OTA PROGRESS
    */
  TS_OTA_EVT_PROGRESS,  
  /*!
    TS NO MATCH SVC NAME
    */
  TS_OTA_SVC_NOT_MATCH,
  /*!
    TS ota evt show  
    */
  TS_OTA_EVT_STATUS,  
  /*!
    TS OTA app event for reset
    */
  TS_OTA_EVT_STOPPED = ((APP_TS_OTA << 16)|SYNC_EVT)

}ts_ota_evt_t;

/*!
  TS OTA AP command definition
  */
typedef enum
{
  /*!
    TS OTA app start to search upg info:
    for UI to start OTA upg info search
    */
  TS_OTA_CMD_START = ASYNC_CMD,
  /*!
    TS OTA app start to search upg info:
    for UI to start OTA upg info search
    */  
  TS_OTA_CMD_RELOCK,   
  /*!
    TS OTA app start to search upg info:
    for UI to start OTA upg info search
    */
  TS_OTA_CMD_START_SEARCH,
  /*!
    TS OTA app start to search upg info:
    for UI to start OTA upg info search
    */  
  TS_OTA_CMD_STOP_SEARCH,   
  /*!
    OTA app stop:for UI to stop OTA upg info search
    */
  TS_OTA_CMD_STOP = SYNC_CMD,
}ts_ota_cmd_t;

/*!
  TS OTA bootloader part download information
  */
typedef struct
{
  /*!
    nim channel info.
    */
  nim_channel_info_t channel_info;
  /*!
    nim lnb porlar.
    */
  nim_lnb_porlar_t polar;
  /*!
    pid
    */
  u16 pid;  
}ts_download_info_t;

/*!
  TS OTA instance policy interface declaration
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
  u32 (*ota_crc32_generate)(u8 *p_buf, u32 length);  
  /*!
    write ota upgrade info to flash
    */
  void (*write_otai)(ota_info_t *p_otai);  
  /*!
    read ota upgrade info from flash
    */  
  void (*read_otai)(void *p_otai);  
  /*!
    burn upgrade file to flash
    */
  BOOL (*burn_file)(u8 *p_buf, u32 size, void *notify_progress);  
  /*!
    Private data for OTA bootloader part policy
    */
  void *p_data;  
}ts_ota_policy_t;

/*!
  Call this method, load ota instance by the policy.

  \param[in] p_policy The policy of application ota
  \return Return app_t instance address
  */
app_t *construct_ap_ts_ota(ts_ota_policy_t *p_policy);


#endif // End for __AP_TS_OTA_H_
