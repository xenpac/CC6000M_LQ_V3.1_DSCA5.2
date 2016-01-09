/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ALBUM_CTRL_H__
#define __ALBUM_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_album.h

   This file defined data structures and macro constant for bitmap control.
   It also defines some interfaces for aplication layer modules.
  */

/*!
  String content type mask
  */
#define ALBUM_STRTYPE_MASK        0x0010000L

/*!
  Set string content by unicode string
  */
#define ALBUM_STRTYPE_EXT         0x0000000L

/*!
   Set string content by string id
  */
#define ALBUM_STRTYPE_ID          0x0010000L

/*!
  String content type mask
  */
#define ALBUM_IMGTYPE_MASK        0x0020000L

/*!
  img type external data.
  */
#define ALBUM_IMGTYPE_EXT         0x0000000L

/*!
  img type bitmap id.
  */
#define ALBUM_IMGTYPE_ID          0x0020000L



/*!
   Register album control class to system.

   \param[in] max_cnt : max album control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE album_register_class(u16 max_cnt);



/*!
   Default class process function for album control.

   \param[in] p_ctrl : point a album control.
   \param[in] msg : message for album control.
   \param[in] para1 : reserved.
   \param[in] para2 : reserved.
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE album_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
  album set image type: id / ext.
  */
void album_set_img_type(control_t *p_ctrl, u32 type);

/*!
  set total image count.
  */
void album_set_total(control_t *p_ctrl, u16 total);

/*!
  set focus image.
  */
void album_set_focus(control_t *p_ctrl, u16 focus);

/*!
  set image by id.
  */
void album_set_img_id(control_t *p_ctrl, u16 idx, u16 img_id);

/*!
  set image by external data.
  */
void album_set_img_ext(control_t *p_ctrl, u16 idx, bitmap_t *p_bmp);

/*!
  get album focus.
  */
u16 album_get_focus(control_t *p_ctrl);

#ifdef  __cplusplus
}
#endif
#endif

