/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __LIVETV_DATA_PROVIDER__
#define  __LIVETV_DATA_PROVIDER__

#ifdef __cplusplus
extern "C" {
#endif

#define TVLIVE_STRING_LONG     (512)
#define TVLIVE_STRING_SHORT   (128)



typedef enum {
    TVLIVE_DATA_FAIL=0,
    TVLIVE_DATA_SUCCESS=1,
    TVLIVE_DATA_MAX,
}
TVLIVE_DATA_STATUS;

typedef struct {
    int year;

    int month;

    int day;

} tvlive_date;

typedef  struct {
    int id;/*category id*/
    char  * name;/*category name*/
    int types/*no use , reserved for the future*/;

} LIVE_TV_CATEGORY_T;

typedef struct {
    int  num;/*total categorys*/
    TVLIVE_DATA_STATUS status;/*identify wether dataprovider get the relative data
                                               for the upper layer*/
    LIVE_TV_CATEGORY_T * array;/*category array*/

} LIVE_TV_CATEGORY_LIST_T;

#define MAX_URL_NUM 32

typedef struct {

    int id;/*channel id*/
    char pid[8];/*program id*/
    int num;/*playurl total*/
    char  *  title;/*channel name*/
    char  *  image;
    int         category_id;/*category id*/
    u32    urlid[MAX_URL_NUM];   ///add this domain for sort the playing url , is it is 0 ,means can not get the url_id
    char   *  playurl[MAX_URL_NUM];
    char   *  urlSource[MAX_URL_NUM];/*source of the relative playurl*/

    //// the id ,which is the same value as that in satellite channel list , means the channel has two source: internet / satellite
    //// if it is 0 , means the channel is not in satellite channel list
    u32  satelliteId ;
    //  BOOL satellite_use_flag ;  ////if it is 0 , means the channel is not in satellite channel list ; = 1 means the channel is in satellite channel list

    /////add the satellite info
    u32 satellite_freq ;
    u32 satellite_symbol_rate ;
    BOOL satellite_polarity ;   //// L/H = 0, R/V = 1
    BOOL satellite_type ;   ////tv =1 , radio=0
    char * satellite_icon_link ;

    u32 satellite_vpid ;
    u32 satellite_apid ;
    u32 satellite_pcrpid ;
} LIVE_TV_CHANNEL_T;




typedef enum {
    /*!
    xxxxxxxx
    */
    URL_SPEED_IDLE,
    /*!
    xxxxxxxx
    */
    URL_SPEED_DONE,
    /*!
    xxxxxxxx
    */
    URL_SPEED_INVALID,
    /*!
    xxxxxxxx
    */


} URL_SPEED_STATE;

typedef enum {
    /*!
    xxxxxxxx
    */
    SORT_TASK_FALSE,
    /*!
    xxxxxxxx
    */
    SORT_TASK_RUN,
    /*!
    xxxxxxxx
    */
    SORT_TASK_TRUE,
    /*!
    xxxxxxxx
    */


} SORT_TASK_STATE;
#define URL_TICKS_THRESHOLD (100*60*30)
typedef struct {

    int id;/*channel id*/
    int index;
    int num;
    char   *  playurl[32];
    int sort_status;//0 not sorted 1 sorted
    int url_speed[32];
    int url_id[32];
    int url_ticks[32];
    URL_SPEED_STATE url_speed_state[32];
    SORT_TASK_STATE url_task_live[32];

} LIVE_TV_SORT_CHANNEL_T;

typedef struct {
    int index;
    LIVE_TV_SORT_CHANNEL_T * p_task_csc;
    void * stack_addr;
} LIVE_TV_SORT_TASK_T;
typedef enum {
    /*!
    xxxxxxxx
    */
    INFO_URL_SUCCESS,
    /*!
    xxxxxxxx
    */
    INFO_URL_NOT_FOUND,
    /*!
    xxxxxxxx
    */
    INFO_URL_MAX,
    /*!
    xxxxxxxx
    */


} INFO_URL_STATE;
typedef struct {
    int url_id;
    char * url;
    int url_type;
    int url_speed;
    u32 url_ticks;
    INFO_URL_STATE get_url_state;

} CLIENT_URL_INFO_T;

typedef struct {

    u32 task_array[9];
    int is_init;

    int channel_num;/*total channel*/
    LIVE_TV_SORT_CHANNEL_T * array;
    int current_program_index;
    int current_sort_step;
    int need_sort_current_range;
    int current_sort_index;
    int last_sort_index;
    int sort_cycle;
    SORT_TASK_STATE task_state;
    int task_stop;
    void * stack_addr;
    /*!
    lock for write back speed
    */
    unsigned int lock;
    CLIENT_URL_INFO_T * p_clienturlinfo;
    int extra_url_get;
} LIVE_TV_CLIENT_SORT_T;




typedef struct {
    int num;/*total channel*/
    LIVE_TV_CHANNEL_T * array;
    int pagecount;/*all channels in one page*/
    int totalcount;/*total channel*/
    TVLIVE_DATA_STATUS status;

    //being used when retriving data to up layer
    int category_id;
    int page_num;
    int page_size;

} LIVE_TV_CHANNEL_LIST_T;

typedef struct {
    void * zip_start_addr;
    u32 zip_length;
    TVLIVE_DATA_STATUS status;
    u8  fileName[64];
} LIVE_TV_CHANNEL_ZIP_T;



typedef enum {
    TVLIVE_EVENT_INVAILID,
    CATEGORY_ARRIVAL,
    ALL_PLAY_CHANNEL_ARRIVAL,
    PLAY_CHANNEL_ZIP_ARRIVAL,
    NEW_PAGE_ARRIVAL,
    PLAYBACK_INFO_ARRIVAL,
    EPG_INFO_ARRIVAL,

    PB_CHANNELS_NAME_ARRIVAL,
    PB_CHANNEL_INFO_ARRIVAL,
    EPG_CHANNEL_INFO_ARRIVAL,

    DATA_PROVIDER_INIT_SUCCESS ,   //////put the data provider init to a another task
    DATA_PROVIDER_INIT_FAIL ,
    EPG_CHANNEL_INFO_FAIL,
    IPTV2_ACTIVE_RESULT,
} TVLIVE_EVENT_TYPE;


typedef enum {
    /*!
    xxxxxxxx
    */
    CHANNEL_OPERATE_DELETE,
    /*!
    xxxxxxxx
    */
    CHANNEL_OPERATE_ADD,
  
} CHANNEL_OPERATE_METHOD;

/*
* TVLive playback structures
*/
typedef struct {
    char * na; /*program name*/
    char * st; /*start time*/
    char * et; /*end time*/
    int chpcount;/*total play urls*/
    char * url[256];/*play urls*/  /////fixme ,maybe the url count is bigger than 256
    char  * urlToPlayUrl;/*by this url we can got all the play urls*/
} PLAYBACK_PGMS_T;

typedef struct {
    char * id;/*channel id*/
    char * title;/*channel name*/

    int  pgms_num;/*total programs of one channel*/

    PLAYBACK_PGMS_T * pgms;

} PLAYBACK_CHANNEL_INFO_T;


typedef struct {
    int channel_number;/*total channels*/

    PLAYBACK_CHANNEL_INFO_T * channels;

    TVLIVE_DATA_STATUS status;
} LIVETV_PLAYBACK_INFO_T;


typedef enum {
    RADIO = 0,
    TV ,
    INVALID_CHANNAL_TYPE,
} CHANNEL_TYPE;


#define     SAT_NAME_LEN             (64)
#define     CHANNEL_TITLE_LEN     (128)
#define     ICON_URL_LEN              (128)


typedef struct {

    u32 vpid;
    u32 apid;
    u32 pcrpid;
    u32 symbolRate;
    u32 freq;
    char  p_title[CHANNEL_TITLE_LEN];
    char  icon_url[ICON_URL_LEN];
    char  polarity[8];
    int   id;
    CHANNEL_TYPE type;


} SAT_CHANNEL_NODE_T;



typedef struct {

    int   total_channel;
    int   xmlversion;
    char  satellite_name[SAT_NAME_LEN];
    SAT_CHANNEL_NODE_T * node_array;

} SAT_CHANNEL_LIST_T;

typedef enum {
  IPTV2_ACTIVE_SUCCESS,
  IPTV2_ACTIVE_WRONG_CODE,
  IPTV2_ACTIVE_EXPIRE_CODE,
  IPTV2_ACTIVE_NETWORK_ERROR,
  IPTV2_ACTIVE_UNKNOWN_ERROR,
} IPTV2_ACTIVE_RESULT_CODE;

typedef struct {
  int   active_result;
  int   ex_data;  			/*when active success value is days  to expaire*/
  char message[256];
} IPTV2_ACTIVE_RESULT_INFO;


typedef struct {

    u32  freq_list[64];
    int   total_freq;

} FREQ_LIST_T;


typedef struct {
    int id;/*channel id*/

    char * name; /*no use: reserved for future*/

    char * title; /*channel name*/

    char * keywords;/*a keywords relative to a channel name*/

    char * cname; /*no use: reserved for future*/

} LIVETV_PB_NAME_T;

typedef struct {
    int channel_number;

    LIVETV_PB_NAME_T * channels;

    TVLIVE_DATA_STATUS status;
} LIVETV_PB_NAMES_LIST_T;



/*
* Tvlive EPG structures
*/
typedef struct {
    char * na; /*program name*/
    char * st; /*start time*/
    char * et; /*end time*/

    //for sky epg
    char   *title;
    u32    unixtime;
    u32    timelen;
    char   *descriptin;
} EPG_PGMS_T;

typedef struct {
    int  cid;/*channel id*/
    char * title;/*channel name*/
    char * epg_id;
    char * item_id;

    //for sky epg
    int frequency;
    int serviceid;

    int  pgms_num;/*total programs in this channel*/

    EPG_PGMS_T * pgms;

} EPG_CHANNEL_INFO_T;



typedef struct {
    BOOL can_destroy;/*data weather or not be destroyed*/

    BOOL empty;/*data weather or not be empty*/

    int channel_number;/*total channels*/

    EPG_CHANNEL_INFO_T * channels; /*channel array*/

    char * date;

    TVLIVE_DATA_STATUS status;

} LIVETV_EPG_INFO_T;


typedef struct {
    /*
    * the date which you want
    */
    tvlive_date day;

} LIVETV_PB_NAMES_REQ;

typedef struct {
    /*
    *the date you want
    */
    tvlive_date day;

    /*
    *the date of today
    */
    tvlive_date today;

    /*
    *channel's id
    */
    int id;

    /*
    *channel's name
    */
    char name[TVLIVE_STRING_SHORT];

    /*
    *channel's title
    */
    char title[TVLIVE_STRING_SHORT];

    /*
    * channel's chinese name
    */
    char cname[TVLIVE_STRING_SHORT];

} LIVETV_PB_CHANNEL_INFO_REQ;


typedef struct {
    /*
    *the date you want
    */
    tvlive_date day;

    /*
    *the date of today
    */
    tvlive_date today;

    /*
    *channel's id
    */
    int id;

    /*
    *channel's name
    */
    char name[TVLIVE_STRING_SHORT];

    /*
    *channel's title
    */
    char title[TVLIVE_STRING_SHORT];

    /*
    * channel's chinese name
    */
    char cname[TVLIVE_STRING_SHORT];

    //for sky epg
    int frequency;
    int serviceid;

} LIVETV_EPG_CHANNEL_INFO_REQ;


typedef RET_CODE(*tvlive_event_callback)(TVLIVE_EVENT_TYPE event, u32 param);


typedef struct {

    LIVE_TV_CATEGORY_LIST_T  * category_list;
    LIVE_TV_CHANNEL_LIST_T   *  channel_list;
    LIVETV_PLAYBACK_INFO_T * playback_info;
    LIVETV_EPG_INFO_T * epg_info;
    LIVE_TV_CHANNEL_ZIP_T * channel_zip;
    LIVETV_PB_NAMES_LIST_T * pb_names_list;
    void    *   p_bg_task_hdl;
    IPTV_DATA_PROVIDER cur_dp_type;/*0:znd   1:bsw   2:mont  3:xgw */
    u32                            cur_sub_dp_type;
    register_event_cb_func_t    register_event_callback;
    tvlive_event_callback event_callback;
    BOOL                       is_init;/*whether this type of data provider has been initilized successfully*/
    void            *            third_dp;
    BOOL                       is_get_epg;

    /*
    *get the mac id from the UI
    */
    char * mac;
    char * id;
    char* active_code;

    char * citystr ;  /////add just for zgw , get the city info for msg getLiveList
} LIVE_TV_DP_HDL_T;






/***************************************************************************************/
/***************************************************************************************/
/*****************A Module For Getting Channel List of satellite from server **********************/
/***************************************************************************************/
/***************************************************************************************/
/**************************************************************************************/
/***************************************************************************************/


/*!
  xxxxxxxx
  */
/*!
define the type of event delived to app
*/
typedef enum {
    /*!
      the thread was parser success
     */
    SATELLITE_CHANNEL_PARSER_FINISH,
    /*!
      the thread was parser fail
    */
    SATELLITE_CHANNEL_PARSER_FAIL,
    /*!
      invalid type
      */
    SATELLITE_CHANNEL_PARSER_INVALID,
} SATELLITE_CHANNEL_EVENT_E;

typedef RET_CODE(*satelitteChannel_event_cb_fun_t)(SATELLITE_CHANNEL_EVENT_E, u32);

/*
*   FUNCTION:    get satellite channel list
*
*   RETURN VALUE:
*
*                            a handler for this module
*/
void  * LiveTV_CreateSatelliteChannels(int  prio, void * p_cb, u32 type);
/*
*
*
*   FUNCTION:   destroy all the internal data struture relative to this module
*
*
*/
void     LiveTV_DestroySatelliteChannels(void *);

/*
*
*  FUNCTION:   get xml version of this satellite
*
*  RETURN VALUE:  the version
*/
int  LiveTV_getXmlVersion(void  * hdl);
/*
*
*  FUNCTION:   get the satellite handle
*
*  RETURN VALUE:  the version
*/
void  * LiveTV_get_satellite_hdl();

/*
*
*  FUNCTION:   get SAT_CHANNEL_NODE_T by id
*
*  RETURN retuen the handle :
*/
SAT_CHANNEL_NODE_T * LiveTV_getChannelInfo_By_id(void  * p_hdl, int id);
/*
*
*  FUNCTION:   get frequence list of this satellite
*
*  RETURN VALUE:
*/
FREQ_LIST_T  * LiveTV_getFreqList(void  * hdl);
/*
*
*
*  FUNCTION:   get channel list by specified frequency
*
*/
SAT_CHANNEL_LIST_T * LiveTV_getChannelByFreq(void * p_hdl, u32 freq);


/****************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/****************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/




/*
*
*
*/
LIVE_TV_DP_HDL_T  * LiveTV_initDataProvider(DO_CMD_TASK_CONFIG_T * p_config);
/*
*
*
*/
int  LiveTV_deinitDataProvider(LIVE_TV_DP_HDL_T * hdl);
/*
*
*
*/
int   LiveTV_getLzmaChannelData(LIVE_TV_DP_HDL_T * hdl);
/*
*
*
*/
int   LiveTV_getCategoryList(LIVE_TV_DP_HDL_T * hdl);

/*
*
*
*/
int LiveTV_getChannelList(void * hdl, void * start, u32 len);
/*
*
* Return: by callback function,
*             callback message: EPG_CHANNEL_INFO_ARRIVAL
*             callback struct:  EPG_CHANNEL_INFO_T *
*/
int  LiveTV_getChannelEpgInfo(LIVE_TV_DP_HDL_T * hdl,
                              LIVETV_EPG_CHANNEL_INFO_REQ * req);
/*
*
* Return: by callback function,
*             callback message: PB_CHANNELS_NAME_ARRIVAL
*             callback struct:  LIVETV_PB_NAMES_LIST_T *
*/
int  LiveTV_getLookbackChannelNames(LIVE_TV_DP_HDL_T * hdl,
                                    LIVETV_PB_NAMES_REQ * req);

/*
*
* Return: by callback function,
*             callback message: PB_CHANNEL_INFO_ARRIVAL
*             callback struct:  PLAYBACK_CHANNEL_INFO_T *
*/
int  LiveTV_getLookbackChannelInfo(LIVE_TV_DP_HDL_T * hdl,
                                   LIVETV_PB_CHANNEL_INFO_REQ * req);

/*
*
* Return: by callback function,
*             callback message: PB_CHANNEL_INFO_ARRIVAL
*             callback struct:  PLAYBACK_CHANNEL_INFO_T *
*/
int  LiveTV_getLookbackPlayUrl(LIVE_TV_DP_HDL_T * hdl, char * prg_name);

/*
*
*
*/
void LiveTV_removeAllCmd(LIVE_TV_DP_HDL_T * hdl);


/*
*
* Return: 1 success 0 fail,
* if operate_method is CHANNEL_OPERATE_DELETE the para data is a int array ptr with last element's value is 0
* the operate_method is CHANNEL_OPERATE_ADD  is for Future expansion
*/
int  LiveTV_manageChannel(LIVE_TV_DP_HDL_T * hdl, int operate_method,void* data);


///just for zgw
/* not use any more
BOOL set_city_for_zgw(char * pcity)  ;

char * get_city_for_zgw()  ;
*/

LIVE_TV_CLIENT_SORT_T  * LiveTV_Client_Sort_Create();
//type =1 :force sort
char  * LiveTV_Client_Sort_Source(LIVE_TV_CLIENT_SORT_T * p_ccs, int channel_id, int type);
int  LiveTV_Client_Sort_Destroy(LIVE_TV_CLIENT_SORT_T * p_ccs);
CLIENT_URL_INFO_T  * LiveTV_Client_Get_Url_Info(char * url, int type);

#ifdef __cplusplus
}
#endif

#endif




