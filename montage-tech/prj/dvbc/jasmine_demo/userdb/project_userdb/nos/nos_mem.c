/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "assert.h"
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_int.h"
#include "mtos_sem.h"
#include "../inc/mtos_os.h"


static pmalloc malloc_func = NULL;
static pfree free_func = NULL;

void  nos_mem_init(pmalloc m, pfree f)
{

  MT_ASSERT((NULL != m) && (NULL != f));
  malloc_func = m;
  free_func = f;
}


void *nos_malloc(u32 size)
{
  void *p_mem = malloc_func(size);
  return p_mem;
}

void nos_free(void *p_addr)
{
  free_func(p_addr);
}



void  nos_memory_init()
{
  mtos_mem_t *p_m_mem = NULL;
  p_m_mem = mtos_get_comp_handler(MTOS_MEMORY);
  p_m_mem->mem_init = nos_mem_init;
  p_m_mem->mem_malloc = nos_malloc;
  p_m_mem->mem_free =  nos_free;
  
}


