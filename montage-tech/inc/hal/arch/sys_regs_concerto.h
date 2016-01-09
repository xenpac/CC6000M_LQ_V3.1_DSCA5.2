/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_REGS_CONCERTO_H__
#define __SYS_REGS_CONCERTO_H__

/************************************************************************
 * Defination of Reset register
 ************************************************************************/
/*!
  comments
  */
#define R_RST_REQ(n)                              (0xBF510000 + (n) * (0x4))
/*!
  comments
  */
#define R_RST_REQ_AO                            0xBF150040
/*!
  comments
  */
#define R_RST_CTRL(n)                             (0xBF510010 + (n) * (0x4))
/*!
  comments
  */
#define R_RST_CTRL_AO                            0xBF150044
/*!
  comments
  */
#define R_RST_ALLOW(n)                          (0xBF510020 + (n) * (0x4))
/*!
  comments
  */
#define R_RST_ALLOW_AO                        0xBF150048

/************************************************************************
 * Defination of UART register
 ************************************************************************/
/*!
  comments
  */
#define UART_BASE_ADDR                            0xbf540000
/*!
  comments
  */
#define R_UART_BAUD                               0xbf500040
/*!
  comments
  */
#define R_UART_CONCTL                             (UART_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_UART_DATA                               (UART_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_UART_STATUS                             (UART_BASE_ADDR + 0xC)

/*!
  comments
  */
#define UART1_BASE_ADDR                           0xbf550000
/*!
  comments
  */
#define R_UART1_BAUD                              0xbf500044
/*!
  comments
  */
#define R_UART1_CONCTL                            (UART1_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_UART1_DATA                              (UART1_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_UART1_STATUS                            (UART1_BASE_ADDR + 0xC)

/************************************************************************
 * Defination of UART Config Params 
 ************************************************************************/
/*!
  comments
  */
#define UART_CR_RXCIE           0x80
/*!
  comments
  */
#define UART_CR_TXCIE           0x40
/*!
  comments
  */
#define UART_CR_UDRIE           0x20
/*!
  comments
  */
#define UART_CR_RXEN            0x10
/*!
  comments
  */
#define UART_CR_TXEN            0x08
/*!
  comments
  */
#define UART_CR_CHR9            0x04
/*!
  comments
  */
#define UART_CR_RXB8            0x02
/*!
  comments
  */
#define UART_CR_TXB8            0x01

/************************************************************************ 
 * Defination of UART status 
 ************************************************************************/
/*!
  comments
  */
#define UART_SR_RXC   0x80
/*!
  comments
  */
#define UART_SR_TXC   0x40
/*!
  comments
  */
#define UART_SR_UDRE  0x20
/*!
  comments
  */
#define UART_SR_FE    0x10

/************************************************************************ 
 * PIC Controller
 ************************************************************************/
/*!
  comments
  */
#define M_PIC_BASE_ADDR           0xbf100000
/*!
  comments
  */
#define R_M_PIC_INT_MODE0       (M_PIC_BASE_ADDR + 0x0)
/*!
  comments
  */
#define R_M_PIC_INT_MODE1       (M_PIC_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_M_PIC_INT_MODE2       (M_PIC_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_M_PIC_INT_MODE3       (M_PIC_BASE_ADDR + 0xc)
/*!
  comments
  */
#define R_M_PIC_INT_MODE4       (M_PIC_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_M_PIC_INT_MODE5       (M_PIC_BASE_ADDR + 0x14)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR0       (M_PIC_BASE_ADDR + 0x20)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR1       (M_PIC_BASE_ADDR + 0x24)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR2       (M_PIC_BASE_ADDR + 0x28)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR3       (M_PIC_BASE_ADDR + 0x2c)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR4       (M_PIC_BASE_ADDR + 0x30)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR5       (M_PIC_BASE_ADDR + 0x34)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR6       (M_PIC_BASE_ADDR + 0x38)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR7       (M_PIC_BASE_ADDR + 0x3c)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR8       (M_PIC_BASE_ADDR + 0x40)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR9       (M_PIC_BASE_ADDR + 0x44)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR10       (M_PIC_BASE_ADDR + 0x48)
/*!
  comments
  */
#define R_M_PIC_INT_PRIOR11       (M_PIC_BASE_ADDR + 0x4c)
/*!
  comments
  */
#define R_M_PIC_INT_MASK0       (M_PIC_BASE_ADDR + 0x60)
/*!
  comments
  */
#define R_M_PIC_INT_MASK1       (M_PIC_BASE_ADDR + 0x64)
/*!
  comments
  */
#define R_M_PIC_INT_NUM      (M_PIC_BASE_ADDR + 0x68)
/*!
  comments
  */
#define R_M_PIC_INT_EOI      (M_PIC_BASE_ADDR + 0x6c)

/*!
  comments
  */
#define R_M_PIC_TI_INIT0      (M_PIC_BASE_ADDR + 0x80)
/*!
  comments
  */
#define R_M_PIC_TI_INIT1      (M_PIC_BASE_ADDR + 0x84)
/*!
  comments
  */
#define R_M_PIC_TI_INIT2      (M_PIC_BASE_ADDR + 0x88)
/*!
  comments
  */
#define R_M_PIC_TI_INIT3      (M_PIC_BASE_ADDR + 0x8c)
/*!
  comments
  */
#define R_M_PIC_TI_CAP0      (M_PIC_BASE_ADDR + 0x90)
/*!
  comments
  */
#define R_M_PIC_TI_CAP1      (M_PIC_BASE_ADDR + 0x94)
/*!
  comments
  */
#define R_M_PIC_TI_CAP2      (M_PIC_BASE_ADDR + 0x98)
/*!
  comments
  */
#define R_M_PIC_TI_CAP3      (M_PIC_BASE_ADDR + 0x9c)
/*!
  comments
  */
#define R_M_PIC_TI_CW      (M_PIC_BASE_ADDR + 0xa0)
/*!
  comments
  */
#define R_M_PIC_WD_MPR     (M_PIC_BASE_ADDR + 0x100)
/*!
  comments
  */
#define R_M_PIC_WD_PROTECT     (M_PIC_BASE_ADDR + 0x104)
/*!
  comments
  */
#define R_M_PIC_WD_EN     (M_PIC_BASE_ADDR + 0x108)
/*!
  comments
  */
#define R_M_PIC_WD_FEED    (M_PIC_BASE_ADDR + 0x10c)
/*!
  comments
  */
#define R_M_PIC_WD_STA_CLR    (M_PIC_BASE_ADDR + 0x110)
/*!
  comments
  */
#define R_M_PIC_WD_STATUS   (M_PIC_BASE_ADDR + 0x114)

/************************************************************************ 
 * IrDA Controller
 ************************************************************************/
/*!
  comments
  */
#define R_IR_BASE_ADDR      0xBF151000
/*!
  comments
  */
#define R_IR_NEC_DATA      R_IR_BASE_ADDR
/*!
  comments
  */
#define R_IR_COM_DATA     (R_IR_BASE_ADDR + 0x04)
/*!
  comments
  */
#define R_IR_GLOBAL_CTRL  (R_IR_BASE_ADDR + 0x08)
/*!
  comments
  */
#define R_IR_GLOBAL_STA   (R_IR_BASE_ADDR + 0x0C)
/*!
  comments
  */
#define R_IR_INT_CFG      (R_IR_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_IR_INT_RAWSTA   (R_IR_BASE_ADDR + 0x14)
/*!
  comments
  */
#define R_IR_NEC_FILT     (R_IR_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_IR_COMBUF_POP   (R_IR_BASE_ADDR + 0x1C)
/*!
  comments
  */
#define R_IR_NEC_START_ONTSET   (R_IR_BASE_ADDR + 0x40)
/*!
  comments
  */
#define R_IR_NEC_START_PRDSET   (R_IR_BASE_ADDR + 0x44)
/*!
  comments
  */
#define R_IR_NEC_REPEAT_ONTSET  (R_IR_BASE_ADDR + 0x48)
/*!
  comments
  */
#define R_IR_NEC_REPEAT_PRDSET  (R_IR_BASE_ADDR + 0x4C)
/*!
  comments
  */
#define R_IR_NEC_BIT1_ONTSET    (R_IR_BASE_ADDR + 0x50)
/*!
  comments
  */
#define R_IR_NEC_BIT1_PRDSET    (R_IR_BASE_ADDR + 0x54)
/*!
  comments
  */
#define R_IR_NEC_BIT0_ONTSET    (R_IR_BASE_ADDR + 0x58)
/*!
  comments
  */
#define R_IR_NEC_BIT0_PRDSET    (R_IR_BASE_ADDR + 0x5C)
/*!
  comments
  */
#define R_IR_WAVEFILT_CFG0  (R_IR_BASE_ADDR + 0x80)
/*!
  comments
  */
#define R_IR_WAVEFILT_CFG1  (R_IR_BASE_ADDR + 0x84)
/*!
  comments
  */
#define R_IR_WAVEFILT_CFG2  (R_IR_BASE_ADDR + 0x88)
/*!
  comments
  */
#define R_IR_WAVEFILT_CFG3  (R_IR_BASE_ADDR + 0x8C)
/*!
  comments
  */
#define R_IR_WAVEFILT_MASK  (R_IR_BASE_ADDR + 0x90)
/*!
  comments
  */
#define R_IR_WFN_ONTSET  (R_IR_BASE_ADDR + 0x100)
/*!
  comments
  */
#define R_IR_WFN_PRDSET  (R_IR_BASE_ADDR + 0x104)

/************************************************************************ 
 * EPI Controller
 ************************************************************************/
/*!
  comments
  */
#define EPI_INSTRUCTION_BASE_ADDR  0xbf040000
/*!
  comments
  */
#define EPI_BASE_ADDR  0xbf040800
/*!
  comments
  */
#define R_EPI_FIFO_DATA  (EPI_BASE_ADDR + 0xa0)
/*!
  comments
  */
#define R_EPI_SGL_DATA    (EPI_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_EPI_CMD_ADDR    (EPI_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_EPI_TRIG_CFG    (EPI_BASE_ADDR + 0xc)
/*!
  comments
  */
#define R_EPI_CTRL    (EPI_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_EPI_BOOT    (EPI_BASE_ADDR + 0x14)
/*!
  comments
  */
#define R_EPI_TC    (EPI_BASE_ADDR + 0x1c)
/*!
  comments
  */
#define R_EPI_AC    (EPI_BASE_ADDR + 0x20)
/*!
  comments
  */
#define R_EPI_CH0_CTRL    (EPI_BASE_ADDR + 0x24)
/*!
  comments
  */
#define R_EPI_STA0    (EPI_BASE_ADDR + 0x34)
/*!
  comments
  */
#define R_EPI_STA1    (EPI_BASE_ADDR + 0X38)
/*!
  comments
  */
#define R_EPI_ECC_CMP   (EPI_BASE_ADDR + 0x3c)
/*!
  comments
  */
#define R_EPI_ECC1_DATA   (EPI_BASE_ADDR + 0x40)
/*!
  comments
  */
#define R_EPI_ECC2_DATA   (EPI_BASE_ADDR + 0x44)
/*!
  comments
  */
#define R_EPI_ECC3_DATA   (EPI_BASE_ADDR + 0x48)
/*!
  comments
  */
#define R_EPI_ECC4_DATA   (EPI_BASE_ADDR + 0x4c)
/*!
  comments
  */
#define R_EPI_ECC_STA   (EPI_BASE_ADDR + 0x50)
/*!
  comments
  */
#define R_EPI_ECC1_RESULT   (EPI_BASE_ADDR + 0x54)
/*!
  comments
  */
#define R_EPI_ECC2_RESULT   (EPI_BASE_ADDR + 0x58)
/*!
  comments
  */
#define R_EPI_ECC3_RESULT   (EPI_BASE_ADDR + 0x5c)
/*!
  comments
  */
#define R_EPI_ECC4_RESULT   (EPI_BASE_ADDR + 0x60)
/*!
  comments
  */
#define R_EPI_ERR12_LOCATION    (EPI_BASE_ADDR + 0x64)
/*!
  comments
  */
#define R_EPI_ERR34_LOCATION    (EPI_BASE_ADDR + 0x68)
/*!
  comments
  */
#define R_EPI_CH0_CFG   (EPI_BASE_ADDR + 0x70)
/*!
  comments
  */
#define R_EPI_ECC_ERRNUM0_3   (EPI_BASE_ADDR + 0x80)
/*!
  comments
  */
#define R_EPI_ECC_ERRNUM4_7   (EPI_BASE_ADDR + 0x84)
/*!
  comments
  */
#define R_EPI_ECC_ERRNUM8_11  (EPI_BASE_ADDR + 0x88)
/*!
  comments
  */
#define R_EPI_ECC_ERRNUM12_15  (EPI_BASE_ADDR + 0x8c)
/*!
  comments
  */
#define R_EPI_ECC_ERR_CNUM0_11    (EPI_BASE_ADDR + 0x90)
/*!
  comments
  */
#define R_EPI_ERRLOC_NUM    (EPI_BASE_ADDR + 0x94)
/*!
  comments
  */
#define R_EPI_ERR_LOCATION_FIFO   (EPI_BASE_ADDR + 0x98)
/*!
  comments
  */
#define R_EPI_WP   (EPI_BASE_ADDR + 0x9c)
/*!
  comments
  */
#define R_EPI_BCH_CR_FLAG   (EPI_BASE_ADDR + 0xe0)


#if 0
/************************************************************************ 
 * SPI Controller
 ************************************************************************/
/*!
  comments
  */
#define CONCERTO_SPI_MEM_BASE                      0x9E000000 
/*!
  comments
  */
#define SPIN_BASE_ADDR                             0xBF010000 
/*!
  comments
  */
#define R_SPIN_TXD(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x100))
/*!
  comments
  */
#define R_SPIN_RXD(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x100))
/*!
  comments
  */
#define R_SPIN_MEM_CMD(i)                            ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x04))
/*!
  comments
  */
#define R_SPIN_BAUD(i)                            ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x08))
/*!
  comments
  */
#define R_SPIN_MODE_CFG(i)                         ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x0c))
/*!
  comments
  */
#define R_SPIN_INT_CFG(i)                         ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x10))
/*!
  comments
  */
#define R_SPIN_TC(i)                               ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x14))
/*!
  comments
  */
#define R_SPIN_CTRL(i)                             ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x18))
/*!
  comments
  */
#define R_SPIN_STA(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x1c))
/*!
  comments
  */
#define R_SPIN_PIN_CFG_IO(i)                     ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x20))
/*!
  comments
  */
#define R_SPIN_MEM_BOOT(i)                        ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x24))
/*!
  comments
  */
#define R_SPIN_MEM_CTRL(i)                        ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x28))
/*!
  comments
  */
#define R_SPIN_PIN_MODE(i)                        ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x2c))
/*!
  comments
  */
#define R_SPIN_DEBUG(i)                           ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x30))
/*!
  comments
  */
#define R_SPIN_MOSI_CTRL(i)                       ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x34))
/*!
  comments
  */
#define R_SPIN_MEMMODE_CTRL1(i)                  ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x40))
/*!
  comments
  */
#define R_SPIN_MEMMODE_CTRL2(i)                  ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x44))
/*!
  comments
  */
#define R_SPIN_MEMMODE_RDY_DLY(i)                ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x38))
/*!
  comments
  */
#define R_SPIN_INT_STATUE(i)                      ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x3c))


#endif

/************************************************************************ 
 * I2C Controller
 ************************************************************************/
/*!
  comments
  */
#define I2C0_MASTER0_BASE_ADDR                             0xbf560000
/*!
  comments
  */
#define R_I2C0_MASTER0_PRER_H                              (I2C0_MASTER0_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_I2C0_MASTER0_PRER_L                              (I2C0_MASTER0_BASE_ADDR + 0x1c)
/*!
  comments
  */
#define R_I2C0_MASTER0_TXR                                 (I2C0_MASTER0_BASE_ADDR + 0x08)
/*!
  comments
  */
#define R_I2C0_MASTER0_RXR                                 (I2C0_MASTER0_BASE_ADDR + 0x0c)
/*!
  comments
  */
#define R_I2C0_MASTER0_SR                                  (I2C0_MASTER0_BASE_ADDR + 0x04)
/*!
  comments
  */
#define R_I2C0_MASTER0_CTR                                 (I2C0_MASTER0_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_I2C0_MASTER0_CR                                  (I2C0_MASTER0_BASE_ADDR + 0x14)
/*!
  comments
  */
#define I2C1_MASTER0_BASE_ADDR                             0xbf570000
/*!
  comments
  */
#define R_I2C1_MASTER0_PRER_H                              (I2C1_MASTER0_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_I2C1_MASTER0_PRER_L                              (I2C1_MASTER0_BASE_ADDR + 0x1c)
/*!
  comments
  */
#define R_I2C1_MASTER0_TXR                                 (I2C1_MASTER0_BASE_ADDR + 0x08)
/*!
  comments
  */
#define R_I2C1_MASTER0_RXR                                 (I2C1_MASTER0_BASE_ADDR + 0x0c)
/*!
  comments
  */
#define R_I2C1_MASTER0_SR                                  (I2C1_MASTER0_BASE_ADDR + 0x04)
/*!
  comments
  */
#define R_I2C1_MASTER0_CTR                                 (I2C1_MASTER0_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_I2C1_MASTER0_CR                                  (I2C1_MASTER0_BASE_ADDR + 0x14)
/*!
  comments
  */
#define I2C2_MASTER0_BASE_ADDR                             0xbf5f0000
/*!
  comments
  */
#define R_I2C2_MASTER0_PRER_H                              (I2C2_MASTER0_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_I2C2_MASTER0_PRER_L                              (I2C2_MASTER0_BASE_ADDR + 0x1c)
/*!
  comments
  */
#define R_I2C2_MASTER0_TXR                                 (I2C2_MASTER0_BASE_ADDR + 0x08)
/*!
  comments
  */
#define R_I2C2_MASTER0_RXR                                 (I2C2_MASTER0_BASE_ADDR + 0x0c)
/*!
  comments
  */
#define R_I2C2_MASTER0_SR                                  (I2C2_MASTER0_BASE_ADDR + 0x04)
/*!
  comments
  */
#define R_I2C2_MASTER0_CTR                                 (I2C2_MASTER0_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_I2C2_MASTER0_CR                                  (I2C2_MASTER0_BASE_ADDR + 0x14)
/*!
  comments
  */
#define I2C3_MASTER0_BASE_ADDR                             0xbf5c0000
/*!
  comments
  */
#define R_I2C3_MASTER0_PRER_H                              (I2C3_MASTER0_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_I2C3_MASTER0_PRER_L                              (I2C3_MASTER0_BASE_ADDR + 0x1c)
/*!
  comments
  */
#define R_I2C3_MASTER0_TXR                                 (I2C3_MASTER0_BASE_ADDR + 0x08)
/*!
  comments
  */
#define R_I2C3_MASTER0_RXR                                 (I2C3_MASTER0_BASE_ADDR + 0x0c)
/*!
  comments
  */
#define R_I2C3_MASTER0_SR                                  (I2C3_MASTER0_BASE_ADDR + 0x04)
/*!
  comments
  */
#define R_I2C3_MASTER0_CTR                                 (I2C3_MASTER0_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_I2C3_MASTER0_CR                                  (I2C3_MASTER0_BASE_ADDR + 0x14)
/*!
  comments
  */
#define I2C4_MASTER0_BASE_ADDR                             0xbf580000
/*!
  comments
  */
#define R_I2C4_MASTER0_PRER_H                              (I2C4_MASTER0_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_I2C4_MASTER0_PRER_L                              (I2C4_MASTER0_BASE_ADDR + 0x1c)
/*!
  comments
  */
#define R_I2C4_MASTER0_TXR                                 (I2C4_MASTER0_BASE_ADDR + 0x08)
/*!
  comments
  */
#define R_I2C4_MASTER0_RXR                                 (I2C4_MASTER0_BASE_ADDR + 0x0c)
/*!
  comments
  */
#define R_I2C4_MASTER0_SR                                  (I2C4_MASTER0_BASE_ADDR + 0x04)
/*!
  comments
  */
#define R_I2C4_MASTER0_CTR                                 (I2C4_MASTER0_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_I2C4_MASTER0_CR                                  (I2C4_MASTER0_BASE_ADDR + 0x14)
/************************************************************************ 
 * OTP Controller
 ************************************************************************/
/*!
  comments
  */
#define OTP_DB_BASE_ADDR        0xbf320000
/*!
  comments
  */
#define OTP_DB_CTRL_BASE_ADDR       0xbf323f00
/*!
  comments
  */
#define OTP_BASE_ADDR       0xbf324000
/*!
  comments
  */
#define R_OTP_TIME_CFG        (OTP_BASE_ADDR + 0x0)
/*!
  comments
  */
#define R_OTP_WR_CFG        (OTP_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_OTP_RD_CFG        (OTP_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_OTP_CTRL        (OTP_BASE_ADDR + 0xc)
/*!
  comments
  */
#define R_OTP_WRRD_PRTCT0       (OTP_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_OTP_WRRD_PRTCT1       (OTP_BASE_ADDR + 0x14)       

/************************************************************************ 
 * CLK Controller
 ************************************************************************/
/*!
  comments
  */
#define R_CLK_CFG_BASE    0xBF500000
/*!
  comments
  */
#define R_CLK_SYS_CFG    (R_CLK_CFG_BASE + 0x0)
/*!
  comments
  */
#define R_CLK_UPDATE_STA    (R_CLK_CFG_BASE + 0x4)
/*!
  comments
  */
#define R_CLK_UPDATE_CFG    (R_CLK_CFG_BASE + 0x8)
/*!
  comments
  */
#define R_CLK_GATE_CFG0    (R_CLK_CFG_BASE + 0x10)
/*!
  comments
  */
#define R_CLK_GATE_CFG1    (R_CLK_CFG_BASE + 0x14)
/*!
  comments
  */
#define R_CLK_GATE_CFG2    (R_CLK_CFG_BASE + 0x18)
/*!
  comments
  */
#define R_CLK_GATE_CFG3    (R_CLK_CFG_BASE + 0x1C)
/*!
  comments
  */
#define R_CLK_DMA_CFG    (R_CLK_CFG_BASE + 0x20)
/*!
  comments
  */
#define R_CLK_TRANSPORT_CFG    (R_CLK_CFG_BASE + 0x24)
/*!
  comments
  */
#define R_CLK_VIDEO_CFG    (R_CLK_CFG_BASE + 0x30)
/*!
  comments
  */
#define R_CLK_AUDIO_CFG    (R_CLK_CFG_BASE + 0x34)
/*!
  comments
  */
#define R_CLK_UART0_CFG    (R_CLK_CFG_BASE + 0x40)
/*!
  comments
  */
#define R_CLK_UART1_CFG    (R_CLK_CFG_BASE + 0x44)
/*!
  comments
  */
#define R_CLK_SMC_CFG    (R_CLK_CFG_BASE + 0x48)
/*!
  comments
  */
#define R_CLK_SPI_CFG    (R_CLK_CFG_BASE + 0x4C)
/*!
  comments
  */
#define R_CLK_MAC_CFG    (R_CLK_CFG_BASE + 0x50)
/*!
  comments
  */
#define R_CLK_SD_CFG    (R_CLK_CFG_BASE + 0x58)

/*!
  comments
  */
#define R_CLKGEN_CORECLK    0xBF5D0048
/*!
  comments
  */
#define R_CLKGEN_VHDINTP    0xBF5D005C
/*!
  comments
  */
#define R_CLKGEN_VSDINTP    0xBF5D0064
/*!
  comments
  */
#define R_CLKGEN_HDMIPLL    0xBF5D0068

/************************************************************************ 
 * SMC Controller
 ************************************************************************/
/*!
  comments
  */
#define R_SMC0_BASE_ADDR    0xBF590000
/*!
  comments
  */
#define R_SMC0_DBUF    R_SMC0_BASE_ADDR
/*!
  comments
  */
#define R_SMC0_STA   (R_SMC0_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_SMC0_FRQ_CFG (R_SMC0_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_SMC0_NC_SET (R_SMC0_BASE_ADDR + 0xC)
/*!
  comments
  */
#define R_SMC0_TRANS_CTRL (R_SMC0_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_SMC0_ETU1_SET (R_SMC0_BASE_ADDR + 0x14)
/*!
  comments
  */
#define R_SMC0_ETU2_SET (R_SMC0_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_SMC0_ETU3_SET (R_SMC0_BASE_ADDR + 0x1C)
/*!
  comments
  */
#define R_SMC0_CPCTRL (R_SMC0_BASE_ADDR + 0x20)
/*!
  comments
  */
#define R_SMC0_CMDLEN_H (R_SMC0_BASE_ADDR + 0x24)
/*!
  comments
  */
#define R_SMC0_CMDLEN_L (R_SMC0_BASE_ADDR + 0x28)
/*!
  comments
  */
#define R_SMC0_BUF_CNT_L (R_SMC0_BASE_ADDR + 0x2C)
/*!
  comments
  */
#define R_SMC0_BUF_CNT_H (R_SMC0_BASE_ADDR + 0x30)
/*!
  comments
  */
#define R_SMC0_PIN_CFG (R_SMC0_BASE_ADDR + 0x34)
/*!
  comments
  */
#define R_SMC0_RESEND_CFG (R_SMC0_BASE_ADDR + 0x38)
/*!
  comments
  */
#define R_SMC1_BASE_ADDR    0xBF5A0000
/*!
  comments
  */
#define R_SMC1_DBUF    R_SMC1_BASE_ADDR
/*!
  comments
  */
#define R_SMC1_STA   (R_SMC1_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_SMC1_FRQ_CFG (R_SMC1_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_SMC1_NC_SET (R_SMC1_BASE_ADDR + 0xC)
/*!
  comments
  */
#define R_SMC1_TRANS_CTRL (R_SMC1_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_SMC1_ETU1_SET (R_SMC1_BASE_ADDR + 0x14)
/*!
  comments
  */
#define R_SMC1_ETU2_SET (R_SMC1_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_SMC1_ETU3_SET (R_SMC1_BASE_ADDR + 0x1C)
/*!
  comments
  */
#define R_SMC1_CPCTRL (R_SMC1_BASE_ADDR + 0x20)
/*!
  comments
  */
#define R_SMC1_CMDLEN_H (R_SMC1_BASE_ADDR + 0x24)
/*!
  comments
  */
#define R_SMC1_CMDLEN_L (R_SMC1_BASE_ADDR + 0x28)
/*!
  comments
  */
#define R_SMC1_BUF_CNT_L (R_SMC1_BASE_ADDR + 0x2C)
/*!
  comments
  */
#define R_SMC1_BUF_CNT_H (R_SMC1_BASE_ADDR + 0x30)
/*!
  comments
  */
#define R_SMC1_PIN_CFG (R_SMC1_BASE_ADDR + 0x34)
/*!
  comments
  */
#define R_SMC1_RESEND_CFG (R_SMC1_BASE_ADDR + 0x38)

/************************************************************************ 
 * DMA Controller
 ************************************************************************/
/*!
  comments
  */
#define R_DMA_BASE_ADDR    0xBF400000
/*!
  comments
  */
#define R_DMA_INT_STATUS_M    R_DMA_BASE_ADDR
/*!
  comments
  */
#define R_DMA_INT_NODE_STATUS_M    (R_DMA_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_DMA_INT_LINK_STATUS_M    (R_DMA_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_DMA_INT_RD_STATUS_M    (R_DMA_BASE_ADDR + 0xC)
/*!
  comments
  */
#define R_DMA_INT_WR_STATUS_M    (R_DMA_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_DMA_INT_STATUS_S    (R_DMA_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_DMA_INT_NODE_STATUS_S    (R_DMA_BASE_ADDR + 0x1C)
/*!
  comments
  */
#define R_DMA_INT_LINK_STATUS_S    (R_DMA_BASE_ADDR + 0x20)
/*!
  comments
  */
#define R_DMA_INT_RD_STATUS_S    (R_DMA_BASE_ADDR + 0x24)
/*!
  comments
  */
#define R_DMA_INT_WR_STATUS_S    (R_DMA_BASE_ADDR + 0x28)
/*!
  comments
  */
#define R_DMA_RAW_INT_STATUS_M    (R_DMA_BASE_ADDR + 0x30)
/*!
  comments
  */
#define R_DMA_RAW_NODE_STATUS_M    (R_DMA_BASE_ADDR + 0x34)
/*!
  comments
  */
#define R_DMA_RAW_LINK_STATUS_M    (R_DMA_BASE_ADDR + 0x38)
/*!
  comments
  */
#define R_DMA_RAW_INT_STATUS_S    (R_DMA_BASE_ADDR + 0x48)
/*!
  comments
  */
#define R_DMA_RAW_NODE_STATUS_S    (R_DMA_BASE_ADDR + 0x4C)
/*!
  comments
  */
#define R_DMA_RAW_LINK_STATUS_S    (R_DMA_BASE_ADDR + 0x50)
/*!
  comments
  */
#define R_DMA_SOFT_RST_EN    (R_DMA_BASE_ADDR + 0x60)
/*!
  comments
  */
#define R_DMA_CLKEN_CONFIG    (R_DMA_BASE_ADDR + 0x64)
/*!
  comments
  */
#define R_DMA_NODE_PRI_SET    (R_DMA_BASE_ADDR + 0x6C)
/*!
  comments
  */
#define R_DMA_CH_PRI_SET0    (R_DMA_BASE_ADDR + 0x70)
/*!
  comments
  */
#define R_DMA_CH_PRI_SET1    (R_DMA_BASE_ADDR + 0x74)
/*!
  comments
  */
#define R_DMA_AHB_PRI_SET    (R_DMA_BASE_ADDR + 0x78)
/*!
  comments
  */
#define R_DMA_AXI_PRI_SET    (R_DMA_BASE_ADDR + 0x7C)
/*!
  comments
  */
#define R_DMA_CHN_LLN    (R_DMA_BASE_ADDR + 0x80)
/*!
  comments
  */
#define R_DMA_CHN_SRC_ADDR    (R_DMA_BASE_ADDR + 0x84)
/*!
  comments
  */
#define R_DMA_CHN_DST_ADDR    (R_DMA_BASE_ADDR + 0x88)
/*!
  comments
  */
#define R_DMA_CHN_SRC_BLOCK    (R_DMA_BASE_ADDR + 0x8C)
/*!
  comments
  */
#define R_DMA_CHN_DST_BLOCK    (R_DMA_BASE_ADDR + 0x90)
/*!
  comments
  */
#define R_DMA_CHN_CONFIG    (R_DMA_BASE_ADDR + 0x94)
/*!
  comments
  */
#define R_DMA_CHN_DATA_CNT    (R_DMA_BASE_ADDR + 0x98)
/*!
  comments
  */
#define R_DMA_CHN_CONTROL    (R_DMA_BASE_ADDR + 0x9C)
/*!
  comments
  */
#define R_DMA_CHN_PUSH    (R_DMA_BASE_ADDR + 0xA0)
/*!
  comments
  */
#define R_DMA_CHN_STATUS    (R_DMA_BASE_ADDR + 0xA4)
/*!
  comments
  */
#define R_DMA_CHN_TRANSED_CNT    (R_DMA_BASE_ADDR + 0xA8)
/*!
  comments
  */
#define R_DMA_CHN_WR_POINT    (R_DMA_BASE_ADDR + 0xAC)
/*!
  comments
  */
#define R_DMA_CHN_RD_POINT    (R_DMA_BASE_ADDR + 0xB0)
/*!
  comments
  */
#define R_DMA_CHN_ALU_WORD    (R_DMA_BASE_ADDR + 0xB4)

/************************************************************************ 
 * DMA Plus Controller
 ************************************************************************/
/*!
  comments
  */
#define R_DMA_PLUS_BASE_ADDR    0xBF400000
/*!
  comments
  */
#define R_DMA_PLUS_INT_STATUS_M    R_DMA_PLUS_BASE_ADDR
/*!
  comments
  */
#define R_DMA_PLUS_INT_NODE_STATUS_M    (R_DMA_PLUS_BASE_ADDR + 0x4)
/*!
  comments
  */
#define R_DMA_PLUS_INT_LINK_STATUS_M    (R_DMA_PLUS_BASE_ADDR + 0x8)
/*!
  comments
  */
#define R_DMA_PLUS_INT_STATUS_S    (R_DMA_PLUS_BASE_ADDR + 0xC)
/*!
  comments
  */
#define R_DMA_PLUS_INT_NODE_STATUS_S    (R_DMA_PLUS_BASE_ADDR + 0x10)
/*!
  comments
  */
#define R_DMA_PLUS_INT_LINK_STATUS_S    (R_DMA_PLUS_BASE_ADDR + 0x14)
/*!
  comments
  */
#define R_DMA_PLUS_RAW_INT_STATUS_M    (R_DMA_PLUS_BASE_ADDR + 0x18)
/*!
  comments
  */
#define R_DMA_PLUS_RAW_NODE_STATUS_M    (R_DMA_PLUS_BASE_ADDR + 0x1C)
/*!
  comments
  */
#define R_DMA_PLUS_RAW_LINK_STATUS_M    (R_DMA_PLUS_BASE_ADDR + 0x20)
/*!
  comments
  */
#define R_DMA_PLUS_RAW_INT_STATUS_S    (R_DMA_PLUS_BASE_ADDR + 0x24)
/*!
  comments
  */
#define R_DMA_PLUS_RAW_NODE_STATUS_S    (R_DMA_PLUS_BASE_ADDR + 0x28)
/*!
  comments
  */
#define R_DMA_PLUS_RAW_LINK_STATUS_S    (R_DMA_PLUS_BASE_ADDR + 0x2C)
/*!
  comments
  */
#define R_DMA_PLUS_SOFT_RST_EN    (R_DMA_PLUS_BASE_ADDR + 0x50)
/*!
  comments
  */
#define R_DMA_PLUS_PRI_SET    (R_DMA_PLUS_BASE_ADDR + 0x54)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_LLN    (R_DMA_PLUS_BASE_ADDR + 0x80)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_SRC_ADDR    (R_DMA_PLUS_BASE_ADDR + 0x84)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_DST_ADDR    (R_DMA_PLUS_BASE_ADDR + 0x88)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_SRC_BLOCK    (R_DMA_PLUS_BASE_ADDR + 0x8C)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_DST_BLOCK    (R_DMA_PLUS_BASE_ADDR + 0x90)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_CONFIG    (R_DMA_PLUS_BASE_ADDR + 0x94)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_TRANSED_CNT    (R_DMA_PLUS_BASE_ADDR + 0x98)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_CONTROL    (R_DMA_PLUS_BASE_ADDR + 0x9C)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_ALU_WORD    (R_DMA_PLUS_BASE_ADDR + 0xA0)
/*!
  comments
  */
#define R_DMA_PLUS_CHN_STATUS    (R_DMA_PLUS_BASE_ADDR + 0xA4)

/************************************************************************ 
 * AVDMA Controller
 ************************************************************************/
/*!
  comments
  */
#define R_AVDMA_SRC_ADDR    0xBF138060
/*!
  comments
  */
#define R_AVDMA_DST_ADDR    0xBF138064
/*!
  comments
  */
#define R_AVDMA_LEN    0xBF138068
/*!
  comments
  */
#define R_AVDMA_CMD    0xBF13806C

/************************************************************
 * EDMA Register definition
 ************************************************************/
/*!
  comments
  */
#define EDMA_REG_BASE                   0xbf080080
/*!
  comments
  */
#define EDMA_TX_CTRL                   (EDMA_REG_BASE + 0x0)
/*!
  comments
  */
#define EDMA_TX_BUFFER_BASE             (EDMA_REG_BASE + 0x4)
/*!
  comments
  */
#define EDMA_TX_BUFFER_SIZE             (EDMA_REG_BASE + 0x8)
/*!
  comments
  */
#define EDMA_TX_PUT_ID                 (EDMA_REG_BASE + 0xc)
/*!
  comments
  */
#define EDMA_TX_GET_ID                 (EDMA_REG_BASE + 0x10)
/*!
  comments
  */
#define EDMA_TX_CUR_RD_OFF             (EDMA_REG_BASE + 0x14)
/*!
  comments
  */
#define EDMA_TX_STATUS                 (EDMA_REG_BASE + 0x18)
/*!
  comments
  */
#define EDMA_MAX_PK_SIZE               (EDMA_REG_BASE + 0x20)
/*!
  comments
  */
#define EDMA_MIN_PK_SIZE               (EDMA_REG_BASE + 0x24)
/*!
  comments
  */
#define EDMA_RX_CTRL                   (EDMA_REG_BASE + 0x40)
/*!
  comments
  */
#define EDMA_RX_INFO_BUFFER_BASE       (EDMA_REG_BASE + 0x44)
/*!
  comments
  */
#define EDMA_RX_INFO_BUFFER_SIZE       (EDMA_REG_BASE + 0x48)
/*!
  comments
  */
#define EDMA_RX_DATA_BUFFER_BASE       (EDMA_REG_BASE + 0x4c)
/*!
  comments
  */
#define EDMA_RX_DATA_BUFFER_SIZE       (EDMA_REG_BASE + 0x50)
/*!
  comments
  */
#define EDMA_RX_PUT_ID                 (EDMA_REG_BASE + 0x54)
/*!
  comments
  */
#define EDMA_RX_INFO_RD_OFF             (EDMA_REG_BASE + 0x58)
/*!
  comments
  */
#define EDMA_RX_INFO_WR_OFF             (EDMA_REG_BASE + 0x5c)
/*!
  comments
  */
#define EDMA_RX_DATA_RD_OFF             (EDMA_REG_BASE + 0x60)
/*!
  comments
  */
#define EDMA_RX_DATA_WR_OFF             (EDMA_REG_BASE + 0x64)
/*!
  comments
  */
#define EDMA_RX_THRESHOLD               (EDMA_REG_BASE + 0x68)
/*!
  comments
  */
#define EDMA_INT_SOURCE                 (EDMA_REG_BASE + 0x6c)
/*!
  comments
  */
#define EDMA_IRQ_STATUS                 (EDMA_REG_BASE + 0x78)

/************************************************************
 * EMAC Register definition
 ************************************************************/
/*!
  comments
  */
#define EMAC_REG_BASE                   0xbf080000
/*!
  comments
  */
#define EMAC_ADDR0                     (EMAC_REG_BASE + 0x0)
/*!
  comments
  */
#define EMAC_ADDR1                     (EMAC_REG_BASE + 0x4)
/*!
  comments
  */
#define EMAC_ADDR2                     (EMAC_REG_BASE + 0x8)
/*!
  comments
  */
#define EMAC_ADDR3                     (EMAC_REG_BASE + 0xc)
/*!
  comments
  */
#define EMAC_ADDR4                     (EMAC_REG_BASE + 0x10)
/*!
  comments
  */
#define EMAC_ADDR5                     (EMAC_REG_BASE + 0x14)
/*!
  comments
  */
#define EMAC_ADDR6                     (EMAC_REG_BASE + 0x18)
/*!
  comments
  */
#define EMAC_ADDR7                     (EMAC_REG_BASE + 0x1c)
/*!
  comments
  */
#define EMAC_ADDR8                     (EMAC_REG_BASE + 0x20)
/*!
  comments
  */
#define EMAC_ADDR9                     (EMAC_REG_BASE + 0x24)
/*!
  comments
  */
#define EMAC_ADDR10                     (EMAC_REG_BASE + 0x28)
/*!
  comments
  */
#define EMAC_ADDR11                     (EMAC_REG_BASE + 0x2c)
/*!
  comments
  */
#define EMAC_CTRL                       (EMAC_REG_BASE + 0x30)
/*!
  comments
  */
#define EMAC_FILTER_CTRL               (EMAC_REG_BASE + 0x34)
/*!
  comments
  */
#define EMAC_IRQ_STATUS                 (EMAC_REG_BASE + 0x38)
/*!
  comments
  */
#define EMAC_IRQ_CONTROL               (EMAC_REG_BASE + 0x3c)
/*!
  comments
  */
#define EMAC_RX_STATUS                 (EMAC_REG_BASE + 0x40)
/*!
  comments
  */
#define EMAC_TX_STATUS                 (EMAC_REG_BASE + 0x44)
/*!
  comments
  */
#define EMAC_FLOW_CTRL                 (EMAC_REG_BASE + 0x48)
/*!
  comments
  */
#define EMAC_SMI_CTRL                   (EMAC_REG_BASE + 0x4c)
/*!
  comments
  */
#define EMAC_SMI_COMM                   (EMAC_REG_BASE + 0x50)
/*!
  comments
  */
#define EMAC_PAUSE_PAT0                 (EMAC_REG_BASE + 0x54)
/*!
  comments
  */
#define EMAC_PAUSE_PAT1                 (EMAC_REG_BASE + 0x58)
/*!
  comments
  */
#define EMAC_PAUSE_PAT2                 (EMAC_REG_BASE + 0x5c)
/*!
  comments
  */
#define EMAC_RXDROP_CNT                 (EMAC_REG_BASE + 0x60)
/*!
  comments
  */
#define EMAC_TXDONE_CNT                 (EMAC_REG_BASE + 0x64)
/*!
  comments
  */
#define EMAC_TXDROP_CNT                 (EMAC_REG_BASE + 0x68)
/*!
  comments
  */
#define SYSCTRL_CHIP_REV     (0xBF140000)

/************************************************************
 * PIN Sel Registers
 ************************************************************/
/*!
  comments
  */
#define R_PIN_SEL_BASE 0xBF156000
/*!
  comments
  */
#define R_PIN8_SEL (R_PIN_SEL_BASE + 0x0)
/*!
  comments
  */
#define R_PIN0_SEL (R_PIN_SEL_BASE + 0x4)
/*!
  comments
  */
#define R_PIN1_SEL (R_PIN_SEL_BASE + 0x8)
/*!
  comments
  */
#define R_PIN2_SEL (R_PIN_SEL_BASE + 0xC)
/*!
  comments
  */
#define R_PIN3_SEL (R_PIN_SEL_BASE + 0x10)
/*!
  comments
  */
#define R_PIN4_SEL (R_PIN_SEL_BASE + 0x14)
/*!
  comments
  */
#define R_PIN5_SEL (R_PIN_SEL_BASE + 0x18)
/*!
  comments
  */
#define R_PIN6_SEL (R_PIN_SEL_BASE + 0x1C)
/*!
  comments
  */
#define R_PIN7_SEL (R_PIN_SEL_BASE + 0x20)

/************************************************************
 * HDMI Controller
 ************************************************************/
/*!
  comments
  */
#define R_HDMI_BASE 0xBF480000
/*!
  comments
  */
#define R_HDMI_VND_ID (R_HDMI_BASE + 0x0)
/*!
  comments
  */
#define R_HDMI_DEV_ID (R_HDMI_BASE + 0x2)
/*!
  comments
  */
#define R_HDMI_DEV_REV (R_HDMI_BASE + 0x4)
/*!
  comments
  */
#define R_HDMI_SFT_RST (R_HDMI_BASE + 0x5)
/*!
  comments
  */
#define R_HDMI_SYS_STAT (R_HDMI_BASE + 0x6)
/*!
  comments
  */
#define R_HDMI_SYS_CTRL1 (R_HDMI_BASE + 0x7)
/*!
  comments
  */
#define R_HDMI_SYS_CTRL2 (R_HDMI_BASE + 0x8)
/*!
  comments
  */
#define R_HDMI_SYS_CTRL3 (R_HDMI_BASE + 0x9)
/*!
  comments
  */
#define R_HDMI_SYS_CTRL4 (R_HDMI_BASE + 0xA)
/*!
  comments
  */
#define R_HDMI_VID_STAT (R_HDMI_BASE + 0x10)
/*!
  comments
  */
#define R_HDMI_VID_MODE0 (R_HDMI_BASE + 0x11)
/*!
  comments
  */
#define R_HDMI_VID_CTRL (R_HDMI_BASE + 0x12)
/*!
  comments
  */
#define R_HDMI_VID_CAPT0 (R_HDMI_BASE + 0x13)
/*!
  comments
  */
#define R_HDMI_VID_CAPT1 (R_HDMI_BASE + 0x14)
/*!
  comments
  */
#define R_HDMI_VID_HRES (R_HDMI_BASE + 0x15)
/*!
  comments
  */
#define R_HDMI_VID_ACT_PIXEL (R_HDMI_BASE + 0x17)
/*!
  comments
  */
#define R_HDMI_VID_HFP (R_HDMI_BASE + 0x19)
/*!
  comments
  */
#define R_HDMI_VID_HSYNC_WIDTH (R_HDMI_BASE + 0x1B)
/*!
  comments
  */
#define R_HDMI_VID_HBP (R_HDMI_BASE + 0x1D)
/*!
  comments
  */
#define R_HDMI_VID_VRES (R_HDMI_BASE + 0x1F)
/*!
  comments
  */
#define R_HDMI_VID_ACT_LINE (R_HDMI_BASE + 0x21)
/*!
  comments
  */
#define R_HDMI_VID_VSYNC_FP_LINE (R_HDMI_BASE + 0x23)
/*!
  comments
  */
#define R_HDMI_VID_VSYNC_ACTWID_LINE (R_HDMI_BASE + 0x24)
/*!
  comments
  */
#define R_HDMI_VID_VSYNC_BP_LINE (R_HDMI_BASE + 0x25)
/*!
  comments
  */
#define R_HDMI_VID_FSTAT_HRES (R_HDMI_BASE + 0x26)
/*!
  comments
  */
#define R_HDMI_VID_FSTAT_ACT_PIXEL (R_HDMI_BASE + 0x28)
/*!
  comments
  */
#define R_HDMI_VID_FSTAT_HFP (R_HDMI_BASE + 0x2A)
/*!
  comments
  */
#define R_HDMI_VID_FSTAT_HSYNC_WIDTH (R_HDMI_BASE + 0x2C)
/*!
  comments
  */
#define R_HDMI_VID_FSTAT_VBP (R_HDMI_BASE + 0x2E)
/*!
  comments
  */
#define R_HDMI_VID_FSTAT_VRES (R_HDMI_BASE + 0x30)
/*!
  comments
  */
#define R_HDMI_VID_FSTAT_ACT_LINE (R_HDMI_BASE + 0x32)
/*!
  comments
  */
#define R_HDMI_VID_VSYNC_FP_LINE_RO (R_HDMI_BASE + 0x34)
/*!
  comments
  */
#define R_HDMI_VID_VSYNC_ACTWID_LINE_RO (R_HDMI_BASE + 0x35)
/*!
  comments
  */
#define R_HDMI_VID_VSYNC_BP_LINE_RW (R_HDMI_BASE + 0x36)
/*!
  comments
  */
#define R_HDMI_VID_MODE1 (R_HDMI_BASE + 0x37)
/*!
  comments
  */
#define R_HDMI_VID_FREQ_CNT (R_HDMI_BASE + 0x38)
/*!
  comments
  */
#define R_HDMI_VID_CAPT2 (R_HDMI_BASE + 0x39)
/*!
  comments
  */
#define R_HDMI_VID_CAPT3 (R_HDMI_BASE + 0x3A)
/*!
  comments
  */
#define R_HDMI_LF_LINE_OFFSET (R_HDMI_BASE + 0x3B)
/*!
  comments
  */
#define R_HDMI_VID_MODE2 (R_HDMI_BASE + 0x3D)
/*!
  comments
  */
#define R_HDMI_VID_MODE3 (R_HDMI_BASE + 0x3E)
/*!
  comments
  */
#define R_HDMI_INT_FLAG (R_HDMI_BASE + 0x40)
/*!
  comments
  */
#define R_HDMI_INT_CTRL (R_HDMI_BASE + 0x41)
/*!
  comments
  */
#define R_HDMI_INT_STAT1 (R_HDMI_BASE + 0x42)
/*!
  comments
  */
#define R_HDMI_INT_STAT2 (R_HDMI_BASE + 0x43)
/*!
  comments
  */
#define R_HDMI_INT_STAT3 (R_HDMI_BASE + 0x44)
/*!
  comments
  */
#define R_HDMI_INT_STAT4 (R_HDMI_BASE + 0x48)
/*!
  comments
  */
#define R_HDMI_INT_STAT5 (R_HDMI_BASE + 0x49)
/*!
  comments
  */
#define R_HDMI_INT_MASK1 (R_HDMI_BASE + 0x45)
/*!
  comments
  */
#define R_HDMI_INT_MASK2 (R_HDMI_BASE + 0x46)
/*!
  comments
  */
#define R_HDMI_INT_MASK3 (R_HDMI_BASE + 0x47)
/*!
  comments
  */
#define R_HDMI_INT_MASK4 (R_HDMI_BASE + 0x4A)
/*!
  comments
  */
#define R_HDMI_INT_MASK5 (R_HDMI_BASE + 0x4B)
/*!
  comments
  */
#define R_HDMI_AUD_CTRL0 (R_HDMI_BASE + 0x50)
/*!
  comments
  */
#define R_HDMI_AUD_CTRL1 (R_HDMI_BASE + 0x51)
/*!
  comments
  */
#define R_HDMI_AUD_I2S_FMT_CTRL (R_HDMI_BASE + 0x52)
/*!
  comments
  */
#define R_HDMI_AUD_I2S_CHN_CTRL (R_HDMI_BASE + 0x53)
/*!
  comments
  */
#define R_HDMI_AUD_SW_WL_CTRL (R_HDMI_BASE + 0x54)
/*!
  comments
  */
#define R_HDMI_AUD_SPDIF_STAT (R_HDMI_BASE + 0x55)
/*!
  comments
  */
#define R_HDMI_AUD_I2S_STAT1 (R_HDMI_BASE + 0x56)
/*!
  comments
  */
#define R_HDMI_AUD_I2S_STAT2 (R_HDMI_BASE + 0x57)
/*!
  comments
  */
#define R_HDMI_AUD_I2S_STAT3 (R_HDMI_BASE + 0x58)
/*!
  comments
  */
#define R_HDMI_AUD_I2S_STAT4 (R_HDMI_BASE + 0x59)
/*!
  comments
  */
#define R_HDMI_AUD_I2S_STAT5 (R_HDMI_BASE + 0x5A)
/*!
  comments
  */
#define R_HDMI_AUD_STAT (R_HDMI_BASE + 0x5B)
/*!
  comments
  */
#define R_HDMI_AUD_BIST_CTRL (R_HDMI_BASE + 0x5C)
/*!
  comments
  */
#define R_HDMI_AUD_FREQ_CNT (R_HDMI_BASE + 0x5D)
/*!
  comments
  */
#define R_HDMI_AUD_DBG_STAT (R_HDMI_BASE + 0x5E)
/*!
  comments
  */
#define R_HDMI_AUD_CTRL2 (R_HDMI_BASE + 0x5F)
/*!
  comments
  */
#define R_HDMI_AUD_1B_CTRL (R_HDMI_BASE + 0x60)
/*!
  comments
  */
#define R_HDMI_AUD_1B_CHN_CTRL (R_HDMI_BASE + 0x61)
/*!
  comments
  */
#define R_HDMI_AUD_1B_CLK_CTRL (R_HDMI_BASE + 0x63)
/*!
  comments
  */
#define R_HDMI_SPDIF_ERR_THRLD (R_HDMI_BASE + 0x66)
/*!
  comments
  */
#define R_HDMI_SPDIF_ERR_CNT (R_HDMI_BASE + 0x67)
/*!
  comments
  */
#define R_HDMI_AUDIO_HBR_CTRL (R_HDMI_BASE + 0x68)
/*!
  comments
  */
#define R_HDMI_LINK_CTRL (R_HDMI_BASE + 0x70)
/*!
  comments
  */
#define R_HDMI_VID_CAPT4 (R_HDMI_BASE + 0x71)
/*!
  comments
  */
#define R_HDMI_LINK_MUTE_ECP_EN (R_HDMI_BASE + 0x72)
/*!
  comments
  */
#define R_HDMI_SERDES_TEST_PATRN0 (R_HDMI_BASE + 0x73)
/*!
  comments
  */
#define R_HDMI_SERDES_TEST_PATRN1 (R_HDMI_BASE + 0x74)
/*!
  comments
  */
#define R_HDMI_SERDES_TEST_PATRN2 (R_HDMI_BASE + 0x75)
/*!
  comments
  */
#define R_HDMI_2C_US_CNT0 (R_HDMI_BASE + 0x76)
/*!
  comments
  */
#define R_HDMI_2C_US_CNT1 (R_HDMI_BASE + 0x77)
/*!
  comments
  */
#define R_HDMI_PLL_MISC_CTRL (R_HDMI_BASE + 0x79)
/*!
  comments
  */
#define R_HDMI_ANLG_BLK_CTRL3 (R_HDMI_BASE + 0x7A)
/*!
  comments
  */
#define R_HDMI_DDC_SLV_DEV_ADDR (R_HDMI_BASE + 0x80)
/*!
  comments
  */
#define R_HDMI_DDC_SLV_SGMT_ADDR (R_HDMI_BASE + 0x81)
/*!
  comments
  */
#define R_HDMI_DDC_SLV_OFFSET_ADDR (R_HDMI_BASE + 0x82)
/*!
  comments
  */
#define R_HDMI_DDC_ACCESS_CMD (R_HDMI_BASE + 0x83)
/*!
  comments
  */
#define R_HDMI_DDC_ACCESS_NUM (R_HDMI_BASE + 0x84)
/*!
  comments
  */
#define R_HDMI_DDC_CHN_STAT (R_HDMI_BASE + 0x86)
/*!
  comments
  */
#define R_HDMI_DDC_FIFO_DATA (R_HDMI_BASE + 0x87)
/*!
  comments
  */
#define R_HDMI_DDC_FIFO_CNT (R_HDMI_BASE + 0x88)
/*!
  comments
  */
#define R_HDMI_AUD_DIN_EN (R_HDMI_BASE + 0x89)
/*!
  comments
  */
#define R_HDMI_ANLG_BLK_CTRL0 (R_HDMI_BASE + 0x8A)
/*!
  comments
  */
#define R_HDMI_PLL_FILT_CTRL0 (R_HDMI_BASE + 0x8B)
/*!
  comments
  */
#define R_HDMI_LINK_TX_PLL_CTRL0 (R_HDMI_BASE + 0x8C)
/*!
  comments
  */
#define R_HDMI_PLL_FILT_LKTX_CTRL (R_HDMI_BASE + 0x8D)
/*!
  comments
  */
#define R_HDMI_ANLG_BLK_CTRL1 (R_HDMI_BASE + 0x8E)
/*!
  comments
  */
#define R_HDMI_ANLG_BLK_CTRL2 (R_HDMI_BASE + 0x8F)
/*!
  comments
  */
#define R_HDMI_SYS_PWRDWN (R_HDMI_BASE + 0x90)
/*!
  comments
  */
#define R_HDMI_PLL_FILT_CTRL1 (R_HDMI_BASE + 0x91)
/*!
  comments
  */
#define R_HDMI_PLL_FILT_CTRL2 (R_HDMI_BASE + 0x92)
/*!
  comments
  */
#define R_HDMI_LINK_TX_PLL_CTRL1 (R_HDMI_BASE + 0x93)
/*!
  comments
  */
#define R_HDMI_LINK_TX_PLL_CTRL2 (R_HDMI_BASE + 0x94)
/*!
  comments
  */
#define R_HDMI_RING_OSC_CTRL (R_HDMI_BASE + 0x95)
/*!
  comments
  */
#define R_HDMI_TDMS_CHN_CFG1 (R_HDMI_BASE + 0x96)
/*!
  comments
  */
#define R_HDMI_TDMS_CHN_CFG2 (R_HDMI_BASE + 0x97)
/*!
  comments
  */
#define R_HDMI_TDMS_CHN_CFG3 (R_HDMI_BASE + 0x98)
/*!
  comments
  */
#define R_HDMI_TDMS_CHN_CFG4 (R_HDMI_BASE + 0x99)
/*!
  comments
  */
#define R_HDMI_TDMS_MISC_CFG0 (R_HDMI_BASE + 0xE5)
/*!
  comments
  */
#define R_HDMI_TDMS_MISC_CFG1 (R_HDMI_BASE + 0xE6)
/*!
  comments
  */
#define R_HDMI_TDMS_MISC_CFG2 (R_HDMI_BASE + 0xE7)
/*!
  comments
  */
#define R_HDMI_CHIP_CTRL (R_HDMI_BASE + 0x9A)
/*!
  comments
  */
#define R_HDMI_CHIP_STAT (R_HDMI_BASE + 0x9B)
/*!
  comments
  */
#define R_HDMI_GPIO_CTRL (R_HDMI_BASE + 0x9C)
/*!
  comments
  */
#define R_HDMI_DBG_CTRL0 (R_HDMI_BASE + 0x9D)
/*!
  comments
  */
#define R_HDMI_DBG_CTRL1 (R_HDMI_BASE + 0x9E)
/*!
  comments
  */
#define R_HDMI_DBG_CTRL2 (R_HDMI_BASE + 0x9F)
/*!
  comments
  */
#define R_HDMI_HDCP_STAT (R_HDMI_BASE + 0xA0)
/*!
  comments
  */
#define R_HDMI_HDCP_CTRL0 (R_HDMI_BASE + 0xA1)
/*!
  comments
  */
#define R_HDMI_HDCP_CTRL1 (R_HDMI_BASE + 0xA2)
/*!
  comments
  */
#define R_HDMI_HDCP_LINK_CHK_FRM_NUM (R_HDMI_BASE + 0xA3)
/*!
  comments
  */
#define R_HDMI_HDCP_CTRL2 (R_HDMI_BASE + 0xA3)
/*!
  comments
  */
#define R_HDMI_HDCP_AKSV0 (R_HDMI_BASE + 0xA5)
/*!
  comments
  */
#define R_HDMI_HDCP_AKSV1 (R_HDMI_BASE + 0xA6)
/*!
  comments
  */
#define R_HDMI_HDCP_AKSV2 (R_HDMI_BASE + 0xA7)
/*!
  comments
  */
#define R_HDMI_HDCP_AKSV3 (R_HDMI_BASE + 0xA8)
/*!
  comments
  */
#define R_HDMI_HDCP_AKSV4 (R_HDMI_BASE + 0xA9)
/*!
  comments
  */
#define R_HDMI_HDCP_AN_L (R_HDMI_BASE + 0xAA)
/*!
  comments
  */
#define R_HDMI_HDCP_AN_H (R_HDMI_BASE + 0xAE)
/*!
  comments
  */
#define R_HDMI_HDCP_BKSV1 (R_HDMI_BASE + 0xB2)
/*!
  comments
  */
#define R_HDMI_HDCP_BKSV2 (R_HDMI_BASE + 0xB3)
/*!
  comments
  */
#define R_HDMI_HDCP_BKSV3 (R_HDMI_BASE + 0xB4)
/*!
  comments
  */
#define R_HDMI_HDCP_BKSV4 (R_HDMI_BASE + 0xB5)
/*!
  comments
  */
#define R_HDMI_HDCP_BKSV5 (R_HDMI_BASE + 0xB6)
/*!
  comments
  */
#define R_HDMI_HDCP_RI (R_HDMI_BASE + 0xB7)
/*!
  comments
  */
#define R_HDMI_HDCP_PJ (R_HDMI_BASE + 0xB9)
/*!
  comments
  */
#define R_HDMI_HDCP_RX_CAPS (R_HDMI_BASE + 0xBA)
/*!
  comments
  */
#define R_HDMI_HDCP_RX_BSTAT (R_HDMI_BASE + 0xBB)
/*!
  comments
  */
#define R_HDMI_HDCP_AM0_L (R_HDMI_BASE + 0xD0)
/*!
  comments
  */
#define R_HDMI_HDCP_AM0_H (R_HDMI_BASE + 0xD4)
/*!
  comments
  */
#define R_HDMI_HDCP_DBG_CTRL (R_HDMI_BASE + 0xBD)
/*!
  comments
  */
#define R_HDMI_HDCP_KEY_STAT (R_HDMI_BASE + 0xBE)
/*!
  comments
  */
#define R_HDMI_HDCP_KEY_CMD (R_HDMI_BASE + 0xBF)
/*!
  comments
  */
#define R_HDMI_HDCP_AUTH_DBG_STAT (R_HDMI_BASE + 0xC7)
/*!
  comments
  */
#define R_HDMI_HDCP_ENCP_DBG_STAT (R_HDMI_BASE + 0xC8)
/*!
  comments
  */
#define R_HDMI_HDCP_ENCP_FRM_NUM (R_HDMI_BASE + 0xC9)
/*!
  comments
  */
#define R_HDMI_DDC_MST_INT_CTRL_STAT (R_HDMI_BASE + 0xCA)
/*!
  comments
  */
#define R_HDMI_DDC_MST_LINK_CTRL_STAT (R_HDMI_BASE + 0xCB)
/*!
  comments
  */
#define R_HDMI_HDCP_VID_EMB (R_HDMI_BASE + 0xCC)
/*!
  comments
  */
#define R_HDMI_HDCP_WRITE_AKSV_WAIT (R_HDMI_BASE + 0xE0)
/*!
  comments
  */
#define R_HDMI_HDCP_LINK_CHK_TIMER (R_HDMI_BASE + 0xE1)
/*!
  comments
  */
#define R_HDMI_HDCP_RPTR_RDY_TIMER (R_HDMI_BASE + 0xE2)
/*!
  comments
  */
#define R_HDMI_HDCP_RPTR_WAIT_TIMER (R_HDMI_BASE + 0xE3)
/*!
  comments
  */
#define R_HDMI_AVI_INFOFRM_PCK (R_HDMI_BASE + 0x100)
/*!
  comments
  */
#define R_HDMI_AUD_INFOFRM_PCK (R_HDMI_BASE + 0x120)
/*!
  comments
  */
#define R_HDMI_SPD_INFOFRM_PCK (R_HDMI_BASE + 0x140)
/*!
  comments
  */
#define R_HDMI_MPEG_INFOFRM_PCK (R_HDMI_BASE + 0x160)
/*!
  comments
  */
#define R_HDMI_USER_INFOFRM_PCK0 (R_HDMI_BASE + 0x180)
/*!
  comments
  */
#define R_HDMI_USER_INFOFRM_PCK1 (R_HDMI_BASE + 0x1A0)
/*!
  comments
  */
#define R_HDMI_INFOFRM_PCK_CTRL (R_HDMI_BASE + 0x1C0)
/*!
  comments
  */
#define R_HDMI_ACR_N_SVAL1 (R_HDMI_BASE + 0x1C2)
/*!
  comments
  */
#define R_HDMI_ACR_N_SVAL2 (R_HDMI_BASE + 0x1C3)
/*!
  comments
  */
#define R_HDMI_ACR_N_SVAL3 (R_HDMI_BASE + 0x1C4)
/*!
  comments
  */
#define R_HDMI_ACR_CTS_SVAL1 (R_HDMI_BASE + 0x1C5)
/*!
  comments
  */
#define R_HDMI_ACR_CTS_SVAL2 (R_HDMI_BASE + 0x1C6)
/*!
  comments
  */
#define R_HDMI_ACR_CTS_SVAL3 (R_HDMI_BASE + 0x1C7)
/*!
  comments
  */
#define R_HDMI_ACR_CTS_HVAL1 (R_HDMI_BASE + 0x1C8)
/*!
  comments
  */
#define R_HDMI_ACR_CTS_HVAL2 (R_HDMI_BASE + 0x1C9)
/*!
  comments
  */
#define R_HDMI_ACR_CTS_HVAL3 (R_HDMI_BASE + 0x1CA)
/*!
  comments
  */
#define R_HDMI_ACR_CTS_CTRL (R_HDMI_BASE + 0x1CB)
/*!
  comments
  */
#define R_HDMI_GCP (R_HDMI_BASE + 0x1CC)
/*!
  comments
  */
#define R_HDMI_AUD_PCK_FLATLINE_CTRL (R_HDMI_BASE + 0x1CD)
/*!
  comments
  */
#define R_HDMI_GCP_HRD_ID (R_HDMI_BASE + 0x1CE)
/*!
  comments
  */
#define R_HDMI_AUD_PKT_ID (R_HDMI_BASE + 0x1CF)
/*!
  comments
  */
#define R_HDMI_CEC_CTRL (R_HDMI_BASE + 0x1D0)
/*!
  comments
  */
#define R_HDMI_CEC_RX_STAT (R_HDMI_BASE + 0x1D1)
/*!
  comments
  */
#define R_HDMI_CEC_TX_STAT (R_HDMI_BASE + 0x1D2)
/*!
  comments
  */
#define R_HDMI_CEC_FIFO (R_HDMI_BASE + 0x1D3)
/*!
  comments
  */
#define R_HDMI_CEC_SPD_CTRL (R_HDMI_BASE + 0x1D4)

/************************************************************
 * WatchDog Register definition
 ************************************************************/
/*!
  comments
  */
#define R_WD_REG_BASE                        0xBF100100
/*!
  comments
  */
#define R_M_WD_MPR                    (R_WD_REG_BASE + 0x0)
/*!
  comments
  */
#define R_M_WD_PROTECT                    (R_WD_REG_BASE + 0x4)
/*!
  comments
  */
#define R_M_WD_EN                                (R_WD_REG_BASE + 0x8)
/*!
  comments
  */
#define R_M_WD_FEED                            (R_WD_REG_BASE + 0xc)
/*!
  comments
  */
#define R_M_WD_STA_CLR                    (R_WD_REG_BASE + 0x10)
/*!
  comments
  */
#define R_M_WD_STATUS                        (R_WD_REG_BASE + 0x14)

/************************************************************
 * GPIO Register definition
 ************************************************************/
/*!
  comments
  */
#define R_GPIO_REG_BASE                        0xBF0A0000
/*!
  comments
  */
#define R_GPIO_0_WDATA                        (R_GPIO_REG_BASE + 0x0)
/*!
  comments
  */
#define R_GPIO_0_WR_EN                        (R_GPIO_REG_BASE + 0x4)
/*!
  comments
  */
#define R_GPIO_0_RDATA                        (R_GPIO_REG_BASE + 0x8)
/*!
  comments
  */
#define R_GPIO_0_MASK                        (R_GPIO_REG_BASE + 0xc)
/*!
  comments
  */
#define R_GPIO_1_WDATA                        (R_GPIO_REG_BASE + 0x10)
/*!
  comments
  */
#define R_GPIO_1_WR_EN                        (R_GPIO_REG_BASE + 0x14)
/*!
  comments
  */
#define R_GPIO_1_RDATA                        (R_GPIO_REG_BASE + 0x18)
/*!
  comments
  */
#define R_GPIO_1_MASK                        (R_GPIO_REG_BASE + 0x1c)
/*!
  comments
  */
#define R_GPIO_AO_WDATA                        (R_GPIO_REG_BASE + 0x20)
/*!
  comments
  */
#define R_GPIO_AO_WR_EN                        (R_GPIO_REG_BASE + 0x24)
/*!
  comments
  */
#define R_GPIO_AO_RDATA                        (R_GPIO_REG_BASE + 0x28)
/*!
  comments
  */
#define R_GPIO_AO_MASK                        (R_GPIO_REG_BASE + 0x2c)
/*!
  comments
  */
#define R_GPIO_0_MODE                        (R_GPIO_REG_BASE + 0x100)
/*!
  comments
  */
#define R_GPIO_0_STA_CLR                        (R_GPIO_REG_BASE + 0x104)
/*!
  comments
  */
#define R_GPIO_0_STA_CLR_R                    (R_GPIO_REG_BASE + 0x108)
/*!
  comments
  */
#define R_GPIO_0_STATE                        (R_GPIO_REG_BASE + 0x10c)
/*!
  comments
  */
#define R_GPIO_1_MODE                        (R_GPIO_REG_BASE + 0x110)
/*!
  comments
  */
#define R_GPIO_1_STA_CLR                        (R_GPIO_REG_BASE + 0x114)
/*!
  comments
  */
#define R_GPIO_1_STA_CLR_R                    (R_GPIO_REG_BASE + 0x118)
/*!
  comments
  */
#define R_GPIO_1_STATE                        (R_GPIO_REG_BASE + 0x11c)
/*!
  comments
  */
#define R_GPIO_AO_MODE                        (R_GPIO_REG_BASE + 0x120)
/*!
  comments
  */
#define R_GPIO_AO_STA_CLR                    (R_GPIO_REG_BASE + 0x124)
/*!
  comments
  */
#define R_GPIO_AO_STA_CLR_R                    (R_GPIO_REG_BASE + 0x128)
/*!
  comments
  */
#define R_GPIO_AO_STATE                        (R_GPIO_REG_BASE + 0x12c)
/*!
  comments
  */
#define GPIO_INT0_SEL                       (R_GPIO_REG_BASE + 0x130)
/*!
  comments
  */
#define GPIO_INT1_SEL                       (R_GPIO_REG_BASE + 0x134)

/************************************************************
 * PINMUX Register definition
 ************************************************************/
/*!
  comments
  */
#define R_PIN_SEL_BASE        0xBF156000
/*!
  comments
  */
#define R_PIN8_SEL            (R_PIN_SEL_BASE + 0x0)
/*!
  comments
  */
#define R_PIN0_SEL            (R_PIN_SEL_BASE + 0x4)
/*!
  comments
  */
#define R_PIN1_SEL            (R_PIN_SEL_BASE + 0x8)
/*!
  comments
  */
#define R_PIN2_SEL            (R_PIN_SEL_BASE + 0xC)
/*!
  comments
  */
#define R_PIN3_SEL            (R_PIN_SEL_BASE + 0x10)
/*!
  comments
  */
#define R_PIN4_SEL            (R_PIN_SEL_BASE + 0x14)
/*!
  comments
  */
#define R_PIN5_SEL            (R_PIN_SEL_BASE + 0x18)
/*!
  comments
  */
#define R_PIN6_SEL            (R_PIN_SEL_BASE + 0x1C)
/*!
  comments
  */
#define R_PIN7_SEL            (R_PIN_SEL_BASE + 0x20)

/*!
  comments
  */
#define R_PIN_IE_BASE      0xBF156300
/*!
  comments
  */
#define R_IE0_SEL            (R_PIN_IE_BASE + 0x0)
/*!
  comments
  */
#define R_IE1_SEL            (R_PIN_IE_BASE + 0x4)
/*!
  comments
  */
#define R_IE2_SEL            (R_PIN_IE_BASE + 0x8)
/*!
  comments
  */
#define R_IE3_SEL            (R_PIN_IE_BASE + 0xC)

/*!
  comments
  */
#define R_FPGA_ID            (0xBF140000)
/*!
  comments
  */
#define R_CHIP_ID            (0xBF140004)
/*!
  comments
  */
#define R_CHIP_ID_RDEN            (0xBF140008)
/*!
  comments
  */
#define R_CHIP_CFG           (0xBF140024)

#endif
