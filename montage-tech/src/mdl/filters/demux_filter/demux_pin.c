/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"

//driver
#include "dmx.h"

//util
#include "class_factory.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "ipin.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "input_pin.h"
#include "output_pin.h"
#include "source_pin.h"
#include "ifilter.h"
#include "chain.h"
#include "demux_filter.h"
#include "demux_interface.h"
#include "demux_pin_intra.h"
#include "err_check_def.h"

static inline demux_pin_t *_get_dmx_pin(handle_t _this)
{
  CHECK_FAIL_RET_NULL(_this != NULL);
  return (demux_pin_t *)(((dmx_interface_t *)_this)->p_owner);
}

static RET_CODE dmx_pin_i_request(handle_t _this, void *p_data)
{
  demux_pin_t *p_dmx_pin = _get_dmx_pin(_this);
  RET_CODE ret = ERR_FAILURE;
  
  if(p_dmx_pin->request != NULL)
  {
    ret = p_dmx_pin->request(p_dmx_pin, p_data);
  }
  else
  {
    //please set pin type.
    CHECK_FAIL_RET_CODE(0);
  }
  
  return ret;
}

static RET_CODE dmx_pin_i_free(handle_t _this, void *p_data)
{
  demux_pin_t *p_dmx_pin = _get_dmx_pin(_this);
  RET_CODE ret = ERR_FAILURE;

  if(p_dmx_pin->free != NULL)
  {
    ret = p_dmx_pin->free(p_dmx_pin, p_data);
  }
  else
  {
    //please set pin type.
    CHECK_FAIL_RET_CODE(0);
  }
  
  return ret;
}

static BOOL dmx_pin_set_type(handle_t _this, dmx_pin_type_t type)
{
  if(DMX_PIN_PSI_TYPE == type)
  {
    attach_psi_pin_instance(_this);
  }
  else if(DMX_PIN_REC_TYPE == type)
  {
    attach_rec_pin_instance(_this);
  }
  else if(DMX_PIN_PES_TYPE == type)
  {
    attach_pes_pin_instance(_this);
  }
  else
  {
    CHECK_FAIL_RET_FALSE(0);
  }

  return TRUE;
}

static RET_CODE dmx_pin_on_open(handle_t _this)
{
  OS_PRINTF("please set the pin type before do open\n");
  CHECK_FAIL_RET_CODE(0);
  return ERR_FAILURE;
}


demux_pin_t * demux_pin_create(demux_pin_t *p_dmx_pin, interface_t *p_owner)
{
  demux_pin_private_t *p_priv = NULL;
  source_pin_para_t src_pin_para = {0};
  ipin_t *p_ipin = NULL;

  //check input parameter
  CHECK_FAIL_RET_NULL(p_dmx_pin != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);

  //init dmx pin private data
  p_dmx_pin->p_child_priv = NULL;

  //init dmx pin interface.
  p_priv = &p_dmx_pin->dmx_pin_priv;
  p_priv->m_dmx_inter.p_owner = (interface_t *)p_dmx_pin;
  p_priv->m_dmx_inter.i_request = dmx_pin_i_request;
  p_priv->m_dmx_inter.i_free = dmx_pin_i_free;

  //create base class
  src_pin_para.p_filter = p_owner;
  src_pin_para.stream_mode = STREAM_MODE_PUSH;
  src_pin_para.p_name = "demux_pin";
  source_pin_create(&p_dmx_pin->m_pin, &src_pin_para);

  //init member func
  p_dmx_pin->set_pin_type = dmx_pin_set_type;
  p_dmx_pin->config = NULL;
  p_dmx_pin->request = NULL;
  p_dmx_pin->free = NULL;

  //overload virtual function
  p_ipin = (ipin_t *)p_dmx_pin;
  p_ipin->on_open = dmx_pin_on_open;
  p_ipin->attach_interface(p_ipin, DMX_INTERFACE_NAME, &p_priv->m_dmx_inter);
  
  return p_dmx_pin;
}

