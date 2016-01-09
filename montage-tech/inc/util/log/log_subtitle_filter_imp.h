/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_SUBTITLE_FILTER_H__
#define __LOG_SUBTITLE_FILTER_H__

/*!
  change pg performace check point
  */
typedef enum
{
/*!
  check PAGE_SEGMENT begin
*/
  PERF_SUBT_PAGE_SEGMENT_BEGIN = 0,
 /*!
  check PAGE_SEGMENT end
*/
  PERF_SUBT_PAGE_SEGMENT_END,
 /*!
  check REGION_SEGMENT begin
*/
  PERF_SUBT_REGION_SEGMENT_BEGIN,
  /*!
  check REGION_SEGMENT end
*/
  PERF_SUBT_REGION_SEGMENT_END,
 /*!
  check CLUT_SEGMENT begin
*/   
  PERF_SUBT_CLUT_SEGMENT_BEGIN,
 /*!
  check CLUT_SEGMENT end
*/   
  PERF_SUBT_CLUT_SEGMENT_END,
 /*!
  check OBJECT_SEGMENT begin
*/   
  PERF_SUBT_OBJECT_SEGMENT_BEGIN,
 /*!
  check OBJECT_SEGMENT end
*/   
  PERF_SUBT_OBJECT_SEGMENT_END,
/*!
  check receive begin
*/
  PERF_SUB_RECIVE_BEGIN,
 /*!
  check push down end
*/
  PERF_SUB_PUSH_DOWN_END,
 /*!
  check finish
*/
  PERF_SUB_FINISH
}subtitle_filter_perf_timepoint_t;

 /*!
   init
 */
handle_t log_mdl_subtitle_filter_init(void);

#endif // End for __LOG_SUBTITLE_FILTER_H__

