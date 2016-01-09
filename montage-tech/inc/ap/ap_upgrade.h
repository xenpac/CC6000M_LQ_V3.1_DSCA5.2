/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_UPGRADE_H_
#define __AP_UPGRADE_H_

/*!
  \file ap_upgrade.h

  Declare upg application.
  UPG application supports block upgrade and all flash upgrade.
  */

/*!
  UPG all macro
  */
#define P2P_UPGRADE_ALL               0xFF
/*!
  the max block count to support, MUST BE THE SAME with upgclient
  */
#define P2P_MAX_BLOCKS                64
/*!
  This must be the same with BOOTLOADER_BLOCK_ID in sys_cfg.h
  */
#define UPG_BOOTLOADER_BLOCK_ID       BOOTLOADER_BLOCK_ID

/*!
  debug print to usb file
  */
//#define _P2P_DEBUG 1

/*!
  UPG command id list
  */
typedef enum
{
  /*!
    Start upgrade
    */
  UPG_CMD_START = 0,
  /*!
    Stop upgrade
    */
  UPG_CMD_EXIT,
  /*!
    Restart upg(return to sync states)
    */
  UPG_CMD_RESTART,
  /*!
    flag MAX cmd, insert new commands before this Macro
    */
  UPG_CMD_CNT
} upg_cmd_t;

/*!
  event id list
  the event notify ui frame work, something done
  */
typedef enum
{
  /*!
    the first event of UPG module,
    if create new event, you must insert it between BEGIN and END
    */
  UPG_EVT_BEGIN = (APP_UPG << 16),
  /*!
    UPG ap update current status and progress
    */
  UPG_EVT_UPDATE_STATUS,
  /*!
    UPG ap update current error
    */
  UPG_EVT_UPDATE_ERROR,
  /*!
    UPG ap exit successfully
    */
  UPG_EVT_QUIT_UPG,
  /*!
    upgrade successfully
    */
  UPG_EVT_SUCCESS,
  /*!
    upgrade received power key
    */
  UPG_EVT_POWER,
  /*!
    the last event of upg module,
    if create new event, you must insert it between BEGIN and END
    */
  UPG_EVT_END
} upg_evt_t;

/*!
  upgrade mode
  */
typedef enum 
{
  /*!
    upgrade all flash once
    */
  UPGRD_ALL_SINGLE = 0,
  /*!
    upgrade all flash circularly
    */
  UPGRD_ALL_MULTI, 
  /*!
    upgrade flash blocks once
    */
  UPGRD_BLOCKS_SINGLE,
  /*!
    upgrade flash blocks circularly
    */
  UPGRD_BLOCKS_MULTI
}upg_mode_t;


/*!
  UPG status machine
  */
typedef enum
{
  /*!
    idle status, ready to upg
    */
  UPG_SM_IDLE = 0,
  /*!
    sync status, 
    */
  UPG_SM_SYNCING,
  /*!
    nogotialtion status, 
    */
  UPG_SM_NEGOTIATING,
  /*!
    data transfer status
    */
  UPG_SM_TRANSFERING,
  /*!
    client flash burning status
    */
  UPG_SM_BURNING,
  /*!
    client reboot status
    */
  UPG_SM_REBOOTING,
  /*!
    pre-exit upg
    */
  UPG_SM_EXIT,    
  /*!
    the end of State machine, below is for error status
    */
  UPG_SM_END,
  /*!
    Master error
    */
  UPG_SM_MASETER_ERROR,
  /*!
    client error
    */
  UPG_SM_REMOTE_ERROR
} upg_sm_t;

/*!
  UPG error code
  */
typedef enum
{
  /*!
    read flash error
    */
  ERR_SYNC_READ_FLASH_ERR = 0x0,
  /*!
    unzip upgclient error
    */
  ERR_SYNC_UNZIP_ERR,
  /*!
    fail to get response from client
    */
  ERR_TEST_FAIL = 0x10,
  /*!
    fail to collect info from client
    */
  ERR_COLLECT_FAIL = 0x20,
  /*!
    crc error of client
    */
  ERR_COLLECT_CRC_ERROR,
  /*!
    fail to inform client
    */
  ERR_INFORM_FAIL = 0x30,
  /*!
    read flash error
    */
  ERR_INFORM_FLASH_READ_ERR,
  /*!
    there is no block id sent by mater in client
    */
  ERR_INFORM_ID_NOT_EXIST,
  /*!
    size error
    */
  ERR_INFORM_BL_SIZE_ERR,
  /*!
    fail to transport data
    */
  ERR_TRANS_FAIL = 0x40,
  /*!
    failed to send transport command
    */
  ERR_TRANS_SEND_CMD_ERR,
  /*!
    read flash error
    */
  ERR_TRANS_FLASH_READ_ERR,
  /*!
    fail to get ACK from client in transporting
    */
  ERR_TRANS_UART_TIMEOUT,
  /*!
    packet sequence error
    */
  ERR_TRANS_DISORDER,
  /*!
    unknown error in transporting
    */
  ERR_TRANS_UNKNOWN,
  /*!
    fail to burn flash in client
    */
  ERR_BURN_FAIL = 0x50,
  /*!
    fail to send burn command
    */
  ERR_BURN_SEND_CMD_FAIL
}upg_err_code_t;


/*!
  UPG key define and index to find key code
  */
typedef enum 
{
  /*!
    default, ignore the key value
    */
  UPGRD_KEY_IGNORE = 0,
  /*!
    receive exit key
    */
  UPGRD_KEY_EXIT,
  /*!
    receive power key
    */
  UPGRD_KEY_POWER,
  /*!
    receive menu key
    */
  UPGRD_KEY_MENU,
}upg_key_t;

/*!
  UPG key define and index to find key code
  */
typedef enum 
{
  /*!
    default, ignore the key value
    */
  UPG_RET_SUCCESS = 0,
  /*!
    receive exit key
    */
  UPG_RET_ERROR,
  /*!
    receive exit key
    */
  UPG_RET_EXIT,
  /*!
    receive menu key
    */
  UPG_RET_MENU,
  /*!
    receive power key
    */
  UPG_RET_POWER,
  /*!
    param error
    */
  UPG_RET_PARAM,  
}upg_ret_t;

/*!
  upg key code
  */
typedef enum
{
  /*!
    index to get exit key (hardware key code)
    */
  UPGRD_CODE_EXIT = 0,
  /*!
    index to get power key (hardware key code)
    */
  UPGRD_CODE_POWER,
  /*!
    index to get menu key (hardware key code)
    */
  UPGRD_CODE_MENU,
}upg_code_t;

/*!
  UPG status update to UI
  */
typedef struct 
{
  /*!
    current status
    */
  upg_sm_t sm;
  /*!
    progress in current status
    */
  u32 progress;
  /*!
    description of current status
    */
  u8 *p_description;
}upg_status_t;

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
  dmh_block_info_t blocks[P2P_MAX_BLOCKS];
}slave_info_t;

/*!
  upg zip type
  */
typedef enum
{
  /*!
    none zip
    */
  UPG_ZIP_NONE = 0,
  /*!
    gzip
    */
  UPG_ZIP_GZIP,
  /*!
    lzma
    */
  UPG_ZIP_LZMA,
}upg_zip_t;

/*!
  upg chip type
  */
typedef enum
{
  /*!
    unknown
    */
  UPG_CHIP_UNKNOWN = 0,
  /*!
    magic
    */
  UPG_CHIP_MAGIC,
  /*!
    warriors
    */
  UPG_CHIP_WARRIORS,
}upg_chip_t;

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
    the start address of upg client in flash
    */
  u32 upg_client_addr;
  /*!
    the binary size of upg client
    */
  u32 upg_client_size;
  /*!
    zip type
    */
  upg_zip_t zip_type;
  /*!
    uart to use in UPG
    */
  u8 uart_id;
  /*!
    uart baudrate when upg
    */
  u32 upg_baudrate;
  /*!
    uart baudrate when normal
    */
  u32 normal_baudrate;  
  /*!
    uart baudrate when normal
    */
  u32 sync_baudrate;  
  /*!
    client buffer
    */
  u8 *p_client_buffer;
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
  slave_info_t *p_slave_info;
  /*!
    exit_key
    */
  u16 exit_key[8];
  /*!
    menu_key
    */
  u16 menu_key[8];
  /*!
    chip_type
    */
  upg_chip_t upg_chip_type;
  /*!
    sync byte
    */
  u8 sync_byte;
  /*!
    response byte
    */
  u8 res_byte;
}upg_config_t;

/*!
  UPG block info, used to negotiate
  */
typedef struct
{
  /*!
    block id in master side
    */
  u8 master_block_id;   
  /*!
    block id in slave side
    */
  u8 slave_block_id;
  /*!
    block size, only available for upgrade all
    */
  u32 size;
}upg_block_t;

/*!
  UIO implementation policy
  */
typedef struct
{
  /*!
    Initialize the upg ap, and add config info
    \param[out] p_cfg the configuration to fill
    */
  upg_ret_t (*p_init)(upg_config_t *p_cfg);
  /*!
    process block info from client
    \param[in] block_num pointer to block number
    \param[in] block pointer to the blocks to upgrade
    \param[in] block pointer to slave info

    \return SUCCESS if OK, else fail
    */
  upg_ret_t (*p_block_process)(u32 block_num, upg_block_t *p_block, slave_info_t *p_info);
  /*!
    process before start UPG
    */
  void (*p_pre_start)(upg_config_t *p_cfg);
  /*!
    process after exit UPG
    */
  void (*p_post_exit)(upg_config_t *p_cfg);
  /*!
    get UIO hardware code
    \param[in] index key index
    \param[out] code hardware key code

    \return the hardware code number
    */
  u16 (*p_get_code)(u32 index, u16 *p_code);
  /*!
    p_set_key
    */
  void (*p_set_key)(s8 *p_key_code, upg_config_t *p_cfg);
  /*!
    set botrom for warrios
    */
    void (*p_set_brom)(u32 index,u32 size);  
  /*!
     ddr init chip
    */
   upg_ret_t(*p_init_chip)(upg_config_t *p_cfg);   
}upg_policy_t;

/*!
  Get UPG policy instance
  
  \return upg policy instance
  */
upg_policy_t *construct_upg_policy(void);

/*!
  Get UPG instance and return upg application instance
  \param[in] p_upg_policy UPG application implementation policy
  */
app_t *construct_ap_upg(upg_policy_t *p_upg_policy);

#endif // End for __AP_UPGRADE_H_


