/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __MEDIA_IP_API_H_
#define __MEDIA_IP_API_H_



/*!
  MUL_ERR_MEDIA_IP_MODULE
  */
#define MUL_ERR_MEDIA_IP_MODULE (0x8080)
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_NOT_INIT                  ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0001) 
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_INVALID_PARA          ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0002)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_NMUL_PTR                   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0003)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_CHN_NOT_INIT          ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0004)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_INVALID_CHNID         ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0005)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_NO_CHN_LEFT           ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0006)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_ALREADY                   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0007)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_BUSY                        ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0008)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_NO_MEM                   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0009)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_NOT_SUPPORT           ((MUL_ERR_MEDIA_IP_MODULE << 16) + 000A)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_RETRY                       ((MUL_ERR_MEDIA_IP_MODULE << 16) + 000B)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_EXIST                ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0011)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_NOT_EXIST        ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0012)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_CANT_OPEN        ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0013)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_CANT_CLOSE       ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0014)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_CANT_SEEK          ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0015)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_CANT_WRITE        ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0016)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_CANT_READ          ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0017)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_INVALID_FNAME   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0018)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_TILL_START          ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0019)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_TILL_END              ((MUL_ERR_MEDIA_IP_MODULE << 16) + 001A)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_FILE_DISC_FULL            ((MUL_ERR_MEDIA_IP_MODULE << 16) + 001B)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_REC_INVALID_STATE       ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0021)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_REC_INVALID_DMXID     ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0022)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_REC_INVALID_FSIZE    ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0023)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_REC_INVALID_UDSIZE   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0024)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_PLAY_INVALID_STATE   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0031)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_PLAY_INVALID_DMXID   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0032)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_INDEX_CANT_MKIDX   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0041)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_INDEX_FORMAT_ERR   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0042) 
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_INDEX_DATA_ERR       ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0043)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_INTF_EVENT_INVAL   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0051)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_INTF_EVENT_NOREG   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0052)  

/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_EVA_FILTER_ADD_FAILED   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0061)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_EVA_FILTER_INSERT_CHN   ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0062)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_EVA_FILTER_NO_PIN          ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0062)  
/*!
  FIXME
  */
#define MUL_ERR_MEDIA_IP_EVA_CONNECT_FAILED     ((MUL_ERR_MEDIA_IP_MODULE << 16) + 0062)  



/*!
  FIXME
  */
#define MEDIA_IP_MAX_CIPHER_KEY_LEN (8)
/*!
  FIXME
  */
#define MEDIA_IP_MAX_FILENAME_LEN (256)


/*!
  FIXME
  */
typedef enum 
{
  /*!
    FIXME
    */
  MUL_MEDIA_IP_REC_INDEX_TYPE_NONE,      
  /*!
    FIXME
    */
  MUL_MEDIA_IP_REC_INDEX_TYPE_VIDEO,     
  /*!
    FIXME
    */
  MUL_MEDIA_IP_REC_INDEX_TYPE_AUDIO,     
  /*!
    FIXME
    */
  MUL_MEDIA_IP_REC_INDEX_TYPE_END       
}mul_media_ip_index_type_t;


/*!
  FIXME
  */
typedef enum  
{
  /*!
    FIXME
    */
  MUL_MIP_STREAM_TYPE_TS,
  /*!
    FIXME
    */
  MUL_MIP_STREAM_TYPE_PES,
  /*!
    FIXME
    */
  MUL_MIP_STREAM_TYPE_ALL_TS,
  /*!
    FIXME
    */
  MUL_MIP_STREAM_TYPE_OTHER,
  /*!
    FIXME
    */
  MUL_MIP_STREAM_TYPE_END  
}mul_media_ip_stream_type_t;

#if 0
/*!
  FIXME
  */
typedef enum
{
  /*!
    DES算法  
    */ 
  MUL_CIPHER_ALG_DES,
  /*!
    3DES算法  
    */ 
  MUL_CIPHER_ALG_3DES,
  /*!
    AES算法  
    */ 
  MUL_CIPHER_ALG_AES,
  /*!
    end
    */ 
  MUL_CIPHER_ALG_END   
}mul_cipher_alg_t;

/*!
  FIXME
  */
typedef struct 
{
  /*!
    是否进行加密或者解密，此项配置为HI_FALSE时，其他项没有意义 
    */ 
  BOOL bDoCipher;
  /*!
    加密或者解码的算法类型 
    */ 
  mul_cipher_alg_t enType;
  /*!
    密钥 
    */ 
  u8 au8Key[MEDIA_IP_MAX_CIPHER_KEY_LEN];
  /*!
    密钥长度  
    */
  u32 u32KeyLen; 
}mul_pvr_cipher_t;
#else
/*!
  tag_mul_pvr_cipher
  */
typedef struct tag_mul_media_ip_cipher
{
  /*!
    reserved
    */
  u32 reserved : 15;
  /*!
    is biss key
    */
  u32 is_biss_key : 1;
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
  u8 audio_key_odd[MEDIA_IP_MAX_CIPHER_KEY_LEN];
  /*!
    audio even key
    */
  u8 audio_key_even[MEDIA_IP_MAX_CIPHER_KEY_LEN];
  /*!
    video odd key
    */
  u8 video_key_odd[MEDIA_IP_MAX_CIPHER_KEY_LEN];
  /*!
    video even key
    */
  u8 video_key_even[MEDIA_IP_MAX_CIPHER_KEY_LEN];
}mul_media_ip_cipher_t;

#endif
/*!
  FIXME
  */
typedef struct 
{
  /*!
    FIXME
    */
  u16 v_pid; 
  /*!
    FIXME
    */
  u16 a_pid; 
  /*!
    FIXME
    */
  u16 pcr_pid;
  /*!
    FIXME
    */
  u16 pmt_pid;
  /*!
    FIXME
    */
  u16 reserved;
  /*!
    FIXME
    */
  u16 extern_num;
  /*!
    FIXME
    */
  extern_pid_t extern_pid[REC_MAX_EXTERN_PID];
}mul_media_ip_media_t;


/*!
  FIXME
  */
typedef struct tag_media_ip_attr
{
  /*!
    FIXME
    */
  mul_media_ip_media_t media_info;
  /*!
    FIXME
    */
  u32 demux_index; 
  /*!
    FIXME
    */
  u32 stk_size;
  /*!
    FIXME
    */
  u32 user_data_size; 
  /*!
    FIXME
    */
  BOOL b_clear_stream;
  /*!
    FIXME
    */
  mul_media_ip_stream_type_t stream_type; 
  /*!
    rec buffer, align 8
    */
  u8 *p_rec_buffer;
  /*!
    rec min size(800 * 188 * 6)
    */
  u32 rec_buffer_size;
  /*!
    biss key cfg
    */
  mul_media_ip_cipher_t  key_cfg;
  /*!
  ts in parameter
  */
  u8 ts_in;
}mul_media_ip_attr_t;

/*!
  FIXME
  */
typedef enum 
{
  /*!
    FIXME
    */
  MUL_MEDIA_IP_REC_STATE_INVALID,        
  /*!
    FIXME
    */
  MUL_MEDIA_IP_STATE_CREATED,           
  /*!
    FIXME
    */
  MUL_MEDIA_IP_STATE_RUNNING,        
  /*!
    FIXME
    */
  MUL_MEDIA_IP_STATE_PAUSE,          
  /*!
    FIXME
    */
  MUL_MEDIA_IP_STATE_STOPPING,       
  /*!
    FIXME
    */
  MUL_MEDIA_IP_STATE_STOP,           
  /*!
    FIXME
    */
  MUL_MEDIA_IP_STATE_END
}mul_media_ip_state_t;


/*!
  FIXME
  */
typedef struct tag_media_ip_status
{
  /*!
  通道所处的状态。 
    */
  mul_media_ip_state_t state; 
  /*!
  当前录制文件的大小（单位：kbyte）。 
    */
 
  u32 cur_write_point; 
  /*!
  当前录制的帧数。 
    */
  u32  cur_write_frame;
  /*!
  当前录制的毫秒数。其值为录制文件的实际有效长度。 
    */
 
  u32 cur_time_ms;
  /*!
  录制文件的实际起始点。录制回绕前，为第一帧时间；录制回绕后，为回绕点的时间。 
    */
  u32 start_time_ms;
  /*!
  录制文件中结束帧的时间，没有环回前即最后一帧的时间。 
    */
 
  u32 end_time_ms;
}mul_media_ip_status_t;



/*!
  FIXME
  */
typedef enum
{
  /*!
    FIXME
    */
  MUL_MEDIA_IP_EVENT_PLAY,
  /*!
    FIXME
    */
  MUL_MEDIA_IP_EVENT_CLIENT_DISCONNECT,
  /*!
    FIXME
    */
  MUL_MEDIA_IP_EVENT_END
}mul_media_ip_event_t;




/*!
  FIXME
  */
typedef void(*media_ip_call_back)(u32 chnid,
                                mul_media_ip_event_t event_type, s32 event_value, void *p_args);

/*!
  FIXME
  */
RET_CODE mul_media_ip_init(u8 rtsp_prio, u32 rtsp_stack, u32 task_num);
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_create_chn(u32 *p_chanid, const mul_media_ip_attr_t *p_rec_attr); 
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_destroy_chn(u32 chanid); 
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_set_chn(u32 chanid, const mul_media_ip_attr_t *p_rec_attr); 
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_get_chn(u32 chanid, mul_media_ip_attr_t *p_rec_attr); 
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_start_chn(u32 chanid); 

/*!
  FIXME
  */
RET_CODE mul_media_ip_pause_chn(u32 chanid);

/*!
  FIXME
  */
RET_CODE mul_media_ip_resume_chn(u32 chanid);

/*!
  FIXME
  */
RET_CODE mul_media_ip_change_pid(u32 chanid, mul_media_ip_media_t *p_media);

/*!
  FIXME
  */
RET_CODE mul_media_ip_stop_chn(u32 chanid); 
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_get_status(u32 chanid, mul_media_ip_status_t *p_rec_status); 
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_register_event(u32 chanid, media_ip_call_back callBack, void *p_args); 
 
/*!
  FIXME
  */
RET_CODE mul_media_ip_unregister_event(u32 chanid); 

/*!
  FIXME
  */
RET_CODE mul_media_ip_register_sys_cb(media_ip_call_back callBack, void *p_args);

#endif //__MEDIA_IP_API_H_

