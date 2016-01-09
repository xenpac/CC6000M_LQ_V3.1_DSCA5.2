/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MTOS_MISC_H__
#define __MTOS_MISC_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
  mtos configuration
  */
typedef struct mtos_cfg
{
/*!
  enable bhr or not
  */
  BOOL enable_bhr;
/*!
  the bhr max routine number, it is valid only if enable_bhr is TRUE 
  */
  u32 max_routine_num;
/*!
  the bhr task priority, it is valid only if enable_bhr is TRUE 
  */
  u32 bhr_prio;
/*!
  the bhr task stack pointer, it is valid only if enable_bhr is TRUE 
  */
  u32 bhr_stack_ptr;
/*!
  the bhr task stack size, it is valid only if enable_bhr is TRUE 
  */
  u32 bhr_stack_size;
}mtos_cfg_t;

/*!
  Must be called at the beginning for os
  */
void mtos_os_init(mtos_cfg_t *p_cfg);
/*!
  Must be called at the beginning for os
  */
void nucleus_os_init(void);
/*!
  start os multi-threads, Before you can call mtos_start(), you MUST have called mtos_os_init(),
  and you MUST have created at least one task.
  */
void mtos_start(void);

/*!
  start mtos statistic task, it MUST be called in the first task you created, and before you start another task!
  */
void mtos_stat_init(void);

/*!
  Init os timer, and it will consume 2 hardware time now, timer 0 for OS scheduling, timer 1 for task delay
  
  \param[in] None

  \return version number
  */
BOOL mtos_ticks_init(u32 cpu_freq);

/*!
  Get os schedule tick(timer 0), unit: 10ms
  
  \param[in] None

  \return os tick
  */
u32 mtos_ticks_get(void);

/*!
  Get os schedule tick(timer 0), unit: 10ms

  \return mtos cpu frequency registered in mtos-ticks_init
  */
u32 mtos_cpu_freq_get(void);

/*!
  Get current hardware timer(timer 1) value, 1 tick means 1 CPU clock, time = 1/mtos_cpu_freq second
  
  \param[in] None

  \return hardware tick
  */
u32 mtos_hw_ticks_get(void);

/*!
  Get current sys time:(s:ms:us)
  
  \param[out] s
  \param[out] ms 
  \param[out] us
  \return sys time
  */
void mtos_systime_get(u32 *s, u32 *ms, u32 *us);

/*!
  Get current hardware timer(timer 1) overflow count, time = 0xFFFFFFFF/mtos_cpu_freq macro second
  
  \param[in] None

  \return os tick
  */
u32 mtos_hw_loop_get(void);

/*!
  Delay a task for some time, unit: ms
  
  \param[in] ms the time to sleep, unit: ms
  */
void mtos_task_delay_ms(u32 ms);

/*!
  Delay a task for some time, unit: us
  
  \param[in] ms the time to sleep, unit: us
  */
void mtos_task_delay_us(u32 us);


/*!
  Start critical section protection
  
  \param[in] p_sr local varialble to save current SR
  */
void mtos_critical_enter(u32 *p_sr);

/*!
  Exit critical section protection
  
  \param[in] sr local varialble used by mtos_critical_enter
  */
void mtos_critical_exit(u32 sr);
#ifdef __cplusplus
}
#endif
 

#endif //__MTOS_MISC_H__
