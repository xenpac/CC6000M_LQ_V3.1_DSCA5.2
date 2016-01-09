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
#include "mtos_sem.h"
#include "lib_memf.h"
#include "class_factory.h"
#include "fifo_db.h"

#ifdef WIN32
#define FIFO_DB_DBG OS_PRINTF
#else
#define FIFO_DB_DBG OS_PRINTF
#endif
/*!
  atom size
  */
#define FIFO_DB_ATOM_SIZE ((50) * (KBYTES))

/*!
  tag
  */
#define FIFO_DB_GET_PAYLOAD(x) ((u8 *)((x)->p_first_cell) + sizeof(fifo_db_cell_t))

/*!
  cell des
  */
typedef struct tag_fifo_db_cell_t
{
  /*!
    payload
    */
  u32 cur_cell_payload;
  /*!
    cext
    */
  struct tag_fifo_db_cell_t *p_next_cell;
}fifo_db_cell_t;

/*!
  x list define
  */
typedef struct tag_x_list_t
{
  /*!
    tag len
    */
  u32 tag_len;
  /*!
    real size
    */
  u32 data_size;
  /*!
    first cell
    */
  fifo_db_cell_t *p_first_cell;
  /*!
    next
    */
  struct tag_x_list_t *p_next;
}x_list_t;

/*!
  fifo db define
  */
typedef struct tag_fifo_db_priv_t
{
  /*!
    list
    */
  lib_memf_t memf_list;
  /*!
    db
    */
  lib_memf_t memf_db;
  /*!
    buffer
    */
  u8 *p_buffer;
  /*!
    size
    */
  u32 db_size;
  /*!
    cell_num
    */
  u32 cell_num;
  /*!
    h
    */
  x_list_t list_head;
  /*!
    sem
    */
  os_sem_t sem;
}fifo_db_priv_t;

static void x_list_show(char *p_name, x_list_t *p_first)
{
  #if 0
  u32 i = 0;

  FIFO_DB_DBG("x list show %s:\n", p_name);
  
  while(p_first != NULL)
  {
    FIFO_DB_DBG("[%d] %s size %d\n", i, FIFO_DB_GET_PAYLOAD(p_first), p_first->data_size);
    p_first = p_first->p_next;
    i++;
  }
  #endif
}

static void x_list_top_node(fifo_db_priv_t *p_this, x_list_t *p_node)
{
  x_list_t *p_h = &p_this->list_head;
  x_list_t *p_cur = p_h->p_next; //the first

  if(p_cur == p_node) //it's first. do nothing
  {
    return;
  }

  while(p_cur->p_next != NULL)
  {
    if(p_cur->p_next == p_node) //found, insert to head
    {
      p_cur->p_next = p_node->p_next;
      p_node->p_next = p_h->p_next;
      p_h->p_next = p_node;
      break;
    }
    else
    {
      p_cur = p_cur->p_next;
    }
  }

  x_list_show((char *)__FUNCTION__, p_node);
}

static void x_list_add_first_node(fifo_db_priv_t *p_this, x_list_t *p_node)
{
  x_list_t *p_h = &p_this->list_head;

  p_node->p_next = p_h->p_next;
  p_h->p_next = p_node;

  x_list_show((char *)__FUNCTION__, p_node);
}

static void x_list_del_last_node(fifo_db_priv_t *p_this)
{
  x_list_t *p_prev = &p_this->list_head; //the head
  x_list_t *p_last = NULL;
  fifo_db_cell_t *p_cell = NULL;
  fifo_db_cell_t *p_temp = NULL;

  if(NULL == p_prev->p_next) //no first
  {
    return;
  }

  //found the last
  while(NULL != p_prev->p_next->p_next)
  {
    p_prev = p_prev->p_next;
  }

  //get the last
  p_last = p_prev->p_next;
  p_prev->p_next = NULL;
  
  //free it
  p_cell = p_last->p_first_cell;

  while(p_cell != NULL)
  {
    p_temp = p_cell;
    p_cell = p_cell->p_next_cell;
    lib_memf_free(&p_this->memf_db, p_temp);
  }

  lib_memf_free(&p_this->memf_list, p_last);
  
  x_list_show((char *)__FUNCTION__, p_this->list_head.p_next);
}

static void cell_store_data(fifo_db_priv_t *p_this, fifo_db_cell_t *p_cell,
  char *p_tag, u8 *p_data, u32 data_size)
{
  u32 tag_len = strlen(p_tag) + 1;
  u8 *p_payload = NULL;
  u32 store_len = 0;
  u32 cell_max_payload = 0;
  u32 copy_len = 0;
  
  //payload must more than tag_len
  if((FIFO_DB_ATOM_SIZE - sizeof(fifo_db_cell_t)) <= tag_len)
  {
    return;
  }

  //first (include tag)
  cell_max_payload = FIFO_DB_ATOM_SIZE - sizeof(fifo_db_cell_t) - tag_len;
  p_payload = (u8 *)p_cell;
  p_payload += sizeof(fifo_db_cell_t);
  
  memcpy(p_payload, p_tag, tag_len);  //save tag
  p_payload += tag_len;
  
  if(cell_max_payload < data_size)
  {
    copy_len = cell_max_payload;
  }
  else
  {
    copy_len = data_size;
  }
  memcpy(p_payload, p_data, copy_len);  //save data
  store_len += copy_len;
  p_data += copy_len;
  p_cell->cur_cell_payload = tag_len + copy_len;
  

  //other
  cell_max_payload = FIFO_DB_ATOM_SIZE - sizeof(fifo_db_cell_t);
  while(store_len < data_size)
  {
    //to next
    p_cell = p_cell->p_next_cell;
    p_payload = (u8 *)p_cell;
    p_payload += sizeof(fifo_db_cell_t);

    if(cell_max_payload < (data_size - store_len))
    {
      copy_len = cell_max_payload;
    }
    else
    {
      copy_len = (data_size - store_len);
    }
    memcpy(p_payload, p_data, copy_len);  //save data
    store_len += copy_len;
    p_data += copy_len;
    p_cell->cur_cell_payload = copy_len;
  }

}

static void cell_capture_data(fifo_db_priv_t *p_this, fifo_db_cell_t *p_cell,
  u32 tag_len, u8 *p_data, u32 data_size)
{
  u8 *p_payload = NULL;
  u32 capture_len = 0;
  u32 cell_payload = 0;

  //the first cell (include tag)
  cell_payload = p_cell->cur_cell_payload - tag_len;
  p_payload = (u8 *)p_cell;
  p_payload += sizeof(fifo_db_cell_t) + tag_len;

  memcpy(p_data, p_payload, cell_payload);
  capture_len += cell_payload;
  p_data += cell_payload;
  
  //other cell (no tag)
  while(capture_len < data_size)
  {
    //to next
    p_cell = p_cell->p_next_cell;
    cell_payload = p_cell->cur_cell_payload;
    p_payload = (u8 *)p_cell;
    p_payload += sizeof(fifo_db_cell_t);

    memcpy(p_data, p_payload, cell_payload);
    capture_len += cell_payload;
    p_data += cell_payload;
  }

}

static fifo_db_cell_t * cell_query(fifo_db_priv_t *p_this, u32 num)
{
  fifo_db_cell_t *p_first = NULL;
  fifo_db_cell_t *p_last = NULL;
  fifo_db_cell_t *p_new = NULL;
  u32 i = 0;

  while(i < num)
  {
    p_new = (fifo_db_cell_t *)lib_memf_alloc(&p_this->memf_db);

    if(p_new != NULL) //found
    {
      memset(p_new, 0, sizeof(fifo_db_cell_t));//reset it

      if(NULL == p_first)
      {
        p_first = p_new;
        p_last = p_first;
      }
      else
      {
        p_last->p_next_cell = p_new;
        p_last = p_last->p_next_cell;
      }
      i++;
    }
    else
    {
      x_list_del_last_node(p_this);
    }
  }

  return p_first;
}


void fifo_db_init(void)
{
  fifo_db_priv_t *p_this = class_get_handle_by_id(FIFO_DB_CLASS_ID);
  
  if(p_this != NULL)
  {
    FIFO_DB_DBG("fifo db already inited\n");
    return;
  }
  p_this = mtos_malloc(sizeof(fifo_db_priv_t));
  memset(p_this, 0, sizeof(fifo_db_priv_t));
  class_register(FIFO_DB_CLASS_ID, p_this);
}

/*!
  Create fifo db

  \param[in] p_buf buffer addr
  \param[in] db_size database size

  \return 
  */
void fifo_db_create(u8 *p_buf, u32 db_size)
{
  fifo_db_priv_t *p_this = class_get_handle_by_id(FIFO_DB_CLASS_ID);
  u8 *p_cur = (u8 *)((u32)p_buf >> 2 << 2); //align 4bytes
  u32 max_cell_num = db_size / (FIFO_DB_ATOM_SIZE + sizeof(x_list_t));
  u32 memf_size = max_cell_num * sizeof(x_list_t);

  if(NULL == p_this)
  {
    FIFO_DB_DBG("the fifo db don't be init %d\n", __LINE__);
    return;
  }
  
  if(p_this->p_buffer != NULL)
  {
    FIFO_DB_DBG("has the fifo db\n");
    return;
  }
  
  db_size -= (p_cur - p_buf); //skip align

  lib_memf_create(&p_this->memf_list, (u32)p_cur, memf_size, sizeof(x_list_t));
  p_cur += memf_size; //skip memf
  db_size -= memf_size;

  p_this->p_buffer = p_cur;
  p_this->cell_num = db_size / FIFO_DB_ATOM_SIZE;
  p_this->db_size = p_this->cell_num * FIFO_DB_ATOM_SIZE; //align
  lib_memf_create(&p_this->memf_db, (u32)p_cur, p_this->db_size, FIFO_DB_ATOM_SIZE);
  
  mtos_sem_create(&p_this->sem, TRUE);
}

void fifo_db_destroy(void)
{
  fifo_db_priv_t *p_this = class_get_handle_by_id(FIFO_DB_CLASS_ID);

  if(NULL == p_this)
  {
    FIFO_DB_DBG("the fifo db don't be init %d\n", __LINE__);
    return;
  }
  
  if(NULL == p_this->p_buffer)
  {
    FIFO_DB_DBG("no fifo db\n");
    return;
  }

  lib_memf_destroy(&p_this->memf_list);
  lib_memf_destroy(&p_this->memf_db);
  mtos_sem_destroy(&p_this->sem, 0);
  memset(p_this, 0, sizeof(fifo_db_priv_t));
}

/*!
  add data to fifo db

  \param[in] p_tag data tag
  \param[in] p_data data
  \param[in] data_size data size

  \return 
  */
void fifo_db_add(char *p_tag, u8 *p_data, u32 data_size)
{
  fifo_db_priv_t *p_this = class_get_handle_by_id(FIFO_DB_CLASS_ID);
  u32 tag_len = strlen(p_tag) + 1;
  u32 cell_payload = FIFO_DB_ATOM_SIZE - sizeof(fifo_db_cell_t);
  u32 total_cell = (data_size + tag_len + cell_payload - 1) / cell_payload;
  x_list_t *p_list = NULL;
  fifo_db_cell_t *p_cell = NULL;
  
  if(NULL == p_this)
  {
    FIFO_DB_DBG("the fifo db don't be init %d\n", __LINE__);
    return;
  }

  if((total_cell * FIFO_DB_ATOM_SIZE) > p_this->db_size) //to big
  {
    return;
  }

  if(tag_len >= cell_payload)  //long long tag. don't support
  {
    return;
  }
  
  mtos_sem_take(&p_this->sem, 0);

  p_list = (x_list_t *)lib_memf_alloc(&p_this->memf_list);
  if(NULL == p_list)
  {
    //free the oldest
    FIFO_DB_DBG("ERROR %d\n", __LINE__);
    x_list_del_last_node(p_this);
    p_list = (x_list_t *)lib_memf_alloc(&p_this->memf_list);
    return;
  }

  memset(p_list, 0, sizeof(x_list_t));
  p_list->tag_len = tag_len;
  p_list->data_size = data_size;
  p_cell = cell_query(p_this, total_cell);
  p_list->p_first_cell = p_cell;

  //save data
  cell_store_data(p_this, p_cell, p_tag, p_data, data_size);
  //add list
  x_list_add_first_node(p_this, p_list);
  
  mtos_sem_give(&p_this->sem);
}

/*!
  add data to fifo db

  \param[in] p_tag data tag
  \param[out] pp_data data

  \return data size, if it's zero means don't found
  */
u32 fifo_db_query(char *p_tag, u8 **pp_data)
{
  fifo_db_priv_t *p_this = class_get_handle_by_id(FIFO_DB_CLASS_ID);
  char *p_db_tag = NULL;
  x_list_t *p_cur = NULL;
  u8 *p_copy = NULL;

  if(NULL == p_this)
  {
    FIFO_DB_DBG("the fifo db don't be init %d\n", __LINE__);
    return 0;
  }

  mtos_sem_take(&p_this->sem, 0);
  p_cur = p_this->list_head.p_next; //the first data

  while(p_cur != NULL)
  {
    if(p_cur->tag_len == (strlen(p_tag) + 1))
    {
      p_db_tag = FIFO_DB_GET_PAYLOAD(p_cur);
      
      if(strcmp(p_db_tag, p_tag) == 0)  //found
      {
        x_list_top_node(p_this, p_cur); //top it
        
        p_copy = mtos_malloc(p_cur->data_size);
        if(p_copy != NULL)
        {
          //get data
          cell_capture_data(p_this, p_cur->p_first_cell,
                      p_cur->tag_len, p_copy, p_cur->data_size);
          *pp_data = p_copy;
          mtos_sem_give(&p_this->sem);
          //FIFO_DB_DBG("fifo_db_query success %s size %d\n", p_tag, p_cur->data_size);
          return p_cur->data_size;
        }
        else
        {
          break;
        }
      }
    }

    p_cur = p_cur->p_next;  //to next
  }

  //FIFO_DB_DBG("fifo_db_query fail %s \n", p_tag);
  mtos_sem_give(&p_this->sem);
  return 0;
}

