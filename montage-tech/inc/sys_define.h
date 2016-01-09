/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_DEFINE_H__
#define __SYS_DEFINE_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   Layer ID for hardware
  */
#define SYS_LAYER_HW        0x01
/*!
   Layer ID for software
  */
#define SYS_LAYER_SW        0x02
/*!
   Layer ID for engine
  */
#define SYS_LAYER_ENG       0x03
/*!
   Layer ID for device driver
  */
#define SYS_LAYER_DEV       0x04
/*!
   Layer ID for bus driver
  */
#define SYS_LAYER_BUS       0x05
/*!
   Nim device type
  */
#define SYS_DEV_TYPE_NIM ((SYS_LAYER_DEV << 8) + 0x01)
/*!
   Char storage device type
  */
#define SYS_DEV_TYPE_CHARSTO ((SYS_LAYER_DEV << 8) + 0x02)
/*!
   User input/output device type
  */
#define SYS_DEV_TYPE_UIO ((SYS_LAYER_DEV << 8) + 0x03)
/*!
   Video device type
  */
#define SYS_DEV_TYPE_VIDEO ((SYS_LAYER_DEV << 8) + 0x04)
/*!
   OSD device type
  */
#define SYS_DEV_TYPE_OSD ((SYS_LAYER_DEV << 8) + 0x05)
/*!
   PTI device type
  */
#define SYS_DEV_TYPE_PTI ((SYS_LAYER_DEV << 8) + 0x06)
/*!
   Audio device type
  */
#define SYS_DEV_TYPE_AUDIO ((SYS_LAYER_DEV << 8) + 0x07)
/*!
   AV sync device type
  */
#define SYS_DEV_TYPE_AVSYNC ((SYS_LAYER_DEV << 8) + 0x08)
/*!
   Graphics process engine device type
  */
#define SYS_DEV_TYPE_GPE ((SYS_LAYER_DEV << 8) + 0x09)
/*!
   Sub device type
  */
#define SYS_DEV_TYPE_SUB  ((SYS_LAYER_DEV << 8) + 0x0A)
/*!
   RF device type
  */
#define SYS_DEV_TYPE_RF ((SYS_LAYER_DEV << 8) + 0x0C)
/*!
   SCART device type
  */
#define SYS_DEV_TYPE_SCART ((SYS_LAYER_DEV << 8) + 0x0D)
/*!
   Smart card device type
  */
#define SYS_DEV_TYPE_SMC ((SYS_LAYER_DEV << 8) + 0x0E)

/*!
   Ethernet card device type
  */
#define SYS_DEV_TYPE_ETH ((SYS_LAYER_DEV << 8) + 0x10)
/*!
   Display device type
  */
#define SYS_DEV_TYPE_DISPLAY ((SYS_LAYER_DEV << 8) + 0x11)
/*!
   GPE new device type
  */
#define SYS_DEV_TYPE_GPE_VSB ((SYS_LAYER_DEV << 8) + 0x12)
/*!
   Video decoder device type
  */
#define SYS_DEV_TYPE_VDEC_VSB ((SYS_LAYER_DEV << 8) + 0x13)
/*
 usb_mass_storage_dev
*/
#define SYS_DEV_TYPE_BLOCK ((SYS_LAYER_DEV << 8) + 0x14)
/*
  Power manager
*/
#define SYS_DEV_TYPE_POW ((SYS_LAYER_DEV << 8) + 0x15)
/*
  Picture decoder
*/
#define SYS_DEV_TYPE_PDEC ((SYS_LAYER_DEV << 8) + 0x16)
/*!
   VBI inserter device type
  */
#define SYS_DEV_TYPE_VBI_INSERTER ((SYS_LAYER_DEV << 8) + 0x17)
/*!
   HDMI device type
  */
#define SYS_DEV_TYPE_HDMI ((SYS_LAYER_DEV << 8) + 0x18)
/*!
   CIC device type
  */
#define SYS_DEV_TYPE_CIC ((SYS_LAYER_DEV << 8) + 0x19)
/*!
   dsp device type
  */
#define SYS_DEV_TYPE_DSP ((SYS_LAYER_DEV << 8) + 0x20)
/*!
   lpower vsb type
   */
#define SYS_DEV_TYPE_POW_VSB ((SYS_LAYER_DEV << 8) + 0x21)
/*!
   wifi device type
  */
#define SYS_DEV_TYPE_USB_WIFI ((SYS_LAYER_DEV << 8) + 0x22)
/*
usb ethernet device
*/
#define SYS_DEV_TYPE_USB_3G_ETHER ((SYS_LAYER_DEV << 8) + 0x23)
/*!
   SYS_DEV_TYPE_USB_CAMERA
  */
#define SYS_DEV_TYPE_USB_CAMERA ((SYS_LAYER_DEV << 8) + 0x23)
/*
win32 fake device
*/
#define SYS_DEV_TYPE_WIN32_FAKE_DEV ((SYS_LAYER_DEV << 8) + 0x24)
/*
usb ethernet device
*/
#define SYS_DEV_TYPE_USB_ETHERNET_DEV ((SYS_LAYER_DEV << 8) + 0x25)
/*
ap add/remove av task fake device
*/
#define SYS_DEV_TYPE_AP_AV_FAKE_DEV ((SYS_LAYER_DEV << 8) + 0x26)
/*!
   Ethernet card device type
  */
#define SYS_DEV_TYPE_SPI_ETH ((SYS_LAYER_DEV << 8) + 0x27)
/*!
  secure conctrl module
  */
#define SYS_DEV_TYPE_SCTRL         ((SYS_LAYER_DEV << 8) + 0x28)
/*!
  modem module
  */
#define SYS_DEV_TYPE_MODEM         ((SYS_LAYER_DEV << 8) + 0x29)
/*!
 usb-tty module
  */
#define SYS_DEV_TYPE_USB_TTY         ((SYS_LAYER_DEV << 8) + 0x30)
/*!
 usb-hid: joystick
  */
#define SYS_DEV_TYPE_USB_HID_JOYSTICK         ((SYS_LAYER_DEV << 8) + 0x31)
/*!
usb-hid: Scanner 
*/
#define SYS_DEV_TYPE_USB_HID_SCANNER         ((SYS_LAYER_DEV << 8) + 0x32)
/*!
usb-ccid module 
*/
#define SYS_DEV_TYPE_USB_CCID        ((SYS_LAYER_DEV << 8) + 0x33)

/*!
   Graphics process engine device type
  */
#define SYS_DEV_TYPE_AUX_GPE ((SYS_LAYER_DEV << 8) + 0x34)


/*!
   UART hardware type
  */
#define SYS_HAL_TYPE_UART ((SYS_LAYER_HW << 8) + 0x01)
/*!
   GPIO hardware type
  */
#define SYS_HAL_TYPE_GPIO ((SYS_LAYER_HW << 8) + 0x02)
/*!
   TIMER hardware type
  */
#define SYS_HAL_TYPE_TIMER ((SYS_LAYER_HW << 8) + 0x03)
/*!
   DMA hardware type
  */
#define SYS_HAL_TYPE_DMA ((SYS_LAYER_HW << 8) + 0x04)
/*!
   WATCHDOG hardware type
  */
#define SYS_HAL_TYPE_WDG ((SYS_LAYER_HW << 8) + 0x05)
/*!
   MISC hardware type
  */
#define SYS_HAL_TYPE_MISC ((SYS_LAYER_HW << 8) + 0x06)
/*!
   Global_info hardware type
  */
#define SYS_HAL_TYPE_GLB_INFO ((SYS_LAYER_HW << 8) + 0x07)
/*!
   CPU hardware type
  */
#define SYS_HAL_TYPE_CPU ((SYS_LAYER_HW << 8) + 0x08)

/*!
   UART 1 hardware type
  */
#define SYS_HAL_TYPE_UART_1 ((SYS_LAYER_HW << 8) + 0x09)

/*!
   OTP hardware type
  */
#define SYS_HAL_TYPE_OTP ((SYS_LAYER_HW << 8) + 0x0a)


/*!
   I2C device type
  */
#define SYS_BUS_TYPE_I2C ((SYS_LAYER_BUS << 8) + 0x01)

/*!
   SPI device type
  */
#define SYS_BUS_TYPE_SPI ((SYS_LAYER_BUS << 8) + 0x02)

/*!
   Driver service type
  */
#define SYS_SW_TYPE_DRVSVC ((SYS_LAYER_SW << 8) + 0x01)

/*!
   Module ID for unknown
  */
#define SYS_MODULE_UNKNOWN           0
/*!
   Module ID for test
  */
#define SYS_MODULE_MEM_TEST          1
/*!
   Module ID for video
  */
#define SYS_MODULE_MEM_VIDEO         2
/*!
   Module ID for GDI
  */
#define SYS_MODULE_MMI               3
/*!
   Module ID for GDI
  */
#define SYS_MODULE_GDI               4
/*!
   Module ID for GUI control
  */
#define SYS_MODULE_GUI_CONTROL       5
/*!
   Module ID for EPG
  */
#define SYS_MODULE_EPG               6
/*!
   Module ID for info service
  */
#define SYS_MODULE_INFO_SERVICE      7
/*!
   Module ID for GUI resources
  */
#define SYS_MODULE_GUI_RESOURCES     8
/*!
   Module ID for framework
  */
#define SYS_MODULE_FRAMEWORK         9
/*!
   Module ID for DVBS
  */
#define SYS_MODULE_DVBS              10
/*!
   Module ID for tools
  */
#define SYS_MODULE_TOOLS             11
/*!
   Module ID for GUI
  */
#define SYS_MODULE_GUI               12
/*!
   Module ID for PTI
  */
#define SYS_MODULE_PTI               13
/*!
   Module ID for flash test
  */
#define SYS_MODULE_FLASH_TEST        14
/*!
   Module ID for DMA test
  */
#define SYS_MODULE_DMA_TEST          15
/*!
   Module ID for shell
  */
#define SYS_MODULE_SHELL             16
/*!
   Module ID for system
  */
#define SYS_MODULE_SYSTEM            17
/*!
   Module ID for APP test
  */
#define SYS_MODULE_APP_TEST          18
/*!
   Module ID for devices
  */
#define SYS_MODULE_DEVICE            19
/*!
   Module ID for OTA
  */
#define SYS_MODULE_OTA               20
/*!
   Module ID for si monitor
  */
#define SYS_MODULE_SI_MON            21
/*!
   Module ID for scan
  */
#define SYS_MODULE_SCAN              22
/*!
   Module ID for satcodx
  */
#define SYS_MODULE_SATCODX           23
/*!
   Module ID for util
  */
#define SYS_MODULE_UTIL              24
/*!
   Module ID for AP_PLAYBACK
  */
#define SYS_MODULE_PLAYBACK          25
/*!
  Module ID for AP_TIME
  */
#define SYS_MODULE_TIME              26
/*!
  Module ID for AP_UPG
  */
#define SYS_MODULE_UPG              27
/*!
  Module ID for data manager
  */
#define SYS_MODULE_DM              28
/*!
  Module ID for AP_TKGS
  */
#define SYS_MODULE_TKGS            29

#ifdef WIN32
/*!
   Assert macro in WIN32
  */
 #ifdef DISABLE_FUNCTION
 #define MT_ASSERT(x) do {} while(0)
 #else
#define MT_ASSERT(condition) \
  do \
  { \
    if((condition) != TRUE) \
    { \
      extern void PC_ThrowException(char *msg); \
      extern void PC_CloseSchHandle(void); \
      extern void PC_DebugBreak(void); \
      extern void PC_MessageBox(char* msg); \
      char dbg_msg[256] = {0}; \
      sprintf(dbg_msg, "\nASSERT: file %s, %s line %d\n", __FILE__, \
        __FUNCTION__, __LINE__); \
      OS_PRINTF(dbg_msg); \
      PC_ThrowException(dbg_msg); \
      PC_CloseSchHandle(); \
      PC_MessageBox(dbg_msg); \
      PC_DebugBreak();\
    } \
  } \
  while(0)
#endif
#else

#ifdef ASSERT_ON
#ifdef MAGIC_2M
#define MT_ASSERT(condition)                                                \
  do                                                                        \
  {                                                                         \
    if ((condition) != TRUE)                                                \
    {                                                                       \
        __asm__ volatile ("break 0");                                       \
    }                                                                       \
  }                                                                             \
  while (0)
#elif defined CKCORE
#define MT_ASSERT(condition) \
  do \
  {  \
      if(!(condition)) \
      { \
        mtos_printk("\nASSERT:%s line %d\n", __FILE__, __LINE__);\
        __asm__ volatile ("bkpt");                               \
        {   \
        }  \
      }\
  }\
  while (0)
#elif defined __KERNEL__
#define MT_ASSERT(condition) \
  do \
  {  \
      if(!(condition)) \
      { \
        mtos_printk("\nASSERT:%s line %d\n", __FILE__, __LINE__);\
        __asm__ volatile ("break 0");                               \
        {   \
        }  \
      }\
  }\
  while (0)
#else
void mt_dbg_fill_log_buff_func_line(char *func,unsigned int line);
#if defined(CHIP_CONCERTO) && defined(__LINUX__)
#define MT_ASSERT(condition) do {} while(0)
#else
#define MT_ASSERT(condition) \
  do \
  {  \
      if(!(condition)) \
      { \
        extern int mt_dbg_printf(unsigned int level, const char *p_fmt, ...);\
        mt_dbg_printf(1, "\nASSERT:%s line %d\n", __FILE__, __LINE__);\
        extern void mt_dbg_fill_log_buff_func_line(char *func,unsigned int line);\
        mt_dbg_fill_log_buff_func_line(__FILE__, __LINE__);\
        __asm__ volatile ("break 0");                                       \
      }\
  }\
  while (0)
#endif

#define MT_ASSERT_C(condition) \
  do \
  {  \
      if((condition) != TRUE) \
      { \
        mtos_printk("\nASSERT:%s line %d\n", __FILE__, __LINE__);\
        __asm__ volatile ("break 0");                                       \
      }\
  }\
  while (0)

#define MT_ASSERT1(condition) \
  do \
  {  \
      if((condition) != TRUE) \
      { \
        mtos_printk_1("\nASSERT:%s line %d\n", __FILE__, __LINE__);\
        __asm__ volatile ("break 0");                                       \
      }\
  }\
  while (0)

#endif
#else
/*!
   Dummy assert macro
  */
#define MT_ASSERT_C(x) do {} while(0)
#define MT_ASSERT1(x) do {} while(0)
#define MT_ASSERT(x) do {} while(0)
#endif
#endif

#ifdef WIN32
#include "stdio.h"
#ifdef DISABLE_FUNCTION
#define DUMMY_PRINTF(...)
#define OS_PRINTF DUMMY_PRINTF
#else
extern int log_printf(const char *fmt, ...);

#define OS_PRINTF log_printf

/*!
   Dummy printf
  */
#define DUMMY_PRINTF
#endif
#else //other archs
/*!
   Dummy printf
  */
#define DUMMY_PRINTF(...)

#define OS_PRINT_WAVE mtos_printk

#ifdef PRINT_ON
/*!
   OS printf macro in MIPS core
  */
 #ifdef __LINUX__
 /*!
   printf from uclibc
  */
#define OS_PRINTF printf
 #else
  /*!
   printf implemented by ourself
  */
#define OS_PRINTF mtos_printk
#endif
#else
/*!
   Dummy OS printf macro
  */
#define OS_PRINTF(...)     do{}while(0)
#endif
#endif

/*!
   Set bit value to 1
  */
#define SYS_SET_BIT(x, bit)    ((x) |= (1 << (bit)))
/*!
   Clear a bit
  */
#define SYS_CLR_BIT(x, bit)    ((x) &= (~(1 << (bit))))
/*!
   Put bit value
  */
#define SYS_PUT_BIT(x, bit, val) \
  ((x) = ((x) & (~(1 << (bit)))) | (((val) != 0) << (bit)))
/*!
   Get bit value
  */
#define SYS_GET_BIT(x, bit)    (((x) >> bit) & 1)
/*!
   Get the 4th byte value in 4 bytes
  */
#define SYS_GET_BYTE3(x)       (((x) >> 24) & 0xFF)
/*!
   Get the 3rd byte value in 4 bytes
  */
#define SYS_GET_BYTE2(x)       (((x) >> 16) & 0xFF)
/*!
   Get the 2nd byte value in 4 bytes
  */
#define SYS_GET_BYTE1(x)       (((x) >> 8) & 0xFF)
/*!
   Get the 1st byte value in 4 bytes
  */
#define SYS_GET_BYTE0(x)       ((x) & 0xFF)
/*!
   1024 bytes macro
  */
#define KBYTES    1024
/*!
   1M(1048576) bytes macro
  */
#define MBYTES    (KBYTES * KBYTES)

/*!
  KHZ
  */
#define KHZ      (1000)
/*!
  MHZ
  */
#define MHZ       (KHZ * KHZ)
/*!
   Combination of two byte(8bit) as a word(16bit)
  */
#define MAKE_WORD(low, high) \
  ((u16)(((u8)(low)) | \
         (((u16)((u8)(high))) << 8)))
/*!
  Combination of two byte(8bit) as a word(16bit)
  */
#define MAKE_WORD2(high, low) ((u16)(((u8)(low)) | \
                                       ((( u16)((u8)(high))) << 8)))

/*!
   Combination of two word(16bit) as a dword(32bit)
  */
#define MT_MAKE_DWORD(low, high) \
  ((u32)(((u16)(low)) | \
         (((u32)((u16)(high))) << 16)))
#define MT_MAKE_DWORD2(high, low) \
  ((u32)(((u16)(low)) | \
         (((u32)((u16)(high))) << 16)))

/*!
  Combination of two dword (32bit) as a lword(64bit)
  */
#define MT_MAKE_LWORD(low, high) \
  ((u64)(((u32)(low)) |           \
        (((u64)((u32)(high))) << 32)))

#define MT_MAKE_LWORD2(high, low) \
  ((u64)(((u32)(low)) |           \
        (((u64)((u32)(high))) << 32)))


/*!
   Todo: fix me
  */
#define SYS_GET_LOW_HALF_BYTE(X)          ((X) & (0x0f))

/*!
   Todo: fix me
  */
#define SYS_GET_HIGH_HALF_BYTE(X)         ((X) & (0xf0))

/*!
   Todo: fix me
  */
#define SYS_GET_HIGH_FIVE_BITS(X)         ((X) & (0xf8))


/*!
   Todo: fix me
  */
#define SYS_GET_LOW_FIVE_BITS(X)          ((X) & (0x1f))


/*!
   Todo: fix me
  */
#define SYS_GET_LOW_THREE_BITS(X)         ((X) & (0x03))


/*!
   Todo: fix me
  */
#define MASK_FIRST_BIT_OF_BYTE(X)         ((X) & (0x01))

/*!
   Todo: fix me
  */
#define MASK_SECOND_BIT_OF_BYTE(X)        ((X) & (0x02))

/*!
   Todo: fix me
  */
#define MASK_THIRD_BIT_OF_BYTE(X)         ((X) & (0x04))

/*!
   Todo: fix me
  */
#define MASK_FOURTH_BIT_OF_BYTE(X)        ((X) & (0x08))


/*!
   Todo: fix me
  */
#define MASK_FIFTH_BIT_OF_BYTE(X)         ((X) & (0x10))

/*!
   Todo: fix me
  */
#define MASK_SIXTH_BIT_OF_BYTE(X)         ((X) & (0x20))

/*!
   Todo: fix me
  */
#define MASK_SEVENTH_BIT_OF_BYTE(X)       ((X) & (0x40))


#ifndef ABS
/*!
   a macro returns the absolute value of x.
  */
#define ABS(x)                  (((x) < 0) ? -(x) : (x))
#endif

/*!
  max file path
  */
#define MAX_FILE_PATH (255)

/*!
  record info packet id
  */
#define REC_INFO_PID (0x1FFF)

/*!
  new rec and ts player
  */
#define NEW_REC_AND_TS_PLAYER (1)
/*!
   INT(x/y+ 1) * y
  */
#define ROUNDUP(x, y)           (((x) + (y) - 1) & ~((y) - 1))
/*!
   INT(x/y) * y
  */
#define ROUNDDOWN(x, y)         ((x) & ~((y) - 1))
/*!
   a macro returns the maximum of x and y.
  */
#define MAX(x, y)               (((x) > (y)) ? (x) : (y))
/*!
   a macro returns the minimum of x and y.
  */
#define MIN(x, y)               (((x) < (y)) ? (x) : (y))


/*!
   Todo: fix me
  */
#define TOOL_BOOTLOADER_ID          0x01
/*!
   Todo: fix me
  */
#define TOOL_BL_CONFIG_ID           0x02
/*!
   Todo: fix me
  */
#define TOOL_UPGCLIENT_ID           0x03

////////////////////////////////////////////////////////////////
//  Block ID definition, high 6bit:type, low 2bit:index
////////////////////////////////////////////////////////////////
/*!
  BOOTLOADER block
  */
#define BOOTLOADER_BLOCK_ID          0x80
/*!
  Data manager header ID
  */
#define DMH_BLOCK_ID                 0x84
/*!
  Maincode block
  */
#define MAINCODE_BLOCK_ID            0x88
/*!
  OTA_0 block
  */
#define OTA_BLOCK_ID                 0x87
/*!
  Fast logo block
  */
#define FAST_LOGO_BLOCK_ID            0x89
/*!
  USB tool block
  */
#define USB_TOOL_BLOCK_ID            0x8A
/*!
  Bootloader resource block ID
  */
#define BL_RS_BLOCK_ID               0x8B
/*!
  Resource block
  */
#define RS_BLOCK_ID                  0x8C
/*!
  OTA_1 block
  */
#define OTA1_BLOCK_ID            0x8D
/*!
  backup ota loader
  */
#define BACKUPLOADER_BLOCK_ID            0x8E
/*!
  identity block
  */
#define IDENTITY_BLOCK_ID                  0x8F

/*!
  Calendar block
  */
#define CALENDAR_BLOCK_ID           0x90
/*!
  Start up LOGO block
  */
#define START_LOGO_BLOCK_ID          0x93
/*!
  LOGO 1 block
  */
#define LOGO_BLOCK_ID_M0             0x94
/*!
  LOGO 2 block
  */
#define LOGO_BLOCK_ID_M1             0x95
/*!
  LOGO 3 block
  */
#define LOGO_BLOCK_ID_M2             0x96
/*!
  LOGO 4 block
  */
#define LOGO_BLOCK_ID_M3             0x97
/*!
  System status data block
  */
#define SS_DATA_BLOCK_ID             0x98
/*!
  board config block
  */
#define BOARD_CONFIG_BLOCK_ID             0x99
/*!
  Preset block block
  */
#define PRESET_BLOCK_ID              0x9C
/*!
  RSS block block
  */
#define RSS_BLOCK_ID              0x9D
/*!
  Upgrade info block
  */
#define UPGRADE_BLOCK_ID             0xA0
/*!
  IW table block
  */
#define IW_TABLE_BLOCK_ID            0xA4
/*!
  IW view block
  */
#define IW_VIEW_BLOCK_ID             0xA8
/*!
  CA Data block
  */
#define CADATA_BLOCK_ID              0xAD
/*!
  IR key block
  */
#define IRKEY_BLOCK_ID               0xAE
/*!
  IR key 1 block
  */
#define IRKEY1_BLOCK_ID              0xAF
/*!
  IR key 2 block
  */
#define IRKEY2_BLOCK_ID              0xB1
/*!
  FP key block
  */
#define FPKEY_BLOCK_ID               0xB0
/*!
  FP config block
  */
#define FPCFG_BLOCK_ID               0xB2
/*!
  Misc option in bootloader block
  */
#define MISC_OPTION_BLOCK_ID         0xB4
/*!
  UPG tool block
  */
#define UPG_TOOL_BLOCK_ID            0xB8
/*!
  ABS tdi block
  */
#define ABS_TDI_BLOCK_ID             0xB9
/*!
  ABS flash header block
  */
#define ABS_FLASH_HEADER_BLOCK_ID    0xBA
/*!
  SDRAM config block
  */
#define SDRAM_CFG_BLOCK_ID           0xBB
/*!
  Hardware config block id
  */
#define HW_CFG_BLOCK_ID             0xBC
/*!
  SN client UPG
  */
#define OTA_BLOCK_SNUPG              0xBF
/*!
  flash code block
  */
#define FLASH_CODE_BLOCK_ID           0xCC

/*!
  system software and hardware info block
  */
#define SYS_INFO_BLOCK_ID          0xC0

/*!
  direct read and write ads block id
  */
#define DEC_RW_ADS_BLOCK_ID         0xC4

/*!
  wstfont2 bits small vsb block id
  */
#define SMALL_VSB_BLOCK_ID          0xC7

/*!
  wstfont2 bits ntsc vsb block id
  */
#define NTSC_VSB_BLOCK_ID           0xC8

/*!
  wstfont2 bits pal vsb block id
  */
#define PAL_VSB_BLOCK_ID            0xC9

/*!
  RSC color_0 block id
  */
#define RS_COLOR0_BLOCK_ID            0xD0

/*!
  RSC color_1 block id
  */
#define RS_COLOR1_BLOCK_ID            0xD1

#define LOGO_MUSIC_ID                0x97

#ifndef WIN32
/*!
  compile align
  */
#define ATTRIB_ALIGN(x) __attribute__ ((aligned (x)))
/*!
  cache align
  */
#define CACHE_ALIGN __attribute__ ((aligned (32)))
#else
/*!
  dummy
  */
#define ATTRIB_ALIGN(x)
/*!
  dummy
  */
#define CACHE_ALIGN
#endif
#ifdef __cplusplus
}
#endif
#endif //__SYS_DEFINE_H__
