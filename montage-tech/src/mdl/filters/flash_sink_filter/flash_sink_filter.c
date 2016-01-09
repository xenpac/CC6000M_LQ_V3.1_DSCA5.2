/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
// std
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"
#include "drv_dev.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_mutex.h"
#include "mtos_event.h"
#include "mt_time.h"
#include "vfs.h"
#include "drv_dev.h"


//util
#include "class_factory.h"
#include "simple_queue.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "ifilter.h"
#include "sink_pin.h"
#include "ifilter.h"
#include "sink_filter.h"
#include "chain.h"
#include "eva_filter_factory.h"
#include "flash_sink_pin_intra.h"
#include "flash_sink_filter_intra.h"
#include "flash_sink_filter.h"
#include "err_check_def.h"

#if 0
static flash_sink_filter_private_t * get_priv(handle_t _this)
{
  return &((flash_sink_filter_t *)_this)->private_data;
}
#endif
static RET_CODE fsink_on_command(handle_t _this, icmd_t *p_cmd)
{
  return SUCCESS;
}

static RET_CODE fsink_on_evt(handle_t _this, os_msg_t *p_msg)
{
  return SUCCESS;
}


static void on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t * flash_sink_filter_create(void *p_para)
{
  flash_sink_filter_private_t *p_priv = NULL;
  flash_sink_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;

  //create filter
  p_ins = mtos_malloc(sizeof(flash_sink_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(flash_sink_filter_t));
  
  //create base class
  sink_filter_create(&p_ins->_sink_filter, NULL);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(flash_sink_filter_private_t));
  p_priv->p_this = p_ins;
  
  //init member function

  //overload virtual interface
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = fsink_on_command;
  p_ifilter->on_process_evt = fsink_on_evt;
  p_ifilter->_interface.on_destroy = on_destory;
  
  //create child pin
  flash_sink_pin_create(&p_priv->m_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}

