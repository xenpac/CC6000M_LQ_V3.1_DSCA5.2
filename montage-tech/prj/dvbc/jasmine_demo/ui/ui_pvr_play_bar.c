/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifdef ENABLE_PVR_REC_CONFIG
#include "ui_common.h"

#include "ui_pvr_play_bar.h"
#include "ui_pause.h"
#include "ui_mute.h"
#include "ui_jump.h"
#include "file_list.h"
#include "vfs.h"
#include "ui_volume_usb.h"
#include "ui_audio_set.h"

#ifdef ENABLE_CA
#include "cas_manager.h"
#ifdef ONLY1_CA_VER
#include "only1.h"
#endif
#endif
//#define V_KEY_GOTO  V_KEY_BLUE

enum pvr_play_bar_local_msg
{
  MSG_INFO = MSG_LOCAL_BEGIN + 750,
  MSG_OK,
//  MSG_EXIT_TO_MENU,
  MSG_ONE_SECOND_ARRIVE,
  MSG_TS_PLAY_PAUSE,
//  MSG_TS_PLAY_PLAY,
  MSG_TS_PLAY_STOP,
  MSG_TS_PLAY_FB,
  MSG_TS_PLAY_FF,
  MSG_TS_PLAY_REVSLOW,
  MSG_TS_PLAY_SLOW,
  MSG_TS_PLAY_PREV,
  MSG_TS_PLAY_NEXT,
  MSG_TS_PLAY_SEEK_F,
  MSG_TS_PLAY_SEEK_B,
  MSG_TS_PLAY_DVR_INFO,
  MSG_TS_PLAY_SEEK_TIME,
  MSG_TS_PLAY_VOL_DOWN,
  MSG_TS_PLAY_VOL_UP,
  MSG_TS_PLAY_AUDIO_SET,
  MSG_TS_PLAY_DO_NOTHING
};

enum control_id
{
  IDC_PVR_PLAY_BAR_LIST_CONT = 1,
  IDC_PVR_PLAY_DUR_FRAME = 2,
  IDC_PVR_PLAY_BAR_FRAME = 3
};

enum list_cont_sub_control_id
{
  IDC_PVR_PLAY_BAR_TITLE = 4,
  IDC_PVR_PLAY_BAR_LIST = 5,
  IDC_PVR_PLAY_BAR_LIST_SCROLL = 6,
};


enum frame_sub_control_id
{
  IDC_PVR_PLAY_ICON = 6,
  IDC_PVR_PLAY_PAUSE_STAT_ICON,
  IDC_PVR_PLAY_PLAY_STAT_ICON,
  IDC_PVR_PLAY_FB_STAT_ICON,
  IDC_PVR_PLAY_FF_STAT_ICON,
  IDC_PVR_PLAY_REVSLOW_STAT_ICON,
  IDC_PVR_PLAY_SLOW_STAT_ICON,
  IDC_PVR_PLAY_PLAY_SPEED,
  IDC_PVR_PLAY_NAME,
  IDC_PVR_PLAY_DATE,
  IDC_PVR_PLAY_TIME,
  IDC_PVR_PLAY_USB_CAPACITY,
  IDC_PVR_PLAY_PLAY_CUR_TIME,
  IDC_PVR_PLAY_PLAY_TOTAL_TIME,
  IDC_PVR_PLAY_PLAY_PROGRESS,
  IDC_PVR_PLAY_SEEK,
};

typedef enum
{
  PVR_PLAY_NORMAL = 0,
  PVR_PLAY_PAUSE,
  PVR_PLAY_FAST,
  PVR_PLAY_SLOW,
  PVR_PLAY_SEEK
}pvr_play_mode_t;

typedef enum
{
  PVR_PLAY_FB_24X = 0,
  PVR_PLAY_FB_16X,
  PVR_PLAY_FB_8X,
  PVR_PLAY_FB_4X,
  PVR_PLAY_FB_2X,
  PVR_PLAY_FAST_NORMAL,
  PVR_PLAY_FF_2X,
  PVR_PLAY_FF_4X,
  PVR_PLAY_FF_8X,
  PVR_PLAY_FF_16X,
  PVR_PLAY_FF_24X,
  PVR_PLAY_FAST_NUMBER
}pvr_play_fast_mode_t;

typedef enum
{
  PVR_PLAY_RB_8X = 0,
  PVR_PLAY_RB_4X,
  PVR_PLAY_RB_2X,
  PVR_PLAY_SLOW_NORMAL,
  PVR_PLAY_RF_2X,
  PVR_PLAY_RF_4X,
  PVR_PLAY_RF_8X,
  PVR_PLAY_SLOW_NUMBER
}pvr_play_slow_mode_t;

#define SEEK_STEP_PERCENT 10

static const pvr_trick_mode_t g_pvr_fast_mode[PVR_PLAY_FAST_NUMBER] =
{
  {MUL_PVR_PLAY_SPEED_32X_FAST_BACKWARD, 32},//only support 32x,not 24x
  {MUL_PVR_PLAY_SPEED_16X_FAST_BACKWARD, 16},
  {MUL_PVR_PLAY_SPEED_8X_FAST_BACKWARD, 8},
  {MUL_PVR_PLAY_SPEED_4X_FAST_BACKWARD, 4},
  {MUL_PVR_PLAY_SPEED_2X_FAST_BACKWARD, 2},
  {MUL_PVR_PLAY_SPEED_NORMAL, 0},
  {MUL_PVR_PLAY_SPEED_2X_FAST_FORWARD, 2},
  {MUL_PVR_PLAY_SPEED_4X_FAST_FORWARD, 4},
  {MUL_PVR_PLAY_SPEED_8X_FAST_FORWARD, 8},
  {MUL_PVR_PLAY_SPEED_16X_FAST_FORWARD, 16},
  {MUL_PVR_PLAY_SPEED_32X_FAST_FORWARD, 32}
};

static const pvr_trick_mode_t g_pvr_slow_mode[PVR_PLAY_SLOW_NUMBER] =
{
  {MUL_PVR_PLAY_SPEED_8X_SLOW_BACKWARD, 8},
  {MUL_PVR_PLAY_SPEED_4X_SLOW_BACKWARD, 4},
  {MUL_PVR_PLAY_SPEED_2X_SLOW_BACKWARD, 2},
  {MUL_PVR_PLAY_SPEED_NORMAL, 0},
  {MUL_PVR_PLAY_SPEED_2X_SLOW_FORWARD, 2},
  {MUL_PVR_PLAY_SPEED_4X_SLOW_FORWARD, 4},
  {MUL_PVR_PLAY_SPEED_8X_SLOW_FORWARD, 8}
};

static list_xstyle_t pvr_play_bar_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

static list_xstyle_t pvr_play_plist_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t pvr_play_plist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t pvr_play_bar_list_attr[PVR_PLAY_BAR_LIST_FIELD] =
{
  { LISTFIELD_TYPE_ICON,
    28, 0, 0, &pvr_play_plist_field_rstyle,  &pvr_play_plist_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT,
    50, 30, 0, &pvr_play_plist_field_rstyle,  &pvr_play_plist_field_fstyle },
  { LISTFIELD_TYPE_UNISTR | STL_LEFT,
    125, 80, 0, &pvr_play_plist_field_rstyle,  &pvr_play_plist_field_fstyle },
  { LISTFIELD_TYPE_UNISTR | STL_LEFT,
    125, 205, 0, &pvr_play_plist_field_rstyle,  &pvr_play_plist_field_fstyle },
  { LISTFIELD_TYPE_UNISTR | STL_LEFT,
    300, 330, 0, &pvr_play_plist_field_rstyle,  &pvr_play_plist_field_fstyle }
};

static u8 STRING_EMPTY[] = " ";

static BOOL g_bPause = FALSE;
static pvr_play_mode_t g_play_mode = PVR_PLAY_NORMAL;
static u32 g_cur_play_time = 0;
static u32 g_cur_seek_sem = 0;
static u32 g_total_play_time = 0;
static u32 g_time_out = 5000;
static BOOL g_bSeek = FALSE;
static u8  g_speed_index = 0;
static pvr_param_t *g_p_pvr_param = NULL;
static BOOL g_bSwitchPlay = FALSE;
static u16 g_newfocus = 0;
#ifdef EXTERN_CA_PVR
extern u32 ca_msg_time;
#endif
//extern BOOL is_key_error;
extern u32 get_record_filename(u16 *p_ascstr, u16 *p_unistr);
u16 pvr_play_bar_mainwin_keymap(u16 key);

RET_CODE pvr_play_bar_mainwin_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 pvr_play_bar_bar_keymap(u16 key);

RET_CODE pvr_play_bar_bar_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 pvr_play_bar_list_keymap(u16 key);

RET_CODE pvr_play_bar_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 pvr_play_bar_pwd_keymap(u16 key);
RET_CODE pvr_play_bar_pwd_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);
u16 pvr_play_bar_time_duration_keymap(u16 key);
RET_CODE pvr_play_bar_time_duration_proc(control_t *cont, u16 msg,
                       u32 para1, u32 para2);

static RET_CODE on_pvr_play_bar_mainwin_dvrinfokey(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2);
static RET_CODE on_pvr_play_bar_mainwin_play_end(control_t *p_list, u16 msg, u32 para1, u32 para2);
static RET_CODE pvr_play_updatetime(control_t *p_mainwin);
static void pvr_play_bar_clear_seek_icon(control_t *p_ctrl);
static void pvr_play_bar_show_seek_icon(control_t *p_ctrl, s32 offset);


static void pvr_play_create_timer(void)
{
  if(SUCCESS != fw_tmr_create(ROOT_ID_PVR_PLAY_BAR, MSG_ONE_SECOND_ARRIVE, g_time_out, TRUE))
  {
    fw_tmr_reset(ROOT_ID_PVR_PLAY_BAR, MSG_ONE_SECOND_ARRIVE, g_time_out);
  }
}

static void pvr_play_paint_icon(control_t *p_ctrl, u8 idc, u16 bmp_id)
{
  control_t *p_subctrl;

  OS_PRINTF("%s,idc[%d],bmp_id[%d]\n",__FUNCTION__, idc, bmp_id);
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  idc);
  bmap_set_content_by_id(p_subctrl,  bmp_id);
  ctrl_paint_ctrl(p_subctrl,  TRUE);
}

static void pvr_play_paint_set_icon(control_t *p_ctrl, u8 idc, u16 bmp_id)
{
  control_t *p_subctrl;

  OS_PRINTF("%s,idc[%d],bmp_id[%d]\n",__FUNCTION__, idc, bmp_id);
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  idc);
  bmap_set_content_by_id(p_subctrl,  bmp_id);
}


static void pvr_play_in_bar_back(control_t *p_ctrl)
{
  control_t *p_text_ctrl;

  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_PAUSE_STAT_ICON,
    IM_MP3_ICON_PAUSE);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_PLAY_STAT_ICON,
    IM_MP3_ICON_PLAY_2);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_FB_STAT_ICON,
    IM_MP3_ICON_FB_V2);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_FF_STAT_ICON,
    IM_MP3_ICON_FF_V2);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_REVSLOW_STAT_ICON,
    IM_MP3_ICON_FB);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_SLOW_STAT_ICON,
    IM_MP3_ICON_FF);
  p_text_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_PLAY_SPEED);
  text_set_content_by_ascstr(p_text_ctrl, STRING_EMPTY);
  ctrl_paint_ctrl(p_text_ctrl, TRUE);
}

static void pvr_play_in_bar_set_back(control_t *p_ctrl)
{
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_PAUSE_STAT_ICON,
    IM_MP3_ICON_PAUSE);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_PLAY_STAT_ICON,
    IM_MP3_ICON_PLAY_2);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_FB_STAT_ICON,
    IM_MP3_ICON_FB_V2);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_FF_STAT_ICON,
    IM_MP3_ICON_FF_V2);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_REVSLOW_STAT_ICON,
    IM_MP3_ICON_FB);
  pvr_play_paint_icon(p_ctrl, IDC_PVR_PLAY_SLOW_STAT_ICON,
    IM_MP3_ICON_FF);
}

static void pvr_play_bar_show(control_t *p_main_win)
{
  control_t *p_pvr_frm = NULL;
  p_pvr_frm = ctrl_get_child_by_id(p_main_win, IDC_PVR_PLAY_BAR_FRAME);

  if(OBJ_STS_SHOW == ctrl_get_sts(p_pvr_frm))
  {
    return ;
  }

  //update time
  pvr_play_updatetime(p_main_win);
  ctrl_set_attr(p_pvr_frm, OBJ_ATTR_HL);
  ctrl_set_sts(p_pvr_frm, OBJ_STS_SHOW);
  pvr_play_bar_clear_seek_icon(p_main_win);
  ctrl_set_active_ctrl(p_main_win, p_pvr_frm);
  ctrl_process_msg(p_pvr_frm, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_main_win, TRUE);
  pvr_play_create_timer();
}

static RET_CODE pvr_play_updatetime(control_t *p_mainwin)
{
  control_t *p_pvr_frm, *p_cur_play_time, *p_play_progress;
  char asc[32];
  static u16 percent = 0;
  u16 temp = 0;

  p_pvr_frm = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_FRAME);
  p_cur_play_time = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_PLAY_PLAY_CUR_TIME);
  p_play_progress = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_PLAY_PLAY_PROGRESS);

  if(OBJ_STS_SHOW != ctrl_get_sts(p_pvr_frm))
  {
    return SUCCESS;
  }

  sprintf(asc, "%.2d:%.2d:%.2d",
    (int)g_cur_play_time/3600, (int)(g_cur_play_time%3600)/60, (int)g_cur_play_time%60);

  text_set_content_by_ascstr(p_cur_play_time, (u8 *)asc);

  temp = percent;
  if(g_total_play_time)
    percent = (u16)(g_cur_play_time*100/g_total_play_time);

  //OS_PRINTF("percent = %d\n",percent);
  pbar_set_current(p_play_progress, percent);

  if (temp != percent)
  {
    ctrl_paint_ctrl(p_play_progress, TRUE);
  }
  ctrl_paint_ctrl(p_cur_play_time, TRUE);

  if(OBJ_STS_SHOW == ctrl_get_sts(ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_PLAY_SEEK)))
  {
    pvr_play_bar_show_seek_icon(p_mainwin, 0);
  }

  return SUCCESS;
}

void fill_pvr_play_info(control_t *p_bar_cont, rec_info_t *p_rec_info)
{
  control_t *p_ctrl, *p_subctrl;
  char asc[32] = {0};
  u32 hour,min,sec;
  u16 program[PROGRAM_NAME_MAX]={0}, i = 0;

  MT_ASSERT(p_rec_info != NULL);
  p_ctrl = p_bar_cont;
  p_subctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_PVR_PLAY_BAR_LIST_CONT);
  list_get_focus_pos(ctrl_get_child_by_id(p_subctrl, IDC_PVR_PLAY_BAR_LIST));

  g_play_mode = PVR_PLAY_NORMAL;
  g_bPause = FALSE;
  g_bSeek = FALSE;
  g_cur_play_time = 0;
  g_speed_index = 0;
  g_total_play_time = p_rec_info->total_time;
  g_bSwitchPlay = FALSE;
  // set name
  {
    i = 0;
    while(p_rec_info->program[i] != 0)
    {
        program[i] = p_rec_info->program[i];
        i++;
    }
    program[i] = 0;
    p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_NAME);
    text_set_content_by_unistr(p_subctrl, program);
  }

  // play total time
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_PLAY_TOTAL_TIME);
  hour = g_total_play_time/3600;
  min = (g_total_play_time - (3600 * hour)) /60;
  sec = g_total_play_time - (hour * 3600) - (min * 60);
  sprintf(asc, "%.2d:%.2d:%.2d", (int)hour, (int)min, (int)sec);
  text_set_content_by_ascstr(p_subctrl, (u8 *)asc);

  // update view
  ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
}


static RET_CODE pvr_play_plist_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u8 asc_str[32];
  u16 file_uniname[200];
  flist_t *p_rec_fav = NULL;
  rec_info_t *p_rec_info = NULL;

  i = 0;
  
  for (; i < size; i++)
  {
    p_rec_fav = ui_pvr_get_rec_fav_by_index(i + start);
    if (i + start < cnt)
    {
      MT_ASSERT(p_rec_fav != NULL);
      p_rec_info = ui_pvr_get_rec_info(p_rec_fav);
      if(ui_pvr_get_lock_flag(i + start))
        list_set_field_content_by_icon(p_list, (u16)(start + i), 0, IM_TV_LOCK);
      else
        list_set_field_content_by_icon(p_list, (u16)(start + i), 0, 0);

      /* NO. */
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, asc_str);
#if 1
  	  if(p_rec_info->start.year)
  	  {
        //Date
        sprintf((char *)asc_str, "%d-%d-%d", p_rec_info->start.year, p_rec_info->start.month, p_rec_info->start.day);
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, asc_str);
      }
      else
      {
        //Date
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, STRING_EMPTY);
      }
      if(p_rec_info->start.hour)
      {
        //TIME
        sprintf((char *)asc_str, "%.2d:%.2d:%.2d", p_rec_info->start.hour, p_rec_info->start.minute, p_rec_info->start.second);
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 3, asc_str);
      }
      else
      {
        //TIME
          list_set_field_content_by_ascstr(p_list, (u16)(start + i), 3, STRING_EMPTY);
      }
      //file_list_get(FLIST_UNIT_NEXT, &g_list);
#endif

      /* NAME */
      get_record_filename(flist_get_name(p_rec_fav), file_uniname);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 4, file_uniname);
    }
    else
      break;
  }

  return SUCCESS;
}

static void pvr_fill_time_info(control_t *cont, BOOL is_redraw)
{
  utc_time_t cur_time;
  control_t *p_frm = NULL, *p_ctrl = NULL;
  u8 time_str[32];

  p_frm = ctrl_get_child_by_id(cont, IDC_PVR_PLAY_BAR_FRAME);
  MT_ASSERT(p_frm != NULL);

  time_get(&cur_time, FALSE);

  sprintf((char *)time_str, "%.4d/%.2d/%.2d", cur_time.year, cur_time.month, cur_time.day);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_PVR_PLAY_DATE);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  sprintf((char *)time_str, "%.2d:%.2d", cur_time.hour, cur_time.minute);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_PVR_PLAY_TIME);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

RET_CODE open_pvr_play_bar(u32 para1, u32 para2)
{
  control_t *p_cont;
  u16 i = 0;
  control_t *p_ctrl, *p_bar_ctrl, *p_subctrl, *p_play_progress, *p_trick;
  control_t *p_list_cont, *p_list,*p_time_ctrl, *p_sbar;
  flist_t *p_rec_fav = NULL;
  rec_info_t *p_rec_info = NULL;
  u8 separator[TBOX_MAX_ITEM_NUM] = {'-', '-', ' ', ':', ':', ' '};

  g_p_pvr_param = (pvr_param_t *)para1;
  p_rec_fav = ui_pvr_get_rec_fav_by_index(g_p_pvr_param->focus);

  p_cont = fw_create_mainwin(ROOT_ID_PVR_PLAY_BAR,
                           PVR_PLAY_BAR_CONT_X,
                           PVR_PLAY_BAR_CONT_Y,
                           PVR_PLAY_BAR_CONT_W,
                           PVR_PLAY_BAR_CONT_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE,
                           0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, pvr_play_bar_mainwin_keymap);
  ctrl_set_proc(p_cont, pvr_play_bar_mainwin_proc);

  //list container
  p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PVR_PLAY_BAR_LIST_CONT,
                             PVR_PLAY_BAR_LIST_CONT_X, PVR_PLAY_BAR_LIST_CONT_Y,
                             PVR_PLAY_BAR_LIST_CONT_W,PVR_PLAY_BAR_LIST_CONT_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_list_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_sts(p_list_cont, OBJ_STS_HIDE);

  //title
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PLAY_BAR_TITLE,
                             PVR_PLAY_BAR_TITLE_X, PVR_PLAY_BAR_TITLE_Y,
                             PVR_PLAY_BAR_TITLE_W,PVR_PLAY_BAR_TITLE_H,
                             p_list_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_CENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_RECORD_FILE);

  // list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PVR_PLAY_BAR_LIST,
                             PVR_PLAY_BAR_LIST_X, PVR_PLAY_BAR_LIST_Y,
                             PVR_PLAY_BAR_LIST_W,PVR_PLAY_BAR_LIST_H,
                             p_list_cont, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, pvr_play_bar_list_keymap);
  ctrl_set_proc(p_list, pvr_play_bar_list_proc);
  ctrl_set_mrect(p_list, PVR_PLAY_BAR_LIST_MIDL, PVR_PLAY_BAR_LIST_MIDT,
    PVR_PLAY_BAR_LIST_MIDL + PVR_PLAY_BAR_LIST_MIDW, PVR_PLAY_BAR_LIST_MIDT + PVR_PLAY_BAR_LIST_MIDH);
  list_set_item_interval(p_list,PVR_PLAY_BAR_LIST_VGAP);
  list_set_item_rstyle(p_list, &pvr_play_bar_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, (u16)g_p_pvr_param->total, PVR_PLAY_BAR_LIST_PAGE);
  list_set_field_count(p_list, PVR_PLAY_BAR_LIST_FIELD, PVR_PLAY_BAR_LIST_PAGE);
  list_set_focus_pos(p_list, g_p_pvr_param->focus);

  for (i = 0; i < PVR_PLAY_BAR_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(pvr_play_bar_list_attr[i].attr),
                        (u16)(pvr_play_bar_list_attr[i].width),
                        (u16)(pvr_play_bar_list_attr[i].left), (u8)(pvr_play_bar_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, pvr_play_bar_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, pvr_play_bar_list_attr[i].fstyle);
  }
  list_set_update(p_list, pvr_play_plist_update, 0);

  //sbar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_PVR_PLAY_BAR_LIST_SCROLL,
                            PVR_PLAY_BAR_LIST_SBAR_X, PVR_PLAY_BAR_LIST_SBAR_Y,
                            PVR_PLAY_BAR_LIST_SBAR_W, PVR_PLAY_BAR_LIST_SBAR_H,
                            p_list_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
//  ctrl_set_mrect(p_sbar, PVR_PLAY_BAR_LIST_BAR_X, PVR_PLAY_BAR_LIST_BAR_Y, PVR_PLAY_BAR_LIST_BAR_W, PVR_PLAY_BAR_LIST_BAR_H);
  list_set_scrollbar(p_list, p_sbar);

  //duration
  p_time_ctrl = ui_comm_timedit_create(p_cont, IDC_PVR_PLAY_DUR_FRAME,
          PVR_PLAY_DUR_TIME_X, PVR_PLAY_DUR_TIME_Y, PVR_PLAY_DUR_TIME_LW, PVR_PLAY_DUR_TIME_RW);
  ui_comm_ctrl_set_keymap(p_time_ctrl, pvr_play_bar_time_duration_keymap);
  ui_comm_ctrl_set_proc(p_time_ctrl, pvr_play_bar_time_duration_proc);
  ui_comm_timedit_set_static_txt(p_time_ctrl, IDS_GO_TO);
  ui_comm_timedit_set_param(p_time_ctrl, 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN | TBOX_SECOND, 
    TBOX_SEPARATOR_TYPE_UNICODE, 18);

  for(i = 0; i < TBOX_MAX_ITEM_NUM; i++)
  {
    ui_comm_timedit_set_separator_by_ascchar(p_time_ctrl, (u8)i, separator[i]);
  }
  ctrl_set_sts(p_time_ctrl, OBJ_STS_HIDE);


  //frame part
  p_bar_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_PVR_PLAY_BAR_FRAME,
                             PVR_PLAY_BAR_FRM_X, PVR_PLAY_BAR_FRM_Y,
                             PVR_PLAY_BAR_FRM_W,PVR_PLAY_BAR_FRM_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_bar_ctrl, RSI_PVR_PLAY_BAR_FRM,
                  RSI_PVR_PLAY_BAR_FRM, RSI_PVR_PLAY_BAR_FRM);
  ctrl_set_keymap(p_bar_ctrl, pvr_play_bar_bar_keymap);
  ctrl_set_proc(p_bar_ctrl, pvr_play_bar_bar_proc);

  //PVR icon
  #if 0
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_ICON,
    PVR_PLAY_BAR_ICON_X, PVR_PLAY_BAR_ICON_Y,
    PVR_PLAY_BAR_ICON_W, PVR_PLAY_BAR_ICON_H,
    p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_PVR);
#endif
  // name
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PLAY_NAME,
                            PVR_PLAY_BAR_NAME_TXT_X, PVR_PLAY_BAR_NAME_TXT_Y,
                            PVR_PLAY_BAR_NAME_TXT_W, PVR_PLAY_BAR_NAME_TXT_H,
                            p_bar_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_PLAY_BAR_NAME,
                      FSI_PVR_PLAY_BAR_NAME, FSI_PVR_PLAY_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // date & time
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    PVR_PLAY_BAR_DATE_ICON_X, PVR_PLAY_BAR_DATE_ICON_Y,
    PVR_PLAY_BAR_DATE_ICON_W, PVR_PLAY_BAR_DATE_ICON_H,
    p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PLAY_DATE,
                            PVR_PLAY_BAR_DATE_TXT_X, PVR_PLAY_BAR_DATE_TXT_Y,
                            PVR_PLAY_BAR_DATE_TXT_W, PVR_PLAY_BAR_DATE_TXT_H,
                            p_bar_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_PLAY_BAR_DATE,
                      FSI_PVR_PLAY_BAR_DATE,FSI_PVR_PLAY_BAR_DATE);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    PVR_PLAY_BAR_TIME_ICON_X, PVR_PLAY_BAR_TIME_ICON_Y,
    PVR_PLAY_BAR_TIME_ICON_W, PVR_PLAY_BAR_TIME_ICON_H,
    p_bar_ctrl, 0);
 // bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PLAY_TIME,
                            PVR_PLAY_BAR_TIME_TXT_X, PVR_PLAY_BAR_TIME_TXT_Y,
                            PVR_PLAY_BAR_TIME_TXT_W, PVR_PLAY_BAR_TIME_TXT_H,
                            p_bar_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_PLAY_BAR_DATE,
                      FSI_PVR_PLAY_BAR_DATE,FSI_PVR_PLAY_BAR_DATE);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //play progress
  p_play_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_PVR_PLAY_PLAY_PROGRESS,
    PVR_PLAY_BAR_PLAY_PROGRESS_X, PVR_PLAY_BAR_PLAY_PROGRESS_Y,
    PVR_PLAY_BAR_PLAY_PROGRESS_W, PVR_PLAY_BAR_PLAY_PROGRESS_H,
    p_bar_ctrl, 0);
  ctrl_set_rstyle(p_play_progress, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_play_progress,
    PVR_PLAY_BAR_PLAY_PROGRESS_MIDX, PVR_PLAY_BAR_PLAY_PROGRESS_MIDY,
    PVR_PLAY_BAR_PLAY_PROGRESS_MIDW, PVR_PLAY_BAR_PLAY_PROGRESS_MIDH);
  pbar_set_rstyle(p_play_progress, PVR_PLAY_BAR_PLAY_PROGRESS_MIN, PVR_PLAY_BAR_PLAY_PROGRESS_MAX, PVR_PLAY_BAR_PLAY_PROGRESS_MID);
  pbar_set_count(p_play_progress, 0, 100, 100);
  pbar_set_direction(p_play_progress, 1);
  pbar_set_workmode(p_play_progress, 0, 0);
  pbar_set_current(p_play_progress, 0);

  //seek point
  p_trick = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_SEEK,
                             PVR_PLAY_BAR_TRICK_X, PVR_PLAY_BAR_TRICK_Y, 
                             PVR_PLAY_BAR_TRICK_W, PVR_PLAY_BAR_TRICK_H, 
                             p_bar_ctrl, 0);
  
  ctrl_set_sts(p_trick, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_trick, IM_ICONS_TRICK);

  // play cur time
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PLAY_PLAY_CUR_TIME,
                            PVR_PLAY_BAR_PLAY_CUR_TIME_X, PVR_PLAY_BAR_PLAY_CUR_TIME_Y,
                            PVR_PLAY_BAR_PLAY_CUR_TIME_W, PVR_PLAY_BAR_PLAY_CUR_TIME_H,
                            p_bar_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_PLAY_BAR_INFO,
                      FSI_PVR_PLAY_BAR_INFO, FSI_PVR_PLAY_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_subctrl, (u8 *)"00:00:00");

  // play total time
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PLAY_PLAY_TOTAL_TIME,
                            PVR_PLAY_BAR_PLAY_TOTAL_TIME_X, PVR_PLAY_BAR_PLAY_TOTAL_TIME_Y,
                            PVR_PLAY_BAR_PLAY_TOTAL_TIME_W, PVR_PLAY_BAR_PLAY_TOTAL_TIME_H,
                            p_bar_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_PLAY_BAR_INFO,
                      FSI_PVR_PLAY_BAR_INFO, FSI_PVR_PLAY_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

 //usb capacity
  p_subctrl = ui_comm_bar_create(p_bar_ctrl, IDC_PVR_PLAY_USB_CAPACITY,
  	                                            PVR_PLAY_BAR_CAPACITY_ICON_X,PVR_PLAY_BAR_CAPACITY_ICON_Y,
  	                                            PVR_PLAY_BAR_CAPACITY_ICON_W,PVR_PLAY_BAR_CAPACITY_ICON_H,
  	                                            PVR_PLAY_BAR_CAPACITY_TEXT_X,PVR_PLAY_BAR_CAPACITY_TEXT_Y,
  	                                            PVR_PLAY_BAR_CAPACITY_TEXT_W,PVR_PLAY_BAR_CAPACITY_TEXT_H,
  	                                            (PVR_PLAY_BAR_CAPACITY_TEXT_X - 10), PVR_PLAY_BAR_CAPACITY_TEXT_Y,
  	                                            PVR_PLAY_BAR_CAPACITY_TEXT_W,PVR_PLAY_BAR_CAPACITY_TEXT_H);

  ui_comm_bar_set_param(p_subctrl, RSC_INVALID_ID, 0, 100, 100);

  ui_comm_bar_set_style(p_subctrl, RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_MID_ORANGE,
  						RSI_IGNORE, FSI_WHITE, RSI_PBACK, FSI_WHITE);

  ui_comm_bar_update(p_subctrl, ui_pvr_get_capacity(), TRUE);

  //pause   4
   p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_PAUSE_STAT_ICON,
    PVR_PLAY_BAR_PLAY_STAT_ICON_X + 202, PVR_PLAY_BAR_PLAY_STAT_ICON_Y,
    PVR_PLAY_BAR_PLAY_STAT_ICON_W, PVR_PLAY_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_PAUSE);

  //play icon   3
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_PLAY_STAT_ICON,
    PVR_PLAY_BAR_PLAY_STAT_ICON_X + 132, PVR_PLAY_BAR_PLAY_STAT_ICON_Y,
    PVR_PLAY_BAR_PLAY_STAT_ICON_W, PVR_PLAY_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_PLAY_2_SELECT);

  //FB   2
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_FB_STAT_ICON,
    PVR_PLAY_BAR_PLAY_STAT_ICON_X + 62, PVR_PLAY_BAR_PLAY_STAT_ICON_Y,
    PVR_PLAY_BAR_PLAY_STAT_ICON_W, PVR_PLAY_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_FB_V2);

  //FF  5
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_FF_STAT_ICON,
    PVR_PLAY_BAR_PLAY_STAT_ICON_X+272, PVR_PLAY_BAR_PLAY_STAT_ICON_Y,
    PVR_PLAY_BAR_PLAY_STAT_ICON_W, PVR_PLAY_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_FF_V2);

  //Revslow     1
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_REVSLOW_STAT_ICON,
    PVR_PLAY_BAR_PLAY_STAT_ICON_X - 8, PVR_PLAY_BAR_PLAY_STAT_ICON_Y,
    PVR_PLAY_BAR_PLAY_STAT_ICON_W, PVR_PLAY_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_FB);

  //Slow   6
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PLAY_SLOW_STAT_ICON,
    PVR_PLAY_BAR_PLAY_STAT_ICON_X+342, PVR_PLAY_BAR_PLAY_STAT_ICON_Y,
    PVR_PLAY_BAR_PLAY_STAT_ICON_W, PVR_PLAY_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_FF);

  // play speed
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PLAY_PLAY_SPEED,
                            PVR_PLAY_BAR_PLAY_STAT_ICON_X + 132 +PVR_PLAY_BAR_PLAY_STAT_ICON_W, PVR_PLAY_BAR_PLAY_SPEED_Y,
                            PVR_PLAY_BAR_PLAY_SPEED_W, PVR_PLAY_BAR_PLAY_SPEED_H,
                            p_bar_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_PLAY_BAR_INFO,
                      FSI_PVR_PLAY_BAR_INFO, FSI_PVR_PLAY_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  p_rec_info = ui_pvr_get_rec_info(p_rec_fav);

  pvr_fill_time_info(p_cont, FALSE);
  fill_pvr_play_info(p_bar_ctrl, p_rec_info);

  //ui_enable_signal_monitor(TRUE);
  ctrl_process_msg(p_bar_ctrl, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  {
    osd_set_t osd_set;
    sys_status_get_osd_set(&osd_set);
    g_time_out = osd_set.timeout * 1000;
    pvr_play_create_timer();
  }

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
    ui_set_mute(ui_is_mute());
  }
  #else
  ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
  ui_set_mute(ui_is_mute());
  #endif
  
  pvr_play_plist_update(p_list, list_get_valid_pos(p_list), PVR_PLAY_BAR_LIST_PAGE, 0);
  
  return SUCCESS;
}
//lint -e438 -e830
static RET_CODE on_pvr_play_bar_mainwin_play_pause(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u8  child_id = 0;
  u16 bmp_id = 0;

  OS_PRINTF("%s,playmode[%d],g_bPause[%d]\n",__FUNCTION__,g_play_mode,g_bPause);
  if ((g_play_mode != PVR_PLAY_NORMAL) && (g_play_mode != PVR_PLAY_PAUSE))
  {
    ts_player_play_mode(MUL_PVR_PLAY_SPEED_NORMAL);
    g_bPause = FALSE;
    g_play_mode = PVR_PLAY_NORMAL;
    child_id = IDC_PVR_PLAY_PLAY_STAT_ICON;
    bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
    ui_set_mute(ui_is_mute());
  }
  else
  {
    if (g_bPause == TRUE)
    {
      ts_player_resume();
      if(g_bSeek)
      {
        ts_player_seek_param_t seek;
        memset(&seek, 0, sizeof(ts_player_seek_param_t));
        seek.seek_type = TS_PLAYER_TIME_SEEK;
        seek.seek_op = TS_PLAYER_SEEK_SET;
        seek.offset = g_cur_play_time;
        ts_player_seek(&seek);
        g_bSeek = FALSE;
      }
      g_bPause = FALSE;
      g_play_mode = PVR_PLAY_NORMAL;
      child_id = IDC_PVR_PLAY_PLAY_STAT_ICON;
      bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
    }
    else
    {
      ts_player_pause();
      g_bPause = TRUE;
      g_play_mode = PVR_PLAY_PAUSE;
      child_id = IDC_PVR_PLAY_PAUSE_STAT_ICON;
      bmp_id = IM_MP3_ICON_PAUSE_SELECT;
    }
  }
  {
    control_t *p_bar_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_BAR_FRAME);

    if(!ctrl_is_onshow(p_bar_ctrl))
    {
      ctrl_set_sts(p_bar_ctrl, OBJ_STS_SHOW);
      ctrl_process_msg(p_bar_ctrl, MSG_GETFOCUS, 0, 0);
      ctrl_paint_ctrl(p_bar_ctrl, TRUE);
    }
    else
    {
      //clear speed
      control_t *p_text_ctrl = ctrl_get_child_by_id(p_bar_ctrl, IDC_PVR_PLAY_PLAY_SPEED);
      text_set_content_by_ascstr(p_text_ctrl, STRING_EMPTY);
      ctrl_paint_ctrl(p_text_ctrl, TRUE);
    }
    pvr_play_in_bar_back(p_bar_ctrl);

    pvr_play_paint_icon(p_bar_ctrl, child_id, bmp_id);
  }

  pvr_play_create_timer();
  return SUCCESS;
}
//lint +e438 +e830
static s16 pvr_play_bar_get_seek_steps(control_t *p_trick, s32 offset)
{
    rect_t rect       = {0};
    s16    center     = 0;
    s16    new_center = 0;
    u32    total_time = 0;
    u32    cur_time = 0;
    
    ctrl_get_frame(p_trick, &rect); 
    
    center = (rect.left + rect.right ) / 2;      

    total_time = g_total_play_time;
    cur_time = (u32)((s32)g_cur_play_time + offset);
    
    if(0 != total_time)
    {
      new_center = (s16)((PVR_PLAY_BAR_PLAY_PROGRESS_W * (s32)cur_time) / (s16)total_time);
    }
    
    new_center += PVR_PLAY_BAR_PLAY_PROGRESS_X;

    return (new_center - center);
    
}

static void pvr_play_bar_clear_seek_icon(control_t *p_ctrl)
{
    control_t *p_pvr_frm, *p_seek;

    p_pvr_frm = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_BAR_FRAME);
    p_seek = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_PLAY_SEEK);
        
    if(OBJ_STS_SHOW == ctrl_get_sts(p_seek))
    {
      ctrl_set_sts(p_seek, OBJ_STS_HIDE);
      //ctrl_erase_ctrl(p_seek);
      ctrl_paint_ctrl(p_pvr_frm, TRUE);
    }  
}

static void pvr_play_bar_show_seek_icon(control_t *p_ctrl, s32 offset)
{
    control_t *p_pvr_frm, *p_seek;
    s16 steps = 0;

    p_pvr_frm = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_BAR_FRAME);
    p_seek = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_PLAY_SEEK);
  
    ctrl_empty_invrgn(p_seek);

    steps = pvr_play_bar_get_seek_steps(p_seek, offset);
    ctrl_move_ctrl(p_seek, steps, 0);
    
    ctrl_set_sts(p_seek, OBJ_STS_SHOW);
    ctrl_paint_ctrl(p_pvr_frm, TRUE);
}

//lint -e438 -e830
static RET_CODE on_pvr_play_bar_mainwin_state_change(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_bar_ctrl = NULL, *p_text_ctrl = NULL;
  mul_pvr_play_speed_t speed = MUL_PVR_PLAY_SPEED_NORMAL;
  s32 offset = 0;
  u8  child_id = 0;
  u16 bmp_id = 0;
  pvr_play_mode_t saved_play_mode = g_play_mode;
  u8  str_speed[20] = {0};
  ts_player_seek_param_t seek;
  memset(&seek, 0, sizeof(ts_player_seek_param_t));

  p_bar_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_BAR_FRAME);

  OS_PRINTF("%s,playmode[%d],g_bPause[%d],g_speed_index[%d]\n",__FUNCTION__,g_play_mode,g_bPause,g_speed_index);
  if(!ctrl_is_onshow(p_bar_ctrl))
  {
    u32 key = 0;
    switch(msg)
    {
      case MSG_TS_PLAY_SEEK_B:
        key = V_KEY_LEFT;
        break;
      case MSG_TS_PLAY_SEEK_F:
        key = V_KEY_RIGHT;
        break;
      default:
        break;
    }
    if(key)
      return open_volume_usb(ROOT_ID_PVR_PLAY_BAR, key);
  }
  switch(msg)
  {
  case MSG_TS_PLAY_VOL_UP:
		return open_volume_usb(ROOT_ID_PVR_PLAY_BAR, V_KEY_VUP);
  case MSG_TS_PLAY_VOL_DOWN:
		return open_volume_usb(ROOT_ID_PVR_PLAY_BAR, V_KEY_VDOWN);
  case MSG_TS_PLAY_NORMAL:
    g_play_mode = PVR_PLAY_NORMAL;
    g_bPause = FALSE;
    if(OBJ_STS_SHOW != ctrl_get_sts(p_bar_ctrl))
    {
      on_pvr_play_bar_mainwin_dvrinfokey(p_ctrl, msg, para1, para2);
    }
    pvr_play_in_bar_back(p_bar_ctrl);
    pvr_play_paint_icon(p_bar_ctrl, IDC_PVR_PLAY_PLAY_STAT_ICON,
      IM_MP3_ICON_PLAY_2_SELECT);
    return SUCCESS;

  case MSG_TS_PLAY_STOP:
    return on_pvr_play_bar_mainwin_play_end(NULL,0,0,0);

  case MSG_TS_PLAY_FB:
    if(g_play_mode == PVR_PLAY_FAST)
    {
      if((g_speed_index < 1) || (g_speed_index > PVR_PLAY_FAST_NORMAL))
        g_speed_index = PVR_PLAY_FAST_NORMAL - 1;
      else
        g_speed_index --;
    }
    else
    {
      g_speed_index = PVR_PLAY_FAST_NORMAL - 1;
      g_play_mode = PVR_PLAY_FAST;
    }
    break;

  case MSG_TS_PLAY_FF:
    if(g_play_mode == PVR_PLAY_FAST)
    {
      if((g_speed_index == PVR_PLAY_FAST_NUMBER - 1) || (g_speed_index <= PVR_PLAY_FAST_NORMAL))
        g_speed_index = PVR_PLAY_FAST_NORMAL + 1;
      else
        g_speed_index ++;
    }
    else
    {
      g_speed_index = PVR_PLAY_FAST_NORMAL + 1;
      g_play_mode = PVR_PLAY_FAST;
    }
    break;

  case MSG_TS_PLAY_REVSLOW:
    if(g_play_mode == PVR_PLAY_SLOW)
    {
      if((g_speed_index < 1) || (g_speed_index > PVR_PLAY_SLOW_NORMAL))
        g_speed_index = PVR_PLAY_SLOW_NORMAL - 1;
      else
        g_speed_index --;
    }
    else
    {
      g_speed_index = PVR_PLAY_SLOW_NORMAL - 1;
      g_play_mode = PVR_PLAY_SLOW;
    }
    break;

  case MSG_TS_PLAY_SLOW:
    if(g_play_mode == PVR_PLAY_SLOW)
    {
      if((g_speed_index == PVR_PLAY_SLOW_NUMBER - 1) || (g_speed_index <= PVR_PLAY_SLOW_NORMAL))
        g_speed_index = PVR_PLAY_SLOW_NORMAL + 1;
      else
        g_speed_index ++;
    }
    else
    {
      g_speed_index = PVR_PLAY_SLOW_NORMAL + 1;
      g_play_mode = PVR_PLAY_SLOW;
    }
    break;

  case MSG_TS_PLAY_SEEK_B:
  case MSG_TS_PLAY_SEEK_F:
    if((g_play_mode == PVR_PLAY_NORMAL) || (g_play_mode == PVR_PLAY_PAUSE))
    {
      //do seek
      if(g_cur_seek_sem)
        return SUCCESS;
      g_play_mode = PVR_PLAY_SEEK;
      break;
    }
    else
    {
      //resume to normal play
      ts_player_play_mode(MUL_PVR_PLAY_SPEED_NORMAL);
      g_play_mode = PVR_PLAY_NORMAL;
      pvr_play_in_bar_back(p_bar_ctrl);
      pvr_play_paint_icon(p_bar_ctrl, IDC_PVR_PLAY_PLAY_STAT_ICON,
        IM_MP3_ICON_PLAY_2_SELECT);
    }
    return SUCCESS;

  case MSG_PLAY_MODE_CHANGED:
    g_play_mode = PVR_PLAY_NORMAL;
    g_bPause = FALSE;
    break;

  default:
    MT_ASSERT(0);
    break;
  }

  if (g_bPause == TRUE && g_play_mode != PVR_PLAY_SEEK)
  {
    ts_player_resume();
    g_bPause = FALSE;
  }
  switch(g_play_mode)
  {
  case PVR_PLAY_NORMAL:
    g_bPause = FALSE;
    speed = MUL_PVR_PLAY_SPEED_NORMAL;
    child_id = IDC_PVR_PLAY_PLAY_STAT_ICON;
    bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
    break;

  case PVR_PLAY_FAST:
    if(g_speed_index > PVR_PLAY_FAST_NORMAL)
    {
      child_id = IDC_PVR_PLAY_FF_STAT_ICON;
      bmp_id = IM_MP3_ICON_FF_SELECT_V2;
    }
    else
    {
      child_id = IDC_PVR_PLAY_FB_STAT_ICON;
      bmp_id = IM_MP3_ICON_FB_SELECT_V2;
    }
    speed = g_pvr_fast_mode[g_speed_index].play_speed;
    sprintf((char *)str_speed,"x%d",g_pvr_fast_mode[g_speed_index].value);
    break;

  case PVR_PLAY_SLOW:
    if(g_speed_index > PVR_PLAY_SLOW_NORMAL)
    {
      child_id = IDC_PVR_PLAY_SLOW_STAT_ICON;
      bmp_id = IM_MP3_ICON_FF_SELECT;
    }
    else
    {
      child_id = IDC_PVR_PLAY_REVSLOW_STAT_ICON;
      bmp_id = IM_MP3_ICON_FB_SELECT;
    }
    speed = g_pvr_slow_mode[g_speed_index].play_speed;
    sprintf((char *)str_speed,"x1/%d",g_pvr_slow_mode[g_speed_index].value);
    break;

  case PVR_PLAY_SEEK:
    child_id = IDC_PVR_PLAY_PLAY_STAT_ICON;
    bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
    offset = ui_jump_para_get();
    if(msg  == MSG_TS_PLAY_SEEK_B)
    {
      if(g_cur_play_time < (u32)offset)
      {
        offset = (-(s32)g_cur_play_time);
      }
      else
      {
        offset = (-offset);
      }

      //sprintf(str_speed,"+%lds", offset);
      seek.seek_type = TS_PLAYER_TIME_SEEK;
      seek.seek_op = TS_PLAYER_SEEK_CUR;
      seek.offset = offset;
    }
    else
    {
      if((u32)((s32)g_cur_play_time + offset) > g_total_play_time)
        offset = (s32)(g_total_play_time - g_cur_play_time - 2);//return SUCCESS;
      if(offset < 0)
      	offset = 0;
      //sprintf(str_speed,"%lds", offset);
      seek.seek_type = TS_PLAYER_TIME_SEEK;
      seek.seek_op = TS_PLAYER_SEEK_CUR;
      seek.offset = offset;
    }
    pvr_play_create_timer();

    pvr_play_bar_show_seek_icon(p_ctrl, offset);
    
    if(saved_play_mode == PVR_PLAY_NORMAL)
    {
      g_cur_seek_sem = 1;
      ts_player_seek(&seek);
    }
    else
    {
      pvr_play_updatetime(p_ctrl);
      g_bSeek = TRUE;
    }
    g_play_mode = saved_play_mode;
    return SUCCESS;

  default:
    return SUCCESS;
  }

  if(!ctrl_is_onshow(p_bar_ctrl))
  {
    ctrl_set_sts(p_bar_ctrl, OBJ_STS_SHOW);
    ctrl_paint_ctrl(p_bar_ctrl, TRUE);
    ctrl_set_active_ctrl(p_ctrl, p_bar_ctrl);
  }
  pvr_play_in_bar_back(p_bar_ctrl);
  if(msg != MSG_PLAY_MODE_CHANGED)
    ts_player_play_mode(speed);

  pvr_play_paint_icon(p_bar_ctrl, child_id, bmp_id);

  p_text_ctrl = ctrl_get_child_by_id(p_bar_ctrl, IDC_PVR_PLAY_PLAY_SPEED);
  text_set_content_by_ascstr(p_text_ctrl, str_speed);
  pvr_play_updatetime(p_ctrl);
  ctrl_paint_ctrl(p_text_ctrl, TRUE);
  pvr_play_create_timer();
  return SUCCESS;
}
//lint +e438 +e830

static RET_CODE on_pvr_play_bar_mainwin_switchplay(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 focus = 0;
  flist_t *p_rec_fav = NULL;
  rec_info_t *p_rec_info = NULL;
  control_t *p_bar_frm = NULL;
  control_t *p_text_ctrl = NULL;
  comm_pwdlg_data_t rec_pwdlg_data =
  {
    ROOT_ID_RECORD_MANAGER,
    PVR_PLAY_BAR_PWD_DLG_FOR_CHK_X,
    PVR_PLAY_BAR_PWD_DLG_FOR_CHK_Y,
    IDS_MSG_INPUT_PASSWORD,
    pvr_play_bar_pwd_keymap,
    pvr_play_bar_pwd_proc,
    PWDLG_T_COMMON
  };
  
  if(g_p_pvr_param->total == 0)
    return SUCCESS;

  if(msg == MSG_TS_PLAY_PREV)
  {
    if(g_p_pvr_param->focus < 1)
      focus = (u16)g_p_pvr_param->total - 1;
    else
      focus = (u16)g_p_pvr_param->focus - 1;
  }
  else
  {
    if(g_p_pvr_param->focus + 1 >= g_p_pvr_param->total)
      focus = 0;
    else
      focus = g_p_pvr_param->focus + 1;
  }

  p_rec_fav = ui_pvr_get_rec_fav_by_index(focus);
  p_rec_info = ui_pvr_get_rec_info(p_rec_fav);
    
  if( ui_pvr_get_lock_flag(focus) )
  {
    g_bSwitchPlay = TRUE;
    g_newfocus = focus;
    ui_comm_pwdlg_open(&rec_pwdlg_data);
    return SUCCESS;
  }
  g_p_pvr_param->focus = focus;

  ui_set_mute(ui_is_mute());

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
    cas_manager_start_pvr_play(focus);
    #endif
  }
  #else
  ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
  #ifdef ENABLE_CA
  cas_manager_start_pvr_play(focus);
  #endif
  #endif

  
  p_bar_frm = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_BAR_FRAME);
  list_set_focus_pos(
    ctrl_get_child_by_id(ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_BAR_LIST_CONT), IDC_PVR_PLAY_BAR_LIST),
    focus);
  p_rec_fav = ui_pvr_get_rec_fav_by_index(focus);
  p_rec_info = ui_pvr_get_rec_info(p_rec_fav);
  fill_pvr_play_info(p_bar_frm, p_rec_info);
  if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_frm))
  {
    pvr_play_create_timer();
    pvr_play_updatetime(p_ctrl);
    pvr_play_in_bar_back(p_bar_frm);
    pvr_play_paint_icon(p_bar_frm, IDC_PVR_PLAY_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2_SELECT);
    p_text_ctrl = ctrl_get_child_by_id(p_bar_frm, IDC_PVR_PLAY_PLAY_SPEED);
    text_set_content_by_ascstr(p_text_ctrl, STRING_EMPTY);
    ctrl_paint_ctrl(p_text_ctrl, TRUE);
  }
  else
  {
    pvr_play_in_bar_set_back(p_bar_frm);
    pvr_play_paint_set_icon(p_bar_frm, IDC_PVR_PLAY_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2_SELECT);
    pvr_play_bar_show(p_ctrl);
    p_text_ctrl = ctrl_get_child_by_id(p_bar_frm, IDC_PVR_PLAY_PLAY_SPEED);
    text_set_content_by_ascstr(p_text_ctrl, STRING_EMPTY);
    ctrl_paint_ctrl(p_text_ctrl, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_mainwin_time_update(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_bar_frm = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_BAR_FRAME);
  
  if(OBJ_STS_SHOW != ctrl_get_sts(p_bar_frm))
  {
    return SUCCESS;
  }
  
  if(!ctrl_is_onshow(p_ctrl))
  {
    return SUCCESS;
  }
  
  pvr_fill_time_info(p_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_mainwin_dvrinfokey(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  pvr_play_bar_show(p_ctrl);
  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_mainwin_seek_time(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_seek_time = NULL, *p_bar_frm = NULL;
  control_t *p_mainwin = p_ctrl;
  utc_time_t seek_time = {0,0,0,0,0,0,0};

  if(g_play_mode != PVR_PLAY_NORMAL)
    return SUCCESS;
  
  p_seek_time = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PLAY_DUR_FRAME);
  ui_comm_timedit_set_time(p_seek_time, &seek_time);

  p_bar_frm = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_FRAME);
  
  if(ctrl_get_sts(p_bar_frm) != OBJ_STS_SHOW)
  {
    ctrl_process_msg(p_bar_frm, MSG_LOSTFOCUS, 0, 0);
  }
  
  ctrl_set_attr(p_seek_time, OBJ_ATTR_HL);
  ctrl_set_sts(p_seek_time, OBJ_STS_SHOW);
  ctrl_set_active_ctrl(p_ctrl, p_seek_time);
  ctrl_process_msg(p_seek_time, MSG_GETFOCUS, 0, 0);
  ui_comm_timedit_enter_edit(p_seek_time);

  ctrl_paint_ctrl(p_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_mainwin_destroy(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  OS_PRINTF("=======%s\n", __FUNCTION__);
  ts_player_stop();
  //ts_player_release();

  fw_tmr_destroy(ROOT_ID_PVR_PLAY_BAR, MSG_ONE_SECOND_ARRIVE);
  //ui_enable_signal_monitor(FALSE);
  if(ui_is_mute())
  {
    close_mute();
  }
  fw_notify_root(fw_find_root_by_id(ROOT_ID_RECORD_MANAGER), NOTIFY_T_MSG,
    FALSE, MSG_PVR_FOCUS_CHANGED, g_p_pvr_param->focus, 0);

  return ERR_NOFEATURE;
}


static RET_CODE on_pvr_play_bar_mainwin_exit_to_menu(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  OS_PRINTF("=======%s,[%d]\n", __FUNCTION__,g_p_pvr_param->b_from_list);
  if(g_p_pvr_param->b_from_list)
    ui_close_all_mennus();
  else
    manage_close_menu(ROOT_ID_PVR_PLAY_BAR, 0, 0);

  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_mainwin_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  OS_PRINTF("=======%s\n", __FUNCTION__);
  return on_pvr_play_bar_mainwin_exit_to_menu(p_cont, msg, para1, para2);
}

static RET_CODE on_pvr_play_bar_mainwin_play_end(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("=======%s\n", __FUNCTION__);

  if(g_bSwitchPlay)
    ui_comm_pwdlg_close();

  if(msg != MSG_PLAY_END)
  {
    return fw_destroy_mainwin_by_id(ROOT_ID_PVR_PLAY_BAR);
  }
  else
  {
    return on_pvr_play_bar_mainwin_switchplay(p_list, MSG_TS_PLAY_NEXT, para1, para2);
  }
}

static RET_CODE on_pvr_play_bar_mainwin_cannot_play(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("\n\n##pvr play error!!!\n\n\n");
  if(g_bSwitchPlay)
    ui_comm_pwdlg_close();
  ui_comm_cfmdlg_open(NULL, IDS_USB_ERROR, NULL, 0);
  
  return fw_destroy_mainwin_by_id(ROOT_ID_PVR_PLAY_BAR);
}

static RET_CODE on_pvr_play_bar_mainwin_played_time(control_t *p_mainwin, u16 msg, u32 para1, u32 para2)
{
  g_cur_seek_sem = 0;
  g_cur_play_time = para1 / 1000;
  pvr_play_updatetime(p_mainwin);
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
  {
    ui_pvr_extern_t *p_pvr_extern = NULL;
    u8 num = 0;
    u8 i = 0;
    num = ui_pvr_extern_get_msg_num(g_cur_play_time);
    
    OS_PRINTF("times = %d \n", num);

    for(i = 0;i < num;i ++)
    {
      p_pvr_extern = ui_pvr_extern_read(g_cur_play_time, i);
      if (p_pvr_extern != NULL)
      {
        OS_PRINTF("g_cur_play_time = %d , p_pvr_extern->rec_time = %d \n",g_cur_play_time,p_pvr_extern->rec_time);
        OS_PRINTF("p_pvr_extern->rec_time = %d , ca_msg_time = %d \n",p_pvr_extern->rec_time,ca_msg_time);
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
                  OS_PRINTF("function [%s], line [%d]\n", __FUNCTION__, __LINE__);
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

static RET_CODE on_pvr_play_bar_bar_dvrinfokey(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_mainwin = ctrl_get_root(p_ctrl);
  control_t *p_cont = NULL, *p_list = NULL, *p_bar_frm = NULL;

  // show file list
  p_bar_frm = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_FRAME);
  p_cont = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_cont, IDC_PVR_PLAY_BAR_LIST);
  
  ctrl_set_sts(p_cont, OBJ_STS_SHOW);
  ctrl_process_msg(p_bar_frm, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  list_set_focus_pos(p_list,g_p_pvr_param->focus);
  
  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_bar_cancel(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_mainwin = ctrl_get_root(p_ctrl);
  control_t *p_list_cont = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_LIST_CONT);
  control_t *p_bar_frm = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_FRAME);

  if(OBJ_STS_SHOW != ctrl_get_sts(p_bar_frm))
  {
    return SUCCESS;
  }
  
  fw_tmr_destroy(ROOT_ID_PVR_PLAY_BAR, MSG_ONE_SECOND_ARRIVE);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);

  ctrl_process_msg(p_bar_frm, MSG_LOSTFOCUS, 0, 0);

  if(OBJ_STS_SHOW != ctrl_get_sts(p_list_cont))
  {
    ctrl_set_active_ctrl(p_mainwin, NULL);
  }
  else
  {
    ctrl_process_msg(p_list_cont, MSG_GETFOCUS, 0, 0);
  }

  ctrl_paint_ctrl(p_mainwin, TRUE);
  return SUCCESS;
}

static RET_CODE on_pvr_play_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list_cont, *p_cont, *p_bar_frm;
  u16 focus = list_get_focus_pos(p_list);
  flist_t *p_rec_fav = ui_pvr_get_rec_fav_by_index(focus);
  rec_info_t *p_rec_info = NULL;
  comm_pwdlg_data_t rec_pwdlg_data =
  {
    ROOT_ID_RECORD_MANAGER,
    PVR_PLAY_BAR_PWD_DLG_FOR_CHK_X,
    PVR_PLAY_BAR_PWD_DLG_FOR_CHK_Y,
    IDS_MSG_INPUT_PASSWORD,
    pvr_play_bar_pwd_keymap,
    pvr_play_bar_pwd_proc,
    PWDLG_T_COMMON
  };

  if(focus == g_p_pvr_param->focus)
  {
    //if(is_key_error && CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
		//ui_comm_showdlg_open(NULL, IDS_ENCRYPETED_VIDEO, NULL, 3000);		
    return SUCCESS;
  }
  if( ui_pvr_get_lock_flag(focus) )
  {
    ui_comm_pwdlg_open(&rec_pwdlg_data);
    return SUCCESS;
  }
  p_rec_info = ui_pvr_get_rec_info(p_rec_fav);
  g_p_pvr_param->focus = focus;
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
    cas_manager_start_pvr_play(focus);
    #endif
  }
  #else
  ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
  #ifdef ENABLE_CA
  cas_manager_start_pvr_play(focus);
  #endif
  #endif
  

  p_list_cont = p_list->p_parent;
  p_cont = p_list_cont->p_parent;
  p_bar_frm = ctrl_get_child_by_id(p_cont, IDC_PVR_PLAY_BAR_FRAME);
  fill_pvr_play_info(p_bar_frm, p_rec_info);
  pvr_play_in_bar_back(p_bar_frm);
  pvr_play_paint_icon(p_bar_frm, IDC_PVR_PLAY_PLAY_STAT_ICON,
    IM_MP3_ICON_PLAY_2_SELECT);
  return SUCCESS;
}

static RET_CODE on_pvr_play_list_cancel(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_list_cont, *p_mainwin, *p_bar_frm;

  pvr_play_create_timer();
  p_list_cont = p_ctrl->p_parent;
  p_mainwin = p_list_cont->p_parent;
  p_bar_frm = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_FRAME);

  ctrl_set_sts(p_list_cont, OBJ_STS_HIDE);
  ctrl_process_msg(p_list_cont, MSG_LOSTFOCUS, 0, 0);
  if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_frm))
  {
    ctrl_process_msg(p_bar_frm, MSG_GETFOCUS, 0, 0);
  }
  else
  {
    ctrl_set_active_ctrl(p_mainwin, NULL);
  }

  ctrl_erase_ctrl(p_list_cont);
  return SUCCESS;
}

static RET_CODE on_pvr_play_donothing(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_pwd_close(control_t *p_ctrl, u16 msg, u32 para1,
                          u32 para2)
{
  ui_comm_pwdlg_close();
  g_bSwitchPlay = FALSE;

  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_audio_set(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  flist_t *p_rec_fav = NULL;
  rec_info_t *p_rec_info = NULL;
  pvr_audio_info_t audio_info;
  u8 loopi = 0, loopj = 0;

  memset(&audio_info, 0, sizeof(pvr_audio_info_t));
  p_rec_fav = ui_pvr_get_rec_fav_by_index(g_p_pvr_param->focus);
  p_rec_info = ui_pvr_get_rec_info(p_rec_fav);

  audio_info.record_play_id = ui_pvr_get_play_id();
  audio_info.total_size = p_rec_info->total_size;
  audio_info.total_time = p_rec_info->total_time;
  audio_info.audio_track = 0;/* 0:stereo, 1: left, 2: right, 3: mono */
  memcpy(audio_info.program_name, p_rec_info->program, sizeof(u16) * PROGRAM_NAME_MAX);
  /*if (p_rec_info->audio_cnt)
  {
    for(loopi = 0; loopi < p_rec_info->audio_cnt; loopi ++)
    {
      audio_info.audio[loopi].p_id = p_rec_info->audio[loopi].p_id;
      audio_info.audio[loopi].type = p_rec_info->audio[loopi].type;
      audio_info.audio[loopi].language_index = p_rec_info->audio[loopi].language_index;
      
      if (p_rec_info->audio[loopi].p_id == ts_player_get_cur_audio())
      {
        audio_info.audio_channel = loopi;
      }
    }
    audio_info.audio_channel_total = p_rec_info->audio_cnt;
  }
  else*/
  {
    audio_info.audio[0].p_id = p_rec_info->a_pid;
    audio_info.audio[0].type = p_rec_info->audio_fmt;
    audio_info.audio[0].language_index = 0;
    audio_info.audio_channel = 0;
    
    for(loopi = 1, loopj = 0; loopj < (p_rec_info->ext_pid_cnt); loopj++)
    {
      if (p_rec_info->ext_pid[loopj].type == EX_AUDIO_PID)
      {
        audio_info.audio[loopi].p_id = p_rec_info->ext_pid[loopj].pid;
        audio_info.audio[loopi].type = p_rec_info->ext_pid[loopj].fmt;
        audio_info.audio[loopi].language_index = 0;
        if (audio_info.audio[loopi].p_id == ts_player_get_cur_audio())
        {
          audio_info.audio_channel = loopi;
        }
        loopi ++;
      }
    }
    
    audio_info.audio_channel_total = loopi;
  }
  manage_open_menu(ROOT_ID_AUDIO_SET_RECORD, AUDIO_SET_PLAY_RECORD, (u32)(&audio_info));
  
  return SUCCESS;
}

static RET_CODE on_pvr_play_bar_pwd_correct(control_t *p_ctrl, u16 msg, u32 para1,
                          u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_list, *p_list_cont, *p_cont, *p_bar_frm;
  u16 focus;
  flist_t  *p_rec_fav = NULL;
  rec_info_t *p_rec_info = NULL;

  ui_comm_pwdlg_close();
  p_list_cont = ui_comm_root_get_ctrl(ROOT_ID_PVR_PLAY_BAR, IDC_PVR_PLAY_BAR_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PVR_PLAY_BAR_LIST);
  if(g_bSwitchPlay)
    focus = g_newfocus;
  else
    focus = list_get_focus_pos(p_list);
  g_p_pvr_param->focus = focus;
  //ui_set_have_pass_pwd(TRUE);
  p_rec_fav = ui_pvr_get_rec_fav_by_index(focus);
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
    cas_manager_start_pvr_play(focus);
    #endif
  }
  #else
  ts_player_start(flist_get_name(p_rec_fav), p_rec_info);
  #ifdef ENABLE_CA
  cas_manager_start_pvr_play(focus);
  #endif
  #endif
  

  p_cont = p_list_cont->p_parent;
  p_bar_frm = ctrl_get_child_by_id(p_cont, IDC_PVR_PLAY_BAR_FRAME);
  fill_pvr_play_info(p_bar_frm, p_rec_info);
  if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_frm))
  {
    pvr_play_create_timer();
    pvr_play_updatetime(p_cont);
    pvr_play_in_bar_back(p_bar_frm);
    pvr_play_paint_icon(p_bar_frm, IDC_PVR_PLAY_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2_SELECT);
  }
  else
    pvr_play_bar_show(p_cont);

  return ret;
}

static RET_CODE on_pvr_play_time_duration_exit(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_time_dur_cont;
  control_t *p_mainwin, *p_bar_frm;
  u32  uTotal = 0;
  utc_time_t seek_time = {0,0,0,0,0,0,0};

  p_time_dur_cont = p_ctrl->p_parent;
  ui_comm_timedit_get_time(p_time_dur_cont, &seek_time);
  uTotal = time_conver(&seek_time);

  if((msg == MSG_OK) && (uTotal >= g_total_play_time))
  {
    //ui_comm_cfmdlg_open(NULL, IDS_DURATION_TENS_THAN_CURRENT, NULL, 4000);
  }
  else
  {
    p_mainwin = p_time_dur_cont->p_parent;
    p_bar_frm = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_FRAME);
    
    //seek
    if(msg == MSG_OK)
    {
      ts_player_seek_param_t seek;
      memset(&seek, 0 ,sizeof(ts_player_seek_param_t));
      seek.seek_type = TS_PLAYER_TIME_SEEK;
      seek.seek_op = TS_PLAYER_SEEK_SET;
      seek.offset = uTotal;
      ts_player_seek(&seek);
    }

    pvr_play_create_timer();
    p_bar_frm = ctrl_get_child_by_id(p_mainwin, IDC_PVR_PLAY_BAR_FRAME);

    ctrl_set_sts(p_time_dur_cont, OBJ_STS_HIDE);
    ctrl_process_msg(p_time_dur_cont, MSG_LOSTFOCUS, 0, 0);
    if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_frm))
    {
      ctrl_set_active_ctrl(p_mainwin, p_bar_frm);
      ctrl_process_msg(p_bar_frm, MSG_GETFOCUS, 0, 0);
    }
    else
    {
      ctrl_set_active_ctrl(p_mainwin, NULL);
    }

    ctrl_erase_ctrl(p_time_dur_cont);

    pvr_play_bar_show(p_mainwin);
  }
  return SUCCESS;
}
static RET_CODE on_pvr_play_bar_donothing(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  return SUCCESS;
}


BEGIN_KEYMAP(pvr_play_bar_mainwin_keymap, NULL)
  ON_EVENT(V_KEY_STOP, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_TS_PLAY_STOP)
  ON_EVENT(V_KEY_CANCEL, MSG_TS_PLAY_STOP)
  ON_EVENT(V_KEY_EXIT, MSG_TS_PLAY_STOP)
  ON_EVENT(V_KEY_BACK, MSG_TS_PLAY_STOP)
  ON_EVENT(V_KEY_PLAY, MSG_TS_PLAY_PAUSE)
  ON_EVENT(V_KEY_PAUSE, MSG_TS_PLAY_PAUSE)
  ON_EVENT(V_KEY_OK, MSG_OK)
  ON_EVENT(V_KEY_FORW2, MSG_TS_PLAY_FF)
  ON_EVENT(V_KEY_BACK2, MSG_TS_PLAY_FB)
  ON_EVENT(V_KEY_SLOW, MSG_TS_PLAY_SLOW)
  ON_EVENT(V_KEY_PREV, MSG_TS_PLAY_PREV)
  ON_EVENT(V_KEY_NEXT, MSG_TS_PLAY_NEXT)
  ON_EVENT(V_KEY_LEFT, MSG_TS_PLAY_SEEK_B)
  ON_EVENT(V_KEY_RIGHT, MSG_TS_PLAY_SEEK_F)
  ON_EVENT(V_KEY_INFO, MSG_TS_PLAY_DVR_INFO)
  ON_EVENT(V_KEY_UP, MSG_TS_PLAY_PREV)
  ON_EVENT(V_KEY_DOWN, MSG_TS_PLAY_NEXT)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_GOTO, MSG_TS_PLAY_SEEK_TIME)
  ON_EVENT(V_KEY_VDOWN, MSG_TS_PLAY_VOL_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_TS_PLAY_VOL_UP)
  ON_EVENT(V_KEY_AUDIO, MSG_TS_PLAY_AUDIO_SET)
  ON_EVENT(V_KEY_BLUE, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_EPG, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_0, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_1, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_2, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_3, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_4, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_5, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_6, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_7, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_8, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_9, MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_TVRADIO,MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_RECALL,MSG_TS_PLAY_DO_NOTHING)
  ON_EVENT(V_KEY_FAV, MSG_TS_PLAY_DO_NOTHING)
END_KEYMAP(pvr_play_bar_mainwin_keymap, NULL)

BEGIN_MSGPROC(pvr_play_bar_mainwin_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_pvr_play_bar_mainwin_destroy)
  ON_COMMAND(MSG_TIME_UPDATE, on_pvr_play_bar_mainwin_time_update)
  ON_COMMAND(MSG_EXIT, on_pvr_play_bar_mainwin_play_end)
  ON_COMMAND(MSG_PLAY_END, on_pvr_play_bar_mainwin_play_end)
  ON_COMMAND(MSG_CANNOT_PLAY, on_pvr_play_bar_mainwin_cannot_play)
  ON_COMMAND(MSG_PLAYED, on_pvr_play_bar_mainwin_played_time)
  ON_COMMAND(MSG_PLAY_MODE_CHANGED, on_pvr_play_bar_mainwin_state_change)
  //ON_COMMAND(MSG_EXIT_TO_MENU, on_pvr_play_bar_mainwin_exit_to_menu)
  ON_COMMAND(MSG_PLUG_OUT, on_pvr_play_bar_mainwin_plug_out)
  ON_COMMAND(MSG_FOCUS_UP, on_pvr_play_donothing)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pvr_play_donothing)
  ON_COMMAND(MSG_OK, on_pvr_play_bar_mainwin_dvrinfokey)
  ON_COMMAND(MSG_TS_PLAY_PAUSE, on_pvr_play_bar_mainwin_play_pause)
  ON_COMMAND(MSG_TS_PLAY_STOP, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_FF, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_FB, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_SLOW, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_PREV, on_pvr_play_bar_mainwin_switchplay)
  ON_COMMAND(MSG_TS_PLAY_NEXT, on_pvr_play_bar_mainwin_switchplay)
  ON_COMMAND(MSG_TS_PLAY_SEEK_B, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_SEEK_F, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_NORMAL, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_DVR_INFO, on_pvr_play_bar_mainwin_dvrinfokey)
  ON_COMMAND(MSG_TS_PLAY_SEEK_TIME, on_pvr_play_bar_mainwin_seek_time)
  ON_COMMAND(MSG_TS_PLAY_VOL_DOWN, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_VOL_UP, on_pvr_play_bar_mainwin_state_change)
  ON_COMMAND(MSG_TS_PLAY_AUDIO_SET, on_pvr_play_bar_audio_set)
  ON_COMMAND(MSG_TS_PLAY_DO_NOTHING, on_pvr_play_bar_donothing)
  ON_COMMAND(MSG_PAGE_UP, on_pvr_play_bar_donothing)
  ON_COMMAND(MSG_PAGE_DOWN, on_pvr_play_bar_donothing)
END_MSGPROC(pvr_play_bar_mainwin_proc, ui_comm_root_proc)

BEGIN_KEYMAP(pvr_play_bar_bar_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
  ON_EVENT(V_KEY_OK, MSG_TS_PLAY_DVR_INFO)
  ON_EVENT(V_KEY_INFO, MSG_TS_PLAY_DVR_INFO)
END_KEYMAP(pvr_play_bar_bar_keymap, NULL)

BEGIN_MSGPROC(pvr_play_bar_bar_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_ONE_SECOND_ARRIVE, on_pvr_play_bar_bar_cancel)
  ON_COMMAND(MSG_CANCEL, on_pvr_play_bar_bar_cancel)
  ON_COMMAND(MSG_TS_PLAY_DVR_INFO, on_pvr_play_bar_bar_dvrinfokey)
END_MSGPROC(pvr_play_bar_bar_proc, ui_comm_root_proc)

BEGIN_KEYMAP(pvr_play_bar_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_INFO, MSG_TS_PLAY_DVR_INFO)
  ON_EVENT(V_KEY_GOTO, MSG_TS_PLAY_SEEK_TIME)
END_KEYMAP(pvr_play_bar_list_keymap, NULL)

BEGIN_MSGPROC(pvr_play_bar_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_pvr_play_list_select)
  ON_COMMAND(MSG_CANCEL, on_pvr_play_list_cancel)
  ON_COMMAND(MSG_TS_PLAY_DVR_INFO, on_pvr_play_donothing)
  ON_COMMAND(MSG_TS_PLAY_SEEK_TIME, on_pvr_play_donothing)  
END_MSGPROC(pvr_play_bar_list_proc, list_class_proc)

BEGIN_KEYMAP(pvr_play_bar_pwd_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)   
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)   
  ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)   
  ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)   
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(pvr_play_bar_pwd_keymap, NULL)

BEGIN_MSGPROC(pvr_play_bar_pwd_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pvr_play_bar_pwd_close)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pvr_play_bar_pwd_close)
  ON_COMMAND(MSG_PAGE_UP, on_pvr_play_bar_pwd_close)
  ON_COMMAND(MSG_PAGE_DOWN, on_pvr_play_bar_pwd_close)
  ON_COMMAND(MSG_EXIT, on_pvr_play_bar_pwd_close)
  ON_COMMAND(MSG_CORRECT_PWD, on_pvr_play_bar_pwd_correct)  
END_MSGPROC(pvr_play_bar_pwd_proc, cont_class_proc)

BEGIN_KEYMAP(pvr_play_bar_time_duration_keymap, ui_comm_tbox_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_OK)
END_KEYMAP(pvr_play_bar_time_duration_keymap, ui_comm_tbox_keymap)

BEGIN_MSGPROC(pvr_play_bar_time_duration_proc, ui_comm_time_proc)
  ON_COMMAND(MSG_EXIT, on_pvr_play_time_duration_exit)
  ON_COMMAND(MSG_OK, on_pvr_play_time_duration_exit)
END_MSGPROC(pvr_play_bar_time_duration_proc, ui_comm_time_proc)

#endif

