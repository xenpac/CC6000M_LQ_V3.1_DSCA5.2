/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_config.h"
#include "commonData.h"
#include "youtubeDataProvider.h"
#include "ui_youporn_api.h"

#define YP_N_VIDEO_PER_PAGE  (10)
#define YP_N_CATE_PER_WEB (1)
#define YP_NWEB      (1)
#define YP_NCATE     (YP_N_CATE_PER_WEB*YP_NWEB)
//#define YP_NMEDIA   (YP_N_VIDEO_PER_PAGE*YP_MAX_VIDEO_PER_CATE*YP_NCATE)
#define YP_NMEDIA   (YP_MAX_VIDEO_PER_CATE*YP_NCATE)


#define APP_YOUPORN APP_RESERVED1

#define YOUPORN_MEM_SIZE (2 * MBYTES)
enum
{
  YOUPORN_EVT_VIDEO_INFO = ((APP_YOUPORN << 16) + 0),
  YOUPORN_EVT_WEB_INFO,
  YOUPORN_EVT_PLAY_URL,
};
/*
typedef struct
{
  u32 video_cnt;
  youx_item video_arr[YP_MAX_VIDEO_PER_CATE];
}youporn_cate_t;
*/
typedef struct
{
  youporn_website_t yp_web_arr[YP_NWEB];
  youporn_cate_t yp_cate_arr[YP_NCATE];
}youporn_movie_t;

typedef struct
{
  youporn_movie_t* p_yp_movies;
  u32 yp_movies_size;
  char* str_pool;
  u32 str_pool_size;
  u32 str_pool_pos; //free space offset in string pool
}youporn_mem_t;

static INIT_TASK_PARAM_T g_task_param;

static youporn_mem_t sg_youporn_mem = {0};
static YOUTUBE_DATA_T *sg_youporn_handle = NULL;

static BOOL sg_yp_init_flag = FALSE;

u16 ui_youporn_evtmap(u32 event);


static char* copy_to_string_pool(char* p_str)
{
  char* p = NULL;
  int len = 0;

  if( NULL == p_str )
    return NULL;

  len = strlen(p_str);  
  if( sg_youporn_mem.str_pool_pos + len + 1 > sg_youporn_mem.str_pool_size )  
    return NULL;

  p = sg_youporn_mem.str_pool + sg_youporn_mem.str_pool_pos;
  strcpy(p, p_str);
  
  sg_youporn_mem.str_pool_pos += len+1; // update current free position, +1 for '\0'

  return p;
}

void request_play_video_url(u16 video_idx)
{
  if( NULL == sg_youporn_handle )
    return;
  sg_youporn_handle->set_focus_item_index(sg_youporn_handle, video_idx);
  sg_youporn_handle->get_play_url(sg_youporn_handle);
}

static RET_CODE  yourporn_event_callback(YOUTUBE_DP_EVENT_TYPE event, u32 param)
{
  YT_DP_EVENT_PARAM_T *p_param = NULL;
  event_t evt = {0};

  if( FALSE == sg_yp_init_flag )
    return 0;

  OS_PRINTF("^^^^^^^^^^^^^^^^^^^^^^^event = %d\n", event);
 
  switch(event)
  {
    case YT_DP_EVE_INIT_OK:
      do
      {
        u16 i, k;
        u16 video_cnt;
        youx_data_t* p_yp_data = NULL;
        youx_item* p_cate_video = NULL;
        youx_item* p_yp_items = NULL;
        youporn_website_t* p_yp_web = NULL;
        youporn_cate_t* p_yp_cate = NULL;

        p_param = (YT_DP_EVENT_PARAM_T *)param;
        p_yp_data = p_param->p_youx_data;
        p_yp_items = p_yp_data->p_youx_item_list;
        video_cnt = p_yp_data->total_item;

        k = 0; //reserved for category, only one category now

        //web info
        p_yp_web = sg_youporn_mem.p_yp_movies->yp_web_arr;
        p_yp_web->cate_count = 1;
        p_yp_web->p_logo_url = copy_to_string_pool(p_yp_data->youx_website_icon);
        strcpy(p_yp_web->name, "YouPorn");
        p_yp_web->p_cate = sg_youporn_mem.p_yp_movies->yp_cate_arr;

        //category info
        p_yp_cate = p_yp_web->p_cate + k;
        strcpy(p_yp_cate->name, "Video");
        p_yp_cate->video_cnt = video_cnt;
        //p_yp_cate->video_arr = {0};

        //video info
        p_cate_video = p_yp_cate->video_arr;
        sg_youporn_mem.p_yp_movies->yp_cate_arr[k].video_cnt = video_cnt;
OS_PRINTF("*********************************, video_cnt = %d\n", video_cnt);
        for( i = 0; i < video_cnt; i++ )
        {
          p_cate_video[i].title = copy_to_string_pool(p_yp_items[i].title);
          p_cate_video[i].link = copy_to_string_pool(p_yp_items[i].link);
          p_cate_video[i].image = copy_to_string_pool(p_yp_items[i].image);
          p_cate_video[i].discription = copy_to_string_pool(p_yp_items[i].discription);
          p_cate_video[i].length = copy_to_string_pool(p_yp_items[i].length);
          p_cate_video[i].pubdata = copy_to_string_pool(p_yp_items[i].pubdata);
          p_cate_video[i].play_video_url = NULL; //get play url by "REQUEST_PLAY_VIDEO_URL" ??
          //request_play_video_url(i);
//OS_PRINTF("request %d\n", i);

/*
OS_PRINTF("%03d, title:%s\n", i, p_cate_video[i].title);
OS_PRINTF("%03d, link:%s\n", i, p_cate_video[i].link);
OS_PRINTF("%03d, image:%s\n", i, p_cate_video[i].image);
OS_PRINTF("%03d, discription:%s\n", i, p_cate_video[i].discription);
OS_PRINTF("%03d, length:%s\n", i, p_cate_video[i].length);
OS_PRINTF("%03d, pubdata:%s\n", i, p_cate_video[i].pubdata);
*/
        }
OS_PRINTF("*********************************\n");


        evt.id = YOUPORN_EVT_WEB_INFO;
        evt.data1 = (u32)p_yp_web;
        evt.data2 = 0;
        ap_frm_send_evt_to_ui(APP_YOUPORN, &evt);


        evt.id = YOUPORN_EVT_VIDEO_INFO;
        evt.data1 = (u32)p_cate_video;
        evt.data2 = video_cnt>YP_N_VIDEO_PER_PAGE?YP_N_VIDEO_PER_PAGE:video_cnt;
        ap_frm_send_evt_to_ui(APP_YOUPORN, &evt);

      }while(0);
    break;

    case YT_DP_EVE_NEW_PLAY_URL_ARRIVE:
      do
      {
        u8 video_idx = 0;
	 //u8 web_idx = 0; //youporn only
	 u8 cate_idx = 0; //only one category currently
	 u8 page_idx = 0;
        youx_item* p_cate_video = NULL;
        youx_data_t* p_yp_data = NULL;
        p_param = (YT_DP_EVENT_PARAM_T *)param;

        p_yp_data = p_param->p_youx_data;
        video_idx = p_param->identify_code & 0xff;
        OS_PRINTF("YT_DP_EVE_NEW_PLAY_URL_ARRIVE, video_idx = %d\n", video_idx);
        page_idx = video_idx / YP_N_VIDEO_PER_PAGE;

        p_cate_video = sg_youporn_mem.p_yp_movies->yp_cate_arr[cate_idx].video_arr;
        if( NULL == p_cate_video[video_idx].play_video_url )
        {
          p_cate_video[video_idx].play_video_url = 
		  	copy_to_string_pool(p_yp_data->p_youx_item_list[video_idx].play_video_url);
          //OS_PRINTF("play_video_url[%d]: %s\n", video_idx,p_cate_video[video_idx].play_video_url);
        }
          evt.id = YOUPORN_EVT_PLAY_URL;
          evt.data1 = (u32)p_cate_video[video_idx].play_video_url;
          evt.data2 = 0;
          ap_frm_send_evt_to_ui(APP_YOUPORN, &evt);
      }while(0);
    break;

    default:
    break;
  }

  return SUCCESS;
}

static void youporn_mem_init()
{
  u32 total_mem_size;

  sg_youporn_mem.p_yp_movies
  	= (youporn_movie_t*)mtos_malloc(YOUPORN_MEM_SIZE);
  MT_ASSERT(NULL != sg_youporn_mem.p_yp_movies);
  total_mem_size = YOUPORN_MEM_SIZE;
  OS_PRINTF("sizeof(youporn_movie_t) = 0x%x\n", sizeof(youporn_movie_t));
  memset(sg_youporn_mem.p_yp_movies, 0, sizeof(youporn_movie_t));
  sg_youporn_mem.yp_movies_size = sizeof(youporn_movie_t);

  sg_youporn_mem.str_pool 
  	= (char*)sg_youporn_mem.p_yp_movies + sizeof(youporn_movie_t);
  sg_youporn_mem.str_pool_size = total_mem_size - sizeof(youporn_movie_t);
  sg_youporn_mem.str_pool_pos = 0;
}

void ui_youporn_init(void)
{
  

  youporn_mem_init();

  memset(&g_task_param, 0, sizeof(g_task_param));
#ifndef WIN32
  g_task_param.priority = YT_DP_PRIORITY;
  g_task_param.http_priority = YT_HTTP_PRIORITY;
  g_task_param.p_stack_mem = (char *)mtos_malloc(512*KBYTES);
  MT_ASSERT(g_task_param.p_stack_mem != NULL);
  memset(g_task_param.p_stack_mem,0,512*KBYTES);
  g_task_param.stack_size = 512*KBYTES;
  sg_youporn_handle =  register_youtube_dataprovider((void *)&g_task_param);
  sg_youporn_handle->register_event_callback(sg_youporn_handle, yourporn_event_callback);
  sg_youporn_handle->init(sg_youporn_handle, YOUX_FEED_TYPE);
#endif
  fw_register_ap_evtmap(APP_YOUPORN, ui_youporn_evtmap);
  fw_register_ap_msghost(APP_YOUPORN, ROOT_ID_YOUPORN);  
  fw_register_ap_msghost(APP_YOUPORN, ROOT_ID_ONMOV_DESCRIPTION);  

  sg_yp_init_flag = TRUE;
}

void ui_youporn_deinit(void)
{
  sg_yp_init_flag = FALSE;
  
#ifndef WIN32
  if (sg_youporn_handle != NULL)
  {
    unregister_youtube_dataprovider((void  *)sg_youporn_handle);
  }
#endif
  fw_unregister_ap_evtmap(APP_YOUPORN);
  fw_unregister_ap_msghost(APP_YOUPORN, ROOT_ID_YOUPORN); 
  fw_unregister_ap_msghost(APP_YOUPORN, ROOT_ID_ONMOV_DESCRIPTION); 

  if(g_task_param.p_stack_mem)
  {
      mtos_free(g_task_param.p_stack_mem);
      g_task_param.p_stack_mem = NULL;
  }

  if (sg_youporn_mem.p_yp_movies != NULL)
  {
    OS_PRINTF("yoporn free use mem\n\n");
    mtos_free(sg_youporn_mem.p_yp_movies);
    sg_youporn_mem.p_yp_movies = NULL;
  }
}

void ui_youporn_video_request(const char* web_name, const char* cate_name, 
	int page_num, int id)
{
  u8 web_idx = 0;
  u8 cate_idx = 0;
  u8 page_idx = 0;
  u32 num_video = 0;
  u8 num_pages = 0;
  u8 num_video_last_page = 0;
  youporn_cate_t* p_yp_cate = NULL;
  youx_item* p_video_page = NULL;
  event_t evt = {0};

  if(NULL == sg_youporn_handle)
    return;
  web_idx = id>>24; //always 0 here, youporn only ^^
  cate_idx = id>>16 & 0xff; //should be 0 now, only one category currently
  page_idx = id & 0xffff;
  num_video = sg_youporn_mem.p_yp_movies->yp_cate_arr[cate_idx].video_cnt;
  num_video_last_page = num_video % YP_N_VIDEO_PER_PAGE;
  num_pages = (num_video / YP_N_VIDEO_PER_PAGE) + 1;
  MT_ASSERT(page_idx < num_pages);

  OS_PRINTF("%s, num_video_last_page = %d, num_pages = %d\n",
  	__FUNCTION__, num_video_last_page, num_pages);
  OS_PRINTF("%s, (%d,%d,%d)", __FUNCTION__, web_idx, cate_idx, page_idx);
  
  p_yp_cate = sg_youporn_mem.p_yp_movies->yp_cate_arr + cate_idx;

  //p_video_page = sg_mov_mem.p_mem_pool->meida_info + GET_MEDIA_OFFSET(id);
  p_video_page = p_yp_cate->video_arr + page_idx * YP_N_VIDEO_PER_PAGE;
  evt.id = YOUPORN_EVT_VIDEO_INFO;
  evt.data1 = (u32)p_video_page;
  evt.data2 = (page_idx<<16)
  	           | (page_idx < num_pages-1 ? YP_N_VIDEO_PER_PAGE : num_video_last_page);
  ap_frm_send_evt_to_ui(APP_YOUPORN, &evt);

}

youporn_website_t* get_yp_web_info(void)
{
  static youporn_website_t web_info;

  if(NULL == sg_youporn_handle)
    return NULL;
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);

  web_info.cate_count = 1;
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
  strcpy(web_info.name, "youporn");
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
  web_info.p_logo_url = NULL;
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
  web_info.p_cate = &(sg_youporn_mem.p_yp_movies->yp_cate_arr[0]);
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
  strcpy(web_info.p_cate->name, "Movie");
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
  return &web_info;
}


BEGIN_AP_EVTMAP(ui_youporn_evtmap)
  CONVERT_EVENT(YOUPORN_EVT_VIDEO_INFO, MSG_YP_VIDEO_INFO)
  CONVERT_EVENT(YOUPORN_EVT_WEB_INFO, MSG_YP_WEB_INFO)
  CONVERT_EVENT(YOUPORN_EVT_PLAY_URL, MSG_YP_PLAY_URL)
END_AP_EVTMAP(ui_youporn_evtmap)



