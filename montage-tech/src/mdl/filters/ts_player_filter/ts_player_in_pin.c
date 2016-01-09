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
#include "lib_util.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"

#include "mt_time.h"
#include "vfs.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"

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
#include "ifilter.h"
#include "input_pin.h"
#include "output_pin.h"
#include "eva_filter_factory.h"
#include "transf_filter.h"
#include "transf_input_pin.h"
#include "ts_player_in_pin_intra.h"


#include "ts_player_filter.h"

static ts_player_in_pin_private_t *get_priv(handle_t _this)
{
  ts_player_in_pin_t *p_in_pin = (ts_player_in_pin_t *)_this;
  
  MT_ASSERT(p_in_pin != NULL);

  return &p_in_pin->private_data;
}

static void request_sample(handle_t _this, media_format_t *p_format, u32 fill_len, 
  s64 position, u32 seek_op)
{
  ts_player_in_pin_private_t *p_priv = get_priv(_this);
  iasync_reader_t *p_reader = p_priv->p_reader;
  u32 seek_mode = 0;

  switch (seek_op)
  {
  case TS_PLAYER_SEEK_SET + 1:
    seek_mode = VFS_SEEK_SET + 1;
    break;
  case TS_PLAYER_SEEK_END + 1:
    seek_mode = VFS_SEEK_END + 1;
    break;
  case TS_PLAYER_SEEK_CUR + 1:
    seek_mode = VFS_SEEK_CUR + 1;
    break;
  default:
    break;
  }
  MT_ASSERT((position % 188) == 0);
  p_reader->request(p_reader, p_format, fill_len, 0, position, NULL, seek_mode);
}

static BOOL notify_allocator(handle_t _this, 
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

static RET_CODE on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE on_start(handle_t _this)
{
  ipin_t *p_ipin = (ipin_t *)_this;
  ipin_t *p_connecter = (ipin_t *)p_ipin->get_connected(_this);
  ts_player_in_pin_private_t *p_priv = get_priv(_this);

  p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE,
    (void **)&p_priv->p_reader);

  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  
  return SUCCESS;
}

ts_player_in_pin_t *ts_player_in_pin_create(ts_player_in_pin_t *p_in_pin, interface_t *p_owner)
{
  ts_player_in_pin_private_t *p_priv = NULL;
  base_input_pin_t *p_input_pin = NULL;
  ipin_t *p_ipin = NULL;
  //interface_t *p_interface = NULL;
  transf_input_pin_para_t para;
  media_format_t media_format = {MT_FILE_DATA};

  MT_ASSERT(p_in_pin != NULL);

  para.p_filter = p_owner;
  para.p_name = "player_in_pin";

  p_in_pin->request_sample = request_sample;
  //create base class
  transf_input_pin_create(&p_in_pin->m_pin, &para);

  //init private date
  p_priv = get_priv((handle_t)p_in_pin);
  p_priv->p_this = p_in_pin;
  p_priv->p_reader = NULL;

  p_input_pin = (base_input_pin_t *)p_in_pin;
  p_input_pin->notify_allocator = notify_allocator;
  
  p_ipin = (ipin_t *)p_in_pin;
  p_ipin->on_open = on_open;
  p_ipin->on_start = on_start;
  p_ipin->on_close = on_close;
  p_ipin->add_supported_media_format(p_ipin, &media_format);

  return p_in_pin;
}

//END OF FILE

