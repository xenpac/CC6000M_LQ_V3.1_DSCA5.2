/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PUBLIC_H__
#define __UI_CA_PUBLIC_H__

#define GET_ENTITLE_COUNT  (10)

typedef enum
{
  ROOT_ID_CONDITIONAL_ACCEPT = ROOT_ID_CA_START,
  ROOT_ID_EMAIL_MESS,
  ROOT_ID_EMAIL_MESS_CONTENT,
  ROOT_ID_NEW_MAIL,
  ROOT_ID_CA_PROMPT,
  ROOT_ID_CA_CARD_INFO,
  ROOT_ID_CONDITIONAL_ACCEPT_LEVEL,
  ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME,
  ROOT_ID_CA_ENTITLE_INFO,
  ROOT_ID_CONDITIONAL_ACCEPT_PIN,
  ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH,
  ROOT_ID_CONDITIONAL_ACCEPT_INFO,
  ROOT_ID_CA_IPPV_PPT_DLG,
  ROOT_ID_CA_CARD_UPDATE,
  ROOT_ID_FINGER_PRINT,
  ROOT_ID_CA_SUPER_OSD,
}ca_ui_root_id;

typedef enum OSD_SHOW_POS
{
	OSD_SHOW_TOP = 0,					//在顶部显示
	OSD_SHOW_BOTTOM,					//在底部显示
	OSD_SHOW_TOP_BOTTOM,				//在顶部和底部同时显示
}OSD_SHOW_POS;

typedef struct
{
u32 index;  //index 
u32 message_type;  // 0:mail  1:announce 2:alert mail 3:alert announce
}prompt_type_t;

RET_CODE open_ca_rolling_menu(u32 para1, u32 para2);


u32 ui_get_ca_last_msg(void);
BOOL ui_set_smart_card_insert(BOOL status);
BOOL ui_is_smart_card_insert(void);
u32 ui_get_smart_card_rate(void);
void ui_set_smart_card_rate(u32 level);
RET_CODE on_conditional_accept_level_update_d(control_t *p_cont, u16 msg, u32 para1, u32 para2);

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);

//void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id);

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2);
void on_ca_zone_check_stop(void);
void on_ca_zone_check_end(void);
void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2);
RET_CODE ui_desktop_proc_cas(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
u16 ui_desktop_keymap_cas(u16 key);
RET_CODE open_conditional_accept_feed(u32 para1, u32 para2);
//RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2);
//RET_CODE open_conditional_accept_pin(u32 para1, u32 para2);
RET_CODE open_conditional_accept_pair(u32 para1, u32 para2);
//RET_CODE open_conditional_accept_level(u32 para1, u32 para2);
//RET_CODE open_conditional_accept_info(u32 para1, u32 para2);
RET_CODE open_ca_rolling_menue(u32 para1, u32 para2);

BOOL ui_is_finger_show(void);
void redraw_finger(void);
void on_desktop_start_roll_position(u16 *uni_str, u8 *display_back_color, u8 *display_color, u8 *display_front_size, u8 *display_pos);
BOOL osd_roll_stop_msg_unsend(void);
BOOL get_force_channel_status(void);

#endif

