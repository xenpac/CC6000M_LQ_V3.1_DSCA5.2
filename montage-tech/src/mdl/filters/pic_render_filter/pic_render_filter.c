/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_mem.h"

//hal
#include "hal_misc.h"

//drv
//#include "usb_drv.h"
#include "common.h"
#include "lib_memp.h"
#include "lib_rect.h"
#include "drv_dev.h"
#include "gpe_vsb.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "lib_util.h"
#include "lib_rect.h"

//eva

#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"
#include "ifilter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"
#include "common_filter.h"
#include "pic_render_filter.h"
#include "pic_render_pin_intra.h"

#include "pic_render_filter_intra.h"

static pic_render_filter_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((pic_render_filter_t *)_this)->private_data;
}

static RET_CODE on_command(handle_t _this, icmd_t *p_cmd)
{
  pic_render_filter_private_t *p_priv = get_priv(_this);
  
  switch (p_cmd->cmd)
  {
    case PIC_RENDER_CONFIG:
      pic_rend_pin_cfg(&p_priv->m_pin, (pic_rend_cfg_t *)p_cmd->p_para);
      break;

    case PIC_RENDER_SHOW:
      pic_rend_pin_show(&p_priv->m_pin, p_cmd->lpara);
      break;
      
    case PIC_RENDER_STOP_ANIMATION:
      pic_rend_stop_anim(&p_priv->m_pin);
      break;

    case PIC_RENDER_CLEAR:
      pic_rend_pin_clear(&p_priv->m_pin, p_cmd->lpara, p_cmd->p_para);
      break;

    case PIC_RENDER_PAUSE:
      pic_rend_pause_anim(&p_priv->m_pin);
      break;

    case PIC_RENDER_RESUME:
      pic_rend_resume_anim(&p_priv->m_pin);
      break;
      
    default:
      return ERR_FAILURE;
  }
  
  return SUCCESS;
}

static void on_destroy(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t * pic_render_filter_create(void *p_para)
{
  pic_render_filter_private_t *p_priv = NULL;
  pic_render_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;

  //create filter
  p_ins = mtos_malloc(sizeof(pic_render_filter_t));
  MT_ASSERT(p_ins != NULL);
  memset(p_ins, 0, sizeof(pic_render_filter_t));
  
  //create base class
  sink_filter_create(&p_ins->_sink_filter, NULL);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(pic_render_filter_private_t));
  p_priv->p_this = p_ins;
  
  //init member function

  //overload virtual interface
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = on_command;

  p_ifilter->_interface.on_destroy = on_destroy;

  //create child pin
  pic_render_pin_create(&p_priv->m_pin, (interface_t *)p_ins, (pic_rend_para_t *)p_para);

  return (ifilter_t *)p_ins;
}

//end of file

