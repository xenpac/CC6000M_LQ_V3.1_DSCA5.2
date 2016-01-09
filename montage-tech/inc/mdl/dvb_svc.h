/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVB_SVC_H_
#define __DVB_SVC_H_


/*!
    The size of filter mask codes
  */
#define MAX_FILTER_MASK_BYTES (12)
/*!
    The size of external software filter mask codes
  */
#define MAX_SOFT_FILTER_MASK_BYTES (12) 

/*!
    The max number of filter
  */
#define MAX_FILTER_NUM (31)

/*!
   Type of PSI
  */
typedef enum
{
  /*!
    Section data
    */
  FILTER_TYPE_SECTION,
  /*!
    TS packet data
    */
  FILTER_TYPE_TSPKT
}filter_type_t;

/*!
  DVB service commands
  */
typedef enum
{
  /*!
    request table
    */
  DVB_REQUEST,
  /*!
    free table
    */
  DVB_FREE,
  /*!
    update table's ts in
    */
  DVB_TABLE_TS_UPDATE
} dvb_svc_cmd_t;


/*!
  DVB events
  */
typedef enum
{
  /*!
    Flag of evt start
    */
  DVB_EVT_BEGIN = (MDL_DVB << 16),  
  /*!
    Invoke this event when get a PAT, value = 0x1
    */
  DVB_PAT_FOUND,
  /*!
    found a PMT info in a PAT, value = 0x2
    */
  DVB_PAT_PMT_INFO,
  /*!
    Invoke this event when get a PMT, value = 0x3
    */
  DVB_PMT_FOUND,
  /*!
    Invoke this event when get a FDT, value = 0x4
    */
  DVB_FDT_FOUND,  
  /*!
    Invoke this event when get a BAT, value = 0x5
    */
  DVB_BAT_FOUND,  
  /*!
    Invoke this event when get a BAT linkage update info, value = 0x6
    */
  DVB_BAT_LINKAGE_UPDATE_FOUND,
  /*!
    Invoke this event when get a NIT, value = 0x7
    */
  DVB_NIT_FOUND,  
  /*!
    Invoke this event when get a SDT, value = 0x8
    */
  DVB_SDT_FOUND,
  /*!
    Invoke this event when get a SDT_OTR, value = 0x9
    */
  DVB_SDT_OTR_FOUND,
  /*!
    Invoke this event when got a EIT_ACT, value = 0xa
    */
  DVB_EIT_ACT_FOUND,
  /*!
    Invoke this event when got a EIT_OTR, value = 0xb
    */
  DVB_EIT_OTR_FOUND,  
  /*!
    Invoke this event when got a TOT, value = 0xc
    */ 
  DVB_TOT_FOUND,
  /*!
    Invoke this event when got a RST, value = 0xd
    */ 
  DVB_RST_FOUND,
  /*!
    Invoke this event when got a TDT, value = 0xe
    */ 
  DVB_TDT_FOUND,
  /*!
    Invoke this event when get CATs, value = 0xf
    */
  DVB_CAT_FOUND,
  /*!
    Invoke this event when get DSIs, value = 0x10
    */
  DVB_DSMCC_DSI_FOUND,
  /*!
    Invoke this event when get DIIs, value = 0x11
    */
  DVB_DSMCC_DII_FOUND,
  /*!
    Invoke this event when get DDBs, value = 0x12
    */
  DVB_DSMCC_DDM_FOUND,
  /*!
    scramble flag found, value = 0x13
    */
  DVB_SCRAMBLE_FLAG_FOUND,
  /*!
    All job done, value = 0x14
    */
  DVB_ALL_JOB_DONE,
  /*!
    dvb stopped, value = 0x15
    */
  DVB_STOPPED,
  /*!
    table timed out, value = 0x16
    */
  DVB_TABLE_TIMED_OUT,
  /*!
    dvb status was cleaned, value = 0x17
    */
  DVB_CLEANED,
  /*!
  dvb task paused, value = 0x18
    */
  DVB_PAUSED,
  /*!
    Invoke this event when get a SDT_OTR_SCH, value = 0x19
    */
  DVB_EIT_SCH_OTR_FOUND,
  /*!
    Invoke this event when get a SDT_OTR_SCH, value = 0x1a
    */
  DVB_EIT_SCH_ACT_FOUND,
  /*!
    Invoke this event when get a DDS table is found
    */
  DVB_DDS_TAB_FOUND,
  /*!
    buffer overflow, value = 0x1b
    */
  DVB_BUF_OVERFLOWING,
  /*!
    found a section, value = 0x1c
    */
  DVB_ONE_SEC_FOUND,
  /*!
    do free, value = 0x1D
    */
  DVB_FREED,
  /*!
    Invoke this event when get ECMs, value = 0x1E
    */
  DVB_ECM_FOUND,
  /*!
    Invoke this event when get ECMs, value = 0x1F
    */
  DVB_EMM_FOUND,
  /*!
    ABS software upgrade id descriptior found
    */
  DVB_ABS_SOFTWARE_UPGRADE_ID_DES_FOUND,
  /*!
    ABS upgrade PMT info
    */
  DVB_ABS_UPGRADE_PMT_INFO,
  /*!
    download control section found
    */
  DVB_DCS_FOUND,
  /*!
    partition control section found
    */  
  DVB_PCS_FOUND, 
  /*!
    datagram found
    */  
  DVB_DTM_FOUND,   
  /*!
    tkgs table found
    */
  DVB_TKGS_FOUND,  
} dvb_event_t;

/*!
  DVB request table mode
  */
typedef enum
{
  /*!
    DVB request table with single tabel, auto free
    */
  DATA_SINGLE = 0,
  /*!
    DVB request table with periodic, manual free
    */
  DATA_PERIODIC,
  /*!
    DVB request table with multi-section, manual free
    */
  DATA_MULTI
}request_mode_t;

/*!
    For PTI extern mode.  
    Defines the external buffer used in PTI extern mode.
  */
typedef struct tag_filter_ext_buf
{
  /*!
    Pointer to buffer
    */
  u8 *p_buf;
  /*!
    Size of buffer
    */
  u32 size;
  /*!
    Pointer to next buffer
    */
  struct tag_filter_ext_buf  *p_next;
  /*!
    priv
    */
  u32 data_size;
  /*!
    priv
    */
  u8  *p_data;
  /*!
    priv
    */
  u8  *p_cache;
  /*!
    priv
    */
  u8  *p_r;
}filter_ext_buf_t;

/*!
  Section data struct
  */
typedef struct
{
  /*!
    Section data buffer
    */
  u8 *p_buffer;  
  /*!
    Service instance
    */
  service_t *p_svc;
  /*!
    PSI table id
    */
  u8 table_id;
  /*!
    valid filter content mask size 
    */
  u8 filter_mask_size;
  /*!
    table mapping index
    */
  u8 table_map_index;
  /*!
    section id
    */
  u16 id;
  /*!
    service id
    */
  u16 sid;
  /*!
     PSI pid
    */
  u16 pid;
  /*!
    data buffer id
    */
  u16 buf_id;
  /*!
    pti dev handle
    */
 void *pti_handle;
  /*!
    dmx channel handle
    */
 u16 dmx_handle;
  /*!
    filter content
    */
  u8  filter_code[MAX_FILTER_MASK_BYTES];
  /*!
    filter content mask
    */
  u8  filter_mask[MAX_FILTER_MASK_BYTES];
  /*!
    start ticks
    */
  u32 start_ticks;
  /*!
    how many times this section time out in ms
    */
  u32 timeout;
  /*!
    how many times this section interval time in ms, if it need cycle request
    */
  u32 interval_time;
  /*!
    filter mode PSI/TS
    */
  filter_type_t filter_mode;
  /*!
    need check crc
    */
  BOOL crc_enable;
  /*!
    extern buffer on multi sec
    */
  filter_ext_buf_t   *p_ext_buf_list;
  /*!
    direct send data without parse
    */
  BOOL direct_data;
  /*!
    Request context
    */
  u32 r_context;
  /*!
    if need external software filter
    */
  BOOL b_filter_external;
  /*!
    external software  filter content
    */
  u8  filter_excode[MAX_SOFT_FILTER_MASK_BYTES];
  /*!
    external software filter content mask
    */
  u8  filter_exmask[MAX_SOFT_FILTER_MASK_BYTES];
  /*!
    valid external software filter content mask size 
    */
  u8 filter_exmask_size;
  /*!
    use dmx driver soft filter
    */
  BOOL use_soft_filter;
  /*!
    TS IN style:     DMX_INPUT_EXTERN0,    DMX_INPUT_EXTERN_CI
    */
   u8 ts_in;
} dvb_section_t;

/*!
  the declaration of request table callback function
  */
typedef void (*request_proc_t)(dvb_section_t *p_sec, u32 para1, u32 para2);

/*!
    the declaration of parse table callback function
  */
typedef void (*parse_proc_t)(handle_t handle, dvb_section_t *p_sec);

/*!
    the declaration of free table callback function
  */
typedef void (*free_proc_t)(void);

/*!
  dvb register parameter data struct
  */
typedef struct
{
  /*!
    the register table id
    */
  u16 table_id;
  /*!
    the register table mode
    */
  request_mode_t req_mode;
  /*!
    request function pointor with table_id
    */
  request_proc_t request_proc;
  /*!
    parse function pointor with table_id
    */
  parse_proc_t parse_proc;

  /*!
    parse function pointor with table_id
    */
  free_proc_t free_proc;
} dvb_register_t;

/*!
  dvb request parameter data struct
  */
typedef struct
{
  /*!
    the request table id
    */
  u16 table_id;
  /*!
    the register table mode
    */
  request_mode_t req_mode;
  /*!
    the period for DATA_PERIODIC only
    */
  u32 period;
  /*!
    request paramter1
    */
  u32 para1;
  /*!
    request paramter2
    */
  u32 para2;
  /*!
    filter code
    */
  u8 filter_code[MAX_FILTER_MASK_BYTES];
  /*!
    filter mask
    */
  u8 filter_mask[MAX_FILTER_MASK_BYTES];
  /*!
    Context
    */
  u32 context;
  /*!
    TS IN style:     DMX_INPUT_EXTERN0,    DMX_INPUT_EXTERN_CI
    */
   u8 ts_in;  
} dvb_request_t;

/*!
  DVB class declaration
  */
typedef struct
{
  /*!
    virtual function to start dvb module
    */
  void (*start)(class_handle_t handle, u8 *p_mem, u32 size, BOOL dynamic,
    u8 filter_num, u8 sec_num);
    
  /*!
    virtual function to stop dvb module
    */
  void (*stop)(class_handle_t handle);

  /*!
    virtual function to get a service handle from dvb module
    */
  service_t * (*get_svc_instance)(class_handle_t handle, u32 context);

  /*!
    remove svc instance
    */
  void (*remove_svc_instance)(class_handle_t handle, service_t *p_svc);

  /*!
    get memory size for running a dvb module
    */
  u32 (*get_mem_size)(class_handle_t handle,  BOOL dynamic, u8 filter_num, u8 sec_num);

  /*!
    register a table
    */
  void (*register_table)(class_handle_t handle, dvb_register_t *p_para);

  /*!
    allocate table

    \param[in] handle dvb's handle
    \param[in] p_src_sec request parameter \see dvb_section_t.
    */
  void (*alloc_section)(class_handle_t handle, dvb_section_t *p_src_sec);

  /*!
    send evt for error data.
    \param[in] p_svc second parameter \see dvb_section_t.
    */
  void (*filter_data_error)(service_t *p_svc, dvb_section_t *p_sec);

  /*!
    dvb class privated data
    */
  void *p_data;
} dvb_t;


/*!
  Module initialize function

  \param[in] priority task priority
  \param[in] stack_size task stack size
  \return dvb's handle
  */
handle_t dvb_init(u32 priority, u32 stack_size);

/*!
  for the new api
  Module initialize function

  \param[in] priority task priority
  \param[in] stack_size task stack size
  \return dvb's handle
  */
handle_t dvb_init_1(u32 priority, u32 stack_size, u8 main_ts_in);

/*!
  set dmx software or hardware for dvb_svc1.c
  if use software, should call after dvb_init_1
  \param[in] use_soft_filter
  \return void
  */
void dvb_svc_use_soft(BOOL use_soft_filter);

/*!
  get dmx software or hardware flag from dvb_svc1.c
  \param[in] None
  \return flag
  */
BOOL dvb_svc_is_use_soft(void);

#endif // End for __DVB_SVC_H_

