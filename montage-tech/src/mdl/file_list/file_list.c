/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"

#include "drv_dev.h"
#include "lib_util.h"
#include "lib_unicode.h"
#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_event.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_mutex.h"

#include "class_factory.h"

#include "mp3_decode_frame.h"
#include "mt_time.h"
#include "vfs.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"
#include "file_list.h"
#include "iconv_ext.h"
#include "err_check_def.h"

/*!
  is audio stream
  */
#define IS_AUDIO_STREAM(stream_id)\
  ((stream_id >= 0xc0) && (stream_id <= 0xdf))

/*!
  is video stream
  */
#define IS_VIDEO_STREAM(stream_id)\
  ((stream_id >= 0xe0) && (stream_id <= 0xef))

/*
  file list save count
  */
#define FLIST_SAVE_CNT (100)

/*!
  priv list
  */
typedef struct tag_priv_list
{
  /*!
    media file list
    */
  media_file_t *p_file;
  /*!
    index
    */
  u16 index;
  /*!
    next
    */
  struct tag_priv_list *p_next;
}priv_list_t;

/*!
  private data
  */
typedef struct tag_file_list_priv
{
  /*!
    partions
    */
  partition_t partition[10];
  /*!
    file list max support count
    */
  u32 max_support_cnt;
  /*!
    opened dir
    */
  void *p_opened_dir;
}file_list_priv_t;

/*!
  file_list_dir_t
  */
typedef struct tag_file_list_dir_t
{
  /*!
    file filter
    */
  u16 filter[MAX_FILE_PATH];
  /*!
    priv list
    */
  priv_list_t *p_priv_list;
  /*!
    current dir
    */
  hdir_t cur_dir;
  /*!
    current path
    */
  u16 cur_path[MAX_FILE_PATH];
  /*!
    unit file cnt
    */
  u16 unit_cnt;
  /*!
    cur index
    */
  u16 cur_index;
  /*!
    all ready
    */
  BOOL all_ready;
  /*!
    ready cnt
    */
  u32 ready_cnt;
}file_list_dir_t;

/*!
  priv list
  */
typedef struct tag_flist_unit
{
  /*!
    media file list
    */
  media_file_t file[FLIST_SAVE_CNT];
  /*!
    next
    */
  struct tag_flist_unit *p_next;
}flist_unit_t;

/*!
  file list dir priv
  */
typedef struct tag_flist_dir_priv
{
    /*!
    current dir
    */
  hdir_t vfs_dir;
  /*!
    file list unit
    */
  flist_unit_t *p_flist;
  /*!
    start index
    */
  u32 start_idx;
  /*!
    total cnt
    */
  u32 total_cnt;
  /*!
    file filter
    */
  u16 filter[MAX_FILE_PATH];
  /*!
    current path
    */
  u16 cur_path[MAX_FILE_PATH];
}flist_dir_priv_t;

/*!
  media_fav_header
  */
typedef struct tag_media_fav_header
{
  /*!
    media fav payload
    */
  u32 payload;
}media_fav_header_t;

typedef BOOL (*qsort_func)(media_file_t * prev_node, media_file_t * cur_node);

#ifdef WIN32
#define STRCMPI strcmpi
#else 
#define STRCMPI strcasecmp
#endif

u32 flist_load_fav(u16 partition_letter, void **pp_media_fav, media_fav_type_t type)
{
  u16 file_name[MAX_FILE_PATH] = {0};
  hfile_t file = NULL;
  vfs_file_info_t file_info = {0};
  u32 read_size = 0;
  u32 fav_cnt = 0;
  media_fav_t *p_media_fav = NULL;
  media_fav_t *p_prev = NULL;
  media_fav_header_t head = {0};
  u32 media_fav_len = 0;
  u16 *p_tip = NULL;
  u8 *p_buffer = NULL;
  u8 *p_r_buf = NULL;
  
  file_name[0] = partition_letter;
  
  switch(type)
  {
    case AUDIO_FAV:
      str_asc2uni(":\\audio_fav_list", file_name + 1);
      media_fav_len = sizeof(media_fav_t);
      break;

    case PIC_FAV:
      str_asc2uni(":\\pic_fav_list", file_name + 1);
      media_fav_len = sizeof(media_fav_t);
      break;

    case RECORD_FAV:
      str_asc2uni(":\\PVRS\\mt_record_list", file_name + 1);
      media_fav_len = sizeof(rec_fav_t);
      break;

    default:
      return 0;
  }
  
  file = vfs_open(file_name, VFS_READ);
  if(file == NULL)
  {
    return 0;
  }

  vfs_get_file_info(file, &file_info);

  if(!file_info.file_size)
  {
    vfs_close(file);
    return 0;
  }

  p_buffer = mtos_align_malloc(file_info.file_size + 4, 16);
  if (p_buffer != NULL)
  {
    memset(p_buffer, 0, file_info.file_size + 4);
    read_size = vfs_read(p_buffer, file_info.file_size, 1, file);
    if (read_size != file_info.file_size)
    {
      mtos_align_free(p_buffer);
      vfs_close(file);
      return 0;
    }
    
    read_size = 0;
    p_r_buf = p_buffer;
    while (read_size < file_info.file_size)
    {
     memcpy((u8 *)(&head), p_r_buf, sizeof(media_fav_header_t));
     p_r_buf += sizeof(media_fav_header_t);
     read_size += sizeof(media_fav_header_t);
      if ((read_size >= file_info.file_size)
        || !head.payload)
      {
        break;
      }
      
      if (head.payload >= media_fav_len)
      {
        break;
      }
      
      p_media_fav = mtos_malloc(media_fav_len);
      CHECK_FAIL_RET_ZERO(p_media_fav != NULL);
      
      memcpy((u8 *)((u32)p_media_fav + 4), p_r_buf, head.payload);
      read_size += head.payload;
      p_r_buf += head.payload;
      p_media_fav->path[0] = partition_letter;
      if((type != RECORD_FAV))
      {
         p_tip = uni_strrchr(p_media_fav->path, 0x5c/*'\\'*/);
         p_media_fav->p_filename = ++ p_tip;
      }
      
      p_media_fav->p_next = NULL;

      if (fav_cnt == 0)
      {
        *pp_media_fav = p_media_fav;
      }
      else
      {
        p_prev->p_next = p_media_fav;
      }
      fav_cnt ++;
      p_prev = p_media_fav;
    }
    mtos_align_free(p_buffer);
    vfs_close(file);
    return fav_cnt;
  }
  
  while (1)
  {
    read_size = vfs_read((u8 *)(&head), sizeof(media_fav_header_t), 1, file);
    if (read_size != sizeof(media_fav_header_t)
      || !head.payload)
    {
      break;
    }
    
    if (head.payload >= media_fav_len)
    {
      break;
    }
    
    p_media_fav = mtos_malloc(media_fav_len);
    CHECK_FAIL_RET_ZERO(p_media_fav != NULL);
    
    read_size = vfs_read((u8 *)p_media_fav + 4, head.payload, 1, file);
    p_media_fav->path[0] = partition_letter;
    if((type != RECORD_FAV))
    {
       p_tip = uni_strrchr(p_media_fav->path, 0x5c/*'\\'*/);
       p_media_fav->p_filename = ++ p_tip;
    }
    
    if (read_size != (media_fav_len - 4))
    {
      break;
    }
    p_media_fav->p_next = NULL;

    if (fav_cnt == 0)
    {
      *pp_media_fav = p_media_fav;
    }
    else
    {
      p_prev->p_next = p_media_fav;
    }
    fav_cnt ++;
    p_prev = p_media_fav;
  }
  
  vfs_close(file);
  return fav_cnt;
}

void flist_unload_fav(void *p_media_fav)
{
  media_fav_t *p_first = NULL;
  media_fav_t *p_next = NULL;

  p_first = (media_fav_t *)p_media_fav;
  while (p_first)
  {
    p_next = p_first;
    p_first = p_next->p_next;
    mtos_free(p_next);
    p_next = NULL;
  }

  p_media_fav = NULL;
}

void flist_save_fav(u16 partition_letter, void *p_media_fav, media_fav_type_t type)
{
  u16 file_name[MAX_FILE_PATH] = {0};
  media_fav_header_t head = {0};
  hfile_t file = NULL;
  media_fav_t *p_next = NULL;
  u8 *p_buffer = NULL;
  u8 *p_w_buf = NULL;
  u32 total_len = 0;
  
  file_name[0] = partition_letter;
  file_name[1] = 0x3a/*'':*/;

  switch (type)
  {
    case AUDIO_FAV:
      str_asc2uni("\\audio_fav_list", &file_name[2]);
      break;

    case PIC_FAV:
      str_asc2uni("\\pic_fav_list", &file_name[2]);
      break;

    case RECORD_FAV:
      str_asc2uni("\\PVRS\\mt_record_list", &file_name[2]);
      break;

    default:
      return;
  }

  vfs_del(file_name);

  if (p_media_fav == NULL)
  {
    return;
  }
  
  file = vfs_open(file_name,  VFS_NEW);
  if(file == NULL)
  {
    return;
  }
  
  if (type == RECORD_FAV)
  {
    head.payload = sizeof(rec_fav_t) - 4;
  }
  else
  {
    head.payload = sizeof(media_fav_t) - 4;
  }

  p_next = (media_fav_t *)p_media_fav;
  while (p_next)
  {
    total_len ++;
    p_next = p_next->p_next;
  }
  total_len *= (sizeof(media_fav_header_t) + head.payload);
  
  p_buffer = mtos_align_malloc(total_len, 16);
  if (p_buffer != NULL)
  {
    memset(p_buffer, 0, total_len);
    p_w_buf = p_buffer;
    p_next = (media_fav_t *)p_media_fav;
    while (p_next)
    {
      memcpy(p_w_buf, (u8 *)(&head), sizeof(media_fav_header_t));
      p_w_buf += (sizeof(media_fav_header_t));
      memcpy(p_w_buf, (u8 *)((u32)p_next + 4), head.payload);
      p_w_buf += head.payload;
      p_next = p_next->p_next;
    }
    vfs_write(p_buffer, total_len, 1, file);
    mtos_align_free(p_buffer);
    vfs_close(file);
    return ;
  }

  p_next = (media_fav_t *)p_media_fav;
  while (p_next)
  {
    vfs_write((u8 *)(&head), sizeof(media_fav_header_t), 1, file);
    vfs_write((u8 *)((u32)p_next + 4), head.payload, 1, file);
    p_next = p_next->p_next;
  }
  vfs_close(file);
}

void * flist_add_fav_by_name(u16 *p_name, void **pp_media_fav, media_fav_type_t type)
{
  media_fav_t *p_fav_next = *(media_fav_t **)pp_media_fav;
  u32 next_len = 0;
  u16 *p_tip = NULL;

  CHECK_FAIL_RET_NULL(p_name != NULL);

  if (type == RECORD_FAV)
  {
    next_len = sizeof(rec_fav_t);
  }
  else
  {
    next_len = sizeof(media_fav_t);
  }
  
  while (p_fav_next)
  {
    if (!uni_strcmp(p_name, p_fav_next->path))
    {
      OS_PRINTF("\n##debug: flist_add_fav_by_name [%s] exist!!\n", p_name);
      return p_fav_next;
    }

    if (p_fav_next->p_next == NULL)
    {
      break;
    }
    p_fav_next = p_fav_next->p_next;
  }

  if (p_fav_next)
  {
    p_fav_next->p_next = mtos_malloc(next_len);
    CHECK_FAIL_RET_NULL(p_fav_next->p_next != NULL);
    p_fav_next = p_fav_next->p_next;
  }
  else
  {
    p_fav_next = mtos_malloc(next_len);
    CHECK_FAIL_RET_NULL(p_fav_next != NULL);
    *pp_media_fav = p_fav_next;
  }

  p_fav_next->p_next = NULL;
  memset(p_fav_next, 0, next_len);
  uni_strcpy(p_fav_next->path, p_name);

  if(type != RECORD_FAV)
  {
    p_tip = uni_strrchr(p_fav_next->path, 0x5c/*'\'*/);
    p_fav_next->p_filename = ++ p_tip;
  }
  return p_fav_next;
}

u8 flist_del_fav(void **pp_fav_list, void *p_fav_del)
{
  media_fav_t *p_next = *pp_fav_list;
  media_fav_t *p_prev = NULL;

  CHECK_FAIL_RET_ZERO(*pp_fav_list != NULL);
  CHECK_FAIL_RET_ZERO(p_fav_del != NULL);

  while (p_next)
  {
    if (!uni_strcmp(p_next->path, ((media_fav_t *)p_fav_del)->path))
    {
      if (p_prev)
      {
        p_prev->p_next = p_next->p_next;
      }
      else
      {
        *pp_fav_list = p_next->p_next;
      }
      mtos_free(p_next);
      p_next = NULL;
      p_fav_del = NULL;
      return 1;
    }
    p_prev = p_next;
    p_next = p_next->p_next;
  }

  return 0;
}

void * flist_get_fav_by_name(u16 *p_name, void *p_media_fav)
{
  media_fav_t *p_next = (media_fav_t *)p_media_fav;

  while (p_next)
  {
    if (uni_strstr(p_next->path, p_name))
    {
      return p_next;
    }

    p_next = p_next->p_next;
  }
  return NULL;
}

void * flist_get_fav_by_index(void *p_media, u16 index)
{
  media_fav_t *p_tmp = (media_fav_t *)p_media;
  u16 loopi = 0;

  for (loopi = 0; loopi < index; loopi ++)
  {
    CHECK_FAIL_RET_NULL(p_tmp != NULL);
    
    p_tmp = p_tmp->p_next;
  }

  return p_tmp;
}

u64 file_list_get_file_size(u16 *p_name)
{
  hfile_t file = NULL;
  vfs_file_info_t file_info = {0};

  file = vfs_open(p_name, VFS_READ);
  if (file == NULL)
  {
    return 0;
  }

  vfs_get_file_info(file, &file_info);
  vfs_close(file);

  return file_info.file_size;
}

BOOL file_list_rename(u16 *p_old, u16 *p_new)
{
  u16 *p_old_temp = p_old;
  u16 *p_new_temp = p_new;
  
  CHECK_FAIL_RET_FALSE(p_old_temp != NULL);
  CHECK_FAIL_RET_FALSE(p_new_temp != NULL);
#ifndef WIN32
  if (p_old_temp[1] == 0x3A
    && (p_old_temp[2] == 0x5C || p_old_temp[2] == 0x2F))//:\ or :/
  {
    p_old_temp += 3;
  }
  
  if (p_new_temp[1] == 0x3A
    && (p_new_temp[2] == 0x5C || p_new_temp[2] == 0x2F))//:\ or :/
  {
    p_new_temp += 3;
  }
#endif
  if(vfs_rename(p_old_temp, p_new_temp) == ERR_FAILURE)
  {
    return FALSE;
  }

  return TRUE;
}

BOOL file_list_delete_file(u16 *p_name)
{
  CHECK_FAIL_RET_FALSE(p_name != NULL);

  if (vfs_del(p_name) == ERR_FAILURE)
  {
    return FALSE;
  }

  return TRUE;
}

static BOOL is_filter_nodir(u16 *p_filter)
{
  u16 *p_ch = NULL;
  u16 tip[8] = {0};
  u16 filter[MAX_FILE_PATH] = {0};
  u16 nodir[8] = {0};

  uni_strncpy(filter, p_filter, MAX_FILE_PATH);
  str_asc2uni(".nodir", nodir);
  uni_strcpy(tip, nodir);
  while (1)
  {
    p_ch = uni_strrchr(filter, 0x7c/*'|'*/);
    if (p_ch == NULL)
    {
      return FALSE;
    }
    p_ch[0] = 0x2e/*'.'*/;
    if (uni_strcmp(p_ch, tip) == 0)
    {
      return TRUE;
    }
    p_ch[0] = 0/*'\0'*/;
  }

  //return FALSE;
}

static BOOL is_filter_file(u16 *p_filter, u16 *p_name)
{
  u16 *p_ch = NULL;
  u16 *p_tip = NULL;
  u16 filter[MAX_FILE_PATH] = {0};

  uni_strncpy(filter, p_filter, MAX_FILE_PATH);

  p_tip = uni_strrchr(p_name, 0x2e/*'.'*/);
  if (p_tip == NULL)
  {
    return FALSE;
  }

  while (1)
  {
    p_ch = uni_strrchr(filter, 0x7c/*'|'*/);
    if (p_ch == NULL)
    {
      return FALSE;
    }
    p_ch[0] = 0x2e/*'.'*/;
    if (uni_strcmp(p_ch, p_tip) == 0)
    {
      return TRUE;
    }
    p_ch[0] = 0/*'\0'*/;
  }

  //return FALSE;
}

static void check_dir_order(media_file_t *p_file, u32 file_cnt)
{
  s32 loopi = 0;
  u32 ex_index = 0;
  media_file_t file;

  memset(&file, 0, sizeof(media_file_t));

  if (p_file[file_cnt -2].type == DIRECTORY)
  {
    return ;
  }

  for (loopi = file_cnt - 2; loopi >= 0; loopi --)
  {
    if (p_file[loopi].type == DIRECTORY)
    {
      ex_index = loopi + 1;
      break;
    }
  }

  if (loopi == -1)
  {
    ex_index = 0;
  }

  memcpy(&file, &(p_file[file_cnt - 1]), sizeof(media_file_t));
  memcpy(&(p_file[file_cnt - 1]), &(p_file[ex_index]), sizeof(media_file_t));
  memcpy(&(p_file[ex_index]), &file, sizeof(media_file_t));

  p_file[file_cnt - 1].p_name = uni_strrchr(p_file[file_cnt - 1].name, 0x5c/*'\\'*/) + 1;
  p_file[file_cnt - 1].default_order = file_cnt - 1;
  p_file[ex_index].p_name = uni_strrchr(p_file[ex_index].name, 0x5c/*'\\'*/) + 1;
  p_file[ex_index].default_order = ex_index;

}

static u32 get_new_file(file_list_dir_t *p_flist_dir, media_file_t *p_file)
{
  RET_CODE ret = 0;
  vfs_dir_info_t file_info;
  u32 file_cnt = 0;
  u32 fname_len = 0, fpath_len = 0;
  u16 uni_str[8] = {0};

  file_cnt = 0;
  while (file_cnt < p_flist_dir->unit_cnt)
  {
    ret = vfs_readdir (p_flist_dir->cur_dir, &file_info);

    if (ret != SUCCESS)
     //|| file_info.fname[0] == 0)
    {
      OS_PRINTF("\n##debug: ufs_readdir end!\n");
      break;
    }

    fpath_len = uni_strlen(p_flist_dir->cur_path);
    fname_len = uni_strlen(file_info.fname);

    if(fpath_len + fname_len + 1 <  MAX_FILE_PATH)
    {
      memset(p_file[file_cnt].name, 0, MAX_FILE_PATH * sizeof(u16));
      uni_strcpy(p_file[file_cnt].name, p_flist_dir->cur_path);

      if (p_file[file_cnt].name[uni_strlen(p_file[file_cnt].name) - 1] != 0x5c/*'\\'*/)
      {
        uni_str[0] = 0x5c;
        uni_str[1] = 0;
        uni_strcat(p_file[file_cnt].name, uni_str, MAX_FILE_PATH);
      }

      if ((file_info.fattrib & VFS_DIR) && !is_filter_nodir(p_flist_dir->filter))
      {
        uni_str[0] = 0x2e;
        uni_str[1] = 0;
        if (uni_strcmp(file_info.fname, uni_str))
        {
          uni_strcat(p_file[file_cnt].name, file_info.fname, MAX_FILE_PATH);
          p_file[file_cnt].p_name = uni_strrchr(p_file[file_cnt].name, 0x5c/*'\\'*/) + 1;

          p_file[file_cnt].type = DIRECTORY;
          p_file[file_cnt].size = file_info.fsize;
          p_file[file_cnt].default_order = file_cnt;
          file_cnt ++;
          if (file_cnt >= 2)
          {
            check_dir_order(p_file, file_cnt);
          }
        }
      }
      else if (p_flist_dir->filter[0] == 0/*'\0'*/
        || is_filter_file(p_flist_dir->filter, file_info.fname))
      {
        uni_strcat(p_file[file_cnt].name, file_info.fname, MAX_FILE_PATH);
        p_file[file_cnt].p_name = uni_strrchr(p_file[file_cnt].name, 0x5c/*'\\'*/) + 1;
        p_file[file_cnt].type = NOT_DIR_FILE;
        p_file[file_cnt].size = file_info.fsize;
        p_file[file_cnt].default_order = file_cnt;
        file_cnt ++;
      }
    }
  }

  /*for(cunt = 0;  cunt < (file_cnt + 1); cunt ++)
  {
     p_file[cunt].default_order = cunt;
  }*/

  return file_cnt;
}

static void get_first_unit(file_list_dir_t *p_dir, file_list_t *p_list)
{
  if (p_dir->ready_cnt == 0 && p_dir->all_ready == FALSE)
  {
    p_dir->p_priv_list
      = mtos_malloc(sizeof(priv_list_t) + p_dir->unit_cnt * sizeof(media_file_t));
    CHECK_FAIL_RET_VOID(p_dir->p_priv_list != NULL);
    p_dir->p_priv_list->p_file = (media_file_t *)((u32)p_dir->p_priv_list + sizeof(priv_list_t));
    p_dir->p_priv_list->p_next = NULL;

    p_dir->ready_cnt = get_new_file(p_dir, p_dir->p_priv_list->p_file);
    p_dir->p_priv_list->index = 0;
    if (p_dir->ready_cnt < p_dir->unit_cnt)
    {
      p_dir->all_ready = TRUE;

      if (p_dir->ready_cnt == 0)
      {
        mtos_free(p_dir->p_priv_list);
        p_dir->p_priv_list = NULL;
      }
    }
  }

  if (p_dir->ready_cnt == 0)
  {
    p_list->p_file = NULL;
    p_list->file_count = 0;
    return ;
  }

  p_dir->cur_index = 0;
  p_list->p_file = p_dir->p_priv_list->p_file;
  p_list->file_count
    = (p_dir->ready_cnt < p_dir->unit_cnt ? p_dir->ready_cnt : p_dir->unit_cnt);

}

static void get_next_unit(file_list_dir_t *p_dir, file_list_t *p_list)
{
  priv_list_t *p_priv_list = NULL;
  u32 file_cnt = 0;

  if (p_dir->ready_cnt == 0)
  {
    p_list->p_file = NULL;
    p_list->file_count = 0;
    return ;
  }

  p_priv_list = p_dir->p_priv_list;
  while (p_priv_list != NULL)
  {
    if (p_dir->cur_index == p_priv_list->index)
    {
      if (p_priv_list->p_next == NULL)
      {
        if (p_dir->all_ready == TRUE)
        {
          p_list->p_file = NULL;
          p_list->file_count = 0;
          return ;
        }
        break;
      }
      else
      {
        p_dir->cur_index ++;
        p_list->p_file = p_priv_list->p_next->p_file;
        file_cnt = p_dir->cur_index * p_dir->unit_cnt;
        file_cnt = p_dir->ready_cnt - file_cnt;
        p_list->file_count = (file_cnt < p_dir->unit_cnt ? file_cnt : p_dir->unit_cnt);
        return ;
      }
    }

    p_priv_list = p_priv_list->p_next;
  }

  p_priv_list->p_next
    = mtos_malloc(sizeof(priv_list_t) + p_dir->unit_cnt * sizeof(media_file_t));
  CHECK_FAIL_RET_VOID(p_priv_list->p_next != NULL);
  p_priv_list->p_next->p_file = (media_file_t *)(p_priv_list->p_next + sizeof(priv_list_t));
  p_priv_list = p_priv_list->p_next;
  p_priv_list->index = p_dir->cur_index + 1;
  p_priv_list->p_next = NULL;

  file_cnt = get_new_file(p_dir, p_priv_list->p_file);


  if (file_cnt < p_dir->unit_cnt)
  {
    p_dir->all_ready = TRUE;
    if (file_cnt == 0)
    {
      mtos_free(p_priv_list);
      p_priv_list = NULL;
      p_list->p_file = NULL;
      p_list->file_count = 0;
      return ;
    }
  }

  p_dir->ready_cnt += file_cnt;
  p_dir->cur_index ++;

  p_list->p_file = p_priv_list->p_file;
  p_list->file_count = file_cnt;

}

static void get_prev_unit(file_list_dir_t *p_dir, file_list_t *p_list)
{
  priv_list_t *p_priv_list = NULL;

  if (p_dir->ready_cnt == 0)
  {
    p_list->p_file = NULL;
    p_list->file_count = 0;
    return ;
  }

  if (p_dir->cur_index == 0)
  {
    p_list->p_file = p_dir->p_priv_list->p_file;
    p_list->file_count
      = (p_dir->ready_cnt < p_dir->unit_cnt ? p_dir->ready_cnt : p_dir->unit_cnt);
    return ;
  }

  p_priv_list = p_dir->p_priv_list;
  while (p_priv_list != NULL)
  {
    if ((p_dir->cur_index - 1) == p_priv_list->index)
    {
        p_dir->cur_index --;
        p_list->p_file = p_priv_list->p_file;
        p_list->file_count = p_dir->unit_cnt;
        return ;
    }

    p_priv_list = p_priv_list->p_next;
  }

}

static void get_last_unit(file_list_dir_t *p_dir, file_list_t *p_list)
{
  priv_list_t *p_priv_list = NULL;
  u32 file_cnt = 0;
  u16 last_idx = 0;

  if (p_dir->ready_cnt == 0)
  {
    p_list->p_file = NULL;
    p_list->file_count = 0;
    return ;
  }

  last_idx = (p_dir->ready_cnt - 1) / p_dir->unit_cnt;
  p_priv_list = p_dir->p_priv_list;
  while (p_priv_list != NULL)
  {
    if (last_idx == p_priv_list->index)
    {
      if (p_dir->all_ready == TRUE)
      {
        p_dir->cur_index = last_idx;
        p_list->p_file = p_priv_list->p_file;
        file_cnt = p_dir->cur_index * p_dir->unit_cnt;
        file_cnt = p_dir->ready_cnt - file_cnt;
        p_list->file_count = (file_cnt < p_dir->unit_cnt ? file_cnt : p_dir->unit_cnt);
        return ;
      }
      else
      {
        break;
      }
    }

    p_priv_list = p_priv_list->p_next;
  }

  while (p_dir->all_ready == FALSE)
  {
    p_priv_list->p_next
      = mtos_malloc(sizeof(priv_list_t) + p_dir->unit_cnt * sizeof(media_file_t));
    CHECK_FAIL_RET_VOID(p_priv_list->p_next != NULL);
    p_priv_list->p_next->p_file = (media_file_t *)(p_priv_list->p_next + sizeof(priv_list_t));

    file_cnt = get_new_file(p_dir, p_priv_list->p_next->p_file);
    if (file_cnt < p_dir->unit_cnt)
    {
      p_dir->all_ready = TRUE;
      if (file_cnt > 0)
      {
        p_priv_list = p_priv_list->p_next;
        p_priv_list->index = p_dir->cur_index + 1;
        p_priv_list->p_next = NULL;
        p_list->p_file = p_priv_list->p_file;
        p_list->file_count = file_cnt;
      }
      else
      {
        p_list->p_file = p_priv_list->p_file;
        p_list->file_count = p_dir->unit_cnt;
        mtos_free(p_priv_list->p_next);
        p_priv_list->p_next = NULL;
      }
      return ;
    }
    else
    {
      p_priv_list = p_priv_list->p_next;
      p_priv_list->index = p_dir->cur_index + 1;
      p_priv_list->p_next = NULL;
    }
  }

}

void file_list_init(void)
{
  file_list_priv_t *p_priv = NULL;

  p_priv = mtos_malloc(sizeof(file_list_priv_t));
  CHECK_FAIL_RET_VOID(p_priv != NULL);

  memset(p_priv, 0, sizeof(file_list_priv_t));
  p_priv->p_opened_dir = NULL;
  p_priv->max_support_cnt = 10 * FLIST_SAVE_CNT;
  class_register(FILE_LIST_CLASS_ID, p_priv);

}

void file_list_init2(u32 max_support_cnt)
{
  file_list_priv_t *p_priv = NULL;

  p_priv = mtos_malloc(sizeof(file_list_priv_t));
  CHECK_FAIL_RET_VOID(p_priv != NULL);

  memset(p_priv, 0, sizeof(file_list_priv_t));
  p_priv->p_opened_dir = NULL;
  if (max_support_cnt < FLIST_SAVE_CNT)
  {
    p_priv->max_support_cnt = FLIST_SAVE_CNT;
  }
  else
  {
    p_priv->max_support_cnt = max_support_cnt;
  }
  class_register(FILE_LIST_CLASS_ID, p_priv);
}

void file_list_deinit(void)
{

}

u32 file_list_get_partition(partition_t **pp_partition)
{
  file_list_priv_t *p_priv = NULL;
  partition_t *p_partition = NULL;
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;
  u8 loopi = 0;
  vfs_dev_info_t dev_info = {0};

  p_priv = class_get_handle_by_id(FILE_LIST_CLASS_ID);
  if (pp_partition)
  {
    *pp_partition = p_priv->partition;
  }

  partition_cnt = vfs_get_partitions(partition_letter, 10);

  for (loopi = 0; loopi < partition_cnt; loopi ++)
  {
    p_partition = &p_priv->partition[loopi];
    vfs_get_dev_info(partition_letter[loopi], &dev_info);
    uni_strcpy(p_partition->name, dev_info.parttion_name);
    p_partition->letter[0] = partition_letter[loopi];
    p_partition->letter[1] = 0x3a/*':'*/;
    p_partition->letter[2] = 0/*'\0'*/;
    p_partition->toatl_size = dev_info.total_size;
    p_partition->free_size = dev_info.free_size;
  }

  return partition_cnt;
}

flist_dir_t file_list_enter_dir(u16 *p_filter, u16 unit_cnt, u16 *p_path)
{
  file_list_priv_t *p_priv = NULL;
  file_list_dir_t *p_flist_dir = NULL;
  u16 *p_temp = NULL;
  u16 parent[4] = {0};
  u16 curn[4] = {0};

  CHECK_FAIL_RET_NULL(p_path != NULL);

  if (unit_cnt == 0)
  {
    return NULL;
  }

  p_priv = class_get_handle_by_id(FILE_LIST_CLASS_ID);

  if (p_priv->p_opened_dir)
  {
    OS_PRINTF("\n\n\n##waring:please close/leave dir[0x%x] first!\n", p_priv->p_opened_dir);
  }
  p_flist_dir = mtos_malloc(sizeof(file_list_dir_t));
  CHECK_FAIL_RET_NULL(p_flist_dir != NULL);
  memset(p_flist_dir, 0, sizeof(file_list_dir_t));

  uni_strcpy(p_flist_dir->cur_path, p_path);

  if (p_filter != NULL)
  {
    uni_strncpy(p_flist_dir->filter, p_filter, MAX_FILE_PATH);
  }

  p_flist_dir->unit_cnt = unit_cnt;

  p_temp = uni_strrchr(p_flist_dir->cur_path, 0x5c/*'\\'*/);

  if (p_temp != NULL)
  {
    //parent dir
    str_asc2uni("..", parent);
    str_asc2uni(".", curn);
    if (uni_strlen(p_temp) >= 3 && uni_strcmp(p_temp + 1, parent/*".."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
      p_temp = uni_strrchr(p_flist_dir->cur_path, 0x5c/*'\\'*/);
      if (p_temp != NULL)
      {
        p_temp[0] = 0/*'\0'*/;
      }
    }
    //cur dir
    else if (uni_strlen(p_temp) >= 2 && uni_strcmp(p_temp + 1, curn/*"."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
    }
  }

  p_flist_dir->cur_dir = vfs_opendir (p_flist_dir->cur_path, unit_cnt);
  if (p_flist_dir->cur_dir == NULL)
  {
    mtos_free(p_flist_dir);
    p_flist_dir = NULL;
  }
  p_priv->p_opened_dir = p_flist_dir;
  return (flist_dir_t)p_flist_dir;
}

BOOL file_list_get(flist_dir_t dir, flist_option_t option, file_list_t *p_list)
{
  file_list_dir_t *p_flist_dir = (file_list_dir_t *)dir;
  u32 loopi = 0;

  CHECK_FAIL_RET_FALSE(p_list != NULL);
  
  if (p_flist_dir == NULL)
  {
    p_list->file_count = 0;
    return FALSE;
  }

  switch (option)
  {
  case FLIST_UNIT_FIRST:
    get_first_unit(p_flist_dir, p_list);
    break;

  case FLIST_UNIT_NEXT:
    get_next_unit(p_flist_dir, p_list);
    break;

  case FLIST_UNIT_PREV:
    get_prev_unit(p_flist_dir, p_list);
    break;

  case FLIST_UNIT_LAST:
    get_last_unit(p_flist_dir, p_list);
    break;

  default:
    return FALSE;
  }

  for (loopi = 0; loopi < p_list->file_count; loopi ++)
  {
    OS_PRINTF("\n[%d][%s][%s][%d]", loopi + 1, p_list->p_file[loopi].name,
      p_list->p_file[loopi].p_name, p_list->p_file[loopi].size);
  }

  OS_PRINTF("\n");
  return TRUE;
}

void file_list_leave_dir(flist_dir_t dir)
{
  file_list_priv_t *p_priv = NULL;
  file_list_dir_t *p_dir = (file_list_dir_t *)dir;
  priv_list_t *p_priv_list = NULL;
  priv_list_t *p_cur_unit = NULL;

  CHECK_FAIL_RET_VOID(p_dir != NULL);
  p_priv = class_get_handle_by_id(FILE_LIST_CLASS_ID);

  if (p_priv->p_opened_dir == dir)
  {
    p_priv->p_opened_dir = NULL;
  }
  
  vfs_closedir(p_dir->cur_dir);

  p_priv_list = p_dir->p_priv_list;
  while (p_priv_list != NULL)
  {
    p_cur_unit = p_priv_list;
    p_priv_list = p_priv_list->p_next;
    mtos_free(p_cur_unit);
    p_cur_unit = NULL;
  }
  mtos_free(p_dir);
  p_dir = NULL;
}

static u32 flist_read_dir_one(flist_dir_priv_t *p_flist_dir, media_file_t *p_file)
{
  RET_CODE ret = 0;
  vfs_dir_info_t file_info;
  u32 file_cnt = 0;
  u32 fname_len = 0, fpath_len = 0;
  u16 uni_str[8] = {0};

  while (file_cnt < 1)
  {
    ret = vfs_readdir (p_flist_dir->vfs_dir, &file_info);

    if (ret != SUCCESS)
    {
      OS_PRINTF("\n##debug: ufs_readdir end!\n");
      break;
    }

    fpath_len = uni_strlen(p_flist_dir->cur_path);
    fname_len = uni_strlen(file_info.fname);

    if (fpath_len + fname_len + 1 <  MAX_FILE_PATH)
    {
      if (p_file)
      {
        memset(p_file->name, 0, MAX_FILE_PATH * sizeof(u16));
        uni_strcpy(p_file->name, p_flist_dir->cur_path);

        if (p_file->name[uni_strlen(p_file->name) - 1] != 0x5c/*'\\'*/)
        {
          uni_str[0] = 0x5c;
          uni_str[1] = 0;
          uni_strcat(p_file->name, uni_str, MAX_FILE_PATH);
        }
      }
      
      if ((file_info.fattrib & VFS_DIR) && !is_filter_nodir(p_flist_dir->filter))
      {
        uni_str[0] = 0x2e;
        uni_str[1] = 0;
        if (uni_strcmp(file_info.fname, uni_str))
        {
          if (p_file)
          {
            uni_strcat(p_file->name, file_info.fname, MAX_FILE_PATH);
            p_file->p_name = uni_strrchr(p_file->name, 0x5c/*'\\'*/) + 1;

            p_file->type = DIRECTORY;
            p_file->size = file_info.fsize;
          }
          file_cnt ++;
          break;
        }
      }
      else if ((p_flist_dir->filter[0] == 0 || is_filter_file(p_flist_dir->filter, file_info.fname)))
      {
        if (p_file)
        {
          uni_strcat(p_file->name, file_info.fname, MAX_FILE_PATH);
          p_file->p_name = uni_strrchr(p_file->name, 0x5c/*'\\'*/) + 1;
          p_file->type = NOT_DIR_FILE;
          p_file->size = file_info.fsize;
        }
        file_cnt ++;
        break;
      }
    }
  }

  return file_cnt;
}

static media_file_t *get_file_by_index(flist_dir_priv_t *p_dir_priv, u32 index)
{
  flist_unit_t *p_flist = NULL;
  u32 loopi = 0;
  
  CHECK_FAIL_RET_NULL(p_dir_priv != NULL);
  
  if (p_dir_priv->p_flist == NULL)
  {
    p_dir_priv->p_flist = mtos_malloc(sizeof(flist_unit_t));
    if (p_dir_priv->p_flist == NULL)
    {
      return NULL;
    }
    memset(p_dir_priv->p_flist, 0, sizeof(flist_unit_t));
    p_dir_priv->p_flist->p_next = NULL;
    OS_PRINTF("\n##get_file_by_index [%lu, 0x%x]\n", index, p_dir_priv->p_flist);
  }
  p_flist = p_dir_priv->p_flist;

  while ((loopi < (index / FLIST_SAVE_CNT)))
  {
    if (p_flist->p_next == NULL)
    {
      p_flist->p_next = mtos_malloc(sizeof(flist_unit_t));
      if (p_flist->p_next == NULL)
      {
        OS_PRINTF("\n##get_file_by_index malloc fail[%lu]\n", index);
        return NULL;
      }
      OS_PRINTF("\n##get_file_by_index [%lu, 0x%x]\n", index, p_flist->p_next);
      memset(p_flist->p_next, 0, sizeof(flist_unit_t));
      p_flist->p_next->p_next = NULL;
    }
    p_flist = p_flist->p_next;
    loopi ++;
  }

  return (media_file_t *)(&p_flist->file[index % FLIST_SAVE_CNT]);
}

static u32 flist_reload_dir(flist_dir_priv_t *p_flist_dir)
{
  media_file_t *p_file = NULL;
  file_list_priv_t *p_priv = NULL;

  p_priv = class_get_handle_by_id(FILE_LIST_CLASS_ID);
  
  if (p_flist_dir->vfs_dir)
  {
    vfs_closedir(p_flist_dir->vfs_dir);
    p_flist_dir->vfs_dir = NULL;
  }
  
  p_flist_dir->vfs_dir = vfs_opendir (p_flist_dir->cur_path, p_priv->max_support_cnt);
  p_flist_dir->total_cnt = 0;
  if (p_flist_dir->vfs_dir == NULL)
  {
    return 0;
  }
  
  while (p_flist_dir->total_cnt < p_priv->max_support_cnt)
  {
    p_file = get_file_by_index(p_flist_dir, p_flist_dir->total_cnt);
    if (p_file == NULL)
    {
      break;
    }
    if (flist_read_dir_one(p_flist_dir, p_file))
    {
      p_file->default_order = p_flist_dir->total_cnt;
      p_flist_dir->total_cnt ++;
    }
    else
    {
      break;
    }
  }

  return p_flist_dir->total_cnt;
}

flist_dir_t flist_open_dir(u16 *p_filter, u16 *p_path, u32 *p_total_cnt)
{
  file_list_priv_t *p_priv = NULL;
  flist_dir_priv_t *p_dir_priv = NULL;
  u16 *p_temp = NULL;
  u16 parent[4] = {0};
  u16 curn[4] = {0};

  CHECK_FAIL_RET_NULL(p_path != NULL);

  p_priv = class_get_handle_by_id(FILE_LIST_CLASS_ID);
  
  if (p_priv->p_opened_dir)
  {
    OS_PRINTF("\n\n\n##waring:please close/leave dir[0x%x] first!\n", p_priv->p_opened_dir);
  }
  p_dir_priv = mtos_malloc(sizeof(flist_dir_priv_t));
  CHECK_FAIL_RET_NULL(p_dir_priv != NULL);
  memset(p_dir_priv, 0, sizeof(flist_dir_priv_t));

  p_dir_priv->p_flist = NULL;
  p_dir_priv->start_idx = 0;
  p_dir_priv->total_cnt = 0;
  
  uni_strcpy(p_dir_priv->cur_path, p_path);

  if (p_filter != NULL)
  {
    uni_strncpy(p_dir_priv->filter, p_filter, MAX_FILE_PATH);
  }

  p_temp = uni_strrchr(p_dir_priv->cur_path, 0x5c/*'\\'*/);

  if (p_temp != NULL)
  {
    //parent dir
    str_asc2uni("..", parent);
    str_asc2uni(".", curn);
    if (uni_strlen(p_temp) >= 3 && uni_strcmp(p_temp + 1, parent/*".."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
      p_temp = uni_strrchr(p_dir_priv->cur_path, 0x5c/*'\\'*/);
      if (p_temp != NULL)
      {
        p_temp[0] = 0/*'\0'*/;
      }
    }
    //cur dir
    else if (uni_strlen(p_temp) >= 2 && uni_strcmp(p_temp + 1, curn/*"."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
    }
  }

  p_dir_priv->vfs_dir = vfs_opendir (p_dir_priv->cur_path, p_priv->max_support_cnt);
  if (p_dir_priv->vfs_dir == NULL)
  {
    mtos_free(p_dir_priv);
    p_dir_priv = NULL;
    return NULL;
  }
  OS_PRINTF("\n##flist read dir please wait...\n");

  flist_reload_dir(p_dir_priv);
  if (p_total_cnt)
  {
    *p_total_cnt = p_dir_priv->total_cnt;
  }
  p_priv->p_opened_dir = p_dir_priv;
  return (flist_dir_t)p_dir_priv;
}

flist_t * flist_get_by_index(flist_dir_t dir, u32 index)
{
  flist_dir_priv_t *p_dir_priv = (flist_dir_priv_t *)dir;
  flist_unit_t *p_flist = NULL;
  u32 loopi = 0;
  
  CHECK_FAIL_RET_NULL(p_dir_priv != NULL);
  
  if ((index >= p_dir_priv->total_cnt)
    || (p_dir_priv->p_flist == NULL))
  {
    return NULL;
  }

  p_flist = p_dir_priv->p_flist;

  while (p_flist && (loopi < (index / FLIST_SAVE_CNT)))
  {
    p_flist = p_flist->p_next;
    loopi ++;
  }

  return (flist_t *)(&p_flist->file[index % FLIST_SAVE_CNT]);
}

BOOL flist_del_by_index(flist_dir_t dir, u32 index)
{
  flist_dir_priv_t *p_dir_priv = (flist_dir_priv_t *)dir;
  media_file_t *p_file = NULL;

  CHECK_FAIL_RET_FALSE(p_dir_priv != NULL);
  
  if (index >= p_dir_priv->total_cnt)
  {
    return FALSE;
  }

  p_file = (media_file_t *)flist_get_by_index(dir, index);
  
  file_list_delete_file((u16 *)p_file->name);
  p_dir_priv->total_cnt --;

  if (p_dir_priv->total_cnt)
  {
    //reload
    flist_reload_dir(p_dir_priv);
  }
  
  return TRUE;
}

u16 * flist_get_name(flist_t *p_list)
{
  media_file_t *p_file = (media_file_t *)p_list;

  CHECK_FAIL_RET_NULL(p_file != NULL);

  return p_file->name;
}

u16 * flist_get_short_name(flist_t *p_list)
{
  media_file_t *p_file = (media_file_t *)p_list;

  CHECK_FAIL_RET_NULL(p_file != NULL);

  return p_file->p_name;
}

file_type_t flist_get_type(flist_t *p_list)
{
  media_file_t *p_file = (media_file_t *)p_list;

  CHECK_FAIL_RET((p_file != NULL), UNKNOW_FILE);

  return p_file->type;
}

u32 flist_get_size(flist_t *p_list)
{
  media_file_t *p_file = (media_file_t *)p_list;

  CHECK_FAIL_RET_ZERO(p_file != NULL);

  return p_file->size;
}

void file_list_close_dir(flist_dir_t dir)
{
  file_list_priv_t *p_priv = NULL;
  flist_dir_priv_t *p_dir_priv = (flist_dir_priv_t *)dir;
  flist_unit_t *p_flist = NULL;
  flist_unit_t *p_temp = NULL;

  CHECK_FAIL_RET_VOID(p_dir_priv != NULL);
  p_priv = class_get_handle_by_id(FILE_LIST_CLASS_ID);

  if (p_priv->p_opened_dir == dir)
  {
    p_priv->p_opened_dir = NULL;
  }
  vfs_closedir(p_dir_priv->vfs_dir);

  p_flist = p_dir_priv->p_flist;
  while (p_flist)
  {
    p_temp = p_flist;
    p_flist = p_flist->p_next;
    mtos_free(p_temp);
    p_temp = NULL;
  }
  mtos_free(p_dir_priv);
  p_dir_priv = NULL;
}

u16 *file_list_get_cur_path(flist_dir_t dir)
{
  file_list_dir_t *p_dir = (file_list_dir_t *)dir;

  CHECK_FAIL_RET_NULL(p_dir != NULL);

  return p_dir->cur_path;
}

#define IN_RANGE(c, lo, up)  ((u16)c >= lo && (u16)c <= up)
#define ISLOWER(c)           IN_RANGE(c, 0x0061/*'a'*/, 0x007A/*'z'*/)
#define IS_CHINESE(c)        (c >= 0x4E00 && c <= 0x9FA5)

static char __get_first_py_letter(u16 n)
{
  if (IN_RANGE(n,0xB0A1,0xB0C4))
  {
    return 'A';
  }
  if (IN_RANGE(n,0xB0C5,0xB2C0))
  {
    return 'B';
  }
  if (IN_RANGE(n,0xB2C1,0xB4ED))
  {
    return 'C';
  }
  if (IN_RANGE(n,0xB4EE,0xB6E9))
  {
    return 'D';
  }
  if (IN_RANGE(n,0xB6EA,0xB7A1))
  {
    return 'E';
  }
  if (IN_RANGE(n,0xB7A2,0xB8c0))
  {
    return 'F';
  }
  if (IN_RANGE(n,0xB8C1,0xB9FD))
  {
    return 'G';
  }
  if (IN_RANGE(n,0xB9FE,0xBBF6))
  {
    return 'H';
  }
  if (IN_RANGE(n,0xBBF7,0xBFA5))
  {
    return 'J';
  }
  if (IN_RANGE(n,0xBFA6,0xC0AB))
  {
    return 'K';
  }
  if (IN_RANGE(n,0xC0AC,0xC2E7))
  {
    return 'L';
  }
  if (IN_RANGE(n,0xC2E8,0xC4C2))
  {
    return 'M';
  }
  if (IN_RANGE(n,0xC4C3,0xC5B5))
  {
    return 'N';
  }
  if (IN_RANGE(n,0xC5B6,0xC5BD))
  {
    return 'O';
  }
  if (IN_RANGE(n,0xC5BE,0xC6D9))
  {
    return 'P';
  }
  if (IN_RANGE(n,0xC6DA,0xC8BA))
  {
    return 'Q';
  }
  if (IN_RANGE(n,0xC8BB,0xC8F5))
  {
    return 'R';
  }
  if (IN_RANGE(n,0xC8F6,0xCBF0))
  {
    return 'S';
  }
  if (IN_RANGE(n,0xCBFA,0xCDD9))
  {
    return 'T';
  }
  if (IN_RANGE(n,0xCDDA,0xCEF3))
  {
    return 'W';
  }
  if (IN_RANGE(n,0xCEF4,0xD1B8))
  {
    return 'X';
  }
  if (IN_RANGE(n,0xD1B9,0xD4D0))
  {
    return 'Y';
  }
  if (IN_RANGE(n,0xD4D1,0xD7F9))
  {
    return 'Z';
  }
  
  return '\0';
}

static inline u16 __toupper(u16 c)
{
  if (ISLOWER(c))
  {
    c -= 0x0061/*'a'*/ - 0x0041/*'A'*/;
  }
  else if (IS_CHINESE(c))
  {
    iconv_t hiconv = iconv_open("euccn", "ucs2le");
    if(hiconv != (iconv_t)-1)
    {
      char inbuf[2];
      char outbuf[2];
      char *p_inbuf = inbuf;
      char *p_outbuf = outbuf;
      size_t src_len = 2;
      size_t dest_len =  2;
      u16 gb2312_code = 0;

      inbuf[0] = (u8)c;
      inbuf[1] = (u8)(c >> 8);
      iconv(hiconv, &p_inbuf, &src_len, &p_outbuf, &dest_len);
      iconv_close(hiconv);

      gb2312_code = MAKE_WORD2(outbuf[0], outbuf[1]);
      c = __get_first_py_letter(gb2312_code);
    }
  }
  return c;
}


static s32 flist_strcmp(u16 *p_dst, u16 *p_src)
{
  s32 ret = 0;
  u16 src_up = 0;
  u16 dst_up = 0;
  
  CHECK_FAIL_RET_ZERO(p_dst != NULL && p_src != NULL);

  //sort except '..'
  if(((p_dst[0] == 0x2e/*'.'*/) && (p_dst[1] == 0x2e/*'.'*/))
     ||((p_src[0] == 0x2e/*'.'*/) && (p_src[1] == 0x2e/*'.'*/)))
  {
     return -1;
  }

  while(*p_dst != 0)
  {
    src_up = __toupper(*p_src);
    dst_up = __toupper(*p_dst);
    if((ret = src_up - dst_up))
    {
      break;
    }
    else if((ret = *p_src - *p_dst))
    {
      break;
    }
    
    ++p_src, ++p_dst;
  }

  if(ret > 0)
  {
    ret = -1;
  }
  else if(ret < 0)
  {
    ret = 1;
  }

  return ret;
}

static BOOL str_cmp_z_a(media_file_t *p_prev_node, media_file_t *p_cur_node)
{
  return (flist_strcmp(p_prev_node->p_name, p_cur_node->p_name) > 0);
}

static BOOL str_cmp_a_z(media_file_t *p_prev_node, media_file_t *p_cur_node)
{
  return (flist_strcmp(p_cur_node->p_name, p_prev_node->p_name) > 0);
}


static BOOL default_order_cmp(media_file_t *p_prev_node, media_file_t *p_cur_node)
{
  return p_prev_node->default_order < p_cur_node->default_order;
  //return FALSE;
}

void str_change(media_file_t *p_1, media_file_t *p_2)
{
  static void *p_void = NULL;
  media_file_t temp = {0};

  if(p_void == NULL)
  {
    p_void = mtos_malloc(MAX_FILE_PATH * sizeof(u16));
    memset(p_void, 0, MAX_FILE_PATH * sizeof(u16));
  }

  temp.p_name = (u16 *)p_void;

  temp.type = p_1->type;
  temp.size = p_1->size;
  temp.default_order = p_1->default_order;
  memset(temp.name, 0, uni_strlen(temp.name) * sizeof(u16));
  memcpy(temp.name, p_1->name, uni_strlen(p_1->name) * sizeof(u16));
  memset(temp.p_name, 0, uni_strlen(temp.p_name) * sizeof(u16));
  memcpy(temp.p_name, p_1->p_name, uni_strlen(p_1->p_name) * sizeof(u16));

  p_1->type = p_2->type;
  p_1->size = p_2->size;
  p_1->default_order = p_2->default_order;
  memset(p_1->name, 0, uni_strlen(p_1->name) * sizeof(u16));
  memcpy(p_1->name, p_2->name, uni_strlen(p_2->name) * sizeof(u16));
  memset(p_1->p_name, 0, uni_strlen(p_1->p_name) * sizeof(u16));
  memcpy(p_1->p_name, p_2->p_name, uni_strlen(p_2->p_name) * sizeof(u16));

  p_2->type = temp.type;
  p_2->size = temp.size;
  p_2->default_order = temp.default_order;
  memset(p_2->name, 0, uni_strlen(p_2->name) * sizeof(u16));
  memcpy(p_2->name, temp.name, uni_strlen(temp.name) * sizeof(u16));
  memset(p_2->p_name, 0, uni_strlen(p_2->p_name) * sizeof(u16));
  memcpy(p_2->p_name, temp.p_name, uni_strlen(temp.p_name) * sizeof(u16));
}

void file_list_sort(file_list_t *p_sort, int cnt, flist_sort_t sort_type)
{
  int i = 0;
  int j = 0;
  qsort_func sort_func = NULL;

  switch(sort_type)
  {
    case A_Z_MODE:
      sort_func = str_cmp_a_z;
      break;
      
    case Z_A_MODE:
      sort_func = str_cmp_z_a;
      break;
      
    case DEFAULT_ORDER_MODE:
      sort_func = default_order_cmp;
      break;
  }

  for(i = 0; i < cnt; i ++)
  {
    for(j = i + 1; j < cnt; j ++)
    {
      if(sort_func(&(p_sort->p_file[j]), &(p_sort->p_file[i])) > 0)
      {
        str_change(&(p_sort->p_file[i]), &(p_sort->p_file[j]));
      }
    }
  }
}

flist_type_t flist_get_file_type(u16 *p_filename)
{
  file_type_t ret = FILE_TYPE_UNKNOW;
  u16 *p_tip = NULL;
  u16 uni_mp3[6] = {0x6d, 0x70, 0x33}, uni_jpeg[6] = {0x6a, 0x70, 0x65, 0x67};
  u16 uni_jpg[6] = {0x6a, 0x70, 0x67}, uni_bmp[6] = {0x62, 0x6d, 0x70};
  u16 uni_ts[6] = {0x74, 0x73}, uni_gif[6] = {0x67, 0x69, 0x66};
  u16 uni_png[6] = {0x70, 0x6e, 0x67};

  if(p_filename != NULL)
  {
    p_tip = uni_strrchr(p_filename, 0x2e/*'.'*/);

    if(p_tip != NULL)
    {
      p_tip++;
      
      if(uni_strcmpi(p_tip, uni_mp3/*".MP3"*/) == 0)
      {
        ret = FILE_TYPE_MP3;
      }
      else if(uni_strcmpi(p_tip, uni_jpeg/*".JPEG"*/) == 0)
      {
        ret = FILE_TYPE_JPG;
      }
      else if(uni_strcmpi(p_tip, uni_jpg/*".JPG"*/) == 0)
      {
        ret = FILE_TYPE_JPG;
      }
      else if(uni_strcmpi(p_tip, uni_bmp/*".BMP"*/) == 0)
      {
        ret = FILE_TYPE_BMP;
      }
      else if(uni_strcmpi(p_tip, uni_ts/*".TS"*/) == 0)
      {
        ret = FILE_TYPE_TS;
      }
      else if(uni_strcmpi(p_tip, uni_gif/*".gif"*/) == 0)
      {
        ret = FILE_TYPE_GIF;
      }
      else if(uni_strcmpi(p_tip, uni_png/*".png"*/) == 0)
      {
        ret = FILE_TYPE_PNG;
      }      
    }
  }

  return ret;
}

//END OF FILE

