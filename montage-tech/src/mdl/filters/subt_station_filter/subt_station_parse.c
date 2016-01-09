/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
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

#include "drv_misc.h"

#include "ifilter.h"
#include "transf_filter.h"
#include "common_filter.h"
#include "subt_station_filter.h"
#include "subt_station_database.h"
#include "subt_station_parse.h"




typedef struct
{
  subt_station_parse_t parsing[SUBT_SYS_UNKOWN];
}subt_station_parse_priv_t;

static subt_station_parse_priv_t g_subt_parse_priv = {{{0}}};

RET_CODE subt_station_parse_func(u8 *p_in_data, u32 in_len,
                subt_sys_type_t type, void *p_subt)
{
  RET_CODE ret = ERR_FAILURE;

 if(type != SUBT_SYS_UNKOWN
  && g_subt_parse_priv.parsing[type].parse != NULL)
 {
  ret = g_subt_parse_priv.parsing[type].parse(p_in_data, in_len, type, p_subt);
 }
 return ret;
}

RET_CODE subt_station_scan_func(u8 *p_in_data, u32 in_len,
                subt_sys_type_t type, void *p_key)
{
  if(type == SUBT_SYS_UNKOWN
    || g_subt_parse_priv.parsing[type].scan == NULL)
  {
    return ERR_FAILURE;
  }
  return g_subt_parse_priv.parsing[type].scan(p_in_data, in_len, p_key);
}

subt_sys_type_t subt_station_get_type(u8 *p_in_data, u32 in_len)
{
  u8 i = 0;
  RET_CODE ret = ERR_FAILURE;
  
  for(i = 0; i < SUBT_SYS_UNKOWN; i ++)
  {
    if(g_subt_parse_priv.parsing[i].check != NULL)
    {
      ret = g_subt_parse_priv.parsing[i].check(p_in_data, in_len);
      if(ret == TRUE)
      {
        break;
      }
    }
  }
  if(i < SUBT_SYS_UNKOWN)
  {
    return i;
  }

  return SUBT_SYS_UNKOWN;
}

void subt_station_register(void *p_parsing)
{
  subt_station_parse_t *p_parse = (subt_station_parse_t *)p_parsing;
  subt_sys_type_t type = p_parse->type;
  
  g_subt_parse_priv.parsing[type].type = type;
  g_subt_parse_priv.parsing[type].parse = p_parse->parse;
  g_subt_parse_priv.parsing[type].scan = p_parse->scan;
  g_subt_parse_priv.parsing[type].check = p_parse->check;
}
