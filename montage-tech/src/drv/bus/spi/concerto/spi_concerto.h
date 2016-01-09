/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __CONCERTO_SPI_H__
#define __CONCERTO_SPI_H__

/************************************************************************ 
 * SPI Controller
 ************************************************************************/
#define CONCERTO_SPI_MEM_BASE                       0x9E000000 
#define SPIN_BASE_ADDR                              0xBF010000 
#define R_SPIN_TXD(i)                               ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x100))
#define R_SPIN_RXD(i)                               ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x100))
#define R_SPIN_MEM_CMD(i)                           ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x04))
#define R_SPIN_BAUD(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x08))
#define R_SPIN_MODE_CFG(i)                          ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x0c))
#define R_SPIN_INT_CFG(i)                           ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x10))
#define R_SPIN_TC(i)                                ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x14))
#define R_SPIN_CTRL(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x18))
#define R_SPIN_STA(i)                               ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x1c))
#define R_SPIN_PIN_CFG_IO(i)                        ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x20))
#define R_SPIN_MEM_BOOT(i)                          ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x24))
#define R_SPIN_MEM_CTRL(i)                          ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x28))
#define R_SPIN_PIN_MODE(i)                          ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x2c))
#define R_SPIN_DEBUG(i)                             ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x30))
#define R_SPIN_MOSI_CTRL(i)                         ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x34))
#define R_SPIN_MEMMODE_CTRL1(i)                     ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x40))
#define R_SPIN_MEMMODE_CTRL2(i)                     ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x44))
#define R_SPIN_MEMMODE_RDY_DLY(i)                   ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x38))
#define R_SPIN_INT_STATUE(i)                        ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x3c))


#endif //__CONCERTO_SPI_H__

