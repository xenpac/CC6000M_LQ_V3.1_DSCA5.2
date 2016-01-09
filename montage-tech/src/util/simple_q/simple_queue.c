/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
  \file simple_queue.c

  This file implement a simple queue manager
  */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "simple_queue.h"
#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "err_check_def.h"

/*!
  support three queue to do fast search
  */
#define MAX_SIMPLE_Q_NUM 50

/*!
  Simple queue structure
  */
typedef struct
{
  /*!
    queue depth
    */
  u16 depth;
  /*!
    First element position
    */
  u16 head;
   /*!
    Last element position
    */
  u16 tail;
   /*!
    Element number
    */
  u16 element_num;
  /*!
    Managed buffer address
    */
  u16 *p_buffer;
} simple_q_t;

/*!
  Simple queue private data
  */
typedef struct
{
  simple_q_t simple_q[MAX_SIMPLE_Q_NUM];
}simple_q_priv_t;

/*!
  Module initialize function

  \return TRUE middleware handle
  */
class_handle_t simple_queue_init(void)
{
  simple_q_priv_t *p_priv = mtos_malloc(sizeof(simple_q_priv_t));
  CHECK_FAIL_RET_NULL(p_priv != NULL);
  memset(p_priv, 0, sizeof(simple_q_priv_t));
  logger_init();
  class_register(SIMPLE_Q_CLASS_ID, (class_handle_t)p_priv);
  return (class_handle_t)p_priv;
}

/*!
  Create a simple queue

  \param[in] handle simple_queue's handle
  \param[in] depth Depth of this queue
  \param[in] p_buffer Memory address for this queue
  \param[out] p_used_size this queue used buffer size

  \return Return queue id otherwise assertion failed
  */
u8 create_simple_queue(class_handle_t handle,
  u16 depth, u8 * p_buffer, u32 *p_used_size)
{
  u8 i = 0;
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  simple_q_t *p_queue = NULL;
  
  CHECK_FAIL_RET((p_buffer != NULL), 0xFF);

  for(i = 0; i < MAX_SIMPLE_Q_NUM; i ++)
  {
    p_queue = p_priv->simple_q + i;
    if(NULL == p_queue->p_buffer)
    {
      p_queue->depth = depth;
      p_queue->head = 0;
      p_queue->tail = 0;
      p_queue->element_num = 0;
      p_queue->p_buffer = (u16 *)p_buffer; 
      memset(p_buffer, 0, sizeof(u16)*depth);
      *p_used_size = sizeof(u16)*depth;
      return i;
    }
  }

  // No more queue, assertion failed
  CHECK_FAIL_RET(0, 0xFF);
  return 0xFF;
}

u8 create_simple_queue1(class_handle_t handle, u16 depth)
{
  u8 i = 0;
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  simple_q_t *p_queue = NULL;
  
  for(i = 0; i < MAX_SIMPLE_Q_NUM; i ++)
  {
    p_queue = p_priv->simple_q + i;
    if(NULL == p_queue->p_buffer)
    {
      p_queue->depth = depth;
      p_queue->head = 0;
      p_queue->tail = 0;
      p_queue->element_num = 0;
      p_queue->p_buffer = mtos_malloc(sizeof(u16) * depth); 
      CHECK_FAIL_RET((p_queue->p_buffer != NULL), 0xFF);
      memset(p_queue->p_buffer, 0, sizeof(u16) * depth);
      return i;
    }
  }

  // No more queue, assertion failed
  CHECK_FAIL_RET(0, 0xFF);
  return 0xFF;
}

/*!
  Destory the simple queue, clear all info

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  */
void destory_simple_queue(class_handle_t handle, u8 id)
{
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  CHECK_FAIL_RET_VOID((id < MAX_SIMPLE_Q_NUM) && (p_priv->simple_q[id].p_buffer != NULL));

  p_priv->simple_q[id].p_buffer = NULL;
}

void destory_simple_queue1(class_handle_t handle, u8 id)
{
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  CHECK_FAIL_RET_VOID((id < MAX_SIMPLE_Q_NUM) && (p_priv->simple_q[id].p_buffer != NULL));

  mtos_free(p_priv->simple_q[id].p_buffer);
  p_priv->simple_q[id].p_buffer = NULL;
}


/*!
  Push an item into queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[in] value Data will be pushed into queue

  \return Return TRUE means successed, FALSE means queue was full
  */
BOOL push_simple_queue(class_handle_t handle, u8 id, u16 value)
{
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  simple_q_t *p_queue = &(p_priv->simple_q[id]);
  CHECK_FAIL_RET_FALSE((id < MAX_SIMPLE_Q_NUM) && (p_queue->p_buffer != NULL));
  
  if(p_queue->element_num < p_queue->depth)
  {
    p_queue->p_buffer[p_queue->head++] = value;
    p_queue->head = (p_queue->head < p_queue->depth) ?
      p_queue->head : 0;
    p_queue->element_num++;
    return TRUE;
  }
  else
  {
    //the queue is full
    return FALSE;
  }
}

/*!
  Push an item into queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[in] value Data will be pushed into queue

  \return Return TRUE means successed, FALSE means queue was full
  */
BOOL push_simple_queue_on_head(class_handle_t handle, u8 id, u16 value)
{
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  simple_q_t *p_queue = &(p_priv->simple_q[id]);
  CHECK_FAIL_RET_FALSE((id < MAX_SIMPLE_Q_NUM) && (p_queue->p_buffer != NULL));
  
  if(p_queue->element_num < p_queue->depth)
  {
    p_queue->tail =  (p_queue->tail > 0) ?
      (p_queue->tail - 1) : (p_queue->depth - 1);
    p_queue->p_buffer[p_queue->tail] = value;
    p_queue->element_num++;
    return TRUE;
  }
  else
  {
    //the queue is full
    return FALSE;
  }
}

/*!
  Pop a data from a queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[out] p_value Data value

  \return Return zero means successed, -1 means queue was empty
  */
BOOL pop_simple_queue(class_handle_t handle, s8 id, u16 *p_value)
{
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  simple_q_t *p_queue = &(p_priv->simple_q[id]);
  CHECK_FAIL_RET_FALSE((id < MAX_SIMPLE_Q_NUM) && (p_queue->p_buffer != NULL));
  
  if(p_queue->element_num > 0)
  {
    *p_value = p_queue->p_buffer[p_queue->tail++];
    p_queue->tail = (p_queue->tail < p_queue->depth) ?
      p_queue->tail : 0;
    p_queue->element_num--;
    return TRUE;
  }
  else
  {
    //the queue is empty
    return FALSE;
  }
}

/*!
  Get the element number from a queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id

  \return Return the element number
  */
u16 get_simple_queue_len(class_handle_t handle, u8 id)
{
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  CHECK_FAIL_RET_ZERO((id < MAX_SIMPLE_Q_NUM) && (p_priv->simple_q[id].p_buffer != NULL));
  return p_priv->simple_q[id].element_num;
}

/*!
  Get the element list from a queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[out] element_list obtain element list
  \return Return the element number
  */
u16 get_simple_queue_element(class_handle_t handle, u8 id, u16 *p_element_list)
{
  simple_q_priv_t *p_priv = (simple_q_priv_t *)handle;
  u16 num = 0;
  u16 head = 0;
  CHECK_FAIL_RET_ZERO((id < MAX_SIMPLE_Q_NUM) && (p_priv->simple_q[id].p_buffer != NULL));
  head = p_priv->simple_q[id].head;
  for(num = 0; num < p_priv->simple_q[id].element_num; num++)
  {
    p_element_list[num] = p_priv->simple_q[id].p_buffer[head];
    head++;
    head = (head < p_priv->simple_q[id].depth) ? head + 1 : 0;
  }
  return num;
}

