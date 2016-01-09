/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/


#ifndef __SYS_HAL_H__
#define __SYS_HAL_H__

typedef enum
{
  /*!
    CT1642
    */
  HAL_CT1642 = 0,
  /*!
    HC164
    */
  HAL_HC164,
  /*!
    FD650
    */
  HAL_FD650,
    /*!
    GPIO
    */
  HAL_GPIO,
  /*!
    TM1637
    */
  HAL_TM1637,
  /*!
    TM1635
    */
  HAL_TM1635,
  /*!
    FD620
    */
  HAL_FD620,
  /*!
    TM1618
    */
  HAL_TM1618,
  /*!
    LM8168
    */
  HAL_LM8168
}hal_fp_type_t;

#endif

