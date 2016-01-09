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
#include "file_sink_pin_intra.h"
#include "file_sink_filter_intra.h"
#include "file_sink_filter.h"
#include "err_check_def.h"

static fsink_filter_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((fsink_filter_t *)_this)->private_data;
}

static RET_CODE fsink_on_command(handle_t _this, icmd_t *p_cmd)
{
  fsink_filter_private_t *p_priv = get_priv(_this);
  fsink_pin_t *p_pin = &p_priv->m_pin;
  switch (p_cmd->cmd)
  {
    case FSINK_CFG_FILE_NAME:
      p_pin->set_file_name(&p_priv->m_pin, (u16 *)p_cmd->p_para, p_cmd->lpara);
      break;
      
    case FSINK_CFG_SINK_MODE:
      p_pin->set_sink_mode(&p_priv->m_pin, p_cmd->lpara, (media_idx_t *)p_cmd->p_para);
      break;
      
    case FSINK_CFG_SINK_SIZE:
      p_pin->set_sink_ready_size(&p_priv->m_pin, p_cmd->lpara, *(u64 *)p_cmd->p_para);
      break;

    case FSINK_SEEK:
      p_pin->seek(&p_priv->m_pin, *(s64 *)p_cmd->p_para, p_cmd->lpara);
      break;
      
    default:
      return ERR_FAILURE;
  }
  return SUCCESS;
}

static RET_CODE fsink_on_evt(handle_t _this, os_msg_t *p_msg)
{
  ifilter_t *p_ifilter = (ifilter_t *)_this;
  
  switch(p_msg->content)
  {
    case FILE_SINK_MSG_NO_ENOUGH_MEM:
      p_ifilter->send_message_upriver(_this, p_msg);
      break;
    default:
      break;
  }
  return SUCCESS;
}


static void on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t * fsink_filter_create(void *p_para)
{
  fsink_filter_private_t *p_priv = NULL;
  fsink_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;

  //create filter
  p_ins = mtos_malloc(sizeof(fsink_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(fsink_filter_t));
  
  //create base class
  sink_filter_create(&p_ins->_sink_filter, NULL);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(fsink_filter_private_t));
  p_priv->p_this = p_ins;
  
  //init member function

  //overload virtual interface
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = fsink_on_command;
  p_ifilter->on_process_evt = fsink_on_evt;
  p_ifilter->_interface.on_destroy = on_destory;
  
  //create child pin
  fsink_pin_create(&p_priv->m_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}

