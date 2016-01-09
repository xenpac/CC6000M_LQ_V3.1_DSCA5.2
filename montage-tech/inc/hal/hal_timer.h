/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

/*!
  The ID of 32-bit Timer 0
  */
#define TIMER_ID0         0x00
/*!
  The ID of 32-bit Timer 1
  */
#define TIMER_ID1         0x01
/*!
  The ID of 32-bit Timer 2
  */
#define TIMER_ID2         0x02
/*!
  The ID of 32-bit Timer 3
  */
#define TIMER_ID3         0x03


/*!
  request a timer, if success, return a valid timer id.
  
  \param[in] interval_ms timer interval in ms
  \param[in] callback the timeout callback function 
  \param[in] circular single or circular mode, TRUE-circular, FALSE-single

   \return 
        >=0: The timer id.
        < 0: Failed.
  */
s32 hal_timer_request(u32 interval_ms,void (* p_callback)(void), BOOL circular);

/*!
  Release an timer, the timer will be free and can be requested again
  
  \param[in] timer_id The timer id.
  */
void hal_timer_release(u8 timer_id);

/*!
  Start a timer
  
  \param[in] timer_id The timer id.
  */
void hal_timer_start(u8 timer_id);

/*!
  Stop a timer
  
  \param[in] timer_id The timer id.
  */
void hal_timer_stop(u8 timer_id);

/*!
  Get the current counting value of specific timer  
  
  \param[in] timer_id The timer id.
  */
u32 hal_timer_get_count(u8 timer_id);

#endif //__HAL_TIMER_H__
