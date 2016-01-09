/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __IPTV_H_
#define __IPTV_H_

#ifdef __cplusplus
extern "C" {
#endif    

/*!
  iptv name array  length
  */
#define IPTV_NAME_MAX_LEN        32

/*!
  iptv channel url array  length
  */
#define IPTV_CHANNEL_URL_MAX_LEN        32

/*!
  iptv_notify
  */
typedef   int (* iptv_notify)(void *, void *);

/*!
  iptv_msg_t
  */
typedef enum 
{
  /*!
  MSG GOT CHANNEL INFO
  */
  IPTV_MSG_GOT_CHANNEL_INFO,
  /*!
  MSG GOT URL INFO
  */
  IPTV_MSG_GOT_URL_INFO,
} iptv_msg_t;

/*!
  iptv_config_t
  */
typedef struct
{
  /*!
  task stack size
  */
  int stack_size;
  /*!
  task prio
  */
  int task_prio;
  /*!
  notify function
  */
  iptv_notify notify;
} iptv_config_t;

/*!
  iptv_notify_event_t
  */
typedef struct
{
  /*!
  event, see IPTV_MSG
  */
  int event;
} iptv_notify_event_t;

/*!
  iptv_channel_info_t
  */
typedef struct
{
  /*!
  channel id
  */
  int channel_id;
  /*!
  channel name
  */
  char channel_name[IPTV_NAME_MAX_LEN];
} iptv_channel_info_t;

/*!
  iptv_url_msg_info_t
  */
typedef struct {
  /*!
  channel id
  */
  int channel_id;
  /*!
  url count
  */
  int url_count;
  /*!
  url array
  */
  char *p_url[IPTV_CHANNEL_URL_MAX_LEN];
} iptv_url_msg_info_t;

/*!
  iptv_channel_msg_info_t
  */
typedef struct {
  /*!
  channel count
  */
  int channel_count;
  /*!
  media info
  */
  iptv_channel_info_t *p_info;
} iptv_channel_msg_info_t;


/*!
  iptv_channel_request_t
  */
typedef struct {
  /*!
  channel id
  */
  int channel_id;
} iptv_channel_request_t;

/*!
  module initialization
  */
int iptv_init(iptv_config_t *p_cfg);

/*!
  request server info, will return by notify
  */
int iptv_request_server_info(void);

/*!
  request channel url info, will return by notify
  */
int iptv_request_channel_info(iptv_channel_request_t *p_request);

#ifdef __cplusplus
}
#endif
#endif

