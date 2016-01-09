/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include "sys_define.h"
#include "sys_types.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "mdl.h"
#include "data_manager.h"
#include "data_base.h"
#include "data_base_priv.h"


u8 db_create_table(void *p_handle, u8 block_id, u16 elements_num)
{
  db_handle_t *p_db_proc = NULL;
  
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;

  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->create_table != NULL);
  
  return p_db_proc->create_table(p_db_proc->p_data, block_id, elements_num);
}

u8 db_create_view(void *p_handle, u8 block_id, u8 table_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->create_view != NULL);
  
  return p_db_proc->create_view(p_db_proc->p_data, block_id, table_id);
}

db_ret_t db_edit_element(void *p_handle, u8 table_id, u16 element_id, 
u8 *p_buffer, u16 len)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->edit_element != NULL);
  
  return p_db_proc->edit_element(p_db_proc->p_data, table_id, element_id, 
         p_buffer, len);
}

void db_save_element_edit(void *p_handle, u8 table_id, u16 element_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->save_element_edit != NULL);
  
  p_db_proc->save_element_edit(p_db_proc->p_data, table_id, element_id);
}

void db_undo_element_edit(void *p_handle, u8 table_id, u16 element_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->undo_element_edit != NULL);
  
  p_db_proc->undo_element_edit(p_db_proc->p_data, table_id, element_id);
}

BOOL db_get_element(void *p_handle, u8 table_id, u16 element_id, 
u8 *p_buffer, u16 offset, u16 len)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->get_element != NULL);
  
  return p_db_proc->get_element(p_db_proc->p_data, table_id, element_id, 
         p_buffer, offset, len);
}

u16 db_create_virtual_view(void *p_handle, u8 table_id, 
db_filter_proc_t p_cond_func, BOOL bit_only, u8 *p_mem, u32 context, 
  u8 *p_vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->create_virtual_view != NULL);
  
  return p_db_proc->create_virtual_view(p_db_proc->p_data, table_id, 
    p_cond_func, bit_only, p_mem, context, p_vv_id);
}

void db_create_virtual_view_all(void *p_handle, u8 table_id, 
db_filter_proc_new_t p_cond_func, BOOL bit_only, u8 *p_mem, u16 *p_context,
u16 fav_count, u8 *p_vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->create_virtual_view_all != NULL);
  
  p_db_proc->create_virtual_view_all(p_db_proc->p_data, table_id, 
    p_cond_func, bit_only, p_mem, p_context,fav_count, p_vv_id);
}

void db_destroy_virtual_view(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->destroy_virtual_view != NULL);
  
  p_db_proc->destroy_virtual_view(p_db_proc->p_data, vv_id);
}

BOOL db_get_view_del_flag(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->get_view_del_flag != NULL);
  
  return p_db_proc->get_view_del_flag(p_db_proc->p_data, vv_id);
}

u16 db_get_virtual_view_count(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->get_virtual_view_count != NULL);
  
  return p_db_proc->get_virtual_view_count(p_db_proc->p_data, vv_id);
}

u16 db_get_real_view_count(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->get_real_view_count != NULL);
  
  return p_db_proc->get_real_view_count(p_db_proc->p_data, vv_id);
}

BOOL db_update_view(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->update_view != NULL);
  
  return p_db_proc->update_view(p_db_proc->p_data, vv_id);
}

BOOL db_update_element(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->update_element != NULL);
  
  return p_db_proc->update_element(p_db_proc->p_data, vv_id);
}

db_ret_t db_add_view_item(void *p_handle, u8 vv_id, u8 *p_buffer, 
u16 len, u16 *p_pos)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->add_view_item != NULL);
  
  return p_db_proc->add_view_item(p_db_proc->p_data, vv_id, p_buffer, len, 
    p_pos);
}

BOOL db_edit_view_item(void *p_handle, u8 vv_id, u16 pos, u8 *p_buffer, 
u16 len)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->edit_view_item != NULL);
  
  return p_db_proc->edit_view_item(p_db_proc->p_data, vv_id, pos, 
          p_buffer, len);
}

void db_save_view_edit(void *p_handle, u8 vv_id, u16 view_pos)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->save_view_edit != NULL);
  
  p_db_proc->save_view_edit(p_db_proc->p_data, vv_id, view_pos);
}

void db_undo_view(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->undo_view != NULL);
  
  p_db_proc->undo_view(p_db_proc->p_data, vv_id);
}

void db_undo_element(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->undo_element != NULL);
  
  p_db_proc->undo_element(p_db_proc->p_data, vv_id);
}

BOOL db_get_view_item(void *p_handle, u8 vv_id, u16 pos, u16 offset, 
u16 len, u8 * p_buffer)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->get_view_item != NULL);
  
  return p_db_proc->get_view_item(p_db_proc->p_data, vv_id, pos, offset, 
         len, p_buffer);
}

void db_del_view_item(void *p_handle, u8 vv_id, u16 pos)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->del_view_item != NULL);

  p_db_proc->del_view_item(p_db_proc->p_data, vv_id, pos);
}

void db_clear_view_item(void *p_handle, u8 vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->clear_view_item != NULL);
  
  p_db_proc->clear_view_item(p_db_proc->p_data, vv_id);
}

void db_active_view_item(void *p_handle, u8 vv_id, u16 pos)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->active_view_item != NULL);
  
  p_db_proc->active_view_item(p_db_proc->p_data, vv_id, pos);
}

void db_set_mask_view_item(void *p_handle, u8 vv_id, u16 pos, 
u8 mask_shift, BOOL flag)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->set_mask_view_item != NULL);
  
  p_db_proc->set_mask_view_item(p_db_proc->p_data, vv_id, pos, 
    mask_shift, flag);
}

BOOL db_get_mask_view_item(void *p_handle, u8 vv_id, u16 pos, 
u8 mask_shift)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->get_mask_view_item != NULL);
  
  return p_db_proc->get_mask_view_item(p_db_proc->p_data, vv_id, pos,
            mask_shift);
}

void db_exchange_view_item(void *p_handle, u8 vv_id, u16 pos_l, 
u16 pos_r)
{
  db_handle_t  *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->exchange_view_item != NULL);
  
  p_db_proc->exchange_view_item(p_db_proc->p_data, vv_id, pos_l, pos_r);
}

void db_dump_view_item(void *p_handle, u8 vv_id, void *p_mem, u16 len)
{
  db_handle_t  *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->dump_view_item != NULL);
  
  p_db_proc->dump_view_item(p_db_proc->p_data, vv_id, p_mem, len);
}

void db_move_view_item(void *p_handle, u8 vv_id, u16 pos_from, 
u16 pos_to)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->move_view_item != NULL);
  
  p_db_proc->move_view_item(p_db_proc->p_data, vv_id, pos_from, pos_to);
}

u16 db_get_element_id_by_pos(void *p_handle, u8 vv_id, u16 pos)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->get_element_id_by_pos != NULL);
  
  return p_db_proc->get_element_id_by_pos(p_db_proc->p_data, vv_id, pos);
}


void db_set_max_view_node_num(void *p_handle, u32 max_num)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;

  if(p_db_proc->set_max_view_node_num != NULL)
  {
    p_db_proc->set_max_view_node_num(p_db_proc->p_data, max_num);
  }
}

u32 db_get_max_view_node_num(void *p_handle)
{
  db_handle_t *p_db_proc = NULL;

  p_db_proc = (db_handle_t *)p_handle;

  if(p_db_proc != NULL)
  {
    if(p_db_proc->get_max_view_node_num != NULL)
    {
      return p_db_proc->get_max_view_node_num(p_db_proc->p_data);
    }
  }
  
  return DEF_VIEW_NI_NUM;
}

void db_clean(void *p_handle)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->clean != NULL);
  
  p_db_proc->clean(p_db_proc->p_data);
}

u16 db_create_sub_virtual_view(void *p_handle, 
  db_filter_proc_t p_cond_func,BOOL bit_only, u8 *p_mem, u32 context, 
u8 *p_sub_vv_id, u8 parent_vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->create_sub_virtual_view != NULL);
  
  return p_db_proc->create_sub_virtual_view(p_db_proc->p_data, p_cond_func, 
   bit_only, p_mem, context, p_sub_vv_id, parent_vv_id);
}

void db_destroy_sub_virtual_view(void *p_handle, u8 sub_vv_id)
{
  db_handle_t *p_db_proc = NULL;
  MT_ASSERT(p_handle != NULL);
  p_db_proc = (db_handle_t *)p_handle;
  
  MT_ASSERT(p_db_proc->p_data != NULL);
  MT_ASSERT(p_db_proc->destroy_sub_virtual_view != NULL);
  
  p_db_proc->destroy_sub_virtual_view(p_db_proc->p_data,sub_vv_id);
}

