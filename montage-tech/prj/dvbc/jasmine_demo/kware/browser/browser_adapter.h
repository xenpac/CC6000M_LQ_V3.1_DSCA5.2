/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BROWSER_ADAPTER_H__
#define __BROWSER_ADAPTER_H__


/*!
  The event callback structure for BROWSER system module:BROWSER_S_ status, BROWSER_O_ operating
  */
typedef enum
{
/*!
    BRS event begin
    */
  BROWSER_EVT_NONE = 0,
/*!
    BRS search frequency information
    */
  BROWSER_S_FREQ_INFO_FROM_TSID,
/*!
    BRS play program according to service id
    */
  BROWSER_O_PLAY_PROGRAM_FROM_SERVICE_ID,
 /*!
    BRS stop playing program 
    */
  BROWSER_O_STOP_PROGRAM,
   /*!
    Browser exit
    */
  BROWSER_O_EXIT,
  /*!
    set BRS url 
    */
  BROWSER_O_SET_URL,
/*!
    end,
    */
  BROWSER_EVT_ALL,
}browser_event_id_t;

/*!
  Browser module id
  */
typedef enum
{
  /*!
    IPanel
    */
  BROWSER_ID_IPANEL28 = 0,
  /*!
    The maximal number  of browser modules
    */
  BROWSER_ID_MAX_NUM,
    /*!
    Unkown module
    */
  BROWSER_UNKNOWN,
}browser_module_id_t;
/*!
  Define the browser system event call back function's prototype
  */
typedef RET_CODE (*browser_event_cb_t)(u32 slot, u32 event, u32 param, u32 
                                 browser_id, u32 context);

/*!
  The event callback structure for Browser module
  */
typedef struct
{
/*!
    The callback function pointer.
    */
  browser_event_cb_t evt_cb;
/*!
    The context need to transfered by this callback function.
    */
  u32 context;
}browser_event_notify_t;
/*!
  Define a macro function to culculate array length in u32
  */
#define BROWSER_EVT_U32_LEN (((BROWSER_EVT_ALL - 1) >> 4) + 1)

/*!
  The browser max slot number
  */
#define BROWSER_MAX_SLOT_NUM 1

/*!
  The event callback structure for browser system module
  */
typedef struct
{
  /*!    
      The events happened
    */
  u32 events[BROWSER_MAX_SLOT_NUM][BROWSER_EVT_U32_LEN];
  /*!    
      The flag to indicate on which slot the events happened
    */
  u8 happened[BROWSER_MAX_SLOT_NUM];
}browser_event_info_t;


/*!
  Browser module function list
  */
typedef struct
{
  /*!
    Initialize the Browser module
    */
  RET_CODE (*init)(void *param);
  /*!
    De-Initialize the Browser module
    */
  RET_CODE (*deinit)();  
  /*!
    Open the Browser module
    */
  RET_CODE (*open)();
  /*!
    Close the Browser module
    */
  RET_CODE (*close)();
   /*!
    Stop the Browser module
    */
  RET_CODE (*resume)();
    /*!
    Pause the Browser module
    */
  RET_CODE (*pause)(); 
  /*!
    IO control function.
    */    
  RET_CODE (*io_ctrl)(u32 cmd, void *param);
}browser_func_t;


/*!
  Browser module function list
  */
typedef struct
{
  /*!
    The flag to indicate the ads module has been attached
    */
  u8 attached;
  /*!
    The flag to indicate the ads module has been initialized
    */
  u8 inited;
  u8 task_start;
  u8 task_pro_end;
  /*!
    The function list of this ADS module
    */  
  browser_func_t func;
  /*!
    The private data internal used for the ads module
    */   
  void *p_priv;
}browser_op_t;

/*!
  Browser adapter global info
  */
typedef struct
{
  /*!
    The flag to indicate if ads is inited
    */
  u32 inited;
   /*!
    The browser module's operation
    */  
  browser_op_t adm_op[BROWSER_ID_MAX_NUM];
    /*!
    The current event happend
    */  
  u32 events[BROWSER_MAX_SLOT_NUM][BROWSER_EVT_U32_LEN];
  /*!
    The current event parameter
    */  
  u32 event_param[BROWSER_MAX_SLOT_NUM];
  /*!
    The mutex used to lock the operation of events setting and getting 
    */   
  os_sem_t evt_mutex;
    /*!
    The event call back infomation 
    */  
  browser_event_notify_t notify;  
  
}bowser_adapter_priv_t;

extern RET_CODE browser_module_init(browser_module_id_t cam_id);
extern void browser_send_event(u32 slot, browser_module_id_t browser_id, u32 event, 
  u32 param);


#endif //__ADS_ADAPTER_H__
