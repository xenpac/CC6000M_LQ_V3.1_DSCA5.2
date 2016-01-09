/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BLOCK_H__
#define __BLOCK_H__
/*!
      BLOCK bus event to notify uplayer
  */
typedef enum {
/*!
      event to indicate device connection to bus
  */
  BLOCK_BUS_DEV_CONNECT = 1,
/*!
     event to indicate device removed from bus
  */
  BLOCK_BUS_DEV_REMOVE,
}block_dev_event_t;

// BLOCK device type
/*!
      block_dev_type
  */
typedef enum {
/*!
     usb-mass-storage class
  */
  BLOCK_DEV_USB_MASS = 1,
/*!
     memery-card class
  */
  BLOCK_DEV_MEM_CARD,
/*!
     nand flash on board
  */
  BLOCK_DEV_NAND,
 /*!
     win 32 fake device
  */
  BLOCK_DEV_WIN32_FAKE,
/*!
    the number of device type
  */
  BLOCK_DEV_TYPE_NUM
}block_dev_type_t;

/************************  for upper layer interface****************/
enum block_data_dir {
/*!
    data from BLOCK device to host
  */
  BLOCK_DIR_IN = 1,
/*!
    data from host to BLOCK device
  */
  BLOCK_DIR_OUT
};

/*!
    block request struct
  */
typedef struct block_xfer_req {
/*!
     data direction
  */
  enum block_data_dir data_dir;
/*!
   start logic address of first block
  */
  u32 start_sec;
/*!
   the block number
  */
  u32 sec_num;
/*!
    data buffer, if NULL, indicate need push data
  */
  u8 *p_buf;
}block_xfer_req_t;
/*!
    BLOCK command request block, used for asynchronous operation to BLOCK device
  */
typedef struct block_tr {
/*!
    crb list node
  */
  struct list_head list;
/*!
    transfer request list
  */
  block_xfer_req_t xfer_req;
/*!
   the context infomation to be transfered
  */
  u32 context;
/*!
   priv
  */
  void *priv;
/*!
    command complete callback function
  */
  void(*complete)(struct block_tr *);
/*!
    command result
  */
  RET_CODE result;
}block_tr_t;
/*!
     max num support block devices
  */
#define NR_BLK_DEV 5
/*!
   comlete callback function for BLOCK command request
  */
typedef void (*BLOCK_TR_COMPLETE)(struct block_tr *trb);
/*!
   bus event notify function
  */
typedef void (*BLOCK_EVT_NOTIFY)(char * name, block_dev_event_t event);

/*!
   block layer configure
  */
typedef struct ufs_register_notify
{
/*!
    when block driver  find block device inserted or remorved will callack this function
  */
  BLOCK_EVT_NOTIFY notify;
 }ufs_register_notify_t;
 
/*!
   device controller info, to distinguish specail contoller from default one by specify this field
 */
 typedef enum{
     /*!
       for default USB & SD controller
       */
      BLOCK_CONTROLLER_DEFAULT = 0,
      /*!
       for SD controller, Z5 CHIP
       */
      BLOCK_CONTROLLER_SD_Z5,
      /*!
        max id
        */
      BLOCK_CONTROLLER_MAX
 }block_dev_controller_type_t;
/*!
   block layer configure
  */
typedef struct block_device_config
{
/*!
   support TR mode
  */
  #define SUPPORT_TR 1
/*!
  unsupport TR mode
  */
  #define UNSUPPORT_TR 0
/*!
   config block layer support or unsupport tr mode
  */
  char  cmd;
/*!
   block device scheduler priority
 */
  u32 tr_submit_prio;
 /*!
   block device bus priority  // sd or usb bus priority
 */
  u32 bus_prio;
  /*!
   block device stack size
 */
  u32 bus_thread_stack_size;
   /*!
   block_usb_mass_stor_prio
 */
  u32 block_mass_stor_prio;
  /*!
   block device stack size
 */
  u32 block_mass_stor_thread_stack_size;
  /*!
   debug mode
 */
  u32 debug_mode;
  /*!
   api locking mode
 */
  u32 lock_mode;
  /*!
   device info, to distinguish specail contoller from default one by specify this field
   */
   block_dev_controller_type_t dev_info;
  /*!
   hub tt priority
  */
   u32 hub_tt_prio;
  /*!
   hub tt stack size
  */
   u32 hub_tt_stack_size;
    /*!
   select usb port
  */
   u8 select_usb_port_num;
}block_device_config_t;
/*!
  This structure defines block commands by block_io_ctrl.
  */
typedef enum
{
/*!
Command code for disk_ioctrl fucntion
 Generic command (defined for FatFs)
*/
/*!
Flush disk cache (for write functions)
*/
  CTRL_SYNC   = 0 ,
/*!
Get media size (for only f_mkfs())
*/
  GET_SECTOR_COUNT  = 1,
/*!
Get sector size (for multiple sector size (_MAX_SS >= 1024))
*/
  GET_SECTOR_SIZE   = 2 ,
/*!
Get erase block size (for only f_mkfs())
*/
 GET_BLOCK_SIZE   = 3 ,
/*!
Force erased a block of sectors (for only _USE_ERASE)
*/
  CTRL_ERASE_SECTOR = 4 ,
/*
init disk command
*/
  GET_STATUS  = 5,

/*!
Generic command
*/
/*!
Get/Set power status
*/
  CTRL_POWER    = 5,
/*!
Lock/Unlock media removal
*/
  CTRL_LOCK   = 6 ,
/*!
Eject media
*/
  CTRL_EJECT    = 7,
/*!
MMC/SDC specific ioctl command
*/
/*!
Get card type
*/
  MMC_GET_TYPE  = 10,
/*!
Get CSD
*/
  MMC_GET_CSD   = 11,
/*!
Get CID
*/
  MMC_GET_CID   = 12,
/*!
Get OCR
*/
  MMC_GET_OCR   = 13,
/*!
Get SD status
*/
  MMC_GET_SDSTAT  = 14,
/*!
ATA/CF specific ioctl command
*/
/*!
Get F/W revision
*/
  ATA_GET_REV   = 20,
/*!
Get model name
*/
  ATA_GET_MODEL = 21,
/*!
Get serial number
*/
  ATA_GET_SN    = 22,
/*!
NAND specific ioctl command
*/
/*
Create physical format
*/
  NAND_FORMAT   = 30,
    /*!
    suspend usb device.
    */
    USB_PORT_SUSPEND = 40,
    /*!
    resume usb device
    */
    USB_PORT_RESUME = 41,
    /*!
    block reset usb device
    */
    RESET_USB_BLOCK = 42,
    /*!
    usb total size
    */
    USB_TOTAL_SIZE = 43,
    /*!
    usb set debug mode
    */
    USB_SET_DEBUG_MODE,
/*
init disk command
*/
  DISK_INIT   = 0xEE,
}block_usb_cmd_t;

/*!
   block nand configure
  */
typedef struct block_nand_config
{
  /*!
  SPI Flash bus handle
    */
  void *p_bus;
  /*!
  SPI Flash rd mode
    */
  u8    rd_mode;
  /*!
  SPI Flash wr mdoe
    */
  u8    wr_mode;
}block_nand_config_t;

/*!
    SPI NAND Flash read mode
  */
typedef enum snand_read_mode
{
    /*!
        SPI Flash default mode,if the user don't know which to use.
      */
    SNAND_READ_DEFAULT_MODE = 0,
    /*!
        SPI Flash 1IO read mode, no mode phase
      */
    SNAND_RD_1IO_MODE,
    /*!
        SPI Flash 2IO read mode, no mode phase
      */
    SNAND_RD_2IO_MODE,
    /*!
        SPI Flash 2IO read mode, no mode phase
      */
    SNAND_RD_4IO_MODE,
    /*!
        SPI Flash x2 output read mode
      */
    SNAND_RD_DOR_MODE,
    /*!
        SPI Flash x4 output read mode
      */
    SNAND_RD_QOR_MODE,
    /*!
        SPI read oob data
      */
    SNAND_RD_OOB,
    /*!
        SPI read all page raw data
     */
    SNAND_RD_ALL_PAGE,

}snand_read_mode_t;

/*!
    SPI Flash Program mode
  */
typedef enum snand_program_mode{
    /*!
    SPI Flash default program mode
      */
    SNAND_PRGM_DEFAULT_MODE = 0,
    /*!
      SPI Flash 1IO page program mode, the address on 1 I/O pins
      */
    SNAND_WR_1IO_MODE,
    /*!
     SPI Flash 1IO page program mode, the address on 4 I/O pins
     */
    SNAND_WR_X4_MODE,
    /*!
      SPI Flash 4IO page program mode, the address on 4 I/O pins
      */
    SNAND_WR_4IO_MODE,
    /*!
     SPI write all page raw data
     */
    SNAND_WR_ALL_PAGE,
    /*!
     SPI write oob data 
     */
    SNAND_WR_OOB,

}snand_pgm_mode_t;

/*!
   SPI NAND IO CTRL CMD
  */
typedef enum spi_nand_io_ctrl
{

  SNAND_IO_GET_ID = 0,
  SNAND_IO_PROTECT_BLOCK,
  SNAND_IO_GET_FET,
  SNAND_IO_SET_FET,
  SNAND_IO_WR_EN,
  SNAND_IO_RESET,
  SNAND_SET_RD_MODE,
  SNAND_SET_WR_MODE,
  SNAND_IO_MAX
}spi_nand_io_ctrl_t;


/*!
  ufs register callback function
  \param[in] config  config command
 */
RET_CODE register_notify(ufs_register_notify_t *p_config);


/*!
  config block device
  \param[in] config  config command
 */
RET_CODE block_tr_task_config(drv_dev_t *p_dev, u32 tr_submit_prio);

/*!
  block sector read function

  \param[in] dev block device handle
  \param[in] sec_addr start  read sector address
  \param[in] sec_addr need read sector numbers
  \param[out] buf  point to read buff
 */
RET_CODE block_sector_read(drv_dev_t *p_dev, u32 sec_addr, u32 sec_num, u8 *p_buf);

/*!
  block sector write function

  \param[in] dev block device handle
  \param[in] sec_addr start  read sector address
  \param[in] sec_addr need read sector numbers
  \param[in] buf  point to write buff
 */
RET_CODE block_sector_write(drv_dev_t *p_dev, u32 sec_addr, u32 sec_num, u8 *p_buf);
/*!
  BLOCK command request list, used for asynchronous operation to BLOCK device
  \param[in] dev block device handle
  \param[in] request list
 */
RET_CODE block_tr_submit(drv_dev_t *p_dev, struct block_tr *p_btr);
/*!
  BLOCK io control function, use to control device and get attribute
  \param[in] p_dev block device handle
  \param[in] cmd command you want to send
  \param[in] p_buf param you want to send
 */
RET_CODE block_io_ctrl(drv_dev_t *p_dev, u32 cmd, u8 *p_buf);
/*!
  BLOCK erase, use to erase device
  \param[in] p_dev block device handle
  \param[in] block block start address you want to erase
  \param[in] sector_nr block num you want to erase
 */
RET_CODE block_erase(drv_dev_t *p_dev, u32 block, u32 sector_nr);
/*!
  BLOCK device data move interal, use to move data
  \param[in] p_dev block device handle
  \param[in] src source address you want to move from
  \param[in] dst dest address you want to move to
  \param[in] buffer pointer of data you want to move
  \param[in] len data length you want to move
 */
RET_CODE interal_data_move(drv_dev_t *p_dev, u32 src, u32 dst, u8 *buffer, u32 len);

#endif  // end of BLOCK_H
