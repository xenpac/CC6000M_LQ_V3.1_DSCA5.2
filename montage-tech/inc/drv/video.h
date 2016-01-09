/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __VIDEO_H__
#define __VIDEO_H__

/*!
  The width of full screen for video layer (in pixels).
  */
#define VIDEO_FULLSCR_WIDTH            720
/*!
  The height of full screen for video layer in PAL standard (in pixels).
  */
#define VIDEO_FULLSCR_HEIGHT_PAL       576
/*!
  The height of full screen for video layer in NTSC standard (in pixels).
  */
#define VIDEO_FULLSCR_HEIGHT_NTSC      480
/*!
  The size of a luma line in a video frame with compression (in bytes).
  */
#define VIDEO_SCALED_LUMA_WIDTH        452
/*!
  The size of a chroma line in a video frame with compression (in bytes).
  */
#define VIDEO_SCALED_CHROMA_WIDTH      228
/*!
  The size of a frame buffer without compression, in Kbytes.
  */
#define VIDEO_FRAME_SIZE_KB  ((u32)(((VIDEO_FULLSCR_WIDTH) * \
                    (VIDEO_FULLSCR_HEIGHT_PAL) * 3 / 2) >> 10) + 1)
/*!
  The size of a frame buffer with compression, in Kbytes.
  */
#define VIDEO_FRAME_SIZE_CMP_KB   ((u32)(((VIDEO_SCALED_CHROMA_WIDTH) * \
              (VIDEO_FULLSCR_HEIGHT_PAL) + (VIDEO_SCALED_LUMA_WIDTH) * \
              (VIDEO_FULLSCR_HEIGHT_PAL)) >> 10) + 1)     
/*!
  The size of a tag buffer, in Kbytes.
  */
#define VIDEO_TAG_SIZE  8
/*!
  The size of a display buffer in 9 picture decoding mode, in Kbytes.
  */
#define VIDEO_9PIC_DISP_BUFFER_SIZE   41  

/*!
  This structure defines video commands by dev_io_ctrl. 
  */
typedef enum
{
  /*!
    Stop video decoding (and audio decoding)
    */
  VIDEO_CMD_STOP = DEV_IOCTRL_TYPE_UNLOCK + 0,
  /*!
    Enable video decoding (and audio decoding)
    */
  VIDEO_CMD_PLAY,
  /*!
    Make a pause for video playing
    */
  VIDEO_CMD_PAUSE,
  /*!
    Resume from puase state
    */
  VIDEO_CMD_RESUME,
  /*!
    Enter preview mode
    */
  VIDEO_CMD_ENTER_PREVIEW,
  /*!
    Set zoom parameters for automatic scaling modes
    */
  VIDEO_CMD_SET_ZOOM_PARAM,
  /*!
    Exit preview mode and return to full screen display mode
    */
  VIDEO_CMD_LEAVE_PREVIEW,
  /*!
    Context preparation for waiting for decoded frames after channel switch
    */
  VIDEO_CMD_START_WAIT_FRAME,
  /*!
    Check if current video display is stable
    */
  VIDEO_CMD_IS_STABLE,
  /*!
    Check if current decoded picture has supported format
    */
  VIDEO_CMD_IS_SUPPORTED,
  /*!
    Set current video standard of output
    */
  VIDEO_CMD_SET_STANDARD,
  /*!
    Get current video standard of output
    */
  VIDEO_CMD_GET_STANDARD,
  /*!
    Get the video standard of source stream
    */
  VIDEO_CMD_GET_STREAM_STANDARD,
  /*!
    Set the color of background layer  
    */
  VIDEO_CMD_SET_BG_COLOR,
  /*!
    Display/Hide the video layer
    */
  VIDEO_CMD_VIDEO_DISPLAY,
  /*!
    Check if the video layer is displayed
    */
  VIDEO_CMD_VIDEO_IS_DISPLAY,
  /*!
    Display/Hide the still picture layer
    */
  VIDEO_CMD_SPIC_DISPLAY,
  /*!
    Display/Hide the background layer
    */
  VIDEO_CMD_BG_DISPLAY,
  /*!
    Set video display to full screen mode or zoom mode
    */
  VIDEO_CMD_SET_ZOOM_STATE,
  /*!
    Fill a 9-picture unit with back
    */
  VIDEO_CMD_9PIC_FILL_BLACK,
  /*!
    Context preparation for page switch in 9-picture mode
    */
  VIDEO_CMD_9PIC_PAGE_SWITCH,
  /*!
    Update video display info.
    */
  VIDEO_CMD_DISP_INFO_UPDATE,
  /*!
    Get current video and audio ES count
    */
  VIDEO_CMD_GET_ES_CNT,
  /*!
    Set disp mode according current video and audio ES count
    */
  VIDEO_CMD_ESCNT_DISPMODE,
  /*!
    Get current bus state for ES buffer access
    */
  VIDEO_CMD_ES_STATE_QUERY,
  /*!
    Set bus state for ES buffer access
    */
  VIDEO_CMD_ES_STATE_SET,
  /*!
    Configure the compression and de-compression settings of video decoding and display
    */
  VIDEO_CMD_COMPRESSION_SET,
  /*!
    Enable/disable video interrupt
    */
  VIDEO_CMD_ISR_EN,
  /*!
    Trigger of video DACs
    */
  VIDEO_CMD_DAC_ONOFF,
  /*!
    Set freezing picture
    */
  VIDEO_CMD_FREEZE_SET,
  /*!
    Resume from freezing mode
    */
  VIDEO_CMD_FREEZE_RESUME,
  /*!
    Force to full screen displa mode with pre-defined aspect ratio setting.
    */
  VIDEO_CMD_ASPECT_SET,  
  /*!
    Get the frame buffer address
    */
  VIDEO_CMD_BUF_GET,
  /*!
    Get the buffer address of still
    */
  VIDEO_CMD_SPIC_ADDR_QUERY,
  /*!
    Set 3 frame buffer address to the same
    */
  VIDEO_CMD_BUF_SET,
  /*!
    Restore video buf
    */
  VIDEO_CMD_BUF_RESTORE,
  /*!
    Set color bar state for display
   */
  VIDEO_CMD_COLOR_BAR_SET,
  /*!
    Set Y/G sync
    */
  VIDEO_CMD_YG_SYNC_EN
}video_cmd_t;


/*!
  This structure defines the supported video standards.
  */
typedef enum
{
  /*! 
    NTSC 
    */
  VIDEO_STANDARD_NTSC,
  /*! 
    PAL 
    */
  VIDEO_STANDARD_PAL,
  /*! 
    PAL-Nc 
    */
  VIDEO_STANDARD_PAL_N,
  /*! 
    PAL-M 
    */
  VIDEO_STANDARD_PAL_M
} video_standard_t;

/*!
  This structure defines the supported aspect ratio for video display.
  */
typedef enum
{
  /*!
    Default mode
    */
  VIDEO_ASPECT_DEFAULT,
  /*!
    4:3 PanScan mode
    */
  VIDEO_ASPECT_43_PANSCAN,
  /*!
    4:3 LetterBox mode
    */
  VIDEO_ASPECT_43_LETTERBOX,
  /*!
    16:9 mode
    */
  VIDEO_ASPECT_169,
  /*!
    16:9 PanScan mode
    */
  VIDEO_ASPECT_169_PANSCAN,
  /*!
    16:9 LetterBox mode
    */
  VIDEO_ASPECT_169_LETTERBOX,  
}video_aspect_t;

/*!
  This structure defines the supported output formats of video DAC.
  */
typedef enum
{
  /*!
    Red component
    */
  VIDEO_DAC_R,
  /*!
    Green component
    */
  VIDEO_DAC_G,
  /*!
    Blue component
    */
  VIDEO_DAC_B,
  /*!
    Y component
    */
  VIDEO_DAC_Y,
  /*!
    U component
    */
  VIDEO_DAC_U,
  /*!
    V component
    */
  VIDEO_DAC_V,
  /*!
    Composite video
    */
  VIDEO_DAC_CVBS,
  /*!
    Luminance component
    */
  VIDEO_DAC_LUMA,
  /*!
    Chrominace componet
    */
  VIDEO_DAC_CHROMA
}video_output_t;

/*!
  The integrated video DACs
  */
typedef enum
{
  /*!
    Video DAC 0
    */
  VDAC_0,
  /*!
    Video DAC 1
    */
  VDAC_1,
  /*!
    Video DAC 2
    */
  VDAC_2,
  /*!
    Video DAC 3
    */
  VDAC_3
}video_dac_t;

/*!
  Video DAC onoff
  */
typedef struct
{
  /*!
    Video DAC Index
    */
  video_dac_t dac;
  /*!
    On or Off
    */
  BOOL is_on;
}video_dac_onoff_t;

/*!
  Descriptor of video display plane size.
  */
typedef struct
{
  /*! 
    The start x of video plane 
    */
  u16 x;
  /*! 
    The start y of video plane 
    */
  u16 y;
  /*! 
    The width of video plane
    */
  u16 width;
  /*! 
    The height of video plane
    */
  u16 height;
} video_rect_t;

/*!
  The target rectangle for video scaling
  */
typedef struct
{
   /*! 
     The rectangle of scaled video plane
     */
   video_rect_t rect;
   /*! 
     True for zoom display mode; False for full screen display mode
     */
   BOOL is_zoom;
}zoom_param_t;

/*!
  This structure defines the bus requests for ES buffer access.
  */
typedef struct
{
  /*!
    Bus request for reading data from ES buffer reading 
    */
  BOOL video_es_r_bus_req;
  /*!
    Bus request for writing data to ES buffer 
    */
  BOOL video_es_w_bus_req;
}video_es_bus_req_t;

/*!
  Initialization parameters for the still picture display layer
  */
typedef struct
{
  /*! 
    The display mode for still picture: TRUE for frame and FALSE for field
    */
  BOOL is_frame;
  /*! 
    The width of still picture plane
    */
  u32 width;
  /*! 
    The height of still picture plane
    */
  u32 height;
  /*!
    Left of horizontal position of still picture plane in screen.
    Min value = 1
    */
  u32 x_start;
  /*!
    Top of vertical position of still picture plane in screen.
    */
  u32 y_start;
  /*!
    The horizontal intervals of still picture 
    */
  u16 interval_h;
  /*!
    The vertical intervals of still picture 
    */
  u16 interval_v;  
  /*!
    The byte order for data storage of sill picture
    */
  BOOL is_endian;
}spic_cfg_t;

/*!
  This structure defines a display plane for 9-picture.
  */
typedef struct
{
  /*! 
    The left x coordinate of 9-picture plane. 
    */
  u16 x;
  /*! 
    The top y coordinate of 9-picture plane
    */
  u16 y;
  /*! 
    The width of 9-picture each unit. It should not exceed 180.
    */
  u8  width;
  /*! 
    The height of 9-picture each unit. It should not exceed 144 
    */
  u8  height;
  /*! 
    The width of horizontal intervals between 9 pic units
    */
  u16 interval_h;
  /*! 
    The width of vertical intervals between 9 pic units
    */
  u16 interval_v;
  /*! 
    The background color of each unit. Format: |no use |y|Cb|Cr|
    */
  u32 ycrcb;
  /*! 
    The display mode of 9-picture static unit: TRUE for frame and FALSE for field
    */
  BOOL is_frame;
} pic_9_t;


/*!
  This structure defines the inforation of video frame buffers
  */
typedef struct
{
  /*!
    frame buffer index
    */
  u8 buffer_idx;
  /*!
    address of the frame buffer
    */
  u32 buffer_addr;
}video_buffer_t;


/*!
  Initialization parameters for a video device.
  */
typedef struct 
{
  /*! 
    If compression is used for decoded video frame
    */
  BOOL is_compression_used;
}video_cfg_t;

/*!
  This structure defines a video device.
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
} video_device_t;


 /*!
  Initialization of 9-picture mode.
  Application should do buffer config before calling this function.
  
  \param[in] p_dev The pointer to the video device.
  \param[in] p_rect The pointer to the descriptor of the 9-picture display plane.

  \return Return SUCCESS for 9-picture mode done
  */
RET_CODE video_enter_9pic(video_device_t *p_dev, const pic_9_t *p_rect);

/*!
  Static: Fill the specific rectangle (in still picture layer) with a frame of video
  data in display buffers.
  Live: Play video in the specific rectangle.

  \param[in] p_dev The pointer to the video device.
  \param[in] p_rect The target 9-picture unit
  \param[in] is_live TRUE to play video in the target unit; 
                 otherwise to fill the unit with a frame of video.

  \return Return SUCCESS for video filling done.                    
  */
RET_CODE video_fill_9pic(video_device_t *p_dev, 
                           const video_rect_t *p_rect, BOOL is_live);     

/*!
  Exit 9-picture decoding mode and return to normal full screen play mode.
  Application should do buffer config before calling this function.

  \param[in] p_dev The pointer to the video device.

  \return Return SUCCESS for returning to normal decoding mdoe done.
  */
RET_CODE video_leave_9pic(video_device_t *p_dev);

/*!
  Direct play video ES data provided by application.

  \param[in] p_dev The pointer to the video device.
  \param[in] p_data pointer to the input ES video data.
  \param[in] size Size of the input ES video data.

  \return Return SUCCESS for ES data decoding done. 
        Return ERR_TIMEOUT for timeout of decoding.
  */
RET_CODE video_play_media(video_device_t *p_dev, 
                               u8 *p_data, u32 size);                          

/*!
  Get current picture size (in pixels).

  \param[in] p_dev The pointer to the video device.
  \param[out] p_h The vertical size of video
  \param[out] p_v The horizontal size of video  
  */
void video_frame_size_query(video_device_t *p_dev, u32 *p_h, u32 *p_v);        

/*!
  Set the output mode for the video DAC.

  \param[in] p_dev The pointer to the video device.
  \param[in] dac The target video DAC.
  \param[in] mode The output format for the video DAC.
  */
void video_dac_set(video_device_t *p_dev, video_dac_t dac, video_output_t mode);

/*!
  Zoom in the cetain region or the full region of the source video frame.
  Up to 16x zoom is supported.

  \param[in] p_dev The pointer to the video device.
  \param[in] src_rect The selected region in source video frame.
  \param[in] dst_rect The target rectangle for video display.

  \return Return SUCCESS for zoom done
  */
RET_CODE video_zoom_set(video_device_t *p_dev, 
                             video_rect_t src_rect, video_rect_t dst_rect);

/*!
  Config the video frame buffers.

  \param[in] p_dev The pointer to the video device.
  \param[in] addr The start address of the memory for the video frame buffers (in Kbytes).
  \param[in] size The Available memory (in Kbytes).
  \param[in] num The number of used frame buffers. 
                It should be 3 or 4 for normal mode and 2 for 9 picture decoding mode.
  \param[in] is_cmp If compression is used for decoded video frame.
  \param[in] is_freezing If freezing function is used.

  \return Return SUCCESS for frame buffer setting done.
        Return ERR_NO_MEM for memory lack.
  */
RET_CODE video_frame_buffer_set(video_device_t *p_dev, u32 addr, 
                u32 size, u8 num, BOOL is_cmp, BOOL is_freezing);
                
/*!
  Config the ES buffers, including audio ES buffer and video ES buffer.
  The ES buffers are continuous and audio ES buffer is in the low address.

  \param[in] p_dev The pointer to the video device.
  \param[in] addr The start address of the memory for the ES buffers (in Kbytes). 
                It should be 8 Kbytes aligned.
  \param[in] aes_size The memory size for audio ES buffer (in Kbytes).
                It should be times of 8 Kbytes.
  \param[in] ves_size The memory size for video ES buffer (in Kbytes). 
                It should be times of 8 Kbytes. 

  \return Return SUCCESS for ES buffer setting done.
        Return ERR_NO_MEM for memory lack.
  */
RET_CODE video_es_buffer_set(video_device_t *p_dev, 
                      u32 addr, u32 aes_size, u32 ves_size);

/*!
  Config the tag buffers.

  \param[in] p_dev The pointer to the video device.
  \param[in] addr The start address of the memory for the tag buffers (in Kbytes).
  \param[in] size The Available memory (in Kbytes).
  \param[in] num The number of the tag buffers.
                It should be 3 or 4 in terms of the number of used frame buffers.

  \return Return SUCCESS for tag buffer setting done.
        Return ERR_NO_MEM for memory lack.
  */
RET_CODE video_tag_buffer_set(video_device_t *p_dev, 
                                   u32 addr, u32 size, u8 num);

/*!
  Config the 3 display buffers of 9 picture decoding mode.

  \param[in] p_dev The pointer to the video device.
  \param[in] addr The start address of the memory for the display buffers (in Kbytes).
  \param[in] size The Available memory (in Kbytes).

  \return Return SUCCESS for display buffer setting done.
        Return ERR_NO_MEM for memory lack.
  */
RET_CODE video_9pic_disp_buffer_set(video_device_t *p_dev, u32 addr, u32 size);

/*!
  Config the still picture buffer.

  \param[in] p_dev The pointer to the video device.
  \param[in] addr The start address of the memory for the still picture buffer (in Kbytes).
  \param[in] size The Available memory (in Kbytes).
  \param[in] is_frame If using frame display mode.

  \return Return SUCCESS for still picture buffer setting done.
        Return ERR_NO_MEM for memory lack.
  */
RET_CODE video_spic_buffer_set(video_device_t *p_dev, 
                                    u32 addr, u32 size, BOOL is_frame);

/*!
  comments

  \param[in] p_dev The pointer to the video device..
  \param[in] p_buf TBD
  \param[in] x TBD
  \param[in] y TBD
  \param[in] w TBD
  \param[in] h TBD
  \param[in] is_video_laye TBD
  \param[in] p_dst TBD
  */
RET_CODE video_show_bmp(video_device_t *p_dev, u8 *p_buf, u16 x, u16 y,
                        u16 w, u16 h, BOOL is_video_layer, u8 *p_dst);

/*!
  comments

  \param[in] p_dev The pointer to the video device..
  \param[in] p_data TBD
  \param[in] size TBD
  \param[in] p_dst TBD
  */
RET_CODE video_play_media_ext(video_device_t *p_dev,
                         u8 *p_data, u32 size, u8 *p_dst);

/*!
  1. Decoding a mpeg still picture on video layer
  2. Copy the decoded data of the video layer to the still layer

  \param[in] p_video handle of video device.
  \param[in] p_buf The porinter to the storage of the source mpeg still data
  \param[in] size The size of  the source mpeg still data
  \param[in] x The left most of the display rectangle
  \param[in] y The top most of the display rectangle
  \param[in] w The width of the still picture
  \param[in] h The height of the still picture
  */
RET_CODE video_spic_show_mpg(video_device_t *p_dev, 
                               u8 *p_buf, 
                               u32 size,
                               u16 x, 
                               u16 y, 
                               u16 w, 
                               u16 h);

#endif //__VIDEO_H__

