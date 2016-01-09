/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BLOCK_CTRL_H__
#define __BLOCK_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif


/*!
  block image type.
  */
typedef enum
{
  /*!
    block image by id.
    */
  BLOCK_IMG_ID = 0,
  /*!
    block image by external data.
    */
  BLOCK_IMG_EXT,
  /*!
    block image by pure color.
    */
  BLOCK_IMG_CLR,    
}block_img_type_t;

/*!
  block string type.
  */
typedef enum
{
  /*!
    block string by id.
    */
  BLOCK_STR_ID = 0,
  /*!
    block string by external data.
    */
  BLOCK_STR_EXT,
}block_str_type_t;

/*!
   \file ctrl_block.h

   This file defined data structures and macro constant for bitmap control.
   It also defines some interfaces for aplication layer modules.
  */

/*!
   Register block control class to system.

   \param[in] max_cnt : max block control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE block_register_class(u16 max_cnt);



/*!
   Default class process function for block control.

   \param[in] p_ctrl : point a block control.
   \param[in] msg : message for block control.
   \param[in] para1 : reserved.
   \param[in] para2 : reserved.
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE block_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
  add block node.
  */
void block_add_node(control_t *p_ctrl,
  block_img_type_t img_type, u32 img_data,
  block_str_type_t str_type, u32 str_data, u32 fstyle_idx, u32 align_style,
  u16 x, u16 y, u16 w, u16 h, BOOL do_reflection, u32 context);

/*!
  set border color.
  */
void block_set_border_color(control_t *p_ctrl, u32 color);  

/*!
  block update.
  */
void block_update(control_t *p_ctrl);

/*!
  get focus context.
  */
u32 block_get_focus_context(control_t *p_ctrl);

#ifdef  __cplusplus
}
#endif
#endif


