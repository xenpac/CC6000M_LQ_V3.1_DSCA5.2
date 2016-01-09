/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SC_T0_H__
#define __SC_T0_H__


/*!
  T=0 defines
  */
#define T0_CMD_LENGTH           5
/*!
  commments
  */
#define T0_CLA_OFFSET           0
/*!
  commments
  */
#define T0_INS_OFFSET           1
/*!
  commments
  */
#define T0_P1_OFFSET            2
/*!
  commments
  */
#define T0_P2_OFFSET            3
/*!
  commments
  */
#define T0_P3_OFFSET            4
/*!
  commments
  */
#define T0_RETRIES_DEFAULT      3

/*!
  commments
  */
ERRORCODE_T sc_t0_transfer(tscdevice * pSCDev,
              u8 *pcommand, u32 command_len, u32 *pnum_write_ok,
              u8 *presponse, u32 num_to_read, u32 *pnum_read_ok);

/*!
  commments
  */
ERRORCODE_T sc_t14_transfer(tscdevice * pSCDev,
            u8 *pcommand, u32 command_len, u32 *pnum_write_ok,
            u8 *presponse, u32 num_to_read, u32 *pnum_read_ok);

/*!
  commments
  */
void smc_delay_us(int t);
                        
#endif //__SC_T0_H__
        

