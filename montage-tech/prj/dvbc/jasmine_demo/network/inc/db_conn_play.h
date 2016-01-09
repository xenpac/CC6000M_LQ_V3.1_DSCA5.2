/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __DB_CONN_PLAY__
#define __DB_CONN_PLAY__

typedef struct
{
    u32 play_time;
    u16 episode_num;  
} conn_play_info;


void db_cnpl_initialize(void);

s32 db_cnpl_get_item_by_key(u16 *p_pgName, conn_play_info *p_info);

void db_cnpl_add_item_by_key(u16 *p_pgName, conn_play_info *p_info);

void db_cnpl_up_item_by_hash_pos(u16 hash_pos, conn_play_info *p_info);

void db_cnpl_del_item_by_idx(u16 index);

void db_cnpl_del_all(void);


#endif