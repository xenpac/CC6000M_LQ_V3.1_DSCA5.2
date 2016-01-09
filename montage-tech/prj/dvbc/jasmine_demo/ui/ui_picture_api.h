/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_PICTURE_API_H__
#define __UI_PICTURE_API_H__

#define OSD0_WIDTH  1280
#define OSD0_HEIGHT 720

typedef enum
{
  PIC_STAT_PLAY,
  PIC_STAT_STOP,
  PIC_STAT_PAUSE,
}pic_play_state_t;

typedef enum 
{
  MSG_PIC_EVT_DRAW_START = MSG_EXTERN_BEGIN + 1110,
  MSG_PIC_EVT_DRAW_END,
  MSG_PLAYER_EVT_GET_PLAY_TIME,
  MSG_PIC_EVT_TOO_LARGE,
  MSG_PIC_EVT_UNSUPPORT,
  MSG_PIC_EVT_UPDATE_SIZE,
  MSG_PIC_EVT_DATA_ERROR,
  MSG_PIC_BACK_TO_NETWORK_PLACE,
  MSG_PIC_EVT_DATA_ERR,
}pic_msg_t;

typedef enum
{
  PIC_MODE_NORMAL = 0,
  PIC_MODE_FAV,
}pic_play_mode_t;

typedef enum
{
  PIC_SOURCE_FILE,
  PIC_SOURCE_NET,
  PIC_SOURCE_BUF,
} pic_source_t;
#define MAX_RETRY_CNT (9)

void ui_pic_init(pic_source_t src);

void ui_pic_release(void);

void pic_start(mul_pic_param_t *p_pic_param);

void pic_stop(void);

void pic_pause(void);

void pic_resume(media_fav_t **pp_media, rect_t *p_rect);

void ui_pic_set_play_mode(pic_play_mode_t mode);

pic_play_mode_t ui_pic_get_play_mode(void);

void ui_pic_unload_fav_list(void);

void ui_pic_get_fav_list(media_fav_t **pp_media);

BOOL ui_pic_set_play_index_dir_by_name(u16 *p_name);

BOOL ui_pic_build_play_list_indir(file_list_t *p_filelist);

u8 ui_pic_get_play_count_of_fav();

u8 ui_pic_get_play_count_of_dir();

void ui_pic_set_play_index_dir(u16 index);

void ui_pic_set_play_index_fav(u16 index);

BOOL ui_pic_set_play_index_fav_by_name(u16 *p_name);

BOOL ui_pic_play_curn(media_fav_t **pp_media, rect_t *rect);

BOOL ui_pic_play_next(media_fav_t **pp_media, rect_t *rect);

BOOL ui_pic_play_prev(media_fav_t **pp_media, rect_t *rect);

BOOL ui_pic_get_cur(media_fav_t **pp_media);

BOOL ui_pic_is_rotate(void);

BOOL ui_pic_play_clock(media_fav_t **pp_media, rect_t *rect, BOOL is_clock);

void ui_pic_load_fav_list(u16 letter);
  
BOOL ui_pic_add_one_fav(u16 *p_name);

void ui_pic_del_one_fav(u16 index);

void ui_pic_save_fav_list(u16 letter);
  
u32 ui_pic_get_fav_count(void);

void ui_pic_undo_save_del(void);

void ui_pic_set_one_del(u16 index);

BOOL ui_pic_is_one_fav_del(u16 index);

void ui_pic_save_del(u8 letter);

BOOL ui_pic_is_del_modified(void);

void ui_pic_set_del_modified(BOOL  boPicModified);

void pic_reset_flip(void);

void ui_pic_clear(u32 color);

BOOL check_picture_format_is_gif(u16 *filename);

void ui_pic_get_current_file_name(u16 **p_current_file_name);

#ifdef ENABLE_NETWORK
BOOL ui_pic_play_by_name(u16 *file_name, rect_t *p_rect);

BOOL ui_pic_play_by_url(u8 *p_url, rect_t *p_rect, u32 identify);

void ui_pic_clear_rect(rect_t *p_rect, u32 color);

handle_t ui_get_pic_region(void);

void ui_pic_clear_all(u32 color);

void ui_pic_set_url(u8 *p_url);
#endif

#endif
