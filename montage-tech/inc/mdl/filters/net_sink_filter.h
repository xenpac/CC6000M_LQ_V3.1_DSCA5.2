/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NET_SINK_FILTER_H_
#define __NET_SINK_FILTER_H_

/*!
  MAX_USER_LEN
  */
#define MAX_USER_LEN (32)
/*!
  MAX_PASSWORD_LEN
  */
#define MAX_PASSWORD_LEN (64)
/*!
  MAX_ADDR_LEN
  */
#define MAX_ADDR_LEN (32)
/*!
  MAX_REMOTE_PATH_LEN
  */
#define MAX_REMOTE_PATH_LEN (256)

/*!
  net_sink_cfg_t
  */
typedef struct tag_net_sink_cfg
{
  /*!
    user
    */
  u8 user[MAX_USER_LEN];
  /*!
    passwd
    */
  u8 passwd[MAX_PASSWORD_LEN];
  /*!
    addr
    */
  u8 addr[MAX_ADDR_LEN];
  /*!
    remote path
    */
  u8 remote_path[MAX_REMOTE_PATH_LEN];
}net_sink_cfg_t;

/*!
  net_sink_mode_t
  */
typedef enum tag_net_sink_mode
{
  /*!
    ftp
    */
  NET_SINK_MODE_FTP = 0,
  /*!
    http
    */
  NET_SINK_MODE_HTTP
}net_sink_mode_t;

/*!
  net_sink_filter_cmd_t
  */
typedef enum tag_net_sink_filter_cmd
{
  /*!
    config read file' name
    */
  NET_SINK_CFG_FILE_NAME,
  /*!
    config sink mode
    */
  NET_SINK_CFG_SINK_MODE,
}net_sink_filter_cmd_t;

/*!
  net_sink_filter_evt_t
  */
typedef enum tag_net_sink_filter_evt
{
  /*!
    file sink get first data
    */
  NET_SINK_MSG_READY = NET_SINK_FILTER << 16,
  /*!
    no enough mem
    */
  NET_SINK_MSG_NO_ENOUGH_MEM,
  /*!
    record save file, when rec file is 4G
    */
  NET_SINK_MSG_SAVE_FILE,
  /*!
    record write error
    */
  NET_SINK_MSG_WRITE_ERROR,
}net_sink_filter_evt_t;

/*!
  create a net sink filter instance
  filetr ID:NET_SINK_FILTER
  \return return the instance of fsrc_filter
  */
ifilter_t * net_sink_filter_create(void *p_para);

#endif // End for __NET_SINK_FILTER_H_