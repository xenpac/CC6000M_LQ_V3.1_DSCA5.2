/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

/*!
  comments
  */
typedef enum
{
  /*!
    comments
    */
  ERROR_PLATFORM_COMMON_BASE = 0,
  /*!
    这里是面向平台的驱动规范接口的错误代码定义
    */
  ERROR_PLATFORM_DRV_BASE = 0x10000000,
  /*!
    这里是UART的错误定义
    */
  ERROR_PLATFORM_DRV_UART_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00010000),
  /*!
    这里是GPIO的错误定义
    */
  ERROR_PLATFORM_DRV_GPIO_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00020000),

  /*!
    这里是WATCHDOG的错误定义
    */
  ERROR_PLATFORM_DRV_WATCHDOG_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00030000),

  /*!
    这里是IRDA的错误定义
    */
  ERROR_PLATFORM_DRV_IRDA_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00040000),

  /*!
    这里是I2C的错误定义
    */
  ERROR_PLATFORM_DRV_I2C_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00050000),

  /*!
    这里是DMA的错误定义
    */
  ERROR_PLATFORM_DRV_DMA_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00060000),

  /*!
    这里是LAYER的错误定义
    */
  ERROR_PLATFORM_DRV_LAYER_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00070000),

  /*!
    这里是AUDIO的错误定义
    */
  ERROR_PLATFORM_DRV_AUDIO_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00080000),

  /*!
    这里是VIDEO的错误定义
    */
  ERROR_PLATFORM_DRV_VIDEO_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x00090000),

  /*!
    这里是PTI的错误定义
    */
  ERROR_PLATFORM_DRV_PTI_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x000A0000),

  /*!
    这里是SMC的错误定义 
    */
  ERROR_PLATFORM_DRV_SMC_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x000B0000),

  /*!
    这里是TIMER的错误定义
    */
  ERROR_PLATFORM_DRV_TIMER_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x000D0000),    
  /*!
    这里是EEPROM的错误定义
    */
  ERROR_PLATFORM_DRV_EEPROM_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x000E0000), 

  /*!
    这里是LEDKB的错误定义
    */
  ERROR_PLATFORM_DRV_LEDKB_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x000F0000),        

  /*!
    这里是DESCARMBLER的错误定义
    */
  ERROR_PLATFORM_DRV_DESCARMBLER_BASE = ((ERROR_PLATFORM_DRV_BASE) | 0x000C0000),
  /*!
    comments
    */
  ERROR_PLATFORM_DRV_DESCARMBLER_BUSY = 0x0000C000,
  /*!
    这里是针对与统一驱动的错误定义 见相应文档规范
    */
  ERROR_UNITED_DRV_BASE = 0x10000000,

  /*!
    这里是针对中间件层的错误定义 见相应文档规范
    */
  ERROR_MIDDLEWARE_BASE = 0x20000000,

  /*!
    这里是针对于应用层的错误定义 见相应文档规范
    */
  ERROR_APPLICATION_BASE = 0x30000000,

  /*!
    comments
    */
  ERROR_CODE_UNKNOWN_BASE
}errorcode_modulebase_t;

/*!
  comments
  */
typedef enum
{
  /*!
    comments
    */
  ERROR_CODE_NO_ERROR = ERROR_PLATFORM_COMMON_BASE,
  /*!
    运行结果错
    */
  ERROR_CODE_ERROR_RESULT,
  /*!
    超时
    */
  ERROR_CODE_ERROR_TIMEOUT,
  /*!
    参数错误
    */
  ERROR_CODE_ERROR_PARM,
  /*!
    设备不存在
    */
  ERROR_CODE_ERROR_NOCONNECT,
  /*!
    设备没打开
    */
  ERROR_CODE_ERROR_NOOPEN,
  /*!
    读出错
    */
  ERROR_CODE_ERROR_READ,
  /*!
    写出错
    */
  ERROR_CODE_ERROR_WRITE,
  /*!
    缓冲区上溢
    */
  ERROR_CODE_ERROR_UNDOWN,
  /*!
    缓冲区下溢
    */
  ERROR_CODE_ERROR_OVER,
  /*!
    超出设备的最大数量
    */
  ERROR_CODE_ERROR_MAXDEV,
  /*!
    设备不存在
    */
  ERROR_CODE_ERROR_NODEV,
  /*!
    没准备好
    */
  ERROR_CODE_ERROR_NOREADY,
  /*!
    内存不够
    */
  ERROR_CODE_ERROR_NOBUFF,
  /*!
    内存不够
    */
  ERROR_CODE_ERROR_NOTENOUGHMEM,
  /*!
    特性不支持
    */
  ERROR_CODE_ERROR_NOT_SUPPORT,
  /*!
    已经初始化
    */
  ERROR_CODE_ERROR_ALREADY_INITIALIZED,
  /*!
    设备被占用
    */
  ERROR_CODE_ERROR_BE_USED
}errorcode_subtype_t;


/*!
  Irda Error Code
  */
#define  ERR_IRDA_NOERR             ((ERROR_PLATFORM_COMMON_BASE) | (ERROR_CODE_NO_ERROR))
/*!
  comments
  */
#define  ERR_IRDA_ERROR_PARAM       ((ERROR_PLATFORM_DRV_IRDA_BASE) | (ERROR_CODE_ERROR_PARM))
/*!
  comments
  */
#define  ERR_IRDA_ERROR_RESULT      ((ERROR_PLATFORM_DRV_IRDA_BASE) | (ERROR_CODE_ERROR_RESULT))


/*!
  I2c Error Code
  */
#define  ERR_I2C_NOERR              ((ERROR_PLATFORM_COMMON_BASE) | (ERROR_CODE_NO_ERROR))
/*!
  comments
  */
#define  ERR_I2C_ERROR_PARAM        ((ERROR_PLATFORM_DRV_I2C_BASE) | (ERROR_CODE_ERROR_PARM))
/*!
  comments
  */
#define  ERR_I2C_ERROR_RESULT       ((ERROR_PLATFORM_DRV_I2C_BASE) | (ERROR_CODE_ERROR_RESULT))

/*!
  SMC Error Code
  */
#define ERR_SMC_NO_ERR    0x00000000

/*!
  Uart Error Code
  */
#define  ERR_UART_NOERR             (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define  ERR_UART_ERROR_PARAM       (ERROR_PLATFORM_DRV_UART_BASE | ERROR_CODE_ERROR_PARM)
/*!
  comments
  */
#define  ERR_UART_ERROR_RESULT      (ERROR_PLATFORM_DRV_UART_BASE | ERROR_CODE_ERROR_RESULT)
/*!
  comments
  */
#define  ERR_UART_ERROR_TIMEOUT      (ERROR_PLATFORM_DRV_UART_BASE | ERROR_CODE_ERROR_TIMEOUT)

/*!
  Watchdog Error Code
  */
#define  ERR_WATCHDOG_NOERR         (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define  ERR_WATCHDOG_ERROR_PARAM   (ERROR_PLATFORM_DRV_WATCHDOG_BASE | ERROR_CODE_ERROR_PARM)
/*!
  comments
  */
#define  ERR_WATCHDOG_ERROR_RESULT  (ERROR_PLATFORM_DRV_WATCHDOG_BASE | ERROR_CODE_ERROR_RESULT)

/*!
  Timer Error Code
  */
#define  ERR_TIMER_NOERR            (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define  ERR_TIMER_ERROR_PARAM      (ERROR_PLATFORM_DRV_TIMER_BASE | ERROR_CODE_ERROR_PARM)
/*!
  comments
  */
#define  ERR_TIMER_ERROR_RESULT     (ERROR_PLATFORM_DRV_TIMER_BASE | ERROR_CODE_ERROR_RESULT) 
/*!
  comments
  */
#define  ERR_TIMER_ERROR_NODEV      (ERROR_PLATFORM_DRV_TIMER_BASE | ERROR_CODE_ERROR_NODEV)
/*!
  comments
  */
#define  ERR_TIMER_ERROR_BE_USED    (ERROR_PLATFORM_DRV_TIMER_BASE | ERROR_CODE_ERROR_BE_USED)  
/*!
  comments
  */
#define  ERR_TIMER_ERROR_NOOPEN   (ERROR_PLATFORM_DRV_TIMER_BASE | ERROR_CODE_ERROR_NOOPEN)


/*!
  Gpio Error Code
  */
#define  ERR_GPIO_NOERR             (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define  ERR_GPIO_ERROR_PARAM       (ERROR_PLATFORM_DRV_GPIO_BASE | ERROR_CODE_ERROR_PARM)
/*!
  comments
  */
#define  ERR_GPIO_ERROR_RESULT      (ERROR_PLATFORM_DRV_GPIO_BASE | ERROR_CODE_ERROR_RESULT)
/*!
  comments
  */
#define  ERR_GPIO_ERROR_BE_USED     (ERROR_PLATFORM_DRV_GPIO_BASE | ERROR_CODE_ERROR_BE_USED)

/*!
  Audio Error Code
  */
#define  ERR_AUDIO_NOERR             (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define  ERR_AUDIO_ERROR_PARAM       (ERROR_PLATFORM_DRV_AUDIO_BASE | ERROR_CODE_ERROR_PARM)
/*!
  comments
  */
#define  ERR_AUDIO_ERROR_RESULT      (ERROR_PLATFORM_DRV_AUDIO_BASE | ERROR_CODE_ERROR_RESULT)
/*!
  comments
  */
#define  ERR_AUDIO_ERROR_NOT_SUPPORT (ERROR_PLATFORM_DRV_AUDIO_BASE | ERROR_CODE_ERROR_NOT_SUPPORT)

/*!
  Ledkb Error Code
  */
#define  ERR_LEDKB_NOERR             (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define  ERR_LEDKB_ERROR_PARAM       (ERROR_PLATFORM_DRV_LEDKB_BASE | ERROR_CODE_ERROR_PARM)
/*!
  comments
  */
#define  ERR_LEDKB_ERROR_RESULT      (ERROR_PLATFORM_DRV_LEDKB_BASE | ERROR_CODE_ERROR_RESULT)

/*!
  DMA Moduel Error Code
  */
#define ERR_DMA_NO_ERR        (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define ERR_DMA_ERROR_PARAM      (ERROR_PLATFORM_DRV_DMA_BASE | ERROR_CODE_ERROR_PARM)    
/*!
  comments
  */
#define ERR_DMA_ERROR_RESULT    (ERROR_PLATFORM_DRV_DMA_BASE | ERROR_CODE_ERROR_RESULT)

/*!
  PTI
  */
#define  ERR_PTI_NOERR                  (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define  ERR_PTI_ERROR_RESULT           (ERROR_PLATFORM_DRV_PTI_BASE | ERROR_CODE_ERROR_RESULT)
/*!
  comments
  */
#define  ERR_PTI_ERROR_PARM             (ERROR_PLATFORM_DRV_PTI_BASE | ERROR_CODE_ERROR_PARM)

/*!
  DESCRAMBLER
  */
#define  ERR_DESCRAMBLER_NOERR          (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define ERR_DESCRAMBLER_ERROR_RESULT (ERROR_PLATFORM_DRV_DESCARMBLER_BASE | ERROR_CODE_ERROR_RESULT)
/*!
  comments
  */
#define  ERR_DESCRAMBLER_ERROR_PARM   (ERROR_PLATFORM_DRV_DESCARMBLER_BASE | ERROR_CODE_ERROR_PARM)

/*!
  VIDEO
  */
#define ERR_VIDEO_NOERR      (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define ERR_VIDEO_RESULT    (ERROR_PLATFORM_DRV_VIDEO_BASE | ERROR_CODE_ERROR_RESULT)
/*!
  comments
  */
#define ERR_VIDEO_INITIALIZED   (ERROR_PLATFORM_DRV_VIDEO_BASE | ERROR_CODE_ERROR_NOREADY)
/*!
  comments
  */
#define ERR_VIDEO_PARAM      (ERROR_PLATFORM_DRV_VIDEO_BASE | ERROR_CODE_ERROR_PARM)


/*!
  OSD Moduel Error Code
  */
#define ERR_LAYER_NO_ERR        (ERROR_PLATFORM_COMMON_BASE | ERROR_CODE_NO_ERROR)
/*!
  comments
  */
#define ERR_LAYER_ERROR_RESULT      (ERROR_PLATFORM_DRV_LAYER_BASE | ERROR_CODE_ERROR_RESULT)
/*!
  comments
  */
#define ERR_LAYER_ERROR_PARAM      (ERROR_PLATFORM_DRV_LAYER_BASE | ERROR_CODE_ERROR_PARM)


#endif //__ERROR_CODE_H__


