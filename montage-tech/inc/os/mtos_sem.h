/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MTOS_SEM_H__ 
#define __MTOS_SEM_H__ 
#ifdef __cplusplus
extern "C" {
#endif
/*!
  Delete semaphore ONLY if no task pending
  */
#define  MTOS_DEL_NO_PEND            0
/*!
  Deletes the semaphore even if tasks are waiting.
  */
#define  MTOS_DEL_ALWAYS             1


#define SEM_WAIT_FOREVER  0
#define SEM_NO_WAIT  (-2)


/*!
  define the semaphore type
  */
typedef unsigned int os_sem_t;

/*!
  Create a semaphore
  
  \param[out] p_sem the semaphore ID created
  \param[in] mutex if it is a mutex type

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_create(os_sem_t *p_sem, BOOL mutex);

/*!
  Destroy a semaphore
  
  \param[in] p_sem the semaphore ID to destroy
  \param[in] opt delete option:
            opt == MTOS_DEL_NO_PEND   Delete semaphore ONLY if no task pending
            opt == MTOS_DEL_ALWAYS    Deletes the semaphore even if tasks are waiting.In this case, all the tasks pending will be readied.

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_destroy(os_sem_t *p_sem, u8 opt);

/*!
  Singal a semaphore
  
  \param[in] p_sem the pointer to semaphore to signal

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_give(os_sem_t *p_sem);

/*!
  Wait for a semaphore
  
  \param[in] p_sem the pointer to semaphore to wait
  \param[in] ms time out value in ms, 0: wait forever, 

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_take(os_sem_t *p_sem, u32 ms);



/*!
  query semaphore count
  
  \param[in] p_sem the pointer to semaphore to wait
  \param[out] p_cnt current count of this semaphore 
  \param[out] p_pending to see if has task pending on this semaphore

  \return TRUE if OK, FALSE if fail
  */
BOOL mtos_sem_query(os_sem_t *p_sem, u32 *p_cnt, BOOL *p_pending);
#ifdef __cplusplus
}
#endif
#endif //__MTOS_SEM_H__
