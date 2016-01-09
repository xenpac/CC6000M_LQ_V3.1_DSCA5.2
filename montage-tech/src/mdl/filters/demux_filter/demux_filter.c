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
#include "src_filter.h"
#include "demux_filter.h"
#include "demux_interface.h"
#include "demux_pin_intra.h"
#include "demux_filter_intra.h"
#include "err_check_def.h"

static demux_filter_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((demux_filter_t *)_this)->private_data;
}

static RET_CODE dmx_on_command(handle_t _this, icmd_t *p_cmd)
{
  demux_filter_private_t *p_priv = get_priv(_this);
  demux_pin_t *p_dmx_pin = (demux_pin_t *)p_cmd->p_para;
  RET_CODE ret = SUCCESS;
  
  if(p_dmx_pin != &p_priv->m_pin)
  {
    return ERR_FAILURE;
  }
  
  switch (p_cmd->cmd)
  {
    case DMX_SET_PIN_TYPE:
      p_dmx_pin->set_pin_type(p_dmx_pin, (dmx_pin_type_t)p_cmd->lpara);
      break;
    case DMX_CFG_PARA:
      p_dmx_pin->config(p_dmx_pin, (void *)p_cmd->lpara);
      break;
    case DMX_SET_PID:
      ret = p_dmx_pin->request(p_dmx_pin, (void *)p_cmd->lpara);
      break;
    case DMX_CLEAR:
      p_dmx_pin->free(p_dmx_pin, (void *)p_cmd->lpara);
      break;
    case DMX_RESET_PID:
      if (p_dmx_pin->reset_pid != NULL)
      {
        ret = p_dmx_pin->reset_pid(p_dmx_pin, (void *)p_cmd->lpara);
      }
      break;
    case DMX_PAUSE_RESUME:
      if (p_dmx_pin->pause_resume != NULL)
      {
        ret = p_dmx_pin->pause_resume(p_dmx_pin, p_cmd->lpara);
      }
      break;
    default:
      return ERR_FAILURE;
  }
  
  return ret;
}

static void dmx_on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t * dmx_filter_create(void *p_para)
{
  demux_filter_private_t *p_priv = NULL;
  demux_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;
  src_filter_para_t src_filter_para = {0};

  src_filter_para.dummy = 0;

  //create filter
  p_ins = mtos_malloc(sizeof(demux_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(demux_filter_t));
  
  //create base class
  src_filter_create(&p_ins->m_filter, &src_filter_para);

  //init private date
  p_priv = &p_ins->private_data;
  p_priv->p_this = p_ins; //this point

  //init member function

  //over loading the virtual function
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = dmx_on_command;
  p_ifilter->_interface.on_destroy = dmx_on_destory;

  //create it's pin
  demux_pin_create(&p_priv->m_pin, (interface_t *)p_ins);
  
  return (ifilter_t *)p_ins;
}

