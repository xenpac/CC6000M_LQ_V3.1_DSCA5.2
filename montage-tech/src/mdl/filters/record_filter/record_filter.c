/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_mutex.h"
#include "mtos_event.h"
#include "mtos_misc.h"
#include "mtos_task.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "lib_util.h"
#include "common.h"
#include "drv_dev.h"
#include "dmx.h"
#include "vdec.h"

#include "vfs.h"
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
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "transf_input_pin.h"
#include "transf_output_pin.h"
#include "ifilter.h"
#include "eva_filter_factory.h"

#include "transf_filter.h"
#include "mt_time.h"
#include "file_sink_filter.h"
#include "record_input_pin_intra.h"
#include "record_output_pin_intra.h"
#include "record_filter.h"
#include "record_filter_intra.h"

#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_dmx_filter_rec_pin_imp.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
//CA advanced
#include "Hal_secure.h"
#endif
#ifdef WIN32
//#include "SDL.h"
extern SDL_GetTicks();
#define GET_MTOS_TICKS (SDL_GetTicks() / 10)
#else
#define GET_MTOS_TICKS (mtos_ticks_get())
#endif

//#define REC_BUFFER_HEAD_PAYLOAD  (512 * 47) //512*188/4 = 23.5k

/*!
  pts offset max, 1s
  */
#define REC_PTS_OFFSET_MAX  (45000)

#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
static u8 stCipherEngineInfo_u8IV[16];
extern int mul_pvr_get_cipher_key(u8 *p_engine, u8 **p_key, u8 *p_keylength);
#endif
static rec_filter_private_t *get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((record_filter_t *)_this)->private_data;
}
/*
static ifilter_t *get_fsink(handle_t _this)
{
  rec_filter_private_t *p_priv = get_priv(_this);
  ipin_t *p_pin = (ipin_t *)&p_priv->m_out_pin;
  ipin_t *p_to = (ipin_t *)p_pin->get_connected(p_pin);

  return (ifilter_t *)p_to->get_filter(p_to);
}
*/

static RET_CODE on_command(handle_t _this, icmd_t *p_cmd)
{
  rec_filter_private_t *p_priv = get_priv(_this);
  static_rec_info_t *p_pg = NULL;
  rec_filter_pg_pid_t *p_pg_pid = NULL;
  
  switch (p_cmd->cmd)
  {
  case RECORD_CFG_PROGRAM:
    p_pg = (static_rec_info_t *)p_cmd->p_para;
    memcpy(&p_priv->st_rec_info, p_pg, sizeof(static_rec_info_t));
    break;
    
  case RECORD_CFG_PROGRAM_PID:
    p_pg_pid = (rec_filter_pg_pid_t *)p_cmd->p_para;
    memset(&p_priv->dy_rec_info, 0, sizeof(dynamic_rec_info_t));
    p_priv->dy_rec_info.v_pid = p_pg_pid->v_pid;
    p_priv->dy_rec_info.a_pid = p_pg_pid->a_pid;
    p_priv->dy_rec_info.pcr_pid = p_pg_pid->pcr_pid;
    p_priv->dy_rec_info.video_fmt = p_pg_pid->video_fmt;
    p_priv->dy_rec_info.audio_fmt = p_pg_pid->audio_fmt;
    p_priv->last_pts = 0;
    p_priv->record_mode = p_pg_pid->record_mode;
    break;

  case RECORD_FILTER_CMD_TIMESHIFT_CFG:
    p_priv->p_media_array = (media_idx_t *)p_cmd->p_para;
    p_priv->timeshift_mode = p_cmd->lpara;
    //MT_ASSERT(p_priv->p_media_array != NULL);
    break;
    
  default:
    return ERR_FAILURE;
  }
  return SUCCESS;
}


static RET_CODE on_evt(handle_t _this, os_msg_t *p_msg)
{
  ifilter_t *p_ifilter = (ifilter_t *)_this;
  
  switch(p_msg->content)
  {
    case FILE_SINK_MSG_NO_ENOUGH_MEM:
      //no space, stop it
      p_ifilter->on_stop(p_ifilter);
      break;
    default:
      break;
  }
  return SUCCESS;
}

/*static void insert_start_tag(rec_filter_private_t *p_priv)
{
  transf_output_pin_t *p_transf_out_pin = NULL;
  media_sample_t *p_output_sample = NULL;
  u8 *p_head = mtos_malloc(REC_BUFFER_HEAD_PAYLOAD);

  if(p_head == NULL)
  {
    //send msg out????? //fix me
    return;
  }

  p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  p_transf_out_pin->query_sample(p_transf_out_pin, &p_output_sample);
  MT_ASSERT(NULL != p_output_sample);
  
  packet_dummy_info(p_head, REC_BUFFER_HEAD_PAYLOAD);

  p_output_sample->p_data = p_head;
  p_output_sample->payload = REC_BUFFER_HEAD_PAYLOAD;
  
  packet_rec_info(p_output_sample->p_data,
    REC_INFO_STATIC, (u8 *)(&p_priv->st_rec_info), sizeof(static_rec_info_t))

  p_transf_out_pin->push_down(p_transf_out_pin, p_output_sample);
 
  mtos_free(p_head);
}
*/
#if 0
static void insert_end_tag(handle_t _this)
{
  rec_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_transf_out_pin = NULL;
  media_sample_t *p_output_sample = NULL;
  ifilter_t *p_fsink = get_fsink(_this);
  s64 st_addr = 0; //point the static
  icmd_t cmd = {0};

  if (p_priv->timeshift_mode) // it's timeshift mode
  {
    return;
  }

  p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  p_transf_out_pin->query_sample(p_transf_out_pin, &p_output_sample);
  MT_ASSERT(NULL != p_output_sample);

  //change the head info
  p_priv->st_rec_info.during_time = p_priv->dy_rec_info.cur_time; //update total time

  //seek the head
  cmd.cmd = FSINK_SEEK;
  cmd.lpara = VFS_SEEK_SET;
  cmd.p_para = &st_addr;
  p_fsink->do_command(p_fsink, &cmd);
  
  //rewrite st data
  p_output_sample->payload = packet_rec_info(p_output_sample->p_data,
      REC_INFO_STATIC, (u8 *)(&p_priv->st_rec_info), sizeof(static_rec_info_t));
  p_transf_out_pin->push_down(p_transf_out_pin, p_output_sample);
}
#endif

static void trans_insert_time_by_pts(rec_filter_private_t *p_priv, u8 *p_buffer, u32 len)
{
  u32 cur_pts = 0;
  u32 pts_skip = 0;
  u32 cur_pos = 0; //from 0~len
  u32 cur_time = 0;
  u32 cur_tick_time = 0;
  s64 total_size = 0;
  u16 pts_pid = p_priv->dy_rec_info.v_pid;
  ifilter_t *p_ifilter = p_priv->p_this;
  os_msg_t msg = {0};
  BOOL b_lost_data = 0;
  video_codec_type_t type = MPEG_TYPE_CODE;
  ts_detail_info_t ts_info;
  u32 index = 0;
  
  /*if (p_priv->dy_rec_info.v_pid == 0)
  {
    pts_pid = p_priv->dy_rec_info.a_pid;
  }*/
  if ((p_priv->unit_pts_offset == 0) || (p_priv->unit_pts_offset >= REC_PTS_OFFSET_MAX))
  {
    p_priv->unit_pts_offset = parse_bit_rate(p_buffer, len, p_priv->dy_rec_info.pcr_pid);
    p_priv->unit_pts_offset = p_priv->unit_pts_offset >> 3;
    if (p_priv->unit_pts_offset)
    {
      p_priv->unit_pts_offset = (TS_STREAM_RW_UNIT * 1000) / p_priv->unit_pts_offset;
      p_priv->unit_pts_offset *= 45;
    }
    OS_PRINTF("\n##unit pts offset [%d]\n", p_priv->unit_pts_offset);
  }
  
  switch (p_priv->dy_rec_info.video_fmt)
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
    
  while(cur_pos < len)
  {
    cur_pts = parse_pts(p_buffer, TS_STREAM_RW_UNIT, pts_pid);
    if (!p_priv->timeshift_mode)
    {
      ts_seq_get_vkey_frame_vsb(p_buffer, TS_STREAM_RW_UNIT,
        type, p_priv->dy_rec_info.v_pid, &ts_info);
      if (ts_info.fragment_num)
      {
        index = p_priv->dy_rec_info.indx;
        p_priv->vkey_frame_offset = index * TS_STREAM_RW_UNIT + ts_info.fragment_offset[0];
      }
      p_priv->dy_rec_info.vkey_frame_offset = p_priv->vkey_frame_offset;
    }
    else
    {
      p_priv->dy_rec_info.vkey_frame_offset = 0;
    }

    if (!cur_pts)  //found pts
    {
      cur_pts = p_priv->last_pts + p_priv->unit_pts_offset;
    }
    
    if(cur_pts >= p_priv->last_pts)
    {
      if (p_priv->last_pts)
      {
        pts_skip = cur_pts - p_priv->last_pts;
      }
      else
      {
        pts_skip = 4500;
      }
    }
    else
    {
      pts_skip = p_priv->last_pts - cur_pts;
    }

    if (pts_skip > 45000)//over 1s. some error data
    {
      if ((p_priv->last_pts > cur_pts) && (pts_skip > 5 * 45000))
      {
        OS_PRINTF("\n##debug: ring stream total about[%d]s\n", pts_skip / 45000);
        pts_skip = p_priv->unit_pts_offset;
      }
      else if (p_priv->last_pts && (p_priv->last_pts < cur_pts))
      {
        if (((GET_MTOS_TICKS - p_priv->start_tick) * 10 - p_priv->skip_time)
          < (p_priv->last_rec_time + pts_skip / 45))
        {
          OS_PRINTF("\n##ts packet error, set rec lost [5ms]!\n");
          pts_skip = 225;
        }
        else
        {
          OS_PRINTF("\n##rec lost data, skip[%lu]ms!\n", pts_skip / 45);
          p_priv->skip_time += (pts_skip / 45);
          b_lost_data = TRUE;
        }
      }
    }

    cur_tick_time = (GET_MTOS_TICKS - p_priv->start_tick) * 10 - p_priv->skip_time;
    if (cur_pos == 0 || b_lost_data)
    {
      cur_time = cur_tick_time;  //to ms
      if (cur_time > (p_priv->last_rec_time + 1000))
      {
        cur_time = p_priv->last_rec_time + 1000;
      }
      b_lost_data = FALSE;
      if (cur_time >= p_priv->dy_rec_info.cur_time)
      {
        p_priv->dy_rec_info.cur_time = cur_time;
      }
    }
    else
    {
      cur_time += (pts_skip / 45);  //pts to ms = (pts_skip/45000 * 1000)
    }
    
    if (cur_time >= p_priv->dy_rec_info.cur_time)
    {
      p_priv->dy_rec_info.cur_time = cur_time;
    }
    else
    {
      p_priv->dy_rec_info.cur_time += 10;//ms
    }
    
    p_priv->last_pts = cur_pts;
    p_priv->dy_rec_info.pts = cur_pts;

    if (p_priv->dy_rec_info.cur_time >=  (p_priv->last_rec_time + 1000))  //over 1s
    {
      p_priv->last_rec_time += 1000;
      
      msg.content = RECORD_MSG_RECORDED_TIME_UPDATE;
      msg.para1 = p_priv->last_rec_time;
      total_size = ((s64)p_priv->dy_rec_info.indx) * TS_STREAM_RW_UNIT;
      msg.para2 = (u32)(total_size >> 10);/*KBYTES*/
      /*
      OS_PRINTF("\n##record [%lu, %lu][%lu]ms",
        msg.para1, p_priv->skip_time, (GET_MTOS_TICKS - p_priv->start_tick) * 10);
      */
      p_ifilter->send_message_out(p_ifilter, &msg);
    }
      
    //save it, if cur_pos can't find pts, use the pre
    if (p_priv->p_media_array)
    {
      media_idx_write(p_priv->p_media_array, &p_priv->dy_rec_info);
    }
    p_priv->dy_rec_info.indx++;

    p_buffer += TS_STREAM_RW_UNIT;
    cur_pos += TS_STREAM_RW_UNIT;
  }
}

static void trans_insert_time_by_ticks(rec_filter_private_t *p_priv, u8 *p_buffer, u32 len)
{
  u32 cur_pos = 0; //from 0~len
  u32 cur_time = 0;
  s64 total_size = 0;

  while(cur_pos < len)
  {
    if (cur_pos == 0)
    {
      cur_time = (GET_MTOS_TICKS - p_priv->start_tick) * 10;  //to ms
      if (cur_time > (p_priv->last_rec_time + 1000))
      {
        cur_time = p_priv->last_rec_time + 1000;
      }
    }
    else
    {
      cur_time += 20;
    }

    if (cur_time >=  (p_priv->last_rec_time + 1000))  //over 1s
    {
      ifilter_t *p_ifilter = p_priv->p_this;
      os_msg_t msg = {0};

      p_priv->last_rec_time += 1000;
      
      msg.content = RECORD_MSG_RECORDED_TIME_UPDATE;
      msg.para1 = p_priv->last_rec_time;
      total_size = ((s64)p_priv->dy_rec_info.indx) * TS_STREAM_RW_UNIT;
      msg.para2 = (u32)(total_size >> 10);/*KBYTES*/
      //OS_PRINTF("\n##debug: record [%d][%d]\n", msg.para1, msg.para2);
      p_ifilter->send_message_out(p_ifilter, &msg);
    }
    p_priv->dy_rec_info.cur_time = cur_time;
    //save it, if cur_pos can't find pts, use the pre
    if (p_priv->record_mode == DMX_PARTIAL_TS_PACKET
      && p_priv->p_media_array)
    {
      media_idx_write(p_priv->p_media_array, &p_priv->dy_rec_info);
    }
    else
    {
      break;
    }
    p_priv->dy_rec_info.indx++;

    p_buffer += TS_STREAM_RW_UNIT;
    cur_pos += TS_STREAM_RW_UNIT;
  }
}

static void data_cipher(media_sample_t *p_in)
{
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  ST_ADVANCEDCA_CIPHERENGINEINFO  stCipherEngineInfo;
  static int flag = 0;
  s32 i = 0;
  #define CIPHER_DATA_LENGTH 512*188
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
        if(flag == 0)
        {
          u8 tmp[16];
          flag = 1;
          stCipherEngineInfo.u32Length     = 16;
          stCipherEngineInfo.pu8Input      = tmp;
          stCipherEngineInfo.pu8Output     = tmp;
          hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVRPlayback, &stCipherEngineInfo);
          hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVRPlayback);
        }
       // stCipherEngineInfo.enBlockMode    = ADVANCEDCA_BLOCKMODE_CBC;
        while(i < p_in->payload)
        {
          if(ADVANCEDCA_BLOCKMODE_CBC == stCipherEngineInfo.enBlockMode)
          {
            memcpy(stCipherEngineInfo.u8IV, stCipherEngineInfo_u8IV, 16);
          }
          
          if((p_in->payload - i) > CIPHER_DATA_LENGTH)
          {
            stCipherEngineInfo.u32Length      = CIPHER_DATA_LENGTH;
          }
          else
          {
            stCipherEngineInfo.u32Length      = p_in->payload - i;
          }
          
          stCipherEngineInfo.pu8Input       = p_in->p_data + i;
          stCipherEngineInfo.pu8Output      = p_in->p_data + i;
          hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVR, &stCipherEngineInfo);
          hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVR);
          if(ADVANCEDCA_BLOCKMODE_CBC == stCipherEngineInfo.enBlockMode)
          {
            memcpy(stCipherEngineInfo_u8IV, 
                   p_in->p_data + i + stCipherEngineInfo.u32Length - 16, 
                   16);
          }
          i = i + CIPHER_DATA_LENGTH;
        }
        break;
    case ADVANCEDCA_CIPHERENGINE_DES:
        stCipherEngineInfo.enCipherEngine = ADVANCEDCA_CIPHERENGINE_DES;
        stCipherEngineInfo.enBlockMode    = ADVANCEDCA_BLOCKMODE_ECB;
        stCipherEngineInfo.u32Length      = p_in->payload;
        stCipherEngineInfo.pu8Input       = p_in->p_data;
        stCipherEngineInfo.pu8Output      = p_in->p_data;
        hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVR, &stCipherEngineInfo);
        hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVR);
        break;
    case ADVANCEDCA_CIPHERENGINE_TDEA:
        stCipherEngineInfo.enCipherEngine = ADVANCEDCA_CIPHERENGINE_TDEA;
        stCipherEngineInfo.enBlockMode    = ADVANCEDCA_BLOCKMODE_ECB;
        stCipherEngineInfo.u32Length      = p_in->payload;
        stCipherEngineInfo.pu8Input       = p_in->p_data;
        stCipherEngineInfo.pu8Output      = p_in->p_data;
        hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVR, &stCipherEngineInfo);
        hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVR);
        break;
    case 6:
        {
          memcpy(stCipherEngineInfo.stKeyConfigInfo.u8Key, p_u8key, ADVANCEDCA_MAXKEYSIZE);
          {
            stCipherEngineInfo.u32Length      = p_in->payload;
            stCipherEngineInfo.pu8Input       = p_in->p_data;
            stCipherEngineInfo.pu8Output      = p_in->p_data;
            hal_secure_EngineStart(ADVANCEDCA_APPLICATION_PVR, &stCipherEngineInfo);
            hal_secure_EngineStop(ADVANCEDCA_APPLICATION_PVR);
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
  rec_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  u64 total_rec = 0;

  //the flow is stoped
  if(!p_priv->running)
  {
    return ERR_FAILURE;
  }

  if((p_in->payload == 0) || (p_in->payload % 188 != 0))
  {
    return ERR_FAILURE;
  }
  
  MT_ASSERT((p_in->payload % TS_STREAM_RW_UNIT) == 0);
  
  //record over 4G
  if (!p_priv->timeshift_mode)
  {
    total_rec = (u64)p_priv->dy_rec_info.indx * TS_STREAM_RW_UNIT + p_in->payload;
    if (total_rec >= MAX_RECORD_FILE_SIZE)
    {
      //insert_end_tag(_this);
      p_transf_out_pin->query_sample(p_transf_out_pin, &p_out_sample);
      MT_ASSERT(NULL != p_out_sample);
      p_out_sample->p_data = p_in->p_data;
      p_out_sample->payload = p_in->payload;
      p_transf_out_pin->push_down(p_transf_out_pin, p_out_sample);
      p_priv->dy_rec_info.indx = 0;
      p_priv->dy_rec_info.cur_time = 0;
      p_priv->start_tick = GET_MTOS_TICKS;
      p_priv->last_rec_time = 0;
      p_priv->last_pts = 0;
      p_priv->skip_time = 0;
    }
  }

  if ((p_priv->record_mode == DMX_PARTIAL_TS_PACKET && p_priv->st_rec_info.encrypt_flag == 0)
    || (p_priv->record_mode == DMX_PARTIAL_TS_PACKET_DESCRAMBLE)|| (p_priv->record_mode == DMX_PARTIAL_TS_PACKET_SCRAMBLE))
  {
    trans_insert_time_by_pts(p_priv, p_in->p_data, p_in->payload);
  }
  else
  {
    trans_insert_time_by_ticks(p_priv, p_in->p_data, p_in->payload);
  }

  log_perf(LOG_DMX_FILTER_REC_PIN, TAG_SPLIT_TRANSFOR_START, 0, 0);
  //push down

  if (0 == p_priv->timeshift_mode)
  {
    data_cipher(p_in);   
  }

  p_transf_out_pin->push_down(p_transf_out_pin, p_in);
  log_perf(LOG_DMX_FILTER_REC_PIN, TAG_SPLIT_TRANSFOR_END, 0, 0);

  return SUCCESS;
}

static RET_CODE on_open(handle_t _this)
{
  rec_filter_private_t *p_priv = get_priv(_this);
   
  p_priv->start_tick = 0;
  p_priv->last_pts = 0;
  p_priv->unit_pts_offset = 0;
  p_priv->vkey_frame_offset = 188;
  p_priv->skip_time = 0;
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  memset(stCipherEngineInfo_u8IV, 0xF0, 16);
  #endif
  return SUCCESS;
}

static RET_CODE on_start(handle_t _this)
{
  rec_filter_private_t *p_priv = get_priv(_this);
  dmx_device_t *p_dmx_dev = NULL;
  dmx_chanid_t p_channel_a = 0,p_channel_v = 0;
  
  p_priv->start_tick = GET_MTOS_TICKS;
  p_priv->running = 1;
  
  p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(p_dmx_dev != NULL);
  
  if ((p_priv->st_rec_info.encrypt_flag == 1) && p_priv->st_rec_info.is_biss_key)
  {
      dmx_get_chanid_bypid(p_dmx_dev, p_priv->dy_rec_info.a_pid,&p_channel_a);
      dmx_descrambler_onoff(p_dmx_dev, p_channel_a, FALSE);
      dmx_descrambler_onoff(p_dmx_dev, p_channel_a, TRUE);
      dmx_descrambler_set_odd_keys(p_dmx_dev,
        p_channel_a, p_priv->st_rec_info.audio_key_odd, p_priv->st_rec_info.audio_key_odd_len);
      dmx_descrambler_set_even_keys(p_dmx_dev,
        p_channel_a, p_priv->st_rec_info.audio_key_even, p_priv->st_rec_info.audio_key_even_len);
      
      dmx_get_chanid_bypid(p_dmx_dev, p_priv->dy_rec_info.v_pid, &p_channel_v);
      dmx_descrambler_onoff(p_dmx_dev, p_channel_v, FALSE);
      dmx_descrambler_onoff(p_dmx_dev, p_channel_v, TRUE);
      dmx_descrambler_set_even_keys(p_dmx_dev,
        p_channel_v, p_priv->st_rec_info.video_key_even, p_priv->st_rec_info.video_key_even_len);
      dmx_descrambler_set_odd_keys(p_dmx_dev,
        p_channel_v, p_priv->st_rec_info.video_key_odd, p_priv->st_rec_info.video_key_odd_len);
  }
  return SUCCESS;
}

static RET_CODE on_pause(handle_t _this)
{
  rec_filter_private_t *p_priv = get_priv(_this);

  p_priv->pause_tick = GET_MTOS_TICKS;
  
  return SUCCESS;
}

static RET_CODE on_resume(handle_t _this)
{
  rec_filter_private_t *p_priv = get_priv(_this);

  p_priv->start_tick += (GET_MTOS_TICKS - p_priv->pause_tick);  //skip pause time
  p_priv->last_pts = 0;
  
  return SUCCESS;
}

static RET_CODE on_stop(handle_t _this)
{
  rec_filter_private_t *p_priv = get_priv(_this);
  
  if(p_priv->running)
  {
    p_priv->running = 0;
    //insert_end_tag(_this);
  }

  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  return SUCCESS;
}

static void on_destory(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t *record_filter_create(void *p_para)
{
  rec_filter_private_t *p_priv = NULL;
  record_filter_t *p_ins = NULL;
  interface_t *p_interface = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_t *p_transffilter = NULL;
  transf_filter_para_t transf_filter_para;
  
  transf_filter_para.dummy = 0;
  //check input parameter
  p_ins = mtos_malloc(sizeof(record_filter_t));
  MT_ASSERT(p_ins != NULL);
  memset(p_ins, 0, sizeof(record_filter_t));
  
  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);  

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(rec_filter_private_t));
  p_priv->p_this = p_ins; //this point
  p_priv->p_media_array = NULL;

  //over loading the virtual function for ifilter_t
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = on_command;
  p_ifilter->on_process_evt = on_evt;
  p_ifilter->on_open = on_open;
  p_ifilter->on_start = on_start;
  p_ifilter->on_pause = on_pause;
  p_ifilter->on_resume = on_resume;
  p_ifilter->on_stop = on_stop;
  p_ifilter->on_close = on_close;

  //over loading the virtual function for transf_filter_t
  p_transffilter = (transf_filter_t *)p_ins;
  p_transffilter->transform = split_transform;
  
  p_interface = (interface_t *)p_ins;
  p_interface->on_destroy = on_destory;
  record_in_pin_create(&p_priv->m_in_pin, p_interface);
  record_out_pin_create(&p_priv->m_out_pin, p_interface);

  return (ifilter_t *)p_ins;
}

/*!
  end of file
  */

