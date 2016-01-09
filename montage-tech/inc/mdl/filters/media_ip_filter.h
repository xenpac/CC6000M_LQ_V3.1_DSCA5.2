/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MEDIA_IP_FILTER_H_
#define __MEDIA_IP_FILTER_H_


/*!
  file source filter command define
  */
typedef enum tag_media_ip_filter_cmd
{
  /*!
    config pid, for descrambe program
    */
  MEDIA_IP_CFG_PROGRAM_PID,
}media_ip_filter_cmd_t;

/*!
  media ip filter event define
  */
typedef enum tag_media_ip_filter_evt
{
  /*!
    media ip update
    */
  MEDIA_IP_MSG_EVT_UPDATE = MEDIA_IP_FILTER << 16,
  /*!
    media ip cilent disconnect
    */
  MEDIA_IP_MSG_CLINET_DISCONNECT
}media_ip_filter_evt_t;

/*!
  tag_record_filter_program_pid for descrambe the program 
  */
typedef struct tag_media_ip_filter_program_pid
{
  /*!
    video pid
    */
  u16 v_pid;
  /*!
    audio pid
    */
  u16 a_pid;
  /*!
    pcr pid
    */
  u16 pcr_pid;
  /*!
    pcr pid
    */
  u32 reserved :28;
  /*!
    pcr pid
    */
  u32 encrypt_flag : 3;
  /*!
    biss
    */
  u32 biss : 1;
  /*!
    audio_key_len
    */
  u32 audio_key_odd_len : 4;
  /*!
    audio_key_len
    */
  u32 audio_key_even_len : 4;
  /*!
    video_key_len
    */
  u32 video_key_odd_len : 4;
  /*!
    video_key_len
    */
  u32 video_key_even_len : 4;
  /*!
    audio odd key
    */
  u8 audio_key_odd[CAS_CH_KEY_MAX];
  /*!
    audio even key
    */
  u8 audio_key_even[CAS_CH_KEY_MAX];
  /*!
    video odd key
    */
  u8 video_key_odd[CAS_CH_KEY_MAX];
  /*!
    video even key
    */
  u8 video_key_even[CAS_CH_KEY_MAX];
  
}media_ip_filter_pg_set_t;

//typedef u16 (*ip_data_write_func)(u8 *p_data, u32 size);

/*typedef struct
{
  ip_data_write_func raw_write;
  
}media_ip_create_para_t;
*/

/*!
  create a record filter instance
  filter ID: MEDIA_IP_FILTER
  \return return the instance of record_filter_t
  */
ifilter_t * media_ip_filter_create(void *p_para);

#endif // End for __RECORD_FILTER_H_

