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
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"


#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_scan_priv.h"
#include "ap_dvbs_scan.h"


#ifdef PRINT_ON
#define TP_DEBUG_ON
#endif

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
    dvbt param
   */
  nim_dvbt_param_t  dvbt_param;
  /*!
    logical channel number.
    */
  logical_channel_descriptor_t lcd[MAX_LCD_NUM];  
  /*!
    logical channel count.
    */
  u16 lcd_cnt;
  /*!
    current plp id 
    */
  u8 cur_plp_id;
  /*!
    current index in plp array 
    */
  u8 cur_plp_index;
  /*!
    search by plp id
    */
  BOOL b_search_by_plp;
  /*!
    tuner id
    */
  u8 tuner_id;
  /*!
    ts in
    */
  u8 ts_in;
  service_t *p_dvb_svc;
  /*!
    hook
    */
  scan_hook_t hook;
  /*!
    user input data
    */
  scan_preset_data_t input_data;
} dvbt_scan_priv_t;

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

static u32 progress_calc(dvbt_scan_priv_t *p_priv, u32 progress_base)
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

static void _do_switch(dvbt_scan_priv_t *p_priv, scan_sat_info_t *p_sat)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  p_priv->tuner_id = p_sat->sat_info.tuner_index;
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
  p_priv->hook.on_new_sat(p_sat);
  p_priv->p_dvb_svc->do_cmd(p_priv->p_dvb_svc, DVB_TABLE_TS_UPDATE, 0,  p_priv->ts_in);
}

static void switch_to_sat(dvbt_scan_priv_t *p_priv, u16 new_sat_id)
{
  u16 i = 0;
  scan_sat_info_t *p_sat = p_priv->input_data.sat_list;

  //found the next satellite
  for(i = 0; i < p_priv->input_data.total_sat; i++)
  {
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

static RET_CODE on_start(dvbt_scan_priv_t *p_priv)
{
  dvb_t * p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);
  p_priv->p_dvb_svc =  p_dvb_handle->get_svc_instance(p_dvb_handle, APP_SCAN);
  p_priv->scan_progress = 0;
  p_priv->cur_sat_idx = 0;
  p_priv->cur_idx_found_list = 0;
  p_priv->total_tp_found_list = 0;
  p_priv->total_tp_from_nit = 0;
  p_priv->cur_idx_preset_list = 0;
  nc_enable_monitor(class_get_handle_by_id(NC_CLASS_ID), p_priv->tuner_id, FALSE);
  switch_to_sat(p_priv, p_priv->input_data.sat_list[0].sat_id);
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
    if((p_tp_para->freq == p_cur_tp_para->freq) && (p_tp_para->sym == p_cur_tp_para->sym))
    {
      TP_DBG("%s,nit tp same\n",__FUNCTION__);
      return TRUE;
    }
  }

  return FALSE;
}

static RET_CODE on_nit(dvbt_scan_priv_t *p_priv, os_msg_t *p_msg)
{
  u8 nit_tp_cnt  = 0;
  nit_t *p_nit = (nit_t *)p_msg->para1;
  scan_tp_info_i_t *p_cur_tp = p_priv->found_tp_list +
    p_priv->cur_idx_found_list - 1;

  if(p_cur_tp->enable_lcn)
  {
    p_priv->lcd_cnt = p_nit->lcd_cnt;
    memcpy(p_priv->lcd, p_nit->lcd, sizeof(logical_channel_descriptor_t) * MAX_LCD_NUM);
  }
  if(!p_cur_tp->enable_nit)
  {
    TP_DBG("%s,enable_nit false\n", __FUNCTION__);
    return SUCCESS;
  }

  if(p_nit->total_tp_num == 0)
  {
    TP_DBG("%s,nit tp not found\n", __FUNCTION__);
    return SUCCESS;
  }
  CHECK_FAIL_RET_CODE(p_nit->total_tp_num <= MAX_TP_NUM);
  for(nit_tp_cnt = 0; nit_tp_cnt < p_nit->total_tp_num; nit_tp_cnt++)
  {
    terrestrial_2_tp_info_t *p_nit_tp = NULL;
    scan_tp_info_i_t *p_cur_tp = p_priv->found_tp_list +
      p_priv->cur_idx_found_list - 1;
    scan_tp_info_i_t *p_last_tp = p_priv->found_tp_list +
      p_priv->total_tp_found_list;  //add to the end of found list
    tp_rcv_para_t *p_last_tp_para = &p_last_tp->tp_info;

    if(p_nit->tp_svc_list[nit_tp_cnt].tp_type == NIT_DVBT2_TP)
    {
      p_nit_tp = &p_nit->tp_svc_list[nit_tp_cnt].t2_tp_info;

      //sat id is the same to current tp
      p_last_tp->sat_id = p_cur_tp->sat_id;
      p_last_tp_para->freq = p_nit_tp->centre_frequency / 100;
      p_last_tp_para->sym = p_nit_tp->bandwidth;
      //p_last_tp_para->nim_type = NIM_UNDEF;
      p_last_tp_para->nim_type = NIM_DVBT_AUTO;      
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
  return SUCCESS;
}

static RET_CODE on_locked(dvbt_scan_priv_t *p_priv, nc_channel_info_t *p_ch)
{    
  scan_tp_info_i_t *p_tp = p_priv->found_tp_list + p_priv->cur_idx_found_list - 1;
  u8 i = 0;

  RET_CODE ret = ERR_FAILURE;
        
  TP_DBG("dvbt scan: nim type driver %d, input %d\n",
    p_ch->channel_info.param.dvbt.nim_type, p_tp->tp_info.nim_type);
  TP_DBG("dvbt scan: tp_origin %d\n",
    p_tp->tp_origin);
  p_tp->tp_info.nim_type = p_ch->channel_info.param.dvbt.nim_type;
  //only one sat id
  p_tp->sat_id = p_priv->input_data.sat_list[0].sat_id;
  TP_DBG("dvbt scan: sat_id %d\n",
    p_tp->sat_id);
  if(p_tp->tp_info.nim_type == NIM_DVBT)
  {
    ret = p_priv->hook.process_tp(p_tp);
    //p_priv->cur_idx_found_list ++;
  }
  else
  {
    TP_DBG("dvbt scan: DataPlpNumber %d,plp id %d\n",
      p_ch->channel_info.param.dvbt.DataPlpNumber, p_ch->channel_info.param.dvbt.plp_id);
    if(p_ch->channel_info.param.dvbt.DataPlpNumber)
    {
      memcpy(&p_priv->dvbt_param, &p_ch->channel_info.param.dvbt, sizeof(nim_dvbt_param_t));
      TP_DBG("p_priv->dvbt_param.DataPlpNumber[%d]\n", p_priv->dvbt_param.DataPlpNumber);
      for(i = 0;i < p_priv->dvbt_param.DataPlpNumber; i++)
      {
        TP_DBG("dvbt scan: plp id[%d], %d\n", i, p_priv->dvbt_param.DataPlpIdArray[i]);
      }
      p_priv->b_search_by_plp = TRUE;
      p_priv->cur_plp_id = p_priv->dvbt_param.plp_id;
      p_priv->cur_plp_index = p_priv->dvbt_param.PLP_index;
    }
    ret = p_priv->hook.process_tp(p_tp);
  }

  return ret;
}

static RET_CODE dvbt_scan_msg_proc(void *p_data, os_msg_t *p_msg)
{
  RET_CODE ret =  SUCCESS;
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_data;
  scan_tp_info_i_t *p_tp = p_priv->found_tp_list + p_priv->cur_idx_found_list - 1;
  
  switch(p_msg->content)
  {
    case NC_EVT_LOCKED:
      p_tp->can_locked = 1;
      //get plp id array
      //p_priv->hook.process_tp(p_tp);
      ret = on_locked(p_priv, (nc_channel_info_t *)p_msg->para1);
      //Notify progress
      notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS1));
      notify_scan_state(SCAN_EVT_TP_LOCKED);
      break;
    case NC_EVT_UNLOCKED:
      p_tp->can_locked = 0;
      p_priv->hook.process_tp(p_tp);
      //Notify progress
      notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS1));
      //if(!p_priv->b_search_by_plp)
        //p_priv->cur_idx_found_list++; //tp unlock goto the next
      notify_scan_state(SCAN_EVT_TP_UNLOCKED);
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
    case SCAN_CMD_START_SCAN:
      ret = on_start(p_priv);
      break;
    default:
      break;
  }
  return ret;
}

static find_tp_ret_t dvbt_scan_get_tp_info(void *p_data,
  nc_channel_info_t *p_channel)
{
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_data;

  while(1)
  {
    if(p_priv->dvbt_param.DataPlpNumber)
    {
      //There are some nit tp or preset tp in the found tp list
      scan_tp_info_i_t *p_cur_tp = p_priv->found_tp_list +
        p_priv->cur_idx_found_list - 1;
      scan_sat_info_t *p_cur_sat = p_priv->input_data.sat_list +
        p_priv->cur_sat_idx;
      tp_rcv_para_t *p_tp_para = &p_cur_tp->tp_info;
      
      if(p_cur_tp->sat_id != p_cur_sat->sat_id)  //the satellite changed
      {
        switch_to_sat(p_priv, p_cur_tp->sat_id);
        return FIND_BUSY;
      }
      if(p_priv->dvbt_param.plp_id == 
        p_priv->dvbt_param.DataPlpIdArray[p_priv->dvbt_param.DataPlpNumber - 1])
      {
        p_priv->dvbt_param.DataPlpNumber --;
        if(!p_priv->dvbt_param.DataPlpNumber)
        {
          //p_priv->cur_idx_found_list ++;
          continue;//all plp id searched
        }
      }
      p_priv->dvbt_param.DataPlpNumber --;
      p_priv->cur_plp_id = p_priv->dvbt_param.DataPlpIdArray[p_priv->dvbt_param.DataPlpNumber];
      p_priv->cur_plp_index = p_priv->dvbt_param.DataPlpNumber;
      // init search info
      p_channel->polarization = NIM_PORLAR_HORIZONTAL;
      p_channel->channel_info.nim_type = p_priv->dvbt_param.nim_type;
      p_channel->channel_info.frequency = p_tp_para->freq;
      p_channel->channel_info.param.dvbt.band_width = p_priv->dvbt_param.band_width;
      p_channel->channel_info.param.dvbt.nim_type = p_priv->dvbt_param.nim_type;
      p_channel->channel_info.param.dvbt.plp_id = p_priv->cur_plp_id;
      p_channel->channel_info.param.dvbt.PLP_index = p_priv->cur_plp_index;
      p_channel->channel_info.spectral_polar = NIM_IQ_AUTO;
      p_channel->channel_info.lock = NIM_CHANNEL_UNLOCK;
      TP_DBG("get tp [%d,%d,plp id %d]\n", p_channel->channel_info.frequency,
        p_channel->channel_info.param.dvbt.band_width,p_channel->channel_info.param.dvbt.plp_id);
      TP_DBG("p_channel->channel_info.param.dvbt.nim_type=%d\n",
        p_channel->channel_info.param.dvbt.nim_type);
      nc_set_blind_scan_mode(class_get_handle_by_id(NC_CLASS_ID), p_priv->tuner_id, FALSE);
     
      return FIND_SUC;
    }
    else if(p_priv->cur_idx_found_list < p_priv->total_tp_found_list)
    {
      //There are some nit tp or preset tp in the found tp list
      scan_tp_info_i_t *p_cur_tp = p_priv->found_tp_list + p_priv->cur_idx_found_list;
      tp_rcv_para_t *p_tp_para = &p_cur_tp->tp_info;

      p_channel->channel_info.nim_type = p_tp_para->nim_type;
      p_channel->channel_info.frequency = p_tp_para->freq;
      p_channel->channel_info.param.dvbt.band_width = p_tp_para->sym;
      p_channel->channel_info.param.dvbt.nim_type = p_channel->channel_info.nim_type;
      p_priv->cur_idx_found_list++;
      if(p_priv->hook.check_tp(p_channel) != SUCCESS)
      {
        //no need to lock, skip it
        continue;
      }
      TP_DBG("%s,%d\n",__FUNCTION__,__LINE__);
      nc_set_blind_scan_mode(class_get_handle_by_id(NC_CLASS_ID), p_priv->tuner_id, TRUE);
      p_priv->b_search_by_plp = FALSE;
      return FIND_SUC;
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

        memcpy(p_last_tp, p_priv->input_data.tp_list + p_priv->cur_idx_preset_list,
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

static RET_CODE dvbt_scan_tp_done(void *p_data, void *p_tp_info)
{
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_data;
  scan_tp_info_t *p_tp = (scan_tp_info_t *)p_tp_info;
  scan_pg_info_t *p_pg = NULL;
  scan_tp_info_i_t tp_info = {{0}};
  u32 pg_cnt = 0;
  u32 lcd_count = 0;
  
  //Notify progress
  notify_progress(progress_calc(p_priv, TP_SCAN_PROGRESS3));
  
  tp_info.pg_num = p_tp->pg_num;
  tp_info.p_pg_list = p_tp->p_pg_list;
  tp_info.p_pg_list->plp_id = p_priv->cur_plp_id;

  for(pg_cnt = 0; pg_cnt < tp_info.pg_num; pg_cnt++)
  {
    p_pg = tp_info.p_pg_list + pg_cnt;
    for(lcd_count = 0;lcd_count < p_priv->lcd_cnt;lcd_count ++)
    {
      if(p_pg->s_id == p_priv->lcd[lcd_count].svc_id)
      {
        p_pg->logical_num = p_priv->lcd[lcd_count].logical_channel_number;
        TP_DBG("%s,lcn[%d]\n", __FUNCTION__, p_pg->logical_num);
        break;
      }
    }
  }
  //p_priv->cur_idx_found_list++;
  
  return p_priv->hook.process_pg_list(&tp_info);
}

static void * dvbt_require_runingtime_buffer(void *p_data, u32 size)
{
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_data;
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

static scan_scramble_origin_t dvbt_get_scan_scramble_origin(void *p_policy)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_bl_scan_policy->p_priv_data;

  return p_priv->input_data.scramble_scan_origin;
}

static BOOL dvbt_require_extend_table(void *p_policy, u8 table_id)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_bl_scan_policy->p_priv_data;
  scan_tp_info_i_t *p_tp_info = p_priv->found_tp_list + p_priv->cur_idx_found_list - 1;
  BOOL ret_value = FALSE;

  switch(table_id)
  {
    case DVB_TABLE_ID_NIT_ACTUAL:
      TP_DBG("nit_scan_type[%x],enable_nit[%d],i=%d\n", p_priv->input_data.nit_scan_type,
        p_tp_info->enable_nit, p_priv->cur_idx_found_list);
      if((((p_priv->input_data.nit_scan_type != NIT_SCAN_WITHOUT) &&
        p_tp_info->enable_nit)) || p_tp_info->enable_lcn)
      {
        ret_value = TRUE;
      }
      break;
   case DVB_TABLE_ID_BAT:
      if(p_priv->input_data.bat_scan_type != BAT_SCAN_WITHOUT)
      {
        ret_value = TRUE;
      }
      break;
    default:
      break;
  }
  TP_DBG("require extend_table[%x],ret[%d]\n", table_id, ret_value);
  return ret_value;
}

static u8 dvbt_scan_get_bouquet_id_list(void *p_policy, void *p_list)
{
  scan_policy_t *p_scan_policy = p_policy;
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_scan_policy->p_priv_data;
  memcpy(p_list, p_priv->input_data.bouquet_id, p_priv->input_data.bouquet_num * sizeof(u16));
  return p_priv->input_data.bouquet_num;
}

static void dvbt_scan_get_scan_info(void *pdata, u8 *p_tuner_id, u8 *p_ts_in)
{
   class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
   dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)pdata;
   *p_tuner_id = p_priv->tuner_id;
   nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, p_ts_in);
}

static void destroy_dvbt_scan_policy(void *p_policy)
{
  scan_policy_t *p_tp_scan_policy = p_policy;
  dvbt_scan_priv_t *p_priv = (dvbt_scan_priv_t *)p_tp_scan_policy->p_priv_data;

  p_priv->hook.free_resource();
  nc_set_blind_scan_mode(class_get_handle_by_id(NC_CLASS_ID), p_priv->tuner_id, FALSE);
  nc_enable_monitor(class_get_handle_by_id(NC_CLASS_ID), p_priv->tuner_id, TRUE);
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

//void dvbt_load_sat_info_for_auto_scan(u8 u8Country, scan_preset_data_t *p_pst)
void dvbt_load_sat_info_for_auto_scan(u8 u8Country, scan_preset_data_t *p_pst, nim_type_t nim_type)
{
  u16 tp_cnt = 0;
  tp_rcv_para_t *p_tp_info = NULL;
  u8 u8MaxRFCh = dvbt_get_max_rf_ch_num(u8Country);
  u8 u8MinRFCh = dvbt_get_min_rf_ch_num(u8Country);

  p_pst->total_sat = 1;
  p_pst->nit_scan_type = NIT_SCAN_ALL_TP;
  for(tp_cnt = u8MinRFCh; tp_cnt <= u8MaxRFCh; tp_cnt++)
  {
    p_pst->tp_list[tp_cnt - u8MinRFCh].tp_origin = TP_FROM_PRESET;
    p_tp_info = &(p_pst->tp_list[tp_cnt - u8MinRFCh].tp_info);
    memset(p_tp_info, 0, sizeof(tp_rcv_para_t));
    dvbt_get_tp_setting(tp_cnt, u8Country, 
      (u16 *)(&(p_tp_info->sym)), &(p_tp_info->freq));
    p_tp_info->sym = p_tp_info->sym / 1000;
    p_tp_info->nim_type = nim_type;    
    p_pst->total_tp ++;
    if(p_pst->total_tp >= MAX_TP_NUM_SUPPORTED)
    {
      TP_DBG("tp full\n");
      break;
    }
  }
}

void dvbt_load_sat_info_for_tp_scan(tp_rcv_para_t *p_tp_info, scan_preset_data_t *p_pst)
{
  p_pst->total_sat = 1;
  p_pst->nit_scan_type = NIT_SCAN_ALL_TP;
  {
    p_pst->tp_list[0].tp_origin = TP_FROM_PRESET;
    p_tp_info->sym = p_tp_info->sym / 1000;
    p_tp_info->nim_type = NIM_DVBT_AUTO;
    memcpy(&(p_pst->tp_list[0].tp_info), p_tp_info, sizeof(tp_rcv_para_t));
    p_pst->total_tp ++;
  }
}

policy_handle_t construct_dvbt_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data)
{
  u32 size = sizeof(dvbt_scan_priv_t);
  u16 i = 0;
  dvbt_scan_priv_t *p_priv = p_hook->get_attach_buffer(size);
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
  p_priv->tp_scan_policy.msg_proc     = dvbt_scan_msg_proc;
  p_priv->tp_scan_policy.get_tp_info  = dvbt_scan_get_tp_info;
  p_priv->tp_scan_policy.on_tp_done   = dvbt_scan_tp_done;
  p_priv->tp_scan_policy.require_runingtime_buffer = 
    dvbt_require_runingtime_buffer;
  p_priv->tp_scan_policy.destroy_policy = destroy_dvbt_scan_policy;
  p_priv->tp_scan_policy.p_priv_data  = p_priv;
  p_priv->tp_scan_policy.require_extend_table = dvbt_require_extend_table;
  p_priv->tp_scan_policy.repeat_tp_check = NULL;
  p_priv->tp_scan_policy.get_scan_scramble_origin = dvbt_get_scan_scramble_origin;
  p_priv->tp_scan_policy.get_blindscan_ver = NULL;
  p_priv->tp_scan_policy.get_bouquet_id_list = dvbt_scan_get_bouquet_id_list;
  p_priv->input_data.nit_scan_type = NIT_SCAN_ALL_TP;
  p_priv->tp_scan_policy.get_scan_info = dvbt_scan_get_scan_info;

  //p_priv->tp_scan_policy.performance = dvbt_scan_performance;
  //Attach implementation policy
  memcpy(&p_priv->hook, p_hook, sizeof(scan_hook_t));
  
  //Load parameter
  memcpy(&p_priv->input_data, p_data, sizeof(scan_preset_data_t));
  CHECK_FAIL_RET_NULL(p_priv->input_data.total_tp <= MAX_TP_NUM_SUPPORTED);
  
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

