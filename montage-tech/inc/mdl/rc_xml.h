/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __RC_XML_H__
#define __RC_XML_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  max len
  */
#define MAX_RC_INPUT_LEN   (256)

/*!
  max len
  */
#define MAX_NAME_LEN      (32)


/*!
  rc_xml_type_t
  */
typedef enum
{
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_UNKNOWN = 0,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_CTRL,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_INPUT,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_ACK,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_PG_LIST,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_CH_PARAM,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_CH_PARAM_BY_ID,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_DVB_PLAY,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_PLAYING_INFO,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_SEND_IP,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_HEARTBEAT,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_TP_CH_LIST_GET,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_TP_CH_LIST_GET_BY_ID,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_CONNECT,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_DISCONNECT,

  /*!
    Flag of evt 
    */
  RC_XML_TYPE_MEDIA_IP_PLAY,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_MEDIA_IP_PLAY_STOP,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_SERVER_CHANGE_CH,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_SET_AUTH,
  
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_GET_AUTH,
  /*!
    Flag of evt 
    */
  RC_XML_TYPE_CUR_PROGRAM,
  
}rc_xml_type_t;

/*!
  rc_xml_result_t
  */
typedef enum
{
  /*!
    Flag of evt 
    */
  RC_XML_RESULT_SUCCESS = 0,
  /*!
    Flag of evt 
    */
  RC_XML_RESULT_FAILED,
  /*!
    Flag of evt 
    */
  RC_XML_RESULT_TIMEOUT,
  /*!
    Flag of evt 
    */
  RC_XML_RESULT_NO_PLAYING_PG,
  /*!
    Flag of evt 
    */
  RC_XML_RESULT_NOT_IN_INPUT_MODE,
  /*!
    Flag of evt 
    */
  RC_XML_RESULT_NOT_CONNECTED,
  /*!
    Flag of evt 
    */
  RC_XML_RESULT_PARAM_ERROR
}rc_xml_result_t;

/*!
  rc_xml_result_t
  */
typedef enum
{
  /*!
    Flag of evt 
    */
  RC_XML_EVT_CH_LIST_UPDATE = 0,
  /*!
    Flag of evt 
    */
  RC_XML_EVT_ON_TEXT_INPUT,
  /*!
    Flag of evt 
    */
  RC_XML_EVT_EXIT_TEXT_INPUT,
  /*!
    Flag of evt 
    */
  RC_XML_EVT_GET_LIST_FAILED
}rc_xml_evt_t;

/*!
  httpd service ack
  */
typedef enum
{
  /*!
    ack success
    */
  HTTPD_SVC_ACK_SUCCESS = 0,
  /*!
    ack failed 
    */
  HTTPD_SVC_ACK_FAILED,
  /*!
    ack no content
    */
  HTTPD_SVC_ACK_NO_CONTENT,
  /*!
    ack type error
    */
  HTTPD_SVC_ACK_TYPE_ERROR,
  /*!
    ack ip error
    */
  HTTPD_SVC_ACK_IP_ERROR,
  /*!
    ack system setting error
    */
  HTTPD_SVC_ACK_SETTING_ERROR,
  /*!
    ack get system setting error
    */
  HTTPD_SVC_ACK_GET_SETTING_ERROR,
  /*!
    ack build setting error
    */
  HTTPD_SVC_ACK_BUILD_SETTING_ERROR,
  /*!
    ack factory error
    */
  HTTPD_SVC_ACK_FACTORY_ERROR,
  /*!
    ack get ap list error
    */
  HTTPD_SVC_ACK_GET_AP_LIST_ERROR,
  /*!
    ack build ap list error
    */
  HTTPD_SVC_ACK_BUILD_AP_LIST_ERROR,
  /*!
    ack get ap status error
    */
  HTTPD_SVC_ACK_GET_AP_STATS_ERROR,
  /*!
    ack build ap status error
    */
  HTTPD_SVC_ACK_BUILD_AP_STATS_ERROR,
  /*!
    ack connect ap error
    */
  HTTPD_SVC_ACK_CONNECT_AP_ERROR,
  /*!
    ack disconnect ap error
    */
  HTTPD_SVC_ACK_DISCONNECT_AP_ERROR,
  /*!
    media control error 
    */
  HTTPD_SVC_ACK_MEDIA_CONTROL_ERROR,
  /*!
    mirror control error 
    */
  HTTPD_SVC_ACK_MIRROR_CONTROL_ERROR,
  /*!
    mirror data error 
    */
  HTTPD_SVC_ACK_MIRROR_DATA_ERROR,
  /*!
    get channel list error(zmcast) 
    */
  HTTPD_SVC_ACK_GET_CH_LIST_ERROR,
  /*!
    get current channel error(zmcast) 
    */
  HTTPD_SVC_ACK_GET_CUR_CH_ERROR,
  /*!
    set authority error(zmcast) 
    */
  HTTPD_SVC_ACK_SET_AUTH_ERROR,
  /*!
    get authority error(zmcast) 
    */
  HTTPD_SVC_ACK_GET_AUTH_ERROR,
  /*!
    ack no connected 
    */
  HTTPD_SVC_ACK_NO_CONNECTED,
}httpd_svc_ack_t;

typedef enum
{
  /*!
    rtsp protocol
    */
   PRO_RTSP,
  /*!
    http protocol
    */   
   PRO_HTTP,

}trans_protocol_t;

/*!
  rc_xml_pg_t
  */
typedef struct
{
  /*!
    utf-8 format name
    */
  char name[32];
  /*!
    id
    */
  u16 id;
  /*!
    sid
    */
  u16 sid;
  /*!
    sid
    */
  u16 org_nw_id;
  /*!
    sid
    */
  u16 stream_id;
  /*!
    utf-8 format name
    */
  char sat_name[32];
  /*!
    sat id
    */
  u16 sat_id;
  /*!
    sat id
    */
  u16 tp_id;
  /*!
    freq
    */
  u32 freq;
  /*!
    sym
    */
  u16 sym;
  /*!
    0: H;  1:V
    */
  u8 pol;
  /*!
    srcam
    */
  u8 scramble;
  /*!
    tv
    */
  u8 tv;
  /*!
    protocol,0: rtsp, 1:http
    */  
  u8 protocol;
  /*!
    url
    */  
  u8 url[128];
}rc_xml_pg_t;

/*!
  rc_xml_pg_root_t
  */
typedef struct
{
  /*!
    cur page
    */
  u16 cur_page;
  /*!
    total page
    */
  u16 total_page;
  /*!
    pg num
    */
  u8 pg_num;
  /*!
    res
    */
  u8 res1;
  /*!
    res2
    */
  u16 res2;
  /*!
    p_list
    */
  rc_xml_pg_t *p_list;
}rc_xml_pg_root_t;


/*!
  rc_xml_pg_root_t
  */
typedef struct
{
  /*!
    cur page
    */
  u16 pg_id_start;
  /*!
    total page
    */
  u16 cnt;
  /*!
    pg num
    */
  u16 total_program;
  /*!
    res
    */
  u8 res1;
  /*!
    res2
    */
  u16 res2;
  /*!
    p_list
    */
  rc_xml_pg_t *p_list;
}rc_xml_pg_root_by_id_t;

/*!
  rc_xml_common_t
  */
typedef struct
{
  /*!
    version
    */
  char version[8];
  /*!
    type
    */
  rc_xml_type_t type;
}rc_xml_common_t;

/*!
  rc_xml_control_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    key
    */
  u16 key;
}rc_xml_control_t;

/*!
  rc_xml_input_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    input
    */
  char input[MAX_RC_INPUT_LEN];
}rc_xml_input_t;

/*!
  rc_xml_ch_list_get_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    page idx
    */
  u16 page_idx;
}rc_xml_ch_list_get_t;

/*!
  rc_xml_auth_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    password
    */
  char password[32];
  char dest[16];
  char priority;
}rc_xml_auth_t;

/*!
  rc_xml_ch_list_get_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    page idx
    */
  u16 pg_id_start;
  /*!
    page idx
    */
  u16 cnt;
}rc_xml_ch_list_get_by_id_t;

/*!
  rc_xml_ch_list_get_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    page idx
    */
  u16 tp_id;
  /*!
    page idx
    */
  u16 page_idx;
}rc_xml_tp_ch_list_get_t;


/*!
  rc_xml_ch_list_get_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    page idx
    */
  u16 tp_id;
  /*!
    page idx
    */
  u16 pg_id_start;
  /*!
    page idx
    */
  u16 cnt;
}rc_xml_tp_ch_list_get_by_id_t;


/*!
  rc_xml_pg_play_t
  */
typedef struct
{
  /*!
    rc_xml_common_t
    */
  rc_xml_common_t common;
  /*!
    pg_id
    */
  u16 pg_id;
  /*!
    name
    */
  u8 name[MAX_NAME_LEN];
}rc_xml_pg_play_t;

/*!
  rc_xml_pg_play_t
  */
typedef struct
{
/*!
  rc_xml_common_t
  */
  rc_xml_common_t common;
  /*!
    ip
    */
  char ip[MAX_NAME_LEN];
}rc_xml_ip_t;

/*!
  create ack xml
  */
BOOL rc_xml_create_ack(u8 *p_buf, u32 buf_size, rc_xml_result_t ret_code);
/*!
  create evt xml
  */
BOOL rc_xml_create_event(u8 *p_buf, u32 buf_size, rc_xml_evt_t evt);
/*!
  rc_xml_create_channel_list
  */
BOOL rc_xml_create_channel_list(u8 *p_buf, u32 buf_size, rc_xml_pg_root_t *p_root);
/*!
  rc_xml_create_channel_list
  */
BOOL rc_xml_create_channel_list_by_id(u8 *p_buf, u32 buf_size, rc_xml_pg_root_by_id_t *p_root);
/*!
  rc_xml_create_cur_play_info
  */
BOOL rc_xml_create_cur_play_info(u8 *p_buf, u32 buf_size, rc_xml_pg_t *p_pg);
/*!
  rc_xml_create_cur_program
  */
BOOL rc_xml_create_cur_program(u8 *p_buf, u32 buf_size, rc_xml_pg_t *p_pg);
/*!
  rc_xml_parse_control
  */
RET_CODE rc_xml_parse_control(u8 *p_buf, u32 buf_size, rc_xml_control_t *p_control);
/*!
  rc_xml_parse_input
  */
RET_CODE rc_xml_parse_input(u8 *p_buf, u32 buf_size, rc_xml_input_t *p_input);
/*!
  rc_xml_parse_ch_list_get
  */
RET_CODE rc_xml_parse_ch_list_get(u8 *p_buf, u32 buf_size, rc_xml_ch_list_get_t *p_list);
/*!
  rc_xml_parse_ch_list_get
  */
RET_CODE rc_xml_parse_ch_list_get_by_id(u8 *p_buf, u32 buf_size, 
                                                            rc_xml_ch_list_get_by_id_t *p_list);
/*!
  rc_xml_parse_ch_list_get
  */
RET_CODE rc_xml_parse_tp_ch_list_get(u8 *p_buf, u32 buf_size, rc_xml_tp_ch_list_get_t *p_list);
/*!
  rc_xml_parse_ch_list_get
  */
RET_CODE rc_xml_parse_tp_ch_list_get_by_id(u8 *p_buf, u32 buf_size, 
                                               rc_xml_tp_ch_list_get_by_id_t *p_list);
/*!
  rc_xml_parse_pg_play
  */
RET_CODE rc_xml_parse_pg_play(u8 *p_buf, u32 buf_size, rc_xml_pg_play_t *p_play);
/*!
  rc_xml_parse_pg_play
  */
RET_CODE rc_xml_parse_media_ip_play(u8 *p_buf, u32 buf_size, rc_xml_pg_play_t *p_play);
/*!
  rc_xml_parse_heartbeat
  */
RET_CODE rc_xml_parse_heartbeat(u8 *p_buf, u32 buf_size, rc_xml_ip_t *p_ip);
/*!
  rc_xml_parse_ip
  */
RET_CODE rc_xml_parse_ip(u8 *p_buf, u32 buf_size, rc_xml_ip_t *p_ip);
/*!
  rc_xml_parse_connect
  */
RET_CODE rc_xml_parse_connect(u8 *p_buf, u32 buf_size, rc_xml_ip_t *p_ip);
/*!
  rc_xml_parse_disconnect
  */
RET_CODE rc_xml_parse_disconnect(u8 *p_buf, u32 buf_size, rc_xml_ip_t *p_ip);
/*!
  rc_xml_parse_server_change_ch
  */
RET_CODE rc_xml_parse_server_change_ch(u8 *p_buf, u32 buf_size, u32 *pg_id);

/*!
  httpd_svc_xml_build_ack
  */  
BOOL httpd_svc_xml_build_ack(u8 *p_buf, u32 buf_size, httpd_svc_ack_t ret_code);
/*!
  rc_xml_parse_set_auth
  */
RET_CODE rc_xml_parse_set_auth(u8 *p_buf, u32 buf_size, rc_xml_auth_t *p_auth);
/*!
  rc_xml_parse_get_auth
  */
RET_CODE rc_xml_parse_get_auth(u8 *p_buf, u32 buf_size, rc_xml_auth_t *p_auth);
/*!
  rc_xml_create_auth
  */
BOOL rc_xml_create_auth(u8 *p_buf, u32 buf_size, rc_xml_auth_t *p_auth);
/*!
  rc_xml_create_message
  */
BOOL rc_xml_create_message(u8 *p_buf, u32 buf_size, const char *message);
#ifdef __cplusplus
}
#endif

#endif

