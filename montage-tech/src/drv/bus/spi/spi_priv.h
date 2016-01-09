/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BUS_SPI_PRIV_H__
#define __BUS_SPI_PRIV_H__

/*!
    SPI bus driver low level structure as the private data("priv") of struct "i2c_bus_t"
  */
typedef struct lld_spi {
  /*!
      The private data of low level driver.
    */
  void *p_priv;
  /*!
      The low level function to implement the high level interface "i2c_write".
    */
  RET_CODE (*read)(struct lld_spi *p_lld, u8 *cmd_buf, u32 cmd_len, 
    spi_cmd_cfg_t * p_cmd_cfg, u8 *p_data_buf, u32 data_len);
  /*!
      The low level function to implement the high level interface "i2c_read".
    */  
  RET_CODE (*write)(struct lld_spi *p_lld, u8 *cmd_buf, u32 cmd_len, 
    spi_cmd_cfg_t * p_cmd_cfg, u8 *p_data_buf, u32 data_len);

  /*!
      The low level function to implement the high level interface "i2c_read".
    */
  RET_CODE (*reset)(struct lld_spi *p_lld);
  /*!
      The low level function to implement the high level interface "i2c_read".
    */
  RET_CODE (*soft_reset)(struct lld_spi *p_lld);
}lld_spi_t;

#endif //__BUS_SPI_PRIV_H__

