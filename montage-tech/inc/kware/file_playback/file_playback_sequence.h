/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef  __FILE_PALYBACK_SEQ_H_
#define  __FILE_PALYBACK_SEQ_H_
#ifdef __cplusplus
extern "C" {
#endif
    /*!
    *
    */
    typedef enum {
        /*!
          xxxxxxxx
          */
        SEND_MODE,
        /*!
        xxxxxxxx
        */
        RECV_MODE,
        /*!
        xxxxxxxx
        */
        INVALID_MODE,

    }
    NW_PVR_WORK_MODE;
    /*!
    *
    */
    typedef enum {
        /*!
        xxxxxxxx
        */
        SA_FILL_TS_START,
        /*!
        xxxxxxxx
        */
        SA_FILL_TS_PLAY,
        /*!
        xxxxxxxx
        */
        SA_FILL_TS_STOP,
        /*!
        xxxxxxxx
        */
        SA_FILL_TS_INVALID_MODE,

    } SA_FILL_TS_MODE;
    /*!
    *
    */
#define  MAX_PATH_NUM  (128)
    /*!
      xxxxxxxx
      */
    typedef enum {
        /*!
          xxxxxxxx
          */
        FILE_SEQ_UNKNOWN,

        /*!
          xxxxxxxx
          */
        FILE_SEQ_STOP,
        /*!
          xxxxxxxx
          */
        FILE_SEQ_PLAY,
        /*!
        xxxxxxxx
        */
        FILE_SEQ_PLAYED,
        /*!
          xxxxxxxx
          */
        FILE_SEQ_PAUSE,
        /*!
        xxxxxxxx
        */
        FILE_SEQ_LOADMEDIA,
        /*!
        xxxxxxxx
        */
        FILE_SEQ_LOADED,
        /*!
          xxxxxxxx
          */
        FILE_SEQ_RESUME,
        /*!
          xxxxxxxx
          */
        FILE_SEQ_PREVIEW,
        /*!
        xxxxxxxx
        */
        FILE_SEQ_EXIT,

        /*!
        xxxxxxxx
        */
        FILE_SEQ_STATUS_MAX,
    } FILE_SEQ_STATUS;
    /*!
    xxxxxxxx
    */
    typedef struct {
        /*!
        xxxxxxxx
        */
        int  type;
        /*!
        xxxxxxxx
        */
        u32 param;

    } FP_USER_CMD_T;

    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        int totalAudioTrack;
        /*!
          xxxxxxxx
          */
        int totalVideoTrack;
        /*!
          xxxxxxxx
          */
        unsigned char ** pp_audio_type;
        /*!
          xxxxxxxx
          */
        unsigned char ** pp_video_type;
        /*!
          xxxxxxxx
          */
        unsigned long long  totalDuration;

    } MEDIA_INFO;
    /*!
      xxxxxxxx
      */
    typedef enum {
        /*!
          xxxxxxxx
          */
        STOP_PLAYBACK = 0,
        /*!
          xxxxxxxx
          */
        PAUSE_PLAYBACK = 1,
        /*!
          xxxxxxxx
          */
        RESUME_PLAYBACK = 2,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FF2X = 3,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FF4X = 4,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FF8X = 5,
        /*!
          xxxxxxxx
          */
        TRICK_PALY_FF16X = 6,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FF32X = 7,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FF64X = 8,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FR2X = 9,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FR4X = 10,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FR8X = 11,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FR16X = 12,
        /*!
          xxxxxxxx
          */
        TRICK_PLAY_FR32X = 13,
        /*!
          xxxxxxxx
          */
        INVALID_USER_CMD ,

    } USER_CMD_TYPE;
    /*!
      xxxxxxxx
      */
    typedef  struct {
        /*!
          xxxxxxxx
          */
        unsigned int   max_ves_num;
        /*!
          xxxxxxxx
          */
        unsigned int   max_aes_num;

    } AV_DEC_CAP;
    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        u32   film_duration;
        /*!
          xxxxxxxx
          */
        int    audio_type;
        /*!
          xxxxxxxx
          */
        int    video_type;
        /*!
          xxxxxxxx
          */
        int    audio_track_num;
        /*!
          xxxxxxxx
          */
        int    video_track_num;
        /*!
          xxxxxxxx
          */
        BOOL  canTrickPlay;
        /*!
          xxxxxxxx
          */
        int video_disp_w;
        /*!
        xxxxxxxx
        */
        int video_disp_h;
        /*!
        xxxxxxxx
        */
        int video_fps;
        /*!
        xxxxxxxx
        */
        int  video_bps;//bytes per secondes
        /*!
          xxxxxxxx
          */
        u64 file_size;
		/*!
        xxxxxxxx
        */
		int audio_track_id; 
		/*!
        xxxxxxxx
        */
		char * audio_language; // pointers to a modification forbidden memory space
		/*!
        xxxxxxxx
        */
		int audio_bps; 
		/*!
        xxxxxxxx
        */
		int audio_samplerate; 
		/*!
        xxxxxxxx
        */
		unsigned char * file_name; // pointers to a modification forbidden memory space
    } FILM_INFO_T;
    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        int video_disp_w;
        /*!
          xxxxxxxx
          */
        int video_disp_h;
        /*!
          xxxxxxxx
          */
        int video_fps;

    } VIDEO_W_H_FPS;

    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        int track_id;
        /*!
          xxxxxxxx
          */
        char * lang;
        /*!
          xxxxxxxx
          */
        char * title;
        /*!
          format in mp
          */
        unsigned int format;
    } TRACK_LANG;
 /*!
      xxxxxxxx
      */
    typedef enum {
        /*!
          load err unknow, this is the init value
          */
        ERR_LOAD_UNKNOW,
        /*!
          play path null
          */
        ERR_LOAD_NAME_NULL,
        /*!
          last loading is still running
          */
        ERR_LOAD_LAST_LOAD_RUNNING,
        /*!
          dns error, couldn't get the ip addr
          */
        ERR_LOAD_RESLV_HOSTNAME_ERR,
        /*!
          tcp connect error
          */
        ERR_LOAD_CONNECT_ERR,
        /*!
          open stream error, this is a general reason
          */
        ERR_LOAD_OPEN_STREAM_ERR,
        /*!
          open demux error, this is a general reason
          */
        ERR_LOAD_DEMUX_ERR,
        /*!
          unsupport av codec type
          */
        ERR_LOAD_AV_UNSUPPORT,
	 
 
    } ERR_LOAD;
    
     /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        ERR_LOAD err_type;
        /*!
          xxxxxxxx
          */
        char url[4096];
    } LOAD_ERR_INFO;

    /*!
      define the type of event delived to app
      */
    typedef enum {
        /*!
          when play to the end of the file, send this event
           app may look at this event as 'EOF'
          */
        FILE_PLAYBACK_SEQ_STOP = 0,
        /*!
        when play to the end of the file, send this event
        app may look at this event as 'EOF'
        */
        FILE_PLAYBACK_SEQ_ABNORMAL_STOP = 0,
        /*!
        display info has updated, such as high and width of picture
        */
        FILE_PLAYBACK_SEQ_RECEIVE_VIDEO_INFO ,
        /*!
           new video pts arrive
          */
        FILE_PLAYBACK_SEQ_GET_VPTS ,
        /*!
          fail to excute the action 'loadmedia'
        */
        FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT,
        /*!
           no use (reserved for future !!!!!)
         */
        FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR ,
        /*!
         success to loadmedia
        */
        FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS ,
        /*!
        no use (reserved for future !!!!!)
        */
        FILE_PLAYBACK_SEQ_NOT_READY ,
        /*!
          player can't support this type of audio codec
         */
        FILE_PLAYBACK_UNSUPPORT_AUDIO ,
        /*!
         player  can't support this type of video codec
         */
        FILE_PLAYBACK_UNSUPPORT_VIDEO ,
        /*!
         player is now in trick play mode
         */
        FILE_PLAYBACK_CHECK_TRICKPLAY ,
        /*!
         no enough memory for decoding such type of video es
         */
        FILE_PLAYBACK_UNSUPPORT_MEMORY ,
        /*!
          new subtitle data has arrived
         */
        FILE_PLAYBACK_NEW_SUB_DATA_RECEIVE ,
        /*!
         can't support the function 'seek'
         */
        FILE_PLAYBACK_UNSUPPORT_SEEK ,
        /*!
         can't support the function 'trick play'
         */
        FILE_PLAYBACK_UNSUPPORT_TRICK ,
        /*!
          the thread 'fill es' was killed successfully
         */
        FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT,
        /*!
         the thread 'fill es' was killed before
        */
        FILE_PLAYBACK_SEQ_ES_TASK_DEAD,

        /*!
          pause av decoder and continue to buffer data
         */
        FILE_PLAYBACK_SEQ_START_BUFFERING,
        /*!
        seek finished
        */
        FILE_PLAYBACK_SEEK_FINISHED,
        /*!
        resume av decoder
        */
        FILE_PLAYBACK_SEQ_FINISH_BUFFERING,
        /*!
        current download speed
        */
        FILE_PLAYBACK_SEQ_UPDATE_BPS,
        /*!
        * es water level is too low and notify up layer change source
        */
        FILE_PLAYBACK_SEQ_REQUEST_CHANGE_SRC,

        /*!
        * fail to set current path so app can try to set this path again
        */
        FILE_PLAYBACK_SEQ_SET_PATH_FAIL,

        /*!
        * update total seconds for loading media
        */
        FILE_PLAYBACK_SEQ_LOAD_MEDIA_TIME,

        /*!
        * notify upper layer that player has already finish updating bps
        */
        FILE_PLAYBACK_FINISH_UPDATE_BPS,
        /*!
        * notify upper layer that video decoder unsupport current hd video
        */
        FILE_PLAYBACK_HD_UNSUPPORT,
        /*!
        * the container don't parser
        */
        FILE_PLAYBACK_UNSUPPORT_CONTAINER,
        /*!
          invalid type
          */
        FILE_PLAYBACK_SEQ_INVALID,

    } FILE_PLAY_EVENT_E;
    typedef enum {
        /*!
          xxxxxxxx
          */
        vUNKNOWN        = -1,
        /*!
          xxxxxxxx
          */
        vVIDEO_MPEG1    = 0,
        /*!
          xxxxxxxx
          */
        vVIDEO_MPEG2,
        /*!
          xxxxxxxx
          */
        vVIDEO_MPEG4,
        /*!
          xxxxxxxx
          */
        vVIDEO_H264,
        /*!
          xxxxxxxx
          */
        vVIDEO_AVC,
        /*!
          xxxxxxxx
          */
        vVIDEO_DIRAC,
        /*!
          xxxxxxxx
          */
        vVIDEO_VC1,
        /*!
          xxxxxxxx
          */
        vVIDEO_VP8,
        vVIDEO_RV30,
        vVIDEO_RV40,
    } ves_stream_type_t;
    typedef enum {
        /*!
          xxxxxxxx
          */
        aUNKNOWN        = -1,
        /*!
          xxxxxxxx
          */
        aAUDIO_MP2 = 0,
        /*!
          xxxxxxxx
          */
        aAUDIO_AC3,
        /*!
          xxxxxxxx
          */
        aAUDIO_DTS,
        /*!
          xxxxxxxx
          */
        aAUDIO_LPCM_BE,
        /*!
          xxxxxxxx
          */
        aAUDIO_AAC,
        /*!
          xxxxxxxx
          */
        aAUDIO_AAC_LATM,
        /*!
          xxxxxxxx
          */
        aAUDIO_TRUEHD,
        /*!
          xxxxxxxx
          */
        aAUDIO_S302M,
        /*!
          xxxxxxxx
          */
        aAUDIO_PCM_BR,
    } aes_stream_type_t;
    typedef enum {
	    /*!
	      xxxxxxxx
	     */
	    VDEC_STOP = 0,
	    /*!
	      xxxxxxxx
	     */
	    VDEC_FREEZE_STOP = 1,
	    /*!
	      xxxxxxxx
	     */
	    VDEC_SWITCH_CH_STOP = 2,
    } vdec_stop_type_t;
    /*!
      xxxxxxxx
      */
    typedef struct file_time {
        /*!
          xxxxxxxx
          */
        short Year;
        /*!
        xxxxxxxx
        */
        char     Month;
        /*!
        xxxxxxxx
        */
        char  Day;
        /*!
        xxxxxxxx
        */
        char  Hour;
        /*!
        xxxxxxxx
        */
        char  Minute;
        /*!
        xxxxxxxx
        */
        char  Second;
    } FILE_TIME;


    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        unsigned int      p_vdec_start;
        /*!
          xxxxxxxx
          */
        unsigned int      vdec_mem_size;
        /*!
          xxxxxxxx
          */
        unsigned  int     p_audio_start;
        /*!
          xxxxxxxx
          */
        unsigned int      audio_mem_size;

    } FW_MEM_CFG_T;

    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_loadmedia_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_loadmedia_times_fun_t)(void *, int);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_unloadmedia_fun_t)(void *);
    /*!
      * FUNCTION:         start player
      * PARAM:              PARAM1 refer to handle, PARAM2 refer to play position (seconds)
      * RETURN VALUE:   0 means ok; other means fail
      */
    typedef  int (* file_seq_start_fun_t)(void *, unsigned int);
    /*!
      FUNCTION:      requir player to stop
      PARAM1:         handle to file_playback_seq
      NOTICE:          stop will be excuted in asyncchrous mode

      */
    typedef  int (* file_seq_stop_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_pause_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_resume_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_set_speed_fun_t)(void *, int);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_preview_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  unsigned long int (* file_seq_get_vpts_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  unsigned long int (* file_seq_play_at_time_fun_t)(void *, int);
    /*!
      xxxxxxxx
      */
    typedef  unsigned long int (* file_seq_get_status_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_change_audio_track_fun_t)(void *, int);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_get_audio_track_lang_fun_t)(void *, TRACK_LANG *);
    /*!
      xxxxxxxx
      */
    typedef  int (* file_seq_change_video_track_fun_t)(void *, int);
    /*!
     */
    typedef  BOOL (* file_seq_get_film_info_fun_t)(void *, FILM_INFO_T *);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_set_file_path_fun_t)(void *, char *);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_set_file_path_ex_fun_t)(void *, char *, u32);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_set_muti_file_path_fun_t)(void *, char *[], u32);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_av_codec_type_fun_t)(void *, int , int);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_do_cmd_fun_t)(void *, int);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_mp_dump_fun_t)(void);
    /*!
      xxxxxxxx
      */
    typedef  void (* file_seq_run_mem_fun_t)(BOOL);
    /*!
      xxxxxxxx
      */
    typedef  void (*file_seq_mem_init_fun_t)(void *, u8 *, u32);
    /*!
      xxxxxxxx
      */
    typedef  void             *            (*file_seq_mem_alloc_fun_t)(void *, u32);
    /*!
      xxxxxxxx
      */
    typedef  void (*file_seq_mem_free_fun_t)(void *, void *);
    /*!
      xxxxxxxx
      */
    typedef void (*file_seq_mem_release_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef void (* file_seq_get_subt_info_fun_t)(void * , void **);
    /*!
      xxxxxxxx
      */
    typedef void (* file_seq_set_subt_id_fun_t)(void *, int);
    /*!
      xxxxxxxx
      */
    typedef  int (*file_seq_get_subt_fun_t)(void *, unsigned char **, int *);
    /*!
      xxxxxxxx
      */
    typedef  int (*file_seq_get_exsubt_fun_t)(void *, unsigned char **, int *);
    /*!
      xxxxxxxx
      */
    typedef RET_CODE(*file_seq_event_cb_fun_t)(FILE_PLAY_EVENT_E, u32);

    /*!
      xxxxxxxx
      */
    typedef   void (*file_seq_register_event_cb_func_t)(void *, void *);

    /*!
      xxxxxxxx
      */
    typedef   void (*file_seq_set_vfmt_fun_t)(void *, BOOL);
    /*!
      xxxxxxxx
      */
    typedef  BOOL (*isNetworkStream_fun_t)(void *);
    /*!
      xxxxxxxx
      */
    typedef  void (*file_seq_set_live_broadcast_fun_t)(void *, BOOL);
    /*!
      xxxxxxxx
      */
    typedef  BOOL (*file_seq_check_bg_task_alive)(void *);
    /*!
      xxxxxxxx
      */
    typedef  void (*file_seq_set_ott_playmode_t)(void *, int);
    /*!
      xxxxxxxx
     */
    typedef  void (*file_seq_set_vdec_stop_type_fun_t)(void *, unsigned int);
	

    typedef  BOOL (*http_request_url_t)(void *, u8 *, u8);


    /*!
      xxxxxxxx
      */
#define FILE_SEQ_SUBTITLE_LEN 20
    /*!
      xxxxxxxx
      */
#define FILE_SEQ_SUBTITLE_CNT 20
    /*!
        video decoder fetch bytes of ves every time
      */
#define  VES_SEG_LEN   (1024)
    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        char lang[FILE_SEQ_SUBTITLE_LEN];
        /*!
        xxxxxxxx
        */
        char title[FILE_SEQ_SUBTITLE_LEN];
        /*!
        xxxxxxxx
        */
        char code[FILE_SEQ_SUBTITLE_LEN];
        /*!
        xxxxxxxx
        */
        int    id;
    } file_seq_subtitle_t;

    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
        xxxxxxxx
        */
        int cnt;
        /*!
        xxxxxxxx
        */
        file_seq_subtitle_t subtitle[FILE_SEQ_SUBTITLE_CNT];
    } file_seq_video_subtitle_t;

    /*!
      xxxxxxxx
      */
#define  LOCAL_FILE_PATH_LEN  (1024*4)
    /*!
      xxxxxxxx
      */
#define  AUD_TMP_ESBUF_LEN   (70)
    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        int size;
        /*!
          xxxxxxxx
          */
        int pts;
        /*!
          xxxxxxxx
          */
        unsigned char * data;

    } SUBT_DATA;


    /*!
      xxxxxxxx
      */
    typedef enum {

        AUDIO_LPCM = 0,

        AUDIO_BYPASS = 1,

    } AUDIO_OUT_MODE;

    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        unsigned int   pb_seq_mem_size;
        /*!
        xxxxxxxx
        */
        unsigned int   pb_seq_mem_start;
        /*!
        xxxxxxxx
        */
        unsigned int   stack_start;
        /*!
        xxxxxxxx
        */
        unsigned int   stack_size;
        /*!
        xxxxxxxx
        */
        unsigned int   task_priority;
        /*!
        xxxxxxxx
        */
        unsigned int   net_task_priority;
        /*!
        xxxxxxxx
        */
        unsigned int   http_task_priority;

        /*!
        xxxxxxxx
        */
        unsigned int   stack_preload_size;

         /*!
        xxxxxxxx
        */
        unsigned int   stack_pbfifo_size;

        /*!
          xxxxxxxx
          */
        unsigned int   task_preload_priority;

        /*!
          xxxxxxxx
          */
        unsigned int   task_pbfifo_priority;
        
        /*!
          xxxxxxxx
          */
        AUDIO_OUT_MODE audio_output_mode;
        /*!
          xxxxxxxx
          */
        unsigned int  preload_audio_buffer_size;
        /*!
          xxxxxxxx
          */
        unsigned int  preload_video_buffer_size;
        /*!
          xxxxxxxx
          */
        void * preload_audio_buffer;
        /*!
          xxxxxxxx
          */
        void * preload_video_buffer;
        /*!
         xxxxxxxx
         */
        unsigned char   is_direct_url;

        /*!
        real type is vdec_buf_policy_t
        */
        unsigned int   vdec_policy;
        /*!
         xxxxxxxx
         */
        int (*camera_open)(char * name);

        /*!
        xxxxxxxx
        */
        int (*camera_read)(int fd, char * buffer, int buffer_len);

        /*!
         xxxxxxxx
         */
        int (*camera_close)(int fd);
        /*!
            xxxxxxxx
            */
        int task_idle;
	/*!
	  hls_server_prio add by zhouxf(445297005@qq.com)
	 */
	u32 hls_server_prio;
	/*!
	  xxxxxxx
	 */
	u32 hls_server_size;
	/*!
	  p2p_server_recv_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_recv_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_recv_size;
	/*!
	  p2p_server_deal_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_deal_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_deal_size;
	/*!
	  p2p_server_send_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_send_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_send_size;
	/*!
	  p2p_server_monitor_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_monitor_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_monitor_size;
	/*!
	  p2p_server_bw_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_bw_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_bw_size;		
	/*!
	  xxxxxxx
	 */
	u32 vod_heartbeat_login_prio;
	/*!
	  xxxxxxx
	 */	
	u32 vod_heartbeat_login_size;
    } PB_SEQ_PARAM_T;

    /*!
      xxxxxxxx
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        u8 * buffer_start;
        /*!
          xxxxxxxx
          */
        u32 buffer_len;
        /*!
          xxxxxxxx
          */
        u32 write_pos;
        /*!
          xxxxxxxx
          */
        u32 read_pos;
    } preload_buffer_t;

    /*!
      xxxxxxxx
      */
    typedef struct es_pkt_data_s {
        /*!
          xxxxxxxx
          */
        u32 index;
        /*!
          xxxxxxxx
          */
        struct es_pkt_data_s * next_pkt;
        /*!
          xxxxxxxx
          */
        struct es_pkt_data_s * prev_pkt;
        /*!
          xxxxxxxx
          */
        u32  eof;
        /*!
          xxxxxxxx
          */
        double pts;
        /*!
          xxxxxxxx
          */
        u32 data_len;
        /*!
          xxxxxxxx
          */
        u32 extra_data_len;
        /*!
          xxxxxxxx
          */
        u8 * data;
        /*!
          xxxxxxxx
          */
        u8 * extra_data;
    } es_pkt_data_t;

    /*!
      xxxxxxxx
      */
    typedef enum {
        /*!
          xxxxxxxx
          */
        OTT_VOD_MODE = 3,
        /*!
          xxxxxxxx
          */
        OTT_LIVE_MODE,
        /*!
          xxxxxxxx
          */
        OTT_INVALID_MODE,

    } OTT_PLAY_MODE_T;
    /*!
      xxxxxxxx
      */
    typedef enum {
        /*!
          xxxxxxxx
          */
        STREAM_LIVE,

        /*!
          xxxxxxxx
          */
        STREAM_VOD,
        /*!
          xxxxxxxx
          */
        STREAM_TYPE_MAX,

    } FILE_STREAM_TYPE;
    /*!
      xxxxxxxx
      */
    typedef enum {
        /*!
          xxxxxxxx
          */
        CHECK_ES_FALSE        = 0,
        /*!
        xxxxxxxx
        */
        CHECK_ES_TRUE,
        /*!
          xxxxxxxx
          */
        CHECK_ES_RUNNING,
        /*!
          xxxxxxxx
          */
        CHECK_ES_IDLE,
        /*!
        xxxxxxxx
        */
        CHECK_ES_EXIT,

    } CHECK_ES_STATE;

 /*!
      HLS url
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        char url[4096];
    } HLS_URL;

 /*!
    HLS load status
  */
typedef enum
{
  /*!
    UNLOAD
    */
  HLS_STATUS_UNLOAD = 0,
  /*!
    FILE SEQ NEED TO LOAD
    */
  HLS_STATUS_NEED_TO_LOAD = 1,
  /*!
    FILE SEQ LOAD SUCCESS
    */
  HLS_STATUS_LOAD_SUCCESS = 2,
    /*!
    FILE SEQ LOAD SUCCESS
    */
  HLS_STATUS_LOAD_FINISHED = 3,

} HLS_LOAD_STATUS;

 /*!
      HLS parser result
      */
    typedef struct {

       /*********************************
       this below is modified by both side
       **********************************/
       
       /*!
          xxxxxxxx
          */
        HLS_LOAD_STATUS ts_load_status;
       
        /*!
          xxxxxxxx
          */
        int cur_seq_num;    //init inside


       /***************************************
       this below is modified only by hls inside
       ***************************************/

        /*!
          xxxxxxxx
          */
        int enable;             //init inside
        
       
        /*!
          xxxxxxxx
          */
        int is_live;
        /*!
          xxxxxxxx
          */
        int start_seq_num;       
       /*!
          xxxxxxxx
          */
        HLS_URL m3u8_ur;
        /*!
          xxxxxxxx
          */
        int url_num;       
        /*!
          xxxxxxxx
          */
        HLS_URL * ts_url;

    } HLS_PARSER_RESULT;

        /*!
      xxxxxxxx
      */
    typedef enum {
	/*!
          xxxxxxxx
          */
        HTTP_INT        = 0,    
        /*!
          xxxxxxxx
          */
        HTTP_CONNECT,

        /*!
          xxxxxxxx
          */
        HTTP_RECV,
        /*!
          xxxxxxxx
          */
        HTTP_STOP,
        /*!
          xxxxxxxx
          */
        HTTP_FAILED,
        /*!
          xxxxxxxx
          */
        HTTP_RECONNECT,

    } PB_FIFO_HTTP_STATE;
    /*!
      xxxxxxxx
      */
    typedef struct {

        /*!
          save file aboslute path
          */
        unsigned char    *   m_path[MAX_PATH_NUM];
        /*!
           path id: app will use this path id and driver will not use it
          */
        u32    path_id;
        /*!
           pointer to the 'BUFFERING_PARAM_T' structure
          */
        void  * p_buffering;
        /*!
           stream type
        */
        FILE_STREAM_TYPE stream_type;
        /*!
           path id: app will use this id
          */
        BOOL  is_stable;
        /*!
          point to audio es packet link list
          */
        es_pkt_data_t * p_audio_buf;
        /*!
          point to video es packet link list
          */
        es_pkt_data_t * p_video_buf;
        /*!
          point to tail node of audio es packet link list
          */
        es_pkt_data_t * p_audio_buf_tail;
        /*!
          point to tail node of video es packet link list
          */
        es_pkt_data_t * p_video_buf_tail;

        /*!
        *  length of video es buffer, used by video decoder
        *
        */
        u32 max_ves_size;

        /*!
          total path
          */
        int total_path;
        /*!
        forced_stop flag
        */
        int force_stopped;
        /*!
          current play url index
          */
        int cur_play_index;
        /*!
          point to current demuxer
          */
        void * p_cur_demuxer ;
        /*!
          point to current stream
          */
        void * p_cur_stream ;
        /*!
          point to current audio  stream
          */
        void * p_cur_ds_audio;
        /*!
          point to current video  stream
          */
        void * p_cur_ds_video;
        /*!
          point to current subtitle stream
          */
        void * p_cur_ds_sub;
        /*!
          audio ring buffer for preloading task
          */
        preload_buffer_t preload_audio_buffer;
        /*!
          video ring buffer for preloading task
          */
        preload_buffer_t preload_video_buffer;
        /*!
          audio ring buffer size of preloading task
          */
        unsigned int preload_audio_buffer_size;
        /*!
         video ring buffer size of preloading task
          */
        unsigned int preload_video_buffer_size;
        /*!
         indicate if malloc preload video buffer internal
          */
        char preload_video_buffer_malloc_internal;
        /*!
         indicate if malloc preload audio buffer internal
          */
        char preload_audio_buffer_malloc_internal;

        /*!
            ID of current video stream
          */
        int  curVideoTrackID;
        /*!
           ID of current audio stream
          */
        int  curAudioTrackID;
        /*!
           current audio codec type
          */
        unsigned int m_audio_codec_type;
        /*!
          current video  codec type
          */
        unsigned int m_video_codec_type;
        /*!
            hint whether variours of av device handles has been initialized
          */
        BOOL init_av_dev;
        /*!
            show what internal event should be processed
          */
        unsigned int internal_event ;
        /*!
            show what command should be processed
          */
        unsigned int m_user_cmd ;
        /*!
            identify whether there is only audio stream
        */
        BOOL  only_audio_mode;
        /*!
        xxxxxxxx
        */
        AUDIO_OUT_MODE audio_output_mode;
        /*!
            identify whether it is played by rtsp
        */
        BOOL  rtsp_play_mode;
        /*!
          xxxxxxxx
          */
        unsigned int   file_seq_mem_size;
        /*!
        xxxxxxxx
        */
        unsigned int   file_seq_mem_start;
        /*!
          xxxxxxxx
          */
        FILE_SEQ_STATUS   m_play_state ;// 1:play  0:stop 2:pause
        /*!
          backup the last play status
          */
        FILE_SEQ_STATUS   m_play_state_backup ;// 1:play  0:stop 2:pause
        /*!
          xxxxxxxx
          */
        FILE_SEQ_STATUS   m_preload_state ;// 1:play  0:stop 2:pause
        /*!
         xxxxxxxx
         */
        int init_buffering_finish;
        /*!
        xxxxxxxx
        */
        int   m_seek_cnt ;//
        /*!
        xxxxxxxx
        */
        int   m_seek_adj ;//

        /*!
          xxxxxxxx
          */
        int   m_aes_left;
        /*!
           to ac3/aac codec, output of demuxer is only raw data without header
           so , audio es raw data and packet header must be assembled in this buffer
          */
        u8 * p_aud_assemble_buf;
        /*!
          xxxxxxxx
          */
        int m_print_cnt;//only for debug
        /*!
          xxxxxxxx
          */
        BOOL disable_avsync;

        /*!
          handle for PTI device
          */
        void * p_pti_dev ;
        /*!
          handle for display  device
          */
        void * p_disp_dev;
        /*!
          handle for vdec  device
          */
        void * p_vdec_dev;
        /*!
          handle for audio  device
          */
        void * p_audio_dev ;
        /*!
          xxxxxxxx
          */

        void * p_sub_fifo_handle ;
        /*!
          xxxxxxxx
          */

        void * p_pb_fifo_handle ;
        /*!
          xxxxxxxx
          */

        BOOL is_fifo_playback ;
        /*!
           mutex for sub fio
          */
        unsigned int sub_fifo_mutex;
        /*!
           mutex for pb fifo
          */
        unsigned int pb_fifo_mutex;

        /*!
            lock for file playback sequencer
          */
        unsigned int lock;
        /*!
            lock for file event
          */
        unsigned int event_lock;
        /*!
        lock for seek in mul
        */
        unsigned int seek_mul_lock;

        /*!
          xxxxxxxx
          */
        unsigned char subt_buf[2048];
        /*!
            audio es warter level is too high
          */
        BOOL isAudioBufferFull ;
        /*!
            video  es warter level is too high
          */
        BOOL isVideoBufferFull ;
        /*!
            audio stream is over
          */
        BOOL isAudioEsEnd ;
        /*!
           vido stream is over
          */
        BOOL isVideoEsEnd ;
        /*!
          xxxxxxxx
          */
        BOOL  checkAvTypeOK;
        /*!
          xxxxxxxx
          */
        BOOL  checkDefinitionOK;

        /*!
          xxxxxxxx
          */
        BOOL  loadMedaiOK;
        /*!
          xxxxxxxx
          */
        BOOL  canTrickPlay;
        /*!
          xxxxxxxxxxxxx
        */
        BOOL   needTranslateUrl;
        /*!
        *  if length of video packet is shorter than  VES_SEG_LEN
        *    pad them in 1024 bytes
        */
        unsigned char * ves_seg_buf;
        /*!
           ponit to temporay ves buffer:  video packet from demuxer should be put here firstly
           and video driver will move data in this buffer to real es ring buffer of video firmware
          */
        unsigned char * p_tmp_ves_buf;
        /*!
           point to temporay aes buffer:  audio packet from demuxer should be put here firstly
           and audio driver will move data in this buffer to real es ring buffer of audio firmware
          */
        unsigned char * p_tmp_aes_buf;
        /*!
          xxxxxxxx
          */
        int  m_tmp_ves_buf_pos;
        /*!
          xxxxxxxx
          */
        s8  last_speed;
        /*!
          xxxxxxxx
          */
        s8  cur_speed;
        /*!
          xxxxxxxx
          */
        s8  tmp_speed;
        /*!
          xxxxxxxx
          */
        BOOL isAudMute;
        /*!
          xxxxxxxx
          */
        BOOL isTrickPlay;
        /*!
          xxxxxxxx
          */
        BOOL isNormalPlay;
        /*!
          xxxxxxxx
          */
        BOOL isTrickToNormal;
        /*!
          This flag show wether  file sequencer should require new video packet from demuxer in this loop
          */
        BOOL needNewVideoData;
        /*!
          This flag show wether  file sequencer should require new audio packet from demuxer in this loop
          */
        BOOL needNewAudioData;
        /*!
          xxxxxxxx
          */
        BOOL use_ext_heap ;
        /*!
            identify whether fill es task is running or has already exited
          */
        BOOL is_task_alive;
        /*!
            identify whether load media task is running or has already exited
          */
        BOOL is_load_task_alive;
        /*!
            load media state
          */
        unsigned  int   load_media_state ;
        /*!
            identify whether preload task is running or has already exited
          */
        int  is_preload_task_alive;
        /*!
            identify whether preload task is running or has already exited
          */
        BOOL check_task_result;

        /*!
        identify whether check task is running or has already exited
        */
        CHECK_ES_STATE check_task_state;
        /*!
            identify whether preload task is running or has already exited
          */
        BOOL  is_pbfifo_task_alive;
         /*!
            identify whether playback fifo is buffered ok
          */
        BOOL  pbfifo_buffering_ok;
        /*!
            identify whether http need reconnected
          */
        BOOL  pbfifo_http_need_reconnect;
        /*!
            http connect offset
          */
        unsigned long  pbfifo_connect_off;
        /*!
            current reading position of http stream 
          */
        unsigned long  pbfifo_stream_pos;
        /*!
            identify whether http need reconnected
          */
        PB_FIFO_HTTP_STATE  pbfifo_http_state;        
        /*!
            content length from http server
          */
        unsigned long  pbfifo_content_len;
        /*!
            current   ves number in es buffer
          */
        unsigned  int   totalVesNum ;
        /*!
          current   aes number in es buffer
          */
        unsigned  int   totalAesNum ;
        /*!
          refer to current demuxer
          */
        void * p_demuxer ;
        /*!
          refer to current demuxer
          */
        void * p_demuxer2 ;
        /*!
          refer to current  file stream
          */
        void * p_stream ;
        /*!
          refer  to audio stream of demuxer
          */
        void * p_ds_audio;
        /*!
          refer to video  stream of demuxer
          */
        void * p_ds_video;
        /*!
           refer to  subtitle  stream of demuxer
          */
        void * p_ds_sub;
        /*!
          stack memory of fill_es task
          */
        void * pstack;
        /*!
        m file duration
        */
        int * p_m_duration;
        /*!
        stack memory of loadmedia
        */
        void * p_loadstack;
        /*!
          xxxxxxxx
          */
        void * p_preloadstack;

        /*!
        xxxxxxxx
        */
        void * p_checkesstack;
        /*!
        xxxxxxxx
        */
        void * p_pbfifostack;
        /*!
          xxxxxxxx
          */
        void * ts_priv;
        /*!
          xxxxxxxx
          */
        int is_ts;
        /*!
          bit0: 1(audio codec support) 0(audio codec not support)
          bit1: 1(video codec support) 0(video codec not support)
          */
        u32 is_av_codec_support;
        /*!
          xxxxxxxx
          */
        //   int audio_format;
        /*!
          xxxxxxxx
          */
        int audio_samplerate;
        /*!
          xxxxxxxx
          */
        int audio_channels;
        /*!
          xxxxxxxx
          */
        int audio_samplesize;
        /*!
          xxxxxxxx
          */
        int  audio_bps;//bytes per secondes
        /*!
          xxxxxxxx
          */
        char * audio_language;
        /*!
          xxxxxxxx
          */
        char * album;
        /*!
          xxxxxxxx
          */
        char * title;
        /*!
          xxxxxxxx
          */
        char * artist;
        /*!
          xxxxxxxx
          */
        char * genre;
        /*!
          xxxxxxxx
          */
        char * comments;
        /*!
          xxxxxxxx
          */
        //  int video_format;
        /*!
          xxxxxxxx
          */
        int video_disp_w;
        /*!
          xxxxxxxx
          */
        int video_disp_h;
        /*!
          xxxxxxxx
          */
        int video_fps;
        /*!
          xxxxxxxx
          */
        u32  video_bps;//bytes per secondes
        /*!
          for uploading
          */
        VIDEO_W_H_FPS video_whfps;
        /*!
          xxxxxxxx
          */
        double file_start_time;
        /*!
          xxxxxxxx
          */
        double file_duration;
        /*!
          xxxxxxxx
          */
        double m_loaded_pts;
        /*!
          xxxxxxxx
          */
        double m_get_vpts;
        /*!
          xxxxxxxx
          */
        double m_get_apts;
        /*!
          xxxxxxxx
          */
        u64   file_size;
        /*!
        xxxxxxxx
        */
        FILE_TIME file_modify_time;
        /*!
          xxxxxxxx
          */
        int is_play_at_time;
        /*!
          xxxxxxxx
          */
        float seek_seconds;
        /*!
          xxxxxxxx
          */
        unsigned int start_seconds;
        /*!
          xxxxxxxx
          */
        int is_play_to_end;
        /*!
          xxxxxxxx
          */
        int audio_track_id;
        /*!
         xxxxxxxx
         */
        int audio_track_num;
        /*!
          audio track language
          */
        TRACK_LANG * audio_lang_array;
        /*!
          xxxxxxxx
          */
        int  video_average_bps;
        /*!
          xxxxxxxx
          */
        int  last_v_average_bps;
        /*!
          xxxxxxxx
          */
        int  audio_average_bps;
        /*!
          xxxxxxxx
          */
        double  max_audio_pts;
        /*!
          xxxxxxxx
          */
        double  max_video_pts;
        /*!
          xxxxxxxx
          */
        float  ref_first_audio_pts;
        /*!
          xxxxxxxx
          */
        float  ref_first_video_pts;
        /*!
          available audio es  number
          */
        u32  available_aes_bytes;
        /*!
          available video es  number
          */
        u32  available_ves_bytes;

        /*!
          xxxxxxxx
          */
        int exsubtitle;
        /*!
          xxxxxxxx
          */
        int subt_id;
        /*!
          xxxxxxxx
          */
        void * sub_info;
        /*!
          xxxxxxxx
          */
        void * p_ext_heap_hdl;
        /*!
          xxxxxxxx
          */
        AV_DEC_CAP  dec_cap;
        /*!
          xxxxxxxx
          */
        double  duration;//seconds
        /*!
        xxxxxxxx
        */
        double  max_duration;//seconds
        /*!
          xxxxxxxx
          */
        int   video_pid;
        /*!
          xxxxxxxx
          */
        int   audio_pid;
        /*!
          xxxxxxxx
          */
        int   pcr_pid;
        /*!
        xxxxxxxx
        */
        int   subtitle_pid;

        /*!
          xxxxxxxx
          */
        int subtitle_count;
        /*!
        xxxxxxxx
        */
        char ** subtitle_path;
        /*!
          xxxxxxxx
          */
        u32   task_prio;
        /*!
          xxxxxxxx
          */
        u32   net_task_prio;
        /*!
          xxxxxxxx
          */
        u32   http_task_prio;
        /*!
          xxxxxxxx
          */
        u32   task_size;
        /*!
          xxxxxxxx
          */
        u32   task_preload_prio;
        /*!
          xxxxxxxx
          */
        u32   task_preload_size;

        /*!
        xxxxxxxx
        */
        u32   task_checkes_prio;
        /*!
          xxxxxxxx
          */
        u32   task_checkes_size;
        /*!
        xxxxxxxx
        */
        u32   task_pb_fifo_prio;
        /*!
          xxxxxxxx
          */
        u32   task_pb_fifo_size;
        /*!
          audio pts from sw demuxer, millisecond
          */
        double   orig_apts;
        /*!
          system audio pts which was transfromed from origin audio pts
          */
        u32   sys_apts;
        /*!
          video pts to upload
          */
        unsigned long vpts_upload;
        /*!
          audio pts to upload (ms)
          */
        unsigned long apts_upload;
        /*!
          video pts from sw demuxer, millisecond
          */
        double   first_vpts;
        /*!
          video pts from sw demuxer, millisecond
          */
        double   orig_vpts;
        /*!
          system audio pts which was transfromed from origin video pts
          */
        u32   sys_vpts;
        /*!
             point to a 4 bytes memory space where video firmware would update
             and notice that only on linux os, this field will be used
        */
        u32 * p_current_sys_vpts;
        /*!
             point to a 4 bytes memory space where video firmware would update
             and notice that only on linux os, this field will be used
        */
        u32 * p_current_sys_apts;
        /*!
           subtitle's pts from sw demuxer, millisecond
          */
        double   orig_spts;
        /*!
          current video paket to be pushed to es buffer of firmware
          */
        unsigned char * p_v_pkt_start;
        /*!
          current audio paket to be pushed to es buffer of firmware
          */
        unsigned char * p_a_pkt_start;
        /*!
          current subtitle paket to be pushed to es buffer of firmware
          */
        unsigned char * p_sub_pkt_start;

        /*!
          left video data to be pushed to ved tmp buffer
          */
        unsigned int left_v_pkt_bytes;
        /*!
          left audio data to be pushed to ved tmp buffer
          */
        unsigned int left_a_pkt_bytes;

        /*!
          extra audo data, such as, to ac3 ,we should add some audio packet header
          */

        unsigned char * p_extra_aud_buf;
        /*!
          len of extra data
          */

        unsigned char extra_audio_size ;
        /*!
          callback function , caller should register it
          */
        file_seq_event_cb_fun_t event_cb;
        /*!
           current tv system
        */
        unsigned char  cur_tv_hd_fmt;


        /*!
        * the following field is only for internal command buffer
        *
        */

        /*!
           read position
        */
        int  rd_pos;
        /*!
            write position
        */
        int wr_pos;
        /*!
           command buffer fifo
        */
        FP_USER_CMD_T * cmd_fifo;
        /*!
           identify whether fifo is full
        */
        u8 is_cmd_buf_full;
        /*!
           identify whether fifo is full
        */
        int load_media_times;
        /*!
        *
        *  the following four fields is for audio buffering mechanism
        *                            peacer add 20140222
        *
        */

        /*!
          xxx
        */
        void  * p_audio_buffering_packet_list;
        /*!
          xxx
        */
        BOOL is_audio_buffering_packet;
        /*!
        xxxx
        */
        void * p_buffering_audio_packet;
        /*!
        xxx
        */
        u32   buffering_audio_data_size;


        /*!
        xxx
        */
        int camera_fd;

        /*!
        xxx
        */
        int camera_rtsp_record;

        /*!
        network play mode
        */
        int ott_playmode;

        /*!
        add by  xxia 1220, for the hls 
        */
        HLS_PARSER_RESULT hls_parser;



        /*
        *   NOTICE :
        *      the folowwing member functions will be explored to upper layer code !!!!!!!!!
        *       upper layer code should not modify the internal member variable of file seq structure !!!!!
        *
        */


        /*!
          xxxxxxxx
          */
        file_seq_set_ott_playmode_t set_ott_playmode;

        /*!
          xxxxxxxx
          */
        file_seq_loadmedia_fun_t loadmedia;
        /*!
          xxxxxxxx
          */
        file_seq_loadmedia_fun_t loadmedia_task;
        /*!
          xxxxxxxx
          */
        file_seq_loadmedia_times_fun_t loadmedia_times;

        /*!
          xxxxxxxx
          */
        file_seq_unloadmedia_fun_t unloadmedia;
        /*!
          xxxxxxxx
          */
        file_seq_start_fun_t      start;
        /*!
          xxxxxxxx
          */
        file_seq_stop_fun_t       stop;
        /*!
        xxxxxxxx
        */
        file_seq_stop_fun_t       force_stop;
        /*!
          xxxxxxxx
          */
        file_seq_pause_fun_t     pause;
        /*!
          xxxxxxxx
          */
        file_seq_resume_fun_t   resume;
        /*!
          xxxxxxxx
          */
        file_seq_set_speed_fun_t set_speed;
        /*!
          xxxxxxxx
          */
        file_seq_preview_fun_t      preview;
        /*!
          xxxxxxxx
          */
        file_seq_get_vpts_fun_t  get_vpts;
        /*!
          xxxxxxxx
          */
        file_seq_play_at_time_fun_t play_at_time;
        /*!
          xxxxxxxx
          */
        file_seq_get_status_fun_t  get_status;
        /*!
          xxxxxxxx
          */
        file_seq_change_audio_track_fun_t change_audio_track;
        /*!
          xxxxxxxx
          */
        file_seq_get_audio_track_lang_fun_t get_audio_track_lang;
        /*!
          xxxxxxxx
          */
        file_seq_change_video_track_fun_t  change_video_track;
        /*!
          xxxxxxxx
          */
        file_seq_get_film_info_fun_t     get_film_info;
        /*!
          xxxxxxxx
          */
        file_seq_set_file_path_fun_t     set_file_path;
        /*!
          xxxxxxxx
          */
        file_seq_set_file_path_ex_fun_t     set_file_path_ex;
        /*!
          xxxxxxxx
          */
        file_seq_set_muti_file_path_fun_t  set_muti_file_path;

        /*!
          xxxxxxxx
          */
        file_seq_av_codec_type_fun_t   set_av_codec_type;
        /*!
          xxxxxxxx
          */
        file_seq_do_cmd_fun_t   do_cmd;
        /*!
          xxxxxxxx
          */
        file_seq_mp_dump_fun_t mp_dump;//only for debug
        /*!
          xxxxxxxx
          */
        file_seq_mp_dump_fun_t mp_dumpaudio;//for dump audio from mp3 file
        /*!
          xxxxxxxx
          */
        file_seq_run_mem_fun_t  check_mem;//only for debug
        /*!
          xxxxxxxx
          */
        file_seq_get_subt_fun_t get_subt;
        /*!
          xxxxxxxx
          */
        file_seq_get_subt_info_fun_t get_subt_info;
        /*!
          xxxxxxxx
          */
        file_seq_set_subt_id_fun_t set_subt_id;
        /*!
          xxxxxxxx
          */
        file_seq_mem_init_fun_t mem_init;
        /*!
          xxxxxxxx
          */
        file_seq_mem_alloc_fun_t mem_alloc;
        /*!
          xxxxxxxx
          */
        file_seq_mem_free_fun_t mem_free;
        /*!
          xxxxxxxx
          */
        file_seq_mem_release_fun_t mem_release;
        /*!
          xxxxxxxx
          */
        file_seq_get_exsubt_fun_t get_exsubt;


        /*!
             upper layer code call this function to register member variable 'event_cb'
          */
        file_seq_register_event_cb_func_t  register_event_cb;

        /*!
          xxxxxxxx
          */
        file_seq_set_vfmt_fun_t set_tv_sys;

        /*!
          xxxxxxxx
          */
        isNetworkStream_fun_t is_network_stream;
        /*!
          xxxxxxxx
          */
        file_seq_set_live_broadcast_fun_t set_live_broadcast;

        /*!
          xxxxxxxx
          */
        file_seq_check_bg_task_alive   check_bg_task_alive;

        /*!
          get download url such as youtube, dailymotion.
          */
        http_request_url_t request_url;
        /*!
          xxxxxxxx
          */
        BOOL tv_sys_auto_set;

        /*!
          xxxxxxxx
          */
        int unable_trickplay;

        /*!
        xxxxxxxx
        */
        unsigned int   vdec_policy;

        /*!
         xxxxxxxx
         */
        int (*camera_open)(char * name);

        /*!
        xxxxxxxx
        */
        int (*camera_read)(int fd, char * buffer, int buffer_len);

        /*!
         xxxxxxxx
         */
        int (*camera_close)(int fd);

        /*!
         xxxxxxxx
         */
        int (*rtsp_video_rec)(void *, int);
        /*!
         xxxxxxxx
         */
        void * pb_internal;
	/*!
	  hls_server_prio add by zhouxf(445297005@qq.com)
	 */	
	u32 hls_server_prio;
	/*!
	  xxxxxxx
	 */
	u32 hls_server_size;
	/*!
	  p2p_server_recv_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_recv_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_recv_size;
	/*!
	  p2p_server_deal_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_deal_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_deal_size;
	/*!
	  p2p_server_send_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_send_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_send_size;
	/*!
	  p2p_server_monitor_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_monitor_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_monitor_size;
	/*!
	  p2p_server_bw_prio add by zhouxf(445297005@qq.com)
	 */
	u32 p2p_server_bw_prio;
	/*!
	  xxxxxxx
	 */
	u32 p2p_server_bw_size;
	/*!
	  xxxxxxx
	 */
	u32 vod_heartbeat_login_prio;
	/*!
	  xxxxxxx
	 */
	u32 vod_heartbeat_login_size;
	/*!
	  xxxxxxxx
	 */
	unsigned int   vdec_stop_type;	
	/*!
	  xxxxxxxx
	 */	
	file_seq_set_vdec_stop_type_fun_t     set_vdec_stop_type;
	
    } FILE_SEQ_T;

    /*!
      network pvr parameter
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        u16 video_pid;
        /*!
          xxxxxxxx
          */
        u16 audio_pid;
        /*!
          xxxxxxxx
          */
        u16 pcr_pid;
        /*!
         xxxxxxxx
         */
        u16 pmt_pid;//=<16 pmt_pid<=0x1ffe

        /*!
          if 0, driver will use a default size buffer, must 16 bytes align
          */
        void * record_buffer;

        /*!
          if 0, driver will use a default size
          */
        u32 recode_buffer_size;

        /*!
          must be type of dmx_input_type_t
          */
        u32 stream_in;

        /*!
          xxxxxxxx
          */
        u32  vdec_type;

        /*!
          xxxxxxxx
          */
        u32  adec_type;
        /*!
          xxxxxxxx
          */
        u32  psi_interval_num;//pmt frequency set by bitrate,500kbps default set as 20

    } NETWORK_PVR_PARM;

    /*!
      SA fill ts parameter
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        int task_prio;
        /*!
          xxxxxxxx
          */
        int stack_size;
        /*!
          xxxxxxxx
          */
        u32 p_vdec_start;

        /*!
          if 0, driver will use a default size buffer, must 16 bytes align
          */
        u32 vdec_mem_size;

        /*!
          if 0, driver will use a default size
          */
        u32 p_audio_start;

        /*!
          must be type of dmx_input_type_t
          */
        u32 audio_mem_size;
        
        /*!
          xxxxxxxxxx
          */
        unsigned char * ts_buf;
        /*!
          xxxxxxxxxxxx
          */
        u32 tsbuf_size;

    } SA_FILL_TS_PARA;


    /*!
      SA transcode ts parameter
      */
    typedef struct {
        /*!
          xxxxxxxx
          */
        u16 video_pid;
        /*!
          xxxxxxxx
          */
        u16 audio_pid;
        /*!
          xxxxxxxx
          */
        u16 pcr_pid;
        /*!
          xxxxxxxx
          */
        u16 pmt_pid;//=<16 pmt_pid<=0x1ffe
        /*!
          must be type of dmx_input_type_t
          */
        u32 stream_in;

        /*!
          xxxxxxxx
          */
        u32  vdec_type;

        /*!
          xxxxxxxx
          */
        u32  adec_type;
        /*!
          xxxxxxxx
          */
        u32  psi_interval_num;//pmt frequency set by bitrate,default set as 20
    } SA_TRANS_TS_PARA;
    /*!
    *
    *  the following API should be exposed to other code
    *
    *            201304-23 PeacerTsui
    *
    */
    FILE_SEQ_T * file_seq_get_instance();
    BOOL is_file_seq_exit();
    /*!
      xxxxxxxx
      */
    void * file_seq_create(PB_SEQ_PARAM_T * p_param);
    /*!
      xxxxxxxx
      */
    void  file_seq_destroy();
    /*!
      xxxxxxxx
      */
    void file_seq_set_fw_mem(unsigned int p_vdec_start,/*IN*/
                             unsigned int  vdec_mem_len,/*IN*/
                             unsigned int p_audio_mem_start,/*IN*/
                             unsigned int audio_mem_len/*IN*/);
    /*!
      xxxxxxxx
      */
    void file_seq_clear_fw_mem();
    /*!
      xxxxxxxx
      */
    char * get_demuxer_tag_info(void * demux_handle, char * tag);
    /*!
      xxxxxxxx
      */
    //void file_seq_event_cb(FILE_PLAY_EVENT_E event_type, void * p);







    /*
     *     PARAMETER:
     *     mode:  0: sender   1:recevier
     *     stack_size:
     *     task_priority:
     *
     *     RETURN VALUE:
     *
     *               handler you will use it in other interface
     */
    void  * NetworkPVR_Init(NW_PVR_WORK_MODE mode, int stack_size, int task_piro);



    RET_CODE NetworkPVR_SetParam(void * hdl, NETWORK_PVR_PARM * param);


    void     NetworkPVR_SetFwMem(void * hdl, unsigned int p_vdec_start,
                                 unsigned int  vdec_mem_len, unsigned int p_audio_mem_start,
                                 unsigned int audio_mem_len) ;

    /*
    *
    *  NOTE:  only after calling NetworkPVR_Stop(), this function can be invoked !!!!
    *
    */
    void  NetworkPVR_ChangeMode(void * hdl, int mode);
    /*
    *
    *   sender call this function
    *
    */
    RET_CODE    NetworkPVR_Send(void * hdl, char * dst_addr, int port);

    RET_CODE    NetworkPVR_Recv(void * hdl, int recv_port);


    void NetworkPVR_Stop(void * hdl);

    RET_CODE   NetworkPVR_Deinit(void * hdl);

    /*
    *
    *  NOTE:  SA fill ts functions!!!!
    *
    */
    void SA_fill_ts_start(SA_FILL_TS_PARA * para);

    void SA_fill_ts_data(unsigned char * buf, int len);

    void SA_fill_ts_stop();




    /*!
    *
    *  SA_trans_ts_init
    *
    para:ts parameters

    return trans ts handle
    *
    *
    */

    void * SA_trans_ts_init(SA_TRANS_TS_PARA * para);
    /*!
    *
    *  SA_trans_ts_getpmt
    *
    return unsigned char *  pmt table,*len is pmt table length
    *
    *
    */
    unsigned char * SA_trans_ts_getpmt(void * hdl, int * len);

    /*!
    *
    *  SA_trans_ts_data
    *
    src:raw audio/video ts packet
    src_size:src packet size
    buf:may used for dst
    dst_length :trans length
    return unsigned char *  trans result addr

    *
    *
    */

    unsigned char * SA_trans_ts_data(void * hdl, void * src, int src_size, void * buf, int * dst_size);
    /*!
    *
    *  SA_trans_ts_deinit
    *
    *
    *
    */
    void SA_trans_ts_deinit(void * hdl);
    ////

    /*!
    *
    *  get_load_err_info
    *
    *
    *
    */
    BOOL file_seq_get_load_err_info(void *p_handle, LOAD_ERR_INFO *p_load_err_info);
    
    /*!
     xxxxxxxx
     */
    typedef struct {
        /*!
         xxxxxxxx
         */
        u8 * buffer;
        /*!
        xxxxxxxx
        */
        int    read_pos;
        /*!
        xxxxxxxx
        */
        int    write_pos;
        /*!
        xxxxxxxx
        */
        int    buffer_len;
        /*!
        xxxxxxxx
        */
        int    total_data;
    } camera_buffer_t;


#ifdef __cplusplus
}
#endif

#endif




