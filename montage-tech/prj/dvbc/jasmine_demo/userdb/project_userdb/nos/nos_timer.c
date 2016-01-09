/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

/*!
  \file mtos_timer.c

  This file defined timer module.

  */
#include "assert.h"
#include "sys_types.h"
#include "sys_define.h"
#include "./inc/nos_ii.h"
#include "mtos_printk.h"
#include "mtos_task.h"

#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "../inc/mtos_os.h"



//void RegisterTickHook(void  (*p_timerproc)(int sig, void *extra, void *cruft));



static void nos_timer_create(int signum, int nsec, int sec, void  (*p_timerproc)(int sig, void *extra, void *cruft))
{
   return;
}




void nos_timer_init()
{
  mtos_timer_t *p_m_timer = NULL; 
  p_m_timer = mtos_get_comp_handler(MTOS_TIMER);
  p_m_timer->timer_create = nos_timer_create;
}

					
