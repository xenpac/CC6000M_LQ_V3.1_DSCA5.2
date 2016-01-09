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
#include "smc_t1.h"

#define SCT1_DEBUG_ENABLE  0
#if  SCT1_DEBUG_ENABLE
#define  SCT1_P(...)      OS_PRINTF(__VA_ARGS__)
#else
#define  SCT1_P(...)
#endif

#define SCT1_ERROR_ENABLE  0

#if  SCT1_ERROR_ENABLE
#define  SCT1_E(...)    do { \
            OS_PRINTF(__VA_ARGS__);\
            OS_PRINTF(" line=%d", __LINE__);\
          }while(0)
#else
#define  SCT1_E(...)
#endif

/* static functions area */

static BOOL T1_ProcessSRequest(tscdevice *pSCDev, tt1details *pT1Details, tt1block *pRxBlock);
static ERRORCODE_T T1_Resync(tscdevice *pSCDev, tt1details *pT1Details, u8 SAD, u8 DAD);
static tblocktype T1_GetBlockType(u8 PCB);
static ERRORCODE_T IO_T1ReadBlock(tscdevice *pSCDev, tt1block *pBlock);
static void IO_T1WriteBlock(tscdevice *pSCDev, tt1block *pBlock);
static void CalculateEDC(tscdevice *pSCDev,tt1block *pBlock);
static void GenerateLRC(u8 *pBuffer, u32 Count, u8 *pLRC);
static void GenerateCRC(u8 *pBuffer, u32 Count, u16 *CRC_p);
static void MakeU8FromBlock(tt1block *pBlock,
                            BOOL CRC,
                            u8 *Buffer,
                            u32 *Length);

static void MakeU8_FromBlockAndReCalculateEDC(tscdevice *pSCDev, tt1block *pBlock,
                                      u8 *pBuffer, u32 *Length);


/* static Data Section */
static u8 g_Tsf_EDCBuffer[SMC_T1_BLOCKMAXLEN];
static u8 g_Tsf_RxBuffer[SMC_T1_BLOCKMAXLEN];
static u8 g_RxBlockBuffer[SMC_T1_BLOCKMAXLEN];
static u8 g_BlockBuffer[SMC_T1_BLOCKMAXLEN];

static void smc_delay_us(int t)
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

 /* 00 25 80 01 08 --> 3 + 10 + 1*/
ERRORCODE_T sc_t1_transfer(tscdevice * pSCDev,
            u8 *pCommand,
            u32 NumToWrite,
            u32 *pNumWritten,
            u8 *pReply,
            u32 NumToReply,
            u32 *pNumReplied)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;
  tblocktype ThisBlockType;  /* Type of current block */
  BOOL TransmitDone = FALSE;
  BOOL ReceiveDone = FALSE;
  BOOL LastTxAcked = FALSE;
  BOOL DoReceive = FALSE;
  BOOL bCRC = FALSE;

  tt1block TxBlock, RxBlock;
  tt1block RxTxBlock;

  u8 LRC;        /* EDC value */
  u16 CRC;
  u8  *pWritePtr = NULL;
  u32 NumLeft = NumToWrite;
  u16 TxMaxInfoStored = 0;

  u8 SAD = 0;
  u8 DAD = 0;
  BOOL Valid = FALSE;
  u32 Length = 0;

  tt1details *pT1Details = &pSCDev->T1Details;

  /* Variables Init */
  pWritePtr = pCommand;
  pT1Details->TxCount = 0;
  pT1Details->Aborting = FALSE;

  *pNumReplied = 0;
  *pNumWritten = 0;

  RxBlock.pBuffer = g_Tsf_RxBuffer;

   if (pSCDev->RC == 1)
   {
     bCRC = TRUE;
   }
  else
  {
    bCRC = FALSE;
  }

  do {
  /* build TxBlock Header info */

    TxBlock.Header.NAD = SAD | (DAD << 4);
    TxBlock.Header.PCB = (pT1Details->OurSequence << 6);

    if (NumLeft > pT1Details->TxMaxInfoSize)
    {
      pT1Details->State |= SMC_T1_CHAININGUS;
      TxBlock.Header.PCB |= I_CHAINING_BIT;
      TxBlock.Header.LEN = pT1Details->TxMaxInfoSize;
    }
    else
    {
      pT1Details->State &= ~SMC_T1_CHAININGUS;
      TxBlock.Header.LEN = NumLeft;
    }

    /* Set TxBlock Info Section */
    TxBlock.pBuffer = pWritePtr;
    CalculateEDC(pSCDev, &TxBlock);

    /* Transmit block */
    IO_T1WriteBlock(pSCDev, &TxBlock);

    TxMaxInfoStored = pT1Details->TxMaxInfoSize;

    /* if chaining, wait for r-block */
    if ((pT1Details->State & SMC_T1_CHAININGUS) && (err == ERR_SMC_NO_ERR))
    {
      DoReceive = TRUE;
    }
    else
    {
      DoReceive = FALSE;
      TransmitDone = TRUE;
    }

    while (DoReceive == TRUE)
    {
      /* Default to one pass through the receive section */
      DoReceive = FALSE;
      err = IO_T1ReadBlock(pSCDev, &RxBlock);
      if (err != ERR_SMC_NO_ERR) return err;

      ThisBlockType = T1_CORRUPT_BLOCK;
      if (err == ERR_SMC_NO_ERR)
      {
        /* Calculate EDC */
        Valid = FALSE;
        MakeU8FromBlock(&RxBlock, bCRC, g_Tsf_EDCBuffer, &Length);
        if (bCRC)
        {
          GenerateCRC(g_Tsf_EDCBuffer, Length - 2, &CRC);
          if (CRC == RxBlock.EDC.CRC)
          {
            Valid = TRUE;
          }
          else
          {
            SCT1_E("\r\nCRC Not Equal");
          }
        }
        else
        {
          GenerateLRC(g_Tsf_EDCBuffer, Length - 1, &LRC);
          if (LRC == RxBlock.EDC.LRC)
          {
            Valid = TRUE;
          }
          else
          {
            SCT1_E("\r\nLRC Not Equal");
          }
        }


         if (Valid == TRUE)
         {
          ThisBlockType = T1_GetBlockType(RxBlock.Header.PCB);
          if (ThisBlockType == T1_S_REQUEST)
          {
            TransmitDone = T1_ProcessSRequest(pSCDev, pT1Details, &RxBlock);
            /* Read another block, since processreq. transmits a
            * block
            */
            if (TransmitDone == TRUE)
            {
              err = IO_T1ReadBlock(pSCDev, &RxBlock);
              if (err != ERR_SMC_NO_ERR) return err;
              if (err == ERR_SMC_NO_ERR)
              {
                ThisBlockType = T1_GetBlockType(RxBlock.Header.PCB);
              }
            }
          }
         }

       }

      /* If r-block received and ack okay, flip our seq. number */
      if ((ThisBlockType == T1_R_BLOCK) &&
        (((RxBlock.Header.PCB & 0x10) >> 4) != pT1Details->OurSequence) &&
        ((RxBlock.Header.PCB & R_EDC_ERROR) == 0) &&
        ((RxBlock.Header.PCB & R_OTHER_ERROR) == 0))

      {
        NumLeft -= TxMaxInfoStored;
        pT1Details->OurSequence = NEXTSEQ(pT1Details->OurSequence);
        pWritePtr += TxMaxInfoStored;
        pT1Details->TxCount = 0;
      }
      else
       {
        /* Note retransmission */
        pT1Details->TxCount++;

        /* How many attemps have we made */
        if (pT1Details->TxCount >= 3)
        {
          err = T1_Resync(pSCDev, pT1Details, SAD, DAD);
          if (err != ERR_SMC_NO_ERR)
          {
            SCT1_E("\r\nT1_Resync Error = %x", err);
          }
        }
        else if (ThisBlockType == T1_CORRUPT_BLOCK)
        {
           /* if corrupt, send an appropriate r-block */
          RxTxBlock.Header.NAD = SAD | (DAD << 4);
          if (pT1Details->FirstBlock)
            RxTxBlock.Header.PCB = 1;
          else
            RxTxBlock.Header.PCB = 1 | (pT1Details->TheirSequence << 4);

          RxTxBlock.Header.PCB |= R_BLOCK;
          RxTxBlock.Header.LEN  = 0;
          RxTxBlock.pBuffer     = NULL;
          CalculateEDC(pSCDev, &RxTxBlock);

          /* Transmit */
          IO_T1WriteBlock(pSCDev, &RxTxBlock);
          DoReceive = TRUE;
        }
      }
    }

    smc_delay_us(1000);
   } while((!TransmitDone) && (*pNumWritten < NumToWrite) && (err == ERR_SMC_NO_ERR));

  pT1Details->FirstBlock = TRUE;
  do {
    if (err != ERR_SMC_NO_ERR) break;

    if ((NULL == pReply) || (NULL == pNumReplied))
    {
      break;
    }

    /* Set the Buffer to be within the user' buffer */
    RxBlock.pBuffer = &g_Tsf_RxBuffer[0];

    err = IO_T1ReadBlock(pSCDev, &RxBlock);
    if (err != ERR_SMC_NO_ERR)
    {
      SCT1_E("\r\nIO_T1ReadBlock()");
      break;
    }
    /* Calculate EDC */
    Valid = FALSE;
    MakeU8FromBlock(&RxBlock, bCRC, g_Tsf_EDCBuffer, &Length);
    if (bCRC)
    {
      GenerateCRC(g_Tsf_EDCBuffer, Length - 2, &CRC);
      if (CRC == RxBlock.EDC.CRC)
      {
        Valid = TRUE;
      }
    }
    else
    {
      GenerateLRC(g_Tsf_EDCBuffer, Length - 1, &LRC);
      if (LRC == RxBlock.EDC.LRC)
      {
        Valid = TRUE;
      }
      else
      {
        SCT1_E("\r\nLRC Not Equal. LRC(c)=%02x, LRC(r)=%02x, EDC=%04x",
            LRC, RxBlock.EDC.LRC, RxBlock.EDC.CRC);
      }
    }

    if ((Valid == FALSE) || (err != ERR_SMC_NO_ERR))
    {
      SCT1_P("\r\nLRC wrong");
      if (pT1Details->TxCount < 3)
      {
        /* Increase the count, retransmit */
        pT1Details->TxCount++;

        /* Build r-block */
        RxTxBlock.Header.NAD = SAD | (DAD << 4);
        if (pT1Details->FirstBlock)
          RxTxBlock.Header.PCB = 0;
        else
          RxTxBlock.Header.PCB = (NEXTSEQ(pT1Details->TheirSequence) << 4);

        RxTxBlock.Header.PCB |= R_BLOCK;

        if (err == ERR_SMC_NO_ERR)
          RxTxBlock.Header.PCB |= R_EDC_ERROR; /* EDC error */
        else
          RxTxBlock.Header.PCB |= R_OTHER_ERROR; /* not EDC error */

        RxTxBlock.Header.LEN = 0;
        RxTxBlock.pBuffer = NULL;
        CalculateEDC(pSCDev, &RxTxBlock);

        IO_T1WriteBlock(pSCDev, &RxTxBlock);

      }
       else
      {
        err = T1_Resync(pSCDev, pT1Details, SAD, DAD);
      }
    }
    else
    {
      /* See What Kind of Block */
      ThisBlockType = T1_GetBlockType(RxBlock.Header.PCB);
      //     OS_PRINTF("\r\nRxBlock PCB = %x, ThisBlockType=%d", RxBlock.Header.PCB, ThisBlockType);

      if ((ThisBlockType != T1_S_REQUEST) && (ThisBlockType != T1_I_BLOCK))
      {
        /* Retransmit */
        pT1Details->TxCount ++;

        if (pT1Details->TxCount < 3)
        {
          ////
          RxTxBlock.Header.NAD = SAD | (DAD << 4);
          RxTxBlock.Header.LEN = 0;
          RxTxBlock.pBuffer    = NULL;
          RxTxBlock.Header.PCB = R_BLOCK;
          ////
          if (ThisBlockType == T1_R_BLOCK)
          {
          /* see if the last block we send out is what they're requesting */

            if (pT1Details->OurSequence != ((RxBlock.Header.PCB >> 4) & 1))
            {
              CalculateEDC(pSCDev, &RxTxBlock);
              IO_T1WriteBlock(pSCDev, &RxTxBlock);
            }
            else
            {
              if (LastTxAcked)
              {
                CalculateEDC(pSCDev, &RxTxBlock);
                IO_T1WriteBlock(pSCDev, &RxTxBlock);
              }
              else
                IO_T1WriteBlock(pSCDev, &TxBlock);
            }
          }
          else
          {
            /*It's all gone horribly wrong. Panic */
            if (LastTxAcked)
            {
              CalculateEDC(pSCDev, &RxTxBlock);
              IO_T1WriteBlock(pSCDev, &RxTxBlock);
            }
            else
            {
              IO_T1WriteBlock(pSCDev, &TxBlock);
            }
          }
        }
        else
        {
          T1_Resync(pSCDev, pT1Details, SAD, DAD);
        }
      }
      else
       {
        /* valid block type, continue */

        /* if s-request,process */
        if (ThisBlockType == T1_S_REQUEST)
        {
          /* this function alse transmits the reply */
          //OS_PRINTF("\r\nThisBlockType == T1_S_REQUEST");
          ReceiveDone = T1_ProcessSRequest(pSCDev, pT1Details, &RxBlock);
        }
        else  /* T1_I_BLOCK */
        {

          //OS_PRINTF("\r\nRxBlock PCB=%d,TSeq=%d",RxBlock.Header.PCB, pT1Details->TheirSequence);
          /* Check if this block has the expected seq. number */
          if (((RxBlock.Header.PCB & I_SEQUENCE_BIT) >> 6) != pT1Details->TheirSequence)
          {
            u32 Number = 0;

            if ((RxBlock.Header.LEN < (NumToReply - *pNumReplied)) ||
              (NumToReply == 0))
            {
              Number = RxBlock.Header.LEN;
            }
            else
            {
              Number = (NumToReply - *pNumReplied);
            }

            *pNumReplied += Number;

            pT1Details->TxCount = 0;

            /* Set their last received seq. number */
            pT1Details->TheirSequence = ((RxBlock.Header.PCB & I_SEQUENCE_BIT) >> 6);

            if (LastTxAcked == FALSE)
            {
              LastTxAcked = TRUE;
              pT1Details->OurSequence = NEXTSEQ(pT1Details->OurSequence);
            }

            if (pT1Details->FirstBlock == TRUE)
              pT1Details->FirstBlock = FALSE;

            /* Is this block chaining */
            if ((RxBlock.Header.PCB & I_CHAINING_BIT) != 0)
            {
              /* yes; send an r-block with their next
               * expected no.  (block must have been
              * received properly to get this far)
              */
              RxTxBlock.Header.PCB = R_BLOCK |
                  (NEXTSEQ(pT1Details->TheirSequence) << 4);
              RxTxBlock.Header.NAD = SAD | (DAD << 4);
              RxTxBlock.Header.LEN = 0;
              RxTxBlock.pBuffer    = NULL;
              CalculateEDC(pSCDev, &RxTxBlock);

              IO_T1WriteBlock(pSCDev, &RxTxBlock);

            }
            else
            {
              /* Not Chaining (now */
              ReceiveDone = TRUE;
            }
          }
          else
          {
            /* case - we received valid i-block with the
            * same seq.number we got last; therefore they
            * didn't get our block, Request retransmission
            * of the missing block.block
            */
            RxTxBlock.Header.NAD = SAD | (DAD << 4);
            RxTxBlock.Header.PCB = R_BLOCK;
            RxTxBlock.Header.PCB |= NEXTSEQ(pT1Details->TheirSequence);
            RxTxBlock.Header.LEN = 0;
            RxTxBlock.pBuffer = NULL;

            CalculateEDC(pSCDev, &RxTxBlock);

            IO_T1WriteBlock(pSCDev, &RxTxBlock);

          }
        }
      }
    }

    smc_delay_us(1000);
  }while ((!ReceiveDone) && ((NumToReply == 0) || (*pNumReplied < NumToReply)) &&
        (err == ERR_SMC_NO_ERR));

  pT1Details->ErrorCode = err;

  if (err == ERR_SMC_NO_ERR)
  {
    /* Modified by David */
    RxBlock.Header.PCB = 0;
    MakeU8_FromBlockAndReCalculateEDC(pSCDev, &RxBlock, pReply, pNumReplied);
  }

  #if 0

  SCT1_P("\r\npT1Details->Aborting = %x",   pT1Details->Aborting);
  SCT1_P("\r\npT1Details->ErrorCode = %x",   pT1Details->ErrorCode);
  SCT1_P("\r\npT1Details->TxCount = %x",   pT1Details->TxCount);
  SCT1_P("\r\npT1Details->TxMaxInfoSize = %x",   pT1Details->TxMaxInfoSize);
  SCT1_P("\r\npT1Details->State = %x",     pT1Details->State);
  SCT1_P("\r\npT1Details->FirstBlock = %x",   pT1Details->FirstBlock);

  SCT1_P("\r\npT1Details->OurSequence = %x",   pT1Details->OurSequence);
  SCT1_P("\r\npT1Details->TheirSequence = %x",   pT1Details->TheirSequence);
  #endif

  return err;

}


static BOOL T1_ProcessSRequest(tscdevice *pSCDev, tt1details *pT1Details, tt1block *pRxBlock)
{
  tt1block TxBlock;
  BOOL ReceiveDone = FALSE;
  tt1block RxBlock;
  BOOL Done = FALSE;
  ERRORCODE_T err;
  BOOL Sblock;

  if ((pRxBlock->Header.PCB & S_VPP_ERROR_RESPONSE) == S_VPP_ERROR_RESPONSE)
  {
    pT1Details->ErrorCode = ERR_SMC_CARD_VPP;
    ReceiveDone = TRUE;
  }
  else if ((pRxBlock->Header.PCB & S_WTX_REQUEST) == S_WTX_REQUEST)
  {
    /* Multiply BWT by buffer[0], for this block only */
    //    pT1Details->BWTMultiplier = pRxBlock->pBuffer[0];
    ReceiveDone = FALSE;
  }
  else if ((pRxBlock->Header.PCB & S_ABORT_REQUEST) == S_ABORT_REQUEST)
  {
    /* Abort any pending IO operations with the card. */
    pT1Details->ErrorCode = ERR_SMC_CARD_ABORTED;
    ReceiveDone = TRUE;
  }
  else if ((pRxBlock->Header.PCB & S_IFS_REQUEST) == S_IFS_REQUEST)
  {
    /* Change Txmaxinfosize */
    pT1Details->TxMaxInfoSize = pRxBlock->pBuffer[0];
  }

  /* Those S-responses with an info field are supposed to match the
  * request. Which is handy.
  */
  memcpy(&TxBlock, pRxBlock, sizeof(tt1block));
  TxBlock.Header.PCB |= S_RESPONSE_BIT;
  CalculateEDC(pSCDev, &TxBlock);

  IO_T1WriteBlock(pSCDev, &TxBlock);

  /* If we're acking an abort request, we need to read (and possibly discard)
  * the block the card sends in return.
  */
  if (pT1Details->ErrorCode == ERR_SMC_CARD_ABORTED)
  {
    pT1Details->TxCount = 0;
    RxBlock.pBuffer = &g_RxBlockBuffer[0];
    do
    {
      /* Read the block tranferring right to send */
      err = IO_T1ReadBlock(pSCDev, &RxBlock);
      //     if (!bRet) return FALSE;

      /* Check type */
      Sblock = ((RxBlock.Header.PCB & 0x0c) == 0x0c) ? TRUE : FALSE;

      /* S-response can be considered acknowledged (in this case) if the
      * card does *not* send another abort request.
      */
      if ((Sblock == FALSE) ||
        ((Sblock == TRUE) && ((RxBlock.Header.PCB & S_ABORT_REQUEST) == 0)))
      {
        Done = TRUE;
      }
      else
      {
        /* Else, retransmit 3 times, before doing resync */
        pT1Details->TxCount++;
        if (pT1Details->TxCount >= 3)
        {
          T1_Resync(pSCDev, pT1Details, pRxBlock->Header.NAD & 0x7,
                  (pRxBlock->Header.NAD & 0x70) >> 4);
          Done = TRUE;
        }
        else
        {
          IO_T1WriteBlock(pSCDev, &TxBlock);
        }
      }
    } while (Done == FALSE);
  }

  return ReceiveDone;
}


static ERRORCODE_T T1_Resync(tscdevice *pSCDev, tt1details *pT1Details, u8 SAD, u8 DAD)
{
    ERRORCODE_T err = ERR_SMC_NO_ERR;
    tt1block TxBlock, RxBlock;
    BOOL Valid;
    u8 LRC = 0, Count = 0;
  u16 CRC = 0;
  BOOL bCRC = FALSE;
    u32 Length;

  if (pSCDev->RC == 1)
  {
    bCRC = TRUE;
  }
    /* Send sync s.block */
    TxBlock.Header.PCB = S_REQUEST_BLOCK | S_RESYNCH_REQUEST;
    TxBlock.Header.NAD = SAD | (DAD << 4);
    TxBlock.Header.LEN = 0;
    TxBlock.pBuffer = NULL;

    CalculateEDC(pSCDev, &TxBlock);

    /* Send until we get a valid response, or until we've tried 3 times.
     * No need to assign RxBlock a buffer, since resync-responses don't
     * have an info field.
     */
    Count = 0;
    do
    {
    IO_T1WriteBlock(pSCDev, &TxBlock);

    RxBlock.pBuffer = &g_RxBlockBuffer[0]; /* Modified by Baorsh */
    err = IO_T1ReadBlock(pSCDev, &RxBlock);
    if (err != ERR_SMC_NO_ERR)
    {
      return err;
    }

    /* Calculate EDC */
    Valid = FALSE;

      MakeU8FromBlock(&RxBlock, bCRC, g_BlockBuffer, &Length);
      if (bCRC)
    {
      GenerateCRC(g_BlockBuffer, Length - 2, &CRC);
      if (CRC == RxBlock.EDC.CRC)
      {
        Valid = TRUE;
      }
    }
    else
    {
      GenerateLRC(g_BlockBuffer, Length - 1, &LRC);
        if (LRC == RxBlock.EDC.LRC)
        {
          Valid = TRUE;
        }
    }

    Count++;
    } while ((Count < 3) &&
             ((RxBlock.Header.PCB != S_RESYNCH_RESPONSE) || (Valid == FALSE)));

    if ((Valid == FALSE) || (RxBlock.Header.PCB != S_RESYNCH_RESPONSE))
    {
        /* stsmart_no_answer would maybe also fit, apart from possibility
         * card is transmitting but EDC incorrect
         */
        err = ERR_SMC_T1_LRC_CEHCK_FAIL;
    }

    /* Reset sequence numbers */
    pT1Details->TheirSequence = 1;  /* So we're expecting 0 next block...   */
    pT1Details->OurSequence = 0;    /* And sending 0 as our first S(N)      */
    pT1Details->FirstBlock = TRUE;
    pT1Details->TxCount = 0;

    return err;
}


static tblocktype T1_GetBlockType(u8 PCB)
{
  tblocktype ThisBlockType;

  /* See what we got back */
  switch (PCB & BLOCK_TYPE_BIT)
  {
    case S_REQUEST_BLOCK:   /* S-block */
      ThisBlockType = T1_S_REQUEST;
      break;

    case R_BLOCK:           /* R-block */
      ThisBlockType = T1_R_BLOCK;
      break;

    case I_BLOCK_1:         /* Two possible forms of i-block */
    case I_BLOCK_2:
      ThisBlockType = T1_I_BLOCK;
      break;

    default:
      ThisBlockType = T1_CORRUPT_BLOCK;
      break;
  }

  return ThisBlockType;
}


static ERRORCODE_T IO_T1ReadBlock(tscdevice *pSCDev, tt1block *pBlock)
{
  ERRORCODE_T err = ERR_SMC_NO_ERR;

  err = sc_rx(pSCDev->SmcId, &pBlock->Header.NAD, 1, NULL);
  if (err != ERR_SMC_NO_ERR)
  {
    SCT1_E("\r\nRead NAD ERROR");
    return ERR_SMC_T1_READ_NAD_FAIL;
  }

  /* Read PCB */
  err = sc_rx(pSCDev->SmcId, &pBlock->Header.PCB, 1, NULL);
  if (err != ERR_SMC_NO_ERR)
  {
    SCT1_E("\r\nRead PCB ERROR");
    return ERR_SMC_T1_READ_PCB_FAIL;
  }

  /* Read LEN */
  err = sc_rx(pSCDev->SmcId, &pBlock->Header.LEN, 1, NULL);
  if (err != ERR_SMC_NO_ERR)
  {
    SCT1_E("\r\nRead LEN ERROR");
    return ERR_SMC_T1_READ_LEN_FAIL;
  }

  /* Read any info field */
  err = sc_rx(pSCDev->SmcId, pBlock->pBuffer, pBlock->Header.LEN, NULL);
  if (err != ERR_SMC_NO_ERR)
  {
    SCT1_E("\r\nRead info field ERROR");
    return ERR_SMC_T1_READ_BUF_FAIL;
  }

  /* Read epilogue field */
  if (pSCDev->RC == 1)
  {
    err = sc_rx(pSCDev->SmcId, (u8 *)&pBlock->EDC.CRC, 2, NULL);
  }
  else
  {
    err = sc_rx(pSCDev->SmcId, &pBlock->EDC.LRC, 1, NULL);
  }
  if (err != ERR_SMC_NO_ERR)
  {
    SCT1_E("\r\nRead epilogue field ERROR");
    return ERR_SMC_T1_READ_LRC_FAIL;
  }

  return ERR_SMC_NO_ERR;
}


static void IO_T1WriteBlock(tscdevice *pSCDev, tt1block *pBlock)
{
    u32 Length;
  BOOL bCRC = FALSE;

    if (pSCDev->RC == 1)
    {
      bCRC = TRUE;
    }

  MakeU8FromBlock(pBlock, bCRC, g_BlockBuffer, &Length);

    sc_tx(pSCDev->SmcId, g_BlockBuffer, Length);
}


static void CalculateEDC(tscdevice *pSCDev, tt1block *pBlock)
{

    u8 BlockBuffer[SMC_T1_BLOCKMAXLEN];
    u32 Length;

    /* Calculate EDC */
    if (pSCDev->RC == 1)
    {
        MakeU8FromBlock(pBlock, TRUE, BlockBuffer, &Length);
        GenerateCRC(BlockBuffer, Length - 2, &pBlock->EDC.CRC);
    }
    else
    {
        MakeU8FromBlock(pBlock, FALSE, BlockBuffer, &Length);
        GenerateLRC(BlockBuffer, Length - 1, &pBlock->EDC.LRC);
    }

}


static void GenerateLRC(u8 *pBuffer, u32 Count, u8 *pLRC)
{
    u32 i;

    *pLRC = 0;

    for (i = 0; i < Count; i++)
  {
      *pLRC ^=  pBuffer[i];
   }
}


/* Calculate on the fly - this is slow, but will do for a first implementation.
 * Can look at a table driven version later if this is actually the right poly.
 */
static void GenerateCRC(u8 *Buffer_p, u32 Count, u16 *CRC_p)
{
    u32 i;
    /* Initialise the CRC */
    *CRC_p = 0;


    /* Then work through the buffer, updating the CRC accordingly as we go */
    for (i = 0; i < Count; i++)
    {
        *CRC_p ^= Buffer_p[i];
        *CRC_p ^= (u8)(*CRC_p & 0xff) >> 4;
        *CRC_p ^= (*CRC_p << 8) << 4;
        *CRC_p ^= ((*CRC_p & 0xff) << 4) << 1;
    }
}


static void MakeU8FromBlock(tt1block *pBlock,
                            BOOL CRC,
                            u8 *Buffer,
                            u32 *Length)
{

    Buffer[0] = pBlock->Header.NAD;
    Buffer[1] = pBlock->Header.PCB;
    Buffer[2] = pBlock->Header.LEN;

  if ((pBlock->pBuffer != NULL) && (Buffer[2] > 0))
  {
      memcpy(&Buffer[3], pBlock->pBuffer, Buffer[2]);
  }

    /* Might not be set yet, but copy it anyway */
    if (CRC)
    {
        /* MSB-first */
        Buffer[3 + Buffer[2]] = (u8)((pBlock->EDC.CRC & 0xff00) >> 8);
        Buffer[4 + Buffer[2]] = (u8)(pBlock->EDC.CRC & 0x00ff);
        *Length = 3 + Buffer[2] + 2;
    }
    else
    {
        Buffer[3 + Buffer[2]] = pBlock->EDC.LRC;
        *Length = 3 + Buffer[2] + 1;
    }

}

static void MakeU8_FromBlockAndReCalculateEDC(tscdevice *pSCDev, tt1block *pBlock,
                                      u8 *pBuffer, u32 *Length)
{
    /* Calculate EDC */
    if (pSCDev->RC == 1)
    {
        MakeU8FromBlock(pBlock, TRUE, pBuffer, Length);
        GenerateCRC(pBuffer, *Length - 2, &pBlock->EDC.CRC);
    pBuffer[*Length - 2] = (pBlock->EDC.CRC >> 8) & 0xff;
    pBuffer[*Length - 1] = (pBlock->EDC.CRC) & 0xff;
    }
    else
    {
        MakeU8FromBlock(pBlock, FALSE, pBuffer, Length);
        GenerateLRC(pBuffer, *Length - 1, &pBlock->EDC.LRC);
    pBuffer[*Length - 1] = pBlock->EDC.LRC;
    }
}

#endif //#ifdef DRV_SEL_SMC

