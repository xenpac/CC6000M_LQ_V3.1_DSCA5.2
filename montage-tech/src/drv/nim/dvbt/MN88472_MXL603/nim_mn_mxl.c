/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_printk.h"
#include "mem_manager.h"
#include "mtos_mem.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "drv_dev.h"
#include "../../../drvbase/drv_dev_priv.h"
#include "../../../drvbase/drv_svc.h"
#include "../inc/drv/bus/i2c.h"
#include "nim.h"
#include "../../nim_priv.h"

#include "MaxLinearDataTypes.h"
#include "MN_DMD_driver.h"
#include "MN_DMD_device.h"
#include "MN8847x_UserFunction.h"
#include "nim_mn_mxl.h"
#include "MN_I2C.h"
#include "Mxl608_OEM_Drv.h"
os_sem_t g_mn_mxl_i2c_rw_mutex;


static u32 nim_mn_mxl_print_level;
#define DSMN_PRINT(lev, msg...) do {   \
  if ((lev) <= nim_mn_mxl_print_level) {      \
    mtos_printk(msg);   \
  }           \
} while (0)

//extern  MS_BOOL MDrv_Tuner_Init(void);
//extern  void ATBM_INIT(void *p_dmd, void *p_tuner);
//extern  MS_U32 MDrv_Tuner_SetTuner(MS_U32 dwFreq /*Khz*/, MS_U8 ucBw /*MHz*/);

static void mn_dvbt2_demd_init(nim_config_t *p_nim_cfg)
{
	DMD_PARAMETER_t	param;			//Demodulator Parameter
	
	DSMN_PRINT(3,"mn_dvbt2_demd_init\n");
	if( DMD_open(&param) == DMD_E_OK )
		DSMN_PRINT(3,"OK\n");
	else
		DSMN_PRINT(3,"NG\n");

	//Initialize LSI
	DSMN_PRINT(3,"Initializing LSI\n");
	if( DMD_init(&param) == DMD_E_OK )
		DSMN_PRINT(3,"DMD init OK\n");
	else
		DSMN_PRINT(3,"DMD init NG\n");
}

RET_CODE nim_mn_mxl_diseqc_ctrl(lld_nim_t *p_lld,
                                   nim_diseqc_cmd_t *p_diseqc_cmd)
{
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);

  if(p_diseqc_cmd->mode == NIM_DISEQC_BYTES)
  {
    if(p_diseqc_cmd->tx_len == 0 || p_diseqc_cmd->p_tx_buf == NULL)
    {
      return ERR_PARAM;
    }
    MT_ASSERT(p_diseqc_cmd->tx_len <= 8);
    memcpy(priv->cur_diseqc.p_tx_buf,
      p_diseqc_cmd->p_tx_buf, p_diseqc_cmd->tx_len);
  }

  priv->cur_diseqc.mode = p_diseqc_cmd->mode;
  priv->cur_diseqc.tx_len = p_diseqc_cmd->tx_len;
  priv->cur_diseqc.rx_len = p_diseqc_cmd->rx_len;

  if(priv->lnb_onoff == 0)
  {
    return SUCCESS;
  }

  if(priv->onoff_22k != 0)
  {
    /* 22k is opened , close it */
 //   ATBMSetContinuousTone(0);
  }
  /* make sure the 22K OFF stable */
  mtos_task_delay_ms(20);

  if(NIM_DISEQC_BURST0 == p_diseqc_cmd->mode ||
      NIM_DISEQC_BURST1 == p_diseqc_cmd->mode)
  {
  //  ATBMSendToneBurst(NIM_DISEQC_BURST0 - 1);
  }
  else if(p_diseqc_cmd->mode == NIM_DISEQC_BYTES)
  {

    if(priv->diseqc_2x == 1)
    {
      if(p_diseqc_cmd->p_rx_buf == NULL)
      {
        return ERR_PARAM;
      }
  //    ATBMSendDiseqcWithReply(p_diseqc_cmd->p_tx_buf,p_diseqc_cmd->tx_len,
  //                         p_diseqc_cmd->p_rx_buf, &(p_diseqc_cmd->rx_len));
    }
    else
    {
  //    ATBMSendDiseqc(p_diseqc_cmd->p_tx_buf, p_diseqc_cmd->tx_len);
    }
  }
  /* make sure the diseqc commond received completely */
  mtos_task_delay_ms(20);

  if(priv->onoff_22k != 0)
  {
    /* 22k was colsed, open it */
//    ATBMSetContinuousTone(1);
  }

  return 0;
}

static void nim_mn_mxl_set_chninfo(lld_nim_t *p_lld, u32 plp_id)
{
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);
  DMD_PARAMETER_t  *p_param = (DMD_PARAMETER_t  *)priv->mn_mxl_hanlde.mn88472_priv;
	DMD_DVBT_HIERARCHY_t   hierarchy_exist;
  
  if(p_param->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED)
  {
    if(p_param->system == DMD_E_DVBT)
    {
        DSMN_PRINT(3,"DVB-T program play\n");
        priv->mn_mxl_hanlde.p_nim_cfg->nim_type = NIM_DVBT;
        DMD_get_info(p_param, DMD_E_INFO_DVBT_HIERARCHY);			
        hierarchy_exist = p_param->info[DMD_E_INFO_DVBT_HIERARCHY];

        if ( hierarchy_exist > 0) //if no Hierarchy mode transferred, not allowed selected
        {
          DMD_set_info(p_param, DMD_E_INFO_DVBT_HIERARCHY_SELECT, DMD_E_DVBT_HIER_SEL_HP);
        }
        else //default : LP 
        {
          DMD_set_info(p_param, DMD_E_INFO_DVBT_HIERARCHY_SELECT, DMD_E_DVBT_HIER_SEL_LP);
        }
    }
    else if(p_param->system == DMD_E_DVBT2)
    {
		//	DSMN_PRINT(3,"frequency =%d\n",p_channel_info->frequency);
			DSMN_PRINT(3,"DVB-T2 program play plp_id =%d\n",plp_id);
      // only selelt DATA PLP
      priv->mn_mxl_hanlde.p_nim_cfg->nim_type = NIM_DVBT2;
      DMD_get_info(p_param , DMD_E_INFO_DVBT2_NUM_PLP);
			if (p_param->info[DMD_E_INFO_DVBT2_NUM_PLP] > 1) //MPLP
      {  
        if (plp_id + 1 <= p_param->info[DMD_E_INFO_DVBT2_NUM_PLP]) //t2_plp_no starts from '0'.
        {
          //T2 signal, set PLP number,  for SPLP(signal PLP), pls. set PLP number to ZERO. 
          DMD_set_info(p_param , DMD_E_INFO_DVBT2_SELECTED_PLP , plp_id);	
        }
        else
        {
          DSMN_PRINT(3,"--- INVALID PLP SETTING ! MORE THAN PLPNUM---\n");
        }
      }
			else //SPLP, default setting
			{
			  DMD_set_info(p_param , DMD_E_INFO_DVBT2_SELECTED_PLP , 0);	
			}
			/*
			 | When performing a PLP switch reset the demod. Helps prevent any possible latchup
			 | or performance issue that may happen when previously tuned to a "bad" PLP
			*/
			DMD_device_reset(p_param);
    }
  }
}
static u32 nim_mn_mxl_get_lock_st(lld_nim_t *p_lld)
{
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);
  DMD_PARAMETER_t  *p_param = (DMD_PARAMETER_t  *)priv->mn_mxl_hanlde.mn88472_priv;
   
  if(priv->mn_mxl_hanlde.p_nim_cfg->nim_type == NIM_UNDEF)
  {
    DSMN_PRINT(3, "nim type undefined\n");
    return 0;
  }
 // DSMN_PRINT(4," NIM_IOCTRL_CHECK_LOCK    set frequency=%d\n",p_param->freq);
 // DSMN_PRINT(4," NIM_IOCTRL_CHECK_LOCK 	  nim_type=%d\n",p_param->system);
 // DSMN_PRINT(4," NIM_IOCTRL_CHECK_LOCK 	  band_width=%d\n",p_param->bw);
  DMD_get_check_lock(p_param);
  if (p_param->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED)
  {
    DSMN_PRINT(3, "%s LOCK\n",(priv->mn_mxl_hanlde.p_nim_cfg->nim_type == NIM_DVBT2)?"DVBT2":"DVBT");
    return 1;
  }
  DSMN_PRINT(3, "%s UNLOCK\n",(priv->mn_mxl_hanlde.p_nim_cfg->nim_type == NIM_DVBT2)?"DVBT2":"DVBT");
  return 0;
}

static RET_CODE nim_mn_mxl_ioctrl(lld_nim_t *p_lld, u32 cmd, u32 param)
{
//  nim_diseqc_cmd_t diseqc_cmd;
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);
 // u8 LockStatus = 0;
  u8 LnbPinLevel = 0;
  u8 pin = 0;
  u8 level = 0;
  u8 value = 0;
//  DMD_PARAMETER_t  *p_param = (DMD_PARAMETER_t  *)priv->mn_mxl_hanlde.mn88472_priv;
  
  switch(cmd)
  {
    case DEV_IOCTRL_POWER:
      switch(param)
      {
        case DEV_POWER_SLEEP:
          /* sleep demod */
          DMD_set_power_mode(DMD_PWR_MODE_SLEEP);
          break;
        case DEV_POWER_FULLSPEED:
          /* wake up demod */
          DMD_set_power_mode(DMD_PWR_MODE_NORMAL);
          break;
        case DEV_POWER_STANDBY:
          DMD_set_power_mode(DMD_PWR_MODE_STANDBY);
          break;
        default:
          return ERR_PARAM;
      }
      break;
    case NIM_IOCTRL_CHECK_LOCK:
      *((u8 *)param) = nim_mn_mxl_get_lock_st(p_lld);
      break;
    case NIM_IOCTRL_DISEQC1X:
      priv->diseqc_2x = 0;
      break;
    case NIM_IOCTRL_DISEQC2X:
      priv->diseqc_2x = 1;
      break;
    case NIM_IOCTRL_CHANGE_IQ:
      if (0 == priv->cur_channel.spectral_polar)
      {
        priv->cur_channel.spectral_polar = 1;
      }
      else
      {
        priv->cur_channel.spectral_polar = 0;
      }
      break;

    case NIM_IOCTRL_SET_LNB_ONOFF:
      if(priv->lnb_onoff == param)
      {
        break;
      }
      priv->lnb_onoff = (u8)param;
      break;
    case NIM_IOCTRL_SET_22K_ONOFF:
      priv->onoff_22k = (u8)param;
      break;
    case NIM_IOCTRL_GET_22K_ONOFF:
      *((u8 *)param) = LnbPinLevel;
      break;
    case NIM_IOCTRL_GET_TN_VERSION:
      break;
    case NIM_IOCTRL_SET_TS_MODE:
      break;

    case NIM_IOCTRL_CHECK_LNB_SC_PROT:
      if(priv->mn_mxl_hanlde.p_nim_cfg->pin_config.lnb_prot_by_mcu == 1)
      {
        /* select gpio */
        pin = priv->mn_mxl_hanlde.p_nim_cfg->pin_config.lnb_prot_pin;
        level = priv->mn_mxl_hanlde.p_nim_cfg->pin_config.lnb_prot_level;
        hal_pinmux_gpio_enable(pin, TRUE);
        DSMN_PRINT(3, "lnb_prot_pin[%d], level[%d]\n", pin, level);
        gpio_ioctl(GPIO_CMD_IO_ENABLE, pin, TRUE);
        gpio_get_dir(pin, &value);
        if(GPIO_DIR_OUTPUT == value)
        {
          DSMN_PRINT(3, "set lnb_prot_pin input\n");
          gpio_set_dir(pin, GPIO_DIR_INPUT);
        }
        gpio_get_value(pin, &value);
        if(value == level)
        {
          /* already protected */
          *((u8 *)param) = NIM_LNB_SC_PROTING;
        }
        else
        {
          *((u8 *)param) = NIM_LNB_SC_NO_PROTING;
        }
      }
      else
      {
        *((u8 *)param) = NIM_LNB_SC_NO_PROTING;
      }
      break;
    case NIM_IOCTRL_SET_CHANNEL_INFO:
      nim_mn_mxl_set_chninfo(p_lld, param);
      break;
    default:
      return ERR_PARAM;
  }

  return SUCCESS;
}
static DMD_BANDWIDTH_t nim_mn_mxl_select_bw(u16 bw)
{
  switch(bw)
  {
  case 5:
    return DMD_E_BW_5MHZ;
  case 6:
    return DMD_E_BW_6MHZ;
  case 7:
    return DMD_E_BW_7MHZ;
  case 8:
    return DMD_E_BW_8MHZ;
  }
  return  DMD_E_BW_NOT_SPECIFIED;
}

static RET_CODE nim_mn_mxl_channel_connect(lld_nim_t *p_lld,
                             nim_channel_info_t *p_channel_info, BOOL for_bs)
{
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);
  //u8  LockStatus = 0;
  DMD_PARAMETER_t  *p_param = (DMD_PARAMETER_t  *)priv->mn_mxl_hanlde.mn88472_priv;
  u32 cnt = 0;
  p_channel_info->lock = 0;
  DSMN_PRINT(3,"nim_mn_mxl_channel_connect in\n");
	/*demod wake up for working*/
/*
  DSMN_PRINT(3," nim_mn_mxl_channel_connect     IN LockStatus=%d\n", LockStatus);
  DSMN_PRINT(3," nim_mn_mxl_channel_connect    cur_channelfrequency=%d\n",priv->cur_channel.frequency);
  DSMN_PRINT(3," nim_mn_mxl_channel_connect    set frequency=%d\n",p_channel_info->frequency);
  DSMN_PRINT(3," nim_mn_mxl_channel_connect 	 nim_type=%d\n",p_channel_info->param.dvbt.nim_type);
  DSMN_PRINT(3," nim_mn_mxl_channel_connect 	 band_width=%d\n",p_channel_info->param.dvbt.band_width);
  */
  p_param->bw = nim_mn_mxl_select_bw(p_channel_info->param.dvbt.band_width);
  p_param->freq = p_channel_info->frequency*1000;
  p_param->ts_out = DMD_E_TSOUT_SERIAL_VARIABLE_CLOCK;
  memcpy(&priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));
  priv->mn_mxl_hanlde.p_nim_cfg->demod_band_width = p_channel_info->param.dvbt.band_width;
  
  if(p_channel_info->param.dvbt.nim_type == NIM_UNDEF || 
     p_channel_info->param.dvbt.nim_type == NIM_DVBT)
  {
    DSMN_PRINT(3, "nim_mn_mxl_channel_connect DMD_E_DVBT\n");
    p_param->system = DMD_E_DVBT;
    DMD_set_system(p_param);
  	DMD_scan(p_param);	 
    for(cnt = 0; cnt < 1900; cnt+=10)
    {
      DMD_get_check_lock(p_param);
      if(p_param->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED)
      {
        DSMN_PRINT(3,"--- DVB-T system found !!---\n");
        p_channel_info->lock = 1;
        p_channel_info->param.dvbt.nim_type = NIM_DVBT;
        priv->mn_mxl_hanlde.p_nim_cfg->nim_type = NIM_DVBT;
#if 0//for debug
extern void DMD_get_status( DMD_PARAMETER_t *param );
  DMD_get_status(p_param);
#endif
        return SUCCESS;
      }
      else if(p_param->info[DMD_E_INFO_LOCK] == DMD_E_LOCK_NOSIGNAL)
      {
        //no signal
        DSMN_PRINT(3,"--- DVB-T no signal!!---\n");
        break;
      }
      mtos_task_delay_ms(10);
    }    
  }
  DSMN_PRINT(3, "nim_mn_mxl_channel_connect DMD_E_DVBT2\n");
  p_param->system = DMD_E_DVBT2;
  DMD_set_info(p_param, DMD_E_INFO_DVBT2_SELECTED_PLP, 0);	
  DMD_set_system(p_param);
	DMD_scan(p_param);	 
  for(cnt = 0; cnt < 2500; cnt+=10)
  {  
    DMD_get_check_lock(p_param);
    if (p_param->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED)
    {
      DSMN_PRINT(3,"DVB-T2 system LOCK!\n");
      p_channel_info->lock = 1;
      if(p_param->system == DMD_E_DVBT2) 
      {
        u32 i =0;
        p_channel_info->param.dvbt.nim_type = NIM_DVBT2;
        priv->mn_mxl_hanlde.p_nim_cfg->nim_type = NIM_DVBT2;
   			memset(p_channel_info->param.dvbt.DataPlpIdArray, 0, sizeof(p_channel_info->param.dvbt.DataPlpIdArray));
        DMD_get_dataPLPs(p_channel_info->param.dvbt.DataPlpIdArray, &p_channel_info->param.dvbt.DataPlpNumber , p_param);
        for(i = 0; i < p_channel_info->param.dvbt.DataPlpNumber; i++)
        {
          DSMN_PRINT(3,"plp_c =%d DataPlpIdArray[%d]=%d\n",p_channel_info->param.dvbt.DataPlpNumber,i,p_channel_info->param.dvbt.DataPlpIdArray[i]);
        }
        DMD_get_info(p_param , DMD_E_INFO_DVBT2_SELECTED_PLP); 
        p_channel_info->param.dvbt.plp_id = p_param->info[DMD_E_INFO_DVBT2_SELECTED_PLP];
        DSMN_PRINT(3, "nim_mn_mxl_channel_connect current dvbt.plp_id=%d\n",p_channel_info->param.dvbt.plp_id);

        return SUCCESS;
      }
    }
    else if(p_param->info[DMD_E_INFO_LOCK] == DMD_E_LOCK_NOSIGNAL)
    {
      //no signal
      DSMN_PRINT(3,"--- DVB-T2 no signal!!---\n");
      break;
    }
    mtos_task_delay_ms(10);
  }
  priv->mn_mxl_hanlde.p_nim_cfg->nim_type = NIM_UNDEF;
  p_channel_info->param.dvbt.nim_type = NIM_UNDEF;
  DSMN_PRINT(3, "nim_mn_mxl_channel_connect over\n");
  return ERR_FAILURE;

}

static RET_CODE nim_mn_mxl_channel_set(lld_nim_t *p_lld,
                                          nim_channel_info_t *p_channel_info,
                                          nim_channel_set_info_t *p_channel_set_info)
{
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);
  DMD_PARAMETER_t  *p_param = (DMD_PARAMETER_t  *)priv->mn_mxl_hanlde.mn88472_priv;
  
  /*demod wake up for working*/
  
//  DSMN_PRINT(3," nim_mn_mxl_channel_set    cur_channelfrequency=%d\n",priv->cur_channel.frequency);
//  DSMN_PRINT(3," nim_mn_mxl_channel_set    set frequency=%d\n",p_channel_info->frequency);
//  DSMN_PRINT(3," nim_mn_mxl_channel_set 	  nim_type=%d\n",p_channel_info->param.dvbt.nim_type);
//  DSMN_PRINT(3," nim_mn_mxl_channel_set 	  band_width=%d\n",p_channel_info->param.dvbt.band_width);


  /*if the nim type is T/T2, need to play the current channel program*/
  p_channel_info->lock = 0;
  p_param->bw = nim_mn_mxl_select_bw(p_channel_info->param.dvbt.band_width);
  p_param->system = (p_channel_info->param.dvbt.nim_type == NIM_DVBT2) ? DMD_E_DVBT2 : DMD_E_DVBT;
  p_param->freq = p_channel_info->frequency*1000;
  p_param->ts_out = DMD_E_TSOUT_SERIAL_VARIABLE_CLOCK;

  memcpy(&priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));
  priv->mn_mxl_hanlde.p_nim_cfg->demod_band_width = p_channel_info->param.dvbt.band_width;
  DMD_set_system(p_param);
  DMD_scan(p_param);
  priv->mn_mxl_hanlde.p_nim_cfg->nim_type = p_channel_info->param.dvbt.nim_type;
  if(p_channel_info->param.dvbt.nim_type == NIM_DVBT)
  {
    p_channel_set_info->lock_time = 1900;
  }
  else
  {
    p_channel_set_info->lock_time = 2500;
  }

  return SUCCESS;
}

static RET_CODE nim_mn_mxl_channel_perf(lld_nim_t *p_lld,
                                           nim_channel_perf_t *p_perf)
{
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);
  u8 LockStatus = 0;
  u8 SNRx10 = 0;
  u16 CalcValue = 0;
  //u32 PER = 0;
  u32 FER = 0;
  DMD_PARAMETER_t  *p_param = (DMD_PARAMETER_t  *)priv->mn_mxl_hanlde.mn88472_priv;
 // DSMN_PRINT(3,"nim_mn_mxl_channel_perf\n");
  if(priv->mn_mxl_hanlde.p_nim_cfg->nim_type == NIM_UNDEF)
  {
    p_perf->agc=0;
    p_perf->snr=0;
    return SUCCESS;
  }
  
  LockStatus = (u8)nim_mn_mxl_get_lock_st(p_lld);;
 // DSMN_PRINT(3,"nim_mn_mxl_channel_perf %s\n",(LockStatus == 1) ? "LOCK" : "UNLOCK");
  if(LockStatus)
  {
    if(p_param->system == DMD_E_DVBT)
    {
  		DMD_get_info(p_param , DMD_E_INFO_DVBT_CNR_INT ); 
  		DMD_get_info(p_param, DMD_E_INFO_DVBT_PERRNUM);
  		DMD_get_info(p_param, DMD_E_INFO_DVBT_AGC);
      CalcValue = p_param->info[DMD_E_INFO_DVBT_CNR_INT];
      SNRx10 = p_param->info[DMD_E_INFO_DVBT_CNR_INT];
      FER = p_param->info[DMD_E_INFO_DVBT_PERRNUM];
    }
    else if(p_param->system == DMD_E_DVBT2)
    {
  		DMD_get_info(p_param , DMD_E_INFO_DVBT2_CNR_INT ); 
  	  DMD_get_info(p_param, DMD_E_INFO_DVBT2_PERRNUM);
  		DMD_get_info(p_param, DMD_E_INFO_DVBT2_AGC);
      CalcValue = p_param->info[DMD_E_INFO_DVBT2_CNR_INT];
      SNRx10 = p_param->info[DMD_E_INFO_DVBT2_CNR_INT];
      FER = p_param->info[DMD_E_INFO_DVBT2_PERRNUM];
    }
    else
    {
      DSMN_PRINT(3,"nim_mn_mxl_channel_perf error system types!\n");
    }
  }
  p_perf->lock = LockStatus;
  p_perf->agc = CalcValue;
  p_perf->snr = SNRx10;

  DSMN_PRINT(3, "nim_mn_mxl_channel_perf: %s %s, agc = %d, snr = %d, fer=%d\n",
    (priv->mn_mxl_hanlde.p_nim_cfg->nim_type == NIM_DVBT2) ? "DVB-T2" : "DVB-T",
    p_perf->lock ? "locked" : "unlocked",
    p_perf->agc,
    p_perf->snr,
    FER
  );
  return SUCCESS;
}

RET_CODE nim_mn_mxl_open(lld_nim_t *p_lld, void *cfg)
{
  drv_dev_t *p_dev = NULL;
  device_base_t *p_base = NULL;

  nim_mn_mxl_priv_t *priv = p_lld->p_priv = mtos_malloc(sizeof(nim_mn_mxl_priv_t));
  MT_ASSERT(NULL != priv);

  memset(priv, 0x00, sizeof(nim_mn_mxl_priv_t));

  priv->mn_mxl_hanlde.p_nim_cfg = (nim_config_t *)mtos_malloc(sizeof(nim_config_t));
  MT_ASSERT(NULL != priv->mn_mxl_hanlde.p_nim_cfg);

  memset(priv->mn_mxl_hanlde.p_nim_cfg, 0x00, sizeof(nim_config_t));
  
  priv->mn_mxl_hanlde.mn88472_priv = (DMD_PARAMETER_t *)mtos_malloc(sizeof(DMD_PARAMETER_t));
  MT_ASSERT(NULL != priv->mn_mxl_hanlde.mn88472_priv);
  memset(priv->mn_mxl_hanlde.mn88472_priv, 0x00, sizeof(DMD_PARAMETER_t));

  p_dev = DEV_FIND_BY_LLD(drv_dev_t, p_lld);
  p_base = p_dev->p_base;

 // priv->drv_base = p_base;
  nim_mn_mxl_print_level = 3;
  /* config driver */
  if(cfg)
  {
    memcpy(priv->mn_mxl_hanlde.p_nim_cfg, cfg, sizeof(nim_config_t));
    p_base->lock_mode = ((nim_config_t *)cfg)->lock_mode;
  }
  else
  {
    priv->mn_mxl_hanlde.p_nim_cfg->ts_mode = 0;//TS_SERIAL_MODE;
    p_base->lock_mode = OS_MUTEX_LOCK;
  }


  if(priv->mn_mxl_hanlde.p_nim_cfg->p_dem_bus == NULL)
  {
    priv->mn_mxl_hanlde.p_nim_cfg->p_dem_bus = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
  }
  if(priv->mn_mxl_hanlde.p_nim_cfg->p_tun_bus == NULL)
  {
    priv->mn_mxl_hanlde.p_nim_cfg->p_tun_bus = priv->mn_mxl_hanlde.p_nim_cfg->p_dem_bus;
  }
  /* create mutex for i2c accessing */
  mtos_sem_create(&g_mn_mxl_i2c_rw_mutex, TRUE);
  
  DMD_I2C_Init(priv->mn_mxl_hanlde.p_nim_cfg->p_dem_bus);
  //MxWare603_OEM_init(DEMOD_MN88472);

	mn_dvbt2_demd_init(priv->mn_mxl_hanlde.p_nim_cfg);

	DSMN_PRINT(2, "nim_mn_mxl_open done\n");

  return SUCCESS;
}

RET_CODE nim_mn_mxl_close(lld_nim_t *p_lld)
{
  nim_mn_mxl_priv_t *priv = (nim_mn_mxl_priv_t *)(p_lld->p_priv);
  if(priv)
  {
    if(priv->mn_mxl_hanlde.p_nim_cfg)
    {
      mtos_free(priv->mn_mxl_hanlde.p_nim_cfg);
      priv->mn_mxl_hanlde.p_nim_cfg = NULL;
    }
    if(priv->mn_mxl_hanlde.mn88472_priv)
    {
      mtos_free(priv->mn_mxl_hanlde.mn88472_priv);
      priv->mn_mxl_hanlde.mn88472_priv = NULL;
    }
    mtos_free(priv);
    p_lld->p_priv = NULL;
  }
  mtos_sem_destroy(&g_mn_mxl_i2c_rw_mutex, 0);
  return SUCCESS;
}

static void nim_mn_mxl_detach(lld_nim_t *p_lld)
{
}

RET_CODE nim_mn_mxl_attach(char *name)
{
  nim_device_t *dev;
  device_base_t *dev_base;
  lld_nim_t *lld_dev;

  /* allocate driver memory resource */
  dev = (nim_device_t *)dev_allocate(name, SYS_DEV_TYPE_NIM,
                                     sizeof(nim_device_t), sizeof(lld_nim_t));
  if(NULL == dev)
  {
    return ERR_FAILURE;
  }

  /* link base function */
  dev_base = (device_base_t *)dev->p_base;
  dev_base->open = (RET_CODE (*)(void *, void *))nim_mn_mxl_open;
  dev_base->close = (RET_CODE (*)(void *))nim_mn_mxl_close;
  dev_base->detach = (void (*)(void *))nim_mn_mxl_detach;
  dev_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))nim_mn_mxl_ioctrl;

  /* attach lld driver */
  lld_dev = (lld_nim_t *)dev->p_priv;
  lld_dev->channel_scan = NULL;
  lld_dev->channel_connect = nim_mn_mxl_channel_connect;
  lld_dev->channel_set = nim_mn_mxl_channel_set;
  lld_dev->channel_perf = nim_mn_mxl_channel_perf;
  lld_dev->diseqc_ctrl = nim_mn_mxl_diseqc_ctrl;
  lld_dev->blind_scan_start= NULL;
  lld_dev->blind_scan_cancel = NULL;
  return SUCCESS;
}

