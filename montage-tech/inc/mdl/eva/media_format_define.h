/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MEDIA_FORMAT_DEF_H_
#define __MEDIA_FORMAT_DEF_H_

/*!
  stream type define
  */
typedef enum
{
  /*!
    unknown type
    */
  MT_UNKNOWN = 0,
  /*!
    file data type
    */
  MT_FILE_DATA,
  /*!
    PSI type
    */
  MT_PSI,
  /*!
	PES type
	  */
  MT_PES,
  /*!
	RECODE type
	  */
   MT_RECODE,
  /*!
    Video type
    */
  MT_VIDEO,
  /*!
    Audio type
    */
  MT_AUDIO,
  /*!
    Piture type
    */
  MT_IMAGE,
  /*!
    ts data type
    */
  MT_TS_PKT,
  /*!
    ts data type
    */
  MT_INFO_DATA, 
  /*!
    network data type
    */
  MT_NETWORK_REAL_DATA,  
  /*!
    local data type
    */
  MT_NETWORK_BAK_DATA,  
}stream_type_t;

/*!
  media type define
  */
typedef enum
{
  /*!
    unknown type
    */
  MF_UNKNOWN = 0,
  /*!
    video type
    */
  MF_VIDEO,
  /*!
    audio type
    */
  MF_AUDIO,
  /*!
    jpeg type
    */
  MF_JPEG,
  /*!
    bmp
    */
  MF_BMP,
  /*!
    png
    */
  MF_PNG,
  /*!
    gif
    */
  MF_GIF,
  /*!
    mp3 type
    */
  MF_MP3,
  /*!
    image data
    */
  MF_IMAGE,
  /*!
    color data
    */
  MF_COLOR,
  /*!
    Pat table
    */
  MF_PAT,
  /*!
    Sdt table for actual stream.
    */
  MF_SDT_ACTUAL,
  /*!
    Sdt table for other stream.
    */
  MF_SDT_OTHER,
  /*!
    Pmt table.
    */
  MF_PMT,
  /*!
    bat table.
    */
  MF_BAT,
  /*!
    bat multi
    */
  MF_BAT_MULTI,
  /*!
    Nit table for actual stream.
    */
  MF_NIT_ACTUAL,
  /*!
    Nit table for other stream.
    */
  MF_NIT_OTHER,
  /*!
    cat table
    */
  MF_CAT,
  /*!
    mpeg ts packet(video packet)
    */
  MF_MPEG_TS_PACKET,
  /*!
    lrc type
    */
  MF_LRC,
  /*!
    cmd
    */
  MF_CMD,
  /*!
    eit table
    */
  MF_EIT,
  /*!
    dsmcc table
    */
  MF_DSMCC,
}media_type_t;

/*!
  media format define
  */
typedef struct tag_media_format
{
  /*!
    \see stream_type_t
    */
  stream_type_t stream_type;
  /*!
    \see media_type_t
    */
  media_type_t media_type;
}media_format_t;


/*!
  media sub type define
  */
typedef enum tag_sample_state
{
  /*!
    unknown state
    */
  SAMP_STATE_UNKNOWN = 0,
  /*!
    sample is good
    */
  SAMP_STATE_GOOD,
  /*!
    sample fail
    */
  SAMP_STATE_FAIL,
    /*!
    sample is insufficient
    */
  SAMP_STATE_INSUFFICIENT,
  /*!
    sample timeout
    */
  SAMP_STATE_TIMEOUT,
  /*!
    sample memory insufficient
    */
  SAMP_STATE_MEM_INSUFFICIENT,  
}sample_state_t;

/*!
  media sample define
  */
typedef struct tag_media_sample
{
  /*!
    \see media_format_t
    */
  media_format_t format;
  /*!
    buffer's state
    */
  sample_state_t state;
  /*!
    total buffer size
    */
  u32 total_buffer_size;
  /*!
    offset
    */
  u32 data_offset;
  /*!
    sample size
    */
  u32 payload;
  /*!
    sample buffer
    */
  u8 *p_data;
  /*!
    sample context, using to pull mode, decide by the user.
    */
  u32 context;
   /*!
    user data, set by user
    */
  void *p_user_data;
 
  // **************************************************************************
  // * Below item can't be overwrite !!
  // **************************************************************************
  /*!
    this media sample's handle
    NOTE: the field is read only. can't modify it!!
    NOTE: the field is read only. can't modify it!!
    NOTE: the field is read only. can't modify it!!
    */
  void *p_handle;
  /*!
    special API, you must have enough reason for using it.
    lock sample API. if you lock it, the sample can't be destoried.
    so you must do unlock when you don't need it.
    */
  void (*lock)(handle_t _this);
  /*!
    lock sample API, must match with "lock()".
    */
  void (*unlock)(handle_t _this);
}media_sample_t;

#endif // End for __MEDIA_FORMAT_DEF_H_

