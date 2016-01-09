/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MUSIC_API_PRIV_H_
#define __MUSIC_API_PRIV_H_

/*!
  music source get from
  */
typedef enum
{
  /*!
    get from file.
    */
  SOURCE_FILE,
  /*!
    get from net.
    */
  SOURCE_NET,

}source_get_t;

/*!
  music handle
  */
typedef struct tag_music_handle
{
  /*!
    Database private data
    */
  void *p_data;
  /*!
    music register callback
   */
  RET_CODE (*music_regist_callback)(void *p_handle, music_evt_callback p_callback);

  /*!
    music unregister callback
   */
  RET_CODE (*music_unregist_callback)(void *p_handle);

  /*!
    music create chain
   */
  RET_CODE (*music_create_chain)(void *p_handle, const music_chain_t *p_chain);
  /*!
    music destroy chain
   */
  RET_CODE (*music_destroy_chain)(void *p_handle);
  /*!
    music start
   */
  RET_CODE (*music_start)(void *p_handle);

  /*!
    music stop
   */
  RET_CODE (*music_stop)(void *p_handle);

  /*!
    music pause
   */
  RET_CODE (*music_pause)(void *p_handle);

  /*!
    music resume
   */
  RET_CODE (*music_resume)(void *p_handle);

  /*!
    music seek_time
   */
  RET_CODE (*music_seek_time)(void *p_handle, u32 time);

  /*!
    music set filename
   */
  RET_CODE (*music_set_filename)(void *p_handle, u16 *filename);

  /*!
    music lrc show or not
   */
  RET_CODE (*music_lrc_show)(void *p_handle, BOOL ishow);

  /*!
    music set lrc info
   */
  RET_CODE (*music_lrc_set_info)(void *p_handle, music_lrc_info_t *p_lrc_info);

  /*!
    music get lrc info
   */
  music_lrc_info_t * (*music_lrc_get_info)(void *p_handle);

  /*!
    music logo show or not
   */
  RET_CODE (*music_logo_show)(void *p_handle, BOOL ishow, music_logo_info_t *p_logo_info);

  /*!
    music set logo info
   */
  RET_CODE (*music_logo_set_info)(void *p_handle, music_logo_info_t *p_logo_info);

  /*!
    music get logo info
   */
  music_logo_info_t * (*music_logo_get_info)(void *p_handle);

  /*!
    music set buffer
   */
  RET_CODE (*music_set_buffer)(void *p_handle, void *p_buffer, u32 buf_size);

  /*!
    clear music logo with color.
    */
  RET_CODE (*mul_music_logo_clear)(void *p_handle, u32 color);

  /*!
    music set lrc charset
    */
  RET_CODE (*mul_music_set_charset)(void *p_handle, str_fmt_t charset);

  /*!
    music set net url.
    */
  RET_CODE (*mul_music_set_url)(void *p_handle, u8 *p_url);

  /*!
    music create net chain.
    */
  RET_CODE (*mul_music_create_net_chain)(void *p_handle, const music_chain_t *p_chain);

  /*!
    music get time long.
    */
  u32 (*mul_music_get_time_long)(void *p_handle);

  /*!
    music adjuge file is mp3 file.
    */
  BOOL (*mul_music_is_mp3_file)(void *p_handle);
}music_handle_t;

#endif
