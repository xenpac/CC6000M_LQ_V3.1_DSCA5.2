/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MP3_TRANS_FILTER_INTRA_H_
#define __MP3_TRANS_FILTER_INTRA_H_



/*!
  private data
  */
typedef struct tag_mp3_trans_filter_private
{
  /*!
    this point !!
    */
  void *p_this;

  /*!
    it's input pin
    */
  mp3_trans_in_pin_t m_in_pin;
  
  /*!
    it's output pin
    */
  mp3_trans_out_pin_t m_out_pin;

  /*!
    connect with lrc filter
    */
  mp3_trans_lrc_out_pin_t m_lrc_out_pin;  

  /*!
    connect with pic filter   
    */
  mp3_trans_pic_out_pin_t m_pic_out_pin;  
  /*!
    using sample
    */
  //mp3_getframe_state_t *p_state;
  
  /*!
    play mode
    */
  mp3_play_mode_t play_mode;
  /*!
    mp3 info
    */
  mpegaudioframeinfo_t *p_mp3_info; 
    /*!
    TOC table
    */
  //u8 TOC[100];
     /*!
    speed count
    */
  u32 cnt;
     /*!
    buff played count
    */
  u32 buff_played;
     /*!
    time jump
    */
  u32 timejump;
  /*!
    file name
    */ 
  u16 *p_file_name;
  /*!
    pic buf push sign 
    */
  BOOL is_push;
  /*!
    current playtime
    */
    u16 cur_play_time;
  /*!
    show logo
    */
  BOOL b_music_logo_show;  
  /*!
    show lrc
    */
  BOOL b_music_lrc_show;  
  /*!
    current frame count
    */
  u32 cur_frame_cnt;
  /*!
    jump frame count
    */
  u32 save_frame_cnt;
  /*!
    true: jump mode, false: normal
    */
  u8 jump;  
  /*!
    jump mode,curn frame count
    */
  u32 jump_cur_frame;
  /*!
    source url
    */
  u8 *p_src_url;  
  /*!
    first_frame
    */
  BOOL first_frame;
}mp3_trans_filter_private_t;

/*!
  the mp3_trans filter define
  */
typedef struct tag_mp3_trans_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data buffer
    */
  mp3_trans_filter_private_t private_data;
}mp3_trans_filter_t;

#endif // End for __MP3_TRANS_FILTER_INTRA_H_

