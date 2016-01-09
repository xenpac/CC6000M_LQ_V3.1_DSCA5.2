/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "mem_manager.h"
#include "class_factory.h"

#include "mtos_mem.h"
#include "mtos_printk.h"

#include "char_map.h"
#include "big5.h"
#include "err_check_def.h"


void link_big5_maptab(char_map_t *p_map, u32 cnt)
{
  char_map_info_t *p_info = NULL;

  p_info = (char_map_info_t *)mtos_malloc(sizeof(char_map_info_t));
  CHECK_FAIL_RET_VOID(p_info != NULL);

  class_register(BIG5_CLASS_ID, (class_handle_t)(p_info));

  p_info->p_table = p_map;
  p_info->count = cnt;
}

void unlink_big5_maptab(void)
{
  char_map_info_t *p_info = NULL;

  p_info = (char_map_info_t *)class_get_handle_by_id(BIG5_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_info != NULL);

  mtos_free(p_info);
}


void gb2312_to_big5(u8 *p_gb2312, s32 length, u8 *p_big5, s32 maxlen)
{
  char_map_info_t *p_info = NULL;
  s32 si = 0, di = 0;
  u16 gb = 0;
  u16 temp_big5 = 0;

  CHECK_FAIL_RET_VOID(NULL != p_gb2312
           && 0 != length
           && NULL != p_big5
           && 0 != maxlen);

  p_info = (char_map_info_t *)class_get_handle_by_id(BIG5_CLASS_ID);//..
  CHECK_FAIL_RET_VOID(p_info != NULL);

  while(si < length && di < (maxlen - 2))
  {
    if((p_gb2312[si + 1] >= 0xa1) && (p_gb2312[si + 1] <= 0xfe)
      && (p_gb2312[si] >= 0xb0) && (p_gb2312[si] <= 0xd7))
    {
      gb = (u16)((p_gb2312[si] << 8) | p_gb2312[si + 1]);
      temp_big5 = map_lookup(gb, p_info->p_table, p_info->count);
      if(temp_big5 == 0xffff)
      {
        /* if not found, blank */
        p_big5[si] = 0x00;
        p_big5[si + 1] = 0x20;
      }
      else
      {
        p_big5[si] = (u8)(temp_big5 >> 8);
        p_big5[si + 1] = (u8)(temp_big5 & 0x00ff);
      }
    }
    else
    {
      p_big5[si] = 0x00;
      p_big5[si + 1] = 0x20;
    }
    si = si + 2;
    di = di + 2;
  }
  p_big5[di] = 0x00;//over
  p_big5[di + 1] = 0x00;
}

void big5_to_unicode(u8 *p_big5, s32 length, u16 *p_unicode, s32 maxlen)
{
  s32 si = 0, di = 0;
  char_map_info_t *p_info = NULL;
  u16 temp_unicode = 0;
  u16 big = 0;

  CHECK_FAIL_RET_VOID(NULL != p_big5
           && 0 != length
           && NULL != p_unicode
           && 0 != maxlen);

  p_info = (char_map_info_t *)class_get_handle_by_id(BIG5_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_info != NULL);

  if(p_big5[si] == 0x28 && p_big5[si + 3] == 0x29)
  {
    if(p_big5[si + 1] == 0x4d && p_big5[si + 2] == 0x44)
    {
      si = si + 4;
      while(p_big5[si]!=0x2f)
      {
        si++;
      }
      si++;
    }
    else
    {
      si = si + 4;
    }
  }
  else if(p_big5[si] == 0x28 && p_big5[si + 4] == 0x29)
  {
    si = si + 5;
  }

  while(si < length && di < (maxlen -1))
  {
    if(p_big5[si] >= 0xa1 && p_big5[si] <= 0xfe)
    {
      if((p_big5[si + 1] >= 0x40 && p_big5[si + 1] <= 0x7e)
        || (p_big5[si + 1] >= 0xa1 && p_big5[si + 1] <= 0xfe))
      {
        big = (u16)((p_big5[si] << 8) | p_big5[si + 1]);
        temp_unicode = map_lookup(big, p_info->p_table, p_info->count);
        if(0xffff == temp_unicode)
        {
          si += 2;
        }
        else
        {
          if(temp_unicode != 0x0000)
          {
            p_unicode[di] = temp_unicode;
            di++;
          }
          si += 2;
        }
      }
      else
      {
        p_unicode[di] = (u16)p_big5[si];
        di++;
        si += 1;
      }
    }
    else
    {
      p_unicode[di] = (u16)p_big5[si];
      di++;
      si++;
    }
  }
  p_unicode[di] = 0x0000;
}


