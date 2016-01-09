/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MPLAYER_AUD_FILTER_INTRA_H_
#define __MPLAYER_AUD_FILTER_INTRA_H_

/*!
 State of play
 */
typedef enum
{
    /*!
     State None
     */
    MPLAYER_AUD_PLAY_STATE_NONE = 0,   
    /*!
     State of load media
     */    
    MPLAYER_AUD_PLAY_STATE_LOAD_MEDIA,
    /*!
     State of play
     */    
    MPLAYER_AUD_PLAY_STATE_PLAY,
    /*!
     State of pause
     */     
    MPLAYER_AUD_PLAY_STATE_PAUSE,
    /*!
     State of preview
     */     
    MPLAYER_AUD_PLAY_STATE_PREVIEW,    
    /*!
     State of stop
     */     
    MPLAYER_AUD_PLAY_STATE_STOP,
    /*!
     State of speed play
     */     
    MPLAYER_AUD_PLAY_STATE_SPEED_PLAY,    
}mplayer_aud_play_state_t;

/*!
  private data
  */
typedef struct tag_mplayer_aud_filter_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    connect with lrc filter
    */
  mplayer_aud_in_pin_t m_in_pin;
  /*!
    connect with lrc filter
    */
  mplayer_aud_lrc_out_pin_t m_lrc_out_pin;  
  /*!
    connect with pic filter   
    */
  mplayer_aud_pic_out_pin_t m_pic_out_pin;
  /*!
    mplayer_aud_info_t
    */
  mplayer_aud_info_t aud_info;
  /*!
    file seq handle
    */
  FILE_SEQ_T *p_handle;
     /*!
    time jump
    */
  u32 timejump;
  /*!
    file name
    */ 
  u16 *p_file_name;
  /*!
    current playtime
    */
  u16 cur_play_time;
  /*!
    lrc_play_time
    */
  u16 lrc_play_time;
  /*!
    show logo
    */
  u32 b_music_logo_show : 1;  
  /*!
    show lrc
    */
  u32 b_music_lrc_show : 1;
  /*!
    first_frame
    */
  u32 first_frame : 1;
  /*!
    load media success
    */
  u32 b_load_meida_success : 1;
  /*!
    reserved
    */
  u32 reserved : 28;
  /*!
    source url
    */
  u8 *p_src_url;  
}mplayer_aud_filter_private_t;

/*!
  the mplayer_aud filter define
  */
typedef struct tag_mplayer_aud_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data buffer
    */
  mplayer_aud_filter_private_t private_data;
}mplayer_aud_filter_t;

#endif // End for __MPLAYER_AUD_FILTER_INTRA_H_

