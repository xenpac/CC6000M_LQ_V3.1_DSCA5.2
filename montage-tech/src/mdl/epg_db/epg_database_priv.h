/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EPG_DATABASE_PRIV_H__
#define __EPG_DATABASE_PRIV_H__
 

/*!
  is present event node
  */
#define IS_PRESENT_EVENT(x)       ((x) & (EPG_PRESENT_EVENT))

/*!
  is follow event node
  */
#define IS_FOLLOW_EVENT(x)        ((x) & (EPG_FOLLOW_EVENT))

/*!
  is p/f event node
  */
#define IS_PF_EVENT(x)            ((x) & (EPG_ALL_PF_EVENT))

/*!
  is schedule event node
  */
#define IS_SCHEDULE_EVENT(x)      ((x) & (EPG_SCHEDULE_EVENT))

/*!
  is all event node
  */
#define IS_ALL_EVENT(x)           ((x) & (EPG_ALL_EVENT))

/*!
  is overdue event node
  */
#define IS_OVERDUE_EVENT(x)         ((x) & (EPG_OVERDUE_EVENT))

/*!
  is network_id's event node
  */
#define IS_NET_ID_EVENT(x)          ((x) & (EPG_NETWORK_ID_EVENT))

/*!
  is not only network_id's event node
  */
#define ONLY_NET_ID_EVENT(x)        ((x) == (EPG_NETWORK_ID_EVENT))

/*!
  clear network id bit.
  */
#define CLEAR_NET_ID_BIT(x)         ((x) & (~ EPG_NETWORK_ID_EVENT))

/*!
  is ts_id's event node
  */
#define IS_TS_ID_EVENT(x)           ((x) & (EPG_TS_ID_EVENT))

/*!
  is not only network_id's event node
  */
#define ONLY_TS_ID_EVENT(x)         ((x) == (EPG_TS_ID_EVENT))

/*!
  clear network id bit.
  */
#define CLEAR_TS_ID_BIT(x)          ((x) & (~ EPG_TS_ID_EVENT))

/*!
  is ts_id's event node
  */
#define IS_SVC_ID_EVENT(x)          ((x) & (EPG_SVC_ID_EVENT))
/*!
  is svc time s event node
  */
#define IS_SVC_ID_TIME_EVENT(x)          ((x) & (EPG_SVC_ID_TIME_EVENT))

/*!
  is not only network_id's event node
  */
#define ONLY_SVC_ID_EVENT(x)     ((x) == (EPG_SVC_ID_EVENT))

/*!
  clear network id bit.
  */
#define CLEAR_SVC_ID_BIT(x)         ((x) & (~ EPG_SVC_ID_EVENT))

/*!
  fixd other svc's event id number
  */
  #define FIXD_SVC_ID_EVENT(x)          ((x) & (EPG_SVC_ID_FIXD_EVENT))
/*!
  is epg text's event 
  */
#define IS_EPG_TEXT_EVENT(x)          ((x) & (EPG_TEXT_EVENT))

/*!
  Service node saved into EPG database
  */
typedef struct svc_node
{
 /*!
  Service id
  */
  u32 svc_id      : 16;
 /*!
  is new svc node
  */
  u32 is_new_node    : 1;
 /*!
  reserved
  */
  u32 reserved    : 15;
 /*!
  event node head.
  */
  event_node_t    *p_sch_evt_head;
 /*!
  present event node pointer.
  */
  event_node_t    *p_present_evt;
 /*!
  following event node pointer.
  */
  event_node_t    *p_follow_evt;
 /*!
  Pointing to next service node of parent ts node
  */
  struct svc_node *p_next_svc_node;
}svc_node_t;

/*!
  Transport stream node
  */
typedef struct ts_node
{
 /*!
  Transort stream id
  */
  u16 ts_id;
 /*!
  Service head
  */
  svc_node_t *p_svc_head;
 /*!
  Next ts node
  */
  struct ts_node *p_next_ts;
}ts_node_t;

/*!
  Network node logically used for fast data searching operation
  */
typedef struct network_node
{
 /*!
  Orginal network id
  */
  u16 network_id;
 /*!
  Ts stream node head
  */
  ts_node_t *p_ts_head;
 /*!
  Next network node
  */
  struct network_node *p_next_net; 
}network_node_t;
/*!
 EPG database global parameter
 */
typedef struct tag_epg_db_priv
{
/*!
  max other event days
  */
  u8 max_other_event_days;
/*!
  max actual event days(max <= 64)
*/
  u8 max_actual_event_days;
/*!
  reserved
  */
  u16 evt_used_node_num;
/*!
  reserved1
  */
  u32 reserved1 : 7;
 /*!
  network node number.
  */
  u32 net_node_num : 3;
 /*!
  ts node number.
  */
  u32 ts_node_num : 16; 
  /*!
  event node number.
  */
  u32 evt_node_num : 16;
 /*!
  svc node number.
  */
  u32 svc_node_num : 16;
 /*!
   fixed svc node number.
   */
  u32 fixed_svc_node_num;
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
  Network node head
  */
  network_node_t *p_net_head;
 
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
  program info.
  */
  epg_prog_info_t  prog_info;
 /*!
  db_cfg_buf,if malloc by myself,it meed to free myself;
  */
 u8 *p_db_cfg_buf;
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
  epg data base sem.
  */
  os_sem_t epg_data_base_sem;
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
  u8 set_lang_code[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN];
/*!
  first epg lang code
  */
  u8 first_lang_code[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN];
/*!
  second lang code
  */
  u8 second_lang_code[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN];
/*!
  default epg lang code
  */
  u8 default_lang_code[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN];
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
  eit edit id call back
  */
  eit_edit_id_call_back_t eit_edit_id_call_back;
/*!
  eit edit info call back
  */
  eit_edit_info_call_back_t eit_edit_info_call_back;
}epg_db_priv_t;


#endif //End files.

