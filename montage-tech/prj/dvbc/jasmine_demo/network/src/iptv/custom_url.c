/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"

#include "class_factory.h"
#include "data_manager.h"

#include "custom_url.h"
#include "sys_cfg.h"

#define CUSTOM_URL_BLOCK_ID  CUS_URL_BLOCK_ID

//static u8 buf[CUSTOM_URL_BLOCK_SIZE];

u32 read_custom_url(u8 *buf, u32 buf_size)
{
  size_t rd;
 //static custom_url_t customs_url_tbl[MAX_CUSTOM_URL];

  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  rd = dm_direct_read(p_dm_handle, CUSTOM_URL_BLOCK_ID, 0, CUSTOM_URL_BLOCK_SIZE, buf);
  rd-=1;
  while(rd != 0 &&0xff == buf[rd])rd--;

  return rd;
}


u16 load_custom_url(custom_url_t** pp_url_array,u8 *buf,custom_url_t *customs_url_tbl)
{
  u8 i = 0;
  u8 j = 0;
  u16 k = 0;
  size_t rd;
 //static custom_url_t customs_url_tbl[MAX_CUSTOM_URL];

  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  rd = dm_direct_read(p_dm_handle, CUSTOM_URL_BLOCK_ID, 0, CUSTOM_URL_BLOCK_SIZE, buf);
  rd-=1;
  while(rd != 0 &&0xff == buf[rd])rd--;
  if(0==rd)
    return 0;
  buf[++rd] = '\0';

  if(rd>0)
  {
    u8* p_line = NULL;
    while(k < MAX_CUSTOM_URL && (p_line = (u8*)strtok(p_line?NULL:(char*)buf, "\r\n")))
    {
      i = 0;
      j = 0;
      memset(customs_url_tbl[k].name, 0, sizeof(customs_url_tbl[k].name));
      memset(customs_url_tbl[k].url, 0, sizeof(customs_url_tbl[k].url));
      while((',' != (customs_url_tbl[k].name[i++] = *p_line++))&&(i<CUS_NAME_LEN))continue;
      if(CUS_NAME_LEN== i)
      {
        while(*p_line&&','!=*p_line)p_line++;
        p_line++; //skip ','
        customs_url_tbl[k].name[CUS_NAME_LEN-1] = '\0';
      }
      else
      {
        customs_url_tbl[k].name[i-1] = '\0';
      }
      while(' ' == *p_line)p_line++;  //skip white spaces
      //lint -e650
      while(((customs_url_tbl[k].url[j++] = *p_line++))&&(j<CUS_URL_LEN))continue;
      if(CUS_URL_LEN == j)
      {
        //lint +e650
        while(*p_line)p_line++;
        customs_url_tbl[k].url[CUS_URL_LEN-1] = '\0';
      }
     // OS_PRINTF("name:%s", customs_url_tbl[k].name);
     // OS_PRINTF("url:%s\n", customs_url_tbl[k].url);
      k++;
    }
  }

  *pp_url_array = customs_url_tbl;
  return k;
}


void save_custom_url(u8* p_data, u32 len)
{
  u32 url_len = len;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  if(len > CUSTOM_URL_BLOCK_SIZE-1)
  {
    url_len = CUSTOM_URL_BLOCK_SIZE - 1; 
  }
  dm_direct_erase(p_dm_handle, CUSTOM_URL_BLOCK_ID, 0,CUSTOM_URL_BLOCK_SIZE-1);
  MT_ASSERT(DM_SUC == dm_direct_write(p_dm_handle, CUSTOM_URL_BLOCK_ID, 0, url_len, p_data));

  //lint -e438 -e550
}
//lint +e438 +e550

