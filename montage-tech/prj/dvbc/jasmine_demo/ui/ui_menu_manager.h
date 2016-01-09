/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_MENU_MANAGER_H__
#define __UI_MENU_MANAGER_H__


enum ui_root_id
{
  ROOT_ID_BACKGROUND = 1,
  ROOT_ID_MAINMENU = 2,
  ROOT_ID_PROG_LIST = 4,
  ROOT_ID_DELETE_ALL = 5,
  ROOT_ID_TP_SEARCH = 6,
  ROOT_ID_DO_SEARCH = 7,
  ROOT_ID_LANGUAGE = 8,

  ROOT_ID_TVSYS_SET = 11,
  ROOT_ID_TIME_SET = 12,
  ROOT_ID_BOOK_LIST = 13,
  ROOT_ID_PARENTAL_LOCK = 14,
  ROOT_ID_OSD_SET = 15,
  ROOT_ID_FAVORITE = 16,
  ROOT_ID_INFO = 18,
  ROOT_ID_FREQ_SET = 19,

  ROOT_ID_FACTORY_SET = 21,
  ROOT_ID_OTA = 22,
  ROOT_ID_UPGRADE = 23,
  ROOT_ID_TETRIS = 24,
  ROOT_ID_OTHELLO = 25,
  ROOT_ID_POPUP = 26,
  ROOT_ID_PASSWORD = 27,
  ROOT_ID_VOLUME = 28,

  ROOT_ID_SIGNAL = 31,
  ROOT_ID_MUTE = 32,
  ROOT_ID_PAUSE = 33,
  ROOT_ID_AUDIO_SET = 34,
  ROOT_ID_PROG_BAR = 35,
  ROOT_ID_NUM_PLAY = 36,
  ROOT_ID_NOTIFY = 37,
  ROOT_ID_ZOOM = 38,

  ROOT_ID_FIND = 41,
  ROOT_ID_EPG = 42,
  ROOT_ID_EPG_DETAIL = 43,
  ROOT_ID_MULTIVIEW = 44,
  ROOT_ID_FAV_SET = 45,
  ROOT_ID_SORT_LIST = 46,
  ROOT_ID_RENAME = 47,
  ROOT_ID_TIMER = 48,

  ROOT_ID_SMALL_LIST = 51,
  ROOT_ID_FAV_LIST = 52,
  ROOT_ID_OTA_SEARCH = 54,
  ROOT_ID_TEST_MENU = 55,
  ROOT_ID_TEST_FKEY = 56,
  ROOT_ID_TEST_TIMER = 57,  
  ROOT_ID_SLEEP_TIMER = 58,

  ROOT_ID_TTX_HELP = 61,
  ROOT_ID_TS_OTA_SEARCH = 62,
  ROOT_ID_KEYBOARD = 63,
  ROOT_ID_SCHEDULE = 64,
  ROOT_ID_EPG_FIND = 65,
  ROOT_ID_UPGRADE_BY_USB = 66,
  ROOT_ID_DUMP_BY_USB = 67,
  ROOT_ID_REMOVE_USB = 68,

  ROOT_ID_USB_MUSIC = 71,
  ROOT_ID_RECORD_MANAGER = 72,
  ROOT_ID_HDD_INFO = 73,
  ROOT_ID_STORAGE_FORMAT = 74,
  ROOT_ID_DVR_CONFIG = 75,
  ROOT_ID_JUMP = 76,
  ROOT_ID_PVR_REC_BAR = 77,
  ROOT_ID_PVR_PLAY_BAR = 78,

  ROOT_ID_PICTURE = 82,
  ROOT_ID_PIC_PLAY_MODE_SET = 83,
  ROOT_ID_UPGRADE_BY_RS232 = 84,
  ROOT_ID_TIMESHIFT_BAR = 85,  
  ROOT_ID_SLEEP_HOTKEY = 86,
  ROOT_ID_V_RESOLUTION = 87,
  ROOT_ID_PROG_DETAIL = 88,

  ROOT_ID_KEYBOARD_V2 = 91,
  ROOT_ID_USB_PICTURE = 93,
  ROOT_ID_USB_MUSIC_FULLSCREEN = 94,
  ROOT_ID_GAME = 95,
  ROOT_ID_TEST = 96,
  ROOT_ID_RENAME_V2 = 97,
  ROOT_ID_VOLUME_USB = 98,
  
  ROOT_ID_FACTORY_TEST = 100,  
  ROOT_ID_USB_TEST = 101,
  ROOT_ID_SUBT_LANGUAGE =102,
  ROOT_ID_FILEPLAY_BAR = 103,
  ROOT_ID_FILEPLAY_SUBT = 104,
  ROOT_ID_FP_AUDIO_SET = 105,
  ROOT_ID_VIDEO_GOTO = 106,
  ROOT_ID_USB_FILEPLAY = 107,
  ROOT_ID_TEXT_ENCODE = 108,
  
  ROOT_ID_SMALL_LIST_PVR = 111,
  ROOT_ID_AUDIO_SET_RECORD = 112,
  ROOT_ID_AUTO_SEARCH = 113,
  ROOT_ID_MANUAL_SEARCH = 114,
  ROOT_ID_RANGE_SEARCH = 115,
  ROOT_ID_CHANNEL_EDIT = 116,
  ROOT_ID_PRO_INFO = 117,
  ROOT_ID_NVOD = 118,
  ROOT_ID_NVOD_VIDEO = 119,
  ROOT_ID_NVOD_VOLUME = 120,
  ROOT_ID_CATEGORY = 121,
  ROOT_ID_FAV_EDIT = 122,
  ROOT_ID_FACTORY_MODE = 123,
  ROOT_ID_TS_RECORD = 124,
  ROOT_ID_IMPORT = 125,
  ROOT_ID_EXPORT = 126,
  ROOT_ID_DISPLAY = 127,
  ROOT_ID_MANUAL_SETTING = 128,
  ROOT_ID_POWER_ON_CHANNEL = 129,
  ROOT_ID_MOSAIC = 130,
  ROOT_ID_MOSAIC_PLAY = 131,
  ROOT_ID_NIT_SETTING = 132,
#ifdef ENABLE_NETWORK
  ROOT_ID_WEATHER_FORECAST = 133,
  ROOT_ID_NETWORK_PLAYBAR = 134,
  ROOT_ID_WIFI_LINK_INFO = 135,
  ROOT_ID_WIFI = 136,
  ROOT_ID_NETWORK_CONFIG = 137,
  ROOT_ID_ONMOV_WEBSITES = 138,
  ROOT_ID_NETWORK_PLAY = 139,
  ROOT_ID_YOUTUBE = 140,
  ROOT_ID_EDIT_USR_PWD = 141,
  ROOT_ID_GOOGLE_MAP = 142,
  ROOT_ID_PING_TEST = 143,
  ROOT_ID_PPPOE_CONNECT = 144,
  ROOT_ID_NETWORK_PLACES = 145,
  ROOT_ID_EDIT_IP_PATH = 146,
  ROOT_ID_EDIT_IPADDRESS = 147,
  ROOT_ID_VEPG = 148,
  ROOT_ID_ALBUMS = 149,
  ROOT_ID_NEWS = 150,
  ROOT_ID_SATIP = 151,
  ROOT_ID_DLNA_DMR = 152,
  ROOT_ID_SUBMENU_NETWORK = 153,
  ROOT_ID_VIDEO_PLAYER = 154,
  ROOT_ID_NETMEDIA = 155,
  ROOT_ID_NM_SIMPLE = ROOT_ID_NETMEDIA,
  ROOT_ID_NM_CLASSIC = ROOT_ID_NETMEDIA,
  ROOT_ID_NM_COUNTRY = ROOT_ID_NETMEDIA,
  ROOT_ID_SUBMENU_NM = 156,
  ROOT_ID_LIVE_TV = 157,
  ROOT_ID_IPTV = 158,
  ROOT_ID_IPTV_DESCRIPTION = 159,  
  ROOT_ID_IPTV_SEARCH = 160,
  ROOT_ID_VDO_FAVORITE = 161,
  ROOT_ID_PLAY_HIST = 162,
  ROOT_ID_IPTV_PLAYER = 163,
  ROOT_ID_YOUPORN = 164,
  ROOT_ID_ONMOV_DESCRIPTION = 165,
  ROOT_ID_NETWORK_CONFIG_WIFI = 166,
  ROOT_ID_NETWORK_CONFIG_3G = 167,
  ROOT_ID_NETWORK_CONFIG_GPRS = 168,
  ROOT_ID_NETWORK_CONFIG_LAN = 169,
  ROOT_ID_YAHOO_NEWS = 170,
  ROOT_ID_NETWORK_MUSIC = 171,
  ROOT_ID_DLNA_START = 172,
  ROOT_ID_REDTUBE = 173,
  ROOT_ID_EDIT_CITY = 174,
  ROOT_ID_SUBNETWORK_CONFIG = 175,
  ROOT_ID_PHOTO_SHOW = 176,
  ROOT_ID_NEWS_TITLE = 177,
  ROOT_ID_NEWS_INFO = 178,
  ROOT_ID_PHOTO_INFO = 179,
  ROOT_ID_PHOTOS = 180,
  ROOT_ID_MOVIE_SEARCH = 181,
  ROOT_ID_DLNA_NET_INFO = 182,  
  ROOT_ID_DLNA_PICTURE = 183,
  ROOT_ID_STB_NAME = 184,
  ROOT_ID_UPGRADE_BY_NETWORK = 185,
  ROOT_ID_NETWORK_UPGRADE = 186,
  ROOT_ID_NETWORK_MUSIC_SEARCH = 187,
  ROOT_ID_MAC_SETTING= 188,
#endif
#ifdef BROWSER_APP
  ROOT_ID_BROWSER = 189,
#endif
  ROOT_ID_TERRESTRIAL_MANUAL_SEARCH = 190,
  ROOT_ID_TERRESTRIAL_FULL_SEARCH = 191,
#ifdef PLAAS_DEXIN_STB_ID_COMBINE
  ROOT_ID_STB_ID_SELECT,
#endif
  ROOT_ID_CA_START = 200,   /**ca_ui_root_id start,don't add id to below!,please add befaut**/
  ROOT_ID_MAX = ROOT_ID_CA_START + 100,
};


#define PS_KEEP                 0 // keep status
#define PS_PLAY                 1 // play full screen
#define PS_PREV                 2 // preview play with logo
#define PS_LOGO                 3 // stop and show logo
#define PS_STOP                 4 // stop
#define PS_TS                   5 // ts play
#define PS_TS_PREV              6 // preview play TS

#define OFF                     0
#define ON                      1

#define SM_OFF                  0 // needn't signal message
#define SM_LOCK                 1 // need message when lock status changed
#define SM_BAR                  2 // need message always

#define MENU_TYPE_FULLSCREEN    0
#define MENU_TYPE_PREVIEW       1
#define MENU_TYPE_NORMAL        2
#define MENU_TYPE_POPUP         3
#define MENU_TYPE_USB_PREV    4
#define MENU_TYPE_VIDEO         5
#define MENU_MAX_CNT   200

enum menu_msg
{
  MSG_SWITCH_PREVIEW = MSG_LOCAL_BEGIN + 125,
};

typedef  RET_CODE (*open_menu_t)(u32 para1, u32 para2);

typedef struct
{
  u8 root_id;
  u8 play_state;                //PS_
  u8 auto_close;                //OFF,ON
  u8 signal_msg;                //SM_
  open_menu_t open_func;      // open function
}menu_attr_t;

typedef struct
{
  u8 root_id;
  u32 logo_id;
}logo_attr_t;

typedef struct
{
  u8 root_id;
  rect_t position;
}preview_attr_t;

#define INVALID_IDX 0xFF

void manage_init(void);

RET_CODE ui_menu_manage(u32 event, u32 para1, u32 para2);

RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_back_to_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_notify_root(u8 root_id, u16 msg, u32 para1, u32 para2);

BOOL ui_is_autoclose_menu(u8 root_id);

void manage_autoclose(void);

menu_attr_t *manage_get_curn_menu_attr(void);

BOOL manage_get_preview_rect(u8 root_id,
                             u16 *left,
                             u16 *top,
                             u16 *width,
                             u16 *height);

void manage_logo2preview(u8 root_id);

void manage_enable_autoswitch(BOOL is_enable);

u8 manage_find_preview(u8 root_id, rect_t *orc);

void ui_close_all_mennus(void);

BOOL ui_is_preview_menu(u8 root_id);

u8 ui_get_preview_menu(void);

BOOL ui_is_fullscreen_menu(u8 root_id);

BOOL ui_is_popup_menu(u8 root_id);

void manage_auto_switch(void);

RET_CODE manage_tmr_reset(void);

BOOL ui_ca_get_roll_status(void);

void ui_ca_set_roll_status(BOOL is_enable);

void ui_update_roll_timer_cnt(void);

void ui_create_hw_timer(u32 interval_ms);

void ui_destroy_hw_timer(void);

void ui_reset_hw_timer(u32 interval_ms);

BOOL ui_menu_reset_proc(u8 root_id, open_menu_t open_func);

#endif
