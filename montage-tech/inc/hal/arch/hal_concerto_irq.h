/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HAL_CONCERTO_IRQ_H__
#define __HAL_CONCERTO_IRQ_H__

/*!
  Timer 0
  */
#define IRQ_T0_ID               0
/*!
  Timer 1
  */
#define IRQ_T1_ID               1
/*!
  Timer 2
  */
#define IRQ_T2_ID               2
/*!
  Timer 3
  */
#define IRQ_T3_ID               3
/*!
  Mail Box
  */
#define IRQ_MAILBOX_ID   4
/*!
  SPDMA
  */
#define IRQ_SPDMA_ID   5
/*!
  DMA
  */
#define IRQ_DMA_ID      6
/*!
  SECURE
  */
#define IRQ_SECURE_ID      7
/*!
  SWTS_GINT
  */
#define IRQ_SWTS_GINT_ID      8
/*!
  PVR_GINT
  */
#define IRQ_PVR_GINT_ID       9
/*!
  TSIG0_GIN
  */
#define IRQ_TSIG0_GIN_ID       10
/*!
  TSIG1_GIN
  */
#define IRQ_TSIG1_GIN_ID      11
/*!
  PTI_SF
  */
#define IRQ_PTI_SF_ID            12
/*!
  VDEC
  */
#define IRQ_VDEC_ID            13
/*!
  JPEG
  */
#define IRQ_JPEG_ID            14
/*!
  AOUT
  */
#define IRQ_AOUT_ID            15
/*!
  AVSYNC
  */
#define IRQ_AVSYNC_ID            16
/*!
  GRA_ENG_0
  */
#define IRQ_GRA_ENG_0_ID            17
/*!
  GRA_ENG_1
  */
#define IRQ_GRA_ENG_1_ID            18
/*!
  HD_TOP_START
  */
#define IRQ_HD_TOP_START_ID            19
/*!
  HD_BOT_START
  */
#define IRQ_HD_BOT_START_ID            20
/*!
  SD_VENC
  */
#define IRQ_SD_VENC_ID            21
/*!
  HDMI
  */
#define IRQ_HDMI_ID            22
/*!
  GPIO_GINT
  */
#define IRQ_GPIO_GINT_ID            23
/*!
  ethernet mac
  */
#define IRQ_EMAC_ID         24
/*!
  ethernet dma
  */
#define IRQ_EDMA_ID         25
/*!
  UART0
  */
#define IRQ_UART0_ID       26
/*!
  UART1
  */
#define IRQ_UART1_ID       27
/*!
  I2C0
  */
#define IRQ_I2C0_ID          28
/*!
  EPI
  */
#define IRQ_EPI_ID         29
/*!
  USB0
  */
#define IRQ_USB0_ID         30
/*!
  USB
  */
#define IRQ_USB1_ID         31
/*!
  SD_MMC0
  */
#define IRQ_SD_MMC0_ID         32
/*!
  SD_MMC1
  */
#define IRQ_SD_MMC1_ID         33
/*!
  GPIO_INT0
  */
#define IRQ_GPIO_INT0_ID         34
/*!
  GPIO_INT1
  */
#define IRQ_GPIO_INT1_ID         35
/*!
  I2C1 | I2C_HDMI_CFG
  */
#define IRQ_I2C1_ID         36
/*!
  I2C_QAM
  */
#define IRQ_I2C_QAM_ID         37
/*!
  I2C_FB
  */
#define IRQ_I2C_FB_ID         38
/*!
  LEDKB
  */
#define IRQ_LEDKB_ID         39
/*!
  IRDA
  */
#define IRQ_IRDA_ID         40
/*!
  Smart card 0
  */
#define IRQ_SMC0_ID         41
/*!
  Smart card 1
  */
#define IRQ_SMC1_ID         42
/*!
  SPI0
  */
#define IRQ_SPI0_ID          43
/*!
  SPI1
  */
#define IRQ_SPI1_ID          44
/*!
  SPI2
  */
#define IRQ_SPI2_ID          45


/*!
  Interrupt type, up to now only support normal
  */
#define IRQ_INT_TYPE_NORMAL     0

#endif