/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifndef __PRESET_DATA_H__
#define __PRESET_DATA_H__

#define MAX_NAME_LENGTH 16

typedef enum
{
  PRESET_SAT = 0,
  PRESET_PRO,
  PRESET_TP
}preset_type_t;

typedef enum
{
  PRESET_PG_AREA_DOMESTIC = 0, 
  PRESET_PG_AREA_OVERSEA,
  PRESET_PG_AREA_ALL
}preset_pg_area_t;

/*!
 * load preset config 
 */
typedef enum
{
  PRESET_PG_TYPE_ABS = 0, 
  PRESET_PG_TYPE_DVBS,
  PRESET_PG_TYPE_ALL
}preset_pg_type_t;


void db_dvbs_load_preset(u8 blockid, preset_pg_type_t pg_type, preset_pg_area_t pg_area);

#endif

