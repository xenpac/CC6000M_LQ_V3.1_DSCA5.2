#ifndef __NETMEDIA_DP_H__
#define __NETMEDIA_DP_H__

#ifdef __cplusplus
extern "C" {
#endif 


#define NM_STRING_SHORT_LEN  128
#define NM_STRING_MID_LEN  512
#define NM_STRING_LONG_LEN  2048


typedef enum
{
    NM_EVENT_INVAILID,
    NM_INIT_SUCCESS,
    NM_INIT_FAIL,
    NM_WEBSITE_INIT_SUCCESS,
    NM_WEBSITE_INIT_FAIL,    
    NM_WEBSITE_INFO_ARRIVAL,
    NM_CLASS_ARRIVAL,
    NM_SUBCLASS_ARRIVAL,
    NM_PAGE_ARRIVAL,
    NM_PROGRAM_INFO_ARRIVAL,
    NM_COLLECTS_INFO_ARRIVAL,
    NM_PLAY_URLS_ARRIVAL,
    NM_SEARCH_ARRIVAL,
    NM_WEBSITE_DEINIT_SUCCESS,
    NM_WEBSITE_DEINIT_FAIL,    
    NM_DEINIT_SUCCESS,
    NM_DEINIT_FAIL,

	NM_PRE_ACTIVE,
	
    NM_EVENT_MAX,
}NM_EVENT_TYPE;

typedef enum{
	NETMEDIA_PRE_ACTIVE_REPOS_TYPE_INVAILID = 0 ,
	NETMEDIA_PRE_ACTIVE_SUCCESS  = 1 ,
	NETMEDIA_PRE_ACTIVE_FAIL_BY_NET  = 2 ,
	NETMEDIA_PRE_ACTIVE_FAIL_BY_MSG_ERROR  = 3 ,
	NETMEDIA_PRE_ACTIVE_FAIL_BY_WRONG_CODE  = 4 ,
	NETMEDIA_PRE_ACTIVE_FAIL_BY_EXPIRE_CODE  = 5 ,
	NETMEDIA_PRE_ACTIVE_FAIL_BY_WRONG_PARAM  = 6 ,
	
	NETMEDIA_PRE_ACTIVE_REPOS_TYPE_MAX,
}NETMEDIA_PRE_ACTIVE_REPOS_E;

typedef enum
{
    NM_SUB_ATTR_NULL,
    NM_SUB_ATTR_ITEM,
    NM_SUB_ATTR_CLASS,
    NM_SUB_ATTR_MAX,
}NM_SUBCLASS_ATTR_E;

typedef enum
{
      NETMEDIA_DP_NULL = 0,
      NETMEDIA_DP_VIMEO =1 ,
      NETMEDIA_DP_ONLINE_MOVIES =2 ,
      NETMEDIA_DP_ONLAIN_FILM =3,
      NETMEDIA_DP_DAILYMOTION =4,
      NETMEDIA_DP_ALJAZEER =5,
      NETMEDIA_DP_VEOH =6,
      NETMEDIA_DP_YAHOO_VIDEO =7,
      NETMEDIA_DP_ADDANIME_VIDEO =8,
	  NETMEDIA_DP_KONTRTUBE =9 ,
      NETMEDIA_DP_SPORTS_VIDEO =10 ,
      NETMEDIA_DP_ARABIC_FILM =11 ,
      NETMEDIA_DP_MBCSHAHID =12,
      NETMEDIA_DP_YOUKU =13 ,
      NETMEDIA_DP_DUBAI_MEDIA =14,
      NETMEDIA_DP_YOUTUBE =15,
      NETMEDIA_DP_YOUPORN =16,
      NETMEDIA_DP_DONANG =17,
      NETMEDIA_DP_WEBYOUTUBE = 18,
      NETMEDIA_DP_2B2_SELFIP =19,
      NETMEDIA_DP_BEFUCK = 20,
      NETMEDIA_DP_TUKIF = 21,
      NETMEDIA_DP_ANYPORN = 22,
      NETMEDIA_DP_EPORNER = 23,
      NETMEDIA_DP_BASKINO = 24,
      
      NETMEDIA_DP_MAX,
}NETMEDIA_DP_TYPE;

typedef enum
{
    NETMEDIA_VIDEO_NULL,
    NETMEDIA_VIDEO_SD,
    NETMEDIA_VIDEO_HD,
    NETMEDIA_VIDEO_1080P,
    NETMEDIA_VIDEO_1080I,
    NETMEDIA_VIDEO_720P,
    NETMEDIA_VIDEO_720I,
    NETMEDIA_VIDEO_MAX,
}NETMEDIA_VIDEO_RESOLUTION_E;


typedef enum {
    NETMEDIA_DATA_NULL ,	
    NETMEDIA_DATA_SUCCESS,
    NETMEDIA_DATA_FAIL,
    NETMEDIA_DATA_MAX,  
}NETMEDIA_DATA_STATUS;

typedef enum{
      NETMEDIA_UI_ATTR_SEARCH = 0x00000001,
      NETMEDIA_UI_ATTR_FASTPAGE = 0x00000002,
      NETMEDIA_UI_ATTR_RESOLUTION = 0x00000004,
}NETMEDIA_UI_ATTR_E;

typedef enum{
      NETMEDIA_UI_STYLE_INVALID,
      NETMEDIA_UI_STYLE_SIMPLE,
      NETMEDIA_UI_STYLE_COUNTRY,
      NETMEDIA_UI_STYLE_CLASSIC,
      NETMEDIA_UI_STYLE_MAX,
}NETMEDIA_UI_STYLE_E;

typedef enum{
      NETMEDIA_URL_TYPE_VOD,
      NETMEDIA_URL_TYPE_LIVE,
      NETMEDIA_URL_TYPE_MAX,
}NETMEDIA_URL_TYPE_E;


typedef enum{
    NETMEDIA_ITEM_SIMPLE_VIDEO,
    NETMEDIA_ITEM_COLLECT_VIDEOES,
}NETMEDIA_ITEM_TYPE_E;

typedef RET_CODE  (*nm_event_callback)(NM_EVENT_TYPE event, u32 param1, u32 param2);

/*
* Return value:
*       zero:  do it
*       none-zero:  ignore it
*/
typedef int (*nm_ignore_callback)(u32 para);





/*
    parameter of NetMedia_initDataProvider
*/
typedef struct
{
    int  priority;
    
    nm_event_callback cb;

}NM_INIT_PARAM;


typedef struct
{
     NETMEDIA_PRE_ACTIVE_REPOS_E status ;

	char expire_time[NM_STRING_SHORT_LEN];
	 
}NM_PRE_ACTIVE_REPOS_PARAM;

typedef struct
{
     NETMEDIA_DP_TYPE nm_dp_type;

     char* activate_code ;  

     u8 active_by_force ; 
     
}NM_PRE_ACTIVE_PARAM;

typedef struct
{
     NETMEDIA_DP_TYPE nm_dp_type;
     
}NM_WEBSITE_INIT_PARAM;

typedef struct
{
      NETMEDIA_DP_TYPE  dp_type;
        
      char *title;
      
      char *logo_url;

      NETMEDIA_UI_STYLE_E ui_style;
    
      NETMEDIA_UI_ATTR_E ui_attr;
}NM_WEBSITE_ITEM_T;


typedef struct
{
    int  rsc_attr ;   ////default is 0 ,. use the url to get ; while is 1, get from flash

    int count;
    
    NM_WEBSITE_ITEM_T *item;
    
    NETMEDIA_DATA_STATUS status;
}NM_WEBSITE_INFO_T;

/*
<----------------NetMedia_getSubClassList----------------->
*/
/*
    entry parameter of NetMedia_getSubClassList
*/
typedef struct
{
    int class_id;

    char class_name[NM_STRING_SHORT_LEN];
    
    nm_ignore_callback cb;

    u32 identify;

}NM_SUBCLASS_REQ;


typedef struct
{
    int class_id;

    char *class_name;

    /*
      to tell its sub item is classes or programs
    */
    NM_SUBCLASS_ATTR_E attr;

}NM_CLASS_ITEM_INFO_T;


/*
    return structure of NetMedia_getClassList and NetMedia_getSubClassList
*/
typedef struct
{
    /*
        total class number
    */
    int count;

    NM_CLASS_ITEM_INFO_T *class_item;

    u32 identify;
    
    NETMEDIA_DATA_STATUS status;
}NM_CLASS_INFO_T;



/*
<----------------NetMedia_updatPage----------------->
*/
/*
  entry parameter of NetMedia_updatPage
*/
typedef struct
{
    int class_id;

    char class_name[NM_STRING_SHORT_LEN];

    int subclass_id;

    char subclass_name[NM_STRING_SHORT_LEN];

    int page_number;
    
    nm_ignore_callback cb;

    u32 identify;

}NM_UPDATEPAGE_REQ;


typedef struct
{
      char *item_id;
      
      char *name;

      char *date;

      char *image_url;

      char *duration;

      char *description;

      char *score;

      char *view_count;

      char *author;

      NETMEDIA_ITEM_TYPE_E type;
}NM_ITEM_INFO_T;

/*
  return structure of NetMedia_updatPage 
*/
typedef struct
{
    int count;

    NM_ITEM_INFO_T *items;

    int total_items;

    u32 identify;

    NETMEDIA_DATA_STATUS status;
}NM_PAGE_INFO_T;

/*
<----------------NetMedia_getProgramInfo----------------->
*/
typedef struct
{
     char item_id[NM_STRING_SHORT_LEN];

     char item_name[NM_STRING_SHORT_LEN];

     nm_ignore_callback cb;
     
     u32 identify;
}NM_PROGINFO_REQ;


typedef struct
{
    //total collection of this program
    int collection_count;

    char *item_id;

    char *name;

    char *date;

    char *image_url;

    char *duration;

    char *description;

    char *score;

    char *view_count;


    u32 identify;
    
    NETMEDIA_DATA_STATUS status;
}NM_PROGINFO_T;


typedef struct
{      
      char item_id[NM_STRING_MID_LEN];
      
      char item_name[NM_STRING_SHORT_LEN];

      int page_index;

      int page_size;
      
      nm_ignore_callback cb;
      
      u32 identify;
}NM_COLLETSINFO_REQ;

typedef struct
{
      char *collect_name;

      //collect_id is used to specify a collect, driver use it to get play url,
      //it likes info_url in vod
      char *collect_id;

      //this collect's duration
      char *duration;
}NM_COLLETSINFO_ITEM_T;

typedef struct
{      
      int count;

      NM_COLLETSINFO_ITEM_T *collect_info;

      int total_count;

      u32 identify;
      
      NETMEDIA_DATA_STATUS status;
}NM_COLLETSINFO_T;


/*
<----------------NetMedia_getPlayUrls----------------->
*/
/*
  entry parameter of NetMedia_getPlayUrls
*/
typedef struct
{
    char item_id[NM_STRING_SHORT_LEN];

    char item_name[NM_STRING_MID_LEN];
    
    nm_ignore_callback cb;

    u32 identify;
}NM_PLAY_URLS_REQ;


typedef struct
{
      NETMEDIA_VIDEO_RESOLUTION_E resolution;

      int count;

      char **playurl;

      NETMEDIA_URL_TYPE_E type;
}NM_PLAY_URL_ITEM;
/*
  return structure of NetMedia_getPlayUrls 
*/
typedef struct
{
    int url_count;

    NM_PLAY_URL_ITEM * playurls;

    u32 identify;

    NETMEDIA_DATA_STATUS status;
}NM_PLAY_URLS_T;

/*
<----------------NetMedia_search----------------->
*/
/*
  entry parameter of NetMedia_search
*/
typedef struct
{
      int mainclass_id;
      
      int page_number;

      nm_ignore_callback cb;
      
      u32 identify;

}NM_SEARCH_REQ;

void *  NetMedia_initDataProvider(NM_INIT_PARAM *param);

int NetMedia_Pre_Active(void *hdl, NM_PRE_ACTIVE_PARAM *param);

int NetMedia_initWebsite(void *hdl, NM_WEBSITE_INIT_PARAM *param);

int NetMedia_deinitWebsite(void *hdl);

int NetMedia_getClassList(void *hdl);

int NetMedia_getWebsiteInfo(void *hdl);

int NetMedia_getSubClassList(void *hdl, NM_SUBCLASS_REQ *req);

int NetMedia_updatPage(void *hdl, NM_UPDATEPAGE_REQ *req);

int NetMedia_getProgramInfo(void *hdl, NM_PROGINFO_REQ *req);

int NetMedia_getCollectsInfo(void *hdl, NM_COLLETSINFO_REQ *req);

int NetMedia_getPlayUrls(void *hdl, NM_PLAY_URLS_REQ *req);

int NetMedia_search(void *hdl, NM_SEARCH_REQ *req);

int NetMedia_setPageSize(void *hdl, int page_size);

int NetMedia_setSearchKeyword(void *hdl, char *keyword);


int  NetMedia_deinitDataProvider(void * hdl);

#ifdef __cplusplus
}
#endif 


#endif
