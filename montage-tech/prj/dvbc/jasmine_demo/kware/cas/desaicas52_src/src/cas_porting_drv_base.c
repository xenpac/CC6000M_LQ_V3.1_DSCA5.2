/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include <stdarg.h>

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

u8 cas_adt_printf = 0;
u8 cas_drv_printf = 0;
u8 cas_lib_printf = 0;
u8 cas_debug_printf = 0;

void cas_porting_drv_printf(char *format,...)
{
    if(cas_drv_printf == 1)
    {
        va_list p_args = NULL;
        int  printed_len = 0;
        char    printk_buf[200];

        va_start(p_args, format);
        printed_len = vsnprintf(printk_buf, sizeof(printk_buf), (char *)format, p_args);

        va_end(p_args);
        OS_PRINTF(printk_buf);
 //lint -esym(550,printed_len)
 //lint -esym(438,printed_len)
    }
}

void cas_porting_lib_printf(char *format,...)
{
    if(cas_lib_printf == 1)
    {
      va_list p_args = NULL;
      int  printed_len = 0;
      char    printk_buf[200];

      va_start(p_args, format);
      printed_len = vsnprintf(printk_buf, sizeof(printk_buf), (char *)format, p_args);

      va_end(p_args);
      OS_PRINTF(printk_buf);
 //lint -esym(550,printed_len)
 //lint -esym(438,printed_len)
    }
}

void cas_porting_adt_printf(char *format,...)
{
    if(cas_adt_printf == 1)
    {
      va_list p_args = NULL;
      int  printed_len = 0;
      char    printk_buf[200];

      va_start(p_args, format);
      printed_len = vsnprintf(printk_buf, sizeof(printk_buf), (char *)format, p_args);

      va_end(p_args);
      OS_PRINTF(printk_buf);
 //lint -esym(550,printed_len)
 //lint -esym(438,printed_len)
   }
}  

void cas_porting_debug_printf(char *format,...)
{
    if(cas_debug_printf == 1)
    {
        va_list p_args = NULL;
        int  printed_len = 0;
        char    printk_buf[200];

        va_start(p_args, format);
        printed_len = vsnprintf(printk_buf, sizeof(printk_buf), (char *)format, p_args);

        va_end(p_args);
        OS_PRINTF(printk_buf);
 //lint -esym(550,printed_len)
 //lint -esym(438,printed_len)
    }
}

void cas_porting_printf_set(u8 adt, u8 drv, u8 lib, u8 debug)
{
    cas_adt_printf = adt;
    cas_drv_printf = drv;
    cas_lib_printf = lib;
    cas_debug_printf = debug;
}

void* cas_porting_drv_base_malloc(u32 size)
{
  return mtos_malloc(size);
}

void cas_porting_drv_base_free(void* p)
{
  mtos_free(p);
  return;
}

