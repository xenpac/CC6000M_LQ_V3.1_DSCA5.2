/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_SYSTATUS_API_H__
#define __UI_SYSTATUS_API_H__

#define RF_NTSC_MIN    14
#define RF_NTSC_MAX    83
#define RF_NTSC_CNT    (RF_NTSC_MAX - RF_NTSC_MIN + 1)

#define RF_PAL_MIN     21
#define RF_PAL_MAX     69
#define RF_PAL_CNT     (RF_PAL_MAX - RF_PAL_MIN + 1)
#if  (defined CUSTOMER_YINHE_TR ||defined CUSTOMER_YINHE_LINGGANG)
#define FAV_LIST_COUNT (1)
#else
#define FAV_LIST_COUNT (5)
#endif
BOOL sys_status_check_group(void);

u16 sys_status_get_sat_group_num(void);
u16 sys_status_get_sat_group_pos(u16 group);

u16 sys_status_get_fav_group_num(void);
u16 sys_status_get_fav_group_pos(u16 group);

u16 sys_status_get_all_group_num(void);

BOOL sys_status_get_group_all_info(u8 mode, u16 *p_curn_rid);

BOOL sys_status_get_sat_group_info(u16 sat_pos, u8 mode, u16 *p_curn_rid);

BOOL sys_status_get_fav_group_info(u16 fav_pos, u8 mode, u16 *p_curn_rid);

BOOL sys_status_get_group_info(u16 group, u8 *p_group_type, u16 *p_pos_in_set, u32 *p_context);

u16 sys_status_get_curn_group(void);

BOOL sys_status_set_curn_group(u16 group);

BOOL sys_status_set_curn_group_not_write_flash(u16 group);

u16 sys_status_shift_curn_group(s16 offset);

BOOL sys_status_get_curn_prog_in_group(u16 group, u8 mode, u16 *p_rid, u32 *p_context);

u8 sys_status_get_curn_prog_mode(void);

BOOL sys_status_set_curn_prog_mode(u8 mode);

u8 sys_status_get_group_curn_type(void);

BOOL sys_status_set_group_curn_type(u8 type);

u16 sys_status_get_curn_group_curn_prog_id(void);

BOOL sys_status_set_curn_group_info(u16 rid, u16 pos);

void sys_status_get_view_info(u16 group, u8 mode,
  u16 *p_view_type, u32 *p_context);

void sys_status_get_curn_view_info(u16 *p_view_type, u32 *p_context);

u16 sys_status_get_pos_by_group(u16 group);

u16 sys_status_get_group_by_pos(u16 pos);

void sys_status_get_group_name(u16 group, u16 *p_name, u32 len);

avc_video_mode_1_t sys_status_get_sd_mode(u8 focus);

avc_video_mode_1_t sys_status_get_hd_mode(u8 focus);
avc_video_mode_1_t sys_status_get_hd_mode_1(u8 focus,void *p_disp);
avc_video_aspect_1_t sys_status_get_video_aspect(u8 focus);

scart_v_format_t sys_status_get_scart_out_cfg(u8 focus);

scart_v_aspect_t sys_status_get_scart_aspect(u8 focus);

rf_sys_t sys_status_get_rf_system(u8 focus);

u16 sys_status_get_rf_channel(BOOL is_pal, u8 focus);

void sys_status_reload_environment_setting(BOOL restory);

u8 sys_status_get_global_media_volume(void);

void sys_status_set_global_media_volume(u8 volume);

u8 sys_status_get_brightness(void);

void sys_status_set_brightness(u8 brightness);

u8 sys_status_get_contrast(void);

void sys_status_set_contrast(u8 contrast);

u8 sys_status_get_saturation(void);

void sys_status_set_saturation(u8 saturation);

BOOL sys_status_set_group_all_info(u8 mode, u16 curn_rid, u16 pos);

u16 sys_status_get_fav1_index(void);

BOOL get_dm_block_real_file_info(u8 block_id,dmh_block_info_t *block_info);
u32 get_dm_block_real_file_size(u8 block_id);
u16 get_dm_block_real_file_ota_version(u8 block_id);
u32 get_dm_block_real_file_crc(u8 block_id);
u32 get_dm_block_real_file_addr(u8 block_id);
BOOL set_dm_block_real_file_ota_version(u8 block_id,u16 version) ;
#endif


