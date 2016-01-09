/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/


#include "ui_common.h"

#if ENABLE_NETWORK
#include "ui_network_playbar.h"
//#include "ui_onmov_provider.h"
#include "ui_pause.h"
//#include "ui_online_movie.h"
#include "ui_video.h"
#include "ui_ping_test.h"
#include "ui_mute.h"
#include "ui_jump.h"
//#include "ui_onmov_api.h"
//#include "netplay.h"


#include "file_play_api.h"
#include "ui_mainmenu.h"
#include "cdlna.h"
#include "ui_dlna_api.h"
#include "ui_dlna.h"
enum control_id
{
  IDC_NETPLAY_BAR_FRAME = 1,
  IDC_NETPLAY_FP_LOGO_FRAME,
  IDC_NETPLAY_FP_NO_SINGAL_FRAME,

  IDC_NETPLAY_ICON,
  IDC_NETPLAY_PAUSE_STAT_ICON,
  IDC_NETPLAY_PLAY_STAT_ICON,
  IDC_NETPLAY_PREV_STAT_ICON,           //Prev File
  IDC_NETPLAY_NEXT_STAT_ICON,           //Next File
#if ENABLE_TRICK_PLAY_FOR_NET
  IDC_NETPLAY_FF_STAT_ICON,           //fast forward
  IDC_NETPLAY_FB_STAT_ICON,           //fast backward
  IDC_NETPLAY_SF_STAT_ICON,           //slow forward
  IDC_NETPLAY_SB_STAT_ICON,           //slow backward
#endif	
  IDC_NETPLAY_FPF_STAT_ICON,          //time seek forward
  IDC_NETPLAY_FPB_STAT_ICON,          //time seek backward
  IDC_NETPLAY_TXT_FP,
  IDC_NETPLAY_NAME,
  IDC_NETPLAY_DATE,
  IDC_NETPLAY_TIME,
  IDC_NETPLAY_CAPACITY,
  IDC_NETPLAY_PLAYING_TIME,
  IDC_NETPLAY_TOTAL_TIME,
  IDC_NETPLAY_PLAY_TIME_BUTTON,
  IDC_NETPLAY_PLAY_TIME_TXT,
  IDC_NETPLAY_PLAY_PROGRESS,
  IDC_NETPLAY_TIRCK,
  IDC_NETPLAY_TITLE,
  IDC_NETPLAY_RED,
  IDC_NETPLAY_GOTO,
};

typedef struct
{
  u16 file_name[MAX_FILE_PATH];
  u32 file_idx;
  u32 play_time;
  u32 total_time;
  u16 jump_time;
} ui_np_pvt_info_t;

typedef struct
{
    BOOL b_showloading;
    BOOL b_showbar;
    /*!
    if True, timeshift is pause, and left / right trick the position.
    */
    BOOL b_pause_trick;
    BOOL b_manual_change_file;
    u8 dlna_mode;
} ui_np_info_t;

extern iconv_t g_cd_utf8_to_utf16le;
#if 0
static comm_help_data_t help_data =
{
    1,1,
    {IDS_GO_TO},//IDS_TEXT_ENCODE//Text Encode
    {IM_EPG_COLORBUTTON_RED}
};
#endif
static netplay_dlna_url *s_pUrl;
static s32 g_seek_delta = 0; //time second
static ui_np_pvt_info_t g_pvt_info = {{0},0,0,0,0};
static ui_np_info_t g_info = {FALSE};

u16 dlna_netplay_bar_cont_keymap(u16 key);

RET_CODE dlna_netplay_bar_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

int eth_getIPAddress(char *str, u32 *ip);

s32 ping_thread(u32 ip1, u32 ip2, u32 ip3, u32 ip4, u32 count, u32 size, ping_test_result_t *p_result);

static void ui_nplay_config_time();

static void nplay_set_play_time(control_t *p_bar_cont);

//static void nplay_set_start_time(control_t *p_bar_cont);

static void nplay_set_total_time(control_t *p_bar_cont);

static RET_CODE netplay_update_sm(u16 msg);

static RET_CODE netplay_update_ui(control_t *p_ctrl);

static void ui_nplay_show_bar(control_t *p_ctrl);

static void netplay_in_bar_back(control_t *p_ctrl);

static void netplay_play_paint_icon(control_t *p_ctrl, u8 idc, u16 bmp_id);

RET_CODE dlna_on_exit_netplay_bar(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static void netplay_open_dlg(u16 str_id, u32 time_out)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };
  dlg_data.content = str_id;
  dlg_data.dlg_tmout = time_out;
  ui_comm_dlg_open(&dlg_data);
}

static void netplay_clear_trick_icon(void)
{
    control_t *p_trick     = NULL;
    control_t *p_mainwin  = NULL;
    control_t *p_frame = NULL;

    
    p_mainwin = fw_find_root_by_id(ROOT_ID_NETWORK_PLAYBAR);
    MT_ASSERT(p_mainwin != NULL);

    p_frame = ctrl_get_child_by_id(p_mainwin, IDC_NETPLAY_BAR_FRAME);
    MT_ASSERT(p_frame != NULL);

    p_trick = ctrl_get_child_by_id(p_frame, IDC_NETPLAY_TIRCK);
  
    if(FALSE == g_info.b_pause_trick)
    {
        if(OBJ_STS_SHOW == ctrl_get_sts(p_trick))
        {
          ctrl_set_sts(p_trick, OBJ_STS_HIDE);
          ctrl_paint_ctrl(p_frame, TRUE);
        }  
    }
}
#ifndef WIN32
extern int cg_dlna_set_current_time(unsigned int pts, unsigned int duration);
#endif
static RET_CODE on_netplay_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_bar_cont = NULL, *p_play_progress = NULL;
  utc_time_t play_time = {0};
  u32 pbar_curn = 0;
  OS_PRINTF("on_netplay_update %d\n", para1);
  if(ROOT_ID_NETWORK_PLAYBAR != fw_get_focus_id())
  {
    return SUCCESS;
  }
#ifndef WIN32
//  {
    OS_PRINTF("on_netplay_update %d\n", para1);
    cg_dlna_set_current_time(para1, g_pvt_info.total_time*1000);
  //}
#endif
  p_bar_cont = ctrl_get_child_by_id(p_cont, IDC_NETPLAY_BAR_FRAME);
  if(p_bar_cont == NULL)
  {
    return SUCCESS;
  }
  p_play_progress = ctrl_get_child_by_id(p_bar_cont, IDC_NETPLAY_PLAY_PROGRESS);
  if(p_play_progress == NULL)
  {
    return SUCCESS;
  }

  g_pvt_info.play_time = (para1/1000);
  time_up(&play_time, g_pvt_info.play_time);

  if(0 != g_pvt_info.total_time)
  {
    pbar_curn = (g_pvt_info.play_time * NETPLAY_BAR_DLNA_PROGRESS_STEP / g_pvt_info.total_time);
  }  
  UI_VIDEO_PRINF("andy-debug---------update,\n");
  if(pbar_curn)
  {
    pbar_set_current(p_play_progress, (u16)pbar_curn);
    if(g_info.b_showbar)
    {
      ctrl_paint_ctrl(p_play_progress, TRUE);
    }
  }
  nplay_set_play_time(p_bar_cont);

  return SUCCESS;
}

static RET_CODE on_netplay_unsupported_video(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("[%s]", __FUNCTION__);
  return SUCCESS;
}

static RET_CODE on_netplay_load_media_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("@@@on_netplay_load_media_error()\n");

  if ( ROOT_ID_NETWORK_PLAYBAR != fw_get_focus_id() )
  {
    return SUCCESS;
  }

  dlna_on_exit_netplay_bar(p_cont, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_netplay_load_media_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frame = NULL;
  osd_set_t osd_set = {0};

  if ( ROOT_ID_NETWORK_PLAYBAR != fw_get_focus_id() )
  {
    return SUCCESS;
  }
  
  if (g_info.b_showloading)
  {
    ui_comm_dlg_close();
    g_info.b_showloading = FALSE;
  }

  p_frame = ctrl_get_child_by_id(p_cont, IDC_NETPLAY_BAR_FRAME);
  ctrl_set_sts(p_frame, OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_frame, FALSE);
  g_info.b_showbar = TRUE;
  netplay_clear_trick_icon();//fix 32729
  
  ui_nplay_config_time();
  nplay_set_total_time(p_frame);
  netplay_in_bar_back(p_frame);

  sys_status_get_osd_set(&osd_set);//get sys osd time
  fw_tmr_create(ROOT_ID_NETWORK_PLAYBAR, MSG_NP_DLNA_HIDE_BAR, (osd_set.timeout*1000), FALSE);

  return SUCCESS;
}

static RET_CODE on_netplay_icon_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  netplay_update_ui(p_cont);

  netplay_clear_trick_icon();//fix 32729
  return SUCCESS;
}

RET_CODE dlna_on_exit_netplay_bar(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
 control_t *p_root = fw_find_root_by_id(ROOT_ID_FP_AUDIO_SET);
  if (g_info.b_showloading)
  {
    ui_comm_dlg_close();
    g_info.b_showloading = FALSE;
  }

if((p_root != NULL) && (ctrl_get_sts(p_root) == OBJ_STS_SHOW))
   {   
       manage_close_menu(ROOT_ID_FP_AUDIO_SET, 0,0);
    }
  manage_close_menu(ROOT_ID_NETWORK_PLAYBAR, 0,0);
  ui_video_c_stop();
  memset(&g_pvt_info, 0, sizeof(g_pvt_info));
  g_info.b_showbar = FALSE;
  g_info.b_pause_trick = FALSE;
  g_seek_delta = 0;
  if(ui_is_mute())
  {
    close_mute();
  }
  
  return SUCCESS;
}

static RET_CODE on_destory_netplay_bar(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root = NULL;

  //TODO gujun what is this menu?

  p_root = fw_find_root_by_id(ROOT_ID_YOUTUBE);
  if(p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }

  p_root = fw_find_root_by_id(ROOT_ID_AUDIO_SET);//fix bug 31430
  if(p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }

  return ERR_NOFEATURE;
}

static RET_CODE on_hide_netplay_bar(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frame = NULL;
  OS_PRINTF("!!!!!!!!!!!!!!showbar:%d\n",g_info.b_showbar);
  if(g_info.b_showbar)
  {
    p_frame = ctrl_get_child_by_id(p_cont, IDC_NETPLAY_BAR_FRAME);
    ctrl_set_sts(p_frame, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_frame);
    g_info.b_showbar = FALSE;
    OS_PRINTF("!!!!!!!!!!!!!!Hide Bar\n");

    fw_tmr_destroy(ROOT_ID_NETWORK_PLAYBAR, MSG_NP_DLNA_HIDE_BAR);
  }
  else
  {
    OS_PRINTF("!!!!!!!!!!!!!!Hide Bar2\n");
    dlna_on_exit_netplay_bar(p_cont, msg, para1, para2);
  }
  
  return SUCCESS;
}

static RET_CODE on_change_netplay_state(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  netplay_update_sm(msg);
  netplay_update_ui(p_cont);

  return SUCCESS;
}

static RET_CODE on_press_leftright(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if (!g_info.b_showbar)
  {
    if (msg == MSG_FOCUS_LEFT)
    {
      manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_NETWORK_PLAYBAR, V_KEY_LEFT);
    }
    else if (msg == MSG_FOCUS_RIGHT)
    {
      manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_NETWORK_PLAYBAR, V_KEY_RIGHT);
    }
    return SUCCESS;
  }

#if 1  //disable seek function by gujun
  on_change_netplay_state(p_cont, msg, para1, para2);
#endif
  return SUCCESS;
}

static RET_CODE on_netplay_show_bar(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
   ui_nplay_show_bar(p_cont);
   return SUCCESS;
}

static RET_CODE on_netplay_show_goto(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  mul_fp_play_state_t cur_state = MUL_PLAY_STATE_NONE;

  cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();

  if (cur_state == MUL_PLAY_STATE_LOAD_MEDIA)
  {
    return SUCCESS;
  }

  if(MUL_PLAY_STATE_PAUSE != cur_state)
  {
    ui_video_c_pause_resume();
  }  

  netplay_update_ui(p_cont);
  
  ctrl_process_msg(p_cont, MSG_NP_DLNA_INFO, 0, 0);

  manage_open_menu(ROOT_ID_VIDEO_GOTO, ROOT_ID_NETWORK_PLAYBAR, 0);

  return SUCCESS;
}

static RET_CODE on_netplay_seek(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  mul_fp_play_state_t cur_state = MUL_PLAY_STATE_NONE;

  cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();

  if (cur_state == MUL_PLAY_STATE_LOAD_MEDIA)
  {
    return SUCCESS;
  }
  
  ui_video_c_seek(para1);
  return SUCCESS;
}

static RET_CODE on_mute(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 index;

  index = fw_get_focus_id();
  if(index != ROOT_ID_DO_SEARCH && index != ROOT_ID_MAINMENU)
  {
    ui_set_mute(!ui_is_mute());
  }

  return SUCCESS;
}

static RET_CODE on_open_audio_set(control_t *p_mainwin_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  control_t *p_frm_cont = NULL;
  
  if(g_info.b_showbar)
  {
    p_frm_cont = ctrl_get_child_by_id(p_mainwin_ctrl, IDC_NETPLAY_BAR_FRAME);
    OS_PRINTF("!!!!!!!!!!!!!!showbar2:%d\n",g_info.b_showbar);
    UI_VIDEO_PRINF("andy-debug---------audio_set,\n");
    ctrl_set_sts(p_frm_cont, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_frm_cont);
    g_info.b_showbar = FALSE;
    fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_NP_DLNA_HIDE_BAR);
  }
  return manage_open_menu(ROOT_ID_FP_AUDIO_SET, para1, 0);
}

static RET_CODE on_netplay_subt(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  UI_VIDEO_PRINF("ROOT_ID_FILEPLAY_SUBT\n");
  //manage_open_menu(ROOT_ID_FILEPLAY_SUBT, 0, 0);
  return SUCCESS;
}

#if 0
int utf8_to_utf16(const char *ins, tchar_t *outs, u32 max_len)
{
   return 0;
}
#endif
//extern u32 utf8_to_unicode(u16 *p_dst, u32 outsize, u8 *p_src, u32 insize);
//extern int utf8_to_utf16(const char *ins, tchar_t *outs, u32 max_len);
RET_CODE open_network_playbar(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_frame, *p_subctrl, *p_play_progress, *p_trick;
  control_t* p_tip = NULL;
  #if 0
  char *inbuf, *outbuf;
  //rect_t rect = {NETPLAY_DLG_X, NETPLAY_DLG_Y, NETPLAY_DLG_X + NETPLAY_DLG_W, NETPLAY_DLG_Y + NETPLAY_DLG_H};
  u16 uni_str[128] = {0};
  size_t src_len;
  size_t dest_len;
#endif
  //RET_CODE  ret;
  s_pUrl = (netplay_dlna_url *)para1;
  g_info.dlna_mode = para2;
  if(fw_find_root_by_id(ROOT_ID_NETWORK_PLAYBAR) != NULL) // already opened
  {
    dlna_on_exit_netplay_bar(0,0,0,0);
  }
  /* create */
  p_cont = fw_create_mainwin(ROOT_ID_NETWORK_PLAYBAR,
                          NETPLAY_BAR_DLNA_CONT_X, NETPLAY_BAR_DLNA_CONT_Y,
                          NETPLAY_BAR_DLNA_CONT_W, NETPLAY_BAR_DLNA_CONT_H,
                          ROOT_ID_INVALID, 0,
                          OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, dlna_netplay_bar_cont_keymap);
  ctrl_set_proc(p_cont, dlna_netplay_bar_cont_proc);
  //frame part
  p_frame = ctrl_create_ctrl(CTRL_CONT, IDC_NETPLAY_BAR_FRAME,
                            NETPLAY_BAR_DLNA_FRM_X, NETPLAY_BAR_DLNA_FRM_Y,
                            NETPLAY_BAR_DLNA_FRM_W,NETPLAY_BAR_DLNA_FRM_H, 
                            p_cont, 0);
  ctrl_set_rstyle(p_frame, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_sts(p_frame, OBJ_STS_HIDE);

#if ENABLE_TRICK_PLAY_FOR_NET	
  //FB icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_FB_STAT_ICON,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_X, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_Y,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_H, 
                            p_frame, 0);
	//SB icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_SB_STAT_ICON,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_X + 70, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_Y,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_H, 
                            p_frame, 0);
  //SF
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_SF_STAT_ICON,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_X+280, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_Y,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_H, 
                            p_frame, 0);
  //FF
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_FF_STAT_ICON,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_X+350, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_Y,
                            NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_H, 
                            p_frame, 0);
#endif

  //Prev File
/*  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_PREV_STAT_ICON,
	                          NETPLAY_BAR_PLAY_STAT_ICON_X+70, NETPLAY_BAR_PLAY_STAT_ICON_Y,
	                          NETPLAY_BAR_PLAY_STAT_ICON_W, NETPLAY_BAR_PLAY_STAT_ICON_H, 
	                          p_frame, 0);*/

  //p_tip = ctrl_create_ctrl(CTRL_BMAP, 255, 1000,10,80,40, p_frame, 0);
  p_tip = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_RED, 1030,0,80,40, p_frame, 0);
  if(p_tip)
  {
    ctrl_set_rstyle(p_tip, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_tip,  IM_HELP_RED);
  }

//  p_tip = ctrl_create_ctrl(CTRL_TEXT, 254, 1080, 10, 120, 40, p_frame, 0);
  p_tip = ctrl_create_ctrl(CTRL_TEXT, IDC_NETPLAY_GOTO, 1100, 0, 120, 40, p_frame, 0);
  if(p_tip)
  {
    ctrl_set_rstyle(p_tip, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_content_type(p_tip, TEXT_STRTYPE_STRID);
    text_set_font_style(p_tip, FSI_WHITE, FSI_BLACK, FSI_GRAY);
    text_set_align_type(p_tip, STL_LEFT | STL_VCENTER);
    text_set_content_by_strid(p_tip, IDS_GO_TO);
  }


  //play icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_PLAY_STAT_ICON,
                              NETPLAY_BAR_DLNA_PLAY_STAT_ICON_X, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_Y,
                              NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_H, 
                              p_frame, 0);
  //pause icon
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_PAUSE_STAT_ICON,
                              (NETPLAY_BAR_DLNA_PLAY_STAT_ICON_X +NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W ), NETPLAY_BAR_DLNA_PLAY_STAT_ICON_Y,
                              NETPLAY_BAR_DLNA_PLAY_STAT_ICON_W, NETPLAY_BAR_DLNA_PLAY_STAT_ICON_H, 
                              p_frame, 0);
	//Next File
 /* p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_NEXT_STAT_ICON,
                              NETPLAY_BAR_PLAY_STAT_ICON_X+280, NETPLAY_BAR_PLAY_STAT_ICON_Y,
                              NETPLAY_BAR_PLAY_STAT_ICON_W, NETPLAY_BAR_PLAY_STAT_ICON_H, 
                              p_frame, 0);*/

#if 0
  p_bar_title = ctrl_create_ctrl(CTRL_CONT, IDC_NETPLAY_TITLE,
                              NETPLAY_BAR_TITLE_X, NETPLAY_BAR_TITLE_Y, 
                              NETPLAY_BAR_TITLE_W, NETPLAY_BAR_TITLE_H,
                              p_frame, 0);
  ctrl_set_rstyle(p_bar_title, RSI_INFO_BAR_TITLE, RSI_INFO_BAR_TITLE, RSI_INFO_BAR_TITLE);
#endif
  //PVR icon
  #if 0
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_ICON,
                              NETPLAY_BAR_ICON_X, NETPLAY_BAR_ICON_Y + 10,
                              NETPLAY_BAR_ICON_W, NETPLAY_BAR_ICON_H, 
                              p_frame, 0);
  bmap_set_content_by_id(p_subctrl, IM_PIC_YOUTUBE_LOGO);
  ctrl_set_sts(p_subctrl, OBJ_STS_HIDE);
  #endif
  #if 0
  //name
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NETPLAY_NAME,
                              NETPLAY_BAR_NAME_TXT_X, NETPLAY_BAR_NAME_TXT_Y,
                              NETPLAY_BAR_NAME_TXT_W, NETPLAY_BAR_NAME_TXT_H,
                              p_frame, 0);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  
  inbuf = s_pUrl->name;
  outbuf = (char*) uni_str;
  src_len = strlen(inbuf) + 1;
  dest_len = sizeof(uni_str);
  iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
  text_set_content_by_unistr(p_subctrl, uni_str);
#endif
  // Playing time
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NETPLAY_PLAYING_TIME,
                              NETPLAY_BAR_DLNA_PLAY_CUR_TIME_X, NETPLAY_BAR_DLNA_PLAY_CUR_TIME_Y,
                              NETPLAY_BAR_DLNA_PLAY_CUR_TIME_W, NETPLAY_BAR_DLNA_PLAY_CUR_TIME_H,
                              p_frame, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_FONT_GRAY, FSI_FONT_GRAY, FSI_FONT_GRAY);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_subctrl, "00:00:00");

//'/'
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NETPLAY_PLAY_TIME_BUTTON,
                              NETPLAY_BAR_DLNA_PLAY_CUR_TIME_X + NETPLAY_BAR_DLNA_PLAY_CUR_TIME_W, NETPLAY_BAR_DLNA_PLAY_CUR_TIME_Y,
                              10, NETPLAY_BAR_DLNA_PLAY_CUR_TIME_H,
                              p_frame, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_FONT_GRAY, FSI_FONT_GRAY, FSI_FONT_GRAY);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_subctrl, "/");
  
  // Total time
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NETPLAY_TOTAL_TIME,
                              NETPLAY_BAR_DLNA_TOTAL_TIME_X, NETPLAY_BAR_DLNA_TOTAL_TIME_Y,
                              NETPLAY_BAR_DLNA_TOTAL_TIME_W, NETPLAY_BAR_DLNA_TOTAL_TIME_H,
                              p_frame, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_FONT_GRAY, FSI_FONT_GRAY, FSI_FONT_GRAY);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //playing progress
  p_play_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_NETPLAY_PLAY_PROGRESS,
                                      NETPLAY_BAR_DLNA_PROGRESS_X, NETPLAY_BAR_DLNA_PROGRESS_Y,
                                      NETPLAY_BAR_DLNA_PROGRESS_W, NETPLAY_BAR_DLNA_PROGRESS_H,
                                      p_frame, 0);
  ctrl_set_rstyle(p_play_progress, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_play_progress,
                NETPLAY_BAR_DLNA_PROGRESS_MIDX, NETPLAY_BAR_DLNA_PROGRESS_MIDY,
                NETPLAY_BAR_DLNA_PROGRESS_MIDW, NETPLAY_BAR_DLNA_PROGRESS_MIDH);
  pbar_set_rstyle(p_play_progress, NETPLAY_BAR_DLNA_PROGRESS_MIN, NETPLAY_BAR_DLNA_PROGRESS_MAX, NETPLAY_BAR_DLNA_PROGRESS_MID);
  pbar_set_count(p_play_progress, 0, NETPLAY_BAR_DLNA_PROGRESS_STEP, NETPLAY_BAR_DLNA_PROGRESS_STEP);
  pbar_set_direction(p_play_progress, 1);
  pbar_set_workmode(p_play_progress, TRUE, 0);
  pbar_set_current(p_play_progress, 0);

  //seek point
  p_trick = ctrl_create_ctrl(CTRL_BMAP, IDC_NETPLAY_TIRCK,
                          NETPLAY_DLNA_TRICK_X, NETPLAY_DLNA_TRICK_Y, 
                          NETPLAY_DLNA_TRICK_W, NETPLAY_DLNA_TRICK_H, 
                          p_frame, 0);
  ctrl_set_sts(p_trick, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_trick, IM_MP3_SOUND_M_YELLOW_PAUSE);

 // ui_comm_help_create(&help_data, p_frame);
 // ui_comm_help_move_pos(p_frame, 750, 20, 190, 20, 42);
//ctrl_set_sts(p_frame, OBJ_STS_SHOW);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

/////////////////////////////////////////////////////////////////////////////
#ifndef WIN32
  UI_PRINTF(" @@@open_netplay_bar() url=%s\n", s_pUrl->url);
  //ret = ui_video_c_load_media_from_net(s_pUrl->url, FALSE);
  ui_video_c_play_by_url(s_pUrl->url, 0, VDO_PLAY_MODE_VOD);
#endif
  //ui_comm_loadingdlg_open(&rect, 0);
  netplay_open_dlg(IDS_LOADING_WITH_WAIT, 0);
  g_info.b_showloading = TRUE;

  return SUCCESS;
}

static void ui_nplay_config_time()
{
  video_play_time_t _time;

  ui_video_c_get_total_time(&_time);
  g_pvt_info.jump_time = ui_jump_para_get();
  g_pvt_info.total_time = ((_time.hour * 3600) + (_time.min * 60) + _time.sec);
}

static void nplay_set_play_time(control_t *p_bar_cont)
{
  control_t *p_ctrl = NULL;
  utc_time_t play_time = {0};
  u8 time_str[32];

  p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_NETPLAY_PLAYING_TIME);
  time_up(&play_time, g_pvt_info.play_time);
  sprintf(time_str, "%.2d:%.2d:%.2d", play_time.hour, play_time.minute, play_time.second);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if(g_info.b_showbar)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

#if 0
static void nplay_set_start_time(control_t *p_bar_cont)
{
  control_t *p_ctrl = NULL;
  //  utc_time_t file_time = {0};
  u8 time_str[32];

  p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_NETPLAY_PLAYING_TIME);
  MT_ASSERT(p_ctrl != NULL);
  //time_up(&file_time, g_pvt_info.total_time);
  sprintf(time_str, "%.2d:%.2d:%.2d", 0, 0, 0);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if(g_info.b_showbar)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}
#endif

static void nplay_set_total_time(control_t *p_bar_cont)
{
  control_t *p_ctrl = NULL;
  utc_time_t file_time = {0};
  u8 time_str[32];

  p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_NETPLAY_TOTAL_TIME);
  MT_ASSERT(p_ctrl != NULL);
  time_up(&file_time, g_pvt_info.total_time);
  sprintf(time_str, "%.2d:%.2d:%.2d", file_time.hour, file_time.minute, file_time.second);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if(g_info.b_showbar)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

static RET_CODE netplay_update_sm(u16 msg)
{
  mul_fp_play_state_t _state = 0;
  //	u32 _seek_time = 0;
  mul_fp_play_state_t cur_state = MUL_PLAY_STATE_NONE;
  
  _state = ui_video_c_get_play_state();

  if (_state == MUL_PLAY_STATE_LOAD_MEDIA)
  {
    return SUCCESS;
  }

  cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();
  switch(cur_state)
  {
      case MUL_PLAY_STATE_PAUSE:
      case MUL_PLAY_STATE_PREVIEW:
	  	#ifndef WIN32
          cg_dlna_user_resume();
		#endif
          break;
      case MUL_PLAY_STATE_PLAY:
	  	#ifndef WIN32
          cg_dlna_user_pause();
		#endif
          break;
      default:
          break;
  }

  switch(msg)
  {
    case MSG_NP_DLNA_PLAY_PAUSE:
      ui_video_c_pause_resume();
      break;
      
    case MSG_FOCUS_LEFT:
    case MSG_FOCUS_RIGHT:
      if(MUL_PLAY_STATE_PAUSE == _state)
      {
        if(g_info.b_pause_trick == FALSE)
        {
          g_seek_delta = g_pvt_info.play_time + g_pvt_info.jump_time;
        }

        g_info.b_pause_trick = TRUE;

        if(msg == MSG_FOCUS_LEFT)
        {
          g_seek_delta -= g_pvt_info.jump_time;

          if(g_seek_delta < 0)
          {
            g_seek_delta = 0;
          }
        }
        else if(msg == MSG_FOCUS_RIGHT)
        {
          g_seek_delta += g_pvt_info.jump_time;

          if(g_seek_delta > (s32)g_pvt_info.total_time)
          {
            g_seek_delta = g_pvt_info.total_time;
          }
        }
      }
      else if(MUL_PLAY_STATE_PLAY == _state || MUL_PLAY_STATE_NONE == _state)
      {
        _state = MUL_PLAY_STATE_PAUSE;
        ui_video_c_pause_resume();
        //redo left and right operation
        if(g_info.b_pause_trick == FALSE)
        {
          g_seek_delta = g_pvt_info.play_time;
        }
        g_info.b_pause_trick = TRUE;

        if(msg == MSG_FOCUS_LEFT)
        {
          g_seek_delta -= g_pvt_info.jump_time;

          if(g_seek_delta < 0)
          {
            g_seek_delta = 0;
          }
        }
        else if(msg == MSG_FOCUS_RIGHT)
        {
          g_seek_delta += g_pvt_info.jump_time;

          if(g_seek_delta > (s32)g_pvt_info.total_time)
          {
            g_seek_delta = g_pvt_info.total_time;
          }
        }
      }
      break;

    case MSG_NP_DLNA_OK:
      if(g_info.b_pause_trick)
      {
        g_info.b_pause_trick = FALSE;
        //g_seek_delta -= g_pvt_info.play_time;
        ui_video_c_seek(g_seek_delta);
        g_seek_delta = 0;
      }
      OS_PRINTF("@@###@@ Press OK B_pause_trick:%d\n",g_info.b_pause_trick);
      break;
      
#if ENABLE_TRICK_PLAY_FOR_NET
    case MSG_NP_DLNA_PLAY_FF:
      ui_video_c_fast_play();
      break;
    case MSG_NP_DLNA_PLAY_FB:
      ui_video_c_fast_back();
      break;
#endif

    default:
      MT_ASSERT(0);
      break;
  }

  return SUCCESS;
}

static RET_CODE netplay_update_ui(control_t *p_ctrl)
{
  control_t *p_bar_ctrl = NULL, *p_trick = NULL;//, *p_time = NULL, *p_point = NULL;
  rect_t rect = {0};
  u8 ctrl_child_id = 0;
  u16 bmp_id = 0;
  u16 center = 0, new_center = 0;
  osd_set_t osd_set = {0};

  mul_fp_play_speed_t _speed = 0;
  mul_fp_play_state_t _state = 0;

  if(g_info.b_showbar)
  {
    sys_status_get_osd_set(&osd_set);//get sys osd time
    fw_tmr_reset(ROOT_ID_NETWORK_PLAYBAR, MSG_NP_DLNA_HIDE_BAR, (osd_set.timeout*1000));
  }
  else
  {
    ui_nplay_show_bar(p_ctrl);
  }
  //
  _speed = ui_video_c_get_play_speed();
  _state = ui_video_c_get_play_state();
  UI_VIDEO_PRINF("andy-------_state%d,_speed%d\n",_state,_speed);
  p_bar_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_NETPLAY_BAR_FRAME);

  //switch(g_state.play_status)
  switch(_state)
  {
    case MUL_PLAY_STATE_PLAY:
    case MUL_PLAY_STATE_LOAD_MEDIA:
      ctrl_child_id = IDC_NETPLAY_PLAY_STAT_ICON;
      bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
      break;

    case MUL_PLAY_STATE_PAUSE:
      ctrl_child_id = IDC_NETPLAY_PAUSE_STAT_ICON;
      bmp_id = IM_MP3_ICON_PAUSE_SELECT;
      break;
      
#if ENABLE_TRICK_PLAY_FOR_NET
    case MUL_PLAY_STATE_SPEED_PLAY:
      if(_state > MUL_PLAY_NORMAL_PLAY)
      {
        ctrl_child_id = IDC_NETPLAY_FF_STAT_ICON;
        bmp_id = IM_MP3_ICON_FF_SELECT_V2;
      }
      else if(_state < MUL_PLAY_NORMAL_PLAY)
      {
        ctrl_child_id = IDC_NETPLAY_FB_STAT_ICON;
        bmp_id = IM_MP3_ICON_FB_SELECT_V2;
      }
      else if(_state == MUL_PLAY_NORMAL_PLAY)
      {
        ctrl_child_id = IDC_NETPLAY_PLAY_STAT_ICON;
        bmp_id = IM_MP3_ICON_PLAY_2_SELECT;
      }
    break;
#endif

    default:
      break;
  }

  if(bmp_id != 0 && ctrl_child_id != 0)
  {
    netplay_in_bar_back(p_bar_ctrl);
    netplay_play_paint_icon(p_bar_ctrl, ctrl_child_id, bmp_id);
  }

  //draw trick flag below the pbar.
  p_trick = ctrl_get_child_by_id(p_bar_ctrl, IDC_NETPLAY_TIRCK);

  if(_state == MUL_PLAY_STATE_PAUSE)
  {
    if(g_info.b_pause_trick)
    {
      ctrl_set_sts(p_trick, OBJ_STS_SHOW);

      ctrl_get_frame(p_trick, &rect);

      center = (rect.left + rect.right )/2;
      if(0 != g_pvt_info.total_time)
      {
        new_center = (u16)((NETPLAY_BAR_DLNA_PROGRESS_W * g_seek_delta) /(g_pvt_info.total_time));
      }
      new_center += NETPLAY_BAR_DLNA_PROGRESS_X;
      ctrl_empty_invrgn(p_trick);
      ctrl_move_ctrl(p_trick, (s16)(new_center - center), 0);
    }
    //time_up(&play_time, g_pvt_info.play_time);
  }
  else
  {
    ctrl_set_sts(p_trick, OBJ_STS_HIDE);
  }
  // Draw cursor
  ctrl_paint_ctrl(p_bar_ctrl, TRUE);

  return SUCCESS;
}

void ui_nplay_show_bar(control_t *p_ctrl)
{
  control_t *p_list_cont;
  osd_set_t osd_set = {0};
  sys_status_get_osd_set(&osd_set);//get sys osd time
  OS_PRINTF("###system OSD time %d\n",osd_set.timeout);
  p_list_cont = ctrl_get_child_by_id(p_ctrl, IDC_NETPLAY_BAR_FRAME);
  ctrl_set_sts(p_list_cont, OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_list_cont, TRUE);
  fw_tmr_create(ROOT_ID_NETWORK_PLAYBAR, MSG_NP_DLNA_HIDE_BAR, (osd_set.timeout*1000), FALSE);
  g_info.b_showbar = TRUE;

  netplay_clear_trick_icon();//fix 32729
}

void netplay_in_bar_back(control_t *p_ctrl)
{
  control_t *p_subctrl;

  /*Pause*/
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_PAUSE_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_PAUSE);
  ctrl_paint_ctrl(p_subctrl,  TRUE);

  /*Play*/
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_PLAY_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_PLAY_2);
  ctrl_paint_ctrl(p_subctrl,  TRUE);

  /*Prev*/
/*  p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_PREV_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_FB);
  ctrl_paint_ctrl(p_subctrl,  TRUE);*/

  /*Next*/
 /* p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_NEXT_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_FF);
  ctrl_paint_ctrl(p_subctrl,  TRUE);*/


#if ENABLE_TRICK_PLAY_FOR_NET
  /*Fast forward*/
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_FF_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_FF_V2);
  ctrl_paint_ctrl(p_subctrl,  TRUE);

  /*Fast backward*/
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_FB_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_FB_V2);
  ctrl_paint_ctrl(p_subctrl,  TRUE);

  /*Slow forward*/
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_SF_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_SF);
  ctrl_paint_ctrl(p_subctrl,  TRUE);

  /*Slow backward*/
  p_subctrl = ctrl_get_child_by_id(p_ctrl,  IDC_NETPLAY_SB_STAT_ICON);
  bmap_set_content_by_id(p_subctrl,  IM_MP3_ICON_SB);
  ctrl_paint_ctrl(p_subctrl,  TRUE);
#endif
}

static void netplay_play_paint_icon(control_t *p_ctrl, u8 idc, u16 bmp_id)
{
  control_t *p_subctrl;

  p_subctrl = ctrl_get_child_by_id(p_ctrl,  idc);
  bmap_set_content_by_id(p_subctrl,  bmp_id);
  ctrl_paint_ctrl(p_subctrl,  TRUE);
}

u32 dlna_video_goto_get_net_play_time(void)
{
  return g_pvt_info.play_time;
}

u32 dlna_video_get_total_time()
{
    return g_pvt_info.total_time;
}
BEGIN_KEYMAP(dlna_netplay_bar_cont_keymap, NULL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_NP_DLNA_HIDE_BAR)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAUSE, MSG_NP_DLNA_PLAY_PAUSE)
  ON_EVENT(V_KEY_OK, MSG_NP_DLNA_OK)
  ON_EVENT(V_KEY_STOP, MSG_NP_DLNA_PLAY_STOP)
  ON_EVENT(V_KEY_INFO, MSG_NP_DLNA_INFO)
  ON_EVENT(V_KEY_RED, MSG_NP_DLNA_SHOW_GOTO)
  
  ON_EVENT(V_KEY_MUTE, MSG_MUTE)
  ON_EVENT(V_KEY_AUDIO, MSG_NP_DLNA_AUDIO_SET)
  ON_EVENT(V_KEY_SUBT, MSG_NP_DLNA_PLAY_SUBT)
  
END_KEYMAP(dlna_netplay_bar_cont_keymap, NULL)

BEGIN_MSGPROC(dlna_netplay_bar_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, on_netplay_update)
  ON_COMMAND(MSG_VIDEO_EVENT_EOS, dlna_on_exit_netplay_bar)
  ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO, on_netplay_unsupported_video)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, on_netplay_load_media_error)
	ON_COMMAND(MUL_PLAY_EVENT_LOAD_MEDIA_EXIT, on_netplay_load_media_error)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, on_netplay_load_media_success)
  ON_COMMAND(MSG_UP_DLNA_NETBAR, on_netplay_icon_update)
  
  ON_COMMAND(MSG_EXIT, dlna_on_exit_netplay_bar)
  ON_COMMAND(MSG_DESTROY, on_destory_netplay_bar)
  ON_COMMAND(MSG_NP_DLNA_HIDE_BAR, on_hide_netplay_bar)
  ON_COMMAND(MSG_FOCUS_LEFT, on_press_leftright)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_press_leftright)
  ON_COMMAND(MSG_NP_DLNA_PLAY_PAUSE, on_change_netplay_state)
  ON_COMMAND(MSG_NP_DLNA_OK, on_change_netplay_state)
  ON_COMMAND(MSG_NP_DLNA_PLAY_STOP, dlna_on_exit_netplay_bar)
  ON_COMMAND(MSG_NP_DLNA_INFO, on_netplay_show_bar)
  ON_COMMAND(MSG_NP_DLNA_SHOW_GOTO, on_netplay_show_goto)
  ON_COMMAND(MSG_NP_DLNA_PLAY_SEEK,on_netplay_seek)

  ON_COMMAND(MSG_MUTE, on_mute)
  ON_COMMAND(MSG_NP_DLNA_AUDIO_SET, on_open_audio_set)
  ON_COMMAND(MSG_NP_DLNA_PLAY_SUBT, on_netplay_subt)
  
END_MSGPROC(dlna_netplay_bar_cont_proc, ui_comm_root_proc)

#endif



