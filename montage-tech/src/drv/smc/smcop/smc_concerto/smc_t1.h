/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SC_T1_H__
#define __SC_T1_H__


/*!
  Const Definition
  */
/*!
  max size of tt1block
  */
#define SMC_T1_BLOCKMAXLEN     (3 + 254 + 2)
/*!
  commments
  */
#define SMC_T1_CHAININGUS  0x0002

 
/*!
  commments
  */
#define NEXTSEQ(i)     ((i == 1) ? 0 : 1)


/*!
  commments
  */
#define R_EDC_ERROR     0x01
/*!
  commments
  */
#define R_OTHER_ERROR     0x02

/*!
  I-block
  */
#define I_CHAINING_BIT          0x20
/*!
  commments
  */
#define I_SEQUENCE_BIT          0x40

/*!
  In S-block
  */
#define S_RESYNCH_REQUEST       0x00
/*!
  commments
  */
#define S_IFS_REQUEST           0x01
/*!
  commments
  */
#define S_ABORT_REQUEST         0x02
/*!
  commments
  */
#define S_WTX_REQUEST           0x03
/*!
  commments
  */
#define S_VPP_ERROR_RESPONSE    0x24
/*!
  commments
  */
#define S_RESPONSE_BIT          0x20
/*!
  commments
  */
#define S_RESYNCH_RESPONSE      0xE0

/*!
  Block types
  */
/*!
  block type mask
  */
#define BLOCK_TYPE_BIT          0xC0
/*!
  management block
  */
#define S_REQUEST_BLOCK         0xC0
/*!
  Receive Block
  */
#define R_BLOCK                 0x80
/*!
  info block
  */
#define I_BLOCK_1               0x00
/*!
  info block
  */
#define I_BLOCK_2               0x40


/*!
  commments
  */
ERRORCODE_T sc_t1_transfer(tscdevice * pSCDev,
            u8 *pCommand,
            u32 NumToWrite,
            u32 *pNumWritten,
            u8 *pReply,
            u32 NumToReply,
            u32 *pNumReplied);


#endif //__SC_T1_H__
        
