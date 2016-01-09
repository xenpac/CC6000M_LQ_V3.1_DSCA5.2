/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __SYS_CFG_H_
#define __SYS_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

/*!
   Config DVB Engine Module ...
 */


#define SYSTEM_MEMORY_END   (8 * 1024 * 1024)
#define SYS_PARTITION_SIZE (4 * 1024 *1024)
#define SYS_PARTITION_ATOM           64

#define DMA_PARTITION_SIZE       30 * 1024 * 1024                                                
#define DMA_PARTITION_ATOM               64

#define SYS_INIT_TASK_STK_SIZE           (16 * KBYTES)
#define SYS_TIMER_TASK_STK_SIZE          (16 * KBYTES)
#define SYS_APP_TASK_STK_SIZE            (16 * KBYTES)

#define SYS_INIT_TASK_PRIORITY      2
#define SYS_TIMER_TASK_PRIORITY     3
#define SYS_APP_TASK_PRIORITY       9

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __SYS_CFG_H_
