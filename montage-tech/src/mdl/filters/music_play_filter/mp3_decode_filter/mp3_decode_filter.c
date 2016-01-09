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
#include "hal_misc.h"

#include "common.h"
#include "drv_dev.h"
#include "scart.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "rf.h"
#include "lib_rect.h"
#include "display.h"
#include "dmx.h"
#include "avctrl1.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"
#include "mp3_decode_pin_intra.h"

#include "ifilter.h"
#include "sink_filter.h"
#include "mp3_decode_filter_intra.h"
#include "err_check_def.h"

/*static mp3_dec_filter_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return (mp3_dec_filter_private_t *)((mp3_dec_filter_t *)_this)->private_data;
}*/

static RET_CODE mp3_dec_on_command(handle_t _this, icmd_t *p_cmd)
{
#if 0  
  switch (p_cmd->cmd)
  {
    default:
      return ERR_FAILURE;
  }
  return SUCCESS;
#endif
  return ERR_FAILURE;
}

static BOOL audio_start(audio_param_t *p_audio, av_start_type_t type) 
{
  RET_CODE ret = SUCCESS;
  void *p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);

  if(type == AV_AUDIO_START)
  {
     ret = aud_start_vsb(p_audio_dev, p_audio->type, p_audio->file_type);
  }

  return ret == SUCCESS ? TRUE : FALSE;
}

static BOOL audio_stop()
{
  RET_CODE ret = SUCCESS;
  void *p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);

  ret = aud_stop_vsb(p_audio_dev);
  
  return ret == SUCCESS ? TRUE : FALSE;

}

static BOOL audio_pause()
{
  RET_CODE ret = SUCCESS;
  void *p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);

  ret = aud_pause_vsb(p_audio_dev);

  return ret == SUCCESS ? TRUE : FALSE;
}

static BOOL audio_resume()
{
  RET_CODE ret = SUCCESS;
  void *p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);

  ret = aud_resume_vsb(p_audio_dev);
  
  return ret == SUCCESS ? TRUE : FALSE;
}

/*!
  mp3 start
  */
static RET_CODE mp3_on_start(handle_t _this)
{
  audio_param_t audio_cfg = {0};
  RET_CODE ret = SUCCESS;

  audio_cfg.type = AUDIO_MP3;
  audio_cfg.file_type = AUDIO_ES_FILE;
  ret = audio_start(&audio_cfg, AV_AUDIO_START);
  CHECK_FAIL_RET_CODE(TRUE == ret);

  return SUCCESS;
}

static RET_CODE mp3_on_stop(handle_t _this)
{
  RET_CODE ret = SUCCESS;
  dmx_device_t *p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                           SYS_DEV_TYPE_PTI);
  ret = audio_stop();
  CHECK_FAIL_RET_CODE(TRUE == ret);

  dmx_av_reset(p_dmx_dev);

  return SUCCESS;
}

static RET_CODE mp3_on_pause(handle_t _this)
{
  audio_pause();

  return SUCCESS;
}

static RET_CODE mp3_on_resume(handle_t _this)
{
  audio_resume();

  return SUCCESS;
}

static void mp3_on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t * mp3_decode_filter_create(void *p_para)
{
  mp3_dec_filter_private_t *p_priv = NULL;
  mp3_dec_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;

  //create filter
  p_ins = mtos_malloc(sizeof(mp3_dec_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(mp3_dec_filter_t));


  //create base class
  sink_filter_create(&p_ins->_sink_filter, NULL);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(mp3_dec_filter_private_t));
  p_priv->p_this = p_ins;

  //init member function

  //overload virtual interface
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = mp3_dec_on_command;
  p_ifilter->on_start = mp3_on_start;
  p_ifilter->on_stop = mp3_on_stop;
  p_ifilter->on_pause = mp3_on_pause;
  p_ifilter->on_resume = mp3_on_resume;
  p_ifilter->_interface.on_destroy = mp3_on_destory;  

  //create child pin
  mp3_dec_pin_create(&p_priv->m_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}

