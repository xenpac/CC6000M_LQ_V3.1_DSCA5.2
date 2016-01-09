/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "./inc/nos_ii.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#ifdef CORE_DUMP_DEBUG
#include "mtos_debug.h"
#endif

#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "../inc/mtos_os.h"



//#define TASK_DBG
#ifdef TASK_DBG
#define TASK_PRINT(x) (OS_PRINTF x)
#else
#define TASK_PRINT(x) do {} while(0)
#endif

			   

BOOL nos_task_create(u8   *p_taskname,
                     void (*p_taskproc)(void *p_param),
                     void  *p_param,
                     u32    nprio,
                     u32   *pstk,
                     u32    nstksize)
{

  return TRUE;
	
	

}


void nos_task_sleep(u32 ms)
{


}


void nos_task_lock(void)
{

  
}


void nos_task_unlock(void)
{

 	 
}


void nos_task_exit(void)
{

}

void nos_task_resume(u8 prio)
{

	
}

void nos_task_suspend(u8 prio)
{



}

void nos_task_change_prio(u8 oldprio, u8 newprio)
{
	

}


	
void nos_task_init()
{
  mtos_task_t *p_m_task = NULL; 
  p_m_task = mtos_get_comp_handler(MTOS_TASK);
  p_m_task->task_create = nos_task_create;
  p_m_task->task_sleep = nos_task_sleep;
  p_m_task->task_exit = nos_task_exit;
	
  p_m_task->task_lock = nos_task_lock;
  p_m_task->task_unlock = nos_task_unlock;
  p_m_task->task_resume = nos_task_resume;
  p_m_task->task_suspend = nos_task_suspend;
  p_m_task->task_change_prio = nos_task_change_prio;
}

void main_entry(void)
{
}
