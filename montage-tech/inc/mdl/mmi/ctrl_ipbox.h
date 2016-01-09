/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __IPBOX_CTRL_H__
#define __IPBOX_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_ipbox.h

   This file defined some interfaces of ipbox control for application layer
   modules.
  */

/*!
   Specify max item number of ipbox, ip address 4 parts (s_b1. s_b2. s_b3. s_b4, like 192.168.1.1).
  */
#define IPBOX_MAX_ITEM_NUM      4

/*!
   IPbox separator type mask.
  */
#define IPBOX_SEPARATOR_TYPE_MASK    0x00000030
/*!
   Specify ipbox separator in external string type.
  */
#define IPBOX_SEPARATOR_TYPE_EXTSTR  0x00000000
/*!
   Specify ipbox separator in bitmap id type.
  */
#define IPBOX_SEPARATOR_TYPE_BMPID   0x00000010
/*!
   Specify ipbox separator in string id type.
  */
#define IPBOX_SEPARATOR_TYPE_STRID   0x00000020
/*!
   Specify ipbox separator in unicode type.
  */
#define IPBOX_SEPARATOR_TYPE_UNICODE 0x00000030

/*!
   IPbox type mask
  */
#define IPBOX_ADDRESS_MASK              0x00000FC0
/*!
   Identify for s_b1,(used for set ipbox type).
  */
#define IPBOX_S_B1                   0x00000040
/*!
   Identify for s_b2,(used for set ipbox type).
  */
#define IPBOX_S_B2                  0x00000080
/*!
   Identify for s_b3,(used for set ipbox type).
  */
#define IPBOX_S_B3                    0x00000100
/*!
   Identify for s_b4,(used for set ipbox type).
  */
#define IPBOX_S_B4                   0x00000200


/*!
   Identify for s_b1 item bit.
  */
#define IPBOX_ITEM_S_B1              0
/*!
   Identify for s_b2 item bit.
  */
#define IPBOX_ITEM_S_B2             1
/*!
   Identify for s_b3 item bit.
  */
#define IPBOX_ITEM_S_B3               2
/*!
   Identify for s_b4 item bit.
  */
#define IPBOX_ITEM_S_B4              3



/*!
   Default value for max number width.
  */
#define IPBOX_DEFAULT_MAX_NUM_WIDTH 12

/*!
   IP box highlight status mask, 0 is normal, 1 is edit.
  */
#define IPBOX_HL_STATUS_MASK        0x00000008
/*!
   IP box align type mask
  */
#define IPBOX_ALIGN_MASK            0xF0000000

/*! 
  UTC format ip address definition
  */
typedef struct
{
  /*!
    s_b1
    */
  u16 s_b1;
  /*!
    s_b2
    */
  u16 s_b2;
  /*!
    s_b3
    */
  u16 s_b3;
  /*!
    s_b4
    */
  u16 s_b4;
  /*!
    researved.
    */
  u16 reserved;
} ip_address_t;

/*!
   Set ip type for ipbox.

   \param[in] p_ctrl : ipbox control.
   \param[in] type : type for set, refer to IPBOX_ADDRESS_MASK
   \return : NULL
  */
void ipbox_set_address_type(control_t *p_ctrl, u32 type);

/*!
   To judge is the ip box on edit.

   \param[in] p_ctrl  : ip box control pointer.
   \return  : is the ip box on edit or not.
  */
BOOL ipbox_is_on_edit(control_t *p_ctrl);


/*!
  To judge is the number box outrange.

  \param[in] p_ctrl       : number box control pointer.
  \return : is the control out range or not.
  */
BOOL ipbox_is_invalid_address(control_t *p_ctrl);


/*!
   Register nbox control to system.

   \param[in] max_cnt : max nbox control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE ipbox_register_class(u16 max_cnt);

/*!
   Set separator type.

   \param[in] p_ctrl : nbox control.
   \param[in] type : separator type.
   \return : NULL
  */
void ipbox_set_separator_type(control_t *p_ctrl, u32 type);

/*!
   Set separator by string id.

   \param[in] p_ctrl : ipbox control.
   \param[in] index : ipbox separator index.
   \param[in] strid : string id for set.
   \return : True for success, else return false.
  */
BOOL ipbox_set_separator_by_strid(control_t *p_ctrl, u8 index, u16 strid);

/*!
   Set separator by external character.

   \param[in] p_ctrl : ipbox control.
   \param[in] index : ipbox separator index.
   \param[in] p_str : external string for set.
   \return : True for success, else return false.
  */
BOOL ipbox_set_separator_by_extstr(control_t *p_ctrl, u8 index, u16 *p_str);

/*!
   Set separator by bitmap id.

   \param[in] p_ctrl : ipbox control.
   \param[in] index : ipbox separator index.
   \param[in] bmpid : bmp id for set.
   \return : True for success, else return false.
  */
BOOL ipbox_set_separator_by_bmpid(control_t *p_ctrl, u8 index, u16 bmpid);

/*!
   Set separator by unicode character.

   \param[in] p_ctrl : ipbox control.
   \param[in] index : ipbox separator index.
   \param[in] uni_char : unicode char for set.
   \return : True for success, else return false.
  */
BOOL ipbox_set_separator_by_unichar(control_t *p_ctrl, u8 index, u16 uni_char);

/*!
   Set separator by ascII character.

   \param[in] p_ctrl : ipbox control.
   \param[in] index : ipbox separator index.
   \param[in] asc_char : ascii char for set.
   \return : True for success, else return false.
  */
BOOL ipbox_set_separator_by_ascchar(control_t *p_ctrl, u8 index, u8 asc_char);

/*!
  Get separator.

  \param[in] p_ctrl : ipbox control.
  \param[in] index  : index
  \return : separator
  */
u32 ipbox_get_separator(control_t *p_ctrl, u8 index);

/*!
   Set font style of ip box.

   \param[in] p_ctrl : ipbox control.
   \param[in] n_fstyle : normal fstyle.
   \param[in] h_fstyle : highlight fstyle.
   \param[in] g_fstyle : gray fstyle.
   \return : NULL
  */
void ipbox_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle);

/*!
  get font style.
  */
void ipbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive);

/*!
   Set number string offset, reference is textfield rectangle.

   \param[in] p_ctrl : ipbox control.
   \param[in] left : left offset, distance from left side of textfield to left
   side of string.
   \param[in] top : top offset, distance from top side of textfield to top side
   of string.
   \return : NULL
  */
void ipbox_set_offset(control_t *p_ctrl, u16 left, u16 top);

/*!
  ipbox get offset.
  */
void ipbox_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top);

/*!
   Set number string align type.

   \param[in] p_ctrl : ipbox control.
   \param[in] style : number string align type. STL_LEFT,.....
   \return : NULL
  */
void ipbox_set_align_type(control_t *p_ctrl, u32 style);

/*!
  get align type.
  */
u32 ipbox_get_align_type(control_t *p_ctrl);

/*!
   Enter ipbox for edit.

   \param[in] p_ctrl : ipbox control.
   \return : TRUE for success.
  */
BOOL ipbox_enter_edit(control_t *p_ctrl);

/*!
   Exit ipbox.

   \param[in] p_ctrl : ipbox control.
   \return : NULL
  */
void ipbox_exit_edit(control_t *p_ctrl);

/*!
   Input character in a ipbox.

   \param[in] p_ctrl : ipbox control.
   \param[in] msg : character message, it contains the information of which
   number is input.
   \return : TRUE for success.
  */
BOOL ipbox_input_number(control_t *p_ctrl, u16 msg);

/*!
   Set focus bit of nbox.

   \param[in] p_ctrl : ipbox control.
   \param[in] bit : focus bit to set
   \param[in] item : focus item  
   \return : NULL
  */
BOOL ipbox_set_focus(control_t *p_ctrl, u8 bit, u8 item);

/*!
   Get current focus bit, it specified it which bit is edit now.

   \param[in] p_ctrl : ipbox control.
   \param[out] p_bit : focus bit.
   \param[out] p_item : focus item.
   \return : current focus.
  */
void ipbox_get_focus(control_t *p_ctrl, u8 *p_bit, u8 *p_item);

/*!
   Change current focus.

   \param[in] p_ctrl : ipbox control.
   \param[in] offset : focus offset.
   \return : TRUE for success, else return FALSE.
  */
BOOL ipbox_shift_focus(control_t *p_ctrl, s8 offset);

/*!
   Tbox message process function.

   \param[in] p_ctrl : ipbox control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : NULL
  */
RET_CODE ipbox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
   IPbox set address.
   \param[in] p_ctrl : ipbox control.
   \param[in] p_address : address for set.
   \return : NULL
  */
void ipbox_set_address(control_t *p_ctrl, ip_address_t *p_address);

/*!
  IPbox get address.

  \param[in] p_ctrl : ipbox control.
  \param[out] p_address : ipbox address.
  \return : NULL
  */
void ipbox_get_address(control_t *p_ctrl, ip_address_t *p_address);

/*!
   Tbox set max number width

   \param[in] p_ctrl : ipbox control.
   \param[in] width : max number width.
   \return : NULL
  */
void ipbox_set_max_num_width(control_t *p_ctrl, u8 width);

/*!
   Tbox get max number width(pixel)

   \param[in] p_ctrl : ipbox control.
   \return : max number width.
  */
u8 ipbox_get_max_num_width(control_t *p_ctrl);

#ifdef __cplusplus
}
#endif

#endif
