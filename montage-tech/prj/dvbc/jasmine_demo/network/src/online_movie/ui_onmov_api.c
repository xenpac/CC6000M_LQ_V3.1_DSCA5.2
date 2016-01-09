/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_config.h"
#include "ui_onmov_api.h"
#include "netplay.h"

#define APP_ONMOV APP_RESERVED2

#define N_WEB_PER_SERV     (8)
#define N_CATE_PER_WEB     (1)
#define N_PAGE_PER_CATE    (300)
#define N_MEDIA_PER_PAGE  (10)
#define NSERV     (1)
#define NWEB      (N_WEB_PER_SERV*NSERV)  // 3
#define NCATE     (N_CATE_PER_WEB*NWEB)  // 15
#define NMEDIA   (N_MEDIA_PER_PAGE*N_PAGE_PER_CATE*NCATE) // 3000

#define GET_PAGE_OFFSET(id) ((id>>24)*N_CATE_PER_WEB*N_PAGE_PER_CATE\
	                                          +((id>>16)&0xff)*N_PAGE_PER_CATE\
	                                          +(id&0xffff))
#define GET_MEDIA_OFFSET(id) (N_MEDIA_PER_PAGE*GET_PAGE_OFFSET(id))

extern u32 g_video_file_play_addr;                       //fileplay addr   and  pvr play addr

typedef enum
{
  ONMOV_API_EVT_SERVER_INFO = ((APP_ONMOV << 16) + 0),
  ONMOV_API_EVT_MEDIA_INFO,
} onmov_api_evt_t;

typedef struct
{
  netplay_server_info_t         serv_info[NSERV];
  netplay_website_info_t       web_info[NWEB];
  netplay_category_info_t      cate_info[NCATE];
  netplay_media_info_t          meida_info[NMEDIA];
} MOV_MEM_POOL;

typedef struct
{
  MOV_MEM_POOL* p_mem_pool;
  u32 mem_pool_size;
  char* str_pool;
  u32 str_pool_size;
  u32 str_pool_pos; //free space offset in string pool
}MOV_MEM;

static MOV_MEM sg_mov_mem = {0};
static netplay_media_request_t sg_request = {0};
static netplay_media_search_request_t sg_search_request = {0};
static u8 sg_page_req_flag_arr[NWEB][N_CATE_PER_WEB][N_PAGE_PER_CATE] = {{{0}}};
static netplay_media_info_t sg_searched_media[N_MEDIA_PER_PAGE] = {{{0}}};
//static u8* sg_p_download_cache = NULL;
static BOOL sg_om_init_flag = FALSE;

u16 ui_onmov_evtmap(u32 event);

static char* copy_to_string_pool(char* p_str)
{
  char* p = NULL;
  int len = 0;

  if( NULL == p_str )
    return NULL;

  len = strlen(p_str);  
  if( sg_mov_mem.str_pool_pos + len + 1 > sg_mov_mem.str_pool_size )  
    return NULL;

  p = sg_mov_mem.str_pool + sg_mov_mem.str_pool_pos;
  strcpy(p, p_str);
  
  sg_mov_mem.str_pool_pos += len+1; // update current free position, +1 for '\0'

  return p;
}

static int  onmov_event_callback(void *msg, void *param)
{
  netplay_notify_event_t *event = (netplay_notify_event_t *)msg;
  event_t evt = {0};

  if( FALSE == sg_om_init_flag )
    return 0;

  switch(event->event )
  {
    case NETPLAY_MSG_GOT_SERVER_INFO:
      do
      {
        int i, m;
        int web_cnt;
        netplay_website_info_t* p_web_arr = NULL;
        netplay_server_info_t* p_serv_info;

        p_serv_info = (netplay_server_info_t*)param;
        p_web_arr = sg_mov_mem.p_mem_pool->web_info;
        sg_mov_mem.p_mem_pool->serv_info[0].website_count= p_serv_info->website_count;
        sg_mov_mem.p_mem_pool->serv_info[0].p_web = p_web_arr;
        web_cnt = p_serv_info->website_count;

        for( i = 0; i < NWEB && i < web_cnt; i++ )
        {
          int cate_cnt;
	   netplay_category_info_t* p_cate_arr;
	   p_web_arr[i] = *(p_serv_info->p_web + i);
          p_web_arr[i].p_logo_url = copy_to_string_pool(p_web_arr[i].p_logo_url);
	   cate_cnt = p_web_arr[i].cate_count;
	   p_cate_arr = sg_mov_mem.p_mem_pool->cate_info + i*N_CATE_PER_WEB;
	   for( m = 0; m < N_CATE_PER_WEB && m < cate_cnt; m++ )
	     p_cate_arr[m] = *(p_web_arr[i].p_cate + m);
        }
      }while(0);

      evt.id = ONMOV_API_EVT_SERVER_INFO;
      evt.data1 = (u32)sg_mov_mem.p_mem_pool->serv_info;
      ap_frm_send_evt_to_ui(APP_ONMOV, &evt);
    break;
     
   case NETPLAY_MSG_GOT_MEDIA_INFO:
      do
      {
        u8  web_idx;
        u8  cate_idx;
        u16 page_idx;
        netplay_media_info_t* p_media_arr;
        netplay_media_info_t* p_media;
        netplay_media_msg_info_t* p_media_msg;
	 int media_offset;
	 int media_cnt;
	 int k;

        if(event->identify != sg_request.identify)
        {
          OS_PRINTF("identify not match: req %x event %x\n", sg_request.identify, event->identify);
          return 0;
        }
        
        web_idx = event->identify >> 24;
        cate_idx = (event->identify >> 16) & 0xff;
        page_idx = event->identify & 0xffff;
        
        media_offset = GET_MEDIA_OFFSET(event->identify);
        p_media_arr = sg_mov_mem.p_mem_pool->meida_info + media_offset;
        p_media_msg = (netplay_media_msg_info_t*)param;
        p_media = p_media_msg->p_info;
        media_cnt = p_media_msg->media_count;

        for(k = 0; k < media_cnt; k++)
        {
          strcpy(p_media_arr[k].name, (p_media+k)->name);
          p_media_arr[k].p_actor = copy_to_string_pool(p_media[k].p_actor);
          p_media_arr[k].p_description = copy_to_string_pool(p_media[k].p_description);
          p_media_arr[k].p_director = copy_to_string_pool(p_media[k].p_director);
          p_media_arr[k].p_logo_url = copy_to_string_pool(p_media[k].p_logo_url);
          p_media_arr[k].p_url = copy_to_string_pool(p_media[k].p_url);
        }
OS_PRINTF("Media count: %d\n", media_cnt);
OS_PRINTF("+++++++++++++++++++++++++++++++++++++++++++++\n");
OS_PRINTF("+++++++++++++++++++++++++++++++++++++++++++++\n");
		
        evt.id = ONMOV_API_EVT_MEDIA_INFO;
        evt.data1 = (u32)p_media_arr;
        evt.data2 = page_idx<<16 | media_cnt;
        ap_frm_send_evt_to_ui(APP_ONMOV, &evt);

        if( N_MEDIA_PER_PAGE == k )//set page requested flag, when media count reaches 10
          sg_page_req_flag_arr[web_idx][cate_idx][page_idx] = 1;
      }while(0);
    break;

    case NETPLAY_MSG_GOT_MEDIA_SEARCH_RESULT:
      do
      {
        netplay_media_msg_info_t* p_media_msg = NULL;
        u8 i = 0;
        if(event->identify != sg_search_request.identify)
        {
          OS_PRINTF("identify not match: req %x event %x\n", sg_search_request.identify, event->identify);
          return 0;
        }

        p_media_msg = (netplay_media_msg_info_t*)param;
OS_PRINTF("Media count: %d\n", p_media_msg->media_count);
OS_PRINTF("+++++++++++++++++++++++++++++++++++++++++++++\n");
        for(i = 0; i < p_media_msg->media_count; i++)
        {
          strcpy(sg_searched_media[i].name, p_media_msg->p_info[i].name);
          sg_searched_media[i].p_logo_url = copy_to_string_pool(p_media_msg->p_info[i].p_logo_url);
          sg_searched_media[i].p_url = copy_to_string_pool(p_media_msg->p_info[i].p_url);
          sg_searched_media[i].p_actor = copy_to_string_pool(p_media_msg->p_info[i].p_actor);
          sg_searched_media[i].p_director = copy_to_string_pool(p_media_msg->p_info[i].p_director);
          sg_searched_media[i].p_description = copy_to_string_pool(p_media_msg->p_info[i].p_description);

OS_PRINTF("a%d, name: %s\n", i, sg_searched_media[i].name);
OS_PRINTF("a%d, p_logo_url: %s\n", i, sg_searched_media[i].p_logo_url);
OS_PRINTF("a%d, p_url: %s\n", i, sg_searched_media[i].p_url);
OS_PRINTF("a%d, p_actor: %s\n", i, sg_searched_media[i].p_actor);
OS_PRINTF("a%d, p_director: %s\n", i, sg_searched_media[i].p_director);
OS_PRINTF("a%d, p_description: %s\n", i, sg_searched_media[i].p_description);
        }

OS_PRINTF("+++++++++++++++++++++++++++++++++++++++++++++\n");

        evt.id = ONMOV_API_EVT_MEDIA_INFO;
        evt.data1 = (u32)sg_searched_media;
        evt.data2 = 0<<16 | p_media_msg->media_count;
        ap_frm_send_evt_to_ui(APP_ONMOV, &evt);
		
      }while(0);
    break;
	
    default:
    break;
  }

  return 0;
}


static void mov_mem_pool_init(void)
{
  u32 total_mem_size;

  sg_mov_mem.p_mem_pool = (MOV_MEM_POOL*)g_video_file_play_addr;
  total_mem_size = VIDEOC_ONMOV_SPLIT_MEM;
  
  memset(sg_mov_mem.p_mem_pool, 0, sizeof(MOV_MEM_POOL));
  sg_mov_mem.mem_pool_size = sizeof(MOV_MEM_POOL);
OS_PRINTF("sizeof(MOV_MEM_POOL) = 0x%x\n", sizeof(MOV_MEM_POOL));
  sg_mov_mem.str_pool 
  	= (char*)sg_mov_mem.p_mem_pool + sizeof(MOV_MEM_POOL);
  sg_mov_mem.str_pool_size = total_mem_size - sizeof(MOV_MEM_POOL);
  sg_mov_mem.str_pool_pos = 0;
}

//extern void pic_set_cache_buf(u8* p_buf, u32 len);
void ui_onmov_init(void)
{   
  netplay_config_t np_cfg;

  mov_mem_pool_init();
/*
  if( NULL == sg_p_download_cache )
  {
    sg_p_download_cache = mtos_malloc(1*MBYTES);
    if( sg_p_download_cache )
      pic_set_cache_buf(sg_p_download_cache, 1*MBYTES);
  }
  */
  np_cfg.page_size = 10;
  np_cfg.stack_size = 32*KBYTES;
#ifndef WIN32
  np_cfg.task_prio = ONMOV_PRIORITY;
  np_cfg.notify = onmov_event_callback;
  netplay_init(&np_cfg);
#endif   
  fw_register_ap_evtmap(APP_ONMOV, ui_onmov_evtmap);
  fw_register_ap_msghost(APP_ONMOV, ROOT_ID_ONMOV_WEBSITES); 
  fw_register_ap_msghost(APP_ONMOV, ROOT_ID_NETWORK_PLAY); 

  sg_om_init_flag = TRUE;
}

void ui_onmov_deinit(void)
{
  sg_om_init_flag = FALSE;

  memset((void*)&sg_mov_mem, 0, sizeof(MOV_MEM));
  memset(sg_page_req_flag_arr, 0, sizeof(sg_page_req_flag_arr));

  fw_unregister_ap_evtmap(APP_ONMOV);
  fw_unregister_ap_msghost(APP_ONMOV, ROOT_ID_ONMOV_WEBSITES); 
  fw_unregister_ap_msghost(APP_ONMOV, ROOT_ID_NETWORK_PLAY); 
}

static u8 req_flag_check(int id)
{
  u8  web_idx = id >> 24;
  u8  cate_idx = (id >> 16) & 0xff;
  u16 page_idx = id & 0xffff;

  return sg_page_req_flag_arr[web_idx][cate_idx][page_idx];
}

void ui_onmov_media_request(const char* web_name, const char* cate_name, 
	int page_num, int id)
{
  event_t evt = {0};

  if( page_num > N_PAGE_PER_CATE-1)
    return;//exceed max per category
  
  if(1 == req_flag_check(id))
  {
    netplay_media_info_t* p_media = NULL;
    p_media = sg_mov_mem.p_mem_pool->meida_info + GET_MEDIA_OFFSET(id);
    evt.id = ONMOV_API_EVT_MEDIA_INFO;
    evt.data1 = (u32)p_media;
    evt.data2 = (id&0xffff)<<16 | N_MEDIA_PER_PAGE;
    ap_frm_send_evt_to_ui(APP_ONMOV, &evt);

    return;
  }

  sg_request.identify = id;
  sg_request.page_num = page_num;
  strcpy(sg_request.website_name, web_name);
  strcpy(sg_request.category_name, cate_name);

#ifndef WIN32
  netplay_request_media_info(&sg_request);
#endif
}

void ui_onmov_media_search(const char* p_web, 
	const char* p_cate, const char* search_pattern, u32 search_id)
{
#ifndef WIN32
  sg_search_request.identify = search_id;
  strcpy(sg_search_request.website_name, p_web);
  strcpy(sg_search_request.category_name, p_cate);
  strcpy(sg_search_request.media_name,search_pattern);
  netplay_request_media_search(&sg_search_request);
#endif
}

BEGIN_AP_EVTMAP(ui_onmov_evtmap)
  CONVERT_EVENT(ONMOV_API_EVT_SERVER_INFO, MSG_ONMOV_EVT_SERVER)
  CONVERT_EVENT(ONMOV_API_EVT_MEDIA_INFO,   MSG_ONMOV_EVT_MEDIA)
END_AP_EVTMAP(ui_onmov_evtmap)
