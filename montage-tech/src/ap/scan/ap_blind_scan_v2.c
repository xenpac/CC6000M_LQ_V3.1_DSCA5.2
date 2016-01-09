/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"

#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_mutex.h"

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

#include "dmx.h"

#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_scan_priv.h"

#include "ap_dvbs_scan.h"

#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_scan_imp.h"

#ifdef PRINT_ON
#define BS_DEBUG_ON
#endif

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
#define BS_C_SINGLE_START_FREQ_KHZ_LOW ((250)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_C_SINGLE_END_FREQ_KHZ_LOW ((950) * (KHZ))

/*!
  Start frequency in one blind scan operation
  */
#define BS_C_DOUBLE_START_FREQ_KHZ_LOW ((250)*(KHZ))

/*!
  End frequency in one blind scan operation
  */
#define BS_C_DOUBLE_END_FREQ_KHZ_LOW ((950) * (KHZ))

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
    CAT scan type
    */
  cat_scan_type_t cat_scan_type;
  /*!
    Sat list for saving tp information in TP scan
    */
  scan_sat_info_t perset_sat_list[MAX_SAT_NUM_SUPPORTED];
  /*!
    NC service handle
    */
  service_t *p_nc_svc;
  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;  
  /*!
    blind scan times
    */
  u8 bs_times;

  /*!
    the index of locked tp
 */
  u16 tp_index_locked[MAX_TP_NUM_PER_SAT / 2];

 /*!
     lock channel number
 */
  u16 channel_count_locked;
  /*!
     the index of current locked tp
 */
  u16 cur_locked_channel_idx;
  /*!
    The channel blind scan information
  */
  nim_scan_info_t scan_info ;

   /*!
    blindscan semaphore
  */
  os_sem_t blind_scan_sem;
   /*!
    define a flag for every sat blindscan
  */
  u8 cur_sat_finish_flag;
   /*!
    the index of last sat
  */
  u8 last_sat_idx;
   /*!
    blindscan over flag
  */
  BOOL  scan_over_flag;
    /*!
    unlocked channel
  */
  nim_channel_info_t  cur_channel_unlocked;
   /*!
     locked channel
  */
  nim_channel_info_t  cur_channel_locked;  
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

#ifdef BS_DEBUG_ON
static u32 g_start_ticks;
static u32 g_lock_ticks;
static u32 g_tpdone_ticks;
static u32 g_used_ticks;
#endif
static BOOL sem_status = FALSE;
static os_sem_t g_sem_handle = 0;
static BOOL cancel_flag = FALSE;

static BOOL sem_status_pause = FALSE;
static os_sem_t g_sem_handle_pause = 0;
static BOOL pause_flag = FALSE;
  
//static bl_scan_priv_t *p_priv_cp = NULL;//copy the blindscan pointer


#define BS_LOCK() bs_lock1((char *)__FUNCTION__, __LINE__)
#define BS_UNLOCK() bs_unlock1((char *)__FUNCTION__, __LINE__)

#define BS_LOCK_PAUSE() bs_lock2((char *)__FUNCTION__, __LINE__)
#define BS_UNLOCK_PAUSE() bs_unlock2((char *)__FUNCTION__, __LINE__)

static RET_CODE blind_scan_call_back_fun(nim_msg_t msg, void *p_data);

static void bs_lock1(char *s1, u32 s2)
{
  if(g_sem_handle != 0)
  {
    OS_PRINTF("Scan Dbg %s, %s %d\n", __FUNCTION__, s1, s2);
    sem_status = TRUE;
    
    mtos_sem_take(&g_sem_handle, 0);
    OS_PRINTF("mtos sem someone gived!!!!");
    sem_status = FALSE;
  }
}


static void bs_unlock1(char *s1, u32 s2)
{
  if(g_sem_handle != 0)
  {
    //if(sem_status)
    {
      OS_PRINTF("Scan Dbg %s, %s %d\n", __FUNCTION__, s1, s2);
      mtos_sem_give(&g_sem_handle);
    }
  }
}

static void bs_lock2(char *s1, u32 s2)
{
  if(g_sem_handle_pause != 0)
  {
    OS_PRINTF("Scan Dbg %s, %s %d\n", __FUNCTION__, s1, s2);
    sem_status_pause = TRUE;
    
    mtos_sem_take(&g_sem_handle_pause, 0);
    OS_PRINTF("mtos sem someone gived!!!!");
    sem_status_pause = FALSE;
  }
}


static void bs_unlock2(char *s1, u32 s2)
{
  if(g_sem_handle_pause != 0)
  {
    if(sem_status_pause)
    {
      OS_PRINTF("Scan Dbg %s, %s %d\n", __FUNCTION__, s1, s2);
      mtos_sem_give(&g_sem_handle_pause);
    }
  }
}

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
  u32 scan_times_unit = scan_sat_unit /p_priv->total_bs_times;
  u32 cur_freq_mhz = p_priv->cur_channel_locked.frequency / 1000;
  u32 star_freq_mhz = p_priv->bs_cfg_info[p_priv->cur_bs_times].start_fre_khz / 1000;
  u32 end_freq_mhz = p_priv->bs_cfg_info[p_priv->cur_bs_times].end_fre_khz / 1000;
  u32 step = 0;

  if(p_priv->lst_fre_khz <p_priv->cur_channel_unlocked.frequency)
  {
    p_priv->lst_fre_khz = p_priv->cur_channel_unlocked.frequency;
  }
  if(p_priv->cur_channel_locked.frequency > p_priv->lst_fre_khz)
  {
    cur_freq_mhz = p_priv->cur_channel_locked.frequency / 1000;
  }
  else
  {
    cur_freq_mhz = p_priv->lst_fre_khz / 1000;
  }
  p_priv->lst_fre_khz = cur_freq_mhz * 1000;

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
  #if 1
    BS_DBG("BS cur_sat_idx %d, scan_sat_unit %d, bs time %d, total time %d, time unit %d, "
      "ch_idx %d, x %d, y %d\n",
    p_priv->cur_sat_idx, scan_sat_unit, p_priv->cur_bs_times,
    p_priv->total_bs_times, scan_times_unit,
    p_priv->channel_count,
    (p_priv->lst_fre_khz / 1000 - star_freq_mhz), (end_freq_mhz - star_freq_mhz));
  #endif
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
    OS_PRINTF("use unicable\n");
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
      blind_scan_info.start_fre_khz = p_sat_para->tuner_type == 1 ? BS_C_SINGLE_START_FREQ_KHZ_LOW : BS_C_SINGLE_START_FREQ_KHZ;
      blind_scan_info.end_fre_khz = p_sat_para->tuner_type == 1 ? BS_C_SINGLE_END_FREQ_KHZ_LOW : BS_C_SINGLE_END_FREQ_KHZ;
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
      blind_scan_info.start_fre_khz = p_sat_para->tuner_type == 1 ? BS_C_DOUBLE_START_FREQ_KHZ_LOW : BS_C_DOUBLE_START_FREQ_KHZ;
      blind_scan_info.end_fre_khz = p_sat_para->tuner_type == 1 ? BS_C_DOUBLE_END_FREQ_KHZ_LOW : BS_C_DOUBLE_END_FREQ_KHZ;
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
#ifdef BS_DEBUG_ON
  {
    u8 i;
    for(i = 0;i < total_scan_times; i++)
    {
      BS_DBG("blind cfg[%d],start[%d],end[%d]\n",
        i,p_priv->bs_cfg_info[i].start_fre_khz,p_priv->bs_cfg_info[i].end_fre_khz);
    }
  }
  BS_DBG("total_scan_times=%d\n",total_scan_times);
#endif
  return total_scan_times;
}

#ifdef BS_DEBUG_ON
static void print_diseqcinfo(nc_diseqc_info_t *p_diseqc)
{
  OS_PRINTF("\n%s\n",__FUNCTION__);
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

static void switch_to_sat(bl_scan_priv_t *p_priv, u8 index)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + index;
  BOOL need_waiting = FALSE;
  p_priv->tuner_id = p_sat->tuner_id; 
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
#ifdef BS_DEBUG_ON
  print_diseqcinfo(&p_sat->diseqc_info);
#endif
  
  //Set nim ctrl blind scan mode into bs mode
  nc_set_blind_scan_mode(nc_handle, p_priv->tuner_id, TRUE);
  nc_enter_monitor_idle(nc_handle, p_priv->tuner_id);
  
  if(!p_sat->diseqc_info.is_fixed)
  {
    need_waiting = TRUE;
    nc_set_diseqc(nc_handle, p_priv->tuner_id, &p_sat->diseqc_info);
  }
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
  p_priv->channel_count = 0;
  p_priv->cur_bs_times = 0;
  p_priv->channel_count_locked = 0;
  p_priv->cur_locked_channel_idx = 0;
  p_priv->scan_info.channel_num =0;
  p_priv->scan_info.max_count = MAX_TP_NUM_PER_SAT;
  p_priv->scan_over_flag = FALSE;
  OS_PRINTF("Scan Dbg %s, sem_status %d, flag %d\n", __FUNCTION__, sem_status,
            p_priv->scan_over_flag);

  p_priv->total_bs_times = blind_scan_config(p_priv, p_sat);
  p_priv->scan_info.start_freq = p_priv->bs_cfg_info[p_priv->cur_bs_times].start_fre_khz;
  p_priv->scan_info.end_freq = p_priv->bs_cfg_info[p_priv->cur_bs_times].end_fre_khz;
  p_priv->cur_fre_khz = p_priv->bs_cfg_info[p_priv->cur_bs_times].start_fre_khz;
  memset(&p_priv->cur_channel_unlocked, 0, sizeof(nim_channel_info_t));
  p_priv->p_dvb_svc->do_cmd(p_priv->p_dvb_svc, DVB_TABLE_TS_UPDATE, 0,  p_priv->ts_in);
  nim_notify_register(p_priv->p_dev[p_priv->tuner_id],blind_scan_call_back_fun);
}

static RET_CODE on_locked(bl_scan_priv_t *p_priv, nc_channel_info_t *p_ch)
{
  tp_rcv_para_t *p_tp_para = &p_priv->cur_tp_info.tp_info;
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;
  RET_CODE ret = ERR_FAILURE;
  bs_cfg_t *p_cur_cfg = p_priv->bs_cfg_info + p_priv->cur_bs_times;

  if(C_BAND == p_cur_cfg->freqBand &&
    DOUBLE_LOCAL_OSCILLATOR == p_cur_cfg->localOscillator)
  {
    p_ch->polarization =
      p_ch->channel_info.frequency > (1550 * KHZ) ? 1 : 0;
    BS_DBG("$$$$ %s,line %d,pol=%d\n",__FUNCTION__,__LINE__,p_ch->polarization);
  }
  else
  {
    p_ch->polarization = p_cur_cfg->polarization;
    BS_DBG("$$$$ %s,line %d,pol=%d\n",__FUNCTION__,__LINE__,p_ch->polarization);
  }

  memset(&p_priv->cur_tp_info, 0, sizeof(scan_tp_info_i_t));
  p_tp_para->freq = bs_calc_down_frq(p_ch, p_sat);
  p_tp_para->sym = p_ch->channel_info.param.dvbs.symbol_rate;
  p_tp_para->polarity = p_ch->polarization;
  p_tp_para->is_on22k = p_ch->onoff22k;
  p_tp_para->nim_type = p_ch->channel_info.param.dvbs.nim_type;
  p_priv->cur_tp_info.sat_id = p_sat->sat_id;
  p_priv->cur_tp_info.can_locked = 1;
  p_priv->cur_tp_info.tp_origin = TP_FROM_SCAN;
  memcpy(&p_priv->cur_tp_info.perf, &p_ch->channel_info.param.dvbs.perf,
    sizeof(nim_channel_perf_t));
  BS_DBG("agc[%ld], snr[%ld],ber[%d]\n", p_priv->cur_tp_info.perf.agc,
    p_priv->cur_tp_info.perf.snr, (u8)p_priv->cur_tp_info.perf.ber);
  //add perf log
  log_perf(LOG_AP_SCAN, PERF_SAVE_DB_START,LOCK_TP_DB,0);
  ret = p_priv->hook.process_tp(&p_priv->cur_tp_info);
  log_perf(LOG_AP_SCAN, PERF_SAVE_DB_DONE,LOCK_TP_DB,0);
  //Notify progress
  notify_progress(progress_calc(p_priv));
  return ret;
}

static RET_CODE blind_scan_call_back_fun(nim_msg_t msg, void *p_data)
{
  nim_channel_info_t *p_channel_info = p_data;
  //nc_channel_info_t  p_nc_channel_info;
  os_msg_t os_msg = {0};

  os_msg.para1 = (u32)p_channel_info;
  os_msg.para2 = sizeof(nim_channel_info_t);
  
  switch(msg)
  {
    case NIM_BSTpLocked:
      BS_DBG("\n $$$$ CB locked , the freq is %ld ,sym is %ld, pol %d, nim type %d\n",
        p_channel_info->frequency,
        p_channel_info->param.dvbs.symbol_rate,
        p_channel_info->spectral_polar, 
        p_channel_info->param.dvbs.nim_type);
      os_msg.content = BLIND_SCAN_EVT_LOCKED;
      ap_frm_send_intral_msg(APP_SCAN, &os_msg);
      if(!cancel_flag)
        BS_LOCK();
#ifdef BS_DEBUG_ON
      g_lock_ticks = mtos_ticks_get();
      //BS_DBG("***************take ticks %d\n",g_lock_ticks);
#endif
      break;
    case NIM_BSTpUnlock:
      BS_DBG("\n $$$$ CB unlocked , the freq is %ld ,sym is %ld, pol %d\n",
        p_channel_info->frequency, p_channel_info->param.dvbs.symbol_rate, 
        p_channel_info->spectral_polar);
     
      if(pause_flag)
        BS_LOCK_PAUSE();

      os_msg.content = BLIND_SCAN_EVT_UNLOCKED;
      ap_frm_send_intral_msg(APP_SCAN, &os_msg);
      break;
    case NIM_BSFinish:
      BS_DBG("\n $$$$ CB Finished \n");
      os_msg.content = BLIND_SCAN_EVT_FINISHED;
      ap_frm_send_intral_msg(APP_SCAN, &os_msg);
     break;
    default:
      break;
  }
  return SUCCESS;
}

static RET_CODE on_start(bl_scan_priv_t *p_priv, os_msg_t *p_msg)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  dvb_t * p_dvb_handle = class_get_handle_by_id(DVB_CLASS_ID);
  
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;
  p_priv->tuner_id = p_sat->tuner_id; 
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
  if(p_priv->hook.on_start != NULL)
  {
    p_priv->hook.on_start();
  }
  p_priv->p_nc_svc = nc_get_svc_instance(nc_handle, APP_SCAN);
  p_priv->p_dvb_svc =  p_dvb_handle->get_svc_instance(p_dvb_handle, APP_SCAN);

  p_priv->bs_times = (u8)p_msg->para2;

  //Set nim ctrl blind scan mode into bs mode
  //nc_set_blind_scan_mode(nc_handle, p_priv->tuner_id, TRUE);
  //nc_enter_monitor_idle(nc_handle, p_priv->tuner_id);

  //Init Parameter
  p_priv->cur_sat_idx = 0;
  p_priv->last_sat_idx = 0xff;
  p_priv->scan_progress = 0;
  cancel_flag = FALSE;
  pause_flag = FALSE;
  switch_to_sat(p_priv, p_priv->cur_sat_idx);
  //nim_notify_register(p_priv->p_dev[p_priv->tuner_id],blind_scan_call_back_fun);
#ifdef BS_DEBUG_ON
  g_start_ticks = mtos_ticks_get();
  OS_PRINTF("***************blind_scan_start time,[%d]\n",
    g_start_ticks);
  g_used_ticks = 0;
#endif
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

static RET_CODE on_unlocked_evt(bl_scan_priv_t *p_priv, nim_channel_info_t *p_ch)
{
  BS_DBG("\n $$$$ on_unlocked_evt 1\n");  
  if(cancel_flag)
  {
    if(pause_flag)
      BS_UNLOCK_PAUSE();
    return SUCCESS;
  }
  BS_DBG("\n $$$$ on_unlocked_evt 2\n");  
  memcpy(& p_priv->cur_channel_unlocked, p_ch, sizeof(nim_channel_info_t));
  //add perf log
  log_perf(LOG_AP_SCAN, PERF_GET_TP_END,
  p_ch->frequency,0);

  log_perf(LOG_AP_SCAN, PERF_UNLOCK_EVT,
   p_ch->frequency, 0);
  log_perf(LOG_AP_SCAN, PERF_GET_ONETP_END,0,0);
  log_perf(LOG_AP_SCAN, PERF_GET_ONETP_BEGIN,0,0);
  log_perf(LOG_AP_SCAN, PERF_GET_TP_BEGIN,0,0);

  log_perf(LOG_AP_SCAN, PERF_SAVE_DB_START,UNLOCK_TP_DB,0);
  log_perf(LOG_AP_SCAN, PERF_SAVE_DB_DONE,UNLOCK_TP_DB,0);
  //Notify progress 
  notify_progress(progress_calc(p_priv));

  return SUCCESS;
}

static RET_CODE on_locked_evt(bl_scan_priv_t *p_priv, nim_channel_info_t *p_ch)
{
  nc_channel_info_t  p_nc_channel_info = {0};
  RET_CODE  ret = SUCCESS;
  bs_cfg_t *p_cur_cfg = p_priv->bs_cfg_info + p_priv->cur_bs_times;

  BS_DBG("\n $$$$ on_locked_evt 1\n");
  if(cancel_flag)
  {
    OS_PRINTF("%s,%s,cancel!\n",__FILE__,__FUNCTION__);
    return ERR_FAILURE;
  }
  BS_DBG("\n $$$$ on_locked_evt 2\n");
  if(p_priv->channel_count >= MAX_TP_NUM_PER_SAT)
  {
    OS_PRINTF("$$$$ %s,%s,channel_map is full!\n",__FILE__,__FUNCTION__);
    return ERR_FAILURE;
  }
  if((p_ch->frequency + p_ch->param.dvbs.symbol_rate / 2 < p_cur_cfg->start_fre_khz)
      || (p_ch->frequency - p_ch->param.dvbs.symbol_rate / 2 > p_cur_cfg->end_fre_khz))
  {
    OS_PRINTF("$$$$ %s,%s,illegal freq!\n",__FILE__,__FUNCTION__);
    return ERR_FAILURE;
  }  
  memcpy(&p_priv->channel_map[ p_priv->channel_count], p_ch, sizeof(nim_channel_info_t));
  memcpy(&p_priv->cur_channel_locked, p_ch, sizeof(nim_channel_info_t));
  p_priv->tp_index_locked[p_priv->channel_count_locked] = p_priv->channel_count;
  memcpy(&p_nc_channel_info.channel_info, p_ch, sizeof(nim_channel_info_t));
  p_nc_channel_info.polarization = p_cur_cfg->polarization;
  p_nc_channel_info.onoff22k = p_cur_cfg->onoff22k;
  if(p_priv->hook.check_tp(&p_nc_channel_info) != SUCCESS)
  {
    OS_PRINTF("$$$$ %s,%s,tp is illegal!\n",__FILE__,__FUNCTION__);
    return ERR_FAILURE;
  }
  log_perf(LOG_AP_SCAN, PERF_GET_TP_END, p_ch->frequency, 0);
  log_perf(LOG_AP_SCAN, PERF_LOCK_EVT, (u32)(nc_channel_info_t *)(&p_nc_channel_info), 0);

  ret = on_locked(p_priv, &p_nc_channel_info);
  if(ret == SUCCESS)
  {
    p_priv->channel_count++;
    p_priv->channel_count_locked++;
  }

  return ret;

}

static RET_CODE on_finished_evt(bl_scan_priv_t *p_priv)
{
  BS_DBG("\n $$$$ on_finished_evt 1\n");  
  if(cancel_flag)
  {
    return SUCCESS;
  }
  BS_DBG("\n $$$$ on_finished_evt 2\n");  
  p_priv->cur_sat_finish_flag = FIND_DONE;
  if((!(p_priv->cur_sat_idx + 1 < p_priv->total_preset_sat))&&\
          (!(p_priv->cur_bs_times < p_priv->total_bs_times -1)))
  {
    p_priv->scan_over_flag = TRUE;
    OS_PRINTF("Scan Dbg %s, sem_status %d, flag %d\n", __FUNCTION__, sem_status,
        p_priv->scan_over_flag);
  } 
  return SUCCESS;
}

static void pre_blind_scan(bl_scan_priv_t *p_priv)
{
  bs_cfg_t *p_cur_cfg = p_priv->bs_cfg_info + p_priv->cur_bs_times;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;

  p_priv->tuner_id = p_sat->tuner_id; 
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
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
    BS_DBG("blind cfg[%d],onoff[%d],polarization[%d]\n",
      p_priv->cur_bs_times, p_cur_cfg->onoff22k, p_cur_cfg->polarization);
  }
  nc_set_diseqc(nc_handle, p_priv->tuner_id, &p_sat->diseqc_info);

  p_priv->lst_fre_khz  = p_cur_cfg->start_fre_khz;
  p_priv->cur_sat_finish_flag = 0;//init the flag when start blind scan
  p_priv->scan_info.start_freq = p_cur_cfg->start_fre_khz;
  p_priv->scan_info.end_freq = p_cur_cfg->end_fre_khz;
  memcpy(&(p_priv->scan_info.uc_param), &(p_cur_cfg->unicable_param), 
                 sizeof(nim_unicable_param_t));
  memset(&p_priv->cur_channel_unlocked, 0, sizeof(nim_channel_info_t));
  memset(&p_priv->cur_channel_locked, 0, sizeof(nim_channel_info_t));
  BS_DBG("pre blind cfg[%d],start[%d],end[%d]\n",
    p_priv->cur_bs_times, p_priv->scan_info.start_freq, p_priv->scan_info.end_freq);
  nim_blind_scan_start(p_priv->p_dev[p_priv->tuner_id], &p_priv->scan_info);//start nim scan
}

static void blind_scan_next_time_process(bl_scan_priv_t *p_priv)
{
  bs_cfg_t *p_cur_cfg = p_priv->bs_cfg_info + p_priv->cur_bs_times;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;

  p_priv->tuner_id = p_sat->tuner_id; 
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
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
     BS_DBG("blind cfg[%d],onoff[%d],polarization[%d]\n",
       p_priv->cur_bs_times, p_cur_cfg->onoff22k, p_cur_cfg->polarization);
  }
  nc_set_diseqc(nc_handle, p_priv->tuner_id, &p_sat->diseqc_info);

  p_priv->lst_fre_khz  = p_cur_cfg->start_fre_khz;
  p_priv->scan_info.start_freq = p_cur_cfg->start_fre_khz;
  p_priv->scan_info.end_freq = p_cur_cfg->end_fre_khz;
  memcpy(&(p_priv->scan_info.uc_param), &(p_cur_cfg->unicable_param), 
    sizeof(nim_unicable_param_t));
  memset(&p_priv->cur_channel_unlocked, 0, sizeof(nim_channel_info_t));
  memset(&p_priv->cur_channel_locked, 0, sizeof(nim_channel_info_t));
  BS_DBG("next blind cfg[%d],start[%d],end[%d]\n",
    p_priv->cur_bs_times, p_priv->scan_info.start_freq, p_priv->scan_info.end_freq);
  nim_blind_scan_start(p_priv->p_dev[p_priv->tuner_id], &p_priv->scan_info);//start nim scan
  log_perf(LOG_AP_SCAN,PERF_DO_LOCK, 0, 0);

}

static find_tp_ret_t find_tp(bl_scan_priv_t *p_priv,
  nc_channel_info_t *p_channel)
{
  u16 index = p_priv->cur_locked_channel_idx;
  bs_cfg_t *p_cur_cfg = NULL;
   while(TRUE)
  {
    if(index < p_priv->channel_count_locked)
    {
    
      p_cur_cfg = p_priv->bs_cfg_info + p_priv->cur_bs_times;
      memcpy(&p_channel->channel_info,
        p_priv->channel_map +p_priv->tp_index_locked[index], sizeof(nim_channel_info_t));
       p_priv->cur_locked_channel_idx++;
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
    //log_perf(LOG_AP_SCAN, PERF_PRE_SCAN, 0, 0);
    //start nim scan
    if((p_priv->channel_count_locked == 0)&& (p_priv->last_sat_idx != p_priv->cur_sat_idx))
     {
        p_priv->last_sat_idx = p_priv->cur_sat_idx;
        pre_blind_scan(p_priv);
     }
    else
     {
        return FIND_BUSY;
      }
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
      BS_DBG("%s,line %d,[%d],[%d]\n", __FUNCTION__, __LINE__, interval, p_priv->disp_move_time);
      p_priv->b_disp_moving = FALSE;
    }
    else
    {
      //Skeep Moving
      return FIND_BUSY;
    }
  }

  ret = find_tp(p_priv, p_channel);
  if(FIND_SUC == ret) //found a new tp
  {
      return FIND_SUC;
  }
  else if(FIND_DONE == p_priv->cur_sat_finish_flag) //cur sat found finish
  {
      
    //go to next time
    if(p_priv->cur_bs_times < p_priv->total_bs_times - 1)
    {
       p_priv->scan_over_flag = FALSE;
       p_priv->cur_sat_finish_flag = 0;
       p_priv->cur_bs_times++;
       blind_scan_next_time_process(p_priv);
       return FIND_BUSY;
    }
    else
    {
      p_priv->cur_sat_idx++;
      if(p_priv->cur_sat_idx < p_priv->total_preset_sat)//has the next
      {
        //goto the next satellite
        p_priv->scan_over_flag = FALSE;
        switch_to_sat(p_priv, p_priv->cur_sat_idx);
        return FIND_BUSY;
      }
      else
      {
        notify_progress(100);
        return FIND_DONE;
      }
    }
  }
  else if(FIND_BUSY == ret) //need retry, go on
  {
    return FIND_BUSY;
  }


  return FIND_DONE;
}

static RET_CODE blind_scan_msg_proc(void *p_data, os_msg_t *p_msg)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
  RET_CODE ret = SUCCESS;
  switch(p_msg->content)
  {
    case BLIND_SCAN_EVT_LOCKED:
      ret = on_locked_evt(p_priv, (nim_channel_info_t *)p_msg->para1);
      if(ret == ERR_FAILURE)
        BS_UNLOCK();
      break;
    case BLIND_SCAN_EVT_UNLOCKED:
      ret = on_unlocked_evt(p_priv, (nim_channel_info_t *)p_msg->para1);
      break;
    case BLIND_SCAN_EVT_FINISHED:
      ret = on_finished_evt(p_priv);
    case DVB_NIT_FOUND:
      ret = on_nit(p_priv);
      break;
    case DVB_CAT_FOUND:
      ret = p_priv->hook.process_cat((void *)(p_msg->para1));
      break;
    case SCAN_CMD_START_SCAN:
      ret = on_start(p_priv, p_msg);
      break;
    case SCAN_CMD_PAUSE_SCAN:
      pause_flag = TRUE;
      break;
    case SCAN_CMD_RESUME_SCAN:
      pause_flag = FALSE;
      BS_UNLOCK_PAUSE();
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
  u8 ret = 0;
  //Notify progress
  notify_progress(progress_calc(p_data));

  tp_info.pg_num = p_tp->pg_num;
  tp_info.p_pg_list = p_tp->p_pg_list;
  BS_UNLOCK();
  
  /*BS_DBG("%s,line %d:tp [%d,%d]\n",__FUNCTION__,__LINE__,
    p_tp->tp_info.channel_info.frequency,p_tp->tp_info.channel_info.param.dvbs.symbol_rate);*/
  //return p_priv->hook.process_pg_list(&tp_info);
#ifdef BS_DEBUG_ON
  g_tpdone_ticks = mtos_ticks_get();
  /*BS_DBG("***************tp done ticks %d,used ticks[%d]\n",
    g_tpdone_ticks, g_tpdone_ticks - g_lock_ticks);*/
  g_used_ticks += (g_tpdone_ticks - g_lock_ticks);
#endif
  ret = p_priv->hook.process_pg_list(&tp_info);
  log_perf(LOG_AP_SCAN,PERF_DO_LOCK, 0, 0);
  return ret;
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
  u32 end_ticks = 0;

  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;
  p_priv->tuner_id = p_sat->tuner_id; 
  nc_get_ts_by_tuner(nc_handle, p_priv->tuner_id, &p_priv->ts_in);
  //blindscan over 
  OS_PRINTF("Scan Dbg %s, sem_status %d, flag %d\n", __FUNCTION__, sem_status,
            p_priv->scan_over_flag);
  if(!p_priv->scan_over_flag)
  {
    nim_blind_scan_cancel(p_priv->p_dev[p_priv->tuner_id]);
    cancel_flag = TRUE;
  }

  BS_UNLOCK();
  mtos_sem_destroy(&g_sem_handle, 0);  
  g_sem_handle = 0;

  BS_UNLOCK_PAUSE();
  mtos_sem_destroy(&g_sem_handle_pause, 0);  
  g_sem_handle_pause = 0;

  nc_set_blind_scan_mode(nc_handle, p_priv->tuner_id, FALSE);
  nc_clr_tp_info(nc_handle, p_priv->tuner_id);

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
  end_ticks = mtos_ticks_get();
#ifdef BS_DEBUG_ON
  OS_PRINTF("***************blind_scan_end\n");
  OS_PRINTF("end time,[%d],used time[%d]\n",
    end_ticks, (end_ticks - g_start_ticks) / 100);
  OS_PRINTF("g_used_ticks[%d,%ds]\n", g_used_ticks, g_used_ticks / 100);
#endif
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
    case DVB_TABLE_ID_CAT:
       if(p_priv->cat_scan_type != CAT_SCAN_WITHOUT)
       {
         ret_value = TRUE;
       }
       break;
    default:
      break;
  }
  return ret_value;
}

static u8 bl2_get_bouquet_id_list(void *p_policy, void *p_list)
{
  scan_policy_t *p_scan_policy = p_policy;
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_scan_policy->p_priv_data;
  memcpy(p_list, p_priv->bouquet_id, p_priv->bouquet_num * sizeof(u16));
  return p_priv->bouquet_num;
}

blindscan_ver_t blind_scan_get_version(void *p_data)
{
  return BLIND_SCAN_VER_2;
}

static void blind_scan_get_scan_info(void *p_data, u8 *p_tuner_id, u8 *p_ts_in)
{
  bl_scan_priv_t *p_priv = (bl_scan_priv_t *)p_data;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  scan_sat_info_t *p_sat = p_priv->perset_sat_list + p_priv->cur_sat_idx;
  *p_tuner_id = p_sat->tuner_id;  
  nc_get_ts_by_tuner(nc_handle, p_sat->tuner_id, p_ts_in);
}

policy_handle_t construct_bl_scan_policy_v2(scan_hook_t *p_hook,
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
  p_priv->bl_scan_policy.repeat_tp_check = NULL;
  p_priv->bl_scan_policy.get_scan_scramble_origin = blind_get_scan_scramble_origin;
  //p_priv->bl_scan_policy.performance = blind_scan_performance;
  p_priv->bl_scan_policy.get_blindscan_ver = blind_scan_get_version;
  p_priv->bl_scan_policy.get_bouquet_id_list = bl2_get_bouquet_id_list;
  p_priv->bl_scan_policy.get_scan_info = blind_scan_get_scan_info;

  //Attach implementation policy
  memcpy(&p_priv->hook, p_hook, sizeof(scan_hook_t));

  //Load parameter
  p_priv->total_preset_sat = p_data->total_sat;
  p_priv->nit_scan_type = p_data->nit_scan_type;
  p_priv->bat_scan_type = p_data->bat_scan_type;
  p_priv->cat_scan_type = p_data->cat_scan_type;
  p_priv->bouquet_num = p_data->bouquet_num;
  p_priv->scramble_scan_origin = p_data->scramble_scan_origin;
  memcpy(p_priv->bouquet_id, p_data->bouquet_id, sizeof(u16) * p_data->bouquet_num);

  if(0 == g_sem_handle)
  {
    mtos_sem_create(&g_sem_handle, 0);
  }

  sem_status = FALSE;

  if(0 == g_sem_handle_pause)
  {
    mtos_sem_create(&g_sem_handle_pause, 0);
  }

  sem_status_pause = FALSE;

  CHECK_FAIL_RET_NULL(p_priv->total_preset_sat <= MAX_SAT_NUM_SUPPORTED);
  memcpy(p_priv->perset_sat_list, p_data->sat_list,
    sizeof(scan_sat_info_t)*p_priv->total_preset_sat);

  return (policy_handle_t)&p_priv->bl_scan_policy;
}

