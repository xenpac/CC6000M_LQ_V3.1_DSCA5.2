/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_prog_bar.h"
#include "ui_pause.h"

enum control_id
{
  IDC_FRAME = 1,
};

enum frame_sub_control_id
{
  IDC_NICON = 1,
  IDC_ICON_NUMBER0,
  IDC_ICON_NUMBER1,
  IDC_ICON_NUMBER2,
  IDC_ICON_NUMBER3,
  IDC_NAME,
  IDC_DATE,
  IDC_TIME,
  IDC_EPG_P,
  IDC_EPG_F,
  IDC_ICON_AUDIO,
  IDC_ICON_MONEY,
  IDC_ICON_FAV,
  IDC_ICON_LOCK,
  IDC_ICON_SKIP,
  IDC_INFO_SUBTT,
  IDC_INFO_TELTEXT,
  IDC_INFO_EPG,
  IDC_GROUP,
  IDC_CA_INFO,
  IDC_TP_INFO,
};

static u8 number_icon_idc[] = 
{
	IDC_ICON_NUMBER0,
	IDC_ICON_NUMBER1,
	IDC_ICON_NUMBER2,
	IDC_ICON_NUMBER3,
};

static u16 number_icon_count = sizeof(number_icon_idc)/sizeof(u8);

enum local_msg
{
  MSG_RECALL = MSG_LOCAL_BEGIN + 600,
};

u16 prog_bar_cont_keymap(u16 key);

RET_CODE prog_bar_cont_proc(control_t *cont, u16 msg,
                       u32 para1, u32 para2);


static BOOL fill_epg_info(dvbs_prog_node_t *p_pg, u16 pos,
  u16 *p_str, u32 len)
{
  epg_filter_t epg_info = {0};
  evt_node_t *p_evt_node;
  u32 item_num = 0;
  
  epg_info.service_id = (u16)p_pg->s_id;
  epg_info.stream_id = (u16)p_pg->ts_id;
  epg_info.orig_network_id = (u16)p_pg->orig_net_id;
  epg_info.cont_level = 0;

  // use curent time as start time
  //time_get(&epg_info.start_time, TRUE);
 // OS_PRINTF("!!!!time :%d %d %d---- %d %d %d\n", 
 
  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID),
    &epg_info, &item_num);

  if ((pos == 2) && (p_evt_node != NULL))
  {
    p_evt_node = p_evt_node->p_next_evt_node;
  }
  
  if (p_evt_node != NULL)
  {
    //str_nasc2uni((u8*)p_evt_node->event_name, p_str, len);
    uni_strncpy(p_str, p_evt_node->event_name, len);
    return TRUE;
  }
  else
  {
    gui_get_string(IDS_MSG_NO_CONTENT, p_str, (u16)len);
    return FALSE;
  }

}


void fill_prog_info(control_t *cont, u16 prog_id)
{
  control_t *p_ctrl, *p_subctrl;
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;
  u8 asc_str[32] = { 0 };

  u16 track_rid[] = { IM_INFORMATION_ICON_STEREO, 
    IM_INFORMATION_ICON_LEFT, 
    IM_INFORMATION_ICON_RIGHT, 
    IM_INFORMATION_ICON_MONO};

  u16 number_icon_rid[] =
  	{
      IM_INFONUMBER_0, IM_INFONUMBER_1, IM_INFONUMBER_2, IM_INFONUMBER_3,
      IM_INFONUMBER_4, IM_INFONUMBER_5, IM_INFONUMBER_6, IM_INFONUMBER_7,
      IM_INFONUMBER_8, IM_INFONUMBER_9,
  	};
  u16 uni_buf[32];
  int i;
  u16 temp;
  u16 prog_pos;
  BOOL is_mark, is_epg_get = FALSE;

  BOOL is_tv = sys_status_get_curn_prog_mode() == CURN_MODE_TV;
  u8 view_id = ui_dbase_get_pg_view_id();

  if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return;
  }

  prog_pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_FRAME);

  // set tv radio icon
 // p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NICON);
  //bmap_set_content_by_id(p_subctrl, is_tv ? IM_TITLEICON_TV : IM_TITLEICON_RADIO);

  // set number
  temp = prog_pos + 1;
  for (i = number_icon_count - 1; i >= 0; i--)
  {	
    p_subctrl = ctrl_get_child_by_id(p_ctrl, number_icon_idc[i]);
    bmap_set_content_by_id(p_subctrl, number_icon_rid[temp%10]);
	temp /= 10;
  }

  // set name
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NAME); 
  text_set_content_by_unistr(p_subctrl, pg.name);

  // set epg pf
  if(fill_epg_info(&pg, 1, uni_buf, 31))
  {
    is_epg_get = TRUE;
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_P); 
  text_set_content_by_unistr(p_subctrl, uni_buf);

  if(fill_epg_info(&pg, 2, uni_buf, 31))
  {
    is_epg_get = TRUE;
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_F); 
  text_set_content_by_unistr(p_subctrl, uni_buf);

  // set mark icon
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_AUDIO); 
  bmap_set_content_by_id(p_subctrl, track_rid[pg.audio_track]);

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_MONEY); 
  is_mark = (BOOL)(pg.is_scrambled);
  bmap_set_content_by_id(p_subctrl,
    (u16)(is_mark ? IM_INFORMATION_ICON_MONEY : 0));

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_FAV);
  is_mark = ui_dbase_pg_is_fav(view_id, prog_pos);
  bmap_set_content_by_id(p_subctrl,
    (u16)(is_mark ? IM_INFORMATION_ICON_7 : 0));

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_LOCK);
	is_mark = db_dvbs_get_mark_status(view_id, prog_pos, DB_DVBS_MARK_LCK, 0);
  bmap_set_content_by_id(p_subctrl, (u16)(is_mark ? IM_INFORMATION_ICON_6 : 0));

  /* fix bug57016  */
#if 0
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_SKIP);
  is_mark = db_dvbs_get_mark_status(view_id, prog_pos, DB_DVBS_MARK_SKP, 0);
  bmap_set_content_by_id(p_subctrl, (u16)(is_mark ? IM_TV_SKIP : 0));
#endif

  // set info icon
#if ENABLE_TTX_SUBTITLE
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_SUBTT);
  is_mark = ui_is_subt_data_ready(prog_id);
  bmap_set_content_by_id(p_subctrl,
    (u16)(is_mark ? IM_INFORMATION_ICON_SUBTITLE: 0));

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_TELTEXT);
  is_mark = ui_is_ttx_data_ready(prog_id);
  bmap_set_content_by_id(p_subctrl,
    (u16)(is_mark ? IM_INFORMATION_ICON_TTX: 0));
#endif

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_EPG);
  is_mark = is_epg_get;
  bmap_set_content_by_id(p_subctrl,
    (u16)(is_mark ? IM_INFORMATION_ICON_EPG: 0));

  // set group
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_GROUP);
  sys_status_get_group_name(sys_status_get_curn_group(), uni_buf, 31);
  text_set_content_by_unistr(p_subctrl, uni_buf);

  // ca info
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_CA_INFO);
  text_set_content_by_ascstr(p_subctrl, ui_dbase_get_ca_system_desc(&pg));

  //tp info
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_TP_INFO);
  db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  sprintf(asc_str, "%d/%c/%d", (int)tp.freq, tp.polarity ? 'V' : 'H', (int)tp.sym);
  text_set_content_by_ascstr(p_subctrl, asc_str);
  
  // update view
  ctrl_paint_ctrl(cont, TRUE);
}

void fill_time_info(control_t *cont, BOOL is_redraw)
{
  utc_time_t time,curn_time;
  time_set_t p_set={{0}};
  control_t *p_frm = NULL, *p_ctrl = NULL;
  u8 time_str[32];

  p_frm = ctrl_get_child_by_id(cont, IDC_FRAME);
  MT_ASSERT(p_frm != NULL);
  sys_status_get_time(&p_set);

  time_get(&time, FALSE);
  time_get(&curn_time, TRUE);
  
 // sys_status_set_utc_time(&curn_time);
  
  sprintf(time_str, "%.2d/%.2d", time.month, time.day);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_DATE);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  sprintf(time_str, "%.2d:%.2d", time.hour, time.minute);
  p_ctrl = ctrl_get_child_by_id(p_frm, IDC_TIME);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

RET_CODE open_prog_bar(u32 para1, u32 para2)
{
  control_t *p_cont;
  u8 view_id;
  u16 view_type;
  u32 context;
#if 1//#ifndef SPT_SUPPORT
  control_t *p_ctrl, *p_subctrl;
  u16 i, x, y;
#endif
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);

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
      ui_recall(FALSE, &prog_id);
      break;
    case V_KEY_TVRADIO:
      if(!ui_tvradio_switch(FALSE, &prog_id))
      {
        u16 content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
        ui_comm_cfmdlg_open(NULL, content, NULL, 0);
        return ERR_FAILURE;
      }

      sys_status_get_curn_view_info(&view_type, &context);
      view_id = ui_dbase_create_view(view_type, context, NULL);
      ui_dbase_set_pg_view_id(view_id);
      break;
    default:
      /* do nothing */;
  }

  // check, if no prog
  if(prog_id == INVALIDID)
  {
    UI_PRINTF("PROGBAR: ERROR, no prog in view!\n");
    return ERR_FAILURE;
  }

  // reset
  is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);

#if 1//#ifndef SPT_SUPPORT
  /* create */
  p_cont =
    fw_create_mainwin(ROOT_ID_PROG_BAR,
                           PROG_BAR_CONT_X,
                           PROG_BAR_CONT_Y,
                           PROG_BAR_CONT_W,
                           PROG_BAR_CONT_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE,
                           0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, 
    RSI_IGNORE, RSI_IGNORE,RSI_IGNORE);
  ctrl_set_keymap(p_cont, prog_bar_cont_keymap);
  ctrl_set_proc(p_cont, prog_bar_cont_proc);
  
  // frame part
  p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_FRAME,
                             PROG_BAR_FRM_X, PROG_BAR_FRM_Y,
                             PROG_BAR_FRM_W,PROG_BAR_FRM_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PROG_BAR_FRM,
                  RSI_PROG_BAR_FRM, RSI_PROG_BAR_FRM);

  // tv radio icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NICON,
    PROG_BAR_ICON_X, PROG_BAR_ICON_Y, 
    PROG_BAR_ICON_W, PROG_BAR_ICON_H, 
    p_ctrl, 0);

  //programe number
  for (i = 0; i < number_icon_count; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, number_icon_idc[i],
      PROG_BAR_ICON_TXT_X + i *(PROG_BAR_ICON_TXT_W/number_icon_count), 
      PROG_BAR_ICON_TXT_Y, PROG_BAR_ICON_TXT_W/number_icon_count, 
      PROG_BAR_ICON_TXT_H, p_ctrl, 0);
    //text_set_font_style(p_subctrl, FSI_PROG_BAR_NAME,
    //                    FSI_PROG_BAR_NAME, FSI_PROG_BAR_NAME);
    //text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  }

  //programe name
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NAME,
                            PROG_BAR_NAME_TXT_X, PROG_BAR_NAME_TXT_Y,
                            PROG_BAR_NAME_TXT_W, PROG_BAR_NAME_TXT_H,
                            p_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PROG_BAR_NAME,
                      FSI_PROG_BAR_NAME,FSI_PROG_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // date & time
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    PROG_BAR_DATE_ICON_X, PROG_BAR_DATE_ICON_Y, 
    PROG_BAR_DATE_ICON_W, PROG_BAR_DATE_ICON_H, 
    p_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_DAY);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_DATE,
                            PROG_BAR_DATE_TXT_X, PROG_BAR_DATE_TXT_Y,
                            PROG_BAR_DATE_TXT_W, PROG_BAR_DATE_TXT_H,
                            p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PROG_BAR_NAME,
                      FSI_PROG_BAR_NAME,FSI_PROG_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    PROG_BAR_TIME_ICON_X, PROG_BAR_TIME_ICON_Y, 
    PROG_BAR_TIME_ICON_W, PROG_BAR_TIME_ICON_H, 
    p_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_TIME);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIME,
                            PROG_BAR_TIME_TXT_X, PROG_BAR_TIME_TXT_Y,
                            PROG_BAR_TIME_TXT_W, PROG_BAR_TIME_TXT_H,
                            p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
 
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PROG_BAR_NAME,
                      FSI_PROG_BAR_NAME,FSI_PROG_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  /*p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
    PROG_BAR_NOTIFY_ICON_X, PROG_BAR_NOTIFY_ICON_Y, 
    PROG_BAR_NOTIFY_ICON_W, PROG_BAR_NOTIFY_ICON_H, 
    p_ctrl, 0);
  bmap_set_content_by_id(p_subctrl, IM_INFORMATION_RADIO);*/

  // epg pf
  y = PROG_BAR_PF_INFO_Y;
  for (i = 0; i < PROG_BAR_PF_INFO_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_EPG_P+ i),
                       PROG_BAR_PF_INFO_X, y,
                       PROG_BAR_PF_INFO_W, PROG_BAR_PF_INFO_H,
                       p_ctrl, 0);
    text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_PROG_BAR_NAME,
                        FSI_PROG_BAR_NAME,FSI_PROG_BAR_NAME);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

    y+= PROG_BAR_PF_INFO_H + PROG_BAR_PF_INFO_V_GAP;
  }

  // mark icon
  x = PROG_BAR_MARK_ICON_X;
  for (i = 0; i < PROG_BAR_MARK_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, (u8)(IDC_ICON_AUDIO+ i),
                       x, PROG_BAR_MARK_ICON_Y,
                       PROG_BAR_MARK_ICON_W, PROG_BAR_MARK_ICON_H,
                       p_ctrl, 0);

    x+= PROG_BAR_MARK_ICON_W + PROG_BAR_MARK_ICON_H_GAP;
  }

  // info icon
  x = PROG_BAR_INFO_ICON_X;
  for (i = 0; i < PROG_BAR_INFO_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, (u8)(IDC_INFO_SUBTT+ i),
                       x, PROG_BAR_INFO_ICON_Y,
                       PROG_BAR_INFO_ICON_W, PROG_BAR_INFO_ICON_H,
                       p_ctrl, 0);

    x+= PROG_BAR_INFO_ICON_W + PROG_BAR_INFO_ICON_H_GAP;
  }

  // group
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_GROUP,
                            PROG_BAR_GROUP_INFO_X, PROG_BAR_GROUP_INFO_Y,
                            PROG_BAR_GROUP_INFO_W, PROG_BAR_GROUP_INFO_H,
                            p_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PROG_BAR_GROUP,
                      FSI_PROG_BAR_GROUP, FSI_PROG_BAR_GROUP);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // ca info
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CA_INFO,
                            PROG_BAR_CA_INFO_X, PROG_BAR_CA_INFO_Y,
                            PROG_BAR_CA_INFO_W, PROG_BAR_CA_INFO_H,
                            p_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PROG_BAR_GROUP,
                      FSI_PROG_BAR_GROUP, FSI_PROG_BAR_GROUP);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //tp info
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TP_INFO,
                            PROG_BAR_TP_X, PROG_BAR_TP_Y,
                            PROG_BAR_TP_W, PROG_BAR_TP_H,
                            p_ctrl, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_PROG_BAR_GROUP,
                      FSI_PROG_BAR_GROUP, FSI_PROG_BAR_GROUP);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

#else
  /* create */
  p_cont = spt_load_menu(ROOT_ID_PROG_BAR);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, prog_bar_cont_keymap);
  ctrl_set_proc(p_cont, prog_bar_cont_proc);
#endif

  fill_time_info(p_cont, FALSE);
  fill_prog_info(p_cont, prog_id);

  ui_enable_signal_monitor(TRUE);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_play_prog(prog_id, FALSE);
  return SUCCESS;
}

static RET_CODE shift_prog_in_bar(control_t *p_ctrl, s16 offset)
{
  u16 prog_id;
  // change prog
  ui_shift_prog(offset, TRUE, &prog_id);
  // update view
  fill_prog_info(p_ctrl, prog_id);
  
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

static RET_CODE on_time_update(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  fill_time_info(p_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_recall(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  u16 prog_id;
  /* do recall */
  if (ui_recall(TRUE, &prog_id))
  {
    UI_PRINTF("PROGBAR: recall is ok\n");
    // update view
    fill_prog_info(p_ctrl, prog_id);
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
  u16 prog_id;
  /* do tvradio switch */
  if (ui_tvradio_switch(TRUE, &prog_id))
  {
    // update view
    fill_prog_info(p_ctrl, prog_id);
  }
  else
  {
    BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
    u16 content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
    ui_comm_cfmdlg_open(NULL, content, NULL, 0);
  }
  return SUCCESS;
}

static RET_CODE on_subt_ready(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  pb_subt_info_t subt_info;
  u8 i = 0;
  BOOL need_show_in_64M = FALSE;
  control_t *p_frm, *p_subctrl;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

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
  
  return ERR_NOFEATURE;
}


BEGIN_KEYMAP(prog_bar_cont_keymap, NULL)
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
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(prog_bar_cont_keymap, NULL)

BEGIN_MSGPROC(prog_bar_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_focus_down)
  ON_COMMAND(MSG_PAGE_UP, on_page_up)
  ON_COMMAND(MSG_PAGE_DOWN, on_page_down)
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
  ON_COMMAND(MSG_RECALL, on_recall)
  ON_COMMAND(MSG_TVRADIO, on_tvradio)
  ON_COMMAND(MSG_TTX_READY, on_ttx_ready)
  ON_COMMAND(MSG_SUBT_READY, on_subt_ready)
  ON_COMMAND(MSG_DESTROY, on_pbar_destory)
END_MSGPROC(prog_bar_cont_proc, ui_comm_root_proc)

