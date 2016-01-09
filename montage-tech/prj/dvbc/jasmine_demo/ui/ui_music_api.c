/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_config.h"
#include "ui_usb_music.h"
#include "ui_music_api.h"
#include "ui_volume.h"
#include "str_filter.h"
#include "music_api.h"
#ifdef FILEPLAY_MUSIC
#include "music_api_mplayer.h"
#else
#include "music_api_mt.h"
#endif
#include "ui_menu_manager.h"
#include "common_filter.h"

typedef struct music_delete_flag
{
  int index;
  struct music_delete_flag *p_next;
}music_delete_flag_t;


static u8 *g_music_mem = NULL;
static u32 g_music_mem_size = 0;

u16 g_dir_count = 0;
static media_fav_t *g_music_play_list_dir = NULL;
static media_fav_t *g_music_play_list_fav = NULL;
static u16 g_music_play_index_fav = 0;
static u16 g_music_play_index_dir = 0;

static music_play_state_t g_music_play_stat = MUSIC_STAT_INVALID;

static media_fav_t *g_p_fav_music_list = NULL;
static mlist_play_mode_t g_music_play_mode = PLAY_MODE_CYCLE_CUR_DIR_ORDER;
static music_playlist_type_t g_music_playlist_type = MUSIC_PLAY_CUR_DIR;
static u8 g_music_state = 0x00;

static music_delete_flag_t *g_music_del = NULL;
static BOOL g_music_modified = FALSE;

static u32 g_music_handle = 0;
static handle_t g_music_rsc_handle = 0;
static handle_t music_region = NULL;

#define MUSIC_STATE (0X01)

#define DAY_PER_YEAR 365
#define MONTH_PER_YEAR 12
#define DAYS_OF_DEC 31
#define HOUR_PER_DAY 24
#define MINUTE_PER_HOUR 60
#define SECOND_PER_MINUTE 60
#define TICK_PER_SECOND 100

#define MUSIC_PREV_LR_W  10
#define MUSIC_PREV_TB_H  10

extern u32 g_video_fw_cfg_addr;
extern u32 g_video_fw_cfg_size;
extern u32 g_audio_fw_cfg_size;
extern u32 g_audio_fw_cfg_addr;
extern u32 g_video_file_play_addr;                       //fileplay addr   and  pvr play addr
extern u32 g_video_file_play_size;                       //for fileplay size = PVR rec size + PVR play size

typedef enum
{
  /*!
    file play end
    */
  UI_MUSIC_EVT_PLAY_END = ((APP_MUSIC_PLAYER << 16) + 0),
  /*!
    can not play the file
    */
  UI_MUSIC_EVT_CANNOT_PLAY,
  /*!
    music player get play time.
    */
  UI_MUSIC_EVT_GET_PLAY_TIME,
  /*!
    music stopped
    */
  UI_MUSIC_EVT_STOPPED,
}ui_music_api_evt_t;

/*!
   gui map info.
  */
typedef struct
{
  /*!
     unicode
    */
  //u16 *p_line_str_code;
  /*!
    string convert(for arabic string)
    */
  u16 *p_str_convert;
  /*!
    string base(for arabic string)
    */
  u16 *p_str_base;    
  /*!
     char pos.
    */
  u16 *p_char_pos;
  /*!
     max count per line.
    */
  u32 line_str_max_cnt;
  /*!
     max lines.
    */
  u32 line_str_max_lines;
  /*!
    line width. 
    */
  u16 *p_linew;
  /*!
    line height. 
    */
  u16 *p_lineh; 
  /*!
    line position. 
    */
  u16 **pp_linep;  
  /*!
    line character numbers. 
    */
  u16 *p_lines;     
  /*!
    resource handle
    */
  handle_t rsc_handle;
  /*!
    chip ic.
    */
  chip_ic_t chip_ic;  
}gui_main_t;

u16 ui_music_evtmap(u32 event);




static void ui_music_callback(u32 pic_handle, u32 content, u32 para1, u32 para2)
{
  event_t evt = {0};

  evt.data1 = para1;
  evt.data2 = para2;
  switch(content)
  {
    case MUSIC_API_EVT_GET_PLAY_TIME:
      evt.id = UI_MUSIC_EVT_GET_PLAY_TIME;
      break;
    case MUSIC_API_EVT_PLAY_END:
      OS_PRINTF("ui_music_callback: MUSIC_API_EVT_PLAY_END !!!");
      evt.id = UI_MUSIC_EVT_PLAY_END;
      break;
    default:
      break;
  }
  
  if (evt.id != 0)
    ap_frm_send_evt_to_ui(APP_MUSIC_PLAYER, &evt);

}


u32 ui_music_get_handle()
{
   return g_music_handle;
}

static u32 convert_time_to_sec(utc_time_t *p_dur_time)
{
  u32 sec = 0;

  sec = (((p_dur_time->year * DAY_PER_YEAR+p_dur_time->day)
        * HOUR_PER_DAY + p_dur_time->hour)
          * MINUTE_PER_HOUR + p_dur_time->minute)
            *SECOND_PER_MINUTE + p_dur_time->second;
  return sec;
}

RET_CODE ui_music_get_glyph(glyph_input_t *p_input, glyph_output_t *p_output)
{
  rsc_char_info_t info = {0};
  rsc_fstyle_t *p_fstyle = NULL;
  BOOL ret_boo = FALSE;
  
  MT_ASSERT(g_music_rsc_handle != 0);

  if(p_input->str_style & STR_HL)
  {
    p_fstyle = rsc_get_fstyle(g_music_rsc_handle, FSI_RED);
  }
  else
  {
    p_fstyle = rsc_get_fstyle(g_music_rsc_handle, FSI_WHITE);
  }

  ret_boo = rsc_get_char(g_music_rsc_handle, p_fstyle, p_input->char_code, &info);

  if(ret_boo != TRUE)
  {
    return ERR_FAILURE;
  }

  p_output->width = info.width;
  p_output->height = info.height;
  p_output->alpha_pitch = info.alpha_pitch;
  p_output->ckey = info.ckey;
  p_output->enable_ckey = TRUE;

  p_output->p_char = info.p_char;
  p_output->p_alpha = info.p_alpha;
  p_output->p_strok_char = info.p_strok_char;
  p_output->p_strok_alpha = info.p_strok_alpha;

  p_output->step_height = info.step_height;
  p_output->step_width = info.step_width;
  p_output->x_step = info.x_step;

  p_output->xoffset = info.xoffset;
  p_output->yoffset = info.yoffset;
  p_output->pitch = info.pitch;


  return SUCCESS;

}
//lint -e438 -e550 -e830
#ifndef WIN32
static void ui_music_region_init(u8 *p_buffer)
{
  rect_size_t rect_size = {0};
  point_t pos;
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                          SYS_DEV_TYPE_DISPLAY);
  pos.x = 0;
  pos.y = 0;
  rect_size.w = SCREEN_WIDTH;
  rect_size.h = SCREEN_HEIGHT;
  music_region = region_create(&rect_size, PIX_FMT_ARGB8888);
  MT_ASSERT(NULL != music_region);

  memset(p_buffer, 0, MUSIC_REGION_USED_SIZE);
  ret = disp_layer_add_region_ex(p_disp_dev, DISP_LAYER_ID_OSD0, music_region, &pos
                              , (void *)(((u32)p_buffer) | 0xa0000000),(void *)(((u32)(p_buffer +(MUSIC_REGION_USED_SIZE/2))) | 0xa0000000)  );
  MT_ASSERT(SUCCESS == ret);
  region_show(music_region, TRUE);

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_OSD0, TRUE);
}

static void ui_music_region_release(void)
{
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                          SYS_DEV_TYPE_DISPLAY);

  if(NULL != music_region)
  {
    ret = disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_OSD0, music_region);
    MT_ASSERT(SUCCESS == ret);

    ret = region_delete(music_region);
    MT_ASSERT(SUCCESS == ret);
    music_region = NULL;

    disp_layer_show(p_disp_dev, DISP_LAYER_ID_OSD0, FALSE);
  }
}
#endif
//lint +e438 +e550 +e830

void ui_music_create_region()
{
#ifndef WIN32
  u8 *p_region_buffer = NULL;

  g_music_mem = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(g_music_mem != NULL);
  g_music_mem_size = MUSIC_MODULE_USED_SIZE;

  mem_mgr_release_block(BLOCK_AV_BUFFER);
  MT_ASSERT(g_music_mem != NULL);

  p_region_buffer = g_music_mem + MUSIC_MODULE_USED_SIZE;

  ui_music_region_init(p_region_buffer);
#else
  music_region = (void *)gdi_get_screen_handle(FALSE);
#endif
}

void ui_music_release_region()
{
#ifndef WIN32
  ui_music_region_release();
#endif
}

void ui_music_player_init(void)
{
  music_chain_t music_chain = {0};
  rsc_config_t rsc_cfg = {0};
  u8 *p_region_buffer = NULL;

  OS_PRINTF("\n##debug: music_player_start 1!\n");

  g_music_mem = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(g_music_mem != NULL);
  g_music_mem_size = MUSIC_MODULE_USED_SIZE;

  mem_mgr_release_block(BLOCK_AV_BUFFER);
  MT_ASSERT(g_music_mem != NULL);

  p_region_buffer = g_music_mem + MUSIC_MODULE_USED_SIZE;
#ifndef WIN32
  ui_music_region_init(p_region_buffer);
#else
  music_region = (void *)gdi_get_screen_handle(FALSE);
#endif

  fw_register_ap_evtmap(APP_MUSIC_PLAYER, ui_music_evtmap);
  fw_register_ap_msghost(APP_MUSIC_PLAYER, ROOT_ID_USB_MUSIC);

  memcpy(&rsc_cfg, &g_rsc_config, sizeof(rsc_cfg));

  rsc_cfg.bmp_buf_size = 0;
  g_music_rsc_handle = rsc_init(&rsc_cfg);

#ifdef FILEPLAY_MUSIC  
  mul_music_mplayer_init();
#else
  mul_music_mt_init();
#endif
  {
    vf_ft_cfg_t vfont_cfg = {0};
    vfont_cfg.max_cnt = 36; //a~z, 0~9
    vfont_cfg.max_height = 32;
    vfont_cfg.max_width = 32;
    vfont_cfg.is_alpha_spt = TRUE;
    vfont_cfg.bpp = 32;
    
    vf_ft_attach(g_music_rsc_handle, &vfont_cfg);
  }

  //register picture ui to ap
  fw_register_ap_msghost(APP_MUSIC_PLAYER, ROOT_ID_USB_PICTURE);

  music_chain.task_stk_size = AP_MUSIC_TASK_STKSIZE;
  music_chain.rsc_get_glyph = ui_music_get_glyph;
#ifdef FILEPLAY_MUSIC
  music_chain.fs_task_prio = FILE_PLAYBACK_PRIORITY;
  music_chain.fs_task_size = FILE_PLAYBACK_STKSIZE;
  music_chain.vdec_start = g_video_fw_cfg_addr;
  music_chain.vdec_size = g_video_fw_cfg_size;
  music_chain.audio_start = g_audio_fw_cfg_addr;
  music_chain.audio_size = g_audio_fw_cfg_size;

  music_chain.v_mem_size = g_video_file_play_size;
  music_chain.v_mem_start = g_video_file_play_addr;

  music_chain.net_task_priority = FILE_PLAY_NET_TASK_PRIORITY;
  
  #ifdef MIN_AV_64M
    #ifdef MIN_AV_SDONLY
      music_chain.vdec_policy = VDEC_SDINPUT_ONLY;
    #else
      music_chain.vdec_policy = VDEC_BUFFER_AD_UNUSEPRESCALE;
    #endif
  #else
    music_chain.vdec_policy = VDEC_OPENDI_64M;
  #endif
#endif
  music_create_chain(&g_music_handle, &music_chain);

  if(g_music_handle != 0)
  {
     OS_PRINTF("\n##debug: music_regist_callback !\n");
     music_regist_callback(g_music_handle, ui_music_callback);
  }

}

void ui_music_player_release(void)
{
  if(g_music_handle != 0)
  {
    OS_PRINTF("\n#debug :ui_music_player_release!\n");

    music_destroy_chain(g_music_handle);

    g_music_handle = 0;
  }

  if(g_music_rsc_handle != 0)
  {
#ifdef FILEPLAY_MUSIC
    mul_music_mplayer_deinit();
#else
    mul_music_mt_deinit();
#endif
    vf_ft_detach(g_music_rsc_handle);

    rsc_release(g_music_rsc_handle);

    g_music_rsc_handle = 0;
  }

  fw_unregister_ap_evtmap(APP_MUSIC_PLAYER);
  fw_unregister_ap_msghost(APP_MUSIC_PLAYER, ROOT_ID_USB_MUSIC);
  fw_unregister_ap_msghost(APP_MUSIC_PLAYER, ROOT_ID_USB_PICTURE);
}

static void ui_cfg_music_data(rect_t logo_rect,rect_t lrc_rect, u16 *p_filename)
{
  music_logo_info_t logo_info = {{0}};
  music_lrc_info_t  lrc_info = {0};
  str_cfg_t        str_cfg = {{0}};

  str_cfg.pal_num = 0;
  str_cfg.bpp = 32;
  str_cfg.fmt = PIX_FMT_ARGB8888;
  str_cfg.win_bg = 0;
  str_cfg.use_font_bg = TRUE;
  str_cfg.font_bg = C_BLACK;

  lrc_info.line_cunt = 15;
  lrc_info.lrc_rect = lrc_rect;
  lrc_info.p_lrc_rgn = (void *)gdi_get_screen_handle(FALSE);
  lrc_info.str_cfg = str_cfg;

  logo_info.p_buffer = g_music_mem;
  logo_info.buffer_size = g_music_mem_size;
  logo_info.logo_rect = logo_rect;
  logo_info.p_logo_rgn = music_region;

  music_set_filename(g_music_handle, p_filename);
  music_logo_set_info(g_music_handle, &logo_info);
  music_lrc_set_info(g_music_handle, &lrc_info);

}

void ui_music_lrc_show(BOOL bshow)
{
   music_lrc_show(g_music_handle, bshow);
}

void ui_music_logo_show(BOOL bshow, rect_t logo_rect)
{
   music_logo_info_t logo_info;

   logo_info.p_buffer = g_music_mem;
   logo_info.buffer_size = g_music_mem_size;
   logo_info.logo_rect = logo_rect;
   logo_info.p_logo_rgn = music_region;

   music_logo_show(g_music_handle, bshow, &logo_info);
}


void ui_music_fullscreen_player(u16 *p_file_name)
{
  rect_t logo_rect = {0};
  rect_t lrc_rect = {0};
  music_logo_info_t logo_info;

  logo_rect.left = MUSIC_FULL_PICTURE_X + COMM_WIN_SHIFT_X + MUSIC_PREV_LR_W;
  logo_rect.top = MUSIC_FULL_PICTURE_Y + COMM_WIN_SHIFT_Y + MUSIC_PREV_TB_H;
  logo_rect.right = MUSIC_FULL_PICTURE_X + COMM_WIN_SHIFT_X + MUSIC_FULL_PICTURE_W - 2 * MUSIC_PREV_LR_W;
  logo_rect.bottom = MUSIC_FULL_PICTURE_Y + COMM_WIN_SHIFT_Y + MUSIC_FULL_PICTURE_H - 2 * MUSIC_PREV_TB_H;
  logo_info.p_buffer = g_music_mem;
  logo_info.buffer_size = g_music_mem_size;
  logo_info.logo_rect = logo_rect;
  logo_info.p_logo_rgn = music_region;

  lrc_rect.left = MUSIC_FULL_LRC_X + MUSIC_PREV_LR_W + SCREEN_POS_X;
  lrc_rect.top = MUSIC_FULL_LRC_Y + MUSIC_PREV_TB_H + SCREEN_POS_Y;
  lrc_rect.right = MUSIC_FULL_LRC_X + MUSIC_FULL_LRC_W - 2 * MUSIC_PREV_LR_W + SCREEN_POS_X;
  lrc_rect.bottom = MUSIC_FULL_LRC_Y + MUSIC_FULL_LRC_H - 2 * MUSIC_PREV_TB_H + SCREEN_POS_Y;

  ui_cfg_music_data(logo_rect, lrc_rect, p_file_name);
  music_lrc_show(g_music_handle, TRUE);
  music_logo_show(g_music_handle, TRUE, &logo_info);
}


static void music_volume_init()
{
    u8 volume = sys_status_get_global_media_volume();
    set_volume(volume);
}


void music_player_start(u16 *p_file_name)
{
  rect_t lrc_rect = {0};

  rect_t logo_rect =  {COMM_WIN_SHIFT_X + MUSIC_PREV_X + 8, COMM_WIN_SHIFT_Y + MUSIC_PREV_Y + 8,
                       COMM_WIN_SHIFT_X +  MUSIC_PREV_X + MUSIC_PREV_W - 2 * 8,
                       COMM_WIN_SHIFT_Y + MUSIC_PREV_Y + MUSIC_PREV_H - 2 * 8};

  music_volume_init();
  ui_cfg_music_data(logo_rect,lrc_rect,p_file_name);

  music_set_filename(g_music_handle, p_file_name);
  music_start(g_music_handle);
  music_lrc_show(g_music_handle, FALSE);
  ui_music_logo_show(TRUE, logo_rect);
  ui_music_set_play_status(MUSIC_STAT_PLAY);

}

void music_player_stop(void)
{
  OS_PRINTF("\n##debug: music_player_stop!\n");

  music_stop(g_music_handle);

  ui_music_set_play_status(MUSIC_STAT_STOP);
}

void music_player_next(u16 *p_file_name)
{
  rect_t logo_rect = {0};
  OS_PRINTF("\n##debug: music_player_next !!!\n");

  //music_stop(g_music_handle);
  //music_set_filename(g_music_handle, p_file_name);
  //music_start(g_music_handle);

  if(music_region == NULL)
  {
     ui_music_logo_show(FALSE, logo_rect);
  }

  if(ui_music_is_fullscreen_play())
  {
    music_stop(g_music_handle);
    ui_music_fullscreen_player(p_file_name);
    music_start(g_music_handle);
  }
  else
  {
    music_player_stop();
    music_player_start(p_file_name);
  }
}

void music_player_pause(void)
{
  OS_PRINTF("\n##debug: music_player_pause!\n");

  music_pause(g_music_handle);

  ui_music_set_play_status(MUSIC_STAT_PAUSE);
}

void music_player_resume(void)
{
	OS_PRINTF("\n##debug: music_player_resume!\n");

	music_resume(g_music_handle);
}

void ui_music_set_playlist_type(music_playlist_type_t music_plist_type)
{
  g_music_playlist_type = music_plist_type;
}

music_playlist_type_t ui_music_get_playlist_type()
{
  return g_music_playlist_type;
}

void ui_music_set_play_status(music_play_state_t state)
{
  g_music_play_stat = state;
}

music_play_state_t ui_music_get_play_status(void)
{
  return g_music_play_stat;
}

void ui_music_set_play_mode(mlist_play_mode_t mode)
{
  g_music_play_mode = mode;
}

mlist_play_mode_t ui_music_get_play_mode(void)
{
  return g_music_play_mode;
}

void ui_music_unload_fav_list(void)
{
  flist_unload_fav(g_p_fav_music_list);
  g_p_fav_music_list = NULL;
}

void ui_music_get_fav_list(media_fav_t **pp_desmedia)
{
  *pp_desmedia = g_p_fav_music_list;
}

void ui_music_set_play_index_dir_by_name(u16 *p_name)
{
  u8 index=0;
  media_fav_t *p_temp = g_music_play_list_dir;
  while (p_temp)
  {
    if (uni_strcmp(p_name, p_temp->path) == 0)
    {
      ui_music_set_play_index_dir(index);
      break;
    }

    index++;

    if (p_temp->p_next == NULL)
    {
      break;
    }
    p_temp = p_temp->p_next;
  }
}

BOOL ui_music_build_play_list_indir(file_list_t *p_filelist)
{
  u32 i;
  flist_type_t file_type = FILE_TYPE_UNKNOW;
  media_fav_t *p_temp = NULL;

  g_dir_count = 0;
  while(g_music_play_list_dir)
  {
    p_temp = g_music_play_list_dir;
    g_music_play_list_dir = g_music_play_list_dir->p_next;
    mtos_free(p_temp);
  }

  g_music_play_list_dir = p_temp = NULL;

  for(i=0; i<p_filelist->file_count; i++)
  {
    if(p_filelist->p_file[i].type == NOT_DIR_FILE)
    {
      file_type = flist_get_file_type(p_filelist->p_file[i].p_name);

      if (file_type == FILE_TYPE_MP3)
      {
        if (p_temp)
        {
          p_temp->p_next = mtos_malloc(sizeof(media_fav_t));
          MT_ASSERT(p_temp->p_next != NULL);
          memset(p_temp->p_next, 0, sizeof(media_fav_t));
          p_temp = p_temp->p_next;
        }
        else
        {
          p_temp = mtos_malloc(sizeof(media_fav_t));
          MT_ASSERT(p_temp != NULL);
          //lint -e668
          memset(p_temp, 0, sizeof(media_fav_t));
          //lint +e668
          g_music_play_list_dir = p_temp;
        }

        p_temp->p_next = NULL;
        memset(p_temp->path, 0, MAX_FILE_PATH * sizeof(u16));
        uni_strcpy(p_temp->path, p_filelist->p_file[i].name);
      }
    }
    else
    {
      g_dir_count ++;
    }
  }

  return TRUE;
}


BOOL ui_music_build_play_list_infav()
{
  g_music_play_list_fav = g_p_fav_music_list;

  return TRUE;
}

u8 ui_music_get_play_count_of_fav()
{
  media_fav_t *p_media=NULL;
  u8 count=0;

  p_media = g_music_play_list_fav;

  while(p_media)
  {
    count++;
    p_media = p_media->p_next;
  }

  return count;
}

u8 ui_music_get_play_count_of_dir()
{
  media_fav_t *p_media=NULL;
  u8 count=0;

  p_media = g_music_play_list_dir;

  while(p_media)
  {
    count++;
    p_media = p_media->p_next;
  }

  return count;
}

void ui_music_set_play_index_dir(u16 index)
{
  g_music_play_index_dir = index;
}

u16 ui_music_get_play_index_dir(void)
{
  return g_music_play_index_dir;
}

void ui_music_set_play_index_fav(u16 index)
{
  g_music_play_index_fav = index;
}

u16 ui_music_get_play_index_fav(void)
{
  return g_music_play_index_fav;
}

BOOL ui_music_set_play_index_fav_by_name(u16 *p_name)
{
  u8 index=0;
  media_fav_t *p_temp = g_music_play_list_fav;
  while (p_temp)
  {
    if (uni_strcmp(p_name, p_temp->path) == 0)
    {
      ui_music_set_play_index_fav(index);
      return TRUE;
    }

    index++;

    if (p_temp->p_next == NULL)
    {
      return FALSE;
    }
    p_temp = p_temp->p_next;
  }

  return FALSE;
}

BOOL ui_music_pre_start_play(media_fav_t **pp_media)
{
  mlist_play_mode_t mode;
  RET_CODE ret = FALSE;

  mode = ui_music_get_play_mode();

  switch(mode)
  {
    case PLAY_MODE_CYCLE_CUR_DIR_ORDER:
    case PLAY_MODE_CYCLE_CUR_DIR_RANDOM:
    case PLAY_MODE_CYCLE_CUR_DIR_SELF:
    case PLAY_MODE_CYCLE_CUR_DIR_NONE:
      if(g_music_playlist_type == MUSIC_PLAY_CUR_DIR)
      {
        *pp_media = flist_get_fav_by_index(g_music_play_list_dir, g_music_play_index_dir);
      }
      else if(g_music_playlist_type == MUSIC_PLAY_FAV_LIST)
      {
        *pp_media = flist_get_fav_by_index(g_music_play_list_fav, g_music_play_index_fav);
      }
      break;

    default:
      break;
  }

  OS_PRINTF("\n\n ui_music_pre_start_play:[%s] \n\n",(*pp_media)->path);
  ui_music_set_play_status(MUSIC_STAT_PLAY);
  ui_music_state_set();
  if(*pp_media != NULL)
  {
     ret = TRUE;
  }

  return ret;
}

BOOL ui_music_pre_play_pre(media_fav_t **pp_media)
{
  mlist_play_mode_t mode;
  u8 count_fav=0;
  u8 count_dir=0;

  mode = ui_music_get_play_mode();

  switch(mode)
  {
    case PLAY_MODE_CYCLE_CUR_DIR_ORDER:
      if(g_music_playlist_type == MUSIC_PLAY_CUR_DIR)
      {
        count_dir = ui_music_get_play_count_of_dir();

        if(count_dir == 0)
        {
          return FALSE;
        }

        if(g_music_play_index_dir == 0)
        {
          g_music_play_index_dir = count_dir;
        }

        g_music_play_index_dir--;
        *pp_media = flist_get_fav_by_index(g_music_play_list_dir, g_music_play_index_dir);
      }
      else
      {
        count_fav = ui_music_get_play_count_of_fav();

        if(count_fav == 0)
        {
          return FALSE;
        }

        if(g_music_play_index_fav == 0)
        {
          g_music_play_index_fav = count_fav;
        }

        g_music_play_index_fav--;

        *pp_media = flist_get_fav_by_index(g_music_play_list_fav, g_music_play_index_fav);
      }
      break;

    default:
      break;
  }
  if (*pp_media == NULL)
  {
    return FALSE;
  }

  OS_PRINTF("\n##debug: ui_music_pre_play_next [%s]\n", (*pp_media)->path);
  return TRUE;
}

BOOL ui_music_pre_play_next(media_fav_t **pp_media)
{
  mlist_play_mode_t mode;
  u8 count_fav=0;
  u8 count_dir=0;
  u32 seed=0;
  utc_time_t timenow = {0};

  mode = ui_music_get_play_mode();
  switch(mode)
  {
    case PLAY_MODE_CYCLE_CUR_DIR_ORDER:
      if(g_music_playlist_type == MUSIC_PLAY_CUR_DIR)
      {
        count_dir = ui_music_get_play_count_of_dir();
        if(count_dir == 0)
        {
          return FALSE;
        }

        g_music_play_index_dir++;
        g_music_play_index_dir %= count_dir;
        *pp_media = flist_get_fav_by_index(g_music_play_list_dir, g_music_play_index_dir);
      }
      else
      {
        count_fav = ui_music_get_play_count_of_fav();
        if(count_fav == 0)
        {
          return FALSE;
        }

        g_music_play_index_fav++;
        g_music_play_index_fav %= count_fav;
        *pp_media = flist_get_fav_by_index(g_music_play_list_fav, g_music_play_index_fav);
      }
      break;

    case PLAY_MODE_CYCLE_CUR_DIR_RANDOM:
      time_get(&timenow, TRUE);
      seed = convert_time_to_sec(&timenow);
      srand(seed);
      if(g_music_playlist_type == MUSIC_PLAY_CUR_DIR)
      {
        count_dir = ui_music_get_play_count_of_dir();
        if(count_dir == 0)
        {
          return FALSE;
        }

        g_music_play_index_dir = (u8)rand()%count_dir;
        OS_PRINTF("rand num = %d, seed=%ld\n",g_music_play_index_dir,seed);
        ////OS_PRINTF("rand num = %d, seed=%ld\n",g_music_play_index_dir,seed);
        *pp_media = flist_get_fav_by_index(g_music_play_list_dir, g_music_play_index_dir);
      }
      else
      {
        count_fav = ui_music_get_play_count_of_fav();
        if(count_fav == 0)
        {
          return FALSE;
        }

        g_music_play_index_fav = (u8)(rand()%count_fav);
        OS_PRINTF("rand num = %d, seed=%ld\n",g_music_play_index_fav,seed);
        ////OS_PRINTF("rand num = %d, seed=%ld\n",g_music_play_index_fav,seed);
        *pp_media = flist_get_fav_by_index(g_music_play_list_fav, g_music_play_index_fav);
      }
      break;

    case PLAY_MODE_CYCLE_CUR_DIR_SELF:
      if(g_music_playlist_type == MUSIC_PLAY_CUR_DIR)
      {
        *pp_media = flist_get_fav_by_index(g_music_play_list_dir, g_music_play_index_dir);
      }
      else
      {
        *pp_media = flist_get_fav_by_index(g_music_play_list_fav, g_music_play_index_fav);
      }
      break;

    case PLAY_MODE_CYCLE_CUR_DIR_NONE:
      return FALSE;

    default:
      break;
  }
  if (*pp_media == NULL)
  {
    return FALSE;
  }

  OS_PRINTF("\n##debug: ui_music_pre_play_next [%s]\n", (*pp_media)->path);
  return TRUE;
}

BOOL ui_music_get_cur(media_fav_t **pp_media)
{
  mlist_play_mode_t mode;
  BOOL bRet = TRUE;

  mode = ui_music_get_play_mode();

  switch(mode)
  {
    case PLAY_MODE_CYCLE_CUR_DIR_ORDER:
    case PLAY_MODE_CYCLE_CUR_DIR_RANDOM:
    case PLAY_MODE_CYCLE_CUR_DIR_SELF:
    case PLAY_MODE_CYCLE_CUR_DIR_NONE:
      if((g_music_playlist_type == MUSIC_PLAY_CUR_DIR) && (g_music_play_list_dir != NULL))
      {
        *pp_media = flist_get_fav_by_index(g_music_play_list_dir, g_music_play_index_dir);
      }
      else if(g_music_play_list_fav != NULL)
      {
        *pp_media = flist_get_fav_by_index(g_music_play_list_fav, g_music_play_index_fav);
      }
      break;

    default:
      bRet = FALSE;
      break;
  }

  return bRet;
}

void ui_music_state_set(void)
{
  g_music_state = MUSIC_STATE;
}

void ui_music_state_clear(void)
{
  g_music_state = 0;
}

BOOL ui_music_state_get(void)
{
  if(g_music_state == MUSIC_STATE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void ui_music_load_fav_list(u16 letter)
{
  media_fav_t *p_media = NULL;
  media_fav_t *p_media_pre = NULL;
  if(g_p_fav_music_list)
  {
      p_media = g_p_fav_music_list;
      while(p_media)
      {
          p_media_pre = p_media;
          p_media = p_media->p_next;
      }
      flist_load_fav(letter, (void **)(&p_media), AUDIO_FAV);
      if(p_media_pre != NULL)
        p_media_pre->p_next =  p_media;
  }else{
      flist_load_fav(letter, (void **)(&g_p_fav_music_list), AUDIO_FAV);
  }
}

void ui_music_add_one_fav(u16 *p_name)
{
  flist_add_fav_by_name(p_name, (void **)(&g_p_fav_music_list), AUDIO_FAV);
}

void ui_music_del_one_fav(u16 index)
{
  media_fav_t *p_media=NULL;
  u16 i;

  p_media = g_p_fav_music_list;

  for (i = 0; i<index && p_media!=NULL; i++)
  {
    p_media = p_media->p_next;
  }

  if(p_media == NULL)
  {
    return;
  }

  flist_del_fav((void **)(&g_p_fav_music_list), p_media);
}
//lint -e438 -e550 -e830
void ui_music_save_fav_list(u16 letter)
{
 
  u32 next_len = 0;
  media_fav_t *p_fav_music_list = NULL;
  media_fav_t *p_media = NULL;
  media_fav_t *p_fav_next =NULL;
  media_fav_t *p_first = NULL;
  media_fav_t *p_next = NULL;
  p_media = g_p_fav_music_list;

  next_len = sizeof(media_fav_t);
  while(p_media)
  {
      if(p_media->path[0] != letter)
      {
          p_media = p_media->p_next;
          continue;
      }
      if(p_fav_music_list)
      {
        if(p_fav_next != NULL)
        {
          p_fav_next->p_next = mtos_malloc(next_len);
          MT_ASSERT(p_fav_next->p_next != NULL);
          p_fav_next = p_fav_next->p_next;
          p_fav_next->p_next = NULL;
        }
      } else {
        p_fav_next = mtos_malloc(next_len);
        MT_ASSERT(p_fav_next != NULL);
        p_fav_music_list = p_fav_next;
        //lint -e613
        p_fav_next->p_next = NULL;
        //lint +e613
      }
      if(p_fav_next != NULL)
      {
        memset(p_fav_next, 0, next_len);
        uni_strcpy(p_fav_next->path, p_media->path);
        p_fav_next->p_filename = p_media->p_filename;
        p_media = p_media->p_next;
      }
  }

  flist_save_fav(letter, p_fav_music_list, AUDIO_FAV);

  p_first = p_fav_music_list;
  while (p_first)
  {
    p_next = p_first;
    p_first = p_next->p_next;
    mtos_free(p_next);
    p_next = NULL;
  }
  p_fav_music_list = NULL;
}
//lint +e438 +e550 +e830
u32 ui_music_get_fav_count(void)
{
  media_fav_t *p_media = NULL;
  u32 count=0;

  p_media = g_p_fav_music_list;

  while(p_media)
  {
    count++;
    p_media = p_media->p_next;
  }

  return count;
}

void ui_music_undo_save_del(void)
{
  music_delete_flag_t *p_temp = NULL;

  while(g_music_del)
  {
    p_temp = g_music_del;
    g_music_del = g_music_del->p_next;

    mtos_free(p_temp);
  }

  g_music_modified = FALSE;
}

void ui_music_set_one_del(u16 index)
{
  music_delete_flag_t *temp = NULL;
  music_delete_flag_t *p_new = NULL;
  music_delete_flag_t *temp2 = NULL;

  g_music_modified = TRUE;

  if(g_music_del != NULL)
  {
    temp = g_music_del;

    if(g_music_del->index == index)
    {
      if(g_music_del->p_next == NULL)
      {
        mtos_free(g_music_del);
        g_music_del = NULL;
      }
      else
      {
        g_music_del = g_music_del->p_next;
        mtos_free(temp);
      }

      return;
    }
    else
    {
      temp = g_music_del;
      temp2 = temp;
      temp = temp->p_next;

      while(temp != NULL)
      {
        //if the index is already del, then cancel the del
        if(temp->index == index)
        {
          temp2->p_next = temp->p_next;
          mtos_free(temp);
          return;
        }

        temp2 = temp;
        temp = temp->p_next;
      }
    }
  }

  if(g_music_del == NULL)
  {
    g_music_del = mtos_malloc(sizeof(music_delete_flag_t));
    MT_ASSERT(g_music_del != NULL);
    memset(g_music_del, 0, sizeof(music_delete_flag_t));
    g_music_del->index = index;
    g_music_del->p_next = NULL;
  }
  else
  {
    p_new = mtos_malloc(sizeof(music_delete_flag_t));
    MT_ASSERT(p_new != NULL);
    //lint -e668
    memset(p_new, 0, sizeof(music_delete_flag_t));
    //lint +e668
    p_new->index = index;
    p_new->p_next = NULL;

    temp = g_music_del;

    //add in order big-->small index
    while(temp != NULL)
    {
      temp2 = temp->p_next;

      if(p_new->index >= temp->index)
      {
        if(temp != g_music_del)
        {
          temp->p_next = p_new;
          p_new->p_next = temp2;
        }
        else
        {
          p_new->p_next = temp;
          g_music_del = p_new;
        }
        break;
      }
      else if(temp->p_next == NULL)
      {
        temp->p_next = p_new;
        break;
      }

      temp = temp->p_next;
    }
  }
}

BOOL ui_music_is_one_fav_del(u16 index)
{
  music_delete_flag_t *temp = g_music_del;
  BOOL ret_boo = FALSE;

  while(temp)
  {
    if(temp->index == index)
    {
      ret_boo = TRUE;
      break;
    }
    temp = temp->p_next;
  }

  return ret_boo;
}

void ui_music_save_del(u8 letter)
{
  music_delete_flag_t *p_temp = NULL;

  while(g_music_del)
  {
    ui_music_del_one_fav((u16)g_music_del->index);

    p_temp = g_music_del;
    g_music_del = g_music_del->p_next;

    mtos_free(p_temp);
  }

  ui_music_save_fav_list(letter);

}

BOOL ui_music_is_del_modified(void)
{
  return g_music_modified;
}

void ui_music_set_del_modified(BOOL  boMusicModified)
{
    g_music_modified = boMusicModified;
}

media_fav_t *ui_music_get_fav_media_by_index(u16 index)
{
  return flist_get_fav_by_index(g_music_play_list_fav, index);
}

void ui_music_logo_clear(u32 color)
{

  OS_PRINTF("music logo clear, 0x%x\n", color);

  if(g_music_handle != 0)
  {
    mul_music_logo_clear(g_music_handle, color);
  }
}

void ui_music_set_charset(str_fmt_t charset)
{
  mul_music_set_charset(g_music_handle, charset);
}

BEGIN_AP_EVTMAP(ui_music_evtmap)
  CONVERT_EVENT(UI_MUSIC_EVT_PLAY_END, MSG_MUSIC_EVT_PLAY_END)
  CONVERT_EVENT(UI_MUSIC_EVT_CANNOT_PLAY, MSG_MUSIC_EVT_CANNOT_PLAY)
  CONVERT_EVENT(UI_MUSIC_EVT_GET_PLAY_TIME,MSG_PLAYER_EVT_GET_PLAY_TIME)
END_AP_EVTMAP(ui_music_evtmap)
