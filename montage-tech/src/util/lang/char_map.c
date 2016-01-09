/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "sys_types.h"
#include "sys_define.h"

#include "char_map.h"

u16 map_lookup(u16 key, const char_map_t *p_table, s32 count)
{
  u16 ret = 0xFFFF;
  s32 mid = 0;
  s32 low = -1;
  s32 high = count;

  if(p_table == NULL)
  {
    return ret;
  }
  do
  {
    mid = (low + high) >> 1;
    if(p_table[mid].key > key)
    {
      high = mid;
    }
    else if(p_table[mid].key < key)
    {
      low = mid;
    }
    else
    {
      ret = p_table[mid].value;
      break;
    }
  }
  while(high - low > 1);

  return ret;
}


u16 reverse_lookup(u16 value, const char_map_t *p_table, s32 count)
{
  s32 i = 0;
  u16 ret = 0xFFFF;

  if(p_table == NULL)
  {
    return ret;
  }

  for(i = 0; i < count; i++)
  {
    if(value == p_table[i].value)
    {
      ret = p_table[i].key;
      break;
    }
  }

  return ret;
}
