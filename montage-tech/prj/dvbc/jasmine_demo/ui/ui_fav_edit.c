/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_mainmenu.h"
#include "ui_fav_edit.h"
#include "ui_volume_usb.h"
#include "ui_mute.h"

enum eidt_control_id
{
  IDC_FAVGROUP_CONT = 1,
  IDC_FAV_EDIT_ID,
  IDC_FAV_EDIT_TV_WIN,
  IDC_EXCHANGE,
  IDC_ADD,
  IDC_GROUP,
  IDC_MOVE,
  IDC_ICON_RED,
  IDC_ICON_YELLOW,
  IDC_ICON_GREEN,
  IDC_ICON_MOVE,
  IDC_FAV_EDIT_PG_HEAD,
  IDC_FAV_EDIT_PG_LIST,
  IDC_FAV_EDIT_PG_BAR,
  IDC_FAV_EDIT_FAV_HEAD,
  IDC_FAV_EDIT_FAV_LIST,
  IDC_FAV_EDIT_FAV_BAR,
};

enum favgroup_control_id
{
  IDC_FAV_EDIT_FAVGROUP_HEAD = 1,
  IDC_FAV_EDIT_FAVGROUP_LIST,
};

enum prv_msg_t
{
  MSG_FAV_GROUP = MSG_USER_BEGIN + 200,
  MSG_ADD,
  MSG_EXCH,
  MSG_DEL_FAV,
  MSG_VOLUME_DEC,
  MSG_VOLUME_INC,
};

static RET_CODE fav_edit_list_update(control_t* ctrl, u16 start, u16 size, u32 context);

static list_xstyle_t favgroup_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t favedit_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t favedit_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t favedit_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t favedit_fav_pg_list_attr[FAV_EDIT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
  60, 10, 0, &favedit_field_rstyle,  &favedit_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
  150, 70, 0, &favedit_field_rstyle,  &favedit_field_fstyle},  

  { LISTFIELD_TYPE_ICON,
  30, 220, 0, &favedit_field_rstyle,  &favedit_field_fstyle},

  { LISTFIELD_TYPE_ICON,
  30, 250, 0, &favedit_field_rstyle,  &favedit_field_fstyle},

  { LISTFIELD_TYPE_ICON,
  30, 280, 0, &favedit_field_rstyle,  &favedit_field_fstyle},
};

static list_field_attr_t favedit_all_pg_list_attr[FAV_EDIT_PROG_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
  80, 10, 0, &favedit_field_rstyle,  &favedit_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
  250, 90, 0, &favedit_field_rstyle,  &favedit_field_fstyle},

  { LISTFIELD_TYPE_ICON,
  30, 340, 0, &favedit_field_rstyle,  &favedit_field_fstyle},

  { LISTFIELD_TYPE_ICON,
  30, 370, 0, &favedit_field_rstyle,  &favedit_field_fstyle},

  { LISTFIELD_TYPE_ICON,
  30, 390, 0, &favedit_field_rstyle,  &favedit_field_fstyle},
  
};


static list_field_attr_t group_list_attr[FAV_EDIT_FAVGROUP_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
  FAV_EDIT_FAVGROUP_LIST_MIDW - FAV_EDIT_FAVGROUP_LIST_MIDL, 0, 0, &favedit_field_rstyle,  &favedit_field_fstyle },
};
#ifdef CUSTOMER_YINHE_TR
static comm_help_data_t2 fav_edit_help_data = //help bar data
{
  8, 120, {40, 140, 40, 160, 40, 160, 40,140},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_EXCHANGE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_ADD,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_FAV,
    HELP_RSC_BMP   | IM_F4,
    HELP_RSC_STRID | IDS_TV_RADIO,
  },
};
#else
static comm_help_data_t2 fav_edit_help_data = //help bar data
{
  8, 120, {40, 140, 40, 160, 40, 160, 40,140},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_EXCHANGE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_ADD,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_FAV_LIST,
    HELP_RSC_BMP   | IM_TVRADIO,
    HELP_RSC_STRID | IDS_TV_RADIO,
  },
};
#endif
 static  comm_dlg_data_t channel_edit_exit_data = //popup dialog data
  {
    ROOT_ID_FAV_EDIT,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_MSG_ASK_FOR_SAV,
    0,
  };
 
static u16 g_fav_index = 0;
static BOOL g_modify = FALSE;
static  BOOL is_give_up = FALSE;

u16 fav_edit_cont_keymap(u16 key);
RET_CODE fav_edit_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 fav_edit_pglist_keymap(u16 key);
RET_CODE fav_edit_pglist_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 fav_edit_favlist_keymap(u16 key);
RET_CODE fav_edit_favlist_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 fav_edit_grouplist_keymap(u16 key);
RET_CODE fav_edit_grouplist_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static u16 fav_edit_pwdlg_keymap(u16 key);
RET_CODE fav_edit_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

extern BOOL have_logic_number(void);

static RET_CODE on_fav_set_attr(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_favgroup_cont, *p_favgroup_list, *p_pg_list;

  p_favgroup_cont = ctrl_get_child_by_id(p_cont, IDC_FAVGROUP_CONT);
  p_pg_list = ctrl_get_child_by_id(p_cont, IDC_FAV_EDIT_PG_LIST);
  
  if( ctrl_get_sts(p_favgroup_cont) != OBJ_STS_HIDE)
  {
    //p_favgroup_list = ctrl_get_child_by_id(p_favgroup_cont, IDC_FAV_EDIT_FAVGROUP_LIST);

    ctrl_process_msg(ctrl_get_active_ctrl(p_favgroup_cont), MSG_LOSTFOCUS, para1, para2);

    ctrl_process_msg(p_pg_list, MSG_GETFOCUS, 0, 0);

     ctrl_set_sts(p_favgroup_cont, OBJ_STS_HIDE);
    ctrl_paint_ctrl(p_cont, TRUE);
  }
  else
  {
    p_favgroup_list = ctrl_get_child_by_id(p_favgroup_cont, IDC_FAV_EDIT_FAVGROUP_LIST);

    ctrl_process_msg(ctrl_get_active_ctrl(p_cont), MSG_LOSTFOCUS, para1, para2);
    
    ctrl_set_attr(p_favgroup_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_favgroup_cont, OBJ_STS_SHOW);
    ctrl_process_msg(p_favgroup_list, MSG_GETFOCUS, 0, 0);
    
    ctrl_paint_ctrl(p_favgroup_cont, TRUE);
  }
  #if 0
  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_list);
  
  if(msg == MSG_FAV_SET)
  {
    if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, FAV0) == TRUE)
    {
      param = (FAV0 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
      list_set_field_content_by_icon(p_list, focus, 2, 0);
    }
    else
    {
      param = (FAV0 | DB_DVBS_PARAM_ACTIVE_FLAG); 
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
      list_set_field_content_by_icon(p_list, focus, 2, IM_ICON_FAVOURITE);//IM_FAV);
    } 
    list_draw_field_ext(p_list, focus, 2, TRUE);
  }
  else if(msg == MSG_LOCK_SET)
  {
    if( db_dvbs_get_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0)== TRUE)
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0);
      list_set_field_content_by_icon(p_list, focus, 3, RSC_INVALID_ID);
    }
    else
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, DB_DVBS_PARAM_ACTIVE_FLAG);
      list_set_field_content_by_icon(p_list, focus, 3, IM_ICON_LOCK); //IM_FAV);
    }
    list_draw_field_ext(p_list, focus, 3, TRUE);
  }
  else if(msg == MSG_HIDE_SET)
  {
    if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_MARK_SKP, 0) == TRUE)
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_SKP,0);
      list_set_field_content_by_icon(p_list, focus, 4, RSC_INVALID_ID);
    }
    else
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_SKP, DB_DVBS_PARAM_ACTIVE_FLAG);
      list_set_field_content_by_icon(p_list, focus, 4, IM_ICON_HIDE);
    }
    list_draw_field_ext(p_list, focus, 4, TRUE);
  }
  #endif
  return SUCCESS;
}

static void fav_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, im_value[4];
  u8 view_id = ui_dbase_get_pg_view_id();

  im_value[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_TV_FAV: 0;
  im_value[1] =
   db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0)? IM_TV_LOCK : 0;
  im_value[2] =
   db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_SKP, FALSE)? IM_TV_MOVE: 0;
  for (i = 0; i < 3; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), im_value[i]);
  }
}

static RET_CODE fav_edit_favlist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 pg_id, cnt = list_get_count(ctrl);
  dvbs_prog_node_t pg;
  u8 asc_str[8];
  u16 uni_str[32];

  u8 view_id;
  u16 i=0, j=0, total;
  u16 cur_favgroup_pg_count = 0;

  view_id = ui_dbase_get_pg_view_id();

  total = db_dvbs_get_count(view_id);
  
  for(j=0; j<total; j++)
  {
    if(cur_favgroup_pg_count==start)
    {
      break;
    }
    
    if(db_dvbs_get_mark_status(view_id, j, DB_DVBS_FAV_GRP, g_fav_index) == TRUE)
    {
      cur_favgroup_pg_count++;
    }
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      while(j<total)
      {       
        if(db_dvbs_get_mark_status(view_id, j, DB_DVBS_FAV_GRP, g_fav_index) == TRUE)
        {
          
          pg_id = db_dvbs_get_id_by_view_pos(view_id, (u16)j);
          db_dvbs_get_pg_by_id(pg_id, &pg);
          j++;
          break;
        }
        else
        {
          j++;
        }
      }

      /* NO. */
      sprintf((char*)asc_str, "%.4d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name
    }
  }

  return SUCCESS;
}

static RET_CODE fav_edit_list_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 pg_id, cnt = list_get_count(ctrl);
  dvbs_prog_node_t pg;
  u8 asc_str[8];
  u16 uni_str[32];

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(
        ui_dbase_get_pg_view_id(),
        (u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &pg);

      /* NO. */
      #ifdef LCN_SWITCH_DS_JHC
      sprintf((char*)asc_str, "%.4d ", pg.logical_num);
      #else
      if(have_logic_number())
        sprintf((char*)asc_str, "%.4d ", pg.logical_num);
      else
        sprintf((char *)asc_str, "%.4d ", (u16)(start + i + 1));
      #endif
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name

      /* MARKS */
      fav_set_list_item_mark(ctrl, (u16)(start + i), pg_id);
      if(pg_id == sys_status_get_curn_group_curn_prog_id())
      {
        list_select_item(ctrl, start + i);
        //ui_set_front_panel_by_num(start + i + 1);
      }      
    }
  }

  return SUCCESS;
}

static RET_CODE fav_edit_grouplist_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(p_list);
  u16 fav_name[MAX_FAV_NAME_LEN];
  
  for( i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sys_status_get_fav_name((u8)(start + i + sys_status_get_fav1_index()), fav_name);
      
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 0, fav_name);
    }
  }
  return SUCCESS;

}

static u16 fav_edit_fav_group_cal(control_t *p_cont)
{
  u8 view_id;
  u16 i=0, total;
  u16 cur_favgroup_pg_count = 0;

  view_id = ui_dbase_get_pg_view_id();

  total = db_dvbs_get_count(view_id);
  for(i=0; i<total; i++)
  {
    
    if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_FAV_GRP, g_fav_index) == TRUE)
    {
      cur_favgroup_pg_count++;
    }
  }

  return cur_favgroup_pg_count;
}

RET_CODE open_fav_edit(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl, *p_pg_list, *p_fav_list,  *p_sbar;
  control_t *p_favgroup_cont, *p_favgroup_list;
  u16 i,str_id;
  u8 vid;
  u16 pg_count = 0;
  u16 unistr[MAX_FAV_NAME_LEN];

  g_fav_index = sys_status_get_fav1_index();
  g_modify = FALSE;

  #if 0
  vid = db_dvbs_create_view(DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG, 0, NULL);
  ui_dbase_set_pg_view_id(vid);
  sys_status_set_curn_group(0);
  sys_status_set_curn_prog_mode(CURN_MODE_TV);
  #else
  ui_cache_view();

  if(sys_status_get_curn_prog_mode() != CURN_MODE_RADIO)
  {
    vid = db_dvbs_create_view(DB_DVBS_ALL_TV, 0, NULL);
    ui_dbase_set_pg_view_id(vid);
    sys_status_set_curn_group(0);
    sys_status_set_curn_prog_mode(CURN_MODE_TV);
    str_id = IDS_TV;
  }
  else
  {
    vid = db_dvbs_create_view(DB_DVBS_ALL_RADIO, 0, NULL);
    ui_dbase_set_pg_view_id(vid);
    sys_status_set_curn_group(0);
    sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
    str_id = IDS_RADIO;
  }
  #endif
  
  sys_status_save();
  pg_count = db_dvbs_get_count(vid);
  p_cont = ui_comm_root_create(ROOT_ID_FAV_EDIT,
                               0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,
                               RSC_INVALID_ID,
                               IDS_EDIT_FAV_CHANNEL);
  
  ctrl_set_keymap(p_cont, fav_edit_cont_keymap);
  ctrl_set_proc(p_cont, fav_edit_cont_proc);

  //TV preview window
  #if 0
  p_tv_win = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_FAV_EDIT_TV_WIN,
                              FAV_EDIT_PREV_X, FAV_EDIT_PREV_Y,
                              FAV_EDIT_PREV_W, FAV_EDIT_PREV_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_tv_win, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
#endif
  /////////////////////////////////////////////////////////////////////////////
  //favgroup container
  p_favgroup_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FAVGROUP_CONT,
                         FAV_EDIT_FAVGROUP_CONT_X, FAV_EDIT_FAVGROUP_CONT_Y,
                         FAV_EDIT_FAVGROUP_CONT_W, FAV_EDIT_FAVGROUP_CONT_H,
                         p_cont, 0);
  ctrl_set_rstyle(p_favgroup_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_sts(p_favgroup_cont, OBJ_STS_HIDE);

  //favgroup list title
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_FAV_EDIT_FAVGROUP_HEAD,
                         FAV_EDIT_FAVGROUP_LIST_TITLE_X, FAV_EDIT_FAVGROUP_LIST_TITLE_Y,
                         FAV_EDIT_FAVGROUP_LIST_TITLE_W, FAV_EDIT_FAVGROUP_LIST_TITLE_H,
                         p_favgroup_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_FAV_LIST);
  
  //favgroup list
  p_favgroup_list = ctrl_create_ctrl(CTRL_LIST, IDC_FAV_EDIT_FAVGROUP_LIST,
                           FAV_EDIT_FAVGROUP_LIST_X, FAV_EDIT_FAVGROUP_LIST_Y,
                           FAV_EDIT_FAVGROUP_LIST_W, FAV_EDIT_FAVGROUP_LIST_H,
                           p_favgroup_cont, 0);
  ctrl_set_rstyle(p_favgroup_list, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_favgroup_list, fav_edit_grouplist_keymap);
  ctrl_set_proc(p_favgroup_list, fav_edit_grouplist_proc);

  ctrl_set_mrect(p_favgroup_list, FAV_EDIT_FAVGROUP_LIST_MIDL, FAV_EDIT_FAVGROUP_LIST_MIDT,
           FAV_EDIT_FAVGROUP_LIST_MIDW, FAV_EDIT_FAVGROUP_LIST_MIDH);
  list_set_item_rstyle(p_favgroup_list, &favgroup_item_rstyle);
  list_set_count(p_favgroup_list, FAV_LIST_COUNT, FAV_EDIT_FAVGROUP_LIST_PAGE);

  list_set_field_count(p_favgroup_list, FAV_EDIT_FAVGROUP_LIST_FIELD, FAV_EDIT_FAVGROUP_LIST_PAGE);
  list_set_focus_pos(p_favgroup_list, g_fav_index - sys_status_get_fav1_index());
  list_set_update(p_favgroup_list, fav_edit_grouplist_update, 0);
  for (i = 0; i < FAV_EDIT_FAVGROUP_LIST_FIELD; i++)
  {
    list_set_field_attr(p_favgroup_list, (u8)i, (u32)(group_list_attr[i].attr), (u16)(group_list_attr[i].width),
                        (u16)(group_list_attr[i].left), (u8)(group_list_attr[i].top));
    list_set_field_rect_style(p_favgroup_list, (u8)i, group_list_attr[i].rstyle);
    list_set_field_font_style(p_favgroup_list, (u8)i, group_list_attr[i].fstyle);
  } 
  fav_edit_grouplist_update(p_favgroup_list, list_get_valid_pos(p_favgroup_list), FAV_EDIT_FAVGROUP_LIST_PAGE, 0);

  ///////////////////////////////////////////////////////////////////////////////////////

  //proglist title
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_FAV_EDIT_PG_HEAD,
                         FAV_EDIT_PROG_LIST_TITLE_X, FAV_EDIT_PROG_LIST_TITLE_Y,
                         FAV_EDIT_PROG_LIST_TITLE_W, FAV_EDIT_PROG_LIST_TITLE_H,
                         p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, str_id);
  
  //prog list
  p_pg_list = ctrl_create_ctrl(CTRL_LIST, IDC_FAV_EDIT_PG_LIST,
                           FAV_EDIT_PROG_LIST_X, FAV_EDIT_PROG_LIST_Y,
                           FAV_EDIT_PROG_LIST_W, FAV_EDIT_PROG_LIST_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_pg_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_pg_list, fav_edit_pglist_keymap);
  ctrl_set_proc(p_pg_list, fav_edit_pglist_proc);

  ctrl_set_mrect(p_pg_list, FAV_EDIT_PROG_LIST_MIDL, FAV_EDIT_PROG_LIST_MIDT,
                 FAV_EDIT_PROG_LIST_MIDL + FAV_EDIT_PROG_LIST_MIDW, FAV_EDIT_PROG_LIST_MIDT + FAV_EDIT_PROG_LIST_MIDH);

  list_set_item_rstyle(p_pg_list, &favedit_item_rstyle);
  list_set_count(p_pg_list, pg_count, FAV_EDIT_PROG_LIST_PAGE);

  list_set_field_count(p_pg_list, FAV_EDIT_LIST_FIELD, FAV_EDIT_PROG_LIST_PAGE);
  list_set_focus_pos(p_pg_list, 0);
  list_set_update(p_pg_list, fav_edit_list_update, 0);
  for (i = 0; i < FAV_EDIT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_pg_list, (u8)i, (u32)(favedit_all_pg_list_attr[i].attr), (u16)(favedit_all_pg_list_attr[i].width),
                        (u16)(favedit_all_pg_list_attr[i].left), (u8)(favedit_all_pg_list_attr[i].top));
    list_set_field_rect_style(p_pg_list, (u8)i, favedit_all_pg_list_attr[i].rstyle);
    list_set_field_font_style(p_pg_list, (u8)i, favedit_all_pg_list_attr[i].fstyle);
  } 
  fav_edit_list_update(p_pg_list, list_get_valid_pos(p_pg_list), FAV_EDIT_PROG_LIST_PAGE, 0);
  
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_FAV_EDIT_PG_BAR, FAV_EDIT_PROG_BAR_X,
                            FAV_EDIT_PROG_BAR_Y, FAV_EDIT_PROG_BAR_W, FAV_EDIT_PROG_BAR_H, p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_FAV_EDIT_SBAR, RSI_FAV_EDIT_SBAR, RSI_FAV_EDIT_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_FAV_EDIT_SBAR_MID, RSI_FAV_EDIT_SBAR_MID,
                     RSI_FAV_EDIT_SBAR_MID);
  list_set_scrollbar(p_pg_list, p_sbar);

  /////////////////////////////////////////////////////////////////////////////////////
  //fav pg list title
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_FAV_EDIT_FAV_HEAD,
                         FAV_EDIT_FAV_LIST_TITLE_X, FAV_EDIT_FAV_LIST_TITLE_Y,
                         FAV_EDIT_FAV_LIST_TITLE_W, FAV_EDIT_FAV_LIST_TITLE_H,
                         p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  sys_status_get_fav_name((u8)g_fav_index, unistr);
  text_set_content_by_unistr(p_ctrl, unistr);
  
  //fav pg list
  p_fav_list = ctrl_create_ctrl(CTRL_LIST, IDC_FAV_EDIT_FAV_LIST,
                           FAV_EDIT_FAV_LIST_X, FAV_EDIT_FAV_LIST_Y,
                           FAV_EDIT_FAV_LIST_W, FAV_EDIT_FAV_LIST_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_fav_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_fav_list, fav_edit_favlist_keymap);
  ctrl_set_proc(p_fav_list, fav_edit_favlist_proc);

  ctrl_set_mrect(p_fav_list, FAV_EDIT_LIST_MIDL, FAV_EDIT_LIST_MIDT,
                 FAV_EDIT_LIST_MIDL + FAV_EDIT_LIST_MIDW, FAV_EDIT_LIST_MIDT + FAV_EDIT_LIST_MIDH);

  list_set_item_rstyle(p_fav_list, &favedit_item_rstyle);
  list_set_count(p_fav_list, fav_edit_fav_group_cal(p_cont), FAV_EDIT_LIST_PAGE);

  list_set_field_count(p_fav_list, FAV_EDIT_LIST_FIELD, FAV_EDIT_LIST_PAGE);
  list_set_focus_pos(p_fav_list, 0);
  list_set_update(p_fav_list, fav_edit_favlist_update, 0);
  for (i = 0; i < FAV_EDIT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_fav_list, (u8)i, (u32)(favedit_fav_pg_list_attr[i].attr), (u16)(favedit_fav_pg_list_attr[i].width),
                        (u16)(favedit_fav_pg_list_attr[i].left), (u8)(favedit_fav_pg_list_attr[i].top));
    list_set_field_rect_style(p_fav_list, (u8)i, favedit_fav_pg_list_attr[i].rstyle);
    list_set_field_font_style(p_fav_list, (u8)i, favedit_fav_pg_list_attr[i].fstyle);
  } 
  fav_edit_favlist_update(p_fav_list, list_get_valid_pos(p_fav_list), FAV_EDIT_LIST_PAGE, 0);
  
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_FAV_EDIT_FAV_BAR, FAV_EDIT_FAV_BAR_X,
                            FAV_EDIT_FAV_BAR_Y, FAV_EDIT_FAV_BAR_W, FAV_EDIT_FAV_BAR_H, p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_FAV_EDIT_SBAR, RSI_FAV_EDIT_SBAR, RSI_FAV_EDIT_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_FAV_EDIT_SBAR_MID, RSI_FAV_EDIT_SBAR_MID,
                     RSI_FAV_EDIT_SBAR_MID);
  list_set_scrollbar(p_fav_list, p_sbar);

  ui_comm_help_create2(&fav_edit_help_data, p_cont, FALSE);
  ctrl_default_proc(p_pg_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  if(ui_is_mute())
  {
    open_mute(0, 0);
  }

  return SUCCESS;
}

static void fav_edit_do_save_all(void)
{
  is_give_up = FALSE;
  db_dvbs_save(ui_dbase_get_pg_view_id());
}

static void fav_edit_undo_save_all(void)
{
  u16 view_type;
  u32 group_context;
  
  is_give_up = TRUE;
  //undo modification about view
  db_dvbs_undo(ui_dbase_get_pg_view_id());
  
  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);     
  ui_dbase_set_pg_view_id(ui_dbase_create_view((dvbs_view_t)view_type, group_context, NULL));
}

static RET_CODE on_fav_edit_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{ 
  dlg_ret_t dlg_ret;
  
  if(g_modify)
  {
    dlg_ret = ui_comm_dlg_open(&channel_edit_exit_data);

    if(dlg_ret == DLG_RET_YES)
    {
      //count = channel_edit_pre_focus_pg();
      fav_edit_do_save_all();
      //channel_edit_suf_focus_pg(count);
    }
    else
    {
      fav_edit_undo_save_all();
    }
  }
  //ui_restore_view();
  return ERR_NOFEATURE;
}

static RET_CODE refresh_fav_list(control_t *p_cont, BOOL is_paint)
{
  control_t *p_fav_list;
  //u16 favgroup = 0;

  p_fav_list = ctrl_get_child_by_id(p_cont, IDC_FAV_EDIT_FAV_LIST);

  list_set_count(p_fav_list, fav_edit_fav_group_cal(p_cont), FAV_EDIT_LIST_PAGE);
  fav_edit_favlist_update(p_fav_list, list_get_valid_pos(p_fav_list), FAV_EDIT_LIST_PAGE, 0);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_fav_list, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_fav_edit_add_fav(control_t *p_pg_list, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_cont;
  u8 view_id;
  u16 focus, param;
  //u16 favgroup = 0;
  u16 pg_id;
  p_cont = p_pg_list->p_parent;

  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_pg_list);
  //favgroup = list_get_focus_pos(p_favgroup_list)+sys_status_get_fav1_index();
  if(db_dvbs_get_count(view_id)<=0)
  {
    return ERR_FAILURE;
  }
  if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, g_fav_index) == TRUE)
  {
    //param = (favgroup & (~DB_DVBS_PARAM_ACTIVE_FLAG));
    //db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
  }
  else
  {
    param = (g_fav_index | DB_DVBS_PARAM_ACTIVE_FLAG); 
    db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);

    g_modify = TRUE;
    pg_id = db_dvbs_get_id_by_view_pos(
        ui_dbase_get_pg_view_id(),
        focus);
    fav_set_list_item_mark(p_pg_list, focus, pg_id);   
    refresh_fav_list(p_cont, TRUE);
    ctrl_paint_ctrl(p_cont, TRUE);
  } 

  return SUCCESS;
}

static RET_CODE on_fav_edit_group_list_select(control_t *p_fav_group_list, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_cont, *p_favgroup_cont, *p_favgroup_list, *p_fav_list_title;
  u16 favgroup = 0;
  u16 unistr[MAX_FAV_NAME_LEN];

  p_cont = p_fav_group_list->p_parent->p_parent;
  p_favgroup_cont = ctrl_get_child_by_id(p_cont, IDC_FAVGROUP_CONT);
  p_favgroup_list = ctrl_get_child_by_id(p_favgroup_cont, IDC_FAV_EDIT_FAVGROUP_LIST);
  p_fav_list_title = ctrl_get_child_by_id(p_cont, IDC_FAV_EDIT_FAV_HEAD);

  favgroup = list_get_focus_pos(p_favgroup_list);
  
  g_fav_index = favgroup+sys_status_get_fav1_index();
  sys_status_get_fav_name((u8)g_fav_index, unistr);
  text_set_content_by_unistr(p_fav_list_title, unistr);
  refresh_fav_list(p_cont, FALSE);
  on_fav_set_attr(p_cont, msg, para1, para2);

  return SUCCESS;
}

static RET_CODE on_fav_edit_change_list(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list_next = NULL;
  control_t *p_cont;

  p_cont = p_list->p_parent;
  switch(p_list->id)
  {
    case IDC_FAV_EDIT_PG_LIST:
      p_list_next = ctrl_get_child_by_id(p_cont, IDC_FAV_EDIT_FAV_LIST);
      if(list_get_count(p_list_next) > 0)
      {
        list_set_focus_pos(p_list_next, 0);
      }
      else
      {
        return SUCCESS;
      }
      break;

    case IDC_FAV_EDIT_FAV_LIST:
      p_list_next = ctrl_get_child_by_id(p_cont, IDC_FAV_EDIT_PG_LIST);
      break;
    default:
      break;
  }

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);

  ctrl_process_msg(p_list_next, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_paint_ctrl(p_list_next, TRUE);

  return SUCCESS;
}

static RET_CODE on_fav_edit_del_fav(control_t *p_fav_list, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_cont, *p_pg_list;
  u8 view_id;
  u16 focus, param;
  //u16 favgroup = 0;
  u16 i = 0, pg_count = 0, temp = 0;

  p_cont = p_fav_list->p_parent;
  p_pg_list = ctrl_get_child_by_id(p_cont, IDC_FAV_EDIT_PG_LIST);

  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_fav_list);
  //favgroup = list_get_focus_pos(p_favgroup_list)+sys_status_get_fav1_index();
  if(list_get_count(p_fav_list) == 1)
  {
    ctrl_process_msg(p_fav_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_pg_list, MSG_GETFOCUS, para1, para2);
  }
  else if(list_get_count(p_fav_list) == focus + 1)
  {
    ctrl_process_msg(p_fav_list, MSG_FOCUS_UP, para1, para2);
  }
  pg_count = db_dvbs_get_count(view_id);
  for(i=0; i<pg_count; i++)
  {
    if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_FAV_GRP, g_fav_index) == TRUE)
    {
      temp++;
      if(temp == (focus+1))
      {
        break;
      }
    }
  }

  if(i != pg_count)
  {
    param = (g_fav_index & (~DB_DVBS_PARAM_ACTIVE_FLAG));
    db_dvbs_change_mark_status(view_id, i, DB_DVBS_FAV_GRP, param);
    fav_edit_list_update(p_pg_list, list_get_valid_pos(p_pg_list), FAV_EDIT_PROG_LIST_PAGE, 0);
    g_modify = TRUE;
    refresh_fav_list(p_cont, TRUE);
    ctrl_paint_ctrl(p_cont, TRUE);
  }  

  return SUCCESS;
}

static RET_CODE on_fav_change_volume(control_t *p_ctrl, u16 msg, 
                                     u32 para1, u32 para2)
{
  open_volume_usb(ROOT_ID_USB_MUSIC, para1);
  return SUCCESS;
}

static RET_CODE on_fav_edit_change_group(control_t *p_ctrl, u16 msg, 
                                     u32 para1, u32 para2)
{
  control_t *p_cont, *p_head;
  u16 pgid, focus, total;
  u8 vid;
  BOOL is_tv = FALSE;

  dlg_ret_t dlg_ret;
  p_cont = p_ctrl->p_parent;
  p_head = ctrl_get_child_by_id(p_cont, IDC_FAV_EDIT_PG_HEAD);
  is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);

  if(g_modify)
  {
    dlg_ret = ui_comm_dlg_open(&channel_edit_exit_data);

    if(dlg_ret == DLG_RET_YES)
    {
      //count = channel_edit_pre_focus_pg();
      fav_edit_do_save_all();
      //channel_edit_suf_focus_pg(count);
    }
    else
    {
      fav_edit_undo_save_all();
    }
    
    g_modify = FALSE;
      
    if(is_give_up)
    {
      is_give_up = FALSE;
      //ui_restore_view();
    }
  }
  if(!ui_tvradio_switch(FALSE, &pgid))
  {
    is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
    ui_comm_cfmdlg_open(NULL,
                        (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG), NULL, 0);

    return ERR_FAILURE;
  }
  else
  {
    is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
    vid = ui_dbase_create_view(is_tv? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, 0, NULL);
    switch (is_tv)
      {
        case FALSE:
          text_set_content_by_strid(p_head, IDS_RADIO);
          break;
        case TRUE:
          text_set_content_by_strid(p_head, IDS_TV);
          break;
        default:
          break;
      }

    //reset list.
    //vid = ui_dbase_get_pg_view_id();

    focus = db_dvbs_get_view_pos_by_id(vid, pgid);
    total = db_dvbs_get_count(vid);

    if((INVALIDPOS == focus)
      && (total > 0))
    {
      focus = 0;
    }
    else if(0 == total)
    {
      ui_comm_cfmdlg_open(NULL,
                          (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG), NULL, 0);
      return ERR_FAILURE;
    }
    list_set_count(p_ctrl, total, FAV_EDIT_PROG_LIST_PAGE);
    list_set_focus_pos(p_ctrl, focus);
    fav_edit_list_update(p_ctrl, list_get_valid_pos(p_ctrl), FAV_EDIT_PROG_LIST_PAGE, 0);
    refresh_fav_list(p_cont, TRUE);
    ctrl_paint_ctrl(p_cont, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_fav_edit_pwdlg_correct(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();

  return open_fav_edit(0, 0);
}

static RET_CODE on_fav_edit_pwdlg_exit(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  
  return SUCCESS;
}

RET_CODE preopen_fav_edit(u32 para1, u32 para2)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y,
    IDS_MSG_INPUT_PASSWORD,
    fav_edit_pwdlg_keymap,
    fav_edit_pwdlg_proc,
    PWDLG_T_COMMON
  };
  BOOL is_lock;

  if (para1 == ROOT_ID_MAINMENU)
  {
    pwdlg_data.left = PWD_DLG_FOR_CHK_X;
    pwdlg_data.top = PWD_DLG_FOR_CHK_Y;
  }
  
  sys_status_get_status(BS_MENU_LOCK, &is_lock);
  if(is_lock)
    ui_comm_pwdlg_open(&pwdlg_data);
  else
    open_fav_edit(0, 0);

  return SUCCESS;
}

BEGIN_KEYMAP(fav_edit_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  #ifdef CUSTOMER_YINHE_TR
    ON_EVENT(V_KEY_FAV,   MSG_FAV_GROUP)
  #endif
  ON_EVENT(V_KEY_GREEN,   MSG_FAV_GROUP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DEC)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_INC)
END_KEYMAP(fav_edit_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(fav_edit_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_VOLUME_DEC, on_fav_change_volume)
  ON_COMMAND(MSG_VOLUME_INC, on_fav_change_volume)
  ON_COMMAND(MSG_FAV_GROUP, on_fav_set_attr)
  ON_COMMAND(MSG_EXIT, on_fav_edit_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_fav_edit_exit)
END_MSGPROC(fav_edit_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(fav_edit_pglist_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_EXCH)    
  ON_EVENT(V_KEY_RIGHT, MSG_EXCH)
  
  #ifdef CUSTOMER_YINHE_TR
  ON_EVENT(V_KEY_TVRADIO, MSG_EXCH)
  ON_EVENT(V_KEY_F4, MSG_AUTO_SWITCH)
  #endif
  ON_EVENT(V_KEY_RED, MSG_EXCH)
  ON_EVENT(V_KEY_YELLOW, MSG_ADD)
  ON_EVENT(V_KEY_OK, MSG_ADD)
  ON_EVENT(V_KEY_TVRADIO, MSG_AUTO_SWITCH)
END_KEYMAP(fav_edit_pglist_keymap, NULL)

BEGIN_MSGPROC(fav_edit_pglist_proc, list_class_proc)
  ON_COMMAND(MSG_EXCH, on_fav_edit_change_list)
  ON_COMMAND(MSG_ADD, on_fav_edit_add_fav)
  ON_COMMAND(MSG_AUTO_SWITCH, on_fav_edit_change_group)
END_MSGPROC(fav_edit_pglist_proc, list_class_proc)

BEGIN_KEYMAP(fav_edit_favlist_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_EXCH)    
  ON_EVENT(V_KEY_RIGHT, MSG_EXCH)
  #ifdef CUSTOMER_YINHE_TR
  ON_EVENT(V_KEY_TVRADIO, MSG_EXCH)
  #endif
  ON_EVENT(V_KEY_RED, MSG_EXCH)
  ON_EVENT(V_KEY_YELLOW, MSG_DEL_FAV)
  ON_EVENT(V_KEY_OK, MSG_DEL_FAV)
END_KEYMAP(fav_edit_favlist_keymap, NULL)

BEGIN_MSGPROC(fav_edit_favlist_proc, list_class_proc)
  ON_COMMAND(MSG_EXCH, on_fav_edit_change_list)
  ON_COMMAND(MSG_DEL_FAV, on_fav_edit_del_fav)
END_MSGPROC(fav_edit_favlist_proc, list_class_proc)

BEGIN_KEYMAP(fav_edit_grouplist_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_FAVUP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_FAVDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(fav_edit_grouplist_keymap, NULL)

BEGIN_MSGPROC(fav_edit_grouplist_proc, list_class_proc)
  //ON_COMMAND(MSG_FOCUS_LEFT, on_fav_edit_change_list)
  //ON_COMMAND(MSG_FOCUS_RIGHT, on_fav_edit_change_list)
  ON_COMMAND(MSG_SELECT, on_fav_edit_group_list_select)
  ON_COMMAND(MSG_ADD, on_fav_edit_add_fav)
END_MSGPROC(fav_edit_grouplist_proc, list_class_proc)

BEGIN_KEYMAP(fav_edit_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(fav_edit_pwdlg_keymap, NULL)

BEGIN_MSGPROC(fav_edit_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_fav_edit_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_fav_edit_pwdlg_exit)
END_MSGPROC(fav_edit_pwdlg_proc, cont_class_proc)

