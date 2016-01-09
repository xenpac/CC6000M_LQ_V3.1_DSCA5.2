/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
//std
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
#include "lib_util.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"

#include "ifilter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"

#include "demux_interface.h"
#include "epg_type.h"
#include "epg_database.h"
#include "demux_filter.h"
#include "epg_filter.h"
#include "epg_pin_intra.h"
#include "epg_filter_intra.h"
#include "err_check_def.h"

#define EPG_FILTER_DEBUG

#ifdef  EPG_FILTER_DEBUG
#define EF_PRINT  OS_PRINTF("%s, %d liens : ",__FUNCTION__, __LINE__); OS_PRINTF
#else
#define EF_PRINT  DUMMY_PRINTF
#endif

static epg_filter_priv_t *epg_filter_get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(_this != NULL);
  return &(((epg_filter_t *)_this)->m_priv_data);
}

static RET_CODE epg_filter_on_command(handle_t _this, icmd_t *p_cmd)
{
  epg_filter_priv_t *p_priv = epg_filter_get_priv(_this);
  epg_pin_t         *p_pin  = &p_priv->m_pin;
  
  CHECK_FAIL_RET_CODE(p_cmd != NULL);
  switch(p_cmd->cmd)
  {
    case EPG_FILTER_CMD_CONFIG:
      p_pin->config(p_pin, (epg_filter_cfg_t *)p_cmd->p_para);
      break;

    case EPG_FILTER_CMD_SET_INFO:
      p_pin->set_info(p_pin, (epg_prog_info_t *)p_cmd->p_para);
      break;
    
    default:
      EF_PRINT("%s", p_cmd->cmd);
      CHECK_FAIL_RET_CODE(0);
      break;
  }

  return SUCCESS;
}

/*!
 state machine
 */
static void epg_filter_active_loop(handle_t _this)
{
  epg_filter_priv_t *p_priv = epg_filter_get_priv(_this);
  epg_pin_t         *p_pin  = &p_priv->m_pin;

  p_pin->active_loop(p_pin);
}

static RET_CODE epg_filter_on_open(handle_t _this)
{
  epg_filter_priv_t *p_priv    = epg_filter_get_priv(_this);
  ifilter_t         *p_ifilter = (ifilter_t *)p_priv->p_this;

  p_ifilter->set_active_enter(p_ifilter, epg_filter_active_loop);

  return SUCCESS;
}

static RET_CODE epg_filter_on_close(handle_t _this)
{
  epg_filter_priv_t *p_priv = epg_filter_get_priv(_this);
  ifilter_t *p_ifilter      = (ifilter_t *)p_priv->p_this;

  p_ifilter->set_active_enter(p_ifilter, NULL);

  return SUCCESS;
}

static void epg_filter_on_destroy(handle_t _this)
{ 
  mtos_free(_this);
}

/*!
 create epg filter.
 */
ifilter_t *epg_filter_create(void *p_para)
{
  interface_t *p_interface  = NULL;
  ifilter_t *p_ifilter = NULL;
  epg_filter_t        *p_epg_filter = NULL;
  epg_filter_priv_t *p_priv       = NULL;

  p_epg_filter = (epg_filter_t *)mtos_malloc(sizeof(epg_filter_t));
  CHECK_FAIL_RET_NULL(p_epg_filter != NULL);
  memset(p_epg_filter, 0x0, sizeof(epg_filter_t));

  p_priv = &p_epg_filter->m_priv_data;
  p_priv->p_this = (void *)p_epg_filter;
  sink_filter_create(&p_epg_filter->m_sink_filter, NULL);

  p_interface = (interface_t *)p_epg_filter;
  p_interface->_rename(p_interface, "epg_filter");
  p_interface->on_destroy = epg_filter_on_destroy;

  p_ifilter = (ifilter_t *)p_epg_filter;
  p_ifilter->on_open    = epg_filter_on_open;
  p_ifilter->on_close   = epg_filter_on_close;
  p_ifilter->on_command = epg_filter_on_command;

  epg_pin_create(&p_priv->m_pin, p_interface);

  return p_ifilter;
}
