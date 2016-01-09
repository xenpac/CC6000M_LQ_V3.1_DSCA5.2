/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _UI_MUSIC_PLAYER_SERVICE_H_
#define _UI_MUSIC_PLAYER_SERVICE_H_
#ifdef  __cplusplus
extern "C" {
#endif //__cplusplus


#define MUSIC_FILE_NAME_SIZE (1024)

typedef enum
{
  MEDIA_FILE,
  MEDIA_BUFFER,
  MEDIA_NET
}MEDIA_URI_TYPE;

typedef enum
{
  MEDIA_TRACK_TYPE_AUDIO,
  MEDIA_TRACK_TYPE_VIDEO,
  MEDIA_TRACK_TYPE_UNKNOWN
}MEDIA_TRACK_TYPE;

typedef struct
{
  MEDIA_URI_TYPE type;
  u16 str[MAX_FILE_PATH];
}MEDIA_URI;


typedef enum
{
  PLAY_MODE_LOOP_LIST,
  PLAY_MODE_LOOP_SINGLE,
  PLAY_MODE_LOOP_RANDOM,
  PLAY_MODE_SINGLE,
  PLAY_MODE_SINGLE_RANDOM
}MUSIC_PLAY_MODE;

typedef enum
{
  MUSIC_PLAY_CMD_PLAY,
  MUSIC_PLAY_CMD_PLAY_NEXT,
  MUSIC_PLAY_CMD_PLAY_PRE,
  MUSIC_PLAY_CMD_PAUSE,
  MUSIC_PLAY_CMD_STOP
}MUSIC_PLAY_CMD;

typedef enum
{
  MUSIC_SEARCH_OPT_DEFAULT,
  MUSIC_SEARCH_OPT_RECURSION,
}MUSIC_SEARCH_OPT;

typedef struct MUSIC_PLAY_LIST_
{
  MEDIA_URI uri;
  struct MUSIC_PLAY_LIST_ *next;
  struct MUSIC_PLAY_LIST_ *pre;
}MUSIC_PLAY_LIST;

typedef struct
{
  void (*init) (void);
  void (*deinit) (void);
  RET_CODE (*add_play_list) (const MEDIA_URI uri);
  void (*reset) (void);
  RET_CODE (*play) (void);
  //RET_CODE (*play_by_uri) (const MEDIA_URI *uri);
  //RET_CODE (*play_by_index) (u32 index);
  RET_CODE (*play_next) (void);
  RET_CODE (*play_pre) (void);
  RET_CODE (*stop) (void);
  void (*set_play_mode) (MUSIC_PLAY_MODE mode);
  MUSIC_PLAY_LIST *(*search_music)(const u8 *path, MUSIC_SEARCH_OPT opt);

// virturl functions:
  void (*play_end_cb)(void);
  void (*play_error_cb)(void);
  //void (*on_init) (void);
  //void (*on_deinit) (void);

//Private:
  void (*construct) (void);
  RET_CODE (*do_command) (MUSIC_PLAY_CMD cmd, void *data, u32 size);
  void (*thread_entry) (void * param);
  
  void *mutex;
  u32 message_queue;
  u32 thread_id;
  u32 *thread_stack;
  MUSIC_PLAY_MODE play_mode;
  MUSIC_PLAY_LIST *play_list;
  MUSIC_PLAY_LIST *play_list_cur;
  u32 play_list_item_count;
  MUSIC_PLAY_LIST *search_list;

  u32 music_api_handle;
  
  //MIEDIA_PLAYER_SERVICE *media_player_service;
}MUSIC_PLAYER_SERVCIE;

const MUSIC_PLAYER_SERVCIE *get_music_player_service_instance(void);


#ifdef  __cplusplus
}
#endif //__cplusplus
#endif//_UI_MUSIC_PLAYER_SERVICE_H_

