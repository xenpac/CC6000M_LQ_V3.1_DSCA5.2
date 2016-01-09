/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std headers
#include  "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_fifo.h"
#include "mtos_sem.h"

// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "lib_util.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dmx.h"

// middleware headers
#include "mdl.h"
#include "err_check_def.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "monitor_service.h"
#include "mosaic.h"
#include "cat.h"
#include "nit.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "smc_ctrl.h"

// ap headers
#include "ap_framework.h"
#include "cas_ware.h"
#include "ap_cas.h"

#define INVALID_SVC_ID (0xFFFF)
#define DATA_SIZE (128 * (KBYTES))
#define MAX_TABLE_NUM (3)
#define SECTION_LENGTH (4 * (KBYTES))
#define CAS_FIFO_SIZE  256
#define INVALID_PID  (0x1FFF)

#define CAS_OPERATOR_SIZE (sizeof(cas_operators_info_t))
#define CAS_ENTITLE_SIZE (sizeof(product_entitles_info_t))
#define CAS_IPPV_SIZE (sizeof(ipps_info_t))
#define CAS_BURSE_SIZE (sizeof(burses_info_t))
#define CAS_CARD_SIZE (sizeof(cas_card_info_t))
#define CAS_MAIL_HEADER_SIZE (sizeof(cas_mail_headers_t))
#define CAS_MAIL_BODY_SIZE (sizeof(cas_mail_body_t))
#define CAS_MON_CHILD_SIZE (sizeof(chlid_card_status_info))
#define CAS_FINGER_SIZE (sizeof(finger_msg_t))
#define CAS_OSD_MSG_SIZE (sizeof(msg_info_t))
#define CAS_IPP_BUY_SIZE (sizeof(ipp_buy_info_t))
#define CAS_OVERDUE_INFO_SERVICE_GET_SIZE (sizeof(cas_overdue_inform_t))
#define CAS_IPP_REC_SIZE (sizeof(ipp_total_record_info_t))
#define CAS_FORCE_CHANNEL_SIZE (sizeof(cas_force_channel_t))
#define CAS_BURSE_CHARGE_SIZE (sizeof(cas_burse_charge_t))
#define CAS_ERROR_PIN_CODE_SIZE (sizeof(cas_pin_err_t))
#define CAS_RATING_GET_SIZE (sizeof(u8)*4)
#define CAS_WORK_TIME_GET_SIZE (sizeof(cas_card_work_time_t))
#define CAS_AUTHEN_DAY_SIZE (sizeof(u32))
#define CAS_PLATFORM_ID_SIZE (sizeof(u16))
#define CAS_FILM_RATING_GET_SIZE (sizeof(cas_program_certificate_info_t))
#define CAS_WORK_TIME_SET_SIZE (sizeof(cas_card_work_time_t))
#define CAS_RATING_SET_SIZE (sizeof(cas_rating_set_t))
#define CAS_PIN_SIZE (sizeof(cas_pin_modify_t))
#define CAS_LIB_VERSION_SIZE 100
#define CAS_OTA_UPDATE_ID_SIZE (sizeof(cas_ota_info_t))
#define CAS_IPPV_SIGNAL_SIZE (sizeof(ipp_info_t))
#define CAS_ACLIST_SIZE (sizeof(u32)*19)
#define CAS_CMD_CA_JUDGE_CARD_SIZE (sizeof(card_stb_paired_list_t))
#define CAS_CMD_CONTINUE_WATCH_LIMIT_SIZE (sizeof(cas_continues_watch_limit_t))

/*!
  Delcare mem id
  */
typedef enum
{
  /*!
    comments
    */
  CAS_OPERATOR_MEM_ID,
  /*!
    comments
    */
  CAS_ENTITLE_MEM_ID,
  /*!
    comments
    */
  CAS_IPPV_MEM_ID,
  /*!
    comments
    */
  CAS_BURSE_MEM_ID,
  /*!
    comments
    */
  CAS_CARD_MEM_ID,
  /*!
    comments
    */
  CAS_MAIL_HEADER_MEM_ID,
  /*!
    comments
    */
  CAS_MAIL_BODY_MEM_ID,
  /*!
    comments
    */
  CAS_MON_CHILD_MEM_ID,
  /*!
    comments
    */
  CAS_FINGER_MEM_ID,
   /*!
    comments
    */
  CAS_ACLIST_MEM_ID,
  /*!
    comments
    */
  CAS_SHOW_OSD_MEM_ID,
  /*!
    comments
    */
  CAS_IPP_BUY_MEM_ID,
  /*!
    comments
    */
  CAS_IPP_RECORD_MEM_ID,
  /*!
    comments
    */
  CAS_FORCE_CHANNEL_MEM_ID,
  /*!
    comments
    */
  CAS_SHOW_MSG_MEM_ID,
  /*!
    comments
    */
  CAS_BURSE_CHARGE_MEM_ID,
  /*!
    comments
    */
  CAS_ERROR_PIN_MEM_ID,
  /*!
    comments
    */
  CAS_RATING_GET_MEM_ID,
  /*!
    comments
    */
  CAS_WORK_TIME_GET_MEM_ID,
  /*!
    comments
    */
  CAS_AUTHEN_DAY_MEM_ID,
    /*!
    comments
    */
  CAS_PLAT_FORM_MEM_ID,
    /*!
    comments
    */
  CAS_OVERDUE_INFO_SERVICE_GET_MEM_ID,
  /*!
    comments
    */
  CAS_FILM_RATING_MEM_ID,
  /*!
   comments
   */
  CAS_RATING_SET_MEM_ID,
    /*!
    comments
    */
  CAS_PIN_MEM_ID,
   /*!
   comments
   */
  CAS_WORK_TIME_SET_MEM_ID,
  /*!
    ca lib version
    */
  CAS_LIB_VERSION_MEM_ID,
    /*!
    comments
    */
 CAS_OTA_UPDATE_MEM_ID,
   /*!
    comments
    */
  CAS_OVERDUE_DATA_ID,
  /*!
    comments
  */
  CAS_PIN_VERIFY_ID,
  /*!
    comments
    */
  CAS_IPPV_SIGNAL_ID,
    /*!
    comments  add by he
    */
  CAS_CA_PAIR_STBCARD_ID,
  /*!
  comments  watch limit
  */
  CAS_CA_CONTINUE_WATCH_LIMIT_ID,
  /*!
   comments
   */
  CAS_MEM_ID_MAX
}cas_mem_id_t;

/*!
  CAS state machine
  */
typedef enum
{
  /*!
    idle
    */
  CAS_SM_IDLE = 0,
  /*!
   waitting tunner lock,
    */
  CAS_SM_WAITTING_LOCK,
  /*!
    status machine monitor
    */
  CAS_SM_MONITOR,
  /*!
    status machine monitor nit request
    */
  CAS_SM_CHECK_REQ,
} ca_sm_t;


/*!
  CAS priv data
  */
typedef struct
{
  /*!
    CAS instance
    */
  app_t instance;
  u16 pgid;
  u8  svc_num;
  u8 repeat_times;
  u16 svc_id[MAX_CAS_SLOT_NUM];
  u16 pmt_id;
  ca_sm_t sm;
  service_t *p_dvb_svc;
  cas_policy_t *p_policy;
  u8 *p_buffer;
  u32 cur_tp_freq;
  BOOL pmt_req_flag;
  BOOL nit_get_flag;
  BOOL change_tp_flag;
  BOOL cat_req_flag;
  BOOL nit_req_flag;
  BOOL pat_req_flag;
  u32 mem_addr[CAS_MEM_ID_MAX];
  u32 mem_end;
  u32 mem_start;
  u32 pmt_ticks;
  u32 cat_ticks;
  u16 pmt_version;
  u16 ca_sys_id;
  u16 lock_mode;
  nc_channel_info_t nc_search_info;
  os_fifo_t rcv_fifo;
  u16 rcv_buf[CAS_FIFO_SIZE];
  ap_cas_evt_t evt[CAS_FIFO_SIZE];
  u16 evt_index;
  os_sem_t evt_mutex;
  u8 ts_in;
  u8   tuner_id;
} cas_priv_t;

static u32 cas_get_mem_addr(handle_t handle, cas_mem_id_t index, u32 size)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  u32 check_end = p_this->mem_start + size;
  if (p_this->mem_addr[index] != 0)
  {
    memset((u8 *)p_this->mem_addr[index], 0, size);
    return p_this->mem_addr[index];
  }
  if (check_end > p_this->mem_end)
  {
    OS_PRINTF("ap cas mem is full\n");
    return 0;
  }

  p_this->mem_addr[index] = p_this->mem_start;
  p_this->mem_start += size;
  p_this->mem_start = ROUNDUP(p_this->mem_start, 4);
  memset((u8 *)p_this->mem_addr[index], 0, size);
  return p_this->mem_addr[index];
}

static void cas_send_evt_to_ui(u32 evt_id, u32 para1, u32 para2)
{
  event_t evt = {0, 0, 0};

  evt.id = evt_id;
  evt.data1 = para1;
  evt.data2 = para2;

  ap_frm_send_evt_to_ui(APP_CA, &evt);
}

static RET_CODE cas_notify_callback(u32 slot, u32 event, u32 param, u32 cam_id, u32 context)
{
  cas_priv_t *p_this = (cas_priv_t *)context;
  ap_cas_evt_t *p_evt = NULL;

  
  mtos_sem_take(&p_this->evt_mutex, 0);
  if(p_this->evt_index == CAS_FIFO_SIZE)
  {
    p_this->evt_index = 0;
  }
  p_evt = &p_this->evt[p_this->evt_index];

  p_evt->cam_id = (u8)cam_id;
  p_evt->event = (u16)event;
  p_evt->param = param;
  p_evt->slot_id = (u8)slot;

  OS_PRINTF("callback evt %d  slot %d,index %d\n", event, slot,p_this->evt_index);
  mtos_fifo_put(&p_this->rcv_fifo, p_this->evt_index);

  p_this->evt_index ++;
  mtos_sem_give(&p_this->evt_mutex);
  return SUCCESS;
}

static RET_CODE cas_events_get(handle_t handle, ap_cas_evt_t *p_event)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  u16 index = 0;
  RET_CODE ret = ERR_FAILURE;

  mtos_sem_take(&p_this->evt_mutex, 0);
  ret = mtos_fifo_get(&p_this->rcv_fifo, &index);
  if (ret == TRUE)
  {
    p_event->cam_id = p_this->evt[index].cam_id;
    p_event->slot_id = p_this->evt[index].slot_id;
    p_event->event = p_this->evt[index].event;
    p_event->param = p_this->evt[index].param;

    OS_PRINTF("get evt %d slot %d. index %d\n", p_event->event, p_event->slot_id, index);
    mtos_sem_give(&p_this->evt_mutex);
    return SUCCESS;
  }
  mtos_sem_give(&p_this->evt_mutex);
  return ERR_FAILURE;
}

static void process_reset_data(handle_t handle, m_table_id_t t_id, m_data_type_t type)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  m_svc_t *p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);
  u32 ret = 0;
  u16 service_id = 0xFFFF;
  m_data_param_t param = {0};
  cas_table_info_t ca_table_info = {0};
  cas_desc_t cas_ecm_info[MAX_ECM_DESC_NUM] = {{0},};
  u8 i = 0;
  u16 ecm_num = MAX_ECM_DESC_NUM;

  param.ap_id = APP_CA;
  param.t_id = t_id;
  param.type = type;
  switch(t_id)
  {
    case M_PMT_TABLE:
      param.p_buffer = p_this->p_buffer;
      if (dvb_monitor_data_get(p_svc, &param))
      {
        if (p_this->p_policy->test_ca_performance != NULL)
        {
          p_this->p_policy->test_ca_performance(CAS_PMT_DATA_STEP, mtos_ticks_get());
        }
        service_id = MAKE_WORD(p_this->p_buffer[4], p_this->p_buffer[3]);
        if (service_id == p_this->svc_id[0])
        {
          cas_table_process(CAS_TID_PMT, param.p_buffer, &ret);
          if (NULL != p_this->p_policy->update_ecm_info)
          {
            if (p_this->p_policy->update_ecm_info(param.p_buffer, cas_ecm_info, (u16 *)&ecm_num))
            {
              ca_table_info.service_id = p_this->svc_id[0];

              for (i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
              {
                ca_table_info.ecm_info[i].ca_system_id = cas_ecm_info[i].ca_sys_id;
                ca_table_info.ecm_info[i].ecm_id = cas_ecm_info[i].ecm_pid;
                ca_table_info.ecm_info[i].es_pid = cas_ecm_info[i].es_pid;
              }
              cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_SET_ECMEMM, &ca_table_info);
            }
          }
        }
      }
      break;
    case M_CAT_TABLE:
      param.p_buffer = p_this->p_buffer + SECTION_LENGTH;
      if (p_this->p_policy->test_ca_performance != NULL)
      {
        p_this->p_policy->test_ca_performance(CAS_CAT_DATA_STEP, mtos_ticks_get());
      }

      if (p_this->change_tp_flag)
      {
        if (dvb_monitor_data_get(p_svc, &param))
        {
          cas_table_process(CAS_TID_CAT, param.p_buffer, &ret);
          p_this->change_tp_flag = FALSE;
          //p_this->last_tp_freq = p_this->cur_tp_freq;
        }
      }
      break;
    case M_NIT_TABLE:
      if (p_this->p_policy->test_ca_performance != NULL)
      {
        p_this->p_policy->test_ca_performance(CAS_NIT_DATA_STEP, mtos_ticks_get());
      }
      break;
    default:
      break;
  }
}

static void process_normal_data(handle_t handle, m_table_id_t t_id, m_data_type_t type)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  m_svc_t *p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);
  u32 ret = 0;
  m_data_param_t param = {0};
  cas_table_info_t ca_table_info = {0};
  cas_desc_t cas_ecm_info[MAX_ECM_DESC_NUM] = {{0},};
  u8 i = 0;
  u16 ecm_num = MAX_ECM_DESC_NUM;
  param.ap_id = APP_CA;
  param.t_id = t_id;
  param.type = type;
  switch(t_id)
  {
    case M_PMT_TABLE:
      param.p_buffer = p_this->p_buffer;
      if (dvb_monitor_data_get(p_svc, &param))
      {
        cas_table_process(CAS_TID_PMT, param.p_buffer, &ret);
        if (NULL == p_this->p_policy->update_ecm_info)
        {
          break;
        }
        if (p_this->p_policy->update_ecm_info(param.p_buffer, cas_ecm_info, (u16 *)&ecm_num))
        {
          ca_table_info.service_id = p_this->svc_id[0];

          for (i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
          {
            ca_table_info.ecm_info[i].ca_system_id = cas_ecm_info[i].ca_sys_id;
            ca_table_info.ecm_info[i].ecm_id = cas_ecm_info[i].ecm_pid;
            ca_table_info.ecm_info[i].es_pid = cas_ecm_info[i].es_pid;
          }
          cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_SET_ECMEMM, &ca_table_info);
        }
      }
      break;
    case M_CAT_TABLE:
      param.p_buffer = p_this->p_buffer + SECTION_LENGTH;

      if (dvb_monitor_data_get(p_svc, &param))
      {
        cas_table_process(CAS_TID_CAT, param.p_buffer, &ret);
      }
      break;
    case M_NIT_TABLE:
      param.p_buffer = p_this->p_buffer + 2 * SECTION_LENGTH;
      if (dvb_monitor_data_get(p_svc, &param))
      {
        cas_table_process(CAS_TID_NIT, param.p_buffer, &ret);
      }
      break;
    default:
      break;
  }
}


static void pat_request(handle_t handle)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_PAT;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = DVB_TABLE_ID_PAT;
  dvb_req.para2 = 0;
  dvb_req.context = MAKE_REQUEST_CONTEXT(p_this->pgid, 0, p_this->repeat_times);
  dvb_req.ts_in = p_this->ts_in;
  p_this->p_dvb_svc->do_cmd(p_this->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
  p_this->pat_req_flag = TRUE;

  OS_PRINTF("CAS: pat request. pgid %d\n", p_this->pgid);
}

static void pmt_request(handle_t * handle, u16 pid, u16 s_id)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  //u8 i = 0;
  u32 param = ((pid << 16) | s_id);
  dvb_request_t req_param = {0};

  //Request PMT information
  req_param.req_mode  = DATA_SINGLE;
  req_param.table_id  = DVB_TABLE_ID_PMT;
  req_param.para1     = DVB_TABLE_ID_PMT;
  req_param.para2     = param;
  req_param.context =
    MAKE_REQUEST_CONTEXT(p_this->pgid, 0, p_this->repeat_times);
  req_param.ts_in = p_this->ts_in;
  p_this->p_dvb_svc->do_cmd(p_this->p_dvb_svc,
    DVB_REQUEST, (u32)&req_param, sizeof(dvb_request_t));
  p_this->pmt_req_flag = TRUE;
  OS_PRINTF("CAS: request pmt, pid %d, sid %d\n", pid, s_id);
  p_this->pmt_ticks = mtos_ticks_get();
}

static void nit_request(handle_t handle)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.para1 = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para2 = 0xFFFF;
  dvb_req.context = MAKE_REQUEST_CONTEXT(p_this->pgid, 0, p_this->repeat_times);
  dvb_req.ts_in = p_this->ts_in;
  p_this->p_dvb_svc->do_cmd(p_this->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
  p_this->nit_req_flag = TRUE;
  OS_PRINTF("CAS: nit request. pgid %d\n", p_this->pgid);
}

static void cat_request(handle_t handle, u16 sid)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_CAT;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = sid;
  dvb_req.para2 = 0;
  dvb_req.context = MAKE_REQUEST_CONTEXT(p_this->pgid, 0, p_this->repeat_times);
  dvb_req.ts_in = p_this->ts_in;
  p_this->p_dvb_svc->do_cmd(p_this->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
  p_this->cat_req_flag = TRUE;
  p_this->cat_ticks = mtos_ticks_get();
  OS_PRINTF("CAS: cat request. pgid %d, sid %d\n", p_this->pgid, sid);
}

static void on_pmt_info_found(handle_t handle,
  os_msg_t *p_msg)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  u8 i = 0;
  for(; i < MAX_CAS_SLOT_NUM; i++)
  {
    //OS_PRINTF("CAS: p_this->svc_id %d, p_msg->para2 %d\n", p_this->svc_id[i], p_msg->para2);
    if(p_this->svc_id[i] == p_msg->para2)
    {
      u32 param = ((p_msg->para1 << 16) | p_msg->para2);
      dvb_request_t req_param = {0};

      //Request PMT information
      req_param.req_mode  = DATA_SINGLE;
      req_param.table_id  = DVB_TABLE_ID_PMT;
      req_param.para1     = DVB_TABLE_ID_PMT;
      req_param.para2     = param;
      req_param.context =
        MAKE_REQUEST_CONTEXT(p_this->pgid, 0, p_this->repeat_times);
      req_param.ts_in = p_this->ts_in;
      p_this->p_dvb_svc->do_cmd(p_this->p_dvb_svc,
        DVB_REQUEST, (u32)&req_param, sizeof(dvb_request_t));
      //p_this->svc_id[i] = INVALID_SVC_ID;
      OS_PRINTF("CAS: request pmt, pid %d, sid %d\n", p_msg->para1, p_msg->para2);
    }
  }
}

static void on_cas_play_info(handle_t handle, cas_sid_t *p_info)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  u8 i = 0;
  u32 temp_tp = 0;

  p_this->svc_num = p_info->num;

  CHECK_FAIL_RET_VOID(p_this->svc_num > 0);
  CHECK_FAIL_RET_VOID(p_this->svc_num <= MAX_CAS_SLOT_NUM);

  p_this->pgid = p_info->pgid;

  for(i = 0; i < p_this->svc_num; i++)
  {
    p_this->svc_id[i] = p_info->pg_sid_list[i];
  }

  p_this->pmt_id = p_info->pmt_pid;

  temp_tp = p_info->tp_freq;

  p_this->lock_mode = p_info->lock_mode;

  if (DVB_S == p_info->lock_mode)
  {
     if(memcmp(&(p_info->nc_search_info), &(p_this->nc_search_info), 
                sizeof(nc_channel_info_t)) != 0)
     {
        p_this->change_tp_flag = TRUE;
        memcpy(&(p_this->nc_search_info), &(p_info->nc_search_info), sizeof(nc_channel_info_t));
     }
     
  }
  else
  {
    if (p_this->cur_tp_freq != temp_tp)
    {
      p_this->change_tp_flag = TRUE;
    }

    p_this->cur_tp_freq = temp_tp;
  }

  p_this->sm = CAS_SM_CHECK_REQ;

  p_this->ca_sys_id = p_info->ca_sys_id;

  OS_PRINTF("CAS: set s id %d, tp_freq %d\n", p_this->svc_id[0], p_this->cur_tp_freq);
}


static void on_cas_set_sid(handle_t handle, cas_sid_t *p_info)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  u8 i = 0;
  u32 temp_tp = 0;

  if(p_this->pgid == p_info->pgid)
  {
    p_this->repeat_times++;
  }
  else
  {
    p_this->repeat_times = 0;
  }

  p_this->svc_num = p_info->num;

  CHECK_FAIL_RET_VOID(p_this->svc_num > 0);
  CHECK_FAIL_RET_VOID(p_this->svc_num <= MAX_CAS_SLOT_NUM);

  p_this->pgid = p_info->pgid;

  for(i = 0; i < p_this->svc_num; i++)
  {
    p_this->svc_id[i] = p_info->pg_sid_list[i];
  }

  p_this->pmt_id = p_info->pmt_pid;

  temp_tp = p_info->tp_freq;
  
  p_this->lock_mode = p_info->lock_mode;
  
  if (DVB_S == p_info->lock_mode)
  {
     if(memcmp(&(p_info->nc_search_info), &(p_this->nc_search_info), 
            sizeof(nc_channel_info_t)) != 0)
     {
        p_this->change_tp_flag = TRUE;
        memcpy(&(p_this->nc_search_info), &(p_info->nc_search_info), sizeof(nc_channel_info_t));
     }
     //memcpy(&(p_this->nc_search_info), &(p_info->nc_search_info), sizeof(nc_channel_info_t));
  }
  else
  {
    if (p_this->cur_tp_freq != temp_tp)
    {
      p_this->change_tp_flag = TRUE;
    }

    p_this->cur_tp_freq = temp_tp;
  }

  p_this->sm = CAS_SM_WAITTING_LOCK;

  p_this->pmt_version = INVALID;

  p_this->ca_sys_id = p_info->ca_sys_id;

  OS_PRINTF("CAS: set s id %d, tp_freq %d, pmt_id %d\n",
    p_this->svc_id[0], p_this->cur_tp_freq, p_this->pmt_id);
}

static void on_timeout(handle_t handle, u32 table_id)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;

  switch(table_id)
  {
  case DVB_TABLE_ID_PAT:
    OS_PRINTF("CAS: pat timeout\n");
    pat_request(handle);
    break;
  case DVB_TABLE_ID_PMT:
    OS_PRINTF("CAS: pmt timeout\n");
    pmt_request(handle, p_this->pmt_id, p_this->svc_id[0]);
    p_this->pmt_ticks = mtos_ticks_get();
    break;
  case DVB_TABLE_ID_CAT:
    OS_PRINTF("CAS: cat timedout with sid\n");
    cat_request(handle, p_this->svc_id[0]);
    break;
  case DVB_TABLE_ID_NIT_ACTUAL:
    OS_PRINTF("CAS: nit timeout\n");
    //p_this->p_policy->on_timeout(p_sec->table_id, p_sec->r_context);
    nit_request(handle);
    break;
  }
}

static void on_dvb_msg(handle_t handle, os_msg_t *p_msg)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  u16 pgid = GET_REQUEST_ID(p_msg->context);
  u8 repeat_times = GET_REQUEST_DATA2(p_msg->context);
  u32 ret = 0;
  cas_table_info_t ca_table_info = {0};
  cas_desc_t cas_ecm_info[MAX_ECM_DESC_NUM] = {{0},};
  u8 i = 0;
  u16 ecm_num = MAX_ECM_DESC_NUM;
  //OS_PRINTF("CAS: curn %d, repeat %d\n", p_this->repeat_times, repeat_times);
  //if((p_this->pgid != pgid) || (repeat_times != p_this->repeat_times))
  //{
    //OS_PRINTF("CAS: p_this->pgid %d, pgid %d, drop msg 0x%x, extend_data 0x%x\n",
      //p_this->pgid, pgid, p_msg->content, p_msg->context);
    //return;
  //}

  switch(p_msg->content)
  {
    case DVB_PAT_FOUND:
      //OS_PRINTF("CAS: pat found, ts_id = %d\n", p_msg->para1);
      if((p_this->pgid != pgid) || (repeat_times != p_this->repeat_times))
      {
        OS_PRINTF("repeat req pat\n");
        pat_request(handle);
        p_this->pmt_ticks = mtos_ticks_get();
        return;
      }

      p_this->pat_req_flag = FALSE;
      break;
    case DVB_PAT_PMT_INFO:
      //OS_PRINTF("CAS: pmt info found\n");
      if((p_this->pgid != pgid) || (repeat_times != p_this->repeat_times))
      {
        //OS_PRINTF("repeat req pat\n");
        //pat_request(handle);
        //p_this->pmt_ticks = mtos_ticks_get();
        return;
      }

      on_pmt_info_found(handle, p_msg);
      break;
    case DVB_PMT_FOUND:
    {
      if((p_this->pgid != pgid) || (repeat_times != p_this->repeat_times))
      {
        OS_PRINTF("repeat req pmt\n");
        if (INVALID_PID != p_this->pmt_id)
        {
          pmt_request(handle, p_this->pmt_id, p_this->svc_id[0]);
        }
        return;
      }
    }
    {
      pmt_t *p_pmt = (pmt_t *)p_msg->para1;
      if (p_this->p_policy->test_ca_performance != NULL)
      {
        p_this->p_policy->test_ca_performance(CAS_PMT_DATA_STEP, mtos_ticks_get());
      }
      //OS_PRINTF("CAS: pmt found %x\n", mtos_ticks_get());
      //p_this->p_policy->on_pmt((pmt_t *)p_msg->para1, p_msg->context);
      p_this->pmt_req_flag = FALSE;
      if (NULL != p_this->p_policy->update_ecm_info)
      {
        if (p_this->p_policy->update_ecm_info(p_pmt->p_origion_data, cas_ecm_info,
                     (u16 *)&ecm_num))
        {
          ca_table_info.service_id = p_this->svc_id[0];

          for (i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
          {
            ca_table_info.ecm_info[i].ca_system_id = cas_ecm_info[i].ca_sys_id;
            ca_table_info.ecm_info[i].ecm_id = cas_ecm_info[i].ecm_pid;
            ca_table_info.ecm_info[i].es_pid = cas_ecm_info[i].es_pid;
          }
          cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_SET_ECMEMM, &ca_table_info);
        }
      }

      if (p_this->pmt_version != p_pmt->version)
      {
        OS_PRINTF("pmt version change org %d, new %d\n", p_this->pmt_version, p_pmt->version);
        p_this->pmt_version = p_pmt->version;
        cas_table_process(CAS_TID_PMT, p_pmt->p_origion_data, &ret);
      }
      break;
    }
    case DVB_CAT_FOUND:
    {
      if((p_this->pgid != pgid) || (repeat_times != p_this->repeat_times))
      {
        OS_PRINTF("repeat req cat\n");
        cat_request(handle, p_this->svc_id[0]);
        return;
      }
    }
    {
      cat_cas_t *p_cat = (cat_cas_t *)p_msg->para1;
      //OS_PRINTF("CAS: cat found %x\n", mtos_ticks_get());
      if (p_this->p_policy->test_ca_performance != NULL)
      {
        p_this->p_policy->test_ca_performance(CAS_CAT_DATA_STEP, mtos_ticks_get());
      }
      if (p_this->change_tp_flag)
      {
        cas_table_process(CAS_TID_CAT, p_cat->p_origion_data, &ret);
        p_this->change_tp_flag = FALSE;
        //p_this->last_tp_freq = p_this->cur_tp_freq;
      }
      p_this->cat_req_flag = FALSE;
      break;
    }
    case DVB_TABLE_TIMED_OUT:
      OS_PRINTF("CAS : time out table %d\n",p_msg->para1);
      on_timeout(handle, p_msg->para1);
      break;
    case DVB_NIT_FOUND:
      //OS_PRINTF("CAS: nit found, len %d\n", p_msg->para2);
      if (p_this->p_policy->test_ca_performance != NULL)
      {
        p_this->p_policy->test_ca_performance(CAS_NIT_DATA_STEP, mtos_ticks_get());
      }
      p_this->nit_req_flag = FALSE;
      cas_table_process(CAS_TID_NIT, ((nit_t *)p_msg->para1)->p_origion_data, &ret);
    default:
      break;
  }
}

static void cas_set_desc_info(handle_t handle, cas_sid_t *p_ca_set)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  cas_channel_state_set channel_set = {0};
  cas_table_info_t ca_table_info = {0};
  u8 i = 0;
  u8 num = 0;

  cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_LAST_PRG_INFO_CLR, NULL);

  //set cur audio, video pid
  cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_VIDEO_PID_SET, &(p_ca_set->v_pid));
  cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_AUDIO_PID_SET, &(p_ca_set->a_pid));

  //set cur pg state
  channel_set.channel_id = p_this->pgid;
  channel_set.state = p_this->change_tp_flag ? 0 : 1;
  channel_set.org_network_id = p_ca_set->org_network_id;
  channel_set.ts_id = p_ca_set->ts_id;
  channel_set.pmt_pid = p_ca_set->pmt_pid;
  channel_set.a_pid = p_ca_set->a_pid;
  channel_set.v_pid = p_ca_set->v_pid;
  channel_set.service_id = p_ca_set->pg_sid_list[0];
  cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_CHANNEL_STATE_SET,
                  (void *)&channel_set);

  //set ecm and emm info
  ca_table_info.service_id = p_ca_set->pg_sid_list[0];

  num = (p_ca_set->ecm_num > CAS_MAX_ECMEMM_NUM) ? CAS_MAX_ECMEMM_NUM : p_ca_set->ecm_num;
  for (i = 0; i < num; i++)
  {
    ca_table_info.ecm_info[i].ca_system_id = p_ca_set->ecm_info[i].ca_sys_id;
    ca_table_info.ecm_info[i].ecm_id = p_ca_set->ecm_info[i].ecm_pid;
    ca_table_info.ecm_info[i].es_pid = p_ca_set->ecm_info[i].es_pid;
  }

  num = (p_ca_set->emm_num > CAS_MAX_ECMEMM_NUM) ? CAS_MAX_ECMEMM_NUM : p_ca_set->emm_num;
  for (i = 0; i < num; i++)
  {
    ca_table_info.emm_info[i].ca_system_id = p_ca_set->emm_info[i].ca_sys_id;
    ca_table_info.emm_info[i].emm_id = p_ca_set->emm_info[i].emm_pid;
  }
  cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_DMX_SLOT_SET, &p_this->ts_in);
  cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_SET_ECMEMM, &ca_table_info);
}

static void cas_process_msg(handle_t handle, os_msg_t *p_msg)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  event_t evt = {CAS_SET_SID, 0, 0};
  RET_CODE ret = 0;
  u32 temp_addr = 0;
  u32 *p_temp = NULL;
  cas_io_param_t cas_param = {0};
  u32 slot = CAS_INVALID_SLOT;
  cas_module_id_t cam_id = CAS_UNKNOWN;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  switch(p_msg->content)
  {
    case M_SVC_DATA_RESET:
      OS_PRINTF("cas get monitor info %d\n", p_msg->para1);
      process_reset_data(p_this, p_msg->para1, M_SECTION_DATA);
      break;
    case M_SVC_VER_CHANGED:
    case M_SVC_DATA_READY:
      process_normal_data(p_this, p_msg->para1, M_SECTION_DATA);
      break;
    case CAS_CMD_START:
      break;
    case CAS_CMD_ZONE_CHECK:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_ZONE_CHECK, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_TEST, 0, 0);
      break;
    case CAS_CMD_SET_ZONE_CODE:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_SET_ZONE_CODE, &cas_param);
      break;
    case CAS_CMD_STOP:
      cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_STOP, NULL);
      p_this->sm = CAS_SM_IDLE;
      break;
    case CAS_CMD_STOP_SYNC:
      cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_STOP, NULL);
      p_this->sm = CAS_SM_IDLE;
      evt.id = CAS_EVT_STOPED;
      ap_frm_send_evt_to_ui(APP_CA, &evt);
      break;
    case CAS_CMD_STOP_CAS:
      cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_STOP_CAS, NULL);
      p_this->sm = CAS_SM_IDLE;
      p_this->pmt_req_flag = FALSE;
      p_this->pat_req_flag = FALSE;
      p_this->cat_req_flag = FALSE;
      p_this->change_tp_flag = TRUE;
      break;
    case CAS_CMD_STOP_CAS_SYNC:
      cas_descrambler_ctrl(p_this->ca_sys_id, CAS_IOCMD_STOP_CAS, NULL);
      p_this->sm = CAS_SM_IDLE;
      p_this->change_tp_flag = TRUE;
      p_this->pmt_req_flag = FALSE;
      p_this->pat_req_flag = FALSE;
      p_this->cat_req_flag = FALSE;
      evt.id = CAS_EVT_STOPED_CAS;
      ap_frm_send_evt_to_ui(APP_CA, &evt);
      break;
    case CAS_CMD_PLAY_INFO_SET:
    case CAS_CMD_PLAY_INFO_SET_ANSYNC:
    {
      cas_sid_t sid_t = {0};
      memcpy(&sid_t, (cas_sid_t *)p_msg->para1, sizeof(cas_sid_t));

      on_cas_play_info(handle, &sid_t);

      cas_set_desc_info(handle, &sid_t);

      if (p_this->p_policy->test_ca_performance != NULL)
      {
        p_this->p_policy->test_ca_performance(CAS_PLAY_REQ_STEP, mtos_ticks_get());
      }
      if (CAS_CMD_PLAY_INFO_SET == p_msg->content)
      {
        evt.id = CAS_EVT_PLAY_INFO_SET;
        ap_frm_send_evt_to_ui(APP_CA, &evt);
      }
    }
      break;
    case CAS_CMD_SID_SET:
    case CAS_CMD_SID_SET_ANSYNC:
    {
      cas_sid_t sid_t = {0};
      memcpy(&sid_t, (cas_sid_t *)p_msg->para1, sizeof(cas_sid_t));

      on_cas_set_sid(handle, &sid_t);
      p_this->tuner_id = p_msg->para2;
      nc_get_ts_by_tuner(nc_handle, p_this->tuner_id, &p_this->ts_in);
      cas_set_desc_info(handle, &sid_t);

      if (p_this->p_policy->test_ca_performance != NULL)
      {
        p_this->p_policy->test_ca_performance(CAS_PLAY_REQ_STEP, mtos_ticks_get());
      }

      if (CAS_CMD_SID_SET == p_msg->content)
      {
        ap_frm_send_evt_to_ui(APP_CA, &evt);
      }
    }
    break;

    case CAS_CMD_OPERATOR_INFO_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_OPERATOR_MEM_ID, CAS_OPERATOR_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_OPERATOR_SIZE);
      OS_PRINTF("get mem addr 0x%x, LINE=%d,size %d\n", temp_addr,
                                __LINE__, CAS_OPERATOR_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_OPERATOR_INFO_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_OPERATOR_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_IPP_REC_INFO_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_IPP_RECORD_MEM_ID, CAS_IPP_REC_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_IPP_REC_SIZE);
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            temp_addr,__LINE__, CAS_IPP_REC_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_IPP_EXCH_RECORD_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_IPP_REC_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_ENTITLE_INFO_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_ENTITLE_MEM_ID, CAS_ENTITLE_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_ENTITLE_SIZE);
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            temp_addr,__LINE__, CAS_ENTITLE_SIZE);
      cam_id = (p_msg->para2 & 0xFFFF);
      slot = ((p_msg->para2 >> 16) & 0xFFFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;

      ((product_entitles_info_t *)temp_addr)->operator_id = p_msg->para1;

      // for tr cas
      ((product_entitles_info_t *)temp_addr)->start_pos = ((p_msg->para1 >> 16) & 0xFFFF);
      ((product_entitles_info_t *)temp_addr)->end_pos = (p_msg->para1 & 0xFFFF);

      ret = cas_io_ctrl(slot, CAS_IOCMD_ENTITLE_INFO_GET, &cas_param);

      cas_send_evt_to_ui(CAS_EVT_ENTITLE_INFO, (u32)ret, temp_addr);
      break;

    case CAS_CMD_IPPV_INFO_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_IPPV_MEM_ID, CAS_IPPV_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_IPPV_SIZE);
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                              temp_addr,__LINE__, CAS_IPPV_SIZE);
      ((ipp_info_t *)temp_addr)->operator_id = p_msg->para1;
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_IPPV_INFO_GET, &cas_param);

      cas_send_evt_to_ui(CAS_EVT_IPPV_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_ACLIST_INFO_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_ACLIST_MEM_ID, CAS_ACLIST_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_ACLIST_SIZE);
      OS_PRINTF("get mem addr 0x%x, LINE=%d\n", temp_addr,__LINE__);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      p_temp = ((u32 *)temp_addr) ;
      *p_temp = p_msg->para1;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_ACLIST_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_ACLIST_INFO, (u32)ret, temp_addr);
      break;
     case CAS_CMD_READ_FEED_DATA:
      temp_addr = cas_get_mem_addr(handle, CAS_MON_CHILD_MEM_ID, CAS_MON_CHILD_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_MON_CHILD_SIZE);      
      OS_PRINTF("get mem addr 0x%x, LINE=%d\n", temp_addr,__LINE__);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;

      ((chlid_card_status_info *)temp_addr)->operator_id = p_msg->para1;
      cas_param.p_param = (void *)(temp_addr);
      ret = cas_io_ctrl(slot, CAS_IOCMD_READ_FEED_DATA, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_READ_FEED_DATA, (u32)ret, temp_addr);
      break;
    case CAS_CMD_IPPV_INFO_SYNC_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_IPPV_MEM_ID, CAS_IPPV_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_IPPV_SIZE);          
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                                  temp_addr,__LINE__, CAS_IPPV_SIZE);
      ((ipp_info_t *)temp_addr)->operator_id = p_msg->para1;
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_IPPV_INFO_GET, &cas_param);

      cas_send_evt_to_ui(CAS_EVT_PPV_SYNC_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_BURSE_INFO_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_BURSE_MEM_ID, CAS_BURSE_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_BURSE_SIZE);           
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            temp_addr,__LINE__, CAS_BURSE_SIZE);
      ((burses_info_t *)temp_addr)->operator_id = p_msg->para1;
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_BURSE_INFO_GET, &cas_param);

      cas_send_evt_to_ui(CAS_EVT_BURSE_INFO, (u32)ret, temp_addr);
      break;

    case CAS_CMD_NEW_MAIL_CHECK:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;   
      OS_PRINTF("FUNCTION = %s, LINE=%d\n", __FUNCTION__, __LINE__);
      cas_param.p_param = NULL;
      ret = cas_io_ctrl(slot, CAS_IOCOM_NEW_MAIL_CHECK, &cas_param);
      break;
      
    case CAS_CMD_MAIL_HEADER_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_MAIL_HEADER_MEM_ID, CAS_MAIL_HEADER_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_MAIL_HEADER_SIZE);             
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            temp_addr,__LINE__, CAS_MAIL_HEADER_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      OS_PRINTF("cam_id %d, slot=%d, para2 0x%x\n", 
                            cam_id,slot, p_msg->para2);
      ret = cas_io_ctrl(slot, CAS_IOCMD_MAIL_HEADER_GET, &cas_param);
      OS_PRINTF("cas_send_evt_to_ui, ret %d\n", 
                            ret);

      cas_send_evt_to_ui(CAS_EVT_MAIL_HEADER_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_MAIL_BODY_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_MAIL_BODY_MEM_ID, CAS_MAIL_BODY_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_MAIL_BODY_SIZE);          
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            temp_addr,__LINE__, CAS_MAIL_BODY_SIZE);
      ((cas_mail_body_t *)temp_addr)->mail_id = p_msg->para1;
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_MAIL_BODY_GET, &cas_param);

      cas_send_evt_to_ui(CAS_EVT_MAIL_BODY_INFO, (u32)ret, temp_addr);
      break;

    case CAS_CMD_ANNOUNCE_HEADER_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_MAIL_HEADER_MEM_ID, CAS_MAIL_HEADER_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_MAIL_HEADER_SIZE);           
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                          temp_addr,__LINE__, CAS_MAIL_HEADER_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_ANNOUNCE_HEADER_GET, &cas_param);

      cas_send_evt_to_ui(CAS_EVT_ANNOUNCE_HEADER_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_ANNOUNCE_BODY_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_MAIL_BODY_MEM_ID, CAS_MAIL_BODY_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_MAIL_BODY_SIZE);           
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                          temp_addr,__LINE__, CAS_MAIL_BODY_SIZE);
      ((cas_announce_body_t *)temp_addr)->mail_id = p_msg->para1;
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_ANNOUNCE_BODY_GET, &cas_param);

      cas_send_evt_to_ui(CAS_EVT_ANNOUNCE_BODY_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_CARD_INFO_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_CARD_MEM_ID, CAS_CARD_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_CARD_SIZE);             
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            temp_addr,__LINE__, CAS_CARD_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ((cas_card_info_t *)temp_addr)->crypg_info.level = p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_CARD_INFO_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_CARD_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_LIB_VERSION_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_LIB_VERSION_MEM_ID, CAS_LIB_VERSION_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_LIB_VERSION_SIZE);               
      OS_PRINTF("get mem addr 0x%x, LINE=%d\n", temp_addr,__LINE__);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      memset((u8 *)temp_addr, 0, 100);
      ret = cas_io_ctrl(slot, CAS_IOCMD_CA_SYS_ID_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_LIB_VERSION, (u32)ret, temp_addr);
      break;
    case CAS_CMD_PLATFORM_ID_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_PLAT_FORM_MEM_ID, CAS_PLATFORM_ID_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_PLATFORM_ID_SIZE);              
      OS_PRINTF("get mem addr 0x%x, LINE=%d\n", temp_addr,__LINE__);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_STB_SERIAL_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_PLATFORM_ID, (u32)ret, temp_addr);
      break;
    case CAS_CMD_RATING_GET:
      {
        u8 rating = 0;
        cam_id = (p_msg->para2 & 0xFF);
        slot = ((p_msg->para2 >> 16) & 0xFF);
        cas_param.cam_id = cam_id;
        cas_param.p_param = (void *)&rating;
        ret = cas_io_ctrl(slot, CAS_IOCMD_RATING_GET, &cas_param);
        cas_send_evt_to_ui(CAS_EVT_RATING_INFO, (u32)ret, (u32)rating);
      }
      break;
    case CAS_CMD_CERTIFICATE_CONFIRM_SERVICE:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_GET_CERTIFICATE_SERVICE, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_CERTIFICATE_CONFIRM_SERVICE, (u32)ret, temp_addr);
    case CAS_CMD_WORK_TIME_GET:
        temp_addr = cas_get_mem_addr(handle, CAS_WORK_TIME_GET_MEM_ID, CAS_WORK_TIME_GET_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_WORK_TIME_GET_SIZE);                
        cam_id = (p_msg->para2 & 0xFF);
        slot = ((p_msg->para2 >> 16) & 0xFF);
        cas_param.cam_id = cam_id;
        cas_param.p_param = (void *)temp_addr;
        ret = cas_io_ctrl(slot, CAS_IOCMD_WORK_TIME_GET, &cas_param);
        cas_send_evt_to_ui(CAS_EVT_WORK_TIME_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_PIN_SET:
      temp_addr = cas_get_mem_addr(handle, CAS_PIN_MEM_ID, CAS_PIN_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_PIN_SIZE);           
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      temp_addr = (u32)p_msg->para1;
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_PIN_SET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_PIN_SET, (u32)ret, (u32)cas_param.p_param);
      break;
    case CAS_CMD_PIN_STATE_GET:
      {
        u8 state = 0;
        cam_id = (p_msg->para2 & 0xFF);
        slot = ((p_msg->para2 >> 16) & 0xFF);
        cas_param.cam_id = cam_id;
        cas_param.p_param = (void *)&state;
        ret = cas_io_ctrl(slot, CAS_IOCMD_PIN_STATE_GET, &cas_param);
        cas_send_evt_to_ui(CAS_EVT_PIN_STATE_INFO, (u32)ret, (u32)state);
      }
      break;
    case CAS_CMD_PIN_ENABLE:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_PIN_ENABLE, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_PIN_ENABLE, (u32)ret, temp_addr);
      break;
    case CAS_CMD_PIN_VERIFY:
      temp_addr = cas_get_mem_addr(handle, CAS_PIN_VERIFY_ID, CAS_PIN_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_PIN_SIZE);               
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      temp_addr = (u32)p_msg->para1;
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_PIN_VERIFY, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_PIN_VERIFY, (u32)ret, temp_addr);
      break;
    case CAS_CMD_UNLOCK_PIN_SET:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      temp_addr = (u32)p_msg->para1;
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_PARENT_UNLOCK_SET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_UNLOCK_PARENTAL_RESULT, (u32)ret, temp_addr);
      break;
    case CAS_CMD_WORK_TIME_SET:
      temp_addr = cas_get_mem_addr(handle, CAS_WORK_TIME_SET_MEM_ID,CAS_WORK_TIME_SET_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_WORK_TIME_SET_SIZE);   
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      temp_addr = (u32)p_msg->para1;
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_WORK_TIME_SET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_WORK_TIME_SET, (u32)ret, temp_addr);
      break;
    case CAS_CMD_RATING_SET:
      temp_addr = cas_get_mem_addr(handle, CAS_RATING_SET_MEM_ID,CAS_RATING_SET_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_RATING_SET_SIZE);         
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      temp_addr = (u32)p_msg->para1;
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_RATING_SET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_RATING_SET, (u32)ret, temp_addr);
      break;
    case CAS_CMD_MON_CHILD_STATUS_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_MON_CHILD_MEM_ID, CAS_MON_CHILD_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_MON_CHILD_SIZE);              
      OS_PRINTF("get mem addr 0x%x, LINE=%d, sz %d\n", 
                            temp_addr,__LINE__, CAS_MON_CHILD_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;

      ((chlid_card_status_info *)temp_addr)->operator_id = p_msg->para1;
      cas_param.p_param = (void *)(temp_addr);
      ret = cas_io_ctrl(slot, CAS_IOCMD_MON_CHILD_STATUS_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_MON_CHILD_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_PAIRE_STATUS_GET:
      temp_addr = cas_get_mem_addr(handle, CAS_CA_PAIR_STBCARD_ID, CAS_CMD_CA_JUDGE_CARD_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_CMD_CA_JUDGE_CARD_SIZE);         
      OS_PRINTF("get mem addr 0x%x, LINE=%d, sz %d\n", 
                            temp_addr,__LINE__, CAS_CMD_CA_JUDGE_CARD_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)(temp_addr);
      ret = cas_io_ctrl(slot, CAS_IOCMD_PATERNER_STA_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_PAIRED_STA, (u32)ret, temp_addr);
      break;
    case CAS_CMD_IPP_BUY_SET:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_PURCHASE_SET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_IPP_BUY_RESULT, (u32)ret, temp_addr);
      break;
    case CAS_CMD_MON_CHILD_FEED:
      temp_addr = p_msg->para1;
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_MON_CHILD_STATUS_SET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_MON_CHILD_FEED, (u32)ret, temp_addr);
      break;
    case CAS_CMD_MAIL_DEL_ONE:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)&(p_msg->para1);
      ret = cas_io_ctrl(slot, CAS_IOCMD_MAIL_DELETE_BY_INDEX, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_MAIL_DEL_RESULT, (u32)ret, 0);
      break;
    case CAS_CMD_MAIL_DEL_ALL:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = NULL;
      ret = cas_io_ctrl(slot, CAS_IOCMD_MAIL_DELETE_ALL, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_MAIL_DEL_RESULT, (u32)ret, 0);
      break;
    case CAS_CMD_FACTORY_SET:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = NULL;
      ret = cas_io_ctrl(slot, CAS_IOCMD_FACTORY_SET, &cas_param);
      break;
    case CAS_CMD_MAIL_CHANGE_STATUS:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)&(p_msg->para1);
      ret = cas_io_ctrl(slot, CAS_IOCMD_MAIL_CHANGE_STATUS, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_MAIL_CHANGE_STATUS, (u32)ret, 0);
      break;
    case CAS_CMD_ANNOUNCE_DEL_ONE:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)&(p_msg->para1);
      ret = cas_io_ctrl(slot, CAS_IOCMD_ANNOUNCE_DELETE_BY_INDEX, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_ANNOUNCE_DEL_RESULT, (u32)ret, 0);
      break;
    case CAS_CMD_ANNOUNCE_DEL_ALL:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = NULL;
      ret = cas_io_ctrl(slot, CAS_IOCMD_ANNOUNCE_DELETE_ALL, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_ANNOUNCE_DEL_RESULT, (u32)ret, 0);
      break;
    case CAS_CMD_GET_IPP_BUY_INFO:
      temp_addr = cas_get_mem_addr(handle, CAS_IPP_BUY_MEM_ID, CAS_IPP_BUY_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_IPP_BUY_SIZE);        
      OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            temp_addr,__LINE__, CAS_IPP_BUY_SIZE);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_IPP_BUY_INFO_GET, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_PPV_BUY_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_CA_DEBUG:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)&(p_msg->para1);
      ret = cas_io_ctrl(slot, CAS_IOCMD_SMC_OPEN_DEBUG, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_DEBUG_EN, (u32)ret, 0);
      break;
    case CAS_CMD_OVERDUE_INFO_SERVICE_SET:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)p_msg->para1;
      ret = cas_io_ctrl(slot, CAS_IOCMD_SET_OVERDUE_INFO_SERVICE, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_OVERDUE_INFO_SERVICE_SET, (u32)ret, temp_addr);
      break;
    case CAS_CMD_CA_OSD_SHOW_TIME:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)&(p_msg->para1);
      ret = cas_io_ctrl(slot, CAS_IOCMD_OSD_MSG_SHOW_END, &cas_param);
      break;
    case CAS_CMD_OVERDUE_INFO_SERVICE_GET:
      temp_addr = cas_get_mem_addr(handle,
                                   CAS_OVERDUE_INFO_SERVICE_GET_MEM_ID,
                                   CAS_OVERDUE_INFO_SERVICE_GET_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_OVERDUE_INFO_SERVICE_GET_SIZE);         
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_GET_OVERDUE_INFO_SERVICE, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_OVERDUE_INFO_SERVICE_INFO, (u32)ret, temp_addr);
      break;
    case CAS_CMD_GET_EMERGENCY_BROADCASTING_INFO:
      temp_addr = cas_get_mem_addr(handle, CAS_FORCE_CHANNEL_MEM_ID, CAS_FORCE_CHANNEL_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_FORCE_CHANNEL_SIZE);             
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      cas_param.p_param = (void *)temp_addr;
      cas_io_ctrl(slot, CAS_IOCMD_FORCE_CHANNEL_INFO_GET, &cas_param);
      evt.id = CAS_EVT_FORCE_CHANNEL_INFO;
      evt.data1 = (slot << 16) | cas_param.cam_id;
      evt.data2 = temp_addr;
      ap_frm_send_evt_to_ui(APP_CA, &evt);
      break;
    case CAS_CMD_OSD_ROLL_OVER:
     cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = NULL;
      ret = cas_io_ctrl(slot, CAS_IOCMD_OSD_MSG_SHOW_END, &cas_param);
    break;
    case CAS_CMD_CA_JUDGE_CARD_TYPE:
      temp_addr = cas_get_mem_addr(handle, CAS_CA_PAIR_STBCARD_ID,sizeof(u32));
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, sizeof(u32));              
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_MON_CHILD_IDENTIFY, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_MON_CHILD_IDENTIFY, (u32)ret, temp_addr);
      break;
    case CAS_CMD_SET_CA_LANGUAGE:
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)&(p_msg->para1);
      ret = cas_io_ctrl(slot, CAS_IOCMD_LANGUAGE_SET, &cas_param);
    break;
    case CAS_CMD_REQUEST_MASK_BUFFER:
      OS_PRINTF("CAS_CMD_REQUEST_MASK_BUFFER\n");
      cam_id = (p_msg->para2 & 0xFF);
      cas_param.cam_id = cam_id;
      slot = ((p_msg->para2 >> 16) & 0xFF);
      ret = cas_io_ctrl(slot, CAS_IOCMD_REQUEST_MASK_BUFFER, &cas_param);
      break;
    case CAS_CMD_REQUEST_UPDATE_BUFFER:
      OS_PRINTF("CAS_CMD_REQUEST_UPDATE_BUFFER\n");
      cam_id = (p_msg->para2 & 0xFF);
      cas_param.cam_id = cam_id;
      slot = ((p_msg->para2 >> 16) & 0xFF);
      ret = cas_io_ctrl(slot, CAS_IOCMD_REQUEST_UPDATE_BUFFER, &cas_param);
      break;
    case CAS_CMD_CA_GET_FORCE_EMAIL:
      temp_addr = cas_get_mem_addr(handle, CAS_MAIL_BODY_MEM_ID, CAS_MAIL_BODY_SIZE);
      CHECK_FAIL_RET_VOID(temp_addr != 0);
      memset((void *)temp_addr, 0x0, CAS_MAIL_BODY_SIZE);        
      OS_PRINTF("get mem addr 0x%x, LINE=%d\n", temp_addr,__LINE__);
      cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = (void *)temp_addr;
      ret = cas_io_ctrl(slot, CAS_IOCMD_GET_IMPORTANT_MAIL, &cas_param);
      cas_send_evt_to_ui(CAS_EVT_FORCE_MSG, (u32)ret, temp_addr);
      break;
    case CAS_CMD_GET_OSD_INFO :
     cam_id = (p_msg->para2 & 0xFF);
      slot = ((p_msg->para2 >> 16) & 0xFF);
      cas_param.cam_id = cam_id;
      cas_param.p_param = NULL;
      ret = cas_io_ctrl(slot, CAS_IOCMD_OSD_GET, &cas_param);
    break;
    default:
      break;
  }
 
  on_dvb_msg(handle, p_msg);
  if(NULL != p_this->p_policy->do_ca_command_by_onself)
  {
    p_this->p_policy->do_ca_command_by_onself(p_msg);
  }

}

static void on_process_section(handle_t handle)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;

  u8 i = 0;

  for(; i < p_this->svc_num; i++)
  {
    if(INVALID_SVC_ID != p_this->svc_id[i])
    {
      if (INVALID_PID == p_this->pmt_id)
      {
        if (!p_this->pat_req_flag)
        {
          pat_request(handle);
        }
      }
      else
      {
        if (!p_this->pmt_req_flag)
        {
          pmt_request(handle, p_this->pmt_id, p_this->svc_id[i]);
        }
      }

      //nit_request(handle);
      if (!p_this->cat_req_flag)
      {
        cat_request(handle, p_this->svc_id[i]);
      }
      p_this->sm = CAS_SM_MONITOR;
    }
  }
}

static void cas_state_machine(handle_t handle)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  nc_channel_info_t tp_info = {0};
  BOOL is_lock = FALSE;
#ifndef WIN32
  ap_cas_evt_t event = {0};
  cas_event_id_t cas_event = CAS_EVT_NONE;
  event_t evt = {0, 0, 0};
  cas_io_param_t cas_param = {0};
  u32 addr = 0;
#endif
  switch(p_this->sm)
  {
    case CAS_SM_IDLE:
      break;
    case CAS_SM_CHECK_REQ:
      if (p_this->nit_get_flag)
      {
        p_this->nit_get_flag = FALSE;
        process_normal_data(p_this, M_NIT_TABLE, M_SECTION_DATA);
      }
      break;
    case CAS_SM_WAITTING_LOCK:
      nc_get_tp(nc_handle, p_this->tuner_id, &tp_info);
      is_lock = nc_get_lock_status(nc_handle, p_this->tuner_id);
      if (DVB_S == p_this->lock_mode)
      {
        p_this->nc_search_info.channel_info.nim_type = tp_info.channel_info.nim_type;
        p_this->nc_search_info.channel_info.param.dvbs.nim_type = 
                          tp_info.channel_info.param.dvbs.nim_type;
        if (memcmp(&tp_info, &(p_this->nc_search_info), sizeof(nc_channel_info_t)) == 0)
        {
            on_process_section(handle);
            p_this->pmt_ticks = mtos_ticks_get();
        }
      }
      else
      {
        if ((p_this->cur_tp_freq == tp_info.channel_info.frequency) && is_lock)
        {
            //OS_PRINTF("CAS: tunner info match\n\n");
            //p_this->sm = CAS_SM_PROCESS_SECTIONS;
            on_process_section(handle);
            p_this->pmt_ticks = mtos_ticks_get();
        }
      }
      break;
    case CAS_SM_MONITOR:
      if (INVALID_PID == p_this->pmt_id)
      {
        if ((!p_this->pat_req_flag) && (mtos_ticks_get() - p_this->pmt_ticks) > 800)
        {
          pat_request(handle);
          p_this->pmt_ticks = mtos_ticks_get();
        }
      }
      else
      {
        if ((!p_this->pmt_req_flag) && (mtos_ticks_get() - p_this->pmt_ticks) > 500)
        {
          pmt_request(handle, p_this->pmt_id, p_this->svc_id[0]);
        }
      }

      if ((p_this->nit_get_flag) && (!p_this->nit_req_flag))
      {
        nit_request(handle);
        p_this->nit_get_flag = FALSE;
      }
      //no response for pmt request, clear the flag
      if (p_this->pmt_req_flag && (mtos_ticks_get() - p_this->pmt_ticks) > 1000)
      {
        OS_PRINTF("TRACE: NO response for pmt request, clear the flag\n");
        p_this->pmt_req_flag = FALSE;
        p_this->pat_req_flag = FALSE;
        p_this->cat_req_flag = FALSE;
      }

      break;
    default:
      CHECK_FAIL_RET_VOID(0);
      break;
  }

#ifndef WIN32
 if(SUCCESS == cas_events_get(p_this, &event))
{
    cas_event = event.event;

    //OS_PRINTF("CAS: parse event[%d]\n", cas_event);
    cas_param.cam_id = event.cam_id;
    switch(cas_event)
    {
      case CAS_S_GET_KEY:
        if (p_this->p_policy->test_ca_performance != NULL)
        {
          p_this->p_policy->test_ca_performance(CAS_DESCRAMBLER_KEY_STEP, mtos_ticks_get());
        }
        return;
      case CAS_C_REQU_NIT_SECITON:
        OS_PRINTF("CAS : get req nit cmd\n");
        p_this->nit_get_flag = TRUE;
        //nit_request(handle);
       evt.id = CAS_CONDITION_SEARCH;
        break;
      case CAS_S_CARD_RST_SUCCESS:
      case CAS_S_ADPT_CARD_RST_SUCCESS:
        OS_PRINTF("CAS : card reset ok\n");
        //p_this->change_tp_flag = TRUE;
        if(NULL != p_this->p_policy->init_ca_module)
        {
          p_this->p_policy->init_ca_module(cas_param.cam_id);
        }
        evt.id = CAS_EVT_RST_SUCCESS;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        break;
      case CAS_E_DISPLAY_USER_NUMBER:
      case CAS_C_SHOW_NEW_FINGER:
        addr = cas_get_mem_addr(handle, CAS_FINGER_MEM_ID, CAS_FINGER_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_FINGER_SIZE);            
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", addr,__LINE__, CAS_FINGER_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_FINGER_MSG_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_FINGER;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_HIDE_NEW_FINGER:
        OS_PRINTF("cas hide new finger  LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_HIDE_FINGER;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_E_POP_MESSAGE:
        addr = cas_get_mem_addr(handle, CAS_SHOW_OSD_MEM_ID, CAS_OSD_MSG_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_OSD_MSG_SIZE);                
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                                addr,__LINE__, CAS_OSD_MSG_SIZE);
        cas_param.p_param = (void *)addr;
        if(SUCCESS == cas_io_ctrl(event.slot_id, CAS_IOCMD_OSD_MSG_GET, &cas_param))
        {
          evt.id = CAS_EVT_SHOW_OSD_MSG;
          evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
          evt.data2 = addr;
        }
        else
        {
          //no message in buffer, return directly
          OS_PRINTF("CAS_E_POP_MESSAGE: no msg in buffer now, return directly\n");
          return;
        }
        break;        
      case CAS_C_SHOW_OSD_UP:
        addr = cas_get_mem_addr(handle, CAS_SHOW_OSD_MEM_ID, CAS_OSD_MSG_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_OSD_MSG_SIZE);        
        OS_PRINTF("get mem addr 0x%x, LINE=%d,size %d\n", 
                              addr,__LINE__, CAS_OSD_MSG_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_OSD_MSG_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_OSD_UP;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_SHOW_OSD_DOWN:
        addr = cas_get_mem_addr(handle, CAS_SHOW_OSD_MEM_ID, CAS_OSD_MSG_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_OSD_MSG_SIZE);   
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                              addr,__LINE__, CAS_OSD_MSG_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_OSD_MSG_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_OSD_DOWN;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_SHOW_MSG:
        addr = cas_get_mem_addr(handle, CAS_SHOW_MSG_MEM_ID, CAS_OSD_MSG_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_OSD_MSG_SIZE);           
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", addr,__LINE__, CAS_OSD_MSG_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_MSG_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_OSD_MSG;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_SHOW_OSD:
        addr = cas_get_mem_addr(handle, CAS_SHOW_OSD_MEM_ID, CAS_OSD_MSG_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_OSD_MSG_SIZE);                
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                                addr,__LINE__, CAS_OSD_MSG_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_OSD_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_OSD_MSG;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_HIDE_OSD_UP:
        OS_PRINTF("\nosd hide up\n");
        evt.id = CAS_EVT_HIDE_OSD_UP;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
	  case CAS_C_HIDE_FORCEOSD:
        OS_PRINTF("cas hide force osd  LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_HIDE_FORCE_OSD;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_HIDE_OSD_DOWN:
        OS_PRINTF("\nosd hide down\n");
        evt.id = CAS_EVT_HIDE_OSD_DOWN;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_HIDE_OSD_TEXT:       
        OS_PRINTF("cas hide super osd text LINE=%d\n", __LINE__);       
        evt.id = CAS_EVT_HIDE_OSD;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_IPP_BUY_OR_NOT:
        addr = cas_get_mem_addr(handle, CAS_IPP_BUY_MEM_ID, CAS_IPP_BUY_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_IPP_BUY_SIZE);                
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                              addr,__LINE__, CAS_IPP_BUY_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_IPP_BUY_INFO_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_IPP_BUY_INFO;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
     case CAS_C_IPP_INFO_UPDATE:
        addr = cas_get_mem_addr(handle, CAS_IPP_BUY_MEM_ID, CAS_IPP_BUY_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_IPP_BUY_SIZE);               
        OS_PRINTF("get mem addr 0x%x, LINE=%d\n", addr,__LINE__);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_IPP_NOTIFY_INFO_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_IPP_BUY_INFO;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_FORCE_CHANNEL:
        addr = cas_get_mem_addr(handle, CAS_FORCE_CHANNEL_MEM_ID, CAS_FORCE_CHANNEL_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_FORCE_CHANNEL_SIZE);           
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                              addr,__LINE__, CAS_FORCE_CHANNEL_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_FORCE_CHANNEL_INFO_GET, &cas_param);
        evt.id = CAS_EVT_FORCE_CHANNEL_INFO;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_UNFORCE_CHANNEL:
        OS_PRINTF("\nunforce channel\n");
        evt.id = CAS_EVT_UNFORCE_CHANNEL_INFO;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_EMM_CHARGE:
        addr = cas_get_mem_addr(handle, CAS_BURSE_CHARGE_MEM_ID, CAS_BURSE_CHARGE_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_BURSE_CHARGE_SIZE);             
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                              addr,__LINE__, CAS_BURSE_CHARGE_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_PPV_CHARGE_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_BURSE_CHARGE;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_IPP_PROG_NOTIFY:
        addr = cas_get_mem_addr(handle, CAS_IPPV_SIGNAL_ID, CAS_IPPV_SIGNAL_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_IPPV_SIGNAL_SIZE);                 
        OS_PRINTF("get mem addr 0x%x, LINE=%d\n", addr,__LINE__);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_IPP_NOTIFY_INFO_GET, &cas_param);
        evt.id = CAS_EVT_IPP_PROG_NOTIFY;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_E_CARD_PARTNER_ERROR_CODE:
        addr = cas_get_mem_addr(handle, CAS_ERROR_PIN_MEM_ID, CAS_ERROR_PIN_CODE_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_ERROR_PIN_CODE_SIZE);                
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                              addr,__LINE__, CAS_ERROR_PIN_CODE_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_PIN_ERR_CNT_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_ERROR_PIN_CODE;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_E_AUTHEN_RECENT_EXPIRE:
        addr = cas_get_mem_addr(handle, CAS_AUTHEN_DAY_MEM_ID, CAS_AUTHEN_DAY_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_AUTHEN_DAY_SIZE);               
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                          addr,__LINE__, CAS_AUTHEN_DAY_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_AUTHEN_DAY_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_AUTHEN_EXPIRE_DAY;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_SON_CARD_NEED_FEED: 
       OS_PRINTF("\nCAS_C_SON_CARD_NEED_FEED line = %d\n",__LINE__);
        evt.id = CAS_EVT_FEED_CARD_DLG;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
     case CAS_C_CARD_NEED_UPDATE:
        OS_PRINTF("cas card need update, LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_CARD_NEED_UPDATE;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_CARD_UPDATE_BEGIN:
        OS_PRINTF("cas card update begin, LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_CARD_UPDATE_BEGIN;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_CARD_UPDATE_PROGRESS:
        OS_PRINTF("cas card update progress, LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_CARD_UPDATE_PROGRESS;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = event.param;
        break;
      case  CAS_C_CARD_UPDATE_ERR:
        OS_PRINTF("cas card update error, LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_CARD_UPDATE_ERR;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = event.param;
        break;
      case CAS_C_CARD_UPDATE_END:
        OS_PRINTF("cas card update end, LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_CARD_UPDATE_END;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_HIDE_IPPV:
        OS_PRINTF("cas hide ippc popup menu, LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_HIDE_PPV_BUY_INFO;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = 0;
        break;
      case CAS_C_IMPORTTANT_MAIL_DISPLAY:
        addr = cas_get_mem_addr(handle, CAS_MAIL_BODY_MEM_ID, CAS_MAIL_BODY_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_MAIL_BODY_SIZE);                 
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                              addr,__LINE__, CAS_MAIL_BODY_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_GET_IMPORTANT_MAIL, &cas_param);
        evt.id = CAS_EVT_FORCE_MSG;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_S_CLEAR_DISPLAY_IMPORTANT_MSG:
        evt.id = CAS_EVT_HIDE_FORCE_MSG;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = cas_event;
        break;
      case CAS_C_IEXIGENT_PROG_STOP:
        OS_PRINTF("CAS_C_IEXIGENT_PROG_STOP, LINE=%d\n", __LINE__);
        evt.id = CAS_EVT_IEXIGENT_PROG_STOP;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = event.param;
        break;
      case CAS_E_ADPT_ILLEGAL_CARD:
      case CAS_E_ADPT_CARD_INIT_FAIL:
      case CAS_E_ADPT_CARD_RST_FAIL:
      case CAS_S_ADPT_CARD_REMOVE:
      case CAS_ERROR_CARD_NOTSUPPORT:
      case CAS_E_CARD_DIS_PARTNER:
      case CAS_E_PROG_UNAUTH:
      case CAS_E_AUTHEN_EXPIRED:
      case CAS_E_SEVICE_SCRAMBLED:
      case CAS_E_DATA_NOT_FIND:
      case CAS_E_DATA_LEN_ERROR :
      case CAS_E_NO_AUTH_STB:
      case CAS_E_PAIRED_OTHERSTB_ERROR:
      case CAS_C_MASTERSLAVE_NEEDPAIRED:
      case CAS_E_ZONE_CODE_ERR:
      case CAS_S_CLEAR_DISPLAY:
        evt.id = CAS_EVT_NOTIFY;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = cas_event;
        if(NULL != p_this->p_policy->init_ca_module)
        {
          p_this->p_policy->init_ca_module(cas_param.cam_id);
        }
        //p_this->change_tp_flag = TRUE;
        break;
      case CAS_E_FILM_RATING:
        addr = cas_get_mem_addr(handle, CAS_FILM_RATING_MEM_ID, CAS_FILM_RATING_GET_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_FILM_RATING_GET_SIZE);              
        OS_PRINTF("get mem addr 0x%x, LINE=%d, size %d\n", 
                            addr,__LINE__, CAS_FILM_RATING_GET_SIZE);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_GET_FILM_RATING, &cas_param);
        evt.id = CAS_EVT_CONFIRM_FILM_RATING;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
     case CAS_C_OTA_INFO:
        OS_PRINTF("cas card update progress, LINE=%d\n", __LINE__);
        addr = cas_get_mem_addr(handle, CAS_OTA_UPDATE_MEM_ID, CAS_OTA_UPDATE_ID_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_OTA_INFO_GET, &cas_param);
        evt.id = CAS_EVT_SOFTWARE_UPDATE_PROGRESS;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        OS_PRINTF("get mem addr 0x%x, LINE=%d\n", addr,__LINE__);
        break;
     case CAS_C_SHOW_ECM_FINGER:
        addr = cas_get_mem_addr(handle, CAS_FINGER_MEM_ID, CAS_FINGER_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_FINGER_SIZE);         
        OS_PRINTF("get mem addr 0x%x, LINE=%d\n", addr,__LINE__);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_ECM_FINGER_MSG_GET, &cas_param);
        evt.id = CAS_EVT_SHOW_ECM_FINGER;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break; 
case CAS_C_STB_URGENT_BROADCAST:
        addr = cas_get_mem_addr(handle, CAS_SHOW_OSD_MEM_ID, CAS_OSD_MSG_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_OSD_MSG_SIZE);            
        OS_PRINTF("get mem addr 0x%x, LINE=%d\n", addr,__LINE__);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_OSD_GET, &cas_param);
        evt.id = CAS_EVT_SENIOR_PREVIEW_SHOW;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
        #if 0
      case CAS_E_YJ_OVERDUE:
        addr = cas_get_mem_addr(handle, CAS_OVERDUE_DATA_ID, 64);
        CHECK_FAIL_RET_VOID(addr != 0);
        OS_PRINTF("get mem addr 0x%x, LINE=%d\n", addr,__LINE__);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_CA_OVERDUE, &cas_param);
        evt.id = CAS_EVT_OVERDUE;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
        #endif
      case CAS_E_CONTINUE_WATCH_LIMIT:
        addr = cas_get_mem_addr(handle, CAS_CA_CONTINUE_WATCH_LIMIT_ID, 
                                CAS_CMD_CONTINUE_WATCH_LIMIT_SIZE);
        CHECK_FAIL_RET_VOID(addr != 0);
        memset((void *)addr, 0x0, CAS_CMD_CONTINUE_WATCH_LIMIT_SIZE);            
        OS_PRINTF("get mem addr 0x%x, LINE=%d\n", addr,__LINE__);
        cas_param.p_param = (void *)addr;
        cas_io_ctrl(event.slot_id, CAS_IOCMD_CONTINUE_WATCH_LIMIT_GET, &cas_param);
        evt.id = CAS_EVT_CONTINUE_WATCH_LIMIT;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = addr;
        break;
      case CAS_C_STB_NOTIFICATION:
        evt.id = CAS_EVT_STB_NOTIFICATION;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = event.param;
        break;
      default:
        evt.id = CAS_EVT_NOTIFY;
        evt.data1 = (event.slot_id << 16) | cas_param.cam_id;
        evt.data2 = cas_event;
        break;
    }
    ap_frm_send_evt_to_ui(APP_CA, &evt);
    
    if(NULL != p_this->p_policy->do_ca_event_by_onself)
    {
       p_this->p_policy->do_ca_event_by_onself(&event);
    }
    
}
#endif
}

static u32 cas_get_msgq_timeout(handle_t handle)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;

  if (p_this->p_policy->get_msgq_timeout != NULL)
    return p_this->p_policy->get_msgq_timeout();
  
  return DEFAULT_MSGQ_TIMEOUT;
}

static void init(handle_t handle)
{
  cas_priv_t *p_this = (cas_priv_t *)handle;
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  u8 i = 0;
  cas_event_notify_t notify = {0};
  u8 *p_addr = NULL;
  u32 size = 0;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  u8 tuner_id = 0, ts_in = 0;

  p_this->p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_CA);
  p_this->pgid = INVALID;
  p_this->cur_tp_freq = INVALID;
  p_this->pmt_version = INVALID;
  p_this->cat_req_flag = FALSE;
  p_this->pmt_req_flag = FALSE;
  p_this->nit_get_flag = FALSE;
  p_this->nit_req_flag = FALSE;
  p_this->ca_sys_id = 0x1FFF;
  p_this->rcv_fifo.p_buffer = p_this->rcv_buf;
  p_this->rcv_fifo.m_size = CAS_FIFO_SIZE;
  p_this->rcv_fifo.m_overlay = TRUE;
  nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);    
  p_this->ts_in = ts_in;
  p_this->tuner_id = tuner_id;
  mtos_fifo_flush(&p_this->rcv_fifo);
  
  p_this->p_buffer = mtos_malloc(MAX_TABLE_NUM * SECTION_LENGTH);
  CHECK_FAIL_RET_VOID(NULL != p_this->p_buffer);

  for(; i < MAX_CAS_SLOT_NUM; i++)
  {
    p_this->svc_id[i] = INVALID_SVC_ID;
  }

  for(i = 0; i < CAS_MEM_ID_MAX; i++)
  {
    p_this->mem_addr[i] = 0;
  }

  CHECK_FAIL_RET_VOID(TRUE == mtos_sem_create(&p_this->evt_mutex, TRUE));

  notify.context = (u32)p_this;
  notify.evt_cb = cas_notify_callback;
  cas_event_notify_register(&notify);

  if (p_this->p_policy->config_ca_data_mem != NULL)
  {
    p_this->p_policy->config_ca_data_mem(&p_addr, &size);
    p_this->mem_start = (u32)p_addr;
    OS_PRINTF("start mem addr 0x%x\n", p_this->mem_start);
    p_this->mem_end = p_this->mem_start + size;
    OS_PRINTF("end mem addr 0x%x\n", p_this->mem_end);
  }
  else
  {
    p_this->mem_start = (u32)mtos_malloc(DATA_SIZE);
    CHECK_FAIL_RET_VOID(p_this->mem_start != 0);
    OS_PRINTF("start mem addr 0x%x\n", p_this->mem_start);
    p_this->mem_end = p_this->mem_start + DATA_SIZE;
    OS_PRINTF("end mem addr 0x%x\n", p_this->mem_end);
  }

  if (p_this->p_policy->on_init != NULL)
  {
    p_this->p_policy->on_init();
  }
}

static void def_on_init(void)
{
}

static void def_test_ca_performance(cas_step_t step, u32 ticks)
{
}

static cas_policy_t *construct_def_policy(void)
{
  cas_policy_t *p_policy = mtos_malloc(sizeof(cas_policy_t));
  CHECK_FAIL_RET_NULL(NULL != p_policy);
  memset(p_policy, 0, sizeof(cas_policy_t));
  p_policy->on_init = def_on_init;
  p_policy->test_ca_performance = def_test_ca_performance;
  p_policy->do_ca_event_by_onself = NULL;
  p_policy->do_ca_command_by_onself = NULL;
  return p_policy;
}

app_t *construct_ap_cas(cas_policy_t *p_policy)
{
  cas_priv_t *p_priv = mtos_malloc(sizeof(cas_priv_t));
  CHECK_FAIL_RET_NULL(NULL != p_priv);
  memset(p_priv, 0, sizeof(cas_priv_t));

  p_priv->p_policy = p_policy;
  if(NULL == p_priv->p_policy)
  {
    p_priv->p_policy = construct_def_policy();
  }
  p_priv->instance.init = init;
  p_priv->instance.process_msg = cas_process_msg;
  p_priv->instance.state_machine = cas_state_machine;
  p_priv->instance.get_msgq_timeout = cas_get_msgq_timeout;
  p_priv->instance.p_data = (void *)p_priv;
  return &p_priv->instance;
}
