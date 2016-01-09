/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PNP_SERV_H__
#define __PNP_SERV_H__

/*!
  essid length.
  */
#define WIFI_ESSID_LEN  32

/*!
  pnp monitor process
  */
typedef void(*pnp_process)(handle_t handle);

/*!
  pnp usb plug in callback
  */
typedef void(*pnp_svc_usb_plug_in_callback)(void);

/*!
  DVB service commands
  */
typedef enum
{
  /*!
    mount device.
    */
  PNP_SVC_MOUNT,
  /*!
    unmount device.
    */
  PNP_SVC_UNMOUNT,
} pnp_svc_cmd_t;

/*!
  pnp service events
  */
typedef enum tag_pnp_svc_evt
{
  /*!
    Flag of evt start
    */
  PNP_SVC_EVT_START = (MDL_PNP_SVC << 16),
  /*!
    Invoke this event when found usb/sd card plug in 0x01
    */
  PNP_SVC_USB_DEV_PLUG_IN,
  /*!
    Invoke this event when found usb/sd card plug out
    */
  PNP_SVC_USB_DEV_PLUG_OUT,
  /*!
    Invoke this event when found usb/sd card plug out
    */
  PNP_SVC_USB_MASS_PLUG_IN,
  /*!
    Invoke this event when found internet plug in   0x03
    */
  PNP_SVC_INTERNET_PLUG_IN,
  /*!
    Invoke this event when found internet plug out
    */
  PNP_SVC_INTERNET_PLUG_OUT,
  /*!
    Invoke this event when found hdmi plug in     0x05
    */
  PNP_SVC_HDMI_PLUG_IN,
  /*!
    Invoke this event when found hdmi plug out
    */
  PNP_SVC_HDMI_PLUG_OUT,
  /*!
    Invoke this event when vfs mount success
    */
  PNP_SVC_VFS_MOUNT_SUCCESS,
  /*!
    Invoke this event when vfs mount fail
    */
  PNP_SVC_VFS_MOUNT_FAIL,
  /*!
    Invoke this event when vfs un-mount success
    */
  PNP_SVC_VFS_UNMOUNT_SUCCESS,
  /*!
    Invoke this event when vfs un-mount fail
    */
  PNP_SVC_VFS_UNMOUNT_FAIL,
  /*!
    Invoke this event when wifi dev connect
    */
  PNP_SVC_WIFI_CONNECT_AP,
  /*!
    Invoke this event when wifi dev disconnect
    */
  PNP_SVC_WIFI_DIS_AP,
  /*!
    Invoke this event when wifi dev connecting
    */
  PNP_SVC_WIFI_CONNECTING_AP,
  /*!
    Invoke this event when wifi dev connect fail    
	*/  
  PNP_SVC_WIFI_CONNECT_FAIL_AP,
  /*!
    Invoke this event when no wifi dev
    */
  PNP_SVC_WIFI_NO_DEV,
  /*!
    Invoke this event when gprs status update
    */
  PNP_SVC_GPRS_STATUS_UPDATE,  
   /*!
    Invoke this event when 3g status update
    */
  PNP_SVC_3G_STATUS_UPDATE,  
   /*!
    Invoke this event when ip camara detect
    */
  PNP_SVC_IP_CAMARA_DETECT,  
  /*!
    Invoke this event when found usb internet plug in
    */
  PNP_SVC_USB_INTERNET_PLUG_IN,
  /*!
    Invoke this event when found usb internet plug out
    */
  PNP_SVC_USB_INTERNET_PLUG_OUT,
  /*!
    Flag of evt end
    */
  PNP_SVC_EVT_END
}pnp_svc_event_t;


/*!
  init parameter pnp service.
  */
typedef struct tag_pnp_svc_init_para
{
  /*!
    service's priority.
    */
  u32 service_prio;
  /*!
    service's stack size.
    */
  u32 nstksize;
  /*!
    callback
    */
  pnp_svc_usb_plug_in_callback callback;
}pnp_svc_init_para_t;


/*!
  pnp svc class declaration
  */
typedef struct tag_pnp_svc
{
  /*!
    get a service handle from pnp svc module
    */
  service_t * (*get_svc_instance)(class_handle_t handle, u32 context);
  /*!
    pnp svc class privated data
    */
  void *p_data;
  /*!
    usb monitor.
    */
  pnp_process usb_monitor;
  /*!
    ethernet monitor.
    */
  pnp_process eth_monitor;
  /*!
    usb_ethernet monitor.
    */
  pnp_process usb_eth_monitor;
  /*!
    wifi monitor
    */
  pnp_process wifi_monitor; 
  /*!
    gprs monitor
    */
  pnp_process gprs_monitor;   
   /*!
    3g dongle monitor
    */
  pnp_process g3_dongle_monitor;  
   /*!
    ip_camara_monitor
    */
  pnp_process ip_camara_monitor;   
   /*!
    hdmi monitor
 */
  pnp_process hdmi_monitor;  
  /*!
    network service ethernet device handle
    */
  void *p_eth_dev;
  /*!
    ethernet check ticks.
    */
  u32 eth_check_ticks;
  /*!
    ethernet is plug in
    */
  BOOL is_plug_in;
  /*!
    ethernet check ticks.
    */
  u32 usb_eth_check_ticks;
  /*!
    usb ethernet is plug in
    */
  BOOL is_usb_plug_in;
  /*!
    wifi dev status
    */
  u32 wifi_state;
  /*!
    gprs dev status
    */  
  u32 gprs_state;
   /*!
    3g dongle dev status
    */  
  u32 g3_dongle_state;  
   /*!
    3g dongle dev signal strength
    */  
  u32 g3_dongle_signal;
   /*!
     ip_camara_name 
    */  
  char ip_camara_name[32];   
   /*!
    3g dongle dev status
  */  
  u32 hdmi_state;  
  /*!
    check wifi dev status ticks
    */
  u32 wifi_check_ticks;  
  /*!
    check gprs dev status ticks
    */
  u32 gprs_check_ticks;
   /*!
    check 3g dev status ticks
    */
  u32 g3_check_ticks;
   /*!
    check ip camara dev status ticks
    */
   u32 ip_camara_ticks;   
  /*!
    network service wifi device handle
    */
  void *p_wifi_dev;  
  /*!
    check wifi connected essid is changed
    */
  u8 connected_essid[WIFI_ESSID_LEN + 1];
}pnp_svc_t;

/*!
  init pnp service.

  \param[in] service_prio   service task priority.

  \return TRUE or FALSE.
  */
handle_t pnp_service_init(pnp_svc_init_para_t *p_para);

/*!
  pnp service mount vfs.

  \param[in] p_url: path.

  \return RET_CODE.
  */
void pnp_service_vfs_mount(u16 *p_url);

/*!
  pnp service un-mount vfs.

  \param[in] p_url: path.

  \return RET_CODE.
  */
void pnp_service_vfs_unmount(u16 *p_url);

/*!
  pnp service usb plug in callback

  \return void.
  */
void pnp_service_usb_plug_in_callback(void);

#endif
