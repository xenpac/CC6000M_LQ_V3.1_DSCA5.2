/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BUS_I2C_PRIV_H__
#define __BUS_I2C_PRIV_H__

/*!
    I2C bus driver low level structure as the private data("priv") of struct "i2c_bus_t"
  */
typedef struct lld_i2c {
  /*!
      The private data of low level driver.
    */
  void *p_priv;
  /*!
      The low level function to implement the high level interface "i2c_write".
    */
  RET_CODE (*write)(struct lld_i2c *p_lld, u8 slv_addr, u8 *p_buf, 
                                       u32 len, u32 param);
  /*!
      The low level function to implement the high level interface "i2c_read".
    */  
  RET_CODE (*read)(struct lld_i2c *p_lld, u8 slv_addr, u8 *p_buf, 
                                       u32 len, u32 param);
  /*!
      The low level function to implement the high level interface "i2c_seq_read".
    */   
  RET_CODE (*seq_read)(struct lld_i2c *p_lld, u8 slv_addr, u8 *p_buf, 
                                        u32 wlen, u32 rlen, u32 param);
  /*!
      The low level function to implement the high level interface "i2c_std_read".
    */  
  RET_CODE (*std_read)(struct lld_i2c *p_lld, u8 slv_addr, u8 *p_buf, 
                                       u32 wlen, u32 rlen, u32 param);
  /*!
      The low level function to implement the high level interface "i2c_raw_write_byte".
    */  
  RET_CODE (*raw_write_byte)(struct lld_i2c *p_lld, u8 data, u8 b_start);
  /*!
      The low level function to implement the high level interface "i2c_raw_read_byte".
    */  
  RET_CODE (*raw_read_byte)(struct lld_i2c *p_lld, u8 *p_data, u8  b_nack);
  /*!
      The low level function to implement the high level interface "i2c_raw_stop".
    */  
  RET_CODE (*raw_stop)(struct lld_i2c *p_lld);
}lld_i2c_t;

#endif //__BUS_I2C_PRIV_H__

