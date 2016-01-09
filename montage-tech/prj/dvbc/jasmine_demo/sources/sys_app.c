/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/*lint -e451 -e530 -e830 -e616  for pc-lint*/
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"
// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_event.h"
#include "mtos_printk.h"

#include "sys_app.h"

char g_printf_buf[4096];
void  app_printf(app_printf_level level,const char *p_format, ...)
{
  va_list arg_ptr;

  memset(g_printf_buf,0,4096);
  va_start(arg_ptr, p_format);
  vsnprintf(g_printf_buf, sizeof(g_printf_buf), p_format, arg_ptr);

  switch(level)
    {
      case PRINTF_DEBUG_LEVEL:
        {
          #ifdef PRINTF_DEBUG
          mtos_printk("%s",g_printf_buf);
          #endif
        }
        break;
      case PRINTF_WARNING_LEVEL:
        {
          #ifdef PRINTF_WARNING
          mtos_printk("%s",g_printf_buf);
          #endif
        }
        break;
      case PRINTF_ERROR_LEVEL:
        {
          #ifdef PRINTF_ERROR
          mtos_printk("%s",g_printf_buf);
          #endif
        }
      case PRINTF_INFO_LEVEL:
        {
          #ifdef PRINTF_INFO
          mtos_printk("%s",g_printf_buf);
          #endif
        }
        break;
      case PRINTF_ALWAYS_LEVEL:
        mtos_printk("%s",g_printf_buf);
        break;
        default:
          break;
    }
  va_end(arg_ptr); 
}
/*lint +e451 +e530 +e830 +e616  for pc-lint*/


