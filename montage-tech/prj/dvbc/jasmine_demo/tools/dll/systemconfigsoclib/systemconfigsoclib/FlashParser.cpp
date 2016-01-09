/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "stdafx.h"
//#include "ConfigTool.h"
//#include "ConfigToolDlg.h"
#include <stdio.h>
#include <string>

#include "FlashParser.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

FlashParser::FlashParser() : m_sBinFile("")
{
  m_pFlashData = NULL;
  m_nFlashSize = 0;

  crc_setup_fast_lut(m_crc_table);
}

FlashParser::~FlashParser()
{
  Clear();
}

void FlashParser::Clear()
{
  if (m_pFlashData != NULL)
  {
    delete [] m_pFlashData;
    m_pFlashData = NULL;
    m_nFlashSize = 0;
  }
  m_sBinFile = "";
}

BOOL FlashParser::LoadBinFile(const char *szBinFile)
{
  FILE *fp;
  m_sBinFile = szBinFile;
  fp = fopen(szBinFile, "rb");

  if (fp)
  {
    fseek(fp, 0, SEEK_END);
    m_nFlashSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    m_pFlashData = new char[m_nFlashSize];
    fread(m_pFlashData, 1, m_nFlashSize, fp);
    fclose(fp);
    return TRUE;
  }
  return FALSE;
}

BOOL FlashParser::LoadBinFile(const wchar_t *wszBinFile)
{
  HANDLE hFile = CreateFile(
    wszBinFile, 
    GENERIC_READ,
    0,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
    ); 

  if (hFile == INVALID_HANDLE_VALUE)
  {
    CloseHandle(hFile);
    return FALSE;
  }

  m_nFlashSize = GetFileSize(hFile, NULL);
  m_pFlashData = new char[m_nFlashSize];
  DWORD readsize;
  ReadFile(hFile, m_pFlashData, m_nFlashSize, &readsize, NULL);
  CloseHandle(hFile);
  return TRUE;
}

BOOL FlashParser::SaveBinFile(const char *szNewBinFile)
{
  if (m_pFlashData != NULL)
  {
    FILE *fp = fopen(szNewBinFile, "wb");
    if (fp != NULL)
    {
      fwrite(m_pFlashData, 1, m_nFlashSize, fp);
      fclose(fp);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL FlashParser::SaveBinFile(const wchar_t *wszNewBinFile)
{
  if (m_pFlashData == NULL)
    return FALSE;

  //HANDLE hFile = CreateFile(
  //  wszNewBinFile, 
  //  GENERIC_WRITE,
  //  0,
  //  NULL,
  //  CREATE_ALWAYS,
  //  FILE_ATTRIBUTE_NORMAL,
  //  NULL
  //  ); 
  //if (hFile == INVALID_HANDLE_VALUE)
  //{
  //  CloseHandle(hFile);
  //  return FALSE;
  //}
  //DWORD writesize;
  //WriteFile(hFile, m_pFlashData, m_nFlashSize, &writesize, NULL);
  //CloseHandle(hFile);
  char szNewBinFile[256*2];
  wcstombs(szNewBinFile, wszNewBinFile, 256);
     FILE *fp = fopen(szNewBinFile, "wb");
    if (fp != NULL)
    {
      fwrite(m_pFlashData, 1, m_nFlashSize, fp);
      fclose(fp);
      return TRUE;
    }
  return FALSE;
}

u32 FlashParser::find_dm_head_start(int block_id)
{
  int i = 0;
  u32 dm_head_start = 0;
  u32 init_start = (u32)m_pFlashData;
  dmh_block_info_t dest;

  for (i = 0; i < m_nFlashSize; i++)
  {
    u32 start_data1 = *((unsigned int *)init_start);
    u32 start_data2 = *((unsigned int *)(init_start +4));
    
    if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
    {  
      dm_head_start = init_start;

	  if(__get_dm_block_head_info(block_id, &dest, dm_head_start - (u32)m_pFlashData))
      {
        return (dm_head_start - (u32)m_pFlashData);
      }
    }
    init_start += 1;//0x10000;
  }

  return 0;
}

int FlashParser::__get_dm_block_head_info(int block_id, dmh_block_info_t *dest, u32 dm_head_start)
{
  s32 blk_num = 0;
  u32 first_block_addr = 0;
  s32 result = 0;
  
  int i = 0;
  u8 block_id_tmp = 0;

  first_block_addr = (u32)m_pFlashData + dm_head_start + DM_TAG_SIZE + DM_BASE_INFO_HEAD_LEN;
  blk_num = *(u16*)((u32)m_pFlashData + dm_head_start + DM_TAG_SIZE + 8);

  for (i = 0; i < blk_num; i++) 
  {
    block_id_tmp = *((u8 *)(first_block_addr));
    
    if (block_id_tmp == block_id)
    {
      result = 1;
      memcpy(dest, (u8 *)(first_block_addr), sizeof(dmh_block_info_t));

      dest->base_addr += dm_head_start + (u32)m_pFlashData;
    }
    
    first_block_addr += 48;
  }
  return result;
}

int FlashParser::get_dm_block_head_info(int block_id, dmh_block_info_t *dest)
{  
  s32 result = 0;
  unsigned int dm_head_start = find_dm_head_start(block_id);

  result = __get_dm_block_head_info(block_id, dest, dm_head_start);

  if (result)
    return result;

  return result;
}

int FlashParser::__update_dm_block_head_info(int block_id, u32 dm_head_start)
{
  s32 blk_num = 0;
  u32 first_block_addr = 0;
  s32 result = 0;
  dmh_block_info_t tmp_block_info;
  int i = 0;
  u8 block_id_tmp = 0;
  u32 tmp_addr;

  first_block_addr = (u32)m_pFlashData + dm_head_start + DM_TAG_SIZE + DM_BASE_INFO_HEAD_LEN;
  blk_num = *(u16*)((u32)m_pFlashData + dm_head_start + DM_TAG_SIZE + 8);

  for (i = 0; i < blk_num; i++) 
  {
    block_id_tmp = *((u8 *)(first_block_addr));
    
    if (block_id_tmp == block_id)
    {
      result = 1;
      memcpy((u8 *)&tmp_block_info, (u8 *)(first_block_addr), sizeof(dmh_block_info_t));
      if (dm_head_start == 0xc)
        tmp_addr = tmp_block_info.base_addr + 0xc + (u32)m_pFlashData;
      else
        tmp_addr = tmp_block_info.base_addr + dm_head_start + (u32)m_pFlashData;
      if (tmp_block_info.crc != 0x4352434E)
      {
        tmp_block_info.crc = crc_fast_calculate(m_crc_table,
          0xFFFFFFFF,  (u8 *)(tmp_addr), tmp_block_info.size);
        memcpy((u8 *)(first_block_addr), (u8 *)&tmp_block_info, sizeof(dmh_block_info_t));
      }
      break;
    }
    
    first_block_addr += 48;
  }
  return result;
}

int FlashParser::update_dm_block_head_info(int block_id)
{
  s32 result = 0;
  
  unsigned int dm_head_start = find_dm_head_start(block_id);
  result = __update_dm_block_head_info(block_id, dm_head_start);
  
  if (result)
    return result;
  
  return result;
}

BOOL FlashParser::GetSSData(char *szData, u32 size)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(SS_DATA_BLOCK_ID, &dest);

  if (size <= dest.size)
  {
    memcpy(szData, (u8 *)(dest.base_addr), size);
    return TRUE;
  }
  return FALSE;
}

BOOL FlashParser::GetIRData(char *szData, u32 size)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(IRKEY1_BLOCK_ID, &dest);

  if (size <= dest.size)
  {
    memcpy(szData, (u8 *)(dest.base_addr), size);
    return TRUE;
  }
  return FALSE;
}

BOOL FlashParser::GetMiscOptions(char *szData, u32 size)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(MISC_OPTION_BLOCK_ID, &dest);
  u32 crc = 0;

  crc = crc_fast_calculate(m_crc_table,
    0xFFFFFFFF,  (u8 *)(dest.base_addr), dest.size);

  if (crc != dest.crc)
  {
    return FALSE;
  }

  if (size < dest.size)
  {
    memcpy(szData, (u8 *)(dest.base_addr), size);
    return TRUE;
  }
  return FALSE;
}

// save ss_data block
BOOL FlashParser::SaveSSData(char *szData, u32 size)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(SS_DATA_BLOCK_ID, &dest);

  if (size <= dest.size)
  {
      memcpy((u8 *)(dest.base_addr), szData, size);
      return TRUE;
  }
  return FALSE;
}

// save ir_data block
BOOL FlashParser::SaveIRData(char *szData, u32 size)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(IRKEY1_BLOCK_ID, &dest);

  if (size <= dest.size)
  {
        memcpy((u8 *)(dest.base_addr), szData, size);
        return TRUE;
   }
  return FALSE;
}
// save misc_options block data
BOOL FlashParser::SaveMiscOptions(char *szData, u32 size)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(MISC_OPTION_BLOCK_ID, &dest);

  if (size <= dest.size)
  {
       memcpy((u8 *)(dest.base_addr), szData, size);
       update_dm_block_head_info(MISC_OPTION_BLOCK_ID);
       return TRUE;
  }
  return FALSE;
}

// save ss_data block
BOOL FlashParser::SingleSaveSSData(const wchar_t *wszNewBinFile)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(SS_DATA_BLOCK_ID, &dest);
  char szNewBinFile[256*2];
  wcstombs(szNewBinFile, wszNewBinFile, 256);

  FILE *fp = fopen(szNewBinFile, "wb");
  if (fp != NULL)
  {
    fwrite((u8 *)(dest.base_addr), 1, dest.size, fp);
    fclose(fp);
    return TRUE;
   }
  return FALSE;
}

// save ir_data block
BOOL FlashParser::SingleSaveIRData(const wchar_t *wszNewBinFile)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(IRKEY1_BLOCK_ID, &dest);
  char szNewBinFile[256*2];
  wcstombs(szNewBinFile, wszNewBinFile, 256);

  FILE *fp = fopen(szNewBinFile, "wb");
  if (fp != NULL)
  {
    fwrite((u8 *)(dest.base_addr), 1, dest.size, fp);
    fclose(fp);
    return TRUE;
  }
  return FALSE;
}
// save misc_options block data
BOOL FlashParser::SingleSaveMiscOptions(const wchar_t *wszNewBinFile)
{
  s32 result;
  dmh_block_info_t dest;
  result  = get_dm_block_head_info(MISC_OPTION_BLOCK_ID, &dest);
  char szNewBinFile[256*2];
  wcstombs(szNewBinFile, wszNewBinFile, 256);

  FILE *fp = fopen(szNewBinFile, "wb");
  if (fp != NULL)
  {
    fwrite((u8 *)(dest.base_addr), 1, dest.size, fp);
    fclose(fp);
    return TRUE;
  }
  
  return FALSE;
}

void FlashParser::crc_setup_fast_lut(DWORD *crc_lut)
{
	unsigned long count, crc;
	int i;

	for (count = 0; count <= 255; count++) 
	{
		crc = count;
		for (i = 0; i < 8; i++) 
		{
			if (crc & 1)          /*Lowest bit procedure*/
				crc = (crc >> 1) ^ 0xedb88320;
			else
				crc >>= 1;
		}
		crc_lut[count] = crc & 0xffffffff;  /*Get lower 32 bits FCS*/
	}
}

DWORD FlashParser::crc_fast_calculate(DWORD *crc_lut, DWORD crc, unsigned char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		crc = (crc_lut[(crc & 0xff) ^ buf[i]] ^ (crc >> 8)) & 0xffffffff;
	return crc;
}
