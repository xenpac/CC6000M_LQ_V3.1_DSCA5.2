/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CAS_ADAPTER_H__
#define __CAS_ADAPTER_H__

/*!
  The max dmx section size used by cas
  */
#define CAS_DVB_SECTION_SIZE  (4096*2)

/*!
  Max audio pid supported when parse PMT information
  and the actual audio pid number can be more or less than this value
  */
#define DVB_MAX_AUD_PID_NUM 16

/*!
  Max teletext description supported when parse PMT information
  */
#define DVB_MAX_ECM_DESC_NUM (40)

/*!
  CAT pid
  */
#define DVB_PID_CAT                   0x01
/*!
  NIT  pid
  */
#define DVB_PID_NIT                   0x10

/*!
  Table id for message section of ECM
  */
#define DVB_TID_ECM                 0x80
/*!
  Table id for EMM information
  */
#define DVB_TID_EMM                 0x82

/*!
  PMT table id
  */
#define DVB_TID_PMT                   0x02
/*!
  CAT table id
  */
#define DVB_TID_CAT                   0x01

/*!
  CA descriptor id
  */
#define DVB_CA_DESC_ID                         (0x9)

/*!
  CAT CRC length
  */
#define CAS_CRC_LEN  (4)
/*!
  CAT default PMT sector length
  */
#define CAS_DEF_PMT_SEC_LEN     (12)
/*!
  CAT PMT descripter header length
  */
#define CAS_PMT_DESC_HEAD_LEN   (5)

/*!
  Stream tag definitions in stream type desc
  */
/*!
  VIDEO TAG for MPEG1
  */
#define DVB_STREAM_MG1_VIDEO               0x01
/*!
  VIDEO TAG for MPEG2
  */
#define DVB_STREAM_MG2_VIDEO               0x02
/*!
  VIDEO TAG for AVS
  */
#define DVB_STREAM_AVS_VIDEO               0x42
/*!
  AUDIO TAG for MPEG1
  */
#define DVB_STREAM_MG1_AUDIO               0x03
/*!
  AUDIO TAG for MPEG2
  */
#define DVB_STREAM_MG2_AUDIO               0x04
/*!
  TAG FOR 264 VIDEO DESC
  */
#define DVB_STREAM_264_VIDEO               0x1B
/*!
  TAG FOR AVS VIDEO DESC
  */
#define DVB_STREAM_AVS_VIDEO               0x42
/*!
  TAG for AAC audio desc
  */
#define DVB_STREAM_AAC_AUDIO               0x0F
/*!
  Component descriptor id
  */
#define DVB_DESC_COMPONENT_DESC            0x50
/*!
  Max audio pid supported when parse PMT information
  and the actual audio pid number can be more or less than this value
  */
#define DVB_MAX_AUDIO_PID_NUM 16

/*!
  CA descriptor number
  */
#define DVB_MAX_EMM_DESC_NUM (16)
/*!
  DMX filter use single mode
  */
#define FILTER_CONTINOUS_DISABLED (1)
/*!
  DMX filter use continuous mode
  */
#define FILTER_CONTINOUS_ENABLED (0)
/*!
  CA card event which need to be handled by CA adapter layer
  */
typedef enum
{
  /*!
    The event to indicate the card plug into the slot 0 
    */ 
  CAS_EVT_SLOT0_PLUG_IN = 0x01,
  /*!
    The event to indicate the card plug out from the slot 0  
    */   
  CAS_EVT_SLOT0_PLUG_OUT = 0x02,
  /*!
    The event to indicate the card plug into the slot 1 
    */ 
  CAS_EVT_SLOT1_PLUG_IN = 0x04,
  /*!
    The event to indicate the card plug out from the slot 1 
    */   
  CAS_EVT_SLOT1_PLUG_OUT = 0x08,
  /*!
    The event to indicate the card plug into the slot 2 
    */ 
  CAS_EVT_SLOT2_PLUG_IN = 0x10,
  /*!
    The event to indicate the card plug out from the slot 2  
    */   
  CAS_EVT_SLOT2_PLUG_OUT = 0x20,
  /*!
    The event to indicate the card plug into the slot 3
    */ 
  CAS_EVT_SLOT3_PLUG_IN = 0x40,
  /*!
    The event to indicate the card plug out from the slot 3
    */   
  CAS_EVT_SLOT3_PLUG_OUT = 0x80,  
  /*!
    The event slot 0 mask
    */   
  CAS_EVT_SLOT0_MASK = 0x03,
  /*!
    The event slot 1 mask
    */   
  CAS_EVT_SLOT1_MASK = 0x0c,
  /*!
    The event slot 2 mask
    */   
  CAS_EVT_SLOT2_MASK = 0x30,
  /*!
    The event slot 3 mask
    */   
  CAS_EVT_SLOT3_MASK = 0xc0,
   /*!
    The event mask
    */   
  CAS_EVT_SLOT_MASK = 0xFF, 
}cas_slot_evt_t;

/*!
  CA data which need to be handled by CA adapter layer
  */
typedef enum
{
  /*!
    The event mail data
    */   
  CAS_EVT_MAIL_MASK = 0x01,
  /*!
    The event serial data
    */   
  CAS_EVT_SERIAL_MASK = 0x02,
   /*!
    The event mask
    */   
  CAS_EVT_DATA_MASK = 0xFF, 
}cas_data_evt_t;

/*!
  CAS mail compare type
  */
typedef enum
{
  /*!
    compare mail id
    */
  MAIL_COMPARE_ID = 1,
  /*!
    compare send date
    */
  MAIL_COMPARE_SEND_DATE,
  /*!
    compare create date
    */
  MAIL_COMPARE_CREATE_DATE,  
  /*!
    max
    */
  MAIL_COMPARE_MAX
}cas_mail_compare_type_t;

/*!
  CA system module function list for MG
  */
typedef struct
{
   /*!
    The smart card driver handle
    */
  void *p_smc;
   /*!
    The ATR data to be parsed by the specific cas module
    */   
  scard_atr_desc_t *p_atr;
}card_reset_info_t;

/*!
  CA system module function list for MG
  */
typedef struct
{
  /*!
    Initialize the CA system module
    */
  RET_CODE (*init)();
  /*!
    Close the CA system module
    */
  RET_CODE (*deinit)();  
  /*!
    Reset the card
    */
  RET_CODE (*card_reset)(u32 slot, card_reset_info_t *p_info);   
  /*!
    The process on card removing
    */
  RET_CODE (*card_remove)();     
  /*!
    The cat,pmt,nit resent
    */
  RET_CODE (*table_resent)();
  /*!
    CA system identification
    */
  RET_CODE (*identify)(u16 ca_sys_id);  
  /*!
    other tables process
    */   
  RET_CODE (*table_process)(u32 t_id, u8 *p_buf, u32 *p_result);  
  /*!
    IO control function.
    */    
  RET_CODE (*io_ctrl)(u32 cmd, void *param);
  /*!
    cas get PlatformID.
    */   
  RET_CODE (*get_platformid)(u16 *param);

}cas_func_t;

/*!
  CA system module function list
  */
typedef struct
{
  /*!
    The flag to indicate the cas module has been attached
    */
  u8 attached;
  /*!
    The flag to indicate the cas module has been initialized
    */
  u8 inited;
  /*!
    The flag to indicate if the card resetting is handled by ca module
    */
  u8 card_reset_by_cam;  
  /*!
    The function list of this cas module
    */  
  cas_func_t func;
  /*!
    The private data internal used for the cas module
    */   
  void *p_priv;
}cas_op_t;

/*!
  CAS adapter event info
  */
typedef struct
{
  /*!
    The flag to indicate on which slot which module the events happened
    */
  u8 slot_evt[CAS_MAX_SLOT_NUM];
}cas_adp_evt_info_t;

/*!
  CAS adapter global info
  */
typedef struct
{
  /*!
    The flag to indicate if cas is inited
    */
  u32 inited;
  /*!
    The data of pmt
    */
  u8 pmt_data[CAS_DVB_SECTION_SIZE];
  /*!
    The length of pmt
    */
  u32 pmt_length;
  /*!
    The data of cat
    */
  u8 cat_data[CAS_DVB_SECTION_SIZE];
  /*!
    The length of pmt
    */
  u32 cat_length;
  /*!
    The data of nit
    */
  u8 nit_data[CAS_DVB_SECTION_SIZE];
  /*!
    The length of pmt
    */
  u32 nit_length;
  /*!
    The task service handle
    */
  drvsvc_handle_t *p_svc;
  /*!
    The task data process handle
    */
  drvsvc_handle_t *p_data_svc;
  /*!
    The cas module's operation
    */  
  cas_op_t cam_op[CAS_ID_MAX_NUM];
  /*!
    The flag to indicate if the attached cas module is actived on the specific card slot
    */   
  u8 slot_active[CAS_MAX_SLOT_NUM];
  /*!
    The smard card driver handle coressponding to the slot
    */  
  void *p_smc_drv[CAS_MAX_SLOT_NUM];
  /*!
    The cas module id coressponding to the slot
    */ 
  u16 cam_id[CAS_MAX_SLOT_NUM];
  /*!
    The event call back infomation 
    */  
  cas_event_notify_t notify;  
  /*!
    The current event info
    */   
  cas_event_info_t evt_info;
  /*!
    The current adp event info
    */   
  cas_adp_evt_info_t adp_evt_info;
  /*!
    The mutex used to lock the operation of events setting and getting 
    */   
  os_sem_t evt_mutex;
  /*!
    The mutex used to lock the operation of data process 
    */   
  os_sem_t proc_mutex;
  /*!
    save data buffer
    */
  u8 *p_buf;
  /*!
    The slot number
    */
  u8 slot_num;
  /*!
    Read nvram data
    \param[in] module id see cas_module_id_t
    \param[in] node_id  data index
    \param[in] offset  data offset
    \param[in] length  data length
    \param[out] p_buffer  save data buffer
    */
  u32 (*nvram_read)(cas_module_id_t module_id, u16 node_id, u16 offset, u32 length, u8 *p_buffer);
  /*!
    Write nvram data
    \param[in] module id see cas_module_id_t
    \param[in] p_node_id  data index
    \param[in] len  data length
    \param[in] p_buffer  save data buffer
    */
  RET_CODE (*nvram_write)(cas_module_id_t module_id, u16 *p_node_id, u8 *p_buffer, u16 len);
  /*!
    Del nvram data
    \param[in] module id see cas_module_id_t
    \param[in] node_id  data index
    */
  RET_CODE (*nvram_del)(cas_module_id_t module_id, u16 node_id);
  /*!
    Get nvram data node list
    \param[in] module id see cas_module_id_t
    \param[out] p_buffer  save node id list
    \param[in/out] p_max [in]p_buffer mem size,[out]p_buffer node num
    */
  RET_CODE (*nvram_node_list)(cas_module_id_t module_id, u16 *p_buffer, u16 *p_max);
}cas_adapter_priv_t;


/*!
  Audio description information
  */
typedef struct
{
  /*!
    Audio pid
    */
  u16 p_id;
}_cas_audio_t;

/*!
  ecm descriptior definition
  ref GD/J 027-2009 4.3.3.2.1
  */
typedef struct 
{
  /*!
    CA system id
    */
  u16 ca_sys_id;
  /*!
    ECM pid
    */
  u16 ecm_pid;
  /*!
    es pid :if es_pid is 0, meas this info parse from program info descriptor
    */
  u16 es_pid;
}_cas_desc_t;

/*!
  CA descriptor
  */
typedef struct 
{
  /*!
    CA system id
    */
  u16 ca_sys_id;
  /*!
    EMM pid
    */
  u16 emm_pid;
}_ca_desc_t;

/*!
  PMT struct
  */
typedef struct
{
  /*!
    Program number
    */
  u16 prog_num;
  /*!
    PMT version
    */
  u8 version;
  /*!
    pcr pid
    */
  u16 pcr_pid;
  /*!
    video pid
    */
  u16 video_pid;
  /*!
    video type
    */
  u8 video_type;
  /*!
    audio pid & type
    */
  _cas_audio_t audio[DVB_MAX_AUD_PID_NUM];
  /*!
    Audio channel count
    */
  u16 audio_count;
  /*!
    Upgrade data stream id
    */
  u16 es_id;
  /*!
    desc count
    */
  u16 ecm_cnt;
  /*!
    CAS description
    */  
  _cas_desc_t cas_descr[DVB_MAX_ECM_DESC_NUM];
} cas_pmt_t;

/*!
  CA structure
  */
typedef struct
{
  /*!
    Table id
    */
  u8 tab_id;
  /*!
    Section number
    */
  u8 sec_num;
  /*!
    Last section number
    */
  u8 last_sec_num;
  /*!
    Version number
    */
  u8 ver_num;
  /*!
    desc num
    */
  u32 emm_cnt;    
  /*!
    CA descriptor information
    */
  _ca_desc_t ca_desc[DVB_MAX_EMM_DESC_NUM];
}cas_cat_t;

/*!
  The max dmx channel used in cas 
  */
#define CAS_DMX_CHANNEL_COUNT (16)

/*!
  The dmx channel info used by cas
  */
typedef struct
{
  /*!
    the buffer pointer of the dmx channel
    */
  u8 *p_buf;
  /*!
    the valid date pointer of the dmx channel
    */
  u8 *p_data;
  /*!
    the buffer size of the dmx channel
    */  
  u32 size;
  /*!
    the pid of the dmx channel
    */  
  u16 pid;
  /*!
    the tid of the dmx channel
    */  
  u16 tid;
  /*!
    the driver handle of the dmx channel
    */  
  u16  handle;
  /*!
    the index of the dmx channel
    */  
  u16  chan_index;
}cas_dmx_chan_t;

/*!
  cas_get_private_data
  
  \return private data
  */
extern cas_adapter_priv_t *cas_get_private_data();

/*!
  Send cas event
  \param[in] slot The index of slot
  \param[in] cam_id The id of cas module
  \param[in] event The event, see cas_event_id_t in cas_ware.h
  \param[in] param The parameter of this event
  
  \return None
  */
extern void cas_send_event(u32 slot, cas_module_id_t cam_id, u32 event, u32 param);

/*!
  Parse PMT table
  \param[in] p_buffer The buffer point of the PMT table
  \param[in] p_pmt The PMT structure need to be filled by the parsing
  
  \return None
  */
extern void cas_parse_pmt(u8 *p_buffer, cas_pmt_t *p_pmt);

/*!
  Parse CAT table
  \param[in] p_buffer The buffer point of the CAT table
  \param[in] p_cat The CAT structure need to be filled by the parsing
  */
extern void cas_parse_cat(u8 *p_buffer, cas_cat_t *p_cat);

/*!
  Derequest table
  \param[in] p_dmx The dmx device handle
  \param[in] p_chan The dmx channel handle
    */
extern void cas_table_derequest(void *p_dmx, cas_dmx_chan_t *p_chan);

/*!
  Request table
  \param[in] p_dmx The dmx device handle
  \param[in] p_chan The dmx channel handle
  \param[in] ECM/EMM pid
  \param[in] value of filter
  \param[in] mask of filter
  \param[in] single mode or continuous
    */
extern RET_CODE cas_table_request(void *p_dmx, cas_dmx_chan_t *p_chan, u16 pid,
                 u8 *p_match,u8 *p_mask, u8 filter_mode);

/*!
  Compare mail
  \param[out] 
  return:
    0:old mail
    1:new mail
  */
extern u8 cas_compare_mail(u8 *p_data, cas_mail_compare_type_t type);
/*!
  Get mail headers
  \param[in] module id see cas_module_id_t
  \param[out] p_buffer see struct cas_mail_headers_t  
  */
extern void cas_get_mail_headers(cas_module_id_t module_id, u8 *p_buffer);

/*!
  Get some mail body
  \param[in] module id see cas_module_id_t
  \param[in] mail_id  mail index
  \param[out] p_buffer save mail body data 
  */
extern void cas_get_mail_body(cas_module_id_t module_id, u8 *p_buffer, u32 mail_id);

/*!
  Delet some mail
  \param[in] module id see cas_module_id_t
  \param[in] mail_id  mail index
  */
extern void cas_del_mail_data(cas_module_id_t module_id, u32 mail_id);

/*!
  Delet all mail
  \param[in] module id see cas_module_id_t
  */
extern void cas_del_all_mail_data(cas_module_id_t module_id);

/*!
  Get stb serial number
  \param[in] module id see cas_module_id_t
  \param[out] p_buffer  save stb serial number max CAS_CARD_SN_MAX_LEN
  */
extern void cas_get_stb_serial(cas_module_id_t module_id, u8 *p_buffer);

/*!
  Init adapter nvram data manager
  \param[in] module id see cas_module_id_t
  */
extern void cas_init_nvram_data(cas_module_id_t module_id);

/*!
  Send cas adapter event
  \param[in] slot The index of slot
  \param[in] cam_id The id of cas module
  \param[in] event The event, see cas_data_evt_t
  */
extern void cas_send_evt_to_adapter(u32 slot, cas_module_id_t cam_id, u32 event);

extern RET_CODE cas_get_platformid(cas_module_id_t cam_id, u16 *platformid);

extern u8 cas_modify_vender(u8 *atr, u32 size);

#endif //__CAS_ADAPTER_H__
