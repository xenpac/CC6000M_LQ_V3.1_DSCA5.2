/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// sys headers
#include "sys_types.h"
#include "sys_define.h"

#include "log.h"
#if 0
/*!
  struct
  */
typedef struct
{
  char *p_file;
  u32 module_id;
}log_info;
#endif
/*
log_info _info[] = 
{
    {"ap_scan.c", LOG_AP_SCAN},
    {"ap_bl_scan.c", LOG_AP_SCAN},
    {"ap_tp_scan.c", LOG_AP_SCAN},

};

BOOL sreach_module(char *p_file_name)
{
  u32 depth = (sizeof(_info) / sizeof(log_info));
  u32 i = 0;
  log_info *p_cur; 

  for(i = 0; i < depth; i++)
  {
    p_cur = _info + i;
    if(strcmp(p_cur->p_file, p_file_name) == 0)
    {
      return p_cur->module_id;
    }
  }

  return FALSE;
}
*/
