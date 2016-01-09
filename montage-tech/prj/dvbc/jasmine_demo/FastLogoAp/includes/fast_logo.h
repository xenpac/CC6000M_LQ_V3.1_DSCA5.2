/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/

/******************************************************************************/
#ifndef __FAST_LOGO_H_
#define __FAST_LOGO_H_

#include "data_manager.h"

#define FLASH_START_ADDRESS   0x90000000

/*!
  DM_GET_BLOCK_ID
  */
#define FL_GET_BLOCK_ID(block_id, h_addr, i)                                          \
do{                                                                           \
    (block_id) =  (*((volatile u8 *) \
    ((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + FLASH_START_ADDRESS)));  \
}while(0)

/*!
  DM_GET_ADDR
  */
#define FL_GET_ADDR(addr, h_addr, i)                                          \
do{                                                                           \
    (addr) =  (*((volatile u32 *) \
    ((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 4 + FLASH_START_ADDRESS)));  \
    (addr) += (h_addr + FLASH_START_ADDRESS);                                                       \
}while(0)

/*!
  DM_GET_SIZE
  */
#define FL_GET_SIZE(size, h_addr, i)                                          \
do{                                                                           \
    (size) = (*((volatile u32 *)((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 8 + FLASH_START_ADDRESS)));\
}while(0)

/*!
  DM_GET_BLOCK_NUM
  */
#define FL_GET_BLOCK_NUM(num, h_addr)                     \
do{                                                       \
  (num) = (*((volatile u16 *)((h_addr) + 16 + 8 + FLASH_START_ADDRESS)));                  \
}while(0)

extern u32 get_flash_addr(void);

#endif

