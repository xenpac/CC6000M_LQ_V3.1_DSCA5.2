/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_FILTER_H__
/*!
  incoude file head
  */
#define __OTA_FILTER_H__

/*!
  dm maz group number
  */
#define DMH_MAX_GROUP_NUMBER 12
/*!
  dm maz block number
  */
#define DMH_GROUP_MAX_BLOCK_NUMBER 64
/*!
  dm make size
  */
#define DMH_MARK_SIZE 12
/*!
  dm tag total size
  */
#define DMH_TAG_TOTAL_SIZE 16 /***mark + 4byte**/
/*!
  dm base info size
  */
#define DMH_BASE_INFO_LEN 12

/*!
  section size
  */
#define OTA_SECTOR_SIZE (64 * 1024)

/*!
  defines eit section buffer size
  */
#define EIT_SECTION_BUFFER_SIZE                   (4*(KBYTES) + 188)

/*!
  ota filter locked status.
  */
typedef enum tg_epg_filter_locked
{
/*!
  ota filter locked status:unlock.
  */
  OTA_FILTER_LOCK_STATUS_UNLOCK = 0,
/*!
  ota filter locked status:locked.
  */
  OTA_FILTER_LOCK_STATUS_LOCKED, 
}epg_filter_lock_status_t;
/*!
  ota filter command.
 */
typedef enum epg_filter_cmd
{
/*!
  config filter config.
  */
  OTA_FILTER_CMD_CONFIG,
/*!
  config filter do policy.
  */
  OTA_FILTER_CMD_DO_POLICY,  
    /*!
  process lock result.
  */
  OTA_FILTER_CMD_LOCK_PROCESS, 
#if 0
/*!
  start lock.
  */
  OTA_FILTER_CMD_START_LOCK,
/*!
  stop lock.
  */
  OTA_FILTER_CMD_LOCK_STOP, 
/*!
  check ota.
  */
  OTA_FILTER_CMD_START_CHECK,
/*!
  download data.
  */
  OTA_FILTER_CMD_START_DOWNLOAD,
/*!
  burn flash.
  */
  OTA_FILTER_CMD_START_BURNFLASH,
 /*!
   filter monitor.
   */
  OTA_FILTER_CMD_START_MONITOR,
 /*!
   filter stop.
   */
  OTA_FILTER_CMD_STOP,
  #endif
}ota_filter_cmd_t;

/*!
  filter event. 
  */
typedef enum tag_ota_filter_evt
{
/*!
  filter event: unkown. 
  */
  OTA_FILTER_EVENT_UNKOWN = OTA_FILTER << 16,
/*!
  filter event:ota save upg version
  */
  OTA_FILTER_EVENT_STOP_RESULT,
/*!
  filter event:lock. 
  */
  OTA_FILTER_EVENT_LOCK,
/*!
  filter event:unlock. 
  */
  OTA_FILTER_EVENT_UNLOCK,
/*!
  filter event:check fail. 
  */
  OTA_FILTER_EVENT_CHECK_FAIL,
/*!
  filter event:check success. 
  */
  OTA_FILTER_EVENT_CHECK_SUCCESS,
/*!
  filter event:download fail. 
  */
  OTA_FILTER_EVENT_DOWNLOAD_FAIL,
/*!
  filter event:download ing. 
  */
  OTA_FILTER_EVENT_DOWNLOAD_START,
/*!
  filter event:download ing. 
  */
  OTA_FILTER_EVENT_DOWNLOAD_ING,
/*!
  filter event:download time out. 
  */
  OTA_FILTER_EVENT_DOWNLOAD_TIMEOUT,
/*!
  filter event:download success. 
  */
  OTA_FILTER_EVENT_DOWNLOAD_SUCCESS,
/*!
  filter event:burnflash reboot. 
  */
  OTA_FILTER_EVENT_NO_BLOCK_UPG,
/*!
  filter event:burn flash fail. 
  */
  OTA_FILTER_EVENT_BURNFLASH_FAIL,
/*!
  filter event:burn flash start. 
  */
  OTA_FILTER_EVENT_BURNFLASH_START,
/*!
  filter event:burn flash factory. 
  */
  OTA_FILTER_EVENT_BURNFLASH_FACTORY,
/*!
  filter event:burn flash block. 
  */
  OTA_FILTER_EVENT_BURNFLASH_BLOCK,
/*!
  filter event:burn flash ing. 
  */
  OTA_FILTER_EVENT_BURNFLASH_ING,
/*!
  filter event:burnflash success. 
  */
  OTA_FILTER_EVENT_BURNFLASH_SUCCESS,
/*!
  filter event:burnflash reboot. 
  */
  OTA_FILTER_EVENT_BURNFLASH_REBOOT,
/*!
  filter event:burnflash request time out. 
  */
  OTA_FILTER_EVENT_REQEUST_TIMEOUT,
/*!
  filter event:save curr ota id. 
  */
  OTA_FILTER_EVENT_BURNFLASH_SAVE_CURR_OTA_ID,
/*!
  filter event:save curr ota id. 
  */
  OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
/*!
  filter event:save version. 
  */
  OTA_FILTER_EVENT_BURNFLASH_SAVE_BLOCK_VERSION_INFO,
/*!
  filter event:ota status. 
  */
  OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_STATUS,
/*!
  filter event:ota destroy_flag
  */
  OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_DSTROY_FLAGE,
/*!
  filter event:ota fail time
  */
  OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME,
/*!
  filter event:ota save upg version
  */
  OTA_FILTER_EVENT_BURNFLASH_SAVE_UPG_VERSION,
  /*!
  filter event:reset db date BLOCK
  */
  OTA_FILTER_EVENT_BURNFLASH_RESET_DB_DATA_BLOCK,
/*!
  filter event:reset db date BLOCK
  */
  OTA_FILTER_EVENT_BURNFLASH_RESET_SYS_DATA_BLOCK,
    /*!
  filter event:reset ca date BLOCK
  */
  OTA_FILTER_EVENT_BURNFLASH_RESET_CA_DATA_BLOCK,
/*!
  filter event:reset ads date BLOCK
  */
  OTA_FILTER_EVENT_BURNFLASH_RESET_ADS_DATA_BLOCK,

}ota_filter_evt_t;

/*!
  defines ota filter  burn file type. 
  */
typedef enum tag_ota_burn
{
  /*!
  unkown 
  */
  OTA_BURN_FILE_TYPE_UNKOWN = 0,
/*!
  only block 
  */
  OTA_BURN_FILE_TYPE_ONLY_BLOCK,
/*!
  main code 
  */
  OTA_BURN_FILE_TYPE_MAIN_CODE,
/*!
  main code 
  */
  OTA_BURN_FILE_TYPE_ALL_CODE,
/*!
  main code 
  */
  OTA_BURN_FILE_TYPE_OTA,
}ota_burn_file_t;

/*!
  the all or main code break up to block
  */
typedef struct tag_ota_cut_info
{
  /*!
    cut block num
    */
  u8 block_num;
  /*!
    cut block id
    */
  u8 block_id[OTA_MAX_BLOCK_NUMBER];
}tag_ota_cut_t;
/*!
  the all or main code jump  block
  */
typedef struct tag_ota_jump_info
{
  /*!
    cut block num
    */
  u8 block_num;
  /*!
    cut block id
    */
  u8 block_id[OTA_MAX_BLOCK_NUMBER];
}tag_ota_jump_t;

/*!
 ota Block information of data manager header
 */
typedef struct tag_ota_dm_info
{
  /*!
    Block id
    */
  u8 id;
  /*!
    Block type refer to enum tag_block_type
    */
  u8 type; 
  /*!
    Base address of current block
    */
  u32 base_addr;
  /*!
    dm offset
    */
  u32 dm_offset;
  /*!
    Block size
    */
  u32 size;
  /*!
    Block size
    */
  u32 crc;
  /*!
     exist mark
    */
  BOOL exist_bit;
}ota_dm_info_t;

/*!
 ota Block information of data 
 */
typedef struct tag_ota_data_info
{
  /*!
    Block id
    */
  u8 id;
  /*!
    Block type refer to enum tag_block_type
    */
  u8 type; 
  /*!
    group id
    */
  u8 group_id;
  /*!
    ota new version
    */
  u16 ota_new_ver;
  /*!
    Base address of current block
    */
  u32 base_addr;
  /*!
    Base address of current block
    */
  u32 dm_offset;
  /*!
    Block size
    */
  u32 size;
  /*!
    CRC byte
    */
  u32 crc;
  /*!
    file size
    */
  u32 file_size;
  /*!
    data length
    */
  u32 data_len;
  /*!
    valid data cnt
    */
  u32 buf_cnt;
  /*!
    the all or main code break up to block:FALSE: no cut;TRUE:break up to block
    */
  BOOL is_data_cut;
  /*!
    ota block exist mak,if block date is actual,set TRUE;
    */
  BOOL exist_bit;
  /*!
    ota block upg mak,if block need to upgard,set TRUE;
    */
  BOOL upg_bit;
  /*!
    ota dl group info mak,if block need to upgard,set TRUE;
    */
  BOOL dl_group_bit;
  /*!
    ota block upg mak,if block need to upgard,set TRUE;
    */
  BOOL downloaded_bit;
  /*!
    ota block upg mak,if block need to upgard,set TRUE;
    */
  BOOL burned_flash_bit;
  /*!
    is zipped.
    */
  BOOL is_zipped;
  /*!
    ota ts force; FALSE:common;TRUE:factory force all flash ota;
    */
  BOOL is_factory_force;
  /*!
    block new buf mark;
    */
  BOOL is_new_buf;
  /*!
    download save buffer
    */
  //u8 *p_save_buf;
  /*!
    download save buffer
    */
  u8 *p_burn_buf;
  /*!
    download burn size
    */
  //u32 save_size;
  /*!
    download burn size
    */
  u32 burn_size;
  /*!
    the all or main code break up to block
    */
  tag_ota_cut_t break_block;
  /*!
    the all or main code break up to block
    */
  tag_ota_jump_t jump_block;
    /*!
    allow dm auto merge block,TRUE:allow
    */
  ota_burn_file_t file_type;
  /*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_user_db_data_clear;
  /*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_user_sys_data_clear;
  /*!
    the ca data clear flag,TRUE:clease
    */
  BOOL g_ca_data_clear;
  /*!
      the ads data clear flag,TRUE:clease
      */
  BOOL g_ads_data_clear;
}ota_data_info_t;
/*!
  ota filterconfig
  */
typedef struct
{
  /*!
    ota work select
    */
  //ota_work_t  run_select;
  /*!
    ota medium select
    */
  ota_medium_t medium;
  /*!
    stream protocol type
    */
  ota_stream_protocol_t protocol_type;
  /*!
    ota config
    */
  ota_config_t filter_cfg;
}ota_filter_cfg_t;

/*!
  ota pin config
  */
typedef struct
{
  /*!
    ota medium select
    */
  ota_medium_t medium;
  /*!
    stream protocol type
    */
  ota_stream_protocol_t protocol_type;
  /*!
    lib memp
    */
  lib_memp_t *p_mem_heap;   
  /*!
    use memory heap
    */
  BOOL use_memp;  
  /*!
    use demux
    */
  BOOL dmx_buf_size;  
/*!
  ota dsmc_msg_timeout
  */
  u32 dsmcc_msg_timeout;
/*!
  ota download direct;
  */
    dmx_filter_param_t  filter_param;
}ota_ipin_cfg_t;

/*!
  block header info
  */
typedef struct tag_block_header_info_t
{
/*!
  dm data :block id
  */
  u8 id;
/*!
  dm data :block type
  */
  u8 type; //refer to enum tag_block_type
/*!
  dm data :block node num
  */
  u16 node_num;
/*!
  dm data :block base addr
  */
  u32 base_addr;
/*!
  dm data :block size
  */
  u32 size;
/*!
  dm data :crc
  */
  u32 crc;
/*!
  dm data :version
  */
  u8 version[8];
/*!
  dm data :name
  */
  u8 name[8];
/*!
  dm data :time
  */
  u8 time[12];
/*!
  dm data :reserve
  */
  u32 reserve;
}block_header_info_t;

/*!
 ota Block information of data manager header
 */
typedef struct tag_ota_dmh_group
{
  /*!
    Base address of current block
    */
  u32 dmh_start_pos;
  /*!
    sdram size
    */
  u32 sdram_size;
  /*!
    flash size
    */
  u32 flash_size;
  /*!
    block number
    */
  u16 block_num;
  /*!
    block header size, use to check
    */
  u16 bh_size;
  /*!
    DM BLOCK INFO
    */
  ota_dm_info_t dm_block[DMH_GROUP_MAX_BLOCK_NUMBER];
}ota_dmh_group_t;

/*!
 ota Block information of data manager header
 */
typedef struct tag_ota_dmh_info
{
  /*!
    dm head number
    */
  u8 header_num;
    /*!
    dm head group
    */
  ota_dmh_group_t header_group[DMH_MAX_GROUP_NUMBER];
}ota_dmh_info_t;



/*!
 create ota filter.
 */
ifilter_t *ota_filter_create(void *p_para);

/*!
  get dm header info .
  */
BOOL ota_filter_find_dmh_info(u8 *p_buf,u32 buf_size,
              ota_dmh_info_t *dmh_info,
              u32 dm_offset);

#endif //End for __OTA_FILTER_H_
