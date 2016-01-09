/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
//lint -e19
#include "math.h"
//lint +e19
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "lib_util.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "nim.h"
#include "scart.h"
#include "rf.h"
#include "common.h"
#include "lib_rect.h"
#include "display.h"
#include "aud_vsb.h"
#include "vdec.h"
#include "uio.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "avctrl1.h"
#include "db_dvbs.h"
#include "uio.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"

#include "ui_common.h"
#include "ap_framework.h"
#include "ap_signal_monitor.h"
//#include "ap_flounder_ota.h"
#include "sys_status.h"
#include "customer_config.h"

// for strength
#define AVG_NUM 1
#define STRENGTH_MAX 95
#define SIGNAL_STRENGTH_RATIO 100
#define STRENGTH_G_THAN_QUALITY 1 //strength is greater than quality

/* TS2000 strength */
#define TS2000_GAIN_NO_SIGNAL 10600
#define TS2000_GAIN_STREN_60 10280
#define TS2000_GAIN_STREN_70 10200
#define TS2000_GAIN_STREN_80 9500
#define TS2000_GAIN_STREN_95 4500
/* TS2020 strength */
#define TS2020_GAIN_NO_SIGNAL 8200
#define TS2020_GAIN_STREN_60 8235
#define TS2020_GAIN_STREN_70 7300
#define TS2020_GAIN_STREN_80 6000
#define TS2020_GAIN_STREN_95 4000
/* TS2022 strength */
#define TS2022_GAIN_NO_SIGNAL 11500
#define TS2022_GAIN_STREN_60 11380
#define TS2022_GAIN_STREN_70 11290
#define TS2022_GAIN_STREN_80 10000
#define TS2022_GAIN_STREN_95 7000

//for snr
#define QUALITY_MAX 85
#define SNR_0 0
#define SNR_30 26
#define SNR_45 46
#define SNR_60 70
#define SNR_75 90
#define SNR_85 115

extern void lock_led_set(BOOL locked);


#if 0
/*
static void convert_perf_hw(nim_channel_perf_t *p_signal)
{
  struct nim_device_t* p_nim = dev_find_identifier(NULL, DEV_IDT_TYPE,
      SYS_DEV_TYPE_NIM);
  u8 tuner_ver, i;

  u32 gain_no_signal, gain_60, gain_70, gain_80, gain_95;
  u16 snr_0, snr_30, snr_45, snr_60, snr_75, snr_85; 
  u8 strength_per, snr_per = 0;
  u16 sum;
  static u8 signal_strength[AVG_NUM];
  static u8 signal_index = 0;  

  MT_ASSERT(p_signal != NULL);

  //OS_PRINTF("---- signal[%s]: agc[%d] snr[%d], ber[%d] ----\n", 
  //  p_signal->lock ? "lock" : "unlock",
  //  p_signal->agc, p_signal->snr, p_signal->ber);


  //OS_PRINTF("original gain[%d], snr[%d]\n", p_signal->agc, perf_info->snr);

  dev_io_ctrl(p_nim, NIM_IOCTRL_GET_TN_VERSION, (u32)&tuner_ver);
  
  switch(tuner_ver)
  {
    case TS2000:
      gain_no_signal = TS2000_GAIN_NO_SIGNAL;
      gain_60 = TS2000_GAIN_STREN_60;
      gain_70 = TS2000_GAIN_STREN_70;
      gain_80 = TS2000_GAIN_STREN_80;
      gain_95 = TS2000_GAIN_STREN_95;
      break;

    case TS2022:
      gain_no_signal = TS2022_GAIN_NO_SIGNAL;
      gain_60 = TS2022_GAIN_STREN_60;
      gain_70 = TS2022_GAIN_STREN_70;
      gain_80 = TS2022_GAIN_STREN_80;
      gain_95 = TS2022_GAIN_STREN_95;
      break;  
      
    default: //TS2020
      gain_no_signal = TS2020_GAIN_NO_SIGNAL;
      gain_60 = TS2020_GAIN_STREN_60;
      gain_70 = TS2020_GAIN_STREN_70;
      gain_80 = TS2020_GAIN_STREN_80;
      gain_95 = TS2020_GAIN_STREN_95;
      break;
  }

  if(p_signal->lock != 1)
  {
    if(p_signal->agc >= gain_no_signal)		
    {//0%, no signal or weak signal
      strength_per =  0;
    }
    else
    {
      strength_per =  60;
    }
  }
  else
  {
    if(p_signal->agc >= gain_60)
    {
      strength_per =  60;
    }
    else if(p_signal->agc > gain_70)
    {//60%-70%    normal signal
      strength_per = (u8)(60 + (gain_60 - p_signal->agc) * 10 
                             / (gain_60-gain_70));   
    }
    else if(p_signal->agc > gain_80)				
    {//70% - 80%   strong signal
      strength_per = (u8)(70 + (gain_70 - p_signal->agc) * 10 
                            / (gain_70-gain_80));  
    }
    else
    {//80% - 95%   very strong signal
      strength_per = (u8)(80 + (gain_80 - p_signal->agc) * 15 
                            / (gain_80-gain_95));        
    }
  }
  
  signal_strength[signal_index] = strength_per;   
  signal_index++;
  if(signal_index == AVG_NUM)
  {
  	signal_index = 0;
  }

  sum = 0;
  for (i = 0; i < AVG_NUM; i++)
  {
  	sum = (u16)(sum + signal_strength[i]);
  }
  strength_per = (u8)(sum / AVG_NUM);

  //Scale the display by multiplying the signal strength with a coefficient
  strength_per = (u8)((u16)strength_per * SIGNAL_STRENGTH_RATIO / 100); 

  //Limit the display within 0% to 100% to avoid errors
  if(strength_per > 100)  strength_per = 100;        
  
  p_signal->agc = strength_per;


  snr_0 = SNR_0;
  snr_30 = SNR_30;
  snr_45 = SNR_45;
  snr_60 = SNR_60;
  snr_75 = SNR_75;
  snr_85 = SNR_85;

  if(p_signal->snr <= snr_0)
  {
    snr_per = 0;
  }
  else if(p_signal->snr < snr_30)
  {
    snr_per = (u8)(30*(p_signal->snr-snr_0)/(snr_30-snr_0));
  }
  else if(p_signal->snr < snr_45)
  {
    snr_per = (u8)(30 + 15*(p_signal->snr-snr_30)/(snr_45-snr_30));
  }
  else if(p_signal->snr < snr_60)
  {
    snr_per = (u8)(45 + 15*(p_signal->snr-snr_45)/(snr_60-snr_45));
  }
  else if(p_signal->snr < snr_75)
  {
    snr_per = (u8)(60 + 15*(p_signal->snr-snr_60)/(snr_75-snr_60));
  }
  else if(p_signal->snr <= snr_85)
  {
    snr_per= (u8)(75 + 10*(p_signal->snr-snr_75)/(snr_85-snr_75));
  }
  else
  {
    snr_per = 85;
  }
  
  p_signal->snr = (u32)snr_per;
}
*/
#ifdef DTMB_PROJECT

static void convert_perf_to_custom(nim_channel_perf_t *p_signal)
{
  if (p_signal->agc <= 15)
  {
    p_signal->agc = 0;
    
  }
  else
  {
    p_signal->agc = (p_signal->agc - 15)*100/70;
  }
  
  if (p_signal->agc>100)
  {
    p_signal->agc = 100;
  }
}
/*
static void dvbt_convert_perf(nim_channel_perf_t *p_signal)
{
  float agc = p_signal->agc;
  float weight = 0.0;


  // unlock
  if (p_signal->lock != 1) 
  {
    p_signal->snr = 0;
    p_signal->agc = 0;

    goto _RETURN_;
  }
  
  // adjust agc
  if (agc > 100.0)
  {
    agc = 100.0;
  }

  weight = 3 * ((100 - agc) / 100) * (100 - agc) / 100;
  agc = agc * (1 + weight);

  p_signal->agc = agc > 100.0 ? 100 : (u32)agc;

  // adjust snr
  if(p_signal->snr > QUALITY_MAX)
  {
    p_signal->snr = QUALITY_MAX;
  }
  else if(p_signal->snr < 5)
  {
    p_signal->snr = 0;
  }
_RETURN_:
}
*/
#else
static void convert_perf_to_custom(nim_channel_perf_t *p_signal)
{
//  static u8 strength_buf = 0;
  
  if(p_signal->lock != 1)
  {
    p_signal->snr = 0;
  	if(p_signal->agc < 50)
          p_signal->agc = 0;
  	else
  			p_signal->agc = 60;
  }
  else
  {
    if(STRENGTH_G_THAN_QUALITY)
    {
      while(p_signal->agc < p_signal->snr)
      {
        p_signal->agc += ((p_signal->snr - p_signal->agc) / 10 + 1) * 10;
      }
      if((p_signal->agc - p_signal->snr) < 5)
      {
        p_signal->agc += 5;
      }
    }
  	
  	if(p_signal->agc < 60)
  		p_signal->agc = 60;
  }

  if(p_signal->snr > QUALITY_MAX)
  {
    p_signal->snr = QUALITY_MAX;
  }
  else if(p_signal->snr < 5)
  {
    p_signal->snr = 0;
  }
  	
  if(p_signal->agc > STRENGTH_MAX)
  {
    p_signal->agc = STRENGTH_MAX;
  }
  else if(p_signal->agc < 10)
  {
    p_signal->agc = 0;
  }
}
#endif
#endif
static void convert_perf(nim_channel_perf_t *p_signal, sig_mon_info_t *p_info)
{

 // convert_perf_hw(p_signal);
  //convert_perf_to_custom(p_signal);
  //dvbt_convert_perf(p_signal);
  p_info->strength = (u8)p_signal->agc;  //already is U8 when calc in DRV
  p_info->snr = (u8)p_signal->snr;   //already is U8 when calc in DRV
  //p_info->ber = (u8)p_signal->ber;
  p_info->ber_c = p_signal->ber;
}

//static u8 last_status = FALSE;
static void lock_set(BOOL locked,u8 tuner_id)
{
  //static u8 setflag = 0xff;
  
  //if(setflag != locked)
  {
    lock_led_set(locked);
    //setflag = locked;
  }

#if 0
 // if(ui_is_playing())
if(ui_get_app_playback_status()&& ui_is_playing())
 {
    if(locked == FALSE)
    {
      if(last_status != locked)
      {
        OS_PRINTF("\nlast_status %d ,locked : %d ui_stop_play\n",last_status,locked);
        ui_stop_play(1,TRUE);
        ui_play_curn_pg();
      }
    }
  }
  last_status = locked;
  #endif
}

sig_mon_policy_t * construct_sig_mon_policy(void)
{
  sig_mon_policy_t *p_sig_mon_pol = mtos_malloc(sizeof(sig_mon_policy_t));
  MT_ASSERT(p_sig_mon_pol != NULL);

  //lint -e668
  memset(p_sig_mon_pol, 0, sizeof(sig_mon_policy_t));
  //lint +e668

  p_sig_mon_pol->p_convert_perf = convert_perf;
  p_sig_mon_pol->lock_led_set = lock_set;
  
  return p_sig_mon_pol;
}


