/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "..\..\uioconfig\system_configure_native_interface.h"

extern "C" 
{
  #include "init_file.h"
}

int ProjectInitial( wchar_t* initial_parameter )
{
    return 0;
}


int Load( wchar_t* file_path, struct NativePropertyArray* all_propertys )
{
	InitFlashImg(file_path);
	InitNativePropertyArray(all_propertys);

	IrBlockInit(all_propertys->first);
	FpKeyBlockInit(all_propertys->first);
	HwCfgBlockInit(all_propertys->first);
//	SSdataBlockInit(all_propertys->first);
    return 0;
}


int Save( wchar_t* file_path, struct NativePropertyArray* all_propertys )
{
    SaveBlockIndep(file_path, all_propertys);
    return 0;
}


int Clean()
{
    DeInitNativePropertyArray(NULL);
    DeinitFlashImg();
    return 0;
}


void GetLastErrorMsg( wchar_t* info, size_t info_length )
{
    int i = 0;
    return;
}

int SaveToFlash( wchar_t* file_path, struct NativePropertyArray* all_propertys )
{	
    SaveBlocktoFlash(file_path, all_propertys);
    return 0;
}

int ModifyBlock(struct NativePropertyArray* block_propertys,char* block_buff)
{
    HwCfgModifyBlock(block_propertys, block_buff);
    return 0;
}

unsigned int QueryValue(wchar_t* key,wchar_t* str_value)
{
	return HwCfgQueryValue(key, str_value);
}