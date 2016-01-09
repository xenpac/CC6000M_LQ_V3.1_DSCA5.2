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
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "class_factory.h"

#include "drv_dev.h"
#include "nim.h"
#include "dmx.h"

#include "mdl.h"
#include "err_check_def.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pmt.h"
#include "cat.h"
#include "bat.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"

#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_scan_priv.h"
#include "ap_dvbs_scan.h"


//#ifdef PRINT_ON
#define TP_DEBUG_ON
//#endif

#ifdef TP_DEBUG_ON
#define TP_DBG OS_PRINTF
#else
#define TP_DBG DUMMY_PRINTF
#endif

/*!
  Scan progress starting value is 1
  */
#define TP_SCAN_PROGRESS0  (1)
/*!
  Default scan progress1
  */
#define TP_SCAN_PROGRESS1 (18)
/*!
  Default scan progress2
  */
#define TP_SCAN_PROGRESS2 (36)
/*!
  Default scan progress3
  */
#define TP_SCAN_PROGRESS3 (64)
/*!
  Start frequency
  */
#define FREQ_MIN_LIMIT ((950)*(KHZ))
/*!
  End frequency
  */
#define FREQ_MAX_LIMIT ((2150) * (KHZ))
/*!
  Start frequency
  */
#define FREQ_MIN_LIMIT_LOW ((250)*(KHZ))
/*!
  End frequency
  */
#define FREQ_MAX_LIMIT_LOW ((950) * (KHZ))

/*!
  Universal low frequency
  */
#define UNS_LOW_LIMIT (11700)

/*!
  Private data used in TP scan
  */
typedef struct
{
  /*!
    my policy
    */
  scan_policy_t tp_scan_policy;
  /*!
    Tp list for saving tp information in TP scan
    */
  scan_tp_info_i_t found_tp_list[MAX_TP_NUM_SUPPORTED];
  /*!
    disc move remain time
    */
  u32 disp_move_start_time;
  /*!
    disc move remain time
    */
  u32 disp_move_time;
  /*!
    disc is move
    */
  BOOL b_disp_moving;
  /*!
    current progress
    */
  u32 scan_progress;
  /*!
    need free ap_scan runing time buffer
    */
  void *p_need_free_rt_buf;
  /*!
    need free tp_scan priv buffer
    */
  void *p_need_free_priv_buf;
  /*!
    Max satellite number supported
    */
  u16 cur_sat_idx;
  /*!
    Current tp index in found tp list
    */
  u16 cur_idx_found_list;
  /*!
    total tp num
    */
  u16 total_tp_found_list;
  /*!
    total tp num from nit
    */
  u16 total_tp_from_nit;
  /*!
    Current tp index in preset tp list
    */
  u16 cur_idx_preset_list;
  /*!
    hook
    */
  scan_hook_t hook;
  /*!
    user input data
    */
  scan_preset_data_t input_data;

  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;

  /*!
    tuner id
    */
  u8 tuner_id;
  /*!
    ts in
    */
  u8 ts_in;
} tp_scan_priv_t;

/*!
  Notify tp scan progress
  */
static void notify_progress(u32 progress)
{
  event_t evt;
  evt.id = SCAN_EVT_PROGRESS;
  evt.data1 = (u32)progress;

  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
}

static u32 progress_calc(tp_scan_priv_t *p_priv, u32 progress_base)
{
  u32 tp_unit = 10000 / p_priv->input_data.total_tp;
  u32 progress = tp_unit * (p_priv->cur_idx_preset_list - 1) +
    tp_unit * progress_base / 100;

  progress /= 100;

  if(progress > 100)
  {
    progress = 100;
  }

  if(progress <= p_priv->scan_progress)
  {
    progress = p_priv->scan_progress;
  }
  else
  {
    p_priv->scan_progress = progress;
  }

  return progress;
}

static void _do_switch(tp_scan_priv_t *p_priv, scan_sat_info_t *p_sat)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  BOOL need_waiting = FALSE;

  p_priv->tuner_id = p_sat->sat_info.tuner_index;
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);

  nc_enable_monitor(class_get_handle_by_id(NC_CLASS_ID), p_priv->tuner_id, FALSE);
  need_waiting = nc_set_diseqc(nc_handle, p_priv->tuner_id, &p_sat->diseqc_info);
  nc_set_12v(nc_handle, p_priv->tuner_id, (u8)p_sat->sat_info.v12);

  p_priv->hook.on_new_sat(p_sat);
  p_priv->disp_move_start_time = mtos_ticks_get() * 10;
  if(need_waiting)
  {
    p_priv->disp_move_time = p_priv->hook.get_switch_disq_time(p_sat);
  }
  else
  {
    p_priv->disp_move_time = 1 * 1000;
  }
  p_priv->b_disp_moving = TRUE;

  p_priv->p_dvb_svc->do_cmd(p_priv->p_dvb_svc, DVB_TABLE_TS_UPDATE, 0,  p_priv->ts_in);
}

static void switch_to_sat(tp_scan_priv_t *p_priv, u16 new_sat_id)
{
  u16 i = 0;
  scan_sat_info_t *p_sat = p_priv->input_data.sat_list;

  //found the next satellite
  for(i = 0; i < p_priv->input_data.total_sat; i++)
  {
    OS_PRINTF("switch_to_sat  sat id %d, new_id %d\n",
      p_sat->sat_id, new_sat_id);
    if(p_sat[i].sat_id == new_sat_id)
    {
      _do_switch(p_priv, p_sat + i);
      p_priv->cur_sat_idx = i;
      return;
    }
  }
  //it's an exception, if has satellite, must found
  CHECK_FAIL_RET_VOID(p_priv->input_data.total_sat == 0);
}

static RET_CODE on_start(tp_scan_priv_t *p_priv)
{
  dvb_t * p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);
  p_priv->p_dvb_svc =  p_dvb_handle->get_svc_instance(p_dvb_handle, APP_SCAN);

  p_priv->scan_progress = 0;
  p_priv->cur_sat_idx = 0;
  p_priv->cur_idx_found_list = 0;
  p_priv->total_tp_found_list = 0;
  p_priv->total_tp_from_nit = 0;
  p_priv->cur_idx_preset_list = 0;
  OS_PRINTF("$$$$ %s,line %d\n", __FUNCTION__,__LINE__);
  //nc_enable_monitor(class_get_handle_by_id(NC_CLASS_ID), p_priv->tuner_id, FALSE);
  //nc_disable_set_diseqc(class_get_handle_by_id(NC_CLASS_ID), tuner_id, TRUE);
  switch_to_sat(p_priv, p_priv->input_data.sat_list[0].sat_id);
  if(p_priv->hook.on_start != NULL)
  {
    p_priv->hook.on_start();
  }
  return SUCCESS;
}

static BOOL is_tp_exist(scan_tp_info_i_t *p_tp_list, u16 total, scan_tp_info_i_t *p_tp_info)
{
  scan_tp_info_i_t *p_cur_tp = NULL;
  tp_rcv_para_t *p_tp_para = &p_tp_info->tp_info;
  tp_rcv_para_t *p_cur_tp_para = NULL;
  u16 tp_cnt = 0;
  s32 deta_fre = 0;
  s32 deta_sym = 0;
  s32 deta_sym_limt = p_tp_para->sym / 10;
  s32 deta_fre_limit = (p_tp_para->sym > 3000) ? 5 : 3;


  for(tp_cnt = 0; tp_cnt < total; tp_cnt++)
  {
    p_cur_tp = p_tp_list + tp_cnt;
    p_cur_tp_para = &p_cur_tp->tp_info;

    deta_fre = (s32)(p_tp_para->freq - p_cur_tp_para->freq);
    deta_sym = (s32)(p_tp_para->sym - p_cur_tp_para->sym);

    if((ABS(deta_fre) <= deta_fre_limit)
        && (ABS(deta_sym) <= deta_sym_limt)
        && p_cur_tp_para->polarity == p_tp_para->polarity
        && p_cur_tp->sat_id == p_tp_info->sat_id)
    {
      return TRUE;
    }
  }

  return FALSE;
}

static RET_CODE on_nit(tp_scan_priv_t *p_priv, os_msg_t *p_msg)
{
  u8 nit_tp_cnt  = 0;
  nit_t *p_nit = (nit_t *)p_msg->para1;

  p_priv->total_tp_from_nit = 0;
  if(p_priv->input_data.nit_scan_type == NIT_SCAN_WITHOUT ||
      (p_nit->total_tp_num == 0))
  {
    return SUCCESS;
  }

  CHECK_FAIL_RET_CODE(p_nit->total_tp_num <= MAX_TP_NUM);
  for(nit_tp_cnt = 0; nit_tp_cnt < p_nit->total_tp_num; nit_tp_cnt++)
  {
    sat_tp_info_t *p_nit_tp = NULL;
    scan_tp_info_i_t *p_cur_tp = p_priv->found_tp_list +
      p_priv->cur_idx_found_list;
    scan_tp_info_i_t *p_last_tp = p_priv->found_tp_list +
      p_priv->total_tp_found_list;  //add to the end of found list
    tp_rcv_para_t *p_last_tp_para = &p_last_tp->tp_info;
    scan_sat_info_t *p_cur_sat = p_priv->input_data.sat_list +
      p_priv->cur_sat_idx;
    sat_rcv_para_t *p_sat_para = &p_cur_sat->sat_info;

    if(p_nit->tp_svc_list[nit_tp_cnt].tp_type == NIT_DVBS_TP)
    {
      p_nit_tp = &p_nit->tp_svc_list[nit_tp_cnt].dvbs_tp_info;
      OS_PRINTF("########nit tp dec [%d,%d,%d]\n",
        (bcd_number_to_dec(p_nit_tp->frequency)/100),
        (bcd_number_to_dec(p_nit_tp->symbol_rate)/10),
        p_nit_tp->polarization);

      if((bcd_number_to_dec(p_nit_tp->frequency)/100) <= 256 ||
        (bcd_number_to_dec(p_nit_tp->symbol_rate)/10) <= 20)
      {
          //Invalid tp information
          OS_PRINTF("********Invalid tp\n");
          continue;
      }

      //sat id is the same to current tp
      p_last_tp->sat_id = p_cur_tp->sat_id;
      p_last_tp_para->freq = bcd_number_to_dec(p_nit_tp->frequency)/100;
      p_last_tp_para->sym = bcd_number_to_dec(p_nit_tp->symbol_rate)/10;
      p_last_tp_para->polarity = p_nit_tp->polarization;
      p_last_tp_para->nim_type = NIM_DVBS_AUTO;
      p_last_tp_para->fec_inner = p_nit_tp->fec_inner;
      #if 0
      if(p_nit_tp->modulation_system == 1)
      {
        p_last_tp_para->nim_type = NIM_DVBS2;
      }
      else
      {
        p_last_tp_para->nim_type = NIM_DVBS;
      }
      #endif
      OS_PRINTF("p_last_tp_para->nim_type = %d\n", p_last_tp_para->nim_type);
      //22k is not sure, using current,
      //it will be try in the function tp_scan_get_tp_info
      if(DVBS_LNB_UNIVERSAL == p_sat_para->lnb_type)
      {
        if(p_last_tp_para->freq >= UNS_LOW_LIMIT)
          p_last_tp_para->is_on22k = 1;
        else
          p_last_tp_para->is_on22k = 0;
      }
      else
        p_last_tp_para->is_on22k = p_cur_tp->tp_info.is_on22k;
      p_last_tp->tp_origin = TP_FROM_NIT;
      if(is_tp_exist(p_priv->found_tp_list, p_priv->total_tp_found_list, p_last_tp) == FALSE)
      {
        if(is_tp_exist(p_priv->input_data.tp_list, p_priv->input_data.total_tp, p_last_tp) == FALSE)
        {
          p_priv->total_tp_found_list++;
          p_priv->total_tp_from_nit++;
        }
      }
    }
  }

  if((p_priv->input_data.nit_scan_type == NIT_SCAN_ONCE)
    && (p_nit->sec_number == p_nit->last_sec_number))
  {
    p_priv->input_data.nit_scan_type = NIT_SCAN_WITHOUT;
  }

  if(p_priv->hook.process_nit_info != NULL)
  {
    p_priv->hook.process_nit_info(p_nit);
  }
  return SUCCESS;
}
static RET_CODE on_bat(tp_scan_priv_t *p_priv, os_msg_t *p_msg)
{
  bat_t *p_bat = (bat_t *)p_msg->para1;

  if(p_priv->input_data.bat_scan_type == BAT_SCAN_WITHOUT)
  {
    return SUCCESS;
  }

  if(p_priv->input_data.bat_scan_type == BAT_SCAN_ONCE
    && p_bat->sec_number == p_bat->last_sec_number)
  {
    p_priv->input_data.bat_scan_type = BAT_SCAN_WITHOUT;
  }
  return SUCCESS;
}


static RET_CODE on_locked(tp_scan_priv_t *p_priv, nc_channel_info_t *p_ch)
{
  scan_tp_info_i_t *p_tp = p_priv->found_tp_list + p_priv->cur_idx_found_list;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  nim_channel_perf_t  perf = {0};

  RET_CODE ret = ERR_FAILURE;

  TP_DBG("tp scan: nim type driver %d, input %d\n",
    p_ch->channel_info.param.dvbs.nim_type, p_tp->tp_info.nim_type);
  nc_perf_get(nc_handle, p_priv->tuner_id, &perf);
  memcpy(&p_tp->perf, &perf, sizeof(nim_channel_perf_t));
  //if(p_tp->tp_info.nim_type == NIM_UNDEF)
    //p_tp->tp_origin = TP_FROM_SCAN;
  p_tp->tp_info.nim_type = p_ch->channel_info.param.dvbs.nim_type;
  p_tp->tp_info.fec_inner = p_ch->channel_info.param.dvbs.fec_inner;
  ret = p_priv->hook.process_tp(p_tp);

  return ret;
}

static RET_CODE tp_scan_msg_proc(void *p_data, os_msg_t *p_msg)
{
  RET_CODE ret =  SUCCESS;
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_data;
  scan_tp_info_i_t *p_tp = p_priv->found_tp_list + p_priv->cur_idx_found_list;
  u8 tuner_id = TUNER0;

  switch(p_msg->content)
  {
    case NC_EVT_LOCKED:
      tuner_id = p_msg->context;
      p_tp->can_locked = 1;
      ret = on_locked(p_priv, (nc_channel_info_t *)p_msg->para1);
      //Notify progress
      notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS1));
      break;
    case NC_EVT_UNLOCKED:
      tuner_id = p_msg->context;
      p_tp->can_locked = 0;
      p_priv->hook.process_tp(p_tp);
      //Notify progress
      notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS1));
      p_priv->cur_idx_found_list++; //tp unlock goto the next
      break;
    case DVB_PAT_FOUND:
      //Notify progress
      notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS2));
      break;
    case DVB_NIT_FOUND:
      ret = on_nit(p_priv, p_msg);
      {
        event_t evt;
        evt.id = SCAN_EVT_NIT_FOUND;
        evt.data1 = (u32)p_priv->total_tp_from_nit;
        evt.data2 = ((nit_t *)p_msg->para1)->version_num;
        ap_frm_send_evt_to_ui(APP_SCAN, &evt);
      }
      break;
    case DVB_BAT_FOUND:
      ret = on_bat(p_priv, p_msg);
      break;
    case DVB_CAT_FOUND:
      ret = p_priv->hook.process_cat((void *)(p_msg->para1));
      break;
    case SCAN_CMD_START_SCAN:
      ret = on_start(p_priv);
      break;
    default:
      break;
  }
  return ret;
}

static find_tp_ret_t tp_scan_get_tp_info(void *p_data,
  nc_channel_info_t *p_channel)
{
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_data;
  u32 tp_freq_min = 0;
  u32 tp_freq_max = 0;

  if(p_priv->b_disp_moving)
  {
    u32 interval = (mtos_ticks_get() * 10) - p_priv->disp_move_start_time;
    if(interval > p_priv->disp_move_time)
    {
      //Moving Done
      p_priv->b_disp_moving = FALSE;
    }
    else
    {
      //Skeep Moving
      return FIND_BUSY;
    }
  }

  while(1)
  {
    if(p_priv->cur_idx_found_list < p_priv->total_tp_found_list)
    {
      //There are some nit tp or preset tp in the found tp list
      scan_tp_info_i_t *p_cur_tp = p_priv->found_tp_list +
        p_priv->cur_idx_found_list;
      scan_sat_info_t *p_cur_sat = p_priv->input_data.sat_list +
        p_priv->cur_sat_idx;
      sat_rcv_para_t *p_sat_para = &p_cur_sat->sat_info;
      tp_rcv_para_t *p_tp_para = &p_cur_tp->tp_info;

      if(p_cur_tp->sat_id != p_cur_sat->sat_id)  //the satellite changed
      {
        switch_to_sat(p_priv, p_cur_tp->sat_id);
        return FIND_BUSY;
      }

      tp_freq_min = p_sat_para->tuner_type == 1 ? FREQ_MIN_LIMIT_LOW: FREQ_MIN_LIMIT;
      tp_freq_max = p_sat_para->tuner_type == 1 ? FREQ_MAX_LIMIT_LOW : FREQ_MAX_LIMIT;
      dvbs_calc_search_info(p_channel, p_sat_para, p_tp_para);
      //check the tp 22konoff which come from NIT
      if(p_channel->channel_info.frequency < tp_freq_min ||
        p_channel->channel_info.frequency > tp_freq_max)
      {
        //p_sat_para->k22 = (p_sat_para->k22 == 1) ? 0 : 1;
        p_tp_para->is_on22k = (p_tp_para->is_on22k == 1) ? 0 : 1;
        dvbs_calc_search_info(p_channel, p_sat_para, p_tp_para);
      }

      if(p_priv->hook.check_tp(p_channel) != SUCCESS)
      {
        //no need to lock, skip it
        p_priv->cur_idx_found_list++;
        continue;
      }
      else
      { // for dvbs.dvbs2. need blind scan the nim type
#if 0
        if(p_channel->channel_info.param.dvbs.nim_type == NIM_UNDEF)
        {
          nc_set_blind_scan_mode(class_get_handle_by_id(NC_CLASS_ID), TRUE);
        }
        else
#endif
        {
          OS_PRINTF("%s\n",__FUNCTION__,__LINE__);
          //nc_set_blind_scan_mode(class_get_handle_by_id(NC_CLASS_ID), tuner_id,
          //  FALSE);
        }

       return FIND_SUC;
      }
    }
    else if(p_priv->cur_idx_preset_list < p_priv->input_data.total_tp)
    {
      if(is_tp_exist(p_priv->found_tp_list, p_priv->total_tp_found_list,
          p_priv->input_data.tp_list + p_priv->cur_idx_preset_list))
      {
        p_priv->cur_idx_preset_list++;
      }
      else
      {
      //copy to the end of found list
      scan_tp_info_i_t *p_last_tp = p_priv->found_tp_list +
        p_priv->total_tp_found_list;

      memcpy(p_last_tp,
        p_priv->input_data.tp_list + p_priv->cur_idx_preset_list,
        sizeof(scan_tp_info_i_t));
      p_last_tp->tp_origin = TP_FROM_PRESET;
      p_priv->total_tp_found_list++;
      p_priv->cur_idx_preset_list++;
      }
      continue;
    }
    else
    {
      notify_progress(100);
      return FIND_DONE;
    }
  }
}

static RET_CODE tp_scan_tp_done(void *p_data, void *p_tp_info)
{
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_data;
  scan_tp_info_t *p_tp = (scan_tp_info_t *)p_tp_info;
  scan_tp_info_i_t tp_info = {{0}};

  //Notify progress
  notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS3));

  tp_info.pg_num = p_tp->pg_num;
  tp_info.p_pg_list = p_tp->p_pg_list;
  p_priv->cur_idx_found_list++;

  return p_priv->hook.process_pg_list(&tp_info);
}

static void * tp_require_runingtime_buffer(void *p_data, u32 size)
{
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_data;
  void *p_buffer = p_priv->hook.get_attach_buffer(size);

  if(p_buffer != NULL)  //using attach buf
  {
    p_priv->p_need_free_rt_buf = NULL;
  }
  else
  {
    p_buffer = mtos_malloc(size);
    CHECK_FAIL_RET_NULL(p_buffer != NULL);
    p_priv->p_need_free_rt_buf = p_buffer;
  }

  return p_buffer;
}


static scan_scramble_origin_t tp_get_scan_scramble_origin(void *p_policy)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_bl_scan_policy->p_priv_data;

  return p_priv->input_data.scramble_scan_origin;
}

static BOOL tp_require_extend_table(void *p_policy, u8 table_id)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_bl_scan_policy->p_priv_data;
  scan_tp_info_i_t *p_tp_info = p_priv->found_tp_list + p_priv->cur_idx_found_list;
  BOOL ret_value = FALSE;

  switch(table_id)
  {
    case DVB_TABLE_ID_NIT_ACTUAL:
      if((p_priv->input_data.nit_scan_type != NIT_SCAN_WITHOUT) &&
        p_tp_info->enable_nit)
      {
        OS_PRINTF("p_tp_info[%d],freq[%d],nim[%d]\n",p_priv->cur_idx_found_list,
          p_tp_info->tp_info.freq, p_tp_info->tp_info.nim_type);
        ret_value = TRUE;
      }
      break;
   case DVB_TABLE_ID_BAT:
      if(p_priv->input_data.bat_scan_type != BAT_SCAN_WITHOUT)
      {
        ret_value = TRUE;
      }
      break;
    case DVB_TABLE_ID_CAT:
      if(p_priv->input_data.cat_scan_type!= CAT_SCAN_WITHOUT)
      {
        ret_value = TRUE;
      }
      break;
    default:
      break;
  }
  return ret_value;
}

static u8 tp_scan_get_bouquet_id_list(void *p_policy, void *p_list)
{
  scan_policy_t *p_scan_policy = p_policy;
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_scan_policy->p_priv_data;
  memcpy(p_list, p_priv->input_data.bouquet_id, p_priv->input_data.bouquet_num * sizeof(u16));
  return p_priv->input_data.bouquet_num;
}

static void tp_scan_get_scan_info(void *pdata, u8 *p_tuner_id, u8 *p_ts_in)
{
   class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
   tp_scan_priv_t *p_priv = (tp_scan_priv_t *)pdata;
   *p_tuner_id = p_priv->tuner_id;
   nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, p_ts_in);
}

static void destroy_tp_scan_policy(void *p_policy)
{
  scan_policy_t *p_tp_scan_policy = p_policy;
  tp_scan_priv_t *p_priv = (tp_scan_priv_t *)p_tp_scan_policy->p_priv_data;
  u8 tuner_id = p_priv->tuner_id;

  //nc_disable_set_diseqc(class_get_handle_by_id(NC_CLASS_ID), tuner_id, FALSE);
  p_priv->hook.free_resource();
  //Release private data
  if(p_priv->p_need_free_rt_buf != NULL)
  {
    mtos_free(p_priv->p_need_free_rt_buf);
  }
  if(p_priv->p_need_free_priv_buf != NULL)
  {
    mtos_free(p_priv->p_need_free_priv_buf);
  }
  OS_PRINTF("$$$$ %s,line %d\n", __FUNCTION__,__LINE__);
  nc_enable_monitor(class_get_handle_by_id(NC_CLASS_ID), tuner_id, TRUE);
}

policy_handle_t construct_tp_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data)
{
  u32 size = sizeof(tp_scan_priv_t);
  u16 i = 0;
  tp_scan_priv_t *p_priv = p_hook->get_attach_buffer(size);
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  if(p_priv != NULL)    //using attach buf
  {
    memset(p_priv, 0, size);
  }
  else
  {
    p_priv = mtos_malloc(size);
    CHECK_FAIL_RET_NULL(p_priv != NULL);
    memset(p_priv, 0, size);
    p_priv->p_need_free_priv_buf = p_priv;
  }

  p_priv->tuner_id = p_data->sat_list[0].sat_info.tuner_index;
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);

  //Attach scan policy
  p_priv->tp_scan_policy.msg_proc     = tp_scan_msg_proc;
  p_priv->tp_scan_policy.get_tp_info  = tp_scan_get_tp_info;
  p_priv->tp_scan_policy.on_tp_done   = tp_scan_tp_done;
  p_priv->tp_scan_policy.require_runingtime_buffer =
    tp_require_runingtime_buffer;
  p_priv->tp_scan_policy.destroy_policy = destroy_tp_scan_policy;
  p_priv->tp_scan_policy.p_priv_data  = p_priv;
  p_priv->tp_scan_policy.require_extend_table = tp_require_extend_table;
  p_priv->tp_scan_policy.get_scan_scramble_origin = tp_get_scan_scramble_origin;
  //p_priv->tp_scan_policy.performance = tp_scan_performance;
  p_priv->tp_scan_policy.repeat_tp_check = NULL;
  p_priv->tp_scan_policy.get_blindscan_ver = NULL;
  p_priv->tp_scan_policy.get_bouquet_id_list = tp_scan_get_bouquet_id_list;
  p_priv->tp_scan_policy.get_scan_info = tp_scan_get_scan_info;

  //Attach implementation policy
  memcpy(&p_priv->hook, p_hook, sizeof(scan_hook_t));

  //Load parameter
  CHECK_FAIL_RET_NULL(p_priv->input_data.total_tp <= MAX_TP_NUM_SUPPORTED);
  memcpy(&p_priv->input_data, p_data, sizeof(scan_preset_data_t));

  if(p_priv->input_data.nit_scan_type != NIT_SCAN_WITHOUT)
  {
    //Full scan or tp scan
    for(i = 0; i < p_priv->input_data.total_tp; i++)
    {
      p_priv->input_data.tp_list[i].enable_nit = TRUE;
    }
  }

  for(i = 0; i < p_priv->input_data.total_tp; i++)
  {
    OS_PRINTF("$$$$ input_tp: freq %d, Sym %d, pol %d, 22k %d, nim_type %d, sat %d, nit %d\n",
      p_priv->input_data.tp_list[i].tp_info.freq,
      p_priv->input_data.tp_list[i].tp_info.sym,
      p_priv->input_data.tp_list[i].tp_info.polarity,
      p_priv->input_data.tp_list[i].tp_info.is_on22k,
      p_priv->input_data.tp_list[i].tp_info.nim_type,
      p_priv->input_data.tp_list[i].sat_id,
      p_priv->input_data.tp_list[i].enable_nit
      );
  }

  return (policy_handle_t)&p_priv->tp_scan_policy;
}

