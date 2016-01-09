/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_prog_list.h"
#include "ui_sort_list.h"

enum fav_set_control_id
{
  IDC_INVALID = 0,
  IDC_SORT_LIST_LIST,
  IDC_SORT_LIST_SBAR,
};

static list_xstyle_t sort_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

/*static list_xstyle_t sort_list_item_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_BLACK,
  FSI_BLACK,
  FSI_BLACK,
};*/

static list_field_attr_t sort_list_field_attr[SORT_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    SORT_LIST_LIST_MIDW - 5, 5, 0, NULL, NULL} //&sort_list_item_rstyle, &sort_list_item_fstyle },
};

u16 sort_list_keymap(u16 keymap);
RET_CODE sort_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

static RET_CODE sort_list_update(control_t* p_list, u16 start, u16 size, 
                                 u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u16 strid [5] = {
    IDS_SORT_NAME_AZ,
    IDS_SORT_NAME_ZA,
    IDS_SORT_CA_FLAG,
    IDS_SORT_LOCK,
    IDS_SORT_DEFAULT,
  };

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_strid(p_list, (u16)(start + i), 0,
                                      strid[start + i]);
    }
  }
  return SUCCESS;
}

static RET_CODE on_sort_list_select(control_t *p_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 focus;
  u8 view_id;

  focus = list_get_focus_pos(p_list);
  view_id = ui_dbase_get_pg_view_id();

  //sort
  db_dvbs_pg_sort(view_id, (dvbs_sort_type_t)focus);

  //close sort list and set plist state.
  ret = fw_notify_parent(ROOT_ID_SORT_LIST, NOTIFY_T_MSG, FALSE,
                        MSG_SORT_UPDATE, MSG_SELECT, 0);

  ret = manage_close_menu(ROOT_ID_SORT_LIST, 0, 0);

  return ret;
}

static RET_CODE on_sort_list_exit(control_t *p_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;

  fw_notify_parent(ROOT_ID_SORT_LIST, NOTIFY_T_MSG, FALSE,
    MSG_SORT_UPDATE, MSG_EXIT, 0);

  ret = manage_close_menu(ROOT_ID_SORT_LIST, 0, 0);

  return ret;
}

static RET_CODE on_sort_list_destory(control_t *p_list, u16 msg, u32 para1,
                               u32 para2)
{
  db_dvbs_pg_sort_deinit();
  
  return ERR_NOFEATURE;
}

RET_CODE open_sort_list(u32 para1, u32 para2)
{
  control_t *p_menu, *p_list, *p_sbar;
  u16 i = 0;

  p_menu = fw_create_mainwin(ROOT_ID_SORT_LIST,
                                  SORT_LIST_X, SORT_LIST_Y, SORT_LIST_W,
                                  SORT_LIST_H,
                                  (u8)para1, 0, OBJ_ATTR_ACTIVE,
                                  0);
  if (p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_menu, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);

  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_SORT_LIST_LIST,
                            SORT_LIST_LIST_X, SORT_LIST_LIST_Y,
                            SORT_LIST_LIST_W, SORT_LIST_LIST_H,
                            p_menu,
                            0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, sort_list_keymap);
  ctrl_set_proc(p_list, sort_list_proc);
  ctrl_set_mrect(p_list, SORT_LIST_LIST_MIDL, SORT_LIST_LIST_MIDT,
    SORT_LIST_LIST_MIDL + SORT_LIST_LIST_MIDW, SORT_LIST_LIST_MIDT + SORT_LIST_LIST_MIDH);
  list_set_item_interval(p_list, SORT_LIST_LIST_VGAP);
  list_set_item_rstyle(p_list, &sort_list_item_rstyle);
  list_set_count(p_list, SORT_LIST_ITEM_TOL, SORT_LIST_ITEM_PAGE);
  list_set_field_count(p_list, SORT_LIST_FIELD_NUM, SORT_LIST_ITEM_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, sort_list_update, 0);

  for (i = 0; i < SORT_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i,
                        (u32)(sort_list_field_attr[i].attr),
                        (u16)(sort_list_field_attr[i].width),
                        (u16)(sort_list_field_attr[i].left),
                        (u8)(sort_list_field_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, sort_list_field_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, sort_list_field_attr[i].fstyle);
  }

  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_SORT_LIST_SBAR,
                            SORT_LIST_SBARX, SORT_LIST_SBARY, SORT_LIST_SBARW,
                            SORT_LIST_SBARH,
                            p_menu,
                            0);
  ctrl_set_rstyle(p_sbar, RSI_SORT_LIST_SBAR, RSI_SORT_LIST_SBAR,
                  RSI_SORT_LIST_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SORT_LIST_SBAR_MID, RSI_SORT_LIST_SBAR_MID,
                     RSI_SORT_LIST_SBAR_MID);
  //ctrl_set_mrect(p_sbar, 0, 12, SORT_LIST_SBARW, SORT_LIST_SBARH - 12);
  list_set_scrollbar(p_list, p_sbar);

  sort_list_update(p_list, list_get_valid_pos(p_list), SORT_LIST_ITEM_PAGE, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_menu, TRUE);

  db_dvbs_pg_sort_init(ui_dbase_get_pg_view_id());

  return SUCCESS;
}

BEGIN_KEYMAP(sort_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(sort_list_keymap, NULL)

BEGIN_MSGPROC(sort_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_sort_list_select)
  ON_COMMAND(MSG_EXIT, on_sort_list_exit)
  ON_COMMAND(MSG_DESTROY, on_sort_list_destory)
END_MSGPROC(sort_list_proc, list_class_proc)
