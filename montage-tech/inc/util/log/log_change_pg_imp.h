/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_CHANGE_PG_H__
#define __LOG_CHANGE_PG_H__

/*!
  change pg performace check point
  */
typedef enum
{
/*!
  check last pg stop,start change pg
*/
  TAB_PB_SWITCH_PG,
/*!
  check pb do stop for several steps
*/
  TAB_PB_DO_STOP,
 /*!
  check pb do play
*/
  TAB_PB_PLAY,
  /*!
  check pb do play for several steps
*/
  TAB_PB_DO_PLAY,
/*!
  check start to lock turner 
*/
  TAB_LOCK_TURNER_START,
  /*!
  check turner locked 
*/
  TAB_LOCK_TURNER_END,
 /*!
  check wait video stable
*/
  TAB_WAIT_VIDEO_STABLE,
/*!
  check wait video sync
*/
  TAB_WAIT_VIDEO_SYNC,
 /*!
  check wait video end
*/   
  TAB_WAIT_VIDEO_END,
  
}change_pg_perf_timepoint_t;

 /*!
   init
 */
handle_t log_ap_change_pg_init(void);

#endif // End for __LOG_CHANGE_PG_H__

