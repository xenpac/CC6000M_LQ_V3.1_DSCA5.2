/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_LIST_H_
#define __FILE_LIST_H_

/*!
  Max file cnt
  */
#define MAX_FILE_CNT (500)
/*!
  Max sort info
  */
#define MAX_SORT_INFO (12)

/*!
  record fav lock flag
  */
#define REC_FAV_LOCK_FLAG         0x01
/*!
  record fav unlock flag
  */
#define REC_FAV_UNLOCK_FLAG       0x00
/*!
  record fav unlock temp flag
  */
#define REC_FAV_UNLOCK_TEMP_FLAG  0x02
/*!
  record fav lock flag operation
  */
#define REC_FAV_UNLOCK(x)         (x &= REC_FAV_UNLOCK_FLAG)
/*!
  record fav lock flag operation
  */
#define REC_FAV_UNLOCK_TEMP(x)    (x |= REC_FAV_UNLOCK_TEMP_FLAG)
/*!
  record fav lock flag operation
  */
#define REC_FAV_IS_LOCK(x)        (x & REC_FAV_LOCK_FLAG)
/*!
  record fav lock flag operation
  */
#define REC_FAV_IS_UNLOCK(x)      (!REC_FAV_IS_LOCK(x))
/*!
  record fav lock flag operation
  */
#define REC_FAV_SHOW_DISABLE(x)   ((x & REC_FAV_LOCK_FLAG) && (!(x & REC_FAV_UNLOCK_TEMP_FLAG)))
/*!
  record fav lock flag operation
  */
#define REC_FAV_SHOW_ENABLED(x)   (!(REC_FAV_SHOW_DISABLE(x)))

/*!
  file list dir handle
  */
typedef void * flist_dir_t;

/*!
  media fav item, used for music and picture
  */
typedef struct tag_music_pic_fav
{ 
  /*!
    next media fav
    */
  struct tag_music_pic_fav *p_next;
  /*!
    cur media fav path
    */
  u16 path[MAX_FILE_PATH];
  /*!
    filename with no path
    */
  u16 *p_filename;
  
}media_fav_t;

/*!
  rec fav item 
  */
typedef struct tag_rec_fav
{ 
  /*!
    next rec fav
    */
  struct tag_rec_fav *p_next;
  /*!
    cur media fav path
    */
  u16 path[MAX_FILE_PATH];
  /*!
    rec total time
    */
  u32 total_time;
  /*!
    lock_flag
    */
  u8 lock_flag;
  /*!
    ca_flag
    */
  u8 ca_flag;
  /*!
    reserved
    */
  u16 reserved;
  /*!
    program name
    */
  u16 program[PROGRAM_NAME_MAX];
  /*!
    start time
    */
  utc_time_t start;
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
  u16 ext_pid[7];
}rec_fav_t;

/*!
  medai fav type
  */
typedef enum tag_media_fav_type
{
  /*!
    media audio fav
    */
  AUDIO_FAV,
  /*!
    media pic fav
    */
  PIC_FAV,
  /*!
    media record fav
    */
  RECORD_FAV,
}media_fav_type_t;

/*!
  partion
  */
typedef struct tag_partition
{
  /*!
    partion letter like (C:)
    */
  u16 letter[3];
  /*!
    partion name
    */
  u16 name[13];
  /*!
    partion total size
    */
  u32 toatl_size;
  /*!
    partion free size
    */
  u32 free_size;
}partition_t;

/*!
  media file type
  */
typedef enum 
{
  /*!
    directory
    */
  DIRECTORY,
  /*!
    not dir file
    */
  NOT_DIR_FILE,
  /*!
    unknow file
    */
  UNKNOW_FILE
}file_type_t;

/*!
  file list option
  */
typedef enum
{
  /*!
    file list unit first
    */
  FLIST_UNIT_FIRST,
  /*!
    file list unit next
    */
  FLIST_UNIT_NEXT,
  /*!
    file list unit previous
    */
  FLIST_UNIT_PREV,
  /*!
    file list unit last
    */
  FLIST_UNIT_LAST
}flist_option_t;

/*!
  media file
  */
typedef struct tag_media_file
{
  /*!
    file type
    */
  file_type_t type;
  /*!
    size
    */
  u32 size;
  /*!
    file name with path
    */
  u16 name[MAX_FILE_PATH];
  /*!
    default order
    */
  u16 default_order;  
  /*!
    file name no path
    */
  u16 *p_name;
}media_file_t;

/*!
  file list
  */
typedef struct tag_file_list
{
  /*!
    count of media file
    */
  u32 file_count;
  /*!
    media file
    */
  media_file_t *p_file;
}file_list_t;

/*!
  file list
  */
typedef void * flist_t;

/*!
  flist_sort_t
  */
typedef enum
{
  /*!
    Sorting mp name from A to Z
    */
  A_Z_MODE,
  /*!
    Sorting mp name from Z to A
    */
  Z_A_MODE,
  /*!
    Default order
    */
  DEFAULT_ORDER_MODE,
}flist_sort_t;

/*!
  flist_type_t
  */
typedef enum 
{
  /*!
    MP3
    */
  FILE_TYPE_MP3,
  /*!
    jpeg
    */
  FILE_TYPE_JPG,
  /*!
    bitmap
    */
  FILE_TYPE_BMP,
  /*!
    ts file
    */
  FILE_TYPE_TS,
  /*!
    gif
    */
  FILE_TYPE_GIF,
  /*!
    png
    */
  FILE_TYPE_PNG,
  /*!
    unknow file
    */
  FILE_TYPE_UNKNOW,
}flist_type_t;

/*!
  file list init
  */
void file_list_init(void);

/*!
  file list init2
  */
void file_list_init2(u32 max_support_cnt);

/*!
  file list deinit
  */
void file_list_deinit(void);

/*!
  get available partition and count
  \param[in/out] pp_partion pointer to partition list
  */
u32 file_list_get_partition(partition_t **pp_partition);

/*!
  file list enter dir
  \param[in] p_filter media file type need
  \param[in] unit_cnt unit file list count
  \param[in] p_path directoty path
  */
flist_dir_t file_list_enter_dir(u16 *p_filter, u16 unit_cnt, u16 *p_path);

/*!
  get file list by option
  \param[in] option get option
  \param[in/out] p_list file list for result
  */
BOOL file_list_get(flist_dir_t dir, flist_option_t option, file_list_t *p_list);

/*!
  file list leave dir
  */
void file_list_leave_dir(flist_dir_t dir);

/*
  ************************new dir interface********************************
  */
/*!
  file list open dir
  \param[in] p_filter media file type need
  \param[in] p_path directoty path
  \param[in/out] dir filter total count
  */
flist_dir_t flist_open_dir(u16 *p_filter, u16 *p_path, u32 *p_total_cnt);

/*!
  get file list by index, return flist pointer
  \param[in] dir flist dir
  \param[in] index index for total
  */
flist_t * flist_get_by_index(flist_dir_t dir, u32 index);

/*!
  del file list by index, return TRUE/FALSE
  \param[in] dir flist dir
  \param[in] index index for total
  */
BOOL flist_del_by_index(flist_dir_t dir, u32 index);

/*!
  get file list file name(full path)
  \param[in] p_list flist pointer
  */
u16 * flist_get_name(flist_t *p_list);

/*!
  get file list file short name, no path
  \param[in] p_list flist pointer
  */
u16 * flist_get_short_name(flist_t *p_list);

/*!
  get file list file type
  \param[in] p_list flist pointer
  */
file_type_t flist_get_type(flist_t *p_list);

/*!
  get file list file size
  \param[in] p_list flist pointer
  */
u32 flist_get_size(flist_t *p_list);

/*!
  close flist dir
  \param[in] dir flist dir
  */
void file_list_close_dir(flist_dir_t dir);
/*
  ************************new dir interface********************************
  */

/*!
  get current path
  */
u16 * file_list_get_cur_path(flist_dir_t dir);

/*!
  load fav list, return fav cnt
  \param[in] partition_letter partition letter
  \param[in] pp_media_fav loaded fav list
  \param[in] type media fav type
  */
u32 flist_load_fav(u16 partition_letter, void **pp_media_fav, media_fav_type_t type);

/*!
  unload fav list
  \param[in] pp_media_fav loaded fav list
  */
void flist_unload_fav(void *p_media_fav);

/*!
  save fav list
  \param[in] partition_letter partition letter
  \param[in] pp_media_fav loaded fav list
  \param[in] type media fav type
  */
void flist_save_fav(u16 partition_letter, void *p_media_fav, media_fav_type_t type);

/*!
  add one fav, return hanlde of added
  \param[in] p_name need add item
  \param[in] p_media_fav list to add
  */
void * flist_add_fav_by_name(u16 *p_name, void **pp_media_fav, media_fav_type_t type);

/*!
  del one fav
  \param[in] p_fav_list fav list
  \param[in] p_fav_del fav need delete
  */
u8 flist_del_fav(void **pp_fav_list, void *p_fav_del);

/*!
  find media fav
  \param[in] file name
  \param[in] p_media_fav media fav list
  */
void * flist_get_fav_by_name(u16 *p_name, void *p_media_fav);

/*!
  get media by index

  \param[in] p_media : media list.
  \param[in] index : index

  \return media
  */
void * flist_get_fav_by_index(void *p_media, u16 index);

/*!
  get file size
  \param[in] p_name file path
  */
u64 file_list_get_file_size(u16 *p_name);

/*!
  rename file
  \param[in] p_old old name
  \param[in] p_new new name
  */
BOOL file_list_rename(u16 *p_old, u16 *p_new);

/*!
  delete file
  \param[in] p_name file to be delete
  */
BOOL file_list_delete_file(u16 *p_name);

/*!
  get file type

  \param[in] p_name file to be delete

  \return type
  */
flist_type_t flist_get_file_type(u16 *p_filename);

/*!
  file list sort.

  \param[in] p_sort : file list
  \param[in] cnt : cnt
  \param[in] sort type : type

  \return NULL
  */
void file_list_sort(file_list_t *p_sort, int cnt, flist_sort_t sort_type);

#endif // End of __FILE_LIST_H_

