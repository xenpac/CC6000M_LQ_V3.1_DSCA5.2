/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NET_SRC_FILTER_H_
#define __NET_SRC_FILTER_H_

/*!
  file source filter source
  */
typedef enum tag_net_src_filter_source
{
  /*!
    source from file
    */
  SRC_FROM_SERVER = 0,
 
}net_src_filter_source_t;
/*!
  weather addr
  */
typedef struct tag_src_pin_addr
{
  /*!
    weather addr
    */
  u8 weather_addr[50];
  /*!
    weather sub addr
    */
  u8 weather_sub_addr[50];
  /*!
    woeid addr
    */
  u8 woeid_addr[50];
  /*!
    woeid sub addr
    */
  u8 woeid_sub_addr[250];
  /*!
    city
    */
  u8 city[50];
}net_src_pin_addr_t;
/*!
  define source pin attribute
  */
typedef struct tag_net_src_pin_attr
{
  /*!
    is circular read, default to no circular
    */
  BOOL is_circular;
  /*!
    is share read, default to engross
    */
  BOOL is_share;
  /*!
    Number of buffers created. default to 1, ignore 0.
    */
  u32 buffers;
  /*!
    Size of each buffer in bytes. default to KBYTE, ignore 0.
    */
  u32 buffer_size;
  /*!
    if user need use extern buffer, set it 
    */
  u8 *p_extern_buf;
  /*!
    if p_extern_buf not NULL, it's valid
    */
  u32 extern_buf_size;
  /*!
    fsrc filter source from
    */
  net_src_filter_source_t src;
}net_src_pin_attr_t;

/*!
  net_src_url_addr_cfg_t
  */
typedef struct tag_net_src_url_addr_cfg
{
  /*!
    url_addr
    */
  u8 *p_url_addr;
  /*!
    context
    */
  u32 context;
  /*!
    is_once_get_all_data
    */
  u32 is_once_get_all_data;
}net_src_url_addr_cfg_t;

/*!
  net_src_init_param_t
  */
typedef struct tag_net_src_init_param
{
  /*!
    priority
    */
  u32 priority;
  /*!
    stack_size
    */
  u32 stack_size;
  /*!
    max_once_download_size, for get all
    */
  u32 max_once_download_size;
  /*!
    timeout secord
    */
  u32 timeout;
}net_src_init_param_t;

/*!
  file source filter command define
  */
typedef enum tag_net_src_filter_cmd
{
  /*!
    config read file' name
    */
  NET_SRC_CFG_URL_ADDR,
  /*!
    config read file' name
    */
  NET_SRC_CANCEL_REQUEST,
  /*!
    config source buffer
    */
  NET_SRC_CFG_SRC_BUFFER,
  /*!
    config source pin attribute //see src_pin_attr_t
    */
  NET_SRC_CFG_SOURCE_PIN,
  
}net_src_filter_cmd_t;


/*!
net source filter event define
*/
typedef enum tag_net_src_filter_evt
{
  /*!
	config read file' name
	*/
  NET_SRC_FILTER_FTP_CONNECT_FAIL = NET_SOURCE_FILTER << 16,
  /*!
	get file error
	*/
  NET_SRC_FILTER_FTP_GET_FILE_ERROR,
}net_src_filter_evt_t;

/*!
  create a file source instance
  filter ID:FILE_SOURCE_FILTER
  \return return the instance of fsrc_filter
  */
ifilter_t * net_src_filter_create(void *p_para);
/*!
  net_source_svc_init
  create a task for net data response
  \return return the instance of fsrc_filter
  */
handle_t net_source_svc_init(u32 priority, u32 stack_size);


/*!
  net_source_svc_init
  create a task for net data response
  \return return the instance of fsrc_filter
  */
handle_t net_source_svc_init_v2(net_src_init_param_t *p_init_para);

#endif // End for __FILE_SRC_FILTER_H_

