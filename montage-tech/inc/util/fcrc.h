/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FAST_CRC_H__
#define __FAST_CRC_H__

/*!
  CRC32 CCITT order
  */
#define CRC32_ARITHMETIC_CCITT      0x21
/*!
  CRC32 order
  */
#define CRC32_ARITHMETIC            0x20

/*!
  Setup fast CRC accelerate table that will be used by fast CRC caculator
  
  \param[in] crc_mode the crc mode, CRC32_ARITHMETIC_CCITT or CRC32_ARITHMETIC
  
  \return SUCCESS if OK, else fail
  */
u32 crc_setup_fast_lut(int crc_mode);

/*!
  calculate the crc value
  
  \param[in] crc_mode the crc mode, CRC32_ARITHMETIC_CCITT or CRC32_ARITHMETIC
  \param[in] crc The initialize value of crc
  \param[in] p_buf the data to be calculated
  \param[in] len the length of the data
  
  \return the crc value
  */
u32 crc_fast_calculate(int crc_mode, u32 crc, u8 *p_buf, int len);

#endif  //__FAST_CRC_H__
