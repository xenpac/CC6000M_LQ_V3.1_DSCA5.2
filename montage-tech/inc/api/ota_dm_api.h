/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_DM_API_H__
/*!
 incoude file head
  */
#define __OTA_DM_API_H__
/*!
 picece block:ota bootload info
  */
#define OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID     0x01
/*!
 picece block:ota param
  */
#define OTA_DM_BLOCK_PIECE_OTA_PARAM_ID      0x02

/*!
 picece max alig size
  */
#define PICE_MAX_ALIGN_SIZE 1024
/*!
 dmh max block number
  */
#define OTA_DM_MAX_BLOCK_NUMBER  64
/*!
 dmh max group number
  */
#define OTA_DM_DMH_MAX_GROUP_NUMBER 12
/*!
 dm header make size
  */
#define OTA_DM_DMH_MARK_SIZE 12
/*!
 dm header tag size
  */
#define OTA_DM_DMH_TAG_TOTAL_SIZE 16 /***mark + 4byte**/
/*!
 dm header base info len
  */
#define OTA_DM_DMH_BASE_INFO_LEN 12

/*!
 ota_dm_api_print_level
  */
typedef enum 
{
  /*!
    ota dm  
    */
  OTA_DM_DEBUG_OFF,
  /*!
    ota dm  all debug info
    */
  OTA_DM_DEBUG_ALL,
  /*!
    ota dm info level
    */
  OTA_DM_DEBUG_INFO,
  /*!
    ota dm info level
    */
  OTA_DM_DEBUG_WARN,
  /*!
    ota dm error level
    */
  OTA_DM_DEBUG_ERROR,
  /*!
    ota dm assert level
    */
  OTA_DM_DEBUG_ASSERT,
  /*!
    ota dm other level
    */
  OTA_DM_DEBUG_OTHER,
  
}ota_dm_debug_level_t;

/*!
  piece register
  */
typedef struct piece_register_tg
{
  /*!
    piece id
    */
  u8 piece_id;
  /*!
    piece size
    */
  u32 piece_size;
}piece_register_t;
/*!
  ota boot load info
  */
typedef struct ota_bl_info_tg
{
  /*!
    ota taltal number
    */
    u32 ota_number;
  /*!
    ota curr id
    */
    u32 ota_curr_id;
  /*!
    ota backup id
    */
    u32 ota_backup_id;
  /*!
    ota flash destroy flag
    */
    BOOL destroy_flag;
  /*!
    ota status: nomal,key,auto,force
    */
    u32 ota_status; 
  /*!
    into ota but don't burn flash times
    */ 
    u32 fail_times;
  /*!
    into ota but don't burn flash times
    */ 
    u32 medium_type;
   /*!
     reserved data
     */
     u32 destroy_block_id;
  /*!
    reserved data
    */
    u32 load_block_id;
  /*!
    reserved data
    */
    u32 reserved3;
  /*!
    reserved data
    */
    u32 reserved4;
}ota_bl_info_t;

/*!
 ota Block information of data 
 */
typedef struct block_ota_info_tg
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
    ota  version
    */
  u16 ota_ver;
  /*!
    Base address of current block
    */
  u32 base_addr;
  /*!
    Block size
    */
  u32 size;
  /*!
    CRC byte
    */
  u32 crc;
 /*!
   reserved
   */
  u32 reserved1;
  /*!
   reserved
   */
  u32 reserved2;
 /*!
   reserved
   */
  u32 reserved3;
  /*!
    ota block exist mak,if block date is actual,set TRUE;
    */
  BOOL exist_bit;
}block_ota_info_t;

/*!
  nim para
  */
typedef struct tg_nim_para_t
{
/*!
  Lock mode
  */
  sys_signal_t lock_mode;
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
  /*!
    The  pid of download stream
    */
  u16 data_pid;
 /*!
   reserved data
   */
  u32 reserved1;
 /*!
    reserved data
   */
  u32 reserved2;
 /*!
    reserved data
   */
  u32 reserved3;
 /*!
    reserved data
   */
  u32 reserved4;
}nim_para_t;
/*!
  ota param
  */
typedef struct ota_param_tg
{
/*!
  ota upg version
  */
  u32 upg_version;
 /*!
   block do ota tp
   */
   nim_para_t ota_tp;
 /*!
   block ota sys set tp
   */
   nim_para_t ota_set_tp;
 /*!
   block ota sys set back tp
   */
   nim_para_t ota_set_back_tp;
 /*!
   block ota info
   */
   block_ota_info_t block_table[OTA_DM_MAX_BLOCK_NUMBER];
  /*!
   reserved data
   */
  u32 reserved1;
 /*!
    reserved data
   */
  u32 reserved2;
 /*!
    reserved data
   */
  u32 reserved3;
 /*!
    reserved data
   */
  u32 reserved4;
}ota_param_t;

/*!
  block header info
  */
typedef struct tag_block_hdr_info_t
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
}block_hdr_info_t;

/*!
 ota Block information of data manager header
 */
typedef struct tag_dmh_table
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
    dm offset
    */
  u32 dm_offset;
  /*!
    DM BLOCK INFO
    */
  block_hdr_info_t dm_block[OTA_DM_MAX_BLOCK_NUMBER];
}dmh_table_t;

/*!
 ota Block information of data manager header
 */
typedef struct tag_dmh_info
{
  /*!
    dm head number
    */
  u8 header_num;
    /*!
    dm head group
    */
  dmh_table_t  header_group[OTA_DM_DMH_MAX_GROUP_NUMBER];
}dm_dmh_info_t;

/*!
  ota dm config
  */
typedef struct ota_dm_config_tg
{
  /*!
    save dm block id
    */
   u8 ota_dm_block_id;
  /*!
    backup block id
    */
   u8 ota_dm_backup_block_id;
  /*!
    save dm address
    */
   u32 ota_dm_addr;
  /*!
    backup address
    */
   u32 ota_dm_backup_addr;
  /*!
    don't set it nomal,if it is TRUE,it will not use backup
    */
  BOOL disable_backup_block ;
  /*!
    Mutex switct,TRUE:mutex,FALSE:LOCK
    */
  BOOL is_use_mutex;
  /*!
    the mutex prioty
    */
  u32 mutex_prio;
  /*!
    flash protect,if it is TRUE,it will user protect flash
    */
    BOOL is_protect;
  /*!
    piece align
    */
    BOOL align_size;
   /*!
    flash size
    */
    u32 flash_size;
  /*!
    flash dm info pointer
    */
    dm_dmh_info_t  **pp_dm_info;
  /*!
    ota dm debug level
    */
  ota_dm_debug_level_t debug_level;
  /*!
    ota dm debug funciton
    */
  int (*ota_dm_api_printf)(const char *p_fmt, ...);
}ota_dm_config_t;


/*!
  ota data manage api proting:api init
  note:piece id dn't repeat
  */
RET_CODE mul_ota_dm_api_init(ota_dm_config_t *p_cfg);
/*!
  ota data manage api proting:un forbid
  */
void mul_ota_dm_api_unforbid(void);
/*!
  ota data manage api proting:forbid
  */
void mul_ota_dm_api_forbid(void);
/*!
  ota data manage api proting:destroy
  */
RET_CODE mul_ota_dm_api_destroy(void);

/*!
  ota data manage api proting:register piece
  note:piece id dn't repeat;the total size dn't biger block size
  */
void mul_ota_dm_api_register_piece(piece_register_t *p_para);

/*!
  ota data manage api proting:check intata
  input param 1:peice id
  return TRUE or FALSE,note:it is FALSE,it is destoried!need to init picec!
  */
BOOL mul_ota_dm_api_check_intact_picec(u32 piece_id);
/*!
  ota data manage api proting:read data
  input param 1:peice id
  input param 2:read data buffer
  input param 3:read data size
  return ERR or SUCCESS
  */
RET_CODE mul_ota_dm_api_read_data(u32 piece_id,u8 *p_data, u32 rd_size);
/*!
  ota data manage api proting:write data
  input param 1:peice id
  input param 2:write data buffer
  input param 3:write  data size
  return ERR or SUCCESS
  */
RET_CODE mul_ota_dm_api_write_data(u32 piece_id,u8 *p_data, u32 wr_size);

/*!
  ota data manage api proting:reset data block
  !!!!! \O/  note: don't use this function,but factory ota!!!!!
  !!!!!  /\   !!!!!
  */
void mul_ota_dm_api_reset_data_block(void);

/*!
 ota find dm header info.the function don't bind thi ota data manange,it only suport function api
  input param1: dm start pos distance buf,
  input param2:buffer,
  input param3,buffer size
  input param4,dmh info
  return TRUE,SUCCESS;
  */
BOOL mul_ota_dm_api_find_dmh_info(u32 flash_addr,
                      u32 dm_offset,
                      u32 buf_size,
                      dm_dmh_info_t *p_dmh_info);
/*!
  ota find dm header info from flash.the function don't bind thi ota data manange,it only suport function api
  input param1: flash addr,
  input param2:flash size,
  input param3,info point
  return TRUE,SUCCESS;
  */
BOOL mul_ota_dm_api_find_dmh_info_from_flash(u32 flash_addr,
                  u32 flash_size,
                  dm_dmh_info_t *p_dmh_info);


/*!
  ********************below is sample,and fixed two piece for ota!**************
  */

/*!
  ota data manage api proting:init boot load save info
  input param 1:info pointer
  */
void mul_ota_dm_api_init_bootload_info(ota_bl_info_t *p_bl_info);
/*!
  ota data manage api proting:read boot load  info
  input param 1:info pointer
  */
void mul_ota_dm_api_read_bootload_info(ota_bl_info_t *p_bl_info);
/*!
  ota data manage api proting:save boot load  info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_bootload_info(ota_bl_info_t *p_bl_info);

/*!
  ota data manage api proting:save boot load  info:destroy flag
  input param 1:flag
  */
void mul_ota_dm_api_save_flash_destroy_flag(BOOL destroy_flag);
/*!
  ota data manage api proting:read boot load  info:destroy flag
  return:flag
  */
BOOL  mul_ota_dm_api_read_flash_destroy_flag(void);
/*!
  ota data manage api proting:save boot load  info:destroy flag
  input param 1:flag
  */
void mul_ota_dm_api_save_flash_destroy_block_id(u32 block_id);

/*!
  ota data manage api proting:read boot load  info:destroy flag
  return:flag
  */
u32  mul_ota_dm_api_read_flash_destroy_block_id(void);
/*!
  ota data manage api proting:save boot load  info:load block id
  input param 1:flag
  */
void mul_ota_dm_api_save_load_block_id(u32 block_id);
/*!
  ota data manage api proting:read boot load  info:load block id
  return:flag
  */
u32  mul_ota_dm_api_read_load_block_id(void);
/*!
  ota data manage api proting:save boot load  info:ota status
  input param 1:status
  */
void mul_ota_dm_api_save_ota_status(u32 status);
/*!
  ota data manage api proting:read boot load  info:destroy flag
  return:status
  */
u32  mul_ota_dm_api_read_ota_status(void);

/*!
  ota data manage api proting:save boot load  info:ota fail time
  input param 1:fail time
  */
void mul_ota_dm_api_save_ota_fail_times(u32 times);
/*!
  ota data manage api proting:read boot load  info:fail time
  return:fail time
  */
u32  mul_ota_dm_api_read_ota_fail_times(void);
/*!
  ota data manage api proting:read boot load  info:ota block id
  return:fail time
  */
void  mul_ota_dm_api_read_ota_block_id(u32 *p_current_id,u32 *p_back_up_id);
/*!
  ota data manage api proting:save boot load  info:ota block id
  input param 1:fail time
  */
void mul_ota_dm_api_save_ota_block_id(u32 current_id,u32 back_up_id);

/*!
  ota data manage api proting:init ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_init_ota_param(ota_param_t *p_param);
/*!
  ota data manage api proting:read ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_read_ota_param(ota_param_t *p_param);
/*!
  ota data manage api proting:save ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_ota_param(ota_param_t *p_param);
/*!
  ota data manage api proting:read ota param upg version
  input param 1:info pointer
  */
u32 mul_ota_dm_api_read_ota_param_upg_version(void);
/*!
  ota data manage api proting:save ota param upg version
  input param 1:info pointer
  */
void mul_ota_dm_api_save_ota_param_upg_version(u32 version);
/*!
  ota data manage api proting:save ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_save_ota_param_tp(nim_para_t *p_tp);

/*!
  ota data manage api proting:read ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_read_ota_param_tp(nim_para_t *p_tp);
/*!
  ota data manage api proting:save ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_save_set_ota_tp(nim_para_t *p_tp);
/*!
  ota data manage api proting:read ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_read_set_ota_tp(nim_para_t *p_tp);
/*!
  ota data manage api proting:save ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_save_set_backup_ota_tp(nim_para_t *p_tp);
/*!
  ota data manage api proting:read ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_read_set_backup_ota_tp(nim_para_t *p_tp);
/*!
  ota data manage api proting:save ota param block info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_all_block_info(
            block_ota_info_t block_table[OTA_DM_MAX_BLOCK_NUMBER]);
/*!
  ota data manage api proting:read ota param block info
  input param 1:info pointer
  */
void mul_ota_dm_api_read_all_block_info(
          block_ota_info_t block_table[OTA_DM_MAX_BLOCK_NUMBER]);
/*!
  ota data manage api proting:save one block ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_one_block_info(u8 block_id,block_ota_info_t *p_block_info);

/*!
  ota data manage api proting:read one block info
  input param 1:info pointer
  note:need check read block info block id ,make be it no exist!!!!
  */
void mul_ota_dm_api_read_one_block_info(u8 block_id,block_ota_info_t *p_block_info);



/*!
  get dm block real info.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
BOOL mul_ota_dm_api_get_block_real_info(u8 block_id,block_hdr_info_t *p_block_info);
/*!
  get dm block real info file size.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
u32 mul_ota_dm_api_get_block_real_file_size(u8 block_id);
/*!
  get dm block real info file size.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
u32 mul_ota_dm_api_get_block_real_reserve(u8 block_id);
/*!
  get dm block real info file size.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
u32 mul_ota_dm_api_get_block_real_crc(u8 block_id);

/*!
  get block addr
  \return 0 if fail, or addr if success
  */
u32 mul_ota_dm_api_get_block_addr(u8 block_id);
/*!
  get block addr
  \return 0 if fail, or addr if success
  */
u32 mul_ota_dm_api_get_block_size(u8 block_id);
/*!
  get block info
  \return 0 if fail, or addr if success
  */
BOOL mul_ota_dm_api_get_block_info(u8 block_id,block_hdr_info_t *p_info);
/*!
  get block data
  \return 0 if fail, or addr if success
  */
u32 mul_ota_dm_api_dm_read(u8 block_id,u16 offset,
                          u32 length, u8 *p_buffer);
/*!
  get dm head info
  return false if fail, maby no init ota dm
  */
BOOL mul_ota_dm_api_get_dmh_info(dm_dmh_info_t *p_dmh_info);


#endif
