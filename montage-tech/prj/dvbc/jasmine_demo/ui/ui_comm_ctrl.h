/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_COMM_CTRL_H__
#define __UI_COMM_CTRL_H__

// common control's keymap
u16 ui_comm_select_keymap(u16 key);

u16 ui_comm_numsel_keymap(u16 key);
  
u16 ui_comm_static_keymap(u16 key);

u16 ui_comm_num_keymap(u16 key);

u16 ui_comm_tbox_keymap(u16 key);
#ifdef ENABLE_NETWORK
u16 ui_comm_ipbox_keymap(u16 key);
u16 ui_comm_t9_v2_keymap(u16 key);
#endif
u16 ui_comm_edit_keymap(u16 key);

u16 ui_comm_t9_keymap(u16 key);

// common control's proc
RET_CODE ui_comm_num_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE ui_comm_numsel_cbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE ui_comm_numsel_nbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE ui_comm_time_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
#ifdef ENABLE_NETWORK
RET_CODE ui_comm_ipbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
#endif
RET_CODE ui_comm_edit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE ui_comm_t9_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE ui_comm_longitude_proc(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2);

RET_CODE ui_comm_latitude_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


/*********************************************************************
 * COMMON CONTROLS
 *********************************************************************/
typedef struct
{
  u32                  attr;
  u16                  width;
  u16                  left;
  u8                   top;
  list_xstyle_t * rstyle;
  list_xstyle_t * fstyle;
}list_field_attr_t;

#ifdef ENABLE_NETWORK
typedef struct
{
  u32                  attr;
  u16                  width;
  u16                  height;
  u16                  left;
  u8                   top;
  list_xstyle_t * rstyle;
  list_xstyle_t * fstyle;
}list_field_attr_ext_t;

typedef struct
{
  u32 attr;
  u16 width;
  u16 height;
  u16 left;
  u16 top;
  list_xstyle_t *p_rstyle;
  list_xstyle_t *p_fstyle;
}list_field_attr2_t;
#endif

enum comm_ctrl_idc
{
  IDC_COMM_TXT = 1,
  IDC_COMM_CTRL,
  IDC_COMM_EXT_NUM,
  IDC_COMM_EXT_SEL,
};

/* coordinate */
#define COMM_CTRL_H              40//45
#define COMM_CTRL_OX             20
#define COMM_BAR_BORDER          0
#ifdef ENABLE_NETWORK
#define COMM_CTRL_ICON_W            (12)
#define COMM_BOX1_BORDER            (12)
#define COMM_DROPLIST_ITEM_VGAP     (2)
#define COMM_DROPLIST_LIST_BORDER   (5)
#endif

/* rect style  */
#define RSI_COMM_CONT_SH         RSI_PBACK//RSI_ITEM_1_SH
#define RSI_COMM_CONT_HL         RSI_ITEM_1_HL
#define RSI_COMM_CONT_GRAY       RSI_PBACK//RSI_ITEM_1_SH

#define RSI_COMM_TXT_SH          RSI_PBACK
#define RSI_COMM_TXT_HL          RSI_PBACK
#define RSI_COMM_TXT_GRAY        RSI_PBACK

#define RSI_COMM_SELECT_SH       RSI_IGNORE
#define RSI_COMM_SELECT_HL       RSI_LEFT_RIGHT_HL
#define RSI_COMM_SELECT_GRAY     RSI_IGNORE

#define RSI_COMM_TIMEDIT_SH      RSI_IGNORE
#define RSI_COMM_TIMEDIT_HL      RSI_LEFT_RIGHT_HL
#define RSI_COMM_TIMEDIT_GRAY    RSI_IGNORE
#ifdef ENABLE_NETWORK
#define RSI_COMM_IPEDIT_SH      RSI_PBACK
#define RSI_COMM_IPEDIT_HL      RSI_PBACK
#define RSI_COMM_IPEDIT_GRAY    RSI_PBACK
#endif

#define RSI_COMM_STATIC_SH       RSI_PBACK
#define RSI_COMM_STATIC_HL       RSI_PBACK
#define RSI_COMM_STATIC_GRAY     RSI_PBACK

#define RSI_COMM_NUMEDIT_SH      RSI_T9EDIT_RIGHT_SH
#define RSI_COMM_NUMEDIT_HL      RSI_T9EDIT_RIGHT_HL
#define RSI_COMM_NUMEDIT_GRAY    RSI_T9EDIT_RIGHT_GRAY

#define RSI_COMM_PWDEDIT_SH      RSI_PBACK
#define RSI_COMM_PWDEDIT_HL      RSI_PBACK
#define RSI_COMM_PWDEDIT_GRAY    RSI_PBACK

#define RSI_COMM_T9EDIT_SH       RSI_T9EDIT_RIGHT_SH
#define RSI_COMM_T9EDIT_HL       RSI_T9EDIT_RIGHT_HL
#define RSI_COMM_T9EDIT_GRAY     RSI_T9EDIT_RIGHT_GRAY

/* font style */
#define FSI_COMM_TXT_SH          FSI_WHITE
#define FSI_COMM_TXT_HL          FSI_WHITE
#define FSI_COMM_TXT_GRAY        FSI_GRAY

#define FSI_COMM_CTRL_SH         FSI_WHITE
#define FSI_COMM_CTRL_HL         FSI_WHITE
#define FSI_COMM_CTRL_GRAY       FSI_GRAY

void ui_comm_ctrl_set_cont_rstyle(control_t *cont,
                                  u8 sh_rstyle,
                                  u8 hl_rstyle,
                                  u8 gr_rstyle);

void ui_comm_ctrl_set_txt_rstyle(control_t *cont,
                                 u8 sh_rstyle,
                                 u8 hl_rstyle,
                                 u8 gr_rstyle);

void ui_comm_ctrl_set_ctrl_rstyle(control_t *cont,
                                  u8 sh_rstyle,
                                  u8 hl_rstyle,
                                  u8 gr_rstyle);

void ui_comm_ctrl_set_keymap(control_t *p_ctrl, keymap_t keymap);

void ui_comm_ctrl_set_proc(control_t *p_ctrl, msgproc_t proc);

void ui_comm_ctrl_update_attr(control_t *p_ctrl, BOOL is_enable);

void ui_comm_ctrl_paint_ctrl(control_t *p_ctrl, BOOL is_force);


/*********************************************************************
 * COMMON SELECT
 *********************************************************************/
#define COMM_SELECT_DROPLIST_PAGE   5

control_t *ui_comm_select_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw);

void ui_comm_select_set_param(control_t *p_ctrl,
                              BOOL is_cycle,
                              u32 workmode,
                              u16 total,
                              u32 content_type,
                              cbox_dync_update_t callback);
void ui_comm_select_right_set_mrect(control_t *p_ctrl, u16 left, u16 top, u16 right, u16 bottom);
                              
void ui_comm_select_set_count(control_t *p_ctrl, u32 workmode, u16 total);

void ui_comm_select_create_droplist(control_t *p_ctrl, u16 page);

void ui_comm_select_droplist_set_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style);

void ui_comm_select_droplist_set_item_rstyle(control_t *p_ctrl,
  list_xstyle_t *p_style);

void ui_comm_select_droplist_set_sbar_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style);

void ui_comm_select_droplist_set_sbar_mid_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style);

void ui_comm_select_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_select_set_content(control_t *p_ctrl, u8 idx, u16 strid);

void ui_comm_select_set_content_by_unistr(control_t *p_ctrl, u8 idx, u16 *unistr);

void ui_comm_select_set_content_by_dec(control_t *p_ctrl, u8 idx, s32 num);

u16 ui_comm_select_get_count(control_t *p_ctrl);

u32 ui_comm_select_get_content(control_t *p_ctrl);

void ui_comm_select_set_focus(control_t *p_ctrl, u16 focus);

u16 ui_comm_select_get_focus(control_t *p_ctrl);

void ui_comm_select_set_num_range(control_t *p_ctrl,
                                  s32 min,
                                  s32 max,
                                  u32 step,
                                  u8 bit_length,
                                  u16 *post_fix);

void ui_comm_select_get_num_range(control_t *p_ctrl,
                                  s32 *min,
                                  s32 *max,
                                  u32 *step);

void ui_comm_select_set_num(control_t *p_ctrl, s32 num);

s32 ui_comm_select_get_num(control_t *p_ctrl);

/*********************************************************************
 * COMMON NUM SEL
 *********************************************************************/
control_t *ui_comm_numsel_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw);

void ui_comm_numsel_set_param(control_t *p_ctrl,
                              BOOL is_cycle,
                              u32 workmode,
                              u16 total,
                              u8 bit_length,
                              u32 content_type,
                              cbox_dync_update_t callback);

void ui_comm_numsel_set_focus(control_t *p_ctrl, u16 focus);

u16 ui_comm_numsel_get_focus(control_t *p_ctrl);

void ui_comm_numsel_create_droplist(control_t *p_ctrl, u16 page);

void ui_comm_numsel_droplist_set_rstyle(control_t *p_ctrl,
                        u8 sh_style, u8 hl_style, u8 gr_style);

void ui_comm_numsel_droplist_set_item_rstyle(control_t *p_ctrl,
                        list_xstyle_t *p_style);

void ui_comm_numsel_droplist_set_sbar_rstyle(control_t *p_ctrl,
                        u8 sh_style, u8 hl_style, u8 gr_style);

/*********************************************************************
 * COMMON TIME EDIT
 *********************************************************************/
control_t *ui_comm_timedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw);

void ui_comm_timedit_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_timedit_set_param(control_t *p_ctrl,
                               u8 bit,
                               u8 item,
                               u32 time_type,
                               u32 sepa_type,
                               u8 num_width);

void ui_comm_timedit_set_separator_by_bmapid(control_t *p_ctrl,
                                             u8 index,
                                             u16 bmapid);

void ui_comm_timedit_set_separator_by_extstr(control_t *p_ctrl,
                                             u8 index,
                                             u16 *p_str);

void ui_comm_timedit_set_separator_by_strid(control_t *p_ctrl,
                                            u8 index,
                                            u16 strid);

void ui_comm_timedit_set_separator_by_ascchar(control_t *p_ctrl,
                                              u8 index,
                                              u8 ascchar);

void ui_comm_timedit_set_separator_by_unichar(control_t *p_ctrl,
                                              u8 index,
                                              u16 unichar);

void ui_comm_timedit_set_time(control_t *p_ctrl, utc_time_t *p_time);

void ui_comm_timedit_get_time(control_t *p_ctrl, utc_time_t *p_time);

BOOL ui_comm_timedit_get_is_on_edit(control_t *p_ctrl);

BOOL ui_comm_timedit_enter_edit(control_t *p_ctrl);
#ifdef ENABLE_NETWORK
/*********************************************************************
 * COMMON IP EDIT
 *********************************************************************/
control_t *ui_comm_ipedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw);

void ui_comm_ipedit_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_ipedit_set_param(control_t *p_ctrl,
                               u8 bit,
                               u8 item,
                               u32 address_type,
                               u32 sepa_type,
                               u8 num_width);

void ui_comm_ipedit_set_separator_by_bmapid(control_t *p_ctrl,
                                             u8 index,
                                             u16 bmapid);

void ui_comm_ipedit_set_separator_by_extstr(control_t *p_ctrl,
                                             u8 index,
                                             u16 *p_str);

void ui_comm_ipedit_set_separator_by_strid(control_t *p_ctrl,
                                            u8 index,
                                            u16 strid);

void ui_comm_ipedit_set_separator_by_ascchar(control_t *p_ctrl,
                                              u8 index,
                                              u8 ascchar);

void ui_comm_ipedit_set_separator_by_unichar(control_t *p_ctrl,
                                              u8 index,
                                              u16 unichar);

void ui_comm_ipedit_set_address(control_t *p_ctrl, ip_address_t *p_address);

void ui_comm_ipedit_get_address(control_t *p_ctrl, ip_address_t *p_address);

BOOL ui_comm_ipedit_get_is_on_edit(control_t *p_ctrl);

void ui_comm_t9edit_set_font(control_t *cont,
                                 u32 n_fstyle,
                                 u32 h_fstyle,
                                 u32 g_fstyle);

/*********************************************************************
 * COMMON T9_V2 EDIT
 *********************************************************************/
control_t *ui_comm_t9_v2_edit_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw,
                                 u8 root_id);

void ui_comm_t9_v2_edit_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_t9_v2_edit_set_content_by_unistr(control_t *p_ctrl, u16 *unistr);

void ui_comm_t9_v2_edit_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr);

u16 *ui_comm_t9_v2_edit_get_content(control_t *p_ctrl);

void ui_comm_t9_v2_edit_set_font(control_t *cont,
                                 u32 n_fstyle,
                                 u32 h_fstyle,
                                 u32 g_fstyle);
#endif
/*********************************************************************
 * COMMON NUM EDIT
 *********************************************************************/
control_t *ui_comm_numedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw);

void ui_comm_numedit_set_param(control_t *p_ctrl,
                               u32 type,
                               u32 min,
                               u32 max,
                               u8 bit_length,
                               u8 focus);

void ui_comm_numedit_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_numedit_set_num(control_t *p_ctrl, u32 num);

u32 ui_comm_numedit_get_num(control_t *p_ctrl);

void ui_comm_numedit_set_postfix(control_t *p_ctrl, u16 strid);

u32 ui_comm_numedit_get_postfix(control_t *p_ctrl);

void ui_comm_numedit_set_decimal_places(control_t *p_ctrl, u8 places);

u8 ui_comm_numedit_get_decimal_places(control_t *p_ctrl);

void ui_comm_numedit_set_separator(control_t *p_ctrl, u8 separator, u8 places);

BOOL ui_comm_numedit_is_on_edit(control_t *p_ctrl);


/*********************************************************************
 * COMMON PWD EDIT
 *********************************************************************/
control_t *ui_comm_pwdedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw);

void ui_comm_pwdedit_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_pwdedit_set_param(control_t *p_ctrl, u16 maxtext);

void ui_comm_pwdedit_set_mask(control_t *p_ctrl, u8 bef_mask, u8 aft_mask);

u32 ui_comm_pwdedit_get_value(control_t *p_ctrl);

void ui_comm_pwdedit_empty_value(control_t *p_ctrl);

/*********************************************************************
 * COMMON T9 EDIT
 *********************************************************************/
control_t *ui_comm_t9edit_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw,
                                 u8 root_id);

void ui_comm_t9edit_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_t9edit_set_param(control_t *p_ctrl, u16 maxtext);

void ui_comm_t9edit_set_upper(control_t *p_ctrl, BOOL is_upper);

void ui_comm_t9edit_set_content_by_unistr(control_t *p_ctrl, u16 *unistr);

void ui_comm_t9edit_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr);

u16 *ui_comm_t9edit_get_content(control_t *p_ctrl);

BOOL ui_comm_t9edit_backspace(control_t *p_ctrl);

/*********************************************************************
 * COMMON STATIC
 *********************************************************************/
control_t *ui_comm_static_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw);

void ui_comm_static_set_param(control_t *p_ctrl, u32 content_type);

void ui_comm_static_set_static_txt(control_t *p_ctrl, u16 strid);

void ui_comm_static_set_content_by_strid(control_t *p_ctrl, u16 strid);

void ui_comm_static_set_content_by_dec(control_t *p_ctrl, s32 num);

void ui_comm_static_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr);

void ui_comm_static_set_content_by_unistr(control_t *p_ctrl, u16 *unistr);

void ui_comm_static_set_rstyle(control_t *cont,
                               u8 rsi_cont,
                               u8 rsi_stxt,
                               u8 rsi_ctrl);

void ui_comm_static_set_align_type(control_t *cont,
                                   u16 stxt_ox,
                                   u32 stxt_align,
                                   u16 ctrl_ox,
                                   u32 ctrl_align);

u32 ui_comm_static_get_content(control_t *cont);

/*********************************************************************
 * COMMON PROGRESS BAR
 *********************************************************************/

enum comm_bar_idc
{
  IDC_COMM_BAR = 1,
  IDC_COMM_BAR_TXT,
  IDC_COMM_BAR_PERCENT,
};

control_t *ui_comm_bar_create(control_t *parent,
                              u8 ctrl_id,
                              u16 x,
                              u16 y,
                              u16 w,
                              u16 h,
                              u16 txt_x,
                              u16 txt_y,
                              u16 txt_w,
                              u16 txt_h,
                              u16 percent_x,
                              u16 percent_y,
                              u16 percent_w,
                              u16 percent_h);

void ui_comm_bar_set_style(control_t *cont,
                           u8 rsi_bar,
                           u8 rsi_mid,
                           u8 rsi_txt,
                           u8 fsi_txt,
                           u8 rsi_percent,
                           u8 fsi_percent);

void ui_comm_bar_set_param(control_t *cont,
                           u16 rsc_id,
                           u16 min,
                           u16 max,
                           u16 step);

BOOL ui_comm_bar_update(control_t *cont, u16 val, BOOL is_force);

BOOL ui_comm_tp_bar_update(control_t *cont, u16 val, BOOL is_force,u8 *pox);

void ui_comm_bar_paint(control_t *cont, BOOL is_force);

BOOL ui_comm_pwdedit_is_full(control_t *p_ctrl);

control_t * ui_comm_ctrl_get_ctrl(control_t *p_ctrl);

#endif
