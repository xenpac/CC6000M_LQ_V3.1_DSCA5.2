/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_small_list_pvr.h"
#include "ui_prog_list.h"

typedef enum
{
  IDC_SMALL_LIST_LIST = 1,
  IDC_SMALL_LIST_SBAR,
  IDC_SMALL_LIST_SAT,
}small_list_ctrl_id_t;


static RET_CODE slist_update(control_t *p_list, u16 start, u16 size, u32 context);
static void slist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id);
static RET_CODE on_slist_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2);

u16 small_list_keymap_pvr(u16 key);
RET_CODE small_list_proc_pvr(control_t *p_list, u16 msg, u32 para1, u32 para2);


static RET_CODE slist_sat_cbox_update(control_t *p_cbox, u16 focus, u16 *p_str,
                                      u16 max_length)
{
  u8             type     = 0;
  u16            pos      = 0;
  u16            group    = 0;
  u32            context  = 0;

  group = sys_status_get_group_by_pos(focus);

  sys_status_get_group_info(group, &type, &pos, &context);

  switch(type)
  {
    case GROUP_T_ALL:
      gui_get_string(IDS_ALL, p_str, 31);
      break;
    case GROUP_T_FAV:
      sys_status_get_fav_name((u8)context, p_str);
      break;
    default:
      MT_ASSERT(0);
  }

  return SUCCESS;
}

static void slist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16       i       = 0;
  u16       img[4]  = {0};
  u8        view_id = ui_dbase_get_pg_view_id();

  img[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_TV_FAV : 0;
  img[1] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0) ? IM_TV_LOCK : 0;
  img[2] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_SKP, FALSE)? IM_TV_SKIP: 0;
  //img[3] = 0;

  for(i = 0; i < 3; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), img[i]);
  }
}

//load data from database, will be modified later
static RET_CODE slist_update(control_t *p_list, u16 start, u16 size,
                             u32 context)
{
  dvbs_prog_node_t   curn_prog    = {0};
  u8                 asc_str[8]   = {0};
  u16                i            =  0 ;
  u16                uni_str[32]  = {0};
  u16                pg_id        =  0 ;
  u16                cnt = list_get_count(p_list);

  for(i = 0; i < size; i++)
  {
    if(i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                         (u16)(i + start));
                                         db_dvbs_get_pg_by_id(pg_id, &curn_prog);
      // NO.
      sprintf((char *)asc_str, "%.4d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0,
                                       asc_str);  //program number in list

      // NAME
      ui_dbase_get_full_prog_name(&curn_prog, uni_str, 31);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str); //pg name
      //$
      list_set_field_content_by_icon(p_list, (u16)(start + i), 5, curn_prog.is_scrambled ? IM_TV_MONEY:0);
      //HD
      list_set_field_content_by_icon(p_list, (u16)(start + i), 6, (curn_prog.service_type == 0x11)?IM_TV_HD:0);
      //set marks
      slist_set_list_item_mark(p_list, (u16)(start + i), pg_id);

      //set selected state.
      if(context == 0xFEFE)
      {
        if(pg_id == sys_status_get_curn_group_curn_prog_id())
        {
          list_select_item(p_list, start + i);
          ui_set_front_panel_by_num(start + i+1);
        }
      }
    }
  }
  return SUCCESS;
}

#if 0
static void play_in_small_list(u16 focus)
{
  u16 rid;

  rid = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), focus);

  if(rid != INVALIDID)
  {
    ui_play_prog(rid, FALSE);
  }
}
#endif


static RET_CODE on_slist_state_process_pvr(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  switch(msg)
  {
    case MSG_EXIT:
    case MSG_EXIT_ALL:
    case MSG_SELECT:
      {
          ui_restore_view();
          manage_close_menu(ROOT_ID_SMALL_LIST_PVR, 0, 0);
          break;
      }

    default:
      {
        break;
      }
  }
  return SUCCESS;
}

static void on_pvr_recbar_save_exittoplay_slist(u32 para1, u32 para2)
{
  fw_destroy_all_mainwin(FALSE);
  ui_play_prog((u16)para1, FALSE);
  manage_open_menu(ROOT_ID_PROG_BAR, para1, para2);
  fw_paint_all_mainwin();
}

static RET_CODE on_slist_pvr_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16              cur_pg_id  = 0;
  u16              next_pg_id = 0;
  dvbs_prog_node_t cur_pg     = {0};
  dvbs_prog_node_t next_pg    = {0};
  u16 focus;

  cur_pg_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(cur_pg_id, &cur_pg);

  focus = list_get_focus_pos(p_list);
  next_pg_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),focus);
  db_dvbs_get_pg_by_id(next_pg_id, &next_pg);

  if(next_pg.tp_id == cur_pg.tp_id)
  {
    if(!memcmp(&next_pg, &cur_pg, sizeof(dvbs_prog_node_t)))
    {
      on_slist_state_process_pvr(p_list, msg, para1, para2);
    }
    else
    {
      ui_play_prog(next_pg_id, FALSE);
    }
    return SUCCESS;
  }
  ui_comm_ask_for_dodlg_open_ex(NULL, IDS_EXIT_RECORD,\
                                  on_pvr_recbar_save_exittoplay_slist, next_pg_id, para2, 0);
  return SUCCESS;
}


static RET_CODE on_slist_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  list_class_proc(p_list, msg, para1, para2);
  return SUCCESS;
}

/*!
 * Create Cont
 */
static control_t * slist_creat_cont(void)
{
  control_t *p_cont = NULL;

  p_cont = fw_create_mainwin(ROOT_ID_SMALL_LIST_PVR,
                             SMALL_LIST_MENU_CONT_X_PVR,
                             SMALL_LIST_MENU_CONT_Y_PVR,
                             SMALL_LIST_MENU_WIDTH_PVR,
                             SMALL_LIST_MENU_HEIGHT_PVR,
                             0,
                             0,
                             OBJ_ATTR_ACTIVE, 0);

  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  return p_cont;
}

/*!
 * Create scrolbar
 */
static control_t * slist_creat_sbar(control_t *p_cont)
{
  control_t *p_sbar = NULL;

  p_sbar = ctrl_create_ctrl(CTRL_SBAR,
                            IDC_SMALL_LIST_SBAR,
                            SMALL_LIST_SBAR_X_PVR,
                            SMALL_LIST_SBAR_Y_PVR,
                            SMALL_LIST_SBAR_W_PVR,
                            SMALL_LIST_SBAR_H_PVR,
                            p_cont,
                            0);
  MT_ASSERT(p_sbar != NULL);
  ctrl_set_rstyle(p_sbar, RSI_SLIST_SBAR_PVR, RSI_SLIST_SBAR_PVR, RSI_SLIST_SBAR_PVR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SLIST_SBAR_MID_PVR, RSI_SLIST_SBAR_MID_PVR, RSI_SLIST_SBAR_MID_PVR);

  return p_sbar;
}

/*!
 * Create list
 */
static void slist_creat_list(control_t *p_cont, control_t *p_sbar)
{
  control_t  *p_list = NULL;
  u16              i = 0;
  u16          total = 0;
  u16     list_focus = 0;
  u16          pg_id = 0;
  u32        context = 0;
  u8         view_id = 0xFF;
  u8      slist_mode = 0;
  u16    slist_group = 0;


  list_xstyle_t slist_item_rstyle =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_ITEM_1_HL,
    RSI_ITEM_1_HL,
    RSI_ITEM_1_HL,
  };

  list_xstyle_t slist_field_fstyle =
  {
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
  };

  list_xstyle_t slist_field_rstyle_left =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };

  list_xstyle_t slist_field_rstyle_mid =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };


  list_field_attr_t slist_attr[SMALL_LIST_FIELD_NUM_PVR] =
  {
    {LISTFIELD_TYPE_UNISTR,
     80, 0, 0, &slist_field_rstyle_left, &slist_field_fstyle},
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
     180, 80, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
    {LISTFIELD_TYPE_ICON,
     28, 260, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
    {LISTFIELD_TYPE_ICON,
     28, 288, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
    {LISTFIELD_TYPE_ICON,
     28, 316, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
    {LISTFIELD_TYPE_ICON,
     28, 344, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
    {LISTFIELD_TYPE_ICON,
     28, 372, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
  };

  slist_mode = sys_status_get_curn_prog_mode();

  view_id = ui_dbase_get_pg_view_id();

  slist_group = sys_status_get_curn_group();

  total = db_dvbs_get_count(view_id);

  sys_status_get_curn_prog_in_group(slist_group, slist_mode, &pg_id, &context);

  if(total > 0)
  {
    list_focus = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  }


  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_SMALL_LIST_LIST,
                            SMALL_LIST_LIST_X_PVR, SMALL_LIST_LIST_Y_PVR,
                            SMALL_LIST_LIST_W_PVR, SMALL_LIST_LIST_H_PVR,
                            p_cont, 0);
  MT_ASSERT(p_list != NULL);

  ctrl_set_rstyle(p_list, RSI_SLIST_LIST_PVR, RSI_SLIST_LIST_PVR, RSI_SLIST_LIST_PVR);
  ctrl_set_keymap(p_list, small_list_keymap_pvr);
  ctrl_set_proc(p_list, small_list_proc_pvr);

  ctrl_set_mrect(p_list, SMALL_LIST_MID_L_PVR, SMALL_LIST_MID_T_PVR,
                 SMALL_LIST_MID_L_PVR + SMALL_LIST_MID_W_PVR, SMALL_LIST_MID_T_PVR+ SMALL_LIST_MID_H_PVR);
  list_set_item_interval(p_list, SMALL_LIST_V_GAP_PVR);
  list_set_item_rstyle(p_list, &slist_item_rstyle);
  list_set_count(p_list, total, SMALL_LIST_PAGE_PVR);
  list_set_field_count(p_list, SMALL_LIST_FIELD_NUM_PVR, SMALL_LIST_PAGE_PVR);
  list_set_focus_pos(p_list, list_focus);
  list_set_update(p_list, slist_update, 0);

  for(i = 0; i < SMALL_LIST_FIELD_NUM_PVR; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(slist_attr[i].attr), (u16)(slist_attr[i].width),
                        (u16)(slist_attr[i].left), (u8)(slist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, slist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, slist_attr[i].fstyle);
  }

  list_set_scrollbar(p_list, p_sbar);

  slist_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE_PVR, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
}

/*!
 * Create sat
 */
static void slist_creat_sat(control_t *p_cont)
{
  control_t  *p_sat = NULL;
  u16    cbox_focus = 0xFFFF;
  u16    pos_in_set = 0;
  u16       all_num = 0;
  u16   slist_group = 0;
  u32       context = 0;
  u8     group_type = 0;

  all_num = sys_status_get_all_group_num();
  slist_group = sys_status_get_curn_group();

  p_sat = ctrl_create_ctrl(CTRL_CBOX, IDC_SMALL_LIST_SAT,
                           SMALL_LIST_SAT_X_PVR, SMALL_LIST_SAT_Y_PVR,
                           SMALL_LIST_SAT_W_PVR, SMALL_LIST_SAT_H_PVR,
                           p_cont, 0);
  MT_ASSERT(p_sat != NULL);

  ctrl_set_rstyle(p_sat, RSI_SLIST_SAT_CBOX_PVR, RSI_SLIST_SAT_CBOX_PVR, RSI_SLIST_SAT_CBOX_PVR);
  cbox_set_font_style(p_sat, FSI_SLIST_TEXT_PVR, FSI_SLIST_BOTTOM_PVR, FSI_SLIST_BOTTOM_PVR);
  cbox_set_work_mode(p_sat, CBOX_WORKMODE_DYNAMIC);
  cbox_set_align_style(p_sat, STL_CENTER | STL_VCENTER);
  cbox_enable_cycle_mode(p_sat, TRUE);
  cbox_dync_set_count(p_sat, all_num);
  cbox_dync_set_update(p_sat, slist_sat_cbox_update);
  sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);
  cbox_focus = sys_status_get_pos_by_group(slist_group);
  cbox_dync_set_focus(p_sat, cbox_focus);
}

RET_CODE open_small_list_pvr(u32 para1, u32 para2)
{
  control_t  *p_cont = NULL;
  control_t  *p_sbar = NULL;

  ui_cache_view();

  //create p_cont
  p_cont = slist_creat_cont();

  //create scroll bar
  p_sbar = slist_creat_sbar(p_cont);

  //create sat
  slist_creat_sat(p_cont);

  //create program list
  slist_creat_list(p_cont, p_sbar);


  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


BEGIN_KEYMAP(small_list_keymap_pvr, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(small_list_keymap_pvr, NULL)

BEGIN_MSGPROC(small_list_proc_pvr, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_slist_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_SELECT, on_slist_pvr_select)
  ON_COMMAND(MSG_EXIT, on_slist_state_process_pvr)
  ON_COMMAND(MSG_EXIT_ALL, on_slist_state_process_pvr)
END_MSGPROC(small_list_proc_pvr, list_class_proc)


