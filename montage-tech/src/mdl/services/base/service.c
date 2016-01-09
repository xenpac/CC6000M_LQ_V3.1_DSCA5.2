/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_printk.h"

#include "mdl.h"
#include "service.h"

/*!
  how many service can be stored in container
  */
#define SVC_CONTAINER_DEPTH 10

/*!
  message queue interval
  */
#define SVC_MSG_Q_INTERVAL 10

/*!
  Service container's private data
  */
typedef struct
{
  /*!
    Service handle list
    */
  service_t *svc_list[SVC_CONTAINER_DEPTH];
  /*!
    How many services in this container
    */
  u32 svc_num;
  /*!
    Stored message queue id created for this container
    */
  u32 msgq_id;
  /*!
    Message queue depth
    */
  u32 msgq_depth;
  /*!
    Semphore for protecting private data access
    */
  os_sem_t sem;
} svc_container_priv_t;


/*!
  Service class private data
  */
typedef struct
{
  /*!
    Context of this service
    */
  u32 context;
  /*!
    Data buffer which helps user to store data
    */
  void *p_data_buffer;
  /*!
    Message queue id of service container
    */
  u32 msgq_id;
} svc_priv_t;

/*!
  Method to notify message out to ap framework. Do not override this function

  \param[in] handle Class handle
  \param[in] p_msg Message data
  */
static void svc_notify(handle_t handle, os_msg_t *p_msg)
{
  service_t *p_this = (service_t *)handle;
  svc_priv_t *p_priv = (svc_priv_t *)p_this->p_data;
  p_msg->extand_data = p_priv->context;
  mdl_send_msg(p_msg);
//  mtos_task_sleep(10);
}

/*!
  Pass command to message queue

  \param[in] handle Service handle
  \param[in] cmd_id Command id defined by derived service
  \param[in] p1 Parameter defined by derived service
  \param[in] p2 Parameter defined by derived service
  */
static void svc_do_cmd(handle_t handle, u32 cmd_id, u32 p1, u32 p2)
{
  os_msg_t msg = {0};
  service_t *p_this = (service_t *)handle;
  svc_priv_t *p_priv = (svc_priv_t *)p_this->p_data;
  BOOL ret = FALSE;
  
  MT_ASSERT(p_priv->msgq_id != INVALID_MSGQ_ID);
  msg.content = cmd_id;
  msg.para1 = p1;
  msg.para2 = p2;
  if(p_this->on_decorate_cmd != NULL)
  {
    p_this->on_decorate_cmd(handle, &msg);
  }
  msg.extand_data = p_priv->context;
  
  ret = mtos_messageq_send(p_priv->msgq_id, &msg);
  if(!ret)
  {
    mtos_message_dump(p_priv->msgq_id);
    MT_ASSERT(0);
  }
}

/*!
  Allocate data buffer

  \param[in] handle Service handle
  \param[in] size Memory size need to be allocated
  */
static void * svc_allocate_data_buffer(handle_t handle, u32 size)
{
  service_t *p_this = (service_t *)handle;
  svc_priv_t *p_priv = (svc_priv_t *)p_this->p_data;
  void *p_buffer = mtos_malloc(size);
  
  MT_ASSERT(p_buffer != NULL);
  p_priv->p_data_buffer = p_buffer;
  return p_buffer;
}

/*!
  Get data buffer

  \param[in] handle Service handle

  \return Return address which allocated before  
  */
static void * svc_get_data_buffer(handle_t handle)
{
  service_t *p_this = (service_t *)handle;
  svc_priv_t *p_priv = (svc_priv_t *)p_this->p_data;
  MT_ASSERT(p_priv->p_data_buffer != NULL);
  return p_priv->p_data_buffer;
}

/*!
  Release data buffer

  \param[in] handle Service handle
  */
static void svc_release_data_buffer(handle_t handle)
{
  service_t *p_this = (service_t *)handle;
  svc_priv_t *p_priv = (svc_priv_t *)p_this->p_data;
  MT_ASSERT(p_priv->p_data_buffer != NULL);
  mtos_free(p_priv->p_data_buffer);
  p_priv->p_data_buffer = NULL;
}

handle_t construct_svc(u32 context)
{
  service_t *p_this = mtos_malloc(sizeof(service_t));
  MT_ASSERT(p_this != NULL);

  p_this->msg_proc = NULL;
  p_this->sm_proc = NULL;
  p_this->allocate_data_buffer = svc_allocate_data_buffer;
  p_this->get_data_buffer = svc_get_data_buffer;
  p_this->release_data_buffer = svc_release_data_buffer;
  p_this->do_cmd = svc_do_cmd;
  p_this->on_decorate_cmd = NULL;
  p_this->notify = svc_notify;
  p_this->p_data = mtos_malloc(sizeof(svc_priv_t));
  MT_ASSERT(p_this->p_data != NULL);
  ((svc_priv_t *)p_this->p_data)->context = (context | SERVICE_MSG_MASK);
  ((svc_priv_t *)p_this->p_data)->msgq_id = INVALID_MSGQ_ID;
  return p_this;
}
/*!
  General service task entry for services

  \param[in] p_param Service container handle
  */
static void svc_task_entry(void *p_param)
{
  svc_container_t *p_this = (svc_container_t *)p_param;
  svc_container_priv_t *p_priv = p_this->p_data;
  os_msg_t msg = {0};
  u32 i = 0;
  service_t *p_svc = NULL;
  svc_priv_t *p_svc_priv = NULL;
  while(1)
  {
    mtos_sem_take(&p_priv->sem, 0);

#ifndef  __LINUX__
    if(p_priv->msgq_depth > 0
      && mtos_messageq_receive(p_priv->msgq_id, &msg, SVC_MSG_Q_INTERVAL))
#else
    if(p_priv->msgq_depth > 0
      && mtos_messageq_receive(p_priv->msgq_id, &msg, SVC_MSG_Q_INTERVAL * 10))
#endif
    {
      for(i = 0; i < p_priv->svc_num; i++)
      {
        p_svc = p_priv->svc_list[i];
        p_svc_priv = p_svc->p_data;
        if(p_svc_priv->context == msg.extand_data)
        {
          p_svc->msg_proc(p_svc, &msg);
        }
      }
    }
    else
    {
      for(i = 0; i < p_priv->svc_num; i++)
      {
        p_svc = p_priv->svc_list[i];
        p_svc->sm_proc(p_svc);
      }
    }
    mtos_sem_give(&p_priv->sem);
    //mtos_task_sleep(100);
  }
}

/*!
  Lock container

  \param[in] handle Service container class handle
  */
static void svc_container_lock(handle_t handle)
{
  svc_container_t *p_this = (svc_container_t *)handle;
  svc_container_priv_t *p_priv = p_this->p_data;
  mtos_sem_take(&p_priv->sem, 0);
}

/*!
  Unlock container

  \param[in] handle Service container class handle
  */
static void svc_container_unlock(handle_t handle)
{
  svc_container_t *p_this = (svc_container_t *)handle;
  svc_container_priv_t *p_priv = p_this->p_data;
  mtos_sem_give(&p_priv->sem);
}

/*!
  Get server list in the service container 

  \param[in] handle Service container class handle
  \param[out] total server num
  \return server list address
  */
u8 svc_container_get_svc_list(handle_t handle, void **p_svc_list)
{
  svc_container_t *p_this = (svc_container_t *)handle;
  svc_container_priv_t *p_priv = p_this->p_data;
  u8 i = 0;

  p_this->lock(handle);
  for (i = 0; i < p_priv->svc_num; i++)
  {
    p_svc_list[i] = p_priv->svc_list[i];
  }
  p_this->unlock(handle);
  return p_priv->svc_num;
}

/*!
  Add a service to service container

  \param[in] handle Service container class handle
  \param[in] context context
  */
service_t *svc_container_get_svc(handle_t handle, u32 context)
{
  svc_container_t *p_this = (svc_container_t *)handle;
  svc_container_priv_t *p_priv = p_this->p_data;
  service_t *p_found_svc = NULL;
  service_t *p_svc = NULL;
  u32 svc_context = 0;
  u32 i = 0;

  //p_this->lock(handle);
  for(i = 0; i < p_priv->svc_num; i++)
  {
    p_svc = p_priv->svc_list[i];
    svc_context = ((svc_priv_t *)p_svc->p_data)->context;
    if((svc_context & ~SERVICE_MSG_MASK) == context)
    {
      p_found_svc = p_svc;
      break; 
    }
  }
  //p_this->unlock(handle);
  return p_found_svc;
}

/*!
  Add a service to service container

  \param[in] handle Service container class handle
  \param[in] p_svc Service class handle
  */
void svc_container_add_svc(handle_t handle, service_t *p_svc)
{
  svc_container_t *p_this = (svc_container_t *)handle;
  svc_container_priv_t *p_priv = p_this->p_data;
  MT_ASSERT(p_priv->svc_num < SVC_CONTAINER_DEPTH);
  MT_ASSERT(p_svc != NULL);
  p_this->lock(handle);
  ((svc_priv_t *)p_svc->p_data)->msgq_id = p_priv->msgq_id;
  p_priv->svc_list[p_priv->svc_num] = p_svc;
  p_priv->svc_num++;
  p_this->unlock(handle);
}

/*!
  Remove a service from service container

  \param[in] handle Service container class handle
  \param[in] p_svc Service class handle
  */
void svc_container_remove_svc(handle_t handle, service_t *p_svc)
{
  svc_container_t *p_this = (svc_container_t *)handle;
  svc_container_priv_t *p_priv = p_this->p_data;
  u32 i = 0;
  u32 j = 0;
  MT_ASSERT(p_svc != NULL);
  p_this->lock(handle);
  for(i = 0; i < p_priv->svc_num; i++)
  {
    if(p_priv->svc_list[i] == p_svc)
    {
      break;
    }
  }
  MT_ASSERT(i < p_priv->svc_num);
  for(j = i + 1; j < p_priv->svc_num; j++, i++)
  {
    p_priv->svc_list[i] = p_priv->svc_list[j];
  }
  p_priv->svc_num--;
  p_this->unlock(handle);
}

/*!
  Get the message queue id of service container. This method is for some
  services need attach extand memory buffer to message queue. For big message
  usage please refer design document of mtos message queue .

  \param[in] handle Service container class handle
  \param[in] p_svc Service class handle
  */
u32 svc_container_get_msgq_id(handle_t handle)
{
  svc_container_t *p_this = (svc_container_t *)handle;
  svc_container_priv_t *p_priv = p_this->p_data;
  return p_priv->msgq_id;
}

handle_t construct_svc_container(const char *p_name, u32 priorty,
  u32 stack_size, u32 msgq_depth)
{
  BOOL res = FALSE;
  svc_container_t *p_this = mtos_malloc(sizeof(svc_container_t));
  u8 *p_stack = mtos_malloc(stack_size);
  svc_container_priv_t *p_priv = mtos_malloc(sizeof(svc_container_priv_t));

  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_stack != NULL);
  MT_ASSERT(p_priv != NULL);

  p_this->get_svc = svc_container_get_svc;
  p_this->get_svc_list = svc_container_get_svc_list;
  p_this->add_svc = svc_container_add_svc;
  p_this->remove_svc = svc_container_remove_svc;
  p_this->lock = svc_container_lock;
  p_this->unlock = svc_container_unlock;
  p_this->get_msgq_id = svc_container_get_msgq_id;
  p_this->p_data = p_priv;

  p_priv->svc_num = 0;
  MT_ASSERT(msgq_depth > 0);
  p_priv->msgq_depth = msgq_depth;
  p_priv->msgq_id = INVALID_MSGQ_ID;
  p_priv->msgq_id = mtos_messageq_create(msgq_depth, (u8 *)p_name);

  mtos_sem_create(&p_priv->sem, TRUE);
  res = mtos_task_create((u8 *)p_name, svc_task_entry, (void *)p_this,
    priorty, (u32 *)p_stack, stack_size);
  MT_ASSERT(TRUE == res);
  
  return p_this;
}

