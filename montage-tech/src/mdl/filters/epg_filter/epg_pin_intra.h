/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EPG_PIN_INTRA_H_
#define __EPG_PIN_INTRA_H_

/*!
  eit section size
  */
#define EIT_SAMPLE_SECTION_SIZE (4*(KBYTES) + 188)

/*!
  fixed me???
  */
#define MONITOR_TICKS                 (1 * 6000) //2 minutes

/*!
  old section number
  */
#define OLD_SECTION_NUM               (5)

/*!
  eit header size.
  */
#define EIT_HEADER_SIZE               (14)

/*!
  Length of EIT descriptor head
  */
#define EIT_SEC_HEAD_LEN              (12)

/*!
  Max progran service for evt P/F	
  */
#define EIT_DVBC_MAX_PF_SERVICE_NUMBER              512
/*!
  Max progran service for evt P/F	
  */
#define EIT_DVBC_MAX_PF_SECTION_NUMBER              2

/*!
  Max progran service for evt P/F	
  */
#define EIT_DVBC_MAX_PF_SAME_SEC_FIND_NUM              2
/*!
  EIT same receive time out	
  */
#define PF_CONFIG_DEFAULT_TIME_OUT            2000 /***xms***/
/*!
  epg section number record
  */
typedef struct 
{
/*!
  The section number
  */
  u8 section_number;
/*!
  The section number exist,TRUE :exist
  */  
  BOOL exist_bit;
/*!
  The same section  find number;
  */  
  u8 find_number;
}epg_record_section_t;
/*!
  epg section record
  */
 typedef struct 
{
/*!
  *Table id 
  */
  u8  table_id;
 /*!
  Id for rebroadcast other network's info
  */
  u16 org_nw_id;   
 /*!
  Ts stream id,concern with different TP
  */
  u16 ts_id; 
 /*!
  Service id
  */
  u16 svc_id;
 /*!
  The counter of this section in max section length
  */
  epg_record_section_t section_rec[EIT_DVBC_MAX_PF_SECTION_NUMBER];
 /*!
  Max section length.
  */
  u8 last_section_number;
 /*!
  section version number
  */
  u8 version_number;
}epg_record_service_t;
/*!
  epg section record
  */
 typedef struct 
{
  /*!
  epg section record table
  */
  epg_record_service_t service_rec[EIT_DVBC_MAX_PF_SERVICE_NUMBER];
/*!
  epg service total number
  */
  u32 service_total_number;
/*!
  the same service id up time
  */
  BOOL finish_last_flag;
/*!
  the finish flag;
  */
  BOOL finish_flag;
/*!
  the finish flag;
  */
  BOOL record_able;
/*!
  epg pf table request timeout.
  */
 u32 pf_timeout;
/*!
  epg eit table request timeout.
  */
 u32 eit_timeout;
}epg_record_sec_t;

/*!
 eit array 
 */
typedef struct tag_eit_array
{
 /*!
  eit table id 
  */ 
  eit_table_id_t table_id;
 /*!
  dmx buffer
  */
  u8 *p_buffer;
 /*!
   table free status
   */
   BOOL free_flag;
}eit_array_t;

/*!
 epg pin private data.
 */
typedef struct tag_epg_pin_priv
{
/*!
 epg pin handle.  
 */  
 void *p_this;
/*!
 dmx interface pointer.
 */
 dmx_interface_t *p_dmx_inter;
/*!
 attach buffer.(if attach_buf_size == 0, attach buffer from intra malloc, must free in the destory.)
 */
 u8 *p_attach_buf;
/*!
 attach buffer size(default == 0, this buffer from intra malloc)
 */
 u32 attach_buf_size;
/*!
 epg eit
 */
 epg_eit_t epg_eit;
/*!
 epg svc
 */
 epg_svc_t epg_svc;
/*!
 program info.
 */  
 epg_prog_info_t prog_info;
/*!
 is request eit data.
 */
 BOOL is_requested;
/*!
 array depth = max table id num;
 */
 u8 pf_array_depth;
/*!
 table id list.
 */
 eit_array_t *p_pf_array;
/*!
 array depth = max table id num;
 */
 u8 sch_array_depth;
/*!
 table id list.
 */
 eit_array_t *p_sch_array;
/*!
 *  network policy
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
 monitor ticks
 */
 u32 monitor_ticks;
/*!
 epg pf timeout ticks
 */
 u32 timeout_pf_ticks;
/*!
 epg eit receiver timeout ticks
 */
 u32 timeout_ticks;
/*!
 epg eit timeout from config
 */
 u32 eit_timeout;
/*!
  epg_record  pf section 
  */
  epg_record_sec_t pf_sec_record;
/*!
  epg fixed event able.
  */
   BOOL evt_fixed_able;
/*!
  have pf pin
  */
   BOOL have_pf_pin;
/*!
  epg task release time,value * ms;if it is 0,don't take effect!
  it  supply eva too fast,but ap is tool slow;it is used jazz plat,
  it will solve by eva oneself
  */
  u32 task_release_time;
/*!
  request data
  */
 psi_request_data_t req_data ;
/*!
  free data
  */
 psi_free_data_t free_data ;
/*!
  free data
  */
os_msg_t msg;
/*!
  private parse buffer
  */
u8  *p_parse_buffer;
/*!
  spin filter size on multi mode.
 */
 u32 spin_multi_buffer_size;
/*!
 msg send cycle,when is 0,the msg monitor is failure
 */
 u32 msg_send_cycle;
/*!
 msg monitor ticks
 */
 u32 msg_monitor_ticks;
/*!
  psi chart list
  */
  os_msg_t *p_msg_list;

}epg_pin_priv_t;

/*!
 epg pin 
 */
typedef struct tag_epg_pin
{
 /*!
  base class.
  */  
  FATHER sink_pin_t m_sink_pin;
 /*!
  private data.
  */
  epg_pin_priv_t    m_priv_data;
 /*!
  config filter.
  */ 
  void (*config)(handle_t _this, epg_filter_cfg_t *p_cfg);
 /*!
  set info(dynamic, can set inforamtion between start and stop.)
  */
  void (*set_info)(handle_t _this, epg_prog_info_t *p_data);
 /*!
  active loop.
  */
  void (*active_loop)(handle_t _this);
}epg_pin_t;


/*!
 create epg pin

 \param[in] p_pin : epg pin handle.
 \param[in] p_owner : epg pin's owner.
 \return epg_pin_t handle
 */
epg_pin_t *epg_pin_create(epg_pin_t *p_pin, interface_t *p_owner);

#endif //End for __EPG_PIN_INTRA_H_
