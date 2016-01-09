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
#include "gb2312.h"
#include "err_check_def.h"

BOOL is_gb2313_non_standard(u8 *p_str, u32 length)
{
  s32 i = 0, len = 0;
  s32 times = 0;

  if((p_str == NULL) || (length == 0))
  {
    return FALSE;
  }

  len = length - 1;
  for(i = 0; i < len; i++)
  {
    if((p_str[i] >= 0xa1) && (p_str[i + 1] >= 0xa1))
    {
      times++;
    }
    if(times >= 2)
    {
      return TRUE;
    }
  }
  return FALSE;
}


void link_gb2312_maptab(char_map_t *p_map, u32 cnt)
{
  char_map_info_t *p_info = NULL;

  p_info = (char_map_info_t *)mtos_malloc(sizeof(char_map_info_t));
  CHECK_FAIL_RET_VOID(p_info != NULL);

  class_register(GB2312_CLASS_ID, (class_handle_t)(p_info));

  p_info->p_table = p_map;
  p_info->count = cnt;
}


void unlink_gb2312_maptab(void)
{
  char_map_info_t *p_info = NULL;

  p_info = (char_map_info_t *)class_get_handle_by_id(GB2312_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_info != NULL);

  mtos_free(p_info);
}


//return value : 0xFFFF is invalid value
u16 convert_gb2312(u16 gb2312)
{
  char_map_info_t *p_info = NULL;

  p_info = (char_map_info_t *)class_get_handle_by_id(GB2312_CLASS_ID);
  CHECK_FAIL_RET((p_info != NULL), 0xFFFF);

  if(p_info->count > 0)
  {
    return map_lookup(gb2312, p_info->p_table, p_info->count);
  }
  else
  {
    return 0xFFFF;
  }
}


void gb2312_to_unicode(u8 *p_gb2312, s32 length, u16 *p_unicode, s32 maxlen)
{
  u16 gb = 0;
  s32 si = 0, di = 0;
  char_map_info_t *p_info = NULL;

  CHECK_FAIL_RET_VOID(NULL != p_gb2312
           && 0 != length
           && NULL != p_unicode
           && 0 != maxlen);

  p_info = (char_map_info_t *)class_get_handle_by_id(GB2312_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_info != NULL);

  while(si < length && di < (maxlen -1))
  {
    if((p_gb2312[si] == 0x0d) && (p_gb2312[si + 1] == 0x0a))
    {
      p_unicode[di] = 0x000a;
      di++;
      si += 2;
    }
    else if(p_gb2312[si] < 0x80)
    {
      p_unicode[di] = (u16)p_gb2312[si];
      di++, si++;
    }
    else if((p_gb2312[si] == 0xE0) && (si < length - 1)
           && (p_gb2312[si + 1] >= 0x80) && (p_gb2312[si + 1] <= 0x9F))
    {
      si += 2; /* control code, ignore them now. */
    }
    else
    {
      if(si + 1 < length)
      {
        gb = (u16)((p_gb2312[si] << 8) | p_gb2312[si + 1]);
        if((p_unicode[di] =
              map_lookup(gb, p_info->p_table, p_info->count)) == 0xFFFF)
        {
          // if not found, blank
          p_unicode[di] = 0x0020;
        }
        di++, si += 2;
      }
      else
      {
        // skip a single byte
        si++;
      }
    }
  }
  p_unicode[di] = 0x0000;
}


void unicode_to_gb2312(u16 *p_unicode, s32 length, u8 *p_gb2312, s32 maxlen)
{
  u16 u_code = 0, g_code = 0;
  s32 si = 0, di = 0;
  char_map_info_t *p_info = NULL;

  CHECK_FAIL_RET_VOID(NULL != p_unicode
           && 0 != length
           && NULL != p_gb2312
           && 0 != maxlen);

  p_info = (char_map_info_t *)class_get_handle_by_id(GB2312_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_info != NULL);

  while(si < length && di < maxlen)
  {
    u_code = p_unicode[si];

    if(u_code < 0x80)
    {
      p_gb2312[di] = (u8)(u_code & 0xFF);
      si++, di++;
    }
    else
    {
      if(di + 1 < maxlen)
      {
        g_code = reverse_lookup(u_code, p_info->p_table, p_info->count);
        if(g_code != 0xFFFF)
        {
          p_gb2312[di] = g_code >> 8;
          p_gb2312[di + 1] = g_code & 0xFF;
          di += 2;
        }
        else
        {
          // if not found, blank
          p_gb2312[di] = 0x20;
          di++;
        }
        si++;
      }
      else
      {
        // skip a single byte
        si++;
      }
    }
  }
  p_gb2312[di] = 0x00;
}
