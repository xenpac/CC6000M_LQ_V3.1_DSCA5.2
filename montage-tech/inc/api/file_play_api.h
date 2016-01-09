/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_PLAY_API_H_
#define __FILE_PLAY_API_H_

/*!
 File play Handle
 */
typedef u32 fp_handle_t;

/*!
 File play subtitle Handle
 */
typedef u32 fp_subt_handle_t;

/*!
 Param for start opt
 */
typedef struct
{
    /*!
     File name which will be played
     */
    char *p_file_name;
    /*!
     Start from this sec
     */
    int   seek_sec;
}mul_fp_start_opt_t;
/*!
 Param for these status, using in feature
 */
typedef u32 mul_fp_stop_opt_t;
/*!
 Param for these status, using in feature
 */
typedef u32 mul_fp_pause_opt_t;
/*!
 Param for these status, using in feature
 */
typedef u32 mul_fp_resume_opt_t;
/*!
 Param for these status, using in feature
 */
typedef u32 mul_fp_force_stop_opt_t;

/*!
 ID for file player attr
 */
typedef enum
{
    /*!
     Addr ID
     */
    MUL_ATTR_ID_NONE = 0,
}mul_fp_attr_id_t;
/*!
  subtitle max len
  */
#define MUL_FP_SUBTITLE_LEN 20
/*!
  subtitle max cnt
  */
#define MUL_FP_SUBTITLE_CNT 20

/*!
 subtitle struct
 */
typedef struct{
    /*!
     subtitle lang string
     */
    char lang[MUL_FP_SUBTITLE_LEN];
    /*!
     subtitle title string
     */    
    char title[MUL_FP_SUBTITLE_LEN];
    /*!
     subtitle code string
     */    
    char code[MUL_FP_SUBTITLE_LEN];
    /*!
     subtitle id
     */    
    int    id;
 }mul_fp_subtitle_t;
/*!
 subtitle struct
 */
typedef struct{
    /*!
     subtitle total count
     */
    int cnt;
    /*!
     subtitle struct array
     */    
    mul_fp_subtitle_t subtitle[MUL_FP_SUBTITLE_CNT];
}mul_fp_video_subtitle_t;
 
/*!
 ID for file player attr
 */
typedef enum
{
    /*!
     Normal play
     */
    MUL_PLAY_NORMAL_PLAY   = 0,
    /*!
     Forword 2X
     */
    MUL_PLAY_FORWORD_2X   = 2,
    /*!
     Forword 4X
     */
    MUL_PLAY_FORWORD_4X   = 4,
    /*!
     Forword 8X
     */
    MUL_PLAY_FORWORD_8X   = 8,
    /*!
     Forword 16X
     */
    MUL_PLAY_FORWORD_16X  = 16,
    /*!
     Forword 32X
     */
    MUL_PLAY_FORWORD_32X  = 32,    
    /*!
     Backword 2X
     */
    MUL_PLAY_BACKWORD_2X  = -2,
    /*!
     Backword 4X
     */
    MUL_PLAY_BACKWORD_4X  = -4,
    /*!
     Backword 8X
     */
    MUL_PLAY_BACKWORD_8X  = -8,
    /*!
     Backword 16X
     */
    MUL_PLAY_BACKWORD_16X = -16,
    /*!
     Backword 32X
     */
    MUL_PLAY_BACKWORD_32X = -32,
}mul_fp_play_speed_t;

/*!
 Type of unsupport video
 */
typedef enum
{
    /*!
     Video unkown type
     */
    MUL_PLAY_V_UNKNOWN    = -1,
    /*!
     Video Mpeg1 type
     */
    MUL_PLAY_V_MPEG1  = 0,
    /*!
     Video Mpeg2 type
     */
    MUL_PLAY_V_MPEG2,
    /*!
     Video Mpeg4 type
     */
    MUL_PLAY_V_MPEG4,
    /*!
     Video h264 type
     */
    MUL_PLAY_V_H264,
    /*!
     Video AVC type
     */  
    MUL_PLAY_V_AVC,
    /*!
     Video DIRAC type
     */   
    MUL_PLAY_V_DIRAC,
    /*!
     Video VC1 type
     */ 
    MUL_PLAY_V_VC1,
}mul_play_v_type_t;

/*!
 Type of unsupport audio
 */
typedef enum
{
    /*!
     Video UNKNOWN type
     */   
    MUL_PLAY_A_UNKNOWN    = -1,
    /*!
     Video MP2 type
     */  
    MUL_PLAY_A_MP2 = 0,
    /*!
     Video AC3 type
     */ 
    MUL_PLAY_A_AC3,
    /*!
     Video DTS type
     */ 
    MUL_PLAY_A_DTS,
    /*!
     Video LPCM type
     */   
    MUL_PLAY_A_LPCM_BE,
    /*!
     Video AAC type
     */ 
    MUL_PLAY_A_AAC,
    /*!
     Video AAC LATM type
     */   
    MUL_PLAY_A_AAC_LATM,
    /*!
     Video TRUEHD type
     */  
    MUL_PLAY_A_TRUEHD,
    /*!
     Video S302M type
     */ 
    MUL_PLAY_A_S302M,
    /*!
     Video PCM type
     */    
    MUL_PLAY_A_PCM_BR,
} mul_play_a_type_t;

/*!
 Event ID
 */
typedef enum
{
    /*!
     Play end event
     */
    MUL_PLAY_EVENT_EOS = 0,
    /*!
     Update play time ms
     */
    MUL_PLAY_EVENT_UP_TIME,    
    /*!
     Update video resolution param = mul_fp_pixel_t
     */
    MUL_PLAY_EVENT_UP_RESOLUTION, 
    /*!
     Unsupported video event, audio is ok
     */
    MUL_PLAY_EVENT_UNSUPPORTED_VIDEO, 
    /*!
     Unsupported audio event, video is ok
     */
    MUL_PLAY_EVENT_UNSUPPORTED_AUDIO,
    /*!
     Trick play to begining of video
     */
    MUL_PLAY_EVENT_TRICK_TO_BEGIN, 
    /*!
     Unsupport because of not enough memory
     */
    MUL_PLAY_EVENT_UNSUPPORT_MEMORY, 
    /*!
     Receive new sub data
     */
    MUL_PLAY_EVENT_NEW_SUB_DATA_RECEIVE, 
    /*!
     Event unsupport seek
     */    
    MUL_PLAY_EVENT_UNSUPPORT_SEEK,
    /*!
     Load media user exit
     */
    MUL_PLAY_EVENT_LOAD_MEDIA_EXIT, 
    /*!
     Load media error
     */
    MUL_PLAY_EVENT_LOAD_MEDIA_ERROR, 
    /*!
     Load media success
     */
    MUL_PLAY_EVENT_LOAD_MEDIA_SUCCESS, 
     /*!
     File es task exit
     */
    MUL_PLAY_EVENT_FILE_ES_TASK_EXIT, 
    /*!
     Start downloading
     */
    MUL_PLAY_EVENT_START_BUFFERING,
    /*!
     Finish downloading
     */
    MUL_PLAY_EVENT_FINISH_BUFFERING,
    /*!
     Update network speed, Byte/s
     */
    MUL_PLAY_EVENT_UPDATE_BPS,
    /*!
     Network speed is low, request to change url src
     */
    MUL_PLAY_EVENT_REQUEST_CHANGE_SRC,
    /*!
     File  set path fail 
     */
    MUL_PLAY_EVENT_SET_PATH_FAIL,
     /*!
     Update cost time while loading media
     */
    MUL_PLAY_EVENT_LOAD_MEDIA_TIME,
    /*!
     Unsupported HD video event, audio is ok
     */
    MUL_PLAY_EVENT_UNSUPPORTED_HD_VIDEO,
    /*!
      TV sys has been changed
     */
    MUL_PLAY_EVENT_SWITCH_TV_SYS,
     /*!
    * notify upper layer that player has already finish updating bps
    */
    MUL_PLAY_EVENT_FINISH_UPDATE_BPS,
    /*!
     Event unsupport trick
     */    
    MUL_PLAY_EVENT_UNSUPPORT_TRICK,
    /*!
     Event max
     */    
    MUL_PLAY_EVENT_MAX,
}mul_fp_play_event_t;

/*!
 State of play
 */
typedef enum
{
    /*!
     State None
     */
    MUL_PLAY_STATE_NONE = 0,   
    /*!
     State of load media
     */    
    MUL_PLAY_STATE_LOAD_MEDIA,
    /*!
     State of play
     */    
    MUL_PLAY_STATE_PLAY,
    /*!
     State of pause
     */     
    MUL_PLAY_STATE_PAUSE,
    /*!
     State of preview
     */     
    MUL_PLAY_STATE_PREVIEW,    
    /*!
     State of stop
     */     
    MUL_PLAY_STATE_STOP,
    /*!
     State of speed play
     */     
    MUL_PLAY_STATE_SPEED_PLAY,    
}mul_fp_play_state_t;

/*!
 File player attr
 */
typedef struct
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
   xxxxxxxx
   */  
   int (*camera_open)(char *name); 
   
  /*!
  xxxxxxxx
  */  
   int (*camera_read)(int fd, char *buffer, int buffer_len); 
    
  /*!
   xxxxxxxx
   */  
  int (*camera_close)(int fd);     

  /*!
  task_idle for drv
 */  
  u32  task_idle;      
}mul_fp_attr_t;

/*!
 File player subtitle attr
 */
typedef struct
{
    /*!
     subtitle path
     */ 
    u16 *p_subt_path;

    /*!
     * Char file rsc config
     */
    BOOL insert_subt;

    /*!
     * Source pin buffer size, it used if p_subt_path set.
     */
    u32 sp_buf_size;
    /*!
     * Source pin buffer count, it used if p_subt_path set.
     */
    u8 sp_buf_cnts;

    /*!
     * Char rect
     */
    rect_t char_rect;

    /*!
     * bpp
     */
    u32 char_bpp;    

    /*!
     * region fmt
     */
    pix_fmt_t char_fmt; 

    /*!
     * Subtitle show rect
     */
    rect_t show_rect;

    /*!
     * Subtitle show region
     */
    void *p_region;
    /*!
     * font color
     */
    u32 font_color;
    /*!
     * font bg color
     */
    u32 bg_color;
    /*!
     * window bg color
     */
    u32 window_color;
    /*!
      align pos in region
      */  
    str_align_t align;
    /*!
     * task size
     */
    u32 stk_size;
    /*!
     * callback for rsc get glyph
     */
    RET_CODE (*rsc_get_glyph)(glyph_input_t *p_input, glyph_output_t *p_output);
}mul_fp_subt_attr_t;

/*!
 State info of file player
 */
typedef struct
{
    /*!
     duration of file
     */ 
    u32                 duration;
    /*!
     audio type of file
     */     
    int                 audio_type;
    /*!
     video type of file
     */    
    int                 video_type;
    /*!
     audio track of file
     */    
    int                 audio_track_num;
    /*!
     video track of file
     */        
    int                 video_track_num;
    /*!
     trick check
     */        
    BOOL                can_trick_play;
    /*!
     Display w
     */    
    int                 video_disp_w;
    /*!
     Display y
     */
    int                 video_disp_h;
    /*!
     Video fps
     */
    int                 video_fps;
    /*!
     video bps
     */
    int                 video_bps;
    /*!
     file size 
     */
    u32                 file_size;    
}mul_fp_state_info_t;

/*!
  audio track strcture
  */
typedef struct{
    /*!
    track id
    */
    int track_id;
    /*!
    language
    */
    char *p_lang;
    /*!
    title
    */
    char *p_title;
}mul_fp_audio_track_t;

/*!
 State callback function of file player
 */
typedef RET_CODE (*fn_fp_event_cb)(fp_handle_t, mul_fp_play_event_t, u32);

/*!
  File play module init
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_init(void);

/*!
  File play module deinit
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_deinit(void);

/*!
  File player create
  \param[in] p_attr: ref to mul_fp_attr_t
  \param[out] p_handle: Handle of file play
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_create(mul_fp_attr_t *p_attr, fp_handle_t *p_handle);

/*!
  File player destroy
  \param[in] handle: Handle of file play
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_destroy(fp_handle_t handle);

/*!
  Set attr to file player
  \param[in] handle: Handle of file play
  \param[in] id: ref to mul_fp_attr_id_t
  \param[in] p_val: value of some attr id, it must be consistent in vaule with attr id
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_set_attr(fp_handle_t handle, mul_fp_attr_id_t id, void *p_val);

/*!
  Get attr from file player
  \param[in] handle: Handle of file play
  \param[in] id: ref to mul_fp_attr_id_t
  \param[out] p_val: value of some attr id, it must be consistent in vaule with attr id
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_get_attr(fp_handle_t handle, mul_fp_attr_id_t id, void *p_val);

/*!
  Start file player, goto play state
  \param[in] handle: Handle of file play
  \param[in] p_start_opt: param for starting, use in feature
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_load_media(fp_handle_t handle, u8 *p_file_name);

/*!
  Load media by file url
  \param[in] handle: Handle of file play
  \param[in] pp_file_name: file path array
  \param[in] file_cnt: file patch count
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_load_mul_media(fp_handle_t handle, char **pp_file_name, u32 file_cnt);

/*!
  Start file player, goto play state
  \param[in] handle: Handle of file play
  \param[in] p_start_opt: param for starting, use in feature
  \param[in] context: param for judge current loading, use in feature
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_load_ex_media(fp_handle_t handle, u8 *p_file_name, u32 context);

/*!
  Start file player, goto play state
  \param[in] handle: Handle of file play
  \param[in] p_start_opt: param for starting, use in feature
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_start(fp_handle_t handle, u32 seek_sec);
/*!
  Preview file player, goto play state
  \param[in] handle: Handle of file play
  \param[in] p_start_opt: param for starting, use in feature
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_preview(fp_handle_t handle, mul_fp_start_opt_t *p_start_opt);
/*!
  Stop file player, goto stop state
  \param[in] handle: Handle of file play
  \param[in] p_stop_opt: param for stoping, use in feature  
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_stop(fp_handle_t handle, mul_fp_stop_opt_t *p_stop_opt);

/*!
  Force stop file player, goto stop state
  \param[in] handle: Handle of file play
  \param[in] p_stop_opt: param for stoping, use in feature  
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_force_stop(fp_handle_t handle, mul_fp_force_stop_opt_t *p_stop_opt);

/*!
  Pause file player, goto pause state
  \param[in] handle: Handle of file play
  \param[in] p_pause_opt: param for pausing, use in feature   
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_pause(fp_handle_t handle, mul_fp_pause_opt_t *p_pause_opt);

/*!
  Resume file player, goto play state
  \param[in] handle: Handle of file play
  \param[in] p_resume_opt: param for resuming, use in feature
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_resume(fp_handle_t handle, mul_fp_resume_opt_t *p_resume_opt);

/*!
  Set speed of file player, goto speed play state
  \param[in] handle: Handle of file play
  \param[in] speed: speed for playing
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_set_speed(fp_handle_t handle, mul_fp_play_speed_t speed);

/*!
  Set time position to play
  \param[in] handle: Handle of file play
  \param[in] sec: seconds position to play
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_seek(fp_handle_t handle, s32 sec);

/*!
  Get current state
  \param[in] handle: Handle of file play
  \param[in] p_status_info: Info of playing status
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_get_status_info(fp_handle_t handle, mul_fp_state_info_t *p_status_info);

/*!
  Get audio track
  \param[in] handle: Handle of file play
  \param[in] p_audio_track: Info of audio track
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_get_audio_track(fp_handle_t handle, mul_fp_audio_track_t *p_audio_track);

/*!
  Register UI callback function
  \param[in] handle: Handle of file play
  \param[in] evt_id: ref to mul_fp_play_event_t
  \param[in] fn_cb: callback function  
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_register_evt(fp_handle_t handle, 
                                    mul_fp_play_event_t evt_id, 
                                    fn_fp_event_cb fn_cb);

/*!
  Unregister UI callback function
  \param[in] handle: Handle of file play
  \param[in] evt_id: ref to mul_fp_play_event_t
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_unregister_evt(fp_handle_t handle, mul_fp_play_event_t evt_id);
/*!
  Change audio track
  \param[in] handle: Handle of file play
  \param[in] track_id: audio track id
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_change_audio_track(fp_handle_t handle, int track_id);
/*!
  Change video track
  \param[in] handle: Handle of file play
  \param[in] track_id: video track id
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_change_video_track(fp_handle_t handle, int track_id);

/*!
  subtitle init
  \param[in] p_subt_attr: subtitle attr
  \param[out] p_subt_handle: subtitle handle
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_subt_init(mul_fp_subt_attr_t *p_subt_attr, fp_subt_handle_t *p_subt_handle);

/*!
  subtitle deinit
  \param[in] subt_handle: subtitle handle
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_subt_deinit(fp_subt_handle_t subt_handle);

/*!
  Advance or delay some ms
  \param[in] subt_handle: subtitle handle
  \param[in] ms: value need to adjust, -1000, -2000 means advance 1, 2s, 1000,2000 delay 1, 2s
  \return SUCCESS if no error    
 */
RET_CODE mul_file_play_subt_adjust_pts(fp_subt_handle_t subt_handle, int ms);

/*!
  Set current state
  \param[in] handle: Handle of file play
  \param[in] state: Info of playing state
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_set_play_state(fp_handle_t handle, mul_fp_play_state_t state);
/*!
  Get current state
  \param[in] handle: Handle of file play
  \param[out] p_state: Info of playing state
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_get_play_state(fp_handle_t handle, mul_fp_play_state_t *p_state);
/*!
  Get subtitle info
  \param[in] handle: Handle of file play
  \param[out] p_subt_info: Info of subtitle
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_get_insert_subt_info(fp_subt_handle_t handle, 
                                            mul_fp_video_subtitle_t **pp_subt_info);
/*!
  Set subtitle id
  \param[in] handle: Handle of file play
  \param[in] idx: idx of subtitle
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_set_subt_by_idx(fp_subt_handle_t handle, int idx);

/*!
  Set subtitle charset
  \param[in] handle: Handle of file play
  \param[in] charset: charset of subtitle
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_set_charset(fp_handle_t handle, str_fmt_t charset);
/*!
  Set system tv mode
  \param[in] handle: Handle of file play
  \param[in] enable: enable tv mode auto change or not
  \return SUCCESS if no error
 */
RET_CODE mul_file_play_enable_tv_mode_change(fp_handle_t handle, BOOL enable);

#endif
