/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SIMPLE_Q_H_
#define __SIMPLE_Q_H_

/*!
  Module initialize function

  \return TRUE middleware handle
  */
class_handle_t simple_queue_init(void);

/*!
  Create a simple queue

  \param[in] handle simple_queue's handle
  \param[in] depth Depth of this queue
  \param[in] p_buffer Memory address for this queue
  \param[out] p_used_size this queue used buffer size

  \return Return queue id otherwise assertion failed
  */
u8 create_simple_queue(class_handle_t handle, u16 depth,
  u8 * p_buffer, u32 *p_used_size);

/*!
  Create a simple queue version 1

  \param[in] handle simple_queue's handle
  \param[in] depth Depth of this queue

  \return Return queue id otherwise assertion failed
  */
u8 create_simple_queue1(class_handle_t handle, u16 depth);


/*!
  Destory the simple queue, clear all info

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  */
void destory_simple_queue(class_handle_t handle, u8 id);

/*!
  Destory the simple queue, clear all info

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  */
void destory_simple_queue1(class_handle_t handle, u8 id);

/*!
  Push an item into queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[in] value Data will be pushed into queue

  \return Return TRUE means successed, FALSE means queue was full
  */
BOOL push_simple_queue(class_handle_t handle, u8 id, u16 value);

/*!
  Push an item into queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[in] value Data will be pushed into queue

  \return Return TRUE means successed, FALSE means queue was full
  */
BOOL push_simple_queue_on_head(class_handle_t handle, u8 id, u16 value);

/*!
  Pop a data from a queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[out] p_value Data value

  \return Return zero means successed, -1 means queue was empty
  */
BOOL pop_simple_queue(class_handle_t handle, s8 id, u16 *p_value);

/*!
  Get the element number from a queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id

  \return Return the element number
  */
u16 get_simple_queue_len(class_handle_t handle, u8 id);

/*!
  Get the element list from a queue

  \param[in] handle simple_queue's handle
  \param[in] id queue id
  \param[out] element_list obtain element list
  \return Return the element number
  */
u16 get_simple_queue_element(class_handle_t handle, u8 id,
  u16 *p_element_list);

#endif // End for __SIMPLE_Q_H_


