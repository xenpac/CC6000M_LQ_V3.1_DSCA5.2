/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NET_EXTERN_FILTER_H__
#define __NET_EXTERN_FILTER_H__

/*!
  net_extern_filter_callback
  */
typedef void (*net_extern_filter_callback)(u8 *p_url, u8 *p_data, u32 size);

/*!
  vod filter command define
  */
typedef enum tag_net_extern_filter_cmd
{
  /*!
    set callback
    */
  NET_EXTERN_SET_CALLBACK,
  /*!
    start to URL request
    */
  NET_EXTERN_REQUEST_URL,
  /*!
    start url request
    */
  NET_EXTERN_STOP_URL,
}net_extern_filter_cmd_t;

/*!
  net_extern_filter_create
  */
ifilter_t * net_extern_filter_create(void *p_para);

#endif

