/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __RF_H__
#define __RF_H__



/*!
  This structure defines the supported RF systems.
  */
typedef enum
{
  /*!
    NTSC
    */
  RF_SYS_NTSC,
  /*!
    PAL-B/G
    */
  RF_SYS_PAL_BG,
  /*!
    PAL-I
    */
  RF_SYS_PAL_I,
  /*!
    PAL-D/K
    */
  RF_SYS_PAL_DK      
}rf_sys_t;

/*!
  This structure defines RF commands by dev_io_ctrl. 
  */
typedef enum
{
  /*!
    Set the output RF channel
    */
  RF_CMD_SET_CHANNEL = DEV_IOCTRL_TYPE_UNLOCK + 1,
  /*!
    Set the output RF system
    */
  RF_CMD_SET_SYSTEM
}rf_cmd_t;

/*!
  This structure defines a RF device.
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
}rf_device_t;

#endif //__RF_H__

