/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_sem.h"
#include "mtos_mutex.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "drv_dev.h"
#include "../inc/drv/bus/spi.h"
#include "../../drvbase/drv_dev_priv.h"
#include "spi_priv.h"

static void spi_lock(device_base_t *p_base)
{
   if (OS_LOCK_DEF == p_base->lock_mode){
      mtos_sem_take(&p_base->mutex, 0);
   }else{
       drv_lock(p_base);
   }
}

static void spi_unlock(device_base_t *p_base)
{
   if (OS_LOCK_DEF == p_base->lock_mode){
      mtos_sem_give(&p_base->mutex);
   }else{
       drv_unlock(p_base);
   }
}


RET_CODE spi_read(spi_bus_t *p_dev, u8 *p_cmd_buf, u32 cmd_len, spi_cmd_cfg_t * p_cmd_cfg,
                  u8 *p_data_buf, u32 data_len)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_spi_t *p_lld = (lld_spi_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS) 
  {
    MT_ASSERT(p_lld->read != NULL);
    spi_lock(p_base);
    ret = p_lld->read(p_lld, p_cmd_buf, cmd_len, p_cmd_cfg, p_data_buf, data_len);
    spi_unlock(p_base);
  }

  return ret;
}

RET_CODE spi_write(spi_bus_t *p_dev, u8 *p_cmd_buf, u32 cmd_len, spi_cmd_cfg_t * p_cmd_cfg,
                   u8 *p_data_buf, u32 data_len)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_spi_t *p_lld = (lld_spi_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS) 
  {
    MT_ASSERT(p_lld->write != NULL);
    spi_lock(p_base);
    ret = p_lld->write(p_lld, p_cmd_buf, cmd_len, p_cmd_cfg,p_data_buf, data_len);
    spi_unlock(p_base);
  }

  return ret;
}

RET_CODE spi_soft_reset(spi_bus_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_spi_t *p_lld = (lld_spi_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->soft_reset != NULL);
    spi_lock(p_base);
    ret = p_lld->soft_reset(p_lld);
    spi_unlock(p_base);
  }

  return ret;
}

RET_CODE spi_reset(spi_bus_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_spi_t *p_lld = (lld_spi_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->reset != NULL);
    spi_lock(p_base);
    ret = p_lld->reset(p_lld);
    spi_unlock(p_base);
  }

  return ret;
}
