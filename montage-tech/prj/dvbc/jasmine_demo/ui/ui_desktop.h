/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/ 
#ifndef __UI_DESKTOP_H__
#define __UI_DESKTOP_H__

#define MOV_DLG_TMOUT (20*SECOND)

#define OUTRANGE_DLG_TMOUT (5*SECOND)

#ifdef ENABLE_NETWORK
#define UI_SEEK_TIME   60

#define MIN_SEEK_STEP   (60)
#define MAX_SEEK_STEP   (600)
#define SEEK_STEP_INCR  (60)
#define MAC_CHIPID_LEN (128)
#endif

enum desktop_local_msg
{
  MSG_START_TTX = MSG_LOCAL_BEGIN + 100,
  MSG_STOP_TTX,
  MSG_SWITCH_SCART,
  MSG_DURATION_TIMEOUT,
  MSG_SWITCH_PN,
  MSG_SWITCH_SUBT,
  MSG_SWITCH_VIDEO_MODE,
  MSG_SWITCH_ASPECT_MODE,
  MSG_SWITCH_LANGUAGE,
  MSG_EPG,
  MSG_START_RECORD,
  MSG_SLEEP,
  MSG_OPEN_RECORD_MANAGER,
  MSG_EDIT_UART,
  MSG_PLAY,
  MSG_STOP_TTX_AND_REC,
  MSG_RADIO_FUNCTION,
  MSG_TV_FUNCTION,
  MSG_FACT_TEST,
  MSG_DELEALL_AISAT,
#ifdef ENABLE_NETWORK
  MSG_OPEN_LIVETV_MENU,
  MSG_DLNA_START_PICTURE,
  MSG_DLNA_CLOSE_PICTURE,
  MSG_DLNA_START_VIDEO,
  MSG_DLNA_CLOSE_VIDEO,
#endif
#if (defined JHC_CN_DS52) || (defined JHC_DALIAN_ZHUANGGUANG)
  MSG_KEY_F3,
#endif
};

#ifdef WIN32
enum local_msg_x86
{
  MSG_TEST1 = MSG_LOCAL_BEGIN + 200,
  MSG_TEST2,
  MSG_TEST3,
  MSG_TEST4,
  MSG_TEST5,
  MSG_TEST6,
  MSG_TEST7,
  MSG_TEST8,
  MSG_TEST9,
  MSG_TEST0,
};
#endif

#define UI_ROLL_TEXT_SIZE (1024)
typedef enum
{
  ROLL_TEXT_SPEED_NORMAL,
  ROLL_TEXT_SPEED_SLOW_X1,
  ROLL_TEXT_SPEED_SLOW_X2,
  ROLL_TEXT_SPEED_FAST_X1,
  ROLL_TEXT_SPEED_FAST_X2,

  ROLL_TEXT_SPEED_NUM
}ROLL_TEXT_SPEED;

#ifdef ENABLE_NETWORK
typedef struct
{
  BOOL is_eth_insert; //TRUE means ethernet cable insert
  BOOL is_eth_conn; //TRUE means ethernet connect successfully
  BOOL is_wifi_insert; //TRUE means wifi device insert
  BOOL is_wifi_conn; //TRUE means wifi connect successfully
  BOOL is_gprs_conn; //TRUE means gprs connect successfully
  BOOL is_3g_insert; //TRUE mens 3g insert
  BOOL is_3g_conn;   // TRUE means 3g connect succefully
}net_conn_stats_t;

typedef struct tag_net_conn_info
{
    u8 eth_conn_info;
    u8 wifi_conn_info;
    u8 gprs_conn_info;
    u8 g3_conn_info;

}net_conn_info_t;
#endif

typedef struct 
{
  u32 location;
  rect_t rect;
  u32 bg_color;
  u32 font_color;
  u32 font_size;
  ROLL_TEXT_SPEED speed;
  u8 text[UI_ROLL_TEXT_SIZE];
  roll_param_t param;
}ui_desktop_roll_text_t;



void ui_desktop_init (void);

void ui_desktop_release(void);

void ui_desktop_main(void);

 void ui_desktop_start(void);

void ui_sleep_timer_create(void);

void ui_sleep_timer_destory(void);

void ui_sleep_timer_reset(void);

void uio_reset_sleep_timer(void);

void ui_set_usb_status(BOOL);

BOOL ui_get_usb_status(void);

void ui_set_enter_standby(BOOL enter_standby);

BOOL ui_get_enter_standby(void);

RET_CODE on_switch_preview(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


BOOL ui_is_desktop_inited(void);
BOOL is_desktop_rolling_text(void);

void on_desktop_save_ota_info(cas_ota_info_t *ota_info_temp);
void do_update_software(void);

#ifdef ENABLE_NETWORK
void ui_evt_disable_ir(void);
void ui_evt_enable_ir(void);
void ui_set_net_connect_status( net_conn_stats_t);
net_conn_stats_t ui_get_net_connect_status(void);
net_conn_info_t ui_get_conn_info(void);
void ui_set_conn_info(net_conn_info_t net_conn_info);
void ui_get_stb_mac_addr_chip_id(u8 *p_mac, u8 *p_id);
BOOL ui_get_ttx_flag(void);
void ui_set_ttx_flag(BOOL flag);
#endif
#endif

