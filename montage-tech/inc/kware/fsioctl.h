/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __FSIOCTL_H__
#define __FSIOCTL_H__

#ifdef __cplusplus
extern "C" 
{
  #endif

/*!
   Devid parttion table
  */
  #define DPT_BASE_ADDRESS  0x1BE
/*!
   one parttion infor length
  */
  #define P_LEN 16
/*!
   parttion status
  */
  #define P_ACTIVE  0x80
/*!
   inactive
  */
  #define P_INACTIVE  0x00
/*!
   file type fat16   <32M
  */
  #define FT_FAT16_L  0x04
/*!
   file type fat16  >32M
  */
  #define FT_FAT16_B  0x06
/*!
   file type ntfs
  */
  #define FT_NTFS 0x07
/*!
   fat32
  */
  #define FT_FAT32  0x0B
  /*!
     iocntl command
    */
  typedef enum  
  {
  /*!
     command mount fs
    */
    FS_MOUNT  = 0x01,
   /*!
      command unmount fs
     */
    FS_UNMOUNT  = 0x02,
    /*!
      command get fs free
     */
    FS_GETFREE  = 0x03,
  /*!
     make dir
    */
    FS_MKDIE = 0x31,
  /*!
     change dir
    */
    FS_CHDIR  = 0x32,
  /*!
     show path
    */
    FS_SHOW_PATH = 0x33,
   /*!
     creat seek linkmap for fast seek!
    */
    FS_SEEK_LINKMAP = 0x34,
   /*!
     creat seek linkmap for fast seek!
   */
    FS_TELL = 0x35,
   /*!
     open  file with len
    */
    FS_OPEN = 0x40,

   /*!
     direct write sect
    */
    FS_DEV_WRITE = 0x50,

  /*!
     direct  read sect
   */
    FS_DEV_READ = 0x51,

   /*!
  	direct write file  with  offset
     */
    FS_DIO_WRITE = 0x52,

   /*!
  	direct read file  with offset
    */
    FS_DIO_READ = 0x53,

   /*!
    format
    */
    FS_FORMAT  = 0xEE,
  /*!
     change parttion
    */
    FS_CHDRIVE  = 0xEF

  }cmd_ioctrl_t;
/*!
   Physical drive# on the Windows mapped as drive 0
  */
  #define PHY_DRV_START 1
/*!
    Last physical drives to be mapped
  */
  #define PHY_DRV_END 5
/*!
   device limit
  */
  #define DEVICE_MAX  (PHY_DRV_END - PHY_DRV_START)
/*!
   logic parttion limit
  */
  #define LOGIC_PARTTION  ((DEVICE_MAX) * 4)
/*!
   ramfs parttion
  */
  #define RAMFS_PARTTION (LOGIC_PARTTION - 1)
/*!
   first partion id
  */
  #define BASE_PARTTION 'C'

/*!
   FAT file system limit
  */
  #define FAT_MAX 20
  /*!
     file status
    */
  typedef enum{
    FL_EASY,
    FL_BUSY
  }file_status_t;

  /*!
     fat parttion map table
    */
  typedef struct
  {
  /*!
     device
   */
    void *p_dev;

  /*!
     file volum
   */
    u8 volum;

  /*!
     flag
   */
    u8 flag;

  /*!
     parttion letter
   */
    u8 parttion;

  }fat_map_t;


  #ifdef __cplusplus
}
#endif

#endif
