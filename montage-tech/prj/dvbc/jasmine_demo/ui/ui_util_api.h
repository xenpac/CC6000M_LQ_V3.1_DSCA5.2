/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_UTIL_API_H__
#define __UI_UTIL_API_H__

#include "lib_rect.h"

typedef enum
{
  PIC_TYPE_MAIN_MENU = 0,
  PIC_TYPE_SUB_MENU
} ads_type_t;

typedef enum
{
  START_LOGO_RES_ID = 0xc0,
  MAIN_MENU_RES_ID,
  SUB_MENU_RES_ID,
  CH_LIST_RES_ID,
  PROG_BAR_RES_ID,
  VOL_BAR_RES_ID,
  WATER_MARK_RES_ID,
} montds_res_id;

void ui_enable_uio(BOOL is_enable);

void ui_set_front_panel_by_str(const char * str);

void ui_set_front_panel_by_num(u16 num);

void ui_show_logo(u8 block_id);

void ui_show_logo_by_data(u8 *data, u32 size);

RET_CODE ui_show_logo_in_osd(u8 *data, u32 size, rect_t *rect, pdec_image_format_t format);

RET_CODE ui_hide_logo_in_osd(void);

void ui_enable_video_display(BOOL is_enable);

void ui_config_9pic_osd(void);

void ui_config_ttx_osd(void);

void ui_config_normal_osd(void);

void ui_conver_file_size_unit(u32 size, u8* p_str);

void ui_set_com_num(u8 num);

void ui_set_front_panel_by_str_with_upg(const char *str_flag, u8 process);

void ui_show_watermark(void);

void ui_watermark_release(void);

void ui_water_mark_refresh(void);

void ui_show_logo_bg(ads_type_t ads_type , rect_t *rect);

void ui_close_logo_bg(void);

void ui_show_ads_pic(ads_type_t ads_type , rect_t *region_rect);

void ui_close_ads_pic(void);

BOOL is_region_y_coordinate_covered(rect_t *region_rect1, rect_t *region_rect2);

BOOL ui_get_watermark_rect(rect_t * rect_region);

#ifdef ENABLE_NETWORK
void ui_set_net_svc_instance(void *p_instance);

void * ui_get_net_svc_instance(void);

enum
{
    APP_DRV_RW_FLASH = APP_PROJECT_DEFINE_START,
    APP_IPTV,
    APP_LIVETV,
    APP_NET_VOD,
    APP_NETMEDIA,
    APP_PROJECT_LAST = APP_LAST,
}prj_app_id_t;

#endif

#endif

