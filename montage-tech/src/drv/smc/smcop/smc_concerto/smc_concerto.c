/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>

#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_int.h"
#include "mtos_fifo.h"
#include "mtos_event.h"
#include "hal_otp.h"
#include "hal_base.h"
#include "hal_concerto_irq.h"
#include "sys_regs_concerto.h"

#include "drv_dev.h"
#include "../../../drvbase/drv_svc.h"
#include "../../../drvbase/drv_dev_priv.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "../smc_op_priv.h"

#include "drv_smc.h"
#include "smc_concerto.h"

static smc_priv_t *p_mtsmc0_priv = NULL;
static smc_priv_t *p_mtsmc1_priv = NULL;

extern ERRORCODE_T drv_smartcarddeinit(drv_smartcardhandle_t handle);
extern ERRORCODE_T drv_smartcardgetid(drv_smartcardhandle_t handle, smc_id_t *p_smc_id);

static void SC0_CardInsert(void)
{
  smc_priv_t *p_priv = p_mtsmc0_priv;

  OS_PRINTF("\nCard 0 Insert\n");
  
  /* trigger driver service to handle this event */
  drvsvc_event_set(p_priv->p_svc, SMC7816_INT_CARD0_IN);
}

static void SC0_CardRemove(void)
{
  smc_priv_t *p_priv = p_mtsmc0_priv;

  OS_PRINTF("\nCard 0 Remove\n");
  
  /* trigger driver service to handle this event */
  drvsvc_event_set(p_priv->p_svc, SMC7816_INT_CARD0_OUT);
}

static void SC1_CardInsert(void)
{
  smc_priv_t *p_priv = p_mtsmc1_priv;

  OS_PRINTF("\nCard 1 Insert\n");
  
  /* trigger driver service to handle this event */
  drvsvc_event_set(p_priv->p_svc, SMC7816_INT_CARD1_IN);
}

static void SC1_CardRemove(void)
{
  smc_priv_t *p_priv = p_mtsmc1_priv;

  OS_PRINTF("\nCard 1 Remove\n");
  
  /* trigger driver service to handle this event */
  drvsvc_event_set(p_priv->p_svc, SMC7816_INT_CARD1_OUT);
}

static RET_CODE smc_concerto_service(void *p_param, u32 event)
{
  smc_id_t smc_id;
  drv_smartcardstatus_t status;
  ERRORCODE_T err = ERR_SMC_NO_ERR;

  smc_priv_t *p_priv = (smc_priv_t *)p_param;
  scard_notify_t ntf_para = {0};

  OS_PRINTF("smc_service event 0x%x \n", event);

  /* get smc id */
  drv_smartcardgetid(p_priv->SCHandler, &smc_id);
  if (smc_id == SMC0_ID)
  {
    switch(event &(SMC7816_INT_CARD0_IN|SMC7816_INT_CARD0_OUT))
    {
      case SMC7816_INT_CARD0_IN:
        ntf_para.card_stat = SMC_CARD_INSERT;
        break;

      case SMC7816_INT_CARD0_OUT:
        ntf_para.card_stat = SMC_CARD_REMOVE;
        break;      

      case (SMC7816_INT_CARD0_IN | SMC7816_INT_CARD0_OUT):
        /* get the init status */
        err = drv_smartcardgetstatus(p_priv->SCHandler, &status);
        if(err == ERR_SMC_NO_ERR)
        {
          if(status.insert_status == DRV_SMART_STATUS_INSERT)
          {
            ntf_para.card_stat = SMC_CARD_INSERT;
          }
          if(status.insert_status == DRV_SMART_STATUS_REMOVE)
          {
            ntf_para.card_stat = SMC_CARD_REMOVE;
          }
        }
        break;

      default :
          return SUCCESS;
    }
  }
  else if (smc_id == SMC1_ID)
  {
    switch(event &(SMC7816_INT_CARD1_IN|SMC7816_INT_CARD1_OUT))
    {
      case SMC7816_INT_CARD1_IN:
        ntf_para.card_stat = SMC_CARD_INSERT;
        break;

      case SMC7816_INT_CARD1_OUT:
        ntf_para.card_stat = SMC_CARD_REMOVE;
        break;      

      case (SMC7816_INT_CARD1_IN | SMC7816_INT_CARD1_OUT):
        /* get the init status */
        err = drv_smartcardgetstatus(p_priv->SCHandler, &status);
        if(err == ERR_SMC_NO_ERR)
        {
          if(status.insert_status == DRV_SMART_STATUS_INSERT)
          {
            ntf_para.card_stat = SMC_CARD_INSERT;
          }
          if(status.insert_status == DRV_SMART_STATUS_REMOVE)
          {
            ntf_para.card_stat = SMC_CARD_REMOVE;
          }
        }
        break;

      default :
          return SUCCESS;
    }
  }

  if(p_priv->notify)
  {
    p_priv->notify(&ntf_para);
  }

  return SUCCESS;
}

static RET_CODE smc_concerto_open(lld_smc_t *p_lld, void *cfg)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;
  smc_priv_t *p_dev_priv = NULL;
  drvsvc_handle_t *p_smc_svc = NULL;
  u32 *p_stack = NULL;
  u32 drv_version = 0xFF;
  scard_open_t *p_open = (scard_open_t *)cfg;

  drv_smartcardinitparam_t init_params;

  //OS_PRINTF("\nsmc_concerto_open\tp_dev=0x%x", p_lld);

  if (SUCCESS == hal_otp_get_property(CMD_GET_DRIVER_VERSION, &drv_version))
  {
    if (drv_version)
    {
      return ERR_NOFEATURE;
    }
  }

  if (p_open->smc_id == SMC0_ID)
  {
      // SMC device 0
      init_params.smc_id = SMC0_ID;
      init_params.SmartInsert = SC0_CardInsert;
      init_params.SmartRemove = SC0_CardRemove;
  }
  else if (p_open->smc_id == SMC1_ID)
  {
      // SMC device 1
      init_params.smc_id = SMC1_ID;
      init_params.SmartInsert = SC1_CardInsert;
      init_params.SmartRemove = SC1_CardRemove;
  }
  else
  {
      return ERR_FAILURE;
  }

  MT_ASSERT(NULL!=p_open);

  init_params.phyclk = p_open->phyclk;
  init_params.convt_en = p_open->convention;
  init_params.convt_value = p_open->convt_value;
  init_params.detect_pin_pol = p_open->detect_pin_pol;
  init_params.iopin_mode = p_open->iopin_mode;
  init_params.rstpin_mode = p_open->rstpin_mode;
  init_params.clkpin_mode = p_open->clkpin_mode;
  init_params.vcc_enable_pol = p_open->vcc_enable_pol;

  p_dev_priv = p_lld->p_priv = mtos_malloc(sizeof(smc_priv_t));
  MT_ASSERT(NULL != p_dev_priv);
  memset(p_lld->p_priv, 0, sizeof(smc_priv_t));
  if (p_open->smc_id == SMC0_ID)
  {
      p_mtsmc0_priv = p_dev_priv;
  }
  else if (p_open->smc_id == SMC1_ID)
  {
      p_mtsmc1_priv = p_dev_priv;
  }

  p_dev_priv->scard_cfg.read_timeout = p_open->read_timeout & 0xFFFF;
  if (p_dev_priv->scard_cfg.read_timeout < 10)
  {
    p_dev_priv->scard_cfg.read_timeout = 500;
  }

  err = drv_smartcardinit(&init_params, &p_dev_priv->SCHandler);
  if(err != ERR_SMC_NO_ERR)
  {
    OS_PRINTF("\nopen err=0x%x\n", err);
    return err;
  }

  /* create driver service to handle the ISR */
  p_stack =  (u32 *)mtos_malloc(p_open->smc_op_stksize);
  MT_ASSERT(p_stack != NULL);
  p_dev_priv->p_stack = p_stack;

  p_smc_svc = drvsvc_create(p_open->smc_op_pri, p_stack, p_open->smc_op_stksize, 4);
  MT_ASSERT(NULL != p_smc_svc);
  p_dev_priv->p_svc = p_smc_svc;

  drvsvc_nod_insert(p_smc_svc, smc_concerto_service, 
    (void *)p_dev_priv, 
    (SMC7816_INT_MASK), 
    MTOS_WAIT_FOREVER);

  return SUCCESS;
}

static RET_CODE smc_concerto_close(lld_smc_t *p_lld)
{
  smc_priv_t *p_dev_priv = (smc_priv_t *)p_lld->p_priv;
  
  /* free service resource */
  drvsvc_destroy(p_dev_priv->p_svc);
  mtos_free(p_dev_priv->p_stack);
  drv_smartcarddeinit(p_dev_priv->SCHandler);

  /* free private resource */
  mtos_free(p_dev_priv);

  return SUCCESS;
}

static void smc_concerto_detach(lld_smc_t *p_lld)
{
}

static RET_CODE smc_concerto_io_ctrl(lld_smc_t *p_lld, u32 cmd, u32 param)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;
  drv_smartcardstatus_t status;  
  smc_priv_t *p_priv = (smc_priv_t *)p_lld->p_priv;
  
  switch(cmd)
  {
    case SMC_IO_CTRL_GET_STATUS:
      err = drv_smartcardgetstatus(p_priv->SCHandler,&status);
      if(err == ERR_SMC_NO_ERR)
      {
        *((u32 *)param) = status.insert_status;
      }
      else
        return ERR_FAILURE;
      break;

    case SMC_IO_CTRL_GET_PROTOCOL:
       err = drv_smartcardgetstatus(p_priv->SCHandler,&status);
       if(err == ERR_SMC_NO_ERR)
       {
         *((u32 *)param) = status.protocol;
       }
       else
         return ERR_FAILURE;     
      break;

    default:
      break;
  }
  
  return SUCCESS;
}

static RET_CODE smc_concerto_active(lld_smc_t *p_lld, scard_atr_desc_t *p_atr)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;
  u32 atr_len = 0;

  smc_priv_t *p_priv = (smc_priv_t *)p_lld->p_priv;
  mtos_task_delay_ms(10);

  err = drv_smartcard_reset(p_priv->SCHandler, p_atr->p_buf, &atr_len);
  if(err != ERR_SMC_NO_ERR)
  {
    OS_PRINTF("\ndrv_smartcard_reset err=0x%x\n", err);
    return err;
  }

  p_atr->atr_len = atr_len;

  return SUCCESS;
}

static RET_CODE smc_concerto_deactive(lld_smc_t *p_lld)
{
  return SUCCESS;
}

static RET_CODE smc_concerto_get_config(lld_smc_t *p_lld, scard_config_t *p_cfg)
{
  smc_priv_t *p_priv = (smc_priv_t *)p_lld->p_priv;

  memcpy(p_cfg,&p_priv->scard_cfg,sizeof(scard_config_t));

  return SUCCESS;
}

static RET_CODE smc_concerto_set_config(lld_smc_t *p_lld, scard_config_t *p_cfg)
{
  smc_priv_t *p_priv = (smc_priv_t *)p_lld->p_priv;
  u32 WorkETU = 0;
  memcpy(&p_priv->scard_cfg,p_cfg,sizeof(scard_config_t));  
  WorkETU = p_priv->scard_cfg.clk_f_rate / p_priv->scard_cfg.bit_d_rate;
  drv_smartcardsetetu(p_priv->SCHandler, WorkETU);
  return SUCCESS;
}

static RET_CODE smc_concerto_reset(lld_smc_t *p_lld)
{
  return SUCCESS;
}

static RET_CODE smc_concerto_transfer(lld_smc_t *p_lld, scard_opt_desc_t *p_rwopt)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;
  u32 bytes_write_ok = 0;
  scard_term_check_arg_t term_para;

  smc_priv_t *p_priv = (smc_priv_t *)p_lld->p_priv;

  mtos_task_delay_ms(4);
  err = drv_smartcardtransfer(p_priv->SCHandler, p_rwopt->p_buf_out, p_rwopt->bytes_out,
      &bytes_write_ok,p_rwopt->p_buf_in, p_rwopt->bytes_in_target, &p_rwopt->bytes_in_actual);

  if(err != ERR_SMC_NO_ERR)
    return err;

  if(p_priv->term_check)
  {
    term_para.bytes_in_actual = p_rwopt->bytes_in_actual;
    if(TRUE == p_priv->term_check(&term_para))
    {
      return SUCCESS;
    }
  }

  return SUCCESS;
}

RET_CODE smc_concerto_register_notify(lld_smc_t *p_lld, smc_op_notify func)
{
  smc_id_t smc_id = 0;
  u32 smc_event_insert = 0;
  u32 smc_event_remove = 0;
  drv_smartcardstatus_t status;
  ERRORCODE_T err = ERR_SMC_NO_ERR;

  smc_priv_t *p_priv = (smc_priv_t *)(p_lld->p_priv);

  p_priv->notify = func;

  /* get smc id */
  drv_smartcardgetid(p_priv->SCHandler, &smc_id);
  if (smc_id == SMC0_ID)
  {
      smc_event_insert = SMC7816_INT_CARD0_IN;
      smc_event_remove = SMC7816_INT_CARD0_OUT;
  }
  else if (smc_id == SMC1_ID)
  {
      smc_event_insert = SMC7816_INT_CARD1_IN;
      smc_event_remove = SMC7816_INT_CARD1_OUT;
  }

  /* get the init status */
  err = drv_smartcardgetstatus(p_priv->SCHandler, &status);
  if(err == ERR_SMC_NO_ERR)
  {
      if(status.insert_status == DRV_SMART_STATUS_INSERT)
      {
        /* card is in, send event to service to notify up layer */
        drvsvc_event_set(p_priv->p_svc, smc_event_insert);
      }
  
      if(status.insert_status == DRV_SMART_STATUS_REMOVE)
      {
        /* card is not present, send event to service to notify up layer */
        drvsvc_event_set(p_priv->p_svc, smc_event_remove);
      }  
  }

  return SUCCESS;
}

RET_CODE smc_concerto_register_term_check(lld_smc_t *p_lld, scard_term_check_cfg_t *p_config)
{
  smc_priv_t *p_dev = (smc_priv_t *)(p_lld->p_priv);
  p_dev->term_check = p_config->func;
  return SUCCESS;
}

static RET_CODE smc_concerto_read(lld_smc_t *p_lld, u8 *buffer, u32 size, u32 *actlen)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;

  smc_priv_t *p_priv = (smc_priv_t *)(p_lld->p_priv);

  err =  drv_smartcardread(p_priv->SCHandler,buffer, size, actlen, p_priv->scard_cfg.read_timeout);

  if(err != ERR_SMC_NO_ERR)
    return err;

  return SUCCESS;
}

static RET_CODE smc_concerto_write(lld_smc_t *p_lld, u8 *buffer, u32 size, u32 *actlen)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;

  smc_priv_t *p_priv = (smc_priv_t *)(p_lld->p_priv);

  err =  drv_smartcardwrite(p_priv->SCHandler, buffer, size, actlen, 0);

  if(err != ERR_SMC_NO_ERR)
    return err;

  return SUCCESS;
}

static RET_CODE smc_concerto_atr_parse(lld_smc_t *p_lld, void *p_atr, void *p_cfg)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;

  //smc_priv_t *p_priv = (smc_priv_t *)(p_lld->p_priv);
  err = scard_pro_atr_parse(p_atr, p_cfg);

  if(err != ERR_SMC_NO_ERR)
    return err;

  return SUCCESS;
}

/*! 
Register smc deivce 
   */
RET_CODE smc_attach_concerto(char *name)
{
  drv_dev_t *p_dev = NULL;
  device_base_t *p_dev_base = NULL;
  lld_smc_t *p_lld_dev = NULL;
  /*! 
  allocate driver memory resource 
     */
  p_dev = (drv_dev_t *)dev_allocate(name, SYS_DEV_TYPE_SMC,
                                          sizeof(scard_device_t),
                                          sizeof(lld_smc_t));
  if (NULL == p_dev)
  {
    return ERR_FAILURE;
  }
  /*!  
  link base function 
      */
  p_dev_base = (device_base_t *)p_dev->p_base;
  p_dev_base->open = (RET_CODE(*) (void *, void *))smc_concerto_open;
  p_dev_base->close = (RET_CODE(*) (void *))smc_concerto_close;
  p_dev_base->detach = (void(*) (void *))smc_concerto_detach;
  p_dev_base->io_ctrl = (RET_CODE(*) (void *, u32, u32))smc_concerto_io_ctrl;

  /*! 
  attach lld driver 
     */
  p_lld_dev = (lld_smc_t *)p_dev->p_priv;
  p_lld_dev->active = smc_concerto_active;
  p_lld_dev->deactive = smc_concerto_deactive;
  p_lld_dev->get_config = smc_concerto_get_config;
  p_lld_dev->set_config = smc_concerto_set_config;
  p_lld_dev->reset = smc_concerto_reset;
  p_lld_dev->rw_transaction = smc_concerto_transfer;
  p_lld_dev->register_notify = smc_concerto_register_notify;
  p_lld_dev->register_term_check = smc_concerto_register_term_check;
  p_lld_dev->read = smc_concerto_read;
  p_lld_dev->write = smc_concerto_write;
  p_lld_dev->atr_parse = smc_concerto_atr_parse;

  return SUCCESS;
}
