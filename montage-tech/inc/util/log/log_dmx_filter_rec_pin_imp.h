/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_DMX_FILTER_REC_PIN_IMP_H__
#define __LOG_DMX_FILTER_REC_PIN_IMP_H__

/*!
  change dmx filter rec pin check point
  */
typedef enum
{
  /*!
  check get payload
  */
  TAG_GET_PALYLOAD,
  /*!
  check split transform one_payload start
  */
  TAG_SPLIT_TRANSFOR_START,
  /*!
  check split transform one_payload end
  */
  TAG_SPLIT_TRANSFOR_END,
}dmx_filter_rec_pin_timepoint_t;

 /*!
   init
 */
handle_t log_dmx_filter_rec_pin_init(void);

#endif // End for __LOG_DMX_FILTER_REC_PIN_IMP_H__

