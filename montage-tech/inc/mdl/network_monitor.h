/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NETWORK_MONITOR_H__
#define __NETWORK_MONITOR_H__

 /*!
  3g status info.
  */
typedef struct tag_g3_status_info_t
{
   /*!
    3g modem status.
    */  
   u32 status;
   /*!
    3g signal strength.
    */   
   u32 strength;
   /*!
    3g operator name.
    */      
   u8 operator[64];
}g3_status_info_t;

/*!
  ethernet monitor attach.
  */
void eth_monitor_attach(void);

/*!
  usb ethernet monitor attach.
  */
void usb_eth_monitor_attach(void);

/*!
  wifi monitor attach.
  */
void wifi_monitor_attach(void);

/*!
  gprs monitor attach.
  */
void gprs_monitor_attach(void);

/*!
  3g dongle monitor attach.
  */
void g3_monitor_attach(void);
/*!
  ip_camara_monitor_attach
  */
void ip_camara_monitor_attach();

/*!
  is wifi disconnect.
  */
RET_CODE is_wifi_disconnect(void);

#endif
