/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"
#include "ui_pause.h"
#include "ui_mute.h"
#include "ap_framework.h"

#ifdef ENABLE_CA
#include "cas_manager.h"
#include "config_cas.h"
#include "ui_ca_public.h"
#endif
#ifdef ENABLE_ADS
#include "ui_ads_display.h"
#include "ui_ad_api.h"
#include "config_ads.h"
#endif
#ifdef QUICK_SEARCH_SUPPORT
#include "ap_table_monitor.h"
#endif
typedef struct
{
  u16 group;
  u8 mode;
  u16 prog_id;
  u8 is_scramble;
} play_info_t;

static play_info_t g_play_info = {0, CURN_MODE_NONE, INVALIDID, FALSE};
static play_info_t g_last_info = {0, CURN_MODE_NONE, INVALIDID, FALSE};

static u8 g_play_mode = CURN_MODE_NONE;

static BOOL g_is_playback_active = FALSE;

static BOOL g_is_playing = FALSE;
static BOOL g_is_blkscr = TRUE;
static BOOL g_is_encrypt = FALSE;
static BOOL g_have_pass_pwd = FALSE;
static BOOL g_is_block = FALSE;

static u8 boot_flag_for_pwd_ui = 0; /* 0: boot, 1: boot and set channel lock, 2: work */

static u8 g_play_timer_state = 0; /* 0: stopped, 1: running */
static u16 g_play_timer_owner = 0;
static u16 g_play_timer_notify = 0;
static u32 g_play_timer_tmout = 300;
static u8 ap_first_play = 0;
static u32 g_play_tp_freq = 0;
#ifdef DISPLAY_AND_VOLUME_AISAT
BOOL is_boot_aisat = TRUE;
#endif
#ifdef CA_SET_ECM_EMM_WAIT_LOCKED
static u8 locked_pg_id = 0xFF;
#endif

#ifdef ENABLE_NETWORK
static play_prg_type_t g_play_prg_type = SAT_PRG_TYPE;
#endif

struct
{
  BOOL is_chkpwd;
  BOOL is_passchk;
  u16 passchk_pg;
  BOOL age_lock;
} g_chkpwd_info = {TRUE, FALSE, INVALIDID, FALSE};

u16 ui_playback_evtmap(u32 event);

u16 pwdlg_keymap_in_scrn(u16 key);

u16 pwdlg_keymap_in_list(u16 key);

play_param_t g_pb_info;

RET_CODE pwdlg_proc_in_scrn(control_t *ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE pwdlg_proc_in_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
#if ENABLE_TTX_SUBTITLE
extern BOOL get_subt_description_1(class_handle_t handle, pb_subt_info_t *p_info, u16 pg_id);
#endif

#ifdef ENABLE_NETWORK
void ui_set_play_prg_type(play_prg_type_t prg_type)
{
  g_play_prg_type = prg_type;
}

play_prg_type_t ui_get_play_prg_type(void)
{
  return g_play_prg_type;
}
#endif

#if ENABLE_TTX_SUBTITLE
void ui_enable_vbi_insert(BOOL is_enable)
{
#if ENABLE_TTX_SUBTITLE
  cmd_t cmd;

  cmd.id = is_enable ? PB_CMD_START_VBI_INSERTER : PB_CMD_STOP_VBI_INSERTER;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
  OS_PRINTF("UI: %s vbi insert.........\n", is_enable ? "enable" : "disable");
#endif
}


BOOL ui_is_ttx_data_ready(u16 prog_id)
{
  pb_ttx_info_t ttx_info;
  class_handle_t handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);

  if(!get_ttx_description_1(handle, &ttx_info, prog_id))
  {
//  if(!pb_get_ttx_description(&ttx_info, prog_id))
//    OS_PRINTF("ui_is_ttx_data_ready: -----1\n");
    return FALSE;
  }

  if(ttx_info.service_cnt == 0)
  {
    OS_PRINTF("ui_is_ttx_data_ready: -----2\n");
    return FALSE;
  }
  else
  {
    s32 i = ttx_info.service_cnt;
    for( ; i > 0; i--)
    {
      OS_PRINTF("Type[%d], i[%d]\n", ttx_info.service[i - 1].type, i);
      if(ttx_info.service[i - 1].type == TTX_INITIAL_PAGE
        || ttx_info.service[i - 1].type == TTX_ADDITIONAL_INFO_PAGE
        || ttx_info.service[i - 1].type == TTX_PROG_SCHEDULE_PAGE)
      {
        return TRUE;
      }
    }

    OS_PRINTF("ui_is_ttx_data_ready: -----3\n");
    return FALSE;
  }
}


BOOL ui_is_subt_data_ready(u16 prog_id)
{
  pb_subt_info_t subt_info;
  class_handle_t handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);

  if(!get_subt_description_1(handle, &subt_info, prog_id))
  {
//  if (!pb_get_subt_description(&subt_info, prog_id))
    return FALSE;
  }

  if(subt_info.service_cnt == 0)
  {
    return FALSE;
  }
  else
  {
    s32 i = subt_info.service_cnt;
    for( ; i > 0; i--)
    {
      if(subt_info.service[i - 1].type != SUBT_TYPE_UNDEF)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

// ETSI EN 300 706 V1.2.1 (2003-04) p103
u8 ui_set_ttx_lang_cb_fun(u8 charset_code, u8 national, BOOL charset_flag, BOOL national_flag)
{
  if(!charset_flag)
  {
    charset_code = 0x10; //set default charset as latin
  }

  if(!national_flag)
  {
    national = 0x06; //set default language as Turket
  }

  return ((charset_code & ~7) + national);
}

void ui_enable_ttx(BOOL is_enable)
{
#if ENABLE_TTX_SUBTITLE //portingUI temporary
  cmd_t cmd;

  cmd.id = is_enable ? PB_CMD_START_TTX : PB_CMD_STOP_TTX;
  cmd.data1 = (TELTEXT_MAX_PAGE_NUM_1 << 16) | TELTEXT_MAX_PAGE_NUM_2;
  cmd.data2 = (u32)(&ui_set_ttx_lang_cb_fun);

  ap_frm_do_command(APP_PLAYBACK, &cmd);

  OS_PRINTF("UI: %s ttx.........\n", is_enable ? "enable" : "disable");
#endif
}


void ui_post_ttx_key(u32 key)
{
#if ENABLE_TTX_SUBTITLE
    cmd_t cmd;
  
    cmd.id = PB_CMD_POST_TTX_KEY;
    cmd.data1 = key;
    cmd.data2 = 0;
  
    ap_frm_do_command(APP_PLAYBACK, &cmd);
  
    OS_PRINTF("UI: post ttx key ......... [%d]\n", key);
  #endif

}


void ui_set_ttx_display(BOOL is_display, u32 regino_handle, s32 magzine,
                        s32 page)
{
#if ENABLE_TTX_SUBTITLE
    cmd_t cmd;
  
    if(is_display)
    {
      cmd.id = PB_CMD_SHOW_TTX;
      cmd.data1 = regino_handle;
  
      if(magzine < 0
        || page < 0)
      {
        cmd.data2 = INVALID_TTX_PAGE;
      }
      else
      {
        cmd.data2 = magzine * 256 + page;
      }
    }
    else
    {
      cmd.id = PB_CMD_HIDE_TTX;
    }
  
    ap_frm_do_command(APP_PLAYBACK, &cmd);
  
    OS_PRINTF("UI: %s ttx display.........\n", is_display ? "enable" : "disable");
  #endif
}


void ui_set_ttx_language(char *lang)
{
#if ENABLE_TTX_SUBTITLE
    cmd_t cmd;
    u16 b_index = iso_639_2_desc_to_idx(lang);
    u16 t_index = iso_639_2_desc_to_idx(lang);
  
    MT_ASSERT(strlen(lang) == 3 || strlen(lang) == 2);
  
    cmd.id = PB_CMD_SET_TTX_LANG;
    cmd.data1 = MT_MAKE_DWORD(b_index, t_index);
    cmd.data2 = 0;
  
    ap_frm_do_command(APP_PLAYBACK, &cmd);
  
    OS_PRINTF("UI: set ttx language [%s].........\n", lang);
  #endif

}


static BOOL is_enable_subt_on_setting(void)
{
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  return (BOOL)(osd_set.enable_subtitle);
}


BOOL is_enable_vbi_on_setting(void)
{
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  return (BOOL)(osd_set.enable_vbinserter);
}


#endif

BOOL is_ap_playback_active(void)
{
  return g_is_playback_active;
}

void ui_enable_playback(BOOL is_enable)
{
  cmd_t cmd;
  cmd.data1 = APP_PLAYBACK;
  cmd.data2 = 0;

  if(is_enable)
  {
    OS_PRINTF("------------->>active ap playback\n");
    cmd.id = AP_FRM_CMD_ACTIVATE_APP;

    ap_frm_do_command(APP_FRAMEWORK, &cmd);

    fw_register_ap_evtmap(APP_PLAYBACK, ui_playback_evtmap);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_BAR);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_LIST);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_BACKGROUND);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_PVR_REC_BAR);

    g_is_playback_active = TRUE;
    g_is_playing = FALSE;
#ifndef WIN32
  #ifdef ENABLE_CA

    OS_PRINTF("@@@@ Dexin init ca when leave search !!!CAS_ID = %d  \n", CAS_ID);
  
    if((CAS_ID == CONFIG_CAS_ID_DS) || (CAS_ID == CONFIG_CAS_ID_SC))
    {
      OS_PRINTF("@@@@ Dexin init ca when leave search !!! \n");
      ui_init_ca();
    }
  #endif
#endif
  }
  else
  {
    cmd.id = AP_FRM_CMD_DEACTIVATE_APP;

    ap_frm_do_command(APP_FRAMEWORK, &cmd);

    fw_unregister_ap_evtmap(APP_PLAYBACK);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_BAR);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_LIST);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_BACKGROUND);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_PVR_REC_BAR);

    g_is_playback_active = FALSE;
#ifndef WIN32
  #ifdef ENABLE_CA
    if((CAS_ID == CONFIG_CAS_ID_DS) || (CAS_ID == CONFIG_CAS_ID_SC))
    {
      OS_PRINTF("@@@@ Dexin release ca when do auto search !!! \n");
      ui_release_ca();
    }
    else
    {
      cas_manager_stop_ca_play();
    }
  #endif
#endif
  }

  OS_PRINTF("UI: %s playback.........\n", is_enable ? "active" : "deactive");
}


void ui_pause_play(void)
{
  cmd_t cmd;

  cmd.id = PB_CMD_PAUSE;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void ui_resume_play(void)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESUME;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void ui_stop_play(stop_mode_t type, BOOL is_sync)
{
  cmd_t cmd;

  //stop cas, free ecm filter
#ifdef ENABLE_CA
  if(type != STOP_PLAY_FREEZE)
    cas_manager_stop_ca_play();
#endif

  cmd.id = is_sync ? PB_CMD_STOP_SYNC : PB_CMD_STOP;
  cmd.data1 = type;
  cmd.data2 = 0;
  OS_PRINTF("stop type[%d]\n", type);
  ap_frm_do_command(APP_PLAYBACK, &cmd);
  // set flag
  g_is_playing = FALSE;
  g_is_blkscr = (BOOL)(type == STOP_PLAY_BLACK);

#if ENABLE_TTX_SUBTITLE
  // subt display off
  OS_PRINTF("Before ui_set_subt_display\n");
  ui_set_subt_display(FALSE);
  OS_PRINTF("After ui_set_subt_display\n");
  ui_enable_subt(FALSE);
  OS_PRINTF("Leave ui_enable_subt\n");
  ui_enable_ttx(FALSE);
  // stop insert
  if(is_enable_vbi_on_setting())
  {
    ui_enable_vbi_insert(FALSE);
  }
#endif
}

#ifdef SMSX_CA
void smsx_ca_stop(void)
{
  u16 curn_group;
  u16 prog_id;
  u8 curn_mode;
  u32 context;
  dvbs_prog_node_t pg_node = {0};
  
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  if(curn_mode == CURN_MODE_NONE)
  {
    sys_status_check_group();
    curn_mode = sys_status_get_curn_prog_mode();
    if(curn_mode == CURN_MODE_NONE)
    {
      return;
    }
  }
//  ui_play_prog(ui_sys_get_curn_prog(view), FALSE);
  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);
    if(prog_id == INVALID)
    {
      return ;
    }
    db_dvbs_get_pg_by_id(g_play_info.prog_id, &pg_node);
    if(pg_node.is_scrambled)
    {
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
    }
    
}
#endif

#if ENABLE_TTX_SUBTITLE
void ui_enable_subt(BOOL is_enable)
{
#if ENABLE_TTX_SUBTITLE
  //if (is_enable_subt_on_setting())
  {
    cmd_t cmd;

    cmd.id = is_enable ? PB_CMD_START_SUBT : PB_CMD_STOP_SUBT_SYNC;
    cmd.data1 = 0;
    cmd.data2 = 0;

    ap_frm_do_command(APP_PLAYBACK, &cmd);

    OS_PRINTF("UI: %s subt.........\n", is_enable ? "enable" : "disable");
  }
 #endif
}


void ui_set_subt_display(BOOL is_display)
{
  if(is_enable_subt_on_setting())
  {
    cmd_t cmd;

    cmd.id = is_display ? PB_CMD_SHOW_SUBT : PB_CMD_HIDE_SUBT;
    cmd.data1 = 0;
    cmd.data2 = 0;

    ap_frm_do_command(APP_PLAYBACK, &cmd);

    OS_PRINTF("UI: %s subt display.........\n",
              is_display ? "enable" : "disable");
  }
}


void ui_set_subt_service(u8 lang_idx, u32 type)
{
  //if (is_enable_subt_on_setting())
  {
    cmd_t cmd;
    char **p_lang_code_list_b = sys_status_get_lang_code(TRUE);
    char **p_lang_code_list_t = sys_status_get_lang_code(FALSE);
    char *p_cur_lang_code_b = p_lang_code_list_b[lang_idx];
    char *p_cur_lang_code_t = p_lang_code_list_t[lang_idx];
    u16 b_index = iso_639_2_desc_to_idx(p_cur_lang_code_b);
    u16 t_index = iso_639_2_desc_to_idx(p_cur_lang_code_t);

    cmd.id = PB_CMD_SET_SUBT_SERVICE;
    cmd.data1 = MT_MAKE_DWORD(b_index, t_index);
    cmd.data2 = type;

    ap_frm_do_command(APP_PLAYBACK, &cmd);
  }
}

void ui_set_subt_service_1(char *lang_code, u32 type)
{
  //if (is_enable_subt_on_setting())
  {
    cmd_t cmd;
    u16 b_index = iso_639_2_desc_to_idx(lang_code);
    u16 t_index = iso_639_2_desc_to_idx(lang_code);

    cmd.id = PB_CMD_SET_SUBT_SERVICE;
    cmd.data1 = MT_MAKE_DWORD(b_index, t_index);
    cmd.data2 = type;

    ap_frm_do_command(APP_PLAYBACK, &cmd);

  }
}
#endif

BOOL ui_get_app_playback_status(void)
{
  return g_is_playback_active;
}

BOOL ui_is_playing(void)
{
  return g_is_playing;
}


BOOL ui_is_blkscr(void)
{
  return g_is_blkscr;
}

BOOL ui_is_have_pass_pwd(void)
{
  return g_have_pass_pwd;
}

void ui_set_have_pass_pwd(BOOL flag)
{
  g_have_pass_pwd = flag;
}

u32 ui_get_playing_tp(void)
{  
   return g_play_tp_freq;
}

static void play_pg_with_pwd(u32 prog_id)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y - 25, //-25 for fix bug 18201
    IDS_MSG_INPUT_PASSWORD,
    NULL,
    NULL,
    PWDLG_T_COMMON
  };

  u8 root_id = fw_get_focus_id();

  if(root_id == ROOT_ID_PROG_BAR)
  {
    pwdlg_data.parent_root = ROOT_ID_BACKGROUND;
  }
  else if(root_id == ROOT_ID_NUM_PLAY)
  {
    manage_close_menu(ROOT_ID_NUM_PLAY, 0, 0);
  }

  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL
    && boot_flag_for_pwd_ui == 0)                                            // already popup
  {
    UI_PRINTF("UI: already open a dialog, force close it! \n");
    ui_comm_dlg_close();
  }
  //if (ui_sys_get_curn_prog_in_curn_view() == prog_id)
  if(sys_status_get_curn_group_curn_prog_id() == prog_id)
  {
    if(root_id == ROOT_ID_PROG_LIST
      || root_id == ROOT_ID_SMALL_LIST
      || root_id == ROOT_ID_FAV_EDIT
      || root_id == ROOT_ID_EPG
      )
    {
      pwdlg_data.keymap = pwdlg_keymap_in_list;
      pwdlg_data.proc = pwdlg_proc_in_list;
    }
    else
    {
      pwdlg_data.keymap = pwdlg_keymap_in_scrn;
      pwdlg_data.proc = pwdlg_proc_in_scrn;
    }


    ui_comm_pwdlg_open(&pwdlg_data);
  }
}


#if ENABLE_TTX_SUBTITLE
/*
  * Just display subtitle on fullscreen
  */
static void fullscreen_subtitle_display(void)
{
  if(is_enable_subt_on_setting())
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
      ui_set_subt_display(TRUE);
    }
    else
    {
      ui_set_subt_display(FALSE);
    }
  }
}


#endif

#if 1
//lint -e438 -e550 -e830
void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};

  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  p_info->ecm_num = (u8)(pg.ecm_num < 8 ? pg.ecm_num : 8);
  p_info->emm_num = (u8)(tp.emm_num < 8 ? tp.emm_num : 8);

#if 1 //ecm_num support
  memcpy(p_info->ecm_info, pg.cas_ecm, p_info->ecm_num * sizeof(cas_desc_t));

  memcpy(p_info->emm_info, tp.emm_info, p_info->emm_num * sizeof(ca_desc_t));

  if (((CAS_ID == CONFIG_CAS_ID_DS)|| (CAS_ID == CONFIG_CAS_ID_SC)) && (p_info->ecm_num == 0))
  {
    OS_PRINTF("@@@ set unscrambled pg ecm pid 0x1FFF !!!\n");
    p_info->ecm_num = 1;
    p_info->ecm_info[0].ecm_pid= 0x1FFF;
  }
#endif
}
//lint +e438 +e550 +e830

#endif

static void do_play_pg(u16 pg_id)
{
  cmd_t cmd;
  language_set_t lang_set;

#if 0 //#ifdef CA_SUPPORT
  cas_sid_t ca_sid;
#endif
  av_set_t av_set = {0};
#if ENABLE_TTX_SUBTITLE
  char **p_lang_code = sys_status_get_lang_code(FALSE);
#endif

  // check pause status
  if(ui_is_pause())
  {
    UI_PRINTF("DO_PLAY_PG: ui_set_pause(FALSE)\n");
    ui_set_pause(FALSE);
  }

  // show scramble mark or not
  update_signal();

  OS_PRINTF("DO_PLAY_PG:  id =%d \n", pg_id);
  load_play_paramter_by_pgid(&g_pb_info, pg_id);

#ifdef CA_SET_ECM_EMM_WAIT_LOCKED
  locked_pg_id = pg_id;
#endif

  //if set DIGITAL_OUTPUT as BS out,  change audio type
  sys_status_get_av_set(&av_set);

   // 1 means BS out
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == g_pb_info.pg_info.audio_type)
    ||(AUDIO_EAC3 == g_pb_info.pg_info.audio_type)))
  {
    g_pb_info.pg_info.audio_type = AUDIO_SPDIF;
  }

  //first play flag
  ap_first_play = 1;

  // to play
  cmd.id = PB_CMD_PLAY;
  cmd.data1 = (u32) & g_pb_info;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);

  #ifdef UPDATE_PG_BACKGROUND
  ui_table_monitor_stop_bat(FALSE, FALSE);
  #endif
  
  #ifdef QUICK_SEARCH_SUPPORT
  ui_table_monitor_stop_sdt_otr(FALSE, FALSE);
  #endif

#ifdef ENABLE_CA
#ifndef CA_SET_ECM_EMM_WAIT_LOCKED
  cas_manager_set_sid(pg_id);
  cas_manager_dvb_monitor_start(pg_id);
  cas_manager_set_pvr_forbid(FALSE);
#endif
#endif

  ui_epg_set_program_info(pg_id);

  sys_status_get_lang_set(&lang_set);
#if ENABLE_TTX_SUBTITLE
  // set teletext language
  ui_enable_ttx(TRUE);
  ui_set_ttx_language(p_lang_code[lang_set.tel_text]);
  // subt display on
  ui_enable_subt(TRUE);
  ui_set_subt_service(lang_set.sub_title, SUBT_TYPE_NORMAL);
  fullscreen_subtitle_display();
  // start insert
  if(is_enable_vbi_on_setting())
  {
    ui_enable_vbi_insert(TRUE);
  }
#endif
}

void ui_reset_tvmode(u32 av_mode)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESET_TVMODE;
  cmd.data1 = av_mode;
  cmd.data2 = 0;

  g_pb_info.pg_info.tv_mode = (u16)av_mode;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

#ifdef CA_SET_ECM_EMM_WAIT_LOCKED
void locked_set_emm_ecm(void)
{
  OS_PRINTF("\n %s,%d,pg_id:%d",__FUNCTION__,__LINE__,locked_pg_id);
  cas_manager_set_sid(locked_pg_id);
  cas_manager_dvb_monitor_start(locked_pg_id);
  cas_manager_set_pvr_forbid(FALSE);
}
#endif

void ui_reset_video_aspect_mode(u32 aspect)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESET_VIDEO_ASPECT_MODE;
  cmd.data1 = aspect;
  cmd.data2 = 0;

  g_pb_info.pg_info.aspect_ratio = aspect;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void do_lock_pg(u16 pg_id)
{
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;

  if(db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
    return;
  }

  if(db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
    return;
  }

}


static BOOL do_chk_age_limit(u16 prog_id);

static BOOL do_chk_age_limit(u16 prog_id)
{
  BOOL is_age_limit = FALSE;

    
  OS_PRINTF("\n is_age_limit[%d] \n", is_age_limit);
  return is_age_limit;
}

BOOL ui_is_prog_block()
{
	return g_is_block;
}


static BOOL do_chk_pg(u16 prog_id)
{
  u8 view_id;
  u16 pos;
  BOOL is_ss_chk;
  BOOL is_lock = FALSE, is_age_limit = FALSE;

  view_id = ui_dbase_get_pg_view_id();
  pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);


  if(ui_is_chk_pwd())
  {
    sys_status_get_status(BS_PROG_LOCK, &is_ss_chk);
    if (is_ss_chk)
    {
      is_lock = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0);
      is_age_limit = do_chk_age_limit(prog_id);
      if(is_lock || is_age_limit)
      {
        if (!ui_is_pass_chkpwd(prog_id))
        {
          if (prog_id != g_chkpwd_info.passchk_pg)
          {
            if(fw_find_root_by_id(ROOT_ID_SMALL_LIST) == NULL)
            {
              ui_set_chk_pwd(prog_id, (is_lock||is_age_limit)?FALSE:TRUE);
              ui_set_age_lock(is_age_limit);
              play_pg_with_pwd(prog_id);
            }
            // TODO: fw_post_msg(MSG_CHK_PWD, prog_id, 0);
          }
          return FALSE;
        }
      }
    }
  }



  // reset flag
  ui_set_chk_pwd(prog_id, TRUE);
  ui_set_age_lock(FALSE);

  return TRUE;
}

void trans_lock_tp_info(dvbc_lock_info_t *p_lock_info, dvbs_tp_node_t *p_tp)
{
  p_lock_info->tp_freq = p_tp->freq;
  p_lock_info->tp_sym = p_tp->sym;
  p_lock_info->nim_modulate = (nim_modulation_t)p_tp->nim_modulate;
}

void trans_tp_info(tp_rcv_para_t *p_tp_rcv, dvbs_tp_node_t *p_tp)
{
  p_tp_rcv->sym = p_tp->sym;
  p_tp_rcv->freq = p_tp->freq;
  p_tp_rcv->nim_modulate = (nim_modulation_t)p_tp->nim_modulate;
  p_tp_rcv->nim_type = p_tp->nim_type;
}

void trans_dtmb_tp_info(dvbt_lock_info_t *p_lock_info, dvbs_tp_node_t *p_tp)
{
  p_lock_info->band_width= p_tp->sym;
  p_lock_info->tp_freq = p_tp->freq;
  p_lock_info->nim_type = p_tp->nim_type;
}


//lint -e732
void trans_pg_info(dvbs_program_t *p_pg_info, dvbs_prog_node_t *p_pg)
{
  av_set_t av_set = {0};
  u32 cur_audio_ch, new_audio_ch = 0;
  u16 audio_pid = 0;
  u16 audio_type = AUDIO_MP2;
  audio_set_t audio_set = {0};
#ifdef DISPLAY_AND_VOLUME_AISAT  
  BOOL is_default_volume = FALSE;
#endif
  sys_status_get_av_set(&av_set);
  new_audio_ch = get_audio_channel(p_pg);

  if((p_pg->audio_channel != (u16)new_audio_ch)
    && !p_pg->is_audio_channel_modify)
  {
    p_pg->audio_channel = new_audio_ch;
    db_dvbs_edit_program(p_pg);
  }
  cur_audio_ch = p_pg->audio_channel;

  audio_pid = p_pg->audio[cur_audio_ch].p_id;
  audio_type = p_pg->audio[cur_audio_ch].type;

  p_pg_info->s_id = (u16)p_pg->s_id;
  p_pg_info->is_scrambled = p_pg->is_scrambled;
  p_pg_info->tv_mode = sys_status_get_sd_mode(av_set.tv_mode);

  p_pg_info->v_pid = p_pg->video_pid;
  p_pg_info->video_type = (u8)p_pg->video_type;
  p_pg_info->pcr_pid = p_pg->pcr_pid;
  p_pg_info->a_pid = audio_pid;
  p_pg_info->audio_type = audio_type;
  //p_pg_info->audio_track = (u16)p_pg->audio_track;
  //p_pg_info->audio_volume = (u16)p_pg->volume;
  p_pg_info->pmt_pid = (u16)p_pg->pmt_pid;
  p_pg_info->aspect_ratio = sys_status_get_video_aspect(av_set.tv_ratio);
  p_pg_info->context1 = p_pg->id;
 #ifdef DISPLAY_AND_VOLUME_AISAT  
  sys_status_get_status(BS_DEFAULT_VOLUME, &is_default_volume);
 #endif
  sys_status_get_audio_set(&audio_set);
  if(audio_set.is_global_volume)
  {
    p_pg_info->audio_volume = audio_set.global_volume;
    p_pg->volume = (u32)p_pg_info->audio_volume;
  }
  else
  {
    p_pg_info->audio_volume = p_pg->volume;
  }
 #ifdef DISPLAY_AND_VOLUME_AISAT  
  if(is_default_volume == TRUE && is_boot_aisat == TRUE)
  {
     p_pg_info->audio_volume = 4;
     p_pg->volume = 4;	 
     audio_set.global_volume = 4;
     sys_status_set_audio_set(&audio_set);
     sys_status_save();
     is_boot_aisat = FALSE;	 
  }
  #endif
  if(audio_set.is_global_track)
  {
    p_pg_info->audio_track = audio_set.global_track;
    p_pg->audio_track = (u32)p_pg_info->audio_track;
  }
  else
  {
    p_pg_info->audio_track = p_pg->audio_track;
  }
}
//lint +e732
//lint -e438 -e550 -e830
void load_play_paramter_by_pgid(play_param_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};

  memset(p_info, 0, sizeof(play_param_t));

  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
  {
    p_info->lock_mode = SYS_DTMB;
    trans_dtmb_tp_info(&(p_info->inner.dvbt_lock_info), &tp);
  }
  else
  {
    p_info->lock_mode = SYS_DVBC;
    trans_lock_tp_info(&p_info->inner.dvbc_lock_info, &tp);
  }
  g_play_tp_freq = tp.freq;
  trans_pg_info(&p_info->pg_info, &pg);
}
//lint +e438 +e550 +e830

//recall
BOOL ui_recall(BOOL is_play, u16 *p_curn)
{
  u16 view_type;
  u32 context;
  dvbs_prog_node_t last_pg;

  // init
  *p_curn = g_play_info.prog_id;
  if(db_dvbs_get_pg_by_id(g_last_info.prog_id, &last_pg) == DB_DVBS_FAILED)
  {
    return FALSE;
  }

  // recreate view for recall
  if((g_last_info.group != g_play_info.group)
    || (g_last_info.mode != g_play_info.mode))
  {
    sys_status_set_curn_prog_mode(g_last_info.mode);
    sys_status_set_curn_group(g_last_info.group);
    sys_status_save();

    sys_status_get_curn_view_info(&view_type, &context);
    ui_dbase_set_pg_view_id(ui_dbase_create_view((dvbs_view_t)view_type, context, NULL));
  }

  *p_curn = g_last_info.prog_id;
  if(is_play)
  {
    return ui_play_prog(*p_curn, FALSE);
  }
  return TRUE;
}


//switch between tv and radio
/*
** If is_play is true, a new view will be created, or you need to create view by your hand ouside this function.
** 
**
*/
BOOL ui_tvradio_switch(BOOL is_play, u16 *p_curn)
{
  u16 org_group;
  u8 curn_mode, org_mode;
  u8 view_id;
  u16 current_pg_id;
  u32 context;
  u16 view_type;
  u16 prog_pos;
  BOOL is_success = FALSE;

  // init
  *p_curn = g_play_info.prog_id;
  org_mode = sys_status_get_curn_prog_mode();
  org_group = sys_status_get_curn_group();

  switch(org_mode)
  {
    case CURN_MODE_TV:
      curn_mode = CURN_MODE_RADIO;
      break;
    case CURN_MODE_RADIO:
      curn_mode = CURN_MODE_TV;
      break;
    default:
      return FALSE;
  }

  ui_cache_view();
  sys_status_set_curn_prog_mode(curn_mode);
  sys_status_save();
  do
  {
    // We get current program from all goup.
    sys_status_get_group_all_info(curn_mode, &current_pg_id);
    if (current_pg_id == INVALIDID)
    {
      break;
    }
    sys_status_get_curn_view_info(&view_type, &context);
    //if(view_type == DB_DVBS_FAV_TV ||view_type == DB_DVBS_FAV_RADIO)
    //{
    //  view_id = ui_dbase_create_view(DB_DVBS_FAV_ALL, org_group - DB_DVBS_MAX_SAT - 1, NULL);
    //}
    //else
    //{
    //  view_id = ui_dbase_create_view((dvbs_view_t)view_type, context, NULL);
    //}
    view_id = ui_dbase_create_view((dvbs_view_t)view_type, context, NULL);//bug 58697
    ui_dbase_set_pg_view_id(view_id);
    prog_pos = db_dvbs_get_view_pos_by_id(view_id, current_pg_id);
    if (prog_pos == INVALIDID)
    {
      u16 count;
      
      count = db_dvbs_get_count(view_id);
      if (count > 0)
      {
        // If the current service is not in current view, then return the frist program in current view.
        current_pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
      }
      else
      {
        break;
      }
    }

    is_success = TRUE;
  }while (0);

  if (!is_success)
  {
    ui_restore_view();
    return FALSE;
  }

  //close pause menu.
  if (ui_is_pause())
  {
    ui_set_pause(FALSE);
  }

  *p_curn = current_pg_id;
  if (is_play)
  {
    return ui_play_prog(*p_curn, FALSE);
  }
  return TRUE;
}


BOOL ui_dvbc_change_view(dvbs_view_t view_type, BOOL is_play)
{
	u16 org_group, pos_in_set;
	u8 curn_mode, org_mode, group_type;
	u8 view_id;
	u16 pg_id = INVALIDID;
	u16 pg_pos = INVALIDPOS;
	u32 context;
	BOOL ret = FALSE;
	dvbs_prog_node_t prog = {0};

	// init
	org_mode = sys_status_get_curn_prog_mode();
	org_group = sys_status_get_curn_group();

	if(org_group != GROUP_T_ALL)
	{
	MT_ASSERT(0);
	}

	switch(view_type)
	{
	case DB_DVBS_ALL_TV:
	  curn_mode = CURN_MODE_TV;
	  if(org_mode == CURN_MODE_TV)
	  {
	    //return TRUE;
	  }
	  break;

	case DB_DVBS_ALL_RADIO:
	  curn_mode = CURN_MODE_RADIO;
	  if(org_mode == CURN_MODE_RADIO)
	  {
	    //return TRUE;
	  }
	  break;

	case DB_DVBS_ALL_PG:
	case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
	case DB_DVBS_TV_RADIO:
	case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
	  curn_mode = org_mode;
	  break;

	case DB_DVBS_FAV_ALL:
	  curn_mode = org_mode;
	  break;

	default:
	  curn_mode = org_mode;
	  break;
	}

	sys_status_get_group_info(org_group, &group_type, &pos_in_set, &context);

	switch (group_type)
	{
	case GROUP_T_ALL:
	  if (!sys_status_get_group_all_info(curn_mode, &pg_id))
	  {
	    pg_id = INVALIDID, pg_pos = INVALIDPOS;
	  }
	  break;

	default:
	  MT_ASSERT(0);
	  return FALSE;
	}

	if (pg_id == INVALIDID || pg_pos == INVALIDPOS)
	{
	view_id = ui_dbase_create_view(view_type, 0, NULL);
	ui_dbase_set_pg_view_id(view_id);

	if(db_dvbs_get_count(view_id)>0)
	{
	  pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
	  pg_pos = 0;
	  
	  db_dvbs_get_pg_by_id(pg_id, &prog);
	  
	  if((BOOL)prog.tv_flag)
	  {
	    curn_mode = CURN_MODE_TV;
	  }
	  else
	  {
	    curn_mode = CURN_MODE_RADIO;
	  }

	  sys_status_set_curn_prog_mode(curn_mode);
	  sys_status_set_curn_group_info(pg_id, pg_pos);
	  //sys_status_save();

	  if(is_play)
	  {
	    ui_play_prog(pg_id, FALSE);
	  }

	  ret = TRUE;
	}
	else
	{
	  return ret;
	}
	}
	else
	{
	view_id = ui_dbase_create_view(view_type, 0, NULL);
	ui_dbase_set_pg_view_id(view_id);

	//set sys info
	pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);

	if(pg_id == INVALIDID || pg_pos == INVALIDPOS) 
	{
	  if(db_dvbs_get_count(view_id)>0)
	  {
	    pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
	    pg_pos = 0;
	    
	    db_dvbs_get_pg_by_id(pg_id, &prog);
	    
	    if((BOOL)prog.tv_flag)
	    {
	      curn_mode = CURN_MODE_TV;
	    }
	    else
	    {
	      curn_mode = CURN_MODE_RADIO;
	    }
	  }
	  else
	  {
	    return ret;
	  }
	}

	sys_status_set_curn_prog_mode(curn_mode);
	sys_status_set_curn_group_info(pg_id, pg_pos);
	//sys_status_save();
	view_id = ui_dbase_create_view(view_type, context, NULL);
	ui_dbase_set_pg_view_id(view_id);

	if(is_play)
	{
	  ui_play_prog(pg_id, is_play);
	}

	ret = TRUE;
	}  

	//if((view_type != DB_DVBS_ALL_PG) && (view_type != DB_DVBS_ALL_PG))
	//{
	//  sys_status_save();
	//}

	return ret;
}

void ui_shift_prog(s16 offset, BOOL is_play, u16 *p_curn)
{
  play_set_t play_set;
  dvbs_prog_node_t pg;
  u16 prog_id, prev_prog, total_prog;
  u16 curn_prog;
  u8 curn_mode, org_mode;  
  s32 dividend = 0;
  u8 curn_view = ui_dbase_get_pg_view_id();
  BOOL is_force_skip = FALSE;

  // init
  *p_curn = g_play_info.prog_id;

  sys_status_get_play_set(&play_set);

  total_prog = db_dvbs_get_count(curn_view);
  prog_id = sys_status_get_curn_group_curn_prog_id();
  curn_prog = prev_prog = db_dvbs_get_view_pos_by_id(
                curn_view, prog_id);

  if(total_prog == 0)
  {
    *p_curn = INVALIDID;
    return;
  }

  do
  {
    dividend = curn_prog + offset;

 #if 0 //fix bug 15865
    while(dividend < 0)
    {
      dividend += total_prog;
    }
    curn_prog = (u16)(dividend) % total_prog;
  #else
    if(dividend >= total_prog)
    {
      if(curn_prog == (total_prog - 1))
      {
        curn_prog = 0;
      }
      else
      {
        curn_prog = total_prog - 1;
      }
    }
    else
    {
      if(dividend <= 0)
      {
        if(curn_prog == 0)
        {
          curn_prog = total_prog - 1;
        }
        else
        {
          curn_prog = 0;
        }
      }
      else
      {
        curn_prog = (u16)(dividend) % total_prog;
      }
    }
  #endif

    /* pos 2 id */
    prog_id = db_dvbs_get_id_by_view_pos(curn_view, curn_prog);
    MT_ASSERT(prog_id != INVALIDID);
    if(prev_prog == curn_prog)  /* all is skip*/
    {
      return;
    }

    // check play type
    db_dvbs_get_pg_by_id(prog_id, &pg);
    OS_PRINTF("pg is scramble = %d, play type = %d,prog_id = %d \n",
              pg.is_scrambled, play_set.type,prog_id);

    switch(play_set.type)
    {
      case 1: // only play free pg
        is_force_skip = (BOOL)(pg.is_scrambled == 1);
        break;

      case 2: // only play scramble pg
        is_force_skip = (BOOL)(pg.is_scrambled == 0);
        break;

      default:
        ;
    }
  }
  while(is_force_skip
       || db_dvbs_get_mark_status(curn_view, curn_prog,
                                  DB_DVBS_MARK_SKP, FALSE));

  //tv radio switch
  org_mode = sys_status_get_curn_prog_mode();
  curn_mode = (pg.tv_flag) ? CURN_MODE_TV : CURN_MODE_RADIO;
  if(org_mode != curn_mode)
  {
    sys_status_set_curn_prog_mode(curn_mode);
    sys_status_save();
  }

  *p_curn = prog_id;
  sys_status_set_curn_group_info(prog_id, curn_prog);  
  if(is_play)
  {
    ui_play_prog(*p_curn, FALSE);
  }
}


void ui_play_set_cw(u8 *p_data, u8 key_length)
{
  dmx_chanid_t p_channel_a = 0, p_channel_v = 0;
  dmx_device_t *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  MT_ASSERT(p_dev != NULL);

  p_data[3] = p_data[0] + p_data[1] + p_data[2];
  p_data[7] = p_data[4] + p_data[5] + p_data[6];

  /* set video cw */
  dmx_get_chanid_bypid(p_dev, g_pb_info.pg_info.v_pid, &p_channel_v);
  dmx_descrambler_onoff(p_dev, p_channel_v, FALSE);
  dmx_descrambler_onoff(p_dev, p_channel_v, TRUE);
  dmx_descrambler_set_even_keys(p_dev, p_channel_v, p_data, key_length);
  dmx_descrambler_set_odd_keys(p_dev, p_channel_v, p_data, key_length);

  /* set audio cw */
  dmx_get_chanid_bypid(p_dev, g_pb_info.pg_info.a_pid, &p_channel_a);
  dmx_descrambler_onoff(p_dev, p_channel_a, FALSE);
  dmx_descrambler_onoff(p_dev, p_channel_a, TRUE);
  dmx_descrambler_set_even_keys(p_dev, p_channel_a, p_data, key_length);
  dmx_descrambler_set_odd_keys(p_dev, p_channel_a, p_data, key_length);
}


void ui_set_frontpanel_by_curn_pg(void)
{
  u16 pos;

  if(g_play_info.prog_id != INVALIDID)
  {
    pos = db_dvbs_get_view_pos_by_id(
      ui_dbase_get_pg_view_id(), g_play_info.prog_id);
    ui_set_front_panel_by_num(pos + 1);
  }
  else
  {
    ui_set_front_panel_by_str("----");
  }
}


static RET_CODE on_pwdlg_correct_in_list(control_t *p_ctrl, u16 msg,
                                         u32 para1, u32 para2)
{
  /* if get correct pwd then don't check again in list
     and enable it when leave list */
  //ui_enable_chk_pwd(FALSE);
  g_chkpwd_info.is_passchk = TRUE;
  /* to play */
  do_play_pg(g_chkpwd_info.passchk_pg);

  ui_comm_pwdlg_close();

  ui_set_age_lock(FALSE);
  update_signal();
  //ui_set_have_pass_pwd(TRUE);
  if(fw_find_root_by_id(ROOT_ID_SMALL_LIST) != NULL)
  {
    manage_close_menu(ROOT_ID_SMALL_LIST, 0, 0);
  }

  return SUCCESS;
}


static RET_CODE on_pwdlg_correct_in_scrn(control_t *p_ctrl, u16 msg,
                                         u32 para1, u32 para2)
{

  u8 root_id = fw_get_focus_id();
  
  g_chkpwd_info.is_passchk = TRUE;
  /* to play */
  do_play_pg(g_chkpwd_info.passchk_pg);

  ui_comm_pwdlg_close();

  ui_set_age_lock(FALSE);
  update_signal();
  //ui_set_have_pass_pwd(TRUE);
  
  root_id = fw_get_focus_id();
  
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL) 
  {
    manage_tmr_reset();
  }
  else
  {
    if(root_id != ROOT_ID_EPG 
      &&  root_id != ROOT_ID_FAV_EDIT 
      && root_id != ROOT_ID_CHANNEL_EDIT)
      {
        manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
      }
  }

  return SUCCESS;
}


static RET_CODE on_pwdlg_exit(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  g_chkpwd_info.is_passchk = FALSE;


  /* only lock */
  do_lock_pg(g_chkpwd_info.passchk_pg);
  ui_comm_pwdlg_close();

  update_signal();
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL) 
  {
    manage_tmr_reset();
  }
  return SUCCESS;
}


static RET_CODE on_pwdlg_save(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  g_chkpwd_info.is_passchk = FALSE;
  /* only lock */
  do_lock_pg(g_chkpwd_info.passchk_pg);

  update_signal();

  return SUCCESS;
}


static void pass_key_to_parent(u16 msg)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    case MSG_FOCUS_LEFT:
      key = V_KEY_LEFT;
      break;
    case MSG_FOCUS_RIGHT:
      key = V_KEY_RIGHT;
      break;
    case MSG_PAGE_UP:
      key = V_KEY_PAGE_UP;
      break;
    case MSG_PAGE_DOWN:
      key = V_KEY_PAGE_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);
}


static RET_CODE on_pwdlg_cancel_in_list(control_t *p_ctrl, u16 msg,
                                        u32 para1, u32 para2)
{
  u8 curn_view = ui_dbase_get_pg_view_id();
  u16 total_prog;
  total_prog = db_dvbs_get_count(curn_view);
  g_chkpwd_info.is_passchk = FALSE;
  /* only lock */
//  do_lock_pg(g_chkpwd_info.passchk_pg);
//  update_signal();

  pass_key_to_parent(msg);
  ui_comm_pwdlg_close();
  if(total_prog == 1)
  {
    update_signal();
  }
  return SUCCESS;
}

static void pass_key_to_progbar(u16 msg)
{
  u16 key;
  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    case MSG_FOCUS_LEFT:
      key = V_KEY_LEFT;
      break;
    case MSG_FOCUS_RIGHT:
      key = V_KEY_RIGHT;
      break;
    case MSG_PAGE_UP:
      key = V_KEY_PAGE_UP;
      break;
    case MSG_PAGE_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_root(fw_find_root_by_id(ROOT_ID_PROG_BAR), NOTIFY_T_KEY, FALSE, key, 0, 0);
}
static RET_CODE on_pwdlg_cancel_in_scrn(control_t *p_ctrl, u16 msg,
                                        u32 para1, u32 para2)
{
  u8 curn_view = ui_dbase_get_pg_view_id();
  u16 total_prog;
  total_prog = db_dvbs_get_count(curn_view);

  g_chkpwd_info.is_passchk = FALSE;
  /* only lock */
//  do_lock_pg(g_chkpwd_info.passchk_pg);
//  update_signal(); 
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) == NULL) 
  {
    manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
  }

  pass_key_to_progbar(msg);
  ui_comm_pwdlg_close();
  
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL) 
  {
    manage_tmr_reset();
  }
  if(total_prog == 1)
  {
    update_signal();
  }
  return SUCCESS;
}


//play current channel in current mode
BOOL ui_play_curn_pg(void)
{
  u16 curn_group;
  u16 prog_id;
  u8 curn_mode;
  u32 context;

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  if(curn_mode == CURN_MODE_NONE)
  {
    sys_status_check_group();
    curn_mode = sys_status_get_curn_prog_mode();
    if(curn_mode == CURN_MODE_NONE)
    {
      //ui_stop_play(STOP_PLAY_BLACK, TRUE);
      ui_set_front_panel_by_str("----");
      return FALSE;
    }
  }
//  ui_play_prog(ui_sys_get_curn_prog(view), FALSE);
  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);
  ui_play_prog(prog_id, FALSE);
  return TRUE;
}


BOOL ui_is_prog_encrypt(void)
{
  return g_is_encrypt;
}

static BOOL ui_fast_play(u16 pg_id, stop_mode_t stop_mode)
{
  cmd_t cmd;
  language_set_t lang_set;
  av_set_t av_set = {0};

#if ENABLE_TTX_SUBTITLE
  char **p_lang_code = sys_status_get_lang_code(FALSE);
#endif

  // check pause status
  if(ui_is_pause())
  {
    ui_set_pause(FALSE);
  }

  // show scramble mark or not
  update_signal();

  load_play_paramter_by_pgid(&g_pb_info, pg_id);

  //if set DIGITAL_OUTPUT as BS out,  change audio type
  sys_status_get_av_set(&av_set);
	
#ifdef CA_SET_ECM_EMM_WAIT_LOCKED
  locked_pg_id = pg_id;
#endif

  if(ap_first_play == 0)
  {
    cmd.id = PB_CMD_STOP;
    cmd.data1 = STOP_PLAY_BLACK;

    ap_frm_do_command(APP_PLAYBACK, &cmd);
    ap_first_play = 1;
  }
  // 1 means BS out
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == g_pb_info.pg_info.audio_type)
    ||(AUDIO_EAC3 == g_pb_info.pg_info.audio_type)))
  {
    g_pb_info.pg_info.audio_type = AUDIO_SPDIF;
  }

  // to play
  cmd.id = PB_CMD_SWITCH_PG;
  cmd.data1 = (u32) & g_pb_info;
  cmd.data2 = stop_mode;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
  
  #ifdef UPDATE_PG_BACKGROUND
  ui_table_monitor_stop_bat(FALSE, FALSE);
  #endif
  
  #ifdef QUICK_SEARCH_SUPPORT
  ui_table_monitor_stop_sdt_otr(FALSE, FALSE);
  #endif

#ifdef ENABLE_CA
#ifndef CA_SET_ECM_EMM_WAIT_LOCKED
  cas_manager_set_sid(pg_id);
  cas_manager_dvb_monitor_start(pg_id);
  cas_manager_set_pvr_forbid(FALSE);
#endif
#endif

  ui_epg_set_program_info(pg_id);
  sys_status_get_lang_set(&lang_set);

#if ENABLE_TTX_SUBTITLE
  // set teletext language
  ui_enable_ttx(TRUE);
  ui_set_ttx_language(p_lang_code[lang_set.tel_text]);
  // subt display on
  ui_enable_subt(TRUE);
  ui_set_subt_service(lang_set.sub_title, SUBT_TYPE_NORMAL);
  fullscreen_subtitle_display();
  // start insert
  if(is_enable_vbi_on_setting())
  {
    ui_enable_vbi_insert(TRUE);
  }
#endif
#ifdef TEMP_SUPPORT_DS_AD
  if(FALSE == get_force_channel_status())
  {
    ui_adv_display_osd_msg(pg_id);
  }
  else
  {
    UI_PRINTF("[play]Forcr channel !!! \n");
  }
#endif
#ifdef TEMP_SUPPORT_SZXC_AD
  ui_check_osd_ad_play();
#endif
  return TRUE;
}

#ifdef SMSX_CA
//extern BOOL g_force_email_flag;
//extern void sv_show_forceemail_again(void);
#endif

//lint -e553
BOOL ui_play_prog(u16 prog_id, BOOL is_force)
{
  dvbs_prog_node_t pg;
  BOOL is_switch = FALSE;
  u8 curn_mode;
  u16 prog_pos;
  play_set_t play_set;
  BOOL is_playing = ui_is_playing();
  BOOL is_sync = FALSE, is_show_logo = FALSE;
  stop_mode_t stop_mode = STOP_PLAY_NONE;
  dvbs_prog_node_t pg_node = {0};
  BOOL result;
#ifdef CAS_CONFIG_CDCAS
  static u8 reboot_flag = 1;
  static u8 switch_pg_flag = 0;
#endif
#ifdef SMSX_CA
//static BOOL g_force_email_start = TRUE;
#endif
  if(prog_id == INVALIDID)
  {
    return FALSE;
  }

    // check for play
  if((g_play_info.prog_id == prog_id)
    && !is_force
    && ui_is_playing()
    && ui_is_pass_chkpwd(prog_id))
  {
    #if ENABLE_TTX_SUBTITLE
    fullscreen_subtitle_display();
    #endif
    if (g_play_info.group != sys_status_get_curn_group())
    {
      // same service but group changed, then we set corresponding group for current service.
      prog_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), prog_id);
      if (prog_pos != INVALIDID)
      {
        sys_status_set_curn_group_info(prog_id, prog_pos);
      }
      else
      {
        OS_PRINTF("Warning, set current group info failed,prgid:%d,pos:%d\n",prog_id, prog_pos);
      }
    }
    OS_PRINTF("@@same pg return \n");
    return FALSE;
  }

    /* clean the playing biss/cw set parameter */
  db_dvbs_get_pg_by_id(g_play_info.prog_id, &pg_node);

 // clean_pg_scramble();

  if(db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    return FALSE;
  }
  #ifdef MIS_ADS
  //set ads info
  ads_set_tp_pg_info(&pg);
  #endif

  // set sys info
  curn_mode = sys_status_get_curn_prog_mode();
  if(curn_mode != g_play_mode)
  {
    is_switch = TRUE;
    g_play_mode = curn_mode;
  }

  // just stop before play
  sys_status_get_play_set(&play_set);
  if(is_playing)
  {
    if(is_switch)
    {
      if(g_play_mode == CURN_MODE_RADIO)
      {
        stop_mode = STOP_PLAY_BLACK;
        is_sync = TRUE;
      }
      else
      {
        stop_mode = play_set.mode ? STOP_PLAY_BLACK : STOP_PLAY_FREEZE;
        is_sync = TRUE;
      }
    }
    else
    {
      if(curn_mode == CURN_MODE_RADIO)
      {
        stop_mode = STOP_PLAY_NONE;
        is_sync = FALSE;
      }
      else
      {
        stop_mode = play_set.mode ? STOP_PLAY_BLACK : STOP_PLAY_FREEZE;
        is_sync = FALSE;
      }
    }
  }
  else
  {
    stop_mode = play_set.mode ? STOP_PLAY_BLACK : STOP_PLAY_FREEZE;
    is_sync = TRUE;
  }

  //disable for temp ,waiting for Rick upadte  //fixed
  if(curn_mode == CURN_MODE_RADIO)
  {
    if(is_switch
      || !is_playing)
    {
      is_show_logo = TRUE;
    }
  }

  // set play info
  if(g_play_info.prog_id != prog_id)
  {
    memcpy(&g_last_info, &g_play_info, sizeof(play_info_t));
    // update scrambe info
    g_play_info.is_scramble = (u8)pg.is_scrambled;
    g_is_encrypt = (BOOL)pg.is_scrambled;
    #ifdef CAS_CONFIG_CDCAS
    switch_pg_flag = 1;
    #endif
  }
  else
  {
  #ifdef CAS_CONFIG_CDCAS
    switch_pg_flag = 0;
  #endif
  }

  #ifdef DIVI_CA
  {
    extern BOOL ui_is_need_clear_display(void);
    extern BOOL ui_is_invalid_smart_card(void);

    if(ui_is_need_clear_display())
    {
      update_ca_message(RSC_INVALID_ID);
    }
    OS_PRINTF("ui_is_smart_card_insert:%d, ui_is_invalid_smart_card:%d\n", ui_is_smart_card_insert(), ui_is_invalid_smart_card());
    if(!ui_is_smart_card_insert())
    {
      if(pg.is_scrambled)
      {
        update_ca_message(IDS_INSERT_SMC);
      }
    }
    else
    {
      if(ui_is_invalid_smart_card())
      {
		  if(pg.is_scrambled)
		  {
	      	update_ca_message(IDS_CAS_E_ILLEGAL_CARD2);
		  }
      }
    }
  }
  #endif

  // set play info
  g_play_info.group = sys_status_get_curn_group();
  g_play_info.mode = curn_mode;
  g_play_info.prog_id = prog_id;

  //set sys info
  prog_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), g_play_info.prog_id);

  sys_status_set_curn_group_info(g_play_info.prog_id, prog_pos);
  if (g_play_info.group != 0)// Aways set all group current program.
  {
    sys_status_set_group_all_info(curn_mode, g_play_info.prog_id, prog_pos);
  }
  sys_bg_data_save();
  // set frontpanel
  ui_set_frontpanel_by_curn_pg();

  // try to close pwd dlg
  ui_comm_pwdlg_close();

  //close eneitle info 
  #if(CONFIG_CAS_ID != CONFIG_CAS_ID_ADT_MG)
  if(fw_find_root_by_id(ROOT_ID_CA_ENTITLE_EXP_DLG) != NULL)
  {
    UI_PRINTF("UI:  force close ROOT_ID_CA_ENTITLE_EXP_DLG ! \n");
    close_ca_entitle_expire_dlg();
  }
  #endif
  #ifdef SMSX_CA
  //if(g_force_email_flag &&g_force_email_start )
  //{
    //sv_show_forceemail_again();
    //g_force_email_start = FALSE;
  //}
  //else
  //{
    //g_force_email_flag = TRUE;
    //g_force_email_start = FALSE;
  //}
  #endif

  
  if(is_show_logo)
  {
    OS_PRINTF("show logo, normal play!!!\n");
    ui_stop_play(stop_mode, is_sync);

  #ifdef ENABLE_ADS
    result = ui_adv_pic_play(ADS_AD_TYPE_LOGO,ROOT_ID_BACKGROUND);
  #else
    result = FALSE;
  #endif
    if (!result)
    {
      ui_show_logo(LOGO_BLOCK_ID_M1);
    }

    if(do_chk_pg(g_play_info.prog_id))
    {
      do_play_pg(g_play_info.prog_id);
    }
  }
  else
  {
    if(do_chk_pg(g_play_info.prog_id))
    {
      OS_PRINTF("default, fast play!!!\n");
      //stop cas, free ecm filter
#ifdef ENABLE_CA
      cas_manager_stop_ca_play();
#endif
      ui_fast_play(g_play_info.prog_id, stop_mode);
      if(!ui_get_curpg_scramble_flag())
      {
        if((CAS_ID == CONFIG_CAS_ID_ADT_MG) ||(CAS_ID == CONFIG_CAS_ID_DS) || (CAS_ID == CONFIG_CAS_ID_SC))
          ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_EVT_NOTIFY, 0, CAS_S_CLEAR_DISPLAY);
        else
        {
		  #ifndef DIVI_CA
	      OS_PRINTF("\nupdate ca invalid\n");
          update_ca_message(RSC_INVALID_ID);
		  #endif
        }
      }
      else
      {
        if(CAS_ID == CONFIG_CAS_ID_UNITEND)
          ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_EVT_NOTIFY, 0, CAS_S_CLEAR_DISPLAY);
      }
    }
    else
    {
      OS_PRINTF("pwd check, normal play!!!\n");

      ui_stop_play(stop_mode, is_sync);
    }
  }

  #if 0
  if(g_play_mode == CURN_MODE_RADIO)
  {
    update_encrypt_message_radio();
  }
  #endif
  
  #ifdef CAS_CONFIG_CDCAS
  {
     if(reboot_flag || switch_pg_flag)
     {
       fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, FALSE, MSG_WATCH_LIMIT_REBOOT, 0, 0);
       reboot_flag = 0;
     }
  }
  #endif

  g_is_playing = TRUE;

  return TRUE;
}
//lint +e553

//clear last prog(for recall) and passwordchecked prog(for password dialog)
void ui_clear_play_history(void)
{
  g_last_info.group = 0;
  g_last_info.mode = CURN_MODE_NONE;
  g_last_info.prog_id = INVALIDID;

  g_chkpwd_info.is_chkpwd = TRUE;
  g_chkpwd_info.is_passchk = FALSE;
  g_chkpwd_info.passchk_pg = INVALIDID;
}


void ui_reset_chkpwd(void)
{
  g_chkpwd_info.is_passchk = FALSE;
  g_chkpwd_info.passchk_pg = INVALIDID;
}


BOOL ui_is_pass_chkpwd(u16 prog_id)
{
  if(prog_id == g_chkpwd_info.passchk_pg)
  {
    return g_chkpwd_info.is_passchk;
  }

  return FALSE;
}


void ui_enable_chk_pwd(BOOL is_enable)
{
  g_chkpwd_info.is_chkpwd = is_enable;
}


BOOL ui_is_chk_pwd(void)
{
  return g_chkpwd_info.is_chkpwd;
}


BOOL ui_is_age_lock(void)
{
  return g_chkpwd_info.age_lock;
}


void ui_set_age_lock(BOOL is_pass)
{
  g_chkpwd_info.age_lock = is_pass;
}


void ui_set_chk_pwd(u16 prog_id, BOOL is_pass)
{
  g_chkpwd_info.is_passchk = is_pass;
  g_chkpwd_info.passchk_pg = prog_id;
}


BOOL ui_is_tvradio_switch(u16 pgid)
{
  dvbs_prog_node_t pg;
  u8 curn_mode = sys_status_get_curn_prog_mode();

  db_dvbs_get_pg_by_id(pgid, &pg);

  if(((curn_mode == CURN_MODE_TV)
     && (pg.video_pid == 0))
    || ((curn_mode == CURN_MODE_RADIO)
       && (pg.video_pid != 0)))
  {
    return TRUE;
  }

  return FALSE;
}

static u16 ui_find_prog_pos_in_cur_group(dvbs_prog_node_t *p_prog)
{
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 count = db_dvbs_get_count(view_id);
  u16 i, pg_id, pg_new_pos = INVALIDPOS;
  dvbs_prog_node_t prog = {0};

  if(count > 0)
  {
    for(i=0; i<db_dvbs_get_count(view_id); i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      db_dvbs_get_pg_by_id(pg_id, &prog);

      if(p_prog->s_id == prog.s_id)
      {
        pg_new_pos = i;
        break;
      }
    }
#if 0
    if(i == db_dvbs_get_count(view_id))
    {
      pg_new_pos = 0;
    }
    else
    {
      pg_new_pos = i;
    }
#endif	
  }

  return pg_new_pos;
}

BOOL ui_india_change_group(dvbs_view_t view_type, u16 group, BOOL is_play)
{
  u16 org_group, pos_in_set;
  u8 curn_mode, org_mode, old_group_type, new_group_type;
  u8 view_id;
  u16 pg_id, pg_new_pos;
  u32 context;
  BOOL ret = FALSE;
  dvbs_prog_node_t old_prog = {0};
  dvbs_prog_node_t prog = {0};

  // init
  org_mode = sys_status_get_curn_prog_mode();
  org_group = sys_status_get_curn_group();

  switch(view_type)
  {
    case DB_DVBS_ALL_TV:
      curn_mode = CURN_MODE_TV;
      break;

    case DB_DVBS_ALL_RADIO:
      curn_mode = CURN_MODE_RADIO;
      break;

    case DB_DVBS_ALL_PG:
    case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TV_RADIO:
    case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
      curn_mode = org_mode;
      break;

    case DB_DVBS_FAV_ALL:
      curn_mode = org_mode;
      break;

    default:
      curn_mode = org_mode;
      break;
  }
  
  sys_status_get_group_info(org_group, &old_group_type, &pos_in_set, &context);
  
  switch (old_group_type)
  {
    case GROUP_T_ALL:
      if (!sys_status_get_group_all_info(curn_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;

    case GROUP_T_FAV:
      if (!sys_status_get_fav_group_info(pos_in_set, org_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;

    default:
      MT_ASSERT(0);
      return FALSE;
  }
  
  if(group == 0)
  {
    new_group_type = GROUP_T_ALL;
  }
  else
  {
    new_group_type = GROUP_T_FAV;
  }

  if((pg_id != INVALIDID) )
  {
    db_dvbs_get_pg_by_id(pg_id, &old_prog);
  }

  view_id = ui_dbase_create_view(view_type, 0, NULL);
  ui_dbase_set_pg_view_id(view_id);

  if(db_dvbs_get_count(view_id)>0)
  {
    pg_new_pos = ui_find_prog_pos_in_cur_group(&old_prog);
    
    pg_id = db_dvbs_get_id_by_view_pos(view_id, pg_new_pos);
    db_dvbs_get_pg_by_id(pg_id, &prog);
          
    if((BOOL)prog.tv_flag)
    {
      curn_mode = CURN_MODE_TV;
    }
    else
    {
      curn_mode = CURN_MODE_RADIO;
    }

    //sys_status_set_group_all_info(pg_id, pg_pos);
    sys_status_set_curn_group(group);
    sys_status_set_curn_prog_mode(curn_mode);
    sys_status_set_curn_group_info(pg_id, pg_new_pos);
    //sys_status_save();
  
    if(is_play)
    {
      ui_play_prog(pg_id, FALSE);
    }

    ret = TRUE;
  }
  else
  {
    return ret;
  }
  
  return ret;
}

BOOL ui_is_playpg_scrambled(void)
{
  return g_play_info.is_scramble;
}

BOOL ui_get_curpg_scramble_flag(void)
{
  dvbs_prog_node_t pg;
  if(g_play_info.prog_id != INVALIDID)
  {
    db_dvbs_get_pg_by_id(g_play_info.prog_id, &pg);
    return pg.is_scrambled;
  }
  return FALSE;
}


void ui_set_playpg_scrambled(BOOL is_scramble)
{
  if(g_play_info.prog_id != INVALIDID)
  {
    g_play_info.is_scramble = (u8)is_scramble;
  }
}

//lint -e438 -e550 -e830
void ui_book_play(u16 pgid)
{
  u16 prog_id;
  u8 mode, view_id;
  BOOL ret;
  BOOL need_create_view = FALSE;

  if(sys_status_get_curn_group_curn_prog_id() != pgid)
  {
    //need tv/radio switch?
    if(ui_is_tvradio_switch(pgid))
    {
      ret = ui_tvradio_switch(FALSE, &prog_id);
      MT_ASSERT(ret == TRUE);
      need_create_view = TRUE;
    }

    //need change group? if current group isn't GROUP_ALL, change group.
    if(sys_status_get_curn_group() != 0)
    {
      //set the group as group all.
      sys_status_set_curn_group(0);
      need_create_view = TRUE;
    }
    if (need_create_view)
    {
      //create new view.
      mode = sys_status_get_curn_prog_mode();
      view_id = ui_dbase_create_view(
        (mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, 0, NULL);

      ui_dbase_set_pg_view_id(view_id);
    }
  }

  ui_play_prog(pgid, FALSE);
}
//lint +e438 +e550 +e830

void ui_init_play_timer(u16 owner, u16 notify, u32 tmout)
{
  g_play_timer_state = 0;
  g_play_timer_owner = owner;
  g_play_timer_notify = notify;
  g_play_timer_tmout = tmout;
}

void ui_deinit_play_timer(void)
{
  if(g_play_timer_state == 1)
  {
    fw_tmr_destroy(g_play_timer_owner, g_play_timer_notify);
	g_play_timer_state = 0;
  }
}

void ui_play_timer_start(void)
{
  RET_CODE ret = ERR_FAILURE;
  OS_PRINTF("%s,%d,g_play_timer_state:%d \n",__FUNCTION__,__LINE__,g_play_timer_state);
  if(g_play_timer_state == 0)
  {
    ret = fw_tmr_create(g_play_timer_owner, g_play_timer_notify, g_play_timer_tmout, FALSE);
	MT_ASSERT(ret == SUCCESS);
    g_play_timer_state = 1;
  }
  else
  {
    ret = fw_tmr_reset(g_play_timer_owner, g_play_timer_notify, g_play_timer_tmout);
	//MT_ASSERT(ret == SUCCESS);
  }
}

void ui_play_timer_set_state(u8 state)
{
  g_play_timer_state = state;
}

BOOL ui_play_api_get_pb_info_scramble()
{
  return g_pb_info.pg_info.is_scrambled;
}

BEGIN_MSGPROC(pwdlg_proc_in_scrn, cont_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel_in_scrn)
ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel_in_scrn)
ON_COMMAND(MSG_PAGE_UP, on_pwdlg_cancel_in_scrn)
ON_COMMAND(MSG_PAGE_DOWN, on_pwdlg_cancel_in_scrn)
ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct_in_scrn)
ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
ON_COMMAND(MSG_SAVE, on_pwdlg_save)
END_MSGPROC(pwdlg_proc_in_scrn, cont_class_proc)

BEGIN_MSGPROC(pwdlg_proc_in_list, cont_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_PAGE_UP, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_PAGE_DOWN, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_FOCUS_LEFT, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_FOCUS_RIGHT, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct_in_list)
ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
ON_COMMAND(MSG_SAVE, on_pwdlg_save)
END_MSGPROC(pwdlg_proc_in_list, cont_class_proc)


BEGIN_KEYMAP(pwdlg_keymap_in_scrn, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_CHUP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_CHDOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
ON_EVENT(V_KEY_BACK, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(pwdlg_keymap_in_scrn, NULL)

BEGIN_KEYMAP(pwdlg_keymap_in_list, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
ON_EVENT(V_KEY_BACK, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(pwdlg_keymap_in_list, NULL)

BEGIN_AP_EVTMAP(ui_playback_evtmap)
CONVERT_EVENT(PB_EVT_TTX_READY, MSG_TTX_READY)
CONVERT_EVENT(PB_EVT_SUB_READY, MSG_SUBT_READY)
CONVERT_EVENT(PB_EVT_DYNAMIC_PID, MSG_DYNC_PID_UPDATE)
CONVERT_EVENT(PB_EVT_UPDATE_VERSION, MSG_VERSION_NUMBER_UPDATE)
CONVERT_EVENT(PB_EVT_NIT_VERSION_SWITCH, MSG_NIT_VERSION_UPDATE)
CONVERT_EVENT(PB_EVT_UPDATE_PG_NAME, MSG_PROGRAM_NAME_UPDATE)
CONVERT_EVENT(PB_DESCRAMBL_SUCCESS, MSG_DESCRAMBLE_SUCCESS)
CONVERT_EVENT(PB_DESCRAMBL_FAILED, MSG_DESCRAMBLE_FAILED)
CONVERT_EVENT(PB_EVT_SCART_VCR_DETECTED, MSG_SCART_VCR_DETECTED)
CONVERT_EVENT(PB_EVT_NOTIFY_VIDEO_FORMAT, MSG_NOTIFY_VIDEO_FORMAT)
#ifdef CA_SET_ECM_EMM_WAIT_LOCKED
CONVERT_EVENT(PB_EVT_LOCK_RSL, MSG_LOCK_RSL)
#endif
END_AP_EVTMAP(ui_playback_evtmap)
