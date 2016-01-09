/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MTOS_TASK_H__
#define __MTOS_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
    Common  task  info handle
  */
typedef struct 
{
   /*!
      The task priority
     */
   u8 prio;
   /*!
      place holder for 32bits aligned
     */
   u8 reserve[3];
   /*!
       the  task stack porinter
     */
   u32 *p_stack;
   /*!
       the  task stack size in 4 bytes
     */
   u32 nstksize;

   /*!
       the task stack used 
      */
   u32 nstkused;
}mtos_task_info_t;


/*!
  Create a new task

  \param[in] p_taskname the task name
  \param[in] p_taskproc the task main process function
  \param[in] p_param parameters for task process function
  \param[in] nprio task priority
  \param[in] pstk pointer to task stack
  \param[in] nstksize ths task stack size

  \return TRUE if success, FALSE if fail
  */
BOOL mtos_task_create(u8   *p_taskname,
                     void (*p_taskproc)(void *p_param),
                     void  *p_param,
                     u32    nprio,
                     u32   *pstk,
                     u32    nstksize);

/*!
  Delete a task

  \param[in] ms the time to sleep, unit: ms
  \return 0 if success, other fail
  */
u32 mtos_task_exit(void);


/*!
  Suspend a task for some time

  \param[in] ms the time to sleep, unit: ms
  */
void mtos_task_sleep(u32 ms);

/*!
  Lock a task, the os will not dispatch the tasks any longer

  \param[in] None
  */
void mtos_task_lock(void);

/*!
  Unlock the task, and the os will start dispatch again

  \param[in] None
  */
void mtos_task_unlock(void);

/*!
  Resume task

  \param[in] prio Task priority
   \return 0 if success, other fail
  */
u32 mtos_task_resume(u8 prio);

/*!
  Suspend task

  \param[in] prio Task priority
   \return 0 if success, other fail
  */
u32 mtos_task_suspend(u8 prio);

/*!
  change task priority

  \param[in] oldprio Task priority
  \param[in] newprio Task priority
  \return 0 if success, other fail
  */
u32 mtos_task_change_prio(u8 oldprio, u8 newprio);

/*!
  get task info

  \param[in/out] p_info handle of mtos_task_info_t,
                 if p_info->prio is 0, get current task info
                 else get the p_info->prio's task info
  \return 0 if success, other fail
  */
u32 mtos_task_get_info(mtos_task_info_t *p_info);

/*!
  delet task 

  \param[in] prio  Task priority 
  \return 0 if success, other fail
  */
u32 mtos_task_delete(u8 prio);


/*!
  get the current task priority

  \return the task priority
  */
u8 mtos_task_get_curn_prio(void);

/*!
  get the os version

  \return the os version
  */
u32  mtos_task_get_os_version(void);

#ifdef __cplusplus
}
#endif

#endif //__MTOS_TASK_H__

