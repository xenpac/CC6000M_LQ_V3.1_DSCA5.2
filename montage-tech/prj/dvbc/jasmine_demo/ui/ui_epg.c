/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_epg.h"
#include "ui_timer.h"
#include "ui_book_list.h"

#define IDC_EPG_CONT_ID IDC_COMM_ROOT_CONT

enum epg_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_GREEN,
  MSG_AREA_CHANGE,
  MSG_BLUE,
  MSG_GROUP,
  MSG_RESET,
  MSG_WEEKDAY_LEFT,
  MSG_WEEKDAY_RIGHT,
  MSG_OPEN_BOOK,
  MSG_BOOK,
  MSG_SHOW_DETAIL,
};


enum focus_style
{
  FOCUS_NONE = 0,
  FOCUS_FIRST,
  FOCUS_LAST,
  FOCUS_NEXT,
  FOCUS_PREV,
};

enum epg_menu_ctrl_id
{
  IDC_EPG_ICON = 1,
  IDC_TV_AND_DETAIL_CONT,
  IDC_EPG_PREVIEW,
  IDC_EPG_PROG_CONT,
  IDC_EPG_TITLE,
  IDC_EPG_TIME,
  IDC_EPG_LIST_CONT,
  IDC_EPG_HELP,
};

enum epg_list_cont_id
{
  IDC_EPG_WEEKDAY = 1,
  IDC_EPG_LIST_EVT,
  IDC_EPG_EVT_SBAR,
};

enum epg_prog_list_id
{
  IDC_EPG_LIST_PG = 1,
  IDC_EPG_LIST_SBAR,
};

enum epg_detail_id
{
  IDC_EPG_DETAIL=1,  
};
static list_xstyle_t epg_list_field_rstyle_left =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
};

list_xstyle_t epg_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
};

list_xstyle_t epg_evt_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

list_xstyle_t epg_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

list_xstyle_t epg_evt_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

list_xstyle_t epg_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static comm_help_data_t2 epg_help_data[] = 
{
  {
    8, 80, {60,140,60,150,60,140,60,140},
    {
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
#ifdef CUSTOMER_HCI
      HELP_RSC_STRID | IDS_BOOK_MANAGE_HK,
#else
      HELP_RSC_STRID | IDS_BOOK_MANAGE,
#endif
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
      HELP_RSC_STRID | IDS_SWITCH_WINDOW,
      HELP_RSC_BMP   | IM_TVRADIO,
      HELP_RSC_STRID | IDS_TV_RADIO,
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_FULL_SCREEN,
    },
  },
  
  {
    8, 80, {60,140,60,150,60,140,60,140},
    {
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
#ifdef CUSTOMER_HCI
      HELP_RSC_STRID | IDS_BOOK_MANAGE_HK,
#else
      HELP_RSC_STRID | IDS_BOOK_MANAGE,
#endif
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
      HELP_RSC_STRID | IDS_SWITCH_WINDOW,
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_BOOK,
      HELP_RSC_BMP   | IM_INFO,
      HELP_RSC_STRID | IDS_INFO,
    },
  },
};


list_field_attr_t epg_pg_attr[EPG_LIST_FIELD] =
{
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   70, 10, 0, &epg_list_field_rstyle, &epg_list_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   200, 80, 0, &epg_list_field_rstyle, &epg_list_field_fstyle},
};

list_field_attr_t epg_evt_attr[EPG_EVT_LIST_FIELD] =
{
  {LISTFIELD_TYPE_ICON | STL_CENTER,
   30, 00, 0, &epg_list_field_rstyle_left, &epg_evt_list_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   120, 30, 0, &epg_list_field_rstyle, &epg_evt_list_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   255, 150, 0, &epg_list_field_rstyle, &epg_evt_list_field_fstyle},
  {LISTFIELD_TYPE_ICON | STL_CENTER,
   25, 405, 0, &epg_list_field_rstyle, &epg_evt_list_field_fstyle},
 };
epg_prog_info_t g_prog_info = {0};
static book_pg_t book_node= {0};
static u16 prog_focus = 0;
static utc_time_t first_time = {0}; 

static u8 g_play_timer_state = 0; /* 0: stopped, 1: running */
static u16 g_play_timer_owner = 0;
static u16 g_play_timer_notify = 0;
static u32 g_play_timer_tmout = 300;
static u16 curn_epg_prog_id = 0;

static RET_CODE epg_list_pg_update(control_t *p_list, u16 start, u16 size, u32 context);
static void epg_deinit_play_timer(void);

u16 epg_cont_keymap(u16 key);
RET_CODE epg_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 epg_list_pg_keymap(u16 key);
RET_CODE epg_list_pg_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

u16 epg_list_evt_keymap(u16 key);
RET_CODE epg_list_evt_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

extern BOOL have_logic_number(void);


static void epg_init_play_timer(u16 owner, u16 notify, u32 tmout)
{
  g_play_timer_state = 0;
  g_play_timer_owner = owner;
  g_play_timer_notify = notify;
  g_play_timer_tmout = tmout;
}

static void epg_deinit_play_timer(void)
{
  if(g_play_timer_state == 1)
  {
    fw_tmr_destroy(g_play_timer_owner, g_play_timer_notify);
	g_play_timer_state = 0;
  }
}

static void epg_play_timer_start(void)
{
  if(g_play_timer_state == 0)
  {
    fw_tmr_create(g_play_timer_owner, g_play_timer_notify, g_play_timer_tmout, FALSE);
    g_play_timer_state = 1;
  }
  else
  {
    fw_tmr_reset(g_play_timer_owner, g_play_timer_notify, g_play_timer_tmout);
  }
}

void epg_play_timer_set_state(u8 state)
{
  g_play_timer_state = state;
}


static RET_CODE on_epg_show_detail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  event_node_t *p_evt_node = NULL;
  u16 total_evt  = 0;
  u16 pg_id = 0;
  u16 pos = 0;
  u16 count = list_get_count(p_list);

  pg_id = sys_status_get_curn_group_curn_prog_id();
  if(count > 0)
  {
    pos = list_get_focus_pos(p_list);
    p_evt_node = mul_epg_get_sch_event(&g_prog_info, &total_evt);
    
    if(pos > 0)
    {
      p_evt_node = mul_epg_get_sch_event_by_pos(&g_prog_info, p_evt_node, pos);
    }
  }
  if (p_evt_node != NULL)
  {
    manage_open_menu(ROOT_ID_EPG_DETAIL, (u32)p_evt_node, pg_id);
  }
  else
  {
    OS_PRINTF("[%s][%d]p_evt_node == NULL!!!,Ignore open ROOT_ID_EPG_DETAIL.g_prog_info.svc_id:%d\n",__FUNCTION__,__LINE__,p_evt_node,g_prog_info.svc_id);
  }

  return SUCCESS;
}

static void epg_weekday_upgrade(control_t *p_mbox_weekday, BOOL is_force)
{
  //u32 old_context = 0;
  //s8 new_focus = 0;
  //u32 item_num = 0;
  u8 i;
  //u16 unistr[64];

  u8  weekday;
  u16 dst_unistr[128];
  #if 0
  u16 tmp_unistr[128];
  u8  ansstr[256];
  #endif
  u16 weekstr[] = 
    {
      IDS_MON,
      IDS_TUS,
      IDS_WED,
      IDS_THU,
      IDS_FRI,
      IDS_SAT,
      IDS_SUN,
    };
  utc_time_t cur_time = {0};
  utc_time_t tmp_time = {0};
  
  time_get(&cur_time, TRUE);

  for(i = 0; i<SEVEN; i++)
  {
    memcpy(&tmp_time, &cur_time, sizeof(utc_time_t));
    time_up(&tmp_time, ONE_DAY_SECONDS*i);
    weekday = date_to_weekday(&tmp_time);
    gui_get_string(weekstr[weekday], dst_unistr, 64);
    mbox_set_content_by_unistr(p_mbox_weekday, i, dst_unistr);
  }

  if(is_force)
  {
    ctrl_paint_ctrl(p_mbox_weekday, is_force);
  }
}

static void epg_event_list_get_time_area(utc_time_t *p_start, utc_time_t *p_end)
{
  utc_time_t g_time = {0};
  //utc_time_t t_time = {0};
  
  time_get(&g_time, FALSE);
  
  memcpy(p_start, &g_time, sizeof(utc_time_t));
  memcpy(p_end, &g_time, sizeof(utc_time_t));
  p_end->hour = 23;
  p_end->minute = 59;
}

static RET_CODE epg_event_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  event_node_t *p_evt_node = NULL;
  u16           total_evt  = 0;
  utc_time_t curn_time = {0};
  s8 time_cmp_result = 0;
  u8 i, pos;

  u8 ascstr[32];
  u16 cnt = list_get_count(p_list);
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};
  book_pg_t temp_node = {0};


  p_evt_node = mul_epg_get_sch_event(&g_prog_info, &total_evt);
  if(p_evt_node == NULL)
  {
    return ERR_FAILURE;
  }
 /*!
  found start event node.
  */ 
  if(start > 0)
  {
    p_evt_node = mul_epg_get_sch_event_by_pos(&g_prog_info, p_evt_node, start);
  }

  for (i = 0; i < size; i++)
  {
    pos = (u8)(i + start);
    if((pos < cnt) && (pos < total_evt))
    {
      if(p_evt_node != NULL)
      { 
        time_to_local(&(p_evt_node->start_time), &start_time);
        memcpy(&end_time, &start_time, sizeof(utc_time_t));
        time_add(&end_time, &(p_evt_node->drt_time));
        sprintf((char*)ascstr,"%.2d:%.2d-%.2d:%.2d", \
          start_time.hour, start_time.minute,\
          end_time.hour, end_time.minute);

        memset(&temp_node, 0, sizeof(book_pg_t));
        temp_node.pgid = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), prog_focus);
        memcpy(&(temp_node.start_time), &start_time, sizeof(utc_time_t));
        memcpy(&(temp_node.drt_time), &(p_evt_node->drt_time), sizeof(utc_time_t));
        temp_node.book_mode = BOOK_TMR_ONCE;
        
        time_get(&curn_time, FALSE);
        time_cmp_result = time_cmp(&start_time, &curn_time, FALSE);
        if((book_get_match_node(&temp_node) < MAX_BOOK_PG) && (time_cmp_result >= 0))
        {
          list_set_field_content_by_icon(p_list, pos, 0, IM_EPG_BOOK);
        }
        else
        {
          list_set_field_content_by_icon(p_list, pos, 0, 0);
        }
        
        list_set_field_content_by_ascstr(p_list, pos, 1, ascstr);
        list_set_field_content_by_unistr(p_list, pos, 2, p_evt_node->event_name);

        if(p_evt_node != NULL && p_evt_node->p_sht_text != NULL)
        {
          list_set_field_content_by_icon(p_list, pos, 3, IM_EPG_INFOR);
        }
        else
        {
          list_set_field_content_by_icon(p_list, pos, 3, 0);
        }
        p_evt_node = mul_epg_get_sch_event_by_pos(&g_prog_info, p_evt_node, 1);
      }
    }
  }

  return SUCCESS;
}

//lint -e438 -e550 -e830
static void epg_item_update(control_t *p_cont, u8 focus_state, BOOL is_paint )
{
  dvbs_prog_node_t pg;
  utc_time_t s_time = {0}; 
  utc_time_t e_time = {0};  
  //utc_time_t tmp_time = {0};
  //utc_time_t tot_time = {0};//total time, 2 hours, for caculate x & w.
  control_t *p_list, *p_event_list, *p_mbox_week;
  control_t *p_list_cont, *p_prog_cont;
  
  event_node_t *p_evt_head = NULL;
  u16 total_evt  = 0;
  u16 list_curn, pg_id;
  u16 unistr[64];
  u16 old_focus = 0;
  u16 new_focus = 0;
  u8 day_pos;
  memset(&pg, 0, sizeof(dvbs_prog_node_t));

  epg_event_list_get_time_area(&s_time, &e_time);
    
  p_prog_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_PROG_CONT);
  p_list = ctrl_get_child_by_id(p_prog_cont, IDC_EPG_LIST_PG);

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_LIST_CONT);
  p_event_list = ctrl_get_child_by_id(p_list_cont, IDC_EPG_LIST_EVT);

  p_mbox_week = ctrl_get_child_by_id(p_list_cont, IDC_EPG_WEEKDAY);

  if(first_time.day != s_time.day)
  {
    epg_weekday_upgrade(p_mbox_week, TRUE);
    time_get(&first_time, FALSE);
  }

  if(p_mbox_week!=NULL)
  {
    day_pos = (u8)mbox_get_focus(p_mbox_week);

    if(day_pos != 0)
    {
      time_up(&s_time, day_pos*24*60*60);
      s_time.hour = 0;
      s_time.minute = 0;
      s_time.second = 0;

      memcpy(&e_time, &s_time, sizeof(utc_time_t));
      e_time.hour = 23;
      e_time.minute = 59;
      e_time.second = 59;
    }
   //ui_set_epg_day_offset(day_pos);
  }
  
  list_curn = list_get_focus_pos(p_list);
  
  pg_id =db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), list_curn);
  db_dvbs_get_pg_by_id(pg_id, &pg);

  g_prog_info.network_id = (u16)pg.orig_net_id;
  g_prog_info.ts_id      = (u16)pg.ts_id;
  g_prog_info.svc_id     = (u16)pg.s_id;
  time_to_gmt(&s_time, &(g_prog_info.start_time));
  time_to_gmt(&e_time, &(g_prog_info.end_time));
  mul_epg_set_db_info(&g_prog_info);
  p_evt_head = mul_epg_get_sch_event(&g_prog_info, &total_evt);

  if(p_evt_head != NULL)
  {
    MT_ASSERT(total_evt > 0);
    old_focus = list_get_focus_pos(p_event_list);
    list_set_count(p_event_list, (u16)total_evt, EPG_EVT_LIST_PAGE);
    switch(focus_state)
    {
      case FOCUS_FIRST:
        new_focus = 0;
        break;
      case FOCUS_PREV:
        new_focus = (old_focus < total_evt) ? old_focus : 0;
        break;
      default:  
        new_focus = 0;
        break;
    }
    if (new_focus != old_focus)
    {
      list_set_focus_pos(p_event_list, new_focus);
      list_select_item(p_event_list, new_focus);
    }
    epg_event_list_update(p_event_list, list_get_valid_pos(p_event_list), EPG_EVT_LIST_PAGE, 0);
  }
  else
  {
    list_set_count(p_event_list, 1, EPG_EVT_LIST_PAGE);
    list_set_focus_pos(p_event_list, 0);
    list_select_item(p_event_list, 0);

    list_set_field_content_by_icon(p_event_list, 0, 0, 0);

    if(CUSTOMER_ID == CUSTOMER_AISAT || CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
    {
      gui_get_string(IDS_MSG_NO_INFORMATION, unistr, 64);
    }
    else
    {
      gui_get_string(IDS_RECEIVING_DATA, unistr, 64);
    }
    list_set_field_content_by_unistr(p_event_list, 0, 2, unistr);
    list_set_field_content_by_icon(p_event_list, 0, 3, 0);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_event_list, is_paint);
  }
}
//lint +e438 +e550 +e830


static void epg_play_pg(u16 focus)
{
  u16 rid;

  if((rid =
        db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                   focus)) != INVALIDID)
  {
    curn_epg_prog_id = rid;
    epg_play_timer_start();
    //ui_play_prog(rid, FALSE);
  }
}
/*
static void ui_pg_list_start_roll(control_t *p_cont)
{
  roll_param_t p_param = {0};
  MT_ASSERT(p_cont != NULL);

  gui_stop_roll(p_cont);

  p_param.pace = ROLL_SINGLE;
  p_param.style = ROLL_LR;
  p_param.repeats = 0;
  p_param.is_force = FALSE;

  gui_start_roll(p_cont, &p_param);

}
*/
void epg_list_update_time(control_t *p_info, BOOL is_paint)
{
  utc_time_t cur_time;
  u8 asc_str[10];

  time_get(&cur_time, FALSE);

//  printf_time(&cur_time, "epg_list_update_time");

  sprintf((char *)asc_str, "%.2d/%.2d", cur_time.month, cur_time.day);
  mbox_set_content_by_ascstr(p_info, 0, asc_str);

  sprintf((char *)asc_str, "%.2d:%.2d", cur_time.hour, cur_time.minute);
  mbox_set_content_by_ascstr(p_info, 1, asc_str);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_info, TRUE);
  }
}


void epg_list_pg_name(u16 *str, u16 length)
{
  u32 len;
  u16 uni_node[3]; 

  len = uni_strlen(str);
  uni_node[0] = uni_node[1] = uni_node[2] ='.';
  if(len > length)
  {
    uni_strncpy(str + length,uni_node, 3);
  }
}

static RET_CODE epg_list_pg_update(control_t *p_list, u16 start, u16 size,
                                    u32 context)
{
  u16 i;
  u8 asc_str[8];
  u16 uni_str[32];
  u16 pg_id;
  u16 cnt = list_get_count(p_list);
  dvbs_prog_node_t pg;

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
        sprintf((char *)asc_str, "%.4d ", pg.logical_num);
      else
        sprintf((char *)asc_str, "%.4d ", start + i + 1);
      #endif
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      /* NAME */
      ui_dbase_get_full_prog_name(&pg, uni_str, 31);
       //epg_list_pg_name(uni_str, 8);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str);
    }
  }
  return SUCCESS;
}

#if 0
static RET_CODE epg_group_list_update(control_t *p_list, u16 start, u16 size,
                                       u32 context)
{
  u16 i;
  u16 uni_str[32];
  u16 group;
  u8 type;
  sat_node_t sat;


  u16 pos;

  u16 cnt = list_get_count(p_list);

  for(i = 0; i < size; i++)
  {
    if(i + start < cnt)
    {
      group = sys_status_get_group_by_pos(i + start);

      sys_status_get_group_info(group, &type, &pos, &context);

      switch(type)
      {
        case GROUP_T_ALL:
          gui_get_string(IDS_ALL, uni_str, 31);
          break;
        case GROUP_T_FAV:
          sys_status_get_fav_name((u8)context, uni_str);
          break;
        case GROUP_T_SAT:
          db_dvbs_get_sat_by_id((u16)context, &sat);
          uni_strncpy(uni_str, sat.name, 31);
          break;
        default:
          MT_ASSERT(0);
      }

      /* NAME */
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 0, uni_str);
    }
  }

  return SUCCESS;
}
#endif

//lint -e438 -e550 -e830
RET_CODE open_epg(u32 para1, u32 para2)
{
  control_t *p_menu;
  control_t *p_prog_cont, *p_list_title;//, *p_preview;
  control_t *p_list_cont;
  control_t *p_list_pg, *p_list_evt;
  control_t *p_evt_sbar;
  control_t *p_info;
  control_t *p_mbox_week;
 // control_t *p_detail, *p_tv_and_detail;
  u16 i, curn_group, curn_mode, pg_id, pg_pos, group_num;
  u32 group_context;
  dvbs_prog_node_t pg;
  utc_time_t s_time = {0};
  
  u16 total_pg = 0;

  //initialize global variables.
  time_get(&s_time, FALSE);
  time_get(&first_time, FALSE);
  
  ui_epg_start(EPG_TABLE_SELECTE_SCH_ALL);   

  group_num = sys_status_get_all_group_num();

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &group_context);  
  pg_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), pg_id);
  db_dvbs_get_pg_by_id(pg_id, &pg);
  total_pg = db_dvbs_get_count(ui_dbase_get_pg_view_id());

  //Create Menu
  p_menu = ui_comm_root_create(ROOT_ID_EPG,
                               0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,
                               RSC_INVALID_ID,
                               IDS_EPG);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_menu, epg_cont_proc);
  ctrl_set_keymap(p_menu, epg_cont_keymap);
  
  //prog container
  p_prog_cont = ctrl_create_ctrl(CTRL_CONT, IDC_EPG_PROG_CONT, EPG_PG_CONT_X,
                              EPG_PG_CONT_Y, EPG_PG_CONT_W, EPG_PG_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_prog_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);


  //EPG pg  list title
  p_list_title = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_TITLE,
                         EPG_LIST_TITLE_X, EPG_LIST_TITLE_Y,
                         EPG_LIST_TITLE_W, EPG_LIST_TITLE_H,
                         p_prog_cont, 0);
  ctrl_set_rstyle(p_list_title, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  text_set_align_type(p_list_title, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_list_title, FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_list_title, TEXT_STRTYPE_STRID);

  if((sys_status_get_curn_prog_mode() == CURN_MODE_TV))
  {
    text_set_content_by_strid(p_list_title, IDS_TV);
  }
  else
  {
    text_set_content_by_strid(p_list_title, IDS_RADIO);
  }
  
  //prog list
  p_list_pg = ctrl_create_ctrl(CTRL_LIST, IDC_EPG_LIST_PG, EPG_LIST_PGX,
                               EPG_LIST_PGY, EPG_LIST_PGW, EPG_LIST_PGH, p_prog_cont, 0);
  ctrl_set_rstyle(p_list_pg, RSI_EPG_LIST, RSI_EPG_LIST, RSI_EPG_LIST);
  ctrl_set_keymap(p_list_pg, epg_list_pg_keymap);
  ctrl_set_proc(p_list_pg, epg_list_pg_proc);
  ctrl_set_mrect(p_list_pg, 4, 14, (EPG_LIST_PGW - 4), (EPG_LIST_PGH - 14));
  list_set_item_interval(p_list_pg, EPG_VGAP);
  list_set_item_rstyle(p_list_pg, &epg_list_item_rstyle);
  list_enable_select_mode(p_list_pg, TRUE);
  list_set_select_mode(p_list_pg, LIST_SINGLE_SELECT);
  list_set_count(p_list_pg, total_pg, EPG_LIST_PAGE);
  list_set_field_count(p_list_pg, EPG_LIST_FIELD, EPG_LIST_PAGE);
  list_set_focus_pos(p_list_pg, pg_pos);
  list_select_item(p_list_pg, pg_pos);
  prog_focus = pg_pos;
  list_set_update(p_list_pg, epg_list_pg_update, 0);
  
  epg_list_pg_update(p_list_pg, list_get_valid_pos(p_list_pg), EPG_LIST_PAGE, 0);

  for(i = 0; i < EPG_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list_pg, (u8)i, (u32)(epg_pg_attr[i].attr), (u16)(epg_pg_attr[i].width),
                        (u16)(epg_pg_attr[i].left), (u8)(epg_pg_attr[i].top));
    list_set_field_rect_style(p_list_pg, (u8)i, epg_pg_attr[i].rstyle);
    list_set_field_font_style(p_list_pg, (u8)i, epg_pg_attr[i].fstyle);
  }

  //prog list sbar
  p_evt_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_EPG_LIST_SBAR, EPG_LIST_SBARX,
    EPG_LIST_SBARY, EPG_LIST_SBARW, EPG_LIST_SBARH, p_prog_cont, 0);
  ctrl_set_rstyle(p_evt_sbar, RSI_EPG_SBAR, RSI_EPG_SBAR, RSI_EPG_SBAR);
  sbar_set_autosize_mode(p_evt_sbar, 1);
  sbar_set_direction(p_evt_sbar, 0);
  sbar_set_mid_rstyle(p_evt_sbar, RSI_EPG_SBAR_MID, RSI_EPG_SBAR_MID,
                     RSI_EPG_SBAR_MID);
  list_set_scrollbar(p_list_pg, p_evt_sbar);
 
  //event list container.
  p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_EPG_LIST_CONT, EPG_EVENT_CONTX,
                                 EPG_EVENT_CONTY, EPG_EVENT_CONTW, EPG_EVENT_CONTH, p_menu, 0);
  ctrl_set_rstyle(p_list_cont, RSI_EPG_LIST_CONT, RSI_EPG_LIST_CONT, RSI_EPG_LIST_CONT);
  
  //week day
  p_mbox_week = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_EPG_WEEKDAY, 
                                  EPG_WEEKDAY_X, EPG_WEEKDAY_Y, EPG_WEEKDAY_W, 
                                  EPG_WEEKDAY_H, p_list_cont, 0);
  ctrl_set_rstyle(p_mbox_week, RSI_PBACK, RSI_PBACK, RSI_PBACK);  
  ctrl_set_style(p_mbox_week, STL_EX_ALWAYS_HL);
  mbox_set_count(p_mbox_week, SEVEN, SEVEN, 1);
  ctrl_set_mrect(p_mbox_week, EPG_WEEKDAY_LEFT, EPG_WEEKDAY_TOP, EPG_WEEKDAY_RIGHT, EPG_WEEKDAY_BOTTOM);
  mbox_set_item_rstyle(p_mbox_week, RSI_DATE_HL, RSI_DATE_SH, RSI_DATE_SH);
  mbox_set_item_interval(p_mbox_week, EPG_WEEKDAY_HGAP, 0);
  mbox_set_string_fstyle(p_mbox_week, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
  mbox_set_focus(p_mbox_week, 0);
  epg_weekday_upgrade(p_mbox_week, FALSE);
  
  //event list
  p_list_evt = ctrl_create_ctrl(CTRL_LIST, IDC_EPG_LIST_EVT, EPG_LIST_EVTX, 
      EPG_LIST_EVTY, EPG_LIST_EVTW, EPG_LIST_EVTH, p_list_cont, 0);
  ctrl_set_rstyle(p_list_evt, RSI_EPG_LIST, RSI_EPG_LIST, RSI_EPG_LIST);
  ctrl_set_keymap(p_list_evt, epg_list_evt_keymap);
  ctrl_set_proc(p_list_evt, epg_list_evt_proc);
  ctrl_set_mrect(p_list_evt, 8, 8, (EPG_LIST_EVTW - 16), (EPG_LIST_EVTH - 16));
  list_set_item_interval(p_list_evt, EPG_VGAP);
  list_set_item_rstyle(p_list_evt, &epg_evt_list_item_rstyle);
  list_enable_select_mode(p_list_evt, TRUE);
  list_set_select_mode(p_list_evt, LIST_SINGLE_SELECT);  
  list_set_count(p_list_evt, 0, EPG_EVT_LIST_PAGE);
  list_set_field_count(p_list_evt, EPG_EVT_LIST_FIELD, EPG_EVT_LIST_PAGE);
  list_set_focus_pos(p_list_evt, 0);
  list_select_item(p_list_evt, 0);
  list_set_update(p_list_evt, epg_event_list_update, 0);
  
  for (i = 0; i < EPG_EVT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list_evt, (u8)i, (u32)(epg_evt_attr[i].attr), (u16)(epg_evt_attr[i].width),
                        (u16)(epg_evt_attr[i].left), (u8)(epg_evt_attr[i].top));
    list_set_field_rect_style(p_list_evt, (u8)i, epg_evt_attr[i].rstyle);
    list_set_field_font_style(p_list_evt, (u8)i, epg_evt_attr[i].fstyle);
  }    

  //prog evt sbar
  p_evt_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_EPG_EVT_SBAR, EPG_EVT_SBARX,
    EPG_EVT_SBARY, EPG_EVT_SBARW, EPG_EVT_SBARH, p_list_cont, 0);
  ctrl_set_rstyle(p_evt_sbar, RSI_EPG_SBAR, RSI_EPG_SBAR, RSI_EPG_SBAR);
  sbar_set_autosize_mode(p_evt_sbar, 1);
  sbar_set_direction(p_evt_sbar, 0);
  sbar_set_mid_rstyle(p_evt_sbar, RSI_EPG_SBAR_MID, RSI_EPG_SBAR_MID,
                     RSI_EPG_SBAR_MID);
  list_set_scrollbar(p_list_evt, p_evt_sbar);

   //time & date on top of epg menu
  p_info = ctrl_create_ctrl(CTRL_MBOX, IDC_EPG_TIME, EPG_TIME_X, EPG_TIME_Y,
    EPG_TIME_W, EPG_TIME_H, p_menu, 0);
  ctrl_set_rstyle(p_info, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_mrect(p_info, 0, 0, EPG_TIME_W, EPG_TIME_H);
  mbox_enable_icon_mode(p_info, FALSE);
  mbox_enable_string_mode(p_info, TRUE);
  mbox_set_count(p_info, EPG_TIME_MBOX_TOL,
    EPG_TIME_MBOX_COL, EPG_TIME_MBOX_ROW);
  mbox_set_item_interval(p_info, EPG_TIME_MBOX_HGAP, EPG_TIME_MBOX_VGAP);
  mbox_set_item_rstyle(p_info, RSI_EPG_TIME,
    RSI_EPG_TIME, RSI_EPG_TIME);
  mbox_set_string_fstyle(p_info, FSI_EPG_TIME_MBOX,
    FSI_EPG_TIME_MBOX, FSI_EPG_TIME_MBOX);
  mbox_set_string_offset(p_info, 0, 0);
  mbox_set_string_align_type(p_info, STL_RIGHT | STL_VCENTER);
  mbox_set_icon_align_type(p_info, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_info, MBOX_STRTYPE_UNICODE);

  epg_list_update_time(p_info, FALSE);
  
  epg_item_update(p_menu, FOCUS_FIRST, FALSE);  
  
  ui_comm_help_create2(&epg_help_data[0], p_menu, FALSE);

  ctrl_default_proc(p_list_pg, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  epg_init_play_timer(ROOT_ID_EPG, MSG_MEDIA_PLAY_TMROUT, 200);
  epg_play_pg(list_get_focus_pos(p_list_pg));
  return SUCCESS;
}
//lint +e438 +e550 +e830

static RET_CODE on_epg_ready(control_t *p_cont, u16 msg,
                              u32 para1, u32 para2)
{
  //update epg data.  
  if(para1 == EVENT_ATTR_CURRENT_PROG)
  {
    epg_item_update(p_cont, FOCUS_PREV, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_epg_time_update(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_info;
  
  p_info = ctrl_get_child_by_id(p_cont, IDC_EPG_TIME);
  epg_list_update_time(p_info, TRUE);

  on_epg_ready(p_cont,msg,EVENT_ATTR_CURRENT_PROG,0);
  return SUCCESS;
}
static RET_CODE on_epg_destory(control_t *p_cont, u16 msg,
                                u32 para1, u32 para2)
{
 // ui_enable_chk_pwd(TRUE);
  epg_deinit_play_timer();
  return ERR_NOFEATURE;
}

static RET_CODE on_epg_exit_all(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  //ui_time_enable_heart_beat(FALSE); 
  
  ui_close_all_mennus();
  
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  return SUCCESS;
}

static RET_CODE on_epg_area_change(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  u8 act_id;
  control_t *p_cont, *p_new_act_list=NULL;//,*p_tv_and_detail_cont, *p_detail;
  control_t *p_prog_cont, *p_list_cont;

  p_cont = ctrl_get_parent(p_list->p_parent);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_LIST_CONT);
  p_prog_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_PROG_CONT);
  
  act_id = (u8)ctrl_get_ctrl_id(p_list);

  switch(act_id)
  {
  case IDC_EPG_LIST_PG:
    p_new_act_list = ctrl_get_child_by_id(p_list_cont, IDC_EPG_LIST_EVT);
    ui_comm_help_create2(&epg_help_data[1], p_cont, TRUE);
    break;

  case IDC_EPG_LIST_EVT:
    list_set_focus_pos(p_list, 0);
    list_select_item(p_list, 0);
    p_new_act_list = ctrl_get_child_by_id(p_prog_cont, IDC_EPG_LIST_PG);
    #if  0
    p_tv_and_detail_cont = ctrl_get_child_by_id(p_cont, IDC_TV_AND_DETAIL_CONT);
    p_detail = ctrl_get_child_by_id(p_tv_and_detail_cont, IDC_EPG_DETAIL);
    text_set_content_by_unistr(p_detail, (u16*)"");
    //ctrl_set_rstyle(p_detail, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_sts(p_tv_and_detail_cont, OBJ_STS_HIDE);
    ctrl_paint_ctrl(p_tv_and_detail_cont, TRUE);
    #endif
    ui_comm_help_create2(&epg_help_data[0], p_cont, TRUE);
    break;
  default:
    break;
  }

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0);
  ctrl_set_attr(p_new_act_list, OBJ_ATTR_ACTIVE);
  ctrl_process_msg(p_new_act_list, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_epg_list_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();
  return SUCCESS;
}

static RET_CODE on_epg_change_weekday(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  control_t *p_weekday_cbox, *p_cont, *p_event_list;//,*p_tv_and_detail_cont, *p_detail;
  u8 old_focus, new_focus=0;
  
  p_cont = ctrl_get_parent(p_list);
  p_weekday_cbox = ctrl_get_child_by_id(p_cont, IDC_EPG_WEEKDAY);
  p_event_list = ctrl_get_child_by_id(p_cont, IDC_EPG_LIST_EVT);

  old_focus = (u8)mbox_get_focus(p_weekday_cbox);

  switch(msg)
  {
  case MSG_WEEKDAY_LEFT:
    new_focus = (old_focus - 1 + SEVEN)%SEVEN;
    break;

  case MSG_WEEKDAY_RIGHT:
    new_focus = (old_focus + 1)%SEVEN;
    break;
  default:
    break;
  }
  mbox_set_focus(p_weekday_cbox, new_focus);

  epg_weekday_upgrade(p_weekday_cbox, TRUE);

  epg_item_update(p_cont->p_parent, 0, TRUE);
  ctrl_paint_ctrl(p_event_list, TRUE);
  #if 0
  p_tv_and_detail_cont = ctrl_get_child_by_id(p_cont->p_parent, IDC_TV_AND_DETAIL_CONT);
  p_detail = ctrl_get_child_by_id(p_tv_and_detail_cont, IDC_EPG_DETAIL);

  if(ctrl_get_sts(p_tv_and_detail_cont) != OBJ_STS_HIDE)
  {
    on_epg_show_detail(p_list, msg, para1, para2);
  }
  #endif
  return SUCCESS;
}

static RET_CODE on_epg_prog_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  u16 old_focus, new_focus;
  control_t *p_weekday_cbox, *p_list_cont;
  control_t *p_cont;

  p_cont = ctrl_get_parent(p_list->p_parent);

  old_focus = list_get_focus_pos(p_list);
  list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  prog_focus = new_focus;
  list_select_item(p_list, new_focus);

  list_draw_item_ext(p_list, old_focus, TRUE);
  list_draw_item_ext(p_list, new_focus, TRUE);
  ctrl_paint_ctrl(p_list, TRUE);
  epg_play_pg(list_get_focus_pos(p_list)); 
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_LIST_CONT);
  p_weekday_cbox = ctrl_get_child_by_id(p_list_cont, IDC_EPG_WEEKDAY); 
  mbox_set_focus(p_weekday_cbox, 0);
  ctrl_paint_ctrl(p_weekday_cbox, TRUE);
  epg_item_update(p_cont, 0, TRUE);

  return SUCCESS;
}

static void epg_replace_the_conflict_event(void)
{
  book_delete_all_conflict_node(&book_node);
  book_add_node(&book_node);
}

static void epg_replace_the_conflict_already_add(void)
{
  control_t *p_list, *p_list_cont;
  u16 focus;
  p_list_cont = ui_comm_root_get_ctrl(ROOT_ID_EPG, IDC_EPG_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_EPG_LIST_EVT);
  focus = list_get_focus_pos(p_list);
  book_delete_all_conflict_node(&book_node);
  book_add_node(&book_node);
  list_set_field_content_by_icon(p_list, focus, 0, IM_EPG_BOOK);
  list_draw_field_ext(p_list, focus, 0, TRUE);
}

static RET_CODE on_epg_event_list_book(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  event_node_t *p_evt_node = NULL;
  u16 total_event = 0;
  u16 count;
  u16 focus;
  dvbs_prog_node_t pg = {0};

  count = list_get_count(p_list);

  if(count > 0)
  {
    focus = list_get_focus_pos(p_list);
    p_evt_node = mul_epg_get_sch_event(&g_prog_info, &total_event);
 
    if(focus > 0)
    {
      p_evt_node = mul_epg_get_sch_event_by_pos(&g_prog_info,
                                                p_evt_node,
                                                focus);
    }
    if(p_evt_node != NULL)
    {
      memset(&book_node, 0, sizeof(book_node));
      book_node.pgid = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),prog_focus);
      db_dvbs_get_pg_by_id(book_node.pgid, &pg);
      //book_node.svc_type = (u8)pg.service_type;
      book_node.book_mode = BOOK_TMR_ONCE;  
      time_to_local(&(p_evt_node->start_time), &(book_node.start_time));
      memcpy(&(book_node.drt_time), &(p_evt_node->drt_time), sizeof(utc_time_t)); 

      memcpy(book_node.event_name, p_evt_node->event_name, sizeof(book_node.event_name));
      book_node.record_enable = TRUE;
      switch(check_book_pg(&book_node))
      {
        case BOOK_ERR_DUR_TOO_SHORT:
          ui_comm_cfmdlg_open(NULL, IDS_LESS_ONE_MIN, NULL, 0); 
          break;

        case BOOK_ERR_PLAYING:
          ui_comm_cfmdlg_open(NULL, IDS_CUR_EVENT_IS_PLAYING, NULL, 0); 
          break;

        case BOOK_ERR_CONFILICT:
          //ui_comm_cfmdlg_open(NULL, IDS_BOOK_CONFLICT, RSC_INVALID_ID, NULL, 0); 
          ui_comm_ask_for_dodlg_open(NULL, IDS_CONFLICT_AND_REPLACE, 
                                   epg_replace_the_conflict_event, NULL, 0);
          if(check_book_pg(&book_node) == BOOK_ERR_SAME)
          {
            list_set_field_content_by_icon(p_list, focus, 0, IM_EPG_BOOK);
            list_draw_field_ext(p_list, focus, 0, TRUE);
          }
          break;
        case BOOK_ERR_SAME:
          book_delete_node(book_get_match_node(&book_node));
          if(check_book_pg(&book_node) == BOOK_ERR_SAME)
          {
            ui_comm_ask_for_dodlg_open(NULL, IDS_BOOK_CONFLICT,
              epg_replace_the_conflict_already_add, NULL, 0);
          }
          else
          {
            list_set_field_content_by_icon(p_list, focus, 0, 0);
            list_draw_field_ext(p_list, focus, 0, TRUE);
          }
          break;

        case BOOK_ERR_FULL:
          ui_comm_cfmdlg_open(NULL, IDS_BOOK_IS_FULL,NULL, 0);
          break;

        case BOOK_ERR_NO:
          list_set_field_content_by_icon(p_list, focus, 0, IM_EPG_BOOK);
          list_draw_field_ext(p_list, focus, 0, TRUE);
          book_add_node(&book_node);
          break;
          
        default:
          break;
      }
    }
  }

  return ret;
}

static RET_CODE on_epg_open_book(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  manage_open_menu(ROOT_ID_BOOK_LIST, 0, 0);
  return ret;
}

static RET_CODE on_epg_change_group(control_t *p_list, u16 msg, 
                                     u32 para1, u32 para2)
{
  u16 pg_id, pg_pos = 0xFFFF;
  u8 view_id = 0xFF;
  u16 view_count = 0;
  control_t * p_epg_title = NULL;
//  u16 rsc_id = RSC_INVALID_ID;

 p_epg_title = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_EPG_TITLE);
 ui_tvradio_switch(TRUE, &pg_id);
/*
  if(rsc_id != RSC_INVALID_ID)
  {
    ui_comm_cfmdlg_open(NULL, rsc_id, NULL, 0);
  }
  else
  */
  { 
    view_id = ui_dbase_get_pg_view_id();
    view_count = db_dvbs_get_count(view_id);
    pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
    
    list_set_count(p_list, view_count, EPG_LIST_PAGE);
    list_set_focus_pos(p_list, pg_pos);
    epg_list_pg_update(p_list, list_get_valid_pos(p_list), EPG_LIST_PAGE, 1);
    list_select_item(p_list, pg_pos);
    prog_focus = pg_pos;
    ctrl_paint_ctrl(p_list, TRUE);

    if(sys_status_get_curn_prog_mode() == CURN_MODE_TV)
    {
      text_set_content_by_strid(p_epg_title, IDS_TV);
    }
    else
    {
      text_set_content_by_strid(p_epg_title, IDS_RADIO);
    }
    
    {
      ctrl_paint_ctrl(p_epg_title, TRUE);
    }
    epg_item_update(p_list->p_parent->p_parent, 0, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_epg_book_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_event_list, *p_list_cont;

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_LIST_CONT);
  p_event_list = ctrl_get_child_by_id(p_list_cont, IDC_EPG_LIST_EVT);
  
  epg_event_list_update(p_event_list, list_get_valid_pos(p_event_list), EPG_EVT_LIST_PAGE, 0);
  
  ctrl_paint_ctrl(p_event_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_epg_event_list_up_down(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //control_t *p_tv_and_detail_cont, *p_detail;
  u16 new_focus;
  
  #if 0
  p_tv_and_detail_cont = ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_TV_AND_DETAIL_CONT);
  p_detail = ctrl_get_child_by_id(p_tv_and_detail_cont, IDC_EPG_DETAIL);
  #endif
  list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  list_select_item(p_list, new_focus);

  ctrl_paint_ctrl(p_list, TRUE);
  #if 0
  if(ctrl_get_sts(p_tv_and_detail_cont) != OBJ_STS_HIDE)
  {
    on_epg_show_detail(p_list, msg, para1, para2);
  }
  #endif
  return SUCCESS;
}





static RET_CODE on_epg_play_curn_prog(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
  /*fix Bug57230*/
  if(ui_is_chk_pwd())
  {
    if (!ui_is_pass_chkpwd(curn_epg_prog_id))
    {
      ui_reset_chkpwd();
    }
  }
  ui_play_prog(curn_epg_prog_id, FALSE);
  epg_play_timer_set_state(0);
  return SUCCESS;
}

BEGIN_KEYMAP(epg_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EPG, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(epg_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(epg_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_epg_exit_all)
  ON_COMMAND(MSG_SCHE_READY, on_epg_ready)
  ON_COMMAND(MSG_TIME_UPDATE, on_epg_time_update)
  ON_COMMAND(MSG_DESTROY, on_epg_destory)
  ON_COMMAND(MSG_BOOK_UPDATE, on_epg_book_update)
  ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, on_epg_play_curn_prog)
END_MSGPROC(epg_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(epg_list_pg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_YELLOW, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_BLUE, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_TVRADIO, MSG_AUTO_SWITCH)
END_KEYMAP(epg_list_pg_keymap, NULL)

BEGIN_MSGPROC(epg_list_pg_proc, list_class_proc)
  ON_COMMAND(MSG_AREA_CHANGE, on_epg_area_change)
  ON_COMMAND(MSG_EXIT_ALL, on_epg_list_ok)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_epg_prog_list_change_focus)
  ON_COMMAND(MSG_OPEN_BOOK, on_epg_open_book)
  ON_COMMAND(MSG_AUTO_SWITCH, on_epg_change_group)
END_MSGPROC(epg_list_pg_proc, list_class_proc)

BEGIN_KEYMAP(epg_list_evt_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_WEEKDAY_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_WEEKDAY_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_YELLOW, MSG_AREA_CHANGE)
  ON_EVENT(V_KEY_OK, MSG_BOOK)
  ON_EVENT(V_KEY_BLUE, MSG_OPEN_BOOK)
  ON_EVENT(V_KEY_INFO, MSG_SHOW_DETAIL)
END_KEYMAP(epg_list_evt_keymap, NULL)

BEGIN_MSGPROC(epg_list_evt_proc, list_class_proc)
  ON_COMMAND(MSG_AREA_CHANGE, on_epg_area_change)
  ON_COMMAND(MSG_WEEKDAY_LEFT, on_epg_change_weekday)
  ON_COMMAND(MSG_WEEKDAY_RIGHT, on_epg_change_weekday)
  ON_COMMAND(MSG_BOOK, on_epg_event_list_book)
  ON_COMMAND(MSG_OPEN_BOOK, on_epg_open_book)
  ON_COMMAND(MSG_SHOW_DETAIL, on_epg_show_detail)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_event_list_up_down)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_event_list_up_down)
END_MSGPROC(epg_list_evt_proc, list_class_proc)
