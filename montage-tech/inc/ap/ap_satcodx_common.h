/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SATCODX_COMMON_H_
#define __AP_SATCODX_COMMON_H_

/*!
  \file ap_satcodx_common.h
  This is the declearation of common types in Satcodx app.  
*/

/*!
    Satcodx debug marco
    */
#define SATCODX_DBG(x)
/*!
   Max name lenght supported in Satcodx APP
   */
#define AP_SATCODX_NAME_MAX_LENGTH 16

/*!
  Satcodx ap state machine
  */
typedef enum
{
  /*!
    do nothing when satcodx ap is in the idle state
    */
  SATCODX_SM_IDLE = 0,
  /*!
    check com whether there's coming data
    */
  SATCODX_SM_WAIT_DATA,
  /*!
    receive data continuously until there's no coming data during the timeout interval
    */
  SATCODX_SM_RECV_DATA,
  /*!
    receive data whether there's coming data
    */
  SATCODX_SM_PARSE_DATA,
  /*!
    do nothing when satcodx ap finishes parsing data
    */
  SATCODX_SM_ALL_FINISH,
} satcodx_sm_t;

/*!
   notify ui framework which state satcodx app is in.
   */
typedef enum
{  
  /*!
    countdown status
    */
  SATCODX_STATUS_COUNTDOWN = 0,
  /*!
    timeout status
    */
  SATCODX_STATUS_TIMEOUT,
  /*!
    receive update status
    */
  SATCODX_STATUS_RECEIVE_UPDATE,
  /*!
    receive end status
    */
  SATCODX_STATUS_RECEIVE_END,
  /*!
    update channel status
    */
  SATCODX_STATUS_UPDATE_CHANNEL,
  /*!
    reach max channel status
    */
  SATCODX_STATUS_REACH_MAX_CHANNEL,
  /*!
    finish status
    */
  SATCODX_STATUS_FINISHED,
} satcodx_status_t;

/*!
  Satcodx tp node
  */
typedef struct satcodx_tp_node_tag {
  /*!
    tp id 
    */
  u16 id;
  /*!
    tp polarity
    */
  u16 polarity;  
  /*!
    tp frequency
    */
  u32 freq;
  /*!
    tp symbol
    */
  u32 sym;
  /*!
    pointer to next tp node
    */
  struct satcodx_tp_node_tag *p_next_tp_node;
}satcodx_tp_node_t;

/*!
  Satcodx satellite node
  */
typedef struct satcodx_sat_node_tag {
  /*!
    satellite id
    */
  u16 id;
  /*!
    satellite name
    */
  u16  name[AP_SATCODX_NAME_MAX_LENGTH];
  /*!
    tp count in current satellite
    */
  u32 cur_tp_node_cnt;
  /*!
    pointer to tp node list
    */
  satcodx_tp_node_t *p_tp_node_list;
  /*!
    pointer to last tp node in tp list
    */
  satcodx_tp_node_t *p_last_node_in_tp_list;
  /*!
    pointer to next satellite node
    */
  struct satcodx_sat_node_tag *p_next_sat_node;
}satcodx_sat_node_t;

/*!
  Satcodx result list
  */
typedef struct satcodx_result_list_tag
{
  /*!
    satellite node count
    */
  u32 cur_sat_node_cnt;
  /*!
    pointer to satellite node list
    */
  satcodx_sat_node_t * p_sat_node_list;
  /*!
    pointer to last satellite node in satellite list
    */
  satcodx_sat_node_t * p_last_node_in_sat_list;
}satcodx_result_list_t;

/*!
  Satcodx satellite list from original database
  */
typedef struct
{
  /*!
    satellite view id
    */
  u8 sat_view_id;
  /*!
    tp view id
    */
  u8 tp_view_id;
  /*!
    program view id
    */
  u8 prog_view_id;
  /*!
    satellite count in current db
    */
  u32 cur_db_sat_cnt;
  /*!
    pointer to satellite list
    */
  sat_node_t *p_sat_list;
}satcodx_db_sat_list_t;

/*!
  Satcodx update channel parameter
  */
typedef struct
{
  /*!
    satellite id
    */
  u32 sat_id;
  /*!
    tp id
    */
  u32 tp_id;
  /*!
     program id
    */
  u32 prog_id;
}satcodx_update_channel_param_t;


/*!
  Satcodx implementation data
  */
typedef struct 
{
  /*!
    memory partition id used in satcodx app
    */
  u8 mem_partition_id;
  /*!
    memory block id used in satcodx app
    */
  u32 mem_block_id;
  /*!
    max buffer size for satcodx data
    unit: byte
    */    
  u32 max_buf_size;
  /*!
    current offset in receive buf
    */
  u32 recv_buf_offset;
  /*!
    max receive buffer size
    */
  u32 recv_buf_max_size;
  /*!
    stop receiving data when there's no coming data during the timeout interval
    unit: ms
    */
  u32 recv_host_stop_timeout;
  /*!
    stop waiting data when there's no coming data during the timeout interval
    unit: ms
    */
  u32 wait_host_start_timeout;
  /*!
    the time units when used for timeout
    */
  u32 duration_per_second;
  /*!
    notify ui once when buffer increases certain fixed bytes
    unit: byte
    */
  u32 notify_per_size;
  /*!
    default max satellite count
    */
  u32 default_sat_max_cnt;
  /*!
    default max tp count
    */
  u32 default_tp_max_cnt;
  /*!
    satcodx ap state machine
    */
  satcodx_sm_t sm;
  /*!
    pointer to buffer for satcodx data
    */
  u8 *p_static_buf;
  /*!
    pointer to static buffer
    */
  u8 *p_cur_pos_of_static_buf;
  /*!
    pointer to recevie buffer
    */
  u8 *p_recv_buf;
  /*!
    current buffer size for satcodx data
    unit: byte
    */
  u32 cur_static_buf_left_size;
  /*!
    buffer used in receive buffer
    */
  u32 cur_recv_buf_used_size;
  /*!
    pointer to result list
    */
  satcodx_result_list_t *p_result_list;
  /*!
    pointer to db satellite list 
    */
  satcodx_db_sat_list_t *p_db_list;
  /*!
    update channel parameter
    */
  satcodx_update_channel_param_t update_param;
} satcodx_impl_data_t;

/*!
  Notify satcodx ui to update ui status
  \param[in] evt_id  event id
  \param[in] para1   event para1
  \param[in] para2   event para2
  */
void ap_satcodx_notify_ui(u32 evt_id, u32 para1, u32 para2);

/*!
  Start satcodx app
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] mem_block_id block id for allocation
  */
BOOL ap_satcodx_start(void *p_satcodx_impl_data, u32 mem_block_id);

/*!
  Stop satcodx app
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data
  */
BOOL ap_satcodx_stop(void *p_satcodx_impl_data);

/*!
  Malloc satcodx pool memroy from system
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] size size of memory to allocate
  */
void *satcodx_sys_malloc(satcodx_impl_data_t *p_satcodx_impl_data, u32 size);

/*!
  Free satcodx pool memroy from system
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] ptr pointer to memory to free
  */
void  satcodx_sys_free(satcodx_impl_data_t *p_satcodx_impl_data, void *ptr);

/*!
  Malloc satcodx pool memroy from block
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[out] p_size pointer to size of memory to allocate and allocated
  */
void *satcodx_block_malloc(satcodx_impl_data_t *p_satcodx_impl_data, u32 *p_size);

/*!
  Free satcodx pool memroy from block
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] ptr pointer to memory to free
  */
void  satcodx_block_free(satcodx_impl_data_t *p_satcodx_impl_data, void *ptr);

/*!
  Malloc satic memroy from satcodx memroy pool
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] size size of memory to allocate from static memory
  */
void *satcodx_static_malloc(satcodx_impl_data_t *p_satcodx_impl_data, 
                                             u32 size);

/*!
  Free satic memroy from satcodx memroy pool
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] ptr pointer to memory to free
  */
void  satcodx_static_free(satcodx_impl_data_t *p_satcodx_impl_data, 
                                         void *ptr);

#endif // End for __AP_SATCODX_COMMON_H_

