/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/

#include "ui_common.h"

#include "ui_find.h"
#include "ui_keyboard_ctrl.h"

enum find_control_id
{
  IDC_FIND_KEYBOARD_CONT = 1,
  IDC_FIND_LIST_CONT,
};

enum find_control_list_id
{
  IDC_FIND_TOP_LINE = 1,
  IDC_FIND_BOTTOM_LINE,
  IDC_FIND_LIST,
  IDC_FIND_SBAR,
};

static list_xstyle_t find_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
};

static list_xstyle_t find_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_BLACK,
  FSI_BLACK,
  FSI_BLACK,
};


static list_xstyle_t find_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};


static list_field_attr_t find_attr[FIND_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_DEC,
    48, 0, 0, &find_field_rstyle,  &find_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    230, 48, 0, &find_field_rstyle,  &find_field_fstyle },
};

enum local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 100,
};

#if 0
static BOOL g_is_upper = FALSE;
#endif
static u8 find_view;
static u8 find_sub_view;

static u16 find_cont_keymap(u16 key);
static u16 find_list_keymap(u16 key);

static RET_CODE find_cont_proc(control_t *ctrl, u16 msg, u32 para1,
                               u32 para2);
static RET_CODE find_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

static void flist_play_pg(u16 focus)
{
#if 0  
  u16 rid;

  if ((rid =
         db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                    focus)) != INVALIDID)
  {
    ui_play_prog(rid, FALSE);
  }
#else
  u16 rid;

  if ((rid =
         db_dvbs_get_id_by_view_pos(find_sub_view, focus)) != INVALIDID)
  {
    ui_play_prog(rid, FALSE);
  }
#endif
}

static RET_CODE find_list_update(control_t* ctrl, u16 start, u16 size, 
                                 u32 context)
{
  dvbs_prog_node_t curn_prog;
  u16 i;
  u16 uni_str[32];
  u16 pg_id, cnt = list_get_count(ctrl);

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(find_sub_view, (u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &curn_prog);

      // NO. 
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0,
                                    (u16)(start + i + 1 )); //program number in list

      // NAME 
      ui_dbase_get_full_prog_name(&curn_prog, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name
    }
  }
  return SUCCESS;
}

RET_CODE open_find(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list_cont;
  control_t *p_list, *p_sbar;
  control_t *p_top_line, *p_bottom_line;
  u16 total = 0;
  u16 list_focus = 0;
  u16 i = 0;

#if 0
  g_is_upper = FALSE;
#endif

  find_view = find_sub_view = ui_dbase_get_pg_view_id();

  total = db_dvbs_get_count(find_sub_view);
  
  p_cont =
    fw_create_mainwin((u8)(ROOT_ID_FIND),
                           FIND_MENU_CONT_X, FIND_MENU_CONT_Y,
                           FIND_MENU_CONT_WIDTH, FIND_MENU_CONT_HEIGHT,
                           0, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_FIND_CONT, RSI_FIND_CONT, RSI_FIND_CONT);
  ctrl_set_keymap(p_cont, find_cont_keymap);
  ctrl_set_proc(p_cont, find_cont_proc);

  //list cont
  p_list_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_FIND_LIST_CONT, 
                     FIND_LIST_CONT_X, FIND_LIST_CONT_Y,
                     FIND_LIST_CONT_W, FIND_LIST_CONT_H, p_cont,
                     0);
  ctrl_set_rstyle(p_list_cont, RSI_FIND_LIST_CONT, RSI_FIND_LIST_CONT, RSI_FIND_LIST_CONT);

  //top line
  p_top_line = ctrl_create_ctrl(CTRL_TEXT, IDC_FIND_TOP_LINE, 
                    FIND_TOP_LINE_X, FIND_TOP_LINE_Y, FIND_TOP_LINE_W, FIND_TOP_LINE_H, 
                    p_list_cont, 0);
  ctrl_set_rstyle(p_top_line, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);

  //bottom line
  p_bottom_line = ctrl_create_ctrl(CTRL_TEXT, IDC_FIND_BOTTOM_LINE, 
                    FIND_BOTTOM_LINE_X, FIND_BOTTOM_LINE_Y, 
                    FIND_BOTTOM_LINE_W, FIND_BOTTOM_LINE_H, 
                    p_list_cont, 0);
  ctrl_set_rstyle(p_bottom_line, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  
  //create program list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_FIND_LIST, 
                     FIND_LIST_X, FIND_LIST_Y,
                     FIND_LIST_W, FIND_LIST_H, p_list_cont,
                     0);
  ctrl_set_rstyle(p_list, RSI_FIND_LIST, RSI_FIND_LIST, RSI_FIND_LIST);
  ctrl_set_keymap(p_list, find_list_keymap);
  ctrl_set_proc(p_list, find_list_proc);
  ctrl_set_mrect(p_list, FIND_LIST_MID_L, FIND_LIST_MID_T,
    FIND_LIST_MID_W + FIND_LIST_MID_L, FIND_LIST_MID_H + FIND_LIST_MID_T);
  list_set_item_interval(p_list, FIND_MENU_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &find_item_rstyle);
  list_set_count(p_list, total, FIND_LIST_PAGE);
  list_set_field_count(p_list, FIND_LIST_FIELD_NUM, FIND_LIST_PAGE);
  list_set_focus_pos(p_list, list_focus);
  list_set_update(p_list, find_list_update, 0);
 
  for (i = 0; i < FIND_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(find_attr[i].attr), (u16)(find_attr[i].width),
                        (u16)(find_attr[i].left), (u8)(find_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, find_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, find_attr[i].fstyle);
  }

  //create scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_FIND_SBAR,
                     FIND_SBAR_X, FIND_SBAR_Y,
                     FIND_SBAR_W, FIND_SBAR_H,
                     p_list_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_FIND_SBAR, RSI_FIND_SBAR, RSI_FIND_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_FIND_SBAR_MID, RSI_FIND_SBAR_MID,
                     RSI_FIND_SBAR_MID);
  ctrl_set_mrect(p_sbar, 0, FIND_SBAR_VERTEX_GAP, 
    FIND_SBAR_W, FIND_SBAR_H - FIND_SBAR_VERTEX_GAP);
  list_set_scrollbar(p_list, p_sbar);
 
  find_list_update(p_list, list_get_valid_pos(p_list), FIND_LIST_PAGE, 0);
  
  ui_keyboard_create(p_cont, IDC_FIND_KEYBOARD_CONT, 
                    FIND_EDIT_CONTX, FIND_EDIT_CONTY, 
                    FIND_EDIT_CONTW, FIND_EDIT_CONTH, 
                    ROOT_ID_FIND);

  ui_keyboard_enter_edit(p_cont, IDC_FIND_KEYBOARD_CONT);
  ui_keyboard_setfocus(p_cont, IDC_FIND_KEYBOARD_CONT);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_find_input_changed(control_t *ctrl, u16 msg, u32 para1,
                                   u32 para2)
{
  u16 total;
  control_t *p_list; 
  
  //Fix limited find sub-view 
  if(find_sub_view != find_view)
  {
    db_dvbs_destroy_view(find_sub_view);
  }

  if(uni_strlen((u16 *)para1)!= 0)
  {
    db_dvbs_find(find_view, (u16 *)para1, &find_sub_view);
  }
  else
  { 
    find_sub_view = find_view;
  }

  total = db_dvbs_get_count(find_sub_view); 

  p_list = ctrl_get_child_by_id(ctrl_get_child_by_id(ctrl, IDC_FIND_LIST_CONT), IDC_FIND_LIST);
  list_set_count(p_list, total, FIND_LIST_PAGE);
  list_set_focus_pos(p_list, 0);

  find_list_update(p_list, list_get_valid_pos(p_list), FIND_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static RET_CODE on_find_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  flist_play_pg(list_get_focus_pos(p_list));

  return SUCCESS;
}

static RET_CODE on_find_hide_keyboard(control_t *p_mask, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_keyboard_cont, *p_list;
  p_keyboard_cont = ctrl_get_child_by_id( p_mask, IDC_FIND_KEYBOARD_CONT);
  p_list = ctrl_get_child_by_id( ctrl_get_child_by_id( p_mask, IDC_FIND_LIST_CONT), IDC_FIND_LIST);

  //ctrl_set_attr(p_keyboard_cont, OBJ_ATTR_HIDDEN);
  ctrl_process_msg(p_keyboard_cont, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_list, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_list, OBJ_STS_SHOW);
  
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_mask, TRUE);
  return SUCCESS;
}

static RET_CODE on_find_tofind_keyboard(control_t *p_list, u16 msg,
										u32 para1, u32 para2)
{

	control_t *p_keyboard_cont,*p_list_cont,*p_mainwin;	
	p_list_cont = ctrl_get_parent(p_list);
	p_mainwin = ctrl_get_parent(p_list_cont);
	p_keyboard_cont = ctrl_get_child_by_id( p_mainwin, IDC_FIND_KEYBOARD_CONT);

	ctrl_set_attr(p_keyboard_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_keyboard_cont, OBJ_STS_SHOW);
  
	ctrl_process_msg(p_keyboard_cont, MSG_GETFOCUS, 0, 0);
	ctrl_process_msg(p_list_cont, MSG_LOSTFOCUS, 0, 0);

	ctrl_paint_ctrl(p_mainwin, TRUE);
	return SUCCESS;
}

BEGIN_KEYMAP(find_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_RED, MSG_SWITCH_CAPS)
  ON_EVENT(V_KEY_GREEN, MSG_BACKSPACE)
END_KEYMAP(find_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(find_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_INPUT_CHANGED, on_find_input_changed)
  ON_COMMAND(MSG_HIDE_KEYBOARD, on_find_hide_keyboard)
END_MSGPROC(find_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(find_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)
  ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_BLUE, MSG_CANCEL)
  ON_EVENT(V_KEY_YELLOW, MSG_CANCEL)
END_KEYMAP(find_list_keymap, NULL)

BEGIN_MSGPROC(find_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_find_list_select)	
  ON_COMMAND(MSG_CANCEL, on_find_tofind_keyboard)
END_MSGPROC(find_list_proc, list_class_proc)


