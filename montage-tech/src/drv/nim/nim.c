/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "drv_dev.h"
#include "../drvbase/drv_dev_priv.h"
#include "nim.h"
#include "nim_priv.h"

/*!
  get nim  lock

  \param[in] p_base The handle of device_base_t of display module.

  */
void nim_lock(device_base_t *p_base)
{
    if (OS_LOCK_DEF == p_base->lock_mode){
        mtos_sem_take(&p_base->mutex, 0);
    }else{
        drv_lock(p_base);
    }
}

/*!
  release  nim  lock

  \param[in] p_base The handle of device_base_t of display module.

  */
void nim_unlock(device_base_t *p_base)
{
    if (OS_LOCK_DEF == p_base->lock_mode){
        mtos_sem_give(&p_base->mutex);
    }else{
        drv_unlock(p_base);
    }
}

RET_CODE nim_channel_scan(nim_device_t *p_dev,
                   nim_scan_info_t *p_scan_info)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  struct device_base *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->channel_scan == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    //MT_ASSERT(p_lld->channel_scan != NULL);
    nim_lock(p_base);
    ret = p_lld->channel_scan(p_lld, p_scan_info);
    nim_unlock(p_base);
  }

  //OS_PRINTF("Current chan number %d\n", p_scan_info->channel_num);
  return ret;
}

RET_CODE nim_channel_connect(nim_device_t *p_dev,
              nim_channel_info_t *p_channel_info, BOOL for_scan)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->channel_connect == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    //MT_ASSERT(p_lld->channel_connect != NULL);
    nim_lock(p_base);
    ret = p_lld->channel_connect(p_lld, p_channel_info, for_scan);
    nim_unlock(p_base);
  }

  return ret;
}

RET_CODE nim_channel_set(nim_device_t *p_dev,
               nim_channel_info_t *p_channel_info,
               nim_channel_set_info_t *p_channel_set_info)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->channel_set == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    //MT_ASSERT(p_lld->channel_set != NULL);
    nim_lock(p_base);
    ret = p_lld->channel_set(p_lld, p_channel_info, p_channel_set_info);
    nim_unlock(p_base);
  }

  return ret;
}

RET_CODE nim_channel_perf(nim_device_t *p_dev,
               nim_channel_perf_t *p_channel_perf)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->channel_perf == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    nim_lock(p_base);
    ret = p_lld->channel_perf(p_lld, p_channel_perf);
    nim_unlock(p_base);
  }

  return ret;
}

RET_CODE nim_diseqc_ctrl(nim_device_t *p_dev,
               nim_diseqc_cmd_t *p_diseqc_cmd)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->diseqc_ctrl == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    //MT_ASSERT(p_lld->diseqc_ctrl != NULL);
    nim_lock(p_base);
    ret = p_lld->diseqc_ctrl(p_lld, p_diseqc_cmd);
    nim_unlock(p_base);
  }

  return ret;
}

RET_CODE nim_blind_scan_start(nim_device_t *p_dev, nim_scan_info_t *p_scan_info)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->blind_scan_start == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    nim_lock(p_base);
    ret = p_lld->blind_scan_start(p_lld, p_scan_info);
    nim_unlock(p_base);
  }

  return ret;
}

RET_CODE nim_blind_scan_cancel(nim_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  //device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->blind_scan_cancel == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    ret = p_lld->blind_scan_cancel(p_lld);
  }

  return ret;
}

RET_CODE nim_notify_register(nim_device_t *p_dev, nim_notify func)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_nim_t *p_lld = (lld_nim_t *)p_dev->p_priv;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;

  if(p_lld->notify_register == NULL)
  {
    return ERR_NOFEATURE;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    nim_lock(p_base);
    ret = p_lld->notify_register(p_lld, func);
    nim_unlock(p_base);
  }

  return ret;
}
