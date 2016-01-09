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

#include "drv_dev.h"
#include "lib_memp.h"
#include "pdec.h"

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
#include "str_input_pin_intra.h"

static RET_CODE str_in_pin_on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE str_in_pin_on_close(handle_t _this)
{
  return SUCCESS;
}

static BOOL str_in_pin_notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}


str_in_pin_t *str_in_pin_create(str_in_pin_t *p_pin, interface_t *p_owner)
{
  str_in_pin_private_t *p_priv = NULL;
  base_input_pin_t *p_input_pin = NULL;
  transf_input_pin_para_t transf_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t media_format = {MT_FILE_DATA};

  //check input parameter
  MT_ASSERT(p_pin != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "str_in_pin";
  transf_input_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &p_pin->private_data;
  memset(p_priv, 0, sizeof(str_in_pin_private_t));
  p_priv->p_this = p_pin; //this point
  p_priv->p_reader = NULL;

  //init member function

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->notify_allocator = str_in_pin_notify_allocator;
  p_ipin = (ipin_t *)p_pin;
  p_ipin->on_open = str_in_pin_on_open;
  p_ipin->on_close = str_in_pin_on_close;

  p_ipin->add_supported_media_format(p_ipin, &media_format);

  return p_pin;
}

