/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_nprog_bar.h"
#include "ui_pause.h"

#ifdef ENABLE_ADS
#include "ui_ads_display.h"
#include "ui_ad_api.h"
#endif

#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

enum control_id
{
  IDC_FRAME = 1,
};

enum frame_sub_control_id
{
  IDC_NICON = 1,
  IDC_NUMBER,
  IDC_NAME,
  IDC_DATE,
  IDC_TIME,
  IDC_EPG_P,
  IDC_EPG_F,
  IDC_EPG_P_DURATION,
  IDC_EPG_F_DURATION,
  IDC_ICON_AUDIO,
  IDC_ICON_MONEY,
  IDC_ICON_FAV,
  IDC_ICON_LOCK,
  IDC_ICON_SKIP,
  IDC_INFO_EPG,
  IDC_INFO_SUBTT,
  IDC_INFO_TELTEXT,
  IDC_GROUP,
  IDC_CA_INFO,
  IDC_TP_INFO,
  IDC_SIGNAL_NAME_Q,
  IDC_SIGNAL_NAME_S,
  IDC_SIGNAL_QUALITY,
  IDC_SIGNAL_INTENSITY,
  IDC_AD,
};

enum local_msg
{
  MSG_RECALL = MSG_LOCAL_BEGIN + 600,
  MSG_INFO,
  MSG_RECORD_PAUSE,
};
static u16 curn_prog_id = 0;
extern BOOL get_subt_description_1(class_handle_t handle, pb_subt_info_t *p_info, u16 pg_id);
u16 nprog_bar_cont_keymap(u16 key);

RET_CODE nprog_bar_cont_proc(control_t *cont, u16 msg,
                       u32 para1, u32 para2);

extern BOOL have_logic_number(void);

static BOOL fill_epg_info(dvbs_prog_node_t *p_pg, u16 pos,
  u16 *p_str, u8 *p_time_str, u32 len)
{
  epg_prog_info_t prog_info = {0};
  event_node_t   *p_evt_node = NULL;
  event_node_t   *p_present_evt = NULL;
  event_node_t   *p_follow_evt = NULL;
  utc_time_t start_time = {0},endtime = {0};
  //u8 str = " ";

  prog_info.network_id = (u16)p_pg->orig_net_id;
  prog_info.ts_id      = (u16)p_pg->ts_id;
  prog_info.svc_id     = (u16)p_pg->s_id;
       
  mul_epg_get_pf_event(&prog_info, &p_present_evt, &p_follow_evt);
  if((pos == 1) && (p_present_evt != NULL))
  {
    p_evt_node = p_present_evt;
  }
  else
    if((pos == 2) && (p_follow_evt != NULL))
    {
      p_evt_node = p_follow_evt;
    }

  if (p_evt_node != NULL)
  {
  time_to_local(&(p_evt_node->start_time), &start_time);
  memcpy(&endtime, &start_time, sizeof(utc_time_t));
  time_add(&endtime, &(p_evt_node->drt_time));
    uni_strncpy(p_str, p_evt_node->event_name, len);
    sprintf((char *)p_time_str, "%.2d:%.2d - %.2d:%.2d",start_time.hour,start_time.minute,\
		 endtime.hour,endtime.minute);
    return TRUE;
  }
  else
  {
    gui_get_string(IDS_MSG_NO_CONTENT, p_str, (u16)len);
    return FALSE;
  }

}

static RET_CODE on_pbar_signal_update(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
#ifdef PBAR_SIG_UPDATE
  struct signal_data *data = (struct signal_data *)(para1);
  control_t *p_frm, *p_quality, *p_intensity;

  p_frm = ctrl_get_child_by_id(p_ctrl, IDC_FRAME);
  p_quality = ctrl_get_child_by_id(p_frm, IDC_SIGNAL_QUALITY);
  p_intensity = ctrl_get_child_by_id(p_frm, IDC_SIGNAL_INTENSITY);
  
 pbar_set_rstyle(p_quality, data->lock?RSI_ITEM_GREEN_HL:RSI_ITEM_7_HL,
      RSI_IGNORE, INVALID_RSTYLE_IDX);
 pbar_set_rstyle(p_intensity, data->lock?RSI_ITEM_6_HL:RSI_ITEM_7_HL,
      RSI_IGNORE, INVALID_RSTYLE_IDX);
 pbar_set_current(p_quality, data->quality);

 pbar_set_current(p_intensity, data->intensity);

 ctrl_paint_ctrl(p_quality, TRUE);
 ctrl_paint_ctrl(p_intensity, TRUE);
#endif
  return SUCCESS;
}

/*static u16 get_audio_format_icon_id(u16 type)
{
  u16 icon_id;
  switch(type)
  {
    case AUDIO_AC3_VSB:
        icon_id = IM_INFORMATION_ICON_AC3;
        break;
     case AUDIO_EAC3:
        icon_id = IM_INFORMATION_ICON_EAC3;
        break;
     case AUDIO_AAC:
      icon_id = IM_INFORMATION_ICON_AAC;
      break;
     default:
      icon_id = IM_INFORMATION_ICON_MPEG;
      break;
     
  }
  return icon_id;
}

static u16 get_video_format_icon_id(u32 type)
{
  u16 icon_id;
  switch(type)
  {
    case VIDEO_H264:
    icon_id = IM_INFORMATION_ICON_H264;
    break;
    case VIDEO_AVS:
    icon_id = IM_INFORMATION_ICON_AVS;
    break;
    default :
    icon_id = IM_INFORMATION_ICON_MPEG;
    break;
  }
  return icon_id;
}

static u16 get_vid_format_icon_id(disp_sys_t vid)
{
	u16 icon_id;

	switch (vid)
	{
		case VID_SYS_NTSC_J:
		case VID_SYS_NTSC_M:
		case VID_SYS_NTSC_443:
			icon_id = IM_INFORMATION_ICON_480I;
			break;
		case VID_SYS_480P:
			icon_id = IM_INFORMATION_ICON_480P;
			break;
		case VID_SYS_PAL:
		case VID_SYS_PAL_N:
		case VID_SYS_PAL_NC:
		case VID_SYS_PAL_M:
			icon_id = IM_INFORMATION_ICON_576I;
		break;
		case VID_SYS_576P_50HZ:
			icon_id = IM_INFORMATION_ICON_576P;
			break;
		case VID_SYS_720P:
		case VID_SYS_720P_24HZ:
		case VID_SYS_720P_25HZ:
		case VID_SYS_720P_30HZ:
		case VID_SYS_720P_50HZ:
			icon_id = IM_INFORMATION_ICON_720P;
			break;
		case VID_SYS_1080I:
		case VID_SYS_1080I_50HZ:
			icon_id = IM_INFORMATION_ICON_1080I;
			break;
		case VID_SYS_1080P:
		case VID_SYS_1080P_24HZ:
		case VID_SYS_1080P_25HZ:
		case VID_SYS_1080P_30HZ:
		case VID_SYS_1080P_50HZ:
			icon_id = IM_INFORMATION_ICON_1080P;
			break;
		default :
			icon_id = IM_INFORMATION_ICON_UNKNOW;
			break;
	}
	return icon_id;
}
*/
static void convert_nim_info(u32 nim_mode,u16 *nim_modulate)
{
  switch(nim_mode )
  {
    case 4:
       *nim_modulate = 16;
      break;

    case 5:
       *nim_modulate = 32;
      break;

    case 6:
       *nim_modulate = 64;
      break;

    case 7:
       *nim_modulate = 128;
      break;

    case 8:
      *nim_modulate = 256;
      break;

    default:
       *nim_modulate = 64;
      break;
  }
}

//lint -e732
void fill_nprog_info(control_t *cont, u16 prog_id)
{
  control_t *p_ctrl, *p_subctrl;
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;
  //sat_node_t sat;
  u8 asc_str[32] = { 0 };
  u16 uni_buf[32];
  u8 time_str[32];
  u16 nim_modulate = 64;
  u16 prog_pos;
  BOOL is_mark, is_epg_get = FALSE;
  //rect_t frame = {0};


  //BOOL is_tv = sys_status_get_curn_prog_mode() == CURN_MODE_TV;
  u8 view_id = ui_dbase_get_pg_view_id();

  if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return;
  }

  prog_pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_FRAME);

  // set tv radio icon
  /*
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NICON);
  bmap_set_content_by_id(p_subctrl, is_tv ? IM_TITLEICON_TV : IM_TITLEICON_RADIO);
  ctrl_get_frame(p_subctrl, &frame);
  ctrl_add_rect_to_invrgn(cont, &frame);  //bug fixed computus  pic control's size 
  */
  
  // set number
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NUMBER);
  #ifdef LCN_SWITCH_DS_JHC
  sprintf((char *)asc_str, "%.4d", pg.logical_num);
  ui_set_front_panel_by_num(pg.logical_num);
  #else
  if(have_logic_number())
 {
	  sprintf((char *)asc_str, "%.4d", pg.logical_num);
	  ui_set_front_panel_by_num(pg.logical_num);
 }
 else
 {
	  sprintf((char *)asc_str, "%.4d", prog_pos + 1);
	  ui_set_front_panel_by_num(prog_pos +1);
  }
 #endif
 {
    text_set_content_by_ascstr(p_subctrl, (u8 *)asc_str);
 }
  // set name
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NAME);
  text_set_content_by_unistr(p_subctrl, pg.name);

  // set epg pf
  if(fill_epg_info(&pg, 1, uni_buf,time_str, 31))
  {
    is_epg_get = TRUE;
	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_P_DURATION);
  text_set_content_by_ascstr(p_subctrl, time_str);
  }
  else
  {
  	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_P_DURATION);
       text_set_content_by_ascstr(p_subctrl, (u8 *)"  -  -  -  -");
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_P);
  text_set_content_by_unistr(p_subctrl, uni_buf);
   

  if(fill_epg_info(&pg, 2, uni_buf,time_str, 31))
  {
    is_epg_get = TRUE;
	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_F_DURATION);
  text_set_content_by_ascstr(p_subctrl, time_str);
  }
  else
  {
  	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_F_DURATION);
       text_set_content_by_ascstr(p_subctrl, (u8 *)"  -  -  -  -");
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_F);
  text_set_content_by_unistr(p_subctrl, uni_buf);

  // set mark icon
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_MONEY);
  is_mark = (BOOL)(pg.is_scrambled);
  if(CUSTOMER_COUNTRY == COUNTRY_INDIA)
  {
       bmap_set_content_by_id(p_subctrl,
       (u16)(is_mark ? IM_INFORMATION_ICON_MONEY_INDIA:IM_INFORMATION_ICON_MONEY_INDIA_2));
  }
  else
  {
	bmap_set_content_by_id(p_subctrl,
       (u16)(is_mark ? IM_INFORMATION_ICON_MONEY :IM_INFORMATION_ICON_MONEY_2));
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_FAV);
  is_mark = ui_dbase_pg_is_fav(view_id, prog_pos);
 bmap_set_content_by_id(p_subctrl,
 (u16)(is_mark ? IM_INFORMATION_ICON_7 : IM_INFORMATION_ICON_7_2));

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_LOCK);
	is_mark = db_dvbs_get_mark_status(view_id, prog_pos, DB_DVBS_MARK_LCK, 0);
 bmap_set_content_by_id(p_subctrl, (u16)(is_mark ? IM_INFORMATION_ICON_6:IM_INFORMATION_ICON_6_2));

/* fix bug57016  */
#if 0
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_SKIP);
  is_mark = db_dvbs_get_mark_status(view_id, prog_pos, DB_DVBS_MARK_SKP, 0);
 bmap_set_content_by_id(p_subctrl, (u16)(is_mark ? IM_INFORMATION_ICON_SKIP: IM_INFORMATION_ICON_SKIP_2));
#endif

  // set info icon
#if ENABLE_TTX_SUBTITLE
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_SUBTT);
  is_mark = ui_is_subt_data_ready(prog_id);
  bmap_set_content_by_id(p_subctrl,
   (u16)(is_mark ? IM_INFORMATION_ICON_SUBTITLE: IM_INFORMATION_ICON_SUBTITLE_2));

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_TELTEXT);
  is_mark = ui_is_ttx_data_ready(prog_id);
 bmap_set_content_by_id(p_subctrl,
   (u16)(is_mark ? IM_INFORMATION_ICON_TTX: IM_INFORMATION_ICON_TTX_2));
#endif

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_EPG);
  is_mark = is_epg_get;
 bmap_set_content_by_id(p_subctrl,
  (u16)(is_mark ? IM_INFORMATION_ICON_EPG: IM_INFORMATION_ICON_EPG_2));
#if 0
 //group info
 p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_GROUP);
 db_dvbs_get_sat_by_id((u16)pg.sat_id, &sat);
 text_set_content_by_unistr(p_subctrl, sat.name);
 #endif
  // ca info
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_CA_INFO);
  db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
  {
    sprintf(asc_str, "FQ:%dK", (int)tp.freq);
  }
  else
  {
    convert_nim_info(tp.nim_modulate,&nim_modulate);
    sprintf((char *)asc_str, "%d/QAM%d/%d", (int)tp.freq, nim_modulate, (int)tp.sym);
  }
  text_set_content_by_ascstr(p_subctrl, asc_str);

  // update view
  ctrl_paint_ctrl(cont, TRUE);
}
//lint +e732

void fill_ntime_info(control_t *cont, BOOL is_redraw)
{
   utc_time_t cur_time = {0};
  //time_set_t p_set={{0}};
  control_t *p_frm = NULL, *p_ctrl = NULL;
  u8 time_str[32];

  #ifdef JHC_DALIAN_ZHUANGGUANG
  return;
  #endif
  
  if(g_customer.customer_id == CUSTOMER_YINHE)
  {
	  return;
  }
  p_frm = ctrl_get_child_by_id(cont, IDC_FRAME);
  MT_ASSERT(p_frm != NULL);

  #ifdef DIVI_GET_TIME_FROM_CA
  time_get(&cur_time, TRUE);
  #else
  time_get(&cur_time, FALSE);
  #endif
  
  //sys_status_get_time(&p_set);
 
  //time_get(&time, p_set.gmt_usage);
  sprintf((char *)time_str, "%.4d/%.2d/%.2d", cur_time.year, cur_time.month, cur_time.day);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_DATE);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  sprintf((char *)time_str, "%.2d:%.2d", cur_time.hour, cur_time.minute);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_TIME);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

RET_CODE open_nprog_bar(u32 para1, u32 para2)
{
  control_t *p_win;
#if 1//#ifndef SPT_SUPPORT
  control_t *p_cont, *p_subctrl;
  u16 i, x, y;
#endif
  u8  pg_type = 3;
  u8 view_id;
  u16 view_type;
  u16 content;
  u16 org_group, pos_in_set;
  u8  group_type;
  u32 context;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id(); 
  BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
  org_group = sys_status_get_curn_group();
  sys_status_get_group_info(org_group, &group_type, &pos_in_set, &context);

  // shift prog
  switch (para1)
  {
    case V_KEY_UP:
    case V_KEY_CHUP:
      ui_shift_prog(1, FALSE, &prog_id);
      break;
    case V_KEY_DOWN:
    case V_KEY_CHDOWN:
      ui_shift_prog(-1, FALSE, &prog_id);
      break;
    case V_KEY_PAGE_UP:
      ui_shift_prog(10, FALSE, &prog_id);
      break;
    case V_KEY_PAGE_DOWN:
      ui_shift_prog(-10, FALSE, &prog_id);
      break;
    case V_KEY_RECALL:
	case V_KEY_BACK:
      ui_recall(FALSE, &prog_id);
      break;
    case V_KEY_TVRADIO:
      if(!ui_tvradio_switch(FALSE, &prog_id))
      {
        if(GROUP_T_FAV == group_type)
        {
          content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
        }
        else
        {
          content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
        }
        ui_comm_cfmdlg_open(NULL, content, NULL, 0);
        return ERR_FAILURE;
      }
      sys_status_set_group_curn_type(pg_type);
      sys_status_get_curn_view_info(&view_type, &context);
      view_id = ui_dbase_create_view((dvbs_view_t)view_type, context, NULL);
      ui_dbase_set_pg_view_id(view_id);
      break;
    default:
      /* do nothing */;
  }

  // check, if no prog
  if(prog_id == INVALIDID)
  {
  	if( ui_is_playing())
  	{
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
  	}
  	ui_set_front_panel_by_str("----");
    ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_PROG, NULL, 0);
    OS_PRINTF("PROGBAR: ERROR, no prog in view!\n");
    return ERR_FAILURE;
  } 
  curn_prog_id = prog_id;
  manage_close_menu(ROOT_ID_SLEEP_HOTKEY, 0, 0) ;
  // reset
  //is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);

  // check for close
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
  {
    UI_PRINTF("====ROOT_ID_PROG_BAR already opened, now close before reopen===\n");
    manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  }
  
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  
#if 1//#ifndef SPT_SUPPORT
  /* create */
  p_win =
    fw_create_mainwin(ROOT_ID_PROG_BAR,
                           NPROG_BAR_CONT_X,
                           NPROG_BAR_CONT_Y,
                           NPROG_BAR_CONT_W,
                           NPROG_BAR_CONT_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE,
                           0);
  if (p_win == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_win,
    RSI_TRANSPARENT, RSI_TRANSPARENT,RSI_TRANSPARENT);
  ctrl_set_keymap(p_win, nprog_bar_cont_keymap);
  ctrl_set_proc(p_win, nprog_bar_cont_proc);
  
  ui_play_prog(prog_id, FALSE);
  // frame part
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FRAME,
                             NPROG_BAR_FRM_X, NPROG_BAR_FRM_Y,
                             NPROG_BAR_FRM_W,NPROG_BAR_FRM_H,
                             p_win, 0);
  ctrl_set_rstyle(p_cont, RSI_NPROG_BAR_FRM,
                  RSI_NPROG_BAR_FRM, RSI_NPROG_BAR_FRM);

  //programe number
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NUMBER, 
  						   NPROG_BAR_NUMER_X, NPROG_BAR_NUMER_Y, 
  						   NPROG_BAR_NUMER_W, NPROG_BAR_NUMER_H, 
  						   p_cont, 0);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NAME,
                      FSI_NPROG_BAR_NAME,FSI_NPROG_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //programe name
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NAME,
                            NPROG_BAR_NAME_TXT_X, NPROG_BAR_NAME_TXT_Y,
                            NPROG_BAR_NAME_TXT_W,NPROG_BAR_NAME_TXT_H,
                            p_cont, 0);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NAME,
                      FSI_NPROG_BAR_NAME,FSI_NPROG_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // date & time
 if((g_customer.customer_id != CUSTOMER_YINHE) && (g_customer.customer_id != CUSTOMER_JIESAI) )
{
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    NPROG_BAR_DATE_ICON_X, NPROG_BAR_DATE_ICON_Y,
    NPROG_BAR_DATE_ICON_W, NPROG_BAR_DATE_ICON_H,
    p_cont, 0);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);
}
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_DATE,
                            NPROG_BAR_DATE_TXT_X, NPROG_BAR_DATE_TXT_Y,
                            NPROG_BAR_DATE_TXT_W, NPROG_BAR_DATE_TXT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT |STL_CENTER);
  text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF,
                      FSI_NPLIST_BRIEF,FSI_NPLIST_BRIEF);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIME,
                            NPROG_BAR_TIME_TXT_X, NPROG_BAR_TIME_TXT_Y,
                            NPROG_BAR_TIME_TXT_W, NPROG_BAR_TIME_TXT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  text_set_align_type(p_subctrl, STL_LEFT |STL_CENTER);
  text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF,
                      FSI_NPLIST_BRIEF,FSI_NPLIST_BRIEF);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  // epg pf
  y = NPROG_BAR_PF_INFO_Y;
  for (i = 0; i < NPROG_BAR_PF_INFO_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_EPG_P+ i),
                       NPROG_BAR_PF_INFO_X + 180, y,
                       320, NPROG_BAR_PF_INFO_H,
                       p_cont, 0);
    text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF,
                        FSI_NPLIST_BRIEF,FSI_NPLIST_BRIEF);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

    y+= NPROG_BAR_PF_INFO_H + NPROG_BAR_PF_INFO_V_GAP;
  }
 y = NPROG_BAR_PF_INFO_Y;
  for(i = 0; i < NPROG_BAR_PF_INFO_CNT; i++)
  {
  	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_EPG_P_DURATION+ i),
                       NPROG_BAR_PF_INFO_X , y,
                       180, NPROG_BAR_PF_INFO_H,
                       p_cont, 0);
    text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF,
                        FSI_NPLIST_BRIEF,FSI_NPLIST_BRIEF);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

    y+= NPROG_BAR_PF_INFO_H + NPROG_BAR_PF_INFO_V_GAP;
  }

  // mark icon    MONEY  FAV LOCK SKIP
  x = NPROG_BAR_MARK_ICON_X;
  for (i = 0; i < NPROG_BAR_MARK_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, (u8)(IDC_ICON_MONEY+ i),
                       x, NPROG_BAR_MARK_ICON_Y,
                       NPROG_BAR_MARK_ICON_W, NPROG_BAR_MARK_ICON_H,
                       p_cont, 0);

    x+= NPROG_BAR_MARK_ICON_W + NPROG_BAR_MARK_ICON_H_GAP;
  }
  // info icon  EPG  SubT  TTX
  x = NPROG_BAR_INFO_ICON_X;
  for (i = 0; i < NPROG_BAR_INFO_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, (u8)(IDC_INFO_EPG+ i),
                       x, NPROG_BAR_INFO_ICON_Y,
                       NPROG_BAR_INFO_ICON_W , NPROG_BAR_INFO_ICON_H,
                       p_cont, 0);

    x+= NPROG_BAR_INFO_ICON_W + NPROG_BAR_INFO_ICON_H_GAP;
  }
  #if 0
  //group info
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_GROUP,
                            NPROG_BAR_GROUP_X, NPROG_BAR_GROUP_Y,
                            NPROG_BAR_GROUP_W, NPROG_BAR_GROUP_H,
                            p_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF,
                      FSI_NPLIST_BRIEF, FSI_NPLIST_BRIEF);
   text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
#endif
  // TP info
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CA_INFO,
                           NPROG_BAR_TP_INFO_X, NPROG_BAR_TP_INFO_Y,
                             NPROG_BAR_TP_INFO_W,  NPROG_BAR_TP_INFO_H,
                            p_cont, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF, FSI_NPLIST_BRIEF,
	  FSI_NPLIST_BRIEF);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //ad window
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_AD,
                           NPROG_BAR_AD_X, NPROG_BAR_AD_Y,
                           NPROG_BAR_AD_W,  NPROG_BAR_AD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_subctrl, RSI_NPROG_BAR_FRM, RSI_NPROG_BAR_FRM, RSI_NPROG_BAR_FRM);

#ifdef PBAR_SIG_UPDATE
  //signal name
   p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_SIGNAL_NAME_Q,
                                 NPROG_BAR_SIGNAL_X - 10, NPROG_BAR_SIGNAL_Y - 30, NPROG_BAR_SIGNAL_W + 20, NPROG_BAR_SIGNAL_H,
                                 p_cont, 0);

  ctrl_set_rstyle(p_subctrl, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF, FSI_NPLIST_BRIEF, FSI_NPLIST_BRIEF);

  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  text_set_align_type(p_subctrl, STL_CENTER | STL_TOP);

  text_set_content_by_ascstr(p_subctrl, "Q");

   p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_SIGNAL_NAME_S,
                                 NPROG_BAR_SIGNAL_X + 31, NPROG_BAR_SIGNAL_Y - 30, NPROG_BAR_SIGNAL_W + 20, NPROG_BAR_SIGNAL_H,
                                 p_cont, 0);

  ctrl_set_rstyle(p_subctrl, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  text_set_font_style(p_subctrl, FSI_NPLIST_BRIEF, FSI_NPLIST_BRIEF, FSI_NPLIST_BRIEF);

  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  text_set_align_type(p_subctrl, STL_CENTER | STL_TOP);

  text_set_content_by_ascstr(p_subctrl, "S");

  //signal bar
  p_subctrl = ctrl_create_ctrl(CTRL_PBAR, IDC_SIGNAL_QUALITY,
    NPROG_BAR_SIGNAL_X, NPROG_BAR_SIGNAL_Y, NPROG_BAR_SIGNAL_W, NPROG_BAR_SIGNAL_H,
    p_cont, 0);
  ctrl_set_rstyle(p_subctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  ctrl_set_mrect(p_subctrl, 3, 2,
    NPROG_BAR_QUALITY_W - 3, NPROG_BAR_QUALITY_H - 2 );
  pbar_set_direction(p_subctrl, 0);

  pbar_set_count(p_subctrl, 0, 100, 100);
   pbar_set_current(p_subctrl, 0);


  p_subctrl = ctrl_create_ctrl(CTRL_PBAR, IDC_SIGNAL_INTENSITY,
    NPROG_BAR_SIGNAL_X+40, NPROG_BAR_SIGNAL_Y,  NPROG_BAR_SIGNAL_W, NPROG_BAR_SIGNAL_H,
    p_cont, 0);
  ctrl_set_rstyle(p_subctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  ctrl_set_mrect(p_subctrl, 3, 2,
    NPROG_BAR_QUALITY_W - 3, NPROG_BAR_QUALITY_H - 2);
  pbar_set_direction(p_subctrl, 0);
   pbar_set_count(p_subctrl, 0, 100, 100);
    pbar_set_current(p_subctrl, 0);
#endif
#else
/* create */
  p_cont = spt_load_menu(ROOT_ID_PROG_BAR);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, nprog_bar_cont_keymap);
  ctrl_set_proc(p_cont, nprog_bar_cont_proc);
#endif

#ifndef CUSTOMER_JIESAI_WUNING
  fill_ntime_info(p_win, FALSE);
#endif
  fill_nprog_info(p_win, prog_id);
  ui_enable_signal_monitor(TRUE);
  ctrl_paint_ctrl(ctrl_get_root(p_win), FALSE);
  
#ifdef ENABLE_ADS
  ui_adv_pic_play(ADS_AD_TYPE_CHBAR, ROOT_ID_PROG_BAR);
#endif
  ui_init_play_timer(ROOT_ID_PROG_BAR, MSG_MEDIA_PLAY_TMROUT, 200);
  return SUCCESS;
}
//lint -e732
void fill_prog_name_info(control_t *cont, u16 prog_id)
{
  control_t *p_ctrl, *p_subctrl;
  dvbs_prog_node_t pg;
  u8 asc_str[32] = { 0 };
  u16 prog_pos;

//  BOOL is_tv = sys_status_get_curn_prog_mode() == CURN_MODE_TV;
  u8 view_id = ui_dbase_get_pg_view_id();

  if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return;
  }

  prog_pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_FRAME);

  // set tv radio icon
  /*
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NICON);
  bmap_set_content_by_id(p_subctrl, is_tv ? IM_TITLEICON_TV : IM_TITLEICON_RADIO);
  */
  // set number
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NUMBER);

  #ifdef LCN_SWITCH_DS_JHC
  sprintf((char *)asc_str, "%.4d", pg.logical_num);
  ui_set_front_panel_by_num(pg.logical_num);
  #else
  if(have_logic_number())
  {
    sprintf((char *)asc_str, "%.4d", pg.logical_num);
    ui_set_front_panel_by_num(pg.logical_num);
  }
  else
  {
    sprintf((char *)asc_str, "%.4d", prog_pos + 1);
    ui_set_front_panel_by_num(prog_pos +1);
  }
  #endif
  text_set_content_by_ascstr(p_subctrl, asc_str);
  // set name
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NAME);
  text_set_content_by_unistr(p_subctrl, pg.name);
  ctrl_paint_ctrl(p_ctrl, TRUE);
}
//lint +e732
#if 0
static u16  ui_get_curn_prog_id(s16 offset, u16 prog_id)
{
  play_set_t play_set;
  dvbs_prog_node_t pg;
  u16 prev_prog, total_prog;
  u16 curn_prog;
  s32 dividend = 0;
  u8 curn_view = ui_dbase_get_pg_view_id();
  BOOL is_force_skip = FALSE;

  sys_status_get_play_set(&play_set);

  total_prog = db_dvbs_get_count(curn_view);
  curn_prog = prev_prog = db_dvbs_get_view_pos_by_id(
                curn_view, prog_id);

  do
  {
    dividend = curn_prog + offset;

 #if 0 //fix bug 15865
    while(dividend < 0)
    {
      dividend += total_prog;
    }
    curn_prog = (u16)(dividend) % total_prog;
  #else
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
  #endif

    /* pos 2 id */
    prog_id = db_dvbs_get_id_by_view_pos(curn_view, curn_prog);
    MT_ASSERT(prog_id != INVALIDID);
    if(prev_prog == curn_prog)  /* all is skip*/
    {
      return prog_id;
    }

    // check play type
    db_dvbs_get_pg_by_id(prog_id, &pg);
    OS_PRINTF("pg is scramble = %d, play type = %d\n",
              pg.is_scrambled, play_set.type);

    switch(play_set.type)
    {
      case 1: // only play free pg
        is_force_skip = (BOOL)(pg.is_scrambled == 1);
        break;

      case 2: // only play scramble pg
        is_force_skip = (BOOL)(pg.is_scrambled == 0);
        break;

      default:
        ;
    }
  }
  while(is_force_skip
       || db_dvbs_get_mark_status(curn_view, curn_prog,
                                  DB_DVBS_MARK_SKP, FALSE));

  return prog_id;
}
#endif
#ifdef ENABLE_PVR_REC_CONFIG
static void on_pvr_recbar_save_exittoplay(u32 para1, u32 para2)
{
  fw_destroy_all_mainwin(FALSE);
  fw_paint_all_mainwin();
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
	      break;
    }
  } while (is_force_skip || db_dvbs_get_mark_status(curn_view, curn_prog,
                                   DB_DVBS_MARK_SKP, FALSE));
  return prog_id;
 }
#endif
static RET_CODE shift_prog_in_bar(control_t *p_ctrl, s16 offset)
{
#ifdef ENABLE_PVR_REC_CONFIG
  u16 cur_pg_id,next_pg_id;
  dvbs_prog_node_t cur_pg, next_pg;

  if(ui_recorder_isrecording())
  {
	//manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
    cur_pg_id = sys_status_get_curn_group_curn_prog_id();
  	db_dvbs_get_pg_by_id(cur_pg_id, &cur_pg);
    next_pg_id = shift_prog_id(offset);
    db_dvbs_get_pg_by_id(next_pg_id, &next_pg);
    if(next_pg.tp_id == cur_pg.tp_id)
    {
      ui_shift_prog(offset, FALSE, &curn_prog_id);
      fill_prog_name_info(p_ctrl, next_pg_id);
      ui_play_timer_start();
      return SUCCESS;
    }
    ui_comm_ask_for_dodlg_open_ex(NULL, IDS_EXIT_RECORD,\
                                  on_pvr_recbar_save_exittoplay, 0, 0, 0);
    return SUCCESS;
  }
#endif  
  // change prog
  ui_shift_prog(offset, FALSE, &curn_prog_id);
  fill_prog_name_info(p_ctrl, curn_prog_id);
  ui_play_timer_start();
  
  #ifdef ENABLE_ADS
  OS_PRINTF("send MSG_REFRESH_ADS_PIC to prog bar\n");
  fw_notify_root(p_ctrl, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
  #endif
  
  return SUCCESS;
}

static RET_CODE on_focus_up(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, 1);
}


static RET_CODE on_focus_down(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, -1);
}

static RET_CODE on_page_up(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, 10);
}

static RET_CODE on_page_down(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, -10);
}

#ifndef CUSTOMER_JIESAI_WUNING
static RET_CODE on_time_update(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  fill_nprog_info(p_ctrl,  prog_id);
  fill_ntime_info(p_ctrl, TRUE);
  return SUCCESS;
}
#endif

static RET_CODE on_recall(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id;
#ifdef ENABLE_PVR_REC_CONFIG
  if(ui_recorder_isrecording())
    return SUCCESS;
#endif
   /* do recall */
  if (ui_recall(TRUE, &prog_id))
  {
    UI_PRINTF("PROGBAR: recall is ok\n");
    // update view
    fill_nprog_info(p_ctrl, prog_id);
  }
  else
  {
    UI_PRINTF("PROGBAR: recall is failed\n");
  }
  return SUCCESS;
}

static RET_CODE on_tvradio(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u8 pg_type = 3;
  u16 prog_id;
  u16 content;
  u16 org_group, pos_in_set;
  u8  group_type;
  u32 context;
  BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
  org_group = sys_status_get_curn_group();
  sys_status_get_group_info(org_group, &group_type, &pos_in_set, &context);
#ifdef ENABLE_PVR_REC_CONFIG
  if(ui_recorder_isrecording())
    return SUCCESS;
#endif
  /* do tvradio switch */
  if (ui_tvradio_switch(TRUE, &prog_id))
  {
    // update view
    fill_nprog_info(p_ctrl, prog_id);
    sys_status_set_group_curn_type(pg_type);
  }
  else
  {
    if(GROUP_T_FAV == group_type)
    {
      content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
    }
    else
    {
      content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
    }
    
    ui_comm_cfmdlg_open(NULL, content, NULL, 0);
  }
  return SUCCESS;
}

static RET_CODE on_subt_ready(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_frm, *p_subctrl;
  pb_subt_info_t subt_info;
  u8 i = 0;
  BOOL need_show_in_64M = FALSE;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
//ttx subt not show in 64M
  if(get_subt_description_1(class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID), &subt_info, prog_id))
  {
    for(i = 0; i < subt_info.service_cnt; i ++)
    {
      if(subt_info.service[i].type < SUBT_TYPE_TTX_NORMAL)
      {
        need_show_in_64M = TRUE;
        break;
      }
    }
  }
  if ((prog_id == (u16)para1) && need_show_in_64M)
  {
    p_frm = ctrl_get_child_by_id(p_ctrl, IDC_FRAME);
    p_subctrl = ctrl_get_child_by_id(p_frm , IDC_INFO_SUBTT);

    bmap_set_content_by_id(p_subctrl, IM_INFORMATION_ICON_SUBTITLE);
    ctrl_paint_ctrl(p_subctrl, TRUE);
  }
  return SUCCESS;
}


static RET_CODE on_ttx_ready(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_frm, *p_subctrl;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

  UI_PRINTF("on_ttx_ready: prog id = %d, curn pg =%d \n", para1, prog_id);

  if (prog_id == (u16)para1)
  {
    p_frm = ctrl_get_child_by_id(p_ctrl, IDC_FRAME);
    p_subctrl = ctrl_get_child_by_id(p_frm , IDC_INFO_TELTEXT);

    bmap_set_content_by_id(p_subctrl, IM_INFORMATION_ICON_TTX);
    ctrl_paint_ctrl(p_subctrl, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_pbar_destory(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  sys_status_save();
  ui_deinit_play_timer();
#ifdef ENABLE_ADS
  pic_adv_stop();
#ifdef TEMP_SUPPORT_DS_AD
  ui_check_fullscr_ad_play();
#endif
#ifdef TEMP_SUPPORT_SZXC_AD
  ui_check_corner_ad_play();
#endif
#endif

  return ERR_NOFEATURE;
}

static RET_CODE on_prog_bar_detail(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{

  RET_CODE ret = SUCCESS;
  dvbs_prog_node_t pg;
  epg_prog_info_t prog_info = {0};
  event_node_t   *p_present_evt = NULL;
  event_node_t   *p_follow_evt = NULL;
  u16 pg_id;

  pg_id = sys_status_get_curn_group_curn_prog_id();

   if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return 0;
  }
  prog_info.network_id = (u16)pg.orig_net_id;
  prog_info.ts_id      = (u16)pg.ts_id;
  prog_info.svc_id     = (u16)pg.s_id;

  mul_epg_get_pf_event(&prog_info, &p_present_evt, &p_follow_evt);
  if(p_present_evt != NULL)
  {
    ret = manage_open_menu(ROOT_ID_PROG_DETAIL, (u32)p_present_evt, pg_id);
  }
  return ret;
}

static RET_CODE on_play_curn_prog(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
  ui_play_prog(curn_prog_id, FALSE);
  fill_nprog_info(p_ctrl, curn_prog_id);
  ui_play_timer_set_state(0);
  return SUCCESS;
}
static RET_CODE on_pbar_update_event(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  fill_nprog_info(p_cont, prog_id);

  return SUCCESS;
}
/*
static RET_CODE on_prog_pvr__pause(control_t *p_cont, u16 msg,
                            u32 para1, u32 para2)
{
  //nothing  when prog bar pause
  return SUCCESS;
}
*/
#ifdef ENABLE_ADS
static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("on_item_refresh_ads\n");

  ui_adv_pic_play(ADS_AD_TYPE_CHBAR, ROOT_ID_PROG_BAR);
  return SUCCESS;
}

static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}
#endif

BEGIN_KEYMAP(nprog_bar_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_RECALL, MSG_RECALL)
  ON_EVENT(V_KEY_TVRADIO, MSG_TVRADIO)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_RECALL) //wdw
  ON_EVENT(V_KEY_INFO, MSG_INFO)
  //ON_EVENT(V_KEY_PAUSE, MSG_RECORD_PAUSE)
  //ON_EVENT(V_KEY_PLAY, MSG_RECORD_PAUSE)
END_KEYMAP(nprog_bar_cont_keymap, NULL)

BEGIN_MSGPROC(nprog_bar_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_focus_down)
  ON_COMMAND(MSG_PAGE_UP, on_page_up)
  ON_COMMAND(MSG_PAGE_DOWN, on_page_down)
#ifndef CUSTOMER_JIESAI_WUNING
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
#endif 
  ON_COMMAND(MSG_RECALL, on_recall)
  ON_COMMAND(MSG_TVRADIO, on_tvradio)
  ON_COMMAND(MSG_TTX_READY, on_ttx_ready)
  ON_COMMAND(MSG_SUBT_READY, on_subt_ready)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_pbar_signal_update)
  ON_COMMAND(MSG_DESTROY, on_pbar_destory)
  ON_COMMAND(MSG_INFO, on_prog_bar_detail)
  ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, on_play_curn_prog)
  ON_COMMAND(MSG_PF_READY, on_pbar_update_event)
#ifdef ENABLE_ADS
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
#endif
  //ON_COMMAND(MSG_RECORD_PAUSE, on_prog_pvr__pause)
//  ON_COMMAND(MSG_NOTIFY_VIDEO_FORMAT, on_prog_bar_video_format)
END_MSGPROC(nprog_bar_cont_proc, ui_comm_root_proc)

