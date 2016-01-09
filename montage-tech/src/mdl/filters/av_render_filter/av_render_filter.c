/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <math.h>
#include  "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "hal_misc.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "common.h"
#include "drv_dev.h"
#include "dmx.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "drv_misc.h"
#include "scart.h"
#include "rf.h"
#include "nim.h"
#include "avsync.h"
#include "lib_rect.h"
#include "display.h"

#include "lib_util.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"
#include "ts_sequence.h"
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

//filter
#include "eva_filter_factory.h"
#include "av_render_filter.h"
#include "av_render_pin_intra.h"
#include "av_render_filter_intra.h"
#include "err_check_def.h"

static av_render_filter_private_t *get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((av_render_filter_t *)_this)->private_data;
}

static RET_CODE on_command(handle_t _this, icmd_t *p_cmd)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  av_render_config_t *p_cfg = NULL;

  switch (p_cmd->cmd)
  {
  case AV_RENDER_CFG:
    p_cfg = (av_render_config_t *)p_cmd->lpara;
    memcpy(&p_priv->cfg, p_cfg, sizeof(av_render_config_t));
    break;
    
  default:
    return ERR_FAILURE;
  }
  return SUCCESS;
}

extern RET_CODE vdec_switch_ch_stop(void *p_dev);
static RET_CODE _stop_av(av_render_filter_private_t *p_priv, BOOL b_freeze_stop)
{
  dmx_device_t *p_dmx_dev = NULL;
  void *p_video_dev = NULL;
  void *p_audio_dev = NULL;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif
  RET_CODE ret = SUCCESS;

  p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_CODE(p_dmx_dev != NULL);
  p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  CHECK_FAIL_RET_CODE(p_video_dev != NULL);
  p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  CHECK_FAIL_RET_CODE(p_audio_dev != NULL);

  if (!b_freeze_stop)
  {
    ret = vdec_stop(p_video_dev);
  }
  else
  {
    if(chip_ic_id == IC_CONCERTO)
      ret = vdec_switch_ch_stop(p_video_dev);
    else
      ret = vdec_freeze_stop(p_video_dev);
    OS_PRINTF("\n##vdec_freeze_stop!!!");
  }
  ret |= aud_stop_vsb(p_audio_dev);
  
  if (p_priv->audio_chain != 0xFFFF)
  {
    ret |= dmx_chan_stop(p_dmx_dev, p_priv->audio_chain);
    ret |= dmx_chan_close(p_dmx_dev, p_priv->audio_chain);
  }
  p_priv->audio_chain = 0xFFFF;

  if (p_priv->pcr_chain != 0xFFFF)
  {
    ret |= dmx_chan_stop(p_dmx_dev, p_priv->pcr_chain);
    ret |= dmx_chan_close(p_dmx_dev, p_priv->pcr_chain);
  }
  p_priv->pcr_chain = 0xFFFF;

  if (p_priv->video_chain != 0xFFFF)
  {
    ret |= dmx_chan_stop(p_dmx_dev, p_priv->video_chain);
    ret |= dmx_chan_close(p_dmx_dev, p_priv->video_chain);
  }
  p_priv->video_chain = 0xFFFF;

  ret |= dmx_av_reset(p_dmx_dev);
  
  return SUCCESS;
}

static RET_CODE _start_av(av_render_filter_private_t *p_priv,
  BOOL b_need_resume, u32 is_trick_no_audio)
{
  RET_CODE ret = SUCCESS;
  dmx_play_setting_t  play_para;
  dmx_device_t *p_dmx_dev = NULL;
  void *p_video_dev = NULL;
  void *p_audio_dev = NULL;
  trick_mode_t trick_mode = 0;
  u8 times = 0;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif

  p_priv->b_pause = FALSE;

  p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_CODE(p_dmx_dev != NULL);
  p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  CHECK_FAIL_RET_CODE(p_video_dev != NULL);
  p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  CHECK_FAIL_RET_CODE(p_audio_dev != NULL);

  if (!is_trick_no_audio)
  {
    play_para.pid = p_priv->cfg.audio_pid;
    play_para.type = DMX_AUDIO_TYPE;
    play_para.stream_in = DMX_INPUT_EXTERN_DMA;
    ret = dmx_play_chan_open(p_dmx_dev, &play_para, &(p_priv->audio_chain));
    if(SUCCESS != ret)
    {
      p_priv->audio_chain = 0xFFFF; 
    }
    CHECK_FAIL_RET_CODE(SUCCESS == ret);
    ret = dmx_chan_start(p_dmx_dev, p_priv->audio_chain);
    CHECK_FAIL_RET_CODE(SUCCESS == ret);
  }
  
   play_para.pid = p_priv->cfg.video_pid;
   play_para.type = DMX_VIDEO_TYPE;
   play_para.stream_in = DMX_INPUT_EXTERN_DMA;
   ret = dmx_play_chan_open(p_dmx_dev, &play_para, &(p_priv->video_chain));
   if(SUCCESS != ret)
   {
     p_priv->video_chain = 0xFFFF; 
   }
  CHECK_FAIL_RET_CODE(SUCCESS == ret);
  ret = dmx_chan_start(p_dmx_dev, p_priv->video_chain);
  CHECK_FAIL_RET_CODE(SUCCESS == ret);

   play_para.pid = p_priv->cfg.pcr_pid;
   play_para.type = DMX_PCR_TYPE;
   play_para.stream_in = DMX_INPUT_EXTERN_DMA;
   ret = dmx_play_chan_open(p_dmx_dev, &play_para, &(p_priv->pcr_chain));
   if(SUCCESS != ret)
   {
     p_priv->pcr_chain = 0xFFFF; 
   }
  CHECK_FAIL_RET_CODE(SUCCESS == ret);
  if(chip_ic_id == IC_CONCERTO)
  {
    u32 cons_ptkInterval = 0x88;
    u32 ptkMode  = (cons_ptkInterval <<16 ) | (0x10 << 0);
    *((volatile u32*)(0xBF22201c)) = ptkMode;
  }
  ret = dmx_chan_start(p_dmx_dev, p_priv->pcr_chain);
  CHECK_FAIL_RET_CODE(SUCCESS == ret);
  
  if (!is_trick_no_audio)
  {    
    switch (p_priv->cfg.audio_fmt)
    {
    case AUDIO_PCM:
    case AUDIO_MP1:
    case AUDIO_MP2:
    case AUDIO_MP3:
    case AUDIO_AC3_VSB:
    case AUDIO_EAC3:
    case AUDIO_AAC:
    case AUDIO_SPDIF:
    case AUDIO_DOLBY_CONVERT:
    case AUDIO_SPDIF_AC3:
    case AUDIO_SPDIF_EAC3:
      break;
    default:
      OS_PRINTF("\n##unsupport audio fmt[%d]!\n", p_priv->cfg.audio_fmt);
      return ERR_FAILURE;
    }
    ret = aud_start_vsb(p_audio_dev, p_priv->cfg.audio_fmt, AUDIO_TS_FILE);
    CHECK_FAIL_RET_CODE(ret == SUCCESS);
  }

  {
   if(chip_ic_id != IC_CONCERTO)
    {
      ret |= vdec_set_data_input(p_video_dev, TRUE);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
   if(chip_ic_id == IC_CONCERTO)
    {
      ret |= vdec_set_avsync_mode(p_video_dev, VDEC_AVSYNC_PVR);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
    ret |=  vdec_start(p_video_dev, p_priv->cfg.video_fmt, VID_UNBLANK_USER);
    CHECK_FAIL_RET_CODE(ret == SUCCESS);
  }

  if (b_need_resume)
  {
    vdec_resume(p_video_dev);
  }
  
  switch (p_priv->play_mode)
  {
  case TS_SEQ_FAST_PLAY_2X:
  case TS_SEQ_FAST_PLAY_4X:
  case TS_SEQ_FAST_PLAY_8X:
  case TS_SEQ_FAST_PLAY_16X:
  case TS_SEQ_FAST_PLAY_32X:
    trick_mode = VDEC_TM_FFWD;
    times = pow(2, p_priv->play_mode + 1 - TS_SEQ_FAST_PLAY_2X);
    break;
    
  case TS_SEQ_REV_FAST_PLAY_2X:
  case TS_SEQ_REV_FAST_PLAY_4X:
  case TS_SEQ_REV_FAST_PLAY_8X:
  case TS_SEQ_REV_FAST_PLAY_16X:
  case TS_SEQ_REV_FAST_PLAY_32X:
    trick_mode = VDEC_TM_FREV;
    times = pow(2, p_priv->play_mode + 1 - TS_SEQ_REV_FAST_PLAY_2X);
    break;
    
  case TS_SEQ_SLOW_PLAY_2X:
  case TS_SEQ_SLOW_PLAY_4X:
    trick_mode = VDEC_TM_SFWD;
    times = pow(2, p_priv->play_mode + 1 - TS_SEQ_SLOW_PLAY_2X);
    break;
    
  case TS_SEQ_NORMAL_PLAY:
  default:
    trick_mode = VDEC_TM_NORMAL;
    break;
  }
  
  vdec_set_trick_mode(p_video_dev, trick_mode, times);
  return SUCCESS;
}

static void av_render_change_pid(handle_t _this, dynamic_rec_info_t *p_rec_info)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  u32 is_trick_no_audio = 0;
  
  if (p_priv->audio_chain == 0xFFFF)
  {
    is_trick_no_audio = 1;
  }
  
  _stop_av(p_priv, TRUE);
  p_priv->cfg.video_pid = p_rec_info->v_pid;
  p_priv->cfg.audio_pid = p_rec_info->a_pid;
  p_priv->cfg.pcr_pid = p_rec_info->pcr_pid;
  p_priv->cfg.video_fmt = p_rec_info->video_fmt;
  p_priv->cfg.audio_fmt = p_rec_info->audio_fmt;

  _start_av(p_priv, TRUE, is_trick_no_audio);
}

static void av_render_reset(handle_t _this)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  u32 is_trick_no_audio = 0;

  if (p_priv->audio_chain == 0xFFFF)
  {
    is_trick_no_audio = 1;
  }
  
  _stop_av(p_priv, TRUE);
  _start_av(p_priv, TRUE, is_trick_no_audio);
}

static u32 av_render_check_audio(handle_t _this, u8 *p_data, u32 payload)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  u8 *p_packet = p_data;
  u32 audio_len = 0;
  u32 loopi = 0;
  
  for (loopi = 0; loopi < payload; loopi += 188)
  {
    if (TS_PID(p_packet) == p_priv->cfg.audio_pid)
    {
      audio_len += 188;
    }
    p_packet += 188;
  }
  
  return audio_len;
}

void av_render_trick_mode(handle_t _this, u32 is_normal)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  RET_CODE ret = SUCCESS;
  dmx_play_setting_t  play_para;
  dmx_device_t *p_dmx_dev = NULL;
  void *p_audio_dev = NULL;

  p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_VOID(p_dmx_dev != NULL);
  p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  CHECK_FAIL_RET_VOID(p_audio_dev != NULL);

  if (is_normal)
  {
    OS_PRINTF("\n##debug: av render start audio!!\n");
    play_para.pid = p_priv->cfg.audio_pid;
    play_para.type = DMX_AUDIO_TYPE;
    play_para.stream_in = DMX_INPUT_EXTERN_DMA;
    ret = dmx_play_chan_open(p_dmx_dev, &play_para, &(p_priv->audio_chain));
    if(ret != SUCCESS)
    {
      p_priv->audio_chain = 0xFFFF;
    }
    CHECK_FAIL_RET_VOID(SUCCESS == ret);
    ret = dmx_chan_start(p_dmx_dev, p_priv->audio_chain);
    CHECK_FAIL_RET_VOID(SUCCESS == ret);
    {    
      ret = aud_start_vsb(p_audio_dev, p_priv->cfg.audio_fmt, AUDIO_TS_FILE);
      CHECK_FAIL_RET_VOID(ret == SUCCESS);
    }
  }
  else
  {
    OS_PRINTF("\n##debug: av render stop audio!!\n");
    aud_stop_vsb(p_audio_dev);
    if (p_priv->audio_chain != 0xFFFF)
    {
      ret |= dmx_chan_stop(p_dmx_dev, p_priv->audio_chain);
      ret |= dmx_chan_close(p_dmx_dev, p_priv->audio_chain);
      p_priv->audio_chain = 0xFFFF;
    }
  }
}

void av_render_av_reset(handle_t _this)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  sink_pin_t *p_sink_pin = (sink_pin_t *)(&p_priv->m_pin);
  dmx_device_t *p_dmx_dev = NULL;
  
  p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_VOID(p_dmx_dev != NULL);
  
  OS_PRINTF("\n##debug: av render trick mode reset!!\n");
  p_sink_pin->flush_async_render(p_sink_pin);
  dmx_av_reset(p_dmx_dev);
}

void av_render_set_play_mode(handle_t _this, u8 play_mode)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  
  p_priv->play_mode = play_mode;
}

static RET_CODE on_open(handle_t _this)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  
  p_priv->audio_chain = 0xFFFF;
  p_priv->pcr_chain = 0xFFFF;
  p_priv->video_chain = 0xFFFF;
  p_priv->play_mode = TS_SEQ_NORMAL_PLAY;
  return SUCCESS;
}

static RET_CODE on_start(handle_t _this)
{
  av_render_filter_private_t *p_priv = get_priv(_this);

  return _start_av(p_priv, FALSE, 0);
}

static RET_CODE on_pause(handle_t _this)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  RET_CODE ret = SUCCESS;
  void *p_video_dev = NULL;
  void *p_audio_dev = NULL;
  
  if (!p_priv->b_pause)
  {
    p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
    CHECK_FAIL_RET_CODE(p_video_dev != NULL);
    p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    CHECK_FAIL_RET_CODE(p_audio_dev != NULL);
    OS_PRINTF("\n##debug: av pause!!\n");
    ret =  vdec_pause(p_video_dev);
    ret |= aud_pause_vsb(p_audio_dev);

    p_priv->b_pause = TRUE;
  }
  return ret;
}

static RET_CODE on_resume(handle_t _this)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  RET_CODE ret = SUCCESS;
  void *p_video_dev = NULL;
  void *p_audio_dev = NULL;

  if(p_priv->b_pause)
  {
    p_priv->b_pause = FALSE;
    p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
    CHECK_FAIL_RET_CODE(p_video_dev != NULL);
    p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    CHECK_FAIL_RET_CODE(p_audio_dev != NULL);
    OS_PRINTF("\n##debug: av resume!!\n");
    ret =  vdec_resume(p_video_dev);
    ret |= aud_resume_vsb(p_audio_dev);
  }
  
  return ret;
}

static RET_CODE on_stop(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  av_render_filter_private_t *p_priv = get_priv(_this);
  
  _stop_av(p_priv, FALSE);
  return SUCCESS;
}

static void on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t *av_render_filter_create(void *p_para)
{
  av_render_filter_private_t *p_priv = NULL;
  av_render_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;

  //create filter
  p_ins = mtos_malloc(sizeof(av_render_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(av_render_filter_t));

  //create base class
  sink_filter_create(&p_ins->_sink_filter, NULL);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(av_render_filter_private_t));
  p_priv->p_this = p_ins;
  p_ins->av_render_change_pid = av_render_change_pid;
  p_ins->av_render_check_audio = av_render_check_audio;
  p_ins->av_render_reset = av_render_reset;
  
  //overload virtual interface
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = on_command;
  p_ifilter->on_open  = on_open;
  p_ifilter->on_start = on_start;
  p_ifilter->on_pause = on_pause;
  p_ifilter->on_resume = on_resume; //resume use async mode
  p_ifilter->on_stop = on_stop;
  p_ifilter->on_close = on_close;
  p_ifilter->_interface.on_destroy = on_destory;
  
  //create child pin
  av_render_pin_create(&p_priv->m_pin, (interface_t *)p_ins,p_para);

  return (ifilter_t *)p_ins;
}

//end of file

