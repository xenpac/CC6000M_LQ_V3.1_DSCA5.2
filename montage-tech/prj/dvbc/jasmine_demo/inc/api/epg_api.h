/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EPG_API_H_
#define __EPG_API_H_

/*!
  mul_err_epg_module
  */
#define MUL_ERR_EPG_MODULE                    (0x8070)

/*!
  Application music player evt enum
  */
typedef enum tag_epg_api_evt
{
 /*!
  schedule event  
  
  */
  EPG_API_SCHE_EVENT = ((APP_EPG << 16) | ASYNC_EVT),
 /*!
  P/F event
  */ 
  EPG_API_PF_EVENT,
 /*!
  delete event 
  */
  EPG_API_DEL_EVENT,
 /*!
  PF EIT UPDATA OR FINISH event 
  */
  EPG_API_PF_SECTION_FINISH,
/*!
  PF EIT TIMEOUT 
  */
  EPG_API_PF_SECTION_TIMEOUT,
/*!
   EIT TIMEOUT 
  */
  EPG_API_EIT_TIMEOUT,
}epg_api_evt_t;

/*!
 epg config 
 */
typedef struct tag_epg_config
{
/*!
  attach buffer . 
  */
  u8 *p_attach_buf;
/*!
  attach buffer size.
  */  
  u32 buffer_size;
/*!
  has P/F event, if not has P/F event, please get schedule event (for DVB-C).
  default has_pf_evt = TRUE, this network has P/F event.
  */
  BOOL has_pf_evt;

/*!
  max eit table number.
  */
 // u8 max_eit_table_num;  
/*!
  table id list.
  */
 // eit_table_id_t   *p_tbl_id_list;
/*!
  P/F event policy
  */
  pf_event_policy   pf_policy;
/*!
  sch event policy 
  */ 
  sch_event_policy  sch_policy;
/*!
  network id policy 
  */
  //network_policy_t  net_policy;

/*!
  fixed sch fixed text policy 
  */ 
  sch_fixed_text_policy  fixed_sch_text_policy;
/*!
  fixed pf fixed text policy 
  */ 
  pf_fixed_text_policy  fixed_pf_text_policy;
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
  max other event days.
  */
  u8 max_other_event_days;
/*!
  max ext event count.
  */
  u8 max_ext_count;
/*!
  config make open to project,if config_open == TURE,the config from project,else ,it will auto config by epg api.
  */
  BOOL config_open;
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
}epg_config_t;

/*!
 epg filter config
 */
typedef struct tag_epg_filter_ply
{
/*!
 attach buffer.
 */
 u8 *p_attach_buf;
/*!
 buffer size. = TOTAL_EPG_FILTER_BUF_SIZE
 */
 u32 buffer_size;
/*!
 max eit table number.
 */
 u8 max_eit_table_num;  
/*!
 table id list.
 */
 eit_table_id_t   *p_tbl_id_list;
/*!
  epg table selecte policy.
  */
  epg_table_selecte_policy epg_selecte;
/*!
  epg pf table request timeout.
  */
 u32 pf_timeout;  /**ms***/
/*!
  epg eit table request timeout.
  */
 u32 eit_timeout; /**ms***/
  /*!
 network id policy.
 */
 network_policy_t  net_policy; 
/*!
 P/F event policy
 */
 pf_event_policy   pf_policy;
/*!
 sch event policy 
 */ 
 sch_event_policy  sch_policy;
/*!
 program info.
 */  
 epg_prog_info_t prog_info;
/*!
  epg fixed event able.
  */
   BOOL evt_fixed_able;
/*!
  epg task release time,value * ms;if it is 0,don't take effect!
  it  supply eva too fast,but ap is tool slow;it is used jazz plat,
  it will solve by eva oneself
  */
  u32 task_release_time;  
}epg_filter_ply_t;

/*!
  event call back
  */
typedef void(*epg_evt_call_back)(u32 content, u32 para1, u32 para2);

/*!
  epg api init 
  */
typedef struct tag_epg_init_para
{
 /*!
  task priority.
  */ 
  u32 task_priority;
 /*!
  task stack size.
  */ 
  u32 task_stk_size;
 /*!
   epg evt call back
   */
 epg_evt_call_back evt_process;  
}epg_init_para_t;

/*!
 epg set program info.
  */
void mul_epg_set_filter_info(epg_prog_info_t *p_prog_info);

/*!
 epg set db info.
  */
void mul_epg_set_db_info(epg_prog_info_t *p_prog_info);
/*!
 epg get pf event
  */
RET_CODE mul_epg_get_pf_event(epg_prog_info_t *p_prog_info,
                              event_node_t    **pp_present_evt,
                              event_node_t    **pp_follow_evt);


/*!
 epg get schedule event 
 \return SUCCESS, represent have event nodes.
         FAILE, represent not have event nodes.

 \param[in] p_prog_info : program information
 \param[in] p_event : schedule event information
 */
event_node_t *mul_epg_get_sch_event(epg_prog_info_t *p_prog_info,                   
                                    u16             *p_event_num);

/*!
 epg get schedule event 
 \return SUCCESS, represent have event nodes.
         FAILE, represent not have event nodes.

 \param[in] p_prog_info : program information
 \param[in] p_event     : schedule event header
 \param[in] u16         : 
 */
event_node_t *mul_epg_get_sch_event_by_pos(epg_prog_info_t *p_prog_info,
                                           event_node_t    *p_evt_head,
                                           u16              pos);

/*!
  epg get nibble
  */
event_node_t *mul_epg_get_event_by_nibble(epg_nibble_t *p_nibble, 
                                          u16          *p_event_num);
/*!
  epg get next nibble
  */
event_node_t *mul_epg_get_next_event_by_nibble(epg_nibble_t *p_nibble, 
                                               event_node_t *p_evt_head);
/*!
  epg open
  */
void mul_epg_open(void);
/*!
  epg stat
  */
void mul_epg_start(void);
/*!
  epg stop
  */
void mul_epg_stop(void);
/*!
  epg close
  */
void mul_epg_close(void);
/*!
  config filter prolicy
  */
RET_CODE mul_epg_filter_policy(epg_filter_ply_t *policy);
/*!
  config help info
  */
void mul_epg_get_config_help_info(u16 net_node_num,
                                                               u16 ts_node_num,
                                                               u16 svc_node_num,
                                                               u16 evt_node_num,
                                                               u32 mem_size);
/*!
 config

 \param[in] p_config : config information
 */
RET_CODE mul_epg_config(epg_config_t *p_config);


/*!
 epg deinit.
 */
RET_CODE mul_epg_deinit(void);

/*!
  epg init. 
  */
RET_CODE mul_epg_init(epg_init_para_t *p_para);

#endif //end for __EPG_API_H_
