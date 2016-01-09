/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EPG_DATA_PRIV4_H__
#define __EPG_DATA_PRIV4_H__

/*!
  Segment management in the data capturing process
  */
typedef struct seg_info_each_tab
{
  /*! 
    Table id
    */
  u32  tab_id   : 8;
  /*!
    TRUE when all events are saved
    */
  u32  is_all_evt_saved : 1;
  /*!
    Reserved 
    */
  u32  reserved : 23;
  /*!
    Captured segment info
    */
  u8  p_capt_seg_info[MAX_SEG_NUM_EACH_TABLE];
  /*!
    Target segment info
    */
  u8  p_targ_seg_info[MAX_SEG_NUM_EACH_TABLE];
}seg_info_each_tab_t;

/*!
  Service node saved into EPG database
  */
typedef struct  service_node_tag
{
  /*!
    Flag recording whether all p/f section has been got
    */ 
  u32  pf_flag    : 8;
  /*!
    Version number
    */
  u32  version    : 5; 
  /*!
    When pf is full, pf full is TRUE else pf full is false
    */
  u32  pf_full    : 1;
  /*!
    Reserved
    */
  u32 new_ver_to_notify    : 1;
  /*!
    Reserved
    */
  u32 reserved    : 1;
  /*!
    Service id
    */
  u32 service_id  : 16;
  /*!
    present event already got
    */
  u8 section_present_got;
  /*!
    Segment information of current stream in each table id
    */
  seg_info_each_tab_t *p_seg_info_in_table;
  /*!
    Pointing to next service node of parent ts node
    */
  struct service_node_tag *p_next_s_node;
  /*!
    Pointing to head event of svc 
    */
  evt_node_t  *p_evt_node_head;
  /*!
    Pointing to head event of each day 
    */
  evt_node_t  **pp_evt_day_head;
  /*!
    Pointing to head event of svc 
    */
  u32 saved_evt_day;
  /*!
    Record last latest adding event node
    */
  evt_node_t  *p_recent_evt_node;
} sev_node_t;

/*!
  Service capturing status of each table with table id as a ID
  */
typedef struct tab_svc_info_tag
{
  /*!
    Table id
    */
  u8  table_id;
  /*!
    Total service node to be captured
    */
  u8 total_serv_num;
  /*!
    Total service node already captured
    */
  u8 capt_serv_num;
  /*!
    Flag to avoid more operation
    */
  u8  is_sch_full;
}tab_svc_info_t;

/*!
  Transport stream node
  */
typedef struct ts_node_tag
{
  /*!
    Transort stream id
    */
  u16 ts_id;
  /*!
    Total PF status to be captured
    */
  u8 pf_total_svc_num;
  /*!
    Total PF status already captured
    */
  u8 pf_cap_svc_num;
  /*!
    Flag to avoid more operation
    */
  BOOL  is_pf_ready;
  /*!
    Flag to avoid more operation  fix bug ##6449
    */
  u32  pf_count;
  /*!
    Service information under each table id
    */
  tab_svc_info_t *p_svc_info_per_tab;
  /*!
    Next ts node
    */
  struct ts_node_tag *p_next_ts_node;
  /*!
    Service head
    */
  sev_node_t *p_svc_head;
}ts_node_t;

/*!
  Network node logically used for fast data searching operation
  */
typedef struct orig_net_node_tag
{
  /*!
    Orginal network id
    */
  u16 orig_network_id;
  /*!
    reserved
    */
  u16 reserved;
  /*!
    Next network node
    */
  struct orig_net_node_tag  *p_next_net_node; 
  /*!
    Ts stream node head
    */
  ts_node_t *p_ts_node_head;
}orig_net_node_t;



/*!
  EPG database global parameter
  */
typedef struct
{
  /*!
    memf for net node with fixed size
    */
  lib_memf_t net_node_memf;
  /*!
    memf for ts node with fixed size
    */
  lib_memf_t ts_node_memf;
  /*!
    memf for svc node with fixed size
    */
  lib_memf_t svc_node_memf;
  /*!
    memf for evt node with fixed size
    */
  lib_memf_t evt_node_memf;
  /*!
    memp for none fixed memory 
    */
  lib_memp_t memp;
  /*!
  epg db report
  */
  epg_db_report_t db_report;
  /*!
     Network node head
    */
  orig_net_node_t *p_net_node_head;
  /*!
    EPG current svc id
    */
  u32 dy_policy_cnt : 16;
  /*!
    Max day of evt saved
  */
  u32 evt_max_day : 8;
  /*!
    evt priority
  */
  u32 evt_priority : 4;
  /*!
    Save evt info selection
  */
  u32 evt_info_priority : 4;
  /*!
    dynamic policy used only for event priority
    is EPG_EVT_OF_CUR_SVCS_ONLY
  */
  epg_dy_policy_t *p_dy_policy;
  /*!
    dynamic policy used only for event priority
    is EPG_EVT_OF_CUR_SVCS_ONLY
  */
  epg_dy_policy_t *p_dy_policy_intra;
  /*!
    Extent txt buffer
    */
  u8 *p_ext_txt_buf;
/*!
  same first lang code come times
  */
  u32 lang_code_first_times;
/*!
  same first lang code come times
  */
  u32 lang_code_second_times;
/*!
  same first lang code come times
  */
  u32 lang_code_default_times;
/*!
  same other lang code come times
  */
  u32 lang_code_other_times;
  /*!
    set epg lang code
    */
  u8 set_lang_code[LANGUAGE_CODE_LEN];
  /*!
    first epg lang code
    */
  u8 first_lang_code[LANGUAGE_CODE_LEN];
  /*!
    second lang code
    */
  u8 second_lang_code[LANGUAGE_CODE_LEN];
  /*!
    default epg lang code
    */
  u8 default_lang_code[LANGUAGE_CODE_LEN];
  /*!
  lang code  forbit_swich
  */
  BOOL lang_code_forbit_swich;
/*!
  same lang code  come times
  */
  u32 lang_code_check_times;
/*!
  lang code  sync
  */
  lang_code_sync_t lang_code_sync;
/*!
  net id
  */
  BOOL lang_set_sync_id;
/*!
  net id
  */
  u16 lang_net_id;
/*!
  Ts stream id,
  */
  u16 lang_ts_id; 
/*!
  Service id
  */
  u16 lang_svc_id;
}epg_db_t;

/*!
  Segment status
  */
typedef enum
{
  /*!
    Segement step full and one segment step consist the segment number defined
    in Macro SEG_SETP
    */
  SEG_STEP_FULL = 0, 
  /*!
    All segments in one table is ready
    */
  SEG_TAB_FULL,
  /*!
    Segment status not ready
    */
  SEG_NOT_READY
}seg_status_t;

/*!
  evt parent node
  */
typedef struct
{
  /*!
    Parent net ndoe
  */
  orig_net_node_t *p_net_node;
  /*!
     Parent ts node
    */
  ts_node_t *p_ts_node;
  /*!
    Parent service node
    */
  sev_node_t *p_svc_node;
}evt_parents_node_t;

#endif

