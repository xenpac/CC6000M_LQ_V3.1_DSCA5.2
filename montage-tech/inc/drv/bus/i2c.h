/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __I2C_H__
#define __I2C_H__

/*!
    2-wire serial bus I/O control command
  */
typedef enum i2c_ioctrl_cmd 
{
    /*!
        Set 2-wire bus clock frequency
      */
    I2C_IOCTRL_SET_CLOCK = 1,
    /*!
        Stop 2-wire bus
      */    
    I2C_IOCTRL_STOP,
     /*!
        2-wire bus interrupt mode
    */     
    I2C_IOCTL_INTERRUPT_MODE,
     /*!
        2-wire bus polling mode
    */     
    I2C_IOCTL_POLLING_MODE,
}i2c_ioctrl_cmd_t;

/*!
    Used for the parameter of "param" of 2-wire bus API
  */
typedef enum i2c_param
{
    /*!
        Switch the 2-wire bus to the connection to SOC external slave
      */
    I2C_PARAM_DEV_SOC_EXTER = 1,
    /*!
        Switch the 2-wire bus to the connection to SOC internal slave
      */
    I2C_PARAM_DEV_SOC_INTER = 2,

    /*!
        Write byte stop for awhile before  next byte write
      */
    I2C_PARAM_DEV_WRITE_STOP_AWHILE = 4
} i2c_param_t;

/*!
    2-wire bus master device
  */
typedef struct i2c_bus
{
    /*!
        base
      */
    void *p_base;
    /*!
        priv
      */
    void *p_priv;
} i2c_bus_t;

/*!
    The structure is used to configure the 2-wire bus driver.    
  */
typedef struct i2c_cfg
{
    /*!
        2-wire bus clock frequency in KHz
      */
    u16 bus_clk_khz;
    /*!
        The GPIO number of the GPIO pin assigned as SDA I/O. This is only used when the 2-wire bus is implemented by GPIOs. 
      */    
    u8 sda;
    /*!
        The GPIO number of the GPIO pin assigned as SCL Output. This is only used when the 2-wire bus is implemented by GPIOs. 
      */    
    u8 scl;
    /*!
        The bus handle in lld
      */    
    u32 lld_bus;
    /*!
        The locking mode of function call, see dev_lock_mode
      */    
    u8 lock_mode;
    /*!
        The ID of i2c controller
      */    
    u8 i2c_id;
} i2c_cfg_t;

/*!
   Send data in unit of bytes to appointed slave.
   This function shouldn't be called in interrupt.
   
   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer.
   \param[in] len The data length.
   \param[in] param This parameter depends on platforms, see i2c_param_t.
   
   \return 0 for success and others for failure.
  */
RET_CODE i2c_write(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, 
                                    u32 len, u32 param);

/*!
   Receive data in unit of bytes from appointed slave.
   This function shouldn't be called in interrupt.
   
   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer.
   \param[in] len The data length.
   \param[in] param This parameter depends on platforms, see i2c_param_t.

   \return 0 for success and others for failure.
  */
RET_CODE i2c_read(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, 
                                   u32 len, u32 param);

/*!
   Receive data from appointed slave: the master sends data bytes first and does not send stop bit before receiving data. 
   start-slave-data(w)-...-start-slave-data(r)-...-stop
   This function shouldn't be called in interrupt.
   
   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer into which the data to be sent is written first and then the received data.
   \param[in] wlen The length of data to be sent firstly.
   \param[in] rlen The length of data to be read out.
   \param[in] param This parameter depends on platforms, see i2c_param_t.

   \return 0 for success and others for failure.
  */
RET_CODE i2c_seq_read(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, 
                 u32 wlen, u32 rlen, u32 param);

/*!
   Receive data from appointed slave: the master sends data bytes first and then the stop bit before receiving data.
   start-slave-data(w)-...-stop-start-slave-data(r)-...-stop
   This function shouldn't be called in interrupt.
   
   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer into which the data to be sent is written first and then the received data.
   \param[in] wlen The length of data to be sent firstly.
   \param[in] rlen The length of data to be read out.
   \param[in] param This parameter depends on platforms, see i2c_param_t.   

   \return 0 for success and others for failure.
  */
RET_CODE i2c_std_read(i2c_bus_t *p_dev, u8 slv_addr, u8 *p_buf, 
                  u32 wlen, u32 rlen, u32 param);
/*!
   recieve 1 byte data.
   
   \param[in] p_dev The device handle.
   \param[in] p_data 1 byte data.
   \param[in] b_nack need ack?.

   \return 0 for success and others for failure.
  */
RET_CODE i2c_raw_read_byte(i2c_bus_t *p_dev, u8 *p_data, u8 b_nack);
/*!
   send 1 byte data.
   
   \param[in] p_dev The device handle.
   \param[in] data 1 byte data.
   \param[in] b_start start before send data.

   \return 0 for success and others for failure.
  */
RET_CODE i2c_raw_write_byte(i2c_bus_t *p_dev, u8 data, u8 b_start);
/*!
   stop.
   
   \param[in] p_dev The device handle.

   \return 0 for success and others for failure.
  */
RET_CODE i2c_raw_stop(i2c_bus_t *p_dev);

#endif //__I2C_H__

