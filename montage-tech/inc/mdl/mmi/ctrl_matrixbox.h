/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MBOX_CTRL_H__
#define __MBOX_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_matrixbox.h

   This file defined some interfaces of matrixbox control for application layer
   modules.
  */

/*!
   String align type mask of matrixbox control
  */
#define MBOX_STR_ALIGN_MASK     0xF0000000
/*!
   Icon align type mask of matrixbox control
  */
#define MBOX_ICON_ALIGN_MASK    0x0F000000
/*!
   String content type mask
  */
#define MBOX_STRTYPE_MASK       0x00F00001
/*!
   Set string content by unicode string
  */
#define MBOX_STRTYPE_UNICODE    0x00000001
/*!
   Set string content by static unicode string
  */
#define MBOX_STRTYPE_EXTSTR     0x00100001
/*!
   Set string content by string id
  */
#define MBOX_STRTYPE_STRID      0x00200001
/*!
   Set string content by decimal number
  */
#define MBOX_STRTYPE_DEC        0x00300001
/*!
   Set string content by hexadecimal number
  */
#define MBOX_STRTYPE_HEX        0x00400001

/*!
   Enable or disable string mode
  */
#define MBOX_STRING_MODE        0x00000001
/*!
   Enable or disable string mode
  */
#define MBOX_ICON_MODE          0x00000002


/*!
   The status of mbox item
  */
typedef enum
{
  /*!
     Mbox item normal
    */
  MBOX_ITEM_NORMAL = 0,
  /*!
     Mbox item disableed
    */
  MBOX_ITEM_DISABLED
}mbox_item_status_t;
 
/*!
   Register matrixbox control class to system.

   \param[in] max_cnt : max matrixbox control number
   \return : SUCCESS or ERR_FAILURE
  */
s32 mbox_register_class(u16 max_cnt);

/*!
   Draw an item

   \param[in] p_ctrl : matrixbox control
   \param[in] is_force : update invalid region before draw
   \param[in] index : item index
   \return : NULL
  */
void mbox_draw_item_ext(control_t *p_ctrl, u16 index, BOOL is_force);

/*!
   Set string color

   \param[in] p_ctrl : matrixbox control
   \param[in] f_fstyle : focus fstyle
   \param[in] n_fstyle : normal fstyle
   \param[in] g_fstyle : gray fstyle
   \return : NULL
  */
void mbox_set_string_fstyle(control_t *p_ctrl,
                            u32 f_fstyle,
                            u32 n_fstyle,
                            u32 g_fstyle);

/*!
  get string fstyle.
  */
void mbox_get_string_fstyle(control_t *p_ctrl,
  u32 *p_factive, u32 *p_fhlight, u32 *p_finactive);

/*!
   Set item color

   \param[in] p_ctrl : matrixbox control
   \param[in] f_rstyle : focus fstyle
   \param[in] n_rstyle : normal fstyle
   \param[in] g_rstyle : gray fstyle
   \return : NULL
  */
void mbox_set_item_rstyle(control_t *p_ctrl,
                          u32 f_rstyle,
                          u32 n_rstyle,
                          u32 g_rstyle);

/*!
  mbox get item rstyle.
  */
void mbox_get_item_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_rstyle);

/*!
  Set item rect

  \param[in] p_ctrl : matrixbox control
  \param[in] h_interval : horizontal interval
  \param[in] v_interval : vertical interval
  \return : NULL
  */
void mbox_set_item_interval(control_t *p_ctrl, u8 h_interval, u8 v_interval);

/*!
  get item interval.
  */
void mbox_get_item_interval(control_t *p_ctrl, u8 *p_h_interval, u8 *p_v_interval);

/*!
   Set string rect

   \param[in] p_ctrl : matrixbox control
   \param[in] str_left : left pos of string
   \param[in] str_top : top pos of string
   \return : NULL
  */
void mbox_set_string_offset(control_t *p_ctrl, u8 str_left, u8 str_top);

/*!
  set window size.

  \param[in] p_ctrl : matrixbox control
  \param[in] win_col : window column.
  \param[in] win_row : window row.
  
  \return : NULL
  */
void mbox_set_win(control_t *p_ctrl, u16 win_col, u16 win_row);

/*!
  Get window size.

  \param[in] p_ctrl : matrixbox control
  \param[in] p_win_col : col number
  \param[in] p_win_row : row number
  
  \return : NULL
  */
void mbox_get_win(control_t *p_ctrl, u16 *p_win_col, u16 *p_win_row);

/*!
   Set count.

   \param[in] p_ctrl : matrixbox control
   \param[in] total : total number
   \param[in] col : col number
   \param[in] row : row number
   \return : TRUE/FALSE
  */
BOOL mbox_set_count(control_t *p_ctrl, u16 total, u16 col, u16 row);

/*!
  set size.
  */
void mbox_set_size(control_t *p_ctrl, u16 col, u16 row, u16 win_col, u16 win_row);

/*!
  get size.
  */
void mbox_get_size(control_t *p_ctrl, u16 *p_col, u16 *p_row, u16 *p_win_col, u16 *p_win_row);


/*!
   Get count.

   \param[in] p_ctrl : matrixbox control
   \param[in] p_total : total number
   \param[in] p_col : col number
   \param[in] p_row : row number
   \return : TRUE/FALSE
  */
BOOL mbox_get_count(control_t *p_ctrl, u16 *p_total, u16 *p_col, u16 *p_row);


/*!
   Set item status.

   \param[in] p_ctrl : matrixbox control
   \param[in] index : item index
   \param[in] status : item status
   \return : TRUE/FALSE
  */
BOOL mbox_set_item_status(control_t *p_ctrl, u16 index, u8 status);

/*!
   Set item content by unicode string.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] p_unistr : unicode string
   \return : TRUE or FALSE
  */
BOOL mbox_set_content_by_unistr(control_t *p_ctrl, u16 item_idx, u16 *p_unistr);

/*!
   Set item content by char string.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] p_ascstr : char string
   \return : TRUE or FALSE
  */
BOOL mbox_set_content_by_ascstr(control_t *p_ctrl, u16 item_idx, u8 *p_ascstr);

/*!
   Set item content by string id.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] strid : string id
   \return : NULL
  */
void mbox_set_content_by_strid(control_t *p_ctrl, u16 item_idx, u16 strid);

/*!
   Set item content by static string.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] p_extstr : static string buffer
   \return : NULL
  */
void mbox_set_content_by_extstr(control_t *p_ctrl, u16 item_idx, u32 p_extstr);

/*!
   Set item content by icon id

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] f_icon_id : focus icon id
   \param[in] n_icon_id : normal icon id
   \return : NULL
  */
void mbox_set_content_by_icon(control_t *p_ctrl,
                              u16 item_idx,
                              u16 f_icon_id,
                              u16 n_icon_id);

/*!
  get icon idx.
  */
void mbox_get_icon(control_t *p_ctrl, u16 item_idx, u16 *p_icon_hlight, u16 *p_icon_normal);

/*!
   Set item content with hex number.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] hex : hex number
   \return : NULL
  */
void mbox_set_content_by_hex(control_t *p_ctrl, u16 item_idx, s32 hex);

/*!
   Set item content with dec number.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \param[in] dec : number
   \return : NULL
  */
void mbox_set_content_by_dec(control_t *p_ctrl, u16 item_idx, s32 dec);

/*!
   Set string align style.

   \param[in] p_ctrl : matrixbox control
   \param[in] style : align style
   \return : NULL
  */
void mbox_set_string_align_type(control_t *p_ctrl, u32 style);

/*!
  get string align.
  */
u32 mbox_get_string_align_type(control_t *p_ctrl);

/*!
   Set icon align style.

   \param[in] p_ctrl : matrixbox control
   \param[in] style : align style
   \return : NULL
  */
void mbox_set_icon_align_type(control_t *p_ctrl, u32 style);

/*!
  get icon align.
  */
u32 mbox_get_icon_align_type(control_t *p_ctrl);

/*!
   Set content type.

   \param[in] p_ctrl : matrixbox control
   \param[in] type : content type
   \return : NULL
  */
void mbox_set_content_strtype(control_t *p_ctrl, u32 type);

/*!
   Set focus.

   \param[in] p_ctrl : matrixbox control
   \param[in] focus : focus index
   \return : NULL
  */
void mbox_set_focus(control_t *p_ctrl, u16 focus);

/*!
   Set icon rect when use same position

   \param[in] p_ctrl : matrixbox control
   \param[in] icon_left : left pos of icon
   \param[in] icon_top : top pos of icon
   \return : NULL
  */
void mbox_set_icon_offset(control_t *p_ctrl, u8 icon_left, u8 icon_top);

/*!
  Set string work mode

  \param[in] p_ctrl : matrixbox control
  \param[in] work_mode : TRUE enable string work
  \return : NULL
  */
void mbox_enable_string_mode(control_t *p_ctrl, BOOL work_mode);

/*!
  Set icon work mode

  \param[in] p_ctrl : matrixbox control
  \param[in] work_mode : TRUE enable icon work
  \return : NULL
  */
void mbox_enable_icon_mode(control_t *p_ctrl, BOOL work_mode);

/*!
   Get item status

   \param[in] p_ctrl : matrixbox control
   \param[in] index : item index
   \return : item status
  */
u8 mbox_get_item_status(control_t *p_ctrl, u16 index);

/*!
   Get item rect. didn't realized yet.

   \param[in] p_ctrl : matrixbox control
   \param[in] index : item index
   \param[out] p_rect : return item rect
   \return : TRUE/FALSE
  */
BOOL mbox_get_item_rect(control_t *p_ctrl, u16 index, rect_t *p_rect);

/*!
   Get content. didn't realized yet.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \return : content string
  */
u32 *mbox_get_content_str(control_t *p_ctrl, u16 item_idx);

/*!
   Get focus.

   \param[in] p_ctrl : matrixbox control
   \return : focus index
  */
u16 mbox_get_focus(control_t *p_ctrl);

/*!
   Get focus icon.

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \return : focus icon id
  */
u16 mbox_get_focus_icon(control_t *p_ctrl, u16 item_idx);

/*!
   Get normal icon

   \param[in] p_ctrl : matrixbox control
   \param[in] item_idx : item index
   \return : normal icon id
  */
u16 mbox_get_normal_icon(control_t *p_ctrl, u16 item_idx);

/*!
   Get string pos in item.

   \param[in] p_ctrl : matrixbox control
   \param[out] p_left : return string left
   \param[out] p_top : return string top
   \return : NULL
  */
void mbox_get_string_offset(control_t *p_ctrl,
                            u8 *p_left,
                            u8 *p_top);

/*!
   Get icon pos in item.

   \param[in] p_ctrl : matrixbox control
   \param[out] p_left : return icon left
   \param[out] p_top : return icon top
   \return : NULL
  */
void mbox_get_icon_offset(control_t *p_ctrl,
                          u8 *p_left,
                          u8 *p_top);
/*!
   Move focus.

   \param[in] p_ctrl : matrixbox control
   \param[in] focus : new focus
   \return : TRUE or FALSE
  */
BOOL mbox_set_focus_ext(control_t *p_ctrl, u16 focus);

/*!
   Montrol proc

   \param[in] p_ctrl : matrixbox control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : NULL
  */
RET_CODE mbox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
/*!
  mbox set focus ex.

  \param[in] p_ctrl : control.
  \param[in] focus : focus.
  \param[in] org_col : org column.
  \param[in] org_row : org row.

  \return TRUE or FALSE.
  */
BOOL mbox_set_focus_ex(control_t *p_ctrl, u16 focus, u16 org_col, u16 org_row);

/*!
  mbox get focus ex.

  \param[in] p_ctrl : control.
  \param[out] p_focus : focus.
  \param[out] p_org_col : org column.
  \param[out] p_org_row : org row.
  */
void mbox_get_focus_ex(control_t *p_ctrl, u16 *p_focus, u16 *p_org_col, u16 *p_org_row);

/*!
  Get mbox content_strbuf.

  \param[in] p_ctrl : mbox ctrl.
  \param[in] item_idx : item id.
  \param[in] type : item string type.
  \param[out] string content.
  */
u32 *mbox_content_strbuf_get(control_t *p_ctrl, u16 item_idx, u32 type);

#ifdef __cplusplus
}
#endif

#endif
