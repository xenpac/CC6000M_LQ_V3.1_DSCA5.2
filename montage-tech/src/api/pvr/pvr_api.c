/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_sem.h"


#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"

#include "drv_dev.h"
#include "common.h"
#include "dmx.h"
#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "eit.h"
#include "tdt.h"
#include "mt_time.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "err_check_def.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"

//filter
#include "eva_filter_factory.h"
#include "demux_filter.h"
#include "record_filter.h"
#include "file_source_filter.h"
#include "file_sink_filter.h"
#include "ts_player_filter.h"
#include "av_render_filter.h"

#include "pvr_api.h"

#define MAX_REC_CHN_NUM  (3)

#define MAX_PLAY_CHN_NUM  (2)

#ifdef WIN32
#define MIN_REC_FILE_SIZE  (188 *348 * 512) // about 32M
#else
#define MIN_REC_FILE_SIZE  (188 *348 * 1024) // about 128M
#endif

#define REC_FILL_UNIT_SIZE (512 * 188) //align to 188(as packet) and 512 (as disk block)
#define REC_FILL_EXPECT_SIZE (REC_FILL_UNIT_SIZE) //the times high, the proformance high

#define PLAY_FILL_UNIT_SIZE (188) //align to 188(as packet)

#define INVAILD_CHN_ID    (0xff)

#define PLAY_CHAN_BIT (0x1000)

#define REC_CHAN_BIT  (0x2000)



/*!
  timeshift_mode_t
  */
typedef enum tag_timeshift_mode
{
  /*!
    playback mode
    */
  TS_PLAYBACK_MODE = 0,
  /*!
    live mode
    */
  TS_LIVE_MODE,
}timeshift_mode_t;

typedef struct
{
  BOOL used;
  controller_t rec_ctrler;

  chain_t *p_chain;
  /*!
    demux filter
    */
  ifilter_t *p_demux_filter;
  /*!
    record filter
    */
  ifilter_t *p_rec_filter;
  /*!
    file sink filter
    */
  ifilter_t *p_fsink_filter;

  ipin_t *p_rec_pin;
  mul_pvr_rec_attr_t rec_attr;
  mul_pvr_rec_status_t status;
  event_call_back call_back;
  u32 context;
  media_idx_t *p_media_idx;
  u8 attach_play_chain;
  u8 rec_in;
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  /*
      cipher engine type
  */
  int cipher_engine;
  u8 cipher_key[ADVANCEDCA_MAXKEYSIZE];
  #endif
}rec_control_t;

typedef struct
{
  BOOL used;
  controller_t play_ctrler;
  chain_t *p_chain;
  /*!
    source filter
    */
  ifilter_t *p_source_filter;
  /*!
    ts player filter
    */
  ifilter_t *p_player_filter;
  /*!
    video render filter
    */
  ifilter_t *p_av_render_filter;
  mul_pvr_play_attr_t play_attr;

  mul_pvr_play_status_t status;

  event_call_back call_back;
  u32 context;
  media_idx_t *p_media_idx;

  u8 attach_rec_chain;
  timeshift_mode_t ts_mode;
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  /*
      cipher engine type
  */
  int cipher_engine;
  u8 cipher_key[ADVANCEDCA_MAXKEYSIZE];
  #endif
}play_control_t;

typedef struct
{
  rec_control_t rec_ctrl[MAX_REC_CHN_NUM];
  play_control_t play_ctrl[MAX_PLAY_CHN_NUM];
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  /*
      cipher engine type
  */
  int cipher_engine;
  u8 cipher_key[ADVANCEDCA_MAXKEYSIZE];
  #endif

}mul_pvr_priv_t;

#ifdef WIN32
extern void dmx_set_timeshift(unsigned char timeshift);
#else
extern int sprintf (char *s, const  char *p_format, ...);
#endif


static u8 s_pvr_find_free_rec_chain(mul_pvr_priv_t *p_this)
{
  u8 i = 0;

  for(i = 0; i < MAX_REC_CHN_NUM; i ++)
  {
    if(!p_this->rec_ctrl[i].used)
    {
      p_this->rec_ctrl[i].used = TRUE;
      return i;
    }
  }
  return INVAILD_CHN_ID;
}

static u8 s_pvr_find_free_play_chain(mul_pvr_priv_t *p_this)
{
  u8 i = 0;

  for(i = 0; i < MAX_PLAY_CHN_NUM; i ++)
  {
    if(!p_this->play_ctrl[i].used)
    {
      p_this->play_ctrl[i].used = TRUE;
      return i;
    }
  }
  return INVAILD_CHN_ID;
}

static void s_pvr_rec_time_update(u32 chanid, u32 time_ms, u32 size)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  if(p_ctrl->status.start_time_ms != 0)
  {
    p_ctrl->status.start_time_ms += (time_ms - p_ctrl->status.cur_time_ms);
  }
  p_ctrl->status.cur_time_ms = time_ms;
  p_ctrl->status.end_time_ms = time_ms;
  p_ctrl->status.cur_write_point = size;
  //callback
  if(p_ctrl->call_back != NULL)
  {
    p_ctrl->call_back(chanid | REC_CHAN_BIT,
      MUL_PVR_EVENT_REC_TIME_UPDATE, time_ms, (void *)p_ctrl->context);
  }
}

static void s_pvr_rec_write_back(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];
  if(0 == p_ctrl->status.start_time_ms) //if the start time is runing, can't reset it.
  {
    p_ctrl->status.start_time_ms = 1;
  }
  //callback
  if(p_ctrl->call_back != NULL)
    p_ctrl->call_back(chanid, MUL_PVR_EVENT_REC_OVER_FIX, 0, (void *)p_ctrl->context);
}

static void s_pvr_rec_event_back(u32 chanid, u32 event)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  mul_pvr_event_t pvr_evt = MUL_PVR_EVENT_PLAY_RESV;
  u32 write_error = 0;
  
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];
  switch(event)
  {
    case FILE_SINK_MSG_BLOCK_WRITE_REACH_READ:
      pvr_evt = MUL_PVR_EVENT_REC_REACH_PLAY;
      break;
    case FILE_SINK_MSG_NO_ENOUGH_MEM:
      pvr_evt = MUL_PVR_EVENT_REC_DISKFULL;
      break;
    case FILE_SINK_MSG_WRITE_ERROR:
      //pvr_evt = MUL_PVR_EVENT_REC_ERROR;
      pvr_evt = MUL_PVR_EVENT_REC_DISKFULL;
      write_error = 1;
      break;
    default:
      break;
  }

  //callback
  if(p_ctrl->call_back != NULL)
    p_ctrl->call_back(chanid, pvr_evt, write_error, (void *)p_ctrl->context);
}

static void s_pvr_rec_save_file(u32 chanid, u32 index)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  //callback
  if(p_ctrl->call_back != NULL)
  {
    p_ctrl->call_back(chanid | REC_CHAN_BIT,
      MUL_PVR_EVENT_REC_RESV, index, (void *)p_ctrl->context);
  }
}

static void s_pvr_play_time_update(u32 chanid, u32 time_ms, u32 size)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return;
  }
  p_ctrl = &p_this->play_ctrl[chanid];
  p_ctrl->status.cur_play_time_ms = time_ms;
  p_ctrl->status.cur_play_pos = size;
  //callback
  if(p_ctrl->call_back != NULL)
    p_ctrl->call_back(chanid, MUL_PVR_EVENT_PLAY_TIME_UPDATE, time_ms, (void *)p_ctrl->context);

}

static void s_pvr_play_event_update(u32 chanid, u32 event)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
  mul_pvr_event_t pvr_evt = MUL_PVR_EVENT_PLAY_RESV;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return;
  }
  p_ctrl = &p_this->play_ctrl[chanid];
  switch(event)
  {
    case PLAYER_MSG_PLAY_MODE_CHANGED:
      pvr_evt = MUL_PVR_EVENT_PLAY_SOF;
      if (((p_ctrl->attach_rec_chain != INVAILD_CHN_ID)
        && ((p_ctrl->attach_rec_chain & 0xf) < MAX_REC_CHN_NUM))
      && (p_ctrl->status.speed == MUL_PVR_PLAY_SPEED_2X_FAST_FORWARD
        ||p_ctrl->status.speed == MUL_PVR_PLAY_SPEED_4X_FAST_FORWARD
        ||p_ctrl->status.speed == MUL_PVR_PLAY_SPEED_8X_FAST_FORWARD
        ||p_ctrl->status.speed == MUL_PVR_PLAY_SPEED_16X_FAST_FORWARD
        ||p_ctrl->status.speed == MUL_PVR_PLAY_SPEED_32X_FAST_FORWARD))
      {
        p_ctrl->ts_mode = TS_LIVE_MODE;
        OS_PRINTF("\n##debug: set ts live mode by fast forward!\n");
      }
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_PLAY;
      p_ctrl->status.speed = MUL_PVR_PLAY_SPEED_NORMAL;
      break;
    case AV_RENDER_MSG_FILE_PLAY_END:
      pvr_evt = MUL_PVR_EVENT_PLAY_EOF;
      break;
    case PLAYER_MSG_PLAY_ERROR:
      pvr_evt = MUL_PVR_EVENT_PLAY_ERROR;
      break;
    //case FSRC_MSG_BLOCK_READ_INSUFFICIENT:
      //pvr_evt = MUL_PVR_EVENT_PLAY_REACH_REC;
      //break;
    default:
      return;
  }

  //callback
  if(p_ctrl->call_back != NULL)
  {
    p_ctrl->call_back(chanid, pvr_evt, 0, (void *)p_ctrl->context);
  }
}

static void s_pvr_ready_for_play(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];
  //callback
  if(p_ctrl->call_back != NULL)
    p_ctrl->call_back(chanid, MUL_PVR_EVENT_REC_READY_FOR_PLAY, 0, (void *)p_ctrl->context);

}

static BOOL s_pvr_on_filter_evt(handle_t _this, os_msg_t *p_msg)
{
  u32 chn_id = 0;
  mul_pvr_priv_t *p_this = NULL;

  chn_id = (u32)((controller_t *)_this)->user_handle;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return TRUE;
  }
  
  switch(p_msg->content)
  {
    case FILE_SINK_MSG_READY:
      // callback
      s_pvr_ready_for_play(chn_id);
      break;
    case FILE_SINK_MSG_BLOCK_WRITE_BACK:
      s_pvr_rec_write_back(chn_id);
      break;
    case FILE_SINK_MSG_BLOCK_WRITE_REACH_READ:
      s_pvr_rec_event_back(chn_id, FILE_SINK_MSG_BLOCK_WRITE_REACH_READ);
      break;
    case FILE_SINK_MSG_SAVE_FILE:
      s_pvr_rec_save_file(chn_id, p_msg->para1);
      break;
    case RECORD_MSG_RECORDED_TIME_UPDATE:
      {
        s_pvr_rec_time_update(chn_id, p_msg->para1, p_msg->para2);

        if ((p_this->rec_ctrl[chn_id & 0xf].attach_play_chain != INVAILD_CHN_ID)
              && ((p_this->rec_ctrl[chn_id & 0xf].attach_play_chain & 0xf) < MAX_REC_CHN_NUM))
        {
          chn_id = p_this->rec_ctrl[chn_id & 0xf].attach_play_chain;
          if ((p_this->play_ctrl[chn_id & 0xf].ts_mode == TS_LIVE_MODE)
            && (p_this->play_ctrl[chn_id & 0xf].status.state == MUL_PVR_PLAY_STATE_PLAY))
          {
            s_pvr_play_time_update(chn_id, p_msg->para1, p_msg->para2);
          }
        }
      }
      break;
    case PLAYER_MSG_PLAY_TIME_UPDATE:
      {
        if (p_this->play_ctrl[chn_id & 0xf].ts_mode == TS_PLAYBACK_MODE)
        {
          s_pvr_play_time_update(chn_id, p_msg->para1, p_msg->para2);
        }
      }
      break;
    case PLAYER_MSG_PLAY_MODE_CHANGED:
    //  OS_PRINTF("\n##debug: PLAYER_MSG_PLAY_MODE_CHANGED !\n");
      s_pvr_play_event_update(chn_id, p_msg->content);
      break;
    case FSRC_MSG_BLOCK_READ_INSUFFICIENT:
      s_pvr_play_event_update(chn_id, p_msg->content);
      break;
    case AV_RENDER_MSG_FILE_PLAY_END:
      s_pvr_play_event_update(chn_id, p_msg->content);
      break;
    case FILE_SINK_MSG_NO_ENOUGH_MEM:
    case FILE_SINK_MSG_WRITE_ERROR:
      s_pvr_rec_event_back(chn_id, p_msg->content);
      break;
    case PLAYER_MSG_PLAY_ERROR:
      s_pvr_play_event_update(chn_id, p_msg->content);
      break;
    default:
      break;
  }
  return TRUE;
}

RET_CODE mul_pvr_init(void)
{
  mul_pvr_priv_t *p_this = NULL;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this != NULL)
  {
    OS_PRINTF("pvr already inited\n");
    return ERR_FAILURE;
  }
  p_this = mtos_malloc(sizeof(mul_pvr_priv_t));
  memset(p_this, 0, sizeof(mul_pvr_priv_t));
  class_register(PVR_API_CLASS_ID, p_this);
  return SUCCESS;
}


RET_CODE mul_pvr_rec_create_chn(u32 *p_chanid, const mul_pvr_rec_attr_t *p_rec_attr)
{
  RET_CODE ret = SUCCESS;
  ctrl_para_t ctrl_para = {0};
  chain_para_t para = {0};
  char chan_name[30] = {0};
  mul_pvr_priv_t *p_this = NULL;
  u32 chn_id = INVAILD_CHN_ID;
  rec_control_t *p_ctrl = NULL;
  ipin_t *p_dmx_out = NULL;
  ipin_t *p_rec_in = NULL;
  ipin_t *p_rec_out = NULL;
  ipin_t *p_fsink_in_pin = NULL;
  icmd_t icmd = {0};
  media_format_t format = {MT_RECODE};
  u32 unit_count = 0;
  u16 idx_file_name[MAX_FILE_PATH] = {0};

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  if(p_rec_attr == NULL
    || p_rec_attr->stk_size == 0
    || p_rec_attr->file_name_len == 0
    || p_rec_attr->p_rec_buffer == NULL
//    || p_rec_attr->rec_buffer_size < REC_BUFFER_UNIT_SIZE
    || (p_rec_attr->max_file_size < MIN_REC_FILE_SIZE
          && p_rec_attr->b_rewind))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  
  chn_id = s_pvr_find_free_rec_chain(p_this);
  if(chn_id == INVAILD_CHN_ID)
  {
    return MUL_ERR_PVR_NO_CHN_LEFT;
  }
  p_ctrl = &p_this->rec_ctrl[chn_id];

  //save the attr
  memcpy(&p_ctrl->rec_attr, p_rec_attr, sizeof(mul_pvr_rec_attr_t));

  ctrl_para.user_handle = (handle_t)chn_id;
  if(NULL == controller_create(&p_ctrl->rec_ctrler, &ctrl_para))
  {
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_NO_CHN_LEFT;
  }
  p_ctrl->rec_ctrler.on_process_evt = s_pvr_on_filter_evt;

  //create chain
  para.p_owner = (interface_t *)&p_ctrl->rec_ctrler;
  sprintf(chan_name, "rec_chain_%ld", chn_id);
  para.p_name = chan_name;
  para.stk_size = p_rec_attr->stk_size;
  p_ctrl->p_chain = chain_create(&para);

  //create filter
  ret = eva_add_filter_by_id(DEMUX_FILTER, &p_ctrl->p_demux_filter, NULL);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }

  ret = eva_add_filter_by_id(RECORD_FILTER, &p_ctrl->p_rec_filter, NULL);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }

  ret = eva_add_filter_by_id(FILE_SINK_FILTER, &p_ctrl->p_fsink_filter, NULL);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }

  //add filter to chain
  sprintf(chan_name, "rec_dmx_filter_%ld", chn_id);
  ret = p_ctrl->p_chain->add_filter(p_ctrl->p_chain, p_ctrl->p_demux_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }
  sprintf(chan_name, "rec_rec_filter_%ld", chn_id);
  ret = p_ctrl->p_chain->add_filter(p_ctrl->p_chain, p_ctrl->p_rec_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }
  sprintf(chan_name, "rec_fsink_filter_%ld", chn_id);
  ret = p_ctrl->p_chain->add_filter(p_ctrl->p_chain, p_ctrl->p_fsink_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }

  //get demux output pin
  ret = p_ctrl->p_demux_filter->get_unconnect_pin(p_ctrl->p_demux_filter, OUTPUT_PIN, &p_dmx_out);
  if(ret != SUCCESS || p_dmx_out == NULL)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_NO_PIN;
  }

  p_ctrl->p_media_idx = NULL;
  p_ctrl->attach_play_chain = INVAILD_CHN_ID;
  if (p_ctrl->rec_attr.stream_type != MUL_PVR_STREAM_TYPE_ALL_TS)
  {
    if (p_ctrl->rec_attr.b_rewind)
    {
      unit_count = p_ctrl->rec_attr.max_file_size / TS_STREAM_RW_UNIT;
      if (!media_idx_create(&p_ctrl->p_media_idx,
        unit_count, NULL, p_ctrl->rec_attr.p_ridx_buf, p_ctrl->rec_attr.ridx_buf_size))
      {
        return MUL_ERR_PVR_EVA_FILTER_NO_PIN;
      }
      p_ctrl->rec_attr.max_file_size = (s64)unit_count * TS_STREAM_RW_UNIT;
    }
    else
    {
      unit_count = 1024 * 2;
      get_idx_info_file(p_ctrl->rec_attr.file_name, idx_file_name, MAX_FILE_PATH);
      if (!media_idx_create(&p_ctrl->p_media_idx,
        unit_count, idx_file_name, p_ctrl->rec_attr.p_ridx_buf, p_ctrl->rec_attr.ridx_buf_size))
      {
        return MUL_ERR_PVR_EVA_FILTER_NO_PIN;
      }
    }
  }
  else
  {
    p_ctrl->p_media_idx = NULL;
  }

  p_ctrl->p_rec_pin = p_dmx_out;
  icmd.cmd = DMX_SET_PIN_TYPE;
  icmd.p_para = p_dmx_out;
  icmd.lpara = DMX_PIN_REC_TYPE;
  p_ctrl->p_demux_filter->do_command(p_ctrl->p_demux_filter, &icmd);
#if 1
  //get recorder input pin
  ret = p_ctrl->p_rec_filter->get_unconnect_pin(p_ctrl->p_rec_filter, INPUT_PIN, &p_rec_in);
  if(ret != SUCCESS || p_rec_in == NULL)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_NO_PIN;
  }

  //get recorder output pin
  ret = p_ctrl->p_rec_filter->get_unconnect_pin(p_ctrl->p_rec_filter, OUTPUT_PIN, &p_rec_out);
  if(ret != SUCCESS || p_rec_out == NULL)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_NO_PIN;
  }

  //get file sink input pin
  ret = p_ctrl->p_fsink_filter->get_unconnect_pin(p_ctrl->p_fsink_filter,
                                                INPUT_PIN, &p_fsink_in_pin);
  if(ret != SUCCESS || p_fsink_in_pin == NULL)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_NO_PIN;
  }

  format.stream_type = MT_RECODE;
  ret = p_ctrl->p_chain->connect_pin(p_ctrl->p_chain, p_dmx_out, p_rec_in, &format);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }

  ret = p_ctrl->p_chain->connect_pin(p_ctrl->p_chain, p_rec_out, p_fsink_in_pin, &format);
  if(ret != SUCCESS)
  {
    p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
    p_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }
#else
  format.stream_type = MT_RECODE;
  ret = p_ctrl->p_chain->connect(p_ctrl->p_chain, p_ctrl->p_demux_filter, p_rec_filter, &format);
  if(ret != SUCCESS)
  {
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }
  format.stream_type = MT_RECODE;
  ret = p_ctrl->p_chain->connect(p_ctrl->p_chain, p_ctrl->p_rec_filter, p_fsink_filter, &format);
  if(ret != SUCCESS)
  {
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }
#endif
  p_ctrl->status.state = MUL_PVR_REC_STATE_CREATED;
  *p_chanid = chn_id | REC_CHAN_BIT;
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  p_ctrl->cipher_engine = p_this->cipher_engine;
  memcpy(p_ctrl->cipher_key, p_this->cipher_key, ADVANCEDCA_MAXKEYSIZE);
  #endif
  return SUCCESS;
}

RET_CODE mul_pvr_rec_destroy_chn(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  p_ctrl = &p_this->rec_ctrl[chanid];
  if (p_ctrl->p_media_idx)
  {
    media_idx_destory(p_ctrl->p_media_idx);
    p_ctrl->p_media_idx = NULL;
  }
  p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
  memset(p_ctrl, 0, sizeof(rec_control_t));
  return SUCCESS;
}

RET_CODE mul_pvr_rec_set_chn(u32 chanid, const mul_pvr_rec_attr_t *p_rec_attr)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  p_ctrl = &p_this->rec_ctrl[chanid];

  memcpy(&p_ctrl->rec_attr, p_rec_attr, sizeof(mul_pvr_rec_attr_t));


  return SUCCESS;
}

RET_CODE mul_pvr_rec_get_chn(u32 chanid, mul_pvr_rec_attr_t *p_rec_attr)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  memcpy(p_rec_attr, &p_ctrl->rec_attr, sizeof(mul_pvr_rec_attr_t));
  return SUCCESS;
}

RET_CODE mul_pvr_rec_start_chn(u32 chanid)
{
  RET_CODE ret = SUCCESS;
  icmd_t icmd = {0};
  ifilter_t *p_demux_filter = NULL;
  ifilter_t *p_rec_filter = NULL;
  ifilter_t *p_fsink_filter = NULL;
  rec_buf_cfg_t dmx_cfg = {0};
  rec_request_data_t rec_req = {0};
  rec_filter_pg_pid_t rec_pg_pid;
  idx_extern_info_t e_info;
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  u16 loopi = 0;
  dmx_info_t dmx_info = {0};
  u32 rec_buf_unit_size = 0;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  /*if (!p_ctrl->rec_attr.b_clear_stream)
  {
    OS_PRINTF("\n##do not support record cas ch!!!\n");
    return -1;
  }*/
  memset(&rec_pg_pid, 0, sizeof(rec_filter_pg_pid_t));
  p_demux_filter = p_ctrl->p_demux_filter;
  p_rec_filter = p_ctrl->p_rec_filter;
  p_fsink_filter = p_ctrl->p_fsink_filter;
  dmx_get_info(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI), &dmx_info);
  #ifdef WIN32
  rec_buf_unit_size = REC_FILL_UNIT_SIZE;
  #else
  rec_buf_unit_size = dmx_info.rec_unit_size;
  #endif
  //fix for ensemble temp
  if (rec_buf_unit_size == 0)
  {
    rec_buf_unit_size = REC_FILL_UNIT_SIZE;
  }
  //cfg buffer
  if(p_ctrl->rec_attr.rec_buffer_size < rec_buf_unit_size)
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  if(p_ctrl->rec_attr.p_rec_buffer == NULL)
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  if (p_ctrl->rec_attr.rec_in > DMX_REC_CONFIG1)
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  
  dmx_cfg.p_rec_buffer = p_ctrl->rec_attr.p_rec_buffer;
  dmx_cfg.total_buf_len = p_ctrl->rec_attr.rec_buffer_size;
  if ((dmx_cfg.total_buf_len % rec_buf_unit_size)
    || (dmx_cfg.total_buf_len % REC_FILL_UNIT_SIZE))
  {
    dmx_cfg.total_buf_len = (p_ctrl->rec_attr.rec_buffer_size - REC_FILL_UNIT_SIZE);
    dmx_cfg.total_buf_len -= dmx_cfg.total_buf_len % rec_buf_unit_size;
  }
  
  dmx_cfg.atom_size = rec_buf_unit_size;
  dmx_cfg.fill_unit_size = REC_FILL_UNIT_SIZE;
  dmx_cfg.fill_expect_size = REC_FILL_EXPECT_SIZE;
  dmx_cfg.rec_in = p_ctrl->rec_attr.rec_in;
  switch(p_ctrl->rec_attr.stream_type)
  {
  case MUL_PVR_STREAM_TYPE_ALL_TS:
    dmx_cfg.rec_mode = DMX_FULL_TS_WITHOUT_NULL_PACKET;
    break;
  case MUL_PVR_STREAM_TYPE_SCRAMBLE_TS:
    dmx_cfg.rec_mode = DMX_PARTIAL_TS_PACKET_SCRAMBLE;
    break;
  default:
    dmx_cfg.rec_mode = DMX_PARTIAL_TS_PACKET_DESCRAMBLE;
    break;
  }

  if (dmx_cfg.rec_mode == DMX_FULL_TS_WITHOUT_NULL_PACKET
    || dmx_cfg.rec_mode == DMX_FULL_TS_WITH_NULL_PACKET)
  {
    icmd.cmd = DMX_SET_PID;
    icmd.p_para = (void *)p_ctrl->p_rec_pin;
    rec_req.pid = p_ctrl->rec_attr.media_info.v_pid;
    rec_req.rec_mode = dmx_cfg.rec_mode;
    rec_req.demux_index = p_ctrl->rec_attr.demux_index;
    rec_req.rec_in = dmx_cfg.rec_in;
    icmd.lpara = (u32)&rec_req;
    if (p_demux_filter->do_command(p_demux_filter, &icmd) != SUCCESS)
    {
      return ERR_FAILURE;
    }
  }
  else
  {
    icmd.cmd = DMX_SET_PID;
    icmd.p_para = (void *)p_ctrl->p_rec_pin;
    rec_req.pid = p_ctrl->rec_attr.media_info.v_pid;
    rec_req.rec_mode = dmx_cfg.rec_mode;
    rec_req.demux_index = p_ctrl->rec_attr.demux_index;
    rec_req.rec_in = dmx_cfg.rec_in;
    icmd.lpara = (u32)&rec_req;
    if (p_demux_filter->do_command(p_demux_filter, &icmd) != SUCCESS)
    {
      return ERR_FAILURE;
    }

    icmd.cmd = DMX_SET_PID;
    icmd.p_para = (void *)p_ctrl->p_rec_pin;
    rec_req.pid = p_ctrl->rec_attr.media_info.a_pid;
    rec_req.rec_mode = dmx_cfg.rec_mode;
    rec_req.demux_index = p_ctrl->rec_attr.demux_index;
    rec_req.rec_in = dmx_cfg.rec_in;
    icmd.lpara = (u32)&rec_req;
    if (p_demux_filter->do_command(p_demux_filter, &icmd) != SUCCESS)
    {
      return ERR_FAILURE;
    }

    if (p_ctrl->rec_attr.media_info.pmt_pid)
    {
      icmd.cmd = DMX_SET_PID;
      icmd.p_para = (void *)p_ctrl->p_rec_pin;
      rec_req.pid = p_ctrl->rec_attr.media_info.pmt_pid;
      rec_req.rec_mode = dmx_cfg.rec_mode;
      rec_req.demux_index = p_ctrl->rec_attr.demux_index;
      rec_req.rec_in = dmx_cfg.rec_in;
      icmd.lpara = (u32)&rec_req;
      if (p_demux_filter->do_command(p_demux_filter, &icmd) != SUCCESS)
      {
        return ERR_FAILURE;
      }
    }
    
    if ((p_ctrl->rec_attr.media_info.pcr_pid != p_ctrl->rec_attr.media_info.v_pid)
      && (p_ctrl->rec_attr.media_info.pcr_pid != p_ctrl->rec_attr.media_info.a_pid))
    {
      icmd.cmd = DMX_SET_PID;
      icmd.p_para = (void *)p_ctrl->p_rec_pin;
      rec_req.pid = p_ctrl->rec_attr.media_info.pcr_pid;
      rec_req.rec_mode = dmx_cfg.rec_mode;
      rec_req.demux_index = p_ctrl->rec_attr.demux_index;
      rec_req.rec_in = dmx_cfg.rec_in;
      icmd.lpara = (u32)&rec_req;
      if (p_demux_filter->do_command(p_demux_filter, &icmd) != SUCCESS)
      {
        return ERR_FAILURE;
      }
    }
    
    CHECK_FAIL_RET_CODE(p_ctrl->rec_attr.media_info.extern_num <= REC_MAX_EXTERN_PID);
    
    for (loopi = 0; loopi < p_ctrl->rec_attr.media_info.extern_num; loopi ++)
    {
      icmd.cmd = DMX_SET_PID;
      icmd.p_para = (void *)p_ctrl->p_rec_pin;
      rec_req.pid = p_ctrl->rec_attr.media_info.extern_pid[loopi].pid;
      rec_req.rec_mode = dmx_cfg.rec_mode;
      rec_req.demux_index = p_ctrl->rec_attr.demux_index;
      rec_req.rec_in = dmx_cfg.rec_in;
      icmd.lpara = (u32)&rec_req;
      if (p_demux_filter->do_command(p_demux_filter, &icmd) != SUCCESS)
      {
        return ERR_FAILURE;
      }
    }
  }

  icmd.cmd = DMX_CFG_PARA;
  icmd.p_para = (void *)p_ctrl->p_rec_pin;
  icmd.lpara = (u32) &dmx_cfg;
  p_demux_filter->do_command(p_demux_filter, &icmd);

  memset(&e_info, 0, sizeof(idx_extern_info_t));
  //if(!p_ctrl->rec_attr.b_rewind)
  {
    memcpy(e_info.st_rec_info.program, p_ctrl->rec_attr.program, sizeof(u16) * PROGRAM_NAME_MAX);
    memcpy(&e_info.st_rec_info.start, &p_ctrl->rec_attr.start, sizeof(utc_time_t));
    e_info.st_rec_info.pmt_pid = p_ctrl->rec_attr.media_info.pmt_pid;
    e_info.st_rec_info.lock_flag = 0;
    if (!p_ctrl->rec_attr.b_clear_stream)
    {
      e_info.st_rec_info.is_biss_key = p_ctrl->rec_attr.key_cfg.is_biss_key;
      if (e_info.st_rec_info.is_biss_key)
      {
        e_info.st_rec_info.audio_key_even_len = p_ctrl->rec_attr.key_cfg.audio_key_even_len;
        e_info.st_rec_info.audio_key_odd_len = p_ctrl->rec_attr.key_cfg.audio_key_odd_len;
        memcpy(e_info.st_rec_info.audio_key_even,
          p_ctrl->rec_attr.key_cfg.audio_key_even, e_info.st_rec_info.audio_key_even_len);
        memcpy(e_info.st_rec_info.audio_key_odd,
          p_ctrl->rec_attr.key_cfg.audio_key_odd, e_info.st_rec_info.audio_key_odd_len);
        
        e_info.st_rec_info.video_key_even_len = p_ctrl->rec_attr.key_cfg.video_key_even_len;
        e_info.st_rec_info.video_key_odd_len = p_ctrl->rec_attr.key_cfg.video_key_odd_len;
        memcpy(e_info.st_rec_info.video_key_even,
          p_ctrl->rec_attr.key_cfg.video_key_even, e_info.st_rec_info.video_key_even_len);
        memcpy(e_info.st_rec_info.video_key_odd,
          p_ctrl->rec_attr.key_cfg.video_key_odd, e_info.st_rec_info.video_key_odd_len);
      }
    }
    else
    {
      e_info.st_rec_info.is_biss_key = 0;
    }
  }
  e_info.st_rec_info.encrypt_flag = p_ctrl->rec_attr.b_clear_stream ? 0 : 1;
  e_info.extern_num = p_ctrl->rec_attr.media_info.extern_num;
  for (loopi = 0; loopi < e_info.extern_num; loopi ++)
  {
    e_info.extern_pid[loopi].pid = p_ctrl->rec_attr.media_info.extern_pid[loopi].pid;
    e_info.extern_pid[loopi].type = p_ctrl->rec_attr.media_info.extern_pid[loopi].type;
    e_info.extern_pid[loopi].fmt = p_ctrl->rec_attr.media_info.extern_pid[loopi].fmt;
  }
  e_info.v_pid = p_ctrl->rec_attr.media_info.v_pid;
  e_info.a_pid = p_ctrl->rec_attr.media_info.a_pid;
  e_info.pcr_pid = p_ctrl->rec_attr.media_info.pcr_pid;
  e_info.video_fmt = p_ctrl->rec_attr.media_info.video_type;
  e_info.audio_fmt = p_ctrl->rec_attr.media_info.audio_type;
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  e_info.cipher_engine = p_ctrl->cipher_engine;
  memcpy(e_info.cipher_key, p_ctrl->cipher_key, ADVANCEDCA_MAXKEYSIZE);
  #endif
  memcpy(e_info.user_data, p_ctrl->rec_attr.user_data, IDX_INFO_USER_DATA_LENGTH);
  if (p_ctrl->p_media_idx)
  {
    media_idx_write_extern_info(p_ctrl->p_media_idx, &e_info);
  }
  
  icmd.cmd = RECORD_CFG_PROGRAM;
  icmd.p_para = &e_info.st_rec_info;
  p_rec_filter->do_command(p_rec_filter, &icmd);

  rec_pg_pid.record_mode = dmx_cfg.rec_mode;
  rec_pg_pid.v_pid = p_ctrl->rec_attr.media_info.v_pid;
  rec_pg_pid.a_pid = p_ctrl->rec_attr.media_info.a_pid;
  rec_pg_pid.pcr_pid = p_ctrl->rec_attr.media_info.pcr_pid;
  rec_pg_pid.video_fmt = p_ctrl->rec_attr.media_info.video_type;
  rec_pg_pid.audio_fmt = p_ctrl->rec_attr.media_info.audio_type;
  rec_pg_pid.ext_pid_cnt = 0;
  icmd.cmd = RECORD_CFG_PROGRAM_PID;
  icmd.p_para = &rec_pg_pid;
  p_rec_filter->do_command(p_rec_filter, &icmd);

  icmd.cmd = RECORD_FILTER_CMD_TIMESHIFT_CFG;
  icmd.p_para = (void *)p_ctrl->p_media_idx;
  icmd.lpara = p_ctrl->rec_attr.b_rewind;
  p_rec_filter->do_command(p_rec_filter, &icmd);

  icmd.cmd = FSINK_CFG_SINK_MODE;
  icmd.p_para = (void *)p_ctrl->p_media_idx;
  if(p_ctrl->rec_attr.b_rewind)
  {
    icmd.lpara = FSINK_SINK_TO_BLOCK;
#ifdef WIN32
    dmx_set_timeshift(1);
#endif
  }
  else
  {
    icmd.lpara = FSINK_SINK_TO_FILE;
  }

  p_fsink_filter->do_command(p_fsink_filter, &icmd);

  icmd.cmd = FSINK_CFG_SINK_SIZE;
  icmd.p_para = (void *)&p_ctrl->rec_attr.max_file_size;
  icmd.lpara = 2 * MBYTES;
  p_fsink_filter->do_command(p_fsink_filter, &icmd);

  icmd.cmd = FSINK_CFG_FILE_NAME;
  icmd.p_para = p_ctrl->rec_attr.file_name;
  icmd.lpara = 0;
  p_fsink_filter->do_command(p_fsink_filter, &icmd);

  ret = p_ctrl->p_chain->open(p_ctrl->p_chain);
  if (ret != SUCCESS)
  {
    OS_PRINTF("\n##debug: p_chain->open fail!\n");
    return MUL_ERR_PVR_FILE_CANT_OPEN;
  }
  ret = p_ctrl->p_chain->start(p_ctrl->p_chain);

  if(ret != SUCCESS)
  {
    OS_PRINTF("\n##debug: p_chain->start fail!\n");
    return MUL_ERR_PVR_FILE_CANT_OPEN;
  }
  else
  {
    p_ctrl->status.state = MUL_PVR_REC_STATE_RUNNING;
    return SUCCESS;
  }
}


RET_CODE mul_pvr_rec_pause_chn(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  ret = p_ctrl->p_chain->pause(p_ctrl->p_chain);

  p_ctrl->status.state = MUL_PVR_REC_STATE_PAUSE;
  return ret;

}

RET_CODE mul_pvr_rec_resume_chn(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  RET_CODE ret = SUCCESS;
  icmd_t icmd = {0};
  ifilter_t *p_demux_filter = NULL;
  
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  p_demux_filter = p_ctrl->p_demux_filter;

  icmd.cmd = DMX_PAUSE_RESUME;
  icmd.p_para = (void *)p_ctrl->p_rec_pin;
  icmd.lpara = 1;
  p_demux_filter->do_command(p_demux_filter, &icmd);

  ret = p_ctrl->p_chain->resume(p_ctrl->p_chain);

  p_ctrl->status.state = MUL_PVR_REC_STATE_RUNNING;
  return ret;

}

RET_CODE mul_pvr_rec_change_pid(u32 chanid, mul_rec_media_t *p_media)
{
  RET_CODE ret = SUCCESS;
  icmd_t icmd = {0};
  ifilter_t *p_demux_filter = NULL;
  ifilter_t *p_rec_filter = NULL;
  rec_buf_cfg_t dmx_cfg = {0};
  rec_reset_pid_data_t reset_req = {0};
  rec_filter_pg_pid_t rec_pg_pid;
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];
  
  p_demux_filter = p_ctrl->p_demux_filter;
  p_rec_filter = p_ctrl->p_rec_filter;
  
  dmx_cfg.rec_mode = DMX_PARTIAL_TS_PACKET_DESCRAMBLE;

  if ((p_ctrl->rec_attr.media_info.v_pid != p_media->v_pid)
    || (p_ctrl->rec_attr.media_info.a_pid != p_media->a_pid)
    || (p_ctrl->rec_attr.media_info.pcr_pid != p_media->pcr_pid))
  {
    icmd.cmd = DMX_RESET_PID;
    icmd.p_para = (void *)p_ctrl->p_rec_pin;
    reset_req.old_v_pid = p_ctrl->rec_attr.media_info.v_pid;
    reset_req.new_v_pid = p_media->v_pid;
    reset_req.old_a_pid = p_ctrl->rec_attr.media_info.a_pid;
    reset_req.new_a_pid = p_media->a_pid;
    reset_req.demux_index = p_ctrl->rec_attr.demux_index;
    if ((p_ctrl->rec_attr.media_info.pcr_pid != p_ctrl->rec_attr.media_info.v_pid)
      && (p_ctrl->rec_attr.media_info.pcr_pid != p_ctrl->rec_attr.media_info.a_pid))
    {
      reset_req.old_pcr_pid = p_ctrl->rec_attr.media_info.pcr_pid;
    }
    else
    {
      reset_req.old_pcr_pid = p_media->pcr_pid;
    }
    reset_req.new_pcr_pid = p_media->pcr_pid;
    
    reset_req.rec_mode = dmx_cfg.rec_mode;
    icmd.lpara = (u32)&reset_req;
    if (p_demux_filter->do_command(p_demux_filter, &icmd) != SUCCESS)
    {
      OS_PRINTF("\n##mul_pvr_rec_change_pid fail!\n");
      return ERR_FAILURE;
    }
  }

  p_ctrl->rec_attr.media_info.v_pid = p_media->v_pid;
  p_ctrl->rec_attr.media_info.video_type = p_media->video_type;
  p_ctrl->rec_attr.media_info.a_pid = p_media->a_pid;
  p_ctrl->rec_attr.media_info.audio_type = p_media->audio_type;
  p_ctrl->rec_attr.media_info.pcr_pid = p_media->pcr_pid;
  
  rec_pg_pid.record_mode = dmx_cfg.rec_mode;
  rec_pg_pid.v_pid = p_ctrl->rec_attr.media_info.v_pid;
  rec_pg_pid.a_pid = p_ctrl->rec_attr.media_info.a_pid;
  rec_pg_pid.pcr_pid = p_ctrl->rec_attr.media_info.pcr_pid;
  rec_pg_pid.video_fmt = p_ctrl->rec_attr.media_info.video_type;
  rec_pg_pid.audio_fmt = p_ctrl->rec_attr.media_info.audio_type;
  rec_pg_pid.ext_pid_cnt = 0;
  icmd.cmd = RECORD_CFG_PROGRAM_PID;
  icmd.p_para = &rec_pg_pid;
  p_rec_filter->do_command(p_rec_filter, &icmd);
  
  return ret;
}

RET_CODE mul_pvr_rec_stop_chn(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
  RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];


  p_ctrl->status.state = MUL_PVR_REC_STATE_STOPPING;
  ret = p_ctrl->p_chain->stop(p_ctrl->p_chain);

  ret = p_ctrl->p_chain->close(p_ctrl->p_chain);
  p_ctrl->status.state = MUL_PVR_REC_STATE_STOP;
  return ret;

}

RET_CODE mul_pvr_rec_get_status(u32 chanid, mul_pvr_rec_status_t *p_rec_status)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;
//  RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  memcpy(p_rec_status, &p_ctrl->status, sizeof(mul_pvr_rec_status_t));

  return SUCCESS;
}

mul_pvr_rec_status_t *mul_pvr_rec_get_status_h(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  rec_control_t *p_ctrl = NULL;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return NULL;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_REC_CHN_NUM
    || (!p_this->rec_ctrl[chanid].used))
  {
    return NULL;
  }
  p_ctrl = &p_this->rec_ctrl[chanid];

  return &p_ctrl->status;
}

RET_CODE mul_pvr_play_create_chn(u32 *p_chnid, const mul_pvr_play_attr_t *p_play_attr)
{
  RET_CODE ret = SUCCESS;
  char chan_name[30] = {0};
  ifilter_t *p_fsrc_filter = NULL;
  ifilter_t *p_ts_player_filter = NULL;
  ifilter_t *p_av_render_filter = NULL;
  mul_pvr_priv_t *p_this = NULL;
  u32 play_chanid = INVAILD_CHN_ID;
  play_control_t *p_play_ctrl = NULL;
  ctrl_para_t ctrl_para = {0};
  chain_para_t para = {0};
  media_format_t format = {MT_RECODE};

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  play_chanid = s_pvr_find_free_play_chain(p_this);
  if(play_chanid == INVAILD_CHN_ID)
  {
    return MUL_ERR_PVR_NO_CHN_LEFT;
  }
  p_play_ctrl = &p_this->play_ctrl[play_chanid];

// create chain
  memcpy(&p_play_ctrl->play_attr, p_play_attr, sizeof(mul_pvr_play_attr_t));
  p_play_ctrl->play_attr.parse_buf_size =
    (p_play_ctrl->play_attr.parse_buf_size / PLAY_FILL_UNIT_SIZE) * PLAY_FILL_UNIT_SIZE;
  ctrl_para.user_handle = (handle_t)play_chanid;
  controller_create(&p_play_ctrl->play_ctrler, &ctrl_para);
  p_play_ctrl->play_ctrler.on_process_evt = s_pvr_on_filter_evt;

  para.p_owner = (interface_t *)&p_play_ctrl->play_ctrler;
  sprintf(chan_name, "play_chain_%ld", play_chanid);
  para.p_name = chan_name;
  para.stk_size = p_play_attr->stk_size;
  p_play_ctrl->p_chain = chain_create(&para);
  p_play_ctrl->attach_rec_chain = INVAILD_CHN_ID;

  //create filter
  ret = eva_add_filter_by_id(FILE_SOURCE_FILTER, &p_play_ctrl->p_source_filter, NULL);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }
  ret = eva_add_filter_by_id(TS_PLAYER_FILTER, &p_play_ctrl->p_player_filter, NULL);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }
  ret = eva_add_filter_by_id(AV_RENDER_FILTER, &p_play_ctrl->p_av_render_filter, &p_play_ctrl->play_attr);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }

  p_fsrc_filter = p_play_ctrl->p_source_filter;
  p_ts_player_filter = p_play_ctrl->p_player_filter;
  p_av_render_filter = p_play_ctrl->p_av_render_filter;

  //add to chain
  sprintf(chan_name, "play_fsrc_filter_%ld", play_chanid);
  ret = p_play_ctrl->p_chain->add_filter(p_play_ctrl->p_chain, p_fsrc_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }
  sprintf(chan_name, "play_play_filter_%ld", play_chanid);
  ret = p_play_ctrl->p_chain->add_filter(p_play_ctrl->p_chain, p_ts_player_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }
  sprintf(chan_name, "play_render_filter_%ld", play_chanid);
  ret = p_play_ctrl->p_chain->add_filter(p_play_ctrl->p_chain, p_av_render_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }

  //1. >>-------------link source filter and player filter ------------------------>>>>
  format.stream_type = MT_FILE_DATA;
  ret = p_play_ctrl->p_chain->connect(p_play_ctrl->p_chain,
        p_fsrc_filter, p_ts_player_filter, &format);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }

  //2. >>-------------link player filter and render filter ------------------------>>>>
  format.stream_type = MT_TS_PKT;
  ret = p_play_ctrl->p_chain->connect(p_play_ctrl->p_chain,
              p_ts_player_filter, p_av_render_filter, &format);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }

  *p_chnid = play_chanid | PLAY_CHAN_BIT;
  return SUCCESS;
}


RET_CODE mul_pvr_play_destroy_chn(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  //RET_CODE ret = SUCCESS;
  play_control_t *p_play_ctrl = NULL;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xF;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_play_ctrl = &p_this->play_ctrl[chanid];

  if (p_play_ctrl->p_media_idx)
  {
    media_idx_destory(p_play_ctrl->p_media_idx);
    p_play_ctrl->p_media_idx = NULL;
  }

  p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);

  memset(p_play_ctrl, 0, sizeof(play_control_t));
  return SUCCESS;
}


RET_CODE mul_pvr_play_start_timeshift(u32 *p_play_chnid,
            u32 rec_chnid, mul_pvr_play_attr_t *p_play_attr)
{
  RET_CODE ret = SUCCESS;
  icmd_t icmd = {0};
  char chan_name[30] = {0};
  ifilter_t *p_fsrc_filter = NULL;
  ifilter_t *p_ts_player_filter = NULL;
  ifilter_t *p_av_render_filter = NULL;
  src_pin_attr_t sp_attr = {0};
  ts_player_config_t player_config = {0};
  av_render_config_t av_config = {0};
  mul_pvr_priv_t *p_this = NULL;
  u32 play_chanid = INVAILD_CHN_ID;
  rec_control_t *p_rec_ctrl = NULL;
  play_control_t *p_play_ctrl = NULL;
  ctrl_para_t ctrl_para = {0};
  chain_para_t para = {0};
  media_format_t format = {MT_RECODE};
  u32 unit = 2; // //2 sample
  u32 unit_size = 0;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  rec_chnid = rec_chnid & 0xF;

  if(rec_chnid >= MAX_REC_CHN_NUM)
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  p_rec_ctrl = &p_this->rec_ctrl[rec_chnid];

  /*if (!p_rec_ctrl->rec_attr.b_clear_stream)
  {
    OS_PRINTF("\n##can not play cas ts!!\n");
    return -1;
  }*/
  play_chanid = s_pvr_find_free_play_chain(p_this);
  if(play_chanid == INVAILD_CHN_ID)
  {
    return MUL_ERR_PVR_NO_CHN_LEFT;
  }
  
  p_rec_ctrl->attach_play_chain = play_chanid;
  p_play_ctrl = &p_this->play_ctrl[play_chanid];

  p_play_ctrl->ts_mode = TS_PLAYBACK_MODE;
#ifdef WIN32
  dmx_set_timeshift(1);
#endif

// create chain
  memcpy(&p_play_ctrl->play_attr, p_play_attr, sizeof(mul_pvr_play_attr_t));

  p_play_ctrl->play_attr.parse_buf_size =
    (p_play_ctrl->play_attr.parse_buf_size / PLAY_FILL_UNIT_SIZE) * PLAY_FILL_UNIT_SIZE;

  ctrl_para.user_handle = (handle_t)play_chanid;
  controller_create(&p_play_ctrl->play_ctrler, &ctrl_para);
  p_play_ctrl->play_ctrler.on_process_evt = s_pvr_on_filter_evt;

  para.p_owner = (interface_t *)&p_play_ctrl->play_ctrler;
  sprintf(chan_name, "play_chain_%ld", play_chanid);
  para.p_name = chan_name;
  para.stk_size = p_play_attr->stk_size;
  p_play_ctrl->p_chain = chain_create(&para);

  //create filter
  ret = eva_add_filter_by_id(FILE_SOURCE_FILTER, &p_play_ctrl->p_source_filter, NULL);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }
  ret = eva_add_filter_by_id(TS_PLAYER_FILTER, &p_play_ctrl->p_player_filter, NULL);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }
  ret = eva_add_filter_by_id(AV_RENDER_FILTER, &p_play_ctrl->p_av_render_filter, &p_play_ctrl->play_attr);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_ADD_FAILED;
  }

  p_fsrc_filter = p_play_ctrl->p_source_filter;
  p_ts_player_filter = p_play_ctrl->p_player_filter;
  p_av_render_filter = p_play_ctrl->p_av_render_filter;

  //add to chain
  sprintf(chan_name, "play_fsrc_filter_%ld", play_chanid);
  ret = p_play_ctrl->p_chain->add_filter(p_play_ctrl->p_chain, p_fsrc_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }
  sprintf(chan_name, "play_play_filter_%ld", play_chanid);
  ret = p_play_ctrl->p_chain->add_filter(p_play_ctrl->p_chain, p_ts_player_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }
  sprintf(chan_name, "play_render_filter_%ld", play_chanid);
  ret = p_play_ctrl->p_chain->add_filter(p_play_ctrl->p_chain, p_av_render_filter, chan_name);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_FILTER_INSERT_CHN;
  }

  //1. >>-------------link source filter and player filter ------------------------>>>>
  format.stream_type = MT_FILE_DATA;
  ret = p_play_ctrl->p_chain->connect(p_play_ctrl->p_chain,
      p_fsrc_filter, p_ts_player_filter, &format);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }

  //2. >>-------------link player filter and render filter ------------------------>>>>
  format.stream_type = MT_TS_PKT;
  ret = p_play_ctrl->p_chain->connect(p_play_ctrl->p_chain,
                      p_ts_player_filter, p_av_render_filter, &format);
  if(ret != SUCCESS)
  {
    p_play_ctrl->p_chain->_interface._destroy(p_play_ctrl->p_chain);
    p_play_ctrl->used = FALSE;
    return MUL_ERR_PVR_EVA_CONNECT_FAILED;
  }

// start chain

  p_fsrc_filter = p_play_ctrl->p_source_filter;
  p_ts_player_filter = p_play_ctrl->p_player_filter;
  p_av_render_filter = p_play_ctrl->p_av_render_filter;

  //because the file source pin need align 
  unit_size = (p_play_attr->play_buffer_size / unit) - 8;
  if (unit_size > 376 * KBYTES)
  {
    unit_size = 4 * TS_STREAM_RW_UNIT;
  }
  
  //config pin
  sp_attr.buffers = unit;
  sp_attr.buffer_size = unit_size;
  sp_attr.p_extern_buf = p_play_attr->p_play_buffer;
  sp_attr.extern_buf_size = p_play_attr->play_buffer_size;
  sp_attr.is_share = TRUE;
  icmd.cmd = FSRC_CFG_SOURCE_PIN;
  sp_attr.src = SRC_FROM_BLOCK;
  icmd.p_para = &sp_attr;
  p_fsrc_filter->do_command(p_fsrc_filter, &icmd);

  //config source pin
  icmd.cmd = FSRC_CFG_FILE_NAME;
  icmd.p_para = p_rec_ctrl->rec_attr.file_name;
  p_fsrc_filter->do_command(p_fsrc_filter, &icmd);

  player_config.video_pid = p_rec_ctrl->rec_attr.media_info.v_pid;
  player_config.video_fmt = p_rec_ctrl->rec_attr.media_info.video_type;
  player_config.audio_pid = p_rec_ctrl->rec_attr.media_info.a_pid;
  player_config.audio_fmt = p_rec_ctrl->rec_attr.media_info.audio_type;
  player_config.load_unit_size = (unit_size / TS_STREAM_RW_UNIT) * TS_STREAM_RW_UNIT;
  CHECK_FAIL_RET_CODE(player_config.load_unit_size != 0);

  player_config.p_buffer = p_play_attr->p_parse_buf;
  player_config.buffer_size = p_play_attr->parse_buf_size;
  player_config.player_src = TS_PLAYER_SRC_REC_BLOCK;
  player_config.pcr_pid = p_rec_ctrl->rec_attr.media_info.pcr_pid;
  player_config.p_media_idx = p_rec_ctrl->p_media_idx;

  icmd.cmd = TS_PLAYER_CMD_CFG;
  icmd.lpara = (u32)&player_config;
  p_ts_player_filter->do_command(p_ts_player_filter, &icmd);

  icmd.cmd = TS_PLAYER_CMD_SET_PLAY_MODE;
  icmd.lpara = (u32)NORMAL_PLAY;
  p_ts_player_filter->do_command(p_ts_player_filter, &icmd);

  av_config.video_pid = player_config.video_pid;
  av_config.audio_pid = player_config.audio_pid;
  av_config.pcr_pid = player_config.pcr_pid;
  if (!av_config.pcr_pid)
  {
    av_config.pcr_pid = av_config.video_pid;
  }
  av_config.video_fmt = p_rec_ctrl->rec_attr.media_info.video_type;
  av_config.audio_fmt = p_rec_ctrl->rec_attr.media_info.audio_type;
  icmd.cmd = AV_RENDER_CFG;
  icmd.lpara = (u32)(&av_config);
  p_av_render_filter->do_command(p_av_render_filter, &icmd);

  ret = p_play_ctrl->p_chain->open(p_play_ctrl->p_chain);
  ret |= p_play_ctrl->p_chain->start(p_play_ctrl->p_chain);
  p_play_ctrl->status.state = MUL_PVR_PLAY_STATE_PLAY;
  p_play_ctrl->status.speed = MUL_PVR_PLAY_SPEED_NORMAL;
  p_play_ctrl->attach_rec_chain = rec_chnid;

  *p_play_chnid = play_chanid | PLAY_CHAN_BIT;

  if(ret != SUCCESS)
  {
    return MUL_ERR_PVR_FILE_CANT_OPEN;
  }
  else
  {
    return SUCCESS;
  }
}


RET_CODE mul_pvr_play_stop_timeshift(u32 play_chnid, const mul_av_play_stop_opt_t *p_stop_opt)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
//  RET_CODE ret = SUCCESS;


  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  play_chnid = play_chnid & 0xf;
  if(play_chnid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[play_chnid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[play_chnid];

#ifdef WIN32
  dmx_set_timeshift(0);
#endif

  p_ctrl->p_chain->stop(p_ctrl->p_chain);

  p_ctrl->p_chain->close(p_ctrl->p_chain);
  p_ctrl->status.state = MUL_PVR_PLAY_STATE_STOP;
  p_ctrl->p_chain->_interface._destroy(p_ctrl->p_chain);
  memset(p_ctrl, 0, sizeof(play_control_t));

  return SUCCESS;
}


RET_CODE mul_pvr_play_set_chn(u32 chanid, const mul_pvr_play_attr_t *p_play_attr)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
 // RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];

  memcpy(&p_ctrl->play_attr, p_play_attr, sizeof(mul_pvr_play_attr_t));

  return SUCCESS;
}


RET_CODE mul_pvr_play_get_chn(u32 chanid, mul_pvr_play_attr_t *p_play_attr)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
//  RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];

  memcpy(p_play_attr, &p_ctrl->play_attr, sizeof(mul_pvr_play_attr_t));

  return SUCCESS;
}


RET_CODE mul_pvr_play_start_chn(u32 chanid)
{
  ifilter_t *p_fsrc_filter = NULL;
  ifilter_t *p_ts_player_filter = NULL;
  ifilter_t *p_av_render_filter = NULL;
  src_pin_attr_t sp_attr = {0};
  ts_player_config_t player_config = {0};
  av_render_config_t av_config = {0};
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
  RET_CODE ret = SUCCESS;
  icmd_t icmd = {0};
  u32 unit = 2; // //2 sample
  u32 unit_size = 0;
  u16 idx_file_name[MAX_FILE_PATH] = {0};

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  p_ctrl = &p_this->play_ctrl[chanid];
  p_ctrl->attach_rec_chain = INVAILD_CHN_ID;
  /*if (!p_ctrl->play_attr.is_clear_stream)
  {
    OS_PRINTF("\n##can not play cas ts!!\n");
    return -1;
  }*/

  p_fsrc_filter = p_ctrl->p_source_filter;
  p_ts_player_filter = p_ctrl->p_player_filter;
  p_av_render_filter = p_ctrl->p_av_render_filter;

  //because the file source pin need align 
  unit_size = (p_ctrl->play_attr.play_buffer_size / unit) - 8;

  if (unit_size > 376 * KBYTES)
  {
    unit_size = 4 * TS_STREAM_RW_UNIT;
  }

  sp_attr.buffers = unit;
  sp_attr.buffer_size = unit_size;
  sp_attr.p_extern_buf = p_ctrl->play_attr.p_play_buffer;
  sp_attr.extern_buf_size = p_ctrl->play_attr.play_buffer_size;
  sp_attr.is_share = FALSE;
  icmd.cmd = FSRC_CFG_SOURCE_PIN;
  sp_attr.src = SRC_FROM_FILE;
  icmd.p_para = &sp_attr;
  p_fsrc_filter->do_command(p_fsrc_filter, &icmd);

  //config source pin
  icmd.cmd = FSRC_CFG_FILE_NAME;
  icmd.p_para = p_ctrl->play_attr.file_name;
  p_fsrc_filter->do_command(p_fsrc_filter, &icmd);

  player_config.video_pid = p_ctrl->play_attr.v_pid;
  player_config.video_fmt = p_ctrl->play_attr.v_type;
  player_config.audio_pid = p_ctrl->play_attr.a_pid;
  player_config.audio_fmt = p_ctrl->play_attr.a_type;
  player_config.load_unit_size = (unit_size / TS_STREAM_RW_UNIT) * TS_STREAM_RW_UNIT;
  CHECK_FAIL_RET_CODE(player_config.load_unit_size != 0);

  player_config.p_buffer = p_ctrl->play_attr.p_parse_buf;
  player_config.buffer_size = p_ctrl->play_attr.parse_buf_size;

  player_config.player_src = TS_PLAYER_SRC_REC_FILE;
  player_config.pcr_pid = p_ctrl->play_attr.pcr_pid;

  p_ctrl->p_media_idx = NULL;
  get_idx_info_file(p_ctrl->play_attr.file_name, idx_file_name, MAX_FILE_PATH);
  if (!media_idx_load(&p_ctrl->p_media_idx,
    1024 * 2, idx_file_name, p_ctrl->play_attr.p_ridx_buf, p_ctrl->play_attr.ridx_buf_size))
  {
    return MUL_ERR_PVR_EVA_FILTER_NO_PIN;
  }
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  p_ctrl->cipher_engine = p_ctrl->p_media_idx->extern_info.cipher_engine;
  memcpy(p_ctrl->cipher_key, p_ctrl->p_media_idx->extern_info.cipher_key, ADVANCEDCA_MAXKEYSIZE);
  if(p_ctrl->cipher_engine != p_this->cipher_engine)
  {
      return MUL_ERR_PVR_CIPHER_KEY_ERROR;
  }
  else if(p_this->cipher_engine != 0)
  {
      int k = 0;
      for(k = 0; k < ADVANCEDCA_MAXKEYSIZE; k++)
      {
          if(p_ctrl->cipher_key[k] != p_this->cipher_key[k])
          {
              return MUL_ERR_PVR_CIPHER_KEY_ERROR;
          }
      }
  }
  #endif
  player_config.p_media_idx = p_ctrl->p_media_idx;

  icmd.cmd = TS_PLAYER_CMD_CFG;
  icmd.lpara = (u32)&player_config;
  p_ts_player_filter->do_command(p_ts_player_filter, &icmd);

  icmd.cmd = TS_PLAYER_CMD_SET_PLAY_MODE;
  icmd.lpara = (u32)NORMAL_PLAY;
  p_ts_player_filter->do_command(p_ts_player_filter, &icmd);

  av_config.video_pid = player_config.video_pid;
  av_config.audio_pid = player_config.audio_pid;
  av_config.pcr_pid = player_config.pcr_pid;
  if (!av_config.pcr_pid)
  {
    av_config.pcr_pid = av_config.video_pid;
  }
  av_config.video_fmt = p_ctrl->play_attr.v_type;
  av_config.audio_fmt = p_ctrl->play_attr.a_type;
  icmd.cmd = AV_RENDER_CFG;
  icmd.lpara = (u32)(&av_config);
  p_av_render_filter->do_command(p_av_render_filter, &icmd);

  ret = p_ctrl->p_chain->open(p_ctrl->p_chain);
  ret |= p_ctrl->p_chain->start(p_ctrl->p_chain);
  p_ctrl->status.state = MUL_PVR_PLAY_STATE_PLAY;
  p_ctrl->status.speed = MUL_PVR_PLAY_SPEED_NORMAL;

  if(ret != SUCCESS)
  {
    return MUL_ERR_PVR_FILE_CANT_OPEN;
  }
  else
  {
  }
  return SUCCESS;
}


RET_CODE mul_pvr_play_stop_chn(u32 chanid, const mul_av_play_stop_opt_t *p_stop_opt)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
//  RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];

#ifdef WIN32
  dmx_set_timeshift(0);
#endif
  p_ctrl->p_chain->stop(p_ctrl->p_chain);
  p_ctrl->p_chain->close(p_ctrl->p_chain);
  p_ctrl->status.state = MUL_PVR_PLAY_STATE_STOP;
  return SUCCESS;
}


RET_CODE mul_pvr_play_pause_chn(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
//  RET_CODE ret = SUCCESS;
  //u32 t = mtos_ticks_get();

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];
  OS_PRINTF("\n##debug: player pause!!\n");
  p_ctrl->p_chain->pause(p_ctrl->p_chain);
  p_ctrl->status.state = MUL_PVR_PLAY_STATE_PAUSE;
  p_ctrl->ts_mode = TS_PLAYBACK_MODE;
  OS_PRINTF("\n##debug: set ts palyback mode by pause!\n");
  //OS_PRINTF("XXXXXXXXXX %s spend %d tick\n", __FUNCTION__, mtos_ticks_get() -t);
  return SUCCESS;
}


RET_CODE mul_pvr_play_resume_chn(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
  rec_control_t *p_rec_ctrl = NULL;
  BOOL b_need_seek = FALSE;
  icmd_t icmd = {0};
  ts_player_seek_param_t seek = {0};
  ifilter_t *p_player_filter = NULL;
  //u32 t = mtos_ticks_get();

//  RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];
  if((p_ctrl->attach_rec_chain != INVAILD_CHN_ID)
    && ((p_ctrl->attach_rec_chain & 0xf) < MAX_REC_CHN_NUM))
  {
    p_rec_ctrl = &p_this->rec_ctrl[p_ctrl->attach_rec_chain & 0xf];
    if(p_rec_ctrl->status.start_time_ms
      && (p_rec_ctrl->status.start_time_ms >= p_ctrl->status.cur_play_time_ms))
    {
      b_need_seek = TRUE;
    }
  }

  OS_PRINTF("\n##mul_pvr_play_resume_chn\n");
  p_ctrl->p_chain->resume(p_ctrl->p_chain);

  if (b_need_seek)
  {
    OS_PRINTF("\n##last play pos do not exist, need seek to head!\n");
    seek.seek_type = TS_PLAYER_TIME_SEEK; 
    seek.seek_op = TS_PLAYER_SEEK_CUR;
    seek.offset = (p_rec_ctrl->status.start_time_ms - p_ctrl->status.cur_play_time_ms) / 1000;

    p_player_filter = p_ctrl->p_player_filter;
    OS_PRINTF("\n##resume do seek[rec start:%ld, play:%ld] seek cur[%lld]\n",
      p_rec_ctrl->status.start_time_ms / 1000,
      p_ctrl->status.cur_play_time_ms / 1000, seek.offset);
    icmd.cmd = TS_PLAYER_CMD_SEEK;
    icmd.p_para = (void *)&seek;
    p_player_filter->do_command(p_player_filter, &icmd);
  }

  p_ctrl->status.state = MUL_PVR_PLAY_STATE_PLAY;
  //OS_PRINTF("XXXXXXXXXX %s spend %d tick\n", __FUNCTION__, mtos_ticks_get() -t);
  return SUCCESS;
}


RET_CODE mul_pvr_play_trick_play(u32 chanid, const mul_pvr_play_mode_t *p_trick_mode)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
//  RET_CODE ret = SUCCESS;
  icmd_t icmd = {0};
  ts_play_mode_t ts_mode = NORMAL_PLAY;
  ifilter_t *p_ts_player_filter = NULL;
  u32 times = 0;
  //u32 t = mtos_ticks_get();
  OS_PRINTF("mul_pvr_play_trick_play speed %d\n", p_trick_mode->speed);
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];
  p_ts_player_filter = p_ctrl->p_player_filter;

  p_ctrl->ts_mode = TS_PLAYBACK_MODE;
  switch(p_trick_mode->speed)
  {
    case MUL_PVR_PLAY_SPEED_NORMAL:
      ts_mode = NORMAL_PLAY;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_PLAY;
      break;
    case MUL_PVR_PLAY_SPEED_2X_FAST_FORWARD:
      ts_mode = FAST_FORWARD;
      times = 2;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FF;
      break;
    case MUL_PVR_PLAY_SPEED_4X_FAST_FORWARD:
      ts_mode = FAST_FORWARD;
      times = 4;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FF;
      break;
    case MUL_PVR_PLAY_SPEED_8X_FAST_FORWARD:
      ts_mode = FAST_FORWARD;
      times = 8;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FF;
      break;
    case MUL_PVR_PLAY_SPEED_16X_FAST_FORWARD:
      ts_mode = FAST_FORWARD;
      times = 16;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FF;
      break;
    case MUL_PVR_PLAY_SPEED_32X_FAST_FORWARD:
      ts_mode = FAST_FORWARD;
      times = 32;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FF;
      break;

    case MUL_PVR_PLAY_SPEED_2X_FAST_BACKWARD:
      ts_mode = FAST_BACKWARD;
      times = 2;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FB;
      break;
    case MUL_PVR_PLAY_SPEED_4X_FAST_BACKWARD:
      ts_mode = FAST_BACKWARD;
      times = 4;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FB;
      break;
    case MUL_PVR_PLAY_SPEED_8X_FAST_BACKWARD:
      ts_mode = FAST_BACKWARD;
      times = 8;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FB;
      break;
    case MUL_PVR_PLAY_SPEED_16X_FAST_BACKWARD:
      ts_mode = FAST_BACKWARD;
      times = 16;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FB;
      break;
    case MUL_PVR_PLAY_SPEED_32X_FAST_BACKWARD:
      ts_mode = FAST_BACKWARD;
      times = 32;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_FB;
      break;
    case MUL_PVR_PLAY_SPEED_2X_SLOW_FORWARD:
        ts_mode = SLOW_FORWARD;
        times = 2;
        p_ctrl->status.state = MUL_PVR_PLAY_STATE_SF;
        break;
    case MUL_PVR_PLAY_SPEED_4X_SLOW_FORWARD:
        ts_mode = SLOW_FORWARD;
        times = 4;
        p_ctrl->status.state = MUL_PVR_PLAY_STATE_SF;
        break;
    case MUL_PVR_PLAY_SPEED_8X_SLOW_FORWARD:
        ts_mode = SLOW_FORWARD;
        times = 8;
        p_ctrl->status.state = MUL_PVR_PLAY_STATE_SF;
      break;
     case MUL_PVR_PLAY_SPEED_16X_SLOW_FORWARD:
       ts_mode = SLOW_FORWARD;
       times = 16;
       p_ctrl->status.state = MUL_PVR_PLAY_STATE_SF;
       break;
     case MUL_PVR_PLAY_SPEED_32X_SLOW_FORWARD:
      ts_mode = SLOW_FORWARD;
      times = 32;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_SF;
      break;
    case MUL_PVR_PLAY_SPEED_2X_SLOW_BACKWARD:
      ts_mode = SLOW_BACKWARD;
      times = 2;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_SB;
      break;
    case MUL_PVR_PLAY_SPEED_4X_SLOW_BACKWARD:
      ts_mode = SLOW_BACKWARD;
      times = 4;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_SB;
      break;
    case MUL_PVR_PLAY_SPEED_8X_SLOW_BACKWARD:
      ts_mode = SLOW_BACKWARD;
      times = 8;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_SB;
      break;
    case MUL_PVR_PLAY_SPEED_16X_SLOW_BACKWARD:
      ts_mode = SLOW_BACKWARD;
      times = 16;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_SB;
      break;
    case MUL_PVR_PLAY_SPEED_32X_SLOW_BACKWARD:
      ts_mode = SLOW_BACKWARD;
      times = 32;
      p_ctrl->status.state = MUL_PVR_PLAY_STATE_SB;
      break;

    default:
      break;
  }

  p_ctrl->status.speed = p_trick_mode->speed;

  icmd.cmd = TS_PLAYER_CMD_SET_PLAY_MODE;
  icmd.lpara = ts_mode;
  icmd.p_para = (void *)&times;
  p_ts_player_filter->do_command(p_ts_player_filter, &icmd);

  //OS_PRINTF("XXXXXXXXXX %s spend %d tick\n", __FUNCTION__, mtos_ticks_get() -t);
  return SUCCESS;
}


RET_CODE mul_pvr_play_seek(u32 chanid, const mul_pvr_play_position_t *p_position)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
  rec_control_t *p_rec_ctrl = NULL;
//  RET_CODE ret = SUCCESS;
  ifilter_t *p_ts_player_filter = NULL;
  icmd_t icmd = {0};
  ts_player_seek_param_t seek = {0};
  u32 seek_offset = 0;

  //u32 t = mtos_ticks_get();
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  if(p_position == NULL)
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];

  p_ts_player_filter = p_ctrl->p_player_filter;

  seek.offset = p_position->offset;

  if(p_position->pos_type == MUL_PVR_PLAY_POS_TYPE_TIME)
  {
    seek.seek_type = TS_PLAYER_TIME_SEEK;
    seek.offset = p_position->offset / 1000;
  }
  else if(p_position->pos_type == MUL_PVR_PLAY_POS_TYPE_SIZE)
  {
    seek.seek_type = TS_PLAYER_POSITION_SEEK;
  }
  else if(p_position->pos_type == MUL_PVR_PLAY_POS_TYPE_FRAME)
  {
    seek.seek_type = TS_PLAYER_FRAME_SEEK;
  }
  else
  {
    seek.seek_type = TS_PLAYER_TIME_SEEK;
  }

  seek.seek_op = p_position->whence;
  if (((p_ctrl->attach_rec_chain != INVAILD_CHN_ID)
    && ((p_ctrl->attach_rec_chain & 0xf) < MAX_REC_CHN_NUM)))
  {
    p_rec_ctrl = &p_this->rec_ctrl[p_ctrl->attach_rec_chain & 0xf];
    if (p_position->pos_type == MUL_PVR_PLAY_POS_TYPE_TIME)
    {
      switch (p_position->whence)
      {
      case MUL_PVR_SEEK_SET:
        if (p_position->offset >= 0)
        {
          seek_offset = p_position->offset;
        }
        else
        {
          seek_offset = 0;
        }
        
        if (seek_offset <= p_rec_ctrl->status.start_time_ms)
        {
          seek_offset = p_rec_ctrl->status.start_time_ms;
        }
        
        if (seek_offset > p_ctrl->status.cur_play_time_ms)
        {
          seek.offset = (seek_offset - p_ctrl->status.cur_play_time_ms) / 1000;
        }
        else
        {
          seek.offset = (s64)(0 - (p_ctrl->status.cur_play_time_ms - seek_offset) / 1000);
        }
        seek.seek_op = MUL_PVR_SEEK_CUR;
        OS_PRINTF("\n##seek do seek[rec start:%ld, play:%ld]seek set[%ld]  seek cur[%lld]\n",
          p_rec_ctrl->status.start_time_ms / 1000,
          p_ctrl->status.cur_play_time_ms / 1000,
          p_position->offset / 1000, seek.offset);
        break;
      case MUL_PVR_SEEK_CUR:
        seek_offset = p_ctrl->status.cur_play_time_ms + p_position->offset;
        break;
      case MUL_PVR_SEEK_END:
        if (p_position->offset == 0)
        {
          seek_offset = p_rec_ctrl->status.end_time_ms;
        }
        break;
      default:
        break;
      }
    }
    
    if (p_position->pos_type == MUL_PVR_PLAY_POS_TYPE_SIZE)
    {
      switch (p_position->whence)
      {
      case MUL_PVR_SEEK_SET:
        seek_offset = p_position->offset;
        break;
      case MUL_PVR_SEEK_CUR:
        seek_offset = p_ctrl->status.cur_play_pos + p_position->offset;
        break;
      case MUL_PVR_SEEK_END:
        if (p_position->offset == 0)
        {
          seek_offset = p_rec_ctrl->status.cur_write_point;
        }
        break;
      default:
        break;
      }
    }
    
    if (((p_position->pos_type == MUL_PVR_PLAY_POS_TYPE_TIME)
        && (seek_offset / 1000) == (p_rec_ctrl->status.end_time_ms / 1000))
      || ((p_position->pos_type == MUL_PVR_PLAY_POS_TYPE_SIZE)
        && (seek_offset == p_rec_ctrl->status.cur_write_point)))
    {
      p_ctrl->ts_mode = TS_LIVE_MODE;
      OS_PRINTF("\n##debug: set ts live mode by seek!\n");
    }
    else
    {
      p_ctrl->ts_mode = TS_PLAYBACK_MODE;
      OS_PRINTF("\n##debug: set ts palyback mode by seek!\n");
    }
  }
  
  if(p_ctrl->status.state == MUL_PVR_PLAY_STATE_PAUSE)
  {
    OS_PRINTF("\n##debug: player resume!!\n");
    p_ctrl->p_chain->resume(p_ctrl->p_chain);
    p_ctrl->status.state = MUL_PVR_PLAY_STATE_PLAY;
  }

  icmd.cmd = TS_PLAYER_CMD_SEEK;
  icmd.p_para = (void *)&seek;
	OS_PRINTF("========>seek do command\n");
  p_ts_player_filter->do_command(p_ts_player_filter, &icmd);

  return SUCCESS;
}

RET_CODE mul_pvr_play_step(u32 chanid, u32 direction)
{
  //FIXME
  return ERR_FAILURE;
}

RET_CODE mul_pvr_play_change_audio(u32 chanid, u16 a_pid, u8 fmt)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
  icmd_t icmd = {0};
  ifilter_t *p_ts_player_filter = NULL;
  ifilter_t *p_av_render_filter = NULL;
  av_render_config_t av_config = {0};
  
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];
  p_ts_player_filter = p_ctrl->p_player_filter;
  p_av_render_filter = p_ctrl->p_av_render_filter;

  av_config.video_pid = p_ctrl->play_attr.v_pid;
  av_config.audio_pid = a_pid;//p_ctrl->play_attr.a_pid;
  av_config.pcr_pid = p_ctrl->play_attr.pcr_pid;
  if (!av_config.pcr_pid)
  {
    av_config.pcr_pid = av_config.video_pid;
  }
  av_config.video_fmt = p_ctrl->play_attr.v_type;
  av_config.audio_fmt = fmt;//p_ctrl->play_attr.a_type;
  icmd.cmd = AV_RENDER_CFG;
  icmd.lpara = (u32)(&av_config);
  p_av_render_filter->do_command(p_av_render_filter, &icmd);
  
  icmd.cmd = TS_PLAYER_CMD_CHANGE_AUDIO;
  icmd.lpara = ((u32)a_pid << 16) | fmt;
  p_ts_player_filter->do_command(p_ts_player_filter, &icmd);

  return SUCCESS;
}

RET_CODE mul_pvr_play_get_status(u32 chanid, mul_pvr_play_status_t *p_status)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;
//  RET_CODE ret = SUCCESS;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }
  p_ctrl = &p_this->play_ctrl[chanid];

  memcpy(p_status, &p_ctrl->status, sizeof(mul_pvr_play_status_t));
  return SUCCESS;
}

mul_pvr_play_status_t *mul_pvr_play_get_status_h(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_ctrl = NULL;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return NULL;
  }
  chanid = chanid & 0xf;
  if(chanid >= MAX_PLAY_CHN_NUM
    || (!p_this->play_ctrl[chanid].used))
  {
    return NULL;
  }
  p_ctrl = &p_this->play_ctrl[chanid];

  return &p_ctrl->status;
}

RET_CODE mul_pvr_register_event(u32 chanid, event_call_back callBack, void *p_args)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_play_ctrl = NULL;
  rec_control_t *p_rec_ctrl = NULL;
//  RET_CODE ret = SUCCESS;
  BOOL rec_chn = FALSE;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  if((chanid & 0xf000) == REC_CHAN_BIT)
  {
    rec_chn = TRUE;
  }
  chanid = chanid & 0xf;
  if((rec_chn && chanid >= MAX_REC_CHN_NUM)
    || (rec_chn && (!p_this->rec_ctrl[chanid].used)))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  if((!rec_chn && chanid >= MAX_PLAY_CHN_NUM)
    || (!rec_chn && (!p_this->play_ctrl[chanid].used)))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }


  if(rec_chn)
  {
    p_rec_ctrl = &p_this->rec_ctrl[chanid];
    p_rec_ctrl->call_back = callBack;
    p_rec_ctrl->context = (u32)p_args;
  }
  else
  {
    p_play_ctrl = &p_this->play_ctrl[chanid];
    p_play_ctrl->call_back = callBack;
    p_play_ctrl->context = (u32)p_args;
  }


  return SUCCESS;
}


RET_CODE mul_pvr_unregister_event(u32 chanid)
{
  mul_pvr_priv_t *p_this = NULL;
  play_control_t *p_play_ctrl = NULL;
  rec_control_t *p_rec_ctrl = NULL;
//  RET_CODE ret = SUCCESS;
  BOOL rec_chn = FALSE;

  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
    return MUL_ERR_PVR_NOT_INIT;
  }

  if((chanid & 0xf000) == REC_CHAN_BIT)
  {
    rec_chn = TRUE;
  }
  chanid = chanid & 0xf;

  if((rec_chn && chanid >= MAX_REC_CHN_NUM)
    || (rec_chn && (!p_this->rec_ctrl[chanid].used)))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }

  if((!rec_chn && chanid >= MAX_PLAY_CHN_NUM)
    || (!rec_chn && (!p_this->play_ctrl[chanid].used)))
  {
    return MUL_ERR_PVR_INVALID_PARA;
  }


  if(rec_chn)
  {
    p_rec_ctrl = &p_this->rec_ctrl[chanid];
    p_rec_ctrl->call_back = NULL;
    p_rec_ctrl->context = (u32)0;
  }
  else
  {
    p_play_ctrl = &p_this->play_ctrl[chanid];
    p_play_ctrl->call_back = NULL;
    p_play_ctrl->context = (u32)0;
  }

  return SUCCESS;
}


void  mul_pvr_config_debug_info(u8 debug_level)
{

  return;
}
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
/*
    configure cipher engine
*/
RET_CODE mul_pvr_config_cipher_engine(int engine, u8 *p_key, u8 keylength)
{
  mul_pvr_priv_t *p_this = NULL;
  RET_CODE ret = SUCCESS;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  if(p_this == NULL)
  {
      mul_pvr_init();
      p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  }
  
  p_this->cipher_engine = engine & 0xF;
  memset(p_this->cipher_key, 0, ADVANCEDCA_MAXKEYSIZE);
  if(keylength > ADVANCEDCA_MAXKEYSIZE)
  {
    keylength = ADVANCEDCA_MAXKEYSIZE;
  }
  memcpy(p_this->cipher_key, p_key, keylength);
 
  {
      int i = 0;
      for(i = 0; i < MAX_PLAY_CHN_NUM; i++)
      {
          if(p_this->play_ctrl[i].used == TRUE)
          {
              if((p_this->play_ctrl[i].status.state > MUL_PVR_PLAY_STATE_INIT) && 
                (p_this->play_ctrl[i].status.state  < MUL_PVR_PLAY_STATE_STOP))
              {
                  if(p_this->play_ctrl[i].cipher_engine != p_this->cipher_engine)
                  {
                      ret = MUL_ERR_PVR_CIPHER_KEY_ERROR;
                      break;
                  }
                  else if(p_this->cipher_engine != 0)
                  {
                      int j = 0;
                      for(j = 0; j < ADVANCEDCA_MAXKEYSIZE; j++)
                      {
                         if(p_this->play_ctrl[i].cipher_key[j] != p_this->cipher_key[j])
                         {
                            ret = MUL_ERR_PVR_CIPHER_KEY_ERROR;
                            break;
                         }
                      }
                  }
                    
              }
           }
      }
  }
  return ret;
}
/*
    get current cipher engine type and key
*/

RET_CODE mul_pvr_get_cipher_key(u8 *p_engine,u8 **p_key, u8 *p_keylength)
{
  mul_pvr_priv_t *p_this = NULL;
  p_this = class_get_handle_by_id(PVR_API_CLASS_ID);
  *p_engine = p_this->cipher_engine;  
  *p_key = p_this->cipher_key;
  *p_keylength = ADVANCEDCA_MAXKEYSIZE;
  return SUCCESS;
}
#endif


