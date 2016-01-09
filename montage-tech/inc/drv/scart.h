/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SCART_H__
#define __SCART_H__

/*!
  VCR is detected
  */
#define VCR_DETECTED      0
/*!
  VCR is not detected
  */
#define VCR_NOT_DETECTED 1


/*!
  This structure defines the supported video formats for SCART.
  */
typedef enum
{
  /*!
    CVBS format for SCART video.
    */
  SCART_VID_CVBS = 0,
  /*!
    RGB format for SCART video.
    */
  SCART_VID_RGB,
  /*!
    YUV format for SCART video.
    */
  SCART_VID_YUV,
  /*!
    None
    */
  SCART_VID_NONE,
}scart_v_format_t;


/*!
  This structure defines the supported video aspect ratio of SCART
  */
typedef enum
{
  /*!
    4:3
    */
  SCART_ASPECT_4_3 = 0,
  /*!
    16:9
    */
  SCART_ASPECT_16_9,
  /*!
    OFF
    */
  SCART_ASPECT_OFF,
}scart_v_aspect_t;


/*!
  This structure defines the supported SCART ternimals.
  */
typedef enum
{
  /*!
    TV set
    */
  SCART_TERM_TV = 0,
  /*!
    STB
    */
  SCART_TERM_STB,
  /*!
    VCR
    */
  SCART_TERM_VCR
}scart_terminal_t;


/*!
  This structure defines SCART commands by dev_io_ctrl.
  */
typedef enum
{
  /*!
    Set the carried video format in a SCART cable.
    */
  SCART_CMD_FORMAT_SET = DEV_IOCTRL_TYPE_UNLOCK + 1,
  /*!
    Set the carried video aspect ratio in a SCART cable.
    */
  SCART_CMD_ASPECT_SET,
  /*!
    Select the master for TV SCART
    */
  SCART_CMD_TV_MASTER_SEL,
  /*!
    Select the record program soure of VCR SCART 
    */
  SCART_CMD_VCR_REC_SEL,
  /*!
    Check if the VCR is detected
    */
  SCART_CMD_VCR_DETECT
}scart_cmd_t;

/*!
  This structure defines a SCART device.
  */
typedef struct 
{
  /*!
    Pointer to device head
    */
  void *p_base;
  /*!
    Pointer to private data
    */
  void *p_priv;
}scart_device_t;


/*!
  GPIO pin information, 1 Byte
  */
typedef struct
{
  /*!
    Polarity of GPIO, GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH active(light)
    */
  u32 polar:1;
  /*!
    GPIO direction: GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
    */
  u32 io   :1;
  /*!
    GPIO index, upto 64 GPIO, 63 means invalid
    */
  u32 pos  :6;
  /*!
    Definition of GPIO level low 
    */
  u32 polar_0 :4;
  /*!
    Definition of GPIO level high
    */
  u32 polar_1 :4;
  /*!
    Definition of GPIO high resistant
    */
  u32 high_resistant :4;
  /*!
    is this gpio valid
    */
  u32 is_valid :1;
  /*!
    reserved
    */
  u32 reserved :12;
}scart_gpio_info_t;

/*!
  GPIO scart configuration
  */
typedef struct        
{
  /*!
    video format
    */
  scart_gpio_info_t outmode;
  /*!
    aspect ratio
    */
  scart_gpio_info_t aspect;
  /*!
    vcr input
    */
  scart_gpio_info_t vcr_input;
  /*!
    tv master
    */
  scart_gpio_info_t tv_master;
  /*!
    vcr detect
    */
  scart_gpio_info_t vcr_detect;
}scart_hw_info_t;


#endif //__SCART_H__

