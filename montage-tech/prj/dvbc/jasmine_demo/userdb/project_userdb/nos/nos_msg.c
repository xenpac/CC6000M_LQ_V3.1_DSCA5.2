/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

/*!
  \file mtos_msg.c

  This file defined msg module.

  */
#include "assert.h"
#include "sys_types.h"
#include "sys_define.h"
#include ".\inc\includes.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_int.h"
#include "../inc/mtos_os.h"



#ifdef CORE_DUMP_DEBUG
#include "mtos_debug.h"
#endif

//#define MSG_DBG
#if 1
#define MSG_PRINT(x) (OS_PRINTF x)
#else
#define MSG_PRINT(x) do {} while(0)
#endif


static void ** os_msg_priv_data;

void *os_osq_create (u32 depth, s32 key)
{

  return (void *)0;
}


BOOL os_osq_del (void *msg_id, u32 msgq_id)
{

  return TRUE;
}

BOOL os_queue_flush(void *msg_id)
{

  return TRUE;
}




BOOL os_osq_post(void *msg_id, void *pmsg)
{

  return FALSE;
}



BOOL os_osq_pend(void *msg_id, void *buffer, u16 timeout, u8 *err)
{

    return FALSE;	
}



void os_queue_query(void *msg_id, u32 *p_left)
{

}



void nos_msg_init()
{

  mtos_msg_t *p_m_msg = NULL; 
  p_m_msg = mtos_get_comp_handler(MTOS_MSG);
	
  p_m_msg->queue_create = os_osq_create;
  p_m_msg->queue_post = os_osq_post;
  p_m_msg->queue_del = os_osq_del;
  p_m_msg->queue_pend = os_osq_pend;
  p_m_msg->queue_flush = os_queue_flush;
  p_m_msg->queue_query = os_queue_query;
  p_m_msg->priv_data = &os_msg_priv_data;
	
}

