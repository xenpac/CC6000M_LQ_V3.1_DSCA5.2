/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"

#include "lib_unicode.h"
#include "err_check_def.h"

u32 uni_strlen(const u16 *p_str)
{
  const u16 *p_eos = p_str;

  while(*p_eos++ != 0)
  {
    ;
  }

  return (u32)(p_eos - p_str - 1);
}


void uni_strnset(u16 *p_str, const u16 val, u32 count)
{
  while(count != 0)
  {
    *p_str++ = val;
    count--;
  }

  *p_str = '\0';
}

s32 uni_strcmp(const u16 *p_dst, const u16 *p_src)
{
  s32 ret = 0;
  
  CHECK_FAIL_RET_ZERO(p_dst != NULL && p_src != NULL);

  while(!(ret = *(u16 *)p_src - *(u16 *)p_dst) && (*p_dst != 0))
  {
    ++p_src, ++p_dst;
  }

  if(ret < 0)
  {
    ret = -1;
  }
  else if(ret > 0)
  {
    ret = 1;
  }

  return ret;
}

s32 uni_strncmp(const u16 *p_dst, const u16 *p_src, u32 count)
{
  s32 ret = 0;

  CHECK_FAIL_RET_ZERO(p_dst != NULL && p_src != NULL);

  while((*p_dst != 0) && (count != 0) && !(ret = *(u16 *)p_src - *(u16 *)p_dst))
  {
    ++p_src, ++p_dst;

    count--;
  }

  if(ret < 0)
  {
    ret = -1;
  }
  else if(ret > 0)
  {
    ret = 1;
  }

  return ret;
}

void uni_strcpy(u16 *p_dst, const u16 *p_src)
{
  //CHECK_FAIL_RET_VOID(p_dst != NULL && p_src != NULL);
  if((NULL == p_dst) || (NULL == p_src))
  {
    return;
  }
  while((*p_dst++ = *p_src++) != 0)
  {
    ; /* copy p_src over p_dst */
  }
}


void uni_strncpy(u16 *p_dst, const u16 *p_src, u32 count)
{
  //CHECK_FAIL_RET_VOID(p_dst != NULL && p_src != NULL);
  if((NULL == p_dst) || (NULL == p_src))
  {
    return;
  }
  while((count != 0) && (*p_dst++ = *p_src++))
  {
    count--;
  }

  *p_dst = 0;
}


u16 *uni_strcat(u16 *p_dst, const u16 *p_src, u32 dst_len)
{
  u32 i = 0;
  u32 j = 0;

  CHECK_FAIL_RET_NULL(p_dst != NULL && p_src != NULL);

  while(p_dst[i] != 0)
  {
    i++;
  }

  for(j = 0; p_src[j] && (i < dst_len); j++, i++)
  {
    p_dst[i] = p_src[j];
  }
  p_dst[i] = 0;

  return p_dst;
}


u16 *uni_strrchr(u16 *p_str, u16 code)
{
  u16 str_len = 0, i = 0;
  
  if(p_str == NULL)
  {
    return NULL;
  }

  str_len = uni_strlen(p_str);

  for(i = 0; i < str_len; i++)
  {
    if(p_str[str_len - i - 1] == code)
    {
      return (p_str + str_len - i - 1);
    }
  }

  return NULL;
}

u16 *uni_strstr(u16 *p_str1, u16 *p_str2)
{
  u32 len2 = 0;
  
  if(!(len2 = uni_strlen(p_str2)))
  {
    return (u16 *)p_str1;
  }
  
  for(; *p_str1; ++p_str1)
  {
    if ((*p_str1 == *p_str2) && uni_strncmp(p_str1, p_str2, len2) == 0)
    {
      return (u16 *)p_str1;
    }
  }
  return NULL;
}

#define IN_RANGE(c, lo, up)  ((u16)c >= lo && (u16)c <= up)
#define ISLOWER(c)           IN_RANGE(c, 0x0061/*'a'*/, 0x007a/*'z'*/)

static inline u16 uni_toupper(u16 c)
{
  if (ISLOWER(c))
  {
    c -= 0x0061/*'a'*/ - 0x0041/*'A'*/;
  }
  return c;
}

u32 uni_strcmpi(u16 *p_str1, u16 *p_str2)
{
   while(uni_toupper(*p_str1) == uni_toupper(*p_str2++))
   {
     if (*p_str1++ == 0)
     {
      return 0;
     }
   }
   
   return(uni_toupper(*p_str1) - uni_toupper(*--p_str2));
}

u32 str_asc2uni(const u8 *p_ascstr, u16 *p_unistr)
{
  u32 i = 0;

  CHECK_FAIL_RET_ZERO(p_ascstr != NULL && p_unistr != NULL);

  while(p_ascstr[i] != 0)
  {
    p_unistr[i] = (u16)p_ascstr[i];
    i++;
  }
  p_unistr[i] = 0;

  return i;
}


u32 str_uni2asc(u8 *p_ascstr, const u16 *p_unistr)
{
  u32 i = 0;

  CHECK_FAIL_RET_ZERO(p_ascstr != NULL && p_unistr != NULL);

  while(p_unistr[i] != 0)
  {
    p_ascstr[i] = (u8)p_unistr[i];
    i++;
  }
  p_ascstr[i] = 0;

  return i;
}


u32 str_nasc2uni(const u8 *p_ascstr, u16 *p_unistr, u32 count)
{
  u32 ncpy = count;

  if((NULL == p_ascstr) || (NULL == p_unistr))
  {
    return 0;
  }
  while((count != 0) && (*p_unistr++ = (u16)(*p_ascstr++)))
  {
    count--; /* copy p_src over p_dst */
  }

  *p_unistr = 0;

  return ncpy - count;
}


u32 str_nuni2asc(u8 *p_ascstr, const u16 *p_unistr, u32 count)
{
  u32 ncpy = count;

  if((NULL == p_ascstr) || (NULL == p_unistr))
  {
    return 0;
  }
  while((count != 0) && (*p_ascstr++ = (u8)(*p_unistr++)))
  {
    count--; /* copy p_src over p_dst */
  }

  *p_ascstr = 0;

  return ncpy - count;
}


u16 char_asc2uni(u8 achar)
{
  return (u16)achar;
}


u8 char_uni2asc(u16 uchar)
{
  return (u8)uchar;
}


u16 swap_unichar_endian(u16 uchar)
{
  return ((uchar >> 8) & 0xFF) | ((uchar << 8) & 0xFF00);
}


void swap_unistr_endian(u16 *p_str)
{
  while(*p_str != 0)
  {
    *p_str = swap_unichar_endian(*p_str);
    p_str++;
  }
}
