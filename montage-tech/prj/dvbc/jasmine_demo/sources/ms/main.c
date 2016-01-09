/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*!
  Win32 main file.
  */

// standard library
#include <malloc.h>
#include <assert.h>

// system
#include "sys_define.h"
#include "sys_types.h"
#include "hal.h"

// os
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_timer.h"

// util
#include "mem_manager.h"

// Project header files
#include "mem_cfg.h"
#include "sys_cfg.h"

u32 get_mem_addr(void)
{
  static u8 g_sys_mem[SYS_MEMORY_END] = {0};
  return (u32)&g_sys_mem[0];
}

u32 get_flash_addr(void)
{
  extern u8 *p_flash_buf;
  return (u32)p_flash_buf;
}

u32 get_bootload_off_addr(void)
{
  return DM_BOOTER_START_ADDR;
}

u32 get_otaback_off_addr(void)
{
  return DM_HDR_START_ADDR_BAK;
}

u32 get_maincode_off_addr(void)
{
  return DM_HDR_START_ADDR;
}

void* my_malloc(u32 size)
{
  mem_mgr_alloc_param_t para = {0};
  para.id = MEM_SYS_PARTITION;
  para.size = size;
  return MEM_ALLOC(&para);
  //return malloc(size);
}

void my_free(void *p_addr)
{
  mem_mgr_free_param_t para = {0};
  para.id = MEM_SYS_PARTITION;
  para.p_addr = p_addr;
  FREE_MEM(&para);
  //free(p_addr);
}

void task_sys_init(void *p_data)
{
  BOOL ret = FALSE;
  u32 *pstack_pnt = NULL;
  mem_mgr_partition_param_t partition_param = { 0 };
  extern void Task_SysTimer(void *p_data);
  extern void ap_init(void);
  hal_config_t hal_config = {0};

  hal_win32_attach(&hal_config);

  mem_map_cfg();
  //init memory manager, to the memory end
  mem_mgr_init((u8*)get_mem_addr(), SYS_MEMORY_END);

  /* create SYS partition */
  partition_param.id   = MEM_SYS_PARTITION;
  partition_param.size = 9 * 1024 * 1024;
  partition_param.p_addr = (u8*)get_mem_addr();
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_SYSTEM;
  partition_param.method_id = MEM_METHOD_NORMAL;
  ret = MEM_CREATE_PARTITION(&partition_param);
  MT_ASSERT(FALSE != ret);
  OS_PRINTF("create patition ok!\n");

  //register malloc/free function, using system partition
  mtos_mem_init(my_malloc, my_free);
  OS_PRINTF("init mem ok!\n");

  mtos_ticks_init(SYS_CPU_CLOCK);

  //init message queue
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);

#ifdef CORE_DUMP_DEBUG
  //start statistic task, MUST BE in the first task!
  mtos_stat_init();
#endif

  //create timer task
  pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STKSIZE);
  MT_ASSERT(pstack_pnt != NULL);

  ret = mtos_task_create((u8 *)"Timer",
    Task_SysTimer,
    (void *)0,
    SYS_TIMER_TASK_PRIORITY,
    pstack_pnt,
    SYS_TIMER_TASK_STKSIZE);

  MT_ASSERT(FALSE != ret);
  OS_PRINTF("create timer task ok!\n");

  mtos_task_sleep(25);
  OS_PRINTF("start ap init!\n");
  ap_init();
  OS_PRINTF("ap init ok!\n");

  mtos_task_exit( );
}

int main_entry(void)
{
  u8* p_stack = malloc(SYS_INIT_TASK_STKSIZE);
  mtos_cfg_t os_cfg = {0};

  os_cfg.enable_bhr = FALSE;
  mtos_os_init(&os_cfg);

  mtos_task_create((u8 *)"initT",
    task_sys_init,
    NULL, 
    SYS_INIT_TASK_PRIORITY,
    (u32*)p_stack,
    SYS_INIT_TASK_STKSIZE);

  /* begin to run multi-task */
  mtos_start();
  return 0;
}
