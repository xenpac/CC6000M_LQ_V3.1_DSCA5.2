/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_category.h"

enum category_control_id
{
  IDC_INVALID = 0,
  IDC_CATEGORY_PREV,
  IDC_CATEGORY_GROUP,
  IDC_CATEGORY_LIST,
  IDC_CATEGORY_BAR,
};

static list_xstyle_t category_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t category_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t category_field_rstyle_left =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
};

static list_xstyle_t category_field_rstyle_mid =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
};


static list_field_attr_t category_attr[CATEGORY_LIST_FIELD] =
{
  {LISTFIELD_TYPE_DEC| STL_CENTER| STL_VCENTER,
   60, 0, 0, &category_field_rstyle_left, &category_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
   580, 60, 0, &category_field_rstyle_mid, &category_field_fstyle},
};

u16 category_list_keymap(u16 key);
RET_CODE category_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE category_update(control_t *ctrl, u16 start, u16 size, u32 context)
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
      sys_status_get_fav_name((u8)(start + i), uni_str);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_category_select(control_t *p_ctrl,
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
    return manage_open_menu(ROOT_ID_SMALL_LIST, V_KEY_FAV, ((u32)((focus + DB_DVBS_MAX_SAT + 1) )<< 16) | focus  /*ALL*/);
  }
}

RET_CODE open_category(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_list;//, *p_preview;
  control_t *p_group;
  control_t *p_sbar;
  u8 i;
  
#ifdef CUSTOMER_HCI
  p_cont = ui_comm_root_create(ROOT_ID_CATEGORY,
                               0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
                               COMM_BG_H, IM_TITLEICON_TV, IDS_CATEGORY_HK);
#else
  p_cont = ui_comm_root_create(ROOT_ID_CATEGORY,
                               0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
                               COMM_BG_H, IM_TITLEICON_TV, IDS_CATEGORY);
#endif

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  #if 0
  //preview
  p_preview =
    ctrl_create_ctrl(CTRL_CONT, IDC_CATEGORY_PREV, CATEGORY_PREV_X, CATEGORY_PREV_Y,
                     CATEGORY_PREV_W, CATEGORY_PREV_H, p_cont,
                     0);
  ctrl_set_rstyle(p_preview, RSI_CATEGORY_PREV, RSI_CATEGORY_PREV, RSI_CATEGORY_PREV);
#endif
  //group
  p_group = ctrl_create_ctrl(CTRL_TEXT, IDC_CATEGORY_GROUP, CATEGORY_GROUP_X,
                             CATEGORY_GROUP_Y, CATEGORY_GROUP_W,
                             CATEGORY_GROUP_H, p_cont,
                             0);
  ctrl_set_rstyle(p_group, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  text_set_align_type(p_group , STL_CENTER|STL_VCENTER);
  text_set_font_style(p_group , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_group, TEXT_STRTYPE_STRID);
  #ifdef CUSTOMER_HCI
  text_set_content_by_strid(p_group , IDS_GROUP_HK);
  #else
  text_set_content_by_strid(p_group , IDS_GROUP);
  #endif

  //prog list
  p_list =
    ctrl_create_ctrl(CTRL_LIST, IDC_CATEGORY_LIST, CATEGORY_LIST_X, CATEGORY_LIST_Y,
                     CATEGORY_LIST_W, CATEGORY_LIST_H, p_cont,
                     0);
  ctrl_set_rstyle(p_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_list, category_list_keymap);
  ctrl_set_proc(p_list, category_list_proc);
  ctrl_set_mrect(p_list, CATEGORY_LIST_MIDL, CATEGORY_LIST_MIDT,
                 CATEGORY_LIST_MIDL + CATEGORY_LIST_MIDW, CATEGORY_LIST_MIDT + CATEGORY_LIST_MIDH);
  list_set_item_interval(p_list, CATEGORY_LIST_VGAP);
  list_set_item_rstyle(p_list, &category_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  if(sys_status_get_categories_count() > FAV_LIST_COUNT)
  {
    list_set_count(p_list, sys_status_get_categories_count() - 5, CATEGORY_LIST_PAGE);
  } 
  list_set_field_count(p_list, CATEGORY_LIST_FIELD, CATEGORY_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, category_update, 0);
  list_enable_page_mode(p_list, FALSE);

  for(i = 0; i < CATEGORY_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(category_attr[i].attr),
                        (u16)(category_attr[i].width),
                        (u16)(category_attr[i].left), (u8)(category_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, category_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, category_attr[i].fstyle);
  }

  //scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_CATEGORY_BAR, CATEGORY_BAR_X,
                            CATEGORY_BAR_Y, CATEGORY_BAR_W, CATEGORY_BAR_H, p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_CATEGORY_SBAR, RSI_CATEGORY_SBAR, RSI_CATEGORY_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_CATEGORY_SBAR_MID, RSI_CATEGORY_SBAR_MID,
                     RSI_CATEGORY_SBAR_MID);
  list_set_scrollbar(p_list, p_sbar);

  category_update(p_list, list_get_valid_pos(p_list), CATEGORY_LIST_PAGE, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(category_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(category_list_keymap, NULL)

BEGIN_MSGPROC(category_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_category_select)
END_MSGPROC(category_list_proc, list_class_proc)

