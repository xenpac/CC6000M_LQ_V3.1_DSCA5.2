/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_PUBLIC_DEF_H__
/*!
  define file header
  */
#define __OTA_PUBLIC_DEF_H__

/*!
  ota main code block id.
  */
#define OTA_ALL_MAIN_CODE_BLOCK_ID 0XFF
/*!
  ota can upg max blockdefine max ota block number.
  */
#define OTA_MAX_BLOCK_NUMBER 64

/*!
  OTA select work type
  */
 typedef enum
{
/*!
  OTA worke select auto,it do lock, check,download burn flash;
  */
  OTA_WORK_SELECT_AUTO = 0,
/*!
  OTA worke select auto,it do lock,check;
  */
  OTA_WORK_SELECT_CHECK,
/*!
  OTA worke select auto,it do download data,but had locked and checked;
  */
  OTA_WORK_SELECT_DOWNLOAD,
/*!
  OTA worke select auto,it do burn flash,but had locked and checked and download;
  */
  OTA_WORK_SELECT_BURNFLASH,
/*!
  OTA worke select auto,it do download and burn flash;but it had check;
  */
  OTA_WORK_SELECT_UPGGRADE,
/*!
  OTA worke select monitor,it work on monitor check;
  */
  OTA_WORK_SELECT_MONITOR,
/*!
  OTA worke select monitor,it work on monitor check;
  */
  OTA_WORK_SELECT_INQUIRE_STOP,
}ota_work_t;

/*!
  OTA monitor commond
  */
typedef enum
{
/*!
  defines ota monitor cmd start 
  */
  OTA_MONITOR_CMD_START = 0,
  /*!
  defines ota monitor cmd stop 
  */
  OTA_MONITOR_CMD_STOP,
}ota_monitor_cmd_t;

/*!
  OTA upgrade medium
  */
  typedef enum
{
 /*!
  OTA upgrade medium unkown
  */
   OTA_MEDIUM_BY_UNKOWN = 0,
/*!
  OTA upgrade medium by tuner
  */
   OTA_MEDIUM_BY_TUNER,
/*!
  OTA upgrade medium by usb
  */
   OTA_MEDIUM_BY_USB,
/*!
  OTA upgrade medium by card
  */
   OTA_MEDIUM_BY_CARD,
/*!
  OTA upgrade medium by net
  */
   OTA_MEDIUM_BY_NET,
/*!
  OTA upgrade medium by ca
  */
   OTA_MEDIUM_BY_CA,
/*!
  OTA upgrade medium by R232
  */
   OTA_MEDIUM_BY_RS232,
}ota_medium_t;

/*!
  stream protocol type
  */
typedef enum
{
  /*!
    stream protocol type:dvbs
    */
  OTA_STREAM_PROTOCOL_TYPE_DVBS = 0,
  /*!
    stream protocol type:abs
    */
  OTA_STREAM_PROTOCOL_TYPE_ABS,
  /*!
    stream protocol type:dsmcc
    */
  OTA_STREAM_PROTOCOL_TYPE_DSMCC,
  /*!
    stream protocol type:ts
    */
  OTA_STREAM_PROTOCOL_TYPE_TS,
}ota_stream_protocol_t;

/*!
  ota trigger mode
  */
typedef enum
{
  /*!
    no ota.
    */
  OTA_TRI_MODE_NONE = 0,
  /*!
    ota force.
    */
  OTA_TRI_MODE_KEY,
  /*!
    ota auto
    */
  OTA_TRI_MODE_AUTO,
  /*!
    ota force.
    */
  OTA_TRI_MODE_FORC,

}ota_tri_mode_t;

/*!
 ota_print_level
  */
typedef enum 
{
  /*!
    ota   
    */
  OTA_DEBUG_OFF,
  /*!
    ota all debug info
    */
  OTA_DEBUG_ALL,
  /*!
    ota info level
    */
  OTA_DEBUG_INFO,
  /*!
    ota info level
    */
  OTA_DEBUG_WARN,
  /*!
    ota error level
    */
  OTA_DEBUG_ERROR,
  /*!
    ota assert level
    */
  OTA_DEBUG_ASSERT,
  /*!
    ota other level
    */
  OTA_DEBUG_OTHER,
  
}ota_debug_level_t;

/*!
  ota stop result inqire
  */
typedef enum
{
/*!
  ota stop result inqire enable stop
  */
 OTA_STOP_RESULT_ENABLE_STOP = 0,
 
/*!
  ota stop result inqire enable stop
  */
 OTA_STOP_RESULT_FORBID_STOP,
}ota_stop_result_t;


/*!
 ota Block information of data 
 */
typedef struct tag_ota_block_info
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
    ota new version
    */
  u16 ota_new_ver;
  /*!
    ota_old_version
    */
  u16 ota_old_ver;
  /*!
    ota block exist mak,if block date is actual,set TRUE;
    */
  BOOL exist_bit;
  /*!
    ota block upg mak,if block need to upgard,set TRUE;
    */
  BOOL upg_bit;
   /*!
    ota  upg process mak,burn block befault :FORC,finish:none,other case:auto,set TRUE;
    */
  ota_tri_mode_t tri_mode;
}ota_block_info_t;

/*!
  ota tdi
  */
typedef struct
{
  /*!
    customer id
    */
  u32 oui; 
  /*!
    manufacture id
    */
  u16 manufacture_id;
  /*!
    hardware version
    */
  u16 hw_version;
  /*!
    hardware version(sub)
    */
  u16 hw_mod_id;
  /*!
    hardware version(main)
    */
  u16 sw_mod_id;
  /*!
    manufacture id
    */
  u8 stbid[64];
}ota_hw_tdi_t;

/*!
  ota dsi info
  */
typedef struct
{
/*!
  factory force mark,TRUE:factory;false:nomal
  */
  BOOL is_force;
/*!
  serial stb id start
  */
  u8 stbid_start[128];
/*!
  serial stb id end
  */
  u8 stbid_end[128];
/*!
  customer id
  */
  u32 oui; 
/*!
  hardware version
  */
  u16 hw_version;
/*!
  hardware version(sub)
  */
  u16 hw_mod_id;
/*!
  hardware version(main)
  */
  u16 sw_mod_id;
/*!
 software version
*/
  u16 sw_version;
}ota_dsi_info_t;

/*! 
  dmx filter param by customer
  */
typedef struct tag_dmx_filter_param
{
/*! 
  Filter match value
  */
  u8 *p_filter_data;
/*! 
  Filter mask value
  */
  u8 *p_filter_mask;
/*! 
  Filter pid
  */
  u16 pid;
/*! 
  Filter grop id
  */
  u8 group_id;
/*! 
  Filter customer flag,use:1,no use 0;
  */
  u8 m_flag; 
}dmx_filter_param_t;

typedef  struct tag_ota_special_info
{
  /*!
    the ota display version value
    */
  u8 sw_disp_version[32];
  /*!
    the ota auto switch
    */     
  BOOL g_ota_auto;
}ota_special_info_t;

/*!
  dsmcc def printf !
  */
typedef BOOL (*ota_printf_t)(const char *p_fmt, ...);

/*!
  ota version check mode !
  */
typedef BOOL (*ota_version_check_t)(u16 new_ver,u16 old_ver,u8 stbid_s[128],u8 stbid_e[128]);
/*!
  ota version check mode !
  */
typedef BOOL (*ota_dsi_info_check_t)(ota_dsi_info_t *p_info);

/*!
  ota get lock status !
  */
typedef BOOL (*ota_get_lock_status_t)(void);

/*!
  ota block data check !
  */
typedef BOOL (*ota_block_data_verify_t)(u8 id,u8 *p_buffer,u32 size);

/*!
  ota md5 data check !
  */
typedef BOOL (*ota_md5_data_sign_t)(u8 md5_value[16],u8 *p_buffer,u32 size,ota_special_info_t *p_info);

/*!
  ota  config
  */
typedef struct tag_ota_config
{
/*!
  buffer for attach buffer.
  */
  u8 *p_attach_buffer;
/*!
  buffer size.
  */
  u32 buffer_size;
/*!
  use demux
  */
  BOOL dmx_buf_size;  
/*!
  flash size
  */
  u32 flash_size;
/*!
  bootload size
  */
  u32 bootload_size;
/*!
  ota number
  */
  u8 ota_number;
 /*!
   ota block id
   */
  u8 ota_curr_block_id;
/*!
 ota block id
 */
  u8 ota_back_block_id;
/*!
  maincode block id
  */
  u8 maincode_block_id;
/*!
  OTA  trigger mode
  */
  ota_tri_mode_t tri_mode;
/*!
  OTA burn flash when flash destroy,
  set it TRUE when burn flash,set FALSE when burn finish
  */
  BOOL destroy_flag;
/*!
  OTA burn flash when flash destroy,
  destroy block id
  */
  u8 destroy_block_id;
/*!
  boot load block in fast logo
  */
  u8 load_block_id;
/*!
  into ota but ota fail times,when auto ota mode;but don't burn flash
  */
  u8 fail_times;
/*!
  ota funtion in maincode,TRUE: ota in maincode
  */
  BOOL ota_in_maincode_bit;
/*!
  ota debug level,ota_api_debug_level
  */
  ota_debug_level_t debug_level;
/*!
 auto ota fail max times
 */
  u8 max_fail_times;
/*!
  ota upg version
  */
  u32 upg_version;
/*!
  ota dsmc_msg_timeout
  */
  u32 dsmcc_msg_timeout;
/*!
  system all block
  */ 
  ota_block_info_t sys_block[OTA_MAX_BLOCK_NUMBER];
/*!
  ota hw tdi
  */
  ota_hw_tdi_t hw_tdi;
/*!
  ota dm debug funciton
  */
  ota_printf_t ota_debug_printf;
/*!
  ota do lock,is_lock TRUE:lock ,FALSE:unlock;
  */
  void (*ota_do_lock)(BOOL is_lock,u8 lock_mode,u32 *param);
/*!
  ota dsi info check;
  */
  ota_dsi_info_check_t ota_dsi_info_check;
/*!
  ota version check;
  */
  ota_version_check_t ota_version_check;
/*!
  ota get lock status call back;
  */
  ota_get_lock_status_t ota_get_lock_status;
/*!
  ota block data  verify;
  */
  ota_block_data_verify_t ota_block_data_verify;
/*!
  ota md5 data  sign;
  */
   ota_md5_data_sign_t ota_md5_data_sign;
/*!
  ota download direct;
  */
    BOOL g_direct_dl;
/*!
  ota download direct;
  */
    dmx_filter_param_t  filter_param;
/*!
  demux index for dmx filter;
  */
  u16 demux_index;
}ota_config_t;

/*!
  nim info
  */
typedef struct tg_nim_info_t
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
}nim_info_t;

/*!
OTA  tp info 
*/
typedef struct tg_ota_tp_info
{
  /*!
    do ota tp,but it may be no ota set tp
    */
  nim_info_t do_ota_tp;
  /*!
    ota set tp,in system ota tp set
    */
  nim_info_t ota_set_tp;
  /*!
    ota set back tp,it is main tp normal
    */
  nim_info_t ota_set_back_tp;
}ota_tp_info_t;

/*!
  upg check info
  */
  typedef struct
{
  /*!
    dsmcc dsi upg info new version
    */
  u32 upg_new_version;
  /*!
    dsmcc dsi old version
    */
  u32 upg_old_version;
  /*!
    file size
    */
  u32 upg_file_size;
  /*!
    dsmcc db pare finish bot
    */
  u8 dsi_group_id;
  /*!
    software is zipped
    */
  BOOL zipped_bit;
  /*!
    dsmcc db pare finish bot
    */
  BOOL focre_bit;
  /*!
    dsmcc db pare finish bot
    */
  BOOL upg_bit;
/*!
  dsmcc dsi private bit
  */
  BOOL dsi_priv_bit;
  
}upg_check_info_t;

/*!
  OTA monitor para
  */
typedef struct
{
/*!
  ota monitor cycle time :value * 1s
  */
   u32 monitor_cycle_time;
/*!
  ota monitor commond
  */
   ota_monitor_cmd_t cmd;
/*!
  ota filter monitor pid
  */
  u16 monitor_pid;
}ota_monitor_para_t;

/*!
  define p_print_list
  */
typedef char *p_print_list;
/*!
  define printf native int
  */
typedef unsigned int  PRINTF_NATIVE_INT;
/*!
  * Storage alignment properties
  */
#define PRINTF_AUPBND (sizeof(PRINTF_NATIVE_INT) - 1)
/*!
  define printf funtion
  */
#define PRINTF_ADNBND (sizeof(PRINTF_NATIVE_INT) - 1)
/*!
  define printf funtion
  */
#define PRINTF_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
/*!
  define printf funtion
  */
#define PRINTF_VA_ARG(e, T)      (*(T *)(((e) += (PRINTF_BND (T, PRINTF_AUPBND))) \
                                                          - (PRINTF_BND (T,PRINTF_ADNBND))))
/*!
  define printf funtion 
  */
#define PRINTF_VA_END(e)         (void)0
/*!
  define printf funtion
  */
#define PRINTF_VA_START(e, A)    (void) ((e) = (((char *) &(A)) + (PRINTF_BND (A,PRINTF_AUPBND))))
/*!
  define printf funtion 
  */
#define MAX_PRINT_BUFFER_LEN  512


#endif

