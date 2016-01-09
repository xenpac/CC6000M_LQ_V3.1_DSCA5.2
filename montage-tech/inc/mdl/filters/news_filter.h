/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NEWS_FILTER_H_
#define __NEWS_FILTER_H_

/*!
  news_site_t
  */
typedef enum
{
  /*!
    pisaca
    */
  NEWS_YAHOO,
}news_site_t;

/*!
  news_type
  */
typedef enum
{
  /*!
  TOPSTORIES
  */
  NEWS_TOPSTORIES,
  /*!
  US
  */
  NEWS_US,
  /*!
  WORLD
  */
  NEWS_WORLD,
  /*!
  POLITICS
  */
  NEWS_POLITICS,
  /*!
  BUSINESS
  */
  NEWS_BUSINESS,
  /*!
  SPORTS
  */
  NEWS_SPORTS,
  /*!
  ENTERTAINMENT
  */
  NEWS_ENTERTAINMENT,
  /*!
  MOST_VIEWED
  */
  NEWS_MOST_VIEWED,
  /*!
  MOST_VIEWED
  */
  NEWS_DETAIL,

}news_type_t;

/*!
  albums_filter_photo_list_t
  */
typedef struct
{
  /*!
    request type.
    */
  news_type_t request_type;    
  /*!
    start
    */
  u16 start;      
  /*!
    count.(for commnunity search & recently upload search)
    */
  u16 count;
}news_req_t;

/*!
  news_evt_t
  */
typedef enum
{
  /*!
    NEWS_EVT_GET_NEWS
    */
  NEWS_EVT_GET_NEWS = 0x0010,
  /*!
    NEWS_EVT_GET_NEWS
    */
  NEWS_EVT_GET_DETAIL,
}news_evt_t;

/*!
  news filter command define
  */
typedef enum
{
  /*!
    news set site.
    */
  NEWS_SET_SITE = 0,    
  /*!
    get news list.
    */
  NEWS_GET_NEWS,
  /*!
    get news list.
    */
  NEWS_GET_DETAIL
}news_filter_cmd_t;

/*!
  news_source_t
  */
typedef struct
{
  /*!
  parse done
  */
  char *p_url;
  /*!
  parse done
  */
  char *p_text;
}news_source_t;
/*!
  news_media_content_t
  */
typedef struct
{
  /*!
  parse done
  */
  char *p_url;
  /*!
  parse done
  */
  char *p_type;
  /*!
  parse done
  */
  u32 width;
  /*!
  parse done
  */
  u32 height;
}news_media_content_t;
/*!
  news_media_text_t
  */
typedef struct
{
  /*!
  parse done
  */
  char *p_type;
  /*!
  parse done
  */
  char *p_content;
}news_media_text_t;
/*!
  news_media_credit_t
  */
typedef struct
{
  /*!
  parse done
  */
  char *p_role;
}news_media_credit_t;

/*!
  item_detail_t
  */
typedef struct
{
  /*!
  parse done
  */
  char *p_title;
  /*!
  parse done
  */
  char *p_description;
  /*!
  parse done
  */
  char *p_link;
  /*!
  parse done
  */
  char *p_pubDate;
  /*!
  parse done
  */
  news_source_t source;
  /*!
  parse done
  */
  news_media_content_t media_content;
  /*!
  parse done
  */
  news_media_text_t media_text;
  /*!
  parse done
  */
  news_media_credit_t media_credit;
}news_item_t;

/*!
  news_detail_t
  */
typedef struct
{
  /*!
    news count.
    */
  u32 count;
  /*!
    parse done
    */
  news_item_t *p_item;
}news_t;

/*!
  news_detail_t
  */
#define MAX_NEWS_IMG_CNT   (3)

/*!
  news_detail_t
  */
typedef struct
{
  /*!
    img count.
    */
  u16 img_cnt;
  /*!
    img url
    */
  char *p_img[MAX_NEWS_IMG_CNT];
  /*!
    segment data len
    */
  u32 seg_len;
  /*!
    parse done
    */
  char *p_segment;
}news_html_t;
/*!
  subt_filter_create
  */
ifilter_t * news_filter_create(void *p_para);

#endif
