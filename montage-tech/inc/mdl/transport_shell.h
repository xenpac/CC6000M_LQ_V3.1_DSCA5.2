/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TRANSPORT_SHELL_H_
#define __TRANSPORT_SHELL_H_

/*!
  \file transport_shell.h
  This part provides the flag definition and initialization for 
  transport shell
  */
/*!
  Transport shell end flag
  */
#define MAGIC_LEN_END   0xFFFF
/*!
  End flag when transferring basic message queue
  */
#define MAGIC_LEN_MSQ_BASIC  0xFFFE
/*! 
  End flag when transferring msg pool
  */
#define MAGIC_LEN_MSQ_POOL    0xFFFD
/*!
  End flag when transferring common message queue
  */
#define MAGIC_LEN_MSQ_COMMON    0xFFFC
/*!
  Message buffer for transferring message buffer control
  */
#define MAGIC_LEN_MSQ_BUFCTRL    0xFFFB

/*!
  Tranport shell event
  */
typedef enum
{
  /*!
    Transport shell start
    */
  SHELL_EVT_BEGIN = (MDL_TRANSPORT_SHELL << 16),
  /*!
    Transport shell key notify
    */  
  SHELL_EVT_KEY,
} shell_evt_t;

/*!
  Initialize transport shell
  */
void transport_shell_init(void);


#endif // End for __TRANSPORT_SHELL_H_
