/******************************************************************************/
/******************************************************************************/
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
#include "nim_atbm.h"

#include "MaxLinearDataTypes.h"
#include "atbm781x.h"
#include "atbmdiseqcapi.h"
#include "MxL603_OEM_Drv.h"
#include "AtbmCommon.h"

os_sem_t g_atbm_i2c_rw_mutex;


static u32 nim_atbm_print_level;
#define DSAT2_PRINT(lev, msg...) do {   \
  if ((lev) <= nim_atbm_print_level) {      \
    mtos_printk(msg);   \
  }           \
} while (0)

extern  MS_BOOL MDrv_Tuner_Init(void);
extern  void ATBM_INIT(void *p_dmd, void *p_tuner);
extern  MS_U32 MDrv_Tuner_SetTuner(MS_U32 dwFreq /*Khz*/, MS_U8 ucBw /*MHz*/);

static void atbm_dvbt2_demd_init(nim_config_t *p_nim_cfg)
{
	u8 chip = 0;
	//ATBMPoweOnInit();
	ATBMInit();
    //ATBMSetDVBTxMode(8);
	ATBMChipID(&chip);
	mtos_printk("atbm_dvbt2_demd_init chip_id =%x\n",chip);//chipid=0xa0
}

RET_CODE nim_atbm_diseqc_ctrl(lld_nim_t *p_lld,
                                   nim_diseqc_cmd_t *p_diseqc_cmd)
{
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);

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
    ATBMSetContinuousTone(0);
  }
  /* make sure the 22K OFF stable */
  mtos_task_delay_ms(20);

  if(NIM_DISEQC_BURST0 == p_diseqc_cmd->mode ||
      NIM_DISEQC_BURST1 == p_diseqc_cmd->mode)
  {
    ATBMSendToneBurst(NIM_DISEQC_BURST0 - 1);
  }
  else if(p_diseqc_cmd->mode == NIM_DISEQC_BYTES)
  {

    if(priv->diseqc_2x == 1)
    {
      if(p_diseqc_cmd->p_rx_buf == NULL)
      {
        return ERR_PARAM;
      }
      ATBMSendDiseqcWithReply(p_diseqc_cmd->p_tx_buf,p_diseqc_cmd->tx_len,
                           p_diseqc_cmd->p_rx_buf, &(p_diseqc_cmd->rx_len));
    }
    else
    {
      ATBMSendDiseqc(p_diseqc_cmd->p_tx_buf, p_diseqc_cmd->tx_len);
    }
  }
  /* make sure the diseqc commond received completely */
  mtos_task_delay_ms(20);

  if(priv->onoff_22k != 0)
  {
    /* 22k was colsed, open it */
    ATBMSetContinuousTone(1);
  }

  return 0;
}
static void nim_atbm_set_chninfo(lld_nim_t *p_lld, u32 plp_id)
{
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);

  /*Set PLP id for dvbt2*/
	if(priv->cur_channel.param.dvbt.nim_type == NIM_DVBT2)
  {

    DSAT2_PRINT(3,"frequency =%d\n",priv->cur_channel.frequency);
    DSAT2_PRINT(3,"ATBMSetPLPForMultiPLPDVBT2 plp_id =%d\n",priv->cur_channel.param.dvbt.plp_id);
    DSAT2_PRINT(3,"PLP_index :%d, PLP id: %d\n", priv->cur_channel.param.dvbt.PLP_index , priv->cur_channel.param.dvbt.plp_id);

    if(priv->cur_channel.param.dvbt.PLP_index > 0)
    {
      if(ATBMSetPLPForMultiPLPDVBT2(plp_id))
       {
    	 DSAT2_PRINT(1,"ATBMSetPLPForMultiPLPDVBT2 ERR_FAILURE\n");
       }
    }
    else
    {
    	ATBMSetAutoPLPT2();
    }
    DSAT2_PRINT(3,"ATBMSetPLPForMultiPLPDVBT2 SUCCESS\n");
  }
}
static u32 nim_atbm_get_lock_st(lld_nim_t *p_lld)
{
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);
  u8 LockStatus = 0;
  u8  DVBTxMode = 0;
  ATBMDVBTxChannelLockCheck(&LockStatus);

  if(LockStatus)
  {
    ATBMDVBTxModeDetected(&DVBTxMode);
    priv->atbm_hanlde.p_nim_cfg->nim_type = (DVBTxMode == ATBM_DVBT2_MODE) ? NIM_DVBT2 : NIM_DVBT;
  }
  return ((u32)LockStatus);
}

static RET_CODE nim_atbm_ioctrl(lld_nim_t *p_lld, u32 cmd, u32 param)
{
//  nim_diseqc_cmd_t diseqc_cmd;
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);
 // u8 LockStatus = 0;
  u8 LnbPinLevel = 0;
  u8 pin = 0;
  u8 level = 0;
  u8 value = 0;

  switch(cmd)
  {
    case DEV_IOCTRL_POWER:
      switch(param)
      {
        case DEV_POWER_SLEEP:
          /* sleep demod */
          ATBMSusPend();
          break;
        case DEV_POWER_FULLSPEED:
          /* wake up demod */
          ATBMPowerOnInit();
          break;
        case DEV_POWER_STANDBY:
          ATBMStandBy();
          break;
        default:
          return ERR_PARAM;
      }
      break;
    case NIM_IOCTRL_CHECK_LOCK:
      //ATBMDVBTxChannelLockCheck(&LockStatus);
      *((u8 *)param) = nim_atbm_get_lock_st(p_lld);;
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
      ATBMSwapIQ(priv->cur_channel.spectral_polar);
      break;

    case NIM_IOCTRL_SET_LNB_ONOFF:
      if(priv->lnb_onoff == param)
      {
        break;
      }
      ATBMSetLNBByGPIO(param);
      priv->lnb_onoff = (u8)param;
      break;
    case NIM_IOCTRL_SET_22K_ONOFF:
      ATBMSetContinuousTone(param);
      priv->onoff_22k = (u8)param;
      break;
    case NIM_IOCTRL_GET_22K_ONOFF:
      ATBMGetLNBStatusFromGPIO(&LnbPinLevel);
      *((u8 *)param) = LnbPinLevel;
      break;
    case NIM_IOCTRL_GET_TN_VERSION:
      break;
    case NIM_IOCTRL_SET_TS_MODE:
      break;

    case NIM_IOCTRL_CHECK_LNB_SC_PROT:
      if(priv->atbm_hanlde.p_nim_cfg->pin_config.lnb_prot_by_mcu == 1)
      {
        /* select gpio */
        pin = priv->atbm_hanlde.p_nim_cfg->pin_config.lnb_prot_pin;
        level = priv->atbm_hanlde.p_nim_cfg->pin_config.lnb_prot_level;
        hal_pinmux_gpio_enable(pin, TRUE);
        DSAT2_PRINT(3, "lnb_prot_pin[%d], level[%d]\n", pin, level);
        gpio_ioctl(GPIO_CMD_IO_ENABLE, pin, TRUE);
        gpio_get_dir(pin, &value);
        if(GPIO_DIR_OUTPUT == value)
        {
          DSAT2_PRINT(3, "set lnb_prot_pin input\n");
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
      nim_atbm_set_chninfo(p_lld, param);
      break;

    default:
      return ERR_PARAM;
  }

  return SUCCESS;
}

static RET_CODE nim_atbm_channel_set(lld_nim_t *p_lld,
                                          nim_channel_info_t *p_channel_info,
                                          nim_channel_set_info_t *p_channel_set_info)
{
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);
  stATBMDvbConfig stPara;
  u16 band_width_type = 0;


  /*demod wake up for working*/
  ATBMStandByWakeUp();


//  ATBMDVBTxChannelLockCheck(&LockStatus);
//  p_channel_info->lock = LockStatus;
  DSAT2_PRINT(3," nim_atbm_channel_set    cur_channelfrequency=%d\n",priv->cur_channel.frequency);
  DSAT2_PRINT(3," nim_atbm_channel_set    set frequency=%d\n",p_channel_info->frequency);
  DSAT2_PRINT(3," nim_atbm_channel_set 	  nim_type=%d\n",p_channel_info->param.dvbt.nim_type);
  DSAT2_PRINT(3," nim_atbm_channel_set 	  band_width=%d\n",p_channel_info->param.dvbt.band_width);


  /* set tuner freq*/
  ATBMHoldDSP();
  MDrv_Tuner_SetTuner(p_channel_info->frequency, p_channel_info->param.dvbt.band_width);
  ATBMStartDSP();

	DSAT2_PRINT(3,"nim_atbm_channel_set change freq\n");
  switch(p_channel_info->param.dvbt.band_width)
  {
  case 5:
    band_width_type = ATBM_DVBT_BANDWIDTH_5MHZ;
    break;
  case 6:
    band_width_type = ATBM_DVBT_BANDWIDTH_6MHZ;
    break;
  case 7:
    band_width_type = ATBM_DVBT_BANDWIDTH_7MHZ;
    break;
  case 8:
    band_width_type = ATBM_DVBT_BANDWIDTH_8MHZ;
    break;
  case 10:
    band_width_type = ATBM_DVBT_BANDWIDTH_10MHZ;
    break;
  default:
    break;
  }

  memcpy(&priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));

  #if 1  //after params is set, the following should not be called again.
  /*set demod freq and bandwidth*/
  stPara.ui8ConfigDVBType = p_channel_info->param.dvbt.nim_type;
  stPara.DvbTxTunerInput.ui8BandwidthType = band_width_type;
  stPara.DvbTxTunerInput.ui8BandwidthMHz = p_channel_info->param.dvbt.band_width;
  stPara.DvbTxTunerInput.u32IFFrequencyKHz = p_channel_info->frequency;
  stPara.ui32DemodClkKHz = 24000;
  ATBMSetFreqAndRateRatio(stPara);
  ATBMSetBandWidthPara(stPara);//?
  /*the bandwidth is not eque last bandwidth,so set ATBMSetDVBTxMode again*/
  ATBMSetDVBTxMode(p_channel_info->param.dvbt.band_width);
  #endif
  /*save the current bandwidth*/
  priv->atbm_hanlde.p_nim_cfg->demod_band_width = p_channel_info->param.dvbt.band_width;
  p_channel_set_info->lock_time = 1200;
  return SUCCESS;
}

static RET_CODE nim_atbm_channel_connect(lld_nim_t *p_lld,
                             nim_channel_info_t *p_channel_info, BOOL for_bs)
{
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);
  nim_channel_set_info_t channel_set_info;
//  stATBMDvbConfig stPara;
//  u16 band_width_type = 0;
  u8  LockStatus = 0;
  u8  DVBTxMode = 0;
	DVBT2_PARAMS	DvbT2Params;
  u32 cnt = 0, t2_cnt;
  p_channel_info->lock = 0;
  DSAT2_PRINT(3,"nim_atbm_channel_connect in\n");
#if 0  
	/*demod wake up for working*/
  ATBMStandByWakeUp();
  switch(p_channel_info->param.dvbt.band_width)
  {
  case 5:
    band_width_type = ATBM_DVBT_BANDWIDTH_5MHZ;
    break;
  case 6:
    band_width_type = ATBM_DVBT_BANDWIDTH_6MHZ;
    break;
  case 7:
    band_width_type = ATBM_DVBT_BANDWIDTH_7MHZ;
    break;
  case 8:
    band_width_type = ATBM_DVBT_BANDWIDTH_8MHZ;
    break;
  case 10:
    band_width_type = ATBM_DVBT_BANDWIDTH_10MHZ;
    break;
  default:
    break;
  }
#if 0 //add by Dren
	u8 chip = 0;
	//ATBMSetDvbtBandwidth(p_channel_info->param.dvbt.band_width);
	ATBMSetDVBTxMode(p_channel_info->param.dvbt.band_width);
	ATBMChipID(&chip);
#endif

  DSAT2_PRINT(3,"nim_atbm_channel_connect     IN LockStatus=%d\n", LockStatus);
  DSAT2_PRINT(3," nim_atbm_channel_connect    cur_channelfrequency=%d\n",priv->cur_channel.frequency);
  DSAT2_PRINT(3," nim_atbm_channel_connect     set frequency=%d\n",p_channel_info->frequency);
  DSAT2_PRINT(3," nim_atbm_channel_connect 	   nim_type=%d\n",p_channel_info->param.dvbt.nim_type);
  DSAT2_PRINT(3," nim_atbm_channel_connect 	   band_width=%d\n",p_channel_info->param.dvbt.band_width);
  /* Demod DSP reset, when the current freq is not == the last freq*/
	ATBMHoldDSP();
	MDrv_Tuner_SetTuner(p_channel_info->frequency, p_channel_info->param.dvbt.band_width);
	ATBMStartDSP();
  memcpy(&priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));

  stPara.ui8ConfigDVBType = ATBM_DVBT_MODE;
	/*the first time the IQ is not confirmed,but after the first time, here will get the right value and return for app,
	  so the app need set the right spectral_polar
	 */
  #if 1
  stPara.DvbTxTunerInput.ui8IQSwapMode = (priv->cur_channel.spectral_polar == 0) ? INPUT_IQ_NORMAL : INPUT_IQ_SWAP;
  stPara.DvbTxTunerInput.ui8BandwidthType = band_width_type;
  stPara.DvbTxTunerInput.u32IFFrequencyKHz = priv->cur_channel.frequency;
	stPara.ui32DemodClkKHz = 24000;
  ATBMSetFreqAndRateRatio(stPara);
  ATBMSetBandWidthPara(stPara);//?
  /*the bandwidth is not eque last bandwidth,so set ATBMSetDVBTxMode again*/
  ATBMSetDVBTxMode(p_channel_info->param.dvbt.band_width);
  #endif
  /*save the current bandwidth*/
  priv->atbm_hanlde.p_nim_cfg->demod_band_width = p_channel_info->param.dvbt.band_width;
#endif  
  nim_atbm_channel_set(p_lld, p_channel_info, &channel_set_info);
  /*check demod lock status*/
  for(cnt = 0; cnt < channel_set_info.lock_time; cnt+=20)
  {
    ATBMDVBTxSigalParamsLockCheck(&LockStatus);
    
    p_channel_info->lock = LockStatus;
    DSAT2_PRINT(3,"nim_atbm_channel_connect  OUT LockStatus=%d\n", LockStatus);
    if(LockStatus == 1)
    {
      ATBMDVBTxModeDetected(&DVBTxMode);
      DSAT2_PRINT(3,"nim_atbm_channel_connect  DVBTxMode=%d\n", DVBTxMode);
      if(DVBTxMode == ATBM_DVBT_MODE)
      {
        p_channel_info->param.dvbt.nim_type = NIM_DVBT;
        priv->atbm_hanlde.p_nim_cfg->nim_type = NIM_DVBT;
        return SUCCESS;
      }
      else if(DVBTxMode == ATBM_DVBT2_MODE)
      {
        u32 i =0;
        p_channel_info->param.dvbt.nim_type = NIM_DVBT2;
        priv->atbm_hanlde.p_nim_cfg->nim_type = NIM_DVBT2;

  	    ATBMSetDVBT2DisablePLPDecode();
        for(t2_cnt = 0; t2_cnt < channel_set_info.lock_time; t2_cnt+=20)
        {
          ATBMDVBTxSigalParamsLockCheck(&LockStatus);
          if(LockStatus == 1)
          {
            DvbT2Params.u8DataPlpNumber = 1;
            ATBMGetSignalParamsT2(&DvbT2Params);
            p_channel_info->param.dvbt.DataPlpNumber = DvbT2Params.u8DataPlpNumber;

            p_channel_info->param.dvbt.plp_id = DvbT2Params.u8ActivePlpId;
            p_channel_info->param.dvbt.PLP_index = 0;
            memset(p_channel_info->param.dvbt.DataPlpIdArray, 0, sizeof(p_channel_info->param.dvbt.DataPlpIdArray));
            memcpy(p_channel_info->param.dvbt.DataPlpIdArray, DvbT2Params.u8DataPlpIdArray, DvbT2Params.u8DataPlpNumber);
          }
          ATBMSleep(50);
        }
        ATBMSetDVBT2EnablePLPDecode();

        for(i = 0; i <DvbT2Params.u8DataPlpNumber; i++)
        {
            DSAT2_PRINT(3,"plp_c =%d DataPlpIdArray[%d]=%d\n",p_channel_info->param.dvbt.DataPlpNumber,i,p_channel_info->param.dvbt.DataPlpIdArray[i]);
            DSAT2_PRINT(3,"DvbT2Params.u8ActivePlpId=%d\n",p_channel_info->param.dvbt.plp_id);
        }
        return SUCCESS;
      }
    }
    ATBMSleep(50);
  }
  priv->atbm_hanlde.p_nim_cfg->nim_type = NIM_UNDEF;
  p_channel_info->param.dvbt.nim_type = NIM_UNDEF;

  return SUCCESS;
}

static RET_CODE nim_atbm_channel_perf(lld_nim_t *p_lld,
                                           nim_channel_perf_t *p_perf)
{
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);
  u8 LockStatus = 0;
  u8 SNRx10 = 0;
  u16 CalcValue = 0;
  u32 PER = 0;
  u32 FER = 0;
  //ATBMDVBTxChannelLockCheck(&LockStatus);
  ATBMDVBTxSigalParamsLockCheck(&LockStatus);
  if(!LockStatus)
  {
    p_perf->lock = 0;
    p_perf->agc = 0;
    p_perf->snr = 0;
    DSAT2_PRINT(3,"nim_atbm_channel_perf unlock\n");
    return SUCCESS;
  }
  p_perf->lock = LockStatus;
  
  ATBMDVBTxSignalIFAGC(&CalcValue);
  p_perf->agc = CalcValue;

  ATBMDVBTxSignalQuality(&SNRx10);
  p_perf->snr = SNRx10;

  ATBMGetDVBTPER(&PER);
  if(priv->atbm_hanlde.p_nim_cfg->nim_type == NIM_DVBT2)
  {
    ATBMGetDVBT2PostBCHFER(&FER);
  }
  DSAT2_PRINT(3, "nim_atbm_channel_perf: %s %s, agc = %d, snr = %d, per=%d fer=%d\n",
    (priv->atbm_hanlde.p_nim_cfg->nim_type == NIM_DVBT2) ? "DVB-T2" : "DVB-T",
    p_perf->lock ? "locked" : "unlocked",
    p_perf->agc,
    p_perf->snr,
    PER,
    FER
  );
  return SUCCESS;
}

RET_CODE nim_atbm_open(lld_nim_t *p_lld, void *cfg)
{
  drv_dev_t *p_dev = NULL;
  device_base_t *p_base = NULL;
//	uint8 chip = 0xff;
//	u32 i = 0;
  nim_atbm_priv_t *priv = p_lld->p_priv = mtos_malloc(sizeof(nim_atbm_priv_t));
  MT_ASSERT(NULL != priv);

  memset(priv, 0x00, sizeof(nim_atbm_priv_t));

  priv->atbm_hanlde.p_nim_cfg = (nim_config_t *)mtos_malloc(sizeof(nim_config_t));
  MT_ASSERT(NULL != priv->atbm_hanlde.p_nim_cfg);
  memset(priv->atbm_hanlde.p_nim_cfg, 0x00, sizeof(nim_config_t));

  p_dev = DEV_FIND_BY_LLD(drv_dev_t, p_lld);
  p_base = p_dev->p_base;

 // priv->drv_base = p_base;
  nim_atbm_print_level = 3;
  /* config driver */
  if(cfg)
  {
    memcpy(priv->atbm_hanlde.p_nim_cfg, cfg, sizeof(nim_config_t));
    p_base->lock_mode = ((nim_config_t *)cfg)->lock_mode;
  }
  else
  {
    priv->atbm_hanlde.p_nim_cfg->ts_mode = TS_SERIAL_MODE;
    p_base->lock_mode = OS_MUTEX_LOCK;
  }


  if(priv->atbm_hanlde.p_nim_cfg->p_dem_bus == NULL)
  {
    priv->atbm_hanlde.p_nim_cfg->p_dem_bus = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
  }
  if(priv->atbm_hanlde.p_nim_cfg->p_tun_bus == NULL)
  {
    priv->atbm_hanlde.p_nim_cfg->p_tun_bus = priv->atbm_hanlde.p_nim_cfg->p_dem_bus;
  }
  /* create mutex for i2c accessing */
  mtos_sem_create(&g_atbm_i2c_rw_mutex, TRUE);
  ATBM_INIT(priv->atbm_hanlde.p_nim_cfg->p_dem_bus, priv->atbm_hanlde.p_nim_cfg->p_tun_bus);

	MxWare603_OEM_init(DEMOD_ATBM);//add by fjh, 20131204. for atbm demod add.
	
	atbm_dvbt2_demd_init(priv->atbm_hanlde.p_nim_cfg);
  MDrv_Tuner_Init();
	DSAT2_PRINT(2, "nim_atbm_open done\n");
  return SUCCESS;
}

RET_CODE nim_atbm_close(lld_nim_t *p_lld)
{
  nim_atbm_priv_t *priv = (nim_atbm_priv_t *)(p_lld->p_priv);
  if(priv)
  {
    if(priv->atbm_hanlde.p_nim_cfg)
    {
      mtos_free(priv->atbm_hanlde.p_nim_cfg);
      priv->atbm_hanlde.p_nim_cfg = NULL;
    }
    mtos_free(priv);
    p_lld->p_priv = NULL;
  }
  mtos_sem_destroy(&g_atbm_i2c_rw_mutex, 0);
  return SUCCESS;
}

static void nim_atbm_detach(lld_nim_t *p_lld)
{
}

RET_CODE nim_atbm_attach(char *name)
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
  dev_base->open = (RET_CODE (*)(void *, void *))nim_atbm_open;
  dev_base->close = (RET_CODE (*)(void *))nim_atbm_close;
  dev_base->detach = (void (*)(void *))nim_atbm_detach;
  dev_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))nim_atbm_ioctrl;

  /* attach lld driver */
  lld_dev = (lld_nim_t *)dev->p_priv;
  lld_dev->channel_scan = NULL;
  lld_dev->channel_connect = nim_atbm_channel_connect;
  lld_dev->channel_set = nim_atbm_channel_set;
  lld_dev->channel_perf = nim_atbm_channel_perf;
  lld_dev->diseqc_ctrl = nim_atbm_diseqc_ctrl;
  lld_dev->blind_scan_start= NULL;
  lld_dev->blind_scan_cancel = NULL;
  return SUCCESS;
}

