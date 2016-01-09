/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _COMM_HEAD_H_
#define _COMM_HEAD_H_

#include "sys_types.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "wstring.h"
#include "WCHAR.h"
//#include "windows.h"

#define FLASH_DEBUG

#define MAKE_WORD(low, high) \
  ((u16)(((u8)(low)) | \
         (((u16)((u8)(high))) << 8)))

#define MT_MAKE_DWORD(low, high) \
  ((u32)(((u16)(low)) | \
         (((u32)((u16)(high))) << 16)))
#if 0
#define MT_UNMAKE_DWORD(data, p, bit)\
{\
  u8 i = 0;\
  for(i = 0; i < bit; i++)\
  {\
    p[i] = (data >> (i *8)) & 0x000000ff;\ 
  }\
}
#endif
#ifdef FLASH_DEBUG
#define OS_PRINTF printf
#else
#define OS_PRINTF(...)     do{}while(0) 
#endif

#define mtos_malloc malloc
#define mtos_free free

#define DM_MAX_BLOCK_NUM             (32)

typedef struct
{
  u8 id;
  u8 type; 
  u16 node_num;
  u32 base_addr;
  u32 size;
  u32 crc;
  u8 version[8];
  u8 name[8];
  u8 time[12];
  u16 ota_ver;
  u16 reserve;
}flash_block_t;

typedef struct
{
  u32 sdram_size;
  u32 flash_size;
  u16 block_num;
  u16 bh_size;
  u32 addr_offset;
  flash_block_t blocks[DM_MAX_BLOCK_NUM];
}img_file_t;

typedef struct
{
  img_file_t img_info[2];

  u8 *flash_buf;

  u32 flash_size;
  
}flash_file_t;

typedef struct
{
  u8 *buf;
  u32 size;
  u8 img_num;
  u8 block_id;
}block_buf_t;


#endif
