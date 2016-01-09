/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifdef ENABLE_PVR_REC_CONFIG
#include "ui_common.h"
#include "vfs.h"
#include "ui_pvr_rec_bar.h"
#include "ui_pause.h"
#include "file_list.h"
#include "ui_volume_usb.h"
#include "ui_small_list_pvr.h"
#include "ui_mute.h"
#ifdef ENABLE_CA
#include "cas_manager.h"
#endif
enum pvr_rec_local_msg
{
  MSG_RECORD = MSG_LOCAL_BEGIN + 350,
  //MSG_ONE_SECOND_ARRIVE,
  MSG_RECORD_PLAY_PAUSE,
  MSG_RECORD_STOP,
  MSG_EXIT_TO_MENU,
  MSG_INFO,
  MSG_DVR_INFO,
  MSG_VOL_DOWN,
  MSG_VOL_UP,
  MSG_PVR_DO_NOTHING,
  MSG_PVR_AUDIO,
  MSG_REC,
  MSG_OK,
};

enum control_id
{
  IDC_PVR_REC_FLAG_FRAME = 1,
  IDC_PVR_DUR_FRAME = 2,
  IDC_PVR_FRAME = 3,
};

enum bar_frame_sub_control_id
{
  IDC_PVR_ICON = 1,
  IDC_PVR_PLAY_STAT_ICON,
  IDC_PVR_TXT_TS,
  IDC_PVR_NAME,
  IDC_PVR_DATE,
  IDC_PVR_TIME,
  IDC_PVR_CAPACITY_ICON,
  IDC_PVR_CAPACITY_TEXT,
  IDC_PVR_RECORDING_ICON,
  IDC_PVR_STOP_ICON,
  IDC_PVR_STOP_TXT,
  IDC_PVR_PAUSE_ICON,
  IDC_PVR_PAUSE_TXT,
  IDC_PVR_PLAY_CUR_TIME,
  IDC_PVR_REC_CUR_TIME,
  IDC_PVR_PLAY_TOTAL_TIME,
  IDC_PVR_REC_TOTAL_TIME,
  IDC_PVR_PLAY_PROGRESS,
  IDC_PVR_REC_PROGRESS,
};

u16 pvr_rec_bar_cont_keymap(u16 key);
u16 pvr_rec_bar_time_duration_keymap(u16 key);
u16 pvr_rec_bar_bar_keymap(u16 key);


RET_CODE pvr_rec_bar_cont_proc(control_t *cont, u16 msg,
                       u32 para1, u32 para2);
RET_CODE pvr_rec_bar_bar_proc(control_t *cont, u16 msg,
                       u32 para1, u32 para2);
RET_CODE pvr_rec_bar_time_duration_proc(control_t *cont, u16 msg,
                       u32 para1, u32 para2);
static void on_pvr_recbar_save_exittoplay(u32 para1, u32 para2);
static RET_CODE on_pvr_recbar_onesecond(control_t *p_cont, u16 msg,
                            u32 para1, u32 para2);

static u8 epg_record = 0;
static mul_pvr_rec_attr_t rec_attr;
static u32 g_cur_total_time = 0;
static u32 g_cur_rec_time = 0;
static u32 g_time_out = 5;
static u32 g_scrambled_flag = 0;
static u8 g_bar_state = 0;
static utc_time_t total_rec_time = {0,0,0,2,0,0,0};
static u32 g_uTotal_rec_time = 0;
static u16 g_capacity = 0;
static u16 rec_focus = 0;
static u8 record_success_flag = 1;
static const u8 g_invalid_char[] = {
  0x22,//'"'
  0x2a,//'*'
  0x2f,//'/'
  0x3a,//':'
  0x3c,//'<'
  0x3e,//'>'
  0x3f,//'?'
  0x5c,//'\'
  0x7c,//'|'
  0x7f
};
static u16 rec_icon[2] =
{
  IM_INFORMATION_RECING, 0
};

u32 get_cur_rec_time(void)
{
  if(g_cur_rec_time == 0)
    return 1;
  else
    return g_cur_rec_time;
}

static u16 shift_prog_id(s16 offset)
{
  play_set_t play_set;
  dvbs_prog_node_t pg;
  u16 curn_prog,prog_id, prev_prog, total_prog;
  s32 dividend = 0;
  u8 curn_view = ui_dbase_get_pg_view_id();
  BOOL is_force_skip = FALSE;

  sys_status_get_play_set(&play_set);
  total_prog = db_dvbs_get_count(curn_view);
  prog_id = sys_status_get_curn_group_curn_prog_id();
  curn_prog = prev_prog = db_dvbs_get_view_pos_by_id(curn_view, prog_id);
  do
  {
    dividend = curn_prog + offset;
    if(dividend >= total_prog)
    {
      if(curn_prog == (total_prog - 1))
      {
        curn_prog = 0;
      }
      else
      {
        curn_prog = total_prog - 1;
      }
    }
    else
    {
      if(dividend <= 0)
      {
        if(curn_prog == 0)
        {
          curn_prog = total_prog - 1;
        }
        else
        {
          curn_prog = 0;
        }
      }
      else
      {
        curn_prog = (u16)(dividend) % total_prog;
      }
    }
    prog_id = db_dvbs_get_id_by_view_pos(curn_view, curn_prog);
    MT_ASSERT(prog_id != INVALIDID);
    if (prev_prog == curn_prog) /* all is skip*/
    {
      return curn_prog;
    }

    db_dvbs_get_pg_by_id(prog_id, &pg);
    switch(play_set.type)
    {
      case 1: // only play free pg
	      is_force_skip = (BOOL)(pg.is_scrambled == 1);
	      break;
      case 2: // only play scramble pg
	      is_force_skip = (BOOL)(pg.is_scrambled == 0);
	      break;
      default:
	      break;;
    }
  } while (is_force_skip || db_dvbs_get_mark_status(curn_view, curn_prog,
                                   DB_DVBS_MARK_SKP, FALSE));
  return prog_id;
 }

static void fill_pvr_rec_info(control_t *cont, BOOL is_redraw)
{
  control_t *p_ctrl, *p_subctrl;
  u8 asc[32] = {0};

  p_ctrl = ctrl_get_child_by_id(cont, IDC_PVR_FRAME);

  // set name
  {
    u16 prog_id = sys_status_get_curn_group_curn_prog_id();
    dvbs_prog_node_t pg;

    if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
    {
      UI_PRINTF("PROGBAR: can NOT get pg!\n");
      return;
    }
    p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_NAME);
    text_set_content_by_unistr(p_subctrl, pg.name);
  }
  // set capacity icon
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_CAPACITY_ICON);
  pbar_set_rstyle(p_subctrl, RSI_ITEM_6_HL, RSI_IGNORE, INVALID_RSTYLE_IDX);
  pbar_set_current(p_subctrl, g_capacity);

  // set capacity text
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_CAPACITY_TEXT);
  sprintf((char *)asc,"%d%%", g_capacity);
  text_set_content_by_ascstr(p_subctrl, asc);

  // set recording icon
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_RECORDING_ICON);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_REC);

#if 0
  // set stop icon
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_STOP_ICON);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_STOP);

  // set stop text
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_STOP_TXT);
  text_set_content_by_ascstr(p_subctrl, "DMP");

  // set pause icon
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PAUSE_ICON);
  bmap_set_content_by_id(p_subctrl, IM_MP3_ICON_PAUSE);

  // set pause text
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_PAUSE_TXT);
  text_set_content_by_strid(p_subctrl, IDS_PAUSE);
#endif
  // play speed
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_TXT_TS);
  text_set_content_by_ascstr(p_subctrl, (u8 *)"TS");

  //rec curl time
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_REC_CUR_TIME);
  sprintf((char *)asc, "%.2d:%.2d:%.2d",
    (int)g_cur_rec_time/3600, (int)(g_cur_rec_time%3600)/60, (int)g_cur_rec_time%60);
  text_set_content_by_ascstr(p_subctrl, asc);

  //rec total time
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_PVR_REC_TOTAL_TIME);
  sprintf((char *)asc, "%02d:%02d:%02d", total_rec_time.hour, total_rec_time.minute, total_rec_time.second);
  text_set_content_by_ascstr(p_subctrl, asc);

  // update view
  if(is_redraw)
  {
     ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

static void pvr_rec_fill_time_info(control_t *cont, BOOL is_redraw)
{
  utc_time_t cur_time;
  control_t *p_frm = NULL, *p_ctrl = NULL;
  u8 time_str[32];

  p_frm = ctrl_get_child_by_id(cont, IDC_PVR_FRAME);
  MT_ASSERT(p_frm != NULL);

  time_get(&cur_time, FALSE);

  sprintf((char *)time_str, "%.4d/%.2d/%.2d", cur_time.year, cur_time.month, cur_time.day);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_PVR_DATE);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  sprintf((char *)time_str, "%.2d:%.2d", cur_time.hour, cur_time.minute);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_PVR_TIME);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

#if 0
BOOL print_invalid_char(void)
{
  hfile_t file_handle = 0;
  u8 j;
  u8 filename[PROGRAM_NAME_MAX+1];

  for(j = 0x1; j < 0x80; j++ )
  {
    sprintf(filename, "C:\\%s\\ .ts",
      PVR_DIR);
    filename[8] = j;
    file_handle = vfs_open(filename, VFS_NEW);
    if(file_handle == 0)
    {
      OS_PRINTF("invalid char=%x,%d,%c\n",j,j,j);
    }
    else
      vfs_close(file_handle);
  }
  MT_ASSERT(0);
}
#endif

static BOOL str_isvalidchar(u16 src)
{
  u32 i = 0;
  
  if(src < 0x20)
    return FALSE;
  
  for(i = 0;i < sizeof(g_invalid_char); i ++)
  {
    if(src == g_invalid_char[i])
    {
      OS_PRINTF("invalid char=%x,%d,%c\n",src,src,src);
      return FALSE;
    }
  }

  return TRUE;
}
//lint -e613
static u32 str_uni_filter(u16 *p_dest, const u16 *p_src)
{
  u32 i = 0,j = 0;

  MT_ASSERT(p_dest != NULL && p_src != NULL);

  while(p_src[i] != 0)
  {
    if(str_isvalidchar(p_src[i]))
    {
      p_dest[j] = p_src[i];
      j ++;
    }
    i++;
  }
  p_dest[j] = 0;

  return j;
}
//lint +e613

//lint -e539
RET_CODE open_pvr_rec_bar(u32 para1, u32 para2)
{
  control_t *p_cont;
  u32 j = 0;
  u8 separator[TBOX_MAX_ITEM_NUM] = {'-', '-', ' ', ':', ' ', ' '};
  u16 filename[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
  u8 asc_tmp[64] = {0};
  u16 uni_tmp[64] = {0};
  dvbs_prog_node_t prog;
  dvbs_tp_node_t tp;
  u16 pg_id, total;
  osd_set_t osd_set;  
  utc_time_t gmt_time;
  u8 i = 0;
  hfile_t file = NULL;
#if 1//#ifndef SPT_SUPPORT
  control_t *p_ctrl, *p_subctrl, *p_record_progress, *p_time_ctrl;
  partition_t *p_partition = NULL;
  //check if can record
  memset(&rec_attr, 0, sizeof(mul_pvr_rec_attr_t));
  epg_record = (u8)para2;
  if(para2 != 0)
  {
  memcpy(&total_rec_time, (u32 *)para1, sizeof(utc_time_t));
  }
  else
  {
  memset(&total_rec_time, 0, sizeof(utc_time_t));
  total_rec_time.hour = 2;
  }
  g_bar_state = 1;
  g_cur_rec_time = 0;
  g_cur_total_time = 0;
  sys_status_get_osd_set(&osd_set);
  g_time_out = osd_set.timeout;

  

  //create ui container
  p_cont = fw_create_mainwin(ROOT_ID_PVR_REC_BAR,
                           PVR_REC_BAR_CONT_X,
                           PVR_REC_BAR_CONT_Y,
                           PVR_REC_BAR_CONT_W,
                           PVR_REC_BAR_CONT_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE,
                           0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, pvr_rec_bar_cont_keymap);
  ctrl_set_proc(p_cont, pvr_rec_bar_cont_proc);

  //rec flag
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_REC_FLAG_FRAME,
          PVR_REC_FLAG_FRM_X, PVR_REC_FLAG_FRM_Y,
          PVR_REC_FLAG_FRM_W, PVR_REC_FLAG_FRM_H,
          p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  bmap_set_content_by_id(p_ctrl, IM_INFORMATION_RECING);
  // animation
  rec_focus = 0;
  fw_tmr_create(ROOT_ID_PVR_REC_BAR, MSG_REC, 1 * SECOND, TRUE);
  
  //duration
  p_time_ctrl = ui_comm_timedit_create(p_cont, IDC_PVR_DUR_FRAME,
          PVR_REC_DUR_TIME_X + 40  , PVR_REC_DUR_TIME_Y, PVR_REC_DUR_TIME_LW, PVR_REC_DUR_TIME_RW);
  ui_comm_ctrl_set_keymap(p_time_ctrl, pvr_rec_bar_time_duration_keymap);
  ui_comm_ctrl_set_proc(p_time_ctrl, pvr_rec_bar_time_duration_proc);
  ui_comm_timedit_set_static_txt(p_time_ctrl, IDS_DURATION);
  ui_comm_timedit_set_time(p_time_ctrl, &total_rec_time);
  ui_comm_timedit_set_param(p_time_ctrl, 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, TBOX_SEPARATOR_TYPE_UNICODE, 18);
  for(j = 0; j < TBOX_MAX_ITEM_NUM; j++)
  {
    ui_comm_timedit_set_separator_by_ascchar(p_time_ctrl, (u8)j, separator[j]);
  }
  ctrl_set_sts(p_time_ctrl, OBJ_STS_HIDE);

  // frame part
  p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_PVR_FRAME,
                             PVR_REC_BAR_FRM_X, PVR_REC_BAR_FRM_Y,
                             PVR_REC_BAR_FRM_W,PVR_REC_BAR_FRM_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PVR_REC_BAR_FRM,
                  RSI_PVR_REC_BAR_FRM, RSI_PVR_REC_BAR_FRM);
  ctrl_set_keymap(p_ctrl, pvr_rec_bar_bar_keymap);
  ctrl_set_proc(p_ctrl, pvr_rec_bar_bar_proc);

  // PVR icon
  #if 0
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_ICON,
    PVR_REC_BAR_ICON_X, PVR_REC_BAR_ICON_Y,
    PVR_REC_BAR_ICON_W, PVR_REC_BAR_ICON_H,
    p_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_PVR);
#endif
  //  TS
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_TXT_TS,
                            PVR_REC_BAR_PLAY_SPEED_X, PVR_REC_BAR_PLAY_SPEED_Y,
                            PVR_REC_BAR_PLAY_SPEED_W, PVR_REC_BAR_PLAY_SPEED_H,
                            p_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_INFO,
                      FSI_PVR_REC_BAR_INFO, FSI_PVR_REC_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // name
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_NAME,
                            PVR_REC_BAR_NAME_TXT_X, PVR_REC_BAR_NAME_TXT_Y,
                            PVR_REC_BAR_NAME_TXT_W, PVR_REC_BAR_NAME_TXT_H,
                            p_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_NAME,
                      FSI_PVR_REC_BAR_NAME, FSI_PVR_REC_BAR_NAME );
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // date & time
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    PVR_REC_BAR_DATE_ICON_X, PVR_REC_BAR_DATE_ICON_Y,
    PVR_REC_BAR_DATE_ICON_W, PVR_REC_BAR_DATE_ICON_H,
    p_ctrl, 0);
    bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_DATE,
                            PVR_REC_BAR_DATE_TXT_X, PVR_REC_BAR_DATE_TXT_Y,
                            PVR_REC_BAR_DATE_TXT_W, PVR_REC_BAR_DATE_TXT_H,
                            p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_DATE,
                      FSI_PVR_REC_BAR_DATE,FSI_PVR_REC_BAR_DATE);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    PVR_REC_BAR_TIME_ICON_X, PVR_REC_BAR_TIME_ICON_Y,
    PVR_REC_BAR_TIME_ICON_W, PVR_REC_BAR_TIME_ICON_H,
    p_ctrl, 0);
  //bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_TIME,
                            PVR_REC_BAR_TIME_TXT_X, PVR_REC_BAR_TIME_TXT_Y,
                            PVR_REC_BAR_TIME_TXT_W, PVR_REC_BAR_TIME_TXT_H,
                            p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_DATE,
                      FSI_PVR_REC_BAR_DATE,FSI_PVR_REC_BAR_DATE);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // capacity icon
  p_subctrl = ctrl_create_ctrl(CTRL_PBAR, IDC_PVR_CAPACITY_ICON,
                     PVR_REC_BAR_CAPACITY_ICON_X, PVR_REC_BAR_CAPACITY_ICON_Y,
                     PVR_REC_BAR_CAPACITY_ICON_W, PVR_REC_BAR_CAPACITY_ICON_H,
                     p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  ctrl_set_mrect(p_subctrl, PVR_REC_BAR_CAPACITY_ICON_MIDX, PVR_REC_BAR_CAPACITY_ICON_MIDY,
    PVR_REC_BAR_CAPACITY_ICON_MIDW, PVR_REC_BAR_CAPACITY_ICON_MIDH);
  pbar_set_direction(p_subctrl, 0);
  pbar_set_count(p_subctrl, 0, 100, 100);
  pbar_set_current(p_subctrl, 0);

  // recording icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_RECORDING_ICON,
                     PVR_REC_BAR_RECORDING_ICON_X, PVR_REC_BAR_RECORDING_ICON_Y,
                     PVR_REC_BAR_RECORDING_ICON_W, PVR_REC_BAR_RECORDING_ICON_H,
                     p_ctrl, 0);

#if 0
  // stop icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_STOP_ICON,
                     PVR_REC_BAR_STOP_ICON_X, PVR_REC_BAR_STOP_ICON_Y,
                     PVR_REC_BAR_STOP_ICON_W, PVR_REC_BAR_STOP_ICON_H,
                     p_ctrl, 0);

  // stop txt
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_STOP_TXT,
                     PVR_REC_BAR_STOP_TXT_X, PVR_REC_BAR_STOP_TXT_Y,
                     PVR_REC_BAR_STOP_TXT_W, PVR_REC_BAR_STOP_TXT_H,
                     p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_INFO,
                      FSI_PVR_REC_BAR_INFO, FSI_PVR_REC_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // pause icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PVR_PAUSE_ICON,
                     PVR_REC_BAR_PAUSE_ICON_X, PVR_REC_BAR_PAUSE_ICON_Y,
                     PVR_REC_BAR_PAUSE_ICON_W, PVR_REC_BAR_PAUSE_ICON_H,
                     p_ctrl, 0);

  // pause txt
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_PAUSE_TXT,
                     PVR_REC_BAR_PAUSE_TXT_X, PVR_REC_BAR_PAUSE_TXT_Y,
                     PVR_REC_BAR_PAUSE_TXT_W, PVR_REC_BAR_PAUSE_TXT_H,
                     p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_INFO,
                      FSI_PVR_REC_BAR_INFO, FSI_PVR_REC_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
#endif
  //rec cur time
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_REC_CUR_TIME,
                            PVR_REC_BAR_REC_CUR_TIME_X, PVR_REC_BAR_REC_CUR_TIME_Y,
                            PVR_REC_BAR_REC_CUR_TIME_W, PVR_REC_BAR_REC_CUR_TIME_H,
                            p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_INFO,
                      FSI_PVR_REC_BAR_INFO, FSI_PVR_REC_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //rec total time
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_REC_TOTAL_TIME,
                            PVR_REC_BAR_REC_TOTAL_TIME_X, PVR_REC_BAR_REC_TOTAL_TIME_Y,
                            PVR_REC_BAR_REC_TOTAL_TIME_W, PVR_REC_BAR_REC_TOTAL_TIME_H,
                            p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_INFO,
                      FSI_PVR_REC_BAR_INFO, FSI_PVR_REC_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //capacity text
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PVR_CAPACITY_TEXT,
                            PVR_REC_BAR_CAPACITY_TEXT_X, PVR_REC_BAR_CAPACITY_TEXT_Y,
                            PVR_REC_BAR_CAPACITY_TEXT_W, PVR_REC_BAR_CAPACITY_TEXT_H,
                            p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PVR_REC_BAR_INFO,
                      FSI_PVR_REC_BAR_INFO, FSI_PVR_REC_BAR_INFO);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //record progress
  p_record_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_PVR_REC_PROGRESS,
    PVR_REC_BAR_REC_PROGRESS_X, PVR_REC_BAR_REC_PROGRESS_Y,
    PVR_REC_BAR_REC_PROGRESS_W, PVR_REC_BAR_REC_PROGRESS_H,
    p_ctrl, 0);
  ctrl_set_rstyle(p_record_progress, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_record_progress,
    PVR_REC_BAR_REC_PROGRESS_MIDX, PVR_REC_BAR_REC_PROGRESS_MIDY,
    PVR_REC_BAR_REC_PROGRESS_MIDW, PVR_REC_BAR_REC_PROGRESS_MIDH);
  pbar_set_rstyle(p_record_progress, PVR_REC_BAR_REC_PROGRESS_MIN, PVR_REC_BAR_REC_PROGRESS_MAX, PVR_REC_BAR_REC_PROGRESS_MID);
  pbar_set_count(p_record_progress, 0, 100, 100);
  pbar_set_direction(p_record_progress, 1);
  pbar_set_workmode(p_record_progress, 0, 0);
  pbar_set_current(p_record_progress, 0);

#else
  /* create */
  p_cont = spt_load_menu(ROOT_ID_PVR_REC_BAR);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, pvr_rec_bar_cont_keymap);
  ctrl_set_proc(p_cont, pvr_rec_bar_cont_proc);
#endif
  ui_recorder_up_devinfo();
  g_capacity = ui_pvr_get_capacity();
  pvr_rec_fill_time_info(p_cont, TRUE);
  fill_pvr_rec_info(p_cont, TRUE);
  //set bar active
  ctrl_process_msg(p_ctrl, MSG_GETFOCUS, 0, 0);

  ui_enable_signal_monitor(TRUE);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  pg_id = sys_status_get_curn_group_curn_prog_id();
  total = db_dvbs_get_count(ui_dbase_get_pg_view_id());

  if ((pg_id == INVALIDID) || (total == 0))
  {
    return ERR_FAILURE;
  }

  db_dvbs_get_pg_by_id(pg_id, &prog);
  db_dvbs_get_tp_by_id((u16)prog.tp_id, &tp);

  memset(&rec_attr, 0, sizeof(mul_pvr_rec_attr_t));
  rec_attr.media_info.v_pid = (u16)prog.video_pid;
  rec_attr.media_info.video_type = (vdec_src_fmt_t)prog.video_type;
  rec_attr.media_info.a_pid = prog.audio[prog.audio_channel].p_id;
  rec_attr.media_info.audio_type = (adec_src_fmt_vsb_t)prog.audio[prog.audio_channel].type;
  rec_attr.media_info.pcr_pid = (u16)prog.pcr_pid;
  rec_attr.b_clear_stream = !prog.is_scrambled;
  rec_attr.media_info.extern_num = 2;
  rec_attr.media_info.extern_pid[0].pid = DVB_PAT_PID;
  rec_attr.media_info.extern_pid[0].type = EX_SI_PID;
  rec_attr.media_info.extern_pid[1].pid = DVB_SDT_PID;
  rec_attr.media_info.extern_pid[1].type = EX_SI_PID;
  rec_attr.stream_type = MUL_PVR_STREAM_TYPE_TS;
#ifdef ENABLE_CA
  if((CAS_ID == CONFIG_CAS_ID_ABV) && prog.is_scrambled)
  {
    u16 ts_emm_id;
    u16 audio_ecm_pid;
    u16 video_ecm_pid;
    RET_CODE ret = cas_manager_get_current_program_ecm_emm_info(&ts_emm_id, &audio_ecm_pid, &video_ecm_pid);

    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = DVB_CAT_PID;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
    rec_attr.media_info.extern_num++;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = prog.s_id;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
    rec_attr.media_info.extern_num++;
#ifdef ENABLE_PVR_SCRAMBLE_TS
    rec_attr.stream_type = MUL_PVR_STREAM_TYPE_SCRAMBLE_TS;
#endif
    if(ret == SUCCESS)
    {
      if((ts_emm_id != 0xFFFF) && (ts_emm_id != 0x1FFF))
      {
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = ts_emm_id;
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
        rec_attr.media_info.extern_num++;
      }

      if((video_ecm_pid != 0xFFFF) && (video_ecm_pid != 0x1FFF))
      {
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = video_ecm_pid;
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
        rec_attr.media_info.extern_num++;
      }

      if((audio_ecm_pid != 0xFFFF) && (audio_ecm_pid != 0x1FFF)&&(audio_ecm_pid != video_ecm_pid))
      {
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = audio_ecm_pid;
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
        rec_attr.media_info.extern_num++;
      }
	}
  }
  else  if((CUSTOMER_ID == CUSTOMER_AISAT_DEMO )&&(CAS_ID == CONFIG_CAS_ID_ONLY_1)
	  	&& prog.is_scrambled)
  {
    u16 ts_emm_id;
    u16 audio_ecm_pid;
    u16 video_ecm_pid;
    RET_CODE ret = cas_manager_get_current_program_ecm_emm_info(&ts_emm_id, &audio_ecm_pid, &video_ecm_pid);

    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = DVB_CAT_PID;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
    rec_attr.media_info.extern_num++;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = prog.s_id;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
    rec_attr.media_info.extern_num++;
#ifdef ENABLE_PVR_SCRAMBLE_TS
    rec_attr.stream_type = MUL_PVR_STREAM_TYPE_SCRAMBLE_TS;
#endif
    if(ret == SUCCESS)
    {
      if((ts_emm_id != 0xFFFF) && (ts_emm_id != 0x1FFF))
      {
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = ts_emm_id;
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
        rec_attr.media_info.extern_num++;
      }

      if((video_ecm_pid != 0xFFFF) && (video_ecm_pid != 0x1FFF))
      {
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = video_ecm_pid;
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
        rec_attr.media_info.extern_num++;
      }

      if((audio_ecm_pid != 0xFFFF) && (audio_ecm_pid != 0x1FFF)&&(audio_ecm_pid != video_ecm_pid))
      {
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = audio_ecm_pid;
        rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
        rec_attr.media_info.extern_num++;
      }
	}
  }
#ifndef WIN32
  else  if((CUSTOMER_ID == CUSTOMER_KINGVON )&&(CAS_ID == CONFIG_CAS_ID_CRYPG)
	  	&& prog.is_scrambled)
  {
	rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = DVB_CAT_PID;
	rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_SI_PID;
	rec_attr.media_info.extern_num++;

	u16 p_ecm_pid;
	u16 p_emm_pid;
	RET_CODE ret = cas_manager_get_current_program_ecm_emm(&p_ecm_pid, &p_emm_pid);

	if(ret == SUCCESS)
	{
		if(p_emm_pid != 0)
		{
			rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = p_emm_pid;
			rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_UERS_PID_1;
			rec_attr.media_info.extern_num++;
		}
		if(p_ecm_pid != 0)
		{
			rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = p_ecm_pid;
			rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_UERS_PID_2;
			rec_attr.media_info.extern_num++;
		}
	}
  }
#endif
#endif
  for(j = 0; j < prog.audio_ch_num; j++)
  {
    if(j == prog.audio_channel)
      continue;
    if(rec_attr.media_info.extern_num >= REC_MAX_EXTERN_PID)
    {
      OS_PRINTF("%s, more audio\n", __FUNCTION__);
      break;
    }
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].pid = prog.audio[j].p_id;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].type = EX_AUDIO_PID;
    rec_attr.media_info.extern_pid[rec_attr.media_info.extern_num].fmt = prog.audio[j].type;
    rec_attr.media_info.extern_num ++;
  }
  rec_attr.media_info.pmt_pid = prog.pmt_pid;

  if (prog.is_scrambled)
  {
    rec_attr.key_cfg.is_biss_key = 0;
    if(prog.ca_system_id == 0x2600)
    {
    }
  }
  else
  {
    rec_attr.key_cfg.is_biss_key = 0;
  }
  g_uTotal_rec_time = time_conver(&total_rec_time);
  g_scrambled_flag = prog.is_scrambled;
  
  OS_PRINTF("\n##debug: 0x%x 0x%x 0x%x!\n",
    rec_attr.media_info.v_pid, rec_attr.media_info.a_pid, rec_attr.media_info.pcr_pid);
  time_get(&gmt_time, TRUE);
  time_to_local(&gmt_time, &(rec_attr.start));

  {
    u16 prog_name[DB_DVBS_MAX_NAME_LENGTH + 1];
    str_uni_filter(prog_name, prog.name);
    memcpy(filename, prog_name, (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16));
    memcpy(rec_attr.program, prog.name, DB_DVBS_MAX_NAME_LENGTH * sizeof(u16));
  }

  //set rec_attr.program
  if(0)
  {
      i = 0;
      while(prog.name[i] != 0)
      {
        rec_attr.program[i] = (u8)prog.name[i];
        i++;
      }
      rec_attr.program[i] = 0;
  }
    
  if (file_list_get_partition(&p_partition) > 0)
  {
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    {
      if((u64)p_partition->free_size < 30 * 1024) //30M
      {
        ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 2000);
        return SUCCESS;
      }
    }
#endif    
    memset(rec_attr.file_name, 0, sizeof(u16) * PVR_MAX_FILENAME_LEN);
    
    uni_strcat(rec_attr.file_name, 
      p_partition[sys_status_get_usb_work_partition()].letter, PVR_MAX_FILENAME_LEN);

    str_asc2uni((u8 *)"\\", uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

    
    str_asc2uni((u8 *)PVR_DIR, uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

    str_asc2uni((u8 *)"\\", uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);    
    

    uni_strcat(rec_attr.file_name, filename, PVR_MAX_FILENAME_LEN);    
      
    sprintf((char *)asc_tmp, "-%.4d-%.2d-%.2d %.2d-%.2d-%.2d.ts",
        rec_attr.start.year,
        rec_attr.start.month,
        rec_attr.start.day,
        rec_attr.start.hour,
        rec_attr.start.minute,
        rec_attr.start.second);
        
    str_asc2uni(asc_tmp, uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

    ui_recorder_init();
    g_capacity = ui_pvr_get_capacity();
    file=vfs_open(rec_attr.file_name, VFS_NEW);
    if(0 == file)
    {
      memset(rec_attr.file_name, 0, sizeof(u16) * PVR_MAX_FILENAME_LEN);
      uni_strcat(rec_attr.file_name, 
        p_partition[sys_status_get_usb_work_partition()].letter, PVR_MAX_FILENAME_LEN);

      str_asc2uni((u8 *)"\\", uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
    
      str_asc2uni((u8 *)PVR_DIR, uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
      
      str_asc2uni((u8 *)"\\", uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
      
      str_asc2uni((u8 *)"No_Name", uni_tmp);

      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);    
        
      sprintf((char *)asc_tmp, "-%.4d-%.2d-%.2d %.2d-%.2d-%.2d.ts",
          rec_attr.start.year,
          rec_attr.start.month,
          rec_attr.start.day,
          rec_attr.start.hour,
          rec_attr.start.minute,
          rec_attr.start.second);

      str_asc2uni(asc_tmp, uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
    }
    else
    {
      vfs_close(file);
    }
    rec_attr.file_name_len = uni_strlen(rec_attr.file_name);
    rec_attr.max_file_size = (u64)p_partition[sys_status_get_usb_work_partition()].free_size * (u64)KBYTES;
    OS_PRINTF("*****free size = %lld\n",rec_attr.max_file_size);
  }
  else
  {
    //no available storage found
    ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 2000);
    return SUCCESS;
  }

  if(ui_recorder_start(&rec_attr) == ERR_FAILURE)
  {
    ui_recorder_end();
    OS_PRINTF("\n##ui_recorder_stop [%s, %d]\n", __FUNCTION__, __LINE__);
    ui_recorder_stop();
    ui_recorder_release();
    record_success_flag=0;
    
    return ERR_FAILURE;
  }
  record_success_flag=1;
  if(ui_is_mute())
    open_mute(0, 0); //set mute
  //fw_tmr_create(ROOT_ID_PVR_REC_BAR, MSG_ONE_SECOND_ARRIVE, 1000, TRUE);
  return SUCCESS;
}
//lint +e539

static RET_CODE on_time_update(control_t *p_cont, u16 msg,
                            u32 para1, u32 para2)
{
  /*control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_PVR_FRAME);
  if(!ctrl_is_onshow(p_frm))
    return SUCCESS;*/
  if(g_bar_state)
    pvr_rec_fill_time_info(p_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_pbar_destory(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  //fw_tmr_destroy(ROOT_ID_PVR_REC_BAR, MSG_ONE_SECOND_ARRIVE);
  OS_PRINTF("%s,g_scrambled_flag=%d\n",__FUNCTION__,g_scrambled_flag);
  if(!record_success_flag)
    return ERR_NOFEATURE;

  ui_recorder_end();
  if(ui_recorder_isrecording())
  {
    OS_PRINTF("\n##ui_recorder_stop [%s, %d]\n", __FUNCTION__, __LINE__);
    ui_recorder_stop();
  }

  MT_ASSERT(ui_recorder_release() == SUCCESS);

  if(ui_is_pause())
  {
    ui_set_pause(FALSE);
    avc_set_mute_1(class_get_handle_by_id(AVC_CLASS_ID),
               FALSE, TRUE);
  }
  return ERR_NOFEATURE;
}

static RET_CODE on_pvr_recbar_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  OS_PRINTF("\n##ui_recorder_stop [%s, %d]\n", __FUNCTION__, __LINE__);
  ui_recorder_stop();
  on_pvr_recbar_save_exittoplay(0,0);
  
  return SUCCESS;
}

static RET_CODE on_pvr_rec_bar_bar_dvrinfokey(control_t *p_cont, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_mainwin = ctrl_get_root(p_cont);
  if(ctrl_is_onshow(p_cont))
  {
    control_t *p_ctrl;

    p_ctrl = ctrl_get_child_by_id(p_mainwin, IDC_PVR_DUR_FRAME);

    ctrl_process_msg(p_cont, MSG_LOSTFOCUS, 0, 0);
    ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_ctrl, OBJ_STS_SHOW);
    ctrl_process_msg(p_ctrl, MSG_GETFOCUS, 0, 0);
    ui_comm_timedit_enter_edit(p_ctrl);
    ctrl_paint_ctrl(p_mainwin, TRUE);
  }
  else
  {
    g_bar_state = 1;
    ctrl_set_attr(p_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_cont, OBJ_STS_SHOW);
    fill_pvr_rec_info(p_mainwin, FALSE);
    ctrl_paint_ctrl(p_cont, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_pvr_recbar_rec_pause(control_t *p_cont, u16 msg,
                            u32 para1, u32 para2)
{
  BOOL  is_pause = ui_is_pause();
  
  ui_set_pause((BOOL)!is_pause);
  avc_set_mute_1(class_get_handle_by_id(AVC_CLASS_ID),
               (BOOL)!is_pause, TRUE);

  return SUCCESS;
}

static RET_CODE on_pvr_recbar_onesecond(control_t *p_cont, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_pvr_frm, *p_cur_rec_time, *p_record_progress, *p_subctrl;
  char asc[32] = {0};
  static u16 percent = 0;
  u16 capacity = 0;
  u16 temp = 0;
  u32 cur_time = g_cur_rec_time + g_cur_total_time;
  static u8 time_count = 0;

  if(!g_bar_state)
    return SUCCESS;
  if(cur_time >= g_uTotal_rec_time)
    return SUCCESS;

  p_pvr_frm = ctrl_get_child_by_id(p_cont, IDC_PVR_FRAME);
  if(!ctrl_is_onshow(p_pvr_frm))
    return SUCCESS;
  time_count ++;
  if(time_count > g_time_out)//auto hide when time out
  {
    g_bar_state = 0;
    time_count = 0;
    ctrl_set_sts(p_pvr_frm, OBJ_STS_HIDE);
    ctrl_paint_ctrl(p_cont, TRUE);
    return SUCCESS;
  }

  p_cur_rec_time = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_REC_CUR_TIME);
  p_record_progress = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_REC_PROGRESS);

  sprintf((char *)asc, "%.2d:%.2d:%.2d",
    (int)cur_time/3600, (int)(cur_time%3600)/60, (int)cur_time%60);
  text_set_content_by_ascstr(p_cur_rec_time, (u8 *)asc);

  temp = percent;
  percent = (u16)(cur_time*100/g_uTotal_rec_time);
  pbar_set_current(p_record_progress, percent);
  if (temp != percent)
    ctrl_paint_ctrl(p_record_progress, TRUE);

  capacity = ui_pvr_get_capacity();
  if ((g_capacity < capacity))
  {
    // set capacity text
    g_capacity = capacity;
    p_subctrl = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_CAPACITY_TEXT);
    sprintf((char *)asc, "%2d%%", g_capacity);
    text_set_content_by_ascstr(p_subctrl, (u8 *)asc);
    ctrl_paint_ctrl(p_subctrl, TRUE);
    p_subctrl = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_CAPACITY_ICON);
    pbar_set_rstyle(p_subctrl, RSI_ITEM_6_HL, RSI_IGNORE, INVALID_RSTYLE_IDX);
    pbar_set_current(p_subctrl, g_capacity);
    ctrl_paint_ctrl(p_subctrl, TRUE);
    OS_PRINTF("\nup capacity[%d]\n",capacity);
  }

  ctrl_paint_ctrl(p_cur_rec_time, TRUE);

  return SUCCESS;
}

static RET_CODE on_pvr_recbar_time_duration_exit(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_time_dur_cont, *p_frm, *p_pvr_frm, *p_total_rec_time;
  char asc[32];
  u32  uTotal = 0;

  p_time_dur_cont = p_ctrl->p_parent;
  p_frm = p_time_dur_cont->p_parent;
  p_pvr_frm = ctrl_get_child_by_id(p_frm, IDC_PVR_FRAME);
  p_total_rec_time = ctrl_get_child_by_id(p_pvr_frm, IDC_PVR_REC_TOTAL_TIME);

  ui_comm_timedit_get_time(p_time_dur_cont, &total_rec_time);
  uTotal = time_conver(&total_rec_time);

  if(uTotal <= g_cur_rec_time + 10)
  {
  	g_bar_state = 0;
    ui_comm_cfmdlg_open(NULL, IDS_DURATION_TENS_THAN_CURRENT, NULL, 4000);
  }
  else
  {
    ctrl_process_msg(p_time_dur_cont, MSG_LOSTFOCUS, 0, 0);
    ctrl_set_sts(p_time_dur_cont, OBJ_STS_HIDE);
    ctrl_set_attr(p_pvr_frm, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_pvr_frm, OBJ_STS_SHOW);

    ctrl_process_msg(p_pvr_frm, MSG_GETFOCUS, 0, 0);

    sprintf((char *)asc, "%02d:%02d:%02d", total_rec_time.hour, total_rec_time.minute, total_rec_time.second);
    text_set_content_by_ascstr(p_total_rec_time, (u8 *)asc);

    ctrl_paint_ctrl(p_frm, TRUE);
    g_uTotal_rec_time = uTotal;
	g_bar_state = 1;
  }
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_time_duration_dvrinfokey(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  return SUCCESS;
}

static void on_pvr_recbar_save_exittomenu(void)
{
  fw_tmr_destroy(ROOT_ID_PVR_REC_BAR, MSG_REC);
  ui_close_all_mennus();
  manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_MAINMENU, 0, 0);
}

static void on_pvr_recbar_save_exittoplay(u32 para1, u32 para2)
{
  ui_close_all_mennus();
  //fw_destroy_all_mainwin(FALSE);
  if(ui_is_mute())
  {
    open_mute(0, 0); //set mute
  }
  manage_open_menu(ROOT_ID_PROG_BAR, para1, para2);
  fw_paint_all_mainwin();
}

static RET_CODE on_focus_change(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
	u16 cur_pg_id,next_pg_id;
	s32 offset = 0;
	dvbs_prog_node_t cur_pg, next_pg;
   control_t *p_pvr_frm, *p_cont;

	cur_pg_id = sys_status_get_curn_group_curn_prog_id();
	db_dvbs_get_pg_by_id(cur_pg_id, &cur_pg);

   p_cont = ctrl_get_root(p_ctrl);
   p_pvr_frm = ctrl_get_child_by_id(p_cont, IDC_PVR_FRAME);

  switch(msg)
  {
    case MSG_FOCUS_UP:
      offset = 1;
      break;
    case MSG_FOCUS_DOWN:
      offset = -1;
      break;
    case MSG_PAGE_UP:
      offset = 8;
      break;
    case MSG_PAGE_DOWN:
      offset = -8;
      break;
    default:
      break;
  }
  next_pg_id = shift_prog_id(offset);
  db_dvbs_get_pg_by_id(next_pg_id, &next_pg);

  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
  {
    if(next_pg.tp_id == cur_pg.tp_id)
    {
       g_bar_state = 0; //if full_screen_info,rec_info_bar time shoule no show
  	 ctrl_set_sts(p_pvr_frm, OBJ_STS_HIDE);
       ctrl_paint_ctrl(p_cont, TRUE);
       
       ui_play_prog(next_pg_id, FALSE);
       manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);

       return SUCCESS;
    }
  }
  ui_comm_ask_for_dodlg_open_ex(NULL, IDS_EXIT_RECORD,\
                                  on_pvr_recbar_save_exittoplay, para1, para2, 0);
  return SUCCESS;
}

static RET_CODE on_pvr_rec_bar_hide(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  if(ctrl_is_onshow(p_ctrl))
  {
    g_bar_state = 0;
    ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
    ctrl_paint_ctrl(ctrl_get_root(p_ctrl), TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_stop(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  control_t *p_bar_frm = ctrl_get_child_by_id(p_ctrl, IDC_PVR_FRAME);
  if(ctrl_is_onshow(p_bar_frm))
  {  
    on_pvr_rec_bar_hide(p_bar_frm, msg, 0, 0);
  }
  else
  {
    ui_comm_ask_for_dodlg_open_ex(NULL, IDS_EXIT_RECORD,
      on_pvr_recbar_save_exittoplay, para1, para2, 0);
  }

  return SUCCESS;
}

static RET_CODE on_pvr_recbar_rec_stopped(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  g_cur_total_time += g_cur_rec_time;
  ui_recorder_up_devinfo();
  g_cur_rec_time = 0;
  OS_PRINTF("\n## MUL_PVR_EVENT_REC_RESV, g_cur_rec_index=%d, [%lu]\n",
      para1, g_cur_total_time);
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_mem_not_enough(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  OS_PRINTF("\n##ui_recorder_stop [%s, %d]\n", __FUNCTION__, __LINE__);
  ui_recorder_stop();
  ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 2000);
  on_pvr_recbar_save_exittoplay(0,0);
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_write_error(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)
{
  OS_PRINTF("\n##ui_recorder_stop [%s, %d]\n", __FUNCTION__, __LINE__);
  ui_recorder_stop();
  ui_comm_cfmdlg_open(NULL, IDS_USB_ERROR, NULL, 0);
  on_pvr_recbar_save_exittoplay(0,0);
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_no_signal(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_updated(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  g_cur_rec_time = para1 / 1000;
//  OS_PRINTF("%s,%d,g_cur_rec_time=%d\n",__FUNCTION__, __LINE__,g_cur_rec_time);

  if(g_cur_rec_time + g_cur_total_time >= g_uTotal_rec_time)
  {
    OS_PRINTF("\n##ui pvr rec time end![%lu, %lu, %lu]\n",
      g_cur_rec_time, g_cur_total_time, g_uTotal_rec_time);
    OS_PRINTF("\n##ui_recorder_stop [%s, %d]\n", __FUNCTION__, __LINE__);
    ui_recorder_stop();
    if(!epg_record)
    {
    ui_comm_cfmdlg_open(NULL, IDS_RECORD_FINISH, NULL, 5000);
    }
    on_pvr_recbar_save_exittoplay(0,0);
  }
  else
    on_pvr_recbar_onesecond(p_ctrl, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_info(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  control_t *p_pvr_frm = ctrl_get_child_by_id(p_ctrl, IDC_PVR_FRAME);
  g_bar_state = 1;
  ctrl_set_attr(p_pvr_frm, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_pvr_frm, OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_pvr_frm, TRUE);

  return SUCCESS;
}

static RET_CODE on_pvr_recbar_volume(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  open_volume_usb(ROOT_ID_PVR_REC_BAR, para1);
  return SUCCESS;
}

static RET_CODE on_pvr_recbar_exit_to_menu(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_RECORD,
    on_pvr_recbar_save_exittomenu, NULL,0);

  return SUCCESS;
}


static RET_CODE on_pvr_rec_bar_donothing(control_t *p_ctrl, u16 msg, 
  u32 para1, u32 para2)  
{
  return SUCCESS;
}

static void set_rec_icon(control_t *cont)
{
  control_t *frm;

  frm = ctrl_get_child_by_id(cont, IDC_PVR_REC_FLAG_FRAME);
  rec_focus = (rec_focus + 1) % 2;
  bmap_set_content_by_id(frm, rec_icon[rec_focus]);
  ctrl_paint_ctrl(frm, TRUE);
}

static RET_CODE on_update_rec(control_t *cont, u16 msg,
                                    u32 para1, u32 para2)
{
  set_rec_icon(cont);
  return SUCCESS;
}

static RET_CODE on_open_auido(control_t *cont, u16 msg,
                                    u32 para1, u32 para2)
{
  OS_PRINTF("%s,line %d\n",__FUNCTION__,__LINE__);
	return manage_open_menu(ROOT_ID_AUDIO_SET, ROOT_ID_PVR_REC_BAR, 0);
}
static RET_CODE on_pvr_donothing(control_t *cont, u16 msg,
                                    u32 para1, u32 para2)
{
	return SUCCESS;
}

static RET_CODE on_pvr_dync_pid(control_t *cont, u16 msg,
                                    u32 para1, u32 para2)
{
  int j;
  dvbs_prog_node_t prog = {0};
  mul_rec_media_t rec_media = {0};
  u16 pg_id = sys_status_get_curn_group_curn_prog_id();
  u16 total = db_dvbs_get_count(ui_dbase_get_pg_view_id());

  if ((pg_id == INVALIDID) || (total == 0))
  {
    return ERR_NOFEATURE;
  }

  db_dvbs_get_pg_by_id(pg_id, &prog);

  memset(&rec_media, 0, sizeof(mul_rec_media_t));
  rec_media.v_pid = (u16)prog.video_pid;
  rec_media.video_type = (vdec_src_fmt_t)prog.video_type;
  rec_media.a_pid = prog.audio[prog.audio_channel].p_id;
  rec_media.audio_type = (adec_src_fmt_vsb_t)prog.audio[prog.audio_channel].type;
  rec_media.pcr_pid = (u16)prog.pcr_pid;
  rec_media.extern_num = 2;
  rec_media.extern_pid[0].pid = DVB_PAT_PID;
  rec_media.extern_pid[0].type = EX_SI_PID;
  rec_media.extern_pid[1].pid = DVB_SDT_PID;
  rec_media.extern_pid[1].type = EX_SI_PID;
  for(j = 0; j < prog.audio_ch_num; j++)
  {
    if(j == prog.audio_channel)
      continue;
    if(j + 2 >= REC_MAX_EXTERN_PID)
    {
      OS_PRINTF("%s, more audio\n", __FUNCTION__);
      break;
    }
    if(j < DB_DVBS_MAX_AUDIO_CHANNEL)
    {
      rec_media.extern_pid[rec_media.extern_num].pid = prog.audio[j].p_id;
      rec_media.extern_pid[rec_media.extern_num].type = EX_AUDIO_PID;
      rec_media.extern_pid[rec_media.extern_num].fmt = prog.audio[j].type;
      rec_media.extern_num ++;
    }
  }
  rec_media.pmt_pid = prog.pmt_pid;

  ui_recorder_change_pid(&rec_media);
  
	return SUCCESS;
}

BEGIN_KEYMAP(pvr_rec_bar_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_CANCEL, MSG_RECORD_STOP)
  ON_EVENT(V_KEY_EXIT, MSG_RECORD_STOP)
  ON_EVENT(V_KEY_BACK, MSG_RECORD_STOP)
  ON_EVENT(V_KEY_BLUE, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_PLAY, MSG_RECORD_PLAY_PAUSE)
  ON_EVENT(V_KEY_PAUSE, MSG_RECORD_PLAY_PAUSE)
  ON_EVENT(V_KEY_STOP, MSG_RECORD_STOP)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_TO_MENU)
  ON_EVENT(V_KEY_INFO, MSG_INFO)
  ON_EVENT(V_KEY_LEFT, MSG_VOL_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOL_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOL_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_VOL_UP)
  ON_EVENT(V_KEY_RECALL, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_EPG, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_FAV, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_TTX, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_0, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_1, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_2, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_3, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_4, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_5, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_6, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_7, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_8, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_9, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_AUDIO, MSG_PVR_AUDIO)
  ON_EVENT(V_KEY_OK, MSG_OK)
END_KEYMAP(pvr_rec_bar_cont_keymap, NULL)

BEGIN_MSGPROC(pvr_rec_bar_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_OK, on_pvr_recbar_info)
  ON_COMMAND(MSG_FOCUS_UP, on_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_focus_change)
  ON_COMMAND(MSG_PAGE_UP, on_focus_change)
  ON_COMMAND(MSG_PAGE_DOWN, on_focus_change)
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
  ON_COMMAND(MSG_DESTROY, on_pbar_destory)
  ON_COMMAND(MSG_RECORD_PLAY_PAUSE, on_pvr_recbar_rec_pause)
  ON_COMMAND(MSG_PLUG_OUT, on_pvr_recbar_plug_out)
  //ON_COMMAND(MSG_ONE_SECOND_ARRIVE, on_pvr_recbar_onesecond)
  ON_COMMAND(MSG_RECORD_STOP, on_pvr_recbar_stop)
  ON_COMMAND(MSG_EXIT_TO_MENU, on_pvr_recbar_exit_to_menu)
  ON_COMMAND(MSG_PVR_REC_STOPPED, on_pvr_recbar_rec_stopped)  
  ON_COMMAND(MSG_PVR_REC_MEM_NOT_ENOUGH, on_pvr_recbar_mem_not_enough)
  ON_COMMAND(MSG_WRITE_ERROR, on_pvr_recbar_write_error)
  ON_COMMAND(MSG_PVR_REC_NO_SIGNAL, on_pvr_recbar_no_signal)  
  ON_COMMAND(MSG_PVR_REC_UPDATED, on_pvr_recbar_updated)  
  ON_COMMAND(MSG_INFO, on_pvr_recbar_info)  
  ON_COMMAND(MSG_VOL_DOWN, on_pvr_recbar_volume)  
  ON_COMMAND(MSG_VOL_UP, on_pvr_recbar_volume)
  ON_COMMAND(MSG_REC, on_update_rec)
  ON_COMMAND(MSG_PVR_AUDIO, on_open_auido)
  ON_COMMAND(MSG_PVR_DO_NOTHING, on_pvr_donothing)
  ON_COMMAND(MSG_DYNC_PID_UPDATE, on_pvr_dync_pid)
END_MSGPROC(pvr_rec_bar_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(pvr_rec_bar_bar_keymap, NULL)
  ON_EVENT(V_KEY_TVRADIO, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_REC, MSG_DVR_INFO)
  ON_EVENT(V_KEY_BLUE, MSG_PVR_DO_NOTHING)
  //ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(pvr_rec_bar_bar_keymap, NULL)

BEGIN_MSGPROC(pvr_rec_bar_bar_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_pvr_rec_bar_donothing)
  ON_COMMAND(MSG_CANCEL, on_pvr_rec_bar_hide)
  ON_COMMAND(MSG_DVR_INFO, on_pvr_rec_bar_bar_dvrinfokey)
  ON_COMMAND(MSG_PVR_DO_NOTHING, on_pvr_donothing)
END_MSGPROC(pvr_rec_bar_bar_proc, ui_comm_root_proc)

BEGIN_KEYMAP(pvr_rec_bar_time_duration_keymap, ui_comm_tbox_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_REC, MSG_DVR_INFO)
  ON_EVENT(V_KEY_TVRADIO, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_BLUE, MSG_PVR_DO_NOTHING)
  ON_EVENT(V_KEY_INFO, MSG_DVR_INFO)
END_KEYMAP(pvr_rec_bar_time_duration_keymap, ui_comm_tbox_keymap)

BEGIN_MSGPROC(pvr_rec_bar_time_duration_proc, ui_comm_time_proc)
  ON_COMMAND(MSG_EXIT, on_pvr_recbar_time_duration_exit)
  ON_COMMAND(MSG_DVR_INFO, on_pvr_recbar_time_duration_dvrinfokey)
  ON_COMMAND(MSG_PVR_DO_NOTHING, on_pvr_donothing)
END_MSGPROC(pvr_rec_bar_time_duration_proc, ui_comm_time_proc)
#endif
