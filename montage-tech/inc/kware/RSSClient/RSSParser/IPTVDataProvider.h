#ifndef  __FUNNYVIDEO_DATA_PROVIDER__
#define  __FUNNYVIDEO_DATA_PROVIDER__

#ifdef __cplusplus
extern "C" {
#endif 

#define IPTV_STRING_LONG     (2048)
#define IPTV_STRING_SHORT   (128)

typedef  enum{

	GET_IPTV_CATEGORY,
	GET_IPTV_AREA,
	GET_IPTV_CATEGORY_TYPE,
	GET_IPTV_NEW_PAGE,
	GET_IPTV_RECOMMEND_INFO,
	GET_IPTV_VIDEO_INFO,
	GET_IPTV_PLAY_URL,
	GET_IPTV_FORMAT_TYPE,
	GET_IPTV_PLAYBACK_INFO,
	GET_IPTV_INIT ,///////add for IPTV_initDataProvider to a thread 
	GET_IPTV_DEINIT,

	GET_IPTV_INVALID_TYPE,

}IPTV_VIDEO_CMD_TYPE;

typedef enum {
    IPTV_DATA_FAIL=0,
    IPTV_DATA_SUCCESS=1,
    IPTV_DATA_MAX,  
}IPTV_DATA_STATUS;

typedef enum {
      IPTV_FORMAT_NORMAL = 0x01,
      IPTV_FORMAT_HIGH = 0x02,
      IPTV_FORMAT_SUPER = 0x04,    
      IPTV_FORMAT_JISU = 0x08,    
      IPTV_FORMAT_DEFAULT = 0x10,
}IPTV_VIDEO_FORMAT_T;


/*
*IPTV_CATEGORY_T is used to describe a category(movie, music ... ...)
*/
typedef  struct{
  /*
  * category id used for identifying this category
  */
   int id;

  /*
  * category name used for displaying in UI
  */
   char  * name;

  /*
  *category types, not used currently
  */
   int types;
}IPTV_CATEGORY_T;


typedef struct{
    /*
    * the number of category in array
    */
    int number;

    /*
    * array of category
    */
    IPTV_CATEGORY_T *array;

    /*
    * status of this request
    */
    IPTV_DATA_STATUS status;

    u32 identify;
}IPTV_CATEGORY_LIST_T;


/*
* IPTV_CATEGORY_TYPE_T is used to describe one category type(such as 爱情，动作)
*/
typedef struct{
    int id;

    /*
    * which category it belongs to
    */
    int catoryid;

    /*
    * charaters being used for displaying in UI
    */
    char *title;

    /*
    * keys this type belongs to, mainly used to search
    */
    char *keys;

    int sort;

    /*
    * total programs in this category type
    */
    int total;

}IPTV_CATEGORY_TYPE_T;

/*
* IPTV_CATEGORY_TYPE_LIST_T describe all types in a category
*/
typedef struct{

    /*
    * describe the size of array below
    */
    int item_count;

    int cat_id;

    IPTV_CATEGORY_TYPE_T *array;


}IPTV_CATEGORY_TYPE_LIST_T;

/*
*IPTV_CATEGORY_TYPE_ALL_T describe all category's types for a data provider
*/
typedef struct{

    /*
    * describe the size of p_list below
    */
    int cat_count;
    
    IPTV_CATEGORY_TYPE_LIST_T *p_list;
      
    IPTV_DATA_STATUS status;

    u32 identify;
}IPTV_CATEGORY_TYPE_ALL_T;


/*
* IPTV_CHANNEL_T describes a program
*/
typedef struct{

    /*
    * this program's id
    */
   char * id;   

   /*
   *this program's title
   */ 
   char *   title;

   /*
   * the category this program belongs to
   */
   int cat_id;

   /*
   *  the url of image
   */
   char *   image;   

   /*
   * the types of this program(爱情，动作...)
   */
   char *   types;

   /*
   * the score of this program, such as 9.6, 7.3 and so on
   */
   char *   score;

   /*
   * clarity, such as 高清，标清
   */
   char *   clarity;

   /*
   *used for UI
   */
   int  issinglepage;

}IPTV_CHANNEL_T;

typedef struct{

  /*
  * describe the size of array
  */
  int number;

  /*
  * page count of this category calculate by current page size
  */
  int pagecount;

  /*
  *all programs count in this category
  */
  int totalcount;
  

  IPTV_CHANNEL_T *array;
  
  IPTV_DATA_STATUS status;

  
  /*
  * being used when retriving data to up layer 
  * return just as parameters in
  */
  int category_id;
  int page_num;
  int page_size;

  u32 identify;
  u32 user_data0;

}IPTV_CHANNEL_LIST_T;


typedef struct{

    /*
    * origin (优酷，土豆)
    */
    char *origin;

    /*
    * collect(第一集，第二集)
    */
    char *collect;

    /*
    *  url
    */
    char *playurl;

    /*
    * title
    */
    char *urltitle;

    //used for uplayers
    u32 identify;
}IPTV_CHANNEL_URL_T;


typedef struct{

    char * id;
    char *title;
    int cat_id;
    /*
    *演员
    */
    char *starring;

    /*
    *program's description
    */
    char *mark;

    /*
    * program's area(香港，美国...)
    */
    char *area;

    /*
    * 导演
    */
    char *director;

    /*
    * image's url
    */
    char *img;

    /*
    * program's types(爱情，动作)
    */
    char *types;

    /*
    * program's years, such as 2009, 2013
    */
    char *years;

    /*
    * subpage count
    */
    int page_count;

    /*
    *all sub items
    */
    int total_count;

    /*
    * how much items in urls below
    */
    int number;

    /*
    *score
    */
    char *score;

    /*
    * where this program comes from, such as youku, tudou...
    */
    char *origins;

    IPTV_CHANNEL_URL_T *urls;

    /*
    * 
    */
    int is_description;

    IPTV_DATA_STATUS status;

    u32 identify;
    u32 user_data0;
}IPTV_CHANNEL_INFO_T;


typedef struct{

    int number;
    
    IPTV_CHANNEL_URL_T *playurl;

    u32 format;

    IPTV_DATA_STATUS status;

    u32 identify;
}IPTV_CHANNEL_PLAY_URL_T;


/*
*IPTV_RECMD_CHANNEL_T describes recommend information
*/
typedef struct{
      char * id;

      char *title;

      int cat_id;

      char *img;

      char *types;

      char *score;

      char *currentcollect;

      char *clarity;      

}IPTV_RECMD_CHANNEL_T;


/*
* IPTV_RECMD_INFO_T describes a program's all recommands information
*/
typedef struct{

  int channel_num;

  int pagecount;

  int totalcount;
  
  IPTV_RECMD_CHANNEL_T *p_channel;

  IPTV_DATA_STATUS status;

  u32 identify;
  
}IPTV_RECMD_INFO_T;


typedef  struct{
   int id;
   char  * name;
   int status;
   int sort;
}IPTV_AREA_INFO_T;


/*
*IPTV_AREA_INFO_LIST_T describes all area infomation of a data provider
*/
typedef struct{

    /*
    * the number of members in array below
    */
    int number;

    /*
    * area arrayes
    */
    IPTV_AREA_INFO_T *array;

    /*
    * the status of this command
    */
    IPTV_DATA_STATUS status;

    u32 identify;
}IPTV_AREA_INFO_LIST_T;


typedef enum
{
    IPTV_EVENT_INVAILID,
    IPTV_INIT_SUCCESS,
    IPTV_INIT_FAIL,
    IPTV_CATEGORY_ARRIVAL,
    IPTV_AREA_INFO_ARRIVAL,
    IPTV_CATEGORY_TYPE_ARRIVAL,
    IPTV_NEW_PAGE_ARRIVAL,
    IPTV_VIDEO_INFO_ARRIVAL,
    IPTV_PLAY_URL_ARRIVAL,
    IPTV_FORMAT_TYPE_ARRIVAL,
    IPTV_PLAYBACK_INFO_ARRIVAL,
    IPTV_RECOMMEND_INFO_ARRIVAL,
    IPTV_DEINIT_SUCCESS,
}IPTV_EVENT_TYPE;


typedef enum
{
    IPTV_PAGE_SORT_DEFAULT,
    IPTV_PAGE_SORT_NEWEST,
    IPTV_PAGE_SORT_HOTTEST,
    IPTV_PAGE_SORT_SCORE,
    IPTV_PAGE_SORT_ROMDAM,
}IPTV_PAGE_SORT_TYPE;

typedef RET_CODE  (*iptv_event_callback)(IPTV_EVENT_TYPE event, u32 param);


/*
* Return value:
*       zero:  do it
*       none-zero:  ignore it
*/
typedef int (*ignore_callback)(u32 para);


/*
* the parameter of IPTV_updatePage()
*/
typedef struct{
	int cat_id;
    int years;
    char names[IPTV_STRING_LONG];
    char title[IPTV_STRING_LONG];
    char area[IPTV_STRING_SHORT];
    
    /*
    *title first char "rzdf" = "让子弹飞"
    */
    char pinyin[IPTV_STRING_SHORT];

    /*
    *only one of "最新", "热播","推荐"
    */
    char attr[IPTV_STRING_SHORT];

    char actor[IPTV_STRING_SHORT];
    char director[IPTV_STRING_SHORT];
    
    char types[IPTV_STRING_SHORT];

    /*
    * used for search, co-operate with types
    */
    char keys[IPTV_STRING_SHORT];

    IPTV_PAGE_SORT_TYPE sort;
    
	int page_index;
	int page_size;

    ignore_callback cb;

    u32 identify;
    u32 user_data0;
}IPTV_UPPAGE_REQ_T;

/*
* the parameter of IPTV_getRecommendInfo()
*/
  typedef struct{
      char * id;
      int cat_id;

      int page_index;

      int page_size;
      
      char area[IPTV_STRING_SHORT];
      char types[IPTV_STRING_SHORT];  

      ignore_callback cb;
      u32 identify;
  }IPTV_RECOMMEND_REQ_T;

/*
* the parameter of IPTV_getVideoInfo()
*/
typedef struct{
    int cat_id;

    char * id;

    /*
    * such as "搜狐", "优酷"
    */
    char origin[IPTV_STRING_SHORT];

    int page_index;

    int page_size;

    /*
    * is_description
    *   1: return program description without url
    *   0: return description and urls if they exist
    */
    int is_description;

    u32 identify;

    ignore_callback cb;
    
    u32 user_data0;
}IPTV_VIDEO_INFO_REQ_T;

/*
* the parameter of IPTV_getPlayUrl()
*/
typedef struct{

    /*
    * a program's website url, some data provider use 
    * this url to get video play url
    */
    char url[IPTV_STRING_LONG];

    /*
    * definition: normal, high, super
    */
    IPTV_VIDEO_FORMAT_T format;

    ignore_callback cb;

    u32 identify;
}IPTV_VIDEO_URL_REQ_T;


typedef struct{

    int id;
	int cat_id;
    int years;
    int month;
    int date;
    char names[IPTV_STRING_LONG];
    char title[IPTV_STRING_SHORT];
    char area[IPTV_STRING_SHORT];

    char actor[IPTV_STRING_SHORT];
    char director[IPTV_STRING_SHORT];
    char types[IPTV_STRING_SHORT];
    char origin[IPTV_STRING_SHORT];
    char url[IPTV_STRING_LONG];
    char pinyin[IPTV_STRING_SHORT];
    char attr[IPTV_STRING_SHORT];
    char keys[IPTV_STRING_SHORT];

    IPTV_VIDEO_FORMAT_T format;
    IPTV_PAGE_SORT_TYPE sort;
	int page_index;
	int page_size;

    int is_description;


    ignore_callback cb;
    u32 identify;
    u32 user_data0;
} IPTV_REQ_PARAM_T;


typedef IPTV_CATEGORY_LIST_T *   (*get_category_list_func)(void * hdl);
typedef IPTV_CATEGORY_TYPE_ALL_T *   (*get_category_type_list_func)(void * hdl, int cat_id);
typedef IPTV_CHANNEL_LIST_T *   (*update_page_func)(void * hdl, IPTV_UPPAGE_REQ_T * p_req);
typedef IPTV_RECMD_INFO_T *   (*get_recommend_info_func)(void * hdl, IPTV_RECOMMEND_REQ_T * p_req);
typedef IPTV_CHANNEL_INFO_T *   (*get_video_info_func)(void * hdl, IPTV_VIDEO_INFO_REQ_T * p_req);
typedef IPTV_CHANNEL_PLAY_URL_T *   (*get_playurl_func)(void * hdl, IPTV_VIDEO_URL_REQ_T *p_req);
typedef u32   (*get_playurl_format_func)(void * hdl, IPTV_VIDEO_URL_REQ_T *p_req);
typedef IPTV_AREA_INFO_LIST_T *   (*get_area_func)(void * hdl);


typedef struct{

	IPTV_CATEGORY_LIST_T  * category_list;
    IPTV_CATEGORY_TYPE_ALL_T *category_type_all;
	IPTV_CHANNEL_LIST_T    * channel_list;
    IPTV_CHANNEL_INFO_T   * channel_info;
    IPTV_CHANNEL_PLAY_URL_T    *  channel_play_url;
    IPTV_RECMD_INFO_T *recommend_info;
    IPTV_AREA_INFO_LIST_T * area_info_list;
    
	void  * 	p_bg_task_hdl;

    register_event_cb_func_t    register_event_callback;
    iptv_event_callback event_callback;

    int page_size;

    IPTV_DATA_PROVIDER  dp;
	u32                         sub_dp;

    /*
    * the private data of each data provider can be placed here
    */
    void * priv;

    /*
    *
    */
    char *mac;

    char *id;

    int (*read_flash)(void *start, int len);

    int (*write_flash)(void *start, int len);

    /*
    *
    */
    get_category_list_func  get_category_list;

    get_category_type_list_func get_category_type_list;

    update_page_func  update_page;

    get_recommend_info_func get_recommend_info;

    get_video_info_func get_video_info;

    get_playurl_func get_playurl;

    get_playurl_format_func get_playurl_format;

    get_area_func get_area;

}IPTV_DP_HDL_T;



IPTV_DP_HDL_T *  IPTV_initDataProvider(DO_CMD_TASK_CONFIG_T * p_config);

int  IPTV_deinitDataProvider(IPTV_DP_HDL_T * hdl);

int   IPTV_getCategoryList(IPTV_DP_HDL_T * hdl, u32 identify);

/*
* cat_id: 0 means all types in all categary
*/
int   IPTV_getCategoryTypeList(IPTV_DP_HDL_T * hdl, int cat_id, u32 identify);


int  IPTV_updatePage(IPTV_DP_HDL_T * hdl, IPTV_UPPAGE_REQ_T  *p_req);

int  IPTV_getRecommendInfo(IPTV_DP_HDL_T * hdl, IPTV_RECOMMEND_REQ_T *p_req);

int  IPTV_getVideoInfo(IPTV_DP_HDL_T * hdl, IPTV_VIDEO_INFO_REQ_T *p_req);

int  IPTV_getPlayUrl(IPTV_DP_HDL_T * hdl, IPTV_VIDEO_URL_REQ_T *p_req);

/*
  * pass url to p_req->url
*/
int  IPTV_getPlayUrlFormat(IPTV_DP_HDL_T * hdl, IPTV_VIDEO_URL_REQ_T *p_req);


int  IPTV_setPageSize(IPTV_DP_HDL_T * hdl, int pagesize);

int  IPTV_getArea(IPTV_DP_HDL_T * hdl, u32 identify);




#ifdef __cplusplus
}
#endif 





#endif
