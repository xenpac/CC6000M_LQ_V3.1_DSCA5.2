/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HTTPD_SERV_H__
#define __HTTPD_SERV_H__

/*!
  max len
  */
#define MAX_HTTPD_IP_LEN    (17)

/*!
  httpd server process callbacb
  */
typedef struct httpd_svc_var
{
  /*!
    p_name
    */  
  char *p_name;
  /*!
    p_value
    */
  char *p_value;
  /*!
    p_nextvalue
    */
  struct httpd_svc_var *p_nextvalue;
  /*!
    p_nextvariable
    */
  struct httpd_svc_var *p_nextvariable;
} http_svc_var_t;

/*!
  max len
  */
typedef struct {
  /*!
    clientAddr
    */
  char clientAddr[MAX_HTTPD_IP_LEN];
  /*!
    p_readbuf
    */
  char *p_readbuf;
  /*!
    p_content
    */
  char *p_content;
  /*!
    content_len
    */
  u16 content_len;
} httpd_svc_param_t;



/*!
  httpd server process callbacb
  */
typedef void(*httpd_cb)(handle_t handle, http_svc_var_t *p_var);

/*!
  httpd service commands
  */
typedef enum
{
  /*!
    config network ip address
    */
  HTTPD_START,
  /*!
    network ping test
    */
} httpd_svc_cmd_t;

/*!
  httpd service commands
  */
#define SVC_LEFT    (16)

/*!
  httpd service events
  */
typedef enum tag_httpd_svc_evt
{
  /*!
    Flag of evt start
    */
  HTTPD_SVC_EVT_START = (MDL_HTTPD_SVC << SVC_LEFT),
  /*!
    Flag of evt end
    */
  HTTPD_SVC_EVT_END
}httpd_svc_event_t;

/*!
  init parameter hpptd service.
  */
typedef struct tag_httpd_svc_init_para
{
  /*!
    service's priority.
    */
  u32 service_prio;
  /*!
    service's stack size.
    */
  u32 nstksize;
}httpd_svc_init_para_t;


/*!
  httpd service do cmd param
  */
typedef struct tag_httpd_svc_cmd_para 
{

  /*!
    network service ethernet device handle
    */
  void *p_eth_dev;
} httpd_svc_cmd_para_t;

/*!
  net svc class declaration
  */
typedef struct tag_httpd_svc
{
  /*!
    virtual function to create module
    */
  RET_CODE (*server_create)(class_handle_t handle, httpd_cb cb);
    
  /*!
    virtual function to destory module
    */
  void (*server_destory)(class_handle_t handle);
  /*!
    virtual function to start module
    */
  void (*start)(class_handle_t handle);
  /*!
    httpd_add_static_content
    */
  void (*add_static_content)(char *p_dir, char *p_name,
    BOOL index_flag, int (*preload)(), char *p_data);
  /*!
    httpd_add_static_content
    */
  void (*add_c_content)(char *p_dir, char *p_name,
    BOOL index_flag, int (*preload)(), void (*function)());
  /*!
    output
    */
  void (*output)(char *p_msg);
  /*!
    output
    */
  void (*send_header)(void);
  /*!
    output
    */
  void (*set_response)(char *p_msg);
  /*!
    output
    */
  void (*get_struct)(httpd_svc_param_t *p_out, void *p_server);
  /*!
    get a service handle from net svc module
    */
  service_t * (*get_svc_instance)(class_handle_t handle, u32 context);
  /*!
    net svc class privated data
    */
  void *p_data;
}httpd_svc_t;

/*!
  httpd_post_to_mb_device.

  */
RET_CODE httpd_post_to_mb_device(char *p_data, u32 data_size);

/*!
  httpd_set_mb_device_ip.

  */
void httpd_set_mb_device_ip(char *p_ip);

/*!
  init httpd service.

  \param[in] service_prio   service task priority.

  \return TRUE or FALSE.
  */
handle_t httpd_service_init(httpd_svc_init_para_t *p_para);

#endif
