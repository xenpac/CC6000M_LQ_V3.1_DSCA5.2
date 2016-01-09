#ifndef  __COMMON_DATA__
#define  __COMMON_DATA__
#ifdef __cplusplus
extern "C" {
#endif

#define POST_MSG_STR_LEN_MAX (1024)

    typedef struct {
        unsigned int pause: 1;         // for pausing torrent job
        unsigned int restart: 1;       // for restarting torrent job
        unsigned int del_file: 1;      // for delete torrent+file
        unsigned int play_picture: 1;  // for pictures
        unsigned int play_audio: 1;   // for music
        unsigned int download: 1;
        unsigned int play: 1;         // for movie files. It means the movie has been fully playable, not from streaming
        unsigned int preview: 1;      // for movie files. It means the movie is partially downloaded, or it can be streaming
        unsigned int subscribe: 1;
        unsigned int unsubscribe: 1;
        unsigned int remove: 1;
        unsigned int move: 1;
        unsigned int enter: 1;
        unsigned int Return: 1;
    } OPERATE_CAP;

    typedef enum {
        ACTION_DOWNLOAD,
        ACTION_PLAY,
        ACTION_PLAY_MUSIC,  // @FIXME, should merge with PLAY
        ACTION_PLAY_PICTURE,// @FIXME, should merge with PLAY
        ACTION_SUBSCRIBE,
        ACTION_UNSUBSCRIBE,
        ACTION_REMOVE,
        ACTION_ENTER,
        ACTION_MOVE,
        ACTION_RETURN,
        ACTION_PAUSE,        // for torrent pause
        ACTION_RESTART,      // for torrent restart
        ACTION_DELFILE,      // remove torrent + files
        ACTION_RELATE,       // @FIXME should be removed because not necessary to put inside canDo list
        ACTION_PREVIEW,      // watching content via http
        ACTION_DL_ADD,       // download torrent and add it
        ACTION_ADD_TO_LIST,  // add the item to list (favorate list)

        ACTION_PG_INFO,         // EPG: show program info
        ACTION_SCHEDULE_REC,    // EPG: schedule record
        ACTION_SCHEDULE_PLAY,   // EPG: schedule playback
        ACTION_EDIT_TIME_S,     // RPG: edit start time
        ACTION_EDIT_TIME_E,     // EPG: edit end time

        ACTION_JUMP_TO_URL,     // jump directory to another URL
        ACTION_FLAGGING,
        ACTION_RATE,            // rate this item.
        ACTION_ADD_TO_QUEUE,    //add the item to queue

        ACTION_NONE,

        ACTION_CUSTOM_CANDO_LIST, // this is the base index of canDoList, the first canDoList item is equal to this number
        // Do not put item after ACTION_CUSTOM_CANDO_LIST

    } ITEM_ACTION;


    typedef enum{

        INVALD_FEED_TYPE,
        VIDEO_FEED,
        STAND_FEED_TOP_POPULAR,
        STAND_FEED_TOP_FAVORITE,
        STAND_FEED_TOP_RATED,
        STAND_FEED_MOST_RECENT,
        STAND_FEED_MOST_RESPONDED,
	    SEARCH_FEED_TYPE,
    	YOUX_FEED_TYPE,//for youporn
	    REDTUBE_FEED_VIDEOS,//redtube  8
	    YOUTUBE_FEED_ALL,    //deinit all youtube data
        MAX_FEED_TYPE,

    } YT_FEED_TYPE;

    typedef enum{
        ALL_COUNTRY,/*all country*/
        AR,/*Argentina*/
        AU,/*Australia*/
        AT,/*Austria*/
        BE,/*Belgium*/
        BR,/*Belgium*/
        CA,/*Canada*/
        CL,/*Chile*/
        CO,/*Colombia*/
        CZ,/*Czech Republic*/
        EG,/*Egypt*/
        FR,/*France*/
        DE,/*Germany*/
        GB,/*Great Britain*/
        HK,/*Hong Kong*/
        HU,/*Hungary*/
        IN,/*INDIA*/
        IE,/*IRELAND*/
        IL,/*ISRAEL*/
        IT,/*ITALY*/
        JP,/*JAPAN*/
        JO,/*JORDAN*/
        MY,/*MALAYSIA*/
        MX,/*MEXICO*/
        MA,/*MOROCCO*/
        NL,/*NETHERLANDS*/
        NZ,/*NEW ZEALAND*/
        PE,/*PERU*/
        PH,/*PHILIPPINES*/
        PL,/*POLAND*/
        RU,/*RUSSIAN FEDERATION*/
        SA,/*SAUDI ARABIA*/
        SG,/*SINGAPORE*/
        ZA,/*SOUTH AFRICA*/
        KR,/*REPUBLIC OF KOREA*/
        ES,/*SPAIN*/
        SE,/*SWEDEN*/
        CH,/*CHINA*/
        TW,/*TAIWAN*/
        AE,/*UNITED EMIRATES*/
        US,/*UNITED STATES*/

        MAX_REG_ID,


    } YT_REGION_ID_T;

typedef enum{
  
  TODAY,
  THIS_WEEK,
  THIS_MONTH,
  ALL_TIME,
  
  }YT_TIME_T;

typedef enum{
      NULL_IPTV_DP,
      ZND_IPTV_DP,
      BSW_IPTV_DP,
      ZGW_IPTV_DP,
      MT_IPTV_DP,
      THIRD_IPTV_DP,
      DUMMY_IPTV_DP,
      SKY_IPTV_DP,
      INVALID_IPTV_DP,
      
}IPTV_DATA_PROVIDER;


    typedef  int (*   init_fun_t)(void *, YT_FEED_TYPE);
    typedef  int (*   deinit_fun_t)(void *, YT_FEED_TYPE);
    typedef int (* search_fun_t)(void *, char *);

    typedef  int (*    set_region_id_fun_t)(void *, YT_REGION_ID_T);
typedef  int                        (*    set_time_fun_t)(void *,YT_TIME_T);
    typedef  int (*    set_video_id_fun_t)(void *, char *);
    typedef  int (*    get_item_index_fun_t)(void *);
typedef  int                        (*    set_region_id         )(void * , YT_REGION_ID_T);
    typedef  int (*    get_category_index_fun_t)(void *);
    typedef  int (*   get_page_index_fun_t)(void *);
    typedef  int (*  get_next_page_fun_t)(void *);
    typedef  int (*  get_prev_page_fun_t)(void *);
    typedef  int (*  get_cur_page_fun_t)(void *);

    typedef  int (*    set_category_index_fun_t)(void *, int);
    typedef  int (*   set_item_index_fun_t)(void *, int);
      typedef  int (*   set_page_index_fun_t)(void *, int);
    typedef  int (*   set_max_category_cnt_fun_t)(void *, int);
    typedef  int (*   set_max_item_cnt_fun_t)(void *, int);
    typedef  int (*  get_video_play_url_fun_t)(void *);



    typedef  int (*   get_item_num_fun_t)(void *);
    typedef  int (*   get_category_num_fun_t)(void *);

    typedef  int (*   init_url_fun_t)(void *, char *);

    typedef  const char   *(*   get_item_info_fun_t)(void *, int /*idx*/, const char ** /*pImageFileName*/, int /*numOfImageFileName*/, OPERATE_CAP * /*canDo*/);
    typedef  const char   *(*   get_item_info_ex_fun_t)(void *, int /*index*/, const char * /*keyword*/, int /*indexForMultipleResults*/);

    typedef  int (*   on_item_action_fun_t)(void *, ITEM_ACTION);

    typedef   void (*  register_event_cb_func_t)(void *, void *);


typedef  int (*  do_cmd_fun_t)(void *,void *);

typedef   struct
{
   /*the three following fields is only for uc-cos, and on linux we don't need*/
   int  priority;
   unsigned char * p_mem_start;
   int   stack_size;
   int http_priority;
   int misc_priotiry;
   /**/
   do_cmd_fun_t   proc_cmd_callback;
   do_cmd_fun_t   event_cmd_callback;
   
   void * p_data_provider;

    IPTV_DATA_PROVIDER  dp;
	u32                         sub_dp;

    
    /*
    *used for iptv
    */
    char *mac;

    char *id;
    char* active_code;
	
    char *citystr;  ///just for zgw getLiveList	

    int (*read_flash)(void *start, int len);

    int (*write_flash)(void *start, int len);
}DO_CMD_TASK_CONFIG_T;


#define    DP_CMD_FIFO_LEN  (18)
typedef  struct 
{
	int  type;
	u32 param;
	char key[128];
       int  index;
       int  grp;

} DP_CMD_T;

typedef struct
{
	BOOL  enforce_stop ;
	BOOL  init_flag;
	DP_CMD_T cmd_fifo[DP_CMD_FIFO_LEN];
	DP_CMD_T cmd_fifo_backup[DP_CMD_FIFO_LEN];
	unsigned char cmd_num ;
	unsigned int cmd_fifo_lock ;
	int task_status;
	int task_prio;
	void * p_dataprovider;
      	do_cmd_fun_t   proc_cmd_cb;

	do_cmd_fun_t   event_cmd_cb;
	  
} PROC_CMD_HDL_T;

#define    MAX_KEYWORD_LEN     (128)
#define    MAX_CATEGORY_NUM     (64)
#define    CATEGORY_NAME_LEN    (128)
#define    MAX_ITEM_NUM_OF_RSS     (10)
#define    MAX_DEFAULT_ITEM_NUM     (10)
#define    ITEM_AUTHOR_LEN    (128)
#define    ITEM_DESCRIPTION_LEN    (1024)
#define    ITEM_TITLE_LEN    (128)
#define    DURATION_STR_LEN  (32)
#define    RATING_STR_LEN  (32)
#define    VIEWCOUNT_STR_LEN  (32)
#define     MAX_ITEM_NUM_PER_PAGE   (10)
#define     MAX_VIDEOID_LEN  (32)


#define  COMMON_DATA  \
   \
      char      catetory_name_arry[MAX_CATEGORY_NUM][CATEGORY_NAME_LEN]; \
      char      item_author[MAX_ITEM_NUM_OF_RSS][ITEM_AUTHOR_LEN];   \
      char      item_description_arry[MAX_ITEM_NUM_OF_RSS][ITEM_DESCRIPTION_LEN];   \
      char      item_title_arry[MAX_ITEM_NUM_OF_RSS][ITEM_TITLE_LEN];   \
      char      * pp_item_play_url[MAX_ITEM_NUM_OF_RSS];\
      char      key_word[512];\
      char      item_video_id[MAX_ITEM_NUM_OF_RSS][MAX_VIDEOID_LEN];\
      char      * pp_item_thumnail_urldef[MAX_ITEM_NUM_OF_RSS];\
      char      * pp_item_thumnail_urlmq[MAX_ITEM_NUM_OF_RSS];\
      char      * pp_item_thumnail_urlhq[MAX_ITEM_NUM_OF_RSS];\
      char      * pp_item_thumnail_urlsd[MAX_ITEM_NUM_OF_RSS];\
      char      * pp_item_thumnail_url1[MAX_ITEM_NUM_OF_RSS];\
      char      * pp_item_thumnail_url2[MAX_ITEM_NUM_OF_RSS];\
      char      * pp_item_thumnail_url3[MAX_ITEM_NUM_OF_RSS];\
      char      item_duration_str[MAX_ITEM_NUM_OF_RSS][DURATION_STR_LEN];\
      char      item_rating_average_str[MAX_ITEM_NUM_OF_RSS][RATING_STR_LEN];\
      char      item_viewcount_str[MAX_ITEM_NUM_OF_RSS][VIEWCOUNT_STR_LEN];\
      BOOL      is_init;\
     void *    p_xmlDoc;          \
 \
     char *    p_video_url; \
 \
     char *    p_video_id; \
\
     int        catetory_total;  /*real category number of curent page*/\
 \
     int        item_num;  /*real item number of curent page*/\
     int        item_num_lasttime;  /*real item numbe of last time request*/\
     int        item_num_total; /*total item num in current feed */\
     int        max_item_count; /*max item number per page*/\
     int        max_category_count;/*max category number per page*/\
  \
     int         cur_focus_catetory_index; \
     int         cur_focus_item_index;  \
     int         last_page_index;\
     int         cur_focus_page_index;\
     \
     YT_FEED_TYPE  cur_feed_type; \
     \
     YT_REGION_ID_T  region_id;\
     \
      YT_TIME_T            time_id;\
       \
 

    int MyEncrypt(const unsigned char *inputMsg, int inputStrlen, unsigned char *sEncryptMsg);
    int MyDecrypt(const unsigned char *inputMsg, int inputStrlen, unsigned char *sDecryptMsg);
    int                DP_init(void * p_handle);
    int                DP_deinit(void * p_handle);
    int                DP_initXml(void * p_handle);
    void *            DP_createXmlDoc(char * p_xmlPath);
    int                 DP_initUrl(void * p_handle, char * p_url);

/*
*
*
*     FUNCTION:   start command processing engine/task/thread
*     PARAM:
*               PARAM1:  the specified callback function response to special data provider
*                             
*               PARAM2:  param for creating a task,such as, priority,stack memory,stack size
*
*    RTURN VALUE:
*                     return one handle
*
*****/
void *    DP_startDoCmdTask(DO_CMD_TASK_CONFIG_T param);
/*
*
*
*     FUNCTION:   destroy command processing engine/task/thread
*     PARAM:
*               PARAM1:  on uc-cos, it refer to task priority
*                             on linux, it refer to thread id
*               PARAM2:  instance for process command
*
*    RTURN VALUE:
*                     no
*
*****/
void   DP_stopDoCmdTask(void * hdl);

/*
*
*  FUNCTION:   data provider send command to command center
*
*  PARAM:  
*              param1:  instance for process command
*
*              param2:  command to be sent
*
*  RTURN VALUE:
*               0:  means success
*              -1:  means fail
*                  
*/
int  DP_sendCmd(void * hdl, DP_CMD_T* p_cmd);


/*
*
*  FUNCTION:   clear internal command fifo
*

*
*  RTURN VALUE:

*                  
*/
void  DP_clearCmdFifo(void * hdl);
	

///used for get the sub url list
typedef struct
{
	/// init it is 0 , while enter parser , it is 1 , while parser finish , it is 2 ,
	/// while error , it is 3 
	unsigned char is_init ; 
	char* version ;

	char* liveurl ; ///for live 
	char* vodurl ; ///vod 
	char* epgurl ; ////get epg info for live
	char* backurl ;////get back play url for live 
	char* updateurl ; ///the url for update , it is less right now 
	char* logservice ; ////used for collect the custom info , not use right now ;
	char* adverurl ; ////used for advertisement , not use right now 
	char* backrooturl;  ///backplay root url
}BSW_TYPE_SERVER_URL ;

/////yiyuan add this , for avoid the redefine  the function by extern 
BSW_TYPE_SERVER_URL* get_server_url_hdl(u32 type,char* req_mac, char* req_id) ;  


#define MAIN_URL_NUMBER_MAX 7			///the max is 6  
typedef struct
{
	u32 count ;
	char* array[MAIN_URL_NUMBER_MAX] ;
	int encrypt_strlen[MAIN_URL_NUMBER_MAX];
}MAIN_URL_LIST_T ;
/*
*
*  FUNCTION:   add api for set the main server url address
*
*  PARAM:  
*              param1:  a MAIN_URL_LIST_T which will be setted to main server url ! 
*
*
*  RTURN VALUE:
*               0:  means success
*              -1:  means fail
*        
*/
BOOL set_the_main_server_address(MAIN_URL_LIST_T* urlinfo) ;

////add a strdup for ott , just use this in all prj case 
char* ott_strdup(const char *s);


#define DEFAULT_CA_STR  "20141229135400"
#define DEFAULT_MAC  "0090E6092734"
#define DEFAULT_SNID  "123456"
#define DEFAULT_HW_ID  1
#define DEFAULT_SW_ID  0
#define DEFAULT_CUSTOM_ID  100
#define DEFAULT_CITY  "jiangsu"
#define DEFAULT_CHIPID   12345678
#define DEFAULT_CHIPID_STR   "12345678"



//////add for active check 
typedef enum
{
	/*!
	the thread was parser success 
	*/
	DO_ACTIVE_PARSER_SUCCESS,
	/*!
	the thread was parser fail 
	*/
	DO_ACTIVE_PARSER_FAIL,
	/*!
	the thread was running right now
	*/
	DO_ACTIVE_PARSERING,
	/*!
	invalid type
	*/   
	DO_ACTIVE_PARSER_INVALID,
}DO_ACTIVE_EVENT_E;
typedef enum{
	DO_ACTIVE_SUCCESS =  0 ,
	DO_ACTIVED =  1 ,
	MAC_SN_NOT_MATCH =  2 ,
	SN_IS_INVALID =  3 ,
	UNDEFINE_ERROR =  4 ,
} DO_ACTIVE_MSG_T;
typedef int  (*do_active_event_cb_fun_t)(DO_ACTIVE_EVENT_E, DO_ACTIVE_MSG_T);
typedef   struct
{
	/*the three following fields is only for uc-cos, and on linux we don't need*/
	int  priority;	///the thread prio
	unsigned char * p_mem_start;
	int   stack_size;
	int http_priority;//////http download 

	do_active_event_cb_fun_t   proc_cmd_callback;

	IPTV_DATA_PROVIDER  dp;
	u32 	sub_dp;

	/*
	*used for iptv
	*/
	char *mac;

	char *id;

	BOOL encrypt_flag ;////// =0 means not encrypt , default is 0

/*
	char* username ; 
	char* password ;
*/
}DO_ACTIVE_TASK_CONFIG_T;
BOOL active_the_stb_box(DO_ACTIVE_TASK_CONFIG_T* p_param) ;


//////add for get upgrade url 
typedef enum
{
	/*!
	the thread was parser success 
	*/
	UPGRADE_INFO_PARSER_SUCCESS,
	/*!
	the thread was parser fail 
	*/
	UPGRADE_INFO_PARSER_FAIL,
	/*!
	the thread was running right now
	*/
	UPGRADE_INFO_PARSERING,
	/*!
	invalid type
	*/   
	UPGRADE_INFO_PARSER_INVALID,
}UPGRADE_INFO_EVENT_E;

typedef int  (*upgrade_info_event_cb_fun_t)(UPGRADE_INFO_EVENT_E, u32);///u32 is the urladdress while success ; is null while fail

typedef   struct
{
	/*the three following fields is only for uc-cos, and on linux we don't need*/
	int  priority;	///the thread prio
	unsigned char * p_mem_start;
	int   stack_size;

	int http_priority;//////http download 

	upgrade_info_event_cb_fun_t   proc_cmd_callback;

	IPTV_DATA_PROVIDER  dp;
	u32 	sub_dp;

	char *mac;
	char *id;
}UPGRADE_INFO_TASK_CONFIG_T;
BOOL get_upgrade_url_hdl(UPGRADE_INFO_TASK_CONFIG_T* p_param);




///////adv info +++++++++++++
/*!
  xxxxxxxx
  */
  /*!
  define the type of event delived to app
  */
typedef enum
{
 /*!
   	the thread was parser success 
  */
  ADV_DETAIL_PARSER_FINISH,
 /*!
   the thread was parser fail 
*/
	ADV_DETAIL_PARSER_FAIL,
 /*!
   the thread was running right now
*/
	ADV_DETAIL_PARSERING,
/*!
  invalid type
  */   
   ADV_DETAIL_PARSER_INVALID,
}ADV_DETAIL_EVENT_E;

void  get_adv_detail_destory(void* p_hdl);

typedef RET_CODE  (*adv_detail_event_cb_fun_t)(ADV_DETAIL_EVENT_E, u32);

typedef  struct{
	adv_detail_event_cb_fun_t  parser_event_cb ;	
	u32 prio_id ;

	IPTV_DATA_PROVIDER  dp;
	u32 	sub_type;

	char* mac ;
	char* snid ;
}ADV_DETAIL_TASK_CONFIG_T ;


#define MAX_LOGO_NUM  10 
typedef  struct{

	int  img_num;/*total of advertisement picture*/
	char *  img_urls[MAX_LOGO_NUM];/*url to picture*/

	int  title_num;
	char * title_array[MAX_LOGO_NUM];

	///for thread 
	adv_detail_event_cb_fun_t  parser_event_cb ;	
	u32 prio_id ;
	u32* p_stack ;

	IPTV_DATA_PROVIDER  dp;
	u32 sub_type ;

	BOOL is_init;/*whether this type of data provider has been initilized successfully*/

	char* mac ;
	char* snid ;
}ADV_DETAIL_T;
ADV_DETAIL_T  *  get_adv_detail(int  prio,void* p_cb, u32 type);  ///add a type to get adv url for different custom server

ADV_DETAIL_T  *get_adv_detail_bsw(ADV_DETAIL_TASK_CONFIG_T* config) ;
///////adv info ----------------


//////add for get the backup main url ++++
typedef enum
{
	/*!
	the thread was parser success 
	*/
	GET_MAIN_URLLIST_PARSER_SUCCESS,
	/*!
	the thread was parser fail 
	*/
	GET_MAIN_URLLIST_PARSER_FAIL,
	/*!
	the thread was running right now
	*/
	GET_MAIN_URLLIST_PARSERING,
	/*!
	invalid type
	*/   
	GET_MAIN_URLLIST_PARSER_INVALID,
}GET_MAIN_URLLIST_EVENT_E;
typedef int  (*get_main_urllist_event_cb_fun_t)(GET_MAIN_URLLIST_EVENT_E, u32 param);
typedef   struct
{
	/*the three following fields is only for uc-cos, and on linux we don't need*/
	int  priority;	///the thread prio
	unsigned char * p_mem_start;
	int   stack_size;
	int http_priority;//////http download 

	get_main_urllist_event_cb_fun_t   proc_cmd_callback;

	IPTV_DATA_PROVIDER  dp;
	u32 	sub_dp;

	/*
	*used for iptv
	*/
	char *mac;

	char *id;
}GET_MAIN_URLLIST_TASK_CONFIG_T;
BOOL get_the_main_url_list(GET_MAIN_URLLIST_TASK_CONFIG_T* p_param) ;
//////add for get the backup main url ----


////yiyuan add this , and it will open very soon 
typedef enum
{
	/*!
	parser success 
	*/
	PARSER_SUCCESS,
	/*!
	parser fail 
	*/
	PARSER_FAIL,
	/*!
	running right now
	*/
	PARSERING,
	/*!
	invalid type
	*/   
	PARSER_INVALID,
}XML_PARSER_EVENT_E;
typedef int  (*xml_parser_event_cb_fun_t)(XML_PARSER_EVENT_E, u32 param);
BOOL set_the_xml_parser(xml_parser_event_cb_fun_t  hdl);
typedef struct
{
	u32 value ;
}DP_INFO_T ;
BOOL set_the_dp_info(DP_INFO_T* hdl);


/////add for get the 
typedef struct
{
	BOOL is_init ; 
	u32 custom_id ;
	u32 hw_version;
	u32 serialno ;	
	u32 sw_version;

	char *CAinfor_str ;
	
	char *mac ;	

	char *chipid ;		////hal_get_chip_unique_numb can get the chidid , but set this for test , while in prj change the chipid 

	char* cpuinfo ; 	///add for define the cpu info 
	char* platforminfo ; ///useless right now , 20150426, just set null for it !
}BOX_INFO_T ;
BOOL set_the_box_hw_info(BOX_INFO_T* hdl);

/////
#define PRIORITY_NUMBER_MAX 10
typedef struct
{
	u16 count ; 
	int array[PRIORITY_NUMBER_MAX] ;
}PRIOLIST_T ;
BOOL set_priority(PRIOLIST_T* p_hdl);

int delete_tmp_file(char * path);

int url_encode(const char *src, char *dest); //size of dest must have min size of  3*strlen(src) +1
int url_decode(const char *src, char *dest); //size of dest must have min size of  strlen(src) +1
void debug_mem_leakage(char* funcname,int linevalue) ;

char * comm_parse_get_inner_string(char *string, char *front, char *end, char **new_start);
int comm_delete_amp_from_urls(char **url);
char * parse_html_to_get_inner_string(char * string, char * front, char * end, char ** new_start);


//Define the white and black list when download speed test
typedef struct {
    char* value;
    int errorcode;
} WHITE_BLACK_LIST_INFO;

typedef struct {
    WHITE_BLACK_LIST_INFO* white_list;
    WHITE_BLACK_LIST_INFO* black_list;

    int white_list_count;
    int black_list_count;
} TEST_SPEED_WHITE_BLACK_LIST;

TEST_SPEED_WHITE_BLACK_LIST  * get_live_speed_list_xml_handle();
void destory_live_speed_list_xml_handle();
void parse_url_white_black_list();

#define    MAX_POST_COUNT  	(4)
#define    DEFAULT_DL_HTML_TIMEOUT  15

////add api for usb debug 
char* get_sort_server_ip();
int get_sort_server_port();
char* get_sort_upgrade_url();
int get_sort_oversea_ltimer();
int get_sort_oversea_btimer();
int get_sort_oversea_bsize();
char* get_root_url() ;
char* get_addanime_cfg_info_url() ;
char* get_addanime_req_fast_url();
char* get_satellite_cfg_url() ;
char* get_netmedia_cfg_url() ;
char* get_abroad_live_cfg_url() ;
char* get_test_speed_list_url() ;
char* get_main_iptv_url();
char* get_adv_url();
char* get_net_speed_test_url();
char* get_sky_cfg_url();
int get_the_base_url(char* out, char* p_src) ;

#if  defined(USE_SCRIPT_FUNCTION)
char * parse_scriptinfo_get_inner_string(char * string, char * front, char * end, char ** new_start);
#endif



//////add for get the resource backup server address ++++
#define CONNECT_TIME_OUT_FAIL_TIME  (10*(100*10))  ///10s , 10*100 tickets , 
#define MAIN_RSC_URL_NUMBER_MAX 10
typedef struct {
	char* p_url ; 
	u32 connect_time ;
} ADDRESS_DETAILS_INFO ;
typedef struct
{
	int count  ; 
	ADDRESS_DETAILS_INFO array[MAIN_RSC_URL_NUMBER_MAX];
}MAIN_SRC_URL_LIST_T ;
typedef enum
{
	/*!
	the main url was parser success 
	*/
	GET_MAIN_RSC_URLLIST_PARSER_SUCCESS,
	/*!
	the main rsc parser fail 
	*/
	GET_MAIN_RSC_URLLIST_PARSER_FAIL,
	/*!
	the main rsc was running right now
	*/
	GET_MAIN_RSC_URLLIST_PARSERING,
	/*!
	invalid type
	*/   
	GET_MAIN_RSC_URLLIST_PARSER_INVALID,
}MAIN_RSC_URLLIST_EVENT_E;
typedef int  (*get_main_rsc_urllist_event_cb_fun_t)(MAIN_RSC_URLLIST_EVENT_E,void* url_addr,u16 str_len);
typedef   struct
{
	int	task_priority;

	get_main_rsc_urllist_event_cb_fun_t   proc_cmd_callback;

	IPTV_DATA_PROVIDER  dp;
	u32                         sub_dp;

}DO_INIT_CMD_TASK_CONFIG_T;
BOOL set_the_resource_server_address(void* url_addr,u16 str_len);
u32  init_resource_task(DO_INIT_CMD_TASK_CONFIG_T* p_param) ;

#ifdef __cplusplus
}
#endif
#endif



