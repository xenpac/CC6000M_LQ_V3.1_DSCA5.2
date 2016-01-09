/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_printk.h"

#include "class_factory.h"
#include "magic_key.h"

#define MAGIC_KEY_INVALID 0

typedef struct 
{
  u16 magic_key;
  magic_key_list_t *p_key_list;
  u8 key_cnt;
  u8 key_state;
}magic_key_info_t;

typedef struct 
{
  u32 curn_ticks;
  u32 last_ticks;
  magic_key_info_t *p_key_info;
}magic_key_node_t;

static BOOL verify_key(magic_key_info_t *p_key_info,u16 key, magic_key_type_t key_type)
{
  if (key == p_key_info->p_key_list[p_key_info->key_state].v_key
    && ((key_type == p_key_info->p_key_list[p_key_info->key_state].key_type) 
    || (p_key_info->p_key_list[p_key_info->key_state].key_type == ALL_TYPE_MGC_KEY)))
  {
    p_key_info->key_state++;
  }
  else
  {
    if (key == p_key_info->p_key_list[0].v_key)
    {
      p_key_info->key_state = 1;
    }
    else
    {
      p_key_info->key_state = 0;
    }
  }

  if (p_key_info->key_state == p_key_info->key_cnt)
  {
    p_key_info->key_state = 0;
    return TRUE;
  }

  return FALSE;
}

static void reset_magic_keylist(magic_key_node_t *p_key_node)
{
  u8 i = 0;
  magic_key_info_t *p_key_info = p_key_node->p_key_info;

  for (i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if (p_key_info->magic_key != MAGIC_KEY_INVALID) 
    {
      p_key_info->key_state = 0;
    }

    p_key_info++;
  }
}

u16 detect_magic_keylist(void *p_handle,u16 key, magic_key_type_t key_type)
{
  u8 i = 0;
  magic_key_node_t *p_key_node = NULL;
  magic_key_info_t *p_key_info = NULL;
  
  MT_ASSERT(p_handle != NULL);
  
  p_key_node = (magic_key_node_t *)p_handle;
  p_key_info = p_key_node->p_key_info;
  
  p_key_node->curn_ticks = mtos_ticks_get();

  if ((p_key_node->curn_ticks - p_key_node->last_ticks) > 2000)
  {
    reset_magic_keylist(p_key_node);
  }

  p_key_node->last_ticks = p_key_node->curn_ticks;

  for (i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if (p_key_info->p_key_list != NULL) 
    {
      if(verify_key(p_key_info, key, key_type))
      {
        return p_key_info->magic_key;
      }
    }

    p_key_info++;
  }

  return MAGIC_KEY_INVALID;
}

BOOL register_magic_keylist(void *p_handle,magic_key_list_t *p_key_list, u8 key_cnt, u16 magic_key)
{
  u8 i = 0;
  magic_key_node_t *p_key_node = NULL;
  magic_key_info_t *p_key_info = NULL;
  
  MT_ASSERT(p_handle != NULL && key_cnt > 0
     && p_key_list !=NULL && magic_key !=MAGIC_KEY_INVALID);

  p_key_node = (magic_key_node_t *)p_handle;
  p_key_info = p_key_node->p_key_info;

  for (i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if (p_key_info->magic_key == MAGIC_KEY_INVALID)
    {
      p_key_info->magic_key = magic_key;
      p_key_info->p_key_list = (magic_key_list_t *)mtos_malloc(key_cnt * sizeof(magic_key_list_t));
      MT_ASSERT(p_key_info->p_key_list !=NULL);
      memcpy(p_key_info->p_key_list, p_key_list, key_cnt * sizeof(magic_key_list_t));
      p_key_info->key_cnt = key_cnt;
      p_key_info->key_state = 0;

      return TRUE;
    }

    p_key_info++;
  }

  return FALSE;
}

BOOL init_magic_keylist(void)
{
  u8 i = 0;
  magic_key_info_t *p_key_info = NULL;
  magic_key_node_t *p_key_node = NULL;
  class_handle_t p_handle = class_get_handle_by_id(MAGIC_KEY_CLASS_ID);

  if(p_handle == NULL)
  {
    p_key_node = (magic_key_node_t *)mtos_malloc(sizeof(magic_key_node_t));
    MT_ASSERT(p_key_node !=NULL);
    p_key_info = (magic_key_info_t *)mtos_malloc(MAX_MAGIC_LIST * sizeof(magic_key_info_t));
    MT_ASSERT(p_key_info !=NULL);
    memset(p_key_info,0,MAX_MAGIC_LIST * sizeof(magic_key_info_t));
    memset(p_key_node,0,sizeof(magic_key_node_t));
    p_key_node->p_key_info = p_key_info;
    
    for (i = 0; i < MAX_MAGIC_LIST; i++)
    {
      p_key_info->p_key_list = NULL;
      p_key_info->key_cnt = 0;
      p_key_info->key_state = 0;
      p_key_info->magic_key = MAGIC_KEY_INVALID;

      p_key_info++;
    }
    class_register(MAGIC_KEY_CLASS_ID, (class_handle_t)p_key_node);
  }
  return TRUE;
}

