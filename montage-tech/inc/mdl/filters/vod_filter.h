/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __VOD_FILTER_H__
#define __VOD_FILTER_H__


/*!
  vod dbg handle
  */
//#define VOD_DBG   DUMMY_PRINTF
/*!
  vod dbg handle
  */
#define VOD_DBG   OS_PRINTF





/*!
  vod_site_t
  */
typedef enum
{
  /*!
    youtube
    */
  VOD_SITE_YOUTUBE,
  /*!
    youku
    */
  VOD_SITE_YOUKU,
  /*!
    ku6
    */
  VOD_SITE_KU6,
  /*!
    tudou
    */
  VOD_SITE_TUDOU,
  /*!
    tudou
    */
  VOD_SITE_LETV
}vod_site_t;





/*!
  vod_filter_params_cfg_t
  */
typedef struct
{
  /*!
    config the callback for data return
    */
  u32 dummy;
}vod_filter_params_cfg_t;


/*****************input struct**********************/

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
  vod_pub_time_t  time;  
  /*!
  category filter params
  */
  char category[VOD_MAX_NAME];  
  /*!
  search
  */
  char search[VOD_MAX_NAME];
}vod_filter_search_t;

/*!
  vod_filter_params_cfg_t
  */
typedef struct
{
  /*!
  video_id
  */
  char video_id[VOD_MAX_NAME];
}vod_filter_item_t;

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
  time
  */
  vod_pub_time_t  time;  
  /*!
  video_id
  */
  char category[VOD_MAX_NAME];
}vod_filter_cg_list_input_t;

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
  region
  */
  vod_pub_region_t region;
  /*!
  time
  */
  vod_pub_time_t  time;
  /*!
  vod_pub_top_t
  */
  vod_pub_top_t top;
  /*!
  vod_pub_top_t
  */
  char category[VOD_MAX_NAME];
}vod_filter_top_list_input_t;



/*!
  vod filter command define
  */
typedef enum tag_vod_filter_cmd
{
  /*!
    config params see p_cmd->p_para = vod_filter_params_cfg_t
    */
  VOD_FILTER_CMD_CFG_PARAMS,
  /*!
    set page no.
    \param[in] see lpara = vod_site_t
    */
  VOD_FILTER_CMD_SET_SITE,
  /*!
    \param[in] NULL
    */
  VOD_FILTER_CMD_GET_CATEGORY,
  /*!
    \param[in] see vod_filter_search_t
    */
  VOD_FILTER_CMD_SEARCH,
  /*!
    \param[in] set vod_filter_category_list_input_t
    */
  VOD_FILTER_CMD_GET_CATEGORY_LIST,
  /*!
    \param[in] set video_id str
    */
  VOD_FILTER_CMD_GET_VIDEO_PLAY_URL,
  /*!
    \param[in] vod_filter_top_list_input_t
    */
  VOD_FILTER_CMD_GET_TOP_LIST,
  /*!
    \param[in] vod_filter_top_list_input_t
    */
  VOD_FILTER_CMD_GET_VIDEO_INFO,
  /*!
    SUBT_FILTER_CMD_STOP
    */
  VOD_FILTER_CMD_STOP
}vod_filter_cmd_t;



/*!
  vod_filter_create
  */
ifilter_t * vod_filter_create(void *p_para);

#endif

