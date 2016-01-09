/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef _app_demux_h_
#define _app_demux_h_


#ifdef __cplusplus
extern "C" {
#endif

#define APP_DMX_FILTER_MAX_DEPTH (16)
#define APP_DMX_FILTER_NEVER_TIMEOUT (0)
typedef enum
{
  APP_DMX_EVENT_DATA_TIMEOUT,
}APP_DMX_EVENT_E;

typedef enum
{
  APP_DMX_CHANNEL_PSI,
  APP_DMX_CHANNEL_PES,
}APP_DMX_CHANNEL_TYPE;


typedef struct
{
  void *private_data;
}app_dmx_event_param;

typedef void* app_dmx_channel_handle;
typedef void* app_dmx_filter_handle;
typedef void* app_dmx_easy_filter_handle;
typedef void *app_dmx_descrambler_handle;
// if use easy filter then self = (void*)app_dmx_easy_filter_handle, otherwise self = (void*)app_dmx_filter_handle
typedef void (*app_dmx_data_notify)(void *self, u8 *p_data, u32 data_size, void *private_data);
typedef void (*app_dmx_event_notify)(void *self, APP_DMX_EVENT_E evt, app_dmx_event_param *param);


typedef struct
{
  u16 pid;
  APP_DMX_CHANNEL_TYPE channel_type;
}app_dmx_channel_param;


typedef struct
{
  BOOL enable_crc;
  BOOL one_shot;
  u32 timeout; //milliseconds
  u32 buffer_size;
  u8 match[APP_DMX_FILTER_MAX_DEPTH];
  u8 mask[APP_DMX_FILTER_MAX_DEPTH];
  //u8 nmatch[APP_DMX_FILTER_MAX_DEPTH];
  u8 depth; // match/mask valid len.
  app_dmx_data_notify data_callback;
  app_dmx_event_notify event_callback;
  void *private_data;
}app_dmx_filter_param;


typedef struct
{
  BOOL enable_crc;
  BOOL one_shot;
  u32 timeout; //milliseconds
  u32 buffer_size;
  u8 match[APP_DMX_FILTER_MAX_DEPTH];
  u8 mask[APP_DMX_FILTER_MAX_DEPTH];
  //u8 nmatch[APP_DMX_FILTER_MAX_DEPTH];
  u8 depth; // match/mask valid len.
  app_dmx_data_notify data_callback;
  app_dmx_event_notify event_callback;
  void *private_data;
  u16 pid;
}app_dmx_easy_filter_param;

typedef enum
{
  APP_DMX_CHANNEL_STOPED,
  APP_DMX_CHANNEL_STARTED
}app_dmx_channel_status;
typedef enum
{
  APP_DMX_FILTER_STOPED,
  APP_DMX_FILTER_STARTED
}app_dmx_filter_status;

typedef enum
{
  APP_DMX_DESCRAMBLER_STOPED,
  APP_DMX_DESCRAMBLER_STARTED
}app_dmx_descrambler_status;

typedef enum
{
  APP_DMX_THREAD_SUSPEND,
  APP_DMX_THREAD_RUNNING,
}app_dmx_thread_status;

typedef struct app_dmx_channel_
{
  u16 pid;
  APP_DMX_CHANNEL_TYPE channel_type;
  app_dmx_channel_status status;
  struct app_dmx_channel_ *next;
}app_dmx_channel;


typedef struct app_dmx_descrambler_
{
  u16 pid;
  dmx_chanid_t descrambler_id;
  app_dmx_descrambler_status status;
  struct app_dmx_descrambler_ *next;
}app_dmx_descrambler;


typedef struct app_dmx_filter_
{
  u16 filter_id;
  u8 *buffer;
  u32 buffer_size;
  u32 start_time; //milliseconds  ticks * 10, 1tick = 10ms
  app_dmx_filter_status status;
  app_dmx_channel *parent;
  app_dmx_filter_param param;
  struct app_dmx_filter_ *next;
}app_dmx_filter;


typedef struct
{
  u8 thread_prio;
  u8 *thread_stack;
  u32 thread_stack_size;
  app_dmx_thread_status thread_status;
}app_dmx_thread;


typedef struct
{
  app_dmx_channel *channel_list;
  app_dmx_filter *filter_list;
  app_dmx_descrambler *descrambler_list;
  app_dmx_thread thread;
  void *mutex;
}app_dmx_class;

RET_CODE app_dmx_init(u8 thread_prio);


// If you need to implement channel, use the channel and filter group interfaces, in other case use easy filters.
app_dmx_channel_handle app_dmx_alloc_channel(void);
RET_CODE app_dmx_set_channel(app_dmx_channel_handle h_channel, app_dmx_channel_param *param);
RET_CODE app_dmx_free_channel(app_dmx_channel_handle h_channel);
RET_CODE app_dmx_start_channel(app_dmx_channel_handle h_channel);
RET_CODE app_dmx_stop_channel(app_dmx_channel_handle h_channel);
app_dmx_filter_handle app_dmx_alloc_filter(app_dmx_channel_handle h_channel);
RET_CODE app_dmx_free_filter(app_dmx_filter_handle h_filter);
RET_CODE app_dmx_set_filter(app_dmx_filter_handle h_filter, app_dmx_filter_param *param);
RET_CODE app_dmx_start_filter(app_dmx_filter_handle h_filter);
RET_CODE app_dmx_stop_filter(app_dmx_filter_handle h_filter);


app_dmx_easy_filter_handle app_dmx_alloc_easy_filter(void);
RET_CODE app_dmx_free_easy_filter(app_dmx_easy_filter_handle h_filter);
RET_CODE app_dmx_set_easy_filter(app_dmx_easy_filter_handle h_filter, app_dmx_easy_filter_param *param);
RET_CODE app_dmx_start_easy_filter(app_dmx_easy_filter_handle h_filter);
RET_CODE app_dmx_stop_easy_filter(app_dmx_easy_filter_handle h_filter);


app_dmx_descrambler_handle app_dmx_alloc_descrambler(void);
RET_CODE app_dmx_free_descrambler(app_dmx_descrambler_handle h_desc);
RET_CODE app_dmx_set_descrambler_pid(app_dmx_descrambler_handle h_desc, u16 pid);
RET_CODE app_dmx_set_descrambler_evenkey(app_dmx_descrambler_handle h_desc, u8 *key, u32 len);
RET_CODE app_dmx_set_descrambler_oddkey(app_dmx_descrambler_handle h_desc, u8 *key, u32 len);
RET_CODE app_dmx_start_descrambler(app_dmx_descrambler_handle h_desc);
RET_CODE app_dmx_stop_descrambler(app_dmx_descrambler_handle h_desc);



#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_app_demux_h_
