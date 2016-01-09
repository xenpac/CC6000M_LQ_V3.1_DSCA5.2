/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_FRAMEWORK_H_
#define __AP_FRAMEWORK_H_

/*!
  All Application's message queue timeout value, AP's task will be pending on
  message queue if the queue is empty. This value can't be less than 10
  */
#define DEFAULT_MSGQ_TIMEOUT (100)

/*!
  system event came from server, ap framework send it to ui with this tag.
  */
#define SYS_EVT_TO_UI_MSG_MASK (0x02000000)

/*!
  Default sync command, all sync command in application should start from this
  value
  */
#define SYNC_CMD 0x1000  

/*!
  Default async command, all async command in application should start from 
  this value
  */
#define ASYNC_CMD 0  

/*!
  Default sync event, all sync event in application should start from this 
  value
  */
#define SYNC_EVT SYNC_CMD  

/*!
  Default async event, all async event in application should start from this 
  value
  */
#define ASYNC_EVT ASYNC_CMD 

/*!
  All support applications' ID

  These values must not be changed!!
  */
typedef enum
{
  /*!
    Reserve zero for framework itself
    */
  APP_FRAMEWORK = 100,
  /*!
    User input/output application id
    */
  APP_UIO = 101,
  /*!
    TV/Radio playback application id
    */
  APP_PLAYBACK = 102,
  /*!
    Program scan application id
    */
  APP_SCAN = 103,
  /*!
    Nie-picture application id
    */
  APP_MULTI_PIC = 104,
  /*!
    Singal monitor application id
    */    
  APP_SIGNAL_MONITOR = 105,
  /*!
    si monitor application id
    */
  APP_SI_MON = 106,
  /*!
    OTA upgrade application id
    */
  APP_OTA = 107,    
  /*!
    Information service application id
    */
  APP_IS = 108,
  /*!
    EPG application id
    */
  APP_EPG = 109,
  /*!
    Upgrade application id
    */
  APP_UPG = 110,
  /*!
    SUBTITLE application id, for debug only
    */
  APP_SUBTITLE = 111,
  /*!
  	Satcodx application id
    */
  APP_SATCODX = 112,
  /*!
  	temp solution
    */
  APP_TIME = 113,
  /*!
    ca application
    */
  APP_CA = 114,
  /*!
    Data process application id
    */
  APP_DATA_PROCESS = 115, 
  /*!
    twin port application id
    */
  APP_TWIN_PORT = 116,
  /*!
    ota for ts
    */
  APP_TS_OTA = 117,  
  /*!
    music player
    */
  APP_MUSIC_PLAYER = 118,
  /*!
    ts file player
    */
  APP_TS_PLAYER = 119,
  /*!
    jpeg player
    */
  APP_PICTURE = 120,
  /*!
    recording
    */
  APP_RECORDER = 121,
  /*!
    recording
    */
  APP_TIMESHIFT = 122,
  /*!
    USB_UPGRADE
    */
  APP_USB_UPG = 123,
  /*!
    NVOD
    */
  APP_NVOD = 124,
  /*!
    ap ad
    */
  APP_AD = APP_NVOD,
  /*!
    ap ttx
    */
  APP_TTX = 126,
  /*!
    ap tkgs
    */
  APP_TKGS = 127,  
  /*!
    recording
    */
  APP_FILEPLAY = 128,
  /*!
    vod
    */
  APP_VOD = 129,
  /*!
    album
    */
  APP_ALBUM = 130,    
  /*!
    gbox
  */
  APP_GBOX = 131,
  /*!
    news
    */
  APP_NEWS = 132,    
  /*!
   auto diseqc
   */
  APP_AUTODISEQC = 133,
   /*!
 net music
 */
  APP_NET_MUSIC_METADATA = 134,
  /*!
  music api
 */
  APP_NET_MUSIC_PLAY_CTRL = 135,
  /*!
  multicas
 */
  APP_FCAS = 136,
   /*!
  music api
 */
  APP_RC = 137,
 /*!
    news
 */
  APP_CLUSTER_MACHINE = 138,   
  /*!
    cy application
    */
  APP_CY = 139,
      /*
 get NIT
  */
  APP_NIT = 140,
  
  /*!
    reserved
    */
  APP_RESERVED1,
  /*!
    reserved
    */
  APP_RESERVED2, 
  /*!
    project define app id start
    */
  APP_PROJECT_DEFINE_START,
  /*!
    Last application id(reserved)
    */
  APP_LAST = APP_PROJECT_DEFINE_START + 20,
} app_id_t;

/*!
  Application framework events
  */
typedef enum
{
  /*!
    Begin event, this event will not be sent to ui
    */
  AP_FRM_EVT_BEGIN = (APP_FRAMEWORK << 16),
  /*!
    This event occurs after restoring to factory
    */
  AP_FRM_EVT_RESTORE_TO_FACTORY,
  /*!
    This event occurs after test uart
    */
  AP_FRM_EVT_TEST_UART,
  /*!
    End event, this event will not be sent to ui
    */
  AP_FRM_EVT_END
} ap_frm_evt_t;

/*!
  Application framework command id list
  */
typedef enum
{
  /*!
    Activate app command
    */
  AP_FRM_CMD_ACTIVATE_APP = 0,
  /*!
    Deactive app command
    */
  AP_FRM_CMD_DEACTIVATE_APP,
  /*!
    Pause app command
    */
  AP_FRM_CMD_PAUSE_APP,
  /*!
    Resume app command
    */
  AP_FRM_CMD_RESUME_APP,
  /*!
    Restore to factory command, this command will be implemented in
    project
    */
  AP_FRM_CMD_RESTORE_TO_FACTORY,
  /*!
    Standby command, this command will be implemented in project
    and the parameter 1 specifies tm_out.
    */
  AP_FRM_CMD_STANDBY,
  /*!
    Test Uart command, this command will be implemented in project
    */
  AP_FRM_CMD_TEST_UART,
  /*!
    Reboot
    */
  AP_FRM_CMD_REBOOT
} ap_frm_cmd_t;

/*!
  print out level
  */
typedef enum
{
  /*!
    undefine the level, default to ERROR level
    */
  AP_FRM_PRINT_UNDEF,
  /*!
    print out all info
    */
  AP_FRM_PRINT_ALL,
  /*!
    print out the major info, eg sync cmd, ack , ap state changed ..
    */
  AP_FRM_PRINT_MAJOR,
  /*!
    print out the error info
    */
  AP_FRM_PRINT_ERROR,
  /*!
    no print
    */
  AP_FRM_PRINT_NONE,
} ap_frm_print_level_t;

/*!
  Event type
  */
typedef struct
{
  /*!
    event id, high 16 bits indicates the event source(app id) and
    low 16 bits start from zero
    */
  u32 id;
  /*!
    event data1
    */
  u32 data1;
  /*!
    event data2
    */
  u32 data2;
} event_t;

/*!
  command type
  */
typedef struct
{
  /*!
    command id start from zero
    */
  u32 id;
  /*!
    event data1
    */
  u32 data1;
  /*!
    event data2
    */
  u32 data2;
} cmd_t;

/*!
  Application interface and all virtual interfaces
  */
typedef struct
{
  /*!
    \see pure_virtual_ap_init
    */
  void (*init)(void *handle);
  /*!
    \see pure_virtual_task_single_step
    */
  void (*task_single_step)(void *handle, os_msg_t *p_msg);
  /*!
    \see process_msg
    */
  void (*process_msg)(void *handle, os_msg_t *p_msg);
  /*!
    \see state_machine
    */
  void (*state_machine)(void *handle);
  /*!
    \see virtual_get_msgq_timeout
    */
  u32 (*get_msgq_timeout)(void *handle);
  /*!
    \see upg_pre_process_msg
    Fix me
    */
  void (*pre_process_msg)(void *handle, os_msg_t *p_msg);

  /*!
    Application private data
    */
  void *p_data;
} app_t;

/*!
  Application information struct
  */
typedef struct
{
  /*!
    Application id
    */
  u16 app_id;
  /*!
    Application instance, one application should have only one global
    instance. This can't be NULL
    */
  app_t *p_instance;
  /*!
    Application name. This can't be NULL
    */
  char *p_name;
  /*!
    Task priority
    */
  u8 priority;
  /*!
    Task stack size
    */
  u32 stack_size;
} app_info_t;

/*!
  Application framework policy interface declaration
  */
typedef struct
{
  /*!
    Detect if last boot status was in standby mode
    */
  BOOL (*is_standby)(void);
  /*!
    Set standby mode, parameter should be TRUE or FALSE
    */
  void (*set_standby)(u32 flag);
  /*!
    Enter standby mode, tm_out specifies how long it will wake up 
    and 0 means never.
    */
  void (*enter_standby)(u32 tm_out);
  /*!
    Give implementation a chance to do some extra stuff, if this function was
    implemented it will be called before application framework receiving its
    message queue. Parameter is application framework's instance
    */
  void (*extand_proc)(void);
  /*!
    Test uart function, no parameter
    */
  void (*test_uart)(void);
  /*!
    Restore to factory function
    */
  void (*resotre_to_factory)(void);
  /*!
    Consume event which will be sent to UI, if the event was consumed by
    ap framework, the event will not go to UI.
    */
  BOOL (*consume_ui_event)(u32 ui_state, os_msg_t *p_msg);
  /*!
    If you need setup mode, you must implement this function
    */
  void (*sys_setup)(void);
  /*!
    If you need setup mode, you must implement this function
    ap_framework will give ap event or server's data by it
    return TRUE means setup finished, or FALSE means waiting something
    */
  BOOL (*sys_setup_loop)(event_t *p_evt);
  /*!
    When UI message queue is full, drop the evt and notify UI
    */
  void (*drop_ui_event)(os_msg_t *p_msg);
} ap_frm_policy_t;

/*!
  Initialize parameter structure
  */
typedef struct
{
  /*!
    Task priority
    */
  u8 priority;
  /*!
    Task stack size
    */
  u32 stack_size;
  /*!
    UI task priority
    */
  u8 ui_priority;
  /*!
    Ap highest task priority
    */
  u8 ap_highest_priority;
  /*!
    Number of applications
    */
  u8 num_of_apps;
  /*!
    Applications id array to indicate which applications will be initialized
    */
  app_info_t *p_app_array;
  /*!
    Implement policy class handle, this can be NULL
    */
  ap_frm_policy_t *p_implement_policy;
  /*!
    ap framwork print level
    */
  ap_frm_print_level_t print_level;
} ap_frm_init_param_t;

/*!
  type def
  */
typedef s32 (*put_uart_byte)(u8 id, u8 chval);

/*!
  Init framework, after calling init, framework task will be suspend and
  waiting to be call run explicitly

  \param[in] p_param Initialize parameter, this can't be NULL.
  \param[out] p_msgq_id Return application framework's message queue id, this
   parameter can't be NULL

  \return return the instance of application framework
  */
handle_t ap_frm_init(const ap_frm_init_param_t *p_param,
    u32 *p_msgq_id);

/*!
  Init framework, after calling init, framework task will be suspend and
  waiting to be call run explicitly
  if you need bundle all of aaps into one task , please use this interface

  \param[in] p_param Initialize parameter, this can't be NULL.
  \param[out] p_msgq_id Return application framework's message queue id, this
   parameter can't be NULL

  \return return the instance of application framework
  */
handle_t ap_frm_master_init(const ap_frm_init_param_t *p_param,
    u32 *p_msgq_id);

/*!
  This function will support to do something before system running
  */
void ap_frm_setup(void);

/*!
  This function will resume application framework task
  */
void ap_frm_run(void);

/*!
  Call application's do command directly and if the application was
  not activated it will assertion failed.
  
  \param[in] handle The instance of application framework
  \param[in] app_id application id
  \param[in] p_cmd command defined by application
  */
void ap_frm_do_command(app_id_t app_id, const cmd_t *p_cmd);

/*!
  Send sync event to UI framework
  
  \param[in] app_id_t application id
  \param[in] event_t Event data defined by application
  */
void ap_frm_send_evt_to_ui(app_id_t app_id, const event_t *p_evt);

/*!
  Set ui framework's message queue id to application framework

  \param[in] handle The instance of application framework
  \param[in] msgq_id UI framework message queue id
  */
void ap_frm_set_ui_msgq_id(const s32 msgq_id);

/*!
  Get ap framework msgq id.

	\return msgq id.
  */
u32 ap_frm_get_msgq_id(void);

/*!
  sometimes, AP need send intral message
  
  \param[in] app_id_t application id
  \param[in] p_msg msg
  */
void ap_frm_send_intral_msg(app_id_t app_id, os_msg_t *p_msg);

/*!
  Set ui framework's message queue id to application framework

  \param[in] handle The instance of application framework
  \param[in] msgq_id UI framework message queue id
  */
void ap_frm_send_notify_to_ui(app_id_t app_id, const event_t *p_evt);

/*!
  Set ui framework's message queue id to application framework

  \param[in] handle The instance of application framework
  \param[in] msgq_id UI framework message queue id
  */
void ap_frm_send_user_key_to_ui(app_id_t app_id, const event_t *p_evt);

/*!
  spend msg
  
  \param[in] opt:  1 open, other close. default to close

  */
void ap_frm_msg_spend(const event_t *p_evt);

/*!
  preformance test function
  
  \param[in] opt:  1 open, other close. default to close

  */
void ap_frm_preformance_test(int opt);

/*!
  reg uart push byte function
  
  \param[in] f:  push function

  */
void ap_frm_reg_push_uart_byte(put_uart_byte f);

/*!
  enter or exist hold state
  in the mode, system can't get notify and user input message
  
  \param[in] b_enter:  TRUE is enter

  */
void ap_frm_sys_enter_hold_state(BOOL b_enter);


#endif
