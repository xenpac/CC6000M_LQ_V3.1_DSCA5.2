/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"

// system
#include "sys_types.h"
#include "sys_define.h"

#include "drv_dev.h"
#include "hotplug.h"
#include "drv_dev.h"
#include "ethernet.h"
#include "wifi.h"
#include "lib_util.h"
#include "class_factory.h"

#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_misc.h"

#include "mdl.h"

#include "vfs.h"
#include "service.h"
#include "pnp_service.h"

#include "ap_framework.h"
#include "modem.h"
#include "plug_monitor.h"

#include "common.h"
#include "lib_rect.h"
#include "drv_dev.h"
#include "vdec.h"
#include "display.h"
#include "hdmi.h"
#include "camera.h"

static void usb_evt_callback(handle_t handle, vfs_dev_event_t *p_event)
{
  os_msg_t msg = {0};
  service_t *p_svc = (service_t *)handle;

  msg.para2 = HP_STORAGE;

  switch(p_event->evt)
  {
    case VFS_PLUG_IN_EVENT:
      OS_PRINTF("\n##VFS_PLUG_IN_EVENT ready %s\n",p_event->p_name);
      msg.content = PNP_SVC_USB_MASS_PLUG_IN;
      msg.para1 = (u32)p_event->p_name;
      pnp_service_usb_plug_in_callback();
      break;
     case VFS_PLUG_OUT_EVENT:
      msg.content = PNP_SVC_USB_DEV_PLUG_OUT;
      msg.para1 = (u32)p_event->p_name;
      break;
    default:
      break;
  }
  
  p_svc->notify(p_svc, &msg);
}

static void usb_process(handle_t handle)
{
  hotplug_msg_t hp_msg = {0};
  os_msg_t msg = {0};
  s32 ret = 0;
  service_t *p_svc = (service_t *)handle;
  
  ret = hotplug_get_msg(&hp_msg); 
  if(ret != SUCCESS)
  {
    return;
  }
  
  msg.para1 = (u32)hp_msg.bus_type << 16;
  msg.para2 = hp_msg.dev_type;
  if(hp_msg.event == HP_BUS_PLUG_IN)
  {
    OS_PRINTF("HP_BUS_PLUG_IN\n");
    msg.content = PNP_SVC_USB_DEV_PLUG_IN;
    msg.para1 |= hp_msg.bus_mode;
  }
  else
  {
    OS_PRINTF("HP_BUS_PLUG_OUT\n");
    msg.content = PNP_SVC_USB_DEV_PLUG_OUT;
  }

  if(hp_msg.dev_type == HP_WIFI)                                                    //deal with wifi
  {
      p_svc->notify(p_svc, &msg);
  }
  else if(!vfs_process(handle, usb_evt_callback))
  {
    p_svc->notify(p_svc, &msg);
  }
}

static void hdmi_process(handle_t handle)
{
   pnp_svc_t *p_this = NULL;
   os_msg_t msg = {0};
   service_t *p_svc = (service_t *)handle;
   void *p_dev = NULL;
   disp_common_info_t p_info = {0};
   hdmi_edid_result_t edid = {0};
   p_info.p_hdmi_eid = (u32 *)&edid;


   p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
   MT_ASSERT(p_this != NULL);

   #ifndef WIN32   
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
   MT_ASSERT(NULL != p_dev);
   disp_get_common_info(p_dev, &p_info);
   #endif
   if(p_info.is_hdmi_connected)
   {
     OS_PRINTF("hdmi status p_info.is_hdmi_connected ==%d\n",p_info.is_hdmi_connected);
     msg.content = PNP_SVC_HDMI_PLUG_IN;
     msg.para1 = (u32) p_info.p_hdmi_eid;
   }
  else
   {
     OS_PRINTF("hdmi status p_info.is_hdmi_connected ==%d\n",p_info.is_hdmi_connected);
     msg.content = PNP_SVC_HDMI_PLUG_OUT;
   }
   if(p_this->hdmi_state != p_info.is_hdmi_connected)
   {
     p_this->hdmi_state = p_info.is_hdmi_connected;
     OS_PRINTF("hdmi get new status==%d\n",p_this->hdmi_state);
     p_svc->notify(p_svc, &msg);
   }
}

void usb_monitor_attach(void)
{
  pnp_svc_t *p_this = NULL;

  p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
  MT_ASSERT(p_this != NULL);

  p_this->usb_monitor = usb_process;
}

void hdmi_monitor_attach(void)
{
  pnp_svc_t *p_this = NULL;

  p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
  MT_ASSERT(p_this != NULL);

  p_this->hdmi_monitor = hdmi_process;
}

BOOL is_hdmi_connect_and_sup_1080p()
{
  void *p_dev = NULL;
  disp_common_info_t p_info = {0};
  hdmi_edid_result_t edid = {0};
  
  #ifndef WIN32   
  p_info.p_hdmi_eid = (u32 *)&edid;
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_dev);
  disp_get_common_info(p_dev, &p_info);
  #endif

  if((p_info.is_hdmi_connected)&&((edid.supported_1080p_60Hz)||(edid.supported_1080p_50Hz)))
  {
   return TRUE;
  }
  else
  {
   return FALSE;
  }
}

BOOL hdmi_connect_and_not_sup_1080p()
{
  void *p_dev = NULL;
  disp_common_info_t p_info = {0};
  hdmi_edid_result_t edid = {0};

  #ifndef WIN32   
  p_info.p_hdmi_eid = (u32 *)&edid;
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_dev);
  disp_get_common_info(p_dev, &p_info);
 #endif

  if((p_info.is_hdmi_connected)&&(!edid.supported_1080p_60Hz)&&(!edid.supported_1080p_50Hz))
  {
   return TRUE;
  }
  else
  {
   return FALSE;
  }
}

