/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CBOX_CTRL_H__
#define __CBOX_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_combobox.h

   This file defined data structures and macro constant for combobox.
   It also defines some interfaces for aplication layer modules.
  */

/*!
   Combobox string align tye mask
  */
#define CBOX_STRING_ALIGN_MASK           0xF0000000

/*!
   Combobox work mode mask
  */
#define CBOX_WORKMODE_MASK               0x0F000000
/*!
   Combobox work in static mode
  */
#define CBOX_WORKMODE_STATIC             0x00000000
/*!
   Combobox work in number mode
  */
#define CBOX_WORKMODE_NUMBER             0x01000000
/*!
   Combobox work in dynamic mode
  */
#define CBOX_WORKMODE_DYNAMIC            0x02000000


/*!
   Combobox string type mask, used in CBOX_WORKMODE_STATIC
  */
#define CBOX_ITEM_STRTYPE_MASK           0x00F00000
/*!
   Set combobox string by unicode
  */
#define CBOX_ITEM_STRTYPE_UNICODE        0x00000000
/*!
   Set combobox string by static unicode
  */
#define CBOX_ITEM_STRTYPE_EXTSTR         0x00100000
/*!
   Set combobox string by string id
  */
#define CBOX_ITEM_STRTYPE_STRID          0x00200000
/*!
   Set combobox string by decimal
  */
#define CBOX_ITEM_STRTYPE_DEC            0x00300000
/*!
   Set combobox string by hex
  */
#define CBOX_ITEM_STRTYPE_HEX            0x00400000


/*!
   Combobxo number type mask
  */
#define CBOX_ITEM_NUMTYPE_MASK           0x000F0000
/*!
   Set combobox number by decimal
  */
#define CBOX_ITEM_NUMTYPE_DEC            0x00000000
/*!
   Set combobox number by hex
  */
#define CBOX_ITEM_NUMTYPE_HEX            0x00010000


/*!
   Combobox prefix type mask
  */
#define CBOX_ITEM_PREFIX_TYPE_MASK       0x0000C000
/*!
   Set combobox prefix by string id
  */
#define CBOX_ITEM_PREFIX_TYPE_STRID      0x00000000
/*!
   Set combobox prefix by static unicode
  */
#define CBOX_ITEM_PREFIX_TYPE_EXTSTR     0x00004000

/*!
   Combobox postfix type mask
  */
#define CBOX_ITEM_POSTFIX_TYPE_MASK      0x00003000
/*!
   Set combobox postfix by string id
  */
#define CBOX_ITEM_POSTFIX_TYPE_STRID     0x00000000
/*!
   Set combobox postfix by static unicode
  */
#define CBOX_ITEM_POSTFIX_TYPE_EXTSTR    0x00001000

/*!
   Combobox status mask
  */
#define CBOX_STATUS_MASK                 0x00000F00
/*!
   Combobox normal
  */
#define CBOX_STATUS_NORMAL               0x00000000
/*!
   Combobox highlight in open list type
  */
#define CBOX_STATUS_LIST                 0x00000100

/*!
   Combobox cycle mode, 0 is normally, 1 is circularly
  */
#define CBOX_CYCLE_MODE                  0x00000001

/*!
   Combobox prefix&postfix string length
  */
#define CBOX_ITEM_NUM_XFIX_LENGTH        10

/*!
   Dynamic combobox update callback
  */
typedef RET_CODE (*cbox_dync_update_t)(control_t *p_ctrl, u16 focus,
                                       u16 *p_str, u16 max_length);

/*!
  Register combobox control class to system.

  \param[in] max_cnt : max combobox control number
  \return : SUCCESS or ERR_FAILURE
  */
RET_CODE cbox_register_class(u16 max_cnt);

/*!
  Set the font style of combobox.

  \param[in] p_ctrl : point to a combobox control.
  \param[in] n_fstyle : normal fstyle.
  \param[in] h_fstyle : highlight fstyle.
  \param[in] g_fstyle : gray fstyle.
  \return : NULL
  */
void cbox_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle);

/*!
  cbox get fstyle.
  */
void cbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive);
                         

/*!
   Set the string offset of the combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] left : left interval.
   \param[in] top : top interval.
   \return : NULL
  */
void cbox_set_offset(control_t *p_ctrl, u16 left, u16 top);

/*!
   Set the text align type of a combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] style : align type to  be set
   \return : NULL
  */
void cbox_set_align_style(control_t *p_ctrl, u32 style);

/*!
  get align style.
  */
u32 cbox_get_align_style(control_t *p_ctrl);

/*!
   Set the cycle mode  of a combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] is_enable : True for enable, False for disable.
   \return : NULL
  */
void cbox_enable_cycle_mode(control_t *p_ctrl, BOOL is_enable);

/*!
  get cycle mode.
  */
BOOL cbox_is_cycle_mode(control_t *p_ctrl);

/*!
   Set the work mode  of a combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] mode : the new workmode to be set.
   \return : NULL
  */
void cbox_set_work_mode(control_t *p_ctrl, u32 mode);

/*!
  Get the work mode  of a combobox.

  \param[in] p_ctrl : point to a combobox control.
  \return : the workmode
  */
u32 cbox_get_work_mode(control_t *p_ctrl);

/*!
   Creates a droplist for a combobox.

   \param[in] p_ctrl : point a combobox control.
   \param[in] page : specified the page of the droplist.
   \param[in] sbar_width : specified the width of the scrollbar of droplist.
   \param[in] sbar_gap : specified the gap between droplist and scrollbar.
   \return : the droplist control.
  */
control_t *cbox_create_droplist(control_t *p_ctrl,
                                u16 page,
                                u16 sbar_width,
                                u16 sbar_gap);

/*!
   Set the droplist rectangle styles for a combobox control.

   \param[in] p_ctrl : point to a combobox control
   \param[in] sh_style : style index on normal
   \param[in] hl_style : style index on highlight
   \param[in] gr_style : style index on gray
   \return : NULL
  */
void cbox_droplist_set_rstyle(control_t *p_ctrl,
                              u8 sh_style,
                              u8 hl_style,
                              u8 gr_style);
/*!
   Set the droplist cycle mode.

   \param[in] p_ctrl : point to a combobox control
   \param[in] is_enable : enable cycle or not
   \return : NULL
  */
void cbox_droplist_set_cycle_mode(control_t *p_ctrl, BOOL is_enable);
/*!
   Set the droplist page mode.

   \param[in] p_ctrl : point to a combobox control
   \param[in] is_enable : enable page mode or not
   \return : NULL
  */
void cbox_droplist_set_page_mode(control_t *p_ctrl, BOOL is_enable);
/*!
   Set middle rectangle for a combobox's droplist.

   \param[in] p_ctrl : point a combobox control.
   \param[in] left : left of middle rect
   \param[in] top : top of middle rect
   \param[in] width : width of middle rect
   \param[in] height : height of middle rect
   \param[in] interval : interval between items
   \return : NULL
  */
void cbox_droplist_set_mid_rect(control_t *p_ctrl,
                                s16 left,
                                s16 top,
                                u16 width,
                                u16 height,
                                u8 interval);

/*!
  Set the rectangle style of items for a combobox's droplist.

  \param[in] p_ctrl : point a combobox control.
  \param[in] p_style : style for list item.
  \return : NULL
  */
void cbox_droplist_set_item_rstyle(control_t *p_ctrl, list_xstyle_t *p_style);


/*!
   Set field attribute for a combobox's droplist.

   \param[in] p_ctrl : point a combobox control.
   \param[in] align_type : align type
   \param[in] width : field width
   \param[in] left : left shift
   \param[in] top : top shift
   \return : NULL
  */
void cbox_droplist_set_field_attr(control_t *p_ctrl,
                                  u32 align_type,
                                  u16 width,
                                  u16 left,
                                  u16 top);

/*!
  Set the field rectangle style for a combobox's droplist.

  \param[in] p_ctrl : point a combobox control.
  \param[in] p_style : style for list field.
  \return : NULL
  */
void cbox_droplist_set_field_rect_style(control_t *p_ctrl,
                                        list_xstyle_t *p_style);


/*!
   Set the field font style for a combobox's droplist.

   \param[in] p_ctrl : point a combobox control.
   \param[in] p_style : font style for list field.
   \return : NULL
  */
void cbox_droplist_set_field_font_style(control_t *p_ctrl,
                                        list_xstyle_t *p_style);

/*!
   Attach a specified droplist for a combobox.

   \param[in] p_ctrl : point a combobox control.
   \param[in] p_droplist : point a list control.
  */
void cbox_attach_droplist(control_t *p_ctrl, control_t *p_droplist);

/*!
   Set a update function  for a combobox's droplist.

   \param[in] p_ctrl : point a combobox control.
   \param[in] p_cb : point a update function.
   \param[in] context : the context.
  */
void cbox_droplist_set_update(control_t *p_ctrl,
                              list_update_t p_cb,
                              u32 context);


/*!
  Set the scrollbar rectangle styles for a combobox control.

  \param[in] p_ctrl : point to a combobox control
  \param[in] sh_style : style index on normal
  \param[in] hl_style : style index on highlight
  \param[in] gr_style : style index on gray
  \return : NULL
  */
void cbox_droplist_set_sbar_rstyle(control_t *p_ctrl,
                                   u8 sh_style,
                                   u8 hl_style,
                                   u8 gr_style);

/*!
   Set the coordinate of middle rectangle for a scrollbar.

   \param[in] p_ctrl : combobox control
   \param[in] left : left of rect
   \param[in] top : top of rect
   \param[in] right : right of rect
   \param[in] bottom : bottom of rect
   \return : NULL
  */
void cbox_droplist_set_sbar_mid_rect(control_t *p_ctrl,
                                     s16 left,
                                     s16 top,
                                     s16 right,
                                     s16 bottom);


/*!
   Set middle move block r-style for a scrollbar.
   \param[in] p_ctrl : combobox control
   \param[in] sh_style : normal move block rstyle
   \param[in] hl_style : highlight move block rstyle
   \param[in] gr_style : gray move block rstyle
   \return : NULL
  */
void cbox_droplist_set_sbar_mid_rstyle(control_t *p_ctrl,
                                       u8 sh_style,
                                       u8 hl_style,
                                       u8 gr_style);

/*!
   To check whether the droplist is opened

   \param[in] p_ctrl : point a combobox control.
   \return : If successful return TRUE, Otherwise return FALSE.
  */
BOOL cbox_droplist_is_opened(control_t *p_ctrl);

/*!
   Open the droplist.

   \param[in] p_ctrl : point a combobox control.
   \return : If successful return TRUE, Otherwise return FALSE.
  */
BOOL cbox_open_droplist(control_t *p_ctrl);

/*!
   Close the droplist.

   \param[in] p_ctrl : point a combobox control.
   \param[in] is_update : update focus by list's focus or NOT.
   \return : If successful return TRUE, Otherwise return FALSE.
  */
BOOL cbox_close_droplist(control_t *p_ctrl, BOOL is_update);

/*!
   Update the droplist.

   \param[in] p_ctrl : point a combobox control.
   \return : If successful return TRUE, Otherwise return FALSE.
  */
BOOL cbox_update_droplist(control_t *p_ctrl);

/*!
   Get the content of a specified position in a combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] pos : a specified position.
   \return : if success, return the content buffer, else return 0.
  */
u32 cbox_get_content(control_t *p_ctrl, u16 pos);


/*!
   Set the count of a static combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] total : count to be set.
   \return : If success, return TRUE, else return FALSE.
  */
BOOL cbox_static_set_count(control_t *p_ctrl, u16 total);

/*!
   Get the total count of a static combobox.

   \param[in] p_ctrl : point to a combobox control.
   \return : if success, return the focus, else return 0.
  */
u16 cbox_static_get_count(control_t *p_ctrl);

/*!
   Set the focus of a static combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] focus : new focus to be set.
   \return : If success, return TRUE, else return FALSE.
  */
BOOL cbox_static_set_focus(control_t *p_ctrl, u16 focus);

/*!
   Get the focus of a static combobox.

   \param[in] p_ctrl : point to a combobox control.
   \return : If success, return the focus, else return FALSE.
  */
u16 cbox_static_get_focus(control_t *p_ctrl);

/*!
   Set the content type of a static combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] type : new type to be set.
   \return : NULL.
  */
void cbox_static_set_content_type(control_t *p_ctrl, u32 type);

/*!
   Set the content of a specified position in a static combobox unicode string.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] pos : a specified position.
   \param[in] p_unistr : unicode string to be set.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_static_set_content_by_unistr(control_t *p_ctrl,
                                       u16 pos,
                                       u16 *p_unistr);
/*!
   Set the content of a specified position in a static combobox by ascii string.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] pos : a specified position.
   \param[in] p_ascstr : ascii string to be set.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_static_set_content_by_ascstr(control_t *p_ctrl,
u16 pos, u8 *p_ascstr);

/*!
  Set the content of a specified position in a static combobox by static
  unicode string.

  \param[in] p_ctrl : point to a combobox control.
  \param[in] pos : a specified position.
  \param[in] p_extstr : static unicode string to be set.
  \return : NULL.
  */
void cbox_static_set_content_by_extstr(control_t *p_ctrl,
                                       u16 pos,
                                       u16 *p_extstr);

/*!
   Set the content of a specified position in a static combobox by string id.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] pos : a specified position.
   \param[in] strid : a specified string id for set.
   \return : NULL.
  */
void cbox_static_set_content_by_strid(control_t *p_ctrl, u16 pos, u16 strid);

/*!
   Set the content of a specified position in a static combobox by decimal
   number.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] pos : a specified position.
   \param[in] dec : decimal number for set.
   \return : NULL.
  */
void cbox_static_set_content_by_dec(control_t *p_ctrl, u16 pos, s32 dec);

/*!
  Set the content of a specified position in a static combobox by hex number.

  \param[in] p_ctrl : point to a combobox control.
  \param[in] pos : a specified position.
  \param[in] hex : hex number for set.
  \return : NULL.
  */
void cbox_static_set_content_by_hex(control_t *p_ctrl, u16 pos, s32 hex);

/*!
   Set the content type of a number combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] type : content type to be set.
   \return : NULL.
  */
void cbox_num_set_type(control_t *p_ctrl, u32 type);

/*!
  Set the number range of a number combobox.

  \param[in] p_ctrl : point to a combobox control.
  \param[in] min : minimum number.
  \param[in] max : maximum number.
  \param[in] step : step.
  \param[in] bit_length : bit length.  
  \return : NULL.
  */
void cbox_num_set_range(control_t *p_ctrl,
                        s32 min,
                        s32 max,
                        u32 step,
                        u8 bit_length);

/*!
   Get the number range of a number combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[out] p_min : minimum number.
   \param[out] p_max : maximum number.
   \param[out] p_inc : strp.
   \return : NULL
  */
void cbox_num_get_range(control_t *p_ctrl, s32 *p_min, s32 *p_max, u32 *p_inc);


/*!
   Set the current value of a number combobox.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] value : current value to be set.
   \return : NULL.
  */
void cbox_num_set_curn(control_t *p_ctrl, s32 value);

/*!
   Get the current value of a number combobox.

   \param[in] p_ctrl : point to a combobox control.
   \return : if success, return the current value, else return -1.
  */
s32 cbox_num_get_curn(control_t *p_ctrl);

/*!
   Set prefix type.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] type : prefix or postfix.
   \return : NULL.
  */
void cbox_num_set_prefix_type(control_t *p_ctrl, u32 type);

/*!
  Set prefix of a number combobox by string id.

  \param[in] p_ctrl : control
  \param[in] strid : string id.
  \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_num_set_prefix_by_strid(control_t *p_ctrl, u16 strid);

/*!
   Set postfix of a number combobox by string id.

   \param[in] p_ctrl : control
   \param[in] strid : string id.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_num_set_postfix_by_strid(control_t *p_ctrl, u16 strid);

/*!
   Set postfix type.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] type : prefix or postfix.
   \return : NULL.
  */
void cbox_num_set_postfix_type(control_t *p_ctrl, u32 type);

/*!
   Set the prefix of a number combobox by static unicode string.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] p_extstr : static unicode string.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_num_set_prefix_by_extstr(control_t *p_ctrl, u16 *p_extstr);

/*!
   Set the postfix of a number combobox by static unicode string.

   \param[in] p_ctrl : point to a combobox control.
   \param[in] p_extstr : static unicode string.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_num_set_postfix_by_extstr(control_t *p_ctrl, u16 *p_extstr);

/*!
  Set the update callback of a dynamic combobox.

  \param[in] p_ctrl : point to a combobox control.
  \param[in] p_callback : callback function for update dynamic cbox.
  \return : NULL.
  */
void cbox_dync_set_update(control_t *p_ctrl, cbox_dync_update_t p_callback);

/*!
  Set the count of a dynamic combobox.

  \param[in] p_ctrl : point to a combobox control.
  \param[in] total : count.
  \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_dync_set_count(control_t *p_ctrl, u16 total);

/*!
   Get the total count of a dynamic combobox.

   \param[in] p_ctrl : point to a combobox control.
   \return : if success, return the focus, else return 0.
  */
u16 cbox_dync_get_count(control_t *p_ctrl);


/*!
  Set the focus of a dynamic combobox.

  \param[in] p_ctrl : point to a combobox control.
  \param[in] focus : focus.
  \return : if success, return TRUE, else return FALSE.
  */
BOOL cbox_dync_set_focus(control_t *p_ctrl, u16 focus);

/*!
   Get the focus of a dynamic combobox.

   \param[in] p_ctrl : point to a combobox control.
   \return : if success, return the focus, else return 0.
  */
u16 cbox_dync_get_focus(control_t *p_ctrl);

/*!
   Get the droplist of a combobox.

   \param[in] p_ctrl : point to a combobox control.
   \return : droplist.
  */
control_t *cbox_droplist_get(control_t *p_ctrl);

/*!
   Default class process function for combobox.

   \param[in] p_ctrl : point a combobox control.
   \param[in] msg : message for combobox.
   \param[in] para1 : reserved.
   \param[in] para2 : reserved.
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE cbox_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif

#endif
