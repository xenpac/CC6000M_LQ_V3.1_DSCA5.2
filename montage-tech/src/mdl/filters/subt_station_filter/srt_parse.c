/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "ctype.h"
// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "common.h"
#include "drv_dev.h"
#include "lib_rect.h"
//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "transf_input_pin.h"
#include "transf_output_pin.h"
#include "common_filter.h"
#include "drv_misc.h"

#include "ifilter.h"
#include "transf_filter.h"

#include "subt_station_filter.h"
#include "subt_station_database.h"
#include "subt_station_parse.h"



/*!
  line_status_t
  */
typedef enum
{

  LINE_TYPE_INDEX,

  LINE_TYPE_TIME,

  LINE_TYPE_CONTENT
}line_status_t;

/*!
  LINE_MAX_SIZE
  */
#define LINE_MAX_SIZE (300)



static RET_CODE srt_parse_time(u8 *p_buf, u32 *p_ts_start, u32 *p_ts_end,
                           u32 *p_x1, u32 *p_y1, u32 *p_x2, u32 *p_y2)
{
  u32  hs = 0, ms = 0, ss = 0, he = 0, me = 0, se = 0;
  /* try to read timestamps in either the first or second line */
  u32 c = sscanf(p_buf, "%ld:%2ld:%2ld%*1[,.]%3ld --> %ld:%2ld:%2ld%*1[,.]%3ld"
                 "%*[ ]X1:%lu X2:%lu Y1:%lu Y2:%lu",
                 &hs, &ms, &ss, p_ts_start, &he, &me, &se, p_ts_end,
                 p_x1, p_x2, p_y1, p_y2);
  p_buf += strcspn(p_buf, "\n") + 1;
  if (c >= 8) 
  {
      *p_ts_start = 1000 * (ss + 60 * (ms + 60 * hs)) + *p_ts_start;
      *p_ts_end   = 1000 * (se + 60 * (me + 60 * he)) + *p_ts_end;
      return SUCCESS;
  }
  return ERR_FAILURE;
}

static RET_CODE srt_get_line(u8 *p_data, int *p_len, int *p_out_len)
{
  int ch = 0;
  int i = 0;
  s32 ret = 0;

  while(i < *p_len)
  {
    ch = p_data[i];
    if ('\n' == ch)      
      break;
    else if (EOF == ch)
    {
      ret = -2;
      break;
    }
    else
      i++;

    if(i >= LINE_MAX_SIZE)
    {
     // EPRINT(" line is too long!!");
      ret = -1;
      break;
    }
  }

  /*String maybe not end of "\n"*/
  if(i == *p_len)
  {
    *p_out_len = i;
  }
  else
  {
    *p_out_len = i + 1;
  }

  return ret;
}
static void srt_content_parse(u8 *p_in_data, u32 in_len, subt_station_content_t *p_subt)
{

  u8 *p_content = NULL;
  int ch = 0;
  u32 i = 0;
  u32 j = 0;
  BOOL b_skip = FALSE;

  p_content = mtos_malloc(in_len);
  if (p_content == NULL)
  {
    return;//error
  }

  memset(p_content, 0, in_len);

  while(i < in_len)
  {
    ch = p_in_data[i];
    if('<' == ch)
    {
      b_skip = TRUE;
    }
    if(!b_skip)
    {
      if((13 != ch) && (10 != ch))
      {     
        //p_content = strcat (p_content, p_in_data + i);
        //strncat (content, p_in_data + i, 1);
        p_content[j] = p_in_data[i];
        //memcpy (content[j], p_in_data + i, 1);
        j++;
      }
    }
    if('>' == ch)
    {
      b_skip = FALSE;
    }
    i++;
  }
  //memcpy(&p_subt->event[p_subt->line_num][0], p_in_data, in_len);
  /*j maybe more than the len of buffer, so cut it.*/
  memcpy(&p_subt->event[p_subt->line_num][0], 
         p_content, 
         MIN(j, SUBT_TEXT_MAX_LINE * SUBT_TEXT_LEN));
  p_subt->line_num ++;
  mtos_free(p_content);
}

/*!
  if the line of data is all blank string, return FALSE.
  */
static BOOL is_valid_content(u8 *p_data, u32 len)
{
   u32 i = 0;
   BOOL ret = TRUE;
   
   if(p_data[0] == 0x20)
   {
      for(i = 1; i < len; i ++)
      {
         if((p_data[i] == '\r') || (p_data[i] == '\n'))
         {         
            ret = FALSE;
            break;
         }
         
         if(p_data[i] != 0x20)
         {
            ret = TRUE;
            break;
         }
      }
      
   }
   
   return ret;
}

static RET_CODE srt_data_parse1(u8 *p_in_data, u32 in_len, subt_station_content_t *p_subt)
{
  int line_len = 0;
  int exist_len = (int)in_len;
  BOOL got_timestamp = FALSE;
  RET_CODE ret = ERR_FAILURE;
  u32 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
  u8 *p_read = p_in_data;
  
  while(exist_len >0)
  {
    
    p_read += line_len;
    srt_get_line(p_read, &exist_len, &line_len);
    exist_len -= line_len;
    //if the line of data is all blank string, break; 
    if((line_len <= 2) || (is_valid_content(p_read, line_len) == FALSE))
    {
      break;
    }

    if(!got_timestamp)
    {
      ret = srt_parse_time(p_read, &p_subt->start, &p_subt->end,
                           &x1, &y1, &x2, &y2);
      if(ret == SUCCESS)
      {
        got_timestamp = TRUE;
      }
      continue;
    }
    srt_content_parse(p_read, line_len, p_subt);
    
  }

  
  return SUCCESS;  
}

BOOL subt_station_srt_check(u8 *p_in_data, u32 in_len)
{
  u32  hs = 0, ms = 0, ss = 0, mms = 0, he = 0, me = 0, se = 0, ems = 0;
        /* try to read timestamps in either the first or second line */
  s32 c = sscanf(p_in_data, "%ld:%2ld:%2ld%*1[,.]%3ld --> %ld:%2ld:%2ld%*1[,.]%3ld",
                 &hs, &ms, &ss, &mms, &he, &me, &se, &ems);
  if (c >= 8) 
  {
      return TRUE;
  }
  return FALSE;
}

RET_CODE  subt_station_srt_parse(u8 *p_in_data, u32 in_len,
                subt_sys_type_t type, subt_station_content_t *p_subt)
{
  RET_CODE ret = ERR_FAILURE;

  if(type != SUBT_SYS_SRT) 
  {
    return ERR_FAILURE;
  }

  ret = srt_data_parse1(p_in_data, in_len, p_subt);
  
  return ret;
}


RET_CODE  subt_station_srt_scan(u8 *p_in_data, u32 in_len, subt_station_key_point_t *p_out)
{
  u32 hs = 0, ms = 0, ss = 0, mms = 0, he = 0, me = 0, se = 0, ems = 0;
        /* try to read timestamps in either the first or second line */
  s32 c = sscanf(p_in_data, "%ld:%2ld:%2ld%*1[,.]%3ld --> %ld:%2ld:%2ld%*1[,.]%3ld",
                 &hs, &ms, &ss, &mms, &he, &me, &se, &ems);
        
  if (c >= 8) 
  {
      p_out->start = 1000 * (ss + 60 * (ms + 60 * hs)) + mms;
      p_out->end = 1000 * (se + 60 * (me + 60 * he)) + ems;
      
      return SUCCESS;
  }
  return ERR_FAILURE;

}

