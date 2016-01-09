/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_NEWS_API_H__
#define __UI_NEWS_API_H__

typedef enum 
{
  MSG_NEWS_EVT_FAILED = MSG_EXTERN_BEGIN + 1330,
  
  MSG_NEWS_EVT_SUCCESS,

}news_msg_t;

void ui_news_init(news_site_t site);

void ui_news_release(void);

RET_CODE ui_news_get(news_req_t *p_news);

news_t *ui_news_get_news(void);

RET_CODE ui_news_detail_create_filter(news_t *p_news, u16 pos);

typedef BOOL (*news_up_callback)();

#endif
