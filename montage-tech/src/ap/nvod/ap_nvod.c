/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"

#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "dvb_protocol.h"
#include "mdl.h"
#include "mt_time.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "bat.h"
#include "eit.h"
#include "pti.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "nvod_data.h"
#include "video_packet.h"
#include "ap_framework.h"
#include "ap_nvod.h"

#include "dmx.h"

#ifdef PRINT_ON
#define AP_NVOD_DEBUG
#endif

#ifdef AP_NVOD_DEBUG
#define NVOD_DBG OS_PRINTF
#else
#define NVOD_DBG DUMMY_PRINTF
#endif

/*!
  pti extern buffer size
  */
#define EXTERN_BUFFER_SIZE ((60) * (KBYTES))
/*!
  eit table count
  */
#define EIT_TABLE_CNT (2)
/*!
  eit table count
  */
#define MAX_TP_CNT (16)

/*!
  nvod state definition
  */
typedef enum
{
  /*!
    IDLE state
    */
  NVOD_IDLE = 0,
  /*!
    nvod pre scaning
    */
  NVOD_SCANING_NIT,
  /*!
    nvod scanning reference and shift service
    */
  NVOD_SCANING_SVC,
  /*!
    nvod scanning reference and shift service end
    */
  NVOD_SCANING_SVC_END,
  /*!
    System scanning
    */
  NVOD_SCANING_EVT,
  /*!
    Nvod exit
    */
  NVOD_EXIT,
} nvod_state_t;

/*!
  Scan private data
  */
typedef struct
{
  /*!
    Current scan state
    */
  nvod_state_t state;
  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;
  /*!
    NIM service handle
    */
  service_t *p_nim_svc;
  /*!
    search info
    */
  nc_channel_info_t search_info;
  /*!
    pti_ext_buf_t
    */
  pti_ext_buf_t ext_buf[EIT_TABLE_CNT];
  /*!
    tp list
    */
  nvod_tp_info_t tp_list[MAX_TP_CNT];
  /*!
    nvod tp count
    */
  u8 tp_cnt;
  /*!
    nvod tp scan index
    */
  u8 tp_idx;
  /*!
    shift evt msg flag
    */
  u16 shift_evt_msg_flag : 1;
  /*!
    shift evt msg flag
    */
  u16 req_pmt : 15;
  /*!
    msg counter
    */
  u32 msg_counter;
  /*!
    overdue timer
    */
  u32 overdue_timer;
  /*!
    ts in
    */
  u8 ts_in;
  /*!
    tuner id
    */
  u8 tuner_id;
} nvod_data_t;

/*!
  Scan private information
  */
typedef struct
{
  /*!
    nvod private data
    */
  nvod_data_t *p_priv_data;
  /*!
    nvod data handle
    */
  void *p_nvod_db;
  /*!
    Application Scan information
    */
  app_t nvod_app;
}nvod_priv_t;

static void nvod_process_msg_idle(void *p_handle, os_msg_t *p_msg);
static void nvod_process_msg_runing(void *p_handle, os_msg_t *p_msg);

static void send_msg_to_ui(u32 id)
{
  event_t evt = {0};

  evt.id = id;
  ap_frm_send_evt_to_ui(APP_NVOD, &evt);
}

static void dvbc_lock_tuner(nvod_priv_t *p_priv, u32 freq, u32 sym, u32 mod)
{
  nvod_data_t *p_nvod_data = p_priv->p_priv_data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  
  NVOD_DBG("\n##debug: nvod lock dvbc tuner, freq %d, sym %d, mo %d\n", freq, sym, mod);

  nc_set_blind_scan_mode(nc_handle, p_nvod_data->tuner_id, FALSE);

  p_nvod_data->search_info.channel_info.frequency = freq;
  p_nvod_data->search_info.channel_info.param.dvbc.symbol_rate = sym;
  p_nvod_data->search_info.channel_info.param.dvbc.modulation = mod;
  p_nvod_data->search_info.polarization = NIM_PORLAR_HORIZONTAL;  
  p_nvod_data->search_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
  p_nvod_data->search_info.channel_info.lock = NIM_CHANNEL_UNLOCK;

  nc_set_tp(nc_handle, p_nvod_data->tuner_id, p_nvod_data->p_nim_svc, &p_nvod_data->search_info);
}

static void request_nit_section(nvod_priv_t *p_priv, u32 para2)
{
  service_t *p_dvb_svc = p_priv->p_priv_data->p_dvb_svc;
  dvb_request_t  nit_req = {0};

  nit_req.req_mode = DATA_SINGLE;
  nit_req.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  nit_req.para1 = DVB_TABLE_ID_NIT_ACTUAL;
  nit_req.para2 = para2;
  nit_req.ts_in = p_priv->p_priv_data->ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&nit_req, sizeof(dvb_request_t));
}

static void request_pat_section(nvod_priv_t *p_priv)
{
  service_t *p_dvb_svc = p_priv->p_priv_data->p_dvb_svc;
  dvb_request_t  pat_req = {0};
  
  p_priv->p_priv_data->msg_counter = mtos_ticks_get();
  pat_req.req_mode  = DATA_SINGLE;
  pat_req.table_id  = DVB_TABLE_ID_PAT;
  pat_req.para1     = DVB_TABLE_ID_PAT;
  pat_req.ts_in = p_priv->p_priv_data->ts_in;
  OS_PRINTF("ap_nvod pat req\n");
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&pat_req, sizeof(dvb_request_t));
}

static void request_sdt_section(nvod_priv_t *p_priv, u8 sec_number)
{
  service_t *p_dvb_svc = p_priv->p_priv_data->p_dvb_svc;
  dvb_request_t sdt_req = {0};

  sdt_req.req_mode = DATA_SINGLE;
  sdt_req.table_id = DVB_TABLE_ID_SDT_ACTUAL;
  sdt_req.para1 = DVB_TABLE_ID_SDT_ACTUAL;
  sdt_req.para2 = sec_number;
  sdt_req.ts_in = p_priv->p_priv_data->ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&sdt_req, sizeof(dvb_request_t));
}

static void request_eit_section(nvod_priv_t *p_priv, u32 table_id)
{
  service_t *p_dvb_svc = p_priv->p_priv_data->p_dvb_svc;
  dvb_request_t eit_req = {0};
  u32 ext_buf_addr = 0;

  switch (table_id)
  {
  case DVB_TABLE_ID_EIT_ACTUAL:
    ext_buf_addr = (u32)&(p_priv->p_priv_data->ext_buf[0]);
    break;
  case DVB_TABLE_ID_EIT_SCH_ACTUAL:
    ext_buf_addr = (u32)&(p_priv->p_priv_data->ext_buf[1]);
    break;
  default:
    break;
  }
  
  //Table id is just one flag
  eit_req.table_id  = table_id;
  eit_req.req_mode  = DATA_MULTI;

  eit_req.para1 = table_id;
  eit_req.para2 = ext_buf_addr;
  eit_req.ts_in = p_priv->p_priv_data->ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&eit_req, sizeof(dvb_request_t));
}

static void free_eit_section(nvod_priv_t *p_priv, u32 table_id)
{
  service_t *p_dvb_svc = p_priv->p_priv_data->p_dvb_svc;
  dvb_request_t eit_req = {0};
  u32 ext_buf_addr = 0;
  
  switch (table_id)
  {
  case DVB_TABLE_ID_EIT_ACTUAL:
    ext_buf_addr = (u32)&(p_priv->p_priv_data->ext_buf[0]);
    break;
  case DVB_TABLE_ID_EIT_SCH_ACTUAL:
    ext_buf_addr = (u32)&(p_priv->p_priv_data->ext_buf[1]);
    break;
  default:
    break;
  }
  
  //Table id is just one flag
  eit_req.table_id  = table_id;
  eit_req.req_mode  = DATA_MULTI;
  
  eit_req.para1 = table_id;
  eit_req.para2 = ext_buf_addr;
  eit_req.ts_in = p_priv->p_priv_data->ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_FREE, (u32)&eit_req, sizeof(dvb_request_t));
}

static void nvod_start(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  nvod_tp_info_t *p_tp = NULL;
  //u8 tuner_id = TUNER0;

  nvod_mosaic_buf_reset(class_get_handle_by_id(NVOD_MOSAIC_CLASS_ID));
  memset(&p_priv->p_priv_data->search_info, 0, sizeof(nc_channel_info_t));
  
  p_priv->p_priv_data->tp_cnt = p_msg->para2;
  memcpy(p_priv->p_priv_data->tp_list,
    (nvod_tp_info_t *)p_msg->para1, p_priv->p_priv_data->tp_cnt * sizeof(nvod_tp_info_t));
  
  p_priv->p_priv_data->tp_idx = 0;
  p_tp = &(p_priv->p_priv_data->tp_list[p_priv->p_priv_data->tp_idx]);
  p_priv->nvod_app.process_msg = nvod_process_msg_runing;
  if (p_tp->main_freq)
  {
    p_priv->p_priv_data->state = NVOD_SCANING_NIT;
  }
  else
  {
    p_priv->p_priv_data->state = NVOD_SCANING_SVC;
  }
  dvbc_lock_tuner(p_priv, p_tp->frequency, p_tp->symbol_rate, p_tp->modulation);
}

static void nvod_start_ref_svc(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  dvbc_tp_info_t *p_tp = NULL;

  p_tp = (dvbc_tp_info_t *)p_msg->para1;
  p_priv->nvod_app.process_msg = nvod_process_msg_runing;

  p_priv->p_priv_data->state = NVOD_SCANING_EVT;
  dvbc_lock_tuner(p_priv, p_tp->frequency, p_tp->symbol_rate, p_tp->modulation);
}

static void nvod_stop_ref_svc(nvod_priv_t *p_priv)
{
  p_priv->nvod_app.process_msg = nvod_process_msg_idle;
  if (p_priv->p_priv_data->state == NVOD_SCANING_EVT)
  {
    free_eit_section(p_priv, DVB_TABLE_ID_EIT_ACTUAL);
    free_eit_section(p_priv, DVB_TABLE_ID_EIT_SCH_ACTUAL);
  }
  p_priv->p_priv_data->state = NVOD_IDLE;
}

static void nvod_stop(nvod_priv_t *p_priv)
{
  p_priv->nvod_app.process_msg = nvod_process_msg_idle;
  if (p_priv->p_priv_data->state == NVOD_SCANING_EVT)
  {
    free_eit_section(p_priv, DVB_TABLE_ID_EIT_ACTUAL);
    free_eit_section(p_priv, DVB_TABLE_ID_EIT_SCH_ACTUAL);
  }
  p_priv->p_priv_data->state = NVOD_IDLE;
  send_msg_to_ui(NVOD_STOP);
}

static void nvod_on_nc_locked(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  nvod_data_t *p_nvod_data = p_priv->p_priv_data;
  nc_channel_info_t *p_tp_info = &p_nvod_data->search_info;

  memcpy(p_tp_info, (void *)p_msg->para1, sizeof(nc_channel_info_t));
  NVOD_DBG("\n##debug: nvod_on_nc_locked[%ld][%d][%d]\n",
    p_tp_info->channel_info.frequency,
    p_tp_info->channel_info.param.dvbc.symbol_rate,
    p_tp_info->channel_info.param.dvbc.modulation);
  
  p_nvod_data->search_info.channel_info.frequency = p_tp_info->channel_info.frequency;
  p_nvod_data->search_info.channel_info.param.dvbc.symbol_rate
    = p_tp_info->channel_info.param.dvbc.symbol_rate;
  p_nvod_data->search_info.channel_info.param.dvbc.modulation
    = p_tp_info->channel_info.param.dvbc.modulation;

  nvod_data_set_locked_tp_info(p_priv->p_nvod_db,
    p_tp_info->channel_info.frequency,
    p_tp_info->channel_info.param.dvbc.symbol_rate,
    p_tp_info->channel_info.param.dvbc.modulation);

  switch (p_priv->p_priv_data->state)
  {
  case NVOD_SCANING_NIT:
    request_nit_section(p_priv, 0xFFFF);
    break;
  case NVOD_SCANING_SVC:
    request_sdt_section(p_priv, 0);
    break;
    
  case NVOD_SCANING_EVT:
    request_eit_section(p_priv, DVB_TABLE_ID_EIT_ACTUAL);
    request_eit_section(p_priv, DVB_TABLE_ID_EIT_SCH_ACTUAL);
    break;
    
  default:
    break;
  }

}

static void nvod_on_nc_unlocked(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  if (p_priv->p_priv_data->state == NVOD_SCANING_NIT
    || p_priv->p_priv_data->state == NVOD_SCANING_SVC)
  {
    p_priv->p_priv_data->state = NVOD_SCANING_SVC_END;
  }
  //send_msg_to_ui(NVOD_NC_UNLOCKED);
  memset(&p_priv->p_priv_data->search_info, 0, sizeof(nc_channel_info_t));
}

static void insert_tp(nvod_priv_t *p_priv, nvod_tp_info_t *p_tp_in)
{
  nvod_tp_info_t *p_tp = NULL;
  u8 loopi = 0;
  
  for (loopi = 0; loopi < p_priv->p_priv_data->tp_cnt; loopi ++)
  {
    p_tp = &p_priv->p_priv_data->tp_list[loopi];
    if (p_tp->frequency == p_tp_in->frequency
      && p_tp->symbol_rate == p_tp_in->symbol_rate
      && p_tp->modulation == p_tp_in->modulation)
    {
      return ;
    }
  }
  
  if (loopi >= MAX_TP_CNT)
  {
    return ;
  }
  
  p_tp = &p_priv->p_priv_data->tp_list[loopi];
  p_tp->frequency = p_tp_in->frequency;
  p_tp->symbol_rate = p_tp_in->symbol_rate;
  p_tp->modulation = p_tp_in->modulation;
  p_tp->main_freq = FALSE;
  p_priv->p_priv_data->tp_cnt ++;
}

static void nvod_on_nit_found(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  nit_t *p_nit = (nit_t *)p_msg->para1;
  cable_tp_info_t *p_nit_tp = NULL;
  nvod_tp_info_t tp = {0};
  u32 para2 = 0;
  u8 loopi = 0;

  if (p_nit->sec_number < p_nit->last_sec_number)
  {
    para2 = ((p_nit->sec_number + 1) << 16) | 0xFFFF;
    request_nit_section(p_priv, para2);
  }
  else
  {
    request_sdt_section(p_priv, 0);
    p_priv->p_priv_data->state = NVOD_SCANING_SVC;
  }
  
  for (loopi = 0; loopi < p_nit->total_tp_num; loopi ++)
  {
    if (p_nit->tp_svc_list[loopi].tp_type == NIT_DVBC_TP)
    {
      p_nit_tp = &p_nit->tp_svc_list[loopi].dvbc_tp_info;

      if ((bcd_number_to_dec(p_nit_tp->frequency) / 10) <= 256 ||
        (bcd_number_to_dec(p_nit_tp->symbol_rate) / 10) <= 20)
      {
          //Invalid tp information
          continue;
      }
      
      tp.frequency = bcd_number_to_dec(p_nit_tp->frequency) / 10;
      tp.symbol_rate = bcd_number_to_dec(p_nit_tp->symbol_rate) / 10;
      switch (p_nit_tp->modulation)
      {
      case 0x01:
        tp.modulation = NIM_MODULA_QAM16;
        break;
      case 0x02:
        tp.modulation = NIM_MODULA_QAM32;
        break;
      case 0x03:
        tp.modulation = NIM_MODULA_QAM64;
        break;
      case 0x04:
        tp.modulation = NIM_MODULA_QAM128;
        break;
      case 0x05:
        tp.modulation = NIM_MODULA_QAM256;
        break;
      default:
        tp.modulation = p_nit_tp->modulation;
        break;
      }

      NVOD_DBG("\n##debug: nvod_on_nit_found [%d], [%d][%d][%d]\n",
        loopi + 1, tp.frequency, tp.symbol_rate, tp.modulation);
      insert_tp(p_priv, &tp);
    }
  }
}

static void nvod_on_pat_found(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  //p_priv->p_priv_data->state = NVOD_SCANING_SVC_END;
}

static void nvod_on_pmt_info_found(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  service_t *p_dvb_svc = p_priv->p_priv_data->p_dvb_svc;
  u32 param = ((p_msg->para1 << 16) | p_msg->para2);
  dvb_request_t req_param = {0};

  p_priv->p_priv_data->req_pmt ++;
  req_param.req_mode = DATA_SINGLE;
  req_param.table_id = DVB_TABLE_ID_PMT;
  req_param.para1 = DVB_TABLE_ID_PMT;
  req_param.para2 = param;
  req_param.ts_in = p_priv->p_priv_data->ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&req_param, sizeof(dvb_request_t));
}

static void nvod_on_pmt_found(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  pmt_t *p_pmt = (pmt_t *)p_msg->para1;
  nvod_db_status_t db_status = 0;

  p_priv->p_priv_data->req_pmt --;
  if (p_priv->p_priv_data->req_pmt == 0)
  {
    p_priv->p_priv_data->state = NVOD_SCANING_SVC_END;
  }
  
  db_status = nvod_data_set_shift_svc(p_priv->p_nvod_db, p_pmt);

  if (db_status == NVOD_DB_SHIFT_SVC_FOUND)
  {

  }
}

static void nvod_on_sdt_found(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  sdt_t *p_sdt = (sdt_t *)p_msg->para1;
  nvod_db_status_t db_status = NVOD_DB_NORM;
  u8 sec_num = 0;
  
  if (p_sdt->sec_number == 0)
  {
    p_priv->p_priv_data->req_pmt = 0;
    request_pat_section(p_priv);
    sec_num = 0;
    while (sec_num < p_sdt->last_sec_number)
    {
      request_sdt_section(p_priv, ++ sec_num);
    }
  }
  
  db_status = nvod_data_add_svc(p_priv->p_nvod_db, p_sdt);
  if (db_status == NVOD_DB_REF_SVC_FOUND)
  {
    send_msg_to_ui(NVOD_REF_SVC_FOUND);
  }
}

static void nvod_on_eit_found(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  eit_t *p_eit = (eit_t *)p_msg->para1;
  nvod_db_status_t db_status = NVOD_DB_NORM;
  
  db_status = nvod_data_add_evt(p_priv->p_nvod_db, p_eit);
    
  switch (db_status)
  {
  case NVOD_DB_REF_EVT_UPDATE:
    send_msg_to_ui(NVOD_REF_EVT_FOUND);
    break;
  case NVOD_DB_SHIFT_EVT_UPDATE:
    p_priv->p_priv_data->shift_evt_msg_flag = 1;
    break;
  default:
    break;
  }
}

static void nvod_on_timedout(nvod_priv_t *p_priv, os_msg_t *p_msg)
{
  NVOD_DBG("\n##debug:time out table [0x%x], module [%d]\n",
    p_msg->para1, p_msg->para2);

  switch(p_msg->para1)
  {
  case DVB_TABLE_ID_PMT:
    break;   
  case DVB_TABLE_ID_PAT:
    if (NVOD_SCANING_EVT != p_priv->p_priv_data->state)
    {
      p_priv->p_priv_data->state = NVOD_SCANING_SVC_END;
    }
    break;
  case DVB_TABLE_ID_NIT_ACTUAL:
    request_sdt_section(p_priv, 0);
    break;
  case DVB_TABLE_ID_SDT_ACTUAL:
    request_sdt_section(p_priv, 0);
    break;
  case DVB_TABLE_ID_BAT:
    break;
  case MPEG_TS_PACKET:
    break;
  case DVB_TABLE_ID_EIT_ACTUAL:
  case DVB_TABLE_ID_EIT_SCH_ACTUAL:
    break;
  default:
    break;
  }

}

static void nvod_process_msg_idle(void *p_handle, os_msg_t *p_msg)
{
  nvod_priv_t *p_priv = (nvod_priv_t *)p_handle;
//  nvod_data_t *p_nvod_data = p_priv->p_priv_data;
  u8 tuner_id = TUNER0;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  switch(p_msg->content)
  {
  case CMD_START_NVOD:
    p_priv->p_priv_data->tuner_id = tuner_id;  //we need the id info from msg, but no module call it
    nc_get_ts_by_tuner(nc_handle, p_priv->p_priv_data->tuner_id, &p_priv->p_priv_data->ts_in);
    nvod_start(p_priv, p_msg);
    break;

  case CMD_NVOD_START_REF_SVC:
    p_priv->p_priv_data->tuner_id = tuner_id;  //we need the id info from msg, but no module call it
    nc_get_ts_by_tuner(nc_handle, p_priv->p_priv_data->tuner_id, &p_priv->p_priv_data->ts_in);
    nvod_start_ref_svc(p_priv, p_msg);
    break;

  case CMD_STOP_NVOD:
    nvod_stop(p_priv);
    break;

  default:
    break;
  }
}

static void nvod_process_msg_runing(void *p_handle, os_msg_t *p_msg)
{
  nvod_priv_t *p_priv = (nvod_priv_t *)p_handle;
  //nvod_data_t *p_nvod_data = p_priv->p_priv_data;
  //u8 tuner_id = TUNER0;


  switch(p_msg->content)
  {
  case NC_EVT_LOCKED:
    p_priv->p_priv_data->tuner_id = p_msg->context;
    NVOD_DBG(("nvod: NC_EVT_LOCKED!\n"));
    nvod_on_nc_locked(p_priv, p_msg);
    break;

  case NC_EVT_UNLOCKED:
    p_priv->p_priv_data->tuner_id = p_msg->context;
    NVOD_DBG(("nvod: NC_EVT_UNLOCKED!\n"));
    nvod_on_nc_unlocked(p_priv, p_msg);
    break;
      
  case DVB_BAT_FOUND:
    NVOD_DBG(("nvod: DVB_BAT_FOUND\n"));
    break;
      
  case DVB_NIT_FOUND:
    NVOD_DBG(("nvod: DVB_NIT_FOUND\n"));
    nvod_on_nit_found(p_priv, p_msg);
    break;
      
  case DVB_PAT_FOUND:
    //NVOD_DBG(("nvod: DVB_PAT_FOUND\n"));
    nvod_on_pat_found(p_priv, p_msg);
    break;
      
  case DVB_PAT_PMT_INFO:
    //NVOD_DBG(("nvod: DVB_PAT_PMT_INFO\n"));
    nvod_on_pmt_info_found(p_priv, p_msg);
    break;
      
  case DVB_PMT_FOUND:
    //NVOD_DBG(("nvod: DVB_PMT_FOUND\n"));
    nvod_on_pmt_found(p_priv, p_msg);
    break;
      
  case DVB_SDT_FOUND:
    NVOD_DBG(("nvod: DVB_SDT_FOUND\n"));
    nvod_on_sdt_found(p_priv, p_msg);
    break;
  case DVB_ONE_SEC_FOUND:
    nvod_on_eit_found(p_priv, p_msg);
    break;
  case DVB_TABLE_TIMED_OUT:
    //NVOD_DBG("nvod: DVB_TABLE_TIMED_OUT [0x%x]!\n", p_msg->para1);
    nvod_on_timedout(p_priv, p_msg);
    break;
  case CMD_NVOD_STOP_REF_SVC:
    nvod_stop_ref_svc(p_priv);
    break;
      
  case CMD_STOP_NVOD:
    nvod_stop(p_priv);
    break;
  default:
    break;
  }

}

static void nvod_state_machine(void *p_handle)
{
  nvod_priv_t *p_priv = (nvod_priv_t *)p_handle;
  nvod_data_t *p_nvod_data = p_priv->p_priv_data;
  nvod_tp_info_t *p_tp = NULL;
  //u8 tuner_id = TUNER0;

  switch(p_nvod_data->state)
  {
  case NVOD_IDLE:
    break;
  case NVOD_SCANING_SVC_END:
    {
      if (p_priv->p_priv_data->tp_idx + 1 < p_priv->p_priv_data->tp_cnt)
      {
        p_priv->p_priv_data->tp_idx ++;
        p_tp = &(p_priv->p_priv_data->tp_list[p_priv->p_priv_data->tp_idx]);
        dvbc_lock_tuner(p_priv, p_tp->frequency, p_tp->symbol_rate, p_tp->modulation);
        if (p_tp->main_freq)
        {
          p_priv->p_priv_data->state = NVOD_SCANING_NIT;
        }
        else
        {
          p_priv->p_priv_data->state = NVOD_SCANING_SVC;
        }
      }
      else
      {
        send_msg_to_ui(NVOD_SVC_SCAN_END);
        p_priv->nvod_app.process_msg = nvod_process_msg_idle;
        p_priv->p_priv_data->state = NVOD_IDLE;
      }
    }
    break;
      
  case NVOD_SCANING_EVT:
    if ((mtos_ticks_get() - p_priv->p_priv_data->msg_counter) >= 100)
    {  
      p_priv->p_priv_data->msg_counter = mtos_ticks_get();
      if (p_priv->p_priv_data->shift_evt_msg_flag)
      {
        p_priv->p_priv_data->shift_evt_msg_flag = 0;
        send_msg_to_ui(NVOD_SHIFT_EVT_FOUND);
      }
    }

    if ((mtos_ticks_get() - p_priv->p_priv_data->overdue_timer) >= 1000)
    {
      p_priv->p_priv_data->overdue_timer = mtos_ticks_get();
      nvod_data_delete_overdue(p_priv->p_nvod_db);
    }
    break;
  case NVOD_EXIT:
    NVOD_DBG("nvod state: NVOD_EXIT\n");
    break;
  default:
    break;
  }
}

static void init(handle_t p_handle)
{
  nvod_priv_t *p_priv = (nvod_priv_t *)p_handle;
  u32 mem_addr = 0;
  u8 loopi = 0;

  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  nim_ctrl_t *p_nc = class_get_handle_by_id(NC_CLASS_ID);
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  u8 tuner_id = 0, ts_in = 0;
  
  p_priv->p_priv_data->p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_NVOD);
  p_priv->p_priv_data->p_nim_svc = nc_get_svc_instance(p_nc, APP_NVOD);
  p_priv->p_nvod_db = class_get_handle_by_id(NVOD_CLASS_ID);
  
  mem_addr = (u32)mtos_malloc(EIT_TABLE_CNT * EXTERN_BUFFER_SIZE);
  for (loopi = 0; loopi < EIT_TABLE_CNT; loopi ++)
  {
    p_priv->p_priv_data->ext_buf[loopi].p_buf = (u8 *)mem_addr;
    p_priv->p_priv_data->ext_buf[loopi].p_next = NULL;
    p_priv->p_priv_data->ext_buf[loopi].size = EXTERN_BUFFER_SIZE;
    mem_addr += EXTERN_BUFFER_SIZE;
  }

  p_priv->p_priv_data->overdue_timer = mtos_ticks_get();
  p_priv->p_priv_data->msg_counter = mtos_ticks_get();
  nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);    
  p_priv->p_priv_data->tuner_id = tuner_id;
  p_priv->p_priv_data->ts_in = ts_in;
}

app_t *construct_ap_nvod(void)
{
  nvod_priv_t *p_priv = mtos_malloc(sizeof(nvod_priv_t));
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  u8 tuner_id = 0, ts_in = 0;
  

  //Malloc private data for scan
  p_priv->p_priv_data = mtos_malloc(sizeof(nvod_data_t));
  memset(p_priv->p_priv_data, 0, sizeof(nvod_data_t));
  memset(&(p_priv->nvod_app), 0, sizeof(app_t));
  
  // Attach scan applition

  // Since we don't know derived scan's pre scan behavior, we can't filter
  // any message. Like NIT table we don't use it in abstract scan, but NIT scan
  // class will use it.
  // The message list of scan should not be NULL 
  p_priv->nvod_app.init = init;
  p_priv->nvod_app.process_msg = nvod_process_msg_idle;  //init to idle
  p_priv->nvod_app.state_machine = nvod_state_machine;
  p_priv->nvod_app.get_msgq_timeout = NULL;
  p_priv->nvod_app.p_data = (void *)p_priv;
  nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);      
  p_priv->p_priv_data->ts_in = ts_in;
  p_priv->p_priv_data->tuner_id = tuner_id;

  return &p_priv->nvod_app;
}

/*!
  end of file
  */

