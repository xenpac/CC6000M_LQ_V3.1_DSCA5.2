/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
   
#include "ui_nvod_video.h"
#include "ui_nvod_api.h"

enum nvod_video_local_msg
{
  MSG_ONE_SECOND = MSG_LOCAL_BEGIN + 750,
};

enum nvod_video_ctrl_id
{
  IDC_INVALID = 0,
  IDC_NVOD_CONT,
  IDC_NVOD_TIME,
  IDC_NVOD_PLAY_INFO_CONT,
  IDC_NVOD_STR,
  
  IDC_NVOD_NAME,
  IDC_NVOD_START,
  IDC_NVOD_END,
  IDC_NVOD_PLAY_PROGRESS,

  IDC_NVOD_NAME_CONTENT,
  IDC_NVOD_STIME_CONTENT,
  IDC_NVOD_ETIME_CONTENT,
  IDC_NVOD_PLAY_PROGRESS_CONTENT,
};

static BOOL g_is_nvod_play_book = FALSE;
static utc_time_t left_time = {0};
static u32 g_past_second = 0;
static book_pg_t nvod_book_node;

u16 nvod_full_play_cont_keymap(u16 key);
RET_CODE nvod_full_play_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 nvod_full_play_info_cont_keymap(u16 key);
RET_CODE nvod_full_play_info_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE nvod_video_refresh_info(control_t *p_cont, BOOL is_paint)
{
  control_t *p_play_info_cont, *p_ctrl;
  nvod_reference_svc_evt_t *p_ref_svc_evt = NULL;
  time_shifted_svc_evt_t *p_ts_svc_evt = NULL;
  u8 asc_str[32];
  u32 temp_data = 0;
  utc_time_t temp_time;

  if(!g_is_nvod_play_book)
  {
    p_ref_svc_evt = ui_get_playing_nvod_reference_svc_evt();
    p_ts_svc_evt = ui_get_playing_nvod_time_shifted_svc_evt();

    p_play_info_cont = ctrl_get_child_by_id(p_cont->p_parent, IDC_NVOD_PLAY_INFO_CONT);

    if(p_ref_svc_evt != NULL && p_ts_svc_evt != NULL)
    {
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_NAME_CONTENT);
      //text_set_content_by_ascstr(p_ctrl, (u8*)p_ref_svc_evt->event_name);
      text_set_content_by_unistr( p_ctrl, p_ref_svc_evt->event_name);
        
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_STIME_CONTENT);
      memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
      time_to_local(&(p_ts_svc_evt->start_time), &temp_time);
      sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
      text_set_content_by_ascstr(p_ctrl, asc_str);
      
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_ETIME_CONTENT);
      time_to_local(&(p_ts_svc_evt->start_time), &temp_time);
      time_add(&temp_time, &p_ts_svc_evt->drt_time);
      memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
      sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
      text_set_content_by_ascstr(p_ctrl, asc_str);
      
      p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_PLAY_PROGRESS_CONTENT);
      memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
      if(time_conver(&p_ts_svc_evt->drt_time)>0)
      {
        temp_data = g_past_second*100/time_conver(&p_ts_svc_evt->drt_time);
        sprintf((char *)asc_str, "%ld%%", temp_data);
      }
      text_set_content_by_ascstr(p_ctrl, asc_str);
    }
  }
  else
  {
    p_play_info_cont = ctrl_get_child_by_id(p_cont->p_parent, IDC_NVOD_PLAY_INFO_CONT);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_NAME_CONTENT);
    text_set_content_by_ascstr(p_ctrl, (u8*)nvod_book_node.event_name);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_STIME_CONTENT);
    memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
    time_to_local(&(nvod_book_node.start_time), &temp_time);
    sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
    text_set_content_by_ascstr(p_ctrl, asc_str);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_ETIME_CONTENT);
    time_to_local(&(nvod_book_node.start_time), &temp_time);
    time_add(&temp_time, &nvod_book_node.drt_time);
    memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
    sprintf((char *)asc_str, "%02d:%02d:%02d", temp_time.hour, temp_time.minute, temp_time.second);
    text_set_content_by_ascstr(p_ctrl, asc_str);

    p_ctrl = ctrl_get_child_by_id(p_play_info_cont, IDC_NVOD_PLAY_PROGRESS_CONTENT);
    memset(asc_str, 0, sizeof(asc_str)/sizeof(u8));
    if(time_conver(&nvod_book_node.drt_time)>0)
    {
      temp_data = g_past_second*100/time_conver(&nvod_book_node.drt_time);
      sprintf((char *)asc_str, "%ld%%", temp_data);
    }
    text_set_content_by_ascstr(p_ctrl, asc_str);
  }
  
  if(is_paint)
  {
    ctrl_paint_ctrl(p_play_info_cont, TRUE);
  }
  
  return SUCCESS;
}

static void nvod_video_calc_left_time()
{
  time_shifted_svc_evt_t *p_ts_svc_evt = NULL;
  u32 total_left_second = 0;
  utc_time_t end_time = {0};
  utc_time_t cur_time = {0};

  p_ts_svc_evt = ui_get_playing_nvod_time_shifted_svc_evt();

  time_get(&cur_time,TRUE);
   
  g_past_second = time_dec(&cur_time, &p_ts_svc_evt->start_time);

  memset(&left_time, 0, sizeof(utc_time_t));
  memcpy(&end_time, &p_ts_svc_evt->start_time, sizeof(utc_time_t));
  time_add(&end_time, &p_ts_svc_evt->drt_time);
  total_left_second = time_dec(&end_time, &cur_time);
  left_time.hour = (u8)(total_left_second / 3600);
  left_time.minute = (u8)((total_left_second % 3600)/60);
  left_time.second = (u8)((total_left_second % 60));
}

RET_CODE open_nvod_video(u32 para1, u32 para2)
{
  control_t *p_cont,*p_info_cont,*p_mask,*p_time, *p_title,*p_title1;
  u16 i;
  u16 strid[] = 
  {
    IDS_PROGRAM_NAME_NVOD, IDS_START_TIME_NVOD, IDS_END_TIME_NVOD, IDS_NVOD_PLAY_PROGRESS
  };
  
  g_is_nvod_play_book = (BOOL)para1;

  if(!g_is_nvod_play_book)
  {
    nvod_video_calc_left_time();
  }
  else
  {
    memcpy((void *)&nvod_book_node, (void *)para2, sizeof(book_pg_t));
    g_past_second = 0;
    memset(&left_time, 0, sizeof(utc_time_t));
    memcpy(&left_time, &nvod_book_node.drt_time, sizeof(utc_time_t));
  }
  
  p_mask = ui_comm_root_create(ROOT_ID_NVOD_VIDEO,
                             ROOT_ID_NVOD,
                             0, 0,
                             COMM_BG_W, COMM_BG_H,
                             0, RSI_IGNORE);  

  ctrl_set_rstyle(p_mask, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NVOD_CONT, NVOD_VIDEO_CONT_X,
                            NVOD_VIDEO_CONT_Y, NVOD_VIDEO_CONT_W, NVOD_VIDEO_CONT_H, p_mask, 0);

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, nvod_full_play_cont_keymap);
  ctrl_set_proc(p_cont, nvod_full_play_cont_proc);
  
  //time
  p_time = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_NVOD_TIME,
                              NVOD_VIDEO_TIME_X, NVOD_VIDEO_TIME_Y,
                              NVOD_VIDEO_TIME_W, NVOD_VIDEO_TIME_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_time, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  text_set_align_type(p_time , STL_VCENTER|STL_VCENTER);
  text_set_font_style(p_time , FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_time , TEXT_STRTYPE_UNICODE);
  //text_set_content_by_strid(p_time, IDS_TIME_SET);

  //
  p_info_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NVOD_PLAY_INFO_CONT,
                             NVOD_VIDEO_INFO_CONT_X, NVOD_VIDEO_INFO_CONT_Y,
                             NVOD_VIDEO_INFO_CONT_W, NVOD_VIDEO_INFO_CONT_H,
                             p_mask, 0);
  ctrl_set_rstyle(p_info_cont, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_info_cont, nvod_full_play_info_cont_keymap);
  ctrl_set_proc(p_info_cont, nvod_full_play_info_cont_proc);
  ctrl_set_sts(p_info_cont, OBJ_STS_HIDE);
 
  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_NVOD_STR,
                             NVOD_TITLE_STR_X, NVOD_TITLE_STR_Y,
                             NVOD_TITLE_STR_W, NVOD_TITLE_STR_H,
                             p_info_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_NVOD_NVOD);
  
  for(i=0; i<4; i++)
  {
    p_title = ctrl_create_ctrl(CTRL_TEXT, (IDC_NVOD_NAME+i),
                               NVOD_TITLE_NAME_X, (u16)(NVOD_TITLE_NAME_Y+i*NVOD_TITLE_NAME_H+2),
                               NVOD_TITLE_NAME_W, NVOD_TITLE_NAME_H,
                               p_info_cont, 0);
    ctrl_set_rstyle(p_title, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_title, STL_RIGHT | STL_VCENTER);
    text_set_content_type(p_title, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_title, strid[i]);

    p_title1 = ctrl_create_ctrl(CTRL_TEXT, (IDC_NVOD_NAME_CONTENT+i),
                               NVOD_TITLE_SCREEN_X, (u16)(NVOD_TITLE_SCREEN_Y+i*NVOD_TITLE_SCREEN_H+2),
                               NVOD_TITLE_SCREEN_W, NVOD_TITLE_SCREEN_H,
                               p_info_cont, 0);
    ctrl_set_rstyle(p_title1, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_title1, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_title1, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_title1, TEXT_STRTYPE_UNICODE);
  }

  nvod_video_refresh_info(p_cont, FALSE);
  fw_tmr_create(ROOT_ID_NVOD_VIDEO, MSG_ONE_SECOND, 1000, TRUE);  
  
  ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_mask,FALSE);
  
  return SUCCESS;
}

static void do_cancel_video(void)
{
  //ui_stop_play_nvod(STOP_PLAY_BLACK, TRUE);
  fw_tmr_destroy(ROOT_ID_NVOD_VIDEO, MSG_ONE_SECOND);
 //fw_notify_parent(ROOT_ID_NVOD_VIDEO, NOTIFY_T_MSG, FALSE, MSG_NVOD_FULL_PLAY_EXIT, 0, 0);
  manage_close_menu(ROOT_ID_NVOD_VIDEO, 0, 0);
}

static void undo_cancel_video(void)
{
 
}

extern void ui_set_shift_event_list_focus(u16 idx);

static RET_CODE on_close_video_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 shift_event_index = ui_nvod_get_shift_event_index();
  ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_NVOD, 
                               do_cancel_video, undo_cancel_video, 0);
  if(fw_find_root_by_id(ROOT_ID_NVOD) != NULL) 
  {
    ui_set_shift_event_list_focus(shift_event_index);
  }
  
  return SUCCESS;
}

static RET_CODE on_display_video_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_child;

  nvod_video_refresh_info(p_ctrl, TRUE);
  p_child = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_NVOD_PLAY_INFO_CONT);
   ctrl_set_sts(p_child, OBJ_STS_SHOW);
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_child, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(p_ctrl->p_parent,TRUE);
  return SUCCESS;
}

static RET_CODE on_nvod_video_updown(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL bRet = FALSE;
  
  switch(msg)
  {
    case MSG_FOCUS_UP:
      bRet = nvod_play_offset(-1);
      break;

    case MSG_FOCUS_DOWN:
      bRet = nvod_play_offset(1);
      break;
    default:
      break;
  }

  if(bRet)
  {
    nvod_video_calc_left_time();
  }
  
  return SUCCESS;
}

static RET_CODE on_nvod_video_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  switch(msg)
  {
    case MSG_FOCUS_LEFT:
      manage_open_menu(ROOT_ID_NVOD_VOLUME, V_KEY_LEFT, para2);
      break;

    case MSG_FOCUS_RIGHT:
      manage_open_menu(ROOT_ID_NVOD_VOLUME, V_KEY_RIGHT, para2);
      break;
    default:
      break;
  }
  
  return SUCCESS;
}

static void nvod_time_down(utc_time_t *p_time, u32 sec)
{
  u32 all_sec = time_conver(p_time);
  //add second
  if(sec > 0)
  {
    if(all_sec>sec)
    {
      all_sec -= sec;
      p_time->hour = (u8)(all_sec / 3600);
      p_time->minute = (u8)((all_sec % 3600)/60);
      p_time->second = (u8)((all_sec % 60));
    }
    else
    {
      memset(p_time, 0, sizeof(utc_time_t));
      fw_tmr_destroy(ROOT_ID_NVOD_VIDEO, MSG_ONE_SECOND);
    }
  }
}

static RET_CODE on_display_nvod_time(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  
  control_t *p_child;
  u8 ascstr[16];
  memset(ascstr, 0, sizeof(ascstr)/sizeof(u8));
  
  p_child = ctrl_get_child_by_id(p_ctrl, IDC_NVOD_TIME);
  //ctrl_set_attr(p_child,OBJ_ATTR_ACTIVE);
  //ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  //ctrl_process_msg(p_child, MSG_GETFOCUS, 0, 0);

  g_past_second++;
  nvod_time_down(&left_time, 1);
  sprintf((char*)ascstr, "%02d:%02d:%02d", left_time.hour, left_time.minute, left_time.second);
  text_set_content_by_ascstr(p_child, (u8*)ascstr);
  
  ctrl_paint_ctrl(p_child,TRUE);
  return SUCCESS;
}

static RET_CODE on_display_nvod_time_when_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  
  control_t *p_mask, *p_time_cont;
  p_mask = ctrl_get_parent(p_ctrl);
  p_time_cont = ctrl_get_child_by_id(p_mask,IDC_NVOD_CONT);

  return on_display_nvod_time(p_time_cont, msg, para1, para2);
}


static RET_CODE on_hid_video_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont;
  p_cont = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_NVOD_CONT);
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  ctrl_paint_ctrl(p_cont,TRUE);
  return SUCCESS;
}

BEGIN_KEYMAP(nvod_full_play_cont_keymap, NULL) 
  ON_EVENT(V_KEY_MENU, MSG_CLOSE_MENU)
  ON_EVENT(V_KEY_CANCEL, MSG_CLOSE_MENU)
  ON_EVENT(V_KEY_EXIT, MSG_CLOSE_MENU)
  ON_EVENT(V_KEY_BACK, MSG_BLACK_SCREEN)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT) 
  ON_EVENT(V_KEY_INFO, MSG_SELECT) 
END_KEYMAP(nvod_full_play_cont_keymap, NULL)
  
BEGIN_MSGPROC(nvod_full_play_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CLOSE_MENU, on_close_video_menu)
  ON_COMMAND(MSG_EXIT, on_close_video_menu)
  ON_COMMAND(MSG_BLACK_SCREEN, on_close_video_menu)
  ON_COMMAND(MSG_SELECT, on_display_video_menu)
  ON_COMMAND(MSG_ONE_SECOND, on_display_nvod_time)
  ON_COMMAND(MSG_FOCUS_UP, on_nvod_video_updown)
  ON_COMMAND(MSG_FOCUS_DOWN, on_nvod_video_updown)
  ON_COMMAND(MSG_FOCUS_LEFT, on_nvod_video_volume)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_nvod_video_volume)
END_MSGPROC(nvod_full_play_cont_proc, cont_class_proc)

BEGIN_KEYMAP(nvod_full_play_info_cont_keymap, NULL) 
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
  ON_EVENT(V_KEY_OK, MSG_CANCEL) 
  ON_EVENT(V_KEY_INFO, MSG_CANCEL) 
END_KEYMAP(nvod_full_play_info_cont_keymap, NULL)

BEGIN_MSGPROC(nvod_full_play_info_cont_proc, cont_class_proc)
   ON_COMMAND(MSG_CANCEL, on_hid_video_menu)
   ON_COMMAND(MSG_ONE_SECOND, on_display_nvod_time_when_info)
END_MSGPROC(nvod_full_play_info_cont_proc, cont_class_proc)


