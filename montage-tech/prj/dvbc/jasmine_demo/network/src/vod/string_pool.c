/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_mem.h"
#include "mtos_printk.h"

#include "string_pool.h"

#define MAX_STRING_POOL (10)

#define STR_POOL_MEM_SLICE_SIZE (6*1024)

typedef struct _STRPOOL_MEM_NODE
{
  struct _STRPOOL_MEM_NODE* p_next;
  u8* p_mem_slice;
  u32 offset;
}STRPOOL_MEM_NODE, *STRPOOL_MEM_LIST;

typedef struct
{
  BOOL is_free;
  STRPOOL_MEM_LIST p_mem_list;
}string_pool_t;

typedef struct
{
  string_pool_t str_pools[MAX_STRING_POOL];
  u32 str_pool_cnt;
}string_pool_priv_t;

static string_pool_priv_t* sg_p_string_pool_priv = NULL;

RET_CODE string_pool_init(u32* p_hdl)
{
  int i;
  if( NULL == sg_p_string_pool_priv )
  {
    sg_p_string_pool_priv = (string_pool_priv_t*)mtos_malloc(sizeof(string_pool_priv_t));
    MT_ASSERT(NULL != sg_p_string_pool_priv);
    for( i = 0; i < MAX_STRING_POOL; i++)
    {
      sg_p_string_pool_priv->str_pool_cnt = 0;
      sg_p_string_pool_priv->str_pools[i].is_free = TRUE;
      sg_p_string_pool_priv->str_pools[i].p_mem_list = NULL;
    }
  }

  if( sg_p_string_pool_priv->str_pool_cnt == MAX_STRING_POOL )
    return ERR_FAILURE;
  
  for(i = 0; i < MAX_STRING_POOL; i++ )
  {
    if( TRUE == sg_p_string_pool_priv->str_pools[i].is_free )
    {
      sg_p_string_pool_priv->str_pools[i].is_free = FALSE;
      *p_hdl = i;
      sg_p_string_pool_priv->str_pool_cnt++;
      return SUCCESS;
    }
  }

  return ERR_FAILURE;
}

RET_CODE string_pool_destroy(u32 hdl)
{
  STRPOOL_MEM_LIST p_list = NULL;
  STRPOOL_MEM_LIST p = NULL;

  if( hdl >= MAX_STRING_POOL )
    return ERR_FAILURE;

  p_list = sg_p_string_pool_priv->str_pools[hdl].p_mem_list;
  while( p_list )
  {
    p = p_list;
    p_list = p_list->p_next;
    mtos_free(p->p_mem_slice);
    mtos_free(p);
  }
  sg_p_string_pool_priv->str_pools[hdl].p_mem_list = NULL;
  sg_p_string_pool_priv->str_pools[hdl].is_free = TRUE;
  sg_p_string_pool_priv->str_pool_cnt--;

  return SUCCESS;
}

u8* str_dup(u8* p_url, u32 hdl)
{
  STRPOOL_MEM_LIST *pp_list = NULL;
  STRPOOL_MEM_NODE* p_node = NULL;
  int len = 0;
  u8* p_ret = NULL;
  
  if( NULL == p_url )
    return NULL;

  if( hdl >= MAX_STRING_POOL )
    return NULL;

  len = strlen(p_url);
  if(len >= STR_POOL_MEM_SLICE_SIZE)
  {
    len = STR_POOL_MEM_SLICE_SIZE - 1;
    p_url[len] = '\0';
  }
  
  pp_list = &sg_p_string_pool_priv->str_pools[hdl].p_mem_list;
  if( NULL == *pp_list )
  {
    *pp_list = (STRPOOL_MEM_NODE*)mtos_malloc(sizeof(STRPOOL_MEM_NODE));
    MT_ASSERT(NULL != *pp_list);
    (*pp_list)->p_mem_slice = (u8*)mtos_malloc(STR_POOL_MEM_SLICE_SIZE);
    MT_ASSERT(NULL != (*pp_list)->p_mem_slice);
    (*pp_list)->p_next = NULL;
    (*pp_list)->offset = 0;
  }

  if( (*pp_list)->offset + len + 1 > STR_POOL_MEM_SLICE_SIZE )
  {
    p_node = (STRPOOL_MEM_NODE*)mtos_malloc(sizeof(STRPOOL_MEM_NODE));
    MT_ASSERT(NULL != p_node);
    p_node->p_mem_slice = (u8*)mtos_malloc(STR_POOL_MEM_SLICE_SIZE);
    MT_ASSERT(NULL != p_node->p_mem_slice);
    p_node->offset = 0;
    p_node->p_next = (*pp_list);
    *pp_list = p_node;

    strcpy((*pp_list)->p_mem_slice, p_url);
    p_ret = (*pp_list)->p_mem_slice;
    p_node->offset += (len+1);
  }
  else
  {
    strcpy((*pp_list)->p_mem_slice + (*pp_list)->offset, p_url);
    p_ret = (*pp_list)->p_mem_slice + (*pp_list)->offset;
    (*pp_list)->offset += (len+1);
  }
  
  return p_ret;;
}


