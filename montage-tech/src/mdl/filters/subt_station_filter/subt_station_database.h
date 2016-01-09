/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SUBT_STATION_DATABASE_H__
#define __SUBT_STATION_DATABASE_H__

/*!
  sst_in_pin_config
  */
typedef struct tag_subt_station_key_point
{
  /*!
    start pts
    */
  u32 start;
  /*!
    end pts
    */
  u32 end;
  /*!
    pos bytes
    */
  u32 pos;
}subt_station_key_point_t;


/*!
  sst_in_pin_config
  */
typedef struct tag_subt_station_in_item
{
  /*!
    start pts
    */
  u32 start;
  /*!
    end pts
    */
  u32 end;
  /*!
    data
    */
  u8 *p_data;
  /*!
    used
    */
  BOOL used;
  /*!
    data len
    */
  u16 data_len;
}subt_station_in_item_t;


/*!
  sst_db_find_item
  */
RET_CODE sst_db_find_out_item(u32 pts, subt_station_key_point_t *p_key);

/*!
  sst_db_save_item
  */
void sst_db_save_out_item(subt_station_key_point_t *p_key);

/*!
  sst_db_save_item
  */
void sst_db_save_in_item(subt_station_in_item_t *p_item);

/*!
  sst_db_save_item
  */
RET_CODE sst_db_find_in_item(u32 pts, subt_station_in_item_t *p_item);
/*!
  sst_db_reset
  */
void sst_db_reset(void);

/*!
  sst_db_set_item_src
  */
void sst_db_set_item_src(BOOL internal_subt);
/*!
  sst_db_init
  */
void sst_db_init(void);

/*!
  sst_db_deinit
  */
void sst_db_deinit(void);
  
#endif

