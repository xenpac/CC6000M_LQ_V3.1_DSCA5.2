/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifdef DRV_SEL_SMC

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
#include "hal_base.h"

#include  "trunk/error_code.h"
#include "sys_regs_concerto.h"
#include "smc_ctl.h"
#include "drv_smc.h"
#include "smc_block.h"
#include "smc_t0.h"
#include "smc_t1.h"

#define DRVSC_DEBUG_ENABLE     0

#if DRVSC_DEBUG_ENABLE
 #define DRVSC_P(...)    OS_PRINTF(__VA_ARGS__)                           
#else
 #define DRVSC_P(...)    do{}while(0)
#endif

#define DRVSC_ERROR_ENABLE  1

#if DRVSC_ERROR_ENABLE
 #define DRVSC_E(...)    OS_PRINTF(__VA_ARGS__)                           
#else
 #define DRVSC_E(...)    do{}while(0)
#endif

#define POWER_3V
#define MAX_SC_COMMAND_LEN   512

static u8 g_SCCommand[MAX_SC_COMMAND_LEN];
extern void smc_controller_glb_rst(smc_id_t smc_id);

static ERRORCODE_T _Smc_UserLock_Create(tscdevice *pSCDev)
{
  if (mtos_sem_create(&pSCDev->UserMutex,1) != TRUE)
  {      
    return ERR_SMC_USERMUTEX_CREATE_FAIL;
  }

  return ERR_SMC_NO_ERR;
}

static ERRORCODE_T _Smc_UserLock_Get(tscdevice *pSCDev)
{
  if (mtos_sem_take(&pSCDev->UserMutex, 3000) != TRUE)
  {
    return ERR_SMC_PENDMUTEX_FAIL;
  }

  return ERR_SMC_NO_ERR;
}

static ERRORCODE_T _Smc_UserLock_Put(tscdevice *pSCDev)
{
  if (mtos_sem_give(&pSCDev->UserMutex) != TRUE)
  {
    return ERR_SMC_POSTMUTEX_FAIL;
  }

  return ERR_SMC_NO_ERR;
}

ERRORCODE_T drv_smartcardinit(const drv_smartcardinitparam_t *pinit_params,
                              drv_smartcardhandle_t *phandle)
{
  ERRORCODE_T err;
  tscdevice *pSCDev = NULL;

  if ((pinit_params == NULL) || (phandle == NULL))
  {
    return ERR_SMC_PARAM_INVALID;
  }

  pSCDev = sc_getscdevice(pinit_params->smc_id);
  if (pSCDev->Init)
  {
    return ERR_SMC_ALREADY_INIT;
  }

  err = _Smc_UserLock_Create(pSCDev);
  if (err != ERR_SMC_NO_ERR)
  {
    return err;
  }

  pSCDev->SmcId = pinit_params->smc_id;
  pSCDev->SmartInsert = pinit_params->SmartInsert;
  pSCDev->SmartRemove = pinit_params->SmartRemove;

  err = sc_intialize(pinit_params->smc_id, pinit_params->phyclk,
                      pinit_params->convt_en, pinit_params->convt_value,
                      pinit_params->detect_pin_pol, pinit_params->vcc_enable_pol,
                      pinit_params->iopin_mode, pinit_params->rstpin_mode, pinit_params->clkpin_mode);

  if (err != ERR_SMC_NO_ERR)
  {
    return err;
  }

  *phandle = (drv_smartcardhandle_t)pSCDev;

  pSCDev->Init = TRUE;

  return ERR_SMC_NO_ERR;
}

ERRORCODE_T drv_smartcarddeinit(drv_smartcardhandle_t handle)
{
  tscdevice *pSCDev = (tscdevice *)handle;
  u8 smc_id = 0;

  if (!pSCDev->Init)
  {
    return ERR_SMC_NOT_INIT;
  }
  if (!sc_checkhandle(pSCDev))
  {
    return ERR_SMC_HANDLE_INVALID;
  }

  smc_id  = pSCDev->SmcId;
  memset((tscdevice *)pSCDev, 0x0, sizeof (tscdevice));
  smc_controller_glb_rst(smc_id);

  return ERR_SMC_NO_ERR;
}

ERRORCODE_T drv_smartcardgetid(drv_smartcardhandle_t handle, smc_id_t *p_smc_id)
{
  tscdevice *pSCDev = (tscdevice *)handle;

  if (!pSCDev->Init)
  {
    return ERR_SMC_NOT_INIT;
  }

  *p_smc_id = pSCDev->SmcId;

  return ERR_SMC_NO_ERR;
}

ERRORCODE_T drv_smartcard_reset(drv_smartcardhandle_t handle,u8 answer[33],u32 *panswer_len)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;
  tscdevice *pSCDev = (tscdevice *)handle;

  if (!sc_checkhandle(pSCDev))
  {
    return ERR_SMC_HANDLE_INVALID;
  }

  if (!pSCDev->Init)
  {
    return ERR_SMC_NOT_INIT;
  }

  DRVSC_P("Before sc_getcardstatus\n");
  if (!sc_getcardstatus(pSCDev->SmcId))
  {
    DRVSC_P("IN sc_getcardstatus\n");
    return ERR_SMC_OUT;
  }
  DRVSC_P("After sc_getcardstatus\n");

  err = _Smc_UserLock_Get(pSCDev);
  if (err != ERR_SMC_NO_ERR)
  {
    return err;
  }

  err = sc_answertoreset(pSCDev->SmcId, answer, panswer_len, FALSE);
  if (err != ERR_SMC_NO_ERR)
  {
    DRVSC_P("\nsc_answertoreset err=0x%x\n", err);
  }
#if 0  /* remove the retry since active retry will be did by upper layer */
  if (err != ERR_SMC_NO_ERR)
  {
    err = sc_answertoreset(answer, panswer_len, TRUE);
  }
#endif
  _Smc_UserLock_Put(pSCDev);
    
  return err;
}

ERRORCODE_T  drv_smartcardtransfer(drv_smartcardhandle_t handle, 
                  const u8 *pcommand, u32 command_len, u32  *pnum_write_ok,
                  u8 *presponse, u32   num_to_read, u32  *pnum_read_ok)
{
    ERRORCODE_T err;
  
    tscdevice *pSCDev = (tscdevice *)handle;
  
  if (!sc_checkhandle(pSCDev))
  {
    return ERR_SMC_HANDLE_INVALID;
  }

  if ((pcommand == NULL) || (pnum_write_ok == NULL) ||
    (presponse == NULL) || (pnum_read_ok == NULL) ||
    (command_len > MAX_SC_COMMAND_LEN))
  {
    return ERR_SMC_PARAM_INVALID;
  }

    if (!sc_getcardstatus(pSCDev->SmcId))
    {
    return ERR_SMC_OUT;
    }

  if (!pSCDev->Init)
  {
    return ERR_SMC_NOT_INIT;
  }
  
  if (pSCDev->WorkingType == SC_UNSUPPORT_PTOTOCOL)
    {
        return ERR_SMC_UNSUPPORT_PTOTOCOL;
    }

  if (_Smc_UserLock_Get(pSCDev) != ERR_SMC_NO_ERR)
  {
    return ERR_SMC_PENDMUTEX_FAIL;
  }
  
//  DRVSC_P("\r\nGet the SC");

  memcpy(g_SCCommand, pcommand, command_len);
  
    sc_flush(pSCDev->SmcId);

  switch (pSCDev->WorkingType)
  {
    case SC_T0_PROTOCOL:
    {
      err = sc_t0_transfer(pSCDev, g_SCCommand, command_len, pnum_write_ok,
                  presponse, num_to_read, pnum_read_ok);
      if (err != ERR_SMC_NO_ERR)
      {
        DRVSC_E("\r\nT0_Transfer Error");
        goto EXT;
      }
      
    }
    break;

    case SC_T1_PROTOCOL: 
    {
      err = sc_t1_transfer(pSCDev, g_SCCommand, command_len, pnum_write_ok,
                presponse, num_to_read, pnum_read_ok);
      if (err != ERR_SMC_NO_ERR)
      {
        DRVSC_E("\r\n T1_Transfer Error");
        goto EXT;
      }
    }
    break;

    case SC_T14_PROTOCOL:
    {
      err = sc_t14_transfer(pSCDev, g_SCCommand, command_len, pnum_write_ok,
                presponse, num_to_read, pnum_read_ok);
      if (err != ERR_SMC_NO_ERR)
      {
        DRVSC_E("\r\n T14 Transfer Error %x\n",err);
        goto EXT;
      }
    }
    break;
    
    default:
    {
      err = ERR_SMC_UNSUPPORT_PTOTOCOL;
      goto EXT;
    }
  }
  
    err = ERR_SMC_NO_ERR;    

EXT:
    if (err != ERR_SMC_NO_ERR)
    {
      DRVSC_E("\r\n Smc Run Fail!");
    }

    _Smc_UserLock_Put(pSCDev);
    
    return err;

}


ERRORCODE_T  drv_smartcardwrite(drv_smartcardhandle_t Handle, 
                IN const U8 *pBuf, 
                U32 NumberToWrite, 
                U32  *pNumberWritten_p, 
                U32 Timeout)

{
    ERRORCODE_T err;
  
    tscdevice *pSCDev = (tscdevice *)Handle;
  
  if (!sc_checkhandle(pSCDev))
  {
    return ERR_SMC_HANDLE_INVALID;
  }

  if ((pBuf == NULL) 
    || (pNumberWritten_p == NULL) 
    || (NumberToWrite == 0) 
    || (NumberToWrite > MAX_SC_COMMAND_LEN))
  {
    return ERR_SMC_PARAM_INVALID;
  }

    if (!sc_getcardstatus(pSCDev->SmcId))
    {
    return ERR_SMC_OUT;
    }

  if (!pSCDev->Init)
  {
    return ERR_SMC_NOT_INIT;
  }
  
  if (pSCDev->WorkingType == SC_UNSUPPORT_PTOTOCOL)
  {
      return ERR_SMC_UNSUPPORT_PTOTOCOL;
  }

  if (_Smc_UserLock_Get(pSCDev) != ERR_SMC_NO_ERR)
  {
    return ERR_SMC_PENDMUTEX_FAIL;
  }
  
  memcpy(g_SCCommand, pBuf, NumberToWrite);

  switch (pSCDev->WorkingType)
  {
    case SC_T0_PROTOCOL:
    case SC_T1_PROTOCOL: 
    case SC_T14_PROTOCOL:
      err =  sc_tx(pSCDev->SmcId, g_SCCommand, NumberToWrite);
      if (err != ERR_SMC_NO_ERR)
      {
        DRVSC_E("\r\nSC_Tx Error");
        goto EXT;
      }
      else
      {
        *pNumberWritten_p = NumberToWrite;
      }

    break;
    default:
    {
      err = ERR_SMC_UNSUPPORT_PTOTOCOL;
      goto EXT;
    }
  }
  
    err = ERR_SMC_NO_ERR;    

EXT:
    if (err != ERR_SMC_NO_ERR)
    {
    DRVSC_E("\r\n Smc Run Fail!");
    }

    _Smc_UserLock_Put(pSCDev);
    
    return err;

}


ERRORCODE_T  drv_smartcardread(drv_smartcardhandle_t Handle,
            OUT U8 *pBuf, 
            U32 NumberToRead, 
            U32  *pNumberRead_p, 
            U32 Timeout)
{
  ERRORCODE_T err;
  
      tscdevice *pSCDev = (tscdevice *)Handle;
  
  if (!sc_checkhandle(pSCDev))
  {
    return ERR_SMC_HANDLE_INVALID;
  }

  if ((pBuf == NULL) 
    || (pNumberRead_p == NULL))
  {
    return ERR_SMC_PARAM_INVALID;
  }

    if (!sc_getcardstatus(pSCDev->SmcId))
    {
    return ERR_SMC_OUT;
    }

  if (!pSCDev->Init)
  {
    return ERR_SMC_NOT_INIT;
  }
  
  if (pSCDev->WorkingType == SC_UNSUPPORT_PTOTOCOL)
  {
      return ERR_SMC_UNSUPPORT_PTOTOCOL;
  }

  if (_Smc_UserLock_Get(pSCDev) != ERR_SMC_NO_ERR)
  {
    return ERR_SMC_PENDMUTEX_FAIL;
  }

  switch (pSCDev->WorkingType)
  {
    case SC_T0_PROTOCOL:
    case SC_T1_PROTOCOL: 
    case SC_T14_PROTOCOL:
      err =  sc_rx_intimeout(pSCDev->SmcId, pBuf, NumberToRead, pNumberRead_p, Timeout);
      if (err != ERR_SMC_NO_ERR)
      {
        DRVSC_E("\r\nSC_Rx Error");
        goto EXT;
      }
    break;
    
    default:
    {
      err = ERR_SMC_UNSUPPORT_PTOTOCOL;
      goto EXT;
    }
  }

EXT:  
    if (err != ERR_SMC_NO_ERR)
    {
    DRVSC_E("\r\n Smc Run Fail!");
    }
  
    _Smc_UserLock_Put(pSCDev);
    
    return err;
}

ERRORCODE_T  drv_smartcardgetstatus(drv_smartcardhandle_t handle,drv_smartcardstatus_t  *pstatus)
{
    tscdevice *pSCDev = (tscdevice *)handle;
  
  if (!sc_checkhandle(pSCDev))
  {
    return ERR_SMC_HANDLE_INVALID;
  }

  if (pstatus == NULL)
  {
    return ERR_SMC_PARAM_INVALID;
  }

  /* Set Caller's Parameters */
  if (sc_getcardstatus(pSCDev->SmcId))
  {
    pstatus->insert_status = DRV_SMART_STATUS_INSERT;
  }
  else
  {
    pstatus->insert_status = DRV_SMART_STATUS_REMOVE;
  }

  switch (pSCDev->WorkingType)
  {
    case SC_T0_PROTOCOL:
      pstatus->protocol = T0_PROTOCOL;
      break;
    case SC_T1_PROTOCOL:
      pstatus->protocol = T1_PROTOCOL;
      break;
    case SC_T14_PROTOCOL:
      pstatus->protocol = T14_PROTOCOL;
      break;
    default:
      pstatus->protocol = UNSUPPORTED_PROTOCOL;
      break;
  }

  return ERR_SMC_NO_ERR;
  
}

void util_print_buffer(u8 * pAddr, u32 len, s8 * pname)
{
  u32 i;
  len = len > 100 ? 100 : len;

  DRVSC_P("\r\n%s = ", pname);
  for(i = 0; i < len; i++)
  {
    DRVSC_P("%02x ", pAddr[i]);
  }
}

/*
This function is added for test the smc circle trouble
*/
int DRV_SmartCardCircleCheck(void)
{
  return 0;
}

void drv_smartcardsetetu(drv_smartcardhandle_t handle, u32 WorkETU)
{
    tscdevice *pSCDev = (tscdevice *)handle;

    smc_set_etu(pSCDev->SmcId, WorkETU);
}
#endif //#ifdef DRV_SEL_SMC
