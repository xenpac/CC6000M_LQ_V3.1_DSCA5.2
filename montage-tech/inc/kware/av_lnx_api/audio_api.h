/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LNX_ADEC_API_H_
#define __LNX_ADEC_API_H_
/*!
  xxxxxxxx
  */
#include "aud_vsb.h"

/*!
  xxxxxxxx
  */
#define DBG_MT_AUD_DECODER
/*!
  xxxxxxxx
  */
#ifdef   DBG_MT_AUD_DECODER

#define MY_LOG(format, args...)              printf(format, ##args)
#define MY_WARNING(format, args...)     printf(format, ##args)
#define MY_DEBUG(format, args...)          printf(format, ##args)
#define MY_ERROR(format, args...)          printf(format, ##args)
#else
#define MY_LOG(format, args...)               printf(format, ##args)
#define MY_WARNING(format, args...)
#define MY_DEBUG(format, args...)
#define MY_ERROR(format, args...)           printf(format, ##args)
#endif
/*!
  xxxxxxxx
  */
#define    PTS_BASE (90 >> 2)
/*!
  xxxxxxxx
  */
#define     VALID_PTS_MASK     (0x80000000)
/*!
  xxxxxxxx
  */
typedef  enum{
/*!
  xxxxxxxx
  */
    _INVALID_STATUS,
    /*!
  xxxxxxxx
  */
    _PLAY_,
    /*!
  xxxxxxxx
  */
    _STOP_,
    /*!
  xxxxxxxx
  */
    _PAUSE_,
    /*!
  xxxxxxxx
  */
    _MAX_STATUS_,
    /*!
  xxxxxxxx
  */

}   adec_play_status_t;
/*!
  xxxxxxxx
  */
typedef  enum{
/*!
  xxxxxxxx
  */
   _pti_inject,
   /*!
  xxxxxxxx
  */
   _cpu_inject,
   /*!
  xxxxxxxx
  */
   _invalid_indject,
} aud_inject_mode_t;

/*!
  xxxxxxxx
  */
typedef  struct{
/*!
  xxxxxxxx
  */
	int fd;
/*!
  xxxxxxxx
  */
	int is_open;
/*!
  xxxxxxxx
  */
	int             codec_type;
/*!
  xxxxxxxx
  */
	adec_file_fmt_vsb_t  filefmt;
/*!
  xxxxxxxx
  */
	aud_inject_mode_t   inject_mode;
/*!
  xxxxxxxx
  */
	adec_play_status_t  status;
/*!
  xxxxxxxx
  */
	unsigned char * p_tmp_aes_buf; 
/*!
  xxxxxxxx
  */
	unsigned int       m_aes_left; 
/*!
  xxxxxxxx
  */
	unsigned int 	tmp_aes_buf_len;
/*!
  xxxxxxxx
  */
	unsigned int 	tmp_aes_buf_pos;

} audio_decoder_info;

/*!
  xxxxxxxx
  */
void *   adec_api_create_handle();
/*!
  xxxxxxxx
  */
void    adec_api_destroy_handle(void *p_handle);
/*!
  xxxxxxxx
  */
int adec_api_open_decoder(audio_decoder_info * handle,char * device_name);
/*!
  xxxxxxxx
  */
int    adec_api_close_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int   adec_api_enable_cpu_inject_mode(void *p_handle);
/*!
  xxxxxxxx
  */
int   adec_api_disable_cpu_inject_mode(void *p_handle);
/*!
  xxxxxxxx
  */
void  adec_api_set_codec_type(void *p_handle,int type);
/*!
  xxxxxxxx
  */
int  adec_api_start_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int  adec_api_stop_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int  adec_api_pause_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int  adec_api_resume_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int   adec_api_push_es(void *p_handle,unsigned char *p_data,int len,u32 apts);
/*!
  xxxxxxxx
  */
int   adec_api_get_input_es_buf_info(void *p_handle, u32 * p_free_size, u32 * p_total_size);
/*!
  xxxxxxxx
  */
int   adec_api_flush_input_buf(void *p_handle);
#endif



