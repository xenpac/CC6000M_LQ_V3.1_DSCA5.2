/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_sem.h"
#include "mtos_fifo.h"
#include "mtos_printk.h"
#include "drv_dev.h"
#include "../drvbase/drv_dev_priv.h"
#include "uio.h"
#include "uio_priv.h"

#define FP_START_WORK 0
#define FP_STOP_WORK  1

RET_CODE uio_get_code(uio_device_t *p_dev, u16 *p_code)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->get_code != NULL);
    ret = p_lld_dev->get_code(p_lld_dev, p_code);
  }

  return ret;
}




RET_CODE uio_display(uio_device_t *p_dev, u8 *p_data, u32 len)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(FALSE == p_lld_dev->fp_en)
  {
    return ret;
  }

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->display != NULL);
    ret = p_lld_dev->display(p_lld_dev, p_data, len);
  }

  return ret;
}

void uio_tmp_ctl(uio_device_t *p_dev, u8 cmd)
{ 
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->fp_start_work != NULL);
    MT_ASSERT(p_lld_dev->fp_stop_work != NULL);
    switch(cmd)
    {
    case FP_START_WORK:
          p_lld_dev->fp_start_work();
          break;
          
    case FP_STOP_WORK:
          p_lld_dev->fp_stop_work();
          break;
          
    default:
          return;
    }
  }
  return;
}


RET_CODE uio_set_rpt_key(uio_device_t *p_dev, uio_type_t uio, 
u8 *p_keycodes, u32 len,u8 ir_index)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->set_rpt_key != NULL);
    ret = p_lld_dev->set_rpt_key(p_lld_dev, uio, p_keycodes, len, ir_index);
  }

  return ret;
}


void uio_clr_rpt_key(uio_device_t *p_dev, uio_type_t uio)
{
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->clr_rpt_key != NULL);
    p_lld_dev->clr_rpt_key(p_lld_dev, uio);
  }
}


void uio_clear_key(uio_device_t *p_dev)
{
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->clear_key != NULL);
    p_lld_dev->clear_key(p_lld_dev);
  }
}


RET_CODE uio_set_user_code(uio_device_t *p_dev,u8 num,u16 *p_array)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->set_sw_usrcode != NULL);
    ret = p_lld_dev->set_sw_usrcode(p_lld_dev, num, p_array);
  }
  return ret;
}

void uio_set_bitmap(uio_device_t *p_dev, led_bitmap_t *p_map, u32 size)
{
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    p_lld_dev->fp_set_bitmap(p_lld_dev, p_map, size);
  }
}

RET_CODE uio_fp_io_ctrl(uio_device_t *p_dev, u32 cmd, u32 param)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->fp_io_ctrl != NULL);
    ret = p_lld_dev->fp_io_ctrl(p_lld_dev, cmd, param);
  }

  return ret;
}

RET_CODE uio_fp_start_work(uio_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->fp_io_ctrl != NULL);
    p_lld_dev->fp_start_work();
  }

  return ret;
}

RET_CODE uio_fp_stop_work(uio_device_t *p_dev)
{
  RET_CODE ret = ERR_NOFEATURE;
  lld_uio_t *p_lld_dev = (lld_uio_t *)p_dev->p_priv;

  if(drv_check_cond(p_dev) == SUCCESS)
  {
    MT_ASSERT(p_lld_dev->fp_io_ctrl != NULL);
    p_lld_dev->fp_stop_work();
  }

  return ret;
}