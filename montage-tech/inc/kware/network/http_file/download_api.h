/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DOWNLOAD_FILE_API_H__
#define __DOWNLOAD_FILE_API_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOWNLOAD_HTTP_NEW
/*******************************************************************************
packet http old api, data structure
********************************************************************************/

/*!
  This part define mthttp_download_start return values
  */
enum{
 /*!
   Unknown error
   */
 MTHTTP_UNKNOWN_ERROR = -1,
 /*!
   invalid download parameter
   */
 MTHTTP_PARAM_INVALID = -2,
 /*!
   Download session timeout
   */
 MTHTTP_SESSION_TIMEOUT = -3,
 /*!
   Download socket create failed
   */
 MTHTTP_SOCK_CREATE_FAILED = -4,
 /*!
   Authentication failed
   */
 MTHTTP_AHTHEN_FAILED = -5,
 /*!
   Request or redirect protocol un-support
   */
 MTHTTP_PROTOCOL_UNSUPPORT = -6,
};
typedef struct HTTP_field_type {
        char *field_name;
        struct HTTP_field_type *next;
} HTTP_field_t;
typedef struct {
        char *protocol;
        char *method;
        char *uri;
        unsigned int status_code;
        char *reason_phrase;
        unsigned int http_minor_version;
        // Field variables
        HTTP_field_t *first_field;
        HTTP_field_t *last_field;
        unsigned int field_nb;
        char *field_search;
        HTTP_field_t *field_search_pos;
        // Body variables
        char *body;
        size_t body_size;
        char *buffer;
        size_t buffer_size;
        size_t content_length;
        unsigned int is_parsed;
        long long chunksize;
        unsigned int isChunked;
} HTTP_header_t;
/*!
  for special info setting
  */
typedef struct {
    /*!
      user specified cookie length
      */
    size_t cookie_len;
    /*!
      user specified cookie content
      */
    char *p_cookie;
    /*!
      user specified user agent string length
      */
     size_t user_agent_len;
     /*!
      user specified user agent content
      */
     char *p_user_agent;
    /*!
      user specified content type length
      */
     size_t content_type_len;
    /*!
      user specified content type 
      */
     char *p_content_type;
    /*!
      user specified X_Request
      */
     char *p_x_request;
    /*!
      user specified X_Request content length
      */
     size_t x_request_len;
    /*!
      user specified post string
     */
     char *p_post_value;
    /*
    post string length
   */
   size_t post_value_len; 
   /*
   timeout
   */
   long  timeout; 
}HTTP_spec_info_t;
/*******************************************************************************
packet http old api, data structure----------------------end
********************************************************************************/
#endif
typedef struct {
    char *key[4];
    char *value_buf[4];
    int value_buflen[4];
} HTTP_rsp_header_t;

typedef struct _HttpDownloadResult
{
    int status;////-2:error  -1:done  0:ok
    int readLen;
}HttpDownloadResult;
typedef struct _HttpDownloadHeader
{
    int Content_length;///<=0 means invalid ,we can not get this value
    const char* redirect_url;/// it is null if no redirect. otherwise it is the redirect url
}HttpDownloadHeader;
/*!
  isGetOrPost:   0-get   1-post
  timeoutSec:   connect timeout seconds
  header:         http header response
  return         NULL means error or unexpected url           others mean download handle
*/
void* Nw_Http_Download_Start(const char * url, BOOL isGetOrPost, const char * postbuffer, unsigned int bufferLen, 
    unsigned int timeoutSec, HttpDownloadHeader *header, char *extraheader);
/*!
  handle:      download handle
  response:   buffer to save data from downloader, which is malloc by uplayer
  buffersize:  response buffer size
  timeoutSec:  read timeout seconds
  return         -1 means error or unexpected url           others mean read length
*/
HttpDownloadResult Nw_Http_Download_Recv(void *handle, unsigned char *response, unsigned int buffersize);
/*!
  handle:     download handle
  return         
*/
void Nw_Http_Download_Stop(void *handle);

#ifdef DOWNLOAD_HTTP_NEW
int chunkhttp_download_common_start(char *p_url, HTTP_header_t **p_outbuf,  HTTP_spec_info_t *p_http_spec_info);
int chunkhttp_download_start(char *p_url, HTTP_header_t **p_outbuf);
int  chunkhttp_recv(int fd, char *response, unsigned int size, unsigned int isChunked);
void chunkhttp_close(int fd);
int mthttp_download_common_start(char *p_url, HTTP_header_t **p_outbuf, HTTP_spec_info_t *p_http_spec_info);
#endif

typedef struct _TestSpeedPara {
    unsigned char abort_flag; /// 0-not abort    1-abort
    unsigned char skipNonLiveStream; // 0- not skip   1-skip
    unsigned char skipNonMediaUrl;// 0-not skip    1-skip
    char *writeFileName;
} TestSpeedCtrl;

typedef  enum{

    TEST_SPEED_OK = -1,
    ERR_RESERVED = 0,
    ERR_INVALID_URL_BANQUANTISHI = 1,
    ERR_INVALID_URL_COPYRIGHT = 2,
    ERR_INVALID_URL_TAOBAO = 3,
    ERR_INVALID_URL_LETV_ERROR = 4,

    ERR_NO_NORMAL_LIVE_URL = 50,
    ERR_INVALID_LIVE_URL = 51,
    ERR_NEGATIVE = 52,
    ERR_NOT_FIND_VALID_URL = 53,

    ERR_INVALID_PARAM1 = 100,
    ERR_INVALID_TIMEOUT = 101,
    ERR_INVALID_DL_HDL = 102,
    ERR_TEST_SPEED_TIMEOUT = 103,
    ERR_DL_EXCEPTION = 104,
    ERR_ABORT_BY_USER = 105,

} TEST_SPEED_ERR_T;


typedef struct _Download_Result {
    int          speed;
    int          connectMS;
    int          downloadMS;
/*
  * errcode indicate download and test dl speed return error code;
  * -1. normal.
  * 0:  reserved
  * 1:  banquantishi string
  * 2:  copyrihgt  string
  * 3.  taobao.com string
  * 4.  final url is not ts/flv/mp4/live url
  * 
  * 50.  no-live
  * 51.  test speed is negative
  * 52.  cannot get final url
  * 53~ 99: reserved
  
  * 100.  Url or param is Null
  * 101.  timeout param is Error
  * 102.  new invalid dl param
  * 103.  test speed timeout 
  * 104.  download exception
  * 
  */
    TEST_SPEED_ERR_T          errcode;
} DownloadResult;

void   Nw_Download_Init_Download_Manager();
/*!
xxxxxxxx
*/
void   Nw_Download_Init(int task_prio, unsigned char * p_stack_mem, int size);
/*!
xxxxxxxx
*/
void   Nw_Download_Deinit(void);
/*!
  xxxxxxxx
*/
int     Nw_DownloadURLTimeout(const char * url,
                              const char * tempFile,
                              unsigned int timeoutSec,
                              void * response,
                              void * arg,
                              const char * extraHeaders,
                              const char  * body,
                              unsigned int bodyLen);

int      Nw_DownloadURLTimeout_Gzip(const char * url,
                          const char * tempFile,
                          unsigned int timeoutSec,
                          void * response,
                          void * arg,
                          const char * extraHeaders,
                          const char * body,
                          unsigned int bodyLen);

/*!
  xxxxxxxx
  return         -1 means error or unexpected url           others mean xxx kBps
*/
DownloadResult Nw_DownloadURL_TestSpeed(const char * url,
                             unsigned int timeoutMS, TestSpeedCtrl * para);
/*!
  xxxxxxxx
  yiyuan change this , not use anymore , use Nw_DownloadURL_POST_ex instand
*/
int  Nw_DownloadURL_POST(const char * url,
                          const char * postbuffer,
                          unsigned int bufferLen,
                          const char * tempFile,
                          const char * extraHeaders,
                          const char * checkHeader,
                          const char * serviceProvider) ;

int  Nw_DownloadURL_POST_Gzip(const char * url,
                          const char * postbuffer,
                          unsigned int bufferLen,
                          const char * tempFile,
                          const char * extraHeaders,
                          const char * checkHeader,
                             const char * serviceProvider,
                             unsigned int timeoutSec);

/*!
  xxxxxxxx
  add timeoutsec from Nw_DownloadURL_POST
*/
int  Nw_DownloadURL_POST_ex(const char * url,
                             const char * postbuffer,
                             unsigned int bufferLen,
                             const char * tempFile,
                             const char * extraHeaders,
                             const char * checkHeader,
                             const char * serviceProvider,
                             unsigned int timeoutSec) ;

/*!
  xxxxxxxx
*/
int  Nw_DownloadURL_POST_ex2(const char * url,
                             const char * postbuffer,
                             unsigned int bufferLen,
                             const char * tempFile,
                             const char * extraHeaders,
                             HTTP_rsp_header_t *rsp_header,
                             unsigned int timeoutSec);///seconds
/*!
  xxxxxxxx
*/
void  Nw_Download_SetAbortFlag(BOOL flag);

void Abort_Download_Task(int task_prio, BOOL abort_flag);

/*!
  xxxxxxxx
  Get download url from youtube-dl run in the server.
*/
#define WEBSITE_YOUTUBE        0
#define WEBSITE_DAILYMOTION    1
#define WEBSITE_CLIPSYNDICATE  2
#define WEBSITE_YAHOO          3
#define WEBSITE_ADDANIME       4
#define WEBSITE_YOUKU          5



//for Nw_Request_Website_DownloadURL
#define QEQUEST_VIDEO_LD 1
#define QEQUEST_VIDEO_SD  2
#define QEQUEST_VIDEO_HD   4

//for communicate to server
#define QEQUEST_VIDEO_QCIF 1
#define QEQUEST_VIDEO_CIF  2
#define QEQUEST_VIDEO_D1   4
#define QEQUEST_VIDEO_720P 8
#define QEQUEST_VIDEO_1080P 16
#define MAX_VIDEO_SIZE_LEN  5

#define  DOWNLOAD_URL_MAX_LEN   (2048)//same to PLAY_URL_MAX_LEN

typedef  struct {
	char     playUrlArray[MAX_VIDEO_SIZE_LEN][DOWNLOAD_URL_MAX_LEN];//from qcif to 1080P
} request_url_t;

//ERRORCODE
//-1 connet server failed
//-2 post url is invalid, given by server
//-3 unspport video size
//-4 wrong xml
//-5 fail to malloc
int Nw_Request_Website_DownloadURL(u8 website, const char * url, u8 size, request_url_t * p_request_url, u8 timeout);


#define   GET_URL_LIST_MAX_SIZE  (8*1024)
#define   GET_VIDEO_INFO_MAX_SIZE  (64*1024)
#define   VIDEO_INFO_BUF_LEN                  (256*1024)
#define   JS_INFO_BUF_LEN                  (1024*1024)
#define   SIG_STR_MAX_LEN                      (128)
#define   ITAG_STR_MAX_LEN                    (8)
#define   QUALITY_STR_MAX_LEN              (32)
#define   CODEC_TYPE_STR_MAX_LEN        (128)

#define    KW_URL                                   "url="
#define    KW_SIG                                    "\\u0026s="
#define    KW_ITAG                                  "itag="
#define    KW_QUALITY                           "quality="
#define    KW_TYPE                                  "type="

typedef  struct {

	int        val;
	char *   container;
	char *   resolution;
	char *   video_enc;
	char *   audio_enc;

} itag_detail_t;

#define   MAX_FILM_LIST_LEN                    (32)
typedef  struct {
    int       index;
    char *    buf[MAX_FILM_LIST_LEN];
    char      qualityArray[MAX_FILM_LIST_LEN][QUALITY_STR_MAX_LEN];
	int       itagArray[MAX_FILM_LIST_LEN];
	char      codecTypeArray[MAX_FILM_LIST_LEN][CODEC_TYPE_STR_MAX_LEN];
	char      sigArray[MAX_FILM_LIST_LEN][SIG_STR_MAX_LEN];
    char      playUrlArray[MAX_FILM_LIST_LEN][DOWNLOAD_URL_MAX_LEN];
}parse_youtube_vedio_info_t;

#define MAX_SEARCH_KEY_NUM  8
#define MAX_SEARCH_LEN  64
typedef  struct {
    //0/1 2/3 for p_video_info->buf start/end
    //4 KW_URL, 5 KW_SIG, 6 KW_ITAG
	char     keyArray[MAX_SEARCH_KEY_NUM][MAX_SEARCH_LEN];
} youtube_search_key_t;


int   youtube_parse_stream_map_list(const char * p_videoid, parse_youtube_vedio_info_t * p_video_info, char * urlenc_data, request_url_t * p_request_url, u8 size, youtube_search_key_t youtube_key);
void  resetParseYoutubeVideoInfoData(parse_youtube_vedio_info_t * p_info);

#ifdef __cplusplus
}
#endif

#endif
