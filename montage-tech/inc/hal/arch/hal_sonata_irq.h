/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HAL_SONATA_IRQ_H__
#define __HAL_SONATA_IRQ_H__

/*!
  Interrupt ID - ethernet dma
  */
#define IRQ_EDMA_ID              0
/*!
  Interrupt ID - ethernet mac
  */
#define IRQ_EMAC_ID              1
/*!
  Interrupt ID - USB0
  */
#define IRQ_USB0_ID 2
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
#define IRQ_SMC0_ID              8
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
  Interrupt ID - AV CPU
  */
#define IRQ_AVCPU_ID            14
/*!
  Interrupt ID - UART1
  */
#define IRQ_UART1_ID            15
/*!
  Interrupt ID - PTI
  */
#define IRQ_PTI_ID            16
/*!
  Interrupt ID - GPIO
  */
#define IRQ_GPIO_ID            17
/*!
  Interrupt ID - DDR EXPIRED
  */
#define IRQ_DDREXP_ID            18
/*!
  Interrupt ID - DDR TOUCH
  */
#define IRQ_DDRTOUCH_ID            19
/*!
  Interrupt ID - GRA ENG PROCESS END
  */
#define IRQ_GEPEND_ID            20
/*!
  Interrupt ID - GRA ENG CMDFIFO END
  */
#define IRQ_GECEND_ID            21
/*!
  Interrupt ID - JPG ENGINE
  */
#define IRQ_JPGE_ID            22
/*!
  Interrupt ID - HD ENCODER TOP START
  */
#define IRQ_HDENC_TOP_START_ID            23
/*!
  Interrupt ID - SDCARD
  */
#define IRQ_SDCARD_ID            24
/*!
  Interrupt ID - HD ENCODER BOT START
  */
#define IRQ_HDENC_BOT_START_ID            25
/*!
  Interrupt ID - CI Plus
  */
#define IRQ_CI_PLUS_ID            26
/*!
  Interrupt ID - USB1
  */
#define IRQ_USB1_ID            27
/*!
  Interrupt ID - Smart card 1
  */
#define IRQ_SMC1_ID            28
/*!
  Interrupt ID - I2C1
  */
#define IRQ_I2C1_ID            29
/*!
  Interrupt ID - always on RTC
  */
#define IRQ_ALWAYS_ON_RTC_ID          30
/*!
  Interrupt ID - always on MCU2AP
  */
#define IRQ_MCU2AP_ID            31

/*!
  Interrupt ID on AV CPU - HDMI
  */
#define IRQ_HDMI_ID            1

/*!
  Interrupt type, up to now only support normal
  */
#define IRQ_INT_TYPE_NORMAL     0

#endif //__HAL_IRQ_H__
