/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LRC_FILTER_INTRA_H_
#define __LRC_FILTER_INTRA_H_


/*!
  the mp3_lrc filter define
  */
typedef struct tag_mp3_lrc_filter
{  
/*!
  lyric start time
  */
  u32 start_time;
/*!
  lyric end time
  */  
  u32 end_time;
/*!
  lyrics
  */
  u8 lyric[256];
/*!
  next lyrics 
  */
  struct tag_mp3_lrc_filter *p_next;
/*!
  preview lyrics 
  */
  struct tag_mp3_lrc_filter *p_pre;

}mp3_lrc_filter_t;


/*!
  lyrics length
  */
#define LRCLEN sizeof(mp3_lrc_filter_t)

/*!
  lrc format
  */
typedef enum tag_lrc_fmt
{
  /*!
     unicode
    */
    LRC_FMT_UNICODE = 0,  
  /*!
    gb2312
    */
    LRC_FMT_GB2312,
  /*!
    ascii
    */
    LRC_FMT_ASCII,
  /*!
    picture
    */
    LRC_FMT_PIC,
  /*!
    utf-8
    */
    LRC_FMT_UTF8,
   /*!
    unknown
    */
    LRC_FMT_UNKNOW,
  
}lrc_fmt_t;

/*!
  lrc filter private data
  */
typedef struct tag_lrc_filter_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    input pin
    */
  lrc_in_pin_t m_in_pin;
  /*!
    connect with music player filter 
    */
  lrc_music_in_pin_t m_music_in_pin;
  /*!
    output pin
    */
  lrc_out_pin_t m_out_pin;
  /*!
    lrc list
    */
  mp3_lrc_filter_t *p_head;
  /*!
    current point
    */
  mp3_lrc_filter_t *p_lrc_curr;

  /*!
    lrc count to push out
    */
  u32 lrc_push_cunt;
  /*!
    lrc count to push out
    */ 
  lrc_fmt_t lrc_fmt;

  /*!
    clear lrc once
    */
  BOOL is_clear;  
  /*!
    lrc play mode
    */
  u8 play_mode;
  /*!
    lrc seek time
    */
  u32 seek_time;
}lrc_filter_private_t;


/*!
  the lrc filter define
  */
typedef struct tag_lrc_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data buffer
    */
  lrc_filter_private_t private_data;
}lrc_filter_t;

#endif // End for __lrc_FILTER_INTRA_H_

