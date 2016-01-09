/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_google_map.h"
//#include "html.h"
//#include "html_parse.h"
#include "ui_rename.h"
#include "ui_keyboard_v2.h"
#include "ui_edit_city.h"

enum select_area_ctl_id
{
  IDC_INVALID = 0,
  IDC_ROADMAP = 1,
  IDC_STATE ,
  IDC_COUNTRY,
  IDC_PRVIENCE,
  IDC_CITY,
  IDC_BEGIN,
  IDC_SEARCH_BOX,
  IDC_CITY_LIST,
};

enum map_ctl_id
{
  IDC_LEFT_AREA_CONT = 11,
  IDC_AREA_NAME_ITEM = 12,
  IDC_MAP_DETAIL_CONT = 22,
  IDC_HELP_BAR,
  IDC_MAP_DIRECT,
  IDC_MAP_SBAR,
  IDC_SBAR_CONT,
};

comm_dlg_data_t map_delete_dlg_data =
{
	ROOT_ID_INVALID,
	DLG_FOR_ASK | DLG_STR_MODE_STATIC,
	COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
	IDS_MSG_ASK_FOR_DEL,
	0,
};


rename_param_t search_box_param;
//static u16 new_search[MAX_FAV_NAME_LEN + 1];
u16 map_area_select_cont_keymap(u16 key);
RET_CODE map_area_select_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 map_cont_keymap(u16 key);
RET_CODE map_cont_proc(control_t *p_cont, u16 msg,u32 para1, u32 para2);
u16 search_box_keymap(u16 key);
RET_CODE search_box_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 map_sbar_keymap(u16 key);
RET_CODE map_sbar_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 map_area_list_keymap(u16 key);
RET_CODE map_area_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


/*
void search_box_update(void)
{
  control_t *p_edit_cont;
  p_edit_cont = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_GOOGLE_MAP, IDC_LEFT_AREA_CONT),IDC_SEARCH_BOX);
  ctrl_paint_ctrl(p_edit_cont, TRUE);
}

static RET_CODE on_edit_search_box(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;
  control_t *p_ebox;

  p_ebox = ctrl_get_parent(p_ctrl);
  param.uni_str = ui_comm_t9edit_get_content(p_ebox);
  param.type = search_box_param.type;
  param.max_len = DB_DVBS_MAX_NAME_LENGTH;
  param.cb = search_box_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}
*/


/* city list */
#define AREA_LIST_PAGE    9
#define AREA_LIST_FIELD   2
#define RSI_WEATHER_AREA_LIST RSI_PBACK

enum local_msg
{
  MSG_ADD_CITY = MSG_LOCAL_BEGIN + 850,
  MSG_EDIT_CITY,
  MSG_DELETE_CITY,
};

static list_xstyle_t area_list_item_rstyle =
{
  RSI_PBACK,
  RSI_ITEM_12_SH,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_HL,
};

static list_xstyle_t area_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

static list_xstyle_t area_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t area_list_attr[AREA_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    30, 9, 10, &area_list_field_rstyle,  &area_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    165, 40, 10, &area_list_field_rstyle,  &area_list_field_fstyle},
};



static u8 city_count = 0;
static BOOL b_add = FALSE;
static u16 city_name[10][32] ={{0},};
static u16 list_pos = 0;

static u8 cur_zoom = 0;
#define MAX_ZOOM_LEVEL 21

static RET_CODE check_network_conn()
{
  net_conn_stats_t eth_connt_stats = {0};
  RET_CODE ret = SUCCESS;
#ifndef WIN32
  eth_connt_stats = ui_get_net_connect_status();
  if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE)
     && (eth_connt_stats.is_gprs_conn == FALSE) && (eth_connt_stats.is_3g_conn == FALSE))
  {
     ret = ERR_FAILURE;  
     ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
  }
#endif
  return ret;
}

static RET_CODE on_map_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{

  manage_close_menu(ROOT_ID_GOOGLE_MAP, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_map_exit_all(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{

  ui_close_all_mennus();
  
  return SUCCESS;
}

static RET_CODE on_map_destroy(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{

   ui_google_map_stop();
   ui_google_map_release();  

   
   return ERR_NOFEATURE;
}

static RET_CODE city_list_update(control_t* p_list, u16 start, u16 size,
                                     u32 context)
{
  u16 cnt = list_get_count(p_list);
  u16 i;
  u8 asc_str[8];
  u16 city_names[32]={0};

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char*)asc_str, "%.d.", (u16)(start + i+1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      sys_status_get_city((u8)(i + start), city_names);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, city_names);
    }
  }
  return SUCCESS;
}

static RET_CODE load_city_list(control_t *p_ctrl)
{
  u16 focus = 0;
  control_t *p_list = NULL;
  u8 city_cnt = 0;
  u16 city_names[50] = {0};
  u8 asc_str[8] = {0};
  u8 i = 0;

  p_list = ui_comm_root_get_ctrl(ROOT_ID_GOOGLE_MAP, IDC_CITY_LIST);
  MT_ASSERT(p_list != NULL);
  city_cnt = sys_status_get_city_cnt();
  if(0 == city_cnt)
  {
    return SUCCESS;
  }

  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  
  city_count = city_cnt; 
  list_set_count(p_list, city_cnt, AREA_LIST_PAGE);
  
  for(i = 0; i < city_cnt; i++)
  {
    sys_status_get_city(i, city_names);
    sprintf((char*)asc_str, "%.d.", (u16)(i + 1));
    list_set_field_content_by_ascstr(p_list, (u16)(i), 0, asc_str);
    list_set_field_content_by_unistr(p_list, (u16)(i), 1,city_names);
  }
  
  list_set_focus_pos(p_list, focus);  
  
  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static RET_CODE on_map_area_select(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_main = NULL, *p_map_bar = NULL, *p_bar_cont = NULL;
  control_t *p_list = NULL;
  u16 item_idx = 0;
  //control_t *p_area_cont = NULL, *p_t9_edit = NULL;

  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }
  
  p_main = ctrl_get_parent(p_cont);
  if(p_main == NULL)
  {
    return ERR_FAILURE;
  }

  p_map_bar = ctrl_get_child_by_id(p_main, IDC_MAP_SBAR);
  if(p_map_bar == NULL)
  {
    return ERR_FAILURE;
  }

  p_bar_cont = ctrl_get_child_by_id(p_main, IDC_SBAR_CONT);
  if(p_bar_cont == NULL)
  {
    return ERR_FAILURE;
  }
  //start google map
  {   
	  rect_t rect = {MAP_DETAIL_CONT_X, 
             		   MAP_DETAIL_CONT_Y, 
              		 MAP_DETAIL_CONT_X + MAP_DETAIL_CONT_W, 
               	   MAP_DETAIL_CONT_Y + MAP_DETAIL_CONT_H};

	  mul_google_map_param_t map_cfg = {{0}};
	  u16 *p_addr = NULL; 
	  u32 addr_len = 0;

	  //memcpy(map_cfg.addr, "china,shanghai,xuhui,yishanlu", 256);  
      /**/
/*    p_area_cont = ctrl_get_child_by_id(p_main, IDC_LEFT_AREA_CONT);
    if(p_area_cont == NULL)
    {
       return ERR_FAILURE;
    }

    p_t9_edit = ctrl_get_child_by_id(p_area_cont, IDC_SEARCH_BOX);
	  if(p_t9_edit == NULL)
	  {
		  return ERR_FAILURE;
	  }
    
    p_addr = ui_comm_t9edit_get_content(p_t9_edit);
	  if(p_addr == NULL)
	  {
         return ERR_FAILURE;
	  }
*/
    p_list = ctrl_get_child_by_id(p_main, IDC_CITY_LIST);
    if(p_list == NULL)
    {
      return ERR_FAILURE;
    }

    item_idx = list_get_focus_pos(p_list);
    //list_select_item(p_list, item_idx);
    list_pos = item_idx;
    p_addr = (u16 *)list_get_field_content(p_list, item_idx, 1);
    if (NULL == p_addr)
    {
    	return ERR_FAILURE;
    }

    addr_len = uni_strlen(p_addr);
    if(addr_len == 0)
    {
         return ERR_FAILURE;
    }

    str_uni2asc(map_cfg.addr, p_addr);
    map_cfg.zoom = 16;
    memcpy(&map_cfg.map_rect, &rect, sizeof(rect_t));
    map_cfg.maptype = MAPTYPE_ROADMAP;
    map_cfg.format = FORMAT_JPG;   

    ui_google_map_stop();
    ui_google_map_start(&map_cfg);

  }

  cur_zoom = 16;

  ctrl_process_msg(p_cont, MSG_LOSTFOCUS, para1, para2); 
  ctrl_process_msg(p_map_bar, MSG_GETFOCUS, 0, 0); 

  sbar_set_current_pos(p_map_bar, MAX_ZOOM_LEVEL - 16);
  ctrl_paint_ctrl(p_bar_cont, TRUE);
  ctrl_paint_ctrl(p_map_bar,TRUE);
  
  ctrl_paint_ctrl(p_cont,TRUE);

  return SUCCESS;
}


static void add_map_city(void)
{
  edit_city_param_t param;
  char *name = "\0";

  b_add = 1;
  param.uni_str = (u16*) name;
  param.max_len = 32;
//  param.cb = edit_ip_update;
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.add_type = 1;

  manage_open_menu(ROOT_ID_EDIT_CITY, (u32)&param, ROOT_ID_GOOGLE_MAP);
}

static void do_add_city(u16 *name)
{
  uni_strncpy((u16*)(city_name[city_count]), name, 32);
  //city_count ++;

  sys_status_set_city(city_count,name);
  city_count ++;
  sys_status_set_city_cnt(city_count);
  sys_status_save();
}

static void do_edit_city(u8 focus, u16 *name)
{
  uni_strncpy((u16*)(city_name[focus]), name, 32);
  sys_status_set_city(focus, name);
  sys_status_save();
}

static void do_delete_city(u16 focus,void *name)
{
  u16 i = 0;
  u16 city_names[32]={0};

  for(i =focus; i < city_count - 1; i++)
  {
     sys_status_get_city((u8)(i+1), city_names);
     sys_status_set_city((u8)i, city_names);
  }

  city_count --;
  sys_status_set_city_cnt(city_count);
  sys_status_save();
}

static RET_CODE on_map_add_city(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{

  if(city_count > 8)
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_SPACE_IS_FULL, NULL, 0);
  }
  else
  {
    add_map_city();
  }
  
  return SUCCESS;
}

static RET_CODE on_map_edit_city(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_map_delete_city(control_t *ctrl, u16 msg,
                                      u32 para1, u32 para2)
{
  u16 focus;
  control_t  *p_list;
  dlg_ret_t dlg_ret;
  u16 city_names[32]={0};


  if(city_count > 0)
  {
	  dlg_ret = ui_comm_dlg_open2(&map_delete_dlg_data);

  	if(dlg_ret == DLG_RET_YES)
  	{
      focus = list_get_focus_pos(ctrl);
      sys_status_get_ip_path_set((u8)focus, city_names);
      do_delete_city(focus, (u16 *)(city_names));

      p_list = ui_comm_root_get_ctrl(ROOT_ID_GOOGLE_MAP, IDC_CITY_LIST);
      MT_ASSERT(p_list != NULL);
      list_set_count(p_list, city_count, AREA_LIST_PAGE);
      if(focus < city_count)
      {
       list_set_focus_pos(p_list, focus);
      }
      else
      {
       list_set_focus_pos(p_list, city_count - 1);
      }
      city_list_update(p_list, list_get_valid_pos(p_list), AREA_LIST_PAGE, 0);
      ctrl_paint_ctrl(p_list, TRUE);
   	}
  }

  return SUCCESS;
}


static RET_CODE on_map_name_check(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}



static RET_CODE on_map_name_update(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u16 *name = (u16 *)para1;
  u16 focus, total;
  control_t  *p_list;

  p_list = ui_comm_root_get_ctrl(ROOT_ID_GOOGLE_MAP, IDC_CITY_LIST);
  MT_ASSERT(p_list != NULL);

  if(b_add)
  {
    //add city
    do_add_city(name);

    total = city_count;
    focus = city_count - 1;
    list_set_count(p_list, total, AREA_LIST_PAGE);
    list_set_focus_pos(p_list, focus);
    city_list_update(p_list, list_get_valid_pos(p_list), AREA_LIST_PAGE, 0);
  }
  else
  {
    //edit city
    focus = list_get_focus_pos(p_list);
    do_edit_city((u8)focus,name);
    city_list_update(p_list, list_get_valid_pos(p_list), AREA_LIST_PAGE, 0);
  }

  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  
  return SUCCESS;
}




/*!
  map operation
   
  */
static RET_CODE on_map_move_left(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }
  
  ui_google_map_move(MAP_MOVE_LEFT);
   
  return SUCCESS;
}

static RET_CODE on_map_move_right(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }
  
  ui_google_map_move(MAP_MOVE_RIGHT);
   
  return SUCCESS;
}

static RET_CODE on_map_move_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }
 
  ui_google_map_move(MAP_MOVE_UP);
   
  return SUCCESS;
}

static RET_CODE on_map_move_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }
  
  ui_google_map_move(MAP_MOVE_DOWN);
   
  return SUCCESS;
}

static RET_CODE on_map_zoom_in(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  control_t *p_sbar_cont = NULL;
  
  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }

  if(cur_zoom < 21)
  {
      ui_google_map_zoom(MAP_ZOOM_IN);

      cur_zoom ++;
      p_menu = ctrl_get_parent(p_ctrl);
      p_sbar_cont = ctrl_get_child_by_id(p_menu, IDC_SBAR_CONT);
      ctrl_paint_ctrl(p_sbar_cont, TRUE);
       
      return ERR_NOFEATURE;
  }
  else
  {
     return ERR_FAILURE;
  }
}

static RET_CODE on_map_zoom_out(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  control_t *p_sbar_cont = NULL;
  
  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }

  if(cur_zoom > 0)
  {
    ui_google_map_zoom(MAP_ZOOM_OUT);

    cur_zoom --;
    p_menu = ctrl_get_parent(p_ctrl);
    p_sbar_cont = ctrl_get_child_by_id(p_menu, IDC_SBAR_CONT);
    ctrl_paint_ctrl(p_sbar_cont, TRUE);
  }
  else
  {
    return ERR_FAILURE;
  }
     
   
  return ERR_NOFEATURE;
}

RET_CODE on_map_return(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = NULL, *p_main = NULL;
  p_main = ctrl_get_parent(p_cont);
  if(p_main == NULL)
  {
     return ERR_FAILURE;
  }

  p_list = ctrl_get_child_by_id(p_main, IDC_CITY_LIST);
  if(p_list == NULL)
  {
    return ERR_FAILURE;
  }
  
  ctrl_process_msg(p_cont, MSG_LOSTFOCUS, para1, para2); 
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0); 

  list_set_focus_pos(p_list, list_pos);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

/*
RET_CODE on_map_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  return SUCCESS;
}
*/

static RET_CODE on_map_change_addr(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
/*    rect_t rect = {MAP_DETAIL_CONT_X + MAP_DIRECT_W, 
              	   MAP_DETAIL_CONT_Y, 
                   MAP_DETAIL_CONT_X + MAP_DETAIL_CONT_W - MAP_DIRECT_W, 
              	   MAP_DETAIL_CONT_Y + MAP_DETAIL_CONT_H};

    mul_google_map_param_t map_cfg = {{0}};
    memcpy(map_cfg.addr, "shanghai,xuhui,yishanlu", 256);
    map_cfg.zoom = 16;
    memcpy(&map_cfg.map_rect, &rect, sizeof(rect_t));
    map_cfg.maptype = MAPTYPE_ROADMAP;
    map_cfg.format = FORMAT_JPG;   

    ui_google_map_stop();
    ui_google_map_start(&map_cfg);
*/
    return SUCCESS;
}


RET_CODE open_google_map(u32 para1, u32 para2)
{
  control_t *p_menu, *p_area_cont, *p_map_detail_cont, *p_help_bar;//,*p_ctrl;
  //control_t *p_select_ctrl[MAP_LEFT_ITEM_CNT];
  control_t *p_map_sbar = NULL, *p_map_direct = NULL, *p_sbar_cont = NULL;
  control_t *p_area_list = NULL;
  u16  i =0;// ,j = 0;
  comm_help_data_t help_data = 
  {
    6, 6,
    {IDS_MOVE, IDS_ADD, IDS_DEL, IDS_ZOOM_IN, IDS_ZOOM_OUT, IDS_RETURN},
    {IM_HELP_ARROW, IM_HELP_RED, IM_HELP_GREEN, IM_HELP_YELLOW, IM_HELP_BLUE, IM_HELP_MENU}
  };
 /*
  u16 area_str[MAP_AREA_CNT] =
  { 
    IDS_STATE,
    IDS_COUNTRY,
    IDS_PROVIENCE,
    IDS_CITY,
  };
*/  

  p_menu = ui_comm_root_create_netmenu(ROOT_ID_GOOGLE_MAP, 0, IM_INDEX_GOOGLE_BANNER, IDS_GOOGLE_MAP);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_menu, map_cont_keymap);
  ctrl_set_proc(p_menu, map_cont_proc);
  //left area container
  p_area_cont =	ctrl_create_ctrl(CTRL_CONT, IDC_LEFT_AREA_CONT, MAP_AREA_CONT_X,
                            		 MAP_AREA_CONT_Y, MAP_AREA_CONT_W,
                            		 MAP_AREA_CONT_H, p_menu,
                            		 0);


  ctrl_set_keymap(p_area_cont, map_area_select_cont_keymap);
  ctrl_set_proc(p_area_cont, map_area_select_cont_proc);
  ctrl_set_rstyle(p_area_cont, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  #if 0
  //country provience city infor 
  for(i=0; i<MAP_AREA_CNT; i++)
  {
      p_ctrl = ctrl_create_ctrl((u8*)CTRL_TEXT, (u8)(IDC_AREA_NAME_ITEM+i),
                            20, 5 + i*100+2*COMM_CTRL_H, 150, 40, p_area_cont, 0);
      ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      text_set_font_style(p_ctrl, FSI_BLACK, FSI_BLACK, FSI_BLACK);
      text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
      text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_ctrl,area_str[i]);
  }
  for(i = 0;i < MAP_LEFT_ITEM_CNT;i++)
  {
    switch(i)
    {
      case 0:
        p_select_ctrl[i] = ui_comm_select_create(p_area_cont, IDC_ROADMAP+i, SELECT_ITEM_X, 5 + i*100+40, SELECT_ITEM_LW, SELECT_ITEM_RW);
        ui_comm_select_set_param(p_select_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           MAP_AREA_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < MAP_AREA_CNT; j++)
        {
          ui_comm_select_set_content(p_select_ctrl[i], (u8)j, area_str[j]);
        }
        ui_comm_select_set_focus(p_select_ctrl[i], 0);
        break;
      case 1:
        p_select_ctrl[i] = ui_comm_select_create(p_area_cont, IDC_ROADMAP+i, SELECT_ITEM_X, 5 + i*100+40, SELECT_ITEM_LW, SELECT_ITEM_RW);
        ui_comm_select_set_param(p_select_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           MAP_AREA_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < MAP_AREA_CNT; j++)
        {
          ui_comm_select_set_content(p_select_ctrl[i], (u8)j, area_str[j]);
        }
        ui_comm_select_set_focus(p_select_ctrl[i], 1);
        break;
      case 2:
        p_select_ctrl[i] = ui_comm_select_create(p_area_cont, IDC_ROADMAP+i, SELECT_ITEM_X, 5 + i*100+40, SELECT_ITEM_LW, SELECT_ITEM_RW);
        ui_comm_select_set_param(p_select_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           MAP_AREA_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < MAP_AREA_CNT; j++)
        {
          ui_comm_select_set_content(p_select_ctrl[i], (u8)j, area_str[j]);
        }
        ui_comm_select_set_focus(p_select_ctrl[i], 0);
        break;
      case 3:
        p_select_ctrl[i] = ui_comm_select_create(p_area_cont, IDC_ROADMAP+i, SELECT_ITEM_X, 5+ i*100+40, SELECT_ITEM_LW, SELECT_ITEM_RW);
        ui_comm_select_set_param(p_select_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           MAP_AREA_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < MAP_AREA_CNT; j++)
        {
          ui_comm_select_set_content(p_select_ctrl[i], (u8)j, area_str[j]);
        }
        ui_comm_select_set_focus(p_select_ctrl[i], 1);
        break;
      case 4:
        p_select_ctrl[i] = ui_comm_select_create(p_area_cont, IDC_ROADMAP+i, SELECT_ITEM_X, 5 + i*100+40, SELECT_ITEM_LW, SELECT_ITEM_RW);
        ui_comm_select_set_param(p_select_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           MAP_AREA_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < MAP_AREA_CNT; j++)
        {
          ui_comm_select_set_content(p_select_ctrl[i], (u8)j, area_str[j]);
        }
        ui_comm_select_set_focus(p_select_ctrl[i], 1);
        break;
      case 5:
        p_select_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_ROADMAP+i, 70, 5 + 4*100+40+50+10, 120, 50, p_area_cont, 0);
        ctrl_set_rstyle(p_select_ctrl[i], RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
        text_set_align_type(p_select_ctrl[i], STL_CENTER | STL_VCENTER);
        text_set_font_style(p_select_ctrl[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
        text_set_content_type(p_select_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_select_ctrl[i], IDS_START);
        break;
      case 6:
        p_select_ctrl[i] = ui_comm_t9edit_create(p_area_cont, IDC_ROADMAP+i, SELECT_ITEM_X, 5 + 
4*100+40+50+10+50+10, 20, 215, ROOT_ID_GOOGLE_MAP);
        ctrl_set_rstyle(p_select_ctrl[i], RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
        ui_comm_ctrl_set_keymap(p_select_ctrl[i], search_box_keymap);
        ui_comm_ctrl_set_proc(p_select_ctrl[i], search_box_proc);
        search_box_param.max_len = DB_DVBS_MAX_NAME_LENGTH;
        search_box_param.type = KB_INPUT_TYPE_SENTENCE;
        search_box_param.uni_str = ui_comm_t9edit_get_content(p_select_ctrl[i]);

        ui_comm_t9edit_set_content_by_ascstr(p_select_ctrl[i], "shanghai");
        break;
    }
    
    ctrl_set_related_id(p_select_ctrl[i],
                        0,                       /* left */
                        (u8)((i - 1 + MAP_LEFT_ITEM_CNT)
                             % MAP_LEFT_ITEM_CNT + 1),    /* up */
                        0,                       /* right */
                        (u8)((i + 1) % MAP_LEFT_ITEM_CNT + 1));    /* down */
    
   }
   #endif


  //left city list
  p_area_list = ctrl_create_ctrl(CTRL_LIST, IDC_CITY_LIST, MAP_AREA_LIST_X,
                            		 MAP_AREA_LIST_Y, MAP_AREA_LIST_W+30,
                            		 MAP_AREA_LIST_H, p_menu, 0);

  ctrl_set_rstyle(p_area_list, RSI_ITEM_11_SH, RSI_ITEM_11_SH, RSI_ITEM_11_SH);
  ctrl_set_keymap(p_area_list, map_area_list_keymap);
  ctrl_set_proc(p_area_list, map_area_list_proc);

  ////
  ctrl_set_mrect(p_area_list, MAP_AREA_LIST_MID_L, MAP_AREA_LIST_MID_T,
                          MAP_AREA_LIST_MID_W , 
                          MAP_AREA_LIST_MID_H );
  list_set_item_interval(p_area_list, MAP_AREA_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_area_list, &area_list_item_rstyle);
  list_enable_select_mode(p_area_list, TRUE);
  list_set_count(p_area_list, 0, AREA_LIST_PAGE);
  list_set_field_count(p_area_list, AREA_LIST_FIELD, AREA_LIST_PAGE);
  list_set_focus_pos(p_area_list, 0);
  list_set_update(p_area_list,city_list_update, 0);
  
  for (i = 0; i < AREA_LIST_FIELD; i++)
  {
    list_set_field_attr(p_area_list, (u8)i, (u32)(area_list_attr[i].attr),
                        (u16)(area_list_attr[i].width),
                        (u16)(area_list_attr[i].left),
                        (u8)(area_list_attr[i].top));
    list_set_field_rect_style(p_area_list, (u8)i, area_list_attr[i].rstyle);
    list_set_field_font_style(p_area_list, (u8)i, area_list_attr[i].fstyle);
  }
  
  load_city_list(p_area_list);

  
  //right map detail info container
  p_map_detail_cont =
			ctrl_create_ctrl(CTRL_CONT, IDC_MAP_DETAIL_CONT, MAP_DETAIL_CONT_X,
			MAP_DETAIL_CONT_Y, MAP_DETAIL_CONT_W,
			MAP_DETAIL_CONT_H, p_menu,
			0);
  ctrl_set_rstyle(p_map_detail_cont, RSI_TV, RSI_TV, RSI_TV);

 //bmp direction
  p_map_direct= ctrl_create_ctrl(CTRL_BMAP, IDC_MAP_DIRECT,
                                 MAP_DIRECT_X, MAP_DIRECT_Y,
                                 MAP_DIRECT_W, MAP_DIRECT_H,
                                 p_menu, 0);
  //ctrl_set_rstyle(p_map_direct, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  bmap_set_content_by_id(p_map_direct, IM_ICON_GOOGLEMAP_ARROW); 

 //scrollbar container
  p_sbar_cont = ctrl_create_ctrl(CTRL_CONT, IDC_SBAR_CONT, MAP_SBAR_X,
                                 MAP_SBAR_Y, MAP_SBAR_W,  MAP_SBAR_H, p_menu, 0);
  ctrl_set_rstyle(p_sbar_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
 
  //scrollbar                               
  p_map_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_MAP_SBAR, MAP_SBAR_X,
                                 MAP_SBAR_Y, MAP_SBAR_W,  MAP_SBAR_H, p_menu, 0);
  ctrl_set_rstyle(p_map_sbar, RSI_MAP_SBAR_BG, RSI_MAP_SBAR_BG, RSI_MAP_SBAR_BG);
  sbar_set_autosize_mode(p_map_sbar, 0);
  sbar_set_direction(p_map_sbar, 0);
  sbar_set_mid_rstyle(p_map_sbar, RSI_MAP_SBAR_MID,
                     RSI_MAP_SBAR_MID, RSI_MAP_SBAR_MID);                    
  ctrl_set_mrect(p_map_sbar, 0, 16,
                    16,
                    MAP_SBAR_H - 16);

  sbar_set_count(p_map_sbar, 1, 22);
  sbar_set_current_pos(p_map_sbar, 0);
  ctrl_set_keymap(p_map_sbar, map_sbar_keymap);
  ctrl_set_proc(p_map_sbar, map_sbar_proc);
  
//bottom help bar
p_help_bar = ctrl_create_ctrl(CTRL_TEXT, IDC_HELP_BAR,
                   MAP_BOTTOM_HELP_X, MAP_BOTTOM_HELP_Y, MAP_BOTTOM_HELP_W,
                   MAP_BOTTOM_HELP_H, p_menu, 0);
  ctrl_set_rstyle(p_help_bar, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
  text_set_font_style(p_help_bar, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_help_bar, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_help_bar, TEXT_STRTYPE_UNICODE);
  ui_comm_help_create_ext(40, 0, MAP_BOTTOM_HELP_W-40, MAP_BOTTOM_HELP_H,  &help_data,  
p_help_bar);

  ctrl_default_proc(p_area_list, MSG_GETFOCUS, 0, 0);
  //ctrl_default_proc(p_map_sbar, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  ui_google_map_init();

  city_count = sys_status_get_city_cnt();
  
  return SUCCESS;

}


BEGIN_KEYMAP(map_area_select_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
 // ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(map_area_select_cont_keymap, NULL)

BEGIN_MSGPROC(map_area_select_cont_proc, cont_class_proc)
 // ON_COMMAND(MSG_SELECT, on_map_area_select)
END_MSGPROC(map_area_select_cont_proc, cont_class_proc)

BEGIN_KEYMAP(map_cont_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(map_cont_keymap, NULL)

BEGIN_MSGPROC(map_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_map_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_map_exit_all)
  ON_COMMAND(MSG_DESTROY, on_map_destroy)
END_MSGPROC(map_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(search_box_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(search_box_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(search_box_proc, ui_comm_t9_proc)
//ON_COMMAND(MSG_SELECT, on_edit_search_box)
//ON_COMMAND(MSG_NUMBER, on_edit_search_box)
END_MSGPROC(search_box_proc, ui_comm_t9_proc)


BEGIN_KEYMAP(map_sbar_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  
  ON_EVENT(V_KEY_YELLOW, MSG_DECREASE)
  ON_EVENT(V_KEY_BLUE, MSG_INCREASE)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(map_sbar_keymap, NULL)

BEGIN_MSGPROC(map_sbar_proc, sbar_class_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_map_move_left)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_map_move_right)
  ON_COMMAND(MSG_FOCUS_UP, on_map_move_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_map_move_down)
  ON_COMMAND(MSG_SELECT, on_map_change_addr) 
  
  ON_COMMAND(MSG_INCREASE, on_map_zoom_out)
  ON_COMMAND(MSG_DECREASE, on_map_zoom_in)  
  ON_COMMAND(MSG_EXIT, on_map_return)
  //ON_COMMAND(MUL_MAP_EVT_DRAW_END, on_map_draw_end)
END_MSGPROC(map_sbar_proc, sbar_class_proc)

//
BEGIN_KEYMAP(map_area_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  
  ON_EVENT(V_KEY_RED, MSG_ADD_CITY)
  ON_EVENT(V_KEY_GREEN, MSG_DELETE_CITY)
  ON_EVENT(V_KEY_YELLOW, MSG_EDIT_CITY)
END_KEYMAP(map_area_list_keymap, NULL)

BEGIN_MSGPROC(map_area_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_map_area_select)
  ON_COMMAND(MSG_ADD_CITY, on_map_add_city)
  ON_COMMAND(MSG_EDIT_CITY, on_map_edit_city)
  ON_COMMAND(MSG_DELETE_CITY, on_map_delete_city)
  ON_COMMAND(MSG_EDIT_CITY_CHECK, on_map_name_check)
  ON_COMMAND(MSG_EDIT_CITY_UPDATE, on_map_name_update)
END_MSGPROC(map_area_list_proc, list_class_proc)
