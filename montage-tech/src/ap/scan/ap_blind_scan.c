/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"

#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_printk.h"

#include "class_factory.h"

#include "drv_dev.h"
#include "nim.h"

#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pmt.h"
#include "cat.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "err_check_def.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dmx.h"

#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_scan_priv.h"

#include "ap_dvbs_scan.h"

#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_scan_imp.h"
//#ifdef PRINT_ON
#define BS_DEBUG_ON
//#endif

#ifdef BS_DEBUG_ON
#define BS_DBG OS_PRINTF
#else
#define BS_DBG DUMMY_PRINTF
#endif

/*!
  Max blind scan times per each satellite
  */
#define MAX_SCAN_TIMES_PER_SAT (4)

/*!
  Start frequency in one blind scan operation
  */
#define BS_START_FREQ_KHZ ((950)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_END_FREQ_KHZ ((2150) * (KHZ))

/*!
  Start frequency in one blind scan operation
  */
#define BS_C_SINGLE_START_FREQ_KHZ ((950)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_C_SINGLE_END_FREQ_KHZ ((2150) * (KHZ))

/*!
  Start frequency in one blind scan operation
  */
#define BS_C_DOUBLE_START_FREQ_KHZ ((950)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_C_DOUBLE_END_FREQ_KHZ ((2150) * (KHZ))

/*!
  Start frequency in one blind scan operation
  */
#define BS_KU_SINGLE_START_FREQ_KHZ ((950)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_KU_SINGLE_END_FREQ_KHZ ((2150) * (KHZ))

/*!
  Start frequency in one blind scan operation
  */
#define BS_KU_DOUBLE_22K_ON_START_FREQ_KHZ ((1100)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_KU_DOUBLE_22K_ON_END_FREQ_KHZ ((2150) * (KHZ))

/*!
  Start frequency in one blind scan operation
  */
#define BS_KU_DOUBLE_22K_OFF_START_FREQ_KHZ ((950)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_KU_DOUBLE_22K_OFF_END_FREQ_KHZ ((1950) * (KHZ))

/*!
  blind scan two times
  */
#define BS_TIMES  2

/*!
  one scan have max tp number
  */
#define MAX_TP_NUM_ONE_SCAN   20

/*!
  Maximum symbol rate offset
  */
#define MAX_SYM_OFFSET(x)  (((x) << 3) / 100)


typedef struct
{
  u32 freq;
  u32 sym;
  u32 freq_offset; 
  u32 sym_reta_offset;
}tp_info_t;

typedef struct
{
  tp_info_t tp_list[MAX_TP_NUM_ONE_SCAN];
  u16 tp_depth;
}tp_list_t;

/*!
  Scan config information per each blind scan operation
  */
typedef struct 
{
  /*!
    the first scan frequency
    */
  u32 start_fre_khz;
  /*!
    the end scan frequency
    */
  u32 end_fre_khz;
  /*!
    Frequency band -- 0: C; 1: Ku 
    */
  u8  freqBand; 
  /*!
  Local oscilator set
    */
  u8  localOscillator;  
  /*!
    nim 22KHz tone signal 0: off, 1: on
    */
  u8  onoff22k;
  /*!
    tuner polarization. @see nim_lnb_porlar_t
    */
  nim_lnb_porlar_t polarization;
  /*!
    the unicable parameters
    */
  nim_unicable_param_t unicable_param;
}bs_cfg_t;

/*!
  Blind scan private data
  */
typedef struct
{
  /*!
    my policy
    */
  scan_policy_t bl_scan_policy;
  /*!
    device
    */
  nim_device_t *p_dev[2];
  /*!
    need free ap_scan runing time buffer
    */
  void *p_need_free_rt_buf;
  /*!
    need free tp_scan priv buffer
    */
  void *p_need_free_priv_buf;
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
    current channel in the channel_map
    */
  u16 cur_channel_idx;
  /*!
    found channel total
    */
  u16 channel_count;
  /*!
    Current scan frequency
    */
  u32 cur_fre_khz;
  /*!
    Last scan frequency
    */
  u32 lst_fre_khz;
  /*!
    current bs times
    */
  u8 cur_bs_times; 
  /*!
    Total scan times in blind scan in one satellite
    */
  u8 total_bs_times;
  /*!
    Blind scan config parameter per each satellite
    */
  bs_cfg_t bs_cfg_info[MAX_SCAN_TIMES_PER_SAT];
  /*!
    Pre-blind buffer
    */
  nim_channel_info_t channel_map[MAX_TP_NUM_PER_SAT];
  /*!
    current TP information
    */
  scan_tp_info_i_t cur_tp_info; 
  /*!
    Blind scan implementation
    */
  scan_hook_t hook;
  /*!
    Current satellite number
    */
  u8 cur_sat_idx;
  /*!
    Satellite total num
    */
  u16 total_preset_sat;
  /*!
    NIT scan type
    */
  nit_scan_type_t nit_scan_type;
  /*!
    BAT scan type
    */
  bat_scan_type_t bat_scan_type;  
  /*!
    Sat list for saving tp information in TP scan
    */
  scan_sat_info_t perset_sat_list[MAX_SAT_NUM_SUPPORTED];
  /*!
    NC service handle
    */
  service_t *p_nc_svc;
  /*!
    blind scan times
    */
  u8 bs_times;
  /*! 
    blind scan time = 2; need filter repeat tp (locked)
    */
  tp_list_t tp_list_locked;
  /*! 
    blind scan time = 2; need filter repeat tp (unlocked)
    */
  tp_list_t tp_list_unlocked;
  scan_scramble_origin_t scramble_scan_origin;
    /*!
    bat bouqust id num
    */
  u8 bouquet_num;
  /*!
    list for bat bouqust id
    */
  u16 bouquet_id[MAX_BOUQUET_ID_IN_SCAN];
  /*!
    tuner id
    */
  u8 tuner_id; 
  /*!
    ts in
    */
  u8 ts_in;
} bl_scan_priv_t;



static void notify_progress(u32 progress)
{
  event_t evt;
  evt.id = SCAN_EVT_PROGRESS;
  evt.data1 = (u32)progress;
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
}

static u32 progress_calc(bl_scan_priv_t *p_priv)
{ 
  u32 progress = 0;
  u32 scan_sat_unit = 10000 / p_priv->total_preset_sat;
  u32 scan_times_unit = scan_sat_unit / p_priv->total_bs_times;
  u32 cur_freq_mhz = p_priv->channel_map[p_priv->cur_channel_idx].frequency / 1000;
  u32 star_freq_mhz = p_priv->bs_cfg_info[p_priv->cur_bs_times].start_fre_khz / 1000;
  u32 end_freq_mhz = p_priv->bs_cfg_info[p_priv->cur_bs_times].end_fre_khz / 1000;
  u32 step = 0;

  if(p_priv->lst_fre_khz >p_priv->cur_fre_khz)
  {
    p_priv->lst_fre_khz = p_priv->cur_fre_khz;
  }
  
  if(p_priv->channel_map[p_priv->cur_channel_idx].frequency > p_priv->lst_fre_khz)
  {
    cur_freq_mhz = p_priv->channel_map[p_priv->cur_channel_idx].frequency / 1000;
  }
  else
  {
    cur_freq_mhz = p_priv->lst_fre_khz / 1000;
  }
  p_priv->lst_fre_khz = p_priv->cur_fre_khz;
  
  //the first tp which pre scan may be less than start
  if(cur_freq_mhz > star_freq_mhz)
  {
    step = cur_freq_mhz - star_freq_mhz;
  }
  
  if(end_freq_mhz > star_freq_mhz)
  {
    progress = p_priv->cur_sat_idx * scan_sat_unit;
    progress += scan_sat_unit * p_priv->cur_bs_times / p_priv->total_bs_times;
    progress += scan_times_unit * step / (end_freq_mhz - star_freq_mhz);
  }
  //BS_DBG("BS cur_sat_idx %d, scan_sat_unit %d, bs time %d,"
  //        ""total time %d, time unit %d, ch_idx %d, x %d, y %d",
  //  p_priv->cur_sat_idx, scan_sat_unit, p_priv->cur_bs_times,
  //  p_priv->total_bs_times, scan_times_unit, 
  //  p_priv->cur_channel_idx,
  //  (cur_freq - star_freq), (end_freq-star_freq));
  
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

static u32 bs_calc_down_frq(nc_channel_info_t *p_ch,scan_sat_info_t *p_sat)
{
  tp_rcv_para_t  tp_para  = {0};

  tp_para.freq = p_ch->channel_info.frequency / KHZ;
  tp_para.sym = p_ch->channel_info.param.dvbs.symbol_rate;
  tp_para.is_on22k = p_ch->onoff22k;
  tp_para.polarity = p_ch->polarization;

  return dvbs_calc_down_freq(&tp_para, &p_sat->sat_info);
}

static void unicable_config(bs_cfg_t *p_cfg, sat_rcv_para_t *p_sat_para)
{
  if((p_sat_para->user_band != 0) &&
    (p_cfg->freqBand == KU_BAND) &&
    (p_cfg->localOscillator == DOUBLE_LOCAL_OSCILLATOR))
  {
    if(p_cfg->polarization == NIM_PORLAR_HORIZONTAL)
    {
      p_cfg->unicable_param.bank = (p_cfg->onoff22k ? 0x03 : 0x02) + 
                                            (p_sat_para->unicable_type * 4); 
    }
    else
    {
      p_cfg->unicable_param.bank = (p_cfg->onoff22k ? 0x01 : 0x00) + 
                                             (p_sat_para->unicable_type * 4);
    }
    p_cfg->unicable_param.use_uc = 1;
    //transfer user band [0~8] to device [0~7]
    //in logic variable zero means disable, [1~8] mapping to [0~7]
    p_cfg->unicable_param.user_band = p_sat_para->user_band - 1;
    p_cfg->unicable_param.ub_freq_mhz = p_sat_para->band_freq;
  }
  else
  {
    memset(&p_cfg->unicable_param, 0x0, sizeof(nim_unicable_param_t));
  }
}

static u8 blind_scan_config(bl_scan_priv_t *p_priv, scan_sat_info_t *p_sat)
{
  bs_cfg_t blind_scan_info = { 0 };
  bs_cfg_t *p_cfg_info = NULL;
  u8 total_scan_times = 0;
  sat_rcv_para_t *p_sat_para = &p_sat->sat_info;
  
  CHECK_FAIL_RET_ZERO(p_priv != NULL);
  p_cfg_info = p_priv->bs_cfg_info;

  if(DVBS_LNB_STANDARD == p_sat_para->lnb_type)
  {
    blind_scan_info.freqBand = dvbs_detect_lnb_freq(p_sat_para->lnb_low);
    blind_scan_info.localOscillator = SINGLE_LOCAL_OSCILLATOR;
  }
  else if(DVBS_LNB_USER == p_sat_para->lnb_type)
  {
    blind_scan_info.freqBand = C_BAND;
    blind_scan_info.localOscillator = DOUBLE_LOCAL_OSCILLATOR;
  }
  else if(DVBS_LNB_UNIVERSAL == p_sat_para->lnb_type)
  {
    blind_scan_info.freqBand = KU_BAND;
    blind_scan_info.localOscillator = DOUBLE_LOCAL_OSCILLATOR;
  }
  else
  {
    return (0);
  }

  if(C_BAND == blind_scan_info.freqBand) // C band
  {
    if(SINGLE_LOCAL_OSCILLATOR == blind_scan_info.localOscillator) 
    {
      blind_scan_info.start_fre_khz = BS_C_SINGLE_START_FREQ_KHZ;
      blind_scan_info.end_fre_khz = BS_C_SINGLE_END_FREQ_KHZ;
      blind_scan_info.onoff22k = (u8)p_sat_para->k22;
      if(DVBS_LNB_POWER_ALL == p_sat_para->lnb_power 
        || DVBS_LNB_POWER_18V == p_sat_para->lnb_power)
      {
        // Horizontal scan
        blind_scan_info.polarization = NIM_PORLAR_HORIZONTAL;
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));

        p_cfg_info++;
        total_scan_times++;
      }
      
      if(DVBS_LNB_POWER_ALL == p_sat_para->lnb_power 
        || DVBS_LNB_POWER_13V == p_sat_para->lnb_power)
      {
        // Vertical scan
        blind_scan_info.polarization = NIM_PORLAR_VERTICAL; 
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));

        p_cfg_info++;
        total_scan_times++;
      }
    }
    else // Double local oscillator
    {
      blind_scan_info.start_fre_khz = BS_C_DOUBLE_START_FREQ_KHZ;
      blind_scan_info.end_fre_khz = BS_C_DOUBLE_END_FREQ_KHZ;
      blind_scan_info.onoff22k = (u8)p_sat_para->k22;
      memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));
      p_cfg_info++;
      total_scan_times++;
    }
  }
  else // Ku band
  {
    if(SINGLE_LOCAL_OSCILLATOR == blind_scan_info.localOscillator) 
    {
      blind_scan_info.start_fre_khz = BS_KU_SINGLE_START_FREQ_KHZ;
      blind_scan_info.end_fre_khz = BS_KU_SINGLE_END_FREQ_KHZ;
      blind_scan_info.onoff22k = (u8)p_sat_para->k22;

      if(DVBS_LNB_POWER_ALL == p_sat_para->lnb_power 
       || DVBS_LNB_POWER_18V == p_sat_para->lnb_power)
      {          
        // Horizontal scan
        blind_scan_info.polarization = NIM_PORLAR_HORIZONTAL; 
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));
        p_cfg_info++;
        total_scan_times++;
      }
      
      if(DVBS_LNB_POWER_ALL == p_sat_para->lnb_power
         || DVBS_LNB_POWER_13V == p_sat_para->lnb_power)
      {
        // Vertical scan
        blind_scan_info.polarization = NIM_PORLAR_VERTICAL; 
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));
        p_cfg_info++;
        total_scan_times++;
      }
    }
    else // Double local oscillator
    {
      if(DVBS_LNB_POWER_ALL == p_sat_para->lnb_power ||
        DVBS_LNB_POWER_18V == p_sat_para->lnb_power)
      {
        // Horizontal scan
        blind_scan_info.polarization = NIM_PORLAR_HORIZONTAL; 
        blind_scan_info.onoff22k = 0; // 22K off
        blind_scan_info.start_fre_khz = BS_KU_DOUBLE_22K_OFF_START_FREQ_KHZ;
        blind_scan_info.end_fre_khz = BS_KU_DOUBLE_22K_OFF_END_FREQ_KHZ;

        //config for unicable
        unicable_config(&blind_scan_info, p_sat_para);
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));
        p_cfg_info++;
        total_scan_times++;

        blind_scan_info.onoff22k = 1; // 22K on
        blind_scan_info.start_fre_khz = BS_KU_DOUBLE_22K_ON_START_FREQ_KHZ;
        blind_scan_info.end_fre_khz = BS_KU_DOUBLE_22K_ON_END_FREQ_KHZ;

        //config for unicable
        unicable_config(&blind_scan_info, p_sat_para);
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));
        p_cfg_info++;
        total_scan_times++;
      }
      
      if(DVBS_LNB_POWER_ALL == p_sat_para->lnb_power ||
        DVBS_LNB_POWER_13V == p_sat_para->lnb_power)
      {
        //Vertical scan
        blind_scan_info.polarization = NIM_PORLAR_VERTICAL; 
        blind_scan_info.onoff22k = 0; // 22K off
        blind_scan_info.start_fre_khz = BS_KU_DOUBLE_22K_OFF_START_FREQ_KHZ;
        blind_scan_info.end_fre_khz = BS_KU_DOUBLE_22K_OFF_END_FREQ_KHZ;

        //config for unicable
        unicable_config(&blind_scan_info, p_sat_para);
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));
        p_cfg_info++;
        total_scan_times++;

        blind_scan_info.onoff22k = 1; // 22K on
        blind_scan_info.start_fre_khz = BS_KU_DOUBLE_22K_ON_START_FREQ_KHZ;
        blind_scan_info.end_fre_khz = BS_KU_DOUBLE_22K_ON_END_FREQ_KHZ;

                //config for unicable
        unicable_config(&blind_scan_info, p_sat_para);
        memcpy(p_cfg_info, &blind_scan_info, sizeof(bs_cfg_t));
        p_cfg_info++;
        total_scan_times++;
      }
    }
  }
  return total_scan_times;
}

static void switch_to_sat(bl_scan_priv_t *p_priv, u8 index)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + index;
  BOOL need_waiting = FALSE;

  p_priv->tuner_id = p_sat->tuner_id;   
  //BS_DBG("switch_to_sat:%x\n", p_priv->tuner_id);
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
 
  //Set nim ctrl blind scan mode into bs mode
  nc_set_blind_scan_mode(nc_handle, p_priv->tuner_id, TRUE);

  need_waiting = nc_set_diseqc(nc_handle, p_priv->tuner_id , &p_sat->diseqc_info);
  nc_set_12v(nc_handle, p_priv->tuner_id , (u8)p_sat->sat_info.v12);
  
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
  p_priv->cur_channel_idx = 0;
  p_priv->channel_count = 0;
  p_priv->cur_bs_times = 0;
  p_priv->total_bs_times = blind_scan_config(p_priv, p_sat);
  p_priv->cur_fre_khz = p_priv->bs_cfg_info[p_priv->cur_bs_times].start_fre_khz;
}

static void _clear_tp_list(bl_scan_priv_t *p_priv)
{
  memset(&p_priv->tp_list_locked, 0x0, sizeof(tp_list_t));
  memset(&p_priv->tp_list_unlocked, 0x0, sizeof(tp_list_t));
}

static RET_CODE on_start(bl_scan_priv_t *p_priv, os_msg_t *p_msg)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);

  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;  
  p_priv->tuner_id = p_sat->tuner_id; 
  BS_DBG("switch_to_sat:%x\n", p_priv->tuner_id);  
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
  if(p_priv->hook.on_start != NULL)
  {
    p_priv->hook.on_start();
  }
  p_priv->p_nc_svc = nc_get_svc_instance(nc_handle, APP_SCAN);

  p_priv->bs_times = (u8)p_msg->para2;

  //Set nim ctrl blind scan mode into bs mode
  //nc_set_blind_scan_mode(nc_handle, p_priv->tuner_id, TRUE);

  //Init Parameter
  p_priv->cur_sat_idx = 0;
  p_priv->scan_progress = 0;

  //init global tp list
  if(p_priv->bs_times == 2)
  {
    _clear_tp_list(p_priv);
  }
  else
  {
    p_priv->bs_times = 1; 
  }
  
  switch_to_sat(p_priv, 0);

  return SUCCESS;
}

static void calc_tp_offset_info(u32 locked_symb_KSs, u32 *p_freq_offset, 
    u32 *p_sym_reta_offset)
{
  if(locked_symb_KSs > 10000)
  {
    *p_freq_offset = 3000;
    *p_sym_reta_offset = 2000;
  }
  else if(locked_symb_KSs > 5000)
  {
    *p_freq_offset = 2100;
    *p_sym_reta_offset = 1000;
  }
  else
  {
    *p_freq_offset = 1000;
    *p_sym_reta_offset = 300;
  }
}

static RET_CODE on_locked(bl_scan_priv_t *p_priv, nc_channel_info_t *p_ch)
{    
  tp_rcv_para_t *p_tp_para = &p_priv->cur_tp_info.tp_info;
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;
  RET_CODE ret = ERR_FAILURE;
        
  memset(&p_priv->cur_tp_info, 0, sizeof(scan_tp_info_i_t));
  p_tp_para->freq = bs_calc_down_frq(p_ch, p_sat);
  p_tp_para->sym = p_ch->channel_info.param.dvbs.symbol_rate;
  p_tp_para->polarity = p_ch->polarization;
  p_tp_para->is_on22k = p_ch->onoff22k;
  p_tp_para->nim_type = p_ch->channel_info.param.dvbs.nim_type;
  p_tp_para->fec_inner = p_ch->channel_info.param.dvbs.fec_inner;
  p_priv->cur_tp_info.sat_id = p_sat->sat_id;
  p_priv->cur_tp_info.can_locked = 1;
  p_priv->cur_tp_info.tp_origin = TP_FROM_SCAN;

  memcpy(&p_priv->cur_tp_info.perf, &p_ch->channel_info.param.dvbs.perf, sizeof(nim_channel_perf_t));
  ret = p_priv->hook.process_tp(&p_priv->cur_tp_info);

  if((ret == SUCCESS) && (p_priv->bs_times == 2))
  {
    //save a locked tp with a scan window
    p_priv->tp_list_locked.tp_list[p_priv->tp_list_locked.tp_depth].freq = 
                                                                  p_ch->channel_info.frequency;
    p_priv->tp_list_locked.tp_list[p_priv->tp_list_locked.tp_depth].sym = \
                                              p_ch->channel_info.param.dvbs.symbol_rate;
    calc_tp_offset_info(
           p_priv->tp_list_locked.tp_list[p_priv->tp_list_locked.tp_depth].sym, 
           &p_priv->tp_list_locked.tp_list[p_priv->tp_list_locked.tp_depth].freq_offset,
           &p_priv->tp_list_locked.tp_list[p_priv->tp_list_locked.tp_depth].sym_reta_offset);

    p_priv->tp_list_locked.tp_depth++;
  }
  //Notify progress 
  notify_progress(progress_calc(p_priv));
  return ret;
}

static RET_CODE on_unlocked(bl_scan_priv_t *p_priv)
{      
  //Notify progress 
  notify_progress(progress_calc(p_priv));
  p_priv->cur_tp_info.can_locked = 0;
  p_priv->hook.process_tp(&p_priv->cur_tp_info);
  
  if(p_priv->bs_times == 2)
  {
    //save a unlocked tp with a scan window
    p_priv->tp_list_unlocked.tp_list[p_priv->tp_list_unlocked.tp_depth].freq = 
                               p_priv->channel_map[p_priv->cur_channel_idx].frequency;
    p_priv->tp_list_unlocked.tp_list[p_priv->tp_list_unlocked.tp_depth].sym = \
                               p_priv->channel_map[p_priv->cur_channel_idx].param.dvbs.symbol_rate;

    p_priv->tp_list_unlocked.tp_list[p_priv->tp_list_unlocked.tp_depth].freq_offset = 0;
    p_priv->tp_list_unlocked.tp_list[p_priv->tp_list_unlocked.tp_depth].sym_reta_offset = 0;

    p_priv->tp_list_unlocked.tp_depth++;
  }
  p_priv->cur_channel_idx++;
  return SUCCESS;
}


static RET_CODE on_nit(bl_scan_priv_t *p_priv)
{
  if(p_priv->nit_scan_type == NIT_SCAN_ONCE)
  {
    p_priv->nit_scan_type = NIT_SCAN_WITHOUT;
  }
  return SUCCESS;
}

tp_compare_result_t _compare_two_tps(u32 locked_freq_KHz,  u32 locked_symb_KSs,  
  u32 freq_offset, u32 sym_rate_offset, u32 scaned_freq_KHz, u32 scaned_symb_KSs)
{
  u32 delta_freq_khz =0 ;
  u32 delta_sym_rate = 0;
  
  if(locked_freq_KHz > scaned_freq_KHz)
  {
    delta_freq_khz = locked_freq_KHz - scaned_freq_KHz;
  }
  else
  {
    delta_freq_khz =  scaned_freq_KHz - locked_freq_KHz;
  }
 
  if(locked_symb_KSs > scaned_symb_KSs)
  {
    delta_sym_rate = locked_symb_KSs - scaned_symb_KSs;
  }
  else
  {
    delta_sym_rate = scaned_symb_KSs - locked_symb_KSs;
  }

  if ((delta_freq_khz <= freq_offset) && (delta_sym_rate <= sym_rate_offset))
  {
    return SAME_TPS;
  }
  else if(delta_freq_khz >=
   (u32)(((locked_symb_KSs - sym_rate_offset) +  \
              (scaned_symb_KSs - sym_rate_offset)) / 2))
  {
    return DIFFERENT_TPS;
  }
  
  return OVERLAPPED_TPS;
}

static BOOL _is_new_tp_of_nim_scan(bl_scan_priv_t *p_priv, 
  u32 freq, u32 symbol)
{
  u16 i = 0;

  //compare locked tp
  for(i = 0; i < p_priv->tp_list_locked.tp_depth; i++)
  {
     if(_compare_two_tps(p_priv->tp_list_locked.tp_list[i].freq,
                                    p_priv->tp_list_locked.tp_list[i].sym, 
                                    p_priv->tp_list_locked.tp_list[i].freq_offset, 
                                    p_priv->tp_list_locked.tp_list[i].sym_reta_offset,
                                    freq, symbol) != DIFFERENT_TPS)
     {
        return FALSE;
     }
  }

  //compare unlocked tp
  for(i = 0; i < p_priv->tp_list_unlocked.tp_depth; i++)
  {
    if(_compare_two_tps(p_priv->tp_list_unlocked.tp_list[i].freq, 
                                   p_priv->tp_list_unlocked.tp_list[i].sym, 
                                   p_priv->tp_list_unlocked.tp_list[i].freq_offset, 
                                   p_priv->tp_list_unlocked.tp_list[i].sym_reta_offset,
                                   freq, symbol) == SAME_TPS)
      {
        return FALSE;
      }
  }

  return TRUE;  
}

static u32  _filter_tp_info(bl_scan_priv_t *p_priv, 
  nim_channel_info_t * p_channel, u32 found_tps, u32 exist_tps)
{
  u32 i = 0 ;
  u32 new_tp = 0;
  
  for(i = 0; i < found_tps; i++)
  {
    if(_is_new_tp_of_nim_scan
        (p_priv, p_channel[i].frequency, p_channel[i].param.dvbs.symbol_rate))
    {
      p_priv->channel_map[exist_tps].frequency = p_channel[i].frequency;
      p_priv->channel_map[exist_tps].param.dvbs.symbol_rate =       \
                                                   p_channel[i].param.dvbs.symbol_rate;
      exist_tps++;
      new_tp++;
    }  
  }

  return new_tp;
}

static BOOL pre_scan(bl_scan_priv_t *p_priv, u16 *p_find_cnt)
{
  bs_cfg_t *p_cur_cfg = p_priv->bs_cfg_info + p_priv->cur_bs_times;
  u32 found_tps = 0;
  u32 i = 0;
  nim_scan_info_t scan_info = {0};
  u32 exist_tps = p_priv->channel_count;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;  

  p_priv->tuner_id = p_sat->tuner_id; 
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
  if(p_priv->cur_fre_khz >= p_cur_cfg->end_fre_khz) //finish current
  {
    p_priv->cur_bs_times++;
    //change polar, needn't compare repeat tp.
    if(p_priv->bs_times == BS_TIMES)
    {
      p_priv->tp_list_locked.tp_depth = 0;
      p_priv->tp_list_unlocked.tp_depth = 0;
    }
   
    if(p_priv->cur_bs_times < p_priv->total_bs_times)
    {
      p_cur_cfg++;  //goto next times
      p_priv->cur_fre_khz = p_cur_cfg->start_fre_khz;
      if(p_cur_cfg->unicable_param.use_uc)
      {
        //always 13V and disable 22K
        nc_set_22k(nc_handle, p_priv->tuner_id, 0);
        nc_set_polarization(nc_handle, p_priv->tuner_id, NIM_PORLAR_VERTICAL);
      }
      else
      {
        nc_set_22k(nc_handle, p_priv->tuner_id, p_cur_cfg->onoff22k);
        nc_set_polarization(nc_handle, p_priv->tuner_id, p_cur_cfg->polarization);
      }
    }
    else
    {
      //the current satellite scan done
      return FALSE;
    }
  }

  if(nc_is_lnb_check_enable(nc_handle, p_priv->tuner_id))
  {
    dev_io_ctrl(p_priv->p_dev[p_priv->tuner_id], NIM_REMOVE_PROTECT, 0);  
    //delay 30ms.
    mtos_task_delay_ms(30);
    //reset
    dev_io_ctrl(p_priv->p_dev[p_priv->tuner_id], NIM_ENABLE_CHECK_PROTECT, 0);
  }
  
  scan_info.start_freq = p_priv->cur_fre_khz;
  scan_info.end_freq = p_priv->cur_fre_khz;
  if(p_priv->bs_times == 2)
  {
    nim_channel_info_t channel_info_map[MAX_TP_NUM_ONE_SCAN];
    memset(channel_info_map, 0x0, sizeof(nim_channel_info_t) * MAX_TP_NUM_ONE_SCAN);
    
    scan_info.p_channel_info = channel_info_map;
    scan_info.max_count = MAX_TP_NUM_PER_SAT - exist_tps;
    scan_info.channel_num = 0;
  }
  else
  {
    scan_info.p_channel_info = p_priv->channel_map;
    scan_info.max_count = MAX_TP_NUM_PER_SAT;
    scan_info.channel_num = exist_tps;
  }
  //set unicable info
  if(p_cur_cfg->unicable_param.use_uc)
  {
    memcpy(&scan_info.uc_param, &p_cur_cfg->unicable_param, sizeof(nim_unicable_param_t));
  }
  else
  {
    memset(&scan_info.uc_param, 0x0, sizeof(nim_unicable_param_t));
  }
  nim_channel_scan(p_priv->p_dev[p_priv->tuner_id], &scan_info);
  p_priv->cur_fre_khz = scan_info.start_freq;

  if(p_priv->bs_times == 2)
  {
    found_tps = _filter_tp_info(p_priv, scan_info.p_channel_info, scan_info.channel_num, exist_tps);
    _clear_tp_list(p_priv);
  }
  else
  {
    found_tps = scan_info.channel_num -  exist_tps;
  }

  for(i = exist_tps; i < scan_info.channel_num; i++)
  {
    BS_DBG("TP[%d]: freq %d, sym %d, pola %d, 22kon %d\n", i,
      scan_info.p_channel_info[i].frequency,
      scan_info.p_channel_info[i].param.dvbs.symbol_rate,
      p_cur_cfg->polarization, p_cur_cfg->onoff22k);
  }
  *p_find_cnt = found_tps;
  
  return TRUE;
}

static find_tp_ret_t find_tp(bl_scan_priv_t *p_priv,
  nc_channel_info_t *p_channel)
{
  u16 count = 0;
  u16 index = p_priv->cur_channel_idx;
  bs_cfg_t *p_cur_cfg = NULL;

  while(TRUE)
  {
    if(index < p_priv->channel_count)
    {
      p_cur_cfg = p_priv->bs_cfg_info + p_priv->cur_bs_times;
      memcpy(&p_channel->channel_info,
        p_priv->channel_map + index, sizeof(nim_channel_info_t));
      p_channel->onoff22k = p_cur_cfg->onoff22k;
      if(C_BAND == p_cur_cfg->freqBand &&
        DOUBLE_LOCAL_OSCILLATOR == p_cur_cfg->localOscillator)
      {
        p_channel->polarization =
          p_channel->channel_info.frequency > (1550 * KHZ) ? 1 : 0;
      }
      else
      {
        p_channel->polarization = p_cur_cfg->polarization;
      }
      //copy unicable information.
      if(p_cur_cfg->unicable_param.use_uc)
      {
        memcpy(&p_channel->channel_info.param.dvbs.uc_param, &p_cur_cfg->unicable_param,
          sizeof(nim_unicable_param_t));
      }
      else
      {
       memset(&p_channel->channel_info.param.dvbs.uc_param, 0x0, sizeof(nim_unicable_param_t));
      }
      return FIND_SUC;
    }
    log_perf(LOG_AP_SCAN, PERF_PRE_SCAN, 0, 0);
    //pre scan current satellite
    if(pre_scan(p_priv, &count))
    {
      if(count > 0)
      {
        p_priv->channel_count += count;
        if(p_priv->channel_count > MAX_TP_NUM_PER_SAT)
        {
          return FIND_DONE;
        }
      }
      else
      {
        log_perf(LOG_AP_SCAN, PERF_SCAN_EVT, 0, 0);
        return FIND_BUSY;
      }
    }
    else
    {
      log_perf(LOG_AP_SCAN, PERF_SCAN_EVT, 0, 0);
      return FIND_DONE;
    }
    log_perf(LOG_AP_SCAN, PERF_SCAN_EVT, 0, 0);
  }
}

static find_tp_ret_t blind_scan_get_tp_info(void *p_data, 
  nc_channel_info_t *p_channel)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
  find_tp_ret_t ret = FIND_SUC;
  
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

  ret = find_tp(p_priv, p_channel);
  notify_progress(progress_calc(p_priv));
  if(FIND_SUC == ret) //found a new tp
  {
    if(p_priv->hook.check_tp(p_channel) != SUCCESS)
    {
      //if the tp isn't need scan, skip it
      p_priv->cur_channel_idx++;
      return FIND_BUSY;
    }
    else
    {
      return FIND_SUC;
    }
  }
  else if(FIND_BUSY == ret) //need retry, go on
  {
    return FIND_BUSY;
  }
  else if(FIND_DONE == ret) //cur sat found finish
  {
    //the current satellite scan done, goto the next one
    p_priv->cur_sat_idx++;
    if(p_priv->cur_sat_idx < p_priv->total_preset_sat)//has the next
    {
      //goto the next satellite
      switch_to_sat(p_priv, p_priv->cur_sat_idx);
      return FIND_BUSY;
    }
    else
    {
      notify_progress(100);
      return FIND_DONE;
    }
  }

  return FIND_DONE;
}

static RET_CODE blind_scan_msg_proc(void *p_data, os_msg_t *p_msg)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
  RET_CODE ret = SUCCESS;
  
  switch(p_msg->content)
  {
    case NC_EVT_LOCKED:
      ret = on_locked(p_priv, (nc_channel_info_t *)p_msg->para1);      
      break;
    case NC_EVT_UNLOCKED:
      ret = on_unlocked(p_priv);     
      break;
    case DVB_NIT_FOUND:
      ret = on_nit(p_priv);
      break;
    case SCAN_CMD_START_SCAN:
      ret = on_start(p_priv, p_msg);
      break;
    default:
      break;
  }
  
  return ret;
}

static RET_CODE blind_scan_tp_done(void *p_data, void *p_tp_info)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
  scan_tp_info_t *p_tp = (scan_tp_info_t *)p_tp_info;
  scan_tp_info_i_t tp_info = {{0}};
  
  //Notify progress 
  notify_progress(progress_calc(p_data));
  
  tp_info.pg_num = p_tp->pg_num;
  tp_info.p_pg_list = p_tp->p_pg_list;
  p_priv->cur_channel_idx++;
  
  return p_priv->hook.process_pg_list(&tp_info);
}

static void * blind_require_runingtime_buffer(void *p_data, u32 size)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
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

static void destroy_bl_scan_policy(void *p_policy)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_bl_scan_policy->p_priv_data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;  

  p_priv->tuner_id = p_sat->tuner_id; 
  //BS_DBG("destroy_bl_scan_policy:%x\n", p_priv->tuner_id);
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
  //Reset blind scan mode in NC module
  nc_set_blind_scan_mode(nc_handle, p_priv->tuner_id, FALSE);
  
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

static scan_scramble_origin_t blind_get_scan_scramble_origin(void *p_policy)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_bl_scan_policy->p_priv_data;

  return p_priv->scramble_scan_origin;
}

static BOOL bind_require_extend_table(void *p_policy, u8 table_id)
{
  scan_policy_t *p_bl_scan_policy = p_policy;
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_bl_scan_policy->p_priv_data;
  BOOL ret_value = FALSE;

  switch(table_id)
  {
    case DVB_TABLE_ID_NIT_ACTUAL:
      if(p_priv->nit_scan_type != NIT_SCAN_WITHOUT)
      {
        ret_value = TRUE;
      }
      break;
   case DVB_TABLE_ID_BAT:
      if(p_priv->bat_scan_type != BAT_SCAN_WITHOUT)
      {
        ret_value = TRUE;
      }
      break;
    default:
      break;
  }
  return ret_value;
}

static BOOL _is_new_tp_of_locked_tp(u32 old_freq, u32 old_sym, 
  u32 new_freq, u32 new_sym)
{
  s32 sym_tol = 0;
  s32 deta_fre = 0;
  s32 deta_sym = 0;

  sym_tol = MAX_SYM_OFFSET(new_freq);

  deta_fre = (s32)(old_freq - new_freq);
  deta_sym = (s32)(old_sym - new_sym);

  if((ABS(deta_fre) <=  ((old_sym + new_sym)/2)/1000)
      && (ABS(deta_sym) <= sym_tol))
  {
      return FALSE;
  }
  
  return TRUE;
}

static BOOL blind_repeat_tp_check(void *p_data, nc_channel_info_t *p_ch)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
  tp_list_t *p_tp_list = &p_priv->tp_list_locked;
  u16 i  =0;

  //now, only used blind scam 2 times
  if(p_priv->bs_times != BS_TIMES)
  {
    return TRUE;
  }

  for(i = 0; i < p_tp_list->tp_depth; i++)
  {
    if(!_is_new_tp_of_locked_tp(p_tp_list->tp_list[i].freq / KHZ, p_tp_list->tp_list[i].sym,
      p_ch->channel_info.frequency / KHZ, p_ch->channel_info.param.dvbs.symbol_rate))
    {
    
      p_priv->cur_channel_idx++;
      return FALSE;
    }
  }
  
  return TRUE;
}

static u8 bl_get_bouquet_id_list(void *p_policy, void *p_list)
{
  scan_policy_t *p_scan_policy = p_policy;
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_scan_policy->p_priv_data;
  memcpy(p_list, p_priv->bouquet_id, p_priv->bouquet_num * sizeof(u16));
  return p_priv->bouquet_num;
}

static void blind_scan_get_scan_info(void *p_data, u8 *p_tuner_id, u8 *p_ts_in)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;
  *p_tuner_id = p_sat->tuner_id;  
  nc_get_ts_by_tuner(nc_handle, p_sat->tuner_id, p_ts_in);
}

policy_handle_t construct_bl_scan_policy(scan_hook_t *p_hook,
  scan_preset_data_t *p_data)
{
  u32 size = sizeof(bl_scan_priv_t);
  bl_scan_priv_t *p_priv = p_hook->get_attach_buffer(size);
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

  //get device  
  p_priv->p_dev[0] = (nim_device_t *)nc_get_nim_handle_by_tuner(nc_handle, TUNER0);
  if(nc_get_tuner_count(nc_handle) == 2)
  {
    p_priv->p_dev[1] = (nim_device_t *)nc_get_nim_handle_by_tuner(nc_handle, TUNER1);
  }
  //Attach scan policy
  p_priv->bl_scan_policy.msg_proc     = blind_scan_msg_proc;
  p_priv->bl_scan_policy.get_tp_info  = blind_scan_get_tp_info;
  p_priv->bl_scan_policy.on_tp_done   = blind_scan_tp_done;
  p_priv->bl_scan_policy.require_runingtime_buffer = 
    blind_require_runingtime_buffer;
  p_priv->bl_scan_policy.destroy_policy = destroy_bl_scan_policy;
  p_priv->bl_scan_policy.p_priv_data  = p_priv;
  p_priv->bl_scan_policy.require_extend_table = bind_require_extend_table;
  p_priv->bl_scan_policy.repeat_tp_check = blind_repeat_tp_check;
  p_priv->bl_scan_policy.get_scan_scramble_origin = blind_get_scan_scramble_origin;
  p_priv->bl_scan_policy.get_blindscan_ver = NULL;
  p_priv->bl_scan_policy.get_bouquet_id_list = bl_get_bouquet_id_list;
  p_priv->bl_scan_policy.get_scan_info = blind_scan_get_scan_info;

  //Attach implementation policy
  memcpy(&p_priv->hook, p_hook, sizeof(scan_hook_t));
  
  //Load parameter
  p_priv->total_preset_sat = p_data->total_sat;
  p_priv->nit_scan_type = p_data->nit_scan_type;
  p_priv->bat_scan_type = p_data->bat_scan_type;
  p_priv->scramble_scan_origin = p_data->scramble_scan_origin;
  p_priv->bouquet_num = p_data->bouquet_num;
  memcpy(p_priv->bouquet_id, p_data->bouquet_id, sizeof(u16) * p_data->bouquet_num);
  
  CHECK_FAIL_RET_NULL(p_priv->total_preset_sat <= MAX_SAT_NUM_SUPPORTED);
  memcpy(p_priv->perset_sat_list, p_data->sat_list,
    sizeof(scan_sat_info_t)*p_priv->total_preset_sat);
  
  return (policy_handle_t)&p_priv->bl_scan_policy;
}

