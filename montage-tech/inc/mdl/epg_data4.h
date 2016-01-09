/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __EPG_DATA4_H_
#define  __EPG_DATA4_H_

/*!
  \file epg_data.h
  
  EPG data provides the data structure of EPG data in EPG database. And the APIs 
  to access these information also be declared. 
  
  Development policy:
  To fulfill the function of EPG, EPG database should work together with 
  application EPG and DVB mid-ware module
  */

/*!
  Max extend text buffer length per each event node
  */
#define MAX_EXT_TXT_BUF_LEN     ((4)*(KBYTES))

/*!
  filter ignore ID
  */
#define IGNORE_ID (0xFFFF)

/*!
  EPG save event priority
  */
typedef enum
{
  /*!
    save evt all
  */
  EPG_EVT_OF_ALL_TS = 0,
  /*!
    evt of cur ts will be saved only
  */
  EPG_EVT_OF_CUR_TS_ONLY,
  /*!
    evt of cur svcs(set by dynamic policy) will be saved only
  */
  EPG_EVT_OF_CUR_SVCS_ONLY
}epg_evt_priority_t;

/*!
  EPG save evt info selction
  */
typedef enum
{
  /*!
    Save evt info all
  */
  EPG_EVT_INFO_ALL = 0,
  /*!
    Do not save evt extend info
  */
  EPG_EVT_NAME_AND_SHORT,
  /*!
    Do not save evt short and extend info,
    only save evt name 
  */
  EPG_EVT_NAME_ONLY
}epg_evt_info_priority_t;

/*!
  EPG max day event saved
  */
typedef enum
{
  /*!
    save 4 days evt
  */
  EPG_EVT_4_DAYS = 4,
  /*!
    save 8 days evt
  */
  EPG_EVT_8_DAYS = 8,
  /*!
    save 12 days evt
  */
  EPG_EVT_12_DAYS = 12,
  /*!
    save 16 days evt
  */
  EPG_EVT_16_DAYS = 16
}epg_evt_day_t;


/*!
  EPG database policy
  */
typedef struct
{
  /*!
    epg db save event priority
  */
  epg_evt_priority_t evt_priority;
  /*!
    epg db save evt info priority
  */
  epg_evt_info_priority_t evt_info_priority;
  /*!
    Max day of evt saved
  */
  epg_evt_day_t evt_max_day;
  /*!
   mem addr for epg db
  */
  void *p_mem_addr;
  /*!
   mem size for epg db
  */
  u32 mem_size;
}epg_db_policy_t;

/*!
  EPG database dynamic policy
  */
typedef struct
{
  /*!
    cur network id
  */
  u16 cur_net_id;
  /*!
    cur ts id
  */
  u16 cur_ts_id;
  /*!
    cur service id
  */
  u16 cur_svc_id;
  /*!
    reserved
  */
  u16 reserved;
  /*!
    start time
    */
  utc_time_t start;
  /*!
    end time
    */
  utc_time_t end;
}epg_dy_policy_t;

/*!
  Item info in extend event descriptor by unicode format
  */
typedef struct
{
  /*!
    Item description, such as cast list etc.
    */
  u16 item_name[MAX_ITEM_DESC_LEN + 1];
  /*!
    Item content, such as cast name etc.
    */
  u16 item_cont[MAX_ITEM_CONT_LEN + 1];
}item_info_uni_t;

/*!
  EPG event desc info in unicode format
  */
typedef struct ext_evt_desc_uni_tag
{
  /*!
    Extend ext event index in extend event description series
    */
  u32 index;
  /*!
    MAX item information
    */
  item_info_uni_t item[MAX_ITEM_NUM];
  /*!
    Item number
    */
  u8  item_num;
  /*!
    Total desc number
    */
  u8  tot_ext_desc;
  /*!
    Event id
    */
  u16 evt_id;  
  /*!
    Language code
    */
  u8 lang_code[LANGUAGE_CODE_LEN];
  /*!
    Next event description 
    */
  struct ext_evt_desc_uni_tag *p_nxt_ext;
}ext_evt_desc_uni_t;

/*!
  Event node information in EPG database 
  */
typedef struct event_node
{
  /*!
    Next event node
    */
  struct event_node *p_next_evt_node;
  /*!
    Event start time
    */
  utc_time_t start_time;
  /*!
    Event duration
    */
  utc_time_t drt_time;
  /*!
    Event id
    */
  u16 event_id;  
  /*! 
    Event name
    */
  u16 event_name[MAX_EVT_NAME_LEN + 1];
  /*!
    Event description of event in unicode
    */
  u16 *p_sht_txt;
  /*! 
    The length of event short description
    */
  u16 sht_txt_len;
  /*! 
    The total length of event short and extend
    */
  u16 shrt_ext_len;
  /*!
    Extend event desc in unicode format
    */
  ext_evt_desc_uni_t *p_ext_desc;
  /*!
    Extend txt in unicode
    */
  u16 *p_ext_txt;
  /*!
    Extend event desc number
    */
  u8 ext_desc_num;
  /*!
    content nibble
    High 4 bits are level 1
    Low 4 bits are level 2
    */
  u8 cont_level;
  /*!
    User nibble
    High 4 bits are nibble 1
    Low 4 bits are nibble 2
    */
  u8 usr_nib;
  /*!
    Content valid or not
    */
  u8 cont_valid_flag;
  /*!
    Language code by unicode
    */
  u16 lang_code[LANGUAGE_CODE_LEN + 1];
  /*!
     parental rating
     */
  u8 parental_rating; 
} evt_node_t;

/*!
  EPG database status
  */
typedef enum
{
  /*!
    Normal return
    */
 EPG_DB_NORM = 0,
  /*!
    EPG database is full
    */
 EPG_DB_OVERFLOW,
  /*!
    New version EPG found
    */
 EPG_DB_NEW_VER_FOUND,
  /*!
    One service is ready
    */
 EPG_DB_ONE_SVC_READY,
  /*!
    PF is ready
    */
 EPG_DB_PF_READY,
 /*!
    EPG in one table is ready
    */
 EPG_DB_ONE_TAB_READY, 
  /*!
    EPG new section found
    */
 EPG_DB_NEW_SEC_FOUND,
  /*!
    EPG unknown error
    */
 EPG_DB_UNKNOWN_ERROR
}epg_db_status_t;

/*!
  operations for del epg data node
*/
typedef enum
{
  /*!
    del all node(reset)
    */
 EPG_DEL_ALL_NODE,
  /*!
    del overdue evt of pointed net node
    */
 EPG_DEL_OVERDEU_EVT,
 /*!
  del node by filter
  */
 EPG_DEL_BY_FILTER,
 /*!
  del node exp filter
  */
 EPG_DEL_EXP_FILTER,
 /*!
  del node exp dynamic policy
  */
 EPG_DEL_EXP_DY_POLICY
}epg_del_opt_t;

/*!
  Input parameter for request EPG information
  */
typedef struct tag_epg_filter
{
  /*!
    Original network id. if id is IGNORE_ID(0xFFFF):
    1.when qurey ignore it.
    2.when delete means all net node.
    */
  u16 orig_network_id;
  /*!
    Stream id. if id is IGNORE_ID(0xFFFF):
    1.when qurey ignore it.
    2.when delete means all ts node.
    */
  u16 stream_id;
  /*!
    Service id.when delete if id is IGNORE_ID(0xFFFF),
    means del all service node.
    */
  u16 service_id;
  /*!
    Content nibble level,only used for qurey.if zero means all.
    high 4 bits is level 1,low 4 bits is level 2,
    if level 2 is 0xf only match the level 1,
  */
  u8 cont_level;
  /*!
    reserved
    */
  u8 reserved;
  /*!
    Event start time. if the start_time is zero:
    1. when qurey, means from now on.
    2. when delete, means all.
    */
  utc_time_t start_time;
  /*!
    Event end time. if the end_time is zero, means forever.
    */
  utc_time_t end_time;
}epg_filter_t;

/*!
    query param and result
    */
typedef struct tag_epg_query_by_nibble
{
  /*!
    Original network id. if id is IGNORE_ID(0xFFFF):
    1.when qurey ignore it.
    2.when delete means all net node.
    */
  u16 orig_network_id;
  /*!
    Stream id. if id is IGNORE_ID(0xFFFF):
    1.when qurey ignore it.
    2.when delete means all ts node.
    */
  u16 stream_id;
  /*!
    Service id.when delete if id is IGNORE_ID(0xFFFF),
    means del all service node.
    */
  u16 service_id;
  /*!
    Content nibble level,only used for qurey.if zero means all.
    high 4 bits is level 1,low 4 bits is level 2,
    if level 2 is 0xf only match the level 1,
  */
  u8 cont_level;
  /*!
    reserved
    */
  u8 reserved;
  /*!
    Event start time. if the start_time is zero,
    means from now on.
    */
  utc_time_t start_time;
  /*!
    Event end time. if the end_time is zero, means forever.
    */
  utc_time_t end_time;
   /*!
    query result
  */
  void **p_result;
    /*!
    query count
    */
  u32 cnt;
}epg_query_by_nibble_t;






/*!
  epg db report
*/
typedef struct tag_epg_db_report
{
  /*!
   net node total cnt
  */
  u16 net_node_total;
  /*!
   net node used cnt
  */
  u16 net_node_used;
  /*!
   ts node total cnt
  */
  u16 ts_node_total;
  /*!
   ts node total cnt
  */
  u16 ts_node_used;
  /*!
   svc node total cnt
  */
  u16 svc_node_total;
  /*!
   svc node used cnt
  */
  u16 svc_node_used;
  /*!
   evt node total cnt
  */
  u16 evt_node_total;
  /*!
   evt node used cnt
  */
  u16 evt_node_used;
  /*!
   non fix mem total
  */
  u32 nonfix_total;
  /*!
   non fix mem used
  */
  u32 nonfix_used;
}epg_db_report_t;

 /*!
  lang code sync
  */
typedef enum tg_lang_code_sync
{
/*!
  lang code sync on same svc
  */
  LANG_CODE_SYNC_SVC = 0,
 /*!
  lang code sync on same ts
  */
  LANG_CODE_SYNC_TS,
/*!
  lang code sync on same net
  */
  LANG_CODE_SYNC_NET,
/*!
  lang code sync on same all
  */
  LANG_CODE_SYNC_ALL,

}lang_code_sync_t;

/*!
  Initialize of EPG data 
  */
void epg_data_init4(void);

/*!
  EPG data delete init
  \param[in] p_data EPG data handle
  */
void epg_data_deinit(void *p_data);

/*!
  EPG data memory allocation
  \param[in] p_data epg db private data
  \param[in] p_epg_db_policy policy for memory allocate
  */
BOOL epg_data_mem_alloc(void *p_data, epg_db_policy_t *p_epg_db_policy, 
                         u16 max_net, u16 max_ts, u16 max_svc, u16 max_evt);

/*!
  Release node memory 
  \param[in] p_data epg db private data
  */
void epg_data_mem_release(void *p_data);

/*!
  Set EPG db dynamic policy
  \param[in] p_data epg db private data
  \param[in] dynamic policy number
*/
void epg_data_init_dy_policy(void *p_data, u16 dy_policy_cnt);

/*!
  Set EPG db dynamic policy
  \param[in] p_data epg db private data
  \param[in] p_dy_policy dynamic policy
*/
void epg_data_set_dy_policy(void *p_data, epg_dy_policy_t *p_dy_policy);

/*!
  Get EPG db dynamic policy return epg_db_dynamic_policy_t*
  \param[in] p_data epg db private data
*/
epg_dy_policy_t *epg_data_get_dy_policy(void *p_data);

/*!
  Save EIT information into memory and return EPG_db_status_t status 
  of EPG data base after saving EIT information into memory
  \param[in] p_data epg db private data
  \param[in] p_eit_info eit information to be saved into memory
  \param[in] wheher p/f information is new version
  */
epg_db_status_t epg_data_add(void *p_data, eit_t *p_eit_info, 
                                                     BOOL *p_pf_new_ver);

/*!
  Del EPG data node with del_op option
  \param[in] p_data epg db private data
  \param[in] p_del_op operation to del node
  \param[in] p_filter input condition for del node
  */
BOOL epg_data_delete(void *p_data, epg_del_opt_t del_op, epg_filter_t *p_filter);

/*!
  Get event first & event count by filter, return evt_node_t* the event node first found 
  in memory or NULL
  \param[in] p_data epg db private data
  \param[in] p_filter input condition to find certain event node
  \param[in/out] p_evt_cnt event count found by filter
  */
evt_node_t *epg_data_get_evt(void *p_data, epg_filter_t *p_filter, u32 *p_evt_cnt);

/*!
  Get event present event by filter, return evt_node_t* the event node found 
  in memory or NULL
  \param[in] p_data epg db private data
  \param[in] p_filter input condition to find certain event node
  */
evt_node_t *epg_data_get_pf(void *p_data, epg_filter_t *p_filter);

/*!
  Get event next by filter, return evt_node_t* the event node found 
  in memory or NULL
  \param[in] p_data epg db private data
  \param[in] p_last_evt last event found by filter
  \param[in] p_filter input condition to find certain event node 
  */
evt_node_t *epg_data_get_next_evt(void *p_data, evt_node_t *p_last_evt, epg_filter_t *p_filter);

/*!
  Get epg data report 
  \param[in] p_data epg db private data
  */
epg_db_report_t *epg_data_get_db_report(void *p_data);

/*!
  Find the events in the given time condition
  */
u32 epg_create_list(void *p_data, evt_node_t **p_buf,
    char * p_str, u8 cont_level, utc_time_t *p_start_day, 
   utc_time_t *p_end_day, utc_time_t *p_start_time, utc_time_t *p_end_time);
/*!
  Get all events number in EPG DB
  */
u32 epg_get_all_evt_num(void *p_data);
/*!
  Sort the created list by time
  */
void epg_list_sort_by_time(evt_node_t **pp_evt, u32 item_num);
/*!
  Sort the created list by name
  */
void epg_list_sort_by_name(evt_node_t **pp_evt, u32 item_num);

/*!
  Query
  */
RET_CODE epg_query_by_nibble(void *p_data,
  epg_query_by_nibble_t *p_query);

/*!
  Query
  */
RET_CODE epg_query_by_nibble_list(void *p_data,
  epg_query_by_nibble_t *p_query, u32 query_cnt);
/*!
  init epg lang code
  */
void epg_data_init_lang_code(void *p_data);
/*!
  init epg set lang code
  */
void epg_data_set_lang_code(void *p_data,
  u8 first_lang[LANGUAGE_CODE_LEN],
  u8 second_lang[LANGUAGE_CODE_LEN],
  u8 default_lang[LANGUAGE_CODE_LEN]);
/*!
  set lang code policy,
  forbit_sw: true:don't swich lang priority but first lang code;
  pos: check event lang code position
  max_times:auto swich max different lang times
  */
void epg_data_set_lang_code_policy(void *p_data,
                                                                BOOL forbit_sw,
                                                                lang_code_sync_t pos,
                                                                u32 max_times);
/*!
  set lang sync sync pg pararm,
  play_pg_pararm
  */
void epg_data_set_lang_sync_pg_pararm(void *p_data,
                                                                u16 pg_net_id,
                                                                u16 pg_ts_id,
                                                                u16 pg_svc_id);
/*!
  init epg try lang code priority
  */
BOOL epg_data_try_lang_code_priority(void *p_data,eit_t *p_eit_info);
/*!
  init epg filter lang code evt
  */
void epg_data_filter_evt_lang_code(void *p_data,eit_t *p_eit_info);
#endif // End for _EPG_DATA_H

