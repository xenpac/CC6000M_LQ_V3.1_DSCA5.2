/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RSS_API_H__
#define __RSS_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  rss events for notitying to user
  */
typedef enum
{
  /*!
    the event for notifying user that getting channel ok
    */
  MUL_RSS_EVT_GET_CH_OK = 0x00000010,
  /*!
    the event for notifying user that getting channel null
    */
  MUL_RSS_EVT_GET_CH_NULL,
  /*!
    the event for notifying user that getting channel fail
    */
  MUL_RSS_EVT_GET_CH_FAIL,
    /*!
    the event for notifying user that getting text ok
    */
  MUL_RSS_EVT_GET_TXT_OK,
  /*!
    the event for notifying user that getting text null
    */
  MUL_RSS_EVT_GET_TXT_NULL,
  /*!
    the event for notifying user that getting text fail
    */
  MUL_RSS_EVT_GET_TXT_FAIL,
  /*!
    the event for notifying user that updating channel ok
    */
  MUL_RSS_EVT_UPDATE_CH_OK,
  /*!
    the event for notifying user that updating channel null
    */
  MUL_RSS_EVT_UPDATE_CH_NULL,
  /*!
    the event for notifying user that updating channel fail
    */
  MUL_RSS_EVT_UPDATE_CH_FAIL,
  /*!
    the event for notifying user that request channel fail
    */
  MUL_RSS_EVT_REQ_CH_FAIL,
  /*!
    the event for notifying user that request text fail
    */
  MUL_RSS_EVT_REQ_TXT_FAIL,
  /*!
    the event for notifying user that feed null
    */
  MUL_RSS_EVT_FEED_NULL,
  /*!
    the event for notifying user that data error
    */
  MUL_RSS_EVT_DATA_ERR,
}mul_rss_evt_t;

/*!
  rss call back function, see 'mul_rss_evt_t'
  */
typedef void(*mul_rss_call_back)(mul_rss_evt_t evt,
                                 const void *p_data,
                                 u16 data_len,
                                 u16 context);

/*!
  rss attribute for creating
  */
typedef struct 
{
  /*!
    stack size.
    */
  u32 stk_size;
  /*!
    call back function, see 'mul_rss_call_back'
    */
  mul_rss_call_back cb;
}mul_rss_attr_t;

/*!
  rss item
  */
typedef struct
{
  /*!
    item title
    */
  u8 *p_title;
  /*!
    item link
    */
  u8 *p_link;
  /*!
    item autho
    */
  u8 *p_author;
  /*!
    item guid
    */
  u8 *p_guid;
  /*!
    item category
    */
  u8 *p_category;
  /*!
    item comments
    */
  u8 *p_comments;
  /*!
    item description
    */
  u8 *p_descr;
  /*!
    item image url
    */
  u8 *p_image_url;   
  /*!
    item publish date
    */
  utc_time_t pub_date;
}mul_rss_item_t;

/*!
  rss image
  */
typedef struct
{
  /*!
    imag title
    */
  u8 *p_title;
  /*!
    imag link
    */
  u8 *p_link;
  /*!
    imag url
    */
  u8 *p_url;
}mul_rss_image_t;

/*!
  rss channel
  */
typedef struct
{
  /*!
    channel title
    */
  u8 *p_title;
  /*!
    channel link
    */
  u8 *p_link;
  /*!
    channel description
    */
  u8 *p_descr;
  /*!
    channel language
    */
  u8 *p_lang;
  /*!
    channel copyright
    */
  u8 *p_copyright;
  /*!
    channel category
    */
  u8 *p_category;
  /*!
    channel generator
    */
  u8 *p_generator;
  /*!
    channel ttl
    */
  u16 ttl;
  /*!
    channel item count
    */
  u16 item_count;
  /*!
    channel item list
    */
  mul_rss_item_t *p_items;
  /*!
    channel image
    */
  mul_rss_image_t image;
  /*!
    channel publish date
    */
  utc_time_t pub_date;
  /*!
    channel last build date
    */
  utc_time_t last_build_date;
}mul_rss_ch_t;

/*!
  rss feed
  */
typedef struct
{
  /*!
    category
    b00-b07:main category(CNN, Yahoo etc.)
            0x00-0xFF, '0x00' means none.
    b08-b15:second category
            0x00-0xFF, '0x00' means none.
    b16-b23: third category
            0x00-0xFF, '0x00' means none.
    b24-b31:fourth category
            0x00-0xFF, '0x00' means none.
    */
  u32 category;
  /*!
    feed name
    */
  u8 *p_name;
  /*!
    feed url
    */
  u8 *p_url;
  /*!
    monitor flag
    '1' means enable monitor
    '0' means disable monitor
    */
  u8 is_monitor;
  /*!
    feed updating interval(minutes)
    */
  u16 update_interval;
}mul_rss_feed_t;

/*!
  rss init
  */
RET_CODE mul_rss_init(void);

/*!
  rss chain create
  */
RET_CODE mul_rss_create(mul_rss_attr_t *p_attr);

/*!
  rss chain destroy
  */
RET_CODE mul_rss_destroy();

/*!
  rss chain start
  */
RET_CODE mul_rss_start(void);

/*!
  rss chain stop
  */
RET_CODE mul_rss_stop(void);

/*!
  rss req channel
  */
RET_CODE mul_rss_channel_req(const mul_rss_feed_t *p_feed, u16 user_id);

/*!
  rss cancel channel request 
  */
RET_CODE mul_rss_channel_cancel(const u8 *p_url);

/*!
  rss get channel info
  */
RET_CODE mul_rss_channel_get_info(const u8 *p_url, mul_rss_ch_t *p_channel);

/*!
  rss add feed
  */
RET_CODE mul_rss_feed_add(const mul_rss_feed_t *p_feed);

/*!
  rss remove feed
  */
RET_CODE mul_rss_feed_remove(const u8 *p_url);

/*!
  rss edit feed
  */
RET_CODE mul_rss_feed_edit(const mul_rss_feed_t *p_feed);

/*!
  rss load feeds
  */
RET_CODE mul_rss_feed_load(mul_rss_feed_t **p_feeds, u16 *p_count);

/*!
  rss req text
  */
RET_CODE mul_rss_text_req(const u8 *p_url, u16 user_id);

/*!
  rss cancel text request 
  */
RET_CODE mul_rss_text_cancel(const u8 *p_url);

/*!
  rss get text
  */
RET_CODE mul_rss_text_get_info(u8 *p_url, u8 **p_text, u32 *p_size);

/*!
  rss get logo data
  */
RET_CODE mul_rss_logo_get_info(u32 category, u8 **p_logo, u32 *p_size);

/*!
  rss cancel all of channel and text requests 
  */
RET_CODE mul_rss_cancel_all(void);

#ifdef __cplusplus
}
#endif

#endif

