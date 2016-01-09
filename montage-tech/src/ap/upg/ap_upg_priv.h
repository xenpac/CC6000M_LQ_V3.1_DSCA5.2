/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_UPG_PRIV_H_
#define __AP_UPG_PRIV_H_

/*!
  \file ap_upg_priv.h

  Declare upg common private interfaces for UPG usage
  */

/*!
  upg get key input
  */
#define UPG_GET_KEY(upg_priv)      ((s8)(upg_priv->upg_data.key))
/*!
  upg get current status
  */
#define UPG_GET_STS(upg_priv)      (upg_priv->upg_data.sm)
/*!
  default uart baudrate
  */
#define UART_DEFAULT_BP             115200
/*!
  default UART ID to upgrade
  */
#define UART_UPG_ID                 UART_ID_0
/*!
  UART baudrate register value, default the highest baudrate
  for 123M CPU, 0x1: 2M bp, 0x2: 1.5M bp, 0x3: 1M bp, 0x7: 512K bp
  */
#define UART_UPG_BP                 0x2
/*!
  the max block number supported in UPG each time
  */
#define UPG_MAX_BLOCK_NUM           (64)

/*!
  upg private data
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
    flag of UPG all or UPG blocks: TRUE: upg all, FALSE:upg blocks
    */
  u8 upg_all_flag;
  /*!
    the message number to support
    */
  u8 msg_num;
  /*!
    message list to support
    */
  u32 msg_list[UPG_CMD_CNT];
  /*!
    the total data size to UPG
    */
  u32 total_size;
  /*!
    current transport size
    */
  u32 out_size;
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
  upg_block_t upg_block[UPG_MAX_BLOCK_NUM];
  /*!
    tmp use to store the data manager information from slave
    */
  //slave_info_t *p_slave_info;
  /*!
    the status updated to UI
    */
  upg_status_t status;
  /*!
    the UPG configuration
    */
  upg_config_t cfg;
  /*!
    divides mode
    */
  BOOL divides_mode;    
  /*!
    divides
    */
  u8 divides;  

  /*!
    divides ready
    */
  u8 divides_ready;    
}upg_data_t;


/*!
  upg private info
  */
typedef struct 
{
  /*!
    upg private data
    */
  upg_data_t upg_data;

  /*!
    UPG implement
    */
  upg_policy_t *p_upg_impl;

  /*!
    Application UPG information
    */
  app_t upg_app;
}upg_priv_t;

/*!
  update UPG current status to UI

  \param[in] p_priv UPG private data
  \param[in] status current status
  \param[in] progress the progress of current status
  \param[in] p_description the description of current status
  */
void upg_update_status(upg_priv_t *p_priv, u8 status, int progress, 
                        char *p_description);

/*!
  sync with upgclient
  
  \param[in] p_upg_priv the private info
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t p2p_cmd_sync(upg_priv_t *p_upg_priv);

/*!
  send cmd to test if the upgclient is available
  
  \param[in] p_upg_priv the private info
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t p2p_cmd_test(upg_priv_t *p_upg_priv);

/*!
  collect the slave data manager info
  
  \param[in] p_upg_priv the private info
  \param[in] check_slave check the crc in slave or not, if crc error, block 
             upgrade is not available
  \param[out] the slave information
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t p2p_cmd_collect(upg_priv_t *p_upg_priv,u8 check_slave,u8 *p_info);

/*!
  sync with upgclient
  
  \param[in] p_upg_priv the private info
  \param[in] block_number the block number to upgrade
  \param[in] p_blocks the blocks info
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t p2p_cmd_inform(upg_priv_t *p_upg_priv, u8 block_number, 
                          upg_block_t *p_blocks);
/*!
  transfer the data to upgclient
  
  \param[in] p_upg_priv the private info
  \param[in] zipped if the data is compressed or not, only available for UPG all
  \param[in] block_id the block id to upgrade
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t p2p_cmd_transfer(upg_priv_t *p_upg_priv,u8 zipped, u8 block_id);

/*!
  send command to make upgclient burn the flash, and send back the progress
  
  \param[in] p_upg_priv the private info
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t p2p_cmd_burn(upg_priv_t *p_upg_priv);

/*!
  send command to make upgclient reboot
  
  \param[in] p_upg_priv the private info
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t p2p_cmd_reboot(upg_priv_t *p_upg_priv);

/*!
  send command to init ddr chip
  
  \param[in] void
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
upg_ret_t  p2p_ddr_init_chip(void);
#endif // End for __AP_UPG_PRIV_H_


