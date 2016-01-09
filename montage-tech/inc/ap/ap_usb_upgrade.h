/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_USB_UPGRADE_H_
#define __AP_USB_UPGRADE_H_

/*!
  \file ap_usb_upgrade.h

  Declare usb upg application.
  USB UPG application supports block upgrade and all-flash upgrade.
  */
/*!
  max block in usb upg
  */
#define USB_UPG_MAX_BLOCK  60

/*!
 dmh indicator header size
  */
#define USB_UPG_DMH_INDC_SIZE 12
/*!
  user db head size
  */
#define USB_UPG_MAX_HEAD_SIZE  (USB_UPG_DMH_INDC_SIZE + sizeof(usb_upg_user_db_header_t))
/*!
  packet size when downloading data from usb
  */
#define USB_UPG_TRANS_PACKET_SIZE (512 * (KBYTES))
/*!
  packet size when downloading data from usb
  */
#define USB_DUMP_WRITE_PACKET_SIZE (512 * (KBYTES))
/*!
  packet size when burning flash
  */
#define USB_UPG_BURN_PACKET_SIZE (64 * (KBYTES))
/*!
  USB UPG command id list
  */
typedef enum
{
  /*!
  Start burn command
    */
  USB_UPG_CMD_START_BURN = 0,
  /*!
    Stop upgrade
    */
  USB_UPG_CMD_EXIT,
  /*!
    reading flash 
    */
  USB_DUMP_CMD_START_DUMP,
  /*!
    flag MAX cmd, insert new commands before this Macro
    */
  USB_UPG_CMD_CNT,
} usb_upg_cmd_t;

/*!
  event id list
  the event notify ui frame work, something done
  */
typedef enum 
{
  /*!
    the first event of USB¡¡UPG module,
    if create new event, you must insert it between BEGIN and END
    */
  USB_UPG_EVT_BEGIN = (APP_USB_UPG << 16),
  /*!
    UPG ap update current status and progress
    */
  USB_UPG_EVT_UPDATE_STATUS,
  /*!
    UPG ap update current error
    */
  USB_UPG_EVT_UPDATE_ERROR,
  /*!
    UPG ap exit successfully
    */
  USB_UPG_EVT_QUIT_UPG,
  /*!
    upgrade successfully
    */
  USB_UPG_EVT_SUCCESS,
  /*!
    upgrade received power key
    */
  USB_UPG_EVT_POWER,
  /*!
     erase flash error
    */
  USB_UPG_ERASE_FLASH_ERROR,
  /*!
     burn flash error
    */
  USB_UPG_BURN_FLASH_ERROR,
  /*!
    OPEN USB UPG FILE FAILED
    */
  USB_OEPN_UPG_FILE_FAILED,
  /*!
    malloc memory failed
    */
  USB_MALLOC_MEMORY_FAILED,
  /*!
    download file fail
    */
  USB_DOWNLOAD_FILE_FAIL,
  /*!
    invalid upg file
    */
  USB_INVLID_UPGRADE_FILE,
  /*!
    upg file is older than the current one
    */
  USB_OLD_UPGRADE_FILE,
  /*!
    read flash error
      */
  USB_DUMP_READ_FLASH_ERROR,
  /*!
    burn flash error
      */
  USB_DUMP_READ_FLASH_SUCCESS,
  /*!
    burn flash error
      */
  USB_DUMP_CREATE_FILE_ERROR,
  /*!
    burn flash error
      */
  USB_DUMP_CREATE_FILE_SUCCESS,
  /*!
    write file error
      */
  USB_DUMP_WRITE_FILE_ERROR,
  /*!
    write file success
      */
  USB_DUMP_WRITE_FILE_SUCCESS, 
  /*!
    the last event of upg module,
    if create new event, you must insert it between BEGIN and END
    */
  USB_UPG_EVT_END
} usb_upg_evt_t;

/*!
  upgrade mode
  */
typedef enum 
{
  /*!
    upgrade all flash 
    */
  USB_UPGRD_ALL = 0,
  /*!
    upgrade user db
    */
  USB_UPGRD_USER_DB,  
  /*!
    upgrade maincode
    */
  USB_UPGRD_MAINCODE,
    /*!
     upgrade tps and programs
    */
  USB_UPGRD_TP_PROG,
    /*!
    upgrade back logo
    */
  USB_UPGRD_BACK_LOGO,
    /*!
    upgrade cccam
    */
  USB_UPGRD_CCCAM,
    /*!
    upgrade constant cw
    */
  USB_UPGRD_CONSTANT_CW,
    /*!
    upgrade ignore list
    */
  USB_UPGRD_IGNORE_LIST,
    /*!
    upgrade replace list
    */
  USB_UPGRD_REPLACE_LIST,
    /*!
    upgrade item new camd list
    */
  USB_UPGRD_NEW_CAMD_LIST,
    /*!
    upgrade item max
    */
  USB_UPGRD_ITEM_MAX,

}usb_upg_mode_t;


/*!
  UPG status machine
  */
typedef enum
{
  /*!
    idle status, ready to upg
    */
  USB_UPG_SM_IDLE = 0,
  /*!
    compare software version
    */
  USB_UPG_SM_CHECK_VALIDNESS,
  /*!
    client flash burning status
    */
  USB_UPG_SM_BURNING,
  /*!
    client reboot status
    */
  USB_UPG_SM_REBOOTING,
  /*!
    read flash
    */
  USB_DUMP_SM_READING_FLASH,
  /*!
    create usb file
    */
  USB_DUMP_SM_CREATING_FILE,
  /*!
      write usb file
      */
  USB_DUMP_SM_WRITING_FILE,
  /*!
    pre-exit upg
    */
  USB_UPG_SM_EXIT,    
  /*!
    the end of State machine, below is for error status
    */
  USB_UPG_SM_END,
  /*!
    client error
    */
  USB_SM_REMOTE_ERROR
} usb_upg_sm_t;

/*!
  USB UPG key define and index to find key code
  */
typedef enum 
{
  /*!
    default, ignore the key value
    */
  USB_UPGRD_KEY_IGNORE = 0,
  /*!
    receive exit key
    */
  USB_UPGRD_KEY_EXIT,
  /*!
    receive power key
    */
  USB_UPGRD_KEY_POWER,
  /*!
    receive menu key
    */
  USB_UPGRD_KEY_MENU,
}usb_upg_key_t;

/*!
  UPG key define and index to find key code
  */
typedef enum 
{
  /*!
    default, ignore the key value
    */
  USB_UPG_RET_SUCCESS = 0,
  /*!
    usb get upg file info error
    */
  USB_OPEN_UPGRADE_FILE_ERROR,
  /*!
    malloc memory failed
    */
   USB_MALLOC_MEMORY_FAILED_ERROR,
  /*!
    read upgfile data error
  */
  USB_READ_UPGFILE_DATA_RET_ERROR,
  /*!
    invalid upg file
  */
  USB_INVALID_UPG_FILE_ERROR,
  /*!
    usb upg error
    */
  USB_UPG_RET_ERROR,
  /*!
    receive exit key
    */
  USB_UPG_RET_EXIT,
  /*!
    receive menu key
    */
  USB_UPG_RET_MENU,
  /*!
    receive power key
    */
  USB_UPG_RET_POWER,
  /*!
    param error
    */
  USB_UPG_RET_PARAM,  
}usb_upg_ret_t;

/*!
  upg key code
  */
typedef enum
{
  /*!
    index to get exit key (hardware key code)
    */
  USB_UPGRD_CODE_EXIT = 0,
  /*!
    index to get power key (hardware key code)
    */
  USB_UPGRD_CODE_POWER,
  /*!
    index to get menu key (hardware key code)
    */
  USB_UPGRD_CODE_MENU,
}usb_upg_code_t;

/*!
  user db header info, used to dump user db
  */
typedef struct
{
  /*!
    user db version 
    */
  u8 version;
  /*!
    customer_id 
    */
  u8 customer_id;
  /*!
    reserve
    */
  u8 reserve[2];
}usb_upg_user_db_header_t;

/*!
  UPG status update to UI
  */
typedef struct 
{
  /*!
    current status
    */
  usb_upg_sm_t sm;
  /*!
    progress in current status
    */
  u32 progress;
  /*!
    description of current status
    */
  u8 *p_description;
}usb_upg_status_t;

/*!
  client flash info, collected from client
  */
typedef struct 
{
  /*!
    data manager header start address
    */
  u32 dmh_start_addr;
  /*!
    the size of bootloader section
    */
  u32 bootloader_size;
  /*!
    client sdram size, below is the same with data manager header
    */
  u32 sdram_size;
  /*!
    client flash size
    */
  u32 flash_size;
  /*!
    client block number
    */
  u16 block_num;
  /*!
    size of each block header
    */
  u16 bh_size;
  /*!
    blocks in client
    */
  dmh_block_info_t blocks[USB_UPG_MAX_BLOCK];
}stb_info_t;

/*!
  upg zip type
  */
typedef enum
{
  /*!
    none zip
    */
  USB_UPG_ZIP_NONE = 0,
  /*!
    gzip
    */
  USB_UPG_ZIP_GZIP,
  /*!
    lzma
    */
  USB_UPG_ZIP_LZMA,
}usb_upg_zip_t;

/*!
  UPG configuration
  */
typedef struct
{
  /*!
    total flash size
    */
  u32 flash_size;

  /*!
    unzip buffer
    */
  u8 *p_unzip_buffer;
  /*!
    inflate buffer
    */
  u8 *p_inflate_buffer;
  /*!
    zip buffer
    */
  u8 *p_zip_buffer;
  /*!
    unzip buffer size
    */
  u32 unzip_size;
  /*!
    inflate buffer size
    */
  u32 inflate_size;
  /*!
    zip size
    */
  u32 zip_size;
  /*!
    tmp use to store the data manager information from slave
    */
  stb_info_t *p_stb_info;
}usb_upg_config_t;


/*!
  UPG start params
  */
typedef struct
{
  /*!
    buf addr
    */
  u32 ext_buf_addr;
  /*!
    buf size
   */
  u32 ext_buf_size;
  /*!
    file path
   */
  u16 file[MAX_FILE_PATH];
  /*!
    flash offset addr
   */
  u32 flash_offset_addr;
  /*!
  flash burn size
  */
  u32 burn_flash_size;
  /*!
   mode
  */
  u8 mode;
  /*!
    user db head size
    */
  u32 head_size;
  /*!
    user db head data
    */
  u8 head_data[USB_UPG_MAX_HEAD_SIZE];  
}usb_upg_start_params_t;

/*!
  UPG block info, used to negotiate
  */
typedef struct
{
  /*!
    block id in usb 
    */
  u8 usb_block_id;   
  /*!
    block id in stb
    */
  u8 stb_block_id;
  /*!
    block size, only available for upgrade all
    */
  u32 size;
}usb_upg_block_t;

/*!
  UIO implementation policy
  */
  typedef struct
{
  /*!
    Initialize the upg ap, and add config info
    \param[out] p_cfg the configuration to fill
    */
  usb_upg_ret_t (*p_init)(usb_upg_config_t *p_cfg);
  /*!
    process block info from client
    \param[in] block_num pointer to block number
    \param[in] block pointer to the blocks to upgrade
    \param[in] block pointer to slave info

    \return SUCCESS if OK, else fail
    */
  usb_upg_ret_t (*p_block_process)(u8 block_num, usb_upg_block_t *p_block, stb_info_t *p_info);
  /*!
    process before start UPG
    */
  void (*p_pre_start)(usb_upg_config_t *p_cfg);
  /*!
    process after exit UPG
    */
  void (*p_post_exit)(usb_upg_config_t *p_cfg);
  /*!
    get UIO hardware code
    \param[in] index key index
    \param[out] code hardware key code

    \return the hardware code number
    */
  u16 (*p_get_code)(u32 index, u16 *p_code);
}usb_upg_policy_t;

/*!
  Get UPG instance and return upg application instance
  \param[in] p_usb_upg_policy UPG application implementation policy
  */
app_t * construct_ap_usb_upg(usb_upg_policy_t *p_usb_upg_policy);

#endif // End for __AP_USB_UPGRADE_H_

