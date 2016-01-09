/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __AVSYNC_H__
#define __AVSYNC_H__



/*!
  Parameter type definition for AVSYNC_CMD_AV_RESET.
  */
#define AVSYNC_CMD_PARAM_AV_RESET        (BOOL)


/*!
  This structure defines avsync commands by dev_io_ctrl.
  */
typedef enum
{
  /*!
    Command to set AV reset mode when error happens.
    */
  AVSYNC_CMD_AV_RESET = DEV_IOCTRL_TYPE_UNLOCK + 0,
  /*!
    Command to set audio sync mode for PCM or AC3 
    */
  AVSYNC_CMD_AUDIO_CFG,
  /*!
    Command to set AV sync mechanism: ES-buffer-control or PCR-based
    */
  AVSYNC_CMD_MODE_CFG
}avsync_cmd_t;

/*!
  This structure defines the supported AV sync modes.
  */
typedef enum
{
  /*!
    PCR mode for AV sync 
    */
  AVSYNC_MODE_PCR = 0,
  /*!
    ES-buffer-control mode for AV sync
    */
  AVSYNC_MODE_ES
} avsync_mode_t;

/*!
  This structure defines an AV sync device.
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
}avsync_device_t;

#endif //__AVSYNC_H__

