/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EPG_FILTER_H_
#define __EPG_FILTER_H_

/*!
  eit buffer size.
  */
#define EIT_BUFFER_SIZE               ((32) * (KBYTES))

/*!
  config epg filter buffer size.
  */
#define TOTAL_EPG_FILTER_BUF_SIZE(x)  ((x) * (EIT_BUFFER_SIZE))

/*!
 epg filter config
 */
typedef struct tag_epg_filter_cfg
{
/*!
 attach buffer.
 */
 u8 *p_attach_buf;
/*!
 buffer size. = max_eit_table_num * spin_multi_buffer_size
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
  epg pf table request timeout.
  */
 u32 pf_timeout;
/*!
  epg eit table request timeout.
  */
 u32 eit_timeout;
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
/*!
  spin filter size on multi mode.
 */
 u32 spin_multi_buffer_size;
/*!
 msg send cycle,when is 0,the msg monitor is failure
 */
 u32 msg_send_cycle;
}epg_filter_cfg_t;

/*!
 defines epg filter command.
 */
typedef enum epg_filter_cmd
{
/*!
  config, before start.
  */
  EPG_FILTER_CMD_CONFIG,
/*!
  set eit filter info. (this cmd is dynamic, can set before start and set after start.)
  */
  EPG_FILTER_CMD_SET_INFO,
}epg_filter_cmd_t;

/*!
  defines epg filter event. 
  */
typedef enum tag_epg_filter_evt
{
/*!
  epg filter unknow schedule event .
  */
  EPG_FILTER_UNKNOW_EVENT = EPG_FILTER << 16,
 /*!
  epg filter new current schedule event .
  */
  EPG_FILTER_SCHE_EVENT,
  /*!
    schedule new other program event
    */
  EPG_FILTER_SCHE_EVENT_OTHER,
 /*!
  epg filter new P/F event. 
  */ 
  EPG_FILTER_PF_EVENT,
/*!
  epg filter new P/Fother program event
  */
  EPG_FILTER_PF_EVENT_OTHER, 
 /*!
  epg filter delete event.
  */ 
  EPG_FILTER_DEL_EVENT,
/*!
  epg filter PF event receiver finish and up data.
  */ 
  EPG_FILTER_PF_FINISH_EVENT,
/*!
  epg filter PF event receiver finish and up data.
  */ 
  EPG_FILTER_PF_TIMEOUT_EVENT,
/*!
  epg filter PF event receiver finish and up data.
  */ 
  EPG_FILTE_EIT_TIMEOUT_EVENT,
  
  
  /*!
  epg filter max evnt,if add event,add before it.
  */ 
  EPG_FILTE_EIT_MAX_EVENT,
    
}epg_filter_evt_t;


/*!
 create epg filter.
 */
ifilter_t *epg_filter_create(void *p_para);
#endif //End for __EPG_FILTER_H_
