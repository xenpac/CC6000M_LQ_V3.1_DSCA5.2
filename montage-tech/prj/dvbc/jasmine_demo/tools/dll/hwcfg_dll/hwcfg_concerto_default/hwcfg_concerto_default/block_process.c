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

#define IrModuleNum (75 + 2)

extern flash_file_t FlashFile;
hw_cfg_t *hw_cfg = NULL;
u16 GetAllModuleItmNum()
{
  u16 AllNum 			= 0;
  u16 AllIrNum 		= IrModuleNum * IrNum;
  u16 AllFpKeyNum 	= FpNum;
  u16 AllHwCfgNum 	= HwCfgNum;
  u16 AllSSdataNum 	= 0;
  
  AllNum = AllIrNum+AllHwCfgNum+AllFpKeyNum+AllSSdataNum;
  return AllNum;
}


//for ir
static wchar_t *IrNameSpace[IrModuleNum] = 
{
  {L"USERCODE_H"},
  {L"USERCODE_L"},
  {L"V_KEY_POWER"},          //p
  {L"V_KEY_MUTE"},           //j
  {L"V_KEY_RECALL"},         //h
  {L"V_KEY_0"},              //0
  {L"V_KEY_1"},              //1
  {L"V_KEY_2"},              //2
  {L"V_KEY_3"},              //3
  {L"V_KEY_4"},              //4
  {L"V_KEY_5"},              //5
  {L"V_KEY_6"},              //6
  {L"V_KEY_7"},              //7
  {L"V_KEY_8"},              //8
  {L"V_KEY_9"},              //9
  {L"V_KEY_FAV"},            //f
  {L"V_KEY_TVRADIO"},        //t
  {L"V_KEY_UP"},             //direction key up
  {L"V_KEY_LEFT"},           //direction key left
  {L"V_KEY_OK"},             //enter
  {L"V_KEY_RIGHT"},          //direction key right
  {L"V_KEY_DOWN"},           //direction key down
  {L"V_KEY_MENU"},           //m
  {L"V_KEY_CANCEL"},         //c
  {L"V_KEY_PAGE_UP"},        //Page up
  {L"V_KEY_PAGE_DOWN"},      //Page down
  {L"V_KEY_RED"},            //'
  {L"V_KEY_GREEN"},          ///
  {L"V_KEY_BLUE"},           //.
  {L"V_KEY_YELLOW"},         //,};
  {L"V_KEY_TTX"},
  {L"V_KEY_EPG"},
  {L"V_KEY_AUDIO"},
  {L"V_KEY_F1"},
  {L"V_KEY_F2"},
  {L"V_KEY_F3"},
  {L"V_KEY_F4"},
  {L"V_KEY_INFO"},
  {L"V_KEY_REC"},//recommend key
  {L"V_KEY_LANG"},//language switch
  {L"V_KEY_VUP"},//volume up
  {L"V_KEY_VDOWN"},//volume down
  {L"V_KEY_EXIT"},//exit
  {L"V_KEY_BACK"},
  {L"V_KEY_GAME"},//game
  {L"V_KEY_TV_PLAYBACK"},//playback
  {L"V_KEY_SEARCH"},  //search
  {L"V_KEY_MAIL"},      //mail
  {L"V_KEY_DATA_BROADCAST"},             //DATA BOARDCAST
  {L"V_KEY_NVOD"},           //NVOD
  {L"V_KEY_FAVUP"},             //FAVUP
  {L"V_KEY_FAVDOWN"},            //FAVDOWN
  {L"V_KEY_STOCK"},            //stock
  {L"V_KEY_BOOK"},            //book
  {L"V_KEY_RADIO"},            //radio
  {L"V_KEY_TV"},            //tv
  {L"V_KEY_F5"},               //F5
  {L"V_KEY_PAUSE"},               //pause
  {L"V_KEY_CHUP"},            //channel up
  {L"V_KEY_CHDOWN"},            //channel down
  {L"V_KEY_INPUT"},            //input method
  {L"V_KEY_HELP"},               //help
  {L"V_KEY_SET"},               //set
  {L"V_KEY_SUBT"},
  {L"V_KEY_UNKNOW"},         //media
  {L"V_KEY_GOTO"},
  {L"V_KEY_ZOOM"},
  {L"V_KEY_VIDEO_MODE"},
  {L"V_KEY_ASPECT_MODE"},
  {L"V_KEY_CHANNEL_LIST"},       //
  {L"V_KEY_STOP"},
  {L"V_KEY_PREV"},
  {L"V_KEY_NEXT"},
  {L"V_KEY_BACK2"},
  {L"V_KEY_FORW2"},
  {L"V_KEY_LIST"},
  {L"V_KEY_PLAY"},      
};

static wchar_t *FpNameSpace[FpNum] = 
{
  {L"V_KEY_POWER"},
  {L"V_KEY_UP"},
  {L"V_KEY_DOWN"},
  {L"V_KEY_LEFT"},
  {L"V_KEY_RIGHT"},
  {L"V_KEY_OK"},
  {L"V_KEY_MENU"},
  {L"V_KEY_VUP"}, 
  {L"V_KEY_VDOWN"},
};

wchar_t *HWCfgSpace[HwCfgNum] = 
{
  {L"FP_TYPE"},
  {L"COM_NUM"},
  {L"LED_NUM"},
  {L"COLON_POS"},
  {L"STATUS"},
  {L"CLOCK_LEVEL"},
  {L"CLOCK_POS"},
  {L"DATA_LEVEL"},
  {L"DATA_POS"},
  {L"LBD_TYPE"}, 
  {L"POWER_LAMP_POS"},
  {L"POWER_LAMP_LEVEL"},
  {L"LOCK_LAMP_POS"},
  {L"LOCK_LAMP_LEVEL"},
  {L"RECORD_LAMP_POS"},
  {L"RECORD_LAMP_LEVEL"},
  {L"SCAN_TYPE"},
  {L"SCAN_0_POS"},
  {L"SCAN_0_LEVEL"},
  {L"COM_0_POS"},
  {L"COM_0_LEVEL"},
  {L"COM_1_POS"},
  {L"COM_1_LEVEL"},
  {L"COM_2_POS"},
  {L"COM_2_LEVEL"},
  {L"COM_3_POS"},
  {L"COM_3_LEVEL"},
  {L"COM_4_POS"},
  {L"COM_4_LEVEL"},
  {L"COM_5_POS"},
  {L"COM_5_LEVEL"},
  {L"COM_6_POS"},
  {L"COM_6_LEVEL"},
  {L"COM_7_POS"},
  {L"COM_7_LEVEL"},
  {L"DETECT_PIN_POL"},
  {L"VCC_ENABLE_POL"},
  {L"VDAC_INFO"},
};

wchar_t *HWCfg_FPType_Space[FpTypeNum] = 
{
  {L"FP_CT1642"},
  {L"FP_HC164"},
  {L"FP_FD650"},
  {L"FP_GPIO"},
  {L"FP_TM1637"},
  {L"FP_TM1635"},
  {L"FP_FD620"},
  {L"FP_TM1618"},
  {L"FP_LM8168"},
};

wchar_t *HWCfg_ComNum_Space[5] = 
{
  {L"0"},
  {L"1"},
  {L"2"},
  {L"3"},
  {L"4"},  
};

wchar_t *HWCfg_LedNum_Space[5] = 
{
  {L"0"},
  {L"1"},
  {L"2"},
  {L"3"},
  {L"4"},  
};

wchar_t *HWCfg_Status_Space[2] = 
{
  {L"默认位置"},
  {L"设置数码管位置"},
};
wchar_t *HWCfg_ClockLevel_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_DataLevel_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_LBDType_Space[2] = 
{
  {L"IO控制"},
  {L"MCU控制"},
};
wchar_t *HWCfg_LockLampLevel_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_PowerLampLevel_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_RecordLampLevel_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Scan0Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_ScanType_Space[2] = 
{
  {L"slot"},
  {L"shadow"},
};
wchar_t *HWCfg_Com0Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Com1Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Com2Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Com3Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Com4Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Com5Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Com6Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};
wchar_t *HWCfg_Com7Level_Space[2] = 
{
  {L"low"},
  {L"high"},
};

wchar_t *HWCfg_VccEnablePol_Space[2] = 
{
  {L"0"},
  {L"1"},
};

wchar_t *HWCfg_DetectPinPol_Space[2] = 
{
  {L"0"},
  {L"1"},
};

wchar_t *HWCfg_VdacInfo_Space[10] = 
{
  {L"VDAC_CVBS_RGB"},
  {L"VDAC_SIGN_CVBS"},
  {L"VDAC_SIGN_CVBS_LOW_POWER"},
  {L"VDAC_DUAL_CVBS"},
  {L"VDAC_SIGN_SVIDEO"},
  {L"VDAC_CVBS_SVIDEO"},
  {L"VDAC_CVBS_YPBPR_SD"},
  {L"VDAC_CVBS_YPBPR_HD"},
  {L"VDAC_RESERVED1"},
  {L"VDAC_RESERVED2"},
};
static u32 m_crc_table[256] = {0,};
void crc_setup_fast_lut()
{
	unsigned long count, crc;
	int i;
	u32 *crc_lut = m_crc_table;

	for (count = 0; count <= 255; count++) 
	{
		crc = count;
		for (i = 0; i < 8; i++) 
		{
			if (crc & 1)					/*Lowest bit procedure*/
				crc = (crc >> 1) ^ 0xedb88320;
			else
				crc >>= 1;
		}
		crc_lut[count] = crc & 0xffffffff;	/*Get lower 32 bits FCS*/
	}

}
u32 crc_fast_calculate(u32 crc, unsigned char *buf, int len)
{
	int i;
	u32 *crc_lut= m_crc_table;
  
	for (i = 0; i < len; i++)
		crc = (crc_lut[(crc & 0xff) ^ buf[i]] ^ (crc >> 8)) & 0xffffffff;
	return crc;
}

void IrBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file)
{
  wchar_t Index = 0;
  s8 Ret = 0;
  u8 *p_addr = NULL;
  block_buf_t Block;
  char asc_str[10];

  Ret = BlockRead(BlockId, file, &Block);

  if(Ret < 0)
  {
    abort();
  }

  p_addr = Block.buf;

  for(Index = 0; Index < IrModuleNum; Index++)
  {
    memcpy(ItmAddr[Index].group, GroupName, wcslen(GroupName) * sizeof(wchar_t));
    memcpy(ItmAddr[Index].key, IrNameSpace[Index], wcslen(IrNameSpace[Index]) * sizeof(wchar_t));
    if(Index >= Block.size)
    {
      abort();
    }

  sprintf(asc_str, "%d", (u8)(*p_addr++));

   mbstowcs(ItmAddr[Index].value, asc_str, 10);
   ItmAddr[Index].m_validation = NULL;
  }

  DeBlockRead(&Block);
}


void FpCfgBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file)
{
	wchar_t Index = 0;
	s8 Ret = 0;
	u8 *p_addr = NULL;
	block_buf_t Block;
	char asc_str[10];

	Ret = BlockRead(BlockId, file, &Block);
	if(Ret < 0)
	{
		abort();
	}

	p_addr = Block.buf;
	Index = 0;
	for(Index = 0; Index < FpNum; Index++)
	{
		memcpy(ItmAddr[Index].group, GroupName, wcslen(GroupName) * sizeof(wchar_t));
		memcpy(ItmAddr[Index].key, FpNameSpace[Index], wcslen(FpNameSpace[Index]) * sizeof(wchar_t));
		if(Index >= Block.size)
		{
			abort();
		}
		sprintf(asc_str, "%d", (u8)(*p_addr++));
		mbstowcs(ItmAddr[Index].value, asc_str, 10);
		ItmAddr[Index].m_validation = NULL;
	}

	DeBlockRead(&Block);
}

void HwCfgBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file)
{
	wchar_t Index = 0;
	s8 Ret = 0;
	u8 *p_addr = NULL;
	block_buf_t Block;
	char asc_str[10];
	char asc_opt_str[64];
	hw_cfg_t *hw_cfg_v = NULL;
	wchar_t * hwcfg_char = L";";

	Ret = BlockRead(BlockId, file, &Block);
	if(Ret < 0)
	{
		abort();
	}
	
	hw_cfg_v = (hw_cfg_t *)Block.buf;
	hw_cfg = hw_cfg_v;
	Index = 0;
	for(Index = 0; Index < HwCfgNum; Index++)
	{
		memcpy(ItmAddr[Index].group, GroupName, wcslen(GroupName) * sizeof(wchar_t));
		memcpy(ItmAddr[Index].key, HWCfgSpace[Index], wcslen(HWCfgSpace[Index]) * sizeof(wchar_t));
		if(Index >= Block.size)
		{
			abort();
		}

		if(0 == wcsncmp(HWCfgSpace[Index], L"FP_TYPE", SHORTLENGTH))
		{
			if( hw_cfg_v->fp_type >=0 && hw_cfg_v->fp_type < FpTypeNum)
			{
				memcpy(ItmAddr[Index].value, HWCfg_FPType_Space[hw_cfg_v->fp_type], wcslen(HWCfg_FPType_Space[hw_cfg_v->fp_type]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_FPType_Space[0], wcslen(HWCfg_FPType_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_FPType_Space[0], wcslen(HWCfg_FPType_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_FPType_Space[1]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_FPType_Space[2]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_FPType_Space[3]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_FPType_Space[4]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_FPType_Space[5]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_FPType_Space[6]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_FPType_Space[7]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_NUM", SHORTLENGTH))
		{
//			sprintf(asc_str, "%d", hw_cfg_v->pan_info.num_com);
//			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.num_com >=0 && hw_cfg_v->pan_info.num_com < 5)
			{
				memcpy(ItmAddr[Index].value, HWCfg_ComNum_Space[hw_cfg_v->pan_info.num_com], wcslen(HWCfg_ComNum_Space[hw_cfg_v->pan_info.num_com]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_ComNum_Space[0], wcslen(HWCfg_ComNum_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_ComNum_Space[0], wcslen(HWCfg_ComNum_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_ComNum_Space[1]/*, wcslen(HWCfg_ComNum_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_ComNum_Space[2]/*, wcslen(HWCfg_ComNum_Space[2]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_ComNum_Space[3]/*, wcslen(HWCfg_ComNum_Space[3]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_ComNum_Space[4]/*, wcslen(HWCfg_ComNum_Space[4]) * sizeof(wchar_t)*/);			
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"LED_NUM", SHORTLENGTH))
		{
//			sprintf(asc_str, "%d", hw_cfg_v->pan_info.num_com);
//			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->led_num >=0 && hw_cfg_v->led_num < 5)
			{
				memcpy(ItmAddr[Index].value, HWCfg_LedNum_Space[hw_cfg_v->led_num], wcslen(HWCfg_LedNum_Space[hw_cfg_v->led_num]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_LedNum_Space[0], wcslen(HWCfg_LedNum_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_LedNum_Space[0], wcslen(HWCfg_LedNum_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_LedNum_Space[1]/*, wcslen(HWCfg_LedNum_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_LedNum_Space[2]/*, wcslen(HWCfg_LedNum_Space[2]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_LedNum_Space[3]/*, wcslen(HWCfg_LedNum_Space[3]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_LedNum_Space[4]/*, wcslen(HWCfg_LedNum_Space[4]) * sizeof(wchar_t)*/);			
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COLON_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.pos_colon);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}		
		else if(0 == wcsncmp(HWCfgSpace[Index], L"STATUS", SHORTLENGTH))
		{
//			sprintf(asc_str, "%d", hw_cfg_v->pan_info.status);
//			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.status >=0 && hw_cfg_v->pan_info.status < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Status_Space[hw_cfg_v->pan_info.status], wcslen(HWCfg_Status_Space[hw_cfg_v->pan_info.status]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Status_Space[1], wcslen(HWCfg_Status_Space[1]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Status_Space[0], wcslen(HWCfg_Status_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Status_Space[1]/*, wcslen(HWCfg_Status_Space[1]) * sizeof(wchar_t)*/);		
		}	
		else if(0 == wcsncmp(HWCfgSpace[Index], L"CLOCK_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.fclock.polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.fclock.polar >=0 && hw_cfg_v->pan_info.fclock.polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_ClockLevel_Space[hw_cfg_v->pan_info.fclock.polar], wcslen(HWCfg_ClockLevel_Space[hw_cfg_v->pan_info.fclock.polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_ClockLevel_Space[0], wcslen(HWCfg_ClockLevel_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_ClockLevel_Space[0], wcslen(HWCfg_ClockLevel_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_ClockLevel_Space[1]/*, wcslen(HWCfg_ClockLevel_Space[1]) * sizeof(wchar_t)*/);		
		}	
		else if(0 == wcsncmp(HWCfgSpace[Index], L"CLOCK_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.fclock.pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"DATA_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.fdata.polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.fdata.polar >=0 && hw_cfg_v->pan_info.fdata.polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_DataLevel_Space[hw_cfg_v->pan_info.fdata.polar], wcslen(HWCfg_DataLevel_Space[hw_cfg_v->pan_info.fdata.polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_DataLevel_Space[0], wcslen(HWCfg_DataLevel_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_DataLevel_Space[0], wcslen(HWCfg_DataLevel_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_DataLevel_Space[1]/*, wcslen(HWCfg_DataLevel_Space[1]) * sizeof(wchar_t)*/);			
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"DATA_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.fdata.pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"LBD_TYPE", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.type_lbd);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.type_lbd >=0 && hw_cfg_v->pan_info.type_lbd < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_LBDType_Space[hw_cfg_v->pan_info.type_lbd], wcslen(HWCfg_LBDType_Space[hw_cfg_v->pan_info.type_lbd]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_LBDType_Space[0], wcslen(HWCfg_LBDType_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_LBDType_Space[0], wcslen(HWCfg_LBDType_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_LBDType_Space[1]/*, wcslen(HWCfg_LBDType_Space[1]) * sizeof(wchar_t)*/);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"POWER_LAMP_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.lbd[0].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}		
		else if(0 == wcsncmp(HWCfgSpace[Index], L"POWER_LAMP_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.lbd[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.lbd[0].polar >=0 && hw_cfg_v->pan_info.lbd[0].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_LockLampLevel_Space[hw_cfg_v->pan_info.lbd[0].polar], wcslen(HWCfg_LockLampLevel_Space[hw_cfg_v->pan_info.lbd[0].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_LockLampLevel_Space[0], wcslen(HWCfg_LockLampLevel_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_LockLampLevel_Space[0], wcslen(HWCfg_LockLampLevel_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_LockLampLevel_Space[1]/*, wcslen(HWCfg_LockLampLevel_Space[1]) * sizeof(wchar_t)*/);			
		}	
		else if(0 == wcsncmp(HWCfgSpace[Index], L"LOCK_LAMP_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.lbd[1].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}	
		else if(0 == wcsncmp(HWCfgSpace[Index], L"LOCK_LAMP_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.lbd[1].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.lbd[1].polar >=0 && hw_cfg_v->pan_info.lbd[1].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_PowerLampLevel_Space[hw_cfg_v->pan_info.lbd[1].polar], wcslen(HWCfg_PowerLampLevel_Space[hw_cfg_v->pan_info.lbd[1].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_PowerLampLevel_Space[0], wcslen(HWCfg_PowerLampLevel_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_PowerLampLevel_Space[0], wcslen(HWCfg_PowerLampLevel_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_PowerLampLevel_Space[1]/*, wcslen(HWCfg_PowerLampLevel_Space[1]) * sizeof(wchar_t)*/);			
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"RECORD_LAMP_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.lbd[2].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}		
		else if(0 == wcsncmp(HWCfgSpace[Index], L"RECORD_LAMP_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.lbd[2].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.lbd[2].polar >=0 && hw_cfg_v->pan_info.lbd[2].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_RecordLampLevel_Space[hw_cfg_v->pan_info.lbd[2].polar], wcslen(HWCfg_RecordLampLevel_Space[hw_cfg_v->pan_info.lbd[2].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_RecordLampLevel_Space[0], wcslen(HWCfg_RecordLampLevel_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_RecordLampLevel_Space[0], wcslen(HWCfg_RecordLampLevel_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_RecordLampLevel_Space[1]/*, wcslen(HWCfg_LockLampLevel_Space[1]) * sizeof(wchar_t)*/);			
		}	
		else if(0 == wcsncmp(HWCfgSpace[Index], L"SCAN_TYPE", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.type_scan);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.type_scan >=0 && hw_cfg_v->pan_info.type_scan < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_ScanType_Space[hw_cfg_v->pan_info.type_scan], wcslen(HWCfg_ScanType_Space[hw_cfg_v->pan_info.type_scan]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_ScanType_Space[0], wcslen(HWCfg_ScanType_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_ScanType_Space[0], wcslen(HWCfg_ScanType_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_ScanType_Space[1]/*, wcslen(HWCfg_ScanType_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"SCAN_0_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"SCAN_0_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.scan[0].polar >=0 && hw_cfg_v->pan_info.scan[0].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Scan0Level_Space[hw_cfg_v->pan_info.scan[0].polar], wcslen(HWCfg_Scan0Level_Space[hw_cfg_v->pan_info.scan[0].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Scan0Level_Space[0], wcslen(HWCfg_Scan0Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Scan0Level_Space[0], wcslen(HWCfg_Scan0Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Scan0Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_0_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[0].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_0_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[0].polar >=0 && hw_cfg_v->pan_info.com[0].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com0Level_Space[hw_cfg_v->pan_info.com[0].polar], wcslen(HWCfg_Com0Level_Space[hw_cfg_v->pan_info.com[0].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com0Level_Space[0], wcslen(HWCfg_Com0Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com0Level_Space[0], wcslen(HWCfg_Com0Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com0Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_1_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[1].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_1_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[1].polar >=0 && hw_cfg_v->pan_info.com[1].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com1Level_Space[hw_cfg_v->pan_info.com[1].polar], wcslen(HWCfg_Com1Level_Space[hw_cfg_v->pan_info.com[1].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com1Level_Space[0], wcslen(HWCfg_Com1Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com1Level_Space[0], wcslen(HWCfg_Com1Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com1Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_2_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[2].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_2_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[2].polar >=0 && hw_cfg_v->pan_info.com[2].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com2Level_Space[hw_cfg_v->pan_info.com[2].polar], wcslen(HWCfg_Com2Level_Space[hw_cfg_v->pan_info.com[2].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com2Level_Space[0], wcslen(HWCfg_Com2Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com2Level_Space[0], wcslen(HWCfg_Com2Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com2Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_3_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[3].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_3_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[3].polar >=0 && hw_cfg_v->pan_info.com[3].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com3Level_Space[hw_cfg_v->pan_info.com[3].polar], wcslen(HWCfg_Com3Level_Space[hw_cfg_v->pan_info.com[3].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com3Level_Space[0], wcslen(HWCfg_Com3Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com3Level_Space[0], wcslen(HWCfg_Com3Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com3Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_4_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[4].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_4_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[4].polar >=0 && hw_cfg_v->pan_info.com[4].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com4Level_Space[hw_cfg_v->pan_info.com[4].polar], wcslen(HWCfg_Com4Level_Space[hw_cfg_v->pan_info.com[4].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com4Level_Space[0], wcslen(HWCfg_Com0Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com4Level_Space[0], wcslen(HWCfg_Com4Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com4Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_5_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[5].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_5_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[5].polar >=0 && hw_cfg_v->pan_info.com[5].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com5Level_Space[hw_cfg_v->pan_info.com[5].polar], wcslen(HWCfg_Com5Level_Space[hw_cfg_v->pan_info.com[5].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com5Level_Space[0], wcslen(HWCfg_Com5Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com5Level_Space[0], wcslen(HWCfg_Com5Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com5Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_6_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[6].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_6_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[6].polar >=0 && hw_cfg_v->pan_info.com[6].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com6Level_Space[hw_cfg_v->pan_info.com[6].polar], wcslen(HWCfg_Com6Level_Space[hw_cfg_v->pan_info.com[6].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com6Level_Space[0], wcslen(HWCfg_Com6Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com6Level_Space[0], wcslen(HWCfg_Com6Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com6Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_7_POS", SHORTLENGTH))
		{
			sprintf(asc_str, "%d", hw_cfg_v->pan_info.com[7].pos);
			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"COM_7_LEVEL", SHORTLENGTH))
		{
		//	sprintf(asc_str, "%d", hw_cfg_v->pan_info.scan[0].polar);
		//	mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->pan_info.com[7].polar >=0 && hw_cfg_v->pan_info.com[7].polar < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com7Level_Space[hw_cfg_v->pan_info.com[7].polar], wcslen(HWCfg_Com7Level_Space[hw_cfg_v->pan_info.com[7].polar]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_Com7Level_Space[0], wcslen(HWCfg_Com7Level_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_Com7Level_Space[0], wcslen(HWCfg_Com7Level_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_Com7Level_Space[1]/*, wcslen(HWCfg_Scan0Level_Space[1]) * sizeof(wchar_t)*/);				
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"DETECT_PIN_POL", SHORTLENGTH))
		{
//			sprintf(asc_str, "%d", hw_cfg_v->pan_info.num_com);
//			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->detect_pin_pol >=0 && hw_cfg_v->detect_pin_pol < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_DetectPinPol_Space[hw_cfg_v->detect_pin_pol], wcslen(HWCfg_DetectPinPol_Space[hw_cfg_v->detect_pin_pol]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_DetectPinPol_Space[0], wcslen(HWCfg_DetectPinPol_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_DetectPinPol_Space[0], wcslen(HWCfg_DetectPinPol_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_DetectPinPol_Space[1]/*, wcslen(HWCfg_LedNum_Space[1]) * sizeof(wchar_t)*/);	
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"VCC_ENABLE_POL", SHORTLENGTH))
		{
//			sprintf(asc_str, "%d", hw_cfg_v->pan_info.num_com);
//			mbstowcs(ItmAddr[Index].value, asc_str, 10);		
			if( hw_cfg_v->vcc_enable_pol >=0 && hw_cfg_v->vcc_enable_pol < 2)
			{
				memcpy(ItmAddr[Index].value, HWCfg_VccEnablePol_Space[hw_cfg_v->vcc_enable_pol], wcslen(HWCfg_VccEnablePol_Space[hw_cfg_v->vcc_enable_pol]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_VccEnablePol_Space[0], wcslen(HWCfg_VccEnablePol_Space[0]) * sizeof(wchar_t));
			}
	
			memcpy(ItmAddr[Index].options, HWCfg_VccEnablePol_Space[0], wcslen(HWCfg_VccEnablePol_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VccEnablePol_Space[1]/*, wcslen(HWCfg_LedNum_Space[1]) * sizeof(wchar_t)*/);	
		}
		else if(0 == wcsncmp(HWCfgSpace[Index], L"VDAC_INFO", SHORTLENGTH))
		{

			if( hw_cfg_v->vdac_info >=0 && hw_cfg_v->vdac_info < 10)
			{
				memcpy(ItmAddr[Index].value, HWCfg_VdacInfo_Space[hw_cfg_v->vdac_info], wcslen(HWCfg_VdacInfo_Space[hw_cfg_v->vdac_info]) * sizeof(wchar_t));
			}
			else
			{
				memcpy(ItmAddr[Index].value, HWCfg_VdacInfo_Space[0], wcslen(HWCfg_VdacInfo_Space[0]) * sizeof(wchar_t));
			}

			memcpy(ItmAddr[Index].options, HWCfg_VdacInfo_Space[0], wcslen(HWCfg_VdacInfo_Space[0]) * sizeof(wchar_t));
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[1]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[2]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[3]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[4]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[5]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[6]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[7]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[8]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, hwcfg_char/*, wcslen(hwcfg_char) * sizeof(wchar_t)*/);
			wcscat(ItmAddr[Index].options, HWCfg_VdacInfo_Space[9]/*, wcslen(HWCfg_FPType_Space[1]) * sizeof(wchar_t)*/);
		}
		else
		{
			return ; //error
		}
	
		ItmAddr[Index].m_validation = NULL;
	}

	DeBlockRead(&Block);
}


void SSdataCfgBlockProcess(NativeProperty_t *ItmAddr, wchar_t *GroupName, u8 BlockId, flash_file_t *file)
{
	wchar_t Index = 0;
	s8 Ret = 0;
	u8 *p_addr = NULL;
	block_buf_t Block;
	char asc_str[10];

	Ret = BlockRead(BlockId, file, &Block);
	if(Ret < 0)
	{
		abort();
	}

	p_addr = Block.buf;
	Index = 0;
//	for(Index = 0; Index < IrModuleNum; Index++)
//	{
		memcpy(ItmAddr[Index].group, GroupName, wcslen(GroupName) * sizeof(wchar_t));
		memcpy(ItmAddr[Index].key, GroupName, wcslen(GroupName) * sizeof(wchar_t));
		if(Index >= Block.size)
		{
			abort();
		}
		sprintf(asc_str, "%d", (u8)(*p_addr++));
		mbstowcs(ItmAddr[Index].value, asc_str, 10);
		ItmAddr[Index].m_validation = NULL;
//	}

	DeBlockRead(&Block);
}

//need to free **buf
s32 IrBlockMerge(NativeProperty_t *ItmAddr, block_buf_t *Block, u32 size)
{
  wchar_t Index = 0, Index_J = 0;
  NativeProperty_t TempItm;
  s32 Ret = -1;
  int Data = 0;
  u8 *p_addr = NULL;
  char asc_str[10];


  Block->size = size;
  

  if(size == 0 || ItmAddr == NULL)
  {
    return ERR_PARAM;
  }
  
//  Block->buf = mtos_malloc(size);
  p_addr = (u8 *)Block->buf;

  for(Index = 0; Index < size; Index++)
  {
    for(Index_J = Index; Index_J < size; Index_J ++)
    {
      if(0 == wcsncmp(IrNameSpace[Index], ItmAddr[Index_J].key, SHORTLENGTH))
      {
        memcpy(&TempItm, &ItmAddr[Index], sizeof(NativeProperty_t));
        memcpy(&ItmAddr[Index], &ItmAddr[Index_J], sizeof(NativeProperty_t));
        memcpy(&ItmAddr[Index_J], &TempItm, sizeof(NativeProperty_t));
        break;
      }
      else
      {
        if(Index_J == size - 1)
        {
          return ERR_FAILURE;//?????
        }
      }
    }
  }

  for(Index = 0; Index < size; Index++)
  {
    wcstombs(asc_str, ItmAddr[Index].value, 10);
    Data = atoi(asc_str);
    if(Data > 255 || Data < 0)
    {
      return ERR_FAILURE;
    }

    p_addr[Index] = (u8)Data;
  }

  return SUCCESS;
}

s32 FPBlockMerge(NativeProperty_t *ItmAddr, block_buf_t *Block, u32 size)
{
  wchar_t Index = 0, Index_J = 0;
  NativeProperty_t TempItm;
  s32 Ret = -1;
  int Data = 0;
  u8 *p_addr = NULL;
  char asc_str[10];


  Block->size = size;
  

  if(size == 0 || ItmAddr == NULL)
  {
    return ERR_PARAM;
  }
  
//  Block->buf = mtos_malloc(size);
  p_addr = (u8 *)Block->buf;

  for(Index = 0; Index < size; Index++)
  {
    for(Index_J = Index; Index_J < size; Index_J ++)
    {
      if(0 == wcsncmp(FpNameSpace[Index], ItmAddr[Index_J].key, SHORTLENGTH))
      {
        memcpy(&TempItm, &ItmAddr[Index], sizeof(NativeProperty_t));
        memcpy(&ItmAddr[Index], &ItmAddr[Index_J], sizeof(NativeProperty_t));
        memcpy(&ItmAddr[Index_J], &TempItm, sizeof(NativeProperty_t));
        break;
      }
      else
      {
        if(Index_J == size - 1)
        {
          return ERR_FAILURE;//?????
        }
      }
    }
  }

  for(Index = 0; Index < size; Index++)
  {
    wcstombs(asc_str, ItmAddr[Index].value, 10);
    Data = atoi(asc_str);
    if(Data > 255 || Data < 0)
    {
      return ERR_FAILURE;
    }

    p_addr[Index] = (u8)Data;
  }

  return SUCCESS;
}
s32 HWCfgBlockMerge(NativeProperty_t *ItmAddr, block_buf_t *Block, u32 size)
{
  wchar_t Index = 0, Index_J = 0;
  NativeProperty_t TempItm;
  s32 Ret = -1;
  int Data = 0;
  u8 *p_addr = NULL;
  char asc_str[10];
	hw_cfg_t *hw_cfg_v = NULL;

	if(size == 0 || ItmAddr == NULL)
	{
		return ERR_PARAM;
	}
	
	Ret = BlockRead(0xBC, &FlashFile, Block);
	if(Ret < 0)
	{
		abort();
	}

//	Block->size = size;
	p_addr = (u8 *)Block->buf;

  for(Index = 0; Index < size; Index++)
  {
    for(Index_J = Index; Index_J < size; Index_J ++)
    {
      if(0 == wcsncmp(HWCfgSpace[Index], ItmAddr[Index_J].key, SHORTLENGTH))
      {
        memcpy(&TempItm, &ItmAddr[Index], sizeof(NativeProperty_t));
        memcpy(&ItmAddr[Index], &ItmAddr[Index_J], sizeof(NativeProperty_t));
        memcpy(&ItmAddr[Index_J], &TempItm, sizeof(NativeProperty_t));
        break;
      }
      else
      {
        if(Index_J == size - 1)
        {
          return ERR_FAILURE;//?????
        }
      }
    }
  }
	hw_cfg_v = (hw_cfg_t *)p_addr;
	//transfer and save the coorelative block raw data
	#if 1 //hw cfg raw data
	{
		for(Index = 0; Index < size; Index ++)
		{
			if(0 == wcsncmp(ItmAddr[Index].key, L"FP_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->fp_type = Data;*/

				for(Index_J = 0; Index_J < FpTypeNum; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_FPType_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_NUM", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.num_com = (u8)Data;*/	
				for(Index_J = 0; Index_J < 5; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_ComNum_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"LED_NUM", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->led_num = (u8)Data;*/	
				for(Index_J = 0; Index_J < 5; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_LedNum_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COLON_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.pos_colon = (u8)Data;	
			}		
			else if(0 == wcsncmp(ItmAddr[Index].key, L"STATUS", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.status = (u8)Data;	*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Status_Space[Index_J], SHORTLENGTH))
					{
						break;
					}
				}
				if(2 == Index_J)
				{
					Index_J = 0;
				}
				//not change default status value
			//	hw_cfg_v->pan_info.status = Index_J;					
			}	
			else if(0 == wcsncmp(ItmAddr[Index].key, L"CLOCK_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.fclock.polar = (u8)Data;*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_ClockLevel_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"CLOCK_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.fclock.pos = (u8)Data;		
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"DATA_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.fdata.polar = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_DataLevel_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"DATA_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.fdata.pos = (u8)Data;		
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"LBD_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.type_lbd = (u8)Data;		*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_LBDType_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"POWER_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.lbd[0].pos = (u8)Data;		
			}		
			else if(0 == wcsncmp(ItmAddr[Index].key, L"POWER_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.lbd[0].polar = (u8)Data;	*/
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_LockLampLevel_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"LOCK_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.lbd[1].pos = (u8)Data;		
			}	
			else if(0 == wcsncmp(ItmAddr[Index].key, L"LOCK_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.lbd[1].polar = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_PowerLampLevel_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"RECORD_LAMP_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.lbd[2].pos = (u8)Data;		
			}	
			else if(0 == wcsncmp(ItmAddr[Index].key, L"RECORD_LAMP_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.lbd[1].polar = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_RecordLampLevel_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"SCAN_TYPE", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.type_scan = (u8)Data;	*/	
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_ScanType_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"SCAN_0_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"SCAN_0_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Scan0Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_0_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[0].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_0_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com0Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_1_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[1].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_1_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com1Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_2_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[2].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_2_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com2Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_3_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[3].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_3_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com3Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_4_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[4].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_4_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com4Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_5_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[5].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_5_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com5Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_6_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[6].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_6_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com6Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_7_POS", SHORTLENGTH))
			{
				wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.com[7].pos = (u8)Data;			
			}
			else if(0 == wcsncmp(ItmAddr[Index].key, L"COM_7_LEVEL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_Com7Level_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"DETECT_PIN_POL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_DetectPinPol_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"VCC_ENABLE_POL", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 2; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_VccEnablePol_Space[Index_J], SHORTLENGTH))
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
			else if(0 == wcsncmp(ItmAddr[Index].key, L"VDAC_INFO", SHORTLENGTH))
			{
			/*	wcstombs(asc_str, ItmAddr[Index].value, 10);
				Data = atoi(asc_str);
				if(Data > 255 || Data < 0)
				{
					return ERR_FAILURE;//error
				}
				hw_cfg_v->pan_info.scan[0].polar = (u8)Data;	*/		
				for(Index_J = 0; Index_J < 10; Index_J++)
				{
					if(0 == wcsncmp(ItmAddr[Index].value, HWCfg_VdacInfo_Space[Index_J], SHORTLENGTH))
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
	#else
	{
		for(Index = 0; Index < size; Index++)
		{
			wcstombs(asc_str, ItmAddr[Index].value, 10);
			Data = atoi(asc_str);
			if(Data > 255 || Data < 0)
			{
				return ERR_FAILURE;
			}

			p_addr[Index] = (u8)Data;
		}
	}
	#endif
  return SUCCESS;

}unsigned int HwCfgBlockQueryValue(wchar_t* key,wchar_t* str_value)
{
	wchar_t Index = 0;
	if(hw_cfg == NULL)
	{
		return 0xffff;
	}
	else
	{
		if(0 == wcsncmp(key, L"FP_TYPE", SHORTLENGTH))
		{
			for(Index = 0; Index < FpTypeNum; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_FPType_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}
		}
		else if(0 == wcsncmp(key, L"COM_NUM", SHORTLENGTH))
		{
			for(Index = 0; Index < 5; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_ComNum_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}
		}	
		else if(0 == wcsncmp(key, L"LED_NUM", SHORTLENGTH))
		{
			for(Index = 0; Index < 5; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_LedNum_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}
		}	
		else if(0 == wcsncmp(key, L"STATUS", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Status_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}	
		else if(0 == wcsncmp(key, L"CLOCK_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_ClockLevel_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}	
		else if(0 == wcsncmp(key, L"DATA_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_DataLevel_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}
		else if(0 == wcsncmp(key, L"LBD_TYPE", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_LBDType_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}
		else if(0 == wcsncmp(key, L"LOCK_LAMP_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_LockLampLevel_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}	
		else if(0 == wcsncmp(key, L"POWER_LAMP_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_PowerLampLevel_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}
		else if(0 == wcsncmp(key, L"RECORD_LAMP_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_RecordLampLevel_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}
		else if(0 == wcsncmp(key, L"SCAN_TYPE", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_ScanType_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}
		else if(0 == wcsncmp(key, L"SCAN_0_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Scan0Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"SCAN_TYPE", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_ScanType_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}
		else if(0 == wcsncmp(key, L"SCAN_0_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Scan0Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"SCAN_TYPE", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_ScanType_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}	
		}
		else if(0 == wcsncmp(key, L"SCAN_0_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Scan0Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_0_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com0Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_1_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com1Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_2_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com2Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_3_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com3Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_4_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com4Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_5_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com5Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_6_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com6Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"COM_7_LEVEL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_Com7Level_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"DETECT_PIN_POL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_DetectPinPol_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"VCC_ENABLE_POL", SHORTLENGTH))
		{
			for(Index = 0; Index < 2; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_VccEnablePol_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else if(0 == wcsncmp(key, L"VDAC_INFO", SHORTLENGTH))
		{
			for(Index = 0; Index < 10; Index++)
			{
				if(0 == wcsncmp(str_value, HWCfg_VdacInfo_Space[Index], SHORTLENGTH))
				{
					return Index;
				}
			}			
		}
		else
		{
			return 0xffff;
		}
	}
	return 0xffff;
}
