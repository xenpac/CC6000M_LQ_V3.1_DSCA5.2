/*
 * library function to send request and get response via http1.1
 */

#ifndef HTTPC_H
#define HTTPC_H

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_OK      0
#define HTTP_ERROR   1 /* Generic error; use ne_get_error(session) for message */
#define HTTP_LOOKUP  2 /* Server or proxy hostname lookup failed */
#define HTTP_AUTH    3 /* User authentication failed on server */
#define HTTP_PROXY_AUTH 4 /* User authentication failed on proxy */
#define HTTP_CONNECT 5 /* Could not connect to server */
#define HTTP_TIMEOUT 6 /* Connection timed out */
#define HTTP_FAILED  7 /* The precondition failed */
#define HTTP_RETRY   8 /* Retry request (ne_end_request ONLY) */
#define HTTP_FILE    100 /* file handling error */


  struct http_filesave_res {
    char *filename;  /* saved filename from http, (NULL if error) */
    char *httprsp;   /* http response in string */
    char *ctype;      /* type of content downloaded - ex. image/jpeg */
    unsigned short code; /* status of the request */
  };

  /* 
   * make a request and save the content in a file, may return NULL
   * url     - this is the http URL
   * dir     - which directory to save this file, can be NULL
   * filen   - filename to save the content, can not be NULL
   * 
   */ 
  extern struct http_filesave_res* http_req_savefile(char *url, 
						     char *dir,
						     char *filen);

  /*
   * make a post request, and save the content into a file, 
   * may return NULL
   * url    - this is the HTTP URL
   * dir    - optional, can be NULL, which dir to store response
   * filen  - mandatory, filename to save the response
   * postbuffer - the buffer that's to be posted to the webserver
   */
  extern struct http_filesave_res* http_post_savefile(char *url, 
						      char *dir, 
					       	      char *filen, 
					       	      const char* postbuffer);
						     

  /* free the structure allocated from http request */
  extern void http_free_file_res(struct http_filesave_res * res);
  

#ifdef __cplusplus
}
#endif
#endif
