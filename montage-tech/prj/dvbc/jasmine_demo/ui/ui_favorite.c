/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_favorite.h"

#include "ui_rename_v2.h"

enum favorite_ctrl_id
{
  IDC_FAVORITE_LIST = 1,
  IDC_FAVORITE_SBAR,
};

static list_xstyle_t favorite_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

static list_xstyle_t favorite_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_BLACK,
  FSI_BLACK,
  FSI_BLACK,
};

static list_xstyle_t favorite_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t favorite_list_attr[FAVORITE_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    160, 20, 0, &favorite_list_field_rstyle,  &favorite_list_field_fstyle},
    
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    300, 180, 0, &favorite_list_field_rstyle,  &favorite_list_field_fstyle},
};

static u16 favorite_list_keymap(u16 key);
                             
static RET_CODE favorite_cont_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);
static RET_CODE favorite_list_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);

 /*
static comm_help_data_t favorite_help_data = 
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
static RET_CODE favorite_list_update(control_t* p_list, u16 start, u16 size, 
                                     u32 context)
{
  u8 i;
  u16 cnt = list_get_count(p_list);  
  u16 name[MAX_FAV_NAME_LEN];
  u8 asc_str[8];
  u8 tkgs_category_num = sys_status_get_category_num();

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sprintf((char *)asc_str, "%d", (start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      if((tkgs_category_num > start) && (i < tkgs_category_num - start))
      sys_status_get_fav_name((u8)(start + i + MAX_PRESET_FAV_CNT), name);
      else
      sys_status_get_fav_name((u8)(start + i - tkgs_category_num), name);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, name);
    }
  }
  
  return SUCCESS;
}

RET_CODE open_favorite(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_sbar;
  u8 i;
  u8 tkgs_category_num = 0;
  
#if 1//#ifndef SPT_SUPPORT
  p_cont =
    ui_comm_root_create(ROOT_ID_FAVORITE, 0, COMM_BG_X, COMM_BG_Y, 
    COMM_BG_W,  COMM_BG_H, IM_TITLEICON_TV, IDS_FAVORITE);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_cont, favorite_cont_proc);

  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_FAVORITE_LIST, 
    FAVORITE_LIST_X, FAVORITE_LIST_Y, 
    FAVORITE_LIST_W, FAVORITE_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_FAVORITE_LIST, RSI_FAVORITE_LIST, RSI_FAVORITE_LIST);
  ctrl_set_keymap(p_list, favorite_list_keymap);
  ctrl_set_proc(p_list, favorite_list_proc);
  ctrl_set_mrect(p_list, FAVORITE_LIST_MID_L, FAVORITE_LIST_MID_T,
    FAVORITE_LIST_MID_W + FAVORITE_LIST_MID_L, FAVORITE_LIST_MID_H + FAVORITE_LIST_MID_T);
  list_set_item_interval(p_list, FAVORITE_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_list, &favorite_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_count(p_list, MAX_PRESET_FAV_CNT+ tkgs_category_num, FAVORITE_LIST_PAGE);
  list_set_field_count(p_list, FAVORITE_LIST_FIELD, FAVORITE_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, favorite_list_update, 0);
  ctrl_set_style(p_list, STL_EX_ALWAYS_HL);
 
  for (i = 0; i < FAVORITE_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(favorite_list_attr[i].attr), 
      (u16)(favorite_list_attr[i].width),
      (u16)(favorite_list_attr[i].left), 
      (u8)(favorite_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, favorite_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, favorite_list_attr[i].fstyle);
  }    

  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_FAVORITE_SBAR, 
    FAVORITE_SBAR_X, FAVORITE_SBAR_Y, 
    FAVORITE_SBAR_WIDTH, FAVORITE_SBAR_HEIGHT, 
    p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_FAVORITE_SBAR_BG,
                  RSI_FAVORITE_SBAR_BG, RSI_FAVORITE_SBAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_FAVORITE_SBAR_MID,
                     RSI_FAVORITE_SBAR_MID, RSI_FAVORITE_SBAR_MID);
  /*ctrl_set_mrect(p_sbar, 0, FAVORITE_SBAR_VERTEX_GAP,
                    FAVORITE_SBAR_WIDTH,
                    FAVORITE_SBAR_HEIGHT - FAVORITE_SBAR_VERTEX_GAP);*/
  list_set_scrollbar(p_list, p_sbar);
  
  favorite_list_update(p_list, list_get_valid_pos(p_list), FAVORITE_LIST_PAGE, 0);
#else
#endif

  //ui_comm_help_create(&favorite_help_data, p_cont);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

static RET_CODE on_favorite_rename_update(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list;
  u16 *uni_str = (u16 *)para1;
  u16 focus;
  
  p_list = ctrl_get_child_by_id(p_ctrl, IDC_FAVORITE_LIST);
  MT_ASSERT(p_list != NULL);

  focus = list_get_focus_pos(p_list);

  list_set_field_content_by_unistr(p_list, focus, 1, uni_str);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

  list_draw_item_ext(p_list, focus, TRUE);

  sys_status_set_fav_name((u8)focus - sys_status_get_category_num(), uni_str);
  sys_status_save();
  
  return SUCCESS;

}

static RET_CODE on_favorite_rename_check(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list;
  u16 *uni_str = (u16 *)para1;
  u16 name[MAX_FAV_NAME_LEN];
  u16 focus;
  u8 i = 0;
    
  p_list = ctrl_get_child_by_id(p_ctrl, IDC_FAVORITE_LIST);
  MT_ASSERT(p_list != NULL);

  focus = list_get_focus_pos(p_list);

  for( i = 0; i < MAX_FAV_CNT; i++)
  {
    sys_status_get_fav_name(i, name);

    if(!uni_strcmp(name, uni_str) && (i != focus))
    {
      return ERR_NOFEATURE;
    }
  }

  return SUCCESS;
}

#if 0
static RET_CODE on_favorite_list_update(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list;
  u16 focus;
  
  p_list = ctrl_get_child_by_id(p_ctrl, IDC_FAVORITE_LIST);
  MT_ASSERT(p_list != NULL);
  focus = list_get_focus_pos(p_list);
  list_select_item(p_list, focus);
  list_draw_item_ext(p_list, focus, TRUE);
  
  return SUCCESS;
}
#endif

static RET_CODE on_favorite_list_yes(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  rename_param_t_v2 re_name;
  u16 pos;

  //open a dialog for rename.
  memset(&re_name, 0, sizeof(rename_param_t_v2));
  re_name.max_len = MAX_FAV_NAME_LEN - 1;
  re_name.uni_str = (u16 *)list_get_field_content(p_ctrl, list_get_focus_pos(p_ctrl), 1);

  pos = list_get_focus_pos(p_ctrl);
  if(sys_status_get_category_num() > 0)
  {
  if(pos <  sys_status_get_category_num())
  return SUCCESS;
  }
  manage_open_menu(ROOT_ID_RENAME_V2, (u32)&re_name, ROOT_ID_FAVORITE);
  
  return SUCCESS;
}

BEGIN_MSGPROC(favorite_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_RENAME_UPDATE, on_favorite_rename_update)
	ON_COMMAND(MSG_RENAME_CHECK, on_favorite_rename_check)
	//ON_COMMAND(MSG_FAV_UPDATE, on_favorite_list_update)
END_MSGPROC(favorite_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(favorite_list_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(favorite_list_keymap, NULL)

BEGIN_MSGPROC(favorite_list_proc, list_class_proc)
	ON_COMMAND(MSG_YES, on_favorite_list_yes)
END_MSGPROC(favorite_list_proc, list_class_proc)

