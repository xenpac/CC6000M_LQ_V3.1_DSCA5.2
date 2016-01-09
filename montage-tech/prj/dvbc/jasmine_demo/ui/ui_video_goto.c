/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_video_v_full_screen.h"
#include "ui_video_goto.h"
#include "ui_video.h"

enum control_id
{
  IDC_VIDEO_GOTO_TEXT = 1,
  IDC_VIDEO_GOTO_TIMEBOX,
};

static u8 separator[TBOX_MAX_ITEM_NUM] = {'-', '-', ' ', ':', ':', ' '};
//static u32 g_play_sec = 0;

u16 video_goto_tbox_keymap(u16 key);
RET_CODE video_goto_tbox_proc(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2);

/*!
 * Create video goto control
 */
static control_t * _ui_video_goto_creat_cont(void)
{
  control_t *p_cont = NULL;
  
  p_cont = fw_create_mainwin(ROOT_ID_VIDEO_GOTO, VIDEO_GOTO_CONT_X,
                                VIDEO_GOTO_CONT_Y, VIDEO_GOTO_CONT_W, VIDEO_GOTO_CONT_H,
                                ROOT_ID_FILEPLAY_BAR,
                                0, OBJ_ATTR_ACTIVE,
                                STL_EX_WHOLE_HL);

  ctrl_set_rstyle(p_cont, RSI_COMM_CONT_SH,
                          RSI_COMM_CONT_HL,
                          RSI_COMM_CONT_GRAY);
  return p_cont;
}

/*!
 * Create video goto text
 */
static control_t * _ui_video_goto_creat_text(control_t *p_cont)
{
  control_t *p_txt = NULL;
  
  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_VIDEO_GOTO_TEXT,
                             0, 0, VIDEO_TIME_ITEM_LW, VIDEO_GOTO_CONT_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_font_style(p_txt, FSI_WHITE, FSI_WHITE, FSI_GRAY);
  text_set_align_type(p_txt, STL_LEFT | STL_VCENTER);
  text_set_offset(p_txt, COMM_CTRL_OX, 0);
  text_set_content_by_strid(p_txt, IDS_GO_TO);
  
  return p_txt;
}

/*!
 * Create video goto timebox
 */
static control_t * _ui_video_goto_creat_tbox(control_t *p_cont)
{
  control_t *p_timebox = NULL;
  u8 i = 0;
  
  p_timebox = ctrl_create_ctrl(CTRL_TBOX, IDC_VIDEO_GOTO_TIMEBOX,
                            VIDEO_TIME_ITEM_LW , 0, VIDEO_TIME_ITEM_RW, VIDEO_GOTO_CONT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_timebox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_timebox, video_goto_tbox_keymap);
  ctrl_set_proc(p_timebox, video_goto_tbox_proc);
  
  tbox_set_align_type(p_timebox, STL_CENTER | STL_VCENTER);
  tbox_set_offset(p_timebox, 0, 0);
  tbox_set_font_style(p_timebox,
                 FSI_COMM_CTRL_SH,
                 FSI_COMM_CTRL_HL,
                 FSI_COMM_CTRL_GRAY);
  
  tbox_set_time_type(p_timebox, TBOX_HOUR | TBOX_MIN | TBOX_SECOND);
  tbox_set_focus(p_timebox, 0, TBOX_ITEM_HOUR);
  tbox_set_separator_type(p_timebox, TBOX_SEPARATOR_TYPE_UNICODE);
  tbox_set_max_num_width(p_timebox, 18);
 
  for(i = 0; i < TBOX_MAX_ITEM_NUM; i++)
  {
    tbox_set_separator_by_ascchar(p_timebox, i, separator[i]);
  }

  return p_timebox;
}

RET_CODE open_video_goto(u32 para1, u32 para2)
{
  control_t *p_cont    = NULL;
  control_t *p_timebox = NULL;

  //g_play_sec = para1;
  
  /*Create Menu*/
  p_cont = _ui_video_goto_creat_cont();
  MT_ASSERT(p_cont != NULL);

  /*Create text*/
  _ui_video_goto_creat_text(p_cont);

  /*Create timebox*/
  p_timebox = _ui_video_goto_creat_tbox(p_cont);

  
  ctrl_default_proc(p_timebox, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);
  tbox_enter_edit(p_timebox);

  return SUCCESS;
}

static u32 _ui_video_goto_switch_time(video_play_time_t *p_time)
{
  u32 sec = 0;
  
  sec = ((u32)p_time->hour * 3600) + ((u32)p_time->min * 60) + ((u32)p_time->sec);
  return sec;
}

static RET_CODE _video_goto_update(utc_time_t *p_jump_time)
{
  u32 jump_sec = 0;
  u32 total_sec = 0;
  video_play_time_t p_total_time = {0};
  u32 play_sec = 0;

  ui_video_c_get_total_time(&p_total_time);

  total_sec = _ui_video_goto_switch_time(&p_total_time);

  OS_PRINTF("\n\n\ntotal_sec = %d", total_sec);
  
  jump_sec = time_conver(p_jump_time);

  OS_PRINTF("\n\n\njump_sec = %d", jump_sec);

  if(is_time_zero(p_jump_time))
  {
    //ui_video_c_pause_resume();
  }
  else
  {
    if(jump_sec <= total_sec)
    {
      //ui_video_c_pause_resume();
      play_sec = ui_video_c_get_play_time();
      ui_video_c_seek(jump_sec);
      OS_PRINTF("\n\n\ng_play_sec = %d", play_sec);
    }
  }
  return SUCCESS;
}

BOOL is_invalid_time(utc_time_t *p_time)
{
  BOOL ret = FALSE;
  
  if(p_time->minute > 59)
  {
    p_time->minute = 59;
    ret = TRUE;
  }
  if(p_time->second > 59)
  {
    p_time->second = 59;
    ret = TRUE;
  }
  return ret;
}

static RET_CODE on_video_goto_select(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  utc_time_t utc_time = {0};

  if(tbox_is_on_edit(p_ctrl))
  {
    tbox_get_time(p_ctrl, &utc_time);

    if(is_invalid_time(&utc_time))
    {
      tbox_set_time(p_ctrl, &utc_time);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      return SUCCESS;
    }          
  }
  
  _video_goto_update(&utc_time);
 
  manage_close_menu(ROOT_ID_VIDEO_GOTO, 0, 0);
  return SUCCESS;
}

static RET_CODE on_exit_video_goto(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  //ui_video_c_pause_resume();
  if(NULL != fw_find_root_by_id(ROOT_ID_FILEPLAY_BAR))
  {
    fw_notify_parent(ROOT_ID_VIDEO_GOTO, NOTIFY_T_MSG, FALSE,
                   MSG_UP_FILEBAR, 0, 0);
  }
  manage_close_menu(ROOT_ID_VIDEO_GOTO, 0, 0);
  return SUCCESS; 
}


BEGIN_KEYMAP(video_goto_tbox_keymap, ui_comm_tbox_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_GOTO, MSG_EXIT)
END_KEYMAP(video_goto_tbox_keymap, ui_comm_tbox_keymap)

BEGIN_MSGPROC(video_goto_tbox_proc, ui_comm_time_proc)
  ON_COMMAND(MSG_SELECT, on_video_goto_select)
  ON_COMMAND(MSG_EXIT, on_exit_video_goto)
END_MSGPROC(video_goto_tbox_proc, ui_comm_time_proc)

