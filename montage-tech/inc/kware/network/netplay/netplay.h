/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NETPLAY_H_
#define __NETPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif    

/*!
  netplay name array  length
  */
#define NETPLAY_NAME_MAX_LEN        64

/*!
  netplay_notify
  */
typedef   int (* netplay_notify)(void *, void *);

/*!
  netplay_msg_t
  */
typedef enum 
{
  /*!
  MSG GOT SERVER INFO
  */
  NETPLAY_MSG_GOT_SERVER_INFO,
  /*!
  MSG GOT MEDIA INFO
  */
  NETPLAY_MSG_GOT_MEDIA_INFO,
  /*!
  MSG GOT MEDIA SEARCH RESULT
  */
  NETPLAY_MSG_GOT_MEDIA_SEARCH_RESULT
} netplay_msg_t;

/*!
  netplay_video_resolution_t
  */
typedef enum 
{
  /*!
  normal resolution
  */
  NETPLAY_VIDEO_RESOLUTION_NORMAL,
  /*!
  high resolution
  */
  NETPLAY_VIDEO_RESOLUTION_HIGH,
  /*!
  super resolution
  */
  NETPLAY_VIDEO_RESOLUTION_SUPER,
} netplay_video_resolution_t;

/*!
  netplay_media_info_t
  */
typedef struct {
  /*!
  media name
  */
  char name[NETPLAY_NAME_MAX_LEN];
  /*!
  media logo url
  */
  char *p_logo_url;
  /*!
  media director
  */
  char *p_director;
  /*!
  media actor
  */
  char *p_actor;
  /*!
  media description
  */
  char *p_description;
  /*!
  media url
  */
  char *p_url;
} netplay_media_info_t;

/*!
  netplay_category_info_t
  */
typedef struct
{
  /*!
  media name
  */
  char name[NETPLAY_NAME_MAX_LEN];
  /*!
  media count
  */
  int media_count;
} netplay_category_info_t;

/*!
  netplay_website_info_t
  */
typedef struct
{
  /*!
  website name
  */
  char name[NETPLAY_NAME_MAX_LEN];
  /*!
  logo url
  */
  char *p_logo_url;
  /*!
  category count
  */
  int cate_count;
  /*!
  category info
  */
  netplay_category_info_t *p_cate;
} netplay_website_info_t;

/*!
  netplay_server_info_t
  */
typedef struct
{
  /*!
  website count
  */
  int website_count;
  /*!
  website info
  */
  netplay_website_info_t *p_web;
} netplay_server_info_t;

/*!
  netplay_config_t
  */
typedef struct
{
  /*!
  stack size
  */
  int stack_size;
  /*!
  task prio
  */
  int task_prio;
  /*!
  page size
  */
  int page_size;
  /*!
  notify function
  */
  netplay_notify notify;
} netplay_config_t;

/*!
  netplay_media_request_t
  */
typedef struct
{
  /*!
  media request page num
  */
  int page_num;
  /*!
  media request identify
  */
  int identify;
  /*!
  category name
  */
  char category_name[NETPLAY_NAME_MAX_LEN];
  /*!
  website name
  */
  char website_name[NETPLAY_NAME_MAX_LEN];
} netplay_media_request_t;

/*!
  netplay_media_search_request_t
  */
typedef struct
{
   /*!
  media search identify
  */
  int identify;
  /*!
  media name to search
  */
  char media_name[NETPLAY_NAME_MAX_LEN];
  /*!
  category name to search
  */
  char category_name[NETPLAY_NAME_MAX_LEN];
  /*!
  website name to search
  */
  char website_name[NETPLAY_NAME_MAX_LEN];
} netplay_media_search_request_t;

/*!
  netplay_notify_event_t
  */
typedef struct
{
  /*!
  event, see NETPLAY_MSG
  */
  int event;
  /*!
  event identify
  */
  int identify;
} netplay_notify_event_t;

/*!
  netplay_media_msg_info_t
  */
typedef struct {
  /*!
  media count
  */
  int media_count;
  /*!
  media info
  */
  netplay_media_info_t *p_info;
} netplay_media_msg_info_t;

/*!
  module initialization
  */
int netplay_init(netplay_config_t *p_cfg);

/*!
  request server info, will return by notify
  */
int netplay_request_server_info(void);

/*!
  request media info, will return by notify
  */
int netplay_request_media_info(netplay_media_request_t *p_request);

/*!
  request media search, will return by notify
  */
int netplay_request_media_search(netplay_media_search_request_t *p_request);

/*!
  set video resolution
  */
int netplay_set_video_resolution(netplay_video_resolution_t resolution);

#ifdef __cplusplus
}
#endif
#endif

