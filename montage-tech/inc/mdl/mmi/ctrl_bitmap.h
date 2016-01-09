/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BMAP_CTRL_H__
#define __BMAP_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_bitmap.h

   This file defined data structures and macro constant for bitmap control.
   It also defines some interfaces for aplication layer modules.
  */

/*!
   Bitmap align type mask
  */
#define BMAP_ALIGN_MASK       0xF0000000
/*!
   Bitmap data mode mask, 0 is bmp_id ; 1 is extern bitmap
  */
#define BMAP_DATAMODE_MASK    0x00000001

/*!
   Register bitmap control class to system.

   \param[in] max_cnt : max bitmap control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE bmap_register_class(u16 max_cnt);

/*!
   Set the positon of the bitmap in a bitmap control.

   \param[in] p_ctrl : point to a bitmap control.
   \param[in] left		 : left interval.
   \param[in] top		 : top interval.
   \return : NULL
  */
void bmap_set_bmap_offset(control_t *p_ctrl, u16 left, u16 top);


/*!
  get bmap offset.
  */
void bmap_get_bmap_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top);

/*!
   Set the align of the bitmap in a bitmap control.

   \param[in] p_ctrl : point to a bitmap control.
   \param[in] style	 : new align type to be set.
   \return : NULL
  */
void bmap_set_align_type(control_t *p_ctrl, u32 style);

/*!
  bmap get align type.
  */
u32 bmap_get_align_type(control_t *p_ctrl);

/*!
   Set the data mode of a bitmap control.

   \param[in] p_ctrl : point to a bitmap control.
   \param[in] enable : True, extern bitmap; False, bitmap id.
   \return : NULL
  */
void bmap_set_content_type(control_t *p_ctrl, BOOL enable);

/*!
  get content type.
  */
BOOL bmap_get_content_type(control_t *p_ctrl);

/*!
   Set the content of bitmap control by bitmap id.

   \param[in] p_ctrl : point to a bitmap control.
   \param[in] bmap_id : bitmap id.
   \return : NULL
  */
void bmap_set_content_by_id(control_t *p_ctrl, u16 bmap_id);

/*!
   Set the content of bitmap control by bitmap data.

   \param[in] p_ctrl : point to a bitmap control.
   \param[in] p_bmp : point to bitmap data buffer.
   \return : NULL
  */
void bmap_set_content_by_data(control_t *p_ctrl, bitmap_t *p_bmp);

/*!
   Get the bitmap id of a bitmap control.

   \param[in] p_ctrl : point to a bitmap control.
   \return : if success, return bitmap content(id or addr, decide by type).
  */
u32 bmap_get_content(control_t *p_ctrl);

/*!
   Default class process function for bitmap control.

   \param[in] p_ctrl : point a bitmap control.
   \param[in] msg : message for bitmap control.
   \param[in] para1 : reserved.
   \param[in] para2 : reserved.
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE bmap_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif

#endif
