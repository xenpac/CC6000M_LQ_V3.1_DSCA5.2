/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
// system
// std
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"
//#include "sys_regs_magic.h"
#include "sys_regs_concerto.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"
#include "common.h"
// os
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_int.h"
#include "mem_manager.h"
#include "hal.h"
#include "hal_uart.h"
#include "mem_cfg.h"

#include "hal_misc.h"
#include "sys_app.h"

extern u32 g_gui_resource_buffer_addr;
extern u32 g_epg_buffer_size;
extern u32 g_epg_buffer_addr;

void *mt_mem_malloc(u32 size)
{
  void *p_addr = NULL;
  mem_mgr_alloc_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.size = size;
  param.user_id = SYS_MODULE_SYSTEM;
  p_addr = mem_mgr_alloc(&param);
  //MT_ASSERT(p_addr != NULL);
  memset(p_addr, 0, size);

  return p_addr;
}

void mt_mem_free(void* p_addr)
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.p_addr = p_addr;
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}



extern void Task_SysTimer(void* pData);


u32 get_mem_addr(void)
{
  return 0x80000000;
}

u32 get_flash_addr(void)
{
  return 0x9e000000;
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


extern void  OSStart (void);
extern void ap_init(void);
extern u32 get_os_version();

#ifdef SHOW_MEM_SUPPORT
extern void show_single_memory_mapping(u32 flag,u32 star_addr,u32 size);
#endif

char TaskStartStk[SYS_INIT_TASK_STK_SIZE] __attribute__ ((aligned (8)));

char buildSTR[]=__DATE__"--"__TIME__;

void gpio_mux(void)
{
    /* uart1 mux 30:28, 26:24 all 1s */
  *(volatile unsigned int *)(0xbf15600c) &= ~0x77000000;
  *(volatile unsigned int *)(0xbf15600c) |= 0x11000000;
}

extern void hal_concerto_attach(void);
extern void mt_hal_invoke_constructors(void);
#if (defined MIN_AV_64M) && (defined DTMB_PROJECT)
extern void mem_map_cfg_dtmb_64m();
#endif
void TaskStart (void *data)
{
  extern u32 _end;
  u32 bss_end = (u32)(&_end);
  BOOL ret = FALSE;
  u32 *pstack_pnt = NULL;
  u32 heap_start = 0;
  u32 cpuclk = 0;
  u32 os_version = 0;
  u32 sys_partition_size = 0;

  heap_start = ROUNDUP(bss_end,4);
  mtos_irq_init();

  hal_concerto_attach( );

  mtos_register_putchar(uart_write_byte);
  mtos_register_getchar(uart_read_byte);

  hal_module_clk_get(HAL_CPU0, &cpuclk);
  mtos_ticks_init(cpuclk);

  gpio_mux();

  uart_init(0);
  uart_init(1);
  uart_set_param(0, 115200, 8, 1, 0);
  uart_set_param(1, 115200, 8, 1, 0);
#if (defined MIN_AV_64M) && (defined DTMB_PROJECT)
  mem_map_cfg_dtmb_64m();
#else
  mem_map_cfg();
#endif
  app_printf(PRINTF_INFO_LEVEL,"\n MaincodeTaskStart\n");
  app_printf(PRINTF_INFO_LEVEL,"\n Built at %s \n", buildSTR);

  app_printf(PRINTF_INFO_LEVEL,"\n 1. Heap_start[0x%x]\n", heap_start);
  app_printf(PRINTF_INFO_LEVEL,"\n 2. Whole memory size[0x%x]\n", (WHOLE_MEM_END & 0xFFFFFFF));
  sys_partition_size = (((g_gui_resource_buffer_addr & (~0xa0000000)) - (heap_start & (~0xa0000000))) / 0x400) * 0x400;
  app_printf(PRINTF_INFO_LEVEL,"\n 3. Sys_partition_size[0x%x]\n", sys_partition_size);
 #ifdef SHOW_MEM_SUPPORT
  show_single_memory_mapping(CODE_SIZE_FLAG,0,heap_start);
  show_single_memory_mapping(SYSTEM_PARTITION_FLAG,heap_start,sys_partition_size);
  #endif

  app_printf(PRINTF_INFO_LEVEL,"\n 4. Sys_partition end is [0x%x], APP end is [0x%x]\n", 
    (heap_start + sys_partition_size), ((g_epg_buffer_addr + g_epg_buffer_size) & (0x8FFFFFFF)));
  OS_PRINTF("\n @@@Heap_start[0x%x], Sys_partition_size[0x%x]\n",(heap_start), sys_partition_size);
  mem_mgr_init((u8 *)heap_start,
  sys_partition_size);

  app_printf(PRINTF_INFO_LEVEL,"\n 5. Mem_mgr_start[0x%x],size=0x%x\n", heap_start,
  	sys_partition_size);
  dlmem_init((void *)heap_start,
  	sys_partition_size);
  mtos_mem_init(dl_malloc, dl_free);
  app_printf(PRINTF_INFO_LEVEL,"Init mem ok!\n");

/*lint -e746*/
  os_version = get_os_version();
/*lint +e746*/

  if (os_version != 0x11112222) {
    app_printf(PRINTF_ERROR_LEVEL,"Wrong os version, please talk with os person!\n");

  }

  app_printf(PRINTF_INFO_LEVEL,"Os version pass!\n");

#ifdef ENABLE_NETWORK
  mt_hal_invoke_constructors(); /* just for c++ init,  if no cpp, not needed*/
#endif

//enable interrupt
  mtos_irq_enable(TRUE);
  app_printf(PRINTF_DEBUG_LEVEL,"Enable irq!\n");

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

  ap_init();

  mtos_task_exit();

}


extern void EXCEP_vInstallIntInRam(void);

int main(void)
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

