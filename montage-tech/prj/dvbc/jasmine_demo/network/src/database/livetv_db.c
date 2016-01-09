/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "drv_dev.h"
#include "charsto.h"
#include "gzip.h"
#include "lib_memp.h"
#include "data_manager.h"
#include "class_factory.h"

#include "livetv_db.h"
#include "string_pool.h"


#define STRUCT_ADDR   (64 * KBYTES- 12)
#define LEN_ADDR   (64 * KBYTES- 8)
#define LATEST_ADDR (64 * KBYTES - 2)
#define OK_ADDR (64 * KBYTES - 1)
#define OK_FLAG   0xAA


typedef enum
{
  DB_INVALID,
  DB_WRITING,
  DB_OK
}livetv_db_status_t;

typedef enum
{
  IDX_MODE,
  DEFAULT_MODE,
  GROUP_MODE
}livetv_sort_type_t;


typedef struct
{
  u16 start_pos;
  u16 end_pos;
}live_db_grp_info_t;


typedef struct
{
  u8 db_id;
  u8 check_valid;
  u8 blk_idx;
  lib_memp_t memp;
  //u8 *p_cache;
 // u8 *p_dest;
  live_db_t *p_db;
  livetv_db_status_t status;
  os_sem_t sem;
  u32 cur_struct_len;
  u8 use_bak;
  u32 strpool_hdl;
  BOOL is_str_dup;
  BOOL init_pool;
}live_db_priv_t;

typedef BOOL (*livesort_func)(live_db_prg_t *prev_node, live_db_prg_t *cur_node);

static live_db_priv_t g_live_db_priv = {0};

//static void livetv_test_write(void);

static live_db_priv_t *get_priv(void)
{
   return &g_live_db_priv;
}

static BOOL idx_cmp(live_db_prg_t *prev_node, live_db_prg_t*cur_node)
{
  return prev_node->id < cur_node->id;
}

static BOOL def_cmp(live_db_prg_t *prev_node, live_db_prg_t*cur_node)
{
  return prev_node->def_order < cur_node->def_order;
}

static BOOL grp_cmp(live_db_prg_t *prev_node, live_db_prg_t*cur_node)
{
  return prev_node->grp_view_id < cur_node->grp_view_id;
}

void _livetv_sort(live_db_sort_info_t *p_sort, int cnt, livetv_sort_type_t sort_type)
{
  int i = 0;
  int j = 0;
 // int k = 0;
  livesort_func sort_func = NULL;
//  live_db_priv_t *p_priv = get_priv();
  live_db_sort_info_t tmp = {0};
  
  switch(sort_type)
  {
    case DEFAULT_MODE:
      sort_func = def_cmp;
      break;
    case GROUP_MODE:
      sort_func = grp_cmp;
      break;
    case IDX_MODE:
      sort_func = idx_cmp;
      break;
    default:
      return;
  }

  for(i = 0; i < cnt; i ++)
  {
    for(j = i + 1; j < cnt; j ++)
    {
      if(sort_func((live_db_prg_t *)p_sort[j].p_addr, (live_db_prg_t *)p_sort[i].p_addr) > 0)
      {
        tmp = p_sort[i];
        p_sort[i] = p_sort[j];
        p_sort[j] = tmp;
       // OS_PRINTF("exchange %x and %x\n", p_sort[i].p_addr, p_sort[j].p_addr);
    //    for(k = 0; k < cnt; k ++)
    //    {
        //  OS_PRINTF("aftx %x and %x\n", p_sort[k].idx, p_sort[k].p_addr);
    //    }
      }
    }
  }
}

#if 0
static u8 check_a_b(u8 a, u8 b)
{
  if(a == 0xff && b == 0xff)
  {
    return 0;
  }
  else
  {
    if(a > b)
    {
      if(a == 0xff && b == 0x1)
      {
         return 1;
      }
      else
      {
        return 0;
      }
    }
    else
    {
      if(b == 0xff && a == 0x1)
      {
         return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  return 0;
}

#endif

RET_CODE livetv_db_get_group_view_id(u8 *p_grp_view, u8 group_idx)
{
  live_db_priv_t *p_priv = get_priv();
  u8 i = 0;
  BOOL is_find = FALSE;
  
  if(p_priv->p_db == NULL)
  {
    OS_PRINTF("####%s,11111##\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  
  mtos_sem_take(&p_priv->sem, 0);
/*  if(p_priv->status != DB_OK)
  {
    OS_PRINTF("####%s,22222222##\n", __FUNCTION__);
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }*/

  for(i = 0; i < MAX_LIVE_GROUP; i ++)
  {
    if(p_priv->p_db->group[i].id == group_idx)
    {
      *p_grp_view = i;
      //is_find = TRUE;
      mtos_sem_give(&p_priv->sem);
      return SUCCESS;
    }
  }
  if(!is_find)
  {
    *p_grp_view = INVALID_GRP_ID;
  }
  
  mtos_sem_give(&p_priv->sem);
  return ERR_FAILURE;
  
}

#if 0
RET_CODE livetv_db_load(u8 *p_data, u32 *p_size)
{
  live_db_priv_t *p_priv = get_priv();
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u32 offset = 0;
  u8 check_a = 0, check_b = 0;
 // u8 *p_cache = NULL;
//  u32 cache_size = 1024 * 1024;
  u32 src_len = 0;
//  u32 out_len = 0;
  u8 *p_src = NULL;
//  int ret = 0;
  u8 valid_block = 0xff;
//  u32 struct_len = 0;
//  return ;
  //p_priv->status = DB_INVALID;
  dm_direct_read(p_dm_handle, p_priv->db_id, OK_ADDR , 1, &check_a);
  if(p_priv->use_bak)
  {
    dm_direct_read(p_dm_handle, p_priv->db_id, OK_ADDR + CHARSTO_SECTOR_SIZE, 1, &check_b);
  }
  if(check_a ==  OK_FLAG  && check_b != OK_FLAG)
  {
    valid_block = 0;
  }
  else if(check_a !=  OK_FLAG  && check_b == OK_FLAG)
  {
    valid_block = 1;
  }
  else if(check_a !=  OK_FLAG  && check_b != OK_FLAG)
  {
    return ERR_FAILURE;
  }
  
  if(p_priv->use_bak)
  {
    dm_direct_read(p_dm_handle, p_priv->db_id, LATEST_ADDR, 1, &check_a);
    dm_direct_read(p_dm_handle, p_priv->db_id, LATEST_ADDR + CHARSTO_SECTOR_SIZE, 1, &check_b);
  }
  else
  {
    valid_block = 0;
  }
  
  if(valid_block == 0xff)
  {
    valid_block = check_a_b(check_a, check_b);
  }

  if(valid_block != 0xff)
  {
    p_priv->blk_idx = valid_block;
    offset = p_priv->blk_idx * CHARSTO_SECTOR_SIZE;
  }
  if(p_priv->blk_idx == 0)
  {
    p_priv->check_valid = check_a;
  }
  else
  {
    p_priv->check_valid = check_b;
  }
  

  dm_direct_read(p_dm_handle, p_priv->db_id, offset + LEN_ADDR, 4, (u8 *)&src_len);
  if(src_len > CHARSTO_SECTOR_SIZE)
  {
    return ERR_FAILURE;
  }
  *p_size = src_len;
  /*
  dm_direct_read(p_dm_handle, p_priv->db_id, offset + STRUCT_ADDR, 4, (u8 *)&struct_len);
  if(struct_len !=  p_priv->cur_struct_len)
  {
    return ERR_FAILURE;
  }
*/
  
  p_src = p_priv->p_dest;
  
  dm_direct_read(p_dm_handle, p_priv->db_id, offset, src_len, p_data);
  
/*
  out_len = sizeof(live_db_t);
  ret = gzip_uncompress((u8 *)p_priv->p_db, &out_len,
    (const u8 *)p_src, src_len, p_priv->p_cache, LIVETV_CACHE_SIZE);
  if(ret != 0)
  {
    OS_PRINTF("no data uncompressed\n");
  }  
*/
 // if(p_priv->p_db)
  p_priv->status = DB_OK;

  return SUCCESS;
}

#endif
//#define LIVEDB_DBG

void livetv_db_create_view(void)
{
  live_db_priv_t *p_priv = get_priv();
  live_db_t *p_db = p_priv->p_db;
  u16 i = 0;
  u8 grp_cnt = 0;
  u16 item_in_grp = 0;
  u16 cur_start = 0;

  mtos_sem_take(&p_priv->sem, 0);
  p_priv->status = DB_WRITING;
  // pg id sort
  for(i = 0; i < p_db->max_prg; i ++)
  {
    p_db->sort_by_id[i].idx = p_db->prog[i].id;
    p_db->sort_by_id[i].p_addr = (u32)&p_db->prog[i];
  //  OS_PRINTF("idx %d  addr %x\n", p_db->sort_by_id[i].idx, p_db->sort_by_id[i].p_addr);
  }
  _livetv_sort(p_db->sort_by_id, p_db->max_prg, IDX_MODE);

#ifdef LIVEDB_DBG  

  for(i = 0; i < p_db->max_prg; i ++)
  {
//   OS_PRINTF("aft idx %d  addr %x\n", p_db->sort_by_id[i].idx, p_db->sort_by_id[i].p_addr);
  }
#endif

  //group sort
  for(i = 0; i < p_db->max_prg; i ++)
  {
    p_db->sort_by_grp[i].idx = p_db->prog[i].grp_view_id;
    p_db->sort_by_grp[i].p_addr = (u32)&p_db->prog[i];
//    OS_PRINTF("idx %d  addr %x\n", p_db->sort_by_grp[i].idx, p_db->sort_by_grp[i].p_addr);
  }

  _livetv_sort(p_db->sort_by_grp, p_db->max_prg, GROUP_MODE);
#ifdef LIVEDB_DBG  

  for(i = 0; i < p_db->max_prg; i ++)
  {
    OS_PRINTF("aft idx %d  addr %x\n", p_db->sort_by_grp[i].idx, p_db->sort_by_grp[i].p_addr);
  }
#endif

  //find group end
  for(i = 0; i < p_db->max_prg - 1 && i < p_priv->p_db->valid_pg - 1; i ++)
  {
 //   OS_PRINTF("##9944idx == %d##\n", p_db->sort_by_grp[i + 1].idx);
    if(p_db->sort_by_grp[i + 1].idx != p_db->sort_by_grp[i].idx)
    {
      p_db->grp_end_pos[grp_cnt] = i;
      OS_PRINTF("grp[%d] end %d\n", grp_cnt, i);
      grp_cnt ++;
    }
  }
  p_db->grp_end_pos[grp_cnt] = i;
  OS_PRINTF("grp[%d] end %d\n", grp_cnt, i);
  grp_cnt ++;

  //sort in group by def order
  for(i = 0; i < grp_cnt - 1; i ++)
  {
    item_in_grp = (u16)(p_db->grp_end_pos[i] - cur_start + 1);
    if(item_in_grp > 1)
    {
      _livetv_sort(p_db->sort_by_grp + cur_start, item_in_grp, DEFAULT_MODE);
    }
    cur_start = (u16)(p_db->grp_end_pos[i] + 1);
  }

  //print all
 // for(i = 0; i < p_db->max_prg; i ++)
  {
 //   OS_PRINTF("aftdd idx %d  addr %x\n", p_db->sort_by_grp[i].idx, p_db->sort_by_grp[i].p_addr);
  }
  p_priv->status = DB_OK;

  mtos_sem_give(&p_priv->sem);
}


BOOL livetv_get_pg_by_id(live_db_prg_t *p_pg, u16 id)
{
  u16 c_start = 0, c_end = MAX_LIVE_PRG;
  live_db_priv_t *p_priv = get_priv();
  u16 cur = c_end / 2;
  u16 cnt = MAX_LIVE_PRG;

  if(p_priv->status != DB_OK)
  {
    return FALSE;
  }
  mtos_sem_take(&p_priv->sem, 0);
  do
  {
    cur = (c_end  - c_start) / 2 + c_start;
    if(p_priv->p_db->sort_by_id[cur].idx < id)
    {
      c_start = cur;
    }
    else
    {
      c_end = cur;
    }

    if(p_priv->p_db->sort_by_id[cur].idx == id)
    {
      memcpy(p_pg, (u8 *)p_priv->p_db->sort_by_id[cur].p_addr, sizeof(live_db_prg_t));
      OS_PRINTF("find pg cnt %d\n", cnt);
      mtos_sem_give(&p_priv->sem);
      return TRUE;
    }
    
  }while(cnt --);

//  if()
  mtos_sem_give(&p_priv->sem);
  return FALSE;
}

RET_CODE livetv_get_group_info(live_db_group_t *p_group, u16 group_view_id)
{
  live_db_priv_t *p_priv = get_priv();
  if(group_view_id >= MAX_LIVE_GROUP || NULL == p_group)
  {
    return ERR_PARAM;
  }
  mtos_sem_take(&p_priv->sem, 0);
  if(p_priv->status != DB_OK)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  memcpy(p_group, &(p_priv->p_db->group[group_view_id]), sizeof(live_db_group_t));
  mtos_sem_give(&p_priv->sem);
  return SUCCESS;
}


u16 livetv_get_cnt_by_group(u8 group_view_id)
{
  live_db_priv_t *p_priv = get_priv();
//  live_db_prg_t *p_pg = NULL;
  u16 grp_start = 0;
  u16 valid = 0;
//  u16 i = 0;

  if(group_view_id >= MAX_LIVE_GROUP && group_view_id != INVALID_GRP)
  {
    return (u16)ERR_FAILURE;
  }

  mtos_sem_take(&p_priv->sem, 0);
  if(p_priv->status != DB_OK)
  {
    mtos_sem_give(&p_priv->sem);
    return (u16)ERR_FAILURE;
  }

  if(group_view_id == 0)
  {
    grp_start = 0;
  }
  else if (group_view_id > MAX_LIVE_GROUP)
  {
    group_view_id = MAX_LIVE_GROUP;
  }
  else
  {
    grp_start = (u16)(p_priv->p_db->grp_end_pos[group_view_id - 1] + 1);
  }

  if(group_view_id == INVALID_GRP)
  {
    valid = p_priv->p_db->valid_pg;// return all pg cnt
  }
  else
  {
    valid = (u16)(p_priv->p_db->grp_end_pos[group_view_id] - grp_start + 1);
  }
  mtos_sem_give(&p_priv->sem);
  return valid;
}

RET_CODE livetv_get_group_cnt(u8 *p_cnt)
{
  live_db_priv_t *p_priv = get_priv();

  if(p_priv->p_db == NULL)
  {
    return ERR_FAILURE;
  }
  
  mtos_sem_take(&p_priv->sem, 0);
  if(p_priv->status != DB_OK)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  *p_cnt = p_priv->p_db->valid_grp;
  mtos_sem_give(&p_priv->sem);
  return SUCCESS;
}

RET_CODE livetv_get_pg_by_group_pos(live_db_prg_t *p_pg, u16 pos, u16 group_view_id)
{
  live_db_priv_t *p_priv = get_priv();
  live_db_prg_t *p_in_pg = NULL;
  u16 grp_start = 0;
  u16 valid = 0;
//  u16 i = 0;

  if(group_view_id >= MAX_LIVE_GROUP && group_view_id != INVALID_GRP)
  {
    return ERR_FAILURE;
  }

  mtos_sem_take(&p_priv->sem, 0);
  if(p_priv->status != DB_OK)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  if(group_view_id == 0 || group_view_id == INVALID_GRP)
  {
    grp_start = 0;
  }
  else
  {
    grp_start = (u16)(p_priv->p_db->grp_end_pos[group_view_id - 1] + 1);
  }
  
  if(group_view_id == INVALID_GRP)
  {
    valid = p_priv->p_db->valid_pg;// return all pg cnt
  //  OS_PRINTF("###valid_pg == %d##\n", valid);
  }
  else
  {
    valid = (u16)(p_priv->p_db->grp_end_pos[group_view_id] - grp_start + 1);
  }

  if(pos > valid)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_PARAM;
  }

  if(group_view_id == INVALID_GRP)
  {
    p_in_pg = (live_db_prg_t *)(p_priv->p_db->sort_by_id[pos].p_addr);
  }
  else
  {  
    p_in_pg = (live_db_prg_t *)(p_priv->p_db->sort_by_grp[grp_start + pos].p_addr);
  }

  if(p_in_pg->active_flg == 0)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }
  else
  {
    memcpy(p_pg, p_in_pg, sizeof(live_db_prg_t));
  }
  mtos_sem_give(&p_priv->sem);
  return SUCCESS;
}

RET_CODE livetv_db_get_pos_by_id(u16 *p_pos, u16 id)
{
  u16 i = 0;
  live_db_priv_t *p_priv = get_priv();
  live_db_prg_t *p_in_pg = NULL;
  
  for(i = 0; i < p_priv->p_db->valid_pg; i ++)
  {
    p_in_pg = (live_db_prg_t *)(p_priv->p_db->sort_by_id[i].p_addr);
    if(p_in_pg != NULL && p_in_pg->id == id)
    {
      *p_pos = i;
      return SUCCESS;
    }
  }
  return ERR_FAILURE;  
}

RET_CODE livetv_db_save(u8 *p_data, u32 size)
{
  live_db_priv_t *p_priv = get_priv();
  u32 dest_len = 0;
//  int ret = 0;
  dm_ret_t dm_ret = DM_FAIL;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u32 offset = 0;//(p_priv->blk_idx ? 0 : 1) * CHARSTO_SECTOR_SIZE;
  u8 tmp = 0;
  //return SUCCESS;

  if(p_priv->use_bak)
  {
    offset = (p_priv->blk_idx ? 0 : 1) * CHARSTO_SECTOR_SIZE;
  }
  
  mtos_sem_take(&p_priv->sem, 0);

#if 0  
  dest_len = DEST_SIZE;
  //zip db and write into flash
  ret = gzip_compress(p_priv->p_dest, (u32 *)&dest_len, (const u8 *)p_priv->p_db,
                              (u32)sizeof(live_db_t), p_priv->p_cache, LIVETV_CACHE_SIZE);
  if(ret != 0)
  {
    OS_PRINTF("return 1\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }
  dm_direct_erase(p_dm_handle, p_priv->db_id, offset, CHARSTO_SECTOR_SIZE);

//  write data
  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset, dest_len, p_priv->p_dest);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 2\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }
// write size
  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset + LEN_ADDR, 4, (u8 *)&dest_len);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 3\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset + STRUCT_ADDR, 4, (u8 *)&p_priv->cur_struct_len);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 3\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }


  p_priv->check_valid ++;
  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset + LATEST_ADDR, 1, &p_priv->check_valid);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 4\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  tmp = OK_FLAG;
  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset + OK_ADDR, 1, &tmp);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 4\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  if(p_priv->use_bak)
  {
    p_priv->blk_idx = p_priv->blk_idx ? 0 : 1;
  }
 //create database in priv
  livetv_db_create_view();
#else
  dm_direct_erase(p_dm_handle, p_priv->db_id, offset, CHARSTO_SECTOR_SIZE);
//  write data
  dest_len = size;
  if(size > CHARSTO_SECTOR_SIZE)
  {
    OS_PRINTF("return 1\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }
  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset, dest_len, p_data);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 2\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }
  
  p_priv->check_valid ++;
  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset + LATEST_ADDR, 1, &p_priv->check_valid);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 4\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  tmp = OK_FLAG;
  dm_ret = dm_direct_write(p_dm_handle, p_priv->db_id, offset + OK_ADDR, 1, &tmp);
  if(DM_SUC != dm_ret)
  {
    OS_PRINTF("return 4\n");
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  if(p_priv->use_bak)
  {
    p_priv->blk_idx = p_priv->blk_idx ? 0 : 1;
  }
#endif
  mtos_sem_give(&p_priv->sem);
  OS_PRINTF("return 5\n");
 return SUCCESS;
}


void livetv_set_crc_ver(u32 crc)
{
  live_db_priv_t *p_priv = get_priv();
  mtos_sem_take(&p_priv->sem, 0);
  p_priv->p_db->crc_ver = crc;
  mtos_sem_give(&p_priv->sem);
}

u32 livetv_get_crc_ver(void)
{
  live_db_priv_t *p_priv = get_priv();
  u32 crc = 0;
  mtos_sem_take(&p_priv->sem, 0);
  crc = p_priv->p_db->crc_ver;
  mtos_sem_give(&p_priv->sem);
   return crc;
}

void livetv_clear_db(void)
{
  live_db_priv_t *p_priv = get_priv();
  u16 i = 0;
 // p_priv->p_db->crc_ver = new_crc;
  mtos_sem_take(&p_priv->sem, 0);
  for(i = 0; i < p_priv->p_db->valid_pg; i ++)
  {
    p_priv->p_db->prog[i].id = INVALID_ID;
    p_priv->p_db->prog[i].sate_id = 0;
    p_priv->p_db->prog[i].sate_apid = 0;
    p_priv->p_db->prog[i].sate_vpid = 0;
    p_priv->p_db->prog[i].sate_pcrpid = 0;
    p_priv->p_db->prog[i].sate_freq = 0;
    p_priv->p_db->prog[i].sate_sym = 0;
    p_priv->p_db->prog[i].sate_polarity = 0;
    p_priv->p_db->prog[i].sate_tv_flag = 0;
    p_priv->p_db->prog[i].active_flg = FALSE;
    p_priv->p_db->prog[i].grp_view_id = INVALID_GRP;
    p_priv->p_db->prog[i].url_cnt = 0;
    p_priv->p_db->prog[i].src_idx = 0;
    p_priv->p_db->prog[i].name[0]= 0;
  } 
  p_priv->p_db->valid_pg = 0;
  for(i = 0; i < p_priv->p_db->valid_grp; i ++)
  {
    p_priv->p_db->group[i].id = INVALID_GRP_ID;
    p_priv->p_db->group[i].view_id = INVALID_GRP;
    p_priv->p_db->group[i].name[0]= 0;
  } 

  p_priv->p_db->valid_grp = 0;
  memset(p_priv->p_db->grp_end_pos, 0, MAX_LIVE_GROUP * sizeof(u16));
  memset(p_priv->p_db->sort_by_id, 0, MAX_LIVE_PRG * sizeof(live_db_sort_info_t));
  memset(p_priv->p_db->sort_by_grp, 0, MAX_LIVE_PRG * sizeof(live_db_sort_info_t));

  for(i = 0; i < MAX_LIVE_PRG; i ++)
  {
    p_priv->p_db->sort_by_id[i].idx = INVALID_ID;
    p_priv->p_db->sort_by_grp[i].idx = INVALID_ID;
  }

  if(p_priv->is_str_dup == TRUE)
  {
    string_pool_destroy(p_priv->strpool_hdl);
    p_priv->init_pool = FALSE;
  }

  if(p_priv->init_pool == FALSE)
  {
    string_pool_init(&p_priv->strpool_hdl);
    p_priv->init_pool = TRUE;
  }
  mtos_sem_give(&p_priv->sem);
}

RET_CODE  livetv_add_pg_info(live_db_prg_t *p_pg)
{
  live_db_priv_t *p_priv = get_priv();
  u16 cur_pg = p_priv->p_db->valid_pg;
  u16 name_len = 0;
  u8 i = 0;
  live_db_t *p_db = p_priv->p_db;

  mtos_sem_take(&p_priv->sem, 0);
 // if(p_pg->p_name != NULL)
  {
    name_len = (u16)strlen((char *)p_pg->name);
  }

  /*if(p_pg->grp_id >= p_priv->p_db->max_grp)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_PARAM;
  }*/
  p_priv->p_db->prog[cur_pg].id = p_pg->id;
  p_priv->p_db->prog[cur_pg].sate_id = p_pg->sate_id;
  p_priv->p_db->prog[cur_pg].sate_apid = p_pg->sate_apid;
  p_priv->p_db->prog[cur_pg].sate_vpid = p_pg->sate_vpid;
  p_priv->p_db->prog[cur_pg].sate_pcrpid = p_pg->sate_pcrpid;
  p_priv->p_db->prog[cur_pg].sate_freq = p_pg->sate_freq;
  p_priv->p_db->prog[cur_pg].sate_sym = p_pg->sate_sym;
  p_priv->p_db->prog[cur_pg].sate_polarity = p_pg->sate_polarity;
  p_priv->p_db->prog[cur_pg].sate_tv_flag = p_pg->sate_tv_flag;
  p_priv->p_db->prog[cur_pg].active_flg = TRUE;
  p_priv->p_db->prog[cur_pg].grp_view_id = p_pg->grp_view_id;
  p_priv->p_db->prog[cur_pg].url_cnt = p_pg->url_cnt;
  p_priv->p_db->prog[cur_pg].src_idx = 0;
  if(name_len >= MAX_LIVE_NAME_LEN)
  {
    name_len = MAX_LIVE_NAME_LEN - 1;
  }
 // OS_PRINTF("live db save pg name len %d\n", name_len);
  if(name_len > 0)
  {
    memcpy(p_priv->p_db->prog[cur_pg].name, p_pg->name, name_len);
    p_priv->p_db->prog[cur_pg].name[name_len] = 0;
  }
  for(i = 0; i < p_pg->url_cnt; i ++)
  {
   if(p_pg->url[i] != NULL)
   {
     name_len = (u16)strlen((char*)p_pg->url[i]);
   }
   else
   {
     name_len = 0;
   }
  // OS_PRINTF("live db save pg url len %d\n", name_len);
   if(name_len > 0)
   {
     p_priv->p_db->prog[cur_pg].url[i] = str_dup(p_pg->url[i], p_priv->strpool_hdl);
     p_priv->is_str_dup = TRUE;
   }
   else
   {
     p_priv->p_db->prog[cur_pg].url[i] = NULL;
   }
  }
  p_db->prog[cur_pg].def_order = p_priv->p_db->valid_pg;
  p_priv->p_db->valid_pg ++;
  
  mtos_sem_give(&p_priv->sem);
  return SUCCESS;
}

RET_CODE  livetv_set_pg_cur_source(u16 pg_id, u8 src_idx)
{
  live_db_priv_t *p_priv = get_priv();
  u16 c_start = 0, c_end = MAX_LIVE_PRG;
  u16 cur = c_end / 2;
  u16 cnt = MAX_LIVE_PRG;
  u8 find = FALSE;
  live_db_prg_t *p_in_pg = NULL;

  mtos_sem_take(&p_priv->sem, 0);
  //find the pg 

  do
  {
    cur = (c_end  - c_start) / 2 + c_start;
    if(p_priv->p_db->sort_by_id[cur].idx < pg_id)
    {
      c_start = cur;
    }
    else
    {
      c_end = cur;
    }

    if(p_priv->p_db->sort_by_id[cur].idx == pg_id)
    {
      find = TRUE;
      break;
    }
  }while(cnt --);

  if(find == FALSE)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }
  p_in_pg = (live_db_prg_t *)p_priv->p_db->sort_by_id[cur].p_addr;
  
  if(p_in_pg->active_flg == FALSE)
  {
    mtos_sem_give(&p_priv->sem);
    return ERR_FAILURE;
  }

  p_in_pg->src_idx = src_idx;

  mtos_sem_give(&p_priv->sem);
  return SUCCESS;
}


RET_CODE  livetv_write_group_info(live_db_group_t *p_group)
{
  live_db_priv_t *p_priv = get_priv();
 // u16 cur_pg = p_priv->p_db->valid_pg;
  u16 name_len = 0;
  u8 i = 0;
//  live_db_t *p_db = p_priv->p_db;
  live_db_group_t *p_grp = NULL;
  u8 find = FALSE;
  
  mtos_sem_take(&p_priv->sem, 0);
 // if(p_pg->p_name != NULL)
  {
    name_len = (u16)strlen((char *)p_group->name);
  }
  
  for(i = 0; i < p_priv->p_db->max_grp; i ++)
  {
    p_grp = &p_priv->p_db->group[i];
    if(p_grp->id == p_group->id)
    {
       name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
       if(name_len > 0)
       {
         memcpy(p_grp->name, p_group->name, name_len);
         p_grp->name[name_len] = 0;
       }
       p_group->view_id = i;
       find = TRUE;
       break;
    }
  }

  if(!find)
  {
    for(i = 0; i < p_priv->p_db->max_grp; i ++)
    {
      p_grp = &p_priv->p_db->group[i];
      if(p_grp->id == INVALID_GRP)
      {
       //  p_grp->view_id = i;
         p_grp->id = p_group->id;
         name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
         if(name_len > 0)
         {
           memcpy(p_grp->name, p_group->name, name_len);
           p_grp->name[name_len] = 0;
         }
         p_group->view_id = i;
         //find = TRUE;
         p_priv->p_db->valid_grp ++;
    //     OS_PRINTF("##%s, valid_grp == %d##\n", __FUNCTION__, p_priv->p_db->valid_grp);
         break;
      }
    }
  }
  mtos_sem_give(&p_priv->sem);
  return SUCCESS;
}

void livetv_db_init(livetv_db_init_t *p_param)
{
  RET_CODE ret = SUCCESS;
  live_db_priv_t *p_priv = get_priv();
  u16 i = 0;

  string_pool_init(&p_priv->strpool_hdl);
  p_priv->init_pool = TRUE;
  if(p_param->buf_size < sizeof(live_db_t))
  {
    OS_PRINTF("buffer size too small %d < %d\n", p_param->buf_size, sizeof(live_db_t));
    MT_ASSERT(0);
  }
  ret = lib_memp_create(&p_priv->memp, (u32)p_param->p_block_buf, p_param->buf_size);
  MT_ASSERT(ret == SUCCESS);

  p_priv->db_id = p_param->db_block_id;
  p_priv->p_db = lib_memp_align_alloc(&p_priv->memp, sizeof(live_db_t), 8);
  MT_ASSERT(p_priv->p_db != NULL);

  mtos_sem_create(&p_priv->sem, 1);
  p_priv->p_db->max_grp = p_param->max_group;
  p_priv->p_db->max_prg = p_param->max_prg;
  p_priv->use_bak = p_param->use_bak;
  p_priv->p_db->valid_pg = 0;
  
  for(i = 0; i < p_param->max_prg; i ++)
  {
    p_priv->p_db->prog[i].id = INVALID_ID;
    p_priv->p_db->prog[i].sate_id = 0;
    p_priv->p_db->prog[i].sate_apid = 0;
    p_priv->p_db->prog[i].sate_vpid = 0;
    p_priv->p_db->prog[i].sate_pcrpid = 0;
    p_priv->p_db->prog[i].sate_freq = 0;
    p_priv->p_db->prog[i].sate_sym = 0;
    p_priv->p_db->prog[i].sate_polarity = 0;
    p_priv->p_db->prog[i].sate_tv_flag = 0;
    p_priv->p_db->prog[i].active_flg = FALSE;
    p_priv->p_db->prog[i].grp_view_id = INVALID_GRP;
    p_priv->p_db->prog[i].url_cnt = 0;
    p_priv->p_db->prog[i].src_idx = 0;
    p_priv->p_db->prog[i].name[0]= 0;
  }

  for(i = 0; i < p_param->max_group; i ++)
  {
    p_priv->p_db->group[i].id = INVALID_GRP;
    p_priv->p_db->group[i].name[0]= 0;
    p_priv->p_db->sort_by_id[i].idx = INVALID_ID;
    p_priv->p_db->sort_by_grp[i].idx = INVALID_ID;
  }

  for(i = 0; i < MAX_LIVE_PRG; i ++)
  {
    p_priv->p_db->sort_by_id[i].idx = INVALID_ID;
    p_priv->p_db->sort_by_grp[i].idx = INVALID_ID;
  }

  p_priv->cur_struct_len =  (u32)sizeof(live_db_t);

//  if(SUCCESS == db_load())
//  {
//    livetv_db_create_view();
//  }

 // livetv_test_write();
 //lint -e438 -e550
}
//lint +e438 +e550

u32 livetv_db_size_get(void)
{
  u32 size = sizeof(live_db_t) + 1*KBYTES;
  return size;
}


#if 0
void livetv_test_write(void)
{
  u16 i = 0; 
  live_db_prg_t pg = {0};

  livetv_clear_db();
  for(i = 0; i < MAX_LIVE_PRG; i ++)
  {
    pg.id = MAX_LIVE_PRG - i;
    pg.grp_id = rand() % 5;
    livetv_write_pg_info(&pg);
  }
  livetv_db_save();

 // db_load();
 // livetv_db_create_view();
  livetv_get_pg_by_id(&pg, 278);
  livetv_get_pg_by_group_pos(&pg, 2, 4);
}
#endif

