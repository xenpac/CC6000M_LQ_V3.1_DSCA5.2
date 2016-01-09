/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_REGS_SONATA_H__
#define __SYS_REGS_SONATA_H__

/*!
  Definition ddr control register
  */
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_BASE                             0xBFD70000
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_CFG_LOCK                         (R_DDR_CTRL_BASE + 0x200)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_TOUCH_TAG                        (R_DDR_CTRL_BASE + 0x210)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_TOUCH_CONDITION                  (R_DDR_CTRL_BASE + 0x20c)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_EXPIRY_CONDITION                 (R_DDR_CTRL_BASE + 0x214)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_EXPIRY_STATUS                    (R_DDR_CTRL_BASE + 0x218)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_MON_ENA                          (R_DDR_CTRL_BASE + 0x21c)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_DEADLOCK_CONDITION               (R_DDR_CTRL_BASE + 0x220)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_DEADLOCK_STATUS                  (R_DDR_CTRL_BASE + 0x224)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_WHOLE_THROUGHPUT                 (R_DDR_CTRL_BASE + 0x240)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_MASTER_THROUGHPUT                (R_DDR_CTRL_BASE + 0x244)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_MASTER_THROUGHPUT1               (R_DDR_CTRL_BASE + 0x248)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_CFG_INSPECT_MASTERS              (R_DDR_CTRL_BASE + 0x25c)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_CFG_WR_PROTECT                   (R_DDR_CTRL_BASE + 0x228)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_WR_PROTECT_START_ADDR            (R_DDR_CTRL_BASE + 0x22c)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_WR_PROTECT_END_ADDR              (R_DDR_CTRL_BASE + 0x230)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_CFG_RD_PROTECT                   (R_DDR_CTRL_BASE + 0x234)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_RD_PROTECT_START_ADDR            (R_DDR_CTRL_BASE + 0x238)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_DDR_CTRL_RD_PROTECT_END_ADDR              (R_DDR_CTRL_BASE + 0x23c)


#define R_DDR_CTRL_DRAM_TYPE (R_DDR_CTRL_BASE + 0x34)

/*!
  AP CPU REGISTERS
  */
#define AP_CPU_REGISTERS_BASE                       0xBFE70000

/*!
  SPIN LOCK GROUPS ADDRESS
  */
#define SPIN_LOCK_ADDRESS                           (AP_CPU_REGISTERS_BASE + 0x30)

/*!
  Definition ddr sysctrl register
  */
/*!
  REGISTER ADDRESS DEFINITION
  */
#define SYSCTRL_BASE_ADDR                           0xbff70000
/*!
  REGISTER ADDRESS DEFINITION
  */
#define R_SYSCTRL_SOFT_RESET                        (SYSCTRL_BASE_ADDR + 0x04)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define SYSCTRL_CHIP_REV                            (SYSCTRL_BASE_ADDR + 0x30)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define SYSCTRL_BLOCK_RESET                         (SYSCTRL_BASE_ADDR + 0x08)
/*!
  REGISTER ADDRESS DEFINITION
  */
#define SYSCTRL_FUNC_PORT_EN                        (SYSCTRL_BASE_ADDR + 0x18)

#define SYSCTRL_HDMI_TX_CFG_0                        (SYSCTRL_BASE_ADDR + 0xE4)
#define SYSCTRL_HDMI_TX_CFG_1                        (SYSCTRL_BASE_ADDR + 0xE8)

#define UART_BASE_ADDR_0                            0xbff00000
#define R_UART_BAUD_0                               (UART_BASE_ADDR_0 + 0x0)
#define R_UART_CONCTL_0                             (UART_BASE_ADDR_0 + 0x4)
#define R_UART_DATA_0                               (UART_BASE_ADDR_0 + 0x8)
#define R_UART_STATUS_0                             (UART_BASE_ADDR_0 + 0xC)
#define R_UART_BAUD_EXT_0                           (UART_BASE_ADDR_0 + 0x10)
//B0
#define R_UART_UIE_0                                (UART_BASE_ADDR_0 + 0x14)
#define R_UART_UFC_0                                (UART_BASE_ADDR_0 + 0x18)
#define R_UART_RFC_0                                (UART_BASE_ADDR_0 + 0x1C)

#define UART_BASE_ADDR_1                            0xbff10000
#define R_UART_BAUD_1                               (UART_BASE_ADDR_1 + 0x0)
#define R_UART_CONCTL_1                             (UART_BASE_ADDR_1 + 0x4)
#define R_UART_DATA_1                               (UART_BASE_ADDR_1 + 0x8)
#define R_UART_STATUS_1                             (UART_BASE_ADDR_1 + 0xC)
#define R_UART_BAUD_EXT_1                           (UART_BASE_ADDR_1 + 0x10)
//B0
#define R_UART_UIE_1                                (UART_BASE_ADDR_1 + 0x14)
#define R_UART_UFC_1                                (UART_BASE_ADDR_1 + 0x18)
#define R_UART_RFC_1                                (UART_BASE_ADDR_1 + 0x1C)



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
#define  GPIO_BASE_ADDR                           0xBFF50000
#define  R_GPIO0_W_DATA_REG                       (GPIO_BASE_ADDR+0x00)
#define  R_GPIO0_W_DATA_EN_REG                    (GPIO_BASE_ADDR+0x04)
#define  R_GPIO0_R_DATA_REG                       (GPIO_BASE_ADDR+0x08)
#define  R_GPIO0_MASK_REG                         (GPIO_BASE_ADDR+0x0C)
//#define  R_GPIO1_W_DATA_REG                       (GPIO_BASE_ADDR+0x10)
//#define  R_GPIO1_W_DATA_EN_REG                    (GPIO_BASE_ADDR+0x14)
//#define  R_GPIO1_R_DATA_REG                       (GPIO_BASE_ADDR+0x18)
//#define  R_GPIO1_MASK_REG                         (GPIO_BASE_ADDR+0x1C)
//#define  R_GPIO1_INT_STATUS_REG                   (GPIO_BASE_ADDR+0x14)
#define  R_GPIO_INT_CFG_REG                      (GPIO_BASE_ADDR+0x20)
#define GPIO_INT_STATUS                           (GPIO_BASE_ADDR + 0x24)
#define GPIO_INT_MASK0                            (GPIO_BASE_ADDR + 0x0c)


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
#define   SG_DMA_MSN_STATUS_ADD                        (DMA_BASE_ADDR + 0x90)
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
 * Definition of SPI BUS register
 ************************************************************************/
/*!
  Definition of SPI register
  */
#define SPIN_BASE_ADDR                          0xBFEE0000
#define R_SPIN_BAUD                             ((SPIN_BASE_ADDR) + (0x08))
#define R_SPIN_MODE_CFG                         ((SPIN_BASE_ADDR) + (0x0c))
#define R_SPIN_INT_CFG                          ((SPIN_BASE_ADDR) + (0x10))
#define R_SPIN_TC                               ((SPIN_BASE_ADDR) + (0x14))
#define R_SPIN_CTRL                             ((SPIN_BASE_ADDR) + (0x18))
#define R_SPIN_STA                              ((SPIN_BASE_ADDR) + (0x1c))
#define R_SPIN_PIN_CFG_IO                       ((SPIN_BASE_ADDR) + (0x20))
#define R_SPIN_MEM_BOOT                         ((SPIN_BASE_ADDR) + (0x24))
#define R_SPIN_PIN_MODE                         ((SPIN_BASE_ADDR) + (0x2c))
#define R_SPIN_DEBUG                            ((SPIN_BASE_ADDR) + (0x30))
#define R_SPIN_MOSI_CTRL                        ((SPIN_BASE_ADDR) + (0x34))
#define R_SPIN_DELAY_CFG                        ((SPIN_BASE_ADDR) + (0x38))
#define R_SPIN_INT_STA                          ((SPIN_BASE_ADDR) + (0x3c))
#define R_SPIN_TXD                              ((SPIN_BASE_ADDR) + (0x100))
#define R_SPIN_RXD                              ((SPIN_BASE_ADDR) + (0x100))
/************************************************************
 *EDMA Register definition
 ************************************************************/
#define EDMA_REG_BASE                   0xbfea0080
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
#define MCU2AP_DATA6 0xbfedc068
#define MCU2AP_DATA7 0xbfedc06c
#define MCU2AP_DATA8 0xbfedc070
#define MCU2AP_DATA9 0xbfedc074
#define MCU2AP_DATA10 0xbfedc078
#define MCU2AP_DATA11 0xbfedc07c


#define R_FPGPIO_OUT_EN_REG 0xbfedc028
#define R_FPGPIO_OUT_REG 0xbfedc02c
#define R_FPGPIO_IN_REG 0xbfedc030
#define R_FPGPIO_MASK_REG 0xbfedc034


/**
 * @brief   advanced ca module register
 *
 *  A data structure used to describe register's name and offset.
 */
enum	_EN_ADVANCEDCA_REGISTER
{
  // system control
  SYSCTRL_BASE	      = 0xBFF70000,
  REG_BLOCK_RESET     = SYSCTRL_BASE + 0x0008,

  // OTP module keys
  OTP_BASE	      = 0xBFF60000,
  REG_OTP_FLAG	      = OTP_BASE + 0x08,
  REG_OTP_SECUREKEY0  = OTP_BASE + 0x0C,
  REG_OTP_SECUREKEY1  = OTP_BASE + 0x10,
  REG_OTP_SECUREKEY2  = OTP_BASE + 0x14,
  REG_OTP_SECUREKEY3  = OTP_BASE + 0x18,
  REG_OTP_PVRKEY0     = OTP_BASE + 0x24,
  REG_OTP_PVRKEY1     = OTP_BASE + 0x28,
  REG_OTP_PVRKEY2     = OTP_BASE + 0x2C,
  REG_OTP_PVRKEY3     = OTP_BASE + 0x30,
  REG_OTP_HDMIKEY     = OTP_BASE + 0x34,

  // PTI module control
  PTI_BASE	      = 0xBFEF0000,
  REG_PTI_SLOT	      = PTI_BASE,
  REG_PTI_ODDKEY0     = PTI_BASE + 0x0600,
  REG_PTI_ODDKEY1     = PTI_BASE + 0x0604,
  REG_PTI_EVENKEY0    = PTI_BASE + 0x0608,
  REG_PTI_EVENKEY1    = PTI_BASE + 0x060C,
  REG_PTI_IV0	      = PTI_BASE + 0x1600,
  REG_PTI_IV1	      = PTI_BASE + 0x1604,
  REG_PTI_IV2	      = PTI_BASE + 0x1608,
  REG_PTI_IV3	      = PTI_BASE + 0x160C,
  REG_PTI_DEBUG	      = PTI_BASE + 0x0774,

  // SG_DMA module register
  ADVANCEDCA_BASE     = 0xBFE30000,
  // registers for CW decryption
  REG_MNG_CTL	      = ADVANCEDCA_BASE + 0x0080,
  REG_DATA_FILE_CTL   = ADVANCEDCA_BASE + 0x0084,
  REG_ENG_CTL	      = ADVANCEDCA_BASE + 0x0088,
  REG_IRQ_CTL	      = ADVANCEDCA_BASE + 0x008C,
  REG_MNG_STATUS      = ADVANCEDCA_BASE + 0x0090,
  REG_KEY_CTL	      = ADVANCEDCA_BASE + 0x0094,
  REG_PTI_CTL	      = ADVANCEDCA_BASE + 0x0098,
  REG_LIMIT_CTL	      = ADVANCEDCA_BASE + 0x009C,
  REG_K_DEBUG	      = ADVANCEDCA_BASE + 0x00A0,
  REG_K_LOAD	      = ADVANCEDCA_BASE + 0x00A4,
  REG_PVR_PATCH	      = ADVANCEDCA_BASE + 0x00A8,
  REG_K_DEBUG1	      = ADVANCEDCA_BASE + 0x0170,

  REG_PTI_AES_CFG     = ADVANCEDCA_BASE + 0x00C0,
  REG_PTI_DES_CFG     = ADVANCEDCA_BASE + 0x00D4,
  REG_K_AES_CFG	      = ADVANCEDCA_BASE + 0x00D8,
  REG_K_DES_CFG	      = ADVANCEDCA_BASE + 0x00EC,

  REG_K_IV0	      = ADVANCEDCA_BASE + 0x00DC,
  REG_K_IV1	      = ADVANCEDCA_BASE + 0x00E0,
  REG_K_IV2	      = ADVANCEDCA_BASE + 0x00E4,
  REG_K_IV3	      = ADVANCEDCA_BASE + 0x00E8,

  REG_JTAG_KEY0	      = ADVANCEDCA_BASE + 0x00F0,
  REG_JTAG_KEY1	      = ADVANCEDCA_BASE + 0x00F4,

  REG_K_DATBLK0_0     = ADVANCEDCA_BASE + 0x0100,
  REG_K_DATBLK0_1     = ADVANCEDCA_BASE + 0x0104,
  REG_K_DATBLK0_2     = ADVANCEDCA_BASE + 0x0108,
  REG_K_DATBLK0_3     = ADVANCEDCA_BASE + 0x010C,
  REG_K_DATBLK1_0     = ADVANCEDCA_BASE + 0x0110,
  REG_K_DATBLK1_1     = ADVANCEDCA_BASE + 0x0114,
  REG_K_DATBLK1_2     = ADVANCEDCA_BASE + 0x0118,
  REG_K_DATBLK1_3     = ADVANCEDCA_BASE + 0x011C,
  REG_K_DATBLK2_0     = ADVANCEDCA_BASE + 0x0120,
  REG_K_DATBLK2_1     = ADVANCEDCA_BASE + 0x0124,
  REG_K_DATBLK2_2     = ADVANCEDCA_BASE + 0x0128,
  REG_K_DATBLK2_3     = ADVANCEDCA_BASE + 0x012C,
  REG_K_DATBLK3_0     = ADVANCEDCA_BASE + 0x0130,
  REG_K_DATBLK3_1     = ADVANCEDCA_BASE + 0x0134,
  REG_K_DATBLK3_2     = ADVANCEDCA_BASE + 0x0138,
  REG_K_DATBLK3_3     = ADVANCEDCA_BASE + 0x013C,
  REG_K_DATBLK4_0     = ADVANCEDCA_BASE + 0x0140,
  REG_K_DATBLK4_1     = ADVANCEDCA_BASE + 0x0144,
  REG_K_DATBLK4_2     = ADVANCEDCA_BASE + 0x0148,
  REG_K_DATBLK4_3     = ADVANCEDCA_BASE + 0x014C,

  // registers for general-purpose engine
  REG_GP_AES_CONFIG   = ADVANCEDCA_BASE + 0x0020,
  REG_GP_KEY0	      = ADVANCEDCA_BASE + 0x0024,
  REG_GP_KEY1	      = ADVANCEDCA_BASE + 0x0028,
  REG_GP_KEY2	      = ADVANCEDCA_BASE + 0x002c,
  REG_GP_KEY3	      = ADVANCEDCA_BASE + 0x0030,
  REG_GP_KEY4	      = ADVANCEDCA_BASE + 0x0034,
  REG_GP_KEY5	      = ADVANCEDCA_BASE + 0x0038,
  REG_GP_KEY6	      = ADVANCEDCA_BASE + 0x003c,
  REG_GP_KEY7	      = ADVANCEDCA_BASE + 0x0040,
  REG_GP_IV0	      = ADVANCEDCA_BASE + 0x0044,
  REG_GP_IV1	      = ADVANCEDCA_BASE + 0x0048,
  REG_GP_IV2	      = ADVANCEDCA_BASE + 0x004c,
  REG_GP_IV3	      = ADVANCEDCA_BASE + 0x0050,
  REG_GP_DES_CONFIG   = ADVANCEDCA_BASE + 0x0054,
  REG_GP_AES_DEBUG    = ADVANCEDCA_BASE + 0x0064,
};
typedef enum	_EN_ADVANCEDCA_REGISTER	  EN_ADVANCEDCA_REGISTER;


#endif //__SYS_REGS_SONATA_H__


