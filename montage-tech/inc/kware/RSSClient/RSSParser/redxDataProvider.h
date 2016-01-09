/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __YOUX_DATA_PROVIDER__
#define  __YOUX_DATA_PROVIDER__
#ifdef __cplusplus
extern "C" {
#endif
#define REDTUBE_ID_LEN  32
#define REDTUBE_URL_LEN  0x200
#define REDTUBE_LIST_LEN 100
#define REDTUBE_CATE_LEN 16
#define REDTUBE_RATING_LEN 8
#define REDTUBE_VIEWTIME_LEN 16
#define REDTUBE_DURATION_LEN 8
#define REDTUBE_ADDED_TIME_LEN 16
#define REDTUBE_IMAGE_LEN 128
#define REDTUBE_LINK_LEN 32
#define REDTUBE_TITLE_LEN 128
typedef enum
{
          
    REDTUBE_VIDEO_DEF,
    REDTUBE_VIDEO_CATE,
    REDTUBE_VIDEO_SEARCH,
    
}REDTUBE_VIDEO_TYPE;
	typedef struct{
		char title[REDTUBE_TITLE_LEN];
		char link[REDTUBE_LINK_LEN];
		char image[REDTUBE_IMAGE_LEN];
		
		char added_time[REDTUBE_ADDED_TIME_LEN];
		char length[REDTUBE_DURATION_LEN];
		char view_times[REDTUBE_VIEWTIME_LEN];
		char rating[REDTUBE_RATING_LEN];  
		char play_video_url[REDTUBE_URL_LEN];

	}redtube_item;


	typedef  struct{

		char * redtube_website_icon;
        int total_group;
		int total_item;
		redtube_item * p_redtube_item_list;

	} redtube_data_t;
typedef struct{
    int num;
    int cate_index;
    int grp_index;
    int grp_size;
    REDTUBE_VIDEO_TYPE video_type;
    int grp_total;
    char * website_icon;
    redtube_item *item_list;
    

}REDTUBE_VIDEO_LIST_T;
typedef struct{
    char  video_id[REDTUBE_ID_LEN];
    char  video_url[REDTUBE_URL_LEN];


}REDTUBE_VIDEO_URL_T;
typedef struct{
    int  cate_total;
    char cate_name[MAX_CATEGORY_NUM][CATEGORY_NAME_LEN];


}REDTUBE_CATE_LIST_T;



typedef enum
{
    REDTUBE_EVENT_INVAILID,      
    REDTUBE_VIDEO_LIST_SUCCESS,
    REDTUBE_VIDEO_LIST_FAIL,
    REDTUBE_URL_GET_SUCCESS,
    REDTUBE_URL_GET_FAIL,
    REDTUBE_CATE_GET_SUCCESS,
    REDTUBE_CATE_GET_FAIL,
}REDTUBE_EVENT_TYPE;


	typedef enum{
	
		REDTUBE_REQUEST_PLAY_VIDEO_URL = 0,
	

		REDTUBE_REQUEST_GET_CATEGORY_TOTAL ,
	
		REDTUBE_REQUEST_VIDEO_LIST,

	} REDTUBE_DP_USER_CMD_TYPE;
typedef RET_CODE  (*redtube_event_callback)(REDTUBE_EVENT_TYPE event, u32 param);
 typedef  int (*  redtube_get_video_play_url_fun_t)(void *,int);
typedef int (* redtube_search_fun_t)(void *, char *, int);
typedef int (* redtube_search_page_fun_t)(void *, int);
typedef int (* redtube_get_def_fun_t)(void *,  int);
typedef int (* redtube_set_grp_size_fun_t)(void *,  int);
typedef int (* redtube_get_category_video_fun_t)(void *, int,int);

typedef struct{

	
	void  * 	p_bg_task_hdl;
    init_fun_t     init;
    redtube_get_video_play_url_fun_t                 get_play_url;
    redtube_search_fun_t  search;
    redtube_search_page_fun_t  search_page;    
    redtube_get_def_fun_t  get_default_video;
    redtube_set_grp_size_fun_t set_grp_page_size;
    get_category_num_fun_t                   get_category_num;
    redtube_get_category_video_fun_t                get_category_video;
    BOOL is_init;
    int  cur_focus_item_index ;  
    char cur_key_word[MAX_KEYWORD_LEN];
    int  catetory_total;
        int  item_total;
    int  group_total;
    int  group_size;
    int  group_index;
    int  page_total;
    int  page_size;
    int  page_index;
    int cur_feed_type;
    char      catetory_name_arry[MAX_CATEGORY_NUM][CATEGORY_NAME_LEN];
    int cur_focus_catetory_index;
    redtube_data_t  * p_redtube;//^^ for youporn
    register_event_cb_func_t    register_event_callback;
    redtube_event_callback event_callback;
}REDTUBE_DP_HDL_T;
int  init_redtube_feed(void * hdl, YT_FEED_TYPE feed_type, YT_REGION_ID_T regID);
int  unregister_redtube_dataprovider(REDTUBE_DP_HDL_T * hdl);
int parse_redtube_toprss(void * hdl, int grp_index);
void   destroy_redtube_data(void * hdl);
void  parse_redtube_vids_html(void * hdl);
 REDTUBE_DP_HDL_T *  register_redtube_dataprovider(DO_CMD_TASK_CONFIG_T * p_config);
#ifdef __cplusplus
}
#endif

#endif




