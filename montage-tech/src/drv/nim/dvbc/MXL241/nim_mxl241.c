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
#include "mtos_printk.h"
#include "mtos_mem.h"

#include "drv_dev.h"
#include "../../../drvbase/drv_dev_priv.h"
#include "i2c.h"
#include "nim.h"
#include "../../nim_priv.h"

#include "MxL241SF_PhyCtrlApi.h"
#include "MxL241SF_OEM_Drv.h"
#include "nim_mxl241.h"

#define QAM_LOCK_TIME_OUT_MS 150
#define FEC_LOCK_TIME_OUT_MS 50

MXL_STATUS CheckQamLock(UINT8 I2cAddr)
{
  MXL_STATUS status;
  UINT32 startTime, endTime;
  MXL_DEMOD_LOCK_STATUS_T QamLockStatus;
  QamLockStatus.I2cSlaveAddr = I2cAddr;
  
  // GetSystemTimeMs returns ticks in millisecond
  startTime = GetSystemTimeMs();
  endTime = GetSystemTimeMs();

  // Checking QAM for 150ms
  while ((endTime - startTime) < QAM_LOCK_TIME_OUT_MS)
  {
    status = MxLWare_API_GetDemodStatus(MXL_DEMOD_QAM_LOCK_REQ, (void*)&QamLockStatus);

    if ((MXL_TRUE == status) && (MXL_LOCKED == QamLockStatus.Status)) return MXL_TRUE;
    endTime = GetSystemTimeMs();
  }

  return MXL_FALSE;
}

MXL_STATUS CheckFecLock(UINT8 I2cAddr)
{
  MXL_STATUS status;
  UINT32 startTime, endTime;
  MXL_DEMOD_LOCK_STATUS_T FecLockStatus;
  FecLockStatus.I2cSlaveAddr = I2cAddr;
  
  // GetSystemTimeMs returns ticks in millisecond
  startTime = GetSystemTimeMs();
  endTime = GetSystemTimeMs();

  // Checking QAM for 150ms
  while ((endTime - startTime) < FEC_LOCK_TIME_OUT_MS)
  {
    status = MxLWare_API_GetDemodStatus(MXL_DEMOD_FEC_LOCK_REQ, (void*)&FecLockStatus);

    if ((MXL_TRUE == status) && (MXL_LOCKED == FecLockStatus.Status)) return MXL_TRUE;
    endTime = GetSystemTimeMs();
  }

  return MXL_FALSE;
}

u32 parse_qam(nim_modulation_t qam_id) 
{
  u32 n_qam = QAM64;

  switch (qam_id) 
  {
    case NIM_MODULA_QAM16:
      n_qam = QAM16;
      break;
    case NIM_MODULA_QAM32:
      n_qam = QAM32;
      break;
    case NIM_MODULA_QAM64:
      n_qam = QAM64;
      break;
    case NIM_MODULA_QAM128:
      n_qam = QAM128;
      break;
    case NIM_MODULA_QAM256:
      n_qam = QAM256;
      break;
    default:
      break;
  }
  
  return n_qam;
}


static signed long nim_mxl241_channel_connect(struct lld_nim *p_dev,
    nim_channel_info_t *p_channel_info, BOOL for_scan) {
  u32 freq = 0;
  REAL32 symb = 0;
  u32 qam = 0;
  
  
  UINT32 loop;
#ifdef __MXL_INTEGER_CALC_STATISTICS__
    SINT64 tmpRealNum1;
    SINT64 tmpRealNum2;
#else
    //REAL32 tmpRealNum1;
    //REAL32 tmpRealNum2;
#endif
  
    MXL_STATUS status;
    //MXL_RESET_CFG_T               MxL241SF_Reset;
    //MXL_OVERWRITE_DEFAULT_CFG_T   MxL241SF_OverwriteDefault;
    //MXL_DEV_INFO_T                MxL241SF_DeviceInfo;
    //MXL_XTAL_CFG_T                MxL241SF_XtalSetting;
    MXL_RF_TUNE_CFG_T             MxL241SF_ChanTune;
    MXL_DEMOD_LOCK_STATUS_T       MxL241SF_LockStatus;
    MXL_SYMBOL_RATE_T             MxL241SF_SymbolRate;
    //MXL_AGC_T                     MxL241SF_Agc;
  //  MXL_TOP_MASTER_CFG_T          MxL241SF_PowerUpTuner;
    MXL_ANNEX_CFG_T               MxL241SF_AnnexType;
    //MXL_PWR_MODE_CFG_T            MxL241SF_PwrMode;
    //MXL_MPEG_OUT_CFG_T            MxL241SF_MpegOut;
    MXL_ADCIQFLIP_CFG_T           MxL241SF_AdcIpFlip;
    //MXL_INTERDEPTH_INFO_T         MxL241SF_InterleaverInfo;
    //MXL_DEMOD_EQUALFILTER_INFO_T  MxL241SF_EqualizerInfo;
    //MXL_DEMOD_TIMINGOFFSET_INFO_T MxL241SF_TimingOffsetInfo;
   // MXL_TUNER_RX_PWR_T            MxL241SF_RfPowerReadback;
    MXL_CHAN_DEPENDENT_T          MxL241SF_ChanDependent;
    MXL_INVERT_CARR_OFFSET_T      MxL241SF_InvertCarrOffset;

    nim_mxl241_priv_t *p_priv = (nim_mxl241_priv_t *) (p_dev->p_priv);
    if (NULL == p_dev || NULL == p_channel_info) {
        return ERR_FAILURE;
    }
  
  memcpy(&p_priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));
  freq = p_channel_info->frequency;
  symb = (REAL32)p_channel_info->param.dvbc.symbol_rate;
  qam = parse_qam(p_channel_info->param.dvbc.modulation);

  // 8. Symbol rate
  MxL241SF_SymbolRate.I2cSlaveAddr = 99;
  if(p_priv->j83b_j83a == 0)
        MxL241SF_SymbolRate.SymbolType = SYM_TYPE_USER_DEFINED_J83B;
  else
        MxL241SF_SymbolRate.SymbolType = SYM_TYPE_USER_DEFINED_J83A;
  //MxL241SF_SymbolRate.SymbolRate = (REAL32)6.900;
  MxL241SF_SymbolRate.SymbolRate = (REAL32) symb / 1000.0;
  MxL241SF_SymbolRate.SymbolRate256 = MxL241SF_SymbolRate.SymbolRate;

  MxLWare_API_ConfigDemod(MXL_DEMOD_SYMBOL_RATE_CFG, (void*)&MxL241SF_SymbolRate);

  // 9. Config Annex Type
  MxL241SF_AnnexType.I2cSlaveAddr = 99;
  if(p_priv->j83b_j83a == 0)
    MxL241SF_AnnexType.AnnexType = ANNEX_B;
  else
    MxL241SF_AnnexType.AnnexType = ANNEX_A;

  MxL241SF_AnnexType.AutoDetectQamType = MXL_DISABLE;
  MxL241SF_AnnexType.AutoDetectMode = MXL_ENABLE;
  MxL241SF_AnnexType.QamType = qam;
  MxLWare_API_ConfigDemod(MXL_DEMOD_ANNEX_QAM_TYPE_CFG, (void*)&MxL241SF_AnnexType);

  // 10. Do MiscSettings if needed

  // 11. Tune RF with channel frequency and bandwidth
  MxL241SF_ChanTune.I2cSlaveAddr = 99;
  MxL241SF_ChanTune.BandWidth = p_priv->bandwidth;                     // 8MHz
//  MxL241SF_ChanTune.Frequency = 299000000;             // 666MHz
  MxL241SF_ChanTune.Frequency = freq * 1000;             // 666MHz

  MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_TUNE_CFG, (void*)&MxL241SF_ChanTune );

  /* Please do not forget to put time delay at least 25ms */
  // Wait 25ms
  // Wait(25ms);

  // Following step is for Annex-A
  // Set MXL_DISABLE for Annex-B and OOB as a default
  // 12. Configure spectrum-inversion
  MxL241SF_AdcIpFlip.I2cSlaveAddr = 99;

  // Enable I/Q path flip
  MxL241SF_AdcIpFlip.AdcIqFlip = MXL_ENABLE;
  MxLWare_API_ConfigDemod(MXL_DEMOD_ADC_IQ_FLIP_CFG, (void*)&MxL241SF_AdcIpFlip);

  // 13. Channel dependent setting
  MxL241SF_ChanDependent.I2cSlaveAddr = 99;
  MxL241SF_ChanDependent.ChanDependentCfg = MXL_DISABLE;
  MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG, (void*)&MxL241SF_ChanDependent);

  // Auto spectrum inversion
  if (MXL_TRUE == CheckQamLock(99) && MXL_FALSE == CheckFecLock(99))
  {
    // Invert carrier offset
    MxL241SF_InvertCarrOffset.I2cSlaveAddr = 99;
    MxLWare_API_ConfigDemod(MXL_DEMOD_INVERT_CARRIER_OFFSET_CFG, (void*)&MxL241SF_InvertCarrOffset);

    // If enabled, disale I/Q path flip or
    // If disable, enable I/Q path flip
    MxL241SF_AdcIpFlip.AdcIqFlip = (MxL241SF_AdcIpFlip.AdcIqFlip == MXL_ENABLE) ? MXL_DISABLE : MXL_ENABLE;
    MxLWare_API_ConfigDemod(MXL_DEMOD_ADC_IQ_FLIP_CFG, (void*)&MxL241SF_AdcIpFlip);
  }
    loop = 0; 
    while (loop < 10) //  This number is experimental to monitor statistics.
    {
        MxL241SF_LockStatus.I2cSlaveAddr = 99;

        status = MxLWare_API_GetDemodStatus(MXL_DEMOD_FEC_LOCK_REQ, (void*)&MxL241SF_LockStatus);
        if (status == MXL_TRUE)
        {
            if(MxL241SF_LockStatus. Status)
                break;
        }
    //     Wait(1000); // This delay is also experimental, it can be anything.
        mtos_task_sleep(100);

        loop++;
    }
    p_channel_info->lock = MxL241SF_LockStatus. Status;
  return SUCCESS;
}

static signed long nim_mxl241_channel_set(struct lld_nim *p_dev,
    nim_channel_info_t *p_channel_info,
    nim_channel_set_info_t *p_channel_set_info) {
      u32 freq = 0;
      REAL32 symb = 0;
      u32 qam = 0;
      
      //UINT32 loop;
#ifdef __MXL_INTEGER_CALC_STATISTICS__
        SINT64 tmpRealNum1;
        SINT64 tmpRealNum2;
#else
        //REAL32 tmpRealNum1;
        //REAL32 tmpRealNum2;
#endif
      
        //MXL_STATUS status;
        //MXL_RESET_CFG_T               MxL241SF_Reset;
        //MXL_OVERWRITE_DEFAULT_CFG_T   MxL241SF_OverwriteDefault;
        //MXL_DEV_INFO_T                MxL241SF_DeviceInfo;
        //MXL_XTAL_CFG_T                MxL241SF_XtalSetting;
        MXL_RF_TUNE_CFG_T             MxL241SF_ChanTune;
        //MXL_DEMOD_LOCK_STATUS_T       MxL241SF_LockStatus;
        MXL_SYMBOL_RATE_T             MxL241SF_SymbolRate;
        //MXL_AGC_T                     MxL241SF_Agc;
        //MXL_TOP_MASTER_CFG_T          MxL241SF_PowerUpTuner;
        MXL_ANNEX_CFG_T               MxL241SF_AnnexType;
        //MXL_PWR_MODE_CFG_T            MxL241SF_PwrMode;
        //MXL_MPEG_OUT_CFG_T            MxL241SF_MpegOut;
        MXL_ADCIQFLIP_CFG_T           MxL241SF_AdcIpFlip;
        //MXL_INTERDEPTH_INFO_T         MxL241SF_InterleaverInfo;
        //MXL_DEMOD_EQUALFILTER_INFO_T  MxL241SF_EqualizerInfo;
       // MXL_DEMOD_TIMINGOFFSET_INFO_T MxL241SF_TimingOffsetInfo;
       // MXL_TUNER_RX_PWR_T            MxL241SF_RfPowerReadback;
        MXL_CHAN_DEPENDENT_T          MxL241SF_ChanDependent;
        MXL_INVERT_CARR_OFFSET_T      MxL241SF_InvertCarrOffset;
    
        nim_mxl241_priv_t *p_priv = (nim_mxl241_priv_t *) (p_dev->p_priv);
        if (NULL == p_dev || NULL == p_channel_info) {
            return ERR_FAILURE;
        }
      
      memcpy(&p_priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));
      freq = p_channel_info->frequency;
      symb = (REAL32)p_channel_info->param.dvbc.symbol_rate;
      qam = parse_qam(p_channel_info->param.dvbc.modulation);
      // 8. Symbol rate
      MxL241SF_SymbolRate.I2cSlaveAddr = 99;
      if(p_priv->j83b_j83a == 0)
            MxL241SF_SymbolRate.SymbolType = SYM_TYPE_USER_DEFINED_J83B;
      else
            MxL241SF_SymbolRate.SymbolType = SYM_TYPE_USER_DEFINED_J83A;
      //MxL241SF_SymbolRate.SymbolRate = (REAL32)6.900;
      MxL241SF_SymbolRate.SymbolRate = (REAL32) symb / 1000.0;
      MxL241SF_SymbolRate.SymbolRate256 = MxL241SF_SymbolRate.SymbolRate;
      MxLWare_API_ConfigDemod(MXL_DEMOD_SYMBOL_RATE_CFG, (void*)&MxL241SF_SymbolRate);
    
      // 9. Config Annex Type
      MxL241SF_AnnexType.I2cSlaveAddr = 99;
      if(p_priv->j83b_j83a == 0)
        MxL241SF_AnnexType.AnnexType = ANNEX_B;
      else
        MxL241SF_AnnexType.AnnexType = ANNEX_A;
    
    MxL241SF_AnnexType.AutoDetectQamType = MXL_DISABLE;
    MxL241SF_AnnexType.AutoDetectMode = MXL_ENABLE;
    MxL241SF_AnnexType.QamType = qam;
    
      MxLWare_API_ConfigDemod(MXL_DEMOD_ANNEX_QAM_TYPE_CFG, (void*)&MxL241SF_AnnexType);
    
      // 10. Do MiscSettings if needed
    
      // 11. Tune RF with channel frequency and bandwidth
      MxL241SF_ChanTune.I2cSlaveAddr = 99;
      MxL241SF_ChanTune.BandWidth = p_priv->bandwidth;                     // 8MHz
    //  MxL241SF_ChanTune.Frequency = 299000000;             // 666MHz
      MxL241SF_ChanTune.Frequency = freq * 1000;             // 666MHz
    
      MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_TUNE_CFG, (void*)&MxL241SF_ChanTune );
    
      /* Please do not forget to put time delay at least 25ms */
      // Wait 25ms
      // Wait(25ms);
    
      // Following step is for Annex-A
      // Set MXL_DISABLE for Annex-B and OOB as a default
      // 12. Configure spectrum-inversion
      MxL241SF_AdcIpFlip.I2cSlaveAddr = 99;
    
      // Enable I/Q path flip
      MxL241SF_AdcIpFlip.AdcIqFlip = MXL_ENABLE;
      MxLWare_API_ConfigDemod(MXL_DEMOD_ADC_IQ_FLIP_CFG, (void*)&MxL241SF_AdcIpFlip);
    
      // 13. Channel dependent setting
      MxL241SF_ChanDependent.I2cSlaveAddr = 99;
      MxL241SF_ChanDependent.ChanDependentCfg = MXL_DISABLE;
      MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG, (void*)&MxL241SF_ChanDependent);
    
      // Auto spectrum inversion
      if (MXL_TRUE == CheckQamLock(99) && MXL_FALSE == CheckFecLock(99))
      {
        // Invert carrier offset
        MxL241SF_InvertCarrOffset.I2cSlaveAddr = 99;
        MxLWare_API_ConfigDemod(MXL_DEMOD_INVERT_CARRIER_OFFSET_CFG, (void*)&MxL241SF_InvertCarrOffset);
    
        // If enabled, disale I/Q path flip or
        // If disable, enable I/Q path flip
        MxL241SF_AdcIpFlip.AdcIqFlip = (MxL241SF_AdcIpFlip.AdcIqFlip == MXL_ENABLE) ? MXL_DISABLE : MXL_ENABLE;
        MxLWare_API_ConfigDemod(MXL_DEMOD_ADC_IQ_FLIP_CFG, (void*)&MxL241SF_AdcIpFlip);
      }
      p_channel_set_info->lock_time = 1000;
  return SUCCESS;
}

static signed long nim_mxl241_channel_perf(struct lld_nim *p_dev,
    nim_channel_perf_t *p_channel_perf) 
{
  //u16 n_qam = 0;
  MXL_STATUS status = 0;
  MXL_TUNER_RX_PWR_T rxpwr = {0,};
  MXL_DEMOD_SNR_INFO_T snr = {0,};
  MXL_DEMOD_BER_INFO_T ber= {0,};
 // nim_mxl241_priv_t *p_priv = (nim_mxl241_priv_t *) (p_dev->p_priv);

  if (NULL == p_dev || NULL == p_channel_perf) {
    return ERR_FAILURE;
  }

  status = MxLWare_API_GetTunerStatus(MXL_TUNER_RF_RX_PWR_REQ, (void *)&rxpwr);
  if(status == MXL_TRUE)
    p_channel_perf->agc = rxpwr.RxPwr - 107;
  else
    p_channel_perf->agc = 0;

  status = MxLWare_API_GetDemodStatus(MXL_DEMOD_SNR_REQ, (void *)&snr);
  if(status == MXL_TRUE)
    p_channel_perf->snr = snr.SNR;
  else
    p_channel_perf->snr = 0; 

  status = MxLWare_API_GetDemodStatus(MXL_DEMOD_BER_UNCODED_BER_CER_REQ, (void *)&ber);
  if(status == MXL_TRUE)
    p_channel_perf->ber= ber.BER;
  else
    p_channel_perf->snr = 0.0;   
  return SUCCESS;
}

/* nim modules ways */
static signed long _mxl241_ioctrl(lld_nim_t *p_dev, u32 cmd, u32 param) 
{
    nim_mxl241_priv_t *p_priv = (nim_mxl241_priv_t *) (p_dev->p_priv);
    MXL_STATUS status;
    MXL_DEMOD_LOCK_STATUS_T       MxL241SF_LockStatus;
    switch (cmd) {
        case NIM_IOCTRL_CHECK_LOCK:
            MxL241SF_LockStatus.I2cSlaveAddr = 99;

            status = MxLWare_API_GetDemodStatus(MXL_DEMOD_FEC_LOCK_REQ, (void*)&MxL241SF_LockStatus);
            if (status == MXL_TRUE)
                *((u8 *) param) = MxL241SF_LockStatus.Status;
            else
                *((u8 *) param) = 0;
            break;
        case NIM_IOCTRL_SWITCH_J83B_J83A:
            p_priv->j83b_j83a = param;
            break;
        case NIM_IOCTRL_SET_TUNER_SLEEP:
            break;
        case NIM_IOCTRL_SET_TUNER_WAKEUP:
            break;
        case NIM_IOCTRL_GET_CHANNEL_INFO:
            memcpy((nim_channel_info_t *)param,&p_priv->cur_channel, sizeof(nim_channel_info_t));
            break;
        case NIM_IOCTRL_SET_TUNER_BANDWIDTH:
            p_priv->bandwidth = param;
            break;
        default:
            break;
  }

  return SUCCESS;
}

static RET_CODE _mxl241_close(lld_nim_t *p_lld) {
  nim_mxl241_priv_t *p_priv = (nim_mxl241_priv_t *) (p_lld->p_priv);
  mtos_free(p_priv);

  return SUCCESS;
}

static void nim_mxl241_detach(lld_nim_t *p_lld) {
}

void *p_mxl241_i2c = NULL;
static RET_CODE _mxl241_open(lld_nim_t *p_lld, void *p_cfg) 
{
  nim_config_t *p_nim_cfg = (nim_config_t *) p_cfg;
  nim_mxl241_priv_t *p_priv = NULL;
  //u32 ts_mode = NIM_TS_INTF_DEF;
  drv_dev_t *p_dev = NULL;
  device_base_t *p_base = NULL;

  //UINT32 loop;
  
#ifdef __MXL_INTEGER_CALC_STATISTICS__
    SINT64 tmpRealNum1;
    SINT64 tmpRealNum2;
#else
    //REAL32 tmpRealNum1;
    //REAL32 tmpRealNum2;
#endif
  
    MXL_STATUS status;
    MXL_RESET_CFG_T               MxL241SF_Reset;
    MXL_OVERWRITE_DEFAULT_CFG_T   MxL241SF_OverwriteDefault;
    MXL_DEV_INFO_T                MxL241SF_DeviceInfo;
    MXL_XTAL_CFG_T                MxL241SF_XtalSetting;
   // MXL_RF_TUNE_CFG_T             MxL241SF_ChanTune;
    //MXL_DEMOD_LOCK_STATUS_T       MxL241SF_LockStatus;
    //MXL_SYMBOL_RATE_T             MxL241SF_SymbolRate;
    MXL_AGC_T                     MxL241SF_Agc;
    MXL_TOP_MASTER_CFG_T          MxL241SF_PowerUpTuner;
    //MXL_ANNEX_CFG_T               MxL241SF_AnnexType;
 //MXL_PWR_MODE_CFG_T            MxL241SF_PwrMode;
    MXL_MPEG_OUT_CFG_T            MxL241SF_MpegOut;
   // MXL_ADCIQFLIP_CFG_T           MxL241SF_AdcIpFlip;
   // MXL_INTERDEPTH_INFO_T         MxL241SF_InterleaverInfo;
   // MXL_DEMOD_EQUALFILTER_INFO_T  MxL241SF_EqualizerInfo;
   // MXL_DEMOD_TIMINGOFFSET_INFO_T MxL241SF_TimingOffsetInfo;
   // MXL_TUNER_RX_PWR_T            MxL241SF_RfPowerReadback;
   // MXL_CHAN_DEPENDENT_T          MxL241SF_ChanDependent;
   // MXL_INVERT_CARR_OFFSET_T      MxL241SF_InvertCarrOffset;

  p_dev = DEV_FIND_BY_LLD(drv_dev_t, p_lld);
  p_base = p_dev->p_base;


  p_lld->p_priv = mtos_malloc(sizeof(nim_mxl241_priv_t));
  memset(p_lld->p_priv, 0x00, sizeof(nim_mxl241_priv_t));
  p_priv = p_lld->p_priv;
  
  if (NULL != p_nim_cfg) 
  {
    p_base->lock_mode = p_nim_cfg->lock_mode;
    p_mxl241_i2c = p_nim_cfg->p_dem_bus;
    if(p_nim_cfg->tuner_bandwidth == 0)
        p_priv->bandwidth = 6;
    else
        p_priv->bandwidth = p_nim_cfg->tuner_bandwidth;
  } 
  else 
  {
      MT_ASSERT(0);
  }

  // 1. Do SW Reset
  MxL241SF_Reset.I2cSlaveAddr = 99;
  status = MxLWare_API_ConfigDevice(MXL_DEV_SOFT_RESET_CFG, (void*)&MxL241SF_Reset);

  // 2. Overwrite default
  MxL241SF_OverwriteDefault.I2cSlaveAddr = 99;
  status = MxLWare_API_ConfigDevice(MXL_DEV_OVERWRITE_DEFAULT_CFG, (void*)&MxL241SF_OverwriteDefault);

  // 3. Read Back Device id and version
  status = MxLWare_API_GetDeviceStatus(MXL_DEV_ID_VERSION_REQ, (void*)&MxL241SF_DeviceInfo);
  if (status == MXL_TRUE) 
  {
    mtos_printk("MxL241SF : DevId = 0x%x, Version = 0x%x\n", MxL241SF_DeviceInfo.DevId, MxL241SF_DeviceInfo.DevVer);
  }

  // 4. XTAL and Clock out setting
  MxL241SF_XtalSetting.I2cSlaveAddr = 99;
  MxL241SF_XtalSetting.XtalEnable = MXL_ENABLE;
  MxL241SF_XtalSetting.DigXtalFreq = XTAL_48MHz;
  MxL241SF_XtalSetting.XtalBiasCurrent = 1;
  MxL241SF_XtalSetting.XtalCap = 10; // 10pF
  MxL241SF_XtalSetting.XtalClkOutEnable = MXL_ENABLE;
  MxL241SF_XtalSetting.XtalClkOutGain =  0xa; 
  MxL241SF_XtalSetting.LoopThruEnable = MXL_DISABLE;

  MxLWare_API_ConfigDevice(MXL_DEV_XTAL_SETTINGS_CFG, (void*)&MxL241SF_XtalSetting);

  // 5. AGC configuration
  MxL241SF_Agc.I2cSlaveAddr = 99;
  MxL241SF_Agc.FreezeAgcGainWord = MXL_NO_FREEZE;

  MxLWare_API_ConfigTuner(MXL_TUNER_AGC_SETTINGS_CFG, (void*)&MxL241SF_Agc);

  // 6. Power Up Tuner
  MxL241SF_PowerUpTuner.I2cSlaveAddr = 99;
  MxL241SF_PowerUpTuner.TopMasterEnable = MXL_ENABLE;

  MxLWare_API_ConfigTuner(MXL_TUNER_TOP_MASTER_CFG, (void*)&MxL241SF_PowerUpTuner);

  //  Wait(1ms);
  
  // 7. MPEG out setting
  MxL241SF_MpegOut.I2cSlaveAddr = 99;
  MxL241SF_MpegOut.SerialOrPar = MPEG_DATA_SERIAL;
  MxL241SF_MpegOut.LsbOrMsbFirst = MPEG_SERIAL_MSB_1ST;
  MxL241SF_MpegOut.MpegSyncPulseWidth = MPEG_SYNC_WIDTH_BIT;
  MxL241SF_MpegOut.MpegValidPol = MPEG_ACTIVE_HIGH;
  MxL241SF_MpegOut.MpegClkPol = MPEG_CLK_POSITIVE;
  MxL241SF_MpegOut.MpegSyncPol = MPEG_ACTIVE_HIGH;
  MxL241SF_MpegOut.MpegClkFreq = MPEG_CLK_57MHz;
  MxL241SF_MpegOut.MpegClkSource = MPEG_CLK_INTERNAL;

  MxLWare_API_ConfigDemod(MXL_DEMOD_MPEG_OUT_CFG, (void*)&MxL241SF_MpegOut);
  return SUCCESS;
}

static signed long nim_mxl241_open(void *p_lld, void *p_cfg) 
{
    return _mxl241_open((lld_nim_t *)p_lld, p_cfg);
}
static signed long nim_mxl241_close(void *p_lld)
{
    return _mxl241_close((lld_nim_t *)p_lld);
}

static signed long nim_mxl241_ioctrl(void *p_lld, unsigned long cmd, unsigned long param)
{
    return _mxl241_ioctrl((lld_nim_t *)p_lld, (u32)cmd, (u32)param);
}


/* Register m88rs2000 tuner deivce */
RET_CODE nim_mxl241_attach(char *name) {
  nim_device_t *dev;
  device_base_t *dev_base;
  lld_nim_t *lld_dev;

  /* allocate driver memory resource */
  dev = (nim_device_t *) dev_allocate(name, SYS_DEV_TYPE_NIM,
      sizeof(nim_device_t), sizeof(lld_nim_t));
  if (NULL == dev) {
    return ERR_FAILURE;
  }

  /* link base function */
  dev_base = (device_base_t *)(dev->p_base);
  //dev_base->open = (RET_CODE (*)(void *, void *))nim_m88dc2800_open;
  dev_base->open = nim_mxl241_open;
  //dev_base->close = (RET_CODE (*)(void *))nim_m88dc2800_close;
  dev_base->close = nim_mxl241_close;
  dev_base->detach = (void (*)(void *))nim_mxl241_detach;
  //dev_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))nim_m88dc2800_ioctrl;
  dev_base->io_ctrl = nim_mxl241_ioctrl;

  /* attach lld driver */
  lld_dev = (lld_nim_t *)dev->p_priv;
  lld_dev->channel_scan = NULL;
  lld_dev->channel_connect =nim_mxl241_channel_connect;
  lld_dev->channel_set = nim_mxl241_channel_set;
  lld_dev->channel_perf = nim_mxl241_channel_perf;
  lld_dev->diseqc_ctrl = NULL;
  
  return SUCCESS;
}

