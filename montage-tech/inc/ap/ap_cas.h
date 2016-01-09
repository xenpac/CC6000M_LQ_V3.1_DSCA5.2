/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_CAS_H_
#define __AP_CAS_H_

/*!
  max cas slot number.
  */
#define MAX_CAS_SLOT_NUM 2

/*!
  make request context.
  */
#define MAKE_REQUEST_CONTEXT(id, data1, data2) ((u32)(((id & 0xFFFF) << 16) \
                                  | ((data1 & 0xFF) << 8) | ((data2 & 0xFF)))) 
/*!
  get request id by request context
  */
#define GET_REQUEST_ID(context) ((u16)((context >> 16) & 0xFFFF)) 
/*!
  get request data1 by request context
  */
#define GET_REQUEST_DATA1(context) ((u16)((context & 0xFF00) >> 8))
/*!
  get request data2 by request context
  */
#define GET_REQUEST_DATA2(context) ((u16)(context & 0xFF))
/*!
  get request data by request context
  */
#define GET_REQUEST_DATA(context) ((u16)(context & 0xFFFF))

/*!
  Delcare ca performance test step
  */
typedef enum
{
  /*!
    UI-->Send req
    */
  CAS_PLAY_REQ_STEP,
  /*!
    CAS get pmt data
    */
  CAS_PMT_DATA_STEP,
  /*!
    CAS get cat data
    */
  CAS_CAT_DATA_STEP,
  /*!
    CAS get NIT data
    */
  CAS_NIT_DATA_STEP,
  /*!
    CAS get descrambler key
    */
  CAS_DESCRAMBLER_KEY_STEP,
}cas_step_t;

/*!
  Delcare tuner lock mode
  */
typedef enum
{
  /*!
    dvb-t
    */
  DVB_T,
  /*!
    dvb-c
    */
  DVB_C,
  /*!
    dvb-s
    */
  DVB_S,
}cas_lock_mode_t;

/*!
  Delcare all command for cas
  */
typedef enum
{
  /*!
    Start cas, with one parameter @see scan_parameter
    */
  CAS_CMD_START = ASYNC_CMD,
  /*!
    comments
    */
  CAS_CMD_OPERATOR_INFO_GET,
  /*!
    comments
    */
  CAS_CMD_CARD_BALANCE_GET,
  /*!
    comments
    */
  CAS_CMD_VIDEO_PID_SET,
  /*!
    comments
    */
  CAS_CMD_AUDIO_PID_SET,
  /*!
    comments
    */
  CAS_CMD_ENTITLE_INFO_GET,
  /*!
    comments
    */
  CAS_CMD_IPPV_INFO_GET,
  /*!
    comments
    */
  CAS_CMD_IPP_REC_INFO_GET,
  /*!
    comments
    */
  CAS_CMD_BURSE_INFO_GET,
  /*!
    comments
    */
  CAS_CMD_MAIL_HEADER_GET,
  /*!
    comments
    */
  CAS_CMD_MAIL_BODY_GET,
  /*!
    comments
    */
  CAS_CMD_MAIL_DEL_ONE,
  /*!
    comments
    */
  CAS_CMD_MAIL_DEL_ALL,
  /*!
    change mail status
    */
  CAS_CMD_MAIL_CHANGE_STATUS,
  /*!
    comments
    */
  CAS_CMD_ANNOUNCE_DEL_ONE,
  /*!
    comments
    */
  CAS_CMD_ANNOUNCE_DEL_ALL,
  /*!
    comments
    */
  CAS_CMD_ANNOUNCE_HEADER_GET,
  /*!
    comments
    */
  CAS_CMD_ANNOUNCE_BODY_GET,
  /*!
    comments
    */
  CAS_CMD_CARD_INFO_GET,
  /*!
    comments
    */
  CAS_CMD_PLATFORM_ID_GET,
  /*!
    comments
    */
  CAS_CMD_MON_CHILD_FEED,
  /*!
    comments
    */
  CAS_CMD_MON_CHILD_STATUS_GET,
  /*!
    comments
    */
  CAS_CMD_PAIRE_STATUS_GET,
  /*!
    can not write flash
  */
  CAS_CMD_REQUEST_MASK_BUFFER,
  /*!
    can write flash
  */
  CAS_CMD_REQUEST_UPDATE_BUFFER,
  /*!
    comments
    */
  CAS_CMD_IPP_BUY_SET,
  /*!
    comments
    */
  CAS_CMD_RATING_GET,
  /*!
    comments
    */
  CAS_CMD_WORK_TIME_GET,
  /*!
    STOP cas
    */
  CAS_CMD_STOP,
  /*!
    STOP all cas process(ecm and emm..)
    */
  CAS_CMD_STOP_CAS,
  /*!
    Set service id ansync cmd
    */
  CAS_CMD_SID_SET_ANSYNC,
   /*!
    Set play info by ansync cmd
    */
  CAS_CMD_PLAY_INFO_SET_ANSYNC,
   /*!
    Set parental unlock pin
    */
  CAS_CMD_UNLOCK_PIN_SET,
   /*!
    Get ipp buy info
    */
  CAS_CMD_GET_IPP_BUY_INFO,
   /*!
    cas factory set
    */
  CAS_CMD_FACTORY_SET,
   /*!
    cas zone check
    */
  CAS_CMD_ZONE_CHECK,
  /*!
    apply for watching higher rating program
    */
  CAS_CMD_CERTIFICATE_CONFIRM_SERVICE,
  /*!
    get PIN state, enabled/disabled
    */
  CAS_CMD_PIN_STATE_GET,
  /*!
    enable/disable PIN
    */
  CAS_CMD_PIN_ENABLE,
  /*!
    set overdule info service
    */
  CAS_CMD_OVERDUE_INFO_SERVICE_SET,
  /*!
    get overdule info service info
    */
  CAS_CMD_OVERDUE_INFO_SERVICE_GET,
  /*!
    get emergency broadcasting info
    */  
  CAS_CMD_GET_EMERGENCY_BROADCASTING_INFO,  
  /*!
    get cas library version
    */
  CAS_CMD_LIB_VERSION_GET,
     /*!
    cas zone set for smsx
    */
  CAS_CMD_SET_ZONE_CODE,
    /*!
    OSD msg  rolling over
    */
  CAS_CMD_OSD_ROLL_OVER,
    /*!
    set ca language
    */
  CAS_CMD_SET_CA_LANGUAGE,
    /*!
    set ca force mail
    */
  CAS_CMD_CA_GET_FORCE_EMAIL,
  /*!
    for sumashixun ca osd show time
    */
  CAS_CMD_CA_OSD_SHOW_TIME,
    /*!
    for cdxg get osd info when change pg
    */
  CAS_CMD_GET_OSD_INFO,
  /*!
  project define cas async cmd start
  */
  CAS_CMD_PRJ_DEFINE_ASYNC_START,
  /*!
  ABVCA:new mail check
  */
  CAS_CMD_NEW_MAIL_CHECK,
/*!
  cas cmd async end
  */
  CAS_CMD_ASYNC_END = CAS_CMD_PRJ_DEFINE_ASYNC_START + 50,
  /*!
    Set service id
    */
  CAS_CMD_SID_SET = SYNC_CMD,
  /*!
    comments
    */
  CAS_CMD_IPPV_INFO_SYNC_GET,
  /*!
    Set play info
    */
  CAS_CMD_PLAY_INFO_SET,
  /*!
    comments
    */
  CAS_CMD_PIN_SET,
  /*!
    Get ACLIST info
  */
  CAS_CMD_ACLIST_INFO_GET,
   /*!
    comments
    */
  CAS_CMD_READ_FEED_DATA,
  /*!
    comments
    */
  CAS_CMD_WORK_TIME_SET,
  /*!
    comments
    */
  CAS_CMD_RATING_SET,
  /*!
  comments
  */
  CAS_CMD_PIN_VERIFY,
  /*!
    open ca debug
    */
  CAS_CMD_CA_DEBUG,
  /*!
    open ca overdue for yingji
    */
  CAS_CMD_CA_OVERDUE,
  /*!
    card match
    */
  CAS_CMD_CA_JUDGE_CARD_TYPE,
  /*!
    Do stop by sync
    */
  CAS_CMD_STOP_SYNC,
  /*!
    STOP all cas process(ecm and emm..)
    */
  CAS_CMD_STOP_CAS_SYNC,
  /*!
  project define cas snyc cmd start
  */
  CAS_CMD_PRJ_DEFINE_SYNC_START,
/*!
  cas cmd sync end
  */
  CAS_CMD_SYNC_END = CAS_CMD_PRJ_DEFINE_SYNC_START + 50,

} cas_cmd_t;

/*!
  Delcare event for cas
  */
typedef enum
{
  /*!
    the first event of CAS module,
    if create new event, you must insert it between BEGIN and END
    */
  CAS_EVT_BEGIN = ((APP_CA << 16) | ASYNC_EVT),
  /*!
    email Space Exhaust
    */
  CAS_EVT_SHOW_SPACE_EXHAUST,
  /*!
    NEW Email
    */
  CAS_EVT_SHOW_NEW_EMAIL,
  /*!
    Hide email icon
    */
  CAS_EVT_HIDE_NEW_EMAIL,
  /*!
    Show finger
    */
  CAS_EVT_SHOW_FINGER,
  /*!
    Hide finger
    */
  CAS_EVT_HIDE_FINGER,
  /*!
    cancel senior preview
    */
  CAS_EVT_SENIOR_PREVIEW_CANCLE,  
  /*!
    cancel senior preview
    */
  CAS_EVT_SENIOR_PREVIEW_SHOW,   
  /*!
    show osd message up
    */
  CAS_EVT_SHOW_OSD_UP,
  /*!
    show osd message down
    */
  CAS_EVT_SHOW_OSD_DOWN,
  /*!
    show osd message down
    */
  CAS_EVT_SHOW_OSD_MSG,
  /*!
    hide osd message down
    */
  CAS_EVT_HIDE_OSD,
  /*!
    hide  force osd message
    */
  CAS_EVT_HIDE_FORCE_OSD,  
  /*!
    show burse charge info
    */
  CAS_EVT_SHOW_BURSE_CHARGE,
  /*!
    show error pin code info
    */
  CAS_EVT_SHOW_ERROR_PIN_CODE,
  /*!
    show authen will expire day
    */
  CAS_EVT_SHOW_AUTHEN_EXPIRE_DAY,
  /*!
    hide osd message up
    */
  CAS_EVT_HIDE_OSD_UP,
  /*!
    hide osd message down
    */
  CAS_EVT_HIDE_OSD_DOWN,
  /*!
    Show ipp buy infomation
    */
  CAS_EVT_SHOW_IPP_BUY_INFO,
  /*!
    force change channel infomation
    */
  CAS_EVT_FORCE_CHANNEL_INFO,
  /*!
    unforce change channel infomation
    */
  CAS_EVT_UNFORCE_CHANNEL_INFO,
  /*!
    card reset success
    */
  CAS_EVT_RST_SUCCESS,
  /*!
    cas notify ui display string
    */
  CAS_EVT_NOTIFY,
  /*!
    operator info get
    */
  CAS_EVT_OPERATOR_INFO,
  /*!
    test
    */
  CAS_EVT_TEST,
  /*!
    operator info get
    */
  CAS_EVT_ENTITLE_INFO,
  /*!
    operator info get
    */
  CAS_EVT_IPPV_INFO,
  /*!
    IPP consume record info get
    */
  CAS_EVT_IPP_REC_INFO,
  /*!
    operator info get
    */
  CAS_EVT_BURSE_INFO,
  /*!
    mail header info get
    */
  CAS_EVT_MAIL_HEADER_INFO,
  /*!
    mail body info get
    */
  CAS_EVT_MAIL_BODY_INFO,
  /*!
    announce header info get
    */
  CAS_EVT_ANNOUNCE_HEADER_INFO,
  /*!
    announce body info get
    */
  CAS_EVT_ANNOUNCE_BODY_INFO,
  /*!
    Email del result
    */
  CAS_EVT_MAIL_DEL_RESULT,
  /*!
    get mail status change result
    */
  CAS_EVT_MAIL_CHANGE_STATUS,
  /*!
    announce del result
    */
  CAS_EVT_ANNOUNCE_DEL_RESULT,
  /*!
    card info get
    */
  CAS_EVT_CARD_INFO,
  /*!
    get plat form id
    */
  CAS_EVT_PLATFORM_ID,
  /*!
    rating info get
    */
  CAS_EVT_RATING_INFO,
  /*!
    work time info get
    */
  CAS_EVT_WORK_TIME_INFO,
  /*!
    ipp buy result
    */
  CAS_EVT_IPP_BUY_RESULT,
  /*!
    monther--child--card info get
    */
  CAS_EVT_MON_CHILD_INFO,
  /*!
    monther--child--card info set
    */
  CAS_EVT_MON_CHILD_FEED,
  /*!
    paired statuse get
    */
  CAS_EVT_PAIRED_STA,
  /*!
    unlock parental pin result
    */
  CAS_EVT_UNLOCK_PARENTAL_RESULT,
  /*!
    ppv buy infomation
    */
  CAS_EVT_PPV_BUY_INFO,
    /*!
    hide ppv buy infomation
    */   
  CAS_EVT_HIDE_PPV_BUY_INFO,
    /*!
    CAS IPP real time buy
    */
  CAS_EVT_IPP_PROG_NOTIFY,
    /*!
    The card info need update
    */  
  CAS_EVT_CARD_NEED_UPDATE,
    /*!
    The card info update begin
    */  
  CAS_EVT_CARD_UPDATE_BEGIN,
   /*!
    The card info update in progress
    */   
  CAS_EVT_CARD_UPDATE_PROGRESS,
  /*!
   show ecm finger
   */ 
  CAS_EVT_SHOW_ECM_FINGER,
  /*!
   watch limit
   */ 
  CAS_EVT_CONTINUE_WATCH_LIMIT,
  /*!
   stb notification
   */ 
  CAS_EVT_STB_NOTIFICATION, 
    /*!
   software update progress
   */ 
  CAS_EVT_SOFTWARE_UPDATE_PROGRESS,
   /*!
    The card info update error
    */  
  CAS_EVT_CARD_UPDATE_ERR,
    /*!
    The card info update error
    */   
  CAS_EVT_CARD_UPDATE_END,
      /*!
    stop current program
    */  
  CAS_EVT_IEXIGENT_PROG_STOP,
  /*!
    The card has not been found
    */
  CAS_ALL,
  /*!
    CAS sv force msg/important email
    */
  CAS_EVT_FORCE_MSG,
  /*!
    CAS sv hide force msg/important email
    */
  CAS_EVT_HIDE_FORCE_MSG,
  /*!
    certificate confirm service
    */
  CAS_EVT_CERTIFICATE_CONFIRM_SERVICE,
  /*!
    PIN state
    */
  CAS_EVT_PIN_STATE_INFO,
  /*!
    enable/disable PIN
    */
  CAS_EVT_PIN_ENABLE,
  /*!
    set overdue info service
    */
  CAS_EVT_OVERDUE_INFO_SERVICE_SET,
  /*!
    get overdue info service info
    */  
  CAS_EVT_OVERDUE_INFO_SERVICE_INFO,
  /*!
    confirm film rating
    */
  CAS_EVT_CONFIRM_FILM_RATING,
  /*!
    NIT condition search
    */  
  CAS_CONDITION_SEARCH,
  /*!
    get lib version
    */
  CAS_EVT_LIB_VERSION,
/*!
  project define cas async evt start
  */
  CAS_EVT_PRJ_DEFINE_ASYNC_START,
/*!
  cas event async end
  */
  CAS_EVT_ASYNC_END = CAS_EVT_PRJ_DEFINE_ASYNC_START + 50,
  /*!
    Set service id
    */
  CAS_SET_SID = ((APP_CA << 16) | SYNC_EVT),
  /*!
    Ipp info ack
    */
  CAS_EVT_PPV_SYNC_INFO,
  /*!
    card pin set
    */
  CAS_EVT_PIN_SET,
  /*!
    card work time set
    */
  CAS_EVT_WORK_TIME_SET,
  /*!
    card rating set
    */
  CAS_EVT_RATING_SET,
  /*!
  card pin set
  */
  CAS_EVT_PIN_VERIFY, 
  /*!
    CAS stop
    */
  CAS_EVT_STOPED,
  /*!
    Stop all cas process event
    */
  CAS_EVT_STOPED_CAS, 
  /*!
    play info set event
    */
  CAS_EVT_PLAY_INFO_SET,

  /*!
    CAS debug open/clse
    */
  CAS_EVT_DEBUG_EN,
  /*!
    CAS show overdue,yingji
    */
  CAS_EVT_OVERDUE,
  /*!
    CAS modify card match,sv
    */
  CAS_EVT_MON_CHILD_IDENTIFY,
  /*!
    ACLIST infomation
    */
  CAS_EVT_ACLIST_INFO,
   /*!
    read feed data from mother card
    */
  CAS_EVT_READ_FEED_DATA,
   /*!
  read feed data from mother card
    */
  CAS_EVT_FEED_CARD_DLG,
/*!
  project define cas snyc evt start
  */
  CAS_EVT_PRJ_DEFINE_SYNC_START,
/*!
  cas event sync end
  */
  CAS_EVT_SYNC_END = CAS_EVT_PRJ_DEFINE_SYNC_START + 50,
  /*!
    the last event of CAS module,
    if create new event, you must insert it between BEGIN and END
    */
  CAS_EVT_END
} cas_evt_t;

/*!
  structure for set sid when change channel.
  */
typedef struct
{
  /*!
    sid number.
    */
  u16 num;
  /*!
    current pgid
    */
  u16 pgid;
  /*!
    sid list.
    */
  u16 pg_sid_list[MAX_CAS_SLOT_NUM];
  /*!
    nim control channel information
    */
  u32 tp_freq;
  /*!
    video pid
    */
  u16 v_pid;
    /*!
    audio pid
    */
  u16 a_pid;
    /*!
    pmt pid
    */
  u16 pmt_pid;
    /*!
    org_network_id
    */
  u16 org_network_id;
    /*!
    ts_id
    */
  u16 ts_id;        
    /*!
    ca system pid
    */
  u16 ca_sys_id;
  /*!
    ecm num
    */
  u8 ecm_num;
  /*!
    emm num
    */
  u8 emm_num;
  /*!
    0:dvb-c; 1:dvb-t; 2:dvb-s
    */
  u16 lock_mode;
  /*!
    nim control channel information
    */
  nc_channel_info_t nc_search_info;
  /*!
    emm information
    */
  ca_desc_t emm_info[MAX_EMM_DESC_NUM];
  /*!
    ecm information
    */
  cas_desc_t ecm_info[MAX_ECM_DESC_NUM];
}cas_sid_t;

/*!
  CAS event struct
  */
typedef struct
{
/*!
  cas slot id
  */
  u8 slot_id;
/*!
  cas module id
  */
  u8 cam_id;
/*!
  cas event
  */
  u16 event;
/*!
  cas param
  */
  u32 param;
}ap_cas_evt_t;

/*!
  CAS policy
  */
typedef struct
{
  /*!
    on initialized
    */
  void (*on_init)(void);
  /*!
    initializ some ca modul
    */
  void (*init_ca_module)(cas_module_id_t cam_id);
  /*!
    for test ca performance
    */
  void (*test_ca_performance)(cas_step_t step, u32 ticks);
  /*!
    for update pmt ecm info
    */
  BOOL (*update_ecm_info)(u8 *p_pmt_data, cas_desc_t *p_ecm_info, u16 *p_max);
  /*!
    config ca data mem size
    */
  void (*config_ca_data_mem)(u8 **p_addr, u32 *p_size);
/*!
  project do event process from ca ware 
  */
  void (*do_ca_event_by_onself)(ap_cas_evt_t *p_event);
/*!
  project do command process from ca ware 
  */
  void (*do_ca_command_by_onself)(os_msg_t *p_msg);
  /*!
    config msgq timeout for ap_ca task
    */
  u32 (*get_msgq_timeout)();
} cas_policy_t;

/*!
  Singleton patern interface which can get a CAS application's
  instance

  \param[in] p_policy policy
  \return instance address
  */
app_t *construct_ap_cas(cas_policy_t *p_policy);

#endif // End for __AP_CAS_H_


