/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ADS_ADAPTER_H__
#define __ADS_ADAPTER_H__

/*!
  ADS module function list
  */
typedef struct
{
  /*!
    Initialize the ADS module
    */
  RET_CODE (*init)(void *param);
  /*!
    De-Initialize the ADS module
    */
  RET_CODE (*deinit)(void);  
  /*!
    Open the ADS module
    */
  RET_CODE (*open)(void);
  /*!
    Close the ADS module
    */
  RET_CODE (*close)(void);
  /*!
    Display the ad
    */
  RET_CODE (*display)(ads_info_t *param);
  /*!
    Hide the ad
    */
  RET_CODE (*hide)(ads_info_t *param); 
  /*!
    IO control function.
    */    
  RET_CODE (*io_ctrl)(u32 cmd, void *param);
}ads_func_t;

/*!
  ADS module function list
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
  /*!
    The function list of this ADS module
    */  
  ads_func_t func;
  /*!
    The private data internal used for the ads module
    */   
  void *p_priv;
}ads_op_t;

/*!
  ADS adapter global info
  */
typedef struct
{
  /*!
    The flag to indicate if ads is inited
    */
  u32 inited;
  /*!
    The ads module's operation
    */  
  ads_op_t adm_op[ADS_ID_ADT_MAX_NUM];
  /*!
    The event call back infomation 
    */  
  ads_event_notify_t notify;  
  /*!
    The current event happend
    */  
  u32 events[ADS_ID_ADT_MAX_NUM][AD_EVT_U32_LEN];
  /*!
    The current event parameter
    */  
  u32 event_param[ADS_ID_ADT_MAX_NUM];
  /*!
    The mutex used to lock the operation of events setting and getting 
    */   
  os_sem_t evt_mutex;
}ads_adapter_priv_t;

/*!
  Clear ads events
  \param[in] slot The index of slot
  
  \return None
  */
void ads_clr_events(u32 slot);

/*!
  Send ads event
  \param[in] slot The index of slot
  \param[in] event The event, see ads_event_id_t in ads_ware.h
  \param[in] param The parameter of this event
  
  \return None
  */
void ads_send_event(u32 slot, u32 event, u32 param);

/*!
  For ads module get the adapter priv handle
  
  \return g_ads_priv
  */
ads_adapter_priv_t *ads_get_adapter_priv(void);

#endif //__ADS_ADAPTER_H__
