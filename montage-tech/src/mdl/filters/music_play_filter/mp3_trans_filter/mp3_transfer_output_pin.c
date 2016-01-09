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
#include "mp3_transfer_output_pin_intra.h"
#include "err_check_def.h"

static mp3_trans_out_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((mp3_trans_out_pin_t *)_this)->private_data;
}

static RET_CODE mp3_trans_out_pin_decide_buffer_size(handle_t _this)
{
  mp3_trans_out_pin_private_t *p_priv = get_priv(_this);
  imem_allocator_t *p_alloc = p_priv->p_alloc;
  allocator_properties_t properties;
  
  //config mem_alloc
  p_alloc->get_properties(p_alloc, &properties);
  properties.buffers = 20;
  properties.buffer_size = 4;
  p_alloc->set_properties(p_alloc, &properties, NULL);
  return SUCCESS;
}


static RET_CODE mp3_trans_out_pin_on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE mp3_trans_out_pin_on_close(handle_t _this)
{
  return SUCCESS;
}

mp3_trans_out_pin_t * mp3_trans_out_pin_create(mp3_trans_out_pin_t *p_pin, 
                                                        interface_t *p_owner)
{
  mp3_trans_out_pin_private_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
  //interface_t *p_interface = NULL;
  transf_output_pin_para_t transf_pin_para = {0};
  media_format_t media_format = {MT_AUDIO};

  //check input parameter
  CHECK_FAIL_RET_NULL(p_pin != NULL);
  p_ipin = (ipin_t *)p_pin;

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "mp3_trans_o_pin";
  transf_output_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &(p_pin->private_data);
  memset(p_priv, 0x0, sizeof(mp3_trans_out_pin_private_t));
  p_priv->p_this = p_pin;
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  
  //init member function

  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_pin;
  p_output_pin->decide_buffer_size = mp3_trans_out_pin_decide_buffer_size;
  
  p_ipin->on_open = mp3_trans_out_pin_on_open;
  p_ipin->on_close = mp3_trans_out_pin_on_close;

  return p_pin;
}
