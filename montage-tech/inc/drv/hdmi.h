/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HDMI_H__
#define __HDMI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  the macro to indicate the command is used for setting
  */
#define HDMI_CMD_SET 0x80

/*!
  the macro to indicate the command is used for getting
  */
#define HDMI_CMD_GET 0x00

/*!
  config video
  */
#define  HDMI_VIDEO_CONFIG  0x80000000
/*!
  config audio
  */
#define  HDMI_AUDIO_CONFIG  0x40000000
/*!
  control video
  */
#define  HDMI_VIDEO_CONTROL 0x20000000
/*!
  control audio
  */
#define  HDMI_AUDIO_CONTROL 0x10000000

/*!
  HDMI native format max number
  */
#define HDMI_NATIVE_FORMAT_MAX_NUM 4

/*!
  HDMI audio format max number
  */
#define HDMI_AUDIO_FORMAT_MAX_NUM 10

/*!
  HDMI audio channel max number
  */
#define HDMI_AUDIO_CHANNEL_MAX_NUM 8

/*!
  HDMI CEC message length
  */
#define HDMI_CEC_MESSAGE_LEN        16

/*!
  HDMI io control command
  */
typedef enum
{
  /*!
    power control, parameter is TRUE means power down or FALSE means power up
    */
  HDMI_POWER_DOWN,
  /*!
    data output control, parameter is TRUE means enable output or FALSE means disable
    */
  HDMI_OUTPUT_ENABLE,
  /*!
    setting Source Product Description infoframe, for parameter please see hdmi_spd_info_t
    */
  HDMI_SPD_INFO_SET,
  /*!
    setting phy pll configuration, for parameter please see hdmi_phy_pll_t
    */
  HDMI_FHY_PLL_SET,
  /*!
    getting phy pll configuration, for parameter please see hdmi_phy_pll_t
    */
  HDMI_FHY_PLL_GET,
  /*!
    getting EDID information, for parameter please see hdmi_edid_result_t
    */
  HDMI_EDID_GET,
  /*!
    copy right protection enable/disable, param is TRUE means enable, FALSE means disable
    */
  HDMI_HDCP_CP_ENABLE,
  /*!
    get hdcp status, for parameter please see hdmi_hdcp_auth_status_t
    */
  HDMI_HDCP_STATUS_GET,
  /*!
   dump register vaule
    */
  HDMI_REGISTER_VALUE_DUMP,
  /*!
    set mute status, parameter is TRUE means mute output or FALSE means unmute
    */
  HDMI_AV_MUTE,
  /*!
    notify register, for parameter please see hdmi_notify_info_t
    */
  HDMI_NOTIFY_REGISTER,
  /*!
    notify register, for parameter please see hdmi_event_id_t
    */
  HDMI_NOTIFY_UNREGISTER,
  /*!
    notify register, for parameter please see hdmi_event_id_t
    */
  HDMI_NOTIFY_GET_EVENTS,
  /*!
    open config
    */
  HDMI_OPEN_CONFIG,
  /*!
    get HDMI sink status, 1 sink connected, 0 not connected
    */
  HDMI_IS_SINK_CONNECTED,
  /*!
    get HDMI audio source
    */
  HDMI_GET_AUD_SOURCE,
   /*!
    create and start HDMI main service thread
    */
  HDMI_START_MAIN_SERVICE
}hdmi_io_cmd_t;

/*!
  HDMI input video format
  */
typedef enum
{
  /*!
    RGB and YcbCr 4:4:4 Formats with Separate Syncs (24-bpp mode)
    */
  RGB_YCBCR444_SYNCS = 0,
  /*!
    YCbCr 4:2:2 Formats with Separate Syncs(16-bbp)
    */
  YCBCR422_SYNCS,
  /*!
    YCbCr 4:2:2 Formats with Embedded Syncs(No HS/VS/DE)
    */
  YCBCR422_EMBED_SYNCS,
  /*!
    YC Mux 4:2:2 Formats with Separate Sync Mode1(bit15:8 and bit 3:0 are used)
    */
  YCMUX422_SYNCS_MODE1,
  /*!
    YC Mux 4:2:2 Formats with Separate Sync Mode2(bit11:0 are used)
    */
  YCMUX422_SYNCS_MODE2,
  /*!
    YC Mux 4:2:2 Formats with Separate Sync Mode3
    */
  YCMUX422_SYNCS_MODE3,
  /*!
    YC Mux 4:2:2 Formats with Embedded Sync Mode1(bit15:8 and bit 3:0 are used)
    */
  YCMUX422_EMBED_SYNCS_MODE1,
  /*!
    YC Mux 4:2:2 Formats with Embedded Sync Mode2(bit11:0 are used)
    */
  YCMUX422_EMBED_SYNCS_MODE2,
  /*!
    YC Mux 4:2:2 Formats with Embedded Sync Mode3
    */
  YCMUX422_EMBED_SYNCS_MODE3,
  /*!
    RGB and YcbCr 4:4:4 DDR Formats with Separate Syncs
    */
  RGB_YCBCR444_DDR_SYNCS,
  /*!
    RGB and YcbCr 4:4:4 DDR Formats with Embedded Syncs
    */
  RGB_YCBCR444_DDR_EMBED_SYNCS,
  /*!
    RGB and YcbCr 4:4:4 Formats with Separate Syncs but no DE
    */
  RGB_YCBCR444_SYNCS_NO_DE,
  /*!
    YCbCr 4:2:2 Formats with Separate Syncs but no DE
    */
  YCBCR422_SYNCS_NO_DE,
}hdmi_input_video_format_t;

/*!
  select input pixel clock edge for DDR mode
  */
typedef enum
{
  /*!
    in DDR mode, use rising edge to latch even numbered pixel data
    in SDR mode, use rising edge of latch the pixel data
    */
  DDR_EDGE_RISING = 0,
  /*!
    in DDR mode, use falling edge to latch even numbered pixel data
    in SDR mode, use falling edge to latch the pixel data
    */
  DDR_EDGE_FALLING,
}hdmi_input_video_ddr_edge_t;

/*!
  select input pixel repeate times
  */
typedef enum
{
  /*!
    pixel repeate 1 times
    */
  PIXEL_RPT_1_TIMES = 1,
  /*!
    pixel repeate 2 times
    */
  PIXEL_RPT_2_TIMES = 2,
  /*!
    pixel repeate 4 times
    */
  PIXEL_RPT_4_TIMES = 4,
}hdmi_input_video_pixel_rpt_t;

/*!
  HDMI audio down sample selection
  */
typedef enum
{
  /*!
    no down sample
    */
  AUDIO_NO_DOWN_SAMPLE = 0,
  /*!
    2 to 1 down sample
    */
  AUDIO_2_TO_1_DOWN_SAMPLE,
  /*!
    4 to 1 down sample
    */
  AUDIO_4_TO_1_DOWN_SAMPLE,
}hdmi_audio_down_sample_t;

/*!
  HDMI one bit super audio configuration
  */
typedef struct
{
  /*!
    one bit control, SEN0 ~ SEN7
    */
  u8 one_bit_ctrl;
}super_audio_cfg_t;

/*!
  hdmi phy pll parameters
  */
typedef struct
{
  /*!
    fmod
    */
  u32 fmod;
  /*!
    mdiv
    */
  u32 mdiv;
  /*!
    ndiv
    */
  u32 ndiv;
  /*!
    adiv
    */
  u32 adiv;
  /*!
    fdiv
    */
  u32 fdiv;
  /*!
    vcodivsel
    */
  u32 vcodivsel;
}hdmi_phy_pll_t;


/*!
  HDMI driver configuration
  */
typedef struct
{
  /*!
    if use the public sharing driver service, set the handle here 
    */
  void *p_drvsvc; 
  /*!
    if not use the public sharing driver service, set this task priority
    */
  u32 task_prio;
  /*!
    if not use the public sharing driver service, set this task statck size
    */
  u32 stack_size;
  /*!
    i2c master handle used by hdmi driver
    */
  void *i2c_master;
  /*!
    is initialized before
    */
  u32 is_initialized;
  /*!
        The locking mode of function call, see dev_lock_mode
      */
  u32 lock_mode;
  /*!
        The chip mode, see chip_package_id_t
      */
  u32 chip_mode;
  /*!
        is fastlogo mode
      */
  u32 is_fastlogo_mode;
}hdmi_cfg_t;

/*!
  HDMI video color space
  */
typedef enum
{
  /*!
    RGB
    */
  HDMI_COLOR_RGB  = 0,
  /*!
    YUV422
    */
  HDMI_COLOR_YUV422,
  /*!
    YUV444
    */
  HDMI_COLOR_YUV444,
  /*!
    auto choose color space
    */
  HDMI_COLOR_AUTO,
}hdmi_video_color_space_t;

/*!
  HDMI display output standard.
  */
typedef enum
{
  /*!
    default setting
    */
  HDMI_OUTPUT_STD_DEF,
  /*!
    NTSC, 60Hz
    */
  HDMI_NTSC,
  /*!
    PAL, 50Hz
    */
  HDMI_PAL,
  /*!
    24 frames per second
    */
  HDMI_24FPS,
}hdmi_disp_output_standard_t;

/*!
  HDMI display output resolution.
  */
typedef enum
{
  /*!
    default resolution
    */
  HDMI_OUTPUT_RESOLUTION_DEF,
  /*!
    720 * 480i
    */
  HDMI_480I,
  /*!
    720 * 576i
    */
  HDMI_576I,
  /*!
    720 or 640 * 480p
    */
  HDMI_480P,
  /*!
    720 * 576p
    */
  HDMI_576P,
  /*!
    1280 * 720p
    */
  HDMI_720P,
  /*!
    1920 * 1080i
    */
  HDMI_1080I,
  /*!
    1920 * 1080p
    */
  HDMI_1080P,
}hdmi_disp_output_resolution_t;

/*!
  HDMI display output shape.
  */
typedef enum
{
  /*!
    default setting
    */
  HDMI_SHAPE_DEF,
  /*!
    4 X 3
    */
  HDMI_SHAPE_4X3,
  /*!
    16 X 9
    */
  HDMI_SHAPE_16X9,
}hdmi_disp_output_shape_t;

/*!
  hdmi input video configuration
  */
typedef struct
{
  /*!
    input video format, see hdmi_input_video_format_t
    */
  hdmi_input_video_format_t fmt;
  /*!
    input video color space, see hdmi_video_color_space_t
    */
  hdmi_video_color_space_t csc;
  /*!
    input video pixel clock edge, see hdmi_input_video_ddr_edge_t
    */
  hdmi_input_video_ddr_edge_t ddr_edge;
  /*!
    input video pixel repeate times, see hdmi_input_video_pixel_rpt_t
    */
  hdmi_input_video_pixel_rpt_t pixel_rpt;
}hdmi_input_video_cfg_t;

/*!
  HDMI output video config.
  */
typedef struct
{
  /*!
    output color space
    */
  hdmi_video_color_space_t color_space;
  /*!
    display standard
    */
  hdmi_disp_output_standard_t standard;
  /*!
    display resolution
    */
  hdmi_disp_output_resolution_t resolution;
  /*!
    display shape
    */
  hdmi_disp_output_shape_t shape;
}hdmi_output_video_cfg_t;

/*!
  HDMI video configuration used by calling hdmi_video_config
  */
typedef struct
{
  /*!
    input video configuration
    */
  hdmi_input_video_cfg_t input_v_cfg;
  /*!
    output video configuration
    */
  hdmi_output_video_cfg_t output_v_cfg;
  /*!
    0: HDCP off, 1: HDCP on
    */
  u32 hdcp_on_off;
}hdmi_video_config_t;

/*!
  HDMI video picture scalling
  */
typedef enum
{
  /*!
    no scaling
    */
  HDMI_NO_SCALING,
  /*!
    horizontal scaling
    */
  HDMI_SCALING_HOR,
  /*!
    vertical scaling
    */
  HDMI_SCALING_VER,
  /*!
    horizontal & vertical scaling
    */
  HDMI_SCALING_HOR_VER,
}hdmi_video_pic_scaling_t;

/*!
  HDMI video scan mode
  */
typedef enum
{
  /*!
    scan no data
    */
  HDMI_SCAN_NO_DATA,
  /*!
    over scanned
    */
  HDMI_SCAN_OVERSCANNED,
  /*!
    under scanned
    */
  HDMI_SCAN_UNDERSCANNED
}hdmi_video_scan_mode_t;

/*!
  HDMI video bar info mode
  */
typedef enum
{
  /*!
    bar info invalid
    */
  HDMI_BARINFO_INVALID,
  /*!
    bar info vertical valid
    */
  HDMI_BARINFO_VER_VALID,
  /*!
    bar info horizontal valid
    */
  HDMI_BARINFO_HOR_VALID,
  /*!
    bar info horizontal & vertical valid
    */
  HDMI_BARINFO_VER_AND_HOR_VALID
}hdmi_video_bar_info_t;

/*!
  HDMI bar info parameter
  */
typedef struct
{
  /*!
    bar info mode
    */
  hdmi_video_bar_info_t bar_info;
  /*!
    bar rectangle
    */
  rect_t bar_rect;
}hdmi_bar_info_param_t;

/*!
  HDMI video active format description
  */
typedef enum
{
  /*!
    same as original picture
    */
  HDMI_AFD_SAME_AS_PICTURE,
  /*!
    4 X 3 center
    */
  HDMI_AFD_4_3_CENTER,
  /*!
    16 X 9 center
    */
  HDMI_AFD_16_9_CENTER,
  /*!
    14 X 9 center
    */
  HDMI_AFD_14_9_CENTER
}hdmi_video_afd_t;

/*!
  HDMI video colorimetry standard
  */
typedef enum
{
  /*!
    None colorimetry
    */
  HDMI_COLORIMETRY_NONE,
  /*!
    SMPTE 170M colorimetry
    */
  HDMI_COLORIMETRY_SMPTE_170M = 0x01,
  /*!
    ITU 709 colorimetry
    */
  HDMI_COLORIMETRY_ITU_709 = 0x02,
  /*!
    XVYCC 601 colorimetry
    */
  HDMI_COLORIMETRY_XVYCC_601 = 0x04,
  /*!
    XVYCC 709 colorimetry
    */
  HDMI_COLORIMETRY_XVYCC_709 = 0x08,
  /*!
    SYCC 601 colorimetry
    */
  HDMI_COLORIMETRY_SYCC_601 = 0x10,
  /*!
    ADOBEYCC 601 colorimetry
    */
  HDMI_COLORIMETRY_ADOBEYCC_601 = 0x20,
  /*!
    ADOBERGB colorimetry
    */
  HDMI_COLORIMETRY_ADOBERGB = 0x40,
}hdmi_video_colorimetry_t;

/*!
  HDMI video colorimetry format for XVYCC standard
  */
typedef enum
{
  /*!
    vertices facets
    */
  HDMI_XVYCC_VERTICES_FACETS,
  /*!
    packet ranges
    */
  HDMI_XVYCC_PACKET_RANGES
}hdmi_colorimetry_xvycc_format_t;

/*!
  HDMI video colorimetry parameter for veritces facets
  */
typedef struct
{
  /*!
    gbd color space
    */
  u8 gbdColorSpace;
  /*!
    gbd color precision
    */
  u8 gbdColorPrecision;
  /*!
    facet mode
    */
  u8 facetMode;
  /*!
    nam vertices
    */
  u16 namVertices;
  /*!
    vertices data
    */
  u16 verticesData[4];
  /*!
    nam facets
    */
  u16 namFacets;
  /*!
    facets data
    */
  u16 facetsData[4];
}hdmi_xvycc_vertices_facets_info_t;

/*!
  HDMI video colorimetry parameter for packet ranges
  */
typedef struct
{
  /*!
    format flag
    */
  u16 formatFlag;
  /*!
    gbd color space
    */
  u16 gbdColorSpace;
  /*!
    gbd color precision
    */
  u16 gbdColorPrecision;
  /*!
    minimal red data
    */
  u16 minRedData;
  /*!
    maximal red data
    */
  u16 maxRedData;
  /*!
    minimal green data
    */
  u16 minGreenData;
  /*!
    maximal green data
    */
  u16 maxGreenData;
  /*!
    minimal blue data
    */
  u16 minBlueData;
  /*!
    maximal blue data
    */
  u16 maxBlueData;
}hdmi_xvycc_packet_ranges_t;

/*!
  HDMI video xvycc info
  */
typedef struct
{
  /*!
    HDMI video colorimetry format for XVYCC standard
    */
  hdmi_colorimetry_xvycc_format_t format;
  /*!
    HDMI video colorimetry parameter for veritces facets
    */
  hdmi_xvycc_vertices_facets_info_t *p_veritces_facets;
  /*!
    HDMI video colorimetry parameter for packet ranges
    */
  hdmi_xvycc_packet_ranges_t *p_packet_ranges;
}hdmi_video_xvycc_info_t;

/*!
  HDMI video colorimetry parameter
  */
typedef struct
{
  /*!
    HDMI video colorimetry standard
    */
  hdmi_video_colorimetry_t colorimetry;
  /*!
    HDMI video xvycc info, only for HDMI_COLORIMETRY_XVYCC_601 or HDMI_COLORIMETRY_XVYCC_709
    */
  hdmi_video_xvycc_info_t *p_xvycc_info;
}hdmi_video_colorimetry_param_t;

/*!
  HDMI video control commands used by calling hdmi_video_ctrl
  */
typedef enum
{
  /*!
    set picture scaling, for parameter, see hdmi_video_pic_scaling_t
    */
  HDMI_PIC_SCALING_SET = HDMI_CMD_SET,
  /*!
    set picture scaling, for parameter, see hdmi_video_pic_scaling_t
    */
  HDMI_SCAN_MODE_SET, //hdmi_video_scan_mode_t
  /*!
    set bar info, for parameter, see hdmi_bar_info_param_t
    */
  HDMI_BAR_INFO_SET,
  /*!
    set active format ratio, for parameter, see hdmi_video_afd_t
    */
  HDMI_AFD_SET,
  /*!
    set colorimetry, for parameter, see hdmi_video_colorimetry_param_t
    */
  HDMI_COLORIMETRY_SET,

  /*!
    get current color spece setting, for parameter, see hdmi_video_color_space_t
    */
  HDMI_COLOR_SPACE_GET = HDMI_CMD_GET,
  /*!
    get colorimetry capability, for parameter, see hdmi_colorimetry_cap_t
    */
  HDMI_COLORIMETRY_CAPABILITY_GET,
  /*!
    get native format, for parameter, see hdmi_native_format_t
    */
  HDMI_NATIVE_FORMAT_GET,
  /*!
    get lip sync latency, for parameter, see hdmi_lip_sync_latency_t
    */
  HDMI_LIP_SYNC_LATENCY_GET,
  /*!
    getting EDID information, for parameter please see hdmi_edid_result_t
    */
  HDMI_EDID_INFO_GET
}hdmi_video_cmd_t;

/*!
  HDMI gbd profile
  */
typedef enum
{
  /*!
    HDMI gbd profile none
    */
  HDMI_VIDEO_GBD_PROFILE_NONE   = 0x0,
  /*!
    HDMI gbd profile 0
    */
  HDMI_VIDEO_GBD_PROFILE_P0   = 0x1,
  /*!
    HDMI gbd profile 1
    */
  HDMI_VIDEO_GBD_PROFILE_P1   = 0x2,
  /*!
    HDMI gbd profile 2
    */
  HDMI_VIDEO_GBD_PROFILE_P2   = 0x4,
  /*!
    HDMI gbd profile 3
    */
  HDMI_VIDEO_GBD_PROFILE_P3   = 0x8
}hdmi_gbd_profile_t;

/*!
  HDMI colorimetry capability
  */
typedef struct
{
  /*!
    colorimetry type, see hdmi_video_colorimetry_t
    */
  u8 colorimetry;
  /*!
    HDMI gbd profile
    */
  hdmi_gbd_profile_t gbd_profiles;
}hdmi_colorimetry_cap_t;

/*!
  HDMI native format
  */
typedef struct
{
  /*!
    the flag to indicate if the device has native format
    */
  u8 has;
  /*!
    HDMI video output display standard, PAL or NTSC ...
    */
  hdmi_disp_output_standard_t video_standard;
  /*!
    HDMI video output resolution.
    */
  hdmi_disp_output_resolution_t video_resolution;
  /*!
    HDMI video active format ratio.
    */
  hdmi_video_afd_t video_afd;
}hdmi_native_format_t;

/*!
  HDMI lip sync latency
  */
typedef struct
{
  /*!
    progressive scan mode video latency
    */
  u16 prog_video_latency;
  /*!
    progressive scan mode audio latency
    */
  u16 prog_audio_latency;
  /*!
    interlaced scan mode video latency
    */
  u16 intr_video_latency;
  /*!
    interlaced scan mode audio latency
    */
  u16 intr_audio_latency;
}hdmi_lip_sync_latency_t;

/*!
  HDMI audio format
  */
typedef enum
{
  /*!
    audio format: LPCM
    */
  HDMI_AUDIO_FORMAT_LPCM = 0x01,
  /*!
    audio format: AC3
    */
  HDMI_AUDIO_FORMAT_AC3,
  /*!
    audio format: MPEG1
    */
  HDMI_AUDIO_FORMAT_MPEG1,
  /*!
    audio format: MP3
    */
  HDMI_AUDIO_FORMAT_MP3,
  /*!
    audio format: MPEG2
    */
  HDMI_AUDIO_FORMAT_MPEG2,
  /*!
    audio format: AAC
    */
  HDMI_AUDIO_FORMAT_AAC,
  /*!
    audio format: DTS
    */
  HDMI_AUDIO_FORMAT_DTS,
  /*!
    audio format: ATRAC
    */
  HDMI_AUDIO_FORMAT_ATRAC,
  /*!
    audio format: One Bit Super Audio
    */
  HDMI_AUDIO_FORMAT_ONE_BIT,
  /*!
    audio format: Dolby Digital
    */
  HDMI_AUDIO_FORMAT_DOLBY_DIGITAL,
  /*!
    audio format: DTS HD
    */
  HDMI_AUDIO_FORMAT_DTS_HD,
  /*!
    audio format: MLP
    */
  HDMI_AUDIO_FORMAT_MLP,
  /*!
    audio format: DST
    */
  HDMI_AUDIO_FORMAT_DST,
  /*!
    audio format: WMA PRO
    */
  HDMI_AUDIO_FORMAT_WMA_PRO,
}hdmi_audio_format_t;

/*!
  HDMI audio sample size
  */
typedef enum
{
  /*!
    default audio sample size
    */
  HDMI_AUDIO_SAMPLE_SIZE_DEF    = 0x00,
  /*!
    audio sample size: 16 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_16,
  /*!
    audio sample size: 17 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_17,
  /*!
    audio sample size: 18 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_18,
  /*!
    audio sample size: 19 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_19,
  /*!
    audio sample size: 20 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_20,
  /*!
    audio sample size: 21 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_21,
  /*!
    audio sample size: 22 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_22,
  /*!
    audio sample size: 23 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_23,
  /*!
    audio sample size: 24 bits
    */
  HDMI_AUDIO_SAMPLE_SIZE_24,
}hdmi_audio_sample_size_t;

/*!
  HDMI audio sample rate
  */
typedef enum
{
  /*!
    default audio sample rate
    */
  HDMI_AUDIO_SR_DEF = 0x00,
  /*!
    default audio sample rate: 32 KHz
    */
  HDMI_AUDIO_SR_32,
  /*!
    default audio sample rate: 44.1 KHz
    */
  HDMI_AUDIO_SR_441,
  /*!
    default audio sample rate: 48 KHz
    */
  HDMI_AUDIO_SR_48,
  /*!
    default audio sample rate: 88.2 KHz
    */
  HDMI_AUDIO_SR_882,
  /*!
    default audio sample rate: 96 KHz
    */
  HDMI_AUDIO_SR_96,
  /*!
    default audio sample rate: 17.64 KHz
    */
  HDMI_AUDIO_SR_1764,
  /*!
    default audio sample rate: 192 KHz
    */
  HDMI_AUDIO_SR_192,
}hdmi_audio_sample_rate_t;

/*!
  HDMI audio sample parameter
  */
typedef struct
{
  /*!
    HDMI audio sample size
    */
  hdmi_audio_sample_size_t word_size;
  /*!
    HDMI audio sample rate
    */
  hdmi_audio_sample_rate_t rate;
}hdmi_audio_sample_param_t;

/*!
  HDMI audio stream type
  */
typedef enum
{
  /*!
    default audio type
    */
  HDMI_AUDIO_DEF,
  /*!
    audio type: spdif
    */
  HDMI_AUDIO_SPDIF = 0x01,
  /*!
    audio type: i2s
    */
  HDMI_AUDIO_I2S = 0x02,
  /*!
    audio type: one bit supper audio
    */
  HDMI_AUDIO_ONE_BIT = 0x04,
}hdmi_audio_source_t;

/*!
  HDMI audio channel count
  */
typedef enum
{
  /*!
    none audio channel count
    */
  HDMI_AUDIO_CHANNELS_NONE,
  /*!
    2 audio channel count
    */
  HDMI_AUDIO_2_CHANNELS,
  /*!
    3 audio channel count
    */
  HDMI_AUDIO_3_CHANNELS,
  /*!
    4 audio channel count
    */
  HDMI_AUDIO_4_CHANNELS,
  /*!
    5 audio channel count
    */
  HDMI_AUDIO_5_CHANNELS,
  /*!
    6 audio channel count
    */
  HDMI_AUDIO_6_CHANNELS,
  /*!
    7 audio channel count
    */
  HDMI_AUDIO_7_CHANNELS,
  /*!
    8 audio channel count
    */
  HDMI_AUDIO_8_CHANNELS,
}hdmi_audio_channel_cnt_t;

/*!
  HDMI audio speaker place
  */
typedef enum
{
  /*!
    none audio speaker
    */
  HDMI_AUDIO_SPEAKER_NONE       = 0x000,
  /*!
    audio speaker, Front Left & Front Right
    */
  HDMI_AUDIO_SPEAKER_FL_FR    = 0x001,
  /*!
    audio speaker, Low Frequency Effect
    */
  HDMI_AUDIO_SPEAKER_LFE    = 0x002,
  /*!
    audio speaker, Front Center
    */
  HDMI_AUDIO_SPEAKER_FC     = 0x004,
  /*!
    audio speaker, Rear Left & Rear Right
    */
  HDMI_AUDIO_SPEAKER_RL_RR      = 0x008,
  /*!
    audio speaker, Rear Center
    */
  HDMI_AUDIO_SPEAKER_RC       = 0x010,
  /*!
    audio speaker, Front Left Center & Front Right Center
    */
  HDMI_AUDIO_SPEAKER_FLC_FRC        = 0x020,
  /*!
    audio speaker, Rear Left Center & Rear Right Center
    */
  HDMI_AUDIO_SPEAKER_RLC_RRC  = 0x040,
}hdmi_audio_speaker_place_t;


/*!
  HDMI audio channel setting
  */
typedef struct
{
  /*!
    audio channel count
    */
  hdmi_audio_channel_cnt_t channel_cnt;
  /*!
    the flag to indicate if need to be setting speaker place
    */
  u8 set_speaker;
  /*!
    audio speaker please, see see hdmi_audio_speaker_place_t
    */
  u8 place;
}hdmi_audio_channel_set_t;

/*!
  hdmi audio control commands used by calling hdmi_audio_ctrl
  */
typedef enum
{
  /*!
    audio format setting, for parameter, please see hdmi_audio_format_t
    */
  HDMI_AUDIO_FORMAT_SET = HDMI_CMD_SET,
  /*!
    audio sample parameter setting, for parameter, please see hdmi_audio_sample_param_t
    */
  HDMI_AUDIO_SAMPLE_PARAM_SET,
  /*!
    audio channel setting, for parameter, please see hdmi_audio_channel_set_t
    */
  HDMI_AUDIO_CHANNEL_SET,
  /*!
    audio capability getting, for parameter, please see hdmi_audio_capability_t
    */
  HDMI_AUDIO_CAPABILITY_GET = HDMI_CMD_GET,
}hdmi_audio_cmd_t;

/*!
  hdmi device audio capability
  */
typedef struct
{
  /*!
    supporting audio format count
    */
  u8 format_cnt;
  /*!
    supporting speaker place, see hdmi_audio_speaker_place_t
    */
  u8 speaker_place;
  /*!
    supporting audio channel count
    */
  u8 channel_cnt[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supporting audio format
    */
  hdmi_audio_format_t farmat[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supporting audio sample rate
    */
  hdmi_audio_sample_rate_t sample_rate[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supporting audio sample size
    */
  hdmi_audio_sample_size_t  sample_size[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supporting audio max bit rate
    */
  u32 max_bit_rate[HDMI_AUDIO_FORMAT_MAX_NUM];
}hdmi_audio_capability_t;

/*!
  hdmi hdcp ksv list
  */
typedef struct
{
  /*!
    hdmi hdcp ksv list
    */
  u8 *p_ksv_list;
  /*!
    hdmi hdcp ksv list size
    */
  u16 list_size;
}hdmi_hdcp_ksv_list_t;

/*!
  hdmi hdcp status
  */
typedef enum
{
  /*!
    hdmi hdcp status: NOT AUTHENTICATED
    */
  HDMI_HDCP_NOT_AUTHENTICATED,
  /*!
    hdmi hdcp status: AUTHENTICATED
    */
  HDMI_HDCP_AUTHENTICATED,
  /*!
    hdmi hdcp status: KSV INVALID
    */
  HDMI_HDCP_NOT_AUTH_KSV_INVALID,
  /*!
    hdmi hdcp status: KSV REVOKED
    */
  HDMI_HDCP_NOT_AUTH_KSV_REVOKED,
  /*!
    hdmi hdcp status: LINK INTEGRITY FAILED
    */
  HDMI_HDCP_NOT_AUTH_LINK_INTEGRITY_FAILED,
  /*!
    hdmi hdcp status: LINK INTEGRITY NO ACK
    */
  HDMI_HDCP_NOT_AUTH_LINK_INTEGRITY_NO_ACK,
  /*!
    hdmi hdcp status: AUTH DISABLED
    */
  HDMI_HDCP_AUTH_DISABLED
}hdmi_hdcp_auth_status_t;

/*!
  Source Product Description infoframe type
  */
typedef enum
{
  /*!
    unknown SPD type
    */
  HDMI_SPD_INFO_UNKNOWN     = 0x00,
  /*!
    SPD type: DIGITAL STB
    */
  HDMI_SPD_INFO_DIGITAL_STB   = 0x01,
  /*!
    SPD type: DVD PLAYER
    */
  HDMI_SPD_INFO_DVD_PLAYER    = 0x02,
  /*!
    SPD type: DVHS
    */
  HDMI_SPD_INFO_DVHS        = 0x03,
  /*!
    SPD type: HDD VIDEO RECODER
    */
  HDMI_SPD_INFO_HDD_VIDEO_REC = 0x04,
  /*!
    SPD type: DVC
    */
  HDMI_SPD_INFO_DVC       = 0x05,
  /*!
    SPD type: DSC
    */
  HDMI_SPD_INFO_DSC     = 0x06,
  /*!
    SPD type: VCD
    */
  HDMI_SPD_INFO_VCD       = 0x07,
  /*!
    SPD type: GAME
    */
  HDMI_SPD_INFO_GAME        = 0x08,
  /*!
    SPD type: PC gereral
    */
  HDMI_SPD_INFO_PC_GENERAL    = 0x09,
  /*!
    SPD type: BD player
    */
  HDMI_SPD_INFO_BD_PLAYER   = 0x0A,
  /*!
    SPD type: SACD
    */
  HDMI_SPD_INFO_SACD        = 0x0B,
  /*!
    Maximum number of SPD type
    */
  HDMI_SPD_INFO_MAX,
}hdmi_spd_info_type_t;

/*!
  Source Product Description infoframe
  */
typedef struct
{
  /*!
    Source Product Description infoframe type
    */
  hdmi_spd_info_type_t type;
  /*!
    vendor name
    */
  u8 vendor_name[8];
  /*!
    product description
    */
  u8 product_desc[16];
}hdmi_spd_info_t;

/*!
  HDMI event need to be notified to up-layer
  */
typedef enum
{
  /*!
    HDMI port connection event, param is 0 means Disconnected, 1 means Connected
    */
  HDMI_EVENT_CONNECTION_STATUS = 0x01,
  /*!
    cec message, for param see hdmi_ece_msg_t,
    */
  HDMI_EVENT_CEC_MESSAGE = 0x02,
  /*!
    Unsupported Native/Preffered video format, param: 16 LSB - standard, 16 MSB - resolution
    */
  HDMI_EVENT_UNSUPPORTED_FORMAT = 0x04,
  /*!
    hdcp status, param is 0 means Not authenticated, 1 means Authenticated
    */
  HDMI_EVENT_HDCP_AUTH_STATUS = 0x08,
  /*!
    HDMI video clock change
    */
  HDMI_EVENT_VIDEO_CLK_CHG = 0x10,
  /*!
    HDMI video format change
    */
  HDMI_EVENT_VIDEO_FMT_CHG = 0x20,
}hdmi_event_id_t;

/*!
  HDMI event nofity function
  */
typedef RET_CODE (*hdmi_notify_t)(u32 event, u32 param, u32 context);

/*!
  HDMI event nofity information used by calling hdmi_notify_register & hdmi_notify_unregister
  */
typedef struct
{
  /*!
    HDMI event ids this notify should supported, see hdmi_event_id_t
    */
  u32 events;
  /*!
    HDMI event notify function registered
    */
  hdmi_notify_t notify;
  /*!
    the context to be transfered back by calling event notify function
    */
  u32 context;
}hdmi_notify_info_t;

/*!
  hdmi nofity information param
  */
typedef struct tag_hdmi_notify_param
{
  /*!
    HDMI event ids this notify should supported, see hdmi_event_id_t
    */
  u32 event;
  /*!
    the param to be transfered back by calling event notify function
    */
  u32 param;
  /*!
    the context to be transfered back by calling event notify function
    */
  u32 context;
}hdmi_notify_param_t;

/*!
  HDMI cec message
  */
typedef struct
{
  /*!
    cec message
    */
  u8 msg[HDMI_CEC_MESSAGE_LEN];
}hdmi_ece_msg_t;

/*!
  HDMI cec deck mode
  */
typedef enum
{
  /*!
    cec deck mode: play
    */
  HDMI_CEC_DECK_MODE_PLAY,
  /*!
    cec deck mode: play bw
    */
  HDMI_CEC_DECK_MODE_PLAY_BW,
  /*!
    cec deck mode: pause
    */
  HDMI_CEC_DECK_MODE_PAUSE,
  /*!
    cec deck mode: slow
    */
  HDMI_CEC_DECK_MODE_SLOW,
  /*!
    cec deck mode: slow bw
    */
  HDMI_CEC_DECK_MODE_SLOW_BW,
  /*!
    cec deck mode: fast
    */
  HDMI_CEC_DECK_MODE_FAST,
  /*!
    cec deck mode: fast bw
    */
  HDMI_CEC_DECK_MODE_FAST_BW,
  /*!
    cec deck mode: no media
    */
  HDMI_CEC_DECK_MODE_NO_MEDIA,
  /*!
    cec deck mode: stop
    */
  HDMI_CEC_DECK_MODE_STOP,
  /*!
    cec deck mode: skip fw
    */
  HDMI_CEC_DECK_MODE_SKIP_FW,
  /*!
    cec deck mode: skip bw
    */
  HDMI_CEC_DECK_MODE_SKIP_BW,
  /*!
    cec invalid deck mode
    */
  HDMI_CEC_DECK_MODE_INVALID
}hdmi_cec_deck_mode_t;

/*!
  HDMI cec tuner broadcast mode
  */
typedef enum
{
  /*!
    HDMI cec tuner broadcast mode: arib generic
    */
  HDMI_CEC_TUNER_BROADCAST_ARIB_GENERIC   = 0x00,
  /*!
    HDMI cec tuner broadcast mode: atsc generic
    */
  HDMI_CEC_TUNER_BROADCAST_ATSC_GENERIC   = 0x01,
  /*!
    HDMI cec tuner broadcast mode: dvb generic
    */
  HDMI_CEC_TUNER_BROADCAST_DVB_GENERIC    = 0x02,
  /*!
    HDMI cec tuner broadcast mode: arib bs
    */
  HDMI_CEC_TUNER_BROADCAST_ARIB_BS      = 0x08,
  /*!
    HDMI cec tuner broadcast mode: arib cs
    */
  HDMI_CEC_TUNER_BROADCAST_ARIB_CS      = 0x09,
  /*!
    HDMI cec tuner broadcast mode: arib t
    */
  HDMI_CEC_TUNER_BROADCAST_ARIB_T     = 0x0A,
  /*!
    HDMI cec tuner broadcast mode: atsc cable
    */
  HDMI_CEC_TUNER_BROADCAST_ATSC_CABLE   = 0x10,
  /*!
    HDMI cec tuner broadcast mode: atsc satellite
    */
  HDMI_CEC_TUNER_BROADCAST_ATSC_SATELLITE = 0x11,
  /*!
    HDMI cec tuner broadcast mode: atsc terresrial
    */
  HDMI_CEC_TUNER_BROADCAST_ATSC_TERRESTRIAL = 0x12,
  /*!
    HDMI cec tuner broadcast mode: dvb-c
    */
  HDMI_CEC_TUNER_BROADCAST_DVB_C        = 0x18,
  /*!
    HDMI cec tuner broadcast mode: dvb-s
    */
  HDMI_CEC_TUNER_BROADCAST_DVB_S        = 0x19,
  /*!
    HDMI cec tuner broadcast mode: dvb-s2
    */
  HDMI_CEC_TUNER_BROADCAST_DVB_S2     = 0x1A,
  /*!
    HDMI cec tuner broadcast mode: dvb-t
    */
  HDMI_CEC_TUNER_BROADCAST_DVB_T        = 0x1B,
}hdmi_cec_tuner_mode_t;

/*!
  HDMI cec osd display control
  */
typedef enum HDMI_CEC_OSD_DISPLAY_CONTROL_TAG
{
  /*!
    cec osd display command: default time
    */
  HDMI_CEC_OSD_DEFAULT_TIME       = 0,
  /*!
    cec osd display command: until cleared
    */
  HDMI_CEC_OSD_UNTIL_CLEARED        = 1,
  /*!
    cec osd display command: clear pre message
    */
  HDMI_CEC_OSD_CLEAR_PRE_MESSAGE      = 2,
  /*!
    reserved cec osd display command
    */
  HDMI_CEC_OSD_RESERVED           = 3
}hdmi_cec_osd_ctrl_t;

/*!
  HDMI cec osd display control
  */
typedef struct
{
  /*!
    HDMI cec osd display command
    */
  hdmi_cec_osd_ctrl_t ctrl;
  /*!
    HDMI cec osd display text
    */
  char  *p_text;
  /*!
    HDMI cec osd display text length
    */
  u32 text_len;
}hdmi_cec_osd_display_t;

/*!
  HDMI cec device osd name
  */
typedef struct
{
  /*!
    HDMI cec device osd name string pointer
    */
  char *p_name;
  /*!
    HDMI cec device osd name length
    */
  u32 name_len;
}hdmi_cec_dev_osd_name_t;

/*!
  HDMI cec vender command
  */
typedef struct
{
  /*!
    HDMI cec vender id
    */
  u32 vendor_id;
  /*!
    HDMI cec vender command string
    */
  u8 *p_cmd;
  /*!
    HDMI cec vender command string length
    */
  u8 cmd_len;
  /*!
    the flag to indicate if has vender command with this id
    */
  BOOL vendor_cmd_with_id;
}hdmi_cec_vender_cmd_t;

/*!
  HDMI cec remote control command
  */
typedef struct
{
  /*!
    key pressed
    */
  BOOL pressed;
  /*!
    remote control command string
    */
  u8 *p_rc_cmd;
  /*!
    remote control command string length
    */
  u8 cmd_len;
}hdmi_cec_rc_cmd_t;

/*!
  HDMI cec message
  */
typedef struct
{
  /*!
    HDMI cec message string
    */
  u8 msg_data[HDMI_CEC_MESSAGE_LEN];
  /*!
    HDMI cec message length
    */
  u8 msg_len;
}hdmi_cec_msg_t;

/*!
  HDMI cec abort information
  */
typedef struct
{
  /*!
    HDMI cec abort operation code
    */
  u8 op_code;
  /*!
    HDMI cec abort reason
    */
  u8 abort_reason;
}hdmi_cec_abort_t;

/*!
  HDMI cec command id
  */
typedef enum
{
  /*!
    HDMI cec command: ACTIVE, param is TRUE means active, FALSE means deactive
    */
  HDMI_CEC_ACTIVE,
  /*!
    HDMI cec command: one touch play, param is TRUE means enable, FALSE means disable
    */
  HDMI_CEC_ONE_TOUCH_PLAY,
  /*!
    HDMI cec command: abort, param is NULL
    */
  HDMI_CEC_ABORT,
  /*!
    HDMI cec command: set deck mode, for param please see hdmi_cec_deck_mode_t
    */
  HDMI_CEC_SET_DECK_MODE,
  /*!
    HDMI cec command: set tuner mode, for param please see hdmi_cec_tuner_mode_t
    */
  HDMI_CEC_SET_TUNER_MODE,
  /*!
    HDMI cec command: set osd display, for param please see hdmi_cec_osd_display_t
    */
  HDMI_CEC_SET_OSD_DISPLAY,
  /*!
    HDMI cec command: set device osd name, for param please see hdmi_cec_dev_osd_name_t
    */
  HDMI_CEC_SET_DEV_OSD_NAME,
  /*!
    HDMI cec command: set device vender id, param is vender id
    */
  HDMI_CEC_SET_DEV_VENDER_ID,
  /*!
    HDMI cec command: set peer device vender id, param is peer device vender id
    */
  HDMI_CEC_SET_PEER_DEV_VENDER_ID,
  /*!
    HDMI cec command: set vender command, for param please see hdmi_cec_vender_cmd_t
    */
  HDMI_CEC_SET_VENDER_CMD,
  /*!
    HDMI cec command: set menu status, param is TRUE means avtived, FALSE means deactived
    */
  HDMI_CEC_SET_MENU_STATUS,
  /*!
    HDMI cec command: send the command of getting menu language, param is NULL
    */
  HDMI_CEC_SEND_GET_MEMU_LANGUAGE,
  /*!
    HDMI cec command: send command of remote control pass through, for param please see hdmi_cec_rc_cmd_t
    */
  HDMI_CEC_SEND_RC_PASS_THROUGH,
  /*!
    HDMI cec command: send message, for param pelease see hdmi_cec_msg_t
    */
  HDMI_CEC_SEND_MSG,
}hdmi_cec_cmd_t;

/*!
  HDMI edid error code
  */
typedef enum
{
  /*!
    no error
    */
  HDMI_TX_EDID_NO_ERR = 0x00,
  /*!
    bad edid header
    */
  HDMI_TX_EDID_BAD_HEADER,
  /*!
    edid version 1.3 not supported
    */
  HDMI_TX_EDID_13_NOT_SUPPORTED,
  /*!
    edid check sum error
    */
  HDMI_TX_EDID_CHECK_SUM_ERR,
  /*!
    no extend block
    */
  HDMI_TX_EDID_NO_EXT_BLOCK,
  /*!
    extend block is not for EIA-CEA-861 standard
    */
  HDMI_TX_EDID_EXT_BLOCK_NOT_FOR_861,
}hdmi_edid_err_code_t;

/*!
  HDMI video format
  */
typedef enum
{
  /*!
    undefined
    */
  HDMI_VIDEO_UNDEFINED,
  /*!
    1080p60
    */
  HDMI_VIDEO_1080P_60,
  /*!
    1080p50
    */
  HDMI_VIDEO_1080P_50,
  /*!
    1080i60
    */
  HDMI_VIDEO_1080I_60,
  /*!
    1080i50
    */
  HDMI_VIDEO_1080I_50,
  /*!
    720p60
    */
  HDMI_VIDEO_720P_60,
  /*!
    720p50
    */
  HDMI_VIDEO_720P_50,
  /*!
    576p50
    */
  HDMI_VIDEO_576P_50,
  /*!
    576i50
    */
  HDMI_VIDEO_576I_50,
  /*!
    640X480P60
    */
  HDMI_VIDEO_640X480P_60,
  /*!
    720x480p60
    */
  HDMI_VIDEO_720X480P_60,
  /*!
    720x480I60
    */
  HDMI_VIDEO_720X480I_60
}hdmi_video_format_t;


/*!
  HDMI edid information
  */
typedef struct
{
  /*!
    the flag to indicate is the device is HDMI supported
    */
  u32 is_hdmi : 1;
  /*!
    the flag to indicate is the device support YCbCr444
    */
  u32 ycbcr444_supported : 1;
  /*!
    the flag to indicate is the device support YCbCr422
    */
  u32 ycbcr422_supported : 1;
  /*!
    the flag to indicate is the device support 1080p 60Hz
    */
  u32 supported_1080p_60Hz : 1;
  /*!
    the flag to indicate is the device support 1080p 50Hz
    */
  u32 supported_1080p_50Hz : 1;
  /*!
    the flag to indicate is the device support 1080i 60Hz
    */
  u32 supported_1080i_60Hz : 1;
  /*!
    the flag to indicate is the device support 1080i 50Hz
    */
  u32 supported_1080i_50Hz : 1;
  /*!
    the flag to indicate is the device support 720p 60Hz
    */
  u32 supported_720p_60Hz : 1;
  /*!
    the flag to indicate is the device support 720p 50Hz
    */
  u32 supported_720p_50Hz : 1;
  /*!
    the flag to indicate is the device support 576p 50Hz
    */
  u32 supported_576p_50Hz : 1;
  /*!
    the flag to indicate is the device support 576i 50Hz
    */
  u32 supported_576i_50Hz : 1;
  /*!
    the flag to indicate is the device support 640X480p 60Hz
    */
  u32 supported_640x480p_60Hz : 1;
  /*!
    the flag to indicate is the device support 720X480p 60Hz
    */
  u32 supported_720x480p_60Hz : 1;
  /*!
    the flag to indicate is the device support 720X480i 60Hz
    */
  u32 supported_720x480i_60Hz : 1;
  /*!
    the flag to indicate is the device support deep color rgb 30bit
    */
  u32 rgb30bit : 1;
  /*!
    the flag to indicate is the device support deep color rgb 36bit
    */
  u32 rgb36bit : 1;
  /*!
    the flag to indicate is the device support deep color rgb 48bit
    */
  u32 rgb48bit : 1;
  /*!
    the flag to indicate is the device support deep color y444
    */
  u32 dc_y444 : 1;
  /*!
    the flag to indicate is the device support xvycc601
    */
  u32 supported_xvycc601 : 1;
  /*!
    the flag to indicate is the device support xvycc709
    */
  u32 supported_xvycc709 : 1;
  /*!
    supported audio format count
    */
  u8 audioformat_cnt;
  /*!
    supported audio format
    */
  u8 audioformat[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supported audio channel
    */
  u8 audiochannel[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supported audio sample rate
    */
  u8 audiofs[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supported audio sample size
    */
  u8 audiolength[HDMI_AUDIO_FORMAT_MAX_NUM];
  /*!
    supported speaker placement
    */
  u8 speakerformat;
  /*!
    edid paser error code
    */
  hdmi_edid_err_code_t edid_errcode;
  /*!
    device native format
    */
  hdmi_native_format_t native_format;
  /*!
    receiver preferred format
    */
  hdmi_video_format_t preferred_format;
}hdmi_edid_result_t;

/*!
  HDMI audio I2S configuration
  */
typedef struct
{
  /*!
    MCLK and Fs relationship
    0:MCLK = 128 * Fs, 1:MCLK = 256 * Fs, 2:MCLK = 384 * Fs, 3:MCLK = 512 * Fs
    */
  u8 mclk_fs_relation;
  /*!
    HDMI audio sample rate, see hdmi_audio_sample_rate_t
    */
  hdmi_audio_sample_rate_t sample_rate;
  /*!
    HDMI audio sample rate, see hdmi_audio_sample_size_t
    */
  hdmi_audio_sample_size_t sample_size;
  /*!
    1:enable channel 0, 0:disable
    */
  u8 channel_0_en : 1;
  /*!
    1:enable channel 1, 0:disable
    */
  u8 channel_1_en : 1;
  /*!
    1:enable channel 2, 0:disable
    */
  u8 channel_2_en : 1;
  /*!
    1:enable channel 3, 0:disable
    */
  u8 channel_3_en : 1;
  /*!
    0: fist bit shift(philips spec), 1:no shift
    */
  u8 shift_ctrl : 1;
  /*!
    0:SD data MSB first, 1:LSB first
    */
  u8 dir_ctrl : 1;
  /*!
    0:left polarity when word select is low, 1:left polarity when word select is high
    */
  u8 ws_pol : 1;
  /*!
    0:data is left justified, 1:data is right justified
    */
  u8 justify_ctrl : 1;
}hdmi_i2s_config_t;

/*!
  HDMI input audio configuration
  */
typedef struct
{
  /*!
    audio type, see hdmi_audio_source_t
    */
  hdmi_audio_source_t type;
  /*!
    audio down sample, see hdmi_audio_down_sample_t
    */
  hdmi_audio_down_sample_t down_sample;
  /*!
    i2s config data
    */
  hdmi_i2s_config_t i2s_cfg;
  /*!
    super audio config data
    */
  super_audio_cfg_t super_audio_cfg;
} hdmi_input_audio_cfg_t;

/*!
  HDMI input audio/video configuration
  */
typedef struct
{
  /*!
    input video configuration
    */
  hdmi_input_video_cfg_t input_v_cfg;
  /*!
    input audio configuration
    */
  hdmi_input_audio_cfg_t input_a_cfg;
}hdmi_hw_cfg_t;

/*!
  register notify function to HDMI driver

  \param[in] p_dev  The HDMI device handle
  \param[in] p_notify_info  The notify information
  \param[in] id  The notify id

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_notify_register(drv_dev_t *p_dev, hdmi_notify_info_t *p_notify_info, u32 id);

/*!
  unregister notify function to HDMI driver

  \param[in] p_dev  The HDMI device handle
  \param[in] events  The notify events needs to be unregistered
  \param[in] id  The notify id

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_notify_unregister(drv_dev_t *p_dev, u32 events, u32 id);

/*!
  HDMI video configuration

  \param[in] p_dev  The HDMI device handle
  \param[in] p_video_cfg  The video configuration parameters

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_video_config(drv_dev_t *p_dev,  hdmi_video_config_t *p_video_cfg);

/*!
  HDMI audio configuration

  \param[in] p_dev  The HDMI device handle
  \param[in] p_audio_cfg  The audio configuration parameters

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_audio_config(drv_dev_t *p_dev,  hdmi_input_audio_cfg_t *p_audio_cfg);

/*!
  HDMI video command control

  \param[in] p_dev  The HDMI device handle
  \param[in] cmd  The video control command
  \param[in] p_param  The video control command parameter

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_video_ctrl(drv_dev_t *p_dev, hdmi_video_cmd_t cmd, void *p_param);

/*!
  HDMI audio command control

  \param[in] p_dev  The HDMI device handle
  \param[in] cmd  The audio control command
  \param[in] p_param  The audio control command parameter

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_audio_ctrl(drv_dev_t *p_dev, hdmi_audio_cmd_t cmd, void *p_param);

/*!
  HDMI cec control

  \param[in] p_dev  The HDMI device handle
  \param[in] cmd  The cec control command
  \param[in] p_param  The cec control command parameter

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_cec_ctrl(drv_dev_t *p_dev, hdmi_cec_cmd_t cmd, void *p_param);

/*!
  mute HDMI audio & video

  \param[in] p_dev  The HDMI device handle
  \param[in] mute  TRUE means mute, FALSE means un-mute

  \return Return 0 for success and others for failure.
  */
extern RET_CODE hdmi_av_mute(drv_dev_t *p_dev, BOOL mute);

#ifdef __cplusplus
}
#endif

#endif //__HDMI_H__


