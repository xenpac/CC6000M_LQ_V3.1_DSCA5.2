/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "../../includes/sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"
#include "common.h"
#include "drv_dev.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_int.h"
#include "mem_manager.h"
#include "hal.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "mem_cfg.h"
#include "hal_misc.h"


char buildSTR[]=__DATE__"--"__TIME__;

extern void Task_SysTimer(void* pData);


u32 get_mem_addr(void)
{
  return 0xa0000000;
}

u32 get_flash_addr(void)
{
  return 0x9e000000;
}
extern void  OSStart (void);
extern void Task_SysTimer(void* pData);
extern void ap_init(void);

char TaskStartStk[SYS_INIT_TASK_STKSIZE] __attribute__ ((aligned (8)));

void TaskStart (void *data)
{
  extern u32 _end;
  u32 bss_end = (u32)(&_end);
  BOOL ret = FALSE;
  u32 heap_start = 0;
  u32 cpuclk = 0;
  u32 sys_partition_size = 0;

  heap_start = ROUNDUP(bss_end,4);
  mtos_irq_init();
  extern void hal_concerto_attach(void);

  hal_concerto_attach( );
  mtos_register_putchar(uart_write_byte);
  mtos_register_getchar(uart_read_byte);

  hal_module_clk_get(HAL_CPU0, &cpuclk);
  mtos_ticks_init(cpuclk);

  uart_init(0);

  OS_PRINTF("\n FastLogo app start\n");
  OS_PRINTF("\n Built at %s \n", buildSTR);

  OS_PRINTF("\n 1. heap_start[0x%x]\n", heap_start);
  OS_PRINTF("\n 2. whole memory size[0x%x]\n", (WHOLE_MEM_END & 0xFFFFFFF));
  sys_partition_size = (((GUI_RESOURCE_BUFFER_ADDR & (~0xa0000000)) - (heap_start & (~0xa0000000))) / 0x400) * 0x400;

  OS_PRINTF("\n 3. Sys_partition_size[0x%x]\n", sys_partition_size);

  //init memory manager, only 11M
  //mem_mgr_init((u8 *)heap_start, SYS_PARTITION_SIZE + (1 * 1024 * 1024));
  mem_mgr_init((u8 *)heap_start,
  sys_partition_size);

  OS_PRINTF("\n 4. mem_mgr_start[0x%x],size=0x%x\n", heap_start,sys_partition_size);
  
  dlmem_init((void *)heap_start,
  	sys_partition_size);
  mtos_mem_init(dl_malloc, dl_free);
  OS_PRINTF("init mem ok!\n");


//enable interrupt
  mtos_irq_enable(TRUE);

  //init message queue
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);


#ifdef CORE_DUMP_DEBUG
  //start statistic task, MUST BE in the first task!
  mtos_stat_init();
#endif

  ap_init();

  mtos_task_exit();
}

extern void EXCEP_vInstallIntInRam(void);

int main(int p1, char **p2, char **p3)
{
  mtos_cfg_t os_cfg = {0};

  EXCEP_vInstallIntInRam();

  /* Initialize uC/OS-II                                      */

  mtos_os_init(&os_cfg);

  mtos_task_create((u8 *)"initT1",
                 TaskStart,
                 NULL,
                 SYS_INIT_TASK_PRIORITY,
                 (u32*)TaskStartStk,
                 SYS_INIT_TASK_STKSIZE);

  OSStart();                              /* Start multitasking    */

  return 0;
}

