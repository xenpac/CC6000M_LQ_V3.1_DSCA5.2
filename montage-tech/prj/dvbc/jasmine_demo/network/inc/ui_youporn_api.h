/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_YOUPORN_API_H__
#define __UI_YOUPORN_API_H__

enum 
{
  MSG_YP_VIDEO_INFO = MSG_EXTERN_BEGIN + 2100,
  MSG_YP_WEB_INFO,
  MSG_YP_PLAY_URL,
  MSG_YP_TIMEOUT,
};

#define YP_MAX_VIDEO_PER_CATE (200)

typedef struct
{
  char name[64];
  u32 video_cnt;
  youx_item video_arr[YP_MAX_VIDEO_PER_CATE];
}youporn_cate_t;


typedef struct
{
  char name[64];
  char *p_logo_url;
  int cate_count;
  youporn_cate_t *p_cate;
}youporn_website_t;

void ui_youporn_init(void);
void ui_youporn_deinit(void);
void ui_youporn_video_request(const char* web_name, const char* cate_name, 
	int page_num, int id);
youporn_website_t* get_yp_web_info(void);
void request_play_video_url(u16 video_idx);

#endif

