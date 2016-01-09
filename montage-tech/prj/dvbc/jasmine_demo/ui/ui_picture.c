/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_picture.h"
#include "file_list.h"
#include "ui_pic_play_mode_set.h"
#include "ui_usb_picture.h"
#include "ui_usb_music.h"
#include "ui_volume_usb.h"
#include "ui_mute.h"
#include "ui_music_player_service.h"


#define BACKGROUND_MUSIC_SEARCH_FOLDER "music"

enum picture_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 1050,
  MSG_GREEN,
  MSG_BLUE,
  MSG_YELLOW,
  MSG_PIC_PLAY,
  MSG_PIC_PAUSE,
  MSG_PREV_PIC,
  MSG_NEXT_PIC,
  MSG_CLOCK_WISE,
  MSG_ANTI_CLOCK,
  MSG_PIC_TIMER,
  MSG_MUSIC_NEXT,
  MSG_MUSIC_PRE,
  MSG_MUSIC_PLAY_END,
  MSG_VOLUME_DOWN,
  MSG_VOLUME_UP,
};

enum picture_menu_ctrl_id
{
  IDC_PAUSE_CONT = 1,
};

enum picture_pause_cont_ctrl_id
{
  IDC_PAUSE = 1,
};

static rect_t g_rect = {0, 0, SCREEN_FULL_PIC_WIDTH, SCREEN_FULL_PIC_HEIGHT};  //bug fixed move pic down
static BOOL g_slide_pause = FALSE;
#ifdef ENABLE_NETWORK
extern s32 get_picture_is_usb();
#endif
static MUSIC_PLAYER_SERVCIE *music_player_service;
static BOOL mute_state_backup;

u16 picture_cont_keymap(u16 key);

RET_CODE picture_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

static void picture_tmr_create(void)
{
  BOOL is_slide_show = pic_play_get_slide_show_sts();
  
  u8 interval = pic_play_get_slide_interval();
  
#ifndef ENABLE_NETWORK
  is_slide_show = TRUE;
#endif

  if(is_slide_show && interval)
  {
    fw_tmr_create(ROOT_ID_PICTURE, MSG_PIC_TIMER, interval * 1000, FALSE);
  }
}

static void picture_tmr_destory(void)
{
  fw_tmr_destroy(ROOT_ID_PICTURE, MSG_PIC_TIMER);
}

static void picture_rotate(control_t *p_cont, BOOL is_clock)
{
  control_t *p_pause = NULL, *p_pause_cont = NULL;

  p_pause_cont = ctrl_get_child_by_id(p_cont, IDC_PAUSE_CONT);
  p_pause = ctrl_get_child_by_id(p_pause_cont, IDC_PAUSE);

  if(ui_pic_is_rotate())
  {
    bmap_set_content_by_id(p_pause, is_clock ? IM_SWORD_R : IM_SWORD_L);
  }
  else
  {
    bmap_set_content_by_id(p_pause, IM_PAUSE);
  }
  g_slide_pause = TRUE;
  ctrl_set_attr(p_pause, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_pause, OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_pause_cont, TRUE);
}

static void picture_pause(control_t *p_cont, BOOL is_pause)
{
  control_t *p_pause = NULL, *p_pause_cont = NULL;

  p_pause_cont = ctrl_get_child_by_id(p_cont, IDC_PAUSE_CONT);
  p_pause = ctrl_get_child_by_id(p_pause_cont, IDC_PAUSE);

  bmap_set_content_by_id(p_pause, IM_PAUSE);

  if(is_pause)
  {
    g_slide_pause = TRUE;
    ctrl_set_attr(p_pause, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_pause, OBJ_STS_SHOW);
    ctrl_paint_ctrl(p_pause_cont, TRUE);
  }
  else
  {
    g_slide_pause = FALSE;
    ctrl_set_sts(p_pause, OBJ_STS_HIDE);
    ctrl_paint_ctrl(p_pause_cont, TRUE);
  }
}

static void _music_play_end(void)
{
  fw_notify_root(fw_find_root_by_id(ROOT_ID_PICTURE), NOTIFY_T_MSG, FALSE, MSG_MUSIC_PLAY_END, 0, 0);
}

static void _music_play_error(void)
{
  OS_PRINTF("[%s][%d] \n",__FUNCTION__,__LINE__);
  // Should we skip the error one, play next directly?
}


RET_CODE open_picture(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_pause_cont = NULL, *p_pause = NULL;
  media_fav_t *p_media = NULL;
  MUSIC_PLAY_LIST *search_list = NULL;
  partition_t *p_partition = NULL;
  u32 partition_count = 0;
  u32 i;
  u8 path[MAX_FILE_PATH];
  u8 partition_leter[10];
  u8 tmp[MAX_FILE_PATH] = {0};

  ui_time_enable_heart_beat(FALSE);
  //pic_stop();
  ui_pic_clear(0);

  g_slide_pause = FALSE;

  /*Create Menu*/
  p_cont = fw_create_mainwin(ROOT_ID_PICTURE,
           PICTURE_CONT_X, PICTURE_CONT_Y,
           PICTURE_CONT_W, PICTURE_CONT_H,
           ROOT_ID_USB_PICTURE, 0,
           OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, picture_cont_keymap);
  ctrl_set_proc(p_cont, picture_cont_proc);

  p_pause_cont = ctrl_create_ctrl(CTRL_BMAP, IDC_PAUSE_CONT,
    PICTURE_PAUSE_X, PICTURE_PAUSE_Y, PICTURE_PAUSE_W, PICTURE_PAUSE_H, p_cont, 0);
  ctrl_set_rstyle(p_pause_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  p_pause = ctrl_create_ctrl(CTRL_BMAP, IDC_PAUSE,
    0, 0, PICTURE_PAUSE_W, PICTURE_PAUSE_H, p_pause_cont, 0);
  //bmap_set_content_by_id(p_pause, IM_PAUSE);
  ctrl_set_sts(p_pause, OBJ_STS_HIDE);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_pic_play_curn(&p_media, &g_rect);

  mute_state_backup = ui_is_mute();
  if(mute_state_backup) //set mute
  {
    open_mute(0, 0);
  }

  music_player_service = (MUSIC_PLAYER_SERVCIE *)get_music_player_service_instance();
  MT_ASSERT(music_player_service != NULL);
  music_player_service->play_end_cb = _music_play_end;
  music_player_service->play_error_cb = _music_play_error;
  music_player_service->init();
  music_player_service->reset();
  music_player_service->set_play_mode(PLAY_MODE_LOOP_RANDOM);

  partition_count = file_list_get_partition(&p_partition);
  if (partition_count == 0 || p_partition == NULL)
  {
    return SUCCESS;
  }
  for (i = 0; i < partition_count; i++)
  {
    memset(partition_leter, 0, sizeof(partition_leter));
    str_uni2asc(partition_leter,p_partition[i].letter);
    memset(path, 0, sizeof(path));
    strcat((char *)path, (char *)partition_leter);
    strcat((char *)path, (char *)"\\");
    strcat((char *)path, (char *)BACKGROUND_MUSIC_SEARCH_FOLDER);

    OS_PRINTF("Start search %s \n", path);
    search_list = music_player_service->search_music(path, MUSIC_SEARCH_OPT_DEFAULT);
    while (search_list != NULL)
    {
      memset(tmp, 0, sizeof(tmp));
      unicode_to_gb2312(search_list->uri.str, (s32)uni_strlen((u16 *)search_list->uri.str),
                      tmp, sizeof(tmp));
      OS_PRINTF("Found %s\n", tmp);
      music_player_service->add_play_list(search_list->uri);
      search_list = search_list->next;
    }
  }

  ui_set_mute(FALSE);
  music_player_service->play();
  
  return SUCCESS;
}

static RET_CODE on_picture_pause(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;

  if(g_slide_pause)
  {
    picture_tmr_destory();
    picture_pause(p_cont, FALSE);
    pic_resume(&p_media, &g_rect);
  }
  else
  {
    picture_tmr_destory();
    picture_pause(p_cont, TRUE);
    pic_pause();
  }

  return SUCCESS;
}

static RET_CODE on_picture_resume(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  picture_pause(p_cont, FALSE);
  pic_resume(&p_media, &g_rect);

  return SUCCESS;
}

static RET_CODE on_picture_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  pic_stop();

  return ERR_NOFEATURE;
}

static RET_CODE on_picture_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_root = NULL;

  pic_reset_flip();

  p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
  ui_pic_clear(0);
  if(p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }

  ui_set_mute(mute_state_backup);
  if(ui_is_mute()) //set mute
  {
    open_mute(0, 0);
  }

  music_player_service = (MUSIC_PLAYER_SERVCIE *)get_music_player_service_instance();
  MT_ASSERT(music_player_service != NULL);
  music_player_service->stop();
  music_player_service->deinit();

  return ERR_NOFEATURE;
}

static RET_CODE on_picture_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
#ifdef ENABLE_NETWORK
  u32 usb_dev_type = para2;
  
  if(usb_dev_type == HP_WIFI || !get_picture_is_usb())
  {
    OS_PRINTF("####wifi device  plug out or samba plug out usb#####\n");
    return ERR_FAILURE;
  }
#endif

  ui_pic_clear(0);
 
  manage_close_menu(ROOT_ID_PICTURE, 0, 0);

#ifdef MIN_AV_64M
  //restart epg
  ui_epg_init();
  
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#else
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
  return SUCCESS;
}

static RET_CODE on_picture_prev_pic(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  pic_stop();
  ui_pic_play_prev(&p_media, &g_rect);

  return ret;
}

static RET_CODE on_picture_next_pic(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  pic_stop();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_clock_wise(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  //picture_pause(p_cont, TRUE);
  pic_pause();
  ui_pic_play_clock(&p_media, &g_rect, TRUE);
  picture_rotate(p_cont, TRUE);

  return ret;
}

static RET_CODE on_picture_anti_clock(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  //picture_pause(p_cont, TRUE);
  pic_pause();
  ui_pic_play_clock(&p_media, &g_rect, FALSE);
  picture_rotate(p_cont, FALSE);

  return ret;
}


static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  //media_fav_t *p_media = NULL;
  u8 interval = pic_play_get_slide_interval();

  if(g_slide_pause)
  {
    return SUCCESS;
  }

  if(interval != 0)
  {
    picture_tmr_create();
  }
  else
  {
    //if(!ui_pic_play_next(&p_media, &g_rect))
    //{
    //  manage_close_menu(ROOT_ID_PICTURE, 0, 0);
    //}
  }
  return ret;
}

static RET_CODE on_picture_timer(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  //stop prev pic
  pic_stop();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_too_large(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  pic_stop();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_unsupport(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  pic_stop();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_data_error(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  pic_stop();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_fsceen_data_error(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  pic_stop();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    ui_pic_clear(0);
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_music_play_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  music_player_service = (MUSIC_PLAYER_SERVCIE *)get_music_player_service_instance();
  MT_ASSERT(music_player_service != NULL);

  music_player_service->stop();
  music_player_service->play_next();
  return SUCCESS;
}

#if 0
static RET_CODE on_picture_next_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;

  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_next(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_picture_prev_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;

  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_pre(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_volume(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
    open_volume_usb(ROOT_ID_PICTURE, para1); 
    return SUCCESS;
}
#endif

BEGIN_KEYMAP(picture_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_PREV_PIC)
  ON_EVENT(V_KEY_DOWN, MSG_NEXT_PIC)
  ON_EVENT(V_KEY_LEFT, MSG_ANTI_CLOCK)
  ON_EVENT(V_KEY_RIGHT, MSG_CLOCK_WISE)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_PLAY, MSG_PIC_PLAY)
  ON_EVENT(V_KEY_PAUSE, MSG_PIC_PAUSE)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_NEXT, MSG_MUSIC_PLAY_END)
  #if 0
  ON_EVENT(V_KEY_NEXT, MSG_MUSIC_NEXT)
  ON_EVENT(V_KEY_PREV, MSG_MUSIC_PRE)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  #endif
END_KEYMAP(picture_cont_keymap, NULL)

BEGIN_MSGPROC(picture_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_picture_exit)
  ON_COMMAND(MSG_DESTROY, on_picture_destory)
  ON_COMMAND(MSG_CLOCK_WISE, on_picture_clock_wise)
  ON_COMMAND(MSG_ANTI_CLOCK, on_picture_anti_clock)
  ON_COMMAND(MSG_PREV_PIC, on_picture_prev_pic)
  ON_COMMAND(MSG_NEXT_PIC, on_picture_next_pic)
  ON_COMMAND(MSG_PIC_PLAY, on_picture_resume)
  ON_COMMAND(MSG_PIC_PAUSE, on_picture_pause)
  ON_COMMAND(MSG_PLUG_OUT, on_picture_plug_out)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
  ON_COMMAND(MSG_PIC_TIMER, on_picture_timer)
  ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_picture_too_large)
  ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, on_picture_unsupport)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, on_picture_data_error)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERR, on_picture_fsceen_data_error)
  ON_COMMAND(MSG_MUSIC_PLAY_END, on_music_play_end)
  #if 0
  ON_COMMAND(MSG_MUSIC_NEXT, on_picture_next_music)
  ON_COMMAND(MSG_MUSIC_PRE, on_picture_prev_music)
  ON_COMMAND(MSG_VOLUME_UP, on_pic_list_volume)
  ON_COMMAND(MSG_VOLUME_DOWN, on_pic_list_volume)
  #endif
END_MSGPROC(picture_cont_proc, ui_comm_root_proc)


