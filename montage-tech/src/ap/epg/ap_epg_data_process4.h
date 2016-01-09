/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_EPG_DATA_PROCESS4_H_
#define __AP_EPG_DATA_PROCESS4_H_

/*!
  Max PTI SLOT NUMBER
  */
#define PTI_SLOT_NUM_AVAIL    (3)
/*!
  Max EIT TABLE NUMBER
  */
#define MAX_TABLE_CNT  (5)
/*!
  table 0x4e index
  */
#define EIT_ACTUAL_INDEX (0)
/*!
  table 0x50 index
  */
#define EIT_SCH_INDEX (1)
/*!
  table 0x51 index
  */
#define EIT_SCH_ACTUAL_51 (2)

/*!
  table status
  */
typedef struct 
{
  //id of current table
  u8 table_id;
  //table requested or not
  BOOL table_is_requested;
  //True -> table id is requested
  BOOL use_st;
  //Time tick when last requested
  u32 tick_requsted;
  //Time tick when last freed
  u32 tick_freed;
  //Data receive buffer
  pti_ext_buf_t *p_buf_addr;
}tab_status_t;


/*!
  EPG process data
  */
typedef struct 
{
  tab_status_t default_tab_map[MAX_TABLE_CNT];
  //EPG DB status 
  epg_db_status_t epg_dbg_st;
  //Is first start
  BOOL is_active;
  //service 
  service_t *p_svc;
  //Last time when system send out a message 
  utc_time_t last_time;
  //Monitor EIT 
  BOOL is_eit_mon;
  //Occuppied PTI number
  BOOL pti_num_ocup;
  //Captured section number
  u32 captured_eit_num; 
  //EPG DB handle
  void *p_epg_db_handle;
}proc_para_t;

/*!
  Process on the state of table requested
  \param[in] p_data epg process private data
  */
u8 epg_process_on_tab_req4(void *p_data, epg_policy_t *p_epg_impl);
/*!
  Process on the state of free table 
  \param[in] p_data epg process private data
  \param[out] tab number freed
  */
u8 epg_process_on_tab_free_all4(void *p_data, epg_policy_t *p_epg_impl);
/*!
  EPG process free one table
  \param[in] p_data epg process private data
  \param[out] Freed pti ext buffer 
  */
pti_ext_buf_t *epg_process_on_tab_free4(void *p_data,
epg_policy_t *p_epg_impl);

/*!
  EPG process request one table
  \param[in] p_data epg process private data
  \param[in] p_ext_buf pti ext buffer for request new table
  \param[out] return TRUE when new table is requested
              FALSE when no new table is requested
  */
BOOL epg_process_on_tab_req_one4(void *p_data,pti_ext_buf_t *p_ext_buf);

/*!
  EPG process start 
  \param[in] p_data epg process private data
  */
void epg_process_on_start4(void *p_data);

/*!
  Reset all tab status into default status
  \param[in] p_data epg process private data
  */
void epg_process_tab_status_reset4(void *p_data);

/*!
  EPG process stop
  \param[in] p_data epg process private data
  */
void epg_process_on_stop4(void *p_data);

/*!
  EPG process on version update
  \param[in] p_data epg process private data
  */
void epg_process_on_ver_update4(void *p_data);


/*!
  request a EIT section/pti slot
  */
void request_eit_section4(service_t *p_svc, u32 table_id,pti_ext_buf_t *p_ext_buf_list, u8 ts_in);

/*!
  free a EIT section/pti slot
  */
void free_eit_section4(service_t *p_svc, u32 table_id,  pti_ext_buf_t *p_ext_buf_list);

/*!
  EPG process contruct
  \param[out] epg process handle after EPG construction
  */
void *epg_process_construct4(service_t *p_svc);

/*!
  EPG process destruct 
  \param[in] EPG handle for destruction
  */
void epg_process_destruct4(void *p_handle);
#endif // End for __AP_EPG_H_

