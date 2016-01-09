/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "err_check_def.h"

#define UNS_LOW_LIMIT (11700)

band_type_t dvbs_detect_lnb_freq(u32 freq)
{
  if(freq > DVBS_KU_BAND_MIN_MHZ
    && freq < DVBS_KU_BAND_MAX_MHZ)
  {
    return KU_BAND;
  }
  if(freq > DVBS_C_BAND_MIN_MHZ
    && freq < DVBS_C_BAND_MAX_MHZ)
  {
    return C_BAND;
  }
  OS_PRINTF("\n freq[%d]\n", freq);
  CHECK_FAIL_RET_ZERO(0);
  return 0;
}


u16 dvbs_calc_down_freq(tp_rcv_para_t *p_tp, sat_rcv_para_t *p_sat)
{
  u16 mid_freq = 0;
  u8 polarity = 0;
  u8 k22 = 0;
  u16 down_freq = 0;

  //__asm__ volatile ("mqiu:\n"); filer
  //__asm__ volatile ("nop \n");
  CHECK_FAIL_RET_ZERO(NULL != p_tp);
  CHECK_FAIL_RET_ZERO(NULL != p_sat);

  mid_freq = (u16)p_tp->freq;
  CHECK_FAIL_RET_ZERO(0 != mid_freq);
  polarity = (u8)p_tp->polarity;
  // if the type is UNIVERSAL, calc with tp 22k
  k22 = (DVBS_LNB_UNIVERSAL == p_sat->lnb_type) ?
        ((u8)p_tp->is_on22k) : ((u8)p_sat->k22);

  if(DVBS_LNB_STANDARD == p_sat->lnb_type)
  {
    if(C_BAND == dvbs_detect_lnb_freq(p_sat->lnb_low))
    {
      return (u16)(p_sat->lnb_low - mid_freq);
    }
    else
    {
      return (u16)(p_sat->lnb_low + mid_freq);
    }
  }
  else if(DVBS_LNB_USER == p_sat->lnb_type)
  {
    if(0 == polarity)
    {
      return (u16)(p_sat->lnb_low - mid_freq);
    }
    else if(1 == polarity)
    {
      return (u16)(p_sat->lnb_high - mid_freq);
    }
    else
    {
      CHECK_FAIL_RET_ZERO(0);
    }
  }
  else if(DVBS_LNB_UNIVERSAL == p_sat->lnb_type)
  {
    if(0 == k22)
    {
      down_freq = p_sat->lnb_low + mid_freq;
      if(down_freq >= UNS_LOW_LIMIT)
      {
        p_tp->is_on22k = 1;
        down_freq = p_sat->lnb_high + mid_freq;
      }
      return down_freq;
    }
    else if(1 == k22)
    {
      down_freq = p_sat->lnb_high + mid_freq;
      if(down_freq < UNS_LOW_LIMIT)
      {
        p_tp->is_on22k = 0;
        down_freq = p_sat->lnb_low + mid_freq;
      }
      return down_freq;
    }
    else
    {
      CHECK_FAIL_RET_ZERO(0);
    }
  }
  else
  {
    OS_PRINTF("Error: bad lnb type\n");
    CHECK_FAIL_RET_ZERO(0);
  }
  return 0;
}


u32 dvbs_calc_mid_freq(tp_rcv_para_t *p_tp, sat_rcv_para_t *p_sat)
{
  u16 mid_freq = 0;
  u8 polarity = 0;
  u8 k22 = 0;
  u32 temp = 0;

  // __asm__ volatile ("nop \n");

  CHECK_FAIL_RET_ZERO(NULL != p_sat);
  CHECK_FAIL_RET_ZERO(NULL != p_tp);

  mid_freq = (u16)p_tp->freq;
  //close check mid_freq, user need set invalid parameter to tuner,
  //if cur tp without DB
  //CHECK_FAIL_RET_ZERO(0 != mid_freq);
  polarity = (u8)p_tp->polarity;
  //if the type is UNIVERSAL, calc with tp 22k
  // k22 =
  //   (DVBS_LNB_UNIVERSAL == p_sat->lnb_type) ?
  //     ((u8)p_tp->is_on22k) : ((u8)p_sat->k22);

  if(DVBS_LNB_STANDARD == p_sat->lnb_type)
  {
    if(C_BAND == dvbs_detect_lnb_freq(p_sat->lnb_low))
    {
      temp = p_sat->lnb_low - p_tp->freq;
    }
    else
    {
      temp = p_tp->freq - p_sat->lnb_low;
    }
  }
  else if(DVBS_LNB_USER == p_sat->lnb_type)
  {
    if(0 == p_tp->polarity)
    {
      temp = p_sat->lnb_low - p_tp->freq;
    }
    else if(1 == p_tp->polarity)
    {
      temp = p_sat->lnb_high - p_tp->freq;
    }
    else
    {
      CHECK_FAIL_RET_ZERO(0);
    }
  }
  else if(DVBS_LNB_UNIVERSAL == p_sat->lnb_type)
  {
    k22 = (p_tp->freq > UNS_LOW_LIMIT) ? 1 : 0;
    p_tp->is_on22k = k22;

    if(0 == k22)
    {
      temp = p_tp->freq - p_sat->lnb_low;
    }
    else if(1 == k22)
    {
      temp = p_tp->freq - p_sat->lnb_high;
    }
    else
    {
      CHECK_FAIL_RET_ZERO(0);
    }
  }
  else
  {
    OS_PRINTF("Error: bad lnb type\n");
    CHECK_FAIL_RET_ZERO(0);
  }
  return temp * KHZ;
}


static u8 dvbs_calc_unicable_bank(nim_lnb_porlar_t polar, u32 freq_MHz, u8 unicable_type)
{
  if(polar == NIM_PORLAR_HORIZONTAL)
  {
    return (u8)(((freq_MHz > UNS_LOW_LIMIT) ? 0x03 : 0x02) + 4 * unicable_type);
  }
  else
  {
    return (u8)(((freq_MHz > UNS_LOW_LIMIT) ? 0x01 : 0x00) + 4 * unicable_type);
  }
}


void dvbs_calc_search_info(nc_channel_info_t *p_search_info,
                           sat_rcv_para_t *p_sat, tp_rcv_para_t *p_tp)
{
  CHECK_FAIL_RET_VOID(NULL != p_search_info);
  CHECK_FAIL_RET_VOID(NULL != p_sat);
  CHECK_FAIL_RET_VOID(NULL != p_tp);

  if(DVBS_LNB_POWER_13V == p_sat->lnb_power)
  {
    //Vertical
    p_search_info->polarization = NIM_PORLAR_VERTICAL;
  }
  else if(DVBS_LNB_POWER_18V == p_sat->lnb_power)
  {
    //Horizontal
    p_search_info->polarization = NIM_PORLAR_HORIZONTAL;
  }
  else  //DB_DVBS_LNB_POWER_ALL
  {
    if(p_tp->polarity == 0)
    {
      p_search_info->polarization = NIM_PORLAR_HORIZONTAL;
    }
    else if(p_tp->polarity == 1)
    {
      p_search_info->polarization = NIM_PORLAR_VERTICAL;
    }
    else
    {
      CHECK_FAIL_RET_VOID(0);
    }
  }

  p_search_info->channel_info.frequency = dvbs_calc_mid_freq(p_tp, p_sat);
  p_search_info->channel_info.nim_type = p_tp->nim_type;
  p_search_info->channel_info.param.dvbs.symbol_rate = p_tp->sym;
  p_search_info->channel_info.param.dvbs.nim_type = p_tp->nim_type;
  p_search_info->channel_info.param.dvbs.fec_inner = NIM_CR_AUTO;
  p_search_info->channel_info.spectral_polar = NIM_IQ_AUTO;
  p_search_info->channel_info.lock = NIM_CHANNEL_UNLOCK;

  if(p_sat->user_band != 0) //use unicable
  {
    OS_PRINTF("DEBUG : use unicable----\n");
    //calculate the unicable information
    p_search_info->channel_info.param.dvbs.uc_param.use_uc = 1;
    p_search_info->channel_info.param.dvbs.uc_param.user_band = p_sat->user_band - 1; //base on 1
    p_search_info->channel_info.param.dvbs.uc_param.ub_freq_mhz = p_sat->band_freq;
    p_search_info->channel_info.param.dvbs.uc_param.bank =
      dvbs_calc_unicable_bank(p_search_info->polarization, p_tp->freq, p_sat->unicable_type);

    //disable 22k off
    p_search_info->onoff22k = 0;
    //keep lnb power to 13V
    p_search_info->polarization = NIM_PORLAR_VERTICAL;
  }
  else
  {
    //disable unicable info
    memset(&(p_search_info->channel_info.param.dvbs.uc_param), 0, sizeof(nim_unicable_param_t));

    //Set 22k and polarity before enter this API
    p_search_info->onoff22k = (DVBS_LNB_UNIVERSAL == p_sat->lnb_type) ?
                              (u8)p_tp->is_on22k : (u8)p_sat->k22;
  }
  OS_PRINTF("dvbs_calc_search_info: 22k %d, polarization: %d\n",
            p_search_info->onoff22k, p_search_info->polarization);
}


void dvbs_get_down_freq_range(tp_rcv_para_t *p_tp,
                              sat_rcv_para_t *p_sat, u16 *p_min, u16 *p_max)
{
  u16 min = 0;
  u16 max = 0;

  if(DVBS_LNB_STANDARD == p_sat->lnb_type)
  {
    if(C_BAND == dvbs_detect_lnb_freq(p_sat->lnb_low))
    {
      //min = 3400;
      //max = 4200;
      if(p_sat->lnb_low == 5150)
      {
        min = p_sat->lnb_low - 2150;
        max = p_sat->lnb_low - 950;
      }
      else
      {
        min = p_sat->lnb_low - 2150;
        max = p_sat->lnb_low - 950;  
      }

      if(min < 3000)
      {
        min = 3000;
      }
    }
    else
    {
      //min = 12250;//p_sat->lnb_low + 950;
      //max = 12750;//p_sat->lnb_low + 1450;
      min = p_sat->lnb_low + 950;
      max = p_sat->lnb_low + 2150;
    }
  }
  else if(DVBS_LNB_USER == p_sat->lnb_type)
  {
    min = 3000;
    max = 4800;
  }
  else if(DVBS_LNB_UNIVERSAL == p_sat->lnb_type)
  {
    min = p_sat->lnb_low + 950;
    max = p_sat->lnb_high + 2150;
  }
  else
  {
    OS_PRINTF("Error: bad lnb type\n");
    CHECK_FAIL_RET_VOID(0);
  }

  if(min < max)
  {
    *p_min = min;
    *p_max = max;
  }
  else
  {
    *p_min = max;
    *p_max = min;
  }
}
