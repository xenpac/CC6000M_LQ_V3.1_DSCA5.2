/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SMC_JAZZ_H__
#define __SMC_JAZZ_H__

/*!
  Card 0 plug in
  */
#define SMC7816_INT_CARD0_IN 0x01
/*!
  Card 0 plug out
  */
#define SMC7816_INT_CARD0_OUT 0x02
/*!
  Card 1 plug in
  */
#define SMC7816_INT_CARD1_IN 0x03
/*!
  Card 0 plug out
  */
#define SMC7816_INT_CARD1_OUT 0x04

/*!
  all int mask bit
  */
#define SMC7816_INT_MASK 0x3FFF

/*! 
  smc private data definition
  */
typedef struct 
{
  /*!
    commments
  */
  drv_smartcardhandle_t SCHandler;
  /*!
    commments
  */
  scard_config_t  scard_cfg;
  /*!
    commments
  */
  smc_op_notify notify;
  /*!
    commments
  */
  smc_op_term_check term_check;
  /*!
    commments
  */
  drvsvc_handle_t *p_svc;
  /*!
    commments
  */
  u32 *p_stack;
}smc_priv_t;

/*!
  get card status
  */
#define SMC_IO_CTRL_GET_STATUS 0x01

/*!
  get card protocol
  */
#define SMC_IO_CTRL_GET_PROTOCOL 0x02


#endif //__SMC_JAZZ_H__

