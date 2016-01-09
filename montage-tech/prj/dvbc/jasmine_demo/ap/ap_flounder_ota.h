/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifndef __AP_OTA_H_
#define __AP_OTA_H_

/*!
  \file ap_dvbs_ota.h
  Delecalre dvbs ota application.
  And this is a base class for all dvbs ota applications.
 */


/*!
  OTA TDI info definition
*/
typedef struct
{
  /*!
    oui
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
    reserved
    */
  u32 reserved;
}ota_tdi_t;

#endif // End for __AP_DVBS_OTA_H_
