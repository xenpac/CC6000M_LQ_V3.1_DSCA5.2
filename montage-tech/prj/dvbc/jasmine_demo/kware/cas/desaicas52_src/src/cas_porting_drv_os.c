/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_mem.h"


#include "cas_porting_drv_os.h"
#if 1
#define MAX_CAS_TASK_NUM           9
typedef enum _cas_task_status
{
    CAS_TASK_STATUS_FREE,
    CAS_TASK_STATUS_BUSY
}cas_task_status_e;

typedef struct _cas_task_struct
{
    u32 task_prio;
    cas_task_status_e task_status;
}cas_task_struct_t;

static cas_task_struct_t cas_task_record[MAX_CAS_TASK_NUM] =
{
    {0, CAS_TASK_STATUS_FREE},
    {0, CAS_TASK_STATUS_FREE},
    {0, CAS_TASK_STATUS_FREE},
    {0, CAS_TASK_STATUS_FREE},
    {0, CAS_TASK_STATUS_FREE},
    {0, CAS_TASK_STATUS_FREE},
};

u32 cas_porting_drv_register_task(char* szName,
                                       u8     byPriority,
                                       void*  pTaskFun,
                                       void*  pParam,
                                       u16    wStackSize)
{
    BOOL ret = 0;
    //u32 ret = 0;
    u8 uIndex = 0;
    u32 *p_stack = NULL;
    
    MT_ASSERT(NULL != szName);
    MT_ASSERT(NULL != pTaskFun);

    for(uIndex = 0; uIndex < MAX_CAS_TASK_NUM; uIndex++)
    {
        if(cas_task_record[uIndex].task_prio == byPriority)
        {
            OS_PRINTF("task priority[%d] has been registed!\n", byPriority);
            return (u32)(ERR_FAILURE);
        }
        if(CAS_TASK_STATUS_FREE == cas_task_record[uIndex].task_status)
        {
            cas_task_record[uIndex].task_status = CAS_TASK_STATUS_BUSY;
            break;
        }
    }

    if(uIndex >= MAX_CAS_TASK_NUM)
    {
        OS_PRINTF("Register task too many task or invalid task priority!\n");
        return(u32)( ERR_FAILURE);
    }
    
    cas_task_record[uIndex].task_prio = byPriority;

    p_stack = (u32 *)mtos_malloc(wStackSize);
    MT_ASSERT(NULL != p_stack);

    ret = mtos_task_create((u8 *)szName,
                                (void *)pTaskFun,
                                (void *)pParam,
                                byPriority,
                                p_stack,
                                wStackSize);
    if(!ret)
    {
        OS_PRINTF("Register task error = 0x%08x!\n", ret);
        cas_task_record[uIndex].task_status = CAS_TASK_STATUS_FREE;
        cas_task_record[uIndex].task_prio = 0;
        return (u32)(ERR_FAILURE);
    }
    OS_PRINTF("Register task success! Name[%s], Priority[%d]\n", 
                szName, cas_task_record[uIndex].task_prio);

    return (u32)(SUCCESS);
}

#endif

BOOL cas_porting_drv_create_semaphore(u32* pSemaphore,BOOL ulInitialCount)
{
	/* We need to modify mtos_sem_create function to support the second parameter */
    return mtos_sem_create((os_sem_t *)pSemaphore, ulInitialCount);
}


BOOL cas_porting_drv_give_semaphore(u32* pSemaphore)
{
    return mtos_sem_give((os_sem_t *)pSemaphore);
}


BOOL cas_porting_drv_wait_semaphore(u32* pSemaphore, u32 WaitMs)
{
    return mtos_sem_take((os_sem_t *)pSemaphore, WaitMs);
}


BOOL cas_porting_drv_delete_semaphore(u32* pSemaphore)
{
  return mtos_sem_destroy((os_sem_t *)pSemaphore, MTOS_DEL_ALWAYS);
}


void cas_porting_drv_task_lock(void)
{
  mtos_task_lock();
}

void cas_porting_drv_task_unlock(void)
{
  mtos_task_unlock();
}


u32 cas_porting_drv_get_osticks(void)
{
  return mtos_ticks_get();
}

void cas_porting_drv_task_sleep(u32 ms)
{
  mtos_task_sleep(ms);
  return;
}



