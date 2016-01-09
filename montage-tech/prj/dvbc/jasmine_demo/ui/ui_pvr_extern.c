/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_pvr_extern.h"
#include "vfs.h"
#include "file_list.h"

#if ENABLE_MUSIC_PICTURE

typedef struct tag_ui_pvr_extern_list
{
  ui_pvr_extern_t pvr_extern;
  u32 file_offset;
  struct tag_ui_pvr_extern_list *p_next;
}ui_pvr_extern_list_t;

static hfile_t s_extern_file = 0;
static ui_pvr_extern_list_t *sp_pvr_extern = NULL;
static u8 *sp_extern_data = NULL;
static ui_pvr_extern_mode_t s_extern_mode = UI_PVR_EXTERN_READ;
static u16 s_extern_file_name[MAX_FILE_PATH + 1] = {0};

static RET_CODE _ui_pvr_extern_write_intra(ui_pvr_extern_t *p_pvr_extern)
{
  u8 *p_pvr_w = NULL;
  u32 head_offset = sizeof(ui_pvr_extern_t) - 4;
  vfs_file_info_t f_info = {0};
  ui_pvr_extern_list_t *p_next = NULL;
  ui_pvr_extern_list_t *p_prev = NULL;

  if (s_extern_mode == UI_PVR_EXTERN_APPEND_RW)
  {
    if (s_extern_file)
    {
      vfs_close(s_extern_file);
    }
    s_extern_file = vfs_open(s_extern_file_name, VFS_WRITE);
    if (s_extern_file == NULL)
    {
      return ERR_FAILURE;
    }
    vfs_get_file_info(s_extern_file, &f_info);
    vfs_seek(s_extern_file, (s64)f_info.file_size, VFS_SEEK_SET);

    p_next = mtos_malloc(sizeof(ui_pvr_extern_list_t));
    if (p_next != NULL)
    {
      p_next->p_next = NULL;
      p_next->file_offset = head_offset;
      memcpy((u8 *)&p_next->pvr_extern, (u8 *)p_pvr_extern, head_offset);
    }
    
    if (sp_pvr_extern == NULL && p_next != NULL)
    {
      p_next->file_offset = head_offset;
      sp_pvr_extern = p_next;
    }
    else
    {
      p_prev = sp_pvr_extern;
      while (p_prev->p_next)
      {
        p_prev = p_prev->p_next;
      }
      if(p_next)
      {
        p_next->file_offset = head_offset + p_prev->file_offset + p_prev->pvr_extern.extern_data_len;
        p_prev->p_next = p_next;
      }
    }
  }
  
  if ((s_extern_file == NULL)
    || (p_pvr_extern == NULL))
  {
    return ERR_FAILURE;
  }
  
  p_pvr_w = mtos_malloc(p_pvr_extern->extern_data_len + head_offset);
  if (p_pvr_w == NULL)
  {
    return ERR_FAILURE;
  }
  memset(p_pvr_w, 0, p_pvr_extern->extern_data_len + head_offset);
  memcpy(p_pvr_w, (u8 *)p_pvr_extern, head_offset);
  memcpy(p_pvr_w + head_offset, p_pvr_extern->p_extern_data, p_pvr_extern->extern_data_len);
  vfs_write(p_pvr_w, 1, p_pvr_extern->extern_data_len + head_offset, s_extern_file);
  if (s_extern_mode != UI_PVR_EXTERN_APPEND_RW)
  {
    vfs_flush(s_extern_file);
  }
  mtos_free(p_pvr_w);
  return SUCCESS;
}

static ui_pvr_extern_t *_ui_pvr_extern_read_intra(u32 rec_time, u8 index)
{
  ui_pvr_extern_list_t *p_next = NULL;
  BOOL is_find = TRUE;
  u8 i = 0;
  
  if (sp_pvr_extern == NULL)
  {
    return NULL;
  }
  
  p_next = sp_pvr_extern;
  while (p_next)
  {
    if (p_next->pvr_extern.rec_time == rec_time)
    {
      for(i = 0;i < index;i ++)
      {
        if((p_next == NULL) || (p_next->pvr_extern.rec_time != rec_time))
        {
          is_find = FALSE;
          break;
        }
        p_next = p_next->p_next;
      }

      if(is_find)
      {
        break;
      }
    }
    p_next = p_next->p_next;
  }
  
  if (p_next == NULL)
  {
    return NULL;
  }
  
  if (s_extern_mode == UI_PVR_EXTERN_APPEND_RW)
  {
    if (s_extern_file)
    {
      vfs_close(s_extern_file);
    }
    
    s_extern_file = vfs_open(s_extern_file_name, VFS_READ);
    if (s_extern_file == NULL)
    {
      return NULL;
    }
  }
  vfs_seek(s_extern_file, (s64)p_next->file_offset, VFS_SEEK_SET);

  if (sp_extern_data)
  {
    mtos_free(sp_extern_data);
  }
  
  sp_extern_data = mtos_malloc(p_next->pvr_extern.extern_data_len + 1);
  if (sp_extern_data == NULL)
  {
    return NULL;
  }
  memset(sp_extern_data, 0, p_next->pvr_extern.extern_data_len + 1);
  vfs_read(sp_extern_data, 1, p_next->pvr_extern.extern_data_len, s_extern_file);
  p_next->pvr_extern.p_extern_data = sp_extern_data;

  return &p_next->pvr_extern;
}

//lint -e716
RET_CODE ui_pvr_extern_open(u16 *p_rec_file, ui_pvr_extern_mode_t mode)
{
  u16 *p_str = NULL;
  ui_pvr_extern_list_t *p_prev = NULL;
  ui_pvr_extern_list_t *p_next = NULL;
  u32 head_offset = sizeof(ui_pvr_extern_t) - 4;
  u32 read_len = 0;
  vfs_file_info_t f_info = {0};
  MT_ASSERT(s_extern_file == NULL);
  
  s_extern_mode = mode;
  memset(s_extern_file_name, 0, sizeof(u16) * (MAX_FILE_PATH + 1));
  uni_strcpy(s_extern_file_name, p_rec_file);

  p_str = uni_strrchr(s_extern_file_name, 0x2e/*'.'*/);
  if (p_str)
  {
    p_str[1] = 0x65;/*e*/
    p_str[2] = 0x78;/*x*/
    p_str[3] = 0x74;/*t*/
  }
  else
  {
    return ERR_FAILURE;
  }
  
  if ((s_extern_mode == UI_PVR_EXTERN_WRITE)
    || (s_extern_mode == UI_PVR_EXTERN_APPEND_RW))
  {
    s_extern_file = vfs_open(s_extern_file_name, VFS_NEW | VFS_WRITE);
    if (s_extern_file == NULL)
    {
      return ERR_FAILURE;
    }
    return SUCCESS;
  }
  
  s_extern_file = vfs_open(s_extern_file_name, VFS_READ);
  
  if (s_extern_file == NULL)
  {
    return ERR_FAILURE;
  }

  vfs_get_file_info(s_extern_file, &f_info);
  if (f_info.file_size == 0)
  {
    vfs_close(s_extern_file);
    s_extern_file = NULL;
    vfs_del(s_extern_file_name);
  }
  
  sp_pvr_extern = mtos_malloc(sizeof(ui_pvr_extern_list_t));
  if (sp_pvr_extern == NULL)
  {
    return ERR_FAILURE;
  }
  memset(sp_pvr_extern, 0, sizeof(ui_pvr_extern_list_t));
  sp_pvr_extern->p_next = NULL;
  p_next = sp_pvr_extern;
  p_prev = NULL;
  
  while (1)
  {
    read_len = vfs_read(&p_next->pvr_extern, 1, head_offset, s_extern_file);
    if (read_len < head_offset)
    {
      if (p_prev)
      {
        p_prev->p_next = NULL;
        mtos_free(p_next);
      }
      break;
    }
    
    if (p_prev)
    {
      p_next->file_offset = head_offset + p_prev->file_offset + p_prev->pvr_extern.extern_data_len;
    }
    else
    {
      p_next->file_offset = head_offset;
    }
    vfs_seek(s_extern_file, (s64)p_next->pvr_extern.extern_data_len, VFS_SEEK_CUR);
    
    p_prev = p_next;
    p_next = mtos_malloc(sizeof(ui_pvr_extern_list_t));
    if (p_next == NULL)
    {
      break;
    }
    memset(p_next, 0, sizeof(ui_pvr_extern_list_t));
    p_next->p_next = NULL;
    p_prev->p_next = p_next;
  }
  return SUCCESS;
}
//lint +e716

RET_CODE ui_pvr_extern_write(ui_pvr_extern_t *p_pvr_extern)
{
  _ui_pvr_extern_write_intra(p_pvr_extern);

  return SUCCESS;
}

ui_pvr_extern_t *ui_pvr_extern_read(u32 rec_time, u8 index)
{
  ui_pvr_extern_t *p_pvr_extern = NULL;

  p_pvr_extern = _ui_pvr_extern_read_intra(rec_time, index);

  return p_pvr_extern;
}

u8 ui_pvr_extern_get_msg_num(u32 rec_time)
{
  ui_pvr_extern_list_t *p_next = NULL;
  u8 num = 0;
  
  if (sp_pvr_extern == NULL)
  {
    return 0;
  }
  
  p_next = sp_pvr_extern;
  while (p_next)
  {
    if (p_next->pvr_extern.rec_time == rec_time)
    {
      num ++;
    }
    p_next = p_next->p_next;
  }
  return num;
}

void ui_pvr_extern_close(void)
{
  ui_pvr_extern_list_t *p_pvr_extern = NULL;
  ui_pvr_extern_list_t *p_next = NULL;

  if (s_extern_file)
  {
    vfs_close(s_extern_file);
    s_extern_file = NULL;
  }

  p_pvr_extern = sp_pvr_extern;
  while (p_pvr_extern)
  {
    p_next = p_pvr_extern->p_next;
    mtos_free(p_pvr_extern);
    p_pvr_extern = p_next;
  }
  sp_pvr_extern = NULL;
  
  if (sp_extern_data)
  {
    mtos_free(sp_extern_data);
    sp_extern_data = NULL;
  }
}

void ui_pvr_extern_rename(u16 *p_rec_file, u16 *p_new_rec)
{
  u16 extern_old[MAX_FILE_PATH + 1] = {0};
  u16 extern_new[MAX_FILE_PATH + 1] = {0};
  u16 *p_str = NULL;
  
  uni_strcpy(extern_old, p_rec_file);
  uni_strcpy(extern_new, p_new_rec);

  p_str = uni_strrchr(extern_old, 0x2e/*'.'*/);
  if (p_str)
  {
    p_str[1] = 0x65;/*e*/
    p_str[2] = 0x78;/*x*/
    p_str[3] = 0x74;/*t*/
  }
  else
  {
    return ;
  }
  
  p_str = uni_strrchr(extern_new, 0x2e/*'.'*/);
  if (p_str)
  {
    p_str[1] = 0x65;/*e*/
    p_str[2] = 0x78;/*x*/
    p_str[3] = 0x74;/*t*/
  }
  else
  {
    return ;
  }
  file_list_rename(extern_old, extern_new);
}

#endif

//end of file
