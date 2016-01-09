/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SCAN_ELI_H_
#define __AP_SCAN_ELI_H_

/*!
  \file ap_scan_eli.h

  Declare scan application and scan.h an base class for all kinds of scan
  process like dvbs scan, dvbs tp scan, dvbs blind scan and considering
  dvbc, dvbt it should support also.
  */


/*!
  Max satellite number supported
  */
#define MAX_SAT_NUM_WHEN_SCAN       (64)

/*!
  Max satellite number supported
  */
#define MAX_TP_NUM_WHEN_SCAN        (3000)


/*!
 scan_eli handle.
 */
typedef void * scan_eli_handle;

/*!
  defines scan eli type
  */
typedef enum tag_scan_eli_mode
{
/*!
  scan eli tp scan mode
  */ 
  SCAN_ELI_TP_SCAN,
/*!
  scan eli blind scan mode
  */   
  SCAN_ELI_BLIND_SCAN,
 /*!
  scan eli dmtb scan mode
  */ 
  SCAN_ELI_DMTB_SCAN,
 /*!
  scan eli dvbc scan mode
  */ 
  SCAN_ELI_DVBC_SCAN,
 /*!
  scan eli end.
  */
  SCAN_ELI_END,
}scan_eli_mode_t;


/*!
  Delcare all command for scan_eli
  */
typedef enum tag_scan_eli_cmd
{
 /*!
  Start scan_eli(with one paramter, para1 save scan_eli_policy_t address)
  */
  SCAN_ELI_CMD_START  = 0, 
 /*!
  Cancel scan_eli
  */
  SCAN_ELI_CMD_CANCEL = SYNC_CMD,
 /*!
  Pause scan_eli
  */
  SCAN_ELI_CMD_PAUSE,
 /*!
  Resume scan_eli
  */ 
  SCAN_ELI_CMD_RESUME,
}scan_eli_cmd_t;

/*!
  Delcare all event for scan_eli
  */
typedef enum tag_scan_eli_event
{
 /*!
  scan eli event porgram fonnd.
  */ 
  SCAN_ELI_EVT_PG_FOUND = (APP_SCAN << 16),
 /*!
  scan eli event tp found.
  */ 
  SCAN_ELI_EVT_TP_FOUND, 
 /*!
  scan eli event progress.
  */ 
  SCAN_ELI_EVT_PROGRESS, 
 /*!
  scan eli event sat switch
  */ 
  SCAN_ELI_EVT_SAT_SWITCH,
 /*!
  scan eli event no memory
  */ 
  SCAN_ELI_EVT_NO_MEMORY,
/*!
  scan eli pause
  */
  SCAN_ELI_EVT_PAUSE = ((APP_SCAN << 16) | SYNC_EVT),
/*!
  scan eli resume
  */
  SCAN_ELI_EVT_RESUME,
/*!
  scan eli finished.
  */
  SCAN_ELI_EVT_FINISHED,
}scan_eli_event_t;


/*!
  defines scan_eli satellite information
  */
typedef struct tag_scan_eli_sat_info
{
/*!
  sat id
  */
u32 sat_id;
/*!
  diseqc info
  */
nc_diseqc_info_t diseqc_info;
/*!
  sat info
  */
sat_rcv_para_t sat_info;
/*!
  Satellite name
  */
u16 name[16];
}scan_eli_sat_info_t;

/*!
 tp origin
 */
typedef enum tag_scan_tp_origin
{
/*!
  the tp info from preset data.(need't save to flash)
  */
 TP_FROM_PRESET,
/*!
  the tp info from nit table.(must save to flash)
  */ 
 TP_FROM_NIT,
/*!
  the tp info from blind scan.(must save to flash)
  */ 
  TP_FROM_SCAN,
}scan_tp_origin_t;

/*!
  defines scan_eli tp information
  */
typedef struct tag_scan_eli_tp_info
{
  /*!
    TP info
    */
  tp_rcv_para_t tp_info;
  /*!
    TP node index in range of 0 - 299
    */
  u32 id          : 16;
  /*!
    Satellite node index 
    */
  u32 sat_id      : 16;
  /*!
    tp origin
    */
  u32 tp_origin   : 3;
  /*!
    0 : tp unlock, 1 : tp locked.
    */
  u32 can_locked  : 1;  
}scan_eli_tp_info_t;


/*!
  scan_eli preset data 
  */
typedef struct scan_eli_preset_data
{
 /*!
  nit type
  */
  scan_nit_type_t       nit_type;
 /*!
  bat type
  */ 
  scan_bat_type_t       bat_type;
 /*!
  cat type
  */
  scan_cat_type_t       cat_type;
 /*!
  scramble type
  */
  scan_scramble_type_t  scramble_type;
 /*!
  satellite total number
  */ 
  u16 total_sat;
 /*!
  Tp total number
  */
  u16 total_tp;
 /*!
  sat list
  */
  scan_eli_sat_info_t sat_list[MAX_SAT_NUM_WHEN_SCAN];
 /*!
  tp list
  */
  scan_eli_tp_info_t tp_list[MAX_TP_NUM_WHEN_SCAN];
 /*!
  bat bouqust id num
  */
  u8 bouquet_num;
 /*!
  list for bat bouqust id
  */
  u16 bouquet_id[MAX_BOUQUET_ID_IN_SCAN];
}scan_eli_preset_data_t;

/*!
  PID scan parameter used in pid scan operation
  This can also be taken as filter conditions. 
  In application scan level, tp scan and pid should be the same
  */
typedef struct tag_scan_pid_param
{
  /*!
    Video pid
    */
  u16 video_pid;
  /*!
    Audio pid
    */
  u16 audio_pid;
  /*!
    PCR pid
    */
  u16 pcr_pid;
}scan_pid_param_t;

/*!
  Program channel type definition
  */
typedef enum
{
  /*!
    Both tv and radio will be stored into flash
    */
  CHANNEL_ALL = 0,
  /*!
    Only TV stored into flash
    */
  CHANNEL_TV,
  /*!
    Only radio stored into flash
    */
  CHANNEL_RADIO
}channel_type_t;

/*!
  process tp return value.
  */
typedef enum tag_process_tp_ret
{
 /*!
  ap scan impl.c process tp return success.
  */
  PROC_TP_SUCCESS, 
 /*!
  ap scan impl.c process tp return repeat tp.
  */
  PROC_TP_REPEAT_TP,
 /*!
  ap scan impl.c process tp return db full.
  */
  PROC_TP_DB_FUL,
 /*!
  ap scan impl.c process tp return fake tp.
  */
  PROC_TP_FAKE_TP,
 /*!
  ap scan impl.c process tp return fail.
  */ 
  PROC_TP_FAIL,
}proc_tp_ret_t;

/*!
  process porgram return value.
  */
typedef enum tag_process_pg_ret
{
 /*!
  ap scan impl.c process program return db full.
  */
  PROC_PROG_DB_FULL,
 /*!
  ap scan impl.c process program return SUCCESS.
  */
  PROC_PROG_SUCCESS,
 /*!
  ap scan impl.c process tp return false.
  */ 
  PROC_PORG_FALSE,
 /*!
  ap scan impl.c process tp return unknown.
  */ 
  PROC_PORG_UNKNOWN,
}proc_pg_ret_t;

/*!
  scan parameters
  */
typedef struct
{
  /*!
    Is free only or not
    */
  BOOL is_free_only;
  /*!
    Scan mode select
    */
  scan_eli_mode_t scan_mode;
  /*!
    Channel type
    */
  channel_type_t chan_type;
  /*!
    PID scan enbabled
    */
  BOOL pid_scan_enable;
  /*!
    PID param
    */
  scan_pid_param_t pid_param;
  /*!
    scan_perset_data_t
    */
  scan_eli_preset_data_t scan_data;
}scan_eli_input_param_t;

/*!
  scan_eli hook
  */
typedef struct tag_scan_eli_hook
{
 /*!
  on start
  */  
  void (*on_start)(void);
 /*!
  get running buffer.
  */  
  void *(*get_running_buffer)(u32 size, char *p_name);
 /*!
  switch one new satellite
  */ 
  RET_CODE (*on_new_sat)(scan_eli_sat_info_t *p_sat_info);
 /*!
  Disq switching operation depends on the disq type
  */
  u32 (*get_switch_disq_time)(scan_eli_sat_info_t *p_sat_info);
 /*!
  Check the tp need to do lock
  */
  proc_tp_ret_t (*check_tp)(nc_channel_info_t *p_channel);
 /*!
  process tp
  */
  proc_tp_ret_t (*process_tp)(scan_eli_tp_info_t *p_tp_info);
 /*!
  process program list
  */ 
  proc_pg_ret_t (*process_pg_list)(u32 pg_num, scan_pg_info_t *p_pg_list);
 /*!
  destory resource.
  */
  RET_CODE (*destory_resource)(void);
 /*!
  process tp cat information when one tp cat found
  */
  RET_CODE (*process_cat)(void *p_cat_buf);
 /*!
  process nit table
  */
  RET_CODE (*process_nit)(nit_eli_t *p_nit);
}scan_eli_hook_t;

/*!
  scan_eli attach block id

  \param[in] use_attach_block : use attach block or not
  \param[in] block_id : memory block id.
  */
void scan_eli_set_attach_block(BOOL use_attach_block, u32 block_id);


/*!
  scan eli param buffer
  \return scan param buffer
  */
scan_eli_input_param_t *scan_eli_param_buffer(void);

/*!
  ap scan param apply.		
  */
scan_eli_handle ap_scan_param_apply(void);

/*!
  get the instance of application tp scan_eli 
  \param[in] p_hook : scan_eli hook
  \param[in] p_data : scan_eli preset data 
  */
scan_eli_handle construct_tp_scan_eli(scan_eli_hook_t *p_hook,
  scan_eli_preset_data_t *p_data);

/*!
  get the instance of application blind scan_eli
  \param[in] p_hook : scan_eli hook
  \param[in] p_data : scan_eli preset data 
  */
scan_eli_handle construct_blind_scan_eli(scan_eli_hook_t *p_hook, 
  scan_eli_preset_data_t *p_data);

/*!
  Get the instance of application scan_eli.
  
  \param[in] chain_stksize : scan_eli chain task stack size.
  */
app_t *construct_ap_scan_eli(u32 chain_stksize);

#endif // End for __AP_SCAN_ELI_H_
