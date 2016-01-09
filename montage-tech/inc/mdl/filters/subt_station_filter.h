/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SUBT_STATION_FILTER_H__
#define __SUBT_STATION_FILTER_H__


/*!
  subt_station_pts_get_func
  */
typedef u32 (*subt_station_pts_get_func)(void);

/*!
  subt_station_subt_data_get_func
  p_addr: return the data addr
  p_size: return the data size
  p_pts: return the pts of this group data
  */
typedef RET_CODE (*subt_station_subt_data_get_func)(u8 **pp_addr, int *p_size, int *p_pts);
/*!
  subt filter command define
  */
typedef enum tag_subt_station_filter_cmd
{
  /*!
    config params see subt_filter_params_cfg_t
    */
  SUBT_STATION_CMD_CONFIG,
  /*!
    start.
    \param[in] 
    */
  SUBT_STATION_CMD_START,
}subt_station_filter_cmd_t;


/*!
  subt_station_params_cfg_t
  */
typedef struct
{
  /*!
    subt type see subt_sys_type_t
    */
  u8 type;
  /*!
    reserved1
    */
  u8 reserved1;
  /*!
    reserved1
    */
  u16 resrved2;
  /*!
    align pos in region
    */  
  str_align_t align;
  /*!
    this func should return ms
    */
  subt_station_pts_get_func pts_get;
  /*!
    TRUE means the subt data come from the stream,
    not a file, should set the callback to get subt data
    */
  BOOL internal_subt;

  /*!
    subt_data_get
    */
  subt_station_subt_data_get_func subt_data_get;
}subt_station_params_cfg_t;



/*!
  subt_station_pts_in_t
  */
typedef struct
{
  /*!
    subt_station_pts_in_t
    */
  u32 time_stamp_ms;
}subt_station_pts_in_t;


/*!
  subt_filter_create
  */
ifilter_t * subt_station_filter_create(void *p_para);

#endif

