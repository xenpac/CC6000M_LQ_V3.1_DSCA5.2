/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __RSS_FILTER_HPP__
#define __RSS_FILTER_HPP__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  rss filter commands
  */
typedef enum
{
  RSS_FILTER_CMD_FEED_ADD = 0,
  RSS_FILTER_CMD_FEED_REMOVE,
  RSS_FILTER_CMD_FEED_EDIT,
  RSS_FILTER_CMD_FEED_LOAD
}rss_filter_cmd_t;

/*!
  rss filter events
  */
typedef enum
{
  RSS_FILTER_EVT_GET_CH_OK = 0,
  RSS_FILTER_EVT_GET_CH_NULL,
  RSS_FILTER_EVT_GET_CH_FAIL,
  RSS_FILTER_EVT_GET_TXT_OK,
  RSS_FILTER_EVT_GET_TXT_NULL,
  RSS_FILTER_EVT_GET_TXT_FAIL,
  RSS_FILTER_EVT_UPDATE_CH_OK,
  RSS_FILTER_EVT_UPDATE_CH_NULL,
  RSS_FILTER_EVT_UPDATE_CH_FAIL,
  RSS_FILTER_EVT_REQ_CH_FAIL,
  RSS_FILTER_EVT_REQ_TXT_FAIL,
  RSS_FILTER_EVT_FEED_NULL,
  RSS_FILTER_EVT_DATA_ERR,
}rss_filter_evt_t;

/*!
  rss filter req type
  */
typedef enum
{
  RSS_FILTER_TYPE_CH = 0,
  RSS_FILTER_TYPE_TXT,
  RSS_FILTER_TYPE_LOGO,
}rss_filter_type_t;

/*!
  rss filter for loading feeds
  */
typedef struct
{
  /*!
    feed count
    */
  u16 count;
  /*!
    feeds
    */
  mul_rss_feed_t *p_feeds;
}rss_filter_load_feeds_t;

/*!
  rss filter for getting channel
  */
typedef struct
{
  /*!
    url for req channel
    */
  u8 *p_url;
  /*!
    channel info cache
    */
  mul_rss_ch_t *p_channel;
}rss_filter_get_channel_t;

/*!
  rss filter for getting text
  */
typedef struct
{
  /*!
    html url
    */
  u8 *p_url;
  /*!
    text size
    */
  u32 size;
  /*!
    text data
    */
  u8 *p_data;
}rss_filter_get_text_t;

/*!
  rss filter for getting logo
  */
typedef struct
{
  /*!
    category
    */
  u32 category;
  /*!
    logo size
    */
  u32 size;
  /*!
    logo data
    */
  u8 *p_data;
}rss_filter_get_logo_t;

/*!
  rss filter load functions
  */
void rss_filter_load(handle_t _this);

#ifdef __cplusplus
}
#endif

#endif

