/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SUBT_STATION_FILTER_INTRA_H__
#define __SUBT_STATION_FILTER_INTRA_H__
/*!
  max buf size
  */
#define MAX_SUBT_DATA_SIZE  (512)

/*!
  output
  */
typedef enum
{
  /*!
    STATE_HIDE
    */
  STATE_HIDE,
  /*!
    STATE_SHOW
    */
  STATE_SHOW
}subt_station_filter_status_t;


/*!
  private data tag_subt_filter_private
  */
typedef struct tag_subt_station_filter_private
{
  /*!
    this point !!
    */
  u8 *p_this;
  /*!
    input
    */
  subt_station_in_pin_t m_in_pin; 
  /*!
    output
    */
  subt_station_out_pin_t m_out_pin;
  /*!
    output
    */
  //u8 type;
  /*!
    state
    */
  subt_station_filter_status_t state;
  /*!
    pts_get
    */
  subt_station_pts_get_func pts_get;
  /*!
    TRUE means the subt data come from the stream, not a file,
    should set the callback to get subt data
    */
  BOOL internal_subt;

  /*!
    subt_data_get
    */
  subt_station_subt_data_get_func subt_data_get;
  /*!
    subt_data_get
    */
  u32 start;  
  BOOL nomal_play;
  u8 subt_data[MAX_SUBT_DATA_SIZE];

}subt_station_filter_priv_t;




/*!
  the subt dec filter define
  */
typedef struct tag_subt_station_dec_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data buffer
    */
  subt_station_filter_priv_t private_data;
}subt_station_filter_t;


#endif

