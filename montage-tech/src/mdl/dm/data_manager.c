/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "data_manager.h"
#include "data_manager_priv.h"



dm_ret_t dm_write_node(void *p_handle, u8 block_id, u16 *p_node_id,
u8 *p_buffer, u16 len)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->write_node != NULL)
  {
    return p_this->write_node(p_priv_data, block_id, p_node_id, p_buffer, len);
  }

  return DM_FAIL;
}

dm_ret_t dm_del_node(void *p_handle, u8 block_id, u16 node_id, BOOL b_sync)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->del_node != NULL)
  {
    return p_this->del_node(p_priv_data, block_id, node_id, b_sync);
  }

  return DM_FAIL;
}

dm_ret_t dm_active_node(void *p_handle, u8 block_id, u16 node_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->active_node != NULL)
  {
    return p_this->active_node(p_priv_data, block_id, node_id);
  }

  return DM_FAIL;
}

void dm_check_block(void *p_handle)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->check_block != NULL)
  {
    p_this->check_block(p_priv_data);
  }
}

dm_ret_t dm_reset_iw_block(void *p_handle, u8 block_id, BOOL is_sync)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->reset_iw_block != NULL)
  {
    return p_this->reset_iw_block(p_priv_data, block_id, is_sync);
  }

  return DM_FAIL;
}

u32 dm_get_block_addr(void *p_handle, u8 block_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->get_block_addr != NULL)
  {
    return p_this->get_block_addr(p_priv_data, block_id);
  }

  return 0;
}

u32 dm_get_block_size(void *p_handle, u8 block_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->get_block_size != NULL)
  {
    return p_this->get_block_size(p_priv_data, block_id);
  }

  return 0;
}

dm_ret_t dm_get_block_info(void *p_handle, u8 *p_block_num, u8 *p_id_list)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->get_block_info != NULL)
  {
    return p_this->get_block_info(p_priv_data, p_block_num, p_id_list);
  }

  return DM_FAIL;
}

dm_ret_t dm_get_block_header(void *p_handle, u8 block_id,
  dmh_block_info_t *p_head)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->get_block_header != NULL)
  {
    return p_this->get_block_header(p_priv_data, block_id, p_head);
  }

  return DM_FAIL;
}

void dm_show_flash_map(void *p_handle)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->show_flash_map != NULL)
  {
    p_this->show_flash_map(p_priv_data);
  }
}

s32 dm_read_bl_block(void *p_handle, u32 bid, u32 offset, u32 len,
  u8 *p_buff)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->read_bl_block != NULL)
  {
    return p_this->read_bl_block(p_priv_data, bid, offset, len, p_buff);
  }

  return 0;
}

void dm_set_fix_id(void *p_handle, u8 block_id, u16 fix_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->set_fix_id != NULL)
  {
    p_this->set_fix_id(p_priv_data, block_id, fix_id);
  }
}

dm_ret_t dm_restore_cache(void *p_handle, u8 block_id, u16 node_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->restore_cache != NULL)
  {
    return p_this->restore_cache(p_priv_data, block_id, node_id);
  }

  return DM_FAIL;
}

dm_ret_t dm_flush_cache(void *p_handle, u8 block_id, u16 node_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->flush_cache != NULL)
  {
    return p_this->flush_cache(p_priv_data, block_id, node_id);
  }

  return DM_FAIL;
}

dm_ret_t dm_set_cache(void *p_handle, u8 block_id, u16 cache_size)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->set_cache != NULL)
  {
    return p_this->set_cache(p_priv_data, block_id, cache_size);
  }

  return DM_FAIL;
}

u16 dm_get_max_node_len(void *p_handle)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->get_max_node_len)
  {
    return p_this->get_max_node_len(p_priv_data);
  }

  return 0;
}

u32 dm_read(void *p_handle, u8 block_id, u16 node_id, u16 offset,
  u32 length, u8 *p_buffer)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->read_node != NULL)
  {
    return p_this->read_node(p_priv_data, block_id, node_id, offset, length,
          p_buffer);
  }

  return 0;
}

u16 dm_get_max_blk_num(void * p_handle)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->get_max_blk_num != NULL)
  {
    return p_this->get_max_blk_num(p_priv_data);
  }

  return 0;
}

void dm_set_header(void * p_handle, u32 addr)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  handle_t p_priv_data = p_this->p_priv_data;

  if(p_this->set_header != NULL)
  {
    p_this->set_header(p_priv_data, addr);
  }
}

void dm_set_header_ex(void *p_handle, u32 addr, dm_set_block_t block_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  handle_t *p_priv_data = p_this->p_priv_data;

  if(p_this->set_header_ex != NULL)
  {
    p_this->set_header_ex(p_priv_data, addr, block_id);
  }
}

void dm_data_restore(void * p_handle)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  handle_t p_priv_data = p_this->p_priv_data;

  if(p_this->data_restore != NULL)
  {
    p_this->data_restore(p_priv_data);
  }
}

void dm_set_decompress_buf(void *p_handle,u8 *p_buf, u32 buf_size,u8 *p_cache, u32 cache_size)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  handle_t p_priv_data = p_this->p_priv_data;

  if(p_this->set_decompress_buf != NULL)
  {
    p_this->set_decompress_buf(p_priv_data,p_buf,buf_size,p_cache,cache_size);
  }
}

void dm_store_piece_data(void * p_handle,u8 *p_zip, u32 zip_size,
          u8 *p_cache,u32 cache_size)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  handle_t p_priv_data = p_this->p_priv_data;

  if(p_this->store_piece_data != NULL)
  {
    p_this->store_piece_data(p_priv_data, p_zip, zip_size, p_cache, cache_size);
  }
}

BOOL dm_check_compress_match_flag(void * p_handle)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  handle_t p_priv_data = p_this->p_priv_data;
  if(p_this->check_compress_match_flag != NULL)
  {
    return p_this->check_compress_match_flag(p_priv_data);
  }

  return FALSE;
}

void dm_set_compress_unmatch_flag(void * p_handle)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  handle_t p_priv_data = p_this->p_priv_data;

  if(p_this->set_compress_unmatch_flag != NULL)
  {
    p_this->set_compress_unmatch_flag(p_priv_data);
  }
}


u32 dm_direct_read(void *p_handle, u8 block_id, u32 offset, u32 len, u8 *p_buf)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->direct_read != NULL)
  {
    return p_this->direct_read(p_priv_data, block_id, offset, len, p_buf);
  }

  return 0;
}

dm_ret_t dm_direct_write(void *p_handle, u8 block_id, u32 offset, u32 len, u8 *p_buf)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data ;

  if(p_this->direct_write != NULL)
  {
    return p_this->direct_write(p_priv_data, block_id, offset, len, p_buf);
  }

  return DM_FAIL;
}


dm_ret_t dm_direct_erase(void *p_handle, u8 block_id, u32 offset, u32 len)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->direct_erase != NULL)
  {
    return p_this->direct_erase(p_priv_data, block_id, offset, len);
  }

  return DM_FAIL;
}

u32 dm_get_node_id_list(void *p_handle, u8 block_id, u16 *p_node_id_list, u32 total_list)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->get_node_id_list != NULL)
  {
    return p_this->get_node_id_list(p_priv_data, block_id, p_node_id_list, total_list);
  }

  return 0;
}

u16 dm_get_blk_node_num(void * p_handle, u8 block_id)
{
  dm_proc_t *p_this = (dm_proc_t *)p_handle;
  void *p_priv_data = p_this->p_priv_data;

  if(p_this->get_blk_node_num != NULL)
  {
    return p_this->get_blk_node_num(p_priv_data, block_id);
  }
  
  return 0;
}

