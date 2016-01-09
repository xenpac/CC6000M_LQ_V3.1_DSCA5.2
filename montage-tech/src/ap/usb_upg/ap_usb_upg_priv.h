/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_USB_UPG_PRIV_H_
#define __AP_USB_UPG_PRIV_H_

/*!
  \file ap_upg_priv.h

  Declare upg common private interfaces for UPG usage
  */
 
/*!
  upg get key input
  */
#define USB_UPG_GET_KEY(usb_upg_priv)      ((s8)(usb_upg_priv->usb_upg_data.key))
/*!
  upg get current status
  */
#define USB_UPG_GET_STS(usb_upg_priv)      (usb_upg_priv->usb_upg_data.sm)

/*!
 usb upg private data
  */
typedef struct
{
  /*!
    current UPG status, see upg_sm_t 
    */
  u8 sm;
  /*!
    received key, see upg_key_t
    */
  s8 key;
  /*!
    flag of UPG all or UPG blocks
    */
  u8 usb_upg_all_flag;
  /*!
    the message number to support
    */
  u8 msg_num;
  /*!
    message list to support
    */
  u32 msg_list[USB_UPG_CMD_CNT];
  /*!
    downloaded_size
    */
  u32 dnloaded_size;
  /*!
    current burnt size
    */
  u32 burnt_size;
  /*!
    user db head size
    */
  u32 head_size;
  /*!
    user db head data
    */
  u8 head_data[USB_UPG_MAX_HEAD_SIZE];
  /*!
    the block number in once upgrade
    */
  u32 blk_num;
  /*!
    the upgrade mode, see upg_mode_t
    */
  u32 upg_mode;
  /*!
    the blocks to upgrade
    */
  usb_upg_block_t usb_upg_block[USB_UPG_MAX_BLOCK];
  /*!
    the status updated to UI
    */
  usb_upg_status_t status;
  /*!
    the UPG configuration
    */
  usb_upg_config_t cfg;
  /*!
    upg file path
    */
  u16 upg_file_path[MAX_FILE_PATH];
  /*!
    upg file pointer
    */
  hfile_t p_file;
  /*!
    downloade upgfile from usb finished
  */
  u8 dnload_end;
  /*!
    burning flash's finished
    */
  u8 burn_end;
}usb_upg_data_t;


/*!
  upg private info
  */
typedef struct 
{
  /*!
    upg private data
    */
  usb_upg_data_t usb_upg_data;

  /*!
    UPG implement
    */
  usb_upg_policy_t *p_usb_upg_impl;
  /*!
    flash start addr to be operated
    */
  u32 flash_offset_addr;
  u32 burn_flash_size;
  u32 ext_buf;
  u32 ext_size;
  u32 using_size;

  /*!
    Application UPG information
    */
  app_t usb_upg_app;
}usb_upg_priv_t;

/*!
  update UPG current status to UI

  \param[in] p_priv UPG private data
  \param[in] status current status
  \param[in] progress the progress of current status
  \param[in] p_description the description of current status
  */
void usb_upg_update_status(usb_upg_priv_t *p_priv, u8 status, int progress,u8 *p_description);
/*!
  transfer the data to upgclient
  
  \param[in] p_upg_priv the private info
  \param[in] zipped if the data is compressed or not, only available for UPG all
  \param[in] block_id the block id to upgrade
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
usb_upg_ret_t usb_upg_file_dnload(usb_upg_priv_t *p_usb_upg_priv, u8 zipped);

/*!
  send command to make upgclient burn the flash, and send back the progress
  
  \param[in] p_upg_priv the private info
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
usb_upg_ret_t usb_upg_burn(usb_upg_priv_t *p_usb_upg_priv);
/*!
  send command to make upgclient reboot
  
  \param[in] p_upg_priv the private info
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
usb_upg_ret_t usb_upg_reboot(usb_upg_priv_t *p_usb_upg_priv);

#endif // End for __AP_USB_UPG_PRIV_H_


