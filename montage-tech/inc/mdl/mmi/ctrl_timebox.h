/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TBOX_CTRL_H__
#define __TBOX_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_timebox.h

   This file defined some interfaces of timebox control for application layer
   modules.
  */

/*!
   Specify max item number of tbox, (year, month, day, hour, minute, second).
  */
#define TBOX_MAX_ITEM_NUM      6

/*!
   Tbox separator type mask.
  */
#define TBOX_SEPARATOR_TYPE_MASK    0x00000030
/*!
   Specify tbox separator in external string type.
  */
#define TBOX_SEPARATOR_TYPE_EXTSTR  0x00000000
/*!
   Specify tbox separator in bitmap id type.
  */
#define TBOX_SEPARATOR_TYPE_BMPID   0x00000010
/*!
   Specify tbox separator in string id type.
  */
#define TBOX_SEPARATOR_TYPE_STRID   0x00000020
/*!
   Specify tbox separator in unicode type.
  */
#define TBOX_SEPARATOR_TYPE_UNICODE 0x00000030

/*!
   Tbox type mask
  */
#define TBOX_TIME_MASK              0x00000FC0
/*!
   Identify for year,(used for set tbox type).
  */
#define TBOX_YEAR                   0x00000040
/*!
   Identify for month,(used for set tbox type).
  */
#define TBOX_MONTH                  0x00000080
/*!
   Identify for day,(used for set tbox type).
  */
#define TBOX_DAY                    0x00000100
/*!
   Identify for hour,(used for set tbox type).
  */
#define TBOX_HOUR                   0x00000200
/*!
   Identify for minute,(used for set tbox type).
  */
#define TBOX_MIN                    0x00000400
/*!
   Identify for second,(used for set tbox type).
  */
#define TBOX_SECOND                 0x00000800

/*!
  timebox item
  */
typedef enum
{
  /*!
     Identify for year item bit.
    */
  TBOX_ITEM_YEAR = 0,
  /*!
     Identify for month item bit.
    */
  TBOX_ITEM_MONTH,
  /*!
     Identify for day item bit.
    */
  TBOX_ITEM_DAY,
  /*!
     Identify for hour item bit.
    */
  TBOX_ITEM_HOUR,
  /*!
     Identify for minute item bit.
    */
  TBOX_ITEM_MIN,
  /*!
     Identify for second item bit.
    */
  TBOX_ITEM_SECOND,
}tbox_item_t;

/*!
  time style
  */
typedef enum
{
  /*!
    time style YMD-HMS
    */
  TIME_STYLE_YMD_HMS = 0,
  /*!
    time style DMY-HMS
    */
  TIME_STYLE_DMY_HMS,    
}time_style_t;

/*!
   Default value for max number width.
  */
#define TBOX_DEFAULT_MAX_NUM_WIDTH 12

/*!
   Time box highlight status mask, 0 is normal, 1 is edit.
  */
#define TBOX_HL_STATUS_MASK        0x00000008
/*!
   Time box align type mask
  */
#define TBOX_ALIGN_MASK            0xF0000000

/*!
   Set time type for tbox.

   \param[in] p_ctrl : tbox control.
   \param[in] type : type for set, refer to TBOX_TIME_MASK
   \return : NULL
  */
void tbox_set_time_type(control_t *p_ctrl, u32 type);

/*!
   To judge is the time box on edit.

   \param[in] p_ctrl  : time box control pointer.
   \return  : is the time box on edit or not.
  */
BOOL tbox_is_on_edit(control_t *p_ctrl);


/*!
  To judge is the number box outrange.

  \param[in] p_ctrl       : number box control pointer.
  \return : is the control out range or not.
  */
BOOL tbox_is_invalid_time(control_t *p_ctrl);


/*!
   Register nbox control to system.

   \param[in] max_cnt : max nbox control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE tbox_register_class(u16 max_cnt);

/*!
   Set separator type.

   \param[in] p_ctrl : nbox control.
   \param[in] type : separator type.
   \return : NULL
  */
void tbox_set_separator_type(control_t *p_ctrl, u32 type);

/*!
   Set separator by string id.

   \param[in] p_ctrl : tbox control.
   \param[in] index : tbox separator index.
   \param[in] strid : string id for set.
   \return : True for success, else return false.
  */
BOOL tbox_set_separator_by_strid(control_t *p_ctrl, u8 index, u16 strid);

/*!
   Set separator by external character.

   \param[in] p_ctrl : tbox control.
   \param[in] index : tbox separator index.
   \param[in] p_str : external string for set.
   \return : True for success, else return false.
  */
BOOL tbox_set_separator_by_extstr(control_t *p_ctrl, u8 index, u16 *p_str);

/*!
   Set separator by bitmap id.

   \param[in] p_ctrl : tbox control.
   \param[in] index : tbox separator index.
   \param[in] bmpid : bmp id for set.
   \return : True for success, else return false.
  */
BOOL tbox_set_separator_by_bmpid(control_t *p_ctrl, u8 index, u16 bmpid);

/*!
   Set separator by unicode character.

   \param[in] p_ctrl : tbox control.
   \param[in] index : tbox separator index.
   \param[in] uni_char : unicode char for set.
   \return : True for success, else return false.
  */
BOOL tbox_set_separator_by_unichar(control_t *p_ctrl, u8 index, u16 uni_char);

/*!
   Set separator by ascII character.

   \param[in] p_ctrl : tbox control.
   \param[in] index : tbox separator index.
   \param[in] asc_char : ascii char for set.
   \return : True for success, else return false.
  */
BOOL tbox_set_separator_by_ascchar(control_t *p_ctrl, u8 index, u8 asc_char);

/*!
  Get separator.

  \param[in] p_ctrl : tbox control.
  \param[in] index  : index
  \return : separator
  */
u32 tbox_get_separator(control_t *p_ctrl, u8 index);

/*!
   Set font style of time box.

   \param[in] p_ctrl : tbox control.
   \param[in] n_fstyle : normal fstyle.
   \param[in] h_fstyle : highlight fstyle.
   \param[in] g_fstyle : gray fstyle.
   \return : NULL
  */
void tbox_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle);

/*!
  tbox get fstyle.
  */
void tbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive);

/*!
   Set number string offset, reference is textfield rectangle.

   \param[in] p_ctrl : tbox control.
   \param[in] left : left offset, distance from left side of textfield to left
   side of string.
   \param[in] top : top offset, distance from top side of textfield to top side
   of string.
   \return : NULL
  */
void tbox_set_offset(control_t *p_ctrl, u16 left, u16 top);

/*!
  get offset.
  */
void tbox_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top);


/*!
   Set number string align type.

   \param[in] p_ctrl : tbox control.
   \param[in] style : number string align type. STL_LEFT,.....
   \return : NULL
  */
void tbox_set_align_type(control_t *p_ctrl, u32 style);

/*!
  get align type.
  */
u32 tbox_get_align_type(control_t *p_ctrl);

/*!
   Enter tbox for edit.

   \param[in] p_ctrl : tbox control.
   \return : TRUE for success.
  */
BOOL tbox_enter_edit(control_t *p_ctrl);

/*!
   Exit tbox.

   \param[in] p_ctrl : tbox control.
   \return : NULL
  */
void tbox_exit_edit(control_t *p_ctrl);

/*!
   Input character in a tbox.

   \param[in] p_ctrl : tbox control.
   \param[in] msg : character message, it contains the information of which
   number is input.
   \return : TRUE for success.
  */
BOOL tbox_input_number(control_t *p_ctrl, u16 msg);

/*!
   Set focus bit of nbox.

   \param[in] p_ctrl : tbox control.
   \param[in] bit : focus bit to set
   \param[in] item : focus item  
   \return : NULL
  */
void tbox_set_focus(control_t *p_ctrl, u8 bit, tbox_item_t item);

/*!
   Get current focus bit, it specified it which bit is edit now.

   \param[in] p_ctrl : tbox control.
   \param[out] p_bit : focus bit.
   \param[out] p_item : focus item.
   \return : current focus.
  */
void tbox_get_focus(control_t *p_ctrl, u8 *p_bit, tbox_item_t *p_item);

/*!
   Change current focus.

   \param[in] p_ctrl : tbox control.
   \param[in] offset : focus offset.
   \return : TRUE for success, else return FALSE.
  */
BOOL tbox_shift_focus(control_t *p_ctrl, s8 offset);

/*!
   Tbox message process function.

   \param[in] p_ctrl : tbox control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : NULL
  */
RET_CODE tbox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
   Tbox set time.
   \param[in] p_ctrl : tbox control.
   \param[in] p_time : time for set.
   \return : NULL
  */
void tbox_set_time(control_t *p_ctrl, utc_time_t *p_time);

/*!
  Tbox get time.

  \param[in] p_ctrl : tbox control.
  \param[out] p_time : tbox time.
  \return : NULL
  */
void tbox_get_time(control_t *p_ctrl, utc_time_t *p_time);

/*!
   Tbox set max number width

   \param[in] p_ctrl : tbox control.
   \param[in] width : max number width.
   \return : NULL
  */
void tbox_set_max_num_width(control_t *p_ctrl, u8 width);

/*!
   Tbox get max number width(pixel)

   \param[in] p_ctrl : tbox control.
   \return : max number width.
  */
u8 tbox_get_max_num_width(control_t *p_ctrl);

/*!
  set time style.
  */
void tbox_set_style(control_t *p_ctrl, time_style_t style);
#ifdef  __cplusplus
}
#endif

#endif
