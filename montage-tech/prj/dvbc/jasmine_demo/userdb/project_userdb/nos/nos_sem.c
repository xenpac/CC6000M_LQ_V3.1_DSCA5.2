/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "assert.h"
#include "sys_types.h"
#include "sys_define.h"
#include "./inc/includes.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "../inc/mtos_os.h"



//#define SEM_DBG
#ifdef SEM_DBG
#define SEM_PRINT(x) (OS_PRINTF x)
#else
#define SEM_PRINT(x) do {} while(0)
#endif



BOOL nos_sem_create(os_sem_t *p_sem, u16 sem_count)
{

  return TRUE;
}

BOOL nos_sem_destroy(os_sem_t *p_sem, u8 opt, u8 *err)
{

  return TRUE;
}


BOOL nos_sem_give(os_sem_t *p_sem)
{

  return FALSE;
}


BOOL nos_sem_take(os_sem_t *p_sem, u16 ms, u8 *err)
{

    return FALSE;

  	
}


void nos_sem_init()
{

  mtos_sem_t *p_m_sem = NULL;
  p_m_sem = mtos_get_comp_handler(MTOS_SEM);
  p_m_sem->sem_create = nos_sem_create;
  p_m_sem->sem_give = nos_sem_give;
  p_m_sem->sem_take = nos_sem_take;
  p_m_sem->sem_destroy = nos_sem_destroy;
}


