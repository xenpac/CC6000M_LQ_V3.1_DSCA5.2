/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __AUD_VSB_H__
#define __AUD_VSB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  data source (use data in header file  or  use file system)
  */
#define AUD_HEAD_FILE_OR_FILE_SYSTEM 0

/*!
  data source (use data in header file  or  use file system)
  */
#define MP3_HEAD_FILE_OR_FILE_SYSTEM 0

  /*!
  The audio decoder get es count timeout value in ticks.
  */
#define AUDIO_GET_ES_TIMEOUT 20


/*!
  fix me
  */
typedef struct
{
/*!
  fix me
  */
  u32 frame_cnt;

/*!
     mp3_bitrate
   */
  u32                 mp3_bitrate;

/*!
      audio channel exist
    */
  u32 channel;

/*!
      mp3_chan_mode
    */
  u32 mp3_chan_mode;
} aud_info_vsb_t;

/*!
  This structure defines the supported formats of audio source
  */
typedef enum
{
  /*!
    PCM
    */
  AUDIO_PCM = 0,
  /*!
    MPEG audio layer I
    */
  AUDIO_MP1 = 1,
  /*!
    MPEG audio layer II
    */
  AUDIO_MP2 = 2,
  /*!
    MPEG audio layer III
    */
  AUDIO_MP3 = 3,
  /*!
     AC3
    */
  AUDIO_AC3_VSB = 4,
  /*!
    EAC3
    */
  AUDIO_EAC3 = 5,
  /*!
    HE_AAC
    */
  AUDIO_AAC = 6,
  /*!
    AAC_V2
    */
  AUDIO_AAC_V2 = 7,
  /*!
    AUDIO_DRA
    */
  AUDIO_DRA = 8,
  /*!
    SPDIF
    */
  AUDIO_SPDIF = 107,
  /*!
    dolby convert
    */
  AUDIO_DOLBY_CONVERT = 108,
  /*!
   SPDIF_AC3
    */
  AUDIO_SPDIF_AC3,
  /*!
   SPDIF_EAC3
    */
  AUDIO_SPDIF_EAC3,
  /*!
    Unknown audio format.
    */
  AUDIO_UNKNOWN  
}adec_src_fmt_vsb_t;

/*!
  This structure defines input audio source format when file play
  */
typedef enum
{
  /*!
    NO File
    */
  AUDIO_NO_FILE,
  /*!
    TS FILE
    */
  AUDIO_TS_FILE,
  /*!
    ES FILE
    */
  AUDIO_ES_FILE,
  /*!
    PCM FILE
    */
  AUDIO_PCM_FILE
}adec_file_fmt_vsb_t;

/*!
  This structure defines the status of supporting of dolby
  */
typedef enum
{
  /*!
    supported
    */
  AUDIO_DOLBY_SUPPORTED,
  /*!
    NOT_SUPPORTED_BY_HARDWARE
    */
  AUDIO_DOLBY_NOT_SUPPORTED_BY_HARDWARE,
  /*!
    NOT_SUPPORTED_BY_SOFTWARE
    */
  AUDIO_DOLBY_NOT_SUPPORTED_BY_SOFTWARE,
  /*!
    NOT_SUPPORTED_BOTH
    */
  AUDIO_DOLBY_NOT_SUPPORTED_BOTH
}aud_dolby_status_vsb_t;


/*!
  This structure defines input audio source format when file play
  */
typedef enum
{

/*!
   ac3 audio frame  format type
  */
  AC3_TYPE = 0,
  
/*!
   eac3 audio frame  format type
  */ 
  EAC3_TYPE,

/*!
   AAC ADIF audio frame format type
  */
  AAC_ADIF_TYPE,

/*!
   AAC ADTS audio frame format type
  */
  AAC_ADTS_TYPE,

/*!
   PCM format type
  */
  PCM_TYPE,

/*!
   other unknown audio frame format type
  */
  UNKNOWN_TYPE
}audio_frm_type_t;




/*!
  This structure defines the supported PCM sample rates.
  */
typedef enum
{
  /*!
    Sample rate is 22.05k 
    */
  AUDIO_SAMPLE_22_VSB = 0,
  /*! 
    Sample rate is 24k 
    */
  AUDIO_SAMPLE_24_VSB = 1,
  /*!
    Sample rate is 16k 
    */
  AUDIO_SAMPLE_16_VSB = 2,
  /*!
    Sample rate reserved1
    */
  AUDIO_SAMPLE_RES1_VSB = 3,
  /*! 
    Sample rate is 44.1k 
    */
  AUDIO_SAMPLE_44_VSB = 4,
  /*! 
    Sample rate is 48k 
    */
  AUDIO_SAMPLE_48_VSB = 5,
  /*! 
    Sample rate is 32k
    */
  AUDIO_SAMPLE_32_VSB = 6,
  /*! 
    Sample rate reserved2
    */
  AUDIO_SAMPLE_RES2_VSB = 7,
  /*! 
    Sample rate is 11k
    */
  AUDIO_SAMPLE_11_VSB = 8,
  /*! 
    Sample rate is 12k
    */
  AUDIO_SAMPLE_12_VSB = 9,
  /*! 
    Sample rate is 8k
    */
  AUDIO_SAMPLE_8_VSB = 10,
  /*! 
    Sample rate reserved3
    */
  AUDIO_SAMPLE_RES3_VSB = 11,
  /*! 
    Sample rate is 88k
    */
  AUDIO_SAMPLE_88_VSB = 12,
  /*! 
    Sample rate is 96k
    */
  AUDIO_SAMPLE_96_VSB = 13,
  /*! 
    Sample rate is 64k
    */
  AUDIO_SAMPLE_64_VSB = 14  
} aud_sample_rate_vsb_t;

/*!
  This structure defines the supported PCM play modes.
  */
typedef enum
{
  /*! 
    Formal routine when playing pcm data.
    */
  AUDIO_PCM_MODE_VSB = 0,
  /*! 
    for debug only.
    Write PP buf when playing multiple pcm channels(multiple files and each 
    file contains 1 channel data).
    */
  AUDIO_PCM_MODE_WRITE_PP_BUF_VSB,
  /*! 
    for debug only.
    Write PP buf when playing 1 or 2 pcm data(1 file containing 2 channels' data).
    */
  AUDIO_PCM_MODE_WRITE_PCM_BUF_VSB
} aud_pcm_play_mode_vsb_t;

/*!
  This structure defines the supported  channel modes.
  */
typedef enum
{
  /*! 
    PCM data with 2 channels.
    */
  AUDIO_CHANNEL_STEREO_VSB = 0,
  /*! 
    PCM data with left channel.
    */
  AUDIO_CHANNEL_LEFT_VSB,
  /*! 
    PCM data with right channel.
    */
  AUDIO_CHANNEL_RIGHT_VSB
} aud_channel_mode_vsb_t;



/*!
  Parameters for direct play audio PCM data.
  */
typedef struct
{
  /*! 
    Endian of the PCM data. TRUE for little endian and FALSE for big endian 
    */
  BOOL is_big_endian;
  /*!
    Audio channel setting for the PCM data: Stereo or Mono 
    */
  BOOL is_stereo;
  /*! 
    The sample rate for the input PCM data 
    */
  aud_sample_rate_vsb_t sample;
  /*!
    Bits per sample
    */
  int   bits;
  /*!
    Audio play mode.
    */
  aud_pcm_play_mode_vsb_t play_mode;

  /*!
    Audio channel mode.
    */
  aud_channel_mode_vsb_t channel_mode;

  
  /*!
    for debug
    determining which channel exists.(3 means 1 and 2 chan exists,0xf means 
    1/2/3/4 channels exist...)
    */
  BOOL exist_channels;
} aud_pcm_param_vsb_t;

/*!
  This structure defines the supported audio channel output mode.
  */
typedef enum
{
  /*! 
    Stereo output 
    */
  AUDIO_MODE_STEREO = 0,
  /*! 
    Left channel output
    */
  AUDIO_MODE_LEFT = 1,
  /*!
    Right channel output
    */
  AUDIO_MODE_RIGHT = 2,
  /*!
    Mono output
    */
  AUDIO_MODE_MONO = 3
} aud_mode_vsb_t;

/*!
  aud description volume mode
  */
typedef enum
{
 /*! 
    ad volue default
    */
  AD_VOL_DEFAULT = 0,
  /*! 
    ad volue -3
    */
  AD_VOL_LEVEL_NEG_3 = 1,
  /*! 
      ad volue -2
    */
  AD_VOL_LEVEL_NEG_2 ,
  /*!
     ad volue -1
    */
  AD_VOL_LEVEL_NEG_1 ,
  /*!
    ad volue 0
    */
  AD_VOL_LEVEL_0 ,
  /*!
    ad volue 1
    */
  AD_VOL_LEVEL_1,
  /*!
    ad volue 2
    */
  AD_VOL_LEVEL_2,
 /*!
    ad volue 3
    */
  AD_VOL_LEVEL_3,
  
} ad_vol_mode_t;
 
/*!
  The supported audio postprocessing
  */
typedef enum
{
  /*!
    Equalizer of 5 bands
    */
  AUDIO_PP_EQ,
  /*!
    Super bass
    */
  AUDIO_PP_BASS,
  /*!
    3D virtual surrounding stereo
    */
  AUDIO_PP_VS,
  /*!
    Pseudo stereo
    */
  AUDIO_PP_PSTEREO,
  
  /*!
    sample rate convertion 
    */
  AUDIO_PP_SRC,

  /*!
    auto gain control 
    */
  AUDIO_PP_AGC,

  /*!
    downmix   
    */
  AUDIO_PP_DOWNMIX,

  /*!
    fader in/out   
    */
  AUDIO_PP_FADER,
  /*!
    mix channel -- only for concerto
    */
  AUDIO_MIX,
  /*!
    ssrc for mix channel -- only for concerto
    */
  AUDIO_MIX_SSRC,
  /*!
    Reverberation 
    */
  AUDIO_PP_REVERB
}aud_pp_vsb_t;

/*!
  The supported EQ bands
  */
typedef enum
{
  /*!
    Band 1
    */
  AUDIO_EQ_BAND1,
  /*!
    Band 2
    */
  AUDIO_EQ_BAND2,
  /*!
    Band 3
    */
  AUDIO_EQ_BAND3,
  /*!
    Band 4
    */
  AUDIO_EQ_BAND4,
  /*!
    Band 5
    */
  AUDIO_EQ_BAND5
}aud_pp_eq_vsb_t;


/*!
  The supported SPDIF output format
  */
typedef enum
{
  /*!
    SPDIF raw/original AC3 data
    */
  AUDIO_SPDIF_RAW,
  /*!
    SPDIF LPCM data
    */
  AUDIO_SPDIF_LPCM,
}aud_spdif_out_fmt_vsb_t;
  

/*!
  The supported channel copy
  */
typedef enum
{
  /*!
    DONOT_COPY
    */
  DONOT_COPY,

  /*!
    RIGHT_TO_LEFT
    */
  RIGHT_TO_LEFT,

  /*!
    LEFT_TO_RIGHT
    */
  LEFT_TO_RIGHT
}aud_lrcopy_vsb_t;

/*!
  This structure defines an audio device.
  */
typedef struct 
{
  /*!
    Pointer to device head
    */
  void *p_base;
  /*!
    Pointer to private data
    */
  void *p_priv;
}aud_device_vsb_t;

/*!
  Parameters for fader in/out of audio pp module.
  */
typedef struct
{
  /*! 
    TRUE for Fader in and FALSE for Fader out. 
    */
  BOOL is_fader_in;
  /*!
    fader target volume.
    */
  u32 target_gain;

  /*!
    fader step.
    */
  u32 step;

  /*!
    fader time in ms.
    */
  u32 time_step;  
} aud_pp_fader_param_vsb_t;

/*!
  Parameters for auto gain control.
  */
typedef struct
{  
  /*!
    attack time.
    */
  u8 attack_time;

  /*!
    target volume gain.
    */
  u8 attack_step;

  /*!
    target volume gain.
    */
  u8 release_time;

  /*!
    target volume gain.
    */
  u8 release_step;
    
  /*!
    target volume gain.
    */
  u16 target_level;  
} aud_pp_agc_param_vsb_t;

/*!
  Initialization parameters for the audio
  */
typedef struct
{
  /*!
    if use the public sharing driver service, set the handle here 
    */
  void *p_drvsvc; 
  /*!
    if not use the public sharing driver service, set this, hdmi notify task priority
    */
  u32 task_prio;
  /*!
    if not use the public sharing driver service, set this, hdmi notify task size
    */  
  u32 stack_size;
  /*!
    lock mode
    */  
  u32 lock_mode;
}aud_cfg_t;


/*!
  Parameters for downmix  of audio pp module.
  Each coefficient can not be larger than 0x8000.
  The sum of the 8 coefficients to the same output channel can not be larger than 0x8000.
  */
typedef struct
{    
  
  /*!
    coefficient of left input channel to left output channel 
    */
  u16 coef_left_left;
  
  /*!
    coefficient of left input channel to right output channel 
    */
  u16 coef_left_right;   
  
  /*!
    coefficient of right input channel to left output channel 
    */
  u16 coef_right_left;
  
  /*!
    coefficient of right input channel to right output channel
    */
  u16 coef_right_right;    
  
  /*!
    coefficient of center input channel to left output channel 
    */
  u16 coef_center_left;       
  
  /*!
    coefficient of center input channel to right output channel
    */
  u16 coef_center_right;  
  
  /*!
    coefficient of bass input channel to left output channel
    */  
  u16 coef_bass_left;       
  
  /*!
    coefficient of bass input channel to right output channel
    */
  u16 coef_bass_right;  
  
  /*!
    coefficient of sl input channel to left output channel 
    */
  u16 coef_sl_left; 
  
  /*!
    coefficient of sl input channel to right output channel 
    */
  u16 coef_sl_right;    
  
  /*!
    coefficient of sr input channel to left output channel 
    */
  u16 coef_sr_left;  
  
  /*!
    coefficient of sr input channel to right output channel 
    */
  u16 coef_sr_right; 
  
  /*!
    coefficient of rsl input channel to left output channel 
    */
  u16 coef_rsl_left;   
  
  /*!
    coefficient of rsl input channel to right output channel 
    */
  u16 coef_rsl_right;  
  
  /*!
    coefficient of rsr input channel to left output channel 
    */
  u16 coef_rsr_left; 
  
  /*!
    coefficient of rsr input channel to right output channel 
    */
  u16 coef_rsr_right;         
} aud_pp_downmix_param_vsb_t;

/*!
  Parameters for play pcm directly. 
  */
typedef struct
{  
  /*!
    which channel to play 7->111b->1,2,3channel to play
    */
  u8 channel;   
  
  /*!
    1: play ; 0: stop 
    */
  u8 play_or_stop;   
  
  /*!
    for customer use
    */
  void *reserve;
  
  /*!
    8 channel audio source buf address
    */
  char *buf_addr[8];
  
  /*!
    8 channel audio source buf size
    */
  u32    buf_size[8];
  
  /*!
    read counter in each channel
    */
  u32    buf_counter[8];  

  /*!
    file pointer of each channel used in file system
    */
  void    *p_fp[8];  
}aud_play_pcm_vsb_t;

/*!
  parameters for dma cpy
  */
typedef struct
{
  /*!
    buf address in ddr buffer
    */
  void * p_ddr_buf;  

  /*!
    copy size
    */
  u32    cpy_size;   

  /*!
    offset in sp ram buffer
    */
  u32    sp_ram_offset;

  /*!
    copy direction (ddr->sp or sp->ddr)
    */
  BOOL sp_to_ddr;
}aud_dma_cpy_vsb_t;

/*!
  This structure defines the audio decoding error state
  */
typedef enum
{
  /*!
    No error
    */
  AUDIO_ERROR_NONE,
  /*!
    The ES buffer is underflow
    */
  AUDIO_ERROR_THIRSTY,
  /*!
    Exception happens in audio decoder. Need reset!
    */
  AUDIO_ERROR_DIE,
  /*!
    Unknown decoding error
    */
  AUDIO_ERROR_UNKNOWN
} aud_err_t;


/*!
  This structure defines the video decoding state
  */
typedef struct
{
  /*!
    decoding error info
    */
  aud_err_t err;
  /*!
    The state of mute
    */
  BOOL is_mute;
    /*!
    The volume level
    */
   u8 volume;
    /*!
      The platform: TRUE for chip and FALSE for fpga platform
      */
    BOOL is_chip;   
} aud_info_t;


/*!
  aud_set_ad_param_vsb : for audio description
  
  \param[in] p_dev The pointer to the audio device.
  \param[in] p_param The pointer the pcm parameters.
  */
RET_CODE aud_set_ad_param_vsb(void *p_dev, void *p_param);

/*!
  aud_set_ad_vol_vsb : for audio description
  
  \param[in] p_dev The pointer to the audio device.
  \param[in] p_param The pointer volume type.
  */
RET_CODE aud_set_ad_vol_vsb(void *p_dev, void *p_param);

/*!
  aud_set_ad_vol_vsb : for audio description
  
  \param[in] p_dev The pointer to the audio device.
  \param[in] param :on or off.
  */
RET_CODE aud_set_ad_vol_flag(void *p_dev, u32 param);
/*!
  Set PCM playback param, this function must only be called before function 
  aud_start for playing back PCM data.
  
  \param[in] p_dev The pointer to the audio device.
  \param[in] p_param The pointer the pcm parameters.
  */
RET_CODE aud_set_pcm_param_vsb(void *p_dev, const aud_pcm_param_vsb_t *p_param);

/*!
  Start audio decoding
  
  \param[in] p_dev The pointer to the audio device.
  */
RET_CODE aud_start_vsb(void *p_dev, adec_src_fmt_vsb_t type,  adec_file_fmt_vsb_t file_fmt);

/*!
  Stop audio decoding
  
  \param[in] p_dev The pointer to the audio device.
  */
RET_CODE aud_stop_vsb(void *p_dev);

/*!
  Get the required buffer size of audio device, including audio es buffer.
  
  \param[in] p_dev The pointer to the audio device.
  \param[out] p_size The pointer to the required buffer size (Kbytes)
  \param[out] p_align Alignment requirement.
  */
RET_CODE aud_get_buf_requirement_vsb(void *p_dev, u32 *p_size, u32 *p_align);

/*!
  Set the buffer for audio device, including audio es buffer, etc.
  
  \param[in] p_dev The pointer to the audio device.
  
  \param[in] mem_policy The memory policy for video decoding
  \param[in] addr The start adress of frame buffer
  */
RET_CODE aud_set_buf_vsb(void *p_dev, u32 addr, u32 size);

/*!
  Push the file buffer to the audio es buffer.
  \param[in] p_dev The pointer to the audio device.
  \param[in] src_addr The addr of the src buffer.
  \param[in] size The size of the buffer 
   */
RET_CODE aud_file_pushesbuffer_vsb(void *p_dev, u32 src_addr, u32 size, u32 apts);

/*!
  Get total audio es buffer.
  \param[in] p_dev The pointer to the audio device.
  \param[out] p_size The total audio es buffer
   */
RET_CODE aud_file_gettotalesbuffer_vsb(void *p_dev, u32 *p_size);

/*!
  Get left audio es buffer.
  \param[in] p_dev The pointer to the audio device.
  \param[out] p_size The available audio es buffer
   */
RET_CODE aud_file_getleftesbuffer_vsb(void *p_dev, u32 *p_size);

/*!
  Get es buffer info.
  \param[in] p_dev The pointer to the audio device.
  \param[out] p_bool TBD
   */
RET_CODE aud_file_getesbufferinfo_vsb(void *p_dev, BOOL *p_bool);

/*!
  set the output mode of the audio channels
  
  \param[in] p_dev The pointer to the audio device.
  \param[in] mode The output mode of audio channels
  */
RET_CODE aud_set_play_mode_vsb(void *p_dev, aud_mode_vsb_t mode);

/*!
  set the volume of audio decoder output
  
  \param[in] p_dev The pointer to the audio device.
  \param[in] percent_l percentage of full volume to set for left channel
  \param[in] percent_r percentage of full volume to set for right channel
  */
RET_CODE aud_set_volume_vsb(void *p_dev, u8 percent_l, u8 percent_r);

/*!
  Mute/Unmute audio play

  \param[in] p_dev The pointer to the audio device.
  \param[in] is_on TRUE to enable mute state and FALSE to disable it.
  */
RET_CODE aud_mute_onoff_vsb(void *p_dev, BOOL is_on);  

/*!
  Pause audio decoding. Only effective for stram file play.
  
  \param[in] p_dev The pointer to the audio device.
  */
RET_CODE aud_pause_vsb(void *p_dev);

/*!
  Resume audio decoding from paused state. Only effective for stram file play.
  
  \param[in] p_dev The pointer to the audio device.
  */
RET_CODE aud_resume_vsb(void *p_dev);

/*!
  Enable or disable post processing.
  
  \param[in] p_dev The pointer to the audio device.
  \param[in] type The type of post processing
  \param[in] is_on TRUE to enable and FALSE to disable it.
  */
RET_CODE aud_pp_onoff_vsb(void *p_dev, aud_pp_vsb_t type, BOOL is_on);

/*!
  Set the parameters of 3D virtual surrounding stereo.

  \param[in] p_dev The pointer to the audio device.
  \param[in] gain_center center gain
  \param[in] gain_space space gain
  */
RET_CODE aud_pp_set_vs_vsb(void *p_dev, u8 gain_center, u8 gain_space);

/*!
  Set the parameters of reverberation.

  \param[in] p_dev The pointer to the audio device.
  \param[in] delay delay of reverberation
  \param[in] gain gain of reverberation
  */
RET_CODE aud_pp_set_reverb_vsb(void *p_dev, u8 delay, u8 gain);  

/*!
  Set the gain of a QE band.

  \param[in] p_dev The pointer to the audio device.
  \param[in] band The target band
  \param[in] gain The gain of the band
  */
RET_CODE aud_pp_set_eq_vsb(void *p_dev, aud_pp_eq_vsb_t band, u8 gain);  

/*!
  Open an audio output DAC

  \param[in] p_dev The pointer to the audio device.
  \param[in] is_on TRUE to enable and FALSE to disable it.
  */
RET_CODE aud_output_dac_onoff_vsb(void *p_dev, BOOL is_on);

/*!
  Open I2S for audio output

  \param[in] p_dev The pointer to the audio device.
  \param[in] is_on TRUE to enable and FALSE to disable it.
  */
RET_CODE aud_output_i2s_onoff_vsb(void *p_dev, BOOL is_on);

/*!
  Open SPDIF for audio output 

  \param[in] p_dev The pointer to the audio device.
  \param[in] is_on TRUE to enable and FALSE to disable it.
  */
RET_CODE aud_output_spdif_onoff_vsb(void *p_dev, BOOL is_on);

/*!
  Set SPDIF output format to RAW or LPCM

  \param[in] p_dev The pointer to the audio device.
  \param[in] fmt RAW or LPCM.
  */
RET_CODE aud_output_set_spdif_fmt_vsb(void *p_dev, aud_spdif_out_fmt_vsb_t fmt);

/*!
  enable SPDIF output

  \param[in] p_dev The pointer to the audio device.
  \param[in] enable or disable.
  */
RET_CODE aud_enable_2way_out_vsb(void *p_dev, BOOL enable);

/*!
  Set HDMI audio source to be I2S or SPDIF

  \param[in] p_dev The pointer to the audio device.
  \param[in] source I2S or SPDIF.
                  0:I2S
                  1:SPDIF
  */
RET_CODE aud_set_hdmi_src_vsb(void *p_dev, u8 src);

/*!
  Set SPDIF audio source to connect to PCM or SPDIF buffer

  \param[in] p_dev The pointer to the audio device.
  \param[in] source PCM or SPDIF buffer.
                    0:PCM buffer
                    1:SPDIF buffer
  */
RET_CODE aud_set_spdif_src_vsb(void *p_dev, u8 src);

/*!
  Set audio fader in/out param
  
  \param[in] p_dev      Pointer to the audio device.
  \param[in] p_param  Pointer to the param struct.
  */
RET_CODE aud_pp_set_fader_param_vsb(void *p_dev, aud_pp_fader_param_vsb_t * p_param);


/*!
  Set audio downmix param
  
  \param[in] p_dev      Pointer to the audio device.
  \param[in] p_param  Pointer to the param struct.
  */
RET_CODE aud_pp_set_downmix_param_vsb(void *p_dev, aud_pp_downmix_param_vsb_t * p_param);

/*!
  Set audio agc  param
  
  \param[in] p_dev      Pointer to the audio device.
  \param[in] p_param  Pointer to the param struct.
  */
RET_CODE aud_pp_set_agc_param_vsb(void *p_dev, aud_pp_agc_param_vsb_t * p_param);

/*!
  Get audio decoding state: for cable plug in/out , display on/off
  
  \param[in] p_dev The pointer to the audiodecoder.
  \param[out] p_state The state of audio decoding.
  */
RET_CODE aud_singal_detect_vsb(void *p_dev, aud_info_t *p_state);

/*!
  Tell audio check module if auto test is turned on or off.
  */
RET_CODE aud_set_chimera_mode_vsb(void *p_dev, u32 mode, u32 tag);

/*!
  Get info from firmware.
  tag:  aud_warriors_check_t **
  */
RET_CODE aud_get_info_vsb(void *p_dev, u32 tag);


/*!
  config audio max volume parameter :
  the range is from negative infinity to 2dbfs(not include), 0x8000 is normal for 0dbfs
  */
RET_CODE aud_set_max_volume(void *p_dev, u16 max_volume);

/*!
  get audio pts
  */
 u32  aud_get_apts(void *p_dev);

/*!
  set device handle
  */
RET_CODE audio_set_dev_handle(int audio_handle);

#ifdef __cplusplus
}
#endif
#endif


