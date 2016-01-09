/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __VOD_PUBLIC_H__
#define __VOD_PUBLIC_H__

/*!
  VOD_MAX_CATEGORY
  */
#define VOD_MAX_CATEGORY (64)
/*!
  VOD_MAX_NAME len
  */
#define VOD_MAX_NAME (32)
/*!
  VOD_MAX_LIST cnt
  */
#define VOD_MAX_LIST   (20)
/*!
  MAX_PLAY_URL cnt
  */
#define MAX_PLAY_URL  (15)
/*!
  MAX_THUMBNAIL cnt
  */
#define MAX_THUMBNAIL  (3)


/*!
  vod_filter_evt_t
  */
typedef enum
{
  /*!
    VOD_EVT_CATEGORY
    */
  VOD_EVT_CATEGORY = 0x00000010,
  /*!
    VOD_EVT_SERACH
    */
  VOD_EVT_SERACH,
  /*!
    VOD_EVT_CATEGORY_LIST
    */
  VOD_EVT_CATEGORY_LIST,
  /*!
  VOD_EVT_VIDEO_DETAIL
  */
  VOD_EVT_VIDEO_PLAY_URL,
  /*!
  VOD_EVT_TOP_LIST
  */
  VOD_EVT_TOP_LIST,
  /*!
  VOD_EVT_TOP_LIST
  */
  VOD_EVT_VIDEO_INFO
}vod_pub_evt_t;

/*!
  vod_filter_top_t
  */
typedef enum
{
  /*!
  VOD_FILTER_TOP_RATED
  */
  VOD_FILTER_TOP_RATED,
  /*!
  VOD_FILTER_TOP_FAV
  */
  VOD_FILTER_TOP_FAV,
  /*!
  VOD_FILTER_TOP_VIEW
  */
  VOD_FILTER_TOP_VIEW,
  /*!
  VOD_FILTER_TOP_POPULAR
  */
  VOD_FILTER_TOP_POPULAR,
  /*!
  VOD_FILTER_TOP_DISCUSSED
  */
  VOD_FILTER_TOP_DISCUSSED,
  /*!
  VOD_FILTER_TOP_RESPONDED
  */
  VOD_FILTER_TOP_RESPONDED
}vod_pub_top_t;


/*!
  time
  */
typedef enum
{
  /*!
    time all
    */  
  VOD_PUB_TIME_ALL,
  /*!
    time today
    */
  VOD_PUB_TIME_TODAY,
  /*!
    time week
    */
  VOD_PUB_TIME_WEEK,
  /*!
    time month
    */
  VOD_PUB_TIME_MONTH
}vod_pub_time_t;

/*!
  region
  */
typedef enum
{
  /*!
    region
    */  
  VOD_PUB_REGION_ALL,
  /*!
    region
    */  
  VOD_PUB_REGION_AUSTRALIA,
  /*!
    region
    */
  VOD_PUB_REGION_BRAZIL,
  /*!
    region
    */
  VOD_PUB_REGION_CANADA,
  /*!
    region
    */
  VOD_PUB_REGION_CZECH_REPUBLIC,
  /*!
    region
    */
  VOD_PUB_REGION_FRANCE,
  /*!
    region
    */
  VOD_PUB_REGION_GERMANTY,
  /*!
    region
    */
  VOD_PUB_REGION_GREAT_BRITAIN,
  /*!
    region
    */
  VOD_PUB_REGION_HONGKONG,
  /*!
    region
    */
  VOD_PUB_REGION_INDIA,
  /*!
    region
    */
  VOD_PUB_REGION_IRELAND,
  /*!
    region
    */
  VOD_PUB_REGION_ISRAEL,
  /*!
    region
    */
  VOD_PUB_REGION_ITALY,
  /*!
    region
    */
  VOD_PUB_REGION_JAPAN,
  /*!
    region
    */
  VOD_PUB_REGION_MEXICO,
  /*!
    region
    */
  VOD_PUB_REGION_NETHERLANDS,
  /*!
    region
    */
  VOD_PUB_REGION_NEWZEALAND,
  /*!
    region
    */
  VOD_PUB_REGION_POLAND,
  /*!
    region
    */
  VOD_PUB_REGION_RUSSIA,
  /*!
    region
    */
  VOD_PUB_REGION_SOUTHKOREA,
  /*!
    region
    */
  VOD_PUB_REGION_SPAIN,
  /*!
    region
    */
  VOD_PUB_REGION_SWEDEN,
  /*!
    region
    */
  VOD_PUB_REGION_TAIWAN,
  /*!
    region
    */
  VOD_PUB_REGION_UNITEDSTATES
}vod_pub_region_t;


/*!
  format
  */
typedef enum
{
  /*!
    format
    */
  VOD_PUB_VIDEO_FORMAT_UNKOWN,
  /*!
    format
    */
  VOD_PUB_VIDEO_FORMAT_FLV,
  /*!
    format
    */
  VOD_PUB_VIDEO_FORMAT_MP4,
  /*!
    format
    */
  VOD_PUB_VIDEO_FORMAT_3GP
}vod_pub_video_format_t;

/*!
  resolution
  */
typedef enum
{
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_176_144 = 0,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_320_240,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_400_240,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_480_360,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_640_360,  
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_854_480,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_1280_720,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_1920_520,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_1920_720,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_1920_1080,
  /*!
  resolution
  */
  VOD_PUB_VIDEO_RESOLUTION_4096_3072
}vod_pub_video_resolution_t;

/*****************output struct**********************/
/*!
  vod_filter_params_cfg_t
  */
typedef struct
{
  /*!
    category cnt
    */
  u8 category_cnt;
  /*!
    category lists
    */
  char category[VOD_MAX_CATEGORY][VOD_MAX_NAME];
}vod_pub_category_t;

/*!
vod_pub_play_url_t
*/
typedef struct
{
  /*!
    type 0: unkown, 1,flv , 2 :mp4/h264
    */  
  vod_pub_video_format_t format;

  /*!
    resolution
    */
  vod_pub_video_resolution_t resolution;
  /*!
    the real play url
    */
  char * p_url;
}vod_pub_play_url_t;


/*!
vod_pub_play_url_t
*/
typedef struct
{
  /*!
  intro
  */
  char *p_intro;
}vod_pub_video_info_t;


/*!
vod_pub_play_url_t
*/
typedef struct
{
  /*!
  url_cnt
  */
  u32 url_cnt;
  /*!
  vod_pub_play_url_t
  */
  vod_pub_play_url_t play_url[MAX_PLAY_URL];
}vod_pub_video_play_url_t;

/*!
vod_pub_play_url_t
*/
typedef struct
{
  /*!
  p_thumbnail_url
  */
  char *p_thumbnail_url;
  /*!
  w
  */
  u16 w;
  /*!
  h
  */
  u16 h;
}vod_pub_thumbnail_t;

/*!
  vod_pub_video_basic_t
  */
typedef struct
{
  /*!
  p_title
  */
  char *p_title;
  /*!
  p_video_url
  */
  char *p_video_url;
  /*!
  p_author
  */
  char *p_author;
  /*!
  vod_pub_thumbnail_t
  */
  vod_pub_thumbnail_t thumb[MAX_THUMBNAIL];
  /*!
  thumnail_cnt
  */
  u8 thumnail_cnt;
  /*!
  p_video_id
  */
  char *p_video_id;
  /*!
  p_intro
  */
  char *p_intro;
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

}vod_pub_video_basic_t;



/*!
  vod_pub_video_list_t
  */
typedef struct
{
  /*!
    video cnt for current data back
    */
  u32 current_start;
  /*!
    video cnt for current data back
    */
  u8 current_cnt;
  /*!
    reserved1
    */
  u8 reserved1;
  /*!
    reserved1
    */
  u16 reserved2;
  /*!
    total  cnt for the search
    */
  u32 total_cnt;  
  /*!
    vod_pub_video_basic_t
    */
  vod_pub_video_basic_t basic[VOD_MAX_LIST];
}vod_pub_video_list_t;




#endif
