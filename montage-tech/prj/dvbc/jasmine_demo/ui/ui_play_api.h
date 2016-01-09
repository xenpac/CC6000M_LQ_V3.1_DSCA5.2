/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __AP_PLAY_API_H__
#define __AP_PLAY_API_H__

#define PWD_DLG_FOR_PLAY_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define PWD_DLG_FOR_PLAY_Y    ((SCREEN_HEIGHT-PWDLG_H)/2)

enum playback_msg
{
  MSG_TTX_READY = MSG_EXTERN_BEGIN + 450,
  MSG_SUBT_READY,
  MSG_DESCRAMBLE_SUCCESS,
  MSG_DESCRAMBLE_FAILED,
  MSG_DYNC_PID_UPDATE,
  MSG_SCART_VCR_DETECTED,
  MSG_LOCK_RSL,
  MSG_VERSION_NUMBER_UPDATE,
  MSG_PROGRAM_NAME_UPDATE,
  MSG_NIT_VERSION_UPDATE,
  MSG_NOTIFY_VIDEO_FORMAT,
  MSG_BOOK_UPDATE,
};

BOOL is_ap_playback_active(void);

#ifdef ENABLE_NETWORK
typedef enum
{
  SAT_PRG_TYPE = 0,
  NET_PRG_TYPE = 1,
  CAMERA_PRG_TYPE = 2,
} play_prg_type_t;
#endif

BOOL ui_recall(BOOL is_play, u16 *p_curn);

void ui_shift_prog(s16 offset, BOOL is_play, u16 *p_curn);

BOOL ui_tvradio_switch(BOOL is_play, u16 *p_curn);

BOOL ui_dvbc_change_view(dvbs_view_t view_type, BOOL is_play);

BOOL ui_play_prog(u16 prog_id, BOOL is_force);

void ui_stop_play(stop_mode_t type, BOOL is_sync);

BOOL ui_play_api_get_pb_info_scramble();

void ui_pause_play(void);

void ui_resume_play(void);

void ui_enable_playback(BOOL is_enable);

void ui_clear_play_history(void);

void ui_reset_chkpwd(void);

BOOL ui_play_curn_pg(void);

void ui_set_frontpanel_by_curn_pg(void);

BOOL ui_is_pass_chkpwd(u16 prog_id);

void ui_enable_chk_pwd(BOOL is_enable);

BOOL ui_is_chk_pwd(void);

void ui_set_chk_pwd(u16 prog_id, BOOL is_pass);

BOOL ui_is_age_lock(void);

void ui_set_age_lock(BOOL is_pass);

BOOL ui_is_playing(void);

BOOL ui_is_blkscr(void);

BOOL ui_is_have_pass_pwd(void);

void ui_set_have_pass_pwd(BOOL flag);

u32 ui_get_playing_tp(void);
// teletext operation
void ui_enable_vbi_insert(BOOL is_enable);

BOOL ui_is_ttx_data_ready(u16 prog_id);

void ui_enable_ttx(BOOL is_enable);

void ui_post_ttx_key(u32 key);

void ui_set_ttx_language(char *lang);

void ui_set_ttx_display(BOOL is_on, u32 regino_handle, s32 magzine, s32 page);

// subtitle operation
void ui_enable_subt(BOOL is_enable);

void ui_set_subt_service(u8 lang_idx, u32 type);
void ui_set_subt_service_1(char *lang_code, u32 type);

void ui_set_subt_display(BOOL is_display);

BOOL ui_is_subt_data_ready(u16 prog_id);

BOOL ui_is_tvradio_switch(u16 pgid);

BOOL ui_is_playpg_scrambled(void);

BOOL ui_get_curpg_scramble_flag(void);

void ui_set_playpg_scrambled(BOOL is_scramble);

void ui_book_play(u16 pgid);

void trans_lock_tp_info(dvbc_lock_info_t *p_lock_info, dvbs_tp_node_t *p_tp);

void trans_tp_info(tp_rcv_para_t *p_tp_rcv, dvbs_tp_node_t *p_tp);

void trans_pg_info(dvbs_program_t *p_pg_info, dvbs_prog_node_t *p_pg);

void load_play_paramter_by_pgid(play_param_t *p_info, u16 pg_id);

void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id);

void do_lock_pg(u16 pg_id);

void ui_reset_video_aspect_mode(u32 aspect);

void ui_reset_tvmode(u32 av_mode);

BOOL is_enable_vbi_on_setting(void);

void ui_age_limit_recheck(u32 para1, u32 para2);

BOOL ui_is_prog_encrypt(void);

void ui_play_set_cw(u8 *p_data, u8 key_length);

void ui_init_play_timer(u16 owner, u16 notify, u32 tmout);

void ui_deinit_play_timer(void);

void ui_play_timer_start(void);

void ui_play_timer_set_state(u8 state);

BOOL ui_is_prog_block(void);

BOOL ui_get_app_playback_status(void);
BOOL ui_india_change_group(dvbs_view_t view_type, u16 group, BOOL is_play);
#ifdef ENABLE_NETWORK
void ui_set_play_prg_type(play_prg_type_t prg_type);

play_prg_type_t ui_get_play_prg_type(void);
#endif

#endif

