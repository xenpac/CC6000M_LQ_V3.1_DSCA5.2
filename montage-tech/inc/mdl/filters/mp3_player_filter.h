/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MP3_PLAYER_FILTER_H_
#define __MP3_PLAYER_FILTER_H_

/*!
  forward speed
  */
#define FORWARDSPEED 2

/*!
  max count
  */
#define CNTMAX 256

/*!
  size per request
  */
#define SIZEREQ (50 * 1024)

/*! 
  mp3 play mode 
  */
typedef enum tag_mp3_player_play_mode
{
  /*!
    normal play
    */
  MP3_NORMAL_PLAY = 0,
  /*!
    fast forward
    */
  MP3_FAST_FORWARD,
  /*!
    fast rewind
    */  
  MP3_FAST_BACKWARD,
  /*!
    time seek
    */
  MP3_TIME_SEEK,
  /*!
    play resume
    */
  MP3_PLAY_RESUME,
  /*!
    play pause
    */
  MP3_PLAY_PAUSE,
}mp3_play_mode_t;

/*!
  mp3 player filter command define
  */
typedef enum tag_mp3_player_filter_cmd
{
  /*!
    config ...
    */
  MP3_PLAYER_CFG,
 /*!
   config play filename
  */ 
  MP3_CFG_NAME,
  /*!
    push mp3 logo again
  */
  MP3_PUSH_LOGO,
  /*!
    is or not push mp3 logo
  */  
  MP3_SHOW_LOGO,
  /*!
    is or not push mp3 lrc
  */   
  MP3_SHOW_LRC,
 /*!
   config srouce url
  */
  MUSIC_CFG_URL,
 /*!
   config head info
  */
  MP3_CFG_HEAD_INFO,  
  /*!
    end
    */
  MP3_PLAYER_CFG_END,
}mp3_player_filter_cmd_t;

/*!
  Audio render filter event define
  */
typedef enum tag_audio_render_evt
{
  /*!
    config read file' name
    */
  AUDIO_FILE_PLAY_START = MP3_DECODE_FILTER << 16,
  /*!
    config read file' name
    */
   // AUDIO_FILE_PLAY_END,
  AUDIO_FILE_PLAY_END,
  /*!
    get current play time
    */
  AUDIO_GET_PLAY_TIME,
  /*!
    audio data error
    */
  AUDIO_GET_DATA_ERROR,
  
}audio_render_evt_t;

/*!
  create a file source instance
  filter ID:MP3_TRANSFER_FILTER
  \return return the instance of mp3_trans_filter_t
  */
ifilter_t * mp3_transfer_filter_create(void *p_para);

/*!
  create a file mp3 decode filter instance
  filter ID:MP3_DECODE_FILTER
  \return return the instance of fsrc_filter
  */
ifilter_t * mp3_decode_filter_create(void *p_para);

#endif // End for __MP3_PLAYER_FILTER_H_

