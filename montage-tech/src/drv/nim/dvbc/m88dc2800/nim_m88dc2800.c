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
#include "mem_manager.h"
#include "mtos_mem.h"

#include "drv_dev.h"
#include "../../../drvbase/drv_dev_priv.h"
#include "i2c.h"
#include "nim.h"
#include "../../nim_priv.h"

#include "mt_fe_common.h"
#include "mt_fe_def.h"
#include "nim_m88dc2800.h"
#include "mt_fe_tn_tc2800.h"


//i2c_bus_t *g_dc2800_i2c = NULL;
os_sem_t reg_rw_mutex = 0;

//nim_m88dc280_priv_t *g_p_dc2800_priv = NULL;
MT_FE_DC2800_SUPPORTED_TUNER dc2800_tuner_type = MT_FE_TN_NOTSUPPORT;
static MT_FE_DC2800_Device_Handle g_p_dc2800_device_handle = NULL;

#define dmd_rd_reg(reg) dc2800_dmd_reg_read(&p_priv->handle, reg)
#define dmd_wr_reg(reg, val) dc2800_dmd_reg_write(&p_priv->handle, reg, val)

u16 parse_qam(nim_modulation_t qam_id) 
{
  u16 n_qam = 64;

  switch (qam_id) 
  {
    case NIM_MODULA_QAM16:
      n_qam = 16;
      break;
    case NIM_MODULA_QAM32:
      n_qam = 32;
      break;
    case NIM_MODULA_QAM64:
      n_qam = 64;
      break;
    case NIM_MODULA_QAM128:
      n_qam = 128;
      break;
    case NIM_MODULA_QAM256:
      n_qam = 256;
      break;
    default:
      break;
  }
  
  return n_qam;
}

/* i2c porting layer */
void dc2800_dmd_reg_write_unlock(MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data)
{
    u8    tmp[2];
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
    tmp[0] = reg;
    tmp[1] = data;
    i2c_write(p_priv->p_i2c, p_priv->dmd_addr, tmp, 2, I2C_PARAM_DEV_SOC_EXTER);
}

u8 dc2800_dmd_reg_read_unlock(MT_FE_DC2800_Device_Handle p_handle, u8 reg)
{
    u8    data = reg;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
    i2c_std_read(p_priv->p_i2c,  p_priv->dmd_addr, &data, 1, 1, I2C_PARAM_DEV_SOC_EXTER);
    return data;
}

void dc2800_dmd_reg_write(MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data)
{
    u8 tmp[2];
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
    tmp[0] = reg;
    tmp[1] = data;
  
    mtos_sem_take(&reg_rw_mutex, 0);
    i2c_write(p_priv->p_i2c, p_priv->dmd_addr, tmp, 2, I2C_PARAM_DEV_SOC_EXTER);
    mtos_sem_give(&reg_rw_mutex);
}

u8 dc2800_dmd_reg_read(MT_FE_DC2800_Device_Handle p_handle, u8 reg)
{
    u8 data = reg;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
    mtos_sem_take(&reg_rw_mutex, 0);
    i2c_std_read(p_priv->p_i2c, p_priv->dmd_addr, &data, 1, 1, I2C_PARAM_DEV_SOC_EXTER);
    mtos_sem_give(&reg_rw_mutex);
  
    return data;
}


void dc2800_tn_reg_write(MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data)
{
  u8 tmp[2];
  u8 value;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
  tmp[0] = reg;
  tmp[1] = data;

  mtos_sem_take(&reg_rw_mutex, 0);

  if(p_priv->chip_mode != 2)
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x87);
    value &= 0x0F;
    value |= 0x90;    // bit7         = 1, Enable I2C repeater
                // bit[6:4]     = 1, Enable I2C repeater for 1 time
    dc2800_dmd_reg_write_unlock(p_handle, 0x87, value);
  }
  else
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x86);
    value |= 0x80;
    dc2800_dmd_reg_write_unlock(p_handle, 0x86, value);
  }

  i2c_write(p_priv->p_i2c, p_handle->tuner_settings.tuner_dev_addr,
                tmp, 2, I2C_PARAM_DEV_SOC_EXTER);

  mtos_sem_give(&reg_rw_mutex);
}

u8 dc2800_tn_reg_read(MT_FE_DC2800_Device_Handle p_handle, u8 reg)
{
  u8 data = reg;
  u8 value;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
  mtos_sem_take(&reg_rw_mutex, 0);

  if(p_priv->chip_mode != 2)
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x87);
    value &= 0x0F;
    value |= 0xa0;    // bit7         = 1, Enable I2C repeater
    // bit[6:4]     = 1, Enable I2C repeater for 1 time
    dc2800_dmd_reg_write_unlock(p_handle, 0x87, value);
    i2c_std_read(p_priv->p_i2c, p_handle->tuner_settings.tuner_dev_addr, 
                &data, 1, 1, I2C_PARAM_DEV_SOC_EXTER);
  }
  else
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x86);
    value |= 0x80;
    dc2800_dmd_reg_write_unlock(p_handle, 0x86, value);
    i2c_seq_read(p_priv->p_i2c, p_handle->tuner_settings.tuner_dev_addr, 
                &data, 1, 1, I2C_PARAM_DEV_SOC_EXTER);
  }


  mtos_sem_give(&reg_rw_mutex);

  return data;
}

s32 dc2800_tn_std_read(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen)
{
  s32 ret = 0;
  u8 value;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
  mtos_sem_take(&reg_rw_mutex, 0);

  if(p_priv->chip_mode != 2)
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x87);
    value &= 0x0F;
    value |= 0xa0;    // bit7         = 1, Enable I2C repeater
    // bit[6:4]     = 1, Enable I2C repeater for 1 time
    dc2800_dmd_reg_write_unlock(p_handle, 0x87, value);
  }
  else
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x86);
    value |= 0x80;
    dc2800_dmd_reg_write_unlock(p_handle, 0x86, value);
  }

  ret = i2c_std_read(p_priv->p_i2c, p_handle->tuner_settings.tuner_dev_addr, 
            p_buf, wlen, rlen, I2C_PARAM_DEV_SOC_EXTER);

  mtos_sem_give(&reg_rw_mutex);

  return ret;
}

s32 dc2800_tn_seq_read(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen)
{
  s32 ret = 0;
  u8 value;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
  mtos_sem_take(&reg_rw_mutex, 0);

  if(p_priv->chip_mode != 2)
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x87);
    value &= 0x0F;
    value |= 0x90;    // bit7         = 1, Enable I2C repeater
                // bit[6:4]     = 1, Enable I2C repeater for 1 time
    dc2800_dmd_reg_write_unlock(p_handle, 0x87, value);
  }
  else
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x86);
    value |= 0x80;
    dc2800_dmd_reg_write_unlock(p_handle, 0x86, value);
  }

  ret = i2c_seq_read(p_priv->p_i2c, p_handle->tuner_settings.tuner_dev_addr, 
            p_buf, wlen, rlen, I2C_PARAM_DEV_SOC_EXTER);

  mtos_sem_give(&reg_rw_mutex);

  return ret;
}

s32 dc2800_dmd_std_read(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen)
{
  s32 ret = 0;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
  mtos_sem_take(&reg_rw_mutex, 0);

  ret = i2c_std_read(p_priv->p_i2c, p_priv->dmd_addr, p_buf, wlen, rlen, 
            I2C_PARAM_DEV_SOC_EXTER);

  mtos_sem_give(&reg_rw_mutex);

  return ret;
}

s32 dc2800_dmd_seq_read(MT_FE_DC2800_Device_Handle p_handle,u8 *p_buf, u32 wlen, u32 rlen)
{
  s32 ret = 0;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
  mtos_sem_take(&reg_rw_mutex, 0);

  ret = i2c_seq_read(p_priv->p_i2c, p_priv->dmd_addr, p_buf, wlen, rlen, 
            I2C_PARAM_DEV_SOC_EXTER);

  mtos_sem_give(&reg_rw_mutex);

  return ret;
}

s32 dc2800_tn_seq_write(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 len)
{
  RET_CODE ret = ERR_FAILURE;
  u8 value;
    nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
  mtos_sem_take(&reg_rw_mutex, 0);

  if(p_priv->chip_mode != 2)
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x87);
    value &= 0x0F;
    value |= 0x90;    // bit7         = 1, Enable I2C repeater
    // bit[6:4]     = 1, Enable I2C repeater for 1 time
    dc2800_dmd_reg_write_unlock(p_handle, 0x87, value);
  }
  else
  {
    value = dc2800_dmd_reg_read_unlock(p_handle, 0x86);
    value |= 0x80;
    dc2800_dmd_reg_write_unlock(p_handle, 0x86, value);
  }

  ret = i2c_write(p_priv->p_i2c, p_handle->tuner_settings.tuner_dev_addr, 
            p_buf, len, I2C_PARAM_DEV_SOC_EXTER);

  mtos_sem_give(&reg_rw_mutex);

  return ret;
}

s32 dc2800_dmd_seq_write(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 len)
{
    RET_CODE ret = ERR_FAILURE;
     nim_m88dc280_priv_t *p_priv = NULL;
    p_priv = CONTAIN_OF(p_handle, nim_m88dc280_priv_t, handle);
    
    mtos_sem_take(&reg_rw_mutex, 0);
  
    ret = i2c_write(p_priv->p_i2c, p_priv->dmd_addr, p_buf, len, 
                I2C_PARAM_DEV_SOC_EXTER);
  
    mtos_sem_give(&reg_rw_mutex);

    return ret;
}

static signed long nim_m88dc2800_ioctrl(lld_nim_t *p_dev, u32 cmd, u32 param);

static signed long nim_m88dc2800_channel_connect(struct lld_nim *p_dev,
    nim_channel_info_t *p_channel_info, BOOL for_scan) {
  S32 lock_timeout_ms, cnt;
  MT_FE_LOCK_STATE lock_st = 0;
  u16 n_qam = 0;
  u32 freq = 0;
  u32 symb = 0;

  nim_m88dc280_priv_t *p_priv = (nim_m88dc280_priv_t *) (p_dev->p_priv);
  if (NULL == p_dev || NULL == p_channel_info) {
    return ERR_FAILURE;
  }
  
  memcpy(&p_priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));
  freq = p_channel_info->frequency;
  symb = p_channel_info->param.dvbc.symbol_rate;

  if(freq < 48000 || freq > 1010000)
  {
        freq = 48000;
  }
  if(freq == 0)
  {
    freq = 1;
  }
  if(symb == 0)
  {
    symb = 1;
  }
  
 // g_dc2800_tn_drv.set_freq(freq, p_channel_info->param.dvbc.modulation);
  n_qam = parse_qam(p_channel_info->param.dvbc.modulation);
  mt_fe_dmd_dc2800_connect(&p_priv->handle, freq, symb, n_qam,  p_channel_info->spectral_polar);
  /* wait for the status, if failed ,until timeout return */
  if (((dmd_rd_reg(0xE3) & 0x80) == 0x80) && ((dmd_rd_reg(0xE4) & 0x80) == 0x80))
    lock_timeout_ms = 800;
  else
    lock_timeout_ms = 500;  
  for (cnt = 0; cnt < lock_timeout_ms; cnt += 80) {
    mt_fe_dmd_dc2800_get_lock_state(&p_priv->handle, &lock_st);

    if (MtFeLockState_Locked == lock_st) {
      p_channel_info->lock = 1;
      return SUCCESS;
    }
    mtos_task_sleep(100);
  }

  /* Jazz mode don't need change io polarity */
  if(p_priv->chip_mode == 1)
  {
    p_channel_info->lock = 0;
    return SUCCESS;
  }

  /* if unlocked, change io polarity and try lock again */
  nim_m88dc2800_ioctrl(p_dev, NIM_IOCTRL_CHANGE_IQ, 0);

  if (((dmd_rd_reg(0xE3) & 0x80) == 0x80) && ((dmd_rd_reg(0xE4) & 0x80) == 0x80))
    lock_timeout_ms = 800;
  else
    lock_timeout_ms = 500;
  
  /* wait for the status, if failed ,until timeout return */
  for (cnt = 0; cnt < lock_timeout_ms; cnt += 80) {
    mt_fe_dmd_dc2800_get_lock_state(&p_priv->handle, &lock_st);
    if (MtFeLockState_Locked == lock_st) {
      p_channel_info->lock = 1;
      p_channel_info->spectral_polar = p_priv->cur_channel.spectral_polar;
      return SUCCESS;
    }
    mtos_task_sleep(100);
  }
  p_channel_info->lock = 0;
  return SUCCESS;
}

static signed long nim_m88dc2800_channel_set(struct lld_nim *p_dev,
    nim_channel_info_t *p_channel_info,
    nim_channel_set_info_t *p_channel_set_info) {
  S32 lock_timeout_ms = 0;
  nim_m88dc280_priv_t *p_priv = (nim_m88dc280_priv_t *) (p_dev->p_priv);
  u16 n_qam = 0;
  u32 freq = 0;
  u32 symb = 0;
  
  if (NULL == p_dev || NULL == p_channel_info) {
    return ERR_FAILURE;
  }
  
  memcpy(&p_priv->cur_channel, p_channel_info, sizeof(nim_channel_info_t));
  freq = p_channel_info->frequency;
  symb = p_channel_info->param.dvbc.symbol_rate;
    if(freq < 48000 || freq > 1010000)
    {
        freq = 48000;
    }
  if(freq == 0)
  {
    freq = 1;
  }
  if(symb == 0)
  {
    symb = 1;
  }
  
//  g_dc2800_tn_drv.set_freq(freq, p_channel_info->param.dvbc.modulation);

  n_qam = parse_qam(p_channel_info->param.dvbc.modulation);
  mt_fe_dmd_dc2800_connect(&p_priv->handle, freq, symb, n_qam, 
  p_channel_info->spectral_polar);
  if (((dmd_rd_reg(0xE3) & 0x80) == 0x80) && ((dmd_rd_reg(0xE4) & 0x80) == 0x80))
    lock_timeout_ms = 800;
  else
    lock_timeout_ms = 500;

  p_channel_set_info->lock_time = lock_timeout_ms;
  return SUCCESS;
}

static signed long nim_m88dc2800_channel_perf(struct lld_nim *p_dev,
    nim_channel_perf_t *p_channel_perf) 
{
  u16 n_qam = 0;
  nim_m88dc280_priv_t *p_priv = (nim_m88dc280_priv_t *) (p_dev->p_priv);

  if (NULL == p_dev || NULL == p_channel_perf) {
    return ERR_FAILURE;
  }
  
  n_qam = parse_qam(p_priv->cur_channel.param.dvbc.modulation);
  
  if(p_priv->handle.tuner_settings.tuner_get_strength == NULL)
  {
    u8 _agc;
    mt_fe_dmd_dc2800_get_strength(&p_priv->handle, &_agc);
    p_channel_perf->agc = _agc;
  }
  else
  {
    u32 _gain = 0;
    u32 __agc = 0;
    p_priv->handle.tuner_settings.tuner_get_strength(&p_priv->handle,&_gain, &__agc);
    p_channel_perf->agc = __agc;
  }
  {
    u32 error_bits;
    u32 total_bits;
    mt_fe_dmd_dc2800_get_ber(&p_priv->handle, &error_bits, &total_bits);
    p_channel_perf->ber  = (double)error_bits / total_bits;
  }

  {
      MT_FE_LOCK_STATE is_lock;
      mt_fe_dmd_dc2800_get_lock_state(&p_priv->handle, &is_lock);
      if(is_lock == MtFeLockState_Locked)
      {
          p_channel_perf->lock  = 1;
      }
      else
      {
          p_channel_perf->lock = 0;
      }
  }

  {
    u8 snr;
    mt_fe_dmd_dc2800_get_snr(&p_priv->handle, &snr);
    p_channel_perf->snr = snr;
  }

  return SUCCESS;
}

/* nim modules ways */
static signed long nim_m88dc2800_ioctrl(lld_nim_t *p_dev, u32 cmd, u32 param) 
{
  MT_FE_LOCK_STATE lock_st = 0;
  nim_m88dc280_priv_t *p_priv = (nim_m88dc280_priv_t *) (p_dev->p_priv);
  //u8 ts_type = 0;
  u32 version_time = 0;
  MT_FE_TN_TC2800_Handle tuner_handle = (MT_FE_TN_TC2800_Handle)(p_priv->handle.tuner_settings.tuner_handle);

  switch (cmd) {
    case NIM_IOCTRL_CHECK_LOCK:

      mt_fe_dmd_dc2800_get_lock_state(&p_priv->handle, &lock_st);
      if(lock_st == MtFeLockState_Locked)
      {
           *((u8 *) param) = 1;
      }
      else
      {
          *((u8 *) param) = 0;
      }
      break;

    case NIM_IOCTRL_CHANGE_IQ:
      if (0 == p_priv->cur_channel.spectral_polar) {
        p_priv->cur_channel.spectral_polar = 1;
      } else {
        p_priv->cur_channel.spectral_polar = 0;
      }

      _mt_fe_dmd_dc2800_set_tx_mode(&p_priv->handle,p_priv->cur_channel.spectral_polar,0);
      mt_fe_dmd_dc2800_soft_reset(&p_priv->handle);

      break;

    case NIM_IOCTRL_SET_TS_MODE:
      if(param == NIM_TS_INTF_PARALLEL)
      {
          _mt_fe_dmd_dc2800_set_ts_output(&p_priv->handle,MtFeTsOutMode_Parallel);
      }
      else if(param == NIM_TS_INTF_COMMON)
      {
         _mt_fe_dmd_dc2800_set_ts_output(&p_priv->handle,MtFeTsOutMode_Common);
      }
      else
      {
            _mt_fe_dmd_dc2800_set_ts_output(&p_priv->handle,MtFeTsOutMode_Serial);

      }
      break;
    case NIM_IOCTRL_GET_TN_VERSION:
      *((u32 *)param) = tuner_handle->tuner_version;
      break;
    case NIM_IOCTRL_GET_DMD_VERSION:
      mt_fe_dmd_dc2800_get_driver_version((U32 *)param, (U32 *)&version_time);
      break;
    case NIM_IOCTRL_SET_TUNER_SLEEP:
    p_priv->handle.tuner_settings.tuner_sleep(&p_priv->handle);
    break;
    case NIM_IOCTRL_SET_TUNER_WAKEUP:
    p_priv->handle.tuner_settings.tuner_wakeup(&p_priv->handle);
    break;
    case NIM_IOCTRL_GET_TN_TYPE:
      switch(p_priv->tuner_type)
        {
           case TN_MONTAGE_TC2800:
            *((u8 *) param) = TC2800;
            break;
            case TN_MONTAGE_TC2000:
            *((u8 *) param) = TC2000;
            break;
            case TN_ALPS_TDAC:
            *((u8 *) param) = TC_ALPS_TDAC;
            break;
            case TN_THOMSON_DCT7070X:
            *((u8 *) param) = TC_THOMSON_DCT7070X;
            break;
            case TN_XUGUANG_XDCT6A:
            *((u8 *) param) = TC_XUGUANG_XDCT6A;
            break;
            case TN_LG_TDCC:
            *((u8 *) param) = TC_LG_TDCC;
            break;
            case TN_TDCCG051F:
            *((u8 *) param) = TC_TDCCG051F;
            break;
            case MT_FE_TN_NOTSUPPORT:
            *((u8 *) param) = TC_MT_FE_TN_NOTSUPPORT;
            break;
             case MT_FE_TN_USERDEFINE:
            *((u8 *) param) = TC_MT_FE_TN_USERDEFINE;
            break;
            default:
             *((u8 *) param) = TC2800;
             break;
        }
     break;
    case NIM_IOCTRL_GET_CHANNEL_INFO:
        memcpy((nim_channel_info_t *)param,&p_priv->cur_channel, sizeof(nim_channel_info_t));
        break;
    case NIM_IOCTRL_SET_TUNER_BANDWIDTH:
        p_priv->handle.tuner_settings.tuner_bandwidth = param;
        break;
    default:
        break;
  }

  return SUCCESS;
}

static signed long nim_m88dc2800_open(lld_nim_t *p_lld, void *p_cfg) 
{
  nim_config_t *p_nim_cfg = (nim_config_t *) p_cfg;
  nim_m88dc280_priv_t *p_priv = NULL;
  i2c_cfg_t i2c_cfg;
  u32 ts_mode = NIM_TS_INTF_DEF;
  drv_dev_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  p_dev = DEV_FIND_BY_LLD(drv_dev_t, p_lld);
  p_base = p_dev->p_base;


  if(reg_rw_mutex == 0)
  {
    mtos_sem_create (&reg_rw_mutex, TRUE);
  }
  
  p_lld->p_priv = mtos_malloc(sizeof(nim_m88dc280_priv_t));
  memset(p_lld->p_priv, 0x00, sizeof(nim_m88dc280_priv_t));
  p_priv = p_lld->p_priv;
  
  if (NULL != p_nim_cfg) 
  {
    p_base->lock_mode = p_nim_cfg->lock_mode;
    p_priv->p_i2c = (i2c_bus_t *) p_nim_cfg->p_dem_bus;
    if (0 != p_nim_cfg->x_crystal) 
    {
      p_priv->x_crystal = p_nim_cfg->x_crystal;
    } 
    else 
    {
      p_priv->x_crystal = X_TAL;
    }
    
    ts_mode = p_nim_cfg->ts_mode;
    
    if(p_nim_cfg->dem_addr != 0)
    {
      p_priv->dmd_addr = p_nim_cfg->dem_addr;
    }
    else
    {
      p_priv->dmd_addr = DEMOD_I2C_ADDR;
    }
    
    if(p_nim_cfg->dem_ver == DEM_VER_0)
    {
      p_priv->chip_mode = 2;
    }    
    else if(p_nim_cfg->dem_ver == DEM_VER_1)
    {
      p_priv->chip_mode = 0;
    }
    else
    {
        p_priv->chip_mode = 1;
    }

    p_priv->tuner_loopthrough = p_nim_cfg->tuner_loopthrough;
    if(p_nim_cfg->tuner_bandwidth == 0)
    {
          p_priv->tuner_bandwidth = 8;
    }
    else
    {
          p_priv->tuner_bandwidth = p_nim_cfg->tuner_bandwidth;        
    }
    if(p_priv->tuner_mode != 0)
    {
        p_priv->tuner_mode = p_nim_cfg->tuner_mode;
    }
  } 
  else 
  {
    p_base->lock_mode = OS_MUTEX_LOCK;
    p_priv->p_i2c = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
    MT_ASSERT(NULL != p_priv->p_i2c);

    i2c_cfg.bus_clk_khz=200;
    dev_open(p_priv->p_i2c, &i2c_cfg);

    p_priv->x_crystal = X_TAL;
    p_priv->dmd_addr = DEMOD_I2C_ADDR;
    p_priv->chip_mode = 0;
    p_priv->tuner_loopthrough = 0;
    p_priv->tuner_bandwidth = 8;
    p_priv->tuner_mode = 0;
  }

  MT_ASSERT(NULL != p_priv->p_i2c);

  /* init nim hw */
  mt_fe_dmd_dc2800_config_default(&p_priv->handle);
  p_priv->handle.demod_dev_addr = p_priv->dmd_addr;
  p_priv->handle.on_board_settings.chip_mode = p_priv->chip_mode;
  p_priv->handle.on_board_settings.xtal_KHz = p_priv->x_crystal;
  p_priv->handle.tuner_settings.tuner_loopthrough = p_priv->tuner_loopthrough;
  p_priv->handle.tuner_settings.tuner_bandwidth = p_priv->tuner_bandwidth;
  p_priv->handle.tuner_settings.tuner_mode = p_priv->tuner_mode;
  if (NULL != p_nim_cfg && 0 != p_nim_cfg->tun_addr)
  {
      p_priv->handle.tuner_settings.tuner_dev_addr = p_nim_cfg->tun_addr;
  }
  if (NULL != p_nim_cfg && (0x29 == p_nim_cfg->tuner_ver || 29 == p_nim_cfg->tuner_ver))
  {
      p_priv->handle.tuner_settings.tuner_ver = p_nim_cfg->tuner_ver;
  }
  
  mt_fe_dmd_dc2800_select_tuner(&p_priv->handle,dc2800_tuner_type);
  p_priv->tuner_type = dc2800_tuner_type;
  
  mt_fe_dmd_dc2800_init(&p_priv->handle);
  
  if(ts_mode == NIM_TS_INTF_SERIAL)
  {
        p_priv->handle.ts_output_settings.output_mode = MtFeTsOutMode_Serial;
  }
  else
  {
p_priv->handle.ts_output_settings.output_mode = MtFeTsOutMode_Parallel;
  }
  
  nim_m88dc2800_ioctrl(p_lld, NIM_IOCTRL_SET_TS_MODE, ts_mode);
  {
      U32 dmd_version_no = 0;
      U32 dmd_version_time = 0;
      U32 tn_version_no = 0;
      U32 tn_version_time = 0;
      
      mt_fe_dmd_dc2800_get_driver_version(&dmd_version_no, &dmd_version_time);
      p_priv->handle.tuner_settings.tuner_get_version
      (&p_priv->handle, &tn_version_no, &tn_version_time);
      OS_PRINTF("\n========Demod Version Info========\n");
      OS_PRINTF("    version no: %d\n", dmd_version_no);
      OS_PRINTF("    version time: %d\n", dmd_version_time);
      OS_PRINTF("========Tuner Version Info========\n");
      OS_PRINTF("    version no: %d\n", tn_version_no);
      OS_PRINTF("    version time: %d\n", tn_version_time);     
  }
  return SUCCESS;
}

RET_CODE nim_m88dc2800_close(lld_nim_t *p_lld) {
  nim_m88dc280_priv_t *p_priv = (nim_m88dc280_priv_t *) (p_lld->p_priv);
  mtos_free(p_priv);

  return SUCCESS;
}

static void nim_m88dc2800_detach(lld_nim_t *p_lld) {
}

static signed long nim_m88dc2800_open1(void *p_lld, void *p_cfg) 
{
  return (signed long)nim_m88dc2800_open((lld_nim_t *)p_lld, p_cfg);
}
static signed long nim_m88dc2800_close1(void *p_lld)
{
   return (signed long)nim_m88dc2800_close((lld_nim_t *)p_lld);
}

static signed long nim_m88dc2800_ioctrl1(void *p_lld, unsigned long cmd, unsigned long param)
{
   return nim_m88dc2800_ioctrl((lld_nim_t *)p_lld,cmd,param);
}


/* Register m88rs2000 tuner deivce */
RET_CODE nim_m88dc2800_attach(char *name) {
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
  dev_base->open = nim_m88dc2800_open1;
  //dev_base->close = (RET_CODE (*)(void *))nim_m88dc2800_close;
  dev_base->close = nim_m88dc2800_close1;
  dev_base->detach = (void (*)(void *))nim_m88dc2800_detach;
  //dev_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))nim_m88dc2800_ioctrl;
  dev_base->io_ctrl = nim_m88dc2800_ioctrl1;

  /* attach lld driver */
  lld_dev = (lld_nim_t *)dev->p_priv;
  lld_dev->channel_scan = NULL;
  lld_dev->channel_connect =nim_m88dc2800_channel_connect;
  lld_dev->channel_set = nim_m88dc2800_channel_set;
  lld_dev->channel_perf = nim_m88dc2800_channel_perf;
  lld_dev->diseqc_ctrl = NULL;

  /* attach the default tuner driver */
  extern void tuner_attach_TDCC_G051F(void);
  tuner_attach_TDCC_G051F();
  
  return SUCCESS;
}

MT_FE_DC2800_Device_Handle dc_2800_get_handle()
{
  return g_p_dc2800_device_handle;
}

extern MT_FE_RET mt_fe_dmd_dc2800_cfg_def_no_tn_intf(MT_FE_DC2800_Device_Handle handle);
static RET_CODE _dc2800_open(lld_nim_t *p_lld, void *p_cfg) 
{
  nim_config_t *p_nim_cfg = (nim_config_t *) p_cfg;
  nim_m88dc280_priv_t *p_priv = NULL;
  i2c_cfg_t i2c_cfg;
  u32 ts_mode = NIM_TS_INTF_DEF;
  drv_dev_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  p_dev = DEV_FIND_BY_LLD(drv_dev_t, p_lld);
  p_base = p_dev->p_base;


  if(reg_rw_mutex == 0)
  {
    mtos_sem_create (&reg_rw_mutex, TRUE);
  }
  
  p_lld->p_priv = mtos_malloc(sizeof(nim_m88dc280_priv_t));
  memset(p_lld->p_priv, 0x00, sizeof(nim_m88dc280_priv_t));
  p_priv = p_lld->p_priv;
  
  if (NULL != p_nim_cfg) 
  {
    p_base->lock_mode = p_nim_cfg->lock_mode;
    p_priv->p_i2c = (i2c_bus_t *) p_nim_cfg->p_dem_bus;
    if (0 != p_nim_cfg->x_crystal) 
    {
      p_priv->x_crystal = p_nim_cfg->x_crystal;
    } 
    else 
    {
      p_priv->x_crystal = X_TAL;
    }
    
    ts_mode = p_nim_cfg->ts_mode;
    
    if(p_nim_cfg->dem_addr != 0)
    {
      p_priv->dmd_addr = p_nim_cfg->dem_addr;
    }
    else
    {
      p_priv->dmd_addr = DEMOD_I2C_ADDR;
    }
    
    if(p_nim_cfg->dem_ver == DEM_VER_0)
    {
      p_priv->chip_mode = 2;
    }    
    else if(p_nim_cfg->dem_ver == DEM_VER_1)
    {
      p_priv->chip_mode = 0;
    }
    else
    {
        p_priv->chip_mode = 1;
    }

    p_priv->tuner_loopthrough = p_nim_cfg->tuner_loopthrough;
    if(p_nim_cfg->tuner_bandwidth == 0)
    {
          p_priv->tuner_bandwidth = 8;
    }
    else
    {
          p_priv->tuner_bandwidth = p_nim_cfg->tuner_bandwidth;        
    }
    if(p_priv->tuner_mode != 0)
    {
        p_priv->tuner_mode = p_nim_cfg->tuner_mode;
    }
  } 
  else 
  {
    p_base->lock_mode = OS_MUTEX_LOCK;
    p_priv->p_i2c = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_BUS_TYPE_I2C);
    MT_ASSERT(NULL != p_priv->p_i2c);

    i2c_cfg.bus_clk_khz=200;
    dev_open(p_priv->p_i2c, &i2c_cfg);

    p_priv->x_crystal = X_TAL;
    p_priv->dmd_addr = DEMOD_I2C_ADDR;
    p_priv->chip_mode = 0;
    p_priv->tuner_loopthrough = 0;
    p_priv->tuner_bandwidth = 8;
    p_priv->tuner_mode = 0;
  }

  MT_ASSERT(NULL != p_priv->p_i2c);

  /* init nim hw */
  g_p_dc2800_device_handle = &p_priv->handle;
  mt_fe_dmd_dc2800_cfg_def_no_tn_intf(&p_priv->handle);
  p_priv->handle.demod_dev_addr = p_priv->dmd_addr;
  p_priv->handle.on_board_settings.chip_mode = p_priv->chip_mode;
  p_priv->handle.on_board_settings.xtal_KHz = p_priv->x_crystal;
  p_priv->handle.tuner_settings.tuner_loopthrough = p_priv->tuner_loopthrough;
  p_priv->handle.tuner_settings.tuner_bandwidth = p_priv->tuner_bandwidth;
  p_priv->handle.tuner_settings.tuner_mode = p_priv->tuner_mode;
  if (NULL != p_nim_cfg && 0 != p_nim_cfg->tun_addr)
  {
      p_priv->handle.tuner_settings.tuner_dev_addr = p_nim_cfg->tun_addr;
  }
  if (NULL != p_nim_cfg && (0x29 == p_nim_cfg->tuner_ver || 29 == p_nim_cfg->tuner_ver))
  {
      p_priv->handle.tuner_settings.tuner_ver = p_nim_cfg->tuner_ver;
  }

  mt_fe_dmd_dc2800_init(&p_priv->handle);
  if(ts_mode == NIM_TS_INTF_SERIAL)
  {
        p_priv->handle.ts_output_settings.output_mode = MtFeTsOutMode_Serial;
  }
  else
  {
p_priv->handle.ts_output_settings.output_mode = MtFeTsOutMode_Parallel;
  }
  nim_m88dc2800_ioctrl(p_lld, NIM_IOCTRL_SET_TS_MODE, ts_mode);
  {
      U32 dmd_version_no = 0;
      U32 dmd_version_time = 0;
      U32 tn_version_no = 0;
      U32 tn_version_time = 0;
      
      mt_fe_dmd_dc2800_get_driver_version(&dmd_version_no, &dmd_version_time);
      p_priv->handle.tuner_settings.tuner_get_version
      (&p_priv->handle, &tn_version_no, &tn_version_time);
      OS_PRINTF("\n========Demod Version Info========\n");
      OS_PRINTF("    version no: %d\n", dmd_version_no);
      OS_PRINTF("    version time: %d\n", dmd_version_time);
      OS_PRINTF("========Tuner Version Info========\n");
      OS_PRINTF("    version no: %d\n", tn_version_no);
      OS_PRINTF("    version time: %d\n", tn_version_time);     
  }
  return SUCCESS;
}

RET_CODE nim_m88dc2800_single_tuner_attach(char *name) 
{
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
  dev_base->open = (signed long (*)(void *, void *))_dc2800_open;
  dev_base->close = (signed long (*)(void *))nim_m88dc2800_close;
  dev_base->detach = (void (*)(void *))nim_m88dc2800_detach;
  dev_base->io_ctrl = (signed long (*)(void *, unsigned long, unsigned long))nim_m88dc2800_ioctrl;

  /* attach lld driver */
  lld_dev = (lld_nim_t *)dev->p_priv;
  lld_dev->channel_scan = NULL;
  lld_dev->channel_connect =nim_m88dc2800_channel_connect;
  lld_dev->channel_set = nim_m88dc2800_channel_set;
  lld_dev->channel_perf = nim_m88dc2800_channel_perf;
  lld_dev->diseqc_ctrl = NULL;
  
  return SUCCESS;
}

