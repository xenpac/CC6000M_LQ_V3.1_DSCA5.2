/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NET_MUSIC_API_H_
#define __NET_MUSIC_API_H_

/*!
  news call back
  */
typedef void(*net_music_call_back)
            (net_music_filter_evt_t event, void *p_data, u32 request_type, u32 context);

/*!
  news attribute.
  */
typedef struct
{
  /*!
    stack size.
    */
  u32 stk_size;
  /*!
    cb see vod_call_back
    */
  net_music_site_t site;
  /*!
    cb see vod_call_back
    */
  net_music_call_back cb;
}mul_net_music_attr_t;

/*!
 * Create filter
 */
RET_CODE mul_net_music_init(void);
/*!
 * Create filter
 */
RET_CODE mul_net_music_deinit(void);
/*!
 * Create filter
 */
RET_CODE mul_net_music_create_chn(mul_net_music_attr_t *p_attr);
/*!
 * Create filter
 */
RET_CODE mul_net_music_destroy_chn(void);
/*!
 * Create filter
 */
RET_CODE mul_net_music_start(void);
/*!
 * Create filter
 */
RET_CODE mul_net_music_stop(void);
/*!
 * Create filter
 */
RET_CODE mul_net_music_get(net_music_param_t *p_net_music, u16 context);
/*!
 * Create filter
 */
RET_CODE mul_net_music_register_event(net_music_call_back callBack, void *p_args);
/*!
 * Create filter
 */
RET_CODE mul_net_music_unregister_event(void);


#endif //__NET_MUSIC_API_H_

