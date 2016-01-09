/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include ".\inc\includes.h"
#include "../nos/inc/nos_handler.h"
#include "../inc/mtos_os.h"


void mtos_handler_init(mtos_cfg_t *p_cfg)
{
  nos_memory_init();
  nos_task_init();
  nos_msg_init();
  nos_sem_init();
  nos_mutex_init();
  nos_timer_init();
  nos_misc_init();
  //#ifndef WIN32
  // Todo: fix me, irq shall not be implemented in this level
  nos_irq_handler_init(p_cfg);
  //#endif
  nos_event_init();
}


void nosInit(void *p_cfg)
{

	mtos_handler_init(p_cfg);
}


void no_os_init(mtos_cfg_t *p_cfg)
{
  nosInit(p_cfg);
  //mtos_handler_init(p_cfg);
}







