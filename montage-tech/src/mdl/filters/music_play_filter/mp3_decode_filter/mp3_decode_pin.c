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
#include "mtos_misc.h"
#include "mtos_task.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"
#include "ifilter.h"
#include "eva_filter_factory.h"

#include "mp3_decode_pin_intra.h"

#include "aud_vsb.h"
#include "drv_dev.h"

#include "mp3_player_filter.h"
#include "mtos_misc.h"
#include "err_check_def.h"

#define MAX_ERROR_DATA_TIMEOUT (2000)

static mp3_dec_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((mp3_dec_pin_t *)_this)->private_data;
}

static RET_CODE mp3_dec_pin_on_open(handle_t _this)
{
  mp3_dec_pin_private_t *p_priv = get_priv(_this);
  
  p_priv->p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  CHECK_FAIL_RET_CODE(p_priv->p_audio_dev != NULL);
  p_priv->found_last_sample = FALSE;
  p_priv->error_data_timeout = 0;
  return SUCCESS;
}

static RET_CODE mp3_dec_pin_on_close(handle_t _this)
{
  return SUCCESS;
}


static void mp3_dec_pin_receive(handle_t _this, media_sample_t *p_sample)
{
  sink_pin_t *p_sink_pin = (sink_pin_t *)_this;

  p_sink_pin->async_render(p_sink_pin, p_sample, 0);
}

static BOOL vsb_idle(handle_t _this, media_sample_t *p_sample, u32 context)
{
  mp3_dec_pin_private_t *p_priv = get_priv(_this);
  u32 buf_len = 0;

  if(p_priv->found_last_sample)
  {
    BOOL bFinish = FALSE;

    p_priv->error_data_timeout ++;
    aud_file_getesbufferinfo_vsb(p_priv->p_audio_dev, &bFinish);
    if(bFinish || (p_priv->error_data_timeout > MAX_ERROR_DATA_TIMEOUT))
    {
        ipin_t *p_ipin = (ipin_t *)_this;
        os_msg_t msg = {0};
              
        msg.content = AUDIO_FILE_PLAY_END;
        p_ipin->send_message_out(p_ipin, &msg);
        p_priv->found_last_sample = FALSE;  //reset it
        
        OS_PRINTF("\n##AUDIO_FILE_PLAY_END[bFinish:%d, timeout:%d]!\n",
          bFinish, p_priv->error_data_timeout);
    }
    return FALSE;
  }
  else
  {
    aud_file_getleftesbuffer_vsb(p_priv->p_audio_dev, &buf_len);
    
    if (p_sample->payload <= buf_len)
    {
      return TRUE;
    }
    
    return FALSE;
  }
  
  //return TRUE;
  
}

static BOOL render_sample_to_vsb(handle_t _this, media_sample_t *p_sample, u32 context)
{
  mp3_dec_pin_private_t *p_priv = get_priv(_this);

  aud_file_pushesbuffer_vsb(p_priv->p_audio_dev, (u32)p_sample->p_data, p_sample->payload,0);
  if (p_sample->state == SAMP_STATE_INSUFFICIENT)
  {
    OS_PRINTF("\n##debug: Flound last sample ~~payload %d~~~~!\n", p_sample->payload);
    p_priv->found_last_sample = TRUE;
  }
  
  return TRUE;
}

static BOOL mp3_dec_pin_notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

mp3_dec_pin_t * mp3_dec_pin_create(mp3_dec_pin_t *p_pin, interface_t *p_owner)
{
  mp3_dec_pin_private_t *p_priv = NULL;
  mp3_dec_pin_t *p_ins = p_pin;
  //interface_t *p_interface = NULL;
  base_input_pin_t *p_input_pin = NULL;
  sink_pin_para_t sink_pin_para = {0};
  ipin_t *p_ipin = NULL;
  sink_pin_t *p_sink_pin = NULL;
  media_format_t format = {MT_AUDIO};

  //check input parameter
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);

  
  //create base class
  sink_pin_para.p_filter = p_owner;
  sink_pin_para.p_name = "mp3_decode_pin";
  sink_pin_create(&p_ins->base_pin, &sink_pin_para);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(mp3_dec_pin_private_t));
  p_priv->p_this = p_ins; //this point

  //init member function
  p_sink_pin = (sink_pin_t *)p_ins;
  p_sink_pin->hw_ready = vsb_idle;
  p_sink_pin->render_sample = render_sample_to_vsb;
  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_ins;
  p_input_pin->on_receive = mp3_dec_pin_receive;
  p_input_pin->notify_allocator = mp3_dec_pin_notify_allocator;

  p_ipin = (ipin_t *)p_ins;
  p_ipin->on_open = mp3_dec_pin_on_open;
  p_ipin->on_close = mp3_dec_pin_on_close;
  p_ipin->add_supported_media_format(p_ipin, &format);
  
  return p_ins;
}

