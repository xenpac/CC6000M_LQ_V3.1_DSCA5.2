/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SCAN_I_H_
#define __AP_SCAN_I_H_
/*!
  Max satellite number supported
  */
#define MAX_SAT_NUM_SUPPORTED (64)

/*!
  Max satellite number supported
  */
#define MAX_TP_NUM_SUPPORTED (3000)
/*!
  Max satellite name supported
  */
#define MAX_SAT_NAME_LENGTH (16)
/*!
  Max satellite name supported
  */
#define MAX_BOUQUET_ID_IN_SCAN (64)
/*!
  Max prog number define in sdt other
  */
#define MAX_BROG_NUM_IN_SDT_OTR (512)

/*!
  Program channel type definition
  */
typedef enum
{
  /*!
    Both tv and radio will be stored into flash
    */
  CHAN_ALL = 0,
  /*!
    Only TV stored into flash
    */
  CHAN_TV,
  /*!
    Only radio stored into flash
    */
  CHAN_RADIO
}chan_type_t;

/*!
  scan mode
  */
typedef enum
{
  /*!
    scan a transponder by given TP id
    */
  TP_SCAN = 0,
  /*!
    blind scan
    */
  BLIND_SCAN,
  /*!
    dtmb scan
    */
  DTMB_SCAN,
  /*!
    dvbc scan
    */
  DVBC_SCAN,    
  /*!
    dvbt scan
    */
  DVBT_SCAN,    
}scan_mode_t;

/*!
  nit scan mode
  */
typedef enum
{
  /*!
    invalid mode
    */
  DVB_INVALID_MODE,
  /*!
    dtmb mode
    */
  DTMB_MODE,
  /*!
    dvbc mode
    */
  DVBC_MODE,    
  /*!
    dvbt mode
    */
  DVBT_MODE,    
  /*!
    dvbs mode
    */
  DVBS_MODE,    
}dvb_mode_t;

/*!
  nit scan mode
  */
typedef enum
{
  /*!
    Scan without request nit
    */
  NIT_SCAN_WITHOUT = 0,
  /*!
    request nit on all tp
    */
  NIT_SCAN_ALL_TP,
  /*!
    request nit once
    */
  NIT_SCAN_ONCE
}nit_scan_type_t;
/*!
  bat scan mode
  */
typedef enum
{
  /*!
    Scan without request bat
    */
  BAT_SCAN_WITHOUT = 0,
  /*!
    request bat on all tp
    */
  BAT_SCAN_ALL_TP,
  /*!
    request bat once
    */
  BAT_SCAN_ONCE,
} bat_scan_type_t;

/*!
  cat scan mode
  */
typedef enum
{
  /*!
    Scan without request cat
    */
  CAT_SCAN_WITHOUT = 0,
  /*!
    request cat on all tp
    */
  CAT_SCAN_ALL_TP,
} cat_scan_type_t;

/*!
  tp origin
  */
typedef enum
{
  /*!
    the tp info from preset data (user input)
    */
  TP_FROM_PRESET = 0,
  /*!
    the tp info was found from tuner scan
    */
  TP_FROM_SCAN,
  /*!
    the tp info get by parse NIT
    */
  TP_FROM_NIT
}tp_origin_t;

/*!
  Blind scan satellite information
  */
typedef struct
{
  /*!
    sat id
    */
  u32 sat_id;
  /*!
    tuner id
    */
  u32 tuner_id;  
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
  u16 name[MAX_SAT_NAME_LENGTH];
} scan_sat_info_t;

/*!
  tp structure
  */
typedef struct
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
    0: tp unlock, 1: locked
    */
  u32 can_locked  : 1;
  /*!
    tp origin
    */
  u32 tp_origin    : 5;
  /*!
    enable nit
    */
  u32 enable_nit   : 1;
  /*!
    enable lcn
    */
  u32 enable_lcn   : 1;
  /*!
    reserved
    */
  u32 pg_num    : 18;
  /*!
    cur tp emm info num
    */
  u32 emm_num :6;
  /*!
    The channel performance structure
    */
  nim_channel_perf_t perf;
  /*!
    emm info des
    */
  ca_desc_t ca_desc[MAX_EMM_DESC_NUM];
  /*!
    Program list
    */
  scan_pg_info_t *p_pg_list;
} scan_tp_info_i_t;

/*!
  scan_preset_data_t
  */
typedef struct
{
  /*!
    NIT scan type
    */
  nit_scan_type_t nit_scan_type;
  /*!
    BAT scan type
    */
  bat_scan_type_t bat_scan_type;
  /*!
    CAT scan type
    */
  cat_scan_type_t cat_scan_type;
  /*!
    scramble scan type
    */
  scan_scramble_origin_t scramble_scan_origin;
  /*!
    Satellite total num
    */
  u16 total_sat;
  /*!
    Tp total num
    */
  u16 total_tp;
  /*!
    Sat list for saving tp information in TP scan
    */
  scan_sat_info_t sat_list[MAX_SAT_NUM_SUPPORTED];
  /*!
    Tp list for saving tp information in TP scan
    */
  scan_tp_info_i_t tp_list[MAX_TP_NUM_SUPPORTED];
  /*!
    bat bouqust id num
    */
  u8 bouquet_num;
  /*!
    reserved1
    */
  u8 reserved1;
  /*!
    reserved2
    */
  u16 reserved2;
  /*!
    list for bat bouqust id
    */
  u16 bouquet_id[MAX_BOUQUET_ID_IN_SCAN];
}scan_preset_data_t;

/*!
  PID scan parameter used in pid scan operation
  This can also be taken as filter conditions. 
  In application scan level, tp scan and pid should be the same
  */
typedef struct
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
}pid_parm_t;

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
  scan_mode_t scan_mode;
  /*!
    Channel type
    */
  chan_type_t chan_type;
  /*!
    PID scan enbabled
    */
  BOOL pid_scan_enable;
  /*!
    PID param
    */
  pid_parm_t pid_parm;
  /*!
    scan_perset_data_t
    */
  scan_preset_data_t scan_data;
}scan_input_param_t;


/*!
  Implement hooks
  */
typedef struct
{
  /*!
    Check the tp need to do lock
    */
  RET_CODE (*on_new_sat)(scan_sat_info_t *p_sat_info);
  /*!
    Disq switching operation depends on the disq type
    */
  u32 (*get_switch_disq_time)(scan_sat_info_t *p_sat_info);
  /*!
    Check the tp need to do lock
    */
  RET_CODE (*check_tp)(nc_channel_info_t *p_channel);
  /*!
    Process tp information when one tp is done
    \param[in] point to tp info
    */
  RET_CODE (*process_tp)(scan_tp_info_i_t *p_tp);
  /*!
    Process tp information when one tp is done
    \param[in] p_tp_list
    */
  RET_CODE (*process_pg_list)(scan_tp_info_i_t *p_tp);
  /*!
    use to free resouece
    */
  RET_CODE (*free_resource)(void);
  /*!
    using attach buffer, if used return the buffer addr
    */
  void * (*get_attach_buffer)(u32 size);
  /*!
    scan module performance track
    */
  void (*performance_hook)(u32 check, u32 context1, u32 context2);
  /*!
    see _process_nit_info
  */
  void (*process_nit_info)(void *p_nit);
  /*!
    use to free resouece
    */
  RET_CODE (*on_start)(void);
  /*!
    Process tp cat information when one tp cat found
    \param[in] point to cat table info
    */
  RET_CODE (*process_cat)(void *p_cat);
      /*!
   get pg sid from sdt flag
    */
  BOOL (*is_no_filter)(void );
}scan_hook_t;

/*!
  Construct tp scan ploicy
  \param[in] tp scan implementation
  \return scan policy
  */
policy_handle_t construct_tp_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data);

/*!
  Construct tp scan ploicy
  \param[in] tp scan implementation
  \return scan policy
  */
policy_handle_t construct_dtmb_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data);

/*!
  Construct tp scan ploicy
  \param[in] tp scan implementation
  \return scan policy
  */
policy_handle_t construct_dvbc_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data);

/*!
  Construct tp scan ploicy for warriors
  \param[in] tp scan implementation
  \return scan policy
  */
policy_handle_t construct_dvbc_scan_policy_warriors(scan_hook_t *p_hook,
  scan_preset_data_t *p_data);

/*!
  Construct tp scan ploicy
  \param[in] tp scan implementation
  \return scan policy
  */
policy_handle_t construct_bl_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data);

/*!
  Construct blind scan ploicy
  \param[in] tp scan implementation
  \return scan policy
  */
policy_handle_t construct_bl_scan_policy_v2(scan_hook_t *p_hook,
  scan_preset_data_t *p_data);

/*!
  Construct dvbt scan ploicy
  \param[in] dvbt tp scan implementation
  \return scan policy
  */
policy_handle_t construct_dvbt_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data);

/*!
  load dvbt tp list
  */
//void dvbt_load_sat_info_for_auto_scan(u8 u8Country, scan_preset_data_t *p_pst);
void dvbt_load_sat_info_for_auto_scan(u8 u8Country, scan_preset_data_t *p_pst, nim_type_t nim_type);
/*!
  load specified tp for nit tp scan
  */
void dvbt_load_sat_info_for_tp_scan(tp_rcv_para_t *p_tp_info, scan_preset_data_t *p_pst);

/*!
  Construct tp scan ploicy
  \param[in] use attach block or not
  \param[in] block id
  */
void ap_scan_set_attach_block(BOOL use_attach_block, u32 block_id);

/*!
  Construct tp scan ploicy
  \return scan param buffer
  */
scan_input_param_t *ap_scan_param_buffer(void);

/*!
  Push param in
  */
policy_handle_t ap_scan_param_apply(void);


#endif  //__AP_SCAN_I_H_

