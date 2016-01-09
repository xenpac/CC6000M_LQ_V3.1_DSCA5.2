/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef _UART_WIN32_H_
#define _UART_WIN32_H_

#include "sys_types.h"

/*!
  The COM parameters 
  */
typedef struct  //SComm Parameter
{
  /*!
    COM index
    */
  u8 bPort;
  /*!
    baud rate
    */
  u8 bBaudRate;
  /*!
    the COM byte size
    */
  u8 bByteSize;
  /*!
    parity
    */
  u8 bParity;
  /*!
    parity check enable
    */
  u8 fParity;
  /*!
    stop bits
    */
  u8 bStopBits;
}COM_PARAM_ST;


void hal_win32_uart_attach(void);

#endif

