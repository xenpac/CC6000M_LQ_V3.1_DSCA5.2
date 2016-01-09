/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
 #ifdef ENABLE_PVR_REC_CONFIG
#include "ui_common.h"
#include "ui_pvr_api.h"
#include "ui_record_manager.h"
#include "ui_timer.h"
#include "ui_rename_v2.h"
#include "file_list.h"
#include "vfs.h"
#include "ui_mute.h"
#include "ui_volume_usb.h"
#ifdef ENABLE_CA
#include "cas_manager.h"
#ifdef ONLY1_CA_VER
#include "only1.h"
#endif
#endif
extern BOOL is_key_error;
enum rec_manager_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_GREEN,
  MSG_BLUE,
  MSG_VOLUME_UP,
  MSG_VOLUME_DOWN,
  MSG_REC_UNSUPPORT
};

enum focus_style
{
  FOCUS_NONE = 0,
  FOCUS_FIRST,
  FOCUS_LAST,
  FOCUS_NEXT,
  FOCUS_PREV,
};

enum rec_manager_menu_ctrl_id
{
  IDC_REC_MANAGER_PREVIEW = 1,
  IDC_REC_MANAGER_LIST_TITLE,
  IDC_REC_MANAGER_DETAIL,
  IDC_REC_MANAGER_LIST,
  IDC_REC_MANAGER_SBAR,
  IDC_REC_MANAGER_HELP,
};

#ifdef RECORD_MANAGER_PREVIEW
enum rec_manager_preview_ctrl_id
{
  IDC_REC_MANAGER_PREVIEW_TEXT = 1,
};

enum rec_manager_list_detail_id
{
  IDC_REC_MANAGER_DETAIL_PGNAME = 1,
  IDC_REC_MANAGER_DETAIL_TIMETOTAL,
  IDC_REC_MANAGER_DETAIL_FILE_SIZE,
};
#else
enum rec_manager_list_detail_id
{
  IDC_REC_MANAGER_DETAIL_PGNAME = 1,
  IDC_REC_MANAGER_DETAIL_TIMETOTAL,
  IDC_REC_MANAGER_DETAIL_FILE_SIZE,
  IDC_REC_MANAGER_DETAIL_TEXT,
};
#endif

static list_xstyle_t rec_manager_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t rec_manager_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLUE,
  FSI_WHITE,
};

static list_xstyle_t  rec_manager_list_field_rstyle_mid =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t rec_manager_list_attr[REC_MANAGER_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    50, 10, 0, &rec_manager_list_field_rstyle_mid,  &rec_manager_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    40, 60, 0, &rec_manager_list_field_rstyle_mid,  &rec_manager_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR |STL_LEFT,
    340, 100, 0, &rec_manager_list_field_rstyle_mid,  &rec_manager_list_field_fstyle},
};

static pvr_param_t  g_pvr_param = {0};
static BOOL  g_b_unlock_temp = FALSE;//true -- use green key,false -- focus
static BOOL  g_b_op_able = FALSE;//del,lock operation enable
//static u16 g_media_player_preview_focus = 0;

static  comm_help_data_t2 help_data = //help bar data
  {
    8, 70, {40, 160, 40, 160,40, 160,40, 160},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_CONFIRM,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
      HELP_RSC_STRID | IDS_LOCK,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
      HELP_RSC_STRID | IDS_DELETE,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
      HELP_RSC_STRID | IDS_RENAME,
     },
  };

u16 g_unistr[REC_FILE_NAME_LEN*2] = {0};

static RET_CODE rec_manager_list_update(control_t* p_list, u16 start, u16 size, u32 context);
static void record_file_update(control_t *p_list);
static RET_CODE on_rec_manager_list_green(control_t *p_list, u16 msg, 
  u32 para1, u32 para2);
u32 get_record_filename(u16 *p_src, u16 *p_unistr);

RET_CODE rec_manager_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

u16 rec_manager_list_keymap(u16 key);
RET_CODE rec_manager_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 rec_manager_pwd_keymap(u16 key);
RET_CODE rec_manager_pwd_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);


void rec_manager_init_param(void)
{
  OS_PRINTF("%s,line %d\n", __FUNCTION__, __LINE__);
  memset(&g_pvr_param, 0, sizeof(pvr_param_t));
}

//ts_player_preview_cfg_t preview_cfg;
static void rec_manager_set_detail(control_t *p_list, BOOL is_paint, BOOL is_pop, BOOL is_pwd_check)
{
  u8 asc_str[32];
  control_t *p_detail_cont,*p_pvr_size,*p_pvr_time_total,*p_pvr_name;
  flist_t *p_rec_fav = NULL;
  rec_info_t *p_rec_info = NULL;
  u16 program[PROGRAM_NAME_MAX]={0}, i = 0;

  p_detail_cont = ctrl_get_child_by_id(p_list->p_parent, IDC_REC_MANAGER_DETAIL);
  p_pvr_name = ctrl_get_child_by_id(p_detail_cont, IDC_REC_MANAGER_DETAIL_PGNAME);
  p_pvr_time_total = ctrl_get_child_by_id(p_detail_cont, IDC_REC_MANAGER_DETAIL_TIMETOTAL);
  p_pvr_size = ctrl_get_child_by_id(p_detail_cont, IDC_REC_MANAGER_DETAIL_FILE_SIZE);
  OS_PRINTF("%s,line %d\n",__FUNCTION__,__LINE__);

  if(0 == g_pvr_param.total)
  {
  	text_set_content_by_unistr(p_pvr_name, (u16*)"");
    text_set_content_by_unistr(p_pvr_time_total, (u16*)"");
    text_set_content_by_unistr(p_pvr_size, (u16*)"");
	  ctrl_paint_ctrl(p_detail_cont, TRUE);
    //ui_show_logo(LOGO_BLOCK_ID_M1);
  	return;
  }
  p_rec_fav = ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);
  p_rec_info = ui_pvr_get_rec_info(p_rec_fav);
  if(p_rec_info && p_rec_info->program[0])
  {
#ifdef RECORD_MANAGER_PREVIEW
    control_t *p_preview = ctrl_get_child_by_id(p_list->p_parent, IDC_REC_MANAGER_PREVIEW);
    control_t *p_text = ctrl_get_child_by_id(p_preview, IDC_REC_MANAGER_PREVIEW_TEXT);
#else
    control_t *p_text = ctrl_get_child_by_id(p_detail_cont, IDC_REC_MANAGER_DETAIL_TEXT);
#endif

    if(is_pwd_check && ui_pvr_get_lock_flag(g_pvr_param.focus))
    {
      //lock
      g_b_op_able = FALSE;
      text_set_content_by_strid(p_text, IDS_PARENTAL_LOCK);
      ctrl_set_attr(p_text, OBJ_ATTR_ACTIVE);
      ctrl_set_sts(p_text, OBJ_STS_SHOW);
      ctrl_paint_ctrl(p_text, TRUE);
    }
    else
    {
#if 0
      OS_PRINTF("p_rec_info->encrypt_flag = %d\n",p_rec_info->encrypt_flag);
      //now can't record ca scambled ts
      if(p_rec_info->encrypt_flag)
      {
        //need ca
        text_set_content_by_strid(p_text, IDS_SCRAMBLED);
        ctrl_set_attr(p_text, OBJ_ATTR_ACTIVE);
        ctrl_set_sts(p_text, OBJ_STS_SHOW);
        ctrl_paint_ctrl(p_text, TRUE);
      }
      else
#endif
      {
#ifdef RECORD_MANAGER_PREVIEW
        //unlock, preview
        on_switch_preview(NULL, 0, 0, 0);
#endif
        g_b_op_able = TRUE;
        g_pvr_param.playing = g_pvr_param.focus;
        ctrl_set_sts(p_text, OBJ_STS_HIDE);
        ctrl_paint_ctrl(ctrl_get_parent(p_text), TRUE);
        list_select_item(p_list, g_pvr_param.focus);
        ctrl_paint_ctrl(p_list, TRUE);
        
        ts_player_stop();
        #ifdef ONLY1_CA_VER
        #ifndef WIN32
        OS_PRINTF("####this pvr scrambled flag [%d], line [%d], service id[%d]\n", ui_get_pvr_scrambled_flag(p_rec_info), __LINE__, p_rec_info->ext_pid[3].pid);
        if((!CDCASTB_IsEntitledService((u16)p_rec_info->ext_pid[3].pid)) && (ui_get_pvr_scrambled_flag(p_rec_info)))
        { 
          OS_PRINTF("####this card is no entitle\n");
          ui_comm_showdlg_open(NULL, IDS_CA_NO_ENTILE_PROG, NULL,4000);
        }
        else
        #endif
        {
          OS_PRINTF("####this card has entitle\n");
          ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
          #ifdef ENABLE_CA
          cas_manager_start_pvr_play(g_pvr_param.focus);
          #endif
          ui_set_mute(ui_is_mute());
        }
        #else
        ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
        #ifdef ENABLE_CA
        cas_manager_start_pvr_play(g_pvr_param.focus);
        #endif
        ui_set_mute(ui_is_mute());
        #endif
  
      }
    }
  }
  else
  {
    ui_comm_showdlg_open(NULL, IDS_UNSUPPORTED, NULL, 3000);
    /*Set timer for play next video*/
    fw_tmr_create(ROOT_ID_RECORD_MANAGER, 
                    MSG_REC_UNSUPPORT, 
                    3000,
                    FALSE);
  }
  if(p_rec_info)
  {
    //set pg name
    {
      i = 0;
      while(p_rec_info->program[i] != 0)
      {
          program[i] = p_rec_info->program[i];
          i++;
      }
      program[i] = 0;
      text_set_content_by_unistr(p_pvr_name, program);
    }

    //set time total
    memset(asc_str, 0, sizeof(asc_str));
    sprintf((char *)asc_str,"%02ld:%02ld:%02ld", p_rec_info->total_time/3600, 
      p_rec_info->total_time%3600/60, p_rec_info->total_time%60);
    text_set_content_by_ascstr(p_pvr_time_total, asc_str);

    //set size
    memset(asc_str, 0, sizeof(asc_str));
    ui_conver_file_size_unit((u32)file_list_get_file_size((u16 *)flist_get_name(p_rec_fav))/1024,
      asc_str);
    text_set_content_by_ascstr(p_pvr_size, asc_str);
  }
  if(is_paint)
  {
    ctrl_paint_ctrl(p_detail_cont, TRUE);
  }
  if(!g_b_op_able && is_pop)
  {
    on_rec_manager_list_green(p_list,0,0,0);
  }
}

static void rec_manager_pass_key_to_parent(u16 msg)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    case MSG_PAGE_UP:
      key = V_KEY_PAGE_UP;
      break;
    case MSG_PAGE_DOWN:
      key = V_KEY_PAGE_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);
}

void ui_rec_manager_destroy(void)
{
  ui_pvr_unload_rec_info();
  ts_player_release();
}

static RET_CODE on_rec_manager_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list = NULL;

  OS_PRINTF("%s,line %d\n",__FUNCTION__,__LINE__);
  p_list = ctrl_get_child_by_id(p_cont, IDC_REC_MANAGER_LIST);

  ui_enable_chk_pwd(TRUE);

  #ifdef ONLY1_CA_VER
  if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL ||
      fw_find_root_by_id(ROOT_ID_FINGER_PRINT_1) != NULL ||
      fw_find_root_by_id(ROOT_ID_FINGER_PRINT_2) != NULL ||
      fw_find_root_by_id(ROOT_ID_FINGER_PRINT_3) != NULL)
  {
    ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_HIDE_ECM_FINGER_PRINT, 0, 0);
  }

  if(ctrl_get_child_by_id(fw_find_root_by_id(ROOT_ID_BACKGROUND), IDC_BG_MENU_CA_ROLL_TOP) ||
    ctrl_get_child_by_id(fw_find_root_by_id(ROOT_ID_BACKGROUND), IDC_BG_MENU_CA_ROLL_BOTTOM))
  {
    ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_OSD_HIDE, 0, 0);
  }

  if(fw_find_root_by_id(ROOT_ID_CA_SUPER_OSD) != NULL)
  {
    ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_SUPER_OSD_HIDE, 0, 0);
  }
  #endif
  
  /*
  ui_pvr_unload_rec_info();
  ts_player_release();
  ui_epg_init();
  ui_epg_start();
  */
  gui_stop_roll(p_list);

  //ui_time_enable_heart_beat(FALSE);
  //ui_deinit_play_timer();
  if(ui_is_mute())
  {
    close_mute();
  }
  if(CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
  	ui_comm_dlg_close();
  return ERR_NOFEATURE;
}

static RET_CODE on_rec_manager_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  OS_PRINTF("%s,line %d,[%d]\n",__FUNCTION__,__LINE__,g_pvr_param.b_from_list);
  if(g_pvr_param.b_from_list)
    ui_close_all_mennus();
  else
    manage_close_menu(ROOT_ID_RECORD_MANAGER, 0, 0);;
  return SUCCESS;
}

static RET_CODE on_rec_manager_focus_change(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t* p_list = ctrl_get_child_by_id(p_cont, IDC_REC_MANAGER_LIST);
  
  g_pvr_param.focus = (u16)para1;
  record_file_update(p_list);
  if(ui_is_mute())
  {
    open_mute(0, 0);
  }
  /*list_set_focus_pos(p_list, g_pvr_param.focus);
  rec_manager_list_update(p_list, list_get_valid_pos(p_list), REC_MANAGER_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_list,TRUE);*/
  
  return SUCCESS;
}

#if 0
static RET_CODE on_rec_manager_play_media(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  if (g_media_player_preview_focus != g_pvr_param.focus)
  {
    control_t* p_list = ctrl_get_child_by_id(p_cont, IDC_REC_MANAGER_LIST);
    rec_manager_set_detail(p_list, TRUE, TRUE);
    g_media_player_preview_focus = g_pvr_param.focus;
  }
  ui_play_timer_set_state(0);
  return SUCCESS;
}
#endif

//static RET_CODE on_rec_manager_donothing(control_t *p_cont, u16 msg,
  //u32 para1, u32 para2)
//{
  //return SUCCESS;
//}

static RET_CODE rec_manager_list_update(control_t* p_list, u16 start, u16 size, 
                                u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u8 asc_str[32];
  u16 file_uniname[MAX_FILE_PATH];
  flist_t *p_rec_fav = NULL;

  i = 0;
  
  for (; i < size; i++)
  {
    p_rec_fav = ui_pvr_get_rec_fav_by_index(i + start);
    if ( (i+start) < cnt)
    {
      MT_ASSERT(p_rec_fav != NULL);

      if(ui_pvr_get_lock_flag(i + start))
        list_set_field_content_by_icon(p_list, i + start, REC_MANAGER_LIST_LOCK, IM_TV_LOCK);
      else
        list_set_field_content_by_icon(p_list, i + start, REC_MANAGER_LIST_LOCK, 0);

      /* NO. */
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), REC_MANAGER_LIST_NO, asc_str); 

      /* NAME */
      get_record_filename(flist_get_name(p_rec_fav), file_uniname);
      //str_uni_printf(file_uniname);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), REC_MANAGER_LIST_NAME, file_uniname);

    }
    else
      break;
  }

  return SUCCESS;
}

static RET_CODE on_rec_manager_pwd_close(control_t *p_ctrl, u16 msg, u32 para1,
                          u32 para2)
{
  rec_manager_pass_key_to_parent(msg);
  ui_comm_pwdlg_close();

  return SUCCESS;
}

static RET_CODE on_rec_manager_pwd_correct(control_t *p_ctrl, u16 msg, u32 para1,
                          u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_list;

  p_list = ui_comm_root_get_ctrl(ROOT_ID_RECORD_MANAGER, IDC_REC_MANAGER_LIST);
  ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);
  
  ui_comm_pwdlg_close();
  if(!g_b_unlock_temp)
  {
    //save unlock
    ui_pvr_change_lock_flag(g_pvr_param.focus);
    if(ui_pvr_get_lock_flag(g_pvr_param.focus))
      list_set_field_content_by_icon(p_list, g_pvr_param.focus, REC_MANAGER_LIST_LOCK, IM_TV_LOCK);
    else
    {
      list_set_field_content_by_icon(p_list, g_pvr_param.focus, REC_MANAGER_LIST_LOCK, 0);
      //ui_set_have_pass_pwd(TRUE);
      ts_player_stop();
      rec_manager_set_detail(p_list, TRUE, FALSE, FALSE); 
    }
    list_draw_item_ext(p_list, g_pvr_param.focus, TRUE);
  }
  else
  {
    //ui_set_have_pass_pwd(TRUE);
    
    if(ts_player_isplaying())
    {
        ts_player_stop();
        rec_manager_set_detail(p_list, TRUE, FALSE, FALSE);
    } else {
        rec_manager_set_detail(p_list, TRUE, FALSE, FALSE); 
    }
  }
  return ret;
}

static RET_CODE on_rec_manager_list_change_volume(control_t *p_list,
  u16 msg, u32 para1, u32 para2)
{
  open_volume_usb(ROOT_ID_RECORD_MANAGER, para1);
  return SUCCESS;
}

static RET_CODE on_rec_manager_list_change_focus(control_t *p_list,
  u16 msg, u32 para1, u32 para2)
{
  u16   new_focus = 0;
  
  if(list_get_count(p_list) == 0)
  {
    return SUCCESS;
  }
  
  list_class_proc(p_list, msg, 0, 0);
  new_focus = list_get_focus_pos(p_list);
  if(g_pvr_param.focus == new_focus)
  {
    return SUCCESS;
  }
  
  g_pvr_param.focus = new_focus;
  ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);
  if( ui_pvr_get_lock_flag(g_pvr_param.focus) )
  {
    g_b_op_able = FALSE;
    on_rec_manager_list_green(p_list,0,0,0);
  }
  else
  {
    g_b_op_able = TRUE;
  }
  //ui_play_timer_start();
  //rec_manager_set_detail(p_list, TRUE, TRUE);  
    
  return SUCCESS;
}

static RET_CODE on_rec_manager_list_green(control_t *p_list, u16 msg, 
  u32 para1, u32 para2)  
{
  comm_pwdlg_data_t rec_pwdlg_data =
  {
    ROOT_ID_RECORD_MANAGER,
    REC_MANAGER_PWD_DLG_FOR_CHK_X,
    REC_MANAGER_PWD_DLG_FOR_CHK_Y,
    IDS_MSG_INPUT_PASSWORD,
    rec_manager_pwd_keymap,
    rec_manager_pwd_proc,
    PWDLG_T_COMMON
  };
  flist_t *p_rec_fav = ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);

  if(p_rec_fav == NULL)
    return SUCCESS;

  if(!msg)
    g_b_unlock_temp = TRUE;
  else
    g_b_unlock_temp = FALSE;
  ui_comm_pwdlg_open(&rec_pwdlg_data);
  return SUCCESS;
}

static void record_file_update(control_t *p_list)
{
  list_set_count(p_list, (u16)g_pvr_param.total, REC_MANAGER_LIST_PAGE);
	list_set_focus_pos(p_list, g_pvr_param.focus);
	rec_manager_list_update(p_list, list_get_valid_pos(p_list), REC_MANAGER_LIST_PAGE, 0);
  gui_roll_reset(p_list);
	ctrl_paint_ctrl(p_list, TRUE);
	rec_manager_set_detail(p_list, TRUE, FALSE, FALSE); 
}

static void delete_file(void)
{
	control_t *p_list = ui_comm_root_get_ctrl(ROOT_ID_RECORD_MANAGER, IDC_REC_MANAGER_LIST);

  if(ts_player_isplaying())
  {
    if(g_pvr_param.focus < g_pvr_param.playing)
    {
      g_pvr_param.playing --;
    }
    else if(g_pvr_param.focus == g_pvr_param.playing)
    {
    ts_player_stop();
      g_pvr_param.playing = 0xffff;
    }
  }
  else
  {
    g_pvr_param.playing = 0xffff;
  }  

  ui_pvr_del_one_rec_byname(g_pvr_param.focus);
  g_pvr_param.total --;
  g_pvr_param.focus = g_pvr_param.focus? (g_pvr_param.focus - 1) : 0;

  list_set_count(p_list, (u16)g_pvr_param.total, REC_MANAGER_LIST_PAGE);
  list_set_focus_pos(p_list, g_pvr_param.focus);
  if(g_pvr_param.playing != 0xffff)
  {
    list_select_item(p_list, g_pvr_param.playing);
  }
  rec_manager_list_update(p_list, list_get_valid_pos(p_list), REC_MANAGER_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_list, TRUE);
}

static RET_CODE on_rec_manager_list_blue(control_t *p_list, u16 msg, 
  u32 para1, u32 para2)  
{
  flist_t *p_rec_fav = ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);

  if(p_rec_fav == NULL)
    return SUCCESS;
    
  if(!g_b_op_able)
      on_rec_manager_list_green(p_list,0,0,0);
    //ui_comm_cfmdlg_open(NULL, IDS_PARENTAL_LOCK, NULL, 5000);
  else
    ui_comm_ask_for_dodlg_open(NULL, IDS_MSG_ASK_FOR_DEL, delete_file, NULL,0);

  return SUCCESS;
}

static RET_CODE on_rec_manager_list_red(control_t *p_list, u16 msg, 
  u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  rename_param_t_v2 rename_param;
  flist_t *p_rec_fav = ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);
  memset(&rename_param, 0, sizeof(rename_param_t_v2));
  if(p_rec_fav == NULL)
    return SUCCESS;
  if(!g_b_op_able)
  {
     on_rec_manager_list_green(p_list,0,0,0);
    //ui_comm_cfmdlg_open(NULL, IDS_PARENTAL_LOCK, NULL, 5000);
  }
  else
  {
    get_record_filename(flist_get_name(p_rec_fav), g_unistr);
    rename_param.max_len = REC_FILE_NAME_LEN - 1;
    rename_param.uni_str = g_unistr;

    ret = manage_open_menu(ROOT_ID_RENAME_V2, (u32)&rename_param, 
          ROOT_ID_RECORD_MANAGER);
  }
  return ret;
}

static RET_CODE on_rename_check(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	return SUCCESS;
}
//lint -e613
u32 get_record_filename(u16 *p_src, u16 *p_unistr)
{
  u32 i = 0;
  u16 *p_name = NULL;

  MT_ASSERT(p_src != NULL && p_unistr != NULL);

  p_name = (u16 *)(p_src + g_pvr_param.path_len);
  
  //remove file extern name, .ts
  while(p_name[i] != 0)
  {
    if((p_name[i] == 0x2e/*'.'*/) && (p_name[i + 1] == 0x74/*'t'*/) && (p_name[i + 2] == 0x73/*'s'*/))
    {
      break;
    } 
    p_unistr[i] = p_name[i];
    i++;
  }
  p_unistr[i] = 0;

  return i;
}
//lint +e613

//lint -e438 -e830
static RET_CODE on_rename_update(control_t *p_list,u16 msg,u32 para1, u32 para2)
{
  u16 new_name[MAX_FILE_PATH]= {0}; 
  flist_t *p_rec_fav = ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);
  BOOL result = FALSE;
  u16 len = (u16)strlen(PVR_DIR);
  u16 idx_file[MAX_FILE_PATH] = {0};
  u16 uni_tmp[32] = {0};
  rec_info_t *p_rec_info = NULL;

  if(ts_player_isplaying() && (g_pvr_param.focus == g_pvr_param.playing))
    ts_player_stop();

  str_asc2uni((u8 *)PVR_DIR, new_name);
  new_name[len] = 0x5c/*'\\'*/;
  uni_strncpy(new_name + len + 1, (u16 *)para1, REC_FILE_NAME_LEN);

  str_asc2uni((u8 *)".ts", uni_tmp);
  uni_strcat(new_name, uni_tmp, MAX_FILE_PATH);
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    ui_pvr_extern_rename(flist_get_name(p_rec_fav), new_name);
#endif
  result = file_list_rename(flist_get_name(p_rec_fav), new_name);
  OS_PRINTF("rename\n");
  //str_uni_printf(p_rec_fav->p_file->name);
  //str_uni_printf(new_name);
  if(result)
  {
    get_idx_info_file(flist_get_name(p_rec_fav), idx_file, MAX_FILE_PATH);
    uni_strncpy(flist_get_name(p_rec_fav) + 3, new_name, MAX_FILE_PATH - 3);
    get_idx_info_file(flist_get_name(p_rec_fav), new_name, MAX_FILE_PATH);
    result = file_list_rename(idx_file, new_name);

    list_set_field_content_by_unistr(p_list, g_pvr_param.focus, REC_MANAGER_LIST_NAME, (u16 *)para1);
    gui_roll_reset(p_list);
    list_select_item(p_list, g_pvr_param.focus);
    ctrl_paint_ctrl(p_list, TRUE);
    p_rec_info = ui_pvr_get_rec_info(p_rec_fav);

    ts_player_stop();
    #ifdef ONLY1_CA_VER
    #ifndef WIN32
    OS_PRINTF("####this pvr scrambled flag [%d], line [%d], service id[%d]\n", ui_get_pvr_scrambled_flag(p_rec_info), __LINE__, p_rec_info->ext_pid[3].pid);
    if((!CDCASTB_IsEntitledService((u16)p_rec_info->ext_pid[3].pid)) && (ui_get_pvr_scrambled_flag(p_rec_info)))
    { 
      OS_PRINTF("####this card is no entitle\n");
      ui_comm_showdlg_open(NULL, IDS_CA_NO_ENTILE_PROG, NULL,4000);
    }
    else
    #endif
    {
      OS_PRINTF("####this card has entitle\n");
      ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
      #ifdef ENABLE_CA
      cas_manager_start_pvr_play(g_pvr_param.focus);
      #endif
    }
    #else
    ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
    #ifdef ENABLE_CA
    cas_manager_start_pvr_play(g_pvr_param.focus);
    #endif
    #endif
    
  }
  return SUCCESS;
}
//lint +e438 +e830
static RET_CODE on_rec_manager_list_select(control_t *p_list, u16 msg, 
  u32 para1, u32 para2)  
{
  if(!g_pvr_param.total)
  {
    OS_PRINTF("No record\n");
    return SUCCESS;
  }
  
  ui_pvr_get_rec_fav_by_index(g_pvr_param.focus);

  if( ui_pvr_get_lock_flag(g_pvr_param.focus) )
  {
    if(ts_player_isplaying() && (g_pvr_param.focus == g_pvr_param.playing) && (is_key_error == FALSE))
    {
      //if preview start, now switch to pvr play
      manage_open_menu(ROOT_ID_PVR_PLAY_BAR, (u32)(&g_pvr_param), 0);
    }
    else
    {
      g_b_op_able = FALSE;
      on_rec_manager_list_green(p_list,0,0,0);
      //ui_comm_cfmdlg_open(NULL, IDS_PARENTAL_LOCK, NULL, 5000);
    }
  }
  else
  {
    if(ts_player_isplaying() && (g_pvr_param.focus == g_pvr_param.playing) && (is_key_error == FALSE))
    {
      //if preview start, now switch to pvr play
      manage_open_menu(ROOT_ID_PVR_PLAY_BAR, (u32)(&g_pvr_param), 0);
    }
    else
    {
      //preview
      ts_player_stop();
      rec_manager_set_detail(p_list, TRUE, FALSE, TRUE);
    }
  }
  
  return SUCCESS;
}

static void record_manager_check(void)
{
  partition_t *p_patition = NULL;

  g_pvr_param.total = 0; 
  if(!file_list_get_partition(&p_patition))
  {
    OS_PRINTF("%s, no filesystem!\n", __FUNCTION__);
    return ;
  }
  
  g_pvr_param.path_len = (u8)strlen(PVR_DIR) + 4;//"c:\PVRS\",use unicode
  g_pvr_param.total = (u16)ui_pvr_load_rec_info();
  if(!g_pvr_param.total)
  {
    OS_PRINTF("%s, no record!\n", __FUNCTION__);
    return ;
  }

  if(g_pvr_param.focus >= g_pvr_param.total)
  {
    g_pvr_param.focus = 0;
  }
  OS_PRINTF("%s, found [%d] record\n", __FUNCTION__,g_pvr_param.total);
}

static RET_CODE on_rec_manager_play_end(control_t *p_list, u16 msg, 
  u32 para1, u32 para2)  
{
  u16 select_index = 0;
  OS_PRINTF("%s,line %d\n", __FUNCTION__,__LINE__);
  
  ts_player_stop();

  if(fw_get_focus_id() == ROOT_ID_RECORD_MANAGER)
  {
    select_index = list_get_the_first_selected(p_list, 0);
    list_set_focus_pos(p_list, select_index);
    on_rec_manager_list_change_focus(p_list, MSG_FOCUS_DOWN,  0, 0);

    return on_rec_manager_list_select(p_list, msg,  para1, para2);
  }
  else
  {
    return SUCCESS;
  }
}
static RET_CODE on_rec_manager_unsupport(control_t *p_list, u16 msg, 
  u32 para1, u32 para2)  
{
  OS_PRINTF("%s,line %d\n", __FUNCTION__,__LINE__);
  
  ts_player_stop();

  if(fw_get_focus_id() == ROOT_ID_RECORD_MANAGER)
  {
    on_rec_manager_list_change_focus(p_list, MSG_FOCUS_DOWN,  0, 0);

    return on_rec_manager_list_select(p_list, msg,  para1, para2);
  }
  else
  {
    return SUCCESS;
  }
}

#ifdef EXTERN_CA_PVR
extern u32 ca_msg_time;
#endif
static RET_CODE on_rec_manager_played_time(control_t *p_list, u16 msg, 
  u32 para1, u32 para2)  
{
#ifdef EXTERN_CA_PVR
  u32 g_cur_play_time = para1 / 1000;
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
  {
    ui_pvr_extern_t *p_pvr_extern = NULL;
    u8 num = 0;
    u8 i = 0;
    num = ui_pvr_extern_get_msg_num(g_cur_play_time);
    
    OS_PRINTF("#times = %d \n", num);

    for(i = 0;i < num;i ++)
    {
      p_pvr_extern = ui_pvr_extern_read(g_cur_play_time, i);
      if (p_pvr_extern != NULL)
      {
        OS_PRINTF("1g_cur_play_time = %d , p_pvr_extern->rec_time = %d \n",g_cur_play_time,p_pvr_extern->rec_time);
        OS_PRINTF("1ca_msg_time = %d , p_pvr_extern->rec_time = %d \n",ca_msg_time,p_pvr_extern->rec_time);
        if(g_cur_play_time == p_pvr_extern->rec_time)
        {
          if(p_pvr_extern->rec_time != ca_msg_time)
          {
            if(i == (num -1))
            {
              ca_msg_time = p_pvr_extern->rec_time;
            }
            switch(p_pvr_extern->type)
            {
              case UI_PVR_FINGER_PRINT:
                OS_PRINTF("function [%s], line [%d]\n", __FUNCTION__, __LINE__);
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_FINGER_PRINT, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              case UI_PVR_ECM_FINGER_PRINT:
                OS_PRINTF("function [%s], line [%d]\n", __FUNCTION__, __LINE__);
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_ECM_FINGER_PRINT, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              case UI_PVR_HIDE_ECM_FINGER_PRINT:
                OS_PRINTF("function [%s], line [%d]\n", __FUNCTION__, __LINE__);
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_HIDE_ECM_FINGER_PRINT, 0, 0);
                break;
              case UI_PVR_OSD:
                OS_PRINTF("function [%s], line [%d]\n", __FUNCTION__, __LINE__);
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_OSD, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              case UI_PVR_OSD_HIDE:
                OS_PRINTF("function [%s], line [%d]\n", __FUNCTION__, __LINE__);
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_OSD_HIDE, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              case UI_PVR_SUPER_OSD_HIDE:
                OS_PRINTF("function [%s], line [%d]\n", __FUNCTION__, __LINE__);
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_SUPER_OSD_HIDE, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              default:
                break;
            }
          }
        }
      }
    }
  }
  #endif
  return SUCCESS;
}


RET_CODE open_record_manager(u32 para1, u32 para2)
{
  control_t *p_menu, *p_detail, *p_pvr_pgname, *p_list_sbar, *p_list, *p_list_title, *p_pvr_time_total, *p_pvr_file_size;
#ifdef RECORD_MANAGER_PREVIEW
  control_t *p_preview;
#endif
  u32 i;
  utc_time_t cur_time;
  roll_param_t param;
  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);
  memset(&param, 0, sizeof(roll_param_t));
  if(!(ui_get_usb_status()))
  {
    return SUCCESS;
  }
  
  record_manager_check();

  //initialize global variables.
  time_get(&cur_time, TRUE);

  ts_player_init();



  /*Create Menu*/
  p_menu = ui_comm_root_create(ROOT_ID_RECORD_MANAGER, 
                  0, COMM_BG_X, COMM_BG_Y, 
 							    COMM_BG_W,  COMM_BG_H,
 							    RSC_INVALID_ID, IDS_RECORD_MANAGER);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  if(para2 == 1) //fix bug
  {
    OS_PRINTF("from list\n");
    g_pvr_param.b_from_list = TRUE;
  }
  else
  {
    OS_PRINTF("from mainmenu\n");
    g_pvr_param.b_from_list = FALSE;
  }
  ctrl_set_proc(p_menu, rec_manager_cont_proc);
  
  //list title
  p_list_title = ctrl_create_ctrl(CTRL_TEXT, IDC_REC_MANAGER_LIST_TITLE, 
  							REC_MANAGER_LIST_TITLE_X, REC_MANAGER_LIST_TITLE_Y, 
  							REC_MANAGER_LIST_TITLE_W, REC_MANAGER_LIST_TITLE_H,
  							p_menu, 0);
  
  ctrl_set_rstyle(p_list_title, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE);
  text_set_font_style(p_list_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_by_strid(p_list_title, IDS_RECORD_MANAGER);

  // list sbar
  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_REC_MANAGER_SBAR, 
  	   					      REC_MANAGER_LIST_SBARX, REC_MANAGER_LIST_SBARY, 
  	   					      REC_MANAGER_LIST_SBARW, REC_MANAGER_LIST_SBARH, 
  	   					      p_menu, 0);

  ctrl_set_rstyle(p_list_sbar, RSI_REC_MANAGER_SBAR, RSI_REC_MANAGER_SBAR, RSI_REC_MANAGER_SBAR);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_REC_MANAGER_SBAR_MID, RSI_REC_MANAGER_SBAR_MID,
                     RSI_REC_MANAGER_SBAR_MID);
  //ctrl_set_mrect(p_list_sbar, REC_MANAGER_LIST_SBAR_MIDL, REC_MANAGER_LIST_SBAR_MIDT, 
  //  REC_MANAGER_LIST_SBAR_MIDR, REC_MANAGER_LIST_SBAR_MIDB);

  //list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_REC_MANAGER_LIST, 
  					     REC_MANAGER_LISTX, REC_MANAGER_LISTY, 
  					     REC_MANAGER_LISTW, REC_MANAGER_LISTH, 
  					     p_menu, 0);
  
  ctrl_set_rstyle(p_list, RSI_REC_MANAGER_LCONT_LIST, RSI_REC_MANAGER_LCONT_LIST, RSI_REC_MANAGER_LCONT_LIST);
  ctrl_set_keymap(p_list, rec_manager_list_keymap);
  ctrl_set_proc(p_list, rec_manager_list_proc);
  ctrl_set_mrect(p_list, REC_MANAGER_LIST_MIDL, REC_MANAGER_LIST_MIDT,
    REC_MANAGER_LIST_MIDL + REC_MANAGER_LIST_MIDW, REC_MANAGER_LIST_MIDT + REC_MANAGER_LIST_MIDH);
  list_set_item_interval(p_list, REC_MANAGER_LCONT_LIST_VGAP);
  list_set_item_rstyle(p_list, &rec_manager_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, (u16)g_pvr_param.total, REC_MANAGER_LIST_PAGE);
  list_set_field_count(p_list, REC_MANAGER_LIST_FIELD, REC_MANAGER_LIST_PAGE);
  list_set_focus_pos(p_list, g_pvr_param.focus);
  list_set_update(p_list, rec_manager_list_update, 0);
 
  for (i = 0; i < REC_MANAGER_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(rec_manager_list_attr[i].attr), (u16)(rec_manager_list_attr[i].width),
                        (u16)(rec_manager_list_attr[i].left), (u8)(rec_manager_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, rec_manager_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, rec_manager_list_attr[i].fstyle);
  }    
  list_set_scrollbar(p_list, p_list_sbar);
  rec_manager_list_update(p_list, list_get_valid_pos(p_list), REC_MANAGER_LIST_PAGE, 0);

#ifdef RECORD_MANAGER_PREVIEW
  //preview window
  p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_REC_MANAGER_PREVIEW, REC_MANAGER_PREV_X, 
    							REC_MANAGER_PREV_Y, REC_MANAGER_PREV_W, 
    							REC_MANAGER_PREV_H, p_menu, 0);
  ctrl_set_rstyle(p_preview, RSI_REC_MANAGER_PREV, RSI_REC_MANAGER_PREV, RSI_REC_MANAGER_PREV);

  {
    //lock 
    control_t *p_text;
    p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_REC_MANAGER_PREVIEW_TEXT, 
								0, 0, 
 								REC_MANAGER_PREV_W-1, REC_MANAGER_PREV_H-1, 
 								p_preview, 0);
  
    ctrl_set_rstyle(p_text, RSI_TV, RSI_TV, RSI_TV);
    text_set_font_style(p_text, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX);
    text_set_align_type(p_text, STL_CENTER | STL_VCENTER);
    text_set_offset(p_text, REC_MANAGER_TXT_OFFSETX, REC_MANAGER_TXT_OFFSETY);
    text_set_content_type(p_text, TEXT_STRTYPE_STRID);
    ctrl_set_sts(p_text, OBJ_STS_HIDE);
  }
#endif

  //detail information
  p_detail = ctrl_create_ctrl(CTRL_CONT, IDC_REC_MANAGER_DETAIL, 
  						  REC_MANAGER_DETAIL_X, REC_MANAGER_DETAIL_Y, 
  						  REC_MANAGER_DETAIL_W, REC_MANAGER_DETAIL_H, 
    						p_menu, 0);
  
  ctrl_set_rstyle(p_detail, RSI_REC_MANAGER_DETAIL, RSI_REC_MANAGER_DETAIL, RSI_REC_MANAGER_DETAIL);

  //name
  p_pvr_pgname = ctrl_create_ctrl(CTRL_TEXT, IDC_REC_MANAGER_DETAIL_PGNAME, 
    							    REC_MANAGER_DETAIL_PGX, REC_MANAGER_DETAIL_PGY, 
    							    REC_MANAGER_DETAIL_PGW, REC_MANAGER_DETAIL_PGH, 
    							    p_detail, 0);
  
  ctrl_set_rstyle(p_pvr_pgname, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_pvr_pgname, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX);
  text_set_align_type(p_pvr_pgname, STL_LEFT | STL_VCENTER);
  text_set_offset(p_pvr_pgname, REC_MANAGER_TXT_OFFSETX, REC_MANAGER_TXT_OFFSETY);
  text_set_content_type(p_pvr_pgname, TEXT_STRTYPE_UNICODE);

  //total time
  p_pvr_time_total = ctrl_create_ctrl(CTRL_TEXT, IDC_REC_MANAGER_DETAIL_TIMETOTAL, 
    								REC_MANAGER_DETAIL_TIMETOTALX, REC_MANAGER_DETAIL_TIMETOTALY, 
    								REC_MANAGER_DETAIL_TIMETOTALW, REC_MANAGER_DETAIL_PGH, 
    								p_detail, 0);
  
  ctrl_set_rstyle(p_pvr_time_total, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_pvr_time_total, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX);
  text_set_align_type(p_pvr_time_total, STL_LEFT | STL_VCENTER);
  text_set_offset(p_pvr_time_total, REC_MANAGER_TXT_OFFSETX, REC_MANAGER_TXT_OFFSETY);
  text_set_content_type(p_pvr_time_total, TEXT_STRTYPE_UNICODE);

  //file size
  p_pvr_file_size = ctrl_create_ctrl(CTRL_TEXT, IDC_REC_MANAGER_DETAIL_FILE_SIZE, 
    							    REC_MANAGER_DETAIL_FILESIZEX, REC_MANAGER_DETAIL_FILESIZEY, 
    							    REC_MANAGER_DETAIL_FILESIZEW, REC_MANAGER_DETAIL_PGH, 
    							    p_detail, 0);
  
  ctrl_set_rstyle(p_pvr_file_size, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_pvr_file_size, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX);
  text_set_align_type(p_pvr_file_size, STL_LEFT | STL_VCENTER);
  text_set_offset(p_pvr_file_size, REC_MANAGER_TXT_OFFSETX, REC_MANAGER_TXT_OFFSETY);
  text_set_content_type(p_pvr_file_size, TEXT_STRTYPE_UNICODE);

#ifndef RECORD_MANAGER_PREVIEW
  {
    //lock 
    control_t *p_text;
    p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_REC_MANAGER_DETAIL_TEXT, 
								REC_MANAGER_PARENTAL_LOCKX, REC_MANAGER_PARENTAL_LOCKY, 
 								REC_MANAGER_PARENTAL_LOCKW, REC_MANAGER_PARENTAL_LOCKH, 
 								p_detail, 0);
  
    ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_text, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX, FSI_REC_MANAGER_INFO_MBOX);
    text_set_align_type(p_text, STL_CENTER | STL_VCENTER);
    text_set_offset(p_text, REC_MANAGER_TXT_OFFSETX, REC_MANAGER_TXT_OFFSETY);
    text_set_content_type(p_text, TEXT_STRTYPE_STRID);
    ctrl_set_sts(p_text, OBJ_STS_HIDE);
  }
#endif

  //help 
  ui_comm_help_create2(&help_data, p_menu,FALSE);
  //ui_comm_help_move_pos(p_menu, 50, 20, 880, 50, 42);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  param.pace = ROLL_SINGLE;
  param.style = ROLL_LR;
  param.repeats = 0;
  param.is_force = FALSE;

  if(ui_is_mute())
  {
    open_mute(0, 0);
  }
  else
  {
   set_volume_usb(sys_status_get_global_media_volume());
  }
  if(sys_status_get_curn_prog_mode()==CURN_MODE_RADIO)
    vdec_set_data_input(p_video_dev,0);
  gui_start_roll(p_list, &param);
  //g_b_op_able = FALSE;
  //g_media_player_preview_focus = g_pvr_param.focus;
  rec_manager_set_detail(p_list, TRUE, TRUE, TRUE);  

  //ui_time_enable_heart_beat(TRUE);   
  //ui_init_play_timer(ROOT_ID_RECORD_MANAGER, MSG_MEDIA_PLAY_TMROUT, 200);
  return SUCCESS;
}


BEGIN_MSGPROC(rec_manager_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_rec_manager_destory)
  ON_COMMAND(MSG_PLUG_OUT, on_rec_manager_plug_out)
  ON_COMMAND(MSG_PVR_FOCUS_CHANGED, on_rec_manager_focus_change)
  //ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, on_rec_manager_play_media)
  //ON_COMMAND(MSG_PLAYED, on_rec_manager_donothing)
END_MSGPROC(rec_manager_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(rec_manager_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_BLUE, MSG_BLUE)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(rec_manager_list_keymap, NULL)

BEGIN_MSGPROC(rec_manager_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_rec_manager_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_rec_manager_list_change_focus)
  ON_COMMAND(MSG_VOLUME_DOWN, on_rec_manager_list_change_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_rec_manager_list_change_volume)
  ON_COMMAND(MSG_PAGE_UP, on_rec_manager_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_rec_manager_list_change_focus)
  ON_COMMAND(MSG_GREEN, on_rec_manager_list_green)
  ON_COMMAND(MSG_BLUE, on_rec_manager_list_blue)
  ON_COMMAND(MSG_RED, on_rec_manager_list_red)
  ON_COMMAND(MSG_SELECT, on_rec_manager_list_select)
  ON_COMMAND(MSG_RENAME_CHECK, on_rename_check)
  ON_COMMAND(MSG_RENAME_UPDATE, on_rename_update)
  ON_COMMAND(MSG_PLAY_END, on_rec_manager_play_end)
  ON_COMMAND(MSG_REC_UNSUPPORT, on_rec_manager_unsupport)
  ON_COMMAND(MSG_PLAYED, on_rec_manager_played_time)
END_MSGPROC(rec_manager_list_proc, list_class_proc)

BEGIN_KEYMAP(rec_manager_pwd_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)   
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)   
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)   
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)   
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(rec_manager_pwd_keymap, NULL)

BEGIN_MSGPROC(rec_manager_pwd_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_rec_manager_pwd_close)
  ON_COMMAND(MSG_FOCUS_DOWN, on_rec_manager_pwd_close)
  ON_COMMAND(MSG_PAGE_UP, on_rec_manager_pwd_close)
  ON_COMMAND(MSG_PAGE_DOWN, on_rec_manager_pwd_close)
  ON_COMMAND(MSG_EXIT, on_rec_manager_pwd_close)
  ON_COMMAND(MSG_CORRECT_PWD, on_rec_manager_pwd_correct)  
END_MSGPROC(rec_manager_pwd_proc, cont_class_proc)
#endif

