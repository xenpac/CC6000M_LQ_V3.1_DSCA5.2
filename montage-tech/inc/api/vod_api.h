/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __VOD_API_H_
#define __VOD_API_H_



/*!
  vod_site_t
  */
typedef enum
{
  /*!
    youtube
    */
  VOD_API_SITE_YOUTUBE,
  /*!
    youku
    */
  VOD_API_SITE_YOUKU,
}vod_api_site_t;


/*!
  vod_call_back
  */
typedef void(*vod_call_back)(vod_pub_evt_t event, void *p_data, u16 context);

/*!
  FIXME
  */
typedef struct 
{
  /*!
    FIXME
    */
  u32 stk_size;
  /*!
    site
    */
  vod_api_site_t site;
  /*!
    cb see vod_call_back
    */
  vod_call_back cb;
}mul_vod_attr_t;


/*!
  vod_filter_params_cfg_t
  */
typedef struct
{
  /*!
  start
  */
  u16 start;
  /*!
  max
  */
  u16 max;
  /*!
  time filter params
  */
  vod_pub_time_t time;
  /*!
  category filter params
  */
  char category[VOD_MAX_NAME];
  /*!
  search
  */
  char search[VOD_MAX_NAME];
}vod_api_search_t;


/*!
  vod_pub_cg_list_input_t
  */
typedef struct
{
  /*!
  video_id
  */
  u16 start;
  /*!
  video_id
  */
  u16 max;
  /*!
  video_id
  */
  vod_pub_time_t time;
  /*!
  video_id
  */
  char category[VOD_MAX_NAME];
}vod_api_category_t;


/*!
  vod_pub_top_list_input_t
  */
typedef struct
{
  /*!
  start
  */
  u16 start;
  /*!
  max
  */
  u16 max;
  /*!
  time filter params
  */
  vod_pub_time_t time;  
  /*!
  region
  */
  vod_pub_region_t region;
  /*!
  video_id
  */
  char category[VOD_MAX_NAME];
  /*!
  vod_pub_top_t
  */
  vod_pub_top_t top;
}vod_api_top_list_t;



/*!
 * Create filter
 */
RET_CODE mul_vod_init(void);
/*!
 * mul_vod_chn_create
 */
RET_CODE mul_vod_chn_create(mul_vod_attr_t *p_attr);
/*!
 * mul_vod_chn_destory
 */
RET_CODE mul_vod_chn_destroy(u32 chanid);
/*!
 * mul_vod_start
 */
RET_CODE mul_vod_start(void);
/*!
 * mul_vod_stop
 */
RET_CODE mul_vod_stop(void);
/*!
 * mul_vod_get_category
 */
RET_CODE mul_vod_get_category(u16 context);

/*!
 * mul_vod_search
 */
RET_CODE mul_vod_search(vod_api_search_t *p_sch, u16 context);
/*!
 * mul_vod_get_list_by_category
 */
RET_CODE mul_vod_get_list_by_category(vod_api_category_t *p_category, u16 context);
/*!
 * mul_vod_get_top_list
 */
RET_CODE mul_vod_get_top_list(vod_api_top_list_t *p_top, u16 context);
/*!
 * mul_vod_get_video_detail
 */
RET_CODE mul_vod_get_play_url(char *p_v_id, u16 context);

/*!
 * mul_vod_get_video_detail
 */
RET_CODE mul_vod_get_description(char *p_v_id, u16 context);
#endif //

