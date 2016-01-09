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
#include "mp3_transfer_input_pin_intra.h"
#include "mp3_player_filter.h"
#include "net_filter_interface.h"
#include "err_check_def.h"

static mp3_trans_in_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((mp3_trans_in_pin_t *)_this)->private_data;
}

static RET_CODE mp3_trans_in_pin_on_open(handle_t _this)
{
  
  return SUCCESS;
}

static RET_CODE mp3_trans_in_pin_on_close(handle_t _this)
{
  return SUCCESS;
}

static void request_sample(handle_t _this, media_sample_t *p_sample)
{
  media_format_t media_format = {MT_FILE_DATA};  
  mp3_trans_in_pin_private_t *p_priv = NULL;
  interface_t *p_interface = NULL;
  ipin_t *p_connecter = NULL;
  net_src_pin_request_t net_request = {0};

  p_priv = get_priv(_this);
  CHECK_FAIL_RET_VOID(p_priv != NULL);

  p_interface = ((ipin_t *)_this)->get_connected(_this);
  CHECK_FAIL_RET_VOID(p_interface != NULL);

  p_connecter = (ipin_t *)p_interface;

  if(p_connecter->is_supported_format(p_connecter, &media_format))
  {
      iasync_reader_t *p_reader = NULL;
      p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE, 
                                 (void **)&p_reader);
      
      if ((p_reader != NULL) && (p_sample->state != SAMP_STATE_INSUFFICIENT))
      {
        #ifndef WIN32
        p_reader->request(p_reader, &p_sample->format, SIZEREQ, 0, 0, NULL, 0);
        #else
        p_reader->request(p_reader, &p_sample->format, 6 * 1024, 0, 0, NULL, 0);
        #endif
        
      }
  }
  else
  {
      net_src_interface_t *p_net_src = NULL;
      void *p_data = p_sample->p_user_data; //for network music, this para is used for url addr.
      
      p_connecter->get_interface(p_connecter, NET_INTERFACE_NAME, (void **)&p_net_src);
      CHECK_FAIL_RET_VOID(p_net_src != NULL); 
      
      if(p_net_src != NULL && p_data != NULL)
      {
        net_request.p_url = (char *)p_data;
        net_request.context = 1;
        net_request.is_once_get_all_data = 0;
        net_request.is_monitor = 0;
        net_request.p_post = NULL;
        p_net_src->i_request(p_connecter, &net_request);  
      }
  }
  

}
/*
static RET_CODE mp3_trans_in_pin_on_start(handle_t _this)
{
  ipin_t *p_ipin = (ipin_t *)_this;
  ipin_t *p_connecter = (ipin_t *)p_ipin->get_connected(_this);
  mp3_trans_in_pin_private_t *p_priv = get_priv(_this);
  media_sample_t sample;

  p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE,
    (void **)&p_priv->p_reader);

  memset(&sample, 0, sizeof(media_sample_t));
  
  request_sample(_this, &sample);


  return SUCCESS;
}
*/
/*
static void mp3_trans_in_pin_on_receive(handle_t _this, media_sample_t *p_sample)
{
  //for test pull mode
  request_sample(_this, p_sample);
}
*/

static BOOL mp3_trans_in_pin_notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

mp3_trans_in_pin_t * mp3_trans_in_pin_create(mp3_trans_in_pin_t *p_pin, interface_t *p_owner)
{
  mp3_trans_in_pin_private_t *p_priv = NULL;
  //interface_t *p_interface = NULL;
  base_input_pin_t *p_input_pin = NULL;
  transf_input_pin_para_t transf_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t media_format = {MT_FILE_DATA}, media_foramt_net = {MT_NETWORK_REAL_DATA};
  
  //check input parameter
  CHECK_FAIL_RET_NULL(p_pin != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);

  
  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "mp3_trans_i_pin";
  transf_input_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &p_pin->private_data;
  memset(p_priv, 0, sizeof(mp3_trans_in_pin_private_t));
  p_priv->p_this = p_pin; //this point
  p_priv->p_reader = NULL;

  //init member function
  p_pin->request_next_sample = request_sample;

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->notify_allocator = mp3_trans_in_pin_notify_allocator;
  //p_input_pin->on_receive = mp3_trans_in_pin_on_receive;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->on_open = mp3_trans_in_pin_on_open;
  p_ipin->on_close = mp3_trans_in_pin_on_close;
  //p_ipin->on_start = mp3_trans_in_pin_on_start;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  p_ipin->add_supported_media_format(p_ipin, &media_foramt_net);
  return p_pin;
}

