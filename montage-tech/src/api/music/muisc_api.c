/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include  "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_printk.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"
#include "lib_rect.h"
#include "lib_unicode.h"
#include "common.h"

#include "aud_vsb.h"
#include "drv_dev.h"
#include "mdl.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "file_playback_sequence.h"
#include "data_manager.h"
//eva
#include "media_format_define.h"
#include "interface.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"

#include "lib_memp.h"
#include "pdec.h"

//filter
#include "common_filter.h"
#include "eva_filter_factory.h"
#include "file_source_filter.h"
#include "mp3_player_filter.h"
#include "Pic_filter.h"
#include "pic_render_filter.h"
#include "mp3_decode_frame.h"
#include "str_filter.h"
#include "lrc_filter.h"

//ap
#include "ap_framework.h"

#include "music_api.h"
#include "music_api_priv.h"
#include "vfs.h"
#include "err_check_def.h"


RET_CODE music_regist_callback(u32 handle, music_evt_callback p_callback)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_regist_callback != NULL);

   return p_handle->music_regist_callback(p_handle->p_data, p_callback);
}

RET_CODE music_unregist_callback(u32 handle)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_unregist_callback != NULL);

  return p_handle->music_unregist_callback(p_handle->p_data);
}

RET_CODE music_create_chain(u32 *p_handle, const music_chain_t *p_music_chain)
{
  music_handle_t *p_m_handle = NULL;

  p_m_handle = (music_handle_t *)class_get_handle_by_id(MUSIC_API_CLASS_ID);
  
  CHECK_FAIL_RET_CODE(p_m_handle != NULL);
  CHECK_FAIL_RET_CODE(p_m_handle->music_create_chain != NULL);
  *p_handle = (u32)p_m_handle;
  return p_m_handle->music_create_chain(p_m_handle->p_data, p_music_chain);
}

RET_CODE music_destroy_chain(u32 handle)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_destroy_chain != NULL);

  return p_handle->music_destroy_chain(p_handle->p_data);
}

RET_CODE music_init(void)
{
   return 0;
}

RET_CODE music_deinit(void)
{
   return 0;
}

RET_CODE music_start(u32 handle)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_start != NULL);

   return p_handle->music_start(p_handle->p_data);
}

RET_CODE music_stop(u32 handle)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_stop != NULL);

   return p_handle->music_stop(p_handle->p_data);
}

RET_CODE music_pause(u32 handle)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_pause != NULL);

   return p_handle->music_pause(p_handle->p_data);
}

RET_CODE music_resume(u32 handle)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_resume != NULL);

   return p_handle->music_resume(p_handle->p_data);
}

RET_CODE music_seek_time(u32 handle, u32 time)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_seek_time != NULL);

   return p_handle->music_seek_time(p_handle->p_data, time);
}

RET_CODE music_lrc_show(u32 handle, BOOL ishow)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_lrc_show != NULL);

  return p_handle->music_lrc_show(p_handle->p_data, ishow);
}

RET_CODE music_lrc_set_info(u32 handle, music_lrc_info_t *p_lrc_info)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_lrc_set_info != NULL);

  return p_handle->music_lrc_set_info(p_handle->p_data, p_lrc_info);
}

music_lrc_info_t *music_lrc_get_info(u32 handle)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_NULL(p_handle != NULL);
  CHECK_FAIL_RET_NULL(p_handle->music_lrc_get_info != NULL);

  return p_handle->music_lrc_get_info(p_handle->p_data);
}

RET_CODE music_logo_show(u32 handle, BOOL ishow, music_logo_info_t *p_logo_info)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_logo_show != NULL);

  return p_handle->music_logo_show(p_handle->p_data, ishow, p_logo_info);
}

RET_CODE music_logo_set_info(u32 handle, music_logo_info_t *p_logo_info)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_logo_set_info != NULL);

  return p_handle->music_logo_set_info(p_handle->p_data, p_logo_info);
}

music_logo_info_t *music_logo_get_info(u32 handle)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_NULL(p_handle != NULL);
  CHECK_FAIL_RET_NULL(p_handle->music_logo_get_info != NULL);

  return p_handle->music_logo_get_info(p_handle->p_data);
}

RET_CODE music_set_filename(u32 handle, u16 *filename)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_set_filename != NULL);

  return p_handle->music_set_filename(p_handle->p_data, filename);
}

RET_CODE music_set_buffer(u32 handle, void *p_buffer, u32 buf_size)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->music_set_buffer != NULL);

  return p_handle->music_set_buffer(p_handle->p_data, p_buffer, buf_size);
}

RET_CODE mul_music_logo_clear(u32 handle, u32 color)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->mul_music_logo_clear != NULL);

  return p_handle->mul_music_logo_clear(p_handle->p_data, color);
}

RET_CODE mul_music_set_charset(u32 handle, str_fmt_t charset)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->mul_music_set_charset != NULL);

  return p_handle->mul_music_set_charset(p_handle->p_data, charset);
}

RET_CODE mul_music_set_url(u32 handle, u8 *p_url)
{
  music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_CODE(p_handle != NULL);
  CHECK_FAIL_RET_CODE(p_handle->mul_music_set_url != NULL);

  return p_handle->mul_music_set_url(p_handle->p_data, p_url);
}

RET_CODE mul_music_create_net_chain(u32 *p_handle, const music_chain_t *p_music_chain)
{
  music_handle_t *p_m_handle = NULL;

  p_m_handle = (music_handle_t *)class_get_handle_by_id(MUSIC_API_CLASS_ID);

  CHECK_FAIL_RET_CODE(p_m_handle != NULL);
  CHECK_FAIL_RET_CODE(p_m_handle->mul_music_create_net_chain != NULL);
  *p_handle = (u32)p_m_handle;
  return p_m_handle->mul_music_create_net_chain(p_m_handle->p_data, p_music_chain);
}


u32 mul_music_get_time_long(u32 handle)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_ZERO(p_handle != NULL);
  CHECK_FAIL_RET_ZERO(p_handle->mul_music_get_time_long != NULL);

   return p_handle->mul_music_get_time_long(p_handle->p_data);
}

BOOL mul_music_is_mp3_file(u32 handle)
{
   music_handle_t *p_handle = (music_handle_t *)handle;

  CHECK_FAIL_RET_FALSE(p_handle != NULL);
  CHECK_FAIL_RET_FALSE(p_handle->mul_music_is_mp3_file != NULL);

   return p_handle->mul_music_is_mp3_file(p_handle->p_data);
}

