/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_fav_list.h"

enum fav_list_control_id
{
  IDC_INVALID = 0,
  IDC_FAV_LIST_PREV,
  IDC_FAV_LIST_GROUP,
  IDC_FAV_LIST_LIST,
  IDC_FAV_LIST_BAR,
};

static list_xstyle_t fav_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t fav_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t fav_list_field_rstyle_left =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
};

static list_xstyle_t fav_list_field_rstyle_mid =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
};


static list_field_attr_t fav_list_attr[FAV_LIST_LIST_FIELD] =
{
  {LISTFIELD_TYPE_DEC| STL_CENTER| STL_VCENTER,
   100, 0, 0, &fav_list_field_rstyle_left, &fav_list_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
   360, 200, 0, &fav_list_field_rstyle_mid, &fav_list_field_fstyle},
};

u16 fav_list_list_keymap(u16 key);
RET_CODE fav_list_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE fav_list_update(control_t *ctrl, u16 start, u16 size, u32 context)
{
  u16 i, uni_str[32];
  u16 cnt = list_get_count(ctrl);

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      // NO. 
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0,
                                    (u16)(start + i + 1 )); //program number in list

      // NAME 
      memset(uni_str, 0, sizeof(uni_str));
      sys_status_get_fav_name((u8)(start + i + sys_status_get_fav1_index()), uni_str);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_fav_list_select(control_t *p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  u16 focus;
  
  focus = list_get_focus_pos(p_ctrl);

  if(list_get_count(p_ctrl) == 0)
  {
    return manage_open_menu(ROOT_ID_SMALL_LIST, V_KEY_OK, sys_status_get_curn_group());
  }
  else
  {
    return manage_open_menu(ROOT_ID_SMALL_LIST, V_KEY_FAV, ((u32)(focus + sys_status_get_fav1_index() + DB_DVBS_MAX_SAT + 1) << 16) | (focus + sys_status_get_fav1_index()) /*ALL*/);
  }
}

RET_CODE open_fav_list(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_list;//, *p_preview;
  control_t *p_group;
  control_t *p_sbar;

  u8 i;
  
  p_cont = ui_comm_root_create(ROOT_ID_FAV_LIST,
                               0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
                               COMM_BG_H, IM_TITLEICON_TV, IDS_FAV_LIST);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  //preview
  #if 0
  p_preview =
    ctrl_create_ctrl(CTRL_CONT, IDC_FAV_LIST_PREV, FAV_LIST_PREV_X, FAV_LIST_PREV_Y,
                     FAV_LIST_PREV_W, FAV_LIST_PREV_H, p_cont,
                     0);
  ctrl_set_rstyle(p_preview, RSI_FAV_LIST_PREV, RSI_FAV_LIST_PREV, RSI_FAV_LIST_PREV);
  #endif
  //group
  p_group = ctrl_create_ctrl(CTRL_TEXT, IDC_FAV_LIST_GROUP, FAV_LIST_GROUP_X,
                             FAV_LIST_GROUP_Y, FAV_LIST_GROUP_W,
                             FAV_LIST_GROUP_H, p_cont,
                             0);
  ctrl_set_rstyle(p_group, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  text_set_align_type(p_group , STL_CENTER|STL_VCENTER);
  text_set_font_style(p_group , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_group, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_group , IDS_LIST);

  //prog list
  p_list =
    ctrl_create_ctrl(CTRL_LIST, IDC_FAV_LIST_LIST, FAV_LIST_LIST_X, FAV_LIST_LIST_Y,
                     FAV_LIST_LIST_W, FAV_LIST_LIST_H, p_cont,
                     0);
  ctrl_set_rstyle(p_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_list, fav_list_list_keymap);
  ctrl_set_proc(p_list, fav_list_list_proc);
  ctrl_set_mrect(p_list, FAV_LIST_LIST_MIDL, FAV_LIST_LIST_MIDT,
                 FAV_LIST_LIST_MIDL + FAV_LIST_LIST_MIDW, FAV_LIST_LIST_MIDT + FAV_LIST_LIST_MIDH);
  list_set_item_interval(p_list, FAV_LIST_LIST_VGAP);
  list_set_item_rstyle(p_list, &fav_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, FAV_LIST_COUNT, FAV_LIST_LIST_PAGE);
  list_set_field_count(p_list, FAV_LIST_LIST_FIELD, FAV_LIST_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, fav_list_update, 0);
  list_enable_page_mode(p_list, FALSE);

  for(i = 0; i < FAV_LIST_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(fav_list_attr[i].attr),
                        (u16)(fav_list_attr[i].width),
                        (u16)(fav_list_attr[i].left), (u8)(fav_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, fav_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, fav_list_attr[i].fstyle);
  }

  //scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_FAV_LIST_BAR, FAV_LIST_BAR_X,
                            FAV_LIST_BAR_Y, FAV_LIST_BAR_W, FAV_LIST_BAR_H, p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_FAV_LIST_SBAR, RSI_FAV_LIST_SBAR, RSI_FAV_LIST_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_FAV_LIST_SBAR_MID, RSI_FAV_LIST_SBAR_MID,
                     RSI_FAV_LIST_SBAR_MID);
  list_set_scrollbar(p_list, p_sbar);

  fav_list_update(p_list, list_get_valid_pos(p_list), FAV_LIST_LIST_PAGE, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


BEGIN_KEYMAP(fav_list_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_FAVUP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_FAVDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(fav_list_list_keymap, NULL)

BEGIN_MSGPROC(fav_list_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_fav_list_select)
END_MSGPROC(fav_list_list_proc, list_class_proc)

