/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SS_CTRL_H_
#define __SS_CTRL_H_

/*!
  \file: ss_ctrl.h
  This module provides the data structure about data written into system 
  status and the interface about how to operate sys_status data
  */
/*!
  Block id storing sysstatus data
  */
#define SS_VAULE_BLOCK_ID   (IW_VIEW_BLOCK_ID)

/*!
  Block id storing sysstatus default data
  */
#define SS_DEFAULT_BLOCK_ID   (SS_DATA_BLOCK_ID)

/*!
  ota trigger mode
  */
typedef enum
{
  /*!
    no ota.
    */
  OTA_TRI_NONE = 0,
  /*!
    ota auto
    */
  OTA_TRI_AUTO,
  /*!
    ota force.
    */
  OTA_TRI_FORC,
  /*!
    ota monitor(monitor in main code)
    */
  OTA_TRI_MONITOR,
  /*!
    ota monitor in oat loader
    */
  OTA_TRI_MON_IN_LOADER,
}ota_trigger_t;

/*!
  OTA information in dvbs
  */
typedef struct
{
  /*!
    OTA download flag in BAT mode 
    */
  ota_trigger_t ota_tri;
  /*!
    sys mode
    */
  sys_signal_t sys_mode;
  /*!
    The data pid of download stream
    */
  u16 download_data_pid;
  /*!
    New software version
    */
  u16 new_software_version; 
  /*!
    Original software version
    */
  u16 orig_software_version; 
  /*!
    dvbs lock info.
    */
  dvbs_lock_info_t locks;
  /*!
    dvbc lock info.
    */
  dvbc_lock_info_t lockc;
  /*!
    dvbt lock info
    */
  dvbt_lock_info_t lockt;
}ota_info_t;

/*!
  sys status public data
  */
typedef struct tag_ss_public
{
  /*!
    Standby flag
    */
  u32 standby_flag;  
  /*!
    ota information in dvbs
    */
  ota_info_t otai;
}ss_public_t;

/*!
  mdi class define
  */
typedef struct tag_mdi_class
{
  /*!
    private data
    */
  void *p_private_data;
  /*!
    read
    */
  u32 (*read)(handle_t _this, u32 r_len, u8 *p_buffer);
  /*!
    write
    */
  u32 (*write)(handle_t _this, u32 w_len, u8 *p_buffer);
  /*!
    destroy
    */
  void (*destroy)(handle_t _this);
}mdi_class_t;


/*!
  The first node index, the module use the same block with view add follow it
  !!!fix me
  */
//#define SS_CTRL_FIRST_NI (0 + VIEW_NI_NUM)  

/*!
  SS_CTRL module support 4*100 bytes now, it can expand
  */
#define SS_CTRL_NI_NUM (500)

/*!
  Init system status public data .
  \param[in] p_data sys status private data
  \param[in] block_id
  */
void ss_ctrl_public_init(void *p_data, u8 block_id);
/*!
  Init system status module and return TRUE if succeed, or not FALSE
  \param[in] block_id
  \param[in] p_buffer
  \param[in] buf_len
  */
BOOL ss_ctrl_init(u8 block_id, u8 *p_buffer, u32 buf_len);

/*!
  sync system status data to flash and return param sync end or not
  \param[in] p_data sys status private data
  \param[in] p_buffer
  */
BOOL ss_ctrl_sync(void *p_data, u8  *p_buffer);

/*!
  Get sysstatus checksum flag. if flag is error, it means some data 
  is iffy possibly and Return TRUE if the checksum is right. or not FALSE.
  \param[in] p_data sys status private data
  */
BOOL ss_ctrl_get_checksum(void *p_data);

/*!
  Set right checksum to flash and return TRUE if succeed. or not FALSE
  \param[in] p_data sys status private data
  */
BOOL ss_ctrl_set_checksum(void *p_data);

/*!
  Clean the checksum from flash and return TRUE if succeed. or not FALSE.
  \param[in] p_data sys status private data
  */
BOOL ss_ctrl_clr_checksum(void *p_data);

/*!
  Get the public struct and return the addr of public data
  \param[in] p_data sys status private data
  */
ss_public_t *ss_ctrl_get_public(void *p_data);

/*!
  Save the public data to flash and Return TRUE if succeed or not FALSE
  \param[in] p_data sys status private data
  */
BOOL ss_ctrl_update_public(void *p_data);

/*!
  Save the public data to flash and Return TRUE if succeed or not FALSE
  \param[in] block_id block id
  \param[in] user_id context
  \param[in] space_size size
  */
mdi_class_t * mdi_create_instance(u8 block_id, u32 user_id, u32 space_size);

#endif // End fo __SS_CTRL_H_
