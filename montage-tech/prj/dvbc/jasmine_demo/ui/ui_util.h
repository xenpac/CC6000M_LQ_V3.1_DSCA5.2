/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_UTIL_H__
#define __UI_UTIL_H__

#define LANG_CODE 14

RET_CODE ui_start_timer(u8 root_id, u16 msg, u32 time_out);
void convert_gb2312_chinese_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len);
void convert_utf8_chinese_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len);


#ifndef WIN32
inline RET_CODE ui_stop_timer(u8 root_id, u16 msg);
#else
RET_CODE ui_stop_timer(u8 root_id, u16 msg);
#endif

void ctrl_change_focus(control_t *p_lfCtrl, control_t *p_gfCtrl);

BOOL iso_639_get_lang_code(u8 *lang_code, u8 index);

u8 iso_639_get_lang_code_num();

BOOL iso_639_get_lang_code_name(u16 *uni_lang_code, u16 *lang_name_id, u8 index);

#endif

