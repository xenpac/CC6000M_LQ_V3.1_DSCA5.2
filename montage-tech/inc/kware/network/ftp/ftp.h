#ifndef __FTP_H__
#define __FTP_H__

#ifndef WIN32
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#endif


#define RET_FAILURE ((int)-1)
#define RET_SUCCESS ((int)0)
#define BUFFER_SIZE  4 * 1024 /* 4k buffer for file transfer */

#ifndef __ssize_t_defined
typedef int ssize_t;
#define __ssize_t_defined
#endif
/*!
  * None
  */
typedef struct{
    /*!
      * Username for remote FTP server access
      */
    char *username;
    /*!
      * Password for remote FTP server access
      */
    char *password; 
    /*!
      *  Socket information
      */
    struct sockaddr_in *host;
}ftp_host_info_t;

/*!
 * This function used to login remote FTP server and create the control 
 * stream for the FTP session.
 *
 * \param[in] Information about remote FTP server.
 * 
 * Return 
 *     File descriptor about FTP session control stream.
 */
int ftp_login(ftp_host_info_t *server);

/*!
 * Close FTP session
 * \param[in] Contorl session pointer
 */
void ftp_quit(int fd);

/*!
 * Retrive a segment of remote file from a data session 
 *
 * \param[in] Data session descriptor
 * \param[in] Local buffer which uses for store received contents.
 * \param[in] Buffer length to receive
 *
 * Return value: > 0 received buffer length, else error occur;
 */
int ftp_session_receive(int data_stream, char *buffer, int len);

/*!
 * Retrive a file from remote server to local buffer
 *
 * \param[in] Information about remote server
 * \param[in] Control session descriptor
 * \param[in] Local buffer which uses for store received contents.
 * \param[in] Destination path on remote host
 * \param[in] Received file length
 *
 * Return value 
 *  -  RET_FAILURE 
 *  -  RET_SUCCESS 
 */
int ftp_buf_receive(ftp_host_info_t *server, int control_stream, char *local_buffer, char *server_path, int *len);

/*!
 * Start retrive file from remote server to local 
 *
 * \param[in] Information about remote server
 * \param[in] Control session descriptor
 * \param[in] Destination path on remote host
 * \param[out] Remote file length
 *
 * Return value, >= 0 data session descriptor else  error occur;
 */
int ftp_open_receive_session(ftp_host_info_t *server, int control_stream, char *server_path, int *len);

/*!
 * Send buffer to a data session 
 *
 * \param[in] Data session descriptor
 * \param[in] Local buffer to send
 * \param[in] Buffer length to send
 *
 * Return value: > 0 send buffer length, else error occur;
 */
int ftp_session_send(int data_stream, char *buffer, int len);

/*!
 * Send local buffer to remote FTP server as a file
 *
 * \param[in] Information about remote server
 * \param[in] Control session descriptor
 * \param[in] Messages will be sent.
 * \param[in] Destination path on remote host
 * \param[in] Message length.
 *
 * Return value 
 *  -  RET_FAILURE 
 *  -  RET_SUCCESS 
 */
int ftp_buf_send(ftp_host_info_t *server, int control_stream, char *local_buffer, char *server_path, int len);

/*!
 * Start send file to remote server 
 *
 * \param[in] Information about remote server
 * \param[in] Control session descriptor
 * \param[in] Destination path on remote host
 *
 * Return value, >= 0 data session descriptor else  error occur;
 */
int ftp_open_send_session(ftp_host_info_t *server, int control_stream, char *server_path);

/*!
 * Close data session
 *
 * \param[in] Control session descriptor
 * \param[in] Data session descriptor
 *  
 * Return value 
 *  -  RET_FAILURE 
 *  -  RET_SUCCESS 
 */
int ftp_session_close(int control_stream, int data_session);

/*!
 * Get remote file size from remote FTP server 
 *
 * \param[in] Information about remote server
 * \param[in] Control session descriptor
 * \param[in] Destination path on remote host
 *  
 * Return
 *     Remote file size
 */
int get_remote_file_size(ftp_host_info_t *server, int control_stream, char *server_path);

/*!
 * Get host information from given URL
 *
 * \param[in] Remote host address
 * \param[in] Specified remote host port
 *  
 * Return
 *     Socket address pointer.
 */
struct sockaddr_in *get_host_info(const char *host, const unsigned int port);

#endif //__FTP_H__
