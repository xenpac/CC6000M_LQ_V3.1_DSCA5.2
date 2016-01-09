/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __EPG_DATABASE_H_
#define  __EPG_DATABASE_H_

/*!
  \file epg_database.h
  
  EPG data provides the data structure of EPG data in EPG database. And the APIs 
  to access these information also be declared. 
  
  Development policy:
  To fulfill the function of EPG, EPG database should work together with 
  application EPG and DVB mid-ware module
  */

/*!
  MAX number of short event per section.
  */
#define MAX_SHT_EVT_PER_SEC            (64)

/*!
  Max number of text event per section
  */
#define MAX_TEXT_EVT_PER_SEC           (64)
  
/*!
  Max number of extend event per event.
  */
#define MAX_EXT_EVT_PER_EVT            (64) //(16) 

/*!
  Max number of content descriptor per event.
  */
#define MAX_CONT_DESC_PER_EVT          (16)

/*!
  Max service name length
  */
#define MAX_EVENT_NAME_LEN             (32)

/*!
  Max short text length
  */
#define MAX_SHT_TEXT_LEN               (256)

/*!
  Language code length
  */
#define LANGUAGE_CODE_LEN              (3)

/*!
  Max extern text length	
  */
#define MAX_EXT_TXT_LEN (256)

/*!
  parental rating descriptor
  */
typedef struct tag_parental_rating
{
  /*!
  country_code
  */
  u8 country_code[3];
  /*!
  event rating
  */
  u8 rating;
}parental_rating_t;

/*!
  Item info in extend event descriptor
  */
typedef struct
{
  /*!
    Item description, such as cast list etc.
    */
  u8 item_desc[MAX_ITEM_DESC_LENGTH];
  /*!
    Item char, such as cast name etc.
    */
  u8 item_char[MAX_ITEM_CHAR_LENGTH];
}item_infor_t;


/*!
 text of short event(detail)
 */
typedef struct tag_text_event
{
/*!
  event id
  */ 
  u16 event_id;
/*!
  Length of short event text(detail)
  */
  u16 text_len;
/*!
  Text of short event (detail)
  */
  u8 text[MAX_SHT_TEXT_LEN];
}text_event_t;


/*!
  Extend event text information
  */
typedef struct tag_extern_event
{
/*!
 event id
 */
 u32 event_id      : 16;
/*!
 descriptor number.
 */
 u32 index         : 5;
/*!
 total extern 
 */ 
 u32 tot_ext_desc  : 5;
/*!
 item number
 */
 u32 item_num      : 6;  
/*!
 Extend text length
 */
 u16 ext_txt_len;
/*!
 MAX item information
 */
 item_infor_t item[MAX_ITEM_NUMBER];
/*!
 TXT info in extend event description
 */
 u8 ext_txt[MAX_EXT_TXT_LEN];
}extern_event_t; 

/*!
 short event information
 */
typedef struct tag_short_event
{
 /*!
  Event id
  */
  u16 event_id; 
 /*!
  evt running status
  */
  u8 running_status;
 /*!
  Length of event name 
  */
  u8 name_len;
 /*!
  content nibble : High 4 bits are level 1; Low 4 bits are level 2.
  */
  u8 cont_level;
 /*!
  User nibble :  High 4 bits are nibble 1; Low 4 bits are nibble 2.
  */
  u8 user_nibble;
 /*!
  Language code
  */
  u8 lang_code[LANGUAGE_CODE_LEN];
 /*!
  Start time of event time
  */
  utc_time_t start_time;  
 /*!
  Duration of event time 
  */
  utc_time_t drt_time;
 /*!
  Event name of cerain event
  */
  u8 evt_nm[MAX_EVENT_NAME_LEN];
 /*!
  parental rating.
  */
  parental_rating_t parental_rating;
}short_event_t;

/*!
 epg eit.
 */
typedef struct tag_epg_eit
{
 /*!
  *Table id 
  */
  u8  table_id;
 /*!
  pf table
  */
  u8  pf_table;
 /*!
  Ts stream id,concern with different TP
  */
  u16 ts_id; 

 /*!
  Service id
  */
  u16 svc_id;
/*!
  section version number
  */
  u8 version_number;
 /*!
  The counter of this section in max section length
  */
  u8 section_number;
 /*!
  Max section length.
  */
  u8 last_section_number;

 /*!
  Last section number in each section
  */
  u8 seg_last_sec_number;
 /*!
  Last table id of EIT with the same service id
  */
  u8 last_table_id;
 /*!
  Id for rebroadcast other network's info
  */
  u16 org_nw_id;    

 /*!
  crc32 checksum
  */
  u32 crc_32;
 /*!
   event current time flag
   */
   BOOL message_flag;
/*!
  epg fixed event able.
  */
   BOOL evt_fixed_able;
/*!
  have pf table
  */
   BOOL have_pf_pin;
 /*!
  count short event
  */
  u16 count_sht_evt;
 /*!
  count text event
  */
  u8 count_text_evt;
 /*!
  count extern event
  */
  u8 count_ext_evt;
 /*!
  short event list.
  */
  short_event_t sht_evt_list[MAX_SHT_EVT_PER_SEC];
 /*!
  text event list.
  */ 
  text_event_t text_evt_list[MAX_TEXT_EVT_PER_SEC];
 /*!
  extern event list.
  */ 
  extern_event_t ext_evt_list[MAX_EXT_EVT_PER_EVT];
}epg_eit_t;

/*!
 epg event type
 */
 typedef enum tag_epg_event_note
{
 /*!
  present event node.
  */
  EPG_PRESENT_EVENT       = 0x01, // (0b1)
 /*!
  following event node.
  */
  EPG_FOLLOW_EVENT        = 0x02, // (0b10)
 /*!
  all p/f event node.
  */
  EPG_ALL_PF_EVENT        = 0x03, // (EPG_PRESENT_EVENT | EPG_FOLLOW_EVENT)
 /*!
  schedule event node.
  */ 
  EPG_SCHEDULE_EVENT      = 0x04, // (0b100)
 /*!
  all event node. 
  */ 
  EPG_ALL_EVENT           = 0x07, // (EPG_PRESENT_EVENT | EPG_FOLLOW_EVENT | EPG_SCHEDULE_EVENT)
/*!
  epg text_event
  */ 
  EPG_TEXT_EVENT        = 0x08, // (0x1 0000 0000)
}epg_event_note_t;

/*!
 epg event 
 */
typedef enum tag_epg_event
{
 /*!
  unknown event type
  */
  EPG_UNKNOWN_EVENT       = 0x00, 
 /*!
  all overdue event node
  */
  EPG_OVERDUE_EVENT       = 0x01, 
 /*!
  only save this network id event node(used for delete other's network_id event node.)
  */ 
  EPG_NETWORK_ID_EVENT    = 0x02,
 /*!
  only save this ts id event node(used for delete other's ts_id event node.)
  */ 
  EPG_TS_ID_EVENT         = 0x04, 
 /*!
  only save this service id event node(used for delete other's svc_id event node.)
  */ 
  EPG_SVC_ID_EVENT        = 0x08,
/*!
  epg time event
  */ 
  EPG_SVC_ID_TIME_EVENT        = 0x10, // (0x1 0000 0000)  
/*!
  fixd other program event number
  */ 
  EPG_SVC_ID_FIXD_EVENT        = 0x20, 
}epg_event_t;


/*!
  epg service node
  */
typedef struct tag_epg_svc
{
 /*!
  svc node handle 
  */ 
  void *p_node;
 /*!
  pf table
  */
  BOOL pf_table;
 /*!
  event note type.
  */
  epg_event_note_t evt_note_type;
}epg_svc_t;
 
/*!
 epg database config
 */
typedef struct tag_epg_db_cfg
{
  /*!
   attach buffer.
   */
  u8 *p_attach_buf;
 /*!
   buffer size.
   */
  u32 buffer_size;
 /*!
  event node number.
  */
  u16 evt_node_num;
 /*!
  svc node number.
  */
  u16 svc_node_num;
 /*!
  ts node number.
  */
  u16 ts_node_num; 
 /*!
  network node number.
  */
  u8 net_node_num;
 /*!
  max actual event days.
  */
  u8 max_actual_event_days;
/*!
  max other event days
  */
  u8 max_other_event_days;
 /*!
  max ext event count.
  */
  u8 max_ext_count;
 /*!
  table id list.
  */
  //eit_table_id_t  *p_tbl_id_list;
 /*!
  P/F event policy
  */
  pf_event_policy  pf_policy;
 /*!
  sch event policy 
  */ 
  sch_event_policy sch_policy;
 /*!
  network id policy 
  */
  //network_policy_t net_policy;
 /*!
  fixed sch fixed text policy 
  */ 
  sch_fixed_text_policy  fixed_sch_text_policy;
 /*!
  fixed pf fixed text policy 
  */ 
  pf_fixed_text_policy  fixed_pf_text_policy;
 /*!
  epg ext event able.
  */
  BOOL ext_able;
 /*!
  epg text event able.
  */
  BOOL text_able;
  /*!
  epg delete pf able.
  */
  BOOL del_pf_able;
  /*!
   fixed svc node number.
   */
  u32 fixed_svc_node_num;
/*!
  fixed text non fix size
  */
  u32 text_buffer_size;
}epg_db_cfg_t;

 /*!
  event id info 
  */
typedef enum tg_event_id_info
{
 /*!
  event id info  no exist
  */
  EVT_ID_EXIST_NO = 0,
/*!
  event id info  no exist lack info
  */
  EVT_ID_EXIST_LACK,
/*!
  event id info  no exist  info is FULL
  */
  EVT_ID_EXIST_FULL,
}event_id_info_t;

/*!
 epg set program info.

 \param[in] p_prog_info : program info.
 */
void epg_db_set_info(epg_prog_info_t *p_prog_info);

/*!
 epg get program info.

 \param[out] p_prog_info : program info.
 */
void epg_db_get_info(epg_prog_info_t *p_prog_info);

 
/*!
 epg database get P/F event
 */  
RET_CODE epg_db_get_pf_event(epg_prog_info_t *p_info, 
                             event_node_t    **pp_present_evt,
                             event_node_t    **pp_follow_evt);


/*!
 epg database get schedule event
 */
event_node_t *epg_db_get_sch_event(epg_prog_info_t *p_prog_info,
                                   u16             *p_event_num);

/*!
  epg database get schedule event by pos.

  \param[in] p_prog_info : program info
  \param[in] p_evt_head  : event header
  \param[in] offset      : offset value of p_evt_header 
  */
event_node_t *epg_db_get_sch_event_by_pos(epg_prog_info_t *p_prog_info,
                                          event_node_t    *p_evt_head,     
                                          u16              offset);

event_node_t *epg_db_get_event_by_nibble(epg_nibble_t *p_nibble, 
                                         u16          *p_event_num);
event_node_t * epg_db_get_pf_event_by_nibble(epg_nibble_t *p_nibble,
                                          u16 *p_event_num);

event_node_t *epg_db_get_next_event_by_nibble(epg_nibble_t *p_nibble,
                                              event_node_t *p_evt_head);

/*!
  epg db check event id.
  */
event_id_info_t epg_db_check_event_id(epg_svc_t *p_svc, u16 evt_id);
/*!
  epg db check max_evt_days.
  */
RET_CODE epg_db_check_max_evt_days_event(short_event_t  *p_sht_evt,epg_eit_t *p_epg_eit);
/*!
  epg database add svc node 
  */
RET_CODE epg_db_add_svc(epg_eit_t *p_eit, epg_svc_t *p_svc);

/*!
  epg database add event
  */
RET_CODE epg_db_add_event(epg_eit_t *p_eit, epg_svc_t *p_svc);


/*!
  epg database delete event

  \param[in] evt_type : event type.
  */
void epg_db_delete(epg_event_t evt_type);
/*!
 epg get db config help info.
 */
void epg_db_get_config_help_info(u16 net_node_num,
                                                               u16 ts_node_num,
                                                               u16 svc_node_num,
                                                               u16 evt_node_num,
                                                               u32 mem_size);
/*!
  epg database config.

  \param[in] p_cfg : config.
  */
void epg_db_config(epg_db_cfg_t *p_cfg);
/*!
  free config buffer,it malloc myself
  */
void epg_db_config_buffer_free(void);
/*!
  epg databse deinit.
  */
void epg_db_deinit(void);

/*!
  epg database init.
  */
RET_CODE epg_db_init(void);

#endif // End for __EPG_DATABASE_H_

