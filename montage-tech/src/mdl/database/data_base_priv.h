/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_BASE_PRIV_H_
#define __DATA_BASE_PRIV_H_

/*!
  \file data_base_priv.h

  This file defines the private data structure of data base.
  And all the implementations of data base should support attach the each 
  process function in order to implement the data base function. 

  And this head file can only be included in data_base.c, data_base16.c, 
  data_base16v2.c and data_base32.c.
  */

/*!
  DB handle definition
  */
typedef struct db_handle_tag
{

  /*!
    Database private data
    */
  void *p_data;

  /*!
    Create a table into blocks
    the elements of table are pure data structures, such as SAT ,TP, PG etc.
    \param[in] p_data private data
    \param[in] block_id which block save the table's data
    \param[in] elements_num table size
    return table id
    */
  u8 (*create_table)(void *p_data, u8 block_id, u16 elements_num);

  /*!
    Create a view for table
    the view associate a table , store element's state and position

    \param[in] p_data private data
    \param[in] block_id which block save the view's data
    \param[in] table_id which table associate to

    \return created view id. 
    */
  u8 (*create_view)(void *p_data, u8 block_id, u8 table_id);

  /*!
    Edit a element 
    return operation Success or not . TRUE is SUC.
    \param[in] p_data private data
    \param[in] table_id which block the elements belong to
    \param[in] element_id which element will be edit
    \param[in] p_buffer element's addr
    \param[in] len element's length
    */
  db_ret_t (*edit_element)(void *p_data, u8 table_id, u16 element_id, 
     u8 *p_buffer, u16 len);
  /*!
    Save element edit operation

    \param[in] p_data private data
    \param[in] table_id which block the elements belong to
    \param[in] element_id which element will be edit
    */
  void (*save_element_edit)(void *p_data, u8 table_id, u16 element_id);

  /*!
    Do not save element edit operation

    \param[in] p_data private data
    \param[in] table_id which block the elements belong to
    \param[in] element_id which element will be edit
    */
  void (*undo_element_edit)(void *p_data, u8 table_id, u16 element_id);

  /*!
    Get a element 
    return operation Success or not . TRUE is SUC.
    \param[in] p_data private data
    \param[in] table_id which block the elements belong to
    \param[in] element_id which element will get
    \param[in] p_buffer element's addr
    \param[in] offset read offset for first addr
    \param[in] len read length
    */
  BOOL (*get_element)(void *p_data, u8 table_id, u16 element_id, u8 *p_buffer, 
    u16 offset, u16 len);  
  /*!
    create virtual view from a real view with a filter
    Return item number of view list
    
    \param[in] p_data private data
    \param[in] table_id the table which one created 
    \param[in] p_cond_func callback function , give a condition by create view 
                if cond_func is NULL, create all
    \param[in] bit_only need loading element data, or not. if it's FALSE, 
    need loading
    \param[in] p_mem extend virtual view, if the mem is NULL, create view in 
    internal buffer
    \param[in] context 
    \param[out] *p_vv_id id of the created virtual view

    */
  u16 (*create_virtual_view)(void *p_data, u8 table_id, 
    db_filter_proc_t p_cond_func, BOOL bit_only, u8 *p_mem, u32 context, 
    u8 *p_vv_id);
  /*!
    create virtual view from a real view with a filter
    Return item number of view list
    
    \param[in] p_data private data
    \param[in] table_id the table which one created 
    \param[in] p_cond_func callback function , give a condition by create view 
                if cond_func is NULL, create all
    \param[in] bit_only need loading element data, or not. if it's FALSE, 
    need loading
    \param[in] p_mem extend virtual view, if the mem is NULL, create view in 
    internal buffer
    \param[in] p_context 
    \param[in] fav_count
    \param[out] *p_vv_id id of the created virtual view

    */
  void (*create_virtual_view_all)(void *p_handle, u8 table_id, 
    db_filter_proc_new_t p_cond_func, BOOL bit_only, u8 *p_mem, u16 *p_context,
    u16 fav_count, u8 *p_vv_id);

  /*!
    Destroy virtual view information

    \param[in] p_data private data
    \param[in] vv_id the virtual view id which buffer created 
    */
  void (*destroy_virtual_view)(void *p_data, u8 vv_id);

  /*!
    Get virtual view information

    \param[in] p_data private data
    \param[in] vv_id the virtual view id
    \return current virtual view items count
    */
  u16 (*get_virtual_view_count)(void *p_data, u8 vv_id);
  /*!
    Get real view count

    \param[in] p_data private data
    \param[in] vv_id the virtual view id
    \param[out] current real view items count
    */
  u16 (*get_real_view_count)(void *p_data, u8 vv_id);


  /*!
    Update virtual view to real virtual and 
    store the real virtual to flash

    \param[in] p_data private data
    \param[in] vv_id the virtual view id which buffer created 
    \param[out] operation Success or not . TRUE is SUC.
    */
  BOOL (*update_view)(void *p_data, u8 vv_id);


  /*!
    Update all the element in 
    \param[in] p_data private data
    \param[in] vv_id : virtual view id
    \param[out] TRUE update complete, FALSE do not update
    */
  BOOL (*update_element)(void *p_data, u8 vv_id);

  /*!
    Add a view item for element
    return operation result

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] p_buffer data's addr
    \param[in] len data's length
    \param[out] p_pos new item's position
    */
  db_ret_t (*add_view_item)(void *p_data, u8 vv_id, u8 *p_buffer, 
    u16 len, u16 *p_pos);

  /*!
    Edit a item 

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] pos which item will be edit
    \param[in] p_buffer element's addr
    \param[in] len element's length
    return operation Success or not. TRUE is SUC.
    */
  BOOL (*edit_view_item)(void *p_data, u8 vv_id, u16 pos, u8 *p_buffer, 
    u16 len);

  /*!
    Save view edit

    \param[in] p_data private data
    \param[in] vv_id virtual view id
    \param[in] view_pos item position in view
    */
  void (*save_view_edit)(void *p_data, u8 vv_id, u16 view_pos);
  /*!
    Dot not save view edit into flash

    \param[in] p_data private data
    \param[in] vv_id: virtual view id
    */
  void (*undo_view)(void *p_data, u8 vv_id);

  /*!
    Do not save element edit into flash

    \param[in] p_data private data
    \param[in] vv_id virtual view id
    */
  void (*undo_element)(void *p_data, u8 vv_id);

  /*!
    Edit a item 
    return Success or not . TRUE is SUC.
    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] pos which item will be edit
    \param[in] offset read offset for first addr
    \param[in] len read length
    \param[out] p_buffer data's addr
    */
  BOOL (*get_view_item)(void *p_data, u8 vv_id, u16 pos, u16 offset, u16 len, 
    u8 *p_buffer);

  /*!
    Delete a view item

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] pos the position of delete item 
    */
  void (*del_view_item)(void *p_data, u8 vv_id, u16 pos);

  /*!
    Delete all of view

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    */
  void (*clear_view_item)(void *p_data, u8 vv_id);

  /*!
    active a view item

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] pos the position of active item 
    \return None
    */
  void (*active_view_item)(void *p_data, u8 vv_id, u16 pos);

  /*!
    Set view bit mask

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] pos the position of edit item 
    \param[in] mask_shift which one mask bit, from 0 to 1
    \param[in] flag set value
    */
  void (*set_mask_view_item)(void *p_data, u8 vv_id, u16 pos, u8 mask_shift, 
    BOOL flag);

  /*!
    Get view mask bit

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] pos the position of edit item 
    \param[in] mask_shift which one mask bit, from 0 to 3
    \return TRUE mask bit is 1, FALSE mask bit is 0
    */
  BOOL (*get_mask_view_item)(void *p_data, u8 vv_id, u16 pos, u8 mask_shift);

  /*!
    Exchange two items

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] pos_l
    \param[in] pos_r
    */
  void (*exchange_view_item)(void *p_data, u8 vv_id, u16 pos_l, u16 pos_r);

  /*!
    Exchange two items

    \param[in] p_data private data
    \param[in] vv_id which virtual view
    \param[in] p_mem
    \param[in] len
    */
  void (*dump_view_item)(void *p_data, u8 vv_id, void *p_mem, u16 len);
  
  /*!
    Move a view item from current position to target,
    and the target be after "pos_to" item

    \param[in] p_data private data
    \param[in] pos_form
    \param[in] pos_to
    \return None
    */
  void (*move_view_item)(void *p_data, u8 vv_id, u16 pos_from, u16 pos_to);

  /*!
    Get the element id of certain position

    \param[in] p_data private data
    \param[in] vv_id: virtual view id
    \param[in] pos: item position in view
    \param[out] element id
    */
  u16 (*get_element_id_by_pos)(void *p_data, u8 vv_id, u16 pos);

  /*!
    set view node index supported in database
    \param[in] p_data private data
    \param[in] max_num: num
    */
  void (*set_max_view_node_num)(void *p_data, u32 max_num);

  /*!
    get view node index supported in database
    \param[in] p_data private data
    \param[out] max
    */
  u32 (*get_max_view_node_num)(void *p_data);

  /*!
    Clean all view and element information
    \param[in] p_data private data
    */
  void (*clean)(void *p_data);

  /*!
    Create virtual view from a real view with a filter

    \param[in] p_data private data
    \param[in] table_id the table which one created 
    \param[in] cond_func callback function , give a condition by create view 
    if cond_func is NULL, create all
    \param[in] bit_only need loading element data, or not. if it's FALSE, 
    need loading
    \param[in] mem extend virtual view, if the mem is NULL, create view 
    in internal buffer
    \param[in] context 
    \param[in] parent_vv_id parent view id
    \param[out] *p_sub_vv_id id of the created virtual view
    \param[out] item number of view list. 
    */
  u16 (*create_sub_virtual_view)(void *p_data, db_filter_proc_t p_cond_func,
  BOOL bit_only, u8 *p_mem, u32 context, u8 *p_sub_vv_id, u8 parent_vv_id);

  /*!
    Destroy sub-virtual view 

    \param[in] p_data private data
    \param[in] sub_vv_id: sub virtual view id
    */
  void (*destroy_sub_virtual_view)(void *p_data, u8 sub_vv_id);

  /*!
   get view delete flag

   \param[in] vv_id the virtual view id
   \return view delete flag
  */
  BOOL (*get_view_del_flag)(void *p_data, u8 vv_id);

}db_handle_t;

#endif //__DATA_BASE_PRIV_H_

