/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/**********************************************************************/
/**********************************************************************/
#ifndef __CUSTOMER_CONFIG_H_
#define __CUSTOMER_CONFIG_H_

/*!
  BISS Data block_biss
  */
#define CUSTOMER_BLOCK_ID          0xAB

/*!
   [enum] for tool config the module enable or not
  */
typedef enum  _EXT_CUS_MODULE_CFG 
{
     EXTERNAL_MUSIC_PICTURE,       
     EXTERNAL_TTX_SUBTITLE,     
     EXTERNAL_SMALL_LIST_V2,          
     EXTERNAL_INFO_BAR_V2,    

     EXTERNAL_INSTALLTION_LIST_V2,   
     EXTERNAL_GLOBAL_VOLUME ,
     EXTERNAL_USB_DUMP_BREAK_LOG, 
     EXTERNAL_TRICK_PLAY ,
    
     EXTERNAL_UCASKEY_EDIT ,
     EXTERNAL_WATCH_DOG,    
     EXTERNAL_OTA,
     EXTERNAL_TKGS_UPDATE,  
    
     EXTERNAL_CUSTOMER_YINHE, 
     EXTERNAL_CUSTOMER_AISAT,
     EXTERNAL_CHINESE_OTA,
}EXT_CUS_MODULE_CFG;
/*!
   [struct]  for tool config system function ,include module
  */
typedef struct _EXT_CUS_SYS_CFG
{
   BOOL  customer_module_cfg;

   //for add sys config below.
}EXT_CUS_SYS_CFG;

/*!
    dm load the block data in mem
  */
void dm_load_customer_sys_cfg(void);

/*!
    dm get  the module enable flag  in customer config mem
  */
u8 handle_dm_get_customer_module_cfg(EXT_CUS_MODULE_CFG customer_cfg);

/*!
    dm load the hw cfg block data in mem
  */
void dm_load_customer_hw_cfg(void);

/*!
   dm get the hw config info from flash
  */
hw_cfg_t dm_get_hw_cfg_info(void);
#endif // END __CUSTOMER_CONFIG_H_
