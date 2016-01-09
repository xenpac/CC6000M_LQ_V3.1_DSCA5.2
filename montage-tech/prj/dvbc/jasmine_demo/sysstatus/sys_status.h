/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/


#ifndef __SYS_STAUTS_H__
#define __SYS_STAUTS_H__

#define MAX_CATEGORY_CNT     (24) //use fav flag, use in MAX_FAV_CNT
#define MAX_PRESET_FAV_CNT   (24)
#define MAX_FAV_CNT          (MAX_CATEGORY_CNT + MAX_PRESET_FAV_CNT)
#define MAX_GROUP_CNT        (64 + MAX_FAV_CNT + 0x1 /*all group*/)

#define EVENT_NAME_LENGTH    (16)
#define MAX_BOOK_PG          20
#define MAX_FAV_NAME_LEN	 24
#define INFO_STRING_LEN  16
#define MAX_CHANGESET_LEN 20
//lint -e601 -e10 -e148 -e38 -e129 -e49  -e101 -e132 -e578  -e745 -e939 -e752 
#ifdef ENABLE_NETWORK
#define MAX_IP_CNT                (8)
#define MAX_CITY_CNT          10//(8)
#define ALBUMS_ID_LENHGT 32
#define MAX_FRIENDS_NUM 10
#define FAV_LIVETV_NUM    (100)
#define FAV_IPTV_NUM       (100)
#define MAX_CONN_PLAY_CNT (100)
#define MAX_PLAY_HIST_CNT (20)
#define MAX_PG_NAME_LEN	(32)
#define MAX_WIFI_NUM 32
#endif

#ifdef HUANGSHI_PROJECT
#define SEARCH_FREQ_MIN 570000
#define SEARCH_FREQ_MAX 642000
#define FULL_SCAN_CNT 10
#else
#define SEARCH_FREQ_MIN 474000
#define SEARCH_FREQ_MAX 858000
#define FULL_SCAN_CNT 48
#endif
#define DTMB_HN_SYMBOL_DEFAULT 8000;
#define SEARCH_FREQ_NIT 602000
#define SEARCH_FREQ_BIT  6
#define SEARCH_SYM_MIN 0
#define SEARCH_SYM_MAX 9999
#define SEARCH_SYM_BIT 4

#define MAX_FILE_COUNT (1000)

#ifdef ONLY1_CA_VER
#define MAXLEN_OSD               180U
#endif
typedef enum 
{
  BS_LNB_POWER = 0,
  BS_NIT_RECIEVED,
  BS_ANTENNA_CONNECT,
  BS_MENU_LOCK,
  BS_PROG_LOCK,
  BS_SLEEP_LOCK,
  BS_DVR_ON,
  BS_UNFIRST_UNPOWER_DOWN,
  BS_LCN_RECEIVED,
  BS_FIRST_ON,
  BS_IS_SCART_OUT,
  BS_IS_TIME_OUT_WAKE_UP,
  BS_FACTORY_ENTERED,
  BS_DB_FULL,
  BS_IS_FINGER_ON,
  BS_BOOT_FIRST_PLAY,
  BS_DEFAULT_VOLUME,
  BS_MAX_CNT
}bit_status_t;

typedef enum 
{
  AGE_LIMIT_VIEW_ALL,
  AGE_LIMIT_7,
  AGE_LIMIT_12,
  AGE_LIMIT_15,
  AGE_LIMIT_18,
  AGE_LIMIT_LOCK_ALL,
  AGE_LIMIT_MAX_CNT
}age_limit_t;

typedef enum 
{
  BOOK_TMR_OFF = 0,
  BOOK_TMR_ONCE,
  BOOK_TMR_DAILY,
  BOOK_TMR_WEEKLY,
}book_tmr_mode_t;

/*!
 * book pg information
 */
typedef struct
{
  u16 pgid;
  u8 book_mode : 4;
  u8 record_enable : 4;
  u8 svc_type:4;
  u8 timer_mode:4;
  u16 event_name[EVENT_NAME_LENGTH + 1];
  utc_time_t start_time;
  utc_time_t drt_time;
} book_pg_t;

/*!
 * book information
 */
typedef struct
{
  book_pg_t pg_info[MAX_BOOK_PG];
} book_info_t;

/*!
* favorite group
*/
typedef struct
{
	u16 fav_name[MAX_FAV_NAME_LEN];
} fav_group_t;

typedef struct
{
  fav_group_t fav_group[MAX_FAV_CNT];
} fav_set_t;

/*!
 * group type
 */
enum
{
  GROUP_T_ALL = 0,
  GROUP_T_SAT,
  GROUP_T_FAV
};

/*!
 * scan mode
 */
enum
{
  SCAN_MODE_ALL = 0,
  SCAN_MODE_FREE,
};

/*!
 * curn mode
 */
enum
{
  CURN_MODE_NONE = 0,
  CURN_MODE_TV,
  CURN_MODE_RADIO,
  CURN_MODE_CNT,
};


/*last sat*/
typedef struct
{
  u8 sat_id;
} last_sat_t;

/*!
 * language setting
 */
typedef struct
{
  u8 osd_text;
  u8 first_audio;
  u8 second_audio;
  u8 tel_text;
  u8 sub_title;
  u8 text_encode_video;
  u8 text_encode_music;
} language_set_t;

/*!
 * audio/vidoe setting
 */
typedef struct
{
  u8 tv_mode;
  u8 tv_resolution;
  u8 tv_ratio;
  u8 video_output;
  u8 digital_audio_output;
  u8 rf_system;
  u8 rf_channel;
  u8 video_effects;
} av_set_t;

/*!
 * audio/vidoe setting
 */
typedef struct
{
  u8 is_global_volume;
  u8 global_volume;
  u8 is_global_track;
  u8 global_track;
} audio_set_t;

/*!
 * osd setting
 */
typedef struct
{
  u8 palette;
  u8 transparent;
  u8 timeout;
  u8 enable_teltext;
  u8 enable_subtitle;
  u8 enable_vbinserter;
} osd_set_t;

/*!
 * password setting
 */
typedef struct
{
  u32 normal;
  u32 super;
} pwd_set_t;

/*!
 program group infor
 */
typedef struct
{
  u16 curn_tv;
  u16 curn_radio;
  u16 tv_pos;
  u16 rd_pos;
  u32 context;
}curn_info_t;

/*!
 program group setting
 */
typedef struct
{
  u8          curn_mode;
  u16         curn_group;
  u8          curn_type;  
  curn_info_t group_info[MAX_GROUP_CNT];
} group_set_t;

/*!
 * local setting
 */
typedef struct
{
  u8   positioner_type; // 0 DiSEqC1.2 1 USALS
  u8   lnb_type;
  u16 longitude;
  u16 latitude;
  u16 lnb_h;
  u16 lnb_l;
} local_set_t;

/*!
 * time setting
 */
typedef struct
{
  utc_time_t sys_time;
  u8 gmt_usage  : 1;
  u8 gmt_offset :  6;
  u8 summer_time :1;
} time_set_t;

/*!
 * play type
 */
enum
{
  PLAY_T_ALL = 0,
  PLAY_T_FREE,
  PLAY_T_SCRAMBLE,
};

/*!
 * channel change mode
 */
enum
{
  CHCHANGE_M_FREEZE = 0,
  CHCHANGE_M_BLACKSCR,
};

/*!
 * channel play setting
 */
typedef struct
{
  /* reserve */
  u8 type;
  /* black freeze */
  u8 mode;
} play_set_t;

/*!
 * preset setting
 */
typedef struct
{
  u8 enable;
  u8 tv_track;
  u8 radio_track;
} preset_set_t;

/*version info*/
typedef struct
{
  u16 bl_ver[INFO_STRING_LEN];
  u16 bl_time[INFO_STRING_LEN];
  u16 main_ver[INFO_STRING_LEN];
  u16 main_time[INFO_STRING_LEN];
  u16 rback_ver[INFO_STRING_LEN];
  u16 rback_time[INFO_STRING_LEN];
  u16 vinfo_ver[INFO_STRING_LEN];
  u16 vinfo_time[INFO_STRING_LEN];
  u16 ddb_ver[INFO_STRING_LEN];
  u16 ddb_time[INFO_STRING_LEN];
  u16 udb_ver[INFO_STRING_LEN];
  u16 udb_time[INFO_STRING_LEN];
}sw_info_t;

/*!
  DVBC lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u8 nim_modulate;

  u8 reserve1;
  u8 reserve2;
  u8 reserve3;
} dvbc_lock_t;

/*version info*/
typedef struct
{
  u8 scan_mode;
  u8 fta_only;
  u8 scan_channel;
  u8 network_search;
  u8 tp_scan_mode;
  u8 reserved;
  u16 vpid;
  u16 apid;
  u16 ppid;
}scan_param_t;

/*!
 * picture show size
 */
enum
{
  PIC_SHOW_SIZE_FULL = 0,
  PIC_SHOW_SIZE_REAL,
};

/*picture play mode*/
typedef struct
{
	u8 slide_show;
	u8 slide_time;
	u8 slide_repeat;
	u8 special_effect;
}pic_showmode_t;

/*!
  OTA TDI info definition
*/
typedef struct
{
  /*!
    oui
    */
  u32 oui;
  /*!
    manufacture id
    */
  u16 manufacture_id;
  /*!
    hardware module id
    */
  u16 hw_mod_id;
  /*!
    software module id
    */
  u16 sw_mod_id;
  /*!
    hardware version
    */
  u16 hw_version;
  /*!
    software version
    */
  u16 sw_version; 
  /*!
    reserved
    */
  u16 reserved;

}ota_tdi_t;
/*!
 * system setting
 */
typedef struct
{
  char debug_info;
  char sig_str_ratio;
  char sig_str_max;
  char sig_qua_max;
  char str_l_qua;
  unsigned int str_0_gain;
  char bs_times;
  char spi_clk;
  char rd_mode;
  char standby_mode;
  char watchdog_mode;    			
  char str_poweron[10];
  char str_standby[10];
  char str_sw_version[20]; 
  char str_last_upgrade_date[20];
  ota_tdi_t ota_tdi;
}misc_options_t;

typedef struct
{
/*!
  limit type
  */
  u8   type;
/*!
  is work time
  */
  u8 is_work_time;
/*!
  work time 
  */
  u32   work_time;
/*!
  stop time 
  */
  u32   stop_time;
}watch_limit_t;


#ifdef ENABLE_NETWORK
/*!
 * network link type includes ethernet lan, wifi
 */
typedef enum
{
  LINK_TYPE_LAN,
  LINK_TYPE_WIFI,
  LINK_TYPE_GPRS,
  LINK_TYPE_3G,
}link_type_t;

/*!
 * network config  information
 */
typedef enum
{
  DHCP,
  STATIC_IP,
  PPPOE,
  
}config_mode_t;

typedef struct
{
  config_mode_t config_mode;
  link_type_t link_type;
  
}net_config_t;

typedef struct
{
	u16 ip_name[32];
	u8 ip_account[32];
	u8 ip_password[32];
} ip_group_t;

typedef struct
{
 u16 city_name[32];
} city_group_t;

 typedef struct
{
  ip_group_t ip_group[MAX_IP_CNT];
} ip_set_t;

typedef struct
{
  city_group_t city_group[MAX_CITY_CNT];
} city_set_t;

/*!
 * wifi ap info
 */
typedef struct
{
  u8 essid[32+1];
  u8 key[32+1];
  u8 encrypt_type;
  u8 latest;
}wifi_info_t;

/*!
 * gprs ap info
 */
typedef struct
{
  u8 name[64];
  u8 apn[64];
  u8 dial_num[64];
  u8 usr_name[64];
  u8 passwd[64];
}gprs_info_t;

/*!
 * 3g connect info
 */
typedef struct
{
  u8 name[64];
  u8 apn[64];
  u8 dial_num[64];
  u8 usr_name[64];
  u8 passwd[64];
}g3_conn_info_t;

/*!
 * pppoe connect info
 */
typedef struct
{
  u8 account[32+1];
  u8 pwd[32+1];
}pppoe_info_t;

/*!
 * youtube config info
 */
typedef struct
{
  u16  region_id;
  u8  time_mode;
  u8  resolution;
}youtube_config_t;

/*!
 * network upg info
 */
typedef struct
{
  u8 protocol;
  u8 url_type;
  u16 ip_address[5];
  u8 url[32+1];
  u8 usr[32+1];
}net_upg_info_t;

/*!
 * nm simple config info
 */
typedef struct
{
  u8  resolution;
}nm_simple_config_t;

/*! 
   Format ip address definition
  */
typedef struct
{
  /*!
    s_a1
    */
  u16 s_a1;
  /*!
    s_a2
    */
  u16 s_a2;
  /*!
    s_a3
    */
  u16 s_a3;
  /*!
    s_a4
    */
  u16 s_a4;

}ip_addr_t;

/*!
 *	ipaddress setting
 */
typedef struct
{
  ip_addr_t sys_ipaddress;
  ip_addr_t sys_netmask;
  ip_addr_t sys_gateway;
  ip_addr_t sys_dnsserver;
}ip_address_set_t;

/*!
 *	mac setting
 */
typedef struct
{
  char mac_one[2];
  char mac_two[2];
  char mac_three[2];
  char mac_four[2];
  char mac_five[2];
  char mac_six[2];
}mac_set_t;

/*!
  * epg lang setting
  */
typedef struct
{
  u8 first_lang;

  u8 second_lang;

  u8 default_lang;
  
}epg_lang_t;

/*!
  albums_site_t
  */
typedef struct
{
  u8 user_id[ALBUMS_ID_LENHGT];
  
  u8 site;

  BOOL is_used;
}albums_friend_t;

typedef struct
{
  u8 friends_num;

  albums_friend_t albums_friend[MAX_FRIENDS_NUM];
}albums_friends_t;

typedef struct
{
  u32 live_fav_id;
  u8 live_fav_name[32];
}live_tv_fav_info;

typedef struct
{
  u32 live_tv_total_num;
  live_tv_fav_info live_fav_pg[FAV_LIVETV_NUM];
}live_tv_fav;

typedef struct
{
  u32 iptv_fav_category_id;
  u32 iptv_fav_id;
  u8 b_single_page; 
  u16 iptv_fav_name[32];
}iptv_fav_info;

typedef struct
{
  u32 iptv_total_num;
  iptv_fav_info iptv_fav_pg[FAV_IPTV_NUM];
}iptv_fav;

typedef struct
{
  u8  is_used;
  u16 episode_num;

  u32 timestamp;
  u32 play_time;

  u16 pg_name[MAX_PG_NAME_LEN];
}db_conn_play_item;

typedef struct
{
  u8  is_used;
  u8  b_single_page;

  u32 timestamp;
  u32 vdo_id;
  u32 res_id;

  u16 pg_name[MAX_PG_NAME_LEN];
}db_play_hist_item;

#endif

/*t2 search info */
typedef struct
{
  u8 country_style;
  u8 antenna_state;
  u8 lcn_state;
}t2_search_info;


/*!
 * stb id mode select
 */
enum
{
  STB_ID_ORIGINAL = 0,
  STB_ID_COMBINED1,
  STB_ID_COMBINED2
};

typedef struct
{
  u8 original_stb_id[32];
  u8 combined_stb_id1[32];
  u8 combined_stb_id2[32];
  u8 stb_id_select;
}stb_id_com_t;

typedef struct
{
  u8 net_check;
  u8 ca_slot_check;
  u8 usb_check;
  u8 flash_check;
  u8 av_output_check;
  u8 hdmi_output_check;
}factory_test_result_t;

/*! * system setting */
typedef struct
{  
  /* change set.no */
  u8 changeset[MAX_CHANGESET_LEN];

  /*version number*/
  u32 sw_version;

  /* language settting */
  language_set_t lang_set;

  /* av settting */
  av_set_t       av_set;

  /* audio settting */
  audio_set_t    audio_set;

  /* play settting */
  play_set_t play_set;

  /* osd settting */
  osd_set_t      osd_set;

  /* password settting */
  pwd_set_t      pwd_set;

  u32 auto_sleep;

  /*advertisment TP*/
  dvbc_lock_t ad_tp;

  /*upgrade TP*/
  dvbc_lock_t upgrade_tp;

  /*mian TP 1*/
  dvbc_lock_t main_tp1;

  /*mian TP 2*/
  dvbc_lock_t main_tp2;

  /*NVOD TP*/
  dvbc_lock_t nvod_tp;

  /* bit status */
  u32 bit_status;

  /*global media volume*/
  u8 global_media_volume;

  /*brightness*/
  u8 brightness;

  /*contrast*/
  u8 contrast;

  /*saturation*/
  u8 saturation;

  /*dvr jump time*/
  u32 dvr_jump_time;

  /* picture play mode */
  pic_showmode_t pic_showmode;

  /* ucas size */
  u8 usb_work_partition;

  /* timeshift on/off */
  u8 timeshift_switch;

  u8 reserved2;

  /*time out*/
  u32 time_out;

  /*sleep time*/
  utc_time_t sleep_time;

  /* time settting */
  time_set_t     time_set;

  /* watch limit for CDCAS CA */
  watch_limit_t watch_limit;

  /* video_quality */
  u8 video_quality;

  /* default progame number */
  u16 default_logic_number;

  /* need show WaterMark */
  u8 b_WaterMark;

  /* WaterMark x position */
  u16 x_WaterMark;

  /* WaterMark y position */
  u16 y_WaterMark;

  /*force key*/
  u16 force_key;

  /*scan parameter*/
  scan_param_t scan_param;

  /* ota info */
  ota_info_t ota_info;

  /*range search tp begin*/
  dvbc_lock_t range_tp_begin;

  /*range search tp end*/
  dvbc_lock_t range_tp_end;
  
#ifdef ENABLE_NETWORK
  pic_showmode_t pic_showmode_net;

  /*ipaddress setting*/
  ip_address_set_t ipaddress_set;

  epg_lang_t e_lang_set;

  net_upg_info_t netupg_set;
#endif

#ifdef ONLY1_CA_VER
 utc_time_t   end_time;
 u16 data_len;
 u8   osd_data[MAXLEN_OSD];
 u32 osd_time;
#endif

#ifdef NIT_SETTING
 u8 nit_setting;
 u8 scankey_status;
#endif 

#ifdef CHANNEL_FOLLOWING_TABLE  
/*channel version */
  u8 channel_version;
#endif

#ifdef MIS_ADS
    /*MIS ads tp info*/
  dvbc_lock_t MIS_ads_tp_info;

  /*MIS ads dsi pid*/
  u16 MIS_ads_dis_pid;
  
  /*MIS ads openApp dsi pid*/
  u16 MIS_ads_openApp_dis_pid;
#endif

#ifdef PLAAS_DEXIN_STB_ID_COMBINE
  /* Dexin stb id combine for plaas*/
  stb_id_com_t  stb_id_com_info;
#endif
  factory_test_result_t factory_test_result;
} sys_status_t;


/*! * system background data */
typedef struct
{
  /* nit version */
  u8 nit_version;

  /* nit tp number */
  u8 nit_tp_num;

  /* categories count */
  u8 categories_count;

  u16 bouquet_id[MAX_FAV_CNT];

  /*information*/
  sw_info_t ver_info;

  /*card upg time*/
  utc_time_t card_upg_time;

  /*card upg state*/
  u32 card_upg_state;

  /* group settting */
  group_set_t    group_set;

  /* book infor */
  book_info_t book_info;

  /* local setting */
  local_set_t local_set;

  /*fav group*/
  fav_set_t fav_set;

#ifdef ENABLE_NETWORK
  /*ip path*/
  ip_set_t ip_path_set;
  
  u8 ip_path_cnt;

  /*city path*/
  city_set_t city_path_set;

  u8 city_cnt;

  /*ipaddress setting*/
  mac_set_t mac_set;

  /*wifi ap info setting*/
  wifi_info_t wifi_set[MAX_WIFI_NUM];

  /*network config setting*/
  net_config_t net_config_set;

  /*pppoe connect setting*/
  pppoe_info_t pppoe_set;

  youtube_config_t youtube_config_set;

  nm_simple_config_t nm_simple_config_set;

  // add friends 
  albums_friends_t albums_friends;

  //dlna device name  
  char dlna_device_name[64];

  live_tv_fav fav_live_pg ;

  iptv_fav fav_iptv_pg;

  db_conn_play_item conn_play_table[MAX_CONN_PLAY_CNT];

  db_play_hist_item play_hist_table[MAX_PLAY_HIST_CNT];

  /*gprs info setting*/
  gprs_info_t gprs_info;

  g3_conn_info_t g3_conn_info;
#endif
} sys_bg_data_t;


void sys_status_init(void);

void sys_status_load(void);

void sys_status_save(void);

sys_status_t *sys_status_get(void);

void sys_bg_data_load(void);

void sys_bg_data_save(void);

sys_bg_data_t *sys_bg_data_get(void);

void sys_status_get_sw_changeset(u8 *changeset);

u32 sys_status_get_sw_version(void);

void sys_status_set_sw_version(u32 new_version);

void  sys_status_add_book_node(u8 index, book_pg_t *node);

void sys_status_delete_book_node(u8 index);

void sys_status_get_book_node(u8 index, book_pg_t *node);

BOOL sys_status_get_status(u8 type, BOOL *p_status);

BOOL sys_status_set_status(u8 type, BOOL enable);

void sys_status_set_pwd_set(pwd_set_t *p_set);

void sys_status_get_pwd_set(pwd_set_t *p_set);

void sys_status_set_time(time_set_t *p_set);

void sys_status_get_time(time_set_t *p_set);

void sys_status_set_play_set(play_set_t *p_set);

void sys_status_get_play_set(play_set_t *p_set);

void sys_status_set_av_set(av_set_t *p_set);

void sys_status_get_av_set(av_set_t *p_set);

void sys_status_set_audio_set(audio_set_t *p_set);

void sys_status_get_audio_set(audio_set_t *p_set);

void sys_status_set_video_quality_set(u8 *p_set);

void sys_status_get_video_quality_set(u8 *p_set);

void sys_status_set_osd_set(osd_set_t *p_set);

void sys_status_get_osd_set(osd_set_t *p_set);

void sys_status_set_adver_tp(dvbc_lock_t *p_set);

void sys_status_get_adver_tp(dvbc_lock_t *p_set);

void sys_status_set_upgrade_tp(dvbc_lock_t *p_set);

void sys_status_get_upgrade_tp(dvbc_lock_t *p_set);

void sys_status_set_main_tp1(dvbc_lock_t *p_set);

void sys_status_get_main_tp1(dvbc_lock_t *p_set);

void sys_status_set_main_tp2(dvbc_lock_t *p_set);

void sys_status_get_main_tp2(dvbc_lock_t *p_set);

void sys_status_set_nvod_tp(dvbc_lock_t *p_set);

void sys_status_get_nvod_tp(dvbc_lock_t *p_set);

void sys_status_set_range_begin_tp(dvbc_lock_t *p_set);

void sys_status_get_range_begin_tp(dvbc_lock_t *p_set);

void sys_status_set_range_end_tp(dvbc_lock_t *p_set);

void sys_status_get_range_end_tp(dvbc_lock_t *p_set);

void sys_status_set_lang_set(language_set_t *p_set);

void sys_status_get_lang_set(language_set_t *p_set);

BOOL sys_status_get_fav_name(u8 index, u16 *name);

void sys_status_set_local_set(local_set_t *p_set);

BOOL sys_status_set_fav_name(u8 index, u16 *name);

void sys_status_get_local_set(local_set_t *p_set);

void sys_status_set_scan_param(scan_param_t *p_set);

void sys_status_get_scan_param(scan_param_t *p_set);

void sys_status_get_sw_info(sw_info_t *p_sw);

void sys_status_set_sw_info(sw_info_t *p_sw);

void sys_status_set_time_zone(void);

void sys_status_get_utc_time(utc_time_t *p_time);

void sys_status_set_utc_time(utc_time_t *p_time);

void sys_status_get_sleep_time(utc_time_t *p_time);

void sys_status_set_sleep_time(utc_time_t *p_time);

void sys_status_get_pic_showmode(pic_showmode_t *p_showmode, BOOL is_net);

void sys_status_set_pic_showmode(pic_showmode_t *p_showmode);

u32 sys_status_get_standby_time_out(void);

void sys_status_set_standby_time_out(u32 time_out);

char **sys_status_get_text_encode(void);

char **sys_status_get_lang_code(BOOL is_2_b);

u16 sys_status_get_force_key(void);

ota_info_t *sys_status_get_ota_info(void);

void sys_status_set_ota_info(ota_info_t *p_otai);

u32 sys_status_get_auto_sleep(void);

void sys_status_set_auto_sleep(u32 auto_sleep);

void sys_status_set_usb_work_partition(u8 partition);

u8 sys_status_get_usb_work_partition(void);

void sys_status_set_timeshift_switch(BOOL is_on);

BOOL sys_status_get_timeshift_switch(void);

BOOL sys_get_serial_num(u8 *p_sn, u8 length);

u8 sys_status_get_category_num(void);

BOOL sys_status_set_category_num(u8 index);

u16 sys_status_get_default_logic_num(void);

void sys_status_set_default_logic_num(u16 num);

u16 sys_status_get_bouquet_id(u16 index);

void sys_status_set_bouquet_id(u16 index, u16 bouquet_id);

u8 sys_status_get_categories_count(void);

void sys_status_set_categories_count(u8 count);

void sys_status_get_nit_version(u32 *p_nit_ver);
  
void sys_status_set_nit_version(u32 nit_ver);

void sys_status_reset_channel_and_nit_version(void);

void sys_group_reset(void);

utc_time_t sys_status_get_card_upg_time(void);

void sys_status_set_card_upg_time(utc_time_t card_upg_time);

u32 sys_status_get_card_upg_state(void);
  
void sys_status_set_card_upg_state(u32 card_upg_state);

BOOL sys_status_get_ota_preset(void *p_nim_param);

void get_sys_stbid(u8 idsize,u8 *idbuf,u32 size);

void sys_status_get_country_set(t2_search_info *p_set);

#ifdef ENABLE_NETWORK
void sys_status_get_albums_friends_info(albums_friends_t *p_friends_info);

void sys_status_set_albums_friends_info(albums_friends_t *p_friends_info);

void sys_status_set_ipaddress(ip_address_set_t *p_set);

void sys_status_get_ipaddress(ip_address_set_t *p_set);

void sys_status_set_wifi_info(wifi_info_t *p_set);

void sys_status_get_wifi_info(wifi_info_t *p_set,signed char* name);

void sys_status_set_net_config_info(net_config_t *p_set);

void sys_status_get_net_config_info(net_config_t *p_set);

void sys_status_set_pppoe_config_info(pppoe_info_t *p_set);

void sys_status_get_pppoe_config_info(pppoe_info_t *p_set);

void sys_status_set_youtube_config_info(youtube_config_t *p_set);

void sys_status_get_youtube_config_info(youtube_config_t *p_set);

void sys_status_set_nm_simple_config_info(nm_simple_config_t *p_set);

void sys_status_get_nm_simple_config_info(nm_simple_config_t *p_set);

void sys_status_get_albums_friends_info(albums_friends_t *p_friends_info);
  
void sys_status_set_albums_friends_info(albums_friends_t *p_friends_info);

BOOL sys_status_set_ip_path_set(u8 index,u16 *name);

BOOL sys_status_get_ip_path_set(u8 index,u16 *name);

void sys_status_set_ip_path_cnt(u8 ip_path_cnt);

u8 sys_status_get_ip_path_cnt(void);

BOOL sys_status_set_city(u8 index,u16 *name);

BOOL sys_status_get_city(u8 index,u16 *name);

void sys_status_set_city_cnt(u8 city_cnt);

u8 sys_status_get_city_cnt(void);

BOOL sys_status_get_mac(u8 index, char *name);

BOOL sys_status_set_mac(u8 index, char *name);

char  sys_status_get_mac_by_index(u8 index);

void sys_status_get_dlna_device_name(char* name);

void sys_status_set_dlna_device_name(char* name);

void sys_status_get_fav_livetv(live_tv_fav *fav_livetv);

void sys_status_set_fav_livetv(live_tv_fav *fav_livetv);

void sys_status_set_fav_livetv_info(u32 index,live_tv_fav_info *fav_livetv_info);

void sys_status_get_fav_livetv_info(u32 index,live_tv_fav_info *fav_livetv_info);

void sys_status_del_fav_livetv_info(u32 index);

void sys_status_set_fav_livetv_total_num(u32 total_num);

void sys_status_get_fav_livetv_total_num(u32 *total_num);

void sys_status_get_fav_iptv(iptv_fav *fav_iptv);

void sys_status_set_fav_iptv(iptv_fav *fav_iptv);

void sys_status_set_fav_iptv_info(u32 index,iptv_fav_info *fav_iptv_info);

void sys_status_get_fav_iptv_info(u32 index,iptv_fav_info *ip_tv_info);

void sys_status_del_fav_iptv_info(u32 index);

void sys_status_set_net_upg_config_info(net_upg_info_t *p_info);

void sys_status_get_net_upg_config_info(net_upg_info_t *p_info);

BOOL sys_status_get_ota_preset(void *p_nim_param);

void sys_status_set_fav_iptv_total_num(u32 total_num);

void sys_status_get_fav_iptv_total_num(u32 *total_num);

db_conn_play_item *sys_status_get_conn_play_table(void);

void sys_status_read_conn_play_item(u16 pos, db_conn_play_item *item);

void sys_status_write_conn_play_item(u16 pos, db_conn_play_item *item);

s32 sys_status_find_free_conn_play_item(void);

db_play_hist_item *sys_status_get_play_hist_table(void);

void sys_status_read_play_hist_item(u16 pos, db_play_hist_item *item);

void sys_status_write_play_hist_item(u16 pos, db_play_hist_item *item);

s32 sys_status_find_free_play_hist_item(void);

gprs_info_t *sys_status_get_gprs_info(void);

void sys_status_set_gprs_info(gprs_info_t *p_gprs_info);

g3_conn_info_t *sys_status_get_3g_info(void);

void sys_status_set_3g_info(g3_conn_info_t *p_3g_info);

BOOL sys_status_get_ip_passwd(u8 index,u8 *passwd);

BOOL sys_status_set_ip_passwd(u8 index,u8 *passwd);

BOOL sys_status_get_ip_account(u8 index,u8 *account);

BOOL sys_status_set_ip_account(u8 index,u8 *account);

void sys_status_get_epg_lang(epg_lang_t *e_lang);

void sys_status_set_epg_lang(epg_lang_t *e_lang);
#endif

#ifdef ONLY1_CA_VER
void sys_status_set_osd_content(char* osd_content,u16 len);
void sys_status_get_osd_content(char* osd_content,u16* len);
void sys_status_set_end_time(utc_time_t* current_time,u8 durtime);
void sys_status_get_end_time(utc_time_t* end_time);
u32 sys_status_get_osd_time(void);
void sys_status_set_osd_time(u32 second);
#endif

#ifdef NIT_SETTING
void sys_status_set_nit_setting(u8 nit_setting);
u32 sys_status_get_nit_setting(void);
void sys_status_set_scankey_status(u8 scankey_status);
u32 sys_status_get_scankey_status(void);
//void sys_status_set_current_time(utc_time_t*  current_time);
#endif

#ifdef CHANNEL_FOLLOWING_TABLE
u8 sys_status_get_channel_version(void);
void sys_status_set_channel_version(u8 new_version);
#endif

#ifdef MIS_ADS
void sys_status_get_MIS_ads_tp_info(dvbc_lock_t *p_set);
void sys_status_set_MIS_ads_tp_info(dvbc_lock_t *p_set);
void sys_status_get_MIS_ads_dis_pid(u16 *p_dsi_pid);
void sys_status_set_MIS_ads_dis_pid(u16 p_dsi_pid);
void sys_status_get_MIS_ads_openApp_dis_pid(u16 *p_openApp_dsi_pid);
void sys_status_set_MIS_ads_openApp_dis_pid(u16 p_openApp_dsi_pid);
#endif

#ifdef PLAAS_DEXIN_STB_ID_COMBINE
void sys_status_get_stb_id_info(stb_id_com_t *p_set);
void sys_status_set_stb_id_info(stb_id_com_t *p_set);
#endif

void sys_status_get_factory_test_result(factory_test_result_t *p_result);
void sys_status_set_factory_test_result(factory_test_result_t *p_result);

//lint +e601 +e10 +e148 +e38 +e129 +e49 +e101 +e132 +e578 +e745 +e939 +e752

#endif

