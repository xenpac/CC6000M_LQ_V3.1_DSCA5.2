/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __LIVETV_DB_H__
#define __LIVETV_DB_H__

#define INVALID_ID      (0xffff)
#define INVALID_GRP_ID      (0xff)
#define INVALID_GRP      (0xff)

#define MAX_LIVE_GROUP  (40)

#define MAX_LIVE_PRG      (3000)

#define MAX_LIVE_URL_LEN      (1024)

#define MAX_LIVE_NAME_LEN       (32)

#define MAX_LIVE_SRC_PER_PG      (16)

//#define LIVETV_CACHE_SIZE    (2 * MBYTES)
//#define DEST_SIZE       (64 * KBYTES)

typedef struct
{
  u16 id;
  u8 grp_view_id;
  u8 active_flg;
  u8 url_cnt;
  u8 src_idx;
  u16 def_order;
  u8 name[MAX_LIVE_NAME_LEN];
  u8* url[MAX_LIVE_SRC_PER_PG];
  u32 sate_id;
  u16 sate_apid;
  u16 sate_vpid;
  u16 sate_pcrpid;
  u32 sate_freq;
  u32 sate_sym;
  BOOL sate_polarity;
  BOOL sate_tv_flag;
  
  
  
}live_db_prg_t;

typedef struct
{
  u8 id;
  u8 view_id;
  char name[MAX_LIVE_NAME_LEN];
}live_db_group_t;

typedef struct
{
  u8 *p_block_buf;
  u32 buf_size;
  u8 max_group;
  u16 max_prg;
  u8 db_block_id;
  u8 use_bak;
}livetv_db_init_t;

typedef struct
{
  u16 idx;
  u32 p_addr;
}live_db_sort_info_t;

typedef struct
{
  u32 crc_ver;
  u8 max_grp;
  u8 valid_grp;
  u16 max_prg;
  u16 valid_pg;
  live_db_prg_t prog[MAX_LIVE_PRG];
  live_db_group_t group[MAX_LIVE_GROUP];
  live_db_sort_info_t sort_by_id[MAX_LIVE_PRG];
  live_db_sort_info_t sort_by_grp[MAX_LIVE_PRG];
  u16 grp_end_pos[MAX_LIVE_GROUP];
}live_db_t;

RET_CODE livetv_db_get_pos_by_id(u16 *p_pos, u16 id);

RET_CODE  livetv_set_pg_cur_source(u16 pg_id, u8 src_idx);

RET_CODE livetv_db_get_group_view_id(u8 *p_grp_view, u8 group_idx);

RET_CODE  livetv_write_group_info(live_db_group_t *p_group);

RET_CODE livetv_add_pg_info(live_db_prg_t *p_pg);

//RET_CODE  livetv_edit_pg_info(live_db_prg_t *p_pg);

void livetv_set_crc_ver(u32 crc);

u32 livetv_get_crc_ver(void);

u16 livetv_get_cnt_by_group(u8 group_view_id);

RET_CODE livetv_get_group_cnt(u8 *p_cnt);

RET_CODE livetv_get_group_info(live_db_group_t *p_group, u16 group_view_id);

RET_CODE livetv_get_pg_by_group_pos(live_db_prg_t *p_pg, u16 pos, u16 group_view_id);

BOOL livetv_get_pg_by_id(live_db_prg_t *p_pg, u16 id);

RET_CODE livetv_db_save(u8 *p_data, u32 size);

//RET_CODE livetv_db_load(u8 *p_data, u32 *p_size);

void livetv_db_create_view(void);

void livetv_db_init(livetv_db_init_t *p_param);

void livetv_clear_db(void);

u32 livetv_db_size_get(void);

#endif

