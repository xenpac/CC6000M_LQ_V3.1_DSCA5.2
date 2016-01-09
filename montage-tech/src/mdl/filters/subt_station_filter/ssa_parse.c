/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

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

#ifdef WIN32
/*!
  STRNICMP
  */
#define STRNICMP _strnicmp
#else 
/*!
  STRNICMP
  */
#define STRNICMP strncasecmp
#endif

/*!
  tag_line_type_spec
  */
typedef enum tag_line_type_spec
{
  LINE_SCRIPT_INFO = 0,
  LINE_TITLE,
  LINE_ORIGINAL_SCRIPT,
  LINE_ORIGINAL_TRANS,
  LINE_ORIGINAL_TIMING,
  LINE_ORIGINAL_POINT,
  LINE_SCRIPT_TYPE,
  LINE_COLLISIONS,
  LINE_RESX,
  LINE_RESY,
  LINE_TIMER,
  LINE_FORMAT,
  LINE_STYLE,
  LINE_EVENT,
  LINE_DIALOGUE,
  LINE_TOTAL_SPEC
}line_type_spec_t;

/*!
  tag_base_cmd_spec
  */
typedef enum tag_base_cmd_spec
{
  LINE_RETURN = 0,
  LINE_FORCE_RETURN,
  LINE_FORCE_SPACE,
  LINE_TOTAL_CMD
}line_cmd_t;


typedef struct {
    const char *p_name;
    int type;
} ssa_line_t;

static const ssa_line_t line_list[] = 
{
  { "[Script Info]",              LINE_SCRIPT_INFO},
  { "Title:",                        LINE_TITLE         },
  {"Original Script:",            LINE_ORIGINAL_SCRIPT},
  { "Original Translation:",     LINE_ORIGINAL_TRANS},
  { "Original Timing:",          LINE_ORIGINAL_TIMING},
  {"Synch Point:",              LINE_ORIGINAL_POINT},
  {"ScriptType:",               LINE_SCRIPT_TYPE},
  {"Collisions:",                 LINE_COLLISIONS},
  {"PlayResX:",                 LINE_RESX},
  {"PlayResY:",                 LINE_RESY},
  {"Timer:",                      LINE_TIMER},
  {"Format:",                     LINE_FORMAT},
  {"Style:",                      LINE_STYLE},
  {"[Events]",                  LINE_EVENT},
  {"Dialogue:",                 LINE_DIALOGUE},
};

static const ssa_line_t cmd_list[] = 
{
 { "n",              LINE_RETURN},
 { "N",              LINE_FORCE_RETURN},
 { "h",              LINE_FORCE_SPACE},
};

static inline int is_eol(char buf)
{
    return buf == '\r' || buf == '\n' || buf == 0;
}


static RET_CODE get_line_len(u8 *p_data, u32 *p_len)
{
  int ch = 0;
  u32 i = 0;
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

    if(i >= 256)
    {
     // EPRINT(" line is too long!!");
      ret = -1;
      break;
    }
  }

  if(i == *p_len)
  {
    return -3;
  }
  
  i ++;
  *p_len = i;


  return ret;
}


#ifdef SSA_DEBUG
static void dump_info(subt_station_content_t *p_subt)
{
  u8 i = 0;

  OS_PRINTF("start: %d\n", p_subt->start);
  OS_PRINTF("end: %d\n", p_subt->end);
  for(i = 0; i < p_subt->line_num; i ++)
  {
    OS_PRINTF("evt[%d]: %s\n", i, &p_subt->event[i][0]);
  }
}
#endif

static u8 ssa_get_line_type(u8 *p_line)
{
  u8 i = 0;
  
  for (i = 0; i < LINE_TOTAL_SPEC ; i ++)
  {
    if(0 == STRNICMP(line_list[i].p_name, p_line, strlen(line_list[i].p_name)))
    {
      //OS_PRINTF("get cmd %d\n", i);
      break;
    }
  }

  if (i >= LINE_TOTAL_SPEC)
  {
    return LINE_TOTAL_SPEC;
  }

  return i;
}

static u32 ssa_time_get(u8 *p_in)
{
  u32 out = 0;
  int c = 0, h = 0, m = 0, s = 0, cs = 0;
    if ((c = sscanf(p_in, "%d:%02d:%02d.%02d", &h, &m, &s, &cs)) == 4)
        out = 3600000 * h + 60000 * m + 1000 * s + cs * 10;

  //OS_PRINTF("time: %d:%d:%d.%d\n", h, m, s, cs);
  return out;
  }

static u8 cmd_parse(u8 *out_len, u8 *p_in)
{
  u8 i = 0;
  
  for (i = 0; i < LINE_TOTAL_CMD; i ++)
  {
    if(0 == STRNICMP(cmd_list[i].p_name, p_in, strlen(cmd_list[i].p_name)))
    {
      //OS_PRINTF("get cmd %d\n", i);
      break;
    }
  }

  if (i >= LINE_TOTAL_CMD)
  {
    *out_len = 0;
    return LINE_TOTAL_CMD;
  }
  else
  {
    *out_len = strlen(cmd_list[i].p_name);
  }

  return i;
}

static RET_CODE parse_dialogue(u8 *p_in_data, u32 in_len, BOOL is_scan, 
              subt_station_content_t *p_subt)
{
  u8 *p_read = p_in_data;
  s16 len = (s16)in_len;
  u8 cnt = 0;
  u16 read_len = 0;
  u8 *p_dest = 0;
  u8 line_num = 0;
  u8 cmd_len = 0;
  
  while(len > 0)
  {
    p_read = memchr(p_read, ',', in_len);
    if(p_read == NULL)
    {
      return SUCCESS;
    }
    p_read ++;
    read_len = (u32)p_read - (u32)p_in_data;
    len = in_len - read_len;
    cnt ++;
    if(cnt == 1) // start time
    {
      p_subt->start = ssa_time_get(p_read);
    }
    else if(cnt == 2) // end time
    {
      p_subt->end = ssa_time_get(p_read);
    }
    else if(cnt == 9)// text
    {
      if(is_scan)
      {
        len = 0;
        break;
      }
      p_dest = &p_subt->event[line_num][0];
      while(!is_eol(*p_read) && len > 0)
      {
        //skip {}
        if(*p_read == '{')//cmd with {}
        {
          while(*p_read != '}' && (!is_eol(*p_read)))
          {
            p_read ++;
            ///TODO  cmd_parse
          }
          p_read ++; //ignore the }
        }

        //skip "\"
        if(*p_read == '\\')
        {//got cmd, parse it, and skip the cmd len
          p_read ++;
          if(LINE_TOTAL_CMD != cmd_parse(&cmd_len,p_read))
          {
            p_read += cmd_len;
          }
          *p_dest = 0;
          line_num ++;
          if(line_num == SUBT_TEXT_MAX_LINE)
          {
            len = 0;
            break;
          }
          p_dest = &p_subt->event[line_num][0];
          continue;//back to top, maybe there is a { }
        }

        //copy data
        if(is_eol(*p_read))
        {
          break;
        }
        *p_dest = *p_read;
        p_dest ++;
        p_read ++;
      }
      *p_dest = 0;
      p_subt->line_num = line_num + 1;
      break;
    }
  }
#ifdef SSA_DEBUG  
  dump_info(p_subt);
#endif
  return SUCCESS;
}

static RET_CODE ssa_data_parse(u8 *p_in_data, u32 in_len, BOOL is_scan,
              subt_station_content_t *p_subt)
{
  u8 type = 0;
  RET_CODE ret = ERR_FAILURE;
  
  type = ssa_get_line_type(p_in_data);

  switch(type)
  {
    case LINE_SCRIPT_INFO:
      break;
    case LINE_TITLE:
      break;
    case LINE_ORIGINAL_SCRIPT:
      break;
    case LINE_ORIGINAL_TRANS:
    case LINE_ORIGINAL_TIMING:
    case LINE_ORIGINAL_POINT:
    case LINE_SCRIPT_TYPE:
    case LINE_COLLISIONS:
    case LINE_RESX:
    case LINE_RESY:
    case LINE_TIMER:
    case LINE_FORMAT:
    case LINE_STYLE:
    case LINE_EVENT:
      break;
    case LINE_DIALOGUE:
      ret = parse_dialogue(p_in_data, in_len, is_scan, p_subt);
      break;

  }

  return ret;
}

BOOL subt_station_ssa_check(u8 *p_in_data, u32 in_len)
{
   if(LINE_TOTAL_SPEC != ssa_get_line_type(p_in_data))
   {
      return TRUE;
   }
   return FALSE;
}

RET_CODE  subt_station_ssa_parse(u8 *p_in_data, u32 in_len,
                subt_sys_type_t type, subt_station_content_t *p_subt)
{
  RET_CODE ret = ERR_FAILURE;
  u32 line_len = 0;

  if(type != SUBT_SYS_SSA) 
  {
    return ERR_FAILURE;
  }
  line_len = in_len;
  get_line_len(p_in_data, &line_len);
  ret = ssa_data_parse(p_in_data, line_len, FALSE, p_subt);
  
  return ret;
}

RET_CODE  subt_station_ssa_scan(u8 *p_in_data, u32 in_len, subt_station_key_point_t *p_out)
{
  RET_CODE ret = ERR_FAILURE;
  subt_station_content_t content = {0};
  
  ret = ssa_data_parse(p_in_data, in_len, TRUE, &content);
  if(ret == SUCCESS)
  {
    p_out->start = content.start;
    p_out->end = content.end;
  }

  return ret;
}


