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
#include "transf_input_pin.h"
#include "record_input_pin_intra.h"

static rec_in_pin_private_t *get_priv(handle_t _this)
{
  rec_in_pin_t *p_in_pin = (rec_in_pin_t *)_this;
  
  MT_ASSERT(p_in_pin != NULL);

  return &p_in_pin->private_data;
}

static void on_receive(handle_t _this, media_sample_t *p_sample)
{
  //push mode do nothing
}

static BOOL notify_allocator(handle_t _this, 
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

rec_in_pin_t *record_in_pin_create(rec_in_pin_t *p_in_pin, interface_t *p_owner)
{
  rec_in_pin_private_t *p_priv = NULL;
  base_input_pin_t *p_input_pin = NULL;
  ipin_t *p_ipin = NULL;
  //interface_t *p_interface = NULL;
  transf_input_pin_para_t para;
  media_format_t media_format = {MT_RECODE};

  MT_ASSERT(p_in_pin != NULL);

  para.p_filter = p_owner;
  para.p_name = "rec_in_pin";
  
  //create base class
  transf_input_pin_create(&p_in_pin->m_pin, &para);

  //init private date
  p_priv = get_priv((handle_t)p_in_pin);
  p_priv->p_this = p_in_pin;

  p_input_pin = (base_input_pin_t *)p_in_pin;
  p_input_pin->on_receive = on_receive;
  p_input_pin->notify_allocator = notify_allocator;
  
  p_ipin = (ipin_t *)p_in_pin;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  
  return p_in_pin;
}

//END OF FILE

