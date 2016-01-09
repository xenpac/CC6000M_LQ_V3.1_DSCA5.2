/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MUSIC_PLAY_API_H_
#define __MUSIC_PLAY_API_H_

/*!
  mul_err_music_module
  */
#define MUL_ERR_MUSIC_MODULE (0x8050)

/*!
  music module not initialed.
  */
#define MUL_ERR_MUSIC_NOT_INIT             ((MUL_ERR_MUSIC_MODULE << 16) + 0001)

/*!
  music module no memory
  */
#define MUL_ERR_MUSIC_NO_MEM               ((MUL_ERR_MUSIC_MODULE << 16) + 0002)

/*!
  music module error parameter
  */
#define MUL_ERR_MUSIC_ERR_PARAM            ((MUL_ERR_MUSIC_MODULE << 16) + 0003)

/*!
  music module add filter failed.
  */
#define MUL_ERR_MUSIC_ADD_FILTER_FAILED    ((MUL_ERR_MUSIC_MODULE << 16) + 0004)

/*!
  music module connect filter failed.
  */
#define MUL_ERR_MUSIC_FILTER_CONNECT_FAILED    ((MUL_ERR_MUSIC_MODULE << 16) + 0005)



/*!
  Application music player evt enum
  */
typedef enum
{
  /*!
    file play start
    */
  MUSIC_API_EVT_PLAY_START,
  /*!
    file play end
    */
  MUSIC_API_EVT_PLAY_END,
  /*!
    can not play the file
    */
  MUSIC_API_EVT_CANNOT_PLAY,
  /*!
    music player get play time.
    */
  MUSIC_API_EVT_GET_PLAY_TIME,    
  /*!
    music stopped
    */
  MUSIC_API_EVT_STOPPED,
  /*!
    music data error
    */
  MUSIC_API_EVT_DATA_ERROR,
  
}music_api_evt_t;


/*!
  parameter for create music chain
  */
typedef struct
{
  /*!
    stack size for chain task
   */
  u32 task_stk_size; 
  /*!
    rsc_get_glyph
    */  
  RET_CODE (*rsc_get_glyph)(glyph_input_t *p_input, glyph_output_t *p_output);
  /*!
   * file play seq task priority
   */
  u32 fs_task_prio;
  /*!
   * file play seq task size
   */  
  u32 fs_task_size;
  /*!
    *file play seq task addr
    */
  u32 fs_stack_addr;
  /*!
   * file play module mem start
   */
  u32 v_mem_start;
  /*!
   * file play module mem size
   */
  u32 v_mem_size; 
  /*!
   * file play video mem size
   */
  u32 vdec_start;
  /*!
   * file play video mem size
   */
  u32 vdec_size;
  /*!
   * file play audio mem size
   */
  u32 audio_start;
  /*!
   * file play audio mem size
   */
  u32 audio_size;  
  /*!
   * file play audio output
   */
  u8 audio_output;
  /*!
   * file play vdec policy
   */    
  u32 vdec_policy;
  /*!
   * file play preload size
   */
  u32 stack_preload_size;
  /*!
   * file play preload priority
   */
  u32 task_preload_priority;
  /*!
   * preload_audio_buf
   */
  u8 *p_preload_audio_buf;
  /*!
   * file play audio output
   */
  u32 preload_audio_buffer_size;
  /*!
   * preload_audio_buf
   */
  u8 *p_preload_video_buf;
  /*!
   * file play audio output
   */
  u32 preload_video_buffer_size;
  /*!
   * file play net task priority
   */
  u32 net_task_priority; 
  /*!
   * direct url play chain creat
   */
  BOOL  direct_url_chain;
}music_chain_t;

/*!
  music_lrc_info_t
  */
typedef struct
{
  /*!
    music lrc show count
   */
  u8 line_cunt;
  /*!
    music lrc show rect
   */  
  rect_t lrc_rect;
  /*!
    music lrc show region
   */  
  void *p_lrc_rgn;
  /*!
   music lrc char_filter config 
    */
  str_cfg_t str_cfg;  
  
}music_lrc_info_t;

/*!
  music_logo_info_t
  */
typedef struct
{
  /*!
    music logo show rect
   */
  rect_t logo_rect;
  /*!
    music logo show region
   */  
  void   *p_logo_rgn;
  /*!
    music logo dec buffer
   */  
  void   *p_buffer;
  /*!
    music logo dec buffer size
   */  
  u32    buffer_size;
}music_logo_info_t;


/*!
  music callback
 */
typedef void(*music_evt_callback)(u32 pic_handle, u32 content, u32 para1, u32 para2);

/*!
  music register callback
 */
RET_CODE music_regist_callback(u32 p_handle, music_evt_callback p_callback);

/*!
  music unregister callback
 */
RET_CODE music_unregist_callback(u32 p_handle);

/*!
  music create chain
 */
RET_CODE music_create_chain(u32 *p_handle, const music_chain_t *p_chain);
/*!
  music destroy chain
 */
RET_CODE music_destroy_chain(u32 p_handle);

/*!
  music api init
 */
RET_CODE music_init(void);
/*!
  music api deinit
 */
RET_CODE music_deinit(void);

/*!
  music start
 */
RET_CODE music_start(u32 p_handle);

/*!
  music stop
 */
RET_CODE music_stop(u32 p_handle);

/*!
  music pause
 */
RET_CODE music_pause(u32 p_handle);

/*!
  music resume
 */
RET_CODE music_resume(u32 p_handle);

/*!
  music seek_time
 */
RET_CODE music_seek_time(u32 p_handle, u32 time);

/*!
  music set filename
 */
RET_CODE music_set_filename(u32 p_handle, u16 *filename);

/*!
  music lrc show or not
 */
RET_CODE music_lrc_show(u32 p_handle, BOOL ishow);

/*!
  music set lrc info
 */
RET_CODE music_lrc_set_info(u32 p_handle, music_lrc_info_t *p_lrc_info);

/*!
  music get lrc info
 */
music_lrc_info_t *music_lrc_get_info(u32 p_handle);

/*!
  music logo show or not
 */
RET_CODE music_logo_show(u32 p_handle, BOOL ishow, music_logo_info_t *p_logo_info);

/*!
  music set logo info
 */
RET_CODE music_logo_set_info(u32 p_handle, music_logo_info_t *p_logo_info);

/*!
  music get logo info
 */
music_logo_info_t *music_logo_get_info(u32 p_handle);

/*!
  music set buffer
 */
RET_CODE music_set_buffer(u32 p_handle, void *p_buffer, u32 buf_size);

/*!
  clear music logo with color.
  */
RET_CODE mul_music_logo_clear(u32 p_handle, u32 color);

/*!
  music set lrc charset
  */
RET_CODE mul_music_set_charset(u32 p_handle, str_fmt_t charset);

/*!
  music set net url.
  */
RET_CODE mul_music_set_url(u32 p_handle, u8 *p_url);

/*!
  music create net chain.
  */
RET_CODE mul_music_create_net_chain(u32 *p_handle, const music_chain_t *p_chain);

/*!
  music get time long.
  */
u32 mul_music_get_time_long(u32 p_handle);

/*!
  music adjuge file is mp3 file.
  */
BOOL mul_music_is_mp3_file(u32 p_handle);

#endif
