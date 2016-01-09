/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#ifndef _UI_ADS_DISPLAY_H
#define _UI_ADS_DISPLAY_H


#define MAX_ROLL_OSD_LENGTH 256
#define MAX_ROLL_OSD_NUM 25


typedef struct 
{
  u8 ucLogType;
  u32 uiLogoSize;
}ST_AD_DS_LOG_SAVE_INFO;


typedef struct 
{
  BOOL b_SaveFlag;
  u8 osd_display_buffer[MAX_ROLL_OSD_LENGTH + 1];	
}ST_ADS_OSD_INFO;

BOOL get_ads_logo_status(void);
BOOL get_full_scr_ad_status(void);
BOOL ui_get_ads_osd_status(void);
BOOL ui_get_full_scr_ad_status(void);

void set_full_scr_ad_status(BOOL is_show);
void set_unauth_ad_status(BOOL status);
void ui_set_full_scr_ad_status(BOOL status);

void ui_ads_osd_roll_stop(void);
void ui_ads_osd_roll(void);
BOOL ui_is_adv_osd_ready(void);
void ui_ads_osd_roll_next(void);
  
RET_CODE ui_adv_desai_display(ads_info_t *p_ads_info);
RET_CODE ui_adv_desai_hide(ads_ad_type_t ad_type, u8 *p_data);
RET_CODE ui_check_fullscr_ad_play(void);
  
RET_CODE ui_adv_display_osd_msg(u16 pg_id);
void ads_get_type(ads_ad_type_t type, u32 * cmd, ads_info_t * ads_info);
void ui_get_unauth_program_ad(void);
RET_CODE on_ads_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#endif
