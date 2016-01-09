/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HOTPLUG_H__
#define __HOTPLUG_H__

/*!
  hot_plug_event_type_t
  */
typedef enum {
/*!
  plug in
  */
  HP_BUS_PLUG_IN = 1,
/*!
  plug out
  */
  HP_BUS_PLUG_OUT,
}hot_plug_event_type_t;

/*!
  hot_plug_bus_type_t
  */
typedef enum {
/*!
  usb bus
  */
  HP_BUS_USB = 1,
}hot_plug_bus_type_t;

/*!
  hot_plug_bus_mode_t
  */
typedef enum {
/*!
  high speed mode
  */
  HP_BUS_HIGH_SPEED = 1,
/*!
  full speed mode
  */
  HP_BUS_FULL_SPEED,
/*!
  low speed mode
  */
  HP_BUS_LOW_SPEED,
/*!
  unknown speed mode
  */
  HP_BUS_UNKNOWN_SPEED,
}hot_plug_bus_mode_t;

/*!
  hot_plug_bus_dev_t
  */
typedef enum {
/*!
  storage device
  */
  HP_STORAGE = 1,
/*!
  hub device
  */
  HP_HUB,
/*!
  wifi device
  */
  HP_WIFI,
  /*!
  UVC device
  */
  HP_UVC,
/*!
  usb ethernet device
  */
  HP_ETHERNET,
/*!
  3g modem
  */
  HP_MODEM,
/*!
  3g modem ethernet
  */
  HP_MODEM_ETHERNET,
  /*!
  HP_JOYSTICK
  */
  HP_JOYSTICK,
  /*!
  HP_SCANNER
  */
  HP_SCANNER,
  /*!
  modem net
  */
  HP_MODEM_NET,
  /*!
  CCID
  */
  HP_CCID,  
  /*!
  unknown device
  */
  HP_UNKNOWN,
}hot_plug_dev_t;

/*!
  hotplug_msg_t
  */
typedef struct
{
/*!
  event
  */
  hot_plug_event_type_t event;
/*!
  bus_type
  */
  hot_plug_bus_type_t bus_type;
/*!
  bus_mode
  */
  hot_plug_bus_mode_t bus_mode;
/*!
  dev_type
  */
  hot_plug_dev_t dev_type;
/*!
  dev_handle
  */
  u32 dev_hld_handle;
}hotplug_msg_t;

/*!
  hotplug_notify
  */
RET_CODE hotplug_notify(hotplug_msg_t *p_msg);

/*!
  hotplug_get_msg
  */
RET_CODE hotplug_get_msg(hotplug_msg_t *p_msg);

#endif  // end of hot_plug
