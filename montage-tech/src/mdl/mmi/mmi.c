/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
//#include "malloc.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "mem_manager.h"
#include "class_factory.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "mdl.h"
#include "mmi.h"


//#define MMI_DEBUG

#ifdef MMI_DEBUG
#define MMI_PRINTF    OS_PRINTF
#else
#define MMI_PRINTF DUMMY_PRINTF
#endif

/*!
   This structrue is defined to descript the global information of mmi module.
  */
typedef struct
{
  /*!
     The global memp object of mmi module.
    */
  lib_memp_t mmi_heap;
  /*!
     The address of the global memp object's buffer.
    */
  void *p_heap_addr;
}mmi_main_t;

void mmi_init_heap(u32 size)
{
  mmi_main_t *p_info = NULL;
  RET_CODE ret = ERR_FAILURE;

  // alloc global info
  p_info = (mmi_main_t *)mtos_malloc(sizeof(mmi_main_t));
  MT_ASSERT(p_info != NULL);

  class_register(MMI_CLASS_ID, (class_handle_t)(p_info));

  // alloc global buffer
  p_info->p_heap_addr = mtos_malloc(size);
  MT_ASSERT(p_info->p_heap_addr != NULL);

  ret = lib_memp_create(&p_info->mmi_heap, (u32)p_info->p_heap_addr, size);
  MT_ASSERT(ret == SUCCESS);
}


void mmi_release_heap(void)
{
  mmi_main_t *p_info = NULL;

  p_info = (mmi_main_t *)class_get_handle_by_id(MMI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  // release mmi heap
  lib_memp_destroy(&p_info->mmi_heap);

  // release mmi buffer
  mtos_free(p_info->p_heap_addr);

  // releae global infor
  mtos_free(p_info);
}


void *mmi_alloc_buf(u32 size)
{
  mmi_main_t *p_info = NULL;

  p_info = (mmi_main_t *)class_get_handle_by_id(MMI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  return (void *)lib_memp_alloc(&p_info->mmi_heap, size);
}


void *mmi_realloc_buf(void *p_addr, u32 size)
{
  mmi_main_t *p_info = NULL;

  p_info = (mmi_main_t *)class_get_handle_by_id(MMI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_addr != NULL)
  {
    return (void *)lib_memp_resize(&p_info->mmi_heap, p_addr, size);
  }
  else
  {
    return (void *)mmi_alloc_buf(size);
  }
}


void mmi_free_buf(void *p_addr)
{
  mmi_main_t *p_info = NULL;

  p_info = (mmi_main_t *)class_get_handle_by_id(MMI_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  lib_memp_free(&p_info->mmi_heap, p_addr);
}


void *mmi_create_memf(lib_memf_t *p_memf, u32 cnt, u32 slice)
{
  u32 size = 0;
  void *p_addr = NULL;

  MT_ASSERT(p_memf != NULL
           && cnt != 0
           && slice != 0);

  slice = ((slice + 3) / 4) * 4;
  size = cnt * slice;

  p_addr = mmi_alloc_buf(size);
  MT_ASSERT(p_addr != NULL);

  MMI_PRINTF("MMI: alloc buf size = %d\n", size);

  if(p_addr != NULL)
  {
    MMI_PRINTF("MMI: create memf slice = %d cnt = %d\n", slice, cnt);
    if(lib_memf_create(p_memf, (u32)p_addr, size, slice) != SUCCESS)
    {
      mmi_free_buf(p_addr);
      p_addr = NULL;
    }
  }

  return p_addr;
}


void mmi_destroy_memf(lib_memf_t *p_memf, void *p_addr)
{
  MT_ASSERT(p_memf != NULL
           && p_addr != NULL);
  lib_memf_destroy(p_memf);
  mmi_free_buf(p_addr);
}


void *mmi_create_memp(lib_memp_t *p_memp, u32 size)
{
  void *p_addr = NULL;

  MT_ASSERT(p_memp != NULL
           && size != 0);

  p_addr = mmi_alloc_buf(size);
  MT_ASSERT(p_addr != NULL);

  if(p_addr != NULL)
  {
    if(lib_memp_create(p_memp, (u32)p_addr, size) != SUCCESS)
    {
      mmi_free_buf(p_addr);
      p_addr = NULL;
    }
  }

  return p_addr;
}


void mmi_destroy_memp(lib_memp_t *p_memp, void *p_addr)
{
  MT_ASSERT(p_memp != NULL
           && p_addr != NULL);
  lib_memp_destroy(p_memp);
  mmi_free_buf(p_addr);
}

void mmi_assert(BOOL is_true)
{
  if(!is_true)
  {
    MT_ASSERT(0);
  }
}
