/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NBOX_CTRL_H__
#define __NBOX_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_numbox.h

   This file defined some interfaces of numbox control for application layer
   modules.
  */

/*!
   Number type mask of nbox, totally support 2 modes.
  */
#define NBOX_NUMTYPE_MASK                0x00000007
/*!
   Set number type of nbox in decimal type.
  */
#define NBOX_NUMTYPE_DEC                 0x00000000
/*!
   Set number type of nbox in hex type.
  */
#define NBOX_NUMTYPE_HEX                 0x00000001

/*!
   Number box highlight status mask, 0 is normal, 1 is edit.
  */
#define NBOX_HL_STATUS_MASK              0x00000008

/*!
  Number box highlight type mask, 0 is underline, 1 is invert. 
  */
#define NBOX_HL_TYPE_MASK                    0x00000010

/*!
   Number box align type mask
  */
#define NBOX_ALIGN_MASK                  0xF0000000

/*!
   Nbox postfix string length
  */
#define NBOX_ITEM_NUM_POSTFIX_LENGTH     10

/*!
   Nbox prefix type mask
  */
#define NBOX_ITEM_PREFIX_TYPE_MASK       0x0000C000
/*!
   Set nbox prefix by string id
  */
#define NBOX_ITEM_PREFIX_TYPE_STRID      0x00000000
/*!
   Set nbox prefix by static unicode
  */
#define NBOX_ITEM_PREFIX_TYPE_EXTSTR     0x00004000

/*!
   Nbox postfix type mask
  */
#define NBOX_ITEM_POSTFIX_TYPE_MASK      0x00003000
/*!
   Set nbox postfix by string id
  */
#define NBOX_ITEM_POSTFIX_TYPE_STRID     0x00000000
/*!
   Set nbox postfix by static unicode
  */
#define NBOX_ITEM_POSTFIX_TYPE_EXTSTR    0x00001000

/*!
   Nbox prefix&postfix string length
  */
#define NBOX_ITEM_NUM_XFIX_LENGTH        10

/*!
   To judge is the number box on edit.

   \param[in] p_ctrl : number box control pointer.
   \return  : is the number box on edit or not.
  */
BOOL nbox_is_on_edit(control_t *p_ctrl);


/*!
   To judge is the number box outrange.

   \param[in] p_ctrl : number box control pointer.
   \param[in] p_border : legel border.
   \return : is the control out range or not.
  */
BOOL nbox_is_outrange(control_t *p_ctrl, u32 *p_border);


/*!
   Register nbox control to system.

   \param[in] max_cnt : max nbox control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE nbox_register_class(u16 max_cnt);

/*!
  Set total bit length of nbox.

  \param[in] p_ctrl : nbox control.
  \param[in] min : min value
  \param[in] max : max value
  \param[in] bit_length : total bit length.
  \return : NULL
  */
void nbox_set_range(control_t *p_ctrl, s32 min, s32 max, u8 bit_length);

/*!
  Get total bit length of nbox.

  \param[in] p_ctrl : nbox control.
  \param[out] p_min : min.
  \param[out] p_max : max.
  \return : NULL
  */
void nbox_get_range(control_t *p_ctrl, s32 *p_min, s32 *p_max);


/*!
  Set separator places.

  \param[in] p_ctrl : nbox control.
  \param[in] pos : place.
  \return : NULL
  */
void nbox_set_separator_pos(control_t *p_ctrl, u8 pos);

/*!
   Get separator places.

   \param[in] p_ctrl : nbox control.
   \return : places
  */
u8 nbox_get_separator_pos(control_t *p_ctrl);

/*!
  Set separator.

  \param[in] p_ctrl : nbox control.
  \param[in] separator : separator.
  \return : NULL
  */
void nbox_set_separator(control_t *p_ctrl, u8 separator);

/*!
   Get separator.

   \param[in] p_ctrl : nbox control.
   \return : separator
  */
u8 nbox_get_separator(control_t *p_ctrl);

/*!
   Set nbox number by decimal.

   \param[in] p_ctrl : nbox control.
   \param[in] dec : number of nbox(dec).
   \return : NULL
  */
void nbox_set_num_by_dec(control_t *p_ctrl, u32 dec);

/*!
   Set nbox number by hex.

   \param[in] p_ctrl : nbox control.
   \param[in] hex : number of nbox (hex).
   \return : NULL
  */
void nbox_set_num_by_hex(control_t *p_ctrl, u32 hex);

/*!
   Get number of nbox.

   \param[in] p_ctrl : nbox control.
   \return : number value of nbox.
  */
u32 nbox_get_num(control_t *p_ctrl);

/*!
   Set font style of number string.

   \param[in] p_ctrl : nbox control.
   \param[in] n_fstyle : normal fstyle.
   \param[in] h_fstyle : highlight fstyle.
   \param[in] g_fstyle : gray fstyle.
   \return : NULL
  */
void nbox_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle);

/*!
  nbox get font style.
  */
void nbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive);

/*!
   Set number string offset, reference is textfield rectangle.

   \param[in] p_ctrl : nbox control.
   \param[in] left : left offset, distance from left side of textfield to left
   side of string.
   \param[in] top : top offset, distance from top side of textfield to top side
   of string.
   \return : NULL
  */
void nbox_set_offset(control_t *p_ctrl, u16 left, u16 top);

/*!
  nbox get offset
  */
void nbox_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top);

/*!
   Set number string type of nbox.

   \param[in] p_ctrl : nbox control.
   \param[in] type : number string type. decimal, hex,.....
   \return : NULL
  */
void nbox_set_num_type(control_t *p_ctrl, u32 type);

/*!
   Set number string align type.

   \param[in] p_ctrl : nbox control.
   \param[in] style : number string align type. STL_LEFT,.....
   \return : NULL
  */
void nbox_set_align_type(control_t *p_ctrl, u32 style);

/*!
  nbox get align type.
  */
u32 nbox_get_align_type(control_t *p_ctrl);

/*!
   Enter nbox for edit.

   \param[in] p_ctrl : nbox control.
   \param[in] is_left : focus on first/last bit
   \return : TRUE for success.
  */
BOOL nbox_enter_edit(control_t *p_ctrl, BOOL is_left);

/*!
   Exit nbox.

   \param[in] p_ctrl : nbox control.
   \return : NULL
  */
void nbox_exit_edit(control_t *p_ctrl);

/*!
   Input character in a nbox.

   \param[in] p_ctrl : nbox control.
   \param[in] msg : character message, it contains the information of which
   number is input.
   \return : TRUE for success.
  */
BOOL nbox_input_num(control_t *p_ctrl, u16 msg);

/*!
   Input character in a nbox, but don't move right.

   \param[in] p_ctrl : nbox control.
   \param[in] msg : character message, it contains the information of which
   number is input.
   \return : TRUE for success.
  */
BOOL nbox_input_num_ex(control_t *p_ctrl, u16 msg);

/*!
   Set focus bit of nbox.

   \param[in] p_ctrl : nbox control.
   \param[in] focus : it specified which bit will be edit.
   \return : NULL
  */
BOOL nbox_set_focus(control_t *p_ctrl, u8 focus);

/*!
   Get current focus bit, it specified it which bit is edit now.

   \param[in] p_ctrl : nbox control.
   \return : current focus.
  */
u8 nbox_get_focus(control_t *p_ctrl);

/*!
   Change current focus.

   \param[in] p_ctrl : nbox control.
   \param[in] offset : focus offset.
   \return : TRUE for success, else return FALSE.
  */
BOOL nbox_shift_focus(control_t *p_ctrl, s8 offset);

/*!
   Set prefix type.

   \param[in] p_ctrl : point to a numbox control.
   \param[in] type : prefix or postfix.
   \return : NULL.
  */
void nbox_set_prefix_type(control_t *p_ctrl, u32 type);

/*!
   Set prefix of a numbox by string id.

   \param[in] p_ctrl : point to a numbox control.
   \param[in] strid : string id.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL nbox_set_prefix_by_strid(control_t *p_ctrl, u16 strid);

/*!
   Set postfix of a numbox by string id.

   \param[in] p_ctrl : point to a numbox control.
   \param[in] strid : string id.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL nbox_set_postfix_by_strid(control_t *p_ctrl, u16 strid);

/*!
   Set postfix type.

   \param[in] p_ctrl : point to a numbox control.
   \param[in] type : prefix or postfix.
   \return : NULL.
  */
void nbox_set_postfix_type(control_t *p_ctrl, u32 type);

/*!
   Set the prefix of a numbox by static unicode string.

   \param[in] p_ctrl : point to a numbox control.
   \param[in] p_extstr : static unicode string.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL nbox_set_prefix_by_extstr(control_t *p_ctrl, u16 *p_extstr);

/*!
   Set the postfix of a numbox by static unicode string.

   \param[in] p_ctrl : point to a numbox control.
   \param[in] p_extstr : static unicode string.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL nbox_set_postfix_by_extstr(control_t *p_ctrl, u16 *p_extstr);

/*!
   Get the postfix of a numbox.

   \param[in] p_ctrl : point to a numbox control.
   \return : the prefix.
  */
u32 nbox_get_pretfix(control_t *p_ctrl);

/*!
   Get the postfix of a numbox.

   \param[in] p_ctrl : point to a numbox control.
   \return : the postfix.
  */
u32 nbox_get_postfix(control_t *p_ctrl);

/*!
   Nbox message process function.

   \param[in] p_ctrl : nbox control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : NULL
  */
RET_CODE nbox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
   Set highlight style.

   \param[in] p_ctrl : point to a numbox control.
   \param[in] is_underline : is underline type or invert type.
   \return : NULL
  */
void nbox_set_highlight_type(control_t *p_ctrl, BOOL is_underline);

/*!
  nbox is underline.
  */
BOOL nbox_is_underline(control_t *p_ctrl);

#ifdef __cplusplus
}
#endif

#endif
