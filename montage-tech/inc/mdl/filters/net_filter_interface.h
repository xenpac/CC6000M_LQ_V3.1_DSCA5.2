/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NET_INTERFACE_H_
#define __NET_INTERFACE_H_

/*!
  for debug
  */
#if 0
/*!
  for debug
  */
#define NET_DBG OS_PRINTF
#else
/*!
  for debug
  */
#define NET_DBG(...)     do{}while(0)
#endif

/*!
  defines interface name macro
  */
#define NET_INTERFACE_NAME    "net_src_interface"

/*!
  defines interface name macro
  */
typedef struct tag_http_post_para
{
  /*!
   a flag, indicates cookie is set or not
  */
  u16 cookie_set;
  /*!
  cookie contents specified by upper layer
  */
  u16 cookie[256];
  /*!
  post_value contents post information 
  */ 
  char *post_value; 
  /*!
  a flag, indicates post is set or not
  */
  u16 post_set; 
  /*
  post content length
  */
  u32 post_value_length; 
}http_post_para;

/*!
  net src pin request
  */
typedef struct tag_net_src_pin_request
{
  /*!
    url
    */
  char *p_url;
  /*!
    context
    */
  u32 context;
  /*!
    is_once_get_all_data
    */
  u32 is_once_get_all_data : 1;
  /*!
    is_monitor
    */
  u32 is_monitor : 1;
  /*!
    monitor interval second
    */
  u32 monitor_interval_sec : 30;

  http_post_para *p_post;
}net_src_pin_request_t;

/*!
  demux interface
  */
typedef struct tag_net_interface
{
  /*!
    point to the owner
    */
  PRIVATE interface_t *p_owner;
  /*!
    request table

    \param[in] _this dmx pin handle
    \param[in] p_req pointer request info 
    */
  RET_CODE (*i_request)(handle_t _this, net_src_pin_request_t *p_req);

  /*!
    free table

    \param[in] _this dmx pin handle
    \param[in] p_url url need free, NULL means free all 
    */
  RET_CODE (*i_free)(handle_t _this, void *p_url);
}net_src_interface_t;

/*!
  net sample user data
  */
typedef struct tag_net_sample_user_data
{
  /*!
    p_url
    */
  char *p_url;
  /*!
    auto send, used for monitor
    */
  u32 auto_send : 1;
  /*!
    reseved
    */
  u32 reseved : 31;
}net_samp_user_data_t;

#endif // End for __NET_FILTER_INTERFACE_H_

