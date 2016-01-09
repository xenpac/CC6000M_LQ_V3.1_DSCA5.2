/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_INFO_H_
#define __SYS_INFO_H_

/*!
  identity info
  */
typedef struct
{
/*!
  version of this struct.
  */
  u32 version;
  /*!
    oui for ota
    */
  u32 oui;
  /*!
    manufacture id
    */
  u16 manufacture_id;
  /*!
    hardware module id
    */
  u16 hw_mod_id;
  /*!
    software module id
    */
  u16 sw_mod_id;
  /*!
    hardware version
    */
  u16 hw_version;
  /*!
    serial id    
    */
  u8 stb_id[32];
  /*!
    mac addr 
    */
  u8 mac_addr[6];
  /*!
    model id of stb    
    */
  u8 model_id;
  /*!
    model id of stb    
    */
  u8 reserved;
  
  }identity_info_t;


#endif
