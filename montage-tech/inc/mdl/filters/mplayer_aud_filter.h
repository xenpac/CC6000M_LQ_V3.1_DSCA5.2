/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MPLAYER_AUD_FILTER_H_
#define __MPLAYER_AUD_FILTER_H_

typedef struct tag_mplayer_aud_info
{
  /*!
    file size
    */
  u32 file_size;
  /*!
    duration time
    */
  u32 duration;
  /*!
    picture buf
    */
  void *p_pic_buf;
  /*!
    pic buf len
    */
  u32 pic_buf_len;
  /*!
    p_lrc data
    */
  u8 *p_lrc_data;
  /*!
    lrc data length
    */
  u32 lrc_data_len;
}mplayer_aud_info_t;

/*!
 File player attr
 */
typedef struct tag_mplayer_aud_filter_para
{
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
  /*!
    callback
    */
  file_seq_event_cb_fun_t cb;
}mplayer_aud_filter_para_t;

/*!
  mplayer aud filter command define
  */
typedef enum tag_mplayer_aud_filter_cmd
{
  /*!
    config name
    */
  MPLAYER_AUD_CFG_NAME,
  /*!
    config ...
    */
  MPLAYER_AUD_CFG_LRC,
  /*!
    config ...
    */
  MPLAYER_AUD_CFG_LOGO,
  /*!
    push mp3 logo again
  */
  MPLAYER_AUD_PUSH_LOGO,
  /*!
    is or not push mp3 logo
  */  
  MPLAYER_AUD_SHOW_LOGO,
  /*!
    is or not push mp3 lrc
  */   
  MPLAYER_AUD_SHOW_LRC,
 /*!
   config srouce url
  */
  MPLAYER_AUD_CFG_URL,
 /*!
   config head info
  */
  MPLAYER_AUD_CFG_HEAD_INFO,
  /*!
    time seek
    */
  MPLAYER_AUD_TIME_SEEK,
  /*!
    end
    */
  MPLAYER_AUD_CFG_END,
}mplayer_aud_filter_cmd_t;

/*!
  Audio render filter event define
  */
typedef enum tag_mplayer_aud_filter_evt
{
  /*!
    config read file' name
    */
  MPLAYER_AUD_PLAY_START = MPLAYER_AUD_FILTER << 16,
  /*!
    config read file' name
    */
  MPLAYER_AUD_FILE_PLAY_END,
  /*!
    get current play time
    */
  MPLAYER_AUD_GET_PLAY_TIME,
  /*!
    audio data error
    */
  MPLAYER_AUD_GET_DATA_ERROR,
  /*!
   Play end event
   */
  MPLAYER_AUD_PLAY_EVENT_EOS,
  /*!
   Update play time ms
   */
  MPLAYER_AUD_PLAY_EVENT_UP_TIME,    
  /*!
   Update video resolution param = mul_fp_pixel_t
   */
  MPLAYER_AUD_PLAY_EVENT_UP_RESOLUTION, 
  /*!
   Unsupported video event, audio is ok
   */
  MPLAYER_AUD_PLAY_EVENT_UNSUPPORTED_VIDEO, 
  /*!
   Unsupported audio event, video is ok
   */
  MPLAYER_AUD_PLAY_EVENT_UNSUPPORTED_AUDIO,
  /*!
   Trick play to begining of video
   */
  MPLAYER_AUD_PLAY_EVENT_TRICK_TO_BEGIN, 
  /*!
   Unsupport because of not enough memory
   */
  MPLAYER_AUD_PLAY_EVENT_UNSUPPORT_MEMORY, 
  /*!
   Receive new sub data
   */
  MPLAYER_AUD_PLAY_EVENT_NEW_SUB_DATA_RECEIVE, 
  /*!
   Event unsupport seek
   */    
  MPLAYER_AUD_PLAY_EVENT_UNSUPPORT_SEEK,
  /*!
   Load media user exit
   */
  MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_EXIT, 
  /*!
   Load media error
   */
  MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_ERROR, 
  /*!
   Load media success
   */
  MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_SUCCESS, 
   /*!
   File es task exit
   */
  MPLAYER_AUD_PLAY_EVENT_FILE_ES_TASK_EXIT, 
  /*!
   Start downloading
   */
  MPLAYER_AUD_PLAY_EVENT_START_BUFFERING,
  /*!
   Finish downloading
   */
  MPLAYER_AUD_PLAY_EVENT_FINISH_BUFFERING,
  /*!
   Update network speed, Byte/s
   */
  MPLAYER_AUD_PLAY_EVENT_UPDATE_BPS,
  /*!
   Network speed is low, request to change url src
   */
  MPLAYER_AUD_PLAY_EVENT_REQUEST_CHANGE_SRC,
  /*!
   File  set path fail 
   */
  MPLAYER_AUD_PLAY_EVENT_SET_PATH_FAIL,
   /*!
   Update cost time while loading media
   */
  MPLAYER_AUD_PLAY_EVENT_LOAD_MEDIA_TIME,
  /*!
   Unsupported HD video event, audio is ok
   */
  MPLAYER_AUD_PLAY_EVENT_UNSUPPORTED_HD_VIDEO,
  /*!
    TV sys has been changed
   */
  MPLAYER_AUD_PLAY_EVENT_SWITCH_TV_SYS,
  /*!
   Event max
   */    
  MPLAYER_AUD_PLAY_EVENT_MAX,
}mplayer_aud_filter_evt_t;


/*!
  create a file source instance
  filter ID:MPLAYER_AUD_FILTER
  \return return the instance of mp3_trans_filter_t
  */
ifilter_t * mplayer_aud_filter_create(void *p_para);

#endif // End for __MPLAYER_AUD_FILTER_H_

