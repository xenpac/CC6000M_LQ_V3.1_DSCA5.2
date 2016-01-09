/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NET_EXTERN_API_H_
#define __NET_EXTERN_API_H_

/*!
  net extern callback
  */
typedef void(*mul_net_extern_callback)(u8 *p_url, u8 *p_data, u32 data_size);

/*!
  mul_net_extern_request_t
  */
typedef struct tag_mul_net_extern_request
{
  /*!
    p_url
    */
  u8 *p_url;
  /*!
    is_once_get_all_data
    */
  u32 is_once_get_all_data;
  /*!
    mul_net_extern_callback
    */
  mul_net_extern_callback cb;
}mul_net_extern_request_t;


/*!
 * mul_net_extern_init
 */
RET_CODE mul_net_extern_init(void);

/*!
 * mul_net_extern_deinit
 */
RET_CODE mul_net_extern_deinit(void);

/*!
 * mul_net_extern_start
 */
RET_CODE mul_net_extern_start(mul_net_extern_request_t *p_request);

/*!
 * mul_net_extern_stop
 */
RET_CODE mul_net_extern_stop(u8 *p_url);


#endif //__WEATHER_API_H_

