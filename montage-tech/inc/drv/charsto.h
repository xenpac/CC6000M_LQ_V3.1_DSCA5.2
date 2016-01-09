/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CHARSTO_H__
#define __CHARSTO_H__

/*!
    Char-storage device's virtual sector size
  */
#define CHARSTO_MIN_SECTOR_SIZE (4 * 1024)

/*!
    Char-storage device's virtual sector size
  */
#define CHARSTO_SECTOR_SIZE (64 * 1024)

/*!
   The virtual sector size's bit offset, used for bit shift operation
  */
#define CHARSTO_SECTOR_SIZE_BO (16)

/*!
    SPI Flash read mode
  */
typedef enum spi_read_mode 
{
    /*!
        SPI Flash default mode,if the user don't know which to use.
      */ 
    SPI_READ_DEFAULT_MODE = 0,
    /*!
        SPI Flash normal read mode
      */    
    SPI_NR_MODE , 
    
    /*!
        SPI Flash fast read mode
      */
    SPI_FR_MODE,
    
    /*!
        SPI Flash dual output read mode
      */
    SPI_DOR_MODE, 
    
     /*!
        SPI Flash quad output read mode
      */
    SPI_QOR_MODE,
    
     /*!
        SPI Flash 2IO read mode, no mode phase
      */
    SPI_2IO_MODE,

     /*!
        SPI Flash 4IO read mode,have mode phase
      */
    SPI_4IO_QIOR_MODE,

    /*!
        SPI Flash dual I/O read mode, have mode phase
      */
    SPI_DIOR_MODE, 
   
}spi_read_mode_t;

/*!
    SPI Flash Program mode
  */
typedef enum spi_program_mode{
    /*!
    SPI Flash default program mode
      */
    SPI_PRGM_DEFAULT_MODE = 0,
 
    /*!
      SPI Flash page program mode
      */
    SPI_PP_MODE,

    /*!
      SPI Flash 4IO page program mode, the address on 4 I/O pins
      */
    SPI_4PP_MODE,

    /*!
      SPI Flash Quad page program read mode, the address on 1 I/O pin
      */
    SPI_QPP_MODE
}spi_pgm_mode_t;

/*!
    SPI Flash clock divider from the system clock
  */
typedef enum spi_clock
{
    /*!
    Fspi = Fsys / 2
      */
    FLASH_C_CLK_P2 = 1,
    /*!
    Fspi = Fsys / 4
      */    
    FLASH_C_CLK_P4,
    /*!
    Fspi = Fsys / 6
      */
    FLASH_C_CLK_P6,
    /*!
    Fspi = Fsys / 8
      */    
    FLASH_C_CLK_P8    
}spi_clock_t;

/*!
    SPI Flash Transfer Mode
  */
typedef enum spi_transfer_mode{
    /*!
    FLASH read/write by default control,used FLASH_CPU_RW_CACHE in default
      */  
    FLASH_DEFAULT_RW = 0,
    /*!
    FLASH read/write by software control
      */  
    FLASH_PIO_RW,
    /*!
    FLASH read/write by hardware cacheable control
      */  
    FLASH_CPU_RW_CACHE,
    /*!
    FLASH read/write by hardware nocacheable control
      */
    FLASH_CPU_RW_NOCACHE,
    /*!
    FLASH read/write by dma control
      */  
    FLASH_DMA_RW,

}spi_trs_mode_t;


/*!
    charsto sectors protect info
*/
typedef enum  spi_prot_block_type
{
  /*!
      block unprotection
   */
  PRT_UNPROT_ALL,
   /*!
     block protection all
  */
  PRT_PROT_ALL, 
  /*!
      block protection, up address 1/64
   */
  PRT_UPPER_1_64,
  /*!
      block protection, up address 1/32
   */
  PRT_UPPER_1_32,
   /*!
      block protection, up address 1/16
   */
  PRT_UPPER_1_16,
   /*!
       block protection, up address 1/8
    */
   PRT_UPPER_1_8,
   /*!
       block protection, up address 1/4
    */
   PRT_UPPER_1_4,
    /*!
       block protection, up address 1/2
    */
   PRT_UPPER_1_2,
  /*!
      block protection, up LOWER 1/64
   */
  PRT_LOWER_1_64,
  /*!
      block protection, up LOWER 1/32
   */
  PRT_LOWER_1_32,
   /*!
      block protection, up LOWER 1/16
   */
  PRT_LOWER_1_16,
  /*!
     block protection, up LOWER 1/8
  */
  PRT_LOWER_1_8,
  /*!
     block protection, up LOWER 1/4
  */
  PRT_LOWER_1_4,
  /*!
     block protection, up LOWER 1/2
  */
  PRT_LOWER_1_2,
  /*!
     block protection, up LOWER 3/4
  */
  PRT_LOWER_3_4,
  /*!
     block protection, up LOWER 7/8
  */
  PRT_LOWER_7_8,
  /*!
     block protection, up LOWER 15/16
  */
  PRT_LOWER_15_16, 
   /*!
     block protection, up LOWER 31/32
  */
  PRT_LOWER_31_32, 
   /*!
     block protection, up LOWER 63/64
  */
  PRT_LOWER_63_64, 

}spi_prot_block_type_t;

/*!
    Char storage device structure
  */
typedef struct charsto_device
{
    /*!
    Device base control
      */
    void *p_base;
    /*!
    Device private handle
      */    
    void *p_priv;
} charsto_device_t;

/*!
    charsto sectors protect info
*/
typedef struct charsto_prot_secs
{
  /*!
      sector protection, start sector 
   */
  u32 protect_s; 
  /*!
      sector protection, end sector  
   */
  u32 protect_e; 
}charsto_prot_secs_t;

/*!
    charsto sectors protect info
*/
typedef struct charsto_prot_status
{
  /*!
      status reg len
   */
  spi_prot_block_type_t prt_t;
  /*!
      status protect status
   */
  u32 st; 
  /*!
      status protect status mask
   */
  u32 st_mask; 
  /*!
      status reg len
   */
  u8 len;

}charsto_prot_status_t;

/*!
    charsto_protect_info
*/
typedef struct charsto_protect_info
{
  /*!
      manufactor id
   */
  u8 m_id;
  /*!
      manufactor id
   */
  charsto_prot_status_t prot_st;
  
}charsto_protect_info_t;

/*!
    Char storage I/O control cammand
  */
typedef enum charsto_iocrl_cmd
{
    /*!
        Check if it is a valid address in the storage device
      */
    CHARSTO_IOCTRL_IS_VALID_ADDR = DEV_IOCTRL_TYPE_UNLOCK + 0,
    /*!
        Get device's capacity size in bytes
      */    
    CHARSTO_IOCTRL_GET_CAPACITY = DEV_IOCTRL_TYPE_UNLOCK + 1,  
    /*!
        Check if the device is in busy status 
      */    
    CHARSTO_IOCTRL_CHECK_BUSY = DEV_IOCTRL_TYPE_UNLOCK + 2,
    /*!
        Enter sa 
      */    
    CHARSTO_IOCTRL_ENTER_SA = DEV_IOCTRL_TYPE_UNLOCK + 3,
    /*!
        Exit sa 
      */    
    CHARSTO_IOCTRL_EXIT_SA = DEV_IOCTRL_TYPE_UNLOCK + 4,
    /*!
        Some customers 's projects need this requirement
      */    
    CHARSTO_IOCTRL_ERASE_MIN_SECTOR = DEV_IOCTRL_TYPE_LOCK + 0,
    /*!
        Check if the whole char-storage device is protected
      */    
    CHARSTO_IOCTRL_CHECK_PROTECT = DEV_IOCTRL_TYPE_LOCK + 1,
     /*!
        protect specified block
      */    
    CHARSTO_IOCTRL_PROTECT_BLOCK_SET = DEV_IOCTRL_TYPE_LOCK + 2, 
    /*!
        protect specified block
      */    
    CHARSTO_IOCTRL_PROTECT_BLOCK_GET = DEV_IOCTRL_TYPE_LOCK + 3, 
    /*!
        set status by app
      */    
    CHARSTO_IOCTRL_SET_STATUS = DEV_IOCTRL_TYPE_LOCK + 6,
    /*!
        get status by app
      */    
    CHARSTO_IOCTRL_GET_STATUS = DEV_IOCTRL_TYPE_LOCK + 7,
    /*!
        get device id
      */    
    CHARSTO_IOCTRL_GET_DEV_ID = DEV_IOCTRL_TYPE_LOCK + 8,
    /*!
        get device unique id, for param refer to charsto_dev_uid_t
    */    
    CHARSTO_IOCTRL_GET_UNIQUE_ID = DEV_IOCTRL_TYPE_LOCK + 9,   
    /*!
        get device unique id, for param refer to charsto_dev_uid_t
    */    
    CHARSTO_IOCTRL_SET_CAPACITY = DEV_IOCTRL_TYPE_LOCK + 10,   
    /*!
        set reg2 LB3-LB1 = 1, to lock OTP
    */    
    CHARSTO_IOCTRL_LOCK_OTP = DEV_IOCTRL_TYPE_LOCK + 11,   
    /*!
        get otp locked status
    */    
    CHARSTO_IOCTRL_IS_LOCKED_OTP = DEV_IOCTRL_TYPE_LOCK + 12,       
} charsto_ioctrl_cmd_t;

/*!
    The device unique id
  */
typedef struct charsto_dev_uid
{
    /*!
      the buffer to get unique id
     */
    u8 *p_buf;
    /*!
      the unique id size in byte you want to get
      */
    u32 length;
}charsto_dev_uid_t;

/*!
    The structure used to configure the Char-storage driver
  */
typedef struct charsto_config
{
    /*!
    The valid memory size in the storage device,in KBytes unit, 4M = 4096
      */
    u32 size;
    /*!
    The SPI clock frequency, see enum spi_clock
      */    
    u8 spi_clk;
    /*!
    The specific reading mode in the SPI device, see enum spi_read_mode
      */    
    u8 rd_mode;
    /*!
    The write mode in the SPI device, see enum spi_program_mode
      */    
    u8 pgm_mode;
    /*!
    The data transfer mode, by software, hardware cpu , or DMA,see enum spi_trs_mode_t
      */
    u8 trs_modes;
    /*!
        Must describe the current charsto device mounted on which bus device
     */
    void *p_bus_handle;

    /*!
    The data transfer mode, by software, hardware cpu , or DMA,see enum spi_trs_mode_t
      */
   u32 lock_mode;
    /*!
        path for win32
     */
    u8 *path;

    /*!
        protect mod, 1 not do protect/unprotect operation
     */
    u32 prt_mod;
} charsto_cfg_t;


/*!
   Read random size byte from valid random-access address

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device 
   \param[in] buf The data buffer.
   \param[in] len The data length.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_read(charsto_device_t *p_dev, u32 addr, 
                    u8 *p_buf, u32 len);

/*!
   Write random size byte to valid random-access address. Note: only do write operation and do not include erasure.

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device 
   \param[in] buf The data buffer.
   \param[in] len The data length.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_writeonly(charsto_device_t *p_dev, u32 addr, 
                    u8 *p_buf, u32 len);

/*!
   Erase specific virtual sectors in unit of 64K bytes

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device. It can be the random address in the start sector to be erased.
   \param[in] sec_cnt The count of the virtual sectors to be erased.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_erase(charsto_device_t *p_dev, u32 addr, u32 sec_cnt);

/*!
   Protect the whole storage memory from programming(including writing and erasure).

   \param[in] dev The device handle.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_protect_all(charsto_device_t *p_dev);

/*!
   Un-protect the whole storage memory (i.e. allow the whole storage memory to be programmed).

   \param[in] dev The device handle.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_unprotect_all(charsto_device_t *p_dev);

/*!
   Get the flash unique ID.

   \param[in] dev The device handle.
   \param[out] buffer The output buffer for save flash unique ID
   \param[in] len The unique ID length will be read from flash.
   \param[out] actual length returned by driver
   \return 0 for success and others for failure.
  */
RET_CODE charsto_get_unique_id(charsto_device_t *p_dev, u8 *buffer, u32 len, u32 *act_len);

/*!
   Get the flash cacapcity.

   \param[in] dev The device handle.
   \param[out] get the cacapcity of the current flash
  */
RET_CODE charsto_get_cacapcity(charsto_device_t *p_dev, u32 *p_cap);




/*!
   Read otp data

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device 
   \param[in] buf The data buffer.
   \param[in] len The data length.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_otp_read(charsto_device_t *p_dev, u32 addr, 
                    u8 *p_buf, u32 len);

/*!
   Write otp data

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device 
   \param[in] buf The data buffer.
   \param[in] len The data length.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_otp_write(charsto_device_t *p_dev, u32 addr, 
                    u8 *p_buf, u32 len);

/*!
   Erase otp

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device. It can be the random address in the start sector to be erased.
   \param[in] sec_cnt The count of the virtual sectors to be erased.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_otp_erase(charsto_device_t *p_dev, u32 addr);







#endif //__CHARSTO_H__
