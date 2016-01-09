/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_mosaic_api.h"   
#include "ui_mosaic.h"

enum mosaic_cont_id
{
  IDC_INVALID = 0,
  IDC_MOSAIC_INFO_FRM,
  IDC_MOSAIC_VIDEO_FRM, 
};

enum mosaic_info_frm_ctrl_id
{
  IDC_MOSAIC_NAME = 1,
  IDC_MOSAIC_HELP, 
  IDC_NO_MOSAIC_TIP, 
};

enum mosaic_video_frm_ctrl_id
{
  IDC_MOSAIC_VIDEO_MBOX = 1,
};


static comm_dlg_data_t mosaic_dlg =
{
  ROOT_ID_MOSAIC,
  DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_NO_MOSAIC,
  2000,
};

u16 mosaic_cont_keymap(u16 key);
RET_CODE mosaic_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 mosaic_video_keymap(u16 key);
RET_CODE mosaic_video_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

//RET_CODE mosaic_video_mbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

//u16 mosaic_text_keymap(u16 key);


static RET_CODE ui_mosaic_refresh_name(control_t *p_info_frm, BOOL is_paint)
{
  control_t *p_name;
  dvbs_prog_node_t cur_logic_cell_pg;

  p_name = ctrl_get_child_by_id(p_info_frm, IDC_MOSAIC_NAME);

  if(ui_mosaic_get_logic_cell_prog(ui_mosaic_get_logic_cell_id()))
  {
    ui_mosaic_get_cur_logic_cell_prog(&cur_logic_cell_pg);
    text_set_content_by_unistr(p_name, cur_logic_cell_pg.name);
  }
  else
  {
    text_set_content_by_unistr(p_name, (u16*)"");
  }
  
  if(is_paint)
  {
    ctrl_paint_ctrl(ctrl_get_parent(p_name), TRUE);
  }
  
  return SUCCESS;
  
}

RET_CODE open_mosaic(u32 para1, u32 para2)
{
  control_t *p_mask,*p_cont,*p_frame_title,*p_frame_video;
  control_t *p_video_mbox,*p_name,*p_help,*p_no_mosaic;
  rect_t rc;
  dvbs_prog_node_t cur_mosaic_pg;
  ui_cache_view();

  ui_init_mosaic();
  ui_mosaic_get_cur_mosaic_prog(&cur_mosaic_pg);
  
  p_mask = fw_create_mainwin(ROOT_ID_MOSAIC,
                             0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                             0, 0, OBJ_ATTR_ACTIVE, 0);
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_ROOT_CONT,
                            MOSAIC_FRM_X, MOSAIC_FRM_Y, 
                            MOSAIC_FRM_W, MOSAIC_FRM_H, 
                            p_mask, 0);

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, mosaic_cont_keymap);
  ctrl_set_proc(p_cont, mosaic_cont_proc);

  //infomation frame
  p_frame_title = ctrl_create_ctrl(CTRL_CONT, IDC_MOSAIC_INFO_FRM,
                            MOSAIC_INFO_FRM_X, MOSAIC_INFO_FRM_Y,
                            MOSAIC_INFO_FRM_W, MOSAIC_INFO_FRM_H,
                            p_cont, 0);

  ctrl_set_rstyle(p_frame_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  //name
  p_name = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_MOSAIC_NAME,
                              MOSAIC_NAME_X, MOSAIC_NAME_Y,
                              MOSAIC_NAME_W, MOSAIC_NAME_H, 
                              p_frame_title, 0);
  ctrl_set_rstyle(p_name, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_align_type(p_name , STL_CENTER | STL_VCENTER);
  text_set_font_style(p_name , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_name , TEXT_STRTYPE_UNICODE);

  ui_mosaic_refresh_name(p_frame_title, FALSE);

  //help
  p_help = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_MOSAIC_HELP,
                              MOSAIC_HELP_X, MOSAIC_HELP_Y,
                              MOSAIC_HELP_W, MOSAIC_HELP_H, 
                              p_frame_title, 0);
  ctrl_set_rstyle(p_help, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_align_type(p_help , STL_VCENTER|STL_VCENTER);
  text_set_font_style(p_help , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_help , TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_help, (u8*)"[OK]Select  [Menu]Exit");
  //text_set_content_by_strid(p_help, IDS_CARD_BASE_INFO_HELP);
  
  if( (db_dvbs_get_count(ui_mosaic_get_mosaic_view())>0)
    && (cur_mosaic_pg.service_type==SVC_TYPE_MOSAIC)
    && (cur_mosaic_pg.mosaic.h_e_cells>0) 
    && (cur_mosaic_pg.mosaic.v_e_cells>0)
    && (cur_mosaic_pg.mosaic.lg_cell_cnt>0)
    && (cur_mosaic_pg.mosaic.screen_right>cur_mosaic_pg.mosaic.screen_left)
    && (cur_mosaic_pg.mosaic.screen_bottom>cur_mosaic_pg.mosaic.screen_top)
    )
  {
    //video frame
    p_frame_video = ctrl_create_ctrl(CTRL_CONT, IDC_MOSAIC_VIDEO_FRM,
                              cur_mosaic_pg.mosaic.screen_left, cur_mosaic_pg.mosaic.screen_top,
                              cur_mosaic_pg.mosaic.screen_right-cur_mosaic_pg.mosaic.screen_left, 
                              cur_mosaic_pg.mosaic.screen_bottom-cur_mosaic_pg.mosaic.screen_top,
                              p_cont, 0);

    ctrl_set_rstyle(p_frame_video, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_keymap(p_frame_video, mosaic_video_keymap);
    ctrl_set_proc(p_frame_video, mosaic_video_proc);
    
    //video mbox
    ui_mosaic_calc_cell_rect(&rc);
    p_video_mbox = ctrl_create_ctrl(CTRL_TEXT, IDC_MOSAIC_VIDEO_MBOX,
                              rc.left, rc.top,
                              rc.right -  rc.left, rc.bottom - rc.top,
                              p_frame_video, 0);
    ctrl_set_rstyle(p_video_mbox, RSI_TV_SMALL, RSI_TV_SMALL, RSI_TV_SMALL);

    ctrl_process_msg(p_frame_video, MSG_GETFOCUS, 0, 0);
  }
  else
  {
    //no mosaic
    p_no_mosaic = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_NO_MOSAIC_TIP,
                                MOSAIC_EMPTY_TIP_X, MOSAIC_EMPTY_TIP_Y,
                                MOSAIC_EMPTY_TIP_W, MOSAIC_EMPTY_TIP_H, 
                                p_cont, 0);
    ctrl_set_rstyle(p_no_mosaic, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_no_mosaic , STL_VCENTER|STL_VCENTER);
    text_set_font_style(p_no_mosaic , FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_no_mosaic , TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_no_mosaic, IDS_NO_MOSAIC);
    ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  }
  
  ctrl_paint_ctrl(p_cont,FALSE);
  return SUCCESS;
  
}

static void mosaic_menu_exit(void)
{
  ui_mosaic_exit();
  manage_close_menu(ROOT_ID_MOSAIC, 0, 0);
}

static void mosaic_menu_exit_all(void)
{
  ui_mosaic_exit();
  ui_close_all_mennus();
}

static void undo_mosaic_do_cancel(void)
{
  ;
}

static RET_CODE on_close_mosaic(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  switch(msg)
  {
    case MSG_EXIT:
      ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_MOSAIC,
                                   mosaic_menu_exit, undo_mosaic_do_cancel, 0);
      break;

    case MSG_EXIT_ALL:
      ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_MOSAIC,
                                   mosaic_menu_exit_all, undo_mosaic_do_cancel, 0);
      break;
  }
  return SUCCESS;
}

static RET_CODE on_mosaic_msg_exit_play(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_mosaic_exit_cell_prog();
  return SUCCESS;
}

static RET_CODE on_mosaic_video_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{ 
  if(ui_mosaic_get_logic_cell_prog(ui_mosaic_get_logic_cell_id()))
  {
    //ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
    ui_india_change_group(DB_DVBS_ALL_TV, 0/*All group*/, TRUE);
    ui_play_prog(ui_mosaic_get_cur_cell_prog_id(), TRUE);
    manage_open_menu(ROOT_ID_MOSAIC_PLAY, 0, 0);
  }
  else
  {
     mosaic_dlg.content = IDS_MSG_NO_SIGNAL;
  
    ui_comm_dlg_open(&mosaic_dlg);
  }
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}

static RET_CODE on_mosaic_video_change_focus(control_t *p_video_frm, u16 msg, u32 para1, u32 para2)
{
  control_t *p_video_mbox,*p_info_frm;
  rect_t rc = {0};

  p_info_frm = ctrl_get_child_by_id(p_video_frm->p_parent, IDC_MOSAIC_INFO_FRM);
  p_video_mbox = ctrl_get_child_by_id(p_video_frm, IDC_MOSAIC_VIDEO_MBOX);
  
  ui_mosaic_change_logic_cell_id(msg);
  ui_mosaic_calc_cell_rect(&rc);
  ctrl_set_frame(p_video_mbox, &rc);

  ui_mosaic_refresh_name(p_info_frm, TRUE);
  ctrl_paint_ctrl(p_video_frm,TRUE);
  return SUCCESS;
}

static RET_CODE on_mosaic_video_page_updown(control_t *p_video_frm, u16 msg, u32 para1, u32 para2)
{
  switch(msg)
  {
    case MSG_PAGE_UP:
      change_mosaic_prog(-1);
      break;
      
    case MSG_PAGE_DOWN:
      change_mosaic_prog(1);
      break;

    default:
      break;
  }
  on_mosaic_video_change_focus(p_video_frm, msg, para1, para2);
  return SUCCESS;
}

#if 0
static RET_CODE on_hid_video_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_parent;
  
  p_parent = ctrl_get_parent(p_ctrl);
  ctrl_set_attr(p_ctrl,OBJ_ATTR_HIDDEN);
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_parent, MSG_GETFOCUS, 0, 0);
  ctrl_set_attr(p_ctrl,OBJ_ATTR_HIDDEN);
  ctrl_paint_ctrl(p_parent,TRUE);
  return SUCCESS;
}
#endif

BEGIN_KEYMAP(mosaic_cont_keymap, ui_comm_root_keymap) 
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(mosaic_cont_keymap, ui_comm_root_keymap)
  
BEGIN_MSGPROC(mosaic_cont_proc, ui_comm_root_proc)
   ON_COMMAND(MSG_EXIT_ALL, on_close_mosaic)
   ON_COMMAND(MSG_EXIT, on_close_mosaic)
   ON_COMMAND(MSG_EXIT_MOSAIC_PROG_PLAY, on_mosaic_msg_exit_play)
END_MSGPROC(mosaic_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(mosaic_video_keymap, NULL) 
  ON_EVENT(V_KEY_LEFT,  MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP,    MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN,  MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP,    MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN,  MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK,    MSG_SELECT) 
END_KEYMAP(mosaic_video_keymap, NULL)
  
BEGIN_MSGPROC(mosaic_video_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_mosaic_video_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_mosaic_video_change_focus)
  ON_COMMAND(MSG_FOCUS_UP, on_mosaic_video_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_mosaic_video_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_mosaic_video_page_updown)
  ON_COMMAND(MSG_PAGE_DOWN, on_mosaic_video_page_updown)
  ON_COMMAND(MSG_SELECT, on_mosaic_video_ok)
END_MSGPROC(mosaic_video_proc, cont_class_proc)

//BEGIN_MSGPROC(mosaic_video_mbox_proc, mbox_class_proc)
//END_MSGPROC(mosaic_video_mbox_proc, mbox_class_proc)

//BEGIN_KEYMAP(mosaic_text_keymap, NULL) 
//  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
//  ON_EVENT(V_KEY_BACK, MSG_BLACK_SCREEN)
//END_KEYMAP(mosaic_text_keymap, NULL)


