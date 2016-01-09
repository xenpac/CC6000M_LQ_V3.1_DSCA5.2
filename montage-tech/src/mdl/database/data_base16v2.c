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
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"

#include "class_factory.h"
#include "data_manager.h"
#include "data_base.h"
#include "data_base_priv.h"
#include "data_base16v2.h"


//#define DB_DEBUG

#ifdef DB_DEBUG
#define DB_DBG    OS_PRINTF(" :%s, %d : ",__FUNCTION__, __LINE__); OS_PRINTF
#else
#define DB_DBG    DUMMY_PRINTF
#endif

/*!
  default database block state.
  */
#define DB_BLK_DAFAULT_STAT (0xFF)

/*!
  Invalid table id.
  */
#define INVALID_TABLE_ID  (0xFF)

/*!
  max supports 3 table : SAT/TP/PG
  */
#define MAX_TABLE_NUM (3)

/*!
  view bundle size.
  */
#define VIEW_BUNDLE_SIZE  (28)

/* !
 view item format:
    bit[0-13]: elements id,
    bit[14]:	 select flag
    bit[15]:	 del flag    1: active, 0: deleted
    And all mark bit will be put into user database
  */
#define ACTIVE_BIT_16V2       (15)

/*!
  select bit	
  */
#define SELECT_BIT_16V2       (14)
/*!
  mark shift bit.
  */
#define MARK_SHIFT_BIT        (14)

/*!
  null item.
  */
#define NULL_ITEM   ((item_type_t)(~0))

/*!
  item id
  */
#define ITEM_ID(x)    ((x) & (0x3FFF))

/*!
  select pos in the view item : 1 is select; 0 is not select.
  */
#define DB_SEL_POS_IN_VIEW    (14)

/*!
  delete pos in the view item : 1 is active; 0 is delete.
  */
#define DB_DEL_POS_IN_VIEW    (15)

/*!
  is num item
  */
#define IS_NULL_ITEM(x) ((x) == NULL_ITEM)

/*!
  is active
  */
#define IS_ACTIVE(x)  ((x) & (1 << ACTIVE_BIT_16V2))

/*!
  is select
  */
#define IS_SELECT(x)  ((x) & (1 << SELECT_BIT_16V2))

/*!
  View information in structure
  */
typedef struct tag_db_vv_info
{
 /*!
  system table id
  */
  u8 table_id;
 /*!
  Whether simply bit operation is enough or not for creating virtual view
  */
  u8 bit_only;
 /*!
  Virtual view needs update or notc
  */
  BOOL view_need_up;
 /*!
  Element needs udpate
  */
  BOOL element_need_up;
 /*!
  Increment flash writing mode or not
  */
  BOOL plus_only;
 /*!
  Virtual view delete item flag
  */
  BOOL view_del_flag;
 /*!
  View creating function
  */
  db_filter_proc_t cond_func;
 /*!
  Memory for storing view item
  */
  item_type_t *p_mem;
 /*!
  Context information
  */
  u32 context;
 /*!
  Total item number in virtual view including active and deactive one
  */
  u16 vv_total;
 /*!
  Total item number when view is created
  */
  u16 create_num;
 /*!
  Active num meeting filter condition in current viretual view
  */
  u16 active_num;
 /*!
  Item number not meeting filter condition when create virtual view
  */
  u16 pass_num;
}db_vv_info_t;

/*!
   Table info
  */
typedef struct tag_table_info_t
{
 /*!
  Block id
  */
  u8 block_id;
 /*!
  Max element number
  */
  u16 max_element_number;
}table_info_t;

/*!
   View information
  */
typedef struct tag_view_info_t
{
 /*!
  Node id start
  */
  u16 ni_start;
 /*!
  Include packet number
  */
  u16 packet_total;
 /*!
  Block id of the rule
  */
  u8 block_id;
 /*!
  mem back up, use for sync flash.(item_type_t)
  */
  item_type_t *p_mem_backup;
 /*!
  is sync flash.
  */
  BOOL sync_st;
 /*!
  item count
  */
  u32 item_count;
}view_info_t;

/*!
  the structure is node saving to flash
  */
typedef struct tag_view_packet_t
{
 /*!
  View table id
  */
  u8 table_id;
 /*!
  reserved
  */
  u8 reserved;
 /*!
  View packet index
  */
  u16 packet_idx;
 /*!
  View item
  */
  item_type_t item[VIEW_BUNDLE_SIZE];
}view_packet_t;

/*!
 Global handle define for database 16v2
 */
typedef struct tag_db_16v2_var
{
 /*!
  View table information
  */
  table_info_t table_info[MAX_TABLE_NUM];
 /*!
  Real view information
  */
  view_info_t view_info[MAX_TABLE_NUM];
 /*!
  Virtual view information
  */
  db_vv_info_t vv_info[MAX_VIRTUAL_VIEW_NUM];
 /*!
  Max block number
  */
  u16 max_blk_num;
 /*!
  Node id used
  */
  u16 node_id_used;
 /*!
  Clean block
  */
  u8 clean_block;
 /*!
  data manager handle.
  */
  void *p_dm_handle;
 /*!
  view node.
  */
  u32 max_view_node_num;
}db_var_t;

/*!
  set element update status in all vv_info.
  */
static void set_element_update_status(void *p_data, u8 table_id)
{
  u32 vv_cnt = 0;
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = NULL;

  MT_ASSERT(p_db_var->vv_info != NULL);
  p_vv_info = p_db_var->vv_info;

  MT_ASSERT(p_vv_info != NULL);

  for(vv_cnt = 0; vv_cnt < MAX_VIRTUAL_VIEW_NUM; vv_cnt++)
  {
    if(p_vv_info[vv_cnt].table_id == table_id)
    {
      p_vv_info[vv_cnt].element_need_up = TRUE;
    }
  }
}

/*!
   create a table into blocks
   the elements of table are pure data structures, such as SAT ,TP, PG and so
   on.

   \param[in] block_id which block save the table's data
   \param[in] elements_num table size
   \return Return created table id.
  */
u8 db_create_table_16v2(void *p_data, u8 block_id, u16 elements_num)
{
  db_var_t     *p_db_var    = (db_var_t *)p_data;
  void         *p_dm_handle = p_db_var->p_dm_handle;
  table_info_t *p_tab_info  = p_db_var->table_info;
  view_info_t  *p_view_info = p_db_var->view_info;
  u16           nodeid_used = 0;
  u32           table_id    = 0;
  u8            clean_block = DB_BLK_DAFAULT_STAT;

  MT_ASSERT(p_dm_handle != NULL);
  MT_ASSERT(p_tab_info  != NULL);
  MT_ASSERT(p_view_info != NULL);

  nodeid_used = p_db_var->node_id_used;
  clean_block = p_db_var->clean_block;

  for(table_id = 0; table_id < MAX_TABLE_NUM; table_id++)
  {
    if(p_tab_info[table_id].block_id == INVALID_TABLE_ID)
    {
      p_tab_info[table_id].block_id = block_id;
      p_tab_info[table_id].max_element_number = elements_num;

      p_view_info[table_id].ni_start = nodeid_used;
      if(p_view_info[table_id].p_mem_backup == NULL)
      {
        p_view_info[table_id].p_mem_backup =
                              (item_type_t *)mtos_malloc(elements_num * sizeof(item_type_t));
        MT_ASSERT(p_view_info[table_id].p_mem_backup != NULL);
        memset(p_view_info[table_id].p_mem_backup, 0xff, elements_num * sizeof(item_type_t));

        p_view_info[table_id].sync_st = FALSE;
        p_view_info[table_id].item_count = 0;
      }

      DB_DBG(" tab=%d ni=%d\n", table_id,p_view_info[table_id].ni_start);
      nodeid_used += (elements_num + VIEW_BUNDLE_SIZE - 1) / VIEW_BUNDLE_SIZE;
      MT_ASSERT(nodeid_used < p_db_var->max_view_node_num);

      if(block_id != clean_block)
      {
        /*!
         reset this block node inactive. uses for table and view sync.
         in create_view, if view active, call dm_active_node active table's node.
         */
        dm_reset_iw_block(p_dm_handle, block_id, FALSE);
        clean_block = block_id;
      }
      p_db_var->node_id_used = nodeid_used;
      p_db_var->clean_block  = clean_block;

      return table_id;
    }
  }
  MT_ASSERT(0);
  return INVALID_TABLE_ID;
}


/*!
   create a view for table
   the view associate a table , store element's state and position

   \param[in] block_id which block save the view's data
   \param[in] table_id which table associate to
   \return Return created view id.
  */
u8 db_create_view_16v2(void *p_data, u8 block_id, u8 table_id)
{
  u32           i           = 0;
  u32           pos         = 0;
  view_packet_t packet      = {0};
  db_var_t     *p_db_var    = (db_var_t *)p_data;
  view_info_t  *p_view_info = p_db_var->view_info;
  table_info_t *p_tab_info  = p_db_var->table_info;
  void         *p_dm_handle = p_db_var->p_dm_handle;

  MT_ASSERT(p_view_info != NULL);
  MT_ASSERT(p_tab_info  != NULL);
  MT_ASSERT(p_dm_handle != NULL);
  MT_ASSERT((table_id < MAX_TABLE_NUM) && (p_view_info[table_id].block_id == INVALID_TABLE_ID));

  //view block id must not equal to table block id.
  MT_ASSERT(block_id != p_tab_info[table_id].block_id);

  p_view_info[table_id].block_id = block_id;

#if 0
  if(block_id == p_tab_info[table_id].block_id)
  {
    u16 cur_view_bundle =
      (p_tab_info[table_id].max_element_number + VIEW_BUNDLE_SIZE - 1) / VIEW_BUNDLE_SIZE;

    dm_set_fix_id(p_dm_handle, block_id, p_view_info[table_id].ni_start + cur_view_bundle);

    for(i = 0; i < cur_view_bundle; i++)
    {
      dm_active_node(p_dm_handle, p_view_info[table_id].block_id,
        p_view_info[table_id].ni_start + i);
    }
  }
#endif

  DB_DBG(" macro = %d, tab_id = %d ni = %d\n",
          p_db_var->max_view_node_num, table_id, p_view_info[table_id].ni_start);

  //load view node , from discrete block node's to list
  for(i = p_view_info[table_id].ni_start; i < p_db_var->max_view_node_num; i++)
  {
    //load finish
    if(dm_read(p_dm_handle, block_id, i, 0, sizeof(view_packet_t),
               (u8 *)(&packet)) == 0)
    {
      break;
    }

    if(packet.table_id != table_id) //found other view data, means load finish
    {
      break;
    }

    MT_ASSERT((i - p_view_info[table_id].ni_start) == packet.packet_idx);

    for(pos = 0; pos < VIEW_BUNDLE_SIZE; pos++)
    {
      if(IS_NULL_ITEM(packet.item[pos]) == FALSE)
      {
        dm_active_node(p_dm_handle, p_tab_info[table_id].block_id,
                       ITEM_ID(packet.item[pos]));
      }
      else
      {
        break;
      }
    }
  }

  p_view_info[table_id].packet_total = i - p_view_info[table_id].ni_start;
  return table_id;
}


/*!
   add a element to table

   \param[in] table_id which block add into
   \param[out] p_element_id id of the new element
   \param[in] pbuffer element's addr
   \param[in] len element's length
   \return Return opt state
  */
static db_ret_t db_add_element_16v2(void *p_data,
                                    u8 table_id,
                                    u16 *p_element_id,
                                    u8 *p_buffer,
                                    u16 len)
{
  db_var_t     *p_db_var     = (db_var_t *)p_data;
  table_info_t *p_table_info = p_db_var->table_info;
  void         *p_dm_handle  = p_db_var->p_dm_handle;
  dm_ret_t      ret = DM_FAIL;

  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_dm_handle != NULL);

  *p_element_id = DB_UNKNOWN_ID;
  ret = dm_write_node(p_dm_handle, p_table_info[table_id].block_id,
                      p_element_id, p_buffer, len);

  DB_DBG("table_id %d, .block_id %d, _element_id %d\n",
          table_id, p_table_info[table_id].block_id, *p_element_id);
  if(ret == DM_SUC)
  {
    dm_flush_cache(p_dm_handle, p_table_info[table_id].block_id, *p_element_id);
    return DB_SUC;
  }
  else if(ret == DM_FULL)
  {
    return DB_FULL;
  }
  else
  {
    return DB_FAIL;
  }
}


/*!
   edit a element

   \param[in] table_id which block the elements belong to
   \param[in] element_id which element will get
   \param[in] pbuffer element's addr
   \param[in] offset read offset for first addr
   \param[in] len read length
   \return operation Success or not . TRUE is SUC.
  */
BOOL db_get_element_16v2(void *p_data,
                         u8 table_id,
                         u16 element_id,
                         u8 *p_buffer,
                         u16 offset,
                         u16 len)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  table_info_t *p_table_info = p_db_var->table_info;

  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_dm_handle != NULL);

  return dm_read(p_dm_handle, p_table_info[table_id].block_id,
                  element_id, offset, len, p_buffer) > 0;
}


/*!
   delete a element from table

   \param[in] table_id which block the elements belong to
   \param[in] element_id which element will be deleted
   \return Return None
  */
static void db_del_element_16v2(void *p_data, u8 table_id, u16 element_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  table_info_t *p_table_info = p_db_var->table_info;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_dm_handle != NULL);

  DB_DBG("table_id %d, .block_id %d, _element_id %d\n",
          table_id, p_table_info[table_id].block_id, element_id);

  dm_del_node(p_dm_handle, p_table_info[table_id].block_id, element_id, TRUE);
}


/*!
   edit a element

   \param[in] table_id which block the elements belong to
   \param[in] element_id which element will be edit
   \param[in] pbuffer element's addr
   \param[in] len element's length
   \return operation Success or not . TRUE is SUC.
  */
db_ret_t db_edit_element_16v2(void *p_data,
                              u8 table_id,
                              u16 element_id,
                              u8 *p_buffer,
                              u16 len)
{
  u8 temp = 0;
  db_var_t *p_db_var = (db_var_t *)p_data;
  void *p_dm_handle = p_db_var->p_dm_handle;
  table_info_t *p_table_info = p_db_var->table_info;

  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_dm_handle != NULL);

  if(element_id == DB_UNKNOWN_ID)
  {
    DB_DBG("Unkown id\n");
    return DB_NOT_FOUND;
  }

  if(db_get_element_16v2(p_data, table_id, element_id, &temp, 0, 1) == TRUE)
  {
    if(dm_write_node(p_dm_handle, p_table_info[table_id].block_id,
                     &element_id, p_buffer, len) == DM_SUC)
    {
      set_element_update_status(p_data, table_id);
      DB_DBG("table_id %d, element_id %d\n",table_id, element_id);
      return DB_SUC;
    }
    DB_DBG("FAILED TO write\n");
    return DB_FAIL;
  }
  else
  {
    DB_DBG("Data not found\n");
    return DB_NOT_FOUND;
  }
}


void db_save_element_edit_16v2(void *p_data, u8 table_id, u16 element_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  table_info_t *p_table_info = p_db_var->table_info;
  void *p_dm_handle = p_db_var->p_dm_handle;

  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_dm_handle != NULL);

  DB_DBG("table_id %d, .block_id %d, _element_id %d\n",
          table_id, p_table_info[table_id].block_id, element_id);
  dm_flush_cache(p_dm_handle, p_table_info[table_id].block_id, element_id);
}


void db_undo_element_edit_16v2(void *p_data, u8 table_id, u16 element_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  table_info_t *p_table_info = p_db_var->table_info;
  void *p_dm_handle = p_db_var->p_dm_handle;

  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_dm_handle != NULL);

  DB_DBG("table_id %d, .block_id %d, _element_id %d\n",
          table_id, p_table_info[table_id].block_id, element_id);

  dm_restore_cache(p_dm_handle, p_table_info[table_id].block_id, element_id);
}

/*!
  check paramter :
  1. check plus, if pos < create_num, plus_only = FALSE;
  2.
  */
static BOOL check_parameter(void *p_data,
                            u8 vv_id,
                            u16 pos,
                            u8 mask_shift,
                            BOOL check_plus)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  MT_ASSERT(p_vv_info != NULL);

  if(vv_id >= MAX_VIRTUAL_VIEW_NUM)
  {
    return FALSE;
  }
  MT_ASSERT(p_vv_info[vv_id].p_mem != NULL);

  if(check_plus
    && p_vv_info[vv_id].plus_only
    && (pos < p_vv_info[vv_id].create_num))
  {
    p_vv_info[vv_id].plus_only = FALSE;
  }

  return (pos < p_vv_info[vv_id].vv_total)
         && (mask_shift < MAX_MARK_NUM);
}

/*!
  judge filter function.
 */
static BOOL is_filter_item(void *p_data, u8 vv_id, item_type_t item)
{
  db_var_t     *p_db_var  = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  u8            table_id  = 0;
  u32           context   = 0;

  MT_ASSERT(p_vv_info != NULL);

  table_id = p_vv_info[vv_id].table_id;
  context = p_vv_info[vv_id].context;

  // if cond_func is NULL, view all
  return (p_vv_info[vv_id].cond_func == NULL)
         || p_vv_info[vv_id].cond_func((item >> MARK_SHIFT_BIT),
                                       context, table_id,ITEM_ID(item));
}

static void is_filter_item_all(void *p_data, u8 vv_id, item_type_t item,
  u16 *p_ret,u16 fav_count,db_filter_proc_new_t p_cond_func)
{
  db_var_t     *p_db_var  = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  u8            table_id  = 0;
  u32           context   = 0;
  u32           k         = 0;
  u32           ret       = 0;

  MT_ASSERT(p_vv_info != NULL);

  table_id = p_vv_info[vv_id].table_id;
  context = p_vv_info[vv_id].context;

  ret = p_cond_func((item >> MARK_SHIFT_BIT),
                   fav_count,table_id,ITEM_ID(item));

  for(k = 0; k < fav_count; k++)
  {
     // if cond_func is NULL, view all
    if((ret & (1 << k)) != 0)
      p_ret[k]++;
  }

}

/*!
   create virtual view from a real view with a filter

   \param[in] table_id the table which one created
   \param[in] cond_func callback function , give a condition by create view
    if cond_func is NULL, create all
   \param[in] bit_only need loading element data, or not. if it's FALSE, need
   loading
   \param[in] mem extend virtual view, if the mem is NULL, create view in
   internal buffer
   \param[in] context
   \param[out] *p_vv_id id of the created virtual view
   \return Return item number of view list.
  */
u16 db_create_virtual_view_16v2(void *p_data,
                                u8 table_id,
                                db_filter_proc_t p_cond_func,
                                BOOL bit_only,
                                u8 *p_mem,
                                u32 context,
                                u8 *p_vv_id)
{
  db_var_t     *p_db_var     = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info    = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;
  view_info_t  *p_view_info  = p_db_var->view_info;
  void         *p_dm_handle  = p_db_var->p_dm_handle;
  item_type_t   cur_item     = 0;
  u16           find_cnt     = 0;
  u32           i            = 0;
  u32           j            = 0;
  u8            cur_vv_id    = 0;
  view_packet_t packet       = {0};
  u16     max_element_number = 0;
  u16           read_len = 0;

  MT_ASSERT(p_vv_info    != NULL);
  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_view_info  != NULL);
  MT_ASSERT(p_dm_handle  != NULL);
  MT_ASSERT(p_mem        != NULL);

  for(i = 0; i < MAX_VIRTUAL_VIEW_NUM; i++)  // default view buffer's id is 0
  {
    if((p_vv_info[i].p_mem == NULL)
      || (p_vv_info[i].p_mem == (item_type_t *)p_mem)) //no use??
    {
      p_vv_info[i].p_mem = (item_type_t *)p_mem;
      cur_vv_id = i;
      break;
    }
  }
  MT_ASSERT(i < MAX_VIRTUAL_VIEW_NUM);

  *p_vv_id = cur_vv_id;

  //reset view buffer
  p_vv_info[cur_vv_id].table_id = table_id;
  p_vv_info[cur_vv_id].bit_only = bit_only; //TRUE
  p_vv_info[cur_vv_id].view_need_up = FALSE;
  p_vv_info[cur_vv_id].view_del_flag = FALSE;
  p_vv_info[cur_vv_id].plus_only = TRUE;
  p_vv_info[cur_vv_id].cond_func = p_cond_func;
  p_vv_info[cur_vv_id].context = context;
  p_vv_info[cur_vv_id].pass_num = 0;

  max_element_number =
    p_table_info[p_vv_info[cur_vv_id].table_id].max_element_number;
  memset(p_vv_info[cur_vv_id].p_mem, 0XFF,
         sizeof(item_type_t) * max_element_number);

  if((p_view_info[table_id].p_mem_backup != NULL) &&
    (p_view_info[table_id].sync_st == TRUE))
  {
    for(i = 0; i < p_view_info[table_id].packet_total; i++)
    {
      //read cur_item
      memset(&packet, 0XFF, sizeof(packet));
      if(i != p_view_info[table_id].packet_total - 1)
      {
        memcpy(packet.item,
               &p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
               sizeof(item_type_t) * VIEW_BUNDLE_SIZE);
      }
      else if(i == 0)
      {
        memcpy(packet.item,
                &p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
                sizeof(item_type_t) * (p_view_info[table_id].item_count));
      }
      else
      {
        memcpy(packet.item,
                &p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
                sizeof(item_type_t) * (p_view_info[table_id].item_count - i * VIEW_BUNDLE_SIZE));
      }

      for(j = 0; j < VIEW_BUNDLE_SIZE; j++)
      {
        if(IS_NULL_ITEM(packet.item[j]) == TRUE) //found finish
        {
          i = p_view_info[table_id].packet_total; //use break outside circle
          break;
        }
        cur_item = packet.item[j];
        //Fetch information in detail

        if(IS_ACTIVE(cur_item) != 0)
        {
          if(is_filter_item(p_data, cur_vv_id, cur_item) == TRUE)
          {
            p_vv_info[cur_vv_id].p_mem[find_cnt++] = cur_item;
          }
          else
          {
            p_vv_info[cur_vv_id].pass_num++;
          }
        }
        else
        { 
          DB_DBG("is active %d, select %d, item_id %d\n",
              IS_ACTIVE(packet.item[j]), IS_SELECT(packet.item[j]), ITEM_ID(packet.item[j]));
          MT_ASSERT(0); //can't exist deleted item
        }
      }

    }
  }
  else
  {
    memset(p_view_info[table_id].p_mem_backup, 0xff, sizeof(item_type_t)* max_element_number);

    for(i = 0; i < p_view_info[table_id].packet_total; i++)
    {
      //read cur_item
      read_len = dm_read(p_dm_handle, p_view_info[table_id].block_id,
                         p_view_info[table_id].ni_start + i, 0,
                         sizeof(view_packet_t), (u8 *)&packet);
      MT_ASSERT(read_len != 0);
      for(j = 0; j < VIEW_BUNDLE_SIZE; j++)
      {
        if(IS_NULL_ITEM(packet.item[j]) == TRUE) //found finish
        {
          i = p_view_info[table_id].packet_total; //use break outside circle
          if(p_view_info[table_id].p_mem_backup != NULL)
          {
            /*memset(p_mem_backup + 2 * ((i > 0 ?i - 1:0) * VIEW_BUNDLE_SIZE + j),0xff,
              2 * (max_element_number - ((i > 0 ?i - 1:0) * VIEW_BUNDLE_SIZE + j)));*/
            p_view_info[table_id].sync_st = TRUE;
            p_view_info[table_id].item_count = (i > 0 ?i - 1:0) * VIEW_BUNDLE_SIZE + j;

          }
          break;
        }
        cur_item = packet.item[j];
        if(p_view_info[table_id].p_mem_backup != NULL)
          p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE + j] = packet.item[j];

        //Fetch information in detail
        if(IS_ACTIVE(cur_item) != 0)
        {
          if(is_filter_item(p_data, cur_vv_id, cur_item) == TRUE)
          {
            p_vv_info[cur_vv_id].p_mem[find_cnt++] = cur_item;

          }
          else
          {
            p_vv_info[cur_vv_id].pass_num++;
          }
        }
        else
        {
          MT_ASSERT(0); //can't exist deleted item
        }
      }
    }
  }

  DB_DBG("cur_vv_id %d, table_id %d, num %d\n", cur_vv_id, table_id, find_cnt);
  p_vv_info[cur_vv_id].vv_total   = find_cnt;
  p_vv_info[cur_vv_id].create_num = find_cnt;
  p_vv_info[cur_vv_id].active_num = find_cnt;
  return find_cnt;
}

/*!
   create virtual view from a real view with a filter

   \param[in] table_id the table which one created
   \param[in] cond_func callback function , give a condition by create view
    if cond_func is NULL, create all
   \param[in] bit_only need loading element data, or not. if it's FALSE, need
   loading
   \param[in] mem extend virtual view, if the mem is NULL, create view in
   internal buffer
   \param[in] p_context
   \param[in] fav_count
   \param[out] *p_vv_id id of the created virtual view
   \return Return item number of view list.
  */
void db_create_virtual_view_all_16v2(void *p_data,
                                u8 table_id,
                                db_filter_proc_new_t p_cond_func,
                                BOOL bit_only,
                                u8 *p_mem,
                                u16 *p_context,
                                u16 fav_count,
                                u8 *p_vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;
  view_info_t *p_view_info = p_db_var->view_info;
  void *p_dm_handle = p_db_var->p_dm_handle;
  u32 i = 0;
  u32 j = 0;
  item_type_t cur_item = 0;
  u8 cur_vv_id = 0;
  view_packet_t packet;
  u16 max_element_number = 0;
  u16 read_len = 0;

  MT_ASSERT(p_vv_info != NULL);
  MT_ASSERT(p_table_info != NULL);
  MT_ASSERT(p_view_info != NULL);
  MT_ASSERT(p_dm_handle != NULL);
  MT_ASSERT(p_mem != NULL);

  for(i = 0; i < MAX_VIRTUAL_VIEW_NUM; i++)  // default view buffer's id is 0
  {
    if((p_vv_info[i].p_mem == NULL)
    || (p_vv_info[i].p_mem == (item_type_t *)p_mem)) //no use??
    {
      p_vv_info[i].p_mem = (item_type_t *)p_mem;
      cur_vv_id = i;
      break;
    }
  }
  MT_ASSERT(i < MAX_VIRTUAL_VIEW_NUM);

  *p_vv_id = cur_vv_id;

  //reset view buffer
  p_vv_info[cur_vv_id].table_id = table_id;
  p_vv_info[cur_vv_id].bit_only = bit_only; //TRUE
  p_vv_info[cur_vv_id].view_need_up = FALSE;
  p_vv_info[cur_vv_id].view_del_flag = FALSE;
  p_vv_info[cur_vv_id].plus_only = TRUE;
  p_vv_info[cur_vv_id].cond_func = NULL;
  p_vv_info[cur_vv_id].context = 0;
  p_vv_info[cur_vv_id].pass_num = 0;

  max_element_number =
    p_table_info[p_vv_info[cur_vv_id].table_id].max_element_number;
  memset(p_vv_info[cur_vv_id].p_mem, 0XFF,
   sizeof(item_type_t) * max_element_number);

  if((p_view_info[table_id].p_mem_backup != NULL) &&
    (p_view_info[table_id].sync_st == TRUE))
  {
    for(i = 0; i < p_view_info[table_id].packet_total; i++)
    {
      //read cur_item
      memset(&packet, 0XFF, sizeof(packet));
      if(i != p_view_info[table_id].packet_total - 1)
      {
        memcpy(packet.item,
               &p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
               sizeof(item_type_t) * VIEW_BUNDLE_SIZE);
      }
      else if(i == 0)
      {
        memcpy(packet.item,
               &p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
               sizeof(item_type_t) * (p_view_info[table_id].item_count));
      }
      else
      {
        memcpy(packet.item,
               &p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
               sizeof(item_type_t) * (p_view_info[table_id].item_count - i * VIEW_BUNDLE_SIZE));
      }
      for(j = 0; j < VIEW_BUNDLE_SIZE; j++)
      {
        if(IS_NULL_ITEM(packet.item[j]) == TRUE) //found finish
        {
          i = p_view_info[table_id].packet_total; //use break outside circle
          break;
        }
        cur_item = packet.item[j];
        //Fetch information in detail
        if(IS_ACTIVE(cur_item) != 0)
        {
          is_filter_item_all(p_data, cur_vv_id, cur_item,p_context,fav_count,p_cond_func);
        }
        else
        {
          MT_ASSERT(0); //can't exist deleted item
        }
      }

    }
  }
  else
  {
    memset(p_view_info[table_id].p_mem_backup, 0XFF, sizeof(item_type_t)* max_element_number);
    for(i = 0; i < p_view_info[table_id].packet_total; i++)
    {
      //read cur_item
      read_len = dm_read(p_dm_handle, p_view_info[table_id].block_id,
                         p_view_info[table_id].ni_start + i, 0,
                         sizeof(view_packet_t), (u8 *)&packet);
      MT_ASSERT(read_len != 0);
      for(j = 0; j < VIEW_BUNDLE_SIZE; j++)
      {
        if(IS_NULL_ITEM(packet.item[j]) == TRUE) //found finish
        {
          i = p_view_info[table_id].packet_total; //use break outside circle
          if(p_view_info[table_id].p_mem_backup != NULL)
          {
            /*memset(p_mem_backup + 2 * ((i > 0 ?i - 1:0) * VIEW_BUNDLE_SIZE + j),0xff,
              2 * (max_element_number - ((i > 0 ?i - 1:0) * VIEW_BUNDLE_SIZE + j)));*/
            p_view_info[table_id].sync_st = TRUE;
            p_view_info[table_id].item_count = (i > 0 ?i - 1:0) * VIEW_BUNDLE_SIZE + j;

          }
          break;
        }
        cur_item = packet.item[j];
        if(p_view_info[table_id].p_mem_backup != NULL)
          p_view_info[table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE + j] = packet.item[j];

        //Fetch information in detail
        if(IS_ACTIVE(cur_item) != 0)
        {
          is_filter_item_all(p_data, cur_vv_id, cur_item,p_context,fav_count,p_cond_func);
        }
        else
        {
          MT_ASSERT(0); //can't exist deleted item
        }
      }
    }
  }
}

/*!
   get virtual view information

   \param[in] vv_id the virtual view id which buffer created
   \return None
  */
void db_destroy_virtual_view_16v2(void *p_data, u8 vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_infor = p_db_var->vv_info;

  MT_ASSERT(p_vv_infor != NULL);
  
  /*!
   if vv_id >= MAX_VIRTUAL_VIEW_NUM, vv_id is invalid.
      vv_id == 0 means default view can't destroy
   */
  if(vv_id > 0 && vv_id <= MAX_VIRTUAL_VIEW_NUM) 
  {
    p_vv_infor[vv_id].cond_func = NULL;
    p_vv_infor[vv_id].p_mem = NULL;
  }
}


/*!
   get virtual view information.

   \param[in] vv_id the virtual view id
   \return current virtual view items count
  */
u16 db_get_virtual_view_count_16v2(void *p_data, u8 vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_infor = p_db_var->vv_info;

  return p_vv_infor[vv_id].vv_total;
}


/*!
   get real view count

   \param[in] vv_id the virtual view id
   \return current real view items count
  */
u16 db_get_real_view_count_16v2(void *p_data, u8 vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  DB_DBG("vv_total %d, pass_num %d\n",
          p_vv_info[vv_id].vv_total, p_vv_info[vv_id].pass_num);
  return p_vv_info[vv_id].vv_total + p_vv_info[vv_id].pass_num;
}

/*!
   get view delete flag

   \param[in] vv_id the virtual view id
   \return view delete flag
  */
BOOL db_get_view_del_flag_16v2(void *p_data, u8 vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_infor = p_db_var->vv_info;

  return p_vv_infor[vv_id].view_del_flag;
}

/*!
  return vv item number after cleanup.
  move delete item to p_mem end.
  */
static u16 cleanup_virtual_view(void *p_data, u8 vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  item_type_t *p_read = p_vv_info[vv_id].p_mem;
  item_type_t del_item = DB_BLK_DAFAULT_STAT;
  u16 del_cnt = 0;
  u16 actvie_cnt = 0;
  u32 i = 0;
  u16 index = 0;

  DB_DBG("vv_id %d, table_id %d, vv_total %d\n",
    vv_id, p_vv_info[vv_id].table_id, p_vv_info[vv_id].vv_total);
  for(i = 0; i < p_vv_info[vv_id].vv_total; i++)
  {
    //Reset index to avoid overlapping
    index = i - del_cnt;
    if(IS_NULL_ITEM(p_read[index]) == TRUE)
    {
      DB_DBG("null item : %x\n", p_read[index]);
      break;
    }

    if(IS_ACTIVE(p_read[index]) != 0)
    {
      actvie_cnt++;
    }
    else
    {
      del_item = p_read[index];
      memcpy(p_read + index, p_read + index + 1,
             sizeof(item_type_t) * (p_vv_info[vv_id].vv_total - i - 1));
      del_cnt++;
      p_vv_info[vv_id].p_mem[p_vv_info[vv_id].vv_total - del_cnt] = del_item;
    }
  }
  return actvie_cnt;
}


/*!
  Delete all item do not meet filter condition
  */
static void clear_item_from_packet(void *p_data,
                                   u8 vv_id,
                                   u16 r_node_inx,
                                   u8 r_pos,
                                   view_packet_t *p_packet_r)
{
  view_packet_t packet_w    = {0};
  db_var_t     *p_db_var    = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info   = p_db_var->vv_info;
  view_info_t  *p_view_info = p_db_var->view_info;
  void         *p_dm_handle = p_db_var->p_dm_handle;
  u16           w_node_inx  = r_node_inx;
  u32           node_inx_r  = r_node_inx; //Index of node to be cleared
  u8            w_pos       = r_pos;
  u32           node_pos_r  = r_pos;
  u8            table_id    = p_vv_info[vv_id].table_id;
  u8            block_id    = p_view_info[table_id].block_id;
  u16           pkt_total   = 0;

  //load current packet for looking up
  if(node_pos_r != 0)
  {
    //Process the half packet for processing
    dm_read(p_dm_handle, block_id, node_inx_r, 0, sizeof(view_packet_t),
            (u8 *)p_packet_r);

    packet_w.table_id = p_packet_r->table_id;
    packet_w.packet_idx = p_packet_r->packet_idx;
    packet_w.table_id = p_packet_r->table_id;
    memcpy(packet_w.item, p_packet_r->item, sizeof(item_type_t) * node_pos_r);

    memset(&(packet_w.item[w_pos]), 0XFF, sizeof(item_type_t) * (VIEW_BUNDLE_SIZE - w_pos));
  }

  //Load all of this table packet to packet_r one by one
  for(; node_inx_r < p_view_info[table_id].ni_start
       + p_view_info[table_id].packet_total; node_inx_r++)
  {
    if(node_pos_r == 0)
    {
      //Read next packet
      if(dm_read(p_dm_handle, block_id, node_inx_r, 0, sizeof(view_packet_t),
                 (u8 *)p_packet_r) == 0) //load finish
      {
        //packets before "g_view_info[table_id].packet_total" must be valid
        MT_ASSERT(0);
      }
    }

    //Packeting all not-delete item into a whole packet
    //look up items onr by one, delete filter items, copy other to packet_w
    for(; node_pos_r < VIEW_BUNDLE_SIZE; node_pos_r++)
    {
      if(IS_NULL_ITEM(p_packet_r->item[node_pos_r]) == TRUE)  //read done
      {
        //Set it enough high, break outside cycle
        node_inx_r = 0x1FFF;
        break;
      }

      if(is_filter_item(p_data, vv_id, p_packet_r->item[node_pos_r]) == FALSE)
      {
        if(w_pos == 0) //is a new write packet
        {
          //init new write packet
          packet_w.table_id = table_id;
          packet_w.packet_idx = w_node_inx - p_view_info[table_id].ni_start;
          memset(packet_w.item, 0XFF, sizeof(packet_w.item));
        }

        //write not filter item one by one
        packet_w.item[w_pos++] = p_packet_r->item[node_pos_r];

        //a write packet full , write down
        if(w_pos == VIEW_BUNDLE_SIZE)
        {
          dm_write_node(p_dm_handle, block_id, &w_node_inx, (u8 *)&packet_w,
                        sizeof(view_packet_t));
          dm_set_compress_unmatch_flag(p_dm_handle);
          //go to next packet
          w_node_inx++;
          w_pos = 0;
        }
      }
    }
    //next read packet
    node_pos_r = 0;
  }

  //the last packet don't write down
  if(w_pos != 0)
  {
    dm_write_node(p_dm_handle, block_id, &w_node_inx, (u8 *)&packet_w,
                  sizeof(view_packet_t));
    dm_set_compress_unmatch_flag(p_dm_handle);
    w_node_inx++;
    w_pos = 0;
  }

  //delete invaild packets
  pkt_total = p_view_info[table_id].packet_total;
  for(; w_node_inx < p_view_info[table_id].ni_start + pkt_total;
       w_node_inx++)
  {
    dm_del_node(p_dm_handle, block_id, w_node_inx, TRUE);
    p_view_info[table_id].packet_total--;
  }
}


/*!
   Find the last NULL item and add all new items from this position
  */
static void add_item_to_packet(void *p_data,
                               u8 vv_id,
                               u16 node_inx,
                               view_packet_t *p_packet,
                               item_type_t *p_new_item,
                               u16 add_num)
{
  u32 i = 0;
  db_var_t     *p_db_var    = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info   = p_db_var->vv_info;
  view_info_t  *p_view_info = p_db_var->view_info;
  void         *p_dm_handle = p_db_var->p_dm_handle;
  u8            table_id    = p_vv_info[vv_id].table_id;
  u16 new_pkt_index = 0;
  u16 pos = 0;
  u16 add_node_idx = node_inx;
  BOOL found_add_pos = FALSE;

  new_pkt_index = add_node_idx - p_view_info[table_id].ni_start;
  //found the first empty position
  for(; add_node_idx < p_db_var->max_view_node_num; add_node_idx++)
  {
    if(found_add_pos == TRUE)
    {
      break;
    }
    if(0 ==
       //load finish
       dm_read(p_dm_handle, p_view_info[table_id].block_id, add_node_idx, 0,
               sizeof(view_packet_t), (u8 *)p_packet))
    {
      //found a empty packet : found_add_pos = TRUE
      pos = 0;
      break;
    }
    else
    {
      MT_ASSERT((p_packet->table_id == table_id)
               && (p_packet->packet_idx == new_pkt_index));
      new_pkt_index++;
    }

    for(i = 0; i < VIEW_BUNDLE_SIZE; i++)  //found first empty in current packet
    {
      if(IS_NULL_ITEM(p_packet->item[i]) == TRUE)  //found
      {
        pos = i;
        found_add_pos = TRUE;
        break;
      }
    }
  }

  i = 0;
  //db_dbg(("DATA_BASE: add table_id %d\n", table_id));
  while((IS_NULL_ITEM(p_new_item[i]) == FALSE) && (add_num > 0))
  {
    if(pos == 0) //is a new packet
    {
      p_packet->table_id = table_id;  //init packet header
      p_packet->packet_idx = new_pkt_index++; //init packet header
      memset(p_packet->item, DB_BLK_DAFAULT_STAT, sizeof(p_packet->item));
      p_view_info[table_id].packet_total++;
    }

    p_packet->item[pos++] = p_new_item[i++];  //write item to packet one by one
    add_num--;
    //db_dbg(("pkt_idx %d, pos %d\n", p_packet->packet_idx, pos));

    if(pos == VIEW_BUNDLE_SIZE)  //a packet full , write down
    {
      add_node_idx = p_packet->packet_idx + p_view_info[table_id].ni_start;
      dm_write_node(p_dm_handle, p_view_info[table_id].block_id, &add_node_idx,
                    (u8 *)p_packet, sizeof(view_packet_t));
      dm_set_compress_unmatch_flag(p_dm_handle);

      pos = 0;
    }
  }

  if(pos != 0) //the last packet don't write down
  {
    add_node_idx = p_packet->packet_idx + p_view_info[table_id].ni_start;
    dm_write_node(p_dm_handle, p_view_info[table_id].block_id, &add_node_idx,
                  (u8 *)p_packet, sizeof(view_packet_t));
    dm_set_compress_unmatch_flag(p_dm_handle);
  }
}


u16 db_add_remove_item_16v2(void *p_data,u8 up_table_id,
  item_type_t mod_item,BOOL is_add)
{
  db_var_t    *p_db_var    = (db_var_t *)p_data;
  view_info_t *p_view_info = p_db_var->view_info;
  item_type_t *p_read      = p_view_info[up_table_id].p_mem_backup;
  item_type_t  cur_item    = NULL_ITEM;
  u16          ret         = 0xffff;
  u32          i           = 0;

  if(!is_add)
  {
    for(i = 0; i < p_view_info[up_table_id].item_count ; i++)
    {
      cur_item = p_read[i];
      if(ITEM_ID(cur_item) == mod_item)
      {
        memcpy(p_read + i, p_read + i + 1,
          sizeof(item_type_t) * (p_view_info[up_table_id].item_count - i - 1));
        p_read[p_view_info[up_table_id].item_count - 1] = NULL_ITEM;
        p_view_info[up_table_id].item_count --;
        ret = i;
        break;
      }
    }
  }
  else
  {
    p_read[p_view_info[up_table_id].item_count] = mod_item;
    p_view_info[up_table_id].item_count ++;
    ret = p_view_info[up_table_id].item_count;
  }

  return ret;
}

/*!
  dump all view item to flash from memory view
  */
void db_dump_total_item_16v2(void *p_data, u8 up_table_id, u16 min_index)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  view_info_t *p_view_info = p_db_var->view_info;
  void *p_dm_handle = p_db_var->p_dm_handle;
  view_packet_t packet;
  u32 pakcek_total = p_view_info[up_table_id].item_count % VIEW_BUNDLE_SIZE == 0 ?
    (p_view_info[up_table_id].item_count / VIEW_BUNDLE_SIZE):
    (p_view_info[up_table_id].item_count / VIEW_BUNDLE_SIZE + 1);
  u32 pakcek_min = min_index % VIEW_BUNDLE_SIZE == 0 ?
    (min_index / VIEW_BUNDLE_SIZE) : (min_index / VIEW_BUNDLE_SIZE + 1);
  u16 add_node_idx = 0;
  u32 i = 0;
  u16 pkt_total = 0;

  DB_DBG("up_table_id %d, min_index %d\n", up_table_id, min_index);

  if(pakcek_min == 0)
    pakcek_min = 1;

  /*!
    pakcek_total is current need's total packet to save item.
    pkt_total    is before has total packet.
    pakcek_min   is last min change packet.(needs update packet start point.)
   */
  if(pakcek_total == 0)
  {
   /*!
    1. pakcek_total == 0 : delete all view packet.
    */
    add_node_idx = p_view_info[up_table_id].ni_start;
    pkt_total = p_view_info[up_table_id].packet_total;
    for(; add_node_idx < p_view_info[up_table_id].ni_start + pkt_total; add_node_idx++)
    {
      dm_del_node(p_dm_handle, p_view_info[up_table_id].block_id, add_node_idx, TRUE);
      p_view_info[up_table_id].packet_total--;
    }
    DB_DBG("delete all view packet, %d\n", pakcek_total);
  }
  else
  {
   /*!
    need update view packet.
    */
    DB_DBG("update view pakcet. tbl_id %d, pkt_total %d\n", up_table_id, pakcek_total);

    for(i = 0; i < pakcek_total; i++)
    {
      //read cur_item
      memset(&packet, 0xFF, sizeof(packet));
      packet.table_id = up_table_id;
      packet.packet_idx = i;



      if(i != pakcek_total - 1)
      {
       /*!
        pakcek_total > 1, and not last one.
        */
        memcpy(packet.item,
               &p_view_info[up_table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
               sizeof(item_type_t) * VIEW_BUNDLE_SIZE);
        DB_DBG("table_id %d, packet_idx %d, reserved %d\n",
        packet.table_id, packet.packet_idx, packet.reserved);
      }
      else if(i == 0)
      {
       /*!
        only have one packet, packet_total = 1.
        */
        memcpy(packet.item,
               &p_view_info[up_table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
               sizeof(item_type_t) * (p_view_info[up_table_id].item_count));

        DB_DBG("table_id %d, packet_idx %d, reserved %d\n",
                packet.table_id, packet.packet_idx, packet.reserved);
      }
      else
      {
        DB_DBG("table_id %d, packet_idx %d, reserved %d, item_cout %d, i %d, rev_cout %d\n",
          packet.table_id, packet.packet_idx, packet.reserved,
          p_view_info[up_table_id].item_count, i, 
          p_view_info[up_table_id].item_count - i * VIEW_BUNDLE_SIZE);
        
        memcpy(packet.item,
               &p_view_info[up_table_id].p_mem_backup[i * VIEW_BUNDLE_SIZE],
               sizeof(item_type_t) * (p_view_info[up_table_id].item_count - i * VIEW_BUNDLE_SIZE));
      }

      add_node_idx = packet.packet_idx + p_view_info[up_table_id].ni_start;
      DB_DBG("add_node_idx %d\n", add_node_idx);

      if(packet.packet_idx >= pakcek_min - 1)
      {
        dm_write_node(p_dm_handle, p_view_info[up_table_id].block_id,
          &add_node_idx, (u8 *)&packet,sizeof(view_packet_t));
      }

      add_node_idx++;
    }

   /*!
    pkt_total is before has total packet.
    add_node_idx is the active packet end point.
    */
    pkt_total = p_view_info[up_table_id].packet_total;
    for(; add_node_idx < p_view_info[up_table_id].ni_start + pkt_total;
         add_node_idx++)
    {
      dm_del_node(p_dm_handle, p_view_info[up_table_id].block_id, add_node_idx, TRUE);
      p_view_info[up_table_id].packet_total--;
    }

    if(pakcek_total > p_view_info[up_table_id].packet_total)
      p_view_info[up_table_id].packet_total = pakcek_total;
  }
}

/*!
   fine item index in memory view
  */
u16 db_find_item_index_16v2(void *p_data,u8 up_table_id,
  item_type_t mod_item)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  view_info_t *p_view_info = p_db_var->view_info;
  u32 i = 0;
  item_type_t *p_read = p_view_info[up_table_id].p_mem_backup;
  item_type_t cur_item = NULL_ITEM;
  u16 ret = 0xffff;

  DB_DBG("mod_item %d\n",mod_item);

  for(i = 0; i < p_view_info[up_table_id].item_count ; i++)
  {
    if(IS_ACTIVE(p_read[i]) != 0)
    {
      cur_item = p_read[i];
      if(ITEM_ID(cur_item) == mod_item)
      {
        ret = i;
        DB_DBG("find index %d\n", ret);
        return ret;
      }
    }
  }
  DB_DBG("not find it %d\n", ret);
  return ret;
}

/*!
   set item data of item_index in memory view
  */
void db_set_item_index_16v2(void *p_data,u8 up_table_id,
  item_type_t mod_item,u16 item_index)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  view_info_t *p_view_info = p_db_var->view_info;
  item_type_t *p_read = p_view_info[up_table_id].p_mem_backup;

  DB_DBG("view item %d, index %d, mod_item %d\n", p_read[item_index], item_index, mod_item);
  if(IS_ACTIVE(p_read[item_index]) != 0)
  {
    p_read[item_index] = mod_item;
  }
}

static void db_shellsort_16v2(u16 *p_sort, u16 item_cout)
{
  u16 i = 0;
  u16 j = 0;
  u16 incre = 0;
  u16 tmp = 0;

  MT_ASSERT(p_sort != NULL);

  for(incre = item_cout / 2; incre > 0; incre /= 2)
  {
    for(i = incre; i < item_cout; i++)
    {
      tmp = *(p_sort + i);

      for(j = i; j >= incre; j -= incre)
      {
        if(tmp < *(p_sort + (j - incre)))
          *(p_sort + j) = *(p_sort + (j - incre));
        else
          break;
      }

      *(p_sort + j) = tmp;

    }
  }
}


static void db_sync_new_item_to_dmup_buf(void *p_data, u8 vv_id)
{
  db_var_t     *p_db_var  = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  item_type_t  *p_read    = p_vv_info[vv_id].p_mem;
  u32           i         = 0;

  for(i = p_vv_info[vv_id].create_num; i < p_vv_info[vv_id].vv_total; i++)
  {
    DB_DBG("vv_id %d, item %x\n", vv_id, p_read[i]);
    db_add_remove_item_16v2(p_data,p_vv_info[vv_id].table_id, p_read[i],TRUE);
  }
}


/*!
   update virtual view to real virtual and
   store the real virtual to flash

   \param[in] vv_id the virtual view id which buffer created
   \return operation Success or not . TRUE is SUC.
  */
BOOL db_update_view_16v2(void *p_data, u8 vv_id)
{
  //all of number in virtual view list
  //after up all packet, if the num less than vv_item_total,
  //it's means some new items aren't updated it flash
  u16 vv_item_total = 0;
  u16 up_item_num = 0;
  u8 up_table_id = 0;
  //read packet's index
  u32 pkt_inx = 0;
  //offset inside packet
  u32 pos = 0;
  view_packet_t packet = {0};
  BOOL need_rewrite = FALSE;
  BOOL look_up_done = FALSE;
  item_type_t cur_item = 0;
  u16 node_id = 0;
  u32 i = 0;
  u32 inval_item_cnt = 0;
  u32 flash_chk_cnt = 0;
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  view_info_t *p_view_info = p_db_var->view_info;
  void *p_dm_handle = p_db_var->p_dm_handle;
  item_type_t *p_read = p_vv_info[vv_id].p_mem;
  item_type_t mod_item = DB_BLK_DAFAULT_STAT;
  u16 *p_item_buf = NULL;
  u16 min_index = 0xffff;
  u16 ret_index = 0;
  u16 vv_total_old = 0;

  MT_ASSERT(p_dm_handle != NULL);

  DB_DBG("vv_id %d, table_id %d\n", vv_id, p_vv_info[vv_id].table_id);

  //didn't create
  if(p_vv_info[vv_id].p_mem == NULL
    || p_vv_info[vv_id].view_need_up == FALSE)
  {
    DB_DBG("need't up\n");
    return FALSE;
  }

  up_table_id = p_vv_info[vv_id].table_id;
  node_id     = p_view_info[up_table_id].ni_start;
  min_index   = p_vv_info[vv_id].create_num;

  /*!
   the first : sync add item to view.p_mem_backup.
   */
  db_sync_new_item_to_dmup_buf(p_data, vv_id);

  /*!
   the second : move all delete item to vv_info.p_mem end.
   */
  vv_item_total = cleanup_virtual_view(p_data, vv_id);
  MT_ASSERT(vv_item_total == p_vv_info[vv_id].active_num);

  if(p_vv_info[vv_id].plus_only == TRUE)
  {
    //only plus, no need check packet
    pkt_inx = p_view_info[up_table_id].packet_total;
    up_item_num = p_vv_info[vv_id].create_num;
  }

  if(p_view_info[up_table_id].p_mem_backup != NULL &&
    p_view_info[up_table_id].sync_st == TRUE)
  {
    vv_total_old = p_vv_info[vv_id].vv_total;
    /*!
      the third : vv_item_total = active_num.
      if active_num < vv_total, deal with delete item.
      */
    if(vv_item_total < p_vv_info[vv_id].vv_total)
    {
      DB_DBG("vv_id %d, have del node\n", vv_id);
      p_vv_info[vv_id].view_del_flag = TRUE;
      for(i = 0; i < p_vv_info[vv_id].vv_total ; i++)
      {
        p_read = p_vv_info[vv_id].p_mem;
        if(IS_NULL_ITEM(p_read[i]) == TRUE)
        {
          break;
        }

        /*!
          if inactive, delete element and remove item to view.p_mem_backup.
          */
        if(IS_ACTIVE(p_read[i]) == 0)
        {
          mod_item = p_read[i];
          ret_index = db_add_remove_item_16v2(p_data,up_table_id, ITEM_ID(mod_item),FALSE);

          if(ret_index < min_index)
            min_index = ret_index;
          db_del_element_16v2(p_data, p_vv_info[vv_id].table_id, ITEM_ID(p_read[i]));
          p_read[i] = NULL_ITEM;
        }
      }
      p_vv_info[vv_id].vv_total = vv_item_total;
    }

    /*!
      the fourth : vv_item_total = active_num,
      if active_num == vv_total, haven't delete item.
      */
    if(vv_item_total == p_vv_info[vv_id].vv_total)
    {
      if(vv_item_total > 0)
      {
        DB_DBG("vv_id %d, have change\n", vv_id);
        p_item_buf = mtos_malloc(p_vv_info[vv_id].vv_total * sizeof(u16));
        MT_ASSERT(p_item_buf != NULL);
        /*!
         1. create vv_info :
            view.p_mem :        vv_info.p_mem :
            index item          index item
              0    A              0    A
              1    B              2    C
              2    C              4    E
              3    D
              4    E

         2. user change item order : (vv_info.p_mem have changed)
            view.p_mem :        vv_info.p_mem :
            index item          index item
              0    A              2    C
              1    B              4    E
              2    C              0    A
              3    D
              4    E

         3. must sort vv_info.p_mem :
            index item
              0    C
              2    E
              4    A

         4. update vv_info.p_mem to view.p_mem :
            index item
              0    C
              1    B
              2    E
              3    D
              4    A
          */
        /*!
         find vv_info's item in the view_info index.
         */
        for(i = 0; i < p_vv_info[vv_id].vv_total ; i++)
        {
          p_read = p_vv_info[vv_id].p_mem;
          if(IS_ACTIVE(p_read[i]) != 0)
          {
            cur_item = p_read[i];
            *(p_item_buf + i) = db_find_item_index_16v2(p_data,up_table_id, ITEM_ID(cur_item));
          }
        }

        /*!
         Shellsort from small to large.
         */
        db_shellsort_16v2(p_item_buf, p_vv_info[vv_id].vv_total);
        //min_index = *p_item_buf;
        if(min_index > (*p_item_buf))
        {
            min_index = *p_item_buf;
        }
        /*!
         dump to view_info.
         */
        for(i = 0; i < p_vv_info[vv_id].vv_total ; i++)
        {
          p_read = p_vv_info[vv_id].p_mem;
          if(IS_ACTIVE(p_read[i]) != 0)
          {
            cur_item = p_read[i];
            db_set_item_index_16v2(p_data, up_table_id, cur_item, *(p_item_buf + i));
          }
        }
        mtos_free(p_item_buf);
      }
    }

    db_dump_total_item_16v2(p_data, up_table_id, min_index);
  }
  else
  {
    //read view packet one by one
    for(; pkt_inx < p_view_info[up_table_id].packet_total; pkt_inx++)
    {
      node_id = p_view_info[up_table_id].ni_start + pkt_inx;
      dm_read(p_dm_handle, p_view_info[up_table_id].block_id, node_id, 0,
              sizeof(view_packet_t), (u8 *)(&packet));
      MT_ASSERT(packet.table_id == up_table_id);
      MT_ASSERT(packet.packet_idx == pkt_inx);
      //reset rewrite flag
      need_rewrite = FALSE;

      //compare items in this packet one by one
      for(pos = 0; pos < VIEW_BUNDLE_SIZE; pos++)
      {
        // if look up done , it depend on two case:
        //        1: look up to end, 2, update all of item in virtual view list

        // 1.(up_item_num == vv.create_num) && (up_item_num < vv_item_total)
        // it means some item is new, rewrite all of items in flash. but some
        // new items in
        // virtual view list without update, need add to flash at end of packets

        // 2.(up_item_num == vv_item_total) && (up_item_num < vv.create_num)
        //  it means some item was deleted, update all of items in virtual view
        // list to flash, and need delete redundant items in flash which must
        // be after the point.

        //found end
        if(IS_NULL_ITEM(packet.item[pos])
          ||(up_item_num == p_vv_info[vv_id].create_num)
          || (up_item_num == vv_item_total))
        {
          look_up_done = TRUE;
          break;
        }

        //check two side :
        // 1. is create to virtual view?
        // 2. is changed in the position?
        cur_item = packet.item[pos];
        //don't exist deleted items,

        if(is_filter_item(p_data, vv_id, cur_item) == TRUE)
        {
          if(p_vv_info[vv_id].p_mem[up_item_num] != cur_item)  //changed
          {
            packet.item[pos] = p_vv_info[vv_id].p_mem[up_item_num];
            need_rewrite = TRUE;
          }
          up_item_num++;
        }
        else if(p_vv_info[vv_id].bit_only == FALSE)
        {
          for(flash_chk_cnt = 0 ; flash_chk_cnt + inval_item_cnt < vv_item_total; flash_chk_cnt ++)
          {
            //This item is removed from flash but may existing in virtual view
            if(p_vv_info[vv_id].p_mem[flash_chk_cnt] == cur_item)
            {
              memcpy(&p_vv_info[vv_id].p_mem[flash_chk_cnt],
                    &p_vv_info[vv_id].p_mem[flash_chk_cnt + 1],
                    sizeof(item_type_t) * (vv_item_total - flash_chk_cnt));
              inval_item_cnt ++;
              break;
            }
          }
        }
      }
      //check a packet done .
      if(need_rewrite == TRUE)
      {
        dm_write_node(p_dm_handle, p_view_info[up_table_id].block_id,
                      &node_id, (u8 *)(&packet), sizeof(view_packet_t));
        dm_flush_cache(p_dm_handle, p_view_info[up_table_id].block_id, node_id);
        need_rewrite = FALSE;
        dm_set_compress_unmatch_flag(p_dm_handle);
      }

      if(look_up_done == TRUE)
      {
        break;
      }
    }

    //some item was deleted, need delete them from flash
    if(up_item_num < p_vv_info[vv_id].create_num)
    {
      // the update number must equal virtual view item number
      p_vv_info[vv_id].view_del_flag = TRUE;
      if(up_item_num == vv_item_total)
      {
        clear_item_from_packet(p_data, vv_id, node_id, pos, &packet);
      }
      else
      {
        //Data in flash is not sync with view
        MT_ASSERT(p_vv_info[vv_id].bit_only == FALSE);
        //Update view status by flash data
        if(inval_item_cnt > 0)
        {
          p_vv_info[vv_id].view_need_up = FALSE;
          p_vv_info[vv_id].plus_only = TRUE;
          p_vv_info[vv_id].create_num = vv_item_total - inval_item_cnt;
          p_vv_info[vv_id].vv_total = vv_item_total - inval_item_cnt;
          return TRUE;
        }
      }
    }

    MT_ASSERT(p_vv_info[vv_id].vv_total >= 1);

    //delete items from table
    for(i = p_vv_info[vv_id].vv_total - 1; i >= vv_item_total; i--)
    {
      db_del_element_16v2(p_data, p_vv_info[vv_id].table_id,
                          ITEM_ID(p_vv_info[vv_id].p_mem[i]));
      p_vv_info[vv_id].p_mem[i] = NULL_ITEM;
      if(i == 0){break; }
    }

    //item in packets was checked done, but some items in virtual
    //view list don't sync to flash, so need add it
    if(up_item_num < vv_item_total)
    {
      // the update number must equal created number
      if(up_item_num == p_vv_info[vv_id].create_num)
      {
       add_item_to_packet(p_data,
                          vv_id,
                          node_id,
                          &packet,
                          p_vv_info[vv_id].p_mem + up_item_num,
                          vv_item_total - up_item_num);
      }
    }
  }

  p_vv_info[vv_id].view_need_up = FALSE;
  p_vv_info[vv_id].plus_only = TRUE;
  p_vv_info[vv_id].create_num = vv_item_total;
  p_vv_info[vv_id].vv_total = vv_item_total;
  MT_ASSERT(p_vv_info[vv_id].vv_total == p_vv_info[vv_id].active_num);
  return TRUE;
}


/*!
   Update all the element in the flash.
  */
BOOL db_update_element_16v2(void *p_data, u8 vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;

  //u8 item_cnt = 0;
  u32 item_cnt = 0;
  u8 blk_id = p_table_info[p_vv_info[vv_id].table_id].block_id;
  u16 element_id = 0;

  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(p_dm_handle != NULL);

  DB_DBG("vv_id %d, table_id %d\n", vv_id, p_vv_info[vv_id].table_id);

  //Make sure that elements in view needs updating
  if(p_vv_info[vv_id].element_need_up == FALSE
    || p_vv_info[vv_id].p_mem == NULL)
  {
    DB_DBG("needn't up\n");
    return FALSE;
  }

  for(item_cnt = 0; item_cnt < p_vv_info[vv_id].active_num; item_cnt++)
  {
    element_id = ITEM_ID(p_vv_info[vv_id].p_mem[item_cnt]);
    dm_flush_cache(p_dm_handle, blk_id, element_id);
  }

  p_vv_info[vv_id].element_need_up = FALSE;
  return TRUE;
}


/*!
   add a view item for element

   \param[in] vv_id which virtual view
   \param[in] pbuffer data's addr
   \param[in] len data's length
   \param[out] p_pos new item's position
   \return operation result
  */
db_ret_t db_add_view_item_16v2(void *p_data,
                               u8 vv_id,
                               u8 *p_buffer,
                               u16 len,
                               u16 *p_pos)
{
  u8            table_id     = 0;
  u16           element_id   = DB_UNKNOWN_ID;
  u16           add_pos      = 0;
  db_ret_t      ret          = DB_FAIL;
  db_var_t     *p_db_var     = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info    = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;

  if(db_get_real_view_count_16v2(p_data, vv_id) >=
     p_table_info[p_vv_info[vv_id].table_id].max_element_number)
  {
    *p_pos = DB_UNKNOWN_ID;
    return DB_FULL;
  }

  *p_pos   = 0xFFFF;
  table_id = p_vv_info[vv_id].table_id;
  add_pos  = p_vv_info[vv_id].vv_total;

  DB_DBG("vv_id %d, table_id %d, vv_total %d, pass_num %d, max_ele %d\n",
    vv_id, p_vv_info[vv_id].table_id,
    p_vv_info[vv_id].vv_total, p_vv_info[vv_id].pass_num,
    p_table_info[p_vv_info[vv_id].table_id].max_element_number);

  //Write table data.
  ret = db_add_element_16v2(p_data, table_id, &element_id, p_buffer, len);

  //add item to virtual view mem.
  if(ret == DB_SUC)
  {
    //set element id and active status
    p_vv_info[vv_id].p_mem[add_pos] = element_id;
    SYS_SET_BIT(p_vv_info[vv_id].p_mem[add_pos], ACTIVE_BIT_16V2);

    //g_vv_info[vv_id].mem[add_pos] = SYS_SET_BIT(element_id, ACTIVE_BIT_16V2);
    p_vv_info[vv_id].vv_total++;
    *p_pos = add_pos;

    p_vv_info[vv_id].active_num++;
    p_vv_info[vv_id].view_need_up = TRUE;

    DB_DBG("add_pos %d, p_mem %d, vv_total %d, active_num %d\n",
      add_pos, p_vv_info[vv_id].p_mem[add_pos],
      p_vv_info[vv_id].vv_total, p_vv_info[vv_id].active_num);
  }
  return ret;
}


/*!
   edit a item

   \param[in] vv_id which virtual view
   \param[in] pos which item will be edit
   \param[in] pbuffer element's addr
   \param[in] len element's length
   \return operation Success or not . TRUE is SUC.
  */
BOOL db_edit_view_item_16v2(void *p_data,
                            u8 vv_id,
                            u16 pos,
                            u8 *p_buffer,
                            u16 len)
{
  u8            table_id     = 0;
  u16           element_id   = DB_UNKNOWN_ID;
  db_var_t     *p_db_var     = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info    = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;
  void         *p_dm_handle  = p_db_var->p_dm_handle;

  if(check_parameter(p_data, vv_id, pos, 0, FALSE) == FALSE)
  {
    return DB_FAIL;
  }

  table_id = p_vv_info[vv_id].table_id;
  element_id = ITEM_ID(p_vv_info[vv_id].p_mem[pos]);

  DB_DBG("vv_id %d, table_id %d\n", vv_id, p_vv_info[vv_id].table_id);

  return dm_write_node(p_dm_handle, p_table_info[table_id].block_id,
                        &element_id, p_buffer, len) == DM_SUC;
}

/*!
  save view edit
  */
void db_save_view_edit_16v2(void *p_data, u8 vv_id, u16 view_pos)
{
  u8            table_id     = 0;
  u16           element_id   = DB_UNKNOWN_ID;
  db_var_t     *p_db_var     = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info    = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;
  void         *p_dm_handle  = p_db_var->p_dm_handle;

  table_id   = p_vv_info[vv_id].table_id;
  element_id = ITEM_ID(p_vv_info[vv_id].p_mem[view_pos]);

  DB_DBG("vv_id %d, table_id %d\n", vv_id, p_vv_info[vv_id].table_id);

  //Flush data cache
  dm_flush_cache(p_dm_handle, p_table_info[table_id].block_id, element_id);
}

/*!
   edit a item

   \param[in] vv_id which virtual view
   \param[in] pos which item will be edit
   \param[in] offset read offset for first addr
   \param[in] len read length
   \param[out] pbuffer data's addr
   \return operation Success or not . TRUE is SUC.
  */
BOOL db_get_view_item_16v2(void *p_data,
                           u8 vv_id,
                           u16 pos,
                           u16 offset,
                           u16 len,
                           u8 *p_buffer)
{
  u8            table_id     = 0;
  u16           element_id   = DB_UNKNOWN_ID;
  db_var_t     *p_db_var     = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info    = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;
  void         *p_dm_handle  = p_db_var->p_dm_handle;

  if(check_parameter(p_data, vv_id, pos, 0, FALSE) == FALSE)
  {
    return FALSE;
  }

  table_id = p_vv_info[vv_id].table_id;
  if(IS_NULL_ITEM(p_vv_info[vv_id].p_mem[pos]) == TRUE)
  {
    return FALSE;
  }
  element_id = ITEM_ID(p_vv_info[vv_id].p_mem[pos]);

  return dm_read(p_dm_handle, p_table_info[table_id].block_id,
                 element_id, offset, len, p_buffer) > 0;
}


/*!
   delete a view item

   \param[in] vv_id which virtual view
   \param[in] pos the position of delete item
   \return None
  */
void db_del_view_item_16v2(void *p_data, u8 vv_id, u16 pos)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  if(check_parameter(p_data, vv_id, pos, 0, TRUE) == FALSE)
  {
    return;
  }

  if((p_vv_info[vv_id].active_num > 0)
    && IS_ACTIVE(p_vv_info[vv_id].p_mem[pos]))
  {
    p_vv_info[vv_id].view_need_up = TRUE;
    SYS_CLR_BIT(p_vv_info[vv_id].p_mem[pos], ACTIVE_BIT_16V2);
    p_vv_info[vv_id].active_num--;
  }
}


/*!
   delete all of view

   \param[in] vv_id which virtual view
   \return None
  */
void db_clear_view_item_16v2(void *p_data, u8 vv_id)
{
  u32 i = 0;
  item_type_t *p_item = NULL;
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;

  p_item = p_vv_info[vv_id].p_mem;

  DB_DBG("vv_id %d, table_id %d\n", vv_id, p_vv_info[vv_id].table_id);
  for(i = 0; i < p_table_info[p_vv_info[vv_id].table_id].max_element_number;
      i++)
  {
    if(IS_NULL_ITEM(p_item[i]) == TRUE) //found end
    {
      break;
    }
    if(IS_ACTIVE(p_item[i]) != 0)
    {
      db_del_view_item_16v2(p_data, vv_id, i);
    }
  }

  p_vv_info[vv_id].view_need_up = TRUE;
}


/*!
   active a view item

   \param[in] vv_id which virtual view
   \param[in] pos the position of active item
   \return None
  */
void db_active_view_item_16v2(void *p_data, u8 vv_id, u16 pos)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  DB_DBG("vv_id %d, table_id %d\n", vv_id, p_vv_info[vv_id].table_id);

  if(check_parameter(p_data, vv_id, pos, 0, TRUE) == FALSE)
  {
    DB_DBG("check para fail\n");
    return;
  }

  if(IS_ACTIVE(p_vv_info[vv_id].p_mem[pos]) == 0)
  {
    SYS_SET_BIT(p_vv_info[vv_id].p_mem[pos], ACTIVE_BIT_16V2);
    p_vv_info[vv_id].view_need_up = TRUE;
    p_vv_info[vv_id].active_num++;
  }
}


/*!
   edit a view item

   \param[in] vv_id which virtual view
   \param[in] pos the position of edit item
   \param[in] mask_shift which one mask bit, from 0 to 1
   \param[in] flag set value
   \return None
  */
void db_set_mask_view_item_16v2(void *p_data,
                                u8 vv_id,
                                u16 pos,
                                u8 mask_shift,
                                BOOL flag)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  if(check_parameter(p_data, vv_id, pos, mask_shift, TRUE) == FALSE)
  {
    return;
  }

  DB_DBG("vv_id %d, table_id %d, mask_shift %d\n", vv_id, p_vv_info[vv_id].table_id, mask_shift);

  if(flag == TRUE)
  {
    SYS_SET_BIT(p_vv_info[vv_id].p_mem[pos], (mask_shift + MARK_SHIFT_BIT));
  }
  else
  {
    SYS_CLR_BIT(p_vv_info[vv_id].p_mem[pos], (mask_shift + MARK_SHIFT_BIT));
  }

  p_vv_info[vv_id].view_need_up = TRUE;
}


/*!
   edit a view item

   \param[in] vv_id which virtual view
   \param[in] pos the position of edit item
   \param[in] mask_shift which one mask bit, from 0 to 3
   \return ret
  */
BOOL db_get_mask_view_item_16v2(void *p_data, u8 vv_id, u16 pos, u8 mask_shift)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  if(check_parameter(p_data, vv_id, pos, mask_shift, FALSE) == FALSE)
  {
    return FALSE;
  }

  return SYS_GET_BIT(p_vv_info[vv_id].p_mem[pos],
                     (mask_shift + MARK_SHIFT_BIT));
}


/*!
   exchange two items

   \param[in] vv_id which virtual view
   \param[in] pos_l
   \param[in] pos_r
   \return None
  */
void db_exchange_view_item_16v2(void *p_data, u8 vv_id, u16 pos_l, u16 pos_r)
{
  item_type_t temp = 0;
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  if(check_parameter(p_data, vv_id, pos_l, 0, TRUE) == FALSE
    || check_parameter(p_data, vv_id, pos_r, 0, TRUE) == FALSE)
  {
    return;
  }

  DB_DBG("vv_id %d, table_id %d, mask_shift %d\n", vv_id, p_vv_info[vv_id].table_id);

  if(pos_l != pos_r)
  {
    temp = p_vv_info[vv_id].p_mem[pos_l];
    p_vv_info[vv_id].p_mem[pos_l] = p_vv_info[vv_id].p_mem[pos_r];
    p_vv_info[vv_id].p_mem[pos_r] = temp;
    p_vv_info[vv_id].view_need_up = TRUE;
  }
}

/*!
   dump items

   \param[in] vv_id which virtual view
   \param[in] p_mem
   \param[in] len
   \return None
  */
void db_dump_view_item_16v2(void *p_data, u8 vv_id, void *p_mem, u16 len)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  u32 i = 0;

  DB_DBG("vv_id %d, table_id %d, mask_shift %d\n", vv_id, p_vv_info[vv_id].table_id);
  for(i = 0; i < len ; i++)
    p_vv_info[vv_id].p_mem[i] = *((u16 *)p_mem + i);

  p_vv_info[vv_id].plus_only = FALSE;
  p_vv_info[vv_id].view_need_up = TRUE;
}


/*!
   move a view item form current position to target,
   and the target be after "pos_to" item

   \param[in] pos_form
   \param[in] pos_to
   \return None
  */
void db_move_view_item_16v2(void *p_data, u8 vv_id, u16 pos_from, u16 pos_to)
{
  item_type_t *p_item = NULL;
  item_type_t temp = 0;
  u32 i = 0;

  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  if(check_parameter(p_data, vv_id, pos_from, 0, TRUE) == FALSE
    || check_parameter(p_data, vv_id, pos_to, 0, TRUE) == FALSE)
  {
    return;
  }

  DB_DBG("vv_id %d, table_id %d, mask_shift %d\n", vv_id, p_vv_info[vv_id].table_id);

  p_item = p_vv_info[vv_id].p_mem;
  temp = p_item[pos_from];

  p_vv_info[vv_id].view_need_up = TRUE;
  if(pos_from > pos_to)
  {
    //this function is error when copy form lower to higher
    //memcpy(item+pos_to+1, item+pos_to, (pos_from-pos_to)*sizeof(item_type_t));
    for(i = pos_from; i > pos_to; i--)
    {
      p_item[i] = p_item[i - 1];
    }
  }
  else if(pos_from < pos_to)
  {
    memcpy(p_item + pos_from, p_item + pos_from + 1,
           (pos_to - pos_from) * sizeof(item_type_t));
  }

  p_item[pos_to] = temp;
}


u16 db_get_element_id_by_pos_16v2(void *p_data, u8 vv_id, u16 pos)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  if(vv_id >= MAX_VIRTUAL_VIEW_NUM
    || pos == DB_UNKNOWN_ID
    || !check_parameter(p_data, vv_id, pos, 0, FALSE))
  {
    return DB_UNKNOWN_ID;
  }

  DB_DBG("vv_id %d, table_id %d, pos %d\n", vv_id, p_vv_info[vv_id].table_id, pos);

  return ITEM_ID(p_vv_info[vv_id].p_mem[pos]);
}

void db_set_max_view_node_num_16v2(void *p_data, u32 max_num)
{
  db_var_t *p_db_var = (db_var_t *)p_data;

  p_db_var->max_view_node_num = max_num;
}


u32 db_get_max_view_node_num_16v2(void *p_data)
{
  db_var_t *p_db_var = (db_var_t *)p_data;

  return p_db_var->max_view_node_num;
}

void db_clean_16v2(void *p_data)
{
  u32 table_id = 0;
  u32 i = 0;
  u32 j = 0;
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  view_info_t *p_view_info = p_db_var->view_info;
  table_info_t *p_table_info = p_db_var->table_info;

  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(p_dm_handle != NULL);

  /*!
   *delete view node
   */
  for(table_id = 0; table_id < MAX_TABLE_NUM; table_id++)
  {
    for(j = 0; j < p_view_info[table_id].packet_total; j++)
    {
      dm_del_node(p_dm_handle, p_view_info[table_id].block_id,
                  p_view_info[table_id].ni_start + j, TRUE);
    }
    p_view_info[table_id].packet_total = 0;
    p_view_info[table_id].sync_st = FALSE;
    p_view_info[table_id].item_count = 0;
  }

  for(i = 0; i < MAX_VIRTUAL_VIEW_NUM; i++)
  {
    p_vv_info[i].p_mem = NULL;
    p_vv_info[i].view_need_up = FALSE;
    p_vv_info[i].plus_only = TRUE;
    p_vv_info[i].create_num = 0;
    p_vv_info[i].vv_total = 0;
  }

  dm_reset_iw_block(p_dm_handle, p_table_info[0].block_id, TRUE);
}


/*!
   The memory length of input memory address should be allocated at least
   MAX_VIRTUAL_VIEW_NUM
  */
static u16 db_create_sub_virtual_view_16v2(void *p_data,
                                           db_filter_proc_t p_cond_func,
                                           BOOL bit_only,
                                           u8 *p_mem,
                                           u32 context,
                                           u8 *p_sub_vv_id,
                                           u8 parent_vv_id)
{
  u32 vv_cnt = 0;
  u8 sub_vv_id = 0;
  u32 max_item_num = 0;
  u32 item_cnt = 0;

  u16 active_sub_vv_num = 0;
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  MT_ASSERT(p_mem != NULL);

  //Fix me check memory size or memset in this API will overwrite memory address
  //Find empty virtual view as new sub-virtual view
  for(vv_cnt = 0; vv_cnt < MAX_VIRTUAL_VIEW_NUM; vv_cnt++)
  {
    if(p_vv_info[vv_cnt].p_mem == NULL)
    {
      //The sub-virtual view id
      sub_vv_id = vv_cnt;
      break;
    }
  }

  MT_ASSERT(vv_cnt < MAX_VIRTUAL_VIEW_NUM);
  *p_sub_vv_id = sub_vv_id;

  //Set basic parameter for sub-virtual view
  p_vv_info[sub_vv_id].bit_only = bit_only;
  p_vv_info[sub_vv_id].context = context;
  p_vv_info[sub_vv_id].p_mem = (item_type_t *)p_mem;
  p_vv_info[sub_vv_id].table_id = p_vv_info[parent_vv_id].table_id;

  p_vv_info[sub_vv_id].view_need_up = FALSE;
  p_vv_info[sub_vv_id].element_need_up = FALSE;
  p_vv_info[sub_vv_id].plus_only = TRUE;
  p_vv_info[sub_vv_id].cond_func = p_cond_func;
  p_vv_info[sub_vv_id].pass_num = 0;

  //Match parent virtual view id
  MT_ASSERT(parent_vv_id < MAX_VIRTUAL_VIEW_NUM);
  MT_ASSERT(p_vv_info[parent_vv_id].p_mem != p_vv_info[sub_vv_id].p_mem);

  max_item_num = p_vv_info[parent_vv_id].active_num;
  memset(p_vv_info[sub_vv_id].p_mem, DB_BLK_DAFAULT_STAT,
         max_item_num * sizeof(item_type_t));

  //Filter new
  for(item_cnt = 0; item_cnt < max_item_num; item_cnt++)
  {
    if(IS_NULL_ITEM(p_vv_info[parent_vv_id].p_mem[item_cnt]) == TRUE)
    {
      //NULL item and system ends
      break;
    }

    if(IS_ACTIVE(p_vv_info[parent_vv_id].p_mem[item_cnt]) != 0)
    {
      if(is_filter_item(p_data, sub_vv_id,
                        p_vv_info[parent_vv_id].p_mem[item_cnt]) == TRUE)
      {
        p_vv_info[sub_vv_id].p_mem[active_sub_vv_num]
          = p_vv_info[parent_vv_id].p_mem[item_cnt];
        active_sub_vv_num++;
      }
      else
      {
        p_vv_info[sub_vv_id].pass_num++;
      }
    }
    else
    {
      continue;
    }
  }

  p_vv_info[sub_vv_id].vv_total = active_sub_vv_num;
  p_vv_info[sub_vv_id].active_num = active_sub_vv_num;
  p_vv_info[sub_vv_id].create_num = active_sub_vv_num;

  return active_sub_vv_num;
}


void db_destroy_sub_virtual_view_16v2(void *p_data, u8 sub_vv_id)
{
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;

  //vv_id == 0 means default view can't destroy
  if(sub_vv_id > 0
    && sub_vv_id < MAX_VIRTUAL_VIEW_NUM)
  {
    p_vv_info[sub_vv_id].cond_func = NULL;
    p_vv_info[sub_vv_id].p_mem = NULL;
  }
}


void db_undo_view_16v2(void *p_data, u8 vv_id)
{
  //read packet's index
  u32 pkt_inx = 0;

  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  view_info_t *p_view_info = p_db_var->view_info;

  u8 up_table_id = p_vv_info[vv_id].table_id;
  u16 node_id = 0;
  u16 blk_id = p_view_info[up_table_id].block_id;

  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(p_dm_handle != NULL);

  //Read view packet one by one
  for(; pkt_inx < p_view_info[up_table_id].packet_total; pkt_inx++)
  {
    //Restore flash node
    node_id = p_view_info[up_table_id].ni_start + pkt_inx;
    dm_restore_cache(p_dm_handle, blk_id, node_id);
  }
}


void db_undo_element_16v2(void *p_data, u8 vv_id)
{
  u32 item_cnt = 0;
  db_var_t *p_db_var = (db_var_t *)p_data;
  db_vv_info_t *p_vv_info = p_db_var->vv_info;
  table_info_t *p_table_info = p_db_var->table_info;

  u8 blk_id = p_table_info[p_vv_info[vv_id].table_id].block_id;
  u16 element_id = 0;

  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(p_dm_handle != NULL);

  //Make sure that elements in view needs updating
  if(p_vv_info[vv_id].element_need_up == FALSE
    || p_vv_info[vv_id].p_mem == NULL)
  {
    return;
  }

  for(item_cnt = 0; item_cnt < p_vv_info[vv_id].active_num; item_cnt++)
  {
    element_id = ITEM_ID(p_vv_info[vv_id].p_mem[item_cnt]);
    dm_restore_cache(p_dm_handle, blk_id, element_id);
  }
}


/*!
   init databse
   \return None.
  */
void db_init_database_16v2(void)
{
  u32 vv_cnt = 0;
  db_handle_t *p_db_handle = NULL;
  db_var_t *p_db_var = NULL;
  db_vv_info_t *p_vv_info = NULL;
  table_info_t *p_table_info = NULL;
  view_info_t *p_view_info = NULL;
  u32 cnt = 0;

  //Allocate memory for database
  p_db_handle = mtos_malloc(sizeof(db_handle_t));
  MT_ASSERT(p_db_handle != NULL);
  memset(p_db_handle, 0, sizeof(db_handle_t));

  p_db_handle->p_data = mtos_malloc(sizeof(db_var_t));
  MT_ASSERT(p_db_handle->p_data != NULL);
  p_db_var = (db_var_t *)p_db_handle->p_data;

  //Allocate private data memory
  //Initialize Virtual view information
  p_vv_info = p_db_var->vv_info;
  memset(p_vv_info, 0, sizeof(db_vv_info_t) * MAX_VIRTUAL_VIEW_NUM);

  //Initialize Real View information
  p_view_info = p_db_var->view_info;

  for(cnt = 0; cnt < MAX_TABLE_NUM; cnt++)
  {
    p_view_info[cnt].block_id = INVALID_TABLE_ID;
    p_view_info[cnt].ni_start = 0;
    p_view_info[cnt].packet_total = 0;
    p_view_info[cnt].p_mem_backup = NULL;
    p_view_info[cnt].sync_st = FALSE;
    p_view_info[cnt].item_count = 0;
  }

  //Initialize view table information
  p_table_info = p_db_var->table_info;

  for(cnt = 0; cnt < MAX_TABLE_NUM; cnt++)
  {
    p_table_info[cnt].block_id = INVALID_TABLE_ID;
    p_table_info[cnt].max_element_number = 0;
  }

  //Initial static array g_vv_info
  for(vv_cnt = 0; vv_cnt < MAX_VIRTUAL_VIEW_NUM; vv_cnt++)
  {
    p_vv_info[vv_cnt].table_id = 0;
    p_vv_info[vv_cnt].bit_only = 1;
    p_vv_info[vv_cnt].view_need_up = FALSE;
    p_vv_info[vv_cnt].element_need_up = FALSE;

    p_vv_info[vv_cnt].plus_only = TRUE;
    p_vv_info[vv_cnt].cond_func = NULL;
    p_vv_info[vv_cnt].p_mem = NULL;
    p_vv_info[vv_cnt].context = 0;
    p_vv_info[vv_cnt].vv_total = 0;
    p_vv_info[vv_cnt].create_num = 0;
    p_vv_info[vv_cnt].active_num = 0;
    p_vv_info[vv_cnt].pass_num = 0;
  }

  //Get data manager handle
  p_db_var->p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_db_var->p_dm_handle != NULL);

  //Max block number
  p_db_var->max_blk_num = 0;
  p_db_var->max_view_node_num = DEF_VIEW_NI_NUM;

  //Default clean block
  p_db_var->clean_block = DB_BLK_DAFAULT_STAT;
  

  //Clean block used
  p_db_var->node_id_used = 0;

  p_db_handle->active_view_item = db_active_view_item_16v2;
  p_db_handle->add_view_item = db_add_view_item_16v2;

  p_db_handle->clean = db_clean_16v2;
  p_db_handle->clear_view_item = db_clear_view_item_16v2;

  p_db_handle->create_table = db_create_table_16v2;
  p_db_handle->create_view = db_create_view_16v2;

  p_db_handle->create_virtual_view = db_create_virtual_view_16v2;
  p_db_handle->create_virtual_view_all = db_create_virtual_view_all_16v2;
  p_db_handle->del_view_item = db_del_view_item_16v2;

  p_db_handle->destroy_virtual_view = db_destroy_virtual_view_16v2;
  p_db_handle->edit_element = db_edit_element_16v2;

  p_db_handle->edit_view_item = db_edit_view_item_16v2;
  p_db_handle->exchange_view_item = db_exchange_view_item_16v2;
  p_db_handle->dump_view_item = db_dump_view_item_16v2;

  p_db_handle->get_element = db_get_element_16v2;
  p_db_handle->get_element_id_by_pos = db_get_element_id_by_pos_16v2;

  p_db_handle->set_max_view_node_num = db_set_max_view_node_num_16v2;
  p_db_handle->get_max_view_node_num = db_get_max_view_node_num_16v2;

  p_db_handle->get_mask_view_item = db_get_mask_view_item_16v2;
  p_db_handle->get_real_view_count = db_get_real_view_count_16v2;

  p_db_handle->get_view_item = db_get_view_item_16v2;
  p_db_handle->get_virtual_view_count = db_get_virtual_view_count_16v2;

  p_db_handle->move_view_item = db_move_view_item_16v2;

  p_db_handle->update_view = db_update_view_16v2;
  p_db_handle->set_mask_view_item = db_set_mask_view_item_16v2;

  p_db_handle->save_element_edit = db_save_element_edit_16v2;
  p_db_handle->save_view_edit = db_save_view_edit_16v2;

  p_db_handle->undo_element = db_undo_element_16v2;
  p_db_handle->undo_element_edit = db_undo_element_edit_16v2;

  p_db_handle->undo_view = db_undo_view_16v2;
  p_db_handle->update_element = db_update_element_16v2;

  p_db_handle->create_sub_virtual_view = db_create_sub_virtual_view_16v2;
  p_db_handle->destroy_sub_virtual_view = db_destroy_sub_virtual_view_16v2;

  p_db_handle->get_view_del_flag = db_get_view_del_flag_16v2;

  //Register class
  class_register(DB_CLASS_ID, p_db_handle);
}


/*!
   destroy databse
   \return None.
  */
void db_destory_database_16v2(void)
{
  db_handle_t *p_db_handle = NULL;
  
  //get database class handle
  p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  if(p_db_handle == NULL)
  {
    return;
  }

  mtos_free(p_db_handle->p_data);
  mtos_free(p_db_handle);

  //unregister class
  class_register(DB_CLASS_ID, NULL);
}

