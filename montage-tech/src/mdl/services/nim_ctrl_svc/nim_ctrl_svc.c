/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_mutex.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_usals.h"
#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "uio.h"

#define NC_SVC_Q_DEPTH (20)

/*!
 how many times per second. 3 time/per.
 */
#define MAX_CHECK_LOCK_TICKS_NUM      30

/*!
  nim ctrl status
  */
typedef enum
{
  NC_IDLE = 0,
  NC_DO_BS,
  NC_CHECK_LOCK,
  NC_MONITOR,
  NC_WAIT_IQ_CHANGE
} nc_state_t;

typedef struct
{
  u8 tuner_id;
  u8 ts_in;
  u32 start_ticks;
  u32 duration_ticks;
  u32 checklock_ticks;
  u32 monitor_ticks;
  u32 wait_ticks;
  u32 maxticks_cnk;
  BOOL is_sync_lock;
  BOOL auto_IQ;
  BOOL for_bs;
  BOOL tuner_locking;
  BOOL monitor_enable;
  BOOL dvb_s2_t2_support;    
  nim_device_t *p_nim_dev;
  nc_channel_info_t cur_channel_info;
  u8 nim_port;
  u8 onoff12v;
  nc_state_t status;

  nc_diseqc_info_t dis_info;
  BOOL dis_need_reset;
  BOOL lnb_check_enble;
  BOOL diseqc_disable;//don't set diseqc when full tp scan
  u32 last_ticks;
  sys_signal_t lock_mode;
  sys_signal_t cur_lock_mode;
  u32 mutex_lock_mode;
  void *p_mutex;
  BOOL need_dvb_switch;
  BOOL dvbt2_switch_enable;  
  u32 dvbt_lock_mode;
} nc_tunerx_data_t;

typedef struct
{
  nim_ctrl_t *p_this;
} nim_ctrl_svc_data_t;

typedef struct
{
    u32 msgq_id;
    u32 tuner_num;
    service_t *p_last_svc;
    svc_container_t *p_container;
    nim_device_t *p_spec_nim_dev;
    nc_tunerx_data_t  nc_tuner[2];
}nc_priv_data_t;
static u32 g_active_tuner = TUNER0;
/*!
  Declaration
  */
static void _set_22k(class_handle_t handle, u8 onoff22k, u8 tuner_id);
static void _set_polarization(class_handle_t handle, nim_lnb_porlar_t polar, u8 tuner_id);
static void _switch_ctrl(class_handle_t handle,
        nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port, u8 tuner_id);

static void _positioner_ctrl(class_handle_t handle,
      rscmd_positer_t cmd, u32 param, u8 tuner_id);

static BOOL _usals_ctrl(class_handle_t handle, double sat_longitude,
        double local_longitude, double local_latitude, u8 tuner_id);

static BOOL _set_diseqc(class_handle_t handle, nc_diseqc_info_t *p_diseqc, u8 tuner_id);

static void _diseqc_ctrl(class_handle_t handle, nim_diseqc_cmd_t *p_diseqc_cmd, u8 tuner_id);
static void _init_nc_channel_info(nc_channel_info_t *p_channel_info, u8 tuner_id);
static nim_device_t * _get_spec_nim_dev(nim_ctrl_t *p_this, u8 tuner_id);
#ifdef PRINT_ON
static void print_tpinfo(sys_signal_t  lock_mode, nc_channel_info_t *p_tp_info);
#endif

static void send_evt(nc_priv_data_t *p_priv, nc_evt_t evt, u8 tuner_id)
{
  os_msg_t msg = {0};
  msg.content = evt;
  msg.context = tuner_id;
  mdl_broadcast_msg(&msg);
}

static void nim_ctrl_lock(nc_priv_data_t *p_priv, u8 tuner_id)
{
  if (OS_MUTEX_LOCK ==p_priv->nc_tuner[tuner_id].mutex_lock_mode) {
     mtos_mutex_take(p_priv->nc_tuner[tuner_id].p_mutex);
  }else{
     mtos_task_lock();
  }
}

static void nim_ctrl_unlock(nc_priv_data_t *p_priv, u8 tuner_id)
{
  if (OS_MUTEX_LOCK ==p_priv->nc_tuner[tuner_id].mutex_lock_mode) {
     mtos_mutex_give(p_priv->nc_tuner[tuner_id].p_mutex);
  }else{
     mtos_task_unlock();
  }
}

static void send_lock_result(nc_priv_data_t *p_priv, nc_evt_t evt, u8 tuner_id)
{
  os_msg_t msg = {0};

  msg.content = evt;
  msg.para1 = (u32)&p_priv->nc_tuner[tuner_id].cur_channel_info;
  msg.para2 = sizeof(nc_channel_info_t);
  msg.context = tuner_id;
  OS_PRINTF("nc: send evt lock[%d]\n", (evt == NC_EVT_LOCKED));
  //send by last svc, fix me
  p_priv->p_last_svc->notify(p_priv->p_last_svc, &msg);
}


static BOOL cmp_channel(nc_priv_data_t *p_priv,
  nc_channel_info_t *p_old, nc_channel_info_t *p_new, u8 tuner_id)
{
  BOOL res = FALSE;

  if(p_old->channel_info.nim_type != p_new->channel_info.nim_type)
  {
    OS_PRINTF("not same nim type,%d,%d!\n",p_old->channel_info.nim_type,p_new->channel_info.nim_type);
    return FALSE;
  }

  switch(p_priv->nc_tuner[tuner_id].cur_lock_mode)
  {
    case SYS_DVBC:
      res = ((p_old->channel_info.frequency == p_new->channel_info.frequency)
        && (p_old->channel_info.param.dvbc.symbol_rate ==
            p_new->channel_info.param.dvbc.symbol_rate)
        && (p_old->channel_info.param.dvbc.modulation ==
            p_new->channel_info.param.dvbc.modulation));
      break;

    case SYS_DVBS:
    case SYS_ABSS:
      res = ((p_old->onoff22k == p_new->onoff22k)
        && (p_old->polarization == p_new->polarization)
        && (p_old->channel_info.frequency == p_new->channel_info.frequency)
        && (p_old->channel_info.spectral_polar ==
            p_new->channel_info.spectral_polar)
        && (p_old->channel_info.param.dvbs.symbol_rate ==
            p_new->channel_info.param.dvbs.symbol_rate)
        && (p_old->channel_info.param.dvbs.uc_param.bank ==
            p_new->channel_info.param.dvbs.uc_param.bank)
        && (p_old->channel_info.param.dvbs.uc_param.user_band ==
            p_new->channel_info.param.dvbs.uc_param.user_band)
        && (p_old->channel_info.param.dvbs.uc_param.ub_freq_mhz ==
            p_new->channel_info.param.dvbs.uc_param.ub_freq_mhz));
      break;

    case SYS_DTMB:
      res = (p_old->channel_info.frequency == p_new->channel_info.frequency);
      break;
    case SYS_DVBT2:
      res = ((p_old->channel_info.frequency == p_new->channel_info.frequency)
        && (p_old->channel_info.param.dvbt.band_width ==
            p_new->channel_info.param.dvbt.band_width)
        && (p_old->channel_info.param.dvbt.plp_id ==
            p_new->channel_info.param.dvbt.plp_id)
        && (p_old->channel_info.param.dvbt.nim_type==
            p_new->channel_info.param.dvbt.nim_type)         
      );
      break;
    default:
      OS_PRINTF("%s, please check lock mode[%d]\n",__FUNCTION__,
        p_priv->nc_tuner[tuner_id].cur_lock_mode);
      break;
  }

  return res;

}

static BOOL tc_need_relock(nc_priv_data_t *p_priv,
  nc_channel_info_t *p_info, BOOL for_bs, u8 tuner_id)
{
  BOOL b_relock = FALSE;
  nc_channel_info_t  cur_nc_nim_info = {0};

  if(g_active_tuner != tuner_id)
  {
    OS_PRINTF("lock tuner [%d] change to [%d]\n", g_active_tuner,
      tuner_id);
    g_active_tuner = tuner_id;
    b_relock = TRUE;
  }
  if(for_bs)  //blind scan v1 or dvbt scan, set tuner force
  {
    b_relock = TRUE;
  }
  if(p_priv->nc_tuner[tuner_id].lock_mode == SYS_DVB_3IN1)
  {
    sys_signal_t new_lock_mode = SYS_DVB_3IN1;
    switch(p_info->channel_info.nim_type)
    {
      case NIM_DVBS:
      case NIM_DVBS2:
      case NIM_DVBS_AUTO:
        new_lock_mode = SYS_DVBS;
        break;
      case NIM_DVBT:
      case NIM_DVBT2:
      case NIM_DVBT_AUTO:
      case NIM_DVBT_ONLY:
      case NIM_DVBT2_ONLY:
        new_lock_mode = SYS_DVBT2;
        break;
      case NIM_DTMB:
        new_lock_mode = SYS_DTMB;
      case NIM_DVBC:
        new_lock_mode = SYS_DVBC;
        break;
      default:
        OS_PRINTF("%s, Please check tp nim type[%d]\n",__FUNCTION__,
          p_info->channel_info.nim_type);
        new_lock_mode = SYS_DVBS;
        break;
    }
    if(p_priv->nc_tuner[tuner_id].cur_lock_mode != new_lock_mode)
    {
      OS_PRINTF("lock mode [%d] change to [%d]\n", p_priv->nc_tuner[tuner_id].cur_lock_mode,
        new_lock_mode);
      p_priv->nc_tuner[tuner_id].cur_lock_mode = new_lock_mode;
      p_priv->nc_tuner[tuner_id].cur_channel_info.polarization = 0xFF;
      b_relock = TRUE;
    }
  }
  if(b_relock)
    return b_relock;
  //get actual tp info
#if 0
  memcpy(&cur_nc_nim_info, &p_priv->nc_tuner[tuner_id].cur_channel_info,
    sizeof(nc_channel_info_t));
#else
  dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_GET_CHANNEL_INFO, (u32)&cur_nc_nim_info.channel_info);
#endif

  if (!cmp_channel(p_priv, &cur_nc_nim_info, p_info, tuner_id))
  {
    OS_PRINTF("tp info changed\n");
    b_relock = TRUE;
  }
  else if((NC_MONITOR == p_priv->nc_tuner[tuner_id].status) || 
    (NC_IDLE == p_priv->nc_tuner[tuner_id].status))
  {
    if(p_priv->nc_tuner[tuner_id].tuner_locking) //tuner locked
    {
      memcpy(&p_priv->nc_tuner[tuner_id].cur_channel_info, &cur_nc_nim_info,
        sizeof(nc_channel_info_t));
      send_lock_result(p_priv, NC_EVT_LOCKED, tuner_id);
      p_priv->nc_tuner[tuner_id].status = NC_MONITOR;
    }
    else
    {
      b_relock = TRUE;
    }
  }
  else
  {
    //tuner locking and no any parameter exchanged. do nothing
  }
  return b_relock;
}

static void do_check_lnb(nim_ctrl_t *p_this, u8 tuner_id)
{
  //try to remove protect
  nc_priv_data_t *p_priv = p_this->p_data;
  nim_device_t *p_spec_nim_dev = _get_spec_nim_dev(p_this, tuner_id);

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none do_check_lnb \n");
    return;
  }
  dev_io_ctrl(p_spec_nim_dev, NIM_REMOVE_PROTECT, 0);
  //delay 10ms.
  mtos_task_delay_ms(10);
  //reset
  dev_io_ctrl(p_spec_nim_dev, NIM_IOCTRL_LNB_SC_PROT_RESTORE, 0);
}

static void dvbs_switch_nimtype(nc_priv_data_t *p_priv, u8 tuner_id)
{
  if((p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBS) && 
    (p_priv->nc_tuner[tuner_id].dvb_s2_t2_support == TRUE))
  {
    OS_PRINTF("%s,nim_type[%d]\n",__FUNCTION__,
      p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbs.nim_type);
    switch(p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbs.nim_type)
    {
      case NIM_DVBS:
        p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbs.nim_type = NIM_DVBS2;
        p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.nim_type = NIM_DVBS2;
        break;
      case NIM_DVBS2:
      default:
        p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbs.nim_type = NIM_DVBS;
        p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.nim_type = NIM_DVBS;
        break;
    }
  }
}

static void dvbt2_switch_nimtype(nc_priv_data_t *p_priv, u8 tuner_id)
{
  if(!p_priv->nc_tuner[tuner_id].dvbt2_switch_enable)
  {
    return ;
  }
  OS_PRINTF("%s,nim_type[%d]\n",__FUNCTION__,
    p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbt.nim_type);
  switch(p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbt.nim_type)
  {
    case NIM_DVBT:
      p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbt.nim_type = NIM_DVBT2;
      p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.nim_type = NIM_DVBT2;
      break;
    case NIM_DVBT2:
    default:
      p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbt.nim_type = NIM_DVBT;
      p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.nim_type = NIM_DVBT;
      break;
  }
}

static void dvb_switch_nimtype(nc_priv_data_t *p_priv, u8 tuner_id)
{
  switch(p_priv->nc_tuner[tuner_id].cur_lock_mode)
  { 
    case SYS_DVBS:
      dvbs_switch_nimtype(p_priv, tuner_id);
      break;
    case SYS_DVBT2:
      dvbt2_switch_nimtype(p_priv, tuner_id);
      break;
    default:
      return;
  }
}

static BOOL dvbs_lock_again(nc_priv_data_t *p_priv, u8 tuner_id)
{
  //static BOOL b_check_again = FALSE;

  if(!p_priv->nc_tuner[tuner_id].need_dvb_switch)
  {
    OS_PRINTF("%s,has checked again\n",__FUNCTION__);
    p_priv->nc_tuner[tuner_id].need_dvb_switch = TRUE;
    return FALSE;
  }
    
  if(p_priv->nc_tuner[tuner_id].dvb_s2_t2_support == TRUE)
  {
    nim_channel_info_t channel_info = {0};
    nim_channel_set_info_t set_info = {0};   

    dvb_switch_nimtype(p_priv, tuner_id);
    memcpy(&channel_info, &p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info,
      sizeof(nim_channel_info_t));
    OS_PRINTF("%s,nimtype[%d]\n",__FUNCTION__,
      p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbs.nim_type);
    nim_channel_set(p_priv->nc_tuner[tuner_id].p_nim_dev, &channel_info, &set_info);
    p_priv->nc_tuner[tuner_id].duration_ticks = set_info.lock_time / 10;  //200 ticks = 2s
    p_priv->nc_tuner[tuner_id].start_ticks = mtos_ticks_get();
    p_priv->nc_tuner[tuner_id].status = NC_CHECK_LOCK;
    p_priv->nc_tuner[tuner_id].need_dvb_switch = FALSE;
    return TRUE;
  }
  return FALSE;
}

static BOOL dvbt2_lock_again(nc_priv_data_t *p_priv, u8 tuner_id)
{
  if(!p_priv->nc_tuner[tuner_id].dvbt2_switch_enable)
  {
    return FALSE;
  }

  if(!p_priv->nc_tuner[tuner_id].need_dvb_switch)
  {
    OS_PRINTF("%s,has checked again\n",__FUNCTION__);
    p_priv->nc_tuner[tuner_id].need_dvb_switch = TRUE;
    return FALSE;
  }
    
  if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBT2)
  {
    nim_channel_info_t channel_info = {0};
    nim_channel_set_info_t set_info = {0};   

    dvb_switch_nimtype(p_priv, tuner_id);
    memcpy(&channel_info, &p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info,
      sizeof(nim_channel_info_t));
    OS_PRINTF("%s,nimtype[%d]\n",__FUNCTION__,
      p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbt.nim_type);
    nim_channel_set(p_priv->nc_tuner[tuner_id].p_nim_dev, &channel_info, &set_info);
    p_priv->nc_tuner[tuner_id].duration_ticks = set_info.lock_time / 10;  //200 ticks = 2s
    p_priv->nc_tuner[tuner_id].start_ticks = mtos_ticks_get();
    p_priv->nc_tuner[tuner_id].status = NC_CHECK_LOCK;
    p_priv->nc_tuner[tuner_id].need_dvb_switch = FALSE;
    return TRUE;
  }
  return FALSE;
}

static BOOL dvb_lock_again(nc_priv_data_t *p_priv, u8 tuner_id)
{
  switch(p_priv->nc_tuner[tuner_id].cur_lock_mode)
  {
    case SYS_DVBT2:
      return dvbt2_lock_again(p_priv,tuner_id);
    case SYS_DVBS:
      return dvbs_lock_again(p_priv,tuner_id);
    default:
      return FALSE;
  }
}

static void do_set_tp(class_handle_t handle, nc_channel_info_t *p_tp_info, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  nc_channel_info_t tp_info = {0};
  nim_channel_set_info_t set_info = {0};
  BOOL is_need_relock = FALSE;
  nim_ctrl_lock(p_priv,tuner_id);
  memcpy(&tp_info, p_tp_info, sizeof(nc_channel_info_t));
  nim_ctrl_unlock(p_priv,tuner_id);

  is_need_relock = tc_need_relock(p_priv, &tp_info, p_priv->nc_tuner[tuner_id].for_bs,tuner_id);
#ifdef PRINT_ON
  OS_PRINTF("is_need_relock[%d]\n",is_need_relock);
  print_tpinfo(p_priv->nc_tuner[tuner_id].cur_lock_mode, &tp_info);
#endif
  if(is_need_relock)
  {
    if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBS 
      || p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_ABSS)
    {
      if (p_priv->nc_tuner[tuner_id].lnb_check_enble == TRUE)
      {
        do_check_lnb(p_this,tuner_id);
      }
      //if current tp 22k on, close 22k
      //_set_22k(handle, 0, tuner_id);
        
      //if taget tp polar =! current tp polar, change polar
      _set_polarization(handle, tp_info.polarization, tuner_id);

      //reset diseqc according to tp info
      if (p_priv->nc_tuner[tuner_id].dis_info.is_fixed)
      {
        OS_PRINTF("move ahead 1\n");
        _set_diseqc(handle, &p_priv->nc_tuner[tuner_id].dis_info,tuner_id);
      }
      if(tp_info.onoff22k == 1)
        _set_22k(handle, 1, tuner_id);
      else
        _set_22k(handle, 0, tuner_id);
    }
    
    if(p_priv->nc_tuner[tuner_id].for_bs)
    {
      p_priv->nc_tuner[tuner_id].status = NC_DO_BS;
      if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBT2)
      {
        if( tp_info.channel_info.param.dvbt.nim_type == NIM_UNDEF)
        {
          tp_info.channel_info.param.dvbt.nim_type = NIM_DVBT2;//check T2 first
        }
        tp_info.channel_info.nim_type = tp_info.channel_info.param.dvbt.nim_type;
      }
      else if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DTMB)
      {
        if( tp_info.channel_info.param.dvbt.nim_type == NIM_UNDEF)
        {
          tp_info.channel_info.param.dvbt.nim_type = NIM_DTMB;
        }
        tp_info.channel_info.nim_type = tp_info.channel_info.param.dvbt.nim_type;
      }
      else if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBS)
      {
        //for dm6k
        tp_info.channel_info.nim_type  = tp_info.channel_info.param.dvbs.nim_type;
      }
    }
    else
    {
      if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBS)
      {

        if( tp_info.channel_info.param.dvbs.nim_type == NIM_UNDEF ||
          tp_info.channel_info.param.dvbs.nim_type == NIM_DVBS_AUTO)
        {
           tp_info.channel_info.param.dvbs.nim_type = NIM_DVBS;
        }
        //for dm6k
        tp_info.channel_info.nim_type  = tp_info.channel_info.param.dvbs.nim_type;
      }
      else if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBT2) 
      {
        if( tp_info.channel_info.param.dvbt.nim_type == NIM_UNDEF ||
          tp_info.channel_info.param.dvbt.nim_type == NIM_DVBT_AUTO)
        {
          tp_info.channel_info.param.dvbt.nim_type = p_priv->nc_tuner[tuner_id].dvbt_lock_mode;
          p_priv->nc_tuner[tuner_id].dvbt2_switch_enable = TRUE;
        }
        else
        {
          p_priv->nc_tuner[tuner_id].dvbt2_switch_enable = FALSE;
        }
        tp_info.channel_info.nim_type  = tp_info.channel_info.param.dvbt.nim_type;
      }
      else if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DTMB) 
      {
        if( tp_info.channel_info.param.dvbt.nim_type == NIM_UNDEF)
        {
          tp_info.channel_info.param.dvbt.nim_type = NIM_DTMB;
        }
        tp_info.channel_info.nim_type  = tp_info.channel_info.param.dvbt.nim_type;
      }
      else if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBC) 
      {
        //if( tp_info.channel_info.nim_type == NIM_UNDEF)
        {
          tp_info.channel_info.nim_type = NIM_DVBC;
        }
      }
      nim_channel_set(p_priv->nc_tuner[tuner_id].p_nim_dev, &(tp_info.channel_info), &set_info);
      p_priv->nc_tuner[tuner_id].duration_ticks = set_info.lock_time / 10;  //200 ticks = 2s
      p_priv->nc_tuner[tuner_id].start_ticks = mtos_ticks_get();
      p_priv->nc_tuner[tuner_id].status = NC_CHECK_LOCK;
    }
    memcpy(&p_priv->nc_tuner[tuner_id].cur_channel_info, &tp_info, sizeof(nc_channel_info_t));
    p_priv->nc_tuner[tuner_id].need_dvb_switch = TRUE;
  }
  //else
  //  p_priv->nc_tuner[tuner_id].status = NC_MONITOR;
  //p_priv->nc_tuner[tuner_id].monitor_enable = TRUE;
}


static void enter_monitor_mode(nc_priv_data_t *p_priv, u8 tuner_id)
{
  p_priv->nc_tuner[tuner_id].checklock_ticks = mtos_ticks_get();
  p_priv->nc_tuner[tuner_id].monitor_ticks = mtos_ticks_get();
  p_priv->nc_tuner[tuner_id].status = NC_MONITOR;
}

static void msg_proc(handle_t handle, os_msg_t *p_msg)
{
  service_t *p_svc = (service_t *)handle;
  nim_ctrl_svc_data_t *p_svc_data = p_svc->get_data_buffer(p_svc);
  nim_ctrl_t *p_this = p_svc_data->p_this;
  nc_priv_data_t *p_priv = p_this->p_data;

  switch(p_msg->content)
  {
    case NC_LOCK_TUNER0:
      p_priv->p_last_svc = p_svc;
      do_set_tp(p_this, (nc_channel_info_t *)p_msg->para1, 0);
      break;
    case NC_LOCK_TUNER1:
      p_priv->p_last_svc = p_svc;
      do_set_tp(p_this, (nc_channel_info_t *)p_msg->para1, 1);
      break;  
    default:
      MT_ASSERT(0);
      break;
  }
}


static void sm_proc_step(handle_t handle, u8 tuner_id)
{
  service_t *p_svc = (service_t *)handle;
  nim_ctrl_svc_data_t *p_svc_data = p_svc->get_data_buffer(p_svc);
  nim_ctrl_t *p_this = p_svc_data->p_this;
  nc_priv_data_t *p_priv = p_this->p_data;
  u32 cur_ticks = 0;
  u8 is_lock = 0;
  nc_evt_t evt = NC_EVT_BEGIN;
  u8 param = 0;

  cur_ticks = mtos_ticks_get();

  if(p_priv->nc_tuner[tuner_id].p_nim_dev == NULL)
     return;
 
  switch(p_priv->nc_tuner[tuner_id].status)
  {
    case NC_IDLE:
      if(p_priv->nc_tuner[tuner_id].lnb_check_enble == TRUE)
      {
        dev_io_ctrl(_get_spec_nim_dev(p_this, tuner_id), NIM_IOCTRL_CHECK_LNB_SC_PROT,
            (u32)&param);
        // check lnb short
        if(param == NIM_LNB_SC_PROTING && 
            ((cur_ticks - p_priv->nc_tuner[tuner_id].last_ticks) >= 300))
        {
          OS_PRINTF("lnb short \n");
          do_check_lnb(p_this, tuner_id);
          p_priv->nc_tuner[tuner_id].last_ticks = cur_ticks;
       }
      }
      break;
    case NC_MONITOR:
      if(p_priv->nc_tuner[tuner_id].monitor_enable)
      {
        if((cur_ticks - p_priv->nc_tuner[tuner_id].checklock_ticks) > 
            MAX_CHECK_LOCK_TICKS_NUM)
        {
          dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_CHECK_LOCK,
            (u32)&is_lock);
          if (is_lock != p_priv->nc_tuner[tuner_id].tuner_locking)
          {
            p_priv->nc_tuner[tuner_id].tuner_locking = is_lock;
            p_priv->nc_tuner[tuner_id].monitor_ticks = mtos_ticks_get();
          }

          if(!p_priv->nc_tuner[tuner_id].tuner_locking) //tuner unlocked
          {
            if((cur_ticks - p_priv->nc_tuner[tuner_id].monitor_ticks) > 
                p_priv->nc_tuner[tuner_id].maxticks_cnk)  //pre 2s  /150
            {
              nim_channel_info_t channel_info = {0};
              nim_channel_set_info_t set_info = {0};

              OS_PRINTF("###nc_monitor\n");
              dvb_switch_nimtype(p_priv,tuner_id);
              memcpy(&channel_info, &p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info,
                sizeof(nim_channel_info_t));
#ifdef PRINT_ON
              print_tpinfo(p_priv->nc_tuner[tuner_id].cur_lock_mode, &p_priv->nc_tuner[tuner_id].cur_channel_info);
#endif
              if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBS 
                || p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_ABSS)
              {

                OS_PRINTF("move ahead 2\n");
                //Reset 22k
                //if(p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k)
                  //dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF, 0);
                //_set_22k(p_this, p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k,tuner_id);
                //Reset dis
                _set_diseqc(p_this, &p_priv->nc_tuner[tuner_id].dis_info,tuner_id);
                //if(p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k == 1)
                  //dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF, 1);
              }
              nim_channel_set(p_priv->nc_tuner[tuner_id].p_nim_dev, &channel_info, &set_info);
              if(p_priv->nc_tuner[tuner_id].auto_IQ)
              {
                dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_CHANGE_IQ,
                  (u32)&channel_info);
                if(p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.spectral_polar == 0)
                  p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.spectral_polar = 1;
                else
                  p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.spectral_polar = 0;
              }
              p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.lock = channel_info.lock;
              p_priv->nc_tuner[tuner_id].dis_need_reset = TRUE;
              p_priv->nc_tuner[tuner_id].monitor_ticks = cur_ticks;
            }
          }
          else
          {
            if(p_priv->nc_tuner[tuner_id].dis_need_reset == TRUE)
            {
              //##6171 reset diseqc to avoid using diseqc default port
              //Reset 22k
              OS_PRINTF("remove dis_need_reset\n");
              //_set_22k(p_this, p_priv->cur_channel_info.onoff22k);
              //Reset dis
              //_set_diseqc(p_this, &p_priv->dis_info);

              p_priv->nc_tuner[tuner_id].dis_need_reset = FALSE;
            }
          }
          p_priv->nc_tuner[tuner_id].checklock_ticks = cur_ticks;
        }
      }

      if(p_priv->nc_tuner[tuner_id].lnb_check_enble == TRUE)
      {
        dev_io_ctrl(_get_spec_nim_dev(p_this, tuner_id), NIM_IOCTRL_CHECK_LNB_SC_PROT, 
            (u32)&param);
        // check lnb short
        if(param == NIM_LNB_SC_PROTING && 
            ((cur_ticks - p_priv->nc_tuner[tuner_id].last_ticks) >= 300))
        {
          OS_PRINTF("lnb short \n");
          do_check_lnb(p_this, tuner_id);
          p_priv->nc_tuner[tuner_id].last_ticks = cur_ticks;
       }
      }
      break;
    case NC_DO_BS:
      //call the sync api
      nim_channel_connect(p_priv->nc_tuner[tuner_id].p_nim_dev,
        &(p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info), TRUE);
      g_active_tuner = tuner_id;
      p_priv->nc_tuner[tuner_id].tuner_locking = 
        p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.lock;
      evt = (p_priv->nc_tuner[tuner_id].tuner_locking) ? NC_EVT_LOCKED : NC_EVT_UNLOCKED;
      send_lock_result(p_priv, evt, tuner_id);
      p_priv->nc_tuner[tuner_id].status = NC_IDLE;
      break;
    case NC_CHECK_LOCK:
      {
        BOOL locked = FALSE;
        dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_CHECK_LOCK, 
            (u32)&locked);

        if(locked) //tuner locked
        {
          p_priv->nc_tuner[tuner_id].tuner_locking = TRUE;
          if(p_priv->nc_tuner[tuner_id].cur_lock_mode == SYS_DVBT2) 
          {
            dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_SET_CHANNEL_INFO, 
              p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.param.dvbt.plp_id);
          }
          send_lock_result(p_priv, NC_EVT_LOCKED, tuner_id);
          enter_monitor_mode(p_priv, tuner_id);
        }
        else
        {
          cur_ticks = mtos_ticks_get();
          if(p_priv->nc_tuner[tuner_id].duration_ticks 
            < (cur_ticks - p_priv->nc_tuner[tuner_id].start_ticks))
          {          
            if(p_priv->nc_tuner[tuner_id].auto_IQ)
            {
              nim_channel_info_t channel_info = {0};
              memcpy(&channel_info, &p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info,
                sizeof(nim_channel_info_t));

              dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_CHANGE_IQ, 
                (u32)&channel_info);
              if(p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.spectral_polar == 0)
                p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.spectral_polar = 1;
              else
                p_priv->nc_tuner[tuner_id].cur_channel_info.channel_info.spectral_polar = 0;
              //p_priv->duration_ticks = 10;  //wait 100ms
              p_priv->nc_tuner[tuner_id].start_ticks = mtos_ticks_get();
              p_priv->nc_tuner[tuner_id].status = NC_WAIT_IQ_CHANGE;
            }
            else
            {
              p_priv->nc_tuner[tuner_id].tuner_locking = FALSE;
              if(dvb_lock_again(p_priv, tuner_id))
              {
                break;
              }
              send_lock_result(p_priv, NC_EVT_UNLOCKED, tuner_id);
              enter_monitor_mode(p_priv, tuner_id);
            }
          }
        }
      }
      break;
    case NC_WAIT_IQ_CHANGE:
      {
        BOOL locked = FALSE;
        dev_io_ctrl(p_priv->nc_tuner[tuner_id].p_nim_dev, NIM_IOCTRL_CHECK_LOCK, (u32)&locked);

        if(locked) //tuner locked
        {
          p_priv->nc_tuner[tuner_id].tuner_locking = TRUE;
          send_lock_result(p_priv, NC_EVT_LOCKED, tuner_id);
          OS_PRINTF("NC_EVT_LOCKED 33\n");
          enter_monitor_mode(p_priv, tuner_id);
        }
        else
        {
          cur_ticks = mtos_ticks_get();
          if(p_priv->nc_tuner[tuner_id].wait_ticks < 
            (cur_ticks - p_priv->nc_tuner[tuner_id].start_ticks))
          {
            p_priv->nc_tuner[tuner_id].tuner_locking = FALSE;
            if(dvb_lock_again(p_priv, tuner_id))
            {
              break;
            }
            send_lock_result(p_priv, NC_EVT_UNLOCKED, tuner_id);
            enter_monitor_mode(p_priv, tuner_id);
          }
        }
      }
      break;
    default:
      MT_ASSERT(0);
      break;
  }
}

static void sm_proc(handle_t handle)
{
    service_t *p_svc = (service_t *)handle;
    nim_ctrl_svc_data_t *p_svc_data = p_svc->get_data_buffer(p_svc);
    nim_ctrl_t *p_this = p_svc_data->p_this;
    nc_priv_data_t *p_priv = p_this->p_data;
    if((p_priv->nc_tuner[1].p_nim_dev == p_priv->nc_tuner[0].p_nim_dev) && (p_priv->nc_tuner[1].p_nim_dev!= NULL))
    {
        sm_proc_step(handle, g_active_tuner);
    }
    else
    {
    sm_proc_step(handle, TUNER0);
    sm_proc_step(handle, TUNER1);
    }
}

static void on_decorate_cmd(handle_t handle, os_msg_t *p_msg)
{
  p_msg->is_ext = ((NC_LOCK_TUNER0 == p_msg->content) 
    ||(NC_LOCK_TUNER1 == p_msg->content));
}

service_t * nc_get_svc_instance(class_handle_t handle, u32 context)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  service_t *p_svc = NULL;

  p_svc = p_priv->p_container->get_svc(p_priv->p_container, context);
  if(NULL == p_svc)
  {
    nim_ctrl_svc_data_t *p_svc_data = NULL;

    p_svc = construct_svc(context);
    p_svc->sm_proc = sm_proc;
    p_svc->msg_proc = msg_proc;
    p_svc->on_decorate_cmd = on_decorate_cmd;
    p_svc->allocate_data_buffer(p_svc, sizeof(nim_ctrl_svc_data_t));
    p_svc_data = (nim_ctrl_svc_data_t *)p_svc->get_data_buffer(p_svc);

    p_svc_data->p_this = p_this;
    p_priv->p_container->add_svc(p_priv->p_container, p_svc);
  }
  return p_svc;
}

void nc_remove_svc_instance(class_handle_t handle, service_t *p_svc)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  p_priv->p_container->remove_svc(p_priv->p_container, p_svc);
}


static void _set_tp(class_handle_t handle, void *p_extend,
                  nc_channel_info_t *p_tp_info, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  service_t *p_svc = p_extend;
  u32 cmd_id = (tuner_id == TUNER0) ? NC_LOCK_TUNER0 : NC_LOCK_TUNER1;

  if(p_priv->nc_tuner[tuner_id].is_sync_lock)
  {
    p_priv->p_last_svc = p_svc;
    do_set_tp(p_this, p_tp_info,tuner_id);
  }
  else
  {
    //fix me. just fot nim_ctrl 2010-09-8
    //how about other service's message??? --edbert @2013-9-27
    mtos_messageq_clr(p_priv->msgq_id);
    //p_priv->nc_tuner[tuner_id].status = NC_IDLE;
    p_svc->do_cmd(p_svc, cmd_id, (u32)p_tp_info, sizeof(nc_channel_info_t));
  }
}

static void _get_tp(class_handle_t handle, nc_channel_info_t *p_current_info, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  MT_ASSERT(p_current_info != NULL);

  memcpy(p_current_info, &(p_priv->nc_tuner[tuner_id].cur_channel_info), sizeof(nc_channel_info_t));
}

/*!
  Get the lock status of the tunner
  \param[in] handle: nim ctrl handle
  */
static BOOL _get_lock_status(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  return p_priv->nc_tuner[tuner_id].tuner_locking;
}

static BOOL _is_finish_locking(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  return (BOOL)(p_priv->nc_tuner[tuner_id].status == NC_MONITOR);
}



/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[out] p_perf: channel performance
  */
static void _perf_get(class_handle_t handle, nim_channel_perf_t *p_perf, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  nim_channel_perf(p_priv->nc_tuner[tuner_id].p_nim_dev, p_perf);
}

static void _set_blind_scan_mode(class_handle_t handle, BOOL bs, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  p_priv->nc_tuner[tuner_id].for_bs = bs;
  p_priv->nc_tuner[tuner_id].monitor_enable = !bs;
}

static void _enable_monitor(class_handle_t handle, BOOL enable, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  p_priv->nc_tuner[tuner_id].monitor_enable = enable;
  if(enable)
    p_priv->nc_tuner[tuner_id].status = NC_MONITOR;
}

static void _enable_lnb_check(class_handle_t handle, BOOL enable, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  p_priv->nc_tuner[tuner_id].lnb_check_enble = enable;
}

static BOOL _is_lnb_check_enable(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  return p_priv->nc_tuner[tuner_id].lnb_check_enble;
}

static void _disable_set_diseqc(class_handle_t handle, BOOL flag, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  p_priv->nc_tuner[tuner_id].diseqc_disable = flag;
}

static void _set_polarization(class_handle_t handle, nim_lnb_porlar_t polar, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  OS_PRINTF("$$$$ %s, pol %d\n", __FUNCTION__, polar);
  if(p_priv->nc_tuner[tuner_id].cur_channel_info.polarization != polar)
  {
    p_priv->nc_tuner[tuner_id].cur_channel_info.polarization = polar;
    dev_io_ctrl(_get_spec_nim_dev(p_this, tuner_id), NIM_IOCTRL_SET_PORLAR, polar);
  }
}

static void _set_22k(class_handle_t handle, u8 onoff22k, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none 22k \n");
    return ;
  }

  OS_PRINTF("$$$$ %s, 22k %d\n", __FUNCTION__, onoff22k);
  if(p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k != onoff22k)
  {
    p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k = onoff22k;
    dev_io_ctrl(_get_spec_nim_dev(p_this, tuner_id), NIM_IOCTRL_SET_22K_ONOFF, onoff22k);
  }
}

/*!
  Set 12v onoff
  \param[in] handle for nc
  \param[in] onoff12v 1 is on and 0 is off
  */
static void _set_12v(class_handle_t handle, u8 onoff12v, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none 12v \n");
    return;
  }


  if(p_priv->nc_tuner[tuner_id].onoff12v != onoff12v)
  {
    p_priv->nc_tuner[tuner_id].onoff12v = onoff12v;
    dev_io_ctrl(_get_spec_nim_dev(p_this, tuner_id), NIM_IOCTRL_SET_12V_ONOFF, onoff12v);
    //OS_PRINTF("---> nc set 12v is %s\n", onoff12v ? "ON" : "OFF");
  }
}

static BOOL _tone_burst(class_handle_t handle, nim_diseqc_mode_t mode, u8 tuner_id)
{
  //nim_ctrl_t *p_this = handle;
  //nc_priv_data_t *p_priv = p_this->p_data;
  ///???how do burst
  //return (SUCCESS == rs2k_tone_burst(mode))
  return TRUE;
}

static void _diseqc_ctrl(class_handle_t handle,
  nim_diseqc_cmd_t *p_diseqc_cmd, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  u8 param = 0;
  nim_device_t *p_spec_nim_dev = _get_spec_nim_dev(p_this, tuner_id);

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return;
  }


  if(p_priv->nc_tuner[tuner_id].lnb_check_enble == FALSE)
  {
    nim_ctrl_lock(p_priv, tuner_id);     // temp modify ,fix me
    nim_diseqc_ctrl(p_spec_nim_dev, p_diseqc_cmd);
    nim_ctrl_unlock(p_priv, tuner_id);  // temp modify ,fix me
  }
  else
  {
    //to detect
    dev_io_ctrl(p_spec_nim_dev, NIM_IOCTRL_CHECK_LNB_SC_PROT, (u32)&param);

    if((param == NIM_LNB_SC_NO_PROTING) || (param == 0))//param == 0, not support lnb protect
    {
      //remove protect.
      dev_io_ctrl(p_spec_nim_dev, NIM_REMOVE_PROTECT, 0);

      //nim diseqc control
      nim_ctrl_lock(p_priv, tuner_id);     // temp modify ,fix me
      nim_diseqc_ctrl(p_spec_nim_dev, p_diseqc_cmd);
      nim_ctrl_unlock(p_priv, tuner_id);  // temp modify ,fix me
      mtos_task_delay_ms(100);

      //reset
      dev_io_ctrl(p_spec_nim_dev, NIM_ENABLE_CHECK_PROTECT, 0);
      p_priv->nc_tuner[tuner_id].last_ticks = mtos_ticks_get();
    }
  }
}

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] diseqc_level: the level of DiSEqC protocol this switch used
  \param[in] mode: switch control mode, applied in DiSEqC x.1
  \param[in] port: switch port to be selected, start form 0
  */
static void _switch_ctrl(class_handle_t handle,
        nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  nim_diseqc_cmd_t diseqc_cmd = {0};
  u8 buf[4] = {0};
  u8 i = 0;
  u8 repeat_times = 1;
  u8 is_22k_on = 0;
  u8 pol_status = 0;

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return;
  }
  diseqc_cmd.mode = NIM_DISEQC_BYTES;
  diseqc_cmd.tx_len = 4;
  diseqc_cmd.p_tx_buf = buf;

  buf[0] = 0xE0;
  buf[1] = 0x10;

  is_22k_on = p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k;
  if(is_22k_on != 1)
  {
    is_22k_on = 0;
  }

  if(p_priv->nc_tuner[tuner_id].cur_channel_info.polarization == NIM_PORLAR_HORIZONTAL)
  {
    pol_status = 1;
  }
  else
  {
    pol_status = 0;
  }

  switch(diseqc_level)
  {
    case NIM_DISEQC_LEVEL_X0:
      MT_ASSERT(port < 4);
      /* X.0, only support committed switch */
      //OS_PRINTF("port [%d], pol_status [%d], is_22k_on[%d]\n",
       // port, pol_status, is_22k_on);
      buf[2] = 0x38;
      buf[3] = 0xF0 + port * 4 + pol_status * 2 + is_22k_on;
      //OS_PRINTF("Command content %x\n", buf[3]);
      break;

    default:
      /* level above X.0, support uncommitted switch */
      MT_ASSERT(port < 16);
      /* uncommitted switch */
      buf[2] = 0x39;
      buf[3] = 0xF0 + port;
      if(mode != RSCMD_SWITCH_TWO_REPEATS)
      {
        repeat_times = 2;
      }
      else
      {
        repeat_times = 3;
      }
      break;
  }

  for(i = 0; i < repeat_times; i++)
  {
    //delay 20ms
    mtos_task_delay_ms(20);
    _diseqc_ctrl(handle, &diseqc_cmd,tuner_id);
    //delay 20ms
    mtos_task_delay_ms(20);
  }
}

/*!
  DiSEqC control for positioner
  \param[in] handle: nim ctrl handle
  \param[in] cmd: the positioner cmd to DiSEqC
  \param[in] param: cmd with paramter
  */
static void _positioner_ctrl(class_handle_t handle,
      rscmd_positer_t cmd, u32 param, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  nim_diseqc_cmd_t msg = {0};
  u8 tx_buf[8] = {0};
  u16 mode = param >> 16;
  u8 data = (u8)param;
//  u16 degree_decimal = 0;
//  u8 degree = 0;
//  u8 degree_dec_tbl[5] = {0x00, 0x02, 0x03, 0x05, 0x06};

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return;
  }

  if(p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k == 1)
  {
    _set_22k(handle, 0, tuner_id);
    mtos_task_delay_ms(20);
  }

  dev_io_ctrl(_get_spec_nim_dev(p_this, tuner_id), NIM_IOCTRL_DISEQC1X, 0);

  msg.mode = NIM_DISEQC_BYTES;
  msg.p_tx_buf = tx_buf;
  msg.tx_len = 4;

  msg.p_tx_buf[0] = 0xE0;
  msg.p_tx_buf[1] = 0x31;
  msg.p_tx_buf[2] = cmd;

  switch(cmd)
  {
    case RSCMD_PSTER_STOP:
    case RSCMD_PSTER_LIMIT_OFF:
    case RSCMD_PSTER_LIMIT_E:
    case RSCMD_PSTER_LIMIT_W:
      msg.tx_len = 3;
      break;

    case RSCMD_PSTER_DRV_E:
    case RSCMD_PSTER_DRV_W:
      if(mode == RSMOD_PSTER_DRV_CONTINUE)
      {
        msg.p_tx_buf[3] = 0;
      }
      else if(mode == RSMOD_PSTER_DRV_TIMEOUT)
      {
        /* time out is 1 ~ 127 s */
        msg.p_tx_buf[3] = data & 0x7F;
      }
      else if(mode == RSMOD_PSTER_DRV_STEPS)
      {
        /* step is 1 ~ 128, data_send is 0x80 ~ 0xFF */
        msg.p_tx_buf[3] = (~data) + 1;
      }
      else
      {
        /* default use continuesly mode */
        msg.p_tx_buf[3] = 0;
      }
      break;

    case RSCMD_PSTER_RECALCULATE:
      //msg.p_tx_buf[3] = data;
      msg.p_tx_buf[3] = p_priv->nc_tuner[tuner_id].nim_port;
      break;

    case RSCMD_PSTER_GOTO_NN:
      msg.p_tx_buf[3] = data;

//      if(data != p_priv->nim_port)
      {
        p_priv->nc_tuner[tuner_id].nim_port = data;
        // notify driving positioner
        send_evt(p_priv, NC_EVT_POS_TURNING, tuner_id);
      }
      break;
case RSCMD_PSTER_STORE_NN:
      msg.p_tx_buf[3] = data;
      p_priv->nc_tuner[tuner_id].nim_port = data;
      break;
    case RSCMD_PSTER_GOTO_ANG:
      msg.tx_len = 5;
#if 0
      degree = param >> 16;
      degree_decimal = (u8)param;
      if(degree_decimal > 9)
      {
        degree_decimal = 9;
      }
      msg.p_tx_buf[4] |= (degree / 256) << 4;
      degree %= 256;
      msg.p_tx_buf[4] |= (degree / 16);
      degree %= 16;
      msg.p_tx_buf[5] |= degree << 4;
      msg.p_tx_buf[5] |= (degree_decimal < 5) ?
        degree_dec_tbl[degree_decimal] :
        (0x08 + degree_dec_tbl[degree_decimal - 5]);
#else
      if(param == 0)
      {
        msg.p_tx_buf[3] = 0xE0;
        msg.p_tx_buf[4] = 0x00;
      }
      else
      {
        msg.p_tx_buf[3] = (param >> 8) & 0xFF;
        msg.p_tx_buf[4] = (param & 0xFF);
      }
      // notify driving positioner
      send_evt(p_priv, NC_EVT_POS_TURNING,tuner_id);
#endif
      break;

    default:
      msg.p_tx_buf[3] = data;
  }

  _diseqc_ctrl(handle, &msg,tuner_id);

  if(p_priv->nc_tuner[tuner_id].cur_channel_info.onoff22k == 1)
  {
    mtos_task_delay_ms(50);
    _set_22k(handle, 1, tuner_id);
  }
  mtos_task_delay_ms(20);
}

static BOOL _usals_ctrl(class_handle_t handle, double sat_longitude,
        double local_longitude, double local_latitude, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  u16 param = 0;
  double degree = 0;

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return FALSE;
  }

  degree = usals_calc_degree(sat_longitude,
                             local_longitude, local_latitude);
  if(degree > 90)
  {
    // notify driving positioner
    send_evt(p_priv, NC_EVT_POS_OUT_RANGE, tuner_id);
    return TRUE;
  }
  else
  {
    param = usals_convert_degree(degree);
    nc_positioner_ctrl(handle, tuner_id, RSCMD_PSTER_GOTO_ANG, param); 
    return FALSE;
  }
}

#ifdef PRINT_ON
static void print_tpinfo(sys_signal_t lock_mode, nc_channel_info_t *p_tp_info)
{
  switch(lock_mode)
  {
    case SYS_DVBS:
      OS_PRINTF("dvbs tc lock tuner freq %d, sym %d, polar %d, pol %d, nim type[%d], 22K[%d]\n",
        p_tp_info->channel_info.frequency,
        p_tp_info->channel_info.param.dvbs.symbol_rate,
        p_tp_info->polarization,
        p_tp_info->channel_info.spectral_polar,
        p_tp_info->channel_info.param.dvbs.nim_type, p_tp_info->onoff22k);
      break;
    case SYS_DVBT2:
      OS_PRINTF("dvbt tc lock tuner freq %d, band_width %d, nim type[%d]\n",
        p_tp_info->channel_info.frequency,
        p_tp_info->channel_info.param.dvbt.band_width,
        p_tp_info->channel_info.param.dvbt.nim_type);
      break;
    case SYS_DVBC:
      OS_PRINTF("dvbc tc lock tuner freq %d, modulation %d, symbol_rate %d\n",
        p_tp_info->channel_info.frequency,
        p_tp_info->channel_info.param.dvbc.modulation,
        p_tp_info->channel_info.param.dvbc.symbol_rate);
      break;
    case SYS_DTMB:
      OS_PRINTF("dtmb tc lock tuner freq %d, band_width %d, nim type[%d]\n",
        p_tp_info->channel_info.frequency,
        p_tp_info->channel_info.param.dvbt.band_width,
        p_tp_info->channel_info.param.dvbt.nim_type);
    default:
      OS_PRINTF("%s, Please check lock mode[%d]\n",__FUNCTION__, lock_mode);
      break;
  }
}

static void print_diseqcinfo(nc_diseqc_info_t *p_diseqc, u8 tuner_id)
{
  OS_PRINTF("\n $$$$ %s\n",__FUNCTION__);
  OS_PRINTF("is_fixed=%d\n",tuner_id);
  OS_PRINTF("is_fixed=%d\n",p_diseqc->is_fixed);
  OS_PRINTF("position_type=%d\n",p_diseqc->position_type);
  OS_PRINTF("position=%d\n",p_diseqc->position);
  OS_PRINTF("diseqc_type_1_0=%d\n",p_diseqc->diseqc_type_1_0);
  OS_PRINTF("diseqc_port_1_0=%d\n",p_diseqc->diseqc_port_1_0);
  OS_PRINTF("diseqc_type_1_1=%d\n",p_diseqc->diseqc_type_1_1);
  OS_PRINTF("diseqc_port_1_1=%d\n",p_diseqc->diseqc_port_1_1);
  OS_PRINTF("used_DiSEqC12=%d\n",p_diseqc->used_DiSEqC12);
  OS_PRINTF("d_sat_longitude=%f\n",p_diseqc->d_sat_longitude);
  OS_PRINTF("d_local_longitude=%f\n",p_diseqc->d_local_longitude);
  OS_PRINTF("d_local_latitude=%f\n",p_diseqc->d_local_latitude);
  OS_PRINTF("diseqc_1_1_mode=%d\n",p_diseqc->diseqc_1_1_mode);
}
#endif

/*!
  Set  diseqc info
  \param[in] handle for nc service
  \param[in] polar nim_lnb_porlar_t mode
  */
static BOOL _set_diseqc(class_handle_t handle, nc_diseqc_info_t *p_diseqc, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  rscmd_switch_mode_t mode = 0;
  u8 port = 0;
  BOOL is_out_of_range = FALSE;
  BOOL ret = FALSE;

#ifdef PRINT_ON
  print_diseqcinfo(p_diseqc, tuner_id);
#endif

  if(p_priv->nc_tuner[tuner_id].cur_lock_mode != SYS_DVBS)
  {
    OS_PRINTF("warning:: not dvbs mode. have none desiqc \n");
    return ret;
  }

  //Record last disq information
  p_priv->nc_tuner[tuner_id].dis_info.is_fixed = p_diseqc->is_fixed;

  //if(p_diseqc->is_fixed)
  {
    if(p_diseqc->diseqc_type_1_1 != 0)
    {
      //Record disq 1.1 info
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_type_1_1 = p_diseqc->diseqc_type_1_1;

      port = p_diseqc->diseqc_port_1_1;
      switch (p_diseqc->diseqc_type_1_1)
      {
        case 1: // no repeat
          mode = RSCMD_SWITCH_NO_REPEAT;
          break;
        case 2: // one repeat
          mode = RSCMD_SWITCH_ONE_REPEAT;
          break;
        default: // two repeat
          mode = RSCMD_SWITCH_TWO_REPEATS;
      }
      if(p_diseqc->diseqc_1_1_mode != 0)
      {
        p_priv->nc_tuner[tuner_id].dis_info.diseqc_1_1_mode = p_diseqc->diseqc_1_1_mode;

        port &= 0x03;
        port = port << (p_diseqc->diseqc_1_1_mode - 1);
      }

      p_priv->nc_tuner[tuner_id].dis_info.diseqc_port_1_1 = port;
      _switch_ctrl(handle, NIM_DISEQC_LEVEL_X1, mode, port,tuner_id);
      ret = TRUE;
    }
    else
    {
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_type_1_1 = 0;
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_1_1_mode = 0;
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_port_1_1 = 0;
    }

    if(p_diseqc->diseqc_type_1_0 != 0)
    {
      _switch_ctrl(handle, NIM_DISEQC_LEVEL_X0, 0,
        (u8)p_diseqc->diseqc_port_1_0,tuner_id);

      //Record Disq info
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_type_1_0 = p_diseqc->diseqc_type_1_0;
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_port_1_0 = p_diseqc->diseqc_port_1_0;
      ret = TRUE;
    }
    else
    {
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_type_1_0 = 0;
      p_priv->nc_tuner[tuner_id].dis_info.diseqc_port_1_0 = 0;
    }
  }
  //else
  if(!p_diseqc->is_fixed)
  {
    p_priv->nc_tuner[tuner_id].dis_info.used_DiSEqC12 = p_diseqc->used_DiSEqC12;
    if(p_diseqc->used_DiSEqC12) // DiSEqC 1.2
    {
      if(p_diseqc->position_type != 0)
      {
        if(p_priv->nc_tuner[tuner_id].dis_info.position != p_diseqc->position
           || p_diseqc ->position_change)

        {
          // bug 41215
          p_priv->nc_tuner[tuner_id].dis_info.position_type = p_diseqc->position_type;
          p_priv->nc_tuner[tuner_id].dis_info.position = p_diseqc->position;
          
          if(p_this->positioner_ctrl != NULL)
          {
            p_this->positioner_ctrl(handle, RSCMD_PSTER_GOTO_NN, p_diseqc->position, tuner_id);
          }

          //clean info for USALS
          p_priv->nc_tuner[tuner_id].dis_info.d_sat_longitude = 0;
          p_priv->nc_tuner[tuner_id].dis_info.d_local_latitude = 0;
          p_priv->nc_tuner[tuner_id].dis_info.d_local_longitude = 0;

          //return TRUE;
          ret = TRUE;
        }
      }
      else
      {
        // clean info
        p_priv->nc_tuner[tuner_id].dis_info.position_type = 0;
        p_priv->nc_tuner[tuner_id].dis_info.position = 0;

      }
    }
    else // USALS
    {
      if((p_priv->nc_tuner[tuner_id].dis_info.d_sat_longitude != p_diseqc->d_sat_longitude)
        || (p_priv->nc_tuner[tuner_id].dis_info.d_local_longitude != p_diseqc->d_local_longitude)
        || (p_priv->nc_tuner[tuner_id].dis_info.d_local_latitude != p_diseqc->d_local_latitude))
      {
        //_usals_ctrl(handle,p_diseqc->d_sat_longitude,
        //  p_diseqc->d_local_longitude, p_diseqc->d_local_latitude);
        if(p_this->usals_ctrl != NULL)
        {
            is_out_of_range = p_this->usals_ctrl(handle,p_diseqc->d_sat_longitude,
            p_diseqc->d_local_longitude, p_diseqc->d_local_latitude,tuner_id);
        }

         //clean info for DiSEqC 1.2
       if(!is_out_of_range)
       {
        p_priv->nc_tuner[tuner_id].dis_info.position_type = 0;
        p_priv->nc_tuner[tuner_id].dis_info.position = 0;

       }

        p_priv->nc_tuner[tuner_id].dis_info.d_sat_longitude = p_diseqc->d_sat_longitude;
        p_priv->nc_tuner[tuner_id].dis_info.d_local_latitude = p_diseqc->d_local_latitude;
        p_priv->nc_tuner[tuner_id].dis_info.d_local_longitude = p_diseqc->d_local_longitude;
        //return TRUE;
        ret = TRUE;
      }
    }
  }

  return ret;
}

/*!
  copy  diseqc info
  \param[in] handle for nc service
  */
static void _cpy_diseqc_info(class_handle_t handle, nc_diseqc_info_t *p_diseqc, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  memcpy(&p_priv->nc_tuner[tuner_id].dis_info, p_diseqc, sizeof(nc_diseqc_info_t));
}

/*!
  Set  diseqc info
  \param[in] handle for nc service
  */
static void _clr_diseqc_info(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  memset(&p_priv->nc_tuner[tuner_id].dis_info, 0, sizeof(nc_diseqc_info_t));
}

/*!
  clear  current channel tp info
  \param[in] handle for nc service
  */
static void _clr_tp_info(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  _init_nc_channel_info(&p_priv->nc_tuner[tuner_id].cur_channel_info,tuner_id);
}


/*!
  for blind scan v2, avoid do lock and check lock at the same time
  \param[in] handle for nc service
  */
static void _enter_monitor_idle(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;
  _init_nc_channel_info(&p_priv->nc_tuner[tuner_id].cur_channel_info,tuner_id);
  p_priv->nc_tuner[tuner_id].status = NC_IDLE;
}


/*!
  config the relation between TUNER & TS IN
  \param[in] handle for nc service 
  \param[in] tuner id
    \param[in] TS ID
  \param[in] tuner id
    \param[in] TS ID 
  */
static void _set_tuner_ts(class_handle_t handle, u8 tuner_0, u8 ts_0, u8 tuner_1, u8 ts_1)
{
   nim_ctrl_t *p_this = handle;
   nc_priv_data_t *p_priv = p_this->p_data;

   p_priv->nc_tuner[0].tuner_id = tuner_0;
   p_priv->nc_tuner[0].ts_in = ts_0;
   p_priv->nc_tuner[1].tuner_id = tuner_1;
   p_priv->nc_tuner[1].ts_in = ts_1;   
}

/*!
  get the ts index by tuner id
  \param[in] handle for nc service 
  \param[in] tuner id
  */
static void _get_ts_by_tuner(class_handle_t handle, u8 tuner_id, u8 *p_ts_in)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;  
  MT_ASSERT(p_this != NULL);
  //MT_ASSERT(p_this->enter_monitor_idle != NULL);
  *p_ts_in = p_priv->nc_tuner[tuner_id].ts_in;
}

/*!
  get the main tuner index
  \param[in] handle for nc service 
  \param[out] tuner id
  */
static void _get_main_tuner_ts(class_handle_t handle, u8 *p_tuner_id, u8 *p_ts_in)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;  
  MT_ASSERT(p_this != NULL);
  //MT_ASSERT(p_this->enter_monitor_idle != NULL);
  *p_tuner_id = p_priv->nc_tuner[0].tuner_id;
  *p_ts_in = p_priv->nc_tuner[0].ts_in;
}

/*!
  get tuner count 
  \param[in] handle for nc service 
  \param[out] none
  */
u32 _get_tuner_count(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;  
  MT_ASSERT(p_this != NULL);
  return p_priv->tuner_num;
}
/*!
  get nim dev handle by tuner indext 
  \param[in] handle for nc service, tuner index
  \param[out] nim dev handle
  */
u32 _get_nim_handle_by_tuner(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;  
  MT_ASSERT(p_this != NULL);
  return (u32)p_priv->nc_tuner[tuner_id].p_nim_dev;
}

/*!
  get the ts index by tuner id
  \param[in] handle for nc service 
  \param[in] tuner id
  */
static void _set_lock_mode(class_handle_t handle, u8 tuner_id, sys_signal_t lock_mode)
{
  nim_ctrl_t *p_this = handle;
  nc_priv_data_t *p_priv = p_this->p_data;  
  MT_ASSERT(p_this != NULL);
  OS_PRINTF("_set_lock_mode[%d,%d]\n",tuner_id,lock_mode);
  p_priv->nc_tuner[tuner_id].cur_lock_mode = lock_mode;
  p_priv->nc_tuner[tuner_id].cur_channel_info.polarization = 0xFF;
}

static void _init_nc_channel_info(nc_channel_info_t *p_channel_info, u8 tuner_id)
{
  memset(p_channel_info, 0, sizeof(nc_channel_info_t));
  p_channel_info->onoff22k = 0xFF; //to invalid
  p_channel_info->polarization = 0xFF; //to invalid
}

nim_device_t * _get_spec_nim_dev(nim_ctrl_t *p_this, u8 tuner_id)
{
  nc_priv_data_t *p_priv = NULL;  
  MT_ASSERT(p_this != NULL);

  p_priv = p_this->p_data;
  MT_ASSERT(p_priv != NULL);

  if(p_priv->p_spec_nim_dev == NULL)
    return p_priv->nc_tuner[tuner_id].p_nim_dev;
  else
    return p_priv->p_spec_nim_dev;
}

handle_t nc_svc_init(nc_svc_cfg_t *p_cfg_0, nc_svc_cfg_t *p_cfg_1)
{
  nim_ctrl_t *p_nim_ctrl = mtos_malloc(sizeof(nim_ctrl_t));
  nc_priv_data_t *p_priv = mtos_malloc(sizeof(nc_priv_data_t));
  u32 attach_size = sizeof(nc_channel_info_t) * NC_SVC_Q_DEPTH;
  void *p_attach_buffer = mtos_malloc(attach_size);
  svc_container_t *p_container = construct_svc_container("nim_ctrl",
    p_cfg_0->priority, p_cfg_0->stack_size, NC_SVC_Q_DEPTH);

  MT_ASSERT(p_nim_ctrl != NULL);
  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_attach_buffer != NULL);

  memset(p_priv, 0, sizeof(nc_priv_data_t));
  memset(p_nim_ctrl, 0, sizeof(nim_ctrl_t));
  p_nim_ctrl->p_data = p_priv;
  p_priv->p_container = p_container;
  p_priv->msgq_id = p_container->get_msgq_id(p_container);  
  p_priv->tuner_num = 1;

  //init tuner 0 paramter
  OS_PRINTF("tuner 0 init edbert\n");
  p_priv->nc_tuner[0].tuner_id = TUNER0;
  p_priv->nc_tuner[0].cur_channel_info.onoff22k = 0xFF; //to invalid
  p_priv->nc_tuner[0].cur_channel_info.polarization = 0xFF; //to invalid
  p_priv->nc_tuner[0].nim_port = 0xFF;
  p_priv->nc_tuner[0].is_sync_lock = p_cfg_0->b_sync_lock;
  p_priv->nc_tuner[0].lock_mode = p_cfg_0->lock_mode;
  p_priv->nc_tuner[0].cur_lock_mode = p_cfg_0->lock_mode;
  p_priv->nc_tuner[0].auto_IQ = p_cfg_0->auto_iq;
  p_priv->nc_tuner[0].dvb_s2_t2_support = p_cfg_0->dvbs_2_support;
  if(p_cfg_0->dvbt_lock_mode)
    p_priv->nc_tuner[0].dvbt_lock_mode = p_cfg_0->dvbt_lock_mode;
  else
    p_priv->nc_tuner[0].dvbt_lock_mode = NIM_DVBT2;
  OS_PRINTF("%s,support dvbs_2[%d]\n",__FUNCTION__,p_cfg_0->dvbs_2_support);

  p_priv->nc_tuner[0].mutex_lock_mode = p_cfg_0->mutex_type;
  p_priv->nc_tuner[0].p_mutex = mtos_mutex_create(1);

  p_priv->nc_tuner[0].checklock_ticks = 0;

  if (p_cfg_0->wait_ticks != 0)
  {
    p_priv->nc_tuner[0].wait_ticks =  p_cfg_0->wait_ticks;
  }
  else
  {
    p_priv->nc_tuner[0].wait_ticks = 10;
  }
  if (p_cfg_0->maxticks_cnk != 0)
  {
    p_priv->nc_tuner[0].maxticks_cnk = p_cfg_0->maxticks_cnk;
  }
  else
  {
    p_priv->nc_tuner[0].maxticks_cnk = 150;
  }

  p_priv->nc_tuner[0].p_nim_dev = (nim_device_t *)dev_find_identifier(NULL, 
    DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(p_priv->nc_tuner[0].p_nim_dev != NULL);
  p_priv->nc_tuner[0].tuner_locking = TRUE;
  if(p_cfg_0->b_used_spec)
    p_priv->p_spec_nim_dev = p_priv->nc_tuner[0].p_nim_dev;

  //init tuner 1 paramter
  if(p_cfg_1 != NULL)
  {
    OS_PRINTF("tuner 1 init edbert\n");
    p_priv->tuner_num = 2;
    p_priv->nc_tuner[1].tuner_id = TUNER1;
    p_priv->nc_tuner[1].cur_channel_info.onoff22k = 0xFF; //to invalid
    p_priv->nc_tuner[1].cur_channel_info.polarization = 0xFF; //to invalid
    p_priv->nc_tuner[1].nim_port = 0xFF;
    p_priv->nc_tuner[1].is_sync_lock = p_cfg_1->b_sync_lock;
    p_priv->nc_tuner[1].lock_mode = p_cfg_1->lock_mode;
    p_priv->nc_tuner[1].cur_lock_mode = p_cfg_1->lock_mode;
    p_priv->nc_tuner[1].auto_IQ = p_cfg_1->auto_iq;
    p_priv->nc_tuner[1].dvb_s2_t2_support = p_cfg_1->dvbs_2_support;
    if(p_cfg_1->dvbt_lock_mode)
      p_priv->nc_tuner[1].dvbt_lock_mode = p_cfg_1->dvbt_lock_mode;
    else
      p_priv->nc_tuner[1].dvbt_lock_mode = NIM_DVBT2;
    OS_PRINTF("%s,support dvbs_2[%d]\n",__FUNCTION__,p_cfg_1->dvbs_2_support);

    p_priv->nc_tuner[1].mutex_lock_mode = p_cfg_1->mutex_type;
    p_priv->nc_tuner[1].p_mutex = mtos_mutex_create(1);

    p_priv->nc_tuner[1].checklock_ticks = 0;

    if (p_cfg_1->wait_ticks != 0)
    {
        p_priv->nc_tuner[1].wait_ticks =  p_cfg_1->wait_ticks;
    }
    else
    {
        p_priv->nc_tuner[1].wait_ticks = 10;
    }
    if (p_cfg_1->maxticks_cnk != 0)
    {
        p_priv->nc_tuner[1].maxticks_cnk = p_cfg_1->maxticks_cnk;
    }
    else
    {
        p_priv->nc_tuner[1].maxticks_cnk = 150;
    }
#ifndef WIN32
    {
      p_priv->nc_tuner[1].p_nim_dev = (nim_device_t *)dev_find_identifier(
        p_priv->nc_tuner[0].p_nim_dev , DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
      MT_ASSERT(p_priv->nc_tuner[1].p_nim_dev != NULL);
      p_priv->nc_tuner[1].tuner_locking = TRUE;
    }
#else
    {
      p_priv->nc_tuner[1].p_nim_dev = p_priv->nc_tuner[0].p_nim_dev;
      MT_ASSERT(p_priv->nc_tuner[1].p_nim_dev != NULL);
      p_priv->nc_tuner[1].tuner_locking = TRUE;
    }
#endif
    if(p_cfg_1->b_used_spec)
      p_priv->p_spec_nim_dev = p_priv->nc_tuner[1].p_nim_dev;
  }
  
  //init function
  p_nim_ctrl->set_tp = _set_tp;
  p_nim_ctrl->get_tp = _get_tp;
  p_nim_ctrl->get_lock_status = _get_lock_status;
  p_nim_ctrl->perf_get = _perf_get;
  p_nim_ctrl->set_blind_scan_mode = _set_blind_scan_mode;
  p_nim_ctrl->enable_monitor = _enable_monitor;
  p_nim_ctrl->is_finish_locking = _is_finish_locking;
  p_nim_ctrl->set_tuner_ts = _set_tuner_ts;
  p_nim_ctrl->get_ts_by_tuner = _get_ts_by_tuner;
  p_nim_ctrl->get_main_tuner_ts = _get_main_tuner_ts;
  p_nim_ctrl->get_tuner_count = _get_tuner_count;
  p_nim_ctrl->get_nim_handle_by_tuner = _get_nim_handle_by_tuner;
  p_nim_ctrl->set_lock_mode = _set_lock_mode;
  if(SYS_DVBC !=  p_priv->nc_tuner[0].cur_lock_mode)
  {
    p_nim_ctrl->lnb_check_enable = _enable_lnb_check;
    p_nim_ctrl->is_lnb_check_enable = _is_lnb_check_enable;
    p_nim_ctrl->disable_set_diseqc = _disable_set_diseqc;
    p_nim_ctrl->set_polarization = _set_polarization;
    p_nim_ctrl->set_22k = _set_22k;
    p_nim_ctrl->set_12v = _set_12v;
    p_nim_ctrl->tone_burst = _tone_burst;
    p_nim_ctrl->switch_ctrl = _switch_ctrl;
    p_nim_ctrl->positioner_ctrl = _positioner_ctrl;
    p_nim_ctrl->usals_ctrl = _usals_ctrl;
    p_nim_ctrl->set_diseqc = _set_diseqc;
    p_nim_ctrl->cpy_diseqc_info = _cpy_diseqc_info;
    p_nim_ctrl->clr_diseqc_info = _clr_diseqc_info;
    p_nim_ctrl->clr_tp_info = _clr_tp_info;
    p_nim_ctrl->enter_monitor_idle = _enter_monitor_idle;
  }
  //attach extern size
  memset(p_attach_buffer, 0, attach_size);
  mtos_messageq_attach(p_priv->msgq_id, p_attach_buffer,
    sizeof(nc_channel_info_t), NC_SVC_Q_DEPTH);

  class_register(NC_CLASS_ID, p_nim_ctrl);

  //tuner 0  reset diseqc
  _switch_ctrl(p_nim_ctrl, NIM_DISEQC_LEVEL_X0, 0, 0, TUNER0);
  //tuner 1  reset diseqc
  if(p_priv->nc_tuner[1].p_nim_dev != NULL)
     _switch_ctrl(p_nim_ctrl, NIM_DISEQC_LEVEL_X0, 0, 0, TUNER1);
  return p_nim_ctrl;
}

handle_t nc_svc_init_without_diseqc12(nc_svc_cfg_t *p_cfg_0, nc_svc_cfg_t *p_cfg_1)
{
  nim_ctrl_t *p_nim_ctrl = mtos_malloc(sizeof(nim_ctrl_t));
  nc_priv_data_t *p_priv = mtos_malloc(sizeof(nc_priv_data_t));
  u32 attach_size = sizeof(nc_channel_info_t) * NC_SVC_Q_DEPTH;
  void *p_attach_buffer = mtos_malloc(attach_size);
  svc_container_t *p_container = construct_svc_container("nim_ctrl",
    p_cfg_0->priority, p_cfg_0->stack_size, NC_SVC_Q_DEPTH);

  MT_ASSERT(p_nim_ctrl != NULL);
  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_attach_buffer != NULL);

  memset(p_priv, 0, sizeof(nc_priv_data_t));
  p_nim_ctrl->p_data = p_priv;
  p_priv->p_container = p_container;
  p_priv->tuner_num = 1;
  
  //init tuner 0 paramter
  p_priv->nc_tuner[0].tuner_id = TUNER0;
  p_priv->nc_tuner[0].cur_channel_info.onoff22k = 0xFF; //to invalid
  p_priv->nc_tuner[0].cur_channel_info.polarization = 0xFF; //to invalid
  p_priv->nc_tuner[0].nim_port = 0xFF;
  p_priv->nc_tuner[0].is_sync_lock = p_cfg_0->b_sync_lock;
  p_priv->nc_tuner[0].lock_mode = p_cfg_0->lock_mode;
  p_priv->nc_tuner[0].cur_lock_mode = p_cfg_0->lock_mode;
  p_priv->nc_tuner[0].checklock_ticks = 0;

  if (p_cfg_0->wait_ticks != 0)
  {
    p_priv->nc_tuner[0].wait_ticks =  p_cfg_0->wait_ticks;
  }
  else
  {
    p_priv->nc_tuner[0].wait_ticks = 10;
  }

  if (p_cfg_0->maxticks_cnk != 0)
  {
    p_priv->nc_tuner[0].maxticks_cnk = p_cfg_0->maxticks_cnk;
  }
  else
  {
    p_priv->nc_tuner[0].maxticks_cnk = 150;
  }
  p_priv->msgq_id = p_container->get_msgq_id(p_container);
  p_priv->nc_tuner[0].p_nim_dev = (nim_device_t *)dev_find_identifier(
    NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);

  //init tuner 1 paramter
  if(p_cfg_1 != NULL)
  {
   p_priv->tuner_num = 2;
    p_priv->nc_tuner[1].tuner_id = TUNER1;
    p_priv->nc_tuner[1].cur_channel_info.onoff22k = 0xFF; //to invalid
    p_priv->nc_tuner[1].cur_channel_info.polarization = 0xFF; //to invalid
    p_priv->nc_tuner[1].nim_port = 0xFF;
    //hacked, sync has a bug.
    //p_priv->nc_tuner[1].is_sync_lock = p_cfg_1->b_sync_lock;
    p_priv->nc_tuner[1].is_sync_lock = FALSE;
    p_priv->nc_tuner[1].lock_mode = p_cfg_1->lock_mode;
    p_priv->nc_tuner[1].cur_lock_mode = p_cfg_1->lock_mode;
    p_priv->nc_tuner[1].checklock_ticks = 0;

    if (p_cfg_1->wait_ticks != 0)
    {
      p_priv->nc_tuner[1].wait_ticks =  p_cfg_1->wait_ticks;
    }
    else
    {
      p_priv->nc_tuner[1].wait_ticks = 10;
    }

    if (p_cfg_1->maxticks_cnk != 0)
    {
      p_priv->nc_tuner[1].maxticks_cnk = p_cfg_1->maxticks_cnk;
    }
    else
    {
      p_priv->nc_tuner[1].maxticks_cnk = 150;
    }
    p_priv->msgq_id = p_container->get_msgq_id(p_container);
#ifndef WIN32
    {
      p_priv->nc_tuner[1].p_nim_dev = (nim_device_t *)dev_find_identifier(
      p_priv->nc_tuner[0].p_nim_dev , DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    }
#else
      p_priv->nc_tuner[1].p_nim_dev = p_priv->nc_tuner[0].p_nim_dev;
#endif
  }
  //init function
  p_nim_ctrl->set_tp = _set_tp;
  p_nim_ctrl->get_tp = _get_tp;
  p_nim_ctrl->get_lock_status = _get_lock_status;
  p_nim_ctrl->perf_get = _perf_get;
  p_nim_ctrl->set_blind_scan_mode = _set_blind_scan_mode;
  p_nim_ctrl->enable_monitor = _enable_monitor;
  p_nim_ctrl->lnb_check_enable = _enable_lnb_check;
  p_nim_ctrl->is_lnb_check_enable = _is_lnb_check_enable;
  p_nim_ctrl->set_polarization = _set_polarization;
  p_nim_ctrl->set_22k = _set_22k;
  p_nim_ctrl->set_12v = _set_12v;
  p_nim_ctrl->tone_burst = NULL;//_tone_burst;
  p_nim_ctrl->switch_ctrl = _switch_ctrl;
  p_nim_ctrl->positioner_ctrl = NULL;//_positioner_ctrl;
  p_nim_ctrl->usals_ctrl = NULL;//_usals_ctrl;
  p_nim_ctrl->set_diseqc = _set_diseqc;
  p_nim_ctrl->clr_diseqc_info = _clr_diseqc_info;
  p_nim_ctrl->is_finish_locking = _is_finish_locking;
  p_nim_ctrl->set_tuner_ts = _set_tuner_ts;
  p_nim_ctrl->get_ts_by_tuner = _get_ts_by_tuner;
  p_nim_ctrl->set_lock_mode = _set_lock_mode;
  
  //attach extern size
  memset(p_attach_buffer, 0, attach_size);
  mtos_messageq_attach(p_priv->msgq_id, p_attach_buffer,
    sizeof(nc_channel_info_t), NC_SVC_Q_DEPTH);

  class_register(NC_CLASS_ID, p_nim_ctrl);

  // reset tuner 0 diseqc
  _switch_ctrl(p_nim_ctrl, NIM_DISEQC_LEVEL_X0, 0, 0, 0);
  //tuner 1  reset diseqc
  if(p_priv->nc_tuner[1].p_nim_dev != NULL)
     _switch_ctrl(p_nim_ctrl, NIM_DISEQC_LEVEL_X0, 0, 0,1);
  return p_nim_ctrl;
}
