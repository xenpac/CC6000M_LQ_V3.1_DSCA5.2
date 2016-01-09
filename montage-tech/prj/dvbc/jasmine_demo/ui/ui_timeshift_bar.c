/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifdef ENABLE_TIMESHIFT_CONFIG
#include "ui_common.h"

#include "ui_timeshift_bar.h"
#include "ui_pause.h"
#include "pvr_api.h"
#include "ui_timeshift_api.h"
#include "file_list.h"
#include "pnp_service.h"
#include "ui_do_search.h"
#include "ui_upgrade_by_rs232.h"
#include "ui_mute.h"
#include "ui_jump.h"
#include "ui_signal.h"
#include "ui_dvr_config.h"
#include "sys_app.h"
#ifdef ENABLE_CA
#include "cas_manager.h"
#endif
typedef enum
{
  MSG_ZOOM = MSG_LOCAL_BEGIN + 1100,
}ts_msg_t;

enum control_id
{
	IDC_TIMESHIFT_BAR_LIST_CONT = 1,
	IDC_TIMESHIFT_BAR_FRAME = 2,
	IDC_TIMESHIFT_SPECIAL_PLAY_IMG_FRAME = 3,
	IDC_TIMESHIFT_SPECIAL_PLAY_TXT_FRAME = 4,
	IDC_TIMESHIFT_TS_LOGO_FRAME = 5,
	IDC_TIMESHIFT_TS_NO_SINGAL_FRAME = 6,
};

enum list_cont_sub_control_id
{
	IDC_TIMESHIFT_BAR_TITLE = 5,
	IDC_TIMESHIFT_BAR_LIST = 6,
	IDC_TIMESHIFT_BAR_LIST_SCROLL = 7,
};


enum frame_sub_control_id
{
	IDC_TIMESHIFT_ICON = 6,
	IDC_TIMESHIFT_PAUSE_STAT_ICON,
	IDC_TIMESHIFT_PLAY_STAT_ICON,
	IDC_TIMESHIFT_FF_STAT_ICON,           //fast forward
	IDC_TIMESHIFT_FB_STAT_ICON,           //fast backward
	IDC_TIMESHIFT_SF_STAT_ICON,           //slow forward
	IDC_TIMESHIFT_SB_STAT_ICON,           //slow backward
	IDC_TIMESHIFT_TSF_STAT_ICON,          //time seek forward
	IDC_TIMESHIFT_TSB_STAT_ICON,          //time seek backward
	IDC_TIMESHIFT_TXT_TS,
	IDC_TIMESHIFT_NAME,
	IDC_TIMESHIFT_DATE,
	IDC_TIMESHIFT_TIME,
	IDC_TIMESHIFT_CAPACITY_ICON,
	IDC_TIMESHIFT_CAPACITY_TEXT,
	IDC_TIMESHIFT_PLAY_CUR_TIME,
	IDC_TIMESHIFT_START_TIME,
	IDC_TIMESHIFT_REC_CUR_TIME,
	IDC_TIMESHIFT_PLAY_TIME_BUTTON,
	IDC_TIMESHIFT_PLAY_TIME_TXT,
	IDC_TIMESHIFT_REC_PROGRESS,
	IDC_TIMESHIFT_PLAY_PROGRESS,
	IDC_TSFT_TIRCK,
	IDC_TIMESHIFT_SEEK_POINT,
};

typedef struct
{
	mul_pvr_play_state_t play_status;
	mul_pvr_play_mode_t trick_speed;
	mul_pvr_play_position_t seek_info;
} ui_timeshift_status_t;

typedef struct
{
  /*!
    record start time.
    */
  u32 rec_start;
  /*!
    record end time.
    */
  u32 rec_curn;
  /*!
    current play time.
    */
  u32 play_curn;
  /*!
    already record size(kb)
    */
  u32 rec_size;
  /*!
    already played size(kb)
    */
  u32 play_size;
}tshift_position_t;

static tshift_position_t g_tshift_opsition = {0};
static s32 g_seek_delta = 0;
static ui_timeshift_status_t g_tshift_status = {MUL_PVR_PLAY_STATE_INVALID, };
static ui_pvr_private_info_t g_ui_ts_info = {INVALID, INVALID, };
static BOOL g_timeshift_start = FALSE;
//static mul_pvr_play_status_t g_play_status = {0};
//static mul_pvr_rec_status_t g_rec_status = {0};

/*!
  if True, timeshift is pause, and left / right trick the position.
  */
static BOOL g_is_pause_trick = FALSE;

/*!

  */
//static BOOL g_timeshift_lock = FALSE;

u16 timeshift_bar_mainwin_keymap(u16 key);

RET_CODE timeshift_bar_mainwin_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
/*
void ui_timeshift_call_back(u32 chnid, mul_pvr_event_t event_type, s32 event_value, void *p_args)
{
	switch(event_type)
	{
  	case MUL_PVR_EVENT_REC_DISKFULL:
  		break;

  	case MUL_PVR_EVENT_REC_DISK_SLOW:
  		break;

  	case MUL_PVR_EVENT_PLAY_TIME_UPDATE:
  		mul_pvr_play_get_status (chnid, &g_play_status);
    //  OS_PRINTF("Andy---play_curn%d,play_size:%d\n",g_play_status.cur_play_time_ms/1000,g_play_status.cur_play_pos);
  		fw_notify_root(fw_find_root_by_id(ROOT_ID_TIMESHIFT_BAR), NOTIFY_T_MSG, FALSE,
  			MSG_TIMESHIFT_EVT_PLAY_UPDATED, event_value, (u32)&g_play_status);
  		break;

  	case MUL_PVR_EVENT_REC_TIME_UPDATE:
  		mul_pvr_rec_get_status (chnid, &g_rec_status);
    //  OS_PRINTF("Andy---rec_curn%d,rec_start%d,rec_size:%d\n",
     //   g_rec_status.cur_time_ms/1000,g_rec_status.start_time_ms/1000,g_rec_status.cur_write_point);
  		fw_notify_root(fw_find_root_by_id(ROOT_ID_TIMESHIFT_BAR), NOTIFY_T_MSG, FALSE,
  			MSG_TIMESHIFT_EVT_REC_UPDARED, event_value, (u32)&g_rec_status);
  		break;

  	case MUL_PVR_EVENT_PLAY_SOF:
  	  fw_notify_root(fw_find_root_by_id(ROOT_ID_TIMESHIFT_BAR), NOTIFY_T_MSG, FALSE,
  			MSG_TIMESHIFT_EVT_PLAY_SOF, 0, 0);
  		break;

    case MUL_PVR_EVENT_PLAY_REACH_REC:
      fw_notify_root(fw_find_root_by_id(ROOT_ID_TIMESHIFT_BAR), NOTIFY_T_MSG, FALSE,
  			MSG_TIMESHIFT_EVT_PLAY_REACH_REC, 0, 0);
  		break;

  	default:
  		break;
	}
}
*/
void timeshift_in_bar_back(control_t *p_ctrl)
{
	control_t *p_subctrl;

	/*Pause*/
	p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_TIMESHIFT_PAUSE_STAT_ICON);
	bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_PAUSE);
	ctrl_paint_ctrl(p_subctrl,  TRUE);

	/*Play*/
	p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_TIMESHIFT_PLAY_STAT_ICON);
	bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_PLAY_2);
	ctrl_paint_ctrl(p_subctrl,  TRUE);

	/*Fast forward*/
	p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_TIMESHIFT_FF_STAT_ICON);
	bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_FF_V2);
	ctrl_paint_ctrl(p_subctrl,  TRUE);

	/*Fast backward*/
	p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_TIMESHIFT_FB_STAT_ICON);
	bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_FB_V2);
	ctrl_paint_ctrl(p_subctrl,  TRUE);
#if 0
	/*Slow forward*/
	p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_TIMESHIFT_SF_STAT_ICON);
	bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_SF);
	ctrl_paint_ctrl(p_subctrl,  TRUE);

	/*Slow backward*/
	p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_TIMESHIFT_SB_STAT_ICON);
	bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_SB);
	ctrl_paint_ctrl(p_subctrl,  TRUE);
    #endif

}

void fill_timeshift_info(control_t *p_bar_cont)
{
	control_t *p_ctrl, *p_subctrl;
	u16 prog_id = sys_status_get_curn_group_curn_prog_id();
	dvbs_prog_node_t pg;
	p_ctrl = p_bar_cont;

	if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
	{
		return;
	}

	// set name
	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_TIMESHIFT_NAME);
	text_set_content_by_unistr(p_subctrl, pg.name);
}

static void timeshift_fill_time(control_t *cont, BOOL is_redraw)
{
	utc_time_t utc_time;
	control_t *p_frm = NULL, *p_ctrl = NULL;
	u8 time_str[32];

	p_frm = ctrl_get_child_by_id(cont, IDC_TIMESHIFT_BAR_FRAME);
	MT_ASSERT(p_frm != NULL);

	time_get(&utc_time, FALSE);

	sprintf((char *)time_str, "%.4d/%.2d/%.2d", utc_time.year, utc_time.month, utc_time.day);
	p_ctrl = ctrl_get_child_by_id(p_frm, IDC_TIMESHIFT_DATE);
	MT_ASSERT(p_ctrl != NULL);
	text_set_content_by_ascstr(p_ctrl, time_str);
	if (is_redraw)
	{
	  if(g_ui_ts_info.b_showbar)
	  {
	    ctrl_paint_ctrl(p_ctrl, TRUE);
	  }
	}
	sprintf((char *)time_str, "%.2d:%.2d", utc_time.hour, utc_time.minute);
	p_ctrl = ctrl_get_child_by_id(p_frm, IDC_TIMESHIFT_TIME);
	MT_ASSERT(p_ctrl != NULL);
	text_set_content_by_ascstr(p_ctrl, time_str);
	if (is_redraw)
	{
	  if(g_ui_ts_info.b_showbar)
	  {
	    ctrl_paint_ctrl(p_ctrl, TRUE);
	  }
	}

}

static void timeshift_fill_rec_time_info(control_t *p_bar_cont, mul_pvr_rec_status_t * p_rec_status)
{
	utc_time_t time_start = {0};
	utc_time_t time_end = {0};
	control_t *p_ctrl = NULL;
	u8 time_str[32];

	p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_TIMESHIFT_START_TIME);
	MT_ASSERT(p_ctrl != NULL);
	time_up(&time_start, p_rec_status->start_time_ms/1000);
	sprintf((char *)time_str, "%.2d:%.2d:%.2d", time_start.hour, time_start.minute, time_start.second);
	text_set_content_by_ascstr(p_ctrl, time_str);
	if(g_ui_ts_info.b_showbar)
	{
	  ctrl_paint_ctrl(p_ctrl, TRUE);
	}
	p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_TIMESHIFT_REC_CUR_TIME);
	MT_ASSERT(p_ctrl != NULL);
	time_up(&time_end, p_rec_status->end_time_ms/1000);
	sprintf((char *)time_str, "%.2d:%.2d:%.2d", time_end.hour, time_end.minute, time_end.second);
	text_set_content_by_ascstr(p_ctrl, time_str);
	if(g_ui_ts_info.b_showbar)
	{
	  ctrl_paint_ctrl(p_ctrl, TRUE);
	}
}

/*!
  recaculte cursors postion by play size, rec size & total size.
  */
static u16 timeshift_play_get_play_pos(void)
{
	u32 motion = 0;
	u32 block_size = 0;

  block_size = (g_ui_ts_info.data_size / 1024);

  if(g_tshift_opsition.rec_size <= block_size)
  {
    if(block_size != 0)
    {
      motion = (u32)(g_tshift_opsition.play_size * TIMESHIFT_BAR_REC_PROGRESS_W / block_size);
    }
  }
  else
	{
		if(g_tshift_opsition.rec_size >= (g_tshift_opsition.play_size  + block_size))
		{
			motion = 0;
		}
		if(g_tshift_opsition.rec_size < (g_tshift_opsition.play_size  + block_size))
		{
		  if(block_size != 0)
      {  
			  motion = (u32)((block_size + g_tshift_opsition.play_size  - g_tshift_opsition.rec_size) * TIMESHIFT_BAR_REC_PROGRESS_W / block_size);
      }
		}
	}

  return (u16)motion;
}


static void timeshift_bar_play_move(control_t *p_bar_cont, utc_time_t utc_time, BOOL b_move)
{
	control_t *p_play_time = NULL;
	control_t *p_cursor = NULL;
	rect_t rect = {0};
	u8 time_str[32];
  u16 center = 0;
  u16 new_center = 0;
  u16 offset;

  new_center = timeshift_play_get_play_pos();
  new_center += TIMESHIFT_BAR_REC_PROGRESS_X;

	p_play_time = ctrl_get_child_by_id(p_bar_cont, IDC_TIMESHIFT_PLAY_TIME_TXT);
	MT_ASSERT(p_play_time != NULL);
	ctrl_set_sts(p_play_time, OBJ_STS_SHOW);

	sprintf((char *)time_str, "%.2d:%.2d:%.2d", utc_time.hour, utc_time.minute, utc_time.second);
	text_set_content_by_ascstr(p_play_time, time_str);

	//play point
	p_cursor = ctrl_get_child_by_id(p_bar_cont, IDC_TIMESHIFT_PLAY_PROGRESS);
	MT_ASSERT(p_cursor != NULL);
	ctrl_set_sts(p_cursor, OBJ_STS_SHOW);

	//get old center.
	ctrl_get_frame(p_cursor, &rect);

  center = (u16)(rect.left + rect.right )/2;

  //caculate offset
  offset = new_center - center;
  ctrl_empty_invrgn(p_play_time);
	ctrl_move_ctrl(p_play_time, (s16)offset, 0);
	ctrl_empty_invrgn(p_cursor);
	ctrl_move_ctrl(p_cursor, (s16)offset, 0);

	if(g_ui_ts_info.b_showbar)
	{
	  ctrl_paint_ctrl(p_bar_cont, FALSE);
	}
}

void reset_pvr_para(void)
{
  //g_ui_ts_info.data_size = 0;

  g_ui_ts_info.file_name_len = 0;
}

static BOOL get_pvr_para(void)
{
	u16 pg_id, total;
	dvbs_prog_node_t prog;

	total = db_dvbs_get_count(ui_dbase_get_pg_view_id());

	pg_id = sys_status_get_curn_group_curn_prog_id();
	if((pg_id == INVALIDID) || (total == 0))
	{
		return FALSE;
	}

	db_dvbs_get_pg_by_id(pg_id, &prog);

	g_ui_ts_info.audio_pid = prog.audio[prog.audio_channel].p_id;
	g_ui_ts_info.audio_type = (u8)prog.audio[prog.audio_channel].type;
	g_ui_ts_info.video_pid = prog.video_pid;
	g_ui_ts_info.video_type = (u8)prog.video_type;
	g_ui_ts_info.pcr_pid = prog.pcr_pid;
	g_ui_ts_info.b_scramble = prog.is_scrambled;
  if(uni_strlen(g_ui_ts_info.file_name) == 0)
  {
    return FALSE;
  }

  if(g_ui_ts_info.file_name_len == 0)
  {
    return FALSE;
  }
  
  if(g_ui_ts_info.data_size == 0)
  {
    return FALSE;
  }
  
  return TRUE;
}

void ui_tshift_stream_seek()
{
  mul_pvr_play_attr_t play_para = {0};

  ui_tshift_play_cfg(&play_para, &g_ui_ts_info);
  ui_stop_play(STOP_PLAY_FREEZE, TRUE);
  ui_tshift_play_start((u32 *)&g_ui_ts_info.h_play, (u32)g_ui_ts_info.h_rec, &play_para);
  ui_tshift_pause((u32)g_ui_ts_info.h_play);

  g_tshift_opsition.play_curn = g_tshift_opsition.rec_curn;
  g_tshift_opsition.play_size = g_tshift_opsition.rec_size;
  ui_register_pvr_cb((u32)g_ui_ts_info.h_play, (event_call_back)ui_tshift_callback);
}

static void timeshift_format(void)
{
  partition_t *p_partition = NULL;
  u8 work_partition = 0, total_partition = 0;
  u16 partition_letter[10] = {0};

  total_partition = vfs_get_partitions(partition_letter, 10);
  work_partition = sys_status_get_usb_work_partition();

  if(work_partition >= total_partition)
  {
    work_partition = 0;

    sys_status_set_usb_work_partition(work_partition);
  }

  file_list_get_partition(&p_partition);

  vfs_format(p_partition[work_partition].letter[0], VFS_FAT32);
}

//lint -e438
RET_CODE open_timeshift_bar(u32 para1, u32 para2)
{
	control_t *p_cont;
	control_t *p_bar_ctrl, *p_txt_ctrl, *p_logo_ctrl, *p_subctrl, *p_play_progress, *p_play_progress2;
	control_t *p_trick = NULL;
	mul_pvr_rec_attr_t rec_para;
  osd_set_t osd_set = {0};
  dvbs_prog_node_t prog;
  u16 pg_id, total;

  partition_t *p_partition = NULL;

  if(file_list_get_partition(&p_partition) == 0)
  {
    //no usb insert.
    ui_comm_cfmdlg_open(NULL, IDS_USB_DISCONNECT, NULL, 2000);

    return ERR_FAILURE;
  }
  else
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
  }
  
  if(!get_pvr_para())
  {
    if(SUCCESS != ui_timeshift_total_size())
    {
      ui_comm_ask_for_dodlg_open(NULL, IDS_TIMESHIFT_FORMAT_USB, timeshift_format, NULL, 0);

      if(SUCCESS != ui_timeshift_total_size())
      {
      return ERR_FAILURE;
      }
    }
  }

  if(!ui_signal_is_lock())
  {
  	return ERR_FAILURE;
  }

  pg_id = sys_status_get_curn_group_curn_prog_id();
  total = db_dvbs_get_count(ui_dbase_get_pg_view_id());

  if ((pg_id == INVALIDID) || (total == 0))
  {
    return ERR_FAILURE;
  }

  db_dvbs_get_pg_by_id(pg_id, &prog);

  if(ui_is_playpg_scrambled())
  {
    ui_comm_cfmdlg_open(NULL, IDS_TIMESHIFT_SCRAMBLE_PRO, NULL, 2000);
    return ERR_FAILURE;
  }

  if(fw_find_root_by_id(ROOT_ID_ZOOM) != NULL)
  {
    manage_close_menu(ROOT_ID_ZOOM, 0, 0);
  }
  //open menu.
  p_cont = fw_create_mainwin(ROOT_ID_TIMESHIFT_BAR,
		TIMESHIFT_BAR_CONT_X, TIMESHIFT_BAR_CONT_Y,
		TIMESHIFT_BAR_CONT_W, TIMESHIFT_BAR_CONT_H,
		ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, timeshift_bar_mainwin_keymap);
  ctrl_set_proc(p_cont, timeshift_bar_mainwin_proc);

	//Top-right remind special play img
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_SPECIAL_PLAY_IMG_FRAME,
		TIMESHIFT_BAR_SPECIAL_PLAY_IMG_X, TIMESHIFT_BAR_SPECIAL_PLAY_IMG_Y,
		TIMESHIFT_BAR_SPECIAL_PLAY_IMG_W,TIMESHIFT_BAR_SPECIAL_PLAY_IMG_H,
		p_cont, 0);
  ctrl_set_rstyle(p_subctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

	//special play txt
	p_txt_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_SPECIAL_PLAY_TXT_FRAME,
		TIMESHIFT_BAR_SPECIAL_PLAY_TXT_X, TIMESHIFT_BAR_SPECIAL_PLAY_TXT_Y,
		TIMESHIFT_BAR_SPECIAL_PLAY_TXT_W,TIMESHIFT_BAR_SPECIAL_PLAY_TXT_H,
		p_cont, 0);
	ctrl_set_rstyle(p_txt_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_align_type(p_txt_ctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_txt_ctrl, FSI_TIMESHIFT_SPECIAL_PLAY_TEXT,
		FSI_TIMESHIFT_SPECIAL_PLAY_TEXT, FSI_TIMESHIFT_SPECIAL_PLAY_TEXT);
	text_set_content_type(p_txt_ctrl, TEXT_STRTYPE_UNICODE);

	//Top-left TS logo
	p_logo_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_TS_LOGO_FRAME,
		TIMESHIFT_BAR_TS_LOGO_X, TIMESHIFT_BAR_TS_LOGO_Y,
		TIMESHIFT_BAR_TS_LOGO_W,TIMESHIFT_BAR_TS_LOGO_H,
		p_cont, 0);
	bmap_set_content_by_id(p_logo_ctrl, IM_INFORMATION_TS);

  //Top-left TS No signal
	p_logo_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_TS_NO_SINGAL_FRAME,
		TIMESHIFT_BAR_TS_NO_SIGNAL_X, TIMESHIFT_BAR_TS_NO_SIGNAL_Y,
		TIMESHIFT_BAR_TS_NO_SIGNAL_W,TIMESHIFT_BAR_TS_NO_SIGNAL_H,
		p_cont, 0);

	//frame part
	p_bar_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_TIMESHIFT_BAR_FRAME,
		TIMESHIFT_BAR_FRM_X, TIMESHIFT_BAR_FRM_Y,
		TIMESHIFT_BAR_FRM_W,TIMESHIFT_BAR_FRM_H, p_cont, 0);
	ctrl_set_rstyle(p_bar_ctrl, RSI_TIMESHIFT_BAR_FRM, RSI_TIMESHIFT_BAR_FRM, RSI_TIMESHIFT_BAR_FRM);

	//FB icon
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_FB_STAT_ICON,
		TIMESHIFT_BAR_PLAY_STAT_ICON_X  + 51, TIMESHIFT_BAR_PLAY_STAT_ICON_Y,
		TIMESHIFT_BAR_PLAY_STAT_ICON_W, TIMESHIFT_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);

	//SB icon
	//p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_SB_STAT_ICON,
	//	TIMESHIFT_BAR_PLAY_STAT_ICON_X + 51, TIMESHIFT_BAR_PLAY_STAT_ICON_Y,
	//	TIMESHIFT_BAR_PLAY_STAT_ICON_W, TIMESHIFT_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);

	//play icon
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_PLAY_STAT_ICON,
		TIMESHIFT_BAR_PLAY_STAT_ICON_X+121, TIMESHIFT_BAR_PLAY_STAT_ICON_Y,
		TIMESHIFT_BAR_PLAY_STAT_ICON_W, TIMESHIFT_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);

	//pause icon
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_PAUSE_STAT_ICON,
		TIMESHIFT_BAR_PLAY_STAT_ICON_X + 191, TIMESHIFT_BAR_PLAY_STAT_ICON_Y,
		TIMESHIFT_BAR_PLAY_STAT_ICON_W, TIMESHIFT_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);

	//SF
	//p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_SF_STAT_ICON,
	//	TIMESHIFT_BAR_PLAY_STAT_ICON_X+261, TIMESHIFT_BAR_PLAY_STAT_ICON_Y,
	//	TIMESHIFT_BAR_PLAY_STAT_ICON_W, TIMESHIFT_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);

	//FF
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_FF_STAT_ICON,
		TIMESHIFT_BAR_PLAY_STAT_ICON_X+261, TIMESHIFT_BAR_PLAY_STAT_ICON_Y,
		TIMESHIFT_BAR_PLAY_STAT_ICON_W, TIMESHIFT_BAR_PLAY_STAT_ICON_H, p_bar_ctrl, 0);

	//TS
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_TXT_TS,
		TIMESHIFT_BAR_TXT_TS_X, TIMESHIFT_BAR_TXT_TS_Y,
		TIMESHIFT_BAR_TXT_TS_W, TIMESHIFT_BAR_TXT_TS_H,
		p_bar_ctrl, 0);
	bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TS);

	//PVR icon
	#if 0
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_ICON,
		TIMESHIFT_BAR_ICON_X, TIMESHIFT_BAR_ICON_Y,
		TIMESHIFT_BAR_ICON_W, TIMESHIFT_BAR_ICON_H, p_bar_ctrl, 0);
	bmap_set_content_by_id(p_subctrl, IM_INFORMATION_PVR);
  #endif
	//name
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_NAME,
		TIMESHIFT_BAR_NAME_TXT_X, TIMESHIFT_BAR_NAME_TXT_Y,
		TIMESHIFT_BAR_NAME_TXT_W, TIMESHIFT_BAR_NAME_TXT_H,
		p_bar_ctrl, 0);
	text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_TIMESHIFT_BAR_DATE,
		FSI_TIMESHIFT_BAR_DATE,FSI_TIMESHIFT_BAR_DATE);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

	// date & time
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
		TIMESHIFT_BAR_DATE_ICON_X, TIMESHIFT_BAR_DATE_ICON_Y,
		TIMESHIFT_BAR_DATE_ICON_W, TIMESHIFT_BAR_DATE_ICON_H,
		p_bar_ctrl, 0);
	bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_DATE,
		TIMESHIFT_BAR_DATE_TXT_X, TIMESHIFT_BAR_DATE_TXT_Y,
		TIMESHIFT_BAR_DATE_TXT_W, TIMESHIFT_BAR_DATE_TXT_H,
		p_bar_ctrl, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_TIMESHIFT_BAR_DATE,
		FSI_TIMESHIFT_BAR_DATE,FSI_TIMESHIFT_BAR_DATE);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
		TIMESHIFT_BAR_TIME_ICON_X, TIMESHIFT_BAR_TIME_ICON_Y,
		TIMESHIFT_BAR_TIME_ICON_W, TIMESHIFT_BAR_TIME_ICON_H,
		p_bar_ctrl, 0);
	//bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_TIME,
		TIMESHIFT_BAR_TIME_TXT_X, TIMESHIFT_BAR_TIME_TXT_Y,
		TIMESHIFT_BAR_TIME_TXT_W, TIMESHIFT_BAR_TIME_TXT_H,
		p_bar_ctrl, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_TIMESHIFT_BAR_DATE,
		FSI_TIMESHIFT_BAR_DATE,FSI_TIMESHIFT_BAR_DATE);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

	// capacity icon
	p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_CAPACITY_ICON,
		TIMESHIFT_BAR_CAPACITY_ICON_X, TIMESHIFT_BAR_CAPACITY_ICON_Y,
		TIMESHIFT_BAR_CAPACITY_ICON_W, TIMESHIFT_BAR_CAPACITY_ICON_H,
		p_bar_ctrl, 0);

	// start timeshift time
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_START_TIME,
		TIMESHIFT_BAR_PLAY_CUR_TIME_X, TIMESHIFT_BAR_PLAY_CUR_TIME_Y,
		TIMESHIFT_BAR_PLAY_CUR_TIME_W, TIMESHIFT_BAR_PLAY_CUR_TIME_H,
		p_bar_ctrl, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_TIMESHIFT_BAR_INFO,
		FSI_TIMESHIFT_BAR_INFO, FSI_TIMESHIFT_BAR_INFO);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

	// cur recoder timeshift time
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_REC_CUR_TIME,
		TIMESHIFT_BAR_REC_TOTAL_TIME_X, TIMESHIFT_BAR_REC_TOTAL_TIME_Y,
		TIMESHIFT_BAR_REC_TOTAL_TIME_W, TIMESHIFT_BAR_REC_TOTAL_TIME_H,
		p_bar_ctrl, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_TIMESHIFT_BAR_INFO,
		FSI_TIMESHIFT_BAR_INFO, FSI_TIMESHIFT_BAR_INFO);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

	//capacity text
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_CAPACITY_TEXT,
		TIMESHIFT_BAR_CAPACITY_TEXT_X, TIMESHIFT_BAR_CAPACITY_TEXT_Y,
		TIMESHIFT_BAR_CAPACITY_TEXT_W, TIMESHIFT_BAR_CAPACITY_TEXT_H,
		p_bar_ctrl, 0);
	text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_TIMESHIFT_BAR_INFO,
		FSI_TIMESHIFT_BAR_INFO, FSI_TIMESHIFT_BAR_INFO);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

	//record progress
	p_play_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_TIMESHIFT_REC_PROGRESS,
		TIMESHIFT_BAR_REC_PROGRESS_X, TIMESHIFT_BAR_REC_PROGRESS_Y,
		TIMESHIFT_BAR_REC_PROGRESS_W, TIMESHIFT_BAR_REC_PROGRESS_H,
		p_bar_ctrl, 0);
	ctrl_set_rstyle(p_play_progress, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	ctrl_set_mrect(p_play_progress,
		TIMESHIFT_BAR_REC_PROGRESS_MIDX, TIMESHIFT_BAR_REC_PROGRESS_MIDY,
		TIMESHIFT_BAR_REC_PROGRESS_MIDW, TIMESHIFT_BAR_REC_PROGRESS_MIDH);
	pbar_set_rstyle(p_play_progress, TIMESHIFT_BAR_REC_PROGRESS_MIN, TIMESHIFT_BAR_REC_PROGRESS_MAX, TIMESHIFT_BAR_REC_PROGRESS_MID);
	pbar_set_count(p_play_progress, 0, TIMESHIFT_BAR_REC_PROGRESS_STEP, TIMESHIFT_BAR_REC_PROGRESS_STEP);
	pbar_set_direction(p_play_progress, 1);
	pbar_set_workmode(p_play_progress, TRUE, 0);
	pbar_set_current(p_play_progress, 0);

	//play process time
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMESHIFT_PLAY_TIME_TXT,
		TIMESHIFT_BAR_PLAY_TIME_X, TIMESHIFT_BAR_PLAY_TIME_Y,
		TIMESHIFT_BAR_PLAY_TIME_W, TIMESHIFT_BAR_PLAY_TIME_H,
		p_bar_ctrl, 0);
	ctrl_set_rstyle(p_subctrl,
	  RSI_TIMESHIFT_PLAY_TIME_PBACK, RSI_TIMESHIFT_PLAY_TIME_PBACK, RSI_TIMESHIFT_PLAY_TIME_PBACK);
  ctrl_set_sts(p_subctrl, OBJ_STS_HIDE);
	text_set_align_type(p_subctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_subctrl, FSI_TIMESHIFT_PLAY_TIME,  FSI_WHITE, FSI_TIMESHIFT_PLAY_TIME);

	//play process point
	p_play_progress2 = ctrl_create_ctrl(CTRL_BMAP, IDC_TIMESHIFT_PLAY_PROGRESS,
		TIMESHIFT_BAR_PLAY_PROGRESS_X, TIMESHIFT_BAR_PLAY_PROGRESS_Y,
		TIMESHIFT_BAR_PLAY_PROGRESS_W, TIMESHIFT_BAR_PLAY_PROGRESS_H, p_bar_ctrl, 0);
  ctrl_set_sts(p_play_progress2, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_play_progress2, IM_ICONS_PLAY);

	//play process point
	p_trick = ctrl_create_ctrl(CTRL_BMAP, IDC_TSFT_TIRCK,
		TSFT_TRICK_X, TSFT_TRICK_Y, TSFT_TRICK_W, TSFT_TRICK_H, p_bar_ctrl, 0);
  ctrl_set_sts(p_trick, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_trick, IM_ICONS_TRICK);

	timeshift_fill_time(p_cont, FALSE);
	fill_timeshift_info(p_bar_ctrl);

	ctrl_process_msg(p_bar_ctrl, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  g_ui_ts_info.jump_time = ui_jump_para_get();
	g_ui_ts_info.b_showbar = TRUE;
	g_timeshift_start = TRUE;

	ui_tshift_rec_cfg(&rec_para, &g_ui_ts_info);

  ui_tshift_keymap_init();
	if(g_ui_ts_info.h_rec == INVALID)
	{
		ui_tshift_rec_create((u32 *)&g_ui_ts_info.h_rec, &rec_para);
	}
	else
	{
		ui_tshift_rec_set((u32)g_ui_ts_info.h_rec, &rec_para);
	}

	ui_register_pvr_cb((u32)g_ui_ts_info.h_rec, (event_call_back)ui_tshift_callback);

  ui_set_mute(ui_is_mute());

  ui_tshift_stream_seek();

  sys_status_get_osd_set(&osd_set);

	fw_tmr_create(ROOT_ID_TIMESHIFT_BAR, MSG_HIDE_BAR, osd_set.timeout * 1000, FALSE);
#ifdef EXTERN_CA_PVR
  cas_manager_set_timeshift_play(TRUE);
#endif
  OS_PRINTF("@@## create tmr\n");

	return SUCCESS;
}
//lint +e438

static RET_CODE timeshift_fast_slow_display(control_t *p_mainwin, ui_timeshift_status_t para)
{
  control_t *p_txt_ctrl = NULL;
	control_t *p_img_ctrl = NULL;

  p_img_ctrl = ctrl_get_child_by_id(p_mainwin, IDC_TIMESHIFT_SPECIAL_PLAY_IMG_FRAME);
  p_txt_ctrl = ctrl_get_child_by_id(p_mainwin, IDC_TIMESHIFT_SPECIAL_PLAY_TXT_FRAME);

	if(MUL_PVR_PLAY_STATE_FF == para.play_status)
	{
	  bmap_set_content_by_id(p_img_ctrl, IM_INFORMATION_FF_2);
		switch(para.trick_speed.speed)
		{
		case MUL_PVR_PLAY_SPEED_2X_FAST_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 2");
			break;
		case MUL_PVR_PLAY_SPEED_4X_FAST_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 4");
			break;
		case MUL_PVR_PLAY_SPEED_8X_FAST_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 8");
			break;
		case MUL_PVR_PLAY_SPEED_16X_FAST_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 16");
			break;
		case MUL_PVR_PLAY_SPEED_32X_FAST_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 32");
			break;
		case MUL_PVR_PLAY_SPEED_NORMAL:
			break;
    default:
      break;
		}
	}
	if(MUL_PVR_PLAY_STATE_FB == para.play_status)
	{
	  bmap_set_content_by_id(p_img_ctrl, IM_INFORMATION_FB_2);
		switch(para.trick_speed.speed)
		{
		case MUL_PVR_PLAY_SPEED_32X_FAST_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 32");
			break;
		case MUL_PVR_PLAY_SPEED_16X_FAST_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 16");
			break;
		case MUL_PVR_PLAY_SPEED_8X_FAST_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 8");
			break;
		case MUL_PVR_PLAY_SPEED_4X_FAST_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 4");
			break;
		case MUL_PVR_PLAY_SPEED_2X_FAST_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 2");
			break;
		case MUL_PVR_PLAY_SPEED_NORMAL:
			break;
    default:
      break;
		}
	}
	if(MUL_PVR_PLAY_STATE_SF == para.play_status)
	{
	  bmap_set_content_by_id(p_img_ctrl, IM_INFORMATION_FF_1);
		switch(para.trick_speed.speed)
		{
		case MUL_PVR_PLAY_SPEED_2X_SLOW_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 1/2");
			break;
		case MUL_PVR_PLAY_SPEED_4X_SLOW_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 1/4");
			break;
		case MUL_PVR_PLAY_SPEED_8X_SLOW_FORWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 1/8");
			break;
		case MUL_PVR_PLAY_SPEED_NORMAL:
			break;
    default:
      break;
		}
	}
	if(MUL_PVR_PLAY_STATE_SB == para.play_status)
	{
	  bmap_set_content_by_id(p_img_ctrl, IM_INFORMATION_FB_1);
		switch(para.trick_speed.speed)
		{
		case MUL_PVR_PLAY_SPEED_8X_SLOW_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 1/8");
			break;
		case MUL_PVR_PLAY_SPEED_4X_SLOW_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 1/4");
			break;
		case MUL_PVR_PLAY_SPEED_2X_SLOW_BACKWARD:
			text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"x 1/2");
			break;
		case MUL_PVR_PLAY_SPEED_NORMAL:
			break;
		default:
      break;
		}
	}
	OS_PRINTF("AndyAndy=======state3:%d\n",para.play_status);
	if(MUL_PVR_PLAY_STATE_PLAY == para.play_status)
	{
	  bmap_set_content_by_id(p_img_ctrl, 0);
		text_set_content_by_ascstr(p_txt_ctrl, (u8 *)"");
	}
	ctrl_paint_ctrl(p_img_ctrl,  TRUE);
	ctrl_paint_ctrl(p_txt_ctrl,  TRUE);
	return SUCCESS;
}

static void timeshift_play_paint_icon(control_t *p_ctrl, u8 idc, u16 bmp_id)
{
	control_t *p_subctrl;

	p_subctrl = ctrl_get_child_by_id(p_ctrl,  idc);
	bmap_set_content_by_id(p_subctrl,  bmp_id);
	ctrl_paint_ctrl(p_subctrl,  TRUE);
}

void ui_tshift_show_bar(control_t *p_ctrl)
{
	control_t *p_list_cont;
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);


	p_list_cont = ctrl_get_child_by_id(p_ctrl, IDC_TIMESHIFT_BAR_FRAME);

	ctrl_set_sts(p_list_cont, OBJ_STS_SHOW);
	ctrl_paint_ctrl(p_list_cont, TRUE);

  g_ui_ts_info.b_showbar = TRUE;

  OS_PRINTF("@@## reset tmr\n");
  if(fw_tmr_reset(ROOT_ID_TIMESHIFT_BAR, MSG_HIDE_BAR, osd_set.timeout * 1000) != SUCCESS)
  {
    OS_PRINTF("@@## failed, create again\n");
    fw_tmr_create(ROOT_ID_TIMESHIFT_BAR, MSG_HIDE_BAR, osd_set.timeout * 1000, FALSE);
  }

}

//lint -e539 -e830
static RET_CODE timeshift_update_sm(u16 msg)
{
	mul_pvr_play_status_t play_status = {MUL_PVR_PLAY_STATE_INVALID, };
   mul_pvr_play_position_t position = {MUL_PVR_PLAY_POS_TYPE_SIZE, };
	mul_pvr_rec_status_t rec_status = {MUL_PVR_REC_STATE_INVALID, };
	mul_av_play_stop_opt_t stop_para = {0};
	u32 seek_time = 0;

  if(INVALID != g_ui_ts_info.h_play)
  {
  	if(mul_pvr_play_get_status ((u32)g_ui_ts_info.h_play, &play_status))
  	{
  		MT_ASSERT(0);
  	}
  }

  switch(msg)
  {
  	case MSG_TS_PLAY_PAUSE:
  	  switch(play_status.state)
  	  {
        case MUL_PVR_PLAY_STATE_INVALID:
        case MUL_PVR_PLAY_STATE_STOP:
  			  /*play_status.state = MUL_PVR_PLAY_STATE_PAUSE;
          ui_tshift_stream_seek();
        	ui_tshift_pause(g_ui_ts_info.h_play);*/
        	OS_PRINTF("\n###pvr play state error !!\n");
          break;

        case MUL_PVR_PLAY_STATE_PAUSE:
          play_status.state = MUL_PVR_PLAY_STATE_PLAY;

    		  ui_tshift_resume((u32)g_ui_ts_info.h_play);
          break;

        case MUL_PVR_PLAY_STATE_PLAY:
          play_status.state = MUL_PVR_PLAY_STATE_PAUSE;

          ui_tshift_pause((u32)g_ui_ts_info.h_play);
          break;

        case MUL_PVR_PLAY_STATE_FF:
        case MUL_PVR_PLAY_STATE_FB:
        case MUL_PVR_PLAY_STATE_SF:
        case MUL_PVR_PLAY_STATE_SB:
          play_status.state = MUL_PVR_PLAY_STATE_PLAY;

          g_tshift_status.trick_speed.speed = MUL_PVR_PLAY_SPEED_NORMAL;
          ui_tshift_play_mode((u32)g_ui_ts_info.h_play, &g_tshift_status.trick_speed);
          ui_set_mute(ui_is_mute());
          break;

        default:
          break;
  	  }
  		break;

  	case MSG_FOCUS_LEFT:
    case MSG_FOCUS_RIGHT:
      if(MUL_PVR_PLAY_STATE_PAUSE == play_status.state)
      {
  		  if(g_is_pause_trick == FALSE)
  		  {
           g_seek_delta = (s32)(g_tshift_opsition.play_curn - g_tshift_opsition.rec_start);
  		  }
  		  g_is_pause_trick = TRUE;

        if(msg == MSG_FOCUS_LEFT)
        {
          g_seek_delta -= g_ui_ts_info.jump_time;

          if(g_seek_delta < 0)
          {
            g_seek_delta = 0;
          }
        }
        else if(msg == MSG_FOCUS_RIGHT)
        {
          g_seek_delta += g_ui_ts_info.jump_time;

          if(g_seek_delta > (s32)(g_tshift_opsition.rec_curn - g_tshift_opsition.rec_start))
          {
            g_seek_delta = (s32)(g_tshift_opsition.rec_curn - g_tshift_opsition.rec_start);
          }
        }
  	   }
      else if(MUL_PVR_PLAY_STATE_PLAY == play_status.state)
      {
        play_status.state = MUL_PVR_PLAY_STATE_PAUSE;
        ui_tshift_pause((u32)g_ui_ts_info.h_play);
        //redo left and right operation
        if(g_is_pause_trick == FALSE)
        {
          g_seek_delta = (s32)(g_tshift_opsition.play_curn - g_tshift_opsition.rec_start);
        }

        g_is_pause_trick = TRUE;

        if(msg == MSG_FOCUS_LEFT)
        {
          g_seek_delta -= g_ui_ts_info.jump_time;

        if(g_seek_delta < 0)
        {
          g_seek_delta = 0;
        }
        }
        else if(msg == MSG_FOCUS_RIGHT)
        {
          g_seek_delta += g_ui_ts_info.jump_time;
          if(g_seek_delta > (s32)(g_tshift_opsition.rec_curn - g_tshift_opsition.rec_start))
          {
            g_seek_delta = (s32)(g_tshift_opsition.rec_curn - g_tshift_opsition.rec_start);
          }
        }
      }
      else if(MUL_PVR_PLAY_STATE_INVALID == play_status.state||MUL_PVR_PLAY_STATE_STOP == play_status.state)
      {
        play_status.state = MUL_PVR_PLAY_STATE_PAUSE;
        ui_tshift_stream_seek();
        ui_tshift_pause((u32)g_ui_ts_info.h_play);
      }
  	   break;

  	case MSG_OK:
      if((MUL_PVR_PLAY_STATE_PAUSE == play_status.state) && (g_is_pause_trick))
      {
        play_status.state = MUL_PVR_PLAY_STATE_PLAY;
        g_is_pause_trick = FALSE;

        seek_time = (g_tshift_opsition.rec_start + (u32)g_seek_delta);

        if(seek_time < g_tshift_opsition.rec_start)
        {
          seek_time = g_tshift_opsition.rec_start;
        }
        else if(seek_time > g_tshift_opsition.rec_curn)
        {
          seek_time = g_tshift_opsition.rec_curn;
        }

        position.whence = MUL_PVR_SEEK_SET;
        position.pos_type = MUL_PVR_PLAY_POS_TYPE_TIME;
        position.offset = (s32)seek_time * 1000;
        ui_tshift_seek((u32)g_ui_ts_info.h_play, &position);
        g_tshift_status.play_status = MUL_PVR_PLAY_STATE_PLAY;

        g_seek_delta = 0;
      }
  	   break;



    case MSG_TS_BEGIN:
      if((g_tshift_status.play_status == MUL_PVR_PLAY_STATE_PLAY))
      {
      	g_tshift_status.seek_info.whence = MUL_PVR_SEEK_SET;
      	g_tshift_status.seek_info.pos_type = MUL_PVR_PLAY_POS_TYPE_TIME;
      	g_tshift_status.seek_info.offset = 0;
      	ui_tshift_seek((u32)g_ui_ts_info.h_play, &g_tshift_status.seek_info);
        ui_set_mute(ui_is_mute());

        g_tshift_opsition.play_size = 0;
      }
      else if(g_tshift_status.play_status == MUL_PVR_PLAY_STATE_PAUSE)
      {
        g_is_pause_trick = TRUE;

        g_seek_delta = 0;
      }
      break;

    case MSG_TS_END:
      if((g_tshift_status.play_status == MUL_PVR_PLAY_STATE_PLAY))
      {
      	g_tshift_status.seek_info.whence = MUL_PVR_SEEK_END;
      	g_tshift_status.seek_info.pos_type = MUL_PVR_PLAY_POS_TYPE_SIZE;
      	g_tshift_status.seek_info.offset = 0;
      	ui_tshift_seek((u32)g_ui_ts_info.h_play, &g_tshift_status.seek_info);
        ui_set_mute(ui_is_mute());

        mul_pvr_rec_get_status ((u32)g_ui_ts_info.h_rec, &rec_status);

        g_tshift_opsition.play_size = rec_status.cur_write_point;

      }
      else if(g_tshift_status.play_status == MUL_PVR_PLAY_STATE_PAUSE)
      {
        g_is_pause_trick = TRUE;

        g_seek_delta = (s32)(g_tshift_opsition.rec_curn - g_tshift_opsition.rec_start);
      }
      break;

  	case MSG_TS_PLAY_STOP:
  		play_status.state = MUL_PVR_PLAY_STATE_STOP;

      //stop timeshift play.
    	stop_para.timeout_ms = 0;
    	stop_para.enMode = MUL_AVPLAY_STOP_MODE_BLACK;
    	ui_tshift_stop((u32)g_ui_ts_info.h_play, INVALIDID, &stop_para);

    	g_ui_ts_info.h_play = INVALID;

    	g_tshift_opsition.play_curn = 0;
    	g_tshift_opsition.play_size = 0;

      g_seek_delta = 0;
      g_is_pause_trick = FALSE;

      //start play.
      ui_play_curn_pg();
  		break;

  	case MSG_TS_PLAY_FF:
      if((MUL_PVR_PLAY_STATE_FF == play_status.state) ||
        (MUL_PVR_PLAY_STATE_PLAY == play_status.state))
  		{
  			play_status.state = MUL_PVR_PLAY_STATE_FF;
  			if(MUL_PVR_PLAY_SPEED_32X_FAST_FORWARD == play_status.speed)
  			{
  			  play_status.state = MUL_PVR_PLAY_STATE_PLAY;
  				play_status.speed= MUL_PVR_PLAY_SPEED_NORMAL;
  			}
  			else if(MUL_PVR_PLAY_SPEED_NORMAL == play_status.speed)
  			{
  				play_status.speed = MUL_PVR_PLAY_SPEED_2X_FAST_FORWARD;
  			}
  			else
  			{
  				play_status.speed++;
  			}
        g_tshift_status.trick_speed.speed = play_status.speed;
        ui_tshift_play_mode((u32)g_ui_ts_info.h_play, &g_tshift_status.trick_speed);
  		}
  	   else if(MUL_PVR_PLAY_STATE_FB == play_status.state)
       {
        play_status.state = MUL_PVR_PLAY_STATE_FF;
        play_status.speed = MUL_PVR_PLAY_SPEED_2X_FAST_FORWARD;
        g_tshift_status.trick_speed.speed = play_status.speed;
        ui_tshift_play_mode((u32)g_ui_ts_info.h_play, &g_tshift_status.trick_speed);
       }
  		break;

  	case MSG_TS_PLAY_FB:
      if((MUL_PVR_PLAY_STATE_FB == play_status.state) ||
        (MUL_PVR_PLAY_STATE_PLAY == play_status.state))
  		{
  			play_status.state = MUL_PVR_PLAY_STATE_FB;
  			if(MUL_PVR_PLAY_SPEED_32X_FAST_BACKWARD == play_status.speed)
  			{
  			  play_status.state = MUL_PVR_PLAY_STATE_PLAY;
  				play_status.speed = MUL_PVR_PLAY_SPEED_NORMAL;
  			}
  			else if(MUL_PVR_PLAY_SPEED_NORMAL == play_status.speed)
  			{
  				play_status.speed = MUL_PVR_PLAY_SPEED_2X_FAST_BACKWARD;
  			}
  			else
  			{
  				play_status.speed++;
  			}
        g_tshift_status.trick_speed.speed = play_status.speed;
        ui_tshift_play_mode((u32)g_ui_ts_info.h_play, &g_tshift_status.trick_speed);
  		}
  	else if(MUL_PVR_PLAY_STATE_FF == play_status.state)
    {
      play_status.state = MUL_PVR_PLAY_STATE_FB;
      play_status.speed = MUL_PVR_PLAY_SPEED_2X_FAST_BACKWARD;
      g_tshift_status.trick_speed.speed = play_status.speed;
      ui_tshift_play_mode((u32)g_ui_ts_info.h_play, &g_tshift_status.trick_speed);
    }
    break;

  	case MSG_TS_PLAY_SF:
        #if 0
      if((MUL_PVR_PLAY_STATE_SF == play_status.state) ||
        (MUL_PVR_PLAY_STATE_PLAY == play_status.state))
  		{
  			play_status.state = MUL_PVR_PLAY_STATE_SF;
  			if(MUL_PVR_PLAY_SPEED_8X_SLOW_FORWARD == play_status.speed)
  			{
    			play_status.state = MUL_PVR_PLAY_STATE_PLAY;
  				play_status.speed = MUL_PVR_PLAY_SPEED_NORMAL;
  			}
  			else if(MUL_PVR_PLAY_SPEED_NORMAL == play_status.speed)
  			{
  				play_status.speed = MUL_PVR_PLAY_SPEED_2X_SLOW_FORWARD;
  			}
  			else
  			{
  				play_status.speed++;
  			}
        g_tshift_status.trick_speed.speed = play_status.speed;
        ui_tshift_play_mode(g_ui_ts_info.h_play, &g_tshift_status.trick_speed);
  		}
      #endif
  		break;

  	case MSG_TS_PLAY_SB:
  	  #if 0
      if((MUL_PVR_PLAY_STATE_SB == play_status.state) ||
        (MUL_PVR_PLAY_STATE_PLAY == play_status.state))
  		{
  			play_status.state = MUL_PVR_PLAY_STATE_SB;
  			if(MUL_PVR_PLAY_SPEED_8X_SLOW_BACKWARD == play_status.speed)
  			{
    			play_status.state = MUL_PVR_PLAY_STATE_PLAY;
  				play_status.speed = MUL_PVR_PLAY_SPEED_NORMAL;
  			}
  			else if(MUL_PVR_PLAY_SPEED_NORMAL == play_status.speed)
  			{
  				play_status.speed = MUL_PVR_PLAY_SPEED_2X_SLOW_BACKWARD;
  			}
  			else
  			{
  				play_status.speed++;
  			}
        g_tshift_status.trick_speed.speed = play_status.speed;
        ui_tshift_play_mode(g_ui_ts_info.h_play, &g_tshift_status.trick_speed);
  		}
  		#endif
  		break;

  	default:
  		MT_ASSERT(0);
  		break;
  }

  g_tshift_status.play_status = play_status.state;

	return SUCCESS;

}

static RET_CODE timeshift_update_ui(control_t *p_ctrl)
{
	control_t *p_bar_ctrl = NULL, *p_trick = NULL, *p_time = NULL, *p_point = NULL;
	rect_t rect = {0};
	u8 ctrl_child_id = 0;
	u16 bmp_id = 0;
	u16 center = 0, new_center = 0;
	utc_time_t play_time = {0};

	p_bar_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_TIMESHIFT_BAR_FRAME);

	switch(g_tshift_status.play_status)
	{
  	case MUL_PVR_PLAY_STATE_PLAY:
  	  ctrl_child_id = IDC_TIMESHIFT_PLAY_STAT_ICON;
  		bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
  		break;

  	case MUL_PVR_PLAY_STATE_PAUSE:
      ctrl_child_id = IDC_TIMESHIFT_PAUSE_STAT_ICON;
			bmp_id = IM_MP3_ICON_PAUSE_SELECT;
	    break;

  	case MUL_PVR_PLAY_STATE_FF:
			ctrl_child_id = IDC_TIMESHIFT_FF_STAT_ICON;
			bmp_id = IM_MP3_ICON_FF_SELECT_V2;
			if(MUL_PVR_PLAY_SPEED_NORMAL == g_tshift_status.trick_speed.speed)
			{
			  bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
			  ctrl_child_id = IDC_TIMESHIFT_PLAY_STAT_ICON;
			}
	    break;

  	case MUL_PVR_PLAY_STATE_FB:
			ctrl_child_id = IDC_TIMESHIFT_FB_STAT_ICON;
			bmp_id = IM_MP3_ICON_FB_SELECT_V2;
			if(MUL_PVR_PLAY_SPEED_NORMAL == g_tshift_status.trick_speed.speed)
			{
			  bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
			  ctrl_child_id = IDC_TIMESHIFT_PLAY_STAT_ICON;
			}
	    break;

  	case MUL_PVR_PLAY_STATE_SF:
        #if 0
			ctrl_child_id = IDC_TIMESHIFT_SF_STAT_ICON;
			bmp_id = IM_MP3_ICON_SF_SELECT;
			if(MUL_PVR_PLAY_SPEED_NORMAL == g_tshift_status.trick_speed.speed)
			{
  			bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
			  ctrl_child_id = IDC_TIMESHIFT_PLAY_STAT_ICON;
			}
            #endif
	    break;

  	case MUL_PVR_PLAY_STATE_SB:
        #if 0
			ctrl_child_id = IDC_TIMESHIFT_SB_STAT_ICON;
			bmp_id = IM_MP3_ICON_SB_SELECT;
			if(MUL_PVR_PLAY_SPEED_NORMAL == g_tshift_status.trick_speed.speed)
			{
  			bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
			  ctrl_child_id = IDC_TIMESHIFT_PLAY_STAT_ICON;
			}
            #endif
	    break;

    default:
  	  break;
	}

	if(bmp_id != 0 && ctrl_child_id != 0)
	{
		timeshift_in_bar_back(p_bar_ctrl);
		timeshift_play_paint_icon(p_bar_ctrl, ctrl_child_id, bmp_id);
		timeshift_fast_slow_display(p_ctrl, g_tshift_status);
		ui_tshift_show_bar(p_ctrl);
	}


  //draw trick flag below the pbar.
  p_trick = ctrl_get_child_by_id(p_bar_ctrl, IDC_TSFT_TIRCK);
  p_time = ctrl_get_child_by_id(p_bar_ctrl, IDC_TIMESHIFT_PLAY_TIME_TXT);
  p_point = ctrl_get_child_by_id(p_bar_ctrl, IDC_TIMESHIFT_PLAY_PROGRESS);

	if(g_tshift_status.play_status == MUL_PVR_PLAY_STATE_PAUSE)
	{
    if(g_is_pause_trick)
    {
      ctrl_set_sts(p_trick, OBJ_STS_SHOW);

      ctrl_get_frame(p_trick, &rect);

      center = (u16)(rect.left + rect.right )/2;

      if(g_tshift_opsition.rec_start == 0)
      {
        if(g_ui_ts_info.data_size != 0)
        {
          new_center = (u16)(g_tshift_opsition.rec_size * TIMESHIFT_BAR_REC_PROGRESS_W / (g_ui_ts_info.data_size / 1024));
        }
      }
      else
      {
        new_center = TIMESHIFT_BAR_REC_PROGRESS_W;
      }

      if(g_tshift_opsition.rec_curn - g_tshift_opsition.rec_start != 0)
      {
        new_center = (u16)((new_center * (u32)g_seek_delta) /(g_tshift_opsition.rec_curn - g_tshift_opsition.rec_start));
      }

      new_center += TIMESHIFT_BAR_REC_PROGRESS_X;

      ctrl_empty_invrgn(p_trick);

      ctrl_move_ctrl(p_trick, (s16)(new_center - center), 0);
    }
  	 time_up(&play_time, g_tshift_opsition.play_curn);

  	 timeshift_bar_play_move(p_bar_ctrl, play_time, FALSE);
	}
	else
	{
    ctrl_set_sts(p_trick, OBJ_STS_HIDE);
	}

	//handle stop state.
	if(g_tshift_status.play_status == MUL_PVR_PLAY_STATE_STOP)
	{
    ctrl_set_sts(p_trick, OBJ_STS_HIDE);
    ctrl_set_sts(p_time, OBJ_STS_HIDE);
    ctrl_set_sts(p_point, OBJ_STS_HIDE);
	}

  // Draw cursor
	ctrl_paint_ctrl(p_bar_ctrl, TRUE);

	return SUCCESS;
}

static RET_CODE on_timeshift_state_change(control_t *p_ctrl, u16 msg,
																							      u32 para1, u32 para2)
{
  if(fw_find_root_by_id(ROOT_ID_ZOOM) != NULL)
  {
    manage_close_menu(ROOT_ID_ZOOM, 0, 0);
  }

  if(!g_ui_ts_info.b_showbar)
  {
    if(msg == MSG_FOCUS_LEFT)
    {
      return manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_TIMESHIFT_BAR, V_KEY_LEFT);
    }
    else if(msg == MSG_FOCUS_RIGHT)
    {
      return manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_TIMESHIFT_BAR, V_KEY_RIGHT);
    }
  }
  if(msg == MSG_TS_VOLDOWN || msg == MSG_TS_VOLUP)
  {
  	return manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_TIMESHIFT_BAR,\
			(msg == MSG_TS_VOLDOWN)? V_KEY_VDOWN : V_KEY_VUP);
  }
	
  timeshift_update_sm(msg);
  timeshift_update_ui(p_ctrl);

	return SUCCESS;
}

static RET_CODE on_time_update(control_t *p_ctrl, u16 msg,
															 u32 para1, u32 para2)
{
	timeshift_fill_time(p_ctrl, TRUE);
	return SUCCESS;
}

static RET_CODE on_timeshift_bar_infokey(control_t *p_mainwin_ctrl, u16 msg,
																				 u32 para1, u32 para2)
{
  ui_tshift_show_bar(p_mainwin_ctrl);

	return SUCCESS;
}

static RET_CODE on_hide_timeshift_bar(control_t *p_mainwin_ctrl, u16 msg,
																				 u32 para1, u32 para2)
{
  control_t *p_frm_cont = NULL;
	p_frm_cont = ctrl_get_child_by_id(p_mainwin_ctrl, IDC_TIMESHIFT_BAR_FRAME);

  ctrl_set_sts(p_frm_cont, OBJ_STS_HIDE);
  ctrl_erase_ctrl(p_frm_cont);
  g_ui_ts_info.b_showbar = FALSE;

  OS_PRINTF("@@## hide bar\n");
	return SUCCESS;
}

static RET_CODE on_timeshift_bar_destroy(control_t *p_mainwin_ctrl, u16 msg,
																				 u32 para1, u32 para2)
{
  mul_av_play_stop_opt_t stop_para;

  stop_para.timeout_ms = 0;
  stop_para.enMode = MUL_AVPLAY_STOP_MODE_BLACK;
  ui_tshift_stop((u32)g_ui_ts_info.h_play, (u32)g_ui_ts_info.h_rec, &stop_para);
  g_ui_ts_info.h_rec = INVALID;
  g_ui_ts_info.h_play = INVALID;

  memset(&g_tshift_opsition, 0, sizeof(g_tshift_opsition));

  g_tshift_status.play_status = MUL_PVR_PLAY_STATE_INVALID;
  g_timeshift_start = FALSE;
  g_seek_delta = 0;
  g_is_pause_trick = FALSE;
  ui_tshift_deinit();
#ifdef EXTERN_CA_PVR
  cas_manager_set_timeshift_play(FALSE);
#endif

  ui_play_curn_pg();//hack hack
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  ui_play_curn_pg();

  return ERR_NOFEATURE;
}

static void timeshift_exit_to_main(void)
{
    ui_close_all_mennus();
    manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_MAINMENU, 0, 0);
}


static RET_CODE on_timeshift_bar_exit_to_menu(control_t *p_ctrl, u16 msg,
																							u32 para1, u32 para2)
{
  ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_TIMESHIFT, timeshift_exit_to_main, NULL, 0);

	return SUCCESS;
}

static void timeshift_exit(void)
{ 
   ui_close_all_mennus();
}

static RET_CODE on_timeshift_bar_exit(control_t *p_ctrl, u16 msg,
																							u32 para1, u32 para2)
{
  ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_TIMESHIFT, timeshift_exit, NULL, 0);

	return SUCCESS;
}

static RET_CODE on_timeshift_plug_out(control_t *p_ctrl, u16 msg,
																							u32 para1, u32 para2)
{
  timeshift_exit();

	return SUCCESS;
}

static RET_CODE on_timeshift_play_updated(control_t *p_cont, u16 msg,
																					u32 para1, u32 para2)//
{
  control_t *p_bar_cont = NULL;
  utc_time_t play_time = {0};
	mul_pvr_play_status_t *p_ts_play = (mul_pvr_play_status_t*) para2;

	p_bar_cont = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_BAR_FRAME);
	MT_ASSERT(p_bar_cont != NULL);

	g_tshift_opsition.play_curn = p_ts_play->cur_play_time_ms / 1000;
	g_tshift_opsition.play_size = p_ts_play->cur_play_pos;
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    {
      ui_pvr_extern_t *p_pvr_extern = NULL;
      u8 num = 0;
      u8 i = 0;
      OS_PRINTF("g_tshift_opsition.play_curn = %d \n",g_tshift_opsition.play_curn);
      num = ui_pvr_extern_get_msg_num(g_tshift_opsition.play_curn);
      
      OS_PRINTF("#times = %d \n", num);

      
      for(i = 0;i < num;i ++)
      {
        p_pvr_extern = ui_pvr_extern_read(g_tshift_opsition.play_curn, i);
        if (p_pvr_extern)
        {
          OS_PRINTF("\n##ui_pvr_extern_read [%lu, %lu, %lu][%s]!!\n",
          p_pvr_extern->rec_time, p_pvr_extern->type, p_pvr_extern->extern_data_len, p_pvr_extern->p_extern_data);

          if(g_tshift_opsition.play_curn == p_pvr_extern->rec_time)
          {
            switch(p_pvr_extern->type)
            {
              case UI_PVR_FINGER_PRINT:
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_FINGER_PRINT, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              case UI_PVR_ECM_FINGER_PRINT:
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_ECM_FINGER_PRINT, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              case UI_PVR_HIDE_ECM_FINGER_PRINT:
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_HIDE_ECM_FINGER_PRINT, 0, 0);
                break;
              case UI_PVR_OSD:
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_OSD, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              case UI_PVR_OSD_HIDE:
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_OSD_HIDE, 0, (u32)p_pvr_extern->p_extern_data);
                break;
         case UI_PVR_SUPER_OSD_HIDE:
                ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_PVR_SUPER_OSD_HIDE, 0, (u32)p_pvr_extern->p_extern_data);
                break;
              default:
                break;
            }
          }
        }
      }
    }
#endif
	time_up(&play_time, g_tshift_opsition.play_curn);
  //OS_PRINTF("Andy---play_curn%d,play_size:%d\n",g_tshift_opsition.play_curn,g_tshift_opsition.play_size);
	timeshift_bar_play_move(p_bar_cont, play_time, FALSE);

	return SUCCESS;
}

static RET_CODE on_timeshift_rec_updated(control_t *p_cont, u16 msg,
																				 u32 para1, u32 para2)
{
	control_t *p_bar_cont = NULL, *p_rec_progress = NULL;

	mul_pvr_rec_status_t *p_ts_rec = (mul_pvr_rec_status_t*) para2;
	utc_time_t play_time = {0};
	u16 pbar_curn = 0;

	p_bar_cont = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_BAR_FRAME);
	MT_ASSERT(p_bar_cont != NULL);

	p_rec_progress = ctrl_get_child_by_id(p_bar_cont, IDC_TIMESHIFT_REC_PROGRESS);
	MT_ASSERT(p_rec_progress != NULL);

	g_tshift_opsition.rec_size = p_ts_rec->cur_write_point;

  if(g_ui_ts_info.data_size != 0)
  {
	  pbar_curn = (u16)(g_tshift_opsition.rec_size * TIMESHIFT_BAR_REC_PROGRESS_STEP / (g_ui_ts_info.data_size / 1024));
  }

	if(pbar_curn)
	{
		pbar_set_current(p_rec_progress, pbar_curn);

		if(g_ui_ts_info.b_showbar)
		{
      ctrl_paint_ctrl(p_rec_progress, TRUE);
		}
	}
	g_tshift_opsition.rec_curn = p_ts_rec->end_time_ms/1000;
  g_tshift_opsition.rec_start = p_ts_rec->start_time_ms/1000;

  if(g_tshift_opsition.rec_start > g_tshift_opsition.play_curn)
  {
    g_tshift_opsition.play_curn = g_tshift_opsition.rec_start;
  }
  timeshift_fill_rec_time_info(p_bar_cont, p_ts_rec);
 // OS_PRINTF("Andy---rec_curn%d,rec_start:%d,rec_size%d\n",
  //  g_tshift_opsition.play_curn,g_tshift_opsition.rec_start,g_tshift_opsition.rec_size);
  if(MUL_PVR_PLAY_STATE_PAUSE == g_tshift_status.play_status)
  {
  	time_up(&play_time, g_tshift_opsition.play_curn);

  	timeshift_bar_play_move(p_bar_cont, play_time, FALSE);
  }
  return SUCCESS;
}
//lint +e539 +e830

static RET_CODE on_timeshift_play_sof(control_t *p_cont, u16 msg,
																					u32 para1, u32 para2)
{
  control_t *p_bar_ctrl = NULL, *p_img_ctrl = NULL, *p_txt_ctrl = NULL;

  g_tshift_status.play_status = MUL_PVR_PLAY_STATE_PLAY;
  g_tshift_status.trick_speed.speed = MUL_PVR_PLAY_SPEED_NORMAL;
  p_bar_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_BAR_FRAME);
 	p_img_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_SPECIAL_PLAY_IMG_FRAME);
  p_txt_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_SPECIAL_PLAY_TXT_FRAME);

  timeshift_in_bar_back(p_bar_ctrl);
  timeshift_play_paint_icon(p_bar_ctrl, IDC_TIMESHIFT_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2_SELECT);
  timeshift_fast_slow_display(p_cont, g_tshift_status);
  ui_tshift_show_bar(p_cont);  
	ctrl_erase_ctrl(p_img_ctrl);
  ctrl_erase_ctrl(p_txt_ctrl);

  return SUCCESS;
}

static RET_CODE on_timeshift_uio_event(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  osd_set_t osd_set = {0};

  sys_status_get_osd_set(&osd_set);

  OS_PRINTF("@@## uio reset tmr\n");
  fw_tmr_reset(ROOT_ID_TIMESHIFT_BAR, MSG_HIDE_BAR, osd_set.timeout * 1000);

  return SUCCESS;
}

static RET_CODE on_mute(control_t *p_ctrl, u16 msg,
                        u32 para1, u32 para2)
{
  u8 index;

  mem_user_dbg_info_t info;

  mtos_mem_user_debug(&info);
  APPLOGI("memory cost alloced 0x%x , peak 0x%x \n",info.alloced,info.alloced_peak);

  index = fw_get_focus_id();
  if(index != ROOT_ID_DO_SEARCH && index != ROOT_ID_MAINMENU)
  {
    ui_set_mute((BOOL)!ui_is_mute());
  }

  return SUCCESS;
}

static RET_CODE on_zoom(control_t *p_mainwin_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  control_t *p_frm_cont = NULL;
	p_frm_cont = ctrl_get_child_by_id(p_mainwin_ctrl, IDC_TIMESHIFT_BAR_FRAME);

  ctrl_set_sts(p_frm_cont, OBJ_STS_HIDE);
  ctrl_erase_ctrl(p_frm_cont);
  g_ui_ts_info.b_showbar = FALSE;
  return manage_open_menu(ROOT_ID_ZOOM, para1, 0);
}

static RET_CODE on_timeshift_play_reach_rec(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{
  control_t *p_bar_ctrl = NULL,*p_img_ctrl = NULL,*p_txt_ctrl = NULL;

  if((MUL_PVR_PLAY_STATE_FF == g_tshift_status.play_status) ||
    (MUL_PVR_PLAY_STATE_SF == g_tshift_status.play_status))
  {
  	p_bar_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_BAR_FRAME);
    p_img_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_SPECIAL_PLAY_IMG_FRAME);
    p_txt_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_SPECIAL_PLAY_TXT_FRAME);

    g_tshift_status.play_status = MUL_PVR_PLAY_STATE_PLAY;
    g_tshift_status.trick_speed.speed = MUL_PVR_PLAY_SPEED_NORMAL;
    timeshift_in_bar_back(p_bar_ctrl);
    timeshift_play_paint_icon(p_bar_ctrl, IDC_TIMESHIFT_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2_SELECT);
    timeshift_fast_slow_display(p_cont, g_tshift_status);
    ui_tshift_show_bar(p_cont);
  	ui_tshift_play_mode((u32)g_ui_ts_info.h_play, &g_tshift_status.trick_speed);

    ctrl_erase_ctrl(p_img_ctrl);
    ctrl_erase_ctrl(p_txt_ctrl);
  }

  return SUCCESS;
}

static RET_CODE on_timeshift_lock(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_bar_ctrl = NULL;
  p_bar_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_TS_NO_SINGAL_FRAME);
  ctrl_set_sts(p_bar_ctrl, OBJ_STS_HIDE);
  ctrl_erase_ctrl(p_bar_ctrl);
  OS_PRINTF("@@## timeshift lock!\n");
  return SUCCESS;
}

static RET_CODE on_timeshift_unlock(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_bar_ctrl = NULL;
  p_bar_ctrl = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT_TS_NO_SINGAL_FRAME);
  ctrl_set_sts(p_bar_ctrl, OBJ_STS_SHOW);
  bmap_set_content_by_id(p_bar_ctrl, IM_INFORMATION_NOSIGNAL);
  ctrl_paint_ctrl(p_bar_ctrl, TRUE);
  OS_PRINTF("@@## timeshift unlock!\n");

  return SUCCESS;
}

ui_pvr_private_info_t * ui_timeshift_para_get(void)
{
	return &g_ui_ts_info;
}

BOOL ui_timeshift_switch_get(void)
{
    return g_timeshift_start;
}

u32 ui_timeshift_get_rec_time(void)
{
  if(g_tshift_opsition.rec_curn == 0)
    return 1;
  else
    return g_tshift_opsition.rec_curn;
}

BEGIN_KEYMAP(timeshift_bar_mainwin_keymap, NULL)
  ON_EVENT(V_KEY_MUTE, MSG_MUTE)
  ON_EVENT(V_KEY_ZOOM, MSG_ZOOM)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_BACK, MSG_EXIT_ALL)
  //ON_EVENT(V_KEY_TVRADIO, MSG_INFO)
  ON_EVENT(V_KEY_MENU, MSG_EXIT_TO_MENU)
  ON_EVENT(V_KEY_PLAY, MSG_TS_PLAY_PAUSE)
  ON_EVENT(V_KEY_PAUSE, MSG_TS_PLAY_PAUSE)
  ON_EVENT(V_KEY_FORW2, MSG_TS_PLAY_FF)
  ON_EVENT(V_KEY_BACK2, MSG_TS_PLAY_FB)
  //ON_EVENT(V_KEY_SLOW, MSG_TS_PLAY_SF)
  ON_EVENT(V_KEY_INFO, MSG_INFO)
  ON_EVENT(V_KEY_STOP, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_RED, MSG_TS_BEGIN)
  ON_EVENT(V_KEY_GREEN, MSG_TS_END)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_OK)
  ON_EVENT(V_KEY_VDOWN, MSG_TS_VOLDOWN)
  ON_EVENT(V_KEY_VUP, MSG_TS_VOLUP)
END_KEYMAP(timeshift_bar_mainwin_keymap, NULL)

BEGIN_MSGPROC(timeshift_bar_mainwin_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_MUTE, on_mute)
  ON_COMMAND(MSG_ZOOM, on_zoom)
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
  ON_COMMAND(MSG_INFO, on_timeshift_bar_infokey)
  ON_COMMAND(MSG_HIDE_BAR, on_hide_timeshift_bar)
  ON_COMMAND(MSG_DESTROY, on_timeshift_bar_destroy)
  //ON_COMMAND(MSG_TIMESHIFT_EVT_TIME_END, on_timeshift_bar_play_end)
  //ON_COMMAND(MSG_TIMESHIFT_EVT_READY_TO_PLAY, on_timeshift_bar_ready_play)
  ON_COMMAND(MSG_PLUG_OUT, on_timeshift_plug_out)
  ON_COMMAND(MSG_EXIT_TO_MENU, on_timeshift_bar_exit_to_menu)
  ON_COMMAND(MSG_EXIT_ALL, on_timeshift_bar_exit)
  ON_COMMAND(MSG_TS_PLAY_PAUSE, on_timeshift_state_change)
  ON_COMMAND(MSG_TS_PLAY_FF, on_timeshift_state_change)
  ON_COMMAND(MSG_TS_PLAY_FB, on_timeshift_state_change)
  //ON_COMMAND(MSG_TS_PLAY_SF, on_timeshift_state_change)
  //ON_COMMAND(MSG_TS_PLAY_SB, on_timeshift_state_change)
  ON_COMMAND(MSG_TS_PLAY_STOP, on_timeshift_state_change)
  ON_COMMAND(MSG_FOCUS_LEFT, on_timeshift_state_change)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_timeshift_state_change)
  ON_COMMAND(MSG_TS_BEGIN, on_timeshift_state_change)
  ON_COMMAND(MSG_TS_END, on_timeshift_state_change)
  ON_COMMAND(MSG_OK, on_timeshift_state_change)
  ON_COMMAND(MSG_TS_VOLDOWN, on_timeshift_state_change)
  ON_COMMAND(MSG_TS_VOLUP, on_timeshift_state_change)
  ON_COMMAND(MSG_SIGNAL_LOCK, on_timeshift_lock)
  ON_COMMAND(MSG_SIGNAL_UNLOCK, on_timeshift_unlock)
  ////////////////////////
  ON_COMMAND(MSG_TIMESHIFT_EVT_PLAY_REACH_REC, on_timeshift_play_reach_rec)
  ON_COMMAND(MSG_TIMESHIFT_EVT_PLAY_SOF, on_timeshift_play_sof)
  ON_COMMAND(MSG_TIMESHIFT_EVT_REC_UPDARED, on_timeshift_rec_updated)
  ON_COMMAND(MSG_TIMESHIFT_EVT_PLAY_UPDATED, on_timeshift_play_updated)
  ON_COMMAND(MSG_UIO_EVENT, on_timeshift_uio_event)
END_MSGPROC(timeshift_bar_mainwin_proc, ui_comm_root_proc)

#endif
