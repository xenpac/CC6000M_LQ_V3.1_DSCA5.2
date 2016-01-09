/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_CHANGE_PG_IMP_H__
#define __LOG_CHANGE_PG_IMP_H__

/*!
  pic_render_filter performace check point
  */
typedef enum
{
/*!
  check start ticks
*/
  PIC_RENDER_FILTER_START_TICKS,
 /*!
  check stop ticks
*/
  PIC_RENDER_FILTER_STOP_TICKS,
}pic_render_filter_perf_timepoint_t;

 /*!
   init
 */
handle_t log_pic_render_filter_init(void);

#endif // End for __LOG_AP_SCAN_H__

