/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __WEATHER_API_H_
#define __WEATHER_API_H_

/*!
  FIXME
  */
typedef enum
{
/*!
  FIXME
  */
  MUL_WEATHER_STATE_INVALID,
  /*!
  FIXME
  */
  MUL_WEATHER_PARSE_DONE,
  /*!
  FIXME
  */
  MUL_WEATHER_DATA_ERROR,
    /*!
  FIXME
  */
  MUL_WEATHER_PARSE_ERROR,
    /*!
  FIXME
  */
  MUL_WEATHER_CITY_ERROR,
}mul_weather_state_t;

/*!
  FIXME
  */
typedef struct
{
/*!
  FIXME
  */
  u32 stk_size;
}mul_weather_attr_t;

/*!
  FIXME
  */
typedef void(*weather_call_back)(mul_weather_state_t event_type, s32 event_value, void *p_args);
/*!
 * Create filter
 */
RET_CODE mul_weather_init(void);
/*!
 * Create filter
 */
RET_CODE mul_weather_deinit(void);
/*!
 * Create filter
 */
RET_CODE mul_weather_create(mul_weather_attr_t *p_attr);
/*!
 * Create filter
 */
RET_CODE mul_weather_destroy_chn(void);
/*!
 * Create filter
 */
RET_CODE mul_weather_start(void);
/*!
 * Create filter
 */
RET_CODE mul_weather_stop(void);
/*!
 * Create filter
 */
RET_CODE mul_weather_config(weather_input_t *p_input, weather_web_t weather_web);
/*!
 * Create filter
 */
RET_CODE mul_weather_register_event(weather_call_back callBack, void *p_args);
/*!
 * Create filter
 */
RET_CODE mul_weather_unregister_event(void);


#endif //__WEATHER_API_H_

