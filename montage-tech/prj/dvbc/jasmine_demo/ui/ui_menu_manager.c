/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/

#include "ui_common.h"

#include "ui_mainmenu.h"
#include "ui_freq_set.h"
#include "ui_volume.h"
#include "ui_volume_usb.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_prog_bar.h"
#include "ui_nprog_bar.h"
#include "ui_num_play.h"
#include "ui_audio_set.h"
#include "ui_signal.h"
#include "ui_tp_search.h"
#include "ui_do_search.h"
#include "ui_prog_list.h"
#include "ui_language.h"
#include "ui_osd_set.h"
#include "ui_tvsys_set.h"
#include "ui_time_set.h"
#include "ui_parental_lock.h"
#include "ui_game.h"
#include "ui_game_tetris.h"
#include "ui_game_othello.h"
#include "ui_find.h"
#include "ui_info.h"
#include "ui_small_list.h"
#include "ui_notify.h"
#include "ui_zoom.h"
#include "ui_fav_set.h"
#include "ui_epg.h"
#include "ui_favorite.h"
#include "ui_timer.h"
#include "ui_epg_detail.h"
#include "ui_prog_detail.h"
#include "ui_book_list.h"
#include "ui_fav_list.h"
#include "ui_ota.h"
#include "ui_ota_search.h"
#include "ui_upgrade_by_rs232.h"
#include "ui_sort_list.h"
#include "ui_rename.h"
#include "ui_rename_v2.h"
#include "ui_sleep_timer.h"
#include "ui_ttx_help.h"
#include "ui_ts_ota_search.h"
#include "ui_keyboard_menu.h"
#include "ui_schedule.h"
#include "ui_epg_find.h"
#include "ui_upgrade_by_usb.h"
#include "ui_dump_by_usb.h"
#include "ui_jump.h"
#include "ui_hdd_info.h"
#include "ui_storage_format.h"
#include "ui_dvr_config.h"
#include "ui_record_manager.h"
#include "ui_pvr_rec_bar.h"
#include "ui_pvr_play_bar.h"
#include "ui_small_list_pvr.h"
#include "ui_picture.h"
#include "ui_pic_play_mode_set.h"
#include "ui_upgrade_by_rs232.h"
#include "ui_timeshift_bar.h"
#include "ui_sleep_hotkey.h"
#include "ui_keyboard_v2.h"
#ifdef ENABLE_MUSIC_PIC_CONFIG  
#include "ui_usb_picture.h"
#include "ui_usb_music.h"
#endif
#include "ui_text_encode.h"
#include "customer_config.h"
#ifdef ENABLE_FILE_PLAY
#include "ui_video.h"
#include "ui_video_v_full_screen.h"
#include "ui_fp_audio_set.h"
#include "ui_video_goto.h"
#endif
#include "ui_auto_search.h"
#include "ui_manual_search.h"
#include "ui_range_search.h"
#include "ui_channel_edit.h"
#include "ui_pro_info.h"
#include "ui_category.h"
#include "ui_fav_edit.h"

#ifdef USB_TEST
#include "ui_usb_test.h"
#endif
#include "ui_subt_language.h"
#ifdef NVOD_ENABLE
#include "ui_nvod.h"
#include "ui_nvod_video.h"
#include "ui_nvod_volume.h"
#endif

#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#include "ui_ca_public.h"
#include "ui_new_mail.h"
#ifdef ONLY1_CA_VER
#include "ui_finger_print.h"
#endif
#endif
#ifdef UPDATE_PG_BACKGROUND
#include "ap_table_monitor.h"
#endif
#include "ui_epg_api.h"
#include "ui_ts_record.h"
#ifdef ENABLE_ADS
#include "ui_ad_api.h"
#include "config_ads.h"
#endif
#ifdef MOSAIC_ENABLE
#include "ui_mosaic.h"
#include "ui_mosaic_play.h"
#endif
#ifndef WIN32
#include "hal_timer.h"
#endif

#ifdef ENABLE_NETWORK
#include "ui_weather.h"
#include "ui_network_config_lan.h"
#include "ui_network_config_wifi.h"
#include "ui_network_config_gprs.h"
#include "ui_network_config_3g.h"
#include "ui_ping_test.h"
#include "ui_google_map.h"
#include "ui_online_movie.h"
#include "ui_movie_search.h"
#include "ui_edit_ipaddress.h"
#include "ui_submenu_network.h"
#include "ui_submenu_online.h"
#include "ui_edit_usr_pwd.h"
#include "ui_wifi.h"
#include "ui_wifi_link_info.h"
#include "ui_edit_ip_path.h"
#include "ui_youtube_gprot.h"
#include "ui_redtube_gprot.h"
#include "ui_youporn.h"
#include "ui_edit_city.h"
#include "ui_networkplaces.h"
#include "ui_onmov_websites.h"
#include "ui_video_player_gprot.h"
#include "ui_network_playbar.h"
#include "ui_dlna.h"
#include "ui_dlna_picture.h"
#include "ui_satip.h"
#include "ui_news.h"
#include "ui_news_title.h"
#include "ui_news_info.h"

#include "ui_albums.h"
#include "ui_photos.h"
#include "ui_photo_show.h"
#include "ui_photo_info.h"

#include "ui_network_music.h"
#include "ui_network_music_search.h"

#include "ui_submenu_networkconfig.h"

#include "ui_stb_name.h"

#include "NetMediaDataProvider.h"
#include "ui_nm_gprot.h"

#if ENABLE_FLICKR
#include "ui_flickr.h"
#include "ui_flickr_fullscrn.h"
#endif

#include "ui_satip.h"
#include "ui_live_tv.h"
#include "ui_iptv_gprot.h"
#include "ui_mac_setting.h"
#endif
#ifdef BROWSER_APP
#include "ui_browser.h"
#endif
#ifdef DISPLAY_AND_VOLUME_AISAT  
#include "ui_power_on_channel.h"
#include "ui_display.h"
#include "ui_display_set.h"
#endif
#ifdef NIT_SETTING
#include "ui_nit_setting.h"
#endif
#ifdef DTMB_PROJECT
#include "ui_terrestrial_full_search.h"
#include "ui_terrestrial_manual_search.h"
#endif
#ifdef PLAAS_DEXIN_STB_ID_COMBINE
#include "ui_stbid_set.h"
#endif
#ifdef QUICK_SEARCH_SUPPORT
#include "ap_table_monitor.h"
#endif
// the information of focus change
typedef struct
{
  // the focus is changed or not
  BOOL is_changed;

  // the focus is changed from...
  u8 from_id;

  // the focus will change to...
  u8 to_id;
}focus_change_info_t;

// the information of menu management
typedef struct
{
  // the attribute of current menu
  menu_attr_t curn_menu_attr;

  // the root which will be automatic close by sys tmr
  u8 pre_autoclose_root;

  // enable or not what process the event about focus changed
  u8 enable_autoswitch;

  // the focus change info between 2 menus
  focus_change_info_t pre_focus_change;
}menu_manage_info_t;

static s32  g_timer_id = -1;
static u16  g_roll_timer_cnt = 0;
static BOOL g_ca_roll_status = FALSE;
static u32 ticks_count = 0;
static u32 rolling_ticks = 0;
static menu_manage_info_t g_menu_manage =
{
  // curn menu attr
  {ROOT_ID_BACKGROUND, PS_PLAY, OFF, SM_LOCK, NULL},
  // enable autoswtich
  ROOT_ID_INVALID, TRUE,
  // focus changed info
  {FALSE, ROOT_ID_INVALID, ROOT_ID_INVALID}
};

#define PRE_AUTOCLOSE_ROOT (g_menu_manage.pre_autoclose_root)

#define CURN_MENU_ROOT_ID (g_menu_manage.curn_menu_attr.root_id)
#define CURN_MENU_PLAY_STATE (g_menu_manage.curn_menu_attr.play_state)
#define CURN_MENU_SIGN_TYPE (g_menu_manage.curn_menu_attr.signal_msg)
#define CURN_MENU_CLOSE_TYPE (g_menu_manage.curn_menu_attr.auto_close)


static menu_attr_t all_menu_attr[MENU_MAX_CNT];
static u32 totle_menu_cnk = 0; 

#ifdef ENABLE_NETWORK
static RET_CODE ui_open_netmedia(u32 para1, u32 para2);
#endif

static  menu_attr_t public_menu_attr[] =
{
//  root_id,            play_state,   auto_close, signal_msg,   open
  {ROOT_ID_BACKGROUND, PS_PLAY, ON, SM_OFF, NULL},
  {ROOT_ID_MAINMENU, PS_PLAY, OFF, SM_OFF, open_main_menu},

  {ROOT_ID_DO_SEARCH, PS_STOP, OFF, SM_OFF, open_do_search},
#ifndef DTMB_PROJECT
  {ROOT_ID_AUTO_SEARCH, PS_STOP, OFF, SM_BAR, open_auto_search},
  {ROOT_ID_MANUAL_SEARCH, PS_STOP, OFF, SM_BAR, open_manual_search},
  {ROOT_ID_RANGE_SEARCH, PS_STOP, OFF, SM_BAR, open_range_search},
#endif

  {ROOT_ID_PROG_LIST, PS_PLAY, OFF, SM_LOCK, open_prog_list},
  {ROOT_ID_CHANNEL_EDIT, PS_STOP, OFF, SM_OFF, preopen_channel_edit},
  {ROOT_ID_SMALL_LIST, PS_PLAY, OFF, SM_BAR, open_small_list},
  {ROOT_ID_PRO_INFO, PS_PLAY, OFF, SM_BAR, open_pro_info},
  {ROOT_ID_CATEGORY, PS_PLAY, OFF, SM_LOCK, open_category},
  {ROOT_ID_FAV_EDIT, PS_PLAY, OFF, SM_OFF, preopen_fav_edit},

  {ROOT_ID_LANGUAGE, PS_STOP, OFF, SM_OFF, open_language},
#if ENABLE_TTX_SUBTITLE
  {ROOT_ID_SUBT_LANGUAGE, PS_KEEP, OFF, SM_OFF, open_subt_language},
#endif
  {ROOT_ID_TVSYS_SET, PS_STOP, OFF, SM_OFF, open_tvsys_set},
  {ROOT_ID_TIME_SET, PS_KEEP, OFF, SM_OFF, open_time_set},
  {ROOT_ID_PARENTAL_LOCK, PS_PLAY, OFF, SM_OFF, preopen_parental_lock},
  {ROOT_ID_OSD_SET, PS_STOP, OFF, SM_OFF, open_osd_set},
  {ROOT_ID_FAVORITE, PS_KEEP, OFF, SM_OFF, open_favorite},
  {ROOT_ID_FACTORY_TEST, PS_KEEP, OFF, SM_OFF, NULL},
  {ROOT_ID_INFO, PS_KEEP, OFF, SM_OFF, open_info},
  {ROOT_ID_FIND, PS_KEEP, OFF, SM_OFF, open_find},
#ifdef NVOD_ENABLE
  {ROOT_ID_NVOD, PS_PLAY, OFF, SM_OFF, open_nvod},
#endif
  {ROOT_ID_POPUP, PS_KEEP, OFF, SM_OFF, NULL},
  {ROOT_ID_PASSWORD, PS_KEEP, OFF, SM_OFF, NULL},
  {ROOT_ID_VOLUME, PS_PLAY, ON, SM_OFF, open_volume},
  {ROOT_ID_VOLUME_USB, PS_KEEP, ON, SM_OFF, open_volume_usb},
#if ENABLE_INFO_BAR_V2
  {ROOT_ID_PROG_BAR, PS_PLAY, ON, SM_BAR, open_nprog_bar},
#else
  {ROOT_ID_PROG_BAR, PS_PLAY, ON, SM_BAR, open_prog_bar},
#endif
  {ROOT_ID_AUDIO_SET, PS_PLAY, ON, SM_OFF, open_audio_set},
  {ROOT_ID_AUDIO_SET_RECORD, PS_TS, ON, SM_OFF, open_audio_set},

  {ROOT_ID_NUM_PLAY, PS_KEEP, OFF, SM_OFF, open_num_play},
  {ROOT_ID_ZOOM, PS_KEEP, OFF, SM_OFF, open_zoom},
  {ROOT_ID_FREQ_SET, PS_PLAY, OFF, SM_OFF, open_freq_set},
  {ROOT_ID_FAV_SET, PS_KEEP, OFF, SM_OFF, open_fav_set},
  {ROOT_ID_EPG, PS_PLAY, OFF, SM_OFF, open_epg},
  {ROOT_ID_EPG_DETAIL, PS_KEEP, OFF, SM_OFF, open_epg_detail},
  {ROOT_ID_PROG_DETAIL, PS_KEEP, OFF, SM_OFF, open_prog_detail},

  {ROOT_ID_TETRIS, PS_KEEP, OFF, SM_OFF, open_game_tetris},
  {ROOT_ID_OTHELLO, PS_KEEP, OFF, SM_OFF, open_game_othello},
  {ROOT_ID_TIMER, PS_KEEP, OFF, SM_OFF, open_timer},
  {ROOT_ID_BOOK_LIST, PS_PLAY, OFF, SM_OFF, open_book_list},
  {ROOT_ID_FAV_LIST, PS_PLAY, OFF, SM_LOCK, open_fav_list},
  {ROOT_ID_SORT_LIST, PS_KEEP, OFF, SM_OFF, open_sort_list},
  {ROOT_ID_RENAME, PS_KEEP, OFF, SM_OFF, open_rename},
  {ROOT_ID_RENAME_V2, PS_KEEP, OFF, SM_OFF, open_rename_v2},
  
#ifdef PLAAS_DEXIN_STB_ID_COMBINE
  {ROOT_ID_STB_ID_SELECT, PS_KEEP, OFF, SM_OFF, preopen_stbid_set},
#endif

#ifdef BROWSER_APP
  {ROOT_ID_BROWSER, PS_KEEP, OFF, SM_OFF, open_browser},
#endif
#ifdef SLEEP_TIMER
  {ROOT_ID_SLEEP_TIMER, PS_KEEP, OFF, SM_OFF, open_sleep_timer},
#endif
  {ROOT_ID_TTX_HELP, PS_KEEP, OFF, SM_OFF, open_ttx_help},
#ifdef ENABLE_USB_CONFIG
  {ROOT_ID_UPGRADE_BY_USB, PS_STOP, OFF, SM_OFF, open_upgrade_by_usb},
  {ROOT_ID_DUMP_BY_USB, PS_STOP, OFF, SM_OFF, open_dump_by_usb},
#endif
  {ROOT_ID_JUMP, PS_KEEP, OFF, SM_OFF, open_jump},
#ifdef ENABLE_UI_MEDIA
  {ROOT_ID_HDD_INFO, PS_KEEP, OFF, SM_OFF, open_hdd_info},
  {ROOT_ID_STORAGE_FORMAT, PS_KEEP, OFF, SM_OFF, open_storage_format},
#endif
#ifdef ENABLE_TIMESHIFT_CONFIG
  {ROOT_ID_DVR_CONFIG, PS_KEEP, OFF, SM_OFF, open_dvr_config},
#endif
#ifdef ENABLE_PVR_REC_CONFIG
#ifdef RECORD_MANAGER_PREVIEW
  {ROOT_ID_RECORD_MANAGER, PS_TS_PREV, OFF, SM_OFF, open_record_manager},
#else
  {ROOT_ID_RECORD_MANAGER, PS_TS, OFF, SM_OFF, open_record_manager},
#endif
#endif
#ifdef ENABLE_MUSIC_PIC_CONFIG  
  {ROOT_ID_USB_MUSIC, PS_STOP, OFF, SM_OFF, open_usb_music},
#endif
  {ROOT_ID_TEXT_ENCODE, PS_KEEP, OFF, SM_OFF, ui_text_encode_open},
#ifdef ENABLE_FILE_PLAY
#ifdef USB_FILEPLAY_PREVIEW
  {ROOT_ID_USB_FILEPLAY, PS_TS_PREV, OFF, SM_OFF, ui_video_v_open},
#else
  {ROOT_ID_USB_FILEPLAY, PS_TS, OFF, SM_OFF, ui_video_v_open},
#endif
  {ROOT_ID_FILEPLAY_BAR, PS_TS, OFF, SM_OFF, ui_video_v_fscreen_open},
  {ROOT_ID_FILEPLAY_SUBT, PS_KEEP, OFF, SM_OFF, ui_video_subt_open},
  {ROOT_ID_FP_AUDIO_SET, PS_TS, ON, SM_OFF, open_fp_audio_set},
  {ROOT_ID_VIDEO_GOTO, PS_KEEP, OFF, SM_OFF, open_video_goto},
#endif
#ifdef ENABLE_MUSIC_PIC_CONFIG  
  {ROOT_ID_USB_PICTURE, PS_STOP, OFF, SM_OFF, open_usb_picture},
#endif
#ifdef ENABLE_PVR_REC_CONFIG
  {ROOT_ID_PVR_REC_BAR, PS_PLAY, OFF, SM_OFF, open_pvr_rec_bar},
  {ROOT_ID_PVR_PLAY_BAR, PS_TS, OFF, SM_OFF, open_pvr_play_bar},
  {ROOT_ID_SMALL_LIST_PVR, PS_TS, OFF, SM_OFF, open_small_list_pvr},
#endif
#ifdef ENABLE_TIMESHIFT_CONFIG
  {ROOT_ID_TIMESHIFT_BAR, PS_KEEP, OFF, SM_LOCK, open_timeshift_bar},
#endif
#ifdef ENABLE_MUSIC_PIC_CONFIG  
  {ROOT_ID_PICTURE, PS_KEEP, OFF, SM_OFF, open_picture},
  {ROOT_ID_PIC_PLAY_MODE_SET, PS_KEEP, OFF, SM_OFF, open_pic_play_mode_set},
  {ROOT_ID_UPGRADE_BY_RS232, PS_STOP, OFF, SM_OFF, open_upgrade_by_rs232},
#endif

  {ROOT_ID_OTA, PS_STOP, OFF, SM_BAR, open_ota},
  {ROOT_ID_OTA_SEARCH, PS_STOP, OFF, SM_OFF, open_ota_search},

  {ROOT_ID_SLEEP_HOTKEY, PS_KEEP, OFF, SM_OFF, open_sleep_hotkey},

  {ROOT_ID_KEYBOARD_V2, PS_KEEP, OFF, SM_OFF, open_keyboard_v2},
#ifdef ENABLE_MUSIC_PIC_CONFIG  
  {ROOT_ID_USB_MUSIC_FULLSCREEN, PS_STOP, OFF, SM_OFF, open_usb_music_fullscreen},
#endif
  {ROOT_ID_GAME, PS_KEEP, OFF, SM_OFF, open_game},
#ifdef NVOD_ENABLE
  {ROOT_ID_NVOD_VIDEO, PS_KEEP, OFF, SM_OFF, open_nvod_video},
  {ROOT_ID_NVOD_VOLUME, PS_KEEP, ON, SM_OFF, open_nvod_volume},
#endif
#ifdef MOSAIC_ENABLE
  {ROOT_ID_MOSAIC, PS_KEEP, OFF, SM_OFF, open_mosaic},
  {ROOT_ID_MOSAIC_PLAY, PS_KEEP, OFF, SM_OFF, open_mosaic_play},
  #endif
#ifdef USB_TEST
  {ROOT_ID_USB_TEST, PS_STOP, OFF, SM_OFF, open_usb_test}
#endif
#ifdef ENABLE_PVR_REC_CONFIG
  {ROOT_ID_TS_RECORD, PS_STOP, OFF, SM_BAR, open_ts_record},
#endif
#ifdef DISPLAY_AND_VOLUME_AISAT  
  {ROOT_ID_DISPLAY, PS_KEEP, OFF, SM_BAR, open_display_setting},
  {ROOT_ID_MANUAL_SETTING, PS_KEEP, OFF, SM_BAR, open_manual_set},
  {ROOT_ID_POWER_ON_CHANNEL, PS_STOP, OFF, SM_BAR, open_channel_set},
 #endif
 #ifdef NIT_SETTING
 {ROOT_ID_NIT_SETTING, PS_STOP, OFF, SM_BAR, open_nit_set},
 #endif
#ifdef ENABLE_NETWORK
#ifndef TONGJIU
  {ROOT_ID_WEATHER_FORECAST, PS_KEEP, OFF, SM_OFF, open_weather},
  {ROOT_ID_GOOGLE_MAP, PS_KEEP, OFF, SM_OFF, open_google_map},
  {ROOT_ID_YAHOO_NEWS, PS_TS, OFF, SM_OFF, open_news},
  {ROOT_ID_NEWS_TITLE, PS_TS, OFF, SM_OFF, open_news_title},
  {ROOT_ID_NEWS_INFO, PS_TS, OFF, SM_OFF, open_news_info},
  {ROOT_ID_EDIT_CITY, PS_KEEP, OFF, SM_OFF, open_edit_city},
  {ROOT_ID_REDTUBE, PS_KEEP, OFF, SM_OFF, ui_open_redtube},
  {ROOT_ID_YOUPORN, PS_KEEP, OFF, SM_OFF, ui_open_yp_video},
  {ROOT_ID_NETWORK_MUSIC, PS_KEEP, OFF, SM_OFF, open_network_music},
  {ROOT_ID_YOUTUBE, PS_KEEP, OFF, SM_OFF, ui_open_youtube},
  {ROOT_ID_ALBUMS, PS_STOP, OFF, SM_OFF, open_albums}, 
  {ROOT_ID_PHOTOS, PS_STOP, OFF, SM_OFF, open_photos},
  {ROOT_ID_PHOTO_SHOW, PS_STOP, OFF, SM_OFF, open_photo_show},
  {ROOT_ID_PHOTO_INFO, PS_STOP, OFF, SM_OFF, open_photo_info},
  {ROOT_ID_SATIP, PS_KEEP, OFF, SM_OFF, open_satip},
  {ROOT_ID_DLNA_START, PS_KEEP, OFF, SM_OFF, open_dlna_start},
  {ROOT_ID_DLNA_PICTURE, PS_KEEP, OFF, SM_OFF, open_dlna_picture},
  {ROOT_ID_STB_NAME, PS_KEEP, OFF, SM_OFF, open_stbname},
  {ROOT_ID_NETWORK_PLACES, PS_STOP, OFF, SM_OFF, open_networkplaces},
  {ROOT_ID_IPTV_DESCRIPTION, PS_KEEP, OFF, SM_OFF, open_iptv_description},
  {ROOT_ID_IPTV_SEARCH, PS_KEEP, OFF, SM_OFF, ui_open_iptv_search},
  {ROOT_ID_IPTV, PS_STOP, OFF, SM_OFF, ui_open_iptv},
  #ifndef WIN32
  //{ROOT_ID_VDO_FAVORITE, PS_KEEP, OFF, SM_OFF, open_vdo_favorite},
  //{ROOT_ID_PLAY_HIST, PS_KEEP, OFF, SM_OFF, open_play_hist},
  {ROOT_ID_IPTV_PLAYER, PS_TS, OFF, SM_OFF, ui_open_iptv_player},
  #endif
#endif
  
  {ROOT_ID_LIVE_TV, PS_STOP, OFF, SM_OFF, open_live_tv},
  {ROOT_ID_SUBMENU_NM, PS_STOP, OFF, SM_OFF, open_submenu_online_movie},
  {ROOT_ID_NETWORK_PLAY, PS_KEEP, OFF, SM_OFF, ui_open_online_movie},
  {ROOT_ID_ONMOV_WEBSITES, PS_KEEP, OFF, SM_OFF, open_onmov_websites},
  {ROOT_ID_MOVIE_SEARCH, PS_KEEP, OFF, SM_OFF, open_movie_search},
  {ROOT_ID_ONMOV_DESCRIPTION, PS_KEEP, OFF, SM_OFF, ui_open_online_movie_description},
  {ROOT_ID_NETMEDIA, PS_KEEP, OFF, SM_OFF, ui_open_netmedia},
  {ROOT_ID_SUBNETWORK_CONFIG,PS_STOP, OFF, SM_OFF,open_submenu_networkconfig},
  {ROOT_ID_SUBMENU_NETWORK, PS_STOP, OFF, SM_OFF, open_submenu_network},

  {ROOT_ID_NETWORK_CONFIG_LAN, PS_KEEP, OFF, SM_OFF, open_network_config_lan},
  {ROOT_ID_NETWORK_CONFIG_WIFI, PS_KEEP, OFF, SM_OFF, open_network_config_wifi},
  {ROOT_ID_NETWORK_CONFIG_GPRS, PS_KEEP, OFF, SM_OFF, open_network_config_gprs},
  {ROOT_ID_NETWORK_CONFIG_3G, PS_KEEP, OFF, SM_OFF, open_network_config_3g},
  {ROOT_ID_EDIT_IP_PATH, PS_KEEP, OFF, SM_OFF, open_edit_ip_path},
//#ifndef WIN32
  {ROOT_ID_PING_TEST, PS_KEEP, OFF, SM_OFF, open_ping_test},
//#endif
  {ROOT_ID_VIDEO_PLAYER, PS_TS, OFF, SM_OFF, open_video_player},
  {ROOT_ID_EDIT_IPADDRESS, PS_KEEP, OFF, SM_OFF, open_edit_ipaddress},
  {ROOT_ID_EDIT_USR_PWD, PS_KEEP, OFF, SM_OFF, open_edit_usr_pwd},
  {ROOT_ID_WIFI, PS_KEEP, OFF, SM_OFF, ui_open_wifi},
  {ROOT_ID_WIFI_LINK_INFO, PS_KEEP, OFF, SM_OFF, open_wifi_link_info},
  {ROOT_ID_PPPOE_CONNECT, PS_KEEP, OFF, SM_OFF, open_pppoe_connect},
  {ROOT_ID_NETWORK_PLAYBAR, PS_TS, OFF, SM_OFF, open_network_playbar},

#ifdef GOBY_PLUS
  {ROOT_ID_UPGRADE_BY_NETWORK, PS_KEEP, OFF, SM_OFF, open_upgrade_by_network},
#endif
#if ENABLE_FLICKR
  //{ROOT_ID_FLICKR, PS_KEEP, OFF, SM_OFF, ui_open_flickr},
  //{ROOT_ID_FLICKR_FULLSCRN, PS_KEEP, OFF, SM_OFF, ui_open_flickr_fullscrn},
#endif
  {ROOT_ID_MAC_SETTING, PS_STOP, OFF, SM_OFF, open_mac_setting},
#endif
#ifdef DTMB_PROJECT
  {ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, PS_STOP, OFF, SM_BAR, open_terrestrial_manual_search},
  {ROOT_ID_TERRESTRIAL_FULL_SEARCH, PS_STOP, OFF, SM_OFF, open_terrestrial_full_search},
#endif  
};

#define MENU_CNT    (sizeof(public_menu_attr) / sizeof(menu_attr_t))

#define MENU_GET_ROOT_ID(idx) (all_menu_attr[idx].root_id)
#define MENU_GET_PLAY_STATE(idx) (all_menu_attr[idx].play_state)
#define MENU_GET_SIGN_TYPE(idx) (all_menu_attr[idx].signal_msg)
#define MENU_GET_CLOSE_TYPE(idx) (all_menu_attr[idx].auto_close)
#define MENU_GET_OPEN_FUNC(idx) (all_menu_attr[idx].open_func)

static const logo_attr_t all_logo_attr[] =
{
  {ROOT_ID_MAINMENU, LOGO_BLOCK_ID_M1},
};
#define LOGO_CNT    (sizeof(all_logo_attr) / sizeof(logo_attr_t))

#define LOGO_GET_ROOT_ID(idx) (all_logo_attr[idx].root_id)
#define LOGO_GET_LOGO_ID(idx) ((u8)all_logo_attr[idx].logo_id)

static preview_attr_t all_preview_attr[MENU_MAX_CNT];
static u32 totle_preview_cnk = 0; 

static const preview_attr_t public_preview_attr[] =
{
#if 0
  {
    ROOT_ID_FAV_EDIT,
    {
      (FAV_EDIT_PREV_X),
      (FAV_EDIT_PREV_Y),
      (FAV_EDIT_PREV_X + FAV_EDIT_PREV_W),
      (FAV_EDIT_PREV_Y + FAV_EDIT_PREV_H)
    },
  },

  {
    ROOT_ID_CATEGORY,
    {
      (CATEGORY_PREV_X),
      (CATEGORY_PREV_Y),
      (CATEGORY_PREV_X + CATEGORY_PREV_W),
      (CATEGORY_PREV_Y + CATEGORY_PREV_H)
    },
  },

  {
    ROOT_ID_PROG_LIST,
    {
      (PLIST_MENU_X + PLIST_PREV_X),
      (PLIST_MENU_Y + PLIST_PREV_Y),
      (PLIST_MENU_X + PLIST_PREV_X + PLIST_PREV_W),
      (PLIST_MENU_Y + PLIST_PREV_Y + PLIST_PREV_H)
    },
  },

  {
    ROOT_ID_MAINMENU,
    {
      (MAINMENU_PREV_X),
      (MAINMENU_PREV_Y),
      (MAINMENU_PREV_X + MAINMENU_PREV_W),
      (MAINMENU_PREV_Y + MAINMENU_PREV_H)
    },
  },

  {
    ROOT_ID_EPG,
    {
      (EPG_PREV_X),
      (EPG_PREV_Y),
      (EPG_PREV_X + EPG_PREV_W),
      (EPG_PREV_Y + EPG_PREV_H)
    },
  },

  {
    ROOT_ID_EPG_FIND,
    {
      (EPG_FIND_PREV_X),
      (EPG_FIND_PREV_Y),
      (EPG_FIND_PREV_X + EPG_FIND_PREV_W),
      (EPG_FIND_PREV_Y + EPG_FIND_PREV_H)
    },
  },

  {
    ROOT_ID_SCHEDULE,
    {
      (SCHEDULE_PREV_X),
      (SCHEDULE_PREV_Y),
      (SCHEDULE_PREV_X + SCHEDULE_PREV_W),
      (SCHEDULE_PREV_Y + SCHEDULE_PREV_H)
    },
  },
#endif
#ifdef ENABLE_PVR_REC_CONFIG
  {
    ROOT_ID_RECORD_MANAGER,
    {
      (COMM_WIN_SHIFT_X + REC_MANAGER_PREV_X),
      (COMM_WIN_SHIFT_Y + REC_MANAGER_PREV_Y),
      (COMM_WIN_SHIFT_X + REC_MANAGER_PREV_X + REC_MANAGER_PREV_W),
      (COMM_WIN_SHIFT_Y + REC_MANAGER_PREV_Y + REC_MANAGER_PREV_H)
    },
  },
#endif
#ifdef ENABLE_FILE_PLAY
  {
    ROOT_ID_USB_FILEPLAY,
    {
      (COMM_WIN_SHIFT_X + VIDEO_V_PREV_X),
      (COMM_WIN_SHIFT_Y + VIDEO_V_PREV_Y),
      (COMM_WIN_SHIFT_X + VIDEO_V_PREV_X + VIDEO_V_PREV_W),
      (COMM_WIN_SHIFT_Y + VIDEO_V_PREV_Y + VIDEO_V_PREV_H)
    },
  },
#endif
#if 0
  {
    ROOT_ID_NVOD,
    {
      (NVOD_PREV_X),
      (NVOD_PREV_Y),
      (NVOD_PREV_X + NVOD_PREV_W),
      (NVOD_PREV_Y + NVOD_PREV_H)
    },
  },

  {
    ROOT_ID_FAV_LIST,
    {
      (FAV_LIST_PREV_X),
      (FAV_LIST_PREV_Y),
      (FAV_LIST_PREV_X + FAV_LIST_PREV_W),
      (FAV_LIST_PREV_Y + FAV_LIST_PREV_H)
    },
  },
  #endif
  {0},
};


//#define PREVIEW_CNT    (sizeof(all_preview_attr) / sizeof(preview_attr_t))

static u8 fullscreen_root[MENU_MAX_CNT] = {0};
static u32 totle_fullscreen_cnk = 0; 

static const u8 public_fullscreen_root[] =
{
  ROOT_ID_BACKGROUND,
  ROOT_ID_VOLUME,
  ROOT_ID_PROG_BAR,
  ROOT_ID_AUDIO_SET,
  ROOT_ID_MUTE,
  ROOT_ID_PAUSE,
  ROOT_ID_SIGNAL,
  ROOT_ID_NOTIFY,
  ROOT_ID_TIMESHIFT_BAR,
  ROOT_ID_PVR_REC_BAR,
  ROOT_ID_PVR_PLAY_BAR
  //  ROOT_ID_NUM_PLAY,
  //ROOT_ID_NVOD_VOLUME,
  //ROOT_ID_NVOD_VIDEO,
};


//#define FULLSCREEN_CNT    (sizeof(fullscreen_root) / sizeof(u8))

static u8 popup_root[] =
{
  ROOT_ID_POPUP,
  ROOT_ID_PASSWORD,
  ROOT_ID_SORT_LIST,
  ROOT_ID_FAV_SET,
};

#define POPUP_CNT    (sizeof(popup_root) / sizeof(u8))

BOOL ui_is_preview_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < totle_preview_cnk; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}

BOOL ui_is_usb_prev_menu(u8 root_id)
{
  if(ROOT_ID_USB_MUSIC == root_id || ROOT_ID_USB_PICTURE== root_id)
  {
    return TRUE;
  }
  return FALSE;
}

#ifdef ENABLE_FILE_PLAY
BOOL ui_is_video_menu(u8 root_id)
{
  if(ROOT_ID_FILEPLAY_BAR == root_id)
  {
    return TRUE;
  }
  return FALSE;
}
#endif
u8 ui_get_preview_menu(void)
{
  u8 i, root_id;

  for(i = 0; i < totle_preview_cnk; i++)
  {
    root_id = all_preview_attr[i].root_id;
    if((fw_find_root_by_id(root_id) != NULL) && (fw_get_focus_id() == root_id ))
    {
      return root_id;
    }
  }
  return ROOT_ID_INVALID;
}


//Can show mute /signal window at the same time
//Other windows can be opened
BOOL ui_is_fullscreen_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < totle_fullscreen_cnk; i++)
  {
    if(fullscreen_root[i] == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}


BOOL ui_is_popup_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < POPUP_CNT; i++)
  {
    if(popup_root[i] == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}


static RET_CODE manage_tmr_create(void)
{
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  //OS_PRINTF("-====================manage create timer, %d\n", osd_set.timeout);

  return fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU,
                       osd_set.timeout * 1000, FALSE);
}


static RET_CODE manage_tmr_destroy(void)
{
  return fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU);
}


RET_CODE manage_tmr_reset(void)
{
#if 0
  manage_tmr_destroy();
  return manage_tmr_create();
#else
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU,
                  osd_set.timeout * 1000) != SUCCESS)
  {
    return manage_tmr_create();
  }


  return SUCCESS;
#endif
}

static void ui_add_to_all_fullscreen_menu(void)
{
#ifdef ENABLE_CA
  cas_manager_policy_t *handle = NULL;
  memset(fullscreen_root,0,sizeof(fullscreen_root));
  memcpy(fullscreen_root,public_fullscreen_root,sizeof(public_fullscreen_root));

  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
    totle_fullscreen_cnk = sizeof(public_fullscreen_root) / sizeof(u8) ;
    MT_ASSERT(totle_fullscreen_cnk <= MENU_MAX_CNT);
  }
  else
  {
    totle_fullscreen_cnk = sizeof(public_fullscreen_root) / sizeof(u8) 
                                        + handle->ca_fullscreen_root_number;
    MT_ASSERT(totle_fullscreen_cnk <= MENU_MAX_CNT);
    memcpy((u8 *)fullscreen_root+sizeof(public_fullscreen_root),
                handle->ca_fullscreen_root,sizeof(u8) * handle->ca_fullscreen_root_number);
  }
#else
  memset(fullscreen_root,0,sizeof(fullscreen_root));
  memcpy(fullscreen_root,public_fullscreen_root,sizeof(public_fullscreen_root));
  totle_fullscreen_cnk = sizeof(public_fullscreen_root) / sizeof(u8) ;
  MT_ASSERT(totle_fullscreen_cnk <= MENU_MAX_CNT);
#endif
}

static void ui_add_to_all_menu(void)
{
#ifdef ENABLE_CA
  cas_manager_policy_t *handle = NULL;

  memset(all_menu_attr,0,sizeof(all_menu_attr));
  memcpy(all_menu_attr,public_menu_attr,sizeof(public_menu_attr));

  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
    totle_menu_cnk = sizeof(public_menu_attr) / sizeof(menu_attr_t) ;
    MT_ASSERT(totle_menu_cnk <= MENU_MAX_CNT);
  }
  else
  {
    totle_menu_cnk = sizeof(public_menu_attr) / sizeof(menu_attr_t) + handle->attr_number;
    MT_ASSERT(totle_menu_cnk <= MENU_MAX_CNT);
    memcpy((u8 *)all_menu_attr+sizeof(public_menu_attr),
    handle->ca_menu_attr,sizeof(menu_attr_t) * handle->attr_number);
  }
#else
  memset(all_menu_attr,0,sizeof(all_menu_attr));
  memcpy(all_menu_attr,public_menu_attr,sizeof(public_menu_attr));
  totle_menu_cnk = sizeof(public_menu_attr) / sizeof(menu_attr_t) ;
  MT_ASSERT(totle_menu_cnk <= MENU_MAX_CNT);
#endif
}

static void ui_add_all_preview(void)
{
#ifdef ENABLE_CA
  cas_manager_policy_t *handle = NULL;

  memset(all_preview_attr,0,sizeof(all_preview_attr));
  memcpy(all_preview_attr,public_preview_attr,sizeof(public_preview_attr));

  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
    totle_preview_cnk = sizeof(public_preview_attr) / sizeof(preview_attr_t) ;
    MT_ASSERT(totle_preview_cnk <= MENU_MAX_CNT);
  }
  else
  {
    totle_preview_cnk = sizeof(public_preview_attr) / sizeof(preview_attr_t) + handle->preview_attr_number;
    MT_ASSERT(totle_preview_cnk <= MENU_MAX_CNT);
    memcpy((u8 *)all_preview_attr+sizeof(public_preview_attr),
    handle->ca_preview_attr,sizeof(preview_attr_t) * handle->preview_attr_number);
  }

#else 
  memset(all_preview_attr,0,sizeof(all_preview_attr));
  memcpy(all_preview_attr,public_preview_attr,sizeof(public_preview_attr));
  totle_preview_cnk = sizeof(public_preview_attr) / sizeof(preview_attr_t) ;
  MT_ASSERT(totle_preview_cnk <= MENU_MAX_CNT);
#endif
}
static u8 manage_find_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < totle_menu_cnk; i++)
  {
    if(MENU_GET_ROOT_ID(i) == root_id)
    {
      return i;
    }
  }
  return INVALID_IDX;
}


static u8 manage_find_logo(u8 root_id)
{
  u8 i;

  for(i = 0; i < LOGO_CNT; i++)
  {
    if(LOGO_GET_ROOT_ID(i) == root_id)
    {
      return i;
    }
  }
  return INVALID_IDX;
}

u8 manage_find_preview(u8 root_id, rect_t *orc)
{
  u8 i;
  //s16 x = 0, y = 0;
  disp_sys_t std;

  empty_rect(orc);
  avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
  std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  OS_PRINTF("UI: swithced to PREV........std=%d\n", std);

  for(i = 0; i < totle_preview_cnk; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      copy_rect(orc, &all_preview_attr[i].position);
      /* convert coordinate to screen */
     /* switch(std)
      {
        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
        case VID_SYS_PAL_M:
          x = SCREEN_POS_NTSC_L, y = SCREEN_POS_NTSC_T;
          break;
        case VID_SYS_PAL_N:
        case VID_SYS_PAL:
        case VID_SYS_PAL_NC:
          x = SCREEN_POS_PAL_L, y = SCREEN_POS_PAL_T;
          break;
        default:

          break;
      }
      offset_rect(orc, x, y);*/

   #ifndef WIN32
      switch(std)
      {
        case VID_SYS_1080I:
        case VID_SYS_1080I_50HZ:
        case VID_SYS_1080P:
        case VID_SYS_1080P_24HZ:
        case VID_SYS_1080P_25HZ:
        case VID_SYS_1080P_30HZ:
        case VID_SYS_1080P_50HZ:
          //to scale video
          orc->left = (s16)(orc->left * 1920 / 1280);
          orc->right = (s16)(orc->right * 1920 / 1280);
          orc->top = (s16)(orc->top * 1080 / 720);
          orc->bottom = (s16)(orc->bottom * 1080 / 720);
          break;

        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
        case VID_SYS_PAL_M:
        case VID_SYS_480P:
          orc->left = (s16)(orc->left * 720 / 1280);
          orc->right = (s16)(orc->right * 720 / 1280);
          orc->top = (s16)(orc->top * 480 / 720);
          orc->bottom = (s16)(orc->bottom * 480 / 720);
          break;

        case VID_SYS_PAL_N:
        case VID_SYS_PAL:
        case VID_SYS_PAL_NC:
        case VID_SYS_576P_50HZ:
          orc->left = (s16)(orc->left * 720 / 1280);
          orc->right = (s16)(orc->right * 720 / 1280);
          orc->top = (s16)(orc->top * 576 / 720);
          orc->bottom = (s16)(orc->bottom * 576 / 720);
          break;

        default:
          break;
      }
  #endif
      return i;
    }
  }
  return INVALID_IDX;
}


BOOL manage_get_preview_rect(u8 root_id,
                             u16 *left,
                             u16 *top,
                             u16 *width,
                             u16 *height)
{
  u8 i;

  for(i = 0; i < totle_preview_cnk; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      *left = (u16)all_preview_attr[i].position.left;
      *top = (u16)all_preview_attr[i].position.top;
      *width = (u16)RECTW(all_preview_attr[i].position);
      *height = (u16)RECTH(all_preview_attr[i].position);
      return TRUE;
    }
  }
  return FALSE;
}


//we should make all background feature work well after register manager
void manage_init(void)
{
  manage_tmr_create();
}


//special : for change the timing of show logo and show OSD
void manage_logo2preview(u8 root_id)
{
  u8 idx;
  rect_t orc;

  idx = manage_find_logo(root_id);
  if(idx != INVALID_IDX)
  {
    ui_show_logo(LOGO_GET_LOGO_ID(idx));
  }
  idx = manage_find_preview(root_id, &orc);
  if(idx != INVALID_IDX)
  {
    avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
    avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                        (u16)orc.left, (u16)orc.top, (u16)orc.right, (u16)orc.bottom);
    //avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                         //orc.left, orc.top, orc.right, orc.bottom);
  }
//  av_video_output_enable();
}

BOOL ui_is_autoclose_menu(u8 root_id)
{ 
  u8 idx, auto_close;
  idx = manage_find_menu(root_id);
  if(idx == INVALID_IDX)
  {
    return FALSE;
  }
  auto_close = MENU_GET_CLOSE_TYPE(idx);
  if(auto_close == ON)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static u8 _get_menu_type(u8 root_id)
{
  if(ui_is_fullscreen_menu(root_id))
  {
    return MENU_TYPE_FULLSCREEN;
  }
  else if(ui_is_preview_menu(root_id))
  {
    return MENU_TYPE_PREVIEW;
  }
  else if(ui_is_popup_menu(root_id))
  {
    return MENU_TYPE_POPUP;
  }
   else if(ui_is_usb_prev_menu(root_id))
  {
    return MENU_TYPE_USB_PREV;
  }
#ifdef ENABLE_FILE_PLAY
  else if(ui_is_video_menu(root_id))
  {
    return MENU_TYPE_VIDEO;
  }
#endif
  else
  {
    return MENU_TYPE_NORMAL;
  }
}


void manage_enable_autoswitch(BOOL is_enable)
{
  g_menu_manage.enable_autoswitch = (u8)is_enable;
}

static void manage_leave_preview(u8 to_id)
{
  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_SWITCH_PREVIEW);
  avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
}

static void _switch_playstatus(u8 from_id, u8 to_id)
{
  u8 idx, new_ps;
  rect_t orc;

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    return;
  }
  else
  {
    new_ps = MENU_GET_PLAY_STATE(idx);
  }
  if(new_ps == PS_KEEP)
  {
    return;
  }

  if(new_ps == CURN_MENU_PLAY_STATE)
  {
    if((to_id == ROOT_ID_PROG_LIST)
       || (to_id == ROOT_ID_MAINMENU)
       || (to_id == ROOT_ID_EPG)
       || (to_id == ROOT_ID_CATEGORY)
       || (to_id == ROOT_ID_FAV_EDIT)
       || (to_id == ROOT_ID_FAV_LIST))
    {
      return;
    }
    else
  	{
        if(new_ps == PS_LOGO)
        {
          ui_set_front_panel_by_str("----");
        }
        return;
  	}
  }

  //we should auto switch logo
  switch(new_ps)
  {
    case PS_TS_PREV:
      OS_PRINTF("UI: switch to TS PREV..........\n");
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_TS:
          idx = manage_find_preview(to_id, &orc);
          if(idx != INVALID_IDX)
          {
#if 0
            OS_PRINTF("UI: swithced to TS PREV 11........\n");
            avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                orc.left, orc.top, orc.right, orc.bottom);
            //avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                 //orc.left, orc.top, orc.right, orc.bottom);
#endif
            OS_PRINTF("UI: swithced to TS PREV 11........\n");
          }
          fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SWITCH_PREVIEW, 100, FALSE);
          break;

        default:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          idx = manage_find_preview(to_id, &orc);
          if(idx != INVALID_IDX)
          {
#if 0
            OS_PRINTF("UI: swithced to TS PREV........\n");
            avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                orc.left, orc.top, orc.right, orc.bottom);
            //avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                 //orc.left, orc.top, orc.right, orc.bottom);
#endif
            fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SWITCH_PREVIEW, 100, FALSE);
            OS_PRINTF("UI: swithced to TS PREV........\n");
          }
          break;
      }
      break;

    case PS_PREV:
      OS_PRINTF("UI: switch to PREV..........\n");
      idx = manage_find_logo(to_id);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_LOGO:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          idx = manage_find_preview(to_id, &orc);
          if(idx != INVALID_IDX)
          {
#if 0
            OS_PRINTF("UI: swithced to PREV........PS_LOGO\n");
            avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                orc.left, orc.top, orc.right, orc.bottom);
            //avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                 //orc.left, orc.top, orc.right, orc.bottom);

            OS_PRINTF("UI: swithced to PREV........\n");
#else
            //fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, FALSE, MSG_SWITCH_PREVIEW, to_id, 0);
            fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SWITCH_PREVIEW, 100, FALSE);
#endif
          }
          break;
        default:
          idx = manage_find_preview(to_id, &orc);
          OS_PRINTF("avc-------x = %d\n-------y = %d\n-------w = %d\n-------h = %d\n",
                    orc.left, orc.top, orc.right, orc.bottom);
          if(idx != INVALID_IDX)
          {
            if(to_id != ROOT_ID_FAV_EDIT)
            ui_play_curn_pg();
#if 0
            OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
            avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                orc.left, orc.top, orc.right, orc.bottom);
            OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
            //avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                 //orc.left, orc.top, orc.right, orc.bottom);
            OS_PRINTF("UI: swithced to PREV........\n");
#else
            //fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, FALSE, MSG_SWITCH_PREVIEW, to_id, 0);
            fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SWITCH_PREVIEW, 100, FALSE);
#endif
          }
      }
      break;
    case PS_PLAY:
      OS_PRINTF("UI: switch to PLAY..........\n");
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_PREV:
        case PS_TS_PREV:
          //ui_enable_video_display(FALSE);
          manage_leave_preview(to_id);
          ui_play_curn_pg();
	   //mtos_task_delay_ms(100);
	   //ui_enable_video_display(TRUE);
          break;
        case PS_LOGO:
          if((to_id == ROOT_ID_PROG_LIST)
            && (sys_status_get_curn_prog_mode() == CURN_MODE_RADIO))
          {
            //ui_show_logo(LOGO_BLOCK_ID_M1);
            ui_show_logo(LOGO_BLOCK_ID_M1); //LOGO_BLOCK_ID_M2
          }
          else
          {
            ui_play_curn_pg();
          }

          break;
        default:
          if(to_id != ROOT_ID_FAV_EDIT)
          {
            ui_play_curn_pg();
          }
          break;
      }
      break;
    case PS_TS:
      OS_PRINTF("UI: switch to TS PLAY..........\n");
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_TS_PREV:
        case PS_PREV:
          manage_leave_preview(to_id);
          break;
        case PS_PLAY:
	   ui_stop_play(STOP_PLAY_BLACK, TRUE);
	   break;
        default:
          break;
      }
      break;
    case PS_LOGO:
      ui_set_front_panel_by_str("----");
      OS_PRINTF("UI: switch to LOGO..........\n");
      idx = manage_find_logo(to_id);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_PREV:

          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          manage_leave_preview(to_id);

          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
          break;
        case PS_PLAY:

          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
          break;
        default:
          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
      }
      break;
    case PS_STOP:
      ui_set_front_panel_by_str("----");
      /* just stop playing */
      OS_PRINTF("UI: switch to STOP..........\n"); 

      ui_stop_play(STOP_PLAY_BLACK, TRUE);
      if(PS_PREV == CURN_MENU_PLAY_STATE)
      {
        manage_leave_preview(to_id);
      }
      /* stop monitor */
//      dvbs_stop_monitor();
      break;

    default:
      /* do nothing */;
  }

  CURN_MENU_PLAY_STATE = new_ps;
}


static void _switch_signalcheck(u8 from_id, u8 to_id)
{
  u8 idx, new_sm;

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    new_sm = SM_OFF;
  }
  else
  {
    new_sm = MENU_GET_SIGN_TYPE(idx);
  }

  CURN_MENU_SIGN_TYPE = new_sm;
}

//lint -e438 -e550 -e830
static void _switch_autoopen(u8 from_id, u8 to_id)
{
  u8 old_status, new_status;
  BOOL is_chk_mute = FALSE;
  BOOL is_chk_pause = FALSE;
  BOOL is_chk_notify = FALSE;
  BOOL is_chk_signal = FALSE;
  BOOL is_chk_email = FALSE;
  BOOL b_time_shift = FALSE;
  old_status = _get_menu_type(from_id);
  new_status = _get_menu_type(to_id);

  if(new_status != old_status
    && old_status != MENU_TYPE_POPUP)
  {
    switch(new_status)
    {
      case MENU_TYPE_FULLSCREEN:
        is_chk_mute = is_chk_pause = is_chk_notify = is_chk_signal = is_chk_email = TRUE;
        break;
      case MENU_TYPE_PREVIEW:
#ifdef ENABLE_FILE_PLAY
        if(ROOT_ID_USB_FILEPLAY == to_id)
        {
          is_chk_mute = TRUE;
        }
        else
#endif
        {
          is_chk_mute = is_chk_pause = is_chk_signal = TRUE;
        }
        break;
#ifdef ENABLE_FILE_PLAY
      case MENU_TYPE_VIDEO:
            is_chk_mute = TRUE;
            break;
#endif
      default:
        ;
    }
  }
  if((to_id == ROOT_ID_PROG_LIST)
     //|| (to_id == ROOT_ID_MAINMENU)
     //|| (to_id == ROOT_ID_EPG)
     //|| (to_id == ROOT_ID_CATEGORY)
     || (to_id == ROOT_ID_FAV_EDIT)
     //|| (to_id == ROOT_ID_NVOD)
     || (to_id == ROOT_ID_NVOD_VIDEO)
     || (to_id == ROOT_ID_FAV_LIST))
  {
     is_chk_pause = is_chk_signal = TRUE;
  }

  if((to_id == ROOT_ID_PROG_LIST)
     || (to_id == ROOT_ID_MAINMENU)
     || (to_id == ROOT_ID_CHANNEL_EDIT)
     || (to_id == ROOT_ID_SMALL_LIST)
     || (to_id == ROOT_ID_PRO_INFO)
     || (to_id == ROOT_ID_CATEGORY)
     || (to_id == ROOT_ID_FAV_EDIT)
     || (to_id == ROOT_ID_PARENTAL_LOCK)
     || (to_id == ROOT_ID_NVOD)
     || (to_id == ROOT_ID_FREQ_SET)
     || (to_id == ROOT_ID_EPG)
     || (to_id == ROOT_ID_BOOK_LIST)
     || (to_id == ROOT_ID_NVOD_VIDEO)
     || (to_id == ROOT_ID_FAV_LIST)
     || (to_id == ROOT_ID_TIME_SET)
     || (to_id == ROOT_ID_INFO)
     || (to_id == ROOT_ID_EPG_DETAIL)
     || (to_id == ROOT_ID_JUMP)
     || (to_id == ROOT_ID_HDD_INFO)
     || (to_id == ROOT_ID_STORAGE_FORMAT)
     || (to_id == ROOT_ID_DVR_CONFIG)
     || (to_id == ROOT_ID_GAME)
     || (to_id == ROOT_ID_USB_MUSIC_FULLSCREEN)
#ifdef ENABLE_CA
     || (to_id == ROOT_ID_CONDITIONAL_ACCEPT)
#endif
    )
  {
     is_chk_mute = TRUE;
  }
#ifdef SMSX_CA
extern  void close_ca_finger_menu(void);

if((from_id == ROOT_ID_BACKGROUND&&to_id == ROOT_ID_PROG_LIST) 
	||(from_id == ROOT_ID_BACKGROUND && to_id == ROOT_ID_MAINMENU))
if( ui_get_osd_is_show())
{
	OS_PRINTF("set osd_show_beat 0 \n");
	set_time_head_bit(0);	
}
if(to_id == ROOT_ID_MAINMENU || to_id == ROOT_ID_SMALL_LIST)
{
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT))
		{
			OS_PRINTF("CLOSE FINGER PRINT \n");
			 close_ca_finger_menu();

		}
}
#endif

  if(ui_is_mute()
    && is_chk_mute)
  {
    open_mute(0, 0);
  }

  if(ui_is_pause()
    && is_chk_pause)
  {
    open_pause(0, 0);
  }

  if(ui_is_notify()
    && is_chk_notify)
  {
    open_notify(NOTIFY_AUTOCLOSE_3000MS, 0);
  }

  if(is_chk_signal)
  {
    update_signal();
  }

#ifdef ENABLE_CA
  if(is_chk_email)
  {
    cas_manage_process_menu(CA_OPEN_EMAIL, NULL, 0, 0);
    cas_manage_process_menu(CA_OPEN_CURTAIN, NULL, 0, 0);
  }
#endif

  if((to_id == ROOT_ID_BACKGROUND)
    && (old_status != MENU_TYPE_FULLSCREEN)
    && (fw_get_focus_id() == ROOT_ID_BACKGROUND))
  {
    sys_status_get_status(BS_DVR_ON, &b_time_shift);
    if(!b_time_shift)
    {
      manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
    }
  }
      #ifdef BOYUAN_AD
  extern  void by_start_search_logo(void);
  extern void by_stop_search_logo(void);
  	if(new_status == MENU_TYPE_FULLSCREEN)
  	{
  		OS_PRINTF("by_start_search_logo -----\n ");
		by_start_search_logo();

	}
	else
	{
		OS_PRINTF("  by_stop_search_logo ----\n");
		by_stop_search_logo();
	}
  #endif

}
//lint +e438 +e550 +e830

static void _switch_autoclose(u8 from_id, u8 to_id)
{
  u8 idx, new_auto;
  u8 old_status, new_status;

  old_status = _get_menu_type(from_id);
  new_status = _get_menu_type(to_id);

  if((old_status == MENU_TYPE_FULLSCREEN
     || old_status == MENU_TYPE_PREVIEW
     || old_status == MENU_TYPE_USB_PREV)
    && (old_status != new_status) /*preview or normal*/
    && new_status != MENU_TYPE_POPUP)
  {
    if(ui_is_mute())
    {
      close_mute();
    }
    if(ui_is_pause())
    {
      close_pause();
    }
    if(ui_is_notify())
    {
      close_notify();
    }

#ifdef ENABLE_CA
    cas_manage_process_menu(CA_CLOSE_EMAIL, NULL, 0, 0);
#endif

    /* try to close */
    close_signal();
  }

#ifdef ENABLE_CA
  //cas_manage_finger_repaint();
#endif

  if((to_id == ROOT_ID_PROG_LIST)
     || (to_id == ROOT_ID_MAINMENU)
     || (to_id == ROOT_ID_EPG)
     || (to_id == ROOT_ID_CATEGORY)
     || (to_id == ROOT_ID_FAV_EDIT)
     || (to_id == ROOT_ID_NVOD)
     || (to_id == ROOT_ID_NVOD_VIDEO)
     || (to_id == ROOT_ID_FAV_LIST))
  {
    if(ui_is_mute())
    {
      close_mute();
    }
    if(ui_is_pause())
    {
      close_pause();
    }
    if(ui_is_notify())
    {
      close_notify();
    }

#ifdef ENABLE_CA
    cas_manage_process_menu(CA_CLOSE_EMAIL, NULL, 0, 0);
#endif

    /* try to close */
    close_signal();
  }
#ifdef SMSX_CA
OS_PRINTF("AUTO CLOSE from_id = %d, to_id = %d \n", from_id, to_id);
	if(from_id == ROOT_ID_PROG_BAR && to_id == ROOT_ID_BACKGROUND)
	{
		if( ui_get_osd_is_show())
		{
			OS_PRINTF("SET show head beat 1\n");
			set_time_head_bit(1);
		}
	}
#endif

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    new_auto = OFF;
  }
  else
  {
    new_auto = MENU_GET_CLOSE_TYPE(idx);
  }

  UI_PRINTF("switch_autoclose: from %d to %d \n", from_id, to_id);

  if(CURN_MENU_CLOSE_TYPE == OFF)
  {
    if(new_auto == OFF)
    {
      PRE_AUTOCLOSE_ROOT = INVALID_IDX;
      return;
    }
    else
    {
      PRE_AUTOCLOSE_ROOT = to_id;
      UI_PRINTF("switch_autoclose: create tmr \n");
      manage_tmr_create();
    }
  }
  else
  {
    if(new_auto == OFF)
    {
      if(to_id == ROOT_ID_PASSWORD)
      {
        PRE_AUTOCLOSE_ROOT = ROOT_ID_PROG_BAR;
      }
      else
      {
        PRE_AUTOCLOSE_ROOT = INVALID_IDX;
        UI_PRINTF("switch_autoclose: destroy tmr \n");
        manage_tmr_destroy();
      }
    }
    else
    {
      PRE_AUTOCLOSE_ROOT = to_id;
      UI_PRINTF("switch_autoclose: reset tmr \n");
      manage_tmr_reset();
    }
  }

  CURN_MENU_CLOSE_TYPE = new_auto;
}


void manage_auto_switch(void)
{
  focus_change_info_t *p_info = &g_menu_manage.pre_focus_change;

  if(p_info->is_changed)
  {
    p_info->is_changed = FALSE;
/* move to ui_menu_manger_proc
    _switch_playstatus(p_info->from_id, p_info->to_id);
    _switch_autoclose(p_info->from_id, p_info->to_id);*/
    _switch_autoopen(p_info->from_id, p_info->to_id);
    _switch_signalcheck(p_info->from_id, p_info->to_id);
  }
}


RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2)
{
  open_menu_t open_func;
  u8 idx;
  RET_CODE ret;

  idx = manage_find_menu(root_id);
  if(idx == INVALID_IDX)
  {
    return ERR_NOFEATURE;
  }

  open_func = MENU_GET_OPEN_FUNC(idx);
  if((open_func == NULL))
  {
    return ERR_NOFEATURE;
  }

  ret = (*open_func)(para1, para2);
  if(ret == SUCCESS)
  {
    manage_auto_switch();
  }
  return ret;
}


RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2)
{
  if(!fw_destroy_mainwin_by_id(root_id))
  {
    return ERR_FAILURE;
  }
  manage_auto_switch();
  return SUCCESS;
}


RET_CODE manage_back_to_menu(u8 root_id, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  if(fw_find_root_by_id(root_id) != NULL)
  {
    while(fw_get_focus_id() != root_id)
    {
      if(!fw_destroy_mainwin_by_id(fw_get_focus_id()))
      {
        ret = ERR_FAILURE;
        break;
      }
    }

    manage_enable_autoswitch(FALSE);
    fw_destroy_mainwin_by_id(root_id);
    manage_enable_autoswitch(TRUE);
    manage_open_menu(root_id, para1, para2);
    //ctrl_paint_ctrl(fw_get_focus(), TRUE);

    ret = SUCCESS;
  }
  else
  {
    manage_open_menu(root_id, para1, para2);
    ret = SUCCESS;
  }

  return ret;
}


RET_CODE manage_notify_root(u8 root_id, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root;

  p_root = fw_find_root_by_id(root_id);
  if(p_root != NULL)
  {
    return ctrl_process_msg(p_root, msg, para1, para2);
  }

  return ERR_NOFEATURE;
}


void manage_autoclose(void)
{
  u8 root_id;
  control_t *p_obj;

  root_id = PRE_AUTOCLOSE_ROOT;

//  if(PRE_AUTOCLOSE_ROOT != CURN_MENU_ROOT_ID)
//  {
//    return;
//  }

  p_obj = fw_find_root_by_id(root_id);
  if(NULL == p_obj)
  {
    return;
  }

  /* add patch to keep prog bar on radio mode. */
  if(root_id == ROOT_ID_PROG_BAR
    && sys_status_get_curn_prog_mode() == CURN_MODE_RADIO)
  {
    return;
  }

  if(ctrl_process_msg(p_obj, MSG_EXIT, 0, 0) != SUCCESS)
  {
    fw_destroy_mainwin_by_root(p_obj);
  }
}


menu_attr_t *manage_get_curn_menu_attr(void)
{
  return &g_menu_manage.curn_menu_attr;
}


static void _pre_enter_preview(u8 root_id, u32 para1, u32 para2)
{
  u8 curn_mode = sys_status_get_curn_prog_mode();

  switch(root_id)
  {
    case ROOT_ID_PROG_LIST:
      if(((curn_mode == CURN_MODE_TV)
         && (para1 == 1))                                  //enter tv list on radio mode.
        || ((curn_mode == CURN_MODE_RADIO)
           && (para1 == 0)))                               //enter radio list on tv mode.
      {
        ui_enable_video_display(FALSE);
      }
      break;
    default:
      /* do nothing */;
  }
}


static void _before_open_menu(u8 old_root, u8 new_root, u32 para1, u32 para2)
{
  u8 new_status;

  new_status = _get_menu_type(new_root);

  if(new_status == MENU_TYPE_PREVIEW)
  {
    _pre_enter_preview(new_root, para1, para2);
  }

  switch(new_root)
  {
    case ROOT_ID_USB_FILEPLAY:
    case ROOT_ID_USB_PICTURE:
    #ifdef MIN_AV_64M
      ui_epg_stop();
      ui_epg_release();
    #endif
      ui_watermark_release();
    #ifdef ENABLE_ADS
      pic_adv_stop();
      ui_adv_pic_release();
      #endif
      break;
    case ROOT_ID_RECORD_MANAGER:
    case ROOT_ID_PVR_REC_BAR:
    case ROOT_ID_TIMESHIFT_BAR:
    case ROOT_ID_TS_RECORD:
    case ROOT_ID_USB_MUSIC:
      //release epg when enter above apps for memory share.
    #ifdef MIN_AV_64M
      ui_epg_stop();
      ui_epg_release();
    #endif
      break;

    default:
      break;
  }
}


static void _before_close_menu(u8 root_id)
{
  /*
     u8 status = _get_menu_type(root_id);

     if(status == MENU_TYPE_PREVIEW)
     {
     close_signal();
     }
    */
  switch(root_id)
  {
#ifdef ENABLE_PVR_REC_CONFIG
    case ROOT_ID_RECORD_MANAGER:
      ui_rec_manager_destroy();
#endif
      //no need break, fall through
    #ifdef ENABLE_MUSIC_PIC_CONFIG
    case ROOT_ID_USB_MUSIC:
      ui_usb_music_exit();
    #endif
      //  no break;
    case ROOT_ID_USB_FILEPLAY:
    case ROOT_ID_PVR_REC_BAR:
    case ROOT_ID_TIMESHIFT_BAR:
    case ROOT_ID_TS_RECORD:
    case ROOT_ID_USB_PICTURE:
    #ifdef MIN_AV_64M
      //restart epg
      ui_epg_init();
      
      ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
    #endif
      break;
    default:
      break;
  }
}


void ui_close_all_mennus(void)
{
  u8 to = ROOT_ID_INVALID, from = fw_get_focus_id();

#ifdef ONLY1_CA_VER
  BOOL need_repaint = FALSE;
  if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
  {
    need_repaint = TRUE;
  }
#endif

  manage_enable_autoswitch(FALSE);
  to = fw_destroy_all_mainwin(FALSE);

  manage_enable_autoswitch(TRUE);

  ui_menu_manage(WINDOW_MANAGE_FOCUS_CHANGED, from, to);

  //fw_paint_all_mainwin();
 fw_paint_all_mainwin();

#ifdef ENABLE_ADS
  pic_adv_stop();
#endif
  manage_auto_switch();
  
  //#ifdef DESAI_56_CA
#ifdef ENABLE_CA
#ifndef ONLY1_CA_VER
cas_manager_reroll_after_close_all_menu();
cas_manage_finger_repaint();
#else
if(need_repaint)
{
  ui_finger_print_repaint();//because finger menu is closed
}
#endif
#endif
  //#endif

}

#ifdef ENABLE_NETWORK
static RET_CODE ui_open_netmedia(u32 para1, u32 para2)
{
    NETMEDIA_DP_TYPE type;
    RET_CODE ret = ERR_FAILURE;
    type = (NETMEDIA_DP_TYPE)para1;
    switch (type)
    {
        case NETMEDIA_DP_VIMEO:
            ret = ui_open_nm_simple(para1, para2);
            break;
        case NETMEDIA_DP_ONLINE_MOVIES:
            break;
        case NETMEDIA_DP_DAILYMOTION:
            ret = ui_open_nm_simple(para1, para2);
            break;
        case NETMEDIA_DP_ALJAZEER:
            ret = ui_open_nm_simple(para1, para2);
            break;
        default:
            break;
    }
    return ret;
}
#endif
extern void usb_monitor_attach(void);
static RET_CODE on_initialize(u32 event, u32 para1, u32 para2)
{
  // start app framework
  OS_PRINTF("menu manager inital\n");
  
  ui_add_to_all_menu();
  ui_add_to_all_fullscreen_menu();
  ui_add_all_preview();
  
  if(g_customer.cus_config_other_menu != NULL)
  {
    g_customer.cus_config_other_menu();
  } 
  
#ifdef ENABLE_CA
  if(CAS_ID  != CONFIG_CAS_ID_DS)
  {
    ui_init_ca();
  }
#endif
  ap_frm_set_ui_msgq_id((s32)para1);

  //usb_monitor_attach();

  OS_PRINTF("menu manager inital 1\n");

  ap_frm_run();
  OS_PRINTF("menu manager inital 2\n");

  ui_enable_uio(TRUE);
  ui_enable_playback(TRUE);

  ui_time_init();

  ui_epg_init();
  /* activate EPG */
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  //activate ota
  //ui_ota_api_init();

  // open signal monitor
  ui_init_signal();
#ifdef UPDATE_PG_BACKGROUND
  ui_table_monitor_init();
#endif

  //create timer for roll.
  #ifndef WIN32
	ui_create_hw_timer(20);
  #endif
  
#if ENABLE_NETWORK
    Iptv_Open_State();

    Iptv_Register_Evtmap();

    Iptv_Set_Desktop();
#endif
  return SUCCESS;
}


static RET_CODE on_pre_open(u32 event, u32 para1, u32 para2)
{
  u8 new_root = (u8)(para1);

  _before_open_menu(CURN_MENU_ROOT_ID, new_root, para1, para2);

  return SUCCESS;
}


static RET_CODE on_pre_close(u32 event, u32 para1, u32 para2)
{
  u8 old_root = (u8)(para1);
  _before_close_menu(old_root);
#if 1
  if(old_root == ROOT_ID_RECORD_MANAGER)
  {
    ui_play_curn_pg();//hack hack
    //ui_stop_play(STOP_PLAY_BLACK, TRUE);
  }
#endif
  return SUCCESS;
}

BOOL ui_ca_get_roll_status(void)
{
  return g_ca_roll_status;
}

void ui_ca_set_roll_status(BOOL is_enable)
{
    g_ca_roll_status = is_enable;
}

void ui_update_roll_timer_cnt(void)
{ 
  //UI_PRINTF("11111 g_roll_timer_cnt:%d\n", g_roll_timer_cnt);
  if(g_roll_timer_cnt != 0)
  {
    g_roll_timer_cnt --;
  }
}

static void timer_rolling(void)
{          	  
  extern RET_CODE fw_post_tmout(u32 dest, u16 content, u32 para1, u32 para2);
  if(ui_ca_get_roll_status() == TRUE)
  { 
    //UI_PRINTF("@@@@@ g_roll_timer_cnt:%d\n", g_roll_timer_cnt);
    if(g_roll_timer_cnt == 0) //msgq count not biger 32
    {
      g_roll_timer_cnt ++;
      ticks_count = mtos_ticks_get();

      fw_post_tmout(ROOT_ID_BACKGROUND, MSG_HEART_BEAT, 0, 0);
    }
    rolling_ticks = mtos_ticks_get();
    if((rolling_ticks - ticks_count) > 20)
    {
    	//mtos_printk("message overtime,send again");
	g_roll_timer_cnt = 0;
    }
  }
}

#ifndef WIN32
void ui_create_hw_timer(u32 interval_ms)
{
  if(g_timer_id == -1)		
  {
    g_timer_id = hal_timer_request(interval_ms, timer_rolling, TRUE);
  }
  
  if(g_timer_id < 0)
  {
    OS_PRINTF("[sub],create timer error ! \n");
    MT_ASSERT(0);
  }
	   
  mtos_task_delay_ms(5);
  hal_timer_start((u8)g_timer_id);     
}

void ui_destroy_hw_timer(void)
{
  hal_timer_stop((u8)g_timer_id);
  hal_timer_release((u8)g_timer_id);
  g_timer_id = -1;
}

void ui_reset_hw_timer(u32 interval_ms)
{
  ui_destroy_hw_timer();
  ui_create_hw_timer(interval_ms);
}
#endif
static RET_CODE on_focus_changed(u32 event, u32 para1, u32 para2)
{
  focus_change_info_t *p_info;
  u8 old_root, new_root;
  #ifdef ENABLE_ADS
  control_t *p_root = NULL;
  #endif
  old_root = (u8)(para1);
  new_root = (u8)(para2);

  p_info = &g_menu_manage.pre_focus_change;
  
  if(g_menu_manage.enable_autoswitch)
  {
    if(p_info->is_changed)
    {
      /* just update to */
      p_info->to_id = new_root;
    }
    else
    {
      /* start to save */
      p_info->is_changed = TRUE;
      p_info->from_id = old_root;
      p_info->to_id = new_root;
    }

    /* because paint menu is too slow, so move it from auto_switch */
    _switch_playstatus(p_info->from_id, p_info->to_id);
    _switch_autoclose(p_info->from_id, p_info->to_id);

    #ifdef ENABLE_CA
    #ifndef ONLY1_CA_VER
    cas_manage_finger_repaint();
    OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
    cas_manage_super_osd_repaint();
    OS_PRINTF("function :%s ,redraw_super_osd_again\n",__FUNCTION__);
    #endif
    #endif

    //auto open adv when switch to mainmenu
#ifdef ENABLE_ADS
    if( (new_root == ROOT_ID_MAINMENU)
      && (old_root != new_root)
      && (old_root != ROOT_ID_BACKGROUND))
    {
      p_root = fw_find_root_by_id(ROOT_ID_MAINMENU);
      if(NULL !=  p_root)
      {
          OS_PRINTF("send MSG_REFRESH_ADS_PIC to mainmenu\n");
          fw_notify_root(p_root, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
      }
    }
    else if((old_root == ROOT_ID_MAINMENU)
      && (old_root != new_root)
      && (!ui_is_popup_menu(new_root)) )
    {
      OS_PRINTF("old_root=%d, new_root=%d\n",old_root,new_root);
      pic_adv_stop();
    }
  #endif
  }
  
  if((p_info->from_id != p_info->to_id)
    && (p_info->from_id == ROOT_ID_BACKGROUND))
  {
    #ifdef UPDATE_PG_BACKGROUND
    ui_table_monitor_able_bat(FALSE, FALSE);
    #endif
    
    #ifdef QUICK_SEARCH_SUPPORT
    ui_table_monitor_able_sdt_otr(FALSE, FALSE);
    #endif
  }
  else if((p_info->from_id != p_info->to_id)
    && (p_info->to_id == ROOT_ID_BACKGROUND))
  {
    #ifdef UPDATE_PG_BACKGROUND
    ui_table_monitor_able_bat(TRUE, FALSE);
    #endif
    
    #ifdef QUICK_SEARCH_SUPPORT
    ui_table_monitor_able_sdt_otr(TRUE, FALSE);
    #endif
  }

  #ifdef MIS_ADS
  if((p_info->from_id != p_info->to_id) && (p_info->from_id == ROOT_ID_BACKGROUND))
  {
    UI_PRINTF("(p_info->from_id == ROOT_ID_BACKGROUND)\n");
    UI_PRINTF("p_info->to_id == %d\n", p_info->to_id);
    /* 1. »ØÍË¼ü/ÉÏÏÂ¼ü/¶ÀÁ¢ÆµµÀ¼ü/Êý×Ö¼ü/Á¬Ðø¿ìËÙµÈ ²»Í¬·½Ê½?ÐÌ¨¿çÌ¨×ÖÄ»¶¼ÄÜÏÔÊ?
        2. ?ó¶¨Í¬Ò»Ìõ¿çÌ¨×ÖÄ»µÄ¶àÆµµÀ¼äÇÐÌ¨£¬¿çÌ¨×ÖÄ»²»ÏûÊ?²âÊÔÁ½ÖÖÇé¿ö£ºÍ¬Æµµã£¬¿çÆµµã)£¬STBÆÁÏÔ½çÃæ²»»á±»Çå³ý
        3. ¾²Òô¼ü/×óÓÒ¼ü/¶ÀÁ¢ÒôÁ¿¼ü/ÉùµÀ¼üµÈµ¯³öÐ¡Í¼±ê°´ü£¬×ÖÄ»²»ÏûÊ§£¬STB½çÃæÎÞ²ÐÈ±
        4. ²Ëµ¥¼ü/ÆµµÀÁÐ±í¼ü/Ï²°®¼ü/ÐÅÏ¢¼üµÈÈ«ÆÁÍ¼±êµÄ°´¼ü£¬×Ö»ÏûÊ§£¬STBçÃæÎÞ²ÐÈ?*/
    if((p_info->to_id == ROOT_ID_MUTE) || (p_info->to_id == ROOT_ID_VOLUME) || (p_info->to_id == ROOT_ID_AUDIO_SET)
            || (p_info->to_id == ROOT_ID_NUM_PLAY) || (p_info->to_id == ROOT_ID_PROG_BAR))
    {
      ads_set_menu_status(MENU_ATTR_NO_FULLSCREEN_0);
    }
    else
    {
      ads_set_menu_status(MENU_ATTR_NO_FULLSCREEN_1);
    }
  }
  else if((p_info->from_id != p_info->to_id) && (p_info->to_id == ROOT_ID_BACKGROUND))
  {
    UI_PRINTF("(p_info->to_id == ROOT_ID_BACKGROUND)\n");
    ads_set_menu_status(MENU_ATTR_FULLSCREEN);
  }
  #endif
	
  CURN_MENU_ROOT_ID = new_root;
  #ifdef TENGRUI_ROLLTITLE
  {
        #include "subt_pic_ware.h"
        #include "ui_roll_title.h"

        extern RET_CODE  subt_pic_module_stop(u32 m_id);
        extern RET_CODE  subt_pic_module_open(u32 m_id);

       UI_PRINTF("ROLL TITLE on_focus_changed CURN_MENU_ROOT_ID=%d\n",CURN_MENU_ROOT_ID);

        if(CURN_MENU_ROOT_ID == ROOT_ID_BACKGROUND)
        {
          //if(old_root == ROOT_ID_PROG_BAR)
          {
            UI_PRINTF("CURN_MENU_ROOT_ID == ROOT_ID_BACKGROUND\n");
            fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_START_TR_TITLE);
            fw_tmr_create(ROOT_ID_BACKGROUND, MSG_START_TR_TITLE, 120*1000, FALSE);
          }
        }
        else
        {
          if(old_root != ROOT_ID_PROG_LIST)
          {
            fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_START_TR_TITLE);

            UI_PRINTF("CURN_MENU_ROOT_ID != ROOT_ID_BACKGROUND\n");
            subt_pic_module_stop(SUBT_PIC_ID_ADT_TR);
            ui_set_roll_title_show(FALSE);
          }
	  else
	  {
	    ui_set_roll_title_show(FALSE);
	  }
        }
      }
  #endif

  return SUCCESS;
}


static RET_CODE on_uio_event(u32 event, u32 para1, u32 para2)
{
  u8 focus_id = 0;
  control_t *p_focus = NULL;

  if(CURN_MENU_CLOSE_TYPE == ON)
  {
    manage_tmr_reset();
  }

  focus_id = fw_get_focus_id();
  p_focus = fw_find_root_by_id(focus_id);
  if(p_focus != NULL)
  {
    ctrl_process_msg(p_focus, MSG_UIO_EVENT, 0, 0);
  }

#ifdef SLEEP_TIMER
  uio_reset_sleep_timer();
#endif

  return SUCCESS;
}

static RET_CODE on_after_close(u32 event, u32 para1, u32 para2)
{

#ifndef MULTI_PIC_ADV 
  u8 old_root = (u8)(para1);
#endif

#ifdef SMSX_CA
  extern void redraw_finger_again(void);
  redraw_finger_again();
#endif
#ifndef MULTI_PIC_ADV 
  if(ROOT_ID_PROG_BAR == old_root)
  {
    OS_PRINTF("@@on_after_close: show watermark!! \n");
    ui_show_watermark();
  }

  #endif
  return SUCCESS;
}

BOOL ui_menu_reset_proc(u8 root_id, open_menu_t open_func)
{
  u8 idx;
  BOOL ret = FALSE;

  OS_PRINTF("ui_menu_reset_proc %d\n",root_id);

  idx = manage_find_menu(root_id);
  if(idx == INVALID_IDX)
  {
    return FALSE;
  }

  all_menu_attr[idx].open_func = open_func;
  ret = TRUE;

  return ret;

}

BEGIN_WINDOW_MANAGE(ui_menu_manage)
ON_MENU_EVENT(WINDOW_MANAGE_ON_UIO_EVENT, on_uio_event)
ON_MENU_EVENT(WINDOW_MANAGE_FOCUS_CHANGED, on_focus_changed)
ON_MENU_EVENT(WINDOW_MANAGE_PRE_OPEN, on_pre_open)
ON_MENU_EVENT(WINDOW_MANAGE_PRE_CLOSE, on_pre_close)
ON_MENU_EVENT(WINDOW_MANAGE_INIT, on_initialize)
ON_MENU_EVENT(WINDOW_MANAGE_CLOSED, on_after_close)
END_WINDOW_MANAGE(ui_menu_manage)
