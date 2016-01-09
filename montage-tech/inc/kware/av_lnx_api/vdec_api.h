/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LNX_VDEC_API_H_
#define __LNX_VDEC_API_H_

/*!
  xxxxxxxx
  */
#ifndef  BOOL
/*!
  xxxxxxxx
  */
#define BOOL  unsigned char 
#endif
/*!
  xxxxxxxx
  */
#define  H264_TYPE    (1)
/*!
  xxxxxxxx
  */
#define  MPEG_TYPE   (0)
/*!
  xxxxxxxx
  */
#define    VIDEO_ES_TMP_BUF_LEN  (128*1024)
/*!
  xxxxxxxx
  */
#define   VIDEO_ES_BUF_OVERFLOW_THRESHOLD  (100) // k bytes

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
    _RESUME,
    /*!
  xxxxxxxx
  */
    _MAX_STATUS,

}   vdec_play_status_t;
/*!
  xxxxxxxx
  */
typedef struct{
/*!
  xxxxxxxx
  */
   unsigned int     es_buf_len;
/*!
  xxxxxxxx
  */
    unsigned int    invalid_es_num;
/*!
  xxxxxxxx
  */
    unsigned int    es_free_space;

}ves_buf_info_t;

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
} inject_mode_t;

/*!
  xxxxxxxx
  */
typedef  struct{
/*!
  xxxxxxxx
  */
       void * p_mutex;
/*!
  xxxxxxxx
  */
       unsigned char    is_open;
/*!
  xxxxxxxx
  */
	int fd;
/*!
  xxxxxxxx
  */
	int                    codec_type;
/*!
  xxxxxxxx
  */
	inject_mode_t     inject_mode;
/*!
  xxxxxxxx
  */
	vdec_play_status_t  status;
/*!
  xxxxxxxx
  */
	unsigned char    valid_ves_buf_flag;
/*!
  xxxxxxxx
  */
	unsigned char * p_tmp_ves_buf;
/*!
  xxxxxxxx
  */
	unsigned int      tmp_ves_buf_pos;
/*!
  xxxxxxxx
  */
	unsigned int      tmp_ves_buf_len;
/*!
  xxxxxxxx
  */
  unsigned int *  p_dec_frame_count;
/*!
  xxxxxxxx
  */
  unsigned int *  p_sys_vpts;



} video_decoder_info;

/*!
  xxxxxxxx
  */
void *           vdec_api_create_handle();
/*!
  xxxxxxxx
  */
void              vdec_api_destroy_handle(void *p_handle);
/*!
  xxxxxxxx
  */
int    vdec_api_open_decoder(void * p_handle,char *p_dev_path);
/*!
  xxxxxxxx
  */
int           vdec_api_close_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int            vdec_api_start_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int           vdec_api_stop_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int           vdec_api_pause_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int           vdec_api_resume_decoder(void *p_handle);
/*!
  xxxxxxxx
  */
int   vdec_api_push_es(void *p_handle,unsigned char *p_data,int len,u32 vpts);
/*!
  xxxxxxxx
  */
void             vdec_api_clear_es_buf(void * p_handle);
/*!
  xxxxxxxx
  */
void             vdec_api_clear_video_buffer(void * p_handle);
/*!
  xxxxxxxx
  */
int               vdec_api_get_inject_mode(void * p_handle);
/*!
  xxxxxxxx
  */
 unsigned int                 vdec_api_get_video_pts(void * p_handle);
/*!
  xxxxxxxx
  */
int   vdec_api_enable_cpu_inject_mode(void *p_handle);
/*!
  xxxxxxxx
  */

BOOL             vdec_api_enable_av_sync(void *p_handle);
/*!
  xxxxxxxx
  */
BOOL             vdec_api_disable_av_sync(void *p_handle);
/*!
  xxxxxxxx
  */
void                vdec_api_get_frame_info(void * p_handle, void * p_frame_info);
/*!
  xxxxxxxx
  */
void                vdec_api_get_fw_requirement(void *p_handle,void *p_req);
/*!
  xxxxxxxx
  */
u32                 vdec_api_get_dec_frame_cnt(void *p_handle);
/*!
  xxxxxxxx
  */
u32                 vdec_api_get_vpts(void *p_handle);




#endif
