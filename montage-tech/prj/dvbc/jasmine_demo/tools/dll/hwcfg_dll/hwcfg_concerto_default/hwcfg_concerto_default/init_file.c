/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "comm_head.h"
#include "flash_Analysis.h"
//#include "comm_struct.h"
#include "..\..\uioconfig\system_configure_native_interface.h"
#include "block_process.h"

#include "uio.h"
#include "sys_hal.h"
#include "../../../../../includes/customer_config.h"

//#include "sys_status.h"

flash_file_t FlashFile;
NativePropertyArray_t *TRoot = NULL;

extern wchar_t *HWCfgSpace[HwCfgNum];
extern wchar_t *HWCfg_FPType_Space[2] ;
extern wchar_t *HWCfg_ComNum_Space[5];
extern wchar_t *HWCfg_LedNum_Space[5];
extern wchar_t *HWCfg_Status_Space[2];
extern wchar_t *HWCfg_ClockLevel_Space[2];
extern wchar_t *HWCfg_DataLevel_Space[2];
extern wchar_t *HWCfg_LBDType_Space[2];
extern wchar_t *HWCfg_LockLampLevel_Space[2];
extern wchar_t *HWCfg_PowerLampLevel_Space[2];
extern wchar_t *HWCfg_RecordLampLevel_Space[2];
extern wchar_t *HWCfg_Scan0Level_Space[2];
extern wchar_t *HWCfg_ScanType_Space[2];
extern wchar_t *HWCfg_Com0Level_Space[2];
extern wchar_t *HWCfg_Com1Level_Space[2];
extern wchar_t *HWCfg_Com2Level_Space[2];
extern wchar_t *HWCfg_Com3Level_Space[2];
extern wchar_t *HWCfg_Com4Level_Space[2];
extern wchar_t *HWCfg_Com5Level_Space[2];
extern wchar_t *HWCfg_Com6Level_Space[2];
extern wchar_t *HWCfg_Com7Level_Space[2];
extern wchar_t *HWCfg_DetectPinPol_Space[2];
extern wchar_t *HWCfg_VccEnablePol_Space[2];
extern wchar_t *HWCfg_VdacInfo_Space[10];
typedef struct
{
  u32 size;
  NativePropertyArray_t *first;
}BlockNativePropertyArray_t;


static u32 _GetBootBlockAddr()
{
  return 0x60000;
}

static u32 _GetMainBlockAddr()
{
  return 0x150000;
}


void InitFlashImg(wchar_t *patch)
{
  FILE*   fileHandle = NULL;
  u32 FileSize = 0;
  
  crc_setup_fast_lut();
  if((fileHandle = _wfopen( patch,L"r+")) == NULL)
  {
    abort();
  }

  memset(&FlashFile, 0, sizeof(FlashFile));
  fseek(fileHandle,0L,SEEK_END);
  FileSize = ftell(fileHandle);

  FlashFile.flash_size = FileSize;
  FlashFile.flash_buf = (u8 *)mtos_malloc(FileSize + 1);
  if(NULL == FlashFile.flash_buf)
  {
  	return ;//no memory
  }

  fseek(fileHandle,0L,SEEK_SET);

  fread(FlashFile.flash_buf,1,FileSize,fileHandle);

  FileBuild(FlashFile.flash_buf, &FlashFile.img_info[0], _GetBootBlockAddr());
  FileBuild(FlashFile.flash_buf, &FlashFile.img_info[1], _GetMainBlockAddr());

  fclose(fileHandle);
}

void DeinitFlashImg()
{
  if(FlashFile.flash_buf != NULL)
  {
    mtos_free(FlashFile.flash_buf);
  }
}

void InitNativePropertyArray(NativePropertyArray_t *Root)
{
  Root->size = GetAllModuleItmNum();
  Root->first = mtos_malloc(Root->size * sizeof(NativeProperty_t));

  memset(Root->first, 0, Root->size * sizeof(NativeProperty_t));
  
  TRoot = Root;
  if(Root->first == NULL)
  {
    abort();
  }
}

void DeInitNativePropertyArray(NativePropertyArray_t *Rt)
{
  NativePropertyArray_t *Root = NULL;

  if(Rt != NULL)
  {
    Root = Rt;
  }
  else
  {
    Root = TRoot;
  }
  
  if (Root == NULL)
  {
      return;
  }

  if(Root->first != NULL)
  {
    mtos_free(Root->first);
  }

  Root->size = 0;
}


void IrBlockInit(NativeProperty_t *ItmAddr)
{
  u8 IrBlockId[IrNum] = 
    {0xAE, 0xAF, 0xB1};
  u8 index = 0;
  wchar_t *IrName[IrNum] = 
  {
    {L"IR0"},
    {L"IR1"},
    {L"IR2"},
  };

  for(index = 0; index < IrNum; index ++)
  {
    IrBlockProcess(ItmAddr+index*IrModuleNum, IrName[index], IrBlockId[index], &FlashFile);
  }
}

void FpKeyBlockInit(NativeProperty_t *ItmAddr)
{
	u32 FpCfgIndex = 0;
	u8 FpCfgBlockId = {0xB0};
	wchar_t *FpCfgName = {L"FP"};
	u32 FpIndex = 0;
	
	FpCfgIndex = IrNum*IrModuleNum+FpIndex;
	FpCfgBlockProcess(ItmAddr+FpCfgIndex, FpCfgName, FpCfgBlockId, &FlashFile);
}

void HwCfgBlockInit(NativeProperty_t *ItmAddr)
{
	u32 HwCfgIndex = 0;
	u8 HwCfgBlockId = {0xBC};
	wchar_t *HwCfgName = {L"HW_CFG"};

	HwCfgIndex = IrNum*IrModuleNum+FpNum;
	HwCfgBlockProcess(ItmAddr+HwCfgIndex, HwCfgName, HwCfgBlockId, &FlashFile);
}

void SSdataBlockInit(NativeProperty_t *ItmAddr)
{
	u32 SSdataCfgIndex = 0;
	u8 SSdataCfgBlockId = {0x98};
	wchar_t *SSdataCfgName = {L"SSDATA"};
	u32 SSdataIndex = 2;
	
	SSdataCfgIndex = IrNum*IrModuleNum+SSdataIndex;
	SSdataCfgBlockProcess(ItmAddr+SSdataCfgIndex, SSdataCfgName, SSdataCfgBlockId, &FlashFile);
}

static s32 _NativePropertyItemProcess(BlockNativePropertyArray_t *Output, NativePropertyArray_t *Input)
{
  u32 Index_X = 0, Index_J = 0, Index = 1, CateGary = 1, Count = 0;
  NativeProperty_t Item;
#if 0
  for(Index_X = 0; Index_X < Input->size; Index_X ++)
  {
    for(Index_J = Index_X; Index_J < Input->size - 1; Index_J ++)
    {
      if(0 > wcsncmp(Input->first[Index_J].group, Input->first[Index_J + 1].group, SHORTLENGTH))
      {
        memcpy(&Item, &Input->first[Index_J], sizeof(NativeProperty_t));
        memcpy(&Input->first[Index_J], &Input->first[Index_J + 1], sizeof(NativeProperty_t));
        memcpy(&Input->first[Index_J + 1], &Item, sizeof(NativeProperty_t));
      }
    }
  }
#endif
  while(Index < Input->size)
  {
    if(0 != wcsncmp(Input->first[Index - 1].group, Input->first[Index].group, SHORTLENGTH))
    {
      CateGary ++;
    }
    Index ++;
  }

  Output->size = CateGary;

  Output->first = mtos_malloc(Output->size * sizeof(NativePropertyArray_t));

  Output->first[0].first = Input->first;
  CateGary = 0;
  Count = 0;
  Index = 1;
  
  while(Index < Input->size)
  {
    Count ++;
    if(0 != wcsncmp(Input->first[Index - 1].group, Input->first[Index].group, SHORTLENGTH))
    {
      CateGary ++;
      Output->first[CateGary - 1].size = Count;
      Count = 0;
      Output->first[CateGary].first = &Input->first[Index];
    }
    Index ++;
  }
  Output->first[CateGary].size = Count + 1;
}

static void _DeNativePropertyItemProcess(BlockNativePropertyArray_t *Output)
{
  if(Output->first != NULL)
  {
    mtos_free(Output->first);
  }
}

static s32 AllBlockGenerate(NativeProperty_t *ItmAddr, block_buf_t *Block, u32 size)
{

  	u8 AllBlockId[6] = 
		{0xAE, 0xAF, 0xB1, 0xB0, 0xBC, 0x98};
	u8 index = 0;
	u8 BlockId = 0, allBlockNum = 0;
	s32 Ret = SUCCESS;
  
	wchar_t *AllGroupName[6] = 
	{
		{L"IR0"},
		{L"IR1"},
		{L"IR2"},
		{L"FP"},
		{L"HW_CFG"},
		{L"SSDATA"},
	};
  if(ItmAddr == NULL)
  {
    return ERR_FAILURE;
  }
  allBlockNum = 6;
  
  for(index = 0; index < allBlockNum; index ++)
  {
    if(0 == wcsncmp(AllGroupName[index], ItmAddr[index].group, SHORTLENGTH))
    {
      BlockId = AllBlockId[index];
      break;
    }
  }
  if(index>=0 && index <=2)
  {
  	Ret = IrBlockMerge(ItmAddr, Block, size);
  }
  else if(index == 3)
  {
  	Ret = FPBlockMerge(ItmAddr, Block, size);
  }
  else if(index == 4)
  {
  	Ret = HWCfgBlockMerge(ItmAddr, Block, size);
  }
  else
  {
  	return ERR_FAILURE;
  }

  if(Ret != SUCCESS)
  {
    DeBlockRead(Block);
    return Ret;
  }
  Block->block_id = BlockId;

  return Ret;
  
  //DeBlockRead(&Block);
}

//save block individually
void SaveBlockIndep(wchar_t* file_path, NativePropertyArray_t* all_propertys)
{
	FILE*   fileHandle = NULL;
	char asc_str[10];
	u32 Data = 0;
	hw_cfg_t *hw_cfg_v = NULL;
	u8 * block_real_buf = NULL;
	u8  BlockId =0, BlockNumIndex = 0, img_index = 0;
	u32 index = 0, index_num = 0, group_num = 11, Index_J=0;
	u32 block_size = 0, block_real_size = 0;
	NativeProperty_t* blockfirst = NULL;
	u8 AllBlockId[6] = 
           {0xAE, 0xAF, 0xB1, 0xB0, 0xBC, 0x98};
	wchar_t *AllGroupName[6] = 
	{
		{L"IR0"},
		{L"IR1"},
		{L"IR2"},
		{L"FP"},
		{L"HW_CFG"},
		{L"SSDATA"},
	};

	if(NULL == all_propertys)
	{
		return;
	}
	
	//all the item size saved in block_size
	block_size = all_propertys->size;
	blockfirst = all_propertys->first;

	//get the block ID based on group name
	for(index = 0; index < group_num; index ++)
	{
		if(0 == wcsncmp(AllGroupName[index], blockfirst->group, SHORTLENGTH))
		{
			BlockId = AllBlockId[index];
			break;
		}
	}
	if(index == group_num)
	{
		return; //error, can not find matched block ID
	}

	//get the item position based on block id
	for(index_num = 0; index_num < FlashFile.img_info[0].block_num; index_num++)
	{
		if(BlockId == FlashFile.img_info[0].blocks[index_num].id)
		{
			BlockNumIndex = index_num;
			img_index = 0;
			break;
		}
	}

	if (index_num == FlashFile.img_info[0].block_num)
	{
		for(index_num = 0; index_num < FlashFile.img_info[1].block_num; index_num++)
		{
			if(BlockId == FlashFile.img_info[1].blocks[index_num].id)
			{
				BlockNumIndex = index_num;
				img_index = 1;
				break;
			}
		}
		if(index_num == FlashFile.img_info[1].block_num)
		{
			return ;//error, can not find matched item
		}
	}
	block_real_size = FlashFile.img_info[img_index].blocks[BlockNumIndex].size;

	block_real_buf= (u8 *)mtos_malloc(block_real_size + 1);
	if(NULL == block_real_buf)
	{
		return;//error,can not allocate memory
	}
	memset(block_real_buf, 0 , sizeof(u8) * block_real_size);

	memcpy(block_real_buf, (u8 *)(FlashFile.flash_buf + (u32)FlashFile.img_info[img_index].addr_offset + (u32)FlashFile.img_info[img_index].blocks[BlockNumIndex].base_addr), sizeof(u8) * block_real_size);
	
	hw_cfg_v = (hw_cfg_t *)block_real_buf;
	//transfer and save the coorelative block raw data
	if(0xBC == BlockId) //hw cfg raw data case
	{
		for(index = 0; index < block_size; index ++)
		{
			if(0 == wcsncmp(blockfirst[index].key, L"FP_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->fp_type = Data;*/
				for(Index_J = 0; Index_J < FpTypeNum; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_FPType_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(FpTypeNum == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->fp_type = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_NUM", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.num_com = (u8)Data;	*/
				for(Index_J = 0; Index_J < 5; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_ComNum_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(5 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.num_com = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"LED_NUM", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->led_num = (u8)Data;	*/
				for(Index_J = 0; Index_J < 5; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_LedNum_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(5 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->led_num = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COLON_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.pos_colon = (u8)Data;	
			}		
			else if(0 == wcsncmp(blockfirst[index].key, L"STATUS", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.status = (u8)Data;*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Status_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				//not change default value
			//	hw_cfg_v->pan_info.status = Index_J;						
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"CLOCK_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fclock.polar = (u8)Data;*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_ClockLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.fclock.polar = Index_J;					
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"CLOCK_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fclock.pos = (u8)Data;		
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"DATA_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fdata.polar = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_DataLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.fdata.polar = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"DATA_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fdata.pos = (u8)Data;		
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"LBD_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.type_lbd = (u8)Data;		*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_LBDType_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.type_lbd = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"POWER_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[0].pos = (u8)Data;		
			}		
			else if(0 == wcsncmp(blockfirst[index].key, L"POWER_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[0].polar = (u8)Data;	*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_LockLampLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.lbd[0].polar = Index_J;					
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"LOCK_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[1].pos = (u8)Data;		
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"LOCK_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[1].polar = (u8)Data;		*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_PowerLampLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.lbd[1].polar = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"RECORD_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[2].pos = (u8)Data;		
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"RECORD_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[1].polar = (u8)Data;		*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_RecordLampLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.lbd[2].polar = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"SCAN_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.type_scan = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_ScanType_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.type_scan = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"SCAN_0_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"SCAN_0_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Scan0Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.scan[0].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_0_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[0].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_0_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com0Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[0].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_1_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[1].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_1_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com1Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[1].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_2_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[2].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_2_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com2Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[2].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_3_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[3].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_3_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com3Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[3].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_4_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[4].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_4_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com4Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[4].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_5_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[5].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_5_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com5Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[5].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_6_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[6].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_6_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com6Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[6].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_7_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[7].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_7_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com7Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[7].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"DETECT_PIN_POL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_DetectPinPol_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->detect_pin_pol = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"VCC_ENABLE_POL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_VccEnablePol_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->vcc_enable_pol = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"VDAC_INFO", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 10; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_VdacInfo_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(10 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->vdac_info = Index_J;				
			}
		}
	}
	else
	{
		for(index = 0; index < block_size; index ++)
		{
			wcstombs(asc_str, blockfirst[index].value, 10);
			Data = atoi(asc_str);
			if(Data > 255 || Data < 0)
			{
				return ;//error
			}
			block_real_buf[index] = (u8)Data;
		}
	}
	
	if((fileHandle = _wfopen( file_path,L"wb")) == NULL)
	{
		abort();
	}
	fseek(fileHandle,0L,SEEK_SET);
	fwrite((const void*)block_real_buf, block_real_size, 1, fileHandle);

	fclose(fileHandle);
	if(block_real_buf)
	{
		mtos_free(block_real_buf);
		block_real_buf = NULL;
	}
	return ;
}

//for save to flash.bin
#define MAX_BLOCK_SIZE 4096
void SaveBlocktoFlash(wchar_t* file_path, NativePropertyArray_t* all_propertys)
{
  block_buf_t Block;
  FILE*   fileHandle = NULL;
  s32 Ret = 0;
  BlockNativePropertyArray_t AllBlockPro = {0};
  u32 BlockNum = 0;
  u32 Index = 0;

  _NativePropertyItemProcess(&AllBlockPro, all_propertys);

  BlockNum = AllBlockPro.size;

	Block.buf = (u8 *)mtos_malloc(MAX_BLOCK_SIZE);
  while(Index < BlockNum)
  {
    Ret = AllBlockGenerate(AllBlockPro.first[Index].first, &Block, AllBlockPro.first[Index].size);
    if(Ret != SUCCESS)
    {
      goto exit_save_to_flash;
    }
  
    Ret = BlockWrite(&FlashFile, &Block);
	
    if(Ret != SUCCESS)
    {
      goto exit_save_to_flash;
    }

    Index ++;
  }
  
  if((fileHandle = _wfopen( file_path,L"wb")) == NULL)
  {
    abort();
  }

  fseek(fileHandle,0L,SEEK_SET);

  fwrite(FlashFile.flash_buf, FlashFile.flash_size, 1, fileHandle);
	fflush(fileHandle);
  fclose(fileHandle);

exit_save_to_flash:
  DeBlockRead(&Block);

  _DeNativePropertyItemProcess(&AllBlockPro);

  return ;//Ret;
}

void HwCfgModifyBlock(NativePropertyArray_t* all_propertys, char *block_buff)
{
	char asc_str[10];
	u32 Data = 0;
	hw_cfg_t *hw_cfg_v = NULL;
	u8  BlockId =0, BlockNumIndex = 0, img_index = 0;
	u32 index = 0, index_num = 0, group_num = 11, Index_J=0;
	u32 block_size = 0, block_real_size = 0;
	NativeProperty_t* blockfirst = NULL;
	u8 AllBlockId[6] = 
           {0xAE, 0xAF, 0xB1, 0xB0, 0xBC, 0x98};
	wchar_t *AllGroupName[6] = 
	{
		{L"IR0"},
		{L"IR1"},
		{L"IR2"},
		{L"FP"},
		{L"HW_CFG"},
		{L"SSDATA"},
	};

	if(NULL == all_propertys)
	{
		return;
	}
	
	//all the item size saved in block_size
	block_size = all_propertys->size;
	blockfirst = all_propertys->first;

	//get the block ID based on group name
	for(index = 0; index < group_num; index ++)
	{
		if(0 == wcsncmp(AllGroupName[index], blockfirst->group, SHORTLENGTH))
		{
			BlockId = AllBlockId[index];
			break;
		}
	}
	if(index == group_num)
	{
		return; //error, can not find matched block ID
	}

	//get the item position based on block id
	for(index_num = 0; index_num < FlashFile.img_info[0].block_num; index_num++)
	{
		if(BlockId == FlashFile.img_info[0].blocks[index_num].id)
		{
			BlockNumIndex = index_num;
			img_index = 0;
			break;
		}
	}

	if (index_num == FlashFile.img_info[0].block_num)
	{
		for(index_num = 0; index_num < FlashFile.img_info[1].block_num; index_num++)
		{
			if(BlockId == FlashFile.img_info[1].blocks[index_num].id)
			{
				BlockNumIndex = index_num;
				img_index = 1;
				break;
			}
		}
		if(index_num == FlashFile.img_info[1].block_num)
		{
			return ;//error, can not find matched item
		}
	}
	block_real_size = FlashFile.img_info[img_index].blocks[BlockNumIndex].size;

	if(NULL == block_buff)
	{
		return;//error,can not allocate memory
	}
	memset(block_buff, 0 , sizeof(u8) * block_real_size);

	memcpy(block_buff, (u8 *)(FlashFile.flash_buf + (u32)FlashFile.img_info[img_index].addr_offset + (u32)FlashFile.img_info[img_index].blocks[BlockNumIndex].base_addr), sizeof(u8) * block_real_size);
	
	hw_cfg_v = (hw_cfg_t *)block_buff;
	//transfer and save the coorelative block raw data
	if(0xBC == BlockId) //hw cfg raw data case
	{
		for(index = 0; index < block_size; index ++)
		{
			if(0 == wcsncmp(blockfirst[index].key, L"FP_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->fp_type = Data;*/
				for(Index_J = 0; Index_J < FpTypeNum; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_FPType_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(FpTypeNum == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->fp_type = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_NUM", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.num_com = (u8)Data;	*/
				for(Index_J = 0; Index_J < 5; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_ComNum_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(5 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.num_com = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"LED_NUM", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->led_num = (u8)Data;	*/
				for(Index_J = 0; Index_J < 5; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_LedNum_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(5 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->led_num = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COLON_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.pos_colon = (u8)Data;	
			}		
			else if(0 == wcsncmp(blockfirst[index].key, L"STATUS", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.status = (u8)Data;*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Status_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				//not change default value
			//	hw_cfg_v->pan_info.status = Index_J;						
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"CLOCK_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fclock.polar = (u8)Data;*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_ClockLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.fclock.polar = Index_J;					
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"CLOCK_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fclock.pos = (u8)Data;		
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"DATA_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fdata.polar = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_DataLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.fdata.polar = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"DATA_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.fdata.pos = (u8)Data;		
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"LBD_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.type_lbd = (u8)Data;		*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_LBDType_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.type_lbd = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"POWER_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[0].pos = (u8)Data;		
			}		
			else if(0 == wcsncmp(blockfirst[index].key, L"POWER_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[0].polar = (u8)Data;	*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_LockLampLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.lbd[0].polar = Index_J;					
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"LOCK_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[1].pos = (u8)Data;		
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"LOCK_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[1].polar = (u8)Data;		*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_PowerLampLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.lbd[1].polar = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"RECORD_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[2].pos = (u8)Data;		
			}	
			else if(0 == wcsncmp(blockfirst[index].key, L"RECORD_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.lbd[1].polar = (u8)Data;		*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_RecordLampLevel_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.lbd[2].polar = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"SCAN_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.type_scan = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_ScanType_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.type_scan = Index_J;					
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"SCAN_0_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"SCAN_0_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Scan0Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.scan[0].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_0_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[0].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_0_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com0Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[0].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_1_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[1].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_1_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com1Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[1].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_2_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[2].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_2_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com2Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[2].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_3_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[3].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_3_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com3Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[3].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_4_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[4].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_4_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com4Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[4].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_5_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[5].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_5_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com5Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[5].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_6_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[6].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_6_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com6Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[6].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_7_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.com[7].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"COM_7_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_Com7Level_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->pan_info.com[7].polar = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"DETECT_PIN_POL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_DetectPinPol_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->detect_pin_pol = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"VCC_ENABLE_POL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_VccEnablePol_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->vcc_enable_pol = Index_J;				
			}
			else if(0 == wcsncmp(blockfirst[index].key, L"VDAC_INFO", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, blockfirst[index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;		*/	
				for(Index_J = 0; Index_J < 10; Index_J++)
				{
					if(0 == wcsncmp(blockfirst[index].value, HWCfg_VdacInfo_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(10 == Index_J)
				{
					Index_J = 0;
				}
				hw_cfg_v->vdac_info = Index_J;				
			}
		}
	}
	return ;
}

unsigned int HwCfgQueryValue(wchar_t* key,wchar_t* str_value)
{
	return HwCfgBlockQueryValue(key,str_value);
}

