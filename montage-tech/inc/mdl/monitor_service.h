/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __M_SVC_H_
#define  __M_SVC_H_

/*!
  Monitor service event
  */
typedef enum tag_m_svc_evt
{
  /*!
    Flag of evt start
    */
  M_SVC_EVT_START = (MDL_MONITOR_SVC << 16),
  /*!
    Invoke this event when monitor data version changed
    */
  M_SVC_VER_CHANGED,
  /*!
    Invoke this event when monitor data restart
    */
  M_SVC_DATA_RESET,
  /*!
    Invoke this event when monitor data is ready
    */
  M_SVC_DATA_READY,
  /*!
    Flag of evt end
    */
  M_SVC_EVT_END
}m_svc_event_t;

/*!
  Monitor service commands
  */
typedef enum tag_m_svc_cmd
{
  /*!
    Stop and start monitor command
    */
  M_SVC_RESET_CMD,
  /*!
    Stop monitor command
    */
  M_SVC_STOP_CMD,
} m_svc_cmd_t;

/*!
  Monitor table index id
  */
typedef enum tag_m_table_id
{
  /*!
    Pat table id
    */
  M_PAT_TABLE,
  /*!
    Pmt table id
    */
  M_PMT_TABLE,
  /*!
    Nit table id
    */
  M_NIT_TABLE,
  /*!
    Sdt table id
    */
  M_SDT_TABLE,
  /*!
    Cat table id
    */
  M_CAT_TABLE,
  /*!
    Bat table id
    */
  M_BAT_TABLE,
  /*!
    Monitor table id max
    */
  M_MAX_TABLE
}m_table_id_t;

/*!
  Monitor cmd param
  */
typedef struct tag_m_svc_cmd_p
{
  /*!
    current play program service id
    */
  u16 s_id;
  /*!
    current play program pmt pid, invalid as 0x1FFF
    */
  u16 pmt_pid;
 /*!
  see dmx_input_type_t
  */
    u8  ts_in;
}m_svc_cmd_p_t;

/*!
  Get monitor data type
  */
typedef enum tag_m_data_type
{
  /*!
    Get monitor section data
    */
  M_SECTION_DATA,
  /*!
    Get monitor table data
    */
  M_TABLE_DATA,
}m_data_type_t;

/*!
  the declaration of request table callback function for monitor
  */
typedef void (*m_request_proc_t)(handle_t handle, void *p_param);

/*!
    the declaration of parse table callback function for monitor, the dest buffer alloc by app
  */
typedef void (*m_parse_proc_t)(handle_t handle, void *p_src, void *p_dst);


/*!
  Monitor register table param
  */
typedef struct tag_m_reg_table
{
  /*!
    table id,see m_table_id_t
    */
  m_table_id_t t_id;
  /*!
    request callback funtion
    */
  //m_request_proc_t request_proc;
  /*!
    parse callback function
    */
  m_parse_proc_t parse_proc;
  /*!
    recoder this table cycle time in ms
    */
  u32 period;
}m_reg_table_t;


/*!
  Monitor register config param
  */
typedef struct tag_m_register
{
  /*!
    table id,see m_reg_table_t
    */
  m_reg_table_t table[M_MAX_TABLE];
  /*!
    register table number
    */
  u16 num;
  /*!
    app id
    */
  u16 app_id;
}m_register_t;

/*!
  Monitor get data param
  */
typedef struct tag_m_data_param
{
  /*!
    which table data you will get, see m_table_id_t
    */
  m_table_id_t t_id;
  /*!
    which data type you will get, see m_data_type_t
    */
  m_data_type_t type;
  /*!
    which app wants to get data
    */
  u32 ap_id;
  /*!
    save data buffer, malloc  by user
    */
  void *p_buffer;
}m_data_param_t;

/*!
  init parameter monitor service.
  */
typedef struct tag_m_svc_init_para
{
  /*!
    service's priority.
    */
  u32 service_prio;
  /*!
    service's stack size.
    */
  u32 nstksize;
}m_svc_init_para_t;

/*!
  monitor svc class declaration
  */
typedef struct tag_m_svc
{
  /*!
    get a service handle from monitor svc module
    */
  service_t * (*get_svc_instance)(class_handle_t handle, u32 context);
  /*!
    remove svc instance
    */
  void (*remove_svc_instance)(class_handle_t handle, service_t *p_svc);
  /*!
    get data
    */
  BOOL (*data_get)(class_handle_t handle, m_data_param_t *p_param);
  /*!
    start monitor
    */
  void (*start)(class_handle_t handle, m_svc_cmd_p_t *p_param);
  /*!
    stop monitor
    */
  void (*stop)(class_handle_t handle);
  /*!
    register a monitor table
    */
  void (*register_table)(class_handle_t handle, m_register_t *p_param);
  /*!
    ansync command process
    */
  void (*do_cmd)(class_handle_t handle, u32 cmd_id, m_svc_cmd_p_t *p_param);
  /*!
    monitor service privated data
    */
  void *p_data;
}m_svc_t;

/*!
  dvb monitor service register table information

  \param[in] handle monitor service handle.
  \param[in] p_param command param see m_register_t.

  */
void dvb_monitor_register_table(class_handle_t handle, m_register_t *p_param);

/*!
  dvb monitor service get data

  \param[in] handle monitor service handle.
  \param[in] p_param command param see m_data_param_t.

  \return TRUE or FALSE.
  */
BOOL dvb_monitor_data_get(class_handle_t handle, m_data_param_t *p_param);

/*!
  dvb monitor service process command

  \param[in] handle monitor service handle.
  \param[in] cmd_id command type see m_svc_cmd_t.
  \param[in] p_param command param see m_svc_cmd_p_t.

  */
void dvb_monitor_do_cmd(class_handle_t handle, u32 cmd_id, m_svc_cmd_p_t *p_param);

/*!
  init monitor service.

  \param[in] service_prio   service task priority.

  \return monitor service handle.
  */
handle_t dvb_monitor_service_init(m_svc_init_para_t *p_para);

#endif // End for __M_SVC_H_
