/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2010
/-----------------------------------------------------------------------*/

#ifndef _DISKIO

/*!
1: Remove write functions 
*/
#define _READONLY	0	
/*!
 1: Use disk_ioctl fucntion
*/
#define _USE_IOCTL	1

#include "integer.h"
#include "sys_types.h"

/*!
 Status of Disk Functions
*/
typedef BYTE	DSTATUS;

/*! 
Results of Disk Functions 
*/
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;



/*!
Prototypes for disk control functions 
*/
int assign_drives (int, int);
#if 0
/*!
initialize
*/
DSTATUS disk_initialize (lld_block_t *);
/*!
status
*/
DSTATUS disk_status (lld_block_t *);
/*!
read
*/
DRESULT disk_read (lld_block_t *, DWORD, BYTE, BYTE*);
/*!
write
*/
#if	_READONLY == 0
DRESULT disk_write (lld_block_t *, DWORD, BYTE, const BYTE*);
#endif
/*!
command
*/
DRESULT disk_ioctl (lld_block_t *, BYTE, void*);

#endif

/*!
Disk Status Bits (DSTATUS) 
*/
/*!
Drive not initialized 
*/
#define STA_NOINIT		0x01	
/*!
No medium in the drive 
*/
#define STA_NODISK		0x02	
/*!
Write protected 
*/
#define STA_PROTECT		0x04	




 /*!
 Test for Win32 
 */
#define WIN32_FS  

#define _DISKIO

struct win32_device
{
	RET_CODE  (* write_sector_ptr)(struct drv_dev *, u32 , u32 , u8 *);
	RET_CODE  (* read_sector_ptr)(struct drv_dev *, u32 , u32 , u8 *);
	RET_CODE  (* device_ioctrl)(struct drv_dev *, u32 , u8*);
	RET_CODE  (* device_getstatus)(struct drv_dev *);
	
};

/*!
    win32_dev bus driver low level structure as the private data("priv") of struct "i2c_bus_t"
  */
typedef struct lld_win32_dev {
  /*!
      The private data of low level driver.
    */
  void *p_priv;
  	RET_CODE  (* write_sector_ptr)(struct drv_dev *, u32 , u32 , u8 *);
	RET_CODE  (* read_sector_ptr)(struct drv_dev *, u32 , u32 , u8 *);
	RET_CODE  (* device_ioctrl)(struct drv_dev *, u32 , u8*);
	RET_CODE  (* device_getstatus)(struct drv_dev *);
}lld_win32_dev_t;

#endif
