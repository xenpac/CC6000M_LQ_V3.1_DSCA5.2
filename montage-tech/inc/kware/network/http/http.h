/* -*- mode: C; c-basic-offset: 8; indent-tabs-mode: nil; tab-width: 8 -*- */
#ifndef __HTTP_H
#define __HTTP_H
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
   Request or redirect URL un-support
   */
 MTHTTP_URL_UNSUPPORT = -6,
};
/*!
  temp solution
  */
typedef struct MTHTTP_field_type {
/*!
  temp solution
  */
	char *field_name;
/*!
  temp solution
  */
	struct HTTP_field_type *next;
} HTTP_field_t;

typedef struct {
/*!
  temp solution
  */
	char *protocol;
/*!
  temp solution
  */
	char *method;
/*!
  temp solution
  */
	char *uri;
/*!
  temp solution
  */
	unsigned int status_code;
/*!
  temp solution
  */
	char *reason_phrase;
/*!
  temp solution
  */
	unsigned int http_minor_version;
/*!
  temp solution
  */
	// Field variables
	HTTP_field_t *first_field;
/*!
  temp solution
  */
	HTTP_field_t *last_field;
/*!
  temp solution
  */
	unsigned int field_nb;
/*!
  temp solution
  */
	char *field_search;
/*!
  temp solution
  */
	HTTP_field_t *field_search_pos;
	// Body variables
/*!
  temp solution
  */
	char *body;
/*!
  temp solution
  */
	size_t body_size;
/*!
  temp solution
  */
	char *buffer;
/*!
  temp solution
  */
	size_t buffer_size;
/*!
  the HTTP content length
  */
    size_t content_length;
/*!
  temp solution
  */
	unsigned int is_parsed;
/*!
  chunksize
  */
   long long chunksize;

/*!
  http chunk
  */
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
   long timeout;
}HTTP_spec_info_t;
/*!
  for http proxy
  */
typedef struct {
    /*
   http proxy server
   */
    char *hostname;
    /*
   http proxy port
   */
    unsigned int port;
    /*
   http proxy username
   */
    char *username;
    /*
   http proxy password
   */
    char *password;
} HTTP_proxy_t;

/*!
  temp solution
  */
void		mthttp_http_free( HTTP_header_t *http_hdr );
/*!
  temp solution
  */
int mthttp_download_start(char *p_url, HTTP_header_t **p_outbuf, unsigned int flag);
/*!
 http get serivce data.
 \param[in] u : transfer one url structure type.
 */
BOOL http_transfer(Url *u);

int chunkhttp_download_start(char *p_url, HTTP_header_t **p_outbuf);

int  chunkhttp_recv(int fd, char *response, unsigned int size, unsigned int isChunked);

void chunkhttp_close(int fd);

int chunkhttp_download_common_start(char *p_url, HTTP_header_t **p_outbuf,  HTTP_spec_info_t *spec_info);

int mthttp_download_common_start(char *p_url, HTTP_header_t **p_outbuf, HTTP_spec_info_t *spec_info);

void *mthttp_proxy_config(HTTP_proxy_t *proxy);

void *mthttp_proxy_get(void);
#endif /* HTTP_H */

