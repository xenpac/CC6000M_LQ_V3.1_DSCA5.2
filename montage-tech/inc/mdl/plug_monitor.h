/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PLUG_MONITOR_H__
#define __PLUG_MONITOR_H__
/*!
  usb monitor attach.
  */
void usb_monitor_attach(void);
/*!
  hdmi monitor attach.
  */
void hdmi_monitor_attach(void);
/*!
  is hdmi connect and support 1080p.
  */
BOOL is_hdmi_connect_and_sup_1080p(void);

BOOL hdmi_connect_and_not_sup_1080p(void);

#endif
