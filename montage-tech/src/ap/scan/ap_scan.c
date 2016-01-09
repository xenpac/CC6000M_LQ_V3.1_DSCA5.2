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

#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "dvb_protocol.h"
#include "mdl.h"
#include "err_check_def.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "nit.h"
#include "pat.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"
#include "bat.h"
#include "video_packet.h"
#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_scan_priv.h"
#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_scan_imp.h"
#include "dmx.h"
#ifdef PRINT_ON
#define AP_SCAN_DEBUG
#endif

#ifdef AP_SCAN_DEBUG
#define SCAN_DBG OS_PRINTF
#else
#define SCAN_DBG DUMMY_PRINTF
#endif
/*!
  Max pid number supported, pid is a 13-bit number
  */
#define MAX_PID_NUM (8 * 1024)

/*!
  Max service number possible existing
  */
#define MAX_SID_NUM (65536)

/*!
  Max service number possible existing
  */
#define MAX_BOU_ID_NUM (64)

/*!
  Max buffer size for bat multi request
  */
#define BAT_REQUEST_BUFFER (8 * 1024)

/*!
  Scan state definition
  */
typedef enum
{
  /*!
    IDLE state
    */
  SCAN_IDLE = 0,
  /*!
    Get TP state
    */
  SCAN_GET_TP,
  /*!
    Lock tp
    */
  SCAN_LOCK_TP,
  /*!
    Wait TP lock/unlock
    */
  SCAN_WAIT_TP_LOCK,
  /*!
    System scanning
    */
  SCAN_SCANING,
  /*!
    Scan finished
    */
  SCAN_FINISHED,
  SCAN_PAT_REQ
} state_t;
/*!
  Nit mode definition
  */
typedef enum
{
  /*!
    Scan without request nit
    */
  NIT_SCAN_WITHOUT = 0,
  /*!
    request nit on all tp
    */
  NIT_SCAN_ALL_TP,
  /*!
    request nit once
    */
  NIT_SCAN_ONCE
} nit_scan_type_t;

/*!
  Job information of requested table
  */
typedef struct
{
  /*!
    PAT number
    */
  u32 pat_num;
  /*!
    SDT number
    */
  u32 sdt_num;
  /*!
    Video packet number
    */
  u32 video_packet_num;
  /*!
    PMT number
    */
  u32 pmt_num;
  /*!
    NIT number
    */
  u32 nit_num;
  /*!
    BAT number
    */
  u32 bat_num;
  /*!
    CAT number
    */
  u32 cat_num;
  /*!
    current tp spend ticks
    */
  u32 spend_ticks;
} job_dbg_info_t;

/*!
  TS packet structure
  */
typedef struct
{
  /*! 
    Record the pgs' sid, which request ts packet by the same pid
    */
  u16 pg_arry[DVB_MAX_PAT_PROG_NUM];
  /*!  
    Number of the program in pg array
    */
  u8 pg_num;
  /*! 
    0: free, 1: scramble
    */
  u8 is_scramble;
  /*! 
    1: found, 0: no found
    */
  u8 is_found;
  /*!
    Unknown field
    */
  u8 reserved;
}ts_packet_state_t;


/*!
  Ts packet manager structure
  */
typedef struct
{
  /*!
    PID scramble array
    */
  BOOL pid_scramble_state[MAX_PID_NUM];
  /*! 
    PID state array
    */
  ts_packet_state_t states[DVB_MAX_PAT_PROG_NUM];
  /*!
    Record the state index, pid is 13bit
    */
  u8 pid_offset[MAX_PID_NUM];
  /*!
    The max pid offset should be less than DVB_MAX_PAT_PROG_NUM
    */
  u8 cur_pid_offset;
}ts_data_t;

/*!
  BAT section and bouquet info
  */
typedef struct
{
  /*!
    store BAT bouquet id
    */
  u16 bouquet_id;
  /*!
    store first recieved BAT section num of specify bouquet
    */
  u8 sec_num_array[16];
  /*!
    store repeat times of first recieved specify bouquet's section
    */
  u8 sec_repeat_array[16];
  /*!
    store repeat times of first recieved specify bouquet's section
    */
  u8 last_sec_num;
  /*!
    store repeat times of first recieved specify bouquet's section
    */
  BOOL b_sec_finish;
}bat_sec_info_t;

/*!
  Scan running data
  */
typedef struct
{
  /*!
    Pending job in scan
    */
  s32 pending_job;
  /*!
    bat_multi_mode
    */
  BOOL bat_multi_mode;
  /*!
    bat_multi_mode start tickets
    */
  u32 start_bat_tickets;
  /*!
    pg list
    */
  scan_pg_info_t pg_list[SCAN_MAX_PG_PER_TP];
  /*!
    found tp info
    */
  nc_channel_info_t find_tp;
  /*!
    Result info
    */
  scan_tp_info_t result;
  /*!
    DVB job record for debug
    */
  job_dbg_info_t job_dbg_info;
  /*!
    TS manager    
    */
  ts_data_t ts_mgr;
  /*!
    store pg index in pg array according to SID
    */
  u8 pg_offset[MAX_SID_NUM];
  /*!
    store BAT section and bouquet info
    */
  bat_sec_info_t bat_sec_info[MAX_BOU_ID_NUM];
  /*!
    Current pg offset value
    */
  u8 cur_pg_offset; 
  /*!
    blind scan version 
    */
  blindscan_ver_t bs_ver;
  /*!
    bat bouqust id num
    */
  u8 bouquet_num;
  /*!
    reserved2
    */
  u16 reserved2;
  /*!
    list for bat bouqust id
    */
  u16 bouquet_id[MAX_BOU_ID_NUM];
}running_data_t;

/*!
  Scan private data
  */
typedef struct
{
  /*!
    Current scan state
    */
  state_t state;
  /*!
    nit scan type
    */
  nit_scan_type_t nit_type;
  /*!
    Scan running data
    */
  running_data_t *p_rt_data;
  /*!
    Whether this ap is initialized or not
    */
  BOOL is_init;
  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;
  /*!
    NC service handle
    */
  service_t *p_nc_svc;
   /*!
    tuner id
    */ 
   u8 tuner_id;
   /*!
    ts in
    */ 
   u8 ts_in; 
} scan_data_t;

/*!
  Scan private information
  */
typedef struct 
{
  /*!
    Scan private data
    */
  scan_data_t *p_priv_data;
  /*!
    Current scan ploicy such as Blind scan or TP scan
    */
  scan_policy_t *p_policy;
  /*!
    Application Scan information
    */
  app_t scan_app;
}scan_priv_t;

static void scan_process_msg_idle(void *p_handle, os_msg_t *p_msg);
static void scan_process_msg_runing(void *p_handle, os_msg_t *p_msg);

#if 0 //for specified bouquet id
#else
static BOOL is_need_free_bat(scan_priv_t *p_scan_priv, bat_t *p_bat)
{
  u16 i = 0;
  BOOL ret = FALSE;

  if(p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].sec_repeat_array[p_bat->sec_number] >= 3)
  {
    for(i = 0; i < MAX_BOU_ID_NUM; i++)
    {
      if(p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].bouquet_id != 0
        && !p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].b_sec_finish)
      {
        OS_PRINTF("is_need_free_bat break\n");
        break;
      }
    }

    if(i == MAX_BOU_ID_NUM)
    {
      ret = TRUE;
      OS_PRINTF("is_need_free_bat FREE\n");
    }
  }
  
  return ret;
}

static void add_bouquet_id(scan_priv_t *p_scan_priv, bat_t *p_bat)
{
  u8 i = 0, j = 0;
  BOOL b = TRUE;

  for(i = 0; i < MAX_BOU_ID_NUM; i++)
  {
    if(p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].bouquet_id == p_bat->bouquet_id)
    {
      p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].sec_repeat_array[p_bat->sec_number]++;

      for(j = 0; j < p_bat->last_sec_number; j++)
      {
        if(p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].sec_repeat_array[j] == 0)
        {
          b = FALSE;
          break;
        }
      }

      if(b)
      {
        p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].b_sec_finish = TRUE;
      }

      break;
    }
    else
    {
      if(p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].bouquet_id == 0)
      {
        p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].bouquet_id = p_bat->bouquet_id;
        p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].last_sec_num = p_bat->last_sec_number;

        if(p_bat->last_sec_number == 0)
        {
          p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].b_sec_finish = TRUE;
        }

        for(j = 0; j < p_bat->last_sec_number; j++)
        {
          p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].sec_num_array[j] = j;
        }

        p_scan_priv->p_priv_data->p_rt_data->bat_sec_info[i].sec_repeat_array[p_bat->sec_number]++;

        break;
      }
    }
  }
  return;
}
#endif

static void add_pg_info(scan_pg_info_t *p_pg, pmt_t *p_pmt)
{
  u8 loopi = 0;

  //Add program information
  p_pg->s_id = p_pmt->prog_num;
  p_pg->audio_ch_num = (p_pmt->audio_count > SCAN_MAX_AUDIO_CHANNEL)?
    SCAN_MAX_AUDIO_CHANNEL : p_pmt->audio_count;
  p_pg->is_scrambled = 0;
  p_pg->ca_system_id = 0;
  p_pg->pcr_pid   = p_pmt->pcr_pid;
  p_pg->video_pid = p_pmt->video_pid;
  p_pg->video_type = p_pmt->video_type;
  p_pg->pmt_pid = p_pmt->pmt_pid;
  p_pg->ecm_num = p_pmt->ecm_cnt;
  memcpy(p_pg->ca_ecm, p_pmt->cas_descr, p_pg->ecm_num * sizeof(cas_desc_t));
  
  for(loopi = 0; loopi < p_pg->audio_ch_num; loopi++)
  {
    p_pg->audio[loopi] = p_pmt->audio[loopi];
  }
  
  if (p_pmt->mosaic_des_found
    || p_pmt->stream_identifier_des_found
    || p_pmt->logic_screen_des_found)
  {
    p_pg->p_mosaic = p_pmt->p_mosaic_des;
   
    p_pg->mosaic_flag = p_pg->p_mosaic->lg_cell_cnt;
    p_pmt->mosaic_des_found = 0;
    p_pmt->stream_identifier_des_found = 0;
    p_pmt->logic_screen_des_found = 0;
  }
}

static scan_pg_info_t *scan_find_pg(scan_priv_t *p_scan_priv, u32 s_id)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  scan_pg_info_t *p_pg = NULL;
  u8 cur_pg_offset = 0;
  
  if(p_scan_data->p_rt_data->pg_offset[s_id] == 0xFF)
  {
    cur_pg_offset = p_scan_data->p_rt_data->cur_pg_offset;
    p_scan_data->p_rt_data->pg_offset[s_id] = cur_pg_offset;
    p_pg = &p_scan_data->p_rt_data->pg_list[cur_pg_offset];
    p_scan_data->p_rt_data->cur_pg_offset++;
    CHECK_FAIL_RET_NULL(p_scan_data->p_rt_data->cur_pg_offset <= SCAN_MAX_PG_PER_TP);
  }
  else
  {
    cur_pg_offset = p_scan_data->p_rt_data->pg_offset[s_id];
    p_pg = &p_scan_data->p_rt_data->pg_list[cur_pg_offset];
  }
  
  return p_pg;
}


/*!
  Add sid state into ts manager array
  \param[in] p_ts_mgr the starting address of ts manager
  \param[in] service id in pmt and equals to program number
  \param[in] pid with the sid 
  */
static void add_sid_state_into_ts_mgr(ts_data_t *p_ts_mgr, u16 sid, u16 pid)
{
  u8 cur_pid_offset = 0;
  ts_packet_state_t *p_ts_state = NULL;

  if(0xFF == p_ts_mgr->pid_offset[pid])
  {
    //PID state with this PID does't existing
    cur_pid_offset = p_ts_mgr->cur_pid_offset;
    p_ts_mgr->pid_offset[pid] = cur_pid_offset;
    p_ts_mgr->cur_pid_offset++;  
    p_ts_state = &p_ts_mgr->states[cur_pid_offset];
  }
  else
  {
    //PID state already existing and add sid information only
    cur_pid_offset = p_ts_mgr->pid_offset[pid];
    p_ts_state     = &p_ts_mgr->states[cur_pid_offset];
  }
  
  //Add sid state into pg state array in pid state array
  p_ts_state->pg_arry[p_ts_state->pg_num] = sid;
  p_ts_state->pg_num ++;
}


static void set_svc_scramble_state(scan_priv_t *p_data, u16 sid, 
BOOL is_scramble)
{
  scan_pg_info_t *p_pg = NULL;
  p_pg = scan_find_pg(p_data, sid);
  CHECK_FAIL_RET_VOID(p_pg != NULL);

  //Set scramble flag
  p_pg->is_scramble_found = TRUE;
  p_pg->is_scrambled = is_scramble;
}

static void ts_packet_request(scan_priv_t *p_scan_priv, u16 sid, u16 pid)
{
  u32 param = (pid | (sid << 16));
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  dvb_request_t pck_req = {0};

  pck_req.para1     = MPEG_TS_PACKET;
  pck_req.para2     = param;
  pck_req.table_id  = MPEG_TS_PACKET;
  pck_req.period    = 0;
  pck_req.req_mode  = DATA_SINGLE;
  pck_req.ts_in = p_scan_data->ts_in;
  p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
    (u32)&pck_req, sizeof(dvb_request_t));
  
  p_scan_data->p_rt_data->job_dbg_info.video_packet_num++;
  p_scan_data->p_rt_data->pending_job++;  
}

#if 0 //for specified bouquet id
void scan_request_bat(scan_priv_t *p_scan_priv)
{
  dvb_request_t param = {0};
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  u8 i = 0;

  for(i = 0; i < p_scan_data->p_rt_data->bouquet_num; i ++)
  {
    param.req_mode  = DATA_SINGLE;
    param.table_id  = DVB_TABLE_ID_BAT;
    param.para1     = DVB_TABLE_ID_BAT;
    param.para2     = (u32)(p_scan_data->p_rt_data->bouquet_id[i] & 0xffff);
   // param.para2     = 0;
    param.ts_in = p_scan_data->ts_in;
    p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
    (u32)&param, sizeof(dvb_request_t));
    p_scan_data->p_rt_data->job_dbg_info.bat_num++;
    p_scan_data->p_rt_data->pending_job++;
  }
  

  p_scan_data->p_rt_data->start_bat_tickets = mtos_ticks_get();

}
#else
void scan_request_bat(scan_priv_t *p_scan_priv)
{
  dvb_request_t param = {0};
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  scan_policy_t *p_policy = p_scan_priv->p_policy;
  filter_ext_buf_t *p_ext_buf_list = NULL;
  
//  p_ext_buf_list = mtos_malloc(sizeof(filter_ext_buf_t));
  p_ext_buf_list = 
    p_policy->require_runingtime_buffer(p_policy->p_priv_data, sizeof(filter_ext_buf_t));
  p_ext_buf_list->p_buf = 
    p_policy->require_runingtime_buffer(p_policy->p_priv_data, BAT_REQUEST_BUFFER);
  p_ext_buf_list->size = BAT_REQUEST_BUFFER;
  
  param.req_mode  = DATA_MULTI;
  param.table_id  = DVB_TABLE_ID_BAT;
  param.para1     = DVB_TABLE_ID_BAT;
  param.para2     = (u32)p_ext_buf_list;
  param.ts_in = p_scan_data->ts_in;
 // param.para2     = 0;
  
  p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
  (u32)&param, sizeof(dvb_request_t));
  p_scan_data->p_rt_data->job_dbg_info.bat_num++;
  p_scan_data->p_rt_data->pending_job++;

  p_scan_data->p_rt_data->bat_multi_mode = TRUE;
  p_scan_data->p_rt_data->start_bat_tickets = mtos_ticks_get();

}

void scan_free_bat(scan_priv_t *p_scan_priv)
{
  dvb_request_t param = {0};
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  
  param.req_mode  = DATA_MULTI;
  param.table_id  = DVB_TABLE_ID_BAT;
  param.para1     = DVB_TABLE_ID_BAT;
  param.para2     = 0;
  p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_FREE,
  (u32)&param, sizeof(dvb_request_t));

  p_scan_data->p_rt_data->job_dbg_info.bat_num--;
  p_scan_data->p_rt_data->pending_job--;
  p_scan_data->p_rt_data->bat_multi_mode = FALSE;
  p_scan_data->p_rt_data->start_bat_tickets = 0;
}
#endif

static void init_para(scan_data_t *p_priv_data)
{
  // Reset pg offset information
  memset(&p_priv_data->p_rt_data->ts_mgr, 0, sizeof(ts_data_t));
  memset(p_priv_data->p_rt_data->ts_mgr.pid_offset, 0xFF, MAX_PID_NUM);
  memset(p_priv_data->p_rt_data->pg_offset, 0xFF, MAX_SID_NUM);
  memset(&p_priv_data->p_rt_data->job_dbg_info, 0, sizeof(job_dbg_info_t));
  memset(p_priv_data->p_rt_data->pg_list, 0, 
    sizeof(scan_pg_info_t)*SCAN_MAX_PG_PER_TP);
  memset(&p_priv_data->p_rt_data->result, 0, sizeof(scan_tp_info_t));
  p_priv_data->p_rt_data->result.p_pg_list = p_priv_data->p_rt_data->pg_list;
  p_priv_data->p_rt_data->pending_job = 0;
  p_priv_data->p_rt_data->cur_pg_offset = 0;
}

/*static void default_performance(void *p_data, perf_check_point_t check, u32 context1, u32 context2)
{

}*/
static void scan_start(scan_priv_t *p_priv, os_msg_t *p_msg)
{
  scan_policy_t *p_policy = (scan_policy_t *)p_msg->para1;
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_policy != NULL);
  
  // Attach tp or bs implement constructed by scan mode and the 
  // implementation of blind scan or tp scan vary with projects
  p_priv->p_policy = p_policy;
  if(p_policy->performance == NULL)
  {
    p_policy->performance = NULL;
  }
  p_policy->get_scan_info(p_policy->p_priv_data, 
    &p_priv->p_priv_data->tuner_id, &p_priv->p_priv_data->ts_in);
  p_priv->p_priv_data->p_rt_data = (running_data_t *)
    p_policy->require_runingtime_buffer(p_policy->p_priv_data,
    sizeof(running_data_t));
  CHECK_FAIL_RET_VOID(p_priv->p_priv_data->p_rt_data != NULL);
  memset(p_priv->p_priv_data->p_rt_data, 0, sizeof(running_data_t));
  init_para(p_priv->p_priv_data);
  p_priv->p_priv_data->p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_SCAN);
  p_priv->scan_app.process_msg = scan_process_msg_runing;  //start running
}

static void scan_stop(scan_priv_t *p_priv)
{
  //destroy policy
  p_priv->p_policy->destroy_policy(p_priv->p_policy);
  p_priv->p_policy = NULL;
  p_priv->scan_app.process_msg = scan_process_msg_idle;
}


static void scan_on_nc_locked(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
  scan_data_t   *p_scan_data = p_scan_priv->p_priv_data;
  dvb_request_t  param = {0};
  //The current tp is always the scan result
  nc_channel_info_t *p_tp_info = &p_scan_data->p_rt_data->result.tp_info;
  if(!p_scan_data->p_rt_data->find_tp.channel_info.lock)
  {

    log_perf(LOG_AP_SCAN, PERF_LOCK_EVT, (u32)(nc_channel_info_t *)(p_tp_info), 0);
  }
  if(p_msg != NULL)
  memcpy(p_tp_info, (void *)p_msg->para1, sizeof(nc_channel_info_t));
  SCAN_DBG("$$$$ lock scan get freq %d, sym %d, 22k %d, pol %d\n",
    p_tp_info->channel_info.frequency,
    p_tp_info->channel_info.param.dvbs.symbol_rate,
    p_tp_info->onoff22k, 
    p_tp_info->polarization);
  //Reset flag information for current tp
  memset(&p_scan_data->p_rt_data->job_dbg_info, 0, sizeof(job_dbg_info_t));

  //Request PAT
  param.req_mode  = DATA_SINGLE;
  param.table_id  = DVB_TABLE_ID_PAT;
  param.para1     = DVB_TABLE_ID_PAT;
  param.ts_in = p_scan_data->ts_in;
  p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
    (u32)&param, sizeof(dvb_request_t));

  log_perf(LOG_AP_SCAN, PERF_PAT_REQUEST, 0, 0);
  p_scan_data->p_rt_data->job_dbg_info.pat_num = 1;
  p_scan_data->p_rt_data->pending_job = 1;
}

static void scan_on_nc_unlocked(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
#ifdef PRINT_ON
  scan_data_t   *p_scan_data = p_scan_priv->p_priv_data;
  nc_channel_info_t *p_tp_info = &p_scan_data->p_rt_data->result.tp_info;

  SCAN_DBG("$$$$ unlock scan get freq %d, sym %d, 22k %d, pol %d\n",
    p_tp_info->channel_info.frequency,
    p_tp_info->channel_info.param.dvbs.symbol_rate,
    p_tp_info->onoff22k, 
    p_tp_info->polarization);
#endif
  log_perf(LOG_AP_SCAN, PERF_UNLOCK_EVT,
   ((nc_channel_info_t *)(p_msg->para1))->channel_info.frequency, 0);
}

static void scan_on_pat_found(scan_priv_t *p_scan_priv, os_msg_t *p_msg, u8 ts_in)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  scan_policy_t *p_policy = p_scan_priv->p_policy;
  dvb_request_t param = {0};

  // Request SDT
  param.req_mode  = DATA_SINGLE;
  param.table_id  = DVB_TABLE_ID_SDT_ACTUAL;
  param.para1     = DVB_TABLE_ID_SDT_ACTUAL;
  param. ts_in = ts_in;
  p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
    (u32)&param, sizeof(dvb_request_t));
  log_perf(LOG_AP_SCAN, PERF_SDT_REQUEST, 0, 0);
  
  if(p_policy->require_extend_table(p_policy,DVB_TABLE_ID_BAT))
  {
    // Request BAT
    log_perf(LOG_AP_SCAN, PERF_BAT_REQUEST, 0, 0);
    scan_request_bat(p_scan_priv);
  }

  if(p_policy->require_extend_table(p_policy,DVB_TABLE_ID_NIT_ACTUAL))
  {
    // Request NIT
    SCAN_DBG("---------------------------------->>request nit in scan\n");
    param.req_mode  = DATA_SINGLE;
    param.table_id  = DVB_TABLE_ID_NIT_ACTUAL;
    param.para1     = DVB_TABLE_ID_NIT_ACTUAL;
    param.para2     = 0xFFFF;
    param. ts_in = ts_in;
    p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
      (u32)&param, sizeof(dvb_request_t));
    
    p_scan_data->p_rt_data->job_dbg_info.nit_num++;
    p_scan_data->p_rt_data->pending_job++;
    log_perf(LOG_AP_SCAN, PERF_NIT_REQUEST, 0, 0);
  }

  if(p_policy->require_extend_table(p_policy,DVB_TABLE_ID_CAT))
  {
    // Request CAT
    SCAN_DBG("---------------------------------->>request cat in scan\n");
    param.req_mode  = DATA_SINGLE;
    param.table_id  = DVB_TABLE_ID_CAT;
    param. ts_in = ts_in;
    p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
      (u32)&param, sizeof(dvb_request_t));
    log_perf(LOG_AP_SCAN, PERF_CAT_REQUEST, 0, 0);
    p_scan_data->p_rt_data->job_dbg_info.cat_num++;
    p_scan_data->p_rt_data->pending_job++;
  }
  
  //Currently only pat has been requested
  
  p_scan_data->p_rt_data->job_dbg_info.pat_num --;
  p_scan_data->p_rt_data->pending_job--;

  p_scan_data->p_rt_data->job_dbg_info.sdt_num++;
  p_scan_data->p_rt_data->pending_job++;
}

static void scan_on_nit_found(scan_priv_t *p_scan_priv, os_msg_t *p_msg, u8 ts_in)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  nit_t *p_nit = (nit_t *)p_msg->para1;

  SCAN_DBG("NIT found ...sec %d, last sec %d\n",
    p_nit->sec_number, p_nit->last_sec_number);
  
  p_scan_data->p_rt_data->job_dbg_info.nit_num --;
  p_scan_data->p_rt_data->pending_job--;

  if(p_nit->sec_number < p_nit->last_sec_number)
  {
    // Request next NIT
    dvb_request_t param = {0};
    
    param.req_mode  = DATA_SINGLE;
    param.table_id  = DVB_TABLE_ID_NIT_ACTUAL;
    param.para1     = DVB_TABLE_ID_NIT_ACTUAL;
    param.para2     = ((p_nit->sec_number + 1) << 16) | 0xFFFF;
    param. ts_in = ts_in;
    p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc,
      DVB_REQUEST, (u32)&param, sizeof(dvb_request_t));
    p_scan_data->p_rt_data->job_dbg_info.nit_num++;
    p_scan_data->p_rt_data->pending_job++;
  }
  
}

#if 0  //for specified bouquet id
static void scan_on_bat_found(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  bat_t *p_bat = (bat_t *)p_msg->para1;
  scan_pg_info_t *p_pg = NULL;
  u8 i = 0;
  
  p_scan_data->p_rt_data->job_dbg_info.bat_num --;
  p_scan_data->p_rt_data->pending_job --;

  SCAN_DBG("BAT found ...sec %d, last sec %d, bouquet id 0x%x\n",
    p_bat->sec_number, p_bat->last_sec_number, p_bat->bouquet_id);
  
#if 1
  if(p_bat->sec_number < p_bat->last_sec_number)
  {
    // Request next BAT
    dvb_request_t param = {0};
    
    param.req_mode  = DATA_SINGLE;
    param.table_id  = DVB_TABLE_ID_BAT;
    param.para1     = DVB_TABLE_ID_BAT;
    param.para2     = ((p_bat->sec_number + 1) << 16) | p_bat->bouquet_id;
    param.ts_in = p_scan_data->ts_in;
    p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc,
      DVB_REQUEST, (u32)&param, sizeof(dvb_request_t));
    p_scan_data->p_rt_data->job_dbg_info.bat_num++;
    p_scan_data->p_rt_data->pending_job++;
  }
//#else

  p_scan_priv->p_priv_data->p_rt_data->start_bat_tickets = mtos_ticks_get();
  for(i = 0; i < p_bat->log_ch_num; i++)
  {
    p_pg = scan_find_pg(p_scan_priv, p_bat->log_ch_info[i].service_id);
    p_pg->logical_num = p_bat->log_ch_info[i].logical_channel_id;
  }
#endif

}
#else
static void scan_on_bat_found(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
#if 0
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
#endif
  bat_t *p_bat = (bat_t *)p_msg->para1;
  scan_pg_info_t *p_pg = NULL;
  u8 i = 0;

  SCAN_DBG("BAT found ...sec %d, last sec %d, bouquet id 0x%x\n",
    p_bat->sec_number, p_bat->last_sec_number, p_bat->bouquet_id);
  
#if 0
  if(p_bat->sec_number < p_bat->last_sec_number)
  {
    // Request next BAT
    dvb_request_t param = {0};
    
    param.req_mode  = DATA_SINGLE;
    param.table_id  = DVB_TABLE_ID_BAT;
    param.para1     = DVB_TABLE_ID_BAT;
    param.para2     = ((p_bat->sec_number + 1) << 16);
    param.ts_in = p_scan_data->ts_in;
    p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc,
      DVB_REQUEST, (u32)&param, sizeof(dvb_request_t));
    p_scan_data->p_rt_data->job_dbg_info.bat_num++;
    p_scan_data->p_rt_data->pending_job++;
  }
#else

  p_scan_priv->p_priv_data->p_rt_data->start_bat_tickets = mtos_ticks_get();
  for(i = 0; i < p_bat->log_ch_num; i++)
  {
    p_pg = scan_find_pg(p_scan_priv, p_bat->log_ch_info[i].service_id);
    CHECK_FAIL_RET_VOID(p_pg != NULL);
    p_pg->logical_num = p_bat->log_ch_info[i].logical_channel_id;
  }

  add_bouquet_id(p_scan_priv, p_bat);
  if(is_need_free_bat(p_scan_priv, p_bat))
  {
    scan_free_bat(p_scan_priv);
  }
#endif

}
#endif

static void scan_on_pmt_info_found(scan_priv_t *p_scan_priv,
os_msg_t *p_msg)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  u32 param = ((p_msg->para1 << 16) | p_msg->para2);
  dvb_request_t req_param = {0};

  //Request PMT information
  req_param.req_mode  = DATA_SINGLE;
  req_param.table_id  = DVB_TABLE_ID_PMT;
  req_param.para1     = DVB_TABLE_ID_PMT;
  req_param.para2     = param;
  req_param.ts_in = p_scan_data->ts_in;
  p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc,
    DVB_REQUEST, (u32)&req_param, sizeof(dvb_request_t));
  
  p_scan_data->p_rt_data->job_dbg_info.pmt_num++;
  p_scan_data->p_rt_data->pending_job++;
  log_perf(LOG_AP_SCAN, PERF_PMT_REQUEST, p_msg->para2, 0);
}

static void scramble_from_ts(scan_priv_t *p_scan_priv, scan_pg_info_t *p_pg)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  ts_data_t *p_ts_data = &p_scan_data->p_rt_data->ts_mgr;
  u16 pid = 0 ;
  
  //Get pid information
  if(p_pg->video_pid != 0)
  {
    pid = (u16)p_pg->video_pid;
  }
  else if(p_pg->audio_ch_num != 0)
  {
    pid = (u16)p_pg->audio[0].p_id;
  }
  else
  {
    pid = 0;
  }
//  SCAN_DBG("sramble state[%d]:%d\n",pid,p_ts_data->pid_scramble_state[pid]);
  //Request ts packet
  if(p_ts_data->pid_scramble_state[pid] == FALSE)
  {
      SCAN_DBG("xxx request a ts packet sid %d \n",p_pg->s_id);

    ts_packet_request(p_scan_priv, p_pg->s_id, pid);
    p_ts_data->pid_scramble_state[pid] = TRUE;
  }
  else
  { 
    //the flag is found
    ts_packet_state_t *p_state = 
      p_ts_data->states + p_ts_data->pid_offset[pid];
//    SCAN_DBG("is found :%d\n",p_state->is_found);
    if(p_state->is_found)
    {
//SCAN_DBG("set pg:%s scramble:%d\n",p_pg->name,p_state->is_scramble);
      set_svc_scramble_state(p_scan_priv, p_pg->s_id, p_state->is_scramble);
    }
  }
//  SCAN_DBG("-------------end------------------------------>>> \n");
  //Record sid state in ts state manager array
  add_sid_state_into_ts_mgr(p_ts_data, p_pg->s_id, pid);

}


static void scan_on_pmt_found(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  scan_policy_t *p_policy = p_scan_priv->p_policy;
  pmt_t *p_pmt = (pmt_t *)p_msg->para1;
  scan_pg_info_t *p_pg = NULL;
//  ts_data_t *p_ts_data = &p_scan_data->p_rt_data->ts_mgr;
//  u16 pid = 0 ;
  scan_scramble_origin_t scramble_org = SCAN_SCRAMBLE_FROM_TS;

  if (p_policy->get_scan_scramble_origin != NULL)
  {
    scramble_org = p_policy->get_scan_scramble_origin(p_policy);
  }

  //Pending job management
  p_scan_data->p_rt_data->job_dbg_info.pmt_num--;
  p_scan_data->p_rt_data->pending_job --;

  if(p_pmt->video_pid == 0
    && p_pmt->audio_count == 0 && p_pmt->pcr_pid == 0)
  {
    //PMT is for data stream
    return;
  }
  
  //Find pointer address in scan result
  //Here prog number in PMT equals to service id
  p_pg = scan_find_pg(p_scan_priv, p_pmt->prog_num);  
  CHECK_FAIL_RET_VOID(p_pg != NULL);

  //Add program information
  add_pg_info(p_pg, p_pmt);
//  SCAN_DBG("-------------start----------------------------->>> on pmt found\n");
//  SCAN_DBG("V_PID:%x\nA_PID1:%x\nPCR_PID:%x\nS_ID:%x\nIS_SCRAM:%d\n",
//    p_pg->video_pid,p_pg->audio[0].p_id,p_pg->pcr_pid,p_pg->s_id,p_pg->is_scrambled);
  //p_scan_priv->p_priv_data->p_rt_data->result.pg_num++;
  
  switch(scramble_org)
  {
    case SCAN_SCRAMBLE_FROM_PMT:
      if(!p_pg->is_scramble_found)
      {
        set_svc_scramble_state(p_scan_priv, p_pg->s_id, ((p_pmt->ecm_cnt > 0) ? TRUE : FALSE));
      }
      break;

    case SCAN_SCRAMBLE_PMT_THEN_TS:
      SCAN_DBG("scan from pmt then ts\n");
      if(!p_pg->is_scramble_found)
      {
        SCAN_DBG("set by pmt flag %d\n", ((p_pmt->ecm_cnt > 0) ? TRUE : FALSE));
        set_svc_scramble_state(p_scan_priv, p_pg->s_id, ((p_pmt->ecm_cnt > 0) ? TRUE : FALSE));
      }

      if(!p_pg->is_scrambled)
      {
        // request ts packet
        scramble_from_ts(p_scan_priv, p_pg);
      }        

      break;
    default:
    case SCAN_SCRAMBLE_FROM_TS:
      scramble_from_ts(p_scan_priv, p_pg);
      break;
  }
}

static void scan_on_sdt_found(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  sdt_t *p_sdt = (sdt_t *)p_msg->para1;
  scan_pg_info_t *p_pg = NULL;
  u32 i = 0;
  dvb_request_t sdt_req = {0};
  
  if(p_sdt->sec_number == 0)
  {
    //request residual sdt after the first sdt arrived
    while(p_sdt->sec_number < p_sdt->last_sec_number)
    {
      sdt_req.req_mode  = DATA_SINGLE;
      sdt_req.table_id  = DVB_TABLE_ID_SDT_ACTUAL;
      sdt_req.para1     = DVB_TABLE_ID_SDT_ACTUAL;
      sdt_req.para2     = ++p_sdt->sec_number;
      sdt_req.ts_in = p_scan_data->ts_in;
      p_scan_data->p_dvb_svc->do_cmd(p_scan_data->p_dvb_svc, DVB_REQUEST,
        (u32)&sdt_req, sizeof(dvb_request_t));
      p_scan_data->p_rt_data->job_dbg_info.sdt_num++;
      p_scan_data->p_rt_data->pending_job++;
    }
  }
  
  for(i = 0; i < p_sdt->svc_count; i++)
  {
    p_pg = scan_find_pg(p_scan_priv, p_sdt->svc_des[i].svc_id);
    CHECK_FAIL_RET_VOID(p_pg != NULL);
    p_pg->running_status = p_sdt->svc_des[i].run_sts;
    
    p_pg->orig_net_id = p_sdt->org_network_id;

    if(p_pg->is_scramble_found != TRUE)
    {
      p_pg->is_scrambled = p_sdt->svc_des[i].is_scrambled;
    }
    p_pg->ca_system_id = p_sdt->svc_des[i].ca_system_id;
    p_pg->ts_id = p_sdt->stream_id;
    p_pg->service_type = p_sdt->svc_des[i].service_type;
    p_pg->volume_compensate = p_sdt->svc_des[i].chnl_vlm_cmpt;
    
    if(p_scan_priv->p_policy->is_no_pg_filter == NULL)
    {
      if (p_pg->service_type == DVB_NVOD_REF_SVC)
      {
        p_pg->s_id = p_sdt->svc_des[i].svc_id;
        //p_scan_priv->p_priv_data->p_rt_data->result.pg_num ++;
      }
    }
    else if(p_scan_priv->p_policy->is_no_pg_filter(p_scan_priv->p_policy) == TRUE)
    {
      p_pg->s_id = p_sdt->svc_des[i].svc_id;
    }
    else
    {
      if (p_pg->service_type == DVB_NVOD_REF_SVC)
      {
        p_pg->s_id = p_sdt->svc_des[i].svc_id;
        //p_scan_priv->p_priv_data->p_rt_data->result.pg_num ++;
      }
    }
    
    if (p_sdt->svc_des[i].mosaic_des_found
      || p_sdt->svc_des[i].logic_screen_des_found
      || p_sdt->svc_des[i].linkage_des_found)
    {
      p_pg->p_mosaic = (mosaic_t *)p_sdt->svc_des[i].p_nvod_mosaic_des;
      p_pg->mosaic_flag = p_pg->p_mosaic->lg_cell_cnt;
      p_sdt->svc_des[i].mosaic_des_found = 0;
      p_sdt->svc_des[i].logic_screen_des_found = 0;
      p_sdt->svc_des[i].linkage_des_found = 0;
    }
    
    //
    //The service name length in sdt is less than 16
    CHECK_FAIL_RET_VOID(MAX_SVC_NAME_SUPPORTED >= MAX_SVC_NAME_LEN);
    memcpy(p_pg->name, p_sdt->svc_des[i].name, MAX_SVC_NAME_LEN - 1);
    p_pg->name[MAX_SVC_NAME_LEN - 1] = 0;
    
    if(p_sdt->svc_des[i].svc_id == 1)
    {
      SCAN_DBG("\n!!!!!is_scramble: %s\n", p_sdt->svc_des[i].name);
    }
  } 

  //Pending job management
  p_scan_data->p_rt_data->job_dbg_info.sdt_num--;
  p_scan_data->p_rt_data->pending_job--;
}

static void scan_on_scrambled(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  u16 sid = 0;
  u16 pid = (u16)p_msg->para1 & 0xFFFF;
  BOOL is_scramble = p_msg->para2;
  ts_packet_state_t *p_ts_state = NULL;
  u8 pid_offset = p_scan_data->p_rt_data->ts_mgr.pid_offset[pid];
  u16 pg_cnt = 0;

  CHECK_FAIL_RET_VOID(pid_offset != 0xFF);
  p_ts_state = p_scan_data->p_rt_data->ts_mgr.states + pid_offset;
  p_ts_state->is_found = TRUE;
  p_ts_state->is_scramble = is_scramble;
  
  for(pg_cnt = 0; pg_cnt < p_ts_state->pg_num; pg_cnt++)
  {
    sid = p_ts_state->pg_arry[pg_cnt];

    //Set service scramble flag

    SCAN_DBG("set sid %d  flag %d by ts\n", sid, is_scramble);
    set_svc_scramble_state(p_scan_priv, sid, is_scramble);
  }
  
  p_scan_data->p_rt_data->job_dbg_info.video_packet_num--;
  p_scan_data->p_rt_data->pending_job--;  
}

static void scan_on_timedout(scan_priv_t *p_scan_priv, os_msg_t *p_msg)
{
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  SCAN_DBG("time out table 0x%x, module %d\n",
    p_msg->para1, p_msg->para2);

  switch(p_msg->para1)
  {
    case DVB_TABLE_ID_PMT:
      p_scan_data->p_rt_data->job_dbg_info.pmt_num --;
      //log_perf(LOG_AP_SCAN, PERF_PMT_FOUND, ((dvb_section_t *)(&(p_msg->para2)))->sid, 0);
      break;   
    case DVB_TABLE_ID_PAT:
      log_perf(LOG_AP_SCAN, PERF_PAT_FOUND, 1, 0);
      p_scan_data->p_rt_data->job_dbg_info.pat_num --;      
      break;
    case DVB_TABLE_ID_NIT_ACTUAL:
      log_perf(LOG_AP_SCAN, PERF_NIT_FOUND, 1, 0);
      p_scan_data->p_rt_data->job_dbg_info.nit_num --;
      break;
    case DVB_TABLE_ID_SDT_ACTUAL:
      log_perf(LOG_AP_SCAN, PERF_SDT_FOUND, 1, 0);
      p_scan_data->p_rt_data->job_dbg_info.sdt_num --;
      break;
    case DVB_TABLE_ID_BAT:
      log_perf(LOG_AP_SCAN, PERF_BAT_FOUND, 1, 0);
      #if 0 //for specified bouquet id
      p_scan_data->p_rt_data->job_dbg_info.bat_num --;
      #else
      scan_free_bat(p_scan_priv);
      #endif
      break;
    case MPEG_TS_PACKET:
      p_scan_data->p_rt_data->job_dbg_info.video_packet_num --;
      break;
    case DVB_TABLE_ID_CAT:
      log_perf(LOG_AP_SCAN, PERF_CAT_FOUND, 1, 0);
      p_scan_data->p_rt_data->job_dbg_info.cat_num --;
      break;
  }
  p_scan_data->p_rt_data->pending_job--;
}

static void scan_cancel(void *p_handle)
{
  scan_priv_t *p_scan_priv = (scan_priv_t *)p_handle;
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;

  p_scan_data->state = SCAN_FINISHED;
  p_scan_priv->scan_app.process_msg = scan_process_msg_idle;  //init to idle
}

static void scan_process_msg_idle(void *p_handle, os_msg_t *p_msg)
{
  scan_priv_t *p_scan_priv = (scan_priv_t *)p_handle;
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  event_t evt = {0};
  
  CHECK_FAIL_RET_VOID(p_msg != NULL);
  switch(p_msg->content)
  {
    case SCAN_CMD_START_SCAN:
      scan_start(p_scan_priv, p_msg);
      log_perf(LOG_AP_SCAN, PERF_BEGIN,0,0);
      p_scan_data->state = SCAN_GET_TP;  
      log_perf(LOG_AP_SCAN, PERF_GET_ONETP_BEGIN,0,0);
      log_perf(LOG_AP_SCAN, PERF_GET_TP_BEGIN,0,0);//start scan

      //The implement policy may override message process of scan process
      p_scan_data->p_rt_data->bs_ver = BLIND_SCAN_VER_UNK;
      
      if(p_scan_priv->p_policy->get_blindscan_ver != NULL)
      {
        p_scan_data->p_rt_data->bs_ver = 
          p_scan_priv->p_policy->get_blindscan_ver(p_scan_priv->p_policy->p_priv_data);
      }
      if(p_scan_priv->p_policy != NULL)
      {
        scan_policy_t *p_policy = p_scan_priv->p_policy;
        if(p_policy->require_extend_table(p_policy,DVB_TABLE_ID_BAT)
          && p_policy->get_bouquet_id_list != NULL)
        {
          p_scan_data->p_rt_data->bouquet_num = 
              p_policy->get_bouquet_id_list(p_policy, p_scan_data->p_rt_data->bouquet_id);
        }        
        p_policy->msg_proc(p_policy->p_priv_data, p_msg);
		p_policy->get_scan_info(p_policy->p_priv_data, 
		     &p_scan_data->tuner_id, &p_scan_data->ts_in);
      }
      break;
    case SCAN_CMD_CANCEL_SCAN:
      evt.id = SCAN_EVT_FINISHED; //ack ui
      ap_frm_send_evt_to_ui(APP_SCAN, &evt);
      break;
    default:
      break;
  }
}

static void scan_process_msg_runing(void *p_handle, os_msg_t *p_msg)
{
  scan_priv_t *p_scan_priv = (scan_priv_t *)p_handle;
  scan_policy_t *p_scan_policy = p_scan_priv->p_policy;
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  nim_channel_perf_t  perf = {0};
  BOOL is_proc_policy = TRUE;
  RET_CODE ret = 0;

  p_scan_policy->get_scan_info(p_scan_policy->p_priv_data, 
    &p_scan_data->tuner_id, &p_scan_data->ts_in);
 
  CHECK_FAIL_RET_VOID(p_msg != NULL);
  //SCAN_DBG("scan get msg 0x%x\n", p_msg->content);
  switch(p_msg->content)
  {
    case NC_EVT_LOCKED:
      nc_perf_get(nc_handle, p_scan_data->tuner_id, &perf);
      SCAN_DBG("SCAN: NC_EVT_TUNER%x_LOCKED,perf[%ld,%ld]\n", 
        p_msg->content == NC_EVT_LOCKED, perf.agc,perf.snr);
      if((p_scan_priv->p_policy != NULL) && 
          (p_scan_priv->p_policy->repeat_tp_check != NULL) &&
          (!p_scan_priv->p_policy->repeat_tp_check
            (p_scan_priv->p_policy->p_priv_data, (nc_channel_info_t *)p_msg->para1)))
      {
        p_scan_data->state = SCAN_GET_TP;
        is_proc_policy = FALSE;
      }
      else
      {
        scan_on_nc_locked(p_scan_priv, p_msg);
        p_scan_data->state = SCAN_SCANING;
      }
      break;
    case NC_EVT_UNLOCKED:
      SCAN_DBG(("SCAN: NC_EVT_TUNER%x_UNLOCKED\n"), 
        (p_msg->content == NC_EVT_LOCKED));
      scan_on_nc_unlocked(p_scan_priv, p_msg);
      p_scan_data->state = SCAN_GET_TP;
      break;
    case DVB_PAT_FOUND: 
      SCAN_DBG(("SCAN: DVB_PAT_FOUND\n"));
      scan_on_pat_found(p_scan_priv, p_msg, p_scan_data->ts_in);
      break;
    case DVB_NIT_FOUND:
      SCAN_DBG(("SCAN: DVB_NIT_FOUND\n"));
      log_perf(LOG_AP_SCAN, PERF_NIT_FOUND, 0, 0);
      scan_on_nit_found(p_scan_priv, p_msg, p_scan_data->ts_in);
      break;
    case DVB_BAT_FOUND:
      SCAN_DBG(("SCAN: DVB_BAT_FOUND\n"));
      log_perf(LOG_AP_SCAN, PERF_BAT_FOUND, 0, 0);
      scan_on_bat_found(p_scan_priv, p_msg);
      break;
    case DVB_PAT_PMT_INFO:
      SCAN_DBG(("SCAN: DVB_PAT_PMT_INFO\n"));
      log_perf(LOG_AP_SCAN, PERF_PAT_FOUND, 0, 0);
      scan_on_pmt_info_found(p_scan_priv, p_msg);
      break;
    case DVB_PMT_FOUND:
      log_perf(LOG_AP_SCAN, PERF_PMT_FOUND, ((pmt_t *)(p_msg->para1))->prog_num, 0);
      SCAN_DBG(("SCAN: DVB_PMT_FOUND\n"));
      scan_on_pmt_found(p_scan_priv, p_msg);
      break;
    case DVB_SCRAMBLE_FLAG_FOUND:
      SCAN_DBG(("SCAN: DVB_SCRAMBLE_FLAG_FOUND\n"));
      scan_on_scrambled(p_scan_priv, p_msg);
      break;
    case DVB_SDT_FOUND:
      SCAN_DBG("SCAN: DVB_SDT_FOUND ticks[%d]\n",mtos_ticks_get());
      log_perf(LOG_AP_SCAN, PERF_SDT_FOUND, 0, 0);
      scan_on_sdt_found(p_scan_priv, p_msg);
      break;
    case DVB_TABLE_TIMED_OUT:
      SCAN_DBG("SCAN: DVB_TABLE_TIMED_OUT%x\n",p_msg->para1);
      scan_on_timedout(p_scan_priv, p_msg);
      break;
    case SCAN_CMD_CANCEL_SCAN:
      p_scan_data->state = SCAN_FINISHED;
      if(p_scan_priv->p_priv_data->p_rt_data->bat_multi_mode == TRUE)
      {
        scan_free_bat(p_scan_priv);
      }
      break;
    case DVB_CAT_FOUND:
      log_perf(LOG_AP_SCAN, PERF_CAT_FOUND, 0, 0);
      p_scan_data->p_rt_data->job_dbg_info.cat_num--;
      p_scan_data->p_rt_data->pending_job--;
      break;
    default:
      break;
  }
  //The implement policy may override message process of scan process
  if(is_proc_policy && (p_scan_priv->p_policy != NULL))
  {
    scan_policy_t *p_policy = p_scan_priv->p_policy;
    if(NC_EVT_LOCKED == p_msg->content)
    {
      log_perf(LOG_AP_SCAN, PERF_SAVE_DB_START,LOCK_TP_DB,0);
      ret = p_policy->msg_proc(p_policy->p_priv_data, p_msg);
      log_perf(LOG_AP_SCAN, PERF_SAVE_DB_DONE,LOCK_TP_DB,0);
    }
    else if(NC_EVT_UNLOCKED == p_msg->content)
    {
      log_perf(LOG_AP_SCAN, PERF_SAVE_DB_START,UNLOCK_TP_DB,0);
      ret = p_policy->msg_proc(p_policy->p_priv_data, p_msg);
      log_perf(LOG_AP_SCAN, PERF_SAVE_DB_DONE,UNLOCK_TP_DB,0);
      log_perf(LOG_AP_SCAN, PERF_GET_ONETP_END,0,0);
      

      log_perf(LOG_AP_SCAN, PERF_GET_ONETP_BEGIN,0,0);
      log_perf(LOG_AP_SCAN, PERF_GET_TP_BEGIN,0,0);
    }
    else
    {
      ret = p_policy->msg_proc(p_policy->p_priv_data, p_msg);
    }

    if(ERR_NO_MEM == ret)
    {
      scan_cancel(p_handle);
    }
  }
}

static void scan_state_machine(void *p_handle)
{
  scan_priv_t *p_scan_priv = (scan_priv_t *)p_handle;
  scan_policy_t *p_scan_policy = p_scan_priv->p_policy;
  scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
  find_tp_ret_t ret = FIND_SUC;   
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  event_t evt = {0};
  
  // State machine
  switch(p_scan_data->state)
  {
    case SCAN_IDLE:
      break;
    case SCAN_GET_TP:
      // Get current tp information
      //log_perf(LOG_AP_SCAN, PERF_GET_TP_BEGIN,0,0);
      //log_perf(LOG_AP_SCAN, PERF_GET_ONETP_BEGIN,0,0);
      ret = p_scan_policy->get_tp_info(p_scan_policy->p_priv_data,
        &p_scan_data->p_rt_data->find_tp);
     
      if(FIND_SUC == ret) //found new tp
      {
        if(!p_scan_data->p_rt_data->find_tp.channel_info.lock)
        {

          log_perf(LOG_AP_SCAN, PERF_GET_TP_END,
            p_scan_data->p_rt_data->find_tp.channel_info.frequency,0);
        }
        SCAN_DBG("\nscan_state_machine SCAN_GET_TP !! freq is %ld ,sym is %ld, pol %d, 22k %d",\
          p_scan_data->p_rt_data->find_tp.channel_info.frequency,\
          p_scan_data->p_rt_data->find_tp.channel_info.param.dvbs.symbol_rate,
          p_scan_data->p_rt_data->find_tp.polarization,
          p_scan_data->p_rt_data->find_tp.onoff22k);
        
        if(p_scan_data->p_rt_data->find_tp.channel_info.lock)
          p_scan_data->state = SCAN_PAT_REQ;
        else
          p_scan_data->state = SCAN_LOCK_TP;
      }
      else if(FIND_DONE == ret)
      {
        log_perf(LOG_AP_SCAN, PERF_END,0,0);
        p_scan_data->state = SCAN_FINISHED;
      }
      else if(FIND_BUSY == ret)
      {
        //waiting....
      }
      else
      {
        //what's happend? UFO?
        CHECK_FAIL_RET_VOID(0);
      }
      break;
    case SCAN_PAT_REQ:
          init_para(p_scan_data);
          memcpy(&p_scan_data->p_rt_data->result.tp_info, \
                       &p_scan_data->p_rt_data->find_tp, sizeof(nc_channel_info_t));
        scan_on_nc_locked(p_scan_priv,NULL);
        p_scan_data->state = SCAN_SCANING;
      break;
    case SCAN_LOCK_TP:
      //Set current tp information
      log_perf(LOG_AP_SCAN,PERF_DO_LOCK, 0, 0);
      nc_set_tp(nc_handle, p_scan_data->tuner_id, p_scan_data->p_nc_svc,
        &p_scan_data->p_rt_data->find_tp);
      p_scan_data->p_rt_data->job_dbg_info.spend_ticks = mtos_ticks_get();

      //reset parameter
      init_para(p_scan_data);
      memcpy(&p_scan_data->p_rt_data->result.tp_info, 
        &p_scan_data->p_rt_data->find_tp, sizeof(nc_channel_info_t));

      //Wait new the lock status of recently set tp
      p_scan_data->state = SCAN_WAIT_TP_LOCK;
      break;
    case SCAN_WAIT_TP_LOCK:
      break;
    case SCAN_FINISHED:
      SCAN_DBG("scan state: SCAN_FINISHED\n");
      log_perf(LOG_AP_SCAN, PERF_FINISH, 0, 0);
      scan_stop(p_scan_priv);
   
      evt.id = SCAN_EVT_FINISHED;
      evt.data1 = 0;
      ap_frm_send_evt_to_ui(APP_SCAN, &evt);
      p_scan_data->state = SCAN_IDLE;
      nvod_mosaic_buf_reset(class_get_handle_by_id(NVOD_MOSAIC_CLASS_ID));
      break;
    case SCAN_SCANING:
      if(p_scan_data->p_rt_data->pending_job == 0)
      {
        RET_CODE ret_1 = SUCCESS; 
        /*
        CHECK_FAIL_RET_VOID(p_scan_data->p_rt_data->job_dbg_info.pat_num == 0);
        CHECK_FAIL_RET_VOID(p_scan_data->p_rt_data->job_dbg_info.sdt_num == 0);
        CHECK_FAIL_RET_VOID(p_scan_data->p_rt_data->job_dbg_info.pmt_num == 0);
        CHECK_FAIL_RET_VOID(p_scan_data->p_rt_data->job_dbg_info.video_packet_num == 0);
        CHECK_FAIL_RET_VOID(p_scan_data->p_rt_data->job_dbg_info.nit_num == 0);
        */
        log_perf(LOG_AP_SCAN,PERF_SAVE_DB_START,PROG_DB,0);
        
        p_scan_data->p_rt_data->result.pg_num = p_scan_data->p_rt_data->cur_pg_offset;
        ret_1 = p_scan_policy->on_tp_done(p_scan_policy->p_priv_data,
          &p_scan_data->p_rt_data->result);
        log_perf(LOG_AP_SCAN,PERF_SAVE_DB_DONE,PROG_DB,0);
        if(ret_1 == SUCCESS)
        {
          log_perf(LOG_AP_SCAN, PERF_GET_ONETP_END,0,0);
          p_scan_data->state = SCAN_GET_TP;

          log_perf(LOG_AP_SCAN, PERF_GET_ONETP_BEGIN,0,0);
          log_perf(LOG_AP_SCAN, PERF_GET_TP_BEGIN,0,0);
        }
        else
        {
          //Database is full or some error, break scan
          p_scan_data->state = SCAN_IDLE;
        }
      }
      #if 0 //for specified bouquet id
      #else
      else if(p_scan_data->p_rt_data->bat_multi_mode)
      {
        if(mtos_ticks_get() - p_scan_data->p_rt_data->start_bat_tickets
            > 100*2)
        {
          scan_free_bat(p_scan_priv);
        }
      }
      #endif
      /*  //for debug
      else
      {
        if(mtos_ticks_get() - p_scan_data->p_rt_data->job_dbg_info.spend_ticks
            > 100*2)
        {
          job_dbg_info_t *p_dbg = &p_scan_data->p_rt_data->job_dbg_info;
          SCAN_DBG("pending job %d, nit %d, pat %d, pmt %d, sdt %d, v %d\n",
            p_scan_data->p_rt_data->pending_job,
            p_dbg->nit_num, p_dbg->pat_num, p_dbg->pmt_num, 
            p_dbg->sdt_num, p_dbg->video_packet_num);
        }
      }
      */
      
      break;
    default:
      break;
  }
}

static void init(handle_t p_handle)
{
  scan_priv_t *p_scan = (scan_priv_t *)p_handle;

  if(p_scan->p_priv_data->is_init == FALSE)
  {
    dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
    nim_ctrl_t *p_nc = class_get_handle_by_id(NC_CLASS_ID);
    /*class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
    u8 tuner_id = 0, ts_in = 0;*/
    p_scan->p_priv_data->is_init = TRUE; 
    p_scan->p_priv_data->p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_SCAN);
    p_scan->p_priv_data->p_nc_svc = nc_get_svc_instance(p_nc, APP_SCAN);
    /*nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);       
    p_scan->p_priv_data->ts_in = ts_in;
    p_scan->p_priv_data->tuner_id = tuner_id;*/
  }

}

app_t *construct_ap_scan(void)
{
  scan_priv_t *p_scan_priv = mtos_malloc(sizeof(scan_priv_t));
  
  CHECK_FAIL_RET_NULL(p_scan_priv != NULL);

  //Set current scan policy empty
  p_scan_priv->p_policy = NULL;

  //Malloc private data for scan
  p_scan_priv->p_priv_data = mtos_malloc(sizeof(scan_data_t));
  CHECK_FAIL_RET_NULL(p_scan_priv->p_priv_data != NULL);
  memset(p_scan_priv->p_priv_data, 0, sizeof(scan_data_t));
  memset(&(p_scan_priv->scan_app), 0, sizeof(app_t));
  
  // Attach scan applition

  // Since we don't know derived scan's pre scan behavior, we can't filter
  // any message. Like NIT table we don't use it in abstract scan, but NIT scan
  // class will use it.
  // The message list of scan should not be NULL 
  p_scan_priv->scan_app.init = init;
  p_scan_priv->scan_app.process_msg = scan_process_msg_idle;  //init to idle
  p_scan_priv->scan_app.state_machine = scan_state_machine;
  p_scan_priv->scan_app.get_msgq_timeout = NULL;
  p_scan_priv->scan_app.p_data = (void *)p_scan_priv;

  return &p_scan_priv->scan_app;
}


