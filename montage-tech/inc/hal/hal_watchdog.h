/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef   __HAL_WATCHDOG_H_
#define   __HAL_WATCHDOG_H_

/*!
  initiate the watchdog, set the feeding interval, the max timeout value: 
  max = 0xFFFFFFFF/cpu_freqency*1000 ms, if timeout_ms > max, timeout will be set to max

  \param[in] timeout_ms the watchdog feeding interval, MUST BE less than WATCHDOG_MAX_TIME_MS

  \return 0
  */
u32 hal_watchdog_init(u32 timeout_ms);

/*!
  feed the watchdog

  \param[in] None 

  \return 0
  */
u32 hal_watchdog_feed(void);

/*!
  Enable the watchdog

  \param[in] None

  \return 0
  */
u32 hal_watchdog_enable(void);

/*!
  disable the watchdog

  \param[in] None 

  \return 0
  */
u32 hal_watchdog_disable(void);

/*!
  reset the watchdog, the system will reboot

  \param[in] None

  \return 0
  */
u32 hal_watchdog_reset(void);
/*!

  */
u32 hal_watchdog_irq(void (* p_callback)(void),u32 timeout_ms);

/*!

  */
u32 hal_watchdog_release(u32 timeout_ms);

#endif    //end of __HAL_WATCHDOG_H_
