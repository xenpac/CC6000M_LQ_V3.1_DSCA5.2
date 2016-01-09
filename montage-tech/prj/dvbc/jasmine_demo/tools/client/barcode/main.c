/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mem_manager.h"
#include "stdlib.h"
#include "string.h"
//#include "scheduler.h"
#include "ipc.h"
#include "hal.h"
#include "hal_misc.h"
#include "sys_cfg.h"

/* File local definitions */
#define  TASK_STK_SIZE 4 *1024                      /* Size of each task's stacks (# of WORDs) */
char TaskStartStk[TASK_STK_SIZE];

extern s32 uart_write_byte(u8 id, u8 chval);
extern s32 uart_init(u8 num);
extern s32 uart_read_byte_polling(u8 id, u8 *p_data, u32 timeout);

#define CPU_IS_AP 1

static void *mt_mem_malloc(u32 size)
{
  void *p_addr = NULL;
  mem_mgr_alloc_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.size = size;
  param.user_id = SYS_MODULE_SYSTEM;
  p_addr = mem_mgr_alloc(&param);
  MT_ASSERT(p_addr != NULL);
  return p_addr;
}

static void mt_mem_free(void* p_addr)
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.p_addr = p_addr;
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}

/* ********************************************************************* */
/* Global functions */
extern void  OSStart (void);
extern void Task_SysTimer(void* pData);
extern void ap_init(void);
extern u32 get_os_version();


//extern s32 hal_uart_warriors_write_byte(u8 id, u8 chval);
//extern s32 hal_uart_warriors_b0_write_byte(u8 id, u8 chval);

 extern void init_section();
extern u32 *p_DUAL_CORE_PRINTK_LOCK ;
void TaskStart (void *data)
{
  extern u32 _end;
  u32 bss_end = (u32)(&_end);
  BOOL ret = FALSE;
  u32 *pstack_pnt = NULL;
  u32 heap_start = 0;
 // u32 montage_auto = 0;
  u32 cpuclk = 0;

  heap_start = ROUNDUP(bss_end,4);

  mtos_irq_init();

//  hal_warriors_attach(&hal_config);
  extern void hal_concerto_attach(void);

  hal_concerto_attach( );

  mtos_register_putchar(uart_write_byte);
  mtos_register_getchar(uart_read_byte_polling);

  //mtos_ticks_init(SYS_CPU_CLOCK);
  hal_module_clk_get(HAL_CPU0, &cpuclk);
  mtos_ticks_init(cpuclk);

  mem_mgr_partition_param_t partition_param = { 0 };//, partition2_param = { 0 };
  uart_init(0);
  uart_init(1);

  //init memory manager, to the memory end
  mem_mgr_init((u8 *)heap_start, SYSTEM_MEMORY_END);

  //create system partition
  partition_param.id   = MEM_SYS_PARTITION;
  partition_param.size = SYS_PARTITION_SIZE;
  partition_param.p_addr = (u8 *)heap_start;
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_SYSTEM;
  partition_param.method_id = MEM_METHOD_NORMAL;
  ret = MEM_CREATE_PARTITION(&partition_param);
  MT_ASSERT(FALSE != ret);
#if 0
  OS_PRINTF("\n 1. mem_start[0x%x]\n", heap_start);
  OS_PRINTF("\n 2. system memory size[%d]\n", SYSTEM_MEMORY_END);
  OS_PRINTF("SYS_PARTITION_SIZE is %x\n", SYS_PARTITION_SIZE);
  OS_PRINTF("create SYS_PARTITION111 ok!\n");

  partition2_param.id   = MEM_DMA_PARTITION;
  partition2_param.size = DMA_PARTITION_SIZE;
  partition2_param.p_addr = (u8 *)((heap_start + SYS_PARTITION_SIZE + 0x40000) | 0xa0000000);
  partition2_param.atom_size = DMA_PARTITION_ATOM;
  partition2_param.user_id = SYS_MODULE_SYSTEM;
  partition2_param.method_id = MEM_METHOD_NORMAL;

  OS_PRINTF("\n 1. mem_start[0x%x]\n", partition2_param.p_addr);
  OS_PRINTF("\n 2. system memory size[%d]\n", SYSTEM_MEMORY_END);
  OS_PRINTF("SYS_PARTITION_SIZE is 0x%x\n", DMA_PARTITION_SIZE);


  ret = MEM_CREATE_PARTITION(&partition2_param);
  MT_ASSERT(FALSE != ret);
#endif
  mtos_mem_init(mt_mem_malloc, mt_mem_free);
  #if 0
  OS_PRINTF("Now check os version!\n");
  u32 os_version = get_os_version();

  if (os_version != 0x11112222) {
    OS_PRINTF("Wrong os version, please talk with os person!\n");
  }

  OS_PRINTF("Os version pass!\n");

  if(CPU_IS_AP) {
      OS_PRINTF("AP CPU id get pass \n");
  }else{
      OS_PRINTF("cannot get right cpu id \n");
  }
  #endif
 //create timer task
  pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STK_SIZE);
  MT_ASSERT(pstack_pnt != NULL);

  ret = mtos_task_create((u8 *)"Timer",
         Task_SysTimer,
         (void *)0,
         SYS_TIMER_TASK_PRIORITY,
         pstack_pnt,
         SYS_TIMER_TASK_STK_SIZE);

  MT_ASSERT(FALSE != ret);

  // init_dual_printk();

   //init message queue
  //OS_PRINTF("start    mtos_irq_enable!!!!!!!!!!!!!!\n");
  mtos_irq_enable(TRUE);
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);


#ifdef CORE_DUMP_DEBUG
   mtos_stat_init();
#endif

   init_section();
  mtos_task_sleep(100);

   ap_init();

  mtos_task_exit();
}



//extern void EXCEP_vInstallIntInRam_war(void);
extern void EXCEP_vInstallIntInRam(void);
#define WARRIORS_AX 0
int main(void)
{
//  EXCEP_vInstallIntInRam_war();
  EXCEP_vInstallIntInRam();

//#if WARRIORS_AX // for chip A0/A1
//  hal_uart_warriors_write_byte(0, 'P');
//#else //for chip B0/B1
//  hal_uart_warriors_b0_write_byte(0, 'P');
//#endif 

  /* Initialize uC/OS-II                                      */
  mtos_cfg_t os_cfg = {0};
  mtos_os_init(&os_cfg);
  mtos_task_create((u8 *)"initT1",
                 TaskStart,
                 NULL,
                 SYS_INIT_TASK_PRIORITY,
                 (u32*)TaskStartStk,
                 TASK_STK_SIZE);

  OSStart();                              /* Start multitasking    */
  return 0;
}


/* ********************************************************************* */
