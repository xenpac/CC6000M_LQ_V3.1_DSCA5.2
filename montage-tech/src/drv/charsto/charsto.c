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
#include "mtos_task.h"
#include "mtos_printk.h"
#include "drv_dev.h"
#include "../drvbase/drv_dev_priv.h"
#include "../charsto.h"
#include "charsto_priv.h"


/*!
  get charsto  lock 
  
  \param[in] p_base The handle of device_base_t of display module.

  */
static void charsto_lock(device_base_t *p_base)
{
    if (OS_LOCK_DEF == p_base->lock_mode){
        mtos_sem_take(&p_base->mutex, 0);
    }else{
        drv_lock(p_base);
    }
}

/*!
  release  charsto  lock 
  
  \param[in] p_base The handle of device_base_t of display module.

  */
static void charsto_unlock(device_base_t *p_base)
{
    if (OS_LOCK_DEF == p_base->lock_mode){
        mtos_sem_give(&p_base->mutex);
    }else{
        drv_unlock(p_base);
    }
}

RET_CODE charsto_read(charsto_device_t *p_dev, u32 addr, 
                   u8 *p_buf, u32 len)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->read != NULL);
    charsto_lock(p_base);
    ret = p_lld->read(p_lld, addr, p_buf, len);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_writeonly(charsto_device_t *p_dev, u32 addr, 
                  u8 *p_buf, u32 len)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->writeonly != NULL);
    charsto_lock(p_base);
    ret = p_lld->writeonly(p_lld, addr, p_buf, len);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_erase(charsto_device_t *p_dev, u32 addr, u32 sec_cnt)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->erase != NULL);
    charsto_lock(p_base);
    ret = p_lld->erase(p_lld, addr, sec_cnt);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_protect_all(charsto_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->protect_all != NULL);
    charsto_lock(p_base);
    ret = p_lld->protect_all(p_lld);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_unprotect_all(charsto_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->unprotect_all != NULL);
    charsto_lock(p_base);
    ret = p_lld->unprotect_all(p_lld);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_get_cacapcity(charsto_device_t *p_dev, u32 *p_cap)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->get_capacity != NULL);
    charsto_lock(p_base);
    ret = p_lld->get_capacity(p_lld,p_cap);
    charsto_unlock(p_base);
  }

  return ret;
}

RET_CODE charsto_get_unique_id(charsto_device_t *p_dev, u8 *buffer, u32 len, u32 *act_len)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->get_unique_id!= NULL);
    charsto_lock(p_base);
    ret = p_lld->get_unique_id(p_lld, buffer, len, act_len);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_otp_read(charsto_device_t *p_dev, u32 addr, 
                   u8 *p_buf, u32 len)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->read != NULL);
    charsto_lock(p_base);
    ret = p_lld->otp_read(p_lld, addr, p_buf, len);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_otp_write(charsto_device_t *p_dev, u32 addr, 
                  u8 *p_buf, u32 len)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->writeonly != NULL);
    charsto_lock(p_base);
    ret = p_lld->otp_write(p_lld, addr, p_buf, len);
    charsto_unlock(p_base);
  }

  return ret;
}


RET_CODE charsto_otp_erase(charsto_device_t *p_dev, u32 addr)
{
  RET_CODE ret = ERR_NOFEATURE;
  device_base_t *p_base = (device_base_t *)p_dev->p_base;
  lld_charsto_t *p_lld = (lld_charsto_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld->erase != NULL);
    charsto_lock(p_base);
    ret = p_lld->otp_erase(p_lld, addr);
    charsto_unlock(p_base);
  }

  return ret;
}

