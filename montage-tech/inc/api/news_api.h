/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __WEATHER_API_H_
#define __WEATHER_API_H_

/*!
  news call back
  */
typedef void(*news_call_back)(news_evt_t event, void *p_data, u16 context);

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
    site
    */
  news_site_t site;
  /*!
    cb see vod_call_back
    */
  news_call_back cb;
}mul_news_attr_t;

/*!
 * Create filter
 */
RET_CODE mul_news_init(void);
/*!
 * Create filter
 */
RET_CODE mul_news_deinit(void);
/*!
 * Create filter
 */
RET_CODE mul_news_create_chn(mul_news_attr_t *p_attr);
/*!
 * Create filter
 */
RET_CODE mul_news_destroy_chn(void);
/*!
 * Create filter
 */
RET_CODE mul_news_start(void);
/*!
 * Create filter
 */
RET_CODE mul_news_stop(void);
/*!
 * Create filter
 */
RET_CODE mul_news_get(news_req_t *p_news, u16 context);

/*!
 * Create filter
 */
RET_CODE mul_news_get_detail(char *url, u16 context);

/*!
 * Create filter
 */
RET_CODE mul_news_register_event(news_call_back callBack, void *p_args);
/*!
 * Create filter
 */
RET_CODE mul_news_unregister_event(void);


#endif //__WEATHER_API_H_

