#ifndef  __YOUTUBE_DATA_PROVIDER__
#define  __YOUTUBE_DATA_PROVIDER__
#ifdef __cplusplus
extern "C" {
#endif

#include "download_api.h"

#define   ITEM_KEYWORD_AUTHOR                        "author"
#define   ITEM_KEYWORD_DESCRIPTION                "description"
#define   ITEM_KEYWORD_TITLE                             "title"
#define   ITEM_KEYWORD_VIDEOID                      "videoid"
#define   ITEM_KEYWORD_THUMNAIL_URLDEF              "thumnail_urldef"
#define   ITEM_KEYWORD_THUMNAIL_URLSD              "thumnail_urlsd"
#define   ITEM_KEYWORD_THUMNAIL_URLHQ              "thumnail_urlhq"
#define   ITEM_KEYWORD_THUMNAIL_URLMQ              "thumnail_urlmq"
#define   ITEM_KEYWORD_THUMNAIL_URL1              "thumnail_url1"
#define   ITEM_KEYWORD_THUMNAIL_URL2              "thumnail_url2"
#define   ITEM_KEYWORD_THUMNAIL_URL3              "thumnail_url3"
#define   ITEM_KEYWORD_DURATION                      "duration"
#define   ITEM_KEYWORD_RATING                           "rating"
#define   ITEM_KEYWORD_VIEWCOUNT                          "viewcount"

	//#define  TMP_BUF_LEN                              (128)
//#define  TMP_BIG_BUF_LEN                       (4096)
#define   PLAY_URL_MAX_LEN                   (2048)

#define   MAX_FILM_LIST_LEN                    (32)

#define     YT_CMD_FIFO_LEN                          (17)

	typedef enum{

		INIT_DATA_PROVIDER = 0,
		DEINIT_DATA_PROVIDER = 1,
		REQUEST_NEW_PAGE_ITEM = 2,
		REQUEST_NEW_CATEGORY = 3,
		REQUEST_PLAY_VIDEO_URL = 4,
		REQUEST_SET_MAX_ITEM_NUM = 5,
		REQUEST_SET_FOCUS_ITEM_INDEX = 6,
		REQUEST_GET_FOCUS_ITEM_INDEX = 7,
		REQUEST_SET_CATEGORY_INDEX = 8,
		REQUEST_GET_CATEGORY_INDEX = 9,
		REQUEST_GET_PAGE_INDEX = 10,
		REQUEST_SET_PAGE_IDNEX = 11,
		REQUEST_GET_CATEGORY_TOTAL = 12,
		REQUEST_GET_NEXT_PAGE = 13,
		REQUEST_GET_PREV_PAGE = 14,
		REQUEST_GET_SEARCH = 15,
		REQUEST_SET_REGION_ID = 16,
		REQUEST_SET_TIME =17,

	} YOUTUBE_DP_USER_CMD_TYPE;

	typedef enum{
		NO_ERR = 0,
		INIT_FAIL = 1,
		ITEM_NUM_ZERO = 2,
		URL_NULL = 3,
		GET_PAGE_FAIL = 4,
		SEARCH_FAIL = 5,


	} YOUTUBE_DP_FAIL_TYPE;

#if 0
	typedef   struct {

		int  category_num;  /*total category*/
		int  item_num_total;  /*total item*/
		char * p_category_name[MAX_CATEGORY_NUM]; /*reserve name of category*/

		int        item_num;                             /*total item of current page*/
		char  *  p_item_video_id[MAX_ITEM_NUM_OF_RSS];
		char  *  p_item_description[MAX_ITEM_NUM_OF_RSS];
		char  *  p_item_author[MAX_ITEM_NUM_OF_RSS];  /*author of each item in one page*/
		char  *  p_item_title[MAX_ITEM_NUM_OF_RSS];  /*title of each item in one page*/
		char  *  p_rating[MAX_ITEM_NUM_OF_RSS];       /*rating of each item in one page*/
		char  *  p_duration[MAX_ITEM_NUM_OF_RSS];   /*duration of each item in one page*/
		char  *  p_thumnaildef[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnailmq[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnailsd[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnailhq[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnail1[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnail2[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnail3[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/

		unsigned int  fail_no;
		unsigned int  identify_code;               /*identify the source of the event*/
		/*bit31 bit30,... bit24  identify the feed type

		  from bit23~bit16:  category index
		  from bit15~bit8:  page number
		  from bit7~bit0:   item number

		 */

	} YT_DP_INIT_OK_EVENT_PARAM_T;
#endif

	typedef struct{
		char * title;
		char * link;
		char * image;
		char * discription;
		char * length;
		char *  pubdata;
		char *  play_video_url;

	}youx_item;


	typedef  struct{

		char * youx_website_icon;

		int total_item;
		youx_item * p_youx_item_list;

	} youx_data_t;


    
	typedef struct {

		int  event_flag;
		int  category_num;  /*total category*/
		int  item_num_total;  /*total item*/
		char * p_category_name[MAX_CATEGORY_NUM]; /*reserve name of category*/

		int item_num;     
		int  page_index;
		char  *  p_item_video_id[MAX_ITEM_NUM_OF_RSS];
		char  *  p_item_description[MAX_ITEM_NUM_OF_RSS];
		char  *  p_item_author[MAX_ITEM_NUM_OF_RSS];  /*author of each item in one page*/
		char  *  p_item_title[MAX_ITEM_NUM_OF_RSS];  /*title of each item in one page*/
		char  *  p_rating[MAX_ITEM_NUM_OF_RSS];       /*rating of each item in one page*/
             char  *  p_viewcount[MAX_ITEM_NUM_OF_RSS];
		char  *  p_duration[MAX_ITEM_NUM_OF_RSS];   /*duration of each item in one page*/
		char  *  p_thumnaildef[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnailmq[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnailsd[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnailhq[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnail1[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnail2[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		char  *  p_thumnail3[MAX_ITEM_NUM_OF_RSS];   /*thumnail of each item in one page*/
		unsigned int  fail_no;
		unsigned int  identify_code;               /*identify the source of the event*/

		int   filmNum;
		char  * p_url[MAX_FILM_LIST_LEN];
		int    *  p_itag;
		int    *  p_orig_index;
		char  * p_filmName;

		youx_data_t * p_youx_data;//only for youporn
		/*bit31 bit30,... bit24  identify the feed type

		  from bit23~bit16:  category index
		  from bit15~bit8:  page number
		  from bit7~bit0:   item number

		 */
	} YT_DP_EVENT_PARAM_T;

	/*
	   typedef struct {

	   int   filmNum;
	   char  * p_url[MAX_FILM_LIST_LEN];
	   int    *  p_itag;
	   int    *  p_orig_index;
	   char  * p_filmName;
	   unsigned int  fail_no;
	   unsigned int  identify_code;

	   } YT_DP_GET_PLAYURL_OK_EVENT_PARAM_T;
	 */


	typedef  enum{

		YT_DP_INVALID_EVE = 0,
		YT_DP_EVE_INIT_OK = 1,  /*youtube dataprovider is initilized successfully*/
        YT_DP_EVE_DEINIT_OK = 2,  /*youtube dataprovider is destroyed successfully*/
        YT_DP_EVE_DEINIT_ALL_OK = 3,  /*youtube dataprovider is destroyed successfully*/
		YT_DP_EVE_NEW_CATEGORY_ARRIVE  = 4,/*new category data is coming in*/
		YT_DP_EVE_NEW_PAGE_ITEM_ARRIVE = 5,/*new page items data is coming in*/
		YT_DP_EVE_NEW_PLAY_URL_ARRIVE = 6,  /*new playvideo url is coming in*/
		YT_DP_EVE_FAIL_OPERATION = 7,            /*last operation is failed*/
		YT_DP_EVE_SET_MAX_ITEM_NUM_OK = 8,
		YT_DP_EVE_GET_CATEGORY_TOTAL_OK = 9,
		YT_DP_EVE_SEARCH_OK = 10,
		YT_DP_EVE_SET_REGION_ID_OK =11,
		YT_DP_EVE_SET_TIME_OK = 12,
		YT_DP_EVE_MAX,

	} YOUTUBE_DP_EVENT_TYPE;


	typedef RET_CODE(*yt_event_cb_fun_t)(YOUTUBE_DP_EVENT_TYPE type, u32 param);


	typedef  struct {
        int       index;
		char     playUrlArray[MAX_FILM_LIST_LEN][PLAY_URL_MAX_LEN];

		char    *  h264EncUrlArray[MAX_FILM_LIST_LEN];
		int          h264EncUrlIndex;
		int          h264EncUrlItagArray[MAX_FILM_LIST_LEN];
		int          h264EncUrlOrigIndexArray[MAX_FILM_LIST_LEN];


	} youtube_video_info_t;


	typedef struct {

		YT_REGION_ID_T  id;
		char *                 country;

	} region_desc_t;

	typedef struct{

		YT_TIME_T    time_id;
		char *           time;

	}time_desc_t;

	typedef  struct {

		YT_FEED_TYPE   feed_type;
		YT_REGION_ID_T  id;

	} INIT_FEED_PARAM_T;


	typedef  struct {

		YOUTUBE_DP_USER_CMD_TYPE type;
		u32 param;
		char key[512];

	} USR_CMD_T;





	typedef  struct {

		COMMON_DATA

			int  task_prio;
		void * task_mem;
		unsigned int lock;
		unsigned int callback_lock;
		unsigned int cmd_fifo_lock;

           
		youx_data_t  * p_youx;//^^ for youporn
		youtube_video_info_t * p_video_info;
        request_url_t * p_request_url;//added by xizhou, store the urls request from server.

		u8 is_stop;

		USR_CMD_T cmd_fifo[YT_CMD_FIFO_LEN];
		USR_CMD_T cmd_fifo_backup[YT_CMD_FIFO_LEN];
		unsigned char cmd_num;


		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(YT_FEED_TYPE)feed_type,
(YT_REGION_ID_T)id
return: (int) 0 success,-1 fail
		 */
		init_fun_t     init;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) 0 success,-1 fail
		 */
		deinit_fun_t  deinit;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(char * )key_word,
(YT_REGION_ID_T )id:not use
return: (int) 0
		 */
		search_fun_t  search;
		yt_event_cb_fun_t  event_callback;


		/*
		 *  the following function will be exposed to upper layer code
		 *
		 */
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(YT_REGION_ID_T )id:not use
return: (int) item index,-1 fail
		 */
		set_region_id_fun_t                  set_region_id;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(char * )time:today/week etc
return: (int) item index,-1 fail
		 */
		set_time_fun_t                  set_time;

		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) item index,-1 fail
		 */
		get_item_index_fun_t                  get_focus_item_index;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) category index,-1 fail
		 */
		get_category_index_fun_t             get_focus_category_index;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(int) index
return: (int) 0 success,-1 fail
		 */
		set_category_index_fun_t                set_focus_category_index;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(int) index
return: (int) 0 success,-1 fail
		 */
		set_item_index_fun_t                     set_focus_item_index;          
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(int) index
return: (int) 0 success,-1 fail
		 */
		set_page_index_fun_t                     set_focus_page_index;
		/*   
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(char *) video_id
return: (int) 0 success,-1 fail
		 */
		set_video_id_fun_t                     set_video_id;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) page_index
		 */
		get_page_index_fun_t                      get_page_index;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) 0
		 */
		get_next_page_fun_t                      get_next_page;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) 0
		 */
		get_prev_page_fun_t                         get_prev_page;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) 0
		 */
		get_cur_page_fun_t                         get_cur_page;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return:
(int) item num,-1 fail
		 */
		get_item_num_fun_t                         get_item_num;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) category num,-1 fail
		 */
		get_category_num_fun_t                   get_category_num;
		/*
		   invalid
		 */
		set_max_category_cnt_fun_t            set_max_category_cnt;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(int) max item num
return: (int) 0
		 */
		set_max_item_cnt_fun_t                 set_max_item_cnt;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
return: (int) 0
		 */
		get_video_play_url_fun_t                 get_play_url;
		/*
		   invalid
		 */
		init_url_fun_t                                      init_url;
		/*
		   invalid
		 */
		get_item_info_fun_t                            get_item_info;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(int) index of item
(const char *) keyword :"title","videoid","thumnail_url","duration","rating"
(int ) indexForMultipleResults:reserved,not use now .
return: (const char *) info,NULL fail
		 */

		get_item_info_ex_fun_t                       get_item_info_ext;
		/*
		   invalid
		 */
		on_item_action_fun_t                           on_item_action;
		/*
in:
(void *) hdl :(YOUTUBE_DATA_T *)
(void *) p_cb_func
return: void
		 */
		register_event_cb_func_t                      register_event_callback;
        int leak_cnt;


	} YOUTUBE_DATA_T;

	typedef struct {

		u8*  p_stack_mem;
		int   stack_size;
		int   priority;
		int   http_priority;

	} INIT_TASK_PARAM_T;

	YOUTUBE_DATA_T *  register_youtube_dataprovider(void *);

	void   unregister_youtube_dataprovider(void  * phal);



#ifdef __cplusplus
}
#endif


#endif
