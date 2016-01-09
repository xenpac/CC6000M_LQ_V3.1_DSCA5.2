/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "assert.h"

#include "sys_types.h"
#include "sys_define.h"
#include ".\inc\includes.h"
#include "hal_timer.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "../inc/mtos_os.h"





//static s32 os_ns_delay_timer = -1;
static u32 os_ns_timer_loop = 0;
static u32 mtos_cpu_freq = 0;


void nos_init(void )
{

}
void nos_start(void )
{

}

void nos_stat_init(void )
{

}

u32 nos_hw_loop_get(void)
{
  return os_ns_timer_loop;
}

//extern void clear_timer_state();

BOOL nos_ticks_init(u32 cpu_freq)
{
  return TRUE;
}

u32 nos_ticks_get(void)
{
  return 0;//OSTimeGet( );
}
u32 nos_cpu_freq_get(void)
{
  return mtos_cpu_freq;
}

u32 nos_hw_ticks_get(void)
{
  volatile u32 cnt;// = hal_timer_get_count(os_ns_delay_timer);
  return cnt;  //just to make the compiler happy
}

void nos_task_delay_us(u32 us)
{
  
}

void nos_critical_enter(u32 *p_sr)
{
#if (OS_CRITICAL_METHOD == 3)
/* Allocate storage for CPU status register */
    u32 cpu_sr = 0;

    //OS_ENTER_CRITICAL();
    *p_sr = cpu_sr;

#endif //(OS_CRITICAL_METHOD == 3)
}

void nos_critical_exit(u32 sr)
{
#if (OS_CRITICAL_METHOD == 3)
/* Allocate storage for CPU status register */
    u32 cpu_sr = sr;
    //OS_EXIT_CRITICAL();
#endif //(OS_CRITICAL_METHOD == 3)
}

#ifdef WIN32
#include <windows.h>
void nos_task_delay_ms(u32 ms)
{

}

#else
void nos_task_delay_ms(u32 ms)
{

}
#endif

#ifndef WIN32
extern void mem_dump(void);                                           
void nos_assert()
{            
//  mem_dump();                                                           
//  __asm volatile ("break 0");                                         
                                                                            
}

#else
void nos_assert()
{                                                                                                             
                                                                                                                 
}
#endif


void nos_misc_init()
{

  mtos_misc_t *p_misc = NULL; 
  p_misc = mtos_get_comp_handler(MTOS_MISC);

  p_misc->ticks_init=  nos_ticks_init;
  p_misc->ticks_get =  nos_ticks_get;
  p_misc->cpu_freq_get = nos_cpu_freq_get;
  p_misc->critical_enter =  nos_critical_enter;
  p_misc->critical_exit = nos_critical_exit;
  p_misc->task_delay_ms =  nos_task_delay_ms;
  p_misc->task_delay_us = nos_task_delay_us;
  p_misc->stat_init = nos_stat_init;
  p_misc->hw_loop_get = nos_hw_loop_get;
  p_misc->hw_ticks_get = nos_hw_ticks_get;
  p_misc->misc_start = nos_start;
  p_misc->os_assert = nos_assert;
}


