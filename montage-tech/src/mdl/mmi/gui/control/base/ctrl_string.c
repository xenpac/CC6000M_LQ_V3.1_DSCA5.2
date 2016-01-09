/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_string.c
   this file  implement the functions defined in  ctrl_string.h,
   it's mainly about string memory allocate and release.
  */

#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"

//#include "osd.h"
//#include "gpe.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"

#include "gdi.h"

#include "mdl.h"
#include "mmi.h"

#include "class_factory.h"

#include "ctrl_base.h"
#include "ctrl_string.h"

//#define STR_DEBUG

#ifdef STR_DEBUG
#define STR_PRINTF    OS_PRINTF
#else
#define STR_PRINTF DUMMY_PRINTF
#endif


/*!
   allocate memory for string.

   \param[in] len			: string length.
   \param[in] is_unistr	: string is unicode or not.
  */
static void *_str_alloc(u32 len, BOOL is_unistr);

/*!
   free memory of string.

   \param[in] str			: string to be released.
   \param[in] is_unistr	: string is unicode type or not.
  */
static void _str_free(void *p_str, BOOL is_unistr);

/*!
   reallocate memory for string.

   \param[in] str			: string to be reallocated.
   \param[in] len			: string length.
   \param[in] is_unistr	: string is unicode type or not.
  */
static void *_str_realloc(void *p_str, u32 len, BOOL is_unistr);


static void *_str_alloc(u32 len, BOOL is_unistr)
{
  u32 bytes = 0;
  void *p_str = NULL;
  control_lib_t *p_info = NULL;
 
  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(len > 0)
  {
    bytes = (len + 1) * (is_unistr ? sizeof(u16) : sizeof(u8));

    STR_PRINTF("-------alloc string bytes = %d ------\n", bytes);

    p_str = lib_memp_alloc(&p_info->string_heap, bytes);

    if(p_str != NULL)
    {
      memset(p_str, 0, bytes);
#ifdef STR_DEBUG
      p_info->count++;
      STR_PRINTF("-------alloc string count = %d len = %d bytes = %d ------\n",
                 p_info->count, len,
                 bytes);
#endif
    }
    MT_ASSERT(p_str != NULL);
  }

  return p_str;
}


static void _str_free(void *p_str, BOOL is_unistr)
{
  control_lib_t *p_info = NULL;

  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_str != NULL)
  {
    lib_memp_free(&p_info->string_heap, p_str);
#ifdef STR_DEBUG
    (p_info->count--);
    STR_PRINTF("-------free string count = %d------\n", p_info->count);
#endif
  }
}


static void *_str_realloc(void *p_str, u32 len, BOOL is_unistr)
{
  u32 bytes = 0;
  void *p_new_str = NULL;
  control_lib_t *p_info = NULL;
 
  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(len > 0)
  {
    if(p_str != NULL)
    {
      bytes = (len + 1) * (is_unistr ? sizeof(u16) : sizeof(u8));
      p_new_str = lib_memp_resize(&p_info->string_heap, p_str, bytes);
    }
    else
    {
      p_new_str = _str_alloc(len, is_unistr);
    }
  }
  else
  {
    _str_free(p_str, is_unistr);
    p_new_str = NULL;
  }

  return p_new_str;
}


void *ctrl_unistr_alloc(u32 len)
{
  return _str_alloc(len, TRUE);
}


void *ctrl_unistr_realloc(void *p_str, u32 len)
{
  return _str_realloc(p_str, len, TRUE);
}


void ctrl_unistr_free(void *p_str)
{
  _str_free(p_str, TRUE);
}


void *ctrl_ascstr_alloc(u32 len)
{
  return _str_alloc(len, FALSE);
}


void ctrl_ascstr_free(void *p_str)
{
  _str_free(p_str, FALSE);
}


void *ctrl_asc_str_realloc(void *p_str, u32 len)
{
  return _str_realloc(p_str, len, FALSE);
}


void ctrl_str_init(u32 buf_size)
{
  control_lib_t *p_info = NULL;
 
  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_info->p_string_heap_addr = mmi_create_memp(&p_info->string_heap, buf_size);
  MT_ASSERT(p_info->p_string_heap_addr != NULL);  
}


void ctrl_str_release(void)
{
  control_lib_t *p_info = NULL;

  p_info = (control_lib_t *)class_get_handle_by_id(GUI_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  mmi_destroy_memp(&p_info->string_heap, p_info->p_string_heap_addr);
}
