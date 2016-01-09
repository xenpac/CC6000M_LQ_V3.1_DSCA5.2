/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"
#include "hal_misc.h"

#include "common.h"
#include "drv_dev.h"
#include "dmx.h"
#include "vdec.h"
//#include "video.h"
#include "aud_vsb.h"
#include "drv_misc.h"
#include "scart.h"
#include "rf.h"
#include "nim.h"
//#include "sub.h"
#include "avsync.h"
#include "lib_rect.h"
#include "display.h"

#include "lib_util.h"
#include "class_factory.h"
#include "mdl.h"
#include "err_check_def.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "avctrl1.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "sdt.h"
#include "nit.h"
#include "dvb_svc.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"


#include "dvbc_util.h"
#include "subtitle_api.h"
#include "vbi_api.h"


#include "ap_framework.h"
#include "ap_playback.h"
#include "ap_playback_i.h"
#include "ap_playback_priv.h"

#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_change_pg_imp.h"
#include "ad_api.h"
#include "ap_time.h"
#include "ap_epg4.h"

#ifdef PRINT_ON
#define PB_DEBUG
#endif

#ifdef PB_DEBUG
#define PB_DBG OS_PRINTF
#else
#define PB_DBG(x) do{}while(0)
#endif

#define SWITCHPG

#define CHANGE_PERF
#ifdef CHANGE_PERF
#define PB_PERF OS_PRINTF
 u32 g_stop_point = 0;
#else
#define PB_PERF(...) do{}while(0)
#endif

/*!
  using set interval for auto pmt
  */
#define PAT_MONITOR_INTERVAL 200

#define NIT_MONITOR_INTERVAL 500

#define SCART_VCR_DETECT_CNT 4

#define INVAILD_CHANNEL_ID 0xFFFF

/*!
  playback status 
  */
typedef enum
{
  /*!
    status machine idle 
    */
  PB_SM_IDLE = 0,
  /*!
    wait lock
    */
  PB_SM_WAIT_LOCK,
  /*!
    status machine waiting_video done 
    */
  PB_SM_WAIT_VIDEO,
  /*!
    status machine monitor 
    */
  PB_SM_MONITOR_SIGNAL,
  /*!
    Wait stop
    */
  PB_SM_WAIT_STOP
} pb_sm_t;

/*!
  playback private data 
  */
typedef struct
{
  /*!
    current status parameter
    */
  pb_status_t status;
  /*!
    status machine
    */
  pb_sm_t sm;
  /*!
    flag waiting omt
    */
  BOOL is_waiting_pmt;
  /*!
    start monitor pat flag
    */
  u32 pat_monitor_start;
  /*!
    interval monitor pat value
    */
  u16 pat_monitor_interval;
  /*!
    pat request flag
    */
  u16 pat_request_flag;
  /*!
    start monitor nit flag
    */
  u32 nit_monitor_start;
  /*!
    nit request flag
    */
  u16 nit_request_flag;
  /*!
    start monitor signal
    */
  u32 signal_monitor_start;
  /*!
    PAT pending job
    */
  u8 pat_pending_job;
  /*!
    PMT pending job
    */
  u8 pmt_pending_job;
  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;
  /*!
    NIM service handle
    */
  service_t *p_nim_svc;
  /*!
    ttx data
    */
  void *p_sub_ttx_data;
  /*!
    play params
    */
  play_param_t play_param;
  /*!
    start ticks for wait video frame cnt
    */
  u32 start_tick;
  /*!
    fix me
    */
  BOOL is_scart_vcr_detected;
  /*!
    audio start cnt
    */
  u32 audio_start_tick;
  /*!
    fix me
    */
  BOOL is_sw_audio_trace;
  /*!
    fix me
    */
  u8 repeat_time;
  /*!
    search info
    */
  nc_channel_info_t search_info;
  /*!
    audio dev handle
    */
  aud_device_vsb_t *p_audio_dev;
  /*!
    video dev handle
    */
  void *p_video_dev;
  /*!
    display dev handle
    */
  void *p_disp_dev;
  /*!
    dmx dev handle
    */
  dmx_device_t *p_dmx_dev;
  /*!
    vpid channel hanle
    */
  dmx_chanid_t v_chanid;    
  /*!
    apid channel hanle
    */
  dmx_chanid_t a_chanid;
  /*!
    ppid channel hanle
    */    
  dmx_chanid_t p_chanid;    
#ifndef SWITCHPG
  u16 last_vpid;
  u16 last_apid;
  u16 last_ppid;
#else
  u32 last_tp;
#endif

    /*!
    ts in style
    */   
  u8 ts_in;
  u8 tuner_id;
  BOOL audio_enable_done;
} priv_data_t;

/*!
  playback handle 
  */
typedef struct
{
  /*!
    playback instance
    */
  app_t instance;
  /*!
    policy
    */
  pb_policy_t *p_policy;
  /*!
    private data
    */
  priv_data_t data;
} pb_handle_t;

static void init_play_param(pb_handle_t *p_handle,play_param_t *p_src)
{
  play_param_t *p_dest = &(p_handle->data.play_param);

  //first cp ui para
  memcpy(p_dest, p_src, sizeof(play_param_t));
  //init para switch
  if(p_handle->p_policy->init_play != NULL)
    p_handle->p_policy->init_play(p_dest);
  else
  {
    p_dest->is_do_sdt = TRUE;
    p_dest->is_do_nit = TRUE;
    p_dest->is_do_dynamic_pid = TRUE;
    p_dest->start_mode = PB_START_STABLE;
  }
}

/*!
  send pb's evt out.

  \param[in] evt_t current evt
  \param[in] data1 parameter 1
  \param[in] data2 parameter 2
  */
static void send_evt_to_ui(pb_evt_t evt_t, u32 data1, u32 data2)
{
  event_t evt;
  evt.id = evt_t;
  evt.data1 = (u32)data1;
  evt.data2 = (u32)data2;
  ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);
}

static void do_play_chan_open(pb_handle_t *p_handle, play_param_t *p_param, u8 ts_in, 
    u8 force_ts_change, u8 ts_new)
{
  priv_data_t *p_data = &p_handle->data;
  dvbs_program_t *p_pg_info = &p_param->pg_info;
  RET_CODE ret = SUCCESS;
  dmx_play_setting_t  play_para;
  u8 stream_in = force_ts_change ? ts_new :  ts_in; 

  OS_PRINTF("open play chan:  vpid =%d , apid = %d, ts=%d\n", 
                    (u16)p_pg_info->v_pid, (u16)p_pg_info->a_pid, ts_in);  

  //set pid
  play_para.pid = (u16)p_pg_info->a_pid;
  play_para.type = DMX_AUDIO_TYPE;
  play_para.stream_in = stream_in;
  ret = dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &(p_data->a_chanid));
  CHECK_FAIL_RET_VOID(SUCCESS == ret);

  play_para.pid = (u16)p_pg_info->pcr_pid;
  play_para.type = DMX_PCR_TYPE;
  play_para.stream_in =  stream_in;
  ret = dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &(p_data->p_chanid));
  CHECK_FAIL_RET_VOID(SUCCESS == ret);

  play_para.pid = (u16)p_pg_info->v_pid;
  play_para.type = DMX_VIDEO_TYPE;
  play_para.stream_in =  stream_in;
  ret = dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &(p_data->v_chanid));
  CHECK_FAIL_RET_VOID(SUCCESS == ret);
}

static void set_audio_on(priv_data_t *p_data)
{ 
  //不支持audio自适应功能，则不需要设置
  if(FALSE == p_data->play_param.audio_type_adjust)
  {
    OS_PRINTF("Not support audio type adjust\n");
    return;
  }

  //保证取到正确的audio type后只设置一次，PMT会不停抓表
  if(p_data->audio_enable_done)
  {
    OS_PRINTF("Set audio done\n");
    return;
  }
  
  //如果开始播放节目设置了静音，这里打开声音
  if(avc_is_mute_1(class_get_handle_by_id(AVC_CLASS_ID)) == FALSE)
  {
    OS_PRINTF("mute disable 2 \n");
    aud_mute_onoff_vsb(p_data->p_audio_dev,FALSE);
  }

  //置上变量，保证后面不再设置，在切台时会清标志
  p_data->audio_enable_done = TRUE;
}
static void do_dynamic_pid(pb_handle_t *p_handle, pmt_t *p_pmt,u8 ts_in)
{
  priv_data_t *p_data = &p_handle->data;
  dvbs_program_t *p_pg_info = &p_data->play_param.pg_info;
  
  u16 new_video_pid = 0;
  u16 new_pcr_pid   = 0;
  u16 new_audio_pid = 0;
  u16 new_audio_type = 0;
  BOOL pid_changed = FALSE;
  BOOL ret = FALSE;
  //dmx_chanid_t p_chan_handle = 0;
  dmx_play_setting_t play_para = {0};
  video_param_t video_cfg = {0};
  audio_param_t audio_cfg = {0};
  av_start_type_t   start_type = 0;
  
  if(!p_data->play_param.is_do_dynamic_pid)
    return;
  if(p_pg_info->s_id != p_pmt->prog_num)
  {
    //it not current pg's PMT
    return;
  }
  /*
  OS_PRINTF("PMT update: v_pid[%d], pcr_pid[%d], audio_cnt[%d],"
    "a_pid_0[%d], a_pid_1[%d], a_pid_2[%d]\n",
    p_pmt->video_pid, p_pmt->pcr_pid,p_pmt->audio_count,
    p_pmt->audio[0].p_id, p_pmt->audio[1].p_id, p_pmt->audio[2].p_id);
  */

  ret = p_handle->p_policy->process_pmt_info(p_handle->p_policy->p_data,
        p_pmt, &new_video_pid, &new_pcr_pid, &new_audio_pid, &new_audio_type);
  if(!ret)
  {
    set_audio_on(p_data);
    return;
  }
#if 0  
  //video pid
  if(p_pg_info->v_pid != new_video_pid
    || p_pg_info->audio_type != p_pmt->video_type)
  {
    play_para.pid = new_video_pid;
    play_para.type = DMX_VIDEO_TYPE;
    //pti_dev_set_video_pid(p_data->p_dmx_dev, new_video_pid);
    dmx_chan_stop(p_data->p_dmx_dev, p_data->v_chanid);
    dmx_chan_close(p_data->p_dmx_dev, p_data->v_chanid);
    dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &p_data->v_chanid);
    dmx_chan_start(p_data->p_dmx_dev, p_data->v_chanid);
    vdec_stop(p_data->p_video_dev);
    vdec_start(p_data->p_video_dev, p_pmt->video_type, VID_UNBLANK_STABLE);

    p_pg_info->v_pid = new_video_pid;
    p_pg_info->video_type = p_pmt->video_type;
    pid_changed = TRUE;
    OS_PRINTF("dynamic pid: change v_pid to %d,type to %d\n", 
      new_video_pid,p_pmt->video_type);
  }

  //prc pid
  if(p_pg_info->pcr_pid != new_pcr_pid)
  {
    play_para.pid = new_pcr_pid;
    play_para.type = DMX_PCR_TYPE;
    //pti_dev_set_pcr_pid(p_data->p_dmx_dev, new_pcr_pid);
    dmx_chan_stop(p_data->p_dmx_dev, p_data->p_chanid);
    dmx_chan_close(p_data->p_dmx_dev, p_data->p_chanid);
    dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &p_data->p_chanid);
    dmx_chan_start(p_data->p_dmx_dev, p_data->p_chanid);

    p_pg_info->pcr_pid = new_pcr_pid;
    pid_changed = TRUE;
    OS_PRINTF("dynamic pid: change pcr_pid to %d\n", new_pcr_pid);
  }
  
  //audio pid
  if(p_pg_info->a_pid != new_audio_pid
    || p_pg_info->audio_type != new_audio_type)
  {
    play_para.pid = new_audio_pid;
    play_para.type = DMX_AUDIO_TYPE;
    //pti_dev_set_audio_pid(p_data->p_dmx_dev, new_audio_pid);
    dmx_chan_stop(p_data->p_dmx_dev, p_data->a_chanid);
    dmx_chan_close(p_data->p_dmx_dev, p_data->a_chanid);
    dmx_play_chan_open(p_data->p_dmx_dev, &play_para,&p_data->a_chanid);
    dmx_chan_start(p_data->p_dmx_dev, p_data->a_chanid);
    aud_stop_vsb(p_data->p_audio_dev);
    aud_start_vsb(p_data->p_audio_dev, new_audio_type, AUDIO_NO_FILE);

    p_pg_info->a_pid = new_audio_pid;
    p_pg_info->audio_type = new_audio_type;
    pid_changed = TRUE;
    OS_PRINTF("dynamic pid: change a_pid to %d, type %d \n",
      new_audio_pid, new_audio_type);
  }
#else // minnan debug it. we must use this flow to stop start . because this is warriors!!!

    if(p_pg_info->pcr_pid == new_pcr_pid
      && p_pg_info->v_pid == new_video_pid
      && p_pg_info->a_pid ==  new_audio_pid
      && p_pg_info->audio_type == new_audio_type)
    {
      return;
    }
  if(p_data->status == PB_PLAYING)
  {
      ret = avc_av_stop(class_get_handle_by_id(AVC_CLASS_ID));
      dmx_chan_stop(p_data->p_dmx_dev, p_data->v_chanid);
      dmx_chan_close(p_data->p_dmx_dev, p_data->v_chanid);
      
      dmx_chan_stop(p_data->p_dmx_dev, p_data->a_chanid);
      dmx_chan_close(p_data->p_dmx_dev, p_data->a_chanid);
      
      dmx_chan_stop(p_data->p_dmx_dev, p_data->p_chanid);
      dmx_chan_close(p_data->p_dmx_dev, p_data->p_chanid);
      dmx_av_reset(p_data->p_dmx_dev);

      video_cfg.format = p_pmt->video_type; 
      video_cfg.mode = VID_UNBLANK_STABLE;
      video_cfg.file_play = FALSE;

      if((p_pg_info->v_pid) && (p_pg_info->v_pid != 0x1FFF))
      {
        start_type = AV_VIDEO_START; 
      }
      if((p_pg_info->a_pid) && (p_pg_info->a_pid != 0x1FFF))
      {
        if(start_type == AV_VIDEO_START)
          start_type = AV_START_ALL; 
        else
          start_type = AV_AUDIO_START;
      }
      audio_cfg.type = new_audio_type;
      audio_cfg.file_type = AUDIO_NO_FILE;
      OS_PRINTF("\ndo_dynamic_pid  start :%d tick=%d\n",__LINE__, mtos_ticks_get());
      ret = avc_av_start(class_get_handle_by_id(AVC_CLASS_ID),
                &video_cfg, &audio_cfg, start_type);

      play_para.pid = new_video_pid;
      play_para.type = DMX_VIDEO_TYPE;
      play_para.stream_in = ts_in; 

      dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &p_data->v_chanid);
      dmx_chan_start(p_data->p_dmx_dev, p_data->v_chanid);
      p_pg_info->v_pid = new_video_pid;
      p_pg_info->video_type = p_pmt->video_type;
      pid_changed = TRUE;

      play_para.pid = new_audio_pid;
      play_para.type = DMX_AUDIO_TYPE;
      play_para.stream_in = ts_in; 
      dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &p_data->a_chanid);
      dmx_chan_start(p_data->p_dmx_dev, p_data->a_chanid);
      p_pg_info->a_pid = new_audio_pid;
      p_pg_info->audio_type = new_audio_type;

      play_para.pid = new_pcr_pid;
      play_para.type = DMX_PCR_TYPE;
      play_para.stream_in = ts_in; 
      dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &p_data->p_chanid);
      dmx_chan_start(p_data->p_dmx_dev, p_data->p_chanid);
      p_pg_info->pcr_pid = new_pcr_pid;
  }
  else
  {
    p_pg_info->v_pid = new_video_pid;
    p_pg_info->video_type = p_pmt->video_type;
    p_pg_info->a_pid = new_audio_pid;
    p_pg_info->audio_type = new_audio_type;
    p_pg_info->pcr_pid = new_pcr_pid;
  }
    pid_changed = TRUE;
#endif
  if(pid_changed)
  {
    send_evt_to_ui(PB_EVT_DYNAMIC_PID, (u32)p_pg_info, 0);
  }
}

static void do_revise_eid(pb_handle_t *p_handle, sdt_t *p_sdt)
{
  BOOL ret = FALSE;

  if (p_handle->p_policy->process_sdt_info == NULL)
  {
    return;
  }
  
  ret = p_handle->p_policy->process_sdt_info(p_handle->p_policy->p_data, p_sdt);

  if(ret)
  {
    send_evt_to_ui(PB_EVT_REVISE_EID, 0, 0);
  }
}
static void lock_dvbs_tuner_1(pb_handle_t *p_handle, play_param_t *p_param, u8 tuner_id)
{
  priv_data_t *p_data = &p_handle->data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  if(p_param->inner.dvbs_lock_info.disepc_rcv.is_fixed)
    nc_cpy_diseqc_info(nc_handle, tuner_id, &p_param->inner.dvbs_lock_info.disepc_rcv);
  else
    nc_set_diseqc(nc_handle, tuner_id, &p_param->inner.dvbs_lock_info.disepc_rcv);
  nc_set_blind_scan_mode(nc_handle, tuner_id, FALSE);
  dvbs_calc_search_info(&p_data->search_info,
    &p_param->inner.dvbs_lock_info.sat_rcv,
    &p_param->inner.dvbs_lock_info.tp_rcv);
  PB_PERF("^^^^^^^^^^ send lock %d\n", mtos_ticks_get()  - g_stop_point);
  nc_set_tp(nc_handle, tuner_id, p_data->p_nim_svc, &p_data->search_info);
}

static void lock_dtmb_tuner_1(pb_handle_t *p_handle, play_param_t *p_param, u8 tuner_id)
{
  priv_data_t *p_data = &p_handle->data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  OS_PRINTF("playback lock dvbt tuner\n");

  nc_set_blind_scan_mode(nc_handle, tuner_id, FALSE);
  
  p_data->search_info.channel_info.frequency
    = p_param->inner.dvbt_lock_info.tp_freq;
  p_data->search_info.channel_info.param.dvbt.band_width
    = p_param->inner.dvbt_lock_info.band_width;
  p_data->search_info.channel_info.param.dvbt.nim_type
    = p_param->inner.dvbt_lock_info.nim_type;
  p_data->search_info.channel_info.nim_type
    = p_param->inner.dvbt_lock_info.nim_type;
 
  nc_set_tp(nc_handle, tuner_id, p_data->p_nim_svc, &p_data->search_info);
}

static void lock_dvbt2_tuner_1(pb_handle_t *p_handle, play_param_t *p_param, u8 tuner_id)
{
  priv_data_t *p_data = &p_handle->data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  OS_PRINTF("playback lock dvbt2 tuner\n");

  nc_set_blind_scan_mode(nc_handle, tuner_id, FALSE);
  
  p_data->search_info.channel_info.nim_type
    = p_param->inner.dvbt_lock_info.nim_type;
  p_data->search_info.channel_info.param.dvbt.nim_type
    = p_param->inner.dvbt_lock_info.nim_type;
  p_data->search_info.channel_info.frequency
    = p_param->inner.dvbt_lock_info.tp_freq;
  p_data->search_info.channel_info.param.dvbt.band_width
    = p_param->inner.dvbt_lock_info.band_width;
  p_data->search_info.channel_info.param.dvbt.plp_id
    = p_param->inner.dvbt_lock_info.plp_id;
  p_data->search_info.channel_info.param.dvbt.PLP_index
    = p_param->inner.dvbt_lock_info.plp_index; 
  nc_set_tp(nc_handle, tuner_id, p_data->p_nim_svc, &p_data->search_info);
}


static void lock_dvbc_tuner_1(pb_handle_t *p_handle, play_param_t *p_param, u8 tuner_id)
{
  priv_data_t *p_data = &p_handle->data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  OS_PRINTF("playback lock dvbc tuner, freq %d, sym %d, mo %d\n", 
    p_param->inner.dvbc_lock_info.tp_freq,
    p_param->inner.dvbc_lock_info.tp_sym,
    p_param->inner.dvbc_lock_info.nim_modulate);

  nc_set_blind_scan_mode(nc_handle, tuner_id, FALSE);
  
  p_data->search_info.channel_info.nim_type = NIM_DVBC;
  p_data->search_info.channel_info.frequency = p_param->inner.dvbc_lock_info.tp_freq;
  p_data->search_info.channel_info.param.dvbc.symbol_rate =
    p_param->inner.dvbc_lock_info.tp_sym;
  p_data->search_info.channel_info.param.dvbc.modulation =
    p_param->inner.dvbc_lock_info.nim_modulate;  
  p_data->search_info.polarization = NIM_PORLAR_HORIZONTAL;  
  p_data->search_info.channel_info.spectral_polar = NIM_IQ_AUTO;  
  p_data->search_info.channel_info.lock = NIM_CHANNEL_UNLOCK;  
 
  nc_set_tp(nc_handle, tuner_id, p_data->p_nim_svc, &p_data->search_info);
}

static void lock_tuner_1(pb_handle_t *p_handle, play_param_t *p_param, u8 tuner_id)
{
  OS_PRINTF("playback lock tuner lock_mode %d\n", p_param->lock_mode);
  if(SYS_DVBS == p_param->lock_mode)
  {
    lock_dvbs_tuner_1(p_handle, p_param, tuner_id);
  }
  else if (SYS_DTMB == p_param->lock_mode)
  {
    lock_dtmb_tuner_1(p_handle, p_param, tuner_id);
  }
  else if (SYS_DVBT2 == p_param->lock_mode)
  {
    lock_dvbt2_tuner_1(p_handle, p_param, tuner_id);
  }
  else if(SYS_DVBC == p_param->lock_mode)
  {
    lock_dvbc_tuner_1(p_handle, p_param, tuner_id);
  }
  else
  {
    // Not support yet
  }
  OS_PRINTF("playback lock tuner end\n");
  
}

static void do_play(pb_handle_t *p_handle, play_param_t *p_param)
{
  priv_data_t *p_data = &p_handle->data;
  dvbs_program_t *p_pg_info = &p_param->pg_info;
  RET_CODE ret = SUCCESS;
  video_param_t video_cfg = {0};

//  audio_param_t audio_cfg = {0};
//  av_start_type_t   start_type = 0;
  PB_PERF("^^^^^^^^^^  step9 : %d\n", mtos_ticks_get()  - g_stop_point);
  log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_PLAY,0,0);
  
  aud_mute_onoff_vsb(p_data->p_audio_dev,TRUE);
  
  OS_PRINTF("Mark ap playback 1 do play:  vpid =%d , apid = %d, mode %d\n", 
                    (u16)p_pg_info->v_pid, (u16)p_pg_info->a_pid, p_param->start_mode);
  switch(p_param->start_mode)
  {
    case PB_START_USER:
      video_cfg.mode = VID_UNBLANK_USER;
      break;
    case PB_START_SYNC:
      if(p_pg_info->a_pid == 0 || p_pg_info->a_pid == 0x1fff)
      {
        video_cfg.mode = VID_UNBLANK_STABLE;
      }
      else
      {
        video_cfg.mode = VID_UNBLANK_SYNC;
      }
      break;
    case PB_START_FAST:
      if(p_pg_info->is_scrambled)
      {
        if(p_handle->p_policy->is_fast_mode == NULL)
        video_cfg.mode = VID_UNBLANK_STABLE;
        else if(p_handle->p_policy->is_fast_mode(p_data) == TRUE)
          video_cfg.mode = VID_UNBLANK_FAST;
        else
          video_cfg.mode = VID_UNBLANK_STABLE;
      }
      else
        video_cfg.mode = VID_UNBLANK_FAST;
      break;
    case PB_START_STABLE:
    default:
      video_cfg.mode = VID_UNBLANK_STABLE;
      break;
  }
  //if no video,don't start video.
  if(p_pg_info->v_pid)
  {
    ret = dmx_chan_start(p_data->p_dmx_dev, p_data->v_chanid);  
    CHECK_FAIL_RET_VOID(SUCCESS == ret);
    ret |= vdec_set_data_input(p_data->p_video_dev, FALSE);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_PLAY,1,0);
    PB_PERF("^^^^^^^^^^  step10 : %d\n", mtos_ticks_get()  - g_stop_point);
    OS_PRINTF("set video mode %d\n", video_cfg.mode);
    ret |=  vdec_start(p_data->p_video_dev, p_pg_info->video_type, video_cfg.mode);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_PLAY,2,0);
    PB_PERF("^^^^^^^^^^  step11 : %d\n", mtos_ticks_get()  - g_stop_point);

  }

  //if no audio,don't start audio.
  if(p_pg_info->a_pid)
  {
    ret = dmx_chan_start(p_data->p_dmx_dev, p_data->a_chanid);
    CHECK_FAIL_RET_VOID(SUCCESS == ret);
    //OS_PRINTF("\nMark ap playback 1 line :%d tick=%d\n",__LINE__, mtos_ticks_get());
    ret = dmx_chan_start(p_data->p_dmx_dev, p_data->p_chanid);
    CHECK_FAIL_RET_VOID(SUCCESS == ret);
    //OS_PRINTF("\nMark ap playback 1 line :%d tick=%d\n",__LINE__, mtos_ticks_get());
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_PLAY,3,0);
    PB_PERF("^^^^^^^^^^  step12 : %d\n", mtos_ticks_get()  - g_stop_point);
    
    ret = aud_start_vsb(p_data->p_audio_dev, p_pg_info->audio_type, AUDIO_NO_FILE);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_PLAY,4,0);
    PB_PERF("^^^^^^^^^^  step1a : %d\n", mtos_ticks_get()  - g_stop_point);
  }

  aud_set_volume_vsb(p_data->p_audio_dev, p_pg_info->audio_volume, p_pg_info->audio_volume);
  aud_set_play_mode_vsb(p_data->p_audio_dev, p_pg_info->audio_track);
  log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_PLAY,5,0);
  OS_PRINTF("\nMark ap playback 1 1 line :%d tick=%d\n",__LINE__, mtos_ticks_get());
  PB_PERF("^^^^^^^^^^  step13 : %d\n", mtos_ticks_get()  - g_stop_point);
  p_data->start_tick = mtos_ticks_get();
  
#ifndef SWITCHPG
  p_data->last_vpid = p_data->play_param.pg_info.v_pid;
  p_data->last_apid = p_data->play_param.pg_info.a_pid;
  p_data->last_ppid = p_data->play_param.pg_info.pcr_pid;
#endif
}

extern RET_CODE vdec_switch_ch_stop(void *p_dev);

static void do_stop(pb_handle_t *p_handle, stop_mode_t mode)
{
  priv_data_t *p_data = &p_handle->data;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif
  RET_CODE ret = SUCCESS;

  OS_PRINTF("##########do_stop 0x%x 0x%x##\n", p_data->status, mode);
#ifdef CHANGE_PERF
    
    PB_PERF("^^^^^^^^^^  stop start ticks %d\n", mtos_ticks_get() - g_stop_point);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,0,0); 
#endif
  if(p_data->status != PB_PLAYING)
  {
    //return;
  }
  if(STOP_PLAY_BLACK == mode)
  {
    //Disable video ISR to avoid garbage by quick channel
    // switch.
    aud_mute_onoff_vsb(p_data->p_audio_dev,TRUE);
  //  mtos_task_delay_ms(30);
  //  ret = avc_av_stop(class_get_handle_by_id(AVC_CLASS_ID));
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,1,0);
    PB_PERF("^^^^^^^^^^  step1 : %d\n", mtos_ticks_get()  - g_stop_point);
    ret =  vdec_stop(p_data->p_video_dev);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,2,0); 
    PB_PERF("^^^^^^^^^^  step2 : %d\n", mtos_ticks_get()  - g_stop_point);
    ret |= aud_stop_vsb(p_data->p_audio_dev);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,3,0); 
    PB_PERF("^^^^^^^^^^  step3 : %d\n", mtos_ticks_get()  - g_stop_point);
    disp_layer_show(p_data->p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,4,0);
    PB_PERF("^^^^^^^^^^  step4 : %d\n", mtos_ticks_get()  - g_stop_point);
    //mtos_task_delay_ms(30);
    //disp_layer_show(p_data->p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
    //pti_dev_av_enable(p_data->p_dmx_dev, FALSE);
    //vdec_stop(p_handle->data.p_video_dev);
  }
  else if(STOP_PLAY_FREEZE == mode)
  {
#if 0
    vdec_freeze(p_data->p_video_dev);
    vdec_stop(p_data->p_video_dev);
    aud_stop_vsb(p_data->p_audio_dev);//for warriors change channel
    disp_layer_show(p_data->p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
#else
  #ifndef WIN32
    if(chip_ic_id == IC_CONCERTO)
      vdec_switch_ch_stop(p_data->p_video_dev);
    else
  #endif    
      vdec_freeze_stop(p_data->p_video_dev);
    aud_stop_vsb(p_data->p_audio_dev);
#endif
  }
  else // STOP_PLAY_NONE
  {
    // Do nothing
   // ret = avc_av_stop(class_get_handle_by_id(AVC_CLASS_ID));   //fix bug 16214
    aud_stop_vsb(p_data->p_audio_dev);//for warriors change channel
  }
  log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,5,0); 
  PB_PERF("^^^^^^^^^^  step5 : %d\n", mtos_ticks_get()  - g_stop_point);
  if(p_data->a_chanid != INVAILD_CHANNEL_ID)
  {
      OS_PRINTF("{Rick LU}do stop!!!\n");
      dmx_chan_stop(p_data->p_dmx_dev, p_data->a_chanid);  
      dmx_chan_close(p_data->p_dmx_dev, p_data->a_chanid);
      p_data->a_chanid = INVAILD_CHANNEL_ID;
  }

  if(p_data->p_chanid != INVAILD_CHANNEL_ID)
  {  
      dmx_chan_stop(p_data->p_dmx_dev, p_data->p_chanid);  
      dmx_chan_close(p_data->p_dmx_dev, p_data->p_chanid);
      p_data->p_chanid = INVAILD_CHANNEL_ID;
  }
  if(p_data->v_chanid != INVAILD_CHANNEL_ID)
  {  
      dmx_chan_stop(p_data->p_dmx_dev, p_data->v_chanid);  
      dmx_chan_close(p_data->p_dmx_dev, p_data->v_chanid);
      p_data->v_chanid = INVAILD_CHANNEL_ID;
  }
  log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,6,0); 
  PB_PERF("^^^^^^^^^^  step6 : %d\n", mtos_ticks_get()  - g_stop_point);
  dmx_av_reset(p_data->p_dmx_dev);
  log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,7,0); 
  PB_PERF("^^^^^^^^^^  step7 : %d\n", mtos_ticks_get()  - g_stop_point);
  if(p_handle->p_policy->is_subtitle_on == TRUE)
  {
    subt_txt_info_reset_1(p_data->p_sub_ttx_data);
  }
  log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_DO_STOP,8,0); 
  PB_PERF("^^^^^^^^^^  step8 : %d\n", mtos_ticks_get()  - g_stop_point);
#ifdef SWITCHPG
  //p_data->last_tp = 0;
#else
  p_data->last_vpid = 0;
  p_data->last_apid = 0;
  p_data->last_ppid = 0;
#endif
  if(p_handle->p_policy->on_stop != NULL)
  {
    p_handle->p_policy->on_stop(p_handle->p_policy->p_data, &p_data->play_param);
  }
}

static void switch_audio_channel(pb_handle_t *p_handle,
  u32 a_pid, adec_src_fmt_vsb_t a_type)
{
  priv_data_t *p_data = &p_handle->data;
//  dmx_chanid_t p_chan_handle = NULL;
  dmx_play_setting_t play_para = {0};

  if(p_data->a_chanid == INVAILD_CHANNEL_ID)
  {
    return;
  }
  //dev_io_ctrl(p_data->p_audio_dev, AUDIO_CMD_SET_MUTE, TRUE);
  //aud_mute_onoff_vsb(p_data->p_audio_dev,TRUE);
  aud_stop_vsb(p_data->p_audio_dev);
  //pti_dev_set_audio_pid(p_data->p_dmx_dev, (u16)a_pid);
  
  dmx_chan_stop(p_data->p_dmx_dev, p_data->a_chanid);  
  dmx_chan_close(p_data->p_dmx_dev, p_data->a_chanid);
  
  play_para.pid = (u16)a_pid;
  play_para.type = DMX_AUDIO_TYPE;
  play_para.stream_in = p_data->ts_in; 
  dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &p_data->a_chanid);
  dmx_chan_start(p_data->p_dmx_dev, p_data->a_chanid);  
  
  mtos_task_delay_ms(50);
  OS_PRINTF("switch_audio_channel\n");
  aud_start_vsb(p_data->p_audio_dev, a_type,  AUDIO_NO_FILE);
//  aud_mute_onoff_vsb(p_data->p_audio_dev,FALSE);
}

static BOOL wait_video_frame_1(pb_handle_t *p_handle)
{
  priv_data_t *p_data = &p_handle->data;
  BOOL wait_done = FALSE;
  u32 wait_limit = 0;
  u32 wait_time = (mtos_ticks_get() - p_data->start_tick) * 10;
  void *p_avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  dvbs_program_t *p_pg_info = &p_data->play_param.pg_info;
  BOOL is_timed_out = FALSE;
  //dmx_chanid_t p_chan_handle = 0;
  vdec_info_t v_info = {0};
  dmx_play_setting_t play_para = {0};
  BOOL stop_hd_audio = TRUE;

  CHECK_FAIL_RET_FALSE(p_avc_handle != NULL);

  //radio or scramble tv don't wait video
  wait_limit = (0 == p_pg_info->v_pid) ? 0 : 4000;
  //
  // wait
  // 4s
  vdec_get_info(p_data->p_video_dev,&v_info);

  if(wait_time >= wait_limit) //time out
  {
    if(0 == p_pg_info->v_pid) //it's radio, show logo
    {
    }
    else
    {
      is_timed_out = TRUE;
      OS_PRINTF("%s,timeout!\n",__FUNCTION__);
    }
    wait_done = TRUE;
  }
  else if((v_info.is_stable) || (v_info.I_frame_stable))
  {
    //PB_PERF("^^^^^^^^^^  step14 : %d\n", mtos_ticks_get()  - g_stop_point);
    log_perf(LOG_AP_CHANGE_CHANNEL, TAB_WAIT_VIDEO_STABLE,0,0);
             
    send_evt_to_ui(PB_EVT_NOTIFY_VIDEO_FORMAT, (u32)v_info.vid_format, (u32) p_pg_info->s_id);
    if((p_data->play_param.start_mode == PB_START_STABLE) ||
      (p_data->play_param.start_mode == PB_START_SYNC && v_info.is_sync)
      || p_data->play_param.start_mode == PB_START_USER
      || p_data->play_param.start_mode == PB_START_FAST)
    {
      //PB_PERF("^^^^^^^^^^  step15 : %d\n", mtos_ticks_get()  - g_stop_point);
      log_perf(LOG_AP_CHANGE_CHANNEL, TAB_WAIT_VIDEO_SYNC,
      ((v_info.width > 720)?1:0),(u8)p_pg_info->video_type);
      avc_switch_video_mode_1(p_avc_handle, p_pg_info->tv_mode);
      avc_set_video_aspect_mode_1(class_get_handle_by_id(AVC_CLASS_ID),
          p_pg_info->aspect_ratio);
      wait_done = TRUE;
      OS_PRINTF("%s,wait start_mode=%d,v_info.is_sync=%d\n",__FUNCTION__,
          p_data->play_param.start_mode, v_info.is_sync);
    }
  }

  if(wait_done)
  {
    // currently we use pg's volume and potentially that in
    // some case maybe user
    // want to use global volume. So in that case we recommand
    // playback module
    // add
    // a new interface to decalare that we go into global
    // volume mode
    // explicitly.
    if(avc_is_mute_1(p_avc_handle) == FALSE)
    {
      if(p_pg_info->v_pid != 0 &&
        v_info.sd_image_flag == FALSE &&
        p_handle->p_policy->is_sd_only != NULL && 
        p_handle->p_policy->is_sd_only(&stop_hd_audio) == TRUE)
      {
        if (stop_hd_audio == TRUE)
        {
          OS_PRINTF("HD video found in SD only mode, stop audio.\n");
          aud_stop_vsb(p_data->p_audio_dev);
        }

        send_evt_to_ui(PB_EVT_NOT_SUPPORT_VIDEO, 0, 0);
      }
      else
      {
        aud_mute_onoff_vsb(p_data->p_audio_dev,FALSE);
      }
    }

    if(!is_timed_out)
    {
      if(FALSE == v_info.is_sup)
      {
        play_para.pid = 0x1FFF;
        play_para.type = DMX_VIDEO_TYPE;
        //pti_dev_set_video_pid(p_data->p_dmx_dev, 0);
        //dmx_play_chan_open(p_data->p_dmx_dev, &play_para, &p_chan_handle);
        //dmx_chan_start(p_data->p_dmx_dev, p_chan_handle);  
        //PB_DBG("ap_playback: unsupport format, reset video pid to 0\n");
      }
    }
  }

  return wait_done;
}

static void check_scart(pb_handle_t *p_handle)
{
  priv_data_t *p_data = &p_handle->data;
  void *p_avc_handle = class_get_handle_by_id(AVC_CLASS_ID);

  static u32 dcnt = 0;
  BOOL is_detected = FALSE;

  CHECK_FAIL_RET_VOID(p_avc_handle != NULL);

  is_detected = avc_detect_scart_vcr_1(p_avc_handle);
  if(is_detected != p_data->is_scart_vcr_detected)
  {
    dcnt++;
  }
  else
  {
    dcnt = 0;
  }

  // check for stable
  if(dcnt < SCART_VCR_DETECT_CNT)
  {
    // check again
    return;
  }
  else
  {
    dcnt = 0;
    p_data->is_scart_vcr_detected = is_detected;

    send_evt_to_ui(PB_EVT_SCART_VCR_DETECTED, is_detected, 0);
  }
}

static BOOL check_same_tp(pb_handle_t *p_handle, play_param_t *p_param, u8 tuner_id)
{
  nc_channel_info_t search_info = {0};
  nc_channel_info_t lock_info = {0};
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  nc_get_tp(nc_handle, tuner_id, &lock_info);

  if(p_param->lock_mode == SYS_DVBS)
  {
    BOOL  flag = FALSE;

    dvbs_calc_search_info(&search_info,
      &p_param->inner.dvbs_lock_info.sat_rcv,
      &p_param->inner.dvbs_lock_info.tp_rcv);
    //remove spectral_polar compare, because of changeset 3340 (b9f63e45ca40).
    flag = (search_info.onoff22k == lock_info.onoff22k)
            && (search_info.polarization == lock_info.polarization)
            && (search_info.channel_info.frequency == lock_info.channel_info.frequency)
            && (search_info.channel_info.param.dvbs.symbol_rate == 
                  lock_info.channel_info.param.dvbs.symbol_rate);
    //if use unicable, compare unicable param.
    if(flag && search_info.channel_info.param.dvbs.uc_param.use_uc)
      flag = flag && (search_info.channel_info.param.dvbs.uc_param.bank == 
                  lock_info.channel_info.param.dvbs.uc_param.bank)
            && (search_info.channel_info.param.dvbs.uc_param.user_band == 
                  lock_info.channel_info.param.dvbs.uc_param.user_band)
            && (search_info.channel_info.param.dvbs.uc_param.ub_freq_mhz == 
                  lock_info.channel_info.param.dvbs.uc_param.ub_freq_mhz);
    return flag;
  }
  else if(p_param->lock_mode == SYS_DTMB)
  {
    return((p_param->inner.dvbt_lock_info.tp_freq == lock_info.channel_info.frequency));
  }
  else if(p_param->lock_mode == SYS_DVBT2)
  {
    switch(p_param->inner.dvbt_lock_info.nim_type)
    {
      case NIM_DVBT:
        return (p_param->inner.dvbt_lock_info.tp_freq == lock_info.channel_info.frequency);
      case NIM_DVBT2:
        return ((p_param->inner.dvbt_lock_info.tp_freq == lock_info.channel_info.frequency) &&
          (p_param->inner.dvbt_lock_info.plp_id == lock_info.channel_info.param.dvbt.plp_id));
      default:
        break;
    }
  }
  else if(p_param->lock_mode == SYS_DVBC)
  {
    return((p_param->inner.dvbc_lock_info.tp_freq == lock_info.channel_info.frequency)
              && (p_param->inner.dvbc_lock_info.tp_sym
              == lock_info.channel_info.param.dvbc.symbol_rate)
              && (p_param->inner.dvbc_lock_info.nim_modulate
              == lock_info.channel_info.param.dvbc.modulation));
  }
  return FALSE;
}

#ifdef SWITCHPG
static u32 get_play_tp(play_param_t *p_param)
{
  nc_channel_info_t search_info = {0};

  if(p_param->lock_mode == SYS_DVBS)
  {
    dvbs_calc_search_info(&search_info,
      &p_param->inner.dvbs_lock_info.sat_rcv,
      &p_param->inner.dvbs_lock_info.tp_rcv);

    OS_PRINTF("get_play_tp  %d\n", search_info.channel_info.frequency);
    return search_info.channel_info.frequency;
  }
  else if(p_param->lock_mode == SYS_DTMB)
  {
    return p_param->inner.dvbt_lock_info.tp_freq;
  }
  else if(p_param->lock_mode == SYS_DVBT2)
  {
    return p_param->inner.dvbt_lock_info.tp_freq;
  }
  else if(p_param->lock_mode == SYS_DVBC)
  {
    return p_param->inner.dvbc_lock_info.tp_freq;
  }
  return 0;
}
#endif

static void monitor(pb_handle_t *p_handle)
{
  priv_data_t *p_data = &p_handle->data;

  /*FIXME: should check whether we have scart device*/
  if(0)
  {
    check_scart(p_handle);
  }

  if(p_handle->p_policy->check_video != NULL)
  {
    p_handle->p_policy->check_video(p_data->play_param);
  }
  else if(p_handle->p_policy->check_video_v2 != NULL)
  {
    p_handle->p_policy->check_video_v2(p_data->play_param, &p_data->a_chanid, &p_data->p_chanid,
                                       &p_data->v_chanid, 0);
  }
  
  avc_switch_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID),
    p_data->play_param.pg_info.tv_mode);
  avc_set_video_aspect_mode_1(class_get_handle_by_id(AVC_CLASS_ID),
    p_data->play_param.pg_info.aspect_ratio);
  vdec_info_update(p_data->p_video_dev);
}

static void check_hd_program(pb_handle_t *p_handle)
{
  if(p_handle->p_policy->check_hd_prog != NULL)
  {
    p_handle->p_policy->check_hd_prog(p_handle->p_policy->p_data);
  }
}

static void pb_on_nit_found(pb_handle_t *p_handle, nit_t *p_nit, u8 ts_in)
{
  priv_data_t *p_data = &p_handle->data;  

  if (p_handle->p_policy->process_nit_info != NULL)
  {
    p_handle->p_policy->process_nit_info(p_handle->p_policy->p_data, p_nit);
  }
  
  if(p_nit->sec_number < p_nit->last_sec_number)
  {
    // Request next NIT
    dvb_request_t dvb_req = {0};
    dvb_req.table_id = DVB_TABLE_ID_NIT_ACTUAL;
    dvb_req.req_mode = DATA_SINGLE;
    dvb_req.period = 0;
    dvb_req.para1 = DVB_TABLE_ID_NIT_ACTUAL;
    dvb_req.para2 = ((p_nit->sec_number + 1) << 16) | 0xFFFF;
    dvb_req.ts_in  = ts_in;

    p_data->p_dvb_svc->do_cmd(p_data->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
  }
}

static void cat_request(pb_handle_t *p_handle, u16 sid, u8 ts_in)
{
  priv_data_t *p_data = &p_handle->data;
  dvb_request_t dvb_req = {0};

  if(!p_data->play_param.is_do_cat)
    return;
    
  dvb_req.table_id = DVB_TABLE_ID_CAT;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = sid;
  dvb_req.para2 = 0;
  dvb_req.ts_in = ts_in;
  
  p_data->p_dvb_svc->do_cmd(p_data->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
  
  OS_PRINTF("CAS: cat request. sid %d\n", sid);
}
static void nit_request(pb_handle_t *p_handle, u8 ts_in)
{
  priv_data_t *p_data = &p_handle->data;
  dvb_request_t dvb_req = {0};

  if(!p_data->play_param.is_do_nit)
    return;
#if 0
  dvb_req.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para2 = 0;
#endif
  dvb_req.req_mode  = DATA_SINGLE;
  dvb_req.table_id  = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para1     = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para2     = 0xFFFF;
  dvb_req.ts_in = ts_in;

  p_data->p_dvb_svc->do_cmd(p_data->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
}

static void pat_request(pb_handle_t *p_handle, u8 ts_in)
{
  priv_data_t *p_data = &p_handle->data;
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_PAT;
  //dvb_req.req_mode = DATA_PERIODIC;
  //dvb_req.period = PAT_MONITOR_INTERVAL;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  
  dvb_req.para1 = DVB_TABLE_ID_PAT;
  dvb_req.para2 = 0;
  dvb_req.ts_in = ts_in;
  //OS_PRINTF("pb pat req\n");
  p_data->p_dvb_svc->do_cmd(p_data->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
}

static void pb_on_sdt_found(pb_handle_t *p_handle, sdt_t *p_sdt)
{
  if (p_handle->p_policy->process_sdt_info == NULL)
  {
    return;
  }
  
  (void)p_handle->p_policy->process_sdt_info(p_handle->p_policy->p_data, p_sdt);
}

static void sdt_request(pb_handle_t *p_handle, u8 ts_in)
{
  priv_data_t *p_data = &p_handle->data;
  dvb_request_t dvb_req = {0};

  if(!p_data->play_param.is_do_sdt)
    return;
  dvb_req.table_id = DVB_TABLE_ID_SDT_ACTUAL;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = DVB_TABLE_ID_SDT_ACTUAL;
  dvb_req.para2 = 0;
  dvb_req.ts_in = ts_in;
  p_data->p_dvb_svc->do_cmd(p_data->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));
}

static void pmt_request(pb_handle_t *p_handle, u16 pid, u16 s_id, u8 ts_in)
{
  priv_data_t *p_data = &p_handle->data;
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_PMT;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = DVB_TABLE_ID_PMT;
  dvb_req.para2 = ((pid << 16) | s_id);
  dvb_req.ts_in = ts_in;
  //OS_PRINTF("pb pmt req\n");
  p_data->p_dvb_svc->do_cmd(p_data->p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));

}

/*!
  process the message mothed, it will be called by playback task.

  \param[in] p_handle playback handle
  \param[in] p_msg new message
  */
static void pb_single_step(handle_t handle, os_msg_t *p_msg)
{
  pb_handle_t *p_handle = (pb_handle_t *)handle;
  priv_data_t *p_data = &p_handle->data;
  play_param_t *p_play_param = NULL;
  cmd_t cmd = {0};
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  u8 force_ts_change = 0;
  u8 ts_new = 0;
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
#ifdef SWITCHPG
  nc_channel_info_t lock_info = {0};
  BOOL b_same_tuner = FALSE;
#endif

  if(p_msg != NULL)
  {
    switch(p_msg->content)
    {
      case NC_EVT_UNLOCKED:
      case NC_EVT_LOCKED:
            p_data->tuner_id = p_msg->context;
            OS_PRINTF("!!!!!PB:PB_EVT_LOCK_RSL %d\n", 
                      NC_EVT_LOCKED == p_msg->content);
            PB_PERF("^^^^^^^^^^ ret lock1 %d\n", mtos_ticks_get()  - g_stop_point);
#ifdef SWITCHPG
            nc_get_tp(nc_handle, p_data->tuner_id, &lock_info);
            p_data->last_tp = lock_info.channel_info.frequency;
#endif
            if(p_data->status != PB_STOP)
            {
               if (check_same_tp(p_handle, &p_data->play_param, p_data->tuner_id))
               {
                 OS_PRINTF("PB play ticks %d  status %d\n", mtos_ticks_get(), p_data->status);
                 if(PB_PLAYING != p_data->status)
                 {
                    do_play(p_handle, &p_data->play_param);
                    p_data->status = PB_PLAYING;
                    p_handle->data.pat_monitor_start = mtos_ticks_get();
                    p_handle->data.pat_monitor_interval = 100;
                    p_handle->p_policy->on_play(p_handle->p_policy->p_data, &p_data->play_param);
                 }
                 log_perf(LOG_AP_CHANGE_CHANNEL, TAB_LOCK_TURNER_END,0,0);
                 p_data->sm = PB_SM_WAIT_VIDEO;
                 }
                 send_evt_to_ui(PB_EVT_LOCK_RSL, NC_EVT_LOCKED == p_msg->content, 0);
             }

      break;
      case PB_CMD_PLAY:
        OS_PRINTF("PB_CMD_PLAY\n");
        OS_PRINTF("do lock ticks %d\n", mtos_ticks_get());
        p_data->tuner_id = p_msg->para2 >> 24;
        force_ts_change = (p_msg->para2 >> 23) & 0x1;
        ts_new = (p_msg->para2 >> 16) & 0x7f;
        nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
        p_msg->para2 &= 0x0000ffff;
        p_play_param = (play_param_t *)p_msg->para1;
        init_play_param(p_handle, p_play_param);
       // p_data->status = PB_PLAYING;
        if(p_msg->para2 == 0)
           do_play_chan_open(p_handle, &p_data->play_param,  p_data->ts_in, 
           force_ts_change, ts_new);
        lock_tuner_1(p_handle, &p_data->play_param, p_data->tuner_id);
        p_data->sm = PB_SM_WAIT_LOCK;
        p_data->status = PB_LOCK_END;
#ifdef SWITCHPG
        if(p_data->last_tp == get_play_tp(&p_data->play_param))
#else
        if(p_data->last_vpid != p_data->play_param.pg_info.v_pid
          || p_data->last_apid != p_data->play_param.pg_info.a_pid
          || p_data->last_ppid != p_data->play_param.pg_info.pcr_pid)
#endif
        {
            do_play(p_handle, &p_data->play_param);
            p_data->status = PB_PLAYING;
            p_handle->data.pat_monitor_start = mtos_ticks_get();
            p_handle->data.pat_monitor_interval = 100;
            p_handle->p_policy->on_play(p_handle->p_policy->p_data, &p_data->play_param);
            if(check_same_tp(p_handle, &p_data->play_param, p_data->tuner_id))
            {
              p_data->sm = PB_SM_WAIT_VIDEO;
            }
        }
      cmd.id = TIME_CMD_UPDATE_SOURCE;
      cmd.data1 = (u32)p_data->tuner_id;
      ap_frm_do_command(APP_TIME, &cmd);  
        break;        
      case PB_CMD_STOP:
        OS_PRINTF("PB_CMD_STOP\n");
        do_stop(p_handle, p_msg->para1);
        //pat_free(p_handle);
        OS_PRINTF("PB stop ticks %d\n", mtos_ticks_get());
        p_data->status = PB_STOP;
#ifdef SWITCHPG
        p_data->last_tp = 0;
#endif
        //p_data->status.cur_pg_id = INVALID_PG_ID;
        p_data->sm = PB_SM_IDLE;
        break;
        
      case PB_CMD_STOP_SYNC:
        OS_PRINTF("PB_CMD_STOP_SYNC\n");
        do_stop(p_handle, p_msg->para1);
        OS_PRINTF("PB stop by sync ticks %d\n", mtos_ticks_get());
        p_data->status = PB_STOP;
#ifdef SWITCHPG
        p_data->last_tp = 0;
#endif
        if ((chip_ic_id == IC_SONATA) && (STOP_PLAY_FREEZE == p_msg->para1))
        {
          p_data->sm = PB_SM_MONITOR_SIGNAL;
        }
        else
        {
          p_data->sm = PB_SM_WAIT_STOP;
        }
        send_evt_to_ui(PB_EVT_STOPPED, 0, 0);
        break;

      case PB_CMD_SWITCH_PG:
        OS_PRINTF("PB_CMD_SWITCH_PG\n");
#ifdef SWITCHPG
        if(p_data->tuner_id == p_msg->para2 >> 24)
          b_same_tuner = TRUE;
#endif
        p_data->tuner_id = p_msg->para2 >> 24;  
        force_ts_change = (p_msg->para2 >> 23) & 0x1;
        ts_new = (p_msg->para2 >> 16) & 0x7f;        
        nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
        p_msg->para2 &= 0x0000ffff;
        if(p_data->status != PB_STOP)
        {
          log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_SWITCH_PG, 
      (u32)(&p_data->play_param.inner.dvbs_lock_info.tp_rcv),(u32)(&p_data->play_param.pg_info));
          do_stop(p_handle, p_msg->para2);
          //pat_free(p_handle);
          OS_PRINTF("PB stop ticks %d\n", mtos_ticks_get());
          p_data->status = PB_STOP;
        }
        OS_PRINTF("do lock ticks %d\n", mtos_ticks_get());
        p_play_param = (play_param_t *)p_msg->para1;
        init_play_param(p_handle, p_play_param);
#ifdef SWITCHPG
        p_data->status = PB_LOCK_END;
#else
        p_data->status = PB_PLAYING;
#endif
        do_play_chan_open(p_handle, &p_data->play_param,  p_data->ts_in, force_ts_change, ts_new);
        log_perf(LOG_AP_CHANGE_CHANNEL, TAB_PB_PLAY,0,0);
#ifdef SWITCHPG
        if( b_same_tuner && p_data->last_tp == get_play_tp(&p_data->play_param))
#else
        if(p_data->last_vpid != p_data->play_param.pg_info.v_pid
          || p_data->last_apid != p_data->play_param.pg_info.a_pid
          || p_data->last_ppid != p_data->play_param.pg_info.pcr_pid)
#endif
        {
          do_play(p_handle, &p_data->play_param);
          p_data->status = PB_PLAYING;
          p_handle->data.pat_monitor_start = mtos_ticks_get();
          p_handle->data.pat_monitor_interval = 100;
          PB_PERF("^^^^^^^^^^ on_play1 %d\n", mtos_ticks_get()  - g_stop_point);
          p_handle->p_policy->on_play(p_handle->p_policy->p_data, &p_data->play_param);
          PB_PERF("^^^^^^^^^^ on_play2 %d\n", mtos_ticks_get()  - g_stop_point);
#ifdef SWITCHPG
          p_data->sm = PB_SM_WAIT_VIDEO;
#endif
        }
#ifdef SWITCHPG
#endif
        {
          log_perf(LOG_AP_CHANGE_CHANNEL, TAB_LOCK_TURNER_START,0,0);
          lock_tuner_1(p_handle, &p_data->play_param, p_data->tuner_id);
#ifdef SWITCHPG
#else
          if(check_same_tp(p_handle, &p_data->play_param,p_data->tuner_id))
          {
            p_data->sm = PB_SM_WAIT_VIDEO;
          }
          else
#endif
          {
            p_data->sm = PB_SM_WAIT_LOCK;
          }
        }
        send_evt_to_ui(PB_EVT_PG_SWTICHED, 0, 0);
        cmd.id = TIME_CMD_UPDATE_SOURCE;
        cmd.data1 = (u32)p_data->tuner_id;
        ap_frm_do_command(APP_TIME, &cmd);  
        break;
        
      case PB_CMD_SWITCH_AUDIO_CHANNEL:
        switch_audio_channel(p_handle, p_msg->para1, p_msg->para2);
        break;
      case PB_CMD_SWITCH_AUDIO_CHANNEL_SYNC:
        switch_audio_channel(p_handle, p_msg->para1, p_msg->para2);
        send_evt_to_ui(PB_EVT_PG_AUDIO_CH_SWTICHED, 0, 0);
        break;
      case PB_CMD_SET_MUTE:
        //avc_set_mute((BOOL)p_msg->para1, (BOOL)p_msg->para2);
        send_evt_to_ui(PB_EVT_SET_MUTE, 0, 0);
        break;
      case PB_CMD_SET_VOL:
        //avc_setvolume((u8)p_msg->para1);
        send_evt_to_ui(PB_EVT_SET_VOL, 0, 0);
        break;
      case PB_CMD_RESET_TVMODE:
        p_data->play_param.pg_info.tv_mode = (u16)p_msg->para1;
        break;
      case PB_CMD_RESET_VIDEO_ASPECT_MODE:
        p_data->play_param.pg_info.aspect_ratio = p_msg->para1;
        break;
      case DVB_TABLE_TIMED_OUT:
        if(p_msg->para1 == DVB_TABLE_ID_PAT)
        {
          p_data->pat_request_flag = 0;
        }
        if(p_msg->para1 == DVB_TABLE_ID_NIT_ACTUAL)
        {
          p_data->nit_request_flag = 0;
        }
        break;
      case DVB_PAT_FOUND:
        p_data->pat_request_flag = 0;
        break;
      case DVB_PAT_PMT_INFO:
        if(p_data->play_param.pg_info.s_id == p_msg->para2)
        {
          pmt_request(p_handle, (u16)p_msg->para1, (u16)p_msg->para2,  p_data->ts_in);
          if(p_data->status != PB_STOP)
          {
            cat_request(p_handle,  (u16)p_msg->para2, p_data->ts_in);
          }
        }
        break;
      case DVB_PMT_FOUND:
        if((p_data->status == PB_PLAYING) && 
          (p_data->play_param.pg_info.s_id == (u32)(((pmt_t *)(p_msg->para1))->prog_num)))
        {
           nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
           do_dynamic_pid(p_handle, (pmt_t *)p_msg->para1,  p_data->ts_in);
            if(p_handle->p_policy->is_subtitle_on == TRUE)
            {
              pmt_update_1(p_handle->data.p_sub_ttx_data, (pmt_t *)p_msg->para1,  p_data->ts_in);
            }
        }
        else
        {
          if ((p_data->status == PB_STOP) && 
          (p_data->play_param.pg_info.s_id == (u32)(((pmt_t *)(p_msg->para1))->prog_num)))
          {
            do_dynamic_pid(p_handle, (pmt_t *)p_msg->para1,  p_data->ts_in);
          }
          if(p_handle->p_policy->is_subtitle_on == TRUE)
          {
             subt_txt_info_reset_1(p_handle->data.p_sub_ttx_data);
          }
        }
        break;
      case DVB_CAT_FOUND:
      {
        if(NULL != p_handle->p_policy->process_cat_info)
        {
          p_handle->p_policy->process_cat_info(p_handle->p_policy->p_data, (cat_cas_t *)p_msg->para1);
        }
        break;
      }
      case DVB_SDT_FOUND:
        if(p_data->status == PB_PLAYING)
        {
          do_revise_eid(p_handle, (sdt_t *)p_msg->para1);
        }
        if(p_data->play_param.lock_mode == SYS_DVBC)
        {
          pb_on_sdt_found(p_handle, (sdt_t *)p_msg->para1);
        }
        break;
      case DVB_NIT_FOUND:
        p_data->nit_request_flag = 0;
        nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
        pb_on_nit_found(p_handle, (nit_t *)p_msg->para1,  p_data->ts_in);
        break;
      //falcon debug
      case PB_CMD_START_TTX:
        //fix 28286
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
        vbi_set_ts_in(p_data->ts_in);
        vbi_ttx_start_vsb(((p_msg->para1 >> 16) & 0xFF) , (p_msg->para1 & 0xFF), p_msg->para2);
        break;
      case PB_CMD_STOP_TTX:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        vbi_ttx_stop_vsb();
       break;
      case PB_CMD_SET_TTX_LANG:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        set_ttx_lang_1(p_handle->data.p_sub_ttx_data, p_msg->para1);
        break;
      case PB_CMD_POST_TTX_KEY:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        post_ttx_key_1(p_handle->data.p_sub_ttx_data, p_msg->para1);
        break;
      case PB_CMD_SHOW_TTX:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        show_ttx_1(p_handle->data.p_sub_ttx_data, p_msg->para1);
        break;
      case PB_CMD_HIDE_TTX:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        if(!hide_ttx_1(p_handle->data.p_sub_ttx_data))
        {
          send_evt_to_ui(PB_EVT_HIDE_TTX, 0, 0);
        }
        break;
      case VBI_TTX_HIDED:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
      //case VBI_TTX_STOPPED:
        send_evt_to_ui(PB_EVT_HIDE_TTX, 0, 0);
        break;
      case VBI_TTX_STOPPED:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        OS_PRINTF("got VBI_TTX_STOPPED\n");
        send_evt_to_ui(PB_EVT_TTX_STOPED, 0, 0);
        break;
      case PB_CMD_START_VBI_INSERTER:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        start_vbi_inserter_1(p_handle->data.p_sub_ttx_data);
        break;
      case PB_CMD_STOP_VBI_INSERTER:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        stop_vbi_inserter_1(p_handle->data.p_sub_ttx_data);
        break;
      case PB_CMD_START_SUBT:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
        vbi_set_ts_in(p_data->ts_in);
        subt_set_ts_in(p_data->ts_in);
        start_subt_dec_1(p_handle->data.p_sub_ttx_data);
        break;
      case PB_CMD_STOP_SUBT:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        stop_subt_dec_1(p_handle->data.p_sub_ttx_data);
        break;
      case PB_CMD_STOP_SUBT_SYNC:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        stop_subt_dec_1(p_handle->data.p_sub_ttx_data);
        break;
      case SUBT_EVT_STOPPED:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        //OS_PRINTF("\n\n\n=====OK======\n\n\n");
        send_evt_to_ui(PB_EVT_SUBT_STOPED, 0, 0);
        break;
      case PB_CMD_SET_SUBT_SERVICE:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        set_subt_service_1(p_handle->data.p_sub_ttx_data,
          p_msg->para1, p_msg->para2);
        break;
      case PB_CMD_SET_SUBT_LANG_CODE:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        set_subt_service_lang_code(p_handle->data.p_sub_ttx_data,
          p_msg->para1, p_msg->para2);
        break;
      case PB_CMD_SHOW_SUBT:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        show_subtitle_1(p_handle->data.p_sub_ttx_data);
        break;
      case PB_CMD_HIDE_SUBT:
        if(p_handle->p_policy->is_subtitle_on != TRUE)
        {
          break;
        }
        if(FALSE == hide_subtitle_1(p_handle->data.p_sub_ttx_data))
        {
          send_evt_to_ui(PB_EVT_HIDE_SUBT, 0, 0);
        }
        //send_evt_to_ui(PB_EVT_HIDE_SUBT, 0, 0);
        break;
      //ENABLE_AUDIO_DESCRIPTION
      case PB_CMD_INIT_AD_TASK:
      case PB_CMD_SET_VISUALLY_IMPAIRED:
      case PB_CMD_START_VISUALLY_IMPAIRED:
      case PB_CMD_STOP_VISUALLY_IMPAIRED:
      case PB_CMD_SET_AD_VOLUME_OFFSET:
        if(p_handle->p_policy->ad_function != NULL)
        {
          p_handle->p_policy->ad_function(p_msg->content, p_msg->para1, p_msg->para2);
        }
        break;
#if 0      
      case PB_CMD_INIT_AD_TASK:
       OS_PRINTF("PB_CMD_INIT_AD_TASK init 1\n");
       #ifndef WIN32
       ad_kernel_init(4,1024*1024);
       mtos_task_delay_ms(10);
       OS_PRINTF("PB_CMD_INIT_AD_TASK init 2\n");
       aud_descrip_init(5,16*1024);
       #endif
       break;
      case PB_CMD_SET_VISUALLY_IMPAIRED:
       OS_PRINTF("PB_CMD_SET_pid = %d, type =%d\n",p_msg->para1, p_msg->para2);
       #ifndef WIN32
       aud_descrip_set_pid_type(p_msg->para1, p_msg->para2);
       #endif
       break;        
      case PB_CMD_START_VISUALLY_IMPAIRED:
       OS_PRINTF("PB_CMD_START_VISUALLY_IMPAIRED\n");
       #ifndef WIN32
       aud_descrip_start();
       #endif
       break;
      case PB_CMD_STOP_VISUALLY_IMPAIRED:
       OS_PRINTF("PB_CMD_STOP_VISUALLY_IMPAIRED\n");
       #ifndef WIN32
       aud_descrip_stop();
       #endif
       break;
      case PB_CMD_SET_AD_VOLUME_OFFSET:                       
       #ifndef WIN32
       {
       u32 volume_offset = AD_VOL_LEVEL_0;    
       volume_offset = p_msg->para1;      
       aud_descrip_vol_set(&volume_offset);
       OS_PRINTF("PB_CMD_SET_AD_VOLUME_OFFSET volume offset = %d\n",p_msg->para1); 
       }   
       #endif
       break;
#endif
      default:
        break;
    }
  }
  else
  {
    switch(p_data->sm)
    {
      case PB_SM_IDLE:
        break;
      case PB_SM_WAIT_LOCK:
        break;
      case PB_SM_WAIT_VIDEO:
        if(wait_video_frame_1(p_handle))
        {
          PF_LOG("video_out_ticks %d\n", mtos_ticks_get());
          PB_PERF("^^^^^^^^^^  step16 : %d\n", mtos_ticks_get()  - g_stop_point);
          log_perf(LOG_AP_CHANGE_CHANNEL, TAB_WAIT_VIDEO_END, 
          (u32)(&p_data->play_param.inner.dvbs_lock_info.tp_rcv), 
          (u32)(&p_data->play_param.pg_info));
          p_data->signal_monitor_start = mtos_ticks_get();
          p_data->sm = PB_SM_MONITOR_SIGNAL;
          sdt_request(p_handle,  p_data->ts_in);
          p_data->pat_request_flag = 0;
          p_data->nit_request_flag = 0;
          check_hd_program(p_handle);
        }
        break;
      case PB_SM_MONITOR_SIGNAL:
        if(p_data->status == PB_STOP)
        {
          if(((mtos_ticks_get() - p_data->pat_monitor_start) > (p_data->pat_monitor_interval / 3))
            && (!p_data->pat_request_flag))
          {
            pat_request(p_handle,  p_data->ts_in);
            p_data->pat_request_flag = 1;
            p_data->pat_monitor_start = mtos_ticks_get();
            p_data->pat_monitor_interval = PAT_MONITOR_INTERVAL;
          }
          return ;
        }
        
        if((mtos_ticks_get() - p_data->signal_monitor_start) > 50) // 50ticks
        {
          p_data->signal_monitor_start = mtos_ticks_get();
          monitor(p_handle);
        }

        if(((mtos_ticks_get() - p_data->pat_monitor_start) > p_data->pat_monitor_interval)
          && (!p_data->pat_request_flag))
        {
          pat_request(p_handle,  p_data->ts_in);
          p_data->pat_request_flag = 1;
          p_data->pat_monitor_start = mtos_ticks_get();
          p_data->pat_monitor_interval = PAT_MONITOR_INTERVAL;
        }

         if(((mtos_ticks_get() - p_data->nit_monitor_start) > NIT_MONITOR_INTERVAL)
          && (!p_data->nit_request_flag))
          //&& (p_data->play_param.lock_mode == SYS_DVBC))// fix bug 20982
        {
          nit_request(p_handle,  p_data->ts_in);
          p_data->nit_request_flag = 1;
          p_data->nit_monitor_start = mtos_ticks_get();
        }
        break;
      case PB_SM_WAIT_STOP:
        p_data->sm = PB_SM_IDLE;
        break;
      default:
        break;
      }
    }
}

/*!
  Initialization function.

  \param[in] p_handle playback handle
  */
static void init(handle_t handle)
{
  pb_handle_t *p_handle = (pb_handle_t *)handle; 
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  nim_ctrl_t *p_nim = class_get_handle_by_id(NC_CLASS_ID);
  u8 tuner_id = 0, ts_in = 0;
  class_handle_t nc_handle = NULL;
  class_handle_t subt_handle = NULL;
 
  p_handle->data.sm = PB_SM_IDLE;
  p_handle->data.status = PB_STOP;
  p_handle->data.is_waiting_pmt = FALSE;
  p_handle->data.pat_monitor_start = mtos_ticks_get();
  p_handle->data.pat_monitor_interval = 0;
  p_handle->data.pat_request_flag = 0;
  p_handle->data.nit_monitor_start = mtos_ticks_get();
  p_handle->data.nit_request_flag = 0;
  p_handle->data.p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_PLAYBACK);
  p_handle->data.p_nim_svc = nc_get_svc_instance(p_nim, APP_PLAYBACK);

   if(p_handle->p_policy->is_subtitle_on == TRUE)
    {
       init_sub_ttx_1();
       nc_handle = class_get_handle_by_id(NC_CLASS_ID);
       nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);       
       vbi_set_ts_in(ts_in);
       subt_handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);
       start_subt_dec_1(subt_handle);
       p_handle->data.p_sub_ttx_data = subt_handle;
    }
   else
    {
      p_handle->data.p_sub_ttx_data = NULL;
    }
   
  
  p_handle->data.p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                             SYS_DEV_TYPE_AUDIO);
  CHECK_FAIL_RET_VOID(p_handle->data.p_audio_dev != NULL);
  p_handle->data.p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                             SYS_DEV_TYPE_VDEC_VSB);
  p_handle->data.p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                             SYS_DEV_TYPE_DISPLAY);  
  CHECK_FAIL_RET_VOID(p_handle->data.p_video_dev != NULL);
  p_handle->data.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                           SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_VOID(p_handle->data.p_dmx_dev != NULL);
}

/*!
  get how long time later to receive msg.

  \param[in] p_handle playback handle
  \return Return waiting time (ms)
  */
static u32 get_msgq_timeout(class_handle_t handle)
{
  pb_handle_t *p_handle = (pb_handle_t *)handle;
  return ((PB_SM_WAIT_VIDEO == p_handle->data.sm) ? 50 : DEFAULT_MSGQ_TIMEOUT);
}

/*!
  Call this method, load playback instance by the policy.

  \param[in] p_policy The policy of application playback
  \return Return app_t instance address
  */
app_t *construct_ap_playback_1(pb_policy_t *p_policy)
{
  pb_handle_t *p_handle = mtos_malloc(sizeof(pb_handle_t));
  
  CHECK_FAIL_RET_NULL((p_handle != NULL));
  p_handle->p_policy = p_policy;

  if(NULL == p_policy)
  {
    p_handle->p_policy = construct_def_ap_playback();
  }
  
  memset(&(p_handle->data), 0, sizeof(priv_data_t));
  memset(&(p_handle->instance), 0, sizeof(app_t));
  p_handle->data.a_chanid = INVAILD_CHANNEL_ID;
  p_handle->data.v_chanid = INVAILD_CHANNEL_ID;
  p_handle->data.p_chanid = INVAILD_CHANNEL_ID;
  p_handle->instance.init = init;
  p_handle->instance.task_single_step = pb_single_step;
  p_handle->instance.get_msgq_timeout = get_msgq_timeout;
  p_handle->instance.p_data = (void *)p_handle;

  return &(p_handle->instance);
}

