/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_EPG4_H_
#define __AP_EPG4_H_

/*!
  \file ap_epg.h
  Delecalre epg application.
  And this is a base class for all epg applications.
  */
/*!
  Application EPG definion
  */
typedef enum
{
  /*!
    All EPG schedule information in one table is ready
    */
  EPG_EVT_TAB_READY = ((APP_EPG << 16)|ASYNC_EVT),
  /*!
    EPG PF information of all services in current stream is ready
    */
  EPG_EVT_PF_READY,
  /*!
    One event is found
    */
  EPG_EVT_EVT_READY,
  /*!
    Event deleted
    */
  EPG_EVT_EVT_DELETED,
  /*!
    New event section found
    */
  EPG_EVT_NEW_SEC_FOUND,
  /*!
    New pf version found, this means current programme has changed
     */
  EPG_EVT_NEW_PF_VER_FOUND,  
  /*!
    New event section found
    */
  EPG_EVT_DB_FULL,
  /*!
    Notify UI that current application EPG module is ready to stop
    */
  EPG_EVT_STOP = ((APP_EPG << 16)|SYNC_EVT)
}epg_evt_t;

/*!
  Max message number supported in APP. EPG
  */
#define AP_EPG_MSG_MAX_NUM  (32)
/*!
  ext buffer size
  */
#define PTI_BUF_UNIT_SIZE        ((100) * (KBYTES))
/*!
  ext buffer number
  */
#define MAX_EXT_BUF_USED          (5)

/*!
  Command definition in application EPG
  */
typedef enum
{
  /*!
    EPG app start for UI to start EPG scan
    */
  EPG_CMD_START_SCAN = ASYNC_CMD,
  /*!
    CMD to request PF info
    */
  EPG_CMD_START_SCAN_PF,  
  /*!
    update EIT's ts in
    */
  EPG_CMD_EIT_TS_UPDATE,  
  /*!
    EPG app stop for UI to stop EPG scan
    */
  EPG_CMD_STOP_SCAN = SYNC_CMD,
  EPG_CMD_SAMPLE
}epg_cmd_t;

/*!
  event para sent to UI
  */
typedef struct epg_event_para
{
  /*!
    network id
    */
  u16 orig_network_id;
  /*!
    ts id
    */
  u16 ts_id;
  /*!
    svc_id
    */
  u16 svc_id;
  /*!
    parental rating
    */
  u8 parental_rating;  
}epg_event_para_t;




/*!
  EPG implement private data
  */
typedef struct 
{
/*!
    config PF section switch
    */
  BOOL pf_switch_enable;
 /*!
    config period when PF table is request
    */
  u32 pf_requested_interval;
/*!
    config period when PF table is not requested
    */
  u32 pf_freed_interval;
/*!
    config number of newsections captured when notice to UI
    */
  u32 new_section_num;

  /*!
    Starting address of buffer unit
    */
  void  *p_ext_buf_list;
  /*!
    PTI buffer number and this value should be more than max PTI number 
    available
    */
  u8 ext_buf_num;
  /*!
    Block id
    */
  u8 block_id;
  /*!
    Block address
    */
  u32 block_addr;
  /*! 
    Block size
    */
  u32 block_size;
  /*! 
    ts in
    */
  u8 ts_in;  
}epg_impl_data_t;


/*!
  Implementation policy definition 
  */
typedef struct epg_policy_tag
{
  /*!
    Implementation policy for start EPG
    */
  BOOL (*epg_impl_start)(void *p_data, u8 block_id);
  /*!
    Implementation policy for stop EPG 
    */
  void (*epg_impl_stop)(void *p_data);
  /*!
    Implementation policy for application EPG initialization
    */
  void (*epg_impl_init)(void *p_data, u8 max_pti_num);
  /*!
   Implementation policy for register callback for rc_epg
    */
  void (*epg_impl_send_rc_epg)(char *p_data, u32 data_size);
  /*!
    Implementation policy for application EPG to get one external buffer 
    */
  //u32 (*get_ext_buf)(void *p_data,int index);
  /*!
    Implementation policy for application EPG to release pti-buffer 
    */
  //void (*release_ext_buf)(void *p_data, u32 p_ext_buf);
  /*!
    Private data for EPG policy
    */
  void *p_data;
}epg_policy_t;


/*!
  Get the instance of application EPG
  \param[in] p_epg_policy implementation policy to be set
  */
app_t * construct_ap_epg4(epg_policy_t *p_epg_policy);


#endif // End for __AP_EPG_H_

