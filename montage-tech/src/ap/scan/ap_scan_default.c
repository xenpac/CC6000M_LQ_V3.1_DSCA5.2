/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"

#include "lib_util.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_misc.h"

#include "drv_dev.h"
#include "class_factory.h"

#include "nim.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "class_factory.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "mosaic.h"
#include "dvb_protocol.h"
#include "nit.h"
#include "pmt.h"
#include "cat.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"

#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_scan_priv.h"
#include "ap_dvbs_scan.h"



static RET_CODE default_on_new_sat(scan_sat_info_t *p_sat_info)
{
  event_t evt = {0};
  evt.id = SCAN_EVT_SAT_SWITCH;
  // evt.data1 = (u32)&cur_tp;
  // evt.data2 = p_tp->can_locked;
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  return SUCCESS;
}

static u32 default_get_switch_disq_time(scan_sat_info_t *p_sat_info)
{
  return (0 * 1000);  //0 s
}

static RET_CODE default_check_tp(nc_channel_info_t *p_channel)
{
  if(p_channel->channel_info.param.dvbs.symbol_rate < 2000)
  {
    return ERR_FAILURE;
  }
  return SUCCESS;

}

static RET_CODE default_check_tp_dtmb(nc_channel_info_t *p_channel)
{
  return SUCCESS;
}

static RET_CODE default_process_tp(scan_tp_info_i_t *p_tp)
{
  event_t evt = {0};

/*  OS_PRINTF("Found new TP: freq[%d], symrate[%d], pol[%d], 22k[%d]\n",
    p_tp->tp_info.freq,
    p_tp->tp_info.sym,
    p_tp->tp_info.polarity,
    p_tp->tp_info.is_on22k);
*/
  evt.id = SCAN_EVT_TP_FOUND;
 // evt.data1 = (u32)&cur_tp;
 // evt.data2 = p_tp->can_locked;
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);

  return SUCCESS;
}

static BOOL match_pid(scan_pg_info_t *p_pg_info)
{
  u32 audio_cnt = 0; 
  scan_input_param_t *p_input_priv = NULL;
  p_input_priv = (scan_input_param_t *)class_get_handle_by_id(SCAN_IMPL_CLASS_ID);


  if(p_input_priv->pid_parm.pcr_pid == p_pg_info->pcr_pid
    && p_input_priv->pid_parm.video_pid == p_pg_info->video_pid)
  {
    for(audio_cnt = 0; audio_cnt < p_pg_info->audio_ch_num; audio_cnt++)
    {
      if(p_pg_info->audio[audio_cnt].p_id == p_input_priv->pid_parm.audio_pid)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}


/*!
  Process tp information when one tp is done
  \param[in] p_tp_list
  */
static RET_CODE default_process_pg_list(scan_tp_info_i_t *p_tp)
{
  
  u16 i = 0;
  event_t evt = {SCAN_EVT_PG_FOUND};
  scan_input_param_t *p_input_priv = NULL;
  p_input_priv = (scan_input_param_t *)class_get_handle_by_id(SCAN_IMPL_CLASS_ID);

  for(i = 0; i < p_tp->pg_num; i++)
  {
    scan_pg_info_t *p_pg = p_tp->p_pg_list + i;
    OS_PRINTF("\tPG[%d]: v_pid[%d], a_pid1[%d], pcr[%d], is_scr%d, is_scr_found %d, name[%s]\n",
      i, p_pg->video_pid,p_pg->audio[0].p_id,p_pg->pcr_pid, p_pg->is_scrambled,
      p_pg->is_scramble_found, p_pg->name);

    if(0 == p_pg->video_pid && 0 == p_pg->audio_ch_num)
    {
      continue; //it's dummy pg
    }

    //Check scramble status
    if(p_input_priv->is_free_only && p_pg->is_scrambled)
    {
      continue;
    }

    if(CHAN_TV == p_input_priv->chan_type
      && 0 == p_pg->video_pid)
    {
      continue;
    }

    else if(CHAN_RADIO == p_input_priv->chan_type
      && p_pg->video_pid != 0)
    {
      continue;
    }

    //Check pid parameter
    if(p_input_priv->pid_scan_enable && !match_pid(p_pg))
    {
      continue;
    }


   // evt.data1 = (u32)p_pg;
    ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  }

  return SUCCESS;
}

static RET_CODE default_free_resource(void)
{
 // OS_PRINTF("Scan Finished\n");
  return SUCCESS;
}

static RET_CODE default_on_start(void)
{
 // OS_PRINTF("Scan Finished\n");
  return SUCCESS;
}


static void * default_get_attach_buffer(u32 size)
{
  void *p_addr = NULL;
  
  return p_addr;
}

/*static void default_performance_hook(perf_check_point_t check, u32 context1, u32 context2)
{
  if(check == PERF_PAT_REQUEST)
  {
    OS_PRINTF("pat request start point\n");
  }
  else if(check == PERF_PAT_FOUND)
  {
    OS_PRINTF("pat found point\n");
  }
}
*/
//void ap_scan_start(void)
handle_t ap_scan_start_default(scan_input_param_t *p_input)  //for blind
{
  class_handle_t p_handle = class_get_handle_by_id(SCAN_IMPL_CLASS_ID);
  scan_input_param_t *p_input_priv = NULL;

  scan_hook_t hook = {0};

  hook.on_new_sat = default_on_new_sat;
  hook.get_switch_disq_time = default_get_switch_disq_time;
  hook.check_tp = default_check_tp;
  hook.process_tp = default_process_tp;
  hook.process_pg_list = default_process_pg_list;
  hook.free_resource = default_free_resource;
  hook.get_attach_buffer = default_get_attach_buffer;
  hook.on_start = default_on_start;
  //hook.performance_hook = default_performance_hook;

  if(p_handle == NULL)
  {
    p_input_priv = mtos_malloc(sizeof(scan_input_param_t));
    class_register(SCAN_IMPL_CLASS_ID, (class_handle_t)p_input_priv);
  }
  p_input_priv = (scan_input_param_t *)class_get_handle_by_id(SCAN_IMPL_CLASS_ID);
  memset(p_input_priv,0,sizeof(scan_input_param_t));
  memcpy(p_input_priv,p_input,sizeof(scan_input_param_t));
  //for blind. it's error. Notify
  if(p_input->scan_mode == BLIND_SCAN)
  {
    return construct_bl_scan_policy(&hook, &p_input->scan_data);
  }
  else if(p_input->scan_mode == TP_SCAN)
  {
    return construct_tp_scan_policy(&hook, &p_input->scan_data);
  }
  else if(p_input->scan_mode == DTMB_SCAN)
  {
    hook.check_tp = default_check_tp_dtmb;
    return construct_dtmb_scan_policy(&hook, &p_input->scan_data);
  }
  else if(p_input->scan_mode == DVBC_SCAN)
  {
    return construct_dvbc_scan_policy(&hook, &p_input->scan_data);
  }
  else
  {
    return NULL;
  }
}

