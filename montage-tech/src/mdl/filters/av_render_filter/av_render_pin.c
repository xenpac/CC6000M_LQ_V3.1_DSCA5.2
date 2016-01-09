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
#include "lib_util.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "hal_misc.h"
#include "mt_time.h"
#include "vfs.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "common.h"
#include "drv_dev.h"
#include "dmx.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "lib_rect.h"
#include "display.h"
#include "lib_util.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"

//filter
#include "ifilter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"
#include "av_render_filter.h"
#include "av_render_pin_intra.h"
#include "av_render_filter_intra.h"
#include "pvr_api.h"
#include "err_check_def.h"

//#define DUMP_AV_TS_FILE 1
//#define DUMP_AV_TS_FILE 2

#ifdef DUMP_AV_TS_FILE
static hfile_t test_file = NULL;
#endif

//#define AV_RANDER_DBG_PRINT 1
#ifdef AV_RANDER_DBG_PRINT
static u32 cur_ticks = 0;
#endif
static av_render_pin_private_t *get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((av_render_pin_t *)_this)->private_data;
}

static RET_CODE on_open(handle_t _this)
{
  av_render_pin_private_t *p_priv = get_priv(_this);

  p_priv->p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_CODE(p_priv->p_dmx_dev != NULL);

  p_priv->p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  CHECK_FAIL_RET_CODE(p_priv->p_video_dev != NULL);

  p_priv->p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  CHECK_FAIL_RET_CODE(p_priv->p_audio_dev != NULL);

  p_priv->p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);  
  CHECK_FAIL_RET_CODE(p_priv->p_disp_dev != NULL);
  
  p_priv->b_video_show = 0;
  p_priv->b_video_freeze = 0;
  p_priv->p_resv_data = NULL;
  p_priv->resv_data[0] = 0x1F;
  p_priv->resv_data[1] = 0xFF;
  p_priv->is_clear_stream= 1;
  
#ifdef DUMP_AV_TS_FILE
  test_file = vfs_open("C:\\test_002.ts", VFS_NEW);
  if(test_file == NULL)
  {
    OS_PRINTF("open file failed\n");
    return ERR_FAILURE;
  }
#endif

  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  av_render_pin_private_t *p_priv = get_priv(_this);
#ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
#else
  chip_ic_t chip_ic_id = IC_WARRIORS;
#endif

#ifdef DUMP_AV_TS_FILE
  if (test_file)
  {
    vfs_close(test_file);
    test_file = NULL;
  }
#endif

  disp_layer_show(p_priv->p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
  if (chip_ic_id != IC_ENSEMBLE)
  {
    disp_layer_show(p_priv->p_disp_dev, DISP_LAYER_ID_VIDEO_HD, FALSE);
  }
  return SUCCESS;
}

static void on_receive(handle_t _this, media_sample_t *p_sample)
{
  sink_pin_t *p_sink_pin = (sink_pin_t *)_this;
  ipin_t *p_ipin = (ipin_t *)_this;
  av_render_filter_t *p_filter = (av_render_filter_t *)p_ipin ->get_filter(p_ipin);
  av_render_pin_private_t *p_priv = get_priv(_this);

  switch (p_sample->context)
  {
  case SAMPLE_CONTEXT_SEEK_FLAG:
    {
      OS_PRINTF("\n##av render seek reset!!\n");
      p_sink_pin->flush_async_render(p_sink_pin);
      p_filter->av_render_change_pid(p_filter, (dynamic_rec_info_t *)p_sample->p_user_data);
      p_sample->p_user_data = NULL;
      p_priv->b_video_freeze = 1;
      p_priv->p_resv_data = NULL;
    }
    break;
  case SAMPLE_CONTEXT_RESET_FLAG:
    {
      //OS_PRINTF("\n##debug: av render trick mode reset!!\n");
      //p_sink_pin->flush_async_render(p_sink_pin);
      //dmx_av_reset(p_priv->p_dmx_dev);
    }
    break;
    
  default:
    break;
  }

#if (DUMP_AV_TS_FILE == 1)
    if (test_file && p_sample->payload)
    {
      vfs_write(p_sample->p_data, p_sample->payload, 1, test_file);
    }
#endif
  p_sink_pin->async_render(p_sink_pin, p_sample, 20);
}

static BOOL dmx_idle(handle_t _this, media_sample_t *p_sample, u32 context)
{
  av_render_pin_private_t *p_priv = get_priv(_this);
  av_render_filter_t *p_filter = (av_render_filter_t *)((ipin_t *)_this) ->get_filter(_this);
  BOOL b_state = FALSE;
  vdec_info_t v_info = {0};
  u32 aud_payload = 0;
  u32 aud_es_left = 0;
  u32 vid_payload = 0;
  u32 vid_es_left = 0;
  BOOL es_buf_small_flag = FALSE;
#ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
#else
  chip_ic_t chip_ic_id = IC_WARRIORS;
#endif
#ifdef AV_RANDER_DBG_PRINT
  u32 cur_tick = 0;
#endif
  while(!p_priv->is_clear_stream)
  {
    b_state = dmx_get_dma_state(p_priv->p_dmx_dev);
    if(b_state == TRUE)
      break;
    mtos_task_sleep(10);
  }  
  // first check
  if(!p_priv->b_video_show || p_priv->b_video_freeze)
  {
    vdec_get_info(p_priv->p_video_dev,&v_info);
    if (v_info.is_stable)
    {
      if (!p_priv->b_video_show)
      {
        p_priv->b_video_show = 1;
        disp_layer_show(p_priv->p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
        if (chip_ic_id != IC_ENSEMBLE)
        {
          disp_layer_show(p_priv->p_disp_dev, DISP_LAYER_ID_VIDEO_HD, TRUE);
        }
      }
      else
      {
        p_priv->b_video_freeze = 0;
        vdec_resume(p_priv->p_video_dev);
        OS_PRINTF("\n##vdec_resume!!!333");
      }
    }
  }
#ifdef AV_RANDER_DBG_PRINT
  cur_tick = mtos_ticks_get();
#endif
  if(p_priv->is_clear_stream)
  {
    b_state = dmx_get_dma_state(p_priv->p_dmx_dev);
    if (!b_state)
    {
      return FALSE;
    }
  }
  aud_payload = p_filter->av_render_check_audio(p_filter, p_sample->p_data, p_sample->payload);
  vid_payload = p_sample->payload - aud_payload;
  
  vdec_get_es_buf_space(p_priv->p_video_dev, &vid_es_left);
  vid_es_left *= KBYTES;
  
  aud_file_getleftesbuffer_vsb(p_priv->p_audio_dev, &aud_es_left);
  CHECK_FAIL_RET_FALSE(aud_es_left <= 128 * KBYTES);
  aud_es_left = (aud_es_left * 7 >> 3);  // becuase there has 8 byte apts 
#ifdef AV_RANDER_DBG_PRINT
  //vdec_get_info(p_priv->p_video_dev,&v_info);
#endif
  if (vid_es_left < vid_payload)
  {
#ifdef AV_RANDER_DBG_PRINT
    OS_PRINTF("\n##av_pin:v[%x][%x][%x][%x][%x]\n",
      p_priv->av_abnormal, cur_tick, vid_es_left,vid_payload,v_info.avsync_info);
#endif
    es_buf_small_flag = TRUE;
  }

  if (aud_es_left < aud_payload)
  {
#ifdef AV_RANDER_DBG_PRINT
    OS_PRINTF("\n##av_pin:a[%x][%x][%x][%x][%x]\n",
      p_priv->av_abnormal, cur_tick, aud_es_left,aud_payload,v_info.avsync_info);
#endif
    es_buf_small_flag = TRUE;
  }
  #ifndef WIN32
  if (es_buf_small_flag)
  {
    mtos_task_sleep(20); // sleep 20 ms
    p_priv->av_abnormal++;
    if (p_priv->av_abnormal > 70)  // so, this is at lest more than 2 seconds
    {
      OS_PRINTF("\n##av_pin: reset...\n");
      p_filter->av_render_reset(p_filter);
    }
    
    return FALSE;
  }
  else
  {
    p_priv->av_abnormal = 0;
  }
  #endif
  return TRUE;
}

static BOOL render_sample_to_dmx(handle_t _this, media_sample_t *p_sample, u32 context)
{
  av_render_pin_private_t *p_priv = get_priv(_this);
  dmx_dma_config_t dma_cfg = {0};

  if (p_priv->p_resv_data
    && ((u8 *)((u32)p_sample->p_data + p_sample->payload - 188) == p_priv->p_resv_data)
    && (p_priv->p_resv_data[1] == 0x1F && p_priv->p_resv_data[2] == 0xFF))
  {
    p_priv->p_resv_data[1] = p_priv->resv_data[0];
    p_priv->p_resv_data[2] = p_priv->resv_data[1];
    p_priv->p_resv_data = NULL;
  }
  else
  {
    p_priv->p_resv_data = NULL;
  }
  
  if (p_sample->payload)
  {
    if (((u32)p_sample->p_data) % 8)
    {
      p_sample->p_data = (u8 *)((u32)p_sample->p_data - 188);
      p_priv->p_resv_data = p_sample->p_data;
      p_priv->resv_data[0] = p_sample->p_data[1];
      p_priv->resv_data[1] = p_sample->p_data[2];
      p_sample->p_data[1] = 0x1F;
      p_sample->p_data[2] = 0xFF;
      p_sample->payload += 188;
    }
    dma_cfg.data_length = p_sample->payload;
    dma_cfg.mem_address = (u32)p_sample->p_data;
#if (DUMP_AV_TS_FILE == 2)
    if (test_file)
    {
      vfs_write(p_sample->p_data, p_sample->payload, 1, test_file);
    }
#endif
#ifdef AV_RANDER_DBG_PRINT
    OS_PRINTF("\n##dmx_set_dma_config");
#endif
    dma_cfg.ts_clk = 40000000;
    dmx_set_dma_config(p_priv->p_dmx_dev, &dma_cfg);
#ifdef AV_RANDER_DBG_PRINT
    OS_PRINTF("##dma_cfg[%x, %lu]##!!\n\n", dma_cfg.mem_address, dma_cfg.data_length);
#endif
  }
else
{
#ifdef AV_RANDER_DBG_PRINT
  OS_PRINTF("\n##render_sample_to_dmx[%x, %lu]\n\n", p_sample->p_data, p_sample->payload);
#endif
}
  return TRUE;
}

static BOOL notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

av_render_pin_t *av_render_pin_create(av_render_pin_t *p_pin, interface_t *p_owner,void *p_para)
{
  av_render_pin_private_t *p_priv = NULL;
  av_render_pin_t *p_ins = p_pin;
  //interface_t *p_interface = NULL;
  sink_pin_t *p_sink_pin = NULL;
  base_input_pin_t *p_input_pin = NULL;
  sink_pin_para_t sink_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t media_format = {MT_TS_PKT};
  mul_pvr_play_attr_t *play_attr;

  //check input parameter
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);

  play_attr = (mul_pvr_play_attr_t *)p_para;
 
  //create base class
  sink_pin_para.p_filter = p_owner;
  sink_pin_para.p_name = "av_render_pin";
  sink_pin_create(&p_ins->base_pin, &sink_pin_para);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(av_render_pin_private_t));
  p_priv->p_this = p_ins; //this point
  p_priv->is_clear_stream = play_attr->is_clear_stream;

  //overload virtual function
  p_sink_pin = (sink_pin_t *)p_pin;
  p_sink_pin->hw_ready = dmx_idle;
  p_sink_pin->render_sample = render_sample_to_dmx;
  p_input_pin = (base_input_pin_t *)p_ins;
  p_input_pin->on_receive = on_receive;
  p_input_pin->notify_allocator = notify_allocator;
  
  p_ipin = (ipin_t *)p_ins;
  p_ipin->on_open = on_open;
  p_ipin->on_close = on_close;
  p_ipin->add_supported_media_format(p_ipin, &media_format);

  return p_ins;
}

//end of file

