/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _UI_NVOD_API_H
#define _UI_NVOD_API_H

enum nvod_msg
{
  MSG_NVOD_REF_SVC_FOUND = MSG_EXTERN_BEGIN + 100,
  MSG_NVOD_REF_EVT_FOUND,
  MSG_NVOD_SHIFT_EVT_FOUND,
  MSG_NVOD_SVC_SCAN_END,
  MSG_NVOD_FULL_PLAY_EXIT,
};

void ui_nvod_init(void);

void ui_nvod_release(void);

u16 ui_nvod_get_shift_event_index(void);

void ui_nvod_set_shift_event_index(u16 idx);
  
void ui_nvod_start(void);

BOOL ui_nvod_start_ref_svc(dvbc_tp_info_t *p_tp);

void ui_nvod_stop_ref_svc(void);

void ui_nvod_stop(void);

void ui_set_playing_nvod_reference_svc_evt(nvod_reference_svc_evt_t *p_ref_svc_event);

nvod_reference_svc_evt_t * ui_get_playing_nvod_reference_svc_evt(void);

void ui_set_playing_nvod_time_shifted_svc_evt(time_shifted_svc_evt_t *p_shift_svc_event);

time_shifted_svc_evt_t * ui_get_playing_nvod_time_shifted_svc_evt(void);

BOOL nvod_play_offset(s8 offset);

u16 ui_get_timeshift_svc_pgid(time_shifted_svc_t *p_ts_svc);

void ui_play_nvod(time_shifted_svc_t *p_ts_svc, dvbc_tp_info_t *p_tp);
BOOL ui_play_nvod_book(u16 pg_id);

void ui_stop_play_nvod(stop_mode_t type, BOOL is_sync);
BOOL ui_nvod_enter(void);
BOOL ui_nvod_exit(void);

#endif

