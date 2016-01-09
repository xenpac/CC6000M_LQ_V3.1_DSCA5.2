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
#include "lrc_input_pin_intra.h"
#include "err_check_def.h"



static lrc_in_pin_private_t *get_priv(handle_t _this)
{
   CHECK_FAIL_RET_NULL(_this != NULL);

   return &((lrc_in_pin_t *)_this)->private_data;

}


static void request_sample(handle_t _this, media_sample_t *p_sample)
{
  s32 ret = -1;
  ipin_t *p_ipin = (ipin_t *)_this;
  lrc_in_pin_private_t *p_priv = get_priv(_this);
  iasync_reader_t *p_reader = p_priv->p_reader;
  if (!p_ipin->is_connected(p_ipin))
  {
    return ;
  }
  ret = p_reader->request(p_reader, &p_sample->format, 4096, 0, 0, NULL, 0);
}

static RET_CODE lrc_in_pin_on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE lrc_in_pin_on_start(handle_t _this)
{
  ipin_t *p_ipin = (ipin_t *)_this;
  ipin_t *p_connecter = (ipin_t *)p_ipin->get_connected(_this);
  lrc_in_pin_private_t *p_priv = get_priv(_this);
  media_sample_t sample;

  if (!p_ipin->is_connected(p_ipin)
    || (p_connecter == NULL))
  {
    return SUCCESS;
  }
  p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE,
                            (void **)&p_priv->p_reader);

  memset(&sample, 0, sizeof(media_sample_t));

  request_sample(_this, &sample);


  return SUCCESS;
}

static BOOL notify_allocator(handle_t _this, 
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

lrc_in_pin_t *lrc_in_pin_create(lrc_in_pin_t *p_in_pin, interface_t *p_owner)
{
  lrc_in_pin_private_t *p_priv = NULL;
  base_input_pin_t *p_input_pin = NULL;
  ipin_t *p_ipin = NULL;
  //interface_t *p_interface = NULL;
  transf_input_pin_para_t para;
  media_format_t media_format = {MT_FILE_DATA};

  CHECK_FAIL_RET_NULL(p_in_pin != NULL);

  para.p_filter = p_owner;
  para.p_name = "lrc_in_pin";
  
  //create base class
  transf_input_pin_create(&p_in_pin->m_pin, &para);

  //init private date
  p_priv = get_priv((handle_t)p_in_pin);
  p_priv->p_this = p_in_pin;

  p_input_pin = (base_input_pin_t *)p_in_pin;
  p_input_pin->notify_allocator = notify_allocator;
  
  p_ipin = (ipin_t *)p_in_pin;
  p_ipin->on_open = lrc_in_pin_on_open;
  p_ipin->on_start = lrc_in_pin_on_start;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  
  return p_in_pin;
}

//END OF FILE

