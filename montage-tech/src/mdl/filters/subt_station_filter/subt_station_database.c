/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"

#include "class_factory.h"
#include "subt_station_database.h"

#define MAX_SST_OUT_ITEM_NUM  (8192)


#define MAX_SST_IN_ITEM_NUM  (100)

#define ONE_DATA_BLOCK_SIZE (256)

typedef struct
{
  subt_station_key_point_t key;
  u32 used;
}sst_item_t;

typedef struct
{
  union
  {
  //out
  sst_item_t out_item[MAX_SST_OUT_ITEM_NUM];

  //int
  subt_station_in_item_t in_item[MAX_SST_IN_ITEM_NUM];
  };
  BOOL internal_item;
  u32 item_cnt;
  u8 *p_data;
}sst_db_t;

static sst_db_t *p_sst_db = NULL;

void sst_db_save_out_item(subt_station_key_point_t *p_key)
{
  u16 i = 0;

  if(p_sst_db->internal_item)
    return;
    //need improve the perf in feature
  for(i = 0; i < MAX_SST_OUT_ITEM_NUM; i ++)
  {
    if(!p_sst_db->out_item[i].used)
    {
      p_sst_db->out_item[i].key.start = p_key->start;
      p_sst_db->out_item[i].key.end = p_key->end;
      p_sst_db->out_item[i].key.pos = p_key->pos;
      p_sst_db->out_item[i].used = TRUE;
      p_sst_db->item_cnt ++;
      return;
    }
  }
}


RET_CODE sst_db_find_out_item(u32 pts, subt_station_key_point_t *p_key)
{
  u16 i = 0;
  
  if(p_sst_db->internal_item)
    return ERR_FAILURE;

  //need improve the perf in feature
  for(i = 0; i < p_sst_db->item_cnt; i ++)
  {
    if(pts >= p_sst_db->out_item[i].key.start
      && pts <= p_sst_db->out_item[i].key.end)
    {
      p_key->start = p_sst_db->out_item[i].key.start;
      p_key->end = p_sst_db->out_item[i].key.end;
      p_key->pos = p_sst_db->out_item[i].key.pos;
      return SUCCESS;
    }
    else if(i < p_sst_db->item_cnt
      && pts >= p_sst_db->out_item[i].key.end
      && pts <= p_sst_db->out_item[i + 1].key.start)
    {
      return ERR_TIMEOUT;
    }
  }
  
  return ERR_FAILURE;
}

void sst_db_save_in_item(subt_station_in_item_t *p_item)
{
  u16 i = 0;

  if(!p_sst_db->internal_item)
    return;

    //need improve the perf in feature
  for(i = 0; i < MAX_SST_IN_ITEM_NUM; i ++)
  {
    if(!p_sst_db->in_item[i].used)
    {
      p_sst_db->in_item[i].start = p_item->start;
      p_sst_db->in_item[i].end = p_item->end;
      p_sst_db->in_item[i].p_data = (p_sst_db->p_data + ONE_DATA_BLOCK_SIZE * i);
      /*Use the len of string instead of 512 const value*/
      p_sst_db->in_item[i].data_len = MIN(p_item->data_len, ONE_DATA_BLOCK_SIZE);
      
      memcpy(p_sst_db->in_item[i].p_data, 
             p_item->p_data, 
             p_sst_db->in_item[i].data_len);
        
      
      p_sst_db->in_item[i].used = TRUE;
      p_sst_db->item_cnt ++;
      return;
    }
  }

}


RET_CODE sst_db_find_in_item(u32 pts, subt_station_in_item_t *p_item)
{
  u16 i = 0;
  
  if(!p_sst_db->internal_item)
    return ERR_FAILURE;

  //need improve the perf in feature
  for(i = 0; i < MAX_SST_IN_ITEM_NUM; i ++)
  {
    /*If the buffer is not used, so the data is dirty*/
    if(FALSE == p_sst_db->in_item[i].used) 
    {
        continue;
    }
    
    if(pts >= p_sst_db->in_item[i].start
      && pts <= p_sst_db->in_item[i].end)
    {
      p_item->start = p_sst_db->in_item[i].start;
      p_item->end = p_sst_db->in_item[i].end;
      p_item->p_data = p_sst_db->in_item[i].p_data;
      p_item->data_len = p_sst_db->in_item[i].data_len;

      p_sst_db->in_item[i].used = FALSE;
      p_sst_db->in_item[i].start = 0xffffffff;
      
      return SUCCESS;
    }
    else if(pts >= p_sst_db->in_item[i].end)
    {
      //timeout item
      p_sst_db->in_item[i].used = FALSE;
      p_sst_db->in_item[i].start = 0xffffffff;
    }
  }
  
  return ERR_FAILURE;
}

void sst_db_reset(void)
{
  u16 i = 0;
  if(p_sst_db != NULL)
  {
    if(p_sst_db->internal_item)
    {
      for(i = 0; i < MAX_SST_IN_ITEM_NUM; i ++)
      {
        p_sst_db->in_item[i].used = FALSE;
        p_sst_db->in_item[i].start = 0xffffffff;
      }
    }
    else
    {
      for(i = 0; i < MAX_SST_OUT_ITEM_NUM; i ++)
      {
        p_sst_db->out_item[i].used = FALSE;
        p_sst_db->out_item[i].key.start = 0xffffffff;
      }
    }
    p_sst_db->item_cnt = 0;
  }
}

void sst_db_set_item_src(BOOL internal_subt)
{
  p_sst_db->internal_item = internal_subt;
  if(internal_subt)
  {
    if(p_sst_db->p_data != NULL)
    {
      mtos_free(p_sst_db->p_data);
    }
    p_sst_db->p_data = mtos_malloc(ONE_DATA_BLOCK_SIZE * MAX_SST_IN_ITEM_NUM);
    MT_ASSERT(p_sst_db->p_data != NULL);
  }  
}

void sst_db_init(void)
{
  p_sst_db = mtos_malloc(sizeof(sst_db_t));
  if(p_sst_db == NULL)
  {
    OS_PRINTF("\n## sst_db_init malloc[%lu] fail!!\n", sizeof(sst_db_t));
    MT_ASSERT(0);
    return ;
  }
  memset(p_sst_db, 0, sizeof(sst_db_t));
}

void sst_db_deinit(void)
{
  
  if(p_sst_db && p_sst_db->p_data != NULL)
  {
    mtos_free(p_sst_db->p_data);
    p_sst_db->p_data = NULL;
  }
    
  if(p_sst_db != NULL)
  {
    mtos_free(p_sst_db);
    OS_PRINTF("sst db deinit\n");
  }
  p_sst_db = NULL;
}

