/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_misc.h"

#include "class_factory.h"

#include "drv_dev.h"
#include "dmx.h"

#include "mdl.h"

#include "service.h"
#include "dvb_protocol.h"

#include "ap_framework.h"

#include "fcrc.h"
#include "lib_util.h"
#include "simple_queue.h"

#include "monitor_service.h"
#include "mdl.h"
#include "nim.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "dvb_svc.h"
#include "hal_misc.h"

/*!
  section data buffer size
  */
#define DATA_BUFFER_SIZE          (4*(KBYTES) + 192)

/*!
  database header size
  */
#define DB_HEADER_SIZE          (sizeof(m_db_header_t))

/*!
  Message queue depth
  */
#define MONITOR_MSG_DEPTH (5)

/*!
  app total num
  */
#define APP_MAX_NUM (6)

/*!
  Internal state machine
  */
typedef enum
{
  /*!
    Idle state
    */
  MONITOR_IDLE = 0,
  /*!
    Allocating DMX filter and process data
    */
  MONITOR_RUNNING,
  /*!
    Stop process state
    */
  MONITOR_STOPPING,
  /*!
    check service id  process state
    */
  MONITOR_CHECK_S_ID,
} m_state_t;

/*!
  monitor database header struct
  */
typedef struct
{
  /*!
    see m_table_id_t
    */
  m_table_id_t t_id;
  /*!
    save data buffer address
    */
  u8 *p_buffer;
  /*!
    data status
    */
  BOOL is_ready;
}m_db_header_t;

/*!
  app register info
  */
typedef struct
{
  /*!
    register app id
    */
  u32 app_id;
  /*!
    Callback function for request
    */
  m_request_proc_t request;
  /*!
    Callback function for parse
    */
  m_parse_proc_t parse;
  /*!
    app get data flag/when data is not ready, set flag as 1
    */
  BOOL get_flag;
}m_app_register_t;


/*!
  monitor table info structure
  */
typedef struct
{
  /*!
    Unique monitor psi table id
    */
  m_table_id_t table_id;
  /*!
    register inlative app info
    */
  m_app_register_t app_info[APP_MAX_NUM];
  /*!
    this table cycle time in ms
    */
  u32 period;
  /*!
    this table start time in ms
    */
  u32 start_ticks;
  /*!
    app register max num
    */
  u8 app_num;
  /*!
    table data version
    */
  u8 version;
  /*!
    dmx channel handle
    */
 u16 dmx_handle;
 /*!
  check table if or not request
  */
 BOOL req_flag;
 /*!
  how many times this table time out in ms
  */
  u32 timeout;
} m_table_info_t;

typedef struct 
{
  /*!
    monitor table info
    */
  m_table_info_t table_info[M_MAX_TABLE];
  /*!
    PTI driver's handle
    */
  dmx_device_t *p_dmx_dev;
  /*!
    Monitor service handle
    */
  service_t *p_m_svc;
  /*!
    data buffer header address
    */
  m_db_header_t *p_data_header;
  /*!
    service container handle
    */
  svc_container_t *p_container;
  /*!
    start service id and pmt pid
    */
  m_svc_cmd_p_t param;
  /*!
    dmx using buffer
    */
  u8 *p_dmx_buffer;
  /*!
    monitor service message queue id
    */
  u32 msgq_id;
  /*!
    sem handle
    */
  os_sem_t sem;
  /*!
    state
    */
  m_state_t state;
  /*!
    register table index
    */
  u8 table_index[M_MAX_TABLE];
  /*!
    register table num
    */
  u8 table_num;
} m_priv_t;

typedef struct
{
  m_svc_t *p_this;
} m_svc_data_t;

static BOOL parse_pat_m(class_handle_t handle, u8 *p_buf)
{
  u8 *p_ptr = NULL;
  u16 section_len = 0;
  u16 prog_no = 0;
  u16 pmt_pid = 0;
  s16 program_len = 0;
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  
  MT_ASSERT(NULL != p_buf);

  /* get section length */
  section_len = MAKE_WORD(p_buf[2], (p_buf[1] & 0x0f));
  section_len += 3;

  program_len = (section_len - 8 - 4);

  p_ptr = &p_buf[8];

  while(program_len > 0)
  {
    /* get program number */
    prog_no = MAKE_WORD(p_ptr[1], p_ptr[0]);

    /* get program map pid */
    pmt_pid = MAKE_WORD(p_ptr[3], (p_ptr[2] & 0x1f));

    if (p_priv->param.s_id == prog_no)
    {
      p_priv->param.pmt_pid = pmt_pid;
      return TRUE;
    }
    else if(p_priv->param.pmt_pid == pmt_pid
        && p_priv->param.s_id == 0x1fff)
    {
      p_priv->param.s_id = prog_no;
      return TRUE;
    }
    
    program_len -= 4;
    p_ptr += 4;
  }
  
  //p_priv->param.pmt_pid = 0x1FFF;
  return FALSE;
}

/*!
  Lock monitor

  \param[in] handle Monitor Service class handle
  */
static void m_svc_lock(handle_t handle)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  mtos_sem_take(&p_priv->sem, 0);
}

/*!
  Unlock monitor

  \param[in] handle Monitor Service class handle
  */
static void m_svc_unlock(handle_t handle)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  mtos_sem_give(&p_priv->sem);
}

static void monitor_notify(u16 app_id, m_table_id_t t_id, m_svc_event_t event)
{
  os_msg_t msg = {0};

  msg.content = event;
  msg.is_ext = 0;
  msg.para1 = t_id;
  msg.extand_data = (app_id | SERVICE_MSG_MASK);
  mdl_send_msg(&msg);
}

static void monitor_config_dmx_param(class_handle_t handle, u16 t_id, 
                      dmx_slot_setting_t *p_slot, dmx_filter_setting_t *p_filter)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif
  
  p_slot->type = DMX_CH_TYPE_SECTION;
  p_slot->in = p_priv->param.ts_in;
  if(chip_ic_id == IC_CONCERTO)
  {
    p_slot->muldisp = DMX_SLOT_MUL_SEC_DIS_DISABLE;
  }
  
  p_filter->continuous = FALSE;
  p_filter->en_crc = TRUE;
  
  switch(t_id)
  {
    case M_PAT_TABLE:
      p_slot->pid = DVB_PAT_PID;
      p_filter->value[0] = DVB_TABLE_ID_PAT;
      p_filter->mask[0] = 0xFF;
      p_priv->table_info[M_PAT_TABLE].timeout = PAT_TIMEOUT;
      break;
    case M_PMT_TABLE:
      p_slot->pid = p_priv->param.pmt_pid;
      p_filter->value[0] = DVB_TABLE_ID_PMT;
      p_filter->mask[0] = 0xFF;
#ifndef WIN32
      p_filter->value[1] = p_priv->param.s_id >> 8;
      p_filter->mask[1] = 0xFF;
      p_filter->value[2] = p_priv->param.s_id & 0xFF;
      p_filter->mask[2] = 0xFF;
#else
      p_filter->value[3] = p_priv->param.s_id >> 8;
      p_filter->mask[3] = 0xFF;
      p_filter->value[4] = p_priv->param.s_id & 0xFF;
      p_filter->mask[4] = 0xFF;
#endif
      p_priv->table_info[M_PMT_TABLE].timeout = PMT_TIMEOUT;
      break;
    case M_NIT_TABLE:
      p_slot->pid = DVB_NIT_PID;
      p_filter->value[0] = DVB_TABLE_ID_NIT_ACTUAL;
      p_filter->mask[0] = 0xFF;
#ifndef WIN32
      p_filter->value[4] = 0;
      p_filter->mask[4] = 0xFF;
#else
      p_filter->value[6] = 0;
      p_filter->mask[6] = 0xFF;
#endif
      p_priv->table_info[M_NIT_TABLE].timeout = NIT_TIMEOUT;
      break;
    case M_CAT_TABLE:
      p_slot->pid = DVB_CAT_PID;
      p_filter->value[0] = DVB_TABLE_ID_CAT;
      p_filter->mask[0] = 0xFF;
      p_priv->table_info[M_CAT_TABLE].timeout = CAT_TIMEOUT;
      break;
    case M_BAT_TABLE:
      break;
    case M_SDT_TABLE:
      p_slot->pid = DVB_SDT_PID;
      p_filter->value[0] = DVB_TABLE_ID_SDT_ACTUAL;
      p_filter->mask[0] = 0xFF;
#ifndef WIN32
      p_filter->value[4] = 0;
      p_filter->mask[4] = 0xFF;
#else
      p_filter->value[6] = 0;
      p_filter->mask[6] = 0xFF;
#endif
      p_priv->table_info[M_SDT_TABLE].timeout = SDT_TIMEOUT;
      break;
    default:
      OS_PRINTF("Unknown table index\n");
      MT_ASSERT(0);
      break;
  }
}

static void monitor_open_dmx_filter(class_handle_t handle, u16 t_id)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  RET_CODE ret = ERR_FAILURE;
  dmx_slot_setting_t slot = {0};
  dmx_filter_setting_t filter = {0};
  u8 index = t_id;
  u8 *p_buffer = NULL;
  u16 dmx_handle = 0xFFFF;

  monitor_config_dmx_param(handle, index, &slot, &filter);
  
  //hack for warriors bug 22712,37060, all use soft filter.
  slot.soft_filter_flag = dvb_svc_is_use_soft();
  ret = dmx_si_chan_open(p_priv->p_dmx_dev, &slot, &dmx_handle);
  if (SUCCESS == ret)
  {
    MT_ASSERT(dmx_handle != 0xffff);
    p_buffer = p_priv->p_dmx_buffer + DATA_BUFFER_SIZE * index;
    ret = dmx_si_chan_set_buffer(p_priv->p_dmx_dev, dmx_handle, p_buffer, DATA_BUFFER_SIZE);
    MT_ASSERT(SUCCESS == ret);

    ret = dmx_si_chan_set_filter(p_priv->p_dmx_dev, dmx_handle, &filter);
    MT_ASSERT(SUCCESS == ret);
    
    ret = dmx_chan_start(p_priv->p_dmx_dev, dmx_handle);
    MT_ASSERT(SUCCESS == ret);

    p_priv->table_info[index].dmx_handle = dmx_handle;
    p_priv->table_info[index].req_flag = TRUE;
    p_priv->table_info[index].start_ticks = mtos_ticks_get();
  }
}

static BOOL check_sec_crc(u8 *p_buffer)
{
  u16 section_len = 0;
  u32 crc = 0;
  
  section_len = MAKE_WORD(p_buffer[2], (p_buffer[1] & 0x0f)); 
  section_len += 3;
  crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,
            0xFFFFFFFF,  p_buffer, section_len - 4);
  return (crc == make32(p_buffer + section_len -4));
}

static void m_free_filter(class_handle_t handle, u8 index)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  
  u16 dmx_handle = p_priv->table_info[index].dmx_handle;

  if (dmx_handle != 0xFFFF)
  {
    dmx_chan_stop(p_priv->p_dmx_dev, dmx_handle);
    dmx_chan_close(p_priv->p_dmx_dev, dmx_handle);
    
    p_priv->table_info[index].dmx_handle = 0xFFFF;
  }
}

static BOOL monitor_data_get(class_handle_t handle, m_data_param_t *p_param)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  u16 section_len = 0;
  u8 *p_buffer = NULL;
  u8 i = 0;

  MT_ASSERT(p_param->t_id < M_MAX_TABLE);

  p_buffer = (u8 *)p_priv->p_data_header[p_param->t_id].p_buffer;

  if ((p_priv->state == MONITOR_RUNNING) &&
      (p_priv->p_data_header[p_param->t_id].is_ready == TRUE))
  {
    m_svc_lock(handle);
    if (p_param->type == M_SECTION_DATA)
    {
      section_len = MAKE_WORD(p_buffer[2], (p_buffer[1] & 0x0f)); 
      section_len += 3;
      memcpy(p_param->p_buffer, p_buffer, section_len);
    }
    else if (p_param->type == M_TABLE_DATA)
    {
      for (i = 0; i < p_priv->table_info[p_param->t_id].app_num; i++)
      {
        if (p_priv->table_info[p_param->t_id].app_info[i].app_id == p_param->ap_id)
        {
          p_priv->table_info[p_param->t_id].app_info[i].parse(handle, p_buffer, p_param->p_buffer);
        }
      }
    }
    else
    {
      OS_PRINTF("error data type!!!!");
      MT_ASSERT(0);
    }
    m_svc_unlock(handle);
    return TRUE;
  }
  else
  {
    for (i = 0; i < p_priv->table_info[p_param->t_id].app_num; i++)
    {
      if (p_priv->table_info[p_param->t_id].app_info[i].app_id == p_param->ap_id)
      {
        p_priv->table_info[p_param->t_id].app_info[i].get_flag = TRUE;
      }
    }
  }

  return FALSE;
}

static void monitor_data_save(class_handle_t handle, m_table_id_t t_id, u8 *p_buffer)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  u8 i = 0;
  u16 section_len = 0;
  u8 ver = (p_buffer[5] & 0x3E) >> 1;

  section_len = MAKE_WORD(p_buffer[2], (p_buffer[1] & 0x0f)); 
  section_len += 3;

  m_svc_lock(handle);
  memcpy(p_priv->p_data_header[t_id].p_buffer, p_buffer, section_len);
  m_svc_unlock(handle);

  if (!p_priv->p_data_header[t_id].is_ready)
  {
    p_priv->p_data_header[t_id].is_ready = TRUE;
    p_priv->table_info[t_id].version = ver;
    for (i = 0; i < p_priv->table_info[t_id].app_num; i++)
    {
      if (p_priv->table_info[t_id].app_info[i].get_flag)
      {
        p_priv->table_info[t_id].app_info[i].get_flag = FALSE;
        monitor_notify(p_priv->table_info[t_id].app_info[i].app_id, t_id, M_SVC_DATA_READY);
      }
      else
      {
        monitor_notify(p_priv->table_info[t_id].app_info[i].app_id, t_id, M_SVC_DATA_RESET);
      }
    }
  }
  else
  {
    if (ver != p_priv->table_info[t_id].version)
    {
      p_priv->table_info[t_id].version = ver;
      for (i = 0; i < p_priv->table_info[t_id].app_num; i++)
      {
        monitor_notify(p_priv->table_info[t_id].app_info[i].app_id, t_id, M_SVC_VER_CHANGED);
      }
    }
  }
}

static void m_alloc_filter(class_handle_t handle)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  u8 i = 0;
  u8 index = 0;
//  u16 dmx_handle;
  u32 ticks = mtos_ticks_get();

  for (i = 0; i < p_priv->table_num; i++)
  {
    index = p_priv->table_index[i];

    if (((ticks - p_priv->table_info[index].start_ticks) * 10 > p_priv->table_info[index].period)
        && (!p_priv->table_info[index].req_flag))
    {
      //p_priv->table_info[index].req_flag = FALSE;
      if (index == M_PMT_TABLE && p_priv->param.pmt_pid == 0x1FFF)
      {
        continue;
      }

      monitor_open_dmx_filter(handle, index);
    }
  }
}

static void m_process_filter(class_handle_t handle)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  //svc_container_t *p_container = p_priv->p_container;
  RET_CODE ret = ERR_FAILURE;
  u8 *p_data = NULL;
  u32 data_len = 0;
  u8 i = 0;
  u16 dmx_handle = 0xFFFF;
  u8 index = 0;
  u32 cur_ticks = mtos_ticks_get();

  for (i = 0; i < p_priv->table_num; i++)
  {
    index = p_priv->table_index[i];
    dmx_handle = p_priv->table_info[index].dmx_handle;
    if (p_priv->table_info[index].req_flag && (dmx_handle != 0xFFFF))
    {
      ret = dmx_si_chan_get_data(p_priv->p_dmx_dev, dmx_handle, &p_data, &data_len);
      if (SUCCESS == ret) //data ready
      {
        m_free_filter(handle, index);
        if (check_sec_crc(p_data))
        {
          if (index == M_PAT_TABLE)
          {
            parse_pat_m(handle, p_data);
          }
          monitor_data_save(handle, index, p_data);
        }
        p_priv->table_info[index].start_ticks = cur_ticks;
        p_priv->table_info[index].req_flag = FALSE;
      }
      else if ((cur_ticks - p_priv->table_info[index].start_ticks)*10 > 
              p_priv->table_info[index].timeout)
      {
        m_free_filter(handle, index);
        OS_PRINTF("monitor timeout %d\n", index);
        p_priv->table_info[index].start_ticks = cur_ticks;
        p_priv->table_info[index].req_flag = FALSE;
      }
    }
  }
}

static void _internal_stop(class_handle_t handle)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  u8 i = 0;
  u8 index = 0;

  for (i = 0; i < M_MAX_TABLE; i++)
  {
    p_priv->p_data_header[i].is_ready = FALSE;
  }
  p_priv->state = MONITOR_IDLE;
  p_priv->param.pmt_pid = 0x1FFF;
  p_priv->param.s_id = 0x1FFF;

  for (i = 0; i < p_priv->table_num; i++)
  {
    index = p_priv->table_index[i];
    if (p_priv->table_info[index].dmx_handle != 0xFFFF)
    {
      dmx_chan_stop(p_priv->p_dmx_dev, p_priv->table_info[index].dmx_handle);
      dmx_chan_close(p_priv->p_dmx_dev, p_priv->table_info[index].dmx_handle);
      p_priv->table_info[index].dmx_handle = 0xFFFF;
      p_priv->table_info[index].req_flag = FALSE;
      p_priv->table_info[index].start_ticks = 0;
    }
  }
}

static void monitor_check_service_id(handle_t handle)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  u16 dmx_handle = 0xFFFF;
  u8 *p_data = NULL;
  u32 data_len = 0;
  RET_CODE ret = ERR_FAILURE;
  u32 cur_ticks = mtos_ticks_get();

  if (!p_priv->table_info[M_PAT_TABLE].req_flag)
  {
    if (p_priv->table_info[M_PAT_TABLE].table_id == M_MAX_TABLE)
    {
      p_priv->table_info[M_PAT_TABLE].period = 500;
      p_priv->table_info[M_PAT_TABLE].table_id = M_PAT_TABLE;
      p_priv->table_index[p_priv->table_num] = M_PAT_TABLE;
      p_priv->table_num++;
    }
    monitor_open_dmx_filter(handle, M_PAT_TABLE);
  }
  else
  {
    dmx_handle = p_priv->table_info[M_PAT_TABLE].dmx_handle;
    if (dmx_handle != 0xFFFF)
    {
      ret = dmx_si_chan_get_data(p_priv->p_dmx_dev, dmx_handle, &p_data, &data_len);
      if (SUCCESS == ret) //data ready
      {
        m_free_filter(handle, M_PAT_TABLE);
        if (check_sec_crc(p_data))
        {
           if (parse_pat_m(handle, p_data))
           {
              p_priv->state = MONITOR_RUNNING;
           }
        }
        p_priv->table_info[M_PAT_TABLE].start_ticks = cur_ticks;
        p_priv->table_info[M_PAT_TABLE].req_flag = FALSE;
      }
      else if ((cur_ticks - p_priv->table_info[M_PAT_TABLE].start_ticks) > 
              p_priv->table_info[M_PAT_TABLE].timeout * 10)
      {
        m_free_filter(handle, M_PAT_TABLE);
        OS_PRINTF("monitor timeout %d\n", M_PAT_TABLE);
        p_priv->table_info[M_PAT_TABLE].start_ticks = cur_ticks;
        p_priv->table_info[M_PAT_TABLE].req_flag = FALSE;
      }
    }
  }
}

static void msg_proc(handle_t handle, os_msg_t *p_msg)
{
  service_t *p_svc = (service_t *)handle;
  m_svc_data_t *p_svc_data = (m_svc_data_t *)p_svc->get_data_buffer(p_svc);
  m_svc_t *p_this = p_svc_data->p_this;
  m_priv_t *p_priv = p_this->p_data;
  m_svc_cmd_p_t *p_param = (m_svc_cmd_p_t *)p_msg->para1;

  switch(p_msg->content)
  {
    case M_SVC_RESET_CMD:
      OS_PRINTF("monitor start\n");
      _internal_stop(p_this);
      p_priv->param.pmt_pid = p_param->pmt_pid;
      p_priv->param.s_id = p_param->s_id;
      p_priv->param.ts_in = p_param->ts_in;
      p_priv->state = MONITOR_CHECK_S_ID;
      break;
    case M_SVC_STOP_CMD:
      _internal_stop(p_this);
      p_priv->state = MONITOR_IDLE;
      break;
    default:
      OS_PRINTF("monitor invalid command\n");
      break;
  }
}

static void sm_proc(handle_t handle)
{
  service_t *p_svc = (service_t *)handle;
  m_svc_data_t *p_svc_data = (m_svc_data_t *)p_svc->get_data_buffer(p_svc);
  m_svc_t *p_this = p_svc_data->p_this;
  m_priv_t *p_priv = p_this->p_data;
  switch(p_priv->state)
  {
    case MONITOR_IDLE:
      break;
    case MONITOR_CHECK_S_ID:
      monitor_check_service_id(p_this);
      break;
    case MONITOR_RUNNING:
      m_alloc_filter(p_this);
      m_process_filter(p_this);
      break;
    case MONITOR_STOPPING:
      break;
    default:
      MT_ASSERT(0);
    break;
  }
}

static void register_table(class_handle_t handle, m_register_t *p_param)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  u8 i = 0;
  u8 index = 0;
  u8 t_id = 0;
  u32 period = 0;

  for (i = 0; i < p_param->num; i++)
  {
    t_id = p_param->table[i].t_id;
    
    index = p_priv->table_info[t_id].app_num;

    if (p_priv->table_info[t_id].table_id == M_MAX_TABLE)
    {
      p_priv->table_index[p_priv->table_num] = t_id;
      p_priv->table_num++;
    }

    period = p_priv->table_info[t_id].period;
    
    if (period > p_param->table[i].period)
    {
      p_priv->table_info[t_id].period = p_param->table[i].period;
    }
    
    p_priv->table_info[t_id].req_flag = FALSE;

    p_priv->table_info[t_id].table_id = t_id;

    p_priv->table_info[t_id].app_info[index].app_id = p_param->app_id;

    p_priv->table_info[t_id].app_info[index].parse = p_param->table[i].parse_proc;

    //p_priv->table_info[t_id].app_info[index].request = p_param->table[i].request_proc;

    p_priv->table_info[t_id].app_num++;
  }
}

static void on_decorate_cmd(handle_t handle, os_msg_t *p_msg)
{
  // Our command need mem copy
  switch(p_msg->content)
  {
    case M_SVC_RESET_CMD:
      p_msg->is_ext = TRUE;
      break;
    default:
      p_msg->is_ext = FALSE;
      break;
  }
}

static service_t * get_svc_instance(class_handle_t handle, u32 context)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  service_t *p_svc = NULL;
  m_svc_data_t *p_svc_data = NULL;

  p_svc = p_priv->p_container->get_svc(p_priv->p_container, context);
  if(NULL == p_svc)
  {
    p_svc = construct_svc(context);
    p_svc->msg_proc = msg_proc;
    p_svc->sm_proc = sm_proc;
    p_svc->on_decorate_cmd = on_decorate_cmd;
    p_svc->allocate_data_buffer(p_svc, sizeof(m_svc_data_t));
    p_svc_data = (m_svc_data_t *)p_svc->get_data_buffer(p_svc);
    p_svc_data->p_this = p_this;
    //p_svc_data->state = MONITOR_RUNNING;
    p_priv->p_container->add_svc(p_priv->p_container, p_svc);
  }
  return p_svc;
}

static void rm_svc_instance(class_handle_t handle, service_t *p_svc)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;

  p_priv->p_container->remove_svc(p_priv->p_container, p_svc);
}


static void start(class_handle_t handle, m_svc_cmd_p_t *p_param)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  svc_container_t *p_container = p_priv->p_container;
  
  p_container->lock(p_container);

  p_priv->state = MONITOR_CHECK_S_ID;
  p_priv->param.s_id = p_param->s_id;
  p_priv->param.pmt_pid = p_param->pmt_pid;

  p_container->unlock(p_container);
}

static void stop(class_handle_t handle)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  svc_container_t *p_container = p_priv->p_container;
  u8 i = 0;
  u8 index = 0;
  p_container->lock(p_container);

  for (i = 0; i < M_MAX_TABLE; i++)
  {
    p_priv->p_data_header[i].is_ready = FALSE;
  }
  p_priv->state = MONITOR_IDLE;
  p_priv->param.pmt_pid = 0x1FFF;
  p_priv->param.s_id = 0x1FFF;
  for (i = 0; i < p_priv->table_num; i++)
  {
    index = p_priv->table_index[i];
    if (p_priv->table_info[index].dmx_handle != 0xFFFF)
    {
      dmx_chan_stop(p_priv->p_dmx_dev, p_priv->table_info[index].dmx_handle);
      dmx_chan_close(p_priv->p_dmx_dev, p_priv->table_info[index].dmx_handle);
      p_priv->table_info[index].dmx_handle = 0xFFFF;
      p_priv->table_info[index].req_flag = FALSE;
      p_priv->table_info[index].start_ticks = 0;
    }
  }
  p_container->unlock(p_container);
}

static void monitor_do_cmd(class_handle_t handle, u32 cmd_id, m_svc_cmd_p_t *p_param)
{
  m_svc_t *p_this = handle;
  m_priv_t *p_priv = p_this->p_data;
  service_t *p_svc = p_priv->p_m_svc;
  //monitor process the last msg
  mtos_messageq_clr(p_priv->msgq_id);
  p_svc->do_cmd(p_svc, cmd_id, (u32)p_param, sizeof(m_svc_cmd_p_t));
}

void dvb_monitor_register_table(class_handle_t handle, m_register_t *p_param)
{
  m_svc_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->register_table != NULL);

  p_this->register_table(handle, p_param);
}

BOOL dvb_monitor_data_get(class_handle_t handle, m_data_param_t *p_param)
{
  m_svc_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->data_get != NULL);
  
  return p_this->data_get(handle, p_param);
}

void dvb_monitor_do_cmd(class_handle_t handle, u32 cmd_id, m_svc_cmd_p_t *p_param)
{
  m_svc_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->do_cmd != NULL);
  
  p_this->do_cmd(handle, cmd_id, p_param);
}

handle_t dvb_monitor_service_init(m_svc_init_para_t *p_para)
{
  m_svc_t *p_this = mtos_malloc(sizeof(m_svc_t));
  m_priv_t *p_priv = mtos_malloc(sizeof(m_priv_t));
  u8 i = 0;
  u8 *p_temp = NULL;
  u32 attach_size = sizeof(m_svc_cmd_p_t) * MONITOR_MSG_DEPTH;
  void *p_attach_buffer = mtos_malloc(attach_size);
  svc_container_t *p_container = construct_svc_container("monitor",
    p_para->service_prio, p_para->nstksize, MONITOR_MSG_DEPTH);

  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_attach_buffer != NULL);

  memset(p_priv, 0, sizeof(m_priv_t));
  p_this->start = start;
  p_this->stop = stop;
  p_this->get_svc_instance = get_svc_instance;
  p_this->remove_svc_instance = rm_svc_instance;
  p_this->register_table = register_table;
  p_this->data_get = monitor_data_get;
  p_this->do_cmd = monitor_do_cmd;

  p_priv->p_container = p_container;
  p_priv->msgq_id = p_container->get_msgq_id(p_container);
  p_priv->p_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  
  p_priv->state = MONITOR_IDLE;
  
  p_priv->p_dmx_buffer = mtos_malloc(M_MAX_TABLE * DATA_BUFFER_SIZE);
  MT_ASSERT(p_priv->p_dmx_buffer != NULL);

  //init database
  p_priv->p_data_header = 
            mtos_malloc(M_MAX_TABLE * (DATA_BUFFER_SIZE + DB_HEADER_SIZE) + 4 * KBYTES);
  MT_ASSERT(p_priv->p_data_header != NULL);
  
  p_temp = (u8 *)(p_priv->p_data_header + M_MAX_TABLE);
  
  p_temp = (u8 *)ROUNDUP((u32)p_temp, 4);//align 4bytes

  for (i = 0; i < M_MAX_TABLE; i++)
  {
    p_priv->table_info[i].table_id = M_MAX_TABLE;
    p_priv->table_info[i].dmx_handle = 0xFFFF;
    p_priv->table_info[i].period = 0xFFFF;
    p_priv->p_data_header[i].t_id = M_PAT_TABLE + i;
    p_priv->p_data_header[i].p_buffer = p_temp + i * DATA_BUFFER_SIZE;
    p_priv->p_data_header[i].is_ready = FALSE;
  }
  
  mtos_sem_create(&p_priv->sem, TRUE);
  
  //attach extern size
  memset(p_attach_buffer, 0, attach_size);
  mtos_messageq_attach(p_priv->msgq_id, p_attach_buffer,
    sizeof(m_svc_cmd_p_t), MONITOR_MSG_DEPTH);

  p_this->p_data = p_priv;

  //system service, so we get instance here
  p_priv->p_m_svc = get_svc_instance(p_this, APP_FRAMEWORK);

  class_register(M_SVC_CLASS_ID, p_this);
  return p_this;
}

