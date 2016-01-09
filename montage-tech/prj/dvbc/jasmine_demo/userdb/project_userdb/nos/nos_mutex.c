/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "assert.h"
#include "sys_types.h"
#include "sys_define.h"
#include ".\inc\includes.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "mtos_mutex.h"
#include "../inc/mtos_os.h"
u32 g_mutex = 0;

void *nos_mutex_create(unsigned int prio)
{


  return (void *)(&g_mutex);
}

int nos_mutex_delete(void *pevent)
{

	return 0;

}


int nos_mutex_give (void *pevent)
{

	return 0;
}


int nos_mutex_take(void *pevent)
{

	return 0;	
}


void nos_mutex_init()
{
  mtos_mutex_t *p_m_mutex = NULL;
  p_m_mutex = mtos_get_comp_handler(MTOS_MUTEX);
  p_m_mutex->mutex_create = nos_mutex_create;
  p_m_mutex->mutex_give = nos_mutex_give;
  p_m_mutex->mutex_take = nos_mutex_take;
  p_m_mutex->mutex_delete = nos_mutex_delete;
}

