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
#include "lib_util.h"
#include "lib_unicode.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_mutex.h"
#include "mtos_sem.h"
#include "mtos_event.h"
#include "mtos_task.h"

#include "mt_time.h"
#include "vfs.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "ifilter.h"
#include "source_pin.h"
#include "ifilter.h"
#include "src_filter.h"
#include "chain.h"
//filter
#include "eva_filter_factory.h"
#include "file_source_filter.h"
#include "file_source_pin_intra.h"
#include "file_source_filter.h"
#include "err_check_def.h"


static fsrc_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((fsrc_pin_t *)_this)->private_data;
}

static void fsrc_pin_set_file_name(handle_t _this, u16 *p_file_name)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);

  CHECK_FAIL_RET_VOID(p_file_name != NULL);
  uni_strncpy(p_priv->file_name, p_file_name, MAX_FILE_PATH);
  p_priv->buf_get = NULL;
}

static void fsrc_pin_set_buffer_read(handle_t _this, void *p_read_data)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);

  p_priv->buf_get = (buffer_get) p_read_data;
  memset(p_priv->file_name, 0, MAX_FILE_PATH * sizeof(u16));
}

static void fsrc_pin_cfg(handle_t _this, void *p_attr)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);

  CHECK_FAIL_RET_VOID(p_attr != NULL);
  
  memcpy(&p_priv->attr, p_attr, sizeof(src_pin_attr_t));
}

static RET_CODE fsrc_pin_on_open(handle_t _this)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  vfs_file_info_t info = {0};

  p_priv->read_insufficient = 0;
  switch (p_priv->attr.src)
  {
  case SRC_FROM_FILE:
    if(p_priv->file_name[0] == 0)
    {
      p_priv->file = NULL;
      return SUCCESS;
    }
    
    if (p_priv->attr.is_share)
    {
      p_priv->file = vfs_open((u16 *)p_priv->file_name, VFS_SHARE);
    }
    else
    {
      p_priv->file = vfs_open((u16 *)p_priv->file_name, VFS_READ);
    }

    if(p_priv->file == NULL)
    {
      OS_PRINTF("open file failed\n");
      return ERR_FAILURE;
    }

    vfs_get_file_info(p_priv->file, &info);
    p_priv->file_size = info.file_size;

    vfs_ioctrl(p_priv->file, VFS_CMD_SEEK_LINKMAP, 0, 0, 0);
    break;
    
  case SRC_FROM_BLOCK:
    p_priv->file = vfs_fast_open(p_priv->file_name, VFS_READ, 0);
    if(p_priv->file == NULL)
    {
      OS_PRINTF("open file failed\n");
      return ERR_FAILURE;
    }    
    break;
    
  case SRC_FROM_BUFFER:
    if(p_priv->buf_get)
    {
      return SUCCESS;
    }
    break;
    
  default:
    break;
  }

  if (p_priv->file != NULL)
  {
    return SUCCESS;
  }
  else
  {
    return ERR_FAILURE;
  }
}

static RET_CODE fsrc_pin_on_close(handle_t _this)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  
  if (p_priv->buf_get)
  {
    return SUCCESS;
  }
  
  if(p_priv->file != NULL)
  {
    if (p_priv->attr.src == SRC_FROM_FILE)
    {
      vfs_close(p_priv->file);
    }
    else
    {
      vfs_fast_close(p_priv->file);
    }
    p_priv->file = NULL;
  }

  memset(p_priv->file_name, 0, MAX_FILE_PATH * sizeof(u16));
  
  return SUCCESS;
}

static RET_CODE _fill_sample_from_file(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, s64 position)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  u32 read_size = 0;
  u32 len = fill_len;
  s64 offset = 0;

  if (p_priv->file == NULL)
  {
    return ERR_FAILURE;
  }
  
  if (p_sample->context)  //context:0 no seek, 1~3 seek mode
  {
    vfs_seek(p_priv->file, position, p_sample->context - 1);
  }

  p_sample->p_user_data = (void *)((u32)vfs_tell(p_priv->file));

  offset = vfs_tell(p_priv->file);
  read_size = vfs_read(p_sample->p_data, len, 1, p_priv->file);
  
  p_sample->payload = read_size;
  
  if (read_size > 0)
  {
    if(offset + read_size == p_priv->file_size)
    {
      p_sample->state = SAMP_STATE_INSUFFICIENT;
    }
    else
    {
      p_sample->state = SAMP_STATE_GOOD;
    }

    return SUCCESS;
  }
  else if(p_priv->attr.is_circular)
  {
    p_sample->state = SAMP_STATE_FAIL;
    vfs_seek(p_priv->file, 0, VFS_SEEK_SET);
    return ERR_FAILURE;
  }
  else
  {
    ipin_t *p_ipin = (ipin_t *)_this;
    os_msg_t msg = {0};
    msg.content = FILE_READ_END;

    OS_PRINTF("\n\n file soure name:%s \n\n", p_priv->file_name);
    p_ipin->send_message_out(p_ipin, &msg);

    p_sample->state = SAMP_STATE_INSUFFICIENT;

    return SUCCESS;
  }

  //return SUCCESS;
}

static RET_CODE _fill_sample_from_buffer(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, s64 position)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  if(p_priv->buf_get)
  {
    p_sample->p_data = p_priv->buf_get(&p_sample->payload);

    p_sample->data_offset = p_sample->payload;
    p_sample->context = p_sample->payload;
    p_sample->state = SAMP_STATE_INSUFFICIENT;
    p_sample->p_user_data = NULL;
    p_priv->buf_get = NULL;

    return SUCCESS;
  }
  else
  {
    return ERR_FAILURE;
  }
}

static RET_CODE _fill_sample_from_block(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, s64 position)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  u32 read_size = 0;
  u32 len = fill_len;
  u32 read_pos = 0;

  if (p_priv->file)
  {
    if (p_sample->context)
    {
      if(vfs_fast_seek(p_priv->file, position, p_sample->context - 1) != SUCCESS)
      {
        p_sample->state = SAMP_STATE_INSUFFICIENT;
        p_sample->payload = 0;
        return SUCCESS;
      }
    }

    //waitting for about 500ms when read state indufficient
    if (p_priv->read_insufficient && (p_sample->context == 0))
    {
      if (p_priv->read_insufficient < 20)
      {
        p_priv->read_insufficient ++;
        mtos_task_sleep(20);
        p_sample->state = SAMP_STATE_INSUFFICIENT;
        p_sample->payload = 0;
        return SUCCESS;
      }
    }
    
    read_size = vfs_fast_read(p_priv->file, p_sample->p_data, len, &read_pos, NULL);
    p_sample->p_user_data = (void *)read_pos;

    p_sample->payload = read_size;
    if (p_sample->payload == len)
    {
      p_sample->state = SAMP_STATE_GOOD;
      if (p_priv->read_insufficient)
      {
        p_priv->read_insufficient = 0;
      }
      return SUCCESS;
    }
    else
    {
      p_sample->state = SAMP_STATE_INSUFFICIENT;
      if (p_priv->read_insufficient == 0)
      {
        //ipin_t *p_ipin = (ipin_t *)_this;
        //os_msg_t msg = {0};
        //msg.content = FSRC_MSG_BLOCK_READ_INSUFFICIENT;
        p_priv->read_insufficient = 1;
        //OS_PRINTF("\n##debug: FSRC_MSG_BLOCK_READ_INSUFFICIENT !!!\n");
        //p_ipin->send_message_out(p_ipin, &msg);
      }
      return SUCCESS;
    }
  }
  return ERR_FAILURE;
}

static RET_CODE fsrc_pin_on_fill_sample(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, s64 position)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  u32 len = (p_sample->total_buffer_size < fill_len) ? p_sample->total_buffer_size : fill_len;

  if (!len)
  {
    return ERR_FAILURE;
  }

  switch (p_priv->attr.src)
  {
    case SRC_FROM_FILE:
      return _fill_sample_from_file(_this, p_sample, len, position);

    case SRC_FROM_BUFFER:
      return _fill_sample_from_buffer(_this, p_sample, len, position);

    case SRC_FROM_BLOCK:
      return _fill_sample_from_block(_this, p_sample, len, position);

    default:
      break;
  }
  
  return ERR_FAILURE;
}

static RET_CODE fsrc_pin_decide_buffer_size(handle_t _this)
{
  fsrc_pin_private_t *p_priv = get_priv(_this);
  imem_allocator_t *p_alloc = p_priv->p_alloc;
  allocator_properties_t properties;
  
  //config mem_alloc
  p_alloc->get_properties(p_alloc, &properties);
  properties.buffers = p_priv->attr.buffers;
  properties.buffer_size = p_priv->attr.buffer_size;
  if(p_priv->attr.p_extern_buf != NULL)
  {
    properties.extant_buffer.p_buffer = p_priv->attr.p_extern_buf;
    properties.extant_buffer.buffer_len = p_priv->attr.extern_buf_size;    
  }
  properties.align_size = 8;
  p_alloc->set_properties(p_alloc, &properties, NULL);
  
  return SUCCESS;
}

fsrc_pin_t * fsrc_pin_create(fsrc_pin_t *p_fsrc_pin, interface_t *p_owner)
{
  fsrc_pin_private_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
  //interface_t *p_interface = NULL;
  source_pin_para_t src_pin_para = {0};
  media_format_t media_format = {MT_FILE_DATA};

  //check input parameter
  CHECK_FAIL_RET_NULL(p_fsrc_pin != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);
  p_ipin = (ipin_t *)p_fsrc_pin;

  //create base class
  src_pin_para.p_filter = p_owner;
  src_pin_para.stream_mode = STREAM_MODE_PULL;
  src_pin_para.p_name = "fsrc_pin";
  source_pin_create(&p_fsrc_pin->m_pin, &src_pin_para);

  //init private date
  p_priv = &(p_fsrc_pin->private_data);
  memset(p_priv, 0x0, sizeof(fsrc_pin_private_t));
  p_priv->p_this = p_fsrc_pin;
  
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);

  p_priv->attr.buffers = 1;
  p_priv->attr.buffer_size = KBYTES;
  
  //init member function
  p_fsrc_pin->set_file_name = fsrc_pin_set_file_name;
  p_fsrc_pin->set_buffer_read = fsrc_pin_set_buffer_read;
  p_fsrc_pin->config = fsrc_pin_cfg;

  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_fsrc_pin;
  p_output_pin->decide_buffer_size = fsrc_pin_decide_buffer_size;
  p_output_pin->on_fill_sample = fsrc_pin_on_fill_sample;
  
  p_ipin->on_open = fsrc_pin_on_open;
  p_ipin->on_close = fsrc_pin_on_close;

  return p_fsrc_pin;
}

//end of file

