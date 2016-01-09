/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_FILTER_INTRA_H_
#define __OTA_FILTER_INTRA_H_
/*!
  ota lock tp type
  */
 typedef enum
{
/*!
  ota lock tp type do tp
  */
   OTA_FILTER_LOCK_TP_NOTHING = 0,
/*!
  ota lock tp type do tp
  */
   OTA_FILTER_LOCK_TP_DO_TP,
/*!
  ota lock tp type sys set
  */
   OTA_FILTER_LOCK_TP_SET_TP,
/*!
  ota lock tp type sys back
  */
   OTA_FILTER_LOCK_TP_BACK_TP,
}ota_filter_auto_lock_t;


/*!
   OTA_FILTER LOOP State Machine definition
  */
typedef enum
{
  /*!
    OTA_FILTER idle state.
    */
   OTA_FILTER_SM_IDLE = 0,
  /*!
    OTA_FILTER locked state.
    */
   OTA_FILTER_SM_LOCKED,
  /*!
    OTA_FILTER unlock state.
    */
   OTA_FILTER_SM_UNLOCK,
   /*!
    OTA_FILTER request time out.
    */
   OTA_FILTER_SM_REQUEST_TIMEOUT,
  /*!
    OTA_FILTER check success.
    */
   OTA_FILTER_SM_CHECK_SUCCESS,
   /*!
    OTA_FILTER check success.
    */
   OTA_FILTER_SM_DOWNLOAD_SUCCESS,
    /*!
    OTA_FILTER  SM AUTO.
    */
   OTA_FILTER_SM_AUTO_START ,
  /*!
    OTA_FILTER  SM check start.
    */
   OTA_FILTER_SM_CHECK_START ,
  /*!
    OTA_FILTER  SM download
    */
   OTA_FILTER_SM_DOWNLOAD_START ,
     /*!
    OTA_FILTER  SM process data download
    */
   OTA_FILTER_SM_CHECK_DOWNLOAD_DATA ,
  /*!
    OTA_FILTER  SM furnflash
    */
   OTA_FILTER_SM_BURNFLASH_START ,
  /*!
    OTA_FILTER  SM upgrade
    */
   OTA_FILTER_SM_UPGRADE_START,
  /*!
    OTA_FILTER  SM stop
    */
   OTA_FILTER_SM_INQUIRE_STOP,
/*!
  OTA_FILTER  SM stop
  */
   OTA_FILTER_SM_MONITOR_START,
/*!
  OTA_FILTER  SM stop
  */
   OTA_FILTER_SM_CHECK_FAIL,
}ota_filter_loop_sm_t;
/*!
   ota filter download data check result
  */
typedef enum
{
/*!
   data is err
  */
  OTA_FILTER_DL_CHECK_RESULT_ERR = 0,
  /*!
   data is success
  */
  OTA_FILTER_DL_CHECK_RESULT_SUCCESS,
 /*!
   data is success
   */
  OTA_FILTER_DL_CHECK_RESULT_NO_BLOCK,
}ota_filter_dl_check_t;


/*!
 ota filter private data.
 */
typedef struct tag_ota_filter_priv
{
/*!
 epg filter handle
 */  
 void *p_this;
  /*!
    input
    */
  ota_input_pin_t m_in_pin; 
  /*!
    output
    */
  ota_output_pin_t m_out_pin;
  /*!
    buffer for attach buffer.
    */
  u8 *p_attach_buffer;
  /*!
    buffer size.
    */
  u32 buffer_size;
  /*!
    lib memp
    */
  lib_memp_t mem_heap;  
  /*!
    lib memp
    */
  lib_memp_t *p_mem_heap;   
  /*!
    use memory heap
    */
  BOOL use_memp;  
  /*!
    ota medium select
    */
  ota_medium_t medium;
  /*!
    ota work select
    */
  ota_work_t  run_select;
  /*!
    stream protocol type
    */
  ota_stream_protocol_t protocol_type;
  /*!
    stream pid
    */
  u16 data_pid;
  /*!
    stream  group
    */
  u16 group_id;
  /*!
    stream  group
    */
  u32 dsi_multi_timeout;
   /*!
  ota dsmc_msg_timeout
  */
  u32 dsmcc_msg_timeout;
  /*!
    system all block table
    */ 
  ota_block_info_t sys_block[OTA_MAX_BLOCK_NUMBER];
  /*!
    ota extern use ota info table 
    */ 
  ota_block_info_t ota_block[OTA_MAX_BLOCK_NUMBER];
  /*!
    ota intra use ota info table 
    */ 
  ota_block_info_t ota_intra_block[OTA_MAX_BLOCK_NUMBER];
  /*!
    ota manage data table
    */ 
  ota_data_info_t  ota_data[OTA_MAX_BLOCK_NUMBER];
  /*!
    ota download file dm info table
    */ 
  ota_dm_info_t dm_info[OTA_MAX_BLOCK_NUMBER];
  /*!
    ota check info
    */ 
  upg_check_info_t upg_check_info;
  /*!
    ota download file dm info table
    */ 
  upg_check_info_t upg_check_intra_info;
  /*!
    ota hw tdi
    */
  ota_hw_tdi_t hw_tdi;
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
   auto ota fail max times
   */
  u8 max_fail_times;
  /*!
    allow upg ota when ota all file,TRUE:allow
    */
  BOOL allow_upg_ota_bit;
  /*!
    program data base reset flag,TRUE:reset
    */
  BOOL db_data_reset_flag;
  /*!
    system data reset flag,TRUE:reset
    */
  BOOL sys_data_reset_flag;
  /*!
    ca data base reset flag,TRUE:reset
    */
  BOOL ca_data_reset_flag;
  /*!
    ads data reset flag,TRUE:reset
    */
  BOOL ads_data_reset_flag;
  /*!
    ota funtion in maincode,TRUE: ota in maincode
    */
  BOOL ota_in_maincode_bit;
  /*!
    ota upg version
    */
  u32 upg_version;
  /*!
    download save buffer
    */
  u8 *p_save_buf;
  /*!
    download save buffer
    */
  u8 *p_burn_buf;
  /*!
    download burn size
    */
  u32 save_size;
  /*!
    download burn size
    */
  u32 burn_size;
  /*!
    ota all tp info
    */
  ota_tp_info_t tp_info;
   /*!
   ota filter auto lock lock tp type;
   */
   ota_filter_auto_lock_t lock_type;
  /*!
    ota filter loop sm
    */
  ota_filter_loop_sm_t loop_sm;
  /*!
    ota filter loop sm param
    */
   u32 sm_param;
  /*!
    ota filter loop sm param
    */
   u32 download_try_times;
   /*!
    ota filter reqest time out mark
    */
   BOOL g_timeout_mark;

  /*!
    ota filter reqest time out mark
    */
   BOOL g_burn_mark;
/*!
  ota filter download start flag
  */
   BOOL download_start_flag;
/*!
  ota filter monitor able flag
  */
  BOOL monitor_enable_flag;
/*!
  ota filter monitor timeout
  */
  u32 monitor_timeout;
/*!
  ota filter monitor ticks
  */
  u32 monitor_ticks;
/*!
  ota filter receiver data buffer
  */
  u8 *p_receiver_buffer;
/*!
  input ping cfg
  */
  ota_ipin_cfg_t ipin_cfg;
/*!
  ota filter debug printf
  */
  ota_debug_level_t debug_level;
/*!
  ota filter debug printf
  */
 ota_printf_t ota_filter_debug_printf;
/*!
  ota do lock,is_lock TRUE:lock ,FALSE:unlock;
  */
  void (*ota_do_lock)(BOOL is_lock,u8 lock_mode,u32 *param);
 /*!
  ota version check;
  */
  ota_version_check_t ota_version_check;
 /*!
  ota dsi info check;
  */
  ota_dsi_info_check_t ota_dsi_info_check;
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
  ota check time out;
  */
    BOOL g_check_timeout;
}ota_filter_priv_t;


/*!
 epg filter 
 */
typedef struct tag_ota_filter
{
#ifndef OTA_FILTER_USE_FLASH_FILTER
/*!
   base class
   */  
 FATHER sink_filter_t m_filter;
#else
/*!
   base class
   */  
 FATHER transf_filter_t m_filter;
 #endif
/*!
 epg fitler private data. 
 */
 ota_filter_priv_t    m_priv_data;
}ota_filter_t;

#endif//End for __EPG_FILTER_INTRA_H_
