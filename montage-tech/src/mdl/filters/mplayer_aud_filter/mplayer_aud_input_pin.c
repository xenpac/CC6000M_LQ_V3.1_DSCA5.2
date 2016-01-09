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
#include "ifilter.h"
#include "eva_filter_factory.h"
#include "mplayer_aud_input_pin_intra.h"
#include "err_check_def.h"

static BOOL _in_pin_notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

mplayer_aud_in_pin_t * mplayer_aud_in_pin_create(mplayer_aud_in_pin_t *p_pin, interface_t *p_owner)
{
  mplayer_aud_in_pin_private_t *p_priv = NULL;
  //interface_t *p_interface = NULL;
  base_input_pin_t *p_input_pin = NULL;
  transf_input_pin_para_t transf_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t media_format = {MT_FILE_DATA};
  
  //check input parameter
  CHECK_FAIL_RET_NULL(p_pin != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);

  
  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "mplayer_aud_i_pin";
  transf_input_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &p_pin->private_data;
  memset(p_priv, 0, sizeof(mplayer_aud_in_pin_private_t));
  p_priv->p_this = p_pin;


  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->notify_allocator = _in_pin_notify_allocator;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  return p_pin;
}

