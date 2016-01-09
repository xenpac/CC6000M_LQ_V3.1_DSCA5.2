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
//util
#include "class_factory.h"
#include "simple_queue.h"

#include "data_manager.h"
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
#include "ifilter.h"
#include "transf_filter.h"
#include "eva_filter_factory.h"

#include "mp3_transfer_output_pin_intra.h"
#include "mp3_transfer_input_pin_intra.h"
#include "mp3_transfer_lrc_output_pin_intra.h"
#include "mp3_transfer_pic_output_pin_intra.h"
#include "mp3_decode_frame.h"

#include "mp3_player_filter.h"
#include "mp3_transfer_filter_intra.h"

#include "aud_vsb.h"
#include "drv_dev.h"
#include "dmx.h"
#include "err_check_def.h"

static mp3_trans_filter_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((mp3_trans_filter_t *)_this)->private_data;
}

static void mp3_loop_trans_time(handle_t _this)
{

    u32 play_time = 0;
    void *p_audio_dev = NULL;
    aud_info_vsb_t aud_info = {0};
    u32 frame_cunt = 0;

    media_sample_t *p_out_sample = NULL;
    mp3_trans_filter_private_t *p_priv = get_priv(_this);

    transf_output_pin_t *p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_lrc_out_pin;
    CHECK_FAIL_RET_VOID(p_transf_out_pin != NULL);

    p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    aud_get_info_vsb(p_audio_dev,(u32)&aud_info);


    frame_cunt = aud_info.frame_cnt;
    p_priv->cur_frame_cnt = frame_cunt;

    //for jump time,culculate frame count which jump
    if(p_priv->jump == TRUE)
    {
      if(frame_cunt >= p_priv->save_frame_cnt)
      {
          //current time + add time
          frame_cunt =p_priv->jump_cur_frame +  (frame_cunt - p_priv->save_frame_cnt);
      }
      else
      {
         frame_cunt =p_priv->jump_cur_frame;
      }
    }

    //calculate time
    CHECK_FAIL_RET_VOID(p_priv->p_mp3_info != NULL);
    if((p_priv->p_mp3_info->frameinfo.mBitrateType == 0)
       ||(p_priv->p_mp3_info->frameinfo.mBitrateType == 1))
    {
       play_time = 0.026 * frame_cunt;  //every frame play 0.026sec
    }
    else if((p_priv->p_mp3_info->frameinfo.mBitrateType == 2)
             ||(p_priv->p_mp3_info->frameinfo.mBitrateType == 3))
    {
      CHECK_FAIL_RET_VOID(p_priv->p_mp3_info->frameinfo.mSamplerate != 0);

      play_time = frame_cunt
                  *p_priv->p_mp3_info->frameinfo.mSamplesPerFrame
                  /p_priv->p_mp3_info->frameinfo.mSamplerate;
    }


   if(play_time != p_priv->cur_play_time)
   {
    ifilter_t *p_mp3_filter = NULL;
    os_msg_t msg = {0};
    msg.content = AUDIO_GET_PLAY_TIME;
    msg.para1 = play_time;
    p_mp3_filter = (ifilter_t *)p_priv->p_this;
    p_mp3_filter->send_message_out(p_mp3_filter,&msg);

    p_priv->cur_play_time = play_time;

    if(p_priv->b_music_lrc_show == TRUE)
    {
      p_transf_out_pin->query_sample(p_transf_out_pin, &p_out_sample);
      if(p_out_sample == NULL)
        return ;

      p_out_sample->data_offset = play_time;
      p_out_sample->format.stream_type = MT_AUDIO;
      if(p_transf_out_pin->push_down(p_transf_out_pin, p_out_sample) != SUCCESS)
      {
         p_transf_out_pin->release_sample(p_transf_out_pin, p_out_sample);
      }
    }

   }

   //OS_PRINTF("\n\n %d \n\n",p_out_sample->data_offset);
}


static RET_CODE mp3_trans_on_command(handle_t _this, icmd_t *p_cmd)
{
  mp3_trans_filter_private_t *p_priv = NULL;
  u32 *p_time = NULL;

  p_priv = get_priv(_this);

  if(p_priv->play_mode == MP3_PLAY_PAUSE)
  {
    if(p_cmd->lpara == MP3_FAST_FORWARD || p_cmd->lpara == MP3_FAST_BACKWARD)
      return SUCCESS;
  }
  switch (p_cmd->cmd)
  {
  case MP3_CFG_NAME:
    p_priv->p_file_name = p_cmd->p_para;
  break;
  case MP3_CFG_HEAD_INFO:
    p_priv->p_mp3_info = p_cmd->p_para;
  break;
  case MP3_PUSH_LOGO:
    if(p_priv->p_mp3_info == NULL)
      break;

    if(p_priv->b_music_logo_show == TRUE)
    {
      media_sample_t *p_out_sample = NULL;
      transf_output_pin_t *p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_pic_out_pin;

      if(p_transf_out_pin != NULL)
      {
        p_transf_out_pin->query_sample(p_transf_out_pin, &p_out_sample);
        if(p_out_sample == NULL)
        {
          return ERR_FAILURE;
          }

        if(p_priv->p_mp3_info->p_pic_buf != NULL)
        {
          p_out_sample->p_data = p_priv->p_mp3_info->p_pic_buf;
          p_out_sample->payload = p_priv->p_mp3_info->pbuf_len;
        }
        else
        {
          u32 addr = 0;
          u32 size = 0;
          void *p_pic_logo =  class_get_handle_by_id(DM_CLASS_ID);

          if(p_pic_logo)
          {
              addr = dm_get_block_addr(p_pic_logo, LOGO_MUSIC_ID);
              size = dm_get_block_size(p_pic_logo, LOGO_MUSIC_ID);
          }

          if((size < 20 * 1024) && (size > 0))
          {
            memcpy(p_out_sample->p_data, (u8 *)addr, size);
            p_out_sample->payload = size;
          }
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

      }

    }

  break;
  case MP3_PLAYER_CFG:
    p_priv->play_mode = p_cmd->lpara;
    p_time = p_cmd->p_para;

    switch (p_priv->play_mode)
    {
    case MP3_TIME_SEEK:
      p_priv->timejump = (*p_time);
      break;
    case MP3_PLAY_RESUME:
      p_priv->play_mode = MP3_NORMAL_PLAY;
      break;
    case MP3_PLAY_PAUSE:
      p_priv->play_mode = p_priv->play_mode;
      break;
    default:

      break;
    }
    break;

  case MP3_SHOW_LOGO:
    p_priv->b_music_logo_show = p_cmd->lpara;
  break;
  case MP3_SHOW_LRC:
    p_priv->b_music_lrc_show = p_cmd->lpara;
  break;

  case MUSIC_CFG_URL:
    p_priv->p_src_url = p_cmd->p_para;
  break;

  default:
    return ERR_FAILURE;
  }
  return SUCCESS;
}

static void request_sample_seek(handle_t in_pin, media_sample_t *p_sample, u32 jump_data)
{
  ipin_t *p_ipin = (ipin_t *)in_pin;
  ipin_t *p_connecter = (ipin_t *)p_ipin->get_connected(in_pin);
  iasync_reader_t *p_reader = NULL;

  p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE,
    (void **)&p_reader);

  p_reader->request(p_reader, &p_sample->format, SIZEREQ, 0, jump_data, NULL, 1);
  //p_reader->request(p_reader, &p_sample->format, SIZEREQ, 0, 0, 0);
}

RET_CODE mp3_demux(handle_t _this, media_sample_t *p_in)
{
  mp3_trans_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
#ifndef WIN32
  media_sample_t *p_out_sample = NULL;
  u32 push_unit = p_in->total_buffer_size / 10;
  u32 push_cnt = 0;
  u32 loopi = 0;

  if (!((ipin_t *)p_out_pin)->is_connected(p_out_pin))
  {
    return SUCCESS;
  }
  if ((!p_in->payload) && (SAMP_STATE_INSUFFICIENT != p_in->state))
  {
    return SUCCESS;
  }
  if(0 == p_in->payload){
    push_cnt = 1;
   }else{
      push_cnt= (((p_in->payload - 1) / push_unit) + 1);
   }
  for (loopi = 1; loopi <= push_cnt; loopi ++)
  {
    if (loopi < push_cnt)
    {
      p_out_pin->query_sample(p_out_pin, &p_out_sample);
      CHECK_FAIL_RET_CODE(p_out_sample != NULL);
      p_out_sample->p_data = p_in->p_data + (loopi - 1) * push_unit;
      p_out_sample->payload = push_unit;
      p_out_pin->push_down(p_out_pin, p_out_sample);
    }
    else
    {
      p_in->p_data = p_in->p_data + (loopi - 1) * push_unit;
      p_in->payload = p_in->payload - (loopi - 1) * push_unit;
      p_out_pin->push_down(p_out_pin, p_in);
    }
  }
 #else
        p_out_pin->push_down(p_out_pin, p_in);
 #endif
  return SUCCESS;
}

RET_CODE mp3_seek_time_transform(handle_t _this, media_sample_t *p_in)
{
  mp3_trans_filter_private_t *p_priv = get_priv(_this);
  mp3_trans_in_pin_t *p_in_pin = &p_priv->m_in_pin;
  u32 jump_data = 0;
  s32 jump_cunt = 0;
  u32 jump_time = 0;
  //u32 vbr_index = 0;
  //s32 st = 0;

  p_priv->save_frame_cnt = p_priv->cur_frame_cnt;

  if (p_priv->p_mp3_info->mDuration * 1000 <= p_priv->timejump)
    p_priv->timejump = 0;

  jump_time = p_priv->timejump / 1000;

  if((p_priv->p_mp3_info->frameinfo.mBitrateType == 0)||
  (p_priv->p_mp3_info->frameinfo.mBitrateType == 1))
  {
    jump_data = p_priv->timejump * p_priv->p_mp3_info->frameinfo.mBitrate
      / 8 + p_priv->p_mp3_info->id3v2_len + 10;

    jump_cunt = jump_time / 0.026;
  }
  else if((p_priv->p_mp3_info->frameinfo.mBitrateType == 2)||
  (p_priv->p_mp3_info->frameinfo.mBitrateType == 3))
  {
#if 1
    jump_data = (p_priv->timejump / 1000) * (p_priv->p_mp3_info->vbrinfo.mTotalBytes)
                / (p_priv->p_mp3_info->mDuration) ;
#else
    vbr_index = p_priv->timejump *100 /p_priv->p_mp3_info->mDuration;
    if(vbr_index >=0 && vbr_index <= 100)
      jump_data = p_priv->TOC[vbr_index] * p_priv->p_mp3_info->mFileSize;
#endif

    jump_cunt = jump_time
                * (p_priv->p_mp3_info->frameinfo.mSamplerate)
                / p_priv->p_mp3_info->frameinfo.mSamplesPerFrame;
  }

  if(p_priv->timejump == 0)
  {
       jump_data = 0;
  }

  request_sample_seek(p_in_pin, p_in, jump_data);
  p_priv->buff_played = p_priv->buff_played + SIZEREQ + jump_data;
  p_priv->play_mode = MP3_NORMAL_PLAY;
  p_priv->jump_cur_frame = jump_cunt;
  p_priv->jump = TRUE;


  return SUCCESS;
}

RET_CODE mp3_trans_transform(handle_t _this, media_sample_t *p_in)
{
  mp3_trans_filter_private_t *p_priv = get_priv(_this);
  mp3_trans_in_pin_t *p_in_pin = &p_priv->m_in_pin;
  s32 st = 0;


  switch (p_priv->play_mode)
  {
  case MP3_TIME_SEEK:
    return mp3_seek_time_transform(_this,p_in);

  case MP3_FAST_FORWARD:
    if(p_priv->cnt % FORWARDSPEED != 0)
    {
      st = mp3_demux(_this,p_in);
      if(!st)
      {
       //request_next_sample
        p_in_pin->request_next_sample(p_in_pin, p_in);
        p_priv->buff_played += SIZEREQ;
      }
    }
    else
    {
      p_in_pin->request_next_sample(p_in_pin, p_in);
      p_priv->buff_played += SIZEREQ;
    }
    p_priv->cnt ++;
    if(p_priv->cnt == CNTMAX)
      p_priv->cnt = 0;
    break;

  case MP3_FAST_BACKWARD:
    if(p_priv->cnt % FORWARDSPEED != 0)
    {
      st = mp3_demux(_this,p_in);
      if(!st)
      {
        if(p_priv->buff_played <= SIZEREQ)
        {
          p_priv->buff_played = p_priv->p_mp3_info->mFileSize - SIZEREQ;
        }
        else
        {
          p_priv->buff_played = p_priv->buff_played - SIZEREQ;
        }
        request_sample_seek(p_in_pin, p_in, p_priv->buff_played);
      }
    }
    else
    {
      if(p_priv->buff_played <= SIZEREQ)
      {
        p_priv->buff_played = p_priv->p_mp3_info->mFileSize - SIZEREQ;
      }
      else
      {
        p_priv->buff_played = p_priv->buff_played - SIZEREQ;
      }
      request_sample_seek(p_in_pin, p_in, p_priv->buff_played);
    }
    p_priv->cnt ++;
    if(p_priv->cnt == CNTMAX)
      p_priv->cnt = 0;
    break;

  default:

    if(SAMP_STATE_FAIL == p_in->state)
    {
        ifilter_t *p_mp3_filter = NULL;
        os_msg_t msg = {0};

        msg.content = AUDIO_GET_DATA_ERROR;
        p_mp3_filter = (ifilter_t *)p_priv->p_this;
        p_mp3_filter->send_message_out(p_mp3_filter,&msg);

        return ERR_FAILURE;
    }

    if((p_priv->is_push == FALSE) && (p_priv->b_music_logo_show) && p_priv->first_frame)
    {
      media_sample_t *p_out_sample = NULL;
      transf_output_pin_t *p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_pic_out_pin;

      if(p_transf_out_pin != NULL)
      {
        p_transf_out_pin->query_sample(p_transf_out_pin, &p_out_sample);
        if(p_out_sample == NULL)
        {
          return ERR_FAILURE;
          }

        if(p_priv->p_mp3_info->p_pic_buf != NULL)
        {
          p_out_sample->p_data = p_priv->p_mp3_info->p_pic_buf;
          p_out_sample->payload = p_priv->p_mp3_info->pbuf_len;
        }
        else
        {
          u32 addr = 0;
          u32 size = 0;
          void *p_pic_logo =  class_get_handle_by_id(DM_CLASS_ID);

          if(p_pic_logo)
          {
              addr = dm_get_block_addr(p_pic_logo, LOGO_MUSIC_ID);
              size = dm_get_block_size(p_pic_logo, LOGO_MUSIC_ID);
          }

          if((size < 20 * 1024) && (size > 0))
          {
            memcpy(p_out_sample->p_data , (u8 *)addr, size);
            p_out_sample->payload = size;
          }
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

        p_priv->is_push = TRUE;
      }

    }

    p_priv->first_frame = FALSE;

    st = mp3_demux(_this,p_in);
    if((!st) && (p_in->format.stream_type == MT_UNKNOWN)) //if data from network, do not request
    {
     //request_next_sample
      p_in_pin->request_next_sample(p_in_pin, p_in);
      p_priv->buff_played += SIZEREQ;
    }
    return SUCCESS;
  }
  return SUCCESS;
}

RET_CODE mp3_trans_on_open(handle_t _this)
{
  mp3_trans_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)_this;

  media_sample_t sample = {{0}};
  mp3_trans_in_pin_t *p_in_pin = &p_priv->m_in_pin;

  p_priv->first_frame = TRUE;

  p_ifilter->set_active_enter(p_ifilter, mp3_loop_trans_time);
  //request first data
  if(p_in_pin != NULL)
  {
    memset(&sample, 0, sizeof(media_sample_t));
    sample.p_user_data = (void *)p_priv->p_src_url;
    if(p_priv->play_mode == MP3_TIME_SEEK)
    {
        mp3_seek_time_transform(_this, &sample);
    }
    else
    {
        p_in_pin->request_next_sample(p_in_pin, &sample);
    }
  }
  return SUCCESS;
}

RET_CODE mp3_trans_on_close(handle_t _this)
{
  mp3_trans_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)_this;

  p_ifilter->set_active_enter(p_ifilter, NULL);

  p_priv->is_push = FALSE;


  p_priv->cur_frame_cnt = 0;
  p_priv->save_frame_cnt = 0;
  p_priv->jump = 0;  
  p_priv->jump_cur_frame = 0;  

  return SUCCESS;
}

static void mp3_trans_on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}


ifilter_t * mp3_transfer_filter_create(void *p_para)
{
  mp3_trans_filter_private_t *p_priv = NULL;
  mp3_trans_filter_t *p_ins = NULL;
  transf_filter_t *p_transf_filter = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_para_t transf_filter_para;

  transf_filter_para.dummy = 0;

  //create filter
  p_ins = mtos_malloc(sizeof(mp3_trans_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(mp3_trans_filter_t));

  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);

  //init private date
  p_priv = &p_ins->private_data;
  p_priv->p_this = p_ins; //this point
  p_priv->play_mode = MP3_NORMAL_PLAY;
  p_priv->p_mp3_info = NULL;
  p_priv->cnt = 0;
  p_priv->timejump = 0;
  p_priv->buff_played = SIZEREQ;
  p_priv->b_music_logo_show = TRUE;
  p_priv->b_music_lrc_show = TRUE;
  //memset(p_priv->TOC, 0, sizeof(p_priv->TOC));
  //init member function

  //over loading the virtual function
  p_transf_filter = (transf_filter_t *)p_ins;
  //p_transf_filter->in_place_transform = mp3_trans_transform;
  p_transf_filter->transform = mp3_trans_transform;

  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = mp3_trans_on_command;
  p_ifilter->on_open = mp3_trans_on_open;
  p_ifilter->on_close = mp3_trans_on_close;
  p_ifilter->_interface.on_destroy = mp3_trans_on_destory;

  //create it's pin
  mp3_trans_in_pin_create(&p_priv->m_in_pin, (interface_t *)p_ins);
  mp3_trans_out_pin_create(&p_priv->m_out_pin, (interface_t *)p_ins);

  mp3_trans_lrc_out_pin_create(&p_priv->m_lrc_out_pin, (interface_t *)p_ins);
  mp3_trans_pic_out_pin_create(&p_priv->m_pic_out_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}
