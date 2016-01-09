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
#include "mtos_task.h"

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
#include "pic_input_pin_intra.h"

#define PIECE_SIZE  ((512) * (KBYTES))

static pic_in_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((pic_in_pin_t *)_this)->private_data;
}

static RET_CODE pic_in_pin_on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE pic_in_pin_on_close(handle_t _this)
{
  return SUCCESS;
}

void pic_in_pin_cfg(pic_in_pin_t *p_this, u32 buffer_size, u8 *p_buffer, BOOL is_push)
{
  pic_in_pin_private_t *p_priv = get_priv(p_this);

  p_priv->pic_buff_size = buffer_size;
  p_priv->p_pic_buf = p_buffer;
  p_priv->is_push = is_push;
}

static void request_sample(handle_t _this, media_sample_t *p_sample)
{
  pic_in_pin_private_t *p_priv = get_priv(_this);
  iasync_reader_t *p_reader = p_priv->p_reader;
  u8 *p_request_buffer = NULL;

  if (p_priv->p_pic_buf != NULL)
  {
    p_request_buffer = p_priv->p_pic_buf + p_priv->current_piece * PIECE_SIZE;
  }
  p_priv->current_piece++;

  p_reader->request(p_reader, &p_sample->format, PIECE_SIZE,
    0, 0, p_request_buffer, 0);
}

static RET_CODE pic_in_pin_on_start(handle_t _this)
{
  ipin_t *p_ipin = (ipin_t *)_this;
  ipin_t *p_connecter = (ipin_t *)p_ipin->get_connected(_this);
  pic_in_pin_private_t *p_priv = get_priv(_this);
  media_sample_t sample = {{0}};

  if(!p_priv->is_push)
  {
    p_priv->current_piece = 0;

    p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE,
      (void **)&p_priv->p_reader);
    
    request_sample(_this, &sample);
  }
  
  return SUCCESS;
}

static void pic_in_pin_on_receive(handle_t _this, media_sample_t * p_sample)
{
  pic_in_pin_private_t *p_priv = get_priv(_this);

  mtos_task_sleep(10);
  if (p_priv->p_pic_buf == NULL)
  {
    return ;
  }
  
  if(p_priv->is_push)
  {
    //copy data from sample to input buffer.
    MT_ASSERT(p_sample->payload <= p_priv->pic_buff_size);
    
    memcpy(p_priv->p_pic_buf, p_sample->p_data, p_sample->payload);
  }
  else
  {
    if(p_priv->pic_buff_size > p_priv->current_piece * PIECE_SIZE)
    {
      request_sample(_this, p_sample);
    }
  }
}

static BOOL pic_in_pin_notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

pic_in_pin_t * pic_in_pin_create(pic_in_pin_t *p_pin, interface_t *p_owner)
{
  pic_in_pin_private_t *p_priv = NULL;
//  interface_t *p_interface = NULL;
  base_input_pin_t *p_input_pin = NULL;
  transf_input_pin_para_t transf_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t media_format = {MT_FILE_DATA}, media_format_net = {MT_NETWORK_REAL_DATA};

  //check input parameter
  MT_ASSERT(p_pin != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "pic_in_pin";
  transf_input_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &p_pin->private_data;
  memset(p_priv, 0, sizeof(pic_in_pin_private_t));
  p_priv->p_this = p_pin; //this point
  p_priv->p_reader = NULL;

  //init member function

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->notify_allocator = pic_in_pin_notify_allocator;
  p_input_pin->on_receive = pic_in_pin_on_receive;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->on_open = pic_in_pin_on_open;
  p_ipin->on_close = pic_in_pin_on_close;
  p_ipin->on_start = pic_in_pin_on_start;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  p_ipin->add_supported_media_format(p_ipin, &media_format_net);
  
  return p_pin;
}

