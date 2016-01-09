/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_AD_API_H__
#define __UI_AD_API_H__

typedef RET_CODE (*adv_nofity) (ads_ad_type_t type, void *param);

void ui_adv_set_adm_id(u32 adm_id);

u32 ui_adv_get_adm_id(void);

void ui_adv_pic_init(pic_source_t src);

void ui_adv_pic_release(void);

void pic_adv_start(mul_pic_param_t *p_pic_param);

void pic_adv_stop(void);

BOOL ui_adv_pic_play(ads_ad_type_t type, u8 root_id);

void ui_adv_set_fullscr_rect(u16 x, u16 y, u16 w, u16 h);

RET_CODE ui_adv_set_pic(u32 size, u8 *pic_add);

RET_CODE ui_adv_add_mul_pic(u32 size, u8 *pic_add ,rect_t rect);

RET_CODE ui_adv_start_mul_pic(void);

RET_CODE ui_adv_stop_mul_pic(void);

void ui_adv_register_nofity(adv_nofity func);

RET_CODE ui_adv_notify(ads_ad_type_t type, void *param);

BOOL ui_adv_get_pic_rect(rect_t * rect_region);

#endif

