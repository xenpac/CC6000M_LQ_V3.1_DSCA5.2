/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ORB_CTRL_H__
#define __ORB_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_orb.h

   This file defined data structures and macro constant for bitmap control.
   It also defines some interfaces for aplication layer modules.
  */

/*!
  orb img type.
  */
typedef enum
{
  /*!
    img by id.
    */
  ORB_IMG_ID,
  /*!
    img by ext.
    */
  ORB_IMG_EXT,
}orb_img_t;

/*!
  orb str type.
  */
typedef enum
{
  /*!
    str by id.
    */
  ORB_STR_ID,
  /*!
    img by ext.
    */
  ORB_STR_EXT,
}orb_str_t;

/*!
  orb node.
  */
typedef struct
{
  /*!
    orb img type.
    */
  orb_img_t img_type;    
  /*!
    img data.
    */
  u32 img_data;
  /*!
    orb str type.
    */
  orb_str_t str_type;    
  /*!
    str data.
    */
  u32 str_data;
  /*!
    node context.
    */
  u32 context;    
}orb_node_t;

/*!
  Defines the callback function of list, it will be called for reloading data.
  */
typedef BOOL (*orb_get_node_t)(control_t *p_ctrl, u16 index, orb_node_t *p_node);


/*!
   Register orb control class to system.

   \param[in] max_cnt : max orb control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE orb_register_class(u16 max_cnt);

/*!
   Default class process function for orb control.

   \param[in] p_ctrl : point a orb control.
   \param[in] msg : message for orb control.
   \param[in] para1 : reserved.
   \param[in] para2 : reserved.
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE orb_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
  get orb focus.
  */
u16 orb_get_focus(control_t *p_ctrl);

/*!
  set cb for get node.
  */
void orb_set_cb(control_t *p_ctrl, orb_get_node_t orb_get_node);

#ifdef  __cplusplus
}
#endif
#endif


