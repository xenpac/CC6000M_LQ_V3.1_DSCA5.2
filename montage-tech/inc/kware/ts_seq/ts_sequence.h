/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TS_SEQUENCE_H__
#define __TS_SEQUENCE_H__

/*!
  ts sequence with less memroy
  */
#define PLAY_LESS_MEM
/*!
  max gop num in the input memory
  */
#define MAX_GOP_NUM   32
/*!
  max length of gop
  */
   #define  MAX_GOP_LEN   256
/*!
  max length of sequence
  */
  #define  MAX_SEQUENCE_LEN   256
 
/*!
  only for debug
  */
#define  PER_FILL_BYTES   (188*10)

/*!
  xxx
  */
#define  MAX_DROP_SEGMENT   (15)

/*!
  xxx
  */
#define  TMP_BUFFER_LEN  (188 * 5000)

/*!
  xxx
  */
#define TS_PACKET_SIZE (188)

/*!
  xxx
  */
#define TS_SYNC_FLAG    (0x47)


/*!
  This structure defines the types of ts fileplay
  */
typedef enum
{
  /*!
    normal play mode
    */
  TS_SEQ_NORMAL_PLAY,
  /*!
    step forward play mode
    */
  TS_SEQ_STEP_FW_PLAY,
  /*!
    step backward play mode
    */
  TS_SEQ_STEP_BW_PLAY,
  /*!
    fast play mode 2x
    */
  TS_SEQ_FAST_PLAY_2X,
  /*!
    fast play mode 4x
    */
  TS_SEQ_FAST_PLAY_4X,
  /*!
    fast play mode 8x
    */
  TS_SEQ_FAST_PLAY_8X,
  /*!
    fast play mode 16x
    */
  TS_SEQ_FAST_PLAY_16X,
  /*!
    fast play mode 32x
    */
  TS_SEQ_FAST_PLAY_32X,
  /*!
    slow play mode 2x
    */
  TS_SEQ_SLOW_PLAY_2X,
  /*!
    slow play mode 4x
    */
  TS_SEQ_SLOW_PLAY_4X,
  /*!
    revert fast play mode 2x
    */
  TS_SEQ_REV_FAST_PLAY_2X,
  /*!
    revert fast play mode 4x
    */
  TS_SEQ_REV_FAST_PLAY_4X,
  /*!
    revert fast play mode 8x
    */
  TS_SEQ_REV_FAST_PLAY_8X,
  /*!
    revert fast play mode 16x
    */
  TS_SEQ_REV_FAST_PLAY_16X,
  /*!
    revert fast play mode 32x
    */
  TS_SEQ_REV_FAST_PLAY_32X,
  /*!
    revert slow play mode 2x
    */
  TS_SEQ_REV_SLOW_PLAY_2X,
  /*!
    revert slow play mode 4x
    */
  TS_SEQ_REV_SLOW_PLAY_4X
  
}ts_seq_play_mode_t;
/*!
 xxx
  */
typedef enum{

  /*!
  */
 MPEG_TYPE_CODE,
 /*!
  */
  H264_TYPE_CODE,
  /*!
  */
  AVS_TYPE_CODE,
  /*!
  */
  INVALID_VIDEO_TYPE,
  
} video_codec_type_t;
/*!
  This structure defines the types of ts seq status
  */
typedef enum
{
  /*!
    Idle
    */
  TS_SEQ_STATUS_IDLE,
  /*!
    Busy
    */
  TS_SEQ_STATUS_BUSY
}ts_seq_status_t;



/*!
  This structure defines the types of ts seq status
  */
typedef enum
{
  /*!
    raw ts
    */
  TS_SEQ_INPUT_TS,
  /*!
    raw es
    */
  TS_SEQ_INPUT_ES,
  /*!
    tag ts
    */
  TS_SEQ_INPUT_TS_WITH_TAG,
  /*!
    normal ts
    */
  TS_SEQ_INPUT_TS_NORMAL,
  /*!
    HD ts
    */
  TS_SEQ_INPUT_TS_HD  
}ts_seq_input_mode_t;



/*!
  this structure define the type of GOP
  */
typedef enum {
  /*!
    no B frame
    */
  I_P_P,  //no B frame in a gop
   /*!
     only one B frame between I and P
    */
  I_B_P,  //only one B frame between I and P or P and P
     /*!
      only two  B frames between I and P
       */
  I_B_B_P, // only two B frames between I and P  or P and P
    /*!
      no P frame
      */
  I_B_B,
   /*!
    no B and P frame
      */
  I_I_I_I,   // no B and P frame in a gop
  /*!
    special frame
    */
  I_SPECIAL,
  /*!
    invalid gop
    */
  INVALID_GOP,

}base_frame_t;




/*!
  this type define frame code type
  */
typedef enum{
  /*!
    key frame
    */
  I_FRAME = 1,
   /*!
     P frame
     */
   P_FRAME = 2,
   /*!
      B frame
     */
   B_FRAME = 3,
   /*!
     invalid frame
      */
   INVALID_FRAME_TYPE,
}video_frame_t;

/*!
  this type define slice codec type
  */
typedef enum{
   /*!
     P slice
     */
     P_SLICE = 0,
    /*!
      I slice
      */
     B_SLICE = 1 ,
   /*!
      B slice
     */
     I_SLICE = 2 ,
   /*!
      SI slice
     */
     SP_SLICE = 3 ,
    /*!
      SP slice
     */
     SI_SLICE ,   
   /*!
      B slice
     */
     P_SLICE_2 ,
   /*!
      SI slice
     */
     B_SLICE_2 ,
    /*!
      SP slice
     */
     I_SLICE_2 ,   
   /*!
      SI slice
     */
     SP_SLICE_2 ,
    /*!
      SP slice
     */
     SI_SLICE_2 ,   
   /*!
     invalid slice
      */
   INVALID_SLICE_TYPE,
}slice_t;
/*!
  this type define nalu  type
  */
typedef enum{
   /*!
      no IDR
     */
     NONE_USE = 0,
   /*!
      no IDR
     */
     NONE_IDR = 1, //no IDR slice and not data partition
   /*!
      data A
     */
     PARTITION_DATA_A = 2,
   /*!
      data B
     */
     PARTITION_DATA_B = 3,
   /*!
      data C
     */
     PARTITION_DATA_C = 4,
   /*!
      IDR
     */
     IDR_TYPE = 5, // slice of IDR
   /*!
      IDR
     */
     SEI_TYPE = 6,
   /*!
      IDR
     */
     SPS_TYPE = 7, //sequence parameter sets
   /*!
      IDR
     */
     PPS_TYPE = 8,  //picture parameter sets
     /*!
      IDR
     */
     SPLIT_NALU_TYPE = 9,
          /*!
      IDR
     */
    END_SEQ_TYPE = 10,
          /*!
      IDR
     */
     END_TS_TYPE = 11,
          /*!
      IDR
     */
     PADDING_TYPE = 12,
     /*!
     INVALID_NALU_TYPE
     */
     RESEVED_START_TYPE = 13,
          /*!
     INVALID_NALU_TYPE
     */
     RESEVED_END_TYPE = 23,
          /*!
     INVALID_NALU_TYPE
     */
     NO_USE2_START_TYPE = 24 ,
          /*!
     INVALID_NALU_TYPE
     */
     NO_USE2_END_TYPE = 31,
          /*!
     INVALID_NALU_TYPE
     */
     INVALID_NALU_TYPE,
   
}nalu_t;

/*!
  this structure define detail of gop
  */
typedef struct  gop{
  /*!
   gop type
    */
  base_frame_t   base_frame_type; 
  /*!
   total frame of gop
    */
  int  len;               // total of  I P B in a gop
  /*!
  array save type of each frame
    */
  video_frame_t   frame_type_array[MAX_GOP_LEN];
   /*!
  array save type of each frame
    */
  int frame_dropbit[MAX_GOP_LEN];  
   /*!
  array save type of each frame
    */
  unsigned int frame_startpos[MAX_GOP_LEN]; 
   /*!
  array save type of each frame
    */
  unsigned int frame_deletesize[MAX_GOP_LEN]; 
  /*!
  array save pointer to frame
    */
  char        *        video_packet_start_array[MAX_GOP_LEN];
}gop_t;

/*!
  this structure define detail of gop
  */
typedef struct  sequence_detail{
  /*!
   total frame of gop
    */
  int  totalSlice;               // total of  I P B slice in current sequence
  /*!
  array save type of each frame
    */
  slice_t      slice_type_array[MAX_SEQUENCE_LEN];
   /*!
  array save type of each frame
    */
  int frame_dropbit[MAX_SEQUENCE_LEN];
   /*!
  array save type of each frame
    */
  int frame_refbit[MAX_SEQUENCE_LEN];   
  /*!
  array save pointer to slice
    */
  char        *        video_packet_start_array[MAX_SEQUENCE_LEN];
}sequence_detail_t;


/*!
  This structure defines of ts detail
  */
typedef struct ts_detail_info
{
  /*!
  xxx
  */
  unsigned int reverse_offset;
  /*!
  xxx
  */
  unsigned int fragment_num;
  /*!
  xxx
  */
  unsigned int fragment_offset[MAX_GOP_LEN];
  /*!
  xxx
  */
  unsigned int fragment_size[MAX_GOP_LEN];
  /*!
  xxx
  */
  int pts[MAX_GOP_LEN];    
}ts_detail_info_t;

/*!
  internal fifo type
  */
typedef struct 
{
/*!
  xxx
  */
int len;
/*!
  read position
  */
unsigned long  rd_pos;
/*!
  write position
  */
unsigned long  wr_pos;
/*!
  point to end 
  */
 unsigned long   end_pos;
/*!
  point to start
  */
unsigned long   start_pos;
/*!
  mem_malloc
  */
BOOL               mem_malloc;
/*!
  mutex
  */
  unsigned int  mutex;
} fifo_type_t ;

#define FIFO_MAX_SEG_NUM 16
/*!
  internal segfifo type
  */
typedef struct 
{
/*!
  xxx
  */
int len;
/*!
  read position
  */
unsigned long  rd_pos;
/*!
  write position
  */
unsigned long  wr_pos;
/*!
  point to end 
  */
unsigned long   seg_len[FIFO_MAX_SEG_NUM];
/*!
  point to start
  */
unsigned long   seg_start_pos[FIFO_MAX_SEG_NUM];
/*!
  point to end 
  */
 unsigned long   seg_pos_offset[FIFO_MAX_SEG_NUM];
/*!
  point to start
  */
int   total_seg_num;
/*!
  point to start
  */
int   cur_read_seg;
/*!
  point to start
  */
int   cur_write_seg;
/*!
  mutex
  */
unsigned int  mutex;
} segfifo_type_t ;
/*!
  */
struct ts_seq;
/*!
define init_func type
*/
typedef  int       (* ts_sequenc_parser_fun_t)(struct ts_seq *, ts_detail_info_t *);

/*!
  This structure defines of ts seq
  */
typedef struct ts_seq
{
/*!
  output of this parser
  */
fifo_type_t *p_TsQueue;

/*!
  msg handle
  */
unsigned int msg_hdl;

/*!
  FF2X  ,FF4X, 8X, ...
  */
ts_seq_play_mode_t   play_mode;
/*!
  FF2X  ,FF4X, 8X, ...
  */
ts_seq_play_mode_t   last_play_mode; 
/*!
  xxx
  */
ts_seq_input_mode_t  input_mode;
/*!
  gop type of this bit stream
  */
gop_t        cur_gop_type;
/*!
  xxx
  */
int   last_gop_len;
/*!
  forward or backward
  */
BOOL        isForward;
/*!
  point to ts bit stream
  */
char *       p_data;
/*!
   len of current bit stream
   */
int           data_len;
/*!
  len of unconsumed data
  */
int           left_data_len;
/*!
  video pid
  */
int            video_pid;
/*!
   vidoe type
  */
  video_codec_type_t cur_video_type;



/*!
  flag refer to drop one gop
  */
BOOL   dropOneGop;
/*!
  number of drop segment
  */
int       drop_segment_num;


/*!
   ****** the following variable only for internal tmp buffer  *********
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  ts paket will be put to this buffer  until a entire gop can be parsed 
  */
char *        p_tmp_buffer;
/*!
  len of tmpbuffer
  */
  int            tmp_buffer_len;
/*!
  start of tmpbuffer
  */
char *        p_tmp_buffer_start;    
/*!
  end of tmpbuffer
  */
char *       p_tmp_buffer_end;
/*!
  current write pointer of tmpbuffer
  */
char *        p_tmp_buffer_wp; 
/*!
  current read pointer of tmpbuffer
  */
char *        p_tmp_buffer_rd;
/*!
  pointer to the free space
  */
int            tmp_buffer_free_space;
/*!
  pointer to the free space
  */
int            tmp_buffer_offset;
 /*!
    *** NOTE: the following variable is only for h264 sequence parser ****
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    h264 use: sequence detail
   */
  sequence_detail_t  cur_sequence_detail;
 /*!
    h264 use: 
   */
 int last_sequence_len;
 /*!
  mpeg parser use:  find first key frame : 
  */
  BOOL       isFindFirstIFrame;  
 /*!
  mpeg parser use:  find first key frame when backward: 
  */
  BOOL       isFindFirstIFrameBackward;   
  /*!
    mpeg parser use : find sencond key frame
    */
   BOOL       isFindSecondIFrame;  
  /*!
    264 parser use:  find first IDR
    */
    BOOL     isFindFirstIDR;
   /*!
      264 parser use:  find second IDR
     */
    BOOL     isFindSecondIDR;
   /*!
     264 parser use:  find PPS
     */
    BOOL     isFindPPS;
   /*!
     264 parser use:  find SPS
     */
    BOOL    isFindSPS;
    /*!
       264 parser use: PPS position in tmp buffer
      */
      unsigned int cur_pps_pos;
    /*!
       264 parser use: SPS position in tmp buffer
      */
       unsigned int cur_sps_pos;

      /*!
      264 parser use:
      */
      unsigned long long  totalParsedSequence;
      /*!
      264 parser user:
      */
      unsigned long long totalParsedSlice;
      /*!
      264 parser use:
      */
      unsigned long long  totalDropSlice;
      /*!
      xxx
      */
      unsigned long long totalSaveSlice;
      /*!
      264 parser use:  
      */
      unsigned char  baseFactor;
      /*!
      h264 parser use:
      */
      unsigned char devide_factor;
      /*!
      h264 parser use
      */
      BOOL  is_init_devide_factor;
      
      
      /*!
      gop parser's status
      */
      ts_seq_status_t  seq_parser_status;
      /*!
      flag refer to force to exit
      */
      /*!
      sequence parser
      */
      ts_sequenc_parser_fun_t   p_forward_parser_func;
      /*!
      sequence revert parser
      */
      ts_sequenc_parser_fun_t   p_backward_parser_func;
      /*!
      xxx
      */
      int last_ts_dropbit;
      /*!
      array save type of each frame
        */
      unsigned int needdropnum; 
      /*!
      xxx
      */
      unsigned int totalframes;
      /*!
      xxx
      */
      video_frame_t  last_video_frame_type;
      /*!
      xxx
      */
      video_frame_t  cur_video_frame_type;
      /*!
      xxx
      */
      BOOL  isExit;
      /*!
      used by low memory play
      */
      int  gop_num;
      /*!
      used by low memory play
      */
      int  bpframe_cpnum[MAX_GOP_NUM];
      /*!
      used by low memory play
      */
      int  i_frame_len[MAX_GOP_NUM];
} ts_seq_t;



/*!
  init ts sequence parser
  param[2]:[IN]   true means trick play in forward mode,otherwise ,in backward mode
  */
void * ts_seq_init(u8 *p_fifo, u32 fifo_size,
                  BOOL isForward, u8 *p_buf, u32 buffer_size);

/*!
  destroy ts sequence parser
  */
int  ts_seq_deinit(void * pHandle);

/*!
  param[0]:[IN]   pointer to one segment of ts bit stream 
                        NOTICE !!: every ts paket should begin with sycn code (0x47),otherwise it will be drop !!!
  param[1]:[IN]   length of ts bit stream
                          NOTICE !!: length of bit stream  should be aligned with 188 bytes !!!!
  param[2]:[IN]       video pid
  param[3]:[IN]       mode of trick play
  param[4]:[IN]       no use
  param[5]:[OUT]    no use

  NOTICE: This function will block the caller's thread until all data was processed,that means
             internal  ts fifo has enough space to contain the data from gop parser 
  */
int   ts_seq_play(void * pHandle, char * p_data, unsigned int  length
    , int    v_pid, ts_seq_play_mode_t play_mode
    , ts_seq_input_mode_t input_mode, unsigned int * p_required);
/*!
  param[0]:[IN]   pointer to the memory buffer  where new segment will be put in
  parme[1]:[IN]   len of the  memory buffer
  RETURN VALUE:  return the actural number fetch from internal  fifo and this value is not bigger than len
  */
int  ts_seq_fetch(void * pHandle,unsigned char * p_data,int len);

/*!
  query the status of ts sequence parser
  */
ts_seq_status_t ts_seq_query(void * pHandle);

/*!
   DISCRIPTION:
      <1> if you want chang the current speed of trick play,you should stop current play mode  
                and then new speed can be set and take effect !!!!
      <2>  this function will block caller until the gop parser is really stopped
  */
void  ts_seq_stop(void * pHandle);

/*!
  DISCRIPTION:  if you change the direction of trickplay, this func must be called
     for example, from FF 32X to FR1X 
  */
void ts_seq_reset_play_direction(void * pHandle , BOOL isForward , video_codec_type_t type);
/*!
  select a parser
  */
void ts_seq_set_sequence_parser(void * pHandle, video_codec_type_t type);

/*!
  this api only for debug
  */
int  ts_seq_parse_vframe(void * pHandle,char * p_ts_packet,int pid);

/*!
  ts_seq_get_vkey_frame
  */
BOOL ts_seq_get_vkey_frame(unsigned char *p_ts_packet, 
                                             unsigned char *p_es_packet,
                                             unsigned int *p_len, 
                                             unsigned int es_data_buffer_size,
                                             int vpid, 
                                             video_codec_type_t type,
                                             unsigned char **pp_first_kframe_addr);

int ts_seq_play1(void * p_Handle, void * p_vHandle, char * p_data
        , unsigned int  length,int    v_pid, ts_seq_play_mode_t play_mode
        , ts_seq_input_mode_t input_mode, ts_detail_info_t *p_ts_detail);

unsigned int ts_get_first_pts(unsigned char *p_data, 
                                             unsigned int length, 
                                             unsigned int vpid);

/*!
  ts_seq_get_vkey_frame
  param[0]:[IN] : ts data
  param[0]:[IN] : data len
  param[0]:[IN] : codec type
  param[0]:[IN] : pid of video
  param[0]:[OUT] : the vkey pos
  */
int ts_seq_get_vkey_frame_vsb(char * p_ts_packet, unsigned int  length,
            video_codec_type_t type, int v_pid, ts_detail_info_t *p_ts_detail);

#endif
