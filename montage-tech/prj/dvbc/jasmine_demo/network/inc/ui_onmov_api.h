/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_ONMOV_API_H__
#define __UI_ONMOV_API_H__

typedef enum 
{
  MSG_ONMOV_EVT_SERVER = MSG_EXTERN_BEGIN + 1250,
  MSG_ONMOV_EVT_MEDIA,
  MSG_MOVIE_SEARCH_UPDATE,
} onmov_msg_t;


void ui_onmov_init(void);

void ui_onmov_deinit(void);

void ui_onmov_media_request(const char* web_name, const char* cate_name, 
	int page_num, int id);

void ui_onmov_media_search(const char* p_web, 
	const char* p_cate, const char* search_pattern, u32 search_id);

#endif// End of __UI_ONMOV_API_H__
