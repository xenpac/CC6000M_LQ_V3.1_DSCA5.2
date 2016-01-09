/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
//util
#include "class_factory.h"
#include "simple_queue.h"
#include "lib_util.h"
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
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"
#include "ts_sequence.h"


#include "vfs.h"

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
#include "eva_filter_factory.h"
#include "transf_filter.h"

#include "transf_input_pin.h"
#include "transf_output_pin.h"
#include "ts_player_in_pin_intra.h"
#include "ts_player_out_pin_intra.h"
#include "ts_player_filter.h"
#include "ts_player_filter_intra.h"
#include "av_render_filter.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
//CA advanced
#include "Hal_secure.h"
#endif
//#define PLAY_TIME_DEBUG 1
#define PLAY_TIME_TASK_ENABLE 1
//#define DUMP_TRICK_MODE_FILE 1

#ifdef DUMP_TRICK_MODE_FILE
static hfile_t test_file = NULL;
#endif

/*!
  max send speed
  */
#define MAX_SEND_SPEED (1024 * 1024 * 10)

/*!
  max push payload, must < 255k
  */
#define MAX_PUSH_PAYLOAD (1024 * 188)//(1024 * 188)

/*!
  REV_FAST_PLAY is 1 else 0
  */
#define SEARCH_DIR(x) ((((x) == TS_SEQ_REV_FAST_PLAY_2X)\
  || ((x) == TS_SEQ_REV_FAST_PLAY_4X)\
  || ((x) == TS_SEQ_REV_FAST_PLAY_8X)\
  || ((x) == TS_SEQ_REV_FAST_PLAY_16X)\
  || ((x) == TS_SEQ_REV_FAST_PLAY_32X)))

#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  
/*
  u8IV for CBC
*/
static u8 stCipherEngineInfo_u8IV[16];

extern int mul_pvr_get_cipher_key(u8 *p_engine, u8 **p_key, u8 *p_keylength);
#endif
static ts_player_filter_private_t *get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((ts_player_filter_t *)_this)->private_data;
}

static u32 get_play_mode_index(u32 times)
{
  u32 loopi = 0;
  u32 temp = times;

  while (temp > 1)
  {
    temp /= 2;
    loopi ++;
  }

  if (loopi)
  {
    loopi -= 1;
  }
  return loopi;
}

static u32 set_play_mode(ts_player_filter_private_t *p_priv,
  u32 play_mode, u32 times, u8 is_auto)
{
  vdec_info_t vdec_info;
  media_format_t  format = {0};
  s64 play_pos = 0;
  s32 pos_offset = 0;
  u8 cur_play_mode = 0;
  transf_output_pin_t *p_out_pin = NULL;
  ipin_t *p_ipin = NULL;
  ifilter_t *p_r_filter = NULL;
  dynamic_rec_info_t *p_dy_info = NULL;
#ifndef WIN32
  if (!p_priv->rec_info.vkey_frame_offset
    && p_priv->cfg.player_src != TS_PLAYER_SRC_REC_BLOCK)
  {
    OS_PRINTF("\n##old version, do not support FFX%d\n", times);
    return 0;
  }
#endif
  p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  p_ipin = (ipin_t *)(p_out_pin->m_pin._ipin.get_connected(p_out_pin));
  p_r_filter = (ifilter_t *)p_ipin->get_filter(p_ipin);

  p_priv->trick_mode_skip = 0;
  
  switch (play_mode)
  {
  case FAST_FORWARD:
    if (times <= 32)
    {
      cur_play_mode = TS_SEQ_FAST_PLAY_2X + get_play_mode_index(times);
      if ((p_priv->bit_rate >> 3) * times >= MAX_SEND_SPEED)
      {
        p_priv->trick_mode_skip = ((p_priv->bit_rate >> 3) * times / 14) / TS_STREAM_RW_UNIT;
      }
      
      OS_PRINTF("\n##debug: FF times[%lu][0x%x]skip data[%d]\n",
        times, p_priv->bit_rate, p_priv->trick_mode_skip);
    }
    else
    {
      OS_PRINTF("\n##debug: do not support FF times[%d]\n", times);
      return 0;
    }
    break;
  case FAST_BACKWARD:
    if (times <= 32)
    {
      cur_play_mode = TS_SEQ_REV_FAST_PLAY_2X + get_play_mode_index(times);
      if ((p_priv->bit_rate >> 3) * times >= MAX_SEND_SPEED)
      {
        p_priv->trick_mode_skip = ((p_priv->bit_rate >> 3) * times / 14) / TS_STREAM_RW_UNIT;
      }
      OS_PRINTF("\n##debug: FB times[%d][%d]skip data[%d]\n",
        times, p_priv->bit_rate, p_priv->trick_mode_skip);
    }
    else
    {
      OS_PRINTF("\n##debug: do not support FB times[%d]\n", times);
      return 0;
    }

    break;
  case SLOW_FORWARD:
    if (times <= 4)
    {
      cur_play_mode = TS_SEQ_SLOW_PLAY_2X + get_play_mode_index(times);
      OS_PRINTF("\n##debug: SF times[%d][%d]\n", times, cur_play_mode);
    }
    else
    {
      OS_PRINTF("\n##debug: do not support SF times[%d]\n", times);
      return 0;
    }
    break;
  case SLOW_BACKWARD:
    OS_PRINTF("\n##debug: do not support SB times[%d]\n", times);
    return 0;
    //p_priv->play_mode = TS_SEQ_REV_SLOW_PLAY_2X + get_play_mode_index(times);
    //OS_PRINTF("\n##debug: SB times[%d][%d]\n", times, p_priv->play_mode);
  case NORMAL_PLAY:
  default:
    cur_play_mode = TS_SEQ_NORMAL_PLAY;
    break;
  }

  if (p_priv->play_mode == cur_play_mode)
  {
    OS_PRINTF("\n##debug: set play mode same to cur mode!![%d]\n", cur_play_mode);
    return 0;
  }
  
  if (!p_priv->p_ts_seq)
  {
    return 0;
  }

  av_render_set_play_mode(p_r_filter, cur_play_mode);

  if (p_priv->play_end)
  {
    if (cur_play_mode == TS_SEQ_REV_FAST_PLAY_2X
          || cur_play_mode == TS_SEQ_REV_FAST_PLAY_4X
          || cur_play_mode == TS_SEQ_REV_FAST_PLAY_8X
          || cur_play_mode == TS_SEQ_REV_FAST_PLAY_16X
          || cur_play_mode == TS_SEQ_REV_FAST_PLAY_32X)
    {
      p_priv->play_end = 0;
      vdec_resume(p_priv->p_video_dev);
      aud_resume_vsb(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO));
    }
    else
    {
      OS_PRINTF("\n##debug: can not set play mode !already play end!!\n");
      return 0;
    }
  }
  
  ///TODO:minnan add should change, purpos to mute and unmute audio, andy pls modify
#ifndef WIN32
  void *p_audio_dev = NULL;
  p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  MT_ASSERT(p_audio_dev != NULL);

  if(cur_play_mode == TS_SEQ_NORMAL_PLAY)
  {
    aud_mute_onoff_vsb(p_audio_dev, FALSE);
  }
  else
  {
    aud_mute_onoff_vsb(p_audio_dev, TRUE);
  }
#endif

  if ((p_priv->play_mode == TS_SEQ_NORMAL_PLAY
      && cur_play_mode == TS_SEQ_FAST_PLAY_2X)
    || (p_priv->play_mode == TS_SEQ_FAST_PLAY_2X
      && cur_play_mode == TS_SEQ_NORMAL_PLAY))
  {
    p_priv->play_mode = cur_play_mode;
    return 0;
  }
  
  {
    if (((p_priv->play_mode == TS_SEQ_FAST_PLAY_4X
      || p_priv->play_mode == TS_SEQ_FAST_PLAY_8X
      || p_priv->play_mode == TS_SEQ_FAST_PLAY_16X
      || p_priv->play_mode == TS_SEQ_FAST_PLAY_32X)
      && cur_play_mode == TS_SEQ_NORMAL_PLAY))
    {
      pos_offset = 1;
    }

    vdec_get_info(p_priv->p_video_dev, &vdec_info);

    if (vdec_info.pts)
    {
      p_dy_info = media_idx_found_by_pts_ex(p_priv->cfg.p_media_idx,
        vdec_info.pts, p_priv->last_send_index, SEARCH_DIR(p_priv->play_mode));
      if (p_dy_info)
      {
        play_pos = p_dy_info->indx + pos_offset;
      }
      else
      {
        play_pos = p_priv->last_play_pos + pos_offset;
      }
    }
    else
    {
      play_pos = p_priv->last_play_pos + pos_offset;
    }

    play_pos *= TS_STREAM_RW_UNIT;

    p_priv->sample_request = SAMPLE_REQUEST_TRICK_MODE_AV_RESET;
    //av_render_av_reset(p_r_filter);
    if (is_auto)
    {
      if (p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_2X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_4X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_8X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_16X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_32X)
      {
        play_pos = 0;
      }
    }
    
    p_priv->m_in_pin.request_sample(&p_priv->m_in_pin,
      &format, p_priv->cfg.load_unit_size, play_pos, TS_PLAYER_SEEK_SET + 1);
    
    OS_PRINTF("\n##do play mode change reset[%d][%d->%d ]!!!\n",
      is_auto, p_priv->play_mode, cur_play_mode);
    
    if ((cur_play_mode == TS_SEQ_NORMAL_PLAY) || 
      (cur_play_mode == TS_SEQ_FAST_PLAY_2X))
    {
      av_render_av_reset(p_r_filter);
      av_render_trick_mode(p_r_filter, 1);
    }
    else if ((p_priv->play_mode == TS_SEQ_NORMAL_PLAY) ||
      (p_priv->play_mode == TS_SEQ_FAST_PLAY_2X))
    {
      av_render_trick_mode(p_r_filter, 0);
      av_render_av_reset(p_r_filter);
    }
    else
    {
      av_render_av_reset(p_r_filter);
    }
  }  

  p_priv->play_mode = cur_play_mode;
  return 1;
}

static void timeshift_do_seek(ts_player_filter_private_t *p_priv,
  u32 play_time, u32 play_pos, ts_player_seek_param_t *p_seek)
{
  s32 index = -1;
  u32 seek_time = 0;
  s64 seek_pos = 0;
  s32 seek_offset = (s32)p_seek->offset;
  dynamic_rec_info_t *p_dy_info = NULL;
  
  switch (p_seek->seek_op)
  {
  case TS_PLAYER_SEEK_SET:
    {
      if (p_seek->seek_type == TS_PLAYER_TIME_SEEK)
      {
        if (seek_offset <= 0)
        {
          seek_time = 0;
        }
        else
        {
          seek_time = seek_offset;
        }

        p_dy_info = media_idx_found_by_ms(p_priv->cfg.p_media_idx, seek_time * 1000);
        MT_ASSERT(p_dy_info != NULL);
        index = p_dy_info->indx;
        
        OS_PRINTF("\n##seek to index[%d] seek[%d] time[%d]\n",
          index, seek_time, p_dy_info->cur_time);
      }
      else if (p_seek->seek_type == TS_PLAYER_POSITION_SEEK)
      {
        if (seek_offset < 0)
        {
          seek_pos = 0;
        }
        else
        {
          seek_pos = seek_offset;
        }
        index = seek_pos / TS_STREAM_RW_UNIT;
      }
    }
    break;

  case TS_PLAYER_SEEK_CUR:
    {
      if (p_seek->seek_type == TS_PLAYER_TIME_SEEK)
      {
        if (seek_offset > 0)
        {
          seek_time = play_time + seek_offset;
        }
        else
        {
          if (play_time >= ABS(seek_offset))
          {
            seek_time = play_time + seek_offset;
          }
          else
          {
            OS_PRINTF("\n##warning seek offset[%ld]\n", seek_offset);
            seek_time = 0;
          }
        }
        p_dy_info = media_idx_found_by_ms(p_priv->cfg.p_media_idx, seek_time * 1000);
        MT_ASSERT(p_dy_info != NULL);
        index = p_dy_info->indx;
        
        OS_PRINTF("\n##SEEK_CUR:seek to index[%lu(total:%lu), %lu][%lu, %ld]seek_time[%lu]\n",
          index, p_priv->cfg.p_media_idx->unit_total, p_dy_info->cur_time,
          play_time, seek_offset, seek_time);
      }
      else if (p_seek->seek_type == TS_PLAYER_POSITION_SEEK)
      {
        if (seek_offset > 0)
        {
          seek_pos = (s64)play_pos * TS_STREAM_RW_UNIT + seek_offset;
        }
        else
        {
          if (((s64)play_pos * TS_STREAM_RW_UNIT) >= ABS(seek_offset))
          {
            seek_pos = (s64)play_pos * TS_STREAM_RW_UNIT + seek_offset;
          }
          else
          {
            OS_PRINTF("\n##warning seek cur offset[%ld]\n", seek_offset);
            seek_pos = 0;
          }
        }
        index = seek_pos / TS_STREAM_RW_UNIT;
      }
    }
    break;

  case TS_PLAYER_SEEK_END:
    p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx,
        p_priv->cfg.p_media_idx->get_cnt - 1);
    MT_ASSERT(p_dy_info != NULL);
    
    if (p_seek->seek_type == TS_PLAYER_TIME_SEEK)
    {
      if (p_dy_info->cur_time > seek_offset)
      {
        seek_time = p_dy_info->cur_time - seek_offset;
        p_dy_info = media_idx_found_by_ms(p_priv->cfg.p_media_idx, seek_time * 1000);
        MT_ASSERT(p_dy_info != NULL);
        index = p_dy_info->indx;
      }
      else
      {
        seek_time = p_dy_info->cur_time;
        index = p_priv->cfg.p_media_idx->get_cnt - 1;
      }
    }
    else
    {
      if (seek_offset > 0)
      {
        seek_pos = (s64)p_dy_info->indx * TS_STREAM_RW_UNIT - seek_offset;
        index = seek_pos / TS_STREAM_RW_UNIT;
      }
      else
      {
        index = p_priv->cfg.p_media_idx->get_cnt - 1;
      }
    }
    break;
  default:
    break;
  }

  if (index == -1)
  {
    OS_PRINTF("\n##timeshift_do_seek error line[%d]!\n", __LINE__);
    index = p_priv->cfg.p_media_idx->get_cnt;
  }

  p_seek->seek_type = TS_PLAYER_POSITION_SEEK;
  p_seek->seek_op = TS_PLAYER_SEEK_SET;
  p_seek->offset = (s64)index * TS_STREAM_RW_UNIT;
  p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, index);
  OS_PRINTF("\n##timeshift seek to index[%lu][%lu]\n",
    index, p_dy_info->cur_time);
}

static s64 file_seek_time2pos(ts_player_filter_private_t *p_priv, s32 seek_time)
{
  dynamic_rec_info_t *p_dy_info = NULL;
  s64 seek_pos = 0;
  
  p_dy_info = media_idx_found_by_ms(p_priv->cfg.p_media_idx, seek_time * 1000);
  if (p_dy_info)
  {
    seek_pos = (s64)p_dy_info->indx * TS_STREAM_RW_UNIT;
    OS_PRINTF("\n##found_by_ms seek[%d]s, found[%d]ms[%ld]\n",
      seek_time, p_dy_info->cur_time, p_dy_info->indx);
  }
  else
  {
    //
  }

  return seek_pos;
}

static void file_do_seek(ts_player_filter_private_t *p_priv,
  u32 play_time, s64 play_pos, ts_player_seek_param_t *p_seek)
{
  s32 seek_offset = (s32)p_seek->offset;
  
  switch (p_seek->seek_type)
  {
  case TS_PLAYER_TIME_SEEK:
    {
      switch (p_seek->seek_op)
      {
      case TS_PLAYER_SEEK_SET:
        if (seek_offset < 0)
        {
          OS_PRINTF("\n##warning seek set offset[%ld]\n", seek_offset);
          seek_offset = 0;
        }
        break;
      case TS_PLAYER_SEEK_CUR:
        if (seek_offset > 0)
        {
          seek_offset = play_time + seek_offset;
        }
        else
        {
          if (play_time >= ABS(seek_offset))
          {
            seek_offset = play_time + seek_offset;
          }
          else
          {
            OS_PRINTF("\n##warning seek cur offset[%ld]\n", seek_offset);
            seek_offset = 0;
          }
        }
        p_seek->seek_op = TS_PLAYER_SEEK_SET;
        break;
      case TS_PLAYER_SEEK_END:
        break;
      default:
        break;
      }

      p_seek->offset = file_seek_time2pos(p_priv, seek_offset);
      p_seek->seek_type = TS_PLAYER_POSITION_SEEK;
      p_seek->offset = p_seek->offset - p_seek->offset % 188;
    }
    break;

  case TS_PLAYER_POSITION_SEEK:
    {
      switch (p_seek->seek_op)
      {
      case TS_PLAYER_SEEK_SET:
        if (seek_offset < 0)
        {
          OS_PRINTF("\n##warning seek set offset[%ld]\n", seek_offset);
          p_seek->offset = 0;
        }
        break;
      case TS_PLAYER_SEEK_CUR:
        if (seek_offset > 0)
        {
          p_seek->offset = play_pos + seek_offset;
        }
        else
        {
          if (play_pos >= ABS(seek_offset))
          {
            p_seek->offset = play_pos + seek_offset;
          }
          else
          {
            OS_PRINTF("\n##debug: warning seek cur offset[%ld]\n", seek_offset);
            p_seek->offset = 0;
          }
        }
        p_seek->seek_op = TS_PLAYER_SEEK_SET;
        break;
      case TS_PLAYER_SEEK_END:
        break;
      default:
        break;
      }
      p_seek->offset = p_seek->offset - p_seek->offset % (188 * 4);
    }
    break;

  case TS_PLAYER_FRAME_SEEK:
    break;

  default:
    break;
  }
}

static void ts_player_seek(ts_player_filter_private_t *p_priv, ts_player_seek_param_t *p_seek)
{
  media_format_t  format = {0};
  ts_player_seek_param_t seek = {0};
  
  MT_ASSERT(p_seek != NULL);
  if (p_priv->play_mode != TS_SEQ_NORMAL_PLAY)
  {
    OS_PRINTF("\n##debug: warning unnormal mode do seek!\n");
    return ;
  }

  OS_PRINTF("\n##player_seek %d, %d, %ld\n", p_seek->seek_type,
    p_seek->seek_op, (s32)p_seek->offset);

  seek.seek_type = p_seek->seek_type;
  seek.seek_op = p_seek->seek_op;
  seek.offset = p_seek->offset;

  if (p_seek->seek_type == TS_PLAYER_FRAME_SEEK)
  {
    OS_PRINTF("\n##debug: do not support now!!\n");
    return ;
  }

  p_priv->sample_request = SAMPLE_REQUEST_SEEK;
  
  if (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_BLOCK)
  {
    timeshift_do_seek(p_priv, 
      p_priv->last_play_time / 1000, p_priv->last_play_pos, &seek);
  }
  else
  {
    if (p_priv->player_running_state == PLAYER_RUNNING_PLAY_END)
    {
      p_priv->player_running_state = PLAYER_RUNNING_NORMAL;
    }
    file_do_seek(p_priv,
      p_priv->last_play_time / 1000, (s64)p_priv->last_play_pos * TS_STREAM_RW_UNIT, &seek);
  }

  p_priv->m_in_pin.request_sample(&p_priv->m_in_pin,
    &format, p_priv->cfg.load_unit_size, seek.offset, seek.seek_op + 1);
  mtos_sem_take(&p_priv->seek_sem, 0);
}

static void change_audio_channel(ts_player_filter_private_t *p_priv, u16 a_pid, u8 fmt)
{
  vdec_info_t vdec_info;
  media_format_t  format = {0};
  s64 play_pos = 0;
  s32 pos_offset = 0;
  transf_output_pin_t *p_out_pin = NULL;
  ipin_t *p_ipin = NULL;
  ifilter_t *p_r_filter = NULL;
  dynamic_rec_info_t *p_dy_info = NULL;
  
  p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  p_ipin = (ipin_t *)(p_out_pin->m_pin._ipin.get_connected(p_out_pin));
  p_r_filter = (ifilter_t *)p_ipin->get_filter(p_ipin);    

  if (p_priv->play_mode != TS_SEQ_NORMAL_PLAY)
  {
    OS_PRINTF("\n##normal play mode do not support change audio pid!\n");
    return ;
  }

  if (p_priv->play_end)
  {
    OS_PRINTF("\n##can not set play mode !already play end!!\n");
    return ;
  }

  if (p_priv->rec_info.a_pid == a_pid)
  {
    return ;
  }

  p_priv->sample_request = SAMPLE_REQUEST_SEEK;
  p_priv->rec_info.a_pid = a_pid;
  p_priv->rec_info.audio_fmt = fmt;
  pos_offset = 1;

  vdec_get_info(p_priv->p_video_dev, &vdec_info);

  if (vdec_info.pts)
  {
    p_dy_info = media_idx_found_by_pts_ex(p_priv->cfg.p_media_idx,
      vdec_info.pts, p_priv->last_send_index, SEARCH_DIR(p_priv->play_mode));
    if (p_dy_info)
    {
      play_pos = p_dy_info->indx + pos_offset;
    }
    else
    {
      play_pos = p_priv->last_play_pos + pos_offset;
    }
  }
  else
  {
    play_pos = p_priv->last_play_pos + pos_offset;
  }
  
  play_pos *= TS_STREAM_RW_UNIT;

  p_priv->m_in_pin.request_sample(&p_priv->m_in_pin,
    &format, p_priv->cfg.load_unit_size, play_pos, TS_PLAYER_SEEK_SET + 1);
  mtos_sem_take(&p_priv->seek_sem, 0);
}

static RET_CODE on_command(handle_t _this, icmd_t *p_cmd)
{
  ts_player_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)_this;

  switch (p_cmd->cmd)
  {
  case TS_PLAYER_CMD_CFG:
    memcpy(&p_priv->cfg, (void *)p_cmd->lpara, sizeof(ts_player_config_t));
    memset(&p_priv->rec_info, 0, sizeof(dynamic_rec_info_t));
    p_priv->rec_info.v_pid = p_priv->cfg.video_pid;
    p_priv->rec_info.a_pid = p_priv->cfg.audio_pid;
    p_priv->rec_info.pcr_pid = p_priv->cfg.pcr_pid;
    p_priv->rec_info.video_fmt = p_priv->cfg.video_fmt;
    p_priv->rec_info.audio_fmt = p_priv->cfg.audio_fmt;
    break;

  case TS_PLAYER_CMD_SET_PLAY_MODE:
    p_ifilter->forward_command(_this, p_cmd);
    //set_play_mode(p_priv, p_cmd->lpara, (*(u32 *)p_cmd->p_para), 0);
    break;

  case TS_PLAYER_CMD_SEEK:
    ts_player_seek(p_priv, (ts_player_seek_param_t *)p_cmd->p_para);
    break;
    
  case TS_PLAYER_CMD_CHANGE_AUDIO:
    change_audio_channel(p_priv, (p_cmd->lpara >> 16), p_cmd->lpara & 0xFF);
    break;
    
  default:
    return ERR_FAILURE;
  }
  return SUCCESS;
}

#ifdef PLAY_TIME_DEBUG
static void play_time_debug(ts_player_filter_private_t *p_priv, u32 found)
{
  u32 loopi = 0;
  u32 start_index = 0;
  u32 end_index = 0;
  dynamic_rec_info_t *p_dy_info = NULL;

  p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, p_priv->last_send_index);
  
  if (p_dy_info->indx > found)
  {
    start_index = found;
    if (start_index >= 1)
    {
      start_index -= 1;
    }
    end_index = p_dy_info->indx;
  }
  else
  {
    start_index = p_dy_info->indx;
    end_index = found + 1;
  }

  for (loopi = start_index; loopi <= end_index; loopi ++)
  {
    p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, loopi);
    OS_PRINTF("\n##search[%ld][%ld, 0x%x, %ld]",
      loopi, p_dy_info->indx, p_dy_info->pts, p_dy_info->cur_time);
  }
}
#endif

static void trans_req_sample(ts_player_filter_private_t *p_priv,
  media_format_t *p_format, s64 position, u32 seek_op)
{
    mtos_sem_take(&p_priv->seek_req_sem, 0);
    if (p_priv->sample_request == 0)
    {
      p_priv->m_in_pin.request_sample(&p_priv->m_in_pin,
        p_format, p_priv->cfg.load_unit_size, position, seek_op);
    }
    mtos_sem_give(&p_priv->seek_req_sem);
}

static BOOL _is_play_time_normal(ts_player_filter_private_t *p_priv, u32 play_time)
{
  u32 forward = 0;

  switch (p_priv->play_mode)
  {
  case TS_SEQ_REV_FAST_PLAY_2X:
  case TS_SEQ_REV_FAST_PLAY_4X:
  case TS_SEQ_REV_FAST_PLAY_8X:
  case TS_SEQ_REV_FAST_PLAY_16X:
  case TS_SEQ_REV_FAST_PLAY_32X:
    forward = 0;
    break;
  default:
    forward = 1;
    break;
  }

  if ((forward && ((play_time / 1000) > (p_priv->last_play_time / 1000)))
    || (!forward && ((play_time / 1000) < (p_priv->last_play_time / 1000))))
  {
    return TRUE;
  }

  return FALSE;
}

static void update_play_time(handle_t _this)
{
  ts_player_filter_private_t *p_priv = get_priv(_this);
  u32 play_pos = 0;
  u32 play_time = 0;
  vdec_info_t vdec_info;
  os_msg_t msg = {0};
  media_format_t  format = {0};
  dynamic_rec_info_t *p_dy_info = NULL;
  ifilter_t *p_ifilter = p_priv->p_this;
  s64 total_size = 0;
  RET_CODE ret = 0;

  if (p_priv->sample_request
    || p_priv->seek_end_check
    || p_priv->player_running_state == PLAYER_RUNNING_PLAY_END) 
  {
    return ;
  }

  ret = vdec_get_info(p_priv->p_video_dev, &vdec_info);
  
  if (vdec_info.pts == 0 && p_priv->player_running_state == PLAYER_RUNNING_NORMAL)
  {
#ifdef PLAY_TIME_DEBUG
    //OS_PRINTF("\n##vdec_get_info [%ld]\n", vdec_info.pts);
#endif
    return ;
  }
  
  if (vdec_info.pts)
  {
    p_dy_info = media_idx_found_by_pts_ex(p_priv->cfg.p_media_idx,
      vdec_info.pts, p_priv->last_send_index, SEARCH_DIR(p_priv->play_mode));
    if (p_dy_info == NULL)
    {
      if (p_priv->player_running_state == PLAYER_RUNNING_CHECK_PLAY_END)
      {
        play_pos = p_priv->last_play_pos + 1;
        play_time = p_priv->last_play_time;
        if ((vdec_info.pts - p_priv->last_pts) >= 45000)
        {
          play_time += 1000;
        }
      }
      else
      {
        return ;
      }
    }
    else
    {
      play_pos = p_dy_info->indx;
      play_time = p_dy_info->cur_time;
#ifdef PLAY_TIME_DEBUG
    }

    {
      u32 time_skip = 0;
      if (play_time > p_priv->last_play_time)
      {
        time_skip = play_time - p_priv->last_play_time;
      }
      else
      {
        time_skip = p_priv->last_play_time - play_time;
      }
      
      if (((p_priv->play_mode == TS_SEQ_NORMAL_PLAY) && (time_skip >= 2000))
        || ((p_priv->play_mode != TS_SEQ_NORMAL_PLAY) && (time_skip > 10000)))
      {
        OS_PRINTF("\n##IN [%d][0x%x, %lu, %lu] found[0x%x, %lu, %lu]",
          SEARCH_DIR(p_priv->play_mode), vdec_info.pts, p_priv->last_send_index,
          p_priv->cfg.p_media_idx->unit_total,
          p_dy_info->pts, p_dy_info->indx, p_dy_info->cur_time);
        //play_time_debug(p_priv, p_dy_info->indx);
        OS_PRINTF("\n");
      }
#endif
    }

    //if ((play_time / 1000) != (p_priv->last_play_time / 1000))
    if (_is_play_time_normal(p_priv, play_time))
    {
      msg.content = PLAYER_MSG_PLAY_TIME_UPDATE;
      p_priv->last_play_time = (play_time / 1000) * 1000;
      p_priv->last_play_pos = play_pos;
      p_priv->last_pts = vdec_info.pts;
      msg.para1 = p_priv->last_play_time;
      total_size = ((s64)play_pos) * TS_STREAM_RW_UNIT;
      msg.para2 = (u32)(total_size >> 10);/*KBYTES*/
#ifdef PLAY_TIME_DEBUG
      OS_PRINTF("\n##play time[%lu, 0x%x][%lu, %lu, %lu]",
      msg.para1 / 1000, vdec_info.pts,
      p_priv->last_send_index, p_dy_info->indx, p_dy_info->cur_time / 1000);
#endif
      p_ifilter->send_message_out(p_ifilter, &msg);
      return ;
    }
  }
  
  if (p_priv->player_running_state != PLAYER_RUNNING_NORMAL)
  {
    if (p_priv->last_sample != vdec_info.pts)
    {
      p_priv->last_sample = vdec_info.pts;
      p_priv->play_end_check = 0;
      mtos_task_delay_ms(40);
    }
    else
    {
      p_priv->play_end_check ++;
      mtos_task_delay_ms(40);
    }
    
    if((p_priv->cfg.player_src == TS_PLAYER_SRC_REC_FILE)
      && (!(p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_2X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_4X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_8X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_16X
          || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_32X)))
    {
      if (p_priv->play_end_check >= 15)//600ms
      {
        msg.content = AV_RENDER_MSG_FILE_PLAY_END;
#ifndef PLAY_TIME_TASK_ENABLE
        p_ifilter->set_active_enter(_this, NULL);
#endif
        OS_PRINTF("\n##debug: AV_RENDER_MSG_FILE_PLAY_END [0x%x]!!!\n", vdec_info.pts);
        p_priv->player_running_state = PLAYER_RUNNING_PLAY_END;
        p_priv->last_sample = 0;
        p_priv->play_end_check = 0;
        p_ifilter->send_message_out(p_ifilter, &msg);
        vdec_pause(p_priv->p_video_dev);
        aud_pause_vsb(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO));
        p_priv->play_end = 1;
        if (p_priv->play_mode != TS_SEQ_NORMAL_PLAY)
        {
          aud_mute_onoff_vsb(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO), FALSE);
          p_priv->play_mode = TS_SEQ_NORMAL_PLAY;
        }
      }
    }
    else
    {
      if (p_priv->play_end_check >= 15)//check play mode end for 600ms
      {
#ifndef PLAY_TIME_TASK_ENABLE
        p_ifilter->set_active_enter(_this, NULL);
#endif
        p_priv->player_running_state = PLAYER_RUNNING_NORMAL;
        p_priv->last_sample = 0;
        p_priv->last_play_time = 0;
        p_priv->play_end_check = 0;

        if (!set_play_mode(p_priv, TS_SEQ_NORMAL_PLAY, 0, 1))
        {
          if (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_FILE)
          {
            trans_req_sample(p_priv, &format, 0, TS_PLAYER_SEEK_SET + 1);
          }
          else
          {
            trans_req_sample(p_priv, &format, 0, TS_PLAYER_SEEK_CUR + 1);
          }
        }

        msg.content = PLAYER_MSG_PLAY_MODE_CHANGED;
        p_ifilter->send_message_out(p_ifilter, &msg);
      }
    }
  }
}

static void split_push_down(ts_player_filter_private_t *p_priv, media_sample_t *p_in)
{
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;

  while (p_in->payload > MAX_PUSH_PAYLOAD)
  {
    p_out_pin->query_sample(p_out_pin, &p_out_sample);
    MT_ASSERT(NULL != p_out_sample);
    p_out_sample->p_data = p_in->p_data;
    p_out_sample->payload = MAX_PUSH_PAYLOAD;
    if (p_priv->seek_end_check || p_priv->pid_changed)
    {
      p_priv->seek_end_check = 0;
      p_priv->pid_changed = 0;
      p_out_sample->context = SAMPLE_CONTEXT_SEEK_FLAG;
      p_out_sample->p_user_data = (void *)&p_priv->rec_info;
    }
    else
    {
      p_out_sample->context = 0;
      p_out_sample->p_user_data = NULL;
    }
    p_in->p_data = (u8 *)((u32)p_in->p_data + p_out_sample->payload);
    p_in->payload -= p_out_sample->payload;
    p_out_pin->push_down(p_out_pin, p_out_sample);
  }
  
  if (p_priv->seek_end_check || p_priv->pid_changed)
  {
    p_priv->seek_end_check = 0;
    p_priv->pid_changed = 0;
    p_in->context = SAMPLE_CONTEXT_SEEK_FLAG;
    p_in->p_user_data = (void *)&p_priv->rec_info;
  }
  else
  {
    p_in->context = 0;
    p_in->p_user_data = NULL;
  }
  p_out_pin->push_down(p_out_pin, p_in);
  return ;
}

static u32 push_down(ts_player_filter_private_t *p_priv, media_sample_t *p_in)
{
  transf_output_pin_t *p_out_pin = NULL;
  video_codec_type_t type = MPEG_TYPE_CODE;
  u32 loopi = 0;
  u8 *p_start = NULL;
  media_sample_t *p_out_sample = NULL;
  ts_seq_input_mode_t input_mode = TS_SEQ_INPUT_TS;

  p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;

  if (p_in->payload == 0)
  {
    return 0;
  }
  
  if (p_priv->trick_mode_skip)
  {
    input_mode = TS_SEQ_INPUT_TS_HD;
  }
  else
  {
    input_mode = TS_SEQ_INPUT_TS;
  }
  
  if (p_priv->seek_end_check == 0)
  {
    ts_seq_play1(p_priv->p_ts_seq, p_priv->p_video_dev,
      p_in->p_data,
      p_in->payload,
      p_priv->cfg.video_pid,
      p_priv->play_mode,
      input_mode, &p_priv->ts_info);
  }
  else
  {
    switch (p_priv->cfg.video_fmt)
    {
    case VIDEO_MPEG:
    case VIDEO_MPEG_ES:
      type = MPEG_TYPE_CODE;
      break;
    case VIDEO_H264:
    case VIDEO_H264_ES:
      type = H264_TYPE_CODE;
      break;
    case VIDEO_AVS:
    case VIDEO_AVS_ES:
      type = AVS_TYPE_CODE;
      break;
    default:
      OS_PRINTF("\n##debug: do not support video fmt!\n");
      break;
    }
   
    ts_seq_get_vkey_frame_vsb(p_in->p_data, p_in->payload,
      type, p_priv->cfg.video_pid, &p_priv->ts_info);
    OS_PRINTF("\n##SEEK: [0x%x][%d, %lu]check[%lu]\n", p_in->p_data,
      p_priv->ts_info.fragment_num,
      p_priv->ts_info.fragment_offset[0],
      p_priv->seek_end_check);
    if (p_priv->ts_info.fragment_num)
    {
      p_priv->ts_info.fragment_size[0] = p_in->payload - p_priv->ts_info.fragment_offset[0];
    }
    else
    {
      p_priv->seek_end_check ++;
      return 0;
    }
  }
  
  MT_ASSERT((p_priv->ts_info.reverse_offset % 188) == 0);
  
  {
#ifdef DUMP_TRICK_MODE_FILE
    for (loopi = 0; loopi < (p_priv->ts_info.fragment_num); loopi ++)
    {
      p_start = (u8 *)(((u32)p_in->p_data) + p_priv->ts_info.fragment_offset[loopi]);
      if (test_file)
      {
        vfs_write(p_start, p_priv->ts_info.fragment_size[loopi], 1, test_file);
      }
    }
#endif

    p_start = p_in->p_data;
    if (p_priv->ts_info.fragment_num > 1)
    {
      for (loopi = 0; loopi < (p_priv->ts_info.fragment_num - 1); loopi ++)
      {
        p_out_pin->query_sample(p_out_pin, &p_out_sample);
        MT_ASSERT(NULL != p_out_sample);
        p_out_sample->p_data = (u8 *)((u32)p_in->p_data + p_priv->ts_info.fragment_offset[loopi]);
        p_out_sample->payload = p_priv->ts_info.fragment_size[loopi];
        p_out_sample->context = 0;
        p_out_sample->p_user_data = NULL;
        split_push_down(p_priv, p_out_sample);
      }
      loopi = p_priv->ts_info.fragment_num - 1;
    }
    else if (p_priv->ts_info.fragment_num == 1)
    {
      loopi = 0;
    }
    else
    {
      return 0;
    }
    p_in->p_data = (u8 *)((u32)p_in->p_data + p_priv->ts_info.fragment_offset[loopi]);
    p_in->payload = p_priv->ts_info.fragment_size[loopi];
    split_push_down(p_priv, p_in);
  }

  return (p_priv->ts_info.fragment_num ? 1 : 0);
}

static RET_CODE get_dy_rec_info(ts_player_filter_private_t *p_priv, media_sample_t *p_in)
{
  dynamic_rec_info_t *p_dy_info = NULL;
  u32 start_index = 0;
  u32 end_index = 0;
  u32 loopi = 0;
  u32 read_pos = 0;

  if (p_in->payload == 0)
  {
    return SUCCESS;
  }

  read_pos = (u32)p_in->p_user_data;
  start_index = read_pos / TS_STREAM_RW_UNIT;
  end_index = (read_pos + p_in->payload) / TS_STREAM_RW_UNIT - 1;
  if (p_priv->play_mode == TS_SEQ_NORMAL_PLAY
    || p_priv->play_mode == TS_SEQ_FAST_PLAY_2X
    || p_priv->play_mode == TS_SEQ_FAST_PLAY_4X
    || p_priv->play_mode == TS_SEQ_FAST_PLAY_8X
    || p_priv->play_mode == TS_SEQ_FAST_PLAY_16X
    || p_priv->play_mode == TS_SEQ_FAST_PLAY_32X
    || p_priv->play_mode == TS_SEQ_SLOW_PLAY_2X
    || p_priv->play_mode == TS_SEQ_SLOW_PLAY_4X)
  {
    p_priv->last_send_index = end_index;
  }
  else
  {
    p_priv->last_send_index = start_index;
  }

  for (loopi = start_index; loopi <= end_index; loopi ++)
  {
    p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, loopi);

    if (p_dy_info)
    {
      if (!p_priv->rec_info.vkey_frame_offset && p_dy_info->vkey_frame_offset)
      {
        p_priv->rec_info.vkey_frame_offset = p_dy_info->vkey_frame_offset;
      }
      
      if ((p_dy_info->v_pid != p_priv->cfg.video_pid && p_dy_info->v_pid != 0)
        || (p_dy_info->a_pid != p_priv->cfg.audio_pid && p_dy_info->a_pid != 0)
        || (p_dy_info->pcr_pid != p_priv->cfg.pcr_pid && p_dy_info->pcr_pid != 0))
      {
        OS_PRINTF("\n##pid changed [v:%d,%d, a:%d,%d, pcr:%d]->[v:%d,%d, a:%d,%d, pcr:%d]\n",
          p_priv->rec_info.v_pid,
          p_priv->rec_info.video_fmt,
          p_priv->rec_info.a_pid,
          p_priv->rec_info.audio_fmt, 
          p_priv->rec_info.pcr_pid,
          p_dy_info->v_pid,
          p_dy_info->video_fmt,
          p_dy_info->a_pid,
          p_dy_info->audio_fmt,
          p_dy_info->pcr_pid);
        
        if ((p_dy_info->audio_fmt >= AUDIO_UNKNOWN)
          || (p_dy_info->video_fmt >= VIDEO_UNKNOWN))
        {
          OS_PRINTF("\n##unsupport fmt[a:%d, v:%d]!\n",
            p_dy_info->audio_fmt, p_dy_info->video_fmt);
          return ERR_FAILURE;
        }
        
        p_priv->pid_changed = 1;
        p_priv->rec_info.v_pid = p_dy_info->v_pid;
        p_priv->rec_info.a_pid = p_dy_info->a_pid;
        p_priv->rec_info.pcr_pid = p_dy_info->pcr_pid;
        p_priv->rec_info.video_fmt = p_dy_info->video_fmt;
        p_priv->rec_info.audio_fmt = p_dy_info->audio_fmt;
        
        p_priv->cfg.video_pid = p_priv->rec_info.v_pid;
        p_priv->cfg.audio_pid = p_priv->rec_info.a_pid;
        p_priv->cfg.pcr_pid = p_priv->rec_info.pcr_pid;
        p_priv->cfg.video_fmt = p_priv->rec_info.video_fmt;
        p_priv->cfg.audio_fmt = p_priv->rec_info.audio_fmt;
      }
    }
    else
    {
      OS_PRINTF("\n##get_dy_rec_info index [%lu]NULL!!\n", loopi);
    }
  }
  return SUCCESS;
}

static u32 get_next_send_offset(ts_player_filter_private_t *p_priv, u32 read_pos, u32 start_index)
{
  dynamic_rec_info_t *p_dy_info = NULL;
  u32 loopi = start_index;
  u32 cur_offset = 0;
  u32 found = 0;

  p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, read_pos / TS_STREAM_RW_UNIT);
  cur_offset = p_dy_info->vkey_frame_offset - p_dy_info->vkey_frame_offset % TS_STREAM_RW_UNIT;
  found = cur_offset;

  while (found == cur_offset)
  {
    if (media_idx_check_index_range(p_priv->cfg.p_media_idx, loopi) >= 2)
    {
      OS_PRINTF("\n##get file end!!!");
      found = read_pos + p_priv->cfg.load_unit_size;
      break;
    }
    p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, loopi);
    loopi ++;
    found = p_dy_info->vkey_frame_offset - p_dy_info->vkey_frame_offset % TS_STREAM_RW_UNIT;
  }

  return found;
}

static u32 get_prev_send_offset(ts_player_filter_private_t *p_priv, u32 read_pos, u32 start_index)
{
  dynamic_rec_info_t *p_dy_info = NULL;
  u32 loopi = start_index;
  u32 cur_offset = 0;
  u32 found = 0;

  p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, read_pos / TS_STREAM_RW_UNIT);
  cur_offset = p_dy_info->vkey_frame_offset - p_dy_info->vkey_frame_offset % TS_STREAM_RW_UNIT;
  found = cur_offset;

  while (found == cur_offset)
  {
    if (loopi == 0
      || cur_offset == 188)
    {
      OS_PRINTF("\n##get file head!!!");
      found = read_pos + TS_STREAM_RW_UNIT;
      break;
    }
    p_dy_info = media_idx_read_by_index(p_priv->cfg.p_media_idx, loopi);
    loopi --;
    found = p_dy_info->vkey_frame_offset - p_dy_info->vkey_frame_offset % TS_STREAM_RW_UNIT;
  }

  return found;
}

static void forward_transform(ts_player_filter_private_t *p_priv, media_sample_t *p_in)
{
  u32 read_pos = 0;
  s64 cur_offset = 0;
  u32 send = 0;
  u32 payload = p_in->payload;

  read_pos = (u32)p_in->p_user_data;

  send = push_down(p_priv, p_in);
  
  if (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_BLOCK)
  {
    if (p_priv->trick_mode_skip)
    {
      if (send == 1)
      {
        cur_offset = (u32)p_priv->trick_mode_skip * TS_STREAM_RW_UNIT;
      }
      else
      {
        if (p_priv->ts_info.reverse_offset)
        {
          cur_offset = payload - (p_priv->ts_info.reverse_offset
            - p_priv->ts_info.reverse_offset % TS_STREAM_RW_UNIT);
          cur_offset = 0 - cur_offset;
        }
        else
        {
          cur_offset = 0;
        }
      }
      
      if ((s64)p_priv->cfg.load_unit_size == (0 - cur_offset))
      {
        cur_offset = 0;
      }
      trans_req_sample(p_priv, &p_in->format, cur_offset, TS_PLAYER_SEEK_CUR + 1);
    }
    else
    {
      trans_req_sample(p_priv, &p_in->format, 0, 0);
    }
  }
  else
  {
    if (p_priv->trick_mode_skip)
    {
#ifndef WIN32
      //if (p_priv->rec_info.vkey_frame_offset)
      {
        cur_offset = get_next_send_offset(p_priv,
          read_pos, read_pos / TS_STREAM_RW_UNIT + p_priv->trick_mode_skip);
      }
#else
      //else
      {
        if (send == 1)
        {
          read_pos += (p_priv->trick_mode_skip * TS_STREAM_RW_UNIT);
        }
        else
        {
          if (p_priv->ts_info.reverse_offset)
          {
            if (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_BLOCK)
            {
              read_pos += (p_priv->ts_info.reverse_offset
                - p_priv->ts_info.reverse_offset % TS_STREAM_RW_UNIT);
            }
            else
            {
              read_pos += p_priv->ts_info.reverse_offset;
            }
          }
          else
          {
            read_pos += p_priv->cfg.load_unit_size;
          }
        }
        cur_offset = read_pos;
      }
#endif
      trans_req_sample(p_priv, &p_in->format, cur_offset, TS_PLAYER_SEEK_SET + 1);
    }
    else
    {
      trans_req_sample(p_priv, &p_in->format, 0, 0);
    }
  }
}

static void backward_transform(ts_player_filter_private_t *p_priv, media_sample_t *p_in)
{
  u32 read_pos = 0;
#ifndef PLAY_TIME_TASK_ENABLE
  ifilter_t *p_ifilter = p_priv->p_this;
#endif
  u32 send = 0;
  u32 req_offset = 0;
  s64 offset = 0;
  u32 payload = p_in->payload;

  read_pos = (u32)p_in->p_user_data;

  send = push_down(p_priv, p_in);

  if (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_BLOCK)
  {
    if (p_priv->trick_mode_skip)
    {
      if (send == 1)
      {
        offset = 0 - (s64)(p_priv->cfg.load_unit_size
          + payload + (u32)p_priv->trick_mode_skip * TS_STREAM_RW_UNIT);
      }
      else
      {
        send = p_priv->ts_info.reverse_offset - p_priv->ts_info.reverse_offset % TS_STREAM_RW_UNIT;
        send += TS_STREAM_RW_UNIT;
        offset = 0 - (s64)(p_priv->cfg.load_unit_size + payload - send);
      }
    }
    else
    {
      send = p_priv->ts_info.reverse_offset - p_priv->ts_info.reverse_offset % TS_STREAM_RW_UNIT;
      send += TS_STREAM_RW_UNIT;
      offset = 0 - (s64)(p_priv->cfg.load_unit_size + payload - send);
    }
    MT_ASSERT((offset % TS_STREAM_RW_UNIT) == 0);
    if ((s64)payload == (0 - offset))
    {
      offset = 0 - (s64)(p_priv->cfg.load_unit_size + payload);
    }
    /*p_priv->m_in_pin.request_sample(&p_priv->m_in_pin,
      &p_in->format, p_priv->cfg.load_unit_size, offset, TS_PLAYER_SEEK_CUR + 1);*/
    trans_req_sample(p_priv, &p_in->format, offset, TS_PLAYER_SEEK_CUR + 1);
  }
  else
  {
    if (p_priv->trick_mode_skip)
    {
#ifndef WIN32
      //if (p_priv->rec_info.vkey_frame_offset)
      {
        req_offset = (u32)p_priv->trick_mode_skip * TS_STREAM_RW_UNIT + p_priv->cfg.load_unit_size;
        if (read_pos >= req_offset)
        {
          req_offset = get_prev_send_offset(p_priv,
            read_pos, (read_pos - req_offset) / TS_STREAM_RW_UNIT);
          if (read_pos >= req_offset)
          {
            req_offset = read_pos - req_offset;
          }
        }
        else
        {
          req_offset = read_pos + TS_STREAM_RW_UNIT;
        }
      }
#else
      //else
    {
      if (send == 1)
      {
        req_offset = (u32)p_priv->trick_mode_skip * TS_STREAM_RW_UNIT + p_priv->cfg.load_unit_size;
      }
      else
      {
        req_offset = p_priv->cfg.load_unit_size - p_priv->ts_info.reverse_offset;
      }
    }
#endif
    }
    else
    {
      req_offset = p_priv->cfg.load_unit_size - p_priv->ts_info.reverse_offset;
    }
    
    if (read_pos >= req_offset)
    {
      /*p_priv->m_in_pin.request_sample(&p_priv->m_in_pin,
        &p_in->format, p_priv->cfg.load_unit_size,
        (read_pos - req_offset), TS_PLAYER_SEEK_SET + 1);*/
      trans_req_sample(p_priv, &p_in->format, (read_pos - req_offset) - (read_pos - req_offset) % (4 * 188), TS_PLAYER_SEEK_SET + 1);
    }
    else
    {
      p_priv->player_running_state = PLAYER_RUNNING_CHECK_PLAY_MODE_TO_NORMAL;
      p_priv->last_sample = 0;
      p_priv->play_end_check = 0;
#ifndef PLAY_TIME_TASK_ENABLE
      p_ifilter->set_active_enter(p_priv->p_this, update_play_time);
#endif
    }
  }
}



static void data_cipher(media_sample_t *p_in)
{
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  s32 i = 0;
  #define CIPHER_DATA_LENGTH 512*188
  ST_ADVANCEDCA_CIPHERENGINEINFO  stCipherEngineInfo;
  u8 enCipherEngine;
  u8 *p_u8key;
  u8 keylength;
  mul_pvr_get_cipher_key(&enCipherEngine, &p_u8key, &keylength); 
  switch(enCipherEngine)
  {
      case ADVANCEDCA_CIPHERENGINE_AES:
          stCipherEngineInfo.enCipherEngine = ADVANCEDCA_CIPHERENGINE_AES;
          stCipherEngineInfo.enBlockMode    = ADVANCEDCA_BLOCKMODE_ECB;
          stCipherEngineInfo.stKeyConfigInfo.enKeySize = ADVANCEDCA_KEYSIZE_128; 
          memcpy(stCipherEngineInfo.stKeyConfigInfo.u8Key, p_u8key, ADVANCEDCA_MAXKEYSIZE);
          //stCipherEngineInfo.enBlockMode    = ADVANCEDCA_BLOCKMODE_CBC;
          while(i < p_in->payload)
          {
            if((p_in->payload - i) > CIPHER_DATA_LENGTH)
            {
              stCipherEngineInfo.u32Length      = CIPHER_DATA_LENGTH;
            }
            else
            {
              stCipherEngineInfo.u32Length    = p_in->payload - i;
            }
            stCipherEngineInfo.pu8Input       = p_in->p_data + i;
            stCipherEngineInfo.pu8Output      = p_in->p_data + i;
            if(ADVANCEDCA_BLOCKMODE_CBC == stCipherEngineInfo.enBlockMode)
            {
              memcpy(stCipherEngineInfo.u8IV, stCipherEngineInfo_u8IV, 16);
              memcpy(stCipherEngineInfo_u8IV,
                     stCipherEngineInfo.pu8Input + stCipherEngineInfo.u32Length - 16, 
                     16);
            }
            hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVRPlayback, &stCipherEngineInfo);
            hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVRPlayback);
            i = i + CIPHER_DATA_LENGTH;
          }
          break;
      case ADVANCEDCA_CIPHERENGINE_DES:
          stCipherEngineInfo.enCipherEngine = ADVANCEDCA_CIPHERENGINE_DES;
          stCipherEngineInfo.enBlockMode    = ADVANCEDCA_BLOCKMODE_ECB;
          stCipherEngineInfo.u32Length      = p_in->payload;
          stCipherEngineInfo.pu8Input       = p_in->p_data;
          stCipherEngineInfo.pu8Output      = p_in->p_data;
          hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVRPlayback, &stCipherEngineInfo);
          hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVRPlayback);
          break;
      case ADVANCEDCA_CIPHERENGINE_TDEA:
          stCipherEngineInfo.enCipherEngine = ADVANCEDCA_CIPHERENGINE_TDEA;
          stCipherEngineInfo.enBlockMode    = ADVANCEDCA_BLOCKMODE_ECB;
          stCipherEngineInfo.u32Length      = p_in->payload;
          stCipherEngineInfo.pu8Input       = p_in->p_data;
          stCipherEngineInfo.pu8Output      = p_in->p_data;
          hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVRPlayback, &stCipherEngineInfo);
          hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVRPlayback);
          break;
      case 6:
        {
           
          memcpy(stCipherEngineInfo.stKeyConfigInfo.u8Key, p_u8key, ADVANCEDCA_MAXKEYSIZE);
          {
            stCipherEngineInfo.u32Length      = p_in->payload;
            stCipherEngineInfo.pu8Input       = p_in->p_data;
            stCipherEngineInfo.pu8Output      = p_in->p_data;
            hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVRPlayback, &stCipherEngineInfo);
            hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVRPlayback);
          }          
        }
        break;
      case ADVANCEDCA_CIPHERENGINE_NONE:
      default:
          break;
  }
#endif
}

static RET_CODE split_transform(handle_t _this, media_sample_t *p_in)
{
  ts_player_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)p_priv->p_this;
  u32 read_pos = 0;
  os_msg_t msg = {0};

  if ((p_in->payload % TS_PACKET_LEN))
  {
    OS_PRINTF("\n##PLAYER_MSG_PLAY_ERROR!!!\n");
    if (p_priv->sample_request == SAMPLE_REQUEST_SEEK)
    {
      p_priv->sample_request = 0;
      mtos_sem_give(&p_priv->seek_sem);
    }
    msg.content = PLAYER_MSG_PLAY_ERROR;
    msg.para1 = 0;
    msg.para2 = 0;
    p_ifilter->send_message_out(p_ifilter, &msg);
    return ERR_FAILURE;
  }
  
  switch (p_priv->sample_request)
  {
  case SAMPLE_REQUEST_SEEK:
    if (p_in->context)
    {
      p_priv->sample_request = 0;
      p_priv->play_end = 0;
      p_in->context = SAMPLE_CONTEXT_SEEK_FLAG;
      p_priv->seek_end_check = 1;
      //reload idx after do seek
      read_pos = (u32)p_in->p_user_data;

      if(p_priv->cfg.player_src == TS_PLAYER_SRC_REC_FILE)
      {
        media_idx_reload(p_priv->cfg.p_media_idx, read_pos / TS_STREAM_RW_UNIT);
      }
      //update play pos
      p_priv->last_play_pos = read_pos / TS_STREAM_RW_UNIT;
      p_priv->last_play_time = 0;
      mtos_sem_give(&p_priv->seek_sem);
      OS_PRINTF("\n##seek data come!!!\n");
    }
    else
    {
      OS_PRINTF("\n##debug: not seek data do not push down!!!\n");
      return SUCCESS;
    }
    break;
  case SAMPLE_REQUEST_TRICK_MODE_AV_RESET:
    if (p_in->context)
    {
      p_priv->sample_request = 0;
      p_in->context = 0;
      OS_PRINTF("\n##debug: trick reset data come!!!\n");
    }
    else
    {
      OS_PRINTF("\n##debug: not trick reset data do not push down!!!\n");
      return SUCCESS;
    }
    break;
  default:
    break;
  }

  if (get_dy_rec_info(p_priv, p_in) == ERR_FAILURE)
  {
    msg.content = PLAYER_MSG_PLAY_ERROR;
    msg.para1 = 0;
    msg.para2 = 0;
    p_ifilter->send_message_out(p_ifilter, &msg);
    return ERR_FAILURE;
  }
  if (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_FILE)
  {
    data_cipher(p_in);      
  } 
  if ((p_in->payload == 0)
    || (p_in->state == SAMP_STATE_INSUFFICIENT))  //data no enough
  {
    if (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_BLOCK) //timeshift mode
    {
      push_down(p_priv, p_in);
      //fix me some opt
      if (p_priv->play_mode == TS_SEQ_FAST_PLAY_2X
        || p_priv->play_mode == TS_SEQ_FAST_PLAY_4X
        || p_priv->play_mode == TS_SEQ_FAST_PLAY_8X
        || p_priv->play_mode == TS_SEQ_FAST_PLAY_16X
        || p_priv->play_mode == TS_SEQ_FAST_PLAY_32X
        || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_2X
        || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_4X
        || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_8X
        || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_16X
        || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_32X)
      {
        p_priv->player_running_state = PLAYER_RUNNING_CHECK_PLAY_MODE_TO_NORMAL;
        p_priv->last_sample = 0;
        p_priv->play_end_check = 0;
#ifndef PLAY_TIME_TASK_ENABLE
        p_ifilter->set_active_enter(p_priv->p_this, update_play_time);
#endif
      }
      else
      {
        trans_req_sample(p_priv, &p_in->format, 0, 0);
      }
      return SUCCESS;
    }
    else
    {
      if ((p_in->state == SAMP_STATE_INSUFFICIENT)
        && (p_priv->player_running_state == PLAYER_RUNNING_NORMAL)
        && (!(p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_2X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_4X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_8X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_16X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_32X)))
      {
        OS_PRINTF("\n##debug: last sample come !!!!\n");
        p_priv->player_running_state = PLAYER_RUNNING_CHECK_PLAY_END;
        p_priv->last_sample = 0;
        p_priv->play_end_check = 0;
#ifndef PLAY_TIME_TASK_ENABLE
        p_ifilter->set_active_enter(p_priv->p_this, update_play_time);
#endif
        push_down(p_priv, p_in);
      }
      return SUCCESS;
    }

    //return ERR_FAILURE;
  }

  if (p_priv->bit_rate == 0)
  {
    p_priv->bit_rate = parse_bit_rate(p_in->p_data, p_in->payload, p_priv->rec_info.pcr_pid);
  }
  
  switch (p_priv->play_mode)
  {
  case TS_SEQ_NORMAL_PLAY:
    forward_transform(p_priv, p_in);
    break;

  case TS_SEQ_FAST_PLAY_2X:
  case TS_SEQ_FAST_PLAY_4X:
  case TS_SEQ_FAST_PLAY_8X:
  case TS_SEQ_FAST_PLAY_16X:
  case TS_SEQ_FAST_PLAY_32X:
    forward_transform(p_priv, p_in);
    break;

  case TS_SEQ_REV_FAST_PLAY_2X:
  case TS_SEQ_REV_FAST_PLAY_4X:
  case TS_SEQ_REV_FAST_PLAY_8X:
  case TS_SEQ_REV_FAST_PLAY_16X:
  case TS_SEQ_REV_FAST_PLAY_32X:
    backward_transform(p_priv, p_in);
    break;

  case TS_SEQ_SLOW_PLAY_2X:
  case TS_SEQ_SLOW_PLAY_4X:
    forward_transform(p_priv, p_in);
    break;

  case TS_SEQ_REV_SLOW_PLAY_2X:
  case TS_SEQ_REV_SLOW_PLAY_4X:
    backward_transform(p_priv, p_in);
    break;

  default:
    break;
  }

  if (((p_priv->player_running_state == PLAYER_RUNNING_CHECK_PLAY_END)
        && (p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_2X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_4X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_8X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_16X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_32X))
     || ((p_priv->player_running_state == PLAYER_RUNNING_CHECK_PLAY_MODE_TO_NORMAL)
         && (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_FILE)
         && (!(p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_2X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_4X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_8X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_16X
            || p_priv->play_mode == TS_SEQ_REV_FAST_PLAY_32X)))
      || ((p_priv->player_running_state == PLAYER_RUNNING_CHECK_PLAY_MODE_TO_NORMAL)
            && (p_priv->cfg.player_src == TS_PLAYER_SRC_REC_BLOCK)
            && (p_priv->play_mode == TS_SEQ_NORMAL_PLAY)))
  {
    p_priv->player_running_state = PLAYER_RUNNING_NORMAL;
    p_priv->last_sample = 0;
    p_priv->play_end_check = 0;
#ifndef PLAY_TIME_TASK_ENABLE
    p_ifilter->set_active_enter(p_priv->p_this, NULL);
  }
  
  if ((p_priv->player_running_state == PLAYER_RUNNING_NORMAL) && !p_priv->sample_request)
  {
    update_play_time(_this);
#endif
  }

  return SUCCESS;
}

static RET_CODE on_open(handle_t _this)
{
  ts_player_filter_private_t *p_priv = get_priv(_this);
  video_codec_type_t type = MPEG_TYPE_CODE;

  p_priv->pid_changed = 0;
  p_priv->sample_request = 0;
  p_priv->last_play_time = 0;
  p_priv->last_play_pos = 0;
  p_priv->last_sample = 0;
  p_priv->play_end_check = 0;
  p_priv->play_end = 0;
  p_priv->seek_end_check = 0;
  p_priv->player_running_state = PLAYER_RUNNING_NORMAL;
  p_priv->trick_mode_skip = 0;
  p_priv->bit_rate = 0;
  
  p_priv->play_mode = TS_SEQ_NORMAL_PLAY;
  p_priv->ts_seq_direction = 1;

  p_priv->p_ts_seq = ts_seq_init(NULL, 0, 
    TRUE, p_priv->cfg.p_buffer, p_priv->cfg.buffer_size);
  if (p_priv->p_ts_seq)
  {
    switch (p_priv->cfg.video_fmt)
    {
    case VIDEO_MPEG:
    case VIDEO_MPEG_ES:
      type = MPEG_TYPE_CODE;
      break;
    case VIDEO_H264:
    case VIDEO_H264_ES:
      type = H264_TYPE_CODE;
      break;
    case VIDEO_AVS:
    case VIDEO_AVS_ES:
      type = AVS_TYPE_CODE;
      break;
    default:
      OS_PRINTF("\n##debug: do not support video fmt!\n");
      break;
    }
    ts_seq_set_sequence_parser(p_priv->p_ts_seq, type);
  }
  else
  {
    OS_PRINTF("\n##debug: ts_seq_init fail!!\n");
  }

  if (!mtos_sem_create(&p_priv->seek_sem, FALSE))
  {
    OS_PRINTF("\n##debug: mtos_sem_create fail!!\n");
  }

  if (!mtos_sem_create(&p_priv->seek_req_sem, TRUE))
  {
    OS_PRINTF("\n##debug: mtos_sem_create fail!!\n");
  }
  
  p_priv->p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(p_priv->p_video_dev != NULL);
  
#ifdef DUMP_TRICK_MODE_FILE
{
  u16 test_name[16] = {0};

  str_asc2uni("C:\\test_001.ts", test_name);
  test_name[10] = 0x30 + 1;

  test_file = vfs_open(test_name, VFS_NEW);
  
  if(test_file == NULL)
  {
    OS_PRINTF("open file failed\n");
    return ERR_FAILURE;
  }
}
#endif
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  memset(stCipherEngineInfo_u8IV, 0xF0, 16);
#endif
  return SUCCESS;
}

static RET_CODE on_start(handle_t _this)
{
  ts_player_filter_private_t *p_priv = get_priv(_this);
  media_format_t format = {0};
#ifdef PLAY_TIME_TASK_ENABLE
  ifilter_t *p_ifilter = (ifilter_t *)p_priv->p_this;
#endif

  p_priv->m_in_pin.request_sample(&p_priv->m_in_pin, &format,
    p_priv->cfg.load_unit_size, 0, 0);
#ifdef PLAY_TIME_TASK_ENABLE
  p_ifilter->set_active_enter(p_priv->p_this, update_play_time);
#endif
  return SUCCESS;
}

static RET_CODE on_stop(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  ts_player_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)p_priv->p_this;

  if(p_priv->play_mode != TS_SEQ_NORMAL_PLAY)
  {
    aud_mute_onoff_vsb(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO), FALSE);
  }
  
  p_ifilter->set_active_enter(_this, NULL);

  if (p_priv->p_ts_seq)
  {
    ts_seq_stop(p_priv->p_ts_seq);
    ts_seq_deinit(p_priv->p_ts_seq);
    p_priv->p_ts_seq = NULL;
  }

  if (p_priv->seek_sem)
  {
    mtos_sem_destroy(&p_priv->seek_sem, 0);
    p_priv->seek_sem = 0;
  }

  if (p_priv->seek_req_sem)
  {
    mtos_sem_destroy(&p_priv->seek_req_sem, 0);
    p_priv->seek_req_sem = 0;
  }
#ifdef DUMP_TRICK_MODE_FILE
  if (test_file)
  {
    vfs_close(test_file);
    test_file = NULL;
  }
#endif
  return SUCCESS;
}

static void on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}

static RET_CODE on_evt(handle_t _this, os_msg_t *p_msg)
{
  ts_player_filter_private_t *p_priv = get_priv(_this);
  
  switch (p_msg->content)
  {
    case TS_PLAYER_CMD_SET_PLAY_MODE:
      set_play_mode(p_priv, p_msg->para1, (*(u32 *)p_msg->para2), 0);
      break;
      
    default:
      break;
  }
  
  return SUCCESS;
}

ifilter_t *ts_player_filter_create(void *p_para)
{
  ts_player_filter_private_t *p_priv = NULL;
  ts_player_filter_t *p_ins = NULL;
  interface_t *p_interface = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_t *p_transffilter = NULL;

  //check input parameter
  p_ins = mtos_malloc(sizeof(ts_player_filter_t));
  MT_ASSERT(p_ins != NULL);
  memset(p_ins, 0, sizeof(ts_player_filter_t));

  //create base class
  transf_filter_create(&p_ins->m_filter, NULL);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(ts_player_filter_private_t));
  p_priv->p_this = p_ins; //this point

  //over loading the virtual function for ifilter_t
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = on_command;
  p_ifilter->on_open = on_open;
  p_ifilter->on_start = on_start;
  p_ifilter->on_stop = on_stop;
  p_ifilter->on_close = on_close;
  p_ifilter->on_process_evt = on_evt;

  //over loading the virtual function for transf_filter_t
  p_transffilter = (transf_filter_t *)p_ins;
  p_transffilter->transform = split_transform;

  p_interface = (interface_t *)p_ins;
  p_interface->on_destroy = on_destory;
  ts_player_in_pin_create(&p_priv->m_in_pin, p_interface);
  ts_player_out_pin_create(&p_priv->m_out_pin, p_interface);

  return (ifilter_t *)p_ins;
}

/*!
  end of file
  */

