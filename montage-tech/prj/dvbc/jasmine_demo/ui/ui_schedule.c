/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"
#include "ui_schedule.h"
#include "ui_timer.h"

#define IDC_SCHEDULE_CONT_ID IDC_COMM_ROOT_CONT

enum schedule_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_GREEN,
  MSG_BLUE,
  MSG_YELLOW
};


enum focus_style
{
  FOCUS_NONE = 0,
  FOCUS_FIRST,
  FOCUS_LAST,
  FOCUS_NEXT,
  FOCUS_PREV,
};

enum schedule_menu_ctrl_id
{
  IDC_SCHEDULE_ICON = 1,
  IDC_SCHEDULE_TOP_LINE,
  IDC_SCHEDULE_BOTTOM_LINE,
  IDC_SCHEDULE_PREVIEW,
  IDC_SCHEDULE_DETAIL,
  IDC_SCHEDULE_TIME,
  IDC_SCHEDULE_HELP,
};

enum schedule_list_cont_id
{
  IDC_SCHEDULE_LIST_PG = 1,
  IDC_SCHEDULE_LIST_DATE,
  IDC_SCHEDULE_LIST_EVT,
};

enum schedule_list_detail_id
{
  IDC_SCHEDULE_DETAIL_TTL = 1,
  IDC_SCHEDULE_DETAIL_PG,
  IDC_SCHEDULE_DETAIL_SAT,
  IDC_SCHEDULE_DETAIL_EVTNAME,
  IDC_SCHEDULE_DETAIL_EVTTIME,
  IDC_SCHEDULE_DETAIL_SHORT_EVT,
};

book_pg_t book_node;

static RET_CODE schedule_list_pg_update(control_t* p_list, u16 start, u16 size, u32 context);

u16 schedule_cont_keymap(u16 key);
RET_CODE schedule_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

u16 schedule_item_keymap(u16 key);
RET_CODE schedule_item_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 schedule_list_pg_keymap(u16 key);
RET_CODE schedule_list_pg_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 schedule_list_date_keymap(u16 key);
RET_CODE schedule_list_date_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 schedule_list_evt_keymap(u16 key);
RET_CODE schedule_list_evt_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);


u16 schedule_step_keymap(u16 key);
RET_CODE schedule_step_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);
#if 0
static void schedule_play_pg(u16 focus)
{
  u16 rid;

  if ((rid =
         db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                    focus)) != INVALIDID)
  {
    ui_play_prog(rid, FALSE);
  }
}
#endif
static void schedule_set_detail(control_t *p_detail, BOOL is_paint)
{
  control_t *p_pg, *p_evtname, *p_evttime, *p_short;
  dvbs_prog_node_t pg;
  evt_node_t *p_evt_node;
  utc_time_t startime = {0};
  utc_time_t endtime = {0};
  u16 uni_str1[32];
  u16 uni_str2[32];
  u8 asc_str[32];
  u16 pg_id,curn_group,pg_pos;
  u8 curn_mode;
  u32 group_context;
  u8 view_id;

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &group_context);

  view_id = ui_dbase_get_pg_view_id();
  pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  
  db_dvbs_get_pg_by_id(pg_id, &pg);

  p_pg = ctrl_get_child_by_id(p_detail, IDC_SCHEDULE_DETAIL_PG);
  p_evtname = ctrl_get_child_by_id(p_detail, IDC_SCHEDULE_DETAIL_EVTNAME);
  p_evttime = ctrl_get_child_by_id(p_detail, IDC_SCHEDULE_DETAIL_EVTTIME);
  p_short = ctrl_get_child_by_id(p_detail, IDC_SCHEDULE_DETAIL_SHORT_EVT);

  //set pg name
  sprintf(asc_str, "%.4d ", pg_pos + 1);
  str_asc2uni(asc_str, uni_str1);
  ui_dbase_get_full_prog_name(&pg, uni_str2, 31);
  uni_strncpy(&uni_str1[uni_strlen(uni_str1)], uni_str2, 31);
 
  text_set_content_by_unistr(p_pg, uni_str1);

  //p_evt_node = schedule_get_curn_node(p_list_cont);
    
  //if(p_evt_node != NULL)
  if(FALSE)
  {
    //set event name 
    text_set_content_by_unistr(p_evtname, p_evt_node->event_name);

    time_to_local(&p_evt_node->start_time, &startime);

    memcpy(&endtime, &startime, sizeof(utc_time_t));
    time_add(&endtime, &(p_evt_node->drt_time));

    //set event time    
    sprintf(asc_str, "%.2d:%.2d-%.2d:%.2d", 
    startime.hour, startime.minute, endtime.hour, endtime.minute);
    text_set_content_by_ascstr(p_evttime, asc_str);

    if(p_evt_node->p_sht_txt != NULL)
    {
      text_set_content_by_unistr(p_short, p_evt_node->p_sht_txt);
    }
    else
    {
      text_set_content_by_ascstr(p_short, " ");
    }
  }
  else
  {
    //no event, set as null
    text_set_content_by_ascstr(p_evtname, " ");
    text_set_content_by_ascstr(p_evttime, " ");
    text_set_content_by_ascstr(p_short, " ");
  }


  
  if(is_paint)
  {
    ctrl_paint_ctrl(p_pg, TRUE);
    ctrl_paint_ctrl(p_evtname, TRUE);
    ctrl_paint_ctrl(p_evttime, TRUE);
    ctrl_paint_ctrl(p_short, TRUE);
  }  
}


static RET_CODE schedule_list_pg_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{
  u8 asc_str[32];
  u16 i;
  u16 uni_str[32];
  u16 pos, cnt = list_get_count(p_list);
  book_pg_t node;
  dvbs_prog_node_t pg;
  u16 timer_type [4] = {IDS_OFF, IDS_ONCE, IDS_DAILY, IDS_WEEKLY};
  utc_time_t tmp_time;

  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      //no.
      list_set_field_content_by_dec(p_list, pos, 0, (u16)(pos + 1 ));

      sys_status_get_book_node((u8)pos, &node);
      OS_PRINTF("node.book_mode[%d], node.pgid[%d]\n", node.book_mode, node.pgid);

      //timer type
      list_set_field_content_by_strid(p_list, pos, 1, timer_type[node.book_mode]);

      //service name
      if(node.book_mode)
      {
        db_dvbs_get_pg_by_id(node.pgid, &pg);
        ui_dbase_get_full_prog_name(&pg, uni_str, 31);
        list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, uni_str);
      }
      else
      {
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, " "); 
      }

      //time
      if(node.book_mode)
      {
        //tmp_time.value = node.start_time.value;
        memcpy(&tmp_time, &(node.start_time), sizeof(utc_time_t));

        time_add(&tmp_time, &(node.drt_time));

        sprintf(asc_str, "%.2d/%.2d/%.4d %.2d:%.2d-%.2d:%.2d", 
          node.start_time.day, node.start_time.month, (node.start_time.year),
          node.start_time.hour, node.start_time.minute, tmp_time.hour, tmp_time.minute);

        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 3, asc_str); 	
      }
      else
      {
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 3, " "); 	
      }
    }
  }
  return SUCCESS;
}





RET_CODE open_schedule(u32 para1, u32 para2)
{
  control_t *p_root;
  control_t *p_top_line, *p_bottom_line;
  control_t *p_detail, *p_preview;
  control_t *p_pgname, *p_short;
  control_t *p_list_pg;
  control_t *p_help;
  control_t *p_evtname, *p_evttime;
  u16 i, curn_group, curn_mode, pg_id;
  u32 group_context;
  utc_time_t s_time = {0};
  list_xstyle_t schedule_list_item_rstyle =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_ITEM_1_HL,
    RSI_ITEM_1_SH,
    RSI_ITEM_1_HL,   
  };

  list_xstyle_t schedule_list_field_fstyle =
  {
    FSI_GRAY,
    FSI_WHITE,
    FSI_BLACK,
    FSI_BLACK,
    FSI_BLACK,
  };

  list_xstyle_t schedule_list_field_rstyle =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };
  
  list_field_attr_t schedule_pg_attr[SCHEDULE_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_DEC,
      40, 0, 0, &schedule_list_field_rstyle, &schedule_list_field_fstyle },//no.
    { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
      80, 40, 0, &schedule_list_field_rstyle, &schedule_list_field_fstyle },//mode
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      180, 120, 0, &schedule_list_field_rstyle, &schedule_list_field_fstyle },//name
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      250, 300, 0, &schedule_list_field_rstyle, &schedule_list_field_fstyle },//time   
  };

  dvbs_prog_node_t pg;
  epg_filter_t item_info;
  
  //initialize global variables.
  time_get(&s_time, FALSE);

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &group_context);
  db_dvbs_get_pg_by_id(pg_id, &pg);

  item_info.service_id      = (u16)pg.s_id;
  item_info.stream_id       = (u16)pg.ts_id;
  item_info.orig_network_id = (u16)pg.orig_net_id;
  item_info.cont_level = 0;
  time_to_gmt(&s_time, &(item_info.start_time));
  s_time.hour = 23;
  s_time.minute = 59;
  s_time.second = 59;
  time_to_gmt(&s_time, &(item_info.end_time));
  
  //Create
  p_root = ui_comm_root_create(ROOT_ID_SCHEDULE, 
    (u8)para1, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, RSC_INVALID_ID, IDS_TIMER_SET);
  if(p_root == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_root, schedule_cont_keymap);
  ctrl_set_proc(p_root, schedule_cont_proc);

  //top line
  p_top_line = ctrl_create_ctrl(CTRL_TEXT, IDC_SCHEDULE_TOP_LINE, 
    SCHEDULE_TOP_LINE_X, SCHEDULE_TOP_LINE_Y, SCHEDULE_TOP_LINE_W, SCHEDULE_TOP_LINE_H, 
    p_root, 0);
  ctrl_set_rstyle(p_top_line, RSI_IGNORE, 
    RSI_IGNORE, RSI_IGNORE);

  //bottom line
  p_bottom_line = ctrl_create_ctrl(CTRL_TEXT, IDC_SCHEDULE_BOTTOM_LINE, 
    SCHEDULE_BOTTOM_LINE_X, SCHEDULE_BOTTOM_LINE_Y, SCHEDULE_BOTTOM_LINE_W, SCHEDULE_BOTTOM_LINE_H, 
    p_root, 0);
  ctrl_set_rstyle(p_bottom_line, RSI_IGNORE, 
    RSI_IGNORE, RSI_IGNORE);

  //detail information
  p_detail = ctrl_create_ctrl(CTRL_CONT, IDC_SCHEDULE_DETAIL, SCHEDULE_DETAIL_X, 
    SCHEDULE_DETAIL_Y, SCHEDULE_DETAIL_W, SCHEDULE_DETAIL_H, p_root, 0);
  ctrl_set_rstyle(p_detail, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL);

  p_pgname = ctrl_create_ctrl(CTRL_TEXT, IDC_SCHEDULE_DETAIL_PG, 
    SCHEDULE_DETAIL_PGX, SCHEDULE_DETAIL_PGY, SCHEDULE_DETAIL_PGW, SCHEDULE_DETAIL_PGH, p_detail, 0);
  ctrl_set_rstyle(p_pgname, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL);
  text_set_font_style(p_pgname, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX);
  text_set_align_type(p_pgname, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_pgname, TEXT_STRTYPE_UNICODE);

  p_evtname = ctrl_create_ctrl(CTRL_TEXT, IDC_SCHEDULE_DETAIL_EVTNAME, 
    SCHEDULE_DETAIL_EVTNX, SCHEDULE_DETAIL_EVTNY, SCHEDULE_DETAIL_EVTNW, SCHEDULE_DETAIL_EVTNH, p_detail, 0);
  ctrl_set_rstyle(p_evtname, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL);
  text_set_font_style(p_evtname, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX);
  text_set_align_type(p_evtname, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_evtname, TEXT_STRTYPE_UNICODE);

  p_evttime = ctrl_create_ctrl(CTRL_TEXT, IDC_SCHEDULE_DETAIL_EVTTIME, 
    SCHEDULE_DETAIL_EVTTX, SCHEDULE_DETAIL_EVTTY, SCHEDULE_DETAIL_EVTTW, SCHEDULE_DETAIL_EVTTH, p_detail, 0);
  ctrl_set_rstyle(p_evttime, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL);
  text_set_font_style(p_evttime, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX);
  text_set_align_type(p_evttime, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_evttime, TEXT_STRTYPE_UNICODE);
  
  p_short = ctrl_create_ctrl(CTRL_TEXT, IDC_SCHEDULE_DETAIL_SHORT_EVT, 
    SCHEDULE_DETAIL_SHORTX, SCHEDULE_DETAIL_SHORTY, SCHEDULE_DETAIL_SHORTW, SCHEDULE_DETAIL_SHORTH, p_detail, 0);
  ctrl_set_rstyle(p_short, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL, RSI_SCHEDULE_DETAIL);
  text_set_font_style(p_short, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX, FSI_SCHEDULE_INFO_MBOX);
  text_set_align_type(p_short, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_short, TEXT_STRTYPE_UNICODE);


  //preview window
  p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_SCHEDULE_PREVIEW, SCHEDULE_PREV_X, 
    SCHEDULE_PREV_Y, SCHEDULE_PREV_W, SCHEDULE_PREV_H, p_root, 0);
  ctrl_set_rstyle(p_preview, RSI_SCHEDULE_PREV, RSI_SCHEDULE_PREV, RSI_SCHEDULE_PREV);  

  //schedule list
  p_list_pg = ctrl_create_ctrl(CTRL_LIST, IDC_SCHEDULE_LIST_PG, SCHEDULE_LIST_PGX, 
    SCHEDULE_LIST_PGY, SCHEDULE_LIST_PGW, SCHEDULE_LIST_PGH, p_root, 0);
  ctrl_set_rstyle(p_list_pg, RSI_SCHEDULE_LIST, RSI_SCHEDULE_LIST, RSI_SCHEDULE_LIST);
  ctrl_set_keymap(p_list_pg, schedule_list_pg_keymap);
  ctrl_set_proc(p_list_pg, schedule_list_pg_proc);
  ctrl_set_mrect(p_list_pg, 4, 4, (SCHEDULE_LIST_PGW - 4), (SCHEDULE_LIST_PGH - 4));
  list_set_item_interval(p_list_pg, SCHEDULE_VGAP);
  list_set_item_rstyle(p_list_pg, &schedule_list_item_rstyle);
  list_enable_select_mode(p_list_pg, TRUE);
  list_set_select_mode(p_list_pg, LIST_SINGLE_SELECT);
  list_set_count(p_list_pg, MAX_BOOK_PG, SCHEDULE_LIST_PAGE);
  list_set_field_count(p_list_pg, SCHEDULE_LIST_FIELD, SCHEDULE_LIST_PAGE);
  list_set_focus_pos(p_list_pg, 0);
  //list_select_item(p_list_pg, pg_pos);
  list_set_update(p_list_pg, schedule_list_pg_update, 0);
 
  for (i = 0; i < SCHEDULE_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list_pg, (u8)i, (u32)(schedule_pg_attr[i].attr), (u16)(schedule_pg_attr[i].width),
                        (u16)(schedule_pg_attr[i].left), (u8)(schedule_pg_attr[i].top));
    list_set_field_rect_style(p_list_pg, (u8)i, schedule_pg_attr[i].rstyle);
    list_set_field_font_style(p_list_pg, (u8)i, schedule_pg_attr[i].fstyle);
  }
  schedule_list_pg_update(p_list_pg, list_get_valid_pos(p_list_pg), SCHEDULE_LIST_PAGE, 0);

  //help
  p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_SCHEDULE_HELP, 
    SCHEDULE_HELP_X, SCHEDULE_HELP_Y, 
    SCHEDULE_HELP_W, SCHEDULE_HELP_H, p_root, 0);
  ctrl_set_rstyle(p_help, RSI_SCHEDULE_LIST_CONT, RSI_SCHEDULE_LIST_CONT, RSI_SCHEDULE_LIST_CONT);
  ctrl_set_mrect(p_help, 20, 0, (SCHEDULE_HELP_W - 40), SCHEDULE_HELP_H);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, 4, 4, 1);
  mbox_set_item_interval(p_help, 40, 0);
  mbox_set_item_rstyle(p_help, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_help, 30, 0);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);
  mbox_set_content_by_strid(p_help, 0, IDS_QUICK_BROWSE);
  mbox_set_content_by_strid(p_help, 1, IDS_LIST);
  mbox_set_content_by_strid(p_help, 2, IDS_SCHEDULE);
  mbox_set_content_by_strid(p_help, 3, IDS_FIND);
  mbox_set_content_by_icon(p_help, 0, IM_EPG_COLORBUTTON_RED, IM_EPG_COLORBUTTON_RED);
  mbox_set_content_by_icon(p_help, 1, IM_EPG_COLORBUTTON_GREEN, IM_EPG_COLORBUTTON_GREEN); 
  mbox_set_content_by_icon(p_help, 2, IM_EPG_COLORBUTTON_YELLOW, IM_EPG_COLORBUTTON_YELLOW);
  mbox_set_content_by_icon(p_help, 3, IM_EPG_COLORBUTTON_BLUE, IM_EPG_COLORBUTTON_BLUE);

  ctrl_default_proc(p_list_pg, MSG_GETFOCUS, 0, 0);
  schedule_set_detail(p_detail, FALSE);  
  ctrl_paint_ctrl(ctrl_get_root(p_root), FALSE);
  
  //schedule_play_pg(list_get_focus_pos(p_list_pg));
  
  return SUCCESS;
}

static RET_CODE on_schedule_ready(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  //control_t *p_list_pg;
  //dvbs_prog_node_t pg;
  //u16 pg_focus, pg_id;
  //
  //pg_focus = list_get_focus_pos(p_list_pg);

  //pg_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), pg_focus);
  //db_dvbs_get_pg_by_id(pg_id, &pg);

  //if((para1 == pg.s_id) && (para2 == pg.ts_id))
  //{
  //  OS_PRINTF("~~~ts_id[%d], s_id[%d] is ready...\n", para2, para1);
  //  schedule_set_detail(ctrl_get_child_by_id(p_cont, IDC_SCHEDULE_DETAIL), TRUE);
  //}

  return SUCCESS;
}


static RET_CODE on_schedule_time_update(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  schedule_set_detail(ctrl_get_child_by_id(p_cont, IDC_SCHEDULE_DETAIL), TRUE);  
  return SUCCESS;
}

static RET_CODE on_schedule_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  ui_enable_chk_pwd(TRUE);
  
  return ERR_NOFEATURE;
}

static RET_CODE on_schedule_update(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list;

  p_list = ctrl_get_child_by_id(p_cont, IDC_SCHEDULE_LIST_PG);
  MT_ASSERT(p_list != NULL);

  schedule_list_pg_update(p_list, list_get_valid_pos(p_list), SCHEDULE_LIST_PAGE, 0);
  //ctrl_paint_ctrl(p_list, TRUE);
  list_draw_item_ext(p_list, list_get_focus_pos(p_list), TRUE);

  return SUCCESS;
}

static RET_CODE on_schedule_list_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  u16 focus = list_get_focus_pos(p_list);
  return manage_open_menu(ROOT_ID_TIMER, focus, FROM_SCHEDULE_MENU);
}

static RET_CODE on_schedule_list_green(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = manage_back_to_menu(ROOT_ID_EPG, (u32)ROOT_ID_SCHEDULE, 0);

  return ret;
}

static RET_CODE on_schedule_list_red(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = manage_back_to_menu(ROOT_ID_EPG, (u32)ROOT_ID_SCHEDULE, 0);

  return ret;
}

static RET_CODE on_schedule_list_blue(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = manage_back_to_menu(ROOT_ID_EPG_FIND, (u32)ROOT_ID_SCHEDULE, 0);

  return ret;
}

BEGIN_KEYMAP(schedule_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_EPG, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_ALL)
END_KEYMAP(schedule_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(schedule_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EPG_READY, on_schedule_ready)
  ON_COMMAND(MSG_TIME_UPDATE, on_schedule_time_update)
  ON_COMMAND(MSG_DESTROY, on_schedule_destory)
  ON_COMMAND(MSG_TIMER_UPDATE, on_schedule_update)
END_MSGPROC(schedule_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(schedule_list_pg_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_BLUE, MSG_BLUE)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
END_KEYMAP(schedule_list_pg_keymap, NULL)

BEGIN_MSGPROC(schedule_list_pg_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_schedule_list_select)
  ON_COMMAND(MSG_RED, on_schedule_list_red)
  ON_COMMAND(MSG_GREEN, on_schedule_list_green)
  ON_COMMAND(MSG_BLUE, on_schedule_list_blue)
END_MSGPROC(schedule_list_pg_proc, list_class_proc)

