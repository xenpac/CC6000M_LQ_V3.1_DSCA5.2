/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _BLOCK_PROCESS_H_
#define _BLOCK_PROCESS_H_

#define IrNum 3
#define IrModuleNum (75 + 2)
#define FpNum (9)
#define HwCfgNum (38)
#define FpTypeNum (9)

u16 GetAllModuleItmNum();
void crc_setup_fast_lut();
u32 crc_fast_calculate(u32 crc, unsigned char *buf, int len);
void IrBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file);
void FpCfgBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file);
void HwCfgBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file);
void SSdataCfgBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file);
s32 IrBlockMerge(NativeProperty_t *ItmAddr, block_buf_t *Block, u32 size);
unsigned int HwCfgBlockQueryValue(wchar_t* key,wchar_t* str_value);
#endif
