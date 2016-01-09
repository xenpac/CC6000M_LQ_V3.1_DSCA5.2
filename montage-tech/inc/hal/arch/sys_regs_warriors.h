/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_REGS_WARRIORS_H__
#define __SYS_REGS_WARRIORS_H__

#define WARRIORS_IO_PA    0x1fe00000
#define WARRIORS_IO_VA    0xbfe00000
#define WARRIORS_IO_SIZE    0x1f0000

#define UART_BASE_ADDR_0                            0xbff00000
#define R_UART_BAUD_0                               (UART_BASE_ADDR_0 + 0x0)
#define R_UART_CONCTL_0                             (UART_BASE_ADDR_0 + 0x4)
#define R_UART_DATA_0                               (UART_BASE_ADDR_0 + 0x8)
#define R_UART_STATUS_0                             (UART_BASE_ADDR_0 + 0xC)
#define R_UART_BAUD_EXT_0                           (UART_BASE_ADDR_0 + 0x10)


#define UART_BASE_ADDR_1                            0xbff10000
#define R_UART_BAUD_1                               (UART_BASE_ADDR_1 + 0x0)
#define R_UART_CONCTL_1                             (UART_BASE_ADDR_1 + 0x4)
#define R_UART_DATA_1                               (UART_BASE_ADDR_1 + 0x8)
#define R_UART_STATUS_1                             (UART_BASE_ADDR_1 + 0xC)
#define R_UART_BAUD_EXT_1                           (UART_BASE_ADDR_1 + 0x10)

#define M_UART_CR_RXCIE                           0x80
#define M_UART_CR_TXCIE                           0x40
#define M_UART_CR_UDRIE                           0x20
#define M_UART_CR_RXEN                            0x10
#define M_UART_CR_TXEN                            0x08
#define M_UART_CR_CHR9                            0x04
#define M_UART_CR_RXB8                            0x02
#define M_UART_CR_TXB8                            0x01
#define M_UART_SR_RXC                             0x80
#define M_UART_SR_TXC                             0x40
#define M_UART_SR_UDRE                            0x20
#define M_UART_SR_FE                              0x10
//B0
#define M_UART_B0_PEIE                            0x01
#define M_UART_B0_RXOVIE                          0x02
#define M_UART_B0_TXCIE                           0x04
#define M_UART_B0_RXCIE                           0x08
#define M_UART_B0_FEIE                            0x10
#define M_UART_B0_TONEIE                          0x20

#define M_UART_RX_FIFO_CLR                        0x40
#define M_UART_TX_FIFO_CLR                        0x80

#define WARRIORS_URRB (0x00)    /* Receive Buffer Register */
#define WARRIORS_URTH (0x04)    /* Transmit Holding Register */
#define WARRIORS_URFC (0x08)    /* FIFO Control Register */
#define WARRIORS_URLC (0x0C)    /* Line Control Register */
#define WARRIORS_URMC (0x10)    /* Modem Control Register */
#define WARRIORS_URLS (0x14)    /* Line Status Register */
#define WARRIORS_URMS (0x18)    /* Modem Status Register */
#define WARRIORS_URBD (0x1C)    /* Baud Rate Divisor Register */
#define WARRIORS_USR  (0x20)    /* Status Register */

/*****************************************************************
 * Definition of GPIO register
 *******************************************************************/
#define  GPIO_BASE_ADDR                           0xBFEE0000
#define  R_GPIO0_W_DATA_REG                       (GPIO_BASE_ADDR+0x00)
#define  R_GPIO0_W_DATA_EN_REG                    (GPIO_BASE_ADDR+0x04)
#define  R_GPIO0_R_DATA_REG                       (GPIO_BASE_ADDR+0x08)
#define  R_GPIO0_MASK_REG                         (GPIO_BASE_ADDR+0x0C)
#define  R_GPIO1_W_DATA_REG                       (GPIO_BASE_ADDR+0x10)
#define  R_GPIO1_W_DATA_EN_REG                    (GPIO_BASE_ADDR+0x14)
#define  R_GPIO1_R_DATA_REG                       (GPIO_BASE_ADDR+0x18)
#define  R_GPIO1_MASK_REG                         (GPIO_BASE_ADDR+0x1C)
#define  R_GPIO1_INT_CFG_REG                      (GPIO_BASE_ADDR+0x20)
#define  R_GPIO1_INT_STATUS_REG                   (GPIO_BASE_ADDR+0x14)

#define GPIO_INT_STATUS                           (GPIO_BASE_ADDR + 0x24)
#define GPIO_INT_MASK0                            (GPIO_BASE_ADDR + 0x0c)

/************************************************************************
 * Defination of UART Config Params
 ************************************************************************/
#define UART_CR_RXCIE           0x80
#define UART_CR_TXCIE           0x40
#define UART_CR_UDRIE           0x20
#define UART_CR_RXEN            0x10
#define UART_CR_TXEN            0x08
#define UART_CR_CHR9            0x04
#define UART_CR_RXB8            0x02
#define UART_CR_TXB8            0x01

/************************************************************************
 * Defination of UART status
 ************************************************************************/
#define UART_SR_RXC   0x80
#define UART_SR_TXC   0x40
#define UART_SR_UDRE  0x20
#define UART_SR_FE    0x10

//#define SYS_CPU_CLOCK_M     81
//#define SYS_CPU_CLOCK       (SYS_CPU_CLOCK_M * 1000000)
#define WARRIORS_CLOCK_TICK_RATE      (27 * 1000000)

#define PIC_BASE_ADDR                             0xbfe50000
#define R_PIC_MASK                        (PIC_BASE_ADDR+0x4)
#define R_PIC_VECTOR                        (PIC_BASE_ADDR+0x8)
#define R_PIC_COW0                        (PIC_BASE_ADDR+0xc)
#define  IRQ_MASK_REG        (PIC_BASE_ADDR+0x4)
#define  IRQ_VECT_REG         (PIC_BASE_ADDR+0x8)
#define  IRQ_COW0_REG         (PIC_BASE_ADDR+0xc)

#define IRQ_PIOR0_REG          (PIC_BASE_ADDR+0x10)
#define IRQ_PIOR1_REG          (PIC_BASE_ADDR+0x14)
#define IRQ_PIOR2_REG          (PIC_BASE_ADDR+0x48)
#define IRQ_PIOR3_REG          (PIC_BASE_ADDR+0x4C)
#define IRQ_PIOR4_REG          (PIC_BASE_ADDR+0x50)
#define IRQ_PIOR5_REG          (PIC_BASE_ADDR+0x54)
#define IRQ_PIOR6_REG          (PIC_BASE_ADDR+0x58)
#define IRQ_PIOR7_REG          (PIC_BASE_ADDR+0x5C)
#define IRQ_PIOR_COW1          (PIC_BASE_ADDR+0x20)

#define IRQ_TRIGER_POLARITY_REG    (PIC_BASE_ADDR+0X0)
#define IRQ_TRIGER_MODE_REG        (PIC_BASE_ADDR+0X60)

#ifndef PIC_BASE_ADDR_AV
#define PIC_BASE_ADDR_AV           0xbfd10000
#define R_PIC_MASK_AV              (PIC_BASE_ADDR_AV+0x4)
#define R_PIC_VECTOR_AV            (PIC_BASE_ADDR_AV+0x8)
#define R_PIC_COW0_AV              (PIC_BASE_ADDR_AV+0xc)
#define IRQ_MASK_REG_AV            (PIC_BASE_ADDR_AV+0x4)
#define IRQ_VECT_REG_AV            (PIC_BASE_ADDR_AV+0x8)
#define IRQ_COW0_REG_AV            (PIC_BASE_ADDR_AV+0xc)
#define IRQ_TRIGER_REG_AV        (PIC_BASE_ADDR_AV+0x0)

#endif


#define TIMER_BASE_ADDR                             PIC_BASE_ADDR
#define R_TIMER0_INIT_DATA                        (TIMER_BASE_ADDR+0x18)
#define R_TIMER1_INIT_DATA                        (TIMER_BASE_ADDR+0x1c)
#define R_TIMER2_INIT_DATA                        (TIMER_BASE_ADDR+0x28)
#define R_TIMER3_INIT_DATA                        (TIMER_BASE_ADDR+0x2c)
#define R_TIMER0_CUR_DATA                         (TIMER_BASE_ADDR+0x30)
#define R_TIMER1_CUR_DATA                         (TIMER_BASE_ADDR+0x34)
#define R_TIMER2_CUR_DATA                         (TIMER_BASE_ADDR+0x38)
#define R_TIMER3_CUR_DATA                         (TIMER_BASE_ADDR+0x3c)
#define R_TIMER_TCCW                                (TIMER_BASE_ADDR+0x24)

#define WATCHDOG_BASE_ADDR                      PIC_BASE_ADDR
#define R_WATCHDOG_DLY                          (WATCHDOG_BASE_ADDR+0x40)
#define R_WATCHDOG_CTRL                         (WATCHDOG_BASE_ADDR+0x44)
#if 0
/************** USB EHCI ******************/
#define USB_EHCI_MT_IO_OFFSET (0x100000)
#define USB_EHCI_MT_P_BASE (WARRIORS_IO_PA+USB_EHCI_MT_IO_OFFSET) // physical base
#define USB_EHCI_MT_REG_BASE (WARRIORS_IO_VA+USB_EHCI_MT_IO_OFFSET) //Vertual base: 0x1ff00000

#define USB_ID_REG (USB_EHCI_MT_REG_BASE+0)
#define USB_HWGENERAL_REG (USB_EHCI_MT_REG_BASE+0x04)
#define USB_HWHOST_REG (USB_EHCI_MT_REG_BASE+0x08)
#define USB_HWTXBUF_REG (USB_EHCI_MT_REG_BASE+0x10)
#define USB_HWRXBUF_REG (USB_EHCI_MT_REG_BASE+0x14)
#define USB_GPTIMER0LD_REG (USB_EHCI_MT_REG_BASE+0x80)
#define USB_GPTIMER0CTRL_REG (USB_EHCI_MT_REG_BASE+0x84)
#define USB_GPTIMER1LD_REG (USB_EHCI_MT_REG_BASE+0x88)
#define USB_GPTIMER1CTRL_REG (USB_EHCI_MT_REG_BASE+0x8C)

#define USB_HST_CAPABILITY_P_REG (USB_EHCI_MT_P_BASE+0x100) //physical addr
#define USB_HST_CAPABILITY_REG (USB_EHCI_MT_REG_BASE+0x100) // Vertual addr
#define USB_HCSPARAMS_REG (USB_HST_CAPABILITY_REG+0x04)
#define USB_HCCPARAMS_REG (USB_HST_CAPABILITY_REG+0x08)

#define USB_USBCMD_REG (USB_HST_CAPABILITY_REG+0x40)
#define USB_USBSTS_REG (USB_USBCMD_REG+0x04)
#define USB_USBINTR_REG (USB_USBCMD_REG+0x08)
#define USB_FRINDEX_REG (USB_USBCMD_REG+0x0C)
#define USB_PERIODICLISTBASE_REG (USB_USBCMD_REG+0x14)
#define USB_ASYNCLISTADDR_REG (USB_USBCMD_REG+0x18)
#define USB_BURSTSIZE_REG (USB_USBCMD_REG+0x20)
#define USB_TXFILLTUNING_REG (USB_USBCMD_REG+0x24)
#define USB_CONFIG_REG (USB_USBCMD_REG+0x40)
#define USB_PORTSCx_REG (USB_USBCMD_REG+0x44)
#define USB_USBMODE_REG (USB_USBCMD_REG+0x68)
#define USB_CAP_TO_OP_REG_OFFSET (USB_USBCMD_REG-USB_HST_CAPABILITY_REG)

#define USB_USBCMD_P_REG (USB_HST_CAPABILITY_P_REG+0x40)
#define USB_PORTSCx_P_REG (USB_USBCMD_P_REG+0x44)
#endif


/************** ddr control ******************/
#define R_DDR_CTRL_BASE 0xBFD70000
#define R_DDR_CTRL_DRAM_TYPE (R_DDR_CTRL_BASE + 0x34)
#define R_DDR_CTRL_TOUCH_TAG (R_DDR_CTRL_BASE + 0x60)
#define R_DDR_CTRL_TOUCH_CONDITION (R_DDR_CTRL_BASE + 0x64)
#define R_DDR_CTRL_EXPIRY_CONDITION (R_DDR_CTRL_BASE + 0x68)
#define R_DDR_CTRL_EXPIRY_STATUS (R_DDR_CTRL_BASE + 0x6C)
#define R_DDR_CTRL_MON_ENA (R_DDR_CTRL_BASE + 0x70)
#define R_DDR_CTRL_DEADLOCK_CONDITION (R_DDR_CTRL_BASE + 0x74)
#define R_DDR_CTRL_DEADLOCK_STATUS (R_DDR_CTRL_BASE + 0x78)
#define R_DDR_CTRL_WHOLE_THROUGHPUT (R_DDR_CTRL_BASE + 0x7C)
#define R_DDR_CTRL_MASTER_THROUGHPUT (R_DDR_CTRL_BASE + 0x80)
#define R_DDR_CTRL_MASTER_THROUGHPUT1 (R_DDR_CTRL_BASE + 0xC8)

/**************** smart card register ********************/
#define SMC_BASE_ADDR                             0xBFF40000

#define R_SMC_CONV                                (SMC_BASE_ADDR + 0x0)
#define R_SMC_PARITY                              (SMC_BASE_ADDR + 0x4)
#define R_SMC_TXRETRY                             (SMC_BASE_ADDR + 0x8)
#define R_SMC_RXRETRY                             (SMC_BASE_ADDR + 0xC)
#define R_SMC_TXTIDE                              (SMC_BASE_ADDR + 0x10)
#define R_SMC_TXCOUNT                             (SMC_BASE_ADDR + 0x14)
#define R_SMC_RXTIDE                              (SMC_BASE_ADDR + 0x18)
#define R_SMC_RXCOUNT                             (SMC_BASE_ADDR + 0x1C)
#define R_SMC_RXTIME                              (SMC_BASE_ADDR + 0x20)
#define R_SMC_TCTRL                               (SMC_BASE_ADDR + 0x24)
#define R_SMC_STABLE                              (SMC_BASE_ADDR + 0x28)
#define R_SMC_ICTRL                               (SMC_BASE_ADDR + 0x2C)
#define R_SMC_ISTAT                               (SMC_BASE_ADDR + 0x30)
#define R_SMC_ATIME                               (SMC_BASE_ADDR + 0x34)
#define R_SMC_DTIME                               (SMC_BASE_ADDR + 0x38)
#define R_SMC_ATR_STIME                           (SMC_BASE_ADDR + 0x3C)
#define R_SMC_ATR_DTIME                           (SMC_BASE_ADDR + 0x40)
#define R_SMC_BLKTIME                             (SMC_BASE_ADDR + 0x44)
#define R_SMC_CHTIME                              (SMC_BASE_ADDR + 0x48)
#define R_SMC_CLKDIV                              (SMC_BASE_ADDR + 0x4C)
#define R_SMC_FD                                  (SMC_BASE_ADDR + 0x50)
#define R_SMC_CONFIG                              (SMC_BASE_ADDR + 0x54)
#define R_SMC_CHGUARD                             (SMC_BASE_ADDR + 0x58)
#define R_SMC_BLKGUARD                            (SMC_BASE_ADDR + 0x5C)
#define R_SMC_RESET                               (SMC_BASE_ADDR + 0x60)
#define R_SMC_RAWSTAT                             (SMC_BASE_ADDR + 0x64)
#define R_SMC_INTEN                               (SMC_BASE_ADDR + 0x68)
#define R_SMC_INTSTAT                             (SMC_BASE_ADDR + 0x6C)
#define R_SMC_INQSTAT                             (SMC_BASE_ADDR + 0x70)
#define R_SMC_DATA                                (SMC_BASE_ADDR + 0x100)

/************************************************************************
 * Definition of NOR-FLASH register
 ************************************************************************/
#define NORF_BASE_ADDR                            0x90000000
#define NORF_BASE_ADDR_NON_CACHE                  (0xb0000000)
#define NORF_CTRL_BASE_ADDR                       0xbfe80000
#define R_NORF_SOFT_CTRL0                         (NORF_CTRL_BASE_ADDR + 0)
#define R_NORF_SOFT_CTRL1                         (NORF_CTRL_BASE_ADDR + 0x0c)
#define R_NORF_SOFT_DATA0                         (NORF_CTRL_BASE_ADDR + 0x04)
#define R_NORF_SOFT_DATA1                         (NORF_CTRL_BASE_ADDR + 0x08)
#define R_NORF_SPI_CTRL                           (NORF_CTRL_BASE_ADDR + 0x14)
#define R_NORF_SOFT_DUMMY0                        (NORF_CTRL_BASE_ADDR + 0x18)
#define R_NORF_SOFT_DUMMY1                        (NORF_CTRL_BASE_ADDR + 0x1c)
#define R_NORF_HW_WR_CTRL                         (NORF_CTRL_BASE_ADDR + 0x20)
#define R_NORF_HW_RD_CTRL                         (NORF_CTRL_BASE_ADDR + 0x24)
#define R_NORF_CHIP_ID_SEL                         (NORF_CTRL_BASE_ADDR + 0x28)

#define R_NORF_CMD_REL                            (NORF_CTRL_BASE_ADDR + 0x800C)

/************************************************************************
 * Definition of I2C Module
 ************************************************************************/
#define I2C_MASTER0_BASE_ADDR                             0xBFF20000
#define R_I2C_MASTER0_PRER_H                              (I2C_MASTER0_BASE_ADDR+0x18)
#define R_I2C_MASTER0_PRER_L                              (I2C_MASTER0_BASE_ADDR+0x1c)
#define R_I2C_MASTER0_TXR                                 (I2C_MASTER0_BASE_ADDR+0x08)
#define R_I2C_MASTER0_RXR                                 (I2C_MASTER0_BASE_ADDR+0x0c)
#define R_I2C_MASTER0_SR                                  (I2C_MASTER0_BASE_ADDR+0x04)
#define R_I2C_MASTER0_CTR                                 (I2C_MASTER0_BASE_ADDR+0x10)
#define R_I2C_MASTER0_CR                                  (I2C_MASTER0_BASE_ADDR+0x14)

#define I2C_MASTER1_BASE_ADDR                             0xBFF30000
#define R_I2C_MASTER1_PRER_H                              (I2C_MASTER1_BASE_ADDR+0x18)
#define R_I2C_MASTER1_PRER_L                              (I2C_MASTER1_BASE_ADDR+0x1c)
#define R_I2C_MASTER1_TXR                                 (I2C_MASTER1_BASE_ADDR+0x08)
#define R_I2C_MASTER1_RXR                                 (I2C_MASTER1_BASE_ADDR+0x0c)
#define R_I2C_MASTER1_SR                                  (I2C_MASTER1_BASE_ADDR+0x04)
#define R_I2C_MASTER1_CTR                                 (I2C_MASTER1_BASE_ADDR+0x10)
#define R_I2C_MASTER1_CR                                  (I2C_MASTER1_BASE_ADDR+0x14)

/************************************************************************
      definition of DMA register
**************************************************/
#define   DMA_BASE_ADDR                                     0xBFE30000
#define   SG_DMA_START_DESCRIPTOR                 (DMA_BASE_ADDR + 0x0)
#define   SG_DMA_SRC_STARTADD                        (DMA_BASE_ADDR + 0x4)
#define   SG_DMA_DST_STARTADD                        (DMA_BASE_ADDR + 0x8)
#define   SG_DMA_DST_BLKPARA0                         (DMA_BASE_ADDR + 0xc)
#define   SG_DMA_DST_BLKPARA1                         (DMA_BASE_ADDR + 0x10)
#define   SG_DMA_ALU_DWORD                              (DMA_BASE_ADDR + 0x14)
#define   SG_DMA_CMD_OPCODE                            (DMA_BASE_ADDR + 0x18)
#define   SG_DMA_NEXT_LINK_DESCRIPTOR          (DMA_BASE_ADDR + 0x1c)
#define   SG_DMA_INTCFG_ADD                        (DMA_BASE_ADDR + 0x8c)
#define   SG_DMA_SET_ADD                        (DMA_BASE_ADDR + 0xf8)

#define  SG_AES_CONFIG                            (DMA_BASE_ADDR + 0x20)
#define  SG_AES_DES_KEY0                          (DMA_BASE_ADDR + 0x24)
#define  SG_AES_DES_KEY1                          (DMA_BASE_ADDR + 0x28)
#define  SG_AES_DES_KEY2                          (DMA_BASE_ADDR + 0x2c)
#define  SG_AES_DES_KEY3                          (DMA_BASE_ADDR + 0x30)
#define  SG_AES_DES_KEY4                          (DMA_BASE_ADDR + 0x34)
#define  SG_AES_DES_KEY5                          (DMA_BASE_ADDR + 0x38)
#define  SG_AES_DES_KEY6                          (DMA_BASE_ADDR + 0x3c)
#define  SG_AES_DES_KEY7                          (DMA_BASE_ADDR + 0x40)
#define  SG_AES_VECTOR_0                          (DMA_BASE_ADDR + 0x44)
#define  SG_AES_VECTOR_1                          (DMA_BASE_ADDR + 0x48)
#define  SG_AES_VECTOR_2                          (DMA_BASE_ADDR + 0x4c)
#define  SG_AES_VECTOR_3                          (DMA_BASE_ADDR + 0x50)
#define SG_DES_CONFIG                             (DMA_BASE_ADDR + 0x54)

/************************************************************
 *EDMA Register definition
 ************************************************************/
#define EDMA_REG_BASE                   0xbfeb0000
#define EDMA_TX_CTRL                   (EDMA_REG_BASE+0x0)
#define EDMA_TX_BUFFER_BASE             (EDMA_REG_BASE+0x4)
#define EDMA_TX_BUFFER_SIZE             (EDMA_REG_BASE+0x8)
#define EDMA_TX_PUT_ID                 (EDMA_REG_BASE+0xc)
#define EDMA_TX_GET_ID                 (EDMA_REG_BASE+0x10)
#define EDMA_TX_CUR_RD_OFF             (EDMA_REG_BASE+0x14)
#define EDMA_TX_STATUS                 (EDMA_REG_BASE+0x18)
#define EDMA_MAX_PK_SIZE               (EDMA_REG_BASE+0x20)
#define EDMA_MIN_PK_SIZE               (EDMA_REG_BASE+0x24)
#define EDMA_RX_CTRL                   (EDMA_REG_BASE+0x40)
#define EDMA_RX_INFO_BUFFER_BASE       (EDMA_REG_BASE+0x44)
#define EDMA_RX_INFO_BUFFER_SIZE       (EDMA_REG_BASE+0x48)
#define EDMA_RX_DATA_BUFFER_BASE       (EDMA_REG_BASE+0x4c)
#define EDMA_RX_DATA_BUFFER_SIZE       (EDMA_REG_BASE+0x50)
#define EDMA_RX_PUT_ID                 (EDMA_REG_BASE+0x54)
#define EDMA_RX_INFO_RD_OFF             (EDMA_REG_BASE+0x58)
#define EDMA_RX_INFO_WR_OFF             (EDMA_REG_BASE+0x5c)
#define EDMA_RX_DATA_RD_OFF             (EDMA_REG_BASE+0x60)
#define EDMA_RX_DATA_WR_OFF             (EDMA_REG_BASE+0x64)
#define EDMA_RX_THRESHOLD               (EDMA_REG_BASE+0x68)
#define EDMA_INT_SOURCE                 (EDMA_REG_BASE+0x6c)
#define EDMA_IRQ_STATUS                 (EDMA_REG_BASE+0x78)

/************************************************************************
      EMAC Register definition
**************************************************/
#define EMAC_REG_BASE                   0xbfea0000
#define EMAC_ADDR0                     (EMAC_REG_BASE+0x0)
#define EMAC_ADDR1                     (EMAC_REG_BASE+0x4)
#define EMAC_ADDR2                     (EMAC_REG_BASE+0x8)
#define EMAC_ADDR3                     (EMAC_REG_BASE+0xc)
#define EMAC_ADDR4                     (EMAC_REG_BASE+0x10)
#define EMAC_ADDR5                     (EMAC_REG_BASE+0x14)
#define EMAC_ADDR6                     (EMAC_REG_BASE+0x18)
#define EMAC_ADDR7                     (EMAC_REG_BASE+0x1c)
#define EMAC_ADDR8                     (EMAC_REG_BASE+0x20)
#define EMAC_ADDR9                     (EMAC_REG_BASE+0x24)
#define EMAC_ADDR10                     (EMAC_REG_BASE+0x28)
#define EMAC_ADDR11                     (EMAC_REG_BASE+0x2c)
#define EMAC_CTRL                       (EMAC_REG_BASE+0x30)
#define EMAC_FILTER_CTRL               (EMAC_REG_BASE+0x34)
#define EMAC_IRQ_STATUS                 (EMAC_REG_BASE+0x38)
#define EMAC_IRQ_CONTROL               (EMAC_REG_BASE+0x3c)
#define EMAC_RX_STATUS                 (EMAC_REG_BASE+0x40)
#define EMAC_TX_STATUS                 (EMAC_REG_BASE+0x44)
#define EMAC_FLOW_CTRL                 (EMAC_REG_BASE+0x48)
#define EMAC_SMI_CTRL                   (EMAC_REG_BASE+0x4c)
#define EMAC_SMI_COMM                   (EMAC_REG_BASE+0x50)
#define EMAC_PAUSE_PAT0                 (EMAC_REG_BASE+0x54)
#define EMAC_PAUSE_PAT1                 (EMAC_REG_BASE+0x58)
#define EMAC_PAUSE_PAT2                 (EMAC_REG_BASE+0x5c)
#define EMAC_RXDROP_CNT                 (EMAC_REG_BASE+0x60)
#define EMAC_TXDONE_CNT                 (EMAC_REG_BASE+0x64)
#define EMAC_TXDROP_CNT                 (EMAC_REG_BASE+0x68)

/************************************************************
 *LEDKB Register definition
 ************************************************************/
#define  LEDKB_BASS_ADRESS          0xbfed8000
#define  LEDKB_IC_CONFIG      (LEDKB_BASS_ADRESS + 0x00)
#define  LEDKB_CLK_DIVIDER    (LEDKB_BASS_ADRESS + 0x04)
#define  LEDKB_INT_STATUS     (LEDKB_BASS_ADRESS + 0x08)
#define  LEDKB_INT_ENABLE     (LEDKB_BASS_ADRESS + 0x0c)
#define  LEDKB_MCU_COMMAND    (LEDKB_BASS_ADRESS + 0x10)
#define  LEDKB_MCU_STATUS     (LEDKB_BASS_ADRESS + 0x14)
#define  LEDKB_MCU_TX_DATA    (LEDKB_BASS_ADRESS + 0x18)
#define  LEDKB_MCU_RX_DATA    (LEDKB_BASS_ADRESS + 0x1c)
#define  LEDKB_LED_SET         (LEDKB_BASS_ADRESS + 0x20)

#define  LEDKB_LED_SETTING     (LEDKB_BASS_ADRESS + 0x20)
#define  LEDKB_KEY_SETTING     (LEDKB_BASS_ADRESS + 0x24)

#define  LEDKB_KEY_SET          (LEDKB_BASS_ADRESS + 0x24)
#define  LEDKB_KEY_SCAN_DELAY (LEDKB_BASS_ADRESS + 0x28)
#define  LEDKB_KEY_VALUE        (LEDKB_BASS_ADRESS + 0x2c)
#define  LEDKB_LED_GRID0        (LEDKB_BASS_ADRESS + 0x30)
#define  LEDKB_LED_GRID1        (LEDKB_BASS_ADRESS + 0x34)
#define  LEDKB_LED_GRID2        (LEDKB_BASS_ADRESS + 0x38)
#define  LEDKB_LED_GRID3        (LEDKB_BASS_ADRESS + 0x3c)
#define  LEDKB_LED_GRID(i)      (LEDKB_BASS_ADRESS + 0x30 + (i) * 4)

/************************************************************
 *SD card Register definition
 ************************************************************/
#define SDCARD_REG_BASE                   0xbfe40000
#define SDCARD_ARGUMENT                  (SDCARD_REG_BASE + 0x00)
#define SDCARD_CMD_SETTING             (SDCARD_REG_BASE + 0x04)
#define SDCARD_STATUS                       (SDCARD_REG_BASE + 0x08)
#define SDCARD_CTRL_SETTING            (SDCARD_REG_BASE + 0x0c)
#define SDCARD_RESPONSE(i)                (SDCARD_REG_BASE + 0x10 + (i * 4))
#define SDCARD_BLOCK_SIZE                (SDCARD_REG_BASE + 0x20)
#define SDCARD_POWER_CTRL               (SDCARD_REG_BASE + 0x24)
#define SDCARD_SOFT_RESET                (SDCARD_REG_BASE + 0x28)
#define SDCARD_TIMEOUT                     (SDCARD_REG_BASE + 0x2c)
#define SDCARD_NORMAL_ISR               (SDCARD_REG_BASE + 0x30)
#define SDCARD_ERROR_ISR                  (SDCARD_REG_BASE + 0x34)
#define SDCARD_NORMAL_IER               (SDCARD_REG_BASE + 0x38)
#define SDCARD_ERROR_IER                  (SDCARD_REG_BASE + 0x3c)
#define SDCARD_CAPABILITY                (SDCARD_REG_BASE + 0x48)
#define SDCARD_CLOCK_DIVIDER          (SDCARD_REG_BASE + 0x4c)
#define SDCARD_SYS_MEM_ADDR                 (SDCARD_REG_BASE + 0x50)
#define SDCARD_CARD_ADDR                (SDCARD_REG_BASE + 0x54)
#define SDCARD_DATA_SETTING                         (SDCARD_REG_BASE + 0x58)
//#define SDCARD_BD_RX                         (SDCARD_REG_BASE + 0x70)


/*******************************************************************
 * Definition of IRDA register
 *******************************************************************/
#define IRDA_BASE_ADDR                            0xbfed4000
#define IRDA_NEC_DATA                           (IRDA_BASE_ADDR + 0x0)
#define IRDA_NEC_CONFIG                       (IRDA_BASE_ADDR + 0x4)
#define IRDA_NEC_STATUS                       (IRDA_BASE_ADDR + 0x8)
#define R_IRDA_CTL                      (IRDA_BASE_ADDR + 0xc)
#define IRDA_GLOBAL_CONF                     (IRDA_BASE_ADDR + 0x10)
#define IRDA_GLOBAL_OTSET                    (IRDA_BASE_ADDR + 0x14)
#define IRDA_GLOBAL_STA                        (IRDA_BASE_ADDR + 0x18)
#define IRDA_COM_DATA                           (IRDA_BASE_ADDR + 0x1c)

/*******************************************************************
 * Definition of sysctrl register
 *******************************************************************/
#define SYSCTRL_BASE_ADDR                            0xbff70000
#define R_SYSCTRL_SOFT_RESET                        (SYSCTRL_BASE_ADDR + 0x04)
#define SYSCTRL_CHIP_REV                            (SYSCTRL_BASE_ADDR + 0x30)
#define SYSCTRL_BLOCK_RESET                         (SYSCTRL_BASE_ADDR + 0x08)
#define SYSCTRL_FUNC_PORT_EN                         (SYSCTRL_BASE_ADDR + 0x18)
#define SYSCTRL_ADAC_CFG1                           (SYSCTRL_BASE_ADDR + 0xb0)
/*******************************************************************
 * Definition of mcu2ap register
 *******************************************************************/
#define ROM_BASE_ADDR 0xbfed0000
#define MCU_SOFT_RESET 0xbfedc038
#define LED_PIN_ENABLE 0xbfedc034

#define MCU2AP_DATA0 0xbfedc014
#define MCU2AP_DATA1 0xbfedc018
#define MCU2AP_DATA2 0xbfedc01c
#define MCU2AP_DATA3 0xbfedc020
#define MCU2AP_DATA4 0xbfedc060
#define MCU2AP_DATA5 0xbfedc064


#define R_FPGPIO_OUT_EN_REG 0xbfedc028
#define R_FPGPIO_OUT_REG 0xbfedc02c
#define R_FPGPIO_IN_REG 0xbfedc030
#define R_FPGPIO_MASK_REG 0xbfedc034

#endif //__SYS_REGS_WARRIORS_H__


