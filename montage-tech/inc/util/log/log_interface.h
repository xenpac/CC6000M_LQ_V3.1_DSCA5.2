/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_INTERFACE_H__
#define __LOG_INTERFACE_H__


 /*!
   init
 */
void logger_init(void);
/*
  log interface
*/

extern void _log_perf(log_module_id_t mid,
                  const char *p_file_name, u32 line,
                  const char *p_func, u32 event, u32 p1, u32 p2);

/*
log send one key
*/
extern void _send_one_key(void);

//inline void log(log_level_id_t lid,char *p_data);
 /*!
   inline LOG_PERF
 */
static inline void log_perf(log_module_id_t mid,  u32 event, u32 p1, u32 p2)
{
  _log_perf(mid, __FILE__, __LINE__, __FUNCTION__, event, p1, p2);
}

/*!
inline send one key
*/
static inline void send_one_key(void)
{
  _send_one_key();
}
#endif // End for __LOG_INTERFACE_H__

