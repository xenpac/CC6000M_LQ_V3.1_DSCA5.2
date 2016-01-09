/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_prog_list.h"
#include "ui_fav_set.h"

static u16 pos = 0;

enum fav_set_control_id
{
  IDC_INVALID = 0,
  IDC_FAV_SET_LIST,
  IDC_FAV_SET_SBAR,
};

static list_xstyle_t fav_set_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

static list_xstyle_t fav_set_item_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_BLACK,
  FSI_BLACK,
  FSI_BLACK,
};

static list_xstyle_t fav_set_field_rstyle_mid =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
  RSI_LIST_FIELD_MID_HL,
  RSI_LIST_FIELD_MID_HL,
};

static list_field_attr_t fav_set_field_attr[FAV_SET_FIELD_NUM] =
{
  { LISTFIELD_TYPE_ICON | STL_LEFT | STL_VCENTER, 
    24, 16, 0, &fav_set_field_rstyle_mid,  &fav_set_item_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    190, 50, 0, &fav_set_field_rstyle_mid,  &fav_set_item_fstyle },
};

u16 fav_set_list_keymap(u16 key);
RET_CODE fav_set_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);
  

static RET_CODE fav_set_update(control_t* p_list, u16 start, u16 size, 
                               u32 context)
{
  u8 view_id;
  u16 i = 0; 
  u16 cnt = list_get_count(p_list);
  u16 fav_name[MAX_FAV_NAME_LEN];

  for( i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sys_status_get_fav_name((u8)(start + i), fav_name);
      
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, fav_name);
      view_id = ui_dbase_get_pg_view_id();
      if(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, (i + start)))
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 0, IM_TV_FAV);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 0, RSC_INVALID_ID);
      }
    }
  }
  return SUCCESS;
}

static RET_CODE on_fav_set_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u8 view_id;
  u16 focus = list_get_focus_pos(p_list);
  u16 param;
  
  view_id = ui_dbase_get_pg_view_id();
  //if(db_dvbs_get_mark_value(view_id, pos, (DB_DVBS_MARK_FAV_1 + focus)))
  if(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, focus) == TRUE)
  {
    param = (focus & (~DB_DVBS_PARAM_ACTIVE_FLAG));
    db_dvbs_change_mark_status(view_id, pos, DB_DVBS_FAV_GRP, param);
    //db_dvbs_view_mark(view_id, pos, (DB_DVBS_MARK_FAV_1 + focus), FALSE);
    list_set_field_content_by_icon(p_list, focus, 0, 0);
  }
  else
  {
    param = (focus | DB_DVBS_PARAM_ACTIVE_FLAG); 
    db_dvbs_change_mark_status(view_id, pos, DB_DVBS_FAV_GRP, param);
    //db_dvbs_view_mark(view_id, pos, (DB_DVBS_MARK_FAV_1 + focus), TRUE);
    list_set_field_content_by_icon(p_list, focus, 0, IM_TV_FAV);//IM_FAV);
  }        
  list_draw_field_ext(p_list, focus, 0, TRUE);
  
  plist_set_modify_state(TRUE);  

  return ret;
}

static RET_CODE on_fav_set_exit(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  //refresh channel list
  fw_notify_parent(ROOT_ID_FAV_SET, NOTIFY_T_MSG, FALSE, 
                        MSG_FAV_UPDATE, pos, 0);
  
  ret = manage_close_menu(ROOT_ID_FAV_SET, 0, 0); 

  return ret;
}

RET_CODE open_fav_set(u32 para1, u32 para2)
{
  control_t *p_menu, *p_list, *p_sbar;
  u16 i = 0;
  u16 total = 0;
  pos = (u16)para2;
  
  p_menu = fw_create_mainwin(ROOT_ID_FAV_SET,
                           FAV_SET_X, FAV_SET_Y, FAV_SET_W, FAV_SET_H,
                           (u8)para1, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_menu == NULL)
  {
    return ERR_FAILURE;
  }  
  ctrl_set_rstyle(p_menu, RSI_FAV_SET_MENU, RSI_FAV_SET_MENU, RSI_FAV_SET_MENU);

    total = FAV_SET_ITEM_TOL;
  
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_FAV_SET_LIST, 
          FAV_SET_LIST_X, FAV_SET_LIST_Y, FAV_SET_LIST_W, FAV_SET_LIST_H, 
          p_menu, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, fav_set_list_keymap);
  ctrl_set_proc(p_list, fav_set_list_proc);
  ctrl_set_mrect(p_list, FAV_SET_LIST_MIDL, FAV_SET_LIST_MIDT,
                    FAV_SET_LIST_MIDW + FAV_SET_LIST_MIDL, FAV_SET_LIST_MIDH + FAV_SET_LIST_MIDT);
  list_set_item_interval(p_list, FAV_SET_LIST_VGAP);
  list_set_item_rstyle(p_list, &fav_set_item_rstyle);
  list_set_count(p_list, total, FAV_SET_ITEM_PAGE);
  list_set_field_count(p_list, FAV_SET_FIELD_NUM, FAV_SET_ITEM_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, fav_set_update, 0);
 
  for (i = 0; i < FAV_SET_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, 
                        (u32)(fav_set_field_attr[i].attr), 
                        (u16)(fav_set_field_attr[i].width),
                        (u16)(fav_set_field_attr[i].left), 
                        (u8)(fav_set_field_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, fav_set_field_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, fav_set_field_attr[i].fstyle);
  }

  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_FAV_SET_SBAR, 
    FAV_SET_SBARX - 6, FAV_SET_SBARY, FAV_SET_SBARW, FAV_SET_SBARH, 
    p_menu, 0);
  ctrl_set_rstyle(p_sbar, RSI_FAV_SET_SBAR, RSI_FAV_SET_SBAR, RSI_FAV_SET_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_FAV_SET_SBAR_MID, RSI_FAV_SET_SBAR_MID,
                     RSI_FAV_SET_SBAR_MID);
  //ctrl_set_mrect(p_sbar, 0, 12, FAV_SET_SBARW, FAV_SET_SBARH - 12);
  list_set_scrollbar(p_list, p_sbar);      

  fav_set_update(p_list, list_get_valid_pos(p_list), FAV_SET_ITEM_PAGE, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  return SUCCESS;
  
}

BEGIN_KEYMAP(fav_set_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP) //fix bug 16246
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(fav_set_list_keymap, NULL)

BEGIN_MSGPROC(fav_set_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_fav_set_select)
  ON_COMMAND(MSG_EXIT, on_fav_set_exit)
END_MSGPROC(fav_set_list_proc, list_class_proc)

