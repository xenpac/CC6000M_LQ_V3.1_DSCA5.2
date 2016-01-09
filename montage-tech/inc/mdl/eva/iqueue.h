/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __IQUEUE_H_
#define __IQUEUE_H_

/*!
   Define the type of parsing function for a element.
  */
typedef void (*ele_parser_t)(u32 element, void *p_parse_param);

/*!
  i queue define
  */
typedef struct tag_iqueue
{
  /*!
    Destory the queue

    \param[in] _this this handle
    */
  void (*destory_iqueue)(handle_t _this);

  /*!
    Push an element into queue

    \param[in] _this this handle
    \param[in] element pushed into queue

    \return Return TRUE means successed, FALSE means the element don't belong here or
          it has be in.
    */
  BOOL (*push_iqueue)(handle_t _this, u32 element);

  /*!
    Push an item into queue, and put it to head

    \param[in] _this this handle
    \param[in] element pushed into queue

    \return Return TRUE means successed, FALSE means the element don't belong here or
          it has be in.
    */
  BOOL (*push_iqueue_on_head)(handle_t _this, u32 element);

  /*!
    Pop a element from the queue

    \param[in] _this this handle
    \param[out] p_element element

    \return Return TRUE means successed, FALSE means queue was empty
    */
  BOOL (*pop_iqueue)(handle_t _this, u32 *p_element);

  /*!
    Get the element number from the queue

    \param[in] _this this handle

    \return Return the element number
    */
  u32 (*get_iqueue_len)(handle_t _this);
  
  /*!
    Print queue.

    \param[in] _this this handle
    \param[in] p_parser parse element callback function.
    \param[in] p_param  parse parameter
    */
  void (*print_iqueue)(handle_t _this, ele_parser_t p_parser, void *p_param);
}iqueue_t;

/*!
  Create a queue

  \param[in] depth Depth of this queue
  \param[in] p_name name
      it's default policy, the element is [0, depth)

  \return Return iqueue_t queue handle
  */
iqueue_t * create_iqueue(u32 depth, char *p_name);

/*!
  Create a queue. it's extern policy, the element is preset by p_element

  \param[in] depth Depth of this queue,
  \param[in] p_element preset element of this queue,
  \param[in] p_name name

  \return Return iqueue_t queue handle
  */
iqueue_t * create_iqueue_ext(u32 depth, u32 *p_element, char *p_name);

#endif // End for __IQUEUE_H_



