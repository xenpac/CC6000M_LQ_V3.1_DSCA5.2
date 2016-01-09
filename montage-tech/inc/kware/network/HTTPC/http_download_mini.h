#ifndef __HTTP_DOWNLOAD_MINI_H__
#define __HTTP_DOWNLOAD_MINI_H__

#include <fcntl.h>
#include <stddef.h>     /* for NULL */
#include <string.h>
#include <stdlib.h>     /* for exit() */
#include <unistd.h>
#ifdef __LINUX__
#include <dirent.h>
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <pthread.h>
#include "eventSink.h"
#include <http_download.h>
#include <download_api.h>

typedef enum http_mini_state_ {
     HTTP_MINI_COMPLETE,
     HTTP_MINI_ERR,
     HTTP_MINI_TIMEOUT,
     HTTP_MINI_ABORT,
     HTTP_MINI_ABORT_BY_USER,
} http_mini_state;
typedef struct _http_download_mini_speed {
    http_mini_state        state;
    int          size;
    int          connectMS;
    int          downloadMS;
} HttpDownloadMiniSpeed;

class http_download_mini
{
private:
    http_state state;
    void * contex;

    void  downloader(unsigned int timeoutMS, unsigned char * abort_flag);

    void clean_data(void);
public:
    http_download_mini(void);
    ~http_download_mini(void);


    int http_connect(char * url, unsigned int timeoutMS, 
        bool isPost, const char * extraHeaders, const char * body, unsigned int bodyLen);
    int http_receive(unsigned char *buffer, unsigned int bufferlen, int *writelen);
    void http_close();
    void abort();
    /*
    * download this url, return download length:byte
    */
    HttpDownloadMiniSpeed download(char * url, unsigned int timeoutMS, char * writeFileName, unsigned char * abort_flag,
                                 bool isPost, const char * extraHeaders, const char * body, unsigned int bodyLen, HTTP_rsp_header_t *rsp_header = NULL);


    /*
    * get direct ts url ,which can play directly
    */
    const char * get_direct_ts_url_from_m3u8(bool *isLive);

    /*
    * get redirect url 
    */
    const char * get_redirect_url();
    
    /*
    * get final url if it has been redirected
    */
    const char * get_final_url();


    const char* get_ContentType_from_response();

};

#endif
