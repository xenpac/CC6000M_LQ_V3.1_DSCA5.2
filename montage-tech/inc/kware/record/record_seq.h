/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RECORD__SEQ__
#define __RECORD__SEQ__

/*!
  record_seq_notify
  */
typedef   int (* record_seq_notify)(int , void *);

/*!
  record_seq_msg_t
  */
typedef enum 
{
  /*!
  write file error
  */
  RECORD_SEQ_MSG_WRITE_FILE_ERROR
} record_seq_msg_t;

/*!
RECORD_PARAM_T
\param[in] param init parameter
*/
typedef struct
{
/*!
   thread priority
  */
  int thread_prio;

/*!
  record file path
  */
  char * file_path;

/*!
  camera path, such as "USB://camera0"
  */
  char * camera_path;

/*!
  record message
  */
  record_seq_notify notify;
}RECORD_PARAM_T;

 /*!
 RECORD_INIT_PARAM_T
 \param[in] param init parameter
 */
 typedef struct
 {
 }RECORD_INIT_PARAM_T;


 /*!
 record file tail
*/
#define RECORD_FILE_TAIL    ".musbc"

 /*!
 record init
 \param[in] param init parameter
*/
int record_init(RECORD_INIT_PARAM_T *param);

 /*!
 record deinit
 \param[in] param init parameter
*/
int record_deinit();

 /*!
 record start
 \param[in] param init parameter
*/
int record_start(RECORD_PARAM_T *param);

 /*!
  record stop
 */
int record_stop();
 
/*!
record_seq_camera_open
\param[in] name device name
*/
int record_seq_camera_open(char *name);
  
 /*!
 record_seq_camera_read
\param[in] fd 
\param[in] buffer
\param[in] buffer_len
*/
int record_seq_camera_read(int fd, char *buffer, int buffer_len);
   
/*!
record_seq_camera_close
\param[in] fd device file descripter
*/
  int record_seq_camera_close(int fd);


#endif
