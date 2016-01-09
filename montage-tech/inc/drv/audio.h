/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __AUDIO_H__
#define __AUDIO_H__



/*!
  AUD_OUT_CLK
  */
//#define AUD_OUT_CLK (54000)  

/*!
  AUD_OUT_CLK
  */
#define AUD_OUT_CLK (256000)   //snt chip
/*!
  AUD_OUT_CLK for concerto fpga
  */
//#define AUD_OUT_CLK_CONCERTO_FPGA (80000*1000)  
#ifdef  FPGA
/*!
  AUD_OUT_CLK for concerto fpga
  */
#define AUD_OUT_CLK_CONCERTO_FPGA (50000*1000)  
#else 
/*!
  AUD_OUT_CLK for concerto chip
  */
#define AUD_OUT_CLK_CONCERTO_FPGA (259200*1000)  
#endif
/*!
  ONE_CPY_CHI
  */
#define ONE_CPY_CHI (2048)   //must less than one frame

/*!
  DEBUG_PCM_DATA_LEN   10M
  */
#define DEBUG_PCM_DATA_LEN 0xA00000

/*!
  DEBUG_SPDIF_DATA_LEN
  */
#define DEBUG_SPDIF_DATA_LEN 8515584

/*!
  AUD_DUMP_ADDR   //220M
  */
#define AUD_DUMP_ADDR 0xDC00000  

/*!
  AUD_DUMP_ADDR   //230M
  */
#define AUD_DUMP_ADDR_2 (AUD_DUMP_ADDR + DEBUG_PCM_DATA_LEN)  

/*!
  AUD_DEBUG_PRINT_PIERI0D   
  */
#define AUD_DEBUG_PRINT_PIERI0D 0x300000

/*!
  For write pcm buffer directly  64 ints  once
  */
#define PCM_ONCE_COPY_SIZE 64

/*!
  SPRAM_ONCE_COPY_SIZE
  */
#define SPRAM_ONCE_COPY_SIZE (6 * 1024)
/*!
  PP_ONCE_COPY_SIZE
  */
#define PP_ONCE_COPY_SIZE SPRAM_ONCE_COPY_SIZE
/*!
  PP_BUF_ADDR    220 - 4   M
  */
#define PP_BUF_ADDR 0xD800000//(AUDIO_FW_CFG_ADDR + 0x20000)

/*!
  PP_BUF_SIZE
  */
#define PP_BUF_SIZE (256*1024)//0x100000
/*!
  PCM_BUF_SIZE
  */
#define PCM_BUF_SIZE PP_BUF_SIZE
/*!
  SPD_BUF_SIZE
  */
#define SPD_BUF_SIZE PP_BUF_SIZE

/*!
  mix channel buffer size in concerto
  */
#define MIX_BUF_SIZE  PP_BUF_SIZE

/*!
  for debug
  DSP calculation limit value
  */
#define DSP_DEBUG_DATA_LIMIT 0x78ffffff

/*!
  for debug  
  */
#define ISR_WHILE_NUM 222

/*!
  This structure defines audio commands by dev_io_ctrl. 
  */
typedef enum
{
  /*!
    Set the audio channel.
    */
  AUDIO_CMD_SET_CHANNEL = DEV_IOCTRL_TYPE_UNLOCK + 0,
  /*!
    Enable/disable mute state
    */
  AUDIO_CMD_SET_MUTE,
  /*!
    Set volume level
    */
  AUDIO_CMD_SET_VOLUME,
  /*!
    Get current volume level
    */
  AUDIO_CMD_GET_VOLUME,
  /*!
    Trigger of audio DAC
    */
  AUDIO_CMD_DAC_ONOFF,
  /*!
    Direct play audio ES data or PCM data.
    */
  AUDIO_CMD_PLAY_MEDIA,
  /*!
    Get current audio channel setting
    */
  AUDIO_CMD_GET_CHANNEL,
  /*!
    Reset audio PCM output and audio DAC
    */
  AUDIO_CMD_AUDIO_RESET,
  /*!
    Check PCM empty number to reset audio decoder & PCM output 
    to keep audio phase difference stable.
    */
  AUDIO_CMD_PCM_EMPTY_MONITOR,
  /*!
    Enable/disable I2s output.
    */
  AUDIO_CMD_I2S_EN,
  /*!
    DEBUG
    Config PP PCM SPDIF buffers.
    */
  AUDIO_CMD_DEBUG_CONFIG_BUFFERS,
  /*!
    DEBUG
    Config ISR.
    */
  AUDIO_CMD_DEBUG_CONFIG_ISR,
  /*!
    DEBUG
    Config ISR.
    */
  AUDIO_CMD_DEBUG_DMA,
  /*!
    DEBUG
    Init DSP.
    */
  AUDIO_CMD_DEBUG_DSP_INIT,
  /*!
    DEBUG
    DSP cal.
    */
  AUDIO_CMD_DEBUG_DSP_CAL,
  /*!
    DEBUG
    Inject data to buffer.
    */
  AUDIO_CMD_DEBUG_I2S_CHANGE,
  /*!
    DEBUG
    Inject data to buffer.
    */
  AUDIO_CMD_DEBUG_INJECT_DATA,
  /*!
    DEBUG
    Preload play mode.
    */
  AUDIO_CMD_DEBUG_PRELOAD,
  /*!
    DEBUG
    Reset buffers.
    */
  AUDIO_CMD_DEBUG_RESET_BUFFER,
  /*!
    DEBUG
    Set individual volume for hdmi or adac.
    */
  AUDIO_CMD_DEBUG_SET_INDIVIDUAL_VOLUME,
  /*!
    DEBUG
    Set play mode.
    */
  AUDIO_CMD_DEBUG_SET_PLAY_MODE,
  /*!
    DEBUG
    Set aud format.
    */
  AUDIO_CMD_DEBUG_SET_AUD_FORMAT,
  /*!
    DEBUG
    Stop PCM buffer output.
    */
  AUDIO_CMD_DEBUG_STOP_PCM,
  /*!
    DEBUG
    SET PP CHANS.
    */
  AUDIO_CMD_DEBUG_SET_CHAN,
  /*!
    DEBUG
    Set the index of sampling rate.
    */
  AUDIO_CMD_DEBUG_SET_SAMPLING_RATE_INDEX,
  /*!
    DEBUG
    Set sampling rate.
    */
  AUDIO_CMD_DEBUG_SET_SAMPLING_RATE,
  /*!
  */
  AUDIO_CMD_DEBUG_MICRO_AVSYNC,
  /*!
    DEBUG
    Enable src.
    */
  AUDIO_CMD_DEBUG_ENABLE_SRC,
  /*!
    Set param for debuging.
    */
  AUDIO_CMD_DEBUG_SET_PARAM,
  /*!
    Set param for itaf auto test.
    */
  AUDIO_CMD_DEBUG_SET_DOLBY_ITAF_ARGS,  
  /*!
    get receive pipe id.
    */
  AUDIO_CMD_DEBUG_GET_ITAF_RECEIVE_PIPE_ID,  
  /*!
    get receive udc hdl.
    */
  AUDIO_CMD_DEBUG_GET_ITAF_UDC_HDL,  
  /*!
    invalid udc hdl.
    */
  AUDIO_CMD_DEBUG_ITAF_INVALID_HDL,  
  /*!
    set udc ret hdl.
    */
  AUDIO_CMD_DEBUG_SET_ITAF_UDC_RET_HDL,  
  /*!
    get output files.
    */
  AUDIO_CMD_DEBUG_GET_ITAF_OUTPUT_FILES,
    /*!
    Set param for itaf auto test.
    */
  AUDIO_CMD_DEBUG_SET_DOLBY_PLAY_ARGS, 
  /*!
    parameters interface for mix channel config
    */
  AUDIO_CMD_MIX_PARAM,
  /*!
    parameter set of alpha conctrol of mix channel 
    */
  AUDIO_CMD_MIX_PARAM_ALPHA,
  /*!
    Set dolby downmix mode
   */
   AUDIO_CMD_DOLBY_DOWMMIX,
  /*!
   Set dolby dualnomo mode
  */
   AUDIO_CMD_DOLBY_DUALNOMO,
   /*!
   Set dolby DRC mode
   */
   AUDIO_CMD_DOLBY_DRC,
  /*!
  Set dolby DR_x: high cut dynamic range
  */
   AUDIO_CMD_DOLBY_X,
  /*!
  Set dolby DR_y: low boost dynamic range
  */
   AUDIO_CMD_DOLBY_Y,  
   /*!
    get receive pipe id.
    */
  AUDIO_CMD_GET_PIPE_ID,      /*!
    audio volume cfg for adac channel
    */
  AUDIO_CMD_SET_VOL_ADAC,
  /*!
    audio volume cfg for hdmi channel
    */
  AUDIO_CMD_SET_VOL_HDMI,
  /*!
    switch spdif channel
    */
  AUDIO_CMD_SWITCH_SPDIF_CHANNEL,

}audio_cmd_t;

/*!
  debug only
  This enum defines the mode to play audio
  */
typedef enum
{
  /*!
    No buffer
    */
  AUD_NO_BUF = 0,
  /*!
    PCM data to PP buffer
    */
  AUD_PP_BUF = 1,
  /*!
    PCM data to pcm buf ,SPDIF buffer don't work
    */
  AUD_PCM_DATA_PCM_BUF = 2,
  /*!
    SPDIF data to PCM buf,SPDIF buffer don't work
    */
  AUD_SPDIF_DATA_PCM_BUF = 3,
  /*!
    PCM data to  SPDIF buf,PCM buffer don't work
    */
  AUD_PCM_DATA_SPDIF_BUF = 4,
  /*!
    AC3 data to  SPDIF buf,PCM buffer don't work
    */
  AUD_SPDIF_DATA_SPDIF_BUF = 5,
  /*!
    PCM data to  PCM buf while PCM data to SPDIF buffer
    */
  AUD_PCM_AND_PCM = 6,
  /*!
    PCM data to  PCM buf while AC3 data to SPDIF buffer
    */
  AUD_PCM_AND_SPDIF = 7,
  //mix buffer is for concerto only
  /*!
    PCM data to  MIX buffer
    */
  AUD_MIX = 8,
  /*!
    PCM data to  PP and MIX buffer
    */
  AUD_PP_MIX = 9,
  /*!
    PCM data to  PCM and MIX buffer
    */
  AUD_PCM_MIX = 10,
  /*!
    AUD_SPDIF_PLUSE_DATA_PCM_BUF
    */
  AUD_SPDIF_PLUSE_DATA_PCM_BUF = 11,
  /*!
    spdif data in pcm buffer   spdif pluse data in spdif buffer
    */
  AUD_SPDIF_AND_SPDIF_PLUSE,
}aud_debug_play_mode_t;

/*!
  debug only
  This enum defines the buffer to inject data
  */
typedef enum
{
  /*!
    PP buffer
    */
  AUD_DEBUG_PP_BUF = 1,
  /*!
    PCM buffer
    */
  AUD_DEBUG_PCM_BUF = 2,
  /*!
    SPDIF buffer
    */
  AUD_DEBUG_SPDIF_BUF = 3,
  /*!
    MIX buffer
    */
  AUD_DEBUG_MIX_BUF = 4,
}aud_debug_inject_buffer_t;


/*!
  debug only
  This enum defines the way to input audio data
  */
typedef enum
{  
  /*!
    DMA 
    */
  AUD_DEBUG_BUF_BY_DMA = 1,
  /*!
    AV DMA 
    */
  AUD_DEBUG_BUF_BY_AV_DMA = 2,
  /*!
    CPU 
    */
  AUD_DEBUG_BUF_BY_CPU = 3,
  /*!
    HARDWARE  PP buffer only
    */
  AUD_DEBUG_BUF_BY_HARDWARE = 4,
}aud_debug_inject_mode_t;

/*!
  debug only
  This enum defines the mode to set individual volume for hdmi or adac
  */
typedef enum
{
  /*!
    set hdmi volume
    */
  AUD_VOL_SETTING_HDMI = 1,
  /*!
    set adac volume
    */
  AUD_VOL_SETTING_ADAC = 2,
  /*!
    set both volume
    */
  AUD_VOL_SETTING_BOTH = 3
}aud_debug_vol_setting_mode_t;

/*!
  debug only
  This enum defines the mode of preload buffer
  */
typedef enum
{
  /*!
    preload disabled
    */
  AUD_PRELOAD_DISABLED = 1,
  /*!
    preload to pp
    */
  AUD_PRELOAD_TO_PP = 2,
  /*!
    preload to pcm
    */
  AUD_PRELOAD_TO_PCM = 3,
  /*!
    preload to spdif
    */
  AUD_PRELOAD_TO_SPDIF = 4,
  /*!
    preload to mix
    */
  AUD_PRELOAD_TO_MIX = 5,
}aud_debug_preload_mode_t;

/*!
  debug only
  This enum defines output of I2S 
  */
typedef enum
{
  /*!
    output left and right
    */
  AUD_DEBUG_I2S_LEFT_RIGHT = 0,
  /*!
    output left and right
    */
  AUD_DEBUG_I2S_LFE_CENTER = 1,
  /*!
    output left and right
    */
  AUD_DEBUG_I2S_SL_SR = 2,
  /*!
    output left and right
    */
  AUD_DEBUG_I2S_RSL_RSR = 3,
  /*!
    output left and right
    */
  AUD_DEBUG_I2S_DOWNMIX = 4,
  /*!
    output left and right
    */
  AUD_DEBUG_I2S_AGC = 5,
}aud_debug_i2s_out_select_t;


/*!
  debug only
  This structure defines the info of audio inputing data
  */
typedef struct
{
  /*!
    The channel to input data
    */
  u8 channel;
  /*!
    reserved
    */
  u8 reserved[3];
  /*!
    The address to input data
    */
  u32 addr;
  /*!
    The length of data
    */
  u32 len;

  /*!
    The address to input data SPD
    */
  u32 addr_spd;
  /*!
    The length of data SPD
    */
  u32 len_spd;

  /*!
    The length of data really copied(Do not wait when buffer is full)
    */
  u32 copied;
  /*!
    The way to input data
    */
  aud_debug_inject_mode_t  inject_mode;
  /*!
    The buffer to input data
    */
  aud_debug_inject_buffer_t  inject_buf;
  /*!
    customer data
    */
  u32  tag;
  /*!
    preload mode 0:disable  1: pp  2:pcm 3:spdif 4:mix
    */
  u8 preload_flag;
} aud_debug_info_t;

/*!
  debug only
  This enum defines the mode of DMA
  */
typedef enum
{
  /*!
    ddr to spram
    */
  AUD_DMA_DDR_TO_SPRAM = 1,
  /*!
    spram to ddr
    */
  AUD_DMA_SPRAM_TO_DDR = 2,
  /*!
    ddr to ddr
    */
  AUD_DMA_DDR_TO_DDR = 3,
  /*!
    ddr to ddr by av dma
    */
  AUD_DMA_DDR_TO_DDR_BY_AV_DMA = 4,
  /*!
    ddr to ddr by av dma
    */
  AUD_DMA_DDR_TO_DDR_BY_AV_DMA_WAIT = 5,
  /*!
    ddr to audio out
    */
  AUD_DMA_DDR_TO_AUD_OUT = 6
}aud_debug_dma_mode_t;

/*!
  debug only
  This structure defines the param for DMA test
  */
typedef struct
{
  /*!
    Dest addr
    */
  u32 dest_addr;
  /*!
    Source addr
    */
  u32 source_addr;
  /*!
    Copy length
    */
  u32 dma_len;
  /*!
    SPRAM offset
    */
  u32 dma_off;
  /*!
    chan number
    */
  u32 aud_out_chan;
  /*!
    The mode of dma
    */
  aud_debug_dma_mode_t  dma_mode;
} aud_debug_dma_param_t;


/*!
  debug only
  This structure defines the param for DSP test
  */
typedef enum
{
  /*! 
    real operand add
    */
  AUD_DEBUG_DSP_REAL_ADD = 1,
  /*! 
    real operand add
    */
  AUD_DEBUG_DSP_REAL_ACC = 2,
  /*! 
    complex operand acc
    */
  AUD_DEBUG_DSP_COMPLEX_ADD = 3,
  /*! 
    complex operand acc
    */
  AUD_DEBUG_DSP_COMPLEX_ACC = 4
}aud_debug_dsp_mode_t;
/*!
  debug only
  This structure defines the param for DSP test
  */
typedef struct
{
  /*!
    DSP mode
    */
  aud_debug_dsp_mode_t dsp_mode;

  /*!
    coef addr
    */
  u32 coef_addr;

  /*!
    coef len
    */
  u32 coef_len;

  /*!
    data addr
    */
  u32 data_addr;

  /*!
    data len
    */
  u32 data_len;
  
  /*!
    0:dsp 0
    other:dsp 1
    */
  u8 dsp_device;  
} aud_debug_dsp_param_t;

/*!
  debug only
  This structure defines the calculation param for DSP test
  */
typedef struct
{
  /*!
    input a real part
    */
  int a_real;

  /*!
    input a image part
    */
  int a_image;

  /*!
    input b real part
    */
  int b_real;

  /*!
    input b image part
    */
  int b_image;

  /*!
    input c real part
    */
  int c_real;

  /*!
    input a image part
    */
  int c_image;

  /*!
    output a real part low
    */
  u32 out_a_real_low;

  /*!
    output a real part high
    */
  int out_a_real_high;

  /*!
    output a image part low
    */
  u32 out_a_image_low;

  /*!
    output a image part high
    */
  int out_a_image_high;

  /*!
    output sub real part low
    */
  u32 out_sub_real_low;

  /*!
    output sub real part high
    */
  int out_sub_real_high;

  /*!
    output sub image part low
    */
  u32 out_sub_image_low;

  /*!
    output sub image part high
    */
  int out_sub_image_high;

  /*!
    for acc data start addr in spram
    */
  u32 data_start;

  /*!
    for acc data jump step
    */
  u32 data_step;

  /*!
    for acc data jump direction  TRUE: positive  FALSE:negative
    */
  BOOL data_direction;

  /*!
    for acc coef start addr in spram
    */
  u32 coef_start;

  /*!
    for acc coef jump step
    */
  u32 coef_step;

  /*!
    for acc coef jump direction  TRUE: positive  FALSE:negative
    */
  BOOL coef_direction;  

  /*!
    for acc    mul count
    */
  u32 mulcnt;

  /*!
    DSP mode
    */
  aud_debug_dsp_mode_t dsp_mode;
  /*!
    0:dsp 0
    other:dsp 1
    */
  u8 dsp_device;
} aud_debug_dsp_cal_t;


/*!
  This structure defines the param for audio cfg
  */
typedef struct
{
  /*!
    PP base
    */
  u32 PP_base;
  /*!
    PP len
    */
  u32 PP_len;  
 /*!
    PCM base
    */
  u32 PCM_base;
  /*!
    PCM len
    */
  u32 PCM_len;  
  /*!
    SPDIF base
    */
  u32 SPDIF_base;
  /*!
    SPDIF len
    */
  u32 SPDIF_len; 
  /*!
    mix buffer addr
    */
  u32 mix_base;  
  /*!
    mix buffer length
    */
  u32 mix_len;  
} aud_debug_param_t;

/*!
  This structure defines the param for itaf auto test in dolby
  */
typedef struct
{
  /*!
    argc
    */
  u32 argc;
  /*!
    argv
    */
  u32 argv;  
} aud_debug_dec_param_t;//aud_debug_itaf_param_t;

/*!
  This structure defines the param for itaf auto test in dolby
  */
typedef struct
{
  /*!
    priority
    */
  u32 task_prio;
  /*!
    size
    */
  u32 stack_size;  
} aud_debug_itaf_filetask_t;

/*!
  This structure defines the supported PCM sample rates.
  */
typedef enum
{
  /*! 
    Sample rate is 48k 
    */
  AUDIO_SAMPLE_48 = 5,
  /*! 
    Sample rate is 44.1k 
    */
  AUDIO_SAMPLE_44 = 4,
  /*! 
    Sample rate is 32k
    */
  AUDIO_SAMPLE_32 = 6,
  /*! 
    Sample rate is 24k 
    */
  AUDIO_SAMPLE_24 = 1,
  /*!
    Sample rate is 22.05k 
    */
  AUDIO_SAMPLE_22 = 0,
  /*!
    Sample rate is 16k 
    */
  AUDIO_SAMPLE_16 = 2
} audio_sample_rate_t;

/*!
  This structure defines the supported audio channel state.
  */
typedef enum
{
  /*! 
    Stereo channel 
    */
  AUDIO_CHANNEL_STEREO,
  /*! 
    Left channel 
    */
  AUDIO_CHANNEL_LEFT,
  /*!
    Right channel 
    */
  AUDIO_CHANNEL_RIGHT,
  /*!
    Mono channel
    */
  AUDIO_CHANNEL_MONO,
  /*! 
    Both channels are closed. 
    */
  AUDIO_CHANNEL_CLOSED
} audio_channel_t;

/*!
  This structure defines the supported audio compression standards.
  */
typedef enum
{
  /*!
    MPEG 1/2 layer I/II
    */
  AUDIO_MPEG,
  /*!
    AC 3
    */
  AUDIO_AC3
}audio_type_t;


/*!
  Parameters for direct play audio ES data or PCM data.
  */
typedef struct
{
  /*!
    If the input audio data is PCM. TRUE for PCM and FALSE for MP1/2 ES data.
    */
  BOOL is_pcm;
  /*! 
    Endian of the PCM data. TRUE for little endian and FALSE for big endian 
    */
  BOOL is_little_endian;
  /*!
    Audio channel setting for the PCM data: Stereo or Mono 
    */
  BOOL is_stereo;
  /*! 
    The sample rate for the input PCM data 
    */
  audio_sample_rate_t sample;
  /*! 
    The start address of the input audio data
    */
  u8 *p_data;
  /*! 
    The size of the input audio data 
    */
  u32 size;
} audio_media_param_t;

/*!
  Initialization parameters for a audio device.
  */
typedef struct 
{
  /*! 
    configure the pinmux port
    true: config pin for audio module 
    false: config pin for extern para ts port
    */
  BOOL pinmux_flag;
}audio_config_t;

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
}audio_device_t;

/*!
  define it for dolby system certification
  */
//#define DOLBY_SYS_CERTIF

/*!
   dolby downmix mode
  */
typedef enum {
  /*!
     dolby downmix mode: automatic
    */
  AUTOMATIC_DOWNMIX = 0,
  /*!
     dolby downmix mode: LtRt
    */
  Lt_Rt_DOWNMIX,
  /*!
     dolby downmix mode: LoRo
    */
  Lo_Ro_DOWNMIX
}DOWN_MIX_MODE;

/*!
   dolby dual mono mode
  */
typedef enum {
  /*!
   dolby dual mono mode: stereo
   */
  STEREO_DUALMONO = 0,
  /*!
   dolby dual mono mode: Ch1
   */
  CH1_DUALMONO,
  /*!
   dolby dual mono mode: Ch2
   */
  CH2_DUALMONO,
  /*!
   dolby dual mono mode: mixed
   */
  MIXED_DULAMONO    
}DUAL_MONO_MODE;

/*!
   dolby drc mode
  */
typedef enum {
  /*!
   dolby drc mode : line mode
  */
  DRC_LINE_MODE = 0,
  /*!
   dolby drc mode : RF mode
  */
  DRC_RF_MODE
}DRC_MODE;

#endif //__AUDIO_H__

