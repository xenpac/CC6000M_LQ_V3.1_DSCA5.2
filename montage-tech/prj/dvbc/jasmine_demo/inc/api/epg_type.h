/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EPG_TYPE_H_
#define __EPG_TYPE_H_

/*!
  Max service name length
  */
#define MAX_EVT_NAME_LENGTH        (32)

/*!
  Language code length
  */
#define LANGUAGE_CODE_LENGTH       (3)

/*!
  MAX length to item description
  */
#define MAX_ITEM_DESC_LENGTH       (16)

/*!
  MAX length to item content
  */
#define MAX_ITEM_CHAR_LENGTH       (16)

/*!
  MAX item number
  */
#define MAX_ITEM_NUMBER            (3)

/*!
  ignore id.(network_id or ts_id or svc_id)
  */
#define EPG_IGNORE_ID              (0XFFFF)

/*!
 all dvb eit table id
 */
typedef enum tag_eit_table_id
{
 /*!
  EIT table id for actual stream with PF info
  */
  EIT_TABLE_ID_PF_ACTUAL      = 0x4E,
 /*!
  EIT table id for actual stream with PF info
  */
  EIT_TABLE_ID_PF_OTHER       = 0x4F,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_50  = 0x50,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_51  = 0x51,
  /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_52  = 0x52,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_53  = 0x53,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_54  = 0x54,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_55  = 0x55,
   /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_56  = 0x56,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_57  = 0x57,
   /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_58  = 0x58,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_59  = 0x59,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_5A  = 0x5A,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_5B  = 0x5B,
   /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_5C  = 0x5C,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_5D  = 0x5D,
   /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_5E  = 0x5E,
 /*!
  EIT table id for actual stream with schedule info
  */
  EIT_TABLE_ID_SCH_ACTUAL_5F  = 0x5F,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_60   = 0x60,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_61   = 0x61,
  /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_62   = 0x62,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_63   = 0x63,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_64   = 0x64,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_65   = 0x65,
   /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_66   = 0x66,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_67   = 0x67,
   /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_68   = 0x68,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_69   = 0x69,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_6A   = 0x6A,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_6B   = 0x6B,
   /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_6C   = 0x6C,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_6D   = 0x6D,
   /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_6E   = 0x6E,
 /*!
  EIT table id for other stream with schedule info
  */
  EIT_TABLE_ID_SCH_OTHER_6F   = 0x6F,
 /*!
  EIT table id end
  */ 
  EIT_TABLE_ID_END,
}eit_table_id_t;

/*!
 *define eit table selecte policy
 */
 typedef enum tag_epg_table_selecte_policy
{
/*!
 *EPG selecte unkown
 */
  EPG_TABLE_SELECTE_UNKOWN,
/*!
 *EPG selecte all pf table
 */
  EPG_TABLE_SELECTE_PF_ALL,
/*!
 *EPG selecte actual pf table
 */
  EPG_TABLE_SELECTE_PF_ACTUAL,
/*!
 *EPG selecte other pf table
 */
  EPG_TABLE_SELECTE_PF_OTHER,
 /*!
 *EPG selecte PF ALL AND SCH ACTUAL
 */
  EPG_TABLE_SELECTE_PF_ALL_SCH_ACTUAL, 
/*!
 *EPG selecte all sch table
 */
  EPG_TABLE_SELECTE_SCH_ALL,
/*!
 *EPG selecte actual sch table
 */
  EPG_TABLE_SELECTE_SCH_ACTUAL,
/*!
 *EPG selecte other sch table
 */
  EPG_TABLE_SELECTE_SCH_OTHER,
/*!
 *EPG selecte other sch table
 */
  EPG_TABLE_SELECTE_FROM_CONFIG,
}epg_table_selecte_policy;
 
/*!
 *define network policy
 */
typedef enum tag_network_policy
{
  /*!
  * network, default . 
  */
  NETWORK_ID_UNKNOWN,
 /*!
  * network ignore, in the DVB-C, only one network. 
  */
  //NETWORK_ID_IGNORE, /***make sure mix is 2 network***/
 /*!
  * network id normal.(for DVB-S)
  */ 
  NETWORK_ID_NORMAL,
}network_policy_t;

/*!
 define P/F event policy
 */
typedef enum tag_pf_event_policy
{
 /*!
   default pf event .
  */
  PF_EVENT_UNKNOWN,
 /*!
  save all pf event .
  */
  PF_EVENT_ALL,
 /*!
  current network id's pf event.
  */ 
  PF_EVENT_OF_NETWORK_ID,
 /*!
  current transport stream id's pf event.
  */ 
  PF_EVENT_OF_TS_ID,
 /*!
  current service id's pf event.
  */ 
  PF_EVENT_OF_SVC_ID,
 /*!
  not P/F event .
  */ 
  PF_EVENT_NOT,
}pf_event_policy;

/*!
 define schedule event policy
 */
typedef enum tag_sch_event_policy
{
  /*!
  default schedule event .
  */
  SCH_EVENT_UNKNOWN,
 /*!
  save all schedule event .
  */
  SCH_EVENT_ALL,
 /*!
  current network id's schedule event.
  */ 
  SCH_EVENT_OF_NETWORK_ID,
 /*!
  current transport stream id's schedule event.(default.)
  */ 
  SCH_EVENT_OF_TS_ID,
 /*!
  current service id's schedule event.
  */ 
  SCH_EVENT_OF_SVC_ID,
/*!
  current service id's schedule event.
  */ 
  SCH_EVENT_OF_TIME_ID,
}sch_event_policy;

/*!
 define schedule fixed text policy
 */
typedef enum tag_sch_fixed_text_policy
{
 /*!
  no fixed sch text policy.
  */
  SCH_FIXED_TEXT_NO_ID,
 /*!
  current network id's schedule event.
  */ 
  SCH_FIXED_TEXT_OF_NET_ID,
 /*!
  current transport stream id's schedule event.(default.)
  */ 
  SCH_FIXED_TEXT_OF_TS_ID,
 /*!
  current service id's schedule event.
  */ 
  SCH_FIXED_TEXT_OF_SVC_ID,
   /*!
  current time service id's schedule event.
  */ 
  SCH_FIXED_TEXT_OF_TIME_ID,
}sch_fixed_text_policy;

/*!
 define schedule fixed text policy
 */
typedef enum tag_pf_fixed_text_policy
{
 /*!
  no fixed sch text policy.
  */
  PF_FIXED_TEXT_NO_ID,
 /*!
  current network id's schedule event.
  */ 
  PF_FIXED_TEXT_OF_NET_ID,
 /*!
  current transport stream id's schedule event.(default.)
  */ 
  PF_FIXED_TEXT_OF_TS_ID,
 /*!
  current service id's schedule event.
  */ 
  PF_FIXED_TEXT_OF_SVC_ID,
}pf_fixed_text_policy;

/*!
 event attribute type
 */
typedef enum tag_event_attr
{
/*!
 event is belong to the current program.
 */  
 EVENT_ATTR_CURRENT_PROG,
/*!
 event is belong to the others prorgrams. 
 */  
 EVENT_ATTR_OTHERS_PROG, 
}event_attr_t;


/*!
  define epg program info
  */
typedef struct tag_epg_prog_info
{
 /*!
  network id.
  */ 
  u16 network_id;
 /*!
  ts id.
  */
  u16 ts_id;
 /*!
  svc id.
  */
  u16 svc_id;
 /*!
  start time
  */
  utc_time_t start_time;
 /*!
  end time
  */
  utc_time_t end_time;
}epg_prog_info_t;

/*!
  define epg nibble info
  */
typedef struct tag_epg_nibble
{
 /*!
  program info 
  */ 
  epg_prog_info_t prog_info;
 /*!
  content nibble level
  */
  u8 nibble_level;
 /*!
  user nibble
  */
  u8 user_nibble;
  /*!
  user nibble in pf data
  */
  BOOL nibble_in_pf;
}epg_nibble_t;


/*!
  Item info in extend event descriptor by unicode format
  */
typedef struct tag_item_info_unicode
{
  /*!
    Item description, such as cast list etc.
    */
  u16 item_desc[MAX_ITEM_DESC_LENGTH];
  /*!
    Item content, such as cast name etc.
    */
  u16 item_char[MAX_ITEM_CHAR_LENGTH];
}item_info_unicode_t;

/*!
  EPG event desc info in unicode format
  */
typedef struct tag_ext_desc
{
 /*!
  extern text length
  */
  u16 ext_text_len;
 /*!
  extern text(detail)
  */
  u16 *p_ext_text; 
 /*!
  MAX item information
  */
  item_info_unicode_t item[MAX_ITEM_NUMBER];
 /*!
  Item number
  */
  u8  item_num;
}ext_desc_t;


/*!
  Event node information in EPG database 
  */
typedef struct tag_event_node
{
 /*!
  Event start time
  */
  utc_time_t start_time;
 /*!
  Event duration
  */
  utc_time_t drt_time;
 /*! 
  Event name
  */
  u16 event_name[MAX_EVT_NAME_LENGTH];
 /*!
  Language code
  */
  u16 lang_code[LANGUAGE_CODE_LENGTH];
 /*!
  content nibble : High 4 bits are level 1; Low 4 bits are level 2.
  */
  u8 cont_level;
 /*!
  User nibble :  High 4 bits are nibble 1; Low 4 bits are nibble 2.
  */
  u8 user_nibble;
 /*!
  Event id
  */
  u32 event_id        : 16;  
 /*!
  short text length.
  */
  u32 sht_text_len    : 11;
 /*!
  reserved
  */
  u32 count_ext_desc  : 5;
 /*!
  text  (detail, and record alloc buffer address.)
  */
  u16 *p_sht_text;
 /*!
  extern event descrtion(detail) list
  */ 
  ext_desc_t *p_ext_desc_list;
 /*!
  Next event node
  */
  struct tag_event_node *p_next_evt_node; 
}event_node_t;

#endif //End for __EPG_TYPE_H_
