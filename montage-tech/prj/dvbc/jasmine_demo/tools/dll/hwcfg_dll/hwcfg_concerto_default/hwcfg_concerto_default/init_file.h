/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _INIT_FILE_H_
#define _INIT_FILE_H_

void InitFlashImg(wchar_t *patch);

void DeinitFlashImg();

void InitNativePropertyArray(NativePropertyArray_t *Root);

void DeInitNativePropertyArray(NativePropertyArray_t *Root);

void IrBlockInit(NativeProperty_t *ItmAddr);
void FpKeyBlockInit(NativeProperty_t *ItmAddr);
void HwCfgBlockInit(NativeProperty_t *ItmAddr);
void SSdataBlockInit(NativeProperty_t *ItmAddr);

//s32 IrBlockGenerate(NativeProperty_t *ItmAddr, block_buf_t Block);
void SaveBlocktoFlash(wchar_t* file_path, NativePropertyArray_t* all_propertys);
void SaveBlockIndep(wchar_t* file_path, NativePropertyArray_t* all_propertys);
void HwCfgModifyBlock(NativePropertyArray_t* all_propertys, char *block_buff);
unsigned int HwCfgQueryValue(wchar_t* key,wchar_t* str_value);
#endif
