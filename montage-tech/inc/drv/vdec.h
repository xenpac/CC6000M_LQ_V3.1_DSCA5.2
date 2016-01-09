/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __VDEC_H__
#define __VDEC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
    check if hd input is disabled according to otp info  
  */
//#define DISABLE_HD_OTP

/*!
    Image point number
  */
#define NUMBER_POINT        10

/*!
    Data number
  */
#define NUMBER_DATA         47

/*!
    Program number
  */
#define NUMBER_PROGRAM      NUMBER_DATA

/*!
    Max Program number length
  */
#define MAX_PRO_NAME_LENGTH 16

/*!
  Initialization parameters for video decoder.
  */
typedef struct 
{
  /*! 
    If compression is used for video reconstruction
    */
  BOOL is_compression;
  /*! 
    Autotest flag
    */
  BOOL is_autotest;
}vdec_cfg_t;

/*!
  The video decoder may support several decoding modes,
  which consume different memory resource and consequently provide different video quality.
  The mode with less memory consumption provides lower video quality.
  It is helpful for memory sensitive solution.
  */
typedef enum
{
  /*!
    9dpb, close di when hd stream input
    With three full size frame buffers to ensure normal reconstruction quality
    */
  VDEC_QAULITY_AD,
  /*!
    8dpb, close di when hd stream input
    With three compressed frame buffers to save buffer consumption.
    The reconstruction video quatliy is slightly degraded.
    */
  VDEC_BUFFER_AD,
  /*!
    Only for multi-picture mode:
    */
  VDEC_MULTI_PIC_AD,
  /*!
    11dpb, open di 
    With 13 full size frame buffers to ensure normal reconstruction quality
    */
  VDEC_QAULITY_AD_128M,
  /*!
    only sd input
  */
  VDEC_SDINPUT_ONLY,
  /*!
    9dpb, open di 
    With 9 full size frame buffers to ensure normal open di
    */
  VDEC_OPENDI_64M,
    /*!
    9dpb, close di when hd stream input, close prescale
    With three full size frame buffers to ensure normal reconstruction quality
    */
  VDEC_QAULITY_AD_UNUSEPRESCALE,
  /*!
    8dpb, close di when hd stream input, close prescale
    With three compressed frame buffers to save buffer consumption.
    The reconstruction video quatliy is slightly degraded.
    */
  VDEC_BUFFER_AD_UNUSEPRESCALE,
  /*!
    11dpb, open di, close prescale 
    With 13 full size frame buffers to ensure normal reconstruction quality
    */
  VDEC_QAULITY_AD_128M_UNUSEPRESCALE,
  /*!
    only sd input
  */
  VDEC_SDINPUT_ONLY_UNUSEPRESCALE,
  /*!
    9dpb, open di, close prescale 
    With 9 full size frame buffers to ensure normal open di
    */
  VDEC_OPENDI_64M_UNUSEPRESCALE,
  /*!
    policy max
    */
  VDEC_BUFFER_POLICY_MAX
} vdec_buf_policy_t;

/*!
  This structure defines the supported formats of video source
  */
typedef enum
{
  /*!
    MPEG
    */
  VIDEO_MPEG,
  /*!
    H.264/AVC
    */
  VIDEO_H264,
  /*!
    AVS
    */
  VIDEO_AVS,
  /*!
    MPEG4
    */
  VIDEO_MPEG4,
    /*!
    MPEG ES
    */
  VIDEO_MPEG_ES,
  /*!
    H.264/AVC ES
    */
  VIDEO_H264_ES,
  /*!
    AVS ES
    */
  VIDEO_AVS_ES,
  /*!
    MPEG4 ES
    */
  VIDEO_MPEG4_ES,
    /*!
    VC1
    */
  VIDEO_VC1,
  
   /*!
    VP8
    */
  VIDEO_VP8,
  
  /*!
    RV34
    */
  VIDEO_RV3,

    /*!
    RV34
    */
  VIDEO_RV4,
	  
	/*!
	RV34
	*/
  VIDEO_H263,
  
  /*!
	Unknown video format
	*/
  VIDEO_UNKNOWN

}vdec_src_fmt_t;

/*!
  This structure defines the behavior of video display when video decoding is ready.
  */
typedef enum
{
  /*!
    The state of video layer is handled by user
    */
  VID_UNBLANK_USER,
  /*!
    The video layer will be displayed When AV is sync
    */
  VID_UNBLANK_SYNC,
  /*!
    The video layer will be displayed when video decoding is ready for display.
    */
  VID_UNBLANK_STABLE,
  /*!
    The video layer will be displayed when video get first I image.
  */
  VID_UNBLANK_FAST
}vdec_start_mode_t;


/*!
  This structure defines the avsync setting
  */
typedef enum
{
  /*!
    The default avsync effect, video pause still sync finished, then video to play
    */
  AVSYNC_PAUSE_SYNC_CMD,

  /*!
    The  avsync effect, video will playing still sync finished, no pause dead 
    */
  AVSYNC_NO_PAUSE_SYNC_CMD,

  /*!
    The  avsync not to adjust, but pts function still is working
    */
  AVSYNC_NO_ADJUST_CMD,
  
  /*!
    The  avsync resume from AVSYNC_NO_ADJUST_CMD, start to sync adjust if aud/vid started
    */
  AVSYNC_RESUME_CMD,

  /*!
    The  avsync cmd end, all cmd need less this value
    */
  AVSYNC_MAX_CMD_END
  
}AVSYNC_CMD_VSB_t;


/*!
  This structure defines the avsync cfg item
  throught cfg those itemes, can get all kinds of avsync effect
  */
typedef enum
{
   /*!
    This avsync cfg item, check the pcr and pts, audio/video will skip frame when pcr skip 
     just only use in the scene of real time ts playing 
     when ts player send the ts stream faster than the rate of av playing , enable this cfg item will keep the es buffer 
     in a normal deepth.
     the scene of non-real time ts playing, must not enabl this cfg
     BOOL value, TRUE is enable the flag, FALSE to disable
    */
   PCR_PTS_SYNC_CFG_ITEM,

   /*!
    This avsync cfg item, will keep the video smoothly playing and only adjust the audio
     just only use in the scene of non-real time ts playing 
     when enable this item, just only adjut the audio to make the av-sync 
     BOOL value, TRUE is enable the flag, FALSE to disable
    */
   VIDEO_PRIOR_CFG_ITEM,   

    /*!
    This avsync cfg item, will keep the audio smoothly playing and only adjust the video
     just only use in the scene of non-real time ts playing 
     when enable this item, just only adjut the audio to make the av-sync 
     BOOL value, TRUE is enable the flag, FALSE to disable
    */
   AUDIO_PRIOR_CFG_ITEM,   

    /*!
      This cfg, used in the avi file play
       if avi file is no real pts, just use the dst instead the pts, so need cfg the item enable
       avsync will make the dst of B frame to 0, and make other frame dst to dst.
       BOOL value, TRUE is enable the flag, FALSE to disable
     */
   FP_DST2PTS_CFG_ITEM,

}AVSYNC_CFG_ITEM_t;



/*!
  This structure defines the change channel mode. single field or double filed
  */
typedef enum
{
  /*!
    change channel mode.single field
    */
  VDEC_CHCHG_SINGLE,
  /*!
    change channel mode.double field
    */
  VDEC_CHCHG_DOUBLE,
}vdec_chchg_mode_t;

/*!
  This structure defines the supported trick modes.
  */
typedef enum
{
  /*!
    Fast forward mode
    */
  VDEC_TM_FFWD,
  /*!
    Fast reverse mode
    */
  VDEC_TM_FREV,
  /*!
    Slow forward mode
    */
  VDEC_TM_SFWD,
  /*!
    Slow reverse mode
    */
  VDEC_TM_SREV,
  /*!
    Normal play mode
    */
  VDEC_TM_NORMAL
}trick_mode_t  ;

/*!
  This structure defines the video decoding error state
  */
typedef enum
{
  /*!
    No error
    */
  VDEC_ERROR_NONE,
  /*!
    The ES buffer is underflow
    */
  VDEC_ERROR_THIRSTY,
  /*!
    Picture size error
    */
  VDEC_ERROR_SIZE,
  /*!
    Exception happens in video decoder. Need reset!
    */
  VDEC_ERROR_DIE,
  /*!
    Unknown decoding error
    */
  VDEC_ERROR_UNKNOWN
}vdec_err_t;

/*!
  This structure defines the avsync mode type
  */
typedef enum
{
  /*!
    nim default TS mode
    */
  VDEC_AVSYNC_DEFAULT_TS,
  /*!
    fileplay movie mode
    */
  VDEC_AVSYNC_FILEPLAY,
  /*!
    PVR mode
    */
  VDEC_AVSYNC_PVR,
  /*!
    fileplay TS mode
    */
  VDEC_AVSYNC_FILEPLAYTS,
}vdec_avsync_mode_t;

/*!
  This structure defines the format of input video
  */
typedef enum
{
  /*!
    PAL
    */
  VDEC_FORMAT_PAL,
  /*!
    NTSC
    */
  VDEC_FORMAT_NTSC,  
  /*!
    SECAM
    */
  VDEC_FORMAT_SECAM,
  /*!
    Unspecified video format
    */
  VDEC_FORMAT_UNSPECIFIED
}vdec_video_format_t;


/*!
  This structure defines the video decoding state
  */
typedef struct
{
  /*!
    decoding error info
    */
  vdec_err_t err;
/*!
  The aspect ratio
  */
  aspect_ratio_t  ar;
  /*!
    current source data is supported
    */
  BOOL is_sup;
  /*!
    video decoding is ready for display
    */
  BOOL is_stable;
  /*!
    AV Sync is OK.
    */
  BOOL is_sync;
  //add by HY 2012-3-19 begin
  /*!
    DMA overflow
    */
  BOOL is_overflow;
    /*!
  current source format is supported by video decoder
  */
  BOOL is_format;
  //add by HY 2012-3-19 end
  //add by HY for autotest 2012-3-28 begin
  /*!
    Key frame.
    */
  int is_key_frame[20];
  /*!
    luma address
    */
  unsigned int luma_addr[20];
  /*!
    chroma address
    */
  unsigned int chroma_addr[20];
  /*!
    heigth
    */
  unsigned int heigth;
  /*!
    width
    */
  unsigned int width;
  /*! 
    cur frames pts, 0 is invalidt
    */
  u32 pts;
  //add by HY for autotest 2012-3-28 end
  /*!
    the format of input video
    */
  disp_sys_t vid_format;
  /*! 
    Autotest flag
    */
  BOOL is_autotest;
  /*! 
    frames statistic, have display frames
    */
  u32 frames_stat;
  /*! 
    frames statistic, have decoder frames
    */
  u32 decoder_frame_count;
  /*! 
    open or close di flage
  */
  BOOL open_di;
  /*! 
    frame rate
  */
  u32 frame_rate;
  /*! 
    no enough buffer for video decoder
  */
  BOOL no_enough_buffer;
  /*! 
    First I frame output
  */
  BOOL I_frame_stable;
  /*! 
    If image is sd image, the flage is true.
  */
  BOOL sd_image_flag;
  /*! 
    If image is right, the flage is true.
  */
  BOOL image_info_enable;
} vdec_info_t;

/*!
  This structure defines the image data information
  */
typedef struct tag_data_point
{
  /*! 
    Image data x point
    */
  u8 x;
  /*! 
    Image data y point
    */
  u8 y;
  /*! 
    Y component value
    */
  u8 y_val;
  /*! 
    U component value
    */
  u8 u_val;
  /*! 
    V component value
    */
  u8 v_val;
}data_point_t;


/*!
  This structure defines the logo match information
  */
typedef struct
{
  /*! 
    The array id
    */
  u16 tp_id;
  /*! 
    Matched flage
    */
  u8 is_matched;
  /*! 
    Reserve
    */
  u8 rev;
  /*! 
    Program name
    */
  u8 name[MAX_PRO_NAME_LENGTH];
}match_names_info_t;

/*!
  Initialization parameters for image match.
  */
typedef struct 
{
  /*! 
    The array id
    */
  u16 tp_id;
  /*! 
    Data is matched flage. It should init 0XFF
    */
  u8 data_ismatched[NUMBER_PROGRAM];
  /*! 
    Index of match program. It should init 0XFF
    */
  u8 matched_index[NUMBER_PROGRAM * 2];

  /*! 
    Program name.
    */
  match_names_info_t *p_pgnames;

  /*!
    Sample data
  */
  data_point_t *p_image_points;
}vdec_image_match_t;


/*!
  Start video decoding.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] format The input video coding format.
  \param[in] mode This parameter indicates how to handle the display of video layer.
  */
RET_CODE vdec_start(void *p_dev, vdec_src_fmt_t format, vdec_start_mode_t mode);

/*!
  Stop video decoding. HW will no longer write video buffer.
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_stop(void *p_dev);

/*!
  Freeze and stop video decoding. HW will no longer write video buffer.
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_freeze_stop(void *p_dev);


/*!
  Freeze and stop video decoding. HW will no longer write video buffer.
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_switch_ch_stop(void *p_dev);


/*!
  Pause video decoding. It's only effective for stream play
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_pause(void *p_dev);

/*!
  Set frozen picture.
  
  \param[in] p_dev The pointer to the video decoder.
  
  \Return Fail if picture can not be freezed till time out, keep status when stop.
  */
RET_CODE vdec_freeze(void *p_dev);

/*!
  Resume from playback pause state or picture freezing mode.
  
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_resume(void *p_dev);

/*!
  Get video decoding state: for cable plug in/out , display on/off
  
  \param[in] p_dev The pointer to the video decoder.
  \param[out] p_state The state of video decoding.
  */
RET_CODE vdec_get_info(void *p_dev, vdec_info_t *p_state);

/*!
   This function updates the vdec info.

   \param[in] p_dev The private data for video decoder.
*/
RET_CODE vdec_info_update(void *p_dev);

/*!
  Get the required buffer size of the specific memory policy. 
  The buffer size includes both size of video es buffer and frame buffer.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] policy The memory policy for video decoding:  frame buf & es buf
  \param[out] p_size The pointer to the required buffer size
  \param[out] p_align The pointer to the required alignment of the start buffer address.
  */
RET_CODE vdec_get_buf_requirement(void *p_dev, vdec_buf_policy_t policy, u32 *p_size, u32 *p_align);

/*!
  Set the es buffer and frame buffer for video decoder.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] policy The memory policy for video decoding
  \param[in] addr The start address of the allocated buffer
  */
RET_CODE vdec_set_buf(void *p_dev, vdec_buf_policy_t policy, u32 addr);

/*!
  Set the es buffer and frame buffer for video decoder.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] addr The start address of the shared buffer
  \param[in] size The shared memory size
  */
RET_CODE vdec_set_shared_mem(void *p_dev, u32 addr, u32 size);

/*!
  decode one frame data.

  \param[in] p_dev The pointer to the video decoder.
  \param[in] p_data The pointer to the input ES video data.
  \param[in] size Size of the input ES video data.  
  */
RET_CODE vdec_dec_one_frame(void *p_dev, u8 *p_data, u32 size);

/*!
  Change video decoder to multi-picture mode.
  
  \param[in] p_dev The pointer to the video decoder.
  
  \return Fail if buffer config or video decoder work state error
  */
RET_CODE vdec_multipic_enter(void *p_dev);

/*!
  Video decoder ruturn to normal mode. 

  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_multipic_leave(void *p_dev);

/*!
  Set trick mode for video decoder. Only effective for stream file play

  \param[in] p_dev The pointer to the video decoder.
  \param[in] mode The trick mode to set
  \param[in] sr The play speed ratio. The decoder will play at 
     sr (fast trick mode) or 1/sr (slow trick mode) times of the normal play speed.
  */
RET_CODE vdec_set_trick_mode(void *p_dev, trick_mode_t mode, u8 sr);

/*!
  Video decoder set data input mode. 

  \param[in] p_dev The pointer to the video decoder.
  \param[in] is_cpu TRUE: means data come from cpu dma mode, FALSE: means data come from demux ts
  */
RET_CODE vdec_set_data_input(void *p_dev, BOOL is_cpu);

/*!
  Video decoder clear video buffer. 

  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_clear_videobuf(void *p_dev);

/*!
  set chchg mode

  \param[in] 
  \return

  */
RET_CODE vdec_chchg_mode(void *p_dev, vdec_chchg_mode_t mode);

/*!
  vdec get es buf space

  \param[in] 
  \return

  */
RET_CODE vdec_get_es_buf_space(void *p_dev, u32 *p_size);

/*!
  vdec_ dec push es

  \param[in] 
  \return

  */
RET_CODE vdec_dec_push_es(void *p_dev, u32 src_addr, u32 size,u32 vpts);
/*!
  vdec_ dec clear es

  \param[in] 
  \return

  */
RET_CODE vdec_dec_clear_es_buf(void *p_dev);
/*!
  vdec_ dec get pts

  \param[in] 
  \return

  */
u32 vdec_dec_get_vpts(void *p_dev);
/*!
  vdec_ dec get pts

  \param[in] 
  \return

  */
RET_CODE vdec_set_avsync(void *p_dev,BOOL en_avsync);
/*!
  vdec_ dec set avsync mode
  \param[in] 
  \return
  */
RET_CODE vdec_set_avsync_mode(void *p_dev,u32 mode);
/*!
  vdec_ dec set file playback frame rate
  \param[in] 
  \return
  */
RET_CODE vdec_set_file_playback_framerate(void *p_dev,u32 frame_rate);

/*!
  vdec_file_clearesbuffer
  \param[in] 
  \return
  */
 RET_CODE vdec_file_clearesbuffer(void *p_dev);

/*!
  vdec detec display order
  \param[in] 
  \return
  */
 RET_CODE vdec_detec_display_order(void *p_dev);

/*!
   This function get program name init.
   
   \param[in] p_dev The pointer to the video decoder.
   \param[in] p_image_data The image data for video decoder.
  */
RET_CODE vdec_get_pgname_init(void *p_dev, void *p_image_data);

/*!
  This function get program name index.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] image.
  \param[in] program index.
  */
RET_CODE vdec_get_pgname_index(void *p_dev, void *p_image_data, u8 index);

/*!
  AVS detec pts repeat
  \param[in] p_dev The pointer to the video decoder.
  */
RET_CODE vdec_avs_detec_pts_repeat(void *p_dev);


/*!
  Force input frame rate, you should use the fun before vdec start.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] frame_rate The param is about input frame rate.
  */
RET_CODE vdec_set_force_input_frame_rate(void *p_dev, u8 frame_rate);

//#if (defined(CONFIG_YINHE_WARRIOS_SDK))
/*!
  set vdec handle
  */
RET_CODE vdec_set_dev_handle(int vdec_handle);

/*!
  do some avsync cmd, AVSYNC_CMD_VSB_t will not been reset by avsync if playing end until uplayer to reset it 
  this function will effect all program playing
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] cmd  The AVSYNC_CMD_VSB_t cmd 
  \param[in] data  The reserve parameter
  */
RET_CODE vdec_do_avsync_cmd(void *p_dev, AVSYNC_CMD_VSB_t cmd, u32 data);

/*!
  do some avsync cfg, avsync will reset the cfg item when every playing end. 
  uplayer need cfg it before start playing one program.
  this function only effect the next playing program.
  
  \param[in] p_dev The pointer to the video decoder.
  \param[in] cmd  The AVSYNC_CFG_ITEM_t cmd 
  \param[in] data  the cfg value, TRUE/FALSE, or other value
  */
RET_CODE vdec_do_avsync_cfg(void *p_dev, AVSYNC_CFG_ITEM_t cfg_item, u32 data);

//#endif

#ifdef __cplusplus
}
#endif
#endif //__VDEC_H__

