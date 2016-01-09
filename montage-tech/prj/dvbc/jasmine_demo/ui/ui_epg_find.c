/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
//#include "time.h"
#include "ui_epg_find.h"
#include "ui_timer.h"
#include "ui_keyboard_ctrl.h"

#define IDC_EPG_FIND_CONT_ID IDC_COMM_ROOT_CONT

enum epg_find_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_GREEN,
  MSG_BLUE,
  MSG_YELLOW,
};


enum focus_style
{
  FOCUS_NONE = 0,
  FOCUS_FIRST,
  FOCUS_LAST,
  FOCUS_NEXT,
  FOCUS_PREV,
};

enum epg_find_menu_ctrl_id
{
  IDC_EPG_FIND_ICON = 1,
  IDC_EPG_FIND_PREVIEW,
  IDC_EPG_FIND_TOP_LINE,
  IDC_EPG_FIND_BOTTOM_LINE,
  IDC_EPG_FIND_DETAIL,
  IDC_EPG_FIND_TIME,
  IDC_EPG_FIND_CONDITION,
  IDC_EPG_FIND_KEYBAORD,
  IDC_EPG_FIND_GENRE,
  IDC_EPG_FIND_TD,
  IDC_EPG_FIND_HELP,
};

enum epg_find_list_cont_id
{
  IDC_EPG_FIND_LIST_EVENT = 1,
  IDC_EPG_FIND_LIST_DATE,
};

enum epg_find_list_detail_id
{
  IDC_EPG_FIND_DETAIL_TTL = 1,
  IDC_EPG_FIND_DETAIL_PG,
  IDC_EPG_FIND_DETAIL_SAT,
  IDC_EPG_FIND_DETAIL_EVTNAME,
  IDC_EPG_FIND_DETAIL_EVTTIME,
  IDC_EPG_FIND_DETAIL_SHORT_EVT,
  IDC_EPG_FIND_DETAIL_TYPE,
  IDC_EPG_FIND_DETAIL_SCRAMBLE,
  IDC_EPG_FIND_DETAIL_AGE,
  IDC_EPG_FIND_DETAIL_RESERVE,
};

enum epg_find_genre_cont_id
{
  IDC_EPG_FIND_GENRE_TITLE = 1,
  IDC_EPG_FIND_GENRE_LLIST,
  IDC_EPG_FIND_GENRE_LSBAR,
  IDC_EPG_FIND_GENRE_RLIST,
  IDC_EPG_FIND_GENRE_RSBAR,
};

enum epg_find_td_cont_id
{
  IDC_EPG_FIND_TD_TITLE = 1,
  IDC_EPG_FIND_TD_TIME_TEXT,
  IDC_EPG_FIND_TD_DAY_TEXT,
  IDC_EPG_FIND_TD_OK_TEXT,
  IDC_EPG_FIND_TD_CANCEL_TEXT,
  IDC_EPG_FIND_TD_TIME_LIST,
  IDC_EPG_FIND_TD_DAY_LIST,
};



book_pg_t book_node;
utc_time_t g_stime = {0};
utc_time_t g_etime = {0};
u8 g_genre_level = 0;
u8 g_day_focus = 0;//(0):everyday  (1):Sunday  (2):Monday  (3:)Tuesday  (4):Wednesday  (5):Thursday  (6):Friday  (7):Saturday
u8 g_time_focus = 0;//(0):00:00-24:00  (1):00:00-06:00  (2):06:00-12:00  (3):12:00-18:00  (4):18:00-24:00
u16 g_keyword[DB_DVBS_MAX_NAME_LENGTH] = {0};
evt_node_t **g_pp_buf = NULL;
u32 g_evt_num = 0;


typedef struct duration
{
  u8 start;
	u8 end;
}duration_s;

duration_s g_time_duration[] =
{
  {0, 24},
  {0, 6},
  {6, 12},
  {12,18},
  {18,24},
};


static RET_CODE epg_find_list_event_update(control_t* p_list, u16 start, u16 size, u32 context);

RET_CODE epg_find_mask_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_cont_keymap(u16 key);
RET_CODE epg_find_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_list_event_keymap(u16 key);
RET_CODE epg_find_list_event_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_condition_mbox_keymap(u16 key);
RET_CODE epg_find_condition_mbox_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_genre_llist_keymap(u16 key);
RET_CODE epg_find_genre_llist_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_genre_rlist_keymap(u16 key);
RET_CODE epg_find_genre_rlist_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_td_time_list_keymap(u16 key);
RET_CODE epg_find_td_time_list_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_td_day_list_keymap(u16 key);
RET_CODE epg_find_td_day_list_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_td_time_keymap(u16 key);
RET_CODE epg_find_td_time_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_td_day_keymap(u16 key);
RET_CODE epg_find_td_day_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_td_ok_keymap(u16 key);
RET_CODE epg_find_td_ok_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

u16 epg_find_td_cancel_keymap(u16 key);
RET_CODE epg_find_td_cancel_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

#if 0
static void epg_find_play_pg(u16 focus)
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

event_node_t *epg_find_get_curn_node(void)
{
  epg_prog_info_t prog_info = {0};
  event_node_t   *p_present_evt = NULL;
  event_node_t   *p_follow_evt = NULL;
  dvbs_prog_node_t pg;
  u16 pg_id;
  
  pg_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(pg_id, &pg);
  
  prog_info.network_id = (u16)pg.orig_net_id;
  prog_info.ts_id      = (u16)pg.ts_id;
  prog_info.svc_id     = (u16)pg.s_id;
       
  mul_epg_get_pf_event(&prog_info, &p_present_evt, &p_follow_evt);
  return p_present_evt;
}
static void epg_find_set_detail(control_t *p_detail, BOOL is_paint)
{
  control_t *p_pg, *p_evtname, *p_evttime, *p_short;
  control_t *p_type, *p_scramble, *p_age, *p_reservation;
  dvbs_prog_node_t pg;
  event_node_t *p_evt_node;
  utc_time_t startime = {0};
  utc_time_t endtime = {0};
  u16 uni_str1[32];
  u16 uni_str2[32];
  u8 asc_str[32];
  u16 pg_id,curn_group,pg_pos;
  u8 curn_mode;
  u32 group_context;

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &group_context);
  pg_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), pg_id);
  db_dvbs_get_pg_by_id(pg_id, &pg);

  p_pg = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_PG);
  p_evtname = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_EVTNAME);
  p_evttime = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_EVTTIME);
  p_short = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_SHORT_EVT);
  p_type = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_TYPE);
  p_scramble = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_SCRAMBLE);
  p_age = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_AGE);
  p_reservation = ctrl_get_child_by_id(p_detail, IDC_EPG_FIND_DETAIL_RESERVE);

  //set pg name
  sprintf(asc_str, "%.4d ", pg_pos + 1);
  str_asc2uni(asc_str, uni_str1);
  ui_dbase_get_full_prog_name(&pg, uni_str2, 31);
  uni_strncpy(&uni_str1[uni_strlen(uni_str1)], uni_str2, 31);
 
  text_set_content_by_unistr(p_pg, uni_str1);

  //pg type
  if(pg.tv_flag)
  {
    text_set_content_by_strid(p_type, IDS_TV);
  }
  else
  {
    text_set_content_by_strid(p_type, IDS_RADIO);
  }
  
  //pg scramble
  if(pg.is_scrambled)
  {
    text_set_content_by_unistr(p_scramble, (u16*)"$");
  }
  else
  {
    text_set_content_by_unistr(p_scramble, (u16*)"");
  }

  p_evt_node = epg_find_get_curn_node();

  if(p_evt_node != NULL)
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

    if(p_evt_node->p_sht_text != NULL)
    {
      text_set_content_by_unistr(p_short, p_evt_node->p_sht_text);
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
    ctrl_paint_ctrl(p_type, TRUE);
    ctrl_paint_ctrl(p_scramble, TRUE);
    ctrl_paint_ctrl(p_age, TRUE);
    ctrl_paint_ctrl(p_reservation, TRUE);
  }  
}

static RET_CODE epg_find_genre_llist_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{ 
  u8 asc_str[MAX_GENRE_LLIST_COUNT][32] =
    {
      "All", "Movie", "News", "Shows", "Sports", "Children's", "Music", "Art",
      "Society", "Education", "Leisure", "Specials",
    };
  u16 i;
  u16 pos, cnt = list_get_count(p_list);

  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      list_set_field_content_by_icon(p_list, pos, 0, IM_TV_DEL);

      list_set_field_content_by_ascstr(p_list, pos, 1, asc_str[pos]); 
    }
  }
  return SUCCESS;
}

static RET_CODE epg_find_genre_rlist_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{
  u8 asc_str[MAX_GENRE_LLIST_COUNT][MAX_GENRE_RLIST_COUNT][64] = 
    {

			{
				"All",
			},
			{
				"All", "Drama", "Detective", "Adventure", "Science fiction", "Comedy", "Soap", "Romance",
			  "Serious", "Adult movie",
			},
			{
				"All", "Current affairs", "Weather report", "News magazine", "Documentary", "Discussion",
			},
			{
				"All", "Game show", "Quiz", "Variety show", "Talk show",
			},
			{
				"All", "Sports(General)", "Special events", "Sports magazines", "Football", "Tennis",
				"Team sports", "Athletics",
			  "Motor sport", "Water sport", "Winter sports", "Equestrian", "Martial sports",
			},
			{
				"All", "Youth programmes", "Pre-school children's", "Entertainment 6 to 14",
				"Entertainment 10 to 16", "Informational", "Cartoons",
			},
			{
				"All", "Dance", "Rock", "Serious", "Folk", "Jazz", "Musical", "Ballet",
			},
			{
				"All", "Culture", "Performing arts", "Fine arts", "Religion", "Popular culture",
				"Literature", "Film", "Experimental film", "Broardcasting", "New media", "Arts", "Fashion",
			},
			{
				"All", "Political issues", "Magazines", "Economics", "Remarkable people",
			},
			{
				"All", "Science", "Nature", "Technology", "Medicine", "Foreign countries",
				"Social", "Art", "Further education", "Languages",
			},
			{
				"All", "Leisure hobbies", "Tourism", "Handicraft", "Motoring", "Fitness & health",
				"Cooking", "Advertisement", "Gardening",
			},
			{
				"All", "Original language", "Black & white", "Unpublished", "Live broardcast",
			},
    };
  u16 i;
  u16 pos, cnt;
  u16 pos_llist = list_get_focus_pos(
    ctrl_get_child_by_id(p_list->p_parent, IDC_EPG_FIND_GENRE_LLIST)
    );

  //cnt = list_get_count(p_list);
  for (i = 0; i < MAX_GENRE_RLIST_COUNT; i++)
  {
    if(strcmp(asc_str[pos_llist][i],"") == 0)
    {
      break;
    }
  }
  cnt = i;

  list_set_count(p_list, cnt, EPG_FIND_GENRE_RLIST_PAGE);

  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      list_set_field_content_by_ascstr(p_list, pos, 0, asc_str[pos_llist][pos]); 
      //list_set_field_content_by_icon(p_list, pos, 0, IM_TV_FAV); 
    }
  }
  return SUCCESS;
}

static RET_CODE epg_find_time_list_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{ 
  u8 asc_str[MAX_TIME_LIST_COUNT][32] =
    {
      "Every Time", "00:00-06:00", "06:00-12:00", "12:00-18:00", "18:00-24:00",
    };
  u16 i;
  u16 pos, cnt = list_get_count(p_list);

  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      list_set_field_content_by_ascstr(p_list, pos, 0, asc_str[pos]); 
    }
  }
  return SUCCESS;
}

static RET_CODE epg_find_day_list_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{ 
  u8 asc_str[MAX_DAY_LIST_COUNT][32] =
		{
		  "Every Day", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
		};
	u16 i;
	u16 pos, cnt = list_get_count(p_list);

	for (i = 0; i < size; i++)
	{
		pos = i + start;
		if (pos < cnt)
		{
			list_set_field_content_by_ascstr(p_list, pos, 0, asc_str[pos]); 
		}
	}
	return SUCCESS;
}

static void epg_find_create_epg_list()
{
	u8 asc_str[32];

	utc_time_t s_time = {0};
	u8 now_weekday = 0;
	u8 increase_day = 0;
	u8 focus_weekday = 0;

	utc_time_t start_day = {0};
	utc_time_t end_day = {0};

	utc_time_t start_time = {0};
	utc_time_t end_time = {0};

	u32 total_num = 0;

	str_uni2asc(asc_str, g_keyword);

	//initialize global variables.
	time_get(&s_time, FALSE);

	start_time.hour = g_time_duration[g_time_focus].start;
	end_time.hour = g_time_duration[g_time_focus].end;
	if(end_time.hour == 24)
	{
		end_time.hour = 23;
		end_time.minute = 59;
		end_time.second = 59;
	}

	if(g_day_focus == 0)
	{
		memcpy(&start_day, &s_time, sizeof(utc_time_t));
		memcpy(&end_day, &s_time, sizeof(utc_time_t));
		time_up(&end_day, 7*24*60*60);
	}
	else
	{
		now_weekday = date_to_weekday(&s_time);

		focus_weekday = (g_day_focus + 5) % 7;

		if( focus_weekday >= now_weekday)
		{
			increase_day = focus_weekday - now_weekday;
		}
		else
		{
			increase_day = 7 - (now_weekday - focus_weekday);
		}

		memcpy(&start_day, &s_time, sizeof(utc_time_t));
		memcpy(&end_day, &s_time, sizeof(utc_time_t));
		time_up(&end_day, increase_day*24*60*60);

		if(focus_weekday != now_weekday)
		{
			memcpy(&start_day, &end_day, sizeof(utc_time_t));
		}
	}

	total_num = epg_get_all_evt_num(class_get_handle_by_id(EPG_CLASS_ID));

	if(total_num)
	{
	  if(g_pp_buf != NULL)
	  {
	    mtos_free(g_pp_buf);
			g_pp_buf = NULL;
	  }
		g_pp_buf = (evt_node_t **)mtos_malloc(total_num*sizeof(void *));
    MT_ASSERT(g_pp_buf != NULL);
    memset(g_pp_buf, 0, total_num*sizeof(void *));
		g_evt_num = epg_create_list(class_get_handle_by_id(EPG_CLASS_ID), g_pp_buf,
			asc_str, 0, &start_day, &end_day, &start_time, &end_time);
	}

}

static RET_CODE epg_find_list_event_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{
	u16 i = 0;
	u8 asc_str[32];

	utc_time_t s_time = {0};
	u8 evt_name[17] = {0};

	evt_node_t *p_evt = NULL;

 	if(g_evt_num)
	{
		epg_list_sort_by_time(g_pp_buf, g_evt_num);

		for(i=0; i<size; i++)
		{
			if ((u32)(i + start) < g_evt_num)
			{
				p_evt = (g_pp_buf[i + start]);
				str_uni2asc((u8*)&evt_name, (u16*)&(p_evt->event_name));

				OS_PRINTF("!!! [%s] %d:%d:%d	%d:%d:%d\n",
					evt_name,
					p_evt->start_time.year,
					p_evt->start_time.month, p_evt->start_time.day,
					p_evt->start_time.hour, p_evt->start_time.minute,
					p_evt->start_time.second);

				if(p_evt != NULL)
				{
					/* NO. */
					list_set_field_content_by_dec(p_list, (u16)(start + i), 0, i + start); 

					/* NAME */
					list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, p_evt->event_name); 

					/* TIME */
					//time_to_local(&(p_evt_node->start_time), &s_time);
					memcpy(&s_time, &(p_evt->start_time), sizeof(utc_time_t));
					sprintf(asc_str, "%02d/%02d %.2d:%.2d",s_time.month, s_time.day, s_time.hour, s_time.minute);
					list_set_field_content_by_ascstr(p_list, (u16)(start + i), 3, asc_str); 
				}
				else
				{
					/* NO. */
					list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, " 		");

					/* NAME */
					list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, " 		"); 

					/* TIME */
					list_set_field_content_by_ascstr(p_list, (u16)(start + i), 3, " 		"); 
				}
			}
		}
	}

	return SUCCESS;
}

RET_CODE open_epg_find(u32 para1, u32 para2)
{
  control_t *p_mask, *p_root;
  control_t *p_top_line, *p_bottom_line;
  control_t *p_detail, *p_preview;
  control_t *p_mbox_condition;
  control_t *p_keyboard_cont;
  control_t *p_pgname, *p_short;
  control_t *p_ctrl;
  control_t *p_list_pg;
  control_t *p_help;
  control_t *p_evtname, *p_evttime;
  control_t *p_genre_cont, *p_genre_text, *p_genre_list_left, *p_genre_list_right;
  control_t *p_lsbar, *p_rsbar;
  control_t *p_td_cont, *p_time_list, *p_day_list;
  control_t *p_td_time_text, *p_td_day_text, *p_td_ok_text, *p_td_cancel_text;
  u16 i;
  list_xstyle_t epg_find_list_item_rstyle =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_ITEM_1_HL,
    RSI_ITEM_1_SH,
    RSI_ITEM_1_HL,   
  };

  list_xstyle_t epg_find_list_field_fstyle =
  {
    FSI_GRAY,
    FSI_WHITE,
    FSI_BLACK,
    FSI_BLACK,
    FSI_BLACK,
  };

  list_xstyle_t epg_find_list_field_rstyle =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };
  
  list_field_attr_t epg_find_event_attr[EPG_FIND_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_DEC,
      60, 0, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//no.
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      280, 60, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//name
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      100, 260, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//date
    { LISTFIELD_TYPE_UNISTR| STL_LEFT | STL_VCENTER,
      170, 440, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//time   
  };

  list_field_attr_t epg_find_genre_llist_attr[EPG_FIND_GENRE_LLIST_FIELD] =
  {
    { LISTFIELD_TYPE_ICON,
      40, 0, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//no.
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      250, 40, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//mode
  };

  list_field_attr_t epg_find_genre_rlist_attr[EPG_FIND_GENRE_RLIST_FIELD] =
  {
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      290, 0, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//no.
  };

  list_field_attr_t epg_find_time_list_attr[EPG_FIND_TIME_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      290, 0, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//no.
  };

  list_field_attr_t epg_find_day_list_attr[EPG_FIND_DAY_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
      290, 0, 0, &epg_find_list_field_rstyle, &epg_find_list_field_fstyle },//no.
  };

  epg_find_create_epg_list();

  //Create
  p_root = ui_comm_root_create(ROOT_ID_EPG_FIND, 
    0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, RSC_INVALID_ID, IDS_TIMER_SET);
  if(p_root == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_root, epg_find_cont_keymap);
  ctrl_set_proc(p_root, epg_find_cont_proc);

  //set p_mask proc for recieving keybodard notify
  p_mask = fw_find_root_by_id(ROOT_ID_EPG_FIND);
  if(p_mask != NULL)
  {
    ctrl_set_proc(p_mask, epg_find_mask_proc);
  }
  
  //top line
  p_top_line = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_TOP_LINE, 
    EPG_FIND_TOP_LINE_X, EPG_FIND_TOP_LINE_Y, EPG_FIND_TOP_LINE_W, EPG_FIND_TOP_LINE_H, 
    p_root, 0);
  ctrl_set_rstyle(p_top_line, RSI_IGNORE, 
    RSI_IGNORE, RSI_IGNORE);

  //bottom line
  p_bottom_line = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_BOTTOM_LINE, 
    EPG_FIND_BOTTOM_LINE_X, EPG_FIND_BOTTOM_LINE_Y, EPG_FIND_BOTTOM_LINE_W, EPG_FIND_BOTTOM_LINE_H, 
    p_root, 0);
  ctrl_set_rstyle(p_bottom_line, RSI_IGNORE, 
    RSI_IGNORE, RSI_IGNORE);

  //detail information
  p_detail = ctrl_create_ctrl(CTRL_CONT, IDC_EPG_FIND_DETAIL, EPG_FIND_DETAIL_X, 
    EPG_FIND_DETAIL_Y, EPG_FIND_DETAIL_W, EPG_FIND_DETAIL_H, p_root, 0);
  ctrl_set_rstyle(p_detail, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);

  p_pgname = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_PG, 
    EPG_FIND_DETAIL_PGX, EPG_FIND_DETAIL_PGY, EPG_FIND_DETAIL_PGW, EPG_FIND_DETAIL_PGH, p_detail, 0);
  ctrl_set_rstyle(p_pgname, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_pgname, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_pgname, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_pgname, TEXT_STRTYPE_UNICODE);

  p_evtname = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_EVTNAME, 
    EPG_FIND_DETAIL_EVTNX, EPG_FIND_DETAIL_EVTNY, EPG_FIND_DETAIL_EVTNW, EPG_FIND_DETAIL_EVTNH, p_detail, 0);
  ctrl_set_rstyle(p_evtname, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_evtname, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_evtname, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_evtname, TEXT_STRTYPE_UNICODE);

  p_evttime = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_EVTTIME, 
    EPG_FIND_DETAIL_EVTTX, EPG_FIND_DETAIL_EVTTY, EPG_FIND_DETAIL_EVTTW, EPG_FIND_DETAIL_EVTTH, p_detail, 0);
  ctrl_set_rstyle(p_evttime, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_evttime, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_evttime, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_evttime, TEXT_STRTYPE_UNICODE);
  
  p_short = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_SHORT_EVT, 
    EPG_FIND_DETAIL_SHORTX, EPG_FIND_DETAIL_SHORTY, EPG_FIND_DETAIL_SHORTW, EPG_FIND_DETAIL_SHORTH, p_detail, 0);
  ctrl_set_rstyle(p_short, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_short, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_short, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_short, TEXT_STRTYPE_UNICODE);

  //pg type
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_TYPE, 
    EPG_FIND_DETAIL_TYPEX, EPG_FIND_DETAIL_TYPEY, EPG_FIND_DETAIL_TYPEW, EPG_FIND_DETAIL_TYPEH, p_detail, 0);
  ctrl_set_rstyle(p_ctrl, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_ctrl, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

  //scramble
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_SCRAMBLE, 
    EPG_FIND_DETAIL_SCRAMBLEX, EPG_FIND_DETAIL_SCRAMBLEY, EPG_FIND_DETAIL_SCRAMBLEW, EPG_FIND_DETAIL_SCRAMBLEH, p_detail, 0);
  ctrl_set_rstyle(p_ctrl, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_ctrl, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //age limit
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_AGE, 
    EPG_FIND_DETAIL_AGEX, EPG_FIND_DETAIL_AGEY, EPG_FIND_DETAIL_AGEW, EPG_FIND_DETAIL_AGEH, p_detail, 0);
  ctrl_set_rstyle(p_ctrl, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_ctrl, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  //reservation
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_DETAIL_RESERVE, 
    EPG_FIND_DETAIL_RESERVEX, EPG_FIND_DETAIL_RESERVEY, EPG_FIND_DETAIL_RESERVEW, EPG_FIND_DETAIL_RESERVEH, p_detail, 0);
  ctrl_set_rstyle(p_ctrl, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL, RSI_EPG_FIND_DETAIL);
  text_set_font_style(p_ctrl, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX, FSI_EPG_FIND_INFO_MBOX);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);


  //preview window
  p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_EPG_FIND_PREVIEW, EPG_FIND_PREV_X, 
    EPG_FIND_PREV_Y, EPG_FIND_PREV_W, EPG_FIND_PREV_H, p_root, 0);
  ctrl_set_rstyle(p_preview, RSI_EPG_FIND_PREV, RSI_EPG_FIND_PREV, RSI_EPG_FIND_PREV);  

  //condition
  p_mbox_condition = ctrl_create_ctrl(CTRL_MBOX, IDC_EPG_FIND_CONDITION, EPG_FIND_CONDITION_MBOX_X, 
    EPG_FIND_CONDITION_MBOX_Y, EPG_FIND_CONDITION_MBOX_W, EPG_FIND_CONDITION_MBOX_H, p_root, 0);
	ctrl_set_rstyle(p_mbox_condition, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT);
	ctrl_set_keymap(p_mbox_condition, epg_find_condition_mbox_keymap);
  ctrl_set_proc(p_mbox_condition, epg_find_condition_mbox_proc);
  ctrl_set_mrect(p_mbox_condition, 0, 0, EPG_FIND_CONDITION_MBOX_W, EPG_FIND_CONDITION_MBOX_H);
  mbox_enable_string_mode(p_mbox_condition, TRUE);
  mbox_set_count(p_mbox_condition, 7, 1, 7);
  mbox_set_item_interval(p_mbox_condition, 0, 0);
  mbox_set_item_rstyle(p_mbox_condition, RSI_ITEM_1_HL, RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_mbox_condition, FSI_BLACK, FSI_WHITE, FSI_GRAY);
  mbox_set_string_offset(p_mbox_condition, 0, 0);
  mbox_set_string_align_type(p_mbox_condition, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_mbox_condition, MBOX_STRTYPE_UNICODE);
  mbox_set_content_by_ascstr(p_mbox_condition, 0, "Keyword");
  mbox_set_content_by_ascstr(p_mbox_condition, 1, "");
	mbox_set_content_by_ascstr(p_mbox_condition, 2, "Genre");
  mbox_set_content_by_ascstr(p_mbox_condition, 3, "All");
	mbox_set_content_by_ascstr(p_mbox_condition, 4, "Time&Day");
  mbox_set_content_by_ascstr(p_mbox_condition, 5, "EveryTime EveryDay");
	mbox_set_content_by_ascstr(p_mbox_condition, 6, "Search");
	
	mbox_set_item_status(p_mbox_condition, 0 , MBOX_ITEM_DISABLED);
	mbox_set_item_status(p_mbox_condition, 2 , MBOX_ITEM_DISABLED);
	mbox_set_item_status(p_mbox_condition, 4 , MBOX_ITEM_DISABLED);

	mbox_set_focus(p_mbox_condition, 1);
	
  //epg_find list
  p_list_pg = ctrl_create_ctrl(CTRL_LIST, IDC_EPG_FIND_LIST_EVENT, EPG_FIND_LIST_EVENTX, 
    EPG_FIND_LIST_EVENTY, EPG_FIND_LIST_EVENTW, EPG_FIND_LIST_EVENTH, p_root, 0);
  ctrl_set_rstyle(p_list_pg, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST);
  ctrl_set_keymap(p_list_pg, epg_find_list_event_keymap);
  ctrl_set_proc(p_list_pg, epg_find_list_event_proc);
  ctrl_set_mrect(p_list_pg, 4, 4, (EPG_FIND_LIST_EVENTW - 4), (EPG_FIND_LIST_EVENTH - 4));
  list_set_item_interval(p_list_pg, EPG_FIND_VGAP);
  list_set_item_rstyle(p_list_pg, &epg_find_list_item_rstyle);
  list_enable_select_mode(p_list_pg, TRUE);
  list_set_select_mode(p_list_pg, LIST_SINGLE_SELECT);
  list_set_count(p_list_pg, (u16)g_evt_num, EPG_FIND_LIST_PAGE);
  list_set_field_count(p_list_pg, EPG_FIND_LIST_FIELD, EPG_FIND_LIST_PAGE);
  list_set_focus_pos(p_list_pg, 0);
  //list_select_item(p_list_pg, pg_pos);
  list_set_update(p_list_pg, epg_find_list_event_update, 0);
 
  for (i = 0; i < EPG_FIND_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list_pg, (u8)i, (u32)(epg_find_event_attr[i].attr), (u16)(epg_find_event_attr[i].width),
                        (u16)(epg_find_event_attr[i].left), (u8)(epg_find_event_attr[i].top));
    list_set_field_rect_style(p_list_pg, (u8)i, epg_find_event_attr[i].rstyle);
    list_set_field_font_style(p_list_pg, (u8)i, epg_find_event_attr[i].fstyle);
  }
  epg_find_list_event_update(p_list_pg, list_get_valid_pos(p_list_pg), EPG_FIND_LIST_PAGE, 0);

  //help
  p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_EPG_FIND_HELP, 
    EPG_FIND_HELP_X, EPG_FIND_HELP_Y, 
    EPG_FIND_HELP_W, EPG_FIND_HELP_H, p_root, 0);
  ctrl_set_rstyle(p_help, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT);
  ctrl_set_mrect(p_help, 20, 0, (EPG_FIND_HELP_W - 20), EPG_FIND_HELP_H);
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

  //keyboard
  p_keyboard_cont = ui_keyboard_create(p_root, IDC_EPG_FIND_KEYBAORD, 
    EPG_FIND_EDIT_CONTX, EPG_FIND_EDIT_CONTY, EPG_FIND_EDIT_CONTW, 
    EPG_FIND_EDIT_CONTH, ROOT_ID_EPG_FIND);
  ctrl_set_sts(p_keyboard_cont, OBJ_STS_HIDE);

  //genre container
  p_genre_cont = ctrl_create_ctrl(CTRL_CONT, IDC_EPG_FIND_GENRE, 
    EPG_FIND_GENRE_CONTX, EPG_FIND_GENRE_CONTY, 
    EPG_FIND_GENRE_CONTW, EPG_FIND_GENRE_CONTH, p_root, 0);
  ctrl_set_rstyle(p_genre_cont, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT);
  //genre title
  p_genre_text = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_GENRE_TITLE, 
    EPG_FIND_GENRE_TITLEX, EPG_FIND_GENRE_TITLEY, 
    EPG_FIND_GENRE_TITLEW, EPG_FIND_GENRE_TITLEH, p_genre_cont, 0);
  text_set_font_style(p_genre_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_genre_text, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_genre_text, "Genre");
  //genre left list
  p_genre_list_left = ctrl_create_ctrl(CTRL_LIST, IDC_EPG_FIND_GENRE_LLIST, 
    EPG_FIND_GENRE_LLISTX, EPG_FIND_GENRE_LLISTY, 
    EPG_FIND_GENRE_LLISTW, EPG_FIND_GENRE_LLISTH, p_genre_cont, 0);
  ctrl_set_rstyle(p_genre_list_left, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST);
  ctrl_set_keymap(p_genre_list_left, epg_find_genre_llist_keymap);
  ctrl_set_proc(p_genre_list_left, epg_find_genre_llist_proc);
  ctrl_set_mrect(p_genre_list_left, 4, 4, (EPG_FIND_GENRE_LLISTW - 4), (EPG_FIND_GENRE_LLISTH - 4));
  list_set_item_interval(p_genre_list_left, EPG_FIND_VGAP);
  list_set_item_rstyle(p_genre_list_left, &epg_find_list_item_rstyle);
  list_enable_select_mode(p_genre_list_left, TRUE);
  list_set_select_mode(p_genre_list_left, LIST_SINGLE_SELECT);
  list_set_count(p_genre_list_left, MAX_GENRE_LLIST_COUNT, EPG_FIND_GENRE_LLIST_PAGE);
  list_set_field_count(p_genre_list_left, EPG_FIND_GENRE_LLIST_FIELD, EPG_FIND_GENRE_LLIST_PAGE);
  list_set_focus_pos(p_genre_list_left, 0);
  //list_select_item(p_list_pg, pg_pos);
  list_set_update(p_genre_list_left, epg_find_genre_llist_update, 0);
 
  for (i = 0; i < EPG_FIND_GENRE_LLIST_FIELD; i++)
  {
    list_set_field_attr(p_genre_list_left, (u8)i, (u32)(epg_find_genre_llist_attr[i].attr), (u16)(epg_find_genre_llist_attr[i].width),
                        (u16)(epg_find_genre_llist_attr[i].left), (u8)(epg_find_genre_llist_attr[i].top));
    list_set_field_rect_style(p_genre_list_left, (u8)i, epg_find_genre_llist_attr[i].rstyle);
    list_set_field_font_style(p_genre_list_left, (u8)i, epg_find_genre_llist_attr[i].fstyle);
  }
  epg_find_genre_llist_update(p_genre_list_left, list_get_valid_pos(p_genre_list_left), EPG_FIND_GENRE_LLIST_PAGE, 0);

  //left scroll bar
  p_lsbar = ctrl_create_ctrl(CTRL_SBAR, IDC_EPG_FIND_GENRE_LSBAR, EPG_FIND_GENRE_LSBARX,
                            EPG_FIND_GENRE_LSBARY, EPG_FIND_GENRE_LSBARW, EPG_FIND_GENRE_LSBARH, p_genre_cont, 0);
  ctrl_set_rstyle(p_lsbar, RSI_EPG_FIND_SBAR, RSI_EPG_FIND_SBAR, RSI_EPG_FIND_SBAR);
  sbar_set_autosize_mode(p_lsbar, 1);
  sbar_set_direction(p_lsbar, 0);
  sbar_set_mid_rstyle(p_lsbar, RSI_EPG_FIND_SBAR_MID, RSI_EPG_FIND_SBAR_MID,
                     RSI_EPG_FIND_SBAR_MID);
  ctrl_set_mrect(p_lsbar, 0, 0, EPG_FIND_GENRE_LSBARW, EPG_FIND_GENRE_LSBARH);
  list_set_scrollbar(p_genre_list_left, p_lsbar);

  //genre right list
  p_genre_list_right = ctrl_create_ctrl(CTRL_LIST, IDC_EPG_FIND_GENRE_RLIST, 
    EPG_FIND_GENRE_RLISTX, EPG_FIND_GENRE_RLISTY, 
    EPG_FIND_GENRE_RLISTW, EPG_FIND_GENRE_RLISTH, p_genre_cont, 0);
  ctrl_set_rstyle(p_genre_list_right, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST);
  ctrl_set_keymap(p_genre_list_right, epg_find_genre_rlist_keymap);
  ctrl_set_proc(p_genre_list_right, epg_find_genre_rlist_proc);
  ctrl_set_mrect(p_genre_list_right, 4, 4, (EPG_FIND_GENRE_RLISTW - 4), (EPG_FIND_GENRE_RLISTH - 4));
  list_set_item_interval(p_genre_list_right, EPG_FIND_VGAP);
  list_set_item_rstyle(p_genre_list_right, &epg_find_list_item_rstyle);
  list_enable_select_mode(p_genre_list_right, TRUE);
  list_set_select_mode(p_genre_list_right, LIST_SINGLE_SELECT);
  list_set_count(p_genre_list_right, MAX_GENRE_RLIST_COUNT, EPG_FIND_GENRE_RLIST_PAGE);
  list_set_field_count(p_genre_list_right, EPG_FIND_GENRE_RLIST_FIELD, EPG_FIND_GENRE_RLIST_PAGE);
  list_set_focus_pos(p_genre_list_right, 0);
  //list_select_item(p_list_pg, pg_pos);
  list_set_update(p_genre_list_right, epg_find_genre_rlist_update, 0);
 
  for (i = 0; i < EPG_FIND_GENRE_RLIST_FIELD; i++)
  {
    list_set_field_attr(p_genre_list_right, (u8)i, (u32)(epg_find_genre_rlist_attr[i].attr), (u16)(epg_find_genre_rlist_attr[i].width),
                        (u16)(epg_find_genre_rlist_attr[i].left), (u8)(epg_find_genre_rlist_attr[i].top));
    list_set_field_rect_style(p_genre_list_right, (u8)i, epg_find_genre_rlist_attr[i].rstyle);
    list_set_field_font_style(p_genre_list_right, (u8)i, epg_find_genre_rlist_attr[i].fstyle);
  }
  epg_find_genre_rlist_update(p_genre_list_right, list_get_valid_pos(p_genre_list_right), EPG_FIND_GENRE_RLIST_PAGE, 0);

  //right scroll bar
  p_rsbar = ctrl_create_ctrl(CTRL_SBAR, IDC_EPG_FIND_GENRE_RSBAR, EPG_FIND_GENRE_RSBARX,
                            EPG_FIND_GENRE_RSBARY, EPG_FIND_GENRE_RSBARW, EPG_FIND_GENRE_RSBARH, p_genre_cont, 0);
  ctrl_set_rstyle(p_rsbar, RSI_EPG_FIND_SBAR, RSI_EPG_FIND_SBAR, RSI_EPG_FIND_SBAR);
  sbar_set_autosize_mode(p_rsbar, 1);
  sbar_set_direction(p_rsbar, 0);
  sbar_set_mid_rstyle(p_rsbar, RSI_EPG_FIND_SBAR_MID, RSI_EPG_FIND_SBAR_MID,
                     RSI_EPG_FIND_SBAR_MID);
  ctrl_set_mrect(p_rsbar, 0, 0, EPG_FIND_GENRE_RSBARW, EPG_FIND_GENRE_RSBARH);
  list_set_scrollbar(p_genre_list_right, p_rsbar);
  //hide genre
  ctrl_set_sts(p_genre_cont, OBJ_STS_HIDE);

  ///////////////////////////////////////
  //time&day container
  p_td_cont = ctrl_create_ctrl(CTRL_CONT, IDC_EPG_FIND_TD, 
    EPG_FIND_TD_CONTX, EPG_FIND_TD_CONTY, 
    EPG_FIND_TD_CONTW, EPG_FIND_TD_CONTH, p_root, 0);
  ctrl_set_rstyle(p_td_cont, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT, RSI_EPG_FIND_LIST_CONT);
  //time text
  p_td_time_text = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_TD_TIME_TEXT, 
    EPG_FIND_TD_TIME_TEXTX, EPG_FIND_TD_TIME_TEXTY, 
    EPG_FIND_TD_TIME_TEXTW, EPG_FIND_TD_TIME_TEXTH, p_td_cont, 0);
  ctrl_set_rstyle(p_td_time_text, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_td_time_text, epg_find_td_time_keymap);
  ctrl_set_proc(p_td_time_text, epg_find_td_time_proc);
  text_set_font_style(p_td_time_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_td_time_text, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_td_time_text, "time");
  //day text
  p_td_day_text = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_TD_DAY_TEXT, 
    EPG_FIND_TD_DAY_TEXTX, EPG_FIND_TD_DAY_TEXTY, 
    EPG_FIND_TD_DAY_TEXTW, EPG_FIND_TD_DAY_TEXTH, p_td_cont, 0);
  ctrl_set_rstyle(p_td_day_text, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_td_day_text, epg_find_td_day_keymap);
  ctrl_set_proc(p_td_day_text, epg_find_td_day_proc);
  text_set_font_style(p_td_day_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_td_day_text, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_td_day_text, "day");
  //ok text
  p_td_ok_text = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_TD_OK_TEXT, 
    EPG_FIND_TD_OK_TEXTX, EPG_FIND_TD_OK_TEXTY, 
    EPG_FIND_TD_OK_TEXTW, EPG_FIND_TD_OK_TEXTH, p_td_cont, 0);
  ctrl_set_rstyle(p_td_ok_text, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_td_ok_text, epg_find_td_ok_keymap);
  ctrl_set_proc(p_td_ok_text, epg_find_td_ok_proc);
  text_set_font_style(p_td_ok_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_td_ok_text, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_td_ok_text, "OK");
  //cancel
  p_td_cancel_text = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_FIND_TD_CANCEL_TEXT, 
    EPG_FIND_TD_CANCEL_TEXTX, EPG_FIND_TD_CANCEL_TEXTY, 
    EPG_FIND_TD_CANCEL_TEXTW, EPG_FIND_TD_CANCEL_TEXTH, p_td_cont, 0);
  ctrl_set_rstyle(p_td_cancel_text, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_td_cancel_text, epg_find_td_cancel_keymap);
  ctrl_set_proc(p_td_cancel_text, epg_find_td_cancel_proc);
  text_set_font_style(p_td_cancel_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_td_cancel_text, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_td_cancel_text, "Cancel");
  //time list
  p_time_list = ctrl_create_ctrl(CTRL_LIST, IDC_EPG_FIND_TD_TIME_LIST, 
    EPG_FIND_TD_TIME_LISTX, EPG_FIND_TD_TIME_LISTY, 
    EPG_FIND_TD_TIME_LISTW, EPG_FIND_TD_TIME_LISTH, p_td_cont, 0);
  ctrl_set_rstyle(p_time_list, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST);
  ctrl_set_keymap(p_time_list, epg_find_td_time_list_keymap);
  ctrl_set_proc(p_time_list, epg_find_td_time_list_proc);
  ctrl_set_mrect(p_time_list, 4, 4, (EPG_FIND_TD_TIME_LISTW - 4), (EPG_FIND_TD_TIME_LISTH - 4));
  list_set_item_interval(p_time_list, EPG_FIND_VGAP);
  list_set_item_rstyle(p_time_list, &epg_find_list_item_rstyle);
  list_enable_select_mode(p_time_list, TRUE);
  list_set_select_mode(p_time_list, LIST_SINGLE_SELECT);
  list_set_count(p_time_list, MAX_TIME_LIST_COUNT, EPG_FIND_TIME_LIST_PAGE);
  list_set_field_count(p_time_list, EPG_FIND_TIME_LIST_FIELD, EPG_FIND_TIME_LIST_PAGE);
  list_set_focus_pos(p_time_list, 0);
  //list_select_item(p_list_pg, pg_pos);
  list_set_update(p_time_list, epg_find_time_list_update, 0);
 
  for (i = 0; i < EPG_FIND_GENRE_LLIST_FIELD; i++)
  {
    list_set_field_attr(p_time_list, (u8)i, (u32)(epg_find_time_list_attr[i].attr), (u16)(epg_find_time_list_attr[i].width),
                        (u16)(epg_find_time_list_attr[i].left), (u8)(epg_find_time_list_attr[i].top));
    list_set_field_rect_style(p_time_list, (u8)i, epg_find_time_list_attr[i].rstyle);
    list_set_field_font_style(p_time_list, (u8)i, epg_find_time_list_attr[i].fstyle);
  }
  epg_find_time_list_update(p_time_list, list_get_valid_pos(p_time_list), EPG_FIND_TIME_LIST_PAGE, 0);
  ctrl_set_sts(p_time_list, OBJ_STS_HIDE);

  //day list
  p_day_list = ctrl_create_ctrl(CTRL_LIST, IDC_EPG_FIND_TD_DAY_LIST, 
    EPG_FIND_TD_DAY_LISTX, EPG_FIND_TD_DAY_LISTY, 
    EPG_FIND_TD_DAY_LISTW, EPG_FIND_TD_DAY_LISTH, p_td_cont, 0);
  ctrl_set_rstyle(p_day_list, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST, RSI_EPG_FIND_LIST);
  ctrl_set_keymap(p_day_list, epg_find_td_day_list_keymap);
  ctrl_set_proc(p_day_list, epg_find_td_day_list_proc);
  ctrl_set_mrect(p_day_list, 4, 4, (EPG_FIND_TD_DAY_LISTW - 4), (EPG_FIND_TD_DAY_LISTH - 4));
  list_set_item_interval(p_day_list, EPG_FIND_VGAP);
  list_set_item_rstyle(p_day_list, &epg_find_list_item_rstyle);
  list_enable_select_mode(p_day_list, TRUE);
  list_set_select_mode(p_day_list, LIST_SINGLE_SELECT);
  list_set_count(p_day_list, MAX_DAY_LIST_COUNT, EPG_FIND_DAY_LIST_PAGE);
  list_set_field_count(p_day_list, EPG_FIND_DAY_LIST_FIELD, EPG_FIND_DAY_LIST_PAGE);
  list_set_focus_pos(p_day_list, 0);
  //list_select_item(p_list_pg, pg_pos);
  list_set_update(p_day_list, epg_find_day_list_update, 0);
 
  for (i = 0; i < EPG_FIND_GENRE_LLIST_FIELD; i++)
  {
    list_set_field_attr(p_day_list, (u8)i, (u32)(epg_find_day_list_attr[i].attr), (u16)(epg_find_day_list_attr[i].width),
                        (u16)(epg_find_day_list_attr[i].left), (u8)(epg_find_day_list_attr[i].top));
    list_set_field_rect_style(p_day_list, (u8)i, epg_find_day_list_attr[i].rstyle);
    list_set_field_font_style(p_day_list, (u8)i, epg_find_day_list_attr[i].fstyle);
  }
  epg_find_day_list_update(p_day_list, list_get_valid_pos(p_day_list), EPG_FIND_DAY_LIST_PAGE, 0);
  ctrl_set_sts(p_day_list, OBJ_STS_HIDE);

  //hide time&day
  ctrl_set_sts(p_td_cont, OBJ_STS_HIDE);
  ///////////////////////////////////////

  ctrl_default_proc(p_mbox_condition, MSG_GETFOCUS, 0, 0);
  epg_find_set_detail(p_detail, FALSE);  
  ctrl_paint_ctrl(p_root, FALSE);
  
  //epg_find_play_pg(list_get_focus_pos(p_list_pg));
  
  return SUCCESS;
}

static RET_CODE on_epg_find_ready(control_t *p_cont, u16 msg,
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
  //  epg_find_set_detail(ctrl_get_child_by_id(p_cont, IDC_EPG_FIND_DETAIL), TRUE);
  //}

  return SUCCESS;
}


static RET_CODE on_epg_find_time_update(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  epg_find_set_detail(ctrl_get_child_by_id(p_cont, IDC_EPG_FIND_DETAIL), TRUE);  
  return SUCCESS;
}

static RET_CODE on_epg_find_condition_word_changed(control_t *p_mask, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_condition_mbox;
  p_condition_mbox = ui_comm_root_get_ctrl( ROOT_ID_EPG_FIND, IDC_EPG_FIND_CONDITION);
  mbox_set_content_by_unistr(p_condition_mbox, 1, (u16 *)para1);

  uni_strcpy((u16 *)&g_keyword, (u16 *)para1);
  ctrl_paint_ctrl(p_condition_mbox, TRUE);
  return SUCCESS;
}

static RET_CODE on_epg_find_hide_keyboard(control_t *p_mask, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_keyboard_cont, *p_condition_mbox;
  p_keyboard_cont = ui_comm_root_get_ctrl( ROOT_ID_EPG_FIND, IDC_EPG_FIND_KEYBAORD);
  p_condition_mbox = ui_comm_root_get_ctrl( ROOT_ID_EPG_FIND, IDC_EPG_FIND_CONDITION);

  ctrl_set_sts(p_keyboard_cont, OBJ_STS_HIDE);
  ctrl_process_msg(p_keyboard_cont, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_condition_mbox, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_condition_mbox, OBJ_STS_SHOW);
  ctrl_process_msg(p_condition_mbox, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_parent(p_keyboard_cont), TRUE);
  return SUCCESS;
}

static RET_CODE on_epg_find_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  ui_enable_chk_pwd(TRUE);
  
  return ERR_NOFEATURE;
}

static RET_CODE on_epg_find_pg_list_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  //u16 focus = list_get_focus_pos(p_list);
  //return manage_open_menu(ROOT_ID_TIMER, focus, 3);
  return SUCCESS;
}

static RET_CODE on_epg_find_pg_list_left(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_conditon_mbox;
  RET_CODE ret = SUCCESS;

  p_cont = p_list->p_parent;

  p_conditon_mbox = ctrl_get_child_by_id(p_cont, IDC_EPG_FIND_CONDITION);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_paint_ctrl(p_list, TRUE);

  ctrl_set_attr(p_conditon_mbox, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_conditon_mbox, OBJ_STS_SHOW);
  ctrl_process_msg(p_conditon_mbox, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_conditon_mbox, TRUE);

  return ret;
}

static void epg_find_set_genre(control_t *p_list)
{
  control_t *p_genre_cont;
  control_t *p_genre_llist;
  control_t *p_genre_rlist;
  u8 temppos1;
  u8 temppos2;

  p_genre_cont = p_list->p_parent;
  p_genre_llist = ctrl_get_child_by_id(p_genre_cont, IDC_EPG_FIND_GENRE_LLIST);
  p_genre_rlist = ctrl_get_child_by_id(p_genre_cont, IDC_EPG_FIND_GENRE_RLIST);

  temppos1 = (u8)list_get_focus_pos(p_genre_llist);
  temppos2 = (u8)list_get_focus_pos(p_genre_rlist);

  g_genre_level = ((temppos1<<4)|temppos2);
}

static RET_CODE on_epg_find_llist_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_root_cont, *p_genre_cont, *p_condition_cont;

  epg_find_set_genre(p_list);

  p_genre_cont = p_list->p_parent;
  p_root_cont = p_genre_cont->p_parent;
  p_condition_cont = ctrl_get_child_by_id(p_root_cont, IDC_EPG_FIND_CONDITION);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_genre_cont, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_genre_cont, OBJ_STS_HIDE);

  ctrl_set_attr(p_condition_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_condition_cont, OBJ_STS_SHOW);
  
  ctrl_process_msg(p_condition_cont, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_root_cont, TRUE);
  return ret;
}

static RET_CODE on_epg_find_llist_focus_change(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_genre_rlist;

  p_genre_rlist = ctrl_get_child_by_id(p_list->p_parent, IDC_EPG_FIND_GENRE_RLIST);

  ret = list_class_proc(p_list, msg, para1, para2);

  list_set_focus_pos(p_genre_rlist, 0);
  epg_find_genre_rlist_update(p_genre_rlist, list_get_valid_pos(p_genre_rlist), EPG_FIND_GENRE_RLIST_PAGE, 0);
  ctrl_paint_ctrl(p_genre_rlist, TRUE);

  return ret;
}

static RET_CODE on_epg_find_llist_lose_focus(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_genre_rlist;

  p_genre_rlist = ctrl_get_child_by_id(p_list->p_parent, IDC_EPG_FIND_GENRE_RLIST);

  //ctrl_set_attr(p_list, OBJ_ATTR_INACTIVE);
  list_class_proc(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_paint_ctrl(p_list, TRUE);

  ctrl_set_attr(p_genre_rlist, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_genre_rlist, OBJ_STS_SHOW);
  list_class_proc(p_genre_rlist, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_genre_rlist, TRUE);

  return ret;
}


static RET_CODE on_epg_find_rlist_lose_focus(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_genre_llist;

  p_genre_llist = ctrl_get_child_by_id(p_list->p_parent, IDC_EPG_FIND_GENRE_LLIST);

  //ctrl_set_attr(p_list, OBJ_ATTR_INACTIVE);
  list_class_proc(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_paint_ctrl(p_list, TRUE);

  ctrl_set_attr(p_genre_llist, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_genre_llist, OBJ_STS_SHOW);
  list_class_proc(p_genre_llist, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_genre_llist, TRUE);

  return ret;
}

static RET_CODE on_epg_find_rlist_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_root_cont, *p_genre_cont, *p_condition_cont;

  epg_find_set_genre(p_list);

  p_genre_cont = p_list->p_parent;
  p_root_cont = p_genre_cont->p_parent;
  p_condition_cont = ctrl_get_child_by_id(p_root_cont, IDC_EPG_FIND_CONDITION);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_genre_cont, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_genre_cont, OBJ_STS_HIDE);

  ctrl_set_attr(p_condition_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_condition_cont, OBJ_STS_SHOW);
  ctrl_process_msg(p_condition_cont, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_root_cont, TRUE);
  return ret;
}

#if 0
static void epg_find_get_time_area(utc_time_t *p_start, utc_time_t *p_end)
{
  utc_time_t g_time_focus = {0};
  utc_time_t t_time = {0};
  
  time_get(&g_time_focus, TRUE);

  g_time_focus.minute = g_time_focus.minute/30*30;
  g_time_focus.second = 0;

  //t_time.day = (u8)(g_time_step/48);
  //t_time.hour = (u8)(g_time_step%48/2);
  //t_time.minute = (u8)(g_time_step%48%2*30);

  time_add(&g_time_focus, &t_time);

  memcpy(p_start, &g_time_focus, sizeof(utc_time_t));

  t_time.day = 0;
  t_time.hour = 2;
  t_time.minute = 0;
  
  time_add(&g_time_focus, &t_time);

  memcpy(p_end, &g_time_focus, sizeof(utc_time_t));
}
#endif
static RET_CODE on_epg_find_condition_mbox_select(control_t *p_mbox, 
  u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_genre_cont;
  control_t *p_genre_llist;
  control_t *p_td_cont;
  control_t *p_td_time;
  control_t *p_event_list;

  u16 focus = mbox_get_focus(p_mbox);

  p_cont = p_mbox->p_parent;

  switch(focus)
  {
    case 1:
      ui_keyboard_enter_edit(p_cont, IDC_EPG_FIND_KEYBAORD);
      ui_keyboard_setfocus(p_cont, IDC_EPG_FIND_KEYBAORD);
      ui_keyboard_hl(p_cont, IDC_EPG_FIND_KEYBAORD);
      break;

    case 3:
      p_genre_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_FIND_GENRE);
      p_genre_llist = ctrl_get_child_by_id(p_genre_cont, IDC_EPG_FIND_GENRE_LLIST);

      ctrl_process_msg(p_genre_llist, MSG_GETFOCUS, 0, 0);
      ctrl_set_attr(p_genre_cont, OBJ_ATTR_ACTIVE);
      ctrl_set_sts(p_genre_cont, OBJ_STS_SHOW);
      ctrl_paint_ctrl(p_genre_cont, TRUE);
      break;

    case 5:
      p_td_cont = ctrl_get_child_by_id(p_cont, IDC_EPG_FIND_TD);
      p_td_time= ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_TIME_TEXT);

      ctrl_process_msg(p_td_time, MSG_GETFOCUS, 0, 0);
      ctrl_set_attr(p_td_cont, OBJ_ATTR_ACTIVE);
      ctrl_set_sts(p_td_cont, OBJ_STS_SHOW);
      ctrl_paint_ctrl(p_td_cont, TRUE);
      break;

    case 6:
      p_event_list = ctrl_get_child_by_id(p_cont, IDC_EPG_FIND_LIST_EVENT);

      epg_find_create_epg_list();

      list_set_count(p_event_list, (u16)g_evt_num, EPG_FIND_LIST_PAGE);
      list_set_focus_pos(p_event_list, 0);

      epg_find_list_event_update(p_event_list, list_get_valid_pos(p_event_list), EPG_FIND_LIST_PAGE, 0);
      ctrl_paint_ctrl(p_event_list, TRUE);
      break;
  }
  return SUCCESS;
}

static RET_CODE on_epg_find_condition_mbox_right(control_t *p_mbox, 
  u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_pg_list;
  RET_CODE ret = SUCCESS;

  p_cont = p_mbox->p_parent;

  p_pg_list = ctrl_get_child_by_id(p_cont, IDC_EPG_FIND_LIST_EVENT);

  ctrl_process_msg(p_mbox, MSG_LOSTFOCUS, para1, para2);
  ctrl_paint_ctrl(p_mbox, TRUE);

  ctrl_set_attr(p_pg_list, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_pg_list, OBJ_STS_SHOW);
  ctrl_process_msg(p_pg_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_pg_list, TRUE);

  return ret;
}

static RET_CODE on_epg_td_time_select(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_time_list;

  p_td_cont = p_text->p_parent;

  p_td_time_list = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_TIME_LIST);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, para1, para2);
  ctrl_paint_ctrl(p_text, TRUE);

  ctrl_set_attr(p_td_time_list, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_time_list, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_time_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_time_list, TRUE);

  return ret;
}

static RET_CODE on_epg_td_time_up(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_ok;

  p_td_cont = p_text->p_parent;

  p_td_ok = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_OK_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_ok, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_ok, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_ok, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_time_down(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_day;

  p_td_cont = p_text->p_parent;

  p_td_day = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_DAY_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_day, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_day, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_day, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_day_select(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_day_list;

  p_td_cont = p_text->p_parent;

  p_td_day_list = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_DAY_LIST);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, para1, para2);
  ctrl_paint_ctrl(p_text, TRUE);

  ctrl_set_attr(p_td_day_list, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_day_list, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_day_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_day_list, TRUE);

  return ret;
}

static RET_CODE on_epg_td_day_up(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_time;

  p_td_cont = p_text->p_parent;

  p_td_time = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_TIME_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_time, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_time, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_time, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_day_down(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_ok;

  p_td_cont = p_text->p_parent;

  p_td_ok = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_OK_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_ok, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_ok, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_ok, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_ok_select(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_root_cont, *p_td_cont, *p_condition_cont;

  control_t *p_td_time;
  control_t *p_td_day;

  p_td_cont = p_text->p_parent;
  p_root_cont = p_td_cont->p_parent;
  p_condition_cont = ctrl_get_child_by_id(p_root_cont, IDC_EPG_FIND_CONDITION);
  p_td_time= ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_TIME_LIST);
  p_td_day= ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_DAY_LIST);

  g_time_focus = (u8)list_get_focus_pos(p_td_time);
  g_day_focus = (u8)list_get_focus_pos(p_td_day);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_td_cont, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_td_cont, OBJ_STS_HIDE);

  ctrl_set_attr(p_condition_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_attr(p_condition_cont, OBJ_STS_SHOW);
  ctrl_process_msg(p_condition_cont, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_root_cont, TRUE);
  return ret;
}

static RET_CODE on_epg_td_ok_up(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_day;

  p_td_cont = p_text->p_parent;

  p_td_day = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_DAY_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_day, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_day, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_day, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_ok_down(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_time;

  p_td_cont = p_text->p_parent;

  p_td_time = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_TIME_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_time, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_time, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_time, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_ok_left(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_cancel;

  p_td_cont = p_text->p_parent;

  p_td_cancel = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_CANCEL_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_cancel, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_cancel, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_cancel, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_ok_right(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  return on_epg_td_ok_left(p_text, msg, para1, para2);
}

static RET_CODE on_epg_td_cancel_select(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_root_cont, *p_td_cont, *p_condition_cont;

  p_td_cont = p_text->p_parent;
  p_root_cont = p_td_cont->p_parent;
  p_condition_cont = ctrl_get_child_by_id(p_root_cont, IDC_EPG_FIND_CONDITION);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_td_cont, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_td_cont, OBJ_STS_HIDE);

  ctrl_set_attr(p_condition_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_condition_cont, OBJ_STS_SHOW);
  ctrl_process_msg(p_condition_cont, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_root_cont, TRUE);
  return ret;
}

static RET_CODE on_epg_td_cancel_up(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_day;

  p_td_cont = p_text->p_parent;

  p_td_day = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_DAY_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_day, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_day, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_day, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_cancel_down(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_time;

  p_td_cont = p_text->p_parent;

  p_td_time = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_TIME_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_time, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_time, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_time, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_cancel_left(control_t *p_text, 
	u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_ok;

  p_td_cont = p_text->p_parent;

  p_td_ok = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_OK_TEXT);

  ctrl_process_msg(p_text, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_td_ok, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_ok, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_ok, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_td_cancel_right(control_t *p_text, 
  u16 msg, u32 para1, u32 para2)
{
  return on_epg_td_cancel_left(p_text, msg, para1, para2);
}


static RET_CODE on_epg_find_td_time_list_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_time_text;

  p_td_cont = p_list->p_parent;

  p_td_time_text = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_TIME_TEXT);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_list, OBJ_STS_HIDE);

  ctrl_set_attr(p_td_time_text, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_time_text, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_time_text, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_find_td_day_list_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_td_cont, *p_td_day_text;

  p_td_cont = p_list->p_parent;

  p_td_day_text = ctrl_get_child_by_id(p_td_cont, IDC_EPG_FIND_TD_DAY_TEXT);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_list, OBJ_STS_HIDE);

  ctrl_set_attr(p_td_day_text, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_td_day_text, OBJ_STS_SHOW);
  ctrl_process_msg(p_td_day_text, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_td_cont, TRUE);

  return ret;
}

static RET_CODE on_epg_find_yellow(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = manage_back_to_menu(ROOT_ID_SCHEDULE, (u32)ROOT_ID_EPG_FIND, 0);

  return ret;
}

static RET_CODE on_epg_find_red(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = manage_back_to_menu(ROOT_ID_EPG, (u32)ROOT_ID_EPG_FIND, 0);

  return ret;
}

static RET_CODE on_epg_find_green(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = manage_back_to_menu(ROOT_ID_EPG, (u32)ROOT_ID_EPG_FIND, 0);

  return ret;
}


BEGIN_MSGPROC(epg_find_mask_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_INPUT_CHANGED, on_epg_find_condition_word_changed)
  ON_COMMAND(MSG_HIDE_KEYBOARD, on_epg_find_hide_keyboard)
END_MSGPROC(epg_find_mask_proc, ui_comm_root_proc)

BEGIN_KEYMAP(epg_find_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_EPG, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_BLUE, MSG_BLUE)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
END_KEYMAP(epg_find_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(epg_find_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EPG_READY, on_epg_find_ready)
  ON_COMMAND(MSG_TIME_UPDATE, on_epg_find_time_update)
  ON_COMMAND(MSG_DESTROY, on_epg_find_destory)
  ON_COMMAND(MSG_YELLOW, on_epg_find_yellow)
  ON_COMMAND(MSG_GREEN, on_epg_find_green)
  ON_COMMAND(MSG_RED, on_epg_find_red)
END_MSGPROC(epg_find_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(epg_find_list_event_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(epg_find_list_event_keymap, NULL)

BEGIN_MSGPROC(epg_find_list_event_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_find_pg_list_select)
  ON_COMMAND(MSG_FOCUS_LEFT, on_epg_find_pg_list_left)
END_MSGPROC(epg_find_list_event_proc, list_class_proc)

BEGIN_KEYMAP(epg_find_condition_mbox_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(epg_find_condition_mbox_keymap, NULL)

BEGIN_MSGPROC(epg_find_condition_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_find_condition_mbox_select)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_epg_find_condition_mbox_right)
END_MSGPROC(epg_find_condition_mbox_proc, mbox_class_proc)

BEGIN_KEYMAP(epg_find_genre_llist_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(epg_find_genre_llist_keymap, NULL)

BEGIN_MSGPROC(epg_find_genre_llist_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_find_llist_select)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_epg_find_llist_lose_focus)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_find_llist_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_find_llist_focus_change)
  ON_COMMAND(MSG_PAGE_UP, on_epg_find_llist_focus_change)
  ON_COMMAND(MSG_PAGE_DOWN, on_epg_find_llist_focus_change)  
END_MSGPROC(epg_find_genre_llist_proc, list_class_proc)

BEGIN_KEYMAP(epg_find_genre_rlist_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(epg_find_genre_rlist_keymap, NULL)

BEGIN_MSGPROC(epg_find_genre_rlist_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_find_rlist_select)
  ON_COMMAND(MSG_FOCUS_LEFT, on_epg_find_rlist_lose_focus)
END_MSGPROC(epg_find_genre_rlist_proc, list_class_proc)

BEGIN_KEYMAP(epg_find_td_time_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(epg_find_td_time_list_keymap, NULL)

BEGIN_MSGPROC(epg_find_td_time_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_find_td_time_list_select)
END_MSGPROC(epg_find_td_time_list_proc, list_class_proc)

BEGIN_KEYMAP(epg_find_td_day_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(epg_find_td_day_list_keymap, NULL)

BEGIN_MSGPROC(epg_find_td_day_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_find_td_day_list_select)
END_MSGPROC(epg_find_td_day_list_proc, list_class_proc)

BEGIN_KEYMAP(epg_find_td_time_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(epg_find_td_time_keymap, NULL)

BEGIN_MSGPROC(epg_find_td_time_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_td_time_select)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_td_time_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_td_time_down)
END_MSGPROC(epg_find_td_time_proc, text_class_proc)

BEGIN_KEYMAP(epg_find_td_day_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(epg_find_td_day_keymap, NULL)

BEGIN_MSGPROC(epg_find_td_day_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_td_day_select)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_td_day_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_td_day_down)
END_MSGPROC(epg_find_td_day_proc, text_class_proc)

BEGIN_KEYMAP(epg_find_td_ok_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(epg_find_td_ok_keymap, NULL)

BEGIN_MSGPROC(epg_find_td_ok_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_td_ok_select)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_td_ok_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_td_ok_down)
  ON_COMMAND(MSG_FOCUS_LEFT, on_epg_td_ok_left)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_epg_td_ok_right)
END_MSGPROC(epg_find_td_ok_proc, text_class_proc)

BEGIN_KEYMAP(epg_find_td_cancel_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(epg_find_td_cancel_keymap, NULL)

BEGIN_MSGPROC(epg_find_td_cancel_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_epg_td_cancel_select)
  ON_COMMAND(MSG_FOCUS_UP, on_epg_td_cancel_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_epg_td_cancel_down)
  ON_COMMAND(MSG_FOCUS_LEFT, on_epg_td_cancel_left)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_epg_td_cancel_right)
END_MSGPROC(epg_find_td_cancel_proc, text_class_proc)


