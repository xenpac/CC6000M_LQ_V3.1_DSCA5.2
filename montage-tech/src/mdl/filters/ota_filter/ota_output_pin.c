/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
//std
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"

//util
#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "dvb_protocol.h"


//driver
#include "dmx.h"

#include "class_factory.h"
#include "common.h"
#include "drv_dev.h"
#include "mdl.h"
#include "nim.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"
#include "output_pin.h"
#include "transf_output_pin.h"

#include "ifilter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"

#include "demux_interface.h"
#include "demux_filter.h"

#include "ota_public_def.h"
#include "ota_filter.h"
#include "ota_output_pin_intra.h"


static ota_output_pin_priv_t * ota_output_pin_get_priv(handle_t _this)
{
  MT_ASSERT(_this != NULL);
  return &(((ota_output_pin_t *)_this)->m_priv_data);
}

static RET_CODE ota_output_pin_on_start(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE ota_output_pin_on_stop(handle_t _this)
{
   return SUCCESS;
}

static RET_CODE ota_output_pin_decide_buffer_size(handle_t _this)
{
  ota_output_pin_priv_t *p_priv = ota_output_pin_get_priv(_this);
  imem_allocator_t *p_alloc = p_priv->p_alloc;

  p_alloc->get_properties(p_alloc, &p_priv->properties);
  p_priv->properties.use_virtual_space = TRUE;
  p_priv->properties.buffers = 1;
  
  p_priv->properties.buffer_size = 1;
  p_alloc->set_properties(p_alloc, &p_priv->properties, NULL);
  return SUCCESS;
}

static void out_output_pin_on_destroy(handle_t _this)
{

}
ota_output_pin_t * ota_output_pin_create(ota_output_pin_t *p_pin, interface_t *p_owner)
{
  ota_output_pin_priv_t *p_priv = &p_pin->m_priv_data;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
//  interface_t *p_interface = NULL;
  transf_output_pin_para_t transf_pin_para;
  media_format_t media_format = {MT_FILE_DATA};

  //check input parameter
  MT_ASSERT(p_pin != NULL);

  //create base class
  memset(&transf_pin_para,0,sizeof(transf_output_pin_para_t));
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "ota_out_pin";
  transf_output_pin_create(&p_pin->m_sink_pin, &transf_pin_para);

  //init private date
  memset(p_priv, 0x0, sizeof(ota_output_pin_priv_t));
  p_priv->p_this = (u8 *)p_pin;


  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_pin;
  p_output_pin->decide_buffer_size = ota_output_pin_decide_buffer_size;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  p_ipin->on_start = ota_output_pin_on_start;
  p_ipin->on_stop = ota_output_pin_on_stop;
  p_ipin->_interface.on_destroy = out_output_pin_on_destroy;
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  return p_pin;
}

