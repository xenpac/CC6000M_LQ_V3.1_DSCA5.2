/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "drv_dev.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_misc.h"

#include "mtos_mutex.h"
#include "mtos_event.h"
#include "mt_time.h"
#include "vfs.h"
#include "drv_dev.h"
//#include "diskio.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"

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
#include "sink_pin.h"
#include "ifilter.h"
#include "sink_filter.h"
#include "chain.h"
#include "eva_filter_factory.h"
#include "file_sink_pin_intra.h"
#include "file_sink_filter.h"
#include "err_check_def.h"

/*!
  file sink offset
  */
#ifdef WIN32
#define FILE_SINK_OFFSET ((1) * (MBYTES))
#else
#define FILE_SINK_OFFSET ((50) * (MBYTES))
#endif

/*!
  ticks offset, about 1min
  */
#define FILE_SINK_TICKS_OFFSET (6000)

static fsink_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((fsink_pin_t *)_this)->private_data;
}

static RET_CODE on_open(handle_t _this)
{
  fsink_pin_private_t *p_priv = get_priv(_this);
  u32 block_size = 0;

  if (p_priv->mode == FSINK_SINK_TO_BLOCK)
  {
    block_size = (u32)p_priv->max_size;
    block_size = block_size - block_size % TS_STREAM_RW_UNIT;
    p_priv->file = vfs_fast_open(p_priv->file_name,  VFS_NEW, block_size);
  }
  else
  {
    if (p_priv->is_share)
    {
      OS_PRINTF("\n##debug: vfs_open VFS_SHARE!\n");
      p_priv->file = vfs_open(p_priv->file_name,  VFS_SHARE);
    }
    else
    {
      OS_PRINTF("\n##debug: file sink new [%s] usb free[%lld]K!\n",
        p_priv->file_name, p_priv->max_size >> 10);
      p_priv->file = vfs_open(p_priv->file_name,  VFS_NEW);
    }
  }
  
  p_priv->w_size = 0;
  p_priv->flush_ticks = mtos_ticks_get();
  p_priv->file_counter ++;
  if (p_priv->file != NULL)
  {
    return SUCCESS;
  }
  else
  {
    OS_PRINTF("\n##debug: file sink open [%s] fail!!!\n", p_priv->file_name);
    return ERR_FAILURE;
  }
}

static RET_CODE on_close(handle_t _this)
{
  fsink_pin_private_t *p_priv = get_priv(_this);
  ipin_t *p_ipin = (ipin_t *)p_priv->p_this;
  os_msg_t msg = {0};
  
  if (p_priv->file != NULL)
  {
    if (p_priv->mode == FSINK_SINK_TO_BLOCK)
    {
      vfs_fast_close(p_priv->file);
    }
    else
    {
      msg.content = FILE_SINK_MSG_SAVE_FILE;
      msg.para1 = p_priv->file_counter;
      p_ipin->send_message_out(p_ipin, &msg);
      vfs_close(p_priv->file);
    }
    p_priv->file = NULL;
  }
  return SUCCESS;
}

static void file_receive(fsink_pin_private_t *p_priv, media_sample_t *p_sample)
{
  u16 temp[MAX_FILE_PATH] = {0};
  u32 write_size = 0;
  u32 ridx_size = 0;
  ipin_t *p_ipin = (ipin_t *)p_priv->p_this;
  os_msg_t msg = {0};
  vfs_dev_info_t info = {0};
    
  if ((p_priv->w_size + p_sample->payload) >= MAX_RECORD_FILE_SIZE)
  {
    on_close(p_priv->p_this);
    file_sink_split_file_name(p_priv->file_name, p_priv->file_counter + 1);
    if (p_priv->p_media_idx)
    {
      memset(temp, 0, MAX_FILE_PATH * sizeof(u16));
      get_idx_info_file(p_priv->file_name, temp, MAX_FILE_PATH);
      ridx_size = media_idx_split_file(p_priv->p_media_idx, temp);
      OS_PRINTF("\n##file sink save file [%llu, %lu] reach 4G![0x%x]\n",
        p_priv->w_size, ridx_size, p_priv->p_media_idx->file);
      CHECK_FAIL_RET_VOID(p_priv->p_media_idx->file != NULL);
    }
    vfs_get_dev_info(p_priv->file_name[0], &info);
    p_priv->max_size = ((u64)info.free_size) << 10;
    on_open(p_priv->p_this);
    
    p_priv->w_size = 0;
    p_priv->flush_ticks = mtos_ticks_get();
    return ;
  }
  
  write_size = vfs_write(p_sample->p_data, p_sample->payload, 1, p_priv->file);
  
  p_priv->w_size += write_size;
  //p_priv->flush_size += write_size;

  //if (p_priv->flush_size >= 100 * MBYTES)
  if ((mtos_ticks_get() - p_priv->flush_ticks) >= FILE_SINK_TICKS_OFFSET)
  {
    vfs_flush(p_priv->file);
    p_priv->flush_ticks = mtos_ticks_get();
    media_idx_flush(p_priv->p_media_idx);
  }

  if (write_size < p_sample->payload)
  {
    OS_PRINTF("\n##write error [%d, %d]!\n", write_size, p_sample->payload);
    vfs_get_dev_info(p_priv->file_name[0], &info);
    if (info.free_size <= (FILE_SINK_OFFSET >> 10))
    {
      OS_PRINTF("\n##write error free_size [%lu]K\n", info.free_size);
      msg.content = FILE_SINK_MSG_NO_ENOUGH_MEM;
    }
    else
    {
      msg.content = FILE_SINK_MSG_WRITE_ERROR;
    }
    
    p_ipin->send_message_to_filter(p_priv->p_this, &msg);
    p_ipin->send_message_out(p_ipin, &msg);
  }
  
  if ((p_priv->w_size + FILE_SINK_OFFSET) >= p_priv->max_size)
  {
    OS_PRINTF("\n##no mem to write!!! \n");
    msg.content = FILE_SINK_MSG_NO_ENOUGH_MEM;
    msg.para1 = p_priv->max_size - p_priv->w_size;
    p_priv->w_size = 0;
    //notify parter
    p_ipin->send_message_to_filter(p_priv->p_this, &msg);
    p_ipin->send_message_out(p_ipin, &msg);
  }
}

static void block_receive(fsink_pin_private_t *p_priv, media_sample_t *p_sample)
{
  u32 write_pos = 0;
  vfs_fast_state_t state = VFS_FAST_WRITE_NARMAL;

  vfs_fast_write(p_priv->file, p_sample->p_data, p_sample->payload, &write_pos, &state);

  if((state & VFS_FAST_WRITE_BACK) == VFS_FAST_WRITE_BACK)
  {
    ipin_t *p_ipin = (ipin_t *)p_priv->p_this;
    os_msg_t msg = {0};

    msg.content = FILE_SINK_MSG_BLOCK_WRITE_BACK;
    msg.para1 = write_pos;
    p_ipin->send_message_out(p_ipin, &msg);
  }
  else if((state & VFS_FAST_WRITE_REACH_READ) == VFS_FAST_WRITE_REACH_READ)
  {
    ipin_t *p_ipin = (ipin_t *)p_priv->p_this;
    os_msg_t msg = {0};

    msg.content = FILE_SINK_MSG_BLOCK_WRITE_REACH_READ;
    msg.para1 = write_pos;
    p_ipin->send_message_out(p_ipin, &msg);
  }
  
  p_priv->w_size += p_sample->payload;
  
  if (p_priv->ready_size && p_priv->w_size >= p_priv->ready_size)
  {
    ipin_t *p_ipin = (ipin_t *)p_priv->p_this;
    os_msg_t msg = {0};

    p_priv->ready_size = 0;
    msg.content = FILE_SINK_MSG_READY;
    
    p_ipin->send_message_out(p_ipin, &msg);
  }
}

static void on_receive(handle_t _this, media_sample_t *p_sample)
{
  fsink_pin_private_t *p_priv = get_priv(_this);

  if (p_priv->file == NULL)
  {
    OS_PRINTF("\n##debug: fink file NULL!\n");
    return ;
  }

  if (p_priv->mode == FSINK_SINK_TO_FILE)
  {
    file_receive(p_priv, p_sample);
  }
  else
  {
    block_receive(p_priv, p_sample);
  }
}

static BOOL notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}

static void fsink_pin_set_file_name(handle_t _this, u16 *p_file_name, BOOL is_share)
{
  fsink_pin_private_t *p_priv = get_priv(_this);

  CHECK_FAIL_RET_VOID(p_file_name != NULL);
  uni_strncpy(p_priv->file_name, p_file_name, MAX_FILE_PATH);
  p_priv->is_share = is_share;
}

static void fsink_pin_set_sink_mode(handle_t _this, u8 sink_mode, media_idx_t *p_media_idx)
{
  fsink_pin_private_t *p_priv = get_priv(_this);

  CHECK_FAIL_RET_VOID(p_priv != NULL);
  p_priv->mode = sink_mode;
  p_priv->p_media_idx = p_media_idx;
}

static void fsink_pin_set_ready_size(handle_t _this, u32 ready_size, u64 max_size)
{
  fsink_pin_private_t *p_priv = get_priv(_this);

  CHECK_FAIL_RET_VOID(p_priv != NULL);
  p_priv->ready_size = ready_size;
  p_priv->max_size = max_size;
  CHECK_FAIL_RET_VOID(p_priv->max_size != 0);
}

static void fsink_pin_seek(handle_t _this, s64 offset, u32 mode)
{
  fsink_pin_private_t *p_priv = get_priv(_this);

  vfs_seek(p_priv->file,  offset, mode);
}


fsink_pin_t * fsink_pin_create(fsink_pin_t *p_pin, interface_t *p_owner)
{
  fsink_pin_private_t *p_priv = NULL;
  base_input_pin_t *p_input_pin = NULL;
  sink_pin_para_t sink_pin_para = {0};
  ipin_t *p_ipin = NULL;

  //check input parameter
  CHECK_FAIL_RET_NULL(p_pin != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);

  //create base class
  sink_pin_para.p_filter = p_owner;
  sink_pin_para.p_name = "fsink_pin";
  sink_pin_create(&p_pin->base_pin, &sink_pin_para);

  p_pin->set_file_name = fsink_pin_set_file_name;
  p_pin->set_sink_mode = fsink_pin_set_sink_mode;
  p_pin->set_sink_ready_size = fsink_pin_set_ready_size;
  p_pin->seek = fsink_pin_seek;
  //init private date
  p_priv = &p_pin->private_data;
  memset(p_priv, 0, sizeof(fsink_pin_private_t));
  p_priv->file_counter = 0;
  p_priv->p_this = p_pin; //this point

  //init member function

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->on_receive = on_receive;
  p_input_pin->notify_allocator = notify_allocator;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->on_open = on_open;
  p_ipin->on_close = on_close;

  return p_pin;
}

