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
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "common.h"
#include "drv_dev.h"
#include "lib_memp.h"
#include "lib_rect.h"
#include "pdec.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "transf_input_pin.h"
#include "transf_output_pin.h"
#include "pic_input_pin_intra.h"
#include "pic_output_pin_intra.h"

#include "ifilter.h"
#include "transf_filter.h"
#include "eva_filter_factory.h"
#include "Pic_filter.h"
#include "pic_filter_intra.h"
#include "pic_filter_intra.h"
#include "pic_output_pin_intra.h"


static pic_out_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((pic_out_pin_t *)_this)->private_data;
}

static RET_CODE pic_out_decide_buffer_size(handle_t _this)
{
  pic_out_pin_private_t *p_priv = get_priv(_this);
  imem_allocator_t *p_alloc = p_priv->p_alloc;

  p_alloc->get_properties(p_alloc, &p_priv->properties);
  p_priv->properties.use_virtual_space = TRUE;
  p_priv->properties.buffers = 1;
  
  p_priv->properties.buffer_size = 1024;
  p_alloc->set_properties(p_alloc, &p_priv->properties, NULL);

  return SUCCESS;
}

static RET_CODE pic_out_pin_on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE pic_out_pin_on_close(handle_t _this)
{
  return SUCCESS;
}

RET_CODE pic_out_on_sample_evt(handle_t _this, sample_evt_t evt,
                              media_sample_t *p_sample)
{
  ifilter_t *p_filter = NULL;
  ipin_t *p_pin = (ipin_t *)_this;
  
  switch(evt)
  {
    case EVT_SAMPLE_DESTROY:
      p_filter = (ifilter_t *)p_pin->get_filter(p_pin);
      
      pic_filter_free_sample_data((pic_filter_t *)p_filter);
      break;

    default:
        break;
  }

  return SUCCESS;
}

pic_out_pin_t * pic_out_pin_create(pic_out_pin_t *p_pin, interface_t *p_owner)
{
  pic_out_pin_private_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
//  interface_t *p_interface = NULL;
  transf_output_pin_para_t transf_pin_para = {0};
  media_format_t media_format = {MT_IMAGE};
  
  //check input parameter
  MT_ASSERT(p_pin != NULL);
  p_ipin = (ipin_t *)p_pin;

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "pic_out_pin";
  transf_output_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &(p_pin->private_data);
  memset(p_priv, 0x0, sizeof(pic_out_pin_private_t));
  p_priv->p_this = p_pin;
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  
  //init member function

  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_pin;
  p_output_pin->decide_buffer_size = pic_out_decide_buffer_size;
  p_output_pin->on_sample_evt = pic_out_on_sample_evt;
  
  p_ipin->on_open = pic_out_pin_on_open;
  p_ipin->on_close = pic_out_pin_on_close;

  return p_pin;
}

