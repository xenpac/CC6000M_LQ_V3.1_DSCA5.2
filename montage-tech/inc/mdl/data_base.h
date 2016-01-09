/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_BASE_H_
#define __DATA_BASE_H_

/*!
  Database unkown id
  */
#define DB_UNKNOWN_ID (0xFFFF)
/*!
  Database return status 
  */
typedef enum tag_db_ret
{
  /*!
    Database fail
    */
  DB_FAIL = 0,
  /*!
    Database full
    */
  DB_FULL,
  /*!
    Database successful
    */
  DB_SUC,
  /*!
    Database data not found
    */
  DB_NOT_FOUND
}db_ret_t;

/*!
  View node index supported in database
  */
#define DEF_VIEW_NI_NUM (300)   //(64+3000+5000)/28

/*!
  Database filer process function
  \param[in] bit_mask: bit mask position
  \param[in] context: context information for filtering data
  \param[in] block_id: node block id
  \param[in] node_id: node id 
  */
typedef BOOL (*db_filter_proc_t)(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id);

/*!
  Database filer process new function
  \param[in] bit_mask: bit mask position
  \param[in] context: context information for filtering data
  \param[in] block_id: node block id
  \param[in] node_id: node id 
  */
typedef u32 (*db_filter_proc_new_t)(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id);

/*!
  Create a table into blocks
  the elements of table are pure data structures, such as SAT ,TP, PG etc.
  \param[in] block_id which block save the table's data
  \param[in] elements_num table size
  \return Return created table id. 
  */
u8 db_create_table(void *p_handle, u8 block_id, u16 elements_num);

/*!
  Create a view for table
  the view associate a table , store element's state and position
  \param[in] block_id which block save the view's data
  \param[in] table_id which table associate to
  \return created view id. 
  */
u8 db_create_view(void *p_handle, u8 block_id, u8 table_id);

/*!
  Edit a element 
  \param[in] table_id which block the elements belong to
  \param[in] element_id which element will be edit
  \param[in] pbuffer element's addr
  \param[in] len element's length
  \return operation Success or not . TRUE is SUC.
  */
db_ret_t db_edit_element(void *p_handle, u8 table_id, u16 element_id, 
u8 *p_buffer,u16 len);

/*!
  Save element edit operation
  \param[in] table_id which block the elements belong to
  \param[in] element_id which element will be edit
  */
void db_save_element_edit(void *p_handle, u8 table_id, u16 element_id);

/*!
  Do not save element edit operation
  \param[in] table_id which block the elements belong to
  \param[in] element_id which element will be edit
  */
void db_undo_element_edit(void *p_handle, u8 table_id, u16 element_id);

/*!
	Get a element
  \param[in] table_id which block the elements belong to
  \param[in] element_id which element will get
  \param[in] pbuffer element's addr
  \param[in] offset read offset for first addr
  \param[in] len read length
  \return operation Success or not . TRUE is SUC.
  */
BOOL db_get_element(void *p_handle, u8 table_id, u16 element_id, 
u8 * p_buffer, u16 offset, u16 len);
/*!
  create virtual view from a real view with a filter
  \param[in] table_id the table which one created 
  \param[in] cond_func callback function , give a condition by create view 
   if cond_func is NULL, create all
  \param[in] bit_only need loading element data, or not. if it's FALSE, 
						need loading
  \param[in] mem extend virtual view, if the mem is NULL, create view in 
						internal buffer
  \param[in] context 
  \param[out] *p_vv_id id of the created virtual view
  \return Return item number of view list. 
  */
u16 db_create_virtual_view(void *p_handle, u8 table_id, 
db_filter_proc_t p_cond_func, BOOL bit_only, u8 *p_mem, u32 context, 
u8 *p_vv_id);
/*!
  create virtual view from a real view with a filter
  \param[in] table_id the table which one created 
  \param[in] new cond_func callback function , give a condition by create view 
   if cond_func is NULL, create all
  \param[in] bit_only need loading element data, or not. if it's FALSE, 
						need loading
  \param[in] mem extend virtual view, if the mem is NULL, create view in 
						internal buffer
  \param[out] p_context, store item count of each fav group 
  \param[in] max fav froup count, <=32 
  \param[out] *p_vv_id id of the created virtual view(not used)
  \return Return item count of each fav group. 
  */
void db_create_virtual_view_all(void *p_handle, u8 table_id, 
db_filter_proc_new_t p_cond_func, BOOL bit_only, u8 *p_mem, u16 *p_context,
u16 fav_count, u8 *p_vv_id);

/*!
  Destroy virtual view information
  \param[in] vv_id the virtual view id which buffer created 
  */
void db_destroy_virtual_view(void *p_handle, u8 vv_id);

/*!
  get virtual view del information
  \param[in] vv_id the virtual view id which buffer created 
  */
BOOL db_get_view_del_flag(void *p_handle, u8 view_id);

/*!
  Get virtual view information
  \param[in] vv_id the virtual view id
  \return current virtual view items count
  */
u16 db_get_virtual_view_count(void *p_handle, u8 vv_id);
/*!
  Get real view count
  \param[in] vv_id the virtual view id
  \return current real view items count
  */
u16 db_get_real_view_count(void *p_handle, u8 vv_id);


/*!
  Update virtual view to real virtual and 
  store the real virtual to flash
  \param[in] vv_id the virtual view id which buffer created 
  \return operation Success or not . TRUE is SUC.
  */
BOOL db_update_view(void *p_handle, u8 vv_id);


/*!
  Update all the element in 
  \param[in] vv_id : virtual view id
  \param[out] TRUE update complete, FALSE do not update
  */
BOOL db_update_element(void *p_handle, u8 vv_id);

/*!
  Add a view item for element
  \param[in] vv_id which virtual view
  \param[in] pbuffer data's addr
  \param[in] len data's length
  \param[out] p_pos new item's position
  \return operation result
  */
db_ret_t db_add_view_item(void *p_handle, u8 vv_id, u8 *p_buffer,
u16 len, u16 *p_pos);

/*!
  Edit a item 
  \param[in] vv_id which virtual view
  \param[in] pos which item will be edit
  \param[in] pbuffer element's addr
  \param[in] len element's length
  \param[out] operation Success or not. TRUE is SUC.
  */
BOOL db_edit_view_item(void *p_handle, u8 vv_id, u16 pos, 
u8 *p_buffer, u16 len);
/*!
  Save view edit
  \param[in] vv_id: virtual view id
  \param[in] view_pos: item position in view
  */
void db_save_view_edit(void *p_handle, u8 vv_id, u16 view_pos);
/*!
  Dot not save view edit into flash
  \param[in] vv_id: virtual view id
  */
void db_undo_view(void *p_handle, u8 vv_id);
/*!
  Do not save element edit into flash
  \param[in] vv_id: virtual view id
  */
void db_undo_element(void *p_handle, u8 vv_id);

/*!
  Edit a item 
  \param[in] vv_id which virtual view
  \param[in] pos which item will be edit
  \param[in] offset read offset for first addr
  \param[in] len read length
  \param[out] pbuffer data's addr
  \param[out] Success or not . TRUE is SUC.
  */
BOOL db_get_view_item(void *p_handle, u8 vv_id, u16 pos, u16 offset, 
u16 len, u8 *p_buffer);

/*!
  Delete a view item
  \param[in] vv_id which virtual view
  \param[in] pos the position of delete item 
  */
void db_del_view_item(void *p_handle, u8 vv_id, u16 pos);

/*!
  Delete all of view
  \param[in] vv_id which virtual view
  */
void db_clear_view_item(void *p_handle, u8 vv_id);

/*!
  active a view item
  \param[in] vv_id which virtual view
  \param[in] pos the position of active item 
  */
void db_active_view_item(void *p_handle, u8 vv_id, u16 pos);
/*!
  Set view bit mask
  \param[in] vv_id which virtual view
  \param[in] pos the position of edit item 
  \param[in] mask_shift which one mask bit, from 0 to 1
  \param[in] flag set value
  */
void db_set_mask_view_item(void *p_handle, u8 vv_id, u16 pos, 
u8 mask_shift, BOOL flag);

/*!
  Get view mask bit
  \param[in] vv_id which virtual view
  \param[in] pos the position of edit item 
  \param[in] mask_shift which one mask bit, from 0 to 3
  \param[out] TRUE mask bit is 1, FALSE mask bit is 0
  */
BOOL db_get_mask_view_item(void *p_handle, u8 vv_id, u16 pos, 
u8 mask_shift);

/*!
  Exchange two items
  \param[in] vv_id which virtual view
  \param[in] pos_l
  \param[in] pos_r
  */
void db_exchange_view_item(void *p_handle, u8 vv_id, u16 pos_l, 
u16 pos_r);

/*!
  Exchange two items
  \param[in] vv_id which virtual view
  \param[in] p_mem
  \param[in] len
  */
void db_dump_view_item(void *p_handle, u8 vv_id, void *p_mem, u16 len);

/*!
	Move a view item from current position to target,
	and the target be after "pos_to" item
	
  \param[in] pos_form
  \param[in] pos_to
  \return None
  */
void db_move_view_item(void *p_handle, u8 vv_id, u16 pos_from, 
u16 pos_to);
/*!
  Get the element id of certain position
  \param[in] vv_id: virtual view id
  \param[in] pos: item position in view
  \param[out] element id
  */
u16 db_get_element_id_by_pos(void *p_handle, u8 vv_id, u16 pos);

/*!
  set view node index supported in database
  \param[in] p_handle: handle
  \param[in] max_num: num
  */
void db_set_max_view_node_num(void *p_handle, u32 max_num);

/*!
  get view node index supported in database
  \param[in] p_handle: handle
  \param[out] max
  */
u32 db_get_max_view_node_num(void *p_handle);

/*!
  Clean all view and element information
  \param[in] p_handle database handle
  */
void db_clean(void *p_handle);

/*!
  Create sub-virtual view from a real view with a filter
  \param[in] cond_func filter function for create view
  \param[in] bit_only need loading element data, or not
  \param[in] p_mem extend buffer for creating view
  \param[in] context context information for creating view
  \param[out] p_sub_vv_id id of the created virtual view
  \param[in] parent_vv_id parent view id
  \param[out] item number of view list
  */
u16 db_create_sub_virtual_view(void *p_handle, 
db_filter_proc_t p_cond_func, BOOL bit_only, u8 *p_mem, u32 context, 
u8 *p_sub_vv_id, u8 parent_vv_id);
/*!
  Destroy sub-virtual view
  \param[in] sub_vv_id: sub virtual view id
  */
void db_destroy_sub_virtual_view(void *p_handle, u8 sub_vv_id);

#endif //__DATA_BASE_H_
