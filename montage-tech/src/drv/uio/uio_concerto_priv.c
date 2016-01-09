/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_fifo.h"
#include "mtos_mem.h"
#include "drv_dev.h"
#include "../drvbase/drv_dev_priv.h"
#include "uio.h"
#include "uio_priv.h"

//#define UIO_DBG
#ifdef UIO_DBG
#define UIOPRINTF(x) (OS_PRINTF x)
#else
#define UIOPRINTF(x) do {} while(0)
#endif

/********Function to be called by IRDA/FP driver************/
BOOL uio_check_rpt_key_concerto(uio_type_t uio, u8 code, u8 ir_index)
{
  u32 start = 0, len = 0, i = 0;
  uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                        DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

  if(UIO_IRDA == uio)
  {
    if(ir_index >= IRDA_MAX_USER)
    {
      return FALSE;
    }
    start = ir_index * UIO_IRDA_MAX_PRTKEY;
    len = UIO_IRDA_MAX_PRTKEY;
  }
  else
  {
    start = UIO_IRDA_MAX_PRTKEY * IRDA_MAX_USER;
    len = UIO_FP_MAX_PRTKEY;
  }

  for(i = 0; i < len; i ++)
  {
    if(p_priv->rpt_key[start + i] == code)
    {
      return TRUE;
    }
  }

  return FALSE;
}


/*Common interface for Irda/Frontpanel*/
static RET_CODE uio_priv_get_code(lld_uio_t *p_lld, u16 *p_code)
{
  BOOL ret = TRUE;
  u32 sr = 0;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

  //OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
  mtos_critical_enter(&sr);
  ret = mtos_fifo_get(&p_priv->fifo, p_code);
  mtos_critical_exit(sr);
  //OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);

  if(TRUE == ret)
  {
    return SUCCESS;
  }

  return ERR_FAILURE;
}


static void uio_priv_clear_key(lld_uio_t *p_lld)
{
  u32 sr = 0;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

  mtos_critical_enter(&sr);
  mtos_fifo_flush(&p_priv->fifo);
  mtos_critical_exit(sr);
}

static RET_CODE uio_priv_set_rpt_key(lld_uio_t *p_lld,
                                     uio_type_t uio,
                                     u8 *p_keycodes,
                                     u32 len,
                                     u8 ir_index)
{
  u32 l = 0, i = 0, start_idx = 0;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

  if(NULL == p_keycodes)
  {
    return ERR_PARAM;
  }
  if(UIO_IRDA == uio)
  {
    if(ir_index >= IRDA_MAX_USER)
    {
      return ERR_PARAM;
    }

    l = (len > UIO_IRDA_MAX_PRTKEY) ? UIO_IRDA_MAX_PRTKEY : len;
    start_idx = ir_index * UIO_IRDA_MAX_PRTKEY;

    for(i = 0; i < l; i ++)
    {
      p_priv->rpt_key[i + start_idx] = p_keycodes[i];
    }
  }
  else if(UIO_FRONTPANEL == uio)
  {
    l = (len > UIO_FP_MAX_PRTKEY) ? UIO_FP_MAX_PRTKEY : len;
    for(i = 0; i < l; i ++)
    {
      p_priv->rpt_key[UIO_IRDA_MAX_PRTKEY * IRDA_MAX_USER + i] = p_keycodes[i];
    }
  }

  return SUCCESS;
}


static void uio_priv_clr_rpt_key(lld_uio_t *p_lld, uio_type_t uio)
{
  u32 i = 0;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

  if(UIO_IRDA == uio)
  {
    for(i = 0; i < UIO_IRDA_MAX_PRTKEY * IRDA_MAX_USER; i ++)
    {
      p_priv->rpt_key[i] = 0xFF;
    }
  }
  else if(UIO_FRONTPANEL == uio)
  {
    for(i = 0; i < UIO_FP_MAX_PRTKEY; i ++)
    {
      p_priv->rpt_key[UIO_IRDA_MAX_PRTKEY * IRDA_MAX_USER + i] = 0xFF;
    }
  }
}


static RET_CODE uio_priv_open(lld_uio_t *p_lld, uio_cfg_t *p_cfg)
{
  RET_CODE ret = ERR_FAILURE;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

  p_priv->fifo.p_buffer = p_priv->key_buf;
  p_priv->fifo.m_size = UIO_KEY_FIFO_SIZE;
  p_priv->fifo.m_overlay = TRUE;
  mtos_fifo_flush(&p_priv->fifo);

  uio_priv_clr_rpt_key(p_lld, UIO_IRDA);
  uio_priv_clr_rpt_key(p_lld, UIO_FRONTPANEL);

  if(NULL != p_cfg)
  {
    if(NULL != p_cfg->p_fpcfg)
    {
      p_lld->fp_en = TRUE;
      if(NULL != p_lld->fp_open)
      {
        ret = p_lld->fp_open(p_lld, p_cfg->p_fpcfg);
      }
    }

    if(NULL != p_cfg->p_ircfg)
    {
      p_lld->irda_en = TRUE;

      if(NULL != p_lld->irda_open)
      {
        ret = p_lld->irda_open(p_lld, p_cfg->p_ircfg);
      }
    }
  }

  return ret;
}


static RET_CODE uio_priv_close(lld_uio_t *p_lld)
{
  RET_CODE ret = ERR_FAILURE;

  if((NULL != p_lld->fp_stop) && (TRUE == p_lld->fp_en))
  {
    ret = p_lld->fp_stop(p_lld);
  }
  if((NULL != p_lld->irda_stop) && (TRUE == p_lld->irda_en))
  {
    ret = p_lld->irda_stop(p_lld);
  }
  return ret;
}


static void uio_detach(lld_uio_t *p_lld)
{
}


static RET_CODE uio_priv_io_ctrl(lld_uio_t *p_lld, u32 cmd, u32 param)
{
  RET_CODE ret = ERR_FAILURE;
  switch(cmd)
  {
    case UIO_IR_SET_HW_USERCODE:
    case UIO_IR_SET_HW_KEYCODE:
    case UIO_IR_GET_USERCODE:
    case UIO_IR_DISABLE_SW_USERCODE:
    case UIO_IR_PRINT_SW_USERCODE:
    case UIO_IR_SET_WAVEFILT:
    case UIO_IR_CHECK_KEY:
    case UIO_IR_CLEAR_KEY_FLAG:
    case UIO_IR_CALLBACK_REGIST:
      if(NULL != p_lld->irda_io_ctrl)
      {
        ret = p_lld->irda_io_ctrl(p_lld,cmd, param);
      }
      break;
    case UIO_FP_SET_LOCK_LBD:
    case UIO_FP_SET_POWER_LBD:
    case UIO_FP_SET_COLON:
    case UIO_FP_POWER_KEY_PRESS:
    case UIO_FP_IR_KEY_ONE:
    case UIO_FP_IR_KEY_TWO:
    case UIO_FP_TIME_OUTO:
    case UIO_FP_GET_CH_PLUS_LBD:
    case UIO_FP_GET_CH_SUB_LBD:
    case UIO_FP_KEY_ENABLE:
    case UIO_FP_SET_SCAN_FREQ:
    case UIO_FP_GET_SCAN_FREQ:
    case UIO_FP_NO_INT_RUN:
    case UIO_FP_SET_SIGNAL:
    case UIO_FP_SET_EMAIL_LBD:
    case UIO_FP_SET_IR_LBD:
    case UIO_FP_GET_DISP_BITMAP:
    case UIO_FP_CALLBACK_REGIST:
      if(NULL != p_lld->fp_io_ctrl)
      {
        ret = p_lld->fp_io_ctrl(p_lld, cmd, param);
      }
      break;
    default:
      break;
  }
  return ret;
}

extern void panel_concerto_attach(lld_uio_t *p_lld);
void irda_attach(lld_uio_t * p_lld);

/*
  this attach function just for wizzards and magic platform use;
  other platform add uio, please add new file see uio_wa_priv.c
  20130823

*/
RET_CODE uio_concerto_attach(char *p_name)
{
  uio_device_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  lld_uio_t *p_lld = NULL;
  uio_priv_t *p_uio_priv = NULL;
  p_dev = dev_allocate(p_name, SYS_DEV_TYPE_UIO, sizeof(uio_device_t),
                      sizeof(lld_uio_t));
  if(NULL == p_dev)
  {
    return ERR_FAILURE;
  }
  p_uio_priv = (uio_priv_t *)mtos_malloc(sizeof(uio_priv_t));
  if(NULL == p_uio_priv)
  {
    return ERR_FAILURE;
  }

  p_base = (device_base_t *)(p_dev->p_base);
  p_base->open = (RET_CODE (*)(void *, void *))uio_priv_open;
  p_base->close = (RET_CODE (*)(void *))uio_priv_close;
  p_base->detach = (void (*)(void *))uio_detach;
  p_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))uio_priv_io_ctrl;

  p_lld = (lld_uio_t *)p_dev->p_priv;
  p_lld->get_code = uio_priv_get_code;
  p_lld->set_rpt_key = uio_priv_set_rpt_key;
  p_lld->clr_rpt_key = uio_priv_clr_rpt_key;
  p_lld->clear_key = uio_priv_clear_key;
  p_lld->set_sw_usrcode = NULL;
  p_lld->irda_io_ctrl = NULL;
  p_lld->irda_open = NULL;
  p_lld->irda_stop = NULL;
  p_lld->irda_en = FALSE;
  p_lld->p_irda = NULL;
  p_lld->fp_open = NULL;
  p_lld->fp_stop = NULL;
  p_lld->display = NULL;
  p_lld->fp_io_ctrl = NULL;
  p_lld->fp_en = FALSE;
  p_lld->p_fp = NULL;
  p_lld->p_priv = p_uio_priv;

  irda_attach(p_lld); //attach irda

  if(0 == memcmp(p_name, "concerto", 8))
  {
    panel_concerto_attach(p_lld);
  }

  return SUCCESS;
}


