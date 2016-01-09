/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MEDIA_DATA_H_
#define __MEDIA_DATA_H_

/*!
  ts packet length
  */
#define TS_PACKET_LEN (188)

/*!
  ts packet id
  */
#define TS_PID(data) \
  ((u16)(((u8)(data[2])) | \
         (((u16)((u8)(data[1] & 0x1F))) << 8)))

/*!
  payload unit  start
  */
#define PAYLOAD_UNIT_START(data) ((((u8)data[0]) == 0x47) && ((((u8)data[1]) & 0x40) == 0x40))

/*!
  video pre start code
  */
#define PRE_START_CODE(x) \
  ((((u8)(x)[0] == 0x00)) && (((u8)(x)[1]) == 0x00) && (((u8)(x)[2]) == 0x01))

/*!
  is pic start code
  */
#define IS_PICTURE_START_CODE(x) \
  (PRE_START_CODE(x) && ((u8)(x)[3] == 0x00))

/*!
  timeshift min unit
  */
#define TS_STREAM_RW_UNIT (512 * 188)

/*!
  max record file size
  */
#define MAX_RECORD_FILE_SIZE (0xFFFFFF00)//4G

/*!
  cas channel key max
  */
#define CAS_CH_KEY_MAX (8)

/*!
  program name max
  */
#define PROGRAM_NAME_MAX (32)

/*!
  record idx count
  */
#define REC_IDX_INFO_CNT (1200)

/*!
  sample context seek flag
  */
#define SAMPLE_CONTEXT_SEEK_FLAG (0x10)

/*!
  sample context trick mode flag
  */
#define SAMPLE_CONTEXT_RESET_FLAG (0x20)

/*!
  dynamic rec info
  */
typedef struct tag_dynamic_rec_info
{
  /*!
    unit data lengh = sizeof(dynamic_rec_info_t)
    */
  u32 unit_len: 8;
  /*!
    form 
    */
  u32 indx : 24;
  /*!
    cur time (ms)
    */
  u32 cur_time;
  /*!
    pts
    */
  u32 pts;
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
    vdec_src_fmt_t
    */
  u8 video_fmt;
  /*!
    adec_src_fmt_vsb_t
    */
  u8 audio_fmt;
  /*!
    vkey_frame_offset
    */
  u32 vkey_frame_offset;
}dynamic_rec_info_t;

/*!
  static record info
  */
typedef struct tag_static_rec_info
{
  /*!
    program name
    */
  u16 program[PROGRAM_NAME_MAX];
  /*!
    start time
    */
  utc_time_t start;
  /*!
    pmt pid
    */
  u16 pmt_pid;
  /*!
    reserved
    */
  u16 reserved;
  /*!
    reserved1
    */
  u32 reserved1 : 11;
  /*!
    lock flag
    */
  u32 lock_flag : 1;
  /*!
    encrypt_flag
    */
  u32 encrypt_flag : 3;
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
}static_rec_info_t;

/*!
  extern_pid_type
  */
typedef enum tag_extern_pid_type
{
  /*!
    si pid
    */
  EX_SI_PID = 0,
  /*!
    video pid
    */
  EX_VIDEO_PID,
  /*!
    audio pid
    */
  EX_AUDIO_PID,
  /*!
    user define pid 1
    */
  EX_UERS_PID_1,
  /*!
    user define pid 2
    */
  EX_UERS_PID_2,
  /*!
    user define pid 3
    */
  EX_UERS_PID_3,
  /*!
    user define pid 4
    */
  EX_UERS_PID_4,
  /*!
    user define pid 5
    */
  EX_UERS_PID_5,
}extern_pid_type_t;

/*!
  extern_pid
  */
typedef struct tag_extern_pid
{
  /*!
    pid
    */
  u32 pid : 16; 
  /*!
    extern_pid_type_t
    */
  u32 type : 8;
  /*!
    fmt if video or audio type
    */
  u32 fmt : 8;
}extern_pid_t;

/*!
  idx extern info version number
  *version history 20130708*
  */
#define IDX_INFO_VERSION (20130708)

/*!
  rec max extern pid
  */
#define REC_MAX_EXTERN_PID (24)
/*
  save user data length
*/
#define IDX_INFO_USER_DATA_LENGTH 128

/*!
  idx info offset new
  */
#define IDX_INFO_OFFSET_NEW (512)

/*!
  record idx extern info
  */
typedef struct tag_idx_extern_info
{
  /*!
    static rec info
    */
  static_rec_info_t st_rec_info;
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
    vdec_src_fmt_t
    */
  u8 video_fmt;
  /*!
    adec_src_fmt_vsb_t
    */
  u8 audio_fmt;
  /*!
    reserved
  */
  u16 reserved[2];
  /*!
    version number, IDX_INFO_OFFSET(128) align end, for version ctrl
    */
  u32 version_number;
  /*!
    reserved1
  */
  u16 reserved1;
  /*!
    extern pid num
    */
  u16 extern_num;
  /*!
    extern pid
    */
  extern_pid_t extern_pid[REC_MAX_EXTERN_PID];
  u8 user_data[IDX_INFO_USER_DATA_LENGTH];
  #ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
  /*
      cipher engine type
  */
  u32 cipher_engine;
  u8  cipher_key[ADVANCEDCA_MAXKEYSIZE];
  /*!
    reserved2
    */
  u8 reserved2[IDX_INFO_OFFSET_NEW - 132 - IDX_INFO_USER_DATA_LENGTH - 4 * REC_MAX_EXTERN_PID  - 4 - ADVANCEDCA_MAXKEYSIZE];
  #else
  /*!
    reserved2
    */
  u8 reserved2[IDX_INFO_OFFSET_NEW - 132 - IDX_INFO_USER_DATA_LENGTH - 4 * REC_MAX_EXTERN_PID];
  #endif
}idx_extern_info_t;

/*!
  idx info offset
  */
#define IDX_INFO_OFFSET (sizeof(static_rec_info_t) + 16)

/*!
  idx info lock flag offset
  */
#define IDX_INFO_LOCK_OFFSET (sizeof(static_rec_info_t) + 1)

/*!
  record info
  */
typedef struct tag_rec_info
{
  /*!
    program name
    */
  u16 program[PROGRAM_NAME_MAX];
  /*!
    start time
    */
  utc_time_t start;
  /*!
    pmt_pid
    */
  u32 pmt_pid : 16;
  /*!
    encrypt_flag
    */
  u32 encrypt_flag : 8;
  /*!
    lock_flag
    */
  u32 lock_flag : 8;
  /*!
    total time (second)
    */
  u32 total_time;
  /*!
    total size
    */
  u32 total_size;
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
    vdec_src_fmt_t
    */
  u8 video_fmt;
  /*!
    adec_src_fmt_vsb_t
    */
  u8 audio_fmt;
  /*!
    ext pid count
    */
  u16 ext_pid_cnt;
  /*!
    ext pid
    */
  extern_pid_t ext_pid[REC_MAX_EXTERN_PID];
  u8 user_data[IDX_INFO_USER_DATA_LENGTH];
}rec_info_t;

/*!
  rec_info_type
  */
typedef enum tag_rec_info_type
{
  /*!
    rec info unknow
    */
  REC_INFO_UNKONOWN = 0,
  /*!
    rec dynamic info(max 180)
    */
  REC_INFO_DYNAMIC,
  /*!
    rec static info
    */
  REC_INFO_STATIC,
  /*!
    rec dummy info
    */
  REC_INFO_DUMMY,
}rec_info_type_t;

/*!
  media_idx_open_mode_t
  */
typedef enum tag_media_idx_open_mode
{
  /*!
    read
    */
  MEDIA_IDX_READ = 0,
  /*!
    write
    */
  MEDIA_IDX_WRITE,
  /*!
    time shift
    */
  MEDIA_IDX_TS,
}media_idx_open_mode_t;

/*!
  media_array_t
  */
typedef struct tag_media_idx
{
  /*!
    sem
    */
  os_sem_t sem;
  /*!
    unit_size
    */
  u32 unit_size : 8;
  /*!
    media_idx_open_mode_t
    */
  u32 open_mode : 7;
  /*!
    media_idx_t malloc
    */
  u32 is_malloc : 1;
  /*!
    start offset
    */
  u32 start_offset : 16;
  /*!
    file handle, if the file handle is NULL, means no need save file
    */
  void *file;
  /*!
    idx_extern_info_t
    */
  idx_extern_info_t extern_info;
  /*!
    buffer
    */
  dynamic_rec_info_t *p_buffer;
  /*!
    unit_total
    */
  u32 unit_total;
  /*!
    get_cnt
    */
  u32 get_cnt;
  /*!
    flush_cnt
    */
  u32 flush_cnt;
  /*!
    last_query
    */
  u32 last_query;
  /*!
    file end index
    */
  u32 file_end_index;
  /*!
    unit_size in file data
    */
  u32 file_data_last_read_index;
}media_idx_t;

/*!
  packet is rec info or not, return rec_info_type_t
  \param[in] p_packet ts packet
  \param[in/out] p_total_cnt total rec packet cnt
  \param[in/out] p_cur cur rec packet
  */
rec_info_type_t is_rec_info_packet(u8 *p_packet, u8 *p_total_cnt, u8 *p_cur_index);

/*!
  calc_split_packet_length
  \param[in] info_len len
  */
u32 calc_split_packet_length(u32 info_len);

/*!
  packet rec info, return packet len
  \param[in/out] p_data packet for add rec info
  \param[in] info_len rec info len
  */
u32 packet_dummy_info(u8 *p_packet, u32 info_len);

/*!
  packet rec info, return packet len
  \param[in/out] p_data packet for add rec info
  \param[in] rec_info_type static rec info or dynamic rec info
  \param[in] p_info rec info
  \param[in] info_len rec info len
  */
u32 packet_rec_info(u8 *p_packet, rec_info_type_t rec_info_type, u8 *p_info, u32 info_len);

/*!
  unpacket rec info, return true if all found
  \param[in] p_packet rec info packet
  \param[in/out] p_rec_info player rec info for ts player
  */
BOOL unpacket_rec_info(u8 *p_packet, u8 *p_rec_info, u32 *p_payload);

/*!
  parse pcr
  \param[in] p_data ts packet
  \param[in/out] p_pcr pcr pointer
  */
BOOL parse_pcr(u8 *p_data, unsigned long long *p_pcr);

/*!
  parse bit rate
  \param[in] p_data ts packet
  \param[in] payload
  \param[in] pcr_pid
  */
u32 parse_bit_rate(u8 *p_data, u32 payload, u16 pcr_pid);

/*!
  parse pts, return high 32 bits of pts
  \param[in] p_data ts packet
  \param[in] payload payload
  \param[in] v_pid pid
  */
u32 parse_pts(u8 *p_ts_packet, u32 payload, u16 v_pid);

/*!
  get idx info file name
  \param[in] p_ts_file_name ts file name
  \param[in/out] p_idx_file_name idx info file name
  \param[in] idx_name_len p_idx_file_name len
  */
void get_idx_info_file(const u16 *p_ts_file_name, u16 *p_idx_file_name, u32 idx_name_len);

/*!
  file sink split file name
  \param[in/out] p_file_name file name
  \param[in] index file count
  */
void file_sink_split_file_name(u16 *p_file_name, u32 index);

/*!
  parse pes data
  \param[in] p_ts_packet ts packet
  \param[in/out] p_pes_data pes data pointer
  \param[in/out] p_payload pes data payload
  */
void parse_pes_data(u8 *p_ts_packet, u8 **pp_pes_data, u32 *p_payload);

/*!
  parse stream
  \param[in] p_file_name file name to be parsed
  \param[in/out] p_a_pid audio pid parsed
  \param[in/out] p_v_pid video pid parsed
  \param[in/out] p_pcr_pid video pid parsed
  */
u32 parse_stream(u16 *p_file_name, u16 *p_a_pid, u16 *p_v_pid, u16 *p_pcr_pid);

/*!
  parse record info
  \param[in/out] p_rec_info record info parsed
  \param[in] p_name file name to be parsed
  */
BOOL parse_record_info(rec_info_t *p_rec_info, u16 *p_name);

/*!
  parse record info by idx file
  \param[in/out] p_rec_info record info parsed
  \param[in] p_name file name to be parsed
  */
BOOL parse_record_info_idx(rec_info_t *p_rec_info, u16 *p_idx_name);

/*!
  get record info lock flag
  \param[in] p_name file name to be parsed
  */
u8 record_info_idx_get_lock_flag(u16 *p_idx_name);

/*!
  change record info lock flag
  \param[in] p_name file name to be parsed
  */
void record_info_idx_change_lock_flag(u16 *p_idx_name);

/*!
  parse mp3 time long (seconds)
  \param[in] p_name file name to be parsed
  \param[in] file_size file size
  */
u32 parse_mp3_time_long(u16 *p_name);

/*!
  media_idx_create
  \param[in] p_array media_idx handle
  \param[in] unit_total unit_total
  \param[in] p_name save file, if it's valid, flush info the disk
  \param[in] p_ridx_buf p_ridx_buf
  \param[in] ridx_buf_size ridx_buf_size
  */
BOOL media_idx_create(media_idx_t **pp_array,
  u32 unit_total, u16 *p_name, u8 *p_ridx_buf, u32 ridx_buf_size);

/*!
  media_idx_load
  \param[in] p_array media_idx handle
  \param[in] unit_total unit_total
  \param[in] p_name save file
  \param[in] p_ridx_buf p_ridx_buf
  \param[in] ridx_buf_size ridx_buf_size
  */
BOOL media_idx_load(media_idx_t **pp_array,
  u32 unit_total, u16 *p_name, u8 *p_ridx_buf, u32 ridx_buf_size);

/*!
  media_idx_destory
  \param[in] p_array media_idx handle
  */
void media_idx_destory(media_idx_t *p_array);

/*!
  media_idx_read_by_index
  \param[in] p_array media_idx handle
  \param[in] index [0 ~ n]
  */
dynamic_rec_info_t *media_idx_read_by_index(media_idx_t *p_array, u32 index);

/*!
  media_idx_read_by_pts
  \param[in] p_array media_idx handle
  \param[in] pts 
  \param[in] to_end , scan direction,  
  */
dynamic_rec_info_t *media_idx_found_by_pts(media_idx_t *p_array, u32 pts);


/*!
  media_idx_read_by_pts
  \param[in] p_array media_idx handle
  \param[in] pts 
  \param[in] start_index , search start index
  \param[in] b_forward , search dir
  */
dynamic_rec_info_t *media_idx_found_by_pts_ex(media_idx_t *p_array,
  u32 pts, u32 start_index, BOOL b_forward);

/*!
  media_idx_read_by_pts
  \param[in] p_array media_idx handle
  \param[in] ms ms
  \param[in] to_end , scan direction,  
  */
dynamic_rec_info_t *media_idx_found_by_ms(media_idx_t *p_array, u32 ms);

/*!
  media_idx_write
  \param[in] p_array media_idx handle
  \param[in] p_st_info p_st_info
  */
void media_idx_write_extern_info(media_idx_t *p_array, idx_extern_info_t *p_extern_info);

/*!
  media_idx_write
  \param[in] p_array media_idx handle
  \param[in] p_st_info p_st_info
  */
s32 media_idx_check_index_range(media_idx_t *p_array, u32 index);
/*!
  media_idx_write
  \param[in] p_array media_idx handle
  \param[in] p_item p_item
  return index
  */
u32 media_idx_write(media_idx_t *p_array, dynamic_rec_info_t *p_item);

/*!
  media_idx_reload
  \param[in] p_array media_idx handle
  \param[in] unit_total unit_total
  \param[in] p_name save file
  */
BOOL media_idx_reload(media_idx_t *p_array, u32 start_index);

/*!
  media_idx_split_file
  \param[in] p_array media_idx handle
  \param[in] p_name save file
  */
u32 media_idx_split_file(media_idx_t *p_array, u16 *p_name);

/*!
  media_idx_split_file
  \param[in] p_array media_idx handle
  \param[in] p_name save file
  */
void media_idx_flush(media_idx_t *p_array);
#endif // End of __MEDIA_DATA_H_

