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
#include "transf_output_pin.h"
#include "str_output_pin_intra.h"


static str_out_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((str_out_pin_t *)_this)->private_data;
}

static RET_CODE str_out_decide_buffer_size(handle_t _this)
{
  str_out_pin_private_t *p_priv = get_priv(_this);
  allocator_properties_t *p_prop = &p_priv->properties;
  imem_allocator_t *p_alloc = p_priv->p_alloc;

  p_alloc->get_properties(p_alloc, p_prop);
  p_prop->buffers = 2;
  p_prop->buffer_size = 1024;
  p_prop->use_virtual_space = TRUE;
  p_alloc->set_properties(p_alloc, &p_priv->properties, NULL);
  return SUCCESS;
}

RET_CODE str_output_pin_on_sample_evt(handle_t _this, sample_evt_t evt,
                              media_sample_t *p_sample)
{
  switch(evt)
  {
    case EVT_SAMPLE_DESTROY:
      if(p_sample->p_data != NULL)
      {
        //  mtos_free(p_sample->p_data);
      }
      break;
    default:
        break;
  }
  
  return SUCCESS;
}


void str_out_pin_cfg(str_out_pin_t *p_output_pin, u32 buffer_size, void *p_buffer)
{

}


static RET_CODE str_out_pin_on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE str_out_pin_on_close(handle_t _this)
{
  return SUCCESS;
}

str_out_pin_t *str_out_pin_create(str_out_pin_t *p_pin, interface_t *p_owner)
{
  str_out_pin_private_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
  transf_output_pin_para_t transf_pin_para = {0};
  media_format_t media_format = {MT_IMAGE};

  //check input parameter
  MT_ASSERT(p_pin != NULL);
  p_ipin = (ipin_t *)p_pin;

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "str_out_pin";
  transf_output_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &(p_pin->private_data);
  memset(p_priv, 0x0, sizeof(str_out_pin_private_t));
  p_priv->p_this = p_pin;
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);

  //init member function

  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_pin;
  p_output_pin->decide_buffer_size = str_out_decide_buffer_size;
  p_output_pin->on_sample_evt = str_output_pin_on_sample_evt;

  p_ipin->on_open = str_out_pin_on_open;
  p_ipin->on_close = str_out_pin_on_close;

  return p_pin;
}

