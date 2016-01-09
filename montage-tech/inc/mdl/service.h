/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SERVICE_H_
#define __SERVICE_H_

/*!
  Service send to mask
  */
#define SERVICE_MSG_MASK (0x02000000)

/*!
  Service task sleep time(ms) for each loop
  */
#define SERVICE_HEART_BEAT (100)

/*!
  Service class declaration
  */
typedef struct
{
  /*!
    \see svc_notify
    */
  void (*notify)(handle_t handle, os_msg_t *p_msg);
  /*!
    \see svc_allocate_data_buffer
    */
  void * (*allocate_data_buffer)(handle_t handle, u32 size);
  /*!
    \see svc_get_data_buffer
    */
  void * (*get_data_buffer)(handle_t handle);
  /*!
    \see svc_release_data_buffer
    */
  void (*release_data_buffer)(handle_t handle);
  /*!
    Set command to service
    */
  void (*do_cmd)(handle_t handle, u32 cmd_id, u32 p1, u32 p2);
  /*!
    virtual function to process messages
    */
  void (*msg_proc)(handle_t handle, os_msg_t *p_msg);
  /*!
    virtual function to process state machine
    */
  void (*sm_proc)(handle_t handle);
  /*!
    virtual function to decorate command message
    */
  void (*on_decorate_cmd)(handle_t handle, os_msg_t *p_msg);
  /*!
    Class data
    */
  void *p_data;
} service_t;

/*!
  Service container class declaration
  */
typedef struct
{
  /*!
    \see svc_container_get_svc
    */
  service_t * (*get_svc)(handle_t handle, u32 context);
  /*!
    \see svc_container_get_svc_list
    */
  u8 (*get_svc_list)(handle_t handle, void **p_svc_list);
  /*!
    \see svc_container_add_svc
    */
  void (*add_svc)(handle_t handle, service_t *p_svc);
  /*!
    \see svc_container_remove_svc
    */
  void (*remove_svc)(handle_t handle, service_t *p_svc);
  /*!
    \see svc_container_lock
    */
  void (*lock)(handle_t handle);
  /*!
    \see svc_container_unlock
    */
  void (*unlock)(handle_t handle);
  /*!
    \see svc_container_get_msgq_id
    */
  u32 (*get_msgq_id)(handle_t handle);
  /*!
    Class data
    */
  void *p_data;
} svc_container_t;

/*!
  Constructor of service

  \param[in] Context handle, it defined by outside module
  
  \return Class handle
  */
handle_t construct_svc(u32 context);

/*!
  Constructor of service container

  \param[in] p_name Module name
  \param[in] priority Task priority
  \param[in] stack_size Task stack size
  \param[in] msgq_depth Message queue depth, 0 means no do not create message
                        queue for me
  
  \return Class handle
  */
handle_t construct_svc_container(const char *p_name, u32 priority,
  u32 stack_size, u32 msgq_depth);

#endif // End for __SERVICE_H_

