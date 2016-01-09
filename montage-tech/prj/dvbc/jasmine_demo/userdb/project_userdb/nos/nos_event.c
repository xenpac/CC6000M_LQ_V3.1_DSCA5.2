/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "mtos_printk.h"
#include "sys_define.h"
#include "mtos_event.h"
#include "mtos_mem.h"
#include "mtos_int.h"
#include "mtos_sem.h"
#include "mtos_os.h"

BOOL nos_event_create(unsigned int *p_event)
{
  

  return FALSE;
}

BOOL nos_event_destroy(unsigned int *p_event)
{


  return FALSE;
}


BOOL nos_event_wait(unsigned int *p_event,
      unsigned int type,
      unsigned int flag,
      unsigned int *p_value,
      unsigned int timeout)
{


  return FALSE;
}

BOOL nos_event_set(unsigned int *p_event, unsigned int mask)
{
  
  return FALSE;
}


BOOL nos_event_query(unsigned int *p_event, unsigned int *mask)
{

  return FALSE;
}

void nos_event_init()
{
  mtos_event_handle_t *p_h_event = NULL;
  p_h_event = mtos_get_comp_handler(MTOS_EVENT);
  p_h_event->create = nos_event_create;
  p_h_event->destroy = nos_event_destroy;
  p_h_event->wait = nos_event_wait;
  p_h_event->set = nos_event_set;
  p_h_event->query = nos_event_query;
}
