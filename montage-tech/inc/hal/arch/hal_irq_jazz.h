/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __HAL_JAZZ_H__
#define __HAL_JAZZ_H__



/*!
  IRQ Interrupt Types.
  */
#define   IRQ_INT_TYPE_NORMAL    0
/*!
  comments
  */
#define   IRQ_INT_TYPE_FAST    1
/*!
  comments
  */
#define   IRQ_INT_TYPE_UNKNOWN    2

/*!
  Definition of IRQ control Functions
  */
//#define IRQ_USER_INT_NUM    16
 
/*!
  Definition of IRQ
  */
#define   IRQ_PTI_ID      0
/*!
  comments
  */
#define   IRQ_AUDOUT_ID      1
/*!
  comments
  */
#define   IRQ_GLB_ID      2
/*!
  comments
  */
#define   IRQ_T0_ID        3
/*!
  comments
  */
#define   IRQ_SPI_ID      4
/*!
  comments
  */
#define   IRQ_T1_ID        5
/*!
  comments
  */
#define   IRQ_T2_ID        6
/*!
  comments
  */
#define   IRQ_T3_ID        7
/*!
  comments
  */
#define   IRQ_ISGPI_ID      8
/*!
  comments
  */
#define   IRQ_JAZZ_SMC_ID      9
/*!
  comments
  */
#define   IRQ_I2C0_ID      10
/*!
  comments
  */
#define   IRQ_UART0_ID      11
/*!
  comments
  */
#define   IRQ_IRDA_ID      12
/*!
  comments
  */
#define   IRQ_KB_ID        13
/*!
  comments
  */
#define   IRQ_GPIO1_ID      14
/*!
  comments
  */
#define   IRQ_GPIO0_ID      15
/*!
  comments
  */
#define   IRQ_RTC_ID      16




/*!
  Definition of Register operation functions
  */
u8 inb(u32 port);
/*!
  comments
  */
void outb(u8 val, u32 port);
/*!
  comments
  */
u16 inw(u32 port);
/*!
  comments
  */
void outw (u16 val , u32 port);
/*!
  comments
  */
u32 inl(u32 port);
/*!
  comments
  */
void outl(u32 val , u32 port);


#endif //__HAL_JAZZ_H__


