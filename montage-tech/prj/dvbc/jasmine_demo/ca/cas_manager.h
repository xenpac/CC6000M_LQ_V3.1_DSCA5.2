/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __CAS_MANAGER_H__
#define __CAS_MANAGER_H__

typedef enum 
{
   MAIN_MENU_EMAIL  = 0,
   MAIN_MENU_ANNOUNCE_INFO,
   SUB_MENU_CONDITIONAL,
   CA_ROLL_ON_DESKTOP,
   CA_OPEN_EMAIL,
   CA_CLOSE_EMAIL,
   CA_CLOSE_AUTO_FEED,
   CA_CLOSE_FINGER_PRINT,
   CA_CLOSE_IPP,
   CA_OPEN_CURTAIN,
   CA_OPEN_HIDE_CA_MENU,
   CA_CHECK_CA_NEW_EMAIL,
}ca_input_menu_type;

/*!
  cas manager policy struct
  */
/*!
  */
typedef struct 
{
    /*!
      ap user ca id,it use to send command
      */
    u8 ap_cas_id;
   /*!
     project user ca id,it use to fix diffent parameter
     */
    u8 cas_cfg_id;
   /*!
     ca area limit flag,FALSE: no limit function
     */
    BOOL cas_area_limit;
   /*!
     set ca menu attr
     */
    menu_attr_t *ca_menu_attr;
   /*!
     ca menu attr number
     */
    u32 attr_number;

   /*!
     set ca preview attr
     */
     preview_attr_t *ca_preview_attr;
   /*!
     ca preview_attr attr number
     */
    u32 preview_attr_number;

   /*!
     set ca fullscreen root
     */
     u8 *ca_fullscreen_root;
   /*!
     ca fullscreen root number
     */
    u32 ca_fullscreen_root_number;
   /*!
     register ca ap msghost
     */
    //u8 *ca_msghost;
   /*!
     register ca ap msghost number
     */
    //u32 msghost_number;
   
   /*!
     ca on config init
     */
    void (*cas_config_init)(void);
   /*!
     ca area limit check start
     */
    void (*cas_area_limit_check_start)(void); 
      /*!
     ca area limit check stop
     */
    void (*cas_area_limit_check_stop)(void); 
   /*!
     ca auto receive email start
     */
    void (*cas_auto_receive_email_start)(void); 
   
   /*!
     ca set sid to cas app when play start
     */
    void (*cas_set_sid)(u16 pg_id);
   /*!
     ca sart dvb moniter when play start
     */
    void (*cas_dvb_monitor_start)(u16 pg_id);
    /*!
     ca stop when play stop
     */
    void (*cas_play_stop)(void);
   /*!
     tell ca when syster is ready,it can work,some ca meed to;
     */
    void (*cas_sys_ready)(void);
  /*!
     reset data when syster factiory reset,it can work,some ca meed to;
     */
    void (*cas_reset_data)(void);
   /*!
     request card info on some public menu,some ca meed to;
     */
    void (*cas_request_card_info)(void);
   /*!
    process deskto message in the some place;
     */
    RET_CODE (*cas_ui_desktop_process)(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);
   /*!
     process ca menu on public menu;
     */
    RET_CODE (*cas_manage_process_menu)(ca_input_menu_type type,control_t *p_ctrl,u32 para1, u32 para2);
   /*!
     process ca messige on ca menu;
     */
    RET_CODE (*cas_manage_process_msg)(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
    u16 (*cas_manage_process_keymap)(u16 key);
   /*!
     paint menu when finger is exist
     */
    void (*cas_manage_finger_repaint)(void);
	    void (*cas_manage_reroll_redraw_finger)(u8 from_id, u8 to_id);

    void (*cas_manager_start_pvr_play)(u16 index);
    RET_CODE (*cas_manager_get_current_program_ecm_emm_info)(u16 *ts_emm_id, u16 *audio_ecm_pid, u16 *video_ecm_pid);
    RET_CODE (*cas_manager_get_current_program_ecm_emm)(u16 *p_ecm_pid, u16 *p_emm_id);
   /*!
     paint menu when super is exist
     */
    void (*cas_manage_super_osd_repaint)(void);
    BOOL (*cas_manage_is_pvr_forbid)(void);
    void (*cas_manage_get_discramble_key)(u8 *p_key);
    void (*cas_manage_set_pvr_forbid)(BOOL pvr_flag);
       /*
    for reroll when close all menu
    */
    void (*cas_manage_reroll_after_close_all_menu)(void);
    BOOL (*cas_manage_ecm_update)(u8 *p_pmt_data, cas_desc_t *p_ecm_info, u16 *p_max);
}cas_manager_policy_t;

cas_manager_policy_t *construct_cas_manager_policy(void);
void cas_manager_policy_init(void);
cas_manager_policy_t * get_cas_manager_policy_handle(void);
void cas_manager_config_init(void);
void cas_manager_set_sid(u16 pg_id);
void cas_manager_dvb_monitor_start(u16 pg_id);
void cas_manager_stop_ca_play(void);
void cas_manager_get_ca_card_info(void);
RET_CODE cas_manager_ca_area_limit_check_start(void);
void cas_manager_ca_auto_receve_email_start(void);
RET_CODE cas_manage_process_menu(ca_input_menu_type type,control_t *p_ctrl,u32 para1, u32 para2);
RET_CODE cas_manage_proc_on_normal_cas(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
u16 cas_manage_keymap_on_normal_cas(u16 key);
void cas_manage_finger_repaint(void);
void cas_manage_reroll_redraw_finger(u8 from_id, u8 to_id);
void cas_manage_ota_update_ver_dm_piece_register(void);
void cas_manager_start_pvr_play(u16 index);
RET_CODE cas_manager_get_current_program_ecm_emm_info(u16 *ts_emm_id, u16 *audio_ecm_pid, u16 *video_ecm_pid);
RET_CODE cas_manager_get_current_program_ecm_emm(u16 *p_ecm_pid, u16 *p_emm_id);
void cas_manager_set_timeshift_play(BOOL is_timeshift);

BOOL cas_manager_is_pvr_play(void);
BOOL cas_manager_is_pvr_forbid();
void cas_manage_get_discramble_key(u8 *p_key);
void cas_manager_set_pvr_forbid(BOOL pvr_flag);
void cas_manager_reroll_after_close_all_menu(void);
void cas_manage_super_osd_repaint(void);
BOOL cas_manage_ecm_update(u8 *p_pmt_data, cas_desc_t *p_ecm_info, u16 *p_max);
#endif

