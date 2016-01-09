/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__

/*!
    2-wire serial bus I/O control command
  */
typedef enum spi_ioctrl_cmd
{
    /*!
        Set 2-wire bus clock frequency
      */
    SPI_IOCTRL_SET_CLCK = 1,
    /*!
       Set 2-wire bus clock delay
      */    
    SPI_IOCTRL_SET_CLK_DELAY,
    /*!
        CS ----\
                \-----------------------
                         /--------------
        CLK ------------/
                |-------| <----CS2CLK_TIME
      */
    SPI_IOCTRL_SET_CS2CLK_TIME,
    /*!
       Transporting that start low bit to high bit.
      */
    SPI_IOCTRL_ENABLE_LSB,
    /*!
       Transporting that start high byte to low byte.
      */
    SPI_IOCTRL_ENABLE_HIGH_BYTE_FIRST,
    /*!
	   delay between transport a data unit and next one.
      */
    SPI_IOCTRL_SET_TRANS_UNIT_DELAY,
    /*!
     How many IOs are needed by transporting.
      */
    SPI_IOCTRL_SET_TRANS_IONUM,
    SPI_IOCTRL_SET_DDR,
    SPI_IOCTRL_SET_DMA,
    SPI_IOCTRL_SET_SECURE
} spi_ioctrl_cmd_t;


/*!
    2-wire bus master device
  */
typedef struct spi_bus
{
    /*!
        base
      */
    void *p_base;
    /*!
        priv
      */
    void *p_priv;
} spi_bus_t;

/*!
   The structure is spi pins configure
*/
typedef struct spi_pins_cfg
{
    /*!
        miso1 --> spiio(n) n=0 or 1
      */
  u8 miso1_src;
    /*!
        miso0 --> spiio(n) n=0 or 1
      */    
  u8 miso0_src;
    /*!
        spiio1 <-- mosi(n) n=0 or 1
      */    
  u8 spiio1_src;
    /*!
        spiio0 <-- mosi(n) n=0 or 1
      */    
  u8 spiio0_src;
} spi_pins_cfg_t;

/*!
   The structure is spi pins direction
*/
typedef struct spi_pins_dir
{
    /*!
        00:input, 01:output, other:bidirectional
      */
    u8 spiio1_dir;
    /*!
        00:input, 01:output, other:bidirectional
      */
    u8 spiio0_dir;
} spi_pins_dir_t;

/*!
   The structure is how many IO numbers have been 
   used when spi's commands are sended and length
   of cmd0 and cmd1.
*/
typedef struct spi_cmd_cfg
{
    /*!
        todo
      */
  u8 cmd0_ionum;
    /*!
        todo
      */  
  u8 cmd0_len;
    /*!
        todo
      */  
  u8 cmd1_ionum;
    /*!
        todo
      */ 
  u8 cmd1_len;
  /*!
    comments
    */
  u8 dummy;
} spi_cmd_cfg_t;

/*!
    The structure is used to configure the 2-wire bus driver.    
  */
typedef struct spi_cfg
{
    /*!
        2-wire bus clock frequency in KHz
      */
    u16 bus_clk_mhz;
    /*!
        spi clock delay.
		Max delay:15 clocks
		Min delay:0 clocks
		0 <= bus_clk_delay <= 15
      */
    u8 bus_clk_delay;
    /*!
        The locking mode of function call, see dev_lock_mode
      */    
    u8 lock_mode;
    /*!
        The ID of i2c controller
      */    
    u8 spi_id;
    /*!
        How may io pins are used when spi transmit
      */
    u8 io_num;
    /*!
        SPI has four operation mode,like below.
        SPI_OP_MODE0: CPHA=0 , CPOL=0
        SPI_OP_MODE1: CPHA=0 , CPOL=1
        SPI_OP_MODE2: CPHA=1 , CPOL=0
        SPI_OP_MODE3: CPHA=1 , CPOL=1
        op_mode=0 or 1 or 2 or 3
      */
    u8 op_mode;

    /*!
       exp:when spi uses one pin to transmit data.
	   if
	     miso0 <-- spiio1
	     mosi0 --> spiio0
	   then
	     pins_cfg.miso1_src = 0
	     pins_cfg.miso0_src = 1
	     pins_cfg.spiio1_src = 0
	     pins_cfg.spiio0_src = 0

	   miso1_src:
         00 SPIIO0
         01 SPIIO1
	   miso0_src:
         00 SPIIO0
         01 SPIIO1
	   spiio1_src:
         00 mosi0
         01 mosi1
	   spiio0_src:
         00 mosi0
         01 mosi1
      */
    spi_pins_cfg_t pins_cfg[2];

    /*!
      spi pins direction
     */
    spi_pins_dir_t  pins_dir[2];
} spi_cfg_t;

/*!
   Send data in unit of bytes to appointed slave.

   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer.
   \param[in] len The data length.
   \param[in] param This parameter depends on platforms, see i2c_param_t.
   
   \return 0 for success and others for failure.
  */
RET_CODE spi_read(spi_bus_t *p_dev,  u8 *p_cmd_buf, u32 cmd_len, 
                                  spi_cmd_cfg_t * p_cmd_cfg, u8 *p_data_buf, u32 data_len);
/*!
   Receive data in unit of bytes from appointed slave.

   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer.
   \param[in] len The data length.
   \param[in] param This parameter depends on platforms, see i2c_param_t.

   \return 0 for success and others for failure.
  */
RET_CODE spi_write(spi_bus_t *p_dev, u8 *p_cmd_buf, u32 cmd_len, 
                                   spi_cmd_cfg_t * p_cmd_cfg, u8 *p_data_buf, u32 data_len);

/*!
   Receive data in unit of bytes from appointed slave.

   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer.
   \param[in] len The data length.
   \param[in] param This parameter depends on platforms, see i2c_param_t.

   \return 0 for success and others for failure.
  */
RET_CODE spi_reset(spi_bus_t *p_dev);

/*!
   Receive data in unit of bytes from appointed slave.

   \param[in] p_dev The device handle.
   \param[in] slv_addr The 2-wire bus address of the slave. Note that (slv_addr>>1)<=112 following the 2-wire bus protocol.
   \param[in] p_buf The data buffer.
   \param[in] len The data length.
   \param[in] param This parameter depends on platforms, see i2c_param_t.

   \return 0 for success and others for failure.
  */
RET_CODE spi_soft_reset(spi_bus_t *p_dev);

#endif //__SPI_H__

