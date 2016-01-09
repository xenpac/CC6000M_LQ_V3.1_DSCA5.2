/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_PVR_API_H_
#define _UI_PVR_API_H_

#include "pvr_api.h"
#include "vfs.h"

enum pvr_msg
{
  MSG_PLAY_END = MSG_EXTERN_BEGIN + 1200,
  MSG_CANNOT_PLAY,
  MSG_PLAYED,
  MSG_PLAY_MODE_CHANGED,
  MSG_PVR_REC_TIME_END,
  MSG_PVR_REC_MEM_NOT_ENOUGH,
  MSG_PVR_REC_NO_SIGNAL,
  MSG_PVR_REC_UPDATED,
  MSG_WRITE_ERROR,
  MSG_UNKNOW_ERROR,
  MSG_PVR_REC_STOPPED,
  MSG_PVR_FOCUS_CHANGED, //play bar return to record manager
  MSG_TS_PLAY_NORMAL
};

typedef struct
{
  u32  total;      //file list count
  u16  focus;      //file list focus
  u16  playing;    //the playing file
  u8   path_len;
  BOOL b_from_list;
}pvr_param_t;

#define PVR_DIR       "PVRS"
//#define REC_DIR       "record"

//#if ENABLE_MUSIC_PICTURE

typedef struct tag_pvr_rec_list
{
  struct tag_pvr_rec_list *p_next;
  media_file_t *p_file;
}pvr_rec_list_t;

u32 ui_pvr_get_play_id(void);
void ui_pvr_get_partition_info(vfs_dev_info_t *p_info);
u16 ui_pvr_get_capacity(void);
u32 ui_pvr_load_rec_info(void);
void ui_pvr_unload_rec_info(void);
void ui_pvr_del_one_rec_byname(u16 index);
rec_info_t *ui_pvr_get_rec_info(flist_t* p_rec_fav);
BOOL ui_pvr_is_scrambled(void);
BOOL ui_get_pvr_scrambled_flag(rec_info_t *p_rec_info);

u8 ui_pvr_get_lock_flag(u16 index);
void ui_pvr_change_lock_flag(u16 index);

flist_t* ui_pvr_get_rec_fav_by_index(u16 index);

void str_uni_printf(const u16 *p_src);

RET_CODE ui_recorder_init(void);
RET_CODE ui_recorder_release(void);
void ui_recorder_up_devinfo(void);
RET_CODE ui_recorder_start(mul_pvr_rec_attr_t *p_rec_data);
RET_CODE ui_recorder_stop(void);
RET_CODE ui_recorder_pause(void);
RET_CODE ui_recorder_resume(void);
BOOL ui_recorder_isrecording(void);
void ui_recorder_end(void);
void ui_recorder_change_pid(mul_rec_media_t *p_media);

RET_CODE ts_player_init(void);
RET_CODE ts_player_release(void);
RET_CODE ts_player_start(u16 *p_file_name, void *p_data);
RET_CODE ts_player_stop(void);
RET_CODE ts_player_pause(void);
RET_CODE ts_player_resume(void);
RET_CODE ts_player_play_mode(mul_pvr_play_speed_t speed);
RET_CODE ts_player_seek(ts_player_seek_param_t *p_seek);
BOOL ts_player_isplaying(void);
void ts_player_change_audio(u16 a_pid, u8 a_fmt);
u16 ts_player_get_cur_audio(void);
RET_CODE ts_player_start_preview(u16 *p_file_name, u16 v_pid, u8 video_fmt);
RET_CODE ts_player_stop_preview(void);
//#endif

#endif

//end of file

