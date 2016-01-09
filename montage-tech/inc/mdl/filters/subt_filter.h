/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SUBT_FILTER_H__
#define __SUBT_FILTER_H__


/*!
  subtitle dbg handle
  */
#define SUBT_DBG   DUMMY_PRINTF
/*!
  subtitle dbg handle
  */
//#define SUBT_DBG   OS_PRINTF

/*!
  subt filter command define
  */
typedef enum tag_subt_filter_cmd
{
  /*!
    config params see subt_filter_params_cfg_t
    */
  SUBT_FILTER_CMD_CFG_PARAMS,
  /*!
    set page no.
    \param[in] lpara: composition_page  << 16 | ancillary_page
    */
  SUBT_FILTER_CMD_SET_PAGE,
  /*!
    set the incoming pes pid
    */
  SUBT_FILTER_CMD_SET_PID,
  /*!
    set the video standrad pal or ntsl
    */
  SUBT_FILTER_CMD_SET_STD,
  /*!
    SUBT_FILTER_CMD_STOP
    */
  SUBT_FILTER_CMD_STOP,  
}subt_filter_cmd_t;


/*!
  subt_filter_params_cfg_t
  */
typedef struct
{
  /*!
    layer id
    */
  u32 layer_id;
}subt_filter_params_cfg_t;


/*!
  subt_filter_create
  */
ifilter_t * subt_filter_create(void *p_para);

#endif

