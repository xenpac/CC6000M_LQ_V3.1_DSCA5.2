/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SBOX_CTRL_H__
#define __SBOX_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_setbox.h

   This file defined some interfaces of setbox control for application layer
   modules.
  */

/*!
   String align type mask of matrixbox control
  */
#define SBOX_ALIGN_MASK     0xF0000000

/*!
   String content type mask
  */
#define SBOX_STRTYPE_MASK       0x00F00001
/*!
   Set string content by unicode string
  */
#define SBOX_STRTYPE_UNICODE    0x00000001
/*!
   Set string content by static unicode string
  */
#define SBOX_STRTYPE_EXTSTR     0x00100001
/*!
   Set string content by string id
  */
#define SBOX_STRTYPE_STRID      0x00200001
/*!
   Set string content by decimal number
  */
#define SBOX_STRTYPE_DEC        0x00300001
/*!
   Set string content by hexadecimal number
  */
#define SBOX_STRTYPE_HEX        0x00400001

/*!
   Register setbox control class to system.

   \param[in] max_cnt : max setbox control number
   \return : SUCESS or ERR_FAILURE.
  */
RET_CODE sbox_register_class(u16 max_cnt);

/*!
   Set middle move block r-style.
   \param[in] p_ctrl : setbox control
   \param[in] n_mid_rstyle : normal move block rstyle
   \param[in] h_mid_rstyle : highlight move block rstyle
   \param[in] g_mid_rstyle : gray move block rstyle
   \return : NULL
  */
void sbox_set_mid_rstyle(control_t *p_ctrl,
                        u32 n_mid_rstyle,
                        u32 h_mid_rstyle,
                        u32 g_mid_rstyle);

/*!
  sbox get mid rstyle.
  */
void sbox_get_mid_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_mid_rstyle);

/*!
   Set total count of setbox.

   \param[in] p_ctrl : setbox control
   \param[in] total : total count
   \return : NULL
  */
BOOL sbox_set_count(control_t *p_ctrl, u8 total);

/*!
   Get total count of setbox.

   \param[in] p_ctrl : setbox control
   \return : total count.
  */
u8 sbox_get_count(control_t *p_ctrl);

/*!
   Set current position of setbox.

   \param[in] p_ctrl : setbox control
   \param[in] cur_pos : new pos
   \return : NULL
  */
void sbox_set_focus_pos(control_t *p_ctrl, u16 cur_pos);

/*!
   Get current position of setbox

   \param[in] p_ctrl : setbox control
   \return : current pos
  */
u8 sbox_get_focus_pos(control_t *p_ctrl);

/*!
   Increase setbox

   \param[in] p_ctrl : setbox control
   \return : TRUE/FALSE
  */
BOOL sbox_increase(control_t *p_ctrl);

/*!
   Decrease setbox

   \param[in] p_ctrl : setbox control
   \return : TRUE/FALSE
  */
BOOL sbar_decrease(control_t *p_ctrl);

/*!
   Set item content by unicode string.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[in] p_unistr : unicode string
   \return : TRUE or FALSE
  */
BOOL sbox_set_content_by_unistr(control_t *p_ctrl, u16 item_idx, u16 *p_unistr);

/*!
   Set item content by char string.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[in] p_ascstr : char string
   \return : TRUE or FALSE
  */
BOOL sbox_set_content_by_ascstr(control_t *p_ctrl, u16 item_idx, u8 *p_ascstr);

/*!
   Set item content by string id.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[in] strid : string id
   \return : NULL
  */
void sbox_set_content_by_strid(control_t *p_ctrl, u16 item_idx, u16 strid);

/*!
   Set item content by static string.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[in] p_extstr : static string buffer
   \return : NULL
  */
void sbox_set_content_by_extstr(control_t *p_ctrl, u16 item_idx, u32 p_extstr);

/*!
   Set item content with hex number.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[in] hex : hex number
   \return : NULL
  */
void sbox_set_content_by_hex(control_t *p_ctrl, u16 item_idx, s32 hex);

/*!
   Set item content with dec number.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[in] dec : number
   \return : NULL
  */
void sbox_set_content_by_dec(control_t *p_ctrl, u16 item_idx, s32 dec);

/*!
   Set string align style.

   \param[in] p_ctrl : setbox control
   \param[in] style : align style
   \return : NULL
  */
void sbox_set_align_type(control_t *p_ctrl, u32 style);

/*!
  sbox get align type
  */
u32 sbox_get_align_type(control_t *p_ctrl);

/*!
   Set content type.

   \param[in] p_ctrl : setbox control
   \param[in] type : content type
   \return : NULL
  */
void sbox_set_content_type(control_t *p_ctrl, u32 type);

/*!
   Get item context.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index.
   \return : item context.
  */
u32 sbox_get_item_context(control_t *p_ctrl, u8 item_idx);

/*!
   Set item context.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index.
   \param[in] context : context for set.
   \return : content.
  */
void sbox_set_item_context(control_t *p_ctrl, u8 item_idx, u32 context);

/*!
   Set string color

   \param[in] p_ctrl : setbox control
   \param[in] f_fstyle : focus fstyle
   \param[in] n_fstyle : normal fstyle
   \param[in] g_fstyle : gray fstyle
   \return : NULL
  */
void sbox_set_fstyle(control_t *p_ctrl,
  u32 f_fstyle, u32 n_fstyle, u32 g_fstyle);

/*!
  sbox get fstyle.
  */
void sbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive);

/*!
   Get string pos in item.

   \param[in] p_ctrl : setbox control
   \param[out] p_left : return string left
   \param[out] p_top : return string top
   \return : NULL
  */
void sbox_get_string_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top);

/*!
   Set string rect

   \param[in] p_ctrl : setbox control
   \param[in] str_left : left pos of string
   \param[in] str_top : top pos of string
   \return : NULL
  */
void sbox_set_string_offset(control_t *p_ctrl, u8 str_left, u8 str_top);

/*!
   Get item status.

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[out] p_left : return left
   \param[out] p_width : return width
   \return : NULL
  */
void sbox_get_item_status(control_t *p_ctrl,
  u8 item_idx, u16 *p_left, u16 *p_width);

/*!
   Set item status

   \param[in] p_ctrl : setbox control
   \param[in] item_idx : item index
   \param[in] left : left pos of item
   \param[in] width : width of item
   \return : NULL
  */
void sbox_set_item_status(control_t *p_ctrl,
  u8 item_idx, u16 left, u16 width);

/*!
   Control proc.

   \param[in] p_ctrl : setbox control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE sbox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif
#endif
