
/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_channel_edit.h"
#include "ui_fav_set.h"
#include "ui_sort_list.h"
#include "ui_rename_v2.h"
#include "ui_signal.h"
#include "customer_config.h"
#include "ui_mute.h"
#include "ui_mainmenu.h"

static list_xstyle_t channel_edit_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t channel_edit_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t channel_edit_field_rstyle_left =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_xstyle_t channel_edit_field_rstyle_mid =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
};

static list_xstyle_t sortgroup_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t sort_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t sort_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t group_list_attr[CHANNEL_EDIT_SORT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
  CHANNEL_EDIT_SORT_LIST_MIDW - CHANNEL_EDIT_SORT_LIST_MIDL, 0, 0, &sort_field_rstyle,  &sort_field_fstyle },
};

static list_field_attr_t channel_edit_attr[CHANNEL_EDIT_LIST_FIELD] =
{
  {LISTFIELD_TYPE_UNISTR,
   60, 10, 0, &channel_edit_field_rstyle_left, &channel_edit_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
   440, 90, 0, &channel_edit_field_rstyle_mid, &channel_edit_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   50, 530, 0, &channel_edit_field_rstyle_mid, &channel_edit_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   50, 580, 0, &channel_edit_field_rstyle_mid, &channel_edit_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   50, 630, 0, &channel_edit_field_rstyle_mid, &channel_edit_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   50, 680, 0, &channel_edit_field_rstyle_mid, &channel_edit_field_fstyle},
 };

#ifndef NIT_SETTING
#ifdef CUSTOMER_YINHE_TR
static comm_help_data_t2 channel_edit_help_data = //help bar data
{
  10, 100, {40, 100, 40, 100,40, 100, 40, 100, 40,200,},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_FAV,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_LOCK,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_MOVE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
    HELP_RSC_STRID | IDS_SORT,
	HELP_RSC_BMP   | IM_F4,
    HELP_RSC_STRID | IDS_DELETE,
    HELP_RSC_BMP   | IM_TVRADIO,
    HELP_RSC_STRID | IDS_TV_RADIO,
   },
};
#else
#if defined(CUSTOMER_JIESAI_WUNING) || defined(CUSTOMER_JIZHONG_ANXIN)
static comm_help_data_t2 channel_edit_help_data = //help bar data
{
  8, 100, {40, 100, 40, 100,40, 100, 40, 100, 40,200,},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_FAV,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_LOCK,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_MOVE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
    HELP_RSC_STRID | IDS_SORT,
	HELP_RSC_BMP   | IM_MP3_ICON_STOP,
    HELP_RSC_STRID | IDS_DELETE,
    HELP_RSC_BMP   | IM_TVRADIO,
    HELP_RSC_STRID | IDS_TV_RADIO,
   },
};
#else
#ifdef CUSTOMER_YINHE_LINGGANG
static comm_help_data_t2 channel_edit_help_data = //help bar data
{
  8, 100, {40, 100, 40, 100,40, 100, 40, 100, 40,200,},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_FAV,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_LOCK,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_MOVE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
    HELP_RSC_STRID | IDS_DELETE,
	HELP_RSC_BMP   | IM_MP3_ICON_STOP,
    HELP_RSC_STRID | IDS_SORT,
    HELP_RSC_BMP   | IM_TVRADIO,
    HELP_RSC_STRID | IDS_TV_RADIO,
   },
};
#else
static comm_help_data_t2 channel_edit_help_data = //help bar data
{
  10, 100, {40, 100, 40, 100,40, 100, 40, 100, 40,200,},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_FAV,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_LOCK,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_MOVE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
    HELP_RSC_STRID | IDS_SORT,
	HELP_RSC_STRID | IDS_FN,
    HELP_RSC_STRID | IDS_DELETE,
    HELP_RSC_BMP   | IM_TVRADIO,
    HELP_RSC_STRID | IDS_TV_RADIO,
   },
};
#endif
#endif
#endif
static comm_help_data_t2 channel_edit_help_data2 = //help bar data
{
  15, 0, {40, 55, 40, 55,40, 55,40,55,40,55,230,40,55,40,55},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_DELETE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_LOCK,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_MOVE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
	HELP_RSC_STRID | IDS_SORT,
    HELP_RSC_BMP   | IM_FAV,
    HELP_RSC_STRID | IDS_FAV,
    HELP_RSC_STRID | IDS_TV_RADIO_AISAT,
    HELP_RSC_BMP   | IM_MENU,
    HELP_RSC_STRID | IDS_BACK,
    HELP_RSC_BMP   | IM_EXIT,
    HELP_RSC_STRID | IDS_EXIT,
   },
};
#else
static comm_help_data_t2 channel_edit_help_data_aisat_demo = //help bar data
{
  10, 100, {40, 135, 40, 135,40, 135, 40, 235,},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_FAV,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_LOCK,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_MOVE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
    HELP_RSC_STRID | IDS_SORT,
	HELP_RSC_BMP   | IM_MP3_ICON_STOP,
    HELP_RSC_STRID | IDS_DELETE,
   },
};
#endif
#define IDC_CHANNEL_EDIT_CONT_ID        1

enum local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 650,
  MSG_GREEN,
  MSG_YELLOW,
  MSG_BLUE,
  MSG_SORT_GROUP,
  MSG_EXIT_SORTGROUP,
  MSG_DEL,
};

enum channel_edit_ctrl_id
{
  IDC_INVALID = 0,
  IDC_CHANNEL_EDIT_LIST_CONT,
  IDC_CHANNEL_EDIT_BAR,
  IDC_CHANNEL_EDIT_SORT_CONT,
};

enum channel_edit_sort_cont_ctrl_id
{
  IDC_CHANNEL_EDIT_SORT_HEAD = 1,
  IDC_CHANNEL_EDIT_SORT_LIST,
};

enum channel_edit_cont_ctrl_id
{
  IDC_PLCONT_HEAD = 1,
  IDC_PLCONT_NUM,
  IDC_PLCONT_LOVE,
  IDC_PLCONT_ARROW,
  IDC_PLCONT_LOCK,
  IDC_PLCONT_DEL,
  IDC_PLCONT_NAME,
  IDC_PLCONT_LIST,
};

static BOOL is_move = FALSE;
static BOOL is_modified = FALSE;
static BOOL is_tv = FALSE;

comm_dlg_data_t channel_edit_exit_data = //popup dialog data
{
  ROOT_ID_CHANNEL_EDIT,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_MSG_ASK_FOR_SAV,
  0,
};

static RET_CODE channel_edit_update(control_t *ctrl, u16 start, u16 size, u32 context);
static void channel_edit_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id);
BOOL channel_edit_get_modify_state(void);

u16 channel_edit_cont_keymap(u16 key);
RET_CODE channel_edit_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 channel_edit_list_keymap(u16 key);
RET_CODE channel_edit_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 channel_edit_sort_keymap(u16 key);
RET_CODE channel_edit_sort_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static u16 channel_edit_pwdlg_keymap(u16 key);
RET_CODE channel_edit_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 channel_edit_list_hotel_keymap(u16 key);
extern BOOL have_logic_number(void);

BOOL get_pg_type(void)
{
  return is_tv;
}
void channel_edit_set_move_state(BOOL state)
{
  is_move = state;
}


BOOL channel_edit_get_move_state(void)
{
  return is_move;
}

void channel_edit_set_modify_state(BOOL state)
{
  is_modified = state;
}

BOOL channel_edit_get_modify_state(void)
{
  return is_modified;
}

static void channel_edit_do_save_all(void)
{
  u16 view_type;
  u32 group_context;
  u16 i, count;
  u8 view_id;
  u32 prog_id;
  comm_dlg_data_t dlg_data =
  {
    ROOT_ID_CHANNEL_EDIT,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_MSG_SAVING,
    0,
  };

  ui_comm_dlg_open(&dlg_data);
#if 1
  view_id = ui_dbase_get_pg_view_id();
  count = db_dvbs_get_count(view_id);
  //to check if current view is favorit view
  sys_status_get_curn_view_info(&view_type, &group_context);

  if((view_type == DB_DVBS_FAV_TV)
    || (view_type == DB_DVBS_FAV_RADIO))
  {
    for(i = 0; i < count; i++)
    {
      if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0) == TRUE)
      {
        db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0);
        db_dvbs_change_mark_status(view_id, i, DB_DVBS_FAV_GRP, (u16)group_context);
      }
    }
  }
#endif
  for(i = 0; i < count; i++)
  {
    if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0) == TRUE)
    {
      db_dvbs_change_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0);
    }
  }
  //save your modifications.
  db_dvbs_save_use_hide(ui_dbase_get_pg_view_id());

  sys_status_check_group();

  sys_status_save();

  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);
  view_id = ui_dbase_get_pg_view_id();
  count = db_dvbs_get_count(view_id);
  OS_PRINTF("view_id = %d, count = %d, view_type = %d \n", view_id,count, view_type);
  if(count == 0 )
	{
		if(view_type == DB_DVBS_ALL_RADIO)
		{
			view_id = ui_dbase_create_view(DB_DVBS_ALL_RADIO, 0, NULL);
			sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
			OS_PRINTF(" db_dvbs_get_count(view_id) = %d \n ", db_dvbs_get_count(view_id));
			if( db_dvbs_get_count(view_id)> 0)
			{
				prog_id = db_dvbs_get_id_by_view_pos(view_id, 0);
				sys_status_set_curn_group_info(prog_id, 0);
				sys_status_get_curn_group_curn_prog_id();
			
			}
			
		}
		else if(view_type == DB_DVBS_ALL_TV)
		{
			view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
			sys_status_set_curn_prog_mode(CURN_MODE_TV);
			if( db_dvbs_get_count(view_id)> 0)
			{
				prog_id = db_dvbs_get_id_by_view_pos(view_id, 0);
				sys_status_set_curn_group_info(prog_id, 0);
			}
		}
		else if(view_type == DB_DVBS_INVALID_VIEW)
		{
			sys_status_set_curn_prog_mode(CURN_MODE_NONE);
		}
		ui_cache_view();
		
	}
  //if curn view is invalid, that means no pg saved now, so we should
  //do nothing about it.
  if(view_type != DB_DVBS_INVALID_VIEW)
  {
	ui_dbase_set_pg_view_id(
	ui_dbase_create_view((dvbs_view_t)view_type, group_context, NULL));

 }

#if 0
 u8 curn_mode = sys_status_get_curn_prog_mode();
  u16 curn_group = sys_status_get_curn_group();

  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);

  sys_status_set_curn_group_info(prog_id);
  sys_status_save();
#endif
  book_check_node_on_delete();

  ui_comm_dlg_close();
}


static void channel_edit_undo_save_all(void)
{
  u16 view_type;
  u32 group_context;

  //undo modification about view
  db_dvbs_undo(ui_dbase_get_pg_view_id());

  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);
  ui_dbase_set_pg_view_id(
    ui_dbase_create_view((dvbs_view_t)view_type, group_context, NULL));
}

static BOOL channel_edit_save_data(void)
{
  dlg_ret_t dlg_ret;
  u8 org_mode, curn_mode;

  //u16 count = 0;
  org_mode = sys_status_get_curn_prog_mode();

  if(channel_edit_get_modify_state())
  {
    dlg_ret = ui_comm_dlg_open(&channel_edit_exit_data);

    if(dlg_ret == DLG_RET_YES)
    {
      //count = channel_edit_pre_focus_pg();
      channel_edit_do_save_all();
      //channel_edit_suf_focus_pg(count);
    }
    else
    {
      channel_edit_undo_save_all();
    }
    
    ui_reset_chkpwd();

    channel_edit_set_modify_state(FALSE);
    channel_edit_set_move_state(FALSE);
  }

  curn_mode = sys_status_get_curn_prog_mode();

  return (org_mode == curn_mode) ? TRUE : FALSE;
}

static RET_CODE on_channel_edit_set_attr(control_t *p_list, u16 msg, u32 para1, u32 para2)
{ 
  u8 view_id;
  u16 focus;
  u16 param;
  u16 FAV1 = sys_status_get_fav1_index();
  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_list);

  channel_edit_set_modify_state(TRUE);
  
  if(msg == MSG_RED)
  {
    if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, FAV1) == TRUE)
    {
      param = (FAV1 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
      list_set_field_content_by_icon(p_list, focus, 2, 0);
    }
    else
    {
      param = (FAV1 | DB_DVBS_PARAM_ACTIVE_FLAG); 
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
      list_set_field_content_by_icon(p_list, focus, 2, IM_TV_FAV);//IM_FAV);
    } 
    list_draw_field_ext(p_list, focus, 2, TRUE);
  }
  else if(msg == MSG_GREEN)
  {
    if( db_dvbs_get_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0)== TRUE)
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0);
      list_set_field_content_by_icon(p_list, focus, 4, RSC_INVALID_ID);
    }
    else
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, DB_DVBS_PARAM_ACTIVE_FLAG);
      list_set_field_content_by_icon(p_list, focus, 4, IM_TV_LOCK); //IM_FAV);
    }
    list_draw_field_ext(p_list, focus, 4, TRUE);
  }
  else if(msg == MSG_YELLOW)
  {
    if(channel_edit_get_move_state() == TRUE)
    {
      channel_edit_set_move_state(FALSE);
    }
    else
    {
      channel_edit_set_move_state(TRUE);
    }
    
    channel_edit_update(p_list, list_get_valid_pos(p_list), CHANNEL_EDIT_LIST_PAGE, 0);
    list_draw_field_ext(p_list, focus, 3, TRUE);
  }
  else if(msg == MSG_DEL)
  {
    if( db_dvbs_get_mark_status(view_id, focus, DB_DVBS_DEL_FLAG, 0)== TRUE)
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_DEL_FLAG, 0);
      list_set_field_content_by_icon(p_list, focus, 5, RSC_INVALID_ID);
    }
    else
    {
      db_dvbs_change_mark_status(view_id, focus, DB_DVBS_DEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
      list_set_field_content_by_icon(p_list, focus, 5, IM_TV_DEL); //IM_FAV);
    }
    list_draw_field_ext(p_list, focus, 5, TRUE);
  }
  return SUCCESS;
}

static void channel_edit_move_list_item(control_t *p_list, u16 msg)
{
  u16 focus, new_focus;
  s16 offset;
  BOOL is_reverse;

  switch (msg)
  {
    case MSG_FOCUS_UP:
      offset = -1;
      break;
    case MSG_FOCUS_DOWN:
      offset = 1;
      break;
    case MSG_PAGE_UP:
      offset = (0 - CHANNEL_EDIT_LIST_PAGE);
      break;
    case MSG_PAGE_DOWN:
      offset = CHANNEL_EDIT_LIST_PAGE;
      break;
    default:
      MT_ASSERT(0);
      offset = 0;
  }

  focus = list_get_focus_pos(p_list);
  new_focus = (u16)list_get_new_focus(p_list, offset, &is_reverse);
  db_dvbs_move_item_in_view(ui_dbase_get_pg_view_id(),
                            focus, new_focus);

  list_class_proc(p_list, msg, 0, 0);

  channel_edit_update(p_list, list_get_valid_pos(p_list), CHANNEL_EDIT_LIST_PAGE, 0);

  //list_draw_item_ext(p_list, focus, TRUE);
  //list_draw_item_ext(p_list, new_focus, TRUE);
  ctrl_paint_ctrl(p_list, TRUE);
}

static RET_CODE on_channel_edit_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  channel_edit_save_data();
  ui_restore_view();
  return ERR_NOFEATURE;
}


static RET_CODE on_channel_edit_exit_all(control_t *p_cont, u16 msg,
                                  u32 para1, u32 para2)
{
  channel_edit_save_data();
  ui_restore_view();
  return ERR_NOFEATURE;
}

static RET_CODE on_channel_edit_tvradio(control_t *p_list, u16 msg,
                                 u32 para1, u32 para2)
{
  control_t *p_lcont;
  u16 pgid, focus, total;
  u8 vid;

  //control_t *p_icon;

  p_lcont = p_list->p_parent;

  //try to save modifications first
  if(!channel_edit_save_data())
  {
    switch(sys_status_get_curn_prog_mode())
    {
      case CURN_MODE_NONE: //no program
        //ui_stop_play(STOP_PLAY_BLACK, TRUE);
        ui_close_all_mennus();
        return ERR_FAILURE;
      case CURN_MODE_TV: //no tv
        sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
        break;
      case CURN_MODE_RADIO: //no radio
        sys_status_set_curn_prog_mode(CURN_MODE_TV);
        break;
      default:
        MT_ASSERT(0);
    }
  }

  if(!ui_tvradio_switch(FALSE, &pgid))
  {
    ui_comm_cfmdlg_open(NULL,
                        (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG), NULL, 0);

    return ERR_FAILURE;
  }
  else
  {
    is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
    vid = ui_dbase_create_view(is_tv? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, 0, NULL);
    
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

    //reset list
    list_set_count(p_list, total, CHANNEL_EDIT_LIST_PAGE);
    list_set_focus_pos(p_list, focus);
    channel_edit_update(p_list, list_get_valid_pos(p_list), CHANNEL_EDIT_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list, TRUE);

    //reset prog list icon
    //p_icon = ui_comm_root_get_ctrl(ROOT_ID_PROG_LIST, IDC_COMM_TITLE_ICON);
    //bmap_set_content_by_id(p_icon,
    // is_tv ? IM_TITLEICON_TV: IM_TITLEICON_RADIO);
    //ctrl_paint_ctrl(p_icon, TRUE);

    //reset prog list title
    ui_comm_title_set_content(p_lcont->p_parent->p_parent, is_tv ? IDS_TV_CHANNEL_LIST : IDS_RADIO_CHANNEL_LIST);
    ui_comm_title_update(p_lcont->p_parent->p_parent);
    ctrl_paint_ctrl(p_lcont->p_parent->p_parent, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_channel_edit_list_change_channel(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = ERR_NOFEATURE;
  if(channel_edit_get_move_state())
  {
    channel_edit_move_list_item(p_list, msg);
    ret = SUCCESS;
  }

  return ret;

}

static RET_CODE on_channel_edit_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(channel_edit_get_move_state())
  {
    on_channel_edit_set_attr(p_list, MSG_YELLOW, para1, para2);
  }
  else if(channel_edit_get_modify_state())
  {
    fw_notify_root(p_list, NOTIFY_T_KEY, FALSE, V_KEY_MENU, 0, 0);
  }
  
  return SUCCESS;
}

static RET_CODE channel_edit_update(control_t *ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 pg_id, cnt = list_get_count(ctrl);
  dvbs_prog_node_t pg;
  u8 asc_str[8];
  u16 uni_str[32];

  for(i = 0; i < size; i++)
  {
    if(i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(
        ui_dbase_get_pg_view_id(),
        (u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &pg);

      /* NO. */
      #ifdef LCN_SWITCH_DS_JHC
      sprintf((char *)asc_str, "%.4d ", pg.logical_num);
      #else
      if(have_logic_number())
      {
        sprintf((char *)asc_str, "%.4d ", pg.logical_num);
      }
      else
      {
        sprintf((char *)asc_str, "%.4d ", start + i + 1);
      }
      #endif
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str);

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name

      /* MARKS */
      channel_edit_set_list_item_mark(ctrl, (u16)(start + i), pg_id);
    }
  }
  return SUCCESS;
}


static void channel_edit_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, im_value[4] = {0, 0, 0,0};
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 FAV1 = sys_status_get_fav1_index();
  
  im_value[0] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, FAV1) ? IM_TV_FAV : 0;
  //im_value[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_TV_FAV : 0;
  if (channel_edit_get_move_state() && (list_get_focus_pos(p_list) == pos))
  {
      im_value[1] = IM_TV_MOVE;
  }
  im_value[2] =
    db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, FALSE) ? IM_TV_LOCK: 0;
  im_value[3] =
    db_dvbs_get_mark_status(view_id, pos, DB_DVBS_DEL_FLAG, FALSE) ? IM_TV_DEL: 0;
  for(i = 0; i < 4; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), im_value[i]);
  }
}


RET_CODE open_channel_edit(u32 para1, u32 para2)
{
  control_t *p_menu, *p_cont;
  control_t *p_list_cont, *p_list;
  //control_t *p_top_line, *p_bottom_line;
  control_t *p_sbar;
  //control_t *p_title_icon;
  control_t *p_ctrl;
  control_t *p_sort_cont, *p_sort_list;
  u8 list_mode, curn_mode;
  u8 view_id, i;
  u8 pg_type = 3;
  u16 curn_group;
  u32 context;
  u16 view_type, view_count;
  u16 pg_id, pg_pos;

  u16 im_value[4] = {IM_TV_FAV, IM_TV_MOVE, IM_TV_LOCK, IM_TV_DEL};
   //global variable initialization.
  is_modified = FALSE;
  is_move = FALSE;
  ui_cache_view();
  curn_mode = sys_status_get_curn_prog_mode();

  is_tv = (BOOL)((curn_mode != CURN_MODE_RADIO) ? TRUE : FALSE);

  list_mode = is_tv ? CURN_MODE_TV : CURN_MODE_RADIO;
  sys_status_set_group_curn_type(pg_type);
  
  if(list_mode != CURN_MODE_RADIO)
  {
    view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
    ui_dbase_set_pg_view_id(view_id);
    sys_status_set_curn_group(0);
    sys_status_set_curn_prog_mode(CURN_MODE_TV);
  }
  else
  {
    view_id = ui_dbase_create_view(DB_DVBS_ALL_RADIO, 0, NULL);
    ui_dbase_set_pg_view_id(view_id);
    sys_status_set_curn_group(0);
    sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
  }
  
  curn_group = sys_status_get_curn_group();

  sys_status_get_view_info(curn_group, list_mode, &view_type,
                           &context);

  sys_status_set_curn_prog_mode(list_mode);
  sys_status_save();
  view_count = db_dvbs_get_count(view_id);
  //MT_ASSERT(view_count != 0);
  
  sys_status_get_all_group_num();

  sys_status_get_curn_prog_in_group(curn_group, list_mode, &pg_id, &context);
  ui_dbase_set_pg_view_id(view_id);

  pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);

  /*Create Menu*/
  p_cont = ui_comm_root_create(ROOT_ID_CHANNEL_EDIT,
                               0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
                               COMM_BG_H, IM_TITLEICON_TV, IDS_TV_CHANNEL_LIST);

  //title icon
  //p_title_icon = ctrl_get_child_by_id(p_cont, IDC_COMM_TITLE_ICON);
  //bmap_set_content_by_id(p_title_icon,
  //is_tv ? IM_TITLEICON_RADIO : IM_TITLEICON_TV);

  //title text
  ui_comm_title_set_content(p_cont, is_tv ? IDS_TV_CHANNEL_LIST : IDS_RADIO_CHANNEL_LIST);

  p_menu = ctrl_create_ctrl(CTRL_CONT, IDC_CHANNEL_EDIT_CONT_ID, CHANNEL_EDIT_MENU_X,
                            CHANNEL_EDIT_MENU_Y, CHANNEL_EDIT_MENU_W, CHANNEL_EDIT_MENU_H, p_cont, 0);

  ctrl_set_rstyle(p_menu, RSI_CHANNEL_EDIT_MENU, RSI_CHANNEL_EDIT_MENU, RSI_CHANNEL_EDIT_MENU);
  ctrl_set_keymap(p_menu, channel_edit_cont_keymap);
  ctrl_set_proc(p_menu, channel_edit_cont_proc);

  p_list_cont =
    ctrl_create_ctrl(CTRL_CONT, IDC_CHANNEL_EDIT_LIST_CONT, CHANNEL_EDIT_LIST_CONTX,
                     CHANNEL_EDIT_LIST_CONTY, CHANNEL_EDIT_LIST_CONTW,
                     CHANNEL_EDIT_LIST_CONTH, p_menu,
                     0);
  ctrl_set_rstyle(p_list_cont, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  
  //head
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PLCONT_HEAD,
                           CHANNEL_EDIT_LIST_HEADX, CHANNEL_EDIT_LIST_HEADY, CHANNEL_EDIT_LIST_HEADW,CHANNEL_EDIT_LIST_HEADH, p_list_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  
  //number
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PLCONT_NUM,
                           CHANNEL_EDIT_LIST_NUMBX, CHANNEL_EDIT_LIST_NUMBY, CHANNEL_EDIT_LIST_NUMBW,CHANNEL_EDIT_LIST_NUMBH, p_list_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NUM);
#ifdef NIT_SETTING
  for(i=0; i<3; i++)
#else  	
  for(i=0; i<4; i++)
#endif  	
  {
    p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PLCONT_LOVE+i,
                            550+i*50,
                            CHANNEL_EDIT_LIST_HEADY+6,
                            30,
                            30,
                            p_list_cont, 0);
    bmap_set_content_by_id(p_ctrl, im_value[i]);
  }
  
  //name
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PLCONT_NAME,
                         CHANNEL_EDIT_LIST_NAMEX, CHANNEL_EDIT_LIST_NAMEY, CHANNEL_EDIT_LIST_NAMEW,CHANNEL_EDIT_LIST_NAMEH, p_list_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_NAME);  
  //prog list
  p_list =
    ctrl_create_ctrl(CTRL_LIST, IDC_PLCONT_LIST, CHANNEL_EDIT_LIST_X, CHANNEL_EDIT_LIST_Y,
                     CHANNEL_EDIT_LIST_W, CHANNEL_EDIT_LIST_H, p_list_cont,
                     0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  if(CUSTOMER_ID == CUSTOMER_AISAT_HOTEL || CUSTOMER_ID == CUSTOMER_AISAT_DIVI)
  	ctrl_set_keymap(p_list, channel_edit_list_hotel_keymap);
  else
  	ctrl_set_keymap(p_list, channel_edit_list_keymap);
  ctrl_set_proc(p_list, channel_edit_list_proc);
  ctrl_set_mrect(p_list, CHANNEL_EDIT_LIST_MIDL, CHANNEL_EDIT_LIST_MIDT,
                 CHANNEL_EDIT_LIST_MIDL + CHANNEL_EDIT_LIST_MIDW, CHANNEL_EDIT_LIST_MIDT + CHANNEL_EDIT_LIST_MIDH);
  list_set_item_interval(p_list, CHANNEL_EDIT_LIST_VGAP);
  list_set_item_rstyle(p_list, &channel_edit_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, view_count, CHANNEL_EDIT_LIST_PAGE);
  list_set_field_count(p_list, CHANNEL_EDIT_LIST_FIELD, CHANNEL_EDIT_LIST_PAGE);
  list_set_focus_pos(p_list, pg_pos);
  list_select_item(p_list, pg_pos);
  list_set_update(p_list, channel_edit_update, 0);
  //list_enable_page_mode(p_list, FALSE);

  for(i = 0; i < CHANNEL_EDIT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(channel_edit_attr[i].attr),
                        (u16)(channel_edit_attr[i].width),
                        (u16)(channel_edit_attr[i].left), (u8)(channel_edit_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, channel_edit_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, channel_edit_attr[i].fstyle);
  }
  
  //scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_CHANNEL_EDIT_BAR, CHANNEL_EDIT_BAR_X,
                            CHANNEL_EDIT_BAR_Y, CHANNEL_EDIT_BAR_W, CHANNEL_EDIT_BAR_H, p_menu, 0);
  ctrl_set_rstyle(p_sbar, RSI_CHANNEL_EDIT_SBAR, RSI_CHANNEL_EDIT_SBAR, RSI_CHANNEL_EDIT_SBAR);
  //ctrl_set_rstyle(p_sbar, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_CHANNEL_EDIT_SBAR_MID, RSI_CHANNEL_EDIT_SBAR_MID,
                     RSI_CHANNEL_EDIT_SBAR_MID);
 // ctrl_set_mrect(p_sbar, 0, 12, CHANNEL_EDIT_BAR_W, CHANNEL_EDIT_BAR_H - 12);
  list_set_scrollbar(p_list, p_sbar);

  channel_edit_update(p_list, list_get_valid_pos(p_list), CHANNEL_EDIT_LIST_PAGE, 0);
//sort container
  p_sort_cont = ctrl_create_ctrl(CTRL_CONT, IDC_CHANNEL_EDIT_SORT_CONT,
                         CHANNEL_EDIT_SORT_CONT_X, CHANNEL_EDIT_SORT_CONT_Y,
                         CHANNEL_EDIT_SORT_CONT_W, CHANNEL_EDIT_SORT_CONT_H,
                         p_menu, 0);
  ctrl_set_rstyle(p_sort_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_sts(p_sort_cont, OBJ_STS_HIDE);


  //sort list title
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CHANNEL_EDIT_SORT_HEAD,
                         CHANNEL_EDIT_SORT_LIST_TITLE_X, CHANNEL_EDIT_SORT_LIST_TITLE_Y,
                         CHANNEL_EDIT_SORT_LIST_TITLE_W, CHANNEL_EDIT_SORT_LIST_TITLE_H,
                         p_sort_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_SORT_TYPE);
  
  //sort list
  p_sort_list = ctrl_create_ctrl(CTRL_LIST, IDC_CHANNEL_EDIT_SORT_LIST,
                           CHANNEL_EDIT_SORT_LIST_X, CHANNEL_EDIT_SORT_LIST_Y,
                           CHANNEL_EDIT_SORT_LIST_W, CHANNEL_EDIT_SORT_LIST_H,
                           p_sort_cont, 0);
  ctrl_set_rstyle(p_sort_list, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_sort_list, channel_edit_sort_keymap);
  ctrl_set_proc(p_sort_list, channel_edit_sort_proc);

  ctrl_set_mrect(p_sort_list, CHANNEL_EDIT_SORT_LIST_MIDL, CHANNEL_EDIT_SORT_LIST_MIDT,
           CHANNEL_EDIT_SORT_LIST_MIDW, CHANNEL_EDIT_SORT_LIST_MIDH);
  list_set_item_rstyle(p_sort_list, &sortgroup_item_rstyle);
  list_set_count(p_sort_list, 5, CHANNEL_EDIT_SORT_LIST_PAGE);

  list_set_field_count(p_sort_list, CHANNEL_EDIT_SORT_LIST_FIELD, CHANNEL_EDIT_SORT_LIST_PAGE);
  list_set_focus_pos(p_sort_list, 0);
 // list_set_update(p_sort_list, fav_edit_grouplist_update, 0);
  for (i = 0; i < CHANNEL_EDIT_SORT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_sort_list, (u8)i, (u32)(group_list_attr[i].attr), (u16)(group_list_attr[i].width),
                        (u16)(group_list_attr[i].left), (u8)(group_list_attr[i].top));
    list_set_field_rect_style(p_sort_list, (u8)i, group_list_attr[i].rstyle);
    list_set_field_font_style(p_sort_list, (u8)i, group_list_attr[i].fstyle);
  } 
  list_set_field_content_by_ascstr(p_sort_list, 0, 0, (u8 *)"a-z");
  list_set_field_content_by_ascstr(p_sort_list, 1, 0, (u8 *)"z-a");
  list_set_field_content_by_ascstr(p_sort_list, 2, 0, (u8 *)"0-9");
  list_set_field_content_by_ascstr(p_sort_list, 3, 0, (u8 *)"9-0");
  list_set_field_content_by_ascstr(p_sort_list, 4, 0, (u8 *)"Default");

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
#ifdef NIT_SETTING
  ui_comm_help_create2(&channel_edit_help_data_aisat_demo, p_cont,FALSE);
#else
  if(CUSTOMER_ID == CUSTOMER_AISAT_HOTEL || CUSTOMER_ID == CUSTOMER_AISAT_DIVI)
  	ui_comm_help_create2(&channel_edit_help_data2, p_cont,FALSE);
  else
  	ui_comm_help_create2(&channel_edit_help_data, p_cont,FALSE);
#endif
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  if(ui_is_mute())
  {
    open_mute(0, 0);
  }

  channel_edit_set_modify_state(FALSE);

  //ui_play_prog(pg_id, FALSE);
  //ui_enable_video_display(TRUE);

  return SUCCESS; 
}
  
static RET_CODE on_channel_edit_sort_set_attr(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_cont, *p_sortgroup_cont, *p_sort_list, *p_list;
  p_cont = ctrl_get_child_by_id(p_ctrl, IDC_CHANNEL_EDIT_LIST_CONT);
  p_sortgroup_cont = ctrl_get_child_by_id(p_ctrl, IDC_CHANNEL_EDIT_SORT_CONT);
  p_list = ctrl_get_child_by_id(p_cont, IDC_PLCONT_LIST);

  if( ctrl_get_sts(p_sortgroup_cont) != OBJ_STS_HIDE)
  {
    ctrl_process_msg(ctrl_get_active_ctrl(p_sortgroup_cont), MSG_LOSTFOCUS, para1, para2);

    ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);

    ctrl_set_sts(p_sortgroup_cont, OBJ_STS_HIDE);
    
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  else
  {
    p_sort_list = ctrl_get_child_by_id(p_sortgroup_cont, IDC_CHANNEL_EDIT_SORT_LIST);

    ctrl_process_msg(ctrl_get_active_ctrl(p_cont), MSG_LOSTFOCUS, para1, para2);

    
    ctrl_set_attr(p_sortgroup_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_sortgroup_cont, OBJ_STS_SHOW);
    ctrl_process_msg(p_sort_list, MSG_GETFOCUS, 0, 0);
  
    ctrl_paint_ctrl(p_sortgroup_cont, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_sort_group_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_cont, *p_sortgroup_cont, *p_lcont, *p_list;
  p_sortgroup_cont = p_ctrl->p_parent;
  p_cont = p_sortgroup_cont->p_parent;
  
  p_lcont = ctrl_get_child_by_id(p_cont, IDC_CHANNEL_EDIT_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_lcont, IDC_PLCONT_LIST);

  ctrl_process_msg(ctrl_get_active_ctrl(p_sortgroup_cont), MSG_LOSTFOCUS, para1, para2);

  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_sortgroup_cont, OBJ_STS_HIDE);
  
  ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}

//lint -e438
static RET_CODE on_sort_group_list_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_cont, *p_sortgroup_cont, *p_lcont, *p_list;
  u8 view_id;
  u16 focus = list_get_focus_pos(p_ctrl);
  u16 list_focus = 0;
  u16 pg_id ,view_pos;
  u16 i, count;
  u16 del_cnt = 0;
  u16 del_pg_id[DB_DVBS_MAX_PRO];
  u16 temp_pg_id = 0;
  u16 pos;

  memset(del_pg_id, 0, sizeof(u16) * DB_DVBS_MAX_PRO);
  view_id = ui_dbase_get_pg_view_id();
  p_sortgroup_cont = p_ctrl->p_parent;
  p_cont = p_sortgroup_cont->p_parent;
  p_lcont = ctrl_get_child_by_id(p_cont, IDC_CHANNEL_EDIT_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_lcont, IDC_PLCONT_LIST);
  list_focus = list_get_focus_pos(p_list);
  pg_id = db_dvbs_get_id_by_view_pos(view_id, list_focus);
  count = db_dvbs_get_count(view_id);
  for(i = 0; i < count; i++)
  {
    if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0) == TRUE)
    {
      temp_pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      del_pg_id[del_cnt] = temp_pg_id;
      db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0);
      del_cnt ++;
    }
  }

  switch(focus)
  {
    case 0:
      ui_dbase_pg_sort_by_sid(view_id);
      ui_dbase_pg_sort_by_mode((dvbs_view_t)view_id, DB_DVBS_A_Z_MODE);
      break;
    case 1:
      ui_dbase_pg_sort_by_sid(view_id);
      ui_dbase_pg_sort_by_mode((dvbs_view_t)view_id, DB_DVBS_Z_A_MODE);
      break;
    case 2:
      ui_dbase_pg_sort_by_sid(view_id);
      ui_dbase_pg_sort_by_mode((dvbs_view_t)view_id, DB_DVBS_0_9_MODE);
      break;
    case 3:
      ui_dbase_pg_sort_by_sid(view_id);
      ui_dbase_pg_sort_by_mode((dvbs_view_t)view_id, DB_DVBS_9_0_MODE);
      break;
    case 4:
      #ifdef LCN_SWITCH_DS_JHC
      ui_dbase_pg_sort_by_logic_num(view_id);
      #else
      if(have_logic_number())
      {
        ui_dbase_pg_sort_by_logic_num(view_id);
      }
      else
      {
        ui_dbase_pg_sort_by_sid(view_id);
      }
      #endif
      break;
    default:
      break;
  }
  //db_dvbs_save(view_id);

  view_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  list_set_focus_pos(p_list, view_pos);
  
  channel_edit_set_modify_state(TRUE);
  
  ctrl_process_msg(ctrl_get_active_ctrl(p_sortgroup_cont), MSG_LOSTFOCUS, para1, para2);

  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_sortgroup_cont, OBJ_STS_HIDE);

  if(del_cnt > 0)
  {
    for(i = 0; i < del_cnt; i++)
    {
      pos = db_dvbs_get_view_pos_by_id(view_id, del_pg_id[i]);
      db_dvbs_change_mark_status(view_id, pos, DB_DVBS_DEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
    }
    del_cnt = 0;
  }
  channel_edit_update(p_list, list_get_valid_pos(p_list), CHANNEL_EDIT_LIST_PAGE, 1);
  
  ctrl_paint_ctrl(p_cont, TRUE);
  
  return SUCCESS;
}
//lint +e438

static RET_CODE on_channel_edit_pwdlg_correct(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();

  return open_channel_edit(0, 0);
}

static RET_CODE on_channel_edit_pwdlg_exit(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  
  return SUCCESS;
}

RET_CODE preopen_channel_edit(u32 para1, u32 para2)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y,
    IDS_MSG_INPUT_PASSWORD,
    channel_edit_pwdlg_keymap,
    channel_edit_pwdlg_proc,
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
    open_channel_edit(0, 0);
  return SUCCESS;
}

BEGIN_KEYMAP(channel_edit_cont_keymap, NULL)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
ON_EVENT(V_KEY_BACK, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_BLUE, MSG_SORT_GROUP)
END_KEYMAP(channel_edit_cont_keymap, NULL)

BEGIN_MSGPROC(channel_edit_cont_proc, cont_class_proc)
ON_COMMAND(MSG_EXIT, on_channel_edit_exit)
ON_COMMAND(MSG_EXIT_ALL, on_channel_edit_exit_all)
ON_COMMAND(MSG_SORT_GROUP, on_channel_edit_sort_set_attr)
END_MSGPROC(channel_edit_cont_proc, cont_class_proc)

BEGIN_KEYMAP(channel_edit_list_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)    
ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)
ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
#ifdef CUSTOMER_YINHE_TR
ON_EVENT(V_KEY_TVRADIO, MSG_RED)
ON_EVENT(V_KEY_FAV, MSG_GREEN)
ON_EVENT(V_KEY_F4, MSG_DEL)
#else
ON_EVENT(V_KEY_RED, MSG_RED)
ON_EVENT(V_KEY_GREEN, MSG_GREEN)
ON_EVENT(V_KEY_RECALL, MSG_DEL)

#ifndef NIT_SETTING
ON_EVENT(V_KEY_STOP, MSG_DEL)
#endif
#endif
ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
#ifdef CUSTOMER_YINHE_LINGGANG
ON_EVENT(V_KEY_BLUE, MSG_DEL)
#else
ON_EVENT(V_KEY_BLUE, MSG_BLUE)
#endif
ON_EVENT(V_KEY_OK, MSG_YES)
ON_EVENT(V_KEY_TVRADIO, MSG_AUTO_SWITCH)
END_KEYMAP(channel_edit_list_keymap, NULL)

BEGIN_KEYMAP(channel_edit_list_hotel_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)    
ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)
ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
ON_EVENT(V_KEY_FAV, MSG_RED)
ON_EVENT(V_KEY_GREEN, MSG_GREEN)
ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
ON_EVENT(V_KEY_BLUE, MSG_BLUE)
ON_EVENT(V_KEY_RED, MSG_DEL)
ON_EVENT(V_KEY_OK, MSG_YES)
ON_EVENT(V_KEY_TVRADIO, MSG_AUTO_SWITCH)
END_KEYMAP(channel_edit_list_hotel_keymap, NULL)

BEGIN_MSGPROC(channel_edit_list_proc, list_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_channel_edit_list_change_channel)
ON_COMMAND(MSG_FOCUS_DOWN, on_channel_edit_list_change_channel)
ON_COMMAND(MSG_PAGE_UP, on_channel_edit_list_change_channel)
ON_COMMAND(MSG_PAGE_DOWN, on_channel_edit_list_change_channel)
ON_COMMAND(MSG_YES, on_channel_edit_ok)
ON_COMMAND(MSG_GREEN, on_channel_edit_set_attr)
ON_COMMAND(MSG_RED, on_channel_edit_set_attr)
ON_COMMAND(MSG_YELLOW, on_channel_edit_set_attr)
ON_COMMAND(MSG_DEL, on_channel_edit_set_attr)
ON_COMMAND(MSG_AUTO_SWITCH, on_channel_edit_tvradio)
END_MSGPROC(channel_edit_list_proc, list_class_proc)

BEGIN_KEYMAP(channel_edit_sort_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  //ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  //ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_SORTGROUP)
  ON_EVENT(V_KEY_BACK, MSG_EXIT_SORTGROUP)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_SORTGROUP)
END_KEYMAP(channel_edit_sort_keymap, NULL)

BEGIN_MSGPROC(channel_edit_sort_proc, list_class_proc)
  //ON_COMMAND(MSG_FOCUS_LEFT, on_fav_edit_change_list)
  ON_COMMAND(MSG_EXIT_SORTGROUP, on_sort_group_exit)
  ON_COMMAND(MSG_SELECT, on_sort_group_list_select)
  //ON_COMMAND(MSG_ADD, on_fav_edit_add_fav)
END_MSGPROC(channel_edit_sort_proc, list_class_proc)

BEGIN_KEYMAP(channel_edit_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(channel_edit_pwdlg_keymap, NULL)

BEGIN_MSGPROC(channel_edit_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_channel_edit_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_channel_edit_pwdlg_exit)
END_MSGPROC(channel_edit_pwdlg_proc, cont_class_proc)

