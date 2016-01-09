/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_mutex.h"
#include "mtos_task.h"

#include "ui_common.h"
#include "str_filter.h"
#include "music_api.h"
#include "ui_music_player_service.h"

// TODO: Not test yet with open macro USE_THREAD
//#define USE_THREAD (1)

/*******************************************************************************/

#define MUSIC_PLAYER_MESSAGE_MAX (10)
#define MUSIC_PLAYER_THREAD_NAME "music service"
#define MUSIC_PLAYER_THREAD_STACK_SIZE ((4) * (KBYTES))//(DEFAULT_STKSIZE)
#define MUSIC_PLAYER_WAIT_FOREVER (0)

/*******************************************************************************/


static MUSIC_PLAYER_SERVCIE *instance = NULL;

extern u32 g_video_fw_cfg_addr;
extern u32 g_video_fw_cfg_size;
extern u32 g_audio_fw_cfg_size;
extern u32 g_audio_fw_cfg_addr;
extern u32 g_video_file_play_addr;                       //fileplay addr   and  pvr play addr
extern u32 g_video_file_play_size;                       //for fileplay size = PVR rec size + PVR play size

/*******************************************************************************/
static RET_CODE _stop(void);

/*******************************************************************************/

static void _music_api_callback(u32 handle, u32 content, u32 para1, u32 para2)
{
  //OS_PRINTF("_music_api_callback: content:%d handle:%p\n", content, handle);
  switch(content)
  {
    case MUSIC_API_EVT_GET_PLAY_TIME:
      break;
    case MUSIC_API_EVT_PLAY_END:
      OS_PRINTF("ui_music_callback: MUSIC_API_EVT_PLAY_END !!!");
      if (instance->play_end_cb != NULL)
      {
        instance->play_end_cb();
      }
      break;
    case MUSIC_API_EVT_CANNOT_PLAY:
      if (instance->play_error_cb != NULL)
      {
        instance->play_error_cb();
      }
    break;
    
    default:
      break;
  }
}

static RET_CODE _do_stop(void)
{
  RET_CODE ret;

  ret = music_stop(instance->music_api_handle);
  return ret;
}

static RET_CODE _do_play(const MUSIC_PLAY_LIST *item)
{
  RET_CODE ret = ERR_FAILURE;

  if (item == NULL)
  {
    _do_stop();
    return SUCCESS;
  }
  switch (item->uri.type)
  {
    case MEDIA_FILE:
      music_set_filename(instance->music_api_handle, (u16*)item->uri.str);
      //music_seek_time(instance->music_api_handle, 0);
      music_start(instance->music_api_handle);
    break;

    default:
      MT_ASSERT(0);
    break;
  }

  return ret;
}

static void _construct(void)
{
#ifdef USE_THREAD
  BOOL result;
#endif
  
  MT_ASSERT(instance != NULL);
  MT_ASSERT(instance->mutex == NULL);


  instance->mutex = mtos_mutex_create(1);
  MT_ASSERT(instance->mutex != NULL);
  
#ifdef USE_THREAD
  instance->message_queue = mtos_messageq_create(MUSIC_PLAYER_MESSAGE_MAX, "music service");
  MT_ASSERT(instance->message_queue != INVALID);
  //instance->media_player_service = new_media_player_service();

  instance->thread_id = MUSIC_PLAYER_CHAIN_PRIORITY;
  //instance->thread_stack = mtos_malloc(MUSIC_PLAYER_THREAD_STACK_SIZE);
  result = mtos_task_create((u8 *)MUSIC_PLAYER_THREAD_NAME,
                          instance->thread_entry,
                          NULL,
                          instance->thread_id,
                          NULL,
                          MUSIC_PLAYER_THREAD_STACK_SIZE);

  MT_ASSERT(result);
#endif
}

//lint -e438 -e550 -e830
static void _init(void)
{
  music_chain_t chain;
  RET_CODE ret;
  
  mtos_mutex_take(instance->mutex);
  memset(&chain, 0, sizeof(chain));
  chain.task_stk_size = AP_MUSIC_TASK_STKSIZE;
#ifdef FILEPLAY_MUSIC
  //chain.rsc_get_glyph = ui_music_get_glyph;
  chain.fs_task_prio = FILE_PLAYBACK_PRIORITY;
  chain.fs_task_size = FILE_PLAYBACK_STKSIZE;
  chain.vdec_start = g_video_fw_cfg_addr;
  chain.vdec_size = g_video_fw_cfg_size;
  chain.audio_start = g_audio_fw_cfg_addr;
  chain.audio_size = g_audio_fw_cfg_size;

  chain.v_mem_size = g_video_file_play_size;
  chain.v_mem_start = g_video_file_play_addr;

  chain.net_task_priority = FILE_PLAY_NET_TASK_PRIORITY;
  
  #ifdef MIN_AV_64M
    #ifdef MIN_AV_SDONLY
      chain.vdec_policy = VDEC_SDINPUT_ONLY;
    #else
      chain.vdec_policy = VDEC_BUFFER_AD_UNUSEPRESCALE;
    #endif
  #else
    chain.vdec_policy = VDEC_OPENDI_64M;
  #endif
#endif
  ret = music_create_chain(&instance->music_api_handle, &chain);
  MT_ASSERT(ret == SUCCESS);
  music_regist_callback(instance->music_api_handle, _music_api_callback);
  music_lrc_show(instance->music_api_handle, FALSE);
  music_logo_show(instance->music_api_handle, FALSE, NULL);
  mtos_mutex_give(instance->mutex);
}
//lint +e438 +e550 +e830
static void _deinit(void)
{
  mtos_mutex_take(instance->mutex);
  
  music_destroy_chain(instance->music_api_handle);
  
  mtos_mutex_give(instance->mutex);
}

static RET_CODE _add_search_list(MEDIA_URI uri)
{
  RET_CODE ret = ERR_FAILURE;
  MUSIC_PLAY_LIST *node;

  node = mtos_malloc(sizeof(MUSIC_PLAY_LIST));
  MT_ASSERT(node != NULL);
  //lint -e668
  memset(node, 0, sizeof(MUSIC_PLAY_LIST));
  node->uri = uri;
  //lint +e668
  
  mtos_mutex_take(instance->mutex);
  if (instance->search_list == NULL)
  {
    instance->search_list = node;
  }
  else
  {
    MUSIC_PLAY_LIST *p;
    
    p = instance->search_list;
    while (p->next != NULL)
    {
      p = p->next;
    }
    p->next = node;
    node->pre = p;
  }
  mtos_mutex_give(instance->mutex);

  return ret;
}


static void _reset_search_list(void)
{
  MUSIC_PLAY_LIST *p;
  MUSIC_PLAY_LIST *tmp;
  
  mtos_mutex_take(instance->mutex);
  p = instance->search_list;
  while (p != NULL)
  {
    tmp = p;
    p = p->next;
    mtos_free(tmp);
  }
  instance->search_list = NULL;
  mtos_mutex_give(instance->mutex);
}

static MUSIC_PLAY_LIST* _search_music (const u8 *path, MUSIC_SEARCH_OPT opt)
{
  u16 filter[32];
  flist_dir_t dir;
  file_list_t file_list = {0};
  u32 i;
  MEDIA_URI uri;
  u16 file_path[MAX_FILE_PATH];
  media_file_t *media_file;

  _reset_search_list();

  memset(filter, 0, sizeof(filter));
  str_asc2uni((u8 *)"|mp3|MP3", filter);
  memset(file_path, 0, sizeof(file_path));
  str_asc2uni(path, file_path);
  if (opt == MUSIC_SEARCH_OPT_RECURSION)
  {
    // TODO:
  }
  else
  {
    dir = file_list_enter_dir(filter, FILE_LIST_MAX_SUPPORT_CNT, file_path);
    if (dir == NULL)
    {
      return NULL;
    }
    file_list_get(dir, FLIST_UNIT_FIRST, &file_list);
    if (file_list.file_count == 0)
    {
      return NULL;
    }
    for (i = 0; i < file_list.file_count; i++)
    {
      memset(&uri, 0, sizeof(uri));
      uri.type = MEDIA_FILE;
      media_file = &file_list.p_file[i];
      if (media_file->type != NOT_DIR_FILE)
      {
        continue;
      }
      uni_strcpy(uri.str, media_file->name);
      //unicode_to_gb2312(media_file->name, uni_strlen(media_file->name), uri.str, sizeof(uri.str));
      _add_search_list(uri);
    }
    file_list_leave_dir(dir);
    return instance->search_list;
  }
  return NULL;
}
static RET_CODE _add_play_list(const MEDIA_URI uri)
{
  RET_CODE ret = ERR_FAILURE;
  MUSIC_PLAY_LIST *node;

  node = mtos_malloc(sizeof(MUSIC_PLAY_LIST));
  MT_ASSERT(node != NULL);
  //lint -e668
  memset(node, 0, sizeof(MUSIC_PLAY_LIST));
  node->uri = uri;
  //lint +e668
  
  mtos_mutex_take(instance->mutex);
  if (instance->play_list == NULL)
  {
    instance->play_list = node;
    instance->play_list_cur = node;
  }
  else
  {
    MUSIC_PLAY_LIST *p;
    
    p = instance->play_list;
    while (p->next != NULL)
    {
      p = p->next;
    }
    p->next = node;
    node->pre = p;
  }
  instance->play_list_item_count++;
  mtos_mutex_give(instance->mutex);

  return ret;
}

static void _reset(void)
{
  MUSIC_PLAY_LIST *p;
  MUSIC_PLAY_LIST *tmp;
  
  mtos_mutex_take(instance->mutex);
  p = instance->play_list;
  while (p != NULL)
  {
    tmp = p;
    p = p->next;
    mtos_free(tmp);
  }
  instance->play_list = NULL;
  instance->play_list_cur = NULL;
  instance->play_list_item_count = 0;
  mtos_mutex_give(instance->mutex);
}

static MUSIC_PLAY_LIST *_rand(void)
{
  s32 rand_num;
  MUSIC_PLAY_LIST *p;

  srand(mtos_ticks_get());
  if (instance->play_list_item_count == 0)
  {
    return NULL;
  }
  rand_num = rand() % (s32)instance->play_list_item_count;

  p = instance->play_list;
  while (p != NULL)
  {
    if (rand_num <= 0)
    {
      break;
    }
    p = p->next;
    rand_num--;
  }

  return p;
}


static RET_CODE _play(void)
{
  RET_CODE ret;

  mtos_mutex_take(instance->mutex);

  if (instance->play_mode == PLAY_MODE_LOOP_RANDOM
    || instance->play_mode == PLAY_MODE_SINGLE_RANDOM)
  {
    instance->play_list_cur = _rand();
  }

#ifdef USE_THREAD
  ret = instance->do_command(MUSIC_PLAY_CMD_PLAY, NULL, 0);
#else
  ret = _do_play(instance->play_list_cur);
#endif
  
  mtos_mutex_give(instance->mutex);
  return ret;
}


static RET_CODE _play_next(void)
{
  RET_CODE ret;

  mtos_mutex_take(instance->mutex);
  switch (instance->play_mode)
  {
    case PLAY_MODE_LOOP_SINGLE:
      // Do not need change current.
    break;

    case PLAY_MODE_LOOP_LIST:
      if (instance->play_list_cur->next != NULL)
      {
        instance->play_list_cur = instance->play_list_cur->next;
      }
      else
      {
        instance->play_list_cur = instance->play_list;
      }
    break;

    case PLAY_MODE_LOOP_RANDOM:
    {
      instance->play_list_cur = _rand();
    }
    break;

    case PLAY_MODE_SINGLE:
      instance->play_list_cur = NULL;
    break;

    case PLAY_MODE_SINGLE_RANDOM:
      if (instance->play_list_cur->next == NULL)
      {
        instance->play_list_cur = NULL;
      }
      else
      {
        instance->play_list_cur = _rand();
      }
    break;

    default:
    break;
  }

#ifdef USE_THREAD
  ret = instance->do_command(MUSIC_PLAY_CMD_PLAY_NEXT, NULL, 0);
#else
  ret = _do_play(instance->play_list_cur);
#endif
  
  mtos_mutex_give(instance->mutex);
  return ret;
}

static RET_CODE _play_pre(void)
{
  RET_CODE ret;

  mtos_mutex_take(instance->mutex);
  switch (instance->play_mode)
  {
    case PLAY_MODE_LOOP_SINGLE:
      // Do not need change current.
    break;

    case PLAY_MODE_LOOP_LIST:
      if (instance->play_list_cur->pre != NULL)
      {
        instance->play_list_cur = instance->play_list_cur->pre;
      }
      else
      {
        MUSIC_PLAY_LIST *p = instance->play_list;

        while (p->next != NULL)
        {
          p = p->next;
        }
        instance->play_list_cur = p;
      }
    break;

    case PLAY_MODE_LOOP_RANDOM:
    {
      instance->play_list_cur = _rand();
    }
    break;

    case PLAY_MODE_SINGLE:
      instance->play_list_cur = NULL;
    break;

    case PLAY_MODE_SINGLE_RANDOM:
      if (instance->play_list_cur->pre == NULL)
      {
        instance->play_list_cur = NULL;
      }
      else
      {
        instance->play_list_cur = _rand();
      }
    break;

    default:
    break;
  }

#ifdef USE_THREAD
  ret = instance->do_command(MUSIC_PLAY_CMD_PLAY_PRE, NULL, 0);
#else
  ret = _do_play(instance->play_list_cur);
#endif
  
  mtos_mutex_give(instance->mutex);
  return ret;
}



static RET_CODE _stop(void)
{
  RET_CODE ret;
  
  mtos_mutex_take(instance->mutex);

#ifdef USE_THREAD
  ret = instance->do_command(MUSIC_PLAY_CMD_STOP, NULL, 0);
#else
  ret = _do_stop();
#endif
  
  mtos_mutex_give(instance->mutex);

  return ret;
}

static void _set_play_mode(MUSIC_PLAY_MODE mode)
{
  mtos_mutex_take(instance->mutex);

  instance->play_mode = mode;
  
  mtos_mutex_give(instance->mutex);
}



static RET_CODE _do_cmd(MUSIC_PLAY_CMD cmd, void *data, u32 size)
{
  os_msg_t msg;
  BOOL result;

  memset(&msg, 0, sizeof(msg));
  msg.context = cmd;
  msg.para1 = (u32)data;
  msg.para2 = size;

  
  result = mtos_messageq_send(instance->message_queue, &msg);
  return result ? SUCCESS: ERR_FAILURE;
}

//lint -e438 -e550 -e716 -e830
static void _thread_entry(void *param)
{
  BOOL result;
  os_msg_t msg;
  
  while (1)
  {
    result = mtos_messageq_receive(instance->message_queue, &msg, MUSIC_PLAYER_WAIT_FOREVER);
    MT_ASSERT(result);

    switch (msg.context)
    {
      case MUSIC_PLAY_CMD_PLAY:
        mtos_mutex_take(instance->mutex);
        _do_play(instance->play_list_cur);
        mtos_mutex_give(instance->mutex);
      break;

      case MUSIC_PLAY_CMD_STOP:
        mtos_mutex_take(instance->mutex);
        _do_stop();
        mtos_mutex_give(instance->mutex);
      break;

      default:
      break;
    }
  }
}
//lint +e438 +e550 +e716 +e830
const MUSIC_PLAYER_SERVCIE * get_music_player_service_instance(void)
{
  if (instance != NULL)
  {
    return instance;
  }
  instance = mtos_malloc(sizeof(MUSIC_PLAYER_SERVCIE));
  MT_ASSERT(instance != NULL);
  memset(instance, 0, sizeof(MUSIC_PLAYER_SERVCIE));

  instance->construct = _construct;
  instance->init = _init;
  instance->deinit = _deinit;
  instance->do_command = _do_cmd;
  instance->thread_entry = _thread_entry;
  instance->search_music = _search_music;
  instance->add_play_list = _add_play_list;
  instance->reset = _reset;
  instance->play = _play;
  instance->play_next = _play_next;
  instance->play_pre = _play_pre;
  instance->stop = _stop;
  instance->set_play_mode = _set_play_mode;

  if (instance->construct != NULL)
  {
    instance->construct();
  }
  return instance;
}

