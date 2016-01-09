/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __DB_PLAY_HIST__
#define __DB_PLAY_HIST__

typedef struct
{
  u32 vdo_id;
  u32 res_id;
  u8  b_single_page;
  u16 pg_name[MAX_PG_NAME_LEN+1];
} play_hist_info;

void db_plht_sort_time_table(void);

u16 db_plht_get_count(void);

BOOL db_plht_get_item_by_idx(u16 index, play_hist_info *p_info);

void db_plht_add_item(play_hist_info *p_info);

void db_plht_add_unique_item(play_hist_info *p_info);

void db_plht_up_item_by_idx(u16 index, play_hist_info *p_info);

void db_plht_del_item_by_idx(u16 index);

void db_plht_del_all(void);

#endif