/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#include "ui_common.h"

#include "ui_video_v_full_screen.h"
#include "ui_pause.h"
#include "pvr_api.h"
#include "ui_video.h"
#include "file_list.h"
#include "pnp_service.h"
#include "ui_do_search.h"
#include "ui_upgrade_by_rs232.h"
#include "ui_mute.h"
#include "ui_jump.h"
#include "lib_char.h"
#include "ui_signal.h"
#include "file_play_api.h"
#include "ui_video_goto.h"
#include "ui_text_encode.h"

#define M_VIDEO_UNSUPPORT_SHOW_TIME  3000
#define M_VIDEO_UNSUPPORT_SEEK_TIME  1000

typedef enum 
{
    IDC_FILEPLAY_BAR_CONT = 1,
    IDC_FILEPLAY_BAR_SPEED_PLAY_CONT,
  
}control_id;

typedef enum 
{
    IDC_FILEPLAY_ICON = 1,
    IDC_FILEPLAY_PAUSE_STAT_ICON,
    IDC_FILEPLAY_PLAY_STAT_ICON,
    IDC_FILEPLAY_PREV_STAT_ICON,           
    IDC_FILEPLAY_NEXT_STAT_ICON,
#if ENABLE_TRICK_PLAY
    IDC_FILEPLAY_FF_STAT_ICON,           
    IDC_FILEPLAY_FB_STAT_ICON,
#endif
    IDC_FILEPLAY_NAME,
    IDC_FILEPLAY_PLAYING_TIME,
    IDC_FILEPLAY_TOTAL_TIME,
    IDC_FILEPLAY_PLAY_PROGRESS,
    IDC_FILEPLAY_SEEK,
    IDC_FILEPLAY_SPECIAL_PLAY_IMG_FRAME,
    IDC_FILEPLAY_SPECIAL_PLAY_TXT_FRAME,
    IDC_FILEPLAY_HELP_IMG,
    IDC_FILEPLAY_HELP_TXT,
}sub_control_id;


typedef struct
{
    BOOL enable_seek;
    BOOL do_change_movie;
    s32  seek_sec;
}ui_video_v_full_screen_t;

static ui_video_v_full_screen_t g_video_full_screen = {0};

u16 fileplay_bar_mainwin_keymap(u16 key);
static RET_CODE _ui_video_v_fscreen_play_next(void);

RET_CODE fileplay_bar_mainwin_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

static void _ui_video_v_fscreen_set_pn_change(BOOL enable)
{   
    av_set_t           av_set  = {0};
    avc_video_mode_1_t tv_mode = AVC_VIDEO_MODE_AUTO_1;

    sys_status_get_av_set(&av_set);
    
    tv_mode = sys_status_get_sd_mode(av_set.tv_mode);  

    /*!
     * tv mode is p or n, don't enable auto change mode
     * tv mode is auto, enable or disable auto change mode
     */
    if((AVC_VIDEO_MODE_AUTO_1 == tv_mode)
&& (enable))
    {
        ui_video_c_enable_tv_mode_change(TRUE);
    }
    else
    {
        ui_video_c_enable_tv_mode_change(FALSE);
    }
}

static control_t *_ui_video_v_fscreen_get_bar_cont(void)
{
    control_t *p_mainwin  = NULL;
    control_t *p_bar_cont = NULL;
    
    p_mainwin = fw_find_root_by_id(ROOT_ID_FILEPLAY_BAR);
    MT_ASSERT(p_mainwin != NULL);

    p_bar_cont = ctrl_get_child_by_id(p_mainwin, IDC_FILEPLAY_BAR_CONT);
    MT_ASSERT(p_bar_cont != NULL);

    return  p_bar_cont;
}

static RET_CODE _ui_video_v_fscreen_up_speed_play_cont(void)
{
    int        speed             = 0;
    control_t *p_mainwin         = NULL;
    control_t *p_speed_play_cont = NULL;
    control_t *p_speed_txt       = NULL;
    control_t *p_speed_bmp       = NULL;
    
    p_mainwin = fw_find_root_by_id(ROOT_ID_FILEPLAY_BAR);
    MT_ASSERT(p_mainwin != NULL);

    p_speed_play_cont = ctrl_get_child_by_id(p_mainwin, IDC_FILEPLAY_BAR_SPEED_PLAY_CONT);

    p_speed_bmp = ctrl_get_child_by_id(p_speed_play_cont, IDC_FILEPLAY_SPECIAL_PLAY_IMG_FRAME);
    p_speed_txt = ctrl_get_child_by_id(p_speed_play_cont, IDC_FILEPLAY_SPECIAL_PLAY_TXT_FRAME);

    speed = ui_video_c_get_play_speed();

    if(speed > MUL_PLAY_NORMAL_PLAY)
    {
      bmap_set_content_by_id(p_speed_bmp, IM_INFORMATION_FF_2);
    }
    else if(speed < MUL_PLAY_NORMAL_PLAY)
    {
      bmap_set_content_by_id(p_speed_bmp, IM_INFORMATION_FB_2);
    }
    else
    {
      bmap_set_content_by_id(p_speed_bmp, 0);
    }
      
    switch(speed)
    {
      case MUL_PLAY_BACKWORD_32X:
      case MUL_PLAY_FORWORD_32X:
        {
          text_set_content_by_ascstr(p_speed_txt, (u8 *)"x 32");
          break;
        }
        
      case MUL_PLAY_BACKWORD_16X:
      case MUL_PLAY_FORWORD_16X:
        {
          text_set_content_by_ascstr(p_speed_txt, (u8 *)"x 16");
          break;
        }
        
      case MUL_PLAY_BACKWORD_8X:
      case MUL_PLAY_FORWORD_8X:
        {
          text_set_content_by_ascstr(p_speed_txt, (u8 *)"x 8");
          break;
        }
      
      case MUL_PLAY_BACKWORD_4X:
      case MUL_PLAY_FORWORD_4X:
        {
          text_set_content_by_ascstr(p_speed_txt, (u8 *)"x 4");
          break;
        }
      
      case MUL_PLAY_BACKWORD_2X:
      case MUL_PLAY_FORWORD_2X:
        {
          text_set_content_by_ascstr(p_speed_txt, (u8 *)"x 2");
          break;
        }
      
      case MUL_PLAY_NORMAL_PLAY:
        {
          text_set_content_by_ascstr(p_speed_txt, (u8 *)"");
          break;
        }
      
      default:
        break;   
    }

    ctrl_paint_ctrl(p_speed_bmp,  TRUE);
    ctrl_paint_ctrl(p_speed_txt,  TRUE);
    
    return SUCCESS;
}

static void _ui_video_v_fscreen_up_ctrl(control_t *p_ctrl)
{
    control_t *p_bar_cont = NULL;

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    
    if(OBJ_STS_HIDE == ctrl_get_sts(p_bar_cont))
    {
      return;
    }  

    ctrl_paint_ctrl(p_ctrl, TRUE);
}

static void _ui_video_v_fscreen_up_unsupported_video(void)
{
    UI_VIDEO_PRINF("[%s]: next:%d\n", __FUNCTION__, g_video_full_screen.do_change_movie);
    
    if(g_video_full_screen.do_change_movie)
    {
      ui_comm_showdlg_open(NULL, IDS_UNSUPPORTED, NULL, 0);
    }
    else
    {
      ui_comm_showdlg_open(NULL, IDS_UNSUPPORTED, NULL, M_VIDEO_UNSUPPORT_SHOW_TIME);

      /*Set timer for play next video*/
      fw_tmr_create(ROOT_ID_FILEPLAY_BAR, 
                    MSG_FP_DISAPPEAR_UNSUPPORT, 
                    M_VIDEO_UNSUPPORT_SHOW_TIME,
                    FALSE);
    }
}

static void _ui_video_v_fscreen_up_name(void)
{
    control_t *p_bar_cont = NULL;
    control_t *p_ctrl     = NULL;
    u16       *p_name_uni = NULL;
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_NAME);
    p_name_uni = ui_video_m_get_cur_playing_file();
    
    text_set_content_by_unistr(p_ctrl, p_name_uni);
    _ui_video_v_fscreen_up_ctrl(p_ctrl);
}

static void _ui_video_v_fscreen_up_process_bar(void)
{  
    u32         pbar_curn       = 0;
    u32         play_time       = 0;
    u32         total_time      = 0;
    control_t  *p_bar_cont      = NULL;
    control_t  *p_play_progress = NULL;
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_play_progress = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_PLAY_PROGRESS);

    play_time = ui_video_c_get_play_time();
    total_time = ui_video_c_get_total_time_by_sec();
    
    if(0 != total_time)
    {
      pbar_curn = (play_time * FILEPLAY_BAR_PROGRESS_STEP / total_time);
    }

    pbar_set_current(p_play_progress, (u16)pbar_curn);
    _ui_video_v_fscreen_up_ctrl(p_play_progress);
}

static void _ui_video_v_fscreen_up_cur_time(void)
{   
    control_t         *p_bar_cont  = NULL;
    control_t         *p_ctrl      = NULL;
    video_play_time_t play_time    = {0};
    u8                time_str[32] = {0};

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_PLAYING_TIME);
    
    ui_video_c_switch_time(&play_time, ui_video_c_get_play_time());
    
    sprintf((char *)time_str, "%.2d:%.2d:%.2d", play_time.hour, play_time.min, play_time.sec);
    
    text_set_content_by_ascstr(p_ctrl, time_str);

    _ui_video_v_fscreen_up_ctrl(p_ctrl);
    
    _ui_video_v_fscreen_up_process_bar();
}

static void _ui_video_v_fscreen_up_total_time(void)
{
    control_t         *p_bar_cont   = NULL;
    control_t         *p_ctrl       = NULL;
    video_play_time_t total_time    = {0};
    u8                time_str[32]  = {0};

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    
    p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_TOTAL_TIME);
    
    ui_video_c_get_total_time(&total_time);
    
    sprintf((char *)time_str, "%.2d:%.2d:%.2d", total_time.hour, total_time.min, total_time.sec);
    
    text_set_content_by_ascstr(p_ctrl, time_str);
    _ui_video_v_fscreen_up_ctrl(p_ctrl);
}

static s16 _ui_video_v_fscreen_get_seek_steps(control_t *p_trick)
{
    rect_t rect       = {0};
    s16    center     = 0;
    s16    new_center = 0;
    u32    total_time = 0;
    
    ctrl_get_frame(p_trick, &rect); 
    
    center = (rect.left + rect.right ) / 2;      

    total_time = ui_video_c_get_total_time_by_sec();
    
    if(0 != total_time)
    {
      new_center = (s16)((FILEPLAY_BAR_PROGRESS_W * g_video_full_screen.seek_sec) / (s32)total_time);
    }
    
    new_center += FILEPLAY_BAR_PROGRESS_X;

    return (new_center - center);
    
}
static void _ui_video_v_fscreen_up_clear_seek_icon(void)
{
    control_t *p_seek     = NULL;
    control_t *p_bar_cont = NULL;
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_seek = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_SEEK);
    
    if(FALSE == g_video_full_screen.enable_seek)
    {
        if(OBJ_STS_SHOW == ctrl_get_sts(p_seek))
        {
          ctrl_set_sts(p_seek, OBJ_STS_HIDE);
          ctrl_paint_ctrl(p_bar_cont, TRUE);
        }  
    }
}
static void _ui_video_v_fscreen_show_seek_icon(void)
{
    control_t *p_bar_cont = NULL;
    control_t *p_seek     = NULL;
    s16        steps      = 0;

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_seek = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_SEEK);
  
    ctrl_empty_invrgn(p_seek);

    steps = _ui_video_v_fscreen_get_seek_steps(p_seek);
    ctrl_move_ctrl(p_seek, steps, 0);
    
    ctrl_set_sts(p_seek, OBJ_STS_SHOW);
    
    ctrl_paint_ctrl(p_bar_cont, TRUE);
}

static void _ui_video_v_fscreen_paint_icon(control_t *p_ctrl, u8 idc, u16 bmp_id)
{
    control_t *p_subctrl = NULL;    
    
    p_subctrl = ctrl_get_child_by_id(p_ctrl,  idc);
    
    bmap_set_content_by_id(p_subctrl,  bmp_id);
    ctrl_paint_ctrl(p_subctrl,  TRUE);
}

static void _ui_video_v_fscreen_up_normal_icon()
{
    control_t *p_bar_cont = NULL;
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PREV_STAT_ICON, IM_MP3_ICON_FB);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PAUSE_STAT_ICON, IM_MP3_ICON_PAUSE);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_NEXT_STAT_ICON, IM_MP3_ICON_FF);
#if ENABLE_TRICK_PLAY    
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_FB_STAT_ICON, IM_MP3_ICON_FB_V2);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_FF_STAT_ICON, IM_MP3_ICON_FF_V2);
#endif
}

//only for PLAY PAUSE FF FB, PREV and NEXT don't need use this func 
static void _ui_video_v_fscreen_up_select_icon(void)
{
    control_t *p_bar_cont = NULL;
    int        speed      = MUL_PLAY_NORMAL_PLAY;
    mul_fp_play_state_t state = (mul_fp_play_state_t)ui_video_c_get_play_state();
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    
    switch(state)
    {
      case MUL_PLAY_STATE_PLAY:
      case MUL_PLAY_STATE_LOAD_MEDIA:
      {
        _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2_SELECT);
        break;
      }
      case MUL_PLAY_STATE_PAUSE:
      {
        _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PAUSE_STAT_ICON, IM_MP3_ICON_PAUSE_SELECT);
        break;
      }
      case MUL_PLAY_STATE_SPEED_PLAY:
      {
          speed = ui_video_c_get_play_speed();
#if ENABLE_TRICK_PLAY
          if(speed > MUL_PLAY_NORMAL_PLAY)
          {
            _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_FF_STAT_ICON, IM_MP3_ICON_FF_SELECT_V2);
          }
          else if(speed < MUL_PLAY_NORMAL_PLAY)
          {
            _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_FB_STAT_ICON, IM_MP3_ICON_FB_SELECT_V2);
          }
          else if(speed == MUL_PLAY_NORMAL_PLAY)
          {
            _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2_SELECT);
          }
#endif          
        break;
      }
      default:
      {
        break;
      }
    }
}
static void _ui_video_v_fscreen_create_timer(BOOL create)
{
    osd_set_t osd_set = {0};
    
    sys_status_get_osd_set(&osd_set);

    if(FALSE == create)
    {
      fw_tmr_reset(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR, (osd_set.timeout * 1000));
    }
    else
    {
      fw_tmr_create(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR, (osd_set.timeout * 1000), FALSE);
    }
}
static void _ui_video_v_fscreen_show_bar(void)
{
    control_t *p_bar_cont = NULL;

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();

    if(OBJ_STS_SHOW != ctrl_get_sts(p_bar_cont))
    {
      ctrl_set_sts(p_bar_cont, OBJ_STS_SHOW);
      ctrl_paint_ctrl(p_bar_cont, TRUE);
      _ui_video_v_fscreen_create_timer(TRUE);
    }
    else
    {
      _ui_video_v_fscreen_create_timer(FALSE);
    }
    ui_video_c_sub_hide();

    _ui_video_v_fscreen_up_clear_seek_icon(); 
}

static void _ui_video_v_fscreen_up_all_icon(void)
{
    _ui_video_v_fscreen_show_bar();
    _ui_video_v_fscreen_up_normal_icon();
    _ui_video_v_fscreen_up_select_icon();
    _ui_video_v_fscreen_up_clear_seek_icon();   
}

static void _ui_video_v_fscreen_up_all(void)
{   
    _ui_video_v_fscreen_up_all_icon();
    _ui_video_v_fscreen_up_name();
    _ui_video_v_fscreen_up_cur_time();
    _ui_video_v_fscreen_up_total_time();
    _ui_video_v_fscreen_up_speed_play_cont();
}

static void _ui_video_v_fscreen_close(u32 para1, u32 para2)
{
    control_t *p_ctrl = NULL;
  
    ui_video_c_sub_hide();
  
    g_video_full_screen.enable_seek = FALSE;
    g_video_full_screen.do_change_movie = FALSE;
    g_video_full_screen.seek_sec = 0;
    
    if(ui_is_mute())
    {
      close_mute();
    }

    fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR);
    fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_DISAPPEAR_UNSUPPORT);
    
    /*!
     * Preview needn't auto change tv mode
     */
    _ui_video_v_fscreen_set_pn_change(FALSE);
      
    /*!
     Send event to ui_video_v to update current playing file
     */
    p_ctrl = fw_find_root_by_id(ROOT_ID_USB_FILEPLAY);
  
    if(NULL != p_ctrl)
    {
      fw_notify_root(p_ctrl,
                     NOTIFY_T_MSG,
                     TRUE,
                     MSG_VIDEO_EVENT_FULLSCREEN_CLOSE,
                     para1,
                     para2);
    }
  
    manage_close_menu(ROOT_ID_FILEPLAY_BAR, 0,0);
}
static void _ui_video_v_fscreen_destroy(u32 para1, u32 para2)
{
    mul_fp_play_state_t state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
      ui_comm_dlg_close();
    }

    if(MUL_PLAY_STATE_SPEED_PLAY == state)
    {
      ui_video_c_play_normal();
      ui_set_mute(ui_is_mute());
    }
    
    _ui_video_v_fscreen_close(para1, para2);
}

static RET_CODE _ui_video_v_fscreen_exit(control_t *p_ctrl, 
                                         u16 msg,
                                         u32 para1, 
                                         u32 para2)
{
    _ui_video_v_fscreen_destroy(0, 0);
    
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_exit_all(control_t *p_ctrl, 
                                         u16 msg,
                                         u32 para1, 
                                         u32 para2)
{
    _ui_video_v_fscreen_destroy(MSG_EXIT_ALL, para2);
    
    return SUCCESS;
}


static RET_CODE _ui_video_v_fscreen_on_show_bar(control_t *p_ctrl, 
                                                u16 msg,
                                                u32 para1, 
                                                u32 para2)
{
    _ui_video_v_fscreen_show_bar();
    return SUCCESS;
}
static void _ui_video_v_fscreen_hide_bar(control_t *p_bar_cont)
{
      fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR);
      ctrl_set_sts(p_bar_cont, OBJ_STS_HIDE);
      ctrl_erase_ctrl(p_bar_cont);
}
static RET_CODE _ui_video_v_fscreen_on_hide_bar(control_t *p_ctrl, 
                                                u16 msg,
                                                u32 para1, 
                                                u32 para2)
{
    control_t *p_bar_cont = NULL;
  
    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);
    
    if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_cont))
    {
      _ui_video_v_fscreen_hide_bar(p_bar_cont);
      ui_video_c_sub_show();
    }
    else
    {
      _ui_video_v_fscreen_destroy(0, 0);
    }
    
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_play_by_focus(u16 focus)
{
    u16               *p_path = NULL;
    control_t *p_root = NULL;
    p_path = ui_video_m_get_path_by_idx(focus);
    
    if(NULL == p_path)
    {
      return ERR_FAILURE;
    }

    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
      ui_comm_dlg_close();
    }

    p_root = fw_get_focus();
    if (p_root && p_root->id != ROOT_ID_FILEPLAY_BAR)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    UI_VIDEO_PRINF("@@@_ui_video_v_fscreen_play_by_focus() idx=%d\n", focus);

    //ui_video_c_load_media_from_local(p_path, FALSE);
    ui_video_c_play_by_file(p_path, 0);
    
    return SUCCESS;
}   


static RET_CODE _ui_video_v_fscreen_play_pause(control_t *p_ctrl, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
    u16 curn_play_idx = 0;
    
    ui_video_c_pause_resume();

    state = (mul_fp_play_state_t)ui_video_c_get_play_state();
    
    if(MUL_PLAY_STATE_PLAY == state)
    {
      OS_PRINTF("[%s] mute = %d", ui_is_mute());
      ui_set_mute(ui_is_mute());
    }

    //press play key and play again after press sop key
    curn_play_idx = ui_video_m_get_cur_playing_idx();
    if(MUL_PLAY_STATE_NONE == state)
    {
   	  _ui_video_v_fscreen_play_by_focus(curn_play_idx);
    }

    _ui_video_v_fscreen_up_speed_play_cont();
    _ui_video_v_fscreen_up_all_icon();

    return SUCCESS;
}

static RET_CODE on_change_volume(control_t *p_ctrl, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{

    return manage_open_menu(ROOT_ID_VOLUME_USB, 
                            ROOT_ID_FILEPLAY_BAR, 
                            (msg == MSG_INCREASE) ? V_KEY_VUP : V_KEY_VDOWN);
}

static RET_CODE _ui_video_v_fscreen_adjust_volume(u16 msg)
{

    return manage_open_menu(ROOT_ID_VOLUME_USB, 
                            ROOT_ID_FILEPLAY_BAR, 
                            (msg == MSG_FOCUS_LEFT) ? V_KEY_LEFT : V_KEY_RIGHT);
}

static RET_CODE _ui_video_v_fscreen_left_right(control_t *p_ctrl, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{   
    s32 jump_time  = 0;
    s32 play_time  = 0;
    s32 total_time = 0;
    control_t *p_bar_cont = NULL;
    mul_fp_play_state_t cur_state  = MUL_PLAY_STATE_NONE;

    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);
    
    if(OBJ_STS_HIDE == ctrl_get_sts(p_bar_cont))
    {
      return _ui_video_v_fscreen_adjust_volume(msg);
    }  

    _ui_video_v_fscreen_show_bar();
    
    jump_time = ui_jump_para_get();
    cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();
      
    if(MUL_PLAY_STATE_PLAY == cur_state)
    {
      ui_video_c_pause_resume();
    }
    else if(MUL_PLAY_STATE_PAUSE != cur_state)
    {
      return SUCCESS;
    }

    /*!
      * First time enable seek and set seek time as play time
      */
    if(FALSE == g_video_full_screen.enable_seek)
    {
      play_time = (s32)ui_video_c_get_play_time();
      g_video_full_screen.seek_sec = play_time;
      g_video_full_screen.enable_seek = TRUE;

      _ui_video_v_fscreen_up_all_icon();
    }
    
    if(msg == MSG_FOCUS_LEFT)
    {
      g_video_full_screen.seek_sec -= jump_time;

      if(g_video_full_screen.seek_sec < 0)
      {
        g_video_full_screen.seek_sec = 0;
      }
    }
    else if(msg == MSG_FOCUS_RIGHT)
    {
      g_video_full_screen.seek_sec += jump_time;

      total_time = (s32)ui_video_c_get_total_time_by_sec();

      if(g_video_full_screen.seek_sec > total_time)
      {
        g_video_full_screen.seek_sec = total_time;
      }
    }

    _ui_video_v_fscreen_show_seek_icon();

    return SUCCESS;
}                                               

static RET_CODE _ui_video_v_fscreen_confirm(control_t *p_ctrl, 
                                            u16 msg,
                                            u32 para1, 
                                            u32 para2)
{
    if(g_video_full_screen.enable_seek)
    {
     // g_video_full_screen.seek_sec -= play_time;
      
      ui_video_c_seek((u32)g_video_full_screen.seek_sec);
      
      g_video_full_screen.seek_sec = 0;
      
      /*Disable seek, because in up all icon function, need hide seek icon*/
      g_video_full_screen.enable_seek = FALSE;

      _ui_video_v_fscreen_up_all_icon();
    }
    else
    {
      _ui_video_v_fscreen_show_bar();  
    }
   
    return SUCCESS;
}
static RET_CODE _ui_video_v_fscreen_stop(control_t *p_ctrl, 
                                         u16 msg,
                                         u32 para1, 
                                         u32 para2)

{
    ui_video_c_stop();
    _ui_video_v_fscreen_exit(p_ctrl, msg, para1, para2);
    return SUCCESS;
}

#if ENABLE_TRICK_PLAY
static RET_CODE _ui_video_v_fscreen_trick_play(control_t *p_ctrl, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{
    if(MSG_FP_PLAY_FF == msg)
    {
        ui_video_c_fast_play();
    }
    else
    {
        ui_video_c_fast_back();
    }

    _ui_video_v_fscreen_up_speed_play_cont();
    _ui_video_v_fscreen_up_all_icon();

    return SUCCESS;
}
#endif

static RET_CODE _ui_video_v_fscreen_play_next(void)
{
    RET_CODE   ret        = ERR_FAILURE;
    u16        idx        = 0;
    control_t *p_bar_cont = NULL;
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();

    _ui_video_v_fscreen_show_bar();
    _ui_video_v_fscreen_up_normal_icon();
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_NEXT_STAT_ICON, IM_MP3_ICON_FF_SELECT);
    
    ui_video_m_get_next_file_idx(&idx);

    ui_video_m_set_cur_playing_file_by_idx(idx);

    ui_video_c_stop();
	  _ui_video_v_fscreen_play_by_focus(idx);

    return ret;
}

static RET_CODE _ui_video_v_fscreen_process_next(control_t *p_ctrl, 
                                                 u16 msg,
                                                 u32 para1, 
                                                 u32 para2)
{
    g_video_full_screen.do_change_movie = TRUE;
    
    _ui_video_v_fscreen_play_next();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_process_prev(control_t *p_ctrl, 
                                                 u16 msg,
                                                 u32 para1, 
                                                 u32 para2)
{
    u16       idx         = 0;
    control_t *p_bar_cont = NULL;
    
    g_video_full_screen.do_change_movie = TRUE;
    
    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);

    _ui_video_v_fscreen_show_bar();
    _ui_video_v_fscreen_up_normal_icon();
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PREV_STAT_ICON, IM_MP3_ICON_FB_SELECT);
    
    ui_video_m_get_prev_file_idx(&idx);

    ui_video_m_set_cur_playing_file_by_idx(idx);

    ui_video_c_stop();
    _ui_video_v_fscreen_play_by_focus(idx);

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_on_mute(control_t *p_ctrl, 
                                            u16 msg,
                                            u32 para1, 
                                            u32 para2)
{
  ui_set_mute((BOOL)(!ui_is_mute()));
  return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_open_text_encode(control_t *p_ctrl, 
                                                     u16 msg,
                                                     u32 para1, 
                                                     u32 para2)
{
  BOOL bOpen = FALSE;
    
  switch(CUSTOMER_ID)
  {
    case CUSTOMER_DEFAULT:
      if(msg == MSG_GREEN)
      {
        bOpen = TRUE;
      }
      break;

    default:
      if(msg == MSG_RED)
      {
        bOpen = TRUE;
      }
      break;
  }

  if(bOpen)
  {
    manage_open_menu(ROOT_ID_TEXT_ENCODE, 0, 0);
  }

  return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_show_goto(control_t *p_ctrl, 
                                              u16 msg,
                                              u32 para1, 
                                              u32 para2)
{
    mul_fp_play_state_t cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();
      
    if((MUL_PLAY_STATE_PAUSE == cur_state) || (MUL_PLAY_STATE_SPEED_PLAY == cur_state))
    {
      ui_video_c_pause_resume();
      _ui_video_v_fscreen_up_speed_play_cont();
      _ui_video_v_fscreen_up_all_icon();
    }
    
    _ui_video_v_fscreen_show_bar();
  
    manage_open_menu(ROOT_ID_VIDEO_GOTO, 0, 0);
  
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_show_subt(control_t *p_ctrl, 
                                              u16 msg,
                                              u32 para1, 
                                              u32 para2)
{
    manage_open_menu(ROOT_ID_FILEPLAY_SUBT, 0, 0);
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_show_audio_set(control_t *p_ctrl, 
                                                   u16 msg,
                                                   u32 para1, 
                                                   u32 para2)
{
    control_t *p_bar_cont = NULL;
    
    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);
  
    if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_cont))
    {
      _ui_video_v_fscreen_hide_bar(p_bar_cont);
    }
    
    return manage_open_menu(ROOT_ID_FP_AUDIO_SET, para1, 0);
}

static RET_CODE _ui_video_v_fscreen_on_stop_cfm(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("@@@_ui_video_v_fscreen_on_stop_cfm()\n");
  return SUCCESS;
}
static RET_CODE _ui_video_v_fscreen_on_load_media_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    rect_size_t        rect   = {0};
    video_play_time_t  play_time   = {0};
    u16 focus;

    UI_PRINTF("@@@_ui_video_v_fscreen_on_load_media_success()\n");

    if ( ROOT_ID_FILEPLAY_BAR != fw_get_focus_id() )
    {
      return SUCCESS;
    }

    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, TRUE);

    /*---get total time and resolution,then save them ----------*/
    ui_video_c_get_total_time(&play_time);
    ui_video_m_save_total_time(&play_time);
    ui_video_c_get_resolution(&rect);
    ui_video_m_save_resolution(&rect);

    _ui_video_v_fscreen_up_all();

    return SUCCESS;
}
static RET_CODE _ui_video_v_fscreen_on_load_media_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    UI_PRINTF("@@@_ui_video_v_fscreen_on_load_media_error()\n");

    if ( ROOT_ID_FILEPLAY_BAR != fw_get_focus_id() )
    {
      return SUCCESS;
    }

    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, FALSE);

    _ui_video_v_fscreen_up_unsupported_video();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_on_eos(control_t *p_ctrl, 
                                           u16 msg,
                                           u32 para1, 
                                           u32 para2)
{
    UI_VIDEO_PRINF("[%s]: \n", __FUNCTION__);

    ui_video_c_play_end();
    
    /*This case is that playing next file in MSG_FP_DISAPPEAR_UNSUPPORT msg arrived*/
    if(NULL != fw_find_root_by_id(ROOT_ID_POPUP))
    { 
        UI_VIDEO_PRINF("[%s]: popup exist\n", __FUNCTION__);
        return SUCCESS;
    }
    
    g_video_full_screen.do_change_movie = FALSE;
    
    _ui_video_v_fscreen_play_next();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_up_time(control_t *p_ctrl, 
                                            u16 msg,
                                            u32 para1, 
                                            u32 para2)
{
    _ui_video_v_fscreen_up_cur_time();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_unsupported_video(control_t *p_ctrl, 
                                                      u16 msg,
                                                      u32 para1, 
                                                      u32 para2)
{
    UI_VIDEO_PRINF("[%s]: \n", __FUNCTION__);
    
    /**Load media failure maybe send this event to ui, so show popup once*/
    if(NULL == fw_find_root_by_id(ROOT_ID_POPUP))
    {
        UI_VIDEO_PRINF("[%s]: popup not exist\n", __FUNCTION__);
        _ui_video_v_fscreen_up_unsupported_video();
    }

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_unsupport_seek(control_t *p_ctrl, 
                                                   u16 msg,
                                                   u32 para1, 
                                                   u32 para2)
{
    ui_comm_showdlg_open(NULL, IDS_UNSUPPORTED_SEEK, NULL, M_VIDEO_UNSUPPORT_SEEK_TIME);  
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_trick_to_begin(control_t *p_ctrl, 
                                                   u16 msg,
                                                   u32 para1, 
                                                   u32 para2)
{
    ui_set_mute(ui_is_mute());
    ui_video_c_play_normal();
    _ui_video_v_fscreen_up_speed_play_cont();
    _ui_video_v_fscreen_up_all_icon();
    
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_unsupported_timeout(control_t *p_ctrl, 
                                                        u16 msg,
                                                        u32 para1, 
                                                        u32 para2)
{
    UI_VIDEO_PRINF("[%s]: unsupport\n", __FUNCTION__);
    
    fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_DISAPPEAR_UNSUPPORT);
    _ui_video_v_fscreen_play_next();
    
    return SUCCESS;
}

static control_t * _ui_video_v_fscreen_create_main(void)
{
    control_t *p_mainwin = NULL;  

    p_mainwin = fw_create_mainwin(ROOT_ID_FILEPLAY_BAR,
                                  FILEPLAY_BAR_CONT_X, FILEPLAY_BAR_CONT_Y,
                                  FILEPLAY_BAR_CONT_W, FILEPLAY_BAR_CONT_H,
                                  ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
    MT_ASSERT(p_mainwin != NULL);
    
    ctrl_set_rstyle(p_mainwin, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_keymap(p_mainwin, fileplay_bar_mainwin_keymap);
    ctrl_set_proc(p_mainwin, fileplay_bar_mainwin_proc);

    return p_mainwin;
}

static void _ui_video_v_fscreen_create_icon(control_t *p_bar_cont)
{
  //control_t *p_pvr_img = NULL;
#if ENABLE_TRICK_PLAY  
  //FB icon
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_FB_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);

    //FF
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_FF_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+350, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);
#endif
    //Prev File
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_PREV_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+70, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);
    
    //play icon
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_PLAY_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+140, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);
    
    //pause icon
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_PAUSE_STAT_ICON,
                    FILEPLAY_BAR_PLAY_STAT_ICON_X + 210, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                    FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                    p_bar_cont, 0);
                                    
    //Next File
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_NEXT_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+280, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);
    
    //PVR icon
    #if 0
    p_pvr_img = ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_ICON,
                                 FILEPLAY_BAR_ICON_X, FILEPLAY_BAR_ICON_Y,
                                 FILEPLAY_BAR_ICON_W, FILEPLAY_BAR_ICON_H, 
                                 p_bar_cont, 0);
    bmap_set_content_by_id(p_pvr_img, IM_INFORMATION_PVR);
    #endif
}
RET_CODE ui_video_v_fscreen_open(u32 para1, u32 para2)
{
    control_t *p_mainwin         = NULL;
    control_t *p_bar_cont        = NULL;
    control_t *p_speed_play_cont = NULL;
    control_t *p_speed_bmp       = NULL;
    control_t *p_speed_txt       = NULL;
    control_t *p_play_time       = NULL;
    control_t *p_total_time      = NULL;
    control_t *p_file_name       = NULL;
    control_t *p_play_progress   = NULL;
    control_t *p_trick           = NULL;
    control_t *p_ctrl            = NULL;
    u16 bmp_id                   = 0;

    //open menu.

    p_mainwin = _ui_video_v_fscreen_create_main();
    
    
    //frame part
    p_bar_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_BAR_CONT,
                                         FILEPLAY_BAR_FRM_X, FILEPLAY_BAR_FRM_Y,
                                         FILEPLAY_BAR_FRM_W,FILEPLAY_BAR_FRM_H, 
                                         p_mainwin, 0);
    
    ctrl_set_rstyle(p_bar_cont, RSI_FILEPLAY_BAR_FRM, RSI_FILEPLAY_BAR_FRM, RSI_FILEPLAY_BAR_FRM);

    //speed play part
    p_speed_play_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_BAR_SPEED_PLAY_CONT,
                                         FILEPLAY_SPEED_PLAY_FRM_X, FILEPLAY_SPEED_PLAY_FRM_Y,
                                         FILEPLAY_SPEED_PLAY_FRM_W,FILEPLAY_SPEED_PLAY_FRM_H, 
                                         p_mainwin, 0);
    
    ctrl_set_rstyle(p_bar_cont, RSI_FILEPLAY_BAR_FRM, RSI_FILEPLAY_BAR_FRM, RSI_FILEPLAY_BAR_FRM);

    //Top-right remind special play img
    p_speed_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_SPECIAL_PLAY_IMG_FRAME,
                                 FILEPLAY_BAR_SPECIAL_PLAY_IMG_X, FILEPLAY_BAR_SPECIAL_PLAY_IMG_Y,
                                 FILEPLAY_BAR_SPECIAL_PLAY_IMG_W,FILEPLAY_BAR_SPECIAL_PLAY_IMG_H,
                                 p_speed_play_cont, 0);
    ctrl_set_rstyle(p_speed_bmp, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

    //special play txt
    p_speed_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_SPECIAL_PLAY_TXT_FRAME,
                                  FILEPLAY_BAR_SPECIAL_PLAY_TXT_X, FILEPLAY_BAR_SPECIAL_PLAY_TXT_Y,
                                  FILEPLAY_BAR_SPECIAL_PLAY_TXT_W,FILEPLAY_BAR_SPECIAL_PLAY_TXT_H,
                                  p_speed_play_cont, 0);
    ctrl_set_rstyle(p_speed_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_align_type(p_speed_txt, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_speed_txt, FSI_FILEPLAY_SPECIAL_PLAY_TEXT,
                                    FSI_FILEPLAY_SPECIAL_PLAY_TEXT, FSI_FILEPLAY_SPECIAL_PLAY_TEXT);
    text_set_content_type(p_speed_txt, TEXT_STRTYPE_UNICODE);

    //creat icons
    _ui_video_v_fscreen_create_icon(p_bar_cont);

    //name
    p_file_name = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_NAME,
                                   FILEPLAY_BAR_NAME_TXT_X, FILEPLAY_BAR_NAME_TXT_Y,
                                   FILEPLAY_BAR_NAME_TXT_W, FILEPLAY_BAR_NAME_TXT_H,
                                   p_bar_cont, 0);
    
    ctrl_set_rstyle(p_file_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_file_name, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_file_name, FSI_FILEPLAY_BAR_NAME,
                        FSI_FILEPLAY_BAR_NAME, FSI_FILEPLAY_BAR_NAME);
    text_set_content_type(p_file_name, TEXT_STRTYPE_UNICODE);


    // Playing time
    p_play_time = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_PLAYING_TIME,
                                   FILEPLAY_BAR_PLAY_CUR_TIME_X, FILEPLAY_BAR_PLAY_CUR_TIME_Y,
                                   FILEPLAY_BAR_PLAY_CUR_TIME_W, FILEPLAY_BAR_PLAY_CUR_TIME_H,
                                   p_bar_cont, 0);
    ctrl_set_rstyle(p_play_time, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_play_time, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_play_time, FSI_FILEPLAY_BAR_INFO,
                        FSI_FILEPLAY_BAR_INFO, FSI_FILEPLAY_BAR_INFO);
    text_set_content_type(p_play_time, TEXT_STRTYPE_UNICODE);

    // Total time
    p_total_time = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_TOTAL_TIME,
                                    FILEPLAY_BAR_TOTAL_TIME_X, FILEPLAY_BAR_TOTAL_TIME_Y,
                                    FILEPLAY_BAR_TOTAL_TIME_W, FILEPLAY_BAR_TOTAL_TIME_H,
                                    p_bar_cont, 0);
    ctrl_set_rstyle(p_total_time, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_total_time, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_total_time, FSI_FILEPLAY_BAR_INFO,
                        FSI_FILEPLAY_BAR_INFO, FSI_FILEPLAY_BAR_INFO);
    text_set_content_type(p_total_time, TEXT_STRTYPE_UNICODE);

    //playing progress
    p_play_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_FILEPLAY_PLAY_PROGRESS,
                                       FILEPLAY_BAR_PROGRESS_X, FILEPLAY_BAR_PROGRESS_Y,
                                       FILEPLAY_BAR_PROGRESS_W, FILEPLAY_BAR_PROGRESS_H,
                                       p_bar_cont, 0);
    ctrl_set_rstyle(p_play_progress, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    ctrl_set_mrect(p_play_progress,
                   FILEPLAY_BAR_PROGRESS_MIDX, FILEPLAY_BAR_PROGRESS_MIDY,
                   FILEPLAY_BAR_PROGRESS_MIDW, FILEPLAY_BAR_PROGRESS_MIDH);
    
    pbar_set_rstyle(p_play_progress, FILEPLAY_BAR_PROGRESS_MIN, FILEPLAY_BAR_PROGRESS_MAX, FILEPLAY_BAR_PROGRESS_MID);
    pbar_set_count(p_play_progress, 0, FILEPLAY_BAR_PROGRESS_STEP, FILEPLAY_BAR_PROGRESS_STEP);
    pbar_set_direction(p_play_progress, 1);
    pbar_set_workmode(p_play_progress, TRUE, 0);
    pbar_set_current(p_play_progress, 0);

    //seek point
    p_trick = ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_SEEK,
                               FILEPLAY_TRICK_X, FILEPLAY_TRICK_Y, 
                               FILEPLAY_TRICK_W, FILEPLAY_TRICK_H, 
                               p_bar_cont, 0);
    
    ctrl_set_sts(p_trick, OBJ_STS_HIDE);
    bmap_set_content_by_id(p_trick, IM_ICONS_TRICK);

    //help img
    p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_HELP_IMG,
                               FILEPLAY_HELP_IMG_X, FILEPLAY_HELP_IMG_Y, 
                               FILEPLAY_HELP_IMG_W, FILEPLAY_HELP_IMG_H, 
                               p_bar_cont, 0);
    switch(CUSTOMER_ID)
    {
      case CUSTOMER_DEFAULT:
        bmp_id = IM_EPG_COLORBUTTON_GREEN;
        break;

      default:
        bmp_id = IM_EPG_COLORBUTTON_RED;
        break;
    }
    bmap_set_content_by_id(p_ctrl, bmp_id);

    //help txt
    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_HELP_TXT,
                                   FILEPLAY_HELP_TXT_X, FILEPLAY_HELP_TXT_Y,
                                   FILEPLAY_HELP_TXT_W, FILEPLAY_HELP_TXT_H,
                                   p_bar_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_FILEPLAY_BAR_INFO,
                        FSI_FILEPLAY_BAR_INFO, FSI_FILEPLAY_BAR_INFO);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, IDS_TEXT_ENCODE);

    ctrl_process_msg(p_bar_cont, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_mainwin), FALSE);

    //update all the ctrls
    _ui_video_v_fscreen_up_all();

    _ui_video_v_fscreen_create_timer(TRUE);

    _ui_video_v_fscreen_set_pn_change(TRUE);

    return SUCCESS;
}

BEGIN_KEYMAP(fileplay_bar_mainwin_keymap, NULL)

  ON_EVENT(V_KEY_INFO, MSG_FP_INFO)
  ON_EVENT(V_KEY_OK, MSG_FP_OK)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_AUDIO, MSG_FP_AUDIO_SET)
  ON_EVENT(V_KEY_GOTO, MSG_SHOW_GOTO)
  ON_EVENT(V_KEY_SUBT, MSG_FP_PLAY_SUBT)
  ON_EVENT(V_KEY_MUTE, MSG_MUTE)  
    
  ON_EVENT(V_KEY_PAUSE, MSG_FP_PLAY_PAUSE)
  ON_EVENT(V_KEY_PLAY, MSG_FP_PLAY_PAUSE)
  ON_EVENT(V_KEY_NEXT, MSG_FP_PLAY_NEXT)
  ON_EVENT(V_KEY_PREV, MSG_FP_PLAY_PREV)  
#if ENABLE_TRICK_PLAY
  ON_EVENT(V_KEY_FORW2, MSG_FP_PLAY_FF)
  ON_EVENT(V_KEY_BACK2, MSG_FP_PLAY_FB)
#endif
  ON_EVENT(V_KEY_STOP, MSG_FP_PLAY_STOP)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)  
  ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_VUP, MSG_INCREASE)

  ON_EVENT(V_KEY_CANCEL, MSG_FP_HIDE_BAR)
  ON_EVENT(V_KEY_EXIT, MSG_FP_HIDE_BAR)
  ON_EVENT(V_KEY_BACK, MSG_FP_HIDE_BAR)
  ON_EVENT(V_KEY_MENU, MSG_FP_EXIT)
END_KEYMAP(fileplay_bar_mainwin_keymap, NULL)

BEGIN_MSGPROC(fileplay_bar_mainwin_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_INCREASE, on_change_volume)
  ON_COMMAND(MSG_DECREASE, on_change_volume)
  ON_COMMAND(MSG_MUTE, _ui_video_v_fscreen_on_mute)
  ON_COMMAND(MSG_RED, _ui_video_v_fscreen_open_text_encode)
  ON_COMMAND(MSG_GREEN, _ui_video_v_fscreen_open_text_encode)
  ON_COMMAND(MSG_FP_INFO, _ui_video_v_fscreen_on_show_bar)
  ON_COMMAND(MSG_SHOW_GOTO, _ui_video_v_fscreen_show_goto)
  ON_COMMAND(MSG_FP_HIDE_BAR, _ui_video_v_fscreen_on_hide_bar)
  ON_COMMAND(MSG_FP_EXIT, _ui_video_v_fscreen_exit)
  ON_COMMAND(MSG_FP_PLAY_PAUSE, _ui_video_v_fscreen_play_pause)
  ON_COMMAND(MSG_FP_PLAY_PREV, _ui_video_v_fscreen_process_prev)
  ON_COMMAND(MSG_FP_PLAY_NEXT, _ui_video_v_fscreen_process_next)
#if ENABLE_TRICK_PLAY
  ON_COMMAND(MSG_FP_PLAY_FF, _ui_video_v_fscreen_trick_play)
  ON_COMMAND(MSG_FP_PLAY_FB, _ui_video_v_fscreen_trick_play)
#endif
  ON_COMMAND(MSG_FP_PLAY_STOP, _ui_video_v_fscreen_stop)
  ON_COMMAND(MSG_FOCUS_LEFT, _ui_video_v_fscreen_left_right)
  ON_COMMAND(MSG_FOCUS_RIGHT, _ui_video_v_fscreen_left_right)
  ON_COMMAND(MSG_FP_OK, _ui_video_v_fscreen_confirm)
  ON_COMMAND(MSG_FP_PLAY_SUBT, _ui_video_v_fscreen_show_subt)
  ON_COMMAND(MSG_FP_AUDIO_SET, _ui_video_v_fscreen_show_audio_set)
  
  /*------Update Msg------*/
  ON_COMMAND(MSG_PLUG_OUT, _ui_video_v_fscreen_exit)
  ON_COMMAND(MSG_EXIT_ALL, _ui_video_v_fscreen_exit_all)
  ON_COMMAND(MSG_VIDEO_EVENT_EOS, _ui_video_v_fscreen_on_eos)
  ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, _ui_video_v_fscreen_up_time)
  ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, _ui_video_v_fscreen_on_stop_cfm)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, _ui_video_v_fscreen_on_load_media_success)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, _ui_video_v_fscreen_on_load_media_error)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, _ui_video_v_fscreen_on_load_media_error)
  ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY, _ui_video_v_fscreen_unsupported_video)
  ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO, _ui_video_v_fscreen_unsupported_video)
  ON_COMMAND(MSG_FP_DISAPPEAR_UNSUPPORT, _ui_video_v_fscreen_unsupported_timeout)
  ON_COMMAND(MSG_VIDEO_EVENT_TRICK_TO_BEGIN, _ui_video_v_fscreen_trick_to_begin)
  ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORT_SEEK, _ui_video_v_fscreen_unsupport_seek)
END_MSGPROC(fileplay_bar_mainwin_proc, ui_comm_root_proc)
