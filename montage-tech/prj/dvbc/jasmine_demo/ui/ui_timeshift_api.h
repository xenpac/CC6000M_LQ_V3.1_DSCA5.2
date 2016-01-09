/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _UI_TIMESHIFT_API_H_
#define _UI_TIMESHIFT_API_H_

enum timeshif_msg
{
  //MSG_TIMESHIFT_EVT_TIME_END = MSG_EXTERN_BEGIN + 1150,
 // MSG_TIMESHIFT_EVT_READY_TO_PLAY,
  //MSG_TIMESHIFT_EVT_STOPPED,
  //MSG_TIMESHIFT_EVT_PLAY_MODE_CHANGED,
  //MSG_TIMESHIFT_EVT_FILE_SIZE,
  //////
  MSG_TIMESHIFT_EVT_REC_UPDARED = MSG_EXTERN_BEGIN + 1150,
  MSG_TIMESHIFT_EVT_PLAY_UPDATED,
  MSG_TIMESHIFT_EVT_PLAY_SOF,
  MSG_TIMESHIFT_EVT_PLAY_REACH_REC
};
void ui_register_pvr_cb(u32 chanid, event_call_back callBack);

void ui_tshift_init(void);
void ui_tshift_keymap_init(void);
void ui_tshift_deinit(void);
void ui_tshift_callback(handle_t tshift_handle, mul_pvr_event_t content, u32 para1, u32 para2);
void ui_tshift_rec_cfg(mul_pvr_rec_attr_t *p_tshift_para, ui_pvr_private_info_t *ui_tshift_para);

void ui_tshift_play_cfg(mul_pvr_play_attr_t *p_tshift_para, ui_pvr_private_info_t *ui_tshift_para);

void ui_tshift_rec_create(u32 *p_chnid, mul_pvr_rec_attr_t *tshift_para);

void ui_tshift_rec_set(u32 h_chnid, mul_pvr_rec_attr_t *tshift_para);

void ui_tshift_play_start(u32 *p_play_id, u32 rec_id, mul_pvr_play_attr_t *p_play_attr);

void ui_tshift_play(u32 h_chnid);
///////////////////////////////////
void ui_tshift_stop(u32 h_chnid_play, u32 h_chnid_rec, mul_av_play_stop_opt_t * p_stop_para);

void ui_tshift_pause(u32 h_chnid);

void ui_tshift_resume(u32 h_chnid);

void ui_tshift_play_mode(u32 chanid, mul_pvr_play_mode_t *p_play_mode);

void ui_tshift_seek(u32 chanid, mul_pvr_play_position_t *p_position);
#endif

//end of file


