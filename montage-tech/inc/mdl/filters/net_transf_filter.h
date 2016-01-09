/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NET_TRANSF_FILTER_H__
#define __NET_TRANSF_FILTER_H__

/*!
  net transf filter type
  */
typedef enum
{
  /*!
    net albums filter
    */
  NET_TRANSF_FILTER_TYPE_ALBUMS = 0,
  /*!
    net map filter
    */
  NET_TRANSF_FILTER_TYPE_MAP,
  /*!
    net music filter
    */
  NET_TRANSF_FILTER_TYPE_MUSIC,
  /*!
    net news filter
    */
  NET_TRANSF_FILTER_TYPE_NEWS,
  /*!
    rss filter
    */
  NET_TRANSF_FILTER_TYPE_RSS,
  /*!
    net weather filter
    */
  NET_TRANSF_FILTER_TYPE_WEATHER,
}net_transf_filter_type_t;

/*!
  net transf filter command define
  */
typedef enum
{
  /*!
    config
    */
  NET_TRANSF_FILTER_CMD_CFG = 0,
  /*!
    request http data
    */
  NET_TRANSF_FILTER_CMD_REQ,
  /*!
    get http data from fifo db
    */
  NET_TRANSF_FILTER_CMD_GET,
  /*!
    cancel http request
    */
  NET_TRANSF_FILTER_CMD_CANCEL,
  /*!
    cancel all of http requests
    */
  NET_TRANSF_FILTER_CMD_CANCEL_ALL,
  /*!
    customize
    */
  NET_TRANSF_FILTER_CMD_CUSTOMIZE,
}net_transf_filter_cmd_t;

/*!
  create net transf filter
  */
ifilter_t *net_transf_filter_create(void *p_para);

#endif

