/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_MUSIC_API_H__
#define __UI_MUSIC_API_H__

typedef enum
{
/*!
  music play cur dir
  */
  MUSIC_PLAY_CUR_DIR,
/*!
  music play fav list
  */ 
  MUSIC_PLAY_FAV_LIST, 
  
}music_playlist_type_t;

typedef enum
{
  MUSIC_STAT_FB,
  MUSIC_STAT_FF,
  MUSIC_STAT_PLAY,
  MUSIC_STAT_STOP,
  MUSIC_STAT_PAUSE,
  MUSIC_STAT_INVALID,
}music_play_state_t;

typedef enum
{
  PLAY_MODE_CYCLE_CUR_DIR_ORDER,
  PLAY_MODE_CYCLE_CUR_DIR_RANDOM,
  PLAY_MODE_CYCLE_CUR_DIR_SELF,
  PLAY_MODE_CYCLE_CUR_DIR_NONE,

  PLAY_MODE_CYCLE_FAV_LIST_ORDER,
  PLAY_MODE_CYCLE_FAV_LIST_RANDOM,
  PLAY_MODE_CYCLE_FAV_LIST_SELF,
  PLAY_MODE_CYCLE_FAV_LIST_NONE,

  PLAY_MODE_MUSIC_MAX,
}mlist_play_mode_t;

typedef enum 
{
  MSG_MUSIC_EVT_PLAY_END = MSG_EXTERN_BEGIN + 1100,
  MSG_MUSIC_EVT_CANNOT_PLAY,
  MSG_MUSIC_EVT_STOPPED,
}music_msg_t;

u32 ui_music_get_handle();

void ui_music_player_init(void);

void ui_music_player_release(void);

void music_player_start(u16 *p_file_name);

void music_player_next(u16 *p_file_name);

void music_player_stop(void);

void music_player_pause(void);

void music_player_resume(void);

void ui_music_state_set(void);

void ui_music_state_clear(void);

BOOL ui_music_state_get(void);

void ui_music_set_play_status(music_play_state_t state);

music_play_state_t ui_music_get_play_status(void);

void ui_music_set_play_mode(mlist_play_mode_t mode);

mlist_play_mode_t ui_music_get_play_mode(void);

void ui_music_unload_fav_list(void);

void ui_music_get_fav_list(media_fav_t **pp_media);

void ui_music_set_play_index_dir_by_name(u16 *p_name);

BOOL ui_music_build_play_list_indir(file_list_t *p_filelist);

BOOL ui_music_build_play_list_infav(void);

u8 ui_music_get_play_count_of_fav(void);

u8 ui_music_get_play_count_of_dir(void);

void ui_music_set_playlist_type(music_playlist_type_t music_plist_type);

music_playlist_type_t ui_music_get_playlist_type(void);

void ui_music_set_play_index_dir(u16 index);

u16  ui_music_get_play_index_dir(void);

void ui_music_set_play_index_fav(u16 index);

u16 ui_music_get_play_index_fav(void);

BOOL ui_music_set_play_index_fav_by_name(u16 *p_name);

BOOL ui_music_pre_start_play(media_fav_t **pp_media);

BOOL ui_music_pre_play_next(media_fav_t **pp_media);

BOOL ui_music_pre_play_pre(media_fav_t **pp_media);

BOOL ui_music_get_cur(media_fav_t **pp_media);

media_fav_t *ui_music_get_fav_media_by_index(u16 index);

void ui_music_load_fav_list(u16 letter);
  
void ui_music_add_one_fav(u16 *p_name);

void ui_music_del_one_fav(u16 index);

void ui_music_save_fav_list(u16 letter);
  
u32 ui_music_get_fav_count(void);

void ui_music_undo_save_del(void);

void ui_music_set_one_del(u16 index);

BOOL ui_music_is_one_fav_del(u16 index);

void ui_music_save_del(u8 letter);

BOOL ui_music_is_del_modified(void);

void ui_music_set_del_modified(BOOL  boMusicModified);

void ui_music_fullscreen_player(u16 *p_file_name);

void ui_music_lrc_show(BOOL bshow);

void ui_music_logo_show(BOOL bshow, rect_t logo_rect);

void ui_music_logo_clear(u32 color);

void ui_music_set_charset(str_fmt_t charset);

#ifndef WIN32
void ui_music_create_region(void);
void ui_music_release_region(void);
#endif

#endif