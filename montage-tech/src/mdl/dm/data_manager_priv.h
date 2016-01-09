/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_MANAGER_PRIV_H_
#define __DATA_MANAGER_PRIV_H_

/*!
  Data manager base class
  */
typedef struct
{
  /*!
    Private data
    */
  void *p_priv_data;
  /*!
    Write a node
    */
  dm_ret_t (*write_node)(void *p_priv_data, u8 block_id, u16 *p_node_id, 
    u8 *p_buffer, u16 len);
  /*!
    Del a node
    */
  dm_ret_t (*del_node)(void *p_priv_data, u8 block_id, u16 node_id, 
    BOOL b_sync);
  /*!
    Active node
    */
  dm_ret_t (*active_node)(void *p_priv_data, u8 block_id, u16 node_id);
  /*!
    check block
    */
  void (*check_block)(void *p_priv_data);
  /*!
    reset increase write block
    */
  dm_ret_t (*reset_iw_block) (void *p_priv_data, u8 block_id, BOOL is_sync);
  /*!
    get block address
    */
  u32 (*get_block_addr)(void *p_priv_data, u8 block_id);
  /*!
    get block size
    */
  u32 (*get_block_size)(void *p_priv_data, u8 block_id);
  /*!
    get block info
    */
  dm_ret_t (*get_block_info)(void *p_priv_data, u8 *block_num, u8 *id_list);
  /*!
    get block header
    */
  dm_ret_t (*get_block_header)(void *p_priv_data, u8 block_id, 
    dmh_block_info_t *p_head);
  /*!
    show flash map
    */
  void (*show_flash_map)(void *p_priv_data);
  /*!
    read bootloader block
    */
  s32 (*read_bl_block)(void *p_priv_data, u32 bid, u32 offset, u32 len, 
    u8 *p_buff);
  /*!
    set a block id
    */
  void (*set_fix_id)(void *p_priv_data, u8 block_id, u16 fix_id);
  /*!
    restore cache
    */
  dm_ret_t (*restore_cache)(void *p_priv_data, u8 block_id, u16 node_id);
  /*!
    flush cache
    */
  dm_ret_t (*flush_cache)(void *p_priv_data, u8 block_id, u16 node_id);
  /*!
    set cache
    */
  dm_ret_t (*set_cache)(void *p_priv_data, u8 block_id, u16 cache_size);
  /*!
    read node
    */
  u32 (*read_node)(void *p_priv_data, u8 block_id, u16 node_id, u16 offset, 
    u32 length, u8 *pbuffer);
  /*!
    get max node len
    */
  u16 (*get_max_node_len)(void *p_priv_data);
  /*!
    get number of block
    */
  u16 (*get_max_blk_num)(void *p_priv_data);
  /*!
    Set a datamanager header by a fixed flash address
    */
  void (*set_header)(void *p_priv_data, u32 addr);
  /*!
    Set a data manager header by a fixed flash address and block id
    */
  void (*set_header_ex)(void *p_priv_data, u32 addr, dm_set_block_t block_id);  
  /*!
    section restore, datamanager supported only!!!
    */
  void (*data_restore)(void *p_priv_data);
  /*!
    set decompress buf!!!
    */
  void (*set_decompress_buf)(void *p_priv_data, u8 *p_buf,
          u32 buf_size,u8 *p_cache, u32 cache_size);
  /*!
    piece data store, datamanager supported only!!!
    */
  BOOL (*store_piece_data)(void *p_priv_data, u8 *p_zip, u32 zip_size,
          u8 *p_cache,u32 cache_size);
  /*!
    check compress match flag
    */
  BOOL (*check_compress_match_flag)(void *p_priv_data);

  /*!
    set compress unmatch flag
    */
  void (*set_compress_unmatch_flag)(void *p_priv_data);

  /*!
    direct write data by block id.
    */
  u32 (*direct_read)(void *p_priv_data, u8 block_id, u32 offset, u32 len, u8 *p_buf);
  /*!
    direct write data by block id.
    */
  dm_ret_t (*direct_write)(void *p_priv_data, u8 block_id, u32 offset, u32 len, u8 *p_buf);

  /*!
    direct erase data by block id.
    */
  dm_ret_t (*direct_erase)(void *p_priv_data, u8 block_id, u32 offset, u32 len);

  /*!
    get all node id of block.
    */
  u32 (*get_node_id_list)(void *p_data, u8 block_id, u16 *p_node_id_list, u32 total_list);

  /*!
    get current block node num
    */
  u16 (*get_blk_node_num)(void * p_data, u8 block_id);
}dm_proc_t;

#endif //__DATA_MANAGER_PRIV_H_

