/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#if ENABLE_INFO_BAR_V2
#include "ui_nprog_bar.h"
#else
#include "ui_prog_bar.h"
#endif
#include "ui_upgrade_by_rs232.h"
#include "ui_ttx_help.h"

#include "ui_video.h"

#ifdef ENABLE_NETWORK
#include "ethernet.h"
#include "net_svc.h"
#include "wifi.h"
#include "ui_wifi.h"
#ifndef WIN32
#include "ppp.h"
#endif
#include "ui_pppoe_connect.h"
#include "wifi.h"
#include "modem.h"
#include "ui_dlna.h"
#include "ui_dlna_api.h"
#include "ui_satip.h"
#include "network_monitor.h"
#include "ui_network_config_3g.h"
#include "ui_network_config_gprs.h"
#include "ui_video.h"
#include "plug_monitor.h"
#include "NetMediaDataProvider.h"
#include "al_netmedia.h"
#include "ui_live_tv_api.h"
#include "IPTVDataProvider.h"
#include "ui_iptv_api.h"
#include "SM_StateMachine.h"
#endif

#include "pnp_service.h"
#include "ui_ca_api.h"
#ifdef ONLY1_CA_VER
#include "ui_ca_super_osd.h"
#endif
#include "ui_upgrade_by_usb.h"
#include "ui_dump_by_usb.h"
#include "ui_sleep_hotkey.h"
#include "ui_dvr_config.h"
#include "ui_hdd_info.h"
#include "ui_timeshift_bar.h"
#include "ui_record_manager.h"
#include "ui_timer.h"
#include "hotplug.h"
#include "customer_config.h"

#ifdef ENABLE_ADS
#if defined TEMP_SUPPORT_DS_AD || defined TEMP_SUPPORT_SZXC_AD || defined BOYUAN_AD
#include "Ui_ads_display.h"
#endif
#endif
//ca
#include "cas_manager.h"
#include "ota_dm_api.h"
#include "ota_public_def.h"
#ifdef NIT_SOFTWARE_UPDATE
#include "bat.h"
#include "user_parse_table.h"
#endif

#ifdef ENABLE_ADS
#include "ui_ad_api.h"
#endif

#ifdef ENABLE_CA
#include "config_cas.h"
#include "ui_ca_api.h"
#endif

#include "lib_rect.h"
#include "lib_memp.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "lib_memf.h"
#include "flinger.h"
#include "gdi.h"
#include "ctrl_base.h"
#include "gui_resource.h"
#include "gui_paint.h"
#include "framework.h"
#include "mdl.h"
#include "uio_key_def.h"
#include "vbi_api.h"
#ifdef TENGRUI_ROLLTITLE
#include "ap_tr_subt.h"
#include "ui_roll_title.h"
#endif

#ifdef FACTORY_TEST_SOFTWARE
//if add this macro , need add ui_factory_test.c, for customer factory test
#include "ui_factory_test.h"
#endif
#include "sys_app.h"
#include "mt_debug.h"
#ifdef ONLY1_CA_VER
#include "ui_finger_print.h"
#endif
#include "ap_nit.h"

BOOL is_tdt_found = FALSE;
static BOOL show_ask_dlg = FALSE;

extern void delete_all_prog(void);
enum local_shfit_msg
{
  MSG_TTX_KEY = MSG_SHIFT_LOCAL_BEGIN + 0x0100,
};

enum
{
  /* value 1 for TITLE_ROLL*/
  // TODO: Check here
  // This value should be bigger than all the ids which are child of background menu.
  IDC_DESKTOP_ROLL_TEXT = 10,
};


u16 sys_evtmap(u32 event);

u16 ui_desktop_keymap_on_normal(u16 key);

u16 ui_desktop_keymap_on_vcr(u16 key);

u16 ui_desktop_keymap_on_standby(u16 key);

RET_CODE ui_desktop_proc_on_normal(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

extern RET_CODE ui_desktop_proc_customer(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
extern u16 ui_desktop_keymap_customer(u16 key);

#if ENABLE_TTX_SUBTITLE
u16 ui_desktop_keymap_on_ttx(u16 key);

RET_CODE ui_desktop_proc_on_ttx(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
#endif
static BOOL g_vcr_input_is_tv = FALSE;
static BOOL time_end_open_menu = FALSE;
static BOOL g_deskto_init = FALSE;
#ifndef WIN32
static BOOL g_usb_status = FALSE; //FALSE: USB OUT; TRUE:USB IN
#else
static BOOL g_usb_status = TRUE; //FALSE: USB OUT; TRUE:USB IN
#endif

#ifdef SLEEP_TIMER
//is the sleep timer has one minute left.

static BOOL g_is_one_min_left = FALSE;
static BOOL g_enter_standby= FALSE;
static BOOL g_is_book_node_dlg_opened = FALSE;
static BOOL is_already_prompt = FALSE;
#ifdef NIT_SOFTWARE_UPDATE
BOOL is_already_prompt_ota = FALSE;
static BOOL g_nit_ota_check = FALSE;

cas_ota_info_t update_info = {0};
#endif
book_pg_t book_node;

extern u32 g_epg_buffer_addr;
extern u32 g_rec_buffer_addr;

#ifdef ENABLE_NETWORK
void do_cmd_disconnect_g3();
extern RET_CODE ui_get_ip(char *ipaddr);
extern void paint_connect_status(BOOL is_conn, BOOL is_paint);
extern void paint_wifi_connect_status(BOOL is_conn, BOOL is_paint);
extern void on_config_ip_update(ethernet_device_t * p_dev);
extern void on_config_ip_update_wifi(ethernet_device_t * p_dev);
extern void paint_connect_wifi_status(BOOL is_conn, BOOL is_paint);
extern u32 get_3g_status();

extern void set_temp_wifi_info(wifi_info_t temp);
extern wifi_info_t get_temp_wifi_info();
extern u16 get_wifi_select_focus();
extern void paint_list_field_is_connect(u16 index, BOOL is_connect, BOOL is_paint);
extern void paint_wifi_status(BOOL is_conn, BOOL is_paint);
//extern void al_netmedia_register_msg(void);

extern s32 get_music_is_usb();
extern s32 get_picture_is_usb();
extern s32 get_video_is_usb();
extern u16 get_wifi_connecting_focus();
extern void set_3g_connect_status(u32 conn_stats, u32 conn_signal, u8 *operator);
extern void set_gprs_connect_status(u32 conn_stats);
extern void paint_wifi_list_field_not_connect(control_t *p_list);
static void auto_connect_wifi();
static void close_all_network_menus();
static void close_all_dlna_switch_after_disconnect_lan_or_wifi();
void do_cmd_connect_g3(g3_conn_info_t *p_g3_info);


//static ethernet_device_t* p_wifi_devv = NULL;
//static BOOL time_end_open_menu = FALSE;
//static BOOL is_usb_wifi = FALSE; //add for check plug out device is wifi
static   void *file_live = NULL;
//static   vfs_file_info_t file_info;
static net_conn_stats_t g_net_connt_stats = {FALSE, FALSE, FALSE, FALSE};
static ethernet_cfg_t   wifi_ethcfg = {0}; 
static ethernet_device_t* p_wifi_devv = NULL;
static wifi_info_t p_wifi_info = {{0},{0}};
static net_conn_info_t g_net_conn_info = {0};
static BOOL is_start_ttx = FALSE; //just for satip check

#endif

#ifdef NIT_SETTING
#define ASK_DIALOG_AISAT_W (340)
#define ASK_DIALOG_AISAT_H (160)
#define ASK_DIALOG_AISAT_L ((SCREEN_WIDTH -ASK_DIALOG_AISAT_W)/2)
#define ASK_DIALOG_AISAT_T ((SCREEN_HEIGHT-ASK_DIALOG_AISAT_H) /2)
static rect_t g_del_all_dlg_rc =
{
  ASK_DIALOG_AISAT_L,
  ASK_DIALOG_AISAT_T,
  ASK_DIALOG_AISAT_L + ASK_DIALOG_AISAT_W,
  ASK_DIALOG_AISAT_T + ASK_DIALOG_AISAT_H,
};
#endif
void ui_set_enter_standby(BOOL enter_standby)
{
   g_enter_standby = enter_standby;
}

BOOL ui_get_enter_standby(void)
{
  return g_enter_standby;
}


void ui_sleep_timer_create(void)
{
  u32 auto_sleep = 0;
  u32 tm_out;

  ui_sleep_timer_destory();

  auto_sleep = sys_status_get_auto_sleep();

  if(auto_sleep != 0)
  {
    if(auto_sleep == 1)
    {
      tm_out = 9 * 60000;
    }
    else
    {
      tm_out = (30 * (auto_sleep - 1) - 1) * 60000;
    }
    OS_PRINTF("tm out %d\n", tm_out);


    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT, tm_out, FALSE);
  }
}


void ui_sleep_timer_destory(void)
{
  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT);
}


void ui_sleep_timer_reset(void)
{
  utc_time_t sleep_time;
  u32 tm_out;
  BOOL is_sleep_on;

  sys_status_get_status(BS_SLEEP_LOCK, &is_sleep_on);

  if(is_sleep_on)
  {
    sys_status_get_sleep_time(&sleep_time);

    printf_time(&sleep_time, "sleep timer create");

    tm_out = (((sleep_time.hour * 60) + sleep_time.minute - 1) * 60000);

    fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT, tm_out);
  }
}


void uio_reset_sleep_timer(void)
{
  ui_set_notify_autoclose(TRUE);

  if(g_is_one_min_left)
  {
    close_notify();
    //destory one minute timer.
    fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_POWER_OFF);

    //create sleep timer by sleep time.
    ui_sleep_timer_create();
  }
  else
  {
    ui_sleep_timer_reset();
  }

  g_is_one_min_left = FALSE;
}


#endif


#if ENABLE_TTX_SUBTITLE
void start_ttx(void)
{
  gdi_set_enable(FALSE);

  manage_enable_autoswitch(FALSE);
  fw_destroy_all_mainwin(TRUE);
  manage_enable_autoswitch(TRUE);

  ui_config_ttx_osd();

  gdi_set_enable(TRUE);

  vbi_set_font_size_vsb(TTX_FONT_HD);
  ui_set_ttx_display(TRUE, (u32)gdi_get_screen_handle(FALSE), -1, -1);
}


static void end_ttx(void)
{
  gdi_set_enable(FALSE);

  manage_enable_autoswitch(TRUE);

  // stop ttx
  ui_set_ttx_display(FALSE, 0, -1, -1);

  gdi_clear_screen();

  ui_config_normal_osd();
  gdi_set_enable(TRUE);
}


#endif

static void enter_standby(u32 tm_out)
{
  cmd_t cmd;

  cmd.id = AP_FRM_CMD_STANDBY;
  cmd.data1 = tm_out;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  ui_set_enter_standby(TRUE);
}

#if 0
static void reset_screen(BOOL is_ntsc)
{
  rect_t orc;
  s16 x, y;
  u8 prv_root_id = ui_get_preview_menu();

  gdi_set_enable(FALSE);

  if(prv_root_id != ROOT_ID_INVALID)
  {
    if(manage_find_preview(prv_root_id, &orc) != INVALID_IDX)
    {
      avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                           (u16)orc.left, (u16)orc.top, (u16)orc.right, (u16)orc.bottom);
    }
  }

  x = is_ntsc ? SCREEN_POS_NTSC_L : SCREEN_POS_PAL_L;
  y = is_ntsc ? SCREEN_POS_NTSC_T : SCREEN_POS_PAL_T;

  gdi_move_screen(x, y);
  gdi_set_enable(TRUE);

  //just for warriors.
  avc_update_region(class_get_handle_by_id(AVC_CLASS_ID),
                    (void *)gdi_get_screen_handle(FALSE), NULL);
}
#endif

static BOOL menu_open_conflict(control_t *p_curn, u8 new_root)
{
  u8 ctrl_id = (u8)ctrl_get_ctrl_id(p_curn);

  if(fw_find_root_by_id(new_root) != NULL)   //this menu has been opened
  {
    return FALSE;
  }

  if(ctrl_id == ROOT_ID_BACKGROUND)
  {
    return TRUE;
  }

  if(ui_is_fullscreen_menu((u8)ctrl_get_ctrl_id(p_curn)))
  {
    if(ctrl_process_msg(p_curn, MSG_EXIT, 0, 0) != SUCCESS)
    {
      fw_destroy_mainwin_by_root(p_curn);
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


#ifdef SLEEP_TIMER
static RET_CODE on_sleep_tmrout(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
  utc_time_t curn_time, one_minute = {0};
  utc_time_t tmp_s, tmp_e;
  osd_set_t osd_set = {0};

  comm_dlg_data_t sleep_notify = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_WITHTEXT_H,
    IDS_BOOKPG_END,
    10000,
    0,
  };

  dlg_ret_t dlg_ret = DLG_RET_NULL;

  book_pg_t tmp_book;
  BOOL ret = FALSE;
  u8 i;

  OS_PRINTF("sleep time out\n");


  //Destory sleep timer.
  ui_sleep_timer_destory();

  //To Check if any booked pg is playing.
  time_get(&curn_time, FALSE);
  one_minute.minute = 1;
  time_add(&curn_time, &one_minute);

  for(i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &tmp_book);

    memcpy(&tmp_s, &(tmp_book.start_time), sizeof(utc_time_t));
    memcpy(&tmp_e, &(tmp_book.start_time), sizeof(utc_time_t));
    time_add(&tmp_e, &(tmp_book.drt_time));

    if((time_cmp(&curn_time, &tmp_s, FALSE) >= 0)
      && (time_cmp(&curn_time, &tmp_e, FALSE) < 0)
      && tmp_book.pgid != INVALIDID)
    {
      printf_time(&tmp_s, "tmp start");
      printf_time(&tmp_e, "tmp end");
      OS_PRINTF("pgid %d, i %d\n", tmp_book.pgid, i);
      ret = TRUE;
    }
  }

  OS_PRINTF("ret %d\n", ret);

  //No booked pg is playing.
  if(!ret)
  {
    g_is_one_min_left = TRUE;

    ui_set_notify_autoclose(FALSE);

    //Create a one minute timer for power off.
    dlg_ret = ui_comm_dlg_open2(&sleep_notify);
    if(dlg_ret == DLG_RET_YES)
    {
      fw_tmr_create(ROOT_ID_BACKGROUND, MSG_POWER_OFF, 1000, FALSE);
    }
    else if(dlg_ret == DLG_RET_NO)
    {
      sys_status_set_auto_sleep(0);
      open_sleep_hotkey(0, 0);
      sys_status_get_osd_set(&osd_set);
      fw_tmr_create(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT, osd_set.timeout * 1000, FALSE);
      return SUCCESS;
    }
  }

  return SUCCESS;
}
#endif

static RET_CODE on_timer_open(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  u8 focus_id = fw_get_focus_id();
  if(focus_id == ROOT_ID_PROG_BAR)
  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  manage_open_menu(ROOT_ID_BOOK_LIST, 0, FROM_TIMER_SETMENU);
  return SUCCESS;
}
#ifdef ENABLE_PVR_REC_CONFIG
static RET_CODE on_open_record_mannger(control_t *p_ctrl, u16 msg,
                         u32 para1, u32 para2)
{
  u8 focus_id = fw_get_focus_id();

  if(!(ui_get_usb_status()))
  {
    return ERR_FAILURE;
  }

  if((focus_id != ROOT_ID_BACKGROUND) && (focus_id != ROOT_ID_PROG_BAR))
  {
    return ERR_FAILURE;
  }

  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);

  ui_stop_play(STOP_PLAY_BLACK, TRUE);

  manage_open_menu(ROOT_ID_RECORD_MANAGER, 0, 1);

  return SUCCESS;
}
#endif
static RET_CODE on_start_record(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
#ifdef ENABLE_PVR_REC_CONFIG
  u8 focus_id = fw_get_focus_id();
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

  if(ui_recorder_isrecording())
  {
    return SUCCESS;
  }

  if(!ui_get_usb_status() && ui_is_fullscreen_menu(focus_id))
  {
    ui_comm_cfmdlg_open(NULL, IDS_USB_DISCONNECT, NULL, 2000);
    return SUCCESS;
  }

  if(prog_id == INVALIDID)
  {
    return ERR_FAILURE;
  }

  if(!ui_is_pass_chkpwd(prog_id))
  {
    ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
    return ERR_FAILURE;
  }

  if((focus_id != ROOT_ID_BACKGROUND) && (focus_id != ROOT_ID_PROG_BAR))
     return SUCCESS;

  if(!ui_signal_is_lock() || !ui_is_playing()
    || ui_is_playpg_scrambled() || (sys_status_get_curn_prog_mode() != CURN_MODE_TV))
  {
    ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
    return SUCCESS;
  }

  {
    partition_t *p_partition = NULL;
    if (file_list_get_partition(&p_partition) > 0)
    {
      // start record ,rec free size should > 100M
      OS_PRINTF("free size=%ld\n",p_partition->free_size);
      if(p_partition[sys_status_get_usb_work_partition()].free_size/KBYTES < 100)
      {
        ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 2000);
      }
      else
      {
        manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
        if(ui_is_pause())
          ui_set_pause(FALSE);
        manage_open_menu(ROOT_ID_PVR_REC_BAR, para1, para2);
      }
    }
  }
#endif
  return SUCCESS;
}


static RET_CODE on_power_off(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u8 index = MAX_BOOK_PG;
  u32 tm_out = 0;
  book_pg_t node;
  utc_time_t loc_time = {0};
#ifdef ENABLE_ADS
#ifndef TENGRUI_ADS
  BOOL result = FALSE;
#endif
#endif

  OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
  switch(fw_get_focus_id())
  {
    case ROOT_ID_DO_SEARCH:
      /* ask for enter standby on prog scan */
      if(!do_search_is_finish())
      {
        comm_dlg_data_t dlg_data = {0};
        dlg_ret_t ret;

        ui_pause_scan();

        // opend dlg
        dlg_data.x = COMM_DLG_X, dlg_data.y = COMM_DLG_Y;
        dlg_data.w = COMM_DLG_W, dlg_data.h = COMM_DLG_H;
        dlg_data.style = DLG_FOR_ASK | DLG_STR_MODE_STATIC;
        dlg_data.content = IDS_MSG_ASK_FOR_STANDBY;

        ret = ui_comm_dlg_open(&dlg_data);

        if(ret == DLG_RET_NO)
        {
          ui_resume_scan();
          return SUCCESS;
        }
        else
        {
          ui_stop_scan();
        }

        gdi_set_enable(FALSE);
      }
      break;
    case ROOT_ID_UPGRADE_BY_RS232:
      if(ui_is_rs232_upgrading())
      {
        return SUCCESS;
      }
      break;
    case ROOT_ID_POPUP:
      if(UI_USB_DUMP_IDEL != ui_usb_dumping_sts())
      {
        OS_PRINTF("[USB] ui release dump\r\n");
        ui_release_dump();
      }

      if (UI_USB_UPG_IDEL != ui_usb_upgade_sts())
      {
        OS_PRINTF("[USB] ui release upg\r\n");
        ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);
      }

      if(!do_search_is_stop())
      {
        ui_stop_scan();
      }

      if(fw_find_root_by_id(ROOT_ID_PVR_REC_BAR))
      {
        fw_destroy_all_mainwin(FALSE);
        mtos_task_delay_ms(100);
      }
      break;
    case ROOT_ID_PVR_REC_BAR:
      {
        fw_destroy_all_mainwin(FALSE);
        mtos_task_delay_ms(100);
        break;
      }

    default:
      break;
  }

  /* pre standby */
  gdi_set_enable(FALSE);

  /* change keymap */
  fw_set_keymap(ui_desktop_keymap_on_standby);

  /* close all menu */
  manage_enable_autoswitch(FALSE);
  fw_destroy_all_mainwin(TRUE);
  manage_enable_autoswitch(TRUE);

#if ENABLE_TTX_SUBTITLE
  /*close ttx*/
  end_ttx();
#endif

  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  ui_release_signal();

  /* clear history */
  ui_clear_play_history();
  ui_enable_playback(FALSE);

#ifdef WIN32
  //to get wake up time.
  index = book_get_latest_index();
  if(index != MAX_BOOK_PG)
  {
    book_get_book_node(index, &node);

    time_get(&loc_time, FALSE);
    if(time_cmp(&loc_time, &(node.start_time), FALSE) >= 0)
    {
      book_overdue_node_handler(index);
      index = book_get_latest_index();
      if(index != MAX_BOOK_PG)
      {
        book_get_book_node(index, &node);

        time_get(&loc_time, FALSE);
        loc_time.second = 0;
        OS_PRINTF("index %d\n", index);
        printf_time(&loc_time, "loc time");
        printf_time(&(node.start_time), "start time");
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
      }
    }
    else
    {
      time_get(&loc_time, FALSE);
      loc_time.second = 0;
      OS_PRINTF("index %d\n", index);
      printf_time(&loc_time, "loc time");
      printf_time(&(node.start_time), "start time");
      tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
    }
  }
#else
  /* do nothing but send cmd to ap to enter standby */

  //to get wake up time.
  index = book_get_latest_index();
  if(index != MAX_BOOK_PG)
  {
    book_get_book_node(index, &node);

    time_get(&loc_time, FALSE);

    if(time_cmp(&loc_time, &(node.start_time), FALSE) >= 0)
    {
      book_overdue_node_handler(index);
      index = book_get_latest_index();
      if(index != MAX_BOOK_PG)
      {
        book_get_book_node(index, &node);

        time_get(&loc_time, FALSE);
        loc_time.second = 0;
        printf_time(&loc_time, "loc time");
        printf_time(&(node.start_time), "start time");
        if(node.record_enable)
        {
         if((time_cmp(&(node.start_time), &loc_time, 0)) > 0)
         tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
         if(tm_out > RECORD_WAKE_UP_TIME)
         tm_out -= RECORD_WAKE_UP_TIME;
	  else if(tm_out > RECORD_TIME_OUT_MS)
	  tm_out -= RECORD_TIME_OUT_MS;
	 }
	 else
    {
        if((time_cmp(&(node.start_time), &loc_time, 0)) > 0)
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
     }
     }
    }
    else
    {
      time_get(&loc_time, FALSE);
      loc_time.second = 0;
      printf_time(&loc_time, "loc time");
      printf_time(&(node.start_time), "start time");
      if(node.record_enable)
      {
        if((time_cmp(&(node.start_time), &loc_time, 0)) > 0)
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
        if(tm_out > RECORD_WAKE_UP_TIME)
        tm_out -= RECORD_WAKE_UP_TIME;
        sys_status_set_standby_time_out(tm_out);
      }
      else
      {
        if((time_cmp(&(node.start_time), &loc_time, 0)) > 0)
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
      }
    }
  }
  else
  {
    sys_status_set_standby_time_out(0);
  }

#ifdef ENABLE_ADS
#ifndef TENGRUI_ADS
  result = ui_adv_pic_play(ADS_AD_POWER_OFF,ROOT_ID_BACKGROUND);
  if (result)
  {
    mtos_task_sleep(3000);
  }
#endif
#endif

  /* enter standby */
  OS_PRINTF("on power off time out : %d\n", tm_out);
  enter_standby(tm_out);
#endif
  return SUCCESS;
}


static RET_CODE on_power_on(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
#if ((!defined WIN32))  /* jump to start address */
  hal_pm_reset();
#else
  fw_set_keymap(ui_desktop_keymap_on_normal);
  ui_init_signal();

  manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);

  if(ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
  if(ui_is_pause())
  {
    ui_set_pause(FALSE);
  }

  ui_play_curn_pg();

  mtos_task_delay_ms(1 * SECOND);
  gdi_set_enable(TRUE);
#endif
  /* set front panel */

  return SUCCESS;
}


static void update_fp_on_scart_switch(BOOL input_is_tv, BOOL is_detected)
{
  if(input_is_tv)
  {
    ui_set_front_panel_by_str(" tu ");
  }
  else
  {
    if(is_detected)
    {
      ui_set_front_panel_by_str("VCR");
    }
    else
    {
      // set frontpanel
      ui_set_frontpanel_by_curn_pg();
    }
  }
}



static RET_CODE on_scart_switch(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
  static control_t *p_orig = NULL;
  static control_t *p_root = NULL;

  BOOL is_detected = avc_detect_scart_vcr_1(class_get_handle_by_id(AVC_CLASS_ID));

  g_vcr_input_is_tv = !g_vcr_input_is_tv;

  if(!is_detected)
  {
    avc_cfg_scart_select_tv_master_1(class_get_handle_by_id(AVC_CLASS_ID),
                                     g_vcr_input_is_tv ? SCART_TERM_VCR : SCART_TERM_STB);
  }

  avc_cfg_scart_vcr_input_1(class_get_handle_by_id(AVC_CLASS_ID),
                            g_vcr_input_is_tv ? SCART_TERM_TV : SCART_TERM_STB);
  update_fp_on_scart_switch(g_vcr_input_is_tv, is_detected);

  p_root = fw_get_root();
  if(g_vcr_input_is_tv)
  {
    OS_PRINTF("vcr keymap mode\n");
    p_orig = fw_get_focus();
    if(p_root != p_orig)
    {
      manage_enable_autoswitch(FALSE);
      fw_set_focus(p_root);
    }
    fw_set_keymap(ui_desktop_keymap_on_vcr);
  }
  else
  {
    OS_PRINTF("normal keymap mode\n");
    if(p_root != p_orig)
    {
      fw_set_focus(p_orig);
      manage_enable_autoswitch(TRUE);
    }
    fw_set_keymap(ui_desktop_keymap_on_normal);
  }

  return SUCCESS;
}


static RET_CODE on_scart_vcr_detected(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2)
{
  BOOL is_detected = (BOOL)(para1);
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  /* video adc */
  void *display_dev = NULL;

  display_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

  disp_cvbs_onoff(display_dev, (cvbs_dacgrp_t)1, (BOOL)!is_detected);
  disp_component_onoff(display_dev, (component_dacgrp_t)1, (BOOL)!is_detected);

  if(is_detected)
  {
    avc_cfg_scart_select_tv_master_1(avc_handle, SCART_TERM_VCR);
    avc_cfg_scart_vcr_input_1(avc_handle, SCART_TERM_TV);
    // force to CVBS mode
    avc_cfg_scart_format_1(avc_handle, SCART_VID_CVBS);
    fw_set_keymap(ui_desktop_keymap_on_vcr);
  }
  else
  {
    av_set_t av_set;
    sys_status_get_av_set(&av_set);
    avc_cfg_scart_select_tv_master_1(avc_handle, SCART_TERM_STB);
    avc_cfg_scart_vcr_input_1(avc_handle, SCART_TERM_STB);

    // restore by setting
    avc_cfg_scart_format_1(avc_handle,
                           sys_status_get_scart_out_cfg(av_set.video_output));
    fw_set_keymap(ui_desktop_keymap_on_normal);
  }

  update_fp_on_scart_switch(FALSE, is_detected);
  return SUCCESS;
}

static RET_CODE on_switch_pn(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  av_set_t av_set = {0};
  u8 org_mode;
  u8 mode_str[5][8] = {"PAL", "NTSC", "Auto"};
  rect_t pn_notify =
  {
    NOTIFY_CONT_X, NOTIFY_CONT_Y,
    NOTIFY_CONT_X + NOTIFY_CONT_W,
    NOTIFY_CONT_Y + NOTIFY_CONT_H,
  };

  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    sys_status_get_av_set(&av_set);
    org_mode = av_set.tv_mode;

    av_set.tv_mode++;

    if(av_set.tv_mode > AVC_VIDEO_MODE_PAL_1)
    {
      av_set.tv_mode = AVC_VIDEO_MODE_AUTO_1;
    }

    if(av_set.tv_mode != org_mode)
    {
      ui_reset_tvmode(av_set.tv_mode);
    }

    avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));

    ui_set_notify(&pn_notify, NOTIFY_TYPE_ASC, (u32)mode_str[av_set.tv_mode], NOTIFY_AUTOCLOSE_3000MS);

    sys_status_set_av_set(&av_set);
    sys_status_save();
  }
  return SUCCESS;
//lint -save -e550
}
//lint -restore


static RET_CODE on_sleep(control_t *p_ctrl, u16 msg,
                         u32 para1, u32 para2)
{
  u32 auto_sleep = 0;
  manage_close_menu(ROOT_ID_PROG_DETAIL, 0, 0);
  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    auto_sleep = sys_status_get_auto_sleep();

    MT_ASSERT(auto_sleep < SLEEP_CNT);

    if(fw_find_root_by_id(ROOT_ID_SLEEP_HOTKEY) != NULL)
    {
      auto_sleep++;

      if(auto_sleep >= SLEEP_CNT)
      {
        auto_sleep = 0;
      }

      sys_status_set_auto_sleep(auto_sleep);
      sys_status_save();
    }

    sleep_hotkey_set_content((u8)auto_sleep);

    ui_sleep_timer_create();
  }
  return SUCCESS;
}


static RET_CODE on_switch_video_mode(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  disp_sys_t video_std = VID_SYS_AUTO;
  av_set_t av_set;
  static u8 *video_resolution_hd_str_50hz[] = {(u8 *)"576i",(u8 *) "576p", (u8 *)"720p", (u8 *)"1080i", (u8 *)"1080p"};
  static u8 *video_resolution_hd_str_60hz[] = {(u8 *)"480i", (u8 *)"480p", (u8 *)"720p", (u8 *)"1080i", (u8 *)"1080p"};
  u32 content = 0;
  rect_t pn_notify =
  {
    NOTIFY_CONT_X, NOTIFY_CONT_Y,
    NOTIFY_CONT_X + NOTIFY_CONT_W,
    NOTIFY_CONT_Y + NOTIFY_CONT_H,
  };

  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    sys_status_get_av_set(&av_set);

    if(fw_find_root_by_id(ROOT_ID_NOTIFY) != NULL)
    {
      av_set.tv_resolution++;
    }

    avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);

    video_std = avc_get_video_mode_1(avc_handle);

    switch(video_std)
    {
      case VID_SYS_NTSC_J:
      case VID_SYS_NTSC_M:
      case VID_SYS_NTSC_443:
      case VID_SYS_PAL_M:
        av_set.tv_resolution %= (sizeof(video_resolution_hd_str_60hz) / sizeof(u8 *));

        content = (u32)video_resolution_hd_str_60hz[av_set.tv_resolution];
        break;

      case VID_SYS_PAL:
      case VID_SYS_PAL_N:
      case VID_SYS_PAL_NC:
        av_set.tv_resolution %= (sizeof(video_resolution_hd_str_50hz) / sizeof(u8 *));

        content = (u32)video_resolution_hd_str_50hz[av_set.tv_resolution];
        break;

      default:
        MT_ASSERT(0);
        break;
    }

    avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));

    ui_set_notify(&pn_notify, NOTIFY_TYPE_ASC, content, NOTIFY_AUTOCLOSE_3000MS);

    sys_status_set_av_set(&av_set);
    sys_status_save();
  }
  return SUCCESS;
}

static RET_CODE on_switch_aspect_mode(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  av_set_t av_set;
  static u16 aspect_mode_str[]  = {IDS_AUTO, IDS_43LETTERBOX, IDS_43PANSCAN, IDS_169};
  u32 content = 0;
  rect_t aspect_notify =
  {
    NOTIFY_CONT_X, NOTIFY_CONT_Y,
    NOTIFY_CONT_X + NOTIFY_CONT_W,
    NOTIFY_CONT_Y + NOTIFY_CONT_H,
  };

  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    sys_status_get_av_set(&av_set);

    if(fw_find_root_by_id(ROOT_ID_NOTIFY) != NULL)
    {
      av_set.tv_ratio++;
    }
    
    av_set.tv_ratio %= (sizeof(aspect_mode_str) / sizeof(u16));
    content = (u32)aspect_mode_str[av_set.tv_ratio];

    ui_reset_video_aspect_mode(sys_status_get_video_aspect(av_set.tv_ratio));
    avc_cfg_scart_aspect_1(avc_handle, sys_status_get_scart_aspect(av_set.tv_ratio));


    ui_set_notify(&aspect_notify, NOTIFY_TYPE_STRID, content, NOTIFY_AUTOCLOSE_3000MS);

    sys_status_set_av_set(&av_set);
    sys_status_save();
  }
  return SUCCESS;
}

static RET_CODE on_switch_language(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
  language_set_t lang_set;
  static u16 language_str[]  = {IDS_LANGUAGE_ENGLISH,IDS_LANGUAGE_SIMPLIFIED_CHINESE};
  u32 content = 0;
  rect_t lang_notify =
  {
    NOTIFY_CONT_X, NOTIFY_CONT_Y,
    NOTIFY_CONT_X + NOTIFY_CONT_W,
    NOTIFY_CONT_Y + NOTIFY_CONT_H,
  };

  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    sys_status_get_lang_set(&lang_set);


    if(fw_find_root_by_id(ROOT_ID_NOTIFY) != NULL)
    {
      lang_set.osd_text++;
    }
    
    lang_set.osd_text %= (sizeof(language_str) / sizeof(u16));
    content = (u32)language_str[lang_set.osd_text];
    
    rsc_set_curn_language(gui_get_rsc_handle(), lang_set.osd_text + 1);


    ui_set_notify(&lang_notify, NOTIFY_TYPE_STRID, content, NOTIFY_AUTOCLOSE_3000MS);

    sys_status_set_lang_set(&lang_set);
    sys_status_save();
  }
  return SUCCESS;
}

#if ENABLE_TTX_SUBTITLE
static RET_CODE on_switch_subt(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
#ifdef ENABLE_PVR_REC_CONFIG
#if ENABLE_MUSIC_PICTURE
  if(ui_recorder_isrecording())
    return SUCCESS;
#endif
#endif
    manage_close_menu(ROOT_ID_PROG_DETAIL, 0, 0);
    manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
    //manage_close_menu(ROOT_ID_VOLUME, 0, 0);
	if(fw_get_focus_id() != ROOT_ID_BACKGROUND)
	{
		return ERR_FAILURE;
	}
	manage_open_menu(ROOT_ID_SUBT_LANGUAGE,
		0, 0);

	return SUCCESS;
}
#endif

#ifdef ENABLE_CA
static RET_CODE on_update_descramble(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
    static u32 start_ticks = 0;
    UI_PRINTF("on_update_descramble msg:%d\n", msg);
    ui_set_playpg_scrambled((BOOL)(msg == MSG_DESCRAMBLE_FAILED));
    if( CAS_ID == CONFIG_CAS_ID_DMT)
    {
        if(msg == MSG_DESCRAMBLE_SUCCESS)
        {  
            if((mtos_ticks_get() - start_ticks) * 10 > 10 * 1000 && ui_is_smart_card_insert())
            {
                update_ca_message(RSC_INVALID_ID);    
                start_ticks = mtos_ticks_get();   
            }
        }
    }
    return SUCCESS;
}
#endif

static RET_CODE on_mute(control_t *p_ctrl, u16 msg,
                        u32 para1, u32 para2)
{
  u8 index,curn_mode;
#ifdef ENABLE_TIMESHIFT_CONFIG
  BOOL time_shift_start = FALSE;
#endif
  mem_user_dbg_info_t info;

  mtos_mem_user_debug(&info);
  APPLOGI("memory cost alloced 0x%x , peak 0x%x \n",info.alloced,info.alloced_peak);

#ifdef ENABLE_TIMESHIFT_CONFIG
  time_shift_start = ui_timeshift_switch_get();
  if(time_shift_start)
  {
    fw_notify_root(fw_find_root_by_id(ROOT_ID_TIMESHIFT_BAR), NOTIFY_T_MSG,
                       FALSE, msg, para1, para2);
  }
  else
#endif
  {
    //ui_release_ca();
    index = fw_get_focus_id();
    curn_mode = sys_status_get_curn_prog_mode();
    if(curn_mode == CURN_MODE_NONE && index != ROOT_ID_USB_FILEPLAY&& index != ROOT_ID_FILEPLAY_BAR && index !=ROOT_ID_USB_MUSIC)
    {
      OS_PRINTF("no mute ---\n ");
        return SUCCESS;
    }
    
    if(index != ROOT_ID_DO_SEARCH
      && do_search_is_finish()
      && !ui_is_rs232_upgrading()
       )
    {
      ui_set_mute((BOOL)!ui_is_mute());
    }
  }
  return SUCCESS;
}


static RET_CODE on_pause(control_t *p_ctrl, u16 msg,
                         u32 para1, u32 para2)
{
  u8 index;
  BOOL time_shift = FALSE;
#ifdef ENABLE_TIMESHIFT_CONFIG
  BOOL time_shift_start = FALSE;
#endif
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

#ifdef ENABLE_TIMESHIFT_CONFIG
  time_shift_start = ui_timeshift_switch_get();
  if(time_shift_start)
  {
     fw_notify_root(fw_find_root_by_id(ROOT_ID_TIMESHIFT_BAR), NOTIFY_T_MSG,
                       FALSE, MSG_TS_PLAY_PAUSE, para1, para2);
  }
  else
#endif
  {
    index = fw_get_focus_id();
    time_shift = sys_status_get_timeshift_switch();
    if((index != ROOT_ID_NUM_PLAY
       && ui_is_fullscreen_menu(index))
      || (index == ROOT_ID_PROG_LIST)
      || (index == ROOT_ID_ZOOM)
      || (index == ROOT_ID_EPG))
    {
      // only respond the key on tv mode
      if(sys_status_get_curn_prog_mode() == CURN_MODE_TV)
      {
          //sys_status_get_status(BS_DVR_ON, &time_shift);
        if((time_shift == TRUE) && ui_is_fullscreen_menu(index) && ui_get_usb_status()&&!ui_is_pause())
        {
        	if(!ui_is_pass_chkpwd(prog_id))
        	{
            ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
            return ERR_FAILURE;
        	}

          //epg & rec buffer shared, close prog bar to avoid time_update when timeshift on
          if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
          {
            fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
          }
          manage_open_menu(ROOT_ID_TIMESHIFT_BAR, 0, 0);
        }
        else
        {
          ui_set_pause((BOOL)!ui_is_pause());
        }
      }
      else if(sys_status_get_curn_prog_mode() == CURN_MODE_RADIO)
      {
        if(time_shift == TRUE)
        {
          ui_comm_cfmdlg_open(NULL, IDS_TIMESHIFT_RADIO_PRO, NULL, 2000);
          return ERR_NOFEATURE;
        }
      }
      else
      {
        return ERR_NOFEATURE;
      }
    }
  }
  return SUCCESS;
}

static RET_CODE on_play(control_t *p_ctrl, u16 msg,
                         u32 para1, u32 para2)
{
  u8 index;
  BOOL time_shift = FALSE;
#ifdef ENABLE_TIMESHIFT_CONFIG
  BOOL time_shift_start = FALSE;

  time_shift_start = ui_timeshift_switch_get();
  if(time_shift_start)
  {
     return ERR_NOFEATURE;
  }
  else
#endif
  {
    index = fw_get_focus_id();
    time_shift = sys_status_get_timeshift_switch();
    if((index != ROOT_ID_NUM_PLAY
       && ui_is_fullscreen_menu(index))
      || (index == ROOT_ID_PROG_LIST)
      || (index == ROOT_ID_ZOOM)
      || (index == ROOT_ID_EPG))
    {
      // only respond the key on tv mode
      if(sys_status_get_curn_prog_mode() == CURN_MODE_TV)
      {
        if((time_shift == TRUE) && ui_is_fullscreen_menu(index))
        {
          return ERR_FAILURE;
        }
        else
        {
          ui_set_pause((BOOL)!ui_is_pause());
        }
      }
      else if(sys_status_get_curn_prog_mode() == CURN_MODE_RADIO)
      {
        return ERR_NOFEATURE;
      }
    }
  }
  return SUCCESS;
}


#if ENABLE_TTX_SUBTITLE
static RET_CODE on_start_ttx(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u8 index;
#ifdef ENABLE_PVR_REC_CONFIG
  if(ui_recorder_isrecording())
    return SUCCESS;
#endif
  index = fw_get_focus_id();
  if(ui_is_fullscreen_menu(index))
  {
    if(ui_is_ttx_data_ready(sys_status_get_curn_group_curn_prog_id()))
    {
      start_ttx();

      fw_set_keymap(ui_desktop_keymap_on_ttx);
      fw_set_proc(ui_desktop_proc_on_ttx);
    }
    else
    {
      ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_TELETEXT, NULL, 2000);
    }
  }

  return SUCCESS;
}


static RET_CODE on_stop_ttx(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  end_ttx();

  // restore keymap
  fw_set_keymap(ui_desktop_keymap_on_normal);
  fw_set_proc(ui_desktop_proc_on_normal);

  // open prog bar
  if(ui_is_mute())
  {
    open_mute(0, 0);
  }

  if(ui_is_pause())
  {
    open_pause(0, 0);
  }

  if(ui_is_notify())
  {
    open_notify(NOTIFY_AUTOCLOSE_3000MS, 0);
  }

  update_signal();
  manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);

  return SUCCESS;
}

static RET_CODE on_stop_ttx_and_rec(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  end_ttx();

  // restore keymap
  fw_set_keymap(ui_desktop_keymap_on_normal);
  fw_set_proc(ui_desktop_proc_on_normal);

  // open prog bar
  if(ui_is_mute())
  {
    open_mute(0, 0);
  }

  if(ui_is_pause())
  {
    open_pause(0, 0);
  }

  if(ui_is_notify())
  {
    open_notify(NOTIFY_AUTOCLOSE_3000MS, 0);
  }

  update_signal();
  //manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
  ctrl_process_msg(p_ctrl, MSG_START_RECORD, 0, 0);
  return SUCCESS;
}

static RET_CODE on_ttx_key(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  ui_post_ttx_key(msg & MSG_DATA_MASK);
  return SUCCESS;
}
#endif

static RET_CODE on_open_in_tab(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  u32 vkey = para1;
  u8 new_root = (u8)(msg & MSG_DATA_MASK);
  comm_dlg_data_t p_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    (SCREEN_WIDTH - COMM_DLG_W) / 2, (SCREEN_HEIGHT - COMM_DLG_H) / 2,
    COMM_DLG_W, COMM_DLG_H,
    IDS_PLEASE_CONNECT_USB,
    3000,
  };

  #ifdef DALIAN_ZHUANGGUANG
  if (new_root == 0)
  {
    if(ui_is_feeding() == TRUE)
		{
      update_ca_message(RSC_INVALID_ID);
      ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED_STOP, 0 ,0);
      ui_set_feeding(FALSE);
      OS_PRINTF("close card feed!\n");
		} 
  }
  #endif

#ifdef ENABLE_TIMESHIFT_CONFIG
  if(ui_timeshift_switch_get())
  {
     return SUCCESS;
  }
#endif
  if(menu_open_conflict(fw_get_focus(), new_root))
  {
    switch(new_root)
    {
    case ROOT_ID_MAINMENU:
    case ROOT_ID_MULTIVIEW:

    case ROOT_ID_FAV_LIST:
    case ROOT_ID_EPG:
    
    break;
    case ROOT_ID_USB_FILEPLAY:
    case ROOT_ID_USB_MUSIC:
      if(!(ui_get_usb_status()))
      {
        ui_comm_dlg_open(&p_data);
        return ERR_NOFEATURE;
      }
    break;
    case ROOT_ID_SMALL_LIST:
    default:
    break;
    }
#ifdef ENABLE_PVR_REC_CONFIG
    if(!ui_recorder_isrecording())
#endif
    {
  #ifdef ENABLE_ADS
      pic_adv_stop();
  #endif
      return manage_open_menu(new_root, vkey, 0);
    }
  }

  return ERR_NOFEATURE;
}


static RET_CODE on_close_menu(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  //OS_PRINTF("------------------------------------ on close menu\n");
  manage_autoclose();
  return SUCCESS;
}

static RET_CODE timer_end_to_play(void)
{
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  comm_dlg_data_t *popup_msg = NULL;
  BOOL p_time_out = FALSE;

  comm_dlg_data_t dlg_data_end = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_WITHTEXT_H,
    IDS_BOOKPG_END,
    60000,
    0,
  };

  comm_dlg_data_t dlg_data_jump_pg = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_WITHTEXT_H,
    IDS_RECALL_PG,
    60000,
    0,
  };

  sys_status_get_status(BS_IS_TIME_OUT_WAKE_UP, &p_time_out);

  if(g_is_book_node_dlg_opened)
  {
    return SUCCESS;
  }
  if(p_time_out)//dlg_data_jump_pg
  {
    //means we need to standby.
    popup_msg = &dlg_data_end;
  }
  else
  {
    //means whether to play curn pg.
    popup_msg = &dlg_data_jump_pg;
  }
  dlg_ret = ui_comm_dlg_open2(popup_msg);

  g_is_book_node_dlg_opened = FALSE;
  switch(dlg_ret)
  {
    case DLG_RET_YES:
      {
        u16 pg_id;
        ui_close_all_mennus();
        ui_recall(TRUE,&pg_id);
      }
      if(p_time_out)//standby.
      {
        fw_tmr_create(ROOT_ID_BACKGROUND, MSG_POWER_OFF, 1000, FALSE);
      }
      break;

    case DLG_RET_NO:
      break;

    default:
      break;
  }

  return SUCCESS;
}

//get below case.
BOOL has_timer_record_pg()
{
  return time_end_open_menu;
}
//when record timer is over, allow the recording process.
//we use this function to know whether has that case.
void set_timer_record_pg(BOOL is_has)
{
  time_end_open_menu = is_has;
}

static void get_book_str(u16 *str, u16 *pg_name, u16 *event_name, u16 max_len)
{
  u16 len;
  //u8 colon_char = ':';
  u16 temp_uni_str[2] = {0x003a, 0};
  //u16 temp_uni_str[1] = {0,};
  len = 0, str[0] = '\0';
  gui_get_string(IDS_BOOKPG_START, str, max_len);

  uni_strcat(str, pg_name, max_len);

  //str_asc2uni(&colon_char, temp_uni_str);
  uni_strcat(str, temp_uni_str, max_len);//add colon
  
  uni_strcat(str, event_name, max_len);
  len = (u16)uni_strlen(str);
  gui_get_string(IDS_BOOKPG_START_1, &str[len], (u16)(max_len - len));

}

//lint -e571
static RET_CODE on_book_prog_check(void)
{
  book_pg_t node;
  u32 tm_out;
  u8 node_state = LATEST_NODE_WAIT;
  u8 latest_index = MAX_BOOK_PG;
  control_t *p_root = NULL;
  //BOOL p_time_out = FALSE;
  u16 pg_id = 0;
  BOOL is_not_close_menu = FALSE;
  BOOL is_under_pvr_state = FALSE;
  utc_time_t  p_time = {0};
  u16 content[64 + 1];
  dvbs_prog_node_t pg = {0,};
  comm_dlg_data_t dlg_data = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_WITHTEXT_H,
    0,
    60000,
    0,
  };

  dlg_ret_t dlg_ret = DLG_RET_NULL;

  book_delete_overdue_node();

  latest_index = book_get_latest_index();
  memset(&node, 0, sizeof(book_pg_t));
  if(latest_index!=MAX_BOOK_PG)
    book_get_book_node(latest_index, &node);
  node_state = book_check_latest_node(latest_index);

  if(ui_get_book_flag())
  {
    if(node_state == LATEST_NODE_START)
    {
       if (ui_get_enter_standby())
      {
      #ifndef WIN32
      hal_pm_reset();
      #endif
      ui_set_enter_standby(FALSE);
      fw_set_keymap(ui_desktop_keymap_on_normal);
      manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
      }
      if(g_is_book_node_dlg_opened)
      {
        return SUCCESS;
      }
      if(node.record_enable)
      {
      #ifdef ENABLE_PVR_REC_CONFIG
        if(ui_recorder_isrecording())
        {
          //is recording, we can't close menu.
          is_not_close_menu = TRUE;
        }
    	#endif
      }

    #ifdef ENABLE_FILE_PLAY
      if (ui_video_c_init_state())
      {
        is_not_close_menu = TRUE;
        is_under_pvr_state = TRUE;
      }
    #endif
    #ifdef ENABLE_TIMESHIFT_CONFIG
      if (ui_timeshift_switch_get())
      {
        is_not_close_menu = TRUE;
        is_under_pvr_state = TRUE;
      }
    #endif

      if(!is_not_close_menu)
      {
        //we should play pg in full sreen, except recording.
        ui_close_all_mennus();
        ui_play_curn_pg();
      }

      /* fix bug57019 */
      db_dvbs_get_pg_by_id( node.pgid, &pg );
      get_book_str(content, pg.name, node.event_name, 64);
      dlg_data.content = (u32)content;
      dlg_ret = ui_comm_dlg_open2(&dlg_data);
      g_is_book_node_dlg_opened = TRUE;
      if(dlg_ret == DLG_RET_YES)
      {
        time_get(&p_time, FALSE);

        if(time_cmp(&node.start_time,&p_time,FALSE) >= 0)
        {
          //recalc continue time.
          time_up(&node.drt_time, time_dec(&node.start_time,&p_time));
        }

        g_is_book_node_dlg_opened = FALSE;
        if(!ui_get_book_flag())
        {
          return SUCCESS;
        }

        tm_out = (node.drt_time.hour * 60 + node.drt_time.minute) * 60 * SECOND;

        if(tm_out == 0)
        {
          book_overdue_node_handler(latest_index);
        }
        book_delete_node(latest_index);

        // close current focus menu such as fullscreen movie playback/movie file list.
        // these menus receive exit all message should quit the corresponding stasus correctly
        // for exmaple exit from movie file list, the movie playback should be also quit correctly.
        if (is_under_pvr_state)
        {
          u8 root_id;
          control_t *root_ctrl;
          
          root_id = fw_get_focus_id();
          root_ctrl = fw_find_root_by_id(root_id);
          if (root_ctrl != NULL)
          {
            fw_notify_root(root_ctrl, NOTIFY_T_MSG, TRUE, MSG_EXIT_ALL, 0, 0);
          }
        }
        ui_close_all_mennus();
        if(node.record_enable)
        {
        #ifdef ENABLE_PVR_REC_CONFIG
          if(ui_recorder_isrecording())
          {
            //ui_close_all_mennus();
            set_timer_record_pg(FALSE);
            //we need to stop this record behavior
          }
      	#endif
          ui_book_play(node.pgid);
          memcpy(&book_node,&node,sizeof(book_pg_t));

          if(tm_out != 0)
          {
            on_start_record(NULL,0,(u32)&(book_node.drt_time),(u32)node.record_enable);
          }
        }
        else
        {
          p_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);

          if(p_root == NULL)
          {
            manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
          }

          ui_book_play(node.pgid);

#if ENABLE_INFO_BAR_V2
          fill_nprog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), node.pgid);
#else
          fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), node.pgid);
#endif
          if(ui_is_mute() && (fw_find_root_by_id(ROOT_ID_MUTE) == NULL))
          {
            open_mute(0, 0);
          }
        }
      }
      else if(dlg_ret == DLG_RET_NO)
      {
        g_is_book_node_dlg_opened = FALSE;
        book_delete_node(latest_index);
        return SUCCESS;
      }
    }
    else if(node_state == LATEST_NODE_END)
    {
      if(g_is_book_node_dlg_opened)
      {
        return SUCCESS;
      }
      book_overdue_node_handler(latest_index);
      latest_index = book_get_latest_index();
      node_state = book_check_latest_node(latest_index);
      if(node_state == LATEST_NODE_START)
      {
        if (ui_get_enter_standby())
        {
#ifndef WIN32
        hal_pm_reset();
#endif
        ui_set_enter_standby(FALSE);
        fw_set_keymap(ui_desktop_keymap_on_normal);
        manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
        }
	#ifdef ENABLE_PVR_REC_CONFIG
        if(!ui_recorder_isrecording())
	#endif
        {
          ui_recall(TRUE, &pg_id);
        }

        /* fix bug57019 */
        db_dvbs_get_pg_by_id( node.pgid, &pg );
        get_book_str(content, pg.name, node.event_name, 64);
        dlg_data.content = (u32)content;
        dlg_ret = ui_comm_dlg_open2(&dlg_data);
        g_is_book_node_dlg_opened = TRUE;

        if(dlg_ret == DLG_RET_YES)
        {
          g_is_book_node_dlg_opened = FALSE;
          book_get_book_node(latest_index, &node);
          tm_out = (node.drt_time.hour * 60 + node.drt_time.minute) * 60 * SECOND;
          if(tm_out == 0)
          {
            book_overdue_node_handler(latest_index);
          }
          #ifdef ENABLE_PVR_REC_CONFIG
          if(ui_recorder_isrecording())
          {
            set_timer_record_pg(FALSE);
            //we need to stop this record behavior
          }
          #endif
          ui_close_all_mennus();
          book_delete_node(latest_index);
          if(node.record_enable)
          {
            ui_book_play(node.pgid);
            memcpy(&book_node,&node,sizeof(book_pg_t));
            if(tm_out != 0)
            {
              on_start_record(NULL,0,(u32)&(book_node.drt_time),(u32)node.record_enable);
            }
          }
          else
          {
              p_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);

              if(p_root == NULL)
              {
                manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
              }
              #if ENABLE_INFO_BAR_V2
              fill_nprog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), node.pgid);
              #else
              fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), node.pgid);
              #endif

              ui_book_play(node.pgid);

              if(ui_is_mute()
                && (fw_find_root_by_id(ROOT_ID_MUTE) == NULL))
              {
                open_mute(0, 0);
              }
    	   }

        }
        else if(dlg_ret == DLG_RET_NO)
        {
          g_is_book_node_dlg_opened = FALSE;
          book_delete_node(latest_index);
          return SUCCESS;
        }
      }
      else
      {
        //view timer or record timer over -- is ture over.
        //if(!node.record_enable || !ui_recorder_isrecording()) -- bug 33158
        {
          timer_end_to_play();
          set_timer_record_pg(FALSE);
        }
        #if 0 //for bug 33158 -- it seem to when timer coming, we must stop the record behavior.
        if(g_is_book_node_dlg_opened)
        {
          return SUCCESS;
        }
        g_is_book_node_dlg_opened = TRUE;

        // if record ,  will not call ui in here ,wii be in pvr record
	 if(!node.record_enable)
    {
      dlg_ret = ui_comm_dlg_open2(&dlg_data2);
      if(dlg_ret == DLG_RET_YES)
      {
        g_is_book_node_dlg_opened = FALSE;
        fw_tmr_create(ROOT_ID_BACKGROUND, MSG_POWER_OFF, 1000, FALSE);
      }
      else if(dlg_ret == DLG_RET_NO)
      {
        g_is_book_node_dlg_opened = FALSE;
        return SUCCESS;
      }
    }
	 else
	 {
	      	 sys_status_get_status(BS_IS_TIME_OUT_WAKE_UP, &p_time_out);
		if(p_time_out)
		{
        	dlg_ret = ui_comm_dlg_open2(&dlg_data2);
        	if(dlg_ret == DLG_RET_YES)
        	{
            	g_is_book_node_dlg_opened = FALSE;
            	fw_tmr_create(ROOT_ID_BACKGROUND, MSG_POWER_OFF, 1000, FALSE);
        	}
        	else if(dlg_ret == DLG_RET_NO)
        	{
        	    g_is_book_node_dlg_opened = FALSE;
        	    return SUCCESS;
        	}
		}
		else
		{
			dlg_ret = ui_comm_dlg_open2(&dlg_data_jump_pg);
			if(dlg_ret == DLG_RET_YES)
			{
    			g_is_book_node_dlg_opened = FALSE;
    			ui_recall(TRUE, &pg_id);
			}
			else if(dlg_ret == DLG_RET_NO)
			{
    			g_is_book_node_dlg_opened = FALSE;
    			return SUCCESS;
			}
		}
	 }
#endif
      }
    }
  }
  return SUCCESS;
}
//lint +e571

static u16 time_update_desktop_times = 0;
static void on_time_update_epg_info(void)
{
    dvbs_prog_node_t pg = {0};
    epg_prog_info_t prog_info = {0};
    utc_time_t g_time_st = {0};
    utc_time_t g_time_end = {0};
    db_dvbs_ret_t ret = DB_DVBS_OK;
    u16 pg_id = 0;

   if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
     time_update_desktop_times ++;
     if(time_update_desktop_times >10)
      {
        time_update_desktop_times = 0;
        time_get(&g_time_st, FALSE);
        memcpy(&g_time_end, &g_time_st, sizeof(utc_time_t));
        time_up(&g_time_end, 24 * 60 * 60);
        pg_id = sys_status_get_curn_group_curn_prog_id();
        if(INVALIDID == pg_id)
        {
          return;
        }
        ret = db_dvbs_get_pg_by_id(pg_id, &pg);
        MT_ASSERT(DB_DVBS_OK == ret);
        prog_info.network_id = (u16)pg.orig_net_id;
        prog_info.ts_id      = (u16)pg.ts_id;
        prog_info.svc_id     = (u16)pg.s_id;
        memcpy(&prog_info.start_time, &g_time_st, sizeof(utc_time_t));
        memcpy(&prog_info.end_time, &g_time_end, sizeof(utc_time_t));
        mul_epg_set_db_info(&prog_info);

      }
  }
  else
  {
    time_update_desktop_times = 0;
  }
  return;
}

static RET_CODE on_time_update(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
#if g_rec_buffer_addr == g_epg_buffer_addr
  BOOL is_under_pvr_state = FALSE;
  // if we reuse epg buffer for rec, then if we are under pvr status, we do not update EPG.
  #ifdef ENABLE_PVR_REC_CONFIG
    if(ui_recorder_isrecording())
    {
      //is recording, we can't close menu.
      is_under_pvr_state = TRUE;
    }
  #endif
  #ifdef ENABLE_FILE_PLAY
    if (ui_video_c_init_state())
    {
      is_under_pvr_state = TRUE;
    }
  #endif
  #ifdef ENABLE_TIMESHIFT_CONFIG
    if (ui_timeshift_switch_get())
    {
      is_under_pvr_state = TRUE;
    }
  #endif
    if (!is_under_pvr_state)
    {
      on_time_update_epg_info();
      if(is_tdt_found)
        (void)on_book_prog_check();
    }
#else
  on_time_update_epg_info();
  if(is_tdt_found)
    (void)on_book_prog_check();
#endif
#if 0
 #ifdef ONLY1_CA_VER
  if(is_tdt_found && is_first_find_tdt)
  {
    #ifdef TONGJIU
    //check_super_osd_status();
    #else
    check_super_osd_status();
    #endif
  }
 #endif
#endif
  return SUCCESS;
}


static RET_CODE on_check_signal(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
  ui_signal_check(para1, para2);
  return SUCCESS;
}


void ui_set_usb_status(BOOL b)
{
  g_usb_status = b;
}


BOOL ui_get_usb_status(void)
{
  return g_usb_status;
}


static RET_CODE ui_exit_usb_upg(u8 root_id)
{
  control_t *p_cont = NULL;
  u32 tmp_status = 0;

  switch(root_id)
  {
    case ROOT_ID_UPGRADE_BY_USB:
      tmp_status = ui_usb_upgade_sts();
      if((UI_USB_UPG_IDEL != tmp_status)
        && (UI_USB_UPG_LOAD_END != tmp_status))
      {
        return ERR_FAILURE;
      }
      break;
    case ROOT_ID_DUMP_BY_USB:
      tmp_status = ui_usb_dumping_sts();
      if((UI_USB_DUMP_IDEL != tmp_status)
        && (UI_USB_DUMP_PRE_READ != tmp_status))
      {
        return ERR_FAILURE;
      }
      break;
    default:
      return ERR_FAILURE;
  }

  p_cont = ui_comm_root_get_root(root_id);
  if(NULL != p_cont)
  {
    ctrl_process_msg(p_cont, MSG_UPG_QUIT, 0, 0);
  }
  return SUCCESS;
}


static void ui_jump_to_usbupg_menu(void)
{
  //para2 ==1 for open_upgrade_by_usb highlight on Start menu
  manage_open_menu(ROOT_ID_UPGRADE_BY_USB, 0, 1);
}

static RET_CODE on_usb_dev_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  u32 usb_dev_type = para2;
  u32 bus_speed = (u16)para1;
  u16 stringid = 0;
#ifdef ENABLE_NETWORK
  net_config_t g_net_config = {0,};
  pnp_svc_t * p_this = NULL;
  control_t *root = NULL;
#endif
  
  OS_PRINTF("\n##on_usb_dev_plug_in, dev_type[%d]\n", usb_dev_type);

  OS_PRINTF("on_usb_dev_plug_in, dev_type[%d]\n", usb_dev_type);
  if(!g_deskto_init)
  {
    return SUCCESS;
  }
  if(bus_speed == HP_BUS_FULL_SPEED)
  {
    ui_comm_cfmdlg_open(NULL, IDS_FULL_SPEED_USB_DEVICE_CONNECT, NULL, 2000);
    update_signal();
    return SUCCESS;
  }
  if(bus_speed == HP_BUS_LOW_SPEED)
  {
    ui_comm_cfmdlg_open(NULL, IDS_LOW_SPEED_USB_DEVICE_CONNECT, NULL, 2000);
    update_signal();
    return SUCCESS;
  }

  if(usb_dev_type == HP_STORAGE)
  {
    return SUCCESS;
  }
  else if(usb_dev_type == HP_HUB)
  {
    stringid = IDS_USB_HUB_CONNECT;
  }
#ifdef ENABLE_NETWORK
  else if(usb_dev_type == HP_WIFI)
  {
    p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
    #ifndef WIN32
    p_this->p_wifi_dev = get_wifi_handle();
    #endif
    stringid = IDS_WIFI_PLUG_IN;
    g_net_connt_stats.is_wifi_insert = TRUE;
    sys_status_get_net_config_info(&g_net_config);
    if(g_net_config.link_type == LINK_TYPE_WIFI)
    {
      auto_connect_wifi();
    }

    root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(root)
    {
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
    }
  }
  else if(usb_dev_type == HP_MODEM)
  {
    stringid = IDS_3G_PLUG_IN;  
    if(g_net_connt_stats.is_3g_insert == TRUE)
    {
      return ERR_FAILURE;
    }
    g_net_connt_stats.is_3g_insert = TRUE;
    sys_status_get_net_config_info(&g_net_config);
    if(g_net_config.link_type == LINK_TYPE_3G)
    {        
     fw_tmr_create(ROOT_ID_BACKGROUND, MSG_G3_READY, 1000, TRUE);
    }

    root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(root)
    {
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
    }
  }
#endif
  else
  {
    #ifdef USB_TEST
    OS_PRINTF("on_plug_in: unsupported usb device\n");
    stringid = IDS_UNSUPPORT_USB_DEVICE;
    #else
    update_signal();
    return SUCCESS;
    #endif
  }

  ui_comm_cfmdlg_open(NULL, stringid, NULL, 2000);
  update_signal();
  return SUCCESS;
}

#ifndef DISABLE_PRINT_TO_USB
static void open_print_to_usb(void)
{
  s32 ret = 0;
  utc_time_t time;
  u8 filename[32];
  memset(filename, 0, sizeof(filename)/sizeof(u8));
  mt_dbg_file_printf_enable(10*1024, PRINT_TO_USB_TASK_PRIO, 200);
  OS_PRINTF("usb file printf enable\n");
  time_get(&time, FALSE);
  sprintf((char*)filename, "usb_dump_log_%d_%02d_%02d_%02d_%02d",time.year, time.month, time.day, time.hour, time.minute);

  ret = mt_dbg_file_printf_start(filename,MT_DBG_FLAG_START_OVERWR);
  OS_PRINTF("usb file printf start\n");
  if(ret != SUCCESS)
  {
    mt_dbg_printf(0, "please plug in usb and init fs\n");
    mt_dbg_file_printf_disable();
    mt_dbg_printf(0, "usb file printf disable\n");
    return;
  }
  mt_dbg_serial_force_print(TRUE);
}


static void close_print_to_usb(void)
{
	s32 ret = 0;
	ret = mt_dbg_file_printf_stop(0);
	if(ret != SUCCESS)
	{
		mt_dbg_printf(0, "stop usb file printf failed\n");
		return;
 	}

	ret = mt_dbg_file_printf_disable();
	if(ret != SUCCESS)
	{
		mt_dbg_printf(0, "disable usb file printf failed\n");
		return;
 	}
	OS_PRINTF("usb file printf disable ok\n");
}
#endif

//lint -e438 -e550 -e830
static RET_CODE on_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  BOOL is_find = FALSE;
  rect_t upgrade_dlg_rc =
  {
    ((SCREEN_WIDTH - 400) / 2),
    ((SCREEN_HEIGHT - 200) / 2),
    ((SCREEN_WIDTH - 400) / 2) + 400,
    ((SCREEN_HEIGHT - 200) / 2) + 200,
  };
  u16 stringid = 0;
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;
  u16 partion_content = 0,len=0;
  vfs_dev_info_t dev_info = {0};
  hfile_t file = NULL;
  u16 dst_file[50] = {0};
  u16  tdst_file[50] = {0},letter[3] = {0};
  u8  tmp[5120] = {0};
  int i;
  
#ifndef DISABLE_PRINT_TO_USB
  open_print_to_usb();
#endif
  partition_cnt = vfs_get_partitions(partition_letter, 10);
  OS_PRINTF("partition_cnt = %d\n",partition_cnt);
  partion_content = sys_status_get_usb_work_partition();//default letter in dvr configuration menu
  OS_PRINTF("plug in partion_content = %d\n",partion_content);
  OS_PRINTF("partition_letter[partion_content]= %c\n",partition_letter[partion_content]);
  vfs_get_dev_info(partition_letter[partion_content],&dev_info);
  if(dev_info.fs_type == VFS_NTFS && dev_info.total_size > 0 && dev_info.free_size > 10240)
  {
    memset(tdst_file, 0, sizeof(u16) * 50);
    memset(dst_file, 0, sizeof(u16) * 50);
    memset(letter, 0, sizeof(u16) * 3);
    letter[0]= partition_letter[partion_content];
    uni_strcat(tdst_file, letter, 50);
    str_asc2uni((u8 *)":\\desktop.ini", dst_file);
    uni_strcat(tdst_file, dst_file, 50);
    file = vfs_open(tdst_file,  VFS_NEW);
    for(i=0;i<5120;i++)
      tmp[i]='a';
    if(file)
    {
      OS_PRINTF("plug in write tick = %d\n",mtos_ticks_get());
      len=(u16)vfs_write(tmp,5120,1,file);
      OS_PRINTF("plug in write len = %d,ticks=%d\n",len,mtos_ticks_get());
      vfs_close(file);
      vfs_del(tdst_file);
    }
  }
  ui_set_usb_status(TRUE);

  is_find = ui_find_usb_upg_file();

//not jump to upg when root id is search,can't save data and init is contradicted
  if(is_find && (ROOT_ID_DO_SEARCH != fw_get_focus_id())&&(ROOT_ID_POPUP!= fw_get_focus_id()))
  {
	if(g_deskto_init)
	{
		ui_comm_ask_for_dodlg_open(&upgrade_dlg_rc, IDS_USB_UPG_FILE_EXIST,
		                         ui_jump_to_usbupg_menu,
		                         NULL, 0);
		update_signal();
	}
	else
	{
	    show_ask_dlg = TRUE;
	}

#ifdef ENABLE_TIMESHIFT_CONFIG
	  if(sys_status_get_timeshift_switch())
	  {
	    if(SUCCESS != ui_timeshift_total_size())
	    {
	      OS_PRINTF("@@## usb plug in, timeshift is on, but initial timeshift failed!!\n");
	    }
	  }
#endif
    return SUCCESS;
  }
  
#if ENABLE_CUSTOMER_URL
  if(fw_get_focus_id() != ROOT_ID_LIVE_TV)
  {
     is_find = Iptv_Find_Cus_Url_Files();
  }

  if(is_find)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_IMPORT_CUSTOM_PGS_OR_NOT,
                                                      Iptv_Start_Load_Cus_Urls, Iptv_Cancel_Load_cus_Pg ,0);
  }
#endif
  else
  {
    stringid = IDS_USB_STORAGE_CONNECT;
    ui_comm_cfmdlg_open(NULL, stringid, NULL, 2000);
  }

#ifdef ENABLE_TIMESHIFT_CONFIG
  if(sys_status_get_timeshift_switch())
  {
    if(SUCCESS != ui_timeshift_total_size())
    {
      OS_PRINTF("@@## usb plug in, timeshift is on, but initial timeshift failed!!\n");
    }
  }
#endif

  if(!g_deskto_init)
  {
    return SUCCESS;
  }
  update_signal();
  return SUCCESS;
}
//lint +e438 +e550 +e830

static RET_CODE on_plug_out(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u8 root_id = fw_get_focus_id();
#ifndef DISABLE_PRINT_TO_USB
  close_print_to_usb();
#endif

#ifdef ENABLE_NETWORK
  u32 usb_dev_type = para2;
  net_config_t net_config = {0,};
  control_t *root = NULL;

  sys_status_get_net_config_info(&net_config);
  OS_PRINTF("\n##on_plug_out, usb type is == %d##!\n", usb_dev_type);
  if(usb_dev_type == HP_WIFI)
  {
    root = fw_find_root_by_id(ROOT_ID_KEYBOARD_V2);
    if(root != NULL)
    {
      OS_PRINTF("###wifi device plug out, will close ROOT_ID_KEYBOARD_V2 menu###\n");
      manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }
    root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);
    ui_comm_cfmdlg_open(NULL, IDS_WIFI_PLUG_OUT, NULL, 2000);

    if(root != NULL)
    {
      OS_PRINTF("###wifi device plug out, will close wifi_link_info menu###\n");
      //manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    root = fw_find_root_by_id(ROOT_ID_WIFI);
    if(root != NULL)
    {
      OS_PRINTF("###wifi device plug out, will close wifi_link_info menu###\n");
      // manage_close_menu(ROOT_ID_WIFI, 0, 0);
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }


    //  OS_PRINTF("#########is_usb_wifi is == %d####\n",usb_dev_type);
    g_net_connt_stats.is_wifi_insert = FALSE;
    g_net_connt_stats.is_wifi_conn = FALSE;
    //    is_usb_wifi = FALSE;

    g_net_conn_info.wifi_conn_info = WIFI_DIS_CONNECT;

    root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
    if(root)
    {
      OS_PRINTF("###wifi device plug out, will close network_config_wifi menu###\n");
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }
    root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(root)
    {
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
    }

    if((!g_net_connt_stats.is_eth_conn)&&(!g_net_connt_stats.is_gprs_conn)&&(!g_net_connt_stats.is_3g_conn))
    {
      close_all_network_menus();
      close_all_dlna_switch_after_disconnect_lan_or_wifi();
    }

  }
  else if(usb_dev_type == HP_MODEM)
  {
    fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_G3_READY);

    g_net_connt_stats.is_3g_insert = FALSE;
    g_net_connt_stats.is_3g_conn = FALSE; 

    paint_3g_conn_status(FALSE);

    root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(root)
    {
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
    }

    ui_comm_cfmdlg_open(NULL, IDS_3G_PLUG_OUT, NULL, 2000);       

    if((!g_net_connt_stats.is_eth_conn)&&(!g_net_connt_stats.is_gprs_conn)&&(!g_net_connt_stats.is_wifi_conn))
    {
      close_all_network_menus();
    }  
  }       
  else
#endif
  {
    ui_set_usb_status(FALSE);
     show_ask_dlg = FALSE;

#ifdef ENABLE_PVR_REC_CONFIG
    rec_manager_init_param();
#endif
#ifdef ENABLE_TIMESHIFT_CONFIG
    reset_pvr_para();
#endif

    switch(root_id)
    {
      case ROOT_ID_POPUP:
        if((UI_USB_UPG_IDEL != ui_usb_upgade_sts())
          || (UI_USB_DUMP_IDEL != ui_usb_dumping_sts()))
        {
          manage_close_menu(ROOT_ID_POPUP, 0, 0);
          fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG,
                         FALSE, MSG_PLUG_OUT, 0, 0);
          update_signal();
          return SUCCESS;
        }
        break;
      case ROOT_ID_UPGRADE_BY_USB:
      case ROOT_ID_DUMP_BY_USB:
        ui_exit_usb_upg(root_id);
        break;
      case ROOT_ID_TS_RECORD:
        manage_close_menu(ROOT_ID_TS_RECORD, 0, 0);
        break;
      default:
        break;
    }

    ui_comm_cfmdlg_open(NULL, IDS_USB_DISCONNECT, NULL, 2000);
    update_signal();
  }

  return SUCCESS;
}


void ui_desktop_rhythm(void)
{
  gui_rolling();
}

//lint -e438 -e550 -e830
extern rsc_rstyle_t c_tab[];

static RET_CODE on_desktop_roll_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_desktop_roll_text_t *p_roll_text = (ui_desktop_roll_text_t *)para1;
  u16 *p_uni_str;
  u32 uni_str_size;
  u32 check_index;
  control_t *p_ctrl_roll = NULL;
  roll_param_t param;
  u32 ctrl_bg;
  u32 font_style;
  u32 font_size;
  u32 w;
  u32 h;
  u8 pps;
  static rect_t last_rect = {0,};


  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_roll_text != NULL);
  //lint -save -e613
  uni_str_size = strlen((char *)p_roll_text->text);
  if (uni_str_size == 0)
  {
    OS_PRINTF("[%s][%d] strlen == 0\n",__FUNCTION__,__LINE__);
    return SUCCESS;
  }
  p_uni_str = mtos_malloc((uni_str_size + 1) * sizeof(u16));
  MT_ASSERT(p_uni_str != NULL);
  //lint -save -e668
  memset(p_uni_str, 0, (uni_str_size + 1) * sizeof(u16));
  //lint -restore

  gb2312_to_unicode(p_roll_text->text, (s32)strlen((char *)p_roll_text->text), p_uni_str, (s32)uni_str_size);
  OS_PRINTF("Roll message(size:%d):%s\n", strlen((char *)p_roll_text->text), p_roll_text->text);

  /* use the blank instead of LF code */
  for(check_index = 0; check_index <= uni_str_size; check_index++)
  {
    if(p_uni_str[check_index] == 0x000a)
    {
      p_uni_str[check_index] = 0x0020;
    }
  }

  p_ctrl_roll = ctrl_get_child_by_id(p_ctrl,IDC_DESKTOP_ROLL_TEXT);
  MT_ASSERT(p_ctrl_roll != NULL);

  ctrl_set_attr(p_ctrl_roll, OBJ_ATTR_ACTIVE);
  gui_stop_roll(p_ctrl_roll);

  ctrl_bg = p_roll_text->bg_color;
  font_style = p_roll_text->font_color;
  font_size = p_roll_text->font_size;

  // TODO: Handle font size.

  OS_PRINTF("bg:%d, font:%d loop:%d,fontsize:%d rec:{%d,%d,%d,%d},location:%d,speed:%d\n",
            p_roll_text->bg_color, p_roll_text->font_color,
            p_roll_text->param.repeats,p_roll_text->font_size,
            p_roll_text->rect.left, p_roll_text->rect.top,
            p_roll_text->rect.right, p_roll_text->rect.bottom,
            p_roll_text->location, p_roll_text->speed);
  //ctrl_set_bg(p_ctrl_roll, ctrl_bg);
  text_set_font_style(p_ctrl_roll, font_style, font_style, font_style);
  ctrl_set_rstyle(p_ctrl_roll, ctrl_bg, ctrl_bg, ctrl_bg);
  text_set_content_by_unistr(p_ctrl_roll,p_uni_str);

  param = p_roll_text->param;
  param.use_bg = TRUE;
  MT_ASSERT(ctrl_bg < MAX_RSTYLE_CNT);
  param.bg_color = c_tab[ctrl_bg].bg.value;
  switch (p_roll_text->speed)
  {
    case ROLL_TEXT_SPEED_NORMAL:
      pps = ROLL_PPS;
    break;
    case ROLL_TEXT_SPEED_SLOW_X1:
      pps = (ROLL_PPS + 1) / 2;
    break;
    case ROLL_TEXT_SPEED_SLOW_X2:
      pps = (ROLL_PPS + 3) / 4;
    break;
    case ROLL_TEXT_SPEED_FAST_X1:
      pps = ROLL_PPS * 2;
    break;
    case ROLL_TEXT_SPEED_FAST_X2:
      pps = ROLL_PPS * 4;
    break;
    default:
      pps = ROLL_PPS;
    break;
  }
  if (is_equal_rect(&p_roll_text->rect, &last_rect))
  {
    gui_start_roll(p_ctrl_roll, &param);
    //ctrl_paint_ctrl(p_ctrl_roll,FALSE);
    ctrl_set_sts(p_ctrl_roll,OBJ_STS_SHOW);
  }
  else
  {
    // relocation the roll test arear
    ctrl_set_frame(p_ctrl_roll, &p_roll_text->rect);
    normalize_rect(&p_roll_text->rect);
    w = (u32)((u16)p_roll_text->rect.right - (u16)p_roll_text->rect.left);
    h = (u32)((u16)p_roll_text->rect.bottom - (u16)p_roll_text->rect.top);
    ctrl_set_mrect(p_ctrl_roll, 0, 0, (u16)w, (u16)h);

    gui_start_roll(p_ctrl_roll, &param);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_set_sts(p_ctrl_roll,OBJ_STS_SHOW);
  }

  ui_ca_set_roll_status(TRUE);
  mtos_free(p_uni_str);
  last_rect = p_roll_text->rect;
  //lint -restore
  return SUCCESS;

}
//lint +e438 +e550 +e830

static RET_CODE on_desktop_roll_stop(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl_roll = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_ctrl_roll = ctrl_get_child_by_id(p_ctrl,IDC_DESKTOP_ROLL_TEXT);
  //MT_ASSERT(p_ctrl_roll != NULL);
  if (p_ctrl_roll == NULL)
  {
    return SUCCESS;
  }

  gui_stop_roll(p_ctrl_roll);
  ctrl_set_sts(p_ctrl_roll,OBJ_STS_HIDE);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  //ctrl_add_rect_to_invrgn(p_parent, &frame);
  ui_ca_set_roll_status(FALSE);
  return SUCCESS;
}

static RET_CODE on_desktop_roll_finish(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#ifdef ENABLE_ADS
  u16 pg_id = 0;
  dvbs_prog_node_t pg = {0};
  u16 service_id;

  //OS_PRINTF("[%s]\n",__FUNCTION__);
  if((pg_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
  {
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      service_id = pg.s_id;
      ui_adv_notify(ADS_AD_TYPE_OSD, (void*)&service_id);
    }
  }
#endif
  return SUCCESS;
}

BOOL ui_is_desktop_inited(void)
{
  return g_deskto_init;
}
static RET_CODE background_roll_text_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static RET_CODE open_desktop_rolling_menu(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_txt_top = NULL;
  static BOOL opened = FALSE;

  if (opened)
  {
    return SUCCESS;
  }

  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  MT_ASSERT(p_cont != NULL);
  p_txt_top = ctrl_create_ctrl(CTRL_TEXT, IDC_DESKTOP_ROLL_TEXT,
                           0, 20,
                           SCREEN_WIDTH,40,
                           p_cont, 0);
  MT_ASSERT(p_txt_top != NULL);
  //ctrl_set_proc(p_txt_top, ui_ads_rolling_proc);
  ctrl_set_sts(p_txt_top,OBJ_STS_HIDE);
  ctrl_set_rstyle(p_txt_top, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
  text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);
  ctrl_set_proc(p_txt_top, background_roll_text_proc);

  opened = TRUE;
  return SUCCESS;
}


BOOL is_desktop_rolling_text(void)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl_roll = NULL;

  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_ctrl_roll = ctrl_get_child_by_id(p_cont, IDC_DESKTOP_ROLL_TEXT);

  return ctrl_is_rolling(p_ctrl_roll);
}


void ui_desktop_init(void)
{
  fw_config_t config =
  {
    {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},                  /* background */

    (u32)DST_IDLE_TMOUT,                                       /* timeout of idle */
    RECEIVE_MSG_TMOUT,                                    /* timeout of recieving msg */
    POST_MSG_TMOUT,                                       /* timeout of sending msg */

    ROOT_ID_BACKGROUND,                                   /* background root_id */
    MAX_ROOT_CONT_CNT,
    MAX_MESSAGE_CNT,
    MAX_HOST_CNT,
    MAX_TMR_CNT,

    RSI_TRANSPARENT,
  };

  fw_init(&config,
          ui_desktop_keymap_on_normal,
          ui_desktop_proc_on_normal,
          ui_menu_manage);

  //fw_set_rhythm(ui_desktop_rhythm);

  manage_init();

  // set callback func
  //avc_install_screen_notification_1(class_get_handle_by_id(AVC_CLASS_ID),
                                    //reset_screen);

  fw_register_ap_evtmap(APP_FRAMEWORK, sys_evtmap);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_JUMP);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_DVR_CONFIG);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_STORAGE_FORMAT);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_HDD_INFO);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PIC_PLAY_MODE_SET);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PICTURE);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PVR_REC_BAR);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PVR_PLAY_BAR);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_RECORD_MANAGER);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_PICTURE);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_MUSIC);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_FILEPLAY);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_FILEPLAY_BAR);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_FILEPLAY_SUBT);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_MUSIC_FULLSCREEN);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_MAINMENU);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_TIMESHIFT_BAR);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_FACTORY_TEST);  
  #ifdef ENABLE_NETWORK
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PING_TEST);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG_WIFI);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG_GPRS);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG_3G);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_WIFI);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_WIFI_LINK_INFO);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_ALBUMS);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PHOTO_SHOW);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_EDIT_USR_PWD);
  #endif
  #ifdef  TENGRUI_ROLLTITLE
  {
    cmd_t cmd;
    cmd.id = AP_FRM_CMD_ACTIVATE_APP;
    cmd.data1 = APP_RESERVED2;
    cmd.data2 = 0;
    ap_frm_do_command(APP_FRAMEWORK, &cmd);
  }
  #endif

  open_desktop_rolling_menu(0, 0);
}


void ui_desktop_release(void)
{
  fw_release();
  // set callback func
  avc_install_screen_notification_1(class_get_handle_by_id(AVC_CLASS_ID), NULL);
}


void ui_desktop_main(void)
{
  fw_default_mainwin_loop(ROOT_ID_BACKGROUND);
}


static RET_CODE on_ota_info_find(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  u8 focus_root_id = 0;

  OS_PRINTF("\r\n[OTA]%s", __FUNCTION__);

  focus_root_id = fw_get_focus_id();

  if((ROOT_ID_DO_SEARCH == focus_root_id)
    || (!do_search_is_finish())
    || (ROOT_ID_OTA_SEARCH == focus_root_id)
    || (ROOT_ID_OTA == focus_root_id)
    || (ROOT_ID_DUMP_BY_USB == focus_root_id)
    || (ROOT_ID_PVR_REC_BAR == focus_root_id)
    || (ROOT_ID_USB_MUSIC == focus_root_id))
  {
    OS_PRINTF("\r\n[OTA] desktop don't process, focus_root_id[%d]", focus_root_id);
    return ERR_FAILURE;
  }

  ui_comm_cfmdlg_open(NULL, IDS_MSG_ASK_FOR_UPGRADE, NULL, 10000);

  OS_PRINTF("\r\n[OTA]%s: reboot stb for upgrade! ", __FUNCTION__);
  #ifndef WIN32
  hal_pm_reset();
  #endif
  return SUCCESS;
}

static void close_fast_logo(void)
{
  static BOOL closed = FALSE;

  OS_PRINTF("%s\n",__FUNCTION__);
  if(!closed)
  {
    void  *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
      SYS_DEV_TYPE_DISPLAY);

    disp_layer_show(p_dev, DISP_LAYER_ID_STILL_HD, FALSE);
    closed = TRUE;
  }
}

#ifdef SMSX_CA
extern RET_CODE sv_nit_area_lock(u32 para1, BOOL get_nit_flag);
#endif

static RET_CODE on_heart_beat(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  //if(ui_get_usb_status())
  { 
    //mtos_printk("\ncall on heart beat\n");
     gui_rolling();
    ui_update_roll_timer_cnt();
   // mtos_printk("\nexit on heart beat\n");
	
  }
  return SUCCESS;
}
u8 g_divi_start_finish = 0;
 void ui_desktop_start(void)
{
  u8 type = 0;
  u16 pos = 0;
  u32 context = 0;
  u8 curn_mode = CURN_MODE_NONE, vid = DB_DVBS_INVALID_VIEW;
  u8 curn_type = 3;
  u16 curn_group = GROUP_T_ALL;
  BOOL is_first_play = FALSE;
  BOOL auto_search_bit = FALSE;
#ifdef STARTUP_NIT_VERSION_CHECK
  u32 old_nit_version = 0xff;
  u32 new_nit_version = 0xff;
#endif
#ifdef NIT_SETTING
  u8 nit_setting;
#endif
 u8 ansstr[64] = {0};

  // for singal check
  extern BOOL is_boot_up;
  
 if(CUSTOMER_ID == CUSTOMER_JIULIAN)
 {
   if(sys_get_serial_num(ansstr, sizeof(ansstr)) == FALSE)
   {
     manage_open_menu(ROOT_ID_FACTORY_TEST, 0, 0);
     return;
   }
 }
 
 if(CUSTOMER_ID == CUSTOMER_AISAT || CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
 {
    sys_status_get_status(BS_BOOT_FIRST_PLAY, &is_first_play);
    if(is_first_play == TRUE)
    {
	sys_status_set_curn_group(0);
	sys_status_set_curn_prog_mode( CURN_MODE_TV);
  	sys_status_save();
    }
 }
 if(CUSTOMER_ID == CUSTOMER_AISAT_HOTEL)
 {
	sys_status_set_curn_group(0);
	sys_status_set_curn_prog_mode( CURN_MODE_TV);
  	sys_status_save();
  }
  sys_status_check_group();
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  curn_type = sys_status_get_group_curn_type();

  sys_status_get_group_info(curn_group, &type, &pos, &context);
  if(curn_mode != CURN_MODE_NONE)
  {
    switch(type)
    {
      case GROUP_T_ALL:
        if(0 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV_FTA : DB_DVBS_ALL_RADIO_FTA, context, NULL);
        }
        else if(1 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV_CAS : DB_DVBS_ALL_RADIO_CAS, context, NULL);
        }
        else if(2 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV_HD : DB_DVBS_ALL_RADIO_HD, context, NULL);
        }
        else if(3 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, context, NULL);
        }
      break;
      case GROUP_T_FAV:
        if(0 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV_FTA : DB_DVBS_FAV_RADIO_FTA, context, NULL);
        }
        else if(1 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV_CAS : DB_DVBS_FAV_RADIO_CAS, context, NULL);
        }
        else if(2 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV_HD : DB_DVBS_FAV_RADIO_HD, context, NULL);
        }
        else if(3 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV : DB_DVBS_FAV_RADIO, context, NULL);
        }
      break;
      default:
        MT_ASSERT(0);
    }
    ui_dbase_set_pg_view_id(vid);
  }
#ifdef SLEEP_TIMER
  //create sleep timer.
  ui_sleep_timer_create();
#endif

#ifdef ENABLE_ADS
#ifdef TEMP_SUPPORT_DS_AD
  set_full_scr_ad_status(TRUE);
#endif
#endif

#ifdef ENABLE_CA
  UI_PRINTF("check new email \n");
  //cas_manage_process_menu(CA_CHECK_CA_NEW_EMAIL,0, 0, 0);  
  ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
#endif
#ifdef STARTUP_NIT_VERSION_CHECK
  //nit version check 
  extern u8 get_channel_init_nit_ver(void);  
  new_nit_version = get_channel_init_nit_ver();
  sys_status_get_nit_version(&old_nit_version);
#ifdef NIT_SETTING
  {
     nit_setting = sys_status_get_nit_setting();
     if(nit_setting)
     {	
        if((old_nit_version != (u32)new_nit_version) && (new_nit_version != 0xff)) /**if no nitversion it is 0xff***/
        {
           auto_search_bit = TRUE;
        }
     }	 
  }
 #else
  {	
      if((old_nit_version != (u32)new_nit_version) && (new_nit_version != 0xff)) /**if no nitversion it is 0xff***/
      {
        auto_search_bit = TRUE;
      }
  }
 #endif
#endif
  if(curn_mode == CURN_MODE_NONE)
  {
     auto_search_bit = TRUE;
  }

  if(TRUE == auto_search_bit)
  {
    if(CUSTOMER_ZHUMUDIAN_BY == CUSTOMER_ID || CUSTOMER_JIZHONGMEX == CUSTOMER_ID)
      {
          manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_FULL, 0);
      }
    else if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
      {
        comm_dlg_data_t dlg_ack_data =
        {
          ROOT_ID_INVALID,
          DLG_FOR_ASK | DLG_STR_MODE_STATIC,
          COMM_DLG_X, COMM_DLG_Y,
          COMM_DLG_W, COMM_DLG_H,
          IDS_ASK_AUTOSCAN_NOPRG,
          4000,
        };
	    dlg_ret_t dlg_ret = 0;
        ui_stop_play(STOP_PLAY_BLACK, TRUE);
        dlg_ret = ui_comm_dlg_open2(&dlg_ack_data);
        if(dlg_ret != DLG_RET_NO)
          manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_AUTO, 0);
        else
          manage_auto_switch();
      }
    else
      {
          manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
      }
  }
  else
  {
    if(LATEST_NODE_START != book_check_latest_node(book_get_latest_index()))
    {
      if((CUSTOMER_AISAT == CUSTOMER_ID || CUSTOMER_AISAT_DEMO == CUSTOMER_ID)
	  	&& (vid != DB_DVBS_INVALID_VIEW) )
      {
        if(is_first_play == TRUE)
        {
        	sys_status_set_curn_group_info(db_dvbs_get_id_by_view_pos(vid, 0), 0);
        }
      }
      if(CUSTOMER_AISAT_HOTEL== CUSTOMER_ID && vid != DB_DVBS_INVALID_VIEW)
	  sys_status_set_curn_group_info(db_dvbs_get_id_by_view_pos(vid, 0), 0);
      manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
    }
  }
  is_boot_up = TRUE;
#if 0  
#ifdef ONLY1_CA_VER
  if(CUSTOMER_AISAT == CUSTOMER_ID || CUSTOMER_AISAT_DEMO == CUSTOMER_ID) 
  	check_super_osd_status();
#endif
#endif
#ifdef SMSX_CA
  sv_nit_area_lock(0, 0);
#endif

#ifdef ENABLE_NETWORK
  if (g_net_connt_stats.is_eth_insert)
  {
    comm_dlg_data_t dlg_data =
    {
      0,
      DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
      COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    };
     
    dlg_data.content = IDS_NET_CABLE_PLUG_IN;
    dlg_data.dlg_tmout = 2000;
    ui_comm_dlg_open(&dlg_data);
  }
#endif

  #ifdef DIVI_CA
  OS_PRINTF("[DIVI]get card entitle time!\n");
  ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, 0 ,0);
  g_divi_start_finish = 1;
  #endif

  g_deskto_init = TRUE;
   if(show_ask_dlg)
    {

        rect_t upgrade_dlg_rc =
        {
        ((SCREEN_WIDTH - 400) / 2),
        ((SCREEN_HEIGHT - 200) / 2),
        ((SCREEN_WIDTH - 400) / 2) + 400,
        ((SCREEN_HEIGHT - 200) / 2) + 200,
        };
        if(ROOT_ID_DO_SEARCH != fw_get_focus_id())
        {
            ui_comm_ask_for_dodlg_open(&upgrade_dlg_rc, IDS_USB_UPG_FILE_EXIST,
                               ui_jump_to_usbupg_menu,
                               NULL, 0);
            update_signal();
        }
        show_ask_dlg = FALSE;
    }
}
#ifdef ENABLE_USB_CONFIG
extern void usb_monitor_attach(void);
#endif
 extern void set_uio_key_status(BOOL is_disable);
#ifdef TEMP_SUPPORT_DS_AD
 extern void ui_close_ad_logo(void);
#endif
 static RET_CODE on_ota_timeout_start_play(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  OS_PRINTF("###debug ota timeout,it will play\n");
#ifdef NIT_SOFTWARE_UPDATE
  if(g_nit_ota_check == TRUE)
  {
    comm_dlg_data_t timeout_data =
    {
      ROOT_ID_INVALID,
      DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
      COMM_DLG_X, COMM_DLG_Y,
      COMM_DLG_W, COMM_DLG_H,
      IDS_OTA_TIMEOUT,
      0,
    };
    ui_comm_dlg_open(&timeout_data);
    g_nit_ota_check = FALSE;
  }
#endif
  if(g_deskto_init == TRUE)
  {
    return SUCCESS;
  }

  set_uio_key_status(FALSE);

  ui_enable_playback(TRUE);
/*
  //ui_enable_uio(TRUE);
#ifdef ENABLE_USB_CONFIG
  usb_monitor_attach();
#endif
*/
#ifdef ENABLE_NETWORK
  eth_monitor_attach();
  wifi_monitor_attach();
  g3_monitor_attach();
  //gprs_monitor_attach();
#endif
  fw_set_keymap(ui_desktop_keymap_on_normal);

  /*close logo*/
  close_fast_logo();

#ifdef TEMP_SUPPORT_DS_AD
  ui_close_ad_logo();
#endif

#ifndef MULTI_PIC_ADV
  ui_show_watermark();
#endif

  if(fw_get_focus_id() != ROOT_ID_OTA_SEARCH)
  {
      OS_PRINTF("###debug ota timeout,it will play2\n");
      ui_ota_api_stop();
#ifdef ENABLE_CA
#ifndef WIN32
      if(SUCCESS != cas_manager_ca_area_limit_check_start())
#endif
#endif
      {
        OS_PRINTF("NO area_limit_check !!!\n");
        ui_desktop_start();
      }
    }

  return SUCCESS;
}

static void ui_ota_upg_reset(void)
{
#ifndef WIN32
    OS_PRINTF("###debug ota restar\n");
    ui_ota_api_stop();
    mtos_task_delay_ms(100);
    hal_pm_reset();
#endif
}

#if 0
static void ui_ota_upg_cancel(void)
{
  ota_bl_info_t bl_info = {0};

  mul_ota_dm_api_read_bootload_info(&bl_info);

  OS_PRINTF("\r\n[OTA]%s:upg ota_tri[%d] ",__FUNCTION__, bl_info.ota_status);

  bl_info.ota_status = OTA_TRI_MODE_NONE;
  mul_ota_dm_api_save_bootload_info(&bl_info);

  ui_ota_api_auto_check_cancel();
}
#endif
static RET_CODE on_ota_reboot_into_ota(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  OS_PRINTF("###debug ota restart %d\n", fw_get_focus_id());
#ifdef NIT_SOFTWARE_UPDATE
  if(g_nit_ota_check == TRUE)
  {
    #ifdef WIN32
    ui_ota_api_stop();
    OS_PRINTF("OTA check over!please run ota app\n");
    MT_ASSERT(0);
    #else
    ui_ota_api_manual_save_ota_info();
    ui_ota_upg_reset();
    #endif
    g_nit_ota_check = FALSE;
  }
#endif
  if(g_deskto_init == TRUE)
  {
    return SUCCESS;
  }
  ui_enable_uio(TRUE);
  if(fw_get_focus_id() != ROOT_ID_OTA_SEARCH)
  {
    if(TRUE == ui_ota_api_get_maincode_ota_flag())
      {
        ui_ota_api_maincode_ota_start();
      }
    else
      {
          #ifdef WIN32
          ui_ota_api_stop();
          //on_ota_timeout_start_play(p_ctrl,msg,para1,para2);
          OS_PRINTF("OTA check over!please run ota app\n");
          MT_ASSERT(0);
          #else
          ui_ota_upg_reset();
          #endif
      }
  }

  return SUCCESS;
}

static RET_CODE on_ota_in_maincode_finsh(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
   if(fw_get_focus_id() != ROOT_ID_OTA_SEARCH)
  {
    #ifdef WIN32
    on_ota_timeout_start_play(p_ctrl,msg,para1,para2);
    #else
    ui_ota_upg_reset();
    #endif
  }
   return SUCCESS;
}

static RET_CODE on_open_uart_ui(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  if(fw_get_focus_id() == ROOT_ID_MAINMENU)
  {
    manage_open_menu(ROOT_ID_TEST_MENU, 0, 0);
  }
  return SUCCESS;
}

void ui_clean_all_menus_do_nothing(void)
{
    /* close all menu */
    manage_enable_autoswitch(FALSE);
    fw_destroy_all_mainwin(TRUE);
    manage_enable_autoswitch(TRUE);
}

//lint -e438 -e550 -e830
void set_prompt_flag_of_nit_change(BOOL b)
{
  is_already_prompt = b;
}

static RET_CODE on_nit_version_update(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2)
{
  u8 focus_root = 0;
  comm_dlg_data_t dlg_ack_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    IDS_NIT_UPDATE,
    0,
  };

	dlg_ret_t dlg_ret = DLG_RET_NULL;
	u32 pre_nit_ver = 0;
	u32 new_nit_ver = 0;
	new_nit_ver = para2;

  sys_status_get_nit_version(&pre_nit_ver);
#ifdef SMSX_CA
     if(new_nit_ver == pre_nit_ver)
  {
	   sys_bg_data_t *p_bg_data;
	  p_bg_data = sys_bg_data_get();  
	  if((p_bg_data->local_set.longitude == 0) || (p_bg_data->local_set.longitude == 0xffff))
	  	sv_nit_area_lock(para1, 1);
	    return SUCCESS;
	  }
#endif
  focus_root = fw_get_focus_id();
  if(focus_root != ROOT_ID_NVOD && focus_root != ROOT_ID_NVOD_VIDEO)
  {
    if(!is_already_prompt)
    {
      set_prompt_flag_of_nit_change(TRUE);

      if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
      {
        manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
      }

      if(fw_find_root_by_id(ROOT_ID_VOLUME) != NULL)
      {
        manage_close_menu(ROOT_ID_VOLUME, 0, 0);
      }

	    dlg_ret = ui_comm_dlg_open(&dlg_ack_data);

	    if(dlg_ret != DLG_RET_NO)
	    {
        //stop monitor service
        {
          m_svc_cmd_p_t param = {0};

          dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
        }
      if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
        manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_AUTO, 0);
      else if(CUSTOMER_JIZHONGMEX == CUSTOMER_ID)
        manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_FULL, 0);
      else
        manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
	    }
    }
  }

  /*
  if(0)
  {
      return SUCCESS;
  }
  else if(pre_nit_ver == 0xFF)
  {
     sys_status_set_nit_version(new_nit_ver);
    return SUCCESS;
  }
  index = fw_get_focus_id();
  if((ROOT_ID_DO_SEARCH == index)
          ||(ROOT_ID_MANUAL_SEARCH == index)
          ||(! do_search_is_finish()))
  {
        return ERR_FAILURE;
  }
    //gui_get_string(IDS_NIT_UPDATE, content, 35);////////////////IDS_PROG_UPDATE_TO_SEARCH
    //dlg_data.content = (u32)content;

    dlg_ret = ui_comm_dlg_open(&dlg_data);
    if(dlg_ret != DLG_RET_NO)
    {
      index = fw_get_focus_id();
      if(index != ROOT_ID_DO_SEARCH)
      {
        ui_clean_all_menus_do_nothing();
        ui_pre_nit_search_set();
      }
    }
    */
 #ifdef SMSX_CA
   dvbc_lock_t main_tp = {0};
  nc_channel_info_t tp_info = {0};
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  nc_get_tp(nc_handle,0, &tp_info);
   sys_status_get_main_tp1(&main_tp);
   if(main_tp.tp_freq == tp_info.channel_info.frequency )
   {
		sv_nit_area_lock(para1, 1);
   }
#endif
  return SUCCESS;
}
//lint +e438 +e550 +e830

#ifdef NIT_SOFTWARE_UPDATE
extern void ui_ota_api_auto_mode_start(ota_work_t  mode);


void on_desktop_save_ota_info(cas_ota_info_t *ota_info_temp)
{
  memcpy(&update_info, ota_info_temp, sizeof(cas_ota_info_t));
}

//extern void ui_ota_api_manual_save_ota_info(void);
//extern void  ui_ota_api_save_do_ota_tp(nim_info_t *tp_info);
void do_update_software(void)
{
  nim_info_t set_tp;

  set_tp.data_pid = update_info.data_pid;

  set_tp.lockc.tp_freq = update_info.freq;
  set_tp.lockc.tp_sym = update_info.symbol;
  set_tp.lockc.nim_modulate = update_info.qam_mode;
  if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
    set_tp.lock_mode = SYS_DTMB;
  else
    set_tp.lock_mode = SYS_DVBC;

  #if 1
  OS_PRINTF("==============================\n");
  OS_PRINTF("up_info.freq=%d\n", update_info.freq);
  OS_PRINTF("up_info.symbol=%d\n", update_info.symbol);
  OS_PRINTF("up_info.qam_mode=%d\n", update_info.qam_mode);
  OS_PRINTF("up_info.data_pid=%d\n", update_info.data_pid);
  OS_PRINTF("up_info.ota_type=%d\n", update_info.ota_type);
  OS_PRINTF("up_info.Serial_number_start=%s\n", update_info.Serial_number_start);
  OS_PRINTF("up_info.Serial_number_end=%s\n", update_info.Serial_number_end);
  OS_PRINTF("up_info.swVersion=%d\n", update_info.swVersion);
  OS_PRINTF("up_info.hwVersion=%d\n", update_info.hwVersion);
  OS_PRINTF("==============================\n");
  #endif
  g_nit_ota_check = TRUE;
  //ui_ota_api_save_do_ota_tp(&set_tp);
  //ui_ota_api_manual_save_ota_info();
 #ifndef WIN32
   mtos_task_delay_ms(100);
   hal_pm_reset();
 #endif
}
#endif

static RET_CODE on_software_update(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2)
{
#ifdef NIT_SOFTWARE_UPDATE
  UI_PRINTF("on_software_update update is_already_prompt:%d\n", is_already_prompt_ota);

  if(fw_get_focus_id() == ROOT_ID_OTA_SEARCH)
  {
    UI_PRINTF("on_software_update ota searching\n");
    return SUCCESS;
  }
/*
  if(AP_CAS_ID == CAS_ID_ABV)
  {
    dvbc_lock_t abv_upg_info = {0};
    is_already_prompt_ota = FALSE;
    memcpy((void *)&update_info, (void *)para2, sizeof(update_t));
    update_info.qam_mode = update_info.qam_mode + 3;
    update_info.swVersion = update_info.swVersion / 0x10000;

    if(update_info.swVersion <= ui_ota_api_get_upg_check_version())
      return SUCCESS;
    if(update_info.swVersion <= 0x101)
      return SUCCESS;

    abv_upg_info.tp_freq = update_info.freq;
    abv_upg_info.tp_sym = update_info.symbol;
    abv_upg_info.nim_modulate = update_info.qam_mode;
    sys_status_set_upgrade_tp(&abv_upg_info);

  //  update_info.ota_type = UI_OTA_NORMAL;]
  }
  else
  */
  {
	   memcpy((void *)&update_info, (void *)para1, sizeof(update_t));
  }

  UI_PRINTF("@@@@@ s_already_prompt:%d, ota_type:%d\n", is_already_prompt_ota, update_info.ota_type);
  if(!is_already_prompt_ota)
  {
    is_already_prompt_ota = TRUE;
    if(update_info.ota_type == UI_OTA_NORMAL)
    {
      UI_PRINTF("@@@@@ update_info.ota_type == 1\n");
      ui_comm_ask_for_dodlg_open(NULL, IDS_UPDATE_SOFTWARE,
                                   do_update_software, NULL, 0);
    }
    else if(update_info.ota_type == UI_OTA_FORCE)
    {
      update_ca_message(IDS_SOFTWARE_UPGRADE);
      do_update_software();
    }

  }

#endif
  return SUCCESS;
}

RET_CODE on_switch_preview(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t orc = {0};

  u8 idx = manage_find_preview(fw_get_focus_id(), &orc);

  if(idx != INVALID_IDX)
  {
    avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
    avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                        (u16)orc.left, (u16)orc.top, (u16)orc.right, (u16)orc.bottom);
    //avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                         //orc.left, orc.top, orc.right, orc.bottom);
  }

  return SUCCESS;
}

static RET_CODE on_epg_request_table_policy_switch(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
    if(fw_get_focus_id() != ROOT_ID_EPG)
    {
      OS_PRINTF("on_epg_request_table_policy_switch\n");
      ui_epg_stop(); 
      ui_epg_start(EPG_TABLE_SELECTE_SCH_ALL);
    }
    return SUCCESS;
}
static RET_CODE on_tv_radio_function(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  if(ui_is_fullscreen_menu(curn_root_id))
  {
	  if( (sys_status_get_curn_prog_mode() == CURN_MODE_TV)
	     )
	  {
	    return manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_TVRADIO, 0);
	  }
	  else if( (sys_status_get_curn_prog_mode() == CURN_MODE_RADIO)
	    )
	  {
	    return manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_TVRADIO, 0);
	  }
  }

  return SUCCESS;
}

extern void db_previosly_program_for_ae(void);
extern void categories_mem_reset(void);
static RET_CODE on_preset_prog_for_ae_test(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  u16 view_id;
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  //set default
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID,0);
  sys_status_load();

  categories_mem_reset();
  sys_status_check_group();
  sys_status_save();

  // set environment according ss_data
  sys_status_reload_environment_setting(TRUE);
  //sys_static_data_restore_factory();

  //create a new view after load default, and save the new view id.
  view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  ui_dbase_set_pg_view_id((u8)view_id);

  db_previosly_program_for_ae();
  return SUCCESS;
}

static RET_CODE on_load_prog_from_usb(control_t *p_ctrl,  u16 msg,
                                                         u32 para1, u32 para2)
{
  rect_t upgrade_dlg_rc =
  {
    ((SCREEN_WIDTH - 400) / 2),
    ((SCREEN_HEIGHT - 200) / 2),
    ((SCREEN_WIDTH - 400) / 2) + 400,
    ((SCREEN_HEIGHT - 200) / 2) + 200,
  };
  u8 curn_mode = CURN_MODE_NONE;
  
  curn_mode = sys_status_get_curn_prog_mode();
  //if(CURN_MODE_NONE == curn_mode)
  if(ui_get_usb_status() && ui_find_usb_pgdata_file())
  {
    ui_comm_ask_for_dodlg_open(&upgrade_dlg_rc, IDS_IMPORT_USB_PRESET_CHANNEL,
                               ui_dbase_pgdata_usb_to_db,   NULL, 0);
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_IMPORT_USB_PRESET_CHANNEL_FAIL, NULL, 3000);
    update_signal();
  }
  return SUCCESS;
}

#ifdef ENABLE_ADS
static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}
#endif

#ifdef TENGRUI_ROLLTITLE
#include "subt_pic_ware.h"
extern RET_CODE  subt_pic_module_open(u32 m_id);
static RET_CODE on_start_roll_title(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 focus_root_id = fw_get_focus_id();

  OS_PRINTF("on_start_roll_title,focus_root_id=%d\n",focus_root_id);

  if(db_dvbs_get_count(ui_dbase_get_pg_view_id())<=0)
  {
    UI_PRINTF("%s,no program\n",__FUNCTION__);
    return SUCCESS;
  }

  if(focus_root_id != ROOT_ID_PROG_BAR)
  {
    if(ui_is_fullscreen_menu(focus_root_id))
    {
      subt_pic_module_open(SUBT_PIC_ID_ADT_TR);
      extern BOOL ui_set_roll_title_show (BOOL show);
      ui_set_roll_title_show(TRUE);
      extern void ui_trad_rolltitle_pic_region_init();
        ui_trad_rolltitle_pic_region_init();
    }
  }
  return SUCCESS;
}

u32 subt_draw_image_ticks = 0;
s32 subt_pending_msg_cnt = 0;
#endif


#ifdef WIN32
static RET_CODE on_test_1(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{

ui_dbase_pgdata_usb_to_db();
#if 0
  u8 p_char[20] = {0,};
  memcpy(p_char, "on_test_1",20);
  ui_comm_cfmdlg_open_gb(NULL, p_char, NULL, 0);
  #endif
  return SUCCESS;
}
#endif

#ifdef ENABLE_NETWORK

void ui_evt_disable_ir()
{
  cmd_t cmd = {0};
  OS_PRINTF("%s\n", __FUNCTION__);
  cmd.id = AP_UIO_CMD_DISABLE_IR;
  cmd.data1 = 0;
  cmd.data2 = 0;
  ap_frm_do_command(APP_UIO, &cmd);
/*
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);  */
}

void ui_evt_enable_ir()
{
  cmd_t cmd = {0};
  OS_PRINTF("%s\n", __FUNCTION__);
  cmd.id = AP_UIO_CMD_ENABLE_IR;
  cmd.data1 = 0;
  cmd.data2 = 0;
  ap_frm_do_command(APP_UIO, &cmd);
 /*
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);   */
}

BOOL ui_get_ttx_flag()
{
    return is_start_ttx;
}

void ui_set_ttx_flag(BOOL flag)
{
    is_start_ttx = flag;
}

net_conn_info_t ui_get_conn_info(void)
{
    return g_net_conn_info;
}

void ui_set_conn_info(net_conn_info_t net_conn_info)
{
    g_net_conn_info = net_conn_info;
}

void ui_set_net_connect_status( net_conn_stats_t net_conn_stats)
{
  g_net_connt_stats = net_conn_stats;
}

net_conn_stats_t ui_get_net_connect_status(void)
{
  return g_net_connt_stats;
}

static void ui_desktop_close_special_menus_before_play_satip_prog()
{
    control_t *p_root = NULL;

    p_root = ui_comm_root_get_root(ROOT_ID_VIDEO_PLAYER);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to netplaybar##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to redtube##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }
    
    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to music##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC_FULLSCREEN);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to music fullscreen##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }
    
    p_root = ui_comm_root_get_root(ROOT_ID_USB_PICTURE);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to picture##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }
    
    p_root = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to net music##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_VEPG);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  epg##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_TETRIS);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  game tetris##\n", __FUNCTION__, __LINE__);
        manage_close_menu(ROOT_ID_TETRIS, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_YOUPORN);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  youporn##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_UPGRADE_BY_USB);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to USB upgrade ##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_UPGRADE_BY_RS232);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to rs232 upgrade ##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_DUMP_BY_USB);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  USB dump##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_ALBUMS);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ALBUMS##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC_FULLSCREEN);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_USB_MUSIC_FULLSCREEN##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_USB_MUSIC##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_ONMOV_DESCRIPTION);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_ONMOV_DESCRIPTION##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_NETWORK_PLAY);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_NETWORK_PLAY##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_NETWORK_PLAYBAR);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_NETWORK_PLAYBAR##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_ONMOV_WEBSITES);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_ONMOV_WEBSITES##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_OTA);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_OTA##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_LIVE_TV);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_LIVE_TV##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_IPTV_PLAYER);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_LIVE_TV##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }
    
    OS_PRINTF("#######%s, line[%d]######\n", __FUNCTION__, __LINE__);
}


static void opt_list_refresh(void)
{
}

BOOL ui_find_usb_onlive_file(void)
{
    u16 g_ffilter_all[32] = {0};
    u32 g_partition_cnt = 0;
    file_list_t g_list = {0};
    flist_dir_t g_flist_dir = NULL;
    partition_t *p_partition = NULL;
    u32 count = 0;
    u16 file_name[MAX_FILE_PATH] = {0};
    str_asc2uni("|txt|TXT", g_ffilter_all);
    g_partition_cnt = 0;
    p_partition = NULL;
    g_partition_cnt = file_list_get_partition(&p_partition);  
    if(g_partition_cnt > 0)
    {
        g_flist_dir = file_list_enter_dir(g_ffilter_all, 1000, p_partition[sys_status_get_usb_work_partition()].letter);
        if (g_flist_dir != NULL)
        {
            file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
            count = (u16)g_list.file_count;
            file_name[0] = p_partition[sys_status_get_usb_work_partition()].letter[0];
            str_asc2uni(":\\custom_url.txt", file_name + 1);
            file_live = vfs_open( file_name, VFS_READ);
            if (file_live == NULL)
            {
                return FALSE; 
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
BOOL check_custom_def_in_usb(hfile_t* pp_file_hdl)
{
    u16 g_ffilter_all[32] = {0};
    u32 g_partition_cnt = 0;
    file_list_t g_list = {0};
    flist_dir_t g_flist_dir = NULL;
    partition_t *p_partition = NULL;
    u32 count = 0;
    u16 file_name[MAX_FILE_PATH] = {0};
    str_asc2uni("|bin|", g_ffilter_all);
    g_partition_cnt = 0;
    p_partition = NULL;
    g_partition_cnt = file_list_get_partition(&p_partition);   
    if(g_partition_cnt > 0)
    {
        g_flist_dir = file_list_enter_dir(g_ffilter_all, 1000, p_partition[0].letter);
        if (g_flist_dir != NULL)
        {
            file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
            count = (u16)g_list.file_count;
            file_name[0] = p_partition[0].letter[0];
            str_asc2uni(":\\custom_def.bin", file_name + 1);
            *pp_file_hdl = vfs_open( file_name, VFS_READ);
            if (*pp_file_hdl  == NULL)
            {
                return FALSE; 
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
#if 0
static void start_load_customer_prog()
{
  u8 *p_buffer = NULL;
  u32 file_size = 0;
  if(file_live == NULL)
  {
    return ;
  }

    if(ui_get_usb_status() == FALSE)
    {
       if(file_live)
      {
        vfs_close(file_live);
        file_live = NULL;
      }
       return;
    }
      
    ui_evt_disable_ir();
    OS_PRINTF("#########%s, line[%d]########\n", __FUNCTION__, __LINE__);
    vfs_get_file_info(file_live, &file_info);
    p_buffer = mtos_malloc(file_info.file_size + 2* KBYTES);
    MT_ASSERT(NULL != p_buffer);
    file_size = vfs_read(p_buffer, file_info.file_size, 1, file_live);
    MT_ASSERT(file_size == file_info.file_size);
    //save_custom_url(p_buffer,file_info.file_size);
    ui_evt_enable_ir();
    mtos_free(p_buffer);
    vfs_close(file_live);
    file_live = NULL;
}
static void cancel_load_customer_prog()
{
  if(file_live)
  {
    vfs_close(file_live);
    file_live = NULL;
  }
}
#endif

/*************************************************************

        add functions to  auto connect network
        
*************************************************************/   

extern BOOL check_is_connect_ok(ethernet_device_t * p_eth_dev);
void init_ethernet_ethcfg(ethernet_cfg_t *ethcfg)
{
  ip_address_set_t ss_ip = {{0}};  
  net_config_t lan_config = {0};

  sys_status_get_net_config_info(&lan_config);
  ethcfg->tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  if(lan_config.config_mode == DHCP)
  {
    ethcfg->is_enabledhcp = 1;
  }
  else if(lan_config.config_mode == STATIC_IP)
  {
    ethcfg->is_enabledhcp = 0;
  }    
   
   sys_status_get_ipaddress(&ss_ip);
   //ipaddress
  ethcfg->ipaddr[0] = ss_ip.sys_ipaddress.s_a4;
  ethcfg->ipaddr[1] = ss_ip.sys_ipaddress.s_a3;
  ethcfg->ipaddr[2] = ss_ip.sys_ipaddress.s_a2;
  ethcfg->ipaddr[3] = ss_ip.sys_ipaddress.s_a1;
  
  //netmask
  ethcfg->netmask[0] = ss_ip.sys_netmask.s_a4;
  ethcfg->netmask[1] = ss_ip.sys_netmask.s_a3;
  ethcfg->netmask[2] = ss_ip.sys_netmask.s_a2;
  ethcfg->netmask[3] = ss_ip.sys_netmask.s_a1;
  
  //gateway
  ethcfg->gateway[0] = ss_ip.sys_gateway.s_a4;
  ethcfg->gateway[1] = ss_ip.sys_gateway.s_a3;
  ethcfg->gateway[2] = ss_ip.sys_gateway.s_a2;
  ethcfg->gateway[3] = ss_ip.sys_gateway.s_a1;
  
  //dns server
  ethcfg->primaryDNS[0] = ss_ip.sys_dnsserver.s_a4;
  ethcfg->primaryDNS[1] = ss_ip.sys_dnsserver.s_a3;
  ethcfg->primaryDNS[2] = ss_ip.sys_dnsserver.s_a2;
  ethcfg->primaryDNS[3] = ss_ip.sys_dnsserver.s_a1;

  //mac address
  ethcfg->hwaddr[0] = sys_status_get_mac_by_index(0);
  ethcfg->hwaddr[1] = sys_status_get_mac_by_index(1);
  ethcfg->hwaddr[2] = sys_status_get_mac_by_index(2);
  ethcfg->hwaddr[3] = sys_status_get_mac_by_index(3);
  ethcfg->hwaddr[4] = sys_status_get_mac_by_index(4);
  ethcfg->hwaddr[5] = sys_status_get_mac_by_index(5);
  
}

void do_cmd_connect_network(ethernet_cfg_t *ethcfg, ethernet_device_t * eth_dev)
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = eth_dev;
  memcpy(&net_svc_para.net_cfg, ethcfg, sizeof(ethernet_cfg_t));
  p_server = (service_t *)ui_get_net_svc_instance();
  p_server->do_cmd(p_server, NET_DO_CONFIG_TCPIP, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}
void auto_connect_ethernet()
{

  ethernet_cfg_t ethcfg = {0};
  ethernet_device_t * eth_dev = NULL;

  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  MT_ASSERT(eth_dev != NULL);
  
  init_ethernet_ethcfg(&ethcfg);
  do_cmd_connect_network(&ethcfg, eth_dev);
  
}

void do_cmd_connect_gprs(gprs_info_t *p_gprs_info)
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"neo_m660");
  MT_ASSERT(p_dev != NULL);
  
  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  net_svc_para.net_cfg.gprs_para.tcpip_task_prio = ETH_NET_TASK_PRIORITY;
  strcpy(net_svc_para.net_cfg.gprs_para.apn, p_gprs_info->apn);
  strcpy(net_svc_para.net_cfg.gprs_para.dial_num, p_gprs_info->dial_num);
  strcpy(net_svc_para.net_cfg.gprs_para.usrname, p_gprs_info->usr_name);
  strcpy(net_svc_para.net_cfg.gprs_para.passwd, p_gprs_info->passwd);
  
  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_GPRS_CONNECT, 
                     (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }

}

void do_cmd_disconnect_gprs()
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"neo_m660");
  MT_ASSERT(p_dev != NULL);
  
  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_GPRS_DISCONNECT, (u32)&net_svc_para, 
                     sizeof(net_svc_cmd_para_t));
  }

}


void do_cmd_connect_g3(g3_conn_info_t *p_g3_info)
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(p_dev != NULL);
  
  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  if(p_g3_info != NULL)
  {
      strcpy(net_svc_para.net_cfg.g3_para.apn, p_g3_info->apn);
      strcpy(net_svc_para.net_cfg.g3_para.dial_num, p_g3_info->dial_num);
      strcpy(net_svc_para.net_cfg.g3_para.usrname, p_g3_info->usr_name);
      strcpy(net_svc_para.net_cfg.g3_para.passwd, p_g3_info->passwd);    
  }

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_3G_CONNECT, 
                     (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }

}

void do_cmd_disconnect_g3(void)
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(p_dev != NULL);
  
  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_3G_DISCONNECT, (u32)&net_svc_para, 
                     sizeof(net_svc_cmd_para_t));
  }

}

void do_cmd_get_g3_info()
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(p_dev != NULL);
  
  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_GET_3G_INFO, 
                     (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }

}


static void save_wifi_info_ssdata()
{
  
  //wifi_info_t wifi_inf = get_temp_wifi_info();
  sys_status_get_wifi_info(&p_wifi_info,NULL);
 /* OS_PRINTF("####temp essid = %s, ssdata essid = %s#####\n",wifi_inf.essid,p_wifi_info.essid);
  if(strcmp("", wifi_inf.essid) == 0)
  {
    OS_PRINTF("####first boot, not need save##\n");
    return ;
  }
  if(strcmp("", p_wifi_info.essid) == 0 || strcmp(wifi_inf.essid, p_wifi_info.essid) == 0)
  {
    OS_PRINTF("####current wifi essid is equal ssdata##\n");
  }
  sys_status_set_wifi_info(&wifi_inf);*/
  set_temp_wifi_info(p_wifi_info);
 // OS_PRINTF("####desktop save wifi ap info to ssdata, saved essid is %s##\n",wifi_inf.essid);
 // sys_status_save();
}

extern void get_addr_param(u8 *p_buffer, ip_address_t *p_addr);

#if 1
static RET_CODE on_save_config_ip_to_ssdata(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
#if 1
  ip_address_t addr = {0,};
  ethernet_device_t *eth_dev = NULL;
  ip_address_set_t ss_ip = {{0}};

  u8 ipaddr[20] = {0};
  u8 netmask[20] = {0};
  u8 gw[20] = {0};
  u8 primarydns[20] = {0};
  u8 alternatedns[20] = {0};
  u8 ipaddress[32] = {0};
  control_t *root = NULL;
  u32 is_connect = para1;
  net_config_t net_config = {0};
  OS_PRINTF("####destop after auto connect network the para1 is_connect == %d###\n", para1);

 
   root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);
  
  if(root)
  {
    OS_PRINTF("###at wifi ui, should return from destop connect ok###\n");
    return ERR_FAILURE;
  }

  g_net_connt_stats = ui_get_net_connect_status();//at for network config ui 

  sys_status_get_net_config_info(&net_config);
   
  if(net_config.link_type == LINK_TYPE_LAN) 
  {
    if(is_connect)
    {
      OS_PRINTF("###########lwip_init_tcpip return failed, desktop connect failed######\n ");
      return ERR_FAILURE;
    }
    eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
    MT_ASSERT(eth_dev != NULL);
#ifndef WIN32
    get_net_device_addr_info(eth_dev, ipaddr, netmask, gw, primarydns, alternatedns);

    get_addr_param(ipaddr, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###ethernet auto connect  get dhcp ip address value is:%s####\n",ipaddress);
    get_addr_param(netmask, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###ethernet auto connect  get dhcp netmask value is:%s####\n",ipaddress);
    get_addr_param(gw, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###ethernet auto connect  get dhcp gateway  value is:%s####\n",ipaddress);
    get_addr_param(primarydns, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###ethernet auto connect  get dhcp dns value is:%s####\n",ipaddress);
#endif
    sys_status_set_ipaddress(&ss_ip);
    sys_status_save();
    
    OS_PRINTF("@@@!auto connect ethernet successfully@@@\n");
    g_net_connt_stats.is_eth_conn = TRUE;
    
    g_net_connt_stats.is_gprs_conn = FALSE;
    g_net_connt_stats.is_3g_conn = FALSE;
    g_net_connt_stats.is_wifi_conn = FALSE;

    root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);

    if(root)
    {
      OS_PRINTF("###will update ethernet dhcp and paint network config connect status at network config menu###\n");
      on_config_ip_update(eth_dev);//update ethernet dhcp ipaddress
      paint_connect_status(g_net_connt_stats.is_eth_conn, TRUE);
    }
    if(ROOT_ID_MAINMENU == fw_get_focus_id())
      opt_list_refresh();
  }
  else if(net_config.link_type == LINK_TYPE_WIFI) 
  {
#ifndef WIN32
    if(g_net_connt_stats.is_wifi_insert == FALSE)
    {
          return SUCCESS;
    }
    //p_wifi_devv = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);//add for wifi ui get wifi connected msg
    //MT_ASSERT(p_wifi_devv != NULL);
      #ifndef WIN32
    p_wifi_devv = (ethernet_device_t *)get_wifi_handle();
	#endif
    if(p_wifi_devv == NULL)
    {
         OS_PRINTF("cannot get wifi handle maybe plug out.........\n");
         return ERR_FAILURE;
    }    
    get_net_device_addr_info(p_wifi_devv, ipaddr, netmask, gw, primarydns, alternatedns);

    get_addr_param(ipaddr, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###wifi auto connect  get dhcp ip address value is:%s####\n",ipaddress);
    get_addr_param(netmask, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###wifi auto connect  get dhcp netmask value is:%s####\n",ipaddress);
    get_addr_param(gw, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###wifi auto connect  get dhcp gateway  value is:%s####\n",ipaddress);
    get_addr_param(primarydns, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###wifi auto connect  get dhcp dns value is:%s####\n",ipaddress);
#endif
    save_wifi_info_ssdata();
    sys_status_set_ipaddress(&ss_ip);
    sys_status_save();
    g_net_connt_stats.is_wifi_conn = TRUE;

   g_net_connt_stats.is_gprs_conn = FALSE;
    g_net_connt_stats.is_3g_conn = FALSE;
    g_net_connt_stats.is_eth_conn = FALSE;

    g_net_conn_info.wifi_conn_info = WIFI_CONNECTED;

    OS_PRINTF("@@@!auto connect wifi successfully@@@\n");
    root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
    if(root)
    {
      OS_PRINTF("###will update wifi dhcp and paint network config connect status at network config menu###\n");
      on_config_ip_update_wifi(p_wifi_devv);//update wifi dhcp ipaddress
      paint_connect_wifi_status(g_net_connt_stats.is_wifi_conn, TRUE);
    }

    if(ROOT_ID_MAINMENU == fw_get_focus_id())
    {
        opt_list_refresh();
    }
    root = fw_find_root_by_id(ROOT_ID_WIFI);
    if(root)
    {
      u16 focus = get_wifi_select_focus();
      if(focus == INVALIDID)
      {
        focus = get_wifi_connecting_focus();
      }
      OS_PRINTF("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$focus======%d$$$$$$$$$$$$$$$$$$\n",focus);
      paint_wifi_status(TRUE, TRUE);
      paint_list_field_is_connect(focus, TRUE, TRUE);
    }
    
  }
  else if(net_config.link_type == LINK_TYPE_GPRS) 
  {
    
    g_net_connt_stats.is_gprs_conn = TRUE;

    g_net_connt_stats.is_3g_conn = FALSE;
    g_net_connt_stats.is_eth_conn = FALSE;
    g_net_connt_stats.is_wifi_conn = FALSE;
    
/*
    root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG);
    if(root)
    {
      OS_PRINTF("###will update gprs dhcp and paint network config connect status at network config menu###\n");
      on_config_ip_update(p_gprs_dev);
    }*/
  if(ROOT_ID_MAINMENU == fw_get_focus_id())
  {
    opt_list_refresh();
   }
  }  
  else if(net_config.link_type == LINK_TYPE_3G)
  {
    g_net_connt_stats.is_3g_conn = TRUE;
    g_net_connt_stats.is_eth_conn = FALSE;
    g_net_connt_stats.is_gprs_conn = FALSE;
    g_net_connt_stats.is_wifi_conn = FALSE;
    if(ROOT_ID_MAINMENU == fw_get_focus_id())
    {
        opt_list_refresh();
    }
  }

#endif
  return SUCCESS;
}
#endif

static RET_CODE on_desktop_gprs_status_update(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
   u32 status = para1;
  
   if(status == MODEM_DEVICE_NETWORK_CONNECTED)
   {
      on_save_config_ip_to_ssdata(p_ctrl, msg, para1, para2);
      paint_gprs_conn_status(TRUE);
   }
   else if(status == MODEM_DEVICE_NETWORK_UNCONNECTED)
   {
     //
     paint_gprs_conn_status(FALSE);
   }

   g_net_conn_info.gprs_conn_info = para1;

   set_gprs_connect_status(status);
   
   return SUCCESS;
} 

static RET_CODE on_desktop_g3_status_update(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
   g3_status_info_t *p_g3_info = NULL;

   p_g3_info = (g3_status_info_t *)para1;
   if(p_g3_info->status == MODEM_DEVICE_NETWORK_CONNECTED)
   {   
      on_save_config_ip_to_ssdata(p_ctrl, msg, para1, para2);
   }

   g_net_conn_info.g3_conn_info = p_g3_info->status;

   set_3g_connect_status(p_g3_info->status, p_g3_info->strength, p_g3_info->operator);
   
   return SUCCESS;
} 


static RET_CODE on_3g_dev_ready(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
   g3_conn_info_t p_g3_info = {{0}};

  // extern u32 get_3g_status();
   if((g_net_conn_info.g3_conn_info != MODEM_DEVICE_NO_SIM_CARD) && (g_net_conn_info.g3_conn_info != MODEM_DEVICE_NO_SIGNAL))
   {
        fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_G3_READY);
        memcpy(&p_g3_info, sys_status_get_3g_info(), sizeof(g3_conn_info_t));
        do_cmd_connect_g3(&p_g3_info);       
   }
   return SUCCESS;
}

static RET_CODE on_desktop_wifi_connecting(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
    g_net_conn_info.wifi_conn_info = WIFI_CONNECTING;
    
    return SUCCESS;

}

static RET_CODE on_desktop_connect_wifi_failed(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  control_t *root = NULL;
  net_config_t net_config = {0};
  net_conn_stats_t wifi_conn_sts = {0};
  root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);

  g_net_conn_info.wifi_conn_info = WIFI_DIS_CONNECT;
  
  if(root)
  {
    OS_PRINTF("###at wifi ui, should return from destop connect failed###\n");
    return ERR_FAILURE;
  }
  wifi_conn_sts = ui_get_net_connect_status();
/*  if(!wifi_conn_sts.is_wifi_conn)
  {
    OS_PRINTF("####wifi status is disconnect, should ignore this disconnect msg###\n");
    return ERR_FAILURE;
  }*/
  sys_status_get_net_config_info(&net_config);
  if(net_config.link_type == LINK_TYPE_LAN) 
  {
    OS_PRINTF("###link type is ethernet, wifi connect failed will return###\n");
    return ERR_FAILURE;
  }
  else if(net_config.link_type == LINK_TYPE_WIFI) 
  {
    wifi_conn_sts.is_wifi_conn = FALSE;
    ui_set_net_connect_status(wifi_conn_sts);
    root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
    if(root)
    {
      paint_wifi_connect_status(wifi_conn_sts.is_wifi_conn, TRUE);
    }
    root = fw_find_root_by_id(ROOT_ID_WIFI);
    if(root)
    {
      control_t *p_list = NULL;
      p_list = ui_comm_root_get_ctrl(ROOT_ID_WIFI, 8);//IDC_WIFI_LIST
      paint_wifi_list_field_not_connect(p_list);
      paint_wifi_status(FALSE, TRUE);
    }
  }
  
  OS_PRINTF("###at  destop, connect wifi failed###\n");
  return SUCCESS;
}

static void close_all_samba_menu()
{
  control_t *root = NULL;
  root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC);
  if(root && !get_music_is_usb())
  {
    control_t *full_screen_root = NULL;
    full_screen_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC_FULLSCREEN);
    if(full_screen_root)
    {
      ctrl_process_msg(full_screen_root, MSG_EXIT, 0, 0);
    }
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
  root = ui_comm_root_get_root(ROOT_ID_USB_PICTURE);
  if(root && !get_picture_is_usb())
  {
    control_t *full_screen_root = NULL;
    full_screen_root = ui_comm_root_get_root(ROOT_ID_PICTURE);
    if(full_screen_root)
    {
      ctrl_process_msg(full_screen_root, MSG_EXIT, 0, 0);
    }
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
  root = ui_comm_root_get_root(ROOT_ID_USB_FILEPLAY);
  
    if(root && !get_video_is_usb())
  {
    control_t *full_screen_root = NULL;
    full_screen_root = ui_comm_root_get_root(ROOT_ID_FILEPLAY_BAR);
    if(full_screen_root)
    {
      ctrl_process_msg(full_screen_root, MSG_EXIT, 0, 0);
    }
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
}


static void close_common_menus()
{
  if(NULL != fw_find_root_by_id(ROOT_ID_POPUP))
  {
    manage_close_menu(ROOT_ID_POPUP, 0, 0);
  }
  if(NULL != fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
  {
    manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
  }
}

static void close_all_network_menus()
{
  control_t *root = NULL;

  root = fw_find_root_by_id(ROOT_ID_ONMOV_WEBSITES);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }
  
  root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAY);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }
  
  root = fw_find_root_by_id(ROOT_ID_ONMOV_DESCRIPTION);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }
  
  root = fw_find_root_by_id(ROOT_ID_VIDEO_PLAYER);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }

  root = fw_find_root_by_id(ROOT_ID_IPTV_PLAYER);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT_ALL, 0, 0);
  }
  
  root = fw_find_root_by_id(ROOT_ID_YOUTUBE);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_INTERNET_PLUG_OUT, 0, 0);
  }
  
  root = ui_comm_root_get_root(ROOT_ID_GOOGLE_MAP);
  if(root)
  {   
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
  
  root = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
  if(root)
  {   
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
  
  root = ui_comm_root_get_root(ROOT_ID_WEATHER_FORECAST);
  if(root)
  {
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }

  root = ui_comm_root_get_root(ROOT_ID_LIVE_TV);
  if(root)
  {
    ctrl_process_msg(root, MSG_EXIT_ALL, 0, 0);
  }
  
  close_all_samba_menu();
}

/*********************************************************/
/*                         if lan or wifi disconnect, will close satip and dlna        */
 /*********************************************************/          
static void close_all_dlna_switch_after_disconnect_lan_or_wifi()
{
#if 0
    control_t *p_root = NULL; 

    OS_PRINTF("####%s####\n", __FUNCTION__);

    p_root = ui_comm_root_get_root(ROOT_ID_DLNA_START);
    if(p_root)
    {   
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to dlna##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }

    manage_close_menu(ROOT_ID_SATIP, 0, 0);  
    if(ui_get_satip_status() == SATIP_ON)
    {        
        ui_comm_cfmdlg_open(NULL, IDS_CLOSING_SATIP, NULL, 2000);
        ui_cg_dlna_server_stop();
        ui_stop_mini_httpd();
        ui_set_satip_status(SATIP_OFF);
    }
#endif
}


static RET_CODE on_ethernet_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  net_config_t g_net_config = {0,};
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };

  g_net_connt_stats.is_eth_insert = TRUE;

  if(root)
  {
    OS_PRINTF("###at network config, should return from destop plug in###\n");
    return ERR_FAILURE;
  }


  root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
  if(root)
  {
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
  }


  sys_status_get_net_config_info(&g_net_config);
  if (g_net_config.link_type == LINK_TYPE_LAN)
  {
    if(g_net_connt_stats.is_eth_conn == FALSE)
    {
      auto_connect_ethernet();
    }
  }
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
  {
      ui_comm_dlg_close();
  }

  if (g_deskto_init)
  {
    dlg_data.content = IDS_NET_CABLE_PLUG_IN;
    dlg_data.dlg_tmout = 2000;
    ui_comm_dlg_open(&dlg_data);
  }
  return SUCCESS;
}

static RET_CODE on_ethernet_plug_out(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };
    
  //ui_comm_cfmdlg_open(NULL, IDS_NET_CABLE_PLUG_OUT, NULL, 3000);
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);

  //if(ROOT_ID_MAINMENU == fw_get_focus_id())
  OS_PRINTF("############desktop--->>ethernet plug out###########\n");
  g_net_connt_stats.is_eth_insert = FALSE;
  g_net_connt_stats.is_eth_conn = FALSE;

  if(ROOT_ID_MAINMENU == fw_get_focus_id())
  {
    opt_list_refresh();
  }
  else if(ROOT_ID_SUBNETWORK_CONFIG == fw_get_focus_id())
  {
    root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(root)
    {
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
    }
  }
  close_common_menus();

  if(root)
  {
    OS_PRINTF("###at network config, should return from destop plug out###\n");
    return ERR_FAILURE;
  }
  
  g_net_connt_stats = ui_get_net_connect_status();
  if((!g_net_connt_stats.is_wifi_conn)&&(!g_net_connt_stats.is_gprs_conn)&&(!g_net_connt_stats.is_3g_conn))
  {
    close_all_network_menus();
    close_all_dlna_switch_after_disconnect_lan_or_wifi();
  }

  if((fw_get_focus_id() == ROOT_ID_SMALL_LIST) ||
    (fw_get_focus_id() == ROOT_ID_PROG_BAR) ||
    (fw_get_focus_id() == ROOT_ID_BACKGROUND))
  {      
      ui_video_c_stop();
      dlg_data.content = IDS_NET_CABLE_PLUG_OUT;
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
        dlg_data.dlg_tmout = 0;
      else
        dlg_data.dlg_tmout = 2000;
      ui_comm_dlg_close();
      if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
      {
        ui_comm_dlg_open(&dlg_data);
      }
  }
  
  return SUCCESS;
}
/**************************************
    auto connect wifi
    
 **************************************/
void init_desktop_wifi_ethcfg()
{
//   ip_address_set_t ss_ip = {{0}};
   
   wifi_ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
   wifi_ethcfg.is_enabledhcp = 1;
   
   /*sys_status_get_ipaddress(&ss_ip);
   //ipaddress
  wifi_ethcfg.ipaddr[0] = ss_ip.sys_ipaddress.s_a4;
  wifi_ethcfg.ipaddr[1] = ss_ip.sys_ipaddress.s_a3;
  wifi_ethcfg.ipaddr[2] = ss_ip.sys_ipaddress.s_a2;
  wifi_ethcfg.ipaddr[3] = ss_ip.sys_ipaddress.s_a1;

  //netmask
  wifi_ethcfg.netmask[0] = ss_ip.sys_netmask.s_a4;
  wifi_ethcfg.netmask[1] = ss_ip.sys_netmask.s_a3;
  wifi_ethcfg.netmask[2] = ss_ip.sys_netmask.s_a2;
  wifi_ethcfg.netmask[3] = ss_ip.sys_netmask.s_a1;

  //gateway
  wifi_ethcfg.gateway[0] = ss_ip.sys_gateway.s_a4;
  wifi_ethcfg.gateway[1] = ss_ip.sys_gateway.s_a3;
  wifi_ethcfg.gateway[2] = ss_ip.sys_gateway.s_a2;
  wifi_ethcfg.gateway[3] = ss_ip.sys_gateway.s_a1;

  //dns server
  wifi_ethcfg.primaryDNS[0] = ss_ip.sys_dnsserver.s_a4;
  wifi_ethcfg.primaryDNS[1] = ss_ip.sys_dnsserver.s_a3;
  wifi_ethcfg.primaryDNS[2] = ss_ip.sys_dnsserver.s_a2;
  wifi_ethcfg.primaryDNS[3] = ss_ip.sys_dnsserver.s_a1;*/
  
  //mac address
  dev_io_ctrl(p_wifi_devv, GET_ETH_HW_MAC_ADDRESS, (u32)&wifi_ethcfg.hwaddr[0]);
  OS_PRINTF("#######init_wifi_ethcfg#######MAC= %02x-%02x-%02x-%02x-%02x-%02x##############\n",
                          wifi_ethcfg.hwaddr[0], wifi_ethcfg.hwaddr[1],wifi_ethcfg.hwaddr[2],
                          wifi_ethcfg.hwaddr[3], wifi_ethcfg.hwaddr[4], wifi_ethcfg.hwaddr[5]);

}

#if 0
void do_cmd_to_search_wifi()
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  if(p_net_svc->net_svc_is_busy(p_net_svc) == TRUE)
  {
    ui_comm_cfmdlg_open(NULL, IDS_NETWORK_BUSY, NULL, 2000);
    OS_PRINTF("###########net service is busy ,will be return#########\n");
    return ;
  }
  net_svc_para.p_eth_dev = p_wifi_devv;
  p_server = (service_t *)ui_get_net_svc_instance();
  
  p_server->do_cmd(p_server, NET_DO_WIFI_SERACH, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}
#endif
void do_cmd_to_conn_desktop_wifi()
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  sys_status_get_wifi_info(&p_wifi_info,NULL);
  if(strlen(p_wifi_info.essid) == 0)
  {
    return;
  }  
  
  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  net_svc_para.p_eth_dev = p_wifi_devv;
  memcpy(&net_svc_para.net_cfg, &wifi_ethcfg, sizeof(ethernet_cfg_t));
  strcpy(net_svc_para.net_cfg.wifi_para.key, p_wifi_info.key);
  strcpy(net_svc_para.net_cfg.wifi_para.ssid, p_wifi_info.essid);
  net_svc_para.net_cfg.wifi_para.is_enable_encrypt = 1;
  net_svc_para.net_cfg.wifi_para.encrypt_type = p_wifi_info.encrypt_type;
  OS_PRINTF("#####################do_cmd_to_conn_desktop_wifi essid == %s###################\n",net_svc_para.net_cfg.wifi_para.ssid);
  p_server = (service_t *)ui_get_net_svc_instance();
  
  p_server->do_cmd(p_server, NET_DO_WIFI_CONNECT, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));

}

#if 0
static RET_CODE on_desktop_search_wifi_over(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
 {
#ifndef WIN32  
  u32 wifi_ap_cnt = para1;
  u16 i;
  wifi_ap_info_t ap_info;
  control_t *root = fw_find_root_by_id(ROOT_ID_WIFI);
  if(root)
  {
    OS_PRINTF("###after search wifi, get ap count == %d###\n",wifi_ap_cnt);
    OS_PRINTF("###at wifi ui, should return from destop wifi search###\n");
    return ERR_FAILURE;
  }
  sys_status_get_wifi_info(&p_wifi_info);
  if(wifi_ap_cnt < 0 || wifi_ap_cnt == 0 || strcmp("", p_wifi_info.essid) == 0)
  {
    OS_PRINTF("@@@@wifi count <= 0, or ssdata wifi essid is NULL, so return@@\n");
    return ERR_FAILURE;
  }
  for(i = 0; i < wifi_ap_cnt; i++)
  {
    wifi_ap_info_get(p_wifi_devv,(u32)i, &ap_info);
    if(strcmp(p_wifi_info.essid, ap_info.essid) == 0)
    {
      OS_PRINTF("###desktop search wifi equal ssdata wifi, is %s, will auto connect  wifi####\n", ap_info.essid);
      do_cmd_to_conn_desktop_wifi();
    }
  }
  
  OS_PRINTF("@@@@wifi count == %d@@\n",wifi_ap_cnt);  
  OS_PRINTF("@@@on_wifi_search_ok@@@\n");
#endif
  return SUCCESS;
}
#endif
#if 0
static RET_CODE on_desktop_wifi_connect_ap(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
 {
#ifndef WIN32  
  ethernet_device_t * p_wifi_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, 
              SYS_DEV_TYPE_USB_WIFI);
  ethernet_cfg_t ethcfg = {0};
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;

  ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  ethcfg.is_enabledhcp = 1;
   
  //mac address
  dev_io_ctrl(p_wifi_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&ethcfg.hwaddr[0]);
  OS_PRINTF("#######init_wifi_ethcfg#######MAC= %02x-%02x-%02x-%02x-%02x-%02x##############\n",
                    ethcfg.hwaddr[0], ethcfg.hwaddr[1],ethcfg.hwaddr[2],
                    ethcfg.hwaddr[3], ethcfg.hwaddr[4], ethcfg.hwaddr[5]);


  net_svc_para.p_eth_dev = p_wifi_dev;
  p_server = (service_t *)ui_get_net_svc_instance();
  memcpy(&net_svc_para.net_cfg, &ethcfg, sizeof(ethernet_cfg_t));
  p_server->do_cmd(p_server, NET_DO_CONFIG_TCPIP, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  OS_PRINTF("set wifi tcpip\n");
#endif
  return SUCCESS;
}
#endif
static void auto_connect_wifi()
{
  #ifndef WIN32
  p_wifi_devv = (ethernet_device_t *)get_wifi_handle();
  //p_wifi_devv = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  MT_ASSERT(p_wifi_devv != NULL);
  #endif
  
  init_desktop_wifi_ethcfg();
 // do_cmd_to_search_wifi();
  do_cmd_to_conn_desktop_wifi();
}


static RET_CODE on_net_ip_mount(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  u32 is_mont_success = para1;
  char ipaddress[128] = "";

  ui_get_ip(ipaddress);

  if(is_mont_success == 1)
  {
    OS_PRINTF("lou it  connect ip success\n");
    
    manage_open_menu(ROOT_ID_USB_FILEPLAY,1 ,(u32)(ipaddress));

    if (fw_find_root_by_id(ROOT_ID_NETWORK_PLACES) != NULL)
      manage_close_menu(ROOT_ID_NETWORK_PLACES, 0, 0);
    if (fw_find_root_by_id(ROOT_ID_EDIT_USR_PWD) != NULL)
      manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);

  }
 else
  {
     ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_CONTENT, NULL, 0);
  }

  return SUCCESS;
}

static RET_CODE on_net_mount_success(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  char ipaddress[128] = {0};
  
  ui_get_ip(ipaddress);
  
  OS_PRINTF("lou it  connect ip success\n");
  
  manage_open_menu(ROOT_ID_USB_FILEPLAY,1 ,(u32)(ipaddress));

  if (fw_find_root_by_id(ROOT_ID_NETWORK_PLACES) != NULL)
      manage_close_menu(ROOT_ID_NETWORK_PLACES, 0, 0);
  if (fw_find_root_by_id(ROOT_ID_EDIT_USR_PWD) != NULL)
      manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);

  return SUCCESS;
}

static RET_CODE on_net_mount_fail(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_CONTENT, NULL, 0);
  return SUCCESS;
}

static BOOL ui_desktop_check_satip_client_request_pg_is_exist(u16 pg_id)
{
    dvbs_prog_node_t db_pg = {0};
    u8 pg_view = 0;
    u32 pg_cnt = 0;
    u8 ret;
    pg_view = db_dvbs_create_view(DB_DVBS_ALL_TV_FTA, 0, NULL);
    pg_cnt = db_dvbs_get_count(pg_view);
    ret = db_dvbs_get_pg_by_id(pg_id, &db_pg);
    OS_PRINTF("##%s, ret == %d##\n", __FUNCTION__, ret);
    if(ret != DB_DVBS_OK)
    {
        ui_comm_cfmdlg_open(NULL, IDS_REQUEST_PG_NOT_EXIST, NULL, 2000);
        OS_PRINTF("##%s, client request pg not exist at database##\n",  __FUNCTION__);
        return FALSE;
    }
    return TRUE;
}
static BOOL ui_check_satip_client_and_curr_pg_tp(u16 client_pg_id)
{
    dvbs_prog_node_t client_pg = {0};
    dvbs_prog_node_t curr_pg = {0};
    u16 curr_pg_id;
    curr_pg_id = sys_status_get_curn_group_curn_prog_id();
    db_dvbs_get_pg_by_id(client_pg_id, &client_pg);
    db_dvbs_get_pg_by_id(curr_pg_id, &curr_pg);
    if(client_pg.tp_id == curr_pg.tp_id)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



    

    
    







    

static RET_CODE on_desktop_recieve_satip_client_msg(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    u16 pg_id = para1;
    OS_PRINTF("##%s, line[%d], prog_id ==[%d]##\n", __FUNCTION__, __LINE__,pg_id);
    if(ui_desktop_check_satip_client_request_pg_is_exist(pg_id) == FALSE)
    {
        OS_PRINTF("##%s, line[%d], datebase not exist this prog##\n", __FUNCTION__, __LINE__);
        return SUCCESS;
    }
    if(pg_id == sys_status_get_curn_group_curn_prog_id() && ui_is_playing())
    {
        OS_PRINTF("##%s, line[%d], the same prog##\n", __FUNCTION__, __LINE__);
        return SUCCESS;
    }
    if(ui_check_satip_client_and_curr_pg_tp(pg_id) == TRUE)
    {
        OS_PRINTF("##%s, line[%d], the same tp##\n", __FUNCTION__, __LINE__);
        return SUCCESS;
    }
    
    if((UI_USB_UPG_IDEL != ui_usb_upgade_sts())
              || (UI_USB_DUMP_IDEL != ui_usb_dumping_sts())
        || ui_is_rs232_upgrading())
    {
        OS_PRINTF("##%s, line[%d], upgrade or dump ui, ignore##\n", __FUNCTION__, __LINE__);
        return SUCCESS;
    }
    
    ui_desktop_close_special_menus_before_play_satip_prog();
    ui_close_all_mennus();
    
    p_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);
    if(p_root == NULL)
    {
        OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
        manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_UNKNOW, pg_id);
    }
    else
    {
        OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
        manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
        manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_UNKNOW, pg_id);
    }
    
    return SUCCESS;
}

static RET_CODE on_desktop_load_media_success(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
  u8 focus = fw_get_focus_id();
  if(ROOT_ID_BACKGROUND!= focus)
    {
      return SUCCESS;
    }
 if( fw_find_root_by_id(ROOT_ID_SMALL_LIST) != NULL)
  return SUCCESS;
 if( fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
  return SUCCESS;
  ui_comm_dlg_close();

  return SUCCESS;
}

static RET_CODE on_desktop_load_media_error(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
   return SUCCESS;
}

static RET_CODE on_desktop_load_media_exit(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
   return SUCCESS;
}

static RET_CODE on_dlna_start_pic(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
   return manage_open_menu(ROOT_ID_DLNA_PICTURE, para1,para2);
}

static RET_CODE on_dlna_close_pic(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
  return manage_close_menu(ROOT_ID_DLNA_PICTURE, 0,0);
}

static RET_CODE on_dlna_start_video(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
  OS_PRINTF("\ngo to network playbar\n");
  return manage_open_menu(ROOT_ID_NETWORK_PLAYBAR, para1,para2);
}

static RET_CODE on_dlna_close_video(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{

  return manage_close_menu(ROOT_ID_NETWORK_PLAYBAR, 0,0);
}
void ui_get_stb_mac_addr_chip_id(u8 *p_mac, u8 *p_id)
{
    u8 temp_mac[32] = {0};
    u8 temp_id[32] = {0};
    u16 i;
    u8 mac_map[3] = {0};
    for(i = 0; i < 6; i ++)  
    {
        sys_status_get_mac(i, mac_map);
        strcat(temp_mac,mac_map); 
    }
    temp_mac[12] = '\0';
    sprintf(temp_id, "%08d", (int)0); 
    memcpy(p_mac, temp_mac, sizeof(temp_mac) - 1);
    memcpy(p_id, temp_id, sizeof(temp_id) - 1);
    OS_PRINTF("##%s, mac addr = [%s], sn num = [%s]##\n", __FUNCTION__, p_mac, p_id);
}
#endif

#ifdef ONLY1_CA_VER
static RET_CODE on_emon_test_finger(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  open_finger_print( para1, para2);
  return SUCCESS;
}	
static RET_CODE on_emon_hide_finger(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("@@@@@ on_ca_hide_finger\n");
  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER);
  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_SHOW_FP);
  fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_CLOSE_FP);

  if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL ||
     fw_find_root_by_id(ROOT_ID_FINGER_PRINT_1) != NULL ||
     fw_find_root_by_id(ROOT_ID_FINGER_PRINT_2) != NULL ||
     fw_find_root_by_id(ROOT_ID_FINGER_PRINT_3) != NULL)
  {
    ui_close_finger_menu();
  }
  return SUCCESS;
}
#endif
#ifdef FACTORY_TEST_SOFTWARE
//if add this macro , need add ui_factory_test.c, for customer factory test
//extern RET_CODE on_factory_test_priv(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE on_factory_test(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  on_factory_test_priv(p_ctrl, msg, para1, para2);
  return SUCCESS;
}


static RET_CODE Test_communication(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  Test_communication_priv(p_ctrl, msg, para1, para2);
  
  return SUCCESS;
}
#endif


//#if ((defined BOYUAN_CA) || ((defined DTMB_PROJECT) && (defined SZXC_423_CA)) || (defined DIVI_CA))
#ifdef CA_SET_ECM_EMM_WAIT_LOCKED

extern void locked_set_emm_ecm(void);
static RET_CODE on_locked_set_ecm_emm(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2)
{
  if(para1 == TRUE)
  {
    locked_set_emm_ecm();
  }  
  return SUCCESS;
}
#endif
#ifdef NIT_SETTING
static RET_CODE on_delete_all_aisat(control_t *p_ctrl,
                                                  u16 msg,
                                                  u32 para1,
                                                  u32 para2)
{
  u8 scankey_status;
  if(fw_get_focus_id() == ROOT_ID_UPGRADE_BY_USB ||
      fw_get_focus_id() == ROOT_ID_PVR_PLAY_BAR ||
      fw_get_focus_id() == ROOT_ID_RECORD_MANAGER)
  {
	return SUCCESS;
  }
  scankey_status = sys_status_get_scankey_status();
  if(scankey_status)
  	  ui_comm_ask_for_savdlg_open(&g_del_all_dlg_rc,
                              IDS_SCAN_CHANNEL,
                              delete_all_prog,NULL,0);
  return SUCCESS;
}
#endif
#ifdef CUSTOMER_YINHE_TR
extern void ui_set_mainmenu_open_level(u32 type);

static RET_CODE on_open_sys_set(control_t *p_ctrl,
                                                  u16 msg,
                                                  u32 para1,
                                                  u32 para2)
{
    ui_set_mainmenu_open_level(2);
    manage_open_menu(ROOT_ID_MAINMENU, 0, 0);
    return SUCCESS;
}
#endif
RET_CODE customer_manage_proc_on_normal_customer(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
   return ui_desktop_proc_customer(p_ctrl,msg,para1,para2);

}
u16 customer_manage_keymap_on_normal_customer(u16 key)
{
   return ui_desktop_keymap_customer(key);   
}

#ifdef DALIAN_ZHUANGGUANG
extern  void divi_restore_to_factory(void);
static RET_CODE on_factory_pwdlg_correct_divi(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  divi_restore_to_factory();
	ui_stop_play(STOP_PLAY_BLACK,TRUE);
	mtos_task_delay_ms(1000);
  
  return manage_open_menu(ROOT_ID_MAINMENU, 0, 0);
}

static RET_CODE on_ca_card_feed_divi(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if (fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    
    if(!ui_is_feeding())
    {
      OS_PRINTF("start card feed!\n");
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_FEED, 0, 0);
    }
  }
  return SUCCESS;
}

static RET_CODE on_prog_info_divi(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if (fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    manage_open_menu(ROOT_ID_PRO_INFO, 0, 0);
  }
  
  return SUCCESS;
}

static RET_CODE on_restore_factory_divi(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_pwdlg_data_t factory_pwdlg_data =
  {
    ROOT_ID_BACKGROUND,
    (SCREEN_WIDTH-PWDLG_W)/2,
    (SCREEN_WIDTH-PWDLG_W)/2,
    IDS_CA_INPUT_PASSWORD_JHC,
    ui_desktop_keymap_on_normal,
    ui_desktop_proc_on_normal,
    PWDLG_T_COMMON,
  };

  if (fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    ui_comm_pwdlg_open(&factory_pwdlg_data);
  }
  
  return SUCCESS;
}
#endif

#ifdef JHC_CN_DS52
static RET_CODE on_prog_info_jhc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if (fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    manage_open_menu(ROOT_ID_PRO_INFO, 0, 0);
  }
  return SUCCESS;
}
#endif

#if ENABLE_CUSTOMER_URL

//add some fast key
void  load_custom_url( u16 *cusNum)
{
    u16 pg_num = 0;
    u8* p_line = NULL, *p_buf = NULL;
    void *p_dm_hdl = NULL;
    s32 dataLen = 0;

    p_dm_hdl = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_hdl != NULL);
    dm_direct_read(p_dm_hdl, CUS_URL_BLOCK_ID, CUSTOM_URL_BLOCK_SIZE, CUSTOM_URL_FLASH_LEN, (u8 *)&dataLen);
    OS_PRINTF("##%s, dataLen=[%d]##\n", __FUNCTION__, dataLen);
    if(dataLen <= 0)
    {
        return ;
    }

    p_buf = (u8*)mtos_malloc(dataLen);
    MT_ASSERT(p_buf != NULL);
    dm_direct_read(p_dm_hdl, CUS_URL_BLOCK_ID, 0, (u32)dataLen, p_buf);
   
    p_buf[--dataLen] = '\0';
    if(dataLen > 0)
    {
        while(pg_num < MAX_CUSTOM_URL && (p_line = strtok(p_line ? NULL : p_buf, "\r\n")))
        {
            pg_num++;
        }
    }

    if(p_buf)
    {
        mtos_free(p_buf);
        p_buf = NULL;
    }

    *cusNum = pg_num;
}


 u16 ui_get_customer_iptv_pg_cnt(void)
{
	u16 pg_cnt = 0;

       load_custom_url( &pg_cnt);

	OS_PRINTF("$$$$$$$ %s,%d,pg_cnt==%d\n\n",__FUNCTION__,__LINE__,pg_cnt);

     return pg_cnt;
}

static BOOL check_at_fullscreen(void)
{
	if(fw_get_focus_id() == ROOT_ID_BACKGROUND || fw_get_focus_id() == ROOT_ID_PROG_BAR)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static BOOL  check_net_is_ok(void)
{
	net_conn_stats_t eth_connt_stats = {0};
	eth_connt_stats = ui_get_net_connect_status();

	if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE)
	&& (eth_connt_stats.is_3g_conn == FALSE) && (eth_connt_stats.is_gprs_conn == FALSE))
	{
		ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
		return FALSE;
	}
	return TRUE;
}

static RET_CODE on_open_iptv_at_fullscreen(control_t *p_ctrl, u16 msg,
							 u32 para1, u32 para2)
{
	ui_livetv_entry_grp_t entry_grp = {0};
	//u8 root_id = ROOT_ID_LIVE_TV;
       manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
	if(check_at_fullscreen() && check_net_is_ok())
	{	
	    #if 0
		if(msg == MSG_OPEN_IPTV_ALL)
		{
                entry_grp = ENTRY_ALL_GRP;
                #ifndef WIN32
                Iptv_Get_Pg_Info(entry_grp, TRUE);
                #endif
		 }
		else if(msg == MSG_OPEN_IPTV_CUS)
      #endif
		{
		    if(ui_get_customer_iptv_pg_cnt() == 0)
			{
				ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
				return SUCCESS;
			}
			else
			{
                        entry_grp = ENTRY_CUS_GRP;
                        manage_open_menu(ROOT_ID_LIVE_TV, entry_grp, 0);
			}
		}
  }

	return SUCCESS;
}
#endif

BEGIN_AP_EVTMAP(sys_evtmap)
CONVERT_EVENT(PNP_SVC_USB_MASS_PLUG_IN, MSG_PLUG_IN)
CONVERT_EVENT(PNP_SVC_USB_DEV_PLUG_OUT, MSG_PLUG_OUT)
CONVERT_EVENT(PNP_SVC_USB_DEV_PLUG_IN, MSG_USB_DEV_PLUG_IN)
#ifdef ENABLE_NETWORK
CONVERT_EVENT(PNP_SVC_VFS_MOUNT_SUCCESS, MSG_NETWORK_MOUNT_SUCCESS)
CONVERT_EVENT(PNP_SVC_VFS_MOUNT_FAIL, MSG_NETWORK_MOUNT_FAIL)
CONVERT_EVENT(PNP_SVC_INTERNET_PLUG_IN, MSG_INTERNET_PLUG_IN)
CONVERT_EVENT(PNP_SVC_INTERNET_PLUG_OUT, MSG_INTERNET_PLUG_OUT)
CONVERT_EVENT(PNP_SVC_WIFI_CONNECTING_AP, MSG_WIFI_AP_CONNECTING)
CONVERT_EVENT(PNP_SVC_WIFI_CONNECT_AP, MSG_WIFI_AP_CONNECT)
CONVERT_EVENT(PNP_SVC_WIFI_DIS_AP, MSG_WIFI_AP_DISCONNECT)
CONVERT_EVENT(PNP_SVC_WIFI_CONNECT_FAIL_AP, MSG_WIFI_AP_DISCONNECT)
CONVERT_EVENT(NET_SVC_CONFIG_IP, MSG_CONFIG_IP)
CONVERT_EVENT(NET_SVC_PING_TEST, MSG_PING_TEST)
CONVERT_EVENT(NET_SVC_WIFI_AP_CNT, MSG_GET_WIFI_AP_CNT)
CONVERT_EVENT(NET_SVC_3G_INFO_GET, MSG_GET_3G_INFO)
CONVERT_EVENT(PNP_SVC_GPRS_STATUS_UPDATE, MSG_GPRS_STATUS_UPDATE)
CONVERT_EVENT(PNP_SVC_3G_STATUS_UPDATE, MSG_G3_STATUS_UPDATE)
#endif
END_AP_EVTMAP(sys_evtmap)

// start message mapping
BEGIN_MSGPROC(ui_desktop_proc_on_normal, customer_manage_proc_on_normal_customer)
ON_COMMAND(MSG_DESKTOP_ROLLING_START, on_desktop_roll_start)
ON_COMMAND(MSG_DESKTOP_ROLLING_STOP, on_desktop_roll_stop)
ON_COMMAND(MSG_HEART_BEAT, on_heart_beat)
ON_COMMAND(MSG_PLUG_IN, on_plug_in)
ON_COMMAND(MSG_PLUG_OUT, on_plug_out)
ON_COMMAND(MSG_USB_DEV_PLUG_IN, on_usb_dev_plug_in)
ON_COMMAND(MSG_SIGNAL_CHECK, on_check_signal)
ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
#ifdef ENABLE_CA
ON_COMMAND(MSG_DESCRAMBLE_SUCCESS, on_update_descramble)
ON_COMMAND(MSG_DESCRAMBLE_FAILED, on_update_descramble)
#endif
ON_COMMAND(MSG_CLOSE_MENU, on_close_menu)
ON_COMMAND(MSG_OPEN_MENU_IN_TAB, on_open_in_tab)
ON_COMMAND(MSG_MUTE, on_mute)
ON_COMMAND(MSG_PAUSE, on_pause)
ON_COMMAND(MSG_PLAY, on_play)

//#if ((defined BOYUAN_CA) || ((defined DTMB_PROJECT) && (defined SZXC_423_CA)) || (defined DIVI_CA))
#ifdef CA_SET_ECM_EMM_WAIT_LOCKED
ON_COMMAND(MSG_LOCK_RSL, on_locked_set_ecm_emm)
#endif 

#ifdef TENGRUI_ROLLTITLE
ON_COMMAND(MSG_START_TR_TITLE, on_start_roll_title)
#endif
#if ENABLE_TTX_SUBTITLE
ON_COMMAND(MSG_START_TTX, on_start_ttx)
ON_COMMAND(MSG_STOP_TTX, on_stop_ttx)
ON_COMMAND(MSG_STOP_TTX_AND_REC, on_stop_ttx_and_rec)
ON_COMMAND(MSG_TTX_KEY, on_ttx_key)
#endif
ON_COMMAND(MSG_POWER_OFF, on_power_off)
ON_COMMAND(MSG_POWER_ON, on_power_on)
ON_COMMAND(MSG_SWITCH_SCART, on_scart_switch)
ON_COMMAND(MSG_SCART_VCR_DETECTED, on_scart_vcr_detected)

ON_COMMAND(MSG_EPG_CHANG_TABLE_POLICY,on_epg_request_table_policy_switch)

#ifdef ENABLE_NETWORK
ON_COMMAND(MSG_INTERNET_PLUG_IN, on_ethernet_plug_in)
ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_ethernet_plug_out)
ON_COMMAND(MSG_CONFIG_IP, on_save_config_ip_to_ssdata)
//ON_COMMAND(MSG_GET_WIFI_AP_CNT, on_desktop_search_wifi_over)
ON_COMMAND(MSG_WIFI_AP_CONNECTING, on_desktop_wifi_connecting)
ON_COMMAND(MSG_WIFI_AP_CONNECT, on_save_config_ip_to_ssdata)
ON_COMMAND(MSG_WIFI_AP_DISCONNECT, on_desktop_connect_wifi_failed)
ON_COMMAND(MSG_GPRS_STATUS_UPDATE, on_desktop_gprs_status_update)
ON_COMMAND(MSG_G3_STATUS_UPDATE, on_desktop_g3_status_update)
ON_COMMAND(MSG_G3_READY, on_3g_dev_ready)

/*for dlna start*/
  ON_COMMAND(MSG_DLNA_START_PICTURE,on_dlna_start_pic)
  ON_COMMAND(MSG_DLNA_CLOSE_PICTURE,on_dlna_close_pic)
  ON_COMMAND(MSG_DLNA_START_VIDEO,on_dlna_start_video)
  ON_COMMAND(MSG_DLNA_CLOSE_VIDEO,on_dlna_close_video)
/*for dlna end*/

ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, on_desktop_load_media_success)
ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, on_desktop_load_media_error)
ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, on_desktop_load_media_exit)

ON_COMMAND(MSG_NET_DEV_MOUNT, on_net_ip_mount)
ON_COMMAND(MSG_NETWORK_MOUNT_SUCCESS, on_net_mount_success)
ON_COMMAND(MSG_NETWORK_MOUNT_FAIL, on_net_mount_fail)
ON_COMMAND(MSG_SATIP_CLIENT_REQ_CHANGE_PG, on_desktop_recieve_satip_client_msg)
 #if ENABLE_CUSTOMER_URL
 ON_COMMAND(MSG_OPEN_IPTV_CUS, on_open_iptv_at_fullscreen)
 #endif
#endif

ON_COMMAND(MSG_EDIT_UART, on_open_uart_ui)
ON_COMMAND(MSG_SWITCH_PN, on_switch_pn)
#if ENABLE_TTX_SUBTITLE
ON_COMMAND(MSG_SWITCH_SUBT, on_switch_subt)
#endif
ON_COMMAND(MSG_SWITCH_VIDEO_MODE, on_switch_video_mode)  
ON_COMMAND(MSG_SWITCH_ASPECT_MODE, on_switch_aspect_mode) 
ON_COMMAND(MSG_SWITCH_LANGUAGE, on_switch_language) 

#ifdef SLEEP_TIMER
ON_COMMAND(MSG_SLEEP_TMROUT, on_sleep_tmrout)
#endif
ON_COMMAND(MSG_START_RECORD, on_start_record)
ON_COMMAND(MSG_OTA_FOUND, on_ota_info_find)
ON_COMMAND(MSG_TIMER_OPEN, on_timer_open)
#ifdef ENABLE_ADS
ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
#ifdef BOYUAN_AD
ON_COMMAND(MSG_CORNER_LOGO_CLOSE, on_ads_corner_process)
ON_COMMAND(MSG_ADS_TO_UI, on_ads_process)
#endif
#if defined TEMP_SUPPORT_DS_AD || defined TEMP_SUPPORT_SZXC_AD
ON_COMMAND(MSG_ADS_TO_UI, on_ads_process)
#endif
#ifndef WIN32
#endif
#endif
ON_COMMAND(MSG_SLEEP, on_sleep)
#ifdef ENABLE_PVR_REC_CONFIG
ON_COMMAND(MSG_OPEN_RECORD_MANAGER, on_open_record_mannger)
#endif
ON_COMMAND(MSG_VERSION_NUMBER_UPDATE, on_nit_version_update)
ON_COMMAND(MSG_NIT_VERSION_UPDATE, on_software_update)
ON_COMMAND(MSG_SWITCH_PREVIEW, on_switch_preview)
ON_COMMAND(MSG_TV_FUNCTION, on_tv_radio_function)
ON_COMMAND(MSG_RADIO_FUNCTION, on_tv_radio_function)
ON_COMMAND(MSG_OTA_TMOUT, on_ota_timeout_start_play)
ON_COMMAND(MSG_OTA_TRIGGER_RESET, on_ota_reboot_into_ota)
ON_COMMAND(MSG_OTA_FINISH,on_ota_in_maincode_finsh)
ON_COMMAND(MSG_PRESET_PG_FOR_AE, on_preset_prog_for_ae_test)  // for ae test
ON_COMMAND(MSG_LOAD_PG_FROM_USB, on_load_prog_from_usb)  // for load pg from usb
#ifdef ONLY1_CA_VER
ON_COMMAND(MSG_EMON_SMC_FINGER, on_emon_test_finger)  
ON_COMMAND(MSG_EMON_HIDE_FINGER, on_emon_hide_finger)
#endif
#ifdef WIN32
ON_COMMAND(MSG_TEST1, on_test_1)
#endif
#ifdef FACTORY_TEST_SOFTWARE//if add this macro , need add ui_factory_test.c, for customer factory test
ON_COMMAND(MSG_FACT_TEST, on_factory_test)
ON_COMMAND(MSG_FACTORY_UART_TEST, Test_communication)
#endif
#ifdef NIT_SETTING
ON_COMMAND(MSG_DELEALL_AISAT, on_delete_all_aisat)
#endif
#ifdef CUSTOMER_YINHE_TR
ON_COMMAND(MSG_SYS_SET, on_open_sys_set)
#endif

#ifdef DALIAN_ZHUANGGUANG
ON_COMMAND(MSG_CORRECT_PWD,  on_factory_pwdlg_correct_divi)
ON_COMMAND(MSG_HOTKEY_BLUE,  on_restore_factory_divi)
ON_COMMAND(MSG_HOTKEY_RED,   on_ca_card_feed_divi)
ON_COMMAND(MSG_HOTKEY_YELLOW,on_prog_info_divi)
#endif

#ifdef JHC_CN_DS52
ON_COMMAND(MSG_KEY_F3, on_prog_info_jhc)
#endif

END_MSGPROC(ui_desktop_proc_on_normal, customer_manage_proc_on_normal_customer)


#if ENABLE_TTX_SUBTITLE
BEGIN_MSGPROC(ui_desktop_proc_on_ttx, cont_class_proc)
ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
ON_COMMAND(MSG_CLOSE_MENU, on_close_menu)
ON_COMMAND(MSG_STOP_TTX, on_stop_ttx)
ON_COMMAND(MSG_STOP_TTX_AND_REC, on_stop_ttx_and_rec)
ON_COMMAND(MSG_TTX_KEY, on_ttx_key)
ON_COMMAND(MSG_POWER_OFF, on_power_off)
#ifdef SLEEP_TIMER
ON_COMMAND(MSG_SLEEP_TMROUT, on_sleep_tmrout)
#endif
END_MSGPROC(ui_desktop_proc_on_ttx, cont_class_proc);
#endif

// start key mapping on normal
BEGIN_KEYMAP(ui_desktop_keymap_on_normal, customer_manage_keymap_on_normal_customer)
ON_EVENT(V_KEY_MUTE, MSG_MUTE)
ON_EVENT(V_KEY_PAUSE, MSG_PAUSE)
ON_EVENT(V_KEY_PLAY, MSG_PLAY)
ON_EVENT(V_KEY_MENU, MSG_OPEN_MENU_IN_TAB | ROOT_ID_MAINMENU)
ON_EVENT(V_KEY_UP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_DOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_CHUP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_CHDOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_PAGE_UP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_RECALL, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_TVRADIO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)


//ON_EVENT(V_KEY_0, MSG_OPEN_MENU_IN_TAB | ROOT_ID_TTX_HELP)

#ifdef FACTORY_TEST_SOFTWARE//if add this macro , need add ui_factory_test.c, for customer factory test
//ON_EVENT(V_KEY_1, MSG_FACT_TEST)//for kingvon factory test
//ON_EVENT(V_KEY_2, MSG_FACT_TEST)
//ON_EVENT(V_KEY_3, MSG_FACT_TEST)
//ON_EVENT(V_KEY_4, MSG_FACT_TEST)
//ON_EVENT(V_KEY_5, MSG_FACT_TEST)
//ON_EVENT(V_KEY_6, MSG_FACT_TEST)
//ON_EVENT(V_KEY_7, MSG_FACT_TEST)
//ON_EVENT(V_KEY_8, MSG_FACT_TEST)

ON_EVENT(V_FAC_KEY_01, MSG_FACT_TEST)
ON_EVENT(V_FAC_KEY_02, MSG_FACT_TEST)
ON_EVENT(V_FAC_KEY_03, MSG_FACT_TEST)
ON_EVENT(V_FAC_KEY_04, MSG_FACT_TEST)
ON_EVENT(V_FAC_KEY_05, MSG_FACT_TEST)
ON_EVENT(V_FAC_KEY_06, MSG_FACT_TEST)
ON_EVENT(V_FAC_KEY_07, MSG_FACT_TEST)
ON_EVENT(V_FAC_KEY_09, MSG_FACT_TEST)
#endif

#ifndef WIN32
ON_EVENT(V_KEY_1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_2, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_3, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_4, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_5, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_6, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_7, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_8, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_9, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
#else
ON_EVENT(V_KEY_1, MSG_TEST1)
ON_EVENT(V_KEY_2, MSG_TEST2)
ON_EVENT(V_KEY_3, MSG_TEST3)
ON_EVENT(V_KEY_4, MSG_TEST4)
ON_EVENT(V_KEY_5, MSG_TEST5)
ON_EVENT(V_KEY_6, MSG_TEST6)
ON_EVENT(V_KEY_7, MSG_TEST7)
ON_EVENT(V_KEY_8, MSG_TEST8)
ON_EVENT(V_KEY_9, MSG_TEST9)
ON_EVENT(V_KEY_0, MSG_TEST0)
#endif

ON_EVENT(V_KEY_LEFT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)
ON_EVENT(V_KEY_RIGHT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)

ON_EVENT(V_KEY_AUDIO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_AUDIO_SET)
ON_EVENT(V_KEY_ZOOM, MSG_OPEN_MENU_IN_TAB | ROOT_ID_ZOOM)
//ON_EVENT(V_KEY_BLUE, MSG_CA_SHOW_OSD)

ON_EVENT(V_KEY_VDOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)
ON_EVENT(V_KEY_VUP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)

ON_EVENT(V_KEY_OK, MSG_OPEN_MENU_IN_TAB | ROOT_ID_SMALL_LIST)
ON_EVENT(V_KEY_FAV, MSG_OPEN_MENU_IN_TAB | ROOT_ID_FAV_LIST)
ON_EVENT(V_KEY_FIND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_FIND)
ON_EVENT(V_KEY_SLEEP, MSG_SLEEP)  //MSG_OPEN_MENU_IN_TAB | ROOT_ID_SLEEP_HOTKEY)
ON_EVENT(V_KEY_BOOK, MSG_OPEN_MENU_IN_TAB |ROOT_ID_BOOK_LIST)  //MSG_OPEN_MENU_IN_TAB | ROOT_ID_SLEEP_HOTKEY)
ON_EVENT(V_KEY_LIST, MSG_OPEN_MENU_IN_TAB | ROOT_ID_SMALL_LIST)
ON_EVENT(V_KEY_EPG, MSG_OPEN_MENU_IN_TAB | ROOT_ID_EPG)
//ON_EVENT(V_KEY_BLUE, MSG_OPEN_MENU_IN_TAB | ROOT_ID_EPG)
//ON_EVENT(V_KEY_GREEN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)

#ifdef ONLY1_CA_VER
ON_EVENT(V_KEY_RED, MSG_DELEALL_AISAT)
#endif
ON_EVENT(V_KEY_SEARCH, MSG_OPEN_MENU_IN_TAB | ROOT_ID_AUTO_SEARCH)
ON_EVENT(V_KEY_GAME, MSG_OPEN_MENU_IN_TAB | ROOT_ID_GAME)
ON_EVENT(V_KEY_F2, MSG_OPEN_MENU_IN_TAB | ROOT_ID_USB_FILEPLAY)
ON_EVENT(V_KEY_F1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_USB_MUSIC)
ON_EVENT(V_KEY_RADIO, MSG_RADIO_FUNCTION)
ON_EVENT(V_KEY_TV, MSG_TV_FUNCTION)
ON_EVENT(V_KEY_TVRADIO, MSG_RADIO_FUNCTION)
#if ENABLE_MUSIC_PICTURE
//ON_EVENT(V_KEY_REC, MSG_START_RECORD)
ON_EVENT(V_KEY_LIST, MSG_OPEN_RECORD_MANAGER)
#endif
ON_EVENT(V_KEY_BISS_KEY, MSG_EDIT_UART)
#if ENABLE_TTX_SUBTITLE
ON_EVENT(V_KEY_TTX, MSG_START_TTX)
#endif
ON_EVENT(V_KEY_TVSAT, MSG_SWITCH_SCART)
ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
ON_EVENT(V_KEY_PN, MSG_SWITCH_PN)
ON_EVENT(V_KEY_SUBT, MSG_SWITCH_SUBT)
ON_EVENT(V_KEY_VIDEO_MODE, MSG_SWITCH_VIDEO_MODE)
ON_EVENT(V_KEY_ASPECT_MODE, MSG_SWITCH_ASPECT_MODE)
ON_EVENT(V_KEY_LANG, MSG_SWITCH_LANGUAGE)
ON_EVENT(V_KEY_PVR, MSG_OPEN_MENU_IN_TAB | ROOT_ID_RECORD_MANAGER)

ON_EVENT(V_KEY_D, MSG_OPEN_MENU_IN_TAB | ROOT_ID_TEST)
ON_EVENT(V_KEY_TV_GUIDE,MSG_PRESET_PG_FOR_AE) // for ae test
ON_EVENT(V_KEY_LOAD_PG, MSG_LOAD_PG_FROM_USB) // for pg integrate
#ifdef ENABLE_CA
ON_EVENT(V_KEY_MAIL,MSG_OPEN_MENU_IN_TAB | ROOT_ID_EMAIL_MESS)
#endif
#ifdef CUSTOMER_YINHE_TR
ON_EVENT(V_KEY_BLUE, MSG_SYS_SET)
ON_EVENT(V_KEY_YELLOW, MSG_OPEN_MENU_IN_TAB|ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_INFO, MSG_OPEN_MENU_IN_TAB|ROOT_ID_PRO_INFO)
#else
ON_EVENT(V_KEY_INFO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PRO_INFO)//ROOT_ID_PROG_BAR
#endif
#ifdef ZHILING_CUSTOMER_DESAI
ON_EVENT(V_KEY_RED, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
#endif

#ifdef DALIAN_ZHUANGGUANG
ON_EVENT(V_KEY_GREEN,MSG_HOTKEY_GREEN)
ON_EVENT(V_KEY_BLUE,MSG_HOTKEY_BLUE)
ON_EVENT(V_KEY_RED,MSG_HOTKEY_RED) 
ON_EVENT(V_KEY_YELLOW,MSG_HOTKEY_YELLOW)
#endif

#ifdef JHC_CN_DS52
ON_EVENT(V_KEY_F3, MSG_KEY_F3)
#endif

#ifdef JHC_DALIAN_ZHUANGGUANG
ON_EVENT(V_KEY_F3, MSG_OPEN_MENU_IN_TAB | ROOT_ID_EPG)
#endif
#if ENABLE_CUSTOMER_URL
ON_EVENT(V_KEY_PREV, MSG_OPEN_IPTV_CUS)
#endif

END_KEYMAP(ui_desktop_keymap_on_normal, customer_manage_keymap_on_normal_customer)


// start key mapping on standby
BEGIN_KEYMAP(ui_desktop_keymap_on_standby, NULL)
ON_EVENT(V_KEY_POWER, MSG_POWER_ON)
END_KEYMAP(ui_desktop_keymap_on_standby, NULL)

// start key mapping on vcr
BEGIN_KEYMAP(ui_desktop_keymap_on_vcr, NULL)
ON_EVENT(V_KEY_TVSAT, MSG_SWITCH_SCART)
ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(ui_desktop_keymap_on_vcr, NULL)

#if ENABLE_TTX_SUBTITLE
// start key mapping on teletext
BEGIN_KEYMAP(ui_desktop_keymap_on_ttx, NULL)
ON_EVENT(V_KEY_TTX, MSG_STOP_TTX)
ON_EVENT(V_KEY_MENU, MSG_STOP_TTX)
ON_EVENT(V_KEY_CANCEL, MSG_STOP_TTX)
ON_EVENT(V_KEY_EXIT, MSG_STOP_TTX)
ON_EVENT(V_KEY_BACK, MSG_STOP_TTX)
ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
//ON_EVENT(V_KEY_REC, MSG_STOP_TTX_AND_REC)

ON_EVENT(V_KEY_0, MSG_TTX_KEY | TTX_KEY_0)
ON_EVENT(V_KEY_1, MSG_TTX_KEY | TTX_KEY_1)
ON_EVENT(V_KEY_2, MSG_TTX_KEY | TTX_KEY_2)
ON_EVENT(V_KEY_3, MSG_TTX_KEY | TTX_KEY_3)
ON_EVENT(V_KEY_4, MSG_TTX_KEY | TTX_KEY_4)
ON_EVENT(V_KEY_5, MSG_TTX_KEY | TTX_KEY_5)
ON_EVENT(V_KEY_6, MSG_TTX_KEY | TTX_KEY_6)
ON_EVENT(V_KEY_7, MSG_TTX_KEY | TTX_KEY_7)
ON_EVENT(V_KEY_8, MSG_TTX_KEY | TTX_KEY_8)
ON_EVENT(V_KEY_9, MSG_TTX_KEY | TTX_KEY_9)

ON_EVENT(V_KEY_OK, MSG_TTX_KEY | TTX_KEY_TRANSPARENT)

ON_EVENT(V_KEY_UP, MSG_TTX_KEY | TTX_KEY_UP)
ON_EVENT(V_KEY_DOWN, MSG_TTX_KEY | TTX_KEY_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_TTX_KEY | TTX_KEY_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_TTX_KEY | TTX_KEY_RIGHT)
ON_EVENT(V_KEY_PAGE_UP, MSG_TTX_KEY | TTX_KEY_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_TTX_KEY | TTX_KEY_PAGE_DOWN)


ON_EVENT(V_KEY_RED, MSG_TTX_KEY | TTX_KEY_RED)
ON_EVENT(V_KEY_GREEN, MSG_TTX_KEY | TTX_KEY_GREEN)
ON_EVENT(V_KEY_YELLOW, MSG_TTX_KEY | TTX_KEY_YELLOW)
ON_EVENT(V_KEY_BLUE, MSG_TTX_KEY | TTX_KEY_CYAN)
END_KEYMAP(ui_desktop_keymap_on_ttx, NULL)
#endif

BEGIN_MSGPROC(background_roll_text_proc, text_class_proc)
  ON_COMMAND(MSG_ROLL_STOPPED, on_desktop_roll_finish)
END_MSGPROC(background_roll_text_proc, text_class_proc)



