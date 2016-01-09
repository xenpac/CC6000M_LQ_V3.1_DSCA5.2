/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HAL_IRQ_H__
#define __HAL_IRQ_H__

/*!
  Interrupt ID - PTI
  */
#define IRQ_PTI_ID              0
/*!
  Interrupt ID - PCR
  */
#define IRQ_PCR_ID              1
/*!
  Interrupt ID - Global
  */
#define IRQ_GLB_ID              2
/*!
  Interrupt ID - Timer 0
  */
#define IRQ_T0_ID               3
/*!
  Interrupt ID - Watchdog
  */
#define IRQ_WDOG_ID             4
/*!
  Interrupt ID - Timer 1
  */
#define IRQ_T1_ID               5
/*!
  Interrupt ID - Timer 2
  */
#define IRQ_T2_ID               6
/*!
  Interrupt ID - Timer 3
  */
#define IRQ_T3_ID               7
/*!
  Interrupt ID - Smart Card
  */
#define IRQ_SMC_ID              8
/*!
  Interrupt ID - DMA
  */
#define IRQ_DMA_ID              9
/*!
  Interrupt ID - I2C
  */
#define IRQ_I2C0_ID             10
/*!
  Interrupt ID - UART0
  */
#define IRQ_UART0_ID            11
/*!
  Interrupt ID - IRDA
  */
#define IRQ_IRDA_ID             12
/*!
  Interrupt ID - Frontpanel
  */
#define IRQ_KB_ID               13
/*!
  Interrupt ID - GPIO1, external interrupt
  */
#define IRQ_GPIO1_ID            14
/*!
  Interrupt ID - GPIO0, external interrupt
  */
#define IRQ_GPIO0_ID            15
/*!
  Interrupt ID - UART1, PINMux with GPIO0
  */
#define IRQ_UART1_ID            15

/*!
  Interrupt watchdog
  */
#define IRQ_WATCHDOG_ID  4




/*!
  Interrupt type, up to now only support normal
  */
#define IRQ_INT_TYPE_NORMAL     0

#endif //__HAL_IRQ_H__
