/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _FLASH_ANALYSIS_H_
#define _FLASH_ANALYSIS_H_


void FileBuild(u8 *p_flash_buf, img_file_t *p_file, u32 addr_offset);

s32 BlockRead_EX(block_buf_t **Block, u16 Num);

s8 BlockRead(u8 BlockId, flash_file_t *file, block_buf_t *Block);

s8 BlockWrite(flash_file_t *file, block_buf_t *Block);

void DeBlockRead(block_buf_t *Block);

void DeBlockRead_EX(block_buf_t *Block, u16 Num);

#endif
