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

#include "smc_ctl.h"
#include "drv_smc.h"
#include "smc_block.h"
#include "smc_t0.h"

#define SCT0_DEBUG_ENABLE  0
#if  SCT0_DEBUG_ENABLE
#define  SCT0_P(...)      OS_PRINTF(__VA_ARGS__)
#else
#define  SCT0_P(...)
#endif

#define SCT0_ERROR_ENABLE  0
#if  SCT0_ERROR_ENABLE
#define  SCT0_E(...)      do { \
            OS_PRINTF(__VA_ARGS__);\
            OS_PRINTF("Fun:%s line:%d", __FUNCTION__, __LINE__);\
          }while(0)
#else
#define  SCT0_E(...)
#endif

static ERRORCODE_T T0_Procedure(tscdevice *pSCDev,
                 u8  INS,
                             u32  WriteSize,
                             u32  ReadSize,
                             u8 *pBuf,
                             u8 *pSize,
                             u32 *pNextWriteSize,
                             u32 *pNextReadSize);

static ERRORCODE_T T0_CheckTransfer(const u8 *Command_p);

/*********************************************************************
 * check T0 command head is right
 ********************************************************************/
static ERRORCODE_T T0_CheckTransfer(const u8 *Command_p)
{
    ERRORCODE_T err = ERR_SMC_NO_ERR;
    /* Check the CLA and INS bytes are valid */
    if (Command_p[T0_CLA_OFFSET] != (u8) 0xFF)
    {
        if (((Command_p[T0_INS_OFFSET] & 0xF0) != 0x60)
          && ((Command_p[T0_INS_OFFSET] & 0xF0) != 0x90))
        {      /* This is a valid command */
            err = ERR_SMC_NO_ERR;
        }
        else   /* INS is invalid */
        {
            err = ERR_SMC_T0CMD_INS;
        }
    }
    else       /* CLA is invalid */
    {
        err = ERR_SMC_T0CMD_CLA;
    }

    return err;

}/* SMART_T0_CheckTransfer()  */

/***************************************************************
 * process T0 protocol proedure byte
 **************************************************************/
void smc_delay_us(int t)
{
  int i = 0;
  int cnt = t * 8;

  for(i = 0;i < cnt;i++)
  {
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  }

}

ERRORCODE_T sc_t0_transfer(tscdevice * pSCDev,
              u8 *pcommand, u32 command_len, u32 *pnum_write_ok,
              u8 *presponse, u32 num_to_read, u32 *pnum_read_ok)
{

  ERRORCODE_T err = ERR_SMC_NO_ERR;
    u32 NextWriteSize;
  u8 INS;
  u8 proc_buf[2];
  u8 proc_size;
  u32 i;
  u32 NextReadSize;
  u32 TotalRead;

  err = T0_CheckTransfer(pcommand);
  if (err != ERR_SMC_NO_ERR)
  {
    return err;
  }

    if (command_len == T0_CMD_LENGTH)
    {
        num_to_read = (u32)pcommand[T0_P3_OFFSET];
    }
    else
    {
        num_to_read = 0;
    }

  *pnum_write_ok = 0;
    *pnum_read_ok = 0;
    INS = pcommand[T0_INS_OFFSET];

    NextWriteSize = T0_CMD_LENGTH;

  /* Should be updated by procedure bytes */
      NextReadSize = 0;

    while ((command_len > 0) && (err == ERR_SMC_NO_ERR) && (NextWriteSize > 0))
    {
        sc_tx(pSCDev->SmcId, pcommand, NextWriteSize);

        SCT0_P("\r\n");

        command_len -= NextWriteSize;
        pcommand     += NextWriteSize;
    *pnum_write_ok += NextWriteSize;

        err = T0_Procedure(pSCDev,
               INS,
                           command_len,
                           num_to_read,
                           proc_buf,
                           &proc_size,
                           &NextWriteSize,
                           &NextReadSize);

  smc_delay_us(1000);

      if (err != ERR_SMC_NO_ERR)
      {
      SCT0_E("\r\n");
    }
    }

    SCT0_P("\r\nFunc:%s, Line:%d", __FUNCTION__, __LINE__);

  SCT0_P("*******************************************\n");
  SCT0_P("NextReadSize is %d\n", NextReadSize);
  SCT0_P("num_to_read is %d\n", num_to_read);

    TotalRead = 0;
    while ((err == ERR_SMC_NO_ERR)  && (NextReadSize > 0) && (num_to_read > 0))
    {
          err = sc_rx(pSCDev->SmcId, &presponse[TotalRead], num_to_read, pnum_read_ok);
    if (err != ERR_SMC_NO_ERR)
    {
      SCT0_E("\r\n");
    }

    num_to_read -= *pnum_read_ok;
    TotalRead += *pnum_read_ok;
    NextReadSize = num_to_read;

    if (err == ERR_SMC_NO_ERR)
    {

      err = T0_Procedure(pSCDev,
              INS,
              0,
              num_to_read,
              proc_buf,
              &proc_size,
              &NextWriteSize,
              &NextReadSize);
    }

    if (err != ERR_SMC_NO_ERR)
    {
      SCT0_E("\r\n");
    }
    }

    SCT0_P("\r\nFunc:%s, Line:%d", __FUNCTION__, __LINE__);

  *pnum_read_ok = TotalRead;
    if (err == ERR_SMC_NO_ERR)
    {
        for (i = 0; i < proc_size; i++)
        {
            presponse[ (*pnum_read_ok) + i ] = proc_buf[i];
        }

        *pnum_read_ok  += proc_size;
    }

    return err;

} /* SMART_T0_Transfer() */


static ERRORCODE_T T0_Procedure(tscdevice *pSCDev,
          u8  INS,
                  u32  WriteSize,
                  u32  ReadSize,
                  u8 *pBuf,
                  u8 *pSize,
                  u32 *pNextWriteSize,
                  u32 *pNextReadSize)
{
    ERRORCODE_T  err = ERR_SMC_NO_ERR;
  BOOL MoreProcedure;
    u8 sw;
    u32 NextWriteSize;
  u32 i;

    NextWriteSize = 0;
    i = 0;
  *pSize = 0;

    /* Assume that the next read is all available bytes */
    *pNextReadSize = ReadSize;


    do {
        MoreProcedure = FALSE;

        err = sc_rx(pSCDev->SmcId, &sw, 1, NULL);
    SCT0_P("data is %x\n", sw);
        if (err != ERR_SMC_NO_ERR)
        {
            break;
        }

        pBuf[i] = sw;

        if (i == 0)
        {
        /* ACK = INS or ACK = INS+1 */
        if (((INS ^ sw) & 0xfe) == 0x00)
        {
                  if (WriteSize > 0)
                  {
                      NextWriteSize = WriteSize;
                  }
                  else if (ReadSize == 0)
                  {
                      MoreProcedure = 1;
                      i = 0;
                      continue;
                  }
        }
        /* ACK = ~INS or ACK = ~(INS+1) */
        else if (((INS ^ sw) & 0xfe) == 0xfe)
        {
                  if (WriteSize > 0)
                  {
                      NextWriteSize = 1;
                  }
         else if (ReadSize == 0)
                  {
                      MoreProcedure = 1;
                      i = 0;
                      continue;
                  }
         else
                     {
                        *pNextReadSize = 1;
                     }
        }
        /* ACK =0x60 (NULL), Read new Procedure Code SW1*/
        else if (sw == 0x60)
      {
          MoreProcedure = 1;
          i = 0;
          //*pNextReadSize = 0;
          continue;
      }
        /* ACK = SW1, Just Read SW2 */
        else if (((sw & 0xf0) == 0x60) || ((sw & 0xf0) == 0x90))
        {
            MoreProcedure = 1;
        *pNextReadSize = 0;
        }
        else
        {
        err = ERR_SMC_INVALID_STATUS_BYTE;
        }

        }/* if(i==0) end */

        i++;

    }while (MoreProcedure);

    if (err != ERR_SMC_NO_ERR)
    {
      return err;
    }

   SCT0_P("i is %d\n", i);
    *pSize = (u8)i;

    SCT0_P("\r\n Smc Get ACK!");

    *pNextWriteSize = NextWriteSize;

    return err;

}/* SMART_T0_ProcessProcedure() */


ERRORCODE_T sc_t14_transfer(tscdevice * pSCDev,
            u8 *pcommand, u32 command_len, u32 *pnum_write_ok,
            u8 *presponse, u32 num_to_read, u32 *pnum_read_ok)
{
    ERRORCODE_T err = ERR_SMC_NO_ERR;
    u32 NextReadSize, TotalRead;

    /* Transmit command */
    sc_tx(pSCDev->SmcId, pcommand, command_len);

  TotalRead = 0;
  NextReadSize = 8;    //Head Length

  err = sc_rx(pSCDev->SmcId,
              &presponse[TotalRead],
                NextReadSize,
                pnum_read_ok);

  if ((err == ERR_SMC_NO_ERR) && (*pnum_read_ok == 8))
  {
    TotalRead = *pnum_read_ok;
    NextReadSize = presponse[7]+1;
    if (NextReadSize > 0)
    {
       err = sc_rx(pSCDev->SmcId,
                   &presponse[TotalRead],
                        NextReadSize,
                        pnum_read_ok);

    }

    *pnum_read_ok += TotalRead ;
  }

  return err;
}

#endif

