/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#if ENABLE_NETWORK
#include "ui_video_player.h"
#include "ui_video_player_prot.h"
#include "ui_video_player_gprot.h"

#include "ui_video.h"
#include "ui_mute.h"
#include "file_play_api.h"

#if 0
#include "ui_pause.h"
#include "ui_online_movie.h"

#include "ui_ping_test.h"

#include "ui_onmov_api.h"
#include "netplay.h"
#include "commonData.h"

#endif

enum control_id
{
    IDC_VDO_PLAYER_LOADMEDIA_TIME_CONT = 1,
    IDC_VDO_PLAYER_LOADMEDIA_TIME,
    IDC_VDO_PLAYER_BPS_CONT,
    IDC_VDO_PLAYER_BPS,
    IDC_VDO_PLAYER_BUF_PERCENT,

    IDC_VDO_PLAYER_PANEL,
    IDC_VDO_PLAYER_NAME,
    IDC_VDO_PLAYER_SYSTEM_TIME,
    IDC_VDO_PLAYER_SMALL_PLAY_ICON,
    IDC_VDO_PLAYER_PROGRESS_BAR,
    IDC_VDO_PLAYER_SEEK_TIME,
    IDC_VDO_PLAYER_PLAY_TIME,
    IDC_VDO_PLAYER_TOTAL_TIME,

    IDC_VDO_PLAYER_LARGE_PLAY_ICON,
};

typedef struct
{
    control_t *hLoadMediaTimeCont;
    control_t *hLoadMediaTime;
    control_t *hBpsCont;
    control_t *hBps;
    control_t *hBufPercent;

    control_t *hPanel;
    control_t *hSysTime;
    control_t *hSmallPlayIcon;
    control_t *hProgressBar;
    control_t *hSeekTime;
    control_t *hPlayTime;
    control_t *hTotalTime;

    control_t *hLargePlayIcon;

    u16 *name;
    u16 url_cnt;
    u8  **pp_urlList;

    u32 play_time;
    u32 seek_time;
    u32 total_time;
    u16 jump_time;

    u16 last_key;

    BOOL b_showloading;
} ui_video_player_app_t;

static comm_help_data_t vdoplayer_help_data =
{
    1,
    1,
    {IDS_GO_TO},
    {IM_HELP_RED}
};

static ui_video_player_app_t *g_pVdoPlayer = NULL;

u16 ui_vdoplayer_cont_keymap(u16 key);
RET_CODE ui_vdoplayer_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

static BOOL ui_vdoplayer_release_app_data(void);
static void ui_vdoplayer_update_panel(void);
static void ui_vdoplayer_show_panel(void);
static void ui_vdoplayer_hide_panel(void);
static void ui_vdoplayer_show_play_icon(void);
static void ui_vdoplayer_hide_play_icon(void);
static void ui_vdoplayer_set_system_time(void);
static void ui_vdoplayer_set_pbar_by_time(u32 time);
static void ui_vdoplayer_set_seek_time(void);
static void ui_vdoplayer_set_play_time(void);



/*================================================================================================
                           video player internel function
 ================================================================================================*/

static BOOL ui_vdoplayer_init_app_data(void)
{
    if (g_pVdoPlayer)
    {
        ui_vdoplayer_release_app_data();
    }

    g_pVdoPlayer = (ui_video_player_app_t *)mtos_malloc(sizeof(ui_video_player_app_t));
    MT_ASSERT(g_pVdoPlayer != NULL);
    memset((void *)g_pVdoPlayer, 0, sizeof(ui_video_player_app_t));

    return TRUE;
}

static BOOL ui_vdoplayer_release_app_data(void)
{
    if (g_pVdoPlayer != NULL)
    {
        mtos_free(g_pVdoPlayer);
        g_pVdoPlayer = NULL;
    }

    return TRUE;
}

static void ui_vdoplay_create_loadmedia_time_cont(control_t *p_cont)
{
    control_t *p_loadMediaTimeCont, *p_subCtrl;

    p_loadMediaTimeCont = ctrl_create_ctrl(CTRL_CONT, IDC_VDO_PLAYER_LOADMEDIA_TIME_CONT,
                                            VDO_PLAYER_LOADMEDIA_TIME_CONT_X, VDO_PLAYER_LOADMEDIA_TIME_CONT_Y, 
                                            VDO_PLAYER_LOADMEDIA_TIME_CONT_W, VDO_PLAYER_LOADMEDIA_TIME_CONT_H, 
                                            p_cont, 0);
    ctrl_set_rstyle(p_loadMediaTimeCont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    g_pVdoPlayer->hLoadMediaTimeCont = p_loadMediaTimeCont;

    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_LOADMEDIA_TIME,
                                  VDO_PLAYER_LOADMEDIA_TIME_X, VDO_PLAYER_LOADMEDIA_TIME_Y, 
                                  VDO_PLAYER_LOADMEDIA_TIME_W, VDO_PLAYER_LOADMEDIA_TIME_H, 
                                  p_loadMediaTimeCont, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    text_set_align_type(p_subCtrl, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    g_pVdoPlayer->hLoadMediaTime = p_subCtrl;

    ctrl_set_sts(p_loadMediaTimeCont, OBJ_STS_HIDE);
}

static void ui_vdoplay_create_bps_cont(control_t *p_cont)
{
    control_t *p_bpsCont, *p_subCtrl;

    p_bpsCont = ctrl_create_ctrl(CTRL_CONT, IDC_VDO_PLAYER_BPS_CONT,
                                    VDO_PLAYER_BPS_CONT_X, VDO_PLAYER_BPS_CONT_Y, 
                                    VDO_PLAYER_BPS_CONT_W, VDO_PLAYER_BPS_CONT_H, 
                                    p_cont, 0);
    ctrl_set_rstyle(p_bpsCont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    g_pVdoPlayer->hBpsCont = p_bpsCont;

    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_BPS,
                                  VDO_PLAYER_BPS_X, VDO_PLAYER_BPS_Y,
                                  VDO_PLAYER_BPS_W, VDO_PLAYER_BPS_H, 
                                  p_bpsCont, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subCtrl, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    g_pVdoPlayer->hBps = p_subCtrl;

    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_BUF_PERCENT,
                                  VDO_PLAYER_BUF_PERCENT_X, VDO_PLAYER_BUF_PERCENT_Y,
                                  VDO_PLAYER_BUF_PERCENT_W, VDO_PLAYER_BUF_PERCENT_H, 
                                  p_bpsCont, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subCtrl, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    g_pVdoPlayer->hBufPercent = p_subCtrl;

    ctrl_set_sts(p_bpsCont, OBJ_STS_HIDE);
}

static void ui_vdoplayer_update_load_media_time(u32 second)
{
    u16 *p_unistr;
    u16 uni_str1[100+1];
    u16 uni_str2[10+1];
    u8  asc_str[10+1];

    memset(uni_str1, 0, sizeof(uni_str1));
    memset(uni_str2, 0, sizeof(uni_str2));
    memset(asc_str, 0, sizeof(asc_str));

    if (second <= 5)
    {
        p_unistr = (u16 *)gui_get_string_addr(IDS_HAVE_LOADED);
        uni_strcpy(uni_str1, p_unistr);
    }
    else
    {
        p_unistr = (u16 *)gui_get_string_addr(IDS_TRY_LOADING_HAVE_LOADED);
        uni_strcpy(uni_str1, p_unistr);
    }

    sprintf(asc_str, " %ld ", second);
    str_asc2uni(asc_str, uni_str2);
    uni_strcat(uni_str1, uni_str2, 100);

    p_unistr = (u16 *)gui_get_string_addr(IDS_SECOND);
    uni_strcat(uni_str1, p_unistr, 100);

    text_set_content_by_unistr(g_pVdoPlayer->hLoadMediaTime, uni_str1);
}

static RET_CODE ui_vdoplayer_hide_loadmedia_time(void)
{
    ctrl_set_sts(g_pVdoPlayer->hLoadMediaTimeCont, OBJ_STS_HIDE);
    ctrl_erase_ctrl(g_pVdoPlayer->hLoadMediaTimeCont);

    return SUCCESS;
}

void ui_vdoplayer_get_bps_infos(u32 param, u16 *percent, u16 *bps)
{
    *percent = param >> 16;  //high 16bit saved percent
    *bps = param & 0xffff;  //low 16 bit saved bps
}

static void ui_vdoplayer_update_bps(u16 cur_bps, u16 load_percent)
{
    u16 *p_unistr;
    u16 uni_str1[100+1];
    u16 uni_str2[16+1];
    u8  asc_str[16+1];

    //bps
    memset(uni_str1, 0, sizeof(uni_str1));
    memset(uni_str2, 0, sizeof(uni_str2));
    memset(asc_str, 0, sizeof(asc_str));

    p_unistr = (u16 *)gui_get_string_addr(IDS_DOWNLOAD_SPEED);
    uni_strcpy(uni_str1, p_unistr);

    sprintf(asc_str, "%dKB/s", cur_bps);
    str_asc2uni(asc_str, uni_str2);
    uni_strcat(uni_str1, uni_str2, 100);

    text_set_content_by_unistr(g_pVdoPlayer->hBps, uni_str1);

    //percent
    memset(uni_str1, 0, sizeof(uni_str1));
    memset(uni_str2, 0, sizeof(uni_str2));
    memset(asc_str, 0, sizeof(asc_str));

    p_unistr = (u16 *)gui_get_string_addr(IDS_BUFFERED);
    uni_strcpy(uni_str1, p_unistr);

    sprintf(asc_str, "%d%%", load_percent);
    str_asc2uni(asc_str, uni_str2);
    uni_strcat(uni_str1, uni_str2, 100);

    text_set_content_by_unistr(g_pVdoPlayer->hBufPercent, uni_str1);
}

static void ui_vdoplayer_enter_seek_mode(void)
{
    osd_set_t osd_set;

    ui_vdoplayer_update_panel();
    ui_vdoplayer_show_panel();

    sys_status_get_osd_set(&osd_set);
    ui_start_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PANEL, (osd_set.timeout*1000));
}

static void ui_vdoplayer_exit_seek_mode(void)
{
    ui_vdoplayer_hide_panel();
    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PANEL);
}

static void ui_vdoplayer_enter_pause_mode(void)
{
    bmap_set_content_by_id(g_pVdoPlayer->hSmallPlayIcon,  IM_PROGRESS_ICON_PAUSE);
    ui_vdoplayer_update_panel();
    ui_vdoplayer_show_panel();
    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PANEL);

    bmap_set_content_by_id(g_pVdoPlayer->hLargePlayIcon,  IM_ICON_PAUSE);
    ui_vdoplayer_show_play_icon();
    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PLAY_ICON);
}

static void ui_vdoplayer_exit_pause_mode(void)
{
    bmap_set_content_by_id(g_pVdoPlayer->hSmallPlayIcon,  IM_PROGRESS_ICON_PLAY);
    ui_vdoplayer_hide_panel();

    bmap_set_content_by_id(g_pVdoPlayer->hLargePlayIcon,  IM_ICON_PLAY);
    ctrl_paint_ctrl(g_pVdoPlayer->hLargePlayIcon, TRUE);
    ui_start_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PLAY_ICON, 1000);
}

static void ui_vdoplayer_update_panel(void)
{
    g_pVdoPlayer->seek_time = g_pVdoPlayer->play_time;
    ui_vdoplayer_set_system_time();
    ui_vdoplayer_set_pbar_by_time(g_pVdoPlayer->play_time);
    ui_vdoplayer_set_seek_time();
    ui_vdoplayer_set_play_time();
}

static void ui_vdoplayer_show_panel(void)
{
    ctrl_set_sts(g_pVdoPlayer->hPanel, OBJ_STS_SHOW);
    ctrl_paint_ctrl(g_pVdoPlayer->hPanel, TRUE);
}

static void ui_vdoplayer_hide_panel(void)
{
    ctrl_set_sts(g_pVdoPlayer->hPanel, OBJ_STS_HIDE);
    ctrl_erase_ctrl(g_pVdoPlayer->hPanel);
}

static void ui_vdoplayer_show_play_icon(void)
{
    ctrl_set_sts(g_pVdoPlayer->hLargePlayIcon, OBJ_STS_SHOW);
    ctrl_paint_ctrl(g_pVdoPlayer->hLargePlayIcon, TRUE);
}

static void ui_vdoplayer_hide_play_icon(void)
{
    ctrl_set_sts(g_pVdoPlayer->hLargePlayIcon, OBJ_STS_HIDE);
    ctrl_erase_ctrl(g_pVdoPlayer->hLargePlayIcon);
}

static void ui_vdoplayer_set_system_time(void)
{
    utc_time_t time;
    u8 time_str[32];

    time_get(&time, FALSE);
    sprintf(time_str, "%.4d-%.2d-%.2d  %.2d:%.2d", time.year, time.month, time.day, time.hour, time.minute);
    text_set_content_by_ascstr(g_pVdoPlayer->hSysTime, time_str);
}

static void ui_vdoplayer_set_pbar_by_time(u32 time)
{
    u16 pbar_curn;

    if (g_pVdoPlayer->total_time > 0)
    {
        pbar_curn = (u16)(time * VDO_PLAYER_PROGRESS_STEP / g_pVdoPlayer->total_time);
        pbar_set_current(g_pVdoPlayer->hProgressBar, pbar_curn);
    }  
}

static void ui_vdoplayer_set_seek_time(void)
{
    utc_time_t seek_time;
    u8 time_str[32];

    memset(&seek_time, 0, sizeof(utc_time_t));
    time_up(&seek_time, g_pVdoPlayer->seek_time);
    sprintf(time_str, "%.2d:%.2d:%.2d", seek_time.hour, seek_time.minute, seek_time.second);
    text_set_content_by_ascstr(g_pVdoPlayer->hSeekTime, time_str);
}

static void ui_vdoplayer_set_play_time(void)
{
    utc_time_t play_time;
    u8 time_str[32];

    memset(&play_time, 0, sizeof(utc_time_t));
    time_up(&play_time, g_pVdoPlayer->play_time);
    sprintf(time_str, "%.2d:%.2d:%.2d", play_time.hour, play_time.minute, play_time.second);
    text_set_content_by_ascstr(g_pVdoPlayer->hPlayTime, time_str);
}

static void ui_vdoplayer_set_total_time(void)
{
    utc_time_t total_time;
    u8 time_str[32];

    memset(&total_time, 0, sizeof(utc_time_t));

    g_pVdoPlayer->total_time = ui_video_c_get_total_time_by_sec();
    time_up(&total_time, g_pVdoPlayer->total_time);
    sprintf(time_str, " / %.2d:%.2d:%.2d", total_time.hour, total_time.minute, total_time.second);
    text_set_content_by_ascstr(g_pVdoPlayer->hTotalTime, time_str);
}

static BOOL ui_vdoplayer_is_repeat_key(u16 msg)
{
    return (g_pVdoPlayer->last_key == msg) ? TRUE : FALSE;
}

static void ui_vdoplayer_open_dlg(u16 str_id, u32 time_out)
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

static void ui_vdoplayer_open_cfm_dlg(u16 str_id)
{
    comm_dlg_data_t dlg_data =
    {
        0,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    dlg_data.content = str_id;
    ui_comm_dlg_open(&dlg_data);
}


/*================================================================================================
                           video player public works function
 ================================================================================================*/

RET_CODE open_video_player(u32 para1, u32 para2)
{
    control_t *p_cont;
    control_t *p_panel, *p_subCtrl;
    control_t *p_largePlayIcon;
    ui_video_player_param *p_param;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_vdoplayer_init_app_data();

///////////////////////////////////////////////////////////////////
    p_param = (ui_video_player_param *)para2;
    g_pVdoPlayer->name = p_param->name;
    g_pVdoPlayer->url_cnt = p_param->url_cnt;
    g_pVdoPlayer->pp_urlList = p_param->pp_urlList;

///////////////////////////////////////////////////////////////////

    /* create */
    p_cont = fw_create_mainwin(ROOT_ID_VIDEO_PLAYER,
                                VDO_PLAYER_CONT_X, VDO_PLAYER_CONT_Y,
                                VDO_PLAYER_CONT_W, VDO_PLAYER_CONT_H,
                                ROOT_ID_INVALID, 0,
                                OBJ_ATTR_ACTIVE, 0);
    MT_ASSERT(p_cont != NULL);

    ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_keymap(p_cont, ui_vdoplayer_cont_keymap);
    ctrl_set_proc(p_cont, ui_vdoplayer_cont_proc);

    //Loadmedia time
    ui_vdoplay_create_loadmedia_time_cont(p_cont);

    //Bps
    ui_vdoplay_create_bps_cont(p_cont);

    //Panel part
    p_panel = ctrl_create_ctrl(CTRL_CONT, IDC_VDO_PLAYER_PANEL,
                                VDO_PLAYER_PANEL_X, VDO_PLAYER_PANEL_Y,
                                VDO_PLAYER_PANEL_W, VDO_PLAYER_PANEL_H, 
                                p_cont, 0);
    ctrl_set_rstyle(p_panel, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    g_pVdoPlayer->hPanel = p_panel;

    //Name
    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_NAME,
                                  VDO_PLAYER_NAME_X, VDO_PLAYER_NAME_Y,
                                  VDO_PLAYER_NAME_W, VDO_PLAYER_NAME_H,
                                  p_panel, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subCtrl, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_subCtrl, g_pVdoPlayer->name);

    //System time
    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_SYSTEM_TIME,
                                  VDO_PLAYER_SYSTEM_TIME_X, VDO_PLAYER_SYSTEM_TIME_Y,
                                  VDO_PLAYER_SYSTEM_TIME_W, VDO_PLAYER_SYSTEM_TIME_H,
                                  p_panel, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subCtrl, STL_CENTER| STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    g_pVdoPlayer->hSysTime = p_subCtrl;

    //Help bar
    ui_comm_help_create_ext(VDO_PLAYER_HELP_BAR_X, VDO_PLAYER_HELP_BAR_Y,
                                VDO_PLAYER_HELP_BAR_W, VDO_PLAYER_HELP_BAR_H,
                                &vdoplayer_help_data,  p_panel);

    //Play icon
    p_subCtrl = ctrl_create_ctrl(CTRL_BMAP, IDC_VDO_PLAYER_SMALL_PLAY_ICON,
                                  VDO_PLAYER_SMALL_PLAY_ICON_X, VDO_PLAYER_SMALL_PLAY_ICON_Y,
                                  VDO_PLAYER_SMALL_PLAY_ICON_W, VDO_PLAYER_SMALL_PLAY_ICON_H, 
                                  p_panel, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_align_type(p_subCtrl, STL_CENTER | STL_VCENTER);
    bmap_set_content_by_id(p_subCtrl,  IM_PROGRESS_ICON_PLAY);
    g_pVdoPlayer->hSmallPlayIcon = p_subCtrl;

    //Playing progress
    p_subCtrl = ctrl_create_ctrl(CTRL_PBAR, IDC_VDO_PLAYER_PROGRESS_BAR,
                                  VDO_PLAYER_PROGRESS_X, VDO_PLAYER_PROGRESS_Y,
                                  VDO_PLAYER_PROGRESS_W, VDO_PLAYER_PROGRESS_H,
                                  p_panel, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_OTT_PROGRESS_BAR_BG, RSI_OTT_PROGRESS_BAR_BG, RSI_OTT_PROGRESS_BAR_BG);
    ctrl_set_mrect(p_subCtrl,
                    VDO_PLAYER_PROGRESS_MIDL, VDO_PLAYER_PROGRESS_MIDT,
                    VDO_PLAYER_PROGRESS_MIDR, VDO_PLAYER_PROGRESS_MIDB);
    pbar_set_rstyle(p_subCtrl, RSI_OTT_PROGRESS_BAR_MID, RSI_IGNORE, INVALID_RSTYLE_IDX);
    pbar_set_count(p_subCtrl, 0, VDO_PLAYER_PROGRESS_STEP, VDO_PLAYER_PROGRESS_STEP);
    pbar_set_direction(p_subCtrl, 1);
    //pbar_set_workmode(p_subCtrl, TRUE, 0);
    pbar_set_current(p_subCtrl, 0);
    g_pVdoPlayer->hProgressBar = p_subCtrl;

    //Seek time
    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_SEEK_TIME,
                                  VDO_PLAYER_SEEK_TIME_X, VDO_PLAYER_SEEK_TIME_Y,
                                  VDO_PLAYER_SEEK_TIME_W, VDO_PLAYER_SEEK_TIME_H,
                                  p_panel, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subCtrl, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    g_pVdoPlayer->hSeekTime = p_subCtrl;

    //Play time
    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_PLAY_TIME,
                                  VDO_PLAYER_PLAY_TIME_X, VDO_PLAYER_PLAY_TIME_Y,
                                  VDO_PLAYER_PLAY_TIME_W, VDO_PLAYER_PLAY_TIME_H,
                                  p_panel, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subCtrl, STL_RIGHT| STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    g_pVdoPlayer->hPlayTime = p_subCtrl;

    //Total time
    p_subCtrl = ctrl_create_ctrl(CTRL_TEXT, IDC_VDO_PLAYER_TOTAL_TIME,
                                  VDO_PLAYER_TOTAL_TIME_X, VDO_PLAYER_TOTAL_TIME_Y,
                                  VDO_PLAYER_TOTAL_TIME_W, VDO_PLAYER_TOTAL_TIME_H,
                                  p_panel, 0);
    ctrl_set_rstyle(p_subCtrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subCtrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_subCtrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_subCtrl, TEXT_STRTYPE_UNICODE);
    g_pVdoPlayer->hTotalTime = p_subCtrl;

    ctrl_set_sts(p_panel, OBJ_STS_HIDE);

    //Large play icon
    p_largePlayIcon = ctrl_create_ctrl(CTRL_BMAP, IDC_VDO_PLAYER_LARGE_PLAY_ICON, 
                                        VDO_PLAYER_LARGE_PLAY_ICON_X, VDO_PLAYER_LARGE_PLAY_ICON_Y,
                                        VDO_PLAYER_LARGE_PLAY_ICON_W, VDO_PLAYER_LARGE_PLAY_ICON_H,
                                        p_cont, 0);
    ctrl_set_rstyle(p_largePlayIcon, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_largePlayIcon, IM_ICON_PAUSE);
    g_pVdoPlayer->hLargePlayIcon = p_largePlayIcon;

    ctrl_set_sts(p_largePlayIcon, OBJ_STS_HIDE);

#if 0
    p_tip = ctrl_create_ctrl(CTRL_BMAP, 255, 900,10,80,40, p_frame, 0);
    if(p_tip)
    {
      ctrl_set_rstyle(p_tip, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      bmap_set_content_by_id(p_tip,  IM_HELP_RED);
    }

    p_tip = ctrl_create_ctrl(CTRL_TEXT, 254, 980, 10, 120, 40, p_frame, 0);
    if(p_tip)
    {
      ctrl_set_rstyle(p_tip, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      text_set_content_type(p_tip, TEXT_STRTYPE_STRID);
      text_set_font_style(p_tip, FSI_WHITE, FSI_BLACK, FSI_GRAY);
      text_set_align_type(p_tip, STL_LEFT | STL_VCENTER);
      text_set_content_by_strid(p_tip, IDS_GO_TO);
    }
#endif

    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

/////////////////////////////////////////////////////////////////////////////
    if (g_pVdoPlayer->url_cnt == 1)
    {
        ui_video_c_play_by_url(g_pVdoPlayer->pp_urlList[0], 0, VDO_PLAY_MODE_VOD);
    }
    else
    {
        ui_video_c_play_by_url_ex(g_pVdoPlayer->pp_urlList, g_pVdoPlayer->url_cnt, 0, VDO_PLAY_MODE_VOD);
    }

  ui_vdoplayer_open_dlg(IDS_LOADING_WITH_WAIT, 0);
  g_pVdoPlayer->b_showloading = TRUE;

  return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);

    if (g_pVdoPlayer->b_showloading)
    {
        ui_comm_dlg_close();
    }

    if (ui_is_mute())
    {
        close_mute();
    }

    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PANEL);
    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PLAY_ICON);
    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_STOP_REPEAT_KEY);

    ui_vdoplayer_release_app_data();
/////////////////////////////////////////////////////////////////////////////
    p_root = fw_find_root_by_id(ROOT_ID_AUDIO_SET);//fix bug 31430
    if(p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }
    p_root = fw_find_root_by_id(ROOT_ID_ONMOV_DESCRIPTION);
    if(p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    p_root = fw_find_root_by_id(ROOT_ID_YOUTUBE);
    if(p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    p_root = fw_find_root_by_id(ROOT_ID_REDTUBE);
    if(p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    p_root = fw_find_root_by_id(ROOT_ID_NETMEDIA);
    if(p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }

    return ERR_NOFEATURE;
}

static RET_CODE ui_vdoplayer_on_update_loadmedia_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s, time=%d\n", __FUNCTION__, para1);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    if (ctrl_get_sts(g_pVdoPlayer->hLoadMediaTimeCont) == OBJ_STS_HIDE)
    {
        ctrl_set_sts(g_pVdoPlayer->hLoadMediaTimeCont, OBJ_STS_SHOW);
    }

    ui_vdoplayer_update_load_media_time(para1);
    ctrl_paint_ctrl(g_pVdoPlayer->hLoadMediaTimeCont, TRUE);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_loadmedia_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    ui_vdoplayer_hide_loadmedia_time();
    if (g_pVdoPlayer->b_showloading)
    {
        ui_comm_dlg_close();
        g_pVdoPlayer->b_showloading = FALSE;
    }

    ui_vdoplayer_set_total_time();

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_loadmedia_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    ui_vdoplayer_hide_loadmedia_time();

    manage_close_menu(ROOT_ID_VIDEO_PLAYER, 0, 0);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_update_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    u16 net_bps, percent;

    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    ui_vdoplayer_get_bps_infos(para1, &percent, &net_bps);
    OS_PRINTF("@@@%s, net_bps=%d, percent=%d\n", __FUNCTION__, net_bps, percent);

    if (ctrl_get_sts(g_pVdoPlayer->hBpsCont) == OBJ_STS_HIDE)
    {
        ctrl_set_sts(g_pVdoPlayer->hBpsCont, OBJ_STS_SHOW);
    }

    ui_vdoplayer_update_bps(net_bps, percent);
    ctrl_paint_ctrl(g_pVdoPlayer->hBpsCont, TRUE);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_hide_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    ctrl_set_sts(g_pVdoPlayer->hBpsCont, OBJ_STS_HIDE);
    ctrl_erase_ctrl(g_pVdoPlayer->hBpsCont);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_update_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    g_pVdoPlayer->play_time = (para1/1000);
    if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
    {
        ui_vdoplayer_set_play_time();
        ctrl_paint_ctrl(g_pVdoPlayer->hPlayTime, TRUE);
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_change_progress(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;
    s32 seek_time;
    osd_set_t osd_set;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            if (ui_vdoplayer_is_repeat_key(msg))
            {
                g_pVdoPlayer->jump_time += SEEK_STEP_INCR;
                if (g_pVdoPlayer->jump_time > MAX_SEEK_STEP)
                {
                    g_pVdoPlayer->jump_time = MAX_SEEK_STEP;
                }
            }
            else
            {
                g_pVdoPlayer->jump_time = MIN_SEEK_STEP;
            }

            seek_time = g_pVdoPlayer->seek_time;
            if (msg == MSG_FOCUS_LEFT)
            {
                seek_time -= g_pVdoPlayer->jump_time;

                if (seek_time < 0)
                {
                    seek_time = 0;
                }
            }
            else if (msg == MSG_FOCUS_RIGHT)
            {
                seek_time += g_pVdoPlayer->jump_time;

                if (seek_time > (s32)g_pVdoPlayer->total_time)
                {
                    seek_time = g_pVdoPlayer->total_time;
                }
            }
            if (seek_time != g_pVdoPlayer->seek_time)
            {
                g_pVdoPlayer->seek_time = seek_time;
                ui_vdoplayer_set_pbar_by_time(g_pVdoPlayer->seek_time);
                ui_vdoplayer_set_seek_time();

                ctrl_paint_ctrl(g_pVdoPlayer->hProgressBar, TRUE);
                ctrl_paint_ctrl(g_pVdoPlayer->hSeekTime, TRUE);
            }

            ui_start_timer(ROOT_ID_VIDEO_PLAYER, MSG_STOP_REPEAT_KEY, 250);
            g_pVdoPlayer->last_key = msg;

            sys_status_get_osd_set(&osd_set);
            ui_start_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PANEL, (osd_set.timeout*1000));
        }
        else
        {
            ui_vdoplayer_enter_seek_mode();
        }
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_change_volume(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            ui_vdoplayer_exit_seek_mode();
        }

        manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_VIDEO_PLAYER, para1);
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_leftright(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_HIDE)
    {
        ui_vdoplayer_on_change_volume(p_cont, msg, para1, para2);
    }
    else
    {
        ui_vdoplayer_on_change_progress(p_cont, msg, para1, para2);
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_select(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            ui_video_c_pause_resume();
            ui_video_c_seek(g_pVdoPlayer->seek_time);

            ui_vdoplayer_exit_seek_mode();
        }
        else
        {
            ui_vdoplayer_enter_seek_mode();
        }
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_show_hide_panel(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            ui_vdoplayer_exit_seek_mode();
        }
        else
        {
            ui_vdoplayer_enter_seek_mode();
        }
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_hide_panel(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            ui_vdoplayer_exit_seek_mode();
        }
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_pause_resume(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY || _state == MUL_PLAY_STATE_PAUSE)
    {
        ui_video_c_pause_resume();

        _state = ui_video_c_get_play_state();
        if (_state == MUL_PLAY_STATE_PAUSE)
        {
            ui_vdoplayer_enter_pause_mode();
        }
        else if (_state == MUL_PLAY_STATE_PLAY)
        {
            ui_vdoplayer_exit_pause_mode();
        }
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_hide_play_icon(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    if (ctrl_get_sts(g_pVdoPlayer->hLargePlayIcon) == OBJ_STS_SHOW)
    {
        ui_vdoplayer_hide_play_icon();
    }

    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_HIDE_PLAY_ICON);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_stop_repeat_key(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    g_pVdoPlayer->last_key = MSG_INVALID;
    ui_stop_timer(ROOT_ID_VIDEO_PLAYER, MSG_STOP_REPEAT_KEY);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_mute(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    ui_set_mute(!ui_is_mute());

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_change_aspect(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    av_set_t av_set;
    class_handle_t avc_handle;

    avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
    sys_status_get_av_set(&av_set);

    if (1 == av_set.tv_ratio)
    {
        av_set.tv_ratio = 2;
    }
    else
    {
        av_set.tv_ratio = 1;
    } 
    avc_set_video_aspect_mode_1(avc_handle, sys_status_get_video_aspect(av_set.tv_ratio));

    sys_status_set_av_set(&av_set);
    sys_status_save();

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_open_audio_set(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            ui_vdoplayer_exit_seek_mode();
        }

        manage_open_menu(ROOT_ID_FP_AUDIO_SET, para1, 0);
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_open_subtitle(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            ui_vdoplayer_exit_seek_mode();
        }

        manage_open_menu(ROOT_ID_FILEPLAY_SUBT, 0, 0);
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_show_goto(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t cur_state;

    cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();
    if (cur_state == MUL_PLAY_STATE_PLAY || cur_state == MUL_PLAY_STATE_PAUSE)
    {
        if (cur_state == MUL_PLAY_STATE_PLAY)
        {
            ui_vdoplayer_on_pause_resume(p_cont, msg, para1, para2);
        }

        manage_open_menu(ROOT_ID_VIDEO_GOTO, g_pVdoPlayer->play_time, ROOT_ID_VIDEO_PLAYER);
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_goto_done(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        ui_vdoplayer_exit_pause_mode();
    }

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_user_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    ui_video_c_stop();
    manage_close_menu(ROOT_ID_VIDEO_PLAYER, 0, 0);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_cancel(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (ctrl_get_sts(g_pVdoPlayer->hPanel) == OBJ_STS_SHOW)
        {
            ui_vdoplayer_exit_seek_mode();

            return SUCCESS;
        }
    }

    ui_vdoplayer_on_user_exit(p_cont, msg, para1, para2);

    return SUCCESS;
}

static RET_CODE ui_vdoplayer_on_eos(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_VIDEO_PLAYER)
    {
        return SUCCESS;
    }

    manage_close_menu(ROOT_ID_VIDEO_PLAYER, 0, 0);

    ui_vdoplayer_open_cfm_dlg(IDS_THANKS_4_WATCHING);

    return SUCCESS;
}

BEGIN_KEYMAP(ui_vdoplayer_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
    ON_EVENT(V_KEY_PAUSE, MSG_PAUSE)
    ON_EVENT(V_KEY_VUP, MSG_INCREASE)
    ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
    ON_EVENT(V_KEY_MUTE, MSG_MUTE)
    ON_EVENT(V_KEY_YELLOW, MSG_CHANGE_ASPECT)
    ON_EVENT(V_KEY_AUDIO, MSG_AUDIO_SET)
    ON_EVENT(V_KEY_RED, MSG_VP_SHOW_GOTO)
    ON_EVENT(V_KEY_SUBT, MSG_SUBTITLE)
    ON_EVENT(V_KEY_STOP, MSG_STOP)
END_KEYMAP(ui_vdoplayer_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ui_vdoplayer_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_TIME, ui_vdoplayer_on_update_loadmedia_time)
    ON_COMMAND(MSG_VIDEO_EVENT_UPDATE_BPS, ui_vdoplayer_on_update_bps)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, ui_vdoplayer_on_update_time)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, ui_vdoplayer_on_loadmedia_success)
    ON_COMMAND(MSG_VIDEO_EVENT_FINISH_BUFFERING, ui_vdoplayer_on_hide_bps)
    ON_COMMAND(MSG_VIDEO_EVENT_FINISH_UPDATE_BPS, ui_vdoplayer_on_hide_bps)

    ON_COMMAND(MSG_FOCUS_LEFT, ui_vdoplayer_on_leftright)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_vdoplayer_on_leftright)
    ON_COMMAND(MSG_SELECT, ui_vdoplayer_on_select)
    ON_COMMAND(MSG_INFO, ui_vdoplayer_on_show_hide_panel)
    ON_COMMAND(MSG_EXIT_ALL, ui_vdoplayer_on_cancel)
    ON_COMMAND(MSG_PAUSE, ui_vdoplayer_on_pause_resume)
    ON_COMMAND(MSG_INCREASE, ui_vdoplayer_on_change_volume)
    ON_COMMAND(MSG_DECREASE, ui_vdoplayer_on_change_volume)

    ON_COMMAND(MSG_HIDE_PANEL, ui_vdoplayer_on_hide_panel)
    ON_COMMAND(MSG_HIDE_PLAY_ICON, ui_vdoplayer_on_hide_play_icon)
    ON_COMMAND(MSG_STOP_REPEAT_KEY, ui_vdoplayer_on_stop_repeat_key)

    ON_COMMAND(MSG_MUTE, ui_vdoplayer_on_mute)
    ON_COMMAND(MSG_CHANGE_ASPECT, ui_vdoplayer_on_change_aspect)
    ON_COMMAND(MSG_AUDIO_SET, ui_vdoplayer_on_open_audio_set)
    ON_COMMAND(MSG_SUBTITLE, ui_vdoplayer_on_open_subtitle)
    ON_COMMAND(MSG_VP_SHOW_GOTO, ui_vdoplayer_on_show_goto)
    ON_COMMAND(MSG_GOTO_DONE, ui_vdoplayer_on_goto_done)

    ON_COMMAND(MSG_DESTROY, ui_vdoplayer_on_destory)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_vdoplayer_on_user_exit)
    ON_COMMAND(MSG_EXIT, ui_vdoplayer_on_user_exit)
    ON_COMMAND(MSG_STOP, ui_vdoplayer_on_user_exit)
    ON_COMMAND(MSG_VIDEO_EVENT_EOS, ui_vdoplayer_on_eos)
    ON_COMMAND(MSG_VIDEO_EVENT_SET_PATH_FAIL, ui_vdoplayer_on_loadmedia_error)
    ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO, ui_vdoplayer_on_loadmedia_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, ui_vdoplayer_on_loadmedia_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, ui_vdoplayer_on_loadmedia_error)
END_MSGPROC(ui_vdoplayer_cont_proc, ui_comm_root_proc)

#endif



