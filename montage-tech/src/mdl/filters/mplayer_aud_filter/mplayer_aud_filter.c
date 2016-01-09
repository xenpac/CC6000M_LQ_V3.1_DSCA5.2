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
#include "mtos_misc.h"

#include "lib_util.h"
#include "mt_time.h"
#include "vfs.h"
#include "lib_unicode.h"
//util
#include "class_factory.h"
#include "simple_queue.h"

#include "data_manager.h"
#include "file_playback_sequence.h"
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
#include "transf_output_pin.h"
#include "source_pin.h"
#include "ifilter.h"
#include "src_filter.h"
#include "transf_filter.h"
#include "eva_filter_factory.h"

#include "mplayer_aud_input_pin_intra.h"
#include "mplayer_aud_lrc_output_pin_intra.h"
#include "mplayer_aud_pic_output_pin_intra.h"
#include "mp3_decode_frame.h"

#include "mplayer_aud_filter.h"
#include "mplayer_aud_filter_intra.h"

#include "aud_vsb.h"
#include "drv_dev.h"
#include "err_check_def.h"

static mplayer_aud_filter_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((mplayer_aud_filter_t *)_this)->private_data;
}

static RET_CODE _push_logo_data(mplayer_aud_filter_private_t *p_priv)
{
  transf_output_pin_t *p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_pic_out_pin;
  media_sample_t *p_out_sample = NULL;
  
  if (!p_priv->b_music_logo_show)
  {
    return SUCCESS;
  }

  p_transf_out_pin->query_sample(p_transf_out_pin, &p_out_sample);
  if (p_out_sample == NULL)
  {
    return ERR_FAILURE;
  }

  if (p_priv->aud_info.p_pic_buf != NULL)
  {
    p_out_sample->p_data = p_priv->aud_info.p_pic_buf;
    p_out_sample->payload = p_priv->aud_info.pic_buf_len;
  }

  if((p_out_sample->p_data != NULL) && (p_out_sample->payload > 0))
  {
    p_out_sample->state = SAMP_STATE_INSUFFICIENT;

    if(p_transf_out_pin->push_down(p_transf_out_pin, p_out_sample) != SUCCESS)
    {
      p_transf_out_pin->release_sample(p_transf_out_pin, p_out_sample);
    }
  }
  else
  {
    p_transf_out_pin->release_sample(p_transf_out_pin, p_out_sample);
  }
  
  return SUCCESS;
}

static void _push_lrc_data(mplayer_aud_filter_private_t *p_priv)
{
  transf_output_pin_t *p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_lrc_out_pin;
  media_sample_t *p_out_sample = NULL;
  ipin_t *p_ipin = (ipin_t *)p_transf_out_pin;
  
  if (!p_ipin->is_connected(p_ipin))
  {
    return ;
  }
  
  if (p_priv->first_frame)
  {
    p_transf_out_pin->query_sample(p_transf_out_pin, &p_out_sample);
    if(p_out_sample == NULL)
    {
      return ;
    }
    
    p_out_sample->p_data = p_priv->aud_info.p_lrc_data;
    p_out_sample->payload = p_priv->aud_info.lrc_data_len;
    p_out_sample->data_offset = 0;
    p_out_sample->format.stream_type = MT_FILE_DATA;

    if(p_transf_out_pin->push_down(p_transf_out_pin, p_out_sample) != SUCCESS)
    {
       p_transf_out_pin->release_sample(p_transf_out_pin, p_out_sample);
    }
    p_priv->first_frame = 0;
    return ;
  }
   
   if(p_priv->lrc_play_time != p_priv->cur_play_time)
   {
    p_priv->lrc_play_time = p_priv->cur_play_time;

    if(p_priv->b_music_lrc_show)
    {
      p_transf_out_pin->query_sample(p_transf_out_pin, &p_out_sample);
      if(p_out_sample == NULL)
      {
        return ;
      }

      p_out_sample->data_offset = p_priv->lrc_play_time;
      p_out_sample->format.stream_type = MT_AUDIO;
      if(p_transf_out_pin->push_down(p_transf_out_pin, p_out_sample) != SUCCESS)
      {
         p_transf_out_pin->release_sample(p_transf_out_pin, p_out_sample);
      }
    }
   }
}

static void _mplayer_aud_loop_time(handle_t _this)
{
  mplayer_aud_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)p_priv->p_this;
  FILM_INFO_T file_info = {0};
  os_msg_t msg = {0};

  if (p_priv->b_load_meida_success)
  {
    p_priv->b_load_meida_success = 0;
    p_priv->p_handle->get_film_info(p_priv->p_handle, &file_info);
    p_priv->aud_info.file_size = file_info.file_size;
    p_priv->aud_info.duration = file_info.film_duration;

    msg.content = MPLAYER_AUD_PLAY_START;
    msg.para1 = (u32)&p_priv->aud_info;
    p_ifilter->send_message_out(p_ifilter, &msg);
    
    p_priv->p_handle->start(p_priv->p_handle, 0);
  }

  _push_lrc_data(p_priv);
}

static RET_CODE _on_command(handle_t _this, icmd_t *p_cmd)
{
  mplayer_aud_filter_private_t *p_priv = NULL;

  p_priv = get_priv(_this);
  
  switch (p_cmd->cmd)
  {
  case MPLAYER_AUD_CFG_NAME:
    p_priv->p_file_name = p_cmd->p_para;
    break;
  case MPLAYER_AUD_CFG_HEAD_INFO:
    break;
  case MPLAYER_AUD_PUSH_LOGO:
    _push_logo_data(p_priv);
    break;
  case MPLAYER_AUD_TIME_SEEK:
    p_priv->timejump = p_cmd->lpara;
    break;
  case MPLAYER_AUD_CFG_LRC:
    p_priv->aud_info.p_lrc_data = p_cmd->p_para;
    p_priv->aud_info.lrc_data_len = p_cmd->lpara;
    break;
  case MPLAYER_AUD_CFG_LOGO:
    p_priv->aud_info.p_pic_buf = p_cmd->p_para;
    p_priv->aud_info.pic_buf_len = p_cmd->lpara;
    break;
  case MPLAYER_AUD_SHOW_LOGO:
    p_priv->b_music_logo_show = p_cmd->lpara;
    break;
  case MPLAYER_AUD_SHOW_LRC:
    p_priv->b_music_lrc_show = p_cmd->lpara;
    break;
  
  case MPLAYER_AUD_CFG_URL:
    p_priv->p_src_url = p_cmd->p_para;
    break;
  
  default:
    return ERR_FAILURE;
  }
  return SUCCESS;
}

static RET_CODE _on_open(handle_t _this)
{
  mplayer_aud_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)_this;
  
  p_priv->first_frame = TRUE;
  p_priv->lrc_play_time = 0;
  p_priv->cur_play_time = 0;
  p_priv->b_load_meida_success = 0;
  p_ifilter->set_active_enter(p_ifilter, _mplayer_aud_loop_time);

  return SUCCESS;
}

static void _uni2utf8(u16 *in, u8 *out, u16 out_size)
{
  u32 i   = 0;
  u32 cnt = 0;
  u16 unicode = 0;

  cnt = uni_strlen(in);

  CHECK_FAIL_RET_VOID(out_size >= (3 * cnt + 1));

  for (i = 0; i < uni_strlen(in); i++)
  {
    unicode = in[i];

    if (unicode >= 0x0000 && unicode <= 0x007f)
    {
      *out = (u8)unicode;
      out += 1;
    }
    else if (unicode >= 0x0080 && unicode <= 0x07ff)
    {
      *out = 0xc0 | (unicode >> 6);
      out += 1;
      *out = 0x80 | (unicode & 0x003f);
      out += 1;
    }
    else if (unicode >= 0x0800 && unicode <= 0xffff)
    {
      *out = 0xe0 | (unicode >> 12);
      out += 1;
      *out = 0x80 | (unicode >> 6 & 0x003f);
      out += 1;
      *out = 0x80 | (unicode & 0x003f);
      out += 1;
    }
  }

  *out = '\0';
}

static RET_CODE _on_start(handle_t _this)
{
  mplayer_aud_filter_private_t *p_priv = get_priv(_this);
  u8 file_name[MAX_FILE_PATH * 3 + 1] = {0};

  if (p_priv->p_src_url == NULL)
  {
    _uni2utf8(p_priv->p_file_name, file_name, MAX_FILE_PATH * 3 + 1);

    p_priv->p_handle->set_file_path_ex(p_priv->p_handle, file_name, 0);
  }
  else
  {
    p_priv->p_handle->set_file_path_ex(p_priv->p_handle, p_priv->p_src_url, 0);
  }
  p_priv->p_handle->loadmedia_task(p_priv->p_handle);

  return SUCCESS;
}

static RET_CODE _on_pause(handle_t _this)
{
  mplayer_aud_filter_private_t *p_priv = get_priv(_this);
  
  p_priv->p_handle->pause(p_priv->p_handle);
  return SUCCESS;
}

static RET_CODE _on_resume(handle_t _this)
{
  mplayer_aud_filter_private_t *p_priv = get_priv(_this);
  
  p_priv->p_handle->resume(p_priv->p_handle);
  return SUCCESS;
}

static RET_CODE _on_stop(handle_t _this)
{
  mplayer_aud_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)p_priv->p_this;
  
  p_ifilter->set_active_enter(p_ifilter, NULL);

  OS_PRINTF("\n##_on_stop1[%lu, %lu] !", p_priv->cur_play_time, p_priv->aud_info.duration);
  if (p_priv->cur_play_time == p_priv->aud_info.duration)
  {
    p_priv->p_handle->stop(p_priv->p_handle);
  }
  else
  {
    p_priv->p_handle->force_stop(p_priv->p_handle);
  }
  OS_PRINTF("\n##_on_stop 2!");

  return SUCCESS;
}

static RET_CODE _on_close(handle_t _this)
{  
  return SUCCESS;
}

static RET_CODE _on_evt(handle_t _this, os_msg_t *p_msg)
{
  mplayer_aud_filter_private_t *p_priv = get_priv(_this);
  
  switch(p_msg->content)
  {
  case MPLAYER_AUD_PLAY_EVENT_EOS:
    break;
  case MPLAYER_AUD_PLAY_EVENT_UP_RESOLUTION:
    break;
  case MPLAYER_AUD_PLAY_EVENT_UP_TIME:
    p_priv->cur_play_time = p_msg->para1 / 1000;
    return SUCCESS;
  case MPLAYER_AUD_PLAY_EVENT_UNSUPPORTED_VIDEO:
    break;
  case MPLAYER_AUD_PLAY_EVENT_UNSUPPORTED_AUDIO:
    break;
  case MPLAYER_AUD_PLAY_EVENT_TRICK_TO_BEGIN:
    break;
  case MPLAYER_AUD_PLAY_EVENT_UNSUPPORT_MEMORY:
    break;
  case MPLAYER_AUD_PLAY_EVENT_NEW_SUB_DATA_RECEIVE:
    break;
  case MPLAYER_AUD_PLAY_EVENT_UNSUPPORT_SEEK:
    break;
  case MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_EXIT:
    break;
  case MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_ERROR:
    break;
  case MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_SUCCESS:
    p_priv->b_load_meida_success = 1;
    break;
  case MPLAYER_AUD_PLAY_EVENT_FILE_ES_TASK_EXIT:
    break;
  case MPLAYER_AUD_PLAY_EVENT_START_BUFFERING:
    break;
  case MPLAYER_AUD_PLAY_EVENT_FINISH_BUFFERING:
    break;
  case MPLAYER_AUD_PLAY_EVENT_UPDATE_BPS:
    break;
  case MPLAYER_AUD_PLAY_EVENT_REQUEST_CHANGE_SRC:
    break;
  case MPLAYER_AUD_PLAY_EVENT_SET_PATH_FAIL:
    break;
  case MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_TIME:
    break;
  case MPLAYER_AUD_PLAY_EVENT_UNSUPPORTED_HD_VIDEO:
    break;
  default:
    break;
  }

  return SUCCESS;
}

static void _on_destory(handle_t _this)
{
#ifndef WIN32
  file_seq_destroy();
#endif
  mtos_free((void *)_this);
}

static FILE_SEQ_T * _mplayer_aud_handle_create(mplayer_aud_filter_para_t *p_para)
{
  PB_SEQ_PARAM_T pb_seq_param = {0};
  FILE_SEQ_T *p_handle = NULL;
  /*!
   * Create file seq handle
   */
  #ifndef WIN32
  pb_seq_param.pb_seq_mem_size = p_para->v_mem_size;
  pb_seq_param.pb_seq_mem_start = p_para->v_mem_start;
  pb_seq_param.stack_size = p_para->fs_task_size;
  pb_seq_param.task_priority = p_para->fs_task_prio;
  pb_seq_param.stack_start = p_para->fs_stack_addr;
  pb_seq_param.audio_output_mode = p_para->audio_output;
  pb_seq_param.vdec_policy = p_para->vdec_policy;
  pb_seq_param.preload_audio_buffer_size = p_para->preload_audio_buffer_size;
  pb_seq_param.preload_audio_buffer = p_para->p_preload_audio_buf;
  pb_seq_param.preload_video_buffer_size = p_para->preload_video_buffer_size;
  pb_seq_param.preload_video_buffer = p_para->p_preload_video_buf;
  pb_seq_param.stack_preload_size = p_para->stack_preload_size;
  pb_seq_param.task_preload_priority = p_para->task_preload_priority;
  pb_seq_param.is_direct_url = p_para->direct_url_chain;
  pb_seq_param.camera_open = NULL;
  pb_seq_param.camera_close = NULL;
  pb_seq_param.camera_read = NULL;
  pb_seq_param.net_task_priority = p_para->net_task_priority;
  
  p_handle = (FILE_SEQ_T *)file_seq_create(&pb_seq_param);

  file_seq_set_fw_mem(p_para->vdec_start, 
                      p_para->vdec_size, 
                      p_para->audio_start, 
                      p_para->audio_size);
  
  CHECK_FAIL_RET_NULL(NULL != p_handle);

   p_handle->register_event_cb(p_handle, p_para->cb);
  #endif

  return p_handle;
}

ifilter_t * mplayer_aud_filter_create(void *p_para)
{
  mplayer_aud_filter_private_t *p_priv = NULL;
  mplayer_aud_filter_t *p_ins = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_para_t transf_filter_para = {0};
  
  transf_filter_para.dummy = 0;

  //create filter
  p_ins = mtos_malloc(sizeof(mplayer_aud_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(mplayer_aud_filter_t));

  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);
  
  //init private date
  p_priv = &p_ins->private_data;
  p_priv->p_this = p_ins; //this point
  p_priv->p_src_url = NULL;
  p_priv->timejump = 0;
  p_priv->b_music_logo_show = TRUE;
  p_priv->b_music_lrc_show = TRUE;
  //init member function
  memset(&p_priv->aud_info, 0, sizeof(mplayer_aud_info_t));
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = _on_command;
  p_ifilter->on_open = _on_open;
  p_ifilter->on_start = _on_start;
  p_ifilter->on_pause = _on_pause;
  p_ifilter->on_resume = _on_resume;
  p_ifilter->on_stop = _on_stop;
  p_ifilter->on_close = _on_close;
  p_ifilter->on_process_evt = _on_evt;
  p_ifilter->_interface.on_destroy = _on_destory;

  //create it's pin
  mplayer_aud_in_pin_create(&p_priv->m_in_pin, (interface_t *)p_ins);
  mplayer_aud_lrc_out_pin_create(&p_priv->m_lrc_out_pin, (interface_t *)p_ins);
  mplayer_aud_pic_out_pin_create(&p_priv->m_pic_out_pin, (interface_t *)p_ins);
  p_priv->p_handle = _mplayer_aud_handle_create((mplayer_aud_filter_para_t *)p_para);
  CHECK_FAIL_RET_NULL(p_priv->p_handle != NULL);
  return (ifilter_t *)p_ins;
}
