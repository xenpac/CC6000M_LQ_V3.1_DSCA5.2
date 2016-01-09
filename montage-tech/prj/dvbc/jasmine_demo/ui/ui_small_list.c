/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_small_list.h"
#include "ui_pause.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif
#ifdef ENABLE_ADS
#include "ui_ads_display.h"
#include "ui_ad_api.h"
#endif
enum small_list_control_id
{
  IDC_SMALL_LIST_LIST = 1,
  IDC_SMALL_LIST_TOP_LINE,
  IDC_SMALL_LIST_BOTTOM_LINE,
  IDC_SMALL_LIST_SBAR,
  IDC_SMALL_LIST_SAT,
  IDC_SMALL_LIST_ADS_CONT,
  IDC_SMALL_LIST_ADS_TEXT,
  IDC_SLIST_AZ,
};

typedef enum
{
  MS_NOR = 0, 
  MS_AZ,
  MS_AZ_NOR, 
}menu_state_t;

#define SLIST_T9_MAX_KEY        10
#define SLIST_T9_MAX_OPT        5

static u8 slist_hkey = 0;
static BOOL is_select_play = FALSE;
static u8 parent_vid = 0xFF;
static u8 sub_vid = 0xFF;
static u16 slist_t9_curn_key = 0xFFFF;
static u8 slist_t9_curn_bit = 0;
static BOOL is_az_select = FALSE;
static menu_state_t ms = MS_NOR;
static menu_state_t old_ms = MS_NOR;

enum hot_key_msg
{
  MSG_GROUP_UP = MSG_LOCAL_BEGIN + 850,
  MSG_GROUP_DOWN,
  MSG_BLUE,
};

static list_xstyle_t slist_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

static list_xstyle_t slist_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};


static list_xstyle_t slist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};


static list_field_attr_t slist_attr[SMALL_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    50, 6, 0, &slist_field_rstyle,  &slist_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    140, 56, 0, &slist_field_rstyle,  &slist_field_fstyle },
  { LISTFIELD_TYPE_ICON,
    24, 196, 0, &slist_field_rstyle,  &slist_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    24, 220, 0, &slist_field_rstyle,  &slist_field_fstyle},
  //{ LISTFIELD_TYPE_ICON,
    //24, 214, 0, &slist_field_rstyle,  &slist_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    24, 244, 0, &slist_field_rstyle,  &slist_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    24, 268, 0, &slist_field_rstyle,  &slist_field_fstyle}, 
};
u16 small_list_root_keymap(u16 key);
u16 small_list_keymap(u16 key);
RET_CODE small_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

RET_CODE small_list_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
  
u16 slist_az_keymap(u16 key);
RET_CODE slist_az_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

extern BOOL have_logic_number(void);

static RET_CODE slist_sat_cbox_update(control_t *p_cbox, u16 focus, u16 *p_str,
                         u16 max_length)
{
  u8 type;
  u16 pos, group = 0;
  u32 context;
//  u8 sat_num;

  switch(slist_hkey)
  {
    case V_KEY_OK:
      //group = sys_status_get_group_by_pos(focus);
      group  = sys_status_get_curn_group();
      break;
    case V_KEY_SAT:
      group = sys_status_get_group_by_pos(focus + 1);
      break;
    case V_KEY_FAV:
      //sat_num = (u8)sys_status_get_sat_group_num();
      //group = sys_status_get_group_by_pos(focus + 1 + sat_num);
      group  = sys_status_get_curn_group();
      break;
    default:
      MT_ASSERT(0);
  }
  
  sys_status_get_group_info(group, &type, &pos, &context);
  
  switch(type)
  {
    case GROUP_T_ALL:
      gui_get_string(IDS_ALL, p_str, 31);
      break;
    case GROUP_T_FAV:
      sys_status_get_fav_name((u8)group - DB_DVBS_MAX_SAT - 1, p_str);
      break;
    default:
      MT_ASSERT(0);  
  }

  return SUCCESS;
}        

static void slist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, img[4];
  u8 view_id = ui_dbase_get_pg_view_id();

  img[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_TV_FAV : 0;
  img[1] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0)? IM_TV_LOCK : 0;

 // img[2] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_SKP, FALSE)? IM_TV_SKIP: IM_TV_SKIP;

  for (i = 0; i < 2; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), img[i]);
  }
}

//load data from database, will be modified later
static RET_CODE load_small_data(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{
  dvbs_prog_node_t curn_prog;
  u16 i;
  //u16 uni_str[32];
  u8 asc_str[8];
  u16 pg_id, cnt = list_get_count(p_list);
  
  u16 width = 0;
  u16 strbuf[3] = {'.', '.', '.'};
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(
        ui_dbase_get_pg_view_id(),
        (u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &curn_prog);


      // NO. 
      #ifdef LCN_SWITCH_DS_JHC
      sprintf((char *)asc_str, "%.4d ", curn_prog.logical_num);
      #else
      if(have_logic_number())
        sprintf((char *)asc_str, "%.4d ", curn_prog.logical_num);
      else
        sprintf((char *)asc_str, "%.4d ", start + i + 1);
      #endif
      
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      // NAME 
      //ui_dbase_get_full_prog_name(&curn_prog, uni_str, 31);
      
      width = rsc_get_unistr_width(gui_get_rsc_handle(), curn_prog.name, FSI_WHITE);
      if(width > 140)
      {
        memcpy(&(curn_prog.name[8]), strbuf, sizeof(strbuf));
        curn_prog.name[11] = '\0';
      }
      
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, curn_prog.name); //pg name
      //$
      if(CUSTOMER_COUNTRY == COUNTRY_INDIA)
      {
      	   list_set_field_content_by_icon(p_list, (u16)(start + i), 4, curn_prog.is_scrambled ? IM_TV_MONEY_INDIA:0);	
       }
	else
	{
      	   list_set_field_content_by_icon(p_list, (u16)(start + i), 4, curn_prog.is_scrambled ? IM_TV_MONEY:0);
	}
      //HD
      list_set_field_content_by_icon(p_list, (u16)(start + i), 5, (curn_prog.service_type == 0x11)?IM_TV_HD:0);
      //set marks
      slist_set_list_item_mark(p_list, (u16)(start + i), pg_id);
    }
  }
  return SUCCESS;
}

static void play_in_small_list(u16 focus)
{
  u16 rid;
  dvbs_prog_node_t pg = {0};
  u8 curn_mode;
  u8 view_id;
  u32 context;
  u16 view_type;
  u16 group;

  if ((rid =
         db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                    focus)) != INVALIDID)
  {
    db_dvbs_get_pg_by_id(rid, &pg);
    curn_mode = sys_status_get_curn_prog_mode();
    if((curn_mode == CURN_MODE_TV && pg.tv_flag == 0) || (curn_mode == 
          CURN_MODE_RADIO&& pg.tv_flag == 1))
    {
      if(pg.tv_flag)
        sys_status_set_curn_prog_mode(CURN_MODE_TV);
      else
        sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
      sys_status_save();
      
       group = sys_status_get_curn_group();
      sys_status_get_curn_view_info(&view_type, &context);
      if(view_type == DB_DVBS_FAV_TV ||view_type == DB_DVBS_FAV_RADIO)
      {
        view_id = ui_dbase_create_view(DB_DVBS_FAV_ALL, group - 
                      DB_DVBS_MAX_SAT - 1, NULL);
      }
      else
      {
        view_id = ui_dbase_create_view((dvbs_view_t)view_type, context, NULL);
      }

      ui_dbase_set_pg_view_id(view_id);

      //close pause menu.
      ui_set_pause(FALSE);
    }
    
        /*fix Bug68779*/
    if(ui_is_chk_pwd())
    {
      if (!ui_is_pass_chkpwd(rid))
      {
        ui_reset_chkpwd();
      }
    }

    ui_play_prog(rid, FALSE);
  }
}

static void slist_reset_list(control_t *p_list)
{
  u8 vid = 0;
  u16 new_group;
  u16 list_cnt = 0;
  u16 curn_focus = 0;
  u16 curn_pgid = 0;

  curn_pgid = sys_status_get_curn_group_curn_prog_id();

  new_group = sys_status_get_curn_group();

  vid = ui_dbase_get_pg_view_id();

  list_cnt = db_dvbs_get_count(vid);

  curn_focus = db_dvbs_get_view_pos_by_id(vid, curn_pgid);

  //reset list count & focus.
  list_set_count(p_list, list_cnt, SMALL_LIST_PAGE);

  if(((curn_focus == INVALIDID) && (list_cnt))||(is_az_select))
  {
    is_az_select = FALSE;
    curn_focus = 0;
  }

  list_set_focus_pos(p_list, curn_focus);

  if(list_get_item_status(p_list, curn_focus) != LIST_ITEM_SELECTED)
  {
    list_class_proc(p_list, MSG_SELECT, 0, 0);
    load_small_data(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    play_in_small_list(curn_focus);
  }
  else
  {
    load_small_data(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list, TRUE);
  }
  ui_set_frontpanel_by_curn_pg();
}

u8 slist_create_view(u8 group_type, BOOL is_tv, u32 context)
{
  u8 view_id = 0;

  switch(group_type)
  {
    case GROUP_T_ALL:
      view_id = ui_dbase_create_view(is_tv ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, context, NULL);
      break;
      
    case GROUP_T_FAV:
        view_id = ui_dbase_create_view(DB_DVBS_FAV_ALL, context, NULL);
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  
  return view_id;
}

static RET_CODE on_slist_change_focus(control_t *p_list,
  u16 msg, u32 para1, u32 para2)
{
  list_class_proc(p_list, msg, 0, 0);
 
  #ifdef ENABLE_CA
  #ifndef ONLY1_CA_VER
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif
  #endif
  //play_in_small_list(list_get_focus_pos(p_list));  
    
  return SUCCESS;
}

static RET_CODE on_slist_change_group(control_t *p_list,
  u16 msg, u32 para1, u32 para2)
{
  control_t *p_cbox;
  u16 org_group = 0;
  u16 new_group = 0;
  u8 org_mode = 0;
  u8 group_type;
  u16 pos_in_set;
  u16 current_pg_id, pg_pos = 0xFFFF;
  u32 context = 0;
  u8 view_id = 0xFF;
  u16 cbox_focus = 0xFFFF;
  u16 total = 0;
  //if((sys_status_get_categories_count() - 5) == 0)
      //return SUCCESS;

  //change sat
  org_mode = sys_status_get_curn_prog_mode();
  org_group = sys_status_get_curn_group();

  // We get current program from all goup.
  sys_status_get_group_all_info(org_mode, &current_pg_id);
  switch(msg)
  {
    case MSG_GROUP_DOWN:
      if(org_group == (DB_DVBS_MAX_SAT+ sys_status_get_categories_count()))
      {
        new_group = 0;
      }
      else if(org_group == 0)
      {
        new_group =  DB_DVBS_MAX_SAT + 1;
      }
      else
      {
        new_group = org_group + 1;
      }
      break;
    case MSG_GROUP_UP:
      if(org_group == DB_DVBS_MAX_SAT + 1)
      {
        new_group = 0;
      }
      else if(org_group == 0)
      {
        new_group = (DB_DVBS_MAX_SAT + sys_status_get_categories_count());
      }
      else
      {
        new_group =  org_group - 1;
      }
      break;
    default:
      break;
  }
  sys_status_get_group_info(new_group, &group_type, &pos_in_set, &context);

  if(new_group != org_group)
  {
    switch(group_type)
    {
      case GROUP_T_ALL:
        view_id = ui_dbase_create_view(
          (org_mode == CURN_MODE_TV)?DB_DVBS_ALL_TV:DB_DVBS_ALL_RADIO, context, NULL);
        cbox_focus = sys_status_get_pos_by_group(new_group);
        break;
      case GROUP_T_FAV:
        view_id = ui_dbase_create_view(
          DB_DVBS_FAV_ALL, new_group - DB_DVBS_MAX_SAT - 1, NULL);
        cbox_focus = sys_status_get_fav_group_pos(new_group) - 1;
        break;
      default:
        return ERR_FAILURE;
    }

    /* fix Bug57070 */
    sys_status_set_curn_group_not_write_flash(new_group);

    ui_dbase_set_pg_view_id(view_id);

    pg_pos = db_dvbs_get_view_pos_by_id(view_id, current_pg_id);
    total = db_dvbs_get_count(view_id);
    list_set_count(p_list, total, SMALL_LIST_PAGE);
    if (pg_pos != INVALIDID)
    {
      list_set_focus_pos(p_list, pg_pos);
      list_select_item(p_list, pg_pos);
    }
    else
    {
      list_set_focus_pos(p_list, 0);
      list_select_item(p_list, 0);
    }
    load_small_data(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
    OS_PRINTF("total[%d], pg_pos[%d], current_pg_id:%d\n", total, pg_pos, current_pg_id);

    //reset cbox content(list title)
    p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_SMALL_LIST_SAT);
    OS_PRINTF("group [%d], type[%d], focus[%d]\n", new_group, group_type, cbox_focus);
    cbox_static_set_focus(p_cbox, cbox_focus);
    ctrl_paint_ctrl(p_cbox, TRUE);
    ctrl_paint_ctrl(p_list, TRUE);
    
    #ifdef ENABLE_CA
    #ifndef ONLY1_CA_VER
    cas_manage_finger_repaint();
    OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
    #endif
    #endif
    ui_set_front_panel_by_num(pg_pos + 1);
    //play_in_small_list(pg_pos);
  }        

  return SUCCESS;
}

static RET_CODE on_slist_save(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  //slist_restore_view();
  if(is_select_play)
  {
    is_select_play = FALSE;
  }
  else
  {
    ui_restore_view();
  }
  ui_enable_chk_pwd(TRUE);
  #ifndef WIN32 
  ui_close_ads_pic();
  #endif
  return SUCCESS;
}

static void slist_az_find(control_t *p_az_list, u16 *p_unistr, BOOL is_paint)
{
  control_t *p_cont, *p_list = NULL;
  u8 vid = 0;
  u16 total;
  char *string_ask= "*\0";
  
  p_cont = ctrl_get_parent(p_az_list);
  p_list = ctrl_get_child_by_id(p_cont, IDC_SMALL_LIST_LIST);  
  
  MT_ASSERT(p_unistr != NULL);

  db_dvbs_destroy_view(sub_vid);
  
  if(uni_strlen(p_unistr) != 0)
  {
   if(strcmp(string_ask,(char *)p_unistr))
    {
     db_dvbs_find(parent_vid, p_unistr, &sub_vid);
     ui_dbase_set_pg_view_id(sub_vid);
    }
   else
     ui_dbase_set_pg_view_id(parent_vid);
  }

  vid = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(vid);

  list_set_count(p_list, total, SMALL_LIST_PAGE);

  if(total > 0)
  {
    list_set_focus_pos(p_list, 0);
    load_small_data(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
  }    

  if(is_paint)
  {
    ctrl_paint_ctrl(p_list, TRUE);
  }
}

RET_CODE on_slist_state_process(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_az_list;
  u16 focus, total;
  u16 *p_unistr = NULL;
  u8 view_id;
  u8 group_type = 0;
  u16 pos_in_set = 0;
  u32 context = 0;
  u16 slist_group = 0;
  BOOL is_tv = 0;

  view_id = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(view_id);
  focus = list_get_focus_pos(p_list);
  
  p_cont = ctrl_get_parent(p_list);
  p_az_list = ctrl_get_child_by_id(p_cont, IDC_SLIST_AZ);
  
  is_tv = ((CURN_MODE_TV == sys_status_get_curn_prog_mode())?TRUE:FALSE);

  slist_group = sys_status_get_curn_group();
  sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);
  
  old_ms = ms;
  switch(msg)
  {
    case MSG_BLUE:
      view_id = slist_create_view(group_type, is_tv, context);
      
      parent_vid = view_id;
      
      ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0);
      
      ctrl_set_sts(p_az_list, OBJ_STS_SHOW);
      ctrl_process_msg(p_az_list, MSG_GETFOCUS, 0, 0);

      focus = list_get_focus_pos(p_az_list);
      p_unistr = (u16 *)list_get_field_content(p_az_list, focus, 0);
      
      MT_ASSERT(p_unistr != NULL);
      
      slist_az_find(p_az_list, p_unistr, TRUE);

      ctrl_paint_ctrl(p_az_list, TRUE);
      
      ms = MS_AZ;
      break;      
    case MSG_EXIT:
      switch(old_ms)
      {
        case MS_AZ:
          view_id = slist_create_view(group_type, is_tv, context);        
          ui_dbase_set_pg_view_id(view_id);
          
          ctrl_process_msg(p_az_list, MSG_LOSTFOCUS, 0, 0); 
          ctrl_set_sts(p_az_list, OBJ_STS_HIDE);
          ctrl_erase_ctrl(p_az_list);  
          ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
          
          ms = MS_NOR;
          break;
        case MS_NOR:
        case MS_AZ_NOR:
          ui_restore_view();
          ui_enable_chk_pwd(TRUE);
          #ifndef WIN32 
          #ifdef ENABLE_ADS
          pic_adv_stop();
          #else
          ui_close_ads_pic();
          #endif
          #endif
          manage_close_menu(ROOT_ID_SMALL_LIST, 0, 0);
          return SUCCESS;
          break;
      }
      break;
    case MSG_SELECT:
      switch(old_ms)
      {
        case MS_AZ:
          is_az_select = TRUE;
          ctrl_process_msg(p_az_list, MSG_LOSTFOCUS, 0, 0);
          ctrl_set_sts(p_az_list, OBJ_STS_HIDE);
          ctrl_erase_ctrl(p_az_list);
          ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
          ms = MS_AZ_NOR;
          break;
        case MS_NOR:
          list_select_item(p_list, list_get_focus_pos(p_list));
          play_in_small_list(list_get_focus_pos(p_list));
          //ui_cache_view();
          is_select_play = TRUE;
          ui_restore_view();
          break;
        case MS_AZ_NOR:
          //ui_cache_view();
          list_select_item(p_list, list_get_focus_pos(p_list));
          play_in_small_list(list_get_focus_pos(p_list));
          is_select_play = TRUE;
          ui_restore_view();
          ms = MS_NOR;
          break;
      }
      break;
    default:
      break;
  }
  
  if(old_ms != ms)
  {
    slist_reset_list(p_list);
  }

  if(is_select_play)
  {
    if(fw_find_root_by_id(ROOT_ID_PASSWORD) == NULL)
    {
      ui_close_all_mennus();
    }
  }
  return SUCCESS;
}

static RET_CODE on_slist_az_change_focus(control_t *p_az_list, u16 msg, u32 para1, u32 para2)
{
  u16 focus;
  u16 *p_unistr = NULL;
  control_t *p_cont;
  
  p_cont = ctrl_get_parent(p_az_list);
  
  list_class_proc(p_az_list, msg, 0, 0);
  
  focus = list_get_focus_pos(p_az_list);
  p_unistr = (u16 *)list_get_field_content(p_az_list, focus, 0);

  
  MT_ASSERT(p_unistr != NULL);
  
  slist_az_find(p_az_list, p_unistr, TRUE);

  return SUCCESS;
}

static RET_CODE on_slist_az_select(control_t *p_az_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_list = NULL;
  
  p_cont = ctrl_get_parent(p_az_list);
  p_list = ctrl_get_child_by_id(p_cont, IDC_SMALL_LIST_LIST);  
  
  if(db_dvbs_get_count(ui_dbase_get_pg_view_id()))
  {
    on_slist_state_process(p_list, MSG_SELECT, 0, 0);
  }
  
  return  SUCCESS;
}

static RET_CODE on_slist_az_cancle(control_t *p_az_list, u16 msg, u32 para1,
                            u32 para2)
{
  control_t *p_list;
  
  p_list = ctrl_get_child_by_id(ctrl_get_parent(p_az_list), IDC_SMALL_LIST_LIST);
  
  if(p_list != NULL)
  {
    on_slist_state_process(p_list, MSG_EXIT, 0, 0);
  }
  
  return  SUCCESS;
}

static u16 slist_t9_get_focus(u16 key)
{
  u8 slist_t9_opt[SLIST_T9_MAX_KEY][SLIST_T9_MAX_OPT] =
  {
    {'0'},
    {'1'},
    {'a', 'b', 'c', '2', },
    {'d', 'e', 'f', '3', },
    {'g', 'h', 'i', '4', },
    {'j', 'k', 'l', '5', },
    {'m', 'n', 'o', '6', },
    {'p', 'q', 'r', 's', '7'},
    {'t', 'u', 'v', '8', },
    {'w', 'x', 'y', 'z', '9'}
  };
  u8 slist_t9_bit_cnt[SLIST_T9_MAX_KEY] =
  {
    1,
    1,
    4,
    4,
    4,
    4,
    4,
    5,
    4,
    5,
  };
  u8 ch;
  u16 focus = 0;

  slist_t9_curn_bit %= slist_t9_bit_cnt[key];

  ch = slist_t9_opt[key][slist_t9_curn_bit];

  if(ch >= '0' && ch <= '9')
  {
    focus = 27 + ch - '0';
  }
  else
  {
    focus = ch - 'a' + 1;
  }

  return focus;
}

static RET_CODE slist_az_update(control_t* ctrl, u16 start,u16 size,
                                u32 context)
{
  u16 i;
  u8 str[2];
  str[1] = '\0';
  
  for (i = 0; i < SLIST_AZ_TOTAL; i++)
  {
    if(i == 0)
    {
      str[0] = '*';
      list_set_field_content_by_ascstr(ctrl, i, 0, str);
    }
    else if(i < 27)
    {
      str[0] = 'A'+ (u8)i - 1;
      list_set_field_content_by_ascstr(ctrl, i, 0, str);
    }
    else
    {
      str[0] = '0' + (u8)i - 27;
      list_set_field_content_by_ascstr(ctrl, i, 0, str); 
    }
  }
  return SUCCESS;
}

static RET_CODE on_slist_t9_input(control_t *p_az_list, u16 msg, u32 para1, u32 para2)
{
  u16 *p_unistr = NULL;
  u16 focus = 0;
  u8 key;

  key = msg & MSG_DATA_MASK;

  if(key == slist_t9_curn_key)
  {
    slist_t9_curn_bit++;
  }
  else
  {
    slist_t9_curn_bit = 0;
  }

  slist_t9_curn_key = key;

  //jump
  focus = slist_t9_get_focus(key);

  list_set_focus_pos(p_az_list, focus);

  slist_az_update(p_az_list, list_get_valid_pos(p_az_list), SLIST_AZ_PAGE, 0);

  ctrl_paint_ctrl(p_az_list, TRUE);
  
  p_unistr = (u16 *)list_get_field_content(p_az_list, focus, 0);
  
  MT_ASSERT(p_unistr != NULL);
  
  slist_az_find(p_az_list, p_unistr, TRUE);  

  return SUCCESS;
}

RET_CODE open_small_list(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_sbar, *p_sat;
  control_t *p_ad_win;//, *p_ad_win_txt;
  control_t *p_az_list;
  u16 i, total, list_focus;
  u16 all_num = 0;
  u16 current_pg_id, cbox_focus = 0xFFFF, pos_in_set;
  u32 context;
  u8 view_id = 0xFF;
  u8 group_type;
  u8 current_mod;
  u16 current_group = 0;
  u16 pg_pos;
  
  list_xstyle_t slist_az_item_rstyle =
  {
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_MID_HL,
  RSI_LIST_FIELD_MID_HL,
  RSI_LIST_FIELD_MID_HL,
};

  list_xstyle_t slist_az_item_fstyle =
  {
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};  
  
  is_select_play = FALSE;

  //for restore view
  ui_cache_view();

  current_mod = sys_status_get_curn_prog_mode();
  
  if(CURN_MODE_TV == current_mod)
  {
    ;
  }
  else if(CURN_MODE_RADIO == current_mod)
  {
    ;
  }
  else
  {
    return ERR_FAILURE;
  }
    
  if(para1 == V_KEY_OK || para1 == V_KEY_LIST)
  {
    slist_hkey = V_KEY_OK;
      
    //view_id = ui_dbase_get_pg_view_id();
    
    current_group = sys_status_get_curn_group();
    if(current_group >= DB_DVBS_MAX_SAT + 1)
    {
      view_id = ui_dbase_create_view(
       DB_DVBS_FAV_ALL, current_group - DB_DVBS_MAX_SAT - 1, NULL);
      all_num = sys_status_get_fav_group_num();
    }
    else
    {
      view_id = ui_dbase_get_pg_view_id();
      all_num = sys_status_get_all_group_num(); 
    }

  }
  else if(para1 == V_KEY_FAV)
  {
    slist_hkey = V_KEY_FAV;
    
    current_group = (u16)(para2>>16);    

    view_id = ui_dbase_create_view(
      DB_DVBS_FAV_ALL, (para2 & 0xFFFF), NULL);

    all_num = sys_status_get_fav_group_num();
  }
  else
  {
    MT_ASSERT(0);
  }

  sys_status_set_curn_group(current_group);
  total = db_dvbs_get_count(view_id);    

  // We get current program from all goup.
  sys_status_get_group_all_info(current_mod, &current_pg_id);
  
  pg_pos = db_dvbs_get_view_pos_by_id(view_id, current_pg_id);
  list_focus = (pg_pos == INVALIDID)? 0: pg_pos;

  ui_dbase_set_pg_view_id(view_id);

  p_cont =
    fw_create_mainwin((u8)(ROOT_ID_SMALL_LIST),
                           SMALL_LIST_MENU_CONT_X, SMALL_LIST_MENU_CONT_Y,
                           SMALL_LIST_MENU_WIDTH, SMALL_LIST_MENU_HEIGHT,
                           0, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_SLIST_CONT, RSI_SLIST_CONT, RSI_SLIST_CONT);
  ctrl_set_keymap(p_cont, small_list_root_keymap);
  ctrl_set_proc(p_cont, small_list_cont_proc);

  //create program list
  p_list =
    ctrl_create_ctrl(CTRL_LIST, IDC_SMALL_LIST_LIST,
                     SMALL_LIST_LIST_X, SMALL_LIST_LIST_Y,
                     SMALL_LIST_LIST_W, SMALL_LIST_LIST_H, p_cont,
                     0);
  ctrl_set_rstyle(p_list, RSI_SLIST_LIST, RSI_SLIST_LIST, RSI_SLIST_LIST);
  ctrl_set_keymap(p_list, small_list_keymap);
  ctrl_set_proc(p_list, small_list_proc);
  ctrl_set_mrect(p_list, SMALL_LIST_MID_L, SMALL_LIST_MID_T,
    SMALL_LIST_MID_L + SMALL_LIST_MID_W, SMALL_LIST_MID_T + SMALL_LIST_MID_H);
  list_set_item_interval(p_list, SMALL_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &slist_item_rstyle);
  list_set_count(p_list, total, SMALL_LIST_PAGE);
  list_set_field_count(p_list, SMALL_LIST_FIELD_NUM, SMALL_LIST_PAGE);
  if(list_focus == INVALIDID && total > 0)
  {
    list_focus = 0;
  }
  list_set_focus_pos(p_list, list_focus);
  list_select_item(p_list, list_focus);
  list_set_update(p_list, load_small_data, 0);
 
  for (i = 0; i < SMALL_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(slist_attr[i].attr), (u16)(slist_attr[i].width),
                        (u16)(slist_attr[i].left), (u8)(slist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, slist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, slist_attr[i].fstyle);
  }

  //sat
  p_sat = ctrl_create_ctrl(CTRL_CBOX, IDC_SMALL_LIST_SAT, SMALL_LIST_SAT_X, 
      SMALL_LIST_SAT_Y, SMALL_LIST_SAT_W, SMALL_LIST_SAT_H, p_cont, 0);
  ctrl_set_rstyle(p_sat, RSI_SLIST_SAT_CBOX, RSI_SLIST_SAT_CBOX, RSI_SLIST_SAT_CBOX);
  cbox_set_font_style(p_sat, FSI_SLIST_TEXT, FSI_SLIST_TEXT, FSI_SLIST_TEXT);
  cbox_set_work_mode(p_sat, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_sat, TRUE);
  cbox_dync_set_count(p_sat, all_num);
  cbox_dync_set_update(p_sat, slist_sat_cbox_update);
  sys_status_get_group_info(current_group, &group_type, &pos_in_set, &context);
  switch(group_type)
  {
    case GROUP_T_ALL:
      cbox_focus = sys_status_get_pos_by_group(current_group);
      break;
    case GROUP_T_SAT:
      cbox_focus = sys_status_get_sat_group_pos(current_group) - 1;
      break;
    case GROUP_T_FAV:
      cbox_focus = sys_status_get_fav_group_pos(current_group) - 1;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  
  OS_PRINTF("group[%d] type[%d] focus[%d]\n", current_group, group_type, cbox_focus);
  cbox_dync_set_focus(p_sat, cbox_focus);

  //create scroll bar
  p_sbar =
    ctrl_create_ctrl(CTRL_SBAR, IDC_SMALL_LIST_SBAR,
                     SMALL_LIST_SBAR_X, SMALL_LIST_SBAR_Y,
                     SMALL_LIST_SBAR_W, SMALL_LIST_SBAR_H,
                     p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SLIST_SBAR, RSI_SLIST_SBAR, RSI_SLIST_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SLIST_SBAR_MID, RSI_SLIST_SBAR_MID,
                     RSI_SLIST_SBAR_MID);
  ctrl_set_mrect(p_sbar, 0, SMALL_LIST_SBAR_VERTEX_GAP, 
    SMALL_LIST_SBAR_W, SMALL_LIST_SBAR_H - SMALL_LIST_SBAR_VERTEX_GAP);
  list_set_scrollbar(p_list, p_sbar);
 
  load_small_data(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);

  //a~z list.
  p_az_list = ctrl_create_ctrl(CTRL_LIST, IDC_SLIST_AZ, 
                        SMALL_LIST_AZ_X, SMALL_LIST_AZ_Y,
                        SMALL_LIST_AZ_W, SMALL_LIST_AZ_H, 
                        p_cont, 0);
  ctrl_set_rstyle(p_az_list, RSI_SLIST_LIST, RSI_SLIST_LIST, RSI_SLIST_LIST);
  ctrl_set_keymap(p_az_list, slist_az_keymap);
  ctrl_set_proc(p_az_list, slist_az_proc);
  ctrl_set_mrect(p_az_list, 
    SMALL_LIST_AZ_ITEM_L, SMALL_LIST_AZ_ITEM_T,
    SMALL_LIST_AZ_ITEM_W, SMALL_LIST_AZ_ITEM_H);
  list_set_item_interval(p_az_list, SMALL_LIST_AZ_ITEM_V_GAP);
  list_set_item_rstyle(p_az_list, &slist_az_item_rstyle);
  list_enable_select_mode(p_az_list, TRUE);
  list_set_count(p_az_list, SLIST_AZ_TOTAL, SLIST_AZ_PAGE);
  list_set_select_mode(p_az_list, LIST_SINGLE_SELECT);
  list_set_field_count(p_az_list, 1, SLIST_AZ_PAGE);
  list_set_field_font_style(p_az_list, 0, &slist_az_item_fstyle);
  list_set_focus_pos(p_az_list, 0);
  list_set_update(p_az_list, slist_az_update, 0);
  list_set_field_attr(p_az_list, 0, LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER, 30,
                        0, 0);
  list_set_field_rect_style(p_az_list, 0, &slist_az_item_rstyle);
  slist_az_update(p_az_list, 0, 0, 0);
  ctrl_set_sts(p_az_list, OBJ_STS_HIDE);
  
  //create AD
  p_ad_win = ctrl_create_ctrl(CTRL_BMAP, IDC_SMALL_LIST_ADS_CONT, SMALL_LIST_HELP_X,
                                              SMALL_LIST_HELP_Y, SMALL_LIST_HELP_W, SMALL_LIST_HELP_H, p_cont, 0);
  
  ctrl_set_rstyle(p_ad_win, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);

  //ui_show_ads_pic(PIC_TYPE_SUB_MENU, &g_ads_small_list_rect);

  
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifdef ENABLE_CA
  #ifndef ONLY1_CA_VER
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif
  #endif
#ifdef ENABLE_ADS
  ui_adv_pic_play(ADS_AD_TYPE_CHLIST, ROOT_ID_SMALL_LIST);
#endif
  return SUCCESS;
 }
#ifdef ENABLE_ADS
static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}
#endif

BEGIN_KEYMAP(small_list_root_keymap, ui_comm_root_keymap)
END_KEYMAP(small_list_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(small_list_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SAVE, on_slist_save)
#ifdef ENABLE_ADS
ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
#endif  
END_MSGPROC(small_list_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(small_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)  
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)  
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)   
  ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)  
  ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)   
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)   
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)   
  ON_EVENT(V_KEY_LEFT, MSG_GROUP_UP)   
  ON_EVENT(V_KEY_RIGHT, MSG_GROUP_DOWN)     
  ON_EVENT(V_KEY_FAVUP, MSG_GROUP_DOWN)   
  ON_EVENT(V_KEY_FAVDOWN, MSG_GROUP_UP)
  ON_EVENT(V_KEY_BLUE, MSG_BLUE)
  ON_EVENT(V_KEY_LIST, MSG_EXIT)
  //#ifdef NEW_LIST_SWITCH
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT) 
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_EXIT)
  //#endif
END_KEYMAP(small_list_keymap, NULL)

BEGIN_MSGPROC(small_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_slist_state_process)
  ON_COMMAND(MSG_FOCUS_UP, on_slist_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_GROUP_UP, on_slist_change_group)
  ON_COMMAND(MSG_GROUP_DOWN, on_slist_change_group)
  ON_COMMAND(MSG_BLUE, on_slist_state_process)
  ON_COMMAND(MSG_EXIT, on_slist_state_process)
  ON_COMMAND(MSG_EXIT_ALL, on_slist_state_process)
END_MSGPROC(small_list_proc, list_class_proc)

BEGIN_KEYMAP(slist_az_keymap, ui_comm_t9_keymap)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(slist_az_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(slist_az_proc, list_class_proc)
  ON_COMMAND(MSG_EXIT, on_slist_az_cancle)
  ON_COMMAND(MSG_FOCUS_UP, on_slist_az_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_slist_az_change_focus)
  ON_COMMAND(MSG_YES, on_slist_az_select)
  ON_COMMAND(MSG_NUMBER, on_slist_t9_input)
END_MSGPROC(slist_az_proc, list_class_proc)
