/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"

#include "lib_util.h"

#include "drv_dev.h"
#include "uio.h"
#include "nim.h"
#include "class_factory.h"
#include "service.h"
#include "mdl.h"
#include "err_check_def.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ap_framework.h"
#include "ap_signal_monitor.h"


/*!
  playback status 
  */
typedef enum
{
  /*!
    idle.
    */
  SIG_MON_STS_IDLE = 0,
  /*!
    signal monitor motor continue mode.
    */
  SIG_MON_STS_MOTOR_CONTINUE,
} sig_mon_sm_t;

typedef enum
{
  /*!
    unlocked
    */
  STATUS_UNLOCKED = 0,
  /*!
    locked
    */
  STATUS_LOCKED = 1,
  /*!
    INVALID
    */
  STATUS_INVALID = 2
} LOCK_STATUS;

/*!
  Signal monitor private data
  */
typedef struct
{
  /*!
    Signal monitor start tick
    */
  u32 start_tick;
  /*!
    Signal monitor performed flag
    */
  BOOL monitor_perf[TUNER_NUM];
  /*!
    NC service handle
    */
  service_t *p_nc_svc;
  /*!
    NC monitor info
    */
  sig_mon_info_t info[TUNER_NUM];
  /*!
    status machine
    */
  sig_mon_sm_t sm;  
  /*!
    continous drv tunner id.
    */
  u8 tunner_id;    
  /*!
    tunner number.
    */
  u8 tunner_number;    
} signal_mon_data_t;

/*!
  Private info for signal monitor APP.
  */
typedef struct 
{
  /*!
    signal monitor private data
    */
  signal_mon_data_t *p_data;
  /*!
    Signal monitor implement policy
    */
  sig_mon_policy_t *p_sig_mon_impl;
  /*!
    Signal monitor instance
    */
  app_t instance;
} sig_mon_priv_t;

static void sf_send_notify(sig_mon_evt_t s_evt, u32 data1, u32 data2)
{
  event_t evt;
  evt.id = s_evt;
  evt.data1 = data1;
  evt.data2 = data2;
  ap_frm_send_notify_to_ui(APP_SIGNAL_MONITOR, &evt);
}

static void sf_send_evt(sig_mon_evt_t s_evt, u32 data1, u32 data2)
{
  event_t evt;
  evt.id = s_evt;
  evt.data1 = data1;
  evt.data2 = data2;
  ap_frm_send_evt_to_ui(APP_SIGNAL_MONITOR, &evt);
}

static void init(handle_t p_handle)
{
  sig_mon_priv_t *p_priv = (sig_mon_priv_t *)p_handle;
  nim_ctrl_t *p_nc = class_get_handle_by_id(NC_CLASS_ID);

  p_priv->p_data->p_nc_svc = nc_get_svc_instance(p_nc, APP_SIGNAL_MONITOR);
}

static BOOL is_valid_tp(dvbs_lock_info_t *p_info, sys_signal_t lock_mode)
{
  u32 lock_freq = 0;
  u32 tp_freq_min = 0;
  u32 tp_freq_max = 0;

  if(lock_mode == SYS_DVBS)
  {
    lock_freq = dvbs_calc_mid_freq(&p_info->tp_rcv, &p_info->sat_rcv);
    tp_freq_min = p_info->sat_rcv.tuner_type == 1 ? 250 * KHZ : 950 * KHZ;
    tp_freq_max = p_info->sat_rcv.tuner_type == 1 ? 950 * KHZ : 2150 * KHZ;
    if((lock_freq > tp_freq_max) || (lock_freq < tp_freq_min))
    {
      return FALSE;
    }
  }
  else if(lock_mode == SYS_DTMB)
  {

  }
  else if(lock_mode == SYS_DVBC)
  {

  }
  else
  {
  }
  return TRUE;
}

static void set_freq_lock(sig_mon_priv_t *p_priv, void *p_info, 
    sys_signal_t lock_mode, u8 tuner_id)
{
  nc_channel_info_t search_info = {0};
  sat_rcv_para_t *p_sat_rcv = NULL;
  tp_rcv_para_t  *p_tp_rcv  = NULL;
  service_t *p_nc_svc = p_priv->p_data->p_nc_svc;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  if(lock_mode == SYS_DVBS)
  {
    dvbs_lock_info_t *p_dvbs_lock_info = (dvbs_lock_info_t *)p_info;
    p_sat_rcv = &p_dvbs_lock_info->sat_rcv;
    p_tp_rcv  = &p_dvbs_lock_info->tp_rcv;
    nc_set_diseqc(nc_handle, tuner_id, &p_dvbs_lock_info->disepc_rcv);
    nc_set_12v(nc_handle, tuner_id, (u8)p_sat_rcv->v12);
    
    if(FALSE == is_valid_tp(p_dvbs_lock_info, lock_mode))  //pass invalid parameter
    {
      dvbs_calc_search_info(&search_info, p_sat_rcv, p_tp_rcv);
    
      search_info.channel_info.frequency = 900 * KHZ;//p_sat_rcv->lnb_low;
      search_info.channel_info.param.dvbs.symbol_rate = p_tp_rcv->sym;
    }
    else
    {
      dvbs_calc_search_info(&search_info, p_sat_rcv, p_tp_rcv);
    }
  }
  else if(lock_mode == SYS_DVBT2 || lock_mode == SYS_DTMB)
  {
    dvbt_lock_info_t *p_dvbt_lock_info = (dvbt_lock_info_t *)p_info;
    search_info.channel_info.nim_type = p_dvbt_lock_info->nim_type;
    search_info.channel_info.param.dvbt.nim_type = p_dvbt_lock_info->nim_type;
    search_info.channel_info.frequency = p_dvbt_lock_info->tp_freq;
    search_info.channel_info.param.dvbt.band_width = p_dvbt_lock_info->band_width;
    search_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
  }
  else if(lock_mode == SYS_DVBC)
  {
    dvbc_lock_info_t *p_dvbc_lock_info = (dvbc_lock_info_t *)p_info;
    search_info.channel_info.nim_type = NIM_DVBC;
    search_info.channel_info.frequency = p_dvbc_lock_info->tp_freq;
    search_info.channel_info.param.dvbc.symbol_rate = p_dvbc_lock_info->tp_sym;
    search_info.channel_info.param.dvbc.modulation = p_dvbc_lock_info->nim_modulate;
    search_info.polarization = NIM_PORLAR_HORIZONTAL;  
    search_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
    search_info.channel_info.lock = NIM_CHANNEL_UNLOCK;      
  }
  else
  {
    CHECK_FAIL_RET_VOID(0);
  }

  nc_set_tp(nc_handle, tuner_id, p_nc_svc, &search_info);
}


static u32 get_signal_info(sig_mon_priv_t *p_priv, u8 tuner_id)
{
  nim_channel_perf_t signal = {0};
  nc_channel_info_t tp_info = {0};
  
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  
  nc_perf_get(nc_handle, tuner_id, &signal);
  nc_get_tp(nc_handle, tuner_id, &tp_info);
  p_priv->p_data->info[tuner_id].tp_freq = tp_info.channel_info.frequency;
  p_priv->p_data->info[tuner_id].tuner_id= tuner_id;

  p_priv->p_sig_mon_impl->p_convert_perf(&signal, &p_priv->p_data->info[tuner_id]);

  return (u32)p_priv->p_data->info;
}

static u32 get_nim_locked(sig_mon_priv_t *p_priv, u8 tuner_id)
{
  static u8 repeat_time[2] = {0};
  LOCK_STATUS status = STATUS_INVALID;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  BOOL flag = FALSE;

  {
    flag = nc_get_lock_status(nc_handle, tuner_id);
    OS_PRINTF("tuner[%d], get_lock[%d], repeat_time[%d]\n",
      tuner_id, flag, repeat_time[tuner_id]);
    if(flag)
    {
      repeat_time[tuner_id] = 0;
      status = STATUS_LOCKED;
    }
    else if(repeat_time[tuner_id]++ > 1)
    {
      repeat_time[tuner_id] = 0;
      status = STATUS_UNLOCKED;
    }
  }
  if((p_priv->p_sig_mon_impl->lock_led_set != NULL) && (status != STATUS_INVALID))
  {
    p_priv->p_sig_mon_impl->lock_led_set(status, tuner_id);
  }
  OS_PRINTF("status[%d][%x]\n", tuner_id, status);
  return status;
}

static void task_single_step(void *p_handle, os_msg_t *p_cmd)
{
  sig_mon_priv_t *p_priv = (sig_mon_priv_t *)p_handle;
  signal_mon_data_t *p_data = p_priv->p_data;  
  u8 event_tuner_id = TUNER0;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  u32 lock_status = 0;
  BOOL is_left = FALSE;
  
  if(p_cmd != NULL)
  {    
    switch(p_cmd->content)
    {
      case SIG_MON_LOCK_TP:
        event_tuner_id = (p_cmd->para2)>>16;
        set_freq_lock(p_priv, (void *)p_cmd->para1, 
            (sys_signal_t)(p_cmd->para2&0x0000FFFF), 
            event_tuner_id);
        //ack sync cmd.
        sf_send_evt(SIG_MON_SYNC_LOCK, 0, 0);            
        break;
      case SIG_MON_MONITOR_PERF:
        event_tuner_id = p_cmd->para2;
        p_data->monitor_perf[event_tuner_id] = p_cmd->para1;
        break;
      case SIG_MON_RESET:
        sf_send_notify(SIG_MON_SIGNAL_INFO, 0, (SF_SIGNAL_STABLE << 8) 
            | (SF_SIGNAL_STABLE));
        OS_PRINTF("!!!SIG_MON_RESET %d\n", SF_SIGNAL_STABLE);
        break;
      case NC_EVT_POS_TURNING:
        event_tuner_id = p_cmd->context;
        sf_send_notify(SIG_MON_POS_TURNING, 0, event_tuner_id << 24);
        break;
      case NC_EVT_POS_OUT_RANGE:
        event_tuner_id = p_cmd->context;
        sf_send_notify(SIG_MON_POS_OUT_RANGE, 0, event_tuner_id << 24);
        break;
      case NC_EVT_LOCKED: 
        OS_PRINTF("Signal Status : LOCKED\n");
        event_tuner_id = p_cmd->context;
        nc_enable_monitor(nc_handle, event_tuner_id, TRUE);
        sf_send_notify(SIG_MON_LOCKED, 0, event_tuner_id << 24);
        break;
      case NC_EVT_UNLOCKED:
        OS_PRINTF("Signal Status : UNLOCKED\n");
        event_tuner_id = p_cmd->context;
        sf_send_notify(SIG_MON_UNLOCKED, 0, event_tuner_id << 24);
        break;  

      case SIG_MON_MOTOR_DRV_STEP:
        is_left = (BOOL)((p_cmd->para1 >> 8) & 0xFF);
        p_data->tunner_id = (BOOL)(p_cmd->para1 & 0xFF);

        nc_positioner_ctrl(nc_handle, p_data->tunner_id,
            is_left ? RSCMD_PSTER_DRV_W : RSCMD_PSTER_DRV_E,
            RS_PSTER_PARAM(RSMOD_PSTER_DRV_STEPS, p_cmd->para2));  

        p_data->sm = SIG_MON_STS_IDLE;

        //ack sync cmd.
        sf_send_evt(SIG_MON_DRV_STEP, 0, 0);
        break;

      case SIG_MON_MOTOR_DRV_AUTO:
        is_left = (BOOL)((p_cmd->para1 >> 8) & 0xFF);
        p_data->tunner_id = (BOOL)(p_cmd->para1 & 0xFF);

        //set continue drv mode.
        nc_positioner_ctrl(nc_handle, p_data->tunner_id,
            is_left ? RSCMD_PSTER_DRV_W : RSCMD_PSTER_DRV_E,
            RS_PSTER_PARAM(RSMOD_PSTER_DRV_CONTINUE, 0));    

        p_data->sm = SIG_MON_STS_MOTOR_CONTINUE;

        //ack sync cmd.
        sf_send_evt(SIG_MON_DRV_AUTO, 0, 0);            
        break;

      case SIG_MON_MOTOR_DRV_STOP:
        nc_positioner_ctrl(nc_handle, p_cmd->para1, RSCMD_PSTER_STOP, 0);

        p_data->sm = SIG_MON_STS_IDLE;

        //ack sync cmd.
        sf_send_evt(SIG_MON_DRV_STOP, 0, 0);            
        break;
        
      default:
        break; 
    }
  }
  else
  {
    switch(p_data->sm)
    {
      case SIG_MON_STS_MOTOR_CONTINUE:
        OS_PRINTF("continue state:\n");
        if(nc_get_lock_status(nc_handle, p_data->tunner_id))
        {
          nc_positioner_ctrl(nc_handle, 0, RSCMD_PSTER_STOP, 0);

          sf_send_evt(SIG_MON_DRV_LOCKED, 0, 0);

          p_data->sm = SIG_MON_STS_IDLE;
        }
        break;

      default:
        break;
    }
  }
  //else
  {
    u32 cur_tick = mtos_ticks_get();
    u32 signal = 0;
    u8  i = 0;
    BOOL is_send = FALSE;
    LOCK_STATUS status = STATUS_INVALID;
    
    if((cur_tick - p_data->start_tick) >= 40)//pre 40 ticks
    {
      for(i = 0; i < p_data->tunner_number; i ++)
      {
        if(nc_is_finish_locking(nc_handle, i))
        {
          if(p_data->monitor_perf[i])
          {
            get_signal_info(p_priv, i);
          }
          else
          {
             memset(&p_priv->p_data->info[i], 0, sizeof(sig_mon_info_t));
          }
          status = get_nim_locked(p_priv, i);
          if(status != STATUS_INVALID)
          {
            lock_status |=  status << (8 * i);
            is_send = TRUE;
            OS_PRINTF("!!!!Signal monitor id[%d], lock[%x]\n", i, 
              lock_status);
          }
          else
            lock_status |= 0xff << (8 * i);//invalid status,not send when only one tuner
        }  
        else
        {
          lock_status |= 0xff << (8 * i);//invalid status,not send when only one tuner
        }
      }
      if(is_send)
      {
        signal = (u32)p_priv->p_data->info;
        sf_send_notify(SIG_MON_SIGNAL_INFO, signal, lock_status);
      }
      p_data->start_tick = cur_tick;      
    }
  }
}

app_t *construct_ap_signal_monintor(sig_mon_policy_t *p_impl_policy)
{
  sig_mon_priv_t *p_priv = mtos_malloc(sizeof(sig_mon_priv_t));
  CHECK_FAIL_RET_NULL(p_priv != NULL);

  //Create private data buffer
  p_priv->p_data = mtos_malloc(sizeof(signal_mon_data_t));
  CHECK_FAIL_RET_NULL(p_priv->p_data != NULL);

  memset(p_priv->p_data, 0, sizeof(signal_mon_data_t));
  memset(&(p_priv->instance), 0, sizeof(app_t));
  
  //Attach SM implement policy
  CHECK_FAIL_RET_NULL(p_impl_policy != NULL);
  p_priv->p_sig_mon_impl = p_impl_policy;
  p_priv->p_data->sm = SIG_MON_STS_IDLE;
  {
    class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
    CHECK_FAIL_RET_NULL(nc_handle != NULL);
    p_priv->p_data->tunner_number = nc_get_tuner_count(nc_handle);
  }
  
  //Attach APP signal instance
  p_priv->instance.p_data = p_priv;
  p_priv->instance.init = init;
  p_priv->instance.task_single_step = task_single_step;
  
  return &p_priv->instance;
}
