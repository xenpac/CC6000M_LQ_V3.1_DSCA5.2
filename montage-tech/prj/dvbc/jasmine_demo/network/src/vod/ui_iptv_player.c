/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"

#if ENABLE_NETWORK
#include "ui_iptv_player.h"
//#include "ui_onmov_provider.h"
#include "ui_pause.h"
#include "ui_video.h"
#include "ui_ping_test.h"
#include "ui_mute.h"
#include "netplay.h"
#include "commonData.h"
#include "IPTVDataProvider.h"
#include "ui_iptv_api.h"
#include "file_play_api.h"
#include "ui_iptv_prot.h"
#include "db_play_hist.h"
#include "ui_iptv_description.h"

/*!
 * Const value
 */
#define MAX_EPISODE_NAME_LEN    (MAX_PG_NAME_LEN + 10)
 /*!
 * Macro
 */

/*!
 * Type define
 */
enum control_id
{
    IDC_IPTV_PLAYER_LOADING_MEDIA_TIME_CONT = 1,
    IDC_IPTV_PLAYER_LOADING_MEDIA_TIME,
    IDC_IPTV_PLAYER_BPS_CONT_CONT,
    IDC_IPTV_PLAYER_BPS_CONT,
    IDC_IPTV_PLAYER_BPS, 
    IDC_IPTV_PLAYER_BUF_PERCENT,

    IDC_IPTV_PLAYER_PANEL,
    IDC_IPTV_PLAYER_EPISODE_NAME,
    IDC_IPTV_PLAYER_SYSTEM_TIME,
    IDC_IPTV_PLAYER_SMALL_PLAY_ICON,
    IDC_IPTV_PLAYER_PROGRESS_BAR,
    IDC_IPTV_PLAYER_SEEK_TIME,
    IDC_IPTV_PLAYER_PLAY_TIME,
    IDC_IPTV_PLAYER_TOTAL_TIME,

    IDC_IPTV_PLAYER_LARGE_PLAY_ICON,

    IDC_IPTV_PLAYER_FORMAT_LIST_CONT,
    IDC_IPTV_PLAYER_FORMAT_LIST,
};

typedef struct
{
    u16 name;
    u8  id;
} ui_iptv_player_format_t;

typedef struct
{
    control_t *hLoadMediaTime;
    control_t *hBpsCont;
    control_t *hBps;
    control_t *hBufPercent;

    control_t *hPanel;
    control_t *hEpisodeName;
    control_t *hSysTime;
    control_t *hSmallPlayIcon;
    control_t *hProgressBar;

    control_t *hSeekTime;
    control_t *hPlayTime;
    control_t *hTotalTime;

    control_t *hLargePlayIcon;

    control_t *hFmtListCont;
    control_t *hFmtList;

    u16 pg_name[MAX_PG_NAME_LEN + 1];   //program name
    u16 key[MAX_PG_NAME_LEN + 1];  //subject name
    u16 episode_name[MAX_EPISODE_NAME_LEN + 1];

    u8  *pInfoUrl;

    u32 vdo_id;
    u32 res_id;
    u32 play_time;

    u16 total_episode;
    u16 episode_num;
    u8  b_single_page;

    u8  category;

    u8  total_format;
    ui_iptv_player_format_t formatList[IPTV_MAX_FORMAT_COUNT];
    u8  format_index;

    u16 total_url;
    u8  **pp_urlList;

    u32 total_time;
    u32 seek_time;
    u16 jump_time;

    u16 last_key;

    BOOL b_showloading;
    BOOL b_showbar;
} ui_iptv_player_app_t;

/*!
 * Function define
 */
extern void ui_livetv_get_bps_infos(u32 param, u16 *percent, u16 *bps);

u16 ui_iptvplayer_cont_keymap(u16 key);
RET_CODE ui_iptvplayer_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
u16 ui_iptvplayer_panel_keymap(u16 key);
RET_CODE ui_iptvplayer_panel_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
u16 ui_iptvplayer_formatlist_keymap(u16 key);
RET_CODE ui_iptvplayer_formatlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static RET_CODE ui_iptvplayer_on_formatlist_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE ui_iptvplayer_on_pause_resume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static BOOL ui_iptvplayer_release_app_data(void);
static void ui_iptvplayer_release_urllist(void);
static RET_CODE ui_close_iptv_player(void);
static void ui_iptvplayer_open_dlg(void);
static void ui_iptvplayer_open_cfm_dlg(u16 str_id);

static RET_CODE ui_iptvplayer_on_hide_load_media_time(void);

/*!
 * Priv data
 */
// Resource list style
static list_xstyle_t formatlist_item_rstyle =
{
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
};

static list_xstyle_t formatlist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t formatlist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE_22,
    FSI_WHITE_22,
    FSI_WHITE_22,
    FSI_WHITE_22,
};
static const list_field_attr_ext_t formatlist_attr[] =
{
    {
        LISTFIELD_TYPE_ICON | STL_CENTER | STL_VCENTER,
        IPTV_PLAYER_FORMAT_LIST_ICON_W,
        IPTV_PLAYER_FORMAT_LIST_ICON_H,
        IPTV_PLAYER_FORMAT_LIST_ICON_X,
        IPTV_PLAYER_FORMAT_LIST_ICON_Y,
        &formatlist_field_rstyle,
        &formatlist_field_fstyle
    },
    {
        LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
        IPTV_PLAYER_FORMAT_LIST_NAME_W,
        IPTV_PLAYER_FORMAT_LIST_NAME_H,
        IPTV_PLAYER_FORMAT_LIST_NAME_X,
        IPTV_PLAYER_FORMAT_LIST_NAME_Y,
        &formatlist_field_rstyle,
        &formatlist_field_fstyle
    }
};

static ui_iptv_player_app_t *g_pIptvPlayer = NULL;

/*================================================================================================
                           iptv player internel function
 ================================================================================================*/

static RET_CODE ui_iptvplayer_update_formatlist(control_t *p_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt, focus;
    u16 icon_id;
    u8  item_status;

    OS_PRINTF("@@@ui_iptvplayer_update_formatlist\n");
    cnt = list_get_count(p_list);
    focus = list_get_focus_pos(p_list);

    for (i = start; i < start + size && i < cnt; i++)
    {
        item_status = list_get_item_status(p_list, i);
        if (i == focus)
        {
            if (item_status == LIST_ITEM_SELECTED)
            {
                icon_id = IM_PLAYOPTION_4;
            }
            else
            {
                icon_id = IM_PLAYOPTION_3;
            }
        }
        else
        {
            if (item_status == LIST_ITEM_SELECTED)
            {
                icon_id = IM_PLAYOPTION_2;
            }
            else
            {
                icon_id = IM_PLAYOPTION_1;
            }
        }
        list_set_field_content_by_icon(p_list, (u16)i, 0, icon_id);
        list_set_field_content_by_strid(p_list, (u16)i, 1, g_pIptvPlayer->formatList[i].name);
    }

    return SUCCESS;
}

static void ui_iptvplayer_show_panel(void)
{
    //if (g_pIptvPlayer->b_showbar == FALSE)
    //{
        ctrl_set_sts(g_pIptvPlayer->hPanel, OBJ_STS_SHOW);
        ctrl_paint_ctrl(g_pIptvPlayer->hPanel, TRUE);
        g_pIptvPlayer->b_showbar = TRUE;
    //}
}

static void ui_iptvplayer_hide_panel(void)
{
    //if (g_pIptvPlayer->b_showbar == TRUE)
    //{
        ctrl_set_sts(g_pIptvPlayer->hPanel, OBJ_STS_HIDE);
        ctrl_erase_ctrl(g_pIptvPlayer->hPanel);
        g_pIptvPlayer->b_showbar = FALSE;
    //}
}

static void ui_iptvplayer_show_play_icon(void)
{
    ctrl_set_sts(g_pIptvPlayer->hLargePlayIcon, OBJ_STS_SHOW);
    ctrl_paint_ctrl(g_pIptvPlayer->hLargePlayIcon, TRUE);
}

static void ui_iptvplayer_hide_play_icon(void)
{
    ctrl_set_sts(g_pIptvPlayer->hLargePlayIcon, OBJ_STS_HIDE);
    ctrl_erase_ctrl(g_pIptvPlayer->hLargePlayIcon);
}

static BOOL ui_iptvplayer_is_repeat_key(u16 msg)
{
    return (g_pIptvPlayer->last_key == msg) ? TRUE : FALSE;
}

static void ui_iptvplayer_update_load_media_time(u32 second)
{
    u16 *p_unistr;
    u16 uni_str1[100+1];
    u16 uni_str2[10+1];
    u8  asc_str[10+1];

    OS_PRINTF("@@@%s, second=%d\n", __FUNCTION__, second);
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

    text_set_content_by_unistr(g_pIptvPlayer->hLoadMediaTime, uni_str1);
}

static void ui_iptvplayer_update_bps(u16 cur_bps, u16 load_percent)
{
    u16 *p_unistr;
    u16 uni_str1[100+1];
    u16 uni_str2[16+1];
    u8  asc_str[16+1];

    OS_PRINTF("@@@%s, bps=%d, percent=%d\n", __FUNCTION__, cur_bps, load_percent);
    //bps
    memset(uni_str1, 0, sizeof(uni_str1));
    memset(uni_str2, 0, sizeof(uni_str2));
    memset(asc_str, 0, sizeof(asc_str));

    p_unistr = (u16 *)gui_get_string_addr(IDS_DOWNLOAD_SPEED);
    uni_strcpy(uni_str1, p_unistr);

    sprintf(asc_str, "%dKB/s", cur_bps);
    str_asc2uni(asc_str, uni_str2);
    uni_strcat(uni_str1, uni_str2, 100);

    text_set_content_by_unistr(g_pIptvPlayer->hBps, uni_str1);

    //percent
    memset(uni_str1, 0, sizeof(uni_str1));
    memset(uni_str2, 0, sizeof(uni_str2));
    memset(asc_str, 0, sizeof(asc_str));

    p_unistr = (u16 *)gui_get_string_addr(IDS_BUFFERED);
    uni_strcpy(uni_str1, p_unistr);

    sprintf(asc_str, "%d%%", load_percent);
    str_asc2uni(asc_str, uni_str2);
    uni_strcat(uni_str1, uni_str2, 100);

    text_set_content_by_unistr(g_pIptvPlayer->hBufPercent, uni_str1);
}

static void ui_iptvplayer_set_system_time(void)
{
    utc_time_t time;
    u8 time_str[32];

    time_get(&time, FALSE);
    sprintf(time_str, "%.4d-%.2d-%.2d  %.2d:%.2d", time.year, time.month, time.day, time.hour, time.minute);
    text_set_content_by_ascstr(g_pIptvPlayer->hSysTime, time_str);
}

static void ui_iptvplayer_set_pbar_by_time(u32 time)
{
    u16 pbar_curn;

    if (g_pIptvPlayer->total_time > 0)
    {
        pbar_curn = (u16)(time * IPTV_PLAYER_PROGRESS_STEP / g_pIptvPlayer->total_time);
        pbar_set_current(g_pIptvPlayer->hProgressBar, pbar_curn);
    }  
}

static void ui_iptvplayer_set_seek_time(void)
{
    utc_time_t seek_time;
    u8 time_str[32];

    memset(&seek_time, 0, sizeof(utc_time_t));
    time_up(&seek_time, g_pIptvPlayer->seek_time);
    sprintf(time_str, "%.2d:%.2d:%.2d", seek_time.hour, seek_time.minute, seek_time.second);
    text_set_content_by_ascstr(g_pIptvPlayer->hSeekTime, time_str);
}

static void ui_iptvplayer_set_play_time(void)
{
    utc_time_t play_time;
    u8 time_str[32];

    memset(&play_time, 0, sizeof(utc_time_t));
    time_up(&play_time, g_pIptvPlayer->play_time);
    sprintf(time_str, "%.2d:%.2d:%.2d", play_time.hour, play_time.minute, play_time.second);
    text_set_content_by_ascstr(g_pIptvPlayer->hPlayTime, time_str);
}

static void ui_iptvplayer_set_total_time(void)
{
    utc_time_t total_time;
    u8 time_str[32];

    memset(&total_time, 0, sizeof(utc_time_t));

    g_pIptvPlayer->total_time = ui_video_c_get_total_time_by_sec();
    time_up(&total_time, g_pIptvPlayer->total_time);
    sprintf(time_str, " / %.2d:%.2d:%.2d", total_time.hour, total_time.minute, total_time.second);
    text_set_content_by_ascstr(g_pIptvPlayer->hTotalTime, time_str);
}

static void ui_iptvplayer_init_episode_info(void)
{
    u16 *p_unistr;
    u16 uni_str[5 + 1];
    u8  asc_str[5 + 1];

    if (g_pIptvPlayer->category == IPTV_MOVIE)
    {
        uni_strcpy(g_pIptvPlayer->key, g_pIptvPlayer->pg_name);
        uni_strcpy(g_pIptvPlayer->episode_name, g_pIptvPlayer->pg_name);
    }
    else if (g_pIptvPlayer->category == IPTV_TV)
    {
        uni_strcpy(g_pIptvPlayer->key, g_pIptvPlayer->pg_name);

        uni_strcpy(g_pIptvPlayer->episode_name, g_pIptvPlayer->pg_name);

        str_asc2uni(" ", uni_str);
        uni_strcat(g_pIptvPlayer->episode_name, uni_str, MAX_EPISODE_NAME_LEN);

        p_unistr = (u16 *)gui_get_string_addr(IDS_EPISODE);
        uni_strcat(g_pIptvPlayer->episode_name, p_unistr, MAX_EPISODE_NAME_LEN);

        sprintf(asc_str, " %d ", g_pIptvPlayer->episode_num + 1);
        str_asc2uni(asc_str, uni_str);
        uni_strcat(g_pIptvPlayer->episode_name, uni_str, MAX_EPISODE_NAME_LEN);
    }
    else if (g_pIptvPlayer->category == IPTV_VARIETY)
    {
        p_unistr = ui_iptvdesc_get_episode_name(g_pIptvPlayer->category, g_pIptvPlayer->episode_num);
        if (p_unistr)
        {
            uni_strncpy(g_pIptvPlayer->key, p_unistr, MAX_PG_NAME_LEN);
            uni_strcpy(g_pIptvPlayer->episode_name, g_pIptvPlayer->key);
        }
        else
        {
            g_pIptvPlayer->key[0] = '\0';
            g_pIptvPlayer->episode_name[0] = '\0';
        }
    }

    g_pIptvPlayer->pInfoUrl = ui_iptvdesc_get_info_url(g_pIptvPlayer->category, g_pIptvPlayer->episode_num);
}

/*================================================================================================
                           iptv player msg resp function
 ================================================================================================*/

static RET_CODE ui_iptvplayer_on_formatlist_leftright(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    list_class_proc(p_ctrl, msg, para1, para2);
    focus = list_get_focus_pos(p_ctrl);

    ui_iptvplayer_update_formatlist(p_ctrl, list_get_valid_pos(p_ctrl), IPTV_MAX_FORMAT_COUNT, 0);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_formatlist_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;
    u16 focus;

    OS_PRINTF("@@@ui_iptvplayer_on_formatlist_select\n");
    focus = list_get_focus_pos(p_ctrl);

    if (focus == g_pIptvPlayer->format_index)
    {
        ui_iptvplayer_on_formatlist_hide(p_ctrl, msg, para1, para2);
        return SUCCESS;
    }

    list_select_item(p_ctrl, focus);
    g_pIptvPlayer->format_index = focus;
    ui_iptvplayer_on_formatlist_hide(p_ctrl, msg, para1, para2);

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PAUSE)
    {
        ui_iptvplayer_hide_panel();
        ui_iptvplayer_hide_play_icon();
    }

    ui_iptvplayer_open_dlg();
    g_pIptvPlayer->b_showloading = TRUE;

    //ui_video_c_force_stop();
    ui_iptv_get_play_url(g_pIptvPlayer->formatList[g_pIptvPlayer->format_index].id, g_pIptvPlayer->pInfoUrl);

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_formatlist_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_iptvplayer_on_formatlist_hide\n");
    ctrl_process_msg(g_pIptvPlayer->hFmtList, MSG_LOSTFOCUS, 0, 0);
    ctrl_set_sts(g_pIptvPlayer->hFmtListCont, OBJ_STS_HIDE);

    ctrl_set_attr(g_pIptvPlayer->hPanel, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(g_pIptvPlayer->hPanel, MSG_GETFOCUS, 0, 0);

    ctrl_erase_ctrl(g_pIptvPlayer->hFmtListCont);
    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_update_load_media_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s, time=%d\n", __FUNCTION__, para1);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_IPTV_PLAYER)
    {
        return SUCCESS;
    }

    if (ctrl_get_sts(g_pIptvPlayer->hLoadMediaTime) == OBJ_STS_HIDE)
    {
        ui_iptvplayer_update_load_media_time(para1);
        ctrl_set_sts(g_pIptvPlayer->hLoadMediaTime, OBJ_STS_SHOW);
        ctrl_paint_ctrl(g_pIptvPlayer->hLoadMediaTime, TRUE);
    }
    else
    {
        ui_iptvplayer_update_load_media_time(para1);
        ctrl_paint_ctrl(g_pIptvPlayer->hLoadMediaTime, TRUE);
    }

    return SUCCESS;
}


static RET_CODE ui_iptvplayer_on_update_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    u16 net_bps = 0;
    u16 percent = 0;

    ui_livetv_get_bps_infos(para1, &percent, &net_bps);
    OS_PRINTF("@@@%s, para1=%d, net_bps=%d, percent=%d\n", __FUNCTION__, para1, net_bps, percent);

    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_IPTV_PLAYER)
    {
        return SUCCESS;
    }

    if (ctrl_get_sts(g_pIptvPlayer->hBpsCont) == OBJ_STS_HIDE)
    {
        ui_iptvplayer_update_bps(net_bps, percent);
        ctrl_set_sts(g_pIptvPlayer->hBpsCont, OBJ_STS_SHOW);
        ctrl_paint_ctrl(g_pIptvPlayer->hBpsCont, TRUE);
    }
    else
    {
        ui_iptvplayer_update_bps(net_bps, percent);
        ctrl_paint_ctrl(g_pIptvPlayer->hBpsCont, TRUE);
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_update_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_IPTV_PLAYER)
    {
        return SUCCESS;
    }

    g_pIptvPlayer->play_time = (para1/1000);
    if(g_pIptvPlayer->b_showbar)
    {
      ui_iptvplayer_set_play_time();
      ctrl_paint_ctrl(g_pIptvPlayer->hPanel, TRUE);
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_source_format_arrive(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    al_iptv_source_format_t *p_data = (al_iptv_source_format_t *)para1;
    u8 i;

    OS_PRINTF("@@@ui_iptvplayer_on_source_format_arrive() total_format=%d\n", p_data->total_format);
    for (i = 0; i < p_data->total_format; i++)
    {
        if (p_data->formatList[i] == IPTV_API_FORMAT_NORMAL)
        {
            g_pIptvPlayer->formatList[i].name = IDS_NORMAL_FORMAT;
        }
        else if (p_data->formatList[i] == IPTV_API_FORMAT_HIGH)
        {
            g_pIptvPlayer->formatList[i].name = IDS_HIGH_FORMAT;
        }
        else if (p_data->formatList[i] == IPTV_API_FORMAT_SUPER)
        {
            g_pIptvPlayer->formatList[i].name = IDS_SUPER_FORMAT;
        }
        else if (p_data->formatList[i] == IPTV_API_FORMAT_TOPSPEED)
        {
            g_pIptvPlayer->formatList[i].name = IDS_TOPSPEED_FORMAT;
        }
        g_pIptvPlayer->formatList[i].id = p_data->formatList[i];
    }
    g_pIptvPlayer->total_format = p_data->total_format;
#if 1
    g_pIptvPlayer->format_index = 0;
#else
    if (g_pIptvPlayer->total_format >= 2)
    {
        g_pIptvPlayer->format_index = 1;
    }
    else
    {
        g_pIptvPlayer->format_index = 0;
    }
#endif

    list_set_count(g_pIptvPlayer->hFmtList, g_pIptvPlayer->total_format, IPTV_MAX_FORMAT_COUNT);
    list_set_focus_pos(g_pIptvPlayer->hFmtList, g_pIptvPlayer->format_index);
    list_select_item(g_pIptvPlayer->hFmtList, g_pIptvPlayer->format_index);

    OS_PRINTF("@@@format_index=%d, id=%d, info_url=%s\n", g_pIptvPlayer->format_index, g_pIptvPlayer->formatList[g_pIptvPlayer->format_index].id, g_pIptvPlayer->pInfoUrl);
    ui_iptv_get_play_url(g_pIptvPlayer->formatList[g_pIptvPlayer->format_index].id, g_pIptvPlayer->pInfoUrl);

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_play_url_arrive(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    al_iptv_play_url_list_t *p_data = (al_iptv_play_url_list_t *)para1;
    u8  *p_url;
    u16 i;
    u16 str_len;

    OS_PRINTF("@@@ui_iptvplayer_on_play_url_arrive()\n");
    if (p_data->total_url > 0 && p_data->pp_urlList != NULL)
    {
        ui_iptvplayer_release_urllist();

        g_pIptvPlayer->pp_urlList = (u8 **)mtos_malloc(p_data->total_url * sizeof(u8 *));
        MT_ASSERT(g_pIptvPlayer->pp_urlList != NULL);

        for (i = 0; i < p_data->total_url; i++)
        {
            str_len = strlen(p_data->pp_urlList[i]);
            p_url = (u8 *)mtos_malloc(str_len + 1);
            MT_ASSERT(p_url != NULL);
            strcpy(p_url, p_data->pp_urlList[i]);
            g_pIptvPlayer->pp_urlList[i] = p_url;
        }
        g_pIptvPlayer->total_url = p_data->total_url;

        if (g_pIptvPlayer->total_url == 1)
        {
            ui_video_c_play_by_url(g_pIptvPlayer->pp_urlList[0], g_pIptvPlayer->play_time, OTT_VOD_MODE);
        }
        else
        {
            ui_video_c_play_by_url_ex(g_pIptvPlayer->pp_urlList, g_pIptvPlayer->total_url, g_pIptvPlayer->play_time, OTT_VOD_MODE);
        }
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_load_media_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    play_hist_info info;

    OS_PRINTF("@@@ui_iptvplayer_on_load_media_success()\n");
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_IPTV_PLAYER)
    {
        return SUCCESS;
    }

    ui_iptvplayer_on_hide_load_media_time();

    if (g_pIptvPlayer->b_showloading)
    {
        ui_comm_dlg_close();
        g_pIptvPlayer->b_showloading = FALSE;
    }

    ui_iptvplayer_set_total_time();

    info.vdo_id = g_pIptvPlayer->vdo_id;
    info.res_id = g_pIptvPlayer->res_id;
    info.b_single_page = g_pIptvPlayer->b_single_page;
    uni_strcpy(info.pg_name, g_pIptvPlayer->pg_name);
    db_plht_add_unique_item(&info);

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_eos(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_iptvplayer_on_eos()\n");
    if (g_pIptvPlayer->episode_num < g_pIptvPlayer->total_episode - 1)
    {
        g_pIptvPlayer->episode_num++;
        ui_iptvplayer_init_episode_info();

        text_set_content_by_extstr(g_pIptvPlayer->hEpisodeName, (u16 *)g_pIptvPlayer->episode_name);

        if (g_pIptvPlayer->pInfoUrl != NULL)
        {
            g_pIptvPlayer->play_time = 0;
            ui_iptvplayer_open_dlg();
            g_pIptvPlayer->b_showloading = TRUE;
            ui_iptv_get_play_format(g_pIptvPlayer->pInfoUrl);
        }
    }
    else
    {
        ui_close_iptv_player();
        ui_iptvplayer_open_cfm_dlg(IDS_THANKS_4_WATCHING);
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_iptvplayer_on_error() msg=%d\n", msg);
    ui_iptvplayer_on_hide_load_media_time();
    ui_close_iptv_player();

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_hide_load_media_time(void)
{
    ctrl_set_sts(g_pIptvPlayer->hLoadMediaTime, OBJ_STS_HIDE);
    ctrl_erase_ctrl(g_pIptvPlayer->hLoadMediaTime);

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_hide_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_set_sts(g_pIptvPlayer->hBpsCont, OBJ_STS_HIDE);
    ctrl_erase_ctrl(g_pIptvPlayer->hBpsCont);

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_hide_play_icon(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_iptvplayer_on_hide_play_icon()\n");
    ui_iptvplayer_hide_play_icon();
    ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PLAY_ICON);

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_hide_panel(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_iptvplayer_on_hide_panel()\n");
    ui_iptvplayer_hide_panel();
    ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PANEL);

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_stop_repeat_key(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_iptvplayer_on_stop_repeat_key()\n");
    ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_STOP_REPEAT_KEY);
    g_pIptvPlayer->last_key = MSG_INVALID;

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_mute(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() != ROOT_ID_IPTV_PLAYER)
    {
        return SUCCESS;
    }

    ui_set_mute(!ui_is_mute());

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_user_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  conn_play_info info;
  s32 hash_pos;

  OS_PRINTF("@@@ui_iptvplayer_on_user_exit()\n");
  hash_pos = db_cnpl_get_item_by_key(g_pIptvPlayer->key, &info);
  OS_PRINTF("@@@hash_pos=%d, episode_num=%d, play_time=%d\n", hash_pos, g_pIptvPlayer->episode_num, g_pIptvPlayer->play_time);
  if (hash_pos >= 0)
  {
      info.episode_num = g_pIptvPlayer->episode_num;
      info.play_time = g_pIptvPlayer->play_time;
      db_cnpl_up_item_by_hash_pos(hash_pos, &info);
  }
  else
  {
      info.episode_num = g_pIptvPlayer->episode_num;
      info.play_time = g_pIptvPlayer->play_time;
      db_cnpl_add_item_by_key(g_pIptvPlayer->key, &info);
  }

  ui_video_c_stop();
  ui_close_iptv_player();

  return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_left_right(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;
    s32 seek_time;
    osd_set_t osd_set;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (g_pIptvPlayer->b_showbar == TRUE)
        {
            if (ui_iptvplayer_is_repeat_key(msg))
            {
                g_pIptvPlayer->jump_time += SEEK_STEP_INCR;
                if (g_pIptvPlayer->jump_time > MAX_SEEK_STEP)
                {
                    g_pIptvPlayer->jump_time = MAX_SEEK_STEP;
                }
            }
            else
            {
                g_pIptvPlayer->jump_time = MIN_SEEK_STEP;
            }

            seek_time = g_pIptvPlayer->seek_time;
            if (msg == MSG_FOCUS_LEFT)
            {
                seek_time -= g_pIptvPlayer->jump_time;

                if (seek_time < 0)
                {
                    seek_time = 0;
                }
            }
            else if (msg == MSG_FOCUS_RIGHT)
            {
                seek_time += g_pIptvPlayer->jump_time;

                if (seek_time > (s32)g_pIptvPlayer->total_time)
                {
                    seek_time = g_pIptvPlayer->total_time;
                }
            }
            if (seek_time != g_pIptvPlayer->seek_time)
            {
                g_pIptvPlayer->seek_time = seek_time;
                ui_iptvplayer_set_pbar_by_time(g_pIptvPlayer->seek_time);
                ui_iptvplayer_set_seek_time();

                ctrl_paint_ctrl(g_pIptvPlayer->hProgressBar, TRUE);
                ctrl_paint_ctrl(g_pIptvPlayer->hSeekTime, TRUE);
            }

            ui_start_timer(ROOT_ID_IPTV_PLAYER, MSG_STOP_REPEAT_KEY, 250);
            g_pIptvPlayer->last_key = msg;
        }
        else
        {
            g_pIptvPlayer->seek_time = g_pIptvPlayer->play_time;
            ui_iptvplayer_set_system_time();
            ui_iptvplayer_set_pbar_by_time(g_pIptvPlayer->seek_time);
            ui_iptvplayer_set_seek_time();
            ui_iptvplayer_set_play_time();

            ui_iptvplayer_show_panel();
        }

        sys_status_get_osd_set(&osd_set);
        ui_start_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PANEL, (osd_set.timeout*1000));
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_panel_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;
    //s32 seek_delta;

    _state = ui_video_c_get_play_state();

    if ((_state == MUL_PLAY_STATE_PLAY && g_pIptvPlayer->b_showbar == FALSE)
        || (_state == MUL_PLAY_STATE_PAUSE))
    {// pause
        ui_iptvplayer_on_pause_resume(p_ctrl, msg, para1, para2);
    }
    else if (_state == MUL_PLAY_STATE_PLAY && g_pIptvPlayer->b_showbar == TRUE)
    {// seek
        ui_video_c_pause_resume();
        ui_video_c_seek(g_pIptvPlayer->seek_time);

        ui_iptvplayer_hide_panel();
        ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PANEL);
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_pause_resume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY || _state == MUL_PLAY_STATE_PAUSE)
    {
        ui_video_c_pause_resume();

        _state = ui_video_c_get_play_state();
        if (_state == MUL_PLAY_STATE_PAUSE)
        {
            bmap_set_content_by_id(g_pIptvPlayer->hSmallPlayIcon,  IM_PROGRESS_ICON_PAUSE);
            g_pIptvPlayer->seek_time = g_pIptvPlayer->play_time;
            ui_iptvplayer_set_system_time();
            ui_iptvplayer_set_pbar_by_time(g_pIptvPlayer->play_time);
            ui_iptvplayer_set_seek_time();
            ui_iptvplayer_set_play_time();
            ui_iptvplayer_show_panel();
            ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PANEL);

            bmap_set_content_by_id(g_pIptvPlayer->hLargePlayIcon,  IM_ICON_PAUSE);
            ui_iptvplayer_show_play_icon();
            ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PLAY_ICON);
        }
        else if (_state == MUL_PLAY_STATE_PLAY)
        {
            bmap_set_content_by_id(g_pIptvPlayer->hSmallPlayIcon,  IM_PROGRESS_ICON_PLAY);
            ui_iptvplayer_hide_panel();

            bmap_set_content_by_id(g_pIptvPlayer->hLargePlayIcon,  IM_ICON_PLAY);
            ctrl_paint_ctrl(g_pIptvPlayer->hLargePlayIcon, TRUE);
            ui_start_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PLAY_ICON, 1000);
        }
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_change_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if (_state == MUL_PLAY_STATE_PLAY)
    {
        if (g_pIptvPlayer->b_showbar == TRUE)
        {
            ui_iptvplayer_hide_panel();
            ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PANEL);
        }

        if (msg == MSG_DECREASE)
        {
            manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_IPTV_PLAYER, V_KEY_LEFT);
        }
        else if (msg == MSG_INCREASE)
        {
            manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_IPTV_PLAYER, V_KEY_RIGHT);
        }
    }

    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_change_aspect(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    void *p_disp_dev = NULL;
    av_set_t av_set;
    class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
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
    
    p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    if(p_disp_dev)
    {
        disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);	
        OS_PRINTF("##%s,call disp_layer_show, line[%d]##\n", __FUNCTION__, __LINE__);
    }
    else
    {
        OS_PRINTF("##%s, p_disp_dev == NULL##\n", __FUNCTION__);
    }
    
    return SUCCESS;
}

static RET_CODE ui_iptvplayer_on_change_definition(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    if (g_pIptvPlayer->total_format == 0)
    {
        return SUCCESS;
    }

    focus = list_get_focus_pos(g_pIptvPlayer->hFmtList);
    if (focus != g_pIptvPlayer->format_index)
    {
        list_set_focus_pos(g_pIptvPlayer->hFmtList, g_pIptvPlayer->format_index);
    }

    ui_iptvplayer_update_formatlist(g_pIptvPlayer->hFmtList, list_get_valid_pos(g_pIptvPlayer->hFmtList), IPTV_MAX_FORMAT_COUNT, 0);

    ctrl_process_msg(g_pIptvPlayer->hPanel, MSG_LOSTFOCUS, 0, 0);

    ctrl_set_sts(g_pIptvPlayer->hFmtListCont, OBJ_STS_SHOW);
    ctrl_set_attr(g_pIptvPlayer->hFmtList, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(g_pIptvPlayer->hFmtList, MSG_GETFOCUS, 0, 0);

    ctrl_paint_ctrl(g_pIptvPlayer->hFmtListCont, TRUE);
    return SUCCESS;
}
//------------

/************************************************
                        show time while loading media
                        
************************************************/


/*================================================================================================
                           iptv player public works function
 ================================================================================================*/

static BOOL ui_iptvplayer_init_app_data(void)
{
    if (g_pIptvPlayer)
    {
        ui_iptvplayer_release_app_data();
    }

    g_pIptvPlayer = (ui_iptv_player_app_t *)mtos_malloc(sizeof(ui_iptv_player_app_t));
    MT_ASSERT(g_pIptvPlayer != NULL);
    memset((void *)g_pIptvPlayer, 0, sizeof(ui_iptv_player_app_t));

    return TRUE;
}

static BOOL ui_iptvplayer_release_app_data(void)
{
    if (g_pIptvPlayer)
    {
        ui_iptvplayer_release_urllist();

        mtos_free(g_pIptvPlayer);
        g_pIptvPlayer = NULL;
    }

    return TRUE;
}

static void ui_iptvplayer_release_urllist(void)
{
    u16 i;

    if (g_pIptvPlayer->pp_urlList)
    {
        for (i = 0; i < g_pIptvPlayer->total_url; i++)
        {
            if (g_pIptvPlayer->pp_urlList[i])
            {
                mtos_free(g_pIptvPlayer->pp_urlList[i]);
            }
        }
        mtos_free(g_pIptvPlayer->pp_urlList);
        g_pIptvPlayer->pp_urlList = NULL;
    }
}

RET_CODE ui_open_iptv_player(u32 para1, u32 para2)
{
    control_t *p_cont;
    control_t *p_loadMediaTime_cont,*p_loadMediaTime;
    control_t *p_bps_cont_cont,*p_bps_cont, *p_bps, *p_percent;
    control_t *p_panel, *p_episodeName, *p_systemTime, *p_smallPlayIcon, *p_progressBar, *p_seekTime, *p_playTime,  *p_totalTime;
    control_t *p_largePlayIcon;
    control_t *p_format_cont, *p_formatList;

    ui_iptv_player_param *p_param;
    u16 i = 0;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_iptvplayer_init_app_data();
///////////////////////////////////////////////////////////////////
    p_param = (ui_iptv_player_param *)para1;

    g_pIptvPlayer->vdo_id = p_param->vdo_id;
    g_pIptvPlayer->res_id = p_param->res_id;
    g_pIptvPlayer->play_time = p_param->play_time;
    g_pIptvPlayer->total_episode = p_param->total_episode;
    g_pIptvPlayer->episode_num = p_param->episode_num;
    g_pIptvPlayer->b_single_page = p_param->b_single_page;
    g_pIptvPlayer->category = p_param->category;

    uni_strncpy(g_pIptvPlayer->pg_name, p_param->pg_name, MAX_PG_NAME_LEN);

    ui_iptvplayer_init_episode_info();

///////////////////////////////////////////////////////////////////
    /* create */
    p_cont = fw_create_mainwin(ROOT_ID_IPTV_PLAYER,
                            IPTV_PLAYER_CONT_X, IPTV_PLAYER_CONT_Y,
                            IPTV_PLAYER_CONT_W, IPTV_PLAYER_CONT_H,
                            ROOT_ID_INVALID, 0,
                            OBJ_ATTR_ACTIVE, 0);
    MT_ASSERT(p_cont != NULL);

    ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_keymap(p_cont, ui_iptvplayer_cont_keymap);
    ctrl_set_proc(p_cont, ui_iptvplayer_cont_proc);

    //show time while loading media
    p_loadMediaTime_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_LOADING_MEDIA_TIME_CONT,
                              IPTV_PLAYER_BPS_CONT_X, IPTV_PLAYER_BPS_CONT_Y, 
                              IPTV_PLAYER_BPS_CONT_W, IPTV_PLAYER_BPS_CONT_H, 
                              p_cont, 0);
    ctrl_set_rstyle(p_loadMediaTime_cont,RSI_PBACK, RSI_PBACK, RSI_PBACK);
    
    p_loadMediaTime = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_LOADING_MEDIA_TIME,
                              0, 0, 
                              IPTV_PLAYER_BPS_CONT_W, IPTV_PLAYER_BPS_CONT_H, 
                              p_loadMediaTime_cont, 0);
    ctrl_set_rstyle(p_loadMediaTime, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);

    text_set_align_type(p_loadMediaTime, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_loadMediaTime, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_loadMediaTime, TEXT_STRTYPE_UNICODE);

    ctrl_set_sts(p_loadMediaTime, OBJ_STS_HIDE);

    //show bps
    p_bps_cont_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_PLAYER_BPS_CONT_CONT,
                                        IPTV_PLAYER_BPS_CONT_X, IPTV_PLAYER_BPS_CONT_Y, 
                                        IPTV_PLAYER_BPS_CONT_W, IPTV_PLAYER_BPS_CONT_H, 
                                        p_cont, 0);
    ctrl_set_rstyle(p_bps_cont_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    p_bps_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_PLAYER_BPS_CONT,
                                        0, 0, 
                                        IPTV_PLAYER_BPS_CONT_W, IPTV_PLAYER_BPS_CONT_H, 
                                        p_bps_cont_cont, 0);
    ctrl_set_rstyle(p_bps_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);

    p_bps = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_BPS,
                                IPTV_PLAYER_BPS_X, IPTV_PLAYER_BPS_Y, 
                                IPTV_PLAYER_BPS_W, IPTV_PLAYER_BPS_H, 
                                p_bps_cont, 0);
    ctrl_set_rstyle(p_bps, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_bps, STL_CENTER| STL_VCENTER);
    text_set_font_style(p_bps, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_bps, TEXT_STRTYPE_UNICODE);

    p_percent = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_BUF_PERCENT,
                                      IPTV_PLAYER_BUF_PERCENT_X, IPTV_PLAYER_BUF_PERCENT_Y, 
                                      IPTV_PLAYER_BUF_PERCENT_W, IPTV_PLAYER_BUF_PERCENT_H, 
                                      p_bps_cont, 0);
    ctrl_set_rstyle(p_percent, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_percent, STL_CENTER| STL_VCENTER);
    text_set_font_style(p_percent, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_percent, TEXT_STRTYPE_UNICODE);

    ctrl_set_sts(p_bps_cont, OBJ_STS_HIDE);

    //panel part
    p_panel = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_PLAYER_PANEL,
                              IPTV_PLAYER_PANEL_X, IPTV_PLAYER_PANEL_Y,
                              IPTV_PLAYER_PANEL_W, IPTV_PLAYER_PANEL_H, 
                              p_cont, 0);
    ctrl_set_rstyle(p_panel, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    ctrl_set_keymap(p_panel, ui_iptvplayer_panel_keymap);
    ctrl_set_proc(p_panel, ui_iptvplayer_panel_proc);

    //name
    p_episodeName = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_EPISODE_NAME,
                                IPTV_PLAYER_TITLE_X, IPTV_PLAYER_TITLE_Y,
                                IPTV_PLAYER_TITLE_W, IPTV_PLAYER_TITLE_H,
                                p_panel, 0);
    text_set_align_type(p_episodeName, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_episodeName, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_episodeName, TEXT_STRTYPE_EXTSTR);
    text_set_content_by_extstr(p_episodeName, (u16 *)g_pIptvPlayer->episode_name);

    // System time
    p_systemTime = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_SYSTEM_TIME,
                                IPTV_PLAYER_SYSTEM_TIME_X, IPTV_PLAYER_SYSTEM_TIME_Y,
                                IPTV_PLAYER_SYSTEM_TIME_W, IPTV_PLAYER_SYSTEM_TIME_H,
                                p_panel, 0);
    ctrl_set_rstyle(p_systemTime, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_systemTime, STL_CENTER| STL_VCENTER);
    text_set_font_style(p_systemTime, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_systemTime, TEXT_STRTYPE_UNICODE);

    //play icon
    p_smallPlayIcon = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_PLAYER_SMALL_PLAY_ICON,
                                IPTV_PLAYER_SMALL_PLAY_ICON_X, IPTV_PLAYER_SMALL_PLAY_ICON_Y,
                                IPTV_PLAYER_SMALL_PLAY_ICON_W, IPTV_PLAYER_SMALL_PLAY_ICON_H, 
                                p_panel, 0);
    ctrl_set_rstyle(p_smallPlayIcon, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_align_type(p_smallPlayIcon, STL_CENTER | STL_VCENTER);
    bmap_set_content_by_id(p_smallPlayIcon,  IM_PROGRESS_ICON_PLAY);

    //playing progress
    p_progressBar = ctrl_create_ctrl(CTRL_PBAR, IDC_IPTV_PLAYER_PROGRESS_BAR,
                                        IPTV_PLAYER_PROGRESS_X, IPTV_PLAYER_PROGRESS_Y,
                                        IPTV_PLAYER_PROGRESS_W, IPTV_PLAYER_PROGRESS_H,
                                        p_panel, 0);
    ctrl_set_rstyle(p_progressBar, RSI_OTT_PROGRESS_BAR_BG, RSI_OTT_PROGRESS_BAR_BG, RSI_OTT_PROGRESS_BAR_BG);
    ctrl_set_mrect(p_progressBar,
                  IPTV_PLAYER_PROGRESS_MIDL, IPTV_PLAYER_PROGRESS_MIDT,
                  IPTV_PLAYER_PROGRESS_MIDR, IPTV_PLAYER_PROGRESS_MIDB);
    pbar_set_rstyle(p_progressBar, RSI_OTT_PROGRESS_BAR_MID, RSI_IGNORE, INVALID_RSTYLE_IDX);
    pbar_set_count(p_progressBar, 0, IPTV_PLAYER_PROGRESS_STEP, IPTV_PLAYER_PROGRESS_STEP);
    pbar_set_direction(p_progressBar, 1);
    //pbar_set_workmode(p_progressBar, TRUE, 0);
    pbar_set_current(p_progressBar, 0);

    // Seek time
    p_seekTime = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_SEEK_TIME,
                                IPTV_PLAYER_SEEK_TIME_X, IPTV_PLAYER_SEEK_TIME_Y,
                                IPTV_PLAYER_SEEK_TIME_W, IPTV_PLAYER_SEEK_TIME_H,
                                p_panel, 0);
    ctrl_set_rstyle(p_seekTime, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_seekTime, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_seekTime, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_seekTime, TEXT_STRTYPE_UNICODE);

    // Play time
    p_playTime = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_PLAY_TIME,
                                IPTV_PLAYER_PLAY_TIME_X, IPTV_PLAYER_PLAY_TIME_Y,
                                IPTV_PLAYER_PLAY_TIME_W, IPTV_PLAYER_PLAY_TIME_H,
                                p_panel, 0);
    ctrl_set_rstyle(p_playTime, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_playTime, STL_RIGHT| STL_VCENTER);
    text_set_font_style(p_playTime, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_playTime, TEXT_STRTYPE_UNICODE);

    // Total time
    p_totalTime = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_PLAYER_TOTAL_TIME,
                                IPTV_PLAYER_TOTAL_TIME_X, IPTV_PLAYER_TOTAL_TIME_Y,
                                IPTV_PLAYER_TOTAL_TIME_W, IPTV_PLAYER_TOTAL_TIME_H,
                                p_panel, 0);
    ctrl_set_rstyle(p_totalTime, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_totalTime, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_totalTime, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_totalTime, TEXT_STRTYPE_UNICODE);

    ctrl_set_sts(p_panel, OBJ_STS_HIDE);

    // Large play icon
    p_largePlayIcon = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_PLAYER_LARGE_PLAY_ICON,
                                IPTV_PLAYER_LARGE_PLAY_ICON_X, IPTV_PLAYER_LARGE_PLAY_ICON_Y,
                                IPTV_PLAYER_LARGE_PLAY_ICON_W, IPTV_PLAYER_LARGE_PLAY_ICON_H, 
                                p_cont, 0);
    ctrl_set_rstyle(p_largePlayIcon, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_align_type(p_largePlayIcon, STL_CENTER | STL_VCENTER);

    //prg source cont 
    p_format_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_PLAYER_FORMAT_LIST_CONT,
                                      IPTV_PLAYER_FORMAT_LIST_CONT_X, IPTV_PLAYER_FORMAT_LIST_CONT_Y,
                                      IPTV_PLAYER_FORMAT_LIST_CONT_W, IPTV_PLAYER_FORMAT_LIST_CONT_H,
                                      p_cont, 0);
    ctrl_set_rstyle(p_format_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);

    // format list
    p_formatList = ctrl_create_ctrl(CTRL_LIST, IDC_IPTV_PLAYER_FORMAT_LIST,
                              IPTV_PLAYER_FORMAT_LIST_X, IPTV_PLAYER_FORMAT_LIST_Y,
                              IPTV_PLAYER_FORMAT_LIST_W, IPTV_PLAYER_FORMAT_LIST_H, 
                              p_format_cont, 0);
    ctrl_set_rstyle(p_formatList, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_formatList, ui_iptvplayer_formatlist_keymap);
    ctrl_set_proc(p_formatList, ui_iptvplayer_formatlist_proc);

    ctrl_set_mrect(p_formatList,
                  IPTV_PLAYER_FORMAT_LIST_MIDL, IPTV_PLAYER_FORMAT_LIST_MIDT,
                  IPTV_PLAYER_FORMAT_LIST_MIDR, IPTV_PLAYER_FORMAT_LIST_MIDB);
    list_set_item_rstyle(p_formatList, &formatlist_item_rstyle);
    list_enable_select_mode(p_formatList, TRUE);
    list_set_select_mode(p_formatList, LIST_SINGLE_SELECT);
    list_set_count(p_formatList, 0, IPTV_MAX_FORMAT_COUNT);//should not be removed
    list_set_field_count(p_formatList, ARRAY_SIZE(formatlist_attr), IPTV_MAX_FORMAT_COUNT);
    list_set_columns(p_formatList, IPTV_MAX_FORMAT_COUNT, TRUE);
    list_set_update(p_formatList, ui_iptvplayer_update_formatlist, 0);

    for (i = 0; i < ARRAY_SIZE(formatlist_attr); i++)
    {
        list_set_field_attr2(p_formatList, (u8)i, formatlist_attr[i].attr,
                            formatlist_attr[i].width, formatlist_attr[i].height,
                            formatlist_attr[i].left, formatlist_attr[i].top);
        list_set_field_rect_style(p_formatList, (u8)i, formatlist_attr[i].rstyle);
        list_set_field_font_style(p_formatList, (u8)i, formatlist_attr[i].fstyle);
    }

    ctrl_set_sts(p_format_cont, OBJ_STS_HIDE);

    g_pIptvPlayer->hLoadMediaTime = p_loadMediaTime;
    g_pIptvPlayer->hBpsCont = p_bps_cont;
    g_pIptvPlayer->hBps = p_bps;
    g_pIptvPlayer->hBufPercent = p_percent;

    g_pIptvPlayer->hPanel = p_panel;
    g_pIptvPlayer->hEpisodeName = p_episodeName;
    g_pIptvPlayer->hSysTime = p_systemTime;
    g_pIptvPlayer->hSmallPlayIcon = p_smallPlayIcon;
    g_pIptvPlayer->hProgressBar = p_progressBar;
    g_pIptvPlayer->hSeekTime = p_seekTime;
    g_pIptvPlayer->hPlayTime = p_playTime;
    g_pIptvPlayer->hTotalTime = p_totalTime;

    g_pIptvPlayer->hLargePlayIcon = p_largePlayIcon;

    g_pIptvPlayer->hFmtListCont = p_format_cont;
    g_pIptvPlayer->hFmtList = p_formatList;

    ctrl_process_msg(p_panel, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

/////////////////////////////////////////////////////////////////////////////
    if (g_pIptvPlayer->pInfoUrl != NULL)
    {
        ui_iptvplayer_open_dlg();
        g_pIptvPlayer->b_showloading = TRUE;
        ui_iptv_get_play_format(g_pIptvPlayer->pInfoUrl);
    }

    return SUCCESS;
}

static RET_CODE ui_close_iptv_player(void)
{
    OS_PRINTF("@@@ui_close_iptv_player\n");
    if (g_pIptvPlayer->b_showloading)
    {
        ui_comm_dlg_close();
    }

    ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PLAY_ICON);
    ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_HIDE_PANEL);
    ui_stop_timer(ROOT_ID_IPTV_PLAYER, MSG_STOP_REPEAT_KEY);

    if (ui_is_mute())
    {
        close_mute();
    }

    manage_close_menu(ROOT_ID_IPTV_PLAYER, 0,0);
    return SUCCESS;
}


static RET_CODE ui_iptvplayer_on_destroy(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_root = fw_find_root_by_id(ROOT_ID_AUDIO_SET);//fix bug 31430
    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    p_root = fw_find_root_by_id(ROOT_ID_IPTV_DESCRIPTION);//fix bug 31430
    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }

    ui_iptvplayer_release_app_data();
    return ERR_NOFEATURE;
}


static void ui_iptvplayer_open_dlg(void)
{
    u16 *p_unistr;
    u16 uni_str1[64 + 1];
    u16 uni_str2[5 + 1];
    comm_dlg_data_t dlg_data =
    {
        0,
        DLG_FOR_SHOW | DLG_STR_MODE_EXTSTR,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    };

    uni_strcpy(uni_str1, g_pIptvPlayer->episode_name);
    str_asc2uni("\n", uni_str2);
    uni_strcat(uni_str1, uni_str2, 64);
    p_unistr = (u16 *)gui_get_string_addr(IDS_LOADING_WITH_WAIT);
    uni_strcat(uni_str1, p_unistr, 64);

    dlg_data.content = (u32)uni_str1;
    dlg_data.dlg_tmout = 0;
    ui_comm_dlg_open(&dlg_data);
}

static void ui_iptvplayer_open_cfm_dlg(u16 str_id)
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


BEGIN_KEYMAP(ui_iptvplayer_cont_keymap, NULL)
    ON_EVENT(V_KEY_MUTE, MSG_MUTE)
END_KEYMAP(ui_iptvplayer_cont_keymap, NULL)

BEGIN_MSGPROC(ui_iptvplayer_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_TIME, ui_iptvplayer_on_update_load_media_time)
    ON_COMMAND(MSG_VIDEO_EVENT_UPDATE_BPS, ui_iptvplayer_on_update_bps)
    ON_COMMAND(MSG_VIDEO_EVENT_FINISH_BUFFERING, ui_iptvplayer_on_hide_bps)
    ON_COMMAND(MSG_VIDEO_EVENT_FINISH_UPDATE_BPS, ui_iptvplayer_on_hide_bps)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, ui_iptvplayer_on_update_time)

    ON_COMMAND(MSG_HIDE_PLAY_ICON, ui_iptvplayer_on_hide_play_icon)
    ON_COMMAND(MSG_HIDE_PANEL, ui_iptvplayer_on_hide_panel)
    ON_COMMAND(MSG_STOP_REPEAT_KEY, ui_iptvplayer_on_stop_repeat_key)

    ON_COMMAND(MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE, ui_iptvplayer_on_source_format_arrive)
    ON_COMMAND(MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE,ui_iptvplayer_on_play_url_arrive)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, ui_iptvplayer_on_load_media_success)
    ON_COMMAND(MSG_VIDEO_EVENT_EOS, ui_iptvplayer_on_eos)

    ON_COMMAND(MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL,ui_iptvplayer_on_error)
    ON_COMMAND(MSG_IPTV_EVT_GET_PLAY_URL_FAIL,ui_iptvplayer_on_error)
    ON_COMMAND(MSG_VIDEO_EVENT_SET_PATH_FAIL, ui_iptvplayer_on_error)
    ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO, ui_iptvplayer_on_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, ui_iptvplayer_on_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, ui_iptvplayer_on_error)

    ON_COMMAND(MSG_MUTE, ui_iptvplayer_on_mute)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_iptvplayer_on_user_exit)
    ON_COMMAND(MSG_DESTROY, ui_iptvplayer_on_destroy)
    ON_COMMAND(MSG_EXIT_ALL, ui_iptvplayer_on_user_exit)
END_MSGPROC(ui_iptvplayer_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ui_iptvplayer_panel_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_PAUSE, MSG_PAUSE)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_VUP, MSG_INCREASE)
    ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
    ON_EVENT(V_KEY_F3, MSG_DECREASE)
    ON_EVENT(V_KEY_SAT, MSG_INCREASE)
    ON_EVENT(V_KEY_YELLOW, MSG_CHANGE_ASPECT)

    ON_EVENT(V_KEY_INFO, MSG_CHANGE_DEFINITION)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_STOP, MSG_STOP)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(ui_iptvplayer_panel_keymap, NULL)

BEGIN_MSGPROC(ui_iptvplayer_panel_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptvplayer_on_left_right)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvplayer_on_left_right)
    ON_COMMAND(MSG_SELECT, ui_iptvplayer_on_panel_select)
    ON_COMMAND(MSG_PAUSE, ui_iptvplayer_on_pause_resume)
    ON_COMMAND(MSG_DECREASE, ui_iptvplayer_on_change_volume)
    ON_COMMAND(MSG_INCREASE, ui_iptvplayer_on_change_volume)
    ON_COMMAND(MSG_CHANGE_ASPECT, ui_iptvplayer_on_change_aspect)
    ON_COMMAND(MSG_CHANGE_DEFINITION, ui_iptvplayer_on_change_definition)

    ON_COMMAND(MSG_EXIT, ui_iptvplayer_on_user_exit)
    ON_COMMAND(MSG_STOP, ui_iptvplayer_on_user_exit)
    ON_COMMAND(MSG_EXIT_ALL, ui_iptvplayer_on_user_exit)
END_MSGPROC(ui_iptvplayer_panel_proc, cont_class_proc)

BEGIN_KEYMAP(ui_iptvplayer_formatlist_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(ui_iptvplayer_formatlist_keymap, NULL)

BEGIN_MSGPROC(ui_iptvplayer_formatlist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptvplayer_on_formatlist_leftright)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvplayer_on_formatlist_leftright)
    ON_COMMAND(MSG_SELECT, ui_iptvplayer_on_formatlist_select)
    ON_COMMAND(MSG_EXIT, ui_iptvplayer_on_formatlist_hide)
    ON_COMMAND(MSG_EXIT_ALL, ui_iptvplayer_on_formatlist_hide)
END_MSGPROC(ui_iptvplayer_formatlist_proc, list_class_proc)

#endif



