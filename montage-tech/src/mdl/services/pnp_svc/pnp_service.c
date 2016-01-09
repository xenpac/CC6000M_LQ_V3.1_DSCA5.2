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

#define MAX_PARTION (64)
#define PNP_SVC_MSG_DEPTH (4)

/*!
  pnp service private data define
  */
typedef struct tag_pnp_svc_private_t
{
  /*!
    pnp svc container
    */
  svc_container_t *p_container;
  /*!
    pnp svc instance.
    */
  service_t *p_pnp_svc;
  /*!
    msg_id
    */
  u32 msg_id;
  /*!
    external buffer for msgq.
    */
  void *p_msg_ext;
  /*!
    callback
    */
  pnp_svc_usb_plug_in_callback callback;
}pnp_svc_priv_t;

/*!
  pnp mount & unmount parameter data.
  */
typedef struct
{
  /*!
    url.
    */
  u16 url[MAX_FILE_PATH];
} pnp_param_t;

void pnp_service_vfs_mount(u16 *p_url)
{
  pnp_svc_t *p_this = NULL;
  pnp_svc_priv_t *p_priv = NULL;
  pnp_param_t pnp_param;

  p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
  MT_ASSERT(p_this != NULL);
  
  p_priv = (pnp_svc_priv_t *)p_this->p_data;
  MT_ASSERT(p_priv != NULL);

  memset(&pnp_param, 0, sizeof(pnp_param_t));
  memcpy(pnp_param.url, p_url, MAX_FILE_PATH);  
  mtos_messageq_clr(p_priv->msg_id);

  p_priv->p_pnp_svc->do_cmd(p_priv->p_pnp_svc, 
                            PNP_SVC_MOUNT, 
                            (u32)&pnp_param, 
                            sizeof(pnp_param_t));
  return;  
}

void pnp_service_vfs_unmount(u16 *p_url)
{
  pnp_svc_t *p_this = NULL;
  pnp_svc_priv_t *p_priv = NULL;
  pnp_param_t pnp_param;
  
  p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
  MT_ASSERT(p_this != NULL);
  
  p_priv = (pnp_svc_priv_t *)p_this->p_data;
  MT_ASSERT(p_priv != NULL);

  memset(&pnp_param, 0, sizeof(pnp_param_t));
  memcpy(pnp_param.url, p_url, MAX_FILE_PATH);  
  mtos_messageq_clr(p_priv->msg_id);
  p_priv->p_pnp_svc->do_cmd(p_priv->p_pnp_svc, 
                            PNP_SVC_UNMOUNT, 
                            (u32)&pnp_param, 
                            sizeof(pnp_param_t));
  return;
}

void pnp_service_usb_plug_in_callback(void)
{
  pnp_svc_t *p_this = NULL;
  pnp_svc_priv_t *p_priv = NULL;
  
  p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
  MT_ASSERT(p_this != NULL);
  
  p_priv = (pnp_svc_priv_t *)p_this->p_data;
  MT_ASSERT(p_priv != NULL);
  if (p_priv->callback != NULL)
  {
    p_priv->callback();
  }
}

static void pnp_msg_proc(handle_t handle, os_msg_t *p_msg)
{
  service_t *p_pnp_svc = (service_t *)handle;
  os_msg_t msg = {0};
  pnp_param_t *p_pnp_param = (pnp_param_t *)p_msg->para1;
  RET_CODE ret = SUCCESS;
  
  switch(p_msg->content)
  {
    case PNP_SVC_MOUNT:
      
      if(NULL != p_pnp_param)
      {
        ret = vfs_uri_mount(p_pnp_param->url);
        if(SUCCESS == ret)
        {
          msg.content = PNP_SVC_VFS_MOUNT_SUCCESS;
        }
        else
        {
          msg.content = PNP_SVC_VFS_MOUNT_FAIL;
        }
      }
      else
      {
        msg.content = PNP_SVC_VFS_MOUNT_FAIL;
      }
      break;
      
    case PNP_SVC_UNMOUNT:
      
      if(NULL != p_pnp_param)
      {
        ret = vfs_uri_unmount(p_pnp_param->url);
        if(SUCCESS == ret)
        {
          msg.content = PNP_SVC_VFS_UNMOUNT_SUCCESS;
        }
        else
        {
          msg.content = PNP_SVC_VFS_UNMOUNT_FAIL;
        }
      }
      else
      {
        msg.content = PNP_SVC_VFS_UNMOUNT_FAIL;
      }
      break;
      
    default:
      MT_ASSERT(0);
      break;
  }  

  p_pnp_svc->notify(p_pnp_svc, &msg);
}

static void pnp_sm_proc(handle_t handle)
{
  pnp_svc_t *p_this = NULL;
  pnp_svc_priv_t *p_priv = NULL;

  p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
  MT_ASSERT(p_this != NULL);

  p_priv = (pnp_svc_priv_t *)p_this->p_data;
  MT_ASSERT(p_priv != NULL);
  
  if(p_this->usb_monitor != NULL)
  {
    p_this->usb_monitor(handle);
  }
  
#ifndef WIN32
  if(p_this->eth_monitor != NULL)
  {
    p_this->eth_monitor(handle);
  }

  if(p_this->usb_eth_monitor != NULL)
  {
    p_this->usb_eth_monitor(handle);
  }

  if(p_this->wifi_monitor != NULL)
  {
    p_this->wifi_monitor(handle);
  }

  if(p_this->gprs_monitor != NULL)
  {
    p_this->gprs_monitor(handle);
  }  

  if(p_this->g3_dongle_monitor != NULL)
  {
     p_this->g3_dongle_monitor(handle);
  }

  if(p_this->hdmi_monitor != NULL)
  {
     p_this->hdmi_monitor(handle);
  }
  if(p_this->ip_camara_monitor != NULL)
  {
     p_this->ip_camara_monitor(handle);
  }
#endif
}

static service_t * get_pnp_svc_instance(class_handle_t handle, u32 context)
{
  pnp_svc_t *p_this = (pnp_svc_t *)handle;
  pnp_svc_priv_t *p_priv = p_this->p_data;
  service_t *p_svc = NULL;

  p_svc = p_priv->p_container->get_svc(p_priv->p_container, context);
  if(NULL == p_svc)
  {
    p_svc = construct_svc(context);
    p_svc->msg_proc = pnp_msg_proc;
    p_svc->sm_proc = pnp_sm_proc;
    p_priv->p_container->add_svc(p_priv->p_container, p_svc);
  }
  return p_svc;
}

static void pnp_svc_on_decorate(handle_t handle, os_msg_t *p_msg)
{
  if((PNP_SVC_MOUNT == p_msg->content) || (PNP_SVC_UNMOUNT == p_msg->content))
  {
    p_msg->is_ext = TRUE;
  }
  else
  {
    p_msg->is_ext = FALSE;
  }
}

handle_t pnp_service_init(pnp_svc_init_para_t *p_para)
{
  pnp_svc_t *p_this = mtos_malloc(sizeof(pnp_svc_t));
  pnp_svc_priv_t *p_priv = mtos_malloc(sizeof(pnp_svc_priv_t));
  svc_container_t *p_container = construct_svc_container("pnp svc",
    p_para->service_prio, p_para->nstksize, PNP_SVC_MSG_DEPTH);
  
  MT_ASSERT(p_para != NULL);
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_container != NULL);

  memset(p_priv, 0, sizeof(pnp_svc_priv_t));
  p_this->get_svc_instance = get_pnp_svc_instance;
  p_this->p_data = p_priv;
  p_this->eth_monitor = NULL;
  p_this->usb_eth_monitor = NULL;
  p_this->usb_monitor = NULL;
  p_this->wifi_monitor = NULL;
  p_this->gprs_monitor = NULL;
  p_this->g3_dongle_monitor = NULL;
  p_this->hdmi_monitor = NULL;
  p_this->ip_camara_monitor = NULL;
  
  p_this->is_plug_in = FALSE;
  p_this->is_usb_plug_in = FALSE;
  p_priv->p_msg_ext = mtos_align_malloc(PNP_SVC_MSG_DEPTH * sizeof(pnp_param_t), 8);
  MT_ASSERT(p_priv->p_msg_ext != NULL);
  p_priv->msg_id = p_container->get_msgq_id(p_container);
  mtos_messageq_attach(p_priv->msg_id,
      p_priv->p_msg_ext, sizeof(pnp_param_t), PNP_SVC_MSG_DEPTH);

  p_priv->p_container = p_container;
  p_priv->callback = p_para->callback;
  class_register(PNP_SVC_CLASS_ID, p_this);

#ifndef WIN32
  p_this->p_eth_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  p_this->p_wifi_dev = get_wifi_handle();
  //p_this->p_wifi_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
#endif  
  p_priv->p_pnp_svc = p_this->get_svc_instance(p_this, APP_FRAMEWORK);
  
  p_priv->p_pnp_svc->on_decorate_cmd = pnp_svc_on_decorate;

  return p_this;
}

