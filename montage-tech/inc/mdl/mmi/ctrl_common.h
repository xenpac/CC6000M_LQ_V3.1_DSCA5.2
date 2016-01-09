/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CTRL_COMMON_H__
#define __CTRL_COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif

/*!
   This function is only used for proc validation check.

   \param[in] p_ctrl : point to a control
   \param[in] proc   : proc function address.  

   \return : NULL.  
  */
void proc_check(control_t *p_ctrl, u32 proc);


/*!
   Message proc map entrys for control default proc.
  */
#define BEGIN_CTRLPROC(msgproc, base) \
  static const msgmap_entry_t msgproc ## _entrys[] = \
  {

/*!
   Message proc map member.
  */
#define ON_COMMAND(msg, p_proc) \
  { \
    msg, p_proc, 0 \
  },

/*!
   Memssage proc searching interface.
  */
#define END_CTRLPROC(msgproc, base) \
  { \
    MSG_INVALID, NULL, 0 \
  } \
  }; \
  static const msgproc_t msgproc##_base = base; \
  RET_CODE msgproc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2) \
  { \
    RET_CODE ret = SUCCESS; \
    proc_check(p_ctrl, (u32)msgproc); \
    ret = ctrl_on_msg(p_ctrl, msgproc ## _entrys, msg, para1, para2); \
    if(ret == ERR_NOFEATURE) \
    { \
      if(msgproc ## _base != NULL) \
      { \
        ret = msgproc ## _base(p_ctrl, msg, para1, para2); \
      } \
    } \
    return ret; \
 }

/*!
   \file ctrl_common.h

   This file defined some common used interfaces about controls for application
   layer modules.
  */

/*!
   Link child to parent.

   \param[in] p_parent : parent
   \param[in] p_child : child
   \return : NULL
  */
void ctrl_add_child(control_t *p_parent, control_t *p_child);

/*!
   Remove child from parent.

   \param[in] p_parent : parent
   \param[in] p_child : child
   \return : TRUE/FALSE
  */
BOOL ctrl_remove_child(control_t *p_parent, control_t *p_child);

/*!
   Init invalid region.

   \param[in] p_ctrl : control
   \return : NULL
  */
void ctrl_init_invrgn(control_t *p_ctrl);

/*!
   Empty invalid region.

   \param[in] p_ctrl : control
   \return : NULL
  */
void ctrl_empty_invrgn(control_t *p_ctrl);

/*!
   Add a rect into invalid region of a control.

   \param[in] p_ctrl : control
   \param[in] p_rc : input control rect
   \return : TRUE/FALSE
  */
BOOL ctrl_add_rect_to_invrgn(control_t *p_ctrl, const rect_t *p_rc);

/*!
   Subtract a rect from invalid region of a control.

   \param[in] p_ctrl : control
   \param[in] p_rc : input control rect
   \return : TRUE/FALSE
  */
BOOL ctrl_subtract_rect_from_invrgn(control_t *p_ctrl, const rect_t *p_rc);
/*!
   Empty invalid region by a rectangle.
   Notice, this function only effect the control itself.

   \param[in] p_ctrl : control
   \param[in] is_add : add a rectangle or not
   \param[in] p_rc : input control rect
   \return : TRUE/FALSE
  */
BOOL ctrl_update_invrgn_by_rect(control_t *p_ctrl, BOOL is_add, const rect_t *p_rc);


/*!
   Get control id by MSG_FOCUS_UP/MSG_FOCUS_DOWN/MSG_FOCUS_LEFT/MSG_FOCUS_RIGHT.

   \param[in] p_ctrl : control
   \param[in] msg : message
   \return : related id
  */
u16 ctrl_get_related_id(control_t *p_ctrl, u16 msg);

/*!
   Destroy all children control.

   \param[in] p_ctrl : control
   \return : NULL
  */
void ctrl_destroy_children(control_t *p_ctrl);


/*!
   Convert number to dec string.

   \param[in] p_buffer : unicode string buffer
   \param[in] number : number
   \return : NULL
  */
void convert_i_to_dec_str(u16 *p_buffer, s32 number);

/*!
   Convert number to hex string.

   \param[in] p_buffer : unicode string buffer
   \param[in] number : number
   \return : NULL
  */
void convert_i_to_hex_str(u16 *p_buffer, s32 number);

/*!
   Convert dec string to number.

   \param[in] p_buffer : unicode string buffer
   \return : number
  */
s32 convert_dec_str_to_i(u16 *p_buffer);

/*!
   Convert hex string to number.

   \param[in] p_buffer : unicode string buffer
   \return : number
  */
s32 convert_hex_str_to_i(u16 *p_buffer);

/*!
   Convert number to dec string with fixed string length.

   \param[in] p_buffer : unicode string buffer, length should be (length+2)
   \param[in] number : number
   \param[in] length : string length, 0 means auto size.
   \return : NULL.
  */
void convert_i_to_dec_str_ex(u16 *p_buffer, s32 number, u8 length);

/*!
   Convert number to hex string with fixed string length.

   \param[in] p_buffer : unicode string buffer , length should be (length+2)
   \param[in] number : number
   \param[in] length : string length, 0 means auto size
   \return : NULL
  */
void convert_i_to_hex_str_ex(u16 *p_buffer, s32 number, u8 length);

/*!
  Insert a char into the specificed string.

  \param[in] p_unistr : unicode string buffer
  \param[in] aschar : the char
  \param[in] pos : the place
  \param[in] len : string buffer length
  \return : NULL
  */
void insert_aschar_into_unistr(u16 *p_unistr, u8 aschar, u32 pos, u32 len);

#ifdef __cplusplus
}
#endif

#endif
