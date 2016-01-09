/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_prog_list.h"
#include "ui_fav_set.h"
#include "ui_sort_list.h"
#include "ui_rename_v2.h"
#include "ui_signal.h"
#include "customer_config.h"

static list_xstyle_t plist_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
};

static list_xstyle_t plist_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t plist_field_rstyle_left =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
  RSI_LIST_FIELD_LEFT_SH,
  RSI_LIST_FIELD_LEFT_HL,
};

static list_xstyle_t plist_field_rstyle_mid =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
  RSI_LIST_FIELD_MID_SH,
  RSI_LIST_FIELD_MID_HL,
};

static list_field_attr_t plist_attr[PLIST_LIST_FIELD] =
{
  {LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
   80, 0, 0, &plist_field_rstyle_left, &plist_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
   185, 80, 0, &plist_field_rstyle_mid, &plist_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   28, 265, 0, &plist_field_rstyle_mid, &plist_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   28, 293, 0, &plist_field_rstyle_mid, &plist_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   28, 321, 0, &plist_field_rstyle_mid, &plist_field_fstyle},
  {LISTFIELD_TYPE_ICON,
   28, 349, 0, &plist_field_rstyle_mid, &plist_field_fstyle},
};

static comm_help_data_t2 prog_list_help_data = //help bar data
{
  4, 380, {40, 250, 40,250},
  {
    HELP_RSC_BMP   | IM_OK,
    HELP_RSC_STRID | IDS_PLAY,
    HELP_RSC_BMP   | IM_CHANGE,
    HELP_RSC_STRID | IDS_TV_RADIO,
  },
};

#define IDC_PLIST_CONT_ID        1
#define IDC_PLIST_LIST_PWD_ID    1

enum local_msg
{
  MSG_GRAY = MSG_LOCAL_BEGIN + 650,
  MSG_RED,
  MSG_GREEN,
  MSG_YELLOW,
  MSG_BLUE,
  MSG_NUM_PLAY,
  MSG_SAT_SELECT,
  MSG_OPEN_BC,
  MSG_INFO,
  MSG_SAT_LIST,
};

enum plist_ctrl_id
{
  IDC_INVALID = 0,
  IDC_PLIST_LIST_CONT,
  IDC_PLIST_BAR,
  IDC_PLIST_PREV,
  IDC_PLIST_BRIEF,
  IDC_PLIST_ICON,
  IDC_PLIST_BRIEF_ICON,
};

enum plist_cont_ctrl_id
{
  IDC_PLCONT_TITLE = 1,
  IDC_PLIST_TOP_LINE,
  IDC_PLIST_BOTTOM_LINE,
  IDC_PLCONT_GROUP,
  IDC_PLCONT_MBOX,
  IDC_PLCONT_LIST,
  IDC_PLIST_MOVE_ICON,
  IDC_PLCONT_MOVE_HELP,
};

enum plist_brief_item_id
{
  IDC_PLBRIEF_PG = 1,
  IDC_PLBRIEF_FREQ,
  IDC_PLBRIEF_PID,
  IDC_PLBRIEF_CA,
};

enum menu_state_t
{
  MS_NOR = 1,
  MS_FAV,
  MS_LOCK,
  MS_SKIP,
  MS_MOVE,
  MS_EDIT_NOR,
  MS_EDIT_SORT,
  MS_EDIT_EDIT,
  MS_EDIT_DEL,
};

static BOOL is_tv = FALSE;
static BOOL is_modified = FALSE;
static u8 ms = MS_NOR;
static u8 old_ms = MS_NOR;
static BOOL is_pwd_chked = FALSE;
static BOOL is_move = FALSE;
static BOOL is_open_list = FALSE;
static BOOL is_menu_closed = FALSE;
//static BOOL is_recall = FALSE;


comm_dlg_data_t plist_change_group_dlg_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_MSG_ASK_FOR_SAV,
  0,
};

comm_dlg_data_t plist_exit_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_MSG_ASK_FOR_SAV,
  0,
};

static RET_CODE close_prog_list(u32 para1, u32 para2);
void plist_set_brief_content(control_t *p_menu, BOOL is_paint);
static void plist_play_pg(u16 focus);
static RET_CODE plist_update(control_t *ctrl, u16 start, u16 size, u32 context);
static void plist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id);
BOOL plist_get_modify_state(void);
static void plist_list_change_group(control_t *p_list);

u16 plist_cont_keymap(u16 key);
RET_CODE plist_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 plist_group_keymap(u16 key);
RET_CODE plist_group_proc(control_t *p_cbox, u16 msg, u32 para1, u32 para2);

u16 plist_list_keymap(u16 key);
RET_CODE plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 plist_pwd_keymap(u16 key);
RET_CODE plist_pwd_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

comm_pwdlg_data_t plist_pwd =
{
  ROOT_ID_INVALID,
  PLIST_PWD_X, PLIST_PWD_Y,
  IDS_MSG_INPUT_PASSWORD,
  NULL,
  NULL,
  PWDLG_T_COMMON
};

extern BOOL have_logic_number(void);

void plist_set_pwd_state(BOOL state)
{
  is_pwd_chked = state;
}


BOOL plist_get_pwd_state(void)
{
  return is_pwd_chked;
}


void plist_set_modify_state(BOOL state)
{
  is_modified = state;
}


BOOL plist_get_modify_state(void)
{
  return is_modified;
}


void plist_set_move_state(BOOL state)
{
  is_move = state;
}


BOOL plist_get_move_state(void)
{
  return is_move;
}

static void plist_do_save_all(void)
{
  u16 view_type;
  u32 group_context;
  u16 i, count;
  u8 view_id;
  //BOOL pg_del_flag = FALSE;


  comm_dlg_data_t dlg_data =
  {
    ROOT_ID_PROG_LIST,
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
        //pg_del_flag = TRUE;
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

  //if curn view is invalid, that means no pg saved now, so we should
  //do nothing about it.
  if(view_type != DB_DVBS_INVALID_VIEW)
  {
    ui_dbase_set_pg_view_id(
      ui_dbase_create_view((dvbs_view_t)view_type, group_context, NULL));
  }
#if 0
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);

  sys_status_set_curn_group_info(prog_id);
  sys_status_save();
#endif
  book_check_node_on_delete();

  ui_comm_dlg_close();
}


static void plist_undo_save_all(void)
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


#if 0
static u16 plist_pre_focus_pg()
{
  u16 curn_pgid = 0, curn_focus = 0, pg_count = 0, count = 0;
  u8 view_id = 0;

  curn_pgid = sys_status_get_curn_group_curn_prog_id();
  view_id = ui_dbase_get_pg_view_id();
  curn_focus = db_dvbs_get_view_pos_by_id(view_id, curn_pgid);

  pg_count = db_dvbs_get_count(view_id);
  return count = pg_count - curn_focus - 1;
}


static void plist_suf_focus_pg(u16 count)
{
  u16 pg_count = 0, focus = 0;
  u8 view_id;


  view_id = ui_dbase_get_pg_view_id();
  pg_count = db_dvbs_get_count(view_id);
  if(count == 0)
  {
    focus = pg_count - 1;
  }
  else
  {
    focus = pg_count - count;
  }

  plist_play_pg(focus);
}


#endif

static BOOL plist_save_data(void)
{
  dlg_ret_t dlg_ret;
  u8 org_mode, curn_mode;

  //u16 count = 0;
  org_mode = sys_status_get_curn_prog_mode();

  if(plist_get_modify_state())
  {
    plist_exit_data.parent_root = ROOT_ID_PROG_LIST;

    dlg_ret = ui_comm_dlg_open(&plist_exit_data);

    if(dlg_ret == DLG_RET_YES)
    {
      //count = plist_pre_focus_pg();
      plist_do_save_all();
      //plist_suf_focus_pg(count);
    }
    else
    {
      plist_undo_save_all();
    }

    ui_set_frontpanel_by_curn_pg();
    plist_set_modify_state(FALSE);
    plist_set_move_state(FALSE);
  }

  plist_set_pwd_state(FALSE);
  ui_enable_chk_pwd(TRUE);

  curn_mode = sys_status_get_curn_prog_mode();

  return (org_mode == curn_mode) ? TRUE : FALSE;
}


static void plist_reset_list(control_t *p_list, u16 old_cnt)
{
  control_t *p_cbox;
  u8 vid = 0;
  u16 new_cnt, new_pos=0;
  u16 list_cnt = 0;
  u16 curn_focus = 0;
  u16 curn_pgid = 0;

  //old_cnt = sys_status_get_all_group_num();

  curn_pgid = sys_status_get_curn_group_curn_prog_id();

  vid = ui_dbase_get_pg_view_id();

  list_cnt = db_dvbs_get_count(vid);

  curn_focus = db_dvbs_get_view_pos_by_id(vid, curn_pgid);

  //reset list count & focus.
  list_set_count(p_list, list_cnt, PLIST_LIST_PAGE);

  list_set_focus_pos(p_list, curn_focus);

  if(list_get_item_status(p_list, curn_focus) != LIST_ITEM_SELECTED)
  {
    list_class_proc(p_list, MSG_SELECT, 0, 0);
    plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    plist_play_pg(curn_focus);
  }
  else
  {
    plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list, TRUE);
  }

  new_cnt = sys_status_get_all_group_num();

  //group changed, reset group name
  if(old_cnt != new_cnt)
  {
    //reset cbox content(list title)
    p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PLCONT_GROUP);
    cbox_dync_set_count(p_cbox, new_cnt);
    /*
     * cbox use logic position, new_group is physical position.
     * save list when the pg in one group have been del.
     */
    cbox_dync_set_focus(p_cbox, new_pos);
    ctrl_paint_ctrl(p_cbox, TRUE);
  }

  // update curn_prog info
  plist_set_brief_content(ctrl_get_parent(ctrl_get_parent(p_list)), TRUE);
}


static RET_CODE on_pwd_close(control_t *p_ctrl, u16 msg, u32 para1,
                             u32 para2)
{
  ui_comm_pwdlg_close();

  return SUCCESS;
}


static RET_CODE on_pwd_correct(control_t *p_ctrl, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_MSG, FALSE,
                         MSG_CORRECT_PWD, 0, 0);

  ui_comm_pwdlg_close();

  return ret;
}


static void plist_start_move(control_t *p_list)
{
  u16 total;
  u8 view_id;
  u16 i = 0;
  u16 Index = 0xfffe;
  view_id = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(view_id);
  if(MS_MOVE == ms)
  {
    if(!(plist_get_move_state()))
    {
      for(i = 0; i < total; i++)
      {
        if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0))
        {
          Index = i;
          break;
        }
      }

      plist_set_move_state(TRUE);

      if(Index != total)
      {
        list_set_focus_pos(p_list, Index);

        plist_play_pg(Index);

        plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0xFEFE);

        ctrl_paint_ctrl(p_list, TRUE);
      }
    }
  }

}

RET_CODE on_plist_state_process(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 total;
  u8 view_id;
  BOOL ret;
  control_t *p_mbox = NULL;
  u16 i = 0;
  u16 bmp_n[PLIST_MBOX_TOL] =
  {
    RSC_INVALID_ID, RSC_INVALID_ID,
    RSC_INVALID_ID, RSC_INVALID_ID,
    RSC_INVALID_ID
  };
  u16 bmp_f[PLIST_MBOX_TOL] =
  {
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID, RSC_INVALID_ID
  };
  u16 edit_n[PLIST_MBOX_TOL] =
  {
    RSC_INVALID_ID, RSC_INVALID_ID,
    RSC_INVALID_ID, RSC_INVALID_ID,
    RSC_INVALID_ID
  };
  u16 edit_f[PLIST_MBOX_TOL] =
  {
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID,
    RSC_INVALID_ID, RSC_INVALID_ID
  };
  u16 str[PLIST_MBOX_TOL] = {IDS_FAV, IDS_LOCK, IDS_SKIP, IDS_MOVE, IDS_EDIT};
  u16 edit_str[PLIST_MBOX_TOL] =
  {RSC_INVALID_ID, IDS_SORT, IDS_EDIT, IDS_DEL, RSC_INVALID_ID};
  u16 old_cnt;

  old_cnt = sys_status_get_all_group_num();
  view_id = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(view_id);
  //do something before state switch.
  switch(msg)
  {
    case MSG_BLUE:
      switch(ms)
      {
        case MS_FAV:
        case MS_LOCK:
        case MS_SKIP:
        case MS_MOVE:
        case MS_NOR:
          if(plist_save_data())
          {
            plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);
            ctrl_paint_ctrl(p_list, TRUE);
          }
          else
          {
            close_prog_list(0, 0);
            return SUCCESS;
          }
          break;
        default:
          break;
      }
      break;
    case MSG_EXIT:
      switch(ms)
      {
        case MS_NOR:
          plist_save_data();
          close_signal();
          close_prog_list(0, 0);
          return SUCCESS;
        case MS_EDIT_NOR:
          ret = plist_get_modify_state();
          if(plist_save_data())
          {
            if(ret)
            {
              plist_reset_list(p_list, old_cnt);
            }
          }
          else
          {
            close_prog_list(0, 0);
            return SUCCESS;
          }
          break;
        default:
          break;
      }
      break;
    case MSG_RED:
      switch(ms)
      {
        case MS_EDIT_DEL:
        case MS_EDIT_EDIT:
        case MS_EDIT_NOR:
          ret = plist_get_modify_state();
          if(plist_save_data())
          {
            if(ret)
            {
              plist_reset_list(p_list, old_cnt);
            }
          }
          else
          {
            close_prog_list(0, 0);
            return SUCCESS;
          }
          break;
        default:
          break;
      }
      break;

    case MSG_EXIT_ALL:
      switch(ms)
      {
        case MS_EDIT_DEL:
        case MS_EDIT_EDIT:
        case MS_EDIT_NOR:
        case MS_EDIT_SORT:
          ret = plist_get_modify_state();
          if(plist_save_data())
          {
            if(ret)
            {
              plist_reset_list(p_list, old_cnt);
            }
          }
          else
          {
            ui_close_all_mennus(); //no program in database, exit to mainmenu.
            //ui_stop_play(STOP_PLAY_BLACK, TRUE);
            return SUCCESS;
          }
          break;
        default:
          plist_save_data();
          ui_close_all_mennus();
          //ui_stop_play(STOP_PLAY_BLACK, TRUE);
          break;
      }
      break;
      case MSG_SAT_LIST:
          {
            if(ms == MS_NOR || ms == MS_EDIT_NOR)
            {
                //is_recall = TRUE;
            }
            else
            {
                //is_recall = FALSE;
            }
          }
        break;
    default:
      break;
  }


  //state switch.
  old_ms = ms;

  switch(ms)
  {
    case MS_NOR:
      switch(msg)
      {
        case MSG_GRAY:
        case MSG_RED:
        case MSG_GREEN:
        case MSG_YELLOW:
          ms = (u8)(MS_FAV + (msg - MSG_GRAY));
          break;
        case MSG_BLUE:
          ms = MS_EDIT_NOR;
//          db_dvbs_pg_sort_init(ui_dbase_get_pg_view_id());
          break;
        default:
          break;
      }
      break;
    case MS_FAV:
    case MS_LOCK:
    case MS_SKIP:
    case MS_MOVE:
      switch(msg)
      {
        case MSG_FOCUS_LEFT:
        case MSG_FOCUS_RIGHT:
        case MSG_EXIT:

        case MSG_SAT_SELECT:
          ms = MS_NOR;
          break;
        case MSG_YELLOW:
          plist_start_move(p_list);
          break;

        default:
          break;
      }
      break;
    case MS_EDIT_NOR:
      switch(msg)
      {
        case MSG_EXIT:
          ms = MS_NOR;
//          db_dvbs_pg_sort_deinit();
          ret = plist_get_modify_state();
          if(plist_save_data())
          {
            if(ret)
            {
              plist_reset_list(p_list, old_cnt);
            }
          }
          break;
        case MSG_RED:
          ms = MS_EDIT_SORT;
          break;
        case MSG_GREEN:
          ms = MS_EDIT_EDIT;
          ret = plist_get_modify_state();
          if(plist_save_data())
          {
            if(ret)
            {
              plist_reset_list(p_list, old_cnt);
            }
          }
          break;
        case MSG_BLUE:
          ms = MS_EDIT_DEL;
          ret = plist_get_modify_state();
          if(plist_save_data())
          {
            if(ret)
            {
              plist_reset_list(p_list, old_cnt);
            }
          }
          break;
        case MSG_EXIT_ALL:
          ms = MS_NOR;
//          db_dvbs_pg_sort_deinit();
          ret = plist_get_modify_state();
          if(plist_save_data())
          {
            if(ret)
            {
              plist_reset_list(p_list, old_cnt);
            }
          }
          break;
        default:
          break;
      }
      break;
    case MS_EDIT_SORT:
      switch(msg)
      {
        case MSG_EXIT: //cancle
        case MSG_YES:  //ok
        case MSG_SAT_SELECT:
          ms = MS_EDIT_NOR;
          break;
        case MSG_EXIT_ALL:
          ms = MS_NOR;
          break;
        default:
          break;
      }
      break;
    case MS_EDIT_EDIT:
    case MS_EDIT_DEL:
      switch(msg)
      {
        case MSG_EXIT: //cancle
        case MSG_FOCUS_LEFT:
        case MSG_FOCUS_RIGHT:
        case MSG_SAT_SELECT:
        case MSG_RED:
          ms = MS_EDIT_NOR;
          break;
        case MSG_EXIT_ALL:
          ms = MS_NOR;
          break;
        default:
          break;
      }
      break;
    default:
      MT_ASSERT(0);
  }

  //do something after state switch.

  if(old_ms != ms)
  {
    //set mbox picture
    p_mbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PLCONT_MBOX);
    switch(ms)
    {
      case MS_NOR:
      case MS_FAV:
      case MS_LOCK:
      case MS_SKIP:
      case MS_MOVE:
        for(i = 0; i < PLIST_MBOX_TOL; i++)
        {
          mbox_set_content_by_icon(p_mbox, i, bmp_n[i], bmp_n[i]);
          mbox_set_content_by_strid(p_mbox, i, str[i]);
          if((i == (ms - MS_NOR))
            && (ms != MS_NOR))
          {
            mbox_set_content_by_icon(p_mbox, (i - 1), bmp_f[i - 1],
                                     bmp_f[i - 1]);
          }
        }
        ctrl_paint_ctrl(p_mbox, TRUE);
        break;
      case MS_EDIT_NOR:
      case MS_EDIT_SORT:
      case MS_EDIT_EDIT:
      case MS_EDIT_DEL:
        for(i = 0; i < PLIST_MBOX_TOL; i++)
        {
          mbox_set_content_by_icon(p_mbox, i, edit_n[i], edit_n[i]);
          mbox_set_content_by_strid(p_mbox, i, edit_str[i]);
          if((i == (ms - MS_EDIT_NOR))
            && (ms != MS_EDIT_NOR))
          {
            mbox_set_content_by_icon(p_mbox, i, edit_f[i], edit_f[i]);
          }
        }
        ctrl_paint_ctrl(p_mbox, TRUE);
        if(ms == MS_EDIT_SORT)
        {
          manage_open_menu(ROOT_ID_SORT_LIST,
                           ROOT_ID_PROG_LIST, 0);
        }
        break;
      default:
        MT_ASSERT(0);
    }

    //clean move icon if current state is not move state
    if((ms != MS_MOVE)
      && (old_ms == MS_MOVE))
    {
      plist_set_move_state(FALSE);
      list_set_field_content_by_icon(p_list,
                                     list_get_focus_pos(p_list), 5, RSC_INVALID_ID);
      for(i = 0; i < total; i++)
      {
        if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0))
        {
          db_dvbs_change_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0);
        }
      }
      plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);
      ctrl_paint_ctrl(p_list, TRUE);
    }
  }
  if(msg != MSG_EXIT_ALL)
  {
    if(ms == MS_NOR || ms == MS_EDIT_NOR)
    {
    }
    else if(msg != MSG_YES)
    {
      if((msg != MSG_FOCUS_UP && msg != MSG_FOCUS_DOWN) )
      {
      }
    }
  }
  return SUCCESS;
}

RET_CODE on_plist_start_move(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  plist_start_move(p_list);
  return SUCCESS;
}


static void plist_move_list_item(control_t *p_list, u16 msg)
{
  s16 offset;
  u16 i, total, old_focus, new_focus;
  u8 view_id;
  u16 base = 0, cnt = 0;

  old_focus = list_get_focus_pos(p_list);
  view_id = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(view_id);

  switch(msg)
  {
    case MSG_FOCUS_UP:
      offset = -1;
      break;
    case MSG_FOCUS_DOWN:
      offset = 1;
      break;
    case MSG_PAGE_UP:
      offset = (0 - PLIST_LIST_PAGE);
      if(total < PLIST_LIST_PAGE)
      {
        return;
      }
      break;
    case MSG_PAGE_DOWN:
      offset = PLIST_LIST_PAGE;
      if(total < PLIST_LIST_PAGE)
      {
        return;
      }
      break;
    default:
      MT_ASSERT(0);
      offset = 0;
  }

  plist_set_modify_state(TRUE);

  if(offset == 0)
  {
    return;
  }

  new_focus = (u16)((old_focus + total + offset) % total);

  //found the first select item.
  base = old_focus;

  //assemble all the select item.
  for(i = base + 1; i < total; i++)
  {
    if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0))
    {
      db_dvbs_move_item_in_view(view_id, i, (base + cnt + 1) % total);

      cnt++;
    }
  }

  for(i = 0; i < old_focus; i++)
  {
    if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0))
    {
      db_dvbs_move_item_in_view(view_id, i, (base + cnt + 1) % total);

      cnt++;
    }
  }

  //move the items.
  if(offset > 0)
  {
    for(i = 0; i < offset; i++)
    {
      if((base + cnt + 1) < total)
      {
        db_dvbs_move_item_in_view(view_id, base + cnt + 1, base);
      }
      else
      {
        db_dvbs_move_item_in_view(view_id, (total - 1), 0);
      }

      base = (base + 1) % total;
    }
  }
  else
  {
    for(i = 0; i < (0 - offset); i++)
    {
      if((base + cnt) % total > cnt)
      {
        db_dvbs_move_item_in_view(view_id, (base + total - 1) % total, (base + cnt) % total);
      }
      else
      {
        db_dvbs_move_item_in_view(view_id, 0, (total - 1));
      }

      base = (base + total - 1) % total;
    }
  }

  list_set_focus_pos(p_list, new_focus);
  plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0xFEFE);
  ctrl_paint_ctrl(p_list, TRUE);
}


static void plist_play_pg(u16 focus)
{
  u16 rid;

  if((rid =
        db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                   focus)) != INVALIDID)
  {
    ui_play_prog(rid, FALSE);
  }
}


static void plist_group_change_group(control_t *p_cbox)
{
  u16 curn_group, curn_focus, group_num;

  curn_group = sys_status_get_curn_group();
  curn_focus = sys_status_get_pos_by_group(curn_group);

  group_num = sys_status_get_all_group_num();

  cbox_dync_set_count(p_cbox, group_num);
  cbox_dync_set_focus(p_cbox, curn_focus);
  ctrl_paint_ctrl(ctrl_get_parent(p_cbox), TRUE);
}


static void plist_list_change_group(control_t *p_list)
{
  u16 curn_group;
  u8 curn_mode, new_type;
  u16 pos_in_set;
  u8 view_id = 0;
  u16 pg_id = 0, pg_pos;
  u32 new_context;
  u16 view_count;

  curn_mode = sys_status_get_curn_prog_mode();

  curn_group = sys_status_get_curn_group();

  sys_status_get_group_info(curn_group, &new_type, &pos_in_set, &new_context);

  switch(new_type)
  {
    case GROUP_T_ALL:
      sys_status_get_group_all_info(curn_mode, &pg_id);
      view_id = ui_dbase_create_view(
        (curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO,
        new_context, NULL);
      break;
    case GROUP_T_FAV:
      sys_status_get_fav_group_info(pos_in_set, curn_mode, &pg_id);
      view_id = ui_dbase_create_view(
        (curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV : DB_DVBS_FAV_RADIO,
        new_context, NULL);
      break;
    default:
      MT_ASSERT(0);
  }

  ui_dbase_set_pg_view_id(view_id);
  pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);

  view_count = db_dvbs_get_count(view_id);
  list_set_count(p_list, view_count, PLIST_LIST_PAGE);
  list_set_focus_pos(p_list, pg_pos);
  plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);

  // update curn_prog info
  plist_set_brief_content(ctrl_get_parent(ctrl_get_parent(p_list)), TRUE);

  ctrl_paint_ctrl(ctrl_get_parent(p_list), TRUE);

  if(list_get_item_status(p_list, pg_pos) != LIST_ITEM_SELECTED)
  {
    list_class_proc(p_list, MSG_SELECT, 0, 0);
    plist_play_pg(pg_pos);
  }
}


static RET_CODE plist_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
                                   u16 max_length)
{
  u16 FunnyConvert = focus;

  /*
   * focus is logic position, FunnyConvert is physical position.
   */
  sys_status_get_group_name(sys_status_get_group_by_pos(FunnyConvert), p_str, max_length);
  return SUCCESS;
}


void plist_set_brief_content(control_t *p_menu, BOOL is_paint)
{
  control_t *p_brief, *p_list;
  control_t *p_list_cont;
  control_t *p_brief_item;
  u16 pg_id, focus;
  u16 uni_str[32] = {0};
  u8 asc_str[32] = {0};
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;
  u16 len = 0;

  p_list_cont = ctrl_get_child_by_id(p_menu, IDC_PLIST_CONT_ID);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PLCONT_LIST);

  focus = list_get_focus_pos(p_list);
  pg_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                     focus);

  MT_ASSERT(pg_id != INVALIDID);
  if(db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }

  if(db_dvbs_get_tp_by_id((u16)(pg.tp_id), &tp) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }

  p_brief = ctrl_get_child_by_id(p_menu, IDC_PLIST_BRIEF);
  
  //pg name
  p_brief_item = ctrl_get_child_by_id(p_brief, IDC_PLBRIEF_PG);
  //--ui_dbase_name2str(pg.name, uni_str, 16);
  len = (u16)uni_strlen(pg.name);
  uni_strncpy(uni_str, pg.name, len);
  text_set_content_by_unistr(p_brief_item, uni_str);

  //tp.freq, tp.polarity, tp.sym
  p_brief_item = ctrl_get_child_by_id(p_brief, IDC_PLBRIEF_FREQ);
  sprintf((char *)asc_str, "%d/%c/%d", (int)tp.freq, tp.polarity ? 'V' : 'H', (int)tp.sym);
  //--ui_dbase_name2str(asc_str, uni_str, 31);
  str_nasc2uni(asc_str, uni_str, 31);
  text_set_content_by_unistr(p_brief_item, uni_str);

  //video Audio Pcr pid
  p_brief_item = ctrl_get_child_by_id(p_brief, IDC_PLBRIEF_PID);
  sprintf((char *)asc_str, "PID:%d/%d/%d", pg.video_pid, pg.audio[pg.audio_channel].p_id,
          pg.pcr_pid);
  //--ui_dbase_name2str(asc_str, uni_str, 31);
  str_nasc2uni(asc_str, uni_str, 31);
  text_set_content_by_unistr(p_brief_item, uni_str);

  //ca info
  p_brief_item = ctrl_get_child_by_id(p_brief, IDC_PLBRIEF_CA);
  text_set_content_by_ascstr(p_brief_item, (u8 *)ui_dbase_get_ca_system_desc(&pg));
  if(is_paint)
  {
    ctrl_paint_ctrl(p_brief, TRUE);
  }
}


static RET_CODE on_plist_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  struct control *p_list_cont = NULL;
  struct control *p_list = NULL;

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PLIST_CONT_ID);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PLCONT_LIST);


  on_plist_state_process(p_list, msg, 0, 0);

  return SUCCESS;
}


static RET_CODE on_plist_exit_all(control_t *p_cont, u16 msg,
                                  u32 para1, u32 para2)
{
  struct control *p_list_cont = NULL;
  struct control *p_list = NULL;

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PLIST_CONT_ID);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PLCONT_LIST);

  on_plist_state_process(p_list, msg, 0, 0);

  return SUCCESS;
}


static RET_CODE on_signal_process(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  //do nothing
  return SUCCESS;
}


static RET_CODE on_plist_save(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  // enable check lock again
  ui_enable_chk_pwd(TRUE);

  if(plist_get_modify_state())
  {
    plist_undo_save_all();
  }
  else
  {
    plist_do_save_all();
  }

  // play curn pg
  ui_play_curn_pg();

  return SUCCESS;
}

static RET_CODE on_plist_num_play(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  ////OS_PRINTF("open num play\n");
  return manage_open_menu(ROOT_ID_NUM_PLAY, para1, 0);
}


static RET_CODE on_plist_num_select(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
  control_t *p_lcont = NULL;
  control_t *p_list = NULL;
  u16 pg_pos = (u16)para1;

  ////OS_PRINTF("num select\n");

  p_lcont = ctrl_get_child_by_id(p_ctrl, IDC_PLIST_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_lcont, IDC_PLCONT_LIST);

  if(list_get_item_status(p_list, pg_pos) != LIST_ITEM_SELECTED)
  {
    list_set_focus_pos(p_list, pg_pos);
    list_class_proc(p_list, MSG_SELECT, 0, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    plist_set_brief_content(p_ctrl, TRUE);
    plist_play_pg(pg_pos);
  }

  return SUCCESS;
}

//lint -e438 -e550 -e830
static RET_CODE on_plist_tvradio(control_t *p_ctrl, u16 msg,
                                 u32 para1, u32 para2)
{
  control_t *p_lcont, *p_list, *p_cbox;
  u16 pgid, focus, total, group_num, new_group;
  u8 view_id;
  u16 view_type;
  u32 context;

  //control_t *p_icon;

  p_lcont = ctrl_get_child_by_id(p_ctrl, IDC_PLIST_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_lcont, IDC_PLCONT_LIST);
  p_cbox = ctrl_get_child_by_id(p_lcont, IDC_PLCONT_GROUP);

  //try to save modifications first
  if(!plist_save_data())
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

    on_plist_state_process(p_list, MSG_SAT_SELECT, 0, 0);

    return ERR_FAILURE;
  }
  else
  {
    sys_status_get_curn_view_info(&view_type, &context);
    view_id = ui_dbase_create_view((dvbs_view_t)view_type, context, NULL);
    ui_dbase_set_pg_view_id(view_id);
    is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);

    //reset list.
    focus = db_dvbs_get_view_pos_by_id(view_id, pgid);
    total = db_dvbs_get_count(view_id);

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
    //list_set_count(p_list, total, PLIST_LIST_PAGE);
    //list_set_focus_pos(p_list, focus);
    //plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);
    //ctrl_paint_ctrl(p_list, TRUE);

    // update curn_prog info
    //plist_set_brief_content(p_ctrl, TRUE);

    //reset prog list icon
    //p_icon = ui_comm_root_get_ctrl(ROOT_ID_PROG_LIST, IDC_COMM_TITLE_ICON);
    //bmap_set_content_by_id(p_icon,
    // is_tv ? IM_TITLEICON_TV: IM_TITLEICON_RADIO);
    //ctrl_paint_ctrl(p_icon, TRUE);

    //reset prog list title
    ui_comm_title_set_content(p_ctrl->p_parent, is_tv ? IDS_TV_CHANNEL_LIST : IDS_RADIO_CHANNEL_LIST);
    ui_comm_title_update(p_ctrl->p_parent);
    //reset group cbox
    group_num = sys_status_get_all_group_num();
    new_group = sys_status_get_curn_group();
    cbox_dync_set_count(p_cbox, group_num);
    {
      u16 Pos = sys_status_get_pos_by_group(new_group);
      /*
       * cbox use logic position, new_group is physical position..
       */
      cbox_dync_set_focus(p_cbox, Pos);
    }
    ctrl_paint_ctrl(p_cbox, TRUE);
    //  plist_play_pg(list_get_focus_pos(p_list));
  }

  return SUCCESS;
}
//lint +e438 +e550 +e830

static RET_CODE on_plist_destory(control_t *p_ctrl, u16 msg,
                                 u32 para1, u32 para2)
{
  ui_set_frontpanel_by_curn_pg();

  return ERR_NOFEATURE;
}


static RET_CODE on_plist_correct_pwd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 view_id;
  u16 focus;

  plist_set_pwd_state(TRUE);
  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_ctrl);

  if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0) == TRUE)
  {
    db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0);
    list_set_field_content_by_icon(p_ctrl, focus, 3, RSC_INVALID_ID);
  }
  else
  {
    db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, DB_DVBS_PARAM_ACTIVE_FLAG);
    list_set_field_content_by_icon(p_ctrl, focus, 3, IM_TV_LOCK);
  }

  list_draw_field_ext(p_ctrl, focus, 3, TRUE);
  plist_set_modify_state(TRUE);

  return SUCCESS;
}


static RET_CODE on_plist_group_close_list(control_t *p_cbox, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list;

  if(!is_menu_closed)
  {
    p_list = ctrl_get_child_by_id(ctrl_get_parent(p_cbox), IDC_PLCONT_LIST);

    ctrl_process_msg(p_cbox, MSG_LOSTFOCUS, 0, 0);
    ctrl_paint_ctrl(p_cbox, TRUE);

    ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_list, TRUE);
  }
  return SUCCESS;
}


static RET_CODE on_plist_group_change_group(control_t *p_cbox, u16 msg, u32 para1, u32 para2)
{
#if 0
  control_t *p_list;
  u16 org_group_num, group_num, curn_pos, curn_group;
  u8 curn_mode, org_mode;

  if(!is_open_list)
  {
    org_group_num = sys_status_get_all_group_num();
    org_mode = sys_status_get_curn_prog_mode();

    plist_save_data();

    group_num = sys_status_get_all_group_num();
    curn_mode = sys_status_get_curn_prog_mode();

    if((0 == group_num)
      || (curn_mode != org_mode))
    {
      close_prog_list(0, 0);
      is_menu_closed = TRUE;
      return SUCCESS;
    }
    else if(org_group_num != group_num)
    {
      plist_group_change_group(p_cbox);
    }

    curn_pos = cbox_dync_get_focus(p_cbox);

    curn_group = sys_status_get_group_by_pos(curn_pos);

    sys_status_set_curn_group(curn_group);

    sys_status_save();

    p_list = ctrl_get_child_by_id(ctrl_get_parent(p_cbox), IDC_PLCONT_LIST);

    plist_list_change_group(p_list);

    on_plist_state_process(p_list, MSG_SAT_SELECT, 0, 0);
  }

  is_open_list = FALSE;

  return SUCCESS;
#else
  control_t *p_list;
  u16 org_group_num, group_num, curn_pos, curn_group;

  if(!is_open_list)
  {
    org_group_num = sys_status_get_all_group_num();

    if(plist_save_data())
    {
      group_num = sys_status_get_all_group_num();

      if(org_group_num != group_num)
      {
        plist_group_change_group(p_cbox);
      }

      curn_pos = cbox_dync_get_focus(p_cbox);
      /*
       * curn_pos is logic position, curn_group should be convert to physical position.
       */
      curn_group = sys_status_get_group_by_pos(curn_pos);

      sys_status_set_curn_group(curn_group);

      sys_status_save();

      p_list = ctrl_get_child_by_id(ctrl_get_parent(p_cbox), IDC_PLCONT_LIST);

      on_plist_state_process(p_list, MSG_SAT_SELECT, 0, 0);

      plist_list_change_group(p_list);
    }
    else
    {
      close_prog_list(0, 0);
      is_menu_closed = TRUE;
      return SUCCESS;
    }
  }

  is_open_list = FALSE;

  return SUCCESS;
#endif
}

static RET_CODE on_select_cbox(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cbox;
  u16 old_cnt;

  old_cnt = sys_status_get_all_group_num();

  if(plist_save_data())
  {
    plist_reset_list(p_list, old_cnt);
    ctrl_paint_ctrl(p_list, TRUE);
  }

  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PLCONT_GROUP);

  return ctrl_process_msg(p_cbox, msg, 0, 0);
}


static RET_CODE on_plist_list_change_group(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
#if 0
  control_t *p_cbox;
  u16 group_num, new_group, org_group;
  u8 org_mode, curn_mode;

  org_mode = sys_status_get_curn_prog_mode();

  plist_save_data();

  curn_mode = sys_status_get_curn_prog_mode();

  group_num = sys_status_get_all_group_num();

  if((0 == group_num)
    || (curn_mode != org_mode))
  {
    close_prog_list(0, 0);
    return SUCCESS;
  }

  org_group = sys_status_get_curn_group();

  //shift group
  sys_status_shift_curn_group((msg == MSG_FOCUS_LEFT) ? (-1) : (1));

  sys_status_save();

  new_group = sys_status_get_curn_group();

  if(new_group != org_group)
  {
    p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PLCONT_GROUP);

    plist_group_change_group(p_cbox);
  }

  on_plist_state_process(p_list, msg, 0, 0);

  return SUCCESS;
#else
  control_t *p_cbox;
  u16 new_group, org_group;

  ui_set_chk_pwd(INVALIDID, FALSE); //fix bug 16244

  org_group = sys_status_get_curn_group();
  if(plist_save_data())
  {
    new_group = sys_status_get_curn_group();

    //shift group
    if(org_group == new_group)
    {
      sys_status_shift_curn_group((msg == MSG_FOCUS_LEFT) ? (-1) : (1));

      sys_status_save();

      new_group = sys_status_get_curn_group();
    }

    on_plist_state_process(p_list, msg, 0, 0);

    if(new_group != org_group)
    {
      p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PLCONT_GROUP);

      plist_group_change_group(p_cbox);
    }

    plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);
    // ctrl_paint_ctrl(p_list, TRUE);
    // plist_play_pg(list_get_focus_pos(p_list));

    return SUCCESS;
  }
  else
  {
    close_prog_list(0, 0);
    return SUCCESS;
  }
#endif
}


static RET_CODE on_plist_list_change_channel(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_menu = NULL;

  p_menu = ctrl_get_parent(ctrl_get_parent(p_list));

  if((ms == MS_MOVE)
    && (plist_get_move_state()))                   //move state, move the pg in list.
  {
    plist_move_list_item(p_list, msg);
  }
  else
  {
    ret = list_class_proc(p_list, msg, para1, para2);

    // update curn_prog info
    plist_set_brief_content(p_menu, TRUE);
    update_signal();
  }

  on_plist_state_process(p_list, msg, 0, 0);


  return ret;
}


static RET_CODE on_plist_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 old_focus, focus;
  u16 total;
  u8 view_id;
  u16 i = 0;
  rename_param_t_v2 re_name;

  view_id = ui_dbase_get_pg_view_id();
  focus = list_get_focus_pos(p_list);
  total = db_dvbs_get_count(view_id);
  old_focus = list_get_the_first_selected(p_list, 0);

  switch(ms)
  {
    case MS_NOR:
    case MS_EDIT_NOR:
      if(list_get_item_status(p_list, focus) != LIST_ITEM_SELECTED)
      {
        list_unselect_item(p_list, old_focus);
        list_draw_item_ext(p_list, old_focus, TRUE);
        list_select_item(p_list, focus);
        list_draw_item_ext(p_list, focus, TRUE);
        ctrl_paint_ctrl(p_list, TRUE);
        plist_play_pg(focus);
      }
      break;
    case MS_FAV:
      ret = manage_open_menu(ROOT_ID_FAV_SET,
                             ROOT_ID_PROG_LIST, focus);
      break;
    case MS_LOCK:
      if(plist_get_pwd_state())
      {
        if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0) == TRUE)
        {
          db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, 0);
          list_set_field_content_by_icon(p_list, focus, 3, RSC_INVALID_ID);
        }
        else
        {
          db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_LCK, DB_DVBS_PARAM_ACTIVE_FLAG);
          list_set_field_content_by_icon(p_list, focus, 3, IM_TV_LOCK); //IM_FAV);
        }
        list_draw_field_ext(p_list, focus, 3, TRUE);
        plist_set_modify_state(TRUE);
      }
      else
      {
        plist_pwd.parent_root = ROOT_ID_PROG_LIST;
        plist_pwd.keymap = plist_pwd_keymap;
        plist_pwd.proc = plist_pwd_proc;
        ui_comm_pwdlg_open(&plist_pwd);
      }
      break;
    case MS_SKIP:
      if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_MARK_SKP, 0) == TRUE)
      {
        db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_SKP, 0);
        list_set_field_content_by_icon(p_list, focus, 4, RSC_INVALID_ID);
      }
      else
      {
        db_dvbs_change_mark_status(view_id, focus, DB_DVBS_MARK_SKP, DB_DVBS_PARAM_ACTIVE_FLAG);
        list_set_field_content_by_icon(p_list, focus, 4, IM_TV_SKIP); //IM_FAV);
      }
      list_draw_field_ext(p_list, focus, 4, TRUE);
      plist_set_modify_state(TRUE);
      break;
    case MS_MOVE:
      if(!plist_get_move_state())
      {
        if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_SEL_FLAG, 0))
        {
          list_set_field_content_by_icon(p_list, focus, 5, RSC_INVALID_ID);
          db_dvbs_change_mark_status(view_id, focus, DB_DVBS_SEL_FLAG, 0);
        }
        else
        {
          list_set_field_content_by_icon(p_list, focus, 5, IM_TV_MOVE);
          db_dvbs_change_mark_status(view_id,
                                     focus, DB_DVBS_SEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
        }
        list_draw_field_ext(p_list, focus, 5, TRUE);
      }
      else if(plist_get_move_state())
      {
        for(i = 0; i < total; i++)
        {
          if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0))
          {
            db_dvbs_change_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0);
          }
        }
        plist_set_move_state(FALSE);
        plist_update(p_list, list_get_valid_pos(p_list), total, 0);
        ctrl_paint_ctrl(p_list, TRUE);
      }
      break;
    case MS_EDIT_EDIT:
      re_name.max_len = DB_DVBS_MAX_NAME_LENGTH;
      re_name.uni_str = (u16 *)list_get_field_content(p_list, focus, 1);

      if(*(re_name.uni_str) == char_asc2uni('$'))
      {
        re_name.uni_str++;
        re_name.uni_str++;
      }

      ret = manage_open_menu(ROOT_ID_RENAME_V2, (u32) & re_name,
                             ROOT_ID_PROG_LIST);
      break;
    case MS_EDIT_DEL:
      if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_DEL_FLAG, 0) == TRUE)
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
      plist_set_modify_state(TRUE);
      break;
    default:
      break;
  }

  on_plist_state_process(p_list, msg, 0, 0);

  return ret;
}


static RET_CODE on_dync_pid_update(control_t *p_ctrl, u16 msg,
                                   u32 para1, u32 para2)
{
  plist_set_brief_content(p_ctrl, TRUE);

  return SUCCESS;
}

static RET_CODE plist_update(control_t *ctrl, u16 start, u16 size, u32 context)
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
      if(have_logic_number())
        sprintf((char *)asc_str, "%.4d ", pg.logical_num);
      else
        sprintf((char *)asc_str, "%.4d ", start + i + 1);

      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str);

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, uni_str); //pg name

      /* MARKS */
      plist_set_list_item_mark(ctrl, (u16)(start + i), pg_id);

      //set selected state.
      if(context == 0xFEFE)
      {
        if(pg_id == sys_status_get_curn_group_curn_prog_id())
        {
          list_select_item(ctrl, start + i);
          ui_set_front_panel_by_num(start + i + 1);
        }
      }
    }
  }
  return SUCCESS;
}


static void plist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, im_value[4];
  BOOL is_select;
  u8 view_id = ui_dbase_get_pg_view_id();

  is_select = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_SEL_FLAG, 0);

  im_value[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_TV_FAV : 0;
  im_value[1] =
    db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0) ? IM_TV_LOCK : 0;
  im_value[2] =
    db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_SKP, FALSE) ? IM_TV_SKIP : 0;


  if((ms == MS_MOVE)
    && plist_get_move_state()
    && (list_get_focus_pos(p_list) == pos))
  {
    im_value[3] = IM_TV_MOVE;
  }
  else if((ms == MS_MOVE)
         && (is_select))
  {
    im_value[3] = IM_TV_MOVE;
  }
  else
  {
    im_value[3] =
      db_dvbs_get_mark_status(view_id, pos, DB_DVBS_DEL_FLAG, 0) ? IM_TV_DEL : 0;
  }

  for(i = 0; i < 4; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), im_value[i]);
  }
}


RET_CODE open_prog_list(u32 para1, u32 para2)
{
  control_t *p_menu, *p_cont;
  control_t *p_list_cont, *p_list, *p_preview;
  //control_t *p_top_line, *p_bottom_line;
  control_t *p_brief, *p_sbar, *p_brief_icon;
  //control_t *p_title_icon;
  control_t *p_group;
  control_t *p_brief_item;
  u8 list_mode, curn_mode;
  u8 view_id, i;
  u8 pg_type = 3;
  u16 curn_group, y;
  u32 context;
  u16 view_type, view_count;
  u16 pg_id, pg_pos;
  u16 group_num;
  rect_t rc_group = {0};

  //global variable initialization.
  is_modified = FALSE;
  is_pwd_chked = FALSE;
  is_move = FALSE;
  is_open_list = TRUE;
  is_menu_closed = FALSE;

  ms = old_ms = MS_NOR;
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

#if 0
  if(list_mode == curn_mode)
  {
    view_id = ui_dbase_get_pg_view_id();
  }
  else
  {
  sys_status_get_group_info(curn_group, &group_type, &pos_in_set, &context);
  switch(group_type)
  {
    case GROUP_T_ALL:
      if(!sys_status_get_group_all_info(list_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;
    case GROUP_T_FAV:
      if(!sys_status_get_fav_group_info(pos_in_set, list_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;
    case GROUP_T_SAT:
      if(!sys_status_get_sat_group_info(pos_in_set, list_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  if(pg_id == INVALIDID)
  {
    //if no radio/tv in this group, check all group.
    if(group_type != GROUP_T_ALL)
    {
      sys_status_get_group_all_info(list_mode, &pg_id);
    }

    if(pg_id == INVALIDID)
    {
      //no radio/tv pg, resume the previous view.
      sys_status_get_view_info(curn_group, curn_mode, &view_type, &context);
      if(view_type != DB_DVBS_INVALID_VIEW)
      {
        view_id = ui_dbase_create_view(view_type, context, NULL);
        ui_dbase_set_pg_view_id(view_id);
      }
      return ERR_FAILURE;
    }
    else
    {
      //find radio/tv pg in group all
      curn_group = 0;
    }
  }

  sys_status_get_view_info(curn_group, list_mode, &view_type,
                           &context);
  view_id = ui_dbase_create_view(view_type, context, NULL);

  sys_status_set_curn_group(curn_group);
  sys_status_set_curn_prog_mode(list_mode);
  sys_status_save();
#endif
  view_count = db_dvbs_get_count(view_id);
  //MT_ASSERT(view_count != 0);

  if(view_count == 0)
    return ERR_FAILURE;

  group_num = sys_status_get_all_group_num();

  sys_status_get_curn_prog_in_group(curn_group, list_mode, &pg_id, &context);
  ui_dbase_set_pg_view_id(view_id);

  pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);

  /*Create Menu*/
  p_cont = ui_comm_root_create(ROOT_ID_PROG_LIST,
                               0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
                               COMM_BG_H, IM_TITLEICON_TV, IDS_TV_CHANNEL_LIST);

  //title icon
  //p_title_icon = ctrl_get_child_by_id(p_cont, IDC_COMM_TITLE_ICON);
  //bmap_set_content_by_id(p_title_icon,
  //is_tv ? IM_TITLEICON_RADIO : IM_TITLEICON_TV);

  //title text
  ui_comm_title_set_content(p_cont, is_tv ? IDS_TV_CHANNEL_LIST : IDS_RADIO_CHANNEL_LIST);

  p_menu = ctrl_create_ctrl(CTRL_CONT, IDC_PLIST_CONT_ID, PLIST_MENU_X,
                            PLIST_MENU_Y, PLIST_MENU_W, PLIST_MENU_H, p_cont, 0);

  ctrl_set_rstyle(p_menu, RSI_PLIST_MENU, RSI_PLIST_MENU, RSI_PLIST_MENU);
  ctrl_set_keymap(p_menu, plist_cont_keymap);
  ctrl_set_proc(p_menu, plist_cont_proc);

  p_list_cont =
    ctrl_create_ctrl(CTRL_CONT, IDC_PLIST_LIST_CONT, PLIST_LIST_CONTX,
                     PLIST_LIST_CONTY, PLIST_LIST_CONTW,
                     PLIST_LIST_CONTH, p_menu,
                     0);
  ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  //group
  p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_PLCONT_GROUP, PLIST_CONT_GROUPX,
                             PLIST_CONT_GROUPY, PLIST_CONT_GROUPW,
                             PLIST_CONT_GROUPH, p_list_cont,
                             0);
  ctrl_set_rstyle(p_group, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  ctrl_set_keymap(p_group, plist_group_keymap);
  ctrl_set_proc(p_group, plist_group_proc);
  cbox_set_font_style(p_group, FSI_PLIST_TITLE, FSI_PLIST_CBOX, FSI_PLIST_CBOX);
  cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
  cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group, TRUE);
  cbox_dync_set_count(p_group, group_num);
  cbox_dync_set_update(p_group, plist_group_update);
  cbox_dync_set_focus(p_group, curn_group);

  ctrl_get_frame(p_group, &rc_group);
  cbox_create_droplist(p_group, PLIST_CBOX_PAGE, PLIST_BAR_W, 0);
  cbox_droplist_set_rstyle(p_group, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  cbox_droplist_set_mid_rect(p_group, 8, 0, (u16)(RECTW(rc_group) - 16 - 16),
                             (u16)(RECTH(rc_group) * PLIST_CBOX_PAGE - 0), 0);
  cbox_droplist_set_item_rstyle(p_group, &plist_item_rstyle);
  cbox_droplist_set_field_attr(p_group, STL_LEFT | STL_VCENTER,
                               (u16)(RECTW(rc_group) - 16 - 16 - 16 - 8), 16, 0);
  //cbox_droplist_set_field_rect_style(p_group, &plist_field_rstyle_left);
  cbox_droplist_set_field_font_style(p_group, &plist_field_fstyle);

  cbox_droplist_set_sbar_rstyle(p_group, RSI_PLIST_SBAR, RSI_PLIST_SBAR, RSI_PLIST_SBAR);
  cbox_droplist_set_sbar_mid_rstyle(p_group,
                                    RSI_PLIST_SBAR_MID,
                                    RSI_PLIST_SBAR_MID,
                                    RSI_PLIST_SBAR_MID);

  //prog list
  p_list =
    ctrl_create_ctrl(CTRL_LIST, IDC_PLCONT_LIST, PLIST_LIST_X, PLIST_LIST_Y,
                     PLIST_LIST_W, PLIST_LIST_H, p_list_cont,
                     0);
  ctrl_set_rstyle(p_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_list, plist_list_keymap);
  ctrl_set_proc(p_list, plist_list_proc);
  ctrl_set_mrect(p_list, PLIST_LIST_MIDL, PLIST_LIST_MIDT,
                 PLIST_LIST_MIDL + PLIST_LIST_MIDW, PLIST_LIST_MIDT + PLIST_LIST_MIDH);
  list_set_item_interval(p_list, PLIST_LIST_VGAP);
  list_set_item_rstyle(p_list, &plist_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, view_count, PLIST_LIST_PAGE);
  list_set_field_count(p_list, PLIST_LIST_FIELD, PLIST_LIST_PAGE);
  list_set_focus_pos(p_list, pg_pos);
  list_select_item(p_list, pg_pos);
  list_set_update(p_list, plist_update, 0);
  //list_enable_page_mode(p_list, FALSE);

  for(i = 0; i < PLIST_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(plist_attr[i].attr),
                        (u16)(plist_attr[i].width),
                        (u16)(plist_attr[i].left), (u8)(plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, plist_attr[i].fstyle);
  }

  //scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_PLIST_BAR, PLIST_BAR_X,
                            PLIST_BAR_Y, PLIST_BAR_W, PLIST_BAR_H, p_menu, 0);
  ctrl_set_rstyle(p_sbar, RSI_PLIST_SBAR, RSI_PLIST_SBAR, RSI_PLIST_SBAR);
  //ctrl_set_rstyle(p_sbar, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_PLIST_SBAR_MID, RSI_PLIST_SBAR_MID,
                     RSI_PLIST_SBAR_MID);
 // ctrl_set_mrect(p_sbar, 0, 12, PLIST_BAR_W, PLIST_BAR_H - 12);
  list_set_scrollbar(p_list, p_sbar);

  plist_update(p_list, list_get_valid_pos(p_list), PLIST_LIST_PAGE, 0);

  //preview
  p_preview =
    ctrl_create_ctrl(CTRL_CONT, IDC_PLIST_PREV, PLIST_PREV_X, PLIST_PREV_Y,
                     PLIST_PREV_W, PLIST_PREV_H, p_menu,
                     0);
  ctrl_set_rstyle(p_preview, RSI_PLIST_PREV, RSI_PLIST_PREV, RSI_PLIST_PREV);

  //brief icon
  p_brief_icon =
    ctrl_create_ctrl(CTRL_BMAP, IDC_PLIST_BRIEF_ICON, PLIST_BRIEF_ICON_X, PLIST_BRIEF_ICON_Y,
                     PLIST_BRIEF_ICON_W, PLIST_BRIEF_ICON_H, p_menu,
                     0);
  bmap_set_content_by_id(p_brief_icon, IM_TITLEICON_TV);

  p_brief =
    ctrl_create_ctrl(CTRL_CONT, IDC_PLIST_BRIEF, PLIST_BRIEF_X, PLIST_BRIEF_Y,
                     PLIST_BRIEF_W, PLIST_BRIEF_H, p_menu,
                     0);
  ctrl_set_rstyle(p_brief, RSI_PLIST_BRIEF, RSI_PLIST_BRIEF, RSI_PLIST_BRIEF);

  y = PLIST_BRIEF_ITEMY;

  for(i = 0; i < 4; i++)  //pg/tp/pid/ca
  {
    //prog name & sat name should offset 6 pixel from the boarder.
    if(i < 1)
    {
      p_brief_item = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_PLBRIEF_PG + i),
                                      PLIST_BRIEF_ITEMAX, y, PLIST_BRIEF_ITEMAW,
                                      PLIST_BRIEF_ITEMH, p_brief, 0);
    }
    else
    {
      p_brief_item = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_PLBRIEF_PG + i),
                                      PLIST_BRIEF_ITEMBX, y, PLIST_BRIEF_ITEMBW,
                                      PLIST_BRIEF_ITEMH, p_brief, 0);
    }

    //ctrl_set_rstyle(p_brief_item, RSI_PLIST_BRIEF_ITEM, RSI_PLIST_BRIEF_ITEM,
    // RSI_PLIST_BRIEF_ITEM);
    ctrl_set_rstyle(p_brief_item, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_brief_item, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_brief_item, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_brief_item, FSI_PLIST_BRIEF, FSI_PLIST_BRIEF,
                        FSI_PLIST_BRIEF);
    y += PLIST_BRIEF_ITEM_GAP + PLIST_BRIEF_ITEMH;
  }

  plist_set_brief_content(p_menu, FALSE);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ui_comm_help_create2(&prog_list_help_data, p_cont, FALSE);

  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  plist_set_modify_state(FALSE);
  plist_set_pwd_state(FALSE);

  ui_play_prog(pg_id, FALSE);
  //ui_enable_video_display(TRUE);

  return SUCCESS;
}


static RET_CODE close_prog_list(u32 para1, u32 para2)
{
  return manage_close_menu(ROOT_ID_PROG_LIST, 0, 0);
}

BEGIN_KEYMAP(plist_cont_keymap, NULL)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
ON_EVENT(V_KEY_BACK, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_1, MSG_NUM_PLAY)
ON_EVENT(V_KEY_2, MSG_NUM_PLAY)
ON_EVENT(V_KEY_3, MSG_NUM_PLAY)
ON_EVENT(V_KEY_4, MSG_NUM_PLAY)
ON_EVENT(V_KEY_5, MSG_NUM_PLAY)
ON_EVENT(V_KEY_6, MSG_NUM_PLAY)
ON_EVENT(V_KEY_7, MSG_NUM_PLAY)
ON_EVENT(V_KEY_8, MSG_NUM_PLAY)
ON_EVENT(V_KEY_9, MSG_NUM_PLAY)
ON_EVENT(V_KEY_LEFT, MSG_TVRADIO)
ON_EVENT(V_KEY_RIGHT, MSG_TVRADIO)
ON_EVENT(V_KEY_TVRADIO, MSG_TVRADIO)
END_KEYMAP(plist_cont_keymap, NULL)

BEGIN_MSGPROC(plist_cont_proc, cont_class_proc)
ON_COMMAND(MSG_EXIT, on_plist_exit)
ON_COMMAND(MSG_EXIT_ALL, on_plist_exit_all)
ON_COMMAND(MSG_SIGNAL_LOCK, on_signal_process)
ON_COMMAND(MSG_SIGNAL_UNLOCK, on_signal_process)
ON_COMMAND(MSG_DYNC_PID_UPDATE, on_dync_pid_update)
ON_COMMAND(MSG_SAVE, on_plist_save)
//ON_COMMAND(MSG_FAV_UPDATE, on_plist_fav_update)
//ON_COMMAND(MSG_SORT_UPDATE, on_plist_sort_update)
//ON_COMMAND(MSG_RENAME_UPDATE, on_plist_rename_update)
//ON_COMMAND(MSG_RENAME_CHECK, on_plist_rename_check)
ON_COMMAND(MSG_NUM_PLAY, on_plist_num_play)
ON_COMMAND(MSG_NUM_SELECT, on_plist_num_select)
ON_COMMAND(MSG_TVRADIO, on_plist_tvradio)
ON_COMMAND(MSG_DESTROY, on_plist_destory)
END_MSGPROC(plist_cont_proc, cont_class_proc)

BEGIN_KEYMAP(plist_group_keymap, NULL)
ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
ON_EVENT(V_KEY_BACK, MSG_CANCEL)
ON_EVENT(V_KEY_MENU, MSG_CANCEL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_OK, MSG_SELECT)
//ON_EVENT(V_KEY_REPEAT, MSG_SAT_LIST) //H
//ON_EVENT(V_KEY_SAT, MSG_SAT_LIST)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(plist_group_keymap, NULL)

BEGIN_MSGPROC(plist_group_proc, cbox_class_proc)
//ON_COMMAND(MSG_OPEN_LIST, on_plist_group_open_list)
ON_COMMAND(MSG_CLOSE_LIST, on_plist_group_close_list)
ON_COMMAND(MSG_CHANGED, on_plist_group_change_group)
//ON_COMMAND(MSG_SAT_LIST, on_sat_list_recall)
ON_COMMAND(MSG_FOCUS_LEFT, on_plist_list_change_group)
ON_COMMAND(MSG_FOCUS_RIGHT, on_plist_list_change_group)
END_MSGPROC(plist_group_proc, cbox_class_proc)

BEGIN_KEYMAP(plist_list_keymap, NULL)
ON_EVENT(V_KEY_SAT, MSG_SELECT)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
//ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)
//ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
//ON_EVENT(V_KEY_FAV, MSG_GRAY)
//ON_EVENT(V_KEY_RED, MSG_RED)
//ON_EVENT(V_KEY_GREEN, MSG_GREEN)
//ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
//ON_EVENT(V_KEY_BLUE, MSG_BLUE)
ON_EVENT(V_KEY_OK, MSG_YES)
//ON_EVENT(V_KEY_0, MSG_OPEN_BC)
ON_EVENT(V_KEY_INFO, MSG_INFO)
//ON_EVENT(V_KEY_REPEAT, MSG_SAT_LIST) //H
//ON_EVENT(V_KEY_SAT, MSG_SAT_LIST)
END_KEYMAP(plist_list_keymap, NULL)

BEGIN_MSGPROC(plist_list_proc, list_class_proc)
ON_COMMAND(MSG_SELECT, on_select_cbox)
//ON_COMMAND(MSG_FOCUS_LEFT, on_plist_list_change_group)
//ON_COMMAND(MSG_FOCUS_RIGHT, on_plist_list_change_group)
ON_COMMAND(MSG_FOCUS_UP, on_plist_list_change_channel)
ON_COMMAND(MSG_FOCUS_DOWN, on_plist_list_change_channel)
ON_COMMAND(MSG_PAGE_UP, on_plist_list_change_channel)
ON_COMMAND(MSG_PAGE_DOWN, on_plist_list_change_channel)
ON_COMMAND(MSG_YES, on_plist_ok)
ON_COMMAND(MSG_CORRECT_PWD, on_plist_correct_pwd)
//ON_COMMAND(MSG_GRAY, on_plist_state_process)
//ON_COMMAND(MSG_GREEN, on_plist_state_process)
//ON_COMMAND(MSG_RED, on_plist_state_process)
//ON_COMMAND(MSG_YELLOW, on_plist_state_process)
//ON_COMMAND(MSG_BLUE, on_plist_state_process)
//ON_COMMAND(MSG_INFO, on_plist_start_move)
//ON_COMMAND(MSG_SAT_LIST, on_sat_list_recall)
END_MSGPROC(plist_list_proc, list_class_proc)

BEGIN_KEYMAP(plist_pwd_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
//ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)
//ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
ON_EVENT(V_KEY_BACK, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(plist_pwd_keymap, NULL)

BEGIN_MSGPROC(plist_pwd_proc, cont_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_pwd_close)
ON_COMMAND(MSG_FOCUS_DOWN, on_pwd_close)
ON_COMMAND(MSG_PAGE_UP, on_pwd_close)
ON_COMMAND(MSG_PAGE_DOWN, on_pwd_close)
ON_COMMAND(MSG_EXIT, on_pwd_close)
ON_COMMAND(MSG_CORRECT_PWD, on_pwd_correct)
END_MSGPROC(plist_pwd_proc, cont_class_proc)

