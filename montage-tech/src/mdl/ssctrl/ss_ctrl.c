/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_define.h"
#include "sys_types.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_mutex.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"

#ifdef PRINT_ON
//#define SC_DEBUG
#endif

#ifdef PRINT_ON
#define SSC_CHK_NI
#endif

#ifdef SS_DEBUG
#define SSC_DEBUG OS_PRINTF
#else
#define SSC_DEBUG DUMMY_PRINTF
#endif

#ifdef SSC_CHK_NI
#define SSC_CHK_NI_DBG OS_PRINTF
#else
#define SSC_CHK_NI_DBG DUMMY_PRINTF
#endif

/*!
  checksum
  */
#define SS_CHECKSUM_VALUE (0x47A547A5)
#define SC_DEF_ATOM_SIZE (40)

typedef struct tag_sc_priv_data
{
  ss_public_t public_data;  //public data
  u16 public_start; //public data node id from
  u16 public_num;//public data node id number
  
  u8 *p_copy_buf;  //user data
  u16 node_start; //user data node id from
  u16 node_num; //user data node id number

  u32 checksum; //checksum must be SS_CHECKSUM_VALUE
  u16 checksum_ni;  //the checksum node id
  u8 block_id;  //ss block id
  u8 atom_size;
  void *p_sc_mutex;

  BOOL miss_data;
}sc_priv_data_t;


void ss_ctrl_public_init(void *p_data, u8 block_id)
{
  u8 atom_size = SC_DEF_ATOM_SIZE;
  u16 i = 0;
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;
  u16 last_node_id = 0;
  u16 last_node_len = 0;
  
  MT_ASSERT(p_sc_handle != NULL);
  
  p_sc_handle->atom_size = atom_size;
  p_sc_handle->block_id = block_id;

  //init public data
  p_sc_handle->public_start = 
        db_get_max_view_node_num(class_get_handle_by_id(DB_CLASS_ID));
  p_sc_handle->public_num = (sizeof(ss_public_t) + atom_size - 1) / atom_size;

  last_node_id = p_sc_handle->public_num - 1;
  last_node_len = sizeof(ss_public_t) - (last_node_id * atom_size);

  SSC_DEBUG("public data debug : last_node_id %d, last_node_len %d\n",
      last_node_id, last_node_len);
  
  for(i = 0; i < p_sc_handle->public_num; i ++)
  {
    p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_handle != NULL);
    if(i == last_node_id)
    {
      dm_read(p_dm_handle, block_id, p_sc_handle->public_start + i, 0, 
      last_node_len, (u8 *)(&p_sc_handle->public_data) + i * atom_size);
      break;
    }
    dm_read(p_dm_handle, block_id, p_sc_handle->public_start + i, 0, 
      atom_size, (u8 *)(&p_sc_handle->public_data) + i * atom_size);
  }

  p_sc_handle->p_sc_mutex = mtos_mutex_create(1);
  MT_ASSERT(p_sc_handle->p_sc_mutex != NULL);
}

BOOL ss_ctrl_init(u8 block_id, u8 *p_buffer, u32 buf_len)
{
  u16 i = 0;
  u32 length = 0;
  u8 atom_size = SC_DEF_ATOM_SIZE;
  //static u8 is_initialized = FALSE;
  sc_priv_data_t *p_sc_handle = NULL;
  void *p_dm_handle = NULL;
  u16 view_blk_node_num = 0;
  u16 read_len = 0;
  BOOL b_complete_data = TRUE;
  
  MT_ASSERT(p_buffer!= NULL);

  p_sc_handle = mtos_malloc(sizeof(sc_priv_data_t));
  MT_ASSERT(p_sc_handle != NULL);

  memset(p_sc_handle, 0, sizeof(sc_priv_data_t));
  SSC_DEBUG("%d", sizeof(sc_priv_data_t));
  //if(is_initialized == TRUE)
  //{
  //  return ss_ctrl_get_checksum();
  //}
  
  //ss ctrl public init
  ss_ctrl_public_init(p_sc_handle, block_id);
  
  atom_size = p_sc_handle->atom_size;
  length = ((buf_len + atom_size - 1) / atom_size * atom_size);

  //init user data
  p_sc_handle->p_copy_buf = (u8 *)mtos_malloc(length);
  MT_ASSERT(p_sc_handle->p_copy_buf != NULL);

  memset(p_sc_handle->p_copy_buf, 0, length);
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  
  p_sc_handle->node_start = 
                p_sc_handle->public_start + p_sc_handle->public_num;
  p_sc_handle->node_num = length / atom_size;
  
  for(i = 0; i < p_sc_handle->node_num; i ++)
  {
    read_len = dm_read(p_dm_handle, block_id, p_sc_handle->node_start + i, 0, atom_size,
     p_sc_handle->p_copy_buf + i * atom_size);

    if(read_len == 0)
    {
      b_complete_data = FALSE;
      break;
    }
  }

  memcpy(p_buffer, p_sc_handle->p_copy_buf, buf_len);

  //init checksum
  p_sc_handle->checksum_ni = p_sc_handle->node_start + p_sc_handle->node_num;

  view_blk_node_num = dm_get_blk_node_num(p_dm_handle, IW_VIEW_BLOCK_ID);

  if(p_sc_handle->checksum_ni >= view_blk_node_num)
  {
    //Notes: if checksum ni >= view_blk_node_num, the checksum data can't save
    //       to flash. because data_manager thinks this node id is invalid.
    //       you must reconfig iwview node bigger than checksum_ni in the flash.cfg
    SSC_CHK_NI_DBG("%s checksum_ni %d, view_blk_node_num %d\n", 
                  __FUNCTION__, p_sc_handle->checksum_ni, view_blk_node_num);
    MT_ASSERT(0);
  }
  else
  {
    p_sc_handle->checksum_ni = view_blk_node_num - 1;
  }

  dm_read(p_dm_handle, block_id, p_sc_handle->checksum_ni, 0, sizeof(u32), 
  (u8 *)(&p_sc_handle->checksum));
  
  class_register(SC_CLASS_ID, p_sc_handle);

  if(b_complete_data)
  {
    b_complete_data = ss_ctrl_get_checksum(p_sc_handle);
  }
  p_sc_handle->miss_data = !b_complete_data;
  return b_complete_data;
}

BOOL ss_ctrl_sync(void *p_data, u8 *p_buffer)
{
  u16 i = 0;
  u16 node_id = 0;
  u32 offset = 0; 
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;
  
  MT_ASSERT(p_sc_handle != NULL);
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  
  mtos_mutex_take(p_sc_handle->p_sc_mutex);
  for(i = 0; i < p_sc_handle->node_num; i++)
  {
    offset = i * p_sc_handle->atom_size;
    if((0 != memcmp(p_sc_handle->p_copy_buf + offset, 
                       p_buffer + offset, p_sc_handle->atom_size)) ||
        p_sc_handle->miss_data)
    {
      memcpy(p_sc_handle->p_copy_buf + offset, p_buffer + offset, 
                                                      p_sc_handle->atom_size);
      node_id = p_sc_handle->node_start + i;
      
      dm_write_node(p_dm_handle, p_sc_handle->block_id, &node_id, 
                      (u8 *)(p_buffer + offset), p_sc_handle->atom_size);
    }
  }
  p_sc_handle->miss_data = FALSE;
  mtos_mutex_give(p_sc_handle->p_sc_mutex);  
  return TRUE;
}


/*!
   Get sysstatus checksum flag. if flag is error, it means some data is iffy 
   possibly
   \return Return TRUE if the checksum is right. or not FALSE.
   */
BOOL ss_ctrl_get_checksum(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  MT_ASSERT(p_sc_handle != NULL);
  
  return (p_sc_handle->checksum == SS_CHECKSUM_VALUE);
}

/*!
   Set right checksum to flash
 \return Return TRUE if succeed. or not FALSE.
 */
BOOL ss_ctrl_set_checksum(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;

  MT_ASSERT(p_sc_handle != NULL);
  p_sc_handle->checksum = SS_CHECKSUM_VALUE;
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  
  dm_write_node(p_dm_handle, p_sc_handle->block_id, 
    &p_sc_handle->checksum_ni, (u8 *)(&p_sc_handle->checksum), 4);
  return TRUE;
}

/*!
   clean the checksum from flash
 \return Return TRUE if succeed. or not FALSE.
 */
BOOL ss_ctrl_clr_checksum(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  void *p_dm_handle = NULL;
  
  MT_ASSERT(p_sc_handle != NULL);

  p_sc_handle->checksum = ~0;
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  
  dm_write_node(p_dm_handle, p_sc_handle->block_id, 
    &p_sc_handle->checksum_ni, (u8 *)(&p_sc_handle->checksum), 4);
  return TRUE;
}

/*!
   get the public struct
 \return the addr of public data
 */
ss_public_t *ss_ctrl_get_public(void *p_data)
{
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  MT_ASSERT(p_sc_handle != NULL);
  
  return &p_sc_handle->public_data;
}

/*!
   save the public data to flash
 \return Return TRUE if succeed. or not FALSE.
 */
BOOL ss_ctrl_update_public(void *p_data)
{
  u16 i = 0;
  u16 node_id = 0;
  u16 last_node_id = 0;
  u16 last_node_len = 0;
  void *p_dm_handle = NULL;
  sc_priv_data_t *p_sc_handle = (sc_priv_data_t *)p_data;
  MT_ASSERT(p_sc_handle != NULL);

  last_node_id = p_sc_handle->public_num - 1;
  last_node_len = sizeof(ss_public_t) - (last_node_id * p_sc_handle->atom_size);
  
  mtos_mutex_take(p_sc_handle->p_sc_mutex);  
  for(i = 0; i < p_sc_handle->public_num; i ++)
  {
    node_id = p_sc_handle->public_start + i;
    p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_handle != NULL);

    if(i == last_node_id)
    {
      dm_write_node(p_dm_handle, p_sc_handle->block_id, &node_id, 
        (u8 *)(&p_sc_handle->public_data) + i * p_sc_handle->atom_size,
        last_node_len);
      break;
    }
    dm_write_node(p_dm_handle, p_sc_handle->block_id, &node_id, 
      (u8 *)(&p_sc_handle->public_data) + i * p_sc_handle->atom_size,
        p_sc_handle->atom_size);
  }
  
  mtos_mutex_give(p_sc_handle->p_sc_mutex);  
  return TRUE;
}


#define MDI_HEAD_NODE_ID (0)
#define MDI_HEAD_MAX_NUM (256)
#define MDI_ATOM_SIZE (58)

typedef struct tag_mdi_head
{
  u32 user_id;
  u16 node_start_id;
  u16 node_num;
}mdi_head_t;


typedef struct tag_mdi_globo_info
{
  u32 using_block_id;
  u32 last_node_id;
  u32 using_head_cnt; //must follow using_block_id
  mdi_head_t head_arry[MDI_HEAD_MAX_NUM];
}mdi_globo_info_t;

typedef struct tag_mdi_priv
{
  u32 user_id;
  u16 rev1;
  u8 block_id;
  u8 rev;
  u32 space_size;
  u16 node_start_id;
  u16 node_num;
  u32 atom_size;
  void *p_dm_handle;
  u8 *p_buf;
  os_sem_t sem;
}mdi_priv_t;


static mdi_priv_t * mdi_get_priv(handle_t _this)
{
  mdi_priv_t *p_priv = NULL;

  if(_this != NULL)
  {
    p_priv = (mdi_priv_t *)((u32)_this + sizeof(mdi_class_t));
  }

  return p_priv;
}

static BOOL mdi_add_instance(mdi_globo_info_t *p_globo, mdi_priv_t *p_priv)
{
  
  mdi_head_t *p_last_head = p_globo->head_arry + p_globo->using_head_cnt;
  u16 h_node_id = MDI_HEAD_NODE_ID;

  if(p_priv->space_size == 0)
  {
    p_priv->node_num = 1;
    p_priv->p_buf = NULL;
  }
  else
  {
    p_priv->node_num = (p_priv->space_size + p_priv->atom_size - 1) / p_priv->atom_size;
    p_priv->p_buf = (u8 *)mtos_malloc(p_priv->space_size);
    if(p_priv->p_buf == NULL)
    {
      SSC_DEBUG("%s fail: no mem.\n", __FUNCTION__);
      return FALSE;
    }

    memset(p_priv->p_buf, 0xFF, p_priv->space_size);
  }
  
  p_priv->node_start_id = p_globo->last_node_id;
  p_globo->last_node_id = p_priv->node_start_id + p_priv->node_num;

  
  p_last_head->user_id = p_priv->user_id;
  p_last_head->node_start_id = p_priv->node_start_id;
  p_last_head->node_num = p_priv->node_num;
  p_globo->using_head_cnt++;

  //save it
  dm_write_node(p_priv->p_dm_handle, p_globo->using_block_id, &h_node_id, (u8 *)p_globo,
    sizeof(mdi_globo_info_t) - sizeof(mdi_head_t) * (MDI_HEAD_MAX_NUM - p_globo->using_head_cnt));

  return TRUE;
}

static BOOL mdi_load_instance(mdi_priv_t *p_priv)
{
  u16 i = 0;
  u32 read_len = 0;
  u32 total_read_len = 0;
  u32 atom_size = 0;

  if(p_priv->space_size == 0) //no need
  {
    p_priv->p_buf = NULL;
    return TRUE;
  }

  atom_size = p_priv->atom_size;
  p_priv->p_buf = (u8 *)mtos_malloc(p_priv->space_size);
  if(p_priv->p_buf == NULL)
  {
    SSC_DEBUG("%s fail: no mem.\n", __FUNCTION__);
    return FALSE;
  }

  memset(p_priv->p_buf, 0xFF, p_priv->space_size);
  
  
  for(i = 0; i < p_priv->node_num; i ++)
  {
    if((p_priv->space_size - total_read_len) < atom_size) //the last
    {
      read_len = p_priv->space_size - total_read_len;
    }
    else
    {
      read_len = atom_size;
    }
    read_len = dm_read(p_priv->p_dm_handle, p_priv->block_id,
      p_priv->node_start_id + i, 0, read_len, p_priv->p_buf + total_read_len);

    if(read_len == 0) //finish
    {
      return FALSE;
    }
    else
    {
      total_read_len += read_len;
    }
  }

  return TRUE;
}

static u32 mdi_read(handle_t _this, u32 r_len, u8 *p_buffer)
{
  mdi_class_t *p_this = (mdi_class_t *)_this;
  mdi_priv_t *p_priv = mdi_get_priv(_this);
  u16 i = 0;
  u32 read_len = 0;
  u32 total_read_len = 0;
  u32 atom_size = 0;

  if(NULL == p_this)
  {
    SSC_DEBUG("%s fail: non handle.\n", __FUNCTION__);
    return 0;
  }

  if(p_priv->user_id == 0)
  {
    SSC_DEBUG("%s fail: non handle.\n", __FUNCTION__);
    return 0;
  }

  if((p_priv->node_start_id == 0) || (p_priv->node_num == 0))
  {
    SSC_DEBUG("%s fail: non handle.\n", __FUNCTION__);
    return 0;
  }

  mtos_sem_take(&p_priv->sem, 0);

  if(p_priv->space_size == 0) //it's nonfix length, only one node
  {
    total_read_len = dm_read(p_priv->p_dm_handle, p_priv->block_id, p_priv->node_start_id,
      0, r_len, p_buffer);
  }
  else
  {
    atom_size = p_priv->atom_size;
    
    for(i = 0; i < p_priv->node_num; i++)
    {
      if((p_priv->space_size - total_read_len) < atom_size) //the last
      {
        read_len = p_priv->space_size - total_read_len;
      }
      else
      {
        read_len = atom_size;
      }
      read_len = dm_read(p_priv->p_dm_handle, p_priv->block_id,
        p_priv->node_start_id + i, 0, read_len, p_buffer + total_read_len);

      if(read_len == 0) //finish
      {
        break;
      }
      else
      {
        total_read_len += read_len;
      }
    }
  }
  
  mtos_sem_give(&p_priv->sem);
  return total_read_len;
}

static u32 mdi_write(handle_t _this, u32 w_len, u8 *p_buffer)
{
  mdi_class_t *p_this = (mdi_class_t *)_this;
  mdi_priv_t *p_priv = mdi_get_priv(_this);
  u16 i = 0;
  u16 node_id = 0;
  u32 sync_len = 0;
  u32 total_sync_len = 0;
  u32 atom_size = 0;
  dm_ret_t ret = ERR_FAILURE;

  if(NULL == p_this)
  {
    SSC_DEBUG("%s fail: non handle.\n", __FUNCTION__);
    return 0;
  }

  if(p_priv->user_id == 0)
  {
    SSC_DEBUG("%s fail: non handle.\n", __FUNCTION__);
    return 0;
  }

  if((p_priv->node_start_id == 0) || (p_priv->node_num == 0))
  {
    SSC_DEBUG("%s fail: non handle.\n", __FUNCTION__);
    return 0;
  }

  mtos_sem_take(&p_priv->sem, 0);
  if(p_priv->space_size == 0) //it's nonfix length, only one node
  {
    ret = dm_write_node(p_priv->p_dm_handle, p_priv->block_id, &p_priv->node_start_id,
      p_buffer, w_len);
    total_sync_len = w_len;
  }
  else
  {
    MT_ASSERT(w_len == p_priv->space_size); //must flush all
    atom_size = p_priv->atom_size;
    
    for(i = 0; i < p_priv->node_num; i++)
    {
      node_id = p_priv->node_start_id + i;
      if((p_priv->space_size - total_sync_len) < atom_size) //the last
      {
        sync_len = p_priv->space_size - total_sync_len;
      }
      else
      {
        sync_len = atom_size;
      }
      if(memcmp(p_priv->p_buf + total_sync_len, p_buffer + total_sync_len, sync_len) != 0)
      {
        memcpy(p_priv->p_buf + total_sync_len, p_buffer + total_sync_len, sync_len);
        ret = dm_write_node(p_priv->p_dm_handle, p_priv->block_id, &node_id,
          p_buffer + total_sync_len, sync_len);
      }
      total_sync_len += sync_len;
    }
  }
  
  mtos_sem_give(&p_priv->sem);
  return total_sync_len;
}

static void mdi_destroy(handle_t _this)
{
  if(_this != NULL)
  {
    mdi_priv_t *p_priv = mdi_get_priv(_this);
    if(p_priv->p_buf != NULL)
    {
      mtos_free(p_priv->p_buf);
    }
    mtos_sem_destroy(&p_priv->sem, 0);
    mtos_free(_this);
  }
}

static mdi_globo_info_t * mdi_init_globo_info(u8 cur_block_id)
{
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  mdi_globo_info_t *p_globo = (mdi_globo_info_t *)class_get_handle_by_id(MDI_CLASS_ID);
  u32 size = 0;
  u32 r_size = 0;

  if(p_globo == NULL) //no create
  {
    p_globo = (mdi_globo_info_t *)mtos_malloc(sizeof(mdi_globo_info_t));

    if(p_globo == NULL)
    {
      SSC_DEBUG("%s fail: no mem.\n", __FUNCTION__);
      return NULL;
    }
    
    memset(p_globo, 0, sizeof(mdi_globo_info_t));
    size = (u32)p_globo->head_arry - (u32)p_globo;

    //load head
    r_size = dm_read(p_dm_handle, cur_block_id, MDI_HEAD_NODE_ID, 0, size, (u8 *)p_globo);
    if(r_size == 0) //no record
    {
      p_globo->using_block_id = cur_block_id;
      p_globo->last_node_id = MDI_HEAD_NODE_ID + 1; //follow head
    }
    else
    {
      if(p_globo->using_block_id != cur_block_id)
      {
        SSC_DEBUG("%s fail: don't support current block.\n", __FUNCTION__);
        mtos_free(p_globo);
        return NULL;
      }
      MT_ASSERT(p_globo->using_head_cnt <= MDI_HEAD_MAX_NUM);
      if(p_globo->using_head_cnt > 0)
      {
        dm_read(p_dm_handle, cur_block_id, MDI_HEAD_NODE_ID, size,
          sizeof(mdi_head_t) * p_globo->using_head_cnt, (u8 *)p_globo->head_arry);
      }
    }

    class_register(MDI_CLASS_ID, (class_handle_t)p_globo);
    return p_globo;
  }
  else
  {
    if(p_globo->using_block_id != cur_block_id)
    {
      SSC_DEBUG("%s fail: don't support multi-block.\n", __FUNCTION__);
      return NULL;
    }
    else
    {
      return p_globo;
    }
  }
}


mdi_class_t * mdi_create_instance(u8 block_id, u32 user_id, u32 space_size)
{
  mdi_globo_info_t *p_globo = NULL;
  u32 size = sizeof(mdi_class_t) + sizeof(mdi_priv_t);
  mdi_class_t *p_this = (mdi_class_t *)mtos_malloc(size);
  mdi_priv_t *p_priv = NULL;
  u32 i = 0;

  if(NULL == p_this)
  {
    SSC_DEBUG("%s fail: no mem.\n", __FUNCTION__);
    return NULL;
  }

  p_globo = mdi_init_globo_info(block_id);
  if(p_globo == NULL)
  {
    mtos_free(p_this);
    return NULL;
  }

  memset(p_this, 0, size);
  p_priv = (mdi_priv_t *)(p_this + 1);
  mtos_sem_create(&p_priv->sem, TRUE);
  p_priv->user_id = user_id;
  p_priv->block_id = block_id;
  p_priv->space_size = space_size;
  p_priv->atom_size = MDI_ATOM_SIZE;
  p_priv->p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  p_this->p_private_data = (void *)p_priv;
  p_this->read = mdi_read;
  p_this->write = mdi_write;
  p_this->destroy = mdi_destroy;

  //match id
  for(i = 0; i < p_globo->using_head_cnt; i++)
  {
    if(user_id == p_globo->head_arry[i].user_id)
    {
      p_priv->node_start_id = p_globo->head_arry[i].node_start_id;
      p_priv->node_num = p_globo->head_arry[i].node_num;
      mdi_load_instance(p_priv);
      return p_this;
    }
  }

  //add new id 
  mdi_add_instance(p_globo, p_priv);

  return p_this;
}




