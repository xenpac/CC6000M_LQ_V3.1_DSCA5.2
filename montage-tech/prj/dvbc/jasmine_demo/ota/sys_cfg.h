/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/**********************************************************************/
/**********************************************************************/
#ifndef __SYS_CFG_H_
#define __SYS_CFG_H_

#ifdef CHIP
#define SYS_CPU_CLOCK                   (324 * 1000000)
#else
#define SYS_CPU_CLOCK                   (81 * 1000000)
#endif
#define SYS_FLASH_END      CHARSTO_SIZE


/*!
   Task priority
  */
typedef enum
{
  SYS_INIT_TASK_PRIORITY = 2,
  SYS_TIMER_TASK_PRIORITY = 3,

  OS_MUTEX_PRIORITY = 6,  

  AP_FRM_TASK_PRIORITY = 10,
  UI_FRM_TASK_PRIORITY,
  AP_DMX_TASK_PRIORITY,

  //EVA_SYS_TASK_PRIORITY,
  EVA_SYS_TASK_PRIORITY_START,
  EVA_SYS_TASK_PRIORITY_END = EVA_SYS_TASK_PRIORITY_START + 5,
  DRV_HDMI_TASK_PRIORITY,
    #if 0
  AP_NIT_TASK_PRIORITY=16,
  #endif

  AP_HIGHEST_TASK_PRIORITY = 20,
  FP_TASK_PRIORITY,
  AP_UIO_TASK_PRIORITY,
  AP_OTA_TASK_PRIORITY,

  MDL_DVB_TASK_PRIORITY = 41,
  MDL_NIM_CTRL_TASK_PRIORITY,
  
  MDL_DM_MONITOR_TASK_PRIORITY,  

  NIM_NOTIFY_TASK_PRIORITY = 50,
  DISP_HDMI_NOTIFY_TASK_PRIORITY,  
  TRANSPORT_SHELL_TASK_PRIORITY = 60,
  PRINT_TO_USB_TASK_PRIO,
  LOWEST_TASK_PRIORITY = 63,
}task_priority_t;

/*!
   Task stack size
  */
#define DEFAULT_STKSIZE              (32 * KBYTES)
#define SYS_INIT_TASK_STKSIZE        DEFAULT_STKSIZE
#define SYS_TIMER_TASK_STKSIZE       DEFAULT_STKSIZE
#define AP_FRM_TASK_STKSIZE          DEFAULT_STKSIZE
#define UI_FRM_TASK_STKSIZE          DEFAULT_STKSIZE

#define AP_UIO_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_OTA_TASK_STKSIZE          DEFAULT_STKSIZE


#define MDL_DVB_TASK_STKSIZE         DEFAULT_STKSIZE
#define MDL_NIM_CTRL_TASK_STKSIZE    DEFAULT_STKSIZE
#define MDL_DM_MONITOR_TASK_STKSIZE  DEFAULT_STKSIZE

#define DRV_HDMI_TASK_STKSIZE     DEFAULT_STKSIZE

#define NIM_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define DISP_HDMI_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define AUD_HDMI_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)

/*!
   Datamanager configuration
  */
#define DM_MAX_BLOCK_NUM             (36)

#define DM_BOOTER_START_ADDR     0x60000//0xB0000
#define DM_HDR_START_ADDR_BAK  0x150000
#define DM_HDR_START_ADDR           0x150000

/*!
  app dm block id
  */
#define DM_MAINCODE_BLOCK_ID         0x86
/*!
  logo and preset in dm block
  */
#define DM_LOGO_PRESET_BLOCK_ID    0x91
/*!
  static save date block
  */
#define STATIC_SAVE_DATA_BLOCK_ID 0xA2
/*!
  static save date backup block
  */
#define STATIC_SAVE_DATA_RESERVE_BLOCK_ID 0xA3


#define WHOLE_MEM_END           0xa3e00000
  
#ifdef FLASH_4M
#define CHARSTO_SIZE    (4 * MBYTES)
#elif defined(FLASH_8M)
#define CHARSTO_SIZE    (8 * MBYTES)
#elif defined(FLASH_16M)
#define CHARSTO_SIZE    (16 * MBYTES)
#elif defined(FLASH_32M)
#define CHARSTO_SIZE    (32 * MBYTES)
#else
#define CHARSTO_SIZE    (8 * MBYTES)
#endif


/*!
  av cpu init code size 20k
  */
#define AV_INIT_CODE_SIZE 0x0000

/*!
  av cpu stack size 1M-20k
  */
#define AV_STACK_SIZE (0x00000 - AV_INIT_CODE_SIZE)

/*!
  av cpu stack bottom addr, 63M
  */
#define AV_STACK_BOT_ADDR (WHOLE_MEM_END - AV_INIT_CODE_SIZE - AV_STACK_SIZE)

/*!
  av cpu init code addr, 63M + av cpu stack size
  */
#define AV_POWER_UP_ADDR (((AV_STACK_BOT_ADDR + AV_STACK_SIZE) & 0xFFFFFFF) | 0x80000000)


/*!
  The OSD0 size :960*900*4.
  */
#define OSD0_VSCALER_BUFFER_SIZE 0x0000 //960*900*4,¡À?D?4K????

/*!
  The OSD0 scaler buffer start. 
  */
#define OSD0_VSCALER_BUFFER_ADDR (AV_STACK_BOT_ADDR - OSD0_VSCALER_BUFFER_SIZE)

/*!
  The OSD1 size 6M, 1280*1080*4.
  */
#define OSD1_VSCALER_BUFFER_SIZE 0//0x600000

/*!
  The OSD1 scaler buffer start .
  */
#define OSD1_VSCALER_BUFFER_ADDR (OSD0_VSCALER_BUFFER_ADDR - OSD1_VSCALER_BUFFER_SIZE)

/*!
  The SUB size : 1280*36*4.  //only need four line
  */
#define SUB_VSCALER_BUFFER_SIZE 0x0000//0x600000

/*!
  The SUB scaler buffer start .
  */
#define SUB_VSCALER_BUFFER_ADDR (OSD1_VSCALER_BUFFER_ADDR - SUB_VSCALER_BUFFER_SIZE)

/*!
  The video used size 40M, take ref to warriors_vdec_get_buf_requirement.
  */
//#define VIDEO_FW_CFG_SIZE           0x2800000

/*!
  The video used size 36.1M, take ref to warriors_vdec_get_buf_requirement.
  */
#define VIDEO_FW_CFG_SIZE           0x0000

/*!
  The video used addr
  */
#define VIDEO_FW_CFG_ADDR           (SUB_VSCALER_BUFFER_ADDR - VIDEO_FW_CFG_SIZE)

/*!
  The video sd write back size, ~3.5M, 720*576*2*4. 422 frame mode
  */
#define VID_SD_WR_BACK_SIZE           0x25f800

/*!
  The video sd write back field no.
  */
#define VID_SD_WR_BACK_FIELD_NO           0x6

/*!
  The video sd write back addr. 
  */
#define VID_SD_WR_BACK_ADDR           (VIDEO_FW_CFG_ADDR - VID_SD_WR_BACK_SIZE)

/*!
  The DI used size, ~2.5M, hd require almost 2.3M
  3 luma fields:  720x288x3
  4 chroma fields(cb and cr , 4:2:0):  360x144x2x4
  2 motion fields: 360x288x2  
  */
#define VID_DI_CFG_SIZE           0x110000

/*!
  The DI used addr.
  */
#define VID_DI_CFG_ADDR           (VID_SD_WR_BACK_ADDR - VID_DI_CFG_SIZE)

/*!
  The audio used size 2M, take ref to warriors_aud_get_buf_requirement.
  */
#define AUDIO_FW_CFG_SIZE           0x000000

/*!
  The audio used addr.
  */
#define AUDIO_FW_CFG_ADDR  (((VID_DI_CFG_ADDR & 0xFFFFFFF) - AUDIO_FW_CFG_SIZE) | 0x80000000)

/*!
  malloc buffer end.
  */
#define SYSTEM_MEMORY_END                AUDIO_FW_CFG_ADDR //last buffer

/*!
   mem partition configuration
 */
extern unsigned long _end;
#define ALL_MEM_SIZE ROUNDDOWN( \
                                 (SYSTEM_MEMORY_END - ROUNDUP((u32)(&_end), 4)) \
                                 , 8)

#define ALL_MEM_HEADER (ALL_MEM_SIZE/MEM_DEFAULT_ALIGN_SIZE/8)


#endif // End for __SYS_CFG_H_

