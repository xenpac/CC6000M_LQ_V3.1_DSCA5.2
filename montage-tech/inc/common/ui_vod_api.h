/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_VOD_API_H__
#define __UI_VOD_API_H__

/*!
  fix me
  */
#define VOD_URL_MAX   2048
/*!
  fix me
  */
#define VOD_TTL_MAX   1024
/*!
  fix me
  */
#define VOD_VID_MAX   64

/*!
  fix me
  */
#define YTB_CAT_CNT 13
/*!
  fix me
  */
#define YTB_CTY_CNT 24
/*!
  fix me
  */
#define YTB_RES_CNT  2
/*!
  fix me
  */
#define YTB_TME_CNT 4

/*!
  fix me
  */
typedef enum 
{
/*!
  fix me
  */
  MSG_VOD_EVT_FAILED = MSG_EXTERN_BEGIN + 1310,
/*!
  fix me
  */
  MSG_VOD_EVT_SUCCESS,
}vod_msg_t;

/*!
  fix me
  */
typedef struct
{
  /*!
  p_thumbnail_url
  */
  u8 thumbnail_url[VOD_URL_MAX];
  /*!
  w
  */
  u16 w;
  /*!
  h
  */
  u16 h;  
}vod_thumb_t;

/*!
  fix me
  */
typedef struct
{
  /*!
  p_title
  */
  u8 title[VOD_TTL_MAX];
  /*!
  p_video_url
  */
  u8 video_url[VOD_URL_MAX];
  /*!
  p_author
  */
  u8 author[VOD_TTL_MAX];
  /*!
  vod_pub_thumbnail_t
  */
  vod_thumb_t thumb[MAX_THUMBNAIL];
  /*!
  thumnail_cnt
  */
  u8 thumnail_cnt;
  /*!
  p_video_id
  */
  u8 video_id[VOD_VID_MAX];
  /*!
  p_description
  */
  u16 description[VOD_TTL_MAX];
  /*!
  duration_s
  */
  u32 duration_s;
  /*!
  rating
  */
  float  rating;  
  /*!
  rate_cnt
  */
  u32 rate_cnt;
  /*!
  view_cnt
  */
  u32 view_cnt;
  /*!
  update_time
  */
  utc_time_t update_time;  
}vod_item_t;

/*!
  fix me
  */
typedef struct
{
  /*!
  fix me
  */
  u32 current_start;
  /*!
    video cnt for current data back
    */
  u32 current_cnt;
  /*!
    total  cnt for the search
    */
  u32 total_cnt; 
  /*!
    vod item.
    */
  vod_item_t item[VOD_MAX_LIST];
}vod_data_t;

/*!
  fix me
  */
typedef struct
{
/*!
  fix me
  */
  vod_pub_video_format_t format;

/*!
  fix me
  */
  vod_pub_video_resolution_t resolution;

/*!
  fix me
  */
  u16 url[VOD_URL_MAX];
}vod_url_t;

/*!
  fix me
  */
typedef struct
{
/*!
  fix me
  */
  u32 url_cnt;
/*!
  fix me
  */
  vod_url_t url[MAX_PLAY_URL];
}vod_play_url_t;



/*!
  fix me
  */
vod_data_t *ui_vod_get_data(void);

/*!
  fix me
  */
void ui_vod_search(u16 start, u16 end, char *p_str);

/*!
  fix me
  */
vod_play_url_t *ui_vod_get_detail(void);

/*!
  fix me
  */
vod_item_t *ui_vod_get_description(void);

/*!
  fix me
  */
void ui_vod_init(vod_api_site_t site);

/*!
  fix me
  */
void ui_vod_release(void);

/*!
  fix me
  */
RET_CODE ui_vod_get_top(vod_api_top_list_t *p_top);

/*!
  fix me
  */
RET_CODE ui_vod_get_cat(vod_api_category_t *p_cat);

/*!
  fix me
  */
RET_CODE ui_vod_get_desc(u8 *p_video_id);

/*!
  fix me
  */
u8 *ui_vod_get_cat_filter(u8 idx);

/*!
  fix me
  */
u8 *ui_vod_get_cty_filter(u8 idx);

/*!
  fix me
  */
u8 *ui_vod_get_res_filter(u8 idx);

/*!
  fix me
  */
u8 *ui_vod_get_tme_filter(u8 idx);

/*!
  fix me
  */
u8 ui_vod_get_cat_select(void);

/*!
  fix me
  */
u8 ui_vod_get_cty_select(void);

/*!
  fix me
  */
u8 ui_vod_get_res_select(void);

/*!
  fix me
  */
u8 ui_vod_get_tme_select(void);

/*!
  fix me
  */
RET_CODE ui_vod_get_play_url(u8 *p_vpid);
#endif
