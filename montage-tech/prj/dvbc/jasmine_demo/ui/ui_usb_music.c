/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#ifdef ENABLE_MUSIC_PIC_CONFIG
#include "ui_usb_music.h"
#include "ui_timer.h"
#include "ui_rename.h"
#include "lib_char.h"
#include "ui_volume_usb.h"
#include "ui_mute.h"
#include "lib_unicode.h"
#ifdef ENABLE_NETWORK
#include "ui_edit_usr_pwd.h"
#include "pnp_service.h"
#endif

enum mp_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_BLUE,
  MSG_GREEN,
  MSG_YELLOW,
  MSG_MUSIC_PLAY,
  MSG_MUSIC_RESUME,
  MSG_MUSIC_PAUSE,
  MSG_MUSIC_NEXT,
  MSG_MUSIC_PRE,
  MSG_MUSIC_STOP,
  MSG_ADD_TO_LIST,
  MSG_ONE_SECOND_UPDATE_PROCESS_BAR,
  MSG_ONE_SECOND_PLAY_STATUS,
  MSG_VOLUME_UP,
  MSG_VOLUME_DOWN,
  MSG_SORT,
  MSG_ADD_FAV,
  MSG_MUSIC_REC_NONE,
#ifdef ENABLE_NETWORK
  MSG_SWITCH_MEDIA_TYPE,
  MSG_MUSIC_UPFOLDER,
  MSG_MUSIC_BACK_TO_NETWORK_PLACE,
#endif
};

enum music_menu_ctrl_id
{
  IDC_MUSIC_PREVIEW = 1,
  IDC_MP_GROUP_ARROWL,
  IDC_MP_GROUP_ARROWR,
  IDC_MP_GROUP,
  IDC_MUSIC_LIST,
  IDC_MP_SBAR,
  IDC_MUSIC_PLAY_DETAIL_CONT,
  IDC_MP_PLAY_LIST_CONT,
  IDC_MP_SORT_LIST,
  IDC_MUSIC_BOTTOM_INFO,
  IDC_MP_HELP,
};

enum music_preview_ctrl_id
{
  IDC_MUSIC_PREVIEW_LOGO = 1,
};

enum mp_play_ctrl_id
{
  IDC_MP_PLAY_MODE = 1,
  IDC_MUSIC_PLAY_FILENAME,
  IDC_MP_PLAY_BOTTOM_LINE,
  IDC_MUSIC_PLAY_PROGRESS,
  IDC_MP_PLAY_CURTM,
  IDC_MP_PLAY_TOLTM,
  IDC_MP_PLAY_CONTROL_ICON,
};

enum music_fav_list_ctrl_id
{
  IDC_MUSIC_FAV_LIST_TITLE= 1,
  IDC_MUSIC_FAV_LIST_LIST,
  IDC_MUSIC_FAV_LIST_HELP,
  IDC_MUSIC_FAV_LIST_SBAR,
};


static list_xstyle_t mp_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t mp_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLUE,
  FSI_WHITE,
};

static list_xstyle_t mp_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t mp_list_attr[MP_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    50, 10, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    40, 60, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT ,
    340, 100, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
};

static list_field_attr_t mp_play_list_attr[MP_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    50, 10, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    40, 60, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT ,
    280, 100, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON | STL_LEFT,
    40, 380, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
};

static list_field_attr_t mp_sort_list_field_attr[MP_SORT_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
   200, 15, 0, &mp_list_field_rstyle, &mp_list_field_fstyle },
};

static rect_t p_dlg_rc =
{
  MP_DLG_L,
  MP_DLG_T,
  MP_DLG_R,
  MP_DLG_B,
};
static BOOL is_clear_del_icon = TRUE;
static BOOL music_fav_timer_over = TRUE;
static file_list_t g_list = {0};

static u16 g_ffilter_all[32] = {0};

static partition_t *p_partition = NULL;
static flist_dir_t g_flist_dir = NULL;
static u32 g_partition_cnt = 0;
static utc_time_t cur_play_time = {0};
static utc_time_t total_play_time = {0};
static u16 cur_music_filename[MAX_FILE_PATH] = {0};
#ifdef ENABLE_NETWORK
static u32 is_usb = 1;
static char ipaddress[128] = "";
static u16 ip_address_with_path[32] ={0};
#endif

static BOOL g_is_music_fav = FALSE;

static BOOL g_media_enter = FALSE;

static RET_CODE music_list_update(control_t* p_list, u16 start, u16 size, u32 context);
static RET_CODE music_fav_list_update(control_t* p_list, u16 start, u16 size, u32 context);

RET_CODE music_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

u16 music_list_keymap(u16 key);
u16 music_cont_keymap(u16 key);

RET_CODE music_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);
u16 music_fav_list_keymap(u16 key);
RET_CODE music_fav_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 music_sort_list_keymap(u16 key);
RET_CODE mpm_sort_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

#ifdef ENABLE_NETWORK
u16 music_network_list_keymap(u16 key);

RET_CODE music_network_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

extern RET_CODE pnp_svc_unmount(u16 *p_url);

u32 get_music_is_usb()//if is_usb is 0 means music entry from samba 
{
  return is_usb;
}
#endif

void ui_music_reset_curn(u16 *p_path)
{
  if(p_path == NULL)
  {
    return;
  }

  uni_strncpy(cur_music_filename, p_path, MAX_FILE_PATH);
}

static flist_sort_t music_get_sort_type( control_t *p_list )
{
    control_t *p_parent, *p_sort_list;
    u16 pos;
    flist_sort_t sort_type = DEFAULT_ORDER_MODE;
    p_parent = ctrl_get_parent(p_list);
    p_sort_list = ctrl_get_child_by_id(p_parent,IDC_MP_SORT_LIST);
    pos = list_get_focus_pos(p_sort_list);
    switch(pos)
    {
      case 0:
        sort_type = DEFAULT_ORDER_MODE;
        break;
    
      case 1:
        sort_type = A_Z_MODE;
        break;
    
      case 2:
        sort_type = Z_A_MODE;
        break;
    
      default:
        sort_type = DEFAULT_ORDER_MODE;
        break;
    }
    return sort_type;
}

void music_reset_time_info(u16 *name)
{
  u32 mp3_total_second = 0;

  mp3_total_second = parse_mp3_time_long(name);
  if (!mp3_total_second)
  {
   OS_PRINTF("\n##debug: mp3_total_second = %d!\n", mp3_total_second);
  }
  total_play_time.hour = (u8)(mp3_total_second/3600);
  total_play_time.minute = (u8)(mp3_total_second%3600/60);
  total_play_time.second = (u8)(mp3_total_second%60);

  memset(&cur_play_time, 0 ,sizeof(utc_time_t));
}

static void ui_list_start_roll(control_t *p_cont)
{
  roll_param_t p_param = {ROLL_LR, };
  MT_ASSERT(p_cont != NULL);

  gui_stop_roll(p_cont);

  p_param.pace = ROLL_SINGLE;
  p_param.style = ROLL_LR;
  p_param.repeats = 0;
  p_param.is_force = FALSE;

  gui_start_roll(p_cont, &p_param);

}

static void music_clear_play_info(control_t *p_list)
{
  control_t *p_play_cont, *p_play_curtm, *p_total_tm, *p_play_progress, *p_name;

  p_play_cont = ctrl_get_child_by_id(p_list->p_parent, IDC_MUSIC_PLAY_DETAIL_CONT);
  p_name = ctrl_get_child_by_id(p_play_cont, IDC_MUSIC_PLAY_FILENAME);
  p_play_curtm = ctrl_get_child_by_id(p_play_cont, IDC_MP_PLAY_CURTM);
  p_total_tm = ctrl_get_child_by_id(p_play_cont, IDC_MP_PLAY_TOLTM);
  p_play_progress = ctrl_get_child_by_id(p_play_cont, IDC_MUSIC_PLAY_PROGRESS);

  text_set_content_by_ascstr(p_play_curtm, (u8 *)"00:00");
  text_set_content_by_ascstr(p_total_tm, (u8 *)"00:00");
  pbar_set_current(p_play_progress, 0);
  text_set_content_by_unistr(p_name, (u16*)"");

  ctrl_paint_ctrl(p_play_progress, TRUE);
  ctrl_paint_ctrl(p_play_curtm, TRUE);
  ctrl_paint_ctrl(p_total_tm, TRUE);
  ctrl_paint_ctrl(p_name,TRUE);

  fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR);
  memset(&cur_play_time, 0 ,sizeof(utc_time_t));
  memset(&total_play_time, 0 ,sizeof(utc_time_t));
}

static void do_play_music(void)
{
  media_fav_t *p_media = NULL;
  OS_PRINTF("\n##debug: start_play_music_list!\n");

  if(ui_music_pre_start_play(&p_media))
  {
    fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR);
    music_reset_time_info(p_media->path);
    music_player_stop();
    music_player_start(p_media->path);
    fw_tmr_create(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR, 1000, TRUE);
  }
  if (!ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
}

static RET_CODE music_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
  //str_asc2uni(p_partition[focus].name, p_str);
	//dvb_to_unicode(p_partition[focus].name,max_length - 1, p_str, max_length);
#ifdef ENABLE_NETWORK
  if (! is_usb)
  {
    u8 ip[128] = "\\\\";
    strcat(ip,ipaddress);

    dvb_to_unicode(ip,max_length - 1, p_str, max_length);
  }
  else
#endif
  {
    uni_strncpy(p_str, p_partition[focus].name, max_length);
  }
  return SUCCESS;
}

static void music_update_prev_logo(control_t *p_list, BOOL is_show, u16 bmp_id)
{
  control_t *p_preview;
  control_t *p_prev_logo;

  p_preview = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_MUSIC_PREVIEW);
  p_prev_logo = ctrl_get_child_by_id(p_preview, IDC_MUSIC_PREVIEW_LOGO);

  ctrl_set_rstyle(p_preview, RSI_MP_PREV, RSI_MP_PREV, RSI_MP_PREV);

  if(is_show)
  {
    bmap_set_align_type(p_prev_logo, STL_CENTER|STL_VCENTER);
    bmap_set_content_by_id(p_prev_logo, bmp_id);

    ctrl_paint_ctrl(p_preview, TRUE);
  }
  else
  {
    ctrl_set_sts(p_prev_logo, OBJ_STS_HIDE);
    ctrl_paint_ctrl(p_preview, TRUE);
  }
}


static void music_update_bottom_info(control_t *p_list)
{
  control_t *p_bottom_info;
  u16 uni_str[MAX_FILE_PATH] = {0};

  p_bottom_info = ctrl_get_child_by_id(p_list->p_parent, IDC_MUSIC_BOTTOM_INFO);
  list_get_focus_pos(p_list);

  memset(uni_str, 0, sizeof(uni_str));
  //str_asc2uni(g_list.p_file[list_focus].name, uni_str);
  //dvb_to_unicode(file_list_get_cur_path(g_flist_dir),(MAX_FILE_PATH - 1), uni_str, MAX_FILE_PATH);
  uni_strncpy(uni_str, file_list_get_cur_path(g_flist_dir), MAX_FILE_PATH);

  text_set_content_by_unistr(p_bottom_info, uni_str);
  ctrl_paint_ctrl(p_bottom_info, TRUE);
}

static void music_update_play_file_name(control_t *p_list, BOOL is_paint)
{
  control_t *p_play_cont;
  control_t *p_name;
  u16 uni_str[128];
  media_fav_t *p_media = NULL;
  music_play_state_t play_status;
  u16 *p_temp = {0};

  p_play_cont = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_MUSIC_PLAY_DETAIL_CONT);
  p_name = ctrl_get_child_by_id(p_play_cont, IDC_MUSIC_PLAY_FILENAME);

  ui_music_build_play_list_infav();
  ui_music_get_cur(&p_media);
  if(p_media == NULL)
  {
    return;
  }

  play_status = ui_music_get_play_status();
  if((play_status == MUSIC_STAT_PLAY) || (play_status == MUSIC_STAT_PAUSE))
  {
    memset(uni_str, 0, sizeof(uni_str));
    p_temp = uni_strrchr(p_media->path, 0x5c/*'\\'*/);
    if(p_temp != NULL)
    {
      //str_asc2uni(++p_temp, uni_str);
		  //dvb_to_unicode(++p_temp,127, uni_str, 128);
		  uni_strncpy(uni_str, ++p_temp, 128);
    }

    text_set_content_by_unistr(p_name, uni_str);

    uni_strncpy(cur_music_filename, p_media->path, MAX_FILE_PATH);
  }
  else
  {
    text_set_content_by_unistr(p_name, (u16*)"");

    memset(cur_music_filename,0, MAX_FILE_PATH * sizeof(u16));
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_name, TRUE);
  }
}

static void music_update_play_status_icon(control_t *p_mbox, BOOL is_paint)
{
  u8 i;
  u16 play_icon_hl[MP_PLAY_MBOX_TOL] =
    {
      IM_MP3_ICON_FB_SELECT,
      IM_MP3_ICON_FF_SELECT,
      IM_MP3_ICON_PLAY_2_SELECT,
      IM_MP3_ICON_STOP_SELECT,
    };
  u16 play_icon_sh[MP_PLAY_MBOX_TOL] =
    {
      IM_MP3_ICON_FB,
      IM_MP3_ICON_FF,
      IM_MP3_ICON_PLAY_2,
      IM_MP3_ICON_STOP,
    };

  for(i = 0; i < MP_PLAY_MBOX_TOL; i++)
  {
    if((i==2) && (ui_music_get_play_status()==MUSIC_STAT_PAUSE))
    {
      mbox_set_content_by_icon(p_mbox, i, IM_MP3_ICON_PAUSE_SELECT, IM_MP3_ICON_PAUSE_SELECT);
      continue;
    }

    if( ui_music_get_play_status() == i)
    {
      mbox_set_content_by_icon(p_mbox, i, play_icon_hl[i], play_icon_hl[i]);
      if((i == MUSIC_STAT_FF) || (i == MUSIC_STAT_FB))
      {
        fw_tmr_create(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_PLAY_STATUS, 1000, FALSE);
      }
    }
    else
    {
      mbox_set_content_by_icon(p_mbox, i, play_icon_sh[i], play_icon_sh[i]);
    }
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_mbox, TRUE);
  }
}

static void play_list_update_help(control_t *p_mbox, BOOL is_paint)
{
  u8 i;
  u16 icon[MP_HELP_MBOX_TOL] =
    {
      IM_EPG_COLORBUTTON_RED,
      IM_EPG_COLORBUTTON_YELLOW,
      IM_EPG_COLORBUTTON_GREEN,
    };
  u16 str[MP_HELP_MBOX_TOL] =
    {
      IDS_PLAY,
      IDS_DELETE,
      IDS_DELETE_ALL,
    };

  for(i=0; i<MP_PLAY_LIST_HELP_MBOX_TOL; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, str[i]);
    mbox_set_content_by_icon(p_mbox, i, icon[i], icon[i]);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_mbox, TRUE);
  }
}

static RET_CODE on_music_update_play_time(control_t *p_cont, u16 msg,u32 para1, u32 para2)
{
  if(para1 == 0)
  {
    memset(&cur_play_time, 0, sizeof(utc_time_t));
  }
  else
  {
    cur_play_time.hour = (u8)(para1/3600);
    cur_play_time.minute = (u8)(para1%3600/60);
    cur_play_time.second = (u8)(para1%60);
  }
  return SUCCESS;
}


void ui_usb_music_exit()
{
  g_media_enter = FALSE;

  fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR);
  music_player_stop();
  if(g_flist_dir != NULL)
  {
    file_list_leave_dir(g_flist_dir);
    g_flist_dir = NULL;
  }
  ui_enable_chk_pwd(TRUE);
  //ui_music_unload_fav_list();
  ui_music_player_release();
  ui_music_set_play_status(MUSIC_STAT_INVALID);
  memset(cur_music_filename, 0, MAX_FILE_PATH * sizeof(u16));

#ifndef WIN32
  ui_music_release_region();
#endif
#ifndef MIN_AV_64M
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
}

static BOOL file_list_is_enter_dir(u16 *p_filter, u16 unit_cnt, u16 *p_path)
{
  u16 *p_temp = NULL;
  u16 parent[4] = {0};
  u16 curn[4] = {0};
  u16 cur_path[255] = {0};
  MT_ASSERT(p_path != NULL);

  uni_strcpy(cur_path, p_path);

  p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);

  if (p_temp != NULL)
  {
    //parent dir
    str_asc2uni((u8 *)"..", parent);
    str_asc2uni((u8 *)".", curn);
    if (uni_strlen(p_temp) >= 3 && uni_strcmp(p_temp + 1, parent/*".."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
      p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);
      if (p_temp != NULL)
      {
        p_temp[0] = 0/*'\0'*/;
      }
    }
    //cur dir
    else if (uni_strlen(p_temp) >= 2 && uni_strcmp(p_temp + 1, curn/*"."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}

static void file_list_update(control_t *p_list)
{
   int count = 0;
   int i = 0;
   music_play_state_t music_play_stat = MUSIC_STAT_INVALID;
   flist_sort_t sort_type = DEFAULT_ORDER_MODE;
   sort_type = music_get_sort_type(p_list); 

    file_list_leave_dir(g_flist_dir);
    g_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, g_list.p_file[0].name);
    if(g_flist_dir != NULL)
    {
      file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
      file_list_sort(&g_list, (int)g_list.file_count, sort_type);
      count = (int)g_list.file_count;

      list_set_count(p_list, (u16)(g_list.file_count), MUSIC_LIST_ITEM_NUM_ONE_PAGE);
      list_set_focus_pos(p_list, 0);
      music_play_stat = ui_music_get_play_status();
      if((music_play_stat == MUSIC_STAT_PLAY) || (music_play_stat == MUSIC_STAT_PAUSE))
      {
        for(i = 0; i < count; i++)
        {
           if(uni_strcmp(cur_music_filename,g_list.p_file[i].name) == 0)
           {
             list_set_focus_pos(p_list, (u16)i);
             list_select_item(p_list, (u16)i);
           }
        }
      }
      music_list_update(p_list, list_get_valid_pos(p_list), MUSIC_LIST_ITEM_NUM_ONE_PAGE, 0);
      ctrl_paint_ctrl(p_list, TRUE);
      ui_list_start_roll(p_list);
      music_update_bottom_info(p_list);
    }
}


static RET_CODE on_music_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list;
  //rect_t logo_rect = {0};
  BOOL is_enter_dir = FALSE;

  p_list = ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST);
  if (g_flist_dir != NULL &&  g_list.p_file != NULL)
  {
    is_enter_dir = file_list_is_enter_dir(g_ffilter_all, 100, g_list.p_file[0].name);
  }
  else
  {
    is_enter_dir = FALSE;
  }
  if(is_enter_dir)
  {
    file_list_update(p_list);//simon fix bug #27346
    return SUCCESS;
  }
  else
  {
    return ERR_NOFEATURE;
  }
  return ERR_NOFEATURE;
}

#ifdef ENABLE_NETWORK
static RET_CODE on_music_process_msg(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list_cont = NULL;
  control_t *p_cont = NULL;
  p_list_cont = ctrl_get_parent(p_list);
  if(MSG_MUSIC_BACK_TO_NETWORK_PLACE == msg)
  {
    manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);
    manage_open_menu(ROOT_ID_NETWORK_PLACES, 0, 0);
  }

  p_cont = ui_comm_root_get_root(ROOT_ID_USB_MUSIC);
  ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
  return SUCCESS;
}
#endif



static RET_CODE on_music_update_process_bar(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_play_cont, *p_play_curtm, *p_total_tm, *p_play_progress;
  char asc[32];
  static u16 percent = 0;
  u16 temp = 0;

  p_play_cont = ctrl_get_child_by_id(p_cont, IDC_MUSIC_PLAY_DETAIL_CONT);

  p_play_curtm = ctrl_get_child_by_id(p_play_cont, IDC_MP_PLAY_CURTM);
  p_total_tm = ctrl_get_child_by_id(p_play_cont, IDC_MP_PLAY_TOLTM);
  p_play_progress = ctrl_get_child_by_id(p_play_cont, IDC_MUSIC_PLAY_PROGRESS);

//  time_up(&cur_play_time, 1);

  if(cur_play_time.second > 59)
    cur_play_time.second = 0;

  sprintf(asc, "%02d:%02d", cur_play_time.minute, cur_play_time.second);
  text_set_content_by_ascstr(p_play_curtm, (u8 *)asc);

  sprintf(asc, "%02d:%02d", total_play_time.minute, total_play_time.second);
  text_set_content_by_ascstr(p_total_tm, (u8 *)asc);

  temp = percent;
  if(time_conver(&total_play_time) != 0)
  {
    percent = (u16)(time_conver(&cur_play_time)*100/time_conver(&total_play_time));
  }

  //OS_PRINTF("percent = %d\n",percent);

  pbar_set_current(p_play_progress, percent);

  if (temp != percent)
  {
    ctrl_paint_ctrl(p_play_progress, TRUE);
  }
  ctrl_paint_ctrl(p_play_curtm, TRUE);
  ctrl_paint_ctrl(p_total_tm, TRUE);

  if(time_cmp(&cur_play_time, &total_play_time, FALSE) >= 0)
  {
    cur_play_time.second--;
  }

  return SUCCESS;
}

u16 ui_music_get_play_dir_index(u16 *p_filename,u16 count)
{
    u16 index = 0;
    u16 i = 0;
    for(i = 0; i < count; i ++)
    {
       if(uni_strcmp(g_list.p_file[i].name, p_filename) == 0)
       {
         break;
       }

       index ++;
    }

    return index;

}

static void music_update_select_item(control_t *p_list)
{
  u16 i = 0;
  media_fav_t *p_media = NULL;
  music_playlist_type_t play_type = MUSIC_PLAY_CUR_DIR;
  u16 count = 0;
  play_type = ui_music_get_playlist_type();

  if(play_type != MUSIC_PLAY_CUR_DIR && play_type != MUSIC_PLAY_FAV_LIST)
  {
    return;
  }

  ui_music_get_cur(&p_media);

  if(p_media == NULL)
  {
    return;
  }

  if(p_list->id == IDC_MUSIC_LIST)
  {
    for(i = 0; i < g_list.file_count; i++)
    {
      if(uni_strcmp(p_media->path, g_list.p_file[i].name) == 0)
      {
        list_set_focus_pos(p_list, i);
        list_select_item(p_list, i);
        list_set_update(p_list, music_list_update, 0);

        ctrl_paint_ctrl(p_list, TRUE);

        return;
      }
    }
  }
  else
  {
    count = (u16)ui_music_get_fav_count();
    for(i = 0; i < count; i++)
    {
      if(uni_strcmp(p_media->path, ui_music_get_fav_media_by_index(i)->path) == 0)
      {
        list_set_focus_pos(p_list, i);
        list_select_item(p_list, i);
        ctrl_paint_ctrl(p_list, TRUE);
        return;
      }
    }
  }
  return;
}

static RET_CODE on_music_evt_play_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;
  control_t *p_list = NULL, *p_detail_cont = NULL, *p_ctrl_icon = NULL;
  //u16 pos = 0;

  if(!ui_music_pre_play_next(&p_media))
  {
	  music_player_stop();
    music_clear_play_info(ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST));

    p_detail_cont = ctrl_get_child_by_id(p_cont, IDC_MUSIC_PLAY_DETAIL_CONT);
    p_ctrl_icon = ctrl_get_child_by_id(p_detail_cont, IDC_MP_PLAY_CONTROL_ICON);
    music_update_play_status_icon(p_ctrl_icon, TRUE);
  }
  else
  {
    memset(cur_music_filename, 0, MAX_FILE_PATH * sizeof(u16));
    music_reset_time_info(p_media->path);

    p_list = ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST);
    //ui_music_build_play_list_indir(&g_list);

    //music_update_prev_logo(p_list, TRUE, IM_MP3_LOGO_MUSIC_SMALL);
    //list_set_focus_pos(p_list, g_music_play_index_dir + g_dir_count);
    //list_select_item(p_list, g_music_play_index_dir + g_dir_count);
    //list_set_update(p_list, music_list_update, 0);
    //ctrl_paint_ctrl(p_list, TRUE);
    if(ui_music_get_playlist_type() == MUSIC_PLAY_FAV_LIST)
    {
      control_t *p_fav_list = NULL;
      control_t *p_fav_list_cont = NULL;
      p_fav_list_cont = ctrl_get_child_by_id(p_list->p_parent, IDC_MP_PLAY_LIST_CONT);
      p_fav_list = ctrl_get_child_by_id(p_fav_list_cont, IDC_MUSIC_FAV_LIST_LIST);

      music_update_select_item(p_fav_list);
    }
    else
    {
      music_update_select_item(p_list);
    }

    music_update_play_file_name(p_list,TRUE);

    fw_tmr_create(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR,1000,TRUE);
    ui_music_set_play_status(MUSIC_STAT_PLAY);

    //strncpy(cur_music_filename,p_media->path,260);
    music_player_next(p_media->path);

  }

  return SUCCESS;
}

static RET_CODE on_music_evt_cannot_play(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  music_player_stop();
  music_clear_play_info(ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST));

  return SUCCESS;
}

static RET_CODE on_usb_music_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  //ui_music_logo_clear(0);

  ui_close_all_mennus();

  return SUCCESS;
}


static RET_CODE on_music_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list = NULL, *p_list_cont = NULL, *p_fav_list = NULL;

  g_media_enter = FALSE;
#ifdef ENABLE_NETWORK
  if(!is_usb)
  {
    u16 *p_url;
    ui_get_ip_path_mount(&p_url);
    pnp_service_vfs_unmount(p_url);
  }
#endif
  

  p_list = ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST);
  if(p_list != NULL)
  {
    gui_stop_roll(p_list);
  }

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_MP_PLAY_LIST_CONT);

  p_fav_list = ctrl_get_child_by_id(p_list_cont, IDC_MUSIC_FAV_LIST_LIST);
  if(p_fav_list != NULL)
  {
    gui_stop_roll(p_fav_list);
  }

  ui_music_unload_fav_list();

  return ERR_NOFEATURE;
}


static RET_CODE on_usb_rec_none(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE music_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i;
  u8 asc_str[32];
  u16 icon_type[5] =
  {
    IM_MP3_ICON_MP3, IM_MP3_ICON_JPG, IM_MP3_ICON_BMP, IM_MP3_ICON_FOLDER, RSC_INVALID_ID
  };
  u16 file_uniname[MAX_FILE_PATH] = {0};
  u16 cnt = list_get_count(p_list);
  flist_type_t type = FILE_TYPE_UNKNOW;

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      /* ICON */
      if(g_list.p_file[i + start].type != DIRECTORY)
      {
        type = flist_get_file_type(g_list.p_file[i + start].p_name);
        list_set_field_content_by_icon(p_list, (u16)(start + i), 1, icon_type[type]);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 1, IM_MP3_ICON_FOLDER);
      }

      /* NAME */
      //str_asc2uni(g_list.p_file[i + start].p_name, file_uniname);
      if(g_list.p_file[i + start].p_name != NULL)
      {
         //dvb_to_unicode(g_list.p_file[i + start].p_name,(MAX_FILE_PATH-1), file_uniname, MAX_FILE_PATH);
         uni_strncpy(file_uniname, g_list.p_file[i + start].p_name, MAX_FILE_PATH);
      }

      list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, file_uniname);
    }

  }

  return SUCCESS;
}

static RET_CODE music_fav_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i;
  u8 asc_str[32];
  u16 file_uniname[MAX_FILE_PATH];
  u16 cnt = list_get_count(p_list);
  media_fav_t *p_media = NULL;
  u16 *p_tip = NULL;

  ui_music_get_fav_list(&p_media);

  for (i = 0; i<start && p_media!=NULL; i++)
  {
    p_media = p_media->p_next;
  }

  for (i = 0; i < size; i++)
  {
    if ((i + start < cnt) && (p_media != NULL) )
    {
      /* NO. */
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      /* ICON TYPE*/
      flist_get_file_type(p_media->path);
      list_set_field_content_by_icon(p_list, (u16)(start + i), 1, IM_MP3_ICON_MP3);

      /* NAME */
      //str_asc2uni(p_media->path,file_uniname);
	    //if(p_media->p_filename != NULL)
	    //{
      // dvb_to_unicode(p_media->p_filename,(MAX_FILE_PATH - 1), file_uniname, MAX_FILE_PATH);
	    // }
	    //else
	    if(p_media->path != NULL)
	    {
        p_tip = uni_strrchr(p_media->path, '\\');
        uni_strncpy(file_uniname,(++p_tip), MAX_FILE_PATH);
	    }
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, file_uniname);

      /* ICON DEL*/
      if(ui_music_is_one_fav_del(start + i))
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_TV_DEL);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 3, RSC_INVALID_ID);
      }
    }

    if(p_media != NULL)
    {
      p_media = p_media->p_next;
    }
  }

  return SUCCESS;
}


static RET_CODE on_music_add_to_fav_list(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  int pos = 0;
  comm_dlg_data_t dlg_data = {0};

  pos = list_get_focus_pos(p_list);

  if(g_list.p_file == NULL)
  {
    return ret;
  }

  if(g_list.p_file[pos].type == NOT_DIR_FILE)
  {
    dlg_data.parent_root = ROOT_ID_USB_MUSIC;
    dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
    dlg_data.x = MP_DLG_L;
    dlg_data.y = MP_DLG_T;
    dlg_data.w = MP_DLG_W;
    dlg_data.h = MP_DLG_H;
    dlg_data.content = IDS_WAIT_PLEASE;
    dlg_data.dlg_tmout = 0;

    if(music_fav_timer_over){
  	   music_fav_timer_over = FALSE;

       ui_comm_dlg_open(&dlg_data);

       ui_music_add_one_fav(g_list.p_file[pos].name);
       ui_music_save_fav_list(g_list.p_file[0].name[0]);

       ui_comm_dlg_close();
       music_fav_timer_over = TRUE;
    } else {
    	 return ret;
    }
  }

  //set HighLight to the next item
  //list_class_proc(p_list, MSG_FOCUS_DOWN, 0, 0);
  //ui_list_start_roll(p_list);

  return SUCCESS;
}


static RET_CODE on_music_list_change_focus(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  ret = list_class_proc(p_list, msg, 0, 0);
  ui_list_start_roll(p_list);

  if(g_list.p_file == NULL)
  {
    return ret;
  }
  return ret;
}
static void music_update_path(control_t *p_list)
{
  control_t *p_path;
  u16 uni_str[MAX_FILE_PATH] = {0};

  p_path = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_MUSIC_BOTTOM_INFO);

  uni_strncpy(uni_str, file_list_get_cur_path(g_flist_dir), MAX_FILE_PATH);
  text_set_content_by_unistr(p_path, uni_str);

  ctrl_paint_ctrl(p_path, TRUE);
}

static RET_CODE on_music_list_change_partition(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cbox;
  u16 focus = 0;
  u16 count = 0;
  flist_sort_t sort_type = DEFAULT_ORDER_MODE;
  sort_type = music_get_sort_type(p_list);

  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_MP_GROUP);
  cbox_class_proc(p_cbox, MSG_INCREASE, 0, 0);
  focus = cbox_dync_get_focus(p_cbox);

  if(g_partition_cnt > 1)
  {
    if(g_flist_dir == NULL)
       return ERR_FAILURE;

    file_list_leave_dir(g_flist_dir);
    g_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, p_partition[focus].letter);
    if(g_flist_dir != NULL)
    {
        file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
        file_list_sort(&g_list, (int)g_list.file_count, sort_type);
        count = (u16)g_list.file_count;

        list_set_count(p_list, count, MUSIC_LIST_ITEM_NUM_ONE_PAGE);
        list_set_focus_pos(p_list, 0);
        list_select_item(p_list, 0);

        music_list_update(p_list, list_get_valid_pos(p_list), MUSIC_LIST_ITEM_NUM_ONE_PAGE, 0);
        //ui_music_build_play_list_indir(&g_list); //change_partition not build list
        ctrl_paint_ctrl(p_list, TRUE);
        music_update_path(p_list);
    }
    else
    {
       OS_PRINTF("\n file_list_enter_dir failure. \n");
    }

  }

  return SUCCESS;
}

static RET_CODE on_music_change_volume(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
    open_volume_usb(ROOT_ID_USB_MUSIC, para1);
    return SUCCESS;
}

//lint -e656
static void music_list_change_play_mode()
{
  mlist_play_mode_t mode;
  mode = ui_music_get_play_mode();
  OS_PRINTF("%s,line:%d,mode:%d\n",__FUNCTION__,__LINE__,mode);
  switch(mode)
  {
    case PLAY_MODE_CYCLE_CUR_DIR_ORDER:
    case PLAY_MODE_CYCLE_CUR_DIR_RANDOM:
    case PLAY_MODE_CYCLE_CUR_DIR_SELF:
    case PLAY_MODE_CYCLE_CUR_DIR_NONE:
      ui_music_set_play_mode((mlist_play_mode_t)((++mode) % (PLAY_MODE_CYCLE_CUR_DIR_NONE + 1)));
      break;

    case PLAY_MODE_CYCLE_FAV_LIST_ORDER:
    case PLAY_MODE_CYCLE_FAV_LIST_RANDOM:
    case PLAY_MODE_CYCLE_FAV_LIST_SELF:
    case PLAY_MODE_CYCLE_FAV_LIST_NONE:
      ui_music_set_play_mode((mlist_play_mode_t)((++mode) % (PLAY_MODE_CYCLE_FAV_LIST_NONE - PLAY_MODE_CYCLE_FAV_LIST_ORDER+1)+PLAY_MODE_CYCLE_FAV_LIST_ORDER));
      break;

    default:
      return;
  }

  return;
}
//lint +e656

static void update_music_list_play_mode(control_t *p_list)
{
  control_t *p_play_detail_cont, *p_play_mode;
  /*
  static u8 folderidx = 0;
  u16 icon_folder[] =
  {
    IM_MP3_ICON_PLAY_1, IM_MP3_ICON_PLAY_R,
    IM_MP3_ICON_SINGLE_R, IM_MP3_ICON_NO_REPEAT,
  };
  */

  u16 icon_list[] =
  {
    IM_MP3_ICON_LISTPLAY, IM_MP3_ICON_LISTPLAY_R,
    IM_MP3_ICON_SINGLE_R, IM_MP3_ICON_NO_REPEAT,
  };
  static u8 listidx = 0;

  p_play_detail_cont = ctrl_get_child_by_id(p_list, IDC_MUSIC_PLAY_DETAIL_CONT);
  MT_ASSERT(p_play_detail_cont != NULL);
  p_play_mode = ctrl_get_child_by_id(p_play_detail_cont, IDC_MP_PLAY_MODE);
  MT_ASSERT(p_play_mode != NULL);

  listidx++;
  listidx %= sizeof(icon_list)/sizeof(u16);

  bmap_set_content_by_id(p_play_mode, icon_list[listidx]);

  ctrl_paint_ctrl(p_play_detail_cont, TRUE);

  music_list_change_play_mode();
}


static RET_CODE on_music_list_change_play_mode(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  update_music_list_play_mode(p_list);

  return SUCCESS;
}

static RET_CODE on_music_list_open_fav_list(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_fav_list_cont, *p_play_list, *p_play_list_title;
  u16 count = 0, i = 0;
  roll_param_t param = {ROLL_LR, };

  comm_dlg_data_t dlg_data_notexist =
  {
    ROOT_ID_USB_MUSIC,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    MP_DLG_L,
    MP_DLG_T,
    MP_DLG_W,
    MP_DLG_H,
    IDS_PLAY_LIST_NOT_EXIST,
    2000,
    FALSE,
    0,
    {0},
  };

  p_fav_list_cont = ctrl_get_child_by_id(p_list->p_parent, IDC_MP_PLAY_LIST_CONT);
  p_play_list = ctrl_get_child_by_id(p_fav_list_cont, IDC_MUSIC_FAV_LIST_LIST);
  p_play_list_title = ctrl_get_child_by_id(p_fav_list_cont, IDC_MUSIC_FAV_LIST_TITLE);

  count = (u16)ui_music_get_fav_count();
  if(count > 0)
  {
    gui_stop_roll(p_list);
    g_is_music_fav = TRUE;
    ctrl_set_attr(p_fav_list_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_fav_list_cont, OBJ_STS_SHOW);

    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);


    text_set_content_by_strid(p_play_list_title, IDS_MUSIC_PLAY_LIST);
    ctrl_set_active_ctrl(p_fav_list_cont, p_play_list);

    list_set_count(p_play_list, count, MUSIC_FAV_LIST_MAX_ITEM_NUME);
    list_set_focus_pos(p_play_list, 0);
    //list_select_item(p_play_list, 0);
    music_fav_list_update(p_play_list, list_get_valid_pos(p_play_list), MUSIC_FAV_LIST_MAX_ITEM_NUME, 0);

	//enter fav list,select the music which is playing
    if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
    {
      media_fav_t *p_media = NULL;
      ui_music_get_fav_list(&p_media);

      for(i = 0; (i < count) && (p_media != NULL); i++)
      {
        if(uni_strcmp(cur_music_filename,p_media->path) == 0)
        {
          list_set_focus_pos(p_play_list, i);
          list_select_item(p_play_list, i);
          break;
        }
        p_media = p_media->p_next;
      }
    }

    ctrl_process_msg(p_play_list, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_parent(p_list), TRUE);

     // empty invrgn but not draw
    //ctrl_empty_invrgn(p_list);

    param.pace = ROLL_SINGLE;
    param.style = ROLL_LR;
    param.repeats = 0;
    param.is_force = FALSE;
    gui_start_roll(p_play_list, &param);

    ui_music_set_playlist_type(MUSIC_PLAY_FAV_LIST);
  }
  else
  {
    ui_comm_dlg_open(&dlg_data_notexist);
  }

  return SUCCESS;
}

static RET_CODE on_music_list_sort(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  control_t *p_cont, *p_sort_list;

  p_cont = p_list->p_parent;

  ui_comm_dlg_close();
  gui_stop_roll(p_list);
  p_sort_list = ctrl_get_child_by_id(p_cont, IDC_MP_SORT_LIST);
  ctrl_set_attr(p_sort_list, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_sort_list, OBJ_STS_SHOW);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_sort_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_sort_list, TRUE);

  return ret;
}

static RET_CODE on_music_list_select(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  int pos = 0;
  int count = 0;
  int i = 0;
  music_play_state_t music_play_stat = MUSIC_STAT_INVALID;

  control_t *p_play_container;
  control_t *p_playicon_mbox;
  rect_t logo_rect = {0};

  flist_sort_t sort_type = DEFAULT_ORDER_MODE;
  sort_type = music_get_sort_type(p_list);

  p_play_container = ctrl_get_child_by_id(p_list->p_parent, IDC_MUSIC_PLAY_DETAIL_CONT);
  p_playicon_mbox = ctrl_get_child_by_id(p_play_container, IDC_MP_PLAY_CONTROL_ICON);

  pos = list_get_focus_pos(p_list);
  gui_stop_roll(p_list);

  if(g_list.p_file == NULL)
  {
    return ret;
  }

  if((g_list.p_file[pos].type == NOT_DIR_FILE) &&
     (flist_get_file_type(g_list.p_file[pos].p_name) != FILE_TYPE_MP3))
    return ERR_FAILURE;

  if((uni_strcmp(cur_music_filename,g_list.p_file[pos].name) == 0) && (g_list.p_file[pos].type == NOT_DIR_FILE))
  {
    //clean pic region, stop pic filter first
    ui_music_logo_show(FALSE, logo_rect);
    ui_music_logo_clear(0);
    ui_music_fullscreen_player(g_list.p_file[pos].p_name);

    manage_open_menu(ROOT_ID_USB_MUSIC_FULLSCREEN, 0, 0);

    uni_strncpy(cur_music_filename, g_list.p_file[pos].name, MAX_FILE_PATH);

    return SUCCESS;
  }

  switch(g_list.p_file[pos].type)
  {
    case DIRECTORY:
    {
       if(g_flist_dir == NULL)
          return ERR_FAILURE;

      file_list_leave_dir(g_flist_dir);
      g_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, g_list.p_file[pos].name);
      if(g_flist_dir != NULL)
      {
          file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
          file_list_sort(&g_list, (int)g_list.file_count, sort_type);
          count = (int)g_list.file_count;

          list_set_count(p_list, (u16)(g_list.file_count), MUSIC_LIST_ITEM_NUM_ONE_PAGE);
          list_set_focus_pos(p_list, 0);
          //list_select_item(p_list, 0);

          music_play_stat = ui_music_get_play_status();
          if((music_play_stat == MUSIC_STAT_PLAY) || (music_play_stat == MUSIC_STAT_PAUSE))
          {
            for(i = 0; i < count; i++)
            {
               if(uni_strcmp(cur_music_filename, g_list.p_file[i].name) == 0)
               {
                 list_set_focus_pos(p_list, (u16)i);
                 list_select_item(p_list, (u16)i);
               }

            }

          }

          music_list_update(p_list, list_get_valid_pos(p_list), MUSIC_LIST_ITEM_NUM_ONE_PAGE, 0);
          ctrl_paint_ctrl(p_list, TRUE);
          ui_list_start_roll(p_list);
          music_update_bottom_info(p_list);

      }
      else
      {
         OS_PRINTF("\n file_list_enter_dir failure. \n");
      }

    }
    break;

    case NOT_DIR_FILE:
    {
      music_update_prev_logo(p_list, FALSE, 0);
      //ui_music_set_play_mode(PLAY_MODE_CYCLE_CUR_DIR_ORDER);
      ui_music_set_playlist_type(MUSIC_PLAY_CUR_DIR);
      ui_music_build_play_list_indir(&g_list);
      ui_music_set_play_index_dir_by_name(g_list.p_file[pos].name);
      do_play_music();
      music_update_play_status_icon(p_playicon_mbox, TRUE);

	  //strncpy(cur_music_filename,g_list.p_file[pos].name,260);
      music_update_play_file_name(p_list, TRUE);
      g_is_music_fav = FALSE;

      list_class_proc(p_list, msg, para1, para2);
      ctrl_paint_ctrl(p_list, TRUE);
      ui_list_start_roll(p_list);
    }
    break;

  default:
    break;
  }

  return SUCCESS;
}

static RET_CODE on_music_fav_list_del(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus;
  focus = list_get_focus_pos(p_list);
  ui_music_set_one_del(focus);

  is_clear_del_icon = 0;
  music_fav_list_update(p_list, list_get_valid_pos(p_list), MUSIC_FAV_LIST_MAX_ITEM_NUME, 0);
  OS_PRINTF("FYANG_cur pos:    ===%d, list_get_valid_pos(p_list) == %d\n",focus,list_get_valid_pos(p_list));
  list_class_proc(p_list, MSG_FOCUS_DOWN, 0, 0);
  
  if(list_get_count(p_list) == 1)
    list_draw_item_ext(p_list, focus, TRUE);

  return SUCCESS;
}

static RET_CODE on_music_fav_list_del_all(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 count = 0, index = 0;
  count = list_get_count(p_list);
  if(is_clear_del_icon == 0)
  {
    is_clear_del_icon = 1;
    ui_music_undo_save_del();
  }

  for(index = 0; index < count; index++)
  {
    ui_music_set_one_del(index);
  }
  music_fav_list_update(p_list, list_get_valid_pos(p_list), MUSIC_FAV_LIST_MAX_ITEM_NUME, 0);
  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static void music_fav_list_save_del()
{
    u16 i;
    for(i=0; i<g_partition_cnt; i++)
    {
        ui_music_save_del((u8)p_partition[i].letter[0]);
    }
    ui_music_build_play_list_infav();
    ui_music_set_del_modified(FALSE);
}

static void music_fav_list_undo_save_del()
{
  ui_music_undo_save_del();
}

//lint -e539
static RET_CODE on_music_fav_list_select(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0, i = 0;
  media_fav_t *p_focus_media;
  control_t *p_fav_cont = NULL, *p_cont = NULL;
  control_t *p_music_list = NULL;
  control_t *p_play_container;
  control_t *p_playicon_mbox;
  u16 count=0;
  BOOL b_save = FALSE;
  music_play_state_t music_play_stat = MUSIC_STAT_INVALID;
  rect_t logo_rect =
  {
    MUSIC_PREV_X + 8, MUSIC_PREV_Y + 8,
    MUSIC_PREV_X + MUSIC_PREV_W - 16,
    MUSIC_PREV_Y + MUSIC_PREV_H - 16
  };
  p_fav_cont = ctrl_get_parent(p_list);
  p_cont = ctrl_get_parent(p_fav_cont);
  p_music_list = ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST);
  p_play_container = ctrl_get_child_by_id(p_cont, IDC_MUSIC_PLAY_DETAIL_CONT);
  p_playicon_mbox = ctrl_get_child_by_id(p_play_container, IDC_MP_PLAY_CONTROL_ICON);

  focus = list_get_focus_pos(p_list);
  music_update_prev_logo(p_music_list, FALSE, 0);
  ui_music_build_play_list_infav();
  p_focus_media = ui_music_get_fav_media_by_index(focus);
  if(p_focus_media == NULL)
  return ERR_FAILURE;

  if(ui_music_is_del_modified())
  {
    b_save = ui_comm_ask_for_savdlg_open(&p_dlg_rc, IDS_MSG_ASK_FOR_SAV,
                                  music_fav_list_save_del, music_fav_list_undo_save_del, 0);
  }

  if(fw_find_root_by_id(ROOT_ID_USB_MUSIC) == NULL)
  {
    return ERR_FAILURE;
  }


  ui_music_build_play_list_infav();
  count = (u16)ui_music_get_fav_count();
  ctrl_set_attr(p_fav_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_fav_cont, OBJ_STS_SHOW);

  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  list_set_count(p_list, count, MUSIC_FAV_LIST_MAX_ITEM_NUME);
  ctrl_set_active_ctrl(p_fav_cont, p_list);
  music_fav_list_update(p_list, list_get_valid_pos(p_list), MUSIC_FAV_LIST_MAX_ITEM_NUME, 0);

  ui_music_set_playlist_type(MUSIC_PLAY_FAV_LIST);

  if(ui_music_get_play_count_of_fav() > 0)
  {
      //ui_music_set_play_mode(PLAY_MODE_CYCLE_FAV_LIST_ORDER);
      if(b_save)
      {
        for(i = 0; i < count; i++)
        {
           if(ui_music_get_fav_media_by_index(i) == NULL)
             break;

           if(uni_strcmp(p_focus_media->path, ui_music_get_fav_media_by_index(i)->path) == 0)
           {
             list_set_focus_pos(p_list, i);
             list_select_item(p_list, i);
             break;
           }
        }
      }
      else
      {
         list_class_proc(p_list, msg, para1, para2);
      }     
	  ui_music_build_play_list_infav();
	  if(ui_music_set_play_index_fav_by_name(p_focus_media->path) == FALSE)
	  {
		  ui_music_set_play_index_fav(0);
		  list_set_focus_pos(p_list, 0);
		  list_select_item(p_list, 0);
	  }

	  do_play_music();
	  music_update_play_status_icon(p_playicon_mbox, TRUE);
	  music_update_play_file_name(p_music_list, TRUE);

      ctrl_paint_ctrl(ctrl_get_parent(p_list), TRUE);
  }
  else
  {
    gui_stop_roll(p_list);
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_music_list, MSG_GETFOCUS, 0, 0);

    ctrl_set_sts(p_fav_cont, OBJ_STS_HIDE);

    ctrl_paint_ctrl(p_cont, TRUE);

    ui_music_logo_show(TRUE, logo_rect);

    music_fav_list_update(p_list, list_get_valid_pos(p_list), MUSIC_FAV_LIST_MAX_ITEM_NUME, 0);

    count = (u16)g_list.file_count;

    music_play_stat = ui_music_get_play_status();


    music_list_update(p_music_list, list_get_valid_pos(p_list), MUSIC_LIST_ITEM_NUM_ONE_PAGE, 0);

    if((music_play_stat == MUSIC_STAT_PLAY) || (music_play_stat == MUSIC_STAT_PAUSE))
    {
      for(i = 0; i < count; i++)
      {
         if(uni_strcmp(cur_music_filename, g_list.p_file[i].name) == 0)
         {
           list_set_focus_pos(p_music_list, i);
           list_select_item(p_music_list, i);
           break;
         }
      }
      if(count == i)
      {
        list_unselect_item(p_music_list, list_get_focus_pos(p_music_list));
      }

    }

    ctrl_paint_ctrl(p_music_list, TRUE);

    //ui_music_set_playlist_type(MUSIC_PLAY_CUR_DIR);

    //ui_music_build_play_list_indir(&g_list);
    //ui_music_set_play_index_dir_by_name(g_list.p_file[i].name);

    ui_list_start_roll(p_music_list);
  }

  return SUCCESS;
}
//lint +e539

static RET_CODE on_music_fav_list_exit(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  int count = 0, i = 0;
  control_t *p_cont = NULL, *p_mp_list = NULL;
  music_play_state_t music_play_stat = MUSIC_STAT_INVALID;

   rect_t logo_rect =  {MUSIC_PREV_X + COMM_WIN_SHIFT_X + 8, MUSIC_PREV_Y + COMM_WIN_SHIFT_Y + 8,
                       MUSIC_PREV_X + COMM_WIN_SHIFT_X + MUSIC_PREV_W - 16,
                       MUSIC_PREV_Y + COMM_WIN_SHIFT_Y + MUSIC_PREV_H - 16};

  gui_stop_roll(p_list);

  if(ui_music_is_del_modified())
  {
    ui_comm_ask_for_savdlg_open(&p_dlg_rc, IDS_MSG_ASK_FOR_SAV,
                                  music_fav_list_save_del, music_fav_list_undo_save_del, 0);
  }

  if(fw_find_root_by_id(ROOT_ID_USB_MUSIC) == NULL)
  {
    return ERR_FAILURE;
  }

  p_cont = p_list->p_parent->p_parent;
  p_mp_list = ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_list->p_parent, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  ui_music_logo_show(TRUE, logo_rect);

  //fw_find_root_by_id(ROOT_ID_USB_MUSIC);
  //p_menu = ctrl_get_child_by_id(control_t * p_parent, IDC_COMM_ROOT_CONT)
  music_fav_list_update(p_list, list_get_valid_pos(p_list), MUSIC_FAV_LIST_MAX_ITEM_NUME, 0);

  count = (int)g_list.file_count;

  music_play_stat = ui_music_get_play_status();


  music_list_update(p_mp_list, list_get_valid_pos(p_list), MUSIC_LIST_ITEM_NUM_ONE_PAGE, 0);

  if((music_play_stat == MUSIC_STAT_PLAY) || (music_play_stat == MUSIC_STAT_PAUSE))
  {
    for(i = 0; i < count; i++)
    {
       if(uni_strcmp(cur_music_filename,g_list.p_file[i].name) == 0)
       {
         list_set_focus_pos(p_mp_list, (u16)i);
         list_select_item(p_mp_list, (u16)i);
         break;
       }
    }
    if(count == i)
    {
      list_unselect_item(p_mp_list,list_get_focus_pos(p_mp_list));
    }

  }

  ctrl_paint_ctrl(p_mp_list, TRUE);

  //ui_music_set_playlist_type(MUSIC_PLAY_CUR_DIR);
  //ui_music_build_play_list_indir(&g_list);
  //ui_music_set_play_index_dir_by_name(g_list.p_file[i].name);

  ui_list_start_roll(p_mp_list);

  return SUCCESS;
}

static RET_CODE on_music_fav_list_change_play_mode(control_t *p_list, u16 msg,
												   u32 para1, u32 para2)
{
	control_t *p_cont = NULL;

	if(p_list->p_parent)
	{
		p_cont = p_list->p_parent->p_parent;
		MT_ASSERT(p_cont != NULL);

    update_music_list_play_mode(p_cont);

	}

  return SUCCESS;
}

static void do_play_pre_music(control_t *p_list, music_play_state_t state)
{
  u16 play_index;
  u8  play_count;
  comm_dlg_data_t dlg_data_notexist =
  {
    ROOT_ID_USB_MUSIC,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    MP_DLG_L,
    MP_DLG_T,
    MP_DLG_W,
    MP_DLG_H,
    IDS_PLAY_LIST_NOT_EXIST,
    2000,
    FALSE,
    0,
    {0},
  };

  if(g_is_music_fav)
  {
    ui_music_build_play_list_infav();
    play_index = ui_music_get_play_index_fav();
    play_count = ui_music_get_play_count_of_fav();
    if( play_count > 0 )
    {
        if(play_index == 0)
        {
           play_index = play_count - 1;
        }
        else
        {
           play_index--;
        }
        play_index %= play_count;
        ui_music_set_play_index_fav(play_index);
        do_play_music();
    }
    else
    {
        ui_music_set_play_status(state);
        ui_comm_dlg_open(&dlg_data_notexist);
    }

  }
  else
  {
    play_index = ui_music_get_play_index_dir();
    play_count = ui_music_get_play_count_of_dir();
    if(play_index == 0)
    {
       play_index = play_count - 1;
    }
    else
    {
       play_index--;
    }
    play_index %= play_count;
    ui_music_set_play_index_dir(play_index);
    do_play_music();
  }

}

static void do_play_next_music(control_t *p_list, music_play_state_t state)
{
    u16 play_index;
    u8  play_count;
    comm_dlg_data_t dlg_data_notexist =
    {
        ROOT_ID_USB_MUSIC,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        MP_DLG_L,
        MP_DLG_T,
        MP_DLG_W,
        MP_DLG_H,
        IDS_PLAY_LIST_NOT_EXIST,
        2000,
        FALSE,
        0,
        {0},
    };
    if(g_is_music_fav)
    {
      ui_music_build_play_list_infav();
      play_index = ui_music_get_play_index_fav();
      play_count = ui_music_get_play_count_of_fav();
      if( play_count >0 )
      {
          play_index++;
          play_index %= play_count;
          ui_music_set_play_index_fav(play_index);
          do_play_music();
      }
      else
      {
          ui_music_set_play_status(state);
          ui_comm_dlg_open(&dlg_data_notexist);
      }
    }
    else
    {
      play_index = ui_music_get_play_index_dir();
      play_count = ui_music_get_play_count_of_dir();
      play_index++;
      play_index %= play_count;
      ui_music_set_play_index_dir(play_index);
      do_play_music();
    }

}

static void on_music_list_play(control_t *p_list)
 {
  int pos = 0;

  control_t *p_play_container;
  control_t *p_playicon_mbox;

  p_play_container = ctrl_get_child_by_id(p_list->p_parent, IDC_MUSIC_PLAY_DETAIL_CONT);
  p_playicon_mbox = ctrl_get_child_by_id(p_play_container, IDC_MP_PLAY_CONTROL_ICON);

  pos = list_get_focus_pos(p_list);
  gui_stop_roll(p_list);

  if(g_list.p_file == NULL)
  {
    return ;
  }

  if((g_list.p_file[pos].type == NOT_DIR_FILE) &&
     (flist_get_file_type(g_list.p_file[pos].p_name) != FILE_TYPE_MP3))
    return ;


  switch(g_list.p_file[pos].type)
  {
    case NOT_DIR_FILE:
    {
      //music_update_prev_logo(p_list, TRUE, IM_MP3_LOGO_MUSIC_SMALL);

      //ui_music_set_play_mode(PLAY_MODE_CYCLE_CUR_DIR_ORDER);
      ui_music_set_playlist_type(MUSIC_PLAY_CUR_DIR);
      ui_music_build_play_list_indir(&g_list);
      ui_music_set_play_index_dir_by_name(g_list.p_file[pos].name);
      do_play_music();
      music_update_play_status_icon(p_playicon_mbox, TRUE);

	  //strncpy(cur_music_filename,g_list.p_file[pos].name,260);
      music_update_play_file_name(p_list, TRUE);
      g_is_music_fav = FALSE;

      list_class_proc(p_list, MSG_SELECT, 0, 0);
      ctrl_paint_ctrl(p_list, TRUE);
      ui_list_start_roll(p_list);
    }
    break;

  default:
    break;
  }

}

static RET_CODE on_music_list_change_play_status(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_play_container;
  control_t *p_playicon_mbox;
  music_play_state_t play_status;

  play_status = ui_music_get_play_status();
  p_play_container = ctrl_get_child_by_id(p_list->p_parent, IDC_MUSIC_PLAY_DETAIL_CONT);
  p_playicon_mbox = ctrl_get_child_by_id(p_play_container, IDC_MP_PLAY_CONTROL_ICON);

  switch(msg)
  {
  case MSG_MUSIC_PLAY:
    ui_music_set_play_status(MUSIC_STAT_PLAY);
    break;

  case MSG_MUSIC_RESUME:
    if(play_status == MUSIC_STAT_PAUSE)
    {
      music_player_resume();
      fw_tmr_create(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR, 1000, TRUE);
      ui_music_set_play_status(MUSIC_STAT_PLAY);
    }
    else if(play_status == MUSIC_STAT_PLAY)
    {
      music_player_pause();
      fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR);
      ui_music_set_play_status(MUSIC_STAT_PAUSE);
    }
    else if((play_status == MUSIC_STAT_INVALID) || (play_status == MUSIC_STAT_STOP))
    {
      on_music_list_play(p_list);
    }
    break;

  case MSG_MUSIC_PAUSE:
    if(play_status == MUSIC_STAT_PLAY)
    {
      music_player_pause();
      fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR);
      ui_music_set_play_status(MUSIC_STAT_PAUSE);
    }
    else if(play_status == MUSIC_STAT_PAUSE)
    {
      music_player_resume();
      fw_tmr_create(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR, 1000, TRUE);
      ui_music_set_play_status(MUSIC_STAT_PLAY);
    }
    else if((play_status == MUSIC_STAT_INVALID) || (play_status == MUSIC_STAT_STOP))
    {
      on_music_list_play(p_list);
    }
    break;

  case MSG_MUSIC_PRE:
    if((play_status == MUSIC_STAT_PLAY) || (play_status == MUSIC_STAT_PAUSE) || (play_status == MUSIC_STAT_STOP))
    {
      ui_music_set_play_status(MUSIC_STAT_FB);
      music_update_play_status_icon(p_playicon_mbox, TRUE);
      do_play_pre_music(p_list, play_status);
      music_update_play_file_name(p_list, TRUE);
      music_update_select_item(p_list);
    }
    break;

  case MSG_MUSIC_NEXT:
    if((play_status == MUSIC_STAT_PLAY) || (play_status == MUSIC_STAT_PAUSE) || (play_status == MUSIC_STAT_STOP))
    {
      ui_music_set_play_status(MUSIC_STAT_FF);
      music_update_play_status_icon(p_playicon_mbox, TRUE);
      do_play_next_music(p_list, play_status);
      music_update_play_file_name(p_list, TRUE);
      music_update_select_item(p_list);
    }
    break;

  case MSG_MUSIC_STOP:
    if((play_status == MUSIC_STAT_PLAY) || (play_status == MUSIC_STAT_PAUSE))
    {
      music_player_stop();
      ui_music_set_play_status(MUSIC_STAT_STOP);
      music_clear_play_info(p_list);

      memset(cur_music_filename, 0, MAX_FILE_PATH * sizeof(u16));
    }
    break;

  default:
    break;
  }

  music_update_play_status_icon(p_playicon_mbox, TRUE);

  return SUCCESS;
}


static RET_CODE on_one_second_change_play_status(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  control_t *p_play_container;
  control_t *p_playicon_mbox;

  p_play_container = ctrl_get_child_by_id(p_list->p_parent, IDC_MUSIC_PLAY_DETAIL_CONT);
  p_playicon_mbox = ctrl_get_child_by_id(p_play_container, IDC_MP_PLAY_CONTROL_ICON);

  p_play_container = ctrl_get_child_by_id(p_list->p_parent, IDC_MUSIC_PLAY_DETAIL_CONT);
  p_playicon_mbox = ctrl_get_child_by_id(p_play_container, IDC_MP_PLAY_CONTROL_ICON);

  music_update_play_status_icon(p_playicon_mbox, TRUE);

  fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_PLAY_STATUS);

  return ret;
}

static RET_CODE music_sort_list_update(control_t* p_list, u16 start, u16 size,
                                 u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u16 strid [MUSIC_SORT_LIST_ITEM_TOL] = {
    IDS_SORT_DEFAULT,
    IDS_SORT_NAME_AZ,
    IDS_SORT_NAME_ZA,
  };

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_strid(p_list, (u16)(start + i), 0,
                                      strid[start + i]);
    }
  }
  return SUCCESS;
}

static RET_CODE on_sort_list_select(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_mp_list;
  u16 pos;
  u32 i;
  flist_sort_t sort_type = DEFAULT_ORDER_MODE;

  p_cont = p_sort_list->p_parent;
  p_mp_list = ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST);

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  if(g_list.p_file == NULL)
  {
    return ret;
  }

  pos = list_get_focus_pos(p_sort_list);
  switch(pos)
  {
    case 0:
      sort_type = DEFAULT_ORDER_MODE;
      break;

    case 1:
      sort_type = A_Z_MODE;
      break;

    case 2:
      sort_type = Z_A_MODE;
      break;

    default:
      break;
  }
  file_list_sort(&g_list, (int)g_list.file_count, sort_type);
  music_list_update(p_mp_list, list_get_valid_pos(p_mp_list), MUSIC_LIST_ITEM_NUM_ONE_PAGE, 0);

  for(i = 0; i < g_list.file_count; i++)
  {
    if(uni_strcmp(cur_music_filename,g_list.p_file[i].name) == 0)
    {
      list_set_focus_pos(p_mp_list, (u16)i);
      list_select_item(p_mp_list, (u16)i);
    }
  }
  ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_mp_list, TRUE);

  ui_list_start_roll(p_mp_list);

  return ret;
}

static RET_CODE on_music_sort_list_exit(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_mp_list;

  p_cont = p_sort_list->p_parent;
  p_mp_list = ctrl_get_child_by_id(p_cont, IDC_MUSIC_LIST);

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  ui_list_start_roll(p_mp_list);

  return ret;
}


void music_show_play_filename(control_t *p_name)
{
  u16 *p_path = NULL;
  music_play_state_t play_status = MUSIC_STAT_INVALID;
  u16 *p_temp = NULL;

  if(cur_music_filename[0] == 0)
  {
     return;
  }

  p_path = (u16 *)mtos_malloc(MAX_FILE_PATH * sizeof(u16) );
  MT_ASSERT(p_path != NULL);
  memset(p_path, 0, MAX_FILE_PATH * sizeof(u16));
  memcpy(p_path, cur_music_filename, MAX_FILE_PATH * sizeof(u16));

  play_status = ui_music_get_play_status();

  if((play_status == MUSIC_STAT_PLAY) || (play_status == MUSIC_STAT_PAUSE))
  {
    p_temp = uni_strrchr(p_path, 0x5c/*'\\'*/);
    //if(p_temp != NULL)
    //{
		//  dvb_to_unicode(++p_temp,127, uni_str, 128);
    //}

    text_set_content_by_unistr(p_name, ++p_temp);

  }
  else
  {
    text_set_content_by_unistr(p_name, (u16*)"");
  }

  mtos_free(p_path);
}

RET_CODE open_usb_music(u32 para1, u32 para2)
{
  control_t *p_menu, *p_group;
  control_t *p_preview, *p_prev_logo;
  control_t *p_play_cont, *p_play_mode, *p_play_progress;
  control_t *p_fav_list_cont, *p_play_list_title, *p_play_list;
  control_t *p_play_curtm, *p_play_toltm, *p_play_ctrl_icon;
  control_t *p_list, *p_list_sbar;
  control_t *p_sort_list;
  control_t *p_help;
  control_t *p_play_filename;
  control_t *p_bottom_info;
  u16 i;
  u16 count=0;
  //media_fav_t *p_media = NULL;
  language_set_t lang_set        = {0};

  music_play_state_t music_play_stat = MUSIC_STAT_INVALID;
  rect_t  logo_rect =  {MUSIC_PREV_X + 8, MUSIC_PREV_Y + 8,
							  MUSIC_PREV_X + MUSIC_PREV_W - 16,
							  MUSIC_PREV_Y + MUSIC_PREV_H - 16};

  comm_help_data_t2 help = //help bar data
  {
    12, 70, {40, 100, 40, 90,40, 100,40, 100, 40, 100,40, 100},
    {
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
      HELP_RSC_STRID | IDS_PLAY_LIST,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
      HELP_RSC_STRID | IDS_SORT,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
      HELP_RSC_STRID | IDS_REPEAT,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
      HELP_RSC_STRID | IDS_PARTITION,
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_VOLUME,
      HELP_RSC_BMP   | IM_FAV,
      HELP_RSC_STRID | IDS_FAV,
     },
  };

#ifdef ENABLE_NETWORK
  comm_help_data_t2 help_network = //help bar data
  {
    8, 70, {40, 120, 40, 120,40, 120, 40, 200},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_SELECT,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
      HELP_RSC_STRID | IDS_SWITCH_TYPE,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
      HELP_RSC_STRID | IDS_REPEAT,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
      HELP_RSC_STRID | IDS_NETWORK_PLACE,
    },
  };
#endif

  /*Create Menu*/
  p_menu = ui_comm_root_create(ROOT_ID_USB_MUSIC,
    0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, RSC_INVALID_ID, IDS_MUSIC);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_menu, music_cont_proc);
  ctrl_set_keymap(p_menu, music_cont_keymap);

#ifdef ENABLE_NETWORK
  is_usb = (para1 == 1)?0:1;
  
  if(!is_usb)
  {
     memcpy(ipaddress, (char *)para2, 128);
  }
#endif

  {
    str_asc2uni((u8 *)"|mp3|MP3", g_ffilter_all);

    ui_epg_stop();
    if(g_media_enter == FALSE)
    {
      OS_PRINTF("\n#debug:ui_music_player_init! \n");
      ui_music_player_init();
    }
    else
    {
      ui_music_create_region();
    }

    //initialize global variables.
#ifdef ENABLE_NETWORK
  if(! is_usb)
  {
    ui_get_ip_path(ip_address_with_path);
    //ui_comm_dlg_open(&dlg_data); 
    //ui_evt_disable_ir();
    //_ui_music_m_free_dir_and_list();
    //p_name_filter = mtos_malloc(sizeof(u16) *(strlen(p_g_music_filter) + 1));
    //MT_ASSERT(p_name_filter != NULL);
   // memset(p_name_filter, 0 , sizeof(u16) *(strlen(p_g_music_filter) + 1));
    //str_asc2uni(p_g_music_filter, p_name_filter);
    g_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, ip_address_with_path);
    //mtos_free(p_name_filter);
    if(NULL == g_flist_dir)
    {
        //ui_comm_dlg_close();
        OS_PRINTF("[%s]: ##ERR## file list null\n", __FUNCTION__);
        count = 0;
    }
    else
    {
        file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
        //ui_comm_dlg_close();
        count = (u16)g_list.file_count;
    }
   }
   else
#endif
   {
    p_partition = NULL;
    g_partition_cnt = 0;
    g_partition_cnt = file_list_get_partition(&p_partition);

    if(g_partition_cnt > 0)
    {
      g_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT,
        p_partition[0].letter);
      if (g_flist_dir != NULL)
      {
        file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
        count = (u16)g_list.file_count;
      }
      else
      {
        count = 0;
      }
      for(i=0; i<g_partition_cnt; i++)
      {
          ui_music_load_fav_list(p_partition[i].letter[0]);
      }

    }
    else
    {
      g_is_music_fav = FALSE;
      memset(&g_list, 0, sizeof(file_list_t));
      memset(&cur_play_time, 0, sizeof(utc_time_t));
      memset(&total_play_time, 0, sizeof(utc_time_t));
    }
  }

    OS_PRINTF("total count = %d\n",count);

    //ui_music_build_play_list_indir(&g_list);
    music_play_stat = ui_music_get_play_status();
  }

  //group
  p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_MP_GROUP, MP_GROUP_X,
                             MP_GROUP_Y, MP_GROUP_W,
                             MP_GROUP_H, p_menu, 0);
  ctrl_set_rstyle(p_group, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE);
  cbox_set_font_style(p_group, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
  cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group, TRUE);

  cbox_dync_set_count(p_group, (u16)g_partition_cnt);
  cbox_dync_set_update(p_group, music_group_update);
  cbox_dync_set_focus(p_group, 0);

  //preview window
  p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_MUSIC_PREVIEW, MUSIC_PREV_X,
    MUSIC_PREV_Y, MUSIC_PREV_W, MUSIC_PREV_H, p_menu, 0);
  ctrl_set_rstyle(p_preview, RSI_MP_PREV, RSI_MP_PREV, RSI_MP_PREV);

  //preview logo
  p_prev_logo = ctrl_create_ctrl(CTRL_BMAP, IDC_MUSIC_PREVIEW_LOGO,
                            MUSIC_PREV_LOGO_X,
                            MUSIC_PREV_LOGO_Y,
                            MUSIC_PREV_LOGO_W,
                            MUSIC_PREV_LOGO_H,
                            p_preview, 0);
  bmap_set_align_type(p_prev_logo, STL_CENTER|STL_VCENTER);
  if(music_play_stat == MUSIC_STAT_INVALID ||music_play_stat == MUSIC_STAT_STOP)
  {
    bmap_set_content_by_id(p_prev_logo, IM_MP3_LOGO_MUSIC_SMALL);
  }
  //scrollbar
  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_MP_SBAR, MUSIC_LIST_SBAR_X,
    MUSIC_LIST_SBAR_Y, MP_LIST_SBARW, MP_LIST_SBARH, p_menu, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_MP_SBAR, RSI_MP_SBAR, RSI_MP_SBAR);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_MP_SBAR_MID, RSI_MP_SBAR_MID,
                     RSI_MP_SBAR_MID);
  /*ctrl_set_mrect(p_list_sbar, MP_LIST_SBAR_MIDL, MP_LIST_SBAR_MIDT,
    MP_LIST_SBAR_MIDR, MP_LIST_SBAR_MIDB);*/

  //list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_MUSIC_LIST, MUSIC_LIST_X,
    MUSIC_LIST_Y, MP_LISTW, MP_LISTH, p_menu, 0);
  ctrl_set_rstyle(p_list, RSI_MP_LCONT_LIST, RSI_MP_LCONT_LIST, RSI_MP_LCONT_LIST);
#ifdef ENABLE_NETWORK
  if(! is_usb)
  {
    ctrl_set_keymap(p_list, music_network_list_keymap);
    ctrl_set_proc(p_list, music_network_list_proc);
  }
  else
#endif
  {
    ctrl_set_keymap(p_list, music_list_keymap);
    ctrl_set_proc(p_list, music_list_proc);
  }
  ctrl_set_mrect(p_list, MP_LIST_MIDL, MP_LIST_MIDT,
    MP_LIST_MIDW + MP_LIST_MIDL, MP_LIST_MIDH + MP_LIST_MIDT);
  list_set_item_interval(p_list, MP_LCONT_LIST_VGAP);
  list_set_item_rstyle(p_list, &mp_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, count, MUSIC_LIST_ITEM_NUM_ONE_PAGE);
  list_set_field_count(p_list, MP_LIST_FIELD, MUSIC_LIST_ITEM_NUM_ONE_PAGE);
  list_set_focus_pos(p_list, 0);
  //list_select_item(p_list, 0);
  list_set_update(p_list, music_list_update, 0);
  ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

  for (i = 0; i < MP_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(mp_list_attr[i].attr), (u16)(mp_list_attr[i].width),
                        (u16)(mp_list_attr[i].left), (u8)(mp_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, mp_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, mp_list_attr[i].fstyle);
  }
  list_set_scrollbar(p_list, p_list_sbar);
  music_list_update(p_list, list_get_valid_pos(p_list), MUSIC_LIST_ITEM_NUM_ONE_PAGE, 0);

  ui_list_start_roll(p_list);

  //play container
  p_play_cont = ctrl_create_ctrl(CTRL_CONT, IDC_MUSIC_PLAY_DETAIL_CONT,
    MP_PLAY_CONT_X, MP_PLAY_CONT_Y,
    MP_PLAY_CONT_W, MP_PLAY_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_play_cont, RSI_MP_DETAIL, RSI_MP_DETAIL, RSI_MP_DETAIL);

  p_play_filename = ctrl_create_ctrl(CTRL_TEXT, IDC_MUSIC_PLAY_FILENAME,
    MP_DETAIL_FILENAME_X, MP_DETAIL_FILENAME_Y,
    MP_DETAIL_FILENAME_W, MP_DETAIL_FILENAME_H, p_play_cont, 0);
  ctrl_set_rstyle(p_play_filename, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_play_filename, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_play_filename, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_play_filename, TEXT_STRTYPE_UNICODE);
  //music_update_play_file_name(p_play_cont, FALSE);
  music_show_play_filename(p_play_filename);

  p_play_mode = ctrl_create_ctrl(CTRL_BMAP, IDC_MP_PLAY_MODE,
    MP_PLAY_MODE_X, MP_PLAY_MODE_Y,
    MP_PLAY_MODE_W, MP_PLAY_MODE_H,
    p_play_cont, 0);
  ctrl_set_rstyle(p_play_mode, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  if(g_media_enter == TRUE)
  {

/*
    IM_MP3_ICON_LISTPLAY, IM_MP3_ICON_LISTPLAY_R,
    IM_MP3_ICON_SINGLE_R, IM_MP3_ICON_NO_REPEAT,
*/
    u16 play_icon = IM_MP3_ICON_LISTPLAY;
    mlist_play_mode_t play_mode = PLAY_MODE_CYCLE_CUR_DIR_ORDER;
    play_mode = ui_music_get_play_mode();
    if((play_mode == PLAY_MODE_CYCLE_CUR_DIR_ORDER) ||
       (play_mode == PLAY_MODE_CYCLE_FAV_LIST_ORDER))
    {
       play_icon = IM_MP3_ICON_LISTPLAY;
    }
    else if((play_mode == PLAY_MODE_CYCLE_CUR_DIR_RANDOM) ||
            (play_mode == PLAY_MODE_CYCLE_FAV_LIST_RANDOM))
    {
       play_icon = IM_MP3_ICON_LISTPLAY_R;
    }
    else if((play_mode == PLAY_MODE_CYCLE_CUR_DIR_SELF) ||
            (play_mode == PLAY_MODE_CYCLE_FAV_LIST_SELF))
    {
       play_icon = IM_MP3_ICON_SINGLE_R;
    }
    else if((play_mode == PLAY_MODE_CYCLE_CUR_DIR_NONE) ||
            (play_mode == PLAY_MODE_CYCLE_FAV_LIST_NONE))
    {
       play_icon = IM_MP3_ICON_NO_REPEAT;
    }

    bmap_set_content_by_id(p_play_mode, play_icon);
  }
  else
  {
    bmap_set_content_by_id(p_play_mode, IM_MP3_ICON_LISTPLAY);
  }

  p_play_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_MUSIC_PLAY_PROGRESS,
    MP_PLAY_PROGRESS_X, MP_PLAY_PROGRESS_Y,
    MP_PLAY_PROGRESS_W, MP_PLAY_PROGRESS_H,
    p_play_cont, 0);
  ctrl_set_rstyle(p_play_progress, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_play_progress,
    MP_PLAY_PROGRESS_MIDX, MP_PLAY_PROGRESS_MIDY,
    MP_PLAY_PROGRESS_MIDW, MP_PLAY_PROGRESS_MIDH);
  pbar_set_rstyle(p_play_progress, MP_PLAY_PROGRESS_MIN, MP_PLAY_PROGRESS_MAX, MP_PLAY_PROGRESS_MID);
  pbar_set_count(p_play_progress, 0, 100, 100);
  pbar_set_direction(p_play_progress, 1);
  pbar_set_workmode(p_play_progress, 0, 0);
  pbar_set_current(p_play_progress, 0);

  p_play_curtm = ctrl_create_ctrl(CTRL_TEXT, IDC_MP_PLAY_CURTM,
    MP_PLAY_CURTM_X, MP_PLAY_CURTM_Y,
    MP_PLAY_CURTM_W, MP_PLAY_CURTM_H,
    p_play_cont, 0);
  ctrl_set_rstyle(p_play_curtm, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_play_curtm, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_play_curtm, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_play_curtm, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_play_curtm, (u8 *)"00:00");

  p_play_toltm = ctrl_create_ctrl(CTRL_TEXT, IDC_MP_PLAY_TOLTM,
    MP_PLAY_TOLTM_X, MP_PLAY_TOLTM_Y,
    MP_PLAY_TOLTM_W, MP_PLAY_TOLTM_H,
    p_play_cont, 0);
  ctrl_set_rstyle(p_play_toltm, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_play_toltm, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_play_toltm, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_play_toltm, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_play_toltm, (u8 *)"00:00");

  p_play_ctrl_icon = ctrl_create_ctrl(CTRL_MBOX, IDC_MP_PLAY_CONTROL_ICON,
    MP_PLAY_CTRL_ICON_X, MP_PLAY_CTRL_ICON_Y,
    MP_PLAY_CTRL_ICON_W, MP_PLAY_CTRL_ICON_H, p_play_cont, 0);
  ctrl_set_rstyle(p_play_ctrl_icon, RSI_MP_TITLE, RSI_MP_TITLE, RSI_MP_TITLE);
  ctrl_set_mrect(p_play_ctrl_icon, 0, 0, MP_PLAY_CTRL_ICON_W, MP_PLAY_CTRL_ICON_H);
  mbox_enable_icon_mode(p_play_ctrl_icon, TRUE);
  mbox_set_count(p_play_ctrl_icon, MP_PLAY_MBOX_TOL,
    MP_PLAY_MBOX_COL, MP_PLAY_MBOX_ROW);
  mbox_set_item_interval(p_play_ctrl_icon, MP_PLAY_MBOX_HGAP, MP_PLAY_MBOX_VGAP);
  mbox_set_item_rstyle(p_play_ctrl_icon, RSI_IGNORE,
    RSI_IGNORE, RSI_IGNORE);
  mbox_set_icon_align_type(p_play_ctrl_icon, STL_CENTER | STL_VCENTER);
  music_update_play_status_icon(p_play_ctrl_icon, FALSE);

  //bottom info bar
  p_bottom_info = ctrl_create_ctrl(CTRL_TEXT, IDC_MUSIC_BOTTOM_INFO,
                   MUSIC_BOTTOM_INFO_X, MUSIC_BOTTOM_INFO_Y, MUSIC_BOTTOM_INFO_W,
                   MUSIC_BOTTOM_INFO_H, p_menu, 0);
  ctrl_set_rstyle(p_bottom_info, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_bottom_info, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_bottom_info, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_bottom_info, TEXT_STRTYPE_UNICODE);
  if(g_flist_dir != NULL)
  {
#ifdef ENABLE_NETWORK
    if (! is_usb)
      text_set_content_by_ascstr(p_bottom_info, ipaddress);  
    else
#endif
      text_set_content_by_unistr(p_bottom_info, p_partition[0].name);
  }
  //play list container
  p_fav_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_MP_PLAY_LIST_CONT,
      MP_PLAY_LIST_CONT_X, MP_PLAY_LIST_CONT_Y,
      MP_PLAY_LIST_CONT_W, MP_PLAY_LIST_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_fav_list_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_sts(p_fav_list_cont, OBJ_STS_HIDE);

  //play list title
  p_play_list_title  = ctrl_create_ctrl(CTRL_TEXT, IDC_MUSIC_FAV_LIST_TITLE,
    MP_PLAY_LIST_TITLE_X, MP_PLAY_LIST_TITLE_Y,
    MP_PLAY_LIST_TITLE_W, MP_PLAY_LIST_TITLE_H,
    p_fav_list_cont, 0);
  ctrl_set_rstyle(p_play_list_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_play_list_title, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_play_list_title, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_play_list_title, TEXT_STRTYPE_STRID);

  //play list scrollbar
  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_MUSIC_FAV_LIST_SBAR, MP_PLAY_LIST_SBARX,
    MP_PLAY_LIST_SBARY, MP_PLAY_LIST_SBARW, MP_PLAY_LIST_SBARH, p_fav_list_cont, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_MP_SBAR, RSI_MP_SBAR, RSI_MP_SBAR);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_MP_SBAR_MID, RSI_MP_SBAR_MID,
                     RSI_MP_SBAR_MID);
 /* ctrl_set_mrect(p_list_sbar, MP_PLAY_LIST_SBAR_MIDL, MP_PLAY_LIST_SBAR_MIDT,
    MP_PLAY_LIST_SBAR_MIDR, MP_PLAY_LIST_SBAR_MIDB);*/

  //play list
  p_play_list = ctrl_create_ctrl(CTRL_LIST, IDC_MUSIC_FAV_LIST_LIST, MP_PLAY_LIST_LIST_X,
    MP_PLAY_LIST_LIST_Y, MP_PLAY_LIST_LIST_W, MP_PLAY_LIST_LIST_H, p_fav_list_cont, 0);
  ctrl_set_rstyle(p_play_list, RSI_MP_LCONT_LIST, RSI_MP_LCONT_LIST, RSI_MP_LCONT_LIST);
  ctrl_set_keymap(p_play_list, music_fav_list_keymap);
  ctrl_set_proc(p_play_list, music_fav_list_proc);
  ctrl_set_mrect(p_play_list, MP_PLAY_LIST_LIST_MIDL, MP_PLAY_LIST_LIST_MIDT,
    MP_PLAY_LIST_LIST_MIDW + MP_PLAY_LIST_LIST_MIDL, MP_PLAY_LIST_LIST_MIDH + MP_PLAY_LIST_LIST_MIDT);
  list_set_item_interval(p_play_list, MP_PLAY_LIST_LIST_VGAP);
  list_set_item_rstyle(p_play_list, &mp_list_item_rstyle);
  list_enable_select_mode(p_play_list, TRUE);
  list_set_select_mode(p_play_list, LIST_SINGLE_SELECT);
  list_set_count(p_play_list, count, MUSIC_FAV_LIST_MAX_ITEM_NUME);
  list_set_field_count(p_play_list, MP_PLAY_LIST_LIST_FIELD, MUSIC_FAV_LIST_MAX_ITEM_NUME);
  list_set_focus_pos(p_play_list, 0);
  list_set_update(p_play_list, music_fav_list_update, 0);
  list_select_item(p_play_list, 0);

  for (i = 0; i < MP_LIST_FIELD; i++)
  {
    list_set_field_attr(p_play_list, (u8)i, (u32)(mp_play_list_attr[i].attr), (u16)(mp_play_list_attr[i].width),
                        (u16)(mp_play_list_attr[i].left), (u8)(mp_play_list_attr[i].top));
    list_set_field_rect_style(p_play_list, (u8)i, mp_play_list_attr[i].rstyle);
    list_set_field_font_style(p_play_list, (u8)i, mp_play_list_attr[i].fstyle);
  }
  list_set_scrollbar(p_play_list, p_list_sbar);

  //play list help
  p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_MUSIC_FAV_LIST_HELP,
    MP_PLAY_LIST_HELP_X, MP_PLAY_LIST_HELP_Y,
    MP_PLAY_LIST_HELP_W, MP_PLAY_LIST_HELP_H, p_fav_list_cont, 0);
  ctrl_set_rstyle(p_help, RSI_MP_TITLE, RSI_MP_TITLE, RSI_MP_TITLE);
  ctrl_set_mrect(p_help, 0, 0, MP_PLAY_LIST_HELP_W, MP_PLAY_LIST_HELP_H);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, MP_PLAY_LIST_HELP_MBOX_TOL,
    MP_PLAY_LIST_HELP_MBOX_COL, MP_PLAY_LIST_HELP_MBOX_ROW);
  mbox_set_item_interval(p_help, MP_PLAY_LIST_HELP_MBOX_HGAP, MP_PLAY_LIST_HELP_MBOX_VGAP);
  mbox_set_item_rstyle(p_help, RSI_MP_TITLE,
    RSI_MP_TITLE, RSI_MP_TITLE);
  mbox_set_string_fstyle(p_help, FSI_MP_INFO_MBOX,
    FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  mbox_set_string_offset(p_help, 40, 0);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);
  play_list_update_help(p_help, FALSE);

  //sort list
  p_sort_list = ctrl_create_ctrl(CTRL_LIST, IDC_MP_SORT_LIST,
                            MUSIC_SORT_LIST_X, MUSIC_SORT_LIST_Y,
                            MUSIC_SORT_LIST_W, MUSIC_SORT_LIST_H,
                            p_menu,
                            0);
  ctrl_set_rstyle(p_sort_list, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_sort_list, music_sort_list_keymap);
  ctrl_set_proc(p_sort_list, mpm_sort_list_proc);
  ctrl_set_mrect(p_sort_list, MUSIC_SORT_LIST_MIDL, MUSIC_SORT_LIST_MIDT,
    MUSIC_SORT_LIST_MIDW + MUSIC_SORT_LIST_MIDL, MUSIC_SORT_LIST_MIDH + MUSIC_SORT_LIST_MIDT);
  list_set_item_interval(p_sort_list, MP_SORT_LIST_VGAP);
  list_set_item_rstyle(p_sort_list, &mp_list_item_rstyle);
  list_set_count(p_sort_list, MUSIC_SORT_LIST_ITEM_TOL, MUSIC_SORT_LIST_ITEM_PAGE);
  list_set_field_count(p_sort_list, MP_SORT_LIST_FIELD_NUM, MUSIC_SORT_LIST_ITEM_PAGE);
  list_set_focus_pos(p_sort_list, 0);
  list_set_update(p_sort_list, music_sort_list_update, 0);

  for (i = 0; i < MP_SORT_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_sort_list, (u8)i,
                        (u32)(mp_sort_list_field_attr[i].attr),
                        (u16)(mp_sort_list_field_attr[i].width),
                        (u16)(mp_sort_list_field_attr[i].left),
                        (u8)(mp_sort_list_field_attr[i].top));
    list_set_field_rect_style(p_sort_list, (u8)i, mp_sort_list_field_attr[i].rstyle);
    list_set_field_font_style(p_sort_list, (u8)i, mp_sort_list_field_attr[i].fstyle);
  }
  music_sort_list_update(p_sort_list, list_get_valid_pos(p_sort_list), MUSIC_SORT_LIST_ITEM_PAGE, 0);
  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);


  //help
#ifdef ENABLE_NETWORK
  ui_comm_help_create2(is_usb ? &help : &help_network, p_menu,FALSE);
#else
  ui_comm_help_create2(&help, p_menu,FALSE);
#endif
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

  if(g_media_enter == TRUE)
  {
     if((music_play_stat == MUSIC_STAT_PLAY) || (music_play_stat == MUSIC_STAT_PAUSE))
	   {
/*
		 rect_t logo_rect =  {MUSIC_PREV_X + 8, MUSIC_PREV_Y + 8,
							  MUSIC_PREV_X + MUSIC_PREV_W - 16,
							  MUSIC_PREV_Y + MUSIC_PREV_H - 16};

		 ui_music_logo_show(TRUE,logo_rect);
*/
       //ui_music_get_cur(&p_media);
       //uni_strcpy(cur_music_filename, p_media->path);

  		 if(g_list.file_count > 0)
  		 {
  			 count = (u16)g_list.file_count;
  			 for(i = 0; i < count; i++)
  			 {
  				 if(uni_strcmp(cur_music_filename, g_list.p_file[i].name) == 0)
  				 {
  					 list_set_focus_pos(p_list, i);
  					 list_select_item(p_list, i);
  				 }

  			 }
  		 }

	   }
  }

  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  if(ui_is_mute())
    open_mute(0, 0);

  ui_music_logo_show(TRUE,logo_rect);
  sys_status_get_lang_set(&lang_set);

  ui_music_set_charset((str_fmt_t) (lang_set.text_encode_music));
  
  ui_list_start_roll(p_list);
  ui_time_enable_heart_beat(TRUE);

  return SUCCESS;
}

#ifdef ENABLE_NETWORK
static RET_CODE _ui_music_change(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    //ui_evt_disable_ir();
    //on_music_change(ctrl_get_parent(ctrl_get_parent (p_list)), MSG_EXIT, para1, para2);
    if(is_usb)
    {
      return SUCCESS;
    }
    else
    {
      g_media_enter = FALSE;

      fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR);
      music_player_stop();
      if(g_flist_dir != NULL)
      {
        file_list_leave_dir(g_flist_dir);
        g_flist_dir = NULL;
      }
      ui_enable_chk_pwd(TRUE);
      //ui_music_unload_fav_list();
      ui_music_player_release();
      ui_music_set_play_status(MUSIC_STAT_INVALID);
      memset(cur_music_filename, 0, MAX_FILE_PATH * sizeof(u16));

#ifndef WIN32
      ui_music_release_region();
#endif
      manage_open_menu(ROOT_ID_USB_PICTURE, 1, (u32)( ipaddress));
    }
    
    manage_close_menu(ROOT_ID_USB_MUSIC, 0, 0);
    
    return SUCCESS;
}
#endif

BEGIN_KEYMAP(music_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_REC,MSG_MUSIC_REC_NONE)
  ON_EVENT(V_KEY_BLUE, MSG_BLUE)
  ON_EVENT(V_KEY_REPEAT, MSG_BLUE)
END_KEYMAP(music_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(music_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_music_exit)
  //ON_COMMAND(MSG_EXIT, on_music_exit_all)
  //ON_COMMAND(MSG_EXIT_ALL, close_usb_music_menu)
  ON_COMMAND(MSG_PLAYER_EVT_GET_PLAY_TIME, on_music_update_play_time)
  ON_COMMAND(MSG_ONE_SECOND_UPDATE_PROCESS_BAR, on_music_update_process_bar)
  ON_COMMAND(MSG_MUSIC_EVT_PLAY_END, on_music_evt_play_end)
  ON_COMMAND(MSG_MUSIC_EVT_CANNOT_PLAY, on_music_evt_cannot_play)
  ON_COMMAND(MSG_PLUG_OUT, on_usb_music_plug_out)
  ON_COMMAND(MSG_MUSIC_REC_NONE,on_usb_rec_none)
  ON_COMMAND(MSG_BLUE, on_music_list_change_play_mode)
  ON_COMMAND(MSG_DESTROY, on_music_destory)
END_MSGPROC(music_cont_proc, ui_comm_root_proc)


BEGIN_KEYMAP(music_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  //ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  //ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  //ON_EVENT(V_KEY_SAT, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_FAV, MSG_ADD_FAV)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_PLAY, MSG_MUSIC_RESUME)
  ON_EVENT(V_KEY_PAUSE, MSG_MUSIC_PAUSE)
  ON_EVENT(V_KEY_NEXT, MSG_MUSIC_NEXT)
  ON_EVENT(V_KEY_PREV, MSG_MUSIC_PRE)
  ON_EVENT(V_KEY_STOP, MSG_MUSIC_STOP)

#if 0
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
#endif
END_KEYMAP(music_list_keymap, NULL)

BEGIN_MSGPROC(music_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_music_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_music_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_music_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_music_list_change_focus)
  //ON_COMMAND(MSG_DECREASE, on_music_list_change_partition)
  //ON_COMMAND(MSG_INCREASE, on_music_list_change_partition)
  ON_COMMAND(MSG_GREEN, on_music_list_change_partition)
  ON_COMMAND(MSG_VOLUME_DOWN, on_music_change_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_music_change_volume)
  ON_COMMAND(MSG_YELLOW, on_music_list_open_fav_list)
  ON_COMMAND(MSG_RED, on_music_list_sort)
  ON_COMMAND(MSG_ADD_FAV, on_music_add_to_fav_list)
  ON_COMMAND(MSG_SELECT, on_music_list_select)
  ON_COMMAND(MSG_MUSIC_PLAY, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_RESUME, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_PAUSE, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_NEXT, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_PRE, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_STOP, on_music_list_change_play_status)
  ON_COMMAND(MSG_ONE_SECOND_PLAY_STATUS, on_one_second_change_play_status)
#if 0
  ON_COMMAND(MSG_EXIT, on_music_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_music_exit_all)
#endif
END_MSGPROC(music_list_proc, list_class_proc)

#ifdef ENABLE_NETWORK
BEGIN_KEYMAP(music_network_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_YELLOW, MSG_SWITCH_MEDIA_TYPE)
  //ON_EVENT(V_KEY_RECALL, MSG_MUSIC_UPFOLDER)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_PLAY, MSG_MUSIC_RESUME)
  ON_EVENT(V_KEY_PAUSE, MSG_MUSIC_PAUSE)
  ON_EVENT(V_KEY_NEXT, MSG_MUSIC_NEXT)
  ON_EVENT(V_KEY_PREV, MSG_MUSIC_PRE)
  ON_EVENT(V_KEY_STOP, MSG_MUSIC_STOP)
  ON_EVENT(V_KEY_RECALL, MSG_EXIT)
  ON_EVENT(V_KEY_GREEN, MSG_MUSIC_BACK_TO_NETWORK_PLACE)
END_KEYMAP(music_network_list_keymap, NULL)

BEGIN_MSGPROC(music_network_list_proc, list_class_proc)
  //ON_COMMAND(MSG_EXIT, on_music_process_msg)
  ON_COMMAND(MSG_SELECT, on_music_list_select)
  ON_COMMAND(MSG_SWITCH_MEDIA_TYPE, _ui_music_change)
  //ON_COMMAND(MSG_MUSIC_UPFOLDER, ui_music_up_folder)
  ON_COMMAND(MSG_FOCUS_UP, on_music_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_music_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_music_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_music_list_change_focus)
  ON_COMMAND(MSG_VOLUME_DOWN, on_music_change_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_music_change_volume)
  ON_COMMAND(MSG_MUSIC_PLAY, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_RESUME, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_PAUSE, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_NEXT, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_PRE, on_music_list_change_play_status)
  ON_COMMAND(MSG_MUSIC_STOP, on_music_list_change_play_status)
  ON_COMMAND(MSG_ONE_SECOND_PLAY_STATUS, on_one_second_change_play_status)
  ON_COMMAND(MSG_MUSIC_BACK_TO_NETWORK_PLACE, on_music_process_msg)
END_MSGPROC(music_network_list_proc, list_class_proc)
#endif


BEGIN_KEYMAP(music_fav_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_BLUE, MSG_BLUE)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
  ON_EVENT(V_KEY_RED, MSG_SELECT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_PLAY, MSG_MUSIC_RESUME)
  ON_EVENT(V_KEY_PAUSE, MSG_MUSIC_PAUSE)
  ON_EVENT(V_KEY_SLOW, MSG_MUSIC_NEXT)
  ON_EVENT(V_KEY_REVSLOW, MSG_MUSIC_PRE)
  ON_EVENT(V_KEY_STOP, MSG_MUSIC_STOP)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(music_fav_list_keymap, NULL)

BEGIN_MSGPROC(music_fav_list_proc, list_class_proc)
  ON_COMMAND(MSG_BLUE, on_music_fav_list_change_play_mode)
  ON_COMMAND(MSG_YELLOW, on_music_fav_list_del)
  ON_COMMAND(MSG_SELECT, on_music_fav_list_select)
  ON_COMMAND(MSG_GREEN, on_music_fav_list_del_all)
  ON_COMMAND(MSG_EXIT, on_music_fav_list_exit)
END_MSGPROC(music_fav_list_proc, list_class_proc)


BEGIN_KEYMAP(music_sort_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(music_sort_list_keymap, NULL)

BEGIN_MSGPROC(mpm_sort_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_sort_list_select)
  ON_COMMAND(MSG_EXIT, on_music_sort_list_exit)
END_MSGPROC(mpm_sort_list_proc, list_class_proc)

#endif
