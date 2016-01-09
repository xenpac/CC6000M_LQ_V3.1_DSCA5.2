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
#include "dvbc_util.h"
#include "dvbt_util.h"


#include "ap_framework.h"
#include "ap_scan.h"

#include "ap_scan_priv.h"
#include "ap_dvbs_scan.h"


//#ifdef PRINT_ON
#define TP_DEBUG_ON
//#endif
#define WARRIORS_PLATFORM

#ifdef TP_DEBUG_ON
#define TP_DBG OS_PRINTF
#else
#define TP_DBG DUMMY_PRINTF
#endif

#ifdef WARRIORS_PLATFORM
#define RETRY_NUM   2
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
    current scan mode
    */
  BOOL nit_scan_mode;
#ifdef WARRIORS_PLATFORM
  /*!
    aviod lock fail
    */
  BOOL b_need_relock;
   /*!
    retry total number
    */
  u16 b_need_retry;
   /*!
    current count
    */
  u16 retry_count;  
  /*!
    save prev tp index in found tp list
    */
  u16 saved_idx_found_list;
#endif
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
    tuner id
    */
  u8 tuner_id;
  /*!
    ts in
    */
  u8 ts_in;  
} dvbc_scan_priv_t;

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

/*!
  Notify tp scan state
  */
static void notify_scan_state(u32 event_id)
{
  event_t evt = {0};
  evt.id = event_id;
  
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
}


static u32 progress_calc(dvbc_scan_priv_t *p_priv, u32 progress_base)
{ 
  u32 tp_unit = 0; 
  u32 progress = 0;
  if(p_priv->nit_scan_mode)  
  {
    tp_unit = 10000 / p_priv->total_tp_found_list;
    progress = tp_unit * p_priv->cur_idx_found_list +
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

  }
  else
  {
    tp_unit = 10000 / p_priv->input_data.total_tp;
    progress = tp_unit * (p_priv->cur_idx_preset_list - 1) +
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
  }
  return progress;
}



static RET_CODE on_start(dvbc_scan_priv_t *p_priv)
{
  p_priv->scan_progress = 0;
  p_priv->cur_idx_found_list = 0;
  p_priv->total_tp_found_list = 0;
  p_priv->total_tp_from_nit = 0;
  p_priv->cur_idx_preset_list = 0;
  if(p_priv->hook.on_start != NULL)
    p_priv->hook.on_start();
  if(p_priv->hook.on_new_sat!= NULL)
    p_priv->hook.on_new_sat(&p_priv->input_data.sat_list[0]);

  {
    class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
    dvb_t * p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);
    service_t *p_dvb_svc = p_dvb_handle->get_svc_instance(p_dvb_handle, APP_SCAN);
    nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
    p_dvb_svc->do_cmd(p_dvb_svc, DVB_TABLE_TS_UPDATE, 0,  p_priv->ts_in);
  }

  return SUCCESS;
}

static BOOL is_tp_exist(scan_tp_info_i_t *p_tp_list, u16 total, scan_tp_info_i_t *p_tp_info)
{
  scan_tp_info_i_t *p_cur_tp = NULL;
  tp_rcv_para_t *p_tp_para = &p_tp_info->tp_info;
  tp_rcv_para_t *p_cur_tp_para = NULL;
  u16 tp_cnt = 0;

  for(tp_cnt = 0; tp_cnt < total; tp_cnt++)
  {
    p_cur_tp = p_tp_list + tp_cnt;
    p_cur_tp_para = &p_cur_tp->tp_info;

    if(p_cur_tp_para->freq == p_tp_para->freq
        && p_cur_tp_para->sym == p_tp_para->sym
        && p_cur_tp_para->nim_modulate == p_tp_para->nim_modulate)
    {
      return TRUE;
    }
  }

  return FALSE;
}

static RET_CODE on_bat(dvbc_scan_priv_t *p_priv, os_msg_t *p_msg)
{
  bat_t *p_bat = (bat_t *)p_msg->para1;

  if(p_priv->input_data.bat_scan_type == BAT_SCAN_WITHOUT)
  {
    return SUCCESS;
  }

  if(p_priv->input_data.bat_scan_type == BAT_SCAN_ONCE
    && (p_bat->sec_number == p_bat->last_sec_number))
  {
    p_priv->input_data.bat_scan_type = BAT_SCAN_WITHOUT;
  }
  return SUCCESS;
}

static RET_CODE on_nit(dvbc_scan_priv_t *p_priv, os_msg_t *p_msg)
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
    cable_tp_info_t *p_nit_tp = NULL;
    scan_tp_info_i_t *p_last_tp = p_priv->found_tp_list +
      p_priv->total_tp_found_list;  //add to the end of found list
    tp_rcv_para_t *p_last_tp_para = &p_last_tp->tp_info;
    
    if(p_priv->input_data.nit_scan_type == NIT_SCAN_ALL_TP)
    {
      p_last_tp->enable_nit = TRUE;
    }
    
    if(p_nit->tp_svc_list[nit_tp_cnt].tp_type == NIT_DVBC_TP)
    {
      p_nit_tp = &p_nit->tp_svc_list[nit_tp_cnt].dvbc_tp_info;

      if((bcd_number_to_dec(p_nit_tp->frequency)/10) <= 256 || 
        (bcd_number_to_dec(p_nit_tp->symbol_rate)/10) <= 20)
      {
          //Invalid tp information
          continue;
      }

      //sat id is the same to current tp
      p_last_tp_para->freq = bcd_number_to_dec(p_nit_tp->frequency)/10;
      p_last_tp_para->sym = bcd_number_to_dec(p_nit_tp->symbol_rate)/10;
      //p_last_tp_para->nim_modulate = p_nit_tp->modulation;
   //convert the QAM table 
   switch(p_nit_tp->modulation)
   {
      case 1:
      p_last_tp_para->nim_modulate = NIM_MODULA_QAM16;
      break;
      
    case 2:
      p_last_tp_para->nim_modulate = NIM_MODULA_QAM32;
      break;

    case 3:
      p_last_tp_para->nim_modulate = NIM_MODULA_QAM64;      
      break;

    case 4:
      p_last_tp_para->nim_modulate = NIM_MODULA_QAM128;
      break;

    case 5:
      p_last_tp_para->nim_modulate = NIM_MODULA_QAM256;
      break;
    /*
    case 6:
      p_last_tp_para->nim_modulate = 0xFF;//reserved for future use
      break; */
    default:
      p_last_tp_para->nim_modulate = NIM_MODULA_QAM64;
      
  }
   

      //22k is not sure, using current, 
      //it will be try in the function tp_scan_get_tp_info
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

  if(p_priv->input_data.nit_scan_type == NIT_SCAN_ONCE
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

static RET_CODE dvbc_scan_msg_proc(void *p_data, os_msg_t *p_msg)
{
  RET_CODE ret =  SUCCESS;
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_data;
  scan_tp_info_i_t *p_tp = p_priv->found_tp_list + p_priv->cur_idx_found_list;
  
  switch(p_msg->content)
  {
    case NC_EVT_LOCKED:
#ifdef WARRIORS_PLATFORM
      if(p_priv->b_need_relock)
      {
        //locked, next tp
        OS_PRINTF("%s,LOCKED [%d]\n", __FUNCTION__, p_priv->b_need_retry);
        if(p_priv->b_need_retry)
        {
          p_priv->b_need_retry = FALSE;
          break;
        }
        p_priv->b_need_retry = FALSE;
      }
#endif
      p_tp->can_locked = 1;
      p_tp->sat_id = p_priv->input_data.sat_list[0].sat_id;
      p_priv->hook.process_tp(p_tp);
      //Notify progress
      notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS0));
      notify_scan_state(SCAN_EVT_TP_LOCKED);
      break;
    case NC_EVT_UNLOCKED:
#ifdef WARRIORS_PLATFORM
      if(p_priv->b_need_relock && p_priv->retry_count < RETRY_NUM)
      {
        //unlocked,please retry.
        OS_PRINTF("%s,UNLOCKED,freq[%d],retry [%d]\n", __FUNCTION__, p_tp->tp_info.freq,
           p_priv->retry_count);
        p_priv->b_need_retry = TRUE;
        break;
      }
#endif
      p_tp->can_locked = 0;
      p_priv->hook.process_tp(p_tp);
      //Notify progress
      notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS1));
      notify_scan_state(SCAN_EVT_TP_UNLOCKED);
      p_priv->cur_idx_found_list++; //tp unlock goto the next
      break;
    case DVB_PAT_FOUND: 
      break;
    case DVB_BAT_FOUND:
      ret = on_bat(p_priv, p_msg);
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
    case SCAN_CMD_START_SCAN:
      ret = on_start(p_priv);
      break;
    case DVB_CAT_FOUND:
      p_priv->hook.process_cat((void *)p_msg->para1);
      break;
#ifdef WARRIORS_PLATFORM
    case DVB_TABLE_TIMED_OUT:
      if(p_priv->b_need_relock)
      {
        OS_PRINTF("%s,TIMED_OUT,freq[%d],retry [%d]\n", __FUNCTION__, p_tp->tp_info.freq,
           p_priv->retry_count);
      }
      break;
#endif
    default:
      break;
  }
  return ret;
}

static find_tp_ret_t dvbc_scan_get_tp_info(void *p_data,
  nc_channel_info_t *p_channel)
{
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_data;

#ifdef WARRIORS_PLATFORM
  if(p_priv->b_need_relock)
  {
    if(p_priv->b_need_retry)
    {
      p_priv->retry_count ++;
      OS_PRINTF("%s,retry [%d]\n", __FUNCTION__, p_priv->retry_count);
      if(p_priv->retry_count <= RETRY_NUM)
      {
        p_priv->cur_idx_found_list = p_priv->saved_idx_found_list;
      }
      else
        p_priv->retry_count = 0;
    }
    else
      p_priv->retry_count = 0;
  }
#endif
  while(1)
  {
    if(p_priv->cur_idx_found_list < p_priv->total_tp_found_list)
    {
      //There are some nit tp or preset tp in the found tp list
      scan_tp_info_i_t *p_cur_tp = p_priv->found_tp_list +
        p_priv->cur_idx_found_list;
      tp_rcv_para_t *p_tp_para = &p_cur_tp->tp_info;

      // init search info
      p_tp_para->nim_type = NIM_DVBC;
      p_channel->channel_info.nim_type = NIM_DVBC;
      p_channel->polarization = NIM_PORLAR_HORIZONTAL;
      p_channel->channel_info.frequency = p_tp_para->freq;

      p_channel->channel_info.param.dvbc.modulation = p_tp_para->nim_modulate;
      p_channel->channel_info.param.dvbc.symbol_rate = p_tp_para->sym;

      p_channel->channel_info.spectral_polar = NIM_IQ_AUTO;
      p_channel->channel_info.lock = NIM_CHANNEL_UNLOCK;

      
#ifdef WARRIORS_PLATFORM
      if(p_priv->b_need_relock)
      {
        OS_PRINTF("%s,b_need_retry [%d]\n", __FUNCTION__, p_priv->b_need_retry);
        if((!p_priv->b_need_retry) && (p_priv->hook.check_tp(p_channel) != SUCCESS))
        {
          //no need to lock, skip it
          p_priv->cur_idx_found_list++;
          continue;
        }
        else
        {
          OS_PRINTF("%s,tp [%d]\n", __FUNCTION__, p_channel->channel_info.frequency);
          p_priv->saved_idx_found_list = p_priv->cur_idx_found_list;
          return FIND_SUC;
        }
      }
      else
#endif
      {
        if(p_priv->hook.check_tp(p_channel) != SUCCESS)
        {
          //no need to lock, skip it
          p_priv->cur_idx_found_list++;
          continue;
        }
        else
        {
          return FIND_SUC;
        }
      }
    }
    else if(p_priv->cur_idx_preset_list < p_priv->input_data.total_tp)
    {
      //copy to the end of found list
      scan_tp_info_i_t *p_last_tp = p_priv->found_tp_list +
        p_priv->total_tp_found_list;
        
      memcpy(p_last_tp,
        p_priv->input_data.tp_list + p_priv->cur_idx_preset_list,
        sizeof(scan_tp_info_i_t));
      p_last_tp->tp_origin = TP_FROM_NIT;
      p_priv->total_tp_found_list++;
      p_priv->cur_idx_preset_list++;
      continue;
    }
    else
    {
      notify_progress(100);
      return FIND_DONE;
    }
  }
}

static RET_CODE dvbc_scan_tp_done(void *p_data, void *p_tp_info)
{
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_data;
  scan_tp_info_t *p_tp = (scan_tp_info_t *)p_tp_info;
  scan_tp_info_i_t tp_info = {{0}};
  
  //Notify progress
  notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS3));
  
  tp_info.pg_num = p_tp->pg_num;
  tp_info.p_pg_list = p_tp->p_pg_list;
  p_priv->cur_idx_found_list++;

  return p_priv->hook.process_pg_list(&tp_info);
}

static void * dvbc_require_runingtime_buffer(void *p_data, u32 size)
{
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_data;
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

static scan_scramble_origin_t dvbc_get_scan_scramble_origin(void *p_policy)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_bl_scan_policy->p_priv_data;

  return p_priv->input_data.scramble_scan_origin;
}

static BOOL dvbc_require_extend_table(void *p_policy, u8 table_id)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_bl_scan_policy->p_priv_data;
  scan_tp_info_i_t *p_tp_info = p_priv->found_tp_list + p_priv->cur_idx_found_list;
  BOOL ret_value = FALSE;


  switch(table_id)
  {
    case DVB_TABLE_ID_NIT_ACTUAL:
      if((p_priv->input_data.nit_scan_type != NIT_SCAN_WITHOUT) &&
        p_tp_info->enable_nit)
      {
        p_priv->nit_scan_mode = TRUE;
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
      if(p_priv->input_data.cat_scan_type != CAT_SCAN_WITHOUT)
      {
        ret_value = TRUE;
      }
      break;
    default:
      break;
  }
  return ret_value;
}

static u8 dvbc_get_bouquet_id_list(void *p_policy, void *p_list)
{
  scan_policy_t *p_scan_policy = p_policy;
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_scan_policy->p_priv_data;
  memcpy(p_list, p_priv->input_data.bouquet_id, p_priv->input_data.bouquet_num * sizeof(u16));
  return p_priv->input_data.bouquet_num;
}

static void destroy_dvbc_scan_policy(void *p_policy)
{
  scan_policy_t *p_tp_scan_policy = p_policy;
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_tp_scan_policy->p_priv_data;

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
}

static void dvbc_scan_get_scan_info(void *pdata, u8 *p_tuner_id, u8 *p_ts_in)
{
   class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
   dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)pdata;
   *p_tuner_id = p_priv->tuner_id;
   nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, p_ts_in);
}

//add for ap_scan_performance by jacky.bian begin
/*static void dvbc_scan_performance(void *p_data, perf_check_point_t check,u32 context1,u32 context2)
{
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_data;
  if(p_priv->hook.performance_hook != NULL)
  {
    p_priv->hook.performance_hook(check, context1, context2);
  }
}*/
//add for ap_scan_performance by jacky.bian end

static BOOL dvbc_is_no_filter(void *p_policy)
{  
  scan_policy_t *p_tp_scan_policy = p_policy;
  dvbc_scan_priv_t *p_priv = (dvbc_scan_priv_t *)p_tp_scan_policy->p_priv_data;
  if(p_priv->hook.is_no_filter)
  {
    return p_priv->hook.is_no_filter();
  }
  else
  {
    return FALSE;
  }
  return FALSE;
}

policy_handle_t construct_dvbc_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data)
{
  u32 size = sizeof(dvbc_scan_priv_t);
  u16 i = 0;
  dvbc_scan_priv_t *p_priv = p_hook->get_attach_buffer(size);
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
  p_priv->tp_scan_policy.msg_proc     = dvbc_scan_msg_proc;
  p_priv->tp_scan_policy.get_tp_info  = dvbc_scan_get_tp_info;
  p_priv->tp_scan_policy.on_tp_done   = dvbc_scan_tp_done;
  p_priv->tp_scan_policy.require_runingtime_buffer = 
    dvbc_require_runingtime_buffer;
  p_priv->tp_scan_policy.destroy_policy = destroy_dvbc_scan_policy;
  p_priv->tp_scan_policy.p_priv_data  = p_priv;
  p_priv->tp_scan_policy.require_extend_table = dvbc_require_extend_table;
  p_priv->tp_scan_policy.get_scan_scramble_origin = dvbc_get_scan_scramble_origin;
  p_priv->tp_scan_policy.repeat_tp_check = NULL;
  p_priv->tp_scan_policy.get_blindscan_ver = NULL;
  p_priv->tp_scan_policy.get_bouquet_id_list = dvbc_get_bouquet_id_list;
  p_priv->tp_scan_policy.get_scan_info = dvbc_scan_get_scan_info;
  p_priv->tp_scan_policy.is_no_pg_filter = dvbc_is_no_filter;
    
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
  return (policy_handle_t)&p_priv->tp_scan_policy;
}

#ifdef WARRIORS_PLATFORM
policy_handle_t construct_dvbc_scan_policy_warriors(scan_hook_t *p_hook,
  scan_preset_data_t *p_data)
{
  u32 size = sizeof(dvbc_scan_priv_t);
  u16 i = 0;
  dvbc_scan_priv_t *p_priv = p_hook->get_attach_buffer(size);
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

  //Attach scan policy
  p_priv->tp_scan_policy.msg_proc     = dvbc_scan_msg_proc;
  p_priv->tp_scan_policy.get_tp_info  = dvbc_scan_get_tp_info;
  p_priv->tp_scan_policy.on_tp_done   = dvbc_scan_tp_done;
  p_priv->tp_scan_policy.require_runingtime_buffer = 
    dvbc_require_runingtime_buffer;
  p_priv->tp_scan_policy.destroy_policy = destroy_dvbc_scan_policy;
  p_priv->tp_scan_policy.p_priv_data  = p_priv;
  p_priv->tp_scan_policy.require_extend_table = dvbc_require_extend_table;
  p_priv->tp_scan_policy.get_scan_scramble_origin = dvbc_get_scan_scramble_origin;
  p_priv->tp_scan_policy.repeat_tp_check = NULL;
  p_priv->tp_scan_policy.get_blindscan_ver = NULL;
  p_priv->tp_scan_policy.get_bouquet_id_list = dvbc_get_bouquet_id_list;
  p_priv->tp_scan_policy.is_no_pg_filter = dvbc_is_no_filter;
  p_priv->b_need_relock = TRUE;
  p_priv->b_need_retry = FALSE;
  p_priv->retry_count = 0;
  p_priv->saved_idx_found_list = 0;
    
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
  return (policy_handle_t)&p_priv->tp_scan_policy;
}
#endif

