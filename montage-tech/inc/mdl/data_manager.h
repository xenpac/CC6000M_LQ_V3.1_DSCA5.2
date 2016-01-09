/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_MANAGER_H_
#define __DATA_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif
/*!
  \file: data_manager.h
  This file provides the interface for all operation to 
  data manager
  */
/*!
  MAX length of each node
  Note!!!! the define using in data_manager32 and data_manager_v2 only
  if you use data_manager_v3, must define the Max in your sys_cfg.(see jazz prj)
  */
#define MAX_NODE_LEN  (1024)//(256)

/*!
  data manager version
  */
typedef enum tag_dm_version
{
  /*!
    data manager version unknown
    */
  DM_VER_UNK,
  /*!
    data manager version 1
    */
  DM_VER_1,
  /*!
    data manager version 2
    */
  DM_VER_2,
  /*!
    data manager version 3
    */
  DM_VER_3,
}dm_version_t;

/*!
  Flash data block type
  */
typedef enum tag_block_type
{
 /*!
  Read only block
  */
  BLOCK_TYPE_RO = 0x00,   
 /*!
  Read and increment writing block
  */   
  BLOCK_TYPE_IW = 0x01,
 /*!
  Read and writing block whole
  */   
  BLOCK_TYPE_PIECE = 0x02,
 /*!
  Direct read and writing super block 
  */
  BLOCK_TYPE_DIRECT_RW = 0x03,
 /*!
  Block for STB identity info
  */
  BLOCK_TYPE_IDENTITY = 0x04,
 /*!
  Block for fixed address
  */ 
  BLOCK_TYPE_FIXED_ADDR = 0x5,
 /*!
  Block type unknown
  */
  BLOCK_TYPE_UNKNOWN
}block_type_t;

/*!
 Block information of data manager header
 */
typedef struct tag_dmh_block_info
{
  /*!
    Block id
    */
  u8 id;
  /*!
    Block type refer to enum tag_block_type
    */
  u8 type; 
  /*!
    Node number in current block
    */
  u16 node_num;
  /*!
    Base address of current block
    */
  u32 base_addr;
  /*!
    Block size
    */
  u32 size;
  /*!
    CRC byte
    */
  u32 crc;
  /*!
    Version info of this block
    */
  u8 version[8];
  /*!
    Name of this block
    */
  u8 name[8];
  /*!
    Time of the last version of this block
    */
  u8 time[12];
  /*!
    Reserve
    */
  u16 ota_ver;
  /*!
    Reserve
    */
  u16 reserve;
}dmh_block_info_t;

/*!
  Data manager return status
  */
typedef enum tag_dm_ret
{
  /*!
    Data manager failed to get certain data
    */
  DM_FAIL = 0,
  /*!
    Data manager is full when first write to data manager
    */
  DM_FULL,
  /*!
    Data manager process sucessfully
    */
  DM_SUC
}dm_ret_t;

/*!
  Data manager type
  */
typedef enum tag_dm_type
{
  /*!
    DM32 supported
    */
  DM_32 = 0,
  /*!
    DM16 supported 
    */
  DM_16
}dm_type_t;


/*!
  warning msg routine
  */
#define DM_BLOCK_SIZE (sizeof(dmh_block_info_t))
/*!
  warning msg routine
  */
#define DM_GET_U8(x) (*((volatile u8 *)(x + 0x8000000)))
/*!
  warning msg routine
  */
#define DM_GET_U16(x) (*((volatile u16 *)(x + 0x8000000)))
/*!
  warning msg routine
  */
#define DM_GET_U32(x) (*((volatile u32 *)(x + 0x8000000)))
/*!
  warning msg routine
  */
#define DM_HEADER_SHIFT (16 + 12)

/*!
  DM_GET_BLOCK_ID
  */
#define DM_GET_BLOCK_ID(block_id, h_addr, i)                                          \
do{                                                                           \
    (block_id) =  (*((volatile u32 *) \
    ((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 0x8000000)));  \
}while(0)

/*!
  DM_GET_ADDR
  */
#define DM_GET_ADDR(addr, h_addr, i)                                          \
do{                                                                           \
    (addr) =  (*((volatile u32 *) \
    ((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 4 + 0x8000000)));  \
    (addr) += (h_addr + 0x8000000);                                                       \
}while(0)

/*!
  DM_GET_SIZE
  */
#define DM_GET_SIZE(size, h_addr, i)                                          \
do{                                                                           \
    (size) = (*((volatile u32 *)((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 8 + 0x8000000)));\
}while(0)

/*!
  DM_GET_CRC
  */
#define DM_GET_CRC(crc, h_addr, i)                                            \
do{                                                                           \
    (crc) = (*((volatile u32 *)((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 12 + 0x8000000)));\
}while(0)

/*!
  DM_GET_BLOCK_NUM
  */
#define DM_GET_BLOCK_NUM(num, h_addr)                     \
do{                                                       \
  (num) = (*((volatile u16 *)((h_addr) + 16 + 8 + 0x8000000)));                  \
}while(0)

/*!
  DM_GET_FIRST_BLOCK_HEAD_ADDR
  */
#define DM_GET_FIRST_BLOCK_HEAD_ADDR(num, h_addr)                     \
do{                                                       \
  (num) = ((h_addr) + 16 + 8 + 4 + 0x8000000);                  \
}while(0)

/*!
  defines set header block id.
  */
typedef enum tag_dm_set_block
{
 /*!
  set table block
  */
  DM_SET_TABLE_BLOCK = 0xA4,
 /*!
  set view block
  */
  DM_SET_VIEW_BLOCK = 0xA8,
}dm_set_block_t;

/*!
  Write data into flash and return node id
  \param[in] p_handle data manager handle  
  \param[in] block_id block id
  \param[out] p_node_id node id storing the written data
  \param[in] p_buffer data buffer with data to be written into flash
  \param[in] len data lenght of data to be written
  */
dm_ret_t dm_write_node(void *p_handle, u8 block_id, u16 *p_node_id, 
u8 *p_buffer, u16 len);
/*!
  Delete node from flash 
  \param[in] p_handle data manager handle  
  \param[in] block_id block id contain the node
  \param[in] node_id node id to be deleted
  \param[in] b_sync TRUE->write data to flash at once
  */
dm_ret_t dm_del_node(void *p_handle, u8 block_id, u16 node_id, 
BOOL b_sync);
/*!
  Activate datamanager node
  \param[in] p_handle data manager handle
  \param[in] block_id block id contain the node
  \param[in] node_id  node id to be activated
  */
dm_ret_t dm_active_node(void *p_handle, u8 block_id, u16 node_id);
/*!
  Check block status
  \param[in] p_handle data manager handle
  */
void dm_check_block(void *p_handle);
/*!
  Reset the iw flash block
  return data manager status
  \param[in] p_handle data manager handle
  \param[in] block_id block id
  \param[in] is_sync sync flash.
  */
dm_ret_t dm_reset_iw_block(void *p_handle, u8 block_id, BOOL is_sync);

/*!
  Get the block address of given block id
  return block address
  \param[in] p_handle data manager handler
  \param[in] block_id block id
  */
u32 dm_get_block_addr(void *p_handle, u8 block_id);
/*!
  Get the block size of the given block id
  \param[in] p_handle data manager handler 
  \param[in] block_id block id
  */
u32 dm_get_block_size(void *p_handle, u8 block_id);
/*!
  Get the block information 
  \param[in] p_handle data manager handle
  \param[out] p_block_num total block number in flash
  \param[out] p_id_list of all the block in flash
  */
dm_ret_t dm_get_block_info(void *p_handle, u8 *p_block_num, u8 *p_id_list);
/*!
  Get block header of given block id and return data manager status
  \param[in] p_handle data manager handler
  \param[in] block_id block id
  \param[out] p_head header of this block
  */
dm_ret_t dm_get_block_header(void *p_handle, u8 block_id, 
dmh_block_info_t *p_head);

/*!
  Show the map of flash block
  \param[in] p_handle data manager handle
  */
void dm_show_flash_map(void *p_handle);
/*!
  Read information of boot loader block in flash and return data length of bl 
  block
  \param[in] p_handle data manager handle
  \param[in] bid block id
  \param[in] offset read starting position in the block
  \param[in] len data length to be read from flash
  \param[in] p_buff buffer address for read boot loader block
  */
s32 dm_read_bl_block(void *p_handle, u32 bid, u32 offset,
u32 len, u8 *p_buff);
/*!
  Set fix block id in certain flash block
  \param[in] p_handle data manager handler
  \param[in] block_id block id 
  \param[in] fix_id to be set into data manager
  */
void dm_set_fix_id(void *p_handle, u8 block_id, u16 fix_id);
/*!
  Restore data cache to certain block id and node id and return data manager 
  status
  \param[in] p_handle data manager handle
  \param[in] block_id block id
  \param[in] node_id node id 
  */
dm_ret_t dm_restore_cache(void *p_handle, u8 block_id, u16 node_id);
/*!
  Flush data cache to certain block id and node id
  \param[in] p_handle data manager handle
  \param[in] block_id block id
  \param[in] node_id node id 
  */
dm_ret_t dm_flush_cache(void *p_handle, u8 block_id, u16 node_id);
/*!
  Set cache with given cache size to certain block id
  \param[in] p_handle data manager handle
  \param[in] block_id
  \param[in] cache_size the cache size set for the given block id
  */
dm_ret_t dm_set_cache(void *p_handle, u8 block_id, u16 cache_size);
/*!
  Get maximum node length 
  \param[in] p_handle data manager handle
  */
u16  dm_get_max_node_len(void *p_handle);
/*!
  Get maximum block number 
  \param[in] p_handle data manager handle
  */
u16 dm_get_max_blk_num(void *p_handle);
/*!
  Read data from data manager
  \param[in] p_handle data manager handle
  \param[in] block_id
  \param[in] node_id
  \param[in] offset starting position to be read in certain block
  \param[in] length data length to be read from flash
  \param[in] p_buffer buffer to stored data read from flash
  */
u32 dm_read(void *p_handle, u8 block_id, u16 node_id, u16 offset, 
            u32 length, u8 *p_buffer);

/*!
  Set DM header address, we support multi headers, so user can call this
  function repeatly.
  \param[in] p_handle data manager handle
  \param[in] addr Header address
  */
void dm_set_header(void *p_handle, u32 addr);

/*!
  Set DM header address, we support multi headers, so user can call this
  function repeatly.
  \param[in] p_handle data manager handle
  \param[in] addr Header address
  \param[in] block_id block id.
  */
void dm_set_header_ex(void *p_handle, u32 addr, dm_set_block_t block_id);

/*!
  section restore, datamanager supported only!!!
  \param[in] p_handle data manager handle
  */
void dm_data_restore(void *p_handle);

/*!
  set decompress buf for piece data !!!
  \param[in] p_handle data manager handle
  \param[in] p_buf the buf addr::: this buf should >=compress data +cache
  \param[in] buf_size size of buffer
  \param[in] p_cache cache
  \param[in] cache_size cache size
  */
void dm_set_decompress_buf(void *p_handle,u8 *p_buf, u32 buf_size,u8 *p_cache, u32 cache_size);

/*!
  store the piece data !!!
  \param[in] p_handle data manager handle
  \param[in] p_zip buffer for the zip data
  \param[in] zip_size size of p_zip
  \param[in] p_cache buffer for lzma compress
  \param[in] cache_size  this size must >=1.53M
  */
void dm_store_piece_data(void * p_handle,u8 *p_zip, u32 zip_size,
          u8 *p_cache,u32 cache_size);

/*!
  check flag
  \param[in] p_handle data manager handle
  */
BOOL dm_check_compress_match_flag(void * p_handle);

/*!
  set flag
  \param[in] p_handle data manager handle
  */
void dm_set_compress_unmatch_flag(void * p_handle);

/*!
  read data into read and write block 
  \param[in] p_handle : data manager handle  
  \param[in] block_id : block id
  \param[in] offset : read starting position in the block  
  \param[in] len : data lenght of data to be written
  \param[in] p_buf : data buffer with data to be written into flash
  */
u32 dm_direct_read(void *p_handle, u8 block_id, u32 offset, u32 len, u8 *p_buf);

/*!
  Write data into read and write block 
  \param[in] p_handle : data manager handle  
  \param[in] block_id : block id
  \param[in] offset : read starting position in the block  
  \param[in] len : data lenght of data to be written
  \param[in] p_buf : data buffer with data to be written into flash
  */
dm_ret_t dm_direct_write(void *p_handle, u8 block_id, u32 offset, 
  u32 len, u8 *p_buf);

/*!
  erase data into read and write block 
  \param[in] p_handle : data manager handle  
  \param[in] block_id : block id
  \param[in] offset : read starting position in the block  
  \param[in] len data lenght of data to be written
  */
dm_ret_t dm_direct_erase(void *p_handle, u8 block_id, u32 offset, 
  u32 len);

/*!
  get node id list of block.
  
  \param[in] p_handle : data manager handle  
  \param[in] block_id : block id
  \param[in] p_node_id_list : save node id list start address  
  \param[in] total_list : how can save node id.
    
  */
u32 dm_get_node_id_list(void *p_handle, u8 block_id, u16 *p_node_id_list, u32 total_list);

/*!
  get block node num

  \param[in] p_handle : data manager handle
  \param[in] block_id : block id
  */
u16 dm_get_blk_node_num(void * p_handle, u8 block_id);

#ifdef __cplusplus
}
#endif

#endif //__DATA_MANAGER_H_
