/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EBOX_CTRL_H__
#define __EBOX_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_editbox.h

   This file defined some interfaces about editbox control for application layer
   modules.
  */

/*!
   Editbox work type mask, totoally support 4 modes.
  */
#define EBOX_WORKTYPE_MASK     0x00000007
/*!
   Editbox work in edit type, show "abc_"
  */
#define EBOX_WORKTYPE_EDIT     0x00000000
/*!
   Editbox work in hide type, show "***"
  */
#define EBOX_WORKTYPE_HIDE     0x00000001
/*!
   Editbox work in shift type, show "a_c"
  */
#define EBOX_WORKTYPE_SHIFT    0x00000002
/*!
   Editbox work in shift type, show "abc"
  */
#define EBOX_WORKTYPE_NO_CURSOR    0x00000004
/*!
   Editbox highlight status mask, 0 is normal; 1 is edit
  */
#define EBOX_HL_STATUS_MASK    0x00000008
/*!
   Editbox arabic type mask, 0 is normal code; 1 is arabic code
  */
#define EBOX_ARABIC_MASK        0x00000010
/*!
   Editbox align type mask
  */
#define EBOX_ALIGN_MASK        0xF0000000
/*!
   Editbox string length
  */
#define EBOX_MAX_STRLEN        64

/*!
   Register editbox control class to system.

   \param[in] max_cnt : max editbox control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE ebox_register_class(u16 max_cnt);

/*!
   Set distance between lines.

   \param[in] p_ctrl : editbox control
   \param[in] l_space : distance between lines
   \return : NULL
  */
void ebox_set_line_gap(control_t *p_ctrl, u8 l_space);

/*!
   Set text content with char string.

   \param[in] p_ctrl : editbox control
   \param[in] p_ascstr : char string
   \return : NULL
  */
void ebox_set_content_by_ascstr(control_t *p_ctrl, char *p_ascstr);

/*!
   Set text content with unicode string.

   \param[in] p_ctrl : editbox control
   \param[in] p_unistr : unicode string
   \return : NULL
  */
void ebox_set_content_by_unistr(control_t *p_ctrl, u16 *p_unistr);

/*!
   Set text content with unicode string, for supporting keyboard.

   \param[in] p_ctrl : editbox control
   \param[in] p_unistr : unicode string
   \return : NULL
  */
void ebox_set_keyboard_content_by_unistr(control_t *p_ctrl, u16 *p_unistr);

/*!
  Set text color.

  \param[in] p_ctrl : editbox control
  \param[in] n_fstyle : normal fstyle
  \param[in] h_fstyle : high fstyle
  \param[in] g_fstyle : gray fstyle
  \return : NULL
  */
void ebox_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle);

/*!
  ebox get font style.
  */
void ebox_get_fstyle(control_t *p_ctrl, u32 *p_active, u32 *p_hlight, u32 *p_inactive);

/*!
   Set text position.

   \param[in] p_ctrl : editbox control
   \param[in] left : left shift of string
   \param[in] top : top shift of string
   \return : NULL
  */
void ebox_set_offset(control_t *p_ctrl, u16 left, u16 top);

/*!
  ebox get offset.
  */
void ebox_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top);

/*!
   Set text maxiamal length.

   \param[in] p_ctrl : editbox control
   \param[in] max : the maxiam length
   \return : NULL
  */
void ebox_set_maxtext(control_t *p_ctrl, u16 max);

/*!
   Get text maxiamal length.

   \param[in] p_ctrl : editbox control
   \return : the maxiam length
  */
u16 ebox_get_maxtext(control_t *p_ctrl);

/*!
  Set work mode.

  \param[in] p_ctrl : editbox control
  \param[in] worktype : work type
  \return : NULL
  */
void ebox_set_worktype(control_t *p_ctrl, u32 worktype);

/*!
  Set hide mask.

  \param[in] p_ctrl : editbox control
  \param[in] bef_mask : before mask, ascii code
  \param[in] aft_mask : after mask.
  \return : success or failure
  */
BOOL ebox_set_hide_mask(control_t *p_ctrl, u8 bef_mask, u8 aft_mask);

/*!
   Set align style.

   \param[in] p_ctrl : editbox control
   \param[in] style : text align style
   \return : NULL
  */
void ebox_set_align_type(control_t *p_ctrl, u32 style);

/*!
  ebox get align type.
  */
u32 ebox_get_align_type(control_t *p_ctrl);

/*!
   Set arabic code style.

   \param[in] p_ctrl : editbox control
   \param[in] style :  0:normal code; 1:arabic code (arrange from right to left)
   \return : NULL
  */
void ebox_set_arabic_type(control_t *p_ctrl, u32 style);
/*!
   Get text content.

   \param[in] p_ctrl : editbox control
   \return : unicode string
  */
u16 *ebox_get_content(control_t *p_ctrl);

/*!
   Enter edit status.

   \param[in] p_ctrl : editbox control
   \return : TRUE for success and FALSE for fail
  */
BOOL ebox_enter_edit(control_t *p_ctrl);

/*!
   Exit edit status.

   \param[in] p_ctrl : editbox control
   \return : TRUE for success and FALSE for fail
  */
BOOL ebox_exit_edit(control_t *p_ctrl);

/*!
   Back space.

   \param[in] p_ctrl : editbox control
   \return : TRUE for success and FALSE for fail
  */
BOOL ebox_back_space(control_t *p_ctrl);

/*!
  Input char.

  \param[in] p_ctrl : editbox control
  \param[in] msg : message
  \return : if success, return TRUE, else return FALSE.
  */
BOOL ebox_input_char(control_t *p_ctrl, u16 msg);

/*!
  Input wchar.

  \param[in] p_ctrl : editbox control
  \param[in] uchar : uchar
  \return : if success, return TRUE, else return FALSE.
  */
BOOL ebox_input_uchar(control_t *p_ctrl, u16 uchar);

/*!
  Input keyboard unicode.

  \param[in] p_ctrl : editbox control
  \param[in] uni_code :  unicode
  \return : if success, return TRUE, else return FALSE.
  */
BOOL ebox_input_keyboard_uni(control_t *p_ctrl, u16 uni_code);

/*!
   Clean the edit string.

   \param[in] p_ctrl : editbox control
   \return : TRUE for success and FALSE for fail
  */
BOOL ebox_empty_content(control_t *p_ctrl);

/*!
   move the cursor.

   \param[in] p_ctrl : editbox control
   \param[in] offset : move  number
   \return : TRUE for success and FALSE for fail
  */
BOOL ebox_shift_focus(control_t *p_ctrl, s8 offset);


/*!
   Control proc.

   \param[in] p_ctrl : editbox control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE ebox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif

#endif
