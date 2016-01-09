/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AVCTRL1_H_
#define __AVCTRL1_H_

/*!
  \file avctrl.h

  Simply wrapper interfaces for AV drivers and database
  */

/*!
  Picture rectangle parameters for multi pictures
  */
typedef struct 
{
  /*!
    Left position of video rectangle
    */
  u16 left;
  /*!
    Top position of video rectangle
    */
  u16 top;
  /*!
	  Width of video rectangle
    */
  u16 width;
  /*!
	  Height of video rectangle
    */
  u16 height;
} avc_rect_1_t;

/*!
  Audio channel mode
  */
typedef enum
{
  /*!
    Stereo audio
    */
  AVC_AUDIO_STEREO_1 = 0,
  /*!
    Left audio channel 
    */
  AVC_AUDIO_LEFT_1,
  /*!
    Right audio channel
    */
  AVC_AUDIO_RIGHT_1,
  /*!
    Mono audio mode
    */
  AVC_AUDIO_MONO_1
} avc_audio_mode_1_t;

/*!
  Video display mode
  */
typedef enum
{
  /*!
    AV display mode. depend by stream standard
    */
  AVC_VIDEO_MODE_AUTO_1,
  /*!
    AV display mode. NTSC standard 480i
    */
  AVC_VIDEO_MODE_NTSC_1,
  /*!
    AV display mode. PAL standard 576i
    */
  AVC_VIDEO_MODE_PAL_1,
  /*!
    AV display mode. PAL N standard
    */
  AVC_VIDEO_MODE_PAL_M_1,
  /*!
    AV display mode. PAL M standard
    */
  AVC_VIDEO_MODE_PAL_N_1,
  /*!
    AV display mode. 480P
    */
  AVC_VIDEO_MODE_480P,
  /*!
    AV display mode. 576P
    */
  AVC_VIDEO_MODE_576P,
  /*!
    AV display mode. 720P
    */
  AVC_VIDEO_MODE_720P_50HZ,
  /*!
    AV display mode. 720P
    */
  AVC_VIDEO_MODE_720P_60HZ,  
  /*!
    AV display mode. 1080I
    */
  AVC_VIDEO_MODE_1080I_50HZ,
  /*!
    AV display mode. 1080I
    */
  AVC_VIDEO_MODE_1080I_60HZ,
  /*!
    AV display mode. 1080P
    */
  AVC_VIDEO_MODE_1080P_50HZ,
  /*!
    AV display mode. 1080P
    */
  AVC_VIDEO_MODE_1080P_60HZ,
  /*!
    AV display mode. 480i
    */
  AVC_VIDEO_MODE_480I,
  /*!
    AV display mode. 576i
    */
  AVC_VIDEO_MODE_576I,
}avc_video_mode_1_t;

/*!
  This structure defines the supported aspect ratio for video display.
  */
typedef enum
{
  /*!
    Auto mode
    */
  AVC_VIDEO_ASPECT_AUTO_1,
  /*!
    4:3 PanScan mode
    */
  AVC_VIDEO_ASPECT_43_PANSCAN_1,
  /*!
    4:3 LetterBox mode
    */
  AVC_VIDEO_ASPECT_43_LETTERBOX_1,
  /*!
    16:9 mode
    */
  AVC_VIDEO_ASPECT_169_1,
  /*!
    16:9 PanScan mode
    */
  AVC_VIDEO_ASPECT_169_PANSCAN_1,
  /*!
    16:9 LetterBox mode
    */
  AVC_VIDEO_ASPECT_169_LETTERBOX_1,  
}avc_video_aspect_1_t;
/*!
  This structure defines the supported signal out mode for jazz.
  */
typedef enum
{
  /*!
    CVBS format for  video out.
    */
  VIDEO_OUT_VID_CVBS = 0,
  /*!
    SVDIEO format for  video out.
    */
 VIDEO_OUT_VID_SVDIEO ,
  /*!
    RGB format for  video out.
    */
  VIDEO_OUT_VID_RGB,
  /*!
    YUV format for  video out.
    */
  VIDEO_OUT_VID_YUV,
  /*!
    YUV + CVBS format for  video out.
    */
  VIDEO_OUT_VID_CVBS_YUV,
  /*!
    YUV + CVBS format for  video out.
    */
  VIDEO_OUT_VID_CVBS_SVDIEO,
  /*!
    CVBS+CVBS format for video out.
    */
    VIDEO_OUT_VID_CVBS_CVBS,
  /*!
    close all dac.
    */
  VIDEO_OUT_VID_NULL
  
}video_out_format_t;


  /*!
    avc start video params struct
    */
typedef struct video_param
{
  /*!
    video format
    */
  vdec_src_fmt_t format;
  /*!
    video start mode
    */
  vdec_start_mode_t mode;
  /*!
    ts file play
    */
   BOOL file_play;
}video_param_t;

/*!
  avc config
  */
typedef struct
{
  /*!
    disp sys format
    */
  disp_sys_t disp_fmt;
  /*!
    disp channel
    */
  disp_channel_t disp_chan;    
}avc_cfg_t;
  
/*!
  avc start audio params struct
  */
typedef struct audio_param
{
/*!
  audio type
  */
  adec_src_fmt_vsb_t type;
/*!
  audio decoding file type
  */
  adec_file_fmt_vsb_t file_type;
}audio_param_t;

/*!
  avc start type
  */
typedef enum
{
/*!
  avc start video only
  */
  AV_VIDEO_START =1,
/*!
  avc start audio only
  */
  AV_AUDIO_START = 2,
/*!
  avc start all
  */
  AV_START_ALL,
}av_start_type_t;

/*!
  set video dac
  \param[in] p_data private data
  \param[in] mode: video format in magic chip
  */
void avc_cfg_scart_format_1(void *p_data, scart_v_format_t mode);

/*!
  config tv master
  \param[in] p_data private data
  \param[in] terminal scart terminal
  */
void avc_cfg_scart_select_tv_master_1(void *p_data, scart_terminal_t terminal);

/*!
  config video aspect
  \param[in] p_data private data
  \param[in] mode: aspect mode
  */
void avc_cfg_scart_aspect_1(void *p_data, scart_v_aspect_t mode);


/*!
  config vcr input
  \param[in] p_data private data
  \param[in] terminal scart terminal
  */
void avc_cfg_scart_vcr_input_1(void *p_data, scart_terminal_t terminal);

/*!
  detect scart vcr is activated or not
  \param[in] p_data private data
  */
BOOL avc_detect_scart_vcr_1(void *p_data);

/*!
  set rf system
  \param[in] p_data private data
  \param[in] sys: rf system
  */
BOOL avc_set_rf_system_1(void *p_data, rf_sys_t sys);

/*!
  set rf channel
  \param[in] p_data private data
  \param[in] channel: rf channel
  */
BOOL avc_set_rf_channel_1(void *p_data, u16 channel);

/*!
  Multi picture mode supported in AVC
  */
typedef enum
{
  /*!
    Normal AVC mode
    */
  AVC_NORMAL_1 = 0,
  /*!
    9 pictures displaying mode
    */
  AVC_9_PICTURES_1, 
  /*!
    6 pictures supported
    */
  AVC_6_PICTURES_1
} avc_multi_pic_mode_1_t;

/*!
  Notification callback function type define
  Notify UI set OSD position after switch P/N
  */
typedef void (*avc_callback_screen_pos_1_t)(BOOL is_ntsc);
/*!
  AV control module initilization
  */
void avc_init_1(avc_cfg_t *p_avc_cfg);

/*!
  Install notify callback functions.
  \param[in] p_data avc private data
  \param[in] p_proc: Function address.
  \return  TRUE if succeed. FALSE for failed.
  */
BOOL avc_install_screen_notification_1(void *p_data, 
avc_callback_screen_pos_1_t p_proc);

/*!
  PAL/NTSC switch for OSD layer. This fuction modify UI move osd region position
  according to the video PAL/NTSC status,
  \param[in] p_data avc private data
  \param[in] is_ntsc: TRUE is NTSC;
                      FALSE is PAL.
  */
void avc_reset_screen_pos_1(void *p_data, BOOL is_ntsc);

/*!
  Set audio volume
  
  \param[in] p_data avc private data
  \param[in] volume: audio volume, only use 5 bits \sa ProgNode
  */
void avc_setvolume_1(void *p_data, u8 volume);

/*!
  Change audio playing mode
  
  \param[in] p_data avc private data
  \param[in] mode:  0 stereo, 1 left, 2 right
  */
void avc_set_audio_mode_1(void *p_data, avc_audio_mode_1_t mode);

/*!
  Check the mute status of avc module
  
  \param[in] p_data avc private data
  \param[out] : TURE avc is mute, FALSE avc is not mute ;
  */
BOOL avc_is_mute_1(void *p_data);

/*!
  Set mute
  
  \param[in] p_data avc private data
  \param[in] is_mute 0: unmute, 1: mute;
  \param[in] is_update 1: change flag, 0: unchanged
  */
void avc_set_mute_1(void *p_data, BOOL is_mute, BOOL is_update);
/*!
  Enter preview
  
  \param[in] p_data avc private data
  \param[in] left: left position of rectangle 
  \param[in] top:  top position of rectangle
  \param[in] height: height of rectangle
  \param[in] width:  width of rectangle
  \param[out] : confirm video format size is correct ;
  */
RET_CODE avc_enter_preview_1(void *p_data, u16 left, u16 top, u16 right, 
u16 bottom);

/*!
  Config a program in an window, it works after pb_enter_preview
  
  \param[in] p_data avc private data
  \param[in] left Left position of window in pixel
  \param[in] top Top position of window in pixel
  \param[in] width Width of window in pixel
  \param[in] height Height of window in pixel
  */
void avc_config_preview_1(void *p_data, u16 left, u16 top, u16 right, 
u16 bottom);

/*!
  Leave preview mode
  \param[in] p_data avc private data
  */
void avc_leave_preview_1(void *p_data);

/*!
  Set video mode
  \param[in] p_data avc private data
  \param[in] mode: new avc mode to be set
  */
void avc_switch_video_mode_1(void *p_data, avc_video_mode_1_t mode);


/*!
  Get video mode
  \param[in] p_data avc private data
  \return current video steam mode.
  */
disp_sys_t avc_get_video_mode_1(void *p_data);

/*!
  Set video aspect
  \param[in] p_data avc private data
  \param[in] mode: new aspect mode to be set
  */
void avc_set_video_aspect_mode_1(void *p_data, avc_video_aspect_1_t mode);

/*!
  Pause video
  \param[in] p_data avc private data
  */
BOOL avc_video_freeze_1(void *p_data);

/*!
  Resume video
  
  \param[in] p_data avc private data
  \param[out] : TRUE video resume, FALSE video not resume
  */
BOOL avc_video_resume_1(void *p_data);

/*!
  Enter vide preview by compress mode
  \param[in] p_data avc private data
  \param[in] p_av_rect config parameter for avc display
  \param[in] buf_addr buffer address 
  */
//void avc_enter_compress_preview_1(void *p_data, rect_t *p_rect,
  //u32 buf_addr);

/*!
  Leave compressed preview mode
  
  \param[in] p_data avc private data
  \param[in] b_replay: TRUE-> play video when leave preview
                       False-> do not play video when leave preview
  */
//void avc_leave_compress_preview_1(void *p_data, BOOL b_replay);

/*!
  video audio start
  
  \param[in] p_data avc private data
  \param[in] p_video: video config params
  \param[in] p_audio: audio config params
  \param[in] type: av start enum, see av_start_type_t
  */
BOOL avc_av_start(void *p_data,
  video_param_t *p_video, audio_param_t *p_audio, av_start_type_t type);

/*!
  video audio stop
  
  \param[in] p_data avc private data
  */
BOOL avc_av_stop(void *p_data);

/*!
  video audio pause
  
  \param[in] p_data avc private data
  */
BOOL avc_av_freeze(void *p_data);

/*!
  video audio resume
  
  \param[in] p_data avc private data
  */
BOOL avc_av_resume(void *p_data);

/*!
   audio pause only
  
  \param[in] p_data avc private data
  */
BOOL avc_audio_pause(void *p_data);

/*!
   audio resume only
  
  \param[in] p_data avc private data
  */
BOOL avc_audio_resume(void *p_data);

/*!
  Set video mode
  \param[in] p_data avc private data
  \param[in] mode: new avc mode to be set
  */
void avc_switch_video_mode_jazz(void *p_data, avc_video_mode_1_t new_std);

/*!
  Set video_sature
  \param[in] p_data avc private data
  \param[in] percent:
  */
void avc_set_video_sature(void *p_data, u8 percent);

/*!
  Set contrast
  \param[in] p_data avc private data
  \param[in] percent: 
  */
void avc_set_video_contrast(void *p_data, u8 percent);

/*!
  Set video bright
  \param[in] p_data avc private data
  \param[in] percent: 
  */
void avc_set_video_bright(void *p_data, u8 percent);

/*!
  Set avc_cfg_scart_format_jazz
  \param[in] p_data avc private data
  \param[in] percent: 
  */
void avc_cfg_scart_format_jazz(void *p_data, scart_v_format_t mode);
/*!
  Set avc_cfg_video_out_format_jazz
  \param[in] p_data avc private data
  \param[in] percent: 
  */
void avc_cfg_video_out_format_jazz(void *p_data, video_out_format_t mode);


/*!
  swich channel
  \param[in] p_data : avc private data
  \param[in] disp_chann : display channel

  \return : NULL
  */
void avc_video_switch_chann(void *p_data, disp_channel_t disp_chann);

/*!
  update region, just for warriors.
  
  \param[in] p_data : avc private data
  \param[in] p_region : region
  \param[in] p_rect : rect

  \return : NULL  
  */
void avc_update_region(void *p_data, void *p_region, rect_t *p_rect);
#endif // End for __AVCTRL_H_
