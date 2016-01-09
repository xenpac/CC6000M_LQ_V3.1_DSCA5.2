#ifndef __URL_H_
#define __URL_H_

/*!
  Service types
  */
typedef enum tag_url_services 
{
 /*!
  http service.
  */ 
  SERVICE_HTTP = 1,
 /*!
  ftp service.
  */ 
  SERVICE_FTP,
 /*!
  service end.
  */ 
  SERVICE_END,
}url_services;

/*!
  defines url structure.
  */
typedef struct _Url
{
 /*!
  full url name.
  */ 
  char *full_url;
 /*!
  url services.
  */
  url_services service_type;
 /*!
  user name
  */
  char *username;
 /*!
  pass word
  */
  char *password;
 /*!
  host name.
  */
  char *host;
 /*!
  port
  */
  int port;
 /*!
  file path
  */
  char *path;
 /*!
  file name
  */
  char *file;
 /*!
  out buffer address
  */
  u8 *p_out_buf;
 /*!
  buffer size.
  */
  u32 buf_size;
}Url;

#if 0
struct _UrlResource {
 /*!
  url structure.
  */
  Url *url;
 /*!
  output buffer
  */
  char *p_outbuf;      //(*outfile)
 /*!
  output buffer total size
  */ 
  off_t total_bufsize;        
  char *proxy;
  char *proxy_username;
  char *proxy_password;
	unsigned char options;
 /*!
  outbuf size
  */
  off_t outbuf_size;    //(outfile_size)
 /*!
  outbuf offset.
  */ 
  off_t outbuf_offset; //(outfile_offset)
};
#endif

Url *url_new(void);

Url *url_init(Url *u, char *string);

url_services url_get_service(Url *u);

void url_destroy(Url *u);

#endif /* URL_H */
	
