/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __CAS_PORTING_DRV_OS_H__
#define __CAS_PORTING_DRV_OS_H__

BOOL cas_porting_drv_create_semaphore(u32* pSemaphore,BOOL ulInitialCount);

BOOL cas_porting_drv_give_semaphore(u32* pSemaphore);

BOOL cas_porting_drv_wait_semaphore(u32* pSemaphore, u32 WaitMs);

BOOL cas_porting_drv_delete_semaphore(u32* pSemaphore);

void cas_porting_drv_task_lock(void);

void cas_porting_drv_task_unlock(void);

u32 cas_porting_drv_get_osticks(void);

void cas_porting_drv_task_sleep(u32 ms);

u32 cas_porting_drv_register_task(char* szName,
                                       u8     byPriority,
                                       void*  pTaskFun,
                                       void*  pParam,
                                       u16    wStackSize);
#endif

