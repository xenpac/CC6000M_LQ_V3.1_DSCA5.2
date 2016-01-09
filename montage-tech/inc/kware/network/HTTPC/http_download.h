#ifndef __HTTP_DOWNLOAD_H__
#define __HTTP_DOWNLOAD_H__

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

#define HTTP_NO_FLAG             0x00000000

/*
 * setting NO_AUTOREDIRECT, and the thread will no follow a redirect link.
 * When redirect is encountered, it will be marked as complete
 */
#define HTTP_NO_AUTOREDIRECT     0x00000001

/*
 * setting AUTO_CLEANUP, and the state will automatically be cleaned up 
 * after the download job is done (either completed successfully, or 
 * aborted).
 */
#define HTTP_AUTO_CLEANUP        0x00000002

/*
 * HEAD command, No download, just check the header 
 */
#define HTTP_HEAD_ONLY           0x00000004

/*
 * If this flag is set, http download will use GFile to store
 * file. This flag should be used for large media file.  
 */ 
#define HTTP_MEDIA_GFILE         0x00000010

/*
 * doing HTTP POST. put postbody (form-urlencoded) in extraheader.
 */
#define HTTP_POST_METHOD         0x00000020

typedef enum http_state_ {
     HTTP_ERR,
     HTTP_INIT,
     HTTP_RECYCLE,
     HTTP_START,
     HTTP_CONNECTED,
     HTTP_DOWNLOADING,
     HTTP_COMPLETE,
     HTTP_TIMESOUT,
     HTTP_ABORTED,
     HTTP_ABORTED_BY_USER
} http_state;

typedef enum http_response_ {
     HTTP_INVALID,
     HTTP_DL_OK,
     HTTP_REDIRECT,/* redirect */
     HTTP_CONNECT, /* connect error */
     HTTP_TIMEOUT, /* http timed out */
     HTTP_ERROR
} http_response;

#define HTTP_N_THREADS      1

class http_download {
   private:
     http_state state;
     void * contex[HTTP_N_THREADS];

	 //peacer add 20130913
	 //only for uc-os
	 int     m_task_prio;   /*priority of download task*/
	 unsigned char * m_stack_start;  /*start address of download task stack*/
	 int m_stack_size;
	 unsigned char      m_use_ext_heap; /*stack of download allocated by upper layer*/

#ifdef __LINUX__
     pthread_t thread_id[HTTP_N_THREADS];
#endif

    // pthread_mutex_t c_mutex;
    u32 c_mutex;//peacer add
	 
     unsigned short descriptor;

     static void  downloader(void *arg);

     int lockin_slot(void);
     int mdownload_next(void *dld);
     void clean_data(void);
   public:
       http_download(void);
	 http_download(int task_prio, unsigned char  * p_stack_mem,int size) ;
     ~http_download(void);


     static http_download* get_http_manager(void);
     static http_download* get_http_manager(int prio,unsigned char *p_mem,int size);
     static http_download* clear_http_manager(void);
     /*
      * before formatting HDD, partition cannot umount if there
      * are still open files.  call this functions to shut 
      * http download manager down before umount its partition.
      */
     void prepareUnmount(void);


     /* 
      * download this url, and save the file into the 
      * specified dir/file. return descriptor (>= 0), or 
      * < 0 if error.
      */
     int download(char * url, char* dir, char *filen, unsigned long flag,
		  void * response=NULL, void *arg=NULL,
		  const char *extraHeaders=NULL, const char *body=NULL,
		  unsigned int bodyLen=0);
     
     /*
      * download a list of files.  The list of URLs are in the first
      * file. The list of saved files are in the second file. URL
      * and saved file must match in count.
      * This is strictly for downloading, do NOT use flags like
      * HEADONLY, or NO_AUDOREDIRECT
      */
     int download_multi(const char * urlFile, const char* saveFile,
                  unsigned long flag, IDownloadEventSink* response=NULL,
                  void *arg=NULL);
     
     /* 
      * wait msec for completion of download task, return 0 if  
      * download task is done during wait.  if msec=0, wait 
      * until download finished. 
      */
     int wait(int fd, unsigned int msec);

     /* is this object currently busy with downloading? */
     int busy(int fd);

     /*
      * get status of the current download job
      */
     http_state get_status(int fd);

     /* 
      *	get the error string (in case there is error)
      */
     const char * get_errString(int fd);

     /* 
      * check how many bytes have been downloaded 
      */
     ssize_t get_downloaded(int fd);

     /* 
      * check how many bytes total are for this download,
      * the result may be 0(unknown), the number may change
      * depending on when "content-length" is received.
      */
     ssize_t get_content_len(int fd);

     /* abort current download job */
     void abort(int fd, int cleanup = 0);

     /* clean up the download job, must be called after the job
      * is already done (completed or aborted ). return 0, if
      * success, less than 0 otherwise.
      */
     int finish(int fd);

     /* 
      * query the return code of finished http download, 
      * only call when a download job is done.
      */ 
     http_response get_http_code(int fd);

     /*
      * if return code is redirect, get redirct url 
      */
     const char * get_redirect_url(int fd);

     /* 
      * get the content-type of downloaded job
      */
     const char * get_content_type(int fd);

     /*
      * given an URL, tell me if this URL is in the process of
      * downloading. does not work with multi-download
      */
     int get_descriptor_fromURL(const char * URL);
};

#endif
