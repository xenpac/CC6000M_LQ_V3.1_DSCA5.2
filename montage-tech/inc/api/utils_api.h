/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __UTILS_API_H_
#define __UTILS_API_H_

/*!
  FIXME
  */
typedef enum
{
/*!
  FIXME
  */
  MUL_UTILS_STATE_INVALID,
  /*!
  FIXME
  */
  MUL_UTILS_PARSE_DONE,
    /*!
  FIXME
  */
  MUL_UTILS_PARSE_ERROR,

}mul_utils_state_t;

/*!
  FIXME
  */
typedef struct
{
/*!
  FIXME
  */
  u32 stk_size;
}mul_utils_attr_t;

/*!
  FIXME
  */
typedef void(*utils_call_back)(mul_utils_state_t event_type, s32 event_value, void *p_args);
/*!
 * Create filter
 */
RET_CODE mul_utils_init(void);
/*!
 * Create filter
 */
RET_CODE mul_utils_deinit(void);
/*!
 * Create filter
 */
RET_CODE mul_utils_create(mul_utils_attr_t *p_attr);
/*!
 * Create filter
 */
RET_CODE mul_utils_destroy_chn(void);
/*!
 * Create filter
 */
RET_CODE mul_utils_start(void);
/*!
 * Create filter
 */
RET_CODE mul_utils_stop(void);
/*!
 * Create filter
 */
RET_CODE mul_utils_config(void);
/*!
 * Create filter
 */
RET_CODE mul_utils_register_event(utils_call_back callBack, void *p_args);
/*!
 * Create filter
 */
RET_CODE mul_utils_unregister_event(void);


#endif //__utils_API_H_

